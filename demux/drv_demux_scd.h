/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic function impl.
 * Author: sdk
 * Create: 2017-05-31
 */

#ifndef __DRV_DEMUX_SCD_H__
#define __DRV_DEMUX_SCD_H__

#include "drv_demux_define.h"
#include "drv_demux_index.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* start code type definition(data from SCD buffer) */
/* pes index based on pes  package */
#define DMX_INDEX_SC_TYPE_PTS_FOR_PES          0x0
/* ts packet index (tpit) */
#define DMX_INDEX_SC_TYPE_TS_INDEX             0x1
/* pes index based on ts package */
#define DMX_INDEX_SC_TYPE_PTS_FOR_TS           0x2
/* the long head start 00 00 01 of frame data(es) based on pes package */
#define DMX_INDEX_SC_TYPE_PIC_FOR_PES          0x3
/* the long head start 00 00 01 of frame data(es) based on ts package */
#define DMX_INDEX_SC_TYPE_PIC_FOR_TS           0x4
/* the short head start  00 01 of frame data(es) based on ts package  */
#define DMX_INDEX_SC_TYPE_PIC_SHORT_FOR_TS     0x5
/* the short head start  00 01 of frame data(es) based on pes package  */
#define DMX_INDEX_SC_TYPE_PIC_SHORT_FOR_PES    0x6
/* the header of PES syntax error */
#define DMX_INDEX_SC_TYPE_PES_ERR              0xf

#define HEVC_DATA_OF_SC_OFFSET 4     /* 00 00 01 xx */
#define HEVC_DATA_OF_SC_TOTAL_LEN 256
#define HEVC_DATA_OF_SC_SAVED_LEN 8  /* this bytes has saved in pvr_scddmx_idx_to_pvr_idx. */
#define HEVC_DUP_DATA_CMP_LEN (HEVC_DATA_OF_SC_OFFSET + HEVC_DATA_OF_SC_SAVED_LEN)
#define HEVC_DUP_DATA_TOTAL_LEN (HEVC_DATA_OF_SC_OFFSET + HEVC_DATA_OF_SC_TOTAL_LEN)

/* dmx index's SCD descriptor                                               */
/* format between firmware index and demux scd */
typedef struct {
    hi_u8  index_type;                           /* type of index(pts,sc,pause,ts) */
    hi_u8  start_code;                           /* type of start code */
    hi_u16 offset_in_ts;                         /* start code offset in a TS package */
    hi_u32 offset_in_dav_buf;                     /* start code offset in DAV buffer */
    hi_u8  byte_after_sc[8];                      /* 8byte next to SC */
    hi_u64 tscnt;                              /* count of TS package */
    hi_s64 pts_us;
    hi_u16 over_flow;
    hi_u16 reserv;
} dmx_index_parse;

/* sequence of dmx_idx_data 's member can not change,must match the sequence defined by hardware */
typedef struct {
    /* back package number */ /* cncomment:回退包计数 */
    hi_u32 scd_type_ovflag_goback_num;
    /* the first byte after start code prefix(00 00 01 is the start code type or start code) */
    hi_u32 sc_type_byte12after_sc_offset_in_ts;
    hi_u32 ts_cnt_lo32;
    hi_u32 ts_cnt_hi8_byte345after_sc;
    hi_u32 sc_code_byte678after_sc;
    hi_u32 scr_clk;
    hi_u32 goback_over_ts_head;
    hi_u32 reserved;
} dmx_index_scd;

typedef struct {
    dmx_index_data index;
    struct list_head list;
} dmx_index_helper;

hi_s32  dmx_parser_scdata(struct dmx_r_rec_fct *rrec_fct, hi_u64 parser_offset, hi_u8 *scdata_buf,
    hi_u32 scdata_buf_len);
hi_s32  dmx_parser_filter_scdata(struct dmx_r_rec_fct *rrec_fct, hi_u64 parser_offset, hi_u8 *scdata_buf,
    hi_u32 scdata_buf_len, hi_u32 *mono_parse_len, hi_u32 *real_data_len);
hi_void dmx_rec_update_frame_info(hi_u32 *param, frame_pos *index_info);
hi_s32  dmx_scd_to_video_index(struct dmx_r_rec_fct *rrec_fct, const dmx_index_scd *sc_data, findex_scd *fidx);
hi_s32  dmx_scd_to_audio_index(dmx_index_data *last_frame, const dmx_index_scd *sc_data);

hi_s32  dmx_fixup_hevc_index(struct dmx_r_rec_fct *rrec_fct, findex_scd *pst_fidx);
hi_s32  dmx_fixup_secure_hevc_index(struct dmx_r_rec_fct *rrec_fct, const findex_scd *pst_fidx);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif

