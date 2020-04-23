/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hal manage
 * Author: sdk
 * Create: 2019-03-18
 */
#ifndef HI_BUILD_IN_BOOT
#include "hi_osal.h"
#else
#include "hi_gfx_list.h"
#endif
#include "tde_hal.h"
#include "tde_define.h"
#include "wmalloc.h"

#include "hi_reg_common.h"
#include "drv_tde_filter_para.h"
#include "tde_adp.h"

#ifdef HI_BUILD_IN_BOOT
#include "hi_errno.h"
#include "hi_gfx_comm.h"
#include "hi_gfx_debug.h"
#else
#include "hi_errno.h"
#include "tde_debug.h"
#endif

#define UPDATE_SIZE 64

#define NO_HSCALE_STEP 0x100000
#define NO_VSCALE_STEP 0x1000

extern tde_color_fmt g_enTdeCommonDrvColorFmt[HI_TDE_COLOR_FMT_MAX + 1];

/* R/W register's encapsulation */
#define TDE_READ_REG(base, offset) ((base) == NULL) ? (0) : (*(volatile unsigned int *)((void *)(base) + (offset)))
#define TDE_WRITE_REG(base, offset, val)                                 \
    if (((base) != NULL) && ((offset) <= (TDE_REG_SIZE - 4))) {          \
        (*(volatile unsigned int *)((void *)(base) + (offset)) = (val)); \
    }

/* TDE register's Address range */
#define TDE_REG_SIZE 0x40a4

/* Step range's type by algorithm team */
#define TDE_RESIZE_PARA_AREA_0 4096  /* 1.0 */
#define TDE_RESIZE_PARA_AREA_1 5461  /* 0.75 */
#define TDE_RESIZE_PARA_AREA_2 8192  /* 0.5 */
#define TDE_RESIZE_PARA_AREA_3 12412 /* 0.33 */
#define TDE_RESIZE_PARA_AREA_4 16384 /* 0.25 */

/* coefficient table rangle of 8*32 area */
#define TDE_RESIZE_8X32_AREA_0 1048576 /* 1.0 */
#define TDE_RESIZE_8X32_AREA_1 1398101 /* 0.75 */
#define TDE_RESIZE_8X32_AREA_2 2097152 /* 0.5 */
#define TDE_RESIZE_8X32_AREA_3 3177503 /* 0.33 */
#define TDE_RESIZE_8X32_AREA_4 4194304 /* 0.25 */

/* Colorkey mode of CMOS chip */
#define TDE_COLORKEY_IGNORE 2
#define TDE_COLORKEY_AREA_OUT 1
#define TDE_COLORKEY_AREA_IN 0

/* Aq control mode */
#define TDE_AQ_CTRL_COMP_LIST 0x0 /* start next AQ list, after complete current list's operations */
#define TDE_AQ_CTRL_COMP_LINE 0x4 /* start next AQ list, after complete current node and line */

#define TDE_MAX_READ_STATUS_TIME 10

#define TDE_HAL_TWO_BIT_SHIFT 2
#define TDE_HAL_ONE_BIT_SHIFT 1
#define TDE_HAL_ZERO_BIT_SHIFT 0

#define TDE_TWENTYFOUR_BITS_SHIFT 24
#define TDE_EIGHT_BITS_SHIFT 8
#define TDE_SIXTEEN_BITS_SHIFT 16

#define TDE_HAL_CHECK_BLEND_CMD(SetBlendCmd)     \
    if ((SetBlendCmd) >= HI_TDE_BLEND_CMD_MAX) { \
        return HI_ERR_TDE_INVALID_PARA;          \
    }

#ifdef CONFIG_GFX_MMU_CLOCK
// #define CONFIG_GFX_SMMU_RESET
// #define CONFIG_GFX_SMMU_CLOSE_CLOCK
#endif

/****************************************************************************/
/* TDE hal inner type definition */
/****************************************************************************/
/* source bitmap running mode */
typedef enum {
    TDE_SRC_MODE_DISA = 0,
    TDE_SRC_MODE_BMP = 1,
    TDE_SRC_MODE_PATTERN = 2,
    TDE_SRC_MODE_FILL = 3,
    TDE_SRC_QUICK_COPY = 5,
    TDE_SRC_QUICK_FILL = 7
} TDE_SRC_MODE_E;

/* Aq control register's configured format, idiographic meanings can refer to register's handbook */
typedef union {
    struct {
        hi_u32 u32Reserve1 : 20;  /* Reserve 1 */
        hi_u32 u32AqOperMode : 4; /* Resume open mode in AQ list */
        hi_u32 u32AqEn : 1;       /* Enable AQ */
        hi_u32 u32Reserve2 : 7;   /* Reserve 2 */
    } stBits;
    hi_u32 all_bits;
} TDE_AQ_CTRL_U;

/* Operate manner register's configured format, idiographic meanings can refer to register's handbook */
typedef union {
    struct {
        hi_u32 u32Src1Mod : 3;        /* source1 runnning mode */
        hi_u32 u32Src2Mod : 2;        /* source2 runnning mode */
        hi_u32 u32IcsConv : 1;        /* Enable to conversion of importing color zone */
        hi_u32 u32Resize : 1;         /* Enable to reszie on 2D */
        hi_u32 u32DfeEn : 1;          /* Enable to Deflicker flicter */
        hi_u32 u32Y1En : 1;           /* Enable Y1 channel */
        hi_u32 u32Y2En : 1;           /* Enable Y2 channel */
        hi_u32 u32Reserve1 : 1;       /* Reserve 1 */
        hi_u32 u32Clut : 1;           /* Enable CLUT */
        hi_u32 u32ColorKey : 1;       /* Enable Colorkey */
        hi_u32 u32IcscSel : 1;        /* Select in importing color zone conversion */
        hi_u32 u32Clip : 1;           /* Enable Clip */
        hi_u32 u32MbMode : 3;         /* MB operate mode */
        hi_u32 u32MbEn : 1;           /* Enable to operate MB */
        hi_u32 u32AqIrqMask : 4;      /* Fake code of interrupt in AQ lsit */
        hi_u32 u32SqIrqMask : 5;      /* Fake code of interrupt in SQ lsit */
        hi_u32 u32OcsConv : 1;        /* Enable to conversion of exporting color zone */
        hi_u32 u32BlockFlag : 1;      /* Flag if block */
        hi_u32 u32FirstBlockFlag : 1; /* Flag that is block at first */
        hi_u32 u32LastBlockFlag : 1;  /* Flag that is block last */
    } stBits;
    hi_u32 all_bits;
} TDE_INS_U;

/* Source bitmap register's configured format, idiographic meanings can refer to register's handbook */
typedef union {
    struct {
        hi_u32 pitch : 16;            /* line pitch */
        hi_u32 u32SrcColorFmt : 5;    /* pixel format */
        hi_u32 u32AlphaRange : 1;     /* 8bit alpha range */
        hi_u32 u32ColorizeEnable : 1; /* Enable Colorize,only effective for Src2, Src1 reserved */
        hi_u32 u32Reserve1 : 1;       /* Reserve */
        hi_u32 u32HScanOrd : 1;       /* Horizontal scanning direction */
        hi_u32 u32VScanOrd : 1;       /* Vertical scanning direction */
        hi_u32 u32Reserve2 : 3;       /* Reserve */
        hi_u32 u32RgbExp : 1;         /* RGB expand mode */
        hi_u32 u32Reserve3 : 2;       /* Reserve */
    } stBits;
    hi_u32 all_bits;
} TDE_SRC_TYPE_U;

/* Register's Config format at section start */
typedef union {
    struct {
        hi_u32 u32X : 12;       /* First X coordinate */
        hi_u32 u32Reserve1 : 4; /* Reserve */
        hi_u32 u32Y : 12;       /* First Y coordinate */
        hi_u32 u32Reserve2 : 4; /* Reserve */
    } stBits;
    hi_u32 all_bits;
} TDE_SUR_XY_U;

/* Bitmap size register's configured format, idiographic meanings can refer to register's handbook */
typedef union {
    struct {
        hi_u32 width : 12; /* Image width */
        /* The lowest 4 bit of alpha threshold, effective in size register of target, reserved in src2 size register */
        hi_u32 u32AlphaThresholdLow : 4;
        /* The highest 4 bit of alpha threshold, effective in size register of target, reserved in src2 size register */
        hi_u32 height : 12; /* Image height */
        hi_u32 u32AlphaThresholdHigh : 4;
    } stBits;
    hi_u32 all_bits;
} TDE_SUR_SIZE_U;

/* Target bitmap type register's configured format, idiographic meanings can refer to register's handbook */
typedef union {
    struct {
        hi_u32 pitch : 16;               /* line pitch */
        hi_u32 u32TarColorFmt : 5;       /* pixel format */
        hi_u32 u32AlphaRange : 1;        /* 8bit alpha range */
        hi_u32 u32AlphaFrom : 2;         /* Emporting alpha from */
        hi_u32 u32HScanOrd : 1;          /* Horizontal scanning direction */
        hi_u32 u32VScanOrd : 1;          /* Vertical scaning direction */
        hi_u32 u32RgbRound : 1;          /* RGB truncate bit mode */
        hi_u32 u32DfeLastlineOutEn : 1;  /* Emporting enable to deflicker at last line */
        hi_u32 u32DfeFirstlineOutEn : 1; /* Emporting enable to deflicker at first line */
        hi_u32 u32Reserve2 : 3;          /* Reserve */
    } stBits;
    hi_u32 all_bits;
} TDE_TAR_TYPE_U;

/* Color zone conversion register's configured format, idiographic meanings can refer to register's handbook */
typedef union {
    struct {
        hi_u32 u32InColorImetry : 1;  /* Import color conversion standard */
        hi_u32 u32InRgb2Yc : 1;       /* Import color conversion oreitation */
        hi_u32 u32InColorSpace : 1;   /* Import matrix of color conversion used */
        hi_u32 u32InChromaFmt : 1;    /* Import chroma format */
        hi_u32 u32OutColorImetry : 1; /* Export color conversion standard */
        hi_u32 u32OutColorSpace : 1;  /* Export matrix of color conversion */
        hi_u32 u32OutChromaFmt : 1;   /* Export chroma format */
        hi_u32 u32Reserve1 : 2;       /* Reserve */
        hi_u32 u32ClutMod : 1;        /* clut mode */
        hi_u32 u32ClutReload : 1;     /* Enable to update clut table */
        hi_u32 u32Reserve2 : 1;       /* Reserve */
        hi_u32 u32OcscReload : 1;     /* Enable to reload exported CSC self-defined arguments */
        hi_u32 u32OcscCustomEn : 1;   /* Enable to self-defined export CSC */
        hi_u32 u32IcscReload : 1;     /* Enable to reload inported CSC self-defined arguments */
        hi_u32 u32IcscCustomEn : 1;   /* Enable to self-defined inported CSC */
        hi_u32 u32Alpha0 : 8;         /* alpha1555 format, 0:alpha value */
        hi_u32 u32Alpha1 : 8;         /* alpha1555 format, 1:alpha value */
    } stBits;
    hi_u32 all_bits;
} TDE_COLOR_CONV_U;

/* Zoom register's configured format, idiographic meanings can refer to register's handbook */
typedef union {
    struct {
        hi_u32 u32HfMod : 2;           /* Horizontal filter mode */
        hi_u32 u32Reserve1 : 2;        /* Reserve */
        hi_u32 u32VfMod : 2;           /* Vertical filter mode */
        hi_u32 u32Reserve2 : 2;        /* Reserve */
        hi_u32 u32DfeMod : 2;          /* Select in deflicker coefficient */
        hi_u32 u32Reserve3 : 2;        /* Reserve */
        hi_u32 u32AlpBorder : 2;       /* alpha border handling mode */
        hi_u32 u32Reserve4 : 2;        /* Reserve */
        hi_u32 u32VfRingEn : 1;        /* Enable to median filter,when vertical filter enable */
        hi_u32 u32HfRingEn : 1;        /* Enable to median filter,when horizontal filter enable */
        hi_u32 u32CoefSym : 1;         /* Property of filter coefficient */
        hi_u32 u32Reserve5 : 5;        /* Reserve */
        hi_u32 u32VfCoefReload : 1;    /* Enable to update vertical filter coefficient */
        hi_u32 u32HfCoefReload : 1;    /* Enable to update horizontal filter coefficient */
        hi_u32 u32DfeAlphaDisable : 1; /* Unable to alpha deflicker */
        hi_u32 u32Reserve6 : 5;        /* Reserve */
    } stBits;
    hi_u32 all_bits;
} TDE_2D_RSZ_U;

/* Bitmap size register's configured format, idiographic meanings can refer to register's handbook */
typedef union {
    struct {
        hi_u32 u32AluMod : 4;          /* ALU running mode */
        hi_u32 u32AlphaThreshodEn : 1; /* Enable to threshold alpha */
        hi_u32 u32AlphaRopMod : 4;     /* Rop operate mode of alpha */
        hi_u32 u32RgbRopMod : 4;       /* Rop operate mode of RGB */
        hi_u32 u32GlobalAlpha : 8;     /* Global alpha value */
        hi_u32 u32CkBMod : 2;          /* Colorkey blue component operating mode */
        hi_u32 u32CkGMod : 2;          /* Colorkey green component operating mode */
        hi_u32 u32CkRMod : 2;          /* Colorkey red component operating mode */
        hi_u32 u32CkAMod : 2;          /* Colorkey alpha component operating mode */
        hi_u32 u32CkSel : 2;           /* Colorkey inporting selection */
        hi_u32 u32BlendReverse : 1;    /* Enable to reserve blending fore and background */
    } stBits;
    hi_u32 all_bits;
} TDE_ALU_U;

/* ARGB/AYUV component order register format */
typedef union {
    struct {
        hi_u32 u32Src1ArgbOrder : 5; /* Src1's ARGB order */
        hi_u32 u32Reserved1 : 3;     /* Reserve */
        hi_u32 u32Src2ArgbOrder : 5; /* Src2's ARGB order */
        hi_u32 u32Reserved2 : 3;     /* Reserve */
        hi_u32 u32TarArgbOrder : 5;  /* Target's ARGB order */
        hi_u32 u32Reserved3 : 3;     /* Reserve */
    } stBits;
    hi_u32 all_bits;
} TDE_ARGB_ORDER_U;

/* Colorkey Mask Register */
typedef union {
    struct {
        hi_u32 u32BlueMask : 8;  /* Blue component /clut/ V component mask */
        hi_u32 u32GreenMask : 8; /* Green component / U component mask */
        hi_u32 u32RedMask : 8;   /* Red component /Y component mask */
        hi_u32 u32AlphaMask : 8; /* Alpha component mask */
    } stBits;
    hi_u32 all_bits;
} TDE_COLORKEY_MASK_U;

/* alpha blend mode register */
typedef union {
    struct {
        hi_u32 u32Src1BlendMode : 4;     /* Src1's blending mode */
        hi_u32 u32Src1PremultiEn : 1;    /* Enable to pre-multiply alpha */
        hi_u32 u32Src1MultiGlobalEn : 1; /* Enable to Src1 pre-multi global alpha */
        hi_u32 u32Src1PixelAlphaEn : 1;  /* Enable to Src1 pixel alpha */
        hi_u32 u32Src1GlobalAlphaEn : 1; /* Enable to Src1 global alpha */
        hi_u32 u32Src2BlendMode : 4;     /* Blend mode in Src2 */
        hi_u32 u32Src2PremultiEn : 1;    /* Enable to Src2 pre-multi alpha */
        hi_u32 u32Src2MultiGlobalEn : 1; /* Enable to Src2 pre-multi global alpha */
        hi_u32 u32Src2PixelAlphaEn : 1;  /* Enable to Src2 pixel alpha */
        hi_u32 u32Src2GlobalAlphaEn : 1; /* Enable to Src2 global alpha */
        hi_u32 u32AlphaRopEn : 1;        /* Enable to alpha,when alpha blending */
        hi_u32 u32Reserved : 15;         /* Reserve */
    } stBits;
    hi_u32 all_bits;
} TDE_ALPHA_BLEND_U;

typedef union {
    struct {
        hi_u32 pitch : 16;       /* line pitch */
        hi_u32 u32Reserved : 16; /* Reserve */
    } stBits;
    hi_u32 all_bits;
} TDE_Y_PITCH_U;

/* Filter argument table */
typedef union {
    struct {
        hi_u32 u32Para0 : 5;
        hi_u32 u32Para1 : 6;
        hi_u32 u32Para2 : 8;
        hi_u32 u32Para3 : 7;
        hi_u32 u32Para4 : 6;
    } stBits;
    hi_u32 all_bits;
} TDE_FILTER_PARA_U;

typedef union {
    struct {
        hi_u32 u32BwCnt : 10;
        hi_u32 u32CasRidSel : 1;
        hi_u32 u32Src1RidSel : 1;
        hi_u32 u32Src2RidSel : 1;
        hi_u32 u32Y1RidSel : 1;
        hi_u32 u32Y2RidSel : 1;
        hi_u32 u32Reserved1 : 1;
        hi_u32 u32WOutstdMax : 4;
        hi_u32 u32ROutstdMax : 4;
        hi_u32 u32Reserved2 : 7;
        hi_u32 u32ClkCtlEn : 1;
    } stBits;
    hi_u32 all_bits;
} TDE_BUSLIMITER_U;

