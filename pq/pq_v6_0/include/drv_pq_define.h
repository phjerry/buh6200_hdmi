/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: common info define
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef _DRV_PQ_DEFINE_V6_H_
#define _DRV_PQ_DEFINE_V6_H_

#include "hi_type.h"

#ifdef HI_PQ_PROC_SUPPORT
#define PQ_PROC_CTRL_SUPPORT
#endif

/*****************************************************************************
PQ head file define
*****************************************************************************/
#ifdef PQ_96CV300_CS_SUPPORT
#define PQ_2DSCALER_SUPPORT
#endif

#define DCI_WGT_COEF_NUM 33
#define DCI_BS_LUT       320

#define CSC_GAMM_COEF_NUM 1024

#define SNR_EDGE_STR    3
#define SNR_MEAN_MAPP_X 6
#define SNR_MEAN_MAPP_Y 5
#define SNR_MEAN_MAPP_K 4
#define SNR_EDGE_MAPP_X 8
#define SNR_EDGE_MAPP_Y 7
#define SNR_EDGE_MAPP_K 6

#define HDR_TM_LUT      512
#define HDR_TMAP        8
#define HDR_TMAP_LUT    64
#define HDR_SMAP        8
#define HDR_SMAP_LUT    64
#define GFXHDR_TMAP     4
#define GFXHDR_TMAP_LUT 32

#define PQ_HDR_ACM_REGCFG_SIZE 40

#define DCI_HISTGRAM0_SIZE 32
#define DCI_HISTGRAM1_SIZE 256

#define PQ_CSC_VIDEO_DEFAULT   128
#define PQ_CSC_VIDEO_MAX       255
#define PQ_UNF_PARA_MAX       1023
#define PQ_UNF_PARA_DEFAULT    512
#define PQ_CSC_PICTURE_MAX 100
#define PQ_CSC_PICTURE_DEFAULT 50
#define PQ_ALG_DEFAULT_VALUE   50
#define PQ_ALG_MAX_VALUE       100

#define PQ_WIDTH_32 32
#define PQ_HEIGHT_32 32

#define PQ_WIDTH_64 64
#define PQ_HEIGHT_64 64
#define PQ_WIDTH_256 256
#define PQ_HEIGHT_256  256

#define PQ_WIDTH_1280 1280
#define PQ_HEIGHT_720 720

#define PQ_WIDTH_720  720
#define PQ_HEIGHT_576 576

#define PQ_WIDTH_1920  1920
#define PQ_HEIGHT_1080 1080

#define PQ_WIDTH_4096  4096
#define PQ_WIDTH_3840  3840
#define PQ_HEIGHT_2160 2160

#define PQ_WIDTH_8192  8192
#define PQ_WIDTH_7680  7680
#define PQ_HEIGHT_4320 4320
#define PQ_HEIGHT_4352 4352

#define REFRESHRATE_120        12000
#define REFRESHRATE_60          6000

#define WIN_SD_W 960

#define PQ_NUM_4 4
#define PQ_NUM_3 3
/*****************************************************************************
PQ software define
*****************************************************************************/
#define PQ_VERSION   "1.0.2.0"
#define PQ_DEF_NAME  "pqparam"
#define PQ_MAGIC_NUM "HIPQ"

#define PQ_CHIP_NAME "3796CV300"
#define PQ_DCI_SUPPORT
#define PQ_DCI_45_SUPPORT
#define GFXZME_REDUCE_SUPPORT
#define SHARPEN_FHD_SUPPORT
#define PQ_HDR_TM_64_SUPPORT
#define PQ_HDR_SM_64_SUPPORT
#define PQ_HDR_GFX_TM_SUPPORT

#define PQ_HDR_ACM_SUPPORT
#define PQ_HDR_ACC_27_SUPPORT

#ifdef HI_PQ_TEE_SUPPORT
#define TNR_TEE_SUPPORT
#else
#define TNR_NOTEE_SUPPORT
#endif

/*****************************************************************************
PQ proc enable define
*****************************************************************************/
#ifdef HI_PQ_PROC_SUPPORT
#define PQ_PROC_CTRL_SUPPORT
#endif

#define PQ_PROC_CMD_NAMELENGTH 64

