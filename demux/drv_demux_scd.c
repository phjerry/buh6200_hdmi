/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function impl.
 * Author: sdk
 * Create: 2017-05-31
 */

#include "drv_demux_scd.h"
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/delay.h>

#include "hi_type.h"
#include "hi_drv_mem.h"

#include "drv_demux_define.h"
#include "drv_demux_recfct.h"
#include "drv_demux_func.h"
#include "hal_demux.h"

#define DMX_INDEX_INVALID_PTS_US             (hi_s64)(-1)

static hi_void dmx_parse_scd(const dmx_index_scd *scd_index, dmx_index_parse *parse_index)
{
    parse_index->over_flow = !((scd_index->scd_type_ovflag_goback_num >> 28) & 0x1); /* 28 bits */
    if (parse_index->over_flow) {
        HI_INFO_DEMUX("parse_index.over_flow == 1\n");
    }

    parse_index->offset_in_ts = scd_index->sc_type_byte12after_sc_offset_in_ts & 0x00ff;
    parse_index->tscnt = scd_index->ts_cnt_hi8_byte345after_sc & 0xff000000;
    parse_index->tscnt = parse_index->tscnt << 8; /* 8 bits */
    parse_index->tscnt |= scd_index->ts_cnt_lo32;  /* 96cv300  count from zero */
    parse_index->offset_in_dav_buf = scd_index->scd_type_ovflag_goback_num & 0x00ffffff;

    parse_index->pts_us = DMX_INDEX_INVALID_PTS_US;
    parse_index->reserv = 0;
    parse_index->index_type = (scd_index->scd_type_ovflag_goback_num >> 29) & 0xf; /* 29 bits */
    parse_index->start_code = (scd_index->sc_type_byte12after_sc_offset_in_ts >> 8) & 0xffU; /* 8 bits */

    if (parse_index->index_type == DMX_INDEX_SC_TYPE_PTS_FOR_TS ||
        parse_index->index_type == DMX_INDEX_SC_TYPE_PTS_FOR_PES) {
        parse_index->byte_after_sc[0] = (scd_index->sc_type_byte12after_sc_offset_in_ts >> 24) & 0xffU; /* 24 bits */
        parse_index->byte_after_sc[1] = (scd_index->sc_type_byte12after_sc_offset_in_ts >> 16) & 0xffU; /* 16 bits */
        parse_index->byte_after_sc[2] = (scd_index->ts_cnt_hi8_byte345after_sc >> 16) & 0xffU; /* index 2, 16 bits */
        parse_index->byte_after_sc[3] = (scd_index->ts_cnt_hi8_byte345after_sc >> 8) & 0xffU; /* index 3, 8 bits */
        parse_index->byte_after_sc[4] = (scd_index->ts_cnt_hi8_byte345after_sc) & 0xffU; /* index 4 */
        parse_index->byte_after_sc[5] = (scd_index->sc_code_byte678after_sc >> 16) & 0xffU; /* index 5, 16 bits */
        parse_index->byte_after_sc[6] = (scd_index->sc_code_byte678after_sc >> 8) & 0xffU; /* index 6, 8 bits */
        parse_index->byte_after_sc[7] = (scd_index->sc_code_byte678after_sc) & 0xffU; /* index 7 */
    } else {
        parse_index->byte_after_sc[0] = (scd_index->sc_type_byte12after_sc_offset_in_ts >> 16) & 0xffU; /* 16 bits */
        parse_index->byte_after_sc[1] = (scd_index->sc_type_byte12after_sc_offset_in_ts >> 24) & 0xffU; /* 24 bits */
        parse_index->byte_after_sc[2] = (scd_index->ts_cnt_hi8_byte345after_sc) & 0xffU; /* index 2 */
        parse_index->byte_after_sc[3] = (scd_index->ts_cnt_hi8_byte345after_sc >> 8) & 0xffU; /* index 3, 8 bits */
        parse_index->byte_after_sc[4] = (scd_index->ts_cnt_hi8_byte345after_sc >> 16) & 0xffU; /* index 4, 16 bits */
        parse_index->byte_after_sc[5] = (scd_index->sc_code_byte678after_sc) & 0xffU; /* index 5 */
        parse_index->byte_after_sc[6] = (scd_index->sc_code_byte678after_sc >> 8) & 0xffU; /* index 6, 8 bits */
        parse_index->byte_after_sc[7] = (scd_index->sc_code_byte678after_sc >> 16) & 0xffU; /* index 7, 16 bits */
    }

    if (parse_index->index_type == DMX_INDEX_SC_TYPE_PTS_FOR_TS) {
        if ((scd_index->ts_cnt_hi8_byte345after_sc & 0x2) == 0) {
            parse_index->pts_us = DMX_INDEX_INVALID_PTS_US;
        } else {
            hi_s64 pts      = 0;
            hi_u32 pts33    = scd_index->ts_cnt_hi8_byte345after_sc & 0x1;
            hi_u32 pts32    = scd_index->sc_code_byte678after_sc;

            if (pts33) {
                pts = 0x100000000ULL;
            }

            pts = pts + (hi_s64)pts32 * MS_2_US;
            parse_index->pts_us = pts / 90; /* trans 90khz count to us. */
        }
    }

    return;
}