/* List of each hardware node, using to maitain hardware node */
typedef struct {
    struct osal_list_head list_head;
    hi_u32 phy_addr;                        /* Point to physical address of hardware buffer */
    hi_u32 *pu32VirAddr;                    /* Virtual address responding to hardware buffer */
    hi_s32 handle;                          /* Current node's job handle */
    hi_void *pSwNode;                       /* Software node pointer of current node */
    hi_u32 u32WordSize;                     /* Size of current node config is needed, united by word */
    hi_u32 u32CurUpdate;                    /* Update flag of current node */
    struct hiTDE_BUF_NODE_S *pstParentAddr; /* Parent node addr of configed information inherited */
} TDE_BUF_NODE_S;

/* State information when TDE operate is suspend */
typedef struct {
    hi_s32 s32AqSuspLine; /* Line executed while AQ is suspend ,if -1 , present no effection */
    TDE_BUF_NODE_S *pstSwBuf;
    hi_void *pSwNode; /* Pointer point to software node be suspend */
} TDE_SUSP_STAT_S;

/* Head address of argument table used as config */
typedef struct {
    hi_u64 u32HfCoefAddr;
    hi_u64 u32VfCoefAddr;
#ifndef HI_BUILD_IN_BOOT
    hi_u64 u32DeflickerVfCoefAddr;
#endif
} TDE_PARA_TABLE_S;

typedef struct {
    hi_bool csc_en;
    hi_u32 csc_coef00;
    hi_u32 csc_coef01;
    hi_u32 csc_coef02;
    hi_u32 csc_coef10;
    hi_u32 csc_coef11;
    hi_u32 csc_coef12;
    hi_u32 csc_coef20;
    hi_u32 csc_coef21;
    hi_u32 csc_coef22;
    hi_u32 idc0;
    hi_u32 idc1;
    hi_u32 idc2;
    hi_u32 odc0;
    hi_u32 odc1;
    hi_u32 odc2;
    hi_u32 scale;
    hi_u32 csc_mode_src2;
} tde_hal_csc_para;

/****************************************************************************/
/* TDE hal inner variable definition */
/****************************************************************************/
/* Base addr of register after mapping */
#ifdef TDE_COREDUMP_DEBUG
volatile hi_u32 *g_base_vir_addr = HI_NULL;
#else
STATIC volatile hi_u32 *g_base_vir_addr = HI_NULL;
#endif

/* Head address of config argument table */
STATIC TDE_PARA_TABLE_S g_para_table = {0};

/* Deflicker level, default is auto */
#ifndef HI_BUILD_IN_BOOT
STATIC hi_tde_deflicker_level g_deflicker_level = HI_TDE_DEFLICKER_LEVEL_AUTO;
#endif
/* alpha threshold switch */
STATIC hi_bool g_alpha_threshold_en = HI_FALSE;

/* alpha threshold value */
STATIC hi_u8 g_alpha_threshold_value = 0xff;

STATIC tde_alu_mode s_u32CbmctrlAlu_mode[TDE_BUTT + 1] = {
    3, TDE_ALU_NONE, TDE_ALU_ROP, 3, TDE_ALU_MASK_ROP1, TDE_ALU_MASK_ROP2, TDE_ALU_MASK_BLEND, 3, 3, 3, 3, TDE_BUTT
};
#ifndef HI_BUILD_IN_BOOT

STATIC hi_tde_blend_mode s_u32Src1BlendCmdMode[HI_TDE_BLEND_CMD_MAX + 1] = {
    HI_TDE_BLEND_INVSRC2ALPHA,
    HI_TDE_BLEND_ZERO,
    HI_TDE_BLEND_ZERO,
    HI_TDE_BLEND_INVSRC2ALPHA,
    HI_TDE_BLEND_ONE,
    HI_TDE_BLEND_ZERO,
    HI_TDE_BLEND_SRC2ALPHA,
    HI_TDE_BLEND_ZERO,
    HI_TDE_BLEND_INVSRC2ALPHA,
    HI_TDE_BLEND_INVSRC2ALPHA,
    HI_TDE_BLEND_SRC2ALPHA,
    HI_TDE_BLEND_ONE,
    HI_TDE_BLEND_INVSRC2ALPHA,
    HI_TDE_BLEND_ONE,
    0,
    0
};

STATIC hi_tde_blend_mode s_u32Src2BlendCmdMode[HI_TDE_BLEND_CMD_MAX + 1] = {
    HI_TDE_BLEND_SRC2ALPHA,
    HI_TDE_BLEND_ZERO,
    HI_TDE_BLEND_ONE,
    HI_TDE_BLEND_ONE,
    HI_TDE_BLEND_INVSRC1ALPHA,
    HI_TDE_BLEND_SRC1ALPHA,
    HI_TDE_BLEND_ZERO,
    HI_TDE_BLEND_INVSRC1ALPHA,
    HI_TDE_BLEND_ZERO,
    HI_TDE_BLEND_SRC1ALPHA,
    HI_TDE_BLEND_INVSRC1ALPHA,
    HI_TDE_BLEND_ONE,
    HI_TDE_BLEND_INVSRC1ALPHA,
    HI_TDE_BLEND_ZERO,
    0,
    0
};
#endif
/****************************************************************************/
/* TDE hal inner function definition */
/****************************************************************************/
STATIC hi_s32 TdeHalInitParaTable(hi_void);

#ifndef HI_BUILD_IN_BOOT
STATIC INLINE hi_s32 TdeHalGetOffsetInNode(hi_u64 u64MaskUpdt, hi_u64 up_data_flag);

STATIC hi_void TdeHalNodeSetCLUTColorKeyPara(tde_hardware_node *hardware_node, tde_color_key_cmd *color_key);
STATIC hi_void TdeHalNodeSetYcbcrColorKeyPara(tde_hardware_node *hardware_node, tde_color_key_cmd *color_key);
STATIC hi_void TdeHalNodeSetARGBColorKeyPara(tde_hardware_node *hardware_node, tde_color_key_cmd *color_key);

STATIC hi_u32 TdeHalGetColorKeyMode(hi_tde_color_key_comp *stColorKey);
STATIC hi_u32 TdeHalGetYCbCrKeyMask(hi_u8 u8Cr, hi_u8 u8Cb, hi_u8 u8Cy, hi_u8 alpha);
STATIC hi_u32 TdeHalGetClutKeyMask(hi_u8 u8Clut, hi_u8 alpha);

#endif

STATIC hi_s32 TdeHalGetbppByFmt(tde_color_fmt color_fmt);

STATIC INLINE hi_u32 TdeHalGetResizeParaHTable(hi_u32 u32Step);
STATIC INLINE hi_u32 TdeHalGetResizeParaVTable(hi_u32 u32Step);

STATIC INLINE hi_void TdeHalInitQueue(hi_void);

hi_void tde_hal_set_clock(hi_bool enable);
#ifndef HI_BUILD_IN_BOOT
#define REGISTDER_FLUSH() osal_mb()
#else
#define REGISTDER_FLUSH() \
    {                     \
        do {              \
        } while (0);      \
    }
#endif

/****************************************************************************/
/* TDE hal ctl interface realization */
/****************************************************************************/
/***************************************************************************************
* func          : tde_hal_init
* description   : map the base address for tde,base_addr: the base address of tde
                  CNcomment: CNend\n
* param[in]     :
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
***************************************************************************************/
hi_s32 tde_hal_init(hi_u32 base_addr)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_VALUE(0, base_addr, HI_FAILURE);

    if (HI_SUCCESS != wmeminit()) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, wmeminit, FAILURE_TAG);
        goto TDE_INIT_ERR;
    }

    /* config start address for the parameter */
    /* CNcomment: 配置参数表首地址 */
    if (HI_SUCCESS != TdeHalInitParaTable()) {
        wmemterm();
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeHalInitParaTable, FAILURE_TAG);
        goto TDE_INIT_ERR;
    }

#ifndef HI_BUILD_IN_BOOT
    g_base_vir_addr = (volatile hi_u32 *)HI_GFX_REG_MAP(base_addr, TDE_REG_SIZE);
    if (g_base_vir_addr == HI_NULL) {
        wmemterm();
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_REG_MAP, FAILURE_TAG);
        goto TDE_INIT_ERR;
    }
#else
    g_base_vir_addr = (hi_u32 *)(uintptr_t)base_addr;
#endif

#ifdef CONFIG_GFX_MMU_SUPPORT
    HI_GFX_MapSmmuReg((hi_u32)TDE_MMU_REG_BASEADDR);
#endif

    tde_hal_set_clock(HI_FALSE);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;

TDE_INIT_ERR:
    GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "tde_hal_init failure");
    return HI_FAILURE;
}

