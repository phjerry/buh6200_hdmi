/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: image
* Create: 2019-04-12
 */

#ifndef _VDP_CHIIP_DEFINE_H_
#define _VDP_CHIIP_DEFINE_H_

#include "vdp_base_define.h"

#ifdef __DISP_PLATFORM_BOOT__
#define VDP_PRINT printf
#define VDP_ERROR printf
#else
#include "hi_drv_mem.h"
#include "hi_osal.h"
#define VDP_PRINT osal_printk
#define VDP_ERROR osal_printk
#endif

#define VDP_MAX_WIDTH_8K  8192
#define VDP_TYPE_WIDTH_8K 7680
#define VDP_TYPE_WIDTH_4K 3840
#define VDP_TYPE_WIDTH_2K 1920

#define VDP_MAX_HEIGHT_8K  8192 /* support rotation 8k */
#define VDP_MAX_HEIGHT_4K  4352
#define VDP_TYPE_HEIGHT_4K 4320
#define VDP_TYPE_HEIGHT_2K 2160
#define VDP_TYPE_HEIGHT_1K 1080

#define VDP_TYPE_WIDTH_SHD      7680
#define VDP_TYPE_HEIGHT_SHD     4320

#define VDP_TYPE_WIDTH_UHD      3840
#define VDP_TYPE_HEIGHT_UHD     2160

#define VDP_TYPE_WIDTH_FHD      1920
#define VDP_TYPE_HEIGHT_FHD     1080

#define VDP_TYPE_WIDTH_HD       1280
#define VDP_TYPE_HEIGHT_HD      720

#define VDP_TYPE_WIDTH_SD       720
#define VDP_TYPE_HEIGHT_SD      576

#define VDP_BASE_ADDR 0x00f00000
#define VDP_CRG_BASE_ADDR  0x00A00000

#define VDP_LAYER_ALIGN 8

#define VID_MAX  5
#define GFX_MAX  4
#define WBC_MAX  5
#define CHN_MAX  2
#define INTF_MAX 9
#define GP_MAX   2
#define VP_MAX   2
#define CBM_MAX  6  // = max_mix_layer_num + 1
#define PARA_MAX 2

// VID
#define MIXV0_EN         1  // if VP0_EN = 0, MIXV0_EN must 0
#define VID_CROP_EN      1
#define VID_UNPACK_DRAW  1
#define VID_DCMP_EN      0
#define ZME_NON_LINEAR   1
#define VID_DRAW_EN      1
#define VID_SHARP_EN     1
#define VID_DCI_EN       1
#define VID_SHARP_FHD_EN 0
#define VID_ES_EN        0
#define VP0_CSC_EN       0  // define csc0 & csc1 in one csc module
#define VP0_ACC_EN       0
#define VP0_WCG_EN       1
#define VDP_TILE_DCMP    1

// WBC
#define WBC_CROP_EN 0
#define WBC_CSC_EN  1
#define WBC_GP0_SEL 1

// DHD
#define SR_EN          1
#define SR_COEF_AXI    0  // sr coef axi
#define SR_COEF_APB    1
#define ZME_NON_LINEAR   1
#define VGA_CLIP       0
#define DIMMING_EN     1
#define VDP_HIST_EN    1

// -----------------------------------
// OFFSET define, for driver
// -----------------------------------
#define VID0_VID1_OFFSET     (0x2f000 / 4)
#define VID_OFFSET           (0x1000 / 4)
#define GFX_OFFSET           (0x800 / 4)
#define VP_OFFSET            (0x800 / 4)
#define GP_OFFSET            (0x800 / 4)
#define MIX_OFFSET           (0x80 / 4)
#define CHN_OFFSET           (0x200 / 4)
#define INTF_OFFSET          (0x100 / 4)
#define WBC_OFFSET           (0x400 / 4)
#define SHARPEN_OFFSET       (0x400 / 4)
#define VID_VMX_OFFSET       (0x100 / 4)
#define VPSS_REG_OFF         (0x800 / 4)
#define VID0_VID1_FDR_OFFSET (0x30000 / 4)
#define FDR_VID_OFFSET       (0x200 / 4)
#define FDR_GFX_OFFSET       (0x200 / 4)
#define CAP_WBC_OFFSET       (0x200 / 4)