#if (DMX_REC_BUF_GAP_EXIST == 1)
static hi_u64 dmx_index_calc_global_offset(struct dmx_r_rec_fct *rrec_fct, const dmx_index_parse *parse_index)
{
    hi_u64 offset; /* frame header offset (tota value) */
    hi_u64 cycle;

    /* When it is detected that the TS packet corresponding to the index has been rewinded,
       the gloabl_offset of the index needs to be accumulated by the length of one or two(ts:2, tts:1)
       ts packets to ensure that the index corresponds to the actual data. */
    if (unlikely(rrec_fct->rec_time_stamp >= DMX_REC_TIMESTAMP_ZERO)) { /* only 96cv300 don't need to add 4 */
        offset = parse_index->tscnt * DMX_TTS_PKT_SIZE;
        /* The wraparound cycle is buffer_size minus 1 tts packet sizes. */
        cycle  = rrec_fct->rbuf->buf_size - DMX_TTS_PKT_SIZE;
        if ((offset > rrec_fct->last_scd_offset) && ((offset % cycle) < (rrec_fct->last_scd_offset % cycle))) {
            rrec_fct->ts_cnt_compensate += 1; /* Ts_cnt compensation accumulation 1 */
        }
        rrec_fct->last_scd_offset = offset;

        return (parse_index->tscnt + rrec_fct->ts_cnt_compensate) * DMX_TTS_PKT_SIZE + parse_index->offset_in_ts;
    } else {
        offset = parse_index->tscnt * DMX_TS_PKT_SIZE;
        /* The wraparound cycle is buffer_size minus 2 ts packet sizes. */
        cycle  = rrec_fct->rbuf->buf_size - 2 * DMX_TS_PKT_SIZE;
        if ((offset > rrec_fct->last_scd_offset) && ((offset % cycle) < (rrec_fct->last_scd_offset % cycle))) {
            rrec_fct->ts_cnt_compensate += 2; /* Ts_cnt compensation accumulation 2 */
        }
        rrec_fct->last_scd_offset = offset;

        return (parse_index->tscnt + rrec_fct->ts_cnt_compensate) * DMX_TS_PKT_SIZE + parse_index->offset_in_ts;
    }
}
#else
static inline hi_u64 dmx_index_calc_global_offset(struct dmx_r_rec_fct *rrec_fct, const dmx_index_parse *parse_index)
{
    hi_u64 offset; /* frame header offset (tota value) */
    if (unlikely(rrec_fct->rec_time_stamp >= DMX_REC_TIMESTAMP_ZERO)) { /* only 96cv300 don't need to add 4 */
        offset = parse_index->tscnt * DMX_TTS_PKT_SIZE + parse_index->offset_in_ts;
    } else {
        offset = parse_index->tscnt * DMX_TS_PKT_SIZE + parse_index->offset_in_ts;
    }

    return offset;
}
#endif

