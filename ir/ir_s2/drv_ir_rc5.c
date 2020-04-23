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

#define RC5_NBITS           14
#define CHECK_RC5X_NBITS    8
#define RC5_UNIT            889 /* us */
#define RC5_HEADER_PULSE    (2 * RC5_UNIT)

#define RC5_BIT_START       (1 * RC5_UNIT)
#define RC5_BIT_END         (1 * RC5_UNIT)
#define RC5X_SPACE          (4 * RC5_UNIT)

typedef enum {
    STATE_INACTIVE,         /* 0 */
    STATE_BIT_START,
    STATE_BIT_END,
    STATE_CHECK_RC5X,
    STATE_FINISHED,
} rc5_state;

typedef struct {
    rc5_state state;
    hi_u64    bits;
    hi_u64    scancode;
    hi_s32    count;
    key_attr  this_key;
    key_attr  last_key;
    hi_s32    has_last_key;
    ir_signal prev_signal;
} rc5_ir;

static rc5_ir              g_rc5_data[MAX_RC5_INFR_NR];
static hi_ulong            g_rc5_repeat_next_time[MAX_RC5_INFR_NR];
static struct timer_list   g_rc5_timer[MAX_RC5_INFR_NR];

static hi_void rc5_keyup_proc(hi_ulong i)
{
    key_attr          *last_key = NULL;
    if (i >= MAX_RC5_INFR_NR) {
        hiir_error("rc5 keyup timer, i > MAX_RC5_INFR_NR!\n");
        return;
    }

    last_key = &g_rc5_data[i].last_key;
    if (g_rc5_data[i].has_last_key &&
        last_key->key_stat != KEY_STAT_UP) {
        last_key->key_stat = KEY_STAT_UP;
        if (g_ir_local.key_up_event) {
            ir_insert_key_tail(g_ir_local.key_buf, last_key);
            wake_up_interruptible(&g_ir_local.read_wait);
        }
    }
}
hi_void rc5_init(hi_void)
{
    hi_s32 i;
    for (i = 0; i < MAX_RC5_INFR_NR; i++) {
        init_timer(&g_rc5_timer[i]);
        g_rc5_timer[i].expires = 0;
        g_rc5_timer[i].data = (hi_ulong)~0;
        g_rc5_timer[i].function = rc5_keyup_proc;
    }

    if (memset_s(g_rc5_data, sizeof(rc5_ir) * MAX_RC5_INFR_NR, 0, sizeof(rc5_ir) * MAX_RC5_INFR_NR)) {
        hiir_error("memset_s g_rc5_data failed\n");
        return;
    }
}
hi_void rc5_exit(hi_void)
{
    hi_s32 i;
    for (i = 0; i < MAX_RC5_INFR_NR; i++) {
        del_timer_sync(&g_rc5_timer[i]);
    }
}
static hi_s32 ir_rc5_decode(rc5_ir *data, ir_signal signal, ir_protocol *ip)
{
    hi_s32 i = ip->priv;
    if (i > MAX_RC5_INFR_NR) {
        goto OUT;
    }

    if (!geq_margin(signal.duration, RC5_UNIT, RC5_UNIT / 2)) {
        goto OUT;
    }

AGAIN:
    hiir_debug("RC5 decode started at state %i (%uus %s)\n",
               data->state, signal.duration, TO_STR(signal.pulse));

    if (!geq_margin(signal.duration, RC5_UNIT, RC5_UNIT / 2)) {
        return 0;
    }

    switch (data->state) {
        case STATE_INACTIVE:
            if (!signal.pulse) {
                break;
            }

            data->state = STATE_BIT_START;
            data->count = 1;
            data->bits = 0;

            if (memset_s(&data->this_key, sizeof(key_attr), 0, sizeof(key_attr))) {
                hiir_error("memset_s data->this_key failed\n");
                break;
            }

            /* we just need enough bits to get to STATE_CHECK_RC5X */
            decrease_duration(&signal, RC5_BIT_START);
            goto AGAIN;
            /* fall-through */
        case STATE_BIT_START:
            if (!eq_margin(signal.duration, RC5_BIT_START, RC5_UNIT / 2)) {
                break;
            }

            data->bits <<= 1;
            if (!signal.pulse) {
                data->bits |= 1;
            }

            data->count++;
            data->state = STATE_BIT_END;
            return 0;

        case STATE_BIT_END:
            if (!is_transition(&signal, &data->prev_signal)) {
                break;
            }

            if (data->count == ip->attr.wanna_bits) {
                data->scancode = data->bits;
                data->this_key.lower = data->scancode & 0x7ff;
                data->this_key.upper = 0;
                data->state = STATE_FINISHED;
            } else if (data->count == CHECK_RC5X_NBITS) {
                data->state = STATE_CHECK_RC5X;
            } else {
                data->state = STATE_BIT_START;
            }

            decrease_duration(&signal, RC5_BIT_END);
            goto AGAIN;
            /* fall-through */
        case STATE_CHECK_RC5X:

            data->state = STATE_BIT_START;
            goto AGAIN;
            /* fall-through */
        case STATE_FINISHED:
            if (signal.pulse) {
                break;
            }

            data->state = STATE_INACTIVE;
            return 0;
    }

OUT:
    hiir_info("RC5 decode failed at state %i (%uus %s), data->count:%d\n",
              data->state, signal.duration, TO_STR(signal.pulse), data->count);
    data->state = STATE_INACTIVE;
    return -EINVAL;
}