#ifndef HI_BUILD_IN_BOOT
hi_void tde_hal_resume_init(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    tde_hal_set_clock(HI_TRUE);

    tde_hal_ctl_reset();

    TdeHalInitQueue();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_void tde_hal_suspend(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    tde_hal_set_clock(HI_FALSE);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/*****************************************************************************
 * Function:      tde_hal_release
 * Description:   release the address that had map
 * Input:         none
 * Output:        none
 * Return:        success/fail
 * Others:        none
 *****************************************************************************/
hi_void tde_hal_release(hi_void)
{
    hi_void *vir_buf = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (g_para_table.u32HfCoefAddr != 0) {
        vir_buf = (hi_void *)wgetvrt(g_para_table.u32HfCoefAddr);
        if (vir_buf != HI_NULL) {
            tde_free(vir_buf);
            g_para_table.u32HfCoefAddr = 0;
        }
    }

    if (g_para_table.u32VfCoefAddr != 0) {
        vir_buf = (hi_void *)wgetvrt(g_para_table.u32VfCoefAddr);
        if (vir_buf != HI_NULL) {
            tde_free(vir_buf);
            g_para_table.u32VfCoefAddr = 0;
        }
    }

    if (g_para_table.u32DeflickerVfCoefAddr != 0) {
        vir_buf = (hi_void *)wgetvrt(g_para_table.u32DeflickerVfCoefAddr);
        if (vir_buf != HI_NULL) {
            tde_free(vir_buf);
            g_para_table.u32DeflickerVfCoefAddr = 0;
        }
    }

    if (g_base_vir_addr != NULL) {
        HI_GFX_REG_UNMAP(g_base_vir_addr);
    }

    g_base_vir_addr = HI_NULL;

#ifdef CONFIG_GFX_MMU_SUPPORT
    HI_GFX_UnMapSmmuReg();
#endif

    wmemterm();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif /* *HI_BUILD_IN_BOOT* */

/*****************************************************************************
 * Function:      tde_hal_ctl_is_idle
 * Description:   get the state of tde
 * Input:         none
 * Output:        none
 * Return:        True: Idle/False: Busy
 * Others:        none
 *****************************************************************************/
hi_bool tde_hal_ctl_is_idle(hi_void)
{
    return (hi_bool)(!((TDE_READ_REG(g_base_vir_addr, TDE_STA)) & 0x1));
}

/*****************************************************************************
 * Function:      tde_hal_ctl_is_idle_safely
 * Description:   get the state of tde one more time ,make sure it's idle
 * Input:         none
 * Output:        none
 * Return:        True: Idle/False: Busy
 * Others:        none
 *****************************************************************************/
hi_bool tde_hal_ctl_is_idle_safely(hi_void)
{
    hi_u32 i = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    /* get the state of tde one more time ,make sure it's idle */
    /* CNcomment: 连续读取多次硬件状态,确保TDE完成 */
    for (i = 0; i < TDE_MAX_READ_STATUS_TIME; i++) {
        if (!tde_hal_ctl_is_idle()) {
            return HI_FALSE;
        }
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_TRUE;
}

/*****************************************************************************
 * Function:      TdeHalCtlIntMask
 * Description:   get the state of interrupt for Sq/Aq
 * Input:         none
 * Output:        none
 * Return:        the interrupt state of Sq/Aq
 * Others:        none
 *****************************************************************************/
hi_u32 tde_hal_ctl_int_status(hi_void)
{
    hi_u32 Value;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    Value = TDE_READ_REG(g_base_vir_addr, TDE_INT);
    TDE_WRITE_REG(g_base_vir_addr, TDE_INTCLR, 0x7);

#ifndef HI_BUILD_IN_BOOT
    (hi_void) HI_GFX_SmmuIsr("HI_MOD_TDE");
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Value;
}

/*****************************************************************************
 * Function:      tde_hal_ctl_reset
 * Description:   reset by software, this operation will clear the interrupt state
 * Input:         none
 * Output:       none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_void tde_hal_ctl_reset(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    TDE_WRITE_REG(g_base_vir_addr, TDE_INTCLR, 0x7);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/***************************************************************************************
* func          : tde_hal_set_clock
* description   : enable or disable the clock of TDE
                  CNcomment: CNend\n
* param[in]     :
* retval        : NA
* retval        : NA
* others:       : NA
***************************************************************************************/
hi_void tde_hal_set_clock(hi_bool enable)
{
    hi_s32 s32Cnt = 0;
#ifndef CONFIG_GFX_TDE_VERSION_3_0
    /* * should not close clock, maybe smmu is working after status is idle* */
#ifndef HI_BUILD_IN_BOOT
    hi_u32 Src1CtrlValue = 0x0;
    hi_u32 u32StartTimeMs = 0;
    hi_u32 u32EndTimeMs = 0;
    hi_u32 u32TotalTime = 0;
#endif
#else
    U_PERI_CRG93 unTempResetValue;
#endif

    U_PERI_CRG337 unTempValue;

#ifdef CONFIG_GFX_MMU_CLOCK
    U_PERI_CRG180 unTempSmmuValue;
#endif
    volatile hi_reg_crg *reg_cfg = HI_NULL;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    reg_cfg = hi_drv_sys_get_crg_reg_ptr();
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(reg_cfg);

    if (enable) {
#ifdef CONFIG_GFX_MMU_CLOCK
        unTempSmmuValue.u32 = reg_cfg->PERI_CRG180.u32;
        unTempSmmuValue.bits.tde_smmu_cken = 0x1;
        reg_cfg->PERI_CRG180.u32 = unTempSmmuValue.u32;
        REGISTDER_FLUSH();
#endif

        unTempValue.u32 = reg_cfg->PERI_CRG337.u32;
        unTempValue.bits.tde_cken = 0x1;
        reg_cfg->PERI_CRG337.u32 = unTempValue.u32;
        REGISTDER_FLUSH();
        do {
            s32Cnt++;
            unTempValue.u32 = reg_cfg->PERI_CRG337.u32;
            if (unTempValue.bits.tde_cken == 0x1) {
                break;
            }
        } while (s32Cnt < 10);

#ifdef CONFIG_GFX_TDE_VERSION_3_0
        unTempValue.u32 = reg_cfg->PERI_CRG337.u32;
        unTempValue.bits.tde_srst_req = 0x1;
        reg_cfg->PERI_CRG337.u32 = unTempValue.u32;
        REGISTDER_FLUSH();

        s32Cnt = 0;
        do {
            s32Cnt++;
            unTempResetValue.u32 = reg_cfg->PERI_CRG93.u32;
#ifndef HI_BUILD_IN_BOOT
            osal_udelay(1);
#endif
            if (s32Cnt > 1000) {
                break;
            }
        } while (unTempResetValue.bits.tde_rst_ok == 0);
#endif

        unTempValue.u32 = reg_cfg->PERI_CRG337.u32;
        unTempValue.bits.tde_srst_req = 0x0;
        reg_cfg->PERI_CRG337.u32 = unTempValue.u32;
        REGISTDER_FLUSH();

#ifdef CONFIG_GFX_MMU_CLOCK
        unTempSmmuValue.u32 = reg_cfg->PERI_CRG180.u32;
        unTempSmmuValue.bits.tde_smmu_srst_req = 0x0;
        reg_cfg->PERI_CRG180.u32 = unTempSmmuValue.u32;
        REGISTDER_FLUSH();
#endif

#ifdef CONFIG_GFX_MMU_SUPPORT
        osal_udelay(1);
        HI_GFX_InitSmmu((hi_u32)TDE_MMU_REG_BASEADDR);
#endif
    }
#if !defined(CONFIG_GFX_TDE_VERSION_3_0) || defined(CONFIG_GFX_TDE_LOW_POWER_SUPPORT)
    else {
        unTempValue.u32 = reg_cfg->PERI_CRG337.u32;
        if (unTempValue.bits.tde_cken == 0x0) {
            unTempValue.bits.tde_cken = 0x1;
            reg_cfg->PERI_CRG337.u32 = unTempValue.u32;
            REGISTDER_FLUSH();
        }

        unTempValue.bits.tde_srst_req = 0x1;
        reg_cfg->PERI_CRG337.u32 = unTempValue.u32;
        REGISTDER_FLUSH();

#ifdef CONFIG_GFX_TDE_LOW_POWER_SUPPORT
        s32Cnt = 0;
        do {
            s32Cnt++;
            unTempResetValue.u32 = reg_cfg->PERI_CRG93.u32;
            if (s32Cnt > 1000) {
                GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "reset error");
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, unTempResetValue.bits.tde_rst_ok);
                break;
            }
        } while (unTempResetValue.bits.tde_rst_ok == 0);
#else
#ifndef HI_BUILD_IN_BOOT
        TDE_WRITE_REG(g_base_vir_addr, TDE_SRC1_CTRL, 0xffffffff);
        HI_GFX_GetTimeStamp(&u32StartTimeMs, NULL);
        do {
            Src1CtrlValue = TDE_READ_REG(g_base_vir_addr, TDE_SRC1_CTRL);
            HI_GFX_GetTimeStamp(&u32EndTimeMs, NULL);
            u32TotalTime = u32EndTimeMs - u32StartTimeMs;
            if (u32TotalTime >= 10) {
                /* reset failure, should not close clock */
                GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, u32TotalTime);
                return;
            }
            osal_udelay(1);
        } while (Src1CtrlValue);
#endif

        unTempValue.u32 = reg_cfg->PERI_CRG337.u32;
        unTempValue.bits.tde_cken = 0x0;
        reg_cfg->PERI_CRG337.u32 = unTempValue.u32;
        REGISTDER_FLUSH();
#endif
    }
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/****************************************************************************/
/* TDE hal node interface */
/****************************************************************************/
static inline hi_u32 tde_product_fill_data_by_fmt(hi_u32 data_value, tde_color_fmt fmt)
{
    hi_u32 i = 0;
    hi_u32 bpp;
    hi_u32 value = 0;
    hi_u32 fill_data = 0;

    bpp = TdeHalGetbppByFmt(fmt);
    if ((bpp != 0xffffffff) && (bpp != 0)) {
        value = data_value & (0xffffffff >> (32 - bpp));
        for (i = 0; i < (32 / bpp); i++) {
            fill_data |= (value << (i * bpp));
        }
    } else {
        fill_data = data_value;
    }

    return fill_data;
}

hi_void tde_hal_set_cfg_reg_offset(tde_hardware_node *hardware_node)
{
    if (hardware_node != HI_NULL) {
        hardware_node->tde_zme_cfg_addr_high.bits.zme_cfg_addr_high =
            HI_GFX_GET_HIGH_PART(wgetphy(hardware_node) + TDE_ZME_CFG_REG_OFFSET);
        hardware_node->tde_zme_cfg_addr_low.bits.zme_cfg_addr_low =
            HI_GFX_GET_LOW_PART(wgetphy(hardware_node) + TDE_ZME_CFG_REG_OFFSET);

        hardware_node->tde_cbm_cfg_addr_high.bits.cbm_cfg_addr_high =
            HI_GFX_GET_HIGH_PART(wgetphy(hardware_node) + TDE_CBM_CFG_REG_OFFSET);
        hardware_node->tde_cbm_cfg_addr_low.bits.cbm_cfg_addr_low =
            HI_GFX_GET_LOW_PART(wgetphy(hardware_node) + TDE_CBM_CFG_REG_OFFSET);
    }
}

/*****************************************************************************
 * Function:      tde_hal_init_node
 * Description:   init the software node struct for tde
 * Input:         hardware_node: the pointer of software node struct
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_s32 tde_hal_init_node(tde_hardware_node **hardware_node)
{
    hi_void *vir_buf = NULL;
    tde_hardware_node *tmp_hardware_node = NULL;
    // HI_UNUSED(Ret);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(hardware_node, HI_FAILURE);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    vir_buf = (hi_void *)tde_malloc(sizeof(tde_hardware_node) + TDE_NODE_HEAD_BYTE + TDE_NODE_TAIL_BYTE);
    if (vir_buf == HI_NULL) {
        wgetfreenum();
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_malloc, FAILURE_TAG);
        return HI_ERR_TDE_NO_MEM;
    }
    *hardware_node = (tde_hardware_node *)(vir_buf + TDE_NODE_HEAD_BYTE);

    tmp_hardware_node = *hardware_node;
    tmp_hardware_node->mmu_prot_ctrl.u32 = 0xffff0000;
    tde_hal_set_cfg_reg_offset(tmp_hardware_node);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

#if defined(CONFIG_GFX_MMU_SUPPORT) && !defined(HI_BUILD_IN_BOOT)
hi_void tde_hal_free_tmp_buf(tde_hardware_node *hardware_node)
{
    return;
}
#endif

/*****************************************************************************
 * Function:      tde_hal_free_node_buf
 * Description:   Free TDE operate node buffer
 * Input:         hardware_node:Node struct pointer.
 * Output:        None
 * Return:        None
 * Others:        None
 *****************************************************************************/
hi_void tde_hal_free_node_buf(tde_hardware_node *hardware_node)
{
    hi_void *vir_buf = NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);

    vir_buf = (hi_void *)hardware_node - TDE_NODE_HEAD_BYTE;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(vir_buf);
#ifdef CONFIG_GFX_MMU_SUPPORT
    tde_hal_free_tmp_buf(hardware_node);
#endif

    tde_free(vir_buf);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
}

/*****************************************************************************
 * Function:      tde_hal_node_execute
 * Description:  start list of tde
 * Input:
 *                node_phy_addr: the start address of head node address
 *                up_data_flag:the head node update set
 *                aq_use_buf: whether use temp buffer
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_s32 tde_hal_node_execute(hi_u64 node_phy_addr, hi_u64 up_data_flag, hi_bool aq_use_buf)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    /* tde is idle */
    if (tde_hal_ctl_is_idle_safely()) {
        tde_hal_set_clock(HI_TRUE);
        tde_hal_ctl_reset();

        TdeHalInitQueue();
        /* write the first node address */
        TDE_WRITE_REG(g_base_vir_addr, TDE_PNEXT_LOW, HI_GFX_GET_LOW_PART(node_phy_addr));
        TDE_WRITE_REG(g_base_vir_addr, TDE_PNEXT_HI, HI_GFX_GET_HIGH_PART(node_phy_addr));

        /* start Aq list */
        TDE_WRITE_REG(g_base_vir_addr, TDE_CTRL, 0x1);
#ifndef HI_BUILD_IN_BOOT
        osal_mb();
#endif
    } else {
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/*****************************************************************************
 * Function:      tde_hal_node_enable_complete_int
 * Description:   enable the finish interrupt of node
 * Input:         vir_buf: buffer of node
 *                enType: type of list
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_void tde_hal_node_enable_complete_int(hi_void *vir_buf)
{
    tde_hardware_node *hardware_node = (tde_hardware_node *)vir_buf;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(vir_buf);

    hardware_node->tde_intmask.bits.eof_mask = 0x1;
    hardware_node->tde_intmask.bits.timeout_mask = 0x1;
    hardware_node->tde_intmask.bits.bus_err_mask = 0x1;
    hardware_node->tde_intmask.bits.eof_end_mask = 0x0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

/*****************************************************************************
 * Function:      tde_hal_set_src1
 * Description:   set the info for source of bitmap 1
 * Input:         hardware_node: pointer of node
 *                surface: bitmap info
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_void tde_hal_set_src1(tde_hardware_node *hardware_node, tde_surface_msg *surface)
{
    hi_u32 u32Bpp;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(surface);

    /* * set the source bitmap attribute info * */
    /* * CNcomment:配置源位图属性信息 * */
    hardware_node->src1_ctrl.bits.src1_fmt = (hi_u32)surface->color_fmt;
    hardware_node->src1_ctrl.bits.src1_alpha_range = 1 - (hi_u32)surface->alpha_max_is_255;
    hardware_node->src1_ctrl.bits.src1_h_scan_ord = (hi_u32)surface->hor_scan;
    hardware_node->src1_ctrl.bits.src1_v_scan_ord = (hi_u32)surface->ver_scan;
    /* * file zero of low area and top area use low area extend * */
    /* * CNcomment:一直使用低位填充为0,高位使用低位的扩展方式 ** */
    hardware_node->src1_ctrl.bits.src1_rgb_exp = 0;

#ifdef CONFIG_GFX_MMU_SUPPORT
    hardware_node->mmu_prot_ctrl.bits.src1_mmu_bypass = surface->is_cma;
#else
    hardware_node->mmu_prot_ctrl.bits.src1_mmu_bypass = 0x1;
#endif

    hardware_node->src1_image_size.bits.src1_width = surface->width - 1;
    hardware_node->src1_image_size.bits.src1_height = surface->height - 1;

    u32Bpp = TdeHalGetbppByFmt(surface->color_fmt);

    hardware_node->src1_ch0_addr_l.bits.src1_ch0_addr_l = surface->phy_addr + surface->pos_y * surface->pitch +
                                                          ((surface->pos_x * u32Bpp) >> 3);

    if (surface->color_fmt <= TDE_DRV_COLOR_FMT_RGB565) {
        hardware_node->src1_ctrl.bits.src1_argb_order = surface->rgb_order;
    }

    /* target bitmapis same with source bitmap 1,so not need set */
    /* CNcomment:源1位图宽高总是与Target位图一致,因此不设置源1的大小 */
    /* config the node */
    /* CNcomment:配置缓存节点 */
    hardware_node->src1_ch0_stride.bits.src1_ch0_stride = surface->pitch;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/*****************************************************************************
 * Function:      tde_hal_set_src2
 * Description:   set the source bitmap 2
 * Input:         hardware_node: pointer of node
 *                surface:  bitmap info
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_void tde_hal_set_src2(tde_hardware_node *hardware_node, tde_surface_msg *surface)
{
    hi_u32 u32Bpp;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(surface);

    /* set attribute info for source bitmap */
    /* CNcomment:配置源位图属性信息 */
    hardware_node->src2_ctrl.bits.src2_fmt = (hi_u32)surface->color_fmt;
    hardware_node->src2_ctrl.bits.src2_alpha_range = 0;
    hardware_node->src2_ctrl.bits.src2_h_scan_ord = (hi_u32)surface->hor_scan;
    hardware_node->src2_ctrl.bits.src2_v_scan_ord = (hi_u32)surface->ver_scan;
    /* file zero of low area and top area use low area extend */
    /* CNcomment:一直使用低位填充为0,高位使用低位的扩展方式 */
    hardware_node->src2_ctrl.bits.src2_rgb_exp = 0;
#ifdef CONFIG_GFX_MMU_SUPPORT
    hardware_node->mmu_prot_ctrl.bits.src2_ch0_mmu_bypass = surface->is_cma;
    hardware_node->mmu_prot_ctrl.bits.src2_ch1_mmu_bypass = surface->is_cma;
#else
    hardware_node->mmu_prot_ctrl.bits.src2_ch0_mmu_bypass = 0x1;
    hardware_node->mmu_prot_ctrl.bits.src2_ch1_mmu_bypass = 0x1;
#endif

    hardware_node->src2_image_size.bits.src2_width = surface->width - 1;
    hardware_node->src2_image_size.bits.src2_height = surface->height - 1;

    u32Bpp = TdeHalGetbppByFmt(surface->color_fmt);
    if (surface->color_fmt >= TDE_DRV_COLOR_FMT_YCbCr400MBP) {
        hardware_node->src2_ch0_addr_l.bits.src2_ch0_addr_l = surface->phy_addr + surface->pos_y * surface->pitch +
                                                              ((surface->pos_x * 8) >> 3);

        hardware_node->src2_ch1_addr_l.bits.src2_ch1_addr_l = surface->cbcr_phy_addr;

        switch (surface->color_fmt) {
            case TDE_DRV_COLOR_FMT_YCbCr422MBH:
                hardware_node->src2_ch1_addr_l.bits.src2_ch1_addr_l =
                    surface->cbcr_phy_addr + surface->pos_y * surface->cbcr_pitch + (surface->pos_x / 2 * 2);
                break;
            case TDE_DRV_COLOR_FMT_YCbCr422MBV:
                hardware_node->src2_ch1_addr_l.bits.src2_ch1_addr_l =
                    surface->cbcr_phy_addr + surface->pos_y / 2 * surface->cbcr_pitch + ((surface->pos_x * 16) >> 3);
                break;
            case TDE_DRV_COLOR_FMT_YCbCr420MB:
                hardware_node->src2_ch1_addr_l.bits.src2_ch1_addr_l =
                    surface->cbcr_phy_addr + surface->pos_y / 2 * surface->cbcr_pitch + (surface->pos_x / 2 * 2);
                break;
            case TDE_DRV_COLOR_FMT_YCbCr444MB:
                hardware_node->src2_ch1_addr_l.bits.src2_ch1_addr_l =
                    surface->cbcr_phy_addr + surface->pos_y * surface->cbcr_pitch + ((surface->pos_x * 16) >> 3);
                break;
            default:
                break;
        }

        if (surface->color_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV) {
            hardware_node->src2_ctrl.bits.src2_422v_pro = 1;
        }
    } else {
        hardware_node->src2_ch0_addr_l.bits.src2_ch0_addr_l = surface->phy_addr + surface->pos_y * surface->pitch +
                                                              ((surface->pos_x * u32Bpp) >> 3);

        if (surface->color_fmt <= TDE_DRV_COLOR_FMT_RGB565) {
            hardware_node->src2_ctrl.bits.src2_argb_order = surface->rgb_order;
        }
    }

    if (surface->color_fmt <= TDE_DRV_COLOR_FMT_A1) {
        hardware_node->src2_alpha.bits.src2_alpha0 = 0x00;
        hardware_node->src2_alpha.bits.src2_alpha1 = 0xff;
    }
    hardware_node->src2_ch0_stride.bits.src2_ch0_stride = surface->pitch;
    hardware_node->src2_ch1_stride.bits.src2_ch1_stride = surface->cbcr_pitch;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/*****************************************************************************
 * Function:      tde_hal_node_set_tqt
 * Description:   set target bitmap info
 * Input:         hardware_node: pointer of node
 *                surface: bitmap info
 *                alpha_from: alpha from
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_void tde_hal_node_set_tqt(tde_hardware_node *hardware_node, tde_surface_msg *surface,
                             hi_tde_out_alpha_from alpha_from)
{
    hi_u32 u32Bpp;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(surface);

    /* set bitmap attribute info */
    /* CNcomment:配置源位图属性信息 */
    hardware_node->dst_ctrl.bits.dst_en = 1;
    hardware_node->dst_ctrl.bits.dst_fmt = surface->color_fmt;
    hardware_node->dst_ctrl.bits.dst_alpha_range = 1 - (hi_u32)surface->alpha_max_is_255;
    hardware_node->dst_ctrl.bits.dst_h_scan_ord = (hi_u32)surface->hor_scan;
    hardware_node->dst_ctrl.bits.dst_v_scan_ord = (hi_u32)surface->ver_scan;
    hardware_node->dst_ctrl.bits.dst_alpha_range = 1 - (hi_u32)surface->alpha_max_is_255;
    /* CNcomment:一直使用使用四舍无入的截位方式 */
    hardware_node->dst_ctrl.bits.dst_rgb_round = 0;

    hardware_node->cbmparaalu1.bits.alu1_alpha_from = (hi_u32)alpha_from;
    hardware_node->dst_alpha.bits.dst_alpha_thd = g_alpha_threshold_value;

    /* set bitmap size info */
    /* CNcomment:配置位图大小信息 */
    hardware_node->dst_image_size.bits.dst_width = (hi_u32)surface->width - 1;
    hardware_node->dst_image_size.bits.dst_height = (hi_u32)surface->height - 1;

    hardware_node->cbm_imgsize.bits.cbm_width = hardware_node->dst_image_size.bits.dst_width;
    hardware_node->cbm_imgsize.bits.cbm_height = hardware_node->dst_image_size.bits.dst_height;

#ifdef CONFIG_GFX_MMU_SUPPORT
    hardware_node->dst_ctrl.bits.dst_ch0_mmu_bypass = surface->is_cma;
    hardware_node->dst_ctrl.bits.dst_ch1_mmu_bypass = surface->is_cma;
#else
    hardware_node->dst_ctrl.bits.dst_ch0_mmu_bypass = 1;
    hardware_node->dst_ctrl.bits.dst_ch1_mmu_bypass = 1;
#endif

    if ((surface->color_fmt <= TDE_DRV_COLOR_FMT_RGB565) || (surface->color_fmt == TDE_DRV_COLOR_FMT_AYCbCr8888)) {
        hardware_node->dst_ctrl.bits.dst_argb_order = surface->rgb_order;
        if (surface->color_fmt == TDE_DRV_COLOR_FMT_AYCbCr8888) {
            surface->color_fmt = TDE_DRV_COLOR_FMT_ARGB8888;
            hardware_node->dst_ctrl.bits.dst_argb_order = 0x17;
        }
    }

    if (TDE_DRV_COLOR_FMT_RABG8888 == surface->color_fmt) {
        hardware_node->dst_ctrl.bits.dst_argb_order = TDE_DRV_ORDER_RABG;
        surface->color_fmt = TDE_DRV_COLOR_FMT_ARGB8888;
    }

    u32Bpp = TdeHalGetbppByFmt(surface->color_fmt);
    hardware_node->dst_ch0_addr_l.bits.dst_ch0_addr_l = surface->phy_addr + surface->pos_y * surface->pitch +
                                                        (surface->pos_x * (u32Bpp >> 3));

    if (surface->color_fmt == TDE_DRV_COLOR_FMT_YCbCr420MB) {
        hardware_node->dst_ch0_addr_l.bits.dst_ch0_addr_l = surface->phy_addr + surface->pos_y * surface->pitch +
                                                            ((surface->pos_x / 2 * 2) * (u32Bpp >> 3));
        hardware_node->dst_ch1_addr_l.bits.dst_ch1_addr_l =
            surface->cbcr_phy_addr + surface->pos_y / 2 * surface->cbcr_pitch + (surface->pos_x / 2 * 2);
        hardware_node->dst_ch1_stride.bits.dst_ch1_stride = surface->cbcr_pitch;
        hardware_node->dst_v_dswm.bits.dst_v_dswm_mode = 1;
        hardware_node->dst_h_dswm.bits.dst_h_dwsm_ow = (hi_u32)surface->width - 1;
    }

    if (surface->color_fmt == TDE_DRV_COLOR_FMT_YCbCr422 || surface->color_fmt == TDE_DRV_COLOR_FMT_YCbCr420MB) {
        hardware_node->dst_h_dswm.bits.dst_h_dswm_mode = 2;
    }

    hardware_node->dst_ch0_stride.bits.dst_ch0_stride = (hi_u32)surface->pitch;
    hardware_node->dst_ctrl.bits.dst_fmt = surface->color_fmt;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

STATIC hi_bool TdeHalIsOperationSupport(tde_alu_mode alu_mode, hi_u32 u32Capability, hi_u32 u32OperMode)
{
    hi_bool bOtherSupportOper = ((u32Capability & u32OperMode) ? HI_TRUE : HI_FALSE);
    hi_bool bMaskBlend = !((alu_mode == TDE_ALU_MASK_BLEND) && (!(u32Capability & u32OperMode)));
    hi_bool bMaskRop = !(((TDE_ALU_MASK_ROP1 == alu_mode) || (TDE_ALU_MASK_ROP2 == alu_mode)) &&
                         (!(u32Capability & u32OperMode)));
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (!(MASKBLEND & u32OperMode) && !(MASKROP & u32OperMode)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return bOtherSupportOper;
    }

    if (MASKBLEND & u32OperMode) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return bMaskBlend;
    }

    if (MASKROP & u32OperMode) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return bMaskRop;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_TRUE;
}

/*****************************************************************************
 * Function:      tde_hal_node_set_base_operate
 * Description:   set base operation
 * Input:         hardware_node: pointer of node
 *                opt_mode: base operation mode
 *                alu_mode: ALU mode
 *                color_fill:fill color value
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_s32 tde_hal_node_set_base_operate(tde_hardware_node *hardware_node, tde_base_opt_mode opt_mode,
                                     tde_alu_mode alu_mode, tde_color_fill *color_fill)
{
    hi_u32 u32Capability = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(hardware_node, HI_FAILURE);
    tde_get_capability(&u32Capability);

    switch (opt_mode) {
        case TDE_QUIKE_FILL: {
            if (!TdeHalIsOperationSupport(alu_mode, u32Capability, QUICKFILL)) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, QUICKCOPY);
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Capability);
                GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeHalIsOperationSupport, FAILURE_TAG);
                GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It deos not support QuickFill");
                return HI_ERR_TDE_UNSUPPORTED_OPERATION;
            }

            GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(color_fill, HI_FAILURE);
            hardware_node->src1_fill.bits.src1_color_fill = tde_product_fill_data_by_fmt(color_fill->fill_data,
                                                                                         color_fill->color_fmt);
            hardware_node->src1_ctrl.bits.src1_en = 1;
            hardware_node->src1_ctrl.bits.src1_mode = 1;
            hardware_node->src1_ctrl.bits.src1_dma = 1;
        } break;

#ifndef HI_BUILD_IN_BOOT
        case TDE_QUIKE_COPY: {
            if (!TdeHalIsOperationSupport(alu_mode, u32Capability, QUICKCOPY)) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, QUICKCOPY);
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Capability);
                GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeHalIsOperationSupport, FAILURE_TAG);
                GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It deos not support QuickCopy");
                return HI_ERR_TDE_UNSUPPORTED_OPERATION;
            }
            hardware_node->src1_ctrl.bits.src1_en = 1;
            hardware_node->src1_ctrl.bits.src1_dma = 1;
        } break;
        case TDE_NORM_FILL_1OPT: /* single fill */
        {
            GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(color_fill, HI_FAILURE);
            hardware_node->src2_fill.bits.src2_color_fill = tde_product_fill_data_by_fmt(color_fill->fill_data,
                                                                                         color_fill->color_fmt);
            hardware_node->src2_ctrl.bits.src2_en = 1;
            hardware_node->src2_ctrl.bits.src2_mode = 1;

            hardware_node->cbm_ctrl.bits.cbm_en = 1;
            hardware_node->cbm_ctrl.bits.cbm_mode = 1;
            hardware_node->cbm_ctrl.bits.alu1_mode = 0;
            hardware_node->cbm_ctrl.bits.cbm_src2_en = 1;
            hardware_node->cbm_ctrl.bits.cbm_mix_prio0 = 0x2; /* src2 */
            hardware_node->cbm_ctrl.bits.alu2_mode = 0;
        } break;
        case TDE_NORM_FILL_2OPT: /* single color with bitmap operation and blit */
        {
            GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(color_fill, HI_FAILURE);
            hardware_node->src2_fill.bits.src2_color_fill = tde_product_fill_data_by_fmt(color_fill->fill_data,
                                                                                         color_fill->color_fmt);
            hardware_node->src1_ctrl.bits.src1_en = 1;
            hardware_node->src1_ctrl.bits.src1_mode = 0;

            hardware_node->src2_ctrl.bits.src2_en = 1;
            hardware_node->src2_ctrl.bits.src2_mode = 1;

            hardware_node->cbm_ctrl.bits.cbm_en = 1;
            hardware_node->cbm_ctrl.bits.cbm_mode = 1;
            hardware_node->cbm_ctrl.bits.cbm_src2_en = 1;
            hardware_node->cbm_ctrl.bits.cbm_mix_prio0 = 0x2; /* src2 */

            GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(alu_mode, TDE_BUTT + 1, HI_FAILURE);
            hardware_node->cbm_ctrl.bits.alu1_mode = s_u32CbmctrlAlu_mode[alu_mode];
        } break;
#endif
        case TDE_NORM_BLIT_1OPT: /* single blit */
        {
            hardware_node->src2_ctrl.bits.src2_en = 1;
            hardware_node->src2_ctrl.bits.src2_mode = 0;

            hardware_node->cbm_ctrl.bits.cbm_en = 1;
            hardware_node->cbm_ctrl.bits.cbm_mode = 1;
            hardware_node->cbm_ctrl.bits.alu1_mode = 0;
            hardware_node->cbm_ctrl.bits.cbm_src2_en = 1;
            hardware_node->cbm_ctrl.bits.cbm_mix_prio0 = 0x2;
            hardware_node->cbm_ctrl.bits.alu2_mode = 0;
        } break;

        case TDE_NORM_BLIT_2OPT: {
            if (!TdeHalIsOperationSupport(alu_mode, u32Capability, MASKROP)) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, MASKROP);
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Capability);
                GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeHalIsOperationSupport, FAILURE_TAG);
                GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It deos not support MaskRop");
                return HI_ERR_TDE_UNSUPPORTED_OPERATION;
            }

            if (!TdeHalIsOperationSupport(alu_mode, u32Capability, MASKBLEND)) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, MASKBLEND);
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Capability);
                GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeHalIsOperationSupport, FAILURE_TAG);
                GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It deos not support MaskBlend");
                return HI_ERR_TDE_UNSUPPORTED_OPERATION;
            }

            hardware_node->src1_ctrl.bits.src1_en = 1;
            hardware_node->src1_ctrl.bits.src1_mode = 0;

            hardware_node->src2_ctrl.bits.src2_en = 1;
            hardware_node->src2_ctrl.bits.src2_mode = 0;

            hardware_node->cbm_ctrl.bits.cbm_en = 1;
            hardware_node->cbm_ctrl.bits.cbm_mode = 1;
            hardware_node->cbm_ctrl.bits.cbm_src2_en = 1;
            hardware_node->cbm_ctrl.bits.cbm_mix_prio0 = 0x1;
            hardware_node->cbm_ctrl.bits.cbm_src1_en = 1;
            hardware_node->cbm_ctrl.bits.cbm_mix_prio1 = 0x2;

            GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(alu_mode, TDE_BUTT + 1, HI_FAILURE);
            hardware_node->cbm_ctrl.bits.alu1_mode = s_u32CbmctrlAlu_mode[alu_mode];
        } break;

        case TDE_MB_2OPT: {
            hardware_node->src2_ctrl.bits.src2_en = 1;
            hardware_node->src2_ctrl.bits.src2_mode = 0;

            hardware_node->cbm_ctrl.bits.cbm_en = 1;
            hardware_node->cbm_ctrl.bits.cbm_mode = 1;
            hardware_node->cbm_ctrl.bits.alu1_mode = 0x1;
        } break;