hi_s32 dmx_scd_to_video_index(struct dmx_r_rec_fct *rrec_fct, const dmx_index_scd *sc_index, findex_scd *fidx)
{
    hi_u64 curr_global_offset;
    dmx_index_parse  parse_index = {0};

    DMX_NULL_POINTER_RETURN(sc_index);
    DMX_NULL_POINTER_RETURN(fidx);

    dmx_parse_scd(sc_index, &parse_index);

    /* just only deal with SC of frame and pts */
    if ((parse_index.index_type != DMX_INDEX_SC_TYPE_PTS_FOR_TS) &&
        (parse_index.index_type != DMX_INDEX_SC_TYPE_PIC_FOR_TS) &&
        (parse_index.index_type != DMX_INDEX_SC_TYPE_PIC_SHORT_FOR_TS)) {
        return HI_FAILURE;
    }

    if (unlikely(parse_index.index_type == DMX_INDEX_SC_TYPE_PTS_FOR_TS &&
                 parse_index.pts_us == DMX_INDEX_INVALID_PTS_US)) {
        return HI_FAILURE;
    }

    curr_global_offset = dmx_index_calc_global_offset(rrec_fct, &parse_index);

    fidx->global_offset = curr_global_offset;
    fidx->pts_us = parse_index.pts_us;
    fidx->index_type = parse_index.index_type;
    fidx->start_code = parse_index.start_code;

    memcpy(fidx->data_after_sc, parse_index.byte_after_sc, sizeof(parse_index.byte_after_sc));
    fidx->extra_scdata = HI_NULL;
    fidx->extra_scdata_size = 0;
    fidx->extra_scdata_phy_addr = 0;

    return HI_SUCCESS;
}

#define TS_PKT_HEADER_LEN     0x4
#define PES_PKT_HEADER_LENGTH 0x9

static inline hi_u32 dmx_adp_fld_len(hi_u8 *ts_data)
{
    WARN_ON(ts_data[0] != 0x47);

    if (((ts_data[3] >> 4) & 0x03) == 0x02) { /* index 3, 4 bits */
        return DMX_TS_PKT_SIZE - TS_PKT_HEADER_LEN;
    } else if (((ts_data[3] >> 4) & 0x03) == 0x03) { /* index 3, 4 bits */
        return  1 + (hi_u32)ts_data[4]; /* index 4 */
    }

    return 0;
}

static inline hi_u32 dmx_pes_header_len(hi_u8 *ts_data)
{
    WARN_ON(ts_data[0] != 0x47);

    if (ts_data[1] & 0x40) {
        hi_u32 adp_len = dmx_adp_fld_len(ts_data);
        /* 4 is ts packet header length, 8 meanse pes header fild is  the 8th bytes of pes header. */
        return PES_PKT_HEADER_LENGTH + (hi_u32)ts_data[4 + adp_len + 8];
    }

    return 0;
}

static inline hi_u32 dmx_get_pid(hi_u8 *ts_data)
{
    WARN_ON(*ts_data != 0x47);
    /*
     * Pid is 13 bits, it is composed of the lower 5 bits of the 2th byte of ts_data shift left 8 bits
     * and the 3ht byte of ts_data.
     */
    return ((*(ts_data + 1) & 0x1F) << 8) | *(ts_data + 2);
}

