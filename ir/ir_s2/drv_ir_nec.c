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
#if 1
#define mm_ps(phase, minp, maxp, mins, maxs) do { \
        minp = (phase)->minp; \
        maxp = (phase)->maxp; \
        mins = (phase)->mins; \
        maxs = (phase)->maxs; \
    } while (0)
#else
#define mm_ps(phase, minp, maxp, mins, maxs)
#endif

static hi_ulong          g_nec_repeat_next_time[MAX_NEC_INFR_NR];
static key_attr          g_nec_last_key[MAX_NEC_INFR_NR];
static struct timer_list g_nec_timer[MAX_NEC_INFR_NR];
hi_u32                   g_redundant_frame_flag = 0;

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

static hi_s32 data_margin_space(hi_u32 lower, hi_u32 upper, ir_protocol *ip,
                                hi_u64 minp, hi_u64 maxp,
                                hi_u64 mins, hi_u64 maxs)
{
    hi_s32 ret = (data_fallin(lower, minp, maxp) &&
                 (data_fallin(upper, mins, maxs) || upper > ip->attr.burst.space));
    return ret;
}

static hi_s32 check_null(hi_u32 first, hi_u32 second)
{
    hi_s32 ret = (hi_s32) (!first && !second);
    return ret;
}

static hi_s32 check_key_stat(hi_s32 fail, key_attr *key)
{
    hi_s32 ret = (hi_s32) (!fail && (key->upper || key->lower) &&key->key_stat != KEY_STAT_UP);
    return ret;
}

static hi_s32 check_last_key(hi_u32 idx)
{
    hi_s32 ret = (hi_s32) ((g_nec_last_key[idx].lower || g_nec_last_key[idx].upper) &&
                           (g_nec_last_key[idx].key_stat != KEY_STAT_UP));
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

static hi_s32 check_skip_burst(key_attr *key,ir_protocol *ip)
{
    hi_s32 ret = (hi_s32) (key->lower && key->upper &&
                           ip->attr.repeat.pluse &&
                           ip->attr.repeat.space);
    return ret;
}

static hi_s32 key_stat_time(hi_s32 fail, key_attr *key, hi_s32 event, hi_u32 idx)
{
    hi_s32 ret = (hi_s32) (!fail && (key->upper || key->lower) &&
                           event && time_after(jiffies,
                                               g_nec_repeat_next_time[idx]));
    return ret;
}

static hi_void nec_keyup_proc(hi_ulong i)
{
    key_attr  *last_key = NULL;

    if (i >= MAX_NEC_INFR_NR) {
        hiir_info("nec keyup timer, i > MAX_NEC_INFR_NR!\n");
        return;
    }

    last_key = &g_nec_last_key[i];
    g_last_key_state = last_key;
    if ((last_key->lower || last_key->upper) &&
        (last_key->key_stat != KEY_STAT_UP) &&
        (g_time_out_flag == HI_FALSE)) {
        last_key->key_stat = KEY_STAT_UP;
        g_redundant_frame_flag = 0;
        if (g_ir_local.key_up_event) {
            ir_insert_key_tail(g_ir_local.key_buf, last_key);
            wake_up_interruptible(&g_ir_local.read_wait);
        }

        last_key->lower = last_key->upper = 0;
    }
}

hi_void nec_init(hi_void)
{
    hi_s32 i;
    for (i = 0; i < MAX_NEC_INFR_NR; i++) {
        init_timer(&g_nec_timer[i]);
        g_nec_timer[i].expires = 0;
        g_nec_timer[i].data = (hi_ulong)~0;
        g_nec_timer[i].function = nec_keyup_proc;
    }

    if (memset_s(g_nec_last_key, MAX_NEC_INFR_NR * sizeof(key_attr), 0, MAX_NEC_INFR_NR * sizeof(key_attr))) {
        hiir_error("memset_s g_nec_last_key failed\n");
        return;
    }
}

hi_void nec_exit(hi_void)
{
    hi_s32 i;
    for (i = 0; i < MAX_NEC_INFR_NR; i++) {
        del_timer_sync(&g_nec_timer[i]);
    }
}

#define print_mm_sp(phase, name) do { \
        hiir_debug("%s: minp:%d, maxp:%d, mins:%d, maxp:%d\n",  \
                   name, (phase)->minp, (phase)->maxp, (phase)->mins, \
                   (phase)->maxs); \
    } while (0)

ir_match_result nec_check_header(ir_buffer *buf, ir_protocol *ip, key_attr *key)
{
    if (check_null(key->upper, key->lower)) {
        return IR_MATCH_NOT_MATCH;
    }

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

    /* repeate frame match? */
    if (check_null(ip->attr.repeat.pluse, ip->attr.repeat.space)) {
        return IR_MATCH_NOT_MATCH;
    }

    if (key_match_phase(key, &ip->attr.repeat) == 0) {
        return IR_MATCH_MATCH;
    }

    hiir_debug("%s->%d, repeat not match! ip at:%p" \
               " repeat[p, s, f]: [%d, %d, %d]," \
               " key[l, u]:[%d, %d]\n",
               __func__, __LINE__, ip,
               ip->attr.repeat.pluse,
               ip->attr.repeat.space,
               ip->attr.repeat.factor,
               (hi_u32)key->lower, (hi_u32)key->upper);

    print_mm_sp(&ip->attr.repeat, "repeat");

    return IR_MATCH_NOT_MATCH;
}

