/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: gfx2d hal hwc
 * Author: sdk
 * Create: 2019-05-13
 */

#include "drv_gfx2d_struct.h"
#include "hi_gfx_comm_k.h"
#include "drv_gfx2d_hal.h"
#include "hi_reg_common.h"
#include "drv_gfx2d_errcode.h"
#include "drv_gfx2d_filter_para.h"
#include "drv_gfx2d_hal_hwc_adp.h"
#include "drv_gfx2d_mem.h"
#include "drv_gfx2d_config.h"
#include "hi_gfx_sys_k.h"
#include "drv_gfx2d_debug.h"
#include <linux/hisilicon/securec.h>
#include "tde_reg.h"
#include "drv_gfx2d_hal_features.h"

/* **************************** Macro Definition ************************************************* */
#define HWC_REG_BASE_ADDR 0x01400000
#define HWC_ISRNUM (116 + 32)
#define TDE_MMU_REG_BASEADDR ((HWC_REG_BASE_ADDR) + 0xF000)

#define GFX2D_HAL_MAX_NODE 7
#define GFX2D_CONFIG_MAX_ZMERATION 256

/* Obtain the symbol bits and the low 9 bits of the filter coefficients to form 10 bits */
#define HWC_GET_BITS_OF_FILTERPARA(para) ((((para)&0x80000000) >> 22) | ((para)&0x1ff))

#define GFX2D_HAL_HWC_HSTEP_FLOATLEN 20
#define GFX2D_HAL_HWC_NOSCALE_HSTEP (0x1 << GFX2D_HAL_HWC_HSTEP_FLOATLEN)

#define GFX2D_HAL_HWC_VSTEP_FLOATLEN 12
#define GFX2D_HAL_HWC_NOSCALE_VSTEP (0x1 << GFX2D_HAL_HWC_VSTEP_FLOATLEN)

#define GFX2D_CONFIG_NODE_SIZE ((sizeof(hwc_node) + 0xf) & 0xfffffff0)
#define GFX2D_ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0])) /* index is 0 */

#define GFX2D_HAL_SRCFMT_PKYUYV 32
#define GFX2D_HAL_SRCFMT_SP400 48
#define GFX2D_HAL_SRCFMT_SP422V 50
#define GFX2D_HAL_SRCFMT_SP444 52
#define GFX2D_HAL_RATIO_PRECISE 20

#define GFX2D_HAL_SLICE_WIDTH 512

#ifdef CONFIG_GFX_MMU_CLOCK

#endif
/* ************************** Structure Definition *********************************************** */
typedef union {
    struct {
        hi_u32 bit16X : 16;
        hi_u32 bit16Y : 16;
    } stBits;
    hi_u32 u32All;
} HWC_POS_U;

typedef union {
    struct {
        hi_u32 bit16Width : 16;
        hi_u32 bit16Height : 16;
    } stBits;
    hi_u32 u32All;
} HWC_RES_U;

typedef union {
    struct {
        hi_u32 bit9Idc0 : 9;
        hi_u32 bit9Idc1 : 9;
        hi_u32 bit9Idc2 : 9;
        hi_u32 bit1IsEnable : 1;
        hi_u32 bit1Reserved : 4;
    } stBits;
    hi_u32 u32All;
} HWC_VHDCSCIDC_U;

typedef union {
    struct {
        hi_u32 bit9Odc0 : 9;
        hi_u32 bit9Odc1 : 9;
        hi_u32 bit9Odc2 : 9;
        hi_u32 bit5Reserved : 5;
    } stBits;
    hi_u32 u32All;
} HWC_VHDCSCODC_U;

typedef union {
    struct {
        hi_u32 bit15Param0 : 15;
        hi_u32 bit1Reserved1 : 1;
        hi_u32 bit15Param1 : 15;
        hi_u32 bit1Reserved2 : 1;
    } stBits;
    hi_u32 u32All;
} HWC_VHDCSCPARAM_U;

typedef union {
    struct {
        hi_u32 bit24Ratio : 24;
        hi_u32 bit1Order : 1;
        hi_u32 bit1ChrmoaMode : 1;
        hi_u32 bit1LumMode : 1;
        hi_u32 bit1AlphaMode : 1;
        hi_u32 bit1IsChromaMid : 1;
        hi_u32 bit1IsLumMid : 1;
        hi_u32 bit1IsChromaEnable : 1;
        hi_u32 bit1IsLumEnable : 1;
    } stBits;
    hi_u32 u32All;
} HWC_VHDHSP_U;

typedef union {
    struct {
        hi_u32 bit28Offset : 28;
        hi_u32 bit4Reserved : 4;
    } stBits;
    hi_u32 u32All;
} HWC_VHDHOFFSET_U;

typedef union {
    struct {
        hi_u32 bit19Reserved : 19;
        hi_u32 bit2InFmt : 2;
        hi_u32 bit2OutFmt : 2;
        hi_u32 bit1ChromaMode : 1;
        hi_u32 bit1LumMode : 1;
        hi_u32 bit1Reserved1 : 1;
        hi_u32 bit1ChromaTap : 1;
        hi_u32 bit1Reserved2 : 1;
        hi_u32 bit1IsChromaMid : 1;
        hi_u32 bit1IsLumMid : 1;
        hi_u32 bit1IsChromaEnable : 1;
        hi_u32 bit1IsLumEnable : 1;
    } stBits;
    hi_u32 u32All;
} HWC_VHDVSP_U;

typedef union {
    struct {
        hi_u32 bit3Prio0 : 3;
        hi_u32 bit3Prio1 : 3;
        hi_u32 bit3Prio2 : 3;
        hi_u32 bit3Prio3 : 3;
        hi_u32 bit3Prio4 : 3;
        hi_u32 bit3Prio5 : 3;
        hi_u32 bit3Prio6 : 3;
        hi_u32 bit4AluMode : 4;                 /* [24..21]  */
        hi_u32 bit1CbmMode : 1; /* [25]      */ /* index is 25 */
        hi_u32 bit5Reserved : 5;                /* [30..26]  */
        hi_u32 bit1En : 1; /* [31]      */      /* index is 31 */
    } stBits;
    hi_u32 u32All;
} HWC_CBMMIX_U;

typedef union {
    struct {
#ifndef CONFIG_GFX_HWC_VERSION_3_0
        hi_u32 bit9Idc0 : 9;
        hi_u32 bit9Idc1 : 9;
        hi_u32 bit9Idc2 : 9;
        hi_u32 bit1IsEnable : 1;
        hi_u32 bit4Reserved : 4;
#else
        hi_u32 bit11Idc0 : 11;
        hi_u32 bit5Reserved : 5;
        hi_u32 bit11Idc1 : 11;
        hi_u32 bit4Reserved : 4;
        hi_u32 bit1IsEnable : 1;
#endif
    } stBits;
    hi_u32 u32All;
} HWC_OUTCSCIDC0_U;

typedef union {
    struct {
        hi_u32 bit11Idc2 : 11;
        hi_u32 bit21Reserved : 21;
    } stBits;
    hi_u32 u32All;
} HWC_OUTCSCIDC1_U;

typedef union {
    struct {
#ifndef CONFIG_GFX_HWC_VERSION_3_0
        hi_u32 bit9Odc0 : 9;
        hi_u32 bit9Odc1 : 9;
        hi_u32 bit9Odc2 : 9;
        hi_u32 bit5Reserved : 5;
#else
        hi_u32 bit11Odc0 : 11;
        hi_u32 bit5Reserved : 5;
        hi_u32 bit11Odc1 : 11;
        hi_u32 bit5Reserved1 : 5;
#endif
    } stBits;
    hi_u32 u32All;
} HWC_OUTCSCODC0_U;

typedef union {
    struct {
        hi_u32 bit11Odc2 : 11;
        hi_u32 bit21Reserved : 21;
    } stBits;
    hi_u32 u32All;
} HWC_OUTCSCODC1_U;

typedef union {
    struct {
        hi_u32 bit15Param0 : 15;
        hi_u32 bit1Reserved1 : 1;
        hi_u32 bit15Param1 : 15;
        hi_u32 bit1Reserved2 : 1;
    } stBits;
    hi_u32 u32All;
} HWC_OUTCSCPARAM_U;

typedef union {
    struct {
        hi_u32 bit29Reserved : 29;
        hi_u32 bit1DitherRound : 1;
        hi_u32 bit1Reserved : 1;
        hi_u32 bit1DitherEn : 1;
    } stBits;
    hi_u32 u32All;
} HWC_OUT_DITHER_CTRL_U;

typedef union {
    struct {
        hi_u32 bit1Reserved : 1;
        hi_u32 bit2HfirMode : 2;
        hi_u32 bit1MidEn : 1;
        hi_u32 bit1CkGtEn : 1;
        hi_u32 bit27Reserved : 27;
    } bits;
    hi_u32 u32All;
} HWC_OUT_HFIR_CTRL_U;

typedef union {
    struct {
        hi_u32 bit10Coef0 : 10;
        hi_u32 bit6Reserved0 : 6;
        hi_u32 bit10Coef1 : 10;
        hi_u32 bit6Reserved1 : 6;
    } bits;
    hi_u32 u32All;

} HWC_OUT_HFIRCOEF01_U;

typedef union {
    struct {
        hi_u32 bit1CmpEnable : 1;
        hi_u32 bit1LossEnable : 1;
        hi_u32 bit1SgmtType : 1;
        hi_u32 bit29Reserved : 29;
    } stBits;
    hi_u32 u32All;
} HWC_CMPCTRL_U;

typedef union {
    struct {
        hi_u32 bit13XPos : 13;
        hi_u32 bit3Reserved1 : 3;
        hi_u32 bit13YPos : 13;
        hi_u32 bit2Reserved2 : 2;
        hi_u32 bit1IsEnable : 1;
    } stBits;
    hi_u32 u32All;
} HWC_CROPPOS_U;

typedef union {
    struct {
        hi_u32 bit13Width : 13;
        hi_u32 bit3Reserved1 : 3;
        hi_u32 bit13Height : 13;
        hi_u32 bit2Reserved2 : 3;
    } stBits;
    hi_u32 u32All;
} HWC_CROPRESO_U;

typedef union {
    struct {
        hi_u32 bit1IsLossLess : 1;
        hi_u32 bit1IsAlphaLossLess : 1;
        hi_u32 bit1Mode : 1;
        hi_u32 bit2OsdMode : 2;
        hi_u32 bit1IsPartition : 1;
        hi_u32 bit3PartitionNum : 3;
        hi_u32 bit23Reserved : 23;
    } stBits;
    hi_u32 u32All;
} HWC_CMPGLBINFO_U;

typedef union {
    struct {
        hi_u32 bit13Width : 13;
        hi_u32 bit3Reserved1 : 3;
        hi_u32 bit13Height : 13;
        hi_u32 bit2Reserved2 : 3;
    } stBits;
    hi_u32 u32All;
} HWC_CMPRESO_U;

typedef union {
    struct {
        hi_u32 bit10BudgetBitsMb : 10;
        hi_u32 bit6Reserved1 : 6;
        hi_u32 bit10MinMbBits : 10;
        hi_u32 bit2Reserved2 : 6;
    } stBits;
    hi_u32 u32All;
} HWC_CMPCFG0_U;

typedef union {
    struct {
        hi_u32 bit10BudgetBitsMbCap : 10;
        hi_u32 bit6Reserved : 22;
    } stBits;
    hi_u32 u32All;
} HWC_CMPCFG1_U;

typedef union {
    struct {
        hi_u32 bit4MaxQp : 4;
        hi_u32 bit4Reserved1 : 4;
        hi_u32 bit4SadBitsNgain : 4;
        hi_u32 bit4Reserved2 : 4;
        hi_u32 bit3RcSmthNgain : 3;
        hi_u32 bit5Reserved3 : 5;
        hi_u32 bit4SpecialBitGain : 4;
        hi_u32 bit4Reserved4 : 4;
    } stBits;
    hi_u32 u32All;
} HWC_CMPCFG2_U;

typedef union {
    struct {
        hi_u32 bit7MaxSadThr : 7;
        hi_u32 bit9Reserved1 : 9;
        hi_u32 bit7MinSadThr : 7;
        hi_u32 bit9Reserved2 : 9;
    } stBits;
    hi_u32 u32All;
} HWC_CMPCFG3_U;

typedef union {
    struct {
        hi_u32 bit7SmthThr : 7;
        hi_u32 bit1Reserved1 : 1;
        hi_u32 bit7StillThr : 7;
        hi_u32 bit1Reserved2 : 1;
        hi_u32 bit10BigGradThr : 10;
        hi_u32 bit6Reserved3 : 6;
    } stBits;
    hi_u32 u32All;
} HWC_CMPCFG4_U;

typedef union {
    struct {
        hi_u32 bit6SmthPixNumThr : 6;
        hi_u32 bit2Reserved1 : 2;
        hi_u32 bit6StillPixNumThr : 6;
        hi_u32 bit2Reserved2 : 2;
        hi_u32 bit6NoisePixNumThr : 6;
        hi_u32 bit2Reserved3 : 2;
        hi_u32 bit6LargeSmthPixNumThr : 6;
        hi_u32 bit2Reserved4 : 2;
    } stBits;
    hi_u32 u32All;
} HWC_CMPCFG5_U;

typedef union {
    struct {
        hi_u32 bit7NoiseSad : 7;
        hi_u32 bit9Reserved1 : 9;
        hi_u32 bit9PixDiffThr : 9;
        hi_u32 bit7Reserved2 : 7;
    } stBits;
    hi_u32 u32All;
} HWC_CMPCFG6_U;

typedef union {
    struct {
        hi_u32 bit7AdjSadBitsThr : 7;
        hi_u32 bit1Reserved1 : 1;
        hi_u32 bit8MaxTrowBits : 8;
        hi_u32 bit16Reserved2 : 16;
    } stBits;
    hi_u32 u32All;
} HWC_CMPCFG7_U;

typedef union {
    struct {
        hi_u32 bit8QpInc1BitsThr : 8;
        hi_u32 bit8QpDec1BitsThr : 8;
        hi_u32 bit8QpInc2BitsThr : 8;
        hi_u32 bit8QpDec2BitsThr : 8;
    } stBits;
    hi_u32 u32All;
} HWC_CMPCFG8_U;

typedef union {
    struct {
        hi_u32 bit10ForceQpThr : 10;
        hi_u32 bit6Reserved1 : 6;
        hi_u32 bit10ForceQpThrCap : 10;
        hi_u32 bit6Reserved2 : 6;
    } stBits;
    hi_u32 u32All;
} HWC_CMPCFG9_U;

typedef struct {
    hi_u32 u32AqListHeadAddr;
    hi_u32 u32HwcCtl;
    hi_u32 u32MaskInt;
    hi_u32 u32IntClr;
    hi_u32 u32Int;
    hi_u32 u32TimeCnt;
    hi_u32 u32MiscEllaneous;
} HWC_REG_S;

typedef struct {
    hi_u32 u32Status;
    hi_u32 u32Reserved[39]; /* index is 39 */
    hi_u32 u32WCnt;
} HWC_DEBUG_REG_S;

#ifdef GFX2D_ALPHADETECT_SUPPORT
typedef struct {
    hi_u32 u32AlphaSum[9]; /* index is 9 */
} HWC_ALPHASUM_REG_S;
#endif

typedef union {
    struct {
        hi_u32 bit6Fmt : 6;                        /* [5..0]    */
        hi_u32 bit5ArgbOrder : 5;                  /* [10..6]   */
        hi_u32 bit1UVOrder : 1; /* [11]      */    /* index is 11 */
        hi_u32 bit2ExpandMode : 2;                 /* [13..12]  */
        hi_u32 bit1ClutMode : 1; /* [14]      */   /* index is 14 */
        hi_u32 bit1RgbMode : 1; /* [15]      */    /* index is 15 */
        hi_u32 bit1_sp_bitw : 1;                   /* [16]  */
        hi_u32 bit1Reserved0 : 1;                  /* [17]  */
        hi_u32 bit1AlphaRange : 1; /* [18]      */ /* index is 18 */
        hi_u32 bit1VScanOrd : 1; /* [19]      */   /* index is 19 */
        hi_u32 bit1HScanOrd : 1; /* [20]      */   /* index is 20 */
        hi_u32 bit1422Pro : 1; /* [21]      */     /* index is 21 */
        hi_u32 bit7Reserved1 : 6;                  /* [27..22]  */
        hi_u32 bit1_afbc_en : 1;                   /* [28]  */
        hi_u32 bit1_dma : 1;                       /* [29]  */
        hi_u32 bit1IsDim : 1; /* [30]      */      /* index is 30 */
        hi_u32 bit1IsEnable : 1; /* [31]      */   /* index is 31 */
    } stBits;
    hi_u32 u32All;
} HWC_SRCCTL_U;

typedef union {
    struct {
        hi_u32 bit6Fmt : 6;                      /* [5..0]    */
        hi_u32 bit5ArgbOrder : 5;                /* [10..6]   */
        hi_u32 bit1UVOrder : 1; /* [11]      */  /* index is 11 */
        hi_u32 bit2ExpandMode : 2;               /* [13..12]  */
        hi_u32 bit1ClutMode : 1; /* [14]      */ /* index is 14 */
#ifndef CONFIG_GFX_HWC_VERSION_3_0
        hi_u32 bit4Reserved0 : 3; /* [17..15]  */
#else
        hi_u32 bit1RgbMode : 1; /* [15]      */ /* index is 15 */
        hi_u32 bit2Reserved0 : 2;               /* [17..16]  */
#endif
        hi_u32 bit1AlphaRange : 1; /* [18]      */ /* index is 18 */
        hi_u32 bit1VScanOrd : 1; /* [19]      */   /* index is 19 */
        hi_u32 bit1HScanOrd : 1; /* [20]      */   /* index is 20 */
        hi_u32 bit1422Pro : 1; /* [21]      */     /* index is 21 */
        hi_u32 bit7Reserved1 : 7;                  /* [28..22]  */
        hi_u32 reverse : 1; /* [29]      */        /* index is 29 */
        hi_u32 bit1IsDim : 1; /* [30]      */      /* index is 30 */
        hi_u32 bit1IsEnable : 1; /* [31]      */   /* index is 31 */
    } stBits;
    hi_u32 u32All;
} HWC_SRCCTL1_U;

typedef union {
    struct {
        hi_u32 bit8Alpha0 : 8;     /* [7..0]  */
        hi_u32 bit8Alpha1 : 8;     /* [15..8]  */
        hi_u32 bit16Reserved : 16; /* [31..16]  */
    } stBits;
    hi_u32 u32All;
} HWC_ALPHA_U;

typedef union {
    struct {
        hi_u32 bit16SrcCmdInterval : 16;            /* [15..0]  */
        hi_u32 bit15Reserved : 15;                  /* [30..16] */
        hi_u32 bit1SrcCmdCtrlEn : 1; /* [31]     */ /* index is 31 */
    } stBits;
    hi_u32 u32All;
} HWC_SRC_CMD_CTL_U;

typedef struct {
    HWC_SRCCTL_U unCtrl; /* 0x0 */
    hi_u32 phy_addr_h;
    hi_u32 phy_addr;
    hi_u32 ch1_addr_addr_h;
    hi_u32 ch1_addr_addr_l;
    hi_u32 stride;
    hi_u32 stride1;
    HWC_RES_U unRes;     /* 0x14 */
    hi_u32 u32DimValue;  /* 0x18 */
    HWC_ALPHA_U unAlpha; /* 0x1c */
    hi_u32 u32PixOffset; /* 0x20 */
} video_layer;

typedef struct {
    HWC_SRCCTL_U unCtrl; /* 0x0 */
    hi_u32 phy_addr_h;
    hi_u32 phy_addr;     /* 0x4 */
    hi_u32 stride;       /* 0xc */
    HWC_RES_U unRes;     /* 0x14 */
    hi_u32 u32DimValue;  /* 0x18 */
    HWC_ALPHA_U unAlpha; /* 0x1c */
    hi_u32 u32PixOffset; /* 0x20 */
} gfx_layer;

typedef union {
    struct {
        hi_u32 bit6Fmt : 6;
        hi_u32 bit5ArgbOrder : 5;
        hi_u32 bit1CbcrOrder : 1;
        hi_u32 bit5Reserved0 : 5;
        hi_u32 bit1Dither : 1;
        hi_u32 bit1AlphaRange : 1;
        hi_u32 bit1VScanOrder : 1;
        hi_u32 bit1HScanOrder : 1;
        hi_u32 bit2Reserved1 : 2;
        hi_u32 bit1CmpEn : 1;
        hi_u32 bit1CmpAddrChangeEn : 1;
        hi_u32 adapt_32b_en : 1;
        hi_u32 dst_ch2_mmu_bypass : 1;
        hi_u32 bit1ChormeMmuBypass : 1;
        hi_u32 bit1LumMmuBypass : 1;
        hi_u32 bit1ARMmuBypass : 1;
        hi_u32 bit1GBMmuBypass : 1;
        hi_u32 bit1Enable : 1;
    } stBits;
    hi_u32 u32All;
} HWC_OUTCTL_U;

/* Define the union U_DES_ALPHA */
typedef union {
    struct {
        hi_u32 bit8AlphaThreshold : 8;      /* [7..0]  */
        hi_u32 bit1CropMode : 1; /* [8]  */ /* index is 8 */
        hi_u32 bit1CropEn : 1; /* [9]  */   /* index is 9 */
        hi_u32 bit22Reserved : 22;          /* [31..10]  */
    } stBits;
    hi_u32 u32All;
} HWC_OUTALPHA_U;

typedef union {
    struct {
        hi_u32 bit16OutCmdInterval : 16;            /* [15..0]  */
        hi_u32 bit15Reserved : 15;                  /* [30..16] */
        hi_u32 bit1OutCmdCtrlEn : 1; /* [31]     */ /* index is 31 */
    } stBits;
    hi_u32 u32All;
} HWC_OUT_CMD_CTL_U;

typedef union {
    struct {
        hi_u32 bit1Src1Ch0MmuBypass : 1; /* [0]  */ /* index is 0 */
        hi_u32 bit1Src1Ch1MmuBypass : 1; /* [1]  */ /* index is 1 */
        hi_u32 bit1Src2Ch0MmuBypass : 1; /* [2]  */ /* index is 2 */
        hi_u32 bit1Src2Ch1MmuBypass : 1; /* [3]  */ /* index is 3 */
        hi_u32 bit1Src3MmuBypass : 1; /* [4]  */    /* index is 4 */
        hi_u32 bit1Src4MmuBypass : 1; /* [5]  */    /* index is 5 */
        hi_u32 bit1Src5MmuBypass : 1; /* [6]  */    /* index is 6 */
        hi_u32 bit1Src6MmuBypass : 1; /* [7]  */    /* index is 7 */
        hi_u32 bit1Src7MmuBypass : 1; /* [8]  */    /* index is 8 */
        hi_u32 bit1ClutMmuBypass : 1; /* [9]  */    /* index is 9 */
        hi_u32 bit6Reserved0 : 6;                   /* [15..10]  */
        hi_u32 bit1Src1Ch0Prot : 1; /* [16]  */     /* index is 16 */
        hi_u32 bit1Src1Ch1Prot : 1; /* [17]  */     /* index is 17 */
        hi_u32 bit1Src2Ch0Prot : 1; /* [18]  */     /* index is 18 */
        hi_u32 bit1Src2Ch1Prot : 1; /* [19]  */     /* index is 19 */
        hi_u32 bit1Src3Prot : 1; /* [20]  */        /* index is 20 */
        hi_u32 bit1Src4Prot : 1; /* [21]  */        /* index is 21 */
        hi_u32 bit1Src5Prot : 1; /* [22]  */        /* index is 22 */
        hi_u32 bit1Src6Prot : 1; /* [23]  */        /* index is 23 */
        hi_u32 bit1Src7Prot : 1; /* [24]  */        /* index is 24 */
        hi_u32 bit7Reserved1 : 7;                   /* [31..25]  */
    } stBits;
    hi_u32 u32All;
} HWC_MMU_PROT_CTL_U;

typedef union {
    struct {
        hi_u32 bit16_src2_hpzme_width : 16;        /* [15..0]  */
        hi_u32 bit4_src2_hpzme_mode : 4;           /* [19..16]  */
        hi_u32 bit10_Reserved0 : 11;               /* [30..20]  */
        hi_u32 bit1_src2_hpzme_en : 1; /* [31]  */ /* index is 0 */
    } stBits;
    hi_u32 u32All;
} HWC_HPZME_U;

typedef union {
    struct {
        hi_u32 bit1CscMode : 1; /* [0]  */      /* index is 0 */
        hi_u32 bit1OutPremultEn : 1; /* [1]  */ /* index is 1 */
        hi_u32 bit30Reserved : 30;              /* [31..2]  */
    } stBits;
    hi_u32 u32All;
} HWC_CSCMUX_U;

typedef union {
    struct {
        hi_u32 bit2HDswmMode : 2;                    /* [1..0]  */
        hi_u32 bit2Reserved0 : 2;                    /* [3..2]  */
        hi_u32 bit1VDswmMode : 1; /* [4]  */         /* index is 4 */
        hi_u32 bit1AlphaDectectClear : 1; /* [5]  */ /* index is 5 */
        hi_u32 bit1AlphaDectectEn : 1; /* [6]  */    /* index is 6 */
        hi_u32 bit25Reserved1 : 25;                  /* [31..7]  */
    } stBits;
    hi_u32 u32All;
} HWC_OUTDSWM_U;

typedef union {
    struct {
        hi_u32 bit1GlobalAlphaEnable : 1; /* [0]  */ /* index is 0 */
        hi_u32 bit1PixelAlphaEnable : 1; /* [1]  */  /* index is 1 */
        hi_u32 bit1bPreMul : 1; /* [2]  */           /* index is 2 */
        hi_u32 bit1MultiGEn : 1; /* [3]  */          /* index is 3 */
        hi_u32 bit4BlendMode : 4;                    /* [7..4]  */
        hi_u32 bit8GlobalAlpha : 8;                  /* [15..8]  */
        hi_u32 bit15Reserved : 15;                   /* [30..16]  */
        hi_u32 bit1bCovBlend : 1; /* [31]  */        /* index is 31 */
    } stBits;
    hi_u32 u32All;
} HWC_CBMPARA_U;

typedef struct {
    HWC_CBMPARA_U unCbmPara;
    HWC_POS_U unCbmStartPos;
} HWC_CBMINFO_S;

typedef struct {
    u_src1_window0_stpos window0_stpos;
    u_src1_window0_edpos window0_edpos;
    u_src1_window0_stpos window1_stpos;
    u_src1_window0_edpos window1_edpos;
    u_src1_window0_stpos window2_stpos;
    u_src1_window0_edpos window2_edpos;
    u_src1_window0_stpos window3_stpos;
    u_src1_window0_edpos window3_edpos;
} dim_window_s;

typedef struct {
    u_src1_window0_rgb window0_rgb;
    u_src1_window0_a window0_a;
    u_src1_window0_rgb window1_rgb;
    u_src1_window0_a window1_a;
    u_src1_window0_rgb window2_rgb;
    u_src1_window0_a window2_a;
    u_src1_window0_rgb window3_rgb;
    u_src1_window0_a window3_a;
} window_argb_s;

typedef struct {
    u_src1_hsp src1_hsp;           /* 0x1000 */
    u_src1_hloffset src1_hloffset; /* 0x1004 */
    u_src1_hcoffset src1_hcoffset; /* 0x1008 */
    unsigned int reserved_20[61];  /* 61: 0x100c~0x10fc */
    u_src1_vsp src1_vsp;           /* 0x1100 */
    u_src1_vsr src1_vsr;           /* 0x1104 */
    u_src1_voffset src1_voffset;   /* 0x1108 */
    u_src1_zmeoreso src1_zme_out_reso;
    u_src1_zmeireso src1_zme_in_reso;
    unsigned int reserved_21[59]; /* 59: 0x1114~0x11fc */
} src_zme_s;

typedef struct {
    u_src_hipp_csc_ctrl src_hipp_csc_ctrl;       /* 0x1900 */
    u_src_hipp_csc_coef00 src_hipp_csc_coef00;   /* 0x1904 */
    u_src_hipp_csc_coef01 src_hipp_csc_coef01;   /* 0x1908 */
    u_src_hipp_csc_coef02 src_hipp_csc_coef02;   /* 0x190c */
    u_src_hipp_csc_coef10 src_hipp_csc_coef10;   /* 0x1910 */
    u_src_hipp_csc_coef11 src_hipp_csc_coef11;   /* 0x1914 */
    u_src_hipp_csc_coef12 src_hipp_csc_coef12;   /* 0x1918 */
    u_src_hipp_csc_coef20 src_hipp_csc_coef20;   /* 0x191c */
    u_src_hipp_csc_coef21 src_hipp_csc_coef21;   /* 0x1920 */
    u_src_hipp_csc_coef22 src_hipp_csc_coef22;   /* 0x1924 */
    u_src_hipp_csc_scale src_hipp_csc_scale;     /* 0x1928 */
    u_src_hipp_csc_idc0 src_hipp_csc_idc0;       /* 0x192c */
    u_src_hipp_csc_idc1 src_hipp_csc_idc1;       /* 0x1930 */
    u_src_hipp_csc_idc2 src_hipp_csc_idc2;       /* 0x1934 */
    u_src_hipp_csc_odc0 src_hipp_csc_odc0;       /* 0x1938 */
    u_src_hipp_csc_odc1 src_hipp_csc_odc1;       /* 0x193c */
    u_src_hipp_csc_odc2 src_hipp_csc_odc2;       /* 0x1940 */
    u_src_hipp_csc_min_y src_hipp_csc_min_y;     /* 0x1944 */
    u_src_hipp_csc_min_c src_hipp_csc_min_c;     /* 0x1948 */
    u_src_hipp_csc_max_y src_hipp_csc_max_y;     /* 0x194c */
    u_src_hipp_csc_max_c src_hipp_csc_max_c;     /* 0x1950 */
    u_src_hipp_csc2_coef00 src_hipp_csc2_coef00; /* 0x1954 */
    u_src_hipp_csc2_coef01 src_hipp_csc2_coef01; /* 0x1958 */
    u_src_hipp_csc2_coef02 src_hipp_csc2_coef02; /* 0x195c */
    u_src_hipp_csc2_coef10 src_hipp_csc2_coef10; /* 0x1960 */
    u_src_hipp_csc2_coef11 src_hipp_csc2_coef11; /* 0x1964 */
    u_src_hipp_csc2_coef12 src_hipp_csc2_coef12; /* 0x1968 */
    u_src_hipp_csc2_coef20 src_hipp_csc2_coef20; /* 0x196c */
    u_src_hipp_csc2_coef21 src_hipp_csc2_coef21; /* 0x1970 */
    u_src_hipp_csc2_coef22 src_hipp_csc2_coef22; /* 0x1974 */
    u_src_hipp_csc2_scale src_hipp_csc2_scale;   /* 0x1978 */
    u_src_hipp_csc2_idc0 src_hipp_csc2_idc0;     /* 0x197c */
    u_src_hipp_csc2_idc1 src_hipp_csc2_idc1;     /* 0x1980 */
    u_src_hipp_csc2_idc2 src_hipp_csc2_idc2;     /* 0x1984 */
    u_src_hipp_csc2_odc0 src_hipp_csc2_odc0;     /* 0x1988 */
    u_src_hipp_csc2_odc1 src_hipp_csc2_odc1;     /* 0x198c */
    u_src_hipp_csc2_odc2 src_hipp_csc2_odc2;     /* 0x1990 */
    u_src_hipp_csc2_min_y src_hipp_csc2_min_y;   /* 0x1994 */
    u_src_hipp_csc2_min_c src_hipp_csc2_min_c;   /* 0x1998 */
    u_src_hipp_csc2_max_y src_hipp_csc2_max_y;   /* 0x199c */
    u_src_hipp_csc2_max_c src_hipp_csc2_max_c;   /* 0x19a0 */
    unsigned int reserved_27[23];                /* 23: 0x19a4~0x19fc */
} hipp_csc_s;

typedef struct {
    u_src1_afbc_rsv src_afbc_rsv;
    u_src1_afbc_mac_ctrl src_afbc_mac_ctrl;
    unsigned int reserved_34[6];                              /* 6: 0x2008~0x201c */
    u_src1_afbc_smmu_bypass src_afbc_smmu_bypass;             /* 0x2020 */
    unsigned int reserved_35[8];                              /* 8: 0x2024~0x2040 */
    u_src1_afbc_src_reso src_afbc_src_reso;                   /* 0x2044 */
    u_src1_afbc_src_crop src_afbc_src_crop;                   /* 0x2048 */
    u_src1_afbc_ireso src_afbc_ireso;                         /* 0x204c */
    u_src1_afbc_addr_h src_afbc_addr_h;                       /* 0x2050 */
    u_src1_afbc_addr_l src_afbc_addr_l;                       /* 0x2054 */
    unsigned int reserved_36[2];                              /* 2: 0x2058~0x205c */
    u_src1_afbc_stride src_afbc_stride;                       /* 0x2060 */
    unsigned int reserved_37[7];                              /* 7: 0x2064~0x207c */
    u_src1_afbc_head_addr_h src_afbc_head_addr_h;             /* 0x2080 */
    u_src1_afbc_head_addr_l src_afbc_head_addr_l;             /* 0x2084 */
    u_src1_afbc_head_stride src_afbc_head_stride;             /* 0x2088 */
    unsigned int reserved_38[13];                             /* 13: 0x208c~0x20bc */
    u_src1_afbc_debug_ctrl src_afbc_debug_ctrl;               /* 0x20c0 */
    u_src1_afbc_press_ctrl src_afbc_press_ctrl;               /* 0x20c4 */
    unsigned int reserved_39[6];                              /* 6: 0x20c8~0x20dc */
    u_src1_afbc_in_ar_checksum0 src_afbc_in_ar_checksum0;     /* 0x20e0 */
    u_src1_afbc_in_ar_checksum1 src_afbc_in_ar_checksum1;     /* 0x20e4 */
    u_src1_afbc_in_gb_checksum0 src_afbc_in_gb_checksum0;     /* 0x20e8 */
    u_src1_afbc_in_gb_checksum1 src_afbc_in_gb_checksum1;     /* 0x20ec */
    u_src1_afbc_chn0_fifosta0 src_afbc_chn0_fifosta0;         /* 0x20f0 */
    u_src1_afbc_chn0_fifosta1 src_afbc_chn0_fifosta1;         /* 0x20f4 */
    u_src1_afbc_chn1_fifosta0 src_afbc_chn1_fifosta0;         /* 0x20f8 */
    u_src1_afbc_chn1_fifosta1 src_afbc_chn1_fifosta1;         /* 0x20fc */
    u_src1_afbc_cur_flow src_afbc_cur_flow;                   /* 0x2100 */
    u_src1_afbc_cur_sreq_time src_afbc_cur_sreq_time;         /* 0x2104 */
    u_src1_afbc_last_flow src_afbc_last_flow;                 /* 0x2108 */
    u_src1_afbc_last_sreq_time src_afbc_last_sreq_time;       /* 0x210c */
    u_src1_afbc_busy_time src_afbc_busy_time;                 /* 0x2110 */
    u_src1_afbc_ar_neednordy_time src_afbc_ar_neednordy_time; /* 0x2114 */
    u_src1_afbc_gb_neednordy_time src_afbc_gb_neednordy_time; /* 0x2118 */
    unsigned int reserved_40;                                 /* 0x211c */
    u_src1_afbc_work_reso src_afbc_work_reso;                 /* 0x2120 */
    u_src1_afbc_work_finfo src_afbc_work_finfo;               /* 0x2124 */
    u_src1_afbc_last_addr src_afbc_last_addr;                 /* 0x2128 */
    unsigned int reserved_41;                                 /* 0x212c */
    u_src1_afbc_work_read_ctrl src_afbc_work_read_ctrl;       /* 0x2130 */
    u_src1_afbc_work_smmu_ctrl src_afbc_work_smmu_ctrl;       /* 0x2134 */
    u_src1_afbc_work_addr src_afbc_work_addr;                 /* 0x2138 */
    u_src1_afbc_work_naddr src_afbc_work_naddr;               /* 0x213c */
    u_src1_afbc_dcmp_cfg src_afbc_dcmp_cfg;                   /* 0x2140 */
    u_src1_afbc_dcmp0_error src_afbc_dcmp0_error;             /* 0x2144 */
    u_src1_afbc_dcmp0_debug src_afbc_dcmp0_debug;             /* 0x2148 */
    u_src1_afbc_dcmp1_error src_afbc_dcmp1_error;             /* 0x214c */
    u_src1_afbc_dcmp1_debug src_afbc_dcmp1_debug;             /* 0x2150 */
    unsigned int reserved_42[43];                             /* 43: 0x2154~0x21fc */
} src_afbc_cfg_s;