hi_s32 dmx_parser_scdata(struct dmx_r_rec_fct *rrec_fct, hi_u64 parser_offset,
    hi_u8 *scdata_buf, hi_u32 scdata_buf_len)
{
    hi_u64 frame_global_offset;
    hi_u8 *data, *data_end;
    hi_u32 dest_idx, src_idx;
    hi_u32 ts_len, ts_offset;
    hi_u8 *ts_rec_buf = rrec_fct->rbuf->buf_ker_addr;
    hi_u32 ts_rec_buf_size = rrec_fct->rbuf->buf_size;

    frame_global_offset = parser_offset;
    data = (hi_u8 *)(do_div(frame_global_offset, (hi_u64)ts_rec_buf_size) + ts_rec_buf);
    data_end = ts_rec_buf + ts_rec_buf_size;

    if (rrec_fct->rec_time_stamp == DMX_REC_TIMESTAMP_NONE) {
        ts_len = DMX_TS_PKT_SIZE;
        ts_offset = 0;
    } else {
        ts_len = DMX_TTS_PKT_SIZE;
        ts_offset = TTS_SYNC_BYTE_OFFSET;
    }

    for (dest_idx = 0, src_idx = 0; dest_idx < scdata_buf_len; src_idx++, data++) {
        hi_u8 *offset_data;

        /* buf rewind */
        if (data >= data_end) {
            data = ts_rec_buf;
        }

        frame_global_offset = parser_offset + src_idx;
        offset_data = data + ts_offset;
        if (unlikely(offset_data < data_end && *offset_data == TS_SYNC_BYTE &&
            do_div(frame_global_offset, ts_len) == 0)) {
            if (dmx_get_pid(data + ts_offset) != rrec_fct->index_src_pid) {
                /* skip entire ts pkt, continue will increase src_idx immeidately, so here sub 1 */
                src_idx += (ts_len) - 1;
                data += (ts_len) - 1;

                continue;
            } else { /* skip ts header,adp, pes header field. */
                hi_u32 skip_len = ts_offset + TS_PKT_HEADER_LEN +
                    dmx_adp_fld_len(offset_data) + dmx_pes_header_len(offset_data);

                src_idx += skip_len - 1;  /* continue will increase src_idx immeidately, so here sub 1 */
                data += skip_len - 1;

                continue;
            }
        }

        scdata_buf[dest_idx++] = *data;
    }

    return HI_SUCCESS;
}

hi_s32 dmx_parser_filter_scdata(struct dmx_r_rec_fct *rrec_fct, hi_u64 parser_offset,
    hi_u8 *scdata_buf, hi_u32 scdata_buf_len, hi_u32 *mono_parse_len, hi_u32 *real_data_len)
{
    hi_u64 frame_global_offset;
    hi_u8 *data, *data_end;
    hi_u32 dest_idx, src_idx;
    hi_u32 ts_len, ts_offset;
    hi_u8 *ts_rec_buf = rrec_fct->rbuf->buf_ker_addr;
    hi_u32 ts_rec_buf_size = rrec_fct->rbuf->buf_size;

    frame_global_offset = parser_offset;
    data = (hi_u8 *)(do_div(frame_global_offset, (hi_u64)ts_rec_buf_size) + ts_rec_buf);
    data_end = ts_rec_buf + ts_rec_buf_size;

    if (rrec_fct->rec_time_stamp == DMX_REC_TIMESTAMP_NONE) {
        ts_len = DMX_TS_PKT_SIZE;
        ts_offset = 0;
    } else {
        ts_len = DMX_TTS_PKT_SIZE;
        ts_offset = TTS_SYNC_BYTE_OFFSET;
    }

    for (src_idx = 0, dest_idx = 0; dest_idx < scdata_buf_len; data++, src_idx++) {
        hi_u8 *offset_data;

        /* might_sleep(cond_resched) for avoid blocking here too long time */
        cond_resched();

        /* buf rewind */
        if (data == data_end) {
            data = ts_rec_buf;
        }

        frame_global_offset = parser_offset + src_idx;
        offset_data = data + ts_offset;
        if (unlikely(offset_data < data_end && *offset_data == TS_SYNC_BYTE &&
            do_div(frame_global_offset, ts_len) == 0)) {
            if (dmx_get_pid(data + ts_offset) != rrec_fct->index_src_pid) {
                /* skip entire ts pkt, continue will increase src_idx immeidately, so here sub 1 */
                src_idx += (ts_len) - 1;
                data += (ts_len) - 1;

                continue;
            } else { /* skip ts header,adp, pes header field. */
                hi_u32 skip_len = ts_offset + TS_PKT_HEADER_LEN +
                    dmx_adp_fld_len(offset_data) + dmx_pes_header_len(offset_data);

                src_idx += skip_len - 1;  /* continue will increase src_idx immeidately, so here sub 1 */
                data += skip_len - 1;

                continue;
            }
        }

        scdata_buf[dest_idx++] = *data;

        /* according to hevc protocol , key pair('00 00 03') '03' means emulation_prevention_three_byte,
         * need to be deleted.
         */
        if (dest_idx >= 0x3 && scdata_buf[dest_idx - 1 - 0x2] == 0
                && scdata_buf[dest_idx - 1 - 1] == 0 && scdata_buf[dest_idx - 1] == 3) { /* prevention 3 byte */
            dest_idx--;
            continue;
        }

        /* according to hevc protocol, reach the next start code prefix(0x000001),
         * we consider it as the end of this nal unit
         */
        if (dest_idx >= 0x6 && scdata_buf[dest_idx - 1 - 0x2] == 0
                && scdata_buf[dest_idx - 1 - 1] == 0 && scdata_buf[dest_idx - 1] == 1) {
            break;
        }
    }

    if (mono_parse_len) {
        *mono_parse_len = src_idx;
    }

    if (real_data_len) {
        *real_data_len = dest_idx;
    }

    return HI_SUCCESS;
}