#ifndef HI_BUILD_IN_BOOT
        case TDE_SINGLE_SRC_PATTERN_FILL_OPT: {
            if (!TdeHalIsOperationSupport(alu_mode, u32Capability, PATTERFILL)) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, PATTERFILL);
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Capability);
                GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeHalIsOperationSupport, FAILURE_TAG);
                GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It deos not support PatternFill");
                return HI_ERR_TDE_UNSUPPORTED_OPERATION;
            }

            hardware_node->src2_ctrl.bits.src2_en = 1;
            hardware_node->src2_ctrl.bits.src2_mode = 2;

            hardware_node->cbm_ctrl.bits.cbm_en = 1;
            hardware_node->cbm_ctrl.bits.cbm_mode = 1;

            GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(alu_mode, TDE_BUTT + 1, HI_FAILURE);
            hardware_node->cbm_ctrl.bits.alu1_mode = s_u32CbmctrlAlu_mode[alu_mode];

            break;
        }

        case TDE_DOUBLE_SRC_PATTERN_FILL_OPT: {
            if (!TdeHalIsOperationSupport(alu_mode, u32Capability, PATTERFILL)) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, PATTERFILL);
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Capability);
                GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeHalIsOperationSupport, FAILURE_TAG);
                GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It deos not support PatternFill");
                return HI_ERR_TDE_UNSUPPORTED_OPERATION;
            }

            hardware_node->src1_ctrl.bits.src1_en = 1;
            hardware_node->src1_ctrl.bits.src1_mode = 0;

            hardware_node->src2_ctrl.bits.src2_en = 1;
            hardware_node->src2_ctrl.bits.src2_mode = 2;

            hardware_node->cbm_ctrl.bits.cbm_en = 1;
            hardware_node->cbm_ctrl.bits.cbm_mode = 1;

            GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(alu_mode, TDE_BUTT + 1, HI_FAILURE);
            hardware_node->cbm_ctrl.bits.alu1_mode = s_u32CbmctrlAlu_mode[alu_mode];

            break;
        }
#endif

        default:
            break;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/*****************************************************************************
 * Function:      tde_hal_node_set_global_alpha
 * Description:   set alpha blend
 * Input:         hardware_node: pointer of node
 *                alpha: Alpha value
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_void tde_hal_node_set_global_alpha(tde_hardware_node *hardware_node, hi_u8 alpha, hi_bool enable)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
#ifndef HI_BUILD_IN_BOOT
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);
    hardware_node->src2_cbm_para.bits.s2_galpha = alpha;
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

#ifndef HI_BUILD_IN_BOOT
hi_void tde_hal_node_set_src1_alpha(tde_hardware_node *hardware_node)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);
    /* set alpha0 alpha1 */
    hardware_node->src1_alpha.bits.src1_alpha0 = 0;
    hardware_node->src1_alpha.bits.src1_alpha1 = 0xff;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

hi_void tde_hal_node_set_src2_alpha(tde_hardware_node *hardware_node)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);
    /* set alpha0 alpha1 */
    hardware_node->src2_alpha.bits.src2_alpha0 = 0;
    hardware_node->src2_alpha.bits.src2_alpha1 = 0xff;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/*****************************************************************************
 * Function:      tde_hal_node_set_exp_alpha
 * Description:   extend to alpha0 and alpha1 operation when extend alpha for RGB5551
 * Input:         hardware_node:pointer of node
 *                alpha0: Alpha0 value
 *                alpha1: Alpha1 value
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_void tde_hal_node_set_exp_alpha(tde_hardware_node *hardware_node, tde_src_mode src_mode, hi_u8 alpha0, hi_u8 alpha1)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);

    if (TDE_DRV_SRC_S1 & (hi_u32)src_mode) {
        hardware_node->src1_alpha.bits.src1_alpha0 = alpha0;
        hardware_node->src1_alpha.bits.src1_alpha1 = alpha1;
        hardware_node->src1_ctrl.bits.src1_rgb_exp = 3;
    }

    if (TDE_DRV_SRC_S2 & (hi_u32)src_mode) {
        hardware_node->src2_alpha.bits.src2_alpha0 = alpha0;
        hardware_node->src2_alpha.bits.src2_alpha1 = alpha1;
        hardware_node->src2_ctrl.bits.src2_rgb_exp = 3;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

/*****************************************************************************
 * Function:      tde_hal_node_set_rop
 * Description:   set rop operation parameter
 * Input:         hardware_node: pointer of node
 *                enRopCode: ROP code
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_s32 tde_hal_node_set_rop(tde_hardware_node *hardware_node, hi_tde_rop_mode rgb_rop, hi_tde_rop_mode alpha_rop)
{
    hi_u32 u32Capability;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(hardware_node, HI_FAILURE);

    tde_get_capability(&u32Capability);
    if (!(u32Capability & ROP)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, ROP);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Capability);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It deos not support Rop");
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    /* set node */
    /* CNcomment:配置缓存节点 */
    hardware_node->cbm_ctrl.bits.cbm_en = 1;
    hardware_node->cbm_ctrl.bits.cbm_mode = 1;

    hardware_node->cbmparaalu1.bits.alu1_rgb_rop = (hi_u32)rgb_rop;
    hardware_node->cbmparaalu1.bits.alu1_a_rop = (hi_u32)alpha_rop;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

/*****************************************************************************
 * Function:      tde_hal_node_set_blend
 * Description:   set blend operation parameter
 * Input:         hardware_node: pointer of node
 *                blend_opt:blend operation
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_s32 tde_hal_node_set_blend(tde_hardware_node *hardware_node, hi_tde_blend_opt *blend_opt)
{
    hi_u32 u32Capability;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(hardware_node, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(blend_opt, HI_FAILURE);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    TDE_HAL_CHECK_BLEND_CMD(blend_opt->blend_cmd);

    tde_get_capability(&u32Capability);
    if (!(u32Capability & ALPHABLEND)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, ALPHABLEND);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Capability);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It deos not support Blend");
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    hardware_node->src1_cbm_para.bits.s1_palphaen = HI_TRUE;
    hardware_node->src1_cbm_para.bits.s1_galphaen = HI_FALSE;
    hardware_node->src1_cbm_para.bits.s1_multiglobalen = HI_FALSE;
    hardware_node->src2_cbm_para.bits.s2_multiglobalen = HI_FALSE;

    hardware_node->src1_cbm_para.bits.s1_premulten = blend_opt->src1_alpha_premulti;
    hardware_node->src2_cbm_para.bits.s2_premulten = blend_opt->src2_alpha_premulti;
    hardware_node->src2_cbm_para.bits.s2_palphaen = blend_opt->pixel_alpha_en;
    hardware_node->src2_cbm_para.bits.s2_galphaen = blend_opt->global_alpha_en;

    /* set mode for src1 and src2 */
    if (blend_opt->blend_cmd >= HI_TDE_BLEND_CMD_CONFIG) {
        hardware_node->src1_cbm_para.bits.s1_blendmode = blend_opt->src1_blend_mode;
        hardware_node->src2_cbm_para.bits.s2_blendmode = blend_opt->src2_blend_mode;
    } else {
        GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(blend_opt->blend_cmd, HI_TDE_BLEND_CMD_MAX + 1, HI_FAILURE);
        hardware_node->src1_cbm_para.bits.s1_blendmode = s_u32Src1BlendCmdMode[blend_opt->blend_cmd];
        hardware_node->src2_cbm_para.bits.s2_blendmode = s_u32Src2BlendCmdMode[blend_opt->blend_cmd];
    }
    hardware_node->cbm_ctrl.bits.cbm_en = 1;
    hardware_node->cbm_ctrl.bits.cbm_mode = 1;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