typedef struct {
    unsigned int reserved_60[700];                                         /* 700: 0x2b10~0x35fc */
    u_src1_db_imap_ctrl src1_db_imap_ctrl;                                 /* 0x3600 */
    u_src1_db_imap_in_rangemin src1_db_imap_in_rangemin;                   /* 0x3604 */
    u_src1_db_imap_in_range src1_db_imap_in_range;                         /* 0x3608 */
    u_src1_db_imap_in_rangeinv src1_db_imap_in_rangeinv;                   /* 0x360c */
    u_src1_db_imap_iptoff0 src1_db_imap_iptoff0;                           /* 0x3610 */
    u_src1_db_imap_iptoff1 src1_db_imap_iptoff1;                           /* 0x3614 */
    u_src1_db_imap_iptoff2 src1_db_imap_iptoff2;                           /* 0x3618 */
    u_src1_db_imap_iptscale src1_db_imap_iptscale;                         /* 0x361c */
    u_src1_db_imap_para_ren src1_db_imap_para_ren;                         /* 0x3620 */
    u_src1_db_imap_para_data src1_db_imap_para_data;                       /* 0x3624 */
    u_src1_db_imap_para_up src1_db_imap_para_up;                           /* 0x3628 */
    u_src1_db_imap_degamma_clip_max src1_db_imap_degamma_clip_max;         /* 0x362c */
    u_src1_db_imap_degamma_clip_min src1_db_imap_degamma_clip_min;         /* 0x3630 */
    u_src1_db_imap_ladj_weight src1_db_imap_ladj_weight;                   /* 0x3634 */
    unsigned int reserved_61[50];                                          /* 50: 0x3638~0x36fc */
    u_src1_db_imap_yuv2rgb_ctrl src1_db_imap_yuv2rgb_ctrl;                 /* 0x3700 */
    u_src1_db_imap_yuv2rgb_coef00 src1_db_imap_yuv2rgb_coef00;             /* 0x3704 */
    u_src1_db_imap_yuv2rgb_coef01 src1_db_imap_yuv2rgb_coef01;             /* 0x3708 */
    u_src1_db_imap_yuv2rgb_coef02 src1_db_imap_yuv2rgb_coef02;             /* 0x370c */
    u_src1_db_imap_yuv2rgb_coef03 src1_db_imap_yuv2rgb_coef03;             /* 0x3710 */
    u_src1_db_imap_yuv2rgb_coef04 src1_db_imap_yuv2rgb_coef04;             /* 0x3714 */
    u_src1_db_imap_yuv2rgb_coef05 src1_db_imap_yuv2rgb_coef05;             /* 0x3718 */
    u_src1_db_imap_yuv2rgb_coef06 src1_db_imap_yuv2rgb_coef06;             /* 0x371c */
    u_src1_db_imap_yuv2rgb_coef07 src1_db_imap_yuv2rgb_coef07;             /* 0x3720 */
    u_src1_db_imap_yuv2rgb_coef08 src1_db_imap_yuv2rgb_coef08;             /* 0x3724 */
    u_src1_db_imap_yuv2rgb_coef10 src1_db_imap_yuv2rgb_coef10;             /* 0x3728 */
    u_src1_db_imap_yuv2rgb_coef11 src1_db_imap_yuv2rgb_coef11;             /* 0x372c */
    u_src1_db_imap_yuv2rgb_coef12 src1_db_imap_yuv2rgb_coef12;             /* 0x3730 */
    u_src1_db_imap_yuv2rgb_coef13 src1_db_imap_yuv2rgb_coef13;             /* 0x3734 */
    u_src1_db_imap_yuv2rgb_coef14 src1_db_imap_yuv2rgb_coef14;             /* 0x3738 */
    u_src1_db_imap_yuv2rgb_coef15 src1_db_imap_yuv2rgb_coef15;             /* 0x373c */
    u_src1_db_imap_yuv2rgb_coef16 src1_db_imap_yuv2rgb_coef16;             /* 0x3740 */
    u_src1_db_imap_yuv2rgb_coef17 src1_db_imap_yuv2rgb_coef17;             /* 0x3744 */
    u_src1_db_imap_yuv2rgb_coef18 src1_db_imap_yuv2rgb_coef18;             /* 0x3748 */
    u_src1_db_imap_yuv2rgb_scale2p src1_db_imap_yuv2rgb_scale2p;           /* 0x374c */
    u_src1_db_imap_yuv2rgb_in_dc00 src1_db_imap_yuv2rgb_in_dc00;           /* 0x3750 */
    u_src1_db_imap_yuv2rgb_in_dc01 src1_db_imap_yuv2rgb_in_dc01;           /* 0x3754 */
    u_src1_db_imap_yuv2rgb_in_dc02 src1_db_imap_yuv2rgb_in_dc02;           /* 0x3758 */
    u_src1_db_imap_yuv2rgb_out_dc00 src1_db_imap_yuv2rgb_out_dc00;         /* 0x375c */
    u_src1_db_imap_yuv2rgb_out_dc01 src1_db_imap_yuv2rgb_out_dc01;         /* 0x3760 */
    u_src1_db_imap_yuv2rgb_out_dc02 src1_db_imap_yuv2rgb_out_dc02;         /* 0x3764 */
    u_src1_db_imap_yuv2rgb_in_dc10 src1_db_imap_yuv2rgb_in_dc10;           /* 0x3768 */
    u_src1_db_imap_yuv2rgb_in_dc11 src1_db_imap_yuv2rgb_in_dc11;           /* 0x376c */
    u_src1_db_imap_yuv2rgb_in_dc12 src1_db_imap_yuv2rgb_in_dc12;           /* 0x3770 */
    u_src1_db_imap_yuv2rgb_out_dc10 src1_db_imap_yuv2rgb_out_dc10;         /* 0x3774 */
    u_src1_db_imap_yuv2rgb_out_dc11 src1_db_imap_yuv2rgb_out_dc11;         /* 0x3778 */
    u_src1_db_imap_yuv2rgb_out_dc12 src1_db_imap_yuv2rgb_out_dc12;         /* 0x377c */
    u_src1_db_imap_yuv2rgb_min src1_db_imap_yuv2rgb_min;                   /* 0x3780 */
    u_src1_db_imap_yuv2rgb_max src1_db_imap_yuv2rgb_max;                   /* 0x3784 */
    u_src1_db_imap_yuv2rgb_thr src1_db_imap_yuv2rgb_thr;                   /* 0x3788 */
    unsigned int reserved_62[29];                                          /* 29: 0x378c~0x37fc */
    u_src1_db_imap_rgb2lms_ctrl src1_db_imap_rgb2lms_ctrl;                 /* 0x3800 */
    u_src1_db_imap_rgb2lms_coef0 src1_db_imap_rgb2lms_coef0;               /* 0x3804 */
    u_src1_db_imap_rgb2lms_coef1 src1_db_imap_rgb2lms_coef1;               /* 0x3808 */
    u_src1_db_imap_rgb2lms_coef2 src1_db_imap_rgb2lms_coef2;               /* 0x380c */
    u_src1_db_imap_rgb2lms_coef3 src1_db_imap_rgb2lms_coef3;               /* 0x3810 */
    u_src1_db_imap_rgb2lms_coef4 src1_db_imap_rgb2lms_coef4;               /* 0x3814 */
    u_src1_db_imap_rgb2lms_coef5 src1_db_imap_rgb2lms_coef5;               /* 0x3818 */
    u_src1_db_imap_rgb2lms_coef6 src1_db_imap_rgb2lms_coef6;               /* 0x381c */
    u_src1_db_imap_rgb2lms_coef7 src1_db_imap_rgb2lms_coef7;               /* 0x3820 */
    u_src1_db_imap_rgb2lms_coef8 src1_db_imap_rgb2lms_coef8;               /* 0x3824 */
    u_src1_db_imap_rgb2lms_scale2p src1_db_imap_rgb2lms_scale2p;           /* 0x3828 */
    u_src1_db_imap_rgb2lms_min src1_db_imap_rgb2lms_min;                   /* 0x382c */
    u_src1_db_imap_rgb2lms_max src1_db_imap_rgb2lms_max;                   /* 0x3830 */
    u_src1_db_imap_rgb2lms_in_dc0 src1_db_imap_rgb2lms_in_dc0;             /* 0x3834 */
    u_src1_db_imap_rgb2lms_in_dc1 src1_db_imap_rgb2lms_in_dc1;             /* 0x3838 */
    u_src1_db_imap_rgb2lms_in_dc2 src1_db_imap_rgb2lms_in_dc2;             /* 0x383c */
    unsigned int reserved_63[48];                                          /* 48: 0x3840~0x38fc */
    u_src1_db_imap_lms2ipt_ctrl src1_db_imap_lms2ipt_ctrl;                 /* 0x3900 */
    u_src1_db_imap_lms2ipt_coef0 src1_db_imap_lms2ipt_coef0;               /* 0x3904 */
    u_src1_db_imap_lms2ipt_coef1 src1_db_imap_lms2ipt_coef1;               /* 0x3908 */
    u_src1_db_imap_lms2ipt_coef2 src1_db_imap_lms2ipt_coef2;               /* 0x390c */
    u_src1_db_imap_lms2ipt_coef3 src1_db_imap_lms2ipt_coef3;               /* 0x3910 */
    u_src1_db_imap_lms2ipt_coef4 src1_db_imap_lms2ipt_coef4;               /* 0x3914 */
    u_src1_db_imap_lms2ipt_coef5 src1_db_imap_lms2ipt_coef5;               /* 0x3918 */
    u_src1_db_imap_lms2ipt_coef6 src1_db_imap_lms2ipt_coef6;               /* 0x391c */
    u_src1_db_imap_lms2ipt_coef7 src1_db_imap_lms2ipt_coef7;               /* 0x3920 */
    u_src1_db_imap_lms2ipt_coef8 src1_db_imap_lms2ipt_coef8;               /* 0x3924 */
    u_src1_db_imap_lms2ipt_scale2p src1_db_imap_lms2ipt_scale2p;           /* 0x3928 */
    u_src1_db_imap_lms2ipt_min src1_db_imap_lms2ipt_min;                   /* 0x392c */
    u_src1_db_imap_lms2ipt_max src1_db_imap_lms2ipt_max;                   /* 0x3930 */
    u_src1_db_imap_lms2ipt_out_dc0 src1_db_imap_lms2ipt_out_dc0;           /* 0x3934 */
    u_src1_db_imap_lms2ipt_out_dc1 src1_db_imap_lms2ipt_out_dc1;           /* 0x3938 */
    u_src1_db_imap_lms2ipt_out_dc2 src1_db_imap_lms2ipt_out_dc2;           /* 0x393c */
    u_src1_db_imap_lms2ipt_in_dc0 src1_db_imap_lms2ipt_in_dc0;             /* 0x3940 */
    u_src1_db_imap_lms2ipt_in_dc1 src1_db_imap_lms2ipt_in_dc1;             /* 0x3944 */
    u_src1_db_imap_lms2ipt_in_dc2 src1_db_imap_lms2ipt_in_dc2;             /* 0x3948 */
    unsigned int reserved_64[45];                                          /* 45: 0x394c~0x39fc */
    u_src1_db_imap_tmap_v1_ctrl src1_db_imap_tmap_v1_ctrl;                 /* 0x3a00 */
    u_src1_db_imap_tmap_v1_ren src1_db_imap_tmap_v1_ren;                   /* 0x3a04 */
    u_src1_db_imap_tmap_v1_data src1_db_imap_tmap_v1_data;                 /* 0x3a08 */
    u_src1_db_imap_tmap_v1_mix_alpha src1_db_imap_tmap_v1_mix_alpha;       /* 0x3a0c */
    u_src1_db_imap_tmap_v1_step1 src1_db_imap_tmap_v1_step1;               /* 0x3a10 */
    u_src1_db_imap_tmap_v1_step2 src1_db_imap_tmap_v1_step2;               /* 0x3a14 */
    u_src1_db_imap_tmap_v1_pos1 src1_db_imap_tmap_v1_pos1;                 /* 0x3a18 */
    u_src1_db_imap_tmap_v1_pos2 src1_db_imap_tmap_v1_pos2;                 /* 0x3a1c */
    u_src1_db_imap_tmap_v1_pos3 src1_db_imap_tmap_v1_pos3;                 /* 0x3a20 */
    u_src1_db_imap_tmap_v1_pos4 src1_db_imap_tmap_v1_pos4;                 /* 0x3a24 */
    u_src1_db_imap_tmap_v1_pos5 src1_db_imap_tmap_v1_pos5;                 /* 0x3a28 */
    u_src1_db_imap_tmap_v1_pos6 src1_db_imap_tmap_v1_pos6;                 /* 0x3a2c */
    u_src1_db_imap_tmap_v1_pos7 src1_db_imap_tmap_v1_pos7;                 /* 0x3a30 */
    u_src1_db_imap_tmap_v1_pos8 src1_db_imap_tmap_v1_pos8;                 /* 0x3a34 */
    u_src1_db_imap_tmap_v1_num1 src1_db_imap_tmap_v1_num1;                 /* 0x3a38 */
    u_src1_db_imap_tmap_v1_num2 src1_db_imap_tmap_v1_num2;                 /* 0x3a3c */
    unsigned int reserved_65[12];                                          /* 12: 0x3a40~0x3a6c */
    u_src1_db_imap_tmap_v1_luma_coef0 src1_db_imap_tmap_v1_luma_coef0;     /* 0x3a70 */
    u_src1_db_imap_tmap_v1_luma_coef1 src1_db_imap_tmap_v1_luma_coef1;     /* 0x3a74 */
    u_src1_db_imap_tmap_v1_luma_coef2 src1_db_imap_tmap_v1_luma_coef2;     /* 0x3a78 */
    u_src1_db_imap_tmap_v1_luma_scale src1_db_imap_tmap_v1_luma_scale;     /* 0x3a7c */
    u_src1_db_imap_tmap_v1_coef_scale src1_db_imap_tmap_v1_coef_scale;     /* 0x3a80 */
    u_src1_db_imap_tmap_v1_out_clip_min src1_db_imap_tmap_v1_out_clip_min; /* 0x3a84 */
    u_src1_db_imap_tmap_v1_out_clip_max src1_db_imap_tmap_v1_out_clip_max; /* 0x3a88 */
    u_src1_db_imap_tmap_v1_out_dc0 src1_db_imap_tmap_v1_out_dc0;           /* 0x3a8c */
    u_src1_db_imap_tmap_v1_out_dc1 src1_db_imap_tmap_v1_out_dc1;           /* 0x3a90 */
    u_src1_db_imap_tmap_v1_out_dc2 src1_db_imap_tmap_v1_out_dc2;           /* 0x3a94 */
    unsigned int reserved_66[26];                                          /* 26: 0x3a98~0x3afc */
    u_src1_db_imap_cacm_reg0 src1_db_imap_cacm_reg0;                       /* 0x3b00 */
    u_src1_db_imap_cacm_reg1 src1_db_imap_cacm_reg1;                       /* 0x3b04 */
    u_src1_db_imap_cacm_reg2 src1_db_imap_cacm_reg2;                       /* 0x3b08 */
    u_src1_db_imap_cacm_reg3 src1_db_imap_cacm_reg3;                       /* 0x3b0c */
} src_hdr_cfg_s;

typedef struct {
    u_src1_xdp_gfx_halfp0 src1_xdp_gfx_halfp0;   /* 0x7000 */
    u_src1_xdp_gfx_halfp1 src1_xdp_gfx_halfp1;   /* 0x7004 */
    u_src1_xdp_gfx_halfp2 src1_xdp_gfx_halfp2;   /* 0x7008 */
    u_src1_xdp_gfx_halfp3 src1_xdp_gfx_halfp3;   /* 0x700c */
    u_src1_xdp_gfx_halfp4 src1_xdp_gfx_halfp4;   /* 0x7010 */
    u_src1_xdp_gfx_halfp5 src1_xdp_gfx_halfp5;   /* 0x7014 */
    u_src1_xdp_gfx_halfp6 src1_xdp_gfx_halfp6;   /* 0x7018 */
    u_src1_xdp_gfx_halfp7 src1_xdp_gfx_halfp7;   /* 0x701c */
    u_src1_xdp_gfx_halfp8 src1_xdp_gfx_halfp8;   /* 0x7020 */
    u_src1_xdp_gfx_halfp9 src1_xdp_gfx_halfp9;   /* 0x7024 */
    u_src1_xdp_gfx_halfp10 src1_xdp_gfx_halfp10; /* 0x7028 */
    u_src1_xdp_gfx_halfp11 src1_xdp_gfx_halfp11; /* 0x702c */
    u_src1_xdp_gfx_halfp12 src1_xdp_gfx_halfp12; /* 0x7030 */
    u_src1_xdp_gfx_halfp13 src1_xdp_gfx_halfp13; /* 0x7034 */
    u_src1_xdp_gfx_halfp14 src1_xdp_gfx_halfp14; /* 0x7038 */
    u_src1_xdp_gfx_halfp15 src1_xdp_gfx_halfp15; /* 0x703c */
    u_src1_xdp_gfx_halfp16 src1_xdp_gfx_halfp16; /* 0x7040 */
    unsigned int reserved_89[47];                /* 47: 0x7044~0x70fc */
} src_vdp_gfx_hal_fp_s;

typedef struct {
    u_tde_src1_hdr_para_haddr_high src1_hdr_para_haddr_high;     /* 0x40 */
    u_tde_src1_hdr_para_haddr_low src1_hdr_para_haddr_low;       /* 0x44 */
    u_tde_src1_hdr_1_para_haddr_high src1_hdr_1_para_haddr_high; /* 0x48 */
    u_tde_src1_hdr_1_para_haddr_low src1_hdr_1_para_haddr_low;   /* 0x4c */
    u_tde_src1_hdr_2_para_haddr_high src1_hdr_2_para_haddr_high; /* 0x50 */
    u_tde_src1_hdr_2_para_haddr_low src1_hdr_2_para_haddr_low;   /* 0x54 */
    u_tde_src1_hdr_3_para_haddr_high src1_hdr_3_para_haddr_high; /* 0x58 */
    u_tde_src1_hdr_3_para_haddr_low src1_hdr_3_para_haddr_low;   /* 0x5c */
} src_hdr_para_addr_s;

typedef struct {
    u_tde_src1_hdr_cfg_addr_low src1_hdr_cfg_addr_low;   /* 0x1a0 */
    u_tde_src1_hdr_cfg_addr_high src1_hdr_cfg_addr_high; /* 0x1a4 */
} src_hdr_cfg_addr_s;

typedef struct {
    u_tde_src1_zme_haddr_high src1_zme_haddr_high;               /* 0x0 */
    u_tde_src1_zme_haddr_low src1_zme_haddr_low;                 /* 0x4 */
    u_tde_src1_zme_vaddr_high src1_zme_vaddr_high;               /* 0x8 */
    u_tde_src1_zme_vaddr_low src1_zme_vaddr_low;                 /* 0xc */
    u_tde_src2_zme_haddr_high src2_zme_haddr_high;               /* 0x10 */
    u_tde_src2_zme_haddr_low src2_zme_haddr_low;                 /* 0x14 */
    u_tde_src2_zme_vaddr_high src2_zme_vaddr_high;               /* 0x18 */
    u_tde_src2_zme_vaddr_low src2_zme_vaddr_low;                 /* 0x1c */
    u_tde_src3_zme_haddr_high src3_zme_haddr_high;               /* 0x20 */
    u_tde_src3_zme_haddr_low src3_zme_haddr_low;                 /* 0x24 */
    u_tde_src3_zme_vaddr_high src3_zme_vaddr_high;               /* 0x28 */
    u_tde_src3_zme_vaddr_low src3_zme_vaddr_low;                 /* 0x2c */
    unsigned int reserved_0[4];                                  /* 4: 0x30~0x3c */
    u_tde_src1_hdr_para_haddr_high src1_hdr_para_haddr_high;     /* 0x40 */
    u_tde_src1_hdr_para_haddr_low src1_hdr_para_haddr_low;       /* 0x44 */
    u_tde_src1_hdr_1_para_haddr_high src1_hdr_1_para_haddr_high; /* 0x48 */
    u_tde_src1_hdr_1_para_haddr_low src1_hdr_1_para_haddr_low;   /* 0x4c */
    u_tde_src1_hdr_2_para_haddr_high src1_hdr_2_para_haddr_high; /* 0x50 */
    u_tde_src1_hdr_2_para_haddr_low src1_hdr_2_para_haddr_low;   /* 0x54 */
    u_tde_src1_hdr_3_para_haddr_high src1_hdr_3_para_haddr_high; /* 0x58 */
    u_tde_src1_hdr_3_para_haddr_low src1_hdr_3_para_haddr_low;   /* 0x5c */
    u_tde_src3_hdr_para_haddr_high src3_hdr_para_haddr_high;     /* 0x60 */
    u_tde_src3_hdr_para_haddr_low src3_hdr_para_haddr_low;       /* 0x64 */
    u_tde_src3_hdr_1_para_haddr_high src3_hdr_1_para_haddr_high; /* 0x68 */
    u_tde_src3_hdr_1_para_haddr_low src3_hdr_1_para_haddr_low;   /* 0x6c */
    u_tde_src3_hdr_2_para_haddr_high src3_hdr_2_para_haddr_high; /* 0x70 */
    u_tde_src3_hdr_2_para_haddr_low src3_hdr_2_para_haddr_low;   /* 0x74 */
    u_tde_src3_hdr_3_para_haddr_high src3_hdr_3_para_haddr_high; /* 0x78 */
    u_tde_src3_hdr_3_para_haddr_low src3_hdr_3_para_haddr_low;   /* 0x7c */
    u_tde_src4_hdr_para_haddr_high src4_hdr_para_haddr_high;     /* 0x80 */
    u_tde_src4_hdr_para_haddr_low src4_hdr_para_haddr_low;       /* 0x84 */
    u_tde_src4_hdr_1_para_haddr_high src4_hdr_1_para_haddr_high; /* 0x88 */
    u_tde_src4_hdr_1_para_haddr_low src4_hdr_1_para_haddr_low;   /* 0x8c */
    u_tde_src4_hdr_2_para_haddr_high src4_hdr_2_para_haddr_high; /* 0x90 */
    u_tde_src4_hdr_2_para_haddr_low src4_hdr_2_para_haddr_low;   /* 0x94 */
    u_tde_src4_hdr_3_para_haddr_high src4_hdr_3_para_haddr_high; /* 0x98 */
    u_tde_src4_hdr_3_para_haddr_low src4_hdr_3_para_haddr_low;   /* 0x9c */
    u_tde_src5_hdr_para_haddr_high src5_hdr_para_haddr_high;     /* 0xa0 */
    u_tde_src5_hdr_para_haddr_low src5_hdr_para_haddr_low;       /* 0xa4 */
    u_tde_src5_hdr_1_para_haddr_high src5_hdr_1_para_haddr_high; /* 0xa8 */
    u_tde_src5_hdr_1_para_haddr_low src5_hdr_1_para_haddr_low;   /* 0xac */
    u_tde_src5_hdr_2_para_haddr_high src5_hdr_2_para_haddr_high; /* 0xb0 */
    u_tde_src5_hdr_2_para_haddr_low src5_hdr_2_para_haddr_low;   /* 0xb4 */
    u_tde_src5_hdr_3_para_haddr_high src5_hdr_3_para_haddr_high; /* 0xb8 */
    u_tde_src5_hdr_3_para_haddr_low src5_hdr_3_para_haddr_low;   /* 0xbc */
    unsigned int reserved_1[16];                                 /* 16: 0xc0~0xfc */
    u_para_up para_up;                                           /* 0x100 */
    unsigned int reserved_2[37];                                 /* 37: 0x104~0x194 */
    u_tde_gfx_cfg_addr_low gfx_cfg_addr_low;                     /* 0x198 */
    u_tde_gfx_addr_high gfx_addr_high;                           /* 0x19c */
    u_tde_src1_hdr_cfg_addr_low src1_hdr_cfg_addr_low;           /* 0x1a0 */
    u_tde_src1_hdr_cfg_addr_high src1_hdr_cfg_addr_high;         /* 0x1a4 */
    u_tde_src3_hdr_cfg_addr_low src3_hdr_cfg_addr_low;           /* 0x1a8 */
    u_tde_src3_hdr_cfg_addr_high src3_hdr_cfg_addr_high;         /* 0x1ac */
    u_tde_src4_hdr_cfg_addr_low src4_hdr_cfg_addr_low;           /* 0x1b0 */
    u_tde_src4_hdr_cfg_addr_high src4_hdr_cfg_addr_high;         /* 0x1b4 */
    u_tde_src5_hdr_cfg_addr_low src5_hdr_cfg_addr_low;           /* 0x1b8 */
    u_tde_src5_hdr_cfg_addr_high src5_hdr_cfg_addr_high;         /* 0x1bc */
    u_tde_zme_cfg_addr_low zme_cfg_addr_low;                     /* 0x1c0 */
    u_tde_zme_cfg_addr_high zme_cfg_addr_high;                   /* 0x1c4 */
    unsigned int reserved_3[2];                                  /* 2: 0x1c8~0x1cc */
    u_tde_lut_cfg_addr_high lut_cfg_addr_high;                   /* 0x1d0 */
    u_tde_lut_cfg_addr_low lut_cfg_addr_low;                     /* 0x1d4 */
    unsigned int reserved_4[2];                                  /* 2: 0x1d8~0x1dc */
    u_tde_cbm_cfg_addr_high cbm_cfg_addr_high;                   /* 0x1e0 */
    u_tde_cbm_cfg_addr_low cbm_cfg_addr_low;                     /* 0x1e4 */
    u_tde_afbc_cfg_addr_high afbc_cfg_addr_high;                 /* 0x1e8 */
    u_tde_afbc_cfg_addr_low afbc_cfg_addr_low;                   /* 0x1ec */
    u_tde_intmask intmask;                                       /* 0x1f0 */
    u_tde_nodeid nodeid;                                         /* 0x1f4 */
    hi_u32 next_node_phyaddr;                                    /* 0x1f8 */
    u_tde_pnext_hi tde_pnext_hi;                                 /* 0x1fc */
    video_layer src_Layer[2];                                    /* 0x200 */
    hi_u32 reserve;                                              /* 0x258 */
    gfx_layer astLayer[8];                                       /* 8: 0x25c ~ 0x358 */
    u_dst_ctrl dst_ctrl;                                         /* 0x35c */
    hi_u32 dst_ch0_addr_h;
    hi_u32 dst_ch0_addr_l;
    hi_u32 dst_ch1_addr_h;
    hi_u32 dst_ch1_addr_l;
    hi_u32 dst_ch2_addr_h;
    hi_u32 dst_ch2_addr_l;
    hi_u32 dst_ch0_stride;
    hi_u32 dst_ch1_stride;
    hi_u32 dst_ch2_stride;
    u_dst_imgsize dst_image_size;        /* 0x384 */
    u_dst_alpha dst_alpha;               /* 0x388 */
    HWC_POS_U unOutCropStart;            /* 0x38c */
    HWC_POS_U unOutCropEnd;              /* 0x390 */
    hi_u32 u32OutPixOffset;              /* 0x394 */
    u_dst_head0_addr_h dst_head0_addr_h; /* 0x398 */
    hi_u32 dst_head0_addr_l;             /* 0x39c */
    u_dst_head1_addr_h dst_head1_addr_h; /* 0x3a0 */
    hi_u32 dst_head1_addr_l;             /* 0x3a4 */
    u_dst_ch0_chk_sum dst_ch0_chk_sum;   /* 0x3a8 */
    u_dst_ch1_chk_sum dst_ch1_chk_sum;   /* 0x3ac */
    u_dst_ch2_chk_sum dst_ch2_chk_sum;   /* 0x3b0 */
    u_dst_cmd_ctrl dst_cmd_ctrl;         /* 0x3b4 */
#ifdef CONFIG_GFX_HWC_VERSION_3_0
    HWC_MMU_PROT_CTL_U unMmuCtl; /* 0x3b8 */
#endif
    u_stt_alpha_addr_h stt_alpha_addr_h; /* 0x3bc */
    u_stt_alpha_addr_l stt_alpha_addr_l; /* 0x3c0 */
    u_dst_head0_stride dst_head0_stride;
    u_dst_head1_stride dst_head1_stride;
    unsigned int reserved_5[13];                           /* 13 :0x3cc~0x3fc */
    u_dst_line_osd_cmp_glb_info dst_line_osd_cmp_glb_info; /* 0x400 */
    u_dst_line_osd_cmp_rc_cfg0 dst_line_osd_cmp_rc_cfg0;   /* 0x404 */
    u_dst_line_osd_cmp_rc_cfg1 dst_line_osd_cmp_rc_cfg1;   /* 0x408 */
    u_dst_line_osd_cmp_rc_cfg2 dst_line_osd_cmp_rc_cfg2;   /* 0x40c */
    u_dst_line_osd_cmp_rc_cfg3 dst_line_osd_cmp_rc_cfg3;   /* 0x410 */
    u_dst_line_osd_cmp_rc_cfg4 dst_line_osd_cmp_rc_cfg4;   /* 0x414 */
    u_dst_line_osd_cmp_rc_cfg5 dst_line_osd_cmp_rc_cfg5;   /* 0x418 */
    u_dst_line_osd_cmp_rc_cfg6 dst_line_osd_cmp_rc_cfg6;   /* 0x41c */
    u_dst_line_osd_cmp_rc_cfg7 dst_line_osd_cmp_rc_cfg7;   /* 0x420 */
    u_dst_line_osd_cmp_rc_cfg8 dst_line_osd_cmp_rc_cfg8;   /* 0x424 */
    u_dst_line_osd_cmp_rc_cfg9 dst_line_osd_cmp_rc_cfg9;   /* 0x428 */
    u_dst_line_osd_cmp_rc_cfg10 dst_line_osd_cmp_rc_cfg10; /* 0x42c */
    u_dst_line_osd_cmp_rc_cfg11 dst_line_osd_cmp_rc_cfg11; /* 0x430 */
    u_dst_line_osd_cmp_rc_cfg12 dst_line_osd_cmp_rc_cfg12; /* 0x434 */
    u_dst_line_osd_cmp_rc_cfg13 dst_line_osd_cmp_rc_cfg13; /* 0x438 */
    u_dst_line_osd_cmp_rc_cfg14 dst_line_osd_cmp_rc_cfg14; /* 0x43c */
    u_dst_line_osd_cmp_rc_cfg15 dst_line_osd_cmp_rc_cfg15; /* 0x440 */
    u_dst_line_osd_cmp_rc_cfg16 dst_line_osd_cmp_rc_cfg16; /* 0x444 */
    u_dst_line_osd_cmp_rsv dst_line_osd_cmp_rsv;
    u_dst_line_osd_cmp_glb_st dst_line_osd_cmp_glb_st;
    u_dst_line_osd_cmp_dbg_reg dst_line_osd_cmp_dbg_reg;
    unsigned int reserved_6[43];                 /* 43: 0x454~0x4fc */
    u_tde_start tde_start;                       /* 0x500 */
    unsigned int reserved_7[3];                  /* 3: 0x504~0x50c */
    u_tde_rawint tde_rawint;                     /* 0x510 */
    u_tde_intstate tde_intstate;                 /* 0x514 */
    unsigned int reserved_8[2];                  /* 2: 0x518~0x51c */
    u_tde_memctrl tde_memctrl;                   /* 0x520 */
    u_tde_memctrl1 tde_memctrl1;                 /* 0x524 */
    unsigned int reserved_9[2];                  /* 2: 0x528~0x52c */
    u_tde_miscellaneous tde_miscellaneous;       /* 0x530 */
    u_tde_timeout tde_timeout;                   /* 0x534 */
    unsigned int reserved_10[50];                /* 50: 0x538~0x5fc */
    u_tde_mst_outstanding tde_mst_outstanding;   /* 0x600 */
    u_tde_mst_ctrl tde_mst_ctrl;                 /* 0x604 */
    unsigned int reserved_11[2];                 /* 2: 0x608~0x60c */
    u_tde_mst_rchn_prio0 tde_mst_rchn_prio0;     /* 0x610 */
    unsigned int reserved_12;                    /* 0x614 */
    u_tde_mst_wchn_prio0 tde_mst_wchn_prio0;     /* 0x618 */
    unsigned int reserved_13[9];                 /* 9: 0x61c~0x63c */
    u_tde_mst_bus_err_clr tde_mst_bus_err_clr;   /* 0x640 */
    u_tde_mst_bus_err tde_mst_bus_err;           /* 0x644 */
    unsigned int reserved_14[2];                 /* 2: 0x648~0x64c */
    u_tde_mst_src0_status0 tde_mst_src0_status0; /* 0x650 */
    u_tde_mst_src0_status1 tde_mst_src0_status1; /* 0x654 */
    unsigned int reserved_15[6];                 /* 6: 0x658~0x66c */
    u_tde_mst_debug_ctrl tde_mst_debug_ctrl;     /* 0x670 */
    u_tde_mst_debug_clr tde_mst_debug_clr;       /* 0x674 */
    unsigned int reserved_16[2];                 /* 2: 0x678~0x67c */
    u_tde_mst0_det_latency tde_mst0_det_latency; /* 0x680 */
    unsigned int reserved_17[3];                 /* 3: 0x684~0x68c */
    u_tde_mst0_rd_info tde_mst0_rd_info;         /* 0x690 */
    u_tde_mst0_wr_info tde_mst0_wr_info;         /* 0x694 */
    unsigned int reserved_18[90];                /* 90: 0x698~0x7fc */
    u_cbmctrl cbm_ctrl;                          /* 0x800 */
    u_cbmprio cbmprio;                           /* 0x804 */
    u_cbmbkgrgb cbmbkgrgb;                       /* 0x808 */
    u_cbmbkga cbmbkga;                           /* 0x80c */
    u_cbmcolorizealu1 cbmcolorizealu1;           /* 0x810 */
    u_cbmparaalu1 cbmparaalu1;                   /* 0x814 */
    u_cbmkeyparaalu1 cbmkeyparaalu1;             /* 0x818 */
    u_cbmkeyminalu1 cbmkeyminalu1;               /* 0x81c */
    u_cbmkeyaminalu1 cbmkeyaminalu1;             /* 0x820 */
    u_cbmkeymaxalu1 cbmkeymaxalu1;               /* 0x824 */
    u_cbmkeyamaxalu1 cbmkeyamaxalu1;             /* 0x828 */
    u_cbmkeymaskalu1 cbmkeymaskalu1;             /* 0x82c */
    u_cbmkeyamaskalu1 cbmkeyamaskalu1;           /* 0x830 */
    u_cbmcolorizealu2 cbmcolorizealu2;           /* 0x834 */
    u_cbmparaalu2 cbmparaalu2;                   /* 0x838 */
    u_cbmkeyparaalu2 cbmkeyparaalu2;             /* 0x83c */
    u_cbmkeyminalu2 cbmkeyminalu2;               /* 0x840 */
    u_cbmkeyaminalu2 cbmkeyaminalu2;             /* 0x844 */
    u_cbmkeymaxalu2 cbmkeymaxalu2;               /* 0x848 */
    u_cbmkeyamaxalu2 cbmkeyamaxalu2;             /* 0x84c */
    u_cbmkeymaskalu2 cbmkeymaskalu2;             /* 0x850 */
    u_cbmkeyamaskalu2 cbmkeyamaskalu2;           /* 0x854 */
    u_srcmiddle_cbmpara srcmiddle_cbmpara;       /* 0x858 */
    HWC_CBMINFO_S cbm_info[10];                  /* 10 layer: 0x85c */
    u_cbmmixwindowctrl1 cbmmixwindowctrl1;       /* 0x8ac */
    u_cbmmixwindowctrl2 cbmmixwindowctrl2;       /* 0x8b0 */
    dim_window_s dim_window[10];                 /* 10 layer: 0x8b4 ~ 0x9f0 */
    window_argb_s window_argb[10];               /* 10 layer: 0x9f4 ~ 0x0B30 */
    u_cbm_imgsize cbm_imgsize;                   /* 0xb34 */
    unsigned int reserved_19[306];               /* 306: 0xb38~0xffc */
    src_zme_s src_zme[3];                        /* 3 layer support zme */
    unsigned int reserved_20[128];               /* 128: 0x17fc */
    u_src1_hpzme src_hpzme[3];                   /* 3 layer support zme */
    unsigned int reserved_26[61];                /* 61: 0x180c~0x18fc */
    hipp_csc_s hipp_csc[3];                      /* 3 for src src3 dst */
    u_tde_csc_ctrl tde_csc_ctrl;                 /* 0x1c00 */
    unsigned int reserved_30[63];                /* 63: 0x1c04~0x1cfc */
    u_dst_h_dswm dst_h_dswm;
    unsigned int reserved_31; /* 0x1d04 */
    u_dst_h_dwsm_hloffset dst_h_dwsm_hloffset;
    u_dst_h_dwsm_hcoffset dst_h_dwsm_hcoffset;
    unsigned int reserved_32[60]; /* 60: 0x1d10~0x1dfc */
    u_dst_v_dswm dst_v_dswm;

    hi_u32 u32AXIID;  /* 0x4f0 */
    hi_u32 u32NodeId; /* 0x4f4 */
    hi_void *pNextNode;
    hi_void *pFirstNode;
} hwc_node;