typedef enum {
    PQ_PROC_PRINT_LEVEL_ORI = 0x0,
    PQ_PROC_PRINT_LEVEL1 = 0x1,
    PQ_PROC_PRINT_LEVEL2 = 0x2,
    PQ_PROC_PRINT_LEVEL3 = 0x3,

    PPQ_ROC_PRINT_MAX
} pq_proc_print_level;

/*****************************************************************************
PQ head file define
*****************************************************************************/
#define STR_LEN_8    8
#define STR_LEN_32   32
#define STR_LEN_80   80
#define STR_LEN_256  256
#define STR_LEN_1024 1024

#define ACM_Y_NUM 9
#define ACM_S_NUM 13
#define ACM_H_NUM 29

#define PHY_REG_MAX           16000
#define SOFT_REG_MAX          512
#define PQ_ALG_OFFSET_NUM_MAX 32

#define PQ_HDR_TM_512_CLUT_SIZE 512

#define PQ_HDR_TM_64_CLUT_SIZE 64
#define PQ_HDR_TM_8_STEP_SIZE  8
#define PQ_HDR_SM_64_CLUT_SIZE 64
#define PQ_HDR_SM_8_STEP_SIZE  8

#define PQ_GFXHDR_TM_CLUT_SIZE 32
#define PQ_HDR_ACM_REGCFG_SIZE 40

#ifdef PQ_DCI_45_SUPPORT
#define DCI_TABLE_LEVEL_MAX  5
#define DCI_TABLE_LEVEL_PARA 9
#else
#define DCI_TABLE_LEVEL_MAX  3
#define DCI_TABLE_LEVEL_PARA 9
#endif

#define DCI_320_CLUT_SIZE 320

#define ACM_I_NUM  4
#define ACM_J_NUM  10
#define ACM_EN_NUM 4

/*****************************************************************************
PQ tools virtual command
*****************************************************************************/

#define PQ_CMD_VIRTUAL_BIN_EXPORT     0xffffff00
#define PQ_CMD_VIRTUAL_BIN_IMPORT     0xffffff04
#define PQ_CMD_VIRTUAL_BIN_FIXED      0xffffff08
#define PQ_CMD_VIRTUAL_DRIVER_VERSION 0xffffffff

#define PQ_CMD_VIRTUAL_HD_CTRL       0xffff0001
#define PQ_CMD_VIRTUAL_SD_CTRL       0xffff0002
#define PQ_CMD_VIRTUAL_DEMO_CTRL     0xffff0003
#define PQ_CMD_VIRTUAL_SOURCE_SELECT 0xffff0004
#define PQ_CMD_VIRTUAL_READ_REGTYPE  0xffff0005
#define PQ_CMD_VIRTUAL_OUT_MODE      0xffff0006

#define PQ_CMD_VIRTUAL_ACM_RECMD_LUT   0xffff0100
#define PQ_CMD_VIRTUAL_ACM_BLUE_LUT    0xffff0101
#define PQ_CMD_VIRTUAL_ACM_GREEN_LUT   0xffff0102
#define PQ_CMD_VIRTUAL_ACM_BG_LUT      0xffff0103
#define PQ_CMD_VIRTUAL_ACM_FLESH_LUT   0xffff0104
#define PQ_CMD_VIRTUAL_ACM_6BCOLOR_LUT 0xffff0105

#define PQ_CMD_VIRTUAL_DCI_LUT    0xffff0200
#define PQ_CMD_VIRTUAL_DCI_BS_LUT 0xffff0201

#define PQ_CMD_VIRTUAL_GAMM_PARA       0xffff0301
#define PQ_CMD_VIRTUAL_DEGAMM_PARA     0xffff0302
#define PQ_CMD_VIRTUAL_GFX_GAMM_PARA   0xffff0303
#define PQ_CMD_VIRTUAL_GFX_DEGAMM_PARA 0xffff0304

#define PQ_CMD_VIRTUAL_HDR_TM_LUT    0xffff0401
#define PQ_CMD_VIRTUAL_HDR_PARA_MODE 0xffff0402
#define PQ_CMD_VIRTUAL_HDR_SMAP      0xffff0407
#define PQ_CMD_VIRTUAL_HDR_TMAP      0xffff0408

