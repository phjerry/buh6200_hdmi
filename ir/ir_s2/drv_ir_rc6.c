#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/spinlock.h>

#include "hi_drv_ir.h"
#include "drv_ir_protocol.h"
#include "drv_ir_utils.h"

#define RC6_UNIT            445   /* us */
#define RC6_HEADER_NBITS    4     /* not including toggle bit */
#define RC6_PREFIX_PULSE    (6 * RC6_UNIT)
#define RC6_PREFIX_SPACE    (2 * RC6_UNIT)
#define RC6_BIT_START       (1 * RC6_UNIT)
#define RC6_BIT_END         (1 * RC6_UNIT)
#define RC6_TOGGLE_START    (2 * RC6_UNIT)
#define RC6_TOGGLE_END      (2 * RC6_UNIT)
#define RC6_TOGGLE_MASK     0x8000

typedef enum {
    STATE_INACTIVE,         /* 0 */
    STATE_PREFIX_SPACE,
    STATE_HEADER_BIT_START,
    STATE_HEADER_BIT_END,   /* 3 */
    STATE_TOGGLE_START,     /* 4 */
    STATE_TOGGLE_END,
    STATE_BIT_START,
    STATE_BIT_END,
    STATE_FINISHED,
} rc6_state;

typedef struct {
    rc6_state state;
    hi_u32 header;
    hi_u64 bits;
    hi_s32 count;
    hi_u64 scancode;
    key_attr this_key;
    key_attr last_key;
    hi_s32 has_last_key;
    ir_signal prev_signal;
} rc6_ir;

static rc6_ir            g_rc6_data[MAX_RC6_INFR_NR];
static hi_ulong          g_rc6_repeat_next_time[MAX_RC6_INFR_NR];
static struct timer_list g_rc6_timer[MAX_RC6_INFR_NR];

static hi_void rc6_keyup_proc(hi_ulong i)
{
    key_attr *last_key = NULL;
    if (i >= MAX_RC6_INFR_NR) {
        hiir_error("rc6 keyup timer, i > MAX_RC6_INFR_NR!\n");
        return;
    }

    last_key = &g_rc6_data[i].last_key;
    if (g_rc6_data[i].has_last_key && last_key->key_stat != KEY_STAT_UP) {
        last_key->key_stat = KEY_STAT_UP;
        if (g_ir_local.key_up_event) {
            ir_insert_key_tail(g_ir_local.key_buf, last_key);
            wake_up_interruptible(&g_ir_local.read_wait);
        }
    }
}

hi_void rc6_init(hi_void)
{
    hi_s32 i;
    for (i = 0; i < MAX_RC6_INFR_NR; i++) {
        init_timer(&g_rc6_timer[i]);
        g_rc6_timer[i].expires = 0;
        g_rc6_timer[i].data = (hi_ulong)~0;
        g_rc6_timer[i].function = rc6_keyup_proc;
    }

    if (memset_s(g_rc6_data, sizeof(rc6_ir) * MAX_RC6_INFR_NR, 0, sizeof(rc6_ir) * MAX_RC6_INFR_NR)) {
        hiir_error("memset_s g_rc6_data failed\n");
        return;
    }
}

hi_void rc6_exit(hi_void)
{
    hi_s32 i;
    for (i = 0; i < MAX_RC6_INFR_NR; i++) {
        del_timer_sync(&g_rc6_timer[i]);
    }
}