/* hardware layer ID definition */
#define HWC_LAYER_ID hwc_layer_id

/* horizontal Coefficient Storage: Each 8 Coefficient Group, 3 + 3 + 2 occupies 12 byte */
#define HWC_HORIZON_LUM_PARA_SIZE_PERLEVEL (((GFX2D_FILTER_PHASE * 12) + 0xf) & 0xfffffff0)
/* Vertical Coefficient Storage: Each group of 4 coefficients occupies 8 byte. */
#define HWC_VERTICAL_LUM_PARA_SIZE_PERLEVEL (((GFX2D_FILTER_PHASE * 8) + 0xf) & 0xfffffff0)

#define HWC_HORIZON_CHM_PARA_SIZE_PERLEVEL (((GFX2D_FILTER_PHASE * 12) + 0xf) & 0xfffffff0)
#define HWC_VERTICAL_CHM_PARA_SIZE_PERLEVEL (((GFX2D_FILTER_PHASE * 8) + 0xf) & 0xfffffff0)

#define HWC_HORIZON_PARA_SIZE_PERLEVEL (HWC_HORIZON_LUM_PARA_SIZE_PERLEVEL)
#define HWC_VERTICAL_PARA_SIZE_PERLEVEL (HWC_VERTICAL_LUM_PARA_SIZE_PERLEVEL)

#define HWC_HORIZON_PARA_SIZE_TOTAL (GFX2D_FILTER_LEVELS * HWC_HORIZON_PARA_SIZE_PERLEVEL)
#define HWC_VERTICAL_PARA_SIZE_TOTAL (GFX2D_FILTER_LEVELS * HWC_VERTICAL_PARA_SIZE_PERLEVEL)

/* ********************* Global Variable declaration ********************************************* */
/* debug level */
static hi_u32 g_gfx2d_hal_debug_level = 0; /* 0 algorithm data */

static volatile hi_u32 *g_hwc_reg = HI_NULL;
#ifdef GFX2D_ALPHADETECT_SUPPORT
static volatile HWC_ALPHASUM_REG_S *g_pst_hwc_alpha_sum_reg = HI_NULL;
#endif

/* ****************************** API forward declarations *************************************** */
static hi_void hwc_hal_set_cfg_reg_offset(hwc_node *node);
static hi_s32 select_layer_id(drv_gfx2d_compose_list *compose_list, HWC_LAYER_ID *layer_id, hi_u32 layer_num);

static hi_s32 InitFilterParam(hi_void); /* load filter coefficients */
static hi_void DeinitFilterParam(hi_void);
static hi_void InitCrgReg(hi_void);   /* initial crg register */
static hi_void DeinitCrgReg(hi_void); /* deinit crg register */
static hi_s32 InitDevReg(hi_void);    /* initial deevice register */
static hi_void DeinitDevReg(hi_void); /* deinit crg register */
static hi_s32 CheckCompose(const drv_gfx2d_dev_id dev_id, drv_gfx2d_compose_list *compose_list,
                           drv_gfx2d_compose_surface *pstDstSurface); /* compose parameters check */
static hi_void InitNode(hwc_node *pstNode);                           /* init task node */
static hi_void SetSrc(hwc_node *pstNode, drv_gfx2d_compose_msg *compose, hi_u32 u32Zorder, HWC_LAYER_ID enLayerId);
static hi_void SetDst(hwc_node *pstNode, drv_gfx2d_compose_surface *pstDstSurface);
static hi_void SetInCsc(HWC_LAYER_ID id, hwc_node *node, hi_u16 color_type);  /* set input csc */
static hi_void SetOutCsc(HWC_LAYER_ID id, hwc_node *node, hi_u16 color_type); /* set output csc */
static inline hi_void SetBgColor(hwc_node *pstNode, hi_u32 back_color);
static hi_s32 SelectLayerId(drv_gfx2d_compose_list *compose_list, HWC_LAYER_ID *penLayerId, hi_u32 layer_num);
static hi_void AdjustCompose(const drv_gfx2d_dev_id dev_id, drv_gfx2d_compose_list *compose_list,
                             drv_gfx2d_compose_surface *pstDstSurface);
static hi_s32 GFX2D_HAL_HWC_LinkNode(hi_void *pCurNode, hi_void *pNextNode);
static hi_u16 GetOptCode(drv_gfx2d_compose_fmt enSrc2Fmt, drv_gfx2d_compose_fmt enDstFmt);

static gfx2d_capability_info g_gfx2d_capability = {0};

/* the size of each node */
const hi_u32 g_u32NodeSize = ((sizeof(hwc_node) + 0xf) & 0xfffffff0);

/* ****************************** API realization ************************************************ */
hi_s32 GFX2D_HAL_HWC_Init(hi_void)
{
    hi_s32 s32Ret;
    hi_u32 u32NodeNum;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    s32Ret = InitFilterParam();
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, InitFilterParam, s32Ret);
        return s32Ret;
    }
    u32NodeNum = GFX2D_CONFIG_GetNodeNum();
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, u32NodeNum);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, sizeof(hwc_node));
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_u32NodeSize);

    s32Ret = GFX2D_MEM_Register(g_u32NodeSize, u32NodeNum);
    if (s32Ret != HI_SUCCESS) {
        DeinitFilterParam();
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_MEM_Register, s32Ret);
        return s32Ret;
    }

    HWC_ADP_GetCapability(&g_gfx2d_capability);

    s32Ret = InitDevReg();
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, InitDevReg, s32Ret);
        return s32Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_void GFX2D_HAL_HWC_Deinit(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    DeinitDevReg();
    GFX2D_Mem_UnRegister(g_u32NodeSize);

    DeinitFilterParam();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_s32 GFX2D_HAL_HWC_Open(hi_void)
{
    return HI_SUCCESS;
}

hi_void GFX2D_HAL_HWC_Close(hi_void)
{
    return;
}

#define GFX2D_HAL_HWC_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define GFX2D_HAL_HWC_MIN(a, b) (((a) < (b)) ? (a) : (b))

/* data_in :positive or negetive  ;  bit:complement width */
static hi_u32 GFX2D_HAL_HWC_TrueValueToComplement(hi_s32 data_in, hi_u32 bit)
{
    hi_u32 data_out = 0; /* 0 algorithm data */
    hi_u32 data_in_tmp = (hi_u32)data_in;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (data_in_tmp >> (bit - 1)) {
        data_in_tmp = 0 - data_in;
        data_out = ((1 << (bit - 1)) | (((~data_in_tmp) & ((1 << (bit - 1)) - 1)) + 1)) & ((1 << bit) - 1);
    } else {
        data_out = data_in;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return data_out;
}

/* data_in :positive or negetive  ;  bit:complement width */
static hi_s32 GFX2D_HAL_HWC_ComplementToTrueValue(hi_u32 data_in, hi_u32 bit)
{
    hi_s32 data_out = 0; /* 0 algorithm data */
    hi_u32 data_in_tmp;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (data_in >> (bit - 1)) {
        data_in_tmp = data_in & ((1 << (bit - 1)) - 1);
        data_out = (((~data_in_tmp) & ((1 << (bit - 1)) - 1)) + 1) * (-1);
    } else {
        data_out = (hi_s32)data_in;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return data_out;
}

static hwc_node *GFX2D_HAL_HWC_DuplicateNodes(hwc_node *pstNode, hi_u32 u32NodeNum)
{
    hi_u32 i = 0; /* 0 algorithm data */
    hwc_node *pstCurNode = HI_NULL;
    hwc_node *pstNodeHead = HI_NULL;
    hwc_node *pstTmpNode = HI_NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstNode, NULL);

    for (i = 0; i < u32NodeNum; i++) {
        pstCurNode = (hwc_node *)GFX2D_MEM_Alloc(sizeof(hwc_node));
        if (pstCurNode == NULL) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_MEM_Alloc, FAILURE_TAG);
            goto ERR;
        }

        if (memcpy_s(pstCurNode, sizeof(hwc_node), pstNode, sizeof(hwc_node)) != EOK) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "call memcpy_s failed");
            goto ERR;
        }

        pstCurNode->pNextNode = pstNodeHead;
        // pstCurNode->cbm_cfg_addr_high.bits.cbm_cfg_addr_high =
        //      HI_GFX_GET_HIGH_PART(GFX2D_MEM_GetPhyaddr(pstCurNode) + 0x0800);
        //   pstCurNode->cbm_cfg_addr_low.bits.cbm_cfg_addr_low =
        //        HI_GFX_GET_LOW_PART(GFX2D_MEM_GetPhyaddr(pstCurNode) + 0x0800);
        hwc_hal_set_cfg_reg_offset(pstCurNode);
        if (pstNodeHead != NULL) {
            pstCurNode->next_node_phyaddr = HI_GFX_GET_LOW_PART(GFX2D_MEM_GetPhyaddr(pstNodeHead));
            pstCurNode->tde_pnext_hi.bits.p_next_hi = HI_GFX_GET_HIGH_PART(GFX2D_MEM_GetPhyaddr(pstNodeHead));
        } else {
            pstCurNode->next_node_phyaddr = 0;           /* 0 algorithm data */
            pstCurNode->tde_pnext_hi.bits.p_next_hi = 0; /* 0 algorithm data */
        }
        pstNodeHead = pstCurNode;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return pstNodeHead;

ERR:
    pstCurNode = pstNodeHead;
    while (pstCurNode) {
        pstTmpNode = pstCurNode->pNextNode;
        GFX2D_MEM_Free(pstCurNode);
        pstCurNode = pstTmpNode;
    }

    return HI_NULL;
}

#define GFX2D_ZME_TAP 8

typedef struct {
    hi_u8 h_scan_ord;
    hi_u8 fmt;
    hi_u32 width;
    hi_u32 hoffset_pix;
    hi_u32 v_pro;

    hi_bool hlmsc_en;
    hi_bool hchmsc_en;
    hi_u32 hratio;
    hi_s32 hor_loffset;
    hi_s32 hor_coffset;

    hi_u32 zme_ow;
    hi_u32 zme_iw;

    hi_bool hpzme_en;
    hi_u8 hpzme_mode;
    hi_u32 hpzme_width;

    hi_u32 xfpos;
    hi_u32 xdpos;

    hi_u32 xst_pos_cord;
    hi_u32 xed_pos_cord;

    hi_u32 hor_loffset_cfg_int;
    hi_u32 hor_loffset_cfg_fraction;
    hi_u32 hor_loffset_pix_fraction;
    hi_u32 hor_loffset_fraction;
    hi_s32 xst_pos_cord_in_offset;
    hi_s32 xed_pos_cord_in_offset;
    hi_u32 xst_pos_cord_in_tap_rgb;
    hi_u32 xed_pos_cord_in_tap_rgb;
    hi_u32 node_cfg_zme_iw_rgb;
    hi_u32 node_cfg_hor_loffset_rgb;

    hi_u32 xst_pos_cord_in_tap_luma;
    hi_u32 xed_pos_cord_in_tap_luma;
    hi_u32 xst_pos_cord_in_chroma;
    hi_u32 xed_pos_cord_in_chroma;
    hi_u32 hor_coffset_cfg_int;
    hi_u32 hor_coffset_cfg_fraction;
    hi_u32 hor_coffset_pix_fraction;
    hi_u32 hor_coffset_fraction;
    hi_s32 xst_pos_cord_in_offset_chroma;
    hi_s32 xed_pos_cord_in_offset_chroma;
    hi_u32 xst_pos_cord_in_tap_chroma;
    hi_u32 xed_pos_cord_in_tap_chroma;
    hi_u32 xst_pos_cord_in_tap_sp;
    hi_u32 xed_pos_cord_in_tap_sp;
    hi_u32 node_cfg_zme_iw_sp;
    hi_u32 node_cfg_hor_loffset_sp;
    hi_u32 node_cfg_hor_coffset_sp;

    hi_u32 u32InWidth;
    hi_u32 u32ZmeInWidth;
    hi_u32 u32ZmeOutWidth;
    hi_u32 u32HpZmeWidth;

    hi_u32 xst_pos_cord_in_tap;
    hi_u32 xed_pos_cord_in_tap;

    hi_u32 xst_pos_cord_in_tap_hso;
    hi_u32 xed_pos_cord_in_tap_hso;

    hi_u32 hor_loffset_int_beyond_complent;
} block_config;

#define SRC_TWO 2
#define SRC_THREE 3

static hi_void GFX2D_HAl_HWC_GetNodeConfig(hwc_node *pstNode, hi_u8 u8LayerId, block_config *pstConfig)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstNode);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstConfig);
    if (u8LayerId < SRC_TWO) {
        pstConfig->h_scan_ord = pstNode->src_Layer[u8LayerId].unCtrl.stBits.bit1HScanOrd;
        pstConfig->fmt = pstNode->src_Layer[u8LayerId].unCtrl.stBits.bit6Fmt;
        pstConfig->width = pstNode->src_Layer[u8LayerId].unRes.stBits.bit16Width + 1;
        pstConfig->hoffset_pix = pstNode->src_Layer[u8LayerId].u32PixOffset;
        pstConfig->v_pro = pstNode->src_Layer[u8LayerId].unCtrl.stBits.bit1422Pro;
    } else {
        pstConfig->h_scan_ord = pstNode->astLayer[u8LayerId - SRC_TWO].unCtrl.stBits.bit1HScanOrd;
        pstConfig->fmt = pstNode->astLayer[u8LayerId - SRC_TWO].unCtrl.stBits.bit6Fmt;
        pstConfig->width = pstNode->astLayer[u8LayerId - SRC_TWO].unRes.stBits.bit16Width + 1;
        pstConfig->hoffset_pix = pstNode->astLayer[u8LayerId - SRC_TWO].u32PixOffset;
        pstConfig->v_pro = pstNode->astLayer[u8LayerId - SRC_TWO].unCtrl.stBits.bit1422Pro;
    }
    if (u8LayerId < SRC_THREE) {
        pstConfig->hlmsc_en = pstNode->src_zme[u8LayerId].src1_hsp.bits.src1_hlmsc_en;
        pstConfig->hchmsc_en = pstNode->src_zme[u8LayerId].src1_hsp.bits.src1_hchmsc_en;
        pstConfig->hratio = pstNode->src_zme[u8LayerId].src1_hsp.bits.src1_hratio;
        pstConfig->hor_loffset = pstNode->src_zme[u8LayerId].src1_hloffset.bits.src1_hor_loffset;
        pstConfig->hor_coffset = pstNode->src_zme[u8LayerId].src1_hcoffset.bits.src1_hor_coffset;

        pstConfig->zme_ow = pstNode->src_zme[u8LayerId].src1_zme_out_reso.bits.src1_ow + 1;
        pstConfig->zme_iw = pstNode->src_zme[u8LayerId].src1_zme_in_reso.bits.src1_iw + 1;

        pstConfig->hpzme_en = pstNode->src_hpzme[u8LayerId].bits.src1_hpzme_en;
        pstConfig->hpzme_mode = pstNode->src_hpzme[u8LayerId].bits.src1_hpzme_mode;
        pstConfig->hpzme_width = pstNode->src_hpzme[u8LayerId].bits.src1_hpzme_width;
    } else {
        pstConfig->hlmsc_en = HI_FALSE;
        pstConfig->hchmsc_en = HI_FALSE;
        pstConfig->hratio = 0;      /* 0 algorithm data */
        pstConfig->hor_loffset = 0; /* 0 algorithm data */
        pstConfig->hor_coffset = 0; /* 0 algorithm data */
        pstConfig->zme_ow = pstNode->astLayer[u8LayerId - SRC_TWO].unRes.stBits.bit16Width + 1;
        pstConfig->zme_iw = pstNode->astLayer[u8LayerId - SRC_TWO].unRes.stBits.bit16Width + 1;
        pstConfig->hpzme_en = HI_FALSE;
        pstConfig->hpzme_mode = 0; /* 0 algorithm data */
        pstConfig->hpzme_width = pstNode->astLayer[u8LayerId - SRC_TWO].unRes.stBits.bit16Width + 1;
    }
    pstConfig->xfpos = pstNode->cbm_info[u8LayerId].unCbmStartPos.stBits.bit16X;
    pstConfig->xdpos = ((hi_u32)pstConfig->hlmsc_en | (hi_u32)pstConfig->hchmsc_en)
                            ? (pstConfig->xfpos + pstConfig->zme_ow - 1)
                            : (pstConfig->hpzme_en ? (pstConfig->xfpos + pstConfig->hpzme_width - 1)
                                                    : (pstConfig->xfpos + pstConfig->width - 1));
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void calc_sp_split_cfg(block_config *pstConfig)
{
    pstConfig->xst_pos_cord_in_tap_luma = ((pstConfig->xst_pos_cord_in_tap_rgb % 2) == 1)
                                                ? /* 2,1 algorithm data */
                                              pstConfig->xst_pos_cord_in_tap_rgb - 1
                                                : pstConfig->xst_pos_cord_in_tap_rgb;

    pstConfig->xed_pos_cord_in_tap_luma = ((pstConfig->xed_pos_cord_in_tap_rgb % 2) == 0)
                                                ? /* 2,0 algorithm data */
                                              pstConfig->xed_pos_cord_in_tap_rgb + 1
                                                : pstConfig->xed_pos_cord_in_tap_rgb;

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->xst_pos_cord_in_tap_luma);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->xed_pos_cord_in_tap_luma);

    pstConfig->xst_pos_cord_in_chroma = (pstConfig->fmt == GFX2D_HAL_SRCFMT_SP422V)
                                            ? ((pstConfig->xst_pos_cord * pstConfig->hratio) >> GFX2D_HAL_RATIO_PRECISE)
                                            : ((pstConfig->xst_pos_cord * (pstConfig->hratio / 2)) >>
                                               GFX2D_HAL_RATIO_PRECISE); /* 2 algorithm data */

    pstConfig->xed_pos_cord_in_chroma = (pstConfig->fmt == GFX2D_HAL_SRCFMT_SP422V)
                                            ? ((pstConfig->xed_pos_cord * pstConfig->hratio) >> GFX2D_HAL_RATIO_PRECISE)
                                            : ((pstConfig->xed_pos_cord * (pstConfig->hratio / 2)) >>
                                               GFX2D_HAL_RATIO_PRECISE); /* 2, algorithm data */

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->xst_pos_cord_in_chroma);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->xed_pos_cord_in_chroma);

    pstConfig->hor_coffset_cfg_int =
        GFX2D_HAL_HWC_ComplementToTrueValue((((hi_u32)pstConfig->hor_coffset) >> GFX2D_HAL_RATIO_PRECISE),
                                            8); /* 8 algorithm data */

    pstConfig->hor_coffset_cfg_fraction = (hi_u32)(pstConfig->hor_coffset) & 0xfffff;

    pstConfig->hor_coffset_pix_fraction = (pstConfig->fmt == GFX2D_HAL_SRCFMT_SP422V)
                                                ? ((pstConfig->xst_pos_cord * pstConfig->hratio) & 0xfffff)
                                                : ((pstConfig->xst_pos_cord * (pstConfig->hratio / 2)) &
                                                 0xfffff); /* 2 algorithm data */

    pstConfig->hor_coffset_fraction = (pstConfig->hor_coffset_cfg_fraction + pstConfig->hor_coffset_pix_fraction) &
                                      0xfffff;

    pstConfig->xst_pos_cord_in_offset_chroma =
        pstConfig->xst_pos_cord_in_chroma + pstConfig->hor_coffset_cfg_int +
        (((pstConfig->hor_coffset_cfg_fraction + pstConfig->hor_coffset_pix_fraction) & 0xfff00000) != 0);

    pstConfig->xed_pos_cord_in_offset_chroma =
        pstConfig->xed_pos_cord_in_chroma + pstConfig->hor_coffset_cfg_int +
        (((pstConfig->hor_coffset_cfg_fraction + pstConfig->hor_coffset_pix_fraction) & 0xfff00000) != 0);

    pstConfig->xst_pos_cord_in_tap_chroma =
        (pstConfig->xst_pos_cord_in_offset_chroma < 0)
            ? 0
            : ((pstConfig->xst_pos_cord_in_offset_chroma >= (GFX2D_ZME_TAP / 2 - 1)) ? /* 2,1 algorithm data */
                   (pstConfig->xst_pos_cord_in_offset_chroma - (GFX2D_ZME_TAP / 2 - 1))
                                                                                        : 0); /* 2,1 algorithm data */
    return;
}

static hi_void GFX2D_HAl_HWC_CalcSpZme(hi_u8 u8LayerId, block_config *pstConfig, hwc_node *pstSplitNode)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstConfig);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstSplitNode);
    calc_sp_split_cfg(pstConfig);
    pstConfig->xed_pos_cord_in_tap_chroma =
        (pstConfig->xed_pos_cord_in_offset_chroma + GFX2D_ZME_TAP / 2) < 0 ? 0 :         /* 2 algorithm data */
            (((pstConfig->xed_pos_cord_in_offset_chroma + GFX2D_ZME_TAP / 2) >=          /* 2 algorithm data */
              ((pstConfig->fmt == GFX2D_HAL_SRCFMT_SP422V) ? ((pstConfig->zme_iw - 1)) : /* 1 algorithm data */
                   ((pstConfig->zme_iw - 1) / 2)))
                    ? ((pstConfig->fmt == GFX2D_HAL_SRCFMT_SP422V) ? /* 2,1 algorithm data */
                        ((pstConfig->zme_iw - 1))
                                                                : ((pstConfig->zme_iw - 1) / 2))
                    :                                                                /* 2,1 algorithm data */
                 (pstConfig->xed_pos_cord_in_offset_chroma + GFX2D_ZME_TAP / 2)); /* 2,1 algorithm data */

    pstConfig->xst_pos_cord_in_tap_chroma = (pstConfig->fmt == GFX2D_HAL_SRCFMT_SP422V)
                                                ? pstConfig->xst_pos_cord_in_tap_chroma
                                                : pstConfig->xst_pos_cord_in_tap_chroma * 2; /* 2,1 algorithm data */

    pstConfig->xed_pos_cord_in_tap_chroma = (pstConfig->fmt == GFX2D_HAL_SRCFMT_SP422V)
                                                ? pstConfig->xed_pos_cord_in_tap_chroma
                                                : (pstConfig->xed_pos_cord_in_tap_chroma * 2 + 1); /* 2,1 algorithm data */
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->xst_pos_cord_in_tap_chroma);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->xed_pos_cord_in_tap_chroma);

    pstConfig->xst_pos_cord_in_tap_sp = GFX2D_HAL_HWC_MIN(pstConfig->xst_pos_cord_in_tap_luma,
                                                          pstConfig->xst_pos_cord_in_tap_chroma);
    pstConfig->xed_pos_cord_in_tap_sp = GFX2D_HAL_HWC_MAX(pstConfig->xed_pos_cord_in_tap_luma,
                                                          pstConfig->xed_pos_cord_in_tap_chroma);

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->xst_pos_cord_in_tap_sp);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->xed_pos_cord_in_tap_sp);

    pstConfig->node_cfg_zme_iw_sp = pstConfig->xed_pos_cord_in_tap_sp - pstConfig->xst_pos_cord_in_tap_sp + 1;

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->node_cfg_zme_iw_sp);

    pstConfig->node_cfg_hor_loffset_sp =
        (GFX2D_HAL_HWC_TrueValueToComplement((pstConfig->xst_pos_cord_in_offset - pstConfig->xst_pos_cord_in_tap_sp), 8)
         << GFX2D_HAL_RATIO_PRECISE) + /* 8 algorithm data */
        pstConfig->hor_loffset_fraction;

    pstConfig->node_cfg_hor_coffset_sp =
        (GFX2D_HAL_HWC_TrueValueToComplement((pstConfig->xst_pos_cord_in_offset_chroma -
                                              ((pstConfig->fmt == GFX2D_HAL_SRCFMT_SP422V)
                                                    ? pstConfig->xst_pos_cord_in_tap_sp
                                                    : pstConfig->xst_pos_cord_in_tap_sp / 2)),
                                             8)
         << /* 2,8 algorithm data */
         GFX2D_HAL_RATIO_PRECISE) +
        pstConfig->hor_coffset_fraction;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void calculate_split_node_cfg(block_config *pstConfig)
{
    pstConfig->hor_loffset_cfg_int =
        GFX2D_HAL_HWC_ComplementToTrueValue(((hi_u32)(pstConfig->hor_loffset) >> GFX2D_HAL_RATIO_PRECISE),
                                            8); /* 8 algorithm data */

    pstConfig->hor_loffset_cfg_fraction = (hi_u32)pstConfig->hor_loffset & 0xfffff;
    pstConfig->hor_loffset_pix_fraction = (pstConfig->xst_pos_cord * pstConfig->hratio) & 0xfffff;
    pstConfig->hor_loffset_fraction = (pstConfig->hor_loffset_cfg_fraction + pstConfig->hor_loffset_pix_fraction) &
                                      0xfffff;

    pstConfig->xst_pos_cord_in_offset =
        ((pstConfig->xst_pos_cord * pstConfig->hratio) >> GFX2D_HAL_RATIO_PRECISE) + pstConfig->hor_loffset_cfg_int +
        (((pstConfig->hor_loffset_cfg_fraction + pstConfig->hor_loffset_pix_fraction) & 0xfff00000) != 0);

    pstConfig->xed_pos_cord_in_offset =
        ((pstConfig->xed_pos_cord * pstConfig->hratio) >> GFX2D_HAL_RATIO_PRECISE) + pstConfig->hor_loffset_cfg_int +
        (((pstConfig->hor_loffset_cfg_fraction + pstConfig->hor_loffset_pix_fraction) & 0xfff00000) != 0);

    pstConfig->xst_pos_cord_in_tap_rgb = (pstConfig->xst_pos_cord_in_offset < 0)
                                                ? 0
                                                : ((pstConfig->xst_pos_cord_in_offset >= (GFX2D_ZME_TAP / 2 - 1))
                                                    ? /* 2,1 algorithm data */
                                                    (pstConfig->xst_pos_cord_in_offset - (GFX2D_ZME_TAP / 2 - 1))
                                                    : 0); /* 2,1 algorithm data */

    pstConfig->xed_pos_cord_in_tap_rgb =
        (pstConfig->xed_pos_cord_in_offset + GFX2D_ZME_TAP / 2) < 0 ? 0 : /* 2 algorithm data */
            (((pstConfig->xed_pos_cord_in_offset + GFX2D_ZME_TAP / 2) >=  /* 2,1 algorithm data */
              pstConfig->zme_iw - 1)
                    ? pstConfig->zme_iw - 1
                    :                                                         /* 1 algorithm data */
                 (pstConfig->xed_pos_cord_in_offset + GFX2D_ZME_TAP / 2)); /* 2 algorithm data */

    pstConfig->node_cfg_zme_iw_rgb = pstConfig->xed_pos_cord_in_tap_rgb - pstConfig->xst_pos_cord_in_tap_rgb + 1;

    pstConfig->node_cfg_hor_loffset_rgb =
        (GFX2D_HAL_HWC_TrueValueToComplement((pstConfig->xst_pos_cord_in_offset - pstConfig->xst_pos_cord_in_tap_rgb), 8)
         << /* 8 algorithm data */
         GFX2D_HAL_RATIO_PRECISE) +
        pstConfig->hor_loffset_fraction;
    return;
}

static hi_void GFX2D_HAL_HWC_CalcZme(hi_u8 u8LayerId, block_config *pstConfig, hwc_node *pstSplitNode)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstConfig);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstSplitNode);

    calculate_split_node_cfg(pstConfig);

    /* SP */
    if (pstConfig->fmt >= GFX2D_HAL_SRCFMT_PKYUYV) {
        GFX2D_HAl_HWC_CalcSpZme(u8LayerId, pstConfig, pstSplitNode);
    }
    pstConfig->u32ZmeOutWidth = pstConfig->xed_pos_cord - pstConfig->xst_pos_cord + 1; /* 1 algorithm data */

    pstConfig->u32ZmeInWidth =
        (((pstConfig->fmt >= GFX2D_HAL_SRCFMT_PKYUYV) && (pstConfig->fmt != GFX2D_HAL_SRCFMT_SP400) &&
          (!((pstConfig->fmt == GFX2D_HAL_SRCFMT_SP422V) && !pstConfig->v_pro)) &&
          (pstConfig->fmt != GFX2D_HAL_SRCFMT_SP444))
                ? pstConfig->node_cfg_zme_iw_sp
                : ((pstConfig->hlmsc_en || pstConfig->hchmsc_en) ? pstConfig->node_cfg_zme_iw_rgb
                                                                : pstConfig->u32ZmeOutWidth));

    pstSplitNode->src_zme[u8LayerId].src1_hloffset.bits.src1_hor_loffset =
        (((pstConfig->fmt >= GFX2D_HAL_SRCFMT_PKYUYV) && (pstConfig->fmt != GFX2D_HAL_SRCFMT_SP400) &&
          (!((pstConfig->fmt == GFX2D_HAL_SRCFMT_SP422V) && !pstConfig->v_pro)) &&
          (pstConfig->fmt != GFX2D_HAL_SRCFMT_SP444))
                ? pstConfig->node_cfg_hor_loffset_sp
                : pstConfig->node_cfg_hor_loffset_rgb);

    pstSplitNode->src_zme[u8LayerId].src1_hcoffset.bits.src1_hor_coffset =
        (((pstConfig->fmt >= GFX2D_HAL_SRCFMT_PKYUYV) && (pstConfig->fmt != GFX2D_HAL_SRCFMT_SP400) &&
          (!((pstConfig->fmt == GFX2D_HAL_SRCFMT_SP422V) && !pstConfig->v_pro)) &&
          (pstConfig->fmt != GFX2D_HAL_SRCFMT_SP444))
                ? pstConfig->node_cfg_hor_coffset_sp
                : pstConfig->node_cfg_hor_loffset_rgb);

    pstConfig->u32HpZmeWidth = pstConfig->u32ZmeInWidth;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void GFX2D_HAL_HWC_EvenWidth(hi_u8 u8LayerId, block_config *pstConfig, hwc_node *pstSplitNode)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstConfig);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstSplitNode);

    if (pstConfig->h_scan_ord) {
        if (u8LayerId < SRC_THREE) {
            pstConfig->u32ZmeInWidth += 1;
        }
    } else {
        if (u8LayerId < SRC_THREE) {
            pstConfig->u32ZmeInWidth += 1;
            pstSplitNode->src_zme[u8LayerId].src1_hloffset.bits.src1_hor_loffset +=
                (pstConfig->hor_loffset_int_beyond_complent << GFX2D_HAL_RATIO_PRECISE);
            pstSplitNode->src_zme[u8LayerId].src1_hcoffset.bits.src1_hor_coffset +=
                (pstConfig->hor_loffset_int_beyond_complent << GFX2D_HAL_RATIO_PRECISE);
        }
    }
    pstConfig->u32InWidth += 1;
    if (u8LayerId < SRC_TWO) {
        pstSplitNode->src_Layer[u8LayerId].u32PixOffset -= 1;
    } else {
        pstSplitNode->astLayer[u8LayerId - SRC_TWO].u32PixOffset -= 1;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_bool CheckEvenWidth(block_config *pstConfig)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstConfig, HI_FALSE);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return ((pstConfig->hpzme_en == 0) && (pstConfig->u32InWidth % 2 == 1) /* 2,1 algorithm data */
            && ((pstConfig->fmt == GFX2D_HAL_SRCFMT_SP400) ||
                ((pstConfig->fmt == GFX2D_HAL_SRCFMT_SP422V) && !pstConfig->v_pro) ||
                (pstConfig->fmt == GFX2D_HAL_SRCFMT_SP444)) &&
            (pstConfig->xed_pos_cord_in_tap_hso == pstConfig->u32InWidth - 1) &&
            (pstConfig->u32InWidth % 2 == 0)); /* 2 algorithm data */
}