ir_match_result nec_check_frame(ir_buffer *buf,ir_protocol *ip, key_attr *key, hi_s32 idx)
{
    hi_s32 i;
    hi_s32 j;
    hi_s32 n;
    hiir_debug("idx:%d, checking symbol(at %d)[%d, %d]!\n",
               idx, buf->reader, (hi_u32)buf->buf[buf->reader].lower, (hi_u32)buf->buf[buf->reader].upper);

    if (check_null(key->upper, key->lower)) {
        hiir_debug("%s->%d, idx:%d, key empty!\n", __func__, __LINE__, idx);
        return IR_MATCH_NOT_MATCH;
    }

    /* header match? */
    hiir_debug("%s->%d, idx:%d, checking header!\n", __func__, __LINE__, idx);
    if (key_match_phase(key, &ip->attr.header)) {
        hiir_debug("%s->%d, header not match! ip at:%p" \
                   " header[p, s, f]: [%d, %d, %d]," \
                   " key[l, u]:[%d, %d]\n",
                   __func__, __LINE__, ip,
                   ip->attr.header.pluse,
                   ip->attr.header.space,
                   ip->attr.header.factor,
                   (hi_u32)key->lower, (hi_u32)key->upper);

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
            if (check_last_key(idx)) {
                return IR_MATCH_MATCH;
            }

            hiir_debug("(%d)[0x%.08llx,0x%.08llx](%s) exists\n", idx, g_nec_last_key[idx].lower,
                       g_nec_last_key[idx].upper, g_nec_last_key[idx].key_stat == KEY_STAT_DOWN ? "DWON" :
                       g_nec_last_key[idx].key_stat == KEY_STAT_UP ? "UP" : "HOLD");
            return IR_MATCH_NOT_MATCH;
        }
    } else {
        n = ip->attr.wanna_bits;
    }

    hiir_debug("%s->%d,idx:%d header match!\n", __func__, __LINE__, idx);
    n = buf->reader + n;
    n %= MAX_SYMBOL_NUM;

    key = &buf->buf[n];
    if (check_null(key->upper, key->lower)) {
        hiir_debug("%s->%d, idx:%d, needs more data\n",
                   __func__, __LINE__, idx);
        return IR_MATCH_NEED_MORE_DATA;
    }

    if ((key_match_phase(key, &ip->attr.b0) == 0) ||
        (key_match_phase(key, &ip->attr.b1) == 0)) {
        hiir_debug("%s->%d, idx:%d, frame  match!\n",
                   __func__, __LINE__, idx);

        hiir_debug("%s->%d, checking frame!\n",
                   __func__, __LINE__);

        /* check frame symbols */
        for (i = 0, j = i + buf->reader + 1; i < ip->attr.wanna_bits;
                i++, j++) {
            j %= MAX_SYMBOL_NUM;

            key = &buf->buf[j];

            if (check_null(key->lower, key->upper)) {
                hiir_info("%s->%d, need more symbols BUT BURST appears!\n",
                          __func__, __LINE__);
                return IR_MATCH_NEED_MORE_DATA;
            }

            /* check data phase is exceed or not */
            if (key_match_phase(key, &ip->attr.b0) &&
                key_match_phase(key, &ip->attr.b1)) {
                hiir_debug("%s->%d, symbol[%d, %d] not exceed" \
                           " b0 or b1's max space or pluse!\n",
                           __func__, __LINE__,
                           (hi_u32)key->upper, (hi_u32)key->lower);
                return IR_MATCH_NOT_MATCH;
            }
#ifdef HI_KPI_OPTIMIZE
            if (i > 23) {
                break;
            }
#endif
        }

        return IR_MATCH_MATCH;
    } else {
        hiir_debug("%s->%d, some datas in a frame not matched\n",
                   __func__, __LINE__);
        return IR_MATCH_NOT_MATCH;
    }
}

/* to see a frame is a full frame or a repeat frame */
ir_match_result nec_match(ir_match_type type,
                          ir_buffer *buf, ir_protocol *ip)
{
    key_attr          *key = NULL;
    hi_s32            idx;
    ir_match_result   ret;

    idx = ip->priv;
    if (idx >= MAX_NEC_INFR_NR) {
        hiir_info("nec , private data error!\n");
        return IR_MATCH_NOT_MATCH;
    }

    key = &buf->buf[buf->reader];
    switch (type) {
        case IR_MTT_HEADER: {
            ret = nec_check_header(buf, ip, key);
            return ret;
        }
        case IR_MTT_FRAME: {
            ret = nec_check_frame(buf, ip, key, idx);
            return ret;
        }
        case IR_MTT_BURST:

        /* fall though */
        default:
            return IR_MATCH_NOT_MATCH;
    }
}

hi_void nec_process_timeout(ir_priv *ir,ir_protocol *ip, key_attr *last_key,
                            ir_buffer *wr, hi_u32 idx, hi_s32 *pcnt)
{
    del_timer_sync(&g_nec_timer[idx]);

    if (g_time_out_flag == HI_TRUE) {
        g_time_out_flag = HI_FALSE;
        last_key->key_stat = KEY_STAT_UP;
        g_redundant_frame_flag = 0;

        if (ir->key_up_event) {
            ir_insert_key_tail(wr, last_key);
        }
        *pcnt = *pcnt + 1;
    } else {
        if (ip->key_hold_timeout_time == 0) {
            g_nec_timer[idx].expires = jiffies +
                                     msecs_to_jiffies(ir->key_hold_timeout_time);
        } else {
            g_nec_timer[idx].expires = jiffies +
                                     msecs_to_jiffies(ip->key_hold_timeout_time);
        }
        g_nec_timer[idx].data = idx;
        add_timer(&g_nec_timer[idx]);
    }

    return;
}