#define MASTER0_OFFSET     (0x00000 / 4)
#define MASTER1_OFFSET     (0x30000 / 4)
// hdr
#define VID0_HDR_OFFSET     (0x2f000 / 4)
#define VID1_HDR_OFFSET     (0x00000 / 4)
// smmu
#define VDP_MMU0_OFFSET     (0x30000 / 4)
#define VDP_MMU1_OFFSET     (0x800 / 4)


#define VDP_AHB_BASE_ADDR     0x20900000
#define HDDATE_BASE_ADDR      (VDP_BASE_ADDR + 0xf000)
#define SDDATE_BASE_ADDR      (VDP_BASE_ADDR + 0xce00)
#define INT_REG_ADDR          (VDP_BASE_ADDR + 0x8)
#define INT_STA_ADDR          (VDP_BASE_ADDR + 0x4)
#define ON_OFFLINE_STATE_ADDR (VDP_BASE_ADDR + 0x108)

#define OFL_INT_REG_ADDR   (VDP_BASE_ADDR + 0x210)
#define OFL_INT_STA_ADDR   (VDP_BASE_ADDR + 0x20c)
#define VDP_CUR_STATE_ADDR (VDP_BASE_ADDR + 0x108)

#define VDP_IP_BASE_ADDR    (0x10000 / 4)
#define GFX0_OSD_DCMP_ERR   (VDP_BASE_ADDR + 0xa944)
#define GFX1_OSD_DCMP_ERR   (VDP_BASE_ADDR + 0xab44)
#define GFX3_OSD_DCMP_ERR   (VDP_BASE_ADDR + 0xaf44)
#define VID_TILE_LDCMP_DBG0 (VDP_BASE_ADDR + 0x3a3a4)
#define VID_TILE_LDCMP_DBG1 (VDP_BASE_ADDR + 0x3a3a8)

typedef enum {
    VDP_AXI_EDGE_MODE_128 = 0,
    VDP_AXI_EDGE_MODE_256,
    VDP_AXI_EDGE_MODE_1024,
    VDP_AXI_EDGE_MODE_2048,
    VDP_AXI_EDGE_MODE_4096,
    VDP_AXI_EDGE_MODE_BUTT
} vdp_axi_edge_mode;

typedef enum tag_vdp_master_arb_mode {
    VDP_MASTER_ARB_MODE_ASK = 0,
    VDP_MASTER_ARB_MODE_GFX_PRE,
    VDP_MASTER_ARB_MODE_BUTT,
} vdp_master_arb_mode;

typedef enum tag_vdp_axi_cmd {
    VDP_AXI_CMD_RID0 = 0,
    VDP_AXI_CMD_RID1,
    VDP_AXI_CMD_WID0,

    VDP_AXI_CMD_RID0OTD,
    VDP_AXI_CMD_RID1OTD,
    VDP_AXI_CMD_WID0OTD,
    VDP_AXI_CMD_BUTT
} vdp_axi_cmd;

typedef enum tag_vdp_layer_vp {
    VDP_LAYER_VP0 = 0,
    VDP_LAYER_VP1 = 1,
    VDP_LAYER_DHD0 = 2,
    VDP_LAYER_VP_BUTT
} vdp_layer_vp;

typedef enum tag_vdp_chn {
    VDP_CHN_DHD0 = 0,
    VDP_CHN_DHD1 = 1,
    VDP_CHN_DSD0 = 2,
    VDP_CHN_WBC0 = 3,
    VDP_CHN_WBC1 = 4,
    VDP_CHN_WBC2 = 5,
    VDP_CHN_WBC3 = 6,
    VDP_CHN_VDPB = 7,
    VDP_CHN_NONE = 8,
    VDP_CHN_BUTT
} vdp_chn;