#define PQ_CMD_VIRTUAL_GFXHDR_MODE 0xffff0600
#define PQ_CMD_VIRTUAL_GFXHDR_STEP 0xffff0601
#define PQ_CMD_VIRTUAL_GFXHDR_TMAP 0xffff0602

#define PQ_CMD_VIRTUAL_HDR_ACM_MODE   0xffff0700
#define PQ_CMD_VIRTUAL_HDR_ACM_REGCFG 0xffff0701
#define PQ_CMD_VIRTUAL_HDR_DYN_MODE   0xffff0702

#define PQ_CMD_VIRTUAL_LCACM_LUT      0xffff0800
#define PQ_CMD_VIRTUAL_SNR_MOTION_LUT 0xffff0802
#define PQ_CMD_VIRTUAL_TNRNOISE_LUT   0xffff0803

#define PQ_CMD_VIRTUAL_TNR_Y_PIXMEAN_2_RATIO    0xffff0804
#define PQ_CMD_VIRTUAL_TNR_C_PIXMEAN_2_RATIO    0xffff0805
#define PQ_CMD_VIRTUAL_TNR_Y_MOTION_MAPPING     0xffff0806
#define PQ_CMD_VIRTUAL_TNR_C_MOTION_MAPPING     0xffff0807
#define PQ_CMD_VIRTUAL_TNR_FINAL_MOTION_MAPPING 0xffff0808
#define PQ_CMD_VIRTUAL_ZME_VID_SEL              0xffff0809
#define PQ_CMD_VIRTUAL_ZME_WBC_SEL              0xffff0810
#define PQ_CMD_VIRTUAL_ZME_GFX_SEL              0xffff0811

#define PQ_CMD_VIRTUAL_HD_BCD_CTRL   0xffff0900 /* HD u16_bright, u16contrast */
#define PQ_CMD_VIRTUAL_HD_COLOR_CTRL 0xffff0904 /* HD u16hue, u16saturation, colortemp */

#define PQ_CMD_VIRTUAL_SD_BCD_CTRL   0xffff0920 /* SD u16_bright, u16contrast */
#define PQ_CMD_VIRTUAL_SD_COLOR_CTRL 0xffff0924 /* SD u16hue, u16saturation, colortemp */

#define PQ_CMD_VIRTUAL_DCI_STRENGHT_CTRL 0xffff0940 /* DCI */
#define PQ_CMD_VIRTUAL_SHARP_CTRL        0xffff0944 /* sharp */
#define PQ_CMD_VIRTUAL_NOISE_CTRL        0xffff0948 /* tnr, snr, db, dm */

/*****************************************************************************
PQ bin define
*****************************************************************************/
/* PQ module类型 */
typedef enum {
    PQ_ALG_MODULE_DCI = 0,
    PQ_ALG_MODULE_ACM,

    PQ_ALG_MODULE_MAX,
} pq_alg_module;

typedef struct {
    /* 参数文件的校验和，file header（除此变量外）和data的所有逐字节校验和,
   用于检验参数正确性和判断是否dirty */
    hi_u32 file_check_sum;
    hi_u32 param_size;           /* 参数文件大小，包括file header和data */
    hi_char version[STR_LEN_32]; /* 版本号，字符串表示 */
    hi_char author[STR_LEN_32];  /* 参数调试者签名，字符串表示 */
    hi_char desc[STR_LEN_1024];  /* 版本描述，字符串表示 */
    /* 参数文件生成（烧写）时间，[0] ~ [5]：yy mm dd hh mm ss，[6]~[7]保留,
    该时间由PQ工具从PC上自动获取，无需用户输入 */
    hi_char time[STR_LEN_32];
    hi_char chip_name[STR_LEN_32];        /* 参数文件生成时的chip_name */
    hi_char sdk_version[STR_LEN_80];      /* 参数文件生成时的SDK版本 */
    hi_u32 phy_reg_num;                   /* 参数文件保存的物理寄存器个数 */
    hi_u32 soft_reg_num;                  /* 参数文件保存的软算法变量个数 */
    hi_u32 offset[PQ_ALG_OFFSET_NUM_MAX]; /* 算法系数偏移地址 */
    hi_u32 phy_offset;                    /* 算法物理寄存器偏移地址 */
} pq_file_header;