hi_s32 nec_frame_simple_parse(ir_priv *ir, ir_protocol *ip,
                              ir_buffer *rd, ir_buffer *wr)
{
    key_attr              *symbol = NULL;
    key_attr              key;
    hi_u64                minp;
    hi_u64                maxp;
    hi_u64                mins;
    hi_u64                maxs;
    hi_u32                i;
    key_attr              *last_key = NULL;
    hi_s32                cnt  = 0;
    hi_s32                fail = 0;
    hi_u32                idx = ip->priv;
    hi_u32                parse_bit_max = ip->attr.wanna_bits;

    if (idx >= MAX_NEC_INFR_NR) {
        hiir_info("nec , private data error!\n");
        goto OUT;
    }

    del_timer_sync(&g_nec_timer[idx]);
    last_key = &g_nec_last_key[idx];
    g_last_key_state = last_key;

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

        hiir_info("NEC : cannot parse!!!" \
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

    /* data phase */
    if (memcpy_s(key.protocol_name, PROTOCOL_NAME_SZ, ip->ir_code_name, PROTOCOL_NAME_SZ)) {
        hiir_error("memcpy_s to key.protocol_name failed\n");
        goto OUT;
    }

    symbol = ir_next_reader_clr_inc(rd);
    hiir_debug("try parse data(at %d)!\n", rd->reader);
    i = 0;

#ifdef HI_KPI_OPTIMIZE
    parse_bit_max = (ip->attr.wanna_bits) - 8;
#endif

    while (symbol->upper && symbol->lower && i < parse_bit_max) {
        /* bit 0? */
        mm_ps(&ip->attr.b0, minp, maxp, mins, maxs);
        if (data_both_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            symbol = ir_next_reader_clr_inc(rd);
            i++;
            continue;
        }

        /* bit 1? */
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

        hiir_info("%s->%d,NEC : unkown symbol[l, u]: [%d, %d]," \
                  " b0[p, s, f]: [%d, %d, %d]," \
                  " b1[p, s, f]: [%d, %d, %d]. discard this frame!\n",
                  __func__, __LINE__,
                  (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                  ip->attr.b0.pluse, ip->attr.b0.space,
                  ip->attr.b0.factor,
                  ip->attr.b1.pluse, ip->attr.b1.space,
                  ip->attr.b1.factor);
        i++;
        symbol = ir_next_reader_clr_inc(rd);
        fail++;
    }

#ifdef HI_KPI_OPTIMIZE
    i = 0;
    while( i < 8) {
        i++;
        symbol = ir_next_reader_clr_inc(rd);
    }
    key.lower |= ((~((key.lower & 0xff0000) << 8)) & 0xff000000);
#endif

    /* be compatible with MNEC */
    if (fail) {
        goto OUT;
    }

    /* if a repeat key. */
    if (check_repeat_key(last_key, key)) {
        g_redundant_frame_flag++;
        /* should i send a repeat key? */
        if (ir->key_repeat_event &&
            time_after(jiffies, g_nec_repeat_next_time[idx]) &&
            (g_redundant_frame_flag > 1)) {
            last_key->key_stat = KEY_STAT_HOLD;
            ir_insert_key_tail(wr, last_key);
            g_nec_repeat_next_time[idx] = jiffies +
                                        msecs_to_jiffies(
                                            ir->key_repeat_interval);

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
        g_redundant_frame_flag = 0;
        if (ir->key_up_event) {
            ir_insert_key_tail(wr, last_key);
        }
        cnt++;
    }

    hiir_debug("key parsed:[l:%llu, u:%llu, s:%d, p:%s]!\n",
               key.lower, key.upper, key.key_stat, key.protocol_name);

    key.key_stat = KEY_STAT_DOWN;
    ir_insert_key_tail(wr, &key);
    g_nec_repeat_next_time[idx] = jiffies +
                                msecs_to_jiffies(ir->key_repeat_interval);

    if (memcpy_s(last_key, sizeof(key_attr), &key, sizeof(key_attr))) {
        hiir_error("memcpy_s to last_key failed\n");
        goto OUT;
    }

    hiir_debug("last_key:(idx:%d)[0x%.08llx, 0x%.08llx]\n", idx, last_key->lower, last_key->upper);
    cnt++;

REPEAT_KEY:

    /* repeat? */
    do {
        ir_match_result r;
        hiir_debug("idx:%d,try parse repeat header(at %d)[%d, %d]!\n",
                   idx, rd->reader,
                   (hi_u32)ir->symbol_buf->buf[rd->reader].lower,
                   (hi_u32)ir->symbol_buf->buf[rd->reader].upper);

        /* checking a full repeat frame received or not. */
        r = ip->match(IR_MTT_FRAME, ir->symbol_buf, ip);
        if (r != IR_MATCH_MATCH) {
            hiir_debug("%s->%d, repeat frame not match\n",
                       __func__, __LINE__);
            if (r == IR_MATCH_NEED_MORE_DATA) {
                goto OUT;
            } else {
                goto START_TIMER_OUT;
            }
        }

        mm_ps(&ip->attr.repeat, minp, maxp, mins, maxs);
        if (data_neither_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            hiir_debug("%s->%d, repeat burst not match!\n",
                       __func__, __LINE__);
            goto START_TIMER_OUT;
        } else {
            hiir_debug("repeat parsed ,last_key[l,u,s] [0x%x,0x%x,%d],"
                       "time_after:%d,idx:%d!\n",
                       (hi_u32)last_key->lower, (hi_u32)last_key->upper,
                       last_key->key_stat,
                       time_after(jiffies, g_nec_repeat_next_time[idx]),
                       idx);
            g_redundant_frame_flag++;
            if (key_stat_time(fail, last_key, ir->key_repeat_event, idx) && g_redundant_frame_flag > 1) {
                hiir_debug("insert repeat parsed !\n");
                last_key->key_stat =
                    last_key->key_stat == KEY_STAT_UP ?
                    KEY_STAT_DOWN : KEY_STAT_HOLD;

                ir_insert_key_tail(wr, last_key);
                g_nec_repeat_next_time[idx] = jiffies +
                                            msecs_to_jiffies(ir->key_repeat_interval);
                cnt++;
            }
        }

        symbol = ir_next_reader_clr_inc(rd);
    } while (symbol->upper && symbol->lower);

START_TIMER_OUT:
    nec_process_timeout(ir, ip, last_key, wr, idx, &cnt);
OUT:
    return fail ? -1 : (cnt ? 0 : -1);
}

hi_s32 nec_frame_full_parse(ir_priv *ir, ir_protocol *ip,
                            ir_buffer *rd, ir_buffer *wr)
{
    key_attr               *symbol = NULL;
    key_attr               key;
    hi_u32                 i;
    hi_u64                 minp;
    hi_u64                 maxp;
    hi_u64                 mins;
    hi_u64                 maxs;
    key_attr               *last_key = NULL;
    hi_s32                 cnt = 0;
    hi_s32                 fail = 0;
    hi_u32                 idx = ip->priv;

    if (idx >= MAX_NEC_INFR_NR) {
        hiir_info("nec , private data error!\n");
        goto OUT;
    }

    del_timer_sync(&g_nec_timer[idx]);
    last_key = &g_nec_last_key[idx];

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
        hiir_info("NEC : cannot parse!!!" \
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

        hiir_info("%s->%d, NEC : unkown symbol[l, u]: [%d, %d]," \
                  " b0[p, s, f]: [%d, %d, %d]," \
                  " b1[p, s, f]: [%d, %d, %d]. assume to 0!\n",
                  __func__, __LINE__,
                  (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                  ip->attr.b0.pluse, ip->attr.b0.space,
                  ip->attr.b0.factor,
                  ip->attr.b1.pluse, ip->attr.b1.space,
                  ip->attr.b1.factor);
        i++;
        symbol = ir_next_reader_clr_inc(rd);
        fail++;
    }

    if (fail) {
        goto OUT;
    }

    /* if a repeat key. */
    if (check_repeat_key(last_key, key)) {
        /* should i send a repeat key? */
        g_redundant_frame_flag++;
        if (ir->key_repeat_event &&
            time_after(jiffies, g_nec_repeat_next_time[idx]) &&
            g_redundant_frame_flag > 1) {
            last_key->key_stat = KEY_STAT_HOLD;
            ir_insert_key_tail(wr, last_key);
            g_nec_repeat_next_time[idx] = jiffies +
                                        msecs_to_jiffies(
                                            ir->key_repeat_interval);

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
        g_redundant_frame_flag = 0;
        if (ir->key_up_event) {
            ir_insert_key_tail(wr, last_key);
        }
        cnt++;
    }

    hiir_debug("key parsed:[l:%llu, u:%llu, s:%d, p:%s]!\n",
               key.lower, key.upper, key.key_stat, key.protocol_name);

    key.key_stat = KEY_STAT_DOWN;
    ir_insert_key_tail(wr, &key);
    g_nec_repeat_next_time[idx] = jiffies +
                                msecs_to_jiffies(ir->key_repeat_interval);

    if (memcpy_s(last_key, sizeof( key_attr), &key, sizeof( key_attr))) {
        hiir_error("memcpy_s to last_key failed\n");
        goto OUT;
    }

    cnt++;

START_TIMER_OUT:
    nec_process_timeout(ir, ip, last_key, wr, idx, &cnt);

OUT:
    return fail ? -1 : (cnt ? 0 : -1);
}

static ir_match_result nec_2headers_match_header(ir_buffer   *buf,
                                                 ir_protocol *ip)
{
    key_attr      *key = NULL;
    key_attr      *key2 = NULL;
    hi_s32        n;
    hi_s32        idx;

    idx = ip->priv;
    if (idx >= MAX_NEC_INFR_NR) {
        hiir_info("nec , private data error!\n");
        return IR_MATCH_NOT_MATCH;
    }

    key = &buf->buf[buf->reader];
    if (check_null(key->upper, key->lower)) {
        return IR_MATCH_NOT_MATCH;
    }

    hiir_debug("%s->%d,idx:%d, checking header key[l, u] at:%d: [%d, %d]\n",
               __func__, __LINE__, idx,
               buf->reader, (hi_u32)key->lower, (hi_u32)key->upper);

    /*
     * if the protocol's header and second header and repeat is the same.
     * we should check out which phase this key is.
     */
    if ((key_match_phase(key, &ip->attr.header) == 0) &&
        (key_match_phase(key, &ip->attr.repeat) == 0)) {
        hi_u64 minp;
        hi_u64 maxp;
        hi_u64 mins;
        hi_u64 maxs;
        n = buf->reader + 1;
        n %= MAX_SYMBOL_NUM;

        /* check repeat. */
        key2 = &buf->buf[n];
        hiir_debug("%s->%d,idx:%d, checking repeat burst at %d [%d, %d].\n",
                   __func__, __LINE__, idx, n, (hi_u32)key->lower, (hi_u32)key->upper);
        if (check_null(key2->upper, key2->lower)) {
            hiir_debug("%s->%d, idx :%d, repeat burst not appear. need more symbols!\n",
                       __func__, __LINE__, idx);
            return IR_MATCH_NEED_MORE_DATA;
        }

        /* this is a repeat frame. */
        mm_ps(&ip->attr.burst, minp, maxp, mins, maxs);
        if (data_margin_space(key2->lower, key2->upper, ip,
                              minp, maxp, mins, maxs)) {
            if ((g_nec_last_key[idx].lower || g_nec_last_key[idx].upper) &&
                (g_nec_last_key[idx].key_stat != KEY_STAT_UP)) {
                hiir_debug("%s->%d, idx :%d, repeat burst match. this is repeat frame!\n",
                           __func__, __LINE__, idx);
                return IR_MATCH_MATCH;
            } else {
                hiir_debug("%s->%d, idx :%d, repeat burst match. but no last key, this frame is not mine.!\n",
                           __func__, __LINE__, idx);
                return IR_MATCH_NOT_MATCH;
            }
        }

        /* if repeat burst not appear, then  fall though to check frame. */
        hiir_debug("%s->%d, idx:%d,it seems that this is not a repeat frame. checking whole!\n",
                   __func__, __LINE__, idx);
    }

    /* checking second header */
    n = buf->reader + ip->attr.second_header_at;
    n %= MAX_SYMBOL_NUM;

    key2 = &buf->buf[n];

    if (key_match_phase(key, &ip->attr.header) == 0) {
        if (check_null(key2->upper, key2->lower)) {
            if (check_null(ip->attr.repeat.pluse, ip->attr.repeat.space)) {
                return IR_MATCH_NEED_MORE_DATA;
            }

            /*
             * if some protocol's repeat phase is the same as the header,
             * we need to check if this is a repeat here.
             */
            if (key_match_phase(key2, &ip->attr.repeat) == 0) {
                hiir_debug("%s->%d idx:%d, checking repeat.\n", __func__, __LINE__, idx);
                if (check_last_key(idx)) {
                    hiir_debug("%s->%d idx:%d, repeat match.\n", __func__, __LINE__, idx);
                    return IR_MATCH_MATCH;
                } else {
                    hiir_debug("%s->%d idx:%d, repeat not match.\n", __func__, __LINE__, idx);
                    return IR_MATCH_NOT_MATCH;
                }
            }

            hiir_debug("%s->%d,idx:%d,need more data to check second header\n",
                       __func__, __LINE__, idx);

            /* if this is not a repeat frame, we should report symbols not enough. */
            return IR_MATCH_NEED_MORE_DATA;
        }

        hiir_debug("%s->%d,idx:%d,checking second header key[l, u] at:%d: [%d, %d]\n",
                   __func__, __LINE__, idx,
                   n, (hi_u32)key2->lower, (hi_u32)key2->upper);
        if (key_match_phase(key2, &ip->attr.second_header) == 0) {
            return IR_MATCH_MATCH;
        }
    }

    hiir_debug("%s->%d, header not match! ip(idx:%d) at:%p" \
               " header[p, s, f]: [%d, %d, %d]," \
               " second header[p, s, f]: [%d, %d, %d]," \
               " key[l, u] at %d:[%d, %d]\n",
               __func__, __LINE__, idx, ip,
               ip->attr.header.pluse,
               ip->attr.header.space,
               ip->attr.header.factor,
               ip->attr.second_header.pluse,
               ip->attr.second_header.space,
               ip->attr.second_header.factor,
               buf->reader,
               (hi_u32)key->lower, (hi_u32)key->upper);

    /* repeate frame match? */
    if (check_null(ip->attr.repeat.pluse, ip->attr.repeat.space)) {
        return IR_MATCH_NOT_MATCH;
    }

    if ((key_match_phase(key, &ip->attr.repeat)) == 0) {
        hiir_debug("%s->%d idx:%d, checking repeat.\n", __func__, __LINE__, idx);
        if (check_last_key(idx)) {
            hiir_debug("%s->%d idx:%d, repeat match.\n", __func__, __LINE__, idx);
            return IR_MATCH_MATCH;
        } else {
            hiir_debug("%s->%d idx:%d, repeat not match.\n", __func__, __LINE__, idx);
            return IR_MATCH_NOT_MATCH;
        }
    }

    hiir_debug("%s->%d, repeat not match! ip(idx :%d) at:%p" \
               " repeat[p, s, f]: [%d, %d, %d]," \
               " key[l, u]:[%d, %d]\n",
               __func__, __LINE__, idx, ip,
               ip->attr.repeat.pluse,
               ip->attr.repeat.space,
               ip->attr.repeat.factor,
               (hi_u32)key->lower, (hi_u32)key->upper);

    return IR_MATCH_NOT_MATCH;
}

static ir_match_result nec_2headers_match_frame(ir_buffer   *buf,
                                                ir_protocol *ip)
{
    hi_u64          minp;
    hi_u64          maxp;
    hi_u64          mins;
    hi_u64          maxs;
    hi_s32          i;
    hi_s32          j;
    hi_s32          n;
    key_attr        *key = NULL;
    key_attr        *key2 = NULL;

    key = &buf->buf[buf->reader];
    if (check_null(key->upper, key->lower)) {
        return IR_MATCH_NOT_MATCH;
    }

    /* checking second header */
    n = buf->reader + ip->attr.second_header_at;
    n %= MAX_SYMBOL_NUM;

    key2 = &buf->buf[n];

    if (key_match_phase(key, &ip->attr.header) ||
        key_match_phase(key2, &ip->attr.second_header)) {
        hiir_debug("%s->%d, header not match! ip at:%p" \
                   " header[p, s, f]: [%d, %d, %d]," \
                   " second header[p, s, f]: [%d, %d, %d]," \
                   " key[l, u] at:%d :[%d, %d],"
                   " key2[l, u] at:%d :[%d, %d]\n",
                   __func__, __LINE__, ip,
                   ip->attr.header.pluse,
                   ip->attr.header.space,
                   ip->attr.header.factor,
                   ip->attr.second_header.pluse,
                   ip->attr.second_header.space,
                   ip->attr.second_header.factor,
                   buf->reader,
                   (hi_u32)key->lower, (hi_u32)key->upper,
                   n,
                   (hi_u32)key2->lower, (hi_u32)key2->upper);

        /* repeate frame match? */
        if (check_null(ip->attr.repeat.pluse, ip->attr.repeat.space)) {
            return IR_MATCH_NOT_MATCH;
        }

        if (key_match_phase(key, &ip->attr.repeat)) {
            hiir_debug("%s->%d, repeat not match!\n",
                       __func__, __LINE__);
            return IR_MATCH_NOT_MATCH;
        } else {
            n = 1;
        }
    } else {
        n = ip->attr.wanna_bits + 2;
    }

    /* try find burst. */
    n = buf->reader + n;
    n %= MAX_SYMBOL_NUM;

    hiir_debug("%s->%d, checking burst at(%d)!\n",
               __func__, __LINE__, n);

    key = &buf->buf[n];
    if (check_null(key->upper, key->lower)) {
        hiir_debug("%s->%d, needs more data\n",
                   __func__, __LINE__);
        return IR_MATCH_NEED_MORE_DATA;
    }

    /* burst match? */
    /*
     * NEC frame burst may fall in [space ,8%],
     * but it still can greater than space !
     */
    mm_ps(&ip->attr.burst, minp, maxp, mins, maxs);
    if (data_margin_space(key->lower, key->upper, ip,
                          minp, maxp, mins, maxs)) {
        hiir_debug("%s->%d, checking frame!\n",
                   __func__, __LINE__);

        /* check frame symbols */
        for (i = 1, j = i + buf->reader; i <= ip->attr.wanna_bits && j != n;
                i++, j++) {
            j %= MAX_SYMBOL_NUM;

            if (j == n) {
                break;
            }

            key = &buf->buf[j];

            if (check_null(key->lower, key->upper)) {
                hiir_info("%s->%d, need more symbols BUT BURST appears!\n",
                          __func__, __LINE__);
                return IR_MATCH_NEED_MORE_DATA;
            }

            hiir_debug("%s->%d, i:%d, ip->attr.second_header_at:%d, " \
                       "key:[p, s], [%d,%d], " \
                       "second_header:[p, s, f], [%d, %d, %d].\n",
                       __func__, __LINE__,
                       i, ip->attr.second_header_at,
                       (hi_u32)key->lower, (hi_u32)key->upper,
                       (hi_u32)ip->attr.second_header.pluse,
                       (hi_u32)ip->attr.second_header.space,
                       ip->attr.second_header.factor);

            if (i == ip->attr.second_header_at) {
                hiir_debug("%s->%d, match second_header! i:%d, ip->attr.second_header_at:%d, " \
                           "symbol:[l, u] at:%d: [%d, %d]\n",
                           __func__, __LINE__,
                           i, ip->attr.second_header_at,
                           j,
                           (hi_u32)key->lower, (hi_u32)key->upper);
                if (key_match_phase(key, &ip->attr.second_header)) {
                    return IR_MATCH_NOT_MATCH;
                }

                continue;
            }

            /* check data phase is exceed or not */
            if (key_match_phase(key, &ip->attr.b0) &&
                key_match_phase(key, &ip->attr.b1)) {
                hiir_debug("%s->%d, symbol[%d, %d] not exceed" \
                           " b0 or b1's max space or pluse!\n",
                           __func__, __LINE__,
                           (hi_u32)key->upper, (hi_u32)key->lower);
                return IR_MATCH_NOT_MATCH;
            }
        }

        /*
         * try check repeat
         * this check is specially for protocols which have the same
         * header, second header, and the same position of the second header,
         * and the same bit numbers. but the repeat header is not the same.
         *
         * so ugly check...
         *
         * if some key press do not constains repeat frame, the first protocol
         * meats will be used to parse the frame.
         */
        /* skip burst */
        j += 2;
        j %= MAX_SYMBOL_NUM;

        key = &buf->buf[j];
        hiir_debug("checking repeat header at:%d,[%d, %d]\n", j, (hi_u32)key->lower, (hi_u32)key->upper);


        if (check_skip_burst(key, ip)) {
            if ((key_match_phase(key, &ip->attr.repeat)) == 0) {
                return IR_MATCH_MATCH;
            }

            if (data_fallin(key->lower, ip->attr.repeat.minp, ip->attr.repeat.maxp) ||
                data_fallin(key->upper, ip->attr.repeat.mins, ip->attr.repeat.maxs)) {
                return IR_MATCH_NOT_MATCH;
            }
        }

        return IR_MATCH_MATCH;
    } else {
        hiir_debug("%s->%d, burst not match!"
                   " key[l, u][%d, %d],"
                   " burst[p, s, f]: [%d, %d, %d]\n",
                   __func__, __LINE__,
                   (hi_u32)key->lower, (hi_u32)key->upper,
                   ip->attr.burst.pluse,
                   ip->attr.burst.space,
                   ip->attr.burst.factor);
        return IR_MATCH_NOT_MATCH;
    }
}

/* to see a frame is a full frame or a repeat frame */
ir_match_result nec_2headers_match(ir_match_type type,
                                   ir_buffer *buf, ir_protocol *ip)
{
    switch (type) {
        case IR_MTT_HEADER:
            return nec_2headers_match_header(buf, ip);
        case IR_MTT_FRAME:
            return nec_2headers_match_frame(buf, ip);
        case IR_MTT_BURST:

        /* fall though */
        default:
            return IR_MATCH_NOT_MATCH;
    }
}

hi_s32 nec_frame_2headers_simple_parse(ir_priv *ir, ir_protocol *ip,
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
    hi_s32             cnt  = 0;
    hi_s32             fail = 0;
    hi_s32             flag = 0;
    hi_s32             flag2 = 0;
    hi_u32             idx = ip->priv;

    if (idx >= MAX_NEC_INFR_NR) {
        hiir_info("nec , private data error!\n");
        goto OUT;
    }

    del_timer_sync(&g_nec_timer[idx]);
    last_key = &g_nec_last_key[idx];

    /* header phase */
    if (memset_s(&key, sizeof(key_attr), 0, sizeof(key_attr))) {
        hiir_error("memset_s key failed\n");
        goto OUT;
    }

    hiir_debug("try parse header(at %d)!\n", rd->reader);
    symbol = &rd->buf[rd->reader];
    mm_ps(&ip->attr.header, minp, maxp, mins, maxs);

    /* try get second header */
    n = (rd->reader + ip->attr.second_header_at) % MAX_SYMBOL_NUM;
    symbol2 = &rd->buf[n];

    /* frame start? */
    if (key_match_phase(symbol, &ip->attr.header) ||
        key_match_phase(symbol2, &ip->attr.second_header)) {
        /* repeate key? */
        if (key_match_phase(symbol, &ip->attr.repeat) == 0) {
            goto REPEAT_KEY;
        }

        hiir_info("NEC 2h simple: cannot parse!!!" \
                  " header phase not match. symbol[l,u]: [%d, %d]," \
                  " symbol2[l,u]: [%d, %d], " \
                  " header phase[p, s, f]: [%d, %d, %d]." \
                  " second header phase[p, s, f]: [%d, %d, %d]." \
                  " repeat phase[p, s, f]: [%d, %d, %d].\n",
                  (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                  (hi_u32)symbol2->lower, (hi_u32)symbol2->upper,
                  ip->attr.header.pluse, ip->attr.header.space,
                  ip->attr.header.factor,
                  ip->attr.second_header.pluse, ip->attr.second_header.space,
                  ip->attr.second_header.factor,
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
    i = 1;
    flag = symbol->upper && symbol->lower && i <= ip->attr.wanna_bits;
    while (flag) {
        /* bit 0? */
        mm_ps(&ip->attr.b0, minp, maxp, mins, maxs);
        if (data_both_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            symbol = ir_next_reader_clr_inc(rd);
            i++;
            flag = symbol->upper && symbol->lower && i <= ip->attr.wanna_bits;
            continue;
        }

        /* bit 1? */
        mm_ps(&ip->attr.b1, minp, maxp, mins, maxs);
        if (data_both_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            if (i - 1 < 64) {
                key.lower |= (hi_u64)(((hi_u64)1) << (i - 1));
            } else {
                key.upper |= (hi_u64)(((hi_u64)1) << (i - 64 - 1));
            }

            symbol = ir_next_reader_clr_inc(rd);
            i++;
            flag = symbol->upper && symbol->lower && i <= ip->attr.wanna_bits;
            continue;
        }

        if (i == ip->attr.second_header_at) {
            hiir_debug("simple parse skip second_header! i:%d, ip->attr.second_header_at:%d, " \
                       "symbol:[l, u]: [%d, %d]\n",
                       i, ip->attr.second_header_at,
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper);
            symbol = ir_next_reader_clr_inc(rd);
            flag = symbol->upper && symbol->lower && i <= ip->attr.wanna_bits;
            continue;
        }

        hiir_info("%s->%d, NEC : unkown symbol[l, u]: [%d, %d]," \
                  " b0[p, s, f]: [%d, %d, %d]," \
                  " b1[p, s, f]: [%d, %d, %d]. Discard this frame!\n",
                  __func__, __LINE__,
                  (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                  ip->attr.b0.pluse, ip->attr.b0.space,
                  ip->attr.b0.factor,
                  ip->attr.b1.pluse, ip->attr.b1.space,
                  ip->attr.b1.factor);
        i++;
        symbol = ir_next_reader_clr_inc(rd);
        flag = symbol->upper && symbol->lower && i <= ip->attr.wanna_bits;
        fail++;
    }

    /* burst */
    hiir_debug("try parse burst(at %d)!\n", rd->reader);
    mm_ps(&ip->attr.burst, minp, maxp, mins, maxs);
    if (data_either_smaller((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
        hiir_info("%s->%d, NEC : unkown symbol[l, u]: [%d, %d]," \
                  " burst[p, s, f]: [%d, %d, %d].\n",
                  __func__, __LINE__,
                  (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                  ip->attr.burst.pluse, ip->attr.burst.space,
                  ip->attr.burst.factor);
        fail++;
        goto OUT;
    } else {
        if (check_key_stat(fail, last_key)) {
            last_key->key_stat = KEY_STAT_UP;
            if (ir->key_up_event) {
                ir_insert_key_tail(wr, last_key);
            }
            cnt++;
        }

        key.key_stat = KEY_STAT_DOWN;

        hiir_debug("key parsed:[l:%lld, u:%lld, s:%d, p:%s]!\n",
                   key.lower, key.upper, key.key_stat, key.protocol_name);
        if (!fail) {
            ir_insert_key_tail(wr, &key);
            cnt++;
            g_nec_repeat_next_time[idx] = jiffies +
                                        msecs_to_jiffies(ir->key_repeat_interval);

            if (memcpy_s(last_key, sizeof(key_attr), &key, sizeof(key_attr))) {
                hiir_error("memcpy_s to last_key failed\n");
                goto OUT;
            }
        }

        symbol = ir_next_reader_clr_inc(rd);
    }

REPEAT_KEY:

    /* repeat? */
    do {
        ir_match_result r;
        hiir_debug("try parse repeat header(at %d)!\n", rd->reader);

        /* checking a full repeat frame received or not. */
        r = ip->match(IR_MTT_FRAME, ir->symbol_buf, ip);
        if (r != IR_MATCH_MATCH) {
            hiir_debug("%s->%d, repeat frame not match\n",
                       __func__, __LINE__);
            if (r == IR_MATCH_NEED_MORE_DATA) {
                goto OUT;
            } else {
                goto START_TIMER_OUT;
            }
        }

        mm_ps(&ip->attr.repeat, minp, maxp, mins, maxs);
        if (data_neither_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            hiir_debug("%s->%d, repeat burst not match!\n",
                       __func__, __LINE__);
            goto START_TIMER_OUT;
        }

        /* burst */
        mm_ps(&ip->attr.burst, minp, maxp, mins, maxs);
        symbol = ir_next_reader_clr_inc(rd);
        hiir_debug("try parse repeat burst(at %d)!\n", rd->reader);

        /*
         * NEC burst's space may fallin [space, 8%], but it also
         * may greate than maxp.
         */
        if (data_either_smaller((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            hiir_debug("NEC : repeat burst not match" \
                       "[l, u]: [%d, %d]," \
                       " burst[p, s, f]: [%d, %d, %d].\n",
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                       ip->attr.burst.pluse, ip->attr.burst.space,
                       ip->attr.burst.factor);
        } else {
            if (key_stat_time(fail, last_key, ir->key_repeat_event, idx)) {
                last_key->key_stat =
                    last_key->key_stat == KEY_STAT_UP ?
                    KEY_STAT_DOWN : KEY_STAT_HOLD;

                ir_insert_key_tail(wr, last_key);
                g_nec_repeat_next_time[idx] = jiffies +
                                            msecs_to_jiffies(
                                                ir->key_repeat_interval);
                cnt++;
            }
        }

        symbol = ir_next_reader_clr_inc(rd);
        flag2 = symbol->upper && symbol->lower;
    } while (flag2);

START_TIMER_OUT:
    nec_process_timeout(ir, ip, last_key, wr, idx, &cnt);
OUT:
    return fail ? -1 : (cnt ? 0 : -1);
}

hi_s32 nec_frame_2headers_full_parse(ir_priv *ir, ir_protocol *ip,
                                     ir_buffer *rd, ir_buffer *wr)
{
    key_attr             *symbol = NULL;
    key_attr             *symbol2 = NULL;
    key_attr             key;
    hi_u32               i;
    hi_u32               n;
    hi_u64               minp;
    hi_u64               maxp;
    hi_u64               mins;
    hi_u64               maxs;
    key_attr             *last_key = NULL;
    hi_s32               cnt = 0;
    hi_s32               fail = 0;
    hi_s32               flag = 0;
    hi_s32               flag2 = 0;
    hi_u32               idx = ip->priv;

    if (idx >= MAX_NEC_INFR_NR) {
        hiir_info("nec , private data error!\n");
        goto OUT;
    }

    del_timer_sync(&g_nec_timer[idx]);
    last_key = &g_nec_last_key[idx];

    /* header phase */
    if (memset_s(&key, sizeof(key_attr), 0, sizeof(key_attr))) {
        hiir_error("memset_s key failed\n");
        goto OUT;
    }

    hiir_debug("try parse header(at %d)!\n", rd->reader);
    symbol = &rd->buf[rd->reader];

    /* try get second header */
    n = rd->reader + ip->attr.second_header_at;
    n %= MAX_SYMBOL_NUM;

    symbol2 = &rd->buf[n];

    /* frame start? */
    if (key_match_phase(symbol, &ip->attr.header) ||
        key_match_phase(symbol2, &ip->attr.second_header)) {
        hiir_info("NEC full 2header: cannot parse!!!" \
                  " header phase not match. symbol[l,u]: [%d, %d]," \
                  " symbol2[l,u]: [%d, %d], " \
                  " header phase[p, s, f]: [%d, %d, %d]." \
                  " second header phase[p, s, f]: [%d, %d, %d]." \
                  " repeat phase[p, s, f]: [%d, %d, %d].\n",
                  (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                  (hi_u32)symbol2->lower, (hi_u32)symbol2->upper,
                  ip->attr.header.pluse, ip->attr.header.space,
                  ip->attr.header.factor,
                  ip->attr.second_header.pluse, ip->attr.second_header.space,
                  ip->attr.second_header.factor,
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
    i = 1;
    flag = symbol->upper && symbol->lower && i <= ip->attr.wanna_bits;
    while (flag) {
        mm_ps(&ip->attr.b0, minp, maxp, mins, maxs);
        if (data_both_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            symbol = ir_next_reader_clr_inc(rd);
            i++;
            flag = symbol->upper && symbol->lower && i <= ip->attr.wanna_bits;
            continue;
        }

        mm_ps(&ip->attr.b1, minp, maxp, mins, maxs);
        if (data_both_margin((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
            if (i - 1 < 64) {
                key.lower |= (hi_u64)(((hi_u64)1) << (i - 1));
            } else {
                key.upper |= (hi_u64)(((hi_u64)1) << (i - 64 - 1));
            }

            symbol = ir_next_reader_clr_inc(rd);
            i++;
            flag = symbol->upper && symbol->lower && i <= ip->attr.wanna_bits;
            continue;
        }

        if (i == ip->attr.second_header_at) {
            hiir_debug("%s->%d, full parse skip second_header! i:%d, ip->attr.second_header_at:%d, " \
                       "symbol:[l, u]: [%d, %d]\n",
                       __func__, __LINE__,
                       i, ip->attr.second_header_at,
                       (hi_u32)symbol->lower, (hi_u32)symbol->upper);
            symbol = ir_next_reader_clr_inc(rd);
            flag = symbol->upper && symbol->lower && i <= ip->attr.wanna_bits;
            continue;
        }

        hiir_info("%s->%d, NEC : unkown symbol[l, u]: [%d, %d]," \
                  " b0[p, s, f]: [%d, %d, %d]," \
                  " b1[p, s, f]: [%d, %d, %d]. assume to 0!\n",
                  __func__, __LINE__,
                  (hi_u32)symbol->lower, (hi_u32)symbol->upper,
                  ip->attr.b0.pluse, ip->attr.b0.space,
                  ip->attr.b0.factor,
                  ip->attr.b1.pluse, ip->attr.b1.space,
                  ip->attr.b1.factor);
        i++;
        symbol = ir_next_reader_clr_inc(rd);
        flag = symbol->upper && symbol->lower && i <= ip->attr.wanna_bits;
        fail++;
    }

    /* burst */
    hiir_debug("try parse burst(at %d)!\n", rd->reader);
    mm_ps(&ip->attr.burst, minp, maxp, mins, maxs);
    if (data_either_smaller((hi_u32)symbol->lower, (hi_u32)symbol->upper, minp, maxp, mins, maxs)) {
        hiir_info("%s->%d, NEC : unkown symbol[l, u]: [%d, %d]," \
                  " burst[p, s, f]: [%d, %d, %d].\n",
                  __func__, __LINE__,
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
            flag2 = ir->key_repeat_event && (HI_FALSE == g_time_out_flag)
                    && time_after(jiffies, g_nec_repeat_next_time[idx]);
            if (flag2) {
                last_key->key_stat = KEY_STAT_HOLD;
                ir_insert_key_tail(wr, last_key);
                (hi_void)ir_next_reader_clr_inc(rd);
                g_nec_repeat_next_time[idx] = jiffies +
                                              msecs_to_jiffies(
                                                ir->key_repeat_interval);

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
        g_nec_repeat_next_time[idx] = jiffies +
                                      msecs_to_jiffies(ir->key_repeat_interval);

        if (memcpy_s(last_key, sizeof(key_attr), &key, sizeof(key_attr))) {
            hiir_error("memcpy_s to last_key failed\n");
            goto OUT;
        }

        cnt++;
    }

START_TIMER_OUT:
    nec_process_timeout(ir, ip, last_key, wr, idx, &cnt);
OUT:
    return fail ? -1 : (cnt ? 0 : -1);
}

EXPORT_SYMBOL(nec_match);
EXPORT_SYMBOL(nec_frame_simple_parse);
EXPORT_SYMBOL(nec_frame_full_parse);
EXPORT_SYMBOL(nec_2headers_match);
EXPORT_SYMBOL(nec_frame_2headers_simple_parse);
EXPORT_SYMBOL(nec_frame_2headers_full_parse);