typedef enum tag_vdp_data_rmode {
    VDP_RMODE_INTERFACE = 0,
    VDP_RMODE_INTERLACE = 0,
    VDP_RMODE_PROGRESSIVE = 1,
    VDP_RMODE_TOP = 2,
    VDP_RMODE_BOTTOM = 3,
    VDP_RMODE_PRO_TOP = 4,
    VDP_RMODE_PRO_BOTTOM = 5,
    VDP_RMODE_BUTT

} vdp_data_rmode;

typedef enum {
    VDP_RMODE_3D_INTF = 0,
    VDP_RMODE_3D_SRC = 1,
    VDP_RMODE_3D_BUTT
} vdp_rmode_3_d;

typedef enum tag_vdp_proc_fmt{
    VDP_PROC_FMT_SP_420 = 0x0,
    VDP_PROC_FMT_SP_422 = 0x1,
    VDP_PROC_FMT_SP_444 = 0x2,   // plannar,in YUV color domain
    VDP_PROC_FMT_SP_400 = 0x3,   // plannar,in YUV color domain
    VDP_PROC_FMT_RGB_888 = 0x3,  // package,in RGB color domain
    VDP_PROC_FMT_RGB_444 = 0x4,  // plannar,in RGB color domain
    VDP_PROC_FMT_BUTT
} vdp_proc_fmt;

typedef enum tag_vdp_req_lenght {
    VDP_REQ_LENGTH_16 = 0,
    VDP_REQ_LENGTH_8 = 1,
    VDP_REQ_LENGTH_BUTT
} vdp_req_length;

typedef enum {
    VDP_DRAW_MODE_1 = 0,
    VDP_DRAW_MODE_2,
    VDP_DRAW_MODE_4,
    VDP_DRAW_MODE_8,
    VDP_DRAW_MODE_16,
    VDP_DRAW_MODE_32,
    VDP_DRAW_MODE_BUTT
} vdp_draw_mode;

typedef enum tag_vdp_vp_para {
    VDP_VP_PARA_ACM = 0x1,         // for coef read and para up
    VDP_VP_PARA_DCI_HBW = 0x2,     // for coef read
    VDP_VP_PARA_DCI_DIV = 0x4,     // for coef read
    VDP_VP_PARA_DCI_LUT = 0x8,     // for coef read
    VDP_VP_PARA_ABC = 0x9,         // for para up
    VDP_VP_PARA_DCI = 0x9,         // for para up
    VDP_VP_PARA_SHARP = 0x10,      // for coef read
    VDP_VP_PARA_SHARP_FHD = 0x10,  // for coef read
    VDP_VP_PARA_BUTT
} vdp_vp_para;

typedef enum {
    VDP_REQ_CTRL_16BURST_1 = 0,
    VDP_REQ_CTRL_16BURST_2,
    VDP_REQ_CTRL_16BURST_4,
    VDP_REQ_CTRL_16BURST_8,
    VDP_REQ_CTRL_BUTT
} vdp_req_ctrl;

typedef enum tag_vdp_cbm_mix {
    VDP_CBM_MIXV0 = 0,
    VDP_CBM_MIXV1 = 1,
    VDP_CBM_MIXG0 = 2,
    VDP_CBM_MIXG1 = 3,
    VDP_CBM_MIX0 = 4,
    VDP_CBM_MIX1 = 5,
    VDP_CBM_MIX2 = 6,
    VDP_CBM_MIX_BUTT
} vdp_cbm_mix;