hi_s32 rc6_process_states(rc6_ir *data, ir_signal signal)
{
    if (data->state == STATE_INACTIVE) {
        if (signal.pulse == 0) {
            return -1;
        }

        if (eq_margin(signal.duration, RC6_PREFIX_PULSE, RC6_UNIT) == 0) {
            return -1;
        }

        data->state = STATE_PREFIX_SPACE;
        data->bits = 0;
        data->count = 0;
        data->header = 0;

        if (memset_s(&data->this_key, sizeof(key_attr), 0, sizeof(key_attr))) {
            hiir_error("memset_s data->this_key failed\n");
            return -1;
        }

        return 0;
    }

    if (data->state == STATE_PREFIX_SPACE) {
        if (signal.pulse) {
            return -1;
        }

        if (eq_margin(signal.duration, RC6_PREFIX_SPACE,
                      RC6_UNIT / 2) == 0) {
            return -1;
        }

        data->state = STATE_HEADER_BIT_START;
        return 0;
    }

    if (data->state == STATE_HEADER_BIT_START) {
        if (eq_margin(signal.duration, RC6_BIT_START,
                      RC6_UNIT / 2) == 0) {
            return -1;
        }

        data->header <<= 1;
        if (signal.pulse) {
            data->header |= 1;
        }

        data->count++;
        data->state = STATE_HEADER_BIT_END;
        return 0;
    }

    if (data->state == STATE_TOGGLE_START) {
        if (eq_margin(signal.duration, RC6_TOGGLE_START, RC6_UNIT / 2) == 0) {
            return -1;
        }

        data->state = STATE_TOGGLE_END;
        return 0;
    }

    if (data->state == STATE_BIT_START) {
        if (eq_margin(signal.duration, RC6_BIT_START, RC6_UNIT / 2) == 0) {
            return -1;
        }

        data->bits <<= 1;
        if (signal.pulse) {
            data->bits |= 1;
        }

        data->count++;
        data->state = STATE_BIT_END;
        return 0;
    }

    if (data->state == STATE_FINISHED) {
        if (signal.pulse) {
            return -1;
        }

        data->state = STATE_INACTIVE;
        return 0;
    }

    return -1;
}