static hi_s32 dmx_fixup_hevc_es_index(struct dmx_r_rec_fct *rrec_fct, findex_scd *pst_fidx)
{
    hi_s32 ret;
    hi_u8  *data_after_sc, *dup_data_buf;
    hi_u32 loop_times = 0;
    hi_u32 ts_len;

    WARN_ON(pst_fidx->extra_scdata_size != (sizeof(hi_u8) * HEVC_DUP_DATA_TOTAL_LEN));

    dup_data_buf = pst_fidx->extra_scdata;

    ret = dmx_parser_scdata(rrec_fct, pst_fidx->global_offset, dup_data_buf, HEVC_DUP_DATA_CMP_LEN);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* verify start code first. */
    if (unlikely(!(dup_data_buf[0] == 0x0 && dup_data_buf[1] == 0x0 && dup_data_buf[2] == 0x1 &&
                   pst_fidx->start_code == dup_data_buf[0x3]))) {
        HI_ERR_DEMUX("invalid start code(0x%02x 0x%02x 0x%02x 0x%02x) at offset(0x%llx).\n",
                     dup_data_buf[0], dup_data_buf[1], dup_data_buf[0x2], dup_data_buf[0x3], pst_fidx->global_offset);
        ret = HI_FAILURE;
        goto out;
    }

    data_after_sc = dup_data_buf + HEVC_DATA_OF_SC_OFFSET;

    /* verify saved bytes */
    if (unlikely(memcmp(data_after_sc, pst_fidx->data_after_sc, sizeof(hi_u8) * HEVC_DATA_OF_SC_SAVED_LEN))) {
        HI_ERR_DEMUX("dismatched bytes(offset:%llx):0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x.\n",
                     pst_fidx->global_offset, data_after_sc[0], data_after_sc[1], data_after_sc[0x2],
                     data_after_sc[0x3], data_after_sc[0x4], data_after_sc[0x5],
                     data_after_sc[0x6], data_after_sc[0x7]);

        HI_ERR_DEMUX("saved bytes(offset:%llx):0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x.\n",
                     pst_fidx->global_offset, pst_fidx->data_after_sc[0], pst_fidx->data_after_sc[1],
                     pst_fidx->data_after_sc[0x2], pst_fidx->data_after_sc[0x3], pst_fidx->data_after_sc[0x4],
                     pst_fidx->data_after_sc[0x5], pst_fidx->data_after_sc[0x6], pst_fidx->data_after_sc[0x7]);

        ret = HI_FAILURE;
        goto out;
    }

    if (rrec_fct->rec_time_stamp == DMX_REC_TIMESTAMP_NONE) {
        ts_len = DMX_TS_PKT_SIZE;
    } else {
        ts_len = DMX_TTS_PKT_SIZE;
    }

    /* verify received enough data */
    do {
        hi_u64 total_ts_rec_cnt = 0;
        hi_u32 mono_parse_len = 0;
        hi_u32 real_parse_len = 0;

        ret = dmx_parser_filter_scdata(rrec_fct, pst_fidx->global_offset, dup_data_buf,
                                       HEVC_DUP_DATA_TOTAL_LEN, &mono_parse_len, &real_parse_len);
        if (ret != HI_SUCCESS) {
            goto out;
        }

        pst_fidx->extra_real_scdata_size = real_parse_len;
        /* ensure dup_data_buf is valid data. */
        total_ts_rec_cnt = dmx_hal_scd_get_rec_ts_cnt(rrec_fct->base.mgmt, rrec_fct->base.id);
        if (total_ts_rec_cnt * ts_len - pst_fidx->global_offset >= mono_parse_len) {
            break;
        } else {
            /* wait 1s/2s/4s/8s/16s */
            if (loop_times == 10 || loop_times == 20 || loop_times == 40 || loop_times == 80 || loop_times == 160) {
                HI_ERR_DEMUX("not receive enough index data within %d seconds.\n", loop_times / 10);

                if (loop_times == 160) { /* max loop time 160 */
                    HI_ERR_DEMUX("escape wait index data.\n");
                    ret = HI_FAILURE;
                    goto out;
                }
            }
            loop_times++;

            if (osal_msleep(5)) { /* 5 ms */
                ret = HI_FAILURE;
                goto out;
            }
        }
    } while (1);

out:
    return ret;
}

