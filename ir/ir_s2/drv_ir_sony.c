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
/* store the max & min value of pluse and space to minx and maxx */
#define mm_ps(phase, minp, maxp, mins, maxs) do { \
        minp = (phase)->minp; \
        maxp = (phase)->maxp; \
        mins = (phase)->mins; \
        maxs = (phase)->maxs; \
    } while (0)

static hi_ulong              g_sony_repeat_next_time[MAX_SONY_INFR_NR];
static key_attr              g_sony_last_key[MAX_SONY_INFR_NR];
static struct timer_list     g_sony_timer[MAX_SONY_INFR_NR];

static hi_s32 data_neither_margin(hi_u32 lower, hi_u32 upper,
                                  hi_u64 minp, hi_u64 maxp,
                                  hi_u64 mins, hi_u64 maxs)
{
    hi_s32 ret =(!data_fallin(lower, minp, maxp) ||
                 !data_fallin(upper, mins, maxs));
    return ret;
}

static hi_s32 data_both_margin(hi_u32 lower, hi_u32 upper,
                               hi_u64 minp, hi_u64 maxp,
                               hi_u64 mins, hi_u64 maxs)
{
    hi_s32 ret =(data_fallin(lower, minp, maxp) &&
                 data_fallin(upper, mins, maxs));
    return ret;
}

static hi_s32 data_margin_burst(hi_s32 fail, hi_u32 lower, hi_u32 upper,
                                ir_protocol *ip,
                                hi_u64 mins, hi_u64 maxs)
{
    hi_s32 ret = (hi_s32) (!fail && ((!data_fallin(lower, ip->attr.b0.minp, ip->attr.b0.maxp) &&
                           !data_fallin(lower, ip->attr.b1.minp, ip->attr.b1.maxp)) ||
                           (upper < maxs && !data_fallin(upper, mins, maxs))));
    return ret;
}

static hi_s32 check_null(hi_u32 first, hi_u32 second)
{
    hi_s32 ret = (hi_s32) (!first && !second);
    return ret;
}

static hi_s32 check_repeat_key(key_attr *last_key, key_attr key)
{
    hi_s32 ret = (hi_s32) ((last_key->upper || last_key->lower) &&
                           last_key->upper == key.upper &&
                           last_key->lower == key.lower &&
                           last_key->key_stat != KEY_STAT_UP);
    return ret;
}

static hi_s32 last_key_exist(key_attr *last_key, key_attr key)
{
    hi_s32 ret = (hi_s32) ((last_key->upper || last_key->lower) &&
                            last_key->key_stat != KEY_STAT_UP &&
                            (last_key->upper != key.upper ||
                            last_key->lower != key.lower));
    return ret;
}

static hi_void sony_keyup_proc(hi_ulong i)
{
    key_attr       *last_key = NULL;
    if (i >= MAX_SONY_INFR_NR) {
        hiir_error("sony keyup timer, i > MAX_SONY_INFR_NR!\n");
        return;
    }

    last_key = &g_sony_last_key[i];
    if ((last_key->lower || last_key->upper) &&
         last_key->key_stat != KEY_STAT_UP) {
        last_key->key_stat = KEY_STAT_UP;
        if (g_ir_local.key_up_event) {
            ir_insert_key_tail(g_ir_local.key_buf, last_key);
            wake_up_interruptible(&g_ir_local.read_wait);
        }
        last_key->lower = last_key->upper = 0;
    }
}

hi_void sony_init(hi_void)
{
    hi_s32 i;
    for (i = 0; i < MAX_SONY_INFR_NR; i++) {
        init_timer(&g_sony_timer[i]);
        g_sony_timer[i].expires = 0;
        g_sony_timer[i].data = (hi_ulong)~0;
        g_sony_timer[i].function = sony_keyup_proc;
    }

    if (memset_s(g_sony_last_key, MAX_SONY_INFR_NR * sizeof(key_attr), 0,
                 MAX_SONY_INFR_NR * sizeof(key_attr))) {
        hiir_error("memset_s g_sony_last_key failed\n");
        return;
    }
}

hi_void sony_exit(hi_void)
{
    hi_s32 i;
    for (i = 0; i < MAX_SONY_INFR_NR; i++) {
        del_timer_sync(&g_sony_timer[i]);
    }
}

#define print_mm_sp(phase, name) do { \
        hiir_debug("%s: minp:%d, maxp:%d, mins:%d, maxp:%d\n", \
            name,(phase)->minp, (phase)->maxp, (phase)->mins, \
            (phase)->maxs); \
    } while(0)