typedef enum tag_vdp_cbm_layer {
    // CV300 CBM0
    VDP_CBM0_VP0 = 0x1,
    VDP_CBM0_GP0 = 0x2,
    VDP_CBM0_GFX2 = 0x3,

    // CV300 CBM1
    VDP_CBM1_VID3 = 0x1,
    VDP_CBM1_GFX3 = 0x2,
    VDP_CBM1_VID2 = 0x3,
    VDP_CBM1_GFX2 = 0x4,

    // CV300 MIXV0
    VDP_MIX0_VID0 = 0x1,
    VDP_MIX0_VID1 = 0x2,
    VDP_MIX0_VID2 = 0x3,

    // CV300 MIXG0
    VDP_MIX0_GFX0 = 0x1,
    VDP_MIX0_GFX1 = 0x2,
    VDP_MIX0_GFX2 = 0x3,

    VDP_CBM_BUTT
} vdp_cbm_layer;

typedef enum {
    // zreo means no layer driver;
    VDP_PRIO_MIXV0_VID0 = 0x1,
    VDP_PRIO_MIXV0_VID1 = 0x2,
    VDP_PRIO_MIXV0_VID2 = 0x3,

    VDP_PRIO_MIXG0_GFX0 = 0x1,
    VDP_PRIO_MIXG0_GFX1 = 0x2,
    VDP_PRIO_MIXG0_GFX2 = 0x3,

    // DHD0 with VP/GP
    VDP_PRIO_CBM0_VP0 = 0x1,
    VDP_PRIO_CBM0_GP0 = 0x2,
    VDP_PRIO_CBM0_GFX2 = 0x3,

    // DHD1
    VDP_PRIO_CBM1_VID3 = 0x1,
    VDP_PRIO_CBM1_GFX3 = 0x2,
    VDP_PRIO_CBM1_VID2 = 0x3,
    VDP_PRIO_CBM1_GFX2 = 0x4,
} vdp_prio;

typedef enum tag_vdp_disp_digfmt {
    VDP_DISP_DIGFMT_PAL = 0,
    VDP_DISP_DIGFMT_NTSC,
    VDP_DISP_DIGFMT_1080P50,
    VDP_DISP_DIGFMT_1080P60,
    VDP_DISP_DIGFMT_1080P24,
    VDP_DISP_DIGFMT_1080P120,
    VDP_DISP_DIGFMT_1080I50,
    VDP_DISP_DIGFMT_1080I60,
    VDP_DISP_DIGFMT_720P50,
    VDP_DISP_DIGFMT_720P60,
    VDP_DISP_DIGFMT_800x600,
    VDP_DISP_DIGFMT_576P,
    VDP_DISP_DIGFMT_576I,
    VDP_DISP_DIGFMT_480P,
    VDP_DISP_DIGFMT_480I,
    VDP_DISP_DIGFMT_768P60,
    VDP_DISP_DIGFMT_1600P60,       // 2560x1600
    VDP_DISP_DIGFMT_2560x1080P60,  // 2560x1080
    VDP_DISP_DIGFMT_2160P50,       // 3840x2160

    VDP_DISP_DIGFMT_2160P60,      // 3840x2160
    VDP_DISP_DIGFMT_2160P60_HBI,  // 3840x2160
    VDP_DISP_DIGFMT_2160P30,      // 3840x2160
    VDP_DISP_DIGFMT_2160P120,     // 3840x2160@120
    VDP_DISP_DIGFMT_4K1KP120,     // 3840x1080@120
    VDP_DISP_DIGFMT_2160P240,     // 3840x540@240
    VDP_DISP_DIGFMT_4320P30,      // 7680x4320@30
    VDP_DISP_DIGFMT_4320P60,      // 7680x4320@60
    VDP_DISP_DIGFMT_4320P120,     // 7680x4320@120

    VDP_DISP_DIGFMT_4320P30_FPGA,   // 4K@30
    VDP_DISP_DIGFMT_4320P60_FPGA,   // 4K@60
    VDP_DISP_DIGFMT_4320P120_FPGA,  // 4K@120

    VDP_DISP_DIGFMT_QVGA,  // 320x240@60
    VDP_DISP_DIGFMT_TESTI,
    VDP_DISP_DIGFMT_TESTP,
    VDP_DISP_DIGFMT_TESTS,
    VDP_DISP_DIGFMT_TESTUT,
    VDP_DISP_DIGFMT_DEBUG_4320P60,  // 7680x4320@60

    VDP_DISP_DIGFMT_BUTT
} vdp_disp_digfmt;