/* DCI coef */
typedef struct {
    hi_s32 as32_dci_lut[DCI_TABLE_LEVEL_MAX][DCI_TABLE_LEVEL_PARA];
} pq_dci_lut;

/* DCI black stretch lut */
typedef struct {
    hi_s16 bs_delta[DCI_320_CLUT_SIZE];
} pq_dci_bslut;

typedef struct {
    pq_dci_lut pq_dci_lut;
    pq_dci_bslut pq_dci_bs_lut;
} pq_dci_coef;

/* ACM coef */
typedef struct {
    hi_s16 as16_y[ACM_Y_NUM][ACM_S_NUM][ACM_H_NUM];
    hi_s16 as16_h[ACM_Y_NUM][ACM_S_NUM][ACM_H_NUM];
    hi_s16 as16_[ACM_Y_NUM][ACM_S_NUM][ACM_H_NUM];
} pq_acm_lut;

typedef struct {
    pq_acm_lut mode_recmd;
    pq_acm_lut mode_blue;
    pq_acm_lut mode_green;
    pq_acm_lut mode_bg;
    pq_acm_lut mode_fleshtone;
    pq_acm_lut mode6_b_color;
} pq_acm_mode;

typedef struct {
    hi_u32 protectmode;
    hi_s32 as32_acm_offset[ACM_I_NUM][ACM_J_NUM];
} hi_pq_acm_offset;

typedef struct {
#if defined(PQ_LCACM_SUPPORT)
    hi_pq_acm_offset acm_offset;
#else
    pq_acm_mode mode_lut;
#endif
} pq_acm_coef;

/* hdr dci_ alg */
typedef struct {
    /* 虚拟寄存器, PQ tool xml 调试 */
    hi_u32 max_lumi_display;  /* [0,1000]~[0,10000nits] */

    hi_u32 dark_detail_gain;    /* [0,4096]~[0,4.0] */
    hi_u32 dark_detail_gmm;     /* [0,4096]~[0,4.0] */
    hi_u32 bright_detail_gain;  /* [0,4096]~[0,4.0] */
    hi_u32 bright_detail_gmm;   /* [0,4096]~[0,4.0] */
    hi_u32 dyn_tm_gain;         /* [0,4096]~[0,4.0] */
    hi_u32 dyn_tm_gmm;          /* [0,4096]~[0,4.0] */

    hi_u32 dark_scene_dyn_tm;  /* [0,1024]~[0,1.0] */
    hi_u32 dark_limit;         /* [0,65536] */
    hi_u32 bleaching_level;    /* [0,100] */

    hi_u32 temporal_smooth;  /* [0,1024]~[0,1.0] default:1024; */
} hi_pq_hdr_dyn_tm_tuning;

/* hdr 调优支持场景类型 */
typedef enum {
    PQ_TOOL_HDR2SDR = 0,
    PQ_TOOL_SDR2HDR,

    PQ_TOOL_HDR_MAX,
} pq_tool_hdr;

#define PQ_HDR_ALG_TM_LUT_SIZE 512

typedef enum hiPQ_HDR_ALG_TM_E {
    PQ_HDR_ALG_TM_TMLUTI = 0,
    PQ_HDR_ALG_TM_TMLUTS,
    PQ_HDR_ALG_TM_SMLUTI,
    PQ_HDR_ALG_TM_SMLUTS,
    PQ_HDR_ALG_TM_MAX
} pq_hdr_alg_tm_mode;

typedef struct {
    hi_s16 as16_hdr2_sdrtm_lut[PQ_HDR_ALG_TM_MAX][PQ_HDR_ALG_TM_LUT_SIZE];
    hi_s16 as16_hdr2_hlgtm_lut[PQ_HDR_ALG_TM_MAX][PQ_HDR_ALG_TM_LUT_SIZE];
    hi_s16 as16_hlg2_sdrtm_lut[PQ_HDR_ALG_TM_MAX][PQ_HDR_ALG_TM_LUT_SIZE];
    hi_s16 as16_hlg2_hdrtm_lut[PQ_HDR_ALG_TM_MAX][PQ_HDR_ALG_TM_LUT_SIZE];
    hi_s16 as16_sdr2_hdrtm_lut[PQ_HDR_ALG_TM_MAX][PQ_HDR_ALG_TM_LUT_SIZE];
    hi_s16 sdr2hlg_tm_lut[PQ_HDR_ALG_TM_MAX][PQ_HDR_ALG_TM_LUT_SIZE];
} pq_hdr_alg_tm;

