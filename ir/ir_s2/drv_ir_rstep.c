#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/poll.h>
//#include <mach/hardware.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/spinlock.h>

#include "hi_drv_ir.h"
#include "drv_ir_protocol.h"
#include "drv_ir_utils.h"

#define RSTEP_NBITS           18
#define CHECK_RSTEPX_NBITS    8
#define RSTEP_UNIT            315 /* us */
#define RSTEP_HEADER_PULSE    (2 * RSTEP_UNIT)

#define RSTEP_BIT_START       (1 * RSTEP_UNIT)
#define RSTEP_BIT_END         (1 * RSTEP_UNIT + 1)

typedef enum {
    STATE_INACTIVE,           /* 0 */
    STATE_BIT_START,
    STATE_BIT_END,
    STATE_CHECK_RSTEPX,       /* 3 */
    STATE_FINISHED,           /* 4 */
} rstep_state;

typedef struct {
    rstep_state state;
    hi_u64      bits;
    hi_u64      scancode;
    hi_s32      count;
    key_attr    this_key;
    key_attr    last_key;
    hi_s32      has_last_key;
    ir_signal   prev_signal;
} rstep_ir;

static rstep_ir           g_rstep_data;
static hi_ulong           g_rstep_repeat_next_time;
static struct timer_list  g_rstep_timer;

static hi_void rstep_keyup_proc(hi_ulong i)
{
    key_attr       *last_key = NULL;

    last_key = &g_rstep_data.last_key;
    if (g_rstep_data.has_last_key &&
        last_key->key_stat != KEY_STAT_UP) {
        last_key->key_stat = KEY_STAT_UP;
        if (g_ir_local.key_up_event) {
            ir_insert_key_tail(g_ir_local.key_buf, last_key);
            wake_up_interruptible(&g_ir_local.read_wait);
        }
    }
}

hi_void rstep_init(hi_void)
{
    init_timer(&g_rstep_timer);
    g_rstep_timer.expires = 0;
    g_rstep_timer.data = (hi_ulong)~0;
    g_rstep_timer.function = rstep_keyup_proc;

    if (memset_s(&g_rstep_data, sizeof(rstep_ir), 0, sizeof(rstep_ir))) {
        hiir_error("memset_s g_rstep_data failed\n");
    }
}

hi_void rstep_exit(hi_void)
{
    del_timer(&g_rstep_timer);
}
static hi_s32 ir_rstep_decode(rstep_ir *data, ir_signal signal, ir_protocol *ip)
{
    if (geq_margin(signal.duration, RSTEP_UNIT, RSTEP_UNIT / 2) == 0) {
        goto OUT;
    }
AGAIN:
    hiir_debug("RSTEP decode started at state %i (%uus %s)\n",
               data->state, signal.duration, TO_STR(signal.pulse));

    if (!geq_margin(signal.duration, RSTEP_UNIT, RSTEP_UNIT / 2)) {
        return 0;
    }

    switch (data->state) {
        case STATE_INACTIVE:
            if (!eq_margin(signal.duration, RSTEP_BIT_START, RSTEP_UNIT / 2) || (!signal.pulse)) {
                break;
            }
            data->bits = 0;
            data->count = 0;
            data->state = STATE_BIT_START;
            return 0;

        case STATE_BIT_START:
            if (is_transition(&signal, &data->prev_signal) == 0) {
                break;
            }
            data->bits <<= 1;
            if (!signal.pulse) {
                data->bits |= 1;
            }
            data->count++;
            data->state = STATE_BIT_END;

            if (data->count == ip->attr.wanna_bits) {
                goto AGAIN;
            }
            if (geq_margin(signal.duration, 2 * RSTEP_UNIT, RSTEP_UNIT / 2)) {
                data->state = STATE_BIT_START;
            }
            return 0;

        case STATE_CHECK_RSTEPX:
           /* fall-through */
        case STATE_BIT_END:
            hiir_debug("RSTEP decode date bit cnt=%d\n", data->count);
            if (data->count == ip->attr.wanna_bits) {

                data->scancode = data->bits;
                data->this_key.lower = data->scancode & 0x3ffff;
                data->this_key.upper = 0;

                data->count = 0;

                data->state = STATE_FINISHED;
            } else {
                data->state = STATE_BIT_START;
                decrease_duration(&signal, RSTEP_BIT_END);
            }

            goto AGAIN;
            /* fall-through */
        case STATE_FINISHED:
            return 0;
    }

OUT:
    hiir_info("RSTEP decode failed at state %i (%uus %s), data->count:%d, key = 0x%.08llx\n",
              data->state, signal.duration, TO_STR(signal.pulse), data->count, data->scancode & 0x3ffff);
    data->count = 0;
    data->state = STATE_INACTIVE;
    return -EINVAL;
}