static hi_void GFX2D_HAL_HWC_CalcXStartPos(block_config *pstConfig)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstConfig);

    if (pstConfig->hlmsc_en || pstConfig->hchmsc_en) {
        if ((pstConfig->fmt >= GFX2D_HAL_SRCFMT_PKYUYV) && (pstConfig->fmt != GFX2D_HAL_SRCFMT_SP400) &&
            (!((pstConfig->fmt == GFX2D_HAL_SRCFMT_SP422V) && !pstConfig->v_pro)) &&
            (pstConfig->fmt != GFX2D_HAL_SRCFMT_SP444)) {
            pstConfig->xst_pos_cord_in_tap = pstConfig->xst_pos_cord_in_tap_sp;
        } else {
            pstConfig->xst_pos_cord_in_tap = pstConfig->xst_pos_cord_in_tap_rgb;
        }
    } else {
        pstConfig->xst_pos_cord_in_tap = pstConfig->xst_pos_cord;
    }

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->xst_pos_cord_in_tap);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->xst_pos_cord);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->xst_pos_cord_in_tap_sp);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->xst_pos_cord_in_tap_rgb);

    pstConfig->xst_pos_cord_in_tap = pstConfig->hpzme_en
                                            ? (pstConfig->xst_pos_cord_in_tap * (pstConfig->hpzme_mode + 1))
                                            : pstConfig->xst_pos_cord_in_tap;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void GFX2D_HAL_HWC_CalcXEndPos(block_config *pstConfig)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstConfig);

    if (pstConfig->hlmsc_en || pstConfig->hchmsc_en) {
        if ((pstConfig->fmt >= GFX2D_HAL_SRCFMT_PKYUYV) && (pstConfig->fmt != GFX2D_HAL_SRCFMT_SP400) &&
            (!((pstConfig->fmt == GFX2D_HAL_SRCFMT_SP422V) && !pstConfig->v_pro)) &&
            (pstConfig->fmt != GFX2D_HAL_SRCFMT_SP444)) {
            pstConfig->xed_pos_cord_in_tap = pstConfig->xed_pos_cord_in_tap_sp;
        } else {
            pstConfig->xed_pos_cord_in_tap = pstConfig->xed_pos_cord_in_tap_rgb;
        }
    } else {
        pstConfig->xed_pos_cord_in_tap = pstConfig->xed_pos_cord;
    }

    pstConfig->xed_pos_cord_in_tap = pstConfig->hpzme_en
                                            ? ((pstConfig->xed_pos_cord_in_tap + 1) * (pstConfig->hpzme_mode + 1) - 1)
                                            : pstConfig->xed_pos_cord_in_tap;

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->xst_pos_cord_in_tap);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstConfig->hpzme_en);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void calc_zme_pix_offset(hi_u8 u8LayerId, block_config *config, hwc_node *pstSplitNode)
{
    if (config->hpzme_en) {
        if (u8LayerId < SRC_THREE) {
            config->u32InWidth = config->u32HpZmeWidth * (config->hpzme_mode + 1);
        }
        if (u8LayerId < SRC_TWO) {
            pstSplitNode->src_Layer[u8LayerId].u32PixOffset =
                ((config->h_scan_ord ? config->xst_pos_cord_in_tap_hso : config->xst_pos_cord_in_tap) *
                 (config->hpzme_mode + 1)) +
                config->hoffset_pix;
        } else {
            pstSplitNode->astLayer[u8LayerId - SRC_TWO].u32PixOffset =
                ((config->h_scan_ord ? config->xst_pos_cord_in_tap_hso : config->xst_pos_cord_in_tap) *
                 (config->hpzme_mode + 1)) +
                config->hoffset_pix;
        }
    } else {
        if (u8LayerId < SRC_THREE) {
            config->u32InWidth = config->u32HpZmeWidth;
        }
        if (u8LayerId < SRC_TWO) {
            pstSplitNode->src_Layer[u8LayerId].u32PixOffset = (config->h_scan_ord ? config->xst_pos_cord_in_tap_hso
                                                                                    : config->xst_pos_cord_in_tap) +
                                                              config->hoffset_pix;
        } else {
            pstSplitNode->astLayer[u8LayerId - SRC_TWO].u32PixOffset =
                (config->h_scan_ord ? config->xst_pos_cord_in_tap_hso : config->xst_pos_cord_in_tap) +
                config->hoffset_pix;
        }
    }
    return;
}

static hi_void GFX2D_HAL_HWC_CalcResolution(hi_u8 u8LayerId, block_config *pstConfig, hwc_node *pstSplitNode)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstConfig);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstSplitNode);

    GFX2D_HAL_HWC_CalcXStartPos(pstConfig);
    GFX2D_HAL_HWC_CalcXEndPos(pstConfig);

    if (pstConfig->h_scan_ord) {
        pstConfig->xst_pos_cord_in_tap_hso = pstConfig->width - 1 - pstConfig->xed_pos_cord_in_tap;
        pstConfig->xed_pos_cord_in_tap_hso = pstConfig->width - 1 - pstConfig->xst_pos_cord_in_tap;
    } else {
        pstConfig->xst_pos_cord_in_tap_hso = pstConfig->xst_pos_cord_in_tap;
        pstConfig->xed_pos_cord_in_tap_hso = pstConfig->xed_pos_cord_in_tap;
    }

    pstConfig->u32InWidth = (pstConfig->xed_pos_cord_in_tap_hso - pstConfig->xst_pos_cord_in_tap_hso + 1);

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(u8LayerId, 10); /* 10 layer */
    if (u8LayerId < SRC_TWO) {
        pstSplitNode->src_Layer[u8LayerId].u32PixOffset = pstConfig->xst_pos_cord_in_tap_hso;
    } else {
        pstSplitNode->astLayer[u8LayerId - SRC_TWO].u32PixOffset = pstConfig->xst_pos_cord_in_tap_hso;
    }

    pstConfig->hor_loffset_int_beyond_complent = GFX2D_HAL_HWC_TrueValueToComplement(1, 8); /* 8,1 algorithm data */

    if (CheckEvenWidth(pstConfig)) {
        GFX2D_HAL_HWC_EvenWidth(u8LayerId, pstConfig, pstSplitNode);
    }

    calc_zme_pix_offset(u8LayerId, pstConfig, pstSplitNode);
    pstSplitNode->src_zme[u8LayerId].src1_zme_in_reso.bits.src1_iw = pstConfig->u32ZmeInWidth - 1;
    pstSplitNode->src_zme[u8LayerId].src1_zme_out_reso.bits.src1_ow = pstConfig->u32ZmeOutWidth - 1;
    pstSplitNode->src_hpzme[u8LayerId].bits.src1_hpzme_width = pstConfig->u32HpZmeWidth - 1;
    if (u8LayerId < SRC_TWO) {
        pstSplitNode->src_Layer[u8LayerId].unRes.stBits.bit16Width = pstConfig->u32InWidth - 1;
    } else {
        pstSplitNode->astLayer[u8LayerId - SRC_TWO].unRes.stBits.bit16Width = pstConfig->u32InWidth - 1;
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void disable_layer_cbm(hi_u8 layer, hwc_node *split_node)
{
    switch (layer) {
        case 0: /* layer 0 */
            split_node->cbm_ctrl.bits.cbm_src1_en = 0;
            break;
        case 1: /* layer 1 */
            split_node->cbm_ctrl.bits.cbm_src2_en = 0;
            break;
        case 2: /* layer 2 */
            split_node->cbm_ctrl.bits.cbm_src3_en = 0;
            break;
        case 3: /* layer 3 */
            split_node->cbm_ctrl.bits.cbm_src4_en = 0;
            break;
        case 4: /* layer 4 */
            split_node->cbm_ctrl.bits.cbm_src5_en = 0;
            break;
        case 5: /* layer 5 */
            split_node->cbm_ctrl.bits.cbm_src6_en = 0;
            break;
        case 6: /* layer 6 */
            split_node->cbm_ctrl.bits.cbm_src7_en = 0;
            break;
        case 7: /* layer 7 */
            split_node->cbm_ctrl.bits.cbm_src8_en = 0;
            break;
        case 8: /* layer 8 */
            split_node->cbm_ctrl.bits.cbm_src9_en = 0;
            break;
        case 9: /* layer 9 */
            split_node->cbm_ctrl.bits.cbm_src10_en = 0;
            break;
    }
    return;
}

static hi_void GFX2D_HAL_HWC_ConfigSplitNodeSrc(hwc_node *pstNode, hwc_node *split_node, hi_u32 u32DstStartPos,
                                                hi_u32 u32DstEndPos)
{
    hi_u8 layer;
    hi_u32 enable;
    block_config stBlockConfig = {0};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstNode);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(split_node);

    for (layer = 0; layer < GFX2D_ARRAY_SIZE(pstNode->astLayer) + GFX2D_ARRAY_SIZE(pstNode->src_Layer); layer++) {
        if (layer < SRC_TWO) {
            enable = pstNode->src_Layer[layer].unCtrl.stBits.bit1IsEnable;
        } else {
            enable = pstNode->astLayer[layer - SRC_TWO].unCtrl.stBits.bit1IsEnable;
        }
        if (!enable) {
            continue;
        }
        GFX2D_HAl_HWC_GetNodeConfig(pstNode, layer, &stBlockConfig);

        /* setp2 : judge src is or not in picture and if in calc src blk pos in big picture */
        if ((stBlockConfig.xdpos < u32DstStartPos) || (stBlockConfig.xfpos > u32DstEndPos)) {
            if (layer < SRC_TWO) {
                split_node->src_Layer[layer].unCtrl.stBits.bit1IsEnable = 0x0;
            } else {
                split_node->astLayer[layer - SRC_TWO].unCtrl.stBits.bit1IsEnable = 0x0;
            }
            disable_layer_cbm(layer, split_node);
            continue;
        }

        /* setp3 : out pos relative to disp start */
        stBlockConfig.xst_pos_cord = GFX2D_HAL_HWC_MAX(u32DstStartPos, stBlockConfig.xfpos) - stBlockConfig.xfpos;
        stBlockConfig.xed_pos_cord = GFX2D_HAL_HWC_MIN(u32DstEndPos, stBlockConfig.xdpos) - stBlockConfig.xfpos;
        split_node->cbm_info[layer].unCbmStartPos.stBits.bit16X =
            GFX2D_HAL_HWC_MAX(u32DstStartPos, stBlockConfig.xfpos) - u32DstStartPos;

        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, u32DstStartPos);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stBlockConfig.xfpos);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, u32DstEndPos);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, stBlockConfig.xdpos);
        /* setp4 : calc src zme parameter */
        GFX2D_HAL_HWC_CalcZme(layer, &stBlockConfig, split_node);
        GFX2D_HAL_HWC_CalcResolution(layer, &stBlockConfig, split_node);
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_s32 GFX2D_HAL_HWC_SplitNode(hwc_node *pstNode)
{
    hi_bool dst_en = pstNode->dst_ctrl.bits.dst_en;
    hi_u8 dst_h_scan_ord = pstNode->dst_ctrl.bits.dst_h_scan_ord;
    hi_bool dst_crop_en = 0;
    hi_u32 dst_crop_mode = pstNode->dst_alpha.bits.dst_clip_mode;
    hi_u32 dst_width = pstNode->dst_image_size.bits.dst_width + 1;
    hi_u32 dst_crop_start_x = pstNode->unOutCropStart.stBits.bit16X;
    hi_u32 dst_crop_end_x = pstNode->unOutCropEnd.stBits.bit16X;
    hi_u32 dst_hoffset_pix = pstNode->u32OutPixOffset;
    hi_u32 node_num, i, dst_xst_pos_blk, dst_xed_pos_blk;
    hwc_node *pstNodeHead = NULL;
    hwc_node *pstCurNode = pstNode;
    node_num = dst_width / GFX2D_HAL_SLICE_WIDTH + ((dst_width % GFX2D_HAL_SLICE_WIDTH) != 0);
    if (node_num == 0 || node_num >= GFX2D_CONFIG_GetNodeNum()) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Split node_num is 0 or bigger than NodeNum(60)");
        return HI_SUCCESS;
    }

    pstNodeHead = GFX2D_HAL_HWC_DuplicateNodes(pstNode, node_num);
    if (pstNodeHead == HI_NULL) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_HAL_HWC_DuplicateNodes, FAILURE_TAG);
        return DRV_GFX2D_ERR_NO_MEM;
    }

    pstCurNode = pstNodeHead;

    for (i = 0; i < node_num; i++) {
        /* setp1 : calc blk pos */
        dst_xst_pos_blk = i * GFX2D_HAL_SLICE_WIDTH;
        dst_xed_pos_blk = (((i + 1) * GFX2D_HAL_SLICE_WIDTH) <= dst_width) ? (((i + 1) * GFX2D_HAL_SLICE_WIDTH) - 1)
                                                                            : (dst_width - 1);

        pstCurNode->cbm_imgsize.bits.cbm_width = dst_xed_pos_blk - dst_xst_pos_blk;

        GFX2D_HAL_HWC_ConfigSplitNodeSrc(pstNode, pstCurNode, dst_xst_pos_blk, dst_xed_pos_blk);

        /* ...................................des................................................................ */
        pstCurNode->dst_ctrl.bits.dst_en = dst_crop_en
                                                ? (((dst_crop_mode == 0) && ((dst_crop_start_x > dst_xed_pos_blk) ||
                                                                            (dst_crop_end_x < dst_xst_pos_blk)))
                                                        ? 0
                                                        : dst_en)
                                                : dst_en;
        pstCurNode->dst_image_size.bits.dst_width = dst_xed_pos_blk - dst_xst_pos_blk + 1 - 1;

        pstCurNode->u32OutPixOffset = (dst_h_scan_ord ? (dst_width - 1 - dst_xed_pos_blk) : dst_xst_pos_blk) +
                                      dst_hoffset_pix;
        pstCurNode->dst_alpha.bits.dst_clip_en =
            (!((dst_crop_start_x > dst_xed_pos_blk) || (dst_crop_end_x < dst_xst_pos_blk))) && dst_crop_en;
        pstCurNode->unOutCropStart.stBits.bit16X =
            pstCurNode->dst_alpha.bits.dst_clip_en
                ? (GFX2D_HAL_HWC_MAX(dst_xst_pos_blk, dst_crop_start_x) - dst_xst_pos_blk)
                : 0;
        pstCurNode->unOutCropEnd.stBits.bit16X =
            pstCurNode->dst_alpha.bits.dst_clip_en
                ? (GFX2D_HAL_HWC_MIN(dst_xed_pos_blk, dst_crop_end_x) - dst_xst_pos_blk)
                : 0;

        pstCurNode = pstCurNode->pNextNode;
    }
    memcpy(pstNode, pstNodeHead, sizeof(hwc_node));
    hwc_hal_set_cfg_reg_offset(pstNode);
    GFX2D_MEM_Free(pstNodeHead);
    return HI_SUCCESS;
}

typedef struct {
    hi_u8 u8NodeCnt;
    hi_u8 u8ZmeLayerCnt;
    hi_u8 u8LayerCnt;
    hi_u8 u8StartLayer;
} layer_config;

static hi_s32 set_new_list(drv_gfx2d_compose_list *compose_list, layer_config *cfg,
                           drv_gfx2d_compose_list *pstNewComposeList, drv_gfx2d_compose_surface *pstDstSurface)
{
    hi_s32 ret;
    drv_gfx2d_compose_msg *compose = NULL;
    compose = HI_GFX_KZALLOC(HIGFX_GFX2D_ID, sizeof(drv_gfx2d_compose_msg) * cfg->u8LayerCnt, GFP_KERNEL);
    if (compose == NULL) {
        ret = DRV_GFX2D_ERR_NO_MEM;
        return ret;
    }

    if (cfg->u8NodeCnt == 0) {
        memcpy(compose, compose_list->compose, sizeof(drv_gfx2d_compose_msg) * cfg->u8LayerCnt);
    } else {
        memcpy(&compose[0].compose_surface, pstDstSurface, sizeof(drv_gfx2d_compose_surface));
        compose[0].in_rect.width = pstDstSurface->width;
        compose[0].in_rect.height = pstDstSurface->height;
        compose[0].out_rect.width = pstDstSurface->width;
        compose[0].out_rect.height = pstDstSurface->height;
        memcpy(&compose[1], &(compose_list->compose[cfg->u8StartLayer]),
               sizeof(drv_gfx2d_compose_msg) * (cfg->u8LayerCnt - 1));
    }
    pstNewComposeList[cfg->u8NodeCnt].compose = compose;
    pstNewComposeList[cfg->u8NodeCnt].background_color = compose_list->background_color;
    pstNewComposeList[cfg->u8NodeCnt].compose_cnt = cfg->u8LayerCnt;
    return HI_SUCCESS;
}

/* 1. Dividing tasks into multiple small tasks supported by hardware
   2. For example, hardware supports one-level zooming, and users pass in two-level zooming,
   which will be decomposed into two tasks, do two operations, such as support 5 layers, users into 12 layers,
   it will be completed in three tasks step by step. */
static hi_u32 GFX2D_HAL_HWC_ConfigComposeList(drv_gfx2d_compose_list *compose_list,
                                              drv_gfx2d_compose_list *pstNewComposeList, hi_u32 new_compose_list_len,
                                              drv_gfx2d_compose_surface *pstDstSurface)
{
    hi_u32 i = 0;
    hi_s32 s32Ret = HI_SUCCESS;
    layer_config layer_cfg = {0};

    for (i = 0; i < compose_list->compose_cnt; i++) {
        if ((layer_cfg.u8NodeCnt + 1) > GFX2D_MAX_LAYERS) {
            s32Ret = DRV_GFX2D_ERR_UNSUPPORT;
            goto ERR;
        }

        if ((compose_list->compose[i].opt.resize.resize_en) &&
            (compose_list->compose[i].compose_surface.surface_type == DRV_GFX2D_SURFACE_TYPE_MEM)) {
            layer_cfg.u8ZmeLayerCnt++;
        }

        if (layer_cfg.u8ZmeLayerCnt <= g_gfx2d_capability.u8ZmeLayerNum) {
            layer_cfg.u8LayerCnt++;
        }

        if ((layer_cfg.u8ZmeLayerCnt <= g_gfx2d_capability.u8ZmeLayerNum) && (i != (compose_list->compose_cnt - 1))) {
            continue;
        }

        if (compose_list->compose == NULL) {
            s32Ret = DRV_GFX2D_ERR_NO_MEM;
            goto ERR;
        }

        s32Ret = set_new_list(compose_list, &layer_cfg, pstNewComposeList, pstDstSurface);
        if (s32Ret < 0) {
            goto ERR;
        }

        if ((layer_cfg.u8ZmeLayerCnt > g_gfx2d_capability.u8ZmeLayerNum) && (i > 0)) {
            i--;
        }

        layer_cfg.u8NodeCnt++;
        layer_cfg.u8StartLayer = i + 1;
        layer_cfg.u8LayerCnt = 1;
        layer_cfg.u8ZmeLayerCnt = 0;
    }
    return layer_cfg.u8NodeCnt;
ERR:
    for (i = 0; i < layer_cfg.u8NodeCnt; i++) {
        if (pstNewComposeList[i].compose != NULL) {
            HI_GFX_KFREE(HIGFX_GFX2D_ID, pstNewComposeList[i].compose);
            pstNewComposeList[i].compose = NULL;
        }
    }
    return 0;
}

static hi_s32 set_node_base_info(hwc_node *pstNode, drv_gfx2d_compose_surface *pstDstSurface, hi_u32 background_color)
{
    hi_s32 ret;
    SetDst(pstNode, pstDstSurface);
    SetBgColor(pstNode, background_color);
    ret = GFX2D_HAL_HWC_SplitNode(pstNode);
    if (ret < 0) {
        return ret;
    }
    return HI_SUCCESS;
}

static hi_s32 select_layer(const drv_gfx2d_dev_id dev_id, drv_gfx2d_compose_list *compose_list,
                           drv_gfx2d_compose_surface *pstDstSurface, HWC_LAYER_ID *aenLayerId)
{
    hi_s32 ret;
    ret = CheckCompose(dev_id, compose_list, pstDstSurface);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    /* adjust the layer where the task will be placed */
    AdjustCompose(dev_id, compose_list, pstDstSurface);
    /* select the corresponding layer ID */
    ret = select_layer_id(compose_list, aenLayerId, HWC_LAYER_G8);
    if (ret < 0) {
        return ret;
    }
    return HI_SUCCESS;
}

static hi_s32 GFX2D_HAL_HWC_CreateNode(const drv_gfx2d_dev_id dev_id, drv_gfx2d_compose_list *compose_list,
                                       drv_gfx2d_compose_surface *pstDstSurface, hi_u8 u8NodeCnt, hwc_node **ppstNode)
{
    hi_u8 i, j;
    hi_s32 s32Ret;
    hi_u16 color_type = 0;
    HWC_LAYER_ID aenLayerId[HWC_LAYER_G8] = {0};
    hwc_node *node[GFX2D_MAX_LAYERS] = {0};
    for (i = 0; i < u8NodeCnt; i++) {
        s32Ret = select_layer(dev_id, &compose_list[i], pstDstSurface, aenLayerId);
        if (s32Ret < 0) {
            goto ERR;
        }
        node[i] = (hwc_node *)GFX2D_MEM_Alloc(sizeof(hwc_node));
        if (node[i] == NULL) {
            s32Ret = DRV_GFX2D_ERR_NO_MEM;
            goto ERR;
        }
        InitNode(node[i]);
        node[i]->pFirstNode = node[0];
        for (j = 0; j < compose_list[i].compose_cnt; j++) {
            if (aenLayerId[j] <= HWC_LAYER_G1) {
                color_type = GetOptCode(compose_list[i].compose[j].compose_surface.format, pstDstSurface->format);
                SetInCsc(aenLayerId[j], node[i], color_type);
            }
            SetSrc(node[i], &(compose_list[i].compose[j]), j, aenLayerId[j]);
        }

        s32Ret = set_node_base_info(node[i], pstDstSurface, compose_list[i].background_color);
        if (s32Ret < 0) {
            goto ERR;
        }
        if (i > 0) {
            s32Ret = GFX2D_HAL_HWC_LinkNode(node[i - 1], node[i]);
            if (s32Ret < 0) {
                goto ERR;
            }
        }
    }
    *ppstNode = node[0]; /* index is 0 */
    return HI_SUCCESS;
ERR:
    for (i = 0; i < u8NodeCnt; i++) {
        if (node[i] != NULL) {
            GFX2D_MEM_Free(node[i]);
        }
    }
    *ppstNode = HI_NULL;
    return s32Ret;
}

static hi_s32 GFX2D_HAL_HWC_CheckCompose(const drv_gfx2d_dev_id dev_id, drv_gfx2d_compose_list *compose_list,
                                         drv_gfx2d_compose_surface *pstDstSurface, hi_void **ppNode,
                                         GFX2D_HAL_DEV_TYPE_E *penNodeType)
{
    hi_u32 i;
    hi_u32 resize_cnt;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(compose_list, DRV_GFX2D_ERR_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(compose_list->compose, DRV_GFX2D_ERR_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstDstSurface, DRV_GFX2D_ERR_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(ppNode, DRV_GFX2D_ERR_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(penNodeType, DRV_GFX2D_ERR_NULL_PTR);

    if ((compose_list->compose_cnt == 0) || (compose_list->compose_cnt > g_gfx2d_capability.u8MaxLayerNum)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, compose_list->compose_cnt);
        return DRV_GFX2D_ERR_INVALID_COMPOSECNT;
    }

    resize_cnt = 0;
    for (i = 0; i < compose_list->compose_cnt; i++) {
        if (compose_list->compose[i].opt.resize.resize_en) {
            resize_cnt++;
        }

        if (resize_cnt > g_gfx2d_capability.u8ZmeLayerNum) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, resize_cnt);
            return DRV_GFX2D_ERR_UNSUPPORT;
        }
    }

    return HI_SUCCESS;
}

/* HWC set composer operation */
hi_s32 GFX2D_HAL_HWC_Compose(const drv_gfx2d_dev_id dev_id, drv_gfx2d_compose_list *compose_list,
                             drv_gfx2d_compose_surface *pstDstSurface, hi_void **ppNode,
                             GFX2D_HAL_DEV_TYPE_E *penNodeType)
{
    hwc_node *pstNode = NULL;
    hi_u32 i = 0; /* 0 algorithm data */
    hi_s32 s32Ret;
    hi_u8 u8NodeCnt;
    drv_gfx2d_compose_list tmp_compose_list[GFX2D_MAX_LAYERS];

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(compose_list, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstDstSurface, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(ppNode, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(penNodeType, HI_FAILURE);

    s32Ret = GFX2D_HAL_HWC_CheckCompose(dev_id, compose_list, pstDstSurface, ppNode, penNodeType);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_HAL_HWC_CheckCompose, s32Ret);
        return s32Ret;
    }

    memset_s(tmp_compose_list, sizeof(drv_gfx2d_compose_list) * GFX2D_MAX_LAYERS, 0,
             sizeof(drv_gfx2d_compose_list) * GFX2D_MAX_LAYERS);

    u8NodeCnt = GFX2D_HAL_HWC_ConfigComposeList(compose_list, tmp_compose_list, GFX2D_MAX_LAYERS, pstDstSurface);
    if ((u8NodeCnt == 0) || (u8NodeCnt > GFX2D_MAX_LAYERS)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, GFX2D_MAX_LAYERS);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u8NodeCnt);
        return DRV_GFX2D_ERR_UNSUPPORT;
    }

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, u8NodeCnt);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");

    s32Ret = GFX2D_HAL_HWC_CreateNode(dev_id, tmp_compose_list, pstDstSurface, u8NodeCnt, &pstNode);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, GFX2D_HAL_HWC_CreateNode, FAILURE_TAG);
        s32Ret = DRV_GFX2D_ERR_UNSUPPORT;
        goto ERR;
    }

    *ppNode = pstNode;
    *penNodeType = GFX2D_HAL_DEV_TYPE_HWC;

ERR:
    for (i = 0; i < u8NodeCnt; i++) {
        if (tmp_compose_list[i].compose != NULL) {
            HI_GFX_KFREE(HIGFX_GFX2D_ID, tmp_compose_list[i].compose);
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return s32Ret;
}

static hi_void InitSmmuCrgReg(hi_void)
{
#ifdef CONFIG_GFX_MMU_SUPPORT
    volatile U_PERI_CRG180 unTempValue;
    hi_u8 u8Cnt = 0; /* 0 algorithm data */
    volatile hi_reg_crg *reg_cfg = HI_NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    reg_cfg = hi_drv_sys_get_crg_reg_ptr();
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(reg_cfg);

    unTempValue.u32 = reg_cfg->PERI_CRG180.u32;

    /* enable clock */
    unTempValue.bits.tde_smmu_cken = 0x1;

    reg_cfg->PERI_CRG180.u32 = unTempValue.u32;

    for (u8Cnt = 0; u8Cnt < 10; u8Cnt++) { /* 10 algorithm data */
        if (reg_cfg->PERI_CRG180.u32 == unTempValue.u32) {
            break;
        }
    }

    /* cancel reset */
    unTempValue.bits.tde_smmu_srst_req = 0x0;

    reg_cfg->PERI_CRG180.u32 = unTempValue.u32;

    for (u8Cnt = 0; u8Cnt < 10; u8Cnt++) { /* 10 algorithm data */
        if (reg_cfg->PERI_CRG180.u32 == unTempValue.u32) {
            break;
        }
    }

    osal_mb();
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void DeinitSmmuCrgReg(hi_void)
{
#ifdef CONFIG_GFX_MMU_SUPPORT
#if defined(CONFIG_GFX_MMU_RESET) || defined(CONFIG_GFX_SMMU_CLOSE_CLOCK)
    U_PERI_CRG180 unTempValue;
    volatile hi_reg_crg *reg_cfg = HI_NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    reg_cfg = hi_drv_sys_get_crg_reg_ptr();
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(reg_cfg);

    unTempValue.u32 = reg_cfg->PERI_CRG180.u32;
#endif

#ifdef CONFIG_GFX_SMMU_CLOSE_CLOCK
    /* disable clock */
    unTempValue.bits.tde_smmu_cken = 0x0;
#endif

#ifdef CONFIG_GFX_MMU_RESET
    /* reset */
    unTempValue.bits.tde_smmu_srst_req = 0x1;
#endif
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void InitCrgReg(hi_void)
{
#ifdef CONFIG_GFX_MMU_SUPPORT
    hi_u32 u32BaseAddr;
#endif

    volatile U_PERI_CRG337 unTempValue;
    hi_u8 u8Cnt = 0; /* 0 algorithm data */
    volatile hi_reg_crg *reg_cfg = HI_NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    reg_cfg = hi_drv_sys_get_crg_reg_ptr();
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(reg_cfg);

    if (reg_cfg == HI_NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, reg_cfg);
        return;
    }

    InitSmmuCrgReg();

    unTempValue.u32 = reg_cfg->PERI_CRG337.u32;

    /* enable clock */
    unTempValue.bits.tde_cken = 0x1;

    reg_cfg->PERI_CRG337.u32 = unTempValue.u32;

    for (u8Cnt = 0; u8Cnt < 10; u8Cnt++) { /* 10 is max count */
        if (reg_cfg->PERI_CRG337.u32 == unTempValue.u32) {
            break;
        }
    }

    /* cancel reset */
    unTempValue.bits.tde_srst_req = 0x0;

    reg_cfg->PERI_CRG337.u32 = unTempValue.u32;

    for (u8Cnt = 0; u8Cnt < 10; u8Cnt++) { /* 10 is max count */
        if (reg_cfg->PERI_CRG337.u32 == unTempValue.u32) {
            break;
        }
    }

    osal_mb();
#if 0
    if (g_pst_hwc_reg != NULL) {
        g_pst_hwc_reg->u32MiscEllaneous = 0x300647f;
    }
#endif
#ifdef CONFIG_GFX_MMU_SUPPORT
    u32BaseAddr = GFX2D_HAL_GetBaseAddr(DRV_GFX2D_DEV_ID_0, GFX2D_HAL_DEV_TYPE_HWC);
    HI_GFX_MapSmmuReg(u32BaseAddr + 0xf000);
    HI_GFX_InitSmmu(u32BaseAddr + 0xf000);
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void DeinitCrgReg(hi_void)
{
    U_PERI_CRG337 unTempValue;
    volatile hi_reg_crg *reg_cfg = HI_NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    reg_cfg = hi_drv_sys_get_crg_reg_ptr();
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(reg_cfg);

    DeinitSmmuCrgReg();

    unTempValue.u32 = reg_cfg->PERI_CRG337.u32;

    /* reset */
    unTempValue.bits.tde_srst_req = 0x1;

    /* disable clock */
    unTempValue.bits.tde_cken = 0x0;

    osal_mb();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

#define REG_SIZE 0x40a4

static hi_s32 InitDevReg(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    /* map register */
    if (g_hwc_reg == HI_NULL) {
        g_hwc_reg = HI_GFX_REG_MAP((HWC_REG_BASE_ADDR), REG_SIZE);
        if (g_hwc_reg == HI_NULL) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_REG_MAP, FAILURE_TAG);
            return DRV_GFX2D_ERR_SYS;
        }
    }
#ifdef GFX2D_ALPHADETECT_SUPPORT
    if (g_pst_hwc_alpha_sum_reg == HI_NULL) {
        g_pst_hwc_alpha_sum_reg = HI_GFX_REG_MAP((HWC_REG_BASE_ADDR + 0x4074), sizeof(HWC_ALPHASUM_REG_S));
        if (g_pst_hwc_alpha_sum_reg == HI_NULL) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_REG_MAP, FAILURE_TAG);
            return DRV_GFX2D_ERR_SYS;
        }
    }
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_void DeinitDevReg(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    /* unmap register */
#ifdef GFX2D_ALPHADETECT_SUPPORT
    if (g_pst_hwc_alpha_sum_reg != HI_NULL) {
        HI_GFX_REG_UNMAP(g_pst_hwc_alpha_sum_reg);
        g_pst_hwc_alpha_sum_reg = HI_NULL;
    }
#endif
    if (g_hwc_reg != HI_NULL) {
        HI_GFX_REG_UNMAP(g_hwc_reg);
        g_hwc_reg = HI_NULL;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

#define ZME_LAYER_THREE 3

static drv_gfx_mem_info g_mem_info_zme_h[ZME_LAYER_THREE] = {{0}};
static drv_gfx_mem_info g_mem_info_zme_v[ZME_LAYER_THREE] = {{0}};

static hi_s32 InitFilterParam(hi_void)
{
    hi_u32 i, hor_filter_array_size, ver_filter_array_size;
    hi_s32 ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    hor_filter_array_size = gfx2d_get_hor_filter_size();
    if (hor_filter_array_size == 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, hor_filter_array_size);
        return DRV_GFX2D_ERR_NO_MEM;
    }

    ver_filter_array_size = gfx2d_get_ver_filter_size();
    if (ver_filter_array_size == 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, ver_filter_array_size);
        return DRV_GFX2D_ERR_NO_MEM;
    }
    for (i = 0; i < ZME_LAYER_THREE; i++) {
        ret = drv_gfx_mem_alloc(&g_mem_info_zme_h[i], "GFX2D_ZmeHParam", HI_FALSE, HI_FALSE, hor_filter_array_size);
        if (ret != HI_SUCCESS) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, ret);
            goto ERR;
        }
        ret = drv_gfx_mem_map(&g_mem_info_zme_h[i]);
        if (ret != HI_SUCCESS) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, ret);
            goto ERR;
        }
        gfx2d_init_zme_h_para(g_mem_info_zme_h[i].virtual_addr, hor_filter_array_size);
    }
    for (i = 0; i < ZME_LAYER_THREE; i++) {
        ret = drv_gfx_mem_alloc(&g_mem_info_zme_v[i], "GFX2D_ZmeVParam", HI_FALSE, HI_FALSE, ver_filter_array_size);
        if (ret != HI_SUCCESS) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, ret);
            goto ERR;
        }
        ret = drv_gfx_mem_map(&g_mem_info_zme_v[i]);
        if (ret != HI_SUCCESS) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, ret);
            goto ERR;
        }
        gfx2d_init_zme_v_para(g_mem_info_zme_v[i].virtual_addr, hor_filter_array_size);
    }
    return HI_SUCCESS;