#if defined(PQ_HDR_TM_512_SUPPORT)
typedef struct {
    /* tone_mapping */
    pq_hdr_alg_tm hdr_tm_lut;
} pq_hdr_tm_coef;
#elif defined(PQ_HDR_TM_64_SUPPORT)
typedef struct {
    hi_u32 au32_step[PQ_TOOL_HDR_MAX][PQ_HDR_TM_8_STEP_SIZE];
    hi_u32 au32_num[PQ_TOOL_HDR_MAX][PQ_HDR_TM_8_STEP_SIZE];
    hi_u32 au32_pos[PQ_TOOL_HDR_MAX][PQ_HDR_TM_8_STEP_SIZE];
    hi_u32 clut[PQ_TOOL_HDR_MAX][PQ_HDR_TM_64_CLUT_SIZE];
} pq_hdr_tm_coef;
#endif

#if defined(PQ_HDR_SM_64_SUPPORT)
typedef struct {
    hi_u32 au32_step[PQ_TOOL_HDR_MAX][PQ_HDR_SM_8_STEP_SIZE];
    hi_u32 au32_num[PQ_TOOL_HDR_MAX][PQ_HDR_SM_8_STEP_SIZE];
    hi_u32 au32_pos[PQ_TOOL_HDR_MAX][PQ_HDR_SM_8_STEP_SIZE];
    hi_u32 clut[PQ_TOOL_HDR_MAX][PQ_HDR_SM_64_CLUT_SIZE];
} pq_hdr_sm_coef;
#endif

#if defined(PQ_HDR_GFX_TM_SUPPORT)
typedef struct {
    hi_u32 au32_tm_sdr2_hdr_clut[PQ_GFXHDR_TM_CLUT_SIZE];
    hi_u32 au32_tm_sdr2_hlg_clut[PQ_GFXHDR_TM_CLUT_SIZE];
} pq_gfxhdr_tm_coef;
#endif

#if defined(PQ_HDR_ACM_SUPPORT)
typedef struct {
    hi_s32 reg_cfg[PQ_TOOL_HDR_MAX][PQ_HDR_ACM_REGCFG_SIZE];
} pq_bin_hdr_acm_regcfg;
#endif

#if defined(PQ_HDR_DCI_27_SUPPORT)
typedef struct {
    hi_pq_hdr_dyn_tm_tuning ast_hdr_dyn_tuning;
} pq_bin_hdr_dyn_tuning;
#endif

/* PQ phy register */
typedef struct {
    hi_u64 reg_addr;    /* register addr */
    hi_u8 lsb;          /* register lsb */
    hi_u8 msb;          /* register msb */
    hi_u8 source_mode;  /* video source */
    hi_u8 output_mode;  /* output mode */
    hi_u32 module;      /* module */
    hi_u32 value;       /* register value */
} pq_bin_phy_reg;


typedef struct {
    hi_u64 key;
    hi_u32 value;
} pq_mixed_key_reg;

/* pq_param */
typedef struct {
#if defined(PQ_DCI_SUPPORT)
    pq_dci_coef dci_coef;
#endif
    pq_acm_coef acm_coef;
    pq_hdr_tm_coef hdr_tm_coef;
#if defined(PQ_HDR_SM_64_SUPPORT)
    pq_hdr_sm_coef hdr_sm_coef;
#endif
#if defined(PQ_HDR_GFX_TM_SUPPORT)
    pq_gfxhdr_tm_coef gfx_hdr_tm_coef;
#endif
#if defined(PQ_HDR_ACM_SUPPORT)
    pq_bin_hdr_acm_regcfg bin_hdr_acm_reg_cfg;
#endif
#if defined(PQ_HDR_DCI_27_SUPPORT)
    pq_bin_hdr_dyn_tuning hdr_dyn_cfg;
#endif
} pq_coef;

typedef struct {
    pq_file_header pq_file_header;
    pq_coef coef;
    pq_bin_phy_reg phy_reg[PHY_REG_MAX];
    pq_bin_phy_reg soft_reg[SOFT_REG_MAX];
} pq_bin_param;

#endif