ir_match_result sony_check_header(ir_buffer *buf, ir_protocol *ip, key_attr *key)
{
    if (check_null(key->upper, key->lower)) {
        return IR_MATCH_NOT_MATCH;
    }
    /* is a frame header? */
    if (key_match_phase(key, &ip->attr.header) == 0) {
        return IR_MATCH_MATCH;
    }
    hiir_debug("%s->%d, header not match! ip at:%p" \
               " header[p, s, f]: [%d, %d, %d]," \
               " key[l, u]:[%d, %d]\n",
               __func__, __LINE__, ip,
               ip->attr.header.pluse,
               ip->attr.header.space,
               ip->attr.header.factor,
               (hi_u32)key->lower, (hi_u32)key->upper);
    print_mm_sp(&ip->attr.header, "header");
    return IR_MATCH_NOT_MATCH;
}

ir_match_result sony_check_frame(ir_buffer *buf, ir_protocol *ip,key_attr *key, hi_s32 idx)
{
    hi_u64            minp;
    hi_u64            maxp;
    hi_s32            i;
    hi_s32            j;
    hi_s32            n;
    hiir_debug("idx:%d, checking symbol(at %d)[%d, %d]!\n",
               idx, buf->reader,
               (hi_u32)buf->buf[buf->reader].lower,
               (hi_u32)buf->buf[buf->reader].upper
              );

    if (check_null(key->upper, key->lower)) {
        hiir_debug("%s->%d, idx:%d, key empty!\n", __func__, __LINE__, idx);
        return IR_MATCH_NOT_MATCH;
    }
    /* header match? */
    hiir_debug("%s->%d, idx:%d, checking header!\n", __func__, __LINE__, idx);
    if (key_match_phase(key, &ip->attr.header)) {
        hiir_debug("%s->%d, header not match! ip at:%p"\
                   " header[p, s, f]: [%d, %d, %d],"\
                   " key[l, u]:[%d, %d]\n",
                   __func__, __LINE__, ip,
                   ip->attr.header.pluse,
                   ip->attr.header.space,
                   ip->attr.header.factor,
                   (hi_u32)key->lower, (hi_u32)key->upper);
        return IR_MATCH_NOT_MATCH;
    } else {
        n = ip->attr.burst_offset;
    }
    /* try find burst. */
    hiir_debug("%s->%d,idx:%d header match!\n", __func__, __LINE__, idx);
    n = buf->reader + n;
    if (n >= MAX_SYMBOL_NUM) {
        n -= MAX_SYMBOL_NUM;
    }
    /* check frame symbols */
    for (i = 0, j = i + buf->reader + 1; i < ip->attr.wanna_bits && j != n;
            i++, j++) {

        if (j >= MAX_SYMBOL_NUM) {
            j -= MAX_SYMBOL_NUM;
        }

        if (j == n) {
            break;
        }

        key = &buf->buf[j];

        if (check_null(key->upper, key->lower)) {
            return IR_MATCH_NEED_MORE_DATA;
        }

        /* check data phase is exceed or not */
        if (key_match_phase(key, &ip->attr.b0)
                && key_match_phase(key, &ip->attr.b1)) {
            return IR_MATCH_NOT_MATCH;
        }
    }

    hiir_debug("%s->%d, checking burst at(%d)!\n",
               __func__, __LINE__, n);
    key = &buf->buf[n];
    if (check_null(key->upper, key->lower)) {
        hiir_debug("%s->%d, idx:%d, needs more data\n",
                   __func__, __LINE__, idx);
        return IR_MATCH_NEED_MORE_DATA;
    }

    /* burst match? */
    /*
     * SONY frame burst may constains b0's pluse or b1's pluse
     * and space will > 20000.
     */
    minp = ip->attr.b0.minp;
    maxp = ip->attr.b1.maxp;
    if (((hi_u32)key->lower >= minp && (hi_u32)key->lower <= maxp) &&
         (hi_u32)key->upper > ip->attr.burst.space) {

        hiir_debug("%s->%d, idx:%d, frame burst match!\n",
                   __func__, __LINE__, idx);
        return IR_MATCH_MATCH;
    } else {
        hiir_debug("%s->%d, idx:%d. burst not match!"
                   " key[l, u][%d, %d],"
                   " burst[p, s, f]: [%d, %d, %d]\n",
                   __func__, __LINE__, idx,
                   (hi_u32)key->lower, (hi_u32)key->upper,
                   ip->attr.burst.pluse,
                   ip->attr.burst.space,
                   ip->attr.burst.factor);

        return IR_MATCH_NOT_MATCH;
    }
}