/*****************************************************************************
 * Function:      tde_hal_node_set_colorize
 * Description:  set blend operation parmeter
 * Input:         hardware_node: point of node
 *                color_resize:Co
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_s32 tde_hal_node_set_colorize(tde_hardware_node *hardware_node, hi_u32 color_resize)
{
    hi_u32 u32Capability;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(hardware_node, HI_FAILURE);

    tde_get_capability(&u32Capability);
    if (!(u32Capability & COLORIZE)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, COLORIZE);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Capability);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It deos not support Colorize");
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    hardware_node->cbmcolorizealu1.bits.alu1_colorizeen = 1;
    hardware_node->cbmcolorizealu1.bits.alu1_colorizeb = color_resize & 0xff;
    hardware_node->cbmcolorizealu1.bits.alu1_colorizeg = (color_resize >> 8) & 0xff;
    hardware_node->cbmcolorizealu1.bits.alu1_colorizer = (color_resize >> 16) & 0xff;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_void tde_hal_node_enable_alpha_rop(tde_hardware_node *hardware_node)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);
    hardware_node->cbmparaalu1.bits.alu1_blendropen = 1;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/*****************************************************************************
 * Function:      TdeHalNodeSetColorExp
 * Description:   set color extend or color revise parameter
 * Input:         hardware_node: pointer of node
 *                clut_cmd: Clut operation parameter
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_s32 tde_hal_node_set_clut_opt(tde_hardware_node *hardware_node, tde_clut_cmd *clut_cmd, hi_bool reload)
{
    hi_u32 u32Capability;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(hardware_node, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(clut_cmd, HI_FAILURE);

    tde_get_capability(&u32Capability);
    if (!(u32Capability & CLUT)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CLUT);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Capability);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It deos not support Clut");
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    hardware_node->src2_ctrl.bits.src2_clut_mode = (hi_u32)clut_cmd->clut_mode;
    /* set node */
    /* CNcomment:配置缓存节点 */
    if (clut_cmd->clut_phy_addr) {
        // hardware_node->clut_addr = clut_cmd->clut_phy_addr;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

STATIC hi_void TdeHalNodeSetYcbcrColorKeyPara(tde_hardware_node *hardware_node, tde_color_key_cmd *color_key)
{
    hi_tde_color_key_comp cr = {0};
    hi_tde_color_key_comp cb = {0};
    hi_tde_color_key_comp y = {0};
    hi_tde_color_key_comp alpha = {0};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(color_key);
    cr = color_key->color_key_value.ycbcr_color_key.cr;
    cb = color_key->color_key_value.ycbcr_color_key.cb;
    y = color_key->color_key_value.ycbcr_color_key.y;
    alpha = color_key->color_key_value.ycbcr_color_key.alpha;

    hardware_node->cbmkeyminalu1.u32 = TdeHalGetYCbCrKeyMask(cr.component_min, cb.component_min, y.component_min,
                                                             alpha.component_min);
    hardware_node->cbmkeymaxalu1.u32 = TdeHalGetYCbCrKeyMask(cr.component_max, cb.component_max, y.component_max,
                                                             alpha.component_max);
    hardware_node->cbmkeymaskalu1.u32 = TdeHalGetYCbCrKeyMask(cr.component_mask, cb.component_mask, y.component_mask,
                                                              alpha.component_mask);

    hardware_node->cbmkeyparaalu1.bits.alu1_keybmode = TdeHalGetColorKeyMode(&cr);
    hardware_node->cbmkeyparaalu1.bits.alu1_keygmode = TdeHalGetColorKeyMode(&cb);
    hardware_node->cbmkeyparaalu1.bits.alu1_keyrmode = TdeHalGetColorKeyMode(&y);
    hardware_node->cbmkeyparaalu1.bits.alu1_keyamode = TdeHalGetColorKeyMode(&alpha);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

STATIC hi_void TdeHalNodeSetCLUTColorKeyPara(tde_hardware_node *hardware_node, tde_color_key_cmd *color_key)
{
    hi_tde_color_key_comp clut = {0};
    hi_tde_color_key_comp alpha = {0};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(color_key);
    clut = color_key->color_key_value.clut_color_key.clut;
    alpha = color_key->color_key_value.clut_color_key.alpha;

    hardware_node->cbmkeyminalu1.u32 = TdeHalGetClutKeyMask(clut.component_min, alpha.component_min);
    hardware_node->cbmkeymaxalu1.u32 = TdeHalGetClutKeyMask(clut.component_max, alpha.component_max);
    hardware_node->cbmkeymaskalu1.u32 = TdeHalGetClutKeyMask(clut.component_mask, alpha.component_mask);

    hardware_node->cbmkeyparaalu1.bits.alu1_keybmode = TdeHalGetColorKeyMode(&clut);
    hardware_node->cbmkeyparaalu1.bits.alu1_keybmode = TdeHalGetColorKeyMode(&alpha);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

STATIC hi_void TdeHalNodeSetARGBColorKeyPara(tde_hardware_node *hardware_node, tde_color_key_cmd *color_key)
{
    hi_tde_color_key_comp blue = {0};
    hi_tde_color_key_comp green = {0};
    hi_tde_color_key_comp red = {0};
    hi_tde_color_key_comp alpha = {0};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(color_key);
    blue = color_key->color_key_value.argb_color_key.blue;
    green = color_key->color_key_value.argb_color_key.green;
    red = color_key->color_key_value.argb_color_key.red;
    alpha = color_key->color_key_value.argb_color_key.alpha;

    hardware_node->cbmkeyminalu1.bits.alu1_keyrmin = red.component_min << 2;   /* 2: 8bits to 10bits */
    hardware_node->cbmkeyminalu1.bits.alu1_keygmin = green.component_min << 2; /* 2: 8bits to 10bits */
    hardware_node->cbmkeyminalu1.bits.alu1_keybmin = blue.component_min << 2;  /* 2: 8bits to 10bits */
    hardware_node->cbmkeyaminalu1.bits.alu1_keyamin = alpha.component_min;

    hardware_node->cbmkeymaxalu1.bits.alu1_keyrmax = red.component_max << 2;   /* 2: 8bits to 10bits */
    hardware_node->cbmkeymaxalu1.bits.alu1_keygmax = green.component_max << 2; /* 2: 8bits to 10bits */
    hardware_node->cbmkeymaxalu1.bits.alu1_keybmax = blue.component_max << 2;  /* 2: 8bits to 10bits */
    hardware_node->cbmkeyamaxalu1.bits.alu1_keyamax = alpha.component_max;

    hardware_node->cbmkeymaskalu1.bits.alu1_keyrmask = red.component_mask << 2;   /* 2: 8bits to 10bits */
    hardware_node->cbmkeymaskalu1.bits.alu1_keygmask = green.component_mask << 2; /* 2: 8bits to 10bits */
    hardware_node->cbmkeymaskalu1.bits.alu1_keybmask = blue.component_mask << 2;  /* 2: 8bits to 10bits */
    hardware_node->cbmkeyamaskalu1.bits.alu1_keyamask = alpha.component_mask;     /* 8bits to 10bits */

    hardware_node->cbmkeyparaalu1.bits.alu1_keybmode = TdeHalGetColorKeyMode(&blue);
    hardware_node->cbmkeyparaalu1.bits.alu1_keygmode = TdeHalGetColorKeyMode(&green);
    hardware_node->cbmkeyparaalu1.bits.alu1_keyrmode = TdeHalGetColorKeyMode(&red);
    hardware_node->cbmkeyparaalu1.bits.alu1_keyamode = TdeHalGetColorKeyMode(&alpha);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

STATIC hi_u32 TdeHalGetColorKeyMode(hi_tde_color_key_comp *stColorKey)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(stColorKey, 0);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return (stColorKey->is_component_ignore)
                ? (hi_u32)TDE_COLORKEY_IGNORE
                : (stColorKey->is_component_out) ? (hi_u32)TDE_COLORKEY_AREA_OUT : (hi_u32)TDE_COLORKEY_AREA_IN;
}

STATIC hi_u32 TdeHalGetYCbCrKeyMask(hi_u8 u8Cr, hi_u8 u8Cb, hi_u8 u8Cy, hi_u8 alpha)
{
    return (hi_u32)(u8Cr | (u8Cb << TDE_EIGHT_BITS_SHIFT) | (u8Cy << TDE_SIXTEEN_BITS_SHIFT) |
                    (alpha << TDE_TWENTYFOUR_BITS_SHIFT));
}

STATIC hi_u32 TdeHalGetClutKeyMask(hi_u8 u8Clut, hi_u8 alpha)
{
    return (hi_u32)(u8Clut | (alpha << TDE_TWENTYFOUR_BITS_SHIFT));
}

/*****************************************************************************
 * Function:      tde_hal_node_set_colorkey
 * Description:  set parameter for color key operation  according color format
 * Input:         hardware_node:pointer of node
 *                color_fmt: color format
 *                color_key: pointer of color key value
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_s32 tde_hal_node_set_colorkey(tde_hardware_node *hardware_node, tde_color_fmt_category fmt_cat,
                                 tde_color_key_cmd *color_key)
{
    hi_u32 u32Capability;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(hardware_node, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(color_key, HI_FAILURE);

    tde_get_capability(&u32Capability);

    if (!(u32Capability & COLORKEY)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, COLORKEY);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Capability);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It deos not support ColorKey");
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    hardware_node->cbmkeyparaalu1.bits.alu1_keysel = (hi_u32)color_key->color_key_mode;
    hardware_node->cbmkeyparaalu1.bits.alu1_keyen = 1;

    if (fmt_cat == TDE_COLORFMT_CATEGORY_ARGB) {
        TdeHalNodeSetARGBColorKeyPara(hardware_node, color_key);
    } else if (fmt_cat == TDE_COLORFMT_CATEGORY_CLUT) {
        TdeHalNodeSetCLUTColorKeyPara(hardware_node, color_key);
    } else if (fmt_cat == TDE_COLORFMT_CATEGORY_YCbCr) {
        TdeHalNodeSetYcbcrColorKeyPara(hardware_node, color_key);
    } else {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, fmt_cat);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It deos not support ColorKey");
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    hardware_node->cbm_ctrl.bits.cbm_en = 1;
    hardware_node->cbm_ctrl.bits.cbm_mode = 1;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

/*****************************************************************************
 * Function:      tde_hal_node_set_clipping
 * Description:  set clip operation parameter
 * Input:         hardware_node: pointer of node
 *                clip: clip range
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_s32 tde_hal_node_set_clipping(tde_hardware_node *hardware_node, tde_clip_cmd *clip)
{
    hi_u32 u32Capability = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(hardware_node, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(clip, HI_FAILURE);

    tde_get_capability(&u32Capability);
    if (!(u32Capability & CLIP)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CLIP);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32Capability);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It deos not support Clip");
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    hardware_node->dst_alpha.bits.dst_clip_mode = 0;
    if (!clip->is_inside_clip) {
        /* over clip */
        /* CNcomment: 区域外clip指示 */
        hardware_node->dst_alpha.bits.dst_clip_mode = 1;
    }

    hardware_node->dst_alpha.bits.dst_clip_en = 1;
    hardware_node->dst_crop_pos_start.bits.dst_crop_start_x = clip->clip_start_x;
    hardware_node->dst_crop_pos_start.bits.dst_crop_start_y = clip->clip_start_y;
    hardware_node->dst_crop_pos_end.bits.dst_crop_end_x = clip->clip_end_x;
    hardware_node->dst_crop_pos_end.bits.dst_crop_end_y = clip->clip_end_y;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}
#endif

STATIC hi_u32 TdeHalGetZmeInFmt(tde_color_fmt enInDrvFmt)
{
    hi_u32 u32ZmeInFmt = 0;
    hi_bool bFmtArgb = (enInDrvFmt <= TDE_DRV_COLOR_FMT_ACLUT88) || (enInDrvFmt == TDE_DRV_COLOR_FMT_YCbCr444MB) ||
                       (enInDrvFmt == TDE_DRV_COLOR_FMT_YCbCr400MBP);
    hi_bool bFmtYuv422 = (enInDrvFmt == TDE_DRV_COLOR_FMT_YCbCr422MBH) || (enInDrvFmt == TDE_DRV_COLOR_FMT_YCbCr422);
    hi_bool bFmtYuv420 = (enInDrvFmt == TDE_DRV_COLOR_FMT_YCbCr420MB) || (enInDrvFmt == TDE_DRV_COLOR_FMT_YCbCr422MBV);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (bFmtArgb) {
        u32ZmeInFmt = TDE_ZME_FMT_ARGB;
    }

    if (bFmtYuv422) {
        u32ZmeInFmt = TDE_ZME_FMT_YUV422;
    }

    if (bFmtYuv420) {
        u32ZmeInFmt = TDE_ZME_FMT_YUV420;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return u32ZmeInFmt;
}

STATIC hi_u32 TdeHalGetHpzmeMode(hi_u32 u32OutRectWidth, hi_u32 u32InRectWidth)
{
    hi_u32 u32HpzmeMode = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((u32OutRectWidth * 32) >= u32InRectWidth) {
        u32HpzmeMode = 1;
    } else if ((u32OutRectWidth * 64) >= u32InRectWidth) {
        u32HpzmeMode = 3;
    } else if ((u32OutRectWidth * 128) >= u32InRectWidth) {
        u32HpzmeMode = 7;
    } else {
        u32HpzmeMode = 15;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return u32HpzmeMode;
}

STATIC hi_u32 TdeHalGetZoomMode(hi_tde_filter_mode filter_mode)
{
    hi_bool bFilterModeColor = (filter_mode == HI_TDE_FILTER_MODE_COLOR);
    hi_bool bFilterModeAlpha = (filter_mode == HI_TDE_FILTER_MODE_ALPHA);
    hi_bool bFilterModeBoth = (filter_mode == HI_TDE_FILTER_MODE_BOTH);
    hi_u32 u32FirstBit = 0;
    hi_u32 u32SecondBit = 0;
    hi_u32 u32ThirdBit = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (bFilterModeColor) {
        u32FirstBit = 1;
        u32SecondBit = 1;
        u32ThirdBit = 0;
    } else if (bFilterModeAlpha) {
        u32FirstBit = 0;
        u32SecondBit = 0;
        u32ThirdBit = 1;
    } else if (bFilterModeBoth) {
        u32FirstBit = 1;
        u32SecondBit = 1;
        u32ThirdBit = 1;
    } else {
        u32FirstBit = 0;
        u32SecondBit = 0;
        u32ThirdBit = 0;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
#ifndef CONFIG_TDE_USE_FLITER_MODE
    return 0;
#endif
    return (u32ThirdBit << TDE_HAL_TWO_BIT_SHIFT) + (u32SecondBit << TDE_HAL_ONE_BIT_SHIFT) +
           (u32FirstBit << TDE_HAL_ZERO_BIT_SHIFT);
}

STATIC hi_u32 TdeHalGetHeightScaleCoff(hi_u32 u32OutRectHeight, hi_u32 u32InRectHeight)
{
    hi_u32 u32HeightScaleCoff = 1;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((u32OutRectHeight * 32) >= u32InRectHeight) {
        u32HeightScaleCoff = 2;
    } else if ((u32OutRectHeight * 64) >= u32InRectHeight) {
        u32HeightScaleCoff = 4;
    } else if ((u32OutRectHeight * 128) >= u32InRectHeight) {
        u32HeightScaleCoff = 8;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return u32HeightScaleCoff;
}

#ifndef HI_BUILD_IN_BOOT
hi_void tde_hal_calc_src1_filter_opt(tde_hardware_node *hardware_node, hi_tde_color_fmt in_color_fmt,
                                     hi_tde_color_fmt out_color_fmt, hi_tde_rect *in_rect, hi_tde_rect *out_rect,
                                     hi_tde_filter_mode filter_mode)
{
#ifdef CONFIG_GFX_TDE_VERSION_3_0
    // HI_UNUSED(out_color_fmt);
    // HI_UNUSED(in_rect);
    // HI_UNUSED(out_rect);
    // HI_UNUSED(filter_mode);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);

    if (HI_TDE_COLOR_FMT_YCbCr422 != in_color_fmt) {
        GRAPHIC_DFX_DEBUG_INFO(GRAPHIC_DFX_DEBUG_INFOMATION, "HI_TDE_COLOR_FMT_YCbCr422 != in_color_fmt");
        return;
    }
    hardware_node->src1_hfir_ctrl.bits.hfir_mode = 0x1;
#else
    tde_color_fmt enInDrvFmt;
    hi_u32 u32TempZoomMode;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(in_rect);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(out_rect);

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(in_color_fmt, HI_TDE_COLOR_FMT_MAX + 1);
    enInDrvFmt = g_enTdeCommonDrvColorFmt[in_color_fmt];

    hardware_node->src1_vsp.bits.src1_zme_in_fmt = TdeHalGetZmeInFmt(enInDrvFmt);

    hardware_node->src1_vsp.bits.src1_zme_out_fmt = (hardware_node->src1_vsp.bits.src1_zme_in_fmt == TDE_ZME_FMT_ARGB)
                                                        ? TDE_ZME_FMT_ARGB
                                                        : TDE_ZME_FMT_YUV444;

    hardware_node->src1_image_size.bits.src1_width = in_rect->width - 1;
    hardware_node->src1_zme_in_reso.bits.src1_iw = in_rect->width - 1;
    hardware_node->src1_zme_out_reso.bits.src1_ow = out_rect->width - 1;

    hardware_node->src1_image_size.bits.src1_height = in_rect->height - 1;
    hardware_node->src1_zme_in_reso.bits.src1_ih = in_rect->height - 1;
    hardware_node->src1_zme_out_reso.bits.src1_oh = out_rect->height - 1;

    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(0, out_rect->width);

    hardware_node->src1_hsp.bits.src1_hratio =
        (1 >= out_rect->width)
            ? 0
            : osal_div64_u64(((hi_u64)(in_rect->width) << TDE_HAL_HSTEP_FLOATLEN), (out_rect->width));

    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(0, out_rect->height);
    hardware_node->src1_vsr.bits.src1_vratio = (1 >= out_rect->height)
                                                    ? 0
                                                    : ((in_rect->height) << TDE_FLOAT_BITLEN) / (out_rect->height);

    hardware_node->src1_hpzme.bits.src1_hpzme_width = in_rect->width;

    if ((out_rect->width * 16) <= in_rect->width) {
        hardware_node->src1_hpzme.bits.src1_hpzme_en = 1;

        hardware_node->src1_hpzme.bits.src1_hpzme_mode = TdeHalGetHpzmeMode(out_rect->width, in_rect->width);

        GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(0, hardware_node->src1_hpzme.bits.src1_hpzme_mode + 1);
        hardware_node->src1_hpzme.bits.src1_hpzme_width =
            in_rect->width / (hardware_node->src1_hpzme.bits.src1_hpzme_mode + 1) +
            ((in_rect->width % (hardware_node->src1_hpzme.bits.src1_hpzme_mode + 1)) ? 1 : 0);
    }

    hardware_node->src1_hsp.bits.src1_hchmsc_en = 1;
    hardware_node->src1_hsp.bits.src1_hlmsc_en = 1;

    u32TempZoomMode = TdeHalGetZoomMode(filter_mode);

    if (hardware_node->src1_hsp.bits.src1_hratio != NO_HSCALE_STEP) {
        hardware_node->src1_hsp.bits.src1_hfir_order = 0;

        hardware_node->src1_hsp.bits.src1_hafir_en = (u32TempZoomMode >> TDE_HAL_TWO_BIT_SHIFT) & 0x1;
        hardware_node->src1_hsp.bits.src1_hlfir_en = (u32TempZoomMode >> TDE_HAL_ONE_BIT_SHIFT) & 0x1;
        hardware_node->src1_hsp.bits.src1_hchfir_en = (u32TempZoomMode >> TDE_HAL_ZERO_BIT_SHIFT) & 0x1;

        hardware_node->tde_src1_zme_haddr_high.bits.src1_scl_h_high = HI_GFX_GET_HIGH_PART(
            g_para_table.u32HfCoefAddr +
            TdeHalGetResizeParaHTable(hardware_node->src1_hsp.bits.src1_hratio) * TDE_PARA_HTABLE_SIZE);

        hardware_node->tde_src1_zme_haddr_low.bits.src1_scl_h_low = HI_GFX_GET_LOW_PART(
            g_para_table.u32HfCoefAddr +
            TdeHalGetResizeParaHTable(hardware_node->src1_hsp.bits.src1_hratio) * TDE_PARA_HTABLE_SIZE);
    }

    hardware_node->src1_vsp.bits.src1_vchmsc_en = 1;
    hardware_node->src1_vsp.bits.src1_vlmsc_en = 1;

    if (hardware_node->src1_vsr.bits.src1_vratio != NO_VSCALE_STEP) {
        hardware_node->src1_vsp.bits.src1_vafir_en = (u32TempZoomMode >> TDE_HAL_TWO_BIT_SHIFT) & 0x1;
        hardware_node->src1_vsp.bits.src1_vlfir_en = (u32TempZoomMode >> TDE_HAL_ONE_BIT_SHIFT) & 0x1;
        hardware_node->src1_vsp.bits.src1_vchfir_en = (u32TempZoomMode >> TDE_HAL_ZERO_BIT_SHIFT) & 0x1;

        hardware_node->tde_src1_zme_vaddr_high.u32 = HI_GFX_GET_HIGH_PART(
            g_para_table.u32VfCoefAddr +
            TdeHalGetResizeParaVTable(hardware_node->src1_vsr.bits.src1_vratio) * TDE_PARA_VTABLE_SIZE);

        hardware_node->tde_src1_zme_vaddr_low.u32 = HI_GFX_GET_LOW_PART(
            g_para_table.u32VfCoefAddr +
            TdeHalGetResizeParaVTable(hardware_node->src1_vsr.bits.src1_vratio) * TDE_PARA_VTABLE_SIZE);
    }

    hardware_node->src1_zme_in_reso.bits.src1_iw = hardware_node->src1_hpzme.bits.src1_hpzme_width - 1;

    hardware_node->src1_hpzme.bits.src1_hpzme_width = hardware_node->src1_hpzme.bits.src1_hpzme_width - 1;
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif

hi_void tde_hal_calc_src2_filter_opt(tde_hardware_node *hardware_node, hi_tde_color_fmt in_color_fmt,
                                     hi_tde_color_fmt out_color_fmt, hi_tde_rect *in_rect, hi_tde_rect *out_rect,
                                     hi_bool defilicker, hi_tde_filter_mode filter_mode)
{
    tde_color_fmt enInDrvFmt;
    hi_u32 u32TempZoomMode;
    hi_u32 u32TempHeightScaleCoff = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(hardware_node);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(in_rect);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(out_rect);

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(in_color_fmt, HI_TDE_COLOR_FMT_MAX + 1);
    enInDrvFmt = g_enTdeCommonDrvColorFmt[in_color_fmt];

    hardware_node->src2_vsp.bits.src2_zme_in_fmt = TdeHalGetZmeInFmt(enInDrvFmt);

    hardware_node->src2_vsp.bits.src2_zme_out_fmt = (hardware_node->src2_vsp.bits.src2_zme_in_fmt == TDE_ZME_FMT_ARGB)
                                                        ? TDE_ZME_FMT_ARGB
                                                        : TDE_ZME_FMT_YUV444;

    hardware_node->src2_image_size.bits.src2_width = in_rect->width - 1;
    hardware_node->src2_zme_in_reso.bits.src2_iw = in_rect->width - 1;
    hardware_node->src2_zme_out_reso.bits.src2_ow = out_rect->width - 1;
    hardware_node->src2_zme_out_reso.bits.src2_oh = out_rect->height - 1;
    hardware_node->src2_image_size.bits.src2_height = in_rect->height - 1;
    hardware_node->src2_zme_in_reso.bits.src2_ih = in_rect->height - 1;

    hardware_node->dst_image_size.bits.dst_width = out_rect->width - 1;
    hardware_node->dst_image_size.bits.dst_height = out_rect->height - 1;

    hardware_node->cbm_imgsize.bits.cbm_width = hardware_node->dst_image_size.bits.dst_width;
    hardware_node->cbm_imgsize.bits.cbm_height = hardware_node->dst_image_size.bits.dst_height;

    GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(0, out_rect->width);
#ifdef HI_BUILD_IN_BOOT
    hardware_node->src2_hsp.bits.src2_hratio = (1 >= out_rect->width)
                                                    ? 0
                                                    : ((in_rect->width) << TDE_HAL_HSTEP_FLOATLEN) / (out_rect->width);
#else
    hardware_node->src2_hsp.bits.src2_hratio =
        (1 >= out_rect->width)
            ? 0
            : osal_div64_u64(((hi_u64)(in_rect->width) << TDE_HAL_HSTEP_FLOATLEN), (out_rect->width));
#endif

    hardware_node->src2_hpzme.bits.src2_hpzme_width = in_rect->width;

    if ((out_rect->width * 16) <= in_rect->width) {
        hardware_node->src2_hpzme.bits.src2_hpzme_en = 1;

        hardware_node->src2_hpzme.bits.src2_hpzme_mode = TdeHalGetHpzmeMode(out_rect->width, in_rect->width);

        GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(0, hardware_node->src2_hpzme.bits.src2_hpzme_mode + 1);
        hardware_node->src2_hpzme.bits.src2_hpzme_width =
            in_rect->width / (hardware_node->src2_hpzme.bits.src2_hpzme_mode + 1) +
            ((in_rect->width % (hardware_node->src2_hpzme.bits.src2_hpzme_mode + 1)) ? 1 : 0);
        hardware_node->src2_hsp.bits.src2_hratio =
            (1 >= out_rect->width)
                ? 0
                : (((hardware_node->src2_hpzme.bits.src2_hpzme_width) << TDE_HAL_HSTEP_FLOATLEN) / (out_rect->width));
    }

    if ((out_rect->height * 16) <= in_rect->height) {
        u32TempHeightScaleCoff = TdeHalGetHeightScaleCoff(out_rect->height, in_rect->height);

        GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(0, u32TempHeightScaleCoff);
        hardware_node->src2_zme_in_reso.bits.src2_ih = in_rect->height / u32TempHeightScaleCoff - 1;
        hardware_node->src2_image_size.bits.src2_height = in_rect->height / u32TempHeightScaleCoff - 1;

        hardware_node->src2_ch0_stride.bits.src2_ch0_stride = hardware_node->src2_ch0_stride.bits.src2_ch0_stride *
                                                              u32TempHeightScaleCoff;

        GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(0, out_rect->height);
        hardware_node->src2_vsr.bits.src2_vratio =
            ((hardware_node->src2_image_size.bits.src2_height) << TDE_FLOAT_BITLEN) / (out_rect->height);
    } else {
        GRAPHIC_CHECK_LEFT_EQUAL_RIGHT_RETURN_NOVALUE(0, out_rect->height);
        hardware_node->src2_vsr.bits.src2_vratio = (1 >= out_rect->height)
                                                        ? 0
                                                        : ((in_rect->height) << TDE_FLOAT_BITLEN) / (out_rect->height);
    }

    hardware_node->src2_hsp.bits.src2_hchmsc_en = 1;
    hardware_node->src2_hsp.bits.src2_hlmsc_en = 1;

    u32TempZoomMode = TdeHalGetZoomMode(filter_mode);

    if (hardware_node->src2_hsp.bits.src2_hratio != NO_HSCALE_STEP) {
        hardware_node->src2_hsp.bits.src2_hfir_order = 0;

        hardware_node->src2_hsp.bits.src2_hafir_en = (u32TempZoomMode >> TDE_HAL_TWO_BIT_SHIFT) & 0x1;
        hardware_node->src2_hsp.bits.src2_hlfir_en = (u32TempZoomMode >> TDE_HAL_ONE_BIT_SHIFT) & 0x1;
        hardware_node->src2_hsp.bits.src2_hchfir_en = (u32TempZoomMode >> TDE_HAL_ZERO_BIT_SHIFT) & 0x1;

        hardware_node->tde_src2_zme_haddr_high.bits.src2_scl_h_high = HI_GFX_GET_HIGH_PART(
            g_para_table.u32HfCoefAddr +
            TdeHalGetResizeParaHTable(hardware_node->src2_hsp.bits.src2_hratio) * TDE_PARA_HTABLE_SIZE);
        hardware_node->tde_src2_zme_haddr_low.u32 = HI_GFX_GET_LOW_PART(
            g_para_table.u32HfCoefAddr +
            TdeHalGetResizeParaHTable(hardware_node->src2_hsp.bits.src2_hratio) * TDE_PARA_HTABLE_SIZE);
    }

    hardware_node->src2_vsp.bits.src2_vchmsc_en = 1;
    hardware_node->src2_vsp.bits.src2_vlmsc_en = 1;

    if ((hardware_node->src2_vsr.bits.src2_vratio != NO_VSCALE_STEP) && (!defilicker)) {
        hardware_node->src2_vsp.bits.src2_vafir_en = (u32TempZoomMode >> TDE_HAL_TWO_BIT_SHIFT) & 0x1;
        hardware_node->src2_vsp.bits.src2_vlfir_en = (u32TempZoomMode >> TDE_HAL_ONE_BIT_SHIFT) & 0x1;
        hardware_node->src2_vsp.bits.src2_vchfir_en = (u32TempZoomMode >> TDE_HAL_ZERO_BIT_SHIFT) & 0x1;

        hardware_node->tde_src2_zme_vaddr_high.u32 = HI_GFX_GET_HIGH_PART(
            g_para_table.u32VfCoefAddr +
            TdeHalGetResizeParaVTable(hardware_node->src2_vsr.bits.src2_vratio) * TDE_PARA_VTABLE_SIZE);

        hardware_node->tde_src2_zme_vaddr_low.u32 = HI_GFX_GET_LOW_PART(
            g_para_table.u32VfCoefAddr +
            TdeHalGetResizeParaVTable(hardware_node->src2_vsr.bits.src2_vratio) * TDE_PARA_VTABLE_SIZE);
    }
#ifndef HI_BUILD_IN_BOOT
    if (defilicker) {
        hardware_node->src2_vsp.bits.src2_vchmsc_en = 1;
        hardware_node->src2_vsp.bits.src2_vlmsc_en = 1;
        hardware_node->src2_vsp.bits.src2_vafir_en = 1;
        hardware_node->src2_vsp.bits.src2_vlfir_en = 1;
        hardware_node->src2_vsp.bits.src2_vchfir_en = 1;

        hardware_node->tde_src2_zme_vaddr_high.bits.src2_scl_v_high = HI_GFX_GET_HIGH_PART(
            g_para_table.u32DeflickerVfCoefAddr +
            TdeHalGetResizeParaVTable(hardware_node->src2_vsr.bits.src2_vratio) * TDE_PARA_VTABLE_SIZE);
        hardware_node->tde_src2_zme_vaddr_low.bits.src2_scl_v_low = HI_GFX_GET_LOW_PART(
            g_para_table.u32DeflickerVfCoefAddr +
            TdeHalGetResizeParaVTable(hardware_node->src2_vsr.bits.src2_vratio) * TDE_PARA_VTABLE_SIZE);
    }
#endif

    hardware_node->src2_zme_in_reso.bits.src2_iw = hardware_node->src2_hpzme.bits.src2_hpzme_width - 1;

    hardware_node->src2_hpzme.bits.src2_hpzme_width = hardware_node->src2_hpzme.bits.src2_hpzme_width - 1;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

#ifndef HI_BUILD_IN_BOOT
hi_s32 tde_set_node_csc(tde_hardware_node *hw_node, hi_tde_csc_opt *csc_opt)
{
    /* CSC需要与算法人员对齐 */
#if 0
    hi_u8 *csc_vir_addr = NULL;
    hi_u32 src_csc_expect_idc0_size;
    hi_u32 src_csc_expect_size;
    hi_u32 dst_csc_expect_idc0_size;
    hi_u32 dst_csc_expect_size;
#ifdef CONFIG_GFX_TDE_VERSION_3_0
    hi_u32 src_csc_expect_idc1_size;
    hi_u32 dst_csc_expect_idc1_size;
#endif

    if ((hw_node == NULL) || (csc_opt == NULL)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, hw_node);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, csc_opt);
        return HI_FAILURE;
    }

    src_csc_expect_idc0_size = TDE_SRC_CSC_IDC0_SIZE;
    dst_csc_expect_idc0_size = TDE_DST_CSC_IDC0_SIZE;
#ifdef CONFIG_GFX_TDE_VERSION_3_0
    src_csc_expect_idc1_size = TDE_SRC_CSC_IDC1_SIZE;
    dst_csc_expect_idc1_size = TDE_DST_CSC_IDC1_SIZE;
    src_csc_expect_size = src_csc_expect_idc0_size + src_csc_expect_idc1_size;
    dst_csc_expect_size = dst_csc_expect_idc0_size + dst_csc_expect_idc1_size;
#else
    src_csc_expect_size = src_csc_expect_idc0_size;
    dst_csc_expect_size = dst_csc_expect_idc0_size;
#endif

    if (csc_opt->src_csc_user_en == HI_TRUE) {
        if (src_csc_expect_size != csc_opt->src_csc_param_size) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, src_csc_expect_size);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, csc_opt->src_csc_param_size);
            return HI_ERR_TDE_NO_MEM;
        }
        csc_vir_addr = HI_GFX_Map(csc_opt->src_csc_param_addr, src_csc_expect_size, HI_TRUE);
        if (csc_vir_addr == NULL) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_Map, FAILURE_TAG);
            return HI_ERR_TDE_NO_MEM;
        }
        memcpy(&hw_node->src1_csc_idc0.all_bits, csc_vir_addr, src_csc_expect_size);
        HI_GFX_Unmap(csc_vir_addr, HI_TRUE);
        csc_vir_addr = NULL;
    }

    if (csc_opt->dst_csc_user_en == HI_TRUE) {
        if (dst_csc_expect_size != csc_opt->dst_csc_param_size) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, dst_csc_expect_size);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, csc_opt->dst_csc_param_size);
            return HI_ERR_TDE_NO_MEM;
        }
        csc_vir_addr = HI_GFX_Map(csc_opt->dst_csc_param_addr, dst_csc_expect_size, HI_TRUE);
        if (csc_vir_addr == NULL) {
            GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_Map, FAILURE_TAG);
            return HI_ERR_TDE_NO_MEM;
        }
        memcpy(&hw_node->dst_csc_idc0.all_bits, csc_vir_addr, dst_csc_expect_size);
        HI_GFX_Unmap(csc_vir_addr, HI_TRUE);
        csc_vir_addr = NULL;
    }