typedef enum tag_vdp_disp_intf {
    VDP_DISP_INTF_HDMI = 0,
    VDP_DISP_INTF_VGA = 1,
    VDP_DISP_INTF_BT1120 = 2,
    VDP_DISP_INTF_BT656 = 3,
    VDP_DISP_INTF_LCD = 4,
    VDP_DISP_INTF_HDDATE = 5,
    VDP_DISP_INTF_MIPI = 6,
    VDP_DISP_INTF_MIPI1 = 7,

    VDP_DISP_INTF_VBO_0 = 9,
    VDP_DISP_INTF_VBO_1 = 11,
    VDP_DISP_INTF_VBO_2 = 12,
    VDP_DISP_INTF_VBO_3 = 13,
    VDP_DISP_INTF_VBO_4 = 14,
    VDP_DISP_INTF_VBO_5 = 15,
    VDP_DISP_INTF_VBO_6 = 16,
    VDP_DISP_INTF_VBO_7 = 17,
    VDP_DISP_INTF_HDMI0 = 18,

    VDP_DISP_INTF_HDMI1 = 26,
    VDP_DISP_INTF_VBO = 27,
    VDP_DISP_INTF_VBO1 = 28,
    VDP_DISP_INTF_VBO1_0 = 29,
    VDP_DISP_INTF_VBO1_1 = 30,
    VDP_DISP_INTF_VBO1_2 = 31,
    VDP_DISP_INTF_VBO1_3 = 32,

    VDP_DISP_INTF_SDDATE = 40,
    VDP_DISP_INTF_CVBS = 41,
    VDP_DISP_INTF_BUTT,
} vdp_disp_intf;

typedef enum tag_vdp_disp_para {
    VDP_DISP_PARA_GMM_R = 0x0,  // for coef read
    VDP_DISP_PARA_GMM_G = 0x2,  // for coef read
    VDP_DISP_PARA_GMM_B = 0x4,  // for coef read
    VDP_DISP_PARA_GMM = 0x8,    // for paraup

    VDP_DISP_PARA_ZME_HORL = 0x10,  // for coef read and para up
    VDP_DISP_PARA_ZME_HORC = 0x20,  // for coef read and para up
    VDP_DISP_PARA_ZME_VERL = 0x40,  // for coef read and para up
    VDP_DISP_PARA_ZME_VERC = 0x80,  // for coef read and para up

    VDP_DISP_PARA_ZME_HOR = 0x81,  // for para up
    VDP_DISP_PARA_ZME_VER = 0x82,  // for para up

    VDP_DISP_PARA_DIM_LUT = 0x100,  // for coef read and para up
    VDP_DISP_PARA_DIM_HF = 0x200,   // for coef read and para up
    VDP_DISP_PARA_DIM_VF = 0x400,   // for coef read and para up
    VDP_DISP_PARA_BUTT
} vdp_disp_para;

// -----------------------------------
// IP define
// -----------------------------------
typedef enum tag_vdp_dither {
    VDP_DITHER_DROP_10 = 0,
    VDP_DITHER_TMP_10 = 1,
    VDP_DITHER_SPA_10 = 2,
    VDP_DITHER_TMP_SPA_8 = 3,
    VDP_DITHER_ROUND_10 = 4,
    VDP_DITHER_ROUND_8 = 5,
    VDP_DITHER_DISEN = 6,
} vdp_dither;

typedef struct {
    hi_u32 dither_coef0;
    hi_u32 dither_coef1;
    hi_u32 dither_coef2;
    hi_u32 dither_coef3;

    hi_u32 dither_coef4;
    hi_u32 dither_coef5;
    hi_u32 dither_coef6;
    hi_u32 dither_coef7;
} vdp_dither_coef;

