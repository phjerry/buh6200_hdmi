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

static hi_ulong           g_tc9012_repeat_next_time[MAX_TC9012_INFR_NR];
static key_attr           g_tc9012_last_key[MAX_TC9012_INFR_NR];
static struct timer_list  g_tc9012_timer[MAX_TC9012_INFR_NR];
static hi_u32             g_tc9012_redundant_frame_flag = 0;

static hi_s32 data_neither_margin(hi_u32 lower, hi_u32 upper,
                                  hi_u64 minp, hi_u64 maxp,
                                  hi_u64 mins, hi_u64 maxs)
{
    hi_s32 ret = (!data_fallin(lower, minp, maxp) ||
                  !data_fallin(upper, mins, maxs));
    return ret;
}

static hi_s32 data_both_margin(hi_u32 lower, hi_u32 upper,
                               hi_u64 minp, hi_u64 maxp,
                               hi_u64 mins, hi_u64 maxs)
{
    hi_s32 ret = (data_fallin(lower, minp, maxp) &&
                  data_fallin(upper, mins, maxs));
    return ret;
}

static hi_s32 data_either_smaller(hi_u32 lower, hi_u32 upper,
                                  hi_u64 minp, hi_u64 maxp,
                                  hi_u64 mins, hi_u64 maxs)
{
    hi_s32 ret = (!data_fallin(lower, minp, maxp) ||
                 (!data_fallin(upper, mins, maxs) && upper < maxs));
    return ret;
}

static hi_s32 data_margin_bigger(hi_u32 lower, hi_u32 upper,
                                 hi_u64 minp, hi_u64 maxp,
                                 hi_u64 mins, hi_u64 maxs)
{
    hi_s32 ret = (data_fallin(lower, minp, maxp) &&
                 (data_fallin(upper, mins, maxs) || upper > maxs));
    return ret;
}

static hi_s32 check_repeat_key(key_attr *last_key, key_attr key)
{
    hi_s32 ret = (hi_s32) ((last_key->upper || last_key->lower) &&
                           (last_key->upper == key.upper) &&
                           (last_key->lower == key.lower) &&
                           (last_key->key_stat != KEY_STAT_UP));
    return ret;
}

static hi_s32 last_key_exist(key_attr *last_key, key_attr key)
{
    hi_s32 ret = (hi_s32) ((last_key->upper || last_key->lower) &&
                           (last_key->key_stat != KEY_STAT_UP) &&
                           ((last_key->upper != key.upper) ||
                           (last_key->lower != key.lower)));
    return ret;
}

static hi_s32 check_last_key(hi_u32 idx)
{
    hi_s32 ret = (hi_s32) ((g_tc9012_last_key[idx].lower || g_tc9012_last_key[idx].upper) &&
                          (g_tc9012_last_key[idx].key_stat != KEY_STAT_UP));
    return ret;
}

static hi_s32 key_stat_time(hi_s32 fail,key_attr *key, hi_s32 event, hi_u32 idx)
{
    hi_s32 ret = (hi_s32) (!fail && (key->upper || key->lower) &&
                           event && time_after(jiffies,
                                               g_tc9012_repeat_next_time[idx]));
    return ret;
}

static hi_s32 check_null(hi_u32 first, hi_u32 second)
{
    hi_s32 ret = (hi_s32) (!first && !second);
    return ret;
}

static hi_void tc9012_keyup_proc(hi_ulong i)
{
    key_attr *last_key = NULL;
    if (i >= MAX_TC9012_INFR_NR) {
        hiir_error("tc9012 keyup timer, i > MAX_TC9012_INFR_NR!\n");
        return;
    }

    last_key = &g_tc9012_last_key[i];
    if ((last_key->lower || last_key->upper) &&
        last_key->key_stat != KEY_STAT_UP) {
        last_key->key_stat = KEY_STAT_UP;
        g_tc9012_redundant_frame_flag = 0;
        if (g_ir_local.key_up_event) {
            ir_insert_key_tail(g_ir_local.key_buf, last_key);
            wake_up_interruptible(&g_ir_local.read_wait);
        }
        last_key->lower = last_key->upper = 0;
    }
}