#endif
    return HI_SUCCESS;
}
#endif

STATIC hi_void tde_hal_node_csc_inconv_rgb2yuv(tde_hal_csc_para *csc_para)
{
    csc_para->csc_coef00 = 5966;   /* csc coef00 algorithm: 5966 */
    csc_para->csc_coef01 = 20071;  /* csc coef01 algorithm: 20071 */
    csc_para->csc_coef02 = 2026;   /* csc coef02 algorithm: 2026 */
    csc_para->csc_coef10 = -3289;  /* csc coef10 algorithm: -3289 */
    csc_para->csc_coef11 = -11063; /* csc coef11 algorithm: -11063 */
    csc_para->csc_coef12 = 14352;  /* csc coef12 algorithm: 14352 */
    csc_para->csc_coef20 = 14352;  /* csc coef20 algorithm: 14352 */
    csc_para->csc_coef21 = -13021; /* csc coef21 algorithm: -13021 */
    csc_para->csc_coef22 = -1316;  /* csc coef022 algorithm: -1316 */
    csc_para->idc0 = 0;            /* csc idc0 algorithm: 0 */
    csc_para->idc1 = 0;            /* csc idc1 algorithm: 0 */
    csc_para->idc2 = 0;            /* csc idc2 algorithm: 0 */
    csc_para->odc0 = 64;           /* csc odc0 algorithm: 64 */
    csc_para->odc1 = 512;          /* csc odc1 algorithm: 512 */
    csc_para->odc2 = 512;          /* csc odc2 algorithm: 512 */
    csc_para->scale = 15;          /* csc scale algorithm: 15 */
}

STATIC hi_void tde_hal_node_csc_inconv_yuv2rgb(tde_hal_csc_para *csc_para)
{
    csc_para->csc_coef00 = 1024; /* csc coef00 algorithm: 1024 */
    csc_para->csc_coef01 = 0;    /* csc coef01 algorithm: 0 */
    csc_para->csc_coef02 = 1576; /* csc coef02 algorithm: 1576 */
    csc_para->csc_coef10 = 1024; /* csc coef10 algorithm: 1024 */
    csc_para->csc_coef11 = -187; /* csc coef11 algorithm: -187 */
    csc_para->csc_coef12 = -470; /* csc coef12 algorithm: -470 */
    csc_para->csc_coef20 = 1024; /* csc coef20 algorithm: 1024 */
    csc_para->csc_coef21 = 1859; /* csc coef21 algorithm: 1859 */
    csc_para->csc_coef22 = 0;    /* csc coef22 algorithm: 0 */
    csc_para->idc0 = -64;        /* csc idc0 algorithm: -64 */
    csc_para->idc1 = -512;       /* csc idc1 algorithm: -512 */
    csc_para->idc2 = -512;       /* csc idc2 algorithm: -512 */
    csc_para->odc0 = 0;          /* csc odc0 algorithm: 0 */
    csc_para->odc1 = 0;          /* csc odc1 algorithm: 0 */
    csc_para->odc2 = 0;          /* csc odc2 algorithm: 0 */
    csc_para->scale = 10;        /* csc scale algorithm: 10 */
}