typedef enum tag_vdp_ifirmode {
    VDP_IFIRMODE_DISEN = 0,
    VDP_IFIRMODE_COPY,
    VDP_IFIRMODE_DOUBLE,
    VDP_IFIRMODE_FILT,
    VDP_IFIRMODE_BUTT
} vdp_ifirmode;

typedef enum tag_vdp_hsclmode {
    VDP_HSCLMODE_DISEN = 0,
    VDP_HSCLMODE_COPY,
    VDP_HSCLMODE_DOUBLE,
    VDP_HSCLMODE_8TAPFIR,
    VDP_SHCLMODE_BUTT
} vdp_hsclmode;

/* **test cbb code***** */
typedef enum tag_vdp_rm_coef_mode {
    VDP_RM_COEF_MODE_TYP = 0x0,
    VDP_RM_COEF_MODE_RAN = 0x1,
    VDP_RM_COEF_MODE_MIN = 0x2,
    VDP_RM_COEF_MODE_MAX = 0x3,
    VDP_RM_COEF_MODE_ZRO = 0x4,
    VDP_RM_COEF_MODE_CUS = 0x5,
    VDP_RM_COEF_MODE_UP = 0x6,
    VDP_RM_COEF_MODE_LBL_TYP = 0x7,
    VDP_RM_COEF_MODE_LBL_RAN = 0x8,
    VDP_RM_COEF_MODE_LBL_MIN = 0x9,
    VDP_RM_COEF_MODE_LBL_MAX = 0x10,
    VDP_RM_COEF_MODE_BUTT
} vdp_rm_coef_mode;

typedef enum tag_vdp_intf_split_mode {
    VDP_SPLIT_MODE_4P_1SPLIT = 0,
    VDP_SPLIT_MODE_4P_4SPLIT = 1,
    VDP_SPLIT_MODE_2P_1SPLIT = 2,
    VDP_SPLIT_MODE_2P_2SPLIT = 3,
    VDP_SPLIT_MODE_2P_ODDEVEN = 4,
    VDP_SPLIT_MODE_1P_1SPLIT = 5,
    VDP_SPLIT_MODE_2P_BYPASS = 6,
    VDP_SPLIT_MODE_4P_2SPLIT = 7,
    VDP_SPLIT_MODE_8P_1SPLIT = 8,
    VDP_SPLIT_MODE_8P_2SPLIT = 9,
    VDP_SPLIT_MODE_8P_4SPLIT = 10,
    VDP_SPLIT_MODE_8P_8SPLIT = 11,
    VDP_INTF_SPLIT_MODE_BUTT
} vdp_intf_split_mode;

typedef enum {
    VDP_DISPCHN_CHN_DHD0 = 0,
    VDP_DISPCHN_CHN_DHD1 = 1,
    VDP_DISPCHN_CHN_DSD0 = 2,
    VDP_DISPCHN_CHN_WBC0 = 3,
    VDP_DISPCHN_CHN_WBC1 = 4,
    VDP_DISPCHN_CHN_WBC2 = 5,
    VDP_DISPCHN_CHN_WBC3 = 6,
    VDP_DISPCHN_CHN_VDPB = 7,
    VDP_DISPCHN_CHN_NONE = 8,
    VDP_DISPCHN_CHN_BUTT
} vdp_dispchn_chn;

typedef enum {
    VDP_DISPCHN_CBM_MIXV0 = 0,
    VDP_DISPCHN_CBM_MIXV1 = 1,
    VDP_DISPCHN_CBM_MIXG0 = 2,
    VDP_DISPCHN_CBM_MIXG1 = 3,
    VDP_DISPCHN_CBM_MIX0 = 4,
    VDP_DISPCHN_CBM_MIX1 = 5,
    VDP_DISPCHN_CBM_MIX2 = 6,
    VDP_DISPCHN_CBM_MIX_BUTT
} vdp_dispchn_cbm_mix;