hi_s32 dmx_fixup_hevc_index(struct dmx_r_rec_fct *rrec_fct, findex_scd *pst_fidx)
{
    hi_s32 ret = HI_FAILURE;

    if (pst_fidx->index_type == DMX_INDEX_SC_TYPE_PIC_FOR_TS) {
        ret = dmx_fixup_hevc_es_index(rrec_fct, pst_fidx);
    } else if (pst_fidx->index_type == DMX_INDEX_SC_TYPE_PIC_SHORT_FOR_TS) {
        WARN(1, "Invalid index type[%#x]!\n", pst_fidx->index_type);
    } else {
        ret = HI_SUCCESS;
    }

    return ret;
}

hi_s32 dmx_fixup_secure_hevc_index(struct dmx_r_rec_fct *rrec_fct, const findex_scd *pst_fidx)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 parse_offset;
    hi_u64 frame_global_offset;

    if (pst_fidx->index_type == DMX_INDEX_SC_TYPE_PIC_FOR_TS || pst_fidx->index_type == DMX_INDEX_SC_TYPE_PTS_FOR_TS) {
        frame_global_offset = pst_fidx->global_offset;
        parse_offset = do_div(frame_global_offset, (hi_u64)rrec_fct->rbuf->buf_size);
        ret = dmx_tee_fixup_hevc_es_index(rrec_fct->base.id, rrec_fct->index_src_pid, parse_offset,
                                          pst_fidx, &rrec_fct->last_frame_info);
    } else if (pst_fidx->index_type == DMX_INDEX_SC_TYPE_PIC_SHORT_FOR_TS) {
        WARN(1, "Invalid index type[%#x]!\n", pst_fidx->index_type);
    } else {
        ret = HI_SUCCESS;
    }

    return ret;
}