hi_void tc9012_init(hi_void)
{
    hi_s32 i;
    for (i = 0; i < MAX_TC9012_INFR_NR; i++) {
        init_timer(&g_tc9012_timer[i]);
        g_tc9012_timer[i].expires = 0;
        g_tc9012_timer[i].data = (hi_ulong)~0;
        g_tc9012_timer[i].function = tc9012_keyup_proc;
    }

    if (memset_s(g_tc9012_last_key, MAX_TC9012_INFR_NR * sizeof(key_attr), 0,
                 MAX_TC9012_INFR_NR * sizeof(key_attr))) {
        hiir_error("memset_s g_tc9012_last_key failed\n");
        return;
    }
}

hi_void tc9012_exit(hi_void)
{
    hi_s32 i;
    for (i = 0; i < MAX_TC9012_INFR_NR; i++) {
        del_timer_sync(&g_tc9012_timer[i]);
    }
}

#define print_mm_sp(phase, name) do { \
        hiir_debug("%s: minp:%d, maxp:%d, mins:%d, maxp:%d\n", \
            name,(phase)->minp, (phase)->maxp, (phase)->mins, \
            (phase)->maxs); \
    } while (0)

ir_match_result tc9012_check_header(ir_buffer *buf, ir_protocol *ip, key_attr *key)
{
    if (check_null(key->upper, key->lower)) {
        return IR_MATCH_NOT_MATCH;
    }
    /* is a frame header? */
    if (key_match_phase(key, &ip->attr.header) == 0) {
        return IR_MATCH_MATCH;
    }
    hiir_debug("%s->%d, header not match! ip at:%p"\
               " header[p, s, f]: [%d, %d, %d],"\
               " key[l, u]:[%d, %d]\n",
               __func__, __LINE__, ip,
               ip->attr.header.pluse,
               ip->attr.header.space,
               ip->attr.header.factor,
               (hi_u32)key->lower, (hi_u32)key->upper);
    print_mm_sp(&ip->attr.header, "header");
    /* repeate frame match? */
    if (check_null(ip->attr.repeat.pluse, ip->attr.repeat.space)) {
        return IR_MATCH_NOT_MATCH;
    }
    if (key_match_phase(key, &ip->attr.repeat) == 0) {
        return IR_MATCH_MATCH;
    }
    hiir_debug("%s->%d, repeat not match! ip at:%p"\
               " repeat[p, s, f]: [%d, %d, %d],"\
               " key[l, u]:[%d, %d]\n",
               __func__, __LINE__, ip,
               ip->attr.repeat.pluse,
               ip->attr.repeat.space,
               ip->attr.repeat.factor,
               (hi_u32)key->lower, (hi_u32)key->upper);

    print_mm_sp(&ip->attr.repeat, "repeat");
    return IR_MATCH_NOT_MATCH;
}