static hi_s32 ir_rc6_decode(rc6_ir *data,
                            ir_signal signal,
                            ir_protocol *ip)
{
    hi_s32 i;
    hi_s32 ret;
    i = ip->priv;
    if (i >= MAX_RC6_INFR_NR) {
        goto OUT;
    }

    if (geq_margin(signal.duration, RC6_UNIT, RC6_UNIT / 2) == 0) {
        goto OUT;
    }

AGAIN:
    hiir_debug("RC6 decode started at state %i (%uus %s) "
               "count %d header 0x%x bits 0x%llx, count:%d\n",
               data->state, signal.duration, TO_STR(signal.pulse),
               data->count, data->header, data->bits, data->count);

    if (geq_margin(signal.duration, RC6_UNIT, RC6_UNIT / 2) == 0) {
        return 0;
    }

    switch (data->state) {
        case STATE_HEADER_BIT_END:
            if (is_transition(&signal, &data->prev_signal) == 0) {
                break;
            }

            if (data->count == RC6_HEADER_NBITS) {
                data->state = STATE_TOGGLE_START;
            } else {
                data->state = STATE_HEADER_BIT_START;
            }

            decrease_duration(&signal, RC6_BIT_END);
            goto AGAIN;
            /* fall-through */
        case STATE_TOGGLE_END:
            if (is_transition(&signal, &data->prev_signal) == 0 ||
                geq_margin(signal.duration, RC6_TOGGLE_END, RC6_UNIT / 2) == 0) {
                break;
            }

            data->state = STATE_BIT_START;
            decrease_duration(&signal, RC6_TOGGLE_END);
            data->count = 0;

            goto AGAIN;
            /* fall-through */
        case STATE_BIT_END:
            if (is_transition(&signal, &data->prev_signal) == 0) {
                break;
            }

            if (data->count == ip->attr.wanna_bits) {
                data->scancode = data->bits;
                data->this_key.lower = (data->scancode & (~RC6_TOGGLE_MASK));
                data->this_key.upper = 0;
                data->state = STATE_FINISHED;
                return 0;
            } else {
                data->state = STATE_BIT_START;
            }

            decrease_duration(&signal, RC6_BIT_END);
            goto AGAIN;
            /* fall-through */
        default:
            ret = rc6_process_states(data, signal);
            if (ret == 0) {
                return 0;
            }

    }

OUT:
    hiir_info("RC6 decode failed at state %i (%uus %s), bits received:%d\n",
              data->state, signal.duration, TO_STR(signal.pulse), data->count);
    data->state = STATE_INACTIVE;
    return -EINVAL;
}
ir_match_result rc6_match(ir_match_type type,
                          ir_buffer *head,
                          ir_protocol *ip)
{
    key_attr *symbol;
    hi_s32   n;
    symbol = &head->buf[head->reader];
    if (!symbol->upper && !symbol->lower) {
        return IR_MATCH_NOT_MATCH;
    }

    switch (type) {
        case IR_MTT_HEADER:
            hiir_debug("rc6, header matching. symbol[l,u]:[%d, %d],"\
                       "header:[p, s, f]:[%d, %d, %d]\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.header.pluse, ip->attr.header.space,
                       ip->attr.header.factor);

            if (key_match_phase(symbol, &ip->attr.header) == 0) {
                hiir_debug("rc6, header matching: match!\n");
                return IR_MATCH_MATCH;
            }
            hiir_debug("rc6, header matching: header mismatch!\n");
            break;
        case IR_MTT_FRAME:
            hiir_debug("rc6, frame matching. symbol[l,u]:[%d, %d],"\
                       "header:[p, s, f]:[%d, %d, %d]\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.header.pluse, ip->attr.header.space,
                       ip->attr.header.factor);

            if (key_match_phase(symbol, &ip->attr.header)) {
                hiir_debug("rc6, frame matching: header mismatch!\n");
                return IR_MATCH_NOT_MATCH;
            }
            hiir_debug("rc6, frame matching, header match at:%d, [l, u]:[%d, %d]\n",
                       head->reader, (hi_u32)symbol->lower, (hi_u32)symbol->upper);
            /* try find burst. */
            n = head->reader + ip->attr.burst_offset + 1;
            if (n >= MAX_SYMBOL_NUM) {
                n -= MAX_SYMBOL_NUM;
            }
            symbol = &head->buf[n];
            /* ignore lower symbol. */
            if (symbol->upper > ip->attr.burst.maxs ||
                data_fallin(symbol->upper,ip->attr.burst.mins,ip->attr.burst.maxs)) {
                hiir_debug("rc6, frame matching: burst match at %d."\
                           " symbol[l,u]:[%d, %d],"\
                           "burst:[p, s, f]:[%d, %d, %d]\n", head->reader,
                           (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                           ip->attr.burst.pluse, ip->attr.burst.space,
                           ip->attr.burst.factor);

                return IR_MATCH_MATCH;
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
 * if rc6_match return not match, this routine will be called.
 */
ir_match_result rc6_match_error_handle(ir_match_type type,
                                       ir_buffer *head,
                                       ir_protocol *ip)
{
    key_attr    *symbol;
    hi_s32      n;
    hi_s32      i;
    hi_s32      s;
    hi_u64      tmp;
    symbol = &head->buf[head->reader];
    if (!symbol->upper && !symbol->lower) {
        return IR_MATCH_NOT_MATCH;
    }

    switch (type) {
        case IR_MTT_HEADER:
            hiir_debug("rc6 match err handle, header matching."\
                       " symbol[l,u]:[%d, %d],"\
                       "header:[p, s, f]:[%d, %d, %d]\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.header.pluse, ip->attr.header.space,
                       ip->attr.header.factor);

            if (key_match_phase(symbol, &ip->attr.header) == 0) {
                hiir_debug("rc6 match err handle,"\
                           " header matching: match!\n");
                return IR_MATCH_MATCH;
            }
            hiir_debug("rc6 match err handle, header matching:"\
                       " header mismatch!\n");
            break;
        case IR_MTT_FRAME:
            hiir_debug("rc6 match err handle, frame matching."\
                       " symbol[l,u]:[%d, %d],"\
                       "header:[p, s, f]:[%d, %d, %d]\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.header.pluse, ip->attr.header.space,
                       ip->attr.header.factor);

            if (key_match_phase(symbol, &ip->attr.header)) {
                hiir_debug("rc6, frame matching: header mismatch!\n");
                return IR_MATCH_NOT_MATCH;
            }
            hiir_debug("rc6, frame err matching, header match at:%d, [l, u]:[%d, %d]\n",
                       head->reader, (hi_u32)symbol->lower, (hi_u32)symbol->upper);

            for (i = head->reader + 1, n = 1, s = 0; n < MAX_DATA_BITS; n++, i++) {
                if (i >= MAX_SYMBOL_NUM) {
                    i -= MAX_SYMBOL_NUM;
                }
                symbol = &head->buf[i];
                if (!symbol->upper && !symbol->lower) {
                    hiir_debug("%s->%d, needs more data\n",
                               __func__, __LINE__);
                    return IR_MATCH_NEED_MORE_DATA;
                }
                hiir_debug("rc6, frame err matching. symbol[l,u]:[%d, %d] at %d,"\
                           "burst:[p, s, f]:[%d, %d, %d] s=%d\n",
                           (hi_u32)symbol->lower, (hi_u32)symbol->upper, i,
                           ip->attr.burst.pluse, ip->attr.burst.space,
                           ip->attr.burst.factor, s);
                /* ignore lower symbol. */
                if (symbol->upper > ip->attr.burst.maxs ||
                    data_fallin(symbol->upper,
                                ip->attr.burst.mins,
                                ip->attr.burst.maxs)) {
                    hiir_debug("rc6, frame matching: burst match at %d. symbol[l,u]:[%d, %d],"\
                               "burst:[p, s, f]:[%d, %d, %d]\n", i,
                               (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                               ip->attr.burst.pluse, ip->attr.burst.space,
                               ip->attr.burst.factor);

                    /* 6: 4bit header and 2bit TR */
                    if ((s >= ((ip->attr.burst_offset + 6) * 2 - 3)) &&
                        (s <= ((ip->attr.burst_offset + 6) * 2 - 1))) {
                        hiir_debug("%s->%d, rc6 frame err matching :"\
                                   " burst at %d, (n=%d), frame match!\n",
                                   __func__, __LINE__, i, n);
                        return IR_MATCH_MATCH;
                    } else {
                        hiir_debug("%s->%d, rc6 frame err matching :"\
                                   " burst at %d,(n=%d), frame not match!\n",
                                   __func__, __LINE__, i, n);
                        return IR_MATCH_NOT_MATCH;
                    }
                }
                tmp = symbol->upper + symbol->lower;
                while (geq_margin(tmp, RC6_UNIT, RC6_UNIT / 2)) {
                    s += 1;
                    if (tmp <= RC6_UNIT) {
                        break;
                    }
                    tmp -= RC6_UNIT;
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
 * if parse_rc6 returns !0, this routine can be called.
 * discard the frame we cannot parsed..
 * if we do not dicard this frame, it maybe recognized as rc5 or other
 * infrared code....
 */
hi_void rc6_parse_error_handle(ir_priv *ir, ir_protocol *ip,
                               ir_buffer *symb_head, ir_buffer *key_head)
{
    key_attr *symbol;
    symbol = &symb_head->buf[symb_head->reader];
    while ((symbol->upper || symbol->lower)) {
        /* clear symbols till burst appears. */
        hiir_debug("%s->%d, process symbol:[l,u] at %d:[%d, %d]\n",
                   __func__, __LINE__, symb_head->reader,
                   (hi_u32)symbol->lower, (hi_u32)symbol->upper);
        if (symbol->upper > ip->attr.burst.maxs ||
                data_fallin(symbol->upper, ip->attr.burst.mins, ip->attr.burst.maxs)) {
            (hi_void)ir_next_reader_clr_inc(symb_head);
            break;
        }
        symbol = ir_next_reader_clr_inc(symb_head);
    }
}

hi_void rc6_process_out(ir_priv *ir, ir_protocol *ip,
                        rc6_ir *rc6, ir_buffer *wr, hi_s32 *pcnt)
{
    rc6->has_last_key = 1;
    if (memcpy_s(&rc6->last_key, sizeof(key_attr), &rc6->this_key, sizeof(key_attr))) {
        hiir_error("memcpy_s to rc6->last_key failed\n");
        return;
    }

    if (g_time_out_flag == HI_TRUE) {
        g_time_out_flag = HI_FALSE;
        rc6->last_key.key_stat = KEY_STAT_UP;
        if (ir->key_up_event) {
            ir_insert_key_tail(wr, &rc6->last_key);
        }
        *pcnt = *pcnt + 1;
    } else {
        if (ip->key_hold_timeout_time == 0) {
            g_rc6_timer[ip->priv].expires = jiffies +
                                          msecs_to_jiffies(ir->key_hold_timeout_time);
        } else {
            g_rc6_timer[ip->priv].expires = jiffies +
                                          msecs_to_jiffies(ip->key_hold_timeout_time);
        }

        g_rc6_timer[ip->priv].data = ip->priv;
        add_timer(&g_rc6_timer[ip->priv]);
    }

    return;
}

hi_s32 parse_rc6(ir_priv *ir, ir_protocol *ip,
                 ir_buffer *rd, ir_buffer *wr)
{
    ir_signal    ir_pulse;
    ir_signal    ir_space;
    rc6_ir       *rc6 = NULL;
    key_attr     *symbol = NULL;
    hi_s32       ret;
    hi_s32       i;
    hi_s32       fail;
    hi_s32       cnt = 0;
    hi_s32       flag = 0;
    hi_s32       flag2 = 0;
    hi_s32       flag3 = 0;
    hi_s32       syms;
    if (ip->priv >= MAX_RC6_INFR_NR) {
        hiir_error("ip->priv > MAX_RC6_INFR_NR!\n");
        return -1;
    }

    rc6 = &g_rc6_data[ip->priv];
    symbol = &rd->buf[rd->reader];
    fail = i = 0;
    rc6->state = STATE_INACTIVE;
    syms = 0;
    flag = symbol && symbol->lower && symbol->upper && !fail;
    while (flag) {
        syms++;
        hiir_debug("%s->%d, parse symbol [l, u]:[%d, %d] at %d, symbols:%d\n",
                   __func__, __LINE__,
                   (hi_u32)symbol->lower, (hi_u32)symbol->upper, rd->reader, syms);
        ir_pulse.duration = symbol->lower;
        ir_pulse.pulse = 1;
        ret = ir_rc6_decode(rc6, ir_pulse, ip);
        if (ret) {
            fail++;
        }
        rc6->prev_signal = ir_pulse;

        ir_space.duration = symbol->upper;
        ir_space.pulse = 0;
        ret = ir_rc6_decode(rc6, ir_space, ip);
        if (ret) {
            fail++;
        }

        rc6->prev_signal = ir_space;
        symbol = ir_next_reader_clr_inc(rd);
        if ((rc6->state == STATE_FINISHED ||
             rc6->state == STATE_INACTIVE)) {
            hiir_debug("%s->%d, rc6->state :%d\n", __func__, __LINE__, rc6->state);
            break;
        }
    }
    if (fail) {
        hiir_info("errors occured while decode rc6 frame, discard this frame!\n");
        return -1;
    }

    if (memcpy_s(rc6->this_key.protocol_name, PROTOCOL_NAME_SZ, ip->ir_code_name, PROTOCOL_NAME_SZ)) {
        hiir_error("memcpy_s to rc6->this_key.protocol_name failed\n");
        return -1;
    }

    del_timer_sync(&g_rc6_timer[ip->priv]);
    /* if a repeat key. */
    flag2 = rc6->has_last_key &&
           (rc6->this_key.lower == rc6->last_key.lower && rc6->this_key.upper == rc6->last_key.upper) &&
            rc6->last_key.key_stat != KEY_STAT_UP;
    if (flag2) {
        if (ir->key_repeat_event &&
            time_after(jiffies,
                       g_rc6_repeat_next_time[ip->priv])) {
            rc6->last_key.key_stat = KEY_STAT_HOLD;
            ir_insert_key_tail(wr, &rc6->last_key);
            g_rc6_repeat_next_time[ip->priv] = jiffies +
                                             msecs_to_jiffies(ir->key_repeat_interval);
        }
        cnt++;
        goto OUT;
    }
    /* if a new key recevied, send a key up event of last key. */
    flag3 = rc6->has_last_key &&
            rc6->last_key.key_stat != KEY_STAT_UP &&
            (rc6->last_key.lower != rc6->this_key.lower ||
             rc6->last_key.upper != rc6->this_key.upper);
    if (flag3) {
        rc6->last_key.key_stat = KEY_STAT_UP;
        if (ir->key_up_event) {
            ir_insert_key_tail(wr, &rc6->last_key);
        }
        cnt++;
    }
    /* the new key */
    rc6->this_key.key_stat = KEY_STAT_DOWN;
    ir_insert_key_tail(wr, &rc6->this_key);
    cnt++;
    g_rc6_repeat_next_time[ip->priv] = jiffies +
                                     msecs_to_jiffies(ir->key_repeat_interval);

OUT:
    rc6_process_out(ir, ip, rc6, wr, &cnt);

    return 0;
}
EXPORT_SYMBOL(rc6_match);
EXPORT_SYMBOL(parse_rc6);
EXPORT_SYMBOL(rc6_match_error_handle);
EXPORT_SYMBOL(rc6_parse_error_handle);