hi_s32 dmx_scd_to_audio_index(dmx_index_data *curr_frame, const dmx_index_scd *scd_index)
{
    hi_s64  pts;
    hi_u32  index_type;
    hi_u32  pts_valid;
    hi_u32  pts33;
    hi_u32  pts32;
    hi_u32  pts_value;
    hi_u64  ts_count;
    hi_u32  offset_in_ts;
    hi_u32  back_ts_count;

    DMX_NULL_POINTER_RETURN(curr_frame);
    DMX_NULL_POINTER_RETURN(scd_index);

    index_type  = (scd_index->scd_type_ovflag_goback_num >> 29) & 0xf;
    pts_valid   = (scd_index->ts_cnt_hi8_byte345after_sc >> 1) & 0x1;
    pts33       = scd_index->ts_cnt_hi8_byte345after_sc & 0x1;
    pts32       = scd_index->sc_code_byte678after_sc;

    if (index_type != DMX_INDEX_SC_TYPE_PTS_FOR_TS) {
        return HI_FAILURE;
    }

    if (pts33) {
        pts = 0x100000000ULL;
    }

    pts += (hi_u64)pts32;
    pts = pts * MS_2_US;
    pts = pts >> 1;
    pts_value = (hi_u32)pts;
    pts_value /= 45;

    offset_in_ts  = scd_index->sc_type_byte12after_sc_offset_in_ts & 0xff;
    ts_count     = (hi_u64)(scd_index->ts_cnt_hi8_byte345after_sc & 0xff000000) << 8;
    ts_count     = ts_count | scd_index->ts_cnt_lo32;
    back_ts_count = scd_index->scd_type_ovflag_goback_num & 0x00ffffff;

    curr_frame->frame_type      = DMX_FRAME_TYPE_I;
    curr_frame->pts_us         = pts_valid ? pts_value : DMX_INDEX_INVALID_PTS_US;
    curr_frame->global_offset  = (ts_count - back_ts_count - 1) * 188 + offset_in_ts; /* Ts packet length is 188 */
    curr_frame->data_time_ms    = scd_index->scr_clk / 90; /* 90KHZ */

    return HI_SUCCESS;
}

hi_void dmx_rec_update_frame_info(hi_u32 *param, frame_pos *index_info)
{
    hi_u32 idx;
    dmx_index_data *frame_info = HI_NULL;

    DMX_NULL_POINTER_VOID(param);
    DMX_NULL_POINTER_VOID(index_info);

    frame_info = (dmx_index_data *)param;

    switch (index_info->frame_type) {
        case FIDX_FRAME_TYPE_I: {
                frame_info->frame_type = DMX_FRAME_TYPE_I;
                break;
            }
        case FIDX_FRAME_TYPE_P: {
                frame_info->frame_type = DMX_FRAME_TYPE_P;
                break;
            }
        case FIDX_FRAME_TYPE_B: {
                frame_info->frame_type = DMX_FRAME_TYPE_B;
                break;
            }
        case FIDX_FRAME_TYPE_IDR: {
                frame_info->frame_type = DMX_FRAME_TYPE_IDR;
                break;
            }
        case FIDX_FRAME_TYPE_BLA: {
                frame_info->frame_type = DMX_FRAME_TYPE_BLA;
                break;
            }
        case FIDX_FRAME_TYPE_CRA: {
                frame_info->frame_type = DMX_FRAME_TYPE_CRA;
                break;
            }

        default :
            return;
    }

    frame_info->pts_us          = index_info->pts;
    frame_info->global_offset   = (hi_u64)index_info->global_offset;
    frame_info->frame_size      = (hi_u32)index_info->frame_size;

    frame_info->cur_poc = (hi_s16)index_info->cur_poc;
    frame_info->ref_poc_cnt = (hi_u16)index_info->ref_num;
    for (idx = 0; idx < frame_info->ref_poc_cnt; idx++) {
        frame_info->ref_poc[idx] = (hi_s16)index_info->ref_poc[idx];
    }
}