/* to see a frame is a full frame or a repeat frame */
ir_match_result sony_match(ir_match_type type,
                                   ir_buffer *buf, ir_protocol *ip)
{
    key_attr         *key = NULL;
    hi_s32           idx;
    ir_match_result  ret = 0;

    idx = ip->priv;
    if (idx >= MAX_SONY_INFR_NR) {
        hiir_error("sony , private data error!\n");
        return IR_MATCH_NOT_MATCH;
    }
    key = &buf->buf[buf->reader];
    switch (type) {
        case IR_MTT_HEADER: {
            ret = sony_check_header(buf, ip, key);
            return ret;
        }
        case IR_MTT_FRAME: {
            ret = sony_check_frame(buf, ip, key, idx);
            return ret;
        }
        case IR_MTT_BURST:
        /* fall though */
        default:
            return IR_MATCH_NOT_MATCH;
    }
}
hi_s32 sony_frame_full_parse(ir_priv *ir, ir_protocol *ip,
                             ir_buffer *rd, ir_buffer *wr)
{
    key_attr           *symbol = NULL;
    key_attr           key;
    hi_u32             i;
    hi_u64             minp;
    hi_u64             maxp;
    hi_u64             mins;
    hi_u64             maxs;
    key_attr           *last_key = NULL;
    hi_s32             cnt = 0;
    hi_s32             fail = 0;
    hi_s32             idx = ip->priv;
    if (idx >= MAX_SONY_INFR_NR) {
        hiir_error("sony , private data error!\n");
        goto OUT;
    }
    del_timer_sync(&g_sony_timer[idx]);
    last_key = &g_sony_last_key[idx];

    /* header phase */
    if (memset_s(&key, sizeof(key_attr), 0, sizeof(key_attr))) {
        hiir_error("memset_s key failed\n");
        goto OUT;
    }

    hiir_debug("try parse header(at %d)!\n", rd->reader);
    symbol = &rd->buf[rd->reader];
    mm_ps(&ip->attr.header, minp, maxp, mins, maxs);
    /* frame start? */
    if (data_neither_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper,
                            minp, maxp, mins, maxs)) {
        hiir_error("SONY : cannot parse!!!"\
                   " header phase not match. symbol[l,u]: [%d, %d],"\
                   " header phase[p, s, f]: [%d, %d, %d]."\
                   " repeat phase[p, s, f]: [%d, %d, %d].\n",
                   (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                   ip->attr.header.pluse, ip->attr.header.space,
                   ip->attr.header.factor,
                   ip->attr.repeat.pluse, ip->attr.repeat.space,
                   ip->attr.repeat.factor);
        goto OUT;
    }

    /* data phase */
    if (memcpy_s(key.protocol_name, PROTOCOL_NAME_SZ, ip->ir_code_name, PROTOCOL_NAME_SZ)) {
        hiir_error("memcpy_s to key.protocol_name failed\n");
        goto OUT;
    }

    symbol = ir_next_reader_clr_inc(rd);
    hiir_debug("try parse data(at %d)!\n", rd->reader);
    i = 0;
    while (symbol->upper && symbol->lower && i < ip->attr.wanna_bits) {
        mm_ps(&ip->attr.b0, minp, maxp, mins, maxs);
        if (data_both_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            symbol = ir_next_reader_clr_inc(rd);
            i++;
            continue;
        }

        mm_ps(&ip->attr.b1, minp, maxp, mins, maxs);
        if (data_both_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            if (i < 64) {
                key.lower |= (hi_u64)(((hi_u64)1) << i);
            } else {
                key.upper |= (hi_u64)(((hi_u64)1) << (i - 64));
            }
            symbol = ir_next_reader_clr_inc(rd);
            i++;
            continue;
        }
        /* meets burst. */
        if (i + 1 == ip->attr.wanna_bits) {
            symbol = &rd->buf[rd->reader];
            hiir_debug("%s->%d, meets burst, i:%d, ip->attr.wanna_bits:%d, rd->reader:%d!\n",
                       __func__, __LINE__, i, ip->attr.wanna_bits, rd->reader);
            mm_ps(&ip->attr.b1, minp, maxp, mins, maxs);
            if (data_fallin((hi_u32)symbol->lower, minp, maxp)) {
                hiir_debug("%s->%d, burst constains bit1!\n", __func__, __LINE__);
                if (i < 64) {
                    key.lower |= (hi_u64)(((hi_u64)1) << i);
                } else {
                    key.upper |= (hi_u64)(((hi_u64)1) << (i - 64));
                }
                i++;
                break;
            }
            mm_ps(&ip->attr.b0, minp, maxp, mins, maxs);
            if (data_fallin((hi_u32)symbol->lower, minp, maxp)) {
                hiir_debug("%s->%d, burst constains bit0!\n", __func__, __LINE__);
                i++;
                break;
            }
        }
        hiir_info("SONY : unkown symbol[l, u]: [%d, %d],"\
                  " b0[p, s, f]: [%d, %d, %d],"\
                  " b1[p, s, f]: [%d, %d, %d]. assume to 0!\n",
                  (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                  ip->attr.b0.pluse, ip->attr.b0.space,
                  ip->attr.b0.factor,
                  ip->attr.b1.pluse, ip->attr.b1.space,
                  ip->attr.b1.factor);
        i++;
        symbol = ir_next_reader_clr_inc(rd);
        fail ++;
    }
    /* burst */
    hiir_debug("try parse burst(at %d)!\n", rd->reader);
    mm_ps(&ip->attr.burst, minp, maxp, mins, maxs);
    if (data_margin_burst(fail, (hi_u32)symbol->lower,
                          (hi_u32)symbol->upper, ip, mins, maxs)) {
        hiir_info("SONY : unkown symbol[l, u]: [%d, %d],"\
                  " burst[p, s, f]: [%d, %d, %d].\n",
                  (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                  ip->attr.burst.pluse, ip->attr.burst.space,
                  ip->attr.burst.factor);
        fail++;
        goto OUT;
    } else {
        if (fail) {
            (hi_void)ir_next_reader_clr_inc(rd);
            goto OUT;
        }
        /* if a repeat key. */
        if (check_repeat_key(last_key, key)) {
            /* should i send a repeat key? */
            if (ir->key_repeat_event && time_after(jiffies,
                                                   g_sony_repeat_next_time[idx])) {
                last_key->key_stat = KEY_STAT_HOLD;
                ir_insert_key_tail(wr, last_key);
                (hi_void)ir_next_reader_clr_inc(rd);
                g_sony_repeat_next_time[idx] = jiffies +
                                               msecs_to_jiffies(
                                                 ir->key_repeat_interval);
            }
            cnt++;
            goto START_TIMER_OUT;
        }
        /*
         * a new key received.
         * send a up event of last key if exist.
         */
        if (last_key_exist(last_key, key)) {
            last_key->key_stat = KEY_STAT_UP;
            if (ir->key_up_event) {
                ir_insert_key_tail(wr, last_key);
            }
            cnt++;
        }

        hiir_debug("key parsed:[l:%llu, u:%llu, s:%d, p:%s]!\n",
                   key.lower, key.upper, key.key_stat, key.protocol_name);

        key.key_stat = KEY_STAT_DOWN;
        ir_insert_key_tail(wr, &key);
        (hi_void)ir_next_reader_clr_inc(rd);
        g_sony_repeat_next_time[idx] = jiffies +
                                       msecs_to_jiffies(ir->key_repeat_interval);

        if (memcpy_s(last_key, sizeof(key_attr), &key, sizeof(key_attr))) {
            hiir_error("memcpy_s to last_key failed\n");
            goto OUT;
        }

        cnt++;
    }
START_TIMER_OUT:
    del_timer_sync(&g_sony_timer[idx]);

    if (g_time_out_flag == HI_TRUE) {
        g_time_out_flag = HI_FALSE;
        last_key->key_stat = KEY_STAT_UP;
        if (ir->key_up_event) {
            ir_insert_key_tail(wr, last_key);
        }
        cnt++;
    } else {
        if (ip->key_hold_timeout_time == 0) {
            g_sony_timer[idx].expires = jiffies +
                                      msecs_to_jiffies(ir->key_hold_timeout_time);
        } else {
            g_sony_timer[idx].expires = jiffies +
                                      msecs_to_jiffies(ip->key_hold_timeout_time);
        }

        g_sony_timer[idx].data = idx;
        add_timer(&g_sony_timer[idx]);
    }

OUT:
    return fail ? -1 : (cnt ? 0 : -1);
}
EXPORT_SYMBOL(sony_match);
EXPORT_SYMBOL(sony_frame_full_parse);