/*
 * call condition:
 * if rstep_match return not match, this routine will be called.
 */
ir_match_result rstep_match_error_handle(ir_match_type type, ir_buffer *head, ir_protocol *ip)
{
    key_attr *symbol;
    hi_s32   n;
    hi_s32   i;
    symbol = &head->buf[head->reader];
    if (!symbol->upper && !symbol->lower) {
        return IR_MATCH_NOT_MATCH;
    }
    switch (type) {
        case IR_MTT_HEADER:
            hiir_info("rstep, header matching. symbol[l,u]:[%d, %d],"\
                      "header:[p, s, f]:[%d, %d, %d]\n",
                      (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                      ip->attr.header.pluse, ip->attr.header.space,
                      ip->attr.header.factor);

            if (data_fallin((hi_u32)symbol->lower, (hi_u32)ip->attr.header.minp,
                            (hi_u32)ip->attr.header.maxp) &&
                (data_fallin(symbol->upper, ip->attr.header.minp,
                             ip->attr.header.maxp) ||
                 data_fallin(symbol->upper, 2 * RSTEP_UNIT * (100 - ip->attr.header.factor) / 100,
                             2 * RSTEP_UNIT * (100 + ip->attr.header.factor) / 100))) {
                hiir_info("rstep, header matching: match!\n");
                return IR_MATCH_MATCH;
            }
            hiir_info("rstep, header matching: header mismatch!\n");
            break;
        case IR_MTT_FRAME:
            hiir_debug("rstep, frame matching. symbol[l,u]:[%d, %d],"\
                       "header:[p, s, f]:[%d, %d, %d]\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.header.pluse, ip->attr.header.space,
                       ip->attr.header.factor);

            if (!data_fallin(symbol->lower, ip->attr.header.minp,
                             ip->attr.header.maxp) ||
               (!data_fallin(symbol->upper, ip->attr.header.minp,
                             ip->attr.header.maxp) &&
                !data_fallin(symbol->upper, 2 * RSTEP_UNIT * (100 - ip->attr.header.factor) / 100,
                             2 * RSTEP_UNIT * (100 + ip->attr.header.factor) / 100))) {
                hiir_info("rstep, frame matching: header not match!\n");
                return IR_MATCH_NOT_MATCH;
            }
            hiir_info("rstep, frame matching, header match at:%d, [l, u]:[%d, %d]\n",
                      head->reader, (hi_u32)symbol->lower, (hi_u32)symbol->upper);
            /* try find burst. */
            for (i = head->reader + 1, n = 1; n < MAX_DATA_BITS; n++, i++) {
                if (i >= MAX_SYMBOL_NUM) {
                    i -= MAX_SYMBOL_NUM;
                }
                symbol = &head->buf[i];
                if (!symbol->upper && !symbol->lower) {
                    hiir_debug("%s->%d, needs more data\n",
                               __func__, __LINE__);
                    return IR_MATCH_NEED_MORE_DATA;
                }

                if (symbol->upper > 1000 && symbol->upper < 8000) {
                    return IR_MATCH_NOT_MATCH;
                }
                hiir_debug("rstep, frame matching. symbol[l,u]:[%d, %d] at %d,"\
                           "burst:[p, s, f]:[%d, %d, %d]\n",
                           (hi_u32)symbol->lower, (hi_u32)symbol->upper, i,
                           ip->attr.burst.pluse, ip->attr.burst.space,
                           ip->attr.burst.factor);
                /* ignore lower symbol. */
                if (symbol->upper > ip->attr.burst.maxs ||
                    data_fallin(symbol->upper,
                                ip->attr.burst.mins,
                                ip->attr.burst.maxs)) {
                    hiir_debug("rstep, frame matching: burst match at %d. symbol[l,u]:[%d, %d],"\
                               "burst:[p, s, f]:[%d, %d, %d]\n", i,
                               (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                               ip->attr.burst.pluse, ip->attr.burst.space,
                               ip->attr.burst.factor);
                    if (n >= ip->attr.burst_offset) {
                        n = n - ip->attr.burst_offset;
                    } else {
                        n = ip->attr.burst_offset - n;
                    }
                    if (n <= RSTEP_BURST_OFFSET) {
                        hiir_debug(KERN_DEBUG"%s->%d, rstep frame err matching :"\
                                   " burst at %d, (n=%d), frame match!\n",
                                   __func__, __LINE__, i, n);
                        return IR_MATCH_MATCH;
                    } else {
                        hiir_info(KERN_DEBUG "%s->%d, rstep frame err matching :"\
                                  " burst at %d,(n=%d), frame not match!\n",
                                  __func__, __LINE__, i, n);
                        return IR_MATCH_NOT_MATCH;
                    }
                }
            }

            hiir_debug("%s->%d, burst not found!\n", __func__, __LINE__);
            break;
        case IR_MTT_BURST:
            break;
    }
    return IR_MATCH_NOT_MATCH;
}
/*
 * call condition:
 * if parse_rstep returns !0, this routine can be called.
 */
hi_void rstep_parse_error_handle(ir_priv *ir,
                                 ir_protocol *ip, ir_buffer *symb_head,
                                 ir_buffer *key_head)
{
    key_attr *symbol;

    symbol = &symb_head->buf[symb_head->reader];
    while ((symbol->upper || symbol->lower)) {
        /* clear symbols till burst appears. */
        hiir_debug("%s->%d, process symbol:[l,u] at %d:[%d, %d]\n",
                   __func__, __LINE__, symb_head->reader,
                   (hi_u32)symbol->lower, (hi_u32)symbol->upper);
        if (symbol->upper > ip->attr.burst.maxs ||
            data_fallin(symbol->upper,
                        ip->attr.burst.mins,
                        ip->attr.burst.maxs)) {
            (hi_void)ir_next_reader_clr_inc(symb_head);
            break;
        }
        symbol = ir_next_reader_clr_inc(symb_head);
    }
}

ir_match_result rstep_match(ir_match_type type,
                            ir_buffer *head, ir_protocol *ip)
{
    key_attr            *symbol;
    hi_s32              n;
    hi_s32              i;
    hi_s32              j;
    symbol = &head->buf[head->reader];
    if (!symbol->upper && !symbol->lower) {
        return IR_MATCH_NOT_MATCH;
    }
    switch (type) {
        case IR_MTT_HEADER:
            hiir_info("rstep, header matching. symbol[l,u]:[%d, %d],"\
                      "header:[p, s, f]:[%d, %d, %d]\n",
                      (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                      ip->attr.header.pluse, ip->attr.header.space,
                      ip->attr.header.factor);

            if (data_fallin((hi_u32)symbol->lower, (hi_u32)ip->attr.header.minp,
                            (hi_u32)ip->attr.header.maxp) &&
               (data_fallin(symbol->upper, ip->attr.header.minp,
                            ip->attr.header.maxp) ||
                data_fallin(symbol->upper, 2 * RSTEP_UNIT * (100 - ip->attr.header.factor) / 100,
                            2 * RSTEP_UNIT * (100 + ip->attr.header.factor) / 100))) {
                hiir_info("rstep, header matching: match!\n");
                return IR_MATCH_MATCH;
            }
            hiir_info("rstep, header matching: header mismatch!\n");
            break;
        case IR_MTT_FRAME:
            hiir_info("rstep, frame matching. symbol[l,u]:[%d, %d],"\
                      "header:[p, s, f]:[%d, %d, %d]\n",
                      (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                      ip->attr.header.pluse, ip->attr.header.space,
                      ip->attr.header.factor);

            if ((data_fallin(symbol->lower, ip->attr.header.minp,
                             ip->attr.header.maxp) ==0) ||
               ((data_fallin(symbol->upper, ip->attr.header.minp,
                             ip->attr.header.maxp) == 0) &&
                (data_fallin(symbol->upper, 2 * RSTEP_UNIT * (100 - ip->attr.header.factor) / 100,
                             2 * RSTEP_UNIT * (100 + ip->attr.header.factor) / 100) == 0))) {
                hiir_info("rstep, header matching: not match!\n");
                return IR_MATCH_NOT_MATCH;
            }
            hiir_info("rstep, frame matching, header match at:%d, [l, u]:[%d, %d]\n",
                      head->reader, (hi_u32)symbol->lower, (hi_u32)symbol->upper);
            /* try find burst. */
            n = head->reader + ip->attr.burst_offset - 1;
            if (n >= MAX_SYMBOL_NUM) {
                n -= MAX_SYMBOL_NUM;
            }
            /* check frame symbols */
            for (i = 0, j = i + head->reader + 1; i < ip->attr.wanna_bits && j != n;
                    i++, j++) {
                j %= MAX_SYMBOL_NUM;

                if (j == n) {
                    break;
                }

                symbol = &head->buf[j];
                if (!symbol->lower && !symbol->upper) {
                    hiir_info("%s->%d, need more symbols BUT BURST appears!\n",
                              __func__, __LINE__);
                    return IR_MATCH_NEED_MORE_DATA;
                }

                /* check data phase is exceed or not */
                if (symbol->upper > 1000 && symbol->upper < 8000) {
                    hiir_debug("%s->%d, symbol[%d, %d] not exceed" \
                               " b0 or b1's max space or pluse!\n",
                               __func__, __LINE__,
                               (hi_u32)symbol->upper, (hi_u32)symbol->lower);
                    return IR_MATCH_NOT_MATCH;
                }
            }

            /* try find burst. */
            symbol = &head->buf[n];
            /* ignore lower symbol. */
            if (symbol->upper > ip->attr.burst.maxs ||
                data_fallin(symbol->upper,
                            ip->attr.burst.mins,
                            ip->attr.burst.maxs)) {
                hiir_info("rstep, frame matching: burst match at %d."\
                          " symbol[l,u]:[%d, %d],"\
                          "burst:[p, s, f]:[%d, %d, %d]\n", head->reader,
                          (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                          ip->attr.burst.pluse, ip->attr.burst.space,
                          ip->attr.burst.factor);

                return IR_MATCH_MATCH;
            }
            hiir_info("%s->%d, burst not found!\n", __func__, __LINE__);
            break;
        case IR_MTT_BURST:
            break;
    }
    return IR_MATCH_NOT_MATCH;
}

hi_s32 parse_rstep(ir_priv *ir, ir_protocol *ip,
                   ir_buffer *rd, ir_buffer *wr)
{
    ir_signal          ir_pulse;
    ir_signal          ir_space;
    rstep_ir           *rstep = NULL;
    key_attr           *symbol = NULL;
    hi_s32             ret;
    hi_s32             fail = 0;
    hi_s32             cnt = 0;
    hi_s32             syms = 0;

    rstep = &g_rstep_data;
    symbol = &rd->buf[rd->reader];
    rstep->state = STATE_INACTIVE;
    while (symbol && symbol->lower && symbol->upper && !fail) {
        hiir_debug("%s->%d, parse symbol [l, u]:[%d, %d] at %d,"\
                   " symbols:%d\n", __func__, __LINE__,
                   (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                   rd->reader, syms++);

        ir_pulse.duration = symbol->lower;
        ir_pulse.pulse = 1;
        ret = ir_rstep_decode(rstep, ir_pulse, ip);
        if (ret) {
            fail = 1;
        }
        rstep->prev_signal = ir_pulse;
        ir_space.duration = symbol->upper;
        ir_space.pulse = 0;
        ret = ir_rstep_decode(rstep, ir_space, ip);
        if (ret) {
            fail = 1;
        }
        rstep->prev_signal = ir_space;
        symbol = ir_next_reader_clr_inc(rd);
        hiir_debug("rstep->state=%d\n", rstep->state);
        if (rstep->state == STATE_FINISHED) {
            break;
        }
    }
    if (fail) {
        hiir_info("errors occured while decod rstep frame,"\
                  " discard this frame!\n");
        return -1;
    }

    rstep->count = 0;
    if (memcpy_s(rstep->this_key.protocol_name, PROTOCOL_NAME_SZ, ip->ir_code_name, PROTOCOL_NAME_SZ)) {
        hiir_error("memcpy_s to rstep->this_key.protocol_name failed\n");
    }

    del_timer(&g_rstep_timer);
    /* if a repeat key. */
    if (rstep->has_last_key &&
       (rstep->this_key.lower == rstep->last_key.lower &&
        rstep->this_key.upper == rstep->last_key.upper) &&
        rstep->last_key.key_stat != KEY_STAT_UP) {
        if (ir->key_repeat_event &&
            time_after(jiffies,g_rstep_repeat_next_time)) {
            rstep->last_key.key_stat = KEY_STAT_HOLD;
            ir_insert_key_tail(wr, &rstep->last_key);
            g_rstep_repeat_next_time = jiffies +
                                     msecs_to_jiffies(ir->key_repeat_interval);
            cnt++;
        }
        goto OUT;
    }
    /* if a new key recevied, send a key up event of last key. */
    if (rstep->has_last_key &&
        rstep->last_key.key_stat != KEY_STAT_UP &&
        (rstep->last_key.lower != rstep->this_key.lower ||
        rstep->last_key.upper != rstep->this_key.upper)) {
        if (ir->key_up_event) {
            rstep->last_key.key_stat = KEY_STAT_UP;
            ir_insert_key_tail(wr, &rstep->last_key);
            cnt++;
        }
    }
    /* the new key */
    rstep->this_key.key_stat = KEY_STAT_DOWN;
    ir_insert_key_tail(wr, &rstep->this_key);
    cnt++;
    g_rstep_repeat_next_time = jiffies +
                             msecs_to_jiffies(ir->key_repeat_interval);
OUT:
    rstep->has_last_key = 1;
    if (memcpy_s(&rstep->last_key, sizeof(key_attr), &rstep->this_key, sizeof(key_attr))) {
        hiir_error("memcpy_s to rstep->last_key failed\n");
    }

    if (g_time_out_flag == HI_TRUE) {
        g_time_out_flag = HI_FALSE;
        if ((rstep->last_key.key_stat != KEY_STAT_UP) &&
            (rstep->last_key.upper || rstep->last_key.lower)) {
            rstep->last_key.key_stat = KEY_STAT_UP;
            if (ir->key_up_event) {
                ir_insert_key_tail(wr, &rstep->last_key);
            }
            cnt++;
        }
    } else {
        if (ip->key_hold_timeout_time == 0) {
            g_rstep_timer.expires = jiffies +
                                  msecs_to_jiffies(ir->key_hold_timeout_time);
        } else {
            g_rstep_timer.expires = jiffies +
                                  msecs_to_jiffies(ip->key_hold_timeout_time);
        }
    }
    g_rstep_timer.data = ip->priv;
    add_timer(&g_rstep_timer);

    return cnt ? 0 : -1;
}

EXPORT_SYMBOL(rstep_match);
EXPORT_SYMBOL(parse_rstep);
EXPORT_SYMBOL(rstep_parse_error_handle);
EXPORT_SYMBOL(rstep_match_error_handle);
