/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#ifndef _VDP_BASE_DEFINE_H_
#define _VDP_BASE_DEFINE_H_

#include "hi_type.h"

#define XDR_ENGINE_NUM          6
#define FDR_REGION_NUM_MAX      64
#define DISP_CHECKSUM_INDEX_MAX 16

typedef enum tag_vdp_intmask {
    VDP_INTMSK_NONE = 0,

    VDP_INTMSK_DHD0_VTTHD = 0x1,
    VDP_INTMSK_DHD0_VTTHD2 = 0x2,
    VDP_INTMSK_DHD0_VTTHD3 = 0x4,
    VDP_INTMSK_DHD0_UFINT = 0x8,

    VDP_INTMSK_DHD1_VTTHD = 0x10,
    VDP_INTMSK_DHD1_VTTHD2 = 0x20,
    VDP_INTMSK_DHD1_VTTHD3 = 0x40,
    VDP_INTMSK_DHD1_UFINT = 0x80,

    VDP_INTMSK_WBC_GP0_INT = 0x100,
    VDP_INTMSK_WBC_HD0_INT = 0x1000,
    VDP_INTMSK_WBC_G0_INT = 0x400,
    VDP_INTMSK_WBC_G4_INT = 0x800,

    VDP_INTMSK_VDAC0_INT = 0x10000,
    VDP_INTMSK_VDAC1_INT = 0x20000,
    VDP_INTMSK_VDAC2_INT = 0x40000,
    VDP_INTMSK_VDAC3_INT = 0x80000,

    VDP_INTMSK_ON_OFFLINE_SW_INT = 0x2000,
    VDP_INTMSK_ON_OFFLINE_ERR_INT = 0x4000,

    VDP_INTMSK_DSD0_VTTHD = 0x10000,
    VDP_INTMSK_DSD0_UFINT = 0x20000,

    VDP_INTMSK_DTV_BACK_INT = 0x100000,
    VDP_INTMSK_DTV_FRM_CHANGE_INT = 0x200000,
    VDP_INTMSK_DTV_START_INT = 0x400000,
    VDP_INTMSK_G0RRINT = 0x800000,

    VDP_INTMSK_G1RRINT = 0x1000000,

    VDP_INTMSK_LOWDLY_INT = 0x20000000,
    VDP_INTMSK_UTENDINT = 0x40000000,
    VDP_INTMSK_BUSEERINT = 0x80000000,

    HAL_INTMSK_ALL = 0xffffffff
} vdp_intmask;

typedef enum {
    INK_POS_V0_CSC_F = 0,
    INK_POS_V0_CSC_B,
    INK_POS_V1_CSC_F,
    INK_POS_V1_CSC_B,
    INK_POS_V3_CSC_F,
    INK_POS_V3_CSC_B,
    INK_POS_BUTT,
} disp_ink_pos;

typedef enum {
    INK_COLOR_REVERSE = 0,
    INK_COLOR_BLACK,
    INK_COLOR_WHITHE,
    INK_COLOR_RED,
    INK_COLOR_BUTT,
} ink_colormode;

typedef struct {
    hi_u32 red;
    hi_u32 greern;
    hi_u32 blue;
} disp_color;

typedef struct {
    hi_bool ink_enable;
    hi_bool cross_display_enable;
    hi_u32 x_pos;
    hi_u32 y_pos;
    disp_ink_pos ink_pos;
    ink_colormode ink_color_mode;
} disp_ink_attr;

typedef struct tag_smmu_error_addr {
    hi_u32 master_wr_addr_ns;
    hi_u32 master_rd_addr_ns;
    hi_u32 master_wr_addr;
    hi_u32 master_rd_addr;
} smmu_error_addr;

typedef enum tag_disp_intf_venc {
    DISP_VENC_HDATE0 = 0,
    DISP_VENC_SDATE0,
    DISP_VENC_VGA0,
    DISP_VENC_BUTT
} disp_intf_venc;

/* ----------testpattern----------- */
typedef enum {
    VDP_FDR_TESTPTTN_MODE_STATIC,
    VDP_FDR_TESTPTTN_MODE_RAND,
    VDP_FDR_TESTPTTN_MODE_DYNAMIC,
    VDP_FDR_TESTPTTN_MODE_BUTT
} vdp_fdr_testpttn_mode;

typedef enum {
    VDP_FDR_TESTPTTN_CLR_MODE_FIX = 0,
    VDP_FDR_TESTPTTN_CLR_MODE_NORM,
    VDP_FDR_TESTPTTN_CLR_MODE_BUTT
} vdp_fdr_testpttn_clr_mode;

typedef enum {
    VDP_FDR_TESTPTTN_LINEW_1P = 0,
    VDP_FDR_TESTPTTN_LINEW_2P,
    VDP_FDR_TESTPTTN_LINEW_BUTT
} vdp_fdr_testpttn_linew;

typedef struct {
    hi_bool pattern_en;
    vdp_fdr_testpttn_mode demo_mode;
    vdp_fdr_testpttn_clr_mode color_mode;
    vdp_fdr_testpttn_linew line_width;
    hi_u32 speed;
    hi_u32 seed;
} vdp_fdr_testpattern;

typedef enum tagvdp_data_wth {
    VDP_DATA_WTH_8 = 0,
    VDP_DATA_WTH_10 = 1,
    VDP_DATA_WTH_12 = 2,
    VDP_DATA_WTH_BUTT
} vdp_data_wth;

typedef struct {
    hi_u32 x;
    hi_u32 y;
    hi_u32 w;
    hi_u32 h;
} vdp_rect;

typedef struct {
    hi_u64 lum_addr;
    hi_u64 chm_addr;

    hi_u32 lum_str;
    hi_u32 chm_str;
} vdp_vid_addr;

typedef enum {
    VDP_FDR_VID_ADDR_DATA = 0,  // 2d left eye data
    VDP_FDR_VID_ADDR_DATA_3D,   // 3d right eye data
    VDP_FDR_VID_ADDR_HEAD,      // dcmp head data
    VDP_FDR_VID_ADDR_2B,        // 2bit
    VDP_FDR_VID_ADDR_BUTT,
} vdp_fdr_vid_addr_type;

typedef enum {
    VDP_FDR_MRG_ADDR_DATA = 0,  /* yc cmp head addr is yc addr */
    VDP_FDR_MRG_ADDR_BUTT,
} vdp_fdr_mrg_addr_type;

typedef enum {
    LAYER_ID_VID0 = 0,
    LAYER_ID_VID1 = 1,
    LAYER_ID_VID2 = 2,
    LAYER_ID_VID3 = 3,
    LAYER_ID_BUTT
} layer_vid;

typedef struct tag_vdp_sync_info {
    hi_u32 iop;

    hi_u32 vact;
    hi_u32 vbb;
    hi_u32 vfb;

    hi_u32 hact;
    hi_u32 hbb;
    hi_u32 hfb;

    hi_u32 bvact;
    hi_u32 bvbb;
    hi_u32 bvfb;

    hi_u32 hpw;
    hi_u32 vpw;
    hi_u32 hmid;

    hi_u32 idv;
    hi_u32 ihs;
    hi_u32 ivs;
} vdp_sync_info;

typedef enum tag_clock_module {
    CLOCK_MODULE_VID0 = 0,
    CLOCK_MODULE_VID1,
    CLOCK_MODULE_VID2,
    CLOCK_MODULE_VID3,
    CLOCK_MODULE_VP0,
    CLOCK_MODULE_VP1,
    CLOCK_MODULE_WBC0,
    CLOCK_MODULE_WBC1,
} clock_module;

#endif