/*
 * call condition:
 * if rc5_match return not match, this routine will be called.
 */
ir_match_result rc5_match_error_handle(ir_match_type type,
                                       ir_buffer *head,
                                       ir_protocol *ip)
{
    key_attr  *symbol;
    hi_s32    n;
    hi_s32    i;
    symbol = &head->buf[head->reader];
    if (!symbol->upper && !symbol->lower) {
        return IR_MATCH_NOT_MATCH;
    }

    switch (type) {
        case IR_MTT_HEADER:
            hiir_debug("rc5, header matching. symbol[l,u]:[%d, %d],"\
                       "header:[p, s, f]:[%d, %d, %d]\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.header.pluse, ip->attr.header.space,
                       ip->attr.header.factor);

            if (key_match_phase(symbol, &ip->attr.header) == 0) {
                hiir_debug("rc5, header matching: match!\n");
                return IR_MATCH_MATCH;
            }
            hiir_debug("rc5, header matching: header mismatch!\n");
            break;
        case IR_MTT_FRAME:
            hiir_debug("rc5, frame matching. symbol[l,u]:[%d, %d],"\
                       "header:[p, s, f]:[%d, %d, %d]\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.header.pluse, ip->attr.header.space,
                       ip->attr.header.factor);

            if (key_match_phase(symbol, &ip->attr.header)) {
                hiir_debug("rc5, frame matching: header mismatch!\n");
                return IR_MATCH_NOT_MATCH;
            }
            hiir_debug("rc5, frame matching, header match at:%d, [l, u]:[%d, %d]\n",
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
                hiir_debug("rc5, frame matching. symbol[l,u]:[%d, %d] at %d,"\
                           "burst:[p, s, f]:[%d, %d, %d]\n",
                           (hi_u32)symbol->lower, (hi_u32)symbol->upper, i,
                           ip->attr.burst.pluse, ip->attr.burst.space,
                           ip->attr.burst.factor);
                /* ignore lower symbol. */
                if (symbol->upper > ip->attr.burst.maxs ||
                    data_fallin(symbol->upper,
                                ip->attr.burst.mins,
                                ip->attr.burst.maxs)) {
                    hiir_debug("rc5, frame matching: burst match at %d. symbol[l,u]:[%d, %d],"\
                               "burst:[p, s, f]:[%d, %d, %d]\n", i,
                               (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                               ip->attr.burst.pluse, ip->attr.burst.space,
                               ip->attr.burst.factor);
                    if (n >= ip->attr.burst_offset) {
                        n = n - ip->attr.burst_offset;
                    } else {
                        n = ip->attr.burst_offset - n;
                    }

                    if (n <= RC5_BURST_OFFSET) {
                        hiir_debug(KERN_DEBUG"%s->%d, rc5 frame err matching :"\
                                   " burst at %d, (n=%d), frame match!\n",
                                   __func__, __LINE__, i, n);
                        return IR_MATCH_MATCH;
                    } else {
                        hiir_debug(KERN_DEBUG "%s->%d, rc5 frame err matching :"\
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
 * if parse_rc5 returns !0, this routine can be called.
 */
hi_void rc5_parse_error_handle(ir_priv *ir, ir_protocol *ip,
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
            data_fallin(symbol->upper,ip->attr.burst.mins,ip->attr.burst.maxs)) {
            (hi_void)ir_next_reader_clr_inc(symb_head);
            break;
        }
        symbol = ir_next_reader_clr_inc(symb_head);
    }
}

ir_match_result rc5_match(ir_match_type type,
                          ir_buffer *head,
                          ir_protocol *ip)
{
    key_attr  *symbol;
    hi_s32    n;
    symbol = &head->buf[head->reader];
    if (!symbol->upper && !symbol->lower) {
        return IR_MATCH_NOT_MATCH;
    }

    switch (type) {
        case IR_MTT_HEADER:
            hiir_debug("rc5, header matching. symbol[l,u]:[%d, %d],"\
                       "header:[p, s, f]:[%d, %d, %d]\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.header.pluse, ip->attr.header.space,
                       ip->attr.header.factor);

            if (key_match_phase(symbol, &ip->attr.header) == 0) {
                hiir_debug("rc5, header matching: match!\n");
                return IR_MATCH_MATCH;
            }
            hiir_debug("rc5, header matching: header mismatch!\n");
            break;
        case IR_MTT_FRAME:
            hiir_debug("rc5, frame matching. symbol[l,u]:[%d, %d],"\
                       "header:[p, s, f]:[%d, %d, %d]\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.header.pluse, ip->attr.header.space,
                       ip->attr.header.factor);

            if (key_match_phase(symbol, &ip->attr.header)) {
                hiir_debug("rc5, frame matching: header mismatch!\n");
                return IR_MATCH_NOT_MATCH;
            }
            hiir_debug("rc5, frame matching, header match at:%d, [l, u]:[%d, %d]\n",
                       head->reader, (hi_u32)symbol->lower, (hi_u32)symbol->upper);
            /* try find burst. */
            n = head->reader + ip->attr.burst_offset + 1;
            if (n >= MAX_SYMBOL_NUM) {
                n -= MAX_SYMBOL_NUM;
            }
            symbol = &head->buf[n];
            /* ignore lower symbol. */
            if (symbol->upper > ip->attr.burst.maxs ||
                data_fallin(symbol->upper,
                            ip->attr.burst.mins,
                            ip->attr.burst.maxs)) {
                hiir_debug("rc5, frame matching: burst match at %d."\
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

hi_void rc5_out_process(ir_priv *ir, ir_protocol *ip, rc5_ir *rc5, ir_buffer *wr, hi_s32 *pcnt)
{
    rc5->has_last_key = 1;
    if (memcpy_s(&rc5->last_key, sizeof(key_attr), &rc5->this_key, sizeof(key_attr))) {
        hiir_error("memcpy_s to rc5->last_key failed\n");
        return;
    }

    if (g_time_out_flag == HI_TRUE) {
        g_time_out_flag = HI_FALSE;
        rc5->last_key.key_stat = KEY_STAT_UP;
        if (ir->key_up_event) {
            ir_insert_key_tail(wr, &rc5->last_key);
        }
        *pcnt = *pcnt + 1;
    } else {
        if (ip->key_hold_timeout_time == 0) {
            g_rc5_timer[ip->priv].expires = jiffies +
                                          msecs_to_jiffies(ir->key_hold_timeout_time);
        } else {
            g_rc5_timer[ip->priv].expires = jiffies +
                                          msecs_to_jiffies(ip->key_hold_timeout_time);
        }

        g_rc5_timer[ip->priv].data = ip->priv;
        add_timer(&g_rc5_timer[ip->priv]);
    }

    return;
}


hi_s32 parse_rc5(ir_priv *ir, ir_protocol *ip,
                 ir_buffer *rd, ir_buffer *wr)
{
    ir_signal    ir_pulse;
    ir_signal    ir_space;
    rc5_ir       *rc5 = NULL;
    key_attr     *symbol = NULL;
    hi_s32       ret;
    hi_s32       i;
    hi_s32       fail;
    hi_s32       cnt = 0;
    hi_s32       flag = 0;
    hi_s32       flag2 = 0;

    if (ip->priv >= MAX_RC5_INFR_NR) {
        hiir_error("ip->pirv > MAX_RC5_INFR_NR!\n");
        return -1;
    }
    rc5 = &g_rc5_data[ip->priv];
    symbol = &rd->buf[rd->reader];
    fail = i = 0;
    rc5->state = STATE_INACTIVE;
    while (symbol && symbol->lower && symbol->upper && !fail) {
        hiir_debug("%s->%d, parse symbol [l, u]:[%d, %d] at %d\n",
                   __func__, __LINE__,
                   (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                   rd->reader);

        ir_pulse.duration = symbol->lower;
        ir_pulse.pulse = 1;
        ret = ir_rc5_decode(rc5, ir_pulse, ip);
        if (ret) {
            fail++;
        }
        rc5->prev_signal = ir_pulse;
        ir_space.duration = symbol->upper;
        ir_space.pulse = 0;
        ret = ir_rc5_decode(rc5, ir_space, ip);
        if (ret) {
            fail++;
        }
        rc5->prev_signal = ir_space;
        symbol = ir_next_reader_clr_inc(rd);
        if (rc5->state == STATE_FINISHED ||
            rc5->state == STATE_INACTIVE) {
            break;
        }
    }
    if (fail) {
        hiir_info("errors occured while decod rc5 frame,"\
                  " discard this frame!\n");
        return -1;
    }

    if (memcpy_s(rc5->this_key.protocol_name, PROTOCOL_NAME_SZ, ip->ir_code_name, PROTOCOL_NAME_SZ)) {
        hiir_error("memcpy_s to rc5->this_key.protocol_name failed\n");
        return -1;
    }

    del_timer_sync(&g_rc5_timer[ip->priv]);
    /* if a repeat key. */
    flag = rc5->has_last_key &&
           (rc5->this_key.lower == rc5->last_key.lower && rc5->this_key.upper == rc5->last_key.upper) &&
           (rc5->last_key.key_stat != KEY_STAT_UP);
    if (flag) {
        if (ir->key_repeat_event &&
            time_after(jiffies, g_rc5_repeat_next_time[ip->priv])) {
            rc5->last_key.key_stat = KEY_STAT_HOLD;
            ir_insert_key_tail(wr, &rc5->last_key);
            g_rc5_repeat_next_time[ip->priv] = jiffies +
                                             msecs_to_jiffies(ir->key_repeat_interval);
            cnt++;
        }
        goto OUT;
    }
    /* if a new key recevied, send a key up event of last key. */
    flag2 = rc5->has_last_key &&
            rc5->last_key.key_stat != KEY_STAT_UP &&
            (rc5->last_key.lower != rc5->this_key.lower ||
            rc5->last_key.upper != rc5->this_key.upper);
    if (flag2) {
        rc5->last_key.key_stat = KEY_STAT_UP;
        if (ir->key_up_event) {
            ir_insert_key_tail(wr, &rc5->last_key);
        }
        cnt++;
    }
    /* the new key */
    rc5->this_key.key_stat = KEY_STAT_DOWN;
    ir_insert_key_tail(wr, &rc5->this_key);
    cnt++;
    g_rc5_repeat_next_time[ip->priv] = jiffies +
                                     msecs_to_jiffies(ir->key_repeat_interval);
OUT:
    rc5_out_process(ir, ip, rc5, wr, &cnt);

    return cnt ? 0 : -1;
}

hi_void extended_rc5_out_process(ir_priv *ir, ir_protocol *ip, rc5_ir *rc5, ir_buffer *wr, hi_s32 *pcnt)
{
    rc5->has_last_key = 1;
    if (memcpy_s(&rc5->last_key, sizeof(key_attr), &rc5->this_key, sizeof(key_attr))) {
        hiir_error("memcpy_s to rc5->last_key failed\n");
        return;
    }

    if (g_time_out_flag == HI_TRUE) {
        g_time_out_flag = HI_FALSE;
        rc5->last_key.key_stat = KEY_STAT_UP;
        if (ir->key_up_event) {
            ir_insert_key_tail(wr, &rc5->last_key);
        }
        *pcnt = *pcnt + 1;
    } else {
        if (ip->key_hold_timeout_time == 0) {
            g_rc5_timer[ip->priv].expires = jiffies +
                                          msecs_to_jiffies(ir->key_hold_timeout_time);
        } else {
            g_rc5_timer[ip->priv].expires = jiffies +
                                          msecs_to_jiffies(ip->key_hold_timeout_time);
        }

        g_rc5_timer[ip->priv].data = ip->priv;
        add_timer(&g_rc5_timer[ip->priv]);
    }

    return;
}

/* extended rc5 handles */
hi_s32 parse_extended_rc5(ir_priv *ir, ir_protocol *ip,
                          ir_buffer *rd, ir_buffer *wr)
{
    ir_signal    ir_pulse;
    ir_signal    ir_space;
    rc5_ir       *rc5 = NULL;
    key_attr     *symbol = NULL;
    hi_s32       ret;
    hi_s32       i;
    hi_s32       fail;
    hi_s32       cnt = 0;
    hi_s32       flag = 0;
    hi_s32       flag2 = 0;

    if (ip->priv >= MAX_RC5_INFR_NR) {
        hiir_error("ip->pirv > MAX_RC5_INFR_NR!\n");
        return -1;
    }

    rc5 = &g_rc5_data[ip->priv];
    symbol = &rd->buf[rd->reader];
    fail = i = 0;
    rc5->state = STATE_INACTIVE;
    flag = symbol && symbol->lower && symbol->upper && !fail;
    while (flag) {
        hiir_debug("%s->%d, parse symbol [l, u]:[%d, %d] at %d\n",
                   __func__, __LINE__,
                   (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                   rd->reader);

        ir_pulse.duration = symbol->lower;
        ir_pulse.pulse = 1;
        ret = ir_rc5_decode(rc5, ir_pulse, ip);
        if (ret) {
            fail++;
        }
        rc5->prev_signal  = ir_pulse;
        ir_space.duration = symbol->upper;
        ir_space.pulse = 0;
        ret = ir_rc5_decode(rc5, ir_space, ip);
        if (ret) {
            fail++;
        }
        rc5->prev_signal = ir_space;
        symbol = ir_next_reader_clr_inc(rd);
        if ((rc5->state == STATE_FINISHED) ||
                (rc5->state == STATE_INACTIVE)) {
            break;
        }
        flag = symbol && symbol->lower && symbol->upper && !fail;
    }

    if (fail) {
        hiir_info("errors occured while decod rc5 frame," \
                  " discard this frame!\n");
        return -1;
    }

    if (memcpy_s(rc5->this_key.protocol_name, PROTOCOL_NAME_SZ, ip->ir_code_name, PROTOCOL_NAME_SZ)) {
        hiir_error("memcpy_s to rc5->this_key.protocol_name failed\n");
        return -1;
    }

    del_timer_sync(&g_rc5_timer[ip->priv]);

    /* if a repeat key. */
    flag2 = rc5->has_last_key &&
            ((rc5->this_key.lower == rc5->last_key.lower) &&
            (rc5->this_key.upper == rc5->last_key.upper)) &&
            (rc5->last_key.key_stat != KEY_STAT_UP);
    if (flag2) {
        if (ir->key_repeat_event &&
                time_after(jiffies,
                           g_rc5_repeat_next_time[ip->priv])) {
            rc5->last_key.key_stat = KEY_STAT_HOLD;
            ir_insert_key_tail(wr, &rc5->last_key);
            g_rc5_repeat_next_time[ip->priv] = jiffies +
                                             msecs_to_jiffies(ir->key_repeat_interval);
            cnt++;
        }

        goto OUT;
    }

    /* if a new key recevied, send a key up event of last key. */
    if (rc5->has_last_key &&
            (rc5->last_key.key_stat != KEY_STAT_UP) &&
            ((rc5->last_key.lower != rc5->this_key.lower) ||
            (rc5->last_key.upper != rc5->this_key.upper))) {
        rc5->last_key.key_stat = KEY_STAT_UP;
        if (ir->key_up_event) {
            ir_insert_key_tail(wr, &rc5->last_key);
        }
        cnt++;
    }

    /* the new key */
    rc5->this_key.key_stat = KEY_STAT_DOWN;
    ir_insert_key_tail(wr, &rc5->this_key);
    cnt++;
    g_rc5_repeat_next_time[ip->priv] = jiffies +
                                     msecs_to_jiffies(ir->key_repeat_interval);
OUT:
    extended_rc5_out_process(ir, ip, rc5, wr, &cnt);

    return cnt ? 0 : -1;
}

ir_match_result extended_rc5_match(ir_match_type type,
                                   ir_buffer *head,
                                   ir_protocol *ip)
{
    key_attr  *symbol;
    hi_s32    n;

    symbol = &head->buf[head->reader];
    if (!symbol->upper && !symbol->lower) {
        return IR_MATCH_NOT_MATCH;
    }

    switch (type) {
        case IR_MTT_HEADER:
            hiir_debug("rc5, header matching. symbol[l,u]:[%d, %d]," \
                       "header:[p, s, f]:[%d, %d, %d]\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.header.pluse, ip->attr.header.space,
                       ip->attr.header.factor);

            /* extended rc5's header may show up as [1750, 1750, 20%] or [1750, 889, 20%] */
            /* check first part 1750(20%) */
            if (data_fallin((hi_u32)symbol->lower, (hi_u32)ip->attr.header.minp, (hi_u32)ip->attr.header.maxp) &&
                    (data_fallin(symbol->upper, ip->attr.header.minp, ip->attr.header.maxp) ||
                     data_fallin(symbol->upper, RC5_UNIT * (100 - ip->attr.header.factor) / 100,
                                 RC5_UNIT * (100 + ip->attr.header.factor) / 100))) {
                hiir_debug("rc5, header matching: match!\n");
                return IR_MATCH_MATCH;
            }

            hiir_debug("rc5, header matching: header mismatch!\n");
            break;
        case IR_MTT_FRAME:
            hiir_debug("rc5, frame matching. symbol[l,u]:[%d, %d]," \
                       "header:[p, s, f]:[%d, %d, %d]\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.header.pluse, ip->attr.header.space,
                       ip->attr.header.factor);

            /* extended rc5's header may show up as [1750, 1750, 20%] or [1750, 889, 20%] */
            /* check first part 1750(20%) */
            if (!data_fallin(symbol->lower, ip->attr.header.minp, ip->attr.header.maxp) ||
                    (!data_fallin(symbol->upper, ip->attr.header.minp, ip->attr.header.maxp) &&
                     !data_fallin(symbol->upper, RC5_UNIT * (100 - ip->attr.header.factor) / 100,
                                  RC5_UNIT * (100 + ip->attr.header.factor) / 100))) {
                hiir_debug("rc5, header matching: match!\n");
                return IR_MATCH_NOT_MATCH;
            }

            hiir_debug("rc5, frame matching, header match at:%d, [l, u]:[%d, %d]\n",
                       head->reader, (hi_u32)symbol->lower, (hi_u32)symbol->upper);

            /* try find burst. */
            n = head->reader + ip->attr.burst_offset + 1;
            if (n >= MAX_SYMBOL_NUM) {
                n -= MAX_SYMBOL_NUM;
            }
            symbol = &head->buf[n];

            /* ignore lower symbol. */
            if ((symbol->upper > ip->attr.burst.maxs) ||
                    data_fallin(symbol->upper,
                                ip->attr.burst.mins,
                                ip->attr.burst.maxs)) {
                hiir_debug("rc5, frame matching: burst match at %d." \
                           " symbol[l,u]:[%d, %d]," \
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
 * if extended_rc5_match return not match, this routine will be called.
 */
ir_match_result extended_rc5_match_error_handle(ir_match_type  type,
                                                ir_buffer   *head,
                                                ir_protocol *ip)
{
    key_attr    *symbol;
    hi_s32      n;
    hi_s32      i;

    symbol = &head->buf[head->reader];
    if (!symbol->upper && !symbol->lower) {
        return IR_MATCH_NOT_MATCH;
    }

    switch (type) {
        case IR_MTT_HEADER:
            hiir_debug("rc5, header matching. symbol[l,u]:[%d, %d]," \
                       "header:[p, s, f]:[%d, %d, %d]\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.header.pluse, ip->attr.header.space,
                       ip->attr.header.factor);

            /* extended rc5's header may show up as [1750, 1750, 20%] or [1750, 889, 20%] */
            /* check first part 1750(20%) */
            if (data_fallin((hi_u32)symbol->lower, (hi_u32)ip->attr.header.minp, (hi_u32)ip->attr.header.maxp) &&
                    (data_fallin((hi_u32)symbol->upper, (hi_u32)ip->attr.header.minp, (hi_u32)ip->attr.header.maxp) ||
                     data_fallin((hi_u32)symbol->upper, RC5_UNIT * (100 - ip->attr.header.factor) / 100,
                                 RC5_UNIT * (100 + ip->attr.header.factor) / 100))) {
                hiir_debug("rc5, header matching: match!\n");
                return IR_MATCH_MATCH;
            }

            hiir_debug("rc5, header matching: header mismatch!\n");
            break;
        case IR_MTT_FRAME:
            hiir_debug("rc5, frame matching. symbol[l,u]:[%d, %d]," \
                       "header:[p, s, f]:[%d, %d, %d]\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.header.pluse, ip->attr.header.space,
                       ip->attr.header.factor);

            /* extended rc5's header may show up as [1750, 1750, 20%] or [1750, 889, 20%] */
            /* check first part 1750(20%) */
            if (!data_fallin((hi_u32)symbol->lower, (hi_u32)ip->attr.header.minp, (hi_u32)ip->attr.header.maxp) ||
                    (data_fallin((hi_u32)symbol->upper, (hi_u32)ip->attr.header.minp, (hi_u32)ip->attr.header.maxp) &&
                     data_fallin((hi_u32)symbol->upper, RC5_UNIT * (100 - ip->attr.header.factor) / 100,
                                 RC5_UNIT * (100 + ip->attr.header.factor) / 100))) {
                hiir_debug("rc5, header matching: match!\n");
                return IR_MATCH_NOT_MATCH;
            }

            hiir_debug("rc5, frame matching, header match at:%d, [l, u]:[%d, %d]\n",
                       head->reader, (hi_u32)symbol->lower, (hi_u32)symbol->upper);

            /* try find burst. */
            for (i = head->reader + 1, n = 0; n < MAX_DATA_BITS; n++, i++) {
                if (i >= MAX_SYMBOL_NUM) {
                    i -= MAX_SYMBOL_NUM;
                }

                symbol = &head->buf[i];
                if (!symbol->upper && !symbol->lower) {
                    hiir_debug("%s->%d, needs more data\n",
                               __func__, __LINE__);
                    return IR_MATCH_NEED_MORE_DATA;
                }

                hiir_debug("rc5, frame matching. symbol[l,u]:[%d, %d] at %d," \
                           "burst:[p, s, f]:[%d, %d, %d]\n",
                           (hi_u32)symbol->lower, (hi_u32)symbol->upper, i,
                           ip->attr.burst.pluse, ip->attr.burst.space,
                           ip->attr.burst.factor);

                /* ignore lower symbol. */
                if ((symbol->upper > ip->attr.burst.maxs) ||
                     data_fallin(symbol->upper,
                                 ip->attr.burst.mins,
                                 ip->attr.burst.maxs)) {
                    hiir_debug("rc5, frame matching: burst match at %d. symbol[l,u]:[%d, %d]," \
                               "burst:[p, s, f]:[%d, %d, %d]\n", i,
                               (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                               ip->attr.burst.pluse, ip->attr.burst.space,
                               ip->attr.burst.factor);
                    if (n >= ip->attr.burst_offset) {
                        n = n - ip->attr.burst_offset;
                    } else {
                        n = ip->attr.burst_offset - n;
                    }

                    if (n <= RC5_BURST_OFFSET) {
                        hiir_debug(KERN_DEBUG "%s->%d, rc5 frame err matching :" \
                                   " burst at %d, (n=%d), frame match!\n",
                                   __func__, __LINE__, i, n);
                        return IR_MATCH_MATCH;
                    } else {
                        hiir_debug(KERN_DEBUG "%s->%d, rc5 frame err matching :" \
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
 * if extended_rc5_match returns !0, this routine can be called.
 */
hi_void extended_rc5_parse_error_handle(ir_priv *ir, ir_protocol *ip,
                                        ir_buffer *symb_head, ir_buffer *key_head)
{
    key_attr *symbol;

    symbol = &symb_head->buf[symb_head->reader];
    while ((symbol->upper || symbol->lower)) {
        /* clear symbols till burst appears. */
        hiir_debug("%s->%d, process symbol:[l,u] at %d:[%d, %d]\n",
                   __func__, __LINE__, symb_head->reader,
                   (hi_u32)symbol->lower, (hi_u32)symbol->upper);
        if ((symbol->upper > ip->attr.burst.maxs) ||
             data_fallin(symbol->upper,
                         ip->attr.burst.mins,
                         ip->attr.burst.maxs)) {
            (hi_void)ir_next_reader_clr_inc(symb_head);
            break;
        }

        symbol = ir_next_reader_clr_inc(symb_head);
    }
}

EXPORT_SYMBOL(rc5_match);
EXPORT_SYMBOL(parse_rc5);
EXPORT_SYMBOL(rc5_parse_error_handle);
EXPORT_SYMBOL(rc5_match_error_handle);