ir_match_result tc9012_check_frame(ir_buffer *buf, ir_protocol *ip, key_attr *key, hi_s32 idx)
{
    hi_u64        minp;
    hi_u64        maxp;
    hi_u64        mins;
    hi_u64        maxs;
    hi_s32        i;
    hi_s32        j;
    hi_s32        n;
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
    }
    /* repeate frame match? */
    if (check_null(ip->attr.repeat.pluse, ip->attr.repeat.space)) {
        return IR_MATCH_NOT_MATCH;
    }
    hiir_debug("%s->%d, idx:%d, checking repeat!\n", __func__, __LINE__, idx);
    if (key_match_phase(key, &ip->attr.repeat)) {
        hiir_debug("%s->%d, idx:%d, repeat not match!\n",
                   __func__, __LINE__, idx);
        return IR_MATCH_NOT_MATCH;
    } else {
        /* if this is repeate key, then check burst here. */
        n = 2;
        n = buf->reader + n;
        n %= MAX_SYMBOL_NUM;

        key = &buf->buf[n];
        hiir_debug("%s->%d, idx:%d, checking repeat burst(at %d)[%d, %d]!\n",
                   __func__, __LINE__, idx, n, (hi_u32)key->lower, (hi_u32)key->upper);
        if (check_null(key->upper, key->lower)) {
            hiir_debug("%s->%d, checking repeat burst, need more symbols!\n",
                       __func__, __LINE__);
            return IR_MATCH_NEED_MORE_DATA;
        }
        mm_ps(&ip->attr.burst, minp, maxp, mins, maxs);
        if (data_margin_bigger((hi_u32)key->lower, (hi_u32)key->upper,
                               minp, maxp, mins, maxs)) {
            /*
             * case a frame only constains repeate key, no first frame,
             * we cannot recognize the key value, so dicard this repeat key.
             */
            if (check_last_key(idx)) {
                hiir_debug("%s->%d, idx:%d, repeat burst match!\n",
                           __func__, __LINE__, idx);
                return IR_MATCH_MATCH;
            } else {
                hiir_debug("%s->%d, idx:%d, repeat burst not match!\n",
                           __func__, __LINE__, idx);
                return IR_MATCH_NOT_MATCH;
            }
        }
    }
    /* the first frame */
    n = ip->attr.wanna_bits + 1;
    /* try find burst. */
    hiir_debug("%s->%d,idx:%d header match!\n", __func__, __LINE__, idx);
    n = buf->reader + n;
    n %= MAX_SYMBOL_NUM;

    hiir_debug("%s->%d, checking burst at(%d)!\n",
               __func__, __LINE__, n);
    key = &buf->buf[n];
    if (check_null(key->upper, key->lower)) {
        hiir_debug("%s->%d, idx:%d, needs more data\n",
                   __func__, __LINE__, idx);
        return IR_MATCH_NEED_MORE_DATA;
    }

    /*
     * burst match?
     * TC9012 frame burst may fall in [space ,8%],
     * but it still can greater than space !
     */
    mm_ps(&ip->attr.burst, minp, maxp, mins, maxs);
    if (data_margin_bigger((hi_u32)key->lower, (hi_u32)key->upper,
                           minp, maxp, mins, maxs)) {
        hiir_debug("%s->%d, checking frame!\n",
                   __func__, __LINE__);
        /* check frame symbols */
        for (i = 0, j = i + buf->reader + 1; i < ip->attr.wanna_bits && j != n;
                i ++, j ++) {
            j %= MAX_SYMBOL_NUM;
            if (j == n) {
                break;
            }
            key = &buf->buf[j];

            if (check_null(key->lower, key->upper)) {
                hiir_error("%s->%d, need more symbols BUT BURST appears!\n",
                           __func__, __LINE__);
                return IR_MATCH_NEED_MORE_DATA;
            }

            /* check data phase is exceed or not */
            if (key_match_phase(key, &ip->attr.b0) &&
                key_match_phase(key, &ip->attr.b1)) {
                hiir_debug("%s->%d, symbol[%d, %d] not exceed"\
                           " b0 or b1's max space or pluse!\n",
                           __func__, __LINE__,
                           (hi_u32)key->upper, (hi_u32)key->lower);
                return IR_MATCH_NOT_MATCH;
            }
        }
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
ir_match_result tc9012_match(ir_match_type type,
                             ir_buffer *buf, ir_protocol *ip)
{
    key_attr             *key = NULL;
    hi_s32               idx;
    ir_match_result      ret = 0;
    idx = ip->priv;
    if (idx >= MAX_TC9012_INFR_NR) {
        hiir_error("tc9012 , private data error!\n");
        return IR_MATCH_NOT_MATCH;
    }
    key = &buf->buf[buf->reader];
    switch (type) {
        case IR_MTT_HEADER: {
            ret = tc9012_check_header(buf, ip, key);
            return ret;
        }
        case IR_MTT_FRAME: {
            ret = tc9012_check_frame(buf, ip, key, idx);
            return ret;
        }
        case IR_MTT_BURST:
        /* fall though */
        default:
            return IR_MATCH_NOT_MATCH;
    }
}

hi_void tc9012_process_timeout(ir_priv *ir, ir_protocol *ip, key_attr *last_key,
                               ir_buffer *wr, hi_u32 idx, hi_s32 *pcnt)
{

    del_timer_sync(&g_tc9012_timer[idx]);

    if (g_time_out_flag == HI_TRUE) {
        g_time_out_flag = HI_FALSE;
        last_key->key_stat = KEY_STAT_UP;
        g_tc9012_redundant_frame_flag = 0;
        if (ir->key_up_event) {
            ir_insert_key_tail(wr, last_key);
        }
        *pcnt = *pcnt + 1;
    } else {
        if (ip->key_hold_timeout_time == 0) {
            g_tc9012_timer[idx].expires = jiffies +
                                        msecs_to_jiffies(ir->key_hold_timeout_time);
        } else {
            g_tc9012_timer[idx].expires = jiffies +
                                        msecs_to_jiffies(ip->key_hold_timeout_time);
        }

        g_tc9012_timer[idx].data = idx;
        add_timer(&g_tc9012_timer[idx]);
    }
    return;
}

hi_s32 tc9012_frame_simple_parse(ir_priv *ir, ir_protocol *ip,
                                 ir_buffer *rd, ir_buffer *wr)
{
    key_attr           *symbol = NULL;
    key_attr           *symbol2 = NULL;
    key_attr           key;
    hi_u64             minp;
    hi_u64             maxp;
    hi_u64             mins;
    hi_u64             maxs;
    hi_u32             i;
    hi_u32             n;
    key_attr           *last_key = NULL;
    hi_s32             cnt = 0;
    hi_s32             fail = 0;
    hi_s32             flsg = 0;
    hi_s32             flag2 = 0;
    hi_u32             idx = ip->priv;
    if (idx >= MAX_TC9012_INFR_NR) {
        hiir_error("tc9012 , private data error!\n");
        goto OUT;
    }
    del_timer_sync(&g_tc9012_timer[idx]);
    last_key = &g_tc9012_last_key[idx];

    /* header phase */
    if (memset_s(&key, sizeof(key_attr), 0, sizeof(key_attr))) {
        hiir_error("memset_s key failed\n");
        goto OUT;
    }

    hiir_debug("try parse header(at %d)!\n", rd->reader);
    symbol = &rd->buf[rd->reader];

    mm_ps(&ip->attr.header, minp, maxp, mins, maxs);
    /* frame start? */
    if (data_neither_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
        /* repeate key? */
        mm_ps(&ip->attr.repeat, minp, maxp, mins, maxs);
        if (data_both_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            goto REPEAT_KEY;
        }
        hiir_error("TC9012 : cannot parse!!!"\
                   " header phase not match. symbol[l,u]: [%d, %d]," \
                   " header phase[p, s, f]: [%d, %d, %d]." \
                   " repeat phase[p, s, f]: [%d, %d, %d].\n",
                   (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                   ip->attr.header.pluse, ip->attr.header.space,
                   ip->attr.header.factor,
                   ip->attr.repeat.pluse, ip->attr.repeat.space,
                   ip->attr.repeat.factor);
        goto OUT;
    }
    /* checking burst. if a repeat frame, burst will appear at current pos + 2. */
    n = (rd->reader + 2) % MAX_SYMBOL_NUM;
    symbol2 = &rd->buf[n];
    hiir_debug("%s->%d, checking burst at %d [%d, %d]\n",
               __func__, __LINE__, n, (hi_u32)symbol2->lower, (hi_u32)symbol2->upper);

    mm_ps(&ip->attr.burst, minp, maxp, mins, maxs);
    if (data_margin_bigger((hi_u32)symbol2->lower, (hi_u32)symbol2->upper, minp, maxp, mins, maxs)) {
        goto REPEAT_KEY;
    }

    /* data phase */
    if (memcpy_s(key.protocol_name, PROTOCOL_NAME_SZ, ip->ir_code_name, PROTOCOL_NAME_SZ)) {
        hiir_error("memcpy_s to key.protocol_name failed\n");
        goto OUT;
    }

    symbol = ir_next_reader_clr_inc(rd);
    hiir_debug("try parse data(at %d)!\n", rd->reader);
    i = 0;

    flsg = symbol->upper && symbol->lower && i < ip->attr.wanna_bits;
    while (flsg) {
        /* bit 0? */
        mm_ps(&ip->attr.b0, minp, maxp, mins, maxs);
        if (data_both_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            symbol = ir_next_reader_clr_inc(rd);
            i++;
            flsg = symbol->upper && symbol->lower && i < ip->attr.wanna_bits;
            continue;
        }
        /* bit 1? */
        mm_ps(&ip->attr.b1, minp, maxp, mins, maxs);
        if (data_both_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            key.lower |= (hi_u64)(((hi_u64)1) << i);
            symbol = ir_next_reader_clr_inc(rd);
            i++;
            flsg = symbol->upper && symbol->lower && i < ip->attr.wanna_bits;
            continue;
        }
        hiir_info("%s->%d, i:%d,TC9012 : unkown symbol[l, u]: [%d, %d],"\
                  " b0[p, s, f]: [%d, %d, %d],"\
                  " b1[p, s, f]: [%d, %d, %d]. discard this frame!\n",
                  __func__, __LINE__, i,
                  (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                  ip->attr.b0.pluse, ip->attr.b0.space,
                  ip->attr.b0.factor,
                  ip->attr.b1.pluse, ip->attr.b1.space,
                  ip->attr.b1.factor);
        i++;
        symbol = ir_next_reader_clr_inc(rd);
        fail++;
        flsg = symbol->upper && symbol->lower && i < ip->attr.wanna_bits;
    }
    /* burst */
    hiir_debug("try parse burst(at %d)!\n", rd->reader);
    mm_ps(&ip->attr.burst, minp, maxp, mins, maxs);
    if (data_either_smaller((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
        hiir_info("%s->%d, TC9012 : unkown symbol[l, u]: [%d, %d],"\
                  " burst[p, s, f]: [%d, %d, %d].\n",
                  __func__, __LINE__,
                  (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                  ip->attr.burst.pluse, ip->attr.burst.space,
                  ip->attr.burst.factor);
        fail++;
        goto OUT;
    } else {
        if (fail) {
            goto OUT;
        }

        /* if a repeat key. */
        if (check_repeat_key(last_key, key)) {
            /* should i send a repeat key? */
            g_tc9012_redundant_frame_flag++;
            if (ir->key_repeat_event &&
                time_after(jiffies, g_tc9012_repeat_next_time[idx]) &&
                (g_tc9012_redundant_frame_flag > 1)) {
                last_key->key_stat = KEY_STAT_HOLD;
                ir_insert_key_tail(wr, last_key);
                g_tc9012_repeat_next_time[idx] = jiffies +
                                               msecs_to_jiffies(ir->key_repeat_interval);

                cnt++;
            }

            goto START_TIMER_OUT;
        }

        /*
         * a new key received.
         * send a up event of last key if exist.
         */
        if (last_key_exist(last_key, key)) {
            last_key->key_stat = KEY_STAT_UP;
            g_tc9012_redundant_frame_flag = 0;
            if (ir->key_up_event) {
                ir_insert_key_tail(wr, last_key);
            }
            cnt++;
        }
        key.key_stat = KEY_STAT_DOWN;

        hiir_debug("key parsed:[l:0x%llx, u:0x%llx, s:%d, p:%s]!\n",
                   key.lower, key.upper, key.key_stat, key.protocol_name);
        if (fail == 0) {
            ir_insert_key_tail(wr, &key);
            cnt++;
            g_tc9012_repeat_next_time[idx] = jiffies +
                                           msecs_to_jiffies(ir->key_repeat_interval);

            if (memcpy_s(last_key, sizeof(key_attr), &key, sizeof(key_attr))) {
                hiir_error("memcpy_s to last_key failed\n");
                goto OUT;
            }

            hiir_debug("idx:%d, key parsed ,last_key[l,u,s] [0x%x,0x%x,%d],idx:%d\n",
                       idx, (hi_u32)last_key->lower, (hi_u32)last_key->upper,
                       last_key->key_stat,
                       idx);
        }
        symbol = ir_next_reader_clr_inc(rd);
    }
REPEAT_KEY:
    /* repeat? */
    do {
        ir_match_result r;
        hiir_debug("idx:%d,try parse repeat header(at %d)[%d, %d]!\n",
                   idx, rd->reader,
                   (hi_u32)ir->symbol_buf->buf[rd->reader].lower,
                   (hi_u32)ir->symbol_buf->buf[rd->reader].upper
                  );

        flag2 = symbol->upper && symbol->lower;
        /* checking a full repeat frame received or not. */
        r = ip->match(IR_MTT_FRAME, ir->symbol_buf, ip);
        if (r != IR_MATCH_MATCH) {
            hiir_debug("%s->%d, idx:%d repeat frame not match\n",
                       __func__, __LINE__, idx);
            goto START_TIMER_OUT;
        }
        mm_ps(&ip->attr.repeat, minp, maxp, mins, maxs);
        if (data_neither_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            hiir_debug("%s->%d, repeat burst not match!\n",
                       __func__, __LINE__);
            goto START_TIMER_OUT;
        }

        /* clear repeat header */
        (hi_void)ir_next_reader_clr_inc(rd);
        /* skip C0 bit in the repeat frame. */
        symbol = ir_get_read_key_n(rd, 1);
        /* burst */
        hiir_debug("%s->%d,idx:%d, try parse repeat burst(at %d)[%d, %d]!\n",
                   __func__, __LINE__,
                   idx, rd->reader, (hi_u32)symbol->lower, (hi_u32)symbol->upper);
        /*
         * TC9012 burst's space may fallin [space, 8%], but it also
         * may greate than maxp.
         */
        mm_ps(&ip->attr.burst, minp, maxp, mins, maxs);
        if (data_either_smaller((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            hiir_debug("TC9012 : repeat burst not match"\
                       "[l, u]: [%d, %d],"\
                       " burst[p, s, f]: [%d, %d, %d].\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.burst.pluse, ip->attr.burst.space,
                       ip->attr.burst.factor);
            /* clear C0 bit in the repeat frame. */
            (hi_void)ir_next_reader_clr_inc(rd);
        } else {
            /* clear C0 bit in the repeat frame. */
            (hi_void)ir_next_reader_clr_inc(rd);
            hiir_debug("repeat parsed ,last_key[l,u,s] [0x%x,0x%x,%d],"
                       "time_after:%d,idx:%d!\n",
                       (hi_u32)last_key->lower, (hi_u32)last_key->upper,
                       last_key->key_stat,
                       time_after(jiffies, g_tc9012_repeat_next_time[idx]),
                       idx);
            g_tc9012_redundant_frame_flag++;
            if (key_stat_time(fail, last_key, ir->key_repeat_event, idx) && (g_tc9012_redundant_frame_flag > 1)) {
                hiir_debug("insert repeat parsed !\n");

                last_key->key_stat =
                    last_key->key_stat == KEY_STAT_UP ?
                    KEY_STAT_DOWN : KEY_STAT_HOLD;

                ir_insert_key_tail(wr, last_key);
                g_tc9012_repeat_next_time[idx] = jiffies +
                                               msecs_to_jiffies(
                                                   ir->key_repeat_interval);
                cnt++;
            }
        }
        symbol = ir_next_reader_clr_inc(rd);
        flag2 = symbol->upper && symbol->lower;
    } while (flag2);
START_TIMER_OUT:
    tc9012_process_timeout(ir, ip, last_key, wr, idx, &cnt);
OUT:
    return fail ? -1 : (cnt ? 0 : -1);
}
EXPORT_SYMBOL(tc9012_frame_simple_parse);
EXPORT_SYMBOL(tc9012_match);