ERR:
    for (i = 0; i < ZME_LAYER_THREE; i++) {
        drv_gfx_mem_free(&g_mem_info_zme_h[i]);
        drv_gfx_mem_free(&g_mem_info_zme_v[i]);
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_void DeinitFilterParam(hi_void)
{
    hi_u32 i;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    for (i = 0; i < ZME_LAYER_THREE; i++) {
        drv_gfx_mem_free(&g_mem_info_zme_h[i]);
        drv_gfx_mem_free(&g_mem_info_zme_v[i]);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

static hi_void hwc_hal_set_cfg_reg_offset(hwc_node *node)
{
    if (node != HI_NULL) {
        node->zme_cfg_addr_high.bits.zme_cfg_addr_high =
            HI_GFX_GET_HIGH_PART(GFX2D_MEM_GetPhyaddr((hi_void *)&node->src_zme[0].src1_hsp));
        node->zme_cfg_addr_low.bits.zme_cfg_addr_low =
            HI_GFX_GET_LOW_PART(GFX2D_MEM_GetPhyaddr((hi_void *)&node->src_zme[0].src1_hsp));

        node->cbm_cfg_addr_high.bits.cbm_cfg_addr_high =
            HI_GFX_GET_HIGH_PART(GFX2D_MEM_GetPhyaddr((hi_void *)&node->cbm_ctrl));
        node->cbm_cfg_addr_low.bits.cbm_cfg_addr_low =
            HI_GFX_GET_LOW_PART(GFX2D_MEM_GetPhyaddr((hi_void *)&node->cbm_ctrl));
    }
}
static hi_void InitNode(hwc_node *pstNode)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstNode);

    memset_s(pstNode, sizeof(hwc_node), 0, sizeof(hwc_node));

    /* Eanble list complete intterupt and err interrupts */
    /* The linklist end interrupt is used here, not the node completion interrupt. */
    pstNode->intmask.bits.eof_end_mask = 0x1;
    pstNode->intmask.bits.timeout_mask = 0x1;
    pstNode->intmask.bits.bus_err_mask = 0x1;

    hwc_hal_set_cfg_reg_offset(pstNode);
    /* overlay control register 31bit */
    pstNode->cbm_ctrl.bits.cbm_en = 0x1;
#ifdef GFX2D_ALPHADETECT_SUPPORT
    pstNode->unOutDswm.stBits.bit1AlphaDectectEn = 0x1;
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

typedef enum {
    REG_COLOR_FMT_ARGB8888 = 0x0,
    REG_COLOR_FMT_KRGB8888 = 0x1,
    REG_COLOR_FMT_ARGB4444 = 0x2,
    REG_COLOR_FMT_ARGB1555 = 0x3,
    REG_COLOR_FMT_ARGB8565 = 0x4,
    REG_COLOR_FMT_RGB888 = 0x5,
    REG_COLOR_FMT_RGB444 = 0x6,
    REG_COLOR_FMT_RGB555 = 0x7,
    REG_COLOR_FMT_RGB565 = 0x8,
    REG_COLOR_FMT_A1 = 0x9,
    REG_COLOR_FMT_A8 = 0xa,
    REG_COLOR_FMT_YCBCR888 = 0xb,
    REG_COLOR_FMT_AYCBCR8888 = 0xc,
    REG_COLOR_FMT_A1B = 0xd,
    REG_COLOR_FMT_CLUT1 = 0x10,
    REG_COLOR_FMT_CLUT2 = 0x11,
    REG_COLOR_FMT_CLUT4 = 0x12,
    REG_COLOR_FMT_CLUT8 = 0x13,
    REG_COLOR_FMT_ACLUT44 = 0x14,
    REG_COLOR_FMT_ACLUT88 = 0x15,
    REG_COLOR_FMT_CLUT1B = 0x16,
    REG_COLOR_FMT_CLUT2B = 0x17,
    REG_COLOR_FMT_CLUT4B = 0x18,
    REG_COLOR_FMT_ARGB2101010 = 0x19,
    REG_COLOR_FMT_FP16 = 0x1A,
    REG_COLOR_FMT_ARGB10101010 = 0x1D,
    REG_COLOR_FMT_RGB101010 = 0x1E,
    REG_COLOR_FMT_PKGYUYV = 0x20,
    REG_COLOR_FMT_PKGYVYU = 0x21,
    REG_COLOR_FMT_PKGUYVY = 0x22,
    REG_COLOR_FMT_PKGVYUY = 0x23,
    REG_COLOR_FMT_PKGVUYY = 0x24,
    REG_COLOR_FMT_PKGYYUV = 0x25,
    REG_COLOR_FMT_PKGUVYY = 0x26,
    REG_COLOR_FMT_PKGYYVU = 0x27,
    REG_COLOR_FMT_SP400 = 0x30,
    REG_COLOR_FMT_SP422H = 0x31,
    REG_COLOR_FMT_SP422V = 0x32,
    REG_COLOR_FMT_SP420 = 0x33,
    REG_COLOR_FMT_SP444 = 0x34,
    REG_COLOR_FMT_PLANER420 = 0x35,
    REG_COLOR_FMT_MAX
} reg_color_fmt;

typedef enum {
    REG_ARGB_ORDER_NOT_ARGB = 0x00,
    REG_ARGB_ORDER_ARGB = 0x00,
    REG_ARGB_ORDER_ARBG = 0x01,
    REG_ARGB_ORDER_AGRB = 0x02,
    REG_ARGB_ORDER_AGBR = 0x03,
    REG_ARGB_ORDER_ABRG = 0x04,
    REG_ARGB_ORDER_ABGR = 0x05,

    REG_ARGB_ORDER_RGB = 0x00,
    REG_ARGB_ORDER_RBG = 0x01,
    REG_ARGB_ORDER_GRB = 0x02,
    REG_ARGB_ORDER_GBR = 0x03,
    REG_ARGB_ORDER_BRG = 0x04,
    REG_ARGB_ORDER_BGR = 0x05,

    REG_ARGB_ORDER_RAGB = 0x06,
    REG_ARGB_ORDER_RABG = 0x07,
    REG_ARGB_ORDER_RGAB = 0x08,
    REG_ARGB_ORDER_RGBA = 0x09,
    REG_ARGB_ORDER_RBAG = 0x0a,
    REG_ARGB_ORDER_RBGA = 0x0b,
    REG_ARGB_ORDER_GRAB = 0x0c,
    REG_ARGB_ORDER_GRBA = 0x0d,
    REG_ARGB_ORDER_GARB = 0x0e,
    REG_ARGB_ORDER_GABR = 0x0f,
    REG_ARGB_ORDER_GBRA = 0x10,
    REG_ARGB_ORDER_GBAR = 0x11,
    REG_ARGB_ORDER_BRGA = 0x12,
    REG_ARGB_ORDER_BRAG = 0x13,
    REG_ARGB_ORDER_BGRA = 0x14,
    REG_ARGB_ORDER_BGAR = 0x15,
    REG_ARGB_ORDER_BARG = 0x16,
    REG_ARGB_ORDER_BAGR = 0x17,
    REG_ARGB_ORDER_MAX
} reg_argb_order;

typedef enum {
    REG_UV_ORDER_NOT_YUV = 0x0,
    REG_UV_ORDER_NO_UV_ORDER = 0x0,
    REG_UV_ORDER_VU = 0x1,
    REG_UV_ORDER_UV = 0x2,
    REG_UV_ORDER_MAX
} reg_yuv_order;

#define make_gfx2d_layer_format(fmt, uv_order, argb_order) \
    ((REG_COLOR_FMT_##fmt) | ((REG_ARGB_ORDER_##argb_order) << 6) | ((REG_UV_ORDER_##uv_order) << 11))

#define get_gfx2d_layer_reg_format(api_fmt) (g_gfx2d_layer_format[api_fmt] & 0x3F)
#define get_gfx2d_layer_reg_argb_order(api_fmt) ((g_gfx2d_layer_format[api_fmt] >> 6) & 0x1F)
#define get_gfx2d_layer_reg_uv_order(api_fmt) ((g_gfx2d_layer_format[api_fmt] >> 11) & 0x1)

/* Map table of graphic layer fmt and HI_GFX2D_FMT */
static hi_u32 g_gfx2d_layer_format[DRV_GFX2D_FMT_PLANAR444 + 1] = {
    make_gfx2d_layer_format(RGB444, NOT_YUV, RGB),             /* HI_GFX2D_FMT_RGB444 */
    make_gfx2d_layer_format(RGB444, NOT_YUV, BGR),             /* HI_GFX2D_FMT_BGR444 */
    make_gfx2d_layer_format(RGB555, NOT_YUV, RGB),             /* HI_GFX2D_FMT_RGB555 */
    make_gfx2d_layer_format(RGB555, NOT_YUV, BGR),             /* HI_GFX2D_FMT_BGR555 */
    make_gfx2d_layer_format(RGB565, NOT_YUV, RGB),             /* HI_GFX2D_FMT_RGB565 */
    make_gfx2d_layer_format(RGB565, NOT_YUV, BGR),             /* HI_GFX2D_FMT_BGR565 */
    make_gfx2d_layer_format(RGB888, NOT_YUV, RGB),             /* HI_GFX2D_FMT_RGB888 */
    make_gfx2d_layer_format(RGB888, NOT_YUV, BGR),             /* HI_GFX2D_FMT_BGR888 */
    make_gfx2d_layer_format(KRGB8888, NOT_YUV, ARGB),          /* HI_GFX2D_FMT_KRGB888 */
    make_gfx2d_layer_format(KRGB8888, NOT_YUV, ABGR),          /* HI_GFX2D_FMT_KBGR888 */
    make_gfx2d_layer_format(ARGB4444, NOT_YUV, ARGB),          /* HI_GFX2D_FMT_ARGB4444 */
    make_gfx2d_layer_format(ARGB4444, NOT_YUV, ABGR),          /* HI_GFX2D_FMT_ABGR4444 */
    make_gfx2d_layer_format(ARGB4444, NOT_YUV, RGBA),          /* HI_GFX2D_FMT_RGBA4444 */
    make_gfx2d_layer_format(ARGB4444, NOT_YUV, BGRA),          /* HI_GFX2D_FMT_BGRA4444 */
    make_gfx2d_layer_format(ARGB1555, NOT_YUV, ARGB),          /* HI_GFX2D_FMT_ARGB1555 */
    make_gfx2d_layer_format(ARGB1555, NOT_YUV, ABGR),          /* HI_GFX2D_FMT_ABGR1555 */
    make_gfx2d_layer_format(ARGB1555, NOT_YUV, RGBA),          /* HI_GFX2D_FMT_RGBA1555 */
    make_gfx2d_layer_format(ARGB1555, NOT_YUV, BGRA),          /* HI_GFX2D_FMT_BGRA1555 */
    make_gfx2d_layer_format(ARGB8565, NOT_YUV, ARGB),          /* HI_GFX2D_FMT_ARGB8565 */
    make_gfx2d_layer_format(ARGB8565, NOT_YUV, ABGR),          /* HI_GFX2D_FMT_ABGR8565 */
    make_gfx2d_layer_format(ARGB8565, NOT_YUV, RGBA),          /* HI_GFX2D_FMT_RGBA8565 */
    make_gfx2d_layer_format(ARGB8565, NOT_YUV, BGRA),          /* HI_GFX2D_FMT_BGRA8565 */
    make_gfx2d_layer_format(ARGB8888, NOT_YUV, ARGB),          /* HI_GFX2D_FMT_ARGB8888 */
    make_gfx2d_layer_format(ARGB8888, NOT_YUV, ABGR),          /* HI_GFX2D_FMT_ABGR8888 */
    make_gfx2d_layer_format(ARGB8888, NOT_YUV, RGBA),          /* HI_GFX2D_FMT_RGBA8888 */
    make_gfx2d_layer_format(ARGB8888, NOT_YUV, BGRA),          /* HI_GFX2D_FMT_BGRA8888 */
    make_gfx2d_layer_format(ARGB2101010, NOT_YUV, ARGB),       /* HI_GFX2D_FMT_ARGB2101010 */
    make_gfx2d_layer_format(ARGB10101010, NOT_YUV, ARGB),      /* HI_GFX2D_FMT_ARGB10101010 */
    make_gfx2d_layer_format(FP16, NOT_YUV, ARGB),              /* HI_GFX2D_FMT_FP16 */
    make_gfx2d_layer_format(CLUT1, NOT_YUV, NOT_ARGB),         /* HI_GFX2D_FMT_CLUT1 */
    make_gfx2d_layer_format(CLUT2, NOT_YUV, NOT_ARGB),         /* HI_GFX2D_FMT_CLUT2 */
    make_gfx2d_layer_format(CLUT4, NOT_YUV, NOT_ARGB),         /* HI_GFX2D_FMT_CLUT4 */
    make_gfx2d_layer_format(CLUT8, NOT_YUV, NOT_ARGB),         /* HI_GFX2D_FMT_CLUT8 */
    make_gfx2d_layer_format(ACLUT44, NOT_YUV, NOT_ARGB),       /* HI_GFX2D_FMT_ACLUT44 */
    make_gfx2d_layer_format(ACLUT88, NOT_YUV, NOT_ARGB),       /* HI_GFX2D_FMT_ACLUT88 */
    make_gfx2d_layer_format(YCBCR888, NOT_YUV, NOT_ARGB),      /* HI_GFX2D_FMT_YUV888 */
    make_gfx2d_layer_format(AYCBCR8888, NOT_YUV, NOT_ARGB),    /* HI_GFX2D_FMT_AYUV8888 */
    make_gfx2d_layer_format(PKGYUYV, NO_UV_ORDER, NOT_ARGB),   /* HI_GFX2D_FMT_YUYV422 */
    make_gfx2d_layer_format(PKGYVYU, NO_UV_ORDER, NOT_ARGB),   /* HI_GFX2D_FMT_YVYU422 */
    make_gfx2d_layer_format(PKGUYVY, NO_UV_ORDER, NOT_ARGB),   /* HI_GFX2D_FMT_UYVY422 */
    make_gfx2d_layer_format(PKGYYUV, NO_UV_ORDER, NOT_ARGB),   /* HI_GFX2D_FMT_YYUV422 */
    make_gfx2d_layer_format(PKGVYUY, NO_UV_ORDER, NOT_ARGB),   /* HI_GFX2D_FMT_VYUY422 */
    make_gfx2d_layer_format(PKGVUYY, NO_UV_ORDER, NOT_ARGB),   /* HI_GFX2D_FMT_VUYY422 */
    make_gfx2d_layer_format(SP400, NO_UV_ORDER, NOT_ARGB),     /* HI_GFX2D_FMT_SEMIPLANAR400 */
    make_gfx2d_layer_format(SP420, UV, NOT_ARGB),              /* HI_GFX2D_FMT_SEMIPLANAR420UV */
    make_gfx2d_layer_format(SP420, VU, NOT_ARGB),              /* HI_GFX2D_FMT_SEMIPLANAR420VU */
    make_gfx2d_layer_format(SP422H, UV, NOT_ARGB),             /* HI_GFX2D_FMT_SEMIPLANAR422UV_H */
    make_gfx2d_layer_format(SP422H, VU, NOT_ARGB),             /* HI_GFX2D_FMT_SEMIPLANAR422VU_H */
    make_gfx2d_layer_format(SP422V, UV, NOT_ARGB),             /* HI_GFX2D_FMT_SEMIPLANAR422UV_V */
    make_gfx2d_layer_format(SP422V, VU, NOT_ARGB),             /* HI_GFX2D_FMT_SEMIPLANAR422VU_V */
    make_gfx2d_layer_format(SP444, UV, NOT_ARGB),              /* HI_GFX2D_FMT_SEMIPLANAR444UV */
    make_gfx2d_layer_format(SP444, VU, NOT_ARGB),              /* HI_GFX2D_FMT_SEMIPLANAR444VU */
    make_gfx2d_layer_format(MAX, NO_UV_ORDER, NOT_ARGB),       /* HI_GFX2D_FMT_PLANAR400 */
    make_gfx2d_layer_format(PLANER420, NO_UV_ORDER, NOT_ARGB), /* HI_GFX2D_FMT_PLANAR420 */
    make_gfx2d_layer_format(MAX, NO_UV_ORDER, NOT_ARGB),       /* HI_GFX2D_FMT_PLANAR411 */
    make_gfx2d_layer_format(MAX, NO_UV_ORDER, NOT_ARGB),       /* HI_GFX2D_FMT_PLANAR410 */
    make_gfx2d_layer_format(MAX, NO_UV_ORDER, NOT_ARGB),       /* HI_GFX2D_FMT_PLANAR422H */
    make_gfx2d_layer_format(MAX, NO_UV_ORDER, NOT_ARGB),       /* HI_GFX2D_FMT_PLANAR422V */
    make_gfx2d_layer_format(MAX, NO_UV_ORDER, NOT_ARGB),       /* HI_GFX2D_FMT_PLANAR444 */
};

static hi_void hwc_hal_csc_inconv_yuv2rgb(HWC_LAYER_ID src_id, hwc_node *node)
{
    if (src_id == HWC_LAYER_G0) {
        node->tde_csc_ctrl.bits.src_csc_mode = 0;
        src_id = 0;
    } else if (src_id == HWC_LAYER_VHD) {
        node->tde_csc_ctrl.bits.src_csc_mode = 1;
        src_id = 0;
    } else if (src_id == HWC_LAYER_G1) {
        src_id = 1;
    } else {
        osal_printk("csc src_id error.\n");
        return;
    }

    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_en = 1;
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_ck_gt_en = 1;
    node->hipp_csc[src_id].src_hipp_csc_coef00.bits.src_hipp_csc_coef00 = 1024; /* 1024 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef01.bits.src_hipp_csc_coef01 = 0;
    node->hipp_csc[src_id].src_hipp_csc_coef02.bits.src_hipp_csc_coef02 = 1576; /* 1576 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef10.bits.src_hipp_csc_coef10 = 1024; /* 1024 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef11.bits.src_hipp_csc_coef11 = -187; /* -187 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef12.bits.src_hipp_csc_coef12 = -470; /* -470 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef20.bits.src_hipp_csc_coef20 = 1024; /* 1024 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef21.bits.src_hipp_csc_coef21 = 1859; /* 1859 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef22.bits.src_hipp_csc_coef22 = 0;
    node->hipp_csc[src_id].src_hipp_csc_idc0.bits.src_hipp_csc_idc0 = -64;  /* -64 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_idc1.bits.src_hipp_csc_idc1 = -512; /* -512 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_idc2.bits.src_hipp_csc_idc2 = -512; /* -512 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_odc0.bits.src_hipp_csc_odc0 = 0;
    node->hipp_csc[src_id].src_hipp_csc_odc1.bits.src_hipp_csc_odc1 = 0;
    node->hipp_csc[src_id].src_hipp_csc_odc2.bits.src_hipp_csc_odc2 = 0;
    node->hipp_csc[src_id].src_hipp_csc_scale.bits.src_hipp_csc_scale = 10; /* 10 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_min_y.bits.src_hipp_csc_min_y = 0;
    node->hipp_csc[src_id].src_hipp_csc_min_c.bits.src_hipp_csc_min_c = 0;
    node->hipp_csc[src_id].src_hipp_csc_max_y.bits.src_hipp_csc_max_y = 0x3ff; /* csc max y: 0x3ff */
    node->hipp_csc[src_id].src_hipp_csc_max_c.bits.src_hipp_csc_max_c = 0x3ff; /* csc max c: 0x3ff */
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_en = 1;
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_demo_en = 0;
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_ck_gt_en = 1;
}

static hi_void hwc_hal_csc_inconv_rgb2yuv(HWC_LAYER_ID src_id, hwc_node *node)
{
    if (src_id == HWC_LAYER_G0) {
        node->tde_csc_ctrl.bits.src_csc_mode = 0; /* SRC1 used */
        src_id = 0;
    } else if (src_id == HWC_LAYER_VHD) {
        node->tde_csc_ctrl.bits.src_csc_mode = 1; /* SRC2 used */
        src_id = 0;
    } else if (src_id == HWC_LAYER_G1) {
        src_id = 1;
    } else {
        osal_printk("csc src_id error.\n");
        return;
    }

    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_en = 1;
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_ck_gt_en = 1;
    node->hipp_csc[src_id].src_hipp_csc_coef00.bits.src_hipp_csc_coef00 = 5966;   /* 5966 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef01.bits.src_hipp_csc_coef01 = 20071;  /* 20071 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef02.bits.src_hipp_csc_coef02 = 2026;   /* 2026 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef10.bits.src_hipp_csc_coef10 = -3289;  /* -3289 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef11.bits.src_hipp_csc_coef11 = -11063; /* -11063 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef12.bits.src_hipp_csc_coef12 = 14352;  /* 14352 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef20.bits.src_hipp_csc_coef20 = 14352;  /* 14352 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef21.bits.src_hipp_csc_coef21 = -13021; /* -13021 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef22.bits.src_hipp_csc_coef22 = -1316;  /* -1316 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_idc0.bits.src_hipp_csc_idc0 = 0;
    node->hipp_csc[src_id].src_hipp_csc_idc1.bits.src_hipp_csc_idc1 = 0;
    node->hipp_csc[src_id].src_hipp_csc_idc2.bits.src_hipp_csc_idc2 = 0;
    node->hipp_csc[src_id].src_hipp_csc_odc0.bits.src_hipp_csc_odc0 = 64;   /* 64 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_odc1.bits.src_hipp_csc_odc1 = 512;  /* 512 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_odc2.bits.src_hipp_csc_odc2 = 512;  /* 512 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_scale.bits.src_hipp_csc_scale = 15; /* 15 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_min_y.bits.src_hipp_csc_min_y = 0;
    node->hipp_csc[src_id].src_hipp_csc_min_c.bits.src_hipp_csc_min_c = 0;
    node->hipp_csc[src_id].src_hipp_csc_max_y.bits.src_hipp_csc_max_y = 0x3ff; /* csc max y: 0x3ff */
    node->hipp_csc[src_id].src_hipp_csc_max_c.bits.src_hipp_csc_max_c = 0x3ff; /* csc max c: 0x3ff */
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_en = 1;
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_demo_en = 0;
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_ck_gt_en = 1;
}

static hi_void hwc_hal_csc_outconv_yuv2rgb(HWC_LAYER_ID src_id, hwc_node *node)
{
    src_id = 2; /* src 2 */

    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_en = 1;
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_ck_gt_en = 1;
    node->hipp_csc[src_id].src_hipp_csc_coef00.bits.src_hipp_csc_coef00 = 1024; /* 1024 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef01.bits.src_hipp_csc_coef01 = 0;
    node->hipp_csc[src_id].src_hipp_csc_coef02.bits.src_hipp_csc_coef02 = 1576; /* 1576 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef10.bits.src_hipp_csc_coef10 = 1024; /* 1024 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef11.bits.src_hipp_csc_coef11 = -187; /* -187 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef12.bits.src_hipp_csc_coef12 = -470; /* -470 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef20.bits.src_hipp_csc_coef20 = 1024; /* 1024 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef21.bits.src_hipp_csc_coef21 = 1859; /* 1859 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef22.bits.src_hipp_csc_coef22 = 0;
    node->hipp_csc[src_id].src_hipp_csc_idc0.bits.src_hipp_csc_idc0 = -64;  /* -64 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_idc1.bits.src_hipp_csc_idc1 = -512; /* -512 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_idc2.bits.src_hipp_csc_idc2 = -512; /* -512 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_odc0.bits.src_hipp_csc_odc0 = 0;
    node->hipp_csc[src_id].src_hipp_csc_odc1.bits.src_hipp_csc_odc1 = 0;
    node->hipp_csc[src_id].src_hipp_csc_odc2.bits.src_hipp_csc_odc2 = 0;
    node->hipp_csc[src_id].src_hipp_csc_scale.bits.src_hipp_csc_scale = 10; /* 10 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_min_y.bits.src_hipp_csc_min_y = 0;
    node->hipp_csc[src_id].src_hipp_csc_min_c.bits.src_hipp_csc_min_c = 0;
    node->hipp_csc[src_id].src_hipp_csc_max_y.bits.src_hipp_csc_max_y = 0x3ff; /* csc max y: 0x3ff */
    node->hipp_csc[src_id].src_hipp_csc_max_c.bits.src_hipp_csc_max_c = 0x3ff; /* csc max c: 0x3ff */
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_en = 1;
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_demo_en = 0;
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_ck_gt_en = 1;
}

static hi_void hwc_hal_csc_outconv_rgb2yuv(HWC_LAYER_ID src_id, hwc_node *node)
{
    src_id = 2; /* src 2 */

    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_en = 1;
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_ck_gt_en = 1;
    node->hipp_csc[src_id].src_hipp_csc_coef00.bits.src_hipp_csc_coef00 = 5966;   /* 5966 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef01.bits.src_hipp_csc_coef01 = 20071;  /* 20071 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef02.bits.src_hipp_csc_coef02 = 2026;   /* 2026 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef10.bits.src_hipp_csc_coef10 = -3289;  /* -3289 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef11.bits.src_hipp_csc_coef11 = -11063; /* -11063 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef12.bits.src_hipp_csc_coef12 = 14352;  /* 14352 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef20.bits.src_hipp_csc_coef20 = 14352;  /* 14352 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef21.bits.src_hipp_csc_coef21 = -13021; /* -13021 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_coef22.bits.src_hipp_csc_coef22 = -1316;  /* -1316 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_idc0.bits.src_hipp_csc_idc0 = 0;
    node->hipp_csc[src_id].src_hipp_csc_idc1.bits.src_hipp_csc_idc1 = 0;
    node->hipp_csc[src_id].src_hipp_csc_idc2.bits.src_hipp_csc_idc2 = 0;
    node->hipp_csc[src_id].src_hipp_csc_odc0.bits.src_hipp_csc_odc0 = 64;   /* 64 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_odc1.bits.src_hipp_csc_odc1 = 512;  /* 512 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_odc2.bits.src_hipp_csc_odc2 = 512;  /* 512 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_scale.bits.src_hipp_csc_scale = 15; /* 15 csc coef */
    node->hipp_csc[src_id].src_hipp_csc_min_y.bits.src_hipp_csc_min_y = 0;
    node->hipp_csc[src_id].src_hipp_csc_min_c.bits.src_hipp_csc_min_c = 0;
    node->hipp_csc[src_id].src_hipp_csc_max_y.bits.src_hipp_csc_max_y = 0x3ff; /* csc max y: 0x3ff */
    node->hipp_csc[src_id].src_hipp_csc_max_c.bits.src_hipp_csc_max_c = 0x3ff; /* csc max c: 0x3ff */
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_en = 1;
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_demo_en = 0;
    node->hipp_csc[src_id].src_hipp_csc_ctrl.bits.src_hipp_csc_ck_gt_en = 1;
}

static hi_void SetInCsc(HWC_LAYER_ID id, hwc_node *node, hi_u16 color_type)
{
    if (color_type == 0) {
        return;
    }
    if (color_type == 1) {
        hwc_hal_csc_inconv_rgb2yuv(id, node);
    } else {
        hwc_hal_csc_inconv_yuv2rgb(id, node);
    }
    return;
}

static hi_void SetOutCsc(HWC_LAYER_ID id, hwc_node *node, hi_u16 color_type)
{
    if (color_type == 1) {
        hwc_hal_csc_outconv_yuv2rgb(id, node);
    } else {
        hwc_hal_csc_outconv_rgb2yuv(id, node);
    }
    return;
}

#define MB_WTH 32
#define MB_WTH_BIT 5
#define SEG_WTH 256
#define MAX(a, b) (((a) >= (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define CLIP(a) (((a) >= 0) ? (a) : (0))
#define CLIP2(m, n, a) (((a) > (m)) ? (m) : ((a) < (n) ? (n) : (a)))

typedef struct {
    hi_u32 frame_width;
    hi_u32 frame_height;
    hi_u32 pix_format;
    hi_u32 bit_depth;
    hi_u32 conv_en;
    hi_u32 cmp_mode;
    hi_u32 is_lossless;
    hi_u32 comp_ratio_int;
} frame_input;

typedef struct {
    hi_u32 is_lossless;
    hi_u32 cmp_mode;
    hi_u32 pix_format;
    hi_u32 bit_depth;
    hi_u32 conv_en;
    hi_u32 frame_width;
    hi_u32 frame_height;
    hi_u32 rm_debug_en;
    hi_u32 smth_thr;
    hi_u32 still_thr;
    hi_u32 big_grad_thr;
    hi_u32 diff_thr;
    hi_u32 smth_pix_num_thr;
    hi_u32 still_pix_num_thr;
    hi_u32 noise_pix_num_thr;
    hi_u32 grph_group_num_thr;
    hi_u32 qp_inc1_bits_thr;
    hi_u32 qp_inc2_bits_thr;
    hi_u32 qp_dec1_bits_thr;
    hi_u32 qp_dec2_bits_thr;
    hi_u32 raw_bits_penalty;
    hi_u32 buf_fullness_thr_reg0;
    hi_u32 buf_fullness_thr_reg1;
    hi_u32 buf_fullness_thr_reg2;
    hi_u32 qp_rge_reg0;
    hi_u32 qp_rge_reg1;
    hi_u32 qp_rge_reg2;
    hi_u32 bits_offset_reg0;
    hi_u32 bits_offset_reg1;
    hi_u32 bits_offset_reg2;
    hi_u32 grph_loss_thr;
    hi_u32 est_err_gain_map;
    hi_u32 buffer_size;
    hi_u32 buffer_init_bits;
    hi_u32 smooth_status_thr;
    hi_u32 budget_mb_bits;
    hi_u32 budget_mb_bits_last;
    hi_u32 min_mb_bits;
    hi_u32 max_mb_qp;
    hi_u32 reserve_para0;
} out_cfg;

static hi_void init_frame_cfg(out_cfg *cfg, frame_input *input)
{
    cfg->is_lossless = 0;
    cfg->cmp_mode = 0;
    cfg->pix_format = 1;
    cfg->frame_width = input->frame_width;
    cfg->frame_height = input->frame_height;
    cfg->rm_debug_en = 1;
    cfg->bit_depth = 0;

    cfg->conv_en = 0;
    cfg->buffer_init_bits = 7000; /* 7000 compress cfg */
    cfg->buffer_size = 8192;      /* 8192 compress cfg */
    return;
}

static hi_void calc_frame_compress_cfg(out_cfg *cfg, frame_input *input)
{
    hi_s32 mb_num_x, delta, lastMbWth;
    hi_s32 mb_ori_bits, mb_safe_bit;
    hi_s32 bit_depth = 8; /* 8 bit depth */
    hi_s32 cpt_num = 4;   /* 4 compress cfg */
    lastMbWth = (cfg->frame_width % MB_WTH) ? (cfg->frame_width % MB_WTH) : MB_WTH;
    mb_ori_bits = bit_depth * MB_WTH * cpt_num;
    mb_safe_bit = 2 * MB_WTH * cpt_num; /* 2 compress cfg */

    mb_num_x = (cfg->frame_width + MB_WTH - 1) / MB_WTH;
    lastMbWth = (cfg->frame_width % MB_WTH) ? (cfg->frame_width % MB_WTH) : MB_WTH;
    delta = MAX(1, (cfg->buffer_size - cfg->buffer_init_bits + mb_num_x / 2) / (mb_num_x));

    cfg->budget_mb_bits = MB_WTH * bit_depth * cpt_num * 1000 / input->comp_ratio_int;
    cfg->budget_mb_bits = input->is_lossless ? MB_WTH * bit_depth * cpt_num : cfg->budget_mb_bits;
    cfg->max_mb_qp = cfg->is_lossless
                            ? 0
                            : bit_depth - (cfg->budget_mb_bits - 76) /                   /* 76 compress cfg */
                                           (MB_WTH * (cfg->pix_format == 0 ? 3 : 4)); /* 3,4 compress cfg */
    cfg->budget_mb_bits = cfg->is_lossless ? cfg->budget_mb_bits : CLIP(cfg->budget_mb_bits - delta);
    cfg->budget_mb_bits = MIN(MAX(cfg->budget_mb_bits, mb_safe_bit), mb_ori_bits);

    cfg->min_mb_bits = cfg->budget_mb_bits * 3 / 4; /* 3,4 compress cfg */
    cfg->budget_mb_bits_last = cfg->budget_mb_bits * lastMbWth / MB_WTH;

    cfg->smth_thr = 6 * bit_depth / 8;                                       /* 6,8 compress cfg */
    cfg->still_thr = 1 * bit_depth / 8;                                      /* 8 compress cfg */
    cfg->big_grad_thr = 30 * bit_depth / 8;                                  /* 30,8 compress cfg */
    cfg->diff_thr = 20 * bit_depth / 8;                                      /* 20,8 compress cfg */
    cfg->smth_pix_num_thr = 6;                                               /* 6 compress cfg */
    cfg->still_pix_num_thr = 10;                                             /* 10 compress cfg */
    cfg->noise_pix_num_thr = 16;                                             /* 16 compress cfg */
    cfg->grph_group_num_thr = 6;                                             /* 6 compress cfg */
    cfg->qp_inc1_bits_thr = 10 * cpt_num;                                    /* 10 compress cfg */
    cfg->qp_inc2_bits_thr = 15 * cpt_num;                                    /* 15 compress cfg */
    cfg->qp_dec1_bits_thr = 10 * cpt_num;                                    /* 10 compress cfg */
    cfg->qp_dec2_bits_thr = 20 * cpt_num;                                    /* 20 compress cfg */
    cfg->raw_bits_penalty = 10;                                              /* 10 compress cfg */
    cfg->grph_loss_thr = 4;                                                  /* 4 compress cfg */
    cfg->buf_fullness_thr_reg0 = (71 << 24) | (56 << 16) | (35 << 8) | 18;   /* 71,24,56,16,35,8,18 compress cfg */
    cfg->buf_fullness_thr_reg1 = (109 << 24) | (108 << 16) | (98 << 8) | 85; /* 109,24,108,16,98,8,85 compress cfg */
    cfg->buf_fullness_thr_reg2 = (0 << 24) | (116 << 16) | (114 << 8) | 110; /* 24,116,16,114,8,110 compress cfg */
    cfg->bits_offset_reg0 = ((-2 & 0xff) << 24) | ((-1 & 0xff) << 16) | ((0 & 0xff) << 8) | (2 & 0xff);
    cfg->bits_offset_reg1 = ((-4 & 0xff) << 24) | ((-4 & 0xff) << 16) | ((-4 & 0xff) << 8) | (-3 & 0xff);
    cfg->bits_offset_reg2 = ((-4 & 0xff) << 24) | ((-4 & 0xff) << 16) | ((-4 & 0xff) << 8) | (-4 & 0xff);
    cfg->est_err_gain_map = (7 << 28) | (7 << 24) | (7 << 20) | (7 << 16) | (6 << 12) | (6 << 8) | (5 << 4) | 5;
    cfg->smooth_status_thr = 10;
    cfg->reserve_para0 = (((10 * cpt_num) & 0xff) << 24) | ((0 & 0xff) << 16) | (((63 * cpt_num) & 0xff) << 8) |
                         ((32 * cpt_num) & 0xff);
    return;
}

static hi_void frame_compress_cfg(out_cfg *cfg, frame_input *input)
{
    init_frame_cfg(cfg, input);
    calc_frame_compress_cfg(cfg, input);

    if (cfg->max_mb_qp > 5) {
        cfg->qp_rge_reg0 = ((cfg->max_mb_qp - 3) << 28) | (3 << 24) | ((cfg->max_mb_qp - 3) << 20) | (2 << 16) |
                           ((cfg->max_mb_qp - 3) << 12) | (2 << 8) | ((cfg->max_mb_qp - 4) << 4) | 2;
        cfg->qp_rge_reg1 = ((cfg->max_mb_qp - 2) << 28) | (3 << 24) | ((cfg->max_mb_qp - 2) << 20) | (3 << 16) |
                           ((cfg->max_mb_qp - 2) << 12) | (3 << 8) | ((cfg->max_mb_qp - 3) << 4) | 3;
        cfg->qp_rge_reg2 = (cfg->max_mb_qp << 28) | (4 << 24) | ((cfg->max_mb_qp - 1) << 20) | (4 << 16) |
                           ((cfg->max_mb_qp - 1) << 12) | (4 << 8) | ((cfg->max_mb_qp - 1) << 4) | 4;
    } else if (cfg->max_mb_qp > 2) {
        cfg->qp_rge_reg0 = ((cfg->max_mb_qp - 3) << 28) | (0 << 24) | ((cfg->max_mb_qp - 3) << 20) | (0 << 16) |
                           ((cfg->max_mb_qp - 3) << 12) | (0 << 8) | ((cfg->max_mb_qp - 3) << 4) | 0;
        cfg->qp_rge_reg1 = ((cfg->max_mb_qp - 2) << 28) | (0 << 24) | ((cfg->max_mb_qp - 2) << 20) | (0 << 16) |
                           ((cfg->max_mb_qp - 2) << 12) | (0 << 8) | ((cfg->max_mb_qp - 3) << 4) | 0;
        cfg->qp_rge_reg2 = (cfg->max_mb_qp << 28) | (1 << 24) | ((cfg->max_mb_qp - 1) << 20) | (1 << 16) |
                           ((cfg->max_mb_qp - 1) << 12) | (1 << 8) | ((cfg->max_mb_qp - 1) << 4) | 1;
    } else if (cfg->max_mb_qp > 1) {
        cfg->qp_rge_reg0 = ((cfg->max_mb_qp - 2) << 28) | (0 << 24) | ((cfg->max_mb_qp - 2) << 20) | (0 << 16) |
                           ((cfg->max_mb_qp - 2) << 12) | (0 << 8) | ((cfg->max_mb_qp - 2) << 4) | 0;
        cfg->qp_rge_reg1 = ((cfg->max_mb_qp - 2) << 28) | (0 << 24) | ((cfg->max_mb_qp - 2) << 20) | (0 << 16) |
                           ((cfg->max_mb_qp - 1) << 12) | (0 << 8) | ((cfg->max_mb_qp - 1) << 4) | 0;
        cfg->qp_rge_reg2 = (cfg->max_mb_qp << 28) | (1 << 24) | ((cfg->max_mb_qp - 1) << 20) | (1 << 16) |
                           ((cfg->max_mb_qp - 1) << 12) | (1 << 8) | ((cfg->max_mb_qp - 1) << 4) | 1;
    } else if (cfg->max_mb_qp > 0) {
        cfg->qp_rge_reg0 = ((cfg->max_mb_qp - 1) << 28) | (0 << 24) | ((cfg->max_mb_qp - 1) << 20) | (0 << 16) |
                           ((cfg->max_mb_qp - 1) << 12) | (0 << 8) | ((cfg->max_mb_qp - 1) << 4) | 0;
        cfg->qp_rge_reg1 = ((cfg->max_mb_qp - 1) << 28) | (0 << 24) | ((cfg->max_mb_qp - 1) << 20) | (0 << 16) |
                           ((cfg->max_mb_qp - 1) << 12) | (0 << 8) | ((cfg->max_mb_qp - 1) << 4) | 0;
        cfg->qp_rge_reg2 = (cfg->max_mb_qp << 28) | (1 << 24) | ((cfg->max_mb_qp - 1) << 20) | (1 << 16) |
                           ((cfg->max_mb_qp - 1) << 12) | (1 << 8) | ((cfg->max_mb_qp - 1) << 4) | 1;
    } else {
        cfg->qp_rge_reg0 = 0;
        cfg->qp_rge_reg1 = 0;
        cfg->qp_rge_reg2 = 0;
    }
    return;
}

static hi_void hal_set_node_compress(hwc_node *node, out_cfg *cfg)
{
    node->dst_line_osd_cmp_rc_cfg0.bits.budget_mb_bits = cfg->budget_mb_bits;
    node->dst_line_osd_cmp_rc_cfg0.bits.budget_mb_bits_last = cfg->budget_mb_bits_last;

    node->dst_line_osd_cmp_rc_cfg1.bits.grph_loss_thr = cfg->grph_loss_thr;
    node->dst_line_osd_cmp_rc_cfg1.bits.raw_bits_penalty = cfg->raw_bits_penalty;

    node->dst_line_osd_cmp_rc_cfg2.bits.smth_thr = cfg->smth_thr;
    node->dst_line_osd_cmp_rc_cfg2.bits.still_thr = cfg->still_thr;
    node->dst_line_osd_cmp_rc_cfg2.bits.big_grad_thr = cfg->big_grad_thr;
    node->dst_line_osd_cmp_rc_cfg2.bits.diff_thr = cfg->diff_thr;

    node->dst_line_osd_cmp_rc_cfg3.bits.smth_pix_num_thr = cfg->smth_pix_num_thr;
    node->dst_line_osd_cmp_rc_cfg3.bits.still_pix_num_thr = cfg->still_pix_num_thr;
    node->dst_line_osd_cmp_rc_cfg3.bits.noise_pix_num_thr = cfg->noise_pix_num_thr;
    node->dst_line_osd_cmp_rc_cfg3.bits.grph_group_num_thr = cfg->grph_group_num_thr;

    node->dst_line_osd_cmp_rc_cfg4.bits.qp_inc1_bits_thr = cfg->qp_inc1_bits_thr;
    node->dst_line_osd_cmp_rc_cfg4.bits.qp_inc2_bits_thr = cfg->qp_inc2_bits_thr;
    node->dst_line_osd_cmp_rc_cfg4.bits.qp_dec1_bits_thr = cfg->qp_dec1_bits_thr;
    node->dst_line_osd_cmp_rc_cfg4.bits.qp_dec2_bits_thr = cfg->qp_dec2_bits_thr;

    node->dst_line_osd_cmp_rc_cfg5.bits.buf_fullness_thr_reg0 = cfg->buf_fullness_thr_reg0;
    node->dst_line_osd_cmp_rc_cfg6.bits.buf_fullness_thr_reg1 = cfg->buf_fullness_thr_reg1;
    node->dst_line_osd_cmp_rc_cfg7.bits.buf_fullness_thr_reg2 = cfg->buf_fullness_thr_reg1;
    node->dst_line_osd_cmp_rc_cfg8.bits.qp_rge_reg0 = cfg->qp_rge_reg0;
    node->dst_line_osd_cmp_rc_cfg9.bits.qp_rge_reg1 = cfg->qp_rge_reg1;
    node->dst_line_osd_cmp_rc_cfg10.bits.qp_rge_reg2 = cfg->qp_rge_reg2;
    node->dst_line_osd_cmp_rc_cfg11.bits.bits_offset_reg0 = cfg->bits_offset_reg0;
    node->dst_line_osd_cmp_rc_cfg12.bits.bits_offset_reg1 = cfg->bits_offset_reg1;
    node->dst_line_osd_cmp_rc_cfg13.bits.bits_offset_reg2 = cfg->bits_offset_reg2;
    node->dst_line_osd_cmp_rc_cfg14.bits.est_err_gain_map = cfg->est_err_gain_map;
    node->dst_line_osd_cmp_rc_cfg15.bits.buffer_init_bits = cfg->buffer_init_bits;
    node->dst_line_osd_cmp_rc_cfg15.bits.smooth_status_thr = cfg->smooth_status_thr;
    node->dst_line_osd_cmp_rc_cfg16.bits.min_mb_bits = cfg->min_mb_bits;
    node->dst_line_osd_cmp_rc_cfg16.bits.max_mb_qp = cfg->max_mb_qp;
    node->dst_line_osd_cmp_rsv.bits.reserve_para0 = cfg->reserve_para0;
    return;
}

static hi_void dst_set_compress(hwc_node *node, drv_gfx2d_compose_surface *dst, frame_input *input, out_cfg *cfg)
{
    hi_u32 head_stride = 16; /* 16 is default head stride */
    node->dst_ctrl.bits.cmp_en = 0x1;
    node->dst_ctrl.bits.cmp_addr_chg = 0x0;
    node->dst_head0_addr_l = dst->phy_addr[0];
    node->dst_head1_addr_l = dst->phy_addr[0] + head_stride;
    node->dst_ch0_addr_l = dst->phy_addr[0] + head_stride * dst->height;
    node->dst_ch1_addr_l = node->dst_ch0_addr_l + dst->stride[0];
    node->dst_ch0_stride = dst->stride[0] * 2;    /* 2 algorithm data */
    node->dst_ch1_stride = dst->stride[0] * 2;    /* 2 algorithm data */
    node->dst_head0_stride.u32 = head_stride * 2; /* 2 algorithm data */
    node->dst_head1_stride.u32 = head_stride * 2; /* 2 algorithm data */
    node->dst_line_osd_cmp_glb_info.bits.ice_en = 1;
    node->dst_line_osd_cmp_glb_info.bits.cmp_mode = 0;
    node->dst_line_osd_cmp_glb_info.bits.is_lossless = 0;
    node->dst_line_osd_cmp_glb_info.bits.conv_en = 0;
    node->dst_line_osd_cmp_glb_info.bits.bit_depth = 0;
    node->dst_line_osd_cmp_glb_info.bits.pix_format = 1;

    if (dst->format == DRV_GFX2D_FMT_ARGB10101010) {
        node->dst_line_osd_cmp_glb_info.bits.bit_depth = 1;
        node->dst_line_osd_cmp_glb_info.bits.pix_format = 1;
    } else {
        node->dst_line_osd_cmp_glb_info.bits.bit_depth = 0;
        node->dst_line_osd_cmp_glb_info.bits.pix_format = 1;
    }

    input->frame_width = node->dst_image_size.bits.dst_width + 1;
    input->frame_height = node->dst_image_size.bits.dst_height + 1;
    input->comp_ratio_int = 2000; /* 2000 is compress ratio */
    frame_compress_cfg(cfg, input);
    hal_set_node_compress(node, cfg);
    return;
}

static hi_void SetDst(hwc_node *pstNode, drv_gfx2d_compose_surface *pstDstSurface)
{
    frame_input input = {0};
    out_cfg cfg = {0};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstNode);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstDstSurface);

    pstNode->dst_ctrl.bits.dst_en = 0x1;

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(pstDstSurface->format, DRV_GFX2D_FMT_PLANAR444 + 1);
    pstNode->dst_ctrl.bits.dst_fmt = get_gfx2d_layer_reg_format(pstDstSurface->format);
    pstNode->dst_ctrl.bits.dst_argb_order = get_gfx2d_layer_reg_argb_order(pstDstSurface->format);

    pstNode->dst_alpha.bits.dst_alpha_thd = 0x80;

#ifdef CONFIG_GFX_MMU_SUPPORT
    pstNode->dst_ctrl.bits.dst_ch0_mmu_bypass = 0x0;
    pstNode->dst_ctrl.bits.dst_ch1_mmu_bypass = 0x0;
    pstNode->dst_ctrl.bits.dst_ch2_mmu_bypass = 0x0;
    pstNode->dst_ctrl.bits.dst_hd_ch0_mmu_bypass = 0x0;
    pstNode->dst_ctrl.bits.dst_hd_ch1_mmu_bypass = 0x0;
#else
    pstNode->dst_ctrl.bits.dst_ch0_mmu_bypass = 0x1;
    pstNode->dst_ctrl.bits.dst_ch1_mmu_bypass = 0x1;
    pstNode->dst_ctrl.bits.dst_ch2_mmu_bypass = 0x1;
    pstNode->dst_ctrl.bits.dst_hd_ch0_mmu_bypass = 0x1;
    pstNode->dst_ctrl.bits.dst_hd_ch1_mmu_bypass = 0x1;
    ;
#endif

    pstNode->dst_ch0_addr_l = pstDstSurface->phy_addr[0]; /* index is 0 */
    pstNode->dst_ch0_stride = pstDstSurface->stride[0];   /* index is 0 */

    pstNode->dst_image_size.bits.dst_width = pstDstSurface->width - 1;
    pstNode->dst_image_size.bits.dst_height = pstDstSurface->height - 1;

    if (pstDstSurface->compress_msg.compress_type != DRV_GFX2D_COMPRESS_NONE) {
        dst_set_compress(pstNode, pstDstSurface, &input, &cfg);
    }
    pstNode->cbm_imgsize.bits.cbm_width = pstNode->dst_image_size.bits.dst_width;
    pstNode->cbm_imgsize.bits.cbm_height = pstNode->dst_image_size.bits.dst_height;
    pstNode->cbm_cfg_addr_high.u32 = HI_GFX_GET_HIGH_PART(GFX2D_MEM_GetPhyaddr(pstNode) + 0x0800);
    pstNode->cbm_cfg_addr_low.u32 = HI_GFX_GET_LOW_PART(GFX2D_MEM_GetPhyaddr(pstNode) + 0x0800);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_u8 GetBpp(drv_gfx2d_compose_fmt format, hi_bool bYComponent)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (format <= DRV_GFX2D_FMT_BGR565) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 2; /* 2 bytes */
    } else if (format <= DRV_GFX2D_FMT_BGR888) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 3; /* 3 bytes */
    } else if (format <= DRV_GFX2D_FMT_KBGR888) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 4; /* 4 bytes */
    } else if (format <= DRV_GFX2D_FMT_BGRA1555) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 2; /* 2 bytes */
    } else if (format <= DRV_GFX2D_FMT_BGRA8565) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 3; /* 3 bytes */
    } else if (format <= DRV_GFX2D_FMT_ARGB2101010) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 4; /* 4 bytes */
    } else if (format <= DRV_GFX2D_FMT_ACLUT88) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 0; /* 0 bytes */
    } else if (format <= DRV_GFX2D_FMT_YUV888) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 3; /* 3 bytes */
    } else if (format <= DRV_GFX2D_FMT_AYUV8888) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 4; /* 4 bytes */
    } else if (format <= DRV_GFX2D_FMT_VUYY422) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 2; /* 2 bytes */
    } else if (format <= DRV_GFX2D_FMT_SEMIPLANAR444VU) {
        if (bYComponent) {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 1; /* 1 bytes */
        } else {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 2; /* 2 bytes */
        }
    } else {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 0; /* 0 bytes */
    }
}