typedef enum {
    VDP_VMX_PPC_MODE_8PPC = 0,
    VDP_VMX_PPC_MODE_2PPC = 1,
    VDP_VMX_PPC_MODE_BUTT
} xdp_vmx_ppc_mode;

typedef enum {
    VDP_HDR_OUTMAP_LINK_V0_STB = 0,
    VDP_HDR_OUTMAP_LINK_V0_DPT = 1,
    VDP_HDR_OUTMAP_LINK_GP0_STB = 2,
    VDP_HDR_OUTMAP_LINK_DHD0_STB = 3,
    VDP_HDR_OUTMAP_LINK_VP0_STB = 4,
    VDP_HDR_OUTMAP_LINK_BUTT
} xdp_hdr_outmap_link;

typedef enum {
    VDP_DCI_LINK_BEFORE_SR = 0,
    VDP_DCI_LINK_AFTER_SR = 1,
    VDP_DCI_LINK_BUTT
} xdp_dci_link;

typedef enum {
    VDP_HDR_LINK_DPT_DBHDR = 0,
    VDP_HDR_LINK_DPT_HIHDR = 1,
    VDP_HDR_LINK_STB_HDR = 2,
    VDP_HDR_LINK_BUTT
} xdp_hdr_link;

typedef enum {
    VDP_8KZME_LINK_BEFORE_SR = 0,
    VDP_8KZME_LINK_AFTER_SR = 1,
    VDP_8KZME_LINK_BUTT
} xdp_8kzme_link;

typedef enum {
    VDP_8KSR_LINK_V0 = 0,
    VDP_8KSR_LINK_DHD0 = 1,
    VDP_8KSR_LINK_GP0 = 2,
    VDP_8KSR_LINK_BUTT
} xdp_8ksr_link;

typedef enum {
    VDP_ACM_LINK_V0 = 0,
    VDP_ACM_LINK_VP = 1,
    VDP_ACM_LINK_BUTT
} xdp_acm_link;

typedef struct {
    hi_u32 y;
    hi_u32 u;
    hi_u32 v;
    hi_u32 a;
    hi_u32 type;
} vdp_bkg;

typedef struct {
    hi_u32 fd_inv;
    hi_u32 vs_inv;
    hi_u32 hs_inv;
    hi_u32 dv_inv;
} vdp_disp_syncinv;

typedef struct {
    hi_u32 clip_en;
    hi_u32 clip_low_y;
    hi_u32 clip_low_cb;
    hi_u32 clip_low_cr;

    hi_u32 clip_high_y;
    hi_u32 clip_high_cb;
    hi_u32 clip_high_cr;
} vdp_disp_clip;

typedef struct {
    hi_s32 csc_coef00;
    hi_s32 csc_coef01;
    hi_s32 csc_coef02;

    hi_s32 csc_coef10;
    hi_s32 csc_coef11;
    hi_s32 csc_coef12;

    hi_s32 csc_coef20;
    hi_s32 csc_coef21;
    hi_s32 csc_coef22;

    hi_s32 csc_in_dc0;
    hi_s32 csc_in_dc1;
    hi_s32 csc_in_dc2;

    hi_s32 csc_out_dc0;
    hi_s32 csc_out_dc1;
    hi_s32 csc_out_dc2;
} vdp_csc_cfg;

typedef enum {
    VDP_LAYER_VID1 = 0,
    VDP_LAYER_VID2 = 1,
    VDP_LAYER_VID3 = 2,
    VDP_LAYER_VID0 = 3,
    VDP_LAYER_DC = 4,
    VDP_LAYER_VID_BUTT
} vdp_layer_vid;

typedef enum {
    VDP_DISP_MODE_2D = 0,
    VDP_DISP_MODE_SBS = 1,
    VDP_DISP_MODE_LBL = 2,
    VDP_DISP_MODE_FS = 3,
    VDP_DISP_MODE_TAB = 4,
    VDP_DISP_MODE_FP = 5,
    VDP_DISP_MODE_BUTT
} vdp_disp_mode;

#endif