STATIC hi_void tde_hal_node_csc_outconv_rgb2yuv(tde_hal_csc_para *csc_para)
{
    csc_para->csc_coef00 = 5966;   /* csc coef00 algorithm: 5966 */
    csc_para->csc_coef01 = 20071;  /* csc coef01 algorithm: 20071 */
    csc_para->csc_coef02 = 2026;   /* csc coef02 algorithm: 2026 */
    csc_para->csc_coef10 = -3289;  /* csc coef10 algorithm: -3289 */
    csc_para->csc_coef11 = -11063; /* csc coef11 algorithm: -11063 */
    csc_para->csc_coef12 = 14352;  /* csc coef12 algorithm: 14352 */
    csc_para->csc_coef20 = 14352;  /* csc coef20 algorithm: 14352 */
    csc_para->csc_coef21 = -13021; /* csc coef21 algorithm: -13021 */
    csc_para->csc_coef22 = -1316;  /* csc coef22 algorithm: -1316 */
    csc_para->idc0 = 0;            /* csc idc0 algorithm: 0 */
    csc_para->idc1 = 0;            /* csc idc1 algorithm: 0 */
    csc_para->idc2 = 0;            /* csc idc2 algorithm: 0 */
    csc_para->odc0 = 64;           /* csc odc0 algorithm: 64 */
    csc_para->odc1 = 512;          /* csc odc1 algorithm: 512 */
    csc_para->odc2 = 512;          /* csc odc2 algorithm: 512 */
    csc_para->scale = 15;          /* csc scale algorithm: 15 */
}

STATIC hi_void tde_hal_node_csc_outconv_yuv2rgb(tde_hal_csc_para *csc_para)
{
    csc_para->csc_coef00 = 1024; /* csc coef00 algorithm: 1024 */
    csc_para->csc_coef01 = 0;    /* csc coef01 algorithm: 0 */
    csc_para->csc_coef02 = 1576; /* csc coef02 algorithm: 1576 */
    csc_para->csc_coef10 = 1024; /* csc coef10 algorithm: 1024 */
    csc_para->csc_coef11 = -187; /* csc coef11 algorithm: -187 */
    csc_para->csc_coef12 = -470; /* csc coef12 algorithm: -470 */
    csc_para->csc_coef20 = 1024; /* csc coef20 algorithm: 1024 */
    csc_para->csc_coef21 = 1859; /* csc coef21 algorithm: 1859 */
    csc_para->csc_coef22 = 0;    /* csc coef22 algorithm: 0 */
    csc_para->idc0 = -64;        /* csc idc0 algorithm: -64 */
    csc_para->idc1 = -512;       /* csc idc1 algorithm: -512 */
    csc_para->idc2 = -512;       /* csc idc2 algorithm: -512 */
    csc_para->odc0 = 0;          /* csc odc0 algorithm: 0 */
    csc_para->odc1 = 0;          /* csc odc1 algorithm: 0 */
    csc_para->odc2 = 0;          /* csc odc2 algorithm: 0 */
    csc_para->scale = 10;        /* csc scale algorithm: 10 */
}

STATIC hi_void tde_hal_set_src_csc(tde_hardware_node *hardware_node, tde_hal_csc_para *csc_para)
{
    hardware_node->src_hipp_csc_coef00.bits.src_hipp_csc_coef00 = csc_para->csc_coef00;
    hardware_node->src_hipp_csc_coef01.bits.src_hipp_csc_coef01 = csc_para->csc_coef01;
    hardware_node->src_hipp_csc_coef02.bits.src_hipp_csc_coef02 = csc_para->csc_coef02;
    hardware_node->src_hipp_csc_coef10.bits.src_hipp_csc_coef10 = csc_para->csc_coef10;
    hardware_node->src_hipp_csc_coef11.bits.src_hipp_csc_coef11 = csc_para->csc_coef11;
    hardware_node->src_hipp_csc_coef12.bits.src_hipp_csc_coef12 = csc_para->csc_coef12;
    hardware_node->src_hipp_csc_coef20.bits.src_hipp_csc_coef20 = csc_para->csc_coef20;
    hardware_node->src_hipp_csc_coef21.bits.src_hipp_csc_coef21 = csc_para->csc_coef21;
    hardware_node->src_hipp_csc_coef22.bits.src_hipp_csc_coef22 = csc_para->csc_coef22;
    hardware_node->src_hipp_csc_idc0.bits.src_hipp_csc_idc0 = csc_para->idc0;
    hardware_node->src_hipp_csc_idc1.bits.src_hipp_csc_idc1 = csc_para->idc1;
    hardware_node->src_hipp_csc_idc2.bits.src_hipp_csc_idc2 = csc_para->idc2;
    hardware_node->src_hipp_csc_odc0.bits.src_hipp_csc_odc0 = csc_para->odc0;
    hardware_node->src_hipp_csc_odc1.bits.src_hipp_csc_odc1 = csc_para->odc1;
    hardware_node->src_hipp_csc_odc2.bits.src_hipp_csc_odc2 = csc_para->odc2;
    hardware_node->src_hipp_csc_scale.bits.src_hipp_csc_scale = csc_para->scale;
    hardware_node->src_hipp_csc_min_y.bits.src_hipp_csc_min_y = 0;
    hardware_node->src_hipp_csc_min_c.bits.src_hipp_csc_min_c = 0;
    hardware_node->src_hipp_csc_max_y.bits.src_hipp_csc_max_y = 0x3ff; /* csc max y: 0x3ff */
    hardware_node->src_hipp_csc_max_c.bits.src_hipp_csc_max_c = 0x3ff; /* csc max c: 0x3ff */
    hardware_node->tde_csc_ctrl.bits.src_csc_premulten = 0;
    hardware_node->tde_csc_ctrl.bits.dst_csc_premulten = 0;
    hardware_node->src_hipp_csc_ctrl.bits.src_hipp_csc_en = csc_para->csc_en;
    hardware_node->src_hipp_csc_ctrl.bits.src_hipp_csc_demo_en = 0;
    hardware_node->src_hipp_csc_ctrl.bits.src_hipp_csc_ck_gt_en = 1;
    hardware_node->tde_csc_ctrl.bits.src_csc_mode = csc_para->csc_mode_src2;
}

STATIC hi_void tde_hal_set_dst_csc(tde_hardware_node *hardware_node, tde_hal_csc_para *csc_para)
{
    hardware_node->dst_hipp_csc_coef00.bits.dst_hipp_csc_coef00 = csc_para->csc_coef00;
    hardware_node->dst_hipp_csc_coef01.bits.dst_hipp_csc_coef01 = csc_para->csc_coef01;
    hardware_node->dst_hipp_csc_coef02.bits.dst_hipp_csc_coef02 = csc_para->csc_coef02;
    hardware_node->dst_hipp_csc_coef10.bits.dst_hipp_csc_coef10 = csc_para->csc_coef10;
    hardware_node->dst_hipp_csc_coef11.bits.dst_hipp_csc_coef11 = csc_para->csc_coef11;
    hardware_node->dst_hipp_csc_coef12.bits.dst_hipp_csc_coef12 = csc_para->csc_coef12;
    hardware_node->dst_hipp_csc_coef20.bits.dst_hipp_csc_coef20 = csc_para->csc_coef20;
    hardware_node->dst_hipp_csc_coef21.bits.dst_hipp_csc_coef21 = csc_para->csc_coef21;
    hardware_node->dst_hipp_csc_coef22.bits.dst_hipp_csc_coef22 = csc_para->csc_coef22;
    hardware_node->dst_hipp_csc_idc0.bits.dst_hipp_csc_idc0 = csc_para->idc0;
    hardware_node->dst_hipp_csc_idc1.bits.dst_hipp_csc_idc1 = csc_para->idc1;
    hardware_node->dst_hipp_csc_idc2.bits.dst_hipp_csc_idc2 = csc_para->idc2;
    hardware_node->dst_hipp_csc_odc0.bits.dst_hipp_csc_odc0 = csc_para->odc0;
    hardware_node->dst_hipp_csc_odc1.bits.dst_hipp_csc_odc1 = csc_para->odc1;
    hardware_node->dst_hipp_csc_odc2.bits.dst_hipp_csc_odc2 = csc_para->odc2;
    hardware_node->dst_hipp_csc_scale.bits.dst_hipp_csc_scale = csc_para->scale;
    hardware_node->dst_hipp_csc_min_y.bits.dst_hipp_csc_min_y = 0;
    hardware_node->dst_hipp_csc_min_c.bits.dst_hipp_csc_min_c = 0;
    hardware_node->dst_hipp_csc_max_y.bits.dst_hipp_csc_max_y = 0x3ff; /* csc max y: 0x3ff */
    hardware_node->dst_hipp_csc_max_c.bits.dst_hipp_csc_max_c = 0x3ff; /* csc max c: 0x3ff */
    hardware_node->tde_csc_ctrl.bits.dst_csc_premulten = 0;
    hardware_node->dst_hipp_csc_ctrl.bits.dst_hipp_csc_en = csc_para->csc_en;
    hardware_node->dst_hipp_csc_ctrl.bits.dst_hipp_csc_demo_en = 0;
    hardware_node->dst_hipp_csc_ctrl.bits.dst_hipp_csc_ck_gt_en = 1;
}

/*****************************************************************************
 * Function:      tde_hal_node_set_color_convert
 * Description:  set parameter for color space change
 * Input:         hardware_node:pointer of node
 *                conv: parameter of color space change
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_s32 tde_hal_node_set_color_convert(tde_hardware_node *hardware_node, tde_conv_mode_cmd *conv)
{
    hi_u32 capability = 0;
    tde_hal_csc_para csc_para = {0};

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(hardware_node, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(conv, HI_FAILURE);

    tde_get_capability(&capability);
    if (!(capability & CSCCOVERT)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CSCCOVERT);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, capability);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It deos not support CSCCovert");
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    if (conv->in_conv) {
        csc_para.csc_en = HI_TRUE;
        csc_para.csc_mode_src2 = (conv->in_src1_conv == 1) ? 0 : 1;
        if (conv->in_rgb2yuv) {
            tde_hal_node_csc_inconv_rgb2yuv(&csc_para);
        } else {
            tde_hal_node_csc_inconv_yuv2rgb(&csc_para);
        }
        tde_hal_set_src_csc(hardware_node, &csc_para);
    }
    csc_para.csc_en = HI_FALSE;

    if (conv->out_conv) {
        csc_para.csc_en = HI_TRUE;
        if (conv->in_rgb2yuv) {
            tde_hal_node_csc_outconv_rgb2yuv(&csc_para);
        } else {
            tde_hal_node_csc_outconv_yuv2rgb(&csc_para);
        }
        tde_hal_set_dst_csc(hardware_node, &csc_para);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/****************************************************************************/
/* TDE hal function inner */
/****************************************************************************/
/***************************************************************************************
* func          : TdeHalInitParaTable
* description   : init config parameter
                  CNcomment: CNend\n
* param[in]     :
* retval        : HI_SUCCESS 成功
* retval        : HI_FAILURE 失败
* others:       : NA
***************************************************************************************/
STATIC hi_s32 TdeHalInitParaTable(hi_void)
{
#define OrgHfCoef g_Tde6x32Coef
    hi_u32 i = 0;
    hi_u32 *pHfCoef = HI_NULL;
    hi_u32 *pVfCoef = HI_NULL;
#ifndef HI_BUILD_IN_BOOT
    hi_u32 *pDeflickerVfCoef = HI_NULL;
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    pHfCoef = (hi_u32 *)tde_malloc(TDE_PARA_HTABLE_SIZE * TDE_PARA_HTABLE_NUM);
    if (pHfCoef == HI_NULL) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_malloc, FAILURE_TAG);
        return HI_FAILURE;
    }

    pVfCoef = (hi_u32 *)tde_malloc(TDE_PARA_VTABLE_SIZE * TDE_PARA_VTABLE_NUM);
    if (pVfCoef == HI_NULL) {
        tde_free(pHfCoef);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_malloc, FAILURE_TAG);
        return HI_FAILURE;
    }

#ifndef HI_BUILD_IN_BOOT
    pDeflickerVfCoef = (hi_u32 *)tde_malloc(TDE_PARA_VTABLE_SIZE * TDE_PARA_VTABLE_NUM);
    if (pDeflickerVfCoef == HI_NULL) {
        tde_free(pHfCoef);
        tde_free(pVfCoef);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_malloc, FAILURE_TAG);
        return HI_FAILURE;
    }
#endif

    /* copy parameter according other offer way */
    /* CNcomment:按照算法组提供的结构拷贝参数表 */
    for (i = 0; i < TDE_PARA_HTABLE_NUM; i++) {
        memcpy(pHfCoef + i * (TDE_PARA_HTABLE_SIZE / 4), OrgHfCoef + i * (TDE_PARA_HTABLE_ORG_SIZE),
               (TDE_PARA_HTABLE_ORG_SIZE)*4);
    }

    for (i = 0; i < TDE_PARA_VTABLE_NUM; i++) {
        memcpy(pVfCoef + i * (TDE_PARA_VTABLE_SIZE / 4), g_org_ver_coef + i * (TDE_PARA_VTABLE_ORG_SIZE),
               (TDE_PARA_VTABLE_ORG_SIZE)*4);
    }

#ifndef HI_BUILD_IN_BOOT
    for (i = 0; i < TDE_PARA_VTABLE_NUM; i++) {
        memcpy(pDeflickerVfCoef + i * (TDE_PARA_VTABLE_SIZE / 4),
               g_org_ver_coef + /* 4 byte */
                   i * (TDE_PARA_VTABLE_ORG_SIZE),
               (TDE_PARA_VTABLE_ORG_SIZE)*4); /* 4 byte */
    }
#endif

    memset(&g_para_table, 0, sizeof(g_para_table));

#ifndef HI_BUILD_IN_BOOT
    g_para_table.u32DeflickerVfCoefAddr = wgetphy((hi_void *)pDeflickerVfCoef);
    if (g_para_table.u32DeflickerVfCoefAddr == 0) {
        tde_free(pHfCoef);
        tde_free(pVfCoef);
        tde_free(pDeflickerVfCoef);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, wgetphy, FAILURE_TAG);
        return HI_FAILURE;
    }
#endif

    g_para_table.u32HfCoefAddr = wgetphy((hi_void *)pHfCoef);
    if (g_para_table.u32HfCoefAddr == 0) {
        ;
        tde_free(pHfCoef);
        tde_free(pVfCoef);
#ifndef HI_BUILD_IN_BOOT
        tde_free(pDeflickerVfCoef);
#endif
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, wgetphy, FAILURE_TAG);
        return HI_FAILURE;
    }

    g_para_table.u32VfCoefAddr = wgetphy((hi_void *)pVfCoef);
    if (g_para_table.u32VfCoefAddr == 0) {
        tde_free(pHfCoef);
        tde_free(pVfCoef);
#ifndef HI_BUILD_IN_BOOT
        tde_free(pDeflickerVfCoef);
#endif
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, wgetphy, FAILURE_TAG);
        return HI_FAILURE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

#ifndef HI_BUILD_IN_BOOT
/*****************************************************************************
 * Function:      tde_hal_cur_node
 * Description:   get the node physics address that is suspended
 * Input:         none
 * Output:        none
 * Return:       the address of current running node
 * Others:        none
 *****************************************************************************/
hi_u32 tde_hal_cur_node()
{
    hi_u32 u32Addr;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    u32Addr = TDE_READ_REG(g_base_vir_addr, TDE_AQ_ADDR);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return u32Addr;
}
#endif

STATIC hi_s32 TdeHalGetbppByFmt16(tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case TDE_DRV_COLOR_FMT_RGB444:
        case TDE_DRV_COLOR_FMT_RGB555:
        case TDE_DRV_COLOR_FMT_RGB565:
        case TDE_DRV_COLOR_FMT_ARGB4444:
        case TDE_DRV_COLOR_FMT_ARGB1555:
        case TDE_DRV_COLOR_FMT_ACLUT88:
        case TDE_DRV_COLOR_FMT_YCbCr422:
        case TDE_DRV_COLOR_FMT_halfword:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 16;
        default:
            return -1;
    }
}

STATIC hi_s32 TdeHalGetbppByFmt24(tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case TDE_DRV_COLOR_FMT_RGB888:
        case TDE_DRV_COLOR_FMT_ARGB8565:
        case TDE_DRV_COLOR_FMT_YCbCr888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 24;
        default:
            return -1;
    }
}

STATIC hi_s32 TdeHalGetbppByFmt32(tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case TDE_DRV_COLOR_FMT_ARGB8888:
        case TDE_DRV_COLOR_FMT_AYCbCr8888:
        case TDE_DRV_COLOR_FMT_RABG8888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 32;
        default:
            return -1;
    }
}

STATIC hi_s32 TdeHalGetbppByFmt1(tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case TDE_DRV_COLOR_FMT_CLUT1:
        case TDE_DRV_COLOR_FMT_A1:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 1;
        default:
            return -1;
    }
}

STATIC hi_s32 TdeHalGetbppByFmt2(tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case TDE_DRV_COLOR_FMT_CLUT2:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 2;
        default:
            return -1;
    }
}

STATIC hi_s32 TdeHalGetbppByFmt4(tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case TDE_DRV_COLOR_FMT_CLUT4:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 4;
        default:
            return -1;
    }
}