static hi_u32 GetFilterParaLevel(hi_u32 u32FloatBitLen, hi_u32 u32Step)
{
    hi_u32 u32Level = 0; /* 0 algorithm data */
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    /* find the index of the table according to the step size */
    if ((0x1 << u32FloatBitLen) > u32Step) {
        u32Level = 0; /* 0 algorithm data */
    } else if ((0x1 << u32FloatBitLen) == u32Step) {
        u32Level = 1; /* 1 algorithm data */
    } else if (((0x4 << u32FloatBitLen) / 0x3) >= u32Step) {
        u32Level = 2; /* 2 algorithm data */
    } else if ((0x2 << u32FloatBitLen) >= u32Step) {
        u32Level = 3; /* 3 algorithm data */
    } else if ((0x3 << u32FloatBitLen) >= u32Step) {
        u32Level = 4; /* 4 algorithm data */
    } else if ((0x4 << u32FloatBitLen) >= u32Step) {
        u32Level = 5; /* 5 algorithm data */
    } else {
        u32Level = 6; /* 6 algorithm data */
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return u32Level;
}

static inline hi_void SetZmeOrder(drv_gfx2d_compose_fmt format, hi_u32 u32OutWidth, HWC_VHDHSP_U *punHsp)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (punHsp != NULL) {
        punHsp->stBits.bit1Order = 0x0;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_u32 GetZmeInputFmt(drv_gfx2d_compose_fmt format)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if ((DRV_GFX2D_FMT_SEMIPLANAR422UV_H == format) || (DRV_GFX2D_FMT_SEMIPLANAR422VU_H == format) ||
        ((DRV_GFX2D_FMT_YUYV422 <= format) && (format <= DRV_GFX2D_FMT_VUYY422)) ||
        (DRV_GFX2D_FMT_PLANAR400 == format) || (DRV_GFX2D_FMT_PLANAR411 == format) ||
        (DRV_GFX2D_FMT_PLANAR422H == format) || (DRV_GFX2D_FMT_PLANAR444 == format)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 0x0;
    } else if ((DRV_GFX2D_FMT_SEMIPLANAR420UV == format) || (DRV_GFX2D_FMT_SEMIPLANAR420VU == format) ||
               (DRV_GFX2D_FMT_SEMIPLANAR422UV_V == format) || (DRV_GFX2D_FMT_SEMIPLANAR422VU_V == format) ||
               (DRV_GFX2D_FMT_PLANAR410 == format) || (DRV_GFX2D_FMT_PLANAR420 == format) ||
               (DRV_GFX2D_FMT_PLANAR422V == format)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 0x1;
    } else {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 0x3;
    }
}

static hi_void SetVhdLayerZmeCoefAddr(hwc_node *pstNode, hi_u32 u32HStep, hi_u32 u32VStep, hi_u32 u32ChromaTap)
{
    return;
}

static hi_void SetGfxLayerZmeCoefAddr(hwc_node *pstNode, hi_u32 u32HStep, hi_u32 u32VStep, hi_u32 u32ChromaTap)
{
    return;
}

typedef struct {
    HWC_SRCCTL_U unCtrl;
    HWC_RES_U unRes;
    HWC_VHDHSP_U unVhdHsp;
    HWC_VHDHOFFSET_U unVhdLumHOffset;
    HWC_VHDHOFFSET_U unVhdChromaHOffset;
    HWC_VHDVSP_U unVhdVsp;
    HWC_RES_U unVhdZmeOutRes;
    HWC_RES_U unVhdZmeInRes;
    HWC_HPZME_U unHpZme;
    hi_u32 u32Hsr;
    hi_u32 u32Vsr;
    hi_bool bFilter;
} GFX2D_HAL_ZME_S;

typedef struct {
    hi_u32 in_width;
    hi_u32 in_height;
    drv_gfx2d_compose_fmt format;
    hi_u32 hdwsm_factor;
    hi_u32 h_step;
    hi_u32 v_step;
    hi_u32 out_width;
    hi_u32 out_height;
    hi_bool bFilter;
} zme_cfg;

static hi_void GetZmeInfo(hwc_node *pstNode, HWC_LAYER_ID enLayerId, GFX2D_HAL_ZME_S *pstZme)
{
    hi_u32 zme_index;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstNode);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstZme);

    if (enLayerId == HWC_LAYER_VHD) {
        pstZme->unCtrl.u32All = pstNode->src_Layer[1].unCtrl.u32All; /* index is 1 */
        pstZme->unRes.u32All = pstNode->src_Layer[1].unRes.u32All;   /* index is 1 */
        zme_index = 1;
    } else if (enLayerId == HWC_LAYER_G0) {
        pstZme->unCtrl.u32All = pstNode->src_Layer[0].unCtrl.u32All; /* index is 1 */
        pstZme->unRes.u32All = pstNode->src_Layer[0].unRes.u32All;   /* index is 1 */
        zme_index = 0;
    } else {
        pstZme->unCtrl.u32All = pstNode->astLayer[0].unCtrl.u32All; /* index is 1 */
        pstZme->unRes.u32All = pstNode->astLayer[0].unRes.u32All;   /* index is 1 */
        zme_index = 2;                                              /* index is 2 */
    }

    pstZme->unVhdHsp.u32All = pstNode->src_zme[zme_index].src1_hsp.u32;
    pstZme->unVhdLumHOffset.u32All = pstNode->src_zme[zme_index].src1_hloffset.u32;
    pstZme->unVhdChromaHOffset.u32All = pstNode->src_zme[zme_index].src1_hcoffset.u32;
    pstZme->unVhdVsp.u32All = pstNode->src_zme[zme_index].src1_vsp.u32;
    pstZme->unVhdZmeOutRes.u32All = pstNode->src_zme[zme_index].src1_zme_out_reso.u32;
    pstZme->unVhdZmeInRes.u32All = pstNode->src_zme[zme_index].src1_zme_in_reso.u32;
    pstZme->unHpZme.u32All = pstNode->src_hpzme[zme_index].u32;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void Set_src1_ZmeCoefAddr(hwc_node *node, hi_u32 u32HStep, hi_u32 u32VStep, hi_u32 u32ChromaTap)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(node);

    node->src1_zme_haddr_high.u32 =
        HI_GFX_GET_HIGH_PART(g_mem_info_zme_h[0].mmz_addr + GetFilterParaLevel(GFX2D_HAL_HWC_HSTEP_FLOATLEN, u32HStep) *
                                                                HWC_HORIZON_PARA_SIZE_PERLEVEL);
    node->src1_zme_haddr_low.u32 =
        HI_GFX_GET_LOW_PART(g_mem_info_zme_h[0].mmz_addr + GetFilterParaLevel(GFX2D_HAL_HWC_HSTEP_FLOATLEN, u32HStep) *
                                                               HWC_HORIZON_PARA_SIZE_PERLEVEL);

    if (u32ChromaTap == 0x0) {
        node->src1_zme_vaddr_high.u32 = HI_GFX_GET_HIGH_PART(
            g_mem_info_zme_v[0].mmz_addr +
            GetFilterParaLevel(GFX2D_HAL_HWC_VSTEP_FLOATLEN, u32VStep) * HWC_VERTICAL_PARA_SIZE_PERLEVEL);
        node->src1_zme_vaddr_low.u32 = HI_GFX_GET_LOW_PART(g_mem_info_zme_v[0].mmz_addr +
                                                           GetFilterParaLevel(GFX2D_HAL_HWC_VSTEP_FLOATLEN, u32VStep) *
                                                               HWC_VERTICAL_PARA_SIZE_PERLEVEL);
    } else {
        node->src1_zme_vaddr_high.u32 = HI_GFX_GET_HIGH_PART(
            g_mem_info_zme_v[0].mmz_addr +
            GetFilterParaLevel(GFX2D_HAL_HWC_VSTEP_FLOATLEN, u32VStep) * HWC_VERTICAL_PARA_SIZE_PERLEVEL);
        node->src1_zme_vaddr_low.u32 = HI_GFX_GET_LOW_PART(g_mem_info_zme_v[0].mmz_addr +
                                                           GetFilterParaLevel(GFX2D_HAL_HWC_VSTEP_FLOATLEN, u32VStep) *
                                                               HWC_VERTICAL_PARA_SIZE_PERLEVEL);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void Set_src2_ZmeCoefAddr(hwc_node *node, hi_u32 u32HStep, hi_u32 u32VStep, hi_u32 u32ChromaTap)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(node);
    node->src2_zme_haddr_high.u32 =
        HI_GFX_GET_HIGH_PART(g_mem_info_zme_h[1].mmz_addr + GetFilterParaLevel(GFX2D_HAL_HWC_HSTEP_FLOATLEN, u32HStep) *
                                                                HWC_HORIZON_PARA_SIZE_PERLEVEL);
    node->src2_zme_haddr_low.u32 =
        HI_GFX_GET_LOW_PART(g_mem_info_zme_h[1].mmz_addr + GetFilterParaLevel(GFX2D_HAL_HWC_HSTEP_FLOATLEN, u32HStep) *
                                                               HWC_HORIZON_PARA_SIZE_PERLEVEL);

    if (u32ChromaTap == 0x0) {
        node->src2_zme_vaddr_high.u32 = HI_GFX_GET_HIGH_PART(
            g_mem_info_zme_v[1].mmz_addr +
            GetFilterParaLevel(GFX2D_HAL_HWC_VSTEP_FLOATLEN, u32VStep) * HWC_VERTICAL_PARA_SIZE_PERLEVEL);
        node->src2_zme_vaddr_low.u32 = HI_GFX_GET_LOW_PART(g_mem_info_zme_v[1].mmz_addr +
                                                           GetFilterParaLevel(GFX2D_HAL_HWC_VSTEP_FLOATLEN, u32VStep) *
                                                               HWC_VERTICAL_PARA_SIZE_PERLEVEL);
    } else {
        node->src2_zme_vaddr_high.u32 = HI_GFX_GET_HIGH_PART(
            g_mem_info_zme_v[1].mmz_addr +
            GetFilterParaLevel(GFX2D_HAL_HWC_VSTEP_FLOATLEN, u32VStep) * HWC_VERTICAL_PARA_SIZE_PERLEVEL);
        node->src2_zme_vaddr_low.u32 = HI_GFX_GET_LOW_PART(g_mem_info_zme_v[1].mmz_addr +
                                                           GetFilterParaLevel(GFX2D_HAL_HWC_VSTEP_FLOATLEN, u32VStep) *
                                                               HWC_VERTICAL_PARA_SIZE_PERLEVEL);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void Set_src3_ZmeCoefAddr(hwc_node *node, hi_u32 u32HStep, hi_u32 u32VStep, hi_u32 u32ChromaTap)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(node);

    node->src3_zme_haddr_high.u32 = HI_GFX_GET_HIGH_PART(g_mem_info_zme_h[2].mmz_addr + /* 2 algorithm data */
                                                         GetFilterParaLevel(GFX2D_HAL_HWC_HSTEP_FLOATLEN, u32HStep) *
                                                             HWC_HORIZON_PARA_SIZE_PERLEVEL);
    node->src3_zme_haddr_low.u32 = HI_GFX_GET_LOW_PART(g_mem_info_zme_h[2].mmz_addr + /* 2 algorithm data */
                                                       GetFilterParaLevel(GFX2D_HAL_HWC_HSTEP_FLOATLEN, u32HStep) *
                                                           HWC_HORIZON_PARA_SIZE_PERLEVEL);

    if (u32ChromaTap == 0x0) {
        node->src3_zme_vaddr_high.u32 = HI_GFX_GET_HIGH_PART(
            g_mem_info_zme_v[2].mmz_addr + /* 2 algorithm data */
            GetFilterParaLevel(GFX2D_HAL_HWC_VSTEP_FLOATLEN, u32VStep) * HWC_VERTICAL_PARA_SIZE_PERLEVEL);
        node->src3_zme_vaddr_low.u32 = HI_GFX_GET_LOW_PART(g_mem_info_zme_v[2].mmz_addr + /* 2 algorithm data */
                                                           GetFilterParaLevel(GFX2D_HAL_HWC_VSTEP_FLOATLEN, u32VStep) *
                                                               HWC_VERTICAL_PARA_SIZE_PERLEVEL);
    } else {
        node->src3_zme_vaddr_high.u32 = HI_GFX_GET_HIGH_PART(
            g_mem_info_zme_v[2].mmz_addr + /* 2 algorithm data */
            GetFilterParaLevel(GFX2D_HAL_HWC_VSTEP_FLOATLEN, u32VStep) * HWC_VERTICAL_PARA_SIZE_PERLEVEL);
        node->src3_zme_vaddr_low.u32 = HI_GFX_GET_LOW_PART(g_mem_info_zme_v[2].mmz_addr + /* 2 algorithm data */
                                                           GetFilterParaLevel(GFX2D_HAL_HWC_VSTEP_FLOATLEN, u32VStep) *
                                                               HWC_VERTICAL_PARA_SIZE_PERLEVEL);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void SetZmeInfo(hwc_node *pstNode, HWC_LAYER_ID enLayerId, GFX2D_HAL_ZME_S *pstZme)
{
    hi_u32 zme_index;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstNode);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstZme);

    if (enLayerId == HWC_LAYER_VHD) {
        Set_src2_ZmeCoefAddr(pstNode, pstZme->u32Hsr, pstZme->u32Vsr, pstZme->unVhdVsp.stBits.bit1ChromaTap);
        pstNode->src_Layer[1].unCtrl.u32All = pstZme->unCtrl.u32All;
        pstNode->src_Layer[1].unRes.u32All = pstZme->unRes.u32All; /* index is 1 */
        zme_index = 1;
    } else if (enLayerId == HWC_LAYER_G0) {
        Set_src1_ZmeCoefAddr(pstNode, pstZme->u32Hsr, pstZme->u32Vsr, pstZme->unVhdVsp.stBits.bit1ChromaTap);
        pstNode->src_Layer[0].unCtrl.u32All = pstZme->unCtrl.u32All; /* index is 1 */
        pstNode->src_Layer[0].unRes.u32All = pstZme->unRes.u32All;   /* index is 1 */
        zme_index = 0;
    } else if (enLayerId == HWC_LAYER_G1) {
        Set_src3_ZmeCoefAddr(pstNode, pstZme->u32Hsr, pstZme->u32Vsr, pstZme->unVhdVsp.stBits.bit1ChromaTap);
        pstNode->astLayer[0].unCtrl.u32All = pstZme->unCtrl.u32All; /* index is 1 */
        pstNode->astLayer[0].unRes.u32All = pstZme->unRes.u32All;   /* index is 1 */
        zme_index = 2;                                              /* index is 2 */
    } else {
        osal_printk("layer id err.\n");
        return;
    }

    pstNode->src_zme[zme_index].src1_hsp.u32 = pstZme->unVhdHsp.u32All;
    pstNode->src_zme[zme_index].src1_hloffset.u32 = pstZme->unVhdLumHOffset.u32All;
    pstNode->src_zme[zme_index].src1_hcoffset.u32 = pstZme->unVhdChromaHOffset.u32All;
    pstNode->src_zme[zme_index].src1_vsp.u32 = pstZme->unVhdVsp.u32All;
    pstNode->src_zme[zme_index].src1_zme_out_reso.u32 = pstZme->unVhdZmeOutRes.u32All;
    pstNode->src_zme[zme_index].src1_zme_in_reso.u32 = pstZme->unVhdZmeInRes.u32All;
    pstNode->src_hpzme[zme_index].u32 = pstZme->unHpZme.u32All;

    pstNode->src_zme[zme_index].src1_vsr.bits.src1_vratio = pstZme->unVhdZmeInRes.stBits.bit16Height * (1 << 12) /
                                                            pstZme->unVhdZmeOutRes.stBits.bit16Height; /* 12 algorithm
                                                                                                          data */

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

typedef enum {
    HWC_COLORFMT_TRANSFORM_ARGB_ARGB = 0,
    HWC_COLORFMT_TRANSFORM_ARGB_YCbCr,
    HWC_COLORFMT_TRANSFORM_CLUT_ARGB,
    HWC_COLORFMT_TRANSFORM_CLUT_YCbCr,
    HWC_COLORFMT_TRANSFORM_CLUT_CLUT,
    HWC_COLORFMT_TRANSFORM_YCbCr_ARGB,
    HWC_COLORFMT_TRANSFORM_YCbCr_YCbCr,
    HWC_COLORFMT_TRANSFORM_An_An,
    HWC_COLORFMT_TRANSFORM_ARGB_An,
    HWC_COLORFMT_TRANSFORM_YCbCr_An,
    HWC_COLORFMT_TRANSFORM_BUTT,
} HWC_COLORFMT_TRANSFORM_E;

typedef enum {
    HWC_COLORFMT_CATEGORY_ARGB,
    HWC_COLORFMT_CATEGORY_CLUT,
    HWC_COLORFMT_CATEGORY_An,
    HWC_COLORFMT_CATEGORY_YCbCr,
    HWC_COLORFMT_CATEGORY_BYTE,
    HWC_COLORFMT_CATEGORY_HALFWORD,
    HWC_COLORFMT_CATEGORY_MB,
    HWC_COLORFMT_CATEGORY_MAX
} hwc_color_fmt_category;

static hwc_color_fmt_category hwc_OsiGetFmtCategory(drv_gfx2d_compose_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    /* target is ARGB format */
    if (color_fmt <= DRV_GFX2D_FMT_BGRA8888) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HWC_COLORFMT_CATEGORY_ARGB;
    } else if (color_fmt <= DRV_GFX2D_FMT_ACLUT88) { /* target is CLUT table format */
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HWC_COLORFMT_CATEGORY_CLUT;
    } else if (color_fmt <= DRV_GFX2D_FMT_PLANAR444) { /* target is YCbCr format */
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HWC_COLORFMT_CATEGORY_YCbCr;
    } else { /* error format */
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HWC_COLORFMT_CATEGORY_MAX;
    }
}

static HWC_COLORFMT_TRANSFORM_E hwc_OsiGetFmtTransType(drv_gfx2d_compose_fmt enSrc2Fmt, drv_gfx2d_compose_fmt enDstFmt)
{
    hwc_color_fmt_category enSrcCategory;
    hwc_color_fmt_category enDstCategory;
    /* get foreground pixel format category */
    enSrcCategory = hwc_OsiGetFmtCategory(enSrc2Fmt);
    /* get target pixel format category */
    enDstCategory = hwc_OsiGetFmtCategory(enDstFmt);
    switch (enSrcCategory) {
        case HWC_COLORFMT_CATEGORY_ARGB:
            if (enDstCategory == HWC_COLORFMT_CATEGORY_ARGB) {
                return HWC_COLORFMT_TRANSFORM_ARGB_ARGB;
            }
            if (enDstCategory == HWC_COLORFMT_CATEGORY_YCbCr) {
                return HWC_COLORFMT_TRANSFORM_ARGB_YCbCr;
            }
            if (enDstCategory == HWC_COLORFMT_CATEGORY_An) {
                return HWC_COLORFMT_TRANSFORM_ARGB_An;
            }
            return HWC_COLORFMT_TRANSFORM_BUTT;
        case HWC_COLORFMT_CATEGORY_CLUT:
            if (enDstCategory == HWC_COLORFMT_CATEGORY_ARGB) {
                return HWC_COLORFMT_TRANSFORM_CLUT_ARGB;
            }
            if (enDstCategory == HWC_COLORFMT_CATEGORY_YCbCr) {
                return HWC_COLORFMT_TRANSFORM_CLUT_YCbCr;
            }
            if (enDstCategory == HWC_COLORFMT_CATEGORY_CLUT) {
                return HWC_COLORFMT_TRANSFORM_CLUT_CLUT;
            }
            return HWC_COLORFMT_TRANSFORM_BUTT;
        case HWC_COLORFMT_CATEGORY_YCbCr:
            if (enDstCategory == HWC_COLORFMT_CATEGORY_ARGB) {
                return HWC_COLORFMT_TRANSFORM_YCbCr_ARGB;
            }
            if (enDstCategory == HWC_COLORFMT_CATEGORY_YCbCr) {
                return HWC_COLORFMT_TRANSFORM_YCbCr_YCbCr;
            }
            if (enDstCategory == HWC_COLORFMT_CATEGORY_An) {
                return HWC_COLORFMT_TRANSFORM_ARGB_An;
            }
            return HWC_COLORFMT_TRANSFORM_BUTT;
        case HWC_COLORFMT_CATEGORY_An:
            if (enDstCategory == HWC_COLORFMT_CATEGORY_An) {
                return HWC_COLORFMT_TRANSFORM_An_An;
            }
            return HWC_COLORFMT_TRANSFORM_BUTT;
        default:
            return HWC_COLORFMT_TRANSFORM_BUTT;
    }
}

static hi_u16 GetOptCode(drv_gfx2d_compose_fmt enSrc2Fmt, drv_gfx2d_compose_fmt enDstFmt)
{
    hi_u16 u16Code = 0;
    HWC_COLORFMT_TRANSFORM_E enColorTransType;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    enColorTransType = hwc_OsiGetFmtTransType(enSrc2Fmt, enDstFmt);
    switch (enColorTransType) {
        case HWC_COLORFMT_TRANSFORM_ARGB_ARGB:
            u16Code = 0x0;
            break;

        case HWC_COLORFMT_TRANSFORM_ARGB_YCbCr:
            u16Code = 0x1;
            break;

        case HWC_COLORFMT_TRANSFORM_YCbCr_ARGB:
            u16Code = 0x2;
            break;

        case HWC_COLORFMT_TRANSFORM_YCbCr_YCbCr:
            u16Code = 0x0;
            break;

        default:
            u16Code = 0x0;
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return u16Code;
}

static hi_void set_zme_cfg(GFX2D_HAL_ZME_S *zme, zme_cfg *cfg)
{
    zme->unVhdHsp.stBits.bit1IsLumEnable = HI_TRUE;
    zme->unVhdHsp.stBits.bit1IsChromaEnable = HI_TRUE;
#if HWC_VERSION <= 0x300
    zme->unVhdHsp.stBits.bit1IsNonLinear = 0x0;
#endif
    zme->unVhdHsp.stBits.bit1LumMode = cfg->bFilter;
    zme->unVhdHsp.stBits.bit1ChrmoaMode = cfg->bFilter;
    zme->unVhdHsp.stBits.bit24Ratio = cfg->h_step;
    zme->unVhdHsp.stBits.bit1IsLumMid = (cfg->h_step < GFX2D_HAL_HWC_NOSCALE_HSTEP) ? 0x1 : 0x0;
    zme->unVhdHsp.stBits.bit1IsChromaMid = (cfg->h_step < GFX2D_HAL_HWC_NOSCALE_HSTEP) ? 0x1 : 0x0;

    SetZmeOrder(cfg->format, cfg->out_width, &zme->unVhdHsp);
    zme->unVhdLumHOffset.stBits.bit28Offset = 0x0;
    zme->unVhdChromaHOffset.stBits.bit28Offset = 0x0;

    zme->unVhdVsp.stBits.bit1IsLumEnable = 0x1;
    zme->unVhdVsp.stBits.bit1IsChromaEnable = 0x1;
    zme->unVhdVsp.stBits.bit1IsLumMid = (cfg->v_step < GFX2D_HAL_HWC_NOSCALE_VSTEP) ? 0x1 : 0x0;
    zme->unVhdVsp.stBits.bit1IsChromaMid = (cfg->v_step < GFX2D_HAL_HWC_NOSCALE_VSTEP) ? 0x1 : 0x0;
    zme->unVhdVsp.stBits.bit1LumMode = cfg->bFilter;
    zme->unVhdVsp.stBits.bit1ChromaMode = cfg->bFilter;
    zme->unVhdVsp.stBits.bit2OutFmt = (cfg->format >= DRV_GFX2D_FMT_YUYV422) ? 0x2 : 0x3;

    zme->unVhdVsp.stBits.bit2InFmt = GetZmeInputFmt(cfg->format);

    zme->unVhdZmeInRes.stBits.bit16Width = cfg->in_width - 1;
    zme->unVhdZmeInRes.stBits.bit16Height = cfg->in_height - 1;
    zme->unVhdZmeOutRes.stBits.bit16Width = cfg->out_width - 1;
    zme->unVhdZmeOutRes.stBits.bit16Height = cfg->out_height - 1;
    zme->unRes.stBits.bit16Width = cfg->in_width * cfg->hdwsm_factor - 1;

    zme->u32Hsr = cfg->h_step;
    zme->u32Vsr = cfg->v_step;
    zme->bFilter = cfg->bFilter;
    return;
}

static hi_void covert_zme_info(zme_cfg *hwc_zme_cfg, drv_gfx2d_compose_msg *compose, hi_bool bFilter, hi_u32 u32HStep,
                               hi_u32 u32InWidth)
{
    hi_u32 v_step;
    v_step = (compose->in_rect.height << GFX2D_HAL_HWC_VSTEP_FLOATLEN) / compose->out_rect.height;
    hwc_zme_cfg->bFilter = bFilter;
    hwc_zme_cfg->h_step = u32HStep;
    hwc_zme_cfg->v_step = v_step;
    hwc_zme_cfg->in_width = u32InWidth;
    hwc_zme_cfg->in_height = compose->in_rect.height;
    hwc_zme_cfg->format = compose->compose_surface.format;
    hwc_zme_cfg->out_width = compose->out_rect.width;
    hwc_zme_cfg->out_height = compose->out_rect.height;
    return;
}

static hi_void SetZme(hwc_node *pstNode, drv_gfx2d_compose_msg *compose, HWC_LAYER_ID enLayerId)
{
    hi_u32 u32HStep, u32InWidth, u32InHeight;
    drv_gfx2d_compose_surface *pstSurface = NULL;
    hi_bool bFilter = HI_FALSE;
    hi_u32 HdwsmFactor = 0x1;
    zme_cfg hwc_zme_cfg;
    GFX2D_HAL_ZME_S stZme;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstNode);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(compose);
    pstSurface = &(compose->compose_surface);

    if (enLayerId != HWC_LAYER_VHD && enLayerId != HWC_LAYER_G0 && enLayerId != HWC_LAYER_G1) {
        return;
    }

    memset(&stZme, 0x0, sizeof(GFX2D_HAL_ZME_S));
    /* Scaling rules:
       1. The video format (package 422) zoom switch must be turned on.
          Not supported (input resolution > 1920 &amp; output resolution > 960)
       2. Graphic Layer Format Zoom Switch can be turned on or off as needed. Not supported
          (input resolution > 1920 &amp; output resolution > 1920) */
    if ((pstSurface->format <= DRV_GFX2D_FMT_ARGB10101010) && (!compose->opt.resize.resize_en)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    GetZmeInfo(pstNode, enLayerId, &stZme);

    if (compose->opt.resize.resize_en && (DRV_GFX2D_FILTER_NONE != compose->opt.resize.filter.filter_mode)) {
        bFilter = HI_FALSE;
    }

    u32InWidth = compose->in_rect.width;
    u32InHeight = compose->in_rect.height;

    if ((compose->out_rect.width == 0) || (compose->out_rect.height == 0)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, compose->out_rect.width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, compose->out_rect.height);
        return;
    }
    u32HStep = (u32InWidth << GFX2D_HAL_HWC_HSTEP_FLOATLEN) / compose->out_rect.width;

    /* When the scale is greater than or equal to 16 times, choose n-decimation 1 before zooming, n is determined by
    zooming multiple; when n-decimation 1, image quality will be affected: due to hardware constraints, the right side
    may lose points.
    In video format, (input resolution > 1920 &amp; output resolution > 960), horizontal point extraction is performed
    first.
    In graphics format, (input resolution > 1920 &amp; output resolution > 1920), the horizontal point extraction is
    performed first. */
    if (u32HStep >= (16 << GFX2D_HAL_HWC_HSTEP_FLOATLEN)) {                  /* 16 algorithm data */
        HdwsmFactor = ((u32HStep >> GFX2D_HAL_HWC_HSTEP_FLOATLEN) / 16 + 1); /* 16,1 algorithm data */
        HdwsmFactor = (HdwsmFactor + 1) / 2 * 2;                             /* 2 algorithm data */

        GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(0, HdwsmFactor);
        u32InWidth = ((u32InWidth / HdwsmFactor) & ~(g_gfx2d_capability.u8ZmeAlign - 1));

        u32HStep = (u32InWidth << GFX2D_HAL_HWC_HSTEP_FLOATLEN) / compose->out_rect.width;

        stZme.unHpZme.stBits.bit1_src2_hpzme_en = HI_TRUE;
        stZme.unHpZme.stBits.bit4_src2_hpzme_mode = HdwsmFactor - 1;
        stZme.unHpZme.stBits.bit16_src2_hpzme_width = u32InWidth;
    }

    hwc_zme_cfg.hdwsm_factor = HdwsmFactor;
    covert_zme_info(&hwc_zme_cfg, compose, bFilter, u32HStep, u32InWidth);
    set_zme_cfg(&stZme, &hwc_zme_cfg);

    SetZmeInfo(pstNode, enLayerId, &stZme);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
static hi_void TraceCfg(drv_gfx2d_compose_list *compose_list)
{
    hi_u32 i = 0; /* 0 algorithm data */
    drv_gfx2d_compose_msg *compose = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(compose_list);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_NOVALUE(compose_list->compose_cnt, GFX2D_MAX_LAYERS);

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");
    for (i = 0; i < compose_list->compose_cnt; i++) {
        compose = &compose_list->compose[i];
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, i);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->compose_surface.surface_type);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->opt.resize.resize_en);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->in_rect.width);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->in_rect.height);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->out_rect.width);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->out_rect.width);
    }
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_bool CheckVhdFormat(drv_gfx2d_compose_surface *pstSurface)
{
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstSurface, HI_FALSE);
    return ((DRV_GFX2D_SURFACE_TYPE_MEM == pstSurface->surface_type) && (DRV_GFX2D_FMT_YUV888 <= pstSurface->format));
}

static hi_bool CheckResizeEnable(drv_gfx2d_compose_surface *pstSurface, drv_gfx2d_compose_opt *pstOpt)
{
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstSurface, HI_FALSE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstOpt, HI_FALSE);
    return ((DRV_GFX2D_SURFACE_TYPE_MEM == pstSurface->surface_type) && (pstOpt->resize.resize_en));
}

static HWC_LAYER_ID SelectZmeLayer(hi_u8 *pu8LayerRefRecord, hi_u8 max_layer_num)
{
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pu8LayerRefRecord, HWC_LAYER_BUTT);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(HWC_LAYER_G0, max_layer_num, HWC_LAYER_BUTT);
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(HWC_LAYER_VHD, max_layer_num, HWC_LAYER_BUTT);
    /* If the graphics layer does not support zooming, the video layer is selected. */
    if (!g_gfx2d_capability.bGZmeSupport) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HWC_LAYER_VHD;
    } else {
        if (pu8LayerRefRecord[HWC_LAYER_G0] == 0) {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HWC_LAYER_G0;
        } else if (pu8LayerRefRecord[HWC_LAYER_G1] == 0) {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HWC_LAYER_G1;
        } else if (pu8LayerRefRecord[HWC_LAYER_VHD] == 0) {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HWC_LAYER_VHD;
        } else {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HWC_LAYER_BUTT;
        }
    }
}

static HWC_LAYER_ID SelectLayer(hi_u8 *pu8LayerRefRecord, hi_u8 layer_num)
{
    hi_u8 i;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pu8LayerRefRecord, HWC_LAYER_BUTT);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(HWC_LAYER_G8, layer_num, HWC_LAYER_BUTT);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(HWC_LAYER_G0, layer_num, HWC_LAYER_BUTT);
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(HWC_LAYER_VHD, layer_num, HWC_LAYER_BUTT);

    /* Priority is given to those without zooming, and then to those with zooming. */
    for (i = HWC_LAYER_G2; i <= HWC_LAYER_G6; i++) {
        if (pu8LayerRefRecord[i] == 0) {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return i;
        }
    }

    if (pu8LayerRefRecord[HWC_LAYER_G1] == 0) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HWC_LAYER_G1;
    } else if (pu8LayerRefRecord[HWC_LAYER_G0] == 0) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HWC_LAYER_G0;
    } else if (pu8LayerRefRecord[HWC_LAYER_VHD] == 0) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HWC_LAYER_VHD;
    } else {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HWC_LAYER_BUTT;
    }
}

static hi_bool CheckSelectResultInvalid(HWC_LAYER_ID enLayerId, drv_gfx2d_compose_msg *compose)
{
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(compose, HI_FALSE);
    return ((enLayerId == HWC_LAYER_VHD) &&
            ((compose->in_rect.width < g_gfx2d_capability.u32MinVWidth) ||
             (compose->in_rect.height < g_gfx2d_capability.u32MinVHeight)) &&
            (DRV_GFX2D_SURFACE_TYPE_MEM == compose->compose_surface.surface_type) &&
            (DRV_GFX2D_FMT_YUV888 <= compose->compose_surface.format));
}

static hi_s32 select_layer_id(drv_gfx2d_compose_list *compose_list, HWC_LAYER_ID *layer_id, hi_u32 layer_num)
{
    hi_s32 i;
    drv_gfx2d_compose_msg *compose = NULL;
    hi_u8 ref[HWC_LAYER_BUTT] = {0};

    for (i = 0; i < compose_list->compose_cnt; i++) {
        compose = &(compose_list->compose[i]);
        layer_id[i] = HWC_LAYER_BUTT;

        layer_id[i] = select_layer_id_from_features(compose, ref, sizeof(ref) / sizeof(ref[0]));
        if (layer_id[i] == HWC_LAYER_BUTT) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "unsupport");
            TraceCfg(compose_list);
            return DRV_GFX2D_ERR_UNSUPPORT;
        }

        if (ref[layer_id[i]] > 0) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "this layer has been seleced");
            TraceCfg(compose_list);
            return DRV_GFX2D_ERR_UNSUPPORT;
        }

        if (CheckSelectResultInvalid(layer_id[i], compose) == HI_TRUE) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "this video layer no support");
            TraceCfg(compose_list);
            return DRV_GFX2D_ERR_UNSUPPORT;
        }

        ref[layer_id[i]]++;
    }
    return HI_SUCCESS;
}
/* According to the application configuration, select the appropriate layer and select the principle:
   If the pixel format is YUV format, then the video layer VHD is used.
   If zooming is required, the G0 layer is preferred, and then the video layer is preferred.
   Otherwise, select unused graphics layers in turn: G1, G2, G3, G4, G5, G0 */
static hi_s32 SelectLayerId(drv_gfx2d_compose_list *compose_list, HWC_LAYER_ID *penLayerId, hi_u32 layer_num)
{
    hi_s32 i, k;
    drv_gfx2d_compose_msg *compose = NULL;
    hi_u8 au8Ref[HWC_LAYER_G8 + 1] = {0};

    for (i = 0; i < compose_list->compose_cnt; i++) {
        compose = &(compose_list->compose[i]);
        if (CheckVhdFormat(&compose->compose_surface)) {
            /* YUV format, video layer must be used */
            penLayerId[i] = HWC_LAYER_VHD;
        } else if (CheckResizeEnable(&compose->compose_surface, &compose->opt)) {
            /* If the graphics layer does not support zooming, the G0 layer is selected. */
            penLayerId[i] = SelectZmeLayer(au8Ref, HWC_LAYER_G8 + 1);
            if (penLayerId[i] == HWC_LAYER_BUTT) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, i);
                goto ERR;
            }
        } else {
            penLayerId[i] = SelectLayer(au8Ref, HWC_LAYER_G8 + 1);
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, penLayerId[i]);
            if (penLayerId[i] == HWC_LAYER_BUTT) {
                goto ERR;
            }
        }
        if (au8Ref[penLayerId[i]] > 0) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "this layer has been seleced");
            goto ERR;
        }

        if (HI_TRUE == CheckSelectResultInvalid(penLayerId[i], compose)) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "this video layer no support");
            goto ERR;
        }

        if ((penLayerId[i] == HWC_LAYER_VHD) &&
            (compose->compose_surface.surface_type == DRV_GFX2D_SURFACE_TYPE_COLOR)) {
            /* Only the graphics layer can choose the color type */
            for (k = i - 1; k >= 0; k--) {
                if ((compose_list->compose[k].in_rect.width  >= g_gfx2d_capability.u32MinVWidth) &&
                    (compose_list->compose[k].in_rect.height >= g_gfx2d_capability.u32MinVHeight) &&
                    (compose_list->compose[k].compose_surface.surface_type == DRV_GFX2D_SURFACE_TYPE_MEM)) {
                    penLayerId[i] = penLayerId[k];
                    penLayerId[k] = HWC_LAYER_VHD;
                    au8Ref[HWC_LAYER_VHD]++;
                    break;
                }
            }
            if (k < 0) {
                goto ERR;
            }
        } else {
            au8Ref[penLayerId[i]]++;
        }
    }
    return HI_SUCCESS;

ERR:
    TraceCfg(compose_list);
    GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "unsupport");
    return DRV_GFX2D_ERR_UNSUPPORT;
}

typedef struct {
    HWC_SRCCTL_U unCtrl;
    HWC_RES_U unRes;
    HWC_ALPHA_U unAlpha;
    HWC_POS_U unOutStartPos;
    HWC_CBMPARA_U unCbmPara;
    HWC_MMU_PROT_CTL_U unMmuCtl;
} gfx_layer_info;

typedef struct {
    hi_u32 phy_addr;
    hi_u32 stride;
    hi_u32 u32DimValue;
} gfx_layer_ext_info;

static hi_void covert_layer(hwc_node *pstNode, HWC_LAYER_ID enLayerId, gfx_layer **pstGLayer, video_layer **src_layer,
                            gfx_layer_info *gfx_layer)
{
    hi_u32 u32Index;
    if (enLayerId == HWC_LAYER_G0) {
        u32Index = 0;
        *src_layer = &(pstNode->src_Layer[u32Index]);
        gfx_layer->unCtrl.u32All = (*src_layer)->unCtrl.u32All;
        gfx_layer->unRes.u32All = (*src_layer)->unRes.u32All;
        gfx_layer->unAlpha.u32All = (*src_layer)->unAlpha.u32All;
        gfx_layer->unOutStartPos.u32All = pstNode->cbm_info[enLayerId - SRC_TWO].unCbmStartPos.u32All;
        gfx_layer->unCbmPara.u32All = pstNode->cbm_info[enLayerId - SRC_TWO].unCbmPara.u32All;
    } else {
        u32Index = enLayerId - SRC_THREE;
        *pstGLayer = &(pstNode->astLayer[u32Index]);
        gfx_layer->unCtrl.u32All = (*pstGLayer)->unCtrl.u32All;
        gfx_layer->unRes.u32All = (*pstGLayer)->unRes.u32All;
        gfx_layer->unAlpha.u32All = (*pstGLayer)->unAlpha.u32All;
        gfx_layer->unOutStartPos.u32All = pstNode->cbm_info[enLayerId - 1].unCbmStartPos.u32All;
        gfx_layer->unCbmPara.u32All = pstNode->cbm_info[enLayerId - 1].unCbmPara.u32All;
    }
    gfx_layer->unMmuCtl.u32All = pstNode->unMmuCtl.u32All;

#ifdef CONFIG_GFX_MMU_SUPPORT
    gfx_layer->unMmuCtl.u32All = 0xffff0000;
#else
    gfx_layer->unMmuCtl.u32All = 0x1fff03ff;
#endif
    return;
}

static hi_void set_layer_base_info(hwc_node *pstNode, HWC_LAYER_ID enLayerId, gfx_layer *pstGLayer,
                                   video_layer *src_layer, gfx_layer_info *gfx_layer)
{
    if (enLayerId == HWC_LAYER_G0) {
        src_layer->unCtrl.u32All = gfx_layer->unCtrl.u32All;
        src_layer->unRes.u32All = gfx_layer->unRes.u32All;
        src_layer->unAlpha.u32All = gfx_layer->unAlpha.u32All;
        pstNode->cbm_info[enLayerId - SRC_TWO].unCbmPara.u32All = gfx_layer->unCbmPara.u32All;
        pstNode->cbm_info[enLayerId - SRC_TWO].unCbmStartPos.u32All = gfx_layer->unOutStartPos.u32All;
    } else {
        pstGLayer->unCtrl.u32All = gfx_layer->unCtrl.u32All;
        pstGLayer->unRes.u32All = gfx_layer->unRes.u32All;
        pstGLayer->unAlpha.u32All = gfx_layer->unAlpha.u32All;
        pstNode->cbm_info[enLayerId - 1].unCbmPara.u32All = gfx_layer->unCbmPara.u32All;
        pstNode->cbm_info[enLayerId - 1].unCbmStartPos.u32All = gfx_layer->unOutStartPos.u32All;
    }
    return;
}

static hi_void set_layer_ext_info(HWC_LAYER_ID enLayerId, gfx_layer *pstGLayer, video_layer *src_layer,
                                  gfx_layer_ext_info *layer_ext_info)
{
    if (enLayerId == HWC_LAYER_G0) {
        src_layer->phy_addr = layer_ext_info->phy_addr;
        src_layer->stride = layer_ext_info->stride;
        src_layer->u32DimValue = layer_ext_info->u32DimValue;
    } else {
        pstGLayer->phy_addr = layer_ext_info->phy_addr;
        pstGLayer->stride = layer_ext_info->stride;
        pstGLayer->u32DimValue = layer_ext_info->u32DimValue;
    }
    return;
}

static hi_void set_gfx_layer_info(gfx_layer_info *gfx_layer, drv_gfx2d_compose_msg *compose,
                                  drv_gfx2d_compose_surface *pstSurface)
{
    gfx_layer->unCtrl.stBits.bit2ExpandMode = 0x0;

    gfx_layer->unCbmPara.stBits.bit8GlobalAlpha = 0xff;
    if (compose->opt.blend.global_alpha_en) {
        gfx_layer->unCbmPara.stBits.bit8GlobalAlpha = compose->opt.blend.global_alpha;
    }

    gfx_layer->unCbmPara.stBits.bit1PixelAlphaEnable = compose->opt.blend.pixel_alpha_en;
    if (!gfx_layer->unCbmPara.stBits.bit1PixelAlphaEnable && gfx_layer->unCtrl.stBits.bit6Fmt == 0x0) {
        gfx_layer->unCtrl.stBits.bit6Fmt = 0x0;
    }

    gfx_layer->unCbmPara.stBits.bit1bCovBlend = compose->opt.blend.cov_blend;
    if (gfx_layer->unCbmPara.stBits.bit1bCovBlend) {
        gfx_layer->unCbmPara.stBits.bit1PixelAlphaEnable = 0x1;
    }

    if (DRV_GFX2D_SURFACE_TYPE_COLOR == pstSurface->surface_type) {
        gfx_layer->unRes.stBits.bit16Width = compose->out_rect.width - 1;
        gfx_layer->unRes.stBits.bit16Height = compose->out_rect.height - 1;
    } else {
        gfx_layer->unRes.stBits.bit16Width = compose->in_rect.width - 1;
        gfx_layer->unRes.stBits.bit16Height = compose->in_rect.height - 1;
    }
    gfx_layer->unOutStartPos.stBits.bit16X = compose->out_rect.x;
    gfx_layer->unOutStartPos.stBits.bit16Y = compose->out_rect.y;
    return;
}

static hi_void SetGLayer(HWC_LAYER_ID enLayerId, hwc_node *pstNode, drv_gfx2d_compose_msg *compose)
{
    gfx_layer_ext_info layer_ext_info = {0};
    hi_u8 u8Bpp = 0;
    drv_gfx2d_compose_surface *pstSurface = NULL;
    gfx_layer *pstGLayer = HI_NULL;
    video_layer *src_layer = HI_NULL;
    gfx_layer_info gfx_layer = {{{0}}};

    pstSurface = &(compose->compose_surface);
    covert_layer(pstNode, enLayerId, &pstGLayer, &src_layer, &gfx_layer);
    gfx_layer.unCtrl.stBits.bit1IsEnable = 0x1;
    if (DRV_GFX2D_SURFACE_TYPE_COLOR == pstSurface->surface_type) {
        gfx_layer.unCtrl.stBits.bit1IsDim = 0x1;
        layer_ext_info.u32DimValue = pstSurface->color_data;
    } else {
        gfx_layer.unCtrl.stBits.bit1IsDim = 0x0;
        GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(pstSurface->format, DRV_GFX2D_FMT_PLANAR444 + 1);
        gfx_layer.unCtrl.stBits.bit6Fmt = get_gfx2d_layer_reg_format(pstSurface->format);
        gfx_layer.unCtrl.stBits.bit5ArgbOrder = get_gfx2d_layer_reg_argb_order(pstSurface->format);

        u8Bpp = GetBpp(pstSurface->format, HI_FALSE);
        layer_ext_info.phy_addr = pstSurface->phy_addr[0] + compose->in_rect.x * u8Bpp +
                                  compose->in_rect.y * pstSurface->stride[0];
        layer_ext_info.stride = pstSurface->stride[0];
    }

    gfx_layer.unCbmPara.stBits.bit1bPreMul = pstSurface->premulti_en;
    if (pstSurface->alpha_ext.alpha_ext_en) {
        gfx_layer.unAlpha.stBits.bit8Alpha0 = pstSurface->alpha_ext.alpha0;
        gfx_layer.unAlpha.stBits.bit8Alpha1 = pstSurface->alpha_ext.alpha1;
    } else {
        gfx_layer.unAlpha.stBits.bit8Alpha0 = 0x0;
        gfx_layer.unAlpha.stBits.bit8Alpha1 = 0xff;
    }
    set_gfx_layer_info(&gfx_layer, compose, pstSurface);
    pstNode->unMmuCtl.u32All = gfx_layer.unMmuCtl.u32All;
    set_layer_base_info(pstNode, enLayerId, pstGLayer, src_layer, &gfx_layer);
    set_layer_ext_info(enLayerId, pstGLayer, src_layer, &layer_ext_info);

    if (DRV_GFX2D_SURFACE_TYPE_COLOR != pstSurface->surface_type) {
        SetZme(pstNode, compose, enLayerId);
    }

    return;
}

static inline hi_bool CheckReverseUVOrder(drv_gfx2d_compose_fmt format)
{
    return ((DRV_GFX2D_FMT_SEMIPLANAR422VU_H == format) || (DRV_GFX2D_FMT_SEMIPLANAR422VU_V == format) ||
            (DRV_GFX2D_FMT_SEMIPLANAR420VU == format));
}

static hi_void GetSampleFactor(drv_gfx2d_compose_fmt format, hi_u8 *pu8HorizonSampleFactor,
                               hi_u8 *pu8VerticalSampleFactor)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pu8HorizonSampleFactor);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pu8VerticalSampleFactor);

    if ((DRV_GFX2D_FMT_PLANAR420 == format) || (DRV_GFX2D_FMT_SEMIPLANAR420UV == format) ||
        (DRV_GFX2D_FMT_SEMIPLANAR420VU == format)) {
        *pu8HorizonSampleFactor = 2;  /* 2 algorithm data */
        *pu8VerticalSampleFactor = 2; /* 2 algorithm data */
    } else if (DRV_GFX2D_FMT_PLANAR444 == format) {
        *pu8HorizonSampleFactor = 1;  /* 1 algorithm data */
        *pu8VerticalSampleFactor = 1; /* 1 algorithm data */
    } else if (DRV_GFX2D_FMT_PLANAR410 == format) {
        *pu8HorizonSampleFactor = 4;  /* 4 algorithm data */
        *pu8VerticalSampleFactor = 4; /* 4 algorithm data */
    } else if (DRV_GFX2D_FMT_PLANAR411 == format) {
        *pu8HorizonSampleFactor = 4;  /* 4 algorithm data */
        *pu8VerticalSampleFactor = 1; /* 1 algorithm data */
    } else if ((DRV_GFX2D_FMT_PLANAR422H == format) || (DRV_GFX2D_FMT_SEMIPLANAR422UV_H == format) ||
               (DRV_GFX2D_FMT_SEMIPLANAR422VU_H == format)) {
        *pu8HorizonSampleFactor = 2;  /* 2 algorithm data */
        *pu8VerticalSampleFactor = 1; /* 1 algorithm data */
    } else if ((DRV_GFX2D_FMT_PLANAR422V == format) || (DRV_GFX2D_FMT_SEMIPLANAR422UV_V == format) ||
               (DRV_GFX2D_FMT_SEMIPLANAR422VU_V == format)) {
        *pu8HorizonSampleFactor = 1;  /* 1 algorithm data */
        *pu8VerticalSampleFactor = 2; /* 2 algorithm data */
    } else {
        *pu8HorizonSampleFactor = 1;  /* 1 algorithm data */
        *pu8VerticalSampleFactor = 1; /* 1 algorithm data */
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

static hi_void set_vhd_layer_base_info(hwc_node *pstNode, hi_u32 u32Index, HWC_LAYER_ID enLayerId,
                                       drv_gfx2d_compose_surface *pstSurface, drv_gfx2d_compose_msg *compose)
{
    pstNode->src_Layer[u32Index].stride = pstSurface->stride[0];  /* index is 1 */
    pstNode->src_Layer[u32Index].stride1 = pstSurface->stride[1]; /* index is 1 */

    pstNode->src_Layer[u32Index].unRes.stBits.bit16Width = compose->in_rect.width - 1;   /* index is 1 */
    pstNode->src_Layer[u32Index].unRes.stBits.bit16Height = compose->in_rect.height - 1; /* index is 1 */

    pstNode->cbm_info[enLayerId].unCbmPara.stBits.bit8GlobalAlpha = 0xff;
    if (compose->opt.blend.global_alpha_en) {
        pstNode->cbm_info[enLayerId].unCbmPara.stBits.bit8GlobalAlpha = compose->opt.blend.global_alpha;
    }
    pstNode->cbm_info[enLayerId].unCbmPara.stBits.bit1bCovBlend = compose->opt.blend.cov_blend;
    pstNode->cbm_info[enLayerId].unCbmPara.stBits.bit1PixelAlphaEnable = compose->opt.blend.pixel_alpha_en;
    pstNode->cbm_info[enLayerId].unCbmPara.stBits.bit1bPreMul = pstSurface->premulti_en;
    if (pstSurface->alpha_ext.alpha_ext_en) {
        pstNode->src_Layer[u32Index].unAlpha.stBits.bit8Alpha0 = pstSurface->alpha_ext.alpha0;
        pstNode->src_Layer[u32Index].unAlpha.stBits.bit8Alpha1 = pstSurface->alpha_ext.alpha1;
    } else {
        pstNode->src_Layer[u32Index].unAlpha.stBits.bit8Alpha0 = 0x0;
        pstNode->src_Layer[u32Index].unAlpha.stBits.bit8Alpha1 = 0xff;
    }

    pstNode->cbm_info[enLayerId].unCbmStartPos.stBits.bit16X = compose->out_rect.x;
    pstNode->cbm_info[enLayerId].unCbmStartPos.stBits.bit16Y = compose->out_rect.y;
    return;
}

static hi_void SetVHDLayer(HWC_LAYER_ID enLayerId, hwc_node *pstNode, drv_gfx2d_compose_msg *compose)
{
    hi_u8 u8Bpp;
    hi_u8 u8HorizonSampleFactor = 1;  /* 1 algorithm data */
    hi_u8 u8VerticalSampleFactor = 1; /* 1 algorithm data */
    drv_gfx2d_compose_surface *pstSurface = NULL;
    hi_u32 u32Index = 1;
    pstSurface = &(compose->compose_surface);

    pstNode->src_Layer[u32Index].unCtrl.stBits.bit1IsEnable = HI_TRUE;
    pstNode->src_Layer[u32Index].unCtrl.stBits.bit2ExpandMode = 0x0;
    pstNode->src_Layer[u32Index].unCtrl.stBits.bit1422Pro = 0x1;

    if (CheckReverseUVOrder(pstSurface->format)) {
        pstNode->src_Layer[u32Index].unCtrl.stBits.bit1UVOrder = 0x1;
    }

    if (pstSurface->format < GFX2D_ARRAY_SIZE(g_gfx2d_layer_format)) {
        pstNode->src_Layer[u32Index].unCtrl.stBits.bit6Fmt = get_gfx2d_layer_reg_format(pstSurface->format);
        pstNode->src_Layer[u32Index].unCtrl.stBits.bit5ArgbOrder = get_gfx2d_layer_reg_argb_order(pstSurface->format);
    }

#ifdef CONFIG_GFX_MMU_SUPPORT
    pstNode->unMmuCtl.u32All = 0xffff0000;
#else
    pstNode->unMmuCtl.u32All = 0x1fff03ff;
#endif

    u8Bpp = GetBpp(pstSurface->format, HI_TRUE);

    GetSampleFactor(pstSurface->format, &u8HorizonSampleFactor, &u8VerticalSampleFactor);
    if (DRV_GFX2D_SURFACE_TYPE_COLOR == pstSurface->surface_type) {
        pstNode->src_Layer[u32Index].unCtrl.stBits.bit1_dma = 0x1; /* src2 dim in bit29, diff with other srcs */
        pstNode->src_Layer[u32Index].u32DimValue = pstSurface->color_data;
    } else {
        pstNode->src_Layer[u32Index].unCtrl.stBits.bit1_dma = 0x0;
    }

    pstNode->src_Layer[u32Index].phy_addr = pstSurface->phy_addr[0] + compose->in_rect.y * pstSurface->stride[0] +
                                            compose->in_rect.x * u8Bpp;
    u8Bpp = GetBpp(pstSurface->format, HI_FALSE);
    if ((pstSurface->format >= DRV_GFX2D_FMT_SEMIPLANAR420UV) && (DRV_GFX2D_FMT_PLANAR400 != pstSurface->format)) {
        GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(0, u8VerticalSampleFactor);
        GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(0, u8HorizonSampleFactor);
        pstNode->src_Layer[u32Index].ch1_addr_addr_l = pstSurface->phy_addr[1] + /* index is 1 */
                                                       (compose->in_rect.y / u8VerticalSampleFactor) *
                                                           pstSurface->stride[0] +
                                                       (compose->in_rect.x / u8HorizonSampleFactor) * u8Bpp;
    }

    set_vhd_layer_base_info(pstNode, u32Index, enLayerId, pstSurface, compose);
    SetZme(pstNode, compose, enLayerId);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/* HWC_LAYER_ID conversion table to hardware layer */
hi_u32 g_gfx2d_layer_to_hal_table[] = { 0x0, 0x2, 0x1, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa };

static hi_void set_layer_zorder(hwc_node *pstNode, hi_u32 u32Zorder, HWC_LAYER_ID enLayerId)
{
    switch (u32Zorder) {
        case 0: /* index is 0 */
            pstNode->cbm_ctrl.bits.cbm_mix_prio0 = g_gfx2d_layer_to_hal_table[enLayerId];
            break;
        case 1: /* cbm_mix_prio1 */
            pstNode->cbm_ctrl.bits.cbm_mix_prio1 = g_gfx2d_layer_to_hal_table[enLayerId];
            break;
        case 2: /* cbm_mix_prio2 */
            pstNode->cbm_ctrl.bits.cbm_mix_prio2 = g_gfx2d_layer_to_hal_table[enLayerId];
            break;
        case 3: /* cbm_mix_prio3 */
            pstNode->cbmprio.bits.cbm_mix_prio3 = g_gfx2d_layer_to_hal_table[enLayerId];
            break;
        case 4: /* cbm_mix_prio4 */
            pstNode->cbmprio.bits.cbm_mix_prio4 = g_gfx2d_layer_to_hal_table[enLayerId];
            break;
        case 5: /* cbm_mix_prio5 */
            pstNode->cbmprio.bits.cbm_mix_prio5 = g_gfx2d_layer_to_hal_table[enLayerId];
            break;
        case 6: /* cbm_mix_prio6 */
            pstNode->cbmprio.bits.cbm_mix_prio6 = g_gfx2d_layer_to_hal_table[enLayerId];
            break;
        case 7: { /* cbm_mix_prio7 */
            pstNode->cbmprio.bits.cbm_mix_prio7 = g_gfx2d_layer_to_hal_table[enLayerId];
            break;
        }
        case 8: { /* cbm_mix_prio8 */
            pstNode->cbmprio.bits.cbm_mix_prio8 = g_gfx2d_layer_to_hal_table[enLayerId];
            break;
        }
        case 9: { /* cbm_mix_prio9 */
            pstNode->cbmprio.bits.cbm_mix_prio9 = g_gfx2d_layer_to_hal_table[enLayerId];
            break;
        }
        default:
            break;
    }
    return;
}

static hi_void set_layer_cbm_enable(hwc_node *pstNode, HWC_LAYER_ID enLayerId)
{
    switch (enLayerId) {
        case 1: /* cbm_src1_en  */
            pstNode->cbm_ctrl.bits.cbm_src2_en = 1;
            break;
        case 2: /* cbm_src2_en */
            pstNode->cbm_ctrl.bits.cbm_src1_en = 1;
            break;
        case 3: /* cbm_src3_en */
            pstNode->cbm_ctrl.bits.cbm_src3_en = 1;
            break;
        case 4: /* cbm_src4_en */
            pstNode->cbm_ctrl.bits.cbm_src4_en = 1;
            break;
        case 5: /* cbm_src5_en */
            pstNode->cbm_ctrl.bits.cbm_src5_en = 1;
            break;
        case 6: /* cbm_src6_en */
            pstNode->cbm_ctrl.bits.cbm_src6_en = 1;
            break;
        case 7: /* cbm_src7_en */
            pstNode->cbm_ctrl.bits.cbm_src7_en = 1;
            break;
        case 8: /* cbm_src8_en */
            pstNode->cbm_ctrl.bits.cbm_src8_en = 1;
            break;
        case 9: /* cbm_src9_en */
            pstNode->cbm_ctrl.bits.cbm_src9_en = 1;
            break;
        case 10: /* cbm_src10_en */
            pstNode->cbm_ctrl.bits.cbm_src10_en = 1;
            break;
        default:
            break;
    }
    return;
}

static hi_void SetSrc(hwc_node *pstNode, drv_gfx2d_compose_msg *compose, hi_u32 u32Zorder, HWC_LAYER_ID enLayerId)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstNode);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(compose);

    if (enLayerId >= GFX2D_ARRAY_SIZE(g_gfx2d_layer_to_hal_table)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }
    if (enLayerId >= HWC_LAYER_G0) {
        SetGLayer(enLayerId, pstNode, compose);
    } else {
        SetVHDLayer(enLayerId, pstNode, compose);
    }

    set_layer_zorder(pstNode, u32Zorder, enLayerId);
    set_layer_cbm_enable(pstNode, enLayerId);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static inline hi_void SetBgColor(hwc_node *pstNode, hi_u32 back_color)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstNode);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_void PrintNodeInfo(hwc_node *pstNode)
{
    return;
}

#define GFX_WRITE_REG(base, offset, val)                                 \
    if (((base) != NULL) && ((offset) <= (REG_SIZE - 4))) {              \
        (*(volatile unsigned int *)((void *)(base) + (offset)) = (val)); \
    }

#define GFX_READ_REG(base, offset) ((base) == NULL) ? (0) : (*(volatile unsigned int *)((void *)(base) + (offset)))

#define AQ_NADDR_LOW 0x1F8
#define AQ_NADDR_HIGH 0x1FC
#define CTRL 0x0500
#define MASK_INT 0x0514
#define CLEAR_INT 0x0510
#define OUTSTANDING 0x0600

hi_s32 GFX2D_HAL_HWC_SubNode(drv_gfx2d_dev_id dev_id, hi_void *pNode)
{
    hwc_node *pstNode = (hwc_node *)pNode;
    hi_u64 addr;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstNode, HI_FAILURE);

    while (pstNode != NULL) {
        PrintNodeInfo(pstNode);
        pstNode = pstNode->pNextNode;
    }

    pstNode = (hwc_node *)pNode;

    /* call initreg here instead in Init() to reduce power cost */
    InitCrgReg();

    /* write to the first node */
    addr = GFX2D_MEM_GetPhyaddr(pstNode);

    GFX_WRITE_REG(g_hwc_reg, OUTSTANDING, 0x77dde);
    GFX_WRITE_REG(g_hwc_reg, AQ_NADDR_HIGH, HI_GFX_GET_HIGH_PART(addr));
    GFX_WRITE_REG(g_hwc_reg, AQ_NADDR_LOW, HI_GFX_GET_LOW_PART(addr));
    GFX_WRITE_REG(g_hwc_reg, CTRL, 0x1);
    osal_isb();
    osal_smp_mb();

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 GFX2D_HAL_HWC_LinkNode(hi_void *pCurNode, hi_void *pNextNode)
{
    hwc_node *pstCurNode = (hwc_node *)pCurNode;
    hwc_node *pstNextNode = (hwc_node *)pNextNode;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pCurNode, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pNextNode, HI_FAILURE);

    /* If curtask request historam info,we can not link curtask and nexttask,
    for nexttask may overlay the info! */