STATIC hi_s32 TdeHalGetbppByFmt8(tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case TDE_DRV_COLOR_FMT_CLUT8:
        case TDE_DRV_COLOR_FMT_ACLUT44:
        case TDE_DRV_COLOR_FMT_A8:
        case TDE_DRV_COLOR_FMT_byte:
        case TDE_DRV_COLOR_FMT_YCbCr420MB:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 8;
        default:
            return -1;
    }
}

/*****************************************************************************
 * Function:      TdeHalGetbppByFmt
 * Description:   get bpp according color of driver
 * Input:         color_fmt: color type
 * Output:        pitch width
 * Return:        -1: wrong format
 * Others:        none
 *****************************************************************************/
STATIC hi_s32 TdeHalGetbppByFmt(tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (TdeHalGetbppByFmt16(color_fmt) == 16) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 16;
    } else if (TdeHalGetbppByFmt24(color_fmt) == 24) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 24;
    } else if (TdeHalGetbppByFmt32(color_fmt) == 32) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 32;
    } else if (TdeHalGetbppByFmt1(color_fmt) == 1) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 1;
    } else if (TdeHalGetbppByFmt2(color_fmt) == 2) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 2;
    } else if (TdeHalGetbppByFmt4(color_fmt) == 4) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 4;
    } else if (TdeHalGetbppByFmt8(color_fmt) == 8) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 8;
    } else {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return -1;
    }
}

/*****************************************************************************
 * Function:      TdeHalGetResizeParaTable
 * Description:   get index table according the step
 * Input:         u32Step: input step
 * Output:        none
 * Return:        index table address
 * Others:        none
 *****************************************************************************/
STATIC INLINE hi_u32 TdeHalGetResizeParaHTable(hi_u32 u32Step)
{
    hi_u32 u32Index = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (u32Step < TDE_RESIZE_8X32_AREA_0) {
        u32Index = 0;
    } else if (u32Step == TDE_RESIZE_8X32_AREA_0) {
        u32Index = 1;
    } else if (u32Step > TDE_RESIZE_8X32_AREA_0 && u32Step <= TDE_RESIZE_8X32_AREA_1) {
        u32Index = 2;
    } else if (u32Step > TDE_RESIZE_8X32_AREA_1 && u32Step <= TDE_RESIZE_8X32_AREA_2) {
        u32Index = 3;
    } else if (u32Step > TDE_RESIZE_8X32_AREA_2 && u32Step <= TDE_RESIZE_8X32_AREA_3) {
        u32Index = 4;
    } else if (u32Step > TDE_RESIZE_8X32_AREA_3 && u32Step <= TDE_RESIZE_8X32_AREA_4) {
        u32Index = 5;
    } else {
        u32Index = 6;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return u32Index;
}

/*****************************************************************************
 * Function:      TdeHalGetResizeParaVTable
 * Description:   get table of parameter for resize
 * Input:         u32Step:input step
 * Output:        none
 * Return:        address of table
 * Others:        none
 *****************************************************************************/
STATIC INLINE hi_u32 TdeHalGetResizeParaVTable(hi_u32 u32Step)
{
    hi_u32 u32Index = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* get index table according step */
    /* CNcomment: 根据步长找到表的索引 */
    if (u32Step < TDE_RESIZE_PARA_AREA_0) {
        u32Index = 0;
    } else if (u32Step == TDE_RESIZE_PARA_AREA_0) {
        u32Index = 1;
    } else if (u32Step > TDE_RESIZE_PARA_AREA_0 && u32Step <= TDE_RESIZE_PARA_AREA_1) {
        u32Index = 2;
    } else if (u32Step > TDE_RESIZE_PARA_AREA_1 && u32Step <= TDE_RESIZE_PARA_AREA_2) {
        u32Index = 3;
    } else if (u32Step > TDE_RESIZE_PARA_AREA_2 && u32Step <= TDE_RESIZE_PARA_AREA_3) {
        u32Index = 4;
    } else if (u32Step > TDE_RESIZE_PARA_AREA_3 && u32Step <= TDE_RESIZE_PARA_AREA_4) {
        u32Index = 5;
    } else {
        u32Index = 6;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return u32Index;
}

/*****************************************************************************
 * Function:      TdeHalInitQueue
 * Description:   Initialize Aq list,config the operation which is needed
 * Input:         none
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
STATIC INLINE hi_void TdeHalInitQueue(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    TDE_WRITE_REG(g_base_vir_addr, TDE_PNEXT_LOW, 0);
    TDE_WRITE_REG(g_base_vir_addr, TDE_PNEXT_HI, 0);
#ifdef CONFIG_GFX_TDE_LOW_POWER_SUPPORT
    TDE_WRITE_REG(g_base_vir_addr, TDE_MISCELLANEOUS, 0x0f006466);
#else
    TDE_WRITE_REG(g_base_vir_addr, TDE_MISCELLANEOUS, 0x30064);
#endif
    TDE_WRITE_REG(g_base_vir_addr, TDE_MST_OUTSTANDING, 0x4f);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
}

#ifndef HI_BUILD_IN_BOOT
/*****************************************************************************
 * Function:      TdeHalGetOffsetInNode
 * Description:   get offset of operation according u32MaskUpdt
 * Input:         none
 * Output:        none
 * Return:        -1:offse
 * Others:        none
 *****************************************************************************/
STATIC INLINE hi_s32 TdeHalGetOffsetInNode(hi_u64 u64MaskUpdt, hi_u64 up_data_flag)
{
    hi_u32 i = 0;
    hi_s32 s32Ret = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((u64MaskUpdt & up_data_flag) == 0) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "0 == (u64MaskUpdt & up_data_flag)");
        return -1;
    }

    for (i = 0; i < 64; i++) {
        if (((u64MaskUpdt >> i) & 0x1) == 1) {
            break;
        }
        if (((up_data_flag >> i) & 0x1) == 1) {
            s32Ret++;
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return s32Ret;
}

/*****************************************************************************
 * Function:      tde_hal_set_deflicer_level
 * Description:   SetDeflicerLevel
 * Input:         deflicker_level:anti-flicker levels including:auto,low,middle,high
 * Output:        none
 * Return:        success
 * Others:        none
 *****************************************************************************/
hi_s32 tde_hal_set_deflicer_level(hi_tde_deflicker_level deflicker_level)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    g_deflicker_level = deflicker_level;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_s32 tde_hal_get_deflicer_level(hi_tde_deflicker_level *deflicker_level)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(deflicker_level, HI_FAILURE);
    *deflicker_level = g_deflicker_level;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

hi_s32 tde_hal_set_alpha_threshold(hi_u8 threshold_value)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    g_alpha_threshold_value = threshold_value;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

#ifndef HI_BUILD_IN_BOOT
hi_s32 tde_hal_get_alpha_threshold(hi_u8 *threshold_value)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(threshold_value, HI_FAILURE);
    *threshold_value = g_alpha_threshold_value;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

hi_s32 tde_hal_set_alpha_threshold_state(hi_bool alpha_threshold_en)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    g_alpha_threshold_en = alpha_threshold_en;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

#ifdef HI_BUILD_IN_BOOT
extern hi_s32 tde_list_init(hi_void);
hi_s32 drv_tde_init(hi_void)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (tde_hal_init(TDE_REG_BASEADDR) < 0) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_FAILURE;
    }
    tde_list_init();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
#endif

#ifndef HI_BUILD_IN_BOOT
hi_s32 tde_hal_get_alpha_threshold_state(hi_bool *alpha_threshold_en)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(alpha_threshold_en, HI_FAILURE);
    *alpha_threshold_en = g_alpha_threshold_en;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

#ifdef CONFIG_GFX_PROC_SUPPORT

#define TDE_PROC_PRINT_NODE_MEMBER(member) osal_proc_print(p, "(%-20s)\t:0x%08x\n", #member, node->member.u32)
static hi_void tde_hal_node_print_info_src1(hi_void *p, tde_hardware_node *node)
{
    if (p != NULL && node != NULL) {
        TDE_PROC_PRINT_NODE_MEMBER(src1_ctrl);
        TDE_PROC_PRINT_NODE_MEMBER(src1_ch0_addr_l);
        TDE_PROC_PRINT_NODE_MEMBER(src1_ch1_addr_l);
        TDE_PROC_PRINT_NODE_MEMBER(src1_ch0_stride);
        TDE_PROC_PRINT_NODE_MEMBER(src1_ch1_stride);
        TDE_PROC_PRINT_NODE_MEMBER(src1_image_size);
        TDE_PROC_PRINT_NODE_MEMBER(src1_fill);
        TDE_PROC_PRINT_NODE_MEMBER(src1_alpha);
        TDE_PROC_PRINT_NODE_MEMBER(src1_pix_offset);
    }
}

static hi_void tde_hal_node_print_info_src2(hi_void *p, tde_hardware_node *node)
{
    if (p != NULL && node != NULL) {
        TDE_PROC_PRINT_NODE_MEMBER(src2_ctrl);
        TDE_PROC_PRINT_NODE_MEMBER(src2_ch0_addr_l);
        TDE_PROC_PRINT_NODE_MEMBER(src2_ch1_addr_l);
        TDE_PROC_PRINT_NODE_MEMBER(src2_ch0_stride);
        TDE_PROC_PRINT_NODE_MEMBER(src2_ch1_stride);
        TDE_PROC_PRINT_NODE_MEMBER(src2_image_size);
        TDE_PROC_PRINT_NODE_MEMBER(src2_fill);
        TDE_PROC_PRINT_NODE_MEMBER(src2_alpha);
        TDE_PROC_PRINT_NODE_MEMBER(src2_pix_offset);
    }
}

static hi_void tde_hal_node_print_info_dst(hi_void *p, tde_hardware_node *node)
{
    if (p != NULL && node != NULL) {
        TDE_PROC_PRINT_NODE_MEMBER(dst_ctrl);
        TDE_PROC_PRINT_NODE_MEMBER(dst_ch0_addr_l);
        TDE_PROC_PRINT_NODE_MEMBER(dst_ch1_addr_l);
        TDE_PROC_PRINT_NODE_MEMBER(dst_ch0_stride);
        TDE_PROC_PRINT_NODE_MEMBER(dst_ch1_stride);
        TDE_PROC_PRINT_NODE_MEMBER(dst_image_size);
        TDE_PROC_PRINT_NODE_MEMBER(dst_alpha);
        TDE_PROC_PRINT_NODE_MEMBER(dst_crop_pos_start);
        TDE_PROC_PRINT_NODE_MEMBER(dst_crop_pos_end);
        TDE_PROC_PRINT_NODE_MEMBER(mmu_prot_ctrl);
        TDE_PROC_PRINT_NODE_MEMBER(dst_pix_offset);
        TDE_PROC_PRINT_NODE_MEMBER(dst_cmd_ctrl);
    }
}

static hi_void tde_hal_node_print_info_src1_hsp(hi_void *p, tde_hardware_node *node)
{
    if (p != NULL && node != NULL) {
        TDE_PROC_PRINT_NODE_MEMBER(src1_hsp);
        TDE_PROC_PRINT_NODE_MEMBER(src1_hloffset);
        TDE_PROC_PRINT_NODE_MEMBER(src1_hcoffset);
        TDE_PROC_PRINT_NODE_MEMBER(src1_vsp);
        TDE_PROC_PRINT_NODE_MEMBER(src1_vsr);
        TDE_PROC_PRINT_NODE_MEMBER(src1_voffset);
        TDE_PROC_PRINT_NODE_MEMBER(src1_zme_out_reso);
        TDE_PROC_PRINT_NODE_MEMBER(src1_zme_in_reso);

        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_ctrl);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_coef00);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_coef01);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_coef02);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_coef10);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_coef11);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_coef12);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_coef20);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_coef21);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_coef22);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_scale);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_idc0);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_idc1);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_idc2);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_odc0);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_odc1);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_odc2);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_min_y);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_min_c);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_max_y);
        TDE_PROC_PRINT_NODE_MEMBER(src_hipp_csc_max_c);
    }
}

static hi_void tde_hal_node_print_info_src2_hsp(hi_void *p, tde_hardware_node *node)
{
    if (p != NULL && node != NULL) {
        TDE_PROC_PRINT_NODE_MEMBER(src2_hsp);
        TDE_PROC_PRINT_NODE_MEMBER(src2_hloffset);
        TDE_PROC_PRINT_NODE_MEMBER(src2_hcoffset);
        TDE_PROC_PRINT_NODE_MEMBER(src2_vsp);
        TDE_PROC_PRINT_NODE_MEMBER(src2_vsr);
        TDE_PROC_PRINT_NODE_MEMBER(src2_voffset);
        TDE_PROC_PRINT_NODE_MEMBER(src2_zme_out_reso);
        TDE_PROC_PRINT_NODE_MEMBER(src2_zme_in_reso);
        TDE_PROC_PRINT_NODE_MEMBER(src2_hpzme);
    }
}

static hi_void tde_hal_node_print_info_dst_csc(hi_void *p, tde_hardware_node *node)
{
    if (p != NULL && node != NULL) {
        TDE_PROC_PRINT_NODE_MEMBER(dst_h_dswm);
        TDE_PROC_PRINT_NODE_MEMBER(dst_h_dwsm_hloffset);
        TDE_PROC_PRINT_NODE_MEMBER(dst_h_dwsm_hcoffset);
        TDE_PROC_PRINT_NODE_MEMBER(dst_v_dswm);
        TDE_PROC_PRINT_NODE_MEMBER(tde_csc_ctrl);

        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_ctrl);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_coef00);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_coef01);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_coef02);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_coef10);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_coef11);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_coef12);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_coef20);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_coef21);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_coef22);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_scale);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_idc0);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_idc1);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_idc2);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_odc0);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_odc1);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_odc2);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_min_y);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_min_c);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_max_y);
        TDE_PROC_PRINT_NODE_MEMBER(dst_hipp_csc_max_c);
    }
}

static hi_void tde_hal_node_print_info_cbm(hi_void *p, tde_hardware_node *node)
{
    if (p != NULL && node != NULL) {
        TDE_PROC_PRINT_NODE_MEMBER(cbm_ctrl);
        TDE_PROC_PRINT_NODE_MEMBER(cbmbkgrgb);
        TDE_PROC_PRINT_NODE_MEMBER(cbmcolorizealu1);
        TDE_PROC_PRINT_NODE_MEMBER(cbmparaalu1);
        TDE_PROC_PRINT_NODE_MEMBER(cbmkeyparaalu1);
        TDE_PROC_PRINT_NODE_MEMBER(cbmkeyminalu1);
        TDE_PROC_PRINT_NODE_MEMBER(cbmkeymaxalu1);
        TDE_PROC_PRINT_NODE_MEMBER(cbmkeymaskalu1);
        TDE_PROC_PRINT_NODE_MEMBER(src1_cbm_para);
        TDE_PROC_PRINT_NODE_MEMBER(src1_cbmstpos);
        TDE_PROC_PRINT_NODE_MEMBER(src2_cbm_para);
        TDE_PROC_PRINT_NODE_MEMBER(src2_cbmstpos);
    }
}

static hi_void tde_hal_node_print_info_zme(hi_void *p, tde_hardware_node *node)
{
    if (p != NULL && node != NULL) {
        TDE_PROC_PRINT_NODE_MEMBER(tde_src1_zme_haddr_high);
        TDE_PROC_PRINT_NODE_MEMBER(tde_src1_zme_haddr_low);
        TDE_PROC_PRINT_NODE_MEMBER(tde_src1_zme_vaddr_high);
        TDE_PROC_PRINT_NODE_MEMBER(tde_src1_zme_vaddr_low);
        TDE_PROC_PRINT_NODE_MEMBER(tde_src2_zme_haddr_high);
        TDE_PROC_PRINT_NODE_MEMBER(tde_src2_zme_haddr_low);
        TDE_PROC_PRINT_NODE_MEMBER(tde_src2_zme_vaddr_high);
        TDE_PROC_PRINT_NODE_MEMBER(tde_src2_zme_vaddr_low);
        TDE_PROC_PRINT_NODE_MEMBER(tde_src3_zme_haddr_high);
        TDE_PROC_PRINT_NODE_MEMBER(tde_src3_zme_haddr_low);
        TDE_PROC_PRINT_NODE_MEMBER(tde_src3_zme_vaddr_high);
        TDE_PROC_PRINT_NODE_MEMBER(tde_src3_zme_vaddr_low);
    }
}

struct seq_file *tde_hal_node_print_info(hi_void *p, tde_hardware_node *node)
{
    if ((p != NULL) && (node != NULL)) {
        osal_proc_print(p, "\n------------- hisilicon TDE node params info -------------\n");

        osal_proc_print(p, "--->src1_ctrl INFO:\n");
        tde_hal_node_print_info_src1(p, node);
        osal_proc_print(p, "--->src2_ctrl INFO:\n");
        tde_hal_node_print_info_src2(p, node);
        osal_proc_print(p, "--->dst_ctrl INFO:\n");
        tde_hal_node_print_info_dst(p, node);
        osal_proc_print(p, "--->src1_hsp INFO:\n");
        tde_hal_node_print_info_src1_hsp(p, node);
        osal_proc_print(p, "--->src2_hsp INFO:\n");
        tde_hal_node_print_info_src2_hsp(p, node);
        osal_proc_print(p, "--->DES_CSC_IDC INFO:\n");
        tde_hal_node_print_info_dst_csc(p, node);
        osal_proc_print(p, "--->cbm_ctrl INFO:\n");
        tde_hal_node_print_info_cbm(p, node);
        osal_proc_print(p, "--->src1_zme_lh_addr INFO:\n");
        tde_hal_node_print_info_zme(p, node);
    }
    return p;
}

#endif

#endif