#ifdef GFX2D_HISTOGRAM_SUPPORT
    if (pstCurNode->pu32Histogram != NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, pstCurNode->pu32Histogram);
        return HI_FAILURE;
    }
#endif

    while (pstCurNode->pNextNode != NULL) {
        pstCurNode = pstCurNode->pNextNode;
    }

    pstCurNode->tde_pnext_hi.bits.p_next_hi = HI_GFX_GET_HIGH_PART(GFX2D_MEM_GetPhyaddr(pstNextNode));

    pstCurNode->next_node_phyaddr = HI_GFX_GET_LOW_PART(GFX2D_MEM_GetPhyaddr(pstNextNode));
    if (pstCurNode->next_node_phyaddr == 0 && pstCurNode->tde_pnext_hi.bits.p_next_hi == 0) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, pstCurNode->next_node_phyaddr);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstNextNode);
    }
    pstCurNode->pNextNode = pstNextNode;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_void GFX2D_HAL_HWC_SetNodeID(hi_void *pNode, hi_u32 u32NodeId)
{
    hwc_node *pstNode = (hwc_node *)pNode;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pNode);

    pstNode->u32NodeId = u32NodeId;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_void GFX2D_HAL_HWC_FreeNode(hi_void *pNode)
{
    hwc_node *pstCurNode = (hwc_node *)pNode;
    hwc_node *pstNextNode = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pNode);

    while ((pstCurNode != NULL) && (pstCurNode->pFirstNode == pNode)) {
        pstNextNode = (hwc_node *)pstCurNode->pNextNode;
        GFX2D_MEM_Free(pstCurNode);
        pstCurNode = pstNextNode;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_void GFX2D_HAL_HWC_NodeIsr(hi_void *pNode)
{
#ifdef GFX2D_HISTOGRAM_SUPPORT
#if 0
    hwc_node *pstNode = (hwc_node *)pNode;
    hi_u32 i = 0; /* 0 algorithm data */
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if ((pstNode == HI_NULL) || (g_pst_hwc_reg == HI_NULL)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstNode);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, g_pst_hwc_reg);
        return;
    }

    if (pstNode->pu32Histogram != NULL) {
        for (i = 0; i < 16; i++) { /* 16, algorithm data */
            pstNode->pu32Histogram[i] = g_pst_hwc_reg->u32Histogram[i];
        }
        unmap_mmb_2(pstNode->pu32Histogram, pstNode->u32Offset);
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
#endif
#endif
    return;
}

hi_void GFX2D_HAL_HWC_AllNodeIsr(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    DeinitCrgReg();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

GFX2D_HAL_NODE_OPS_S g_stHwcNodeOps = {
    .pfnSubNode = GFX2D_HAL_HWC_SubNode,
    .pfnLinkNode = GFX2D_HAL_HWC_LinkNode,
    .pfnSetNodeID = GFX2D_HAL_HWC_SetNodeID,
    .pfnFreeNode = GFX2D_HAL_HWC_FreeNode,
    .pfnNodeIsr = GFX2D_HAL_HWC_NodeIsr,
    .pfnAllNodeIsr = GFX2D_HAL_HWC_AllNodeIsr,
};

hi_void GFX2D_HAL_HWC_GetNodeOps(GFX2D_HAL_NODE_OPS_S **ppstNodeOps)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(ppstNodeOps);

    *ppstNodeOps = &g_stHwcNodeOps;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_u32 GFX2D_HAL_HWC_GetIntStatus(hi_void)
{
    hi_u32 u32IntStatus;
    hi_u32 u32Status = 0; /* 0 algorithm data */
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (g_hwc_reg == HI_NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, g_hwc_reg);
        return u32Status;
    }

    u32IntStatus = GFX_READ_REG(g_hwc_reg, MASK_INT);
    /* clear interrupt */
    GFX_WRITE_REG(g_hwc_reg, CLEAR_INT, 0xe);

    if (u32IntStatus & 0x4) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32IntStatus);
    }

    if (u32IntStatus & 0x2) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32IntStatus);
    }

    if (u32IntStatus & 0x8) {
        u32Status |= 0x1;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return u32Status;
}

hi_u32 GFX2D_HAL_HWC_GetIsrNum(hi_void)
{
    return HWC_ISRNUM;
}

hi_u32 GFX2D_HAL_HWC_GetBaseAddr(hi_void)
{
    return HWC_REG_BASE_ADDR;
}

/* Use binary conversion tools to see which pixel format is supported, 1 is supported, 0 is not supported. */
#define IS_BIT_SET(bit, value) (0 == ((0x1 << (bit)) & (value)) ? (HI_FALSE) : (HI_TRUE))

static hi_s32 CheckSurfaceType(drv_gfx2d_compose_surface *pstSurface, hi_bool bInputSurface)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstSurface, HI_FAILURE);

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, bInputSurface);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->surface_type);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, DRV_GFX2D_SURFACE_TYPE_COLOR);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, DRV_GFX2D_SURFACE_TYPE_MEM);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");

    if (DRV_GFX2D_SURFACE_TYPE_COLOR < pstSurface->surface_type) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, pstSurface->surface_type);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Invalid surface type");
        return DRV_GFX2D_ERR_INVALID_SURFACE_TYPE;
    }

    if (!bInputSurface && (DRV_GFX2D_SURFACE_TYPE_MEM != pstSurface->surface_type)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, bInputSurface);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, pstSurface->surface_type);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Invalid output surface type");
        return DRV_GFX2D_ERR_INVALID_SURFACE_TYPE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 CheckSurfaceFmt(drv_gfx2d_compose_surface *pstSurface, hi_bool bInputSurface)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstSurface, HI_FAILURE);

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, bInputSurface);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->format);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, DRV_GFX2D_FMT_YUV888);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, DRV_GFX2D_FMT_AYUV8888);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->format / 32); /* 32 algorithm data */
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");

    if (pstSurface->format > DRV_GFX2D_FMT_PLANAR444) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->format);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Invalid fmt");
        return DRV_GFX2D_ERR_INVALID_SURFACE_FMT;
    }

    if (bInputSurface) {
        if ((pstSurface->format / 32) >= /* 32 algorithm data */
            GFX2D_ARRAY_SIZE(g_gfx2d_capability.au32InputFmtBitmap)) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->format);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Invalid input fmt");
            return DRV_GFX2D_ERR_INVALID_SURFACE_FMT;
        }

        if (!IS_BIT_SET((pstSurface->format % 32), g_gfx2d_capability.au32InputFmtBitmap[pstSurface->format / 32])) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->format);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Invalid input fmt");
            return DRV_GFX2D_ERR_INVALID_SURFACE_FMT;
        }
    } else {
        if ((pstSurface->format / 32) >=                                /* 32 algorithm data */
            GFX2D_ARRAY_SIZE(g_gfx2d_capability.au32OutputFmtBitmap)) { /* 32 algorithm data */
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->format);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Invalid output fmt");
            return DRV_GFX2D_ERR_INVALID_SURFACE_FMT;
        }

        if (!IS_BIT_SET((pstSurface->format % 32), g_gfx2d_capability.au32OutputFmtBitmap[pstSurface->format / 32])) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->format);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Invalid output fmt");
            return DRV_GFX2D_ERR_INVALID_SURFACE_FMT;
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 CheckSurfaceAddr(drv_gfx2d_compose_surface *pstSurface, hi_bool bInputSurface)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstSurface, HI_FAILURE);

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, bInputSurface);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->format);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstSurface->phy_addr[0]); /* index is 0 */
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstSurface->phy_addr[1]); /* index is 1 */
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, pstSurface->phy_addr[2]); /* index is 2 */

    if (pstSurface->phy_addr[0] == 0) {                                       /* index is 0 */
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, pstSurface->phy_addr[0]); /* index is 0 */
        return DRV_GFX2D_ERR_INVALID_SURFACE_ADDR;
    }

    if ((pstSurface->format <= DRV_GFX2D_FMT_SEMIPLANAR400) || (pstSurface->format == DRV_GFX2D_FMT_PLANAR400)) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->format);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, DRV_GFX2D_FMT_SEMIPLANAR400);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, DRV_GFX2D_FMT_PLANAR400);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "===================================");
        return HI_SUCCESS;
    }

    if (pstSurface->phy_addr[1] == 0) {                                       /* index is 1 */
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, pstSurface->phy_addr[0]); /* index is 0 */
        return DRV_GFX2D_ERR_INVALID_SURFACE_ADDR;
    }

    if (pstSurface->format <= DRV_GFX2D_FMT_PLANAR400) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "===================================");
        return HI_SUCCESS;
    }

    if (pstSurface->phy_addr[2] == 0) {                                       /* index is 2 */
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, pstSurface->phy_addr[2]); /* index is 2 */
        return DRV_GFX2D_ERR_INVALID_SURFACE_ADDR;
    }

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "========================================");
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 CheckSurfaceStride(drv_gfx2d_compose_surface *pstSurface, hi_bool bInputSurface)
{
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstSurface, HI_FAILURE);

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "=========================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, bInputSurface);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->format);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->stride[0]); /* index is 0 */
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->stride[1]); /* index is 1 */
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->stride[2]); /* index is 2 */
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability.u32MinStride);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability.u32MaxStride);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability.u8StrideAlign);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");

    if ((pstSurface->stride[0] < g_gfx2d_capability.u32MinStride)           /* index is 0 */
        || (pstSurface->stride[0] > g_gfx2d_capability.u32MaxStride)        /* index is 0 */
        || (pstSurface->stride[0] % g_gfx2d_capability.u8StrideAlign)) {    /* index is 0 */
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->stride[0]); /* index is 0 */
        return DRV_GFX2D_ERR_INVALID_SURFACE_STRIDE;
    }

    if ((pstSurface->format <= DRV_GFX2D_FMT_SEMIPLANAR400) || (pstSurface->format == DRV_GFX2D_FMT_PLANAR400)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if ((pstSurface->stride[1] < g_gfx2d_capability.u32MinStride)           /* index is 1 */
        || (pstSurface->stride[1] > g_gfx2d_capability.u32MaxStride)        /* index is 1 */
        || (pstSurface->stride[1] % g_gfx2d_capability.u8StrideAlign)) {    /* index is 1 */
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->stride[1]); /* index is 1 */
        return DRV_GFX2D_ERR_INVALID_SURFACE_STRIDE;
    }

    if (pstSurface->format <= DRV_GFX2D_FMT_PLANAR400) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if ((pstSurface->stride[2] < g_gfx2d_capability.u32MinStride)           /* index is 2 */
        || (pstSurface->stride[2] > g_gfx2d_capability.u32MaxStride)        /* index is 2 */
        || (pstSurface->stride[2] % g_gfx2d_capability.u8StrideAlign)) {    /* index is 2 */
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->stride[2]); /* index is 2 */
        return DRV_GFX2D_ERR_INVALID_SURFACE_STRIDE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 CheckSurfaceCompress(drv_gfx2d_compose_surface *pstSurface, hi_bool bInputSurface)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstSurface, HI_FAILURE);

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, bInputSurface);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->format);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->compress_msg.compress_type);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability.u32CmpWidthAlign);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->format / 32); /* 32 algorithm data */
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");

    if (pstSurface->compress_msg.compress_type >= DRV_GFX2D_COMPRESS_MAX) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->compress_msg.compress_type);
        return DRV_GFX2D_ERR_INVALID_SURFACE_CMPTYPE;
    }

    if (bInputSurface) {
        if (DRV_GFX2D_COMPRESS_NONE != pstSurface->compress_msg.compress_type) {
            /* Input data can only be uncompressed */
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->compress_msg.compress_type);
            return DRV_GFX2D_ERR_INVALID_SURFACE_CMPTYPE;
        }
    } else {
        if (pstSurface->compress_msg.compress_type == DRV_GFX2D_COMPRESS_HFBC) {
            if ((pstSurface->format / 32) >= /* 32 algorithm data */
                GFX2D_ARRAY_SIZE(g_gfx2d_capability.au32CmpFmtBitmap)) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->format);
                return DRV_GFX2D_ERR_UNSUPPORT;
            }

            /* Only ARGB8888 is supported in cmp surface. */
            if (!IS_BIT_SET((pstSurface->format % 32), g_gfx2d_capability.au32CmpFmtBitmap[pstSurface->format / 32])) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->format);
                return DRV_GFX2D_ERR_UNSUPPORT;
            }

            if (g_gfx2d_capability.u32CmpWidthAlign == 0) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->width);
                return DRV_GFX2D_ERR_UNSUPPORT;
            }

            /* Width of cmp surface should be even. */
            if (pstSurface->width % g_gfx2d_capability.u32CmpWidthAlign) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->width);
                return DRV_GFX2D_ERR_UNSUPPORT;
            }
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 CheckSurfaceSolution(drv_gfx2d_compose_surface *pstSurface, hi_bool bInputSurface)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstSurface, HI_FAILURE);

    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, bInputSurface);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstSurface->height);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability.u32MinWidth);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, g_gfx2d_capability.u32MinHeight);
    GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "==========================================================");

    if (pstSurface->width < g_gfx2d_capability.u32MinWidth) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_gfx2d_capability.u32MinWidth);
        return DRV_GFX2D_ERR_INVALID_SURFACE_RESO;
    }

    if (pstSurface->height < g_gfx2d_capability.u32MinHeight) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->height);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_gfx2d_capability.u32MinHeight);
        return DRV_GFX2D_ERR_INVALID_SURFACE_RESO;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 CheckSurface(drv_gfx2d_compose_surface *pstSurface, hi_bool bInputSurface)
{
    hi_s32 s32Ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstSurface, HI_FAILURE);

    s32Ret = CheckSurfaceType(pstSurface, bInputSurface);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, CheckSurfaceType, s32Ret);
        return s32Ret;
    }

    if (DRV_GFX2D_SURFACE_TYPE_COLOR == pstSurface->surface_type) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    s32Ret = CheckSurfaceFmt(pstSurface, bInputSurface);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, CheckSurfaceFmt, s32Ret);
        return s32Ret;
    }

    s32Ret = CheckSurfaceAddr(pstSurface, bInputSurface);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, CheckSurfaceAddr, s32Ret);
        return s32Ret;
    }

    s32Ret = CheckSurfaceStride(pstSurface, bInputSurface);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, CheckSurfaceStride, s32Ret);
        return s32Ret;
    }

    s32Ret = CheckSurfaceCompress(pstSurface, bInputSurface);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, CheckSurfaceCompress, s32Ret);
        return s32Ret;
    }

    s32Ret = CheckSurfaceSolution(pstSurface, bInputSurface);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, CheckSurfaceSolution, s32Ret);
        return s32Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 CheckRect(drv_gfx2d_compose_rect *pstRegionRect, drv_gfx2d_compose_rect *pstRect)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstRegionRect, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstRect, HI_FAILURE);

    if ((pstRegionRect->x < 0) || (pstRegionRect->x >= pstRect->width) || (pstRegionRect->width == 0) ||
        (((hi_u64)pstRegionRect->x + pstRegionRect->width) > pstRect->width) || (pstRegionRect->y < 0) ||
        (pstRegionRect->y >= pstRect->height) || (pstRegionRect->height == 0) ||
        (((hi_u64)pstRegionRect->y + pstRegionRect->height) > pstRect->height)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstRegionRect->x);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstRegionRect->y);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRegionRect->width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRegionRect->height);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRect->width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRect->height);
        return DRV_GFX2D_ERR_INVALID_RECT;
    }

    if ((pstRegionRect->width < g_gfx2d_capability.u32MinWidth) ||
        (pstRegionRect->width > g_gfx2d_capability.u32MaxWidth)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRegionRect->width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_gfx2d_capability.u32MinWidth);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_gfx2d_capability.u32MaxWidth);
        return DRV_GFX2D_ERR_INVALID_SURFACE_RESO;
    }

    if ((pstRegionRect->height < g_gfx2d_capability.u32MinHeight) ||
        (pstRegionRect->height > g_gfx2d_capability.u32MaxHeight)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRegionRect->height);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_gfx2d_capability.u32MinHeight);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, g_gfx2d_capability.u32MaxHeight);
        return DRV_GFX2D_ERR_INVALID_SURFACE_RESO;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 CheckClip(drv_gfx2d_compose_clip *pstClip, drv_gfx2d_compose_rect *pstRect)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstClip, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstRect, HI_FAILURE);

    if (pstClip->clip_mode >= DRV_GFX2D_CLIP_OUTSIDE) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstClip->clip_mode);
        return DRV_GFX2D_ERR_INVALID_CLIP_MODE;
    } else if (DRV_GFX2D_CLIP_INSIDE == pstClip->clip_mode) {
        hi_s64 s64LXPos = pstRect->x;
        hi_s64 s64RXPos = (hi_s64)pstRect->x + (hi_s64)pstRect->width - 1;
        hi_s64 s64TYPos = pstRect->y;
        hi_s64 s64BYPos = (hi_s64)pstRect->y + (hi_s64)pstRect->height - 1;

        hi_s64 s64ClipLXPos = pstClip->clip_rect.x;
        hi_s64 s64ClipRXPos = (hi_s64)pstClip->clip_rect.x + (hi_s64)pstClip->clip_rect.width - 1;
        hi_s64 s64ClipTYPos = pstClip->clip_rect.y;
        hi_s64 s64ClipBYPos = (hi_s64)pstClip->clip_rect.y + (hi_s64)pstClip->clip_rect.height - 1;

        /* no intersection between clip rect and surface rect */
        if ((s64ClipLXPos > s64RXPos) || (s64ClipRXPos < s64LXPos) || (s64ClipTYPos > s64BYPos) ||
            (s64ClipBYPos < s64TYPos)) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s64ClipLXPos);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s64RXPos);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s64LXPos);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s64ClipTYPos);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s64BYPos);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s64TYPos);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstClip->clip_rect.x);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstClip->clip_rect.y);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstClip->clip_rect.width);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstClip->clip_rect.height);
            return DRV_GFX2D_ERR_INVALID_CLIP_RECT;
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 check_resize_info(drv_gfx2d_compose_resize *pstResize, drv_gfx2d_compose_rect *pstInRect,
                                drv_gfx2d_compose_rect *pstOutRect)
{
    if (pstResize->filter.filter_mode > DRV_GFX2D_FILTER_NONE) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, DRV_GFX2D_FILTER_NONE);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstResize->filter.filter_mode);
        return DRV_GFX2D_ERR_UNSUPPORT;
    }

    /* Resize ratio exceed the limit of hardware,return errcode:
    Horizontal deflate ratio should less than 256.
    Vertical deflate ratio should less tha 16. */
    if ((pstOutRect->width * GFX2D_CONFIG_MAX_ZMERATION) <= pstInRect->width) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstInRect->width);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstOutRect->width);
        return DRV_GFX2D_ERR_UNSUPPORT;
    }
    return HI_SUCCESS;
}

static hi_s32 CheckResize(drv_gfx2d_compose_msg *compose)
{
    drv_gfx2d_compose_rect *pstInRect = NULL;
    drv_gfx2d_compose_rect *pstOutRect = NULL;
    drv_gfx2d_compose_resize *pstResize = NULL;
    pstInRect = &(compose->in_rect);
    pstOutRect = &(compose->out_rect);
    pstResize = &(compose->opt.resize);

    if (!pstResize->resize_en) {
        if ((pstInRect->width != pstOutRect->width) || (pstInRect->height > pstOutRect->height)) {
            return DRV_GFX2D_ERR_INVALID_RECT;
        }
        return HI_SUCCESS;
    }

    if (check_resize_info(pstResize, pstInRect, pstOutRect) != HI_SUCCESS) {
        return DRV_GFX2D_ERR_UNSUPPORT;
    }

    if ((pstOutRect->height * GFX2D_CONFIG_MAX_ZMERATION) <= pstInRect->height) {
        return DRV_GFX2D_ERR_INVALID_RESIZE_RATIO;
    } else if ((pstOutRect->height * g_gfx2d_capability.u32MaxVerticalZmeRatio) <= pstInRect->height) {
        hi_u32 u32Factor = 0;
        if ((pstOutRect->height * g_gfx2d_capability.u32MaxVerticalZmeRatio) == 0) {
            return DRV_GFX2D_ERR_UNSUPPORT;
        }

        u32Factor = osal_div64_s64((hi_u64)pstInRect->height,
                                   (hi_u64)pstOutRect->height * g_gfx2d_capability.u32MaxVerticalZmeRatio) +
                    1;

        if ((hi_u64)compose->compose_surface.stride[0] * u32Factor > g_gfx2d_capability.u32MaxStride) {
            return DRV_GFX2D_ERR_UNSUPPORT;
        }
        compose->compose_surface.stride[0] *= u32Factor;

        if (compose->compose_surface.format >= DRV_GFX2D_FMT_SEMIPLANAR420UV) {
            if (compose->compose_surface.stride[1] * u32Factor > g_gfx2d_capability.u32MaxStride) {
                return DRV_GFX2D_ERR_UNSUPPORT;
            }
            compose->compose_surface.stride[1] *= u32Factor;
        }

        if (compose->compose_surface.format >= DRV_GFX2D_FMT_PLANAR420) {
            if (compose->compose_surface.stride[SRC_TWO] * u32Factor > g_gfx2d_capability.u32MaxStride) {
                return DRV_GFX2D_ERR_UNSUPPORT;
            }
            compose->compose_surface.stride[SRC_TWO] *= u32Factor;
        }

        if (u32Factor == 0) {
            return DRV_GFX2D_ERR_UNSUPPORT;
        }
        compose->in_rect.y /= u32Factor;
        compose->in_rect.height /= u32Factor;
    }
    return HI_SUCCESS;
}

static hi_s32 CheckRop(drv_gfx2d_compose_rop *pstRop)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstRop, DRV_GFX2D_ERR_UNSUPPORT);
    if (pstRop->enable) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "unsupport");
        return DRV_GFX2D_ERR_UNSUPPORT;
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 CheckColorkey(drv_gfx2d_compose_colorkey *pstColorkey)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstColorkey, DRV_GFX2D_ERR_UNSUPPORT);
    if (pstColorkey->enable) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "unsupport");
        return DRV_GFX2D_ERR_UNSUPPORT;
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 CheckComposeOption(drv_gfx2d_compose_opt *pstOpt, drv_gfx2d_compose_rect *pstOutRect)
{
    hi_s32 s32Ret;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstOpt, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstOutRect, HI_FAILURE);

    s32Ret = CheckRop(&pstOpt->rop);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, CheckRop, s32Ret);
        return s32Ret;
    }

    s32Ret = CheckColorkey(&pstOpt->colorkey);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, CheckColorkey, s32Ret);
        return s32Ret;
    }

    s32Ret = CheckClip(&pstOpt->clip, pstOutRect);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, CheckClip, s32Ret);
        return s32Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

static hi_s32 check_surface_info(drv_gfx2d_compose_msg *compose, drv_gfx2d_compose_surface *pstDstSurface)
{
    drv_gfx2d_compose_surface *surface = NULL;
    drv_gfx2d_compose_rect *out_rect = NULL;
    drv_gfx2d_compose_rect dst_surface_rect;
    drv_gfx2d_compose_opt *pstOpt = NULL;
    hi_s32 ret;
    surface = &(compose->compose_surface);
    out_rect = &(compose->out_rect);
    pstOpt = &(compose->opt);
    dst_surface_rect.x = 0; /* 0 algorithm data */
    dst_surface_rect.y = 0; /* 0 algorithm data */
    dst_surface_rect.width = pstDstSurface->width;
    dst_surface_rect.height = pstDstSurface->height;

    ret = CheckSurface(surface, HI_TRUE);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, CheckSurface, ret);
        return ret;
    }

    ret = CheckRect(out_rect, &dst_surface_rect);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, CheckRect, ret);
        return ret;
    }

    ret = CheckComposeOption(pstOpt, out_rect);
    if (ret != HI_SUCCESS) {
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, CheckComposeOption, ret);
        return ret;
    }
    return HI_SUCCESS;
}

static hi_s32 CheckComposeSrc(drv_gfx2d_compose_msg *compose, drv_gfx2d_compose_surface *pstDstSurface)
{
    drv_gfx2d_compose_surface *pstSurface = NULL;
    drv_gfx2d_compose_rect *pstInRect = NULL;
    drv_gfx2d_compose_opt *pstOpt = NULL;
    drv_gfx2d_compose_rect stSrcSurfaceRect = {0};
    hi_s32 ret;

    pstSurface = &(compose->compose_surface);
    pstInRect = &(compose->in_rect);
    pstOpt = &(compose->opt);

    stSrcSurfaceRect.width = pstSurface->width;
    stSrcSurfaceRect.height = pstSurface->height;

    ret = check_surface_info(compose, pstDstSurface);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (DRV_GFX2D_SURFACE_TYPE_COLOR == pstSurface->surface_type) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    ret = CheckRect(pstInRect, &stSrcSurfaceRect);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = CheckResize(compose);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* To do clip operation in scaling, hardware support is needed for clip */
    if (pstOpt->resize.resize_en && (pstOpt->clip.clip_mode != DRV_GFX2D_CLIP_NONE) &&
        g_gfx2d_capability.bClipSupport) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "unsupport");
        return DRV_GFX2D_ERR_UNSUPPORT;
    }

    if (pstSurface->format >= DRV_GFX2D_FMT_YUV888) {
        if ((pstInRect->width < g_gfx2d_capability.u32MinVWidth) ||
            (pstInRect->height < g_gfx2d_capability.u32MinVHeight)) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "unsupport");
            return DRV_GFX2D_ERR_UNSUPPORT;
        }
    }
    return HI_SUCCESS;
}

static hi_s32 CheckCompose(const drv_gfx2d_dev_id dev_id, drv_gfx2d_compose_list *compose_list,
                           drv_gfx2d_compose_surface *pstDstSurface)
{
    hi_u32 i = 0;
    hi_s32 s32Ret;
    hi_u32 u32VhdLayerNum = 0;
    hi_u32 u32GLayerNum = 0;
    hi_u32 u32ZmeLayerNum = 0;
    drv_gfx2d_compose_opt *pstOpt = NULL;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstDstSurface, HI_FAILURE);
    s32Ret = CheckSurface(pstDstSurface, HI_FALSE);
    if (s32Ret != HI_SUCCESS) {
        return s32Ret;
    }
    GRAPHIC_CHECK_LEFT_LARGER_RIGHT_RETURN_VALUE(compose_list->compose_cnt, GFX2D_MAX_LAYERS, HI_FAILURE);
    for (i = 0; i < compose_list->compose_cnt; i++) {
        s32Ret = CheckComposeSrc(&(compose_list->compose[i]), pstDstSurface);
        if (s32Ret != HI_SUCCESS) {
            return s32Ret;
        }

        if (DRV_GFX2D_SURFACE_TYPE_COLOR == compose_list->compose[i].compose_surface.surface_type) {
            u32GLayerNum++;
        } else {
            pstOpt = &(compose_list->compose[i].opt);

            if (pstOpt->resize.resize_en) {
                u32ZmeLayerNum++;
            }

            if (compose_list->compose[i].compose_surface.format >= DRV_GFX2D_FMT_YUV888) {
                u32VhdLayerNum++;
            } else if ((compose_list->compose[i].in_rect.width < g_gfx2d_capability.u32MinVWidth) ||
                       (compose_list->compose[i].in_rect.height < g_gfx2d_capability.u32MinVHeight)) {
                u32GLayerNum++;
            }
        }
    }
    /* video layer over hardware number */
    if ((u32VhdLayerNum > g_gfx2d_capability.u8MaxVLayerNum) || (u32GLayerNum > g_gfx2d_capability.u8MaxGLayerNum) ||
        (u32ZmeLayerNum > g_gfx2d_capability.u8ZmeLayerNum)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32VhdLayerNum);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32GLayerNum);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32ZmeLayerNum);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "unsupport");
        return DRV_GFX2D_ERR_UNSUPPORT;
    }
    return HI_SUCCESS;
}

static hi_void AdjustClipRect(drv_gfx2d_compose_rect *pstInRect, drv_gfx2d_compose_rect *pstOutRect,
                              drv_gfx2d_compose_rect *pstClipRect)
{
    hi_s32 s32LXPos = pstOutRect->x;
    hi_s32 s32RXPos = pstOutRect->x + pstOutRect->width - 1;
    hi_s32 s32TYPos = pstOutRect->y;
    hi_s32 s32BYPos = pstOutRect->y + pstOutRect->height - 1;

    hi_s32 s32ClipLXPos = pstClipRect->x;
    hi_s32 s32ClipRXPos = pstClipRect->x + pstClipRect->width - 1;
    hi_s32 s32ClipTYPos = pstClipRect->y;
    hi_s32 s32ClipBYPos = pstClipRect->y + pstClipRect->height - 1;
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstInRect->x);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstInRect->y);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstInRect->width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstInRect->height);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstOutRect->x);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstOutRect->y);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstOutRect->width);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, pstOutRect->height);

    /* calculate intersection */
    if (s32ClipLXPos < s32LXPos) {
        s32ClipLXPos = s32LXPos;
    }

    if (s32ClipRXPos > s32RXPos) {
        s32ClipRXPos = s32RXPos;
    }

    if (s32ClipTYPos < s32TYPos) {
        s32ClipTYPos = s32TYPos;
    }

    if (s32ClipBYPos > s32BYPos) {
        s32ClipBYPos = s32BYPos;
    }

    pstInRect->x += (s32ClipLXPos - pstOutRect->x);
    pstInRect->y += (s32ClipTYPos - pstOutRect->y);
    pstOutRect->x = s32ClipLXPos;
    pstOutRect->y = s32ClipTYPos;

    pstOutRect->width = s32ClipRXPos - s32ClipLXPos + 1;
    pstOutRect->height = s32ClipBYPos - s32ClipTYPos + 1;

    pstInRect->width = pstOutRect->width;
    pstInRect->height = pstOutRect->height;
    return;
}

static hi_void AlignRect(const drv_gfx2d_compose_fmt format, drv_gfx2d_compose_rect *pRect)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pRect);
    switch (format) {
        case DRV_GFX2D_FMT_SEMIPLANAR420UV:
        case DRV_GFX2D_FMT_SEMIPLANAR420VU:
        case DRV_GFX2D_FMT_PLANAR420: {
            pRect->width &= 0xfffffffe;
            pRect->height &= 0xfffffffc;
            pRect->y = (hi_u32)(pRect->y) & 0xfffffffe;
            break;
        }
        case DRV_GFX2D_FMT_SEMIPLANAR422UV_H:
        case DRV_GFX2D_FMT_SEMIPLANAR422VU_H:
        case DRV_GFX2D_FMT_PLANAR400:
        case DRV_GFX2D_FMT_PLANAR422H:
        case DRV_GFX2D_FMT_PLANAR444:
        case DRV_GFX2D_FMT_YUYV422:
        case DRV_GFX2D_FMT_YVYU422:
        case DRV_GFX2D_FMT_UYVY422:
        case DRV_GFX2D_FMT_YYUV422:
        case DRV_GFX2D_FMT_VYUY422:
        case DRV_GFX2D_FMT_VUYY422: {
            pRect->width &= 0xfffffffe;
            pRect->height &= 0xfffffffe;
            break;
        }
        case DRV_GFX2D_FMT_SEMIPLANAR422UV_V:
        case DRV_GFX2D_FMT_SEMIPLANAR422VU_V:
        case DRV_GFX2D_FMT_PLANAR410:
        case DRV_GFX2D_FMT_PLANAR422V: {
            pRect->width &= 0xfffffffc;
            pRect->height &= 0xfffffffc;
            break;
        }
        case DRV_GFX2D_FMT_PLANAR411:
        default: {
            pRect->width &= 0xfffffffc;
            pRect->height &= 0xfffffffe;
            break;
        }
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

static hi_void AdjustCompose(const drv_gfx2d_dev_id dev_id, drv_gfx2d_compose_list *compose_list,
                             drv_gfx2d_compose_surface *pstDstSurface)
{
    hi_u32 i = 0; /* 0 algorithm data */
    drv_gfx2d_compose_msg *compose = NULL;
    for (i = 0; i < compose_list->compose_cnt; i++) {
        compose = &compose_list->compose[i];

        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->in_rect.x);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->in_rect.y);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->in_rect.width);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->in_rect.height);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->out_rect.x);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->out_rect.y);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->out_rect.width);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, compose->out_rect.height);

        if (DRV_GFX2D_CLIP_INSIDE == compose->opt.clip.clip_mode && !compose->opt.resize.resize_en) {
            AdjustClipRect(&(compose->in_rect), &(compose->out_rect), &(compose->opt.clip.clip_rect));
        }

        if (DRV_GFX2D_SURFACE_TYPE_COLOR == compose->compose_surface.surface_type) {
            continue;
        }

        if (compose->compose_surface.format >= DRV_GFX2D_FMT_YUV888) {
            /* Downward even, video layer only supports even */
            compose->in_rect.x = ((hi_u32)compose->in_rect.x) & 0xfffffffe;

            AlignRect(compose->compose_surface.format, &(compose->in_rect));

            if (compose->opt.resize.resize_en) {
                compose->out_rect.width &= 0xfffffffe;
                compose->out_rect.height &= 0xfffffffe;
            } else {
                /* Without zooming, output should be equal to input */
                compose->out_rect.width = compose->in_rect.width;
                compose->out_rect.height = compose->in_rect.height;
            }
            break;
        } else {
            if (compose->opt.resize.resize_en) {
                compose->in_rect.width &= ~(g_gfx2d_capability.u8ZmeAlign - 1);
                compose->in_rect.height &= ~(g_gfx2d_capability.u8ZmeAlign - 1);
                compose->out_rect.width &= ~(g_gfx2d_capability.u8ZmeAlign - 1);
                compose->out_rect.height &= ~(g_gfx2d_capability.u8ZmeAlign - 1);
            }
        }
    }
}

#ifdef GFX2D_ALPHADETECT_SUPPORT
hi_void GFX2D_HAL_HWC_GetTransparent(drv_gfx2d_compose_ext_info *ext_info)
{
    hi_u32 i = 0; /* 0 algorithm data */

    if ((ext_info == HI_NULL) || (g_pst_hwc_alpha_sum_reg == HI_NULL)) {
        return;
    }

    for (i = 0; i < sizeof(g_pst_hwc_alpha_sum_reg->u32AlphaSum) / sizeof(g_pst_hwc_alpha_sum_reg->u32AlphaSum[0]);
         i++) {
        ext_info->alpha_sum[i] = g_pst_hwc_alpha_sum_reg->u32AlphaSum[i];
    }

    return;
}
#endif

#ifdef CONFIG_GFX_PROC_SUPPORT
hi_void GFX2D_HAL_HWC_ReadProc(hi_void *p, hi_void *v)
{
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(p);

    osal_proc_print(p, "++++++++++++++++++++++++++++ GFX2D Hal Info +++++++++++++++++++++++\n");
    return;
}

hi_s32 GFX2D_HAL_HWC_WriteProc(struct seq_file *p, hi_void *v)
{
    return HI_SUCCESS;
}

hi_void gfx2d_set_debug_level(hi_u32 debug_level)
{
    g_gfx2d_hal_debug_level = debug_level;
}
#endif
