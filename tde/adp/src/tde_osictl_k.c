/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: osictl manage
 * Author: sdk
 * Create: 2019-03-18
 */
#ifndef HI_BUILD_IN_BOOT
#include "hi_osal.h"
#include "tde_proc.h"
#include "hi_errno.h"
#else
#include "exports.h"
#include "hi_gfx_mem.h"
#include "hi_errno.h"
#endif
#include "tde_define.h"
#include "tde_handle.h"
#include "tde_osictl.h"
#include "tde_osilist.h"
#include "tde_hal.h"
#include "wmalloc.h"
#include "tde_adp.h"

#ifdef HI_BUILD_IN_BOOT
#include "hi_gfx_comm.h"
#include "hi_gfx_debug.h"
#else
#include "tde_debug.h"
#endif
/***********************************************************************************************/

/* **************************** Macro Definition ******************************************* */

/****************************************************************************/
/* TDE osi ctl macro definition */
/****************************************************************************/

#define TDE_MIN(a, b) (((a) > (b)) ? (b) : (a))

#define TDE_CALC_MAX_SLICE_NUM(out_w) ((out_w) / 256 + ((out_w % 256) != 0))

#define TDE_FILLUP_RECT_BY_DRVSURFACE(rect, drvSur) do { \
    (rect).pos_x = (hi_s32)(drvSur).pos_x;      \
    (rect).pos_y = (hi_s32)(drvSur).pos_y;      \
    (rect).width = (drvSur).width;              \
    (rect).height = (drvSur).height;            \
} while (0)

#define TDE_CHANGE_DIR_BY_MIRROR(pDirect, mirror) do { \
    switch (mirror) {                                     \
        case HI_TDE_MIRROR_HORIZONTAL:                    \
            (pDirect)->hor_scan = !((pDirect)->hor_scan); \
            break;                                        \
        case HI_TDE_MIRROR_VERTICAL:                      \
            (pDirect)->ver_scan = !((pDirect)->ver_scan); \
            break;                                        \
        case HI_TDE_MIRROR_BOTH:                          \
            (pDirect)->hor_scan = !((pDirect)->hor_scan); \
            (pDirect)->ver_scan = !((pDirect)->ver_scan); \
            break;                                        \
        default:                                          \
            break;                                        \
    }                                                     \
} while (0)

#define TDE_SET_CLIP(phwNode, opt) do { \
    if (HI_TDE_CLIP_MODE_NONE != (opt)->clip_mode) {                                                   \
        tde_clip_cmd stClipCmd;                                                                        \
        stClipCmd.clip_start_x = (hi_u16)(opt)->clip_rect.pos_x;                                       \
        stClipCmd.clip_start_y = (hi_u16)(opt)->clip_rect.pos_y;                                       \
        stClipCmd.clip_end_x = ((opt)->clip_rect.pos_x + (opt)->clip_rect.width - 1);                  \
        stClipCmd.clip_end_y = (opt)->clip_rect.pos_y + ((opt)->clip_rect.height - 1);                 \
        stClipCmd.is_inside_clip = (HI_TDE_CLIP_MODE_INSIDE == (opt)->clip_mode) ? HI_TRUE : HI_FALSE; \
        if (tde_hal_node_set_clipping(phwNode, &stClipCmd) < 0) {                                      \
            return HI_ERR_TDE_UNSUPPORTED_OPERATION;                                                   \
        }                                                                                              \
    }                                                                                                  \
} while (0)

/*
start % 4 = 0: (h-1)/4 + 1
start % 4 = 1: (h/4) + 1
start % 4 = 2: (h+1)/4 + 1
start % 4 = 3: (h+2)/4 + 1 */
#define TDE_ADJ_SIZE_BY_START_I(start, size) (((size) + (((start) % 4) - 1)) / 4 + 1)

/*
when bottom filed, adjust height, only according by 4 integal-multiple of start
address.so, start reserve is not used
*/
#define TDE_ADJ_B_SIZE_BY_START_I(start, size) ((size >> 2) + ((size % 4) >> 1))
/*
start even number:  (w-1)/2 +1
start odd number: w/2 + 1 */
#define TDE_ADJ_SIZE_BY_START_P(start, size) (((size) + (((start) % 2) - 1)) / 2 + 1)

#define TDE_ADJ_FIELD_HEIGHT_BY_START(y, h) ((h) / 2 + (((y)&1) & ((h)&1)))

/*
    return CbCr422R fill value
      ------------
      |Y'|Cb|Y|Cr|
      ------------
       ^_____|
      copy Y to Y'
*/
#define TDE_GET_YC422R_FILLVALUE(value) ((value)&0xffffff) | ((((value) >> 8) & 0xff) << 24)

#define TDE_UNIFY_RECT(psrcRect, pdstRect) do { \
    if ((psrcRect)->height != (pdstRect)->height) {                           \
        (psrcRect)->height = TDE_MIN((psrcRect)->height, (pdstRect)->height); \
        (pdstRect)->height = (psrcRect)->height;                              \
    }                                                                         \
    if ((psrcRect)->width != (pdstRect)->width) {                             \
        (psrcRect)->width = TDE_MIN((psrcRect)->width, (pdstRect)->width);    \
        (pdstRect)->width = (psrcRect)->width;                                \
    }                                                                         \
} while (0)

#define TDE_CHECK_COLORFMT(color_fmt) do { \
    if (HI_TDE_COLOR_FMT_MAX <= color_fmt) {                                    \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, color_fmt);                  \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_COLOR_FMT_MAX);  \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "color format error"); \
        return HI_ERR_TDE_INVALID_PARA;                                         \
    }                                                                           \
} while (0)

#define TDE_CHECK_MBCOLORFMT(enMbColorFmt) do { \
    if (HI_TDE_MB_COLOR_FMT_MAX <= enMbColorFmt) {                                 \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enMbColorFmt);                  \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_MB_COLOR_FMT_MAX);  \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "mb color format error"); \
        return HI_ERR_TDE_INVALID_PARA;                                            \
    }                                                                              \
} while (0)

#define TDE_CHECK_OUTALPHAFROM(out_alpha_from) do { \
    if (HI_TDE_OUT_ALPHA_FROM_MAX <= out_alpha_from) {                              \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, out_alpha_from);                 \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_OUT_ALPHA_FROM_MAX); \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "out_alpha_from error");   \
        return HI_ERR_TDE_INVALID_PARA;                                             \
    }                                                                               \
} while (0)

#define TDE_CHECK_ROPCODE(enRopCode) do { \
    if (HI_TDE_ROP_MAX <= enRopCode) {                                       \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enRopCode);               \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_ROP_MAX);     \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "enRopCode error"); \
        return HI_ERR_TDE_INVALID_PARA;                                      \
    }                                                                        \
} while (0)

#define TDE_CHECK_ALUCMD(alpha_blending_cmd) do { \
    if (HI_TDE_ALPHA_BLENDING_MAX <= alpha_blending_cmd) {                            \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, alpha_blending_cmd);               \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_ALPHA_BLENDING_MAX);   \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "alpha_blending_cmd error"); \
        return HI_ERR_TDE_INVALID_PARA;                                               \
    }                                                                                 \
} while (0)

#define TDE_CHECK_MIRROR(mirror) do { \
    if (HI_TDE_MIRROR_MAX <= mirror) {                                    \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, mirror);               \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "mirror error"); \
        return HI_ERR_TDE_INVALID_PARA;                                   \
    }                                                                     \
} while (0)

#define TDE_CHECK_CLIPMODE(clip_mode) do { \
    if (HI_TDE_CLIP_MODE_MAX <= clip_mode) {                                 \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, clip_mode);               \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "clip_mode error"); \
        return HI_ERR_TDE_INVALID_PARA;                                      \
    }                                                                        \
} while (0)

#define TDE_CHECK_MBRESIZE(enMbResize) do { \
    if (HI_TDE_MB_RESIZE_MAX <= enMbResize) {                                 \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enMbResize);               \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "enMbResize error"); \
        return HI_ERR_TDE_INVALID_PARA;                                       \
    }                                                                         \
} while (0)

#define TDE_CHECK_MBPICMODE(enMbPicMode) do { \
    if (TDE_PIC_MODE_BUTT <= enMbPicMode) {                                    \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enMbPicMode);               \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "enMbPicMode error"); \
        return HI_ERR_TDE_INVALID_PARA;                                        \
    }                                                                          \
} while (0)

#define TDE_CHECK_COLORKEYMODE(color_key_mode) do { \
    if (HI_TDE_COLOR_KEY_MODE_MAX <= color_key_mode) {                              \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, color_key_mode);                 \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_COLOR_KEY_MODE_MAX); \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "enMbPicMode error");      \
        return HI_ERR_TDE_INVALID_PARA;                                             \
    }                                                                               \
} while (0)

#define TDE_CHECK_FILTERMODE(filter_mode) do { \
    if (HI_TDE_FILTER_MODE_MAX <= filter_mode) {                               \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, filter_mode);               \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "filter_mode error"); \
        return HI_ERR_TDE_INVALID_PARA;                                        \
    }                                                                          \
} while (0)

#define TDE_CHECK_ROTATE(enRotate) do { \
    if (TDE_ROTATE_BUTT <= enRotate) {                                      \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enRotate);               \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "enRotate error"); \
        return HI_ERR_TDE_INVALID_PARA;                                     \
    }                                                                       \
} while (0)

#define TDE_CHECK_SUBBYTE_STARTX(startx, w, outFmt) do { \
    hi_s32 s32Bpp = -1;                                                                  \
    s32Bpp = TdeOsiGetbppByFmt(outFmt);                                                  \
    if (0 > s32Bpp) {                                                                    \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s32Bpp);                              \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Unkown color format");         \
        return HI_ERR_TDE_INVALID_PARA;                                                  \
    }                                                                                    \
                                                                                             \
        if (s32Bpp < 8) {                                                                    \
        /* when writing, subbyte format align ask start point byte align */              \
        if (((startx)*s32Bpp % 8) || ((w)*s32Bpp % 8)) {                                 \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s32Bpp);                     \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, startx);                     \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, w);                          \
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION,                             \
                                 "Subbyte's start or width for writing is not aligned"); \
            return HI_ERR_TDE_INVALID_PARA;                                              \
        }                                                                                \
    }                                                                                    \
} while (0)

#define TDE_CHECK_BLENDCMD(enBlendCmd) do { \
    if (enBlendCmd >= HI_TDE_BLEND_CMD_MAX) {                                  \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enBlendCmd);                \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Unknown blend cmd"); \
        return HI_ERR_TDE_INVALID_PARA;                                        \
    }                                                                          \
} while (0)

#define TDE_CHECK_BLENDMODE(blend_mode) do { \
    if (blend_mode >= HI_TDE_BLEND_MAX) {                                       \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, blend_mode);                 \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Unknown blend mode"); \
        return HI_ERR_TDE_INVALID_PARA;                                         \
    }                                                                           \
} while (0)

#define TDE_CHECK_NOT_MB(color_fmt) do { \
    if (color_fmt >= HI_TDE_COLOR_FMT_byte) {                                                            \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, color_fmt);                                           \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "This operation doesn't support Semi-plannar"); \
        return HI_ERR_TDE_INVALID_PARA;                                                                  \
    }                                                                                                    \
} while (0)

#define TDE_CHECK_NOT_MB_EXCEPT_SP420(color_fmt) do { \
    if (color_fmt >= HI_TDE_COLOR_FMT_byte && color_fmt != HI_TDE_COLOR_FMT_JPG_YCbCr420MBP) {           \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, color_fmt);                                           \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "This operation doesn't support Semi-plannar"); \
        return HI_ERR_TDE_INVALID_PARA;                                                                  \
    }                                                                                                    \
} while (0)

#define TDE_CHECK_DST_FMT(color_fmt) do { \
    if ((color_fmt >= HI_TDE_COLOR_FMT_CLUT1) && (color_fmt <= HI_TDE_COLOR_FMT_A8)) {            \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, color_fmt);                                    \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Dest format doesn't support Clut&&A1"); \
        return HI_ERR_TDE_INVALID_PARA;                                                           \
    }                                                                                             \
} while (0)

#define TDE_CHECK_BACKGROUND_FMT(color_fmt) TDE_CHECK_DST_FMT(color_fmt)

#define TDE_STRIDE_CHECK_ALIGN(stride, u32AlignNum) do { \
    if (stride % u32AlignNum) {                                                    \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, stride);                  \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32AlignNum);             \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "stride is not aligned"); \
        return HI_ERR_TDE_NOT_ALIGNED;                                             \
    }                                                                              \
} while (0)

#define TDE_ADDRESS_CHECK_ALIGN(phy_addr, u32AlignNum) do { \
    if (phy_addr % u32AlignNum) {                                                          \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, phy_addr);                        \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32AlignNum);                     \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Bitmap address is not aligned"); \
        return HI_ERR_TDE_NOT_ALIGNED;                                                     \
    }                                                                                      \
} while (0)

#define TDE_PATTERNBACKGROUNDCATEGORY_CHECK_CLUT(enBackGroundCategory, enForeGroundCategory, enBackGroundFmt,          \
                                                 enForeGroundFmt, enDstFmt, alpha_blending_cmd, enDstCategory)         \
    do {                                                                                                               \
        if ((TDE_COLORFMT_CATEGORY_CLUT == enBackGroundCategory) &&                                                    \
            (HI_TDE_ALPHA_BLENDING_NONE != alpha_blending_cmd)) {                                                      \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enBackGroundCategory);                                     \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, alpha_blending_cmd);                                       \
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It doesn't support ROP");                                \
            return -1;                                                                                                 \
        }                                                                                                              \
        if ((TDE_COLORFMT_CATEGORY_CLUT == enBackGroundCategory) &&                                                    \
            ((enBackGroundFmt != enForeGroundFmt) || (enBackGroundFmt != enDstFmt))) {                                 \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enBackGroundCategory);                                     \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enBackGroundFmt);                                          \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enForeGroundFmt);                                          \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enDstFmt);                                                 \
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION,                                                           \
                                 "If background, foreground , dst are clut, they should be the same fmt");             \
            return -1;                                                                                                 \
        }                                                                                                              \
        if ((TDE_COLORFMT_CATEGORY_CLUT == enBackGroundCategory) &&                                                    \
            ((TDE_COLORFMT_CATEGORY_CLUT != enForeGroundCategory) || (TDE_COLORFMT_CATEGORY_CLUT != enDstCategory))) { \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enBackGroundCategory);                                     \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enForeGroundCategory);                                     \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enDstCategory);                                            \
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Unsupported operation");                                 \
            return -1;                                                                                                 \
        }                                                                                                              \
    } while (0)

#define TDE_BACKGROUNDCATEGORY_CHECK_CLUT(enBackGroundCategory, enForeGroundCategory, resize, deflicker_mode,          \
                                          alpha_blending_cmd, enDstCategory)                                           \
    do {                                                                                                               \
        if ((TDE_COLORFMT_CATEGORY_CLUT == enBackGroundCategory) &&                                                    \
            ((TDE_COLORFMT_CATEGORY_CLUT != enForeGroundCategory) || (TDE_COLORFMT_CATEGORY_CLUT != enDstCategory))) { \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enBackGroundCategory);                                     \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enForeGroundCategory);                                     \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enDstCategory);                                            \
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Unsupported operation");                                 \
            return -1;                                                                                                 \
        }                                                                                                              \
        if (((deflicker_mode != HI_TDE_DEFLICKER_MODE_NONE) || (resize) ||                                             \
             (HI_TDE_ALPHA_BLENDING_NONE != alpha_blending_cmd)) &&                                                    \
            (TDE_COLORFMT_CATEGORY_CLUT == enBackGroundCategory)) {                                                    \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, deflicker_mode);                                           \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, resize);                                                   \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, alpha_blending_cmd);                                       \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enBackGroundCategory);                                     \
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It doesn't support deflicker or ROP or mirror");         \
            return -1;                                                                                                 \
        }                                                                                                              \
    } while (0)

#define TDE_BACKGROUNDCATEGORY_CHECK_AN(enBackGroundCategory, enForeGroundCategory, enDstCategory, alpha_blending_cmd) do { \
    TDE_PATTERNBACKGROUNDCATEGORY_CHECK_AN(enBackGroundCategory, enForeGroundCategory, enDstCategory,              \
                                           alpha_blending_cmd);                                                    \
} while (0)

#define TDE_PATTERNBACKGROUNDCATEGORY_CHECK_AN(enBackGroundCategory, enForeGroundCategory, enDstCategory,        \
                                               alpha_blending_cmd)                                               \
    do {                                                                                                         \
        if ((TDE_COLORFMT_CATEGORY_An == enBackGroundCategory) &&                                                \
            (TDE_COLORFMT_CATEGORY_An == enForeGroundCategory) && (TDE_COLORFMT_CATEGORY_An == enDstCategory) && \
            (HI_TDE_ALPHA_BLENDING_NONE != alpha_blending_cmd)) {                                                \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enBackGroundCategory);                               \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enForeGroundCategory);                               \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enDstCategory);                                      \
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, alpha_blending_cmd);                                 \
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It doesn't support ROP or mirror");                \
            return -1;                                                                                           \
        }                                                                                                        \
    } while (0)

#define TDE_BACKGROUNDCATEGORY_CHECK_ARGB(enBackGroundCategory, enForeGroundCategory, enDstFmt) do { \
    if ((TDE_COLORFMT_CATEGORY_ARGB == enBackGroundCategory ||                                          \
         TDE_COLORFMT_CATEGORY_YCbCr == enBackGroundCategory) &&                                        \
        (TDE_COLORFMT_CATEGORY_An == enForeGroundCategory) && (!TdeOsiWhetherContainAlpha(enDstFmt))) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enBackGroundCategory);                          \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enForeGroundCategory);                          \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enDstFmt);                                      \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Target must have alpha component");           \
        return -1;                                                                                      \
    }                                                                                                   \
} while (0)

#define TDE_PATTERNBACKGROUNDCATEGORY_CHECK_ARGB(enBackGroundCategory, enForeGroundCategory, enDstFmt) do { \
    if ((TDE_COLORFMT_CATEGORY_ARGB == enBackGroundCategory) &&                                         \
        (TDE_COLORFMT_CATEGORY_An == enForeGroundCategory) && (!TdeOsiWhetherContainAlpha(enDstFmt))) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enBackGroundCategory);                          \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enForeGroundCategory);                          \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Target must have alpha component");           \
        return -1;                                                                                      \
    }                                                                                                   \
} while (0)

#define TDE_PATTERNFOREGROUNDCATEGORY_CHECK_AN(enForeGroundCategory, enBackGroundFmt, enForeGroundFmt) do { \
    if ((TDE_COLORFMT_CATEGORY_An == enForeGroundCategory) &&                                                   \
        ((HI_TDE_COLOR_FMT_YCbCr888 == enBackGroundFmt) || (HI_TDE_COLOR_FMT_AYCbCr8888 == enForeGroundFmt))) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enForeGroundCategory);                                  \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enBackGroundFmt);                                       \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enForeGroundFmt);                                       \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Target must have alpha component");                   \
        return -1;                                                                                              \
    }                                                                                                           \
} while (0)

#define TDE_PATTERNDSTCATEGORY_CHECK_CLUT(enBackGroundCategory, enForeGroundCategory, enDstCategory) do { \
    if ((enDstCategory == TDE_COLORFMT_CATEGORY_CLUT) && ((TDE_COLORFMT_CATEGORY_CLUT != enForeGroundCategory) ||  \
                                                          (TDE_COLORFMT_CATEGORY_CLUT != enBackGroundCategory))) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enDstCategory);                                            \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enForeGroundCategory);                                     \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enBackGroundCategory);                                     \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Unsupported operation");                                 \
        return -1;                                                                                                 \
    }                                                                                                              \
} while (0)

#define TDE_YCBCR422_FMT_CHECK_ODD(color_fmt, pos_x, width) do { \
    if (((pos_x & 0x1) || (width & 0x1)) && (HI_TDE_COLOR_FMT_YCbCr422 == color_fmt)) {            \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pos_x);                                    \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, color_fmt);                               \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, width);                                    \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "x, width of YCbCr422R couldn't be odd"); \
        return -1;                                                                                 \
    }                                                                                              \
} while (0)

#define TDE_MB_FMT_CHECK_ODD(color_fmt, width, height) do { \
    if (((height & 0x1) || (width & 0x1)) && ((HI_TDE_MB_COLOR_FMT_MP1_YCbCr420MBP == color_fmt) ||  \
                                              (HI_TDE_MB_COLOR_FMT_MP2_YCbCr420MBP == color_fmt) ||  \
                                              (HI_TDE_MB_COLOR_FMT_MP2_YCbCr420MBI == color_fmt) ||  \
                                              (HI_TDE_MB_COLOR_FMT_JPG_YCbCr420MBP == color_fmt))) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, height);                                    \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, width);                                     \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, color_fmt);                                  \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "width,height of SP420 couldn't be odd");   \
        return -1;                                                                                   \
    }                                                                                                \
    if ((width & 0x1) && (HI_TDE_MB_COLOR_FMT_JPG_YCbCr422MBHP == color_fmt)) {                      \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, width);                                     \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, color_fmt);                                  \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "width of SP422H couldn't be odd");         \
        return -1;                                                                                   \
    }                                                                                                \
    if ((height & 0x1) && (HI_TDE_MB_COLOR_FMT_JPG_YCbCr422MBVP == color_fmt)) {                     \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, height);                                    \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, color_fmt);                                  \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Height of SP422V couldn't be odd");        \
        return -1;                                                                                   \
    }                                                                                                \
} while (0)

#define TDE_YCBCR_FMT_CHECK_STRIDE(cbcr_stride, color_fmt) do { \
    if (((cbcr_stride > TDE_MAX_SURFACE_PITCH) || (cbcr_stride == 0)) &&         \
        (color_fmt != HI_TDE_COLOR_FMT_JPG_YCbCr400MBP)) {                       \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, cbcr_stride);           \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, color_fmt);              \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Invalid CbCr stride"); \
        return -1;                                                               \
    }                                                                            \
} while (0)

#define TDE_CHECK_SURFACEPARA(pstSurface, pstRect) do { \
    if ((TDE_MAX_SURFACE_PITCH < pstSurface->stride) || (0 == pstSurface->stride) || (0 == pstRect->height) ||   \
        (0 == pstRect->width) || (pstRect->pos_x < 0) || ((hi_u32)pstRect->pos_x >= pstSurface->width) ||        \
        (pstRect->pos_y < 0) || ((hi_u32)pstRect->pos_y >= pstSurface->height) || (0 == pstSurface->phy_addr)) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->width);                                     \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->height);                                    \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, pstSurface->stride);                                    \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, pstSurface->phy_addr);                                  \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstRect->pos_x);                                         \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstRect->pos_y);                                         \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRect->width);                                        \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRect->height);                                       \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION,                                                         \
                             "invalid surface phyaddr or invalid surface size or operation area");               \
        return -1;                                                                                               \
    }                                                                                                            \
} while (0)

#define TDE_CHECK_SURFACEPARA_EX(pstSurface, pstRect) do { \
    if ((TDE_MAX_SURFACE_PITCH < pstSurface->stride) || (0 == pstSurface->stride) || (0 == pstRect->height) ||   \
        (0 == pstRect->width) || (pstRect->pos_x < 0) || ((hi_u32)pstRect->pos_x >= pstSurface->width) ||        \
        (pstRect->pos_y < 0) || ((hi_u32)pstRect->pos_y >= pstSurface->height) || (0 == pstSurface->phy_addr)) { \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->width);                                     \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->height);                                    \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, pstSurface->stride);                                    \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, pstSurface->phy_addr);                                  \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstRect->pos_x);                                         \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstRect->pos_y);                                         \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRect->width);                                        \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRect->height);                                       \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION,                                                         \
                             "invalid surface phyaddr or invalid surface size or operation area");               \
        return -1;                                                                                               \
    }                                                                                                            \
} while (0)

#define TDE_CHECK_MBCBCRPARA(pstMbSurface) do { \
    if ((HI_TDE_MB_COLOR_FMT_JPG_YCbCr400MBP < pstMbSurface->mb_color_fmt) &&            \
        (HI_TDE_MB_COLOR_FMT_MAX > pstMbSurface->mb_color_fmt)) {                        \
        if (0 == pstMbSurface->cbcr_stride) {                                            \
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstMbSurface->cbcr_stride);      \
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "mb cbcr_stride is null "); \
            return HI_ERR_TDE_INVALID_PARA;                                              \
        }                                                                                \
        if (0 == pstMbSurface->cbcr_phy_addr) {                                          \
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstMbSurface->cbcr_phy_addr);    \
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "mb cbcr_phy_addr is 0");   \
            return HI_ERR_TDE_INVALID_PARA;                                              \
        }                                                                                \
    }                                                                                    \
} while (0)

#define TDE_CHECK_MBSURFACEPARA(pstMbSurface, pstRect, rectWidth, rectHeight) do { \
    if ((pstRect->pos_x < 0) || (rectWidth < pstRect->width) || (rectHeight < pstRect->height) ||              \
        (0 == pstRect->height) || (0 == pstRect->width) || (TDE_MAX_SURFACE_PITCH < pstMbSurface->y_stride) || \
        (TDE_MAX_SURFACE_PITCH < pstMbSurface->cbcr_stride) ||                                                 \
        ((hi_u32)pstRect->pos_x >= pstMbSurface->y_width) || (pstRect->pos_y < 0) ||                           \
        ((hi_u32)pstRect->pos_y >= pstMbSurface->y_height) || (0 == pstMbSurface->y_addr)) {                   \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstMbSurface->y_width);                               \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstMbSurface->y_height);                              \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, pstMbSurface->y_stride);                              \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, pstMbSurface->y_addr);                                \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstRect->pos_x);                                       \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, pstRect->pos_y);                                       \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRect->width);                                      \
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRect->height);                                     \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION,                                                       \
                             "invalid mbsurface phyaddr or invalid surface size or operation area");           \
        return -1;                                                                                             \
    }                                                                                                          \
} while (0)

#define TDE_CHECK_ROTATERECTAREA(fore_ground_rect, dst_rect, enRotateAngle) do { \
    hi_bool bWrongRotateRectArea = ((fore_ground_rect->height != dst_rect->width) ||            \
                                    (fore_ground_rect->width != dst_rect->height));             \
    if ((TDE_ROTATE_CLOCKWISE_180 != enRotateAngle) && bWrongRotateRectArea) {                  \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enRotateAngle);                              \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Rotate operation rect is wrong!");    \
        return -1;                                                                              \
    }                                                                                           \
    if ((TDE_ROTATE_CLOCKWISE_180 == enRotateAngle) && bWrongRotateRectArea) {                  \
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, enRotateAngle);                              \
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Rotate 180 operation rect is wrong"); \
        return -1;                                                                              \
    }                                                                                           \
} while (0)

#define TDE_OSI_FOUR_BITS_SHIFT 4
#define TDE_OSI_THREE_BITS_SHIFT 3
#define TDE_OSI_TWO_BITS_SHIFT 2
#define TDE_OSI_ONE_BIT_SHIFT 1
#define TDE_OSI_ZERO_BIT_SHIFT 0
#define TDE_OSI_20_BIT_SHIFT 20

/* ************************** Enum Definition ********************************************** */

/* ************************** Structure Definition ***************************************** */

/****************************************************************************/
/* TDE osi ctl struct definition */
/****************************************************************************/

typedef struct {
    hi_u32 node_num;
    hi_u32 dst_width;
    hi_u32 dst_hoffset_pix;
    hi_u32 dst_h_scan_ord;
    hi_u32 dst_crop_en;
    hi_u32 dst_crop_start_x;
    hi_u32 dst_crop_end_x;
    hi_u32 dst_fmt;
    hi_u32 dst_h_dwsm_ow;
    hi_u32 dst_h_dswm_mode;
    hi_u32 dst_xst_pos_cord_crop_in;
    hi_u32 dst_xed_pos_cord_crop_in;
    hi_u32 dst_xst_pos_blk;
    hi_u32 dst_xed_pos_blk;
    hi_u32 crop_start_x;
    hi_u32 crop_end_x;
    hi_u32 cbm_mode;

    hi_u32 sx_xfpos;
    hi_u32 srcx_en;
    hi_u32 srcx_fmt;
    hi_u32 srcx_hratio;
    hi_u32 srcx_hor_loffset;
    hi_u32 srcx_hor_coffset;
    hi_u32 srcx_iw;
    hi_u32 srcx_ow;
    hi_u32 srcx_422v_pro;
    hi_u32 srcx_hlmsc_en;
    hi_u32 srcx_hchmsc_en;
    hi_u32 srcx_hpzme_en;
    hi_u32 srcx_hpzme_mode;
    hi_u32 srcx_hpzme_width;
    hi_u32 srcx_width;
    hi_u32 srcx_h_scan_ord;
    hi_u32 srcx_xdpos;
    hi_u32 srcx_xst_pos_blk;
    hi_u32 srcx_xed_pos_blk;
    hi_u32 srcx_xst_pos_cord;
    hi_u32 srcx_xed_pos_cord;
    hi_u32 srcx_xst_pos_cord_in;
    hi_u32 srcx_xed_pos_cord_in;
    hi_u32 srcx_hor_loffset_cfg_int_comp;
    hi_u32 srcx_hor_loffset_cfg_int;
    hi_u32 srcx_hor_loffset_cfg_fraction;
    hi_u32 srcx_hor_loffset_pix_fraction;
    hi_u32 srcx_hor_loffset_fraction;
    hi_u32 srcx_xst_pos_cord_in_offset;
    hi_u32 srcx_xed_pos_cord_in_offset;
    hi_u32 srcx_xst_pos_cord_in_tap_rgb;
    hi_u32 srcx_xed_pos_cord_in_tap_rgb;
    hi_u32 srcx_hor_loffset_int;
    hi_u32 srcx_hor_loffset_int_complement;
    hi_u32 node_cfg_srcx_iw_rgb;
    hi_u32 node_cfg_srcx_hor_loffset_rgb;
    hi_u32 srcx_xst_pos_cord_in_tap_luma;
    hi_u32 srcx_xed_pos_cord_in_tap_luma;
    hi_u32 srcx_xst_pos_cord_in_chroma;
    hi_u32 srcx_xed_pos_cord_in_chroma;
    hi_u32 srcx_hor_coffset_cfg_int_comp;
    hi_u32 srcx_hor_coffset_cfg_int;
    hi_u32 srcx_hor_coffset_cfg_fraction;
    hi_u32 srcx_hor_coffset_pix_fraction;
    hi_u32 srcx_hor_coffset_fraction;
    hi_u32 srcx_xst_pos_cord_in_offset_chroma;
    hi_u32 srcx_xed_pos_cord_in_offset_chroma;
    hi_u32 srcx_xst_pos_cord_in_tap_chroma;
    hi_u32 srcx_xed_pos_cord_in_tap_chroma;
    hi_u32 srcx_xst_pos_cord_in_tap_chroma_x2;
    hi_u32 srcx_xed_pos_cord_in_tap_chroma_x2;
    hi_u32 srcx_xst_pos_cord_in_tap_sp;
    hi_u32 srcx_xed_pos_cord_in_tap_sp;
    hi_u32 node_cfg_srcx_iw_sp;
    hi_u32 srcx_hor_loffset_int_sp;
    hi_u32 srcx_hor_coffset_int_sp;
    hi_u32 srcx_hor_loffset_int_sp_complent;
    hi_u32 srcx_hor_coffset_int_sp_complent;
    hi_u32 node_cfg_srcx_hor_loffset_sp;
    hi_u32 node_cfg_srcx_hor_coffset_sp;
    hi_u32 srcx_xst_pos_cord_in_tap;
    hi_u32 srcx_xed_pos_cord_in_tap;
    hi_u32 srcx_xst_pos_cord_in_tap_hpzme;
    hi_u32 srcx_xed_pos_cord_in_tap_hpzme;
    hi_u32 srcx_xst_pos_cord_in_tap_hpzme_hso;
    hi_u32 srcx_xed_pos_cord_in_tap_hpzme_hso;

    hi_u32 srcx_8ali_en;
    hi_u32 srcx_xst_pos_cord_in_tap_hpzme_8ali;
    hi_u32 srcx_xst_pos_cord_in_tap_8ali;
    hi_u32 srcx_xed_pos_cord_in_tap_hpzme_8ali;
    hi_u32 srcx_xed_pos_cord_in_tap_8ali;
    hi_u32 node_cfg_srcx_iw_8ali;
    hi_u32 srcx_hor_loffset_int_8ali;
    hi_u32 srcx_hor_loffset_int_complement_8ali;
    hi_u32 node_cfg_srcx_hor_loffset_8ali;

    hi_u32 srcx_hor_loffset_int_beyond;
    hi_u32 srcx_hor_loffset_int_beyond_complent;
} tde_slice_data;

/* pixel format transform type */
typedef enum {
    TDE_COLORFMT_TRANSFORM_ARGB_ARGB = 0,
    TDE_COLORFMT_TRANSFORM_ARGB_YCbCr,
    TDE_COLORFMT_TRANSFORM_CLUT_ARGB,
    TDE_COLORFMT_TRANSFORM_CLUT_YCbCr,
    TDE_COLORFMT_TRANSFORM_CLUT_CLUT,
    TDE_COLORFMT_TRANSFORM_YCbCr_ARGB,
    TDE_COLORFMT_TRANSFORM_YCbCr_YCbCr,
    TDE_COLORFMT_TRANSFORM_An_An,
    TDE_COLORFMT_TRANSFORM_ARGB_An,
    TDE_COLORFMT_TRANSFORM_YCbCr_An,
    TDE_COLORFMT_TRANSFORM_BUTT,
} TDE_COLORFMT_TRANSFORM_E;

/* CLUT table use */
typedef enum {
    TDE_CLUT_COLOREXPENDING = 0, /* color expend */
    TDE_CLUT_COLORCORRECT,       /* color correct */
    TDE_CLUT_CLUT_BYPASS,
    TDE_CLUT_USAGE_BUTT
} TDE_CLUT_USAGE_E;

/* frame/filed operate mode */
typedef enum {
    TDE_FRAME_PIC_MODE = 0, /* frame operate mode */
    TDE_TOP_FIELD_PIC_MODE, /* top filed operate mode */
    TDE_PIC_MODE_BUTT
} TDE_PIC_MODE_E;

typedef struct {
    hi_u32 u32SliceWidth;
    hi_u32 u32SliceWi;
    hi_s32 s32SliceCOfst;
    hi_s32 s32SliceLOfst;
    hi_u32 u32SliceHoffset;
    hi_u32 u32SliceWo;
    hi_u32 u32SliceWHpzme;
    hi_u32 u32SliceDstWidth;
    hi_u32 u32SliceDstHeight;
    hi_u32 u32SliceDstHoffset;
    tde_slice_type slice_type;
    hi_u32 dst_crop_en;
    hi_u32 dst_crop_start_x;
    hi_u32 dst_crop_end_x;
} TDE_SLICE_INFO_S;

typedef enum {
    TDE_OPERATION_SINGLE_SRC1 = 0,
    TDE_OPERATION_SINGLE_SRC2,
    TDE_OPERATION_DOUBLE_SRC,
    TDE_OPERATION_BUTT
} TDE_OPERATION_CATEGORY_E;

typedef enum {
    TDE_PATTERN_OPERATION_SINGLE_SRC = 0,
    TDE_PATTERN_OPERATION_DOUBLE_SRC,
    TDE_PATTERN_OPERATION_BUTT
} TDE_PATTERN_OPERATION_CATEGORY_E;

/****************************************************************************/
/* TDE osi ctl inner variables definition */
/****************************************************************************/
tde_color_fmt g_enTdeCommonDrvColorFmt[HI_TDE_COLOR_FMT_MAX + 1] = {
    TDE_DRV_COLOR_FMT_RGB444,       TDE_DRV_COLOR_FMT_RGB444,      TDE_DRV_COLOR_FMT_RGB555,
    TDE_DRV_COLOR_FMT_RGB555,       TDE_DRV_COLOR_FMT_RGB565,      TDE_DRV_COLOR_FMT_RGB565,
    TDE_DRV_COLOR_FMT_RGB888,       TDE_DRV_COLOR_FMT_RGB888,      TDE_DRV_COLOR_FMT_ARGB4444,
    TDE_DRV_COLOR_FMT_ARGB4444,     TDE_DRV_COLOR_FMT_ARGB4444,    TDE_DRV_COLOR_FMT_ARGB4444,
    TDE_DRV_COLOR_FMT_ARGB1555,     TDE_DRV_COLOR_FMT_ARGB1555,    TDE_DRV_COLOR_FMT_ARGB1555,
    TDE_DRV_COLOR_FMT_ARGB1555,     TDE_DRV_COLOR_FMT_ARGB8565,    TDE_DRV_COLOR_FMT_ARGB8565,
    TDE_DRV_COLOR_FMT_ARGB8565,     TDE_DRV_COLOR_FMT_ARGB8565,    TDE_DRV_COLOR_FMT_ARGB8888,
    TDE_DRV_COLOR_FMT_ARGB8888,     TDE_DRV_COLOR_FMT_ARGB8888,    TDE_DRV_COLOR_FMT_ARGB8888,
    TDE_DRV_COLOR_FMT_RABG8888,     TDE_DRV_COLOR_FMT_ABGR2101010, TDE_DRV_COLOR_FMT_FP16,
    TDE_DRV_COLOR_FMT_ABGR10101010, TDE_DRV_COLOR_FMT_CLUT1,       TDE_DRV_COLOR_FMT_CLUT2,
    TDE_DRV_COLOR_FMT_CLUT4,        TDE_DRV_COLOR_FMT_CLUT8,       TDE_DRV_COLOR_FMT_ACLUT44,
    TDE_DRV_COLOR_FMT_ACLUT88,      TDE_DRV_COLOR_FMT_A1,          TDE_DRV_COLOR_FMT_A8,
    TDE_DRV_COLOR_FMT_YCbCr888,     TDE_DRV_COLOR_FMT_AYCbCr8888,  TDE_DRV_COLOR_FMT_YCbCr422,
    TDE_DRV_COLOR_FMT_byte,         TDE_DRV_COLOR_FMT_halfword,    TDE_DRV_COLOR_FMT_YCbCr400MBP,
    TDE_DRV_COLOR_FMT_YCbCr422MBH,  TDE_DRV_COLOR_FMT_YCbCr422MBV, TDE_DRV_COLOR_FMT_YCbCr420MB,
    TDE_DRV_COLOR_FMT_YCbCr420MB,   TDE_DRV_COLOR_FMT_YCbCr420MB,  TDE_DRV_COLOR_FMT_YCbCr420MB,
    TDE_DRV_COLOR_FMT_YCbCr444MB,   TDE_DRV_COLOR_FMT_MAX
};
STATIC tde_argb_order_mode g_enTdeArgbOrder[HI_TDE_COLOR_FMT_MAX + 1] = {
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_ARGB,
    TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR,
    TDE_DRV_ORDER_RGBA, TDE_DRV_ORDER_BGRA, TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_RGBA,
    TDE_DRV_ORDER_BGRA, TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_RGBA, TDE_DRV_ORDER_BGRA,
    TDE_DRV_ORDER_ARGB, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_RGBA, TDE_DRV_ORDER_BGRA, TDE_DRV_ORDER_RABG,
    TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_ABGR, TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,
    TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,
    TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,
    TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,
    TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX,  TDE_DRV_ORDER_MAX
};

/* ****************************** API declaration ****************************************** */

/* ****************************** API release ********************************************** */
#ifndef HI_BUILD_IN_BOOT
STATIC hi_bool s_bRegionDeflicker = HI_FALSE;
#endif

/****************************************************************************/
/* TDE osi ctl inner interface definition */
/****************************************************************************/
STATIC tde_color_fmt_category TdeOsiGetFmtCategory(hi_tde_color_fmt color_fmt);

STATIC TDE_COLORFMT_TRANSFORM_E TdeOsiGetFmtTransType(hi_tde_color_fmt enSrc2Fmt, hi_tde_color_fmt enDstFmt);

STATIC INLINE hi_s32 TdeOsiSetClutOpt(hi_tde_surface *pClutSur, hi_tde_surface *pOutSur, TDE_CLUT_USAGE_E *penClutUsage,
                                      hi_bool clut_reload, tde_hardware_node *hardware_node);

#ifndef HI_BUILD_IN_BOOT
STATIC INLINE TDE_CLUT_USAGE_E TdeOsiGetClutUsage(hi_tde_color_fmt enSrcFmt, hi_tde_color_fmt enDstFmt);
#endif

STATIC hi_s32 TdeOsiGetScanInfo(hi_tde_surface *pSrc, hi_tde_rect *src_rect, hi_tde_surface *dst_surface,
                                hi_tde_rect *dst_rect, hi_tde_opt *opt, tde_scandirection_mode *pstSrcDirection,
                                tde_scandirection_mode *pstDstDirection);

#ifndef HI_BUILD_IN_BOOT
STATIC hi_s32 TdeOsiGetInterRect(hi_tde_rect *pRect1, hi_tde_rect *pRect2, hi_tde_rect *pInterRect);
#endif

STATIC INLINE hi_s32 TdeOsiSetMbPara(hi_s32 handle, hi_tde_mb_surface *mb_surface, hi_tde_rect *mb_rect,
                                     hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_mb_opt *pMbOpt);

STATIC hi_s32 TdeOsiSetFilterNode(hi_s32 handle, tde_hardware_node *hardware_node, hi_tde_surface *pstBackGround,
                                  hi_tde_rect *back_ground_rect, hi_tde_surface *fore_ground_surface,
                                  hi_tde_rect *fore_ground_rect, hi_tde_surface *dst_surface, hi_tde_rect *dst_rect,
                                  hi_tde_deflicker_mode deflicker_mode, hi_tde_filter_mode enFliterMode);

STATIC hi_s32 TdeOsi1SourceFill(hi_s32 handle, hi_tde_surface *dst_surface, hi_tde_rect *dst_rect,
                                hi_tde_fill_color *fill_color, hi_tde_opt *opt);

STATIC hi_s32 TdeOsiSingleSrc2Blit(hi_s32 handle, hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                                   hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_opt *opt,
                                   hi_bool mmz_for_src, hi_bool mmz_for_dst);

#ifndef HI_BUILD_IN_BOOT
STATIC hi_s32 TdeOsi2SourceFill(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                                hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_fill_color *fill_color,
                                hi_tde_opt *opt);

#endif

STATIC INLINE hi_s32 TdeOsiCheckSurface(hi_tde_surface *pstSurface, hi_tde_rect *pstRect);

#ifndef HI_BUILD_IN_BOOT
STATIC TDE_OPERATION_CATEGORY_E TdeOsiGetOptCategory(hi_tde_surface *pstBackGround, hi_tde_rect *back_ground_rect,
                                                     hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                                                     hi_tde_surface *dst_surface, hi_tde_rect *dst_rect,
                                                     hi_tde_opt *opt);
#endif

STATIC INLINE hi_void TdeOsiConvertSurface(hi_tde_surface *pstSur, hi_tde_rect *pstRect,
                                           tde_scandirection_mode *pstScanInfo, tde_surface_msg *pstDrvSur,
                                           hi_tde_rect *pstOperationArea);

STATIC hi_s32 TdeOsiSetClipPara(hi_tde_surface *pstBackGround, hi_tde_rect *pstBGRect,
                                hi_tde_surface *fore_ground_surface, hi_tde_rect *pstFGRect,
                                hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_opt *opt,
                                tde_hardware_node *pstHwNode);

STATIC hi_s32 TdeOsiSetBaseOptParaForBlit(hi_tde_opt *opt, hi_tde_surface *pstSrc1, hi_tde_surface *pstSrc2,
                                          TDE_OPERATION_CATEGORY_E enOptCategory, tde_hardware_node *pstHwNode);

STATIC hi_s32 TdeOsiSetNodeFinish(hi_s32 handle, tde_hardware_node *hardware_node, hi_u32 work_buf_num,
                                  tde_node_submit_type submit_type);

STATIC INLINE hi_s32 TdeOsiCheckResizePara(hi_u32 u32InWidth, hi_u32 u32InHeight, hi_u32 u32OutWidth,
                                           hi_u32 u32OutHeight);

#ifndef HI_BUILD_IN_BOOT
STATIC INLINE hi_bool TdeOsiWhetherContainAlpha(hi_tde_color_fmt color_fmt);
#endif

STATIC INLINE hi_void TdeOsiSetExtAlpha(hi_tde_surface *pstBackGround, hi_tde_surface *fore_ground_surface,
                                        tde_hardware_node *pstHwNode);

/****************************************************************************/
/* TDE osi ctl inner interface realization */
/****************************************************************************/
/* Y  =  (263 * R + 516 * G + 100 * B) >> 10 + 16 */
/* Cb =  (-152 * R - 298 * G + 450 * B) >> 10  + 128 */
/* Cr =  (450 * R - 377 * G - 73 * B) >> 10  + 128 */
#ifndef HI_BUILD_IN_BOOT
STATIC hi_void rgb2ycc(hi_u8 r, hi_u8 g, hi_u8 b, hi_u8 *y, hi_u8 *cb, hi_u8 *cr)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    *y = ((((263 * r + 516 * g + 100 * b) >> 9) + 1) >> 1) + 16;
    *cb = ((((-152 * r - 298 * g + 450 * b) >> 9) + 1) >> 1) + 128;
    *cr = ((((450 * r - 377 * g - 73 * b) >> 9) + 1) >> 1) + 128;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

/* R  =  ((298 * (Y-16) + 0 * (Cb-128) + 409 * (Cr-128)) >> 8 */
/* G =  ((298 * (Y-16) - 100 * (Cb-128) - 208 * (Cr-128)) >> 8 */
/* B = ((298 * (Y-16) + 517 * (Cb-128) - 0 * (Cr-128)) >> 8 */
STATIC hi_void ycc2rgb(hi_u8 y, hi_u8 cb, hi_u8 cr, hi_u8 *r, hi_u8 *g, hi_u8 *b)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    *r = (((298 * (y - 16) + 409 * (cr - 128)) >> 7) + 1) >> 1;
    *g = (((298 * (y - 16) - 100 * (cb - 128) - 208 * (cr - 128)) >> 7) + 1) >> 1;
    *b = (((298 * (y - 16) + 517 * (cb - 128)) >> 7) + 1) >> 1;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}
#endif

typedef struct {
    hi_u8 u8AlphaBits;
    hi_u8 u8RedBits;
    hi_u8 u8GreenBits;
    hi_u8 u8BlueBits;
    hi_u8 u8AlphaOffset;
    hi_u8 u8RedOffset;
    hi_u8 u8GreenOffset;
    hi_u8 u8BlueOffset;
} TDE2_FMT_BITOFFSET_S;

#ifndef HI_BUILD_IN_BOOT
STATIC TDE2_FMT_BITOFFSET_S s_u8FmtBitAndOffsetArray[HI_TDE_COLOR_FMT_AYCbCr8888 + 1] = {
    { 0, 4, 4, 4, 12, 8, 4, 0 },  /* HI_TDE_COLOR_FMT_RGB444 */
    { 0, 4, 4, 4, 12, 0, 4, 8 },  /* HI_TDE_COLOR_FMT_BGR444 */
    { 0, 5, 5, 5, 15, 10, 5, 0 }, /* HI_TDE_COLOR_FMT_RGB555 */
    { 0, 5, 5, 5, 15, 0, 5, 10 }, /* HI_TDE_COLOR_FMT_BGR555 */
    { 0, 5, 6, 5, 16, 11, 5, 0 }, /* HI_TDE_COLOR_FMT_RGB565 */
    { 0, 5, 6, 6, 16, 0, 5, 11 }, /* HI_TDE_COLOR_FMT_BGR565 */
    { 0, 8, 8, 8, 24, 16, 8, 0 }, /* HI_TDE_COLOR_FMT_RGB888 */
    { 0, 8, 8, 8, 24, 0, 8, 16 }, /* HI_TDE_COLOR_FMT_BGR888 */
    { 4, 4, 4, 4, 12, 8, 4, 0 },  /* HI_TDE_COLOR_FMT_ARGB4444 */
    { 4, 4, 4, 4, 12, 0, 4, 8 },  /* HI_TDE_COLOR_FMT_ABGR4444 */
    { 4, 4, 4, 4, 0, 12, 8, 4 },  /* HI_TDE_COLOR_FMT_RGBA4444 */
    { 4, 4, 4, 4, 0, 4, 8, 12 },  /* HI_TDE_COLOR_FMT_BGRA4444 */
    { 1, 5, 5, 5, 15, 10, 5, 0 }, /* HI_TDE_COLOR_FMT_ARGB1555 */
    { 1, 5, 5, 5, 15, 0, 5, 10 }, /* HI_TDE_COLOR_FMT_ABGR1555 */
    { 1, 5, 5, 5, 0, 11, 6, 1 },  /* HI_TDE_COLOR_FMT_RGBA1555 */
    { 1, 5, 5, 5, 0, 1, 6, 11 },  /* HI_TDE_COLOR_FMT_BGRA1555 */
    { 8, 5, 6, 5, 16, 11, 5, 0 }, /* HI_TDE_COLOR_FMT_ARGB8565 */
    { 8, 5, 6, 5, 16, 0, 5, 11 }, /* HI_TDE_COLOR_FMT_ABGR8565 */
    { 8, 5, 6, 5, 0, 19, 13, 8 }, /* HI_TDE_COLOR_FMT_RGBA8565 */
    { 8, 5, 6, 6, 0, 8, 13, 19 }, /* HI_TDE_COLOR_FMT_BGRA8565 */
    { 8, 8, 8, 8, 24, 16, 8, 0 }, /* HI_TDE_COLOR_FMT_ARGB8888 */
    { 8, 8, 8, 8, 24, 0, 8, 16 }, /* HI_TDE_COLOR_FMT_ABGR8888 */
    { 8, 8, 8, 8, 0, 24, 16, 8 }, /* HI_TDE_COLOR_FMT_RGBA8888 */
    { 8, 8, 8, 8, 0, 8, 16, 24 }, /* HI_TDE_COLOR_FMT_BGRA8888 */
    { 8, 8, 8, 8, 16, 24, 0, 8 }, /* HI_TDE_COLOR_FMT_RABG8888 */

    { 0, 0, 0, 0, 0, 0, 0, 0 },   { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },   { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 8, 8, 8, 24, 16, 8, 0 }, /* HI_TDE_COLOR_FMT_YCbCr888 */
    { 8, 8, 8, 8, 24, 16, 8, 0 }, /* HI_TDE_COLOR_FMT_AYCbCr8888 */
};
#endif

#ifndef HI_BUILD_IN_BOOT
/*****************************************************************************
 Prototype       : TdeOsiColorConvert
 Description     : translate other RGB  pixel format to ARGB8888's
 Input           : color_fmt    **
                   u32InColor    **
                   pu32OutColor  **
 Output          : None
 Return Value    :
 Global Variable
    Read Only    :
    Read & Write :
*****************************************************************************/
STATIC hi_s32 TdeOsiColorConvert(hi_tde_fill_color *fill_color, hi_tde_surface *pstSur, hi_u32 *pu32OutColor)
{
    hi_u8 a, r, g, b, y, cb, cr;
    TDE_COLORFMT_TRANSFORM_E enColorTrans;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(fill_color, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstSur, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pu32OutColor, HI_FAILURE);

    enColorTrans = TdeOsiGetFmtTransType(fill_color->color_fmt, pstSur->color_fmt);

    if (((HI_TDE_COLOR_FMT_CLUT1 <= fill_color->color_fmt) && (fill_color->color_fmt <= HI_TDE_COLOR_FMT_A8)) ||
        (fill_color->color_fmt >= HI_TDE_COLOR_FMT_YCbCr422)) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HI_TDE_COLOR_FMT_CLUT1);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HI_TDE_COLOR_FMT_A8);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HI_TDE_COLOR_FMT_YCbCr422);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Unsupported color");
        return -1;
    }

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(fill_color->color_fmt, HI_TDE_COLOR_FMT_AYCbCr8888 + 1, HI_FAILURE);

    a = (fill_color->color_value >> s_u8FmtBitAndOffsetArray[fill_color->color_fmt].u8AlphaOffset) &
        (0xff >> (8 - s_u8FmtBitAndOffsetArray[fill_color->color_fmt].u8AlphaBits));
    r = (fill_color->color_value >> s_u8FmtBitAndOffsetArray[fill_color->color_fmt].u8RedOffset) &
        (0xff >> (8 - s_u8FmtBitAndOffsetArray[fill_color->color_fmt].u8RedBits));
    g = (fill_color->color_value >> s_u8FmtBitAndOffsetArray[fill_color->color_fmt].u8GreenOffset) &
        (0xff >> (8 - s_u8FmtBitAndOffsetArray[fill_color->color_fmt].u8GreenBits));
    b = (fill_color->color_value >> s_u8FmtBitAndOffsetArray[fill_color->color_fmt].u8BlueOffset) &
        (0xff >> (8 - s_u8FmtBitAndOffsetArray[fill_color->color_fmt].u8BlueBits));

    if ((HI_TDE_COLOR_FMT_ARGB1555 <= fill_color->color_fmt) && (fill_color->color_fmt <= HI_TDE_COLOR_FMT_BGRA1555)) {
        if (a) {
            a = pstSur->alpha1;
        } else {
            a = pstSur->alpha0;
        }
    } else {
        a = a << (8 - s_u8FmtBitAndOffsetArray[fill_color->color_fmt].u8AlphaBits);
    }

    r = r << (8 - s_u8FmtBitAndOffsetArray[fill_color->color_fmt].u8RedBits);
    g = g << (8 - s_u8FmtBitAndOffsetArray[fill_color->color_fmt].u8GreenBits);
    b = b << (8 - s_u8FmtBitAndOffsetArray[fill_color->color_fmt].u8BlueBits);

    switch (enColorTrans) {
        case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
        case TDE_COLORFMT_TRANSFORM_YCbCr_YCbCr: {
            *pu32OutColor = (a << 24) + (r << 16) + (g << 8) + b;
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 0;
        }

        case TDE_COLORFMT_TRANSFORM_ARGB_YCbCr: {
            rgb2ycc(r, g, b, &y, &cb, &cr);
            *pu32OutColor = (a << 24) + (y << 16) + (cb << 8) + cr;
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 0;
        }

        case TDE_COLORFMT_TRANSFORM_YCbCr_ARGB: {
            ycc2rgb(r, g, b, &y, &cb, &cr);
            *pu32OutColor = (a << 24) + (y << 16) + (cb << 8) + cr;
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 0;
        }
        default:
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Unsupported color transport");
            return -1;
    }
}
#endif

/*****************************************************************************
 * Function:      TdeOsiCheckResizePara
 * Description:   check zoom ratio limit
 * Input:         pstInRect: rect before zoom
 *                pstOutRect: rect after zoom
 * Output:        none
 * Return:        tde_color_fmt_category   pixel format category
 * Others:        none
 *****************************************************************************/
STATIC hi_s32 TdeOsiCheckResizePara(hi_u32 u32InWidth, hi_u32 u32InHeight, hi_u32 u32OutWidth, hi_u32 u32OutHeight)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if ((TDE_MAX_RECT_WIDTH < u32InWidth) || (TDE_MAX_RECT_HEIGHT < u32InHeight) ||
        (TDE_MAX_RECT_WIDTH < u32OutWidth) || (TDE_MAX_RECT_HEIGHT < u32OutHeight)) {
        if ((u32InWidth != u32OutWidth) || (u32InHeight != u32OutHeight)) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32InWidth);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32OutWidth);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32InHeight);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32OutHeight);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Can not support resize");
            return HI_ERR_TDE_INVALID_PARA;
        }
    }

    if (((u32OutWidth * TDE_MAX_MINIFICATION_H) < u32InWidth) ||
        ((u32OutHeight * TDE_MAX_MINIFICATION_V) < u32InHeight)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32InWidth);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32OutWidth);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32InHeight);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32OutHeight);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Resize parameter error");
        return -1;
    } else {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 0;
    }
}

hi_tde_color_fmt TdeOsiCovertMbFmt(hi_tde_mb_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case HI_TDE_MB_COLOR_FMT_JPG_YCbCr400MBP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_TDE_COLOR_FMT_JPG_YCbCr400MBP;
        case HI_TDE_MB_COLOR_FMT_JPG_YCbCr422MBHP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_TDE_COLOR_FMT_JPG_YCbCr422MBHP;
        case HI_TDE_MB_COLOR_FMT_JPG_YCbCr422MBVP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_TDE_COLOR_FMT_JPG_YCbCr422MBVP;
        case HI_TDE_MB_COLOR_FMT_MP1_YCbCr420MBP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_TDE_COLOR_FMT_MP1_YCbCr420MBP;
        case HI_TDE_MB_COLOR_FMT_MP2_YCbCr420MBP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_TDE_COLOR_FMT_MP2_YCbCr420MBP;
        case HI_TDE_MB_COLOR_FMT_MP2_YCbCr420MBI:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_TDE_COLOR_FMT_MP2_YCbCr420MBI;
        case HI_TDE_MB_COLOR_FMT_JPG_YCbCr420MBP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_TDE_COLOR_FMT_JPG_YCbCr420MBP;
        case HI_TDE_MB_COLOR_FMT_JPG_YCbCr444MBP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_TDE_COLOR_FMT_JPG_YCbCr444MBP;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_TDE_COLOR_FMT_MAX;
    }
}

/*****************************************************************************
 * Function:      TdeOsiGetFmtCategory
 * Description:   get pixel format category info
 * Input:         color_fmt: pixel format
 * Output:        none
 * Return:        tde_color_fmt_category  pixel format category
 * Others:        none
 *****************************************************************************/
STATIC tde_color_fmt_category TdeOsiGetFmtCategory(hi_tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    /* target is ARGB format */
    if (color_fmt <= HI_TDE_COLOR_FMT_RABG8888) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return TDE_COLORFMT_CATEGORY_ARGB;
    }
    /* target is CLUT table format */
    else if (color_fmt <= HI_TDE_COLOR_FMT_ACLUT88) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return TDE_COLORFMT_CATEGORY_CLUT;
    }
    /* target is alpha CLUT table format */
    else if (color_fmt <= HI_TDE_COLOR_FMT_A8) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return TDE_COLORFMT_CATEGORY_An;
    }
    /* target is YCbCr format */
    else if (color_fmt <= HI_TDE_COLOR_FMT_YCbCr422) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return TDE_COLORFMT_CATEGORY_YCbCr;
    }
    /* byte format */
    else if (color_fmt == HI_TDE_COLOR_FMT_byte) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return TDE_COLORFMT_CATEGORY_BYTE;
    }
    /* halfword  format */
    else if (color_fmt == HI_TDE_COLOR_FMT_halfword) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return TDE_COLORFMT_CATEGORY_HALFWORD;
    } else if (color_fmt <= HI_TDE_COLOR_FMT_JPG_YCbCr444MBP) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return TDE_COLORFMT_CATEGORY_YCbCr;
    }
    /* error format */
    else {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return TDE_COLORFMT_CATEGORY_MAX;
    }
}

/*****************************************************************************
* Function:      TdeOsiGetFmtTransType
* Description:   get pixel format transform type
* Input:         enSrc2Fmt: foreground pixel format
                 enDstFmt: target pixel format
* Output:        none
* Return:        TDE_COLORFMT_TRANSFORM_E pixel format transform type
* Others:        none
*****************************************************************************/
STATIC TDE_COLORFMT_TRANSFORM_E TdeOsiGetFmtTransType(hi_tde_color_fmt enSrc2Fmt, hi_tde_color_fmt enDstFmt)
{
    tde_color_fmt_category enSrcCategory;
    tde_color_fmt_category enDstCategory;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* get foreground pixel format category */
    enSrcCategory = TdeOsiGetFmtCategory(enSrc2Fmt);

    /* get target pixel format category */
    enDstCategory = TdeOsiGetFmtCategory(enDstFmt);

    switch (enSrcCategory) {
        case TDE_COLORFMT_CATEGORY_ARGB: {
            if (TDE_COLORFMT_CATEGORY_ARGB == enDstCategory) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return TDE_COLORFMT_TRANSFORM_ARGB_ARGB;
            }

            if (TDE_COLORFMT_CATEGORY_YCbCr == enDstCategory) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return TDE_COLORFMT_TRANSFORM_ARGB_YCbCr;
            }

            if (TDE_COLORFMT_CATEGORY_An == enDstCategory) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return TDE_COLORFMT_TRANSFORM_ARGB_An;
            }
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return TDE_COLORFMT_TRANSFORM_BUTT;
        }

        case TDE_COLORFMT_CATEGORY_CLUT: {
            if (TDE_COLORFMT_CATEGORY_ARGB == enDstCategory) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return TDE_COLORFMT_TRANSFORM_CLUT_ARGB;
            }

            if (TDE_COLORFMT_CATEGORY_YCbCr == enDstCategory) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return TDE_COLORFMT_TRANSFORM_CLUT_YCbCr;
            }

            if (TDE_COLORFMT_CATEGORY_CLUT == enDstCategory) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return TDE_COLORFMT_TRANSFORM_CLUT_CLUT;
            }
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return TDE_COLORFMT_TRANSFORM_BUTT;
        }

        case TDE_COLORFMT_CATEGORY_YCbCr: {
            if (TDE_COLORFMT_CATEGORY_ARGB == enDstCategory) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return TDE_COLORFMT_TRANSFORM_YCbCr_ARGB;
            }

            if (TDE_COLORFMT_CATEGORY_YCbCr == enDstCategory) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return TDE_COLORFMT_TRANSFORM_YCbCr_YCbCr;
            }

            if (TDE_COLORFMT_CATEGORY_An == enDstCategory) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return TDE_COLORFMT_TRANSFORM_ARGB_An;
            }
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return TDE_COLORFMT_TRANSFORM_BUTT;
        }
        case TDE_COLORFMT_CATEGORY_An: {
            if (TDE_COLORFMT_CATEGORY_An == enDstCategory) {
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return TDE_COLORFMT_TRANSFORM_An_An;
            }
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return TDE_COLORFMT_TRANSFORM_BUTT;
        }
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return TDE_COLORFMT_TRANSFORM_BUTT;
    }
}

#ifndef HI_BUILD_IN_BOOT
/*****************************************************************************
 * Function:      TdeOsiIsSingleSrc2Rop
 * Description:   query if ROP operate is if single source2 operate
 * Input:         enRop: rop operate type
 * Output:        none
 * Return:        HI_TRUE: single ROP;HI_FALSE: non single ROP
 * Others:        none
 *****************************************************************************/
STATIC INLINE hi_bool TdeOsiIsSingleSrc2Rop(hi_tde_rop_mode enRop)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (enRop) {
        case HI_TDE_ROP_BLACK:
        case HI_TDE_ROP_NOTCOPYPEN:
        case HI_TDE_ROP_COPYPEN:
        case HI_TDE_ROP_WHITE:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_TRUE;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_FALSE;
    }
}

/*****************************************************************************
* Function:      TdeOsiGetClutUsage
* Description:   get CLUT table usage
* Input:         enSrc2Fmt foreground pixel format
                 enDstFmt  target pixel format
* Output:        none
* Return:        TDE_CLUT_USAGE_E:  clut  usage
* Others:        none
*****************************************************************************/
STATIC INLINE TDE_CLUT_USAGE_E TdeOsiGetClutUsage(hi_tde_color_fmt enSrcFmt, hi_tde_color_fmt enDstFmt)
{
    TDE_COLORFMT_TRANSFORM_E enColorTransType;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    enColorTransType = TdeOsiGetFmtTransType(enSrcFmt, enDstFmt);
    switch (enColorTransType) {
        case TDE_COLORFMT_TRANSFORM_CLUT_ARGB:
        case TDE_COLORFMT_TRANSFORM_CLUT_YCbCr:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return TDE_CLUT_COLOREXPENDING;
        case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
        case TDE_COLORFMT_TRANSFORM_YCbCr_YCbCr:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return TDE_CLUT_COLORCORRECT;
        case TDE_COLORFMT_TRANSFORM_CLUT_CLUT:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return TDE_CLUT_CLUT_BYPASS;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return TDE_CLUT_USAGE_BUTT;
    }
}

STATIC INLINE hi_bool TdeOsiWhetherContainAlpha(hi_tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case HI_TDE_COLOR_FMT_ARGB4444:
        case HI_TDE_COLOR_FMT_ABGR4444:
        case HI_TDE_COLOR_FMT_RGBA4444:
        case HI_TDE_COLOR_FMT_BGRA4444:
        case HI_TDE_COLOR_FMT_ARGB1555:
        case HI_TDE_COLOR_FMT_ABGR1555:
        case HI_TDE_COLOR_FMT_RGBA1555:
        case HI_TDE_COLOR_FMT_BGRA1555:
        case HI_TDE_COLOR_FMT_ARGB8565:
        case HI_TDE_COLOR_FMT_ABGR8565:
        case HI_TDE_COLOR_FMT_RGBA8565:
        case HI_TDE_COLOR_FMT_BGRA8565:
        case HI_TDE_COLOR_FMT_ARGB8888:
        case HI_TDE_COLOR_FMT_ABGR8888:
        case HI_TDE_COLOR_FMT_RGBA8888:
        case HI_TDE_COLOR_FMT_BGRA8888:
        case HI_TDE_COLOR_FMT_AYCbCr8888:
        case HI_TDE_COLOR_FMT_RABG8888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_TRUE;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_FALSE;
    }
}
#endif

#define TDE_CLUT_SIZE (256 * 4)
/*****************************************************************************
 * Function:      TdeOsiSetClutOpt
 * Description:   set clut parameter
 * Input:         pClutSur Clut bitmap info
 *                penClutUsage  return clut usage type: expand/adjust
 *                clut_reload   if reload clut
 *                hardware_node     hardware node information
 * Output:        none
 * Return:        TDE_CLUT_USAGE_E: clut usage
 *****************************************************************************/
STATIC INLINE hi_s32 TdeOsiSetClutOpt(hi_tde_surface *pClutSur, hi_tde_surface *pOutSur, TDE_CLUT_USAGE_E *penClutUsage,
                                      hi_bool clut_reload, tde_hardware_node *hardware_node)
{
#ifdef HI_BUILD_IN_BOOT
#else
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pClutSur, HI_ERR_TDE_UNSUPPORTED_OPERATION);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pOutSur, HI_ERR_TDE_UNSUPPORTED_OPERATION);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(penClutUsage, HI_ERR_TDE_UNSUPPORTED_OPERATION);

    if (pClutSur->clut_phy_addr != 0) {
        tde_color_fmt_category enFmtCate;
        hi_size_t clutPhyaddr = (hi_size_t)(uintptr_t)pClutSur->clut_phy_addr;

        enFmtCate = TdeOsiGetFmtCategory(pOutSur->color_fmt);

        /* when user input the type of clut is not consistent with output format,rerurn error */
        if ((!pClutSur->is_ycbcr_clut && TDE_COLORFMT_CATEGORY_YCbCr == enFmtCate) ||
            (pClutSur->is_ycbcr_clut && TDE_COLORFMT_CATEGORY_ARGB == enFmtCate)) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, TDE_COLORFMT_CATEGORY_YCbCr);
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, TDE_COLORFMT_CATEGORY_ARGB);
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enFmtCate);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "clut fmt not same");
            return HI_ERR_TDE_UNSUPPORTED_OPERATION;
        }

        /* Because of unsupport output CSC, input and background color zone is consistent.
            In trine sources operation,clut need use background, so output color zone instand of background's */
        *penClutUsage = TdeOsiGetClutUsage(pClutSur->color_fmt, pOutSur->color_fmt);
        if (TDE_CLUT_CLUT_BYPASS > *penClutUsage) {
            tde_clut_cmd stClutCmd;
            if (TDE_CLUT_COLOREXPENDING == *penClutUsage) {
                stClutCmd.clut_mode = TDE_COLOR_EXP_CLUT_MODE;
            } else {
                stClutCmd.clut_mode = TDE_COLOR_CORRCT_CLUT_MODE;
            }

            if (clutPhyaddr % 4) {
                GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pClutSur->clut_phy_addr);
                return HI_ERR_TDE_NOT_ALIGNED;
            }

            stClutCmd.clut_phy_addr = (hi_u32)(uintptr_t)pClutSur->clut_phy_addr;
            if (tde_hal_node_set_clut_opt(hardware_node, &stClutCmd, clut_reload) < 0) {
                GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_clut_opt, FAILURE_TAG);
                return HI_ERR_TDE_UNSUPPORTED_OPERATION;
            }
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
#endif
    return HI_SUCCESS;
}

/*****************************************************************************
* Function:      TdeOsiCheckSingleSrc2Opt
* Description:   check if valid of foreground single source operate
* Input:         enSrc2Fmt foreground pixel format
                 enDstFmt  target pixel format
                 opt     operate attribute pointer
* Output:        none
* Return:        0  valid parameter;
                 -1 invalid parameter;
* Others:        none
*****************************************************************************/
STATIC hi_s32 TdeOsiCheckSingleSrc2Opt(hi_tde_color_fmt enSrc2Fmt, hi_tde_color_fmt enDstFmt, hi_tde_opt *opt)
{
    TDE_COLORFMT_TRANSFORM_E enColorTransType;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(opt, HI_FAILURE);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* return error, if enable color key */
    if (HI_TDE_COLOR_KEY_MODE_NONE != opt->color_key_mode) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It doesn't support colorkey in single source mode");
        return -1;
    }

    TDE_CHECK_ALUCMD(opt->alpha_blending_cmd);

#ifndef HI_BUILD_IN_BOOT
    /* if operate type is ROP and it is not single operate,return error */
    if ((hi_u32)opt->alpha_blending_cmd & HI_TDE_ALPHA_BLENDING_ROP) {
        TDE_CHECK_ROPCODE(opt->rop_alpha);
        TDE_CHECK_ROPCODE(opt->rop_color);
        if ((!TdeOsiIsSingleSrc2Rop(opt->rop_alpha)) || (!TdeOsiIsSingleSrc2Rop(opt->rop_color))) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiIsSingleSrc2Rop, FAILURE_TAG);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Only support single s2 rop");
            return -1;
        }
    }
#endif

    /* single source can not do blend operate */
    if ((hi_u32)opt->alpha_blending_cmd & HI_TDE_ALPHA_BLENDING_BLEND) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Alu mode error");
        return -1;
    }
    TDE_CHECK_DST_FMT(enDstFmt);
    enColorTransType = TdeOsiGetFmtTransType(enSrc2Fmt, enDstFmt);
    if (TDE_COLORFMT_TRANSFORM_BUTT == enColorTransType) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, TDE_COLORFMT_TRANSFORM_BUTT);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enColorTransType);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Unkown color transport type");
        return -1;
    }

    if (TDE_COLORFMT_TRANSFORM_CLUT_CLUT == enColorTransType) {
        /* unsupport deflicker,zoom, Rop, mirror,colorize */
        if ((opt->deflicker_mode != HI_TDE_DEFLICKER_MODE_NONE) || (opt->resize) ||
            ((hi_u32)opt->alpha_blending_cmd & HI_TDE_ALPHA_BLENDING_ROP) ||
            ((hi_u32)opt->alpha_blending_cmd & HI_TDE_ALPHA_BLENDING_COLORIZE)
            /* || (HI_TDE_MIRROR_NONE != opt->mirror) */) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It doesn't support deflicker or ROP or mirror");
            return -1;
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return 0;
}

/*****************************************************************************
* Function:      TdeOsiCheckDoubleSrcOpt
* Description:   check if valid of dual source operate
* Input:         enSrc1Fmt background pixel format
                 enSrc2Fmt foreground pixel format
                 enDstFmt  target pixel format
                 opt    operate attribute operate
* Output:        none
* Return:        0  valid parameter;
                 -1 invalid parameter;
* Others:        none
*****************************************************************************/
STATIC hi_s32 TdeOsiCheckDoubleSrcOpt(hi_tde_color_fmt enSrc1Fmt, hi_tde_color_fmt enSrc2Fmt, hi_tde_color_fmt enDstFmt,
                                      hi_tde_opt *opt)
{
    tde_color_fmt_category enSrc1Category;
    tde_color_fmt_category enSrc2Category;
    tde_color_fmt_category enDstCategory;
    hi_bool bUnknownFmt = HI_FALSE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    TDE_CHECK_DST_FMT(enDstFmt);

    TDE_CHECK_BACKGROUND_FMT(enSrc1Fmt);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(opt, HI_FAILURE);
    TDE_CHECK_ALUCMD(opt->alpha_blending_cmd);

    /* get background pixel format category */
    enSrc1Category = TdeOsiGetFmtCategory(enSrc1Fmt);

    /* get foreground pixel format category */
    enSrc2Category = TdeOsiGetFmtCategory(enSrc2Fmt);

    /* get target pixel format category */
    enDstCategory = TdeOsiGetFmtCategory(enDstFmt);

    bUnknownFmt = ((TDE_COLORFMT_CATEGORY_BYTE <= enSrc1Category) || (TDE_COLORFMT_CATEGORY_BYTE <= enSrc2Category) ||
                   (TDE_COLORFMT_CATEGORY_BYTE <= enDstCategory));

    if (bUnknownFmt) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, TDE_COLORFMT_CATEGORY_BYTE);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enSrc1Category);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enSrc2Category);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enDstCategory);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "unkown format");
        return -1;
    }

#ifndef HI_BUILD_IN_BOOT
    TDE_BACKGROUNDCATEGORY_CHECK_ARGB(enSrc1Category, enSrc2Category, enDstFmt);

    TDE_BACKGROUNDCATEGORY_CHECK_CLUT(enSrc1Category, enSrc2Category, opt->resize, opt->deflicker_mode,
                                      opt->alpha_blending_cmd, enDstCategory);

    TDE_BACKGROUNDCATEGORY_CHECK_AN(enSrc1Category, enSrc2Category, enDstCategory, opt->alpha_blending_cmd);

    if ((hi_u32)opt->alpha_blending_cmd & HI_TDE_ALPHA_BLENDING_ROP) {
        TDE_CHECK_ROPCODE(opt->rop_alpha);
        TDE_CHECK_ROPCODE(opt->rop_color);
    }

    if ((hi_u32)opt->alpha_blending_cmd & HI_TDE_ALPHA_BLENDING_BLEND) {
        TDE_CHECK_BLENDCMD(opt->blend_opt.blend_cmd);
        if (opt->blend_opt.blend_cmd == HI_TDE_BLEND_CMD_CONFIG) {
            TDE_CHECK_BLENDMODE(opt->blend_opt.src1_blend_mode);
            TDE_CHECK_BLENDMODE(opt->blend_opt.src2_blend_mode);
        }
    }
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

/*****************************************************************************
* Function:      TdeOsiDoubleSrcGetOptCode
* Description:   get dual source operate encode
* Input:         enSrc1Fmt background pixel format
                 enSrc2Fmt foreground pixel format
                 enDstFmt: target pixel format
* Output:        none
* Return:        code value
* Others:        none
*****************************************************************************/
STATIC hi_u16 TdeOsiDoubleSrcGetOptCode(hi_tde_color_fmt enSrc1Fmt, hi_tde_color_fmt enSrc2Fmt,
                                        hi_tde_color_fmt enDstFmt)
{
    hi_u16 u16Code1 = 0x0;
    hi_u16 u16Code2 = 0x0;
    TDE_COLORFMT_TRANSFORM_E enColorTransType;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    enColorTransType = TdeOsiGetFmtTransType(enSrc2Fmt, enSrc1Fmt);
    switch (enColorTransType) {
        case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
            u16Code1 = 0x0;
            break;

        case TDE_COLORFMT_TRANSFORM_ARGB_YCbCr:
            u16Code1 = 0x5;
            break;

        case TDE_COLORFMT_TRANSFORM_CLUT_ARGB:
            u16Code1 = 0x8;
            break;
        case TDE_COLORFMT_TRANSFORM_CLUT_YCbCr:
            u16Code1 = 0x8 | 0x10 | 0x1;
            break;
        case TDE_COLORFMT_TRANSFORM_YCbCr_ARGB:
            u16Code1 = 0x1;
            break;

        case TDE_COLORFMT_TRANSFORM_YCbCr_YCbCr:
            u16Code1 = 0x0;
            break;

        default:
            u16Code1 = 0x8000;
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return u16Code1;
    }

    enColorTransType = TdeOsiGetFmtTransType(enSrc1Fmt, enDstFmt);
    switch (enColorTransType) {
        case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
            u16Code2 = 0x0;
            break;

        case TDE_COLORFMT_TRANSFORM_ARGB_YCbCr:
            u16Code2 = 0x2;
            break;

        case TDE_COLORFMT_TRANSFORM_CLUT_ARGB:
            u16Code2 = 0x8;
            break;

        case TDE_COLORFMT_TRANSFORM_CLUT_YCbCr:
            u16Code2 = 0xa;
            break;

        case TDE_COLORFMT_TRANSFORM_YCbCr_ARGB:
            u16Code2 = 0x2 | 0x4;
            if (enSrc2Fmt >= HI_TDE_COLOR_FMT_CLUT1 && enSrc2Fmt <= HI_TDE_COLOR_FMT_ACLUT88) {
                u16Code2 = 0;
            }
            break;

        case TDE_COLORFMT_TRANSFORM_YCbCr_YCbCr:
            u16Code2 = 0x0;
            if (enSrc2Fmt >= HI_TDE_COLOR_FMT_CLUT1 && enSrc2Fmt <= HI_TDE_COLOR_FMT_ACLUT88) {
                u16Code2 = 0x2;
            }
            break;

        default:
            u16Code2 = 0x8000;
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return u16Code2;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return (u16Code1 | u16Code2);
}

/*****************************************************************************
* Function:      TdeOsiSingleSrc2GetOptCode
* Description:   get dual source operate encode
* Input:         enSrc2Fmt foreground pixel format
                 enDstFmt  target pixel format
* Output:        none
* Return:        encode value, expression is fllowinf:

|------0-----|------1-----|------2-------|------3-----|
|-----ICSC---|-----OCSC---|(in)RGB->YCbCr|-----CLUT---|

* Others:       none
*****************************************************************************/
STATIC hi_u16 TdeOsiSingleSrc2GetOptCode(hi_tde_color_fmt enSrc2Fmt, hi_tde_color_fmt enDstFmt)
{
    hi_u16 u16Code = 0;
    TDE_COLORFMT_TRANSFORM_E enColorTransType;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    enColorTransType = TdeOsiGetFmtTransType(enSrc2Fmt, enDstFmt);
    switch (enColorTransType) {
        case TDE_COLORFMT_TRANSFORM_ARGB_ARGB:
            u16Code = 0x0;
            break;

        case TDE_COLORFMT_TRANSFORM_ARGB_YCbCr:
            u16Code = 0x5;
            break;

        case TDE_COLORFMT_TRANSFORM_CLUT_ARGB:
            u16Code = 0x8;
            break;

        case TDE_COLORFMT_TRANSFORM_CLUT_CLUT:
            u16Code = 0x0;
            break;

        case TDE_COLORFMT_TRANSFORM_CLUT_YCbCr:
            u16Code = 0xA;
            break;

        case TDE_COLORFMT_TRANSFORM_YCbCr_ARGB:
            u16Code = 0x1;
            break;

        case TDE_COLORFMT_TRANSFORM_YCbCr_YCbCr:
            u16Code = 0x0;
            break;

        default:
            u16Code = 0x8000;
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return u16Code;
}

/*****************************************************************************
 * Function:      TdeOsiGetConvbyCode
 * Description:   get format conversion manner by format conversion code
 * Input:         u16Code  format conversion code
 *                pstConv  format conversion struct
 * Output:        none
 * Return:        encode value
 * Others:        none
 *****************************************************************************/
STATIC hi_void TdeOsiGetConvbyCode(hi_u16 u16Code, tde_conv_mode_cmd *pstConv)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (pstConv != HI_NULL) {
        pstConv->in_conv = u16Code & 0x1;
        pstConv->out_conv = (u16Code >> 1) & 0x1;
        pstConv->in_rgb2yuv = ((u16Code >> 2) & 0x1);
        pstConv->in_src1_conv = ((u16Code >> 4) & 0x1);
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

STATIC hi_s32 TdeOsiGetbppByFmt16(hi_tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case HI_TDE_COLOR_FMT_RGB444:
        case HI_TDE_COLOR_FMT_BGR444:
        case HI_TDE_COLOR_FMT_RGB555:
        case HI_TDE_COLOR_FMT_BGR555:
        case HI_TDE_COLOR_FMT_RGB565:
        case HI_TDE_COLOR_FMT_BGR565:
        case HI_TDE_COLOR_FMT_ARGB4444:
        case HI_TDE_COLOR_FMT_ABGR4444:
        case HI_TDE_COLOR_FMT_RGBA4444:
        case HI_TDE_COLOR_FMT_BGRA4444:
        case HI_TDE_COLOR_FMT_ARGB1555:
        case HI_TDE_COLOR_FMT_ABGR1555:
        case HI_TDE_COLOR_FMT_RGBA1555:
        case HI_TDE_COLOR_FMT_BGRA1555:
        case HI_TDE_COLOR_FMT_ACLUT88:
        case HI_TDE_COLOR_FMT_YCbCr422:
        case HI_TDE_COLOR_FMT_halfword:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 16;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return -1;
    }
}

STATIC hi_s32 TdeOsiGetbppByFmt24(hi_tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case HI_TDE_COLOR_FMT_RGB888:
        case HI_TDE_COLOR_FMT_BGR888:
        case HI_TDE_COLOR_FMT_ARGB8565:
        case HI_TDE_COLOR_FMT_ABGR8565:
        case HI_TDE_COLOR_FMT_RGBA8565:
        case HI_TDE_COLOR_FMT_BGRA8565:
        case HI_TDE_COLOR_FMT_YCbCr888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 24;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return -1;
    }
}

STATIC hi_s32 TdeOsiGetbppByFmt32(hi_tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case HI_TDE_COLOR_FMT_ARGB8888:
        case HI_TDE_COLOR_FMT_ABGR8888:
        case HI_TDE_COLOR_FMT_RGBA8888:
        case HI_TDE_COLOR_FMT_BGRA8888:
        case HI_TDE_COLOR_FMT_AYCbCr8888:
        case HI_TDE_COLOR_FMT_RABG8888:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 32;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return -1;
    }
}

#ifndef HI_BUILD_IN_BOOT
STATIC hi_s32 TdeOsiGetbppByFmt8(hi_tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case HI_TDE_COLOR_FMT_CLUT8:
        case HI_TDE_COLOR_FMT_ACLUT44:
        case HI_TDE_COLOR_FMT_A8:
        case HI_TDE_COLOR_FMT_byte:
        case HI_TDE_COLOR_FMT_JPG_YCbCr420MBP:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 8;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return -1;
    }
}

STATIC hi_s32 TdeOsiGetbppByFmt4(hi_tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case HI_TDE_COLOR_FMT_CLUT4:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 4;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return -1;
    }
}

STATIC hi_s32 TdeOsiGetbppByFmt2(hi_tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case HI_TDE_COLOR_FMT_CLUT2:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 2;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return -1;
    }
}

STATIC hi_s32 TdeOsiGetbppByFmt1(hi_tde_color_fmt color_fmt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    switch (color_fmt) {
        case HI_TDE_COLOR_FMT_CLUT1:
        case HI_TDE_COLOR_FMT_A1:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 1;
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return -1;
    }
}
#endif

/*****************************************************************************
 * Function:      TdeOsiGetbppByFmt
 * Description:   get pixel bit of pixel format
 * Input:         color_fmt  target pixel format
 * Output:        none
 * Return:        -1 fail; other:pixel bit
 * Others:        none
 *****************************************************************************/
STATIC hi_s32 TdeOsiGetbppByFmt(hi_tde_color_fmt color_fmt)
{
    hi_s32 s32Ret = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (16 == (s32Ret = TdeOsiGetbppByFmt16(color_fmt))) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 16;
    } else if (24 == (s32Ret = TdeOsiGetbppByFmt24(color_fmt))) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 24;
    } else if (32 == (s32Ret = TdeOsiGetbppByFmt32(color_fmt))) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 32;
    }
#ifndef HI_BUILD_IN_BOOT
    else if (1 == (s32Ret = TdeOsiGetbppByFmt1(color_fmt))) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 1;
    } else if (2 == (s32Ret = TdeOsiGetbppByFmt2(color_fmt))) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 2;
    } else if (4 == (s32Ret = TdeOsiGetbppByFmt4(color_fmt))) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 4;
    } else if (8 == (s32Ret = TdeOsiGetbppByFmt8(color_fmt))) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 8;
    }
#endif
    else {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return -1;
    }
}

/*****************************************************************************
* Function:      TdeOsiGetScanInfo
* Description:   get scanning direction, avoid lap
* Input:         pSrc source bitmap
                 dst_surface target bitmap
                 mirror mirror type
* Output:        pstSrcDirection source scanning information
                 pstDstDirection target scanning information
* Return:        0  success
                 -1 fail
*****************************************************************************/
STATIC hi_s32 TdeOsiGetScanInfo(hi_tde_surface *pSrc, hi_tde_rect *src_rect, hi_tde_surface *dst_surface,
                                hi_tde_rect *dst_rect, hi_tde_opt *opt, tde_scandirection_mode *pstSrcDirection,
                                tde_scandirection_mode *pstDstDirection)
{
    hi_u32 u32SrcAddr = 0;
    hi_u32 u32DstAddr = 0;
    hi_tde_mirror_mode mirror = HI_TDE_MIRROR_NONE;
    hi_s32 s32SrcdBpp = 0, s32DstBpp = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pSrc, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(src_rect, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_surface, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_rect, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstSrcDirection, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstDstDirection, HI_FAILURE);

    /* when writing, subbyte format align ask for byte align of start point */
    TDE_CHECK_SUBBYTE_STARTX(dst_rect->pos_x, dst_rect->width, dst_surface->color_fmt);

    /* default scanning direction */
    pstSrcDirection->hor_scan = TDE_SCAN_LEFT_RIGHT;
    pstSrcDirection->ver_scan = TDE_SCAN_UP_DOWN;
    pstDstDirection->hor_scan = TDE_SCAN_LEFT_RIGHT;
    pstDstDirection->ver_scan = TDE_SCAN_UP_DOWN;

    if (opt != HI_NULL) {
        mirror = opt->mirror;
    }

    if (HI_TDE_MIRROR_NONE != mirror) {
        TDE_CHANGE_DIR_BY_MIRROR(pstSrcDirection, mirror);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 0;
    }

    if (opt == HI_NULL) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 0;
    }

    /* * if open clip and open resize, should not support opposite direction.
     ** 如果打开了clip和缩放，不支持反向功能
     * */
    if ((opt->clip_mode != HI_TDE_CLIP_MODE_NONE) && (HI_TRUE == opt->resize)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 0;
    }

    /* only if stride is the same, can be do conversion */
    if ((pSrc->stride != dst_surface->stride) || (pSrc->color_fmt >= HI_TDE_COLOR_FMT_JPG_YCbCr400MBP)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 0;
    }

    s32SrcdBpp = TdeOsiGetbppByFmt(pSrc->color_fmt);
    s32DstBpp = TdeOsiGetbppByFmt(dst_surface->color_fmt);

    if ((s32DstBpp <= 0) || (s32SrcdBpp <= 0)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return 0;
    }

    u32SrcAddr = pSrc->phy_addr + src_rect->pos_y * pSrc->stride +
                 (((hi_u32)src_rect->pos_x * (hi_u32)s32SrcdBpp) >> 3);

    u32DstAddr = dst_surface->phy_addr + dst_rect->pos_y * dst_surface->stride +
                 (((hi_u32)dst_rect->pos_x * (hi_u32)s32DstBpp) >> 3);

    /* source is above of target or on the left of the same direction */
    if (u32SrcAddr <= u32DstAddr) {
        pstSrcDirection->ver_scan = TDE_SCAN_DOWN_UP;
        pstDstDirection->ver_scan = TDE_SCAN_DOWN_UP;

        pstSrcDirection->hor_scan = TDE_SCAN_RIGHT_LEFT;
        pstDstDirection->hor_scan = TDE_SCAN_RIGHT_LEFT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return 0;
}

#ifndef HI_BUILD_IN_BOOT
/*****************************************************************************
* Function:      TdeOsiGetInterRect
* Description:   get inter rect of two rectangles
* Input:         pRect1  input rectangle1
                 pRect2  input rectangle2
* Output:        pInterRect output inter rectangle
* Return:        0  have inter zone
                 -1 no inter zone
* Others:        none
*****************************************************************************/
STATIC hi_s32 TdeOsiGetInterRect(hi_tde_rect *pDstRect, hi_tde_rect *pClipRect, hi_tde_rect *pInterRect)
{
    hi_s32 s32Left = 0, s32Top = 0, s32Right = 0, s32Bottom = 0;
    hi_s32 s32Right1 = 0, s32Bottom1 = 0, s32Right2 = 0, s32Bottom2 = 0;
    hi_s32 pos_x = 0, pos_y = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pDstRect, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pClipRect, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pInterRect, HI_FAILURE);

    pos_x = pDstRect->pos_x;
    pos_y = pDstRect->pos_y;
    s32Left = (pos_x > pClipRect->pos_x) ? pos_x : pClipRect->pos_x;
    s32Top = (pos_y > pClipRect->pos_y) ? pos_y : pClipRect->pos_y;

    s32Right1 = pos_x + pDstRect->width - 1;
    s32Right2 = pClipRect->pos_x + pClipRect->width - 1;
    s32Right = (s32Right1 > s32Right2) ? s32Right2 : s32Right1;

    s32Bottom1 = pos_y + pDstRect->height - 1;
    s32Bottom2 = pClipRect->pos_y + pClipRect->height - 1;
    s32Bottom = (s32Bottom1 > s32Bottom2) ? s32Bottom2 : s32Bottom1;

    if ((s32Left > s32Right) || (s32Top > s32Bottom)) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s32Left);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s32Right);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s32Top);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, s32Bottom);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "TdeOsiGetInterRect failure");
        return -1;
    }

    pInterRect->pos_x = s32Left;
    pInterRect->pos_y = s32Top;
    pInterRect->width = s32Right - s32Left + 1;
    pInterRect->height = s32Bottom - s32Top + 1;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return 0;
}

/*****************************************************************************
* Function:      TdeOsiIsRect1InRect2
* Description:   Rect1 is if inside Rect2
* Input:         pRect1  input rectangle1
                 pRect2  input rectangle2
* Output:        none
* Return:        1  have inter zone
                 0  no inter zone
* Others:        none
*****************************************************************************/
STATIC INLINE hi_bool TdeOsiIsRect1InRect2(hi_tde_rect *pRect1, hi_tde_rect *pRect2)
{
    hi_s32 s32Right1 = 0, s32Bottom1 = 0, s32Right2 = 0, s32Bottom2 = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((pRect1 != HI_NULL) && (pRect2 != HI_NULL)) {
        s32Right1 = pRect1->pos_x + pRect1->width - 1;
        s32Right2 = pRect2->pos_x + pRect2->width - 1;

        s32Bottom1 = pRect1->pos_y + pRect1->height - 1;
        s32Bottom2 = pRect2->pos_y + pRect2->height - 1;

        if ((pRect1->pos_x >= pRect2->pos_x) && (pRect1->pos_y >= pRect2->pos_y) && (s32Right1 <= s32Right2) &&
            (s32Bottom1 <= s32Bottom2)) {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_TRUE;
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_FALSE;
}
#endif

/*****************************************************************************
 * Function:      TdeOsiSetMbPara
 * Description:   MB operate setting parameter interface
 * Input:         handle: task handle
 *                hardware_node: config node
 *                mb_surface:    brightness block information struct
 *                mb_rect: MB bitmap operate zone
 *                dst_surface:  target bitmap information struct
 *                dst_rect: target bitmap operate zone
 *                pMbOpt:  operate parameter setting struct
 *                enPicMode: picture top filed/ bottom filed/frame
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
STATIC INLINE hi_s32 TdeOsiSetMbPara(hi_s32 handle, hi_tde_mb_surface *mb_surface, hi_tde_rect *mb_rect,
                                     hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_mb_opt *pMbOpt)
{
    hi_tde_surface fore_ground_surface = {0};
    hi_tde_rect fore_ground_rect = {0};
    hi_tde_color_fmt color_fmt;
    hi_tde_opt option = {0};

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    fore_ground_surface.phy_addr = mb_surface->y_addr;
    fore_ground_surface.stride = mb_surface->y_stride;
    fore_ground_surface.width = mb_surface->y_width;
    color_fmt = TdeOsiCovertMbFmt(mb_surface->mb_color_fmt);
    fore_ground_surface.color_fmt = color_fmt;
    fore_ground_surface.height = mb_surface->y_height;
    fore_ground_surface.cbcr_phy_addr = mb_surface->cbcr_phy_addr;
    fore_ground_surface.cbcr_stride = mb_surface->cbcr_stride;

    memcpy(&fore_ground_rect, mb_rect, sizeof(hi_tde_rect));

    option.deflicker_mode = (pMbOpt->is_deflicker) ? HI_TDE_DEFLICKER_MODE_BOTH : HI_TDE_DEFLICKER_MODE_NONE;
    option.out_alpha_from = (pMbOpt->is_set_out_alpha) ? HI_TDE_OUT_ALPHA_FROM_GLOBALALPHA : HI_TDE_OUT_ALPHA_FROM_NORM;
    option.resize = (HI_TDE_MB_RESIZE_NONE == pMbOpt->resize_en) ? HI_FALSE : HI_TRUE;
    option.clip_rect = pMbOpt->clip_rect;
    option.global_alpha = pMbOpt->out_alpha;
    option.clip_mode = pMbOpt->clip_mode;
    TdeOsiSingleSrc2Blit(handle, &fore_ground_surface, &fore_ground_rect, dst_surface, dst_rect, &option, HI_FALSE,
                         HI_FALSE);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

#define MAX2(a, b) (((a) < (b)) ? (b) : (a))
#define MIN2(a, b) (((a) < (b)) ? (a) : (b))

STATIC hi_u32 TdeTrueValueToComplement(hi_s32 data_in, hi_u32 bit)
{
    hi_u32 data_out = 0;
    hi_u32 data_in_tmp = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (data_in >> (bit - 1)) {
        data_in_tmp = 0 - data_in;
        data_out = ((1 << (bit - 1)) | (((~data_in_tmp) & ((1 << (bit - 1)) - 1)) + 1)) & ((1 << bit) - 1);
    } else {
        data_out = data_in;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return data_out;
}

STATIC hi_s32 TdeComplementToTrueValue(hi_u32 data_in, hi_u32 bit)
{
    hi_s32 data_out = 0;
    hi_u32 data_in_tmp = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (data_in >> (bit - 1)) {
        data_in_tmp = data_in & ((1 << (bit - 1)) - 1);
        data_out = (((~data_in_tmp) & ((1 << (bit - 1)) - 1)) + 1) * (-1);
    } else {
        data_out = data_in;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return data_out;
}

STATIC hi_void tde_osi_init_slice_data_src1(tde_slice_data *slice_data, tde_hardware_node *child_node)
{
    slice_data->sx_xfpos = child_node->src1_cbmstpos.bits.s1_xfpos;
    slice_data->srcx_hlmsc_en = child_node->src1_hsp.bits.src1_hlmsc_en;
    slice_data->srcx_hchmsc_en = child_node->src1_hsp.bits.src1_hchmsc_en;
    slice_data->srcx_ow = child_node->src1_zme_out_reso.bits.src1_ow + 1;
    slice_data->srcx_hpzme_en = child_node->src1_hpzme.bits.src1_hpzme_en;
    slice_data->srcx_hpzme_mode = child_node->src1_hpzme.bits.src1_hpzme_mode;
    slice_data->srcx_hpzme_width = child_node->src1_hpzme.bits.src1_hpzme_width + 1;
    slice_data->srcx_width = child_node->src1_image_size.bits.src1_width + 1;
    slice_data->srcx_en = child_node->src1_ctrl.bits.src1_en;
    slice_data->srcx_hratio = child_node->src1_hsp.bits.src1_hratio;
    slice_data->srcx_hor_loffset = child_node->src1_hloffset.bits.src1_hor_loffset;

    slice_data->srcx_xdpos = (slice_data->srcx_hlmsc_en | slice_data->srcx_hchmsc_en)
                                    ? (slice_data->sx_xfpos + slice_data->srcx_ow - 1)
                                    : (slice_data->srcx_hpzme_en
                                        ? (slice_data->sx_xfpos + slice_data->srcx_hpzme_width - 1)
                                        : (slice_data->sx_xfpos + slice_data->srcx_width - 1));

    slice_data->srcx_fmt = child_node->src1_ctrl.bits.src1_fmt;
    slice_data->srcx_hor_coffset = child_node->src1_hcoffset.bits.src1_hor_coffset;
    slice_data->srcx_iw = child_node->src1_zme_in_reso.bits.src1_iw + 1;
    slice_data->srcx_ow = child_node->src1_zme_out_reso.bits.src1_ow + 1;
    slice_data->srcx_422v_pro = child_node->src1_ctrl.bits.src1_422v_pro;
    slice_data->srcx_h_scan_ord = child_node->src1_ctrl.bits.src1_h_scan_ord;

    child_node->src1_ctrl.bits.src1_en = ((slice_data->cbm_mode == 1) ||
                                          (!((slice_data->sx_xfpos > slice_data->dst_xed_pos_cord_crop_in) ||
                                             (slice_data->srcx_xdpos < slice_data->dst_xst_pos_cord_crop_in)))) &&
                                         slice_data->srcx_en;
    slice_data->srcx_xst_pos_blk = MAX2(slice_data->dst_xst_pos_cord_crop_in, slice_data->sx_xfpos);
    slice_data->srcx_xed_pos_blk = MIN2(slice_data->dst_xed_pos_cord_crop_in, slice_data->srcx_xdpos);
    child_node->src1_cbmstpos.bits.s1_xfpos = slice_data->srcx_xst_pos_blk - slice_data->dst_xst_pos_cord_crop_in;
}

STATIC hi_void tde_osi_init_slice_data_src2(tde_slice_data *slice_data, tde_hardware_node *child_node)
{
    slice_data->sx_xfpos = child_node->src2_cbmstpos.bits.s2_xfpos;
    slice_data->srcx_hlmsc_en = child_node->src2_hsp.bits.src2_hlmsc_en;
    slice_data->srcx_hchmsc_en = child_node->src2_hsp.bits.src2_hchmsc_en;
    slice_data->srcx_ow = child_node->src2_zme_out_reso.bits.src2_ow + 1;
    slice_data->srcx_hpzme_en = child_node->src2_hpzme.bits.src2_hpzme_en;
    slice_data->srcx_hpzme_mode = child_node->src2_hpzme.bits.src2_hpzme_mode;
    slice_data->srcx_hpzme_width = child_node->src2_hpzme.bits.src2_hpzme_width + 1;
    slice_data->srcx_width = child_node->src2_image_size.bits.src2_width + 1;
    slice_data->srcx_en = child_node->src2_ctrl.bits.src2_en;
    slice_data->srcx_hratio = child_node->src2_hsp.bits.src2_hratio;
    slice_data->srcx_hor_loffset = child_node->src2_hloffset.bits.src2_hor_loffset;

    slice_data->srcx_xdpos = (slice_data->srcx_hlmsc_en | slice_data->srcx_hchmsc_en)
                                    ? (slice_data->sx_xfpos + slice_data->srcx_ow - 1)
                                    : (slice_data->srcx_hpzme_en
                                        ? (slice_data->sx_xfpos + slice_data->srcx_hpzme_width - 1)
                                        : (slice_data->sx_xfpos + slice_data->srcx_width - 1));

    slice_data->srcx_fmt = child_node->src2_ctrl.bits.src2_fmt;
    slice_data->srcx_hor_coffset = child_node->src2_hcoffset.bits.src2_hor_coffset;
    slice_data->srcx_iw = child_node->src2_zme_in_reso.bits.src2_iw + 1;
    slice_data->srcx_ow = child_node->src2_zme_out_reso.bits.src2_ow + 1;
    slice_data->srcx_422v_pro = child_node->src2_ctrl.bits.src2_422v_pro;
    slice_data->srcx_h_scan_ord = child_node->src2_ctrl.bits.src2_h_scan_ord;

    child_node->src2_ctrl.bits.src2_en = ((slice_data->cbm_mode == 1) ||
                                          (!((slice_data->sx_xfpos > slice_data->dst_xed_pos_cord_crop_in) ||
                                             (slice_data->srcx_xdpos < slice_data->dst_xst_pos_cord_crop_in)))) &&
                                         slice_data->srcx_en;
    slice_data->srcx_xst_pos_blk = MAX2(slice_data->dst_xst_pos_cord_crop_in, slice_data->sx_xfpos);
    slice_data->srcx_xed_pos_blk = MIN2(slice_data->dst_xed_pos_cord_crop_in, slice_data->srcx_xdpos);
    child_node->src2_cbmstpos.bits.s2_xfpos = slice_data->srcx_xst_pos_blk - slice_data->dst_xst_pos_cord_crop_in;
}

STATIC hi_void tde_osi_init_slice_data(tde_slice_data *slice_data, tde_hardware_node *child_node, hi_u32 i)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (i == 0) {
        tde_osi_init_slice_data_src1(slice_data, child_node);
    } else {
        tde_osi_init_slice_data_src2(slice_data, child_node);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

STATIC hi_void tde_osi_calc_slice_s1(tde_slice_data *slice_data)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    slice_data->srcx_xst_pos_cord = slice_data->srcx_xst_pos_blk - slice_data->sx_xfpos;
    slice_data->srcx_xed_pos_cord = slice_data->srcx_xed_pos_blk - slice_data->sx_xfpos;

    slice_data->srcx_xst_pos_cord_in = ((hi_u64)slice_data->srcx_xst_pos_cord * slice_data->srcx_hratio) >>
                                       TDE_OSI_20_BIT_SHIFT;
    slice_data->srcx_xed_pos_cord_in = ((hi_u64)slice_data->srcx_xed_pos_cord * slice_data->srcx_hratio) >>
                                       TDE_OSI_20_BIT_SHIFT;

    slice_data->srcx_hor_loffset_cfg_int_comp = slice_data->srcx_hor_loffset >> TDE_OSI_20_BIT_SHIFT;
    /* algorithm: complement to true value 8 bits */
    slice_data->srcx_hor_loffset_cfg_int = TdeComplementToTrueValue(slice_data->srcx_hor_loffset_cfg_int_comp, 8);

    slice_data->srcx_hor_loffset_cfg_fraction = slice_data->srcx_hor_loffset & 0xfffff;
    slice_data->srcx_hor_loffset_pix_fraction = ((hi_u64)slice_data->srcx_xst_pos_cord * slice_data->srcx_hratio) &
                                                0xfffff;
    slice_data->srcx_hor_loffset_fraction =
        (slice_data->srcx_hor_loffset_cfg_fraction + slice_data->srcx_hor_loffset_pix_fraction) & 0xfffff;

    slice_data->srcx_xst_pos_cord_in_offset =
        slice_data->srcx_xst_pos_cord_in + slice_data->srcx_hor_loffset_cfg_int +
        (((slice_data->srcx_hor_loffset_cfg_fraction + slice_data->srcx_hor_loffset_pix_fraction) & 0xfff00000) != 0);
    slice_data->srcx_xed_pos_cord_in_offset =
        slice_data->srcx_xed_pos_cord_in + slice_data->srcx_hor_loffset_cfg_int +
        (((slice_data->srcx_hor_loffset_cfg_fraction + slice_data->srcx_hor_loffset_pix_fraction) & 0xfff00000) != 0);

    /* algorithm: divid 2 for even */
    slice_data->srcx_xst_pos_cord_in_tap_rgb =
        (slice_data->srcx_xst_pos_cord_in_offset < 0)
            ? 0
            : ((slice_data->srcx_xst_pos_cord_in_offset >= ((TDE_MAX_ZOOM_OUT_STEP / 2) - 1))
                    ? (slice_data->srcx_xst_pos_cord_in_offset - ((TDE_MAX_ZOOM_OUT_STEP / 2) - 1))
                    : 0);
    /* algorithm: divid 2 for even */
    slice_data->srcx_xed_pos_cord_in_tap_rgb =
        (slice_data->srcx_xed_pos_cord_in_offset + TDE_MAX_ZOOM_OUT_STEP / 2) < 0
            ? 0
            : (((slice_data->srcx_xed_pos_cord_in_offset + TDE_MAX_ZOOM_OUT_STEP / 2) >= slice_data->srcx_iw - 1)
                    ? slice_data->srcx_iw - 1
                    : slice_data->srcx_xed_pos_cord_in_offset + TDE_MAX_ZOOM_OUT_STEP / 2);

    slice_data->node_cfg_srcx_iw_rgb = slice_data->srcx_xed_pos_cord_in_tap_rgb -
                                       slice_data->srcx_xst_pos_cord_in_tap_rgb + 1;

    slice_data->srcx_hor_loffset_int =
        (slice_data->srcx_xst_pos_cord_in_offset - slice_data->srcx_xst_pos_cord_in_tap_rgb);
    /* algorithm: true value to complement 8 bits */
    slice_data->srcx_hor_loffset_int_complement = TdeTrueValueToComplement(slice_data->srcx_hor_loffset_int, 8);

    slice_data->node_cfg_srcx_hor_loffset_rgb = (slice_data->srcx_hor_loffset_int_complement << TDE_OSI_20_BIT_SHIFT) +
                                                slice_data->srcx_hor_loffset_fraction;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

STATIC hi_void tde_osi_calc_slice_sp(tde_slice_data *slice_data)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    /* algorithm: divid 2 for even */
    slice_data->srcx_xst_pos_cord_in_tap_luma = ((slice_data->srcx_xst_pos_cord_in_tap_rgb % 2) == 1)
                                                    ? slice_data->srcx_xst_pos_cord_in_tap_rgb - 1
                                                    : slice_data->srcx_xst_pos_cord_in_tap_rgb;
    /* algorithm: divid 2 for even */
    slice_data->srcx_xed_pos_cord_in_tap_luma =
        (((slice_data->srcx_xed_pos_cord_in_tap_rgb % 2) == 0)
                ? slice_data->srcx_xed_pos_cord_in_tap_rgb + 1
                : slice_data->srcx_xed_pos_cord_in_tap_rgb) > (slice_data->srcx_iw - 1)
            ? (slice_data->srcx_iw - 1)
            : (((slice_data->srcx_xed_pos_cord_in_tap_rgb % 2) == 0) ? slice_data->srcx_xed_pos_cord_in_tap_rgb + 1
                                                                        : slice_data->srcx_xed_pos_cord_in_tap_rgb);
    /* algorithm: divid 2 for even */
    slice_data->srcx_xst_pos_cord_in_chroma =
        (slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV)
            ? ((hi_u64)slice_data->srcx_xst_pos_cord * (slice_data->srcx_hratio)) >> TDE_OSI_20_BIT_SHIFT
            : ((hi_u64)slice_data->srcx_xst_pos_cord * (slice_data->srcx_hratio / 2)) >> TDE_OSI_20_BIT_SHIFT;
    /* algorithm: divid 2 for even */
    slice_data->srcx_xed_pos_cord_in_chroma =
        (slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV)
            ? ((hi_u64)slice_data->srcx_xed_pos_cord * (slice_data->srcx_hratio)) >> TDE_OSI_20_BIT_SHIFT
            : ((hi_u64)slice_data->srcx_xed_pos_cord * (slice_data->srcx_hratio / 2)) >> TDE_OSI_20_BIT_SHIFT;
    /* algorithm: complement to true value 8 bits */
    slice_data->srcx_hor_coffset_cfg_int_comp = slice_data->srcx_hor_coffset >> TDE_OSI_20_BIT_SHIFT;
    slice_data->srcx_hor_coffset_cfg_int = TdeComplementToTrueValue(slice_data->srcx_hor_coffset_cfg_int_comp, 8);

    slice_data->srcx_hor_coffset_cfg_fraction = slice_data->srcx_hor_coffset & 0xfffff;
    slice_data->srcx_hor_coffset_pix_fraction =
        ((slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV)
                ? ((hi_u64)slice_data->srcx_xst_pos_cord * (slice_data->srcx_hratio))
                : ((hi_u64)slice_data->srcx_xst_pos_cord * (slice_data->srcx_hratio / 2))) &
        0xfffff;
    slice_data->srcx_hor_coffset_fraction =
        (slice_data->srcx_hor_coffset_cfg_fraction + slice_data->srcx_hor_coffset_pix_fraction) & 0xfffff;

    slice_data->srcx_xst_pos_cord_in_offset_chroma =
        slice_data->srcx_xst_pos_cord_in_chroma + slice_data->srcx_hor_coffset_cfg_int +
        (((slice_data->srcx_hor_coffset_cfg_fraction + slice_data->srcx_hor_coffset_pix_fraction) & 0xfff00000) != 0);
    slice_data->srcx_xed_pos_cord_in_offset_chroma =
        slice_data->srcx_xed_pos_cord_in_chroma + slice_data->srcx_hor_coffset_cfg_int +
        (((slice_data->srcx_hor_coffset_cfg_fraction + slice_data->srcx_hor_coffset_pix_fraction) & 0xfff00000) != 0);
    /* algorithm: divid 2 for even */
    slice_data->srcx_xst_pos_cord_in_tap_chroma =
        (slice_data->srcx_xst_pos_cord_in_offset_chroma < 0)
            ? 0
            : ((slice_data->srcx_xst_pos_cord_in_offset_chroma >= ((TDE_MAX_ZOOM_OUT_STEP / 2) - 1))
                    ? (slice_data->srcx_xst_pos_cord_in_offset_chroma - ((TDE_MAX_ZOOM_OUT_STEP / 2) - 1))
                    : 0);
    /* algorithm: divid 2 for even */
    slice_data->srcx_xed_pos_cord_in_tap_chroma =
        (slice_data->srcx_xed_pos_cord_in_offset_chroma + TDE_MAX_ZOOM_OUT_STEP / 2) < 0
            ? 0
            : (((slice_data->srcx_xed_pos_cord_in_offset_chroma + TDE_MAX_ZOOM_OUT_STEP / 2) >=
                ((slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV) ? ((slice_data->srcx_iw - 1))
                                                                            : ((slice_data->srcx_iw - 1) / 2)))
                    ? ((slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV) ? ((slice_data->srcx_iw - 1))
                                                                                : ((slice_data->srcx_iw - 1) / 2))
                    : (slice_data->srcx_xed_pos_cord_in_offset_chroma + TDE_MAX_ZOOM_OUT_STEP / 2));
    /* algorithm: multi 2 */
    slice_data->srcx_xst_pos_cord_in_tap_chroma_x2 = (slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV)
                                                            ? slice_data->srcx_xst_pos_cord_in_tap_chroma
                                                            : slice_data->srcx_xst_pos_cord_in_tap_chroma * 2;
    slice_data->srcx_xed_pos_cord_in_tap_chroma_x2 = (slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV)
                                                            ? slice_data->srcx_xed_pos_cord_in_tap_chroma
                                                            : slice_data->srcx_xed_pos_cord_in_tap_chroma * 2 + 1;

    slice_data->srcx_xst_pos_cord_in_tap_sp = MIN2(slice_data->srcx_xst_pos_cord_in_tap_luma,
                                                   slice_data->srcx_xst_pos_cord_in_tap_chroma_x2);
    slice_data->srcx_xed_pos_cord_in_tap_sp = MAX2(slice_data->srcx_xed_pos_cord_in_tap_luma,
                                                   slice_data->srcx_xed_pos_cord_in_tap_chroma_x2);

    slice_data->node_cfg_srcx_iw_sp = slice_data->srcx_xed_pos_cord_in_tap_sp -
                                      slice_data->srcx_xst_pos_cord_in_tap_sp + 1;

    slice_data->srcx_hor_loffset_int_sp = slice_data->srcx_xst_pos_cord_in_offset -
                                          slice_data->srcx_xst_pos_cord_in_tap_sp;
    /* algorithm: divid 2 */
    slice_data->srcx_hor_coffset_int_sp =
        (slice_data->srcx_xst_pos_cord_in_offset_chroma - ((slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV)
                                                                ? slice_data->srcx_xst_pos_cord_in_tap_sp
                                                                : slice_data->srcx_xst_pos_cord_in_tap_sp / 2));
    /* algorithm: true value to complement 8 bits */
    slice_data->srcx_hor_loffset_int_sp_complent = TdeTrueValueToComplement(slice_data->srcx_hor_loffset_int_sp, 8);
    /* algorithm: true value to complement 8 bits */
    slice_data->srcx_hor_coffset_int_sp_complent = TdeTrueValueToComplement(slice_data->srcx_hor_coffset_int_sp, 8);

    slice_data->node_cfg_srcx_hor_loffset_sp = (slice_data->srcx_hor_loffset_int_sp_complent << TDE_OSI_20_BIT_SHIFT) +
                                               slice_data->srcx_hor_loffset_fraction;
    slice_data->node_cfg_srcx_hor_coffset_sp = (slice_data->srcx_hor_coffset_int_sp_complent << TDE_OSI_20_BIT_SHIFT) +
                                               slice_data->srcx_hor_coffset_fraction;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

STATIC hi_void tde_osi_calc_slice_mux(tde_slice_data *slice_data, tde_hardware_node *child_node, int i)
{
    hi_u32 srcx_ow;
    hi_u32 srcx_en;
    hi_u32 srcx_iw;
    hi_u32 srcx_hor_loffset;
    hi_u32 srcx_hor_coffset;
    hi_u32 srcx_hpzme_width;
    hi_u32 srcx_width;
    hi_u32 srcx_hoffset_pix;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (i == 0) {
        srcx_ow = child_node->src1_zme_out_reso.bits.src1_ow;
        srcx_en = child_node->src1_ctrl.bits.src1_en;
        srcx_iw = child_node->src1_zme_in_reso.bits.src1_iw;
        srcx_hor_loffset = child_node->src1_hloffset.bits.src1_hor_loffset;
        srcx_hor_coffset = child_node->src1_hcoffset.bits.src1_hor_coffset;
        srcx_hpzme_width = child_node->src1_hpzme.bits.src1_hpzme_width;
        srcx_width = child_node->src1_image_size.bits.src1_width;
        srcx_hoffset_pix = child_node->src1_pix_offset.bits.src1_hoffset_pix;
    } else {
        srcx_ow = child_node->src2_zme_out_reso.bits.src2_ow;
        srcx_en = child_node->src2_ctrl.bits.src2_en;
        srcx_iw = child_node->src2_zme_in_reso.bits.src2_iw;
        srcx_hor_loffset = child_node->src2_hloffset.bits.src2_hor_loffset;
        srcx_hor_coffset = child_node->src2_hcoffset.bits.src2_hor_coffset;
        srcx_hpzme_width = child_node->src2_hpzme.bits.src2_hpzme_width;
        srcx_width = child_node->src2_image_size.bits.src2_width;
        srcx_hoffset_pix = child_node->src2_pix_offset.bits.src2_hoffset_pix;
    }

    srcx_ow = slice_data->srcx_en ? (slice_data->srcx_xed_pos_cord - slice_data->srcx_xst_pos_cord + 1) : 1;
    srcx_iw = slice_data->srcx_en
                    ? (((slice_data->srcx_fmt >= TDE_DRV_COLOR_FMT_PKGYUYV) &&
                      (slice_data->srcx_fmt != TDE_DRV_COLOR_FMT_YCbCr400MBP) &&
                      (!((slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV) && !slice_data->srcx_422v_pro)) &&
                      (slice_data->srcx_fmt != TDE_DRV_COLOR_FMT_YCbCr444MB))
                            ? ((slice_data->srcx_hlmsc_en || slice_data->srcx_hchmsc_en) ? slice_data->node_cfg_srcx_iw_sp
                                                                                        : srcx_ow)
                            : ((slice_data->srcx_hlmsc_en || slice_data->srcx_hchmsc_en) ? slice_data->node_cfg_srcx_iw_rgb
                                                                                        : srcx_ow))
                    : 1;
    srcx_hor_loffset =
        slice_data->srcx_en
            ? (((slice_data->srcx_fmt >= TDE_DRV_COLOR_FMT_PKGYUYV) &&
                (slice_data->srcx_fmt != TDE_DRV_COLOR_FMT_YCbCr400MBP) &&
                (!((slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV) && !slice_data->srcx_422v_pro)) &&
                (slice_data->srcx_fmt != TDE_DRV_COLOR_FMT_YCbCr444MB))
                    ? slice_data->node_cfg_srcx_hor_loffset_sp
                    : slice_data->node_cfg_srcx_hor_loffset_rgb)
            : 0;
    srcx_hor_coffset =
        slice_data->srcx_en
            ? (((slice_data->srcx_fmt >= TDE_DRV_COLOR_FMT_PKGYUYV) &&
                (slice_data->srcx_fmt != TDE_DRV_COLOR_FMT_YCbCr400MBP) &&
                (!((slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV) && !slice_data->srcx_422v_pro)) &&
                (slice_data->srcx_fmt != TDE_DRV_COLOR_FMT_YCbCr444MB))
                    ? slice_data->node_cfg_srcx_hor_coffset_sp
                    : slice_data->node_cfg_srcx_hor_loffset_rgb)
            : 0;
    srcx_hpzme_width = srcx_iw;

    slice_data->srcx_xst_pos_cord_in_tap =
        (slice_data->srcx_hlmsc_en || slice_data->srcx_hchmsc_en)
            ? (((slice_data->srcx_fmt >= TDE_DRV_COLOR_FMT_PKGYUYV) &&
                (slice_data->srcx_fmt != TDE_DRV_COLOR_FMT_YCbCr400MBP) &&
                (!((slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV) && !slice_data->srcx_422v_pro)) &&
                (slice_data->srcx_fmt != TDE_DRV_COLOR_FMT_YCbCr444MB))
                    ? slice_data->srcx_xst_pos_cord_in_tap_sp
                    : slice_data->srcx_xst_pos_cord_in_tap_rgb)
            : slice_data->srcx_xst_pos_cord;
    slice_data->srcx_xed_pos_cord_in_tap =
        (slice_data->srcx_hlmsc_en || slice_data->srcx_hchmsc_en)
            ? (((slice_data->srcx_fmt >= TDE_DRV_COLOR_FMT_PKGYUYV) &&
                (slice_data->srcx_fmt != TDE_DRV_COLOR_FMT_YCbCr400MBP) &&
                (!((slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV) && !slice_data->srcx_422v_pro)) &&
                (slice_data->srcx_fmt != TDE_DRV_COLOR_FMT_YCbCr444MB))
                    ? slice_data->srcx_xed_pos_cord_in_tap_sp
                    : slice_data->srcx_xed_pos_cord_in_tap_rgb)
            : slice_data->srcx_xed_pos_cord;

    slice_data->srcx_xst_pos_cord_in_tap_hpzme =
        slice_data->srcx_hpzme_en ? (slice_data->srcx_xst_pos_cord_in_tap * (slice_data->srcx_hpzme_mode + 1))
                                    : slice_data->srcx_xst_pos_cord_in_tap;
    slice_data->srcx_xed_pos_cord_in_tap_hpzme =
        slice_data->srcx_hpzme_en ? ((slice_data->srcx_xed_pos_cord_in_tap + 1) * (slice_data->srcx_hpzme_mode + 1) - 1)
                                    : slice_data->srcx_xed_pos_cord_in_tap;
    slice_data->srcx_xed_pos_cord_in_tap_hpzme = (slice_data->srcx_xed_pos_cord_in_tap_hpzme >
                                                  (slice_data->srcx_width - 1))
                                                        ? (slice_data->srcx_width - 1)
                                                        : slice_data->srcx_xed_pos_cord_in_tap_hpzme;

    if (slice_data->srcx_h_scan_ord != 0) {
        slice_data->srcx_xst_pos_cord_in_tap_hpzme_hso = slice_data->srcx_width - 1 -
                                                         slice_data->srcx_xed_pos_cord_in_tap_hpzme;
        slice_data->srcx_xed_pos_cord_in_tap_hpzme_hso = slice_data->srcx_width - 1 -
                                                         slice_data->srcx_xst_pos_cord_in_tap_hpzme;
    } else {
        slice_data->srcx_xst_pos_cord_in_tap_hpzme_hso = slice_data->srcx_xst_pos_cord_in_tap_hpzme;
        slice_data->srcx_xed_pos_cord_in_tap_hpzme_hso = slice_data->srcx_xed_pos_cord_in_tap_hpzme;
    }

    srcx_width = slice_data->srcx_en ? (slice_data->srcx_xed_pos_cord_in_tap_hpzme_hso -
                                        slice_data->srcx_xst_pos_cord_in_tap_hpzme_hso + 1)
                                        : 1;
    srcx_hoffset_pix = slice_data->srcx_en ? slice_data->srcx_xst_pos_cord_in_tap_hpzme_hso : 0;

    slice_data->srcx_8ali_en =
        (slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_A1 || slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_A1B ||
         slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_CLUT1 || slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_CLUT1B)
            ? 1
            : 0;
    /* algorithm: 8 ali */
    slice_data->srcx_xst_pos_cord_in_tap_hpzme_8ali = slice_data->srcx_xst_pos_cord_in_tap_hpzme_hso / 8 * 8;
    slice_data->srcx_xst_pos_cord_in_tap_8ali =
        slice_data->srcx_hpzme_en
            ? (slice_data->srcx_xst_pos_cord_in_tap_hpzme_8ali / (slice_data->srcx_hpzme_mode + 1))
            : slice_data->srcx_xst_pos_cord_in_tap_hpzme_8ali;
    /* algorithm: 8 ali */
    slice_data->srcx_xed_pos_cord_in_tap_hpzme_8ali =
        (slice_data->srcx_xed_pos_cord_in_tap_hpzme_hso == (slice_data->srcx_width - 1))
            ? (slice_data->srcx_width - 1)
            : ((slice_data->srcx_xed_pos_cord_in_tap_hpzme_hso + 8) / 8 * 8 - 1);
    slice_data->srcx_xed_pos_cord_in_tap_8ali =
        slice_data->srcx_hpzme_en
            ? ((slice_data->srcx_xed_pos_cord_in_tap_hpzme_8ali + 1) / (slice_data->srcx_hpzme_mode + 1))
            : (slice_data->srcx_xed_pos_cord_in_tap_hpzme_8ali + 1);
    slice_data->srcx_xed_pos_cord_in_tap_8ali = (slice_data->srcx_xed_pos_cord_in_tap_8ali < 1)
                                                    ? 0
                                                    : (slice_data->srcx_xed_pos_cord_in_tap_8ali - 1);

    slice_data->node_cfg_srcx_iw_8ali = slice_data->srcx_xed_pos_cord_in_tap_8ali + 1 -
                                        slice_data->srcx_xst_pos_cord_in_tap_8ali;
    slice_data->node_cfg_srcx_iw_8ali = slice_data->srcx_hpzme_en
                                            ? (((slice_data->srcx_xed_pos_cord_in_tap_hpzme_8ali + 1 -
                                                 slice_data->srcx_xst_pos_cord_in_tap_hpzme_8ali) /
                                                (slice_data->srcx_hpzme_mode + 1)) +
                                               (((slice_data->srcx_xed_pos_cord_in_tap_hpzme_8ali + 1 -
                                                  slice_data->srcx_xst_pos_cord_in_tap_hpzme_8ali) %
                                                 (slice_data->srcx_hpzme_mode + 1))
                                                    ? 1
                                                    : 0))
                                            : slice_data->node_cfg_srcx_iw_8ali;
    slice_data->srcx_hor_loffset_int_8ali =
        (slice_data->srcx_xst_pos_cord_in_offset - slice_data->srcx_xst_pos_cord_in_tap_8ali);
    /* algorithm: true value to complement 8 bits */
    slice_data->srcx_hor_loffset_int_complement_8ali = TdeTrueValueToComplement(slice_data->srcx_hor_loffset_int_8ali,
                                                                                8);
    slice_data->node_cfg_srcx_hor_loffset_8ali = (slice_data->srcx_hor_loffset_int_complement_8ali
                                                  << TDE_OSI_20_BIT_SHIFT) +
                                                 slice_data->srcx_hor_loffset_fraction;
    srcx_iw = (slice_data->srcx_en & slice_data->srcx_8ali_en) ? slice_data->node_cfg_srcx_iw_8ali : srcx_iw;
    srcx_hor_loffset = (slice_data->srcx_en & slice_data->srcx_8ali_en) ? slice_data->node_cfg_srcx_hor_loffset_8ali
                                                                        : srcx_hor_loffset;
    srcx_hor_coffset = (slice_data->srcx_en & slice_data->srcx_8ali_en) ? slice_data->node_cfg_srcx_hor_loffset_8ali
                                                                        : srcx_hor_coffset;
    srcx_hpzme_width = srcx_iw;
    srcx_width =
        (slice_data->srcx_en & slice_data->srcx_8ali_en)
            ? (slice_data->srcx_xed_pos_cord_in_tap_hpzme_8ali - slice_data->srcx_xst_pos_cord_in_tap_hpzme_8ali + 1)
            : srcx_width;
    srcx_hoffset_pix = (slice_data->srcx_en & slice_data->srcx_8ali_en)
                            ? (slice_data->srcx_h_scan_ord
                                    ? (slice_data->srcx_width - 1 - slice_data->srcx_xed_pos_cord_in_tap_hpzme_8ali)
                                    : slice_data->srcx_xst_pos_cord_in_tap_hpzme_8ali)
                            : srcx_hoffset_pix;

    slice_data->srcx_hor_loffset_int_beyond = 1;
    slice_data->srcx_hor_loffset_int_beyond_complent = TdeTrueValueToComplement(slice_data->srcx_hor_loffset_int_beyond,
                                                                                8);
    /* algorithm: 2 for check if is even */
    if ((slice_data->srcx_hpzme_en == 0) && (srcx_width % 2 == 1) &&
        ((slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr400MBP) ||
         ((slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBV) && !slice_data->srcx_422v_pro) ||
         (slice_data->srcx_fmt == TDE_DRV_COLOR_FMT_YCbCr444MB)) &&
        (slice_data->srcx_xed_pos_cord_in_tap_hpzme_hso == slice_data->srcx_width - 1) &&
        (slice_data->srcx_width % 2 == 0)) {
        if (slice_data->srcx_h_scan_ord) {
            srcx_iw = srcx_iw + 1;
            srcx_width = srcx_width + 1;
            if (srcx_hoffset_pix == 0) {
                srcx_hor_loffset = srcx_hor_loffset +
                                   (slice_data->srcx_hor_loffset_int_beyond_complent << TDE_OSI_20_BIT_SHIFT);
                srcx_hor_coffset = srcx_hor_coffset +
                                   (slice_data->srcx_hor_loffset_int_beyond_complent << TDE_OSI_20_BIT_SHIFT);
            } else {
                srcx_hoffset_pix = srcx_hoffset_pix - 1;
            }
        } else {
            srcx_iw = srcx_iw + 1;
            srcx_width = srcx_width + 1;
            srcx_hoffset_pix = srcx_hoffset_pix - 1;
            srcx_hor_loffset = srcx_hor_loffset +
                               (slice_data->srcx_hor_loffset_int_beyond_complent << TDE_OSI_20_BIT_SHIFT);
            srcx_hor_coffset = srcx_hor_coffset +
                               (slice_data->srcx_hor_loffset_int_beyond_complent << TDE_OSI_20_BIT_SHIFT);
        }
    }

    if (i == 0) {
        child_node->src1_zme_out_reso.bits.src1_ow = srcx_ow - 1;
        child_node->src1_ctrl.bits.src1_en = srcx_en;
        child_node->src1_zme_in_reso.bits.src1_iw = srcx_iw - 1;
        child_node->src1_hloffset.bits.src1_hor_loffset = srcx_hor_loffset;
        child_node->src1_hcoffset.bits.src1_hor_coffset = srcx_hor_coffset;
        child_node->src1_hpzme.bits.src1_hpzme_width = srcx_hpzme_width - 1;
        child_node->src1_image_size.bits.src1_width = srcx_width - 1;
        child_node->src1_pix_offset.bits.src1_hoffset_pix = srcx_hoffset_pix;
    } else {
        child_node->src2_zme_out_reso.bits.src2_ow = srcx_ow - 1;
        child_node->src2_ctrl.bits.src2_en = srcx_en;
        child_node->src2_zme_in_reso.bits.src2_iw = srcx_iw - 1;
        child_node->src2_hloffset.bits.src2_hor_loffset = srcx_hor_loffset;
        child_node->src2_hcoffset.bits.src2_hor_coffset = srcx_hor_coffset;
        child_node->src2_hpzme.bits.src2_hpzme_width = srcx_hpzme_width - 1;
        child_node->src2_image_size.bits.src2_width = srcx_width - 1;
        child_node->src2_pix_offset.bits.src2_hoffset_pix = srcx_hoffset_pix;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

STATIC hi_void tde_osi_final_slice_calc(tde_slice_data *slice_data, tde_hardware_node *child_node)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    child_node->dst_ctrl.bits.dst_en = slice_data->dst_crop_en
                                            ? (((child_node->dst_alpha.bits.dst_clip_mode == 0) &&
                                               ((slice_data->crop_start_x > slice_data->dst_xed_pos_blk) ||
                                                (slice_data->crop_end_x < slice_data->dst_xst_pos_blk)))
                                                    ? 0
                                                    : child_node->dst_ctrl.bits.dst_en)
                                            : child_node->dst_ctrl.bits.dst_en;
    child_node->dst_ctrl.bits.dst_en = slice_data->dst_crop_en
                                            ? (((child_node->dst_alpha.bits.dst_clip_mode == 0) &&
                                               ((slice_data->dst_crop_start_x > slice_data->dst_xed_pos_blk) ||
                                                (slice_data->dst_crop_end_x < slice_data->dst_xst_pos_blk)))
                                                    ? 0
                                                    : child_node->dst_ctrl.bits.dst_en)
                                            : child_node->dst_ctrl.bits.dst_en;

    child_node->dst_image_size.bits.dst_width = slice_data->dst_xed_pos_blk - slice_data->dst_xst_pos_blk + 1;
    child_node->dst_pix_offset.bits.dst_hoffset_pix = (slice_data->dst_h_scan_ord
                                                            ? (slice_data->dst_width - 1 - slice_data->dst_xed_pos_blk)
                                                            : slice_data->dst_xst_pos_blk) +
                                                      slice_data->dst_hoffset_pix;
    child_node->dst_alpha.bits.dst_clip_en = (!((slice_data->dst_crop_start_x > slice_data->dst_xed_pos_blk) ||
                                                (slice_data->dst_crop_end_x < slice_data->dst_xst_pos_blk))) &&
                                             child_node->dst_alpha.bits.dst_clip_en;
    child_node->dst_crop_pos_start.bits.dst_crop_start_x =
        (child_node->dst_ctrl.bits.dst_en && slice_data->dst_crop_en)
            ? (MAX2(slice_data->dst_xst_pos_blk, slice_data->dst_crop_start_x) - slice_data->dst_xst_pos_blk)
            : 0;
    child_node->dst_crop_pos_end.bits.dst_crop_end_x =
        (child_node->dst_ctrl.bits.dst_en && slice_data->dst_crop_en)
            ? (MIN2(slice_data->dst_xed_pos_blk, slice_data->dst_crop_end_x) - slice_data->dst_xst_pos_blk)
            : 0;

    child_node->dst_h_dswm.bits.dst_h_dwsm_ow = slice_data->dst_h_dwsm_ow - 1;
    child_node->dst_image_size.bits.dst_width = child_node->dst_image_size.bits.dst_width - 1;
    child_node->cbm_imgsize.bits.cbm_width = child_node->cbm_imgsize.bits.cbm_width - 1;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

STATIC hi_s32 TdeOsiCalcSlice(hi_s32 handle, tde_hardware_node *hardware_node)
{
    hi_s32 ret;
    tde_slice_data slice_data = {0};
    hi_u32 n = 0;
    hi_u32 i = 0;
    tde_hardware_node **child_nodes = HI_NULL;
    hi_void *vir_buf = HI_NULL;
    hi_u32 over_lab;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    slice_data.dst_fmt = hardware_node->dst_ctrl.bits.dst_fmt;
    slice_data.dst_width = hardware_node->dst_image_size.bits.dst_width + 1;
    slice_data.dst_h_dswm_mode = hardware_node->dst_h_dswm.bits.dst_h_dswm_mode;
    slice_data.cbm_mode = hardware_node->cbm_ctrl.bits.cbm_mode;

    /* algorithm: over lab value: 32 or 4 or 2 */
    over_lab = hardware_node->src4_ctrl.bits.src4_afbc_en ? 32
                                                            : (slice_data.dst_fmt >= TDE_DRV_COLOR_FMT_PKGYUYV &&
                                                             slice_data.dst_fmt <= TDE_DRV_COLOR_FMT_PKGYYVU)
                                                                ? 4
                                                                : 2;

    slice_data.dst_hoffset_pix = 0;
    slice_data.dst_h_scan_ord = hardware_node->dst_ctrl.bits.dst_h_scan_ord;

    slice_data.dst_crop_en = hardware_node->dst_alpha.bits.dst_clip_en;
    slice_data.dst_crop_start_x = hardware_node->dst_crop_pos_start.bits.dst_crop_start_x;
    slice_data.dst_crop_end_x = hardware_node->dst_crop_pos_end.bits.dst_crop_end_x;
    slice_data.crop_end_x = hardware_node->dst_crop_pos_end.bits.dst_crop_end_x;
    slice_data.crop_start_x = hardware_node->dst_crop_pos_start.bits.dst_crop_start_x;
    slice_data.dst_h_dswm_mode = hardware_node->dst_h_dswm.bits.dst_h_dswm_mode;

    /* dst_h_dswm_mode -> 2 for loose point mode */
    if ((slice_data.dst_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBH || slice_data.dst_fmt == TDE_DRV_COLOR_FMT_YCbCr420MB ||
         slice_data.dst_fmt == TDE_DRV_COLOR_FMT_PLANNER420 ||
         (slice_data.dst_fmt >= TDE_DRV_COLOR_FMT_PKGYUYV && slice_data.dst_fmt <= TDE_DRV_COLOR_FMT_PKGYYVU)) &&
        (slice_data.dst_h_dswm_mode == 2)) {
        /* algorithm: node num calc: multiply 2 */
        slice_data.node_num = slice_data.dst_width / (TDE_MAX_SLICE_WIDTH - over_lab * 2) +
                              ((slice_data.dst_width % (TDE_MAX_SLICE_WIDTH - over_lab * 2)) != 0);
    } else {
        slice_data.node_num = slice_data.dst_width / TDE_MAX_SLICE_WIDTH +
                              ((slice_data.dst_width % TDE_MAX_SLICE_WIDTH) != 0);
    }
#ifdef HI_BUILD_IN_BOOT
    child_nodes = (tde_hardware_node **)hi_gfx_sys_mem_malloc(slice_data.node_num * sizeof(tde_hardware_node *), "tde");
#else
    child_nodes = (tde_hardware_node **)HI_GFX_VMALLOC(HIGFX_TDE_ID, slice_data.node_num * sizeof(tde_hardware_node *));
#endif
    if (child_nodes == HI_NULL) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_malloc, FAILURE_TAG);
        return HI_FAILURE;
    }
    memset(child_nodes, 0, slice_data.node_num * sizeof(tde_hardware_node *));

    for (n = 0; n < slice_data.node_num; n++) {
        hi_u32 node_size = sizeof(tde_hardware_node) + TDE_NODE_HEAD_BYTE + TDE_NODE_TAIL_BYTE;
        vir_buf = (hi_void *)tde_malloc(node_size);
        if (vir_buf == HI_NULL) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, sizeof(tde_hardware_node));
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_malloc, FAILURE_TAG);
            wgetfreenum();
            goto ERR;
        }
        child_nodes[n] = (tde_hardware_node *)(vir_buf + TDE_NODE_HEAD_BYTE);

        memcpy(child_nodes[n], hardware_node, sizeof(tde_hardware_node));
    }

    for (n = 0; n < slice_data.node_num; n++) {
        if (n == (slice_data.node_num - 1)) {
            child_nodes[n]->tde_pnext_low.u32 = 0;
            child_nodes[n]->tde_pnext_hi.u32 = 0;
        } else {
            child_nodes[n]->tde_pnext_low.u32 = HI_GFX_GET_LOW_PART(wgetphy(child_nodes[n + 1]));
            child_nodes[n]->tde_pnext_hi.u32 = HI_GFX_GET_HIGH_PART(wgetphy(child_nodes[n + 1]));
        }

        tde_hal_set_cfg_reg_offset(child_nodes[n]);

        /* dst_h_dswm_mode -> 2 for loose point mode */
        if ((slice_data.dst_fmt == TDE_DRV_COLOR_FMT_YCbCr422MBH ||
             slice_data.dst_fmt == TDE_DRV_COLOR_FMT_YCbCr420MB || slice_data.dst_fmt == TDE_DRV_COLOR_FMT_PLANNER420 ||
             (slice_data.dst_fmt >= TDE_DRV_COLOR_FMT_PKGYUYV && slice_data.dst_fmt <= TDE_DRV_COLOR_FMT_PKGYYVU)) &&
            (slice_data.dst_h_dswm_mode == 2)) {
            /* algorithm: multiply 2 */
            slice_data.dst_xst_pos_blk = n * (TDE_MAX_SLICE_WIDTH - over_lab * 2);
            slice_data.dst_xed_pos_blk = (((n + 1) * (TDE_MAX_SLICE_WIDTH - over_lab * 2)) <= slice_data.dst_width)
                                                ? (((n + 1) * (TDE_MAX_SLICE_WIDTH - over_lab * 2)) - 1)
                                                : slice_data.dst_width - 1;

            slice_data.dst_h_dwsm_ow = (slice_data.dst_xed_pos_blk - slice_data.dst_xst_pos_blk + 1);
            child_nodes[n]->dst_h_dwsm_hloffset.bits.dst_h_dwsm_hor_loffset = (slice_data.dst_xst_pos_blk == 0)
                                                                                    ? 0
                                                                                    : over_lab;

            slice_data.dst_xst_pos_cord_crop_in = slice_data.dst_xst_pos_blk -
                                                  child_nodes[n]->dst_h_dwsm_hloffset.bits.dst_h_dwsm_hor_loffset;
            slice_data.dst_xed_pos_cord_crop_in = slice_data.dst_xed_pos_blk + over_lab;
            slice_data.dst_xed_pos_cord_crop_in = (slice_data.dst_xed_pos_cord_crop_in > (slice_data.dst_width - 1))
                                                        ? (slice_data.dst_width - 1)
                                                        : slice_data.dst_xed_pos_cord_crop_in;
        } else {
            slice_data.dst_xst_pos_blk = n * TDE_MAX_SLICE_WIDTH;
            slice_data.dst_xed_pos_blk = (((n + 1) * TDE_MAX_SLICE_WIDTH) <= slice_data.dst_width)
                                                ? (((n + 1) * TDE_MAX_SLICE_WIDTH) - 1)
                                                : slice_data.dst_width - 1;

            slice_data.dst_h_dwsm_ow = (slice_data.dst_xed_pos_blk - slice_data.dst_xst_pos_blk + 1);
            child_nodes[n]->dst_h_dwsm_hloffset.bits.dst_h_dwsm_hor_loffset = 0;

            slice_data.dst_xst_pos_cord_crop_in = slice_data.dst_xst_pos_blk -
                                                  child_nodes[n]->dst_h_dwsm_hloffset.bits.dst_h_dwsm_hor_loffset;
            slice_data.dst_xed_pos_cord_crop_in = slice_data.dst_xed_pos_blk;
        }

        child_nodes[n]->cbm_imgsize.bits.cbm_width = slice_data.dst_xed_pos_cord_crop_in -
                                                     slice_data.dst_xst_pos_cord_crop_in + 1;
        child_nodes[n]->dst_h_dwsm_hcoffset.bits.dst_h_dwsm_hor_coffset =
            child_nodes[n]->dst_h_dwsm_hloffset.bits.dst_h_dwsm_hor_loffset;

        /* algorithm: 2 for src and src2 */
        for (i = 0; i < 2; i++) {
            tde_osi_init_slice_data(&slice_data, child_nodes[n], i);

            tde_osi_calc_slice_s1(&slice_data);

            tde_osi_calc_slice_sp(&slice_data);

            tde_osi_calc_slice_mux(&slice_data, child_nodes[n], i);
        }

        tde_osi_final_slice_calc(&slice_data, child_nodes[n]);

        ret = TdeOsiSetNodeFinish(handle, child_nodes[n], 0, TDE_NODE_SUBM_ALONE);
        if (ret != HI_SUCCESS) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetNodeFinish, ret);
            goto ERR;
        }
    }

#ifdef HI_BUILD_IN_BOOT
    hi_gfx_sys_mem_free((hi_char *)child_nodes);
#else
    HI_GFX_VFREE(HIGFX_TDE_ID, child_nodes);
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
ERR:
    for (n = 0; n < slice_data.node_num; n++) {
        if (child_nodes[n] == HI_NULL) {
            break;
        }
        vir_buf = (hi_void *)child_nodes[n] - TDE_NODE_HEAD_BYTE;

        tde_free(vir_buf);
    }

#ifdef HI_BUILD_IN_BOOT
    hi_gfx_sys_mem_free((hi_char *)child_nodes);
#else
    HI_GFX_VFREE(HIGFX_TDE_ID, child_nodes);
#endif
    return HI_FAILURE;
}

/*****************************************************************************
 * Function:      TdeOsiSetFilterChildNode
 * Description:   calculate child nodes of slice and add into task list
 * Input:         handle: task list handle
 *                hardware_node: node config parameter
 *                in_rect: input bitmap zone
 *                out_rect: output bitmap zone
 *                is_deflicker: if deflicker
 *                filter_opt: filter config parameter
 * Output:        none
 * Return:        return slice number
 * Others:        none
 *****************************************************************************/
#ifndef HI_BUILD_IN_BOOT

typedef struct {
    hi_s32 ori_in_width;    // original image width
    hi_s32 ori_in_height;   // original image height
    hi_s32 zme_out_width;   // output full image width
    hi_s32 zme_out_height;  // output full image height

    hi_s32 update_instart_w;  // the start_x of update area in original image
    hi_s32 update_instart_h;  // the start_y of update area in original image
    hi_s32 update_in_width;   // the width of update area in original image
    hi_s32 update_in_height;  // the height of update area in original image
} UpdateConfig;

typedef struct {
    hi_s32 zme_instart_w;  // the start_x of needed readin area in original image
    hi_s32 zme_instart_h;  // the start_y of needed readin area in original image
    hi_s32 zme_in_width;   // the width of needed readin area in original image
    hi_s32 zme_in_height;  // the height of needed readin area in original image

    hi_s32 zme_outstart_w;  // the start_x of needed update area in output image
    hi_s32 zme_outstart_h;  // the start_y of needed update area in output image
    hi_s32 zme_out_width;   // the width of needed update area in output image
    hi_s32 zme_out_height;  // the height of needed update area in output image

    hi_s32 zme_hphase;      // the start phase of horizontal scale
    hi_s32 zme_vphase;      // the start phase of vertical scale
    hi_s32 def_offsetup;    // the up offset of deflicker
    hi_s32 def_offsetdown;  // the down offset of deflicker
} UpdateInfo;

STATIC hi_void TdeOsiGetHUpdateInfo(UpdateConfig *pstReg, UpdateInfo *pstInfo, hi_bool bScaler)
{
    hi_s32 s32ZmeHinstart = 0, s32ZmeHinstop = 0;
    hi_s32 s32ZmeHoutstart = 0, s32ZmeHoutstop = 0;
    hi_s32 s32UpdateHstart = pstReg->update_instart_w;
    hi_s32 s32UpdateHstop = s32UpdateHstart + pstReg->update_in_width - 1;
    hi_s32 s32ZmeHphase = 0;
    hi_s32 s32Ratio = 0;
    hi_s32 s32Dratio = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((pstReg->zme_out_width <= 1) || (pstReg->ori_in_width <= 1)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }
    s32Ratio = (hi_s32)(4096 * (pstReg->ori_in_width - 1) / (pstReg->zme_out_width - 1) + 1 / 2);
    s32Dratio = 4096 * (pstReg->zme_out_width - 1) / (pstReg->ori_in_width - 1);

    if (bScaler != HI_TRUE) { /* hor_scaler not enable */
        pstInfo->zme_instart_w = pstReg->update_instart_w;
        pstInfo->zme_outstart_w = pstReg->update_instart_w;
        pstInfo->zme_in_width = pstReg->update_in_width;
        pstInfo->zme_out_width = pstReg->update_in_width;
        pstInfo->zme_hphase = 0;
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    // hor_scaler enable
    if (s32UpdateHstart >= 0 && s32UpdateHstart < 3) {
        /* update outstretched area exceed left limit bordline */
        s32ZmeHinstart = 0;
        s32ZmeHoutstart = 0;
        s32ZmeHphase = 0;
    } else {
        /* update outstretched area didn't exceed the left limit bordline */
        s32ZmeHinstart = (s32UpdateHstart - 3) * s32Dratio;
        s32ZmeHoutstart = (s32ZmeHinstart % 4096) == 0 ? (s32ZmeHinstart >> 12) : ((s32ZmeHinstart >> 12) + 1);
        s32ZmeHinstart = s32ZmeHoutstart * s32Ratio >> 12;
        if (s32ZmeHinstart - 2 < 0) {
            /* the left few point need mirror pixels when scale */
            s32ZmeHphase = (s32ZmeHoutstart * s32Ratio) % 4096 + s32ZmeHinstart * 4096;
            s32ZmeHinstart = 0;
        } else {
            /* the left few point not need mirror pixels when scale */
            s32ZmeHphase = (s32ZmeHoutstart * s32Ratio) % 4096 + 3 * 4096;
            s32ZmeHinstart = s32ZmeHinstart - 2;
        }
    }

    if (s32UpdateHstop > (pstReg->ori_in_width - 3) && s32UpdateHstop < pstReg->ori_in_width) {
        /* update outstretched area exceed the right limit bordline */
        s32ZmeHinstop = pstReg->ori_in_width - 1;
        s32ZmeHoutstop = pstReg->zme_out_width - 1;
    } else {
        /* update outstretched area didn't exceed the right limit bordline */
        s32ZmeHinstop = (s32UpdateHstop + 2 + 1) * s32Dratio;
        s32ZmeHoutstop = (s32ZmeHinstop % 4096) == 0 ? ((s32ZmeHinstop >> 12) - 1) : (s32ZmeHinstop >> 12);
        s32ZmeHinstop = s32ZmeHoutstop * s32Ratio >> 12;
        if (s32ZmeHinstop + 3 > (pstReg->ori_in_width - 1)) {
            /* the right few point need mirror pixels when scale */
            s32ZmeHinstop = pstReg->ori_in_width - 1;
        } else {
            /* the right few point need mirror pixels when scale */
            s32ZmeHinstop = s32ZmeHinstop + 3;
        }
    }

    pstInfo->zme_instart_w = s32ZmeHinstart;
    pstInfo->zme_outstart_w = s32ZmeHoutstart;
    pstInfo->zme_in_width = s32ZmeHinstop - s32ZmeHinstart + 1;
    pstInfo->zme_out_width = s32ZmeHoutstop - s32ZmeHoutstart + 1;
    pstInfo->zme_hphase = s32ZmeHphase;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

STATIC void TdeOsiGetDeflickerEnVUpdateInfo(UpdateConfig *reg, UpdateInfo *info, hi_s32 update_vstart,
                                            hi_s32 update_vstop)
{
    hi_s32 zme_vinstart = 0, zme_vinstop = 0;
    hi_s32 zme_voutstart = 0, zme_voutstop = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(reg);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);

    /* ver_scale not enable & deflicker enable */
    if (update_vstart < 2) {
        /* the update outstreatched area exceed the up limit bordline */
        zme_vinstart = 0;
        zme_voutstart = 0;
        info->def_offsetup = 0;
    } else {
        /* the update outstreatched area didn't exceed the up limit bordline */
        zme_vinstart = update_vstart - 2;
        zme_voutstart = reg->update_instart_h - 1;
        info->def_offsetup = 1;
    }

    if (update_vstop > (reg->ori_in_height - 3)) {
        /* the update outstreatched area exceed the down limit bordline */
        zme_vinstop = reg->ori_in_height - 1;
        zme_voutstop = reg->ori_in_height - 1;
        info->def_offsetdown = 0;
    } else {
        /* the update outstreatched area didn't exceed the down limit bordline */
        zme_vinstop = update_vstop + 2;
        zme_voutstop = zme_vinstop - 1;
        info->def_offsetdown = 1;
    }

    info->zme_in_height = zme_vinstop - zme_vinstart + 1;
    info->zme_instart_h = zme_vinstart;
    info->zme_outstart_h = zme_voutstart;
    info->zme_out_height = zme_voutstop - zme_voutstart + 1;
    info->zme_vphase = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

STATIC void TdeOsiGetBothEnVUpdateInfo(UpdateConfig *reg, UpdateInfo *info, hi_s32 update_vstart, hi_s32 update_vstop,
                                       hi_s32 ratio, hi_s32 dratio)
{
    hi_s32 zme_vinstart = 0, zme_vinstop = 0;
    hi_s32 zme_voutstart = 0, zme_voutstop = 0;
    hi_s32 zme_vphase = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(reg);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);

    /* ver_scale enable & deflicker enable */
    if (update_vstart >= 0 && update_vstart <= 2) {
        /* the update outstreatched area exceed the up limit bordline */
        zme_vinstart = 0;
        zme_voutstart = 0;
        zme_vphase = 0;
        info->def_offsetup = 0;
    } else {
        /* the update outstreatched area didn't exceed the up limit bordline */
        zme_vinstart = (update_vstart - 2) * dratio;
        zme_voutstart = ((zme_vinstart % 4096) == 0 ? (zme_vinstart >> 12) : ((zme_vinstart >> 12) + 1)) - 1;
        if (zme_voutstart <= 1) {
            /* the update outstreatched deflicker area exceed the up limit bordline */
            zme_vinstart = 0;
            zme_vphase = 0;
            info->def_offsetup = (zme_voutstart == 0) ? 0 : 1;
        } else {
            /* the update outstreatched deflicker area didn't exceed the up limit bordline */
            zme_vinstart = (zme_voutstart - 1) * ratio >> 12;
            if (zme_vinstart < 2) {
                /* the up few point need mirror pixels when scale */
                zme_vphase = ((zme_voutstart - 1) * ratio) % 4096 + zme_vinstart * 4096;
                zme_vinstart = 0;
            } else {
                /* the up few point not need mirror pixels when scale */
                zme_vphase = ((zme_voutstart - 1) * ratio) % 4096 + 2 * 4096;
                zme_vinstart = zme_vinstart - 2;
            }
            info->def_offsetup = 1;
        }
    }

    if (update_vstop > (reg->ori_in_height - 3) && update_vstop < reg->ori_in_height) {
        /* the update outstreatched area exceed the down limit bordline */
        zme_vinstop = reg->ori_in_height - 1;
        zme_voutstop = reg->zme_out_height - 1;
        info->def_offsetdown = 0;
    } else {
        /* the update outstreatched area didn't exceed the down limit bordline */
        zme_vinstop = (update_vstop + 2 + 1) * dratio;
        zme_voutstop = ((zme_vinstop % 4096) == 0 ? (zme_vinstop >> 12) : (zme_vinstop >> 12)) + 2;
        if (zme_voutstop > (reg->zme_out_height - 3)) {
            /* the update outstreatched deflicker area exceed the down limit bordline */
            zme_vinstop = reg->ori_in_height - 1;
            info->def_offsetdown = (zme_voutstop >= (reg->zme_out_height - 1)) ? 0 : 1;
        } else {
            zme_vinstop = (zme_voutstop + 1) * ratio >> 12;
            zme_vinstop = (zme_vinstop > (reg->ori_in_height - 3)) ? (reg->ori_in_height - 1) : (zme_vinstop + 2);
            info->def_offsetdown = 1;
        }

        if (zme_voutstop >= reg->zme_out_height) {
            zme_voutstop = reg->zme_out_height - 1;
        }
    }

    info->zme_instart_h = zme_vinstart;
    info->zme_outstart_h = zme_voutstart;
    info->zme_in_height = zme_vinstop - zme_vinstart + 1;
    info->zme_out_height = zme_voutstop - zme_voutstart + 1;
    info->zme_vphase = zme_vphase;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

STATIC void TdeOsiGetScalerEnVUpdateInfo(UpdateConfig *reg, UpdateInfo *info, hi_s32 update_vstart, hi_s32 update_vstop,
                                         hi_s32 ratio, hi_s32 dratio)
{
    hi_s32 zme_vinstart = 0, zme_vinstop = 0;
    hi_s32 zme_voutstart = 0, zme_voutstop = 0;
    hi_s32 zme_vphase = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(reg);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);

    /* ver_scale enable & deflicker not enable */
    if (update_vstart >= 0 && update_vstart < 2) {
        /* the update outstreatched area exceed the up limit bordline */
        zme_vinstart = 0;
        zme_voutstart = 0;
        zme_vphase = 0;
    } else {
        /* the update outstreatched area didn't exceed the up limit bordline */
        zme_vinstart = (update_vstart - 2) * dratio;
        zme_voutstart = (zme_vinstart % 4096) == 0 ? (zme_vinstart >> 12) : ((zme_vinstart >> 12) + 1);
        zme_vinstart = zme_voutstart * ratio >> 12;
        if (zme_vinstart - 2 < 0) {
            /* the up few point need mirror pixels when scale */
            zme_vphase = (zme_voutstart * ratio) % 4096 + zme_vinstart * 4096;
            zme_vinstart = 0;
        } else {
            /* the up few point not need mirror pixels when scale */
            zme_vphase = (zme_voutstart * ratio) % 4096 + 2 * 4096;
            zme_vinstart = zme_vinstart - 2;
        }
    }

    if (update_vstop > (reg->ori_in_height - 3) && update_vstop < reg->ori_in_height) {
        /* the update outstreatched area exceed the down limit bordline */
        zme_vinstop = reg->ori_in_height - 1;
        zme_voutstop = reg->zme_out_height - 1;
    } else {
        /* the update outstreatched area didn't exceed the down limit bordline */
        zme_vinstop = (update_vstop + 2 + 1) * dratio;
        zme_voutstop = (zme_vinstop % 4096) == 0 ? (zme_vinstop >> 12) : (zme_vinstop >> 12) + 1;
        zme_vinstop = zme_voutstop * ratio >> 12;
        if (zme_vinstop + 2 > (reg->ori_in_height - 1)) {
            /* the down few point need mirror pixels when scale */
            zme_vinstop = reg->ori_in_height - 1;
        } else {
            /* the down few point not need mirror pixels when scale */
            zme_vinstop = zme_vinstop + 2;
        }
    }

    info->zme_in_height = zme_vinstop - zme_vinstart + 1;
    info->zme_instart_h = zme_vinstart;
    info->zme_outstart_h = zme_voutstart;
    info->zme_out_height = zme_voutstop - zme_voutstart + 1;
    info->zme_vphase = zme_vphase;
    info->def_offsetup = 0;
    info->def_offsetdown = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return;
}

STATIC hi_void TdeOsiGetVUpdateInfo(UpdateConfig *pstReg, UpdateInfo *pstInfo, int bScaler, int is_deflicker)
{
    hi_s32 s32UpdateVstart = pstReg->update_instart_h;
    hi_s32 s32UpdateVstop = s32UpdateVstart + pstReg->update_in_height - 1;
    hi_s32 s32Ratio = 0;
    hi_s32 s32Dratio = 0;
    hi_bool bDeflickerEn = HI_FALSE;
    hi_bool bScalerEn = HI_FALSE;
    hi_bool bBothDeflickerAndScalerEn = HI_FALSE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstReg);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstInfo);

    if ((pstReg->zme_out_height <= 0) || (pstReg->ori_in_height <= 0)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }
    s32Ratio = (hi_s32)(4096 * (pstReg->ori_in_height) / (pstReg->zme_out_height));
    s32Dratio = 4096 * (pstReg->zme_out_height) / (pstReg->ori_in_height);

    bDeflickerEn = (bScaler == 0 && is_deflicker == 1);
    bScalerEn = (bScaler == 1 && is_deflicker == 0);
    bBothDeflickerAndScalerEn = (bScaler == 1 && is_deflicker == 1);

    if (bDeflickerEn) {
        TdeOsiGetDeflickerEnVUpdateInfo(pstReg, pstInfo, s32UpdateVstart, s32UpdateVstop);
    } else if (bScalerEn) {
        TdeOsiGetScalerEnVUpdateInfo(pstReg, pstInfo, s32UpdateVstart, s32UpdateVstop, s32Ratio, s32Dratio);
    } else if (bBothDeflickerAndScalerEn) {
        TdeOsiGetBothEnVUpdateInfo(pstReg, pstInfo, s32UpdateVstart, s32UpdateVstop, s32Ratio, s32Dratio);
    } else {
        pstInfo->zme_instart_h = pstReg->update_instart_h;
        pstInfo->zme_in_height = pstReg->update_in_height;
        pstInfo->zme_outstart_h = pstReg->update_instart_h;
        pstInfo->zme_out_height = pstReg->update_in_height;
        pstInfo->zme_vphase = 0;
        pstInfo->def_offsetup = 0;
        pstInfo->def_offsetdown = 0;
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }
}
#endif

/*****************************************************************************
 * Function:      TdeOsiSetFilterNode
 * Description:   do deflicker(include VF/HF/FF)
 * Input:         handle:Job head node pointer of needing operate
 *                hardware_node: set parent node fliter parameter information
 *                pInSurface: input bitmap information
 *                pOutSurface: output bitmap information
 *                is_deflicker: if deflicker
 *                opt: config option of filter operate needings
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
STATIC hi_s32 TdeOsiSetFilterNode(hi_s32 handle, tde_hardware_node *hardware_node, hi_tde_surface *pstBackGround,
                                  hi_tde_rect *back_ground_rect, hi_tde_surface *fore_ground_surface,
                                  hi_tde_rect *fore_ground_rect, hi_tde_surface *dst_surface, hi_tde_rect *dst_rect,
                                  hi_tde_deflicker_mode deflicker_mode, hi_tde_filter_mode filter_mode)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_tde_color_fmt enSrc2Fmt;
    hi_tde_color_fmt out_color_fmt;
    hi_bool is_deflicker = HI_FALSE;
#ifndef HI_BUILD_IN_BOOT
    hi_tde_color_fmt enSrc1Fmt;
    UpdateConfig reg;
    UpdateInfo info;
    hi_tde_rect stUpdateInRect;
    hi_tde_rect stUpdateOutRect;
    hi_bool bScale = HI_FALSE;
    hi_bool bBackGroundOperation = ((HI_NULL != pstBackGround) && (HI_NULL != back_ground_rect));
#endif
    hi_bool bForeGroundOperation = ((HI_NULL != fore_ground_surface) && (HI_NULL != fore_ground_rect));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    out_color_fmt = dst_surface->color_fmt;
    GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(out_color_fmt, HI_TDE_COLOR_FMT_MAX, HI_ERR_TDE_INVALID_PARA);
    is_deflicker = (HI_TDE_DEFLICKER_MODE_NONE == deflicker_mode) ? HI_FALSE : HI_TRUE;

#ifndef HI_BUILD_IN_BOOT
    if (bBackGroundOperation) {
        enSrc1Fmt = pstBackGround->color_fmt;
        tde_hal_calc_src1_filter_opt(hardware_node, enSrc1Fmt, out_color_fmt, back_ground_rect, dst_rect, filter_mode);
    }
#endif

    if (bForeGroundOperation) {
        enSrc2Fmt = fore_ground_surface->color_fmt;
        GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(enSrc2Fmt, HI_TDE_COLOR_FMT_MAX, HI_ERR_TDE_INVALID_PARA);
        tde_hal_calc_src2_filter_opt(hardware_node, enSrc2Fmt, out_color_fmt, fore_ground_rect, dst_rect, is_deflicker,
                                     filter_mode);
#ifndef HI_BUILD_IN_BOOT
        if (((fore_ground_surface->width != fore_ground_rect->width) ||
             (fore_ground_surface->height != fore_ground_rect->height)) &&
            s_bRegionDeflicker) {
            if (bBackGroundOperation) {
                GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstBackGround);
                GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, back_ground_rect);
                return HI_ERR_TDE_UNSUPPORTED_OPERATION;
            }
            reg.ori_in_height = fore_ground_surface->height;
            reg.ori_in_width = fore_ground_surface->width;
            reg.zme_out_height = dst_surface->height;
            reg.zme_out_width = dst_surface->width;
            reg.update_instart_w = fore_ground_rect->pos_x;
            reg.update_instart_h = fore_ground_rect->pos_y;
            reg.update_in_width = fore_ground_rect->width;
            reg.update_in_height = fore_ground_rect->height;

            if (TDE_NO_SCALE_HSTEP != hardware_node->src2_hsp.bits.src2_hratio) {
                bScale = HI_TRUE;
            }

            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, bScale);
            TdeOsiGetHUpdateInfo(&reg, &info, bScale);

            bScale = HI_FALSE;
            if (TDE_NO_SCALE_VSTEP != hardware_node->src2_vsr.bits.src2_vratio) {
                bScale = HI_TRUE;
            }
            GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_XINT, bScale);
            TdeOsiGetVUpdateInfo(&reg, &info, bScale, is_deflicker);

            hardware_node->src2_image_size.bits.src2_width = info.zme_in_width - 1;
            hardware_node->src2_zme_in_reso.bits.src2_iw = info.zme_in_width - 1;

            hardware_node->src2_image_size.bits.src2_height = info.zme_in_height - 1;
            hardware_node->src2_zme_in_reso.bits.src2_ih = info.zme_in_height - 1;

            dst_rect->pos_x = info.zme_outstart_w;
            dst_rect->pos_y = info.zme_outstart_h;
            dst_rect->width = info.zme_out_width;
            dst_rect->height = info.zme_out_height;

            hardware_node->src2_hcoffset.u32 = info.zme_hphase;
            hardware_node->src2_voffset.bits.src2_vluma_offset = info.zme_vphase;

            stUpdateInRect.pos_x = info.zme_instart_w;
            stUpdateInRect.pos_y = info.zme_instart_h;
            stUpdateInRect.width = info.zme_in_width;
            stUpdateInRect.height = info.zme_in_height;

            stUpdateOutRect.pos_x = info.zme_outstart_w;
            stUpdateOutRect.pos_y = info.zme_outstart_h;
            stUpdateOutRect.width = info.zme_out_width;
            stUpdateOutRect.height = info.zme_out_height;

            tde_hal_calc_src2_filter_opt(hardware_node, enSrc2Fmt, out_color_fmt, &stUpdateInRect, &stUpdateOutRect,
                                         is_deflicker, filter_mode);

            hardware_node->src2_ch0_addr_l.bits.src2_ch0_addr_l =
                fore_ground_surface->phy_addr +
                stUpdateInRect.pos_y * hardware_node->src2_ch0_stride.bits.src2_ch0_stride +
                ((stUpdateInRect.pos_x * TdeOsiGetbppByFmt(enSrc2Fmt)) / 8);

            hardware_node->dst_ch0_addr_l.bits.dst_ch0_addr_l =
                dst_surface->phy_addr + stUpdateOutRect.pos_y * hardware_node->dst_ch0_stride.bits.dst_ch0_stride +
                ((stUpdateOutRect.pos_x * TdeOsiGetbppByFmt(out_color_fmt)) / 8);

            if (out_color_fmt == HI_TDE_COLOR_FMT_JPG_YCbCr420MBP) {
                hardware_node->dst_ch0_addr_l.bits.dst_ch0_addr_l =
                    dst_surface->phy_addr + stUpdateOutRect.pos_y * hardware_node->dst_ch0_stride.bits.dst_ch0_stride +
                    (((stUpdateOutRect.pos_x / 2 * 2) * TdeOsiGetbppByFmt(out_color_fmt)) / 8);
                hardware_node->dst_ch1_addr_l.bits.dst_ch1_addr_l =
                    dst_surface->cbcr_phy_addr + stUpdateOutRect.pos_y / 2 * dst_surface->cbcr_stride +
                    (stUpdateOutRect.pos_x / 2 * 2);
            }
        }
#endif
    }

    Ret = TdeOsiCalcSlice(handle, hardware_node);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return Ret;
}

/*****************************************************************************
 * Function:      TdeOsiAdjPara4YCbCr422R
 * Description:   when fill color is YCbCr422R, fill by word
 * Input:         dst_surface: target bitmap info struct
 *                dst_rect: target operate zone
 *                fill_color: fill color
 *
 * Output:        none
 * Return:        none
 *****************************************************************************/
STATIC INLINE hi_void TdeOsiAdjPara4YCbCr422R(hi_tde_surface *dst_surface, hi_tde_rect *dst_rect,
                                              hi_tde_fill_color *fill_color)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (HI_TDE_COLOR_FMT_YCbCr422 != dst_surface->color_fmt) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return;
    }

    dst_surface->color_fmt = HI_TDE_COLOR_FMT_AYCbCr8888;
    dst_surface->alpha_max_is_255 = HI_TRUE;
    fill_color->color_fmt = HI_TDE_COLOR_FMT_AYCbCr8888;
    dst_surface->width /= 2;
    dst_rect->width /= 2;
    dst_rect->pos_x /= 2;

    fill_color->color_value = TDE_GET_YC422R_FILLVALUE(fill_color->color_value);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
}

/*****************************************************************************
 * Function:      TdeOsi1SourceFill
 * Description:   single source fill operate,source1 is fill color,target bitmap is dst_surface,support source1 and fill
 *color do ROP or alpha blending to target bitmap, unsupport mirror,colorkey src bitmap is not support MB color format
 * Input:         handle: task handle
 *                pSrc: background bitmap info struct
 *                dst_surface: foreground bitmap info struct
 *                fill_color:  target bitmap info struct
 *                opt: operate parameter setting struct
 * Output:        none
 * Return:        HI_SUCCESS/HI_FAILURE
 * Others:        none
 *****************************************************************************/
STATIC hi_s32 TdeOsi1SourceFill(hi_s32 handle, hi_tde_surface *dst_surface, hi_tde_rect *dst_rect,
                                hi_tde_fill_color *fill_color, hi_tde_opt *opt)
{
    tde_base_opt_mode enBaseMode = {0};
    tde_alu_mode enAluMode = TDE_ALU_NONE;
    tde_hardware_node *hardware_node = HI_NULL;
    tde_surface_msg stDrvSurface = {0};
    tde_color_fill stDrvColorFill = {0};
    tde_scandirection_mode stScanInfo = {0};
    hi_tde_out_alpha_from out_alpha_from = HI_TDE_OUT_ALPHA_FROM_NORM;
    hi_tde_rect tmp_dst_rect = {0};
    hi_s32 s32Ret = HI_SUCCESS;
    hi_bool bContainNullPtr = ((HI_NULL == dst_surface) || (HI_NULL == dst_rect) || (HI_NULL == fill_color));
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (bContainNullPtr) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fill_color);
        return HI_ERR_TDE_NULL_PTR;
    }
    memcpy(&tmp_dst_rect, dst_rect, sizeof(hi_tde_rect));

    TDE_CHECK_NOT_MB(dst_surface->color_fmt);
    TDE_CHECK_NOT_MB(fill_color->color_fmt);

    if (TdeOsiCheckSurface(dst_surface, &tmp_dst_rect) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    TDE_CHECK_SUBBYTE_STARTX(tmp_dst_rect.pos_x, tmp_dst_rect.width, dst_surface->color_fmt);

    TdeOsiAdjPara4YCbCr422R(dst_surface, &tmp_dst_rect, fill_color);

    if (tde_hal_init_node(&hardware_node) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_init_node, FAILURE_TAG);
        return HI_ERR_TDE_NO_MEM;
    }

    if (HI_NULL == opt) {
        enBaseMode = (fill_color->color_fmt == dst_surface->color_fmt) ? TDE_QUIKE_FILL : TDE_NORM_FILL_1OPT;
        out_alpha_from = HI_TDE_OUT_ALPHA_FROM_NORM;
    }
#ifndef HI_BUILD_IN_BOOT
    else {
        hi_bool bOutAlphaFromError = HI_FALSE;
        if (HI_TDE_ALPHA_BLENDING_MAX <= opt->alpha_blending_cmd) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HI_TDE_ALPHA_BLENDING_MAX);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, opt->alpha_blending_cmd);
            tde_hal_free_node_buf(hardware_node);
            return HI_ERR_TDE_INVALID_PARA;
        }

        if (HI_TDE_ALPHA_BLENDING_NONE != opt->alpha_blending_cmd) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HI_TDE_ALPHA_BLENDING_NONE);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, opt->alpha_blending_cmd);
            tde_hal_free_node_buf(hardware_node);
            return HI_ERR_TDE_INVALID_PARA;
        }

        bOutAlphaFromError = HI_FALSE;
        enBaseMode = (fill_color->color_fmt == dst_surface->color_fmt) ? TDE_QUIKE_FILL : TDE_NORM_FILL_1OPT;

        out_alpha_from = opt->out_alpha_from;
        bOutAlphaFromError = (HI_TDE_OUT_ALPHA_FROM_MAX <= out_alpha_from) ||
                             (HI_TDE_OUT_ALPHA_FROM_FOREGROUND == out_alpha_from);

        if (bOutAlphaFromError) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HI_TDE_OUT_ALPHA_FROM_MAX);
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HI_TDE_OUT_ALPHA_FROM_FOREGROUND);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, out_alpha_from);
            tde_hal_free_node_buf(hardware_node);
            return HI_ERR_TDE_INVALID_PARA;
        }

        tde_hal_node_set_global_alpha(hardware_node, opt->global_alpha, opt->blend_opt.global_alpha_en);

        if (TdeOsiSetClipPara(HI_NULL, HI_NULL, dst_surface, &tmp_dst_rect, dst_surface, &tmp_dst_rect, opt,
                              hardware_node) < 0) {
            tde_hal_free_node_buf(hardware_node);
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetClipPara, FAILURE_TAG);
            return HI_ERR_TDE_CLIP_AREA;
        }
    }
#endif
#ifndef HI_BUILD_IN_BOOT
    if (TDE_NORM_FILL_1OPT == enBaseMode) {
        if (TdeOsiColorConvert(fill_color, dst_surface, &stDrvColorFill.fill_data) < 0) {
            tde_hal_free_node_buf(hardware_node);
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiColorConvert, FAILURE_TAG);
            return HI_ERR_TDE_INVALID_PARA;
        }

        stDrvColorFill.color_fmt = TDE_DRV_COLOR_FMT_ARGB8888;
    } else
#endif
    {
        stDrvColorFill.fill_data = fill_color->color_value;
        GRAPHIC_CHECK_ARRAY_OVER_RETURN_VALUE(fill_color->color_fmt, HI_TDE_COLOR_FMT_MAX + 1, HI_FAILURE);
        if (fill_color->color_fmt >= HI_TDE_COLOR_FMT_MAX) {
            tde_hal_free_node_buf(hardware_node);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, fill_color->color_fmt);
            return HI_ERR_TDE_INVALID_PARA;
        }
        stDrvColorFill.color_fmt = g_enTdeCommonDrvColorFmt[fill_color->color_fmt];
    }

    if (tde_hal_node_set_base_operate(hardware_node, enBaseMode, enAluMode, &stDrvColorFill) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_base_operate, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    stScanInfo.hor_scan = TDE_SCAN_LEFT_RIGHT;
    stScanInfo.ver_scan = TDE_SCAN_UP_DOWN;

    TdeOsiConvertSurface(dst_surface, &tmp_dst_rect, &stScanInfo, &stDrvSurface, HI_NULL);

#ifndef HI_BUILD_IN_BOOT
    if (TDE_NORM_FILL_1OPT == enBaseMode) {
        tde_hal_set_src1(hardware_node, &stDrvSurface);
    }
#endif

    tde_hal_node_set_tqt(hardware_node, &stDrvSurface, out_alpha_from);

    TdeOsiSetExtAlpha(dst_surface, HI_NULL, hardware_node);

    if ((s32Ret = TdeOsiSetNodeFinish(handle, hardware_node, 0, TDE_NODE_SUBM_ALONE)) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetNodeFinish, FAILURE_TAG);
        return s32Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

#ifndef HI_BUILD_IN_BOOT
STATIC hi_s32 TdeOsiSetForegroundColorKey(tde_hardware_node *hardware_node, hi_tde_surface *src_surface,
                                          hi_tde_opt *opt, TDE_CLUT_USAGE_E enClutUsage)
{
    tde_color_key_cmd stColorkey;
    tde_color_fmt_category enFmtCategory;
    hi_bool bForegroundColorkeyAfterClut = HI_FALSE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    stColorkey.color_key_value = opt->color_key_value;
    bForegroundColorkeyAfterClut = (TDE_CLUT_COLOREXPENDING != enClutUsage && TDE_CLUT_CLUT_BYPASS != enClutUsage);

    if (HI_TDE_COLOR_KEY_MODE_FOREGROUND != opt->color_key_mode) {
        if (HI_TDE_COLOR_KEY_MODE_BACKGROUND == opt->color_key_mode) {
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Unsupported solidraw colorkey in background mode");
            return HI_ERR_TDE_INVALID_PARA;
        } else {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_SUCCESS;
        }
    }

    stColorkey.color_key_mode = (bForegroundColorkeyAfterClut) ? TDE_DRV_COLORKEY_FOREGROUND_AFTER_CLUT
                                                                : TDE_DRV_COLORKEY_FOREGROUND_BEFORE_CLUT;

    enFmtCategory = TdeOsiGetFmtCategory(src_surface->color_fmt);
    if (enFmtCategory >= TDE_COLORFMT_CATEGORY_MAX) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, TDE_COLORFMT_CATEGORY_MAX);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiGetFmtCategory, enFmtCategory);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    if (tde_hal_node_set_colorkey(hardware_node, enFmtCategory, &stColorkey) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_colorkey, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

STATIC hi_s32 TdeOsiSetBlend(tde_hardware_node *hardware_node, hi_tde_alpha_blending alpha_blending_cmd,
                             hi_tde_blend_opt blend_opt, tde_alu_mode *enAluMode, hi_bool bCheckBlend)
{
    hi_bool bSetBlend = ((hi_u32)alpha_blending_cmd & HI_TDE_ALPHA_BLENDING_BLEND) ? HI_TRUE : HI_FALSE;
    hi_bool bUnknownBlendMode = (blend_opt.blend_cmd == HI_TDE_BLEND_CMD_CONFIG) &&
                                ((blend_opt.src1_blend_mode >= HI_TDE_BLEND_MAX) ||
                                 (blend_opt.src2_blend_mode >= HI_TDE_BLEND_MAX));
    hi_bool bEnableAlphaRop = ((hi_u32)alpha_blending_cmd & HI_TDE_ALPHA_BLENDING_ROP) ? HI_TRUE : HI_FALSE;

    hi_bool BUnknownBlendCmd = (blend_opt.blend_cmd >= HI_TDE_BLEND_CMD_MAX);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (!bSetBlend) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    *enAluMode = TDE_ALU_BLEND;

    if (bCheckBlend) {
        if (BUnknownBlendCmd) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_BLEND_CMD_MAX);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, blend_opt.blend_cmd);
            return HI_ERR_TDE_INVALID_PARA;
        }

        if (bUnknownBlendMode) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HI_TDE_BLEND_CMD_CONFIG);
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HI_TDE_BLEND_MAX);
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, blend_opt.blend_cmd);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, blend_opt.src1_blend_mode);
            return HI_ERR_TDE_INVALID_PARA;
        }
    }

    if (tde_hal_node_set_blend(hardware_node, &blend_opt) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_blend, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    if (bEnableAlphaRop) {
        tde_hal_node_enable_alpha_rop(hardware_node);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

STATIC hi_s32 TdeOsiSetColorize(tde_hardware_node *hardware_node, hi_tde_alpha_blending alpha_blending_cmd,
                                hi_s32 color_resize)
{
    hi_bool bSetColorize = ((hi_u32)alpha_blending_cmd & HI_TDE_ALPHA_BLENDING_COLORIZE) ? HI_TRUE : HI_FALSE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (!bSetColorize) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if (tde_hal_node_set_colorize(hardware_node, color_resize) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_colorize, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

STATIC hi_s32 TdeOsiSetRop(tde_hardware_node *hardware_node, hi_tde_alpha_blending alpha_blending_cmd,
                           hi_tde_rop_mode rop_color, hi_tde_rop_mode rop_alpha, tde_alu_mode *enAluMode,
                           hi_bool SingleSr2Rop)
{
    hi_bool bSetRop = ((hi_u32)alpha_blending_cmd & HI_TDE_ALPHA_BLENDING_ROP) ? HI_TRUE : HI_FALSE;
    hi_bool bErrorRopCode = (HI_TDE_ROP_MAX <= rop_color) || (HI_TDE_ROP_MAX <= rop_alpha);
    hi_bool bOnlySupportSingleSr2Rop = ((!TdeOsiIsSingleSrc2Rop(rop_alpha)) || (!TdeOsiIsSingleSrc2Rop(rop_color)));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (!bSetRop) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    *enAluMode = TDE_ALU_ROP;

    if (bErrorRopCode) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_ROP_MAX);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, rop_color);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, rop_alpha);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (SingleSr2Rop) {
        if (bOnlySupportSingleSr2Rop) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, rop_alpha);
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, rop_color);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "tde_hal_node_set_rop return HI_FALSE");
            return -1;
        }
    }

    if (tde_hal_node_set_rop(hardware_node, rop_color, rop_alpha) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_rop, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/*****************************************************************************
 * Function:      TdeOsi2SourceFill
 * Description:   double sources fill operate, source1 is fill color, source2 is src_surface,bitmap is dst_surface,after
 *source2 zoom or deflicker, Rop with fill color or alpha blending to target bitmap, unsupport mirror,colorkey fi src
 *bitmap is mb format, only support single source mode, just set pstBackGround or fore_ground_surface Input: handle:
 *task handle pSrc: background bitmap info struct dst_surface: foreground bitmap info struct fill_color:  target bitmap
 *info struct opt:  operate parameter setting struct Output:        none Return:        HI_SUCCESS/HI_FAILURE Others:
 *none
 *****************************************************************************/
STATIC hi_s32 TdeOsi2SourceFill(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                                hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_fill_color *fill_color,
                                hi_tde_opt *opt)
{
    tde_base_opt_mode enBaseMode = TDE_QUIKE_FILL;
    tde_alu_mode enAluMode = TDE_SRC1_BYPASS;
    tde_hardware_node *hardware_node = HI_NULL;
    tde_surface_msg stDrvSurface = {0};
    tde_color_fill stDrvColorFill = {0};
    tde_scandirection_mode stSrcScanInfo = {0};
    tde_scandirection_mode stDstScanInfo = {0};
    hi_tde_rect stSrcOptArea = {0};
    hi_tde_rect stDstOptArea = {0};
    TDE_CLUT_USAGE_E enClutUsage = TDE_CLUT_USAGE_BUTT;
    hi_tde_rect tmp_src_rect = {0};
    hi_tde_rect tmp_dst_rect = {0};
    hi_s32 s32Ret = HI_FAILURE;
    hi_bool bSetFileNode;
    hi_bool bCheckSingleSrc2Rop = HI_FALSE;
    hi_bool bCheckBlend = HI_TRUE;
    hi_bool bContainNullPtr = ((NULL == dst_surface) || (NULL == dst_rect) || (NULL == fill_color) || (NULL == opt) ||
                               (NULL == src_surface) || (NULL == src_rect));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (bContainNullPtr) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fill_color);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, opt);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, src_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, src_rect);
        return HI_ERR_TDE_NULL_PTR;
    }

    memcpy(&tmp_src_rect, src_rect, sizeof(hi_tde_rect));
    memcpy(&tmp_dst_rect, dst_rect, sizeof(hi_tde_rect));

    TDE_CHECK_COLORFMT(fill_color->color_fmt);

    TDE_CHECK_DST_FMT(dst_surface->color_fmt);

    TDE_CHECK_OUTALPHAFROM(opt->out_alpha_from);

    TDE_CHECK_ALUCMD(opt->alpha_blending_cmd);

    if (TdeOsiCheckSurface(dst_surface, &tmp_dst_rect) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (TdeOsiCheckSurface(src_surface, &tmp_src_rect) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (!opt->resize) {
        TDE_UNIFY_RECT(&tmp_src_rect, &tmp_dst_rect);
    }

    if (tde_hal_init_node(&hardware_node) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_init_node, FAILURE_TAG);
        ;
        return HI_ERR_TDE_NO_MEM;
    }
    enBaseMode = TDE_NORM_FILL_2OPT;
    enAluMode = TDE_ALU_NONE;

    s32Ret = TdeOsiSetRop(hardware_node, opt->alpha_blending_cmd, opt->rop_color, opt->rop_alpha, &enAluMode,
                          bCheckSingleSrc2Rop);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetRop, FAILURE_TAG);
        tde_hal_free_node_buf(hardware_node);
        return s32Ret;
    }

    s32Ret = TdeOsiSetBlend(hardware_node, opt->alpha_blending_cmd, opt->blend_opt, &enAluMode, bCheckBlend);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetBlend, FAILURE_TAG);
        tde_hal_free_node_buf(hardware_node);
        return s32Ret;
    }

    s32Ret = TdeOsiSetColorize(hardware_node, opt->alpha_blending_cmd, opt->color_resize);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetColorize, FAILURE_TAG);
        tde_hal_free_node_buf(hardware_node);
        return s32Ret;
    }

    tde_hal_node_set_global_alpha(hardware_node, opt->global_alpha, opt->blend_opt.global_alpha_en);

    if (TdeOsiSetClipPara(HI_NULL, HI_NULL, src_surface, &tmp_src_rect, dst_surface, &tmp_dst_rect, opt,
                          hardware_node) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetClipPara, FAILURE_TAG);
        return HI_ERR_TDE_CLIP_AREA;
    }

    if (TdeOsiColorConvert(fill_color, src_surface, &stDrvColorFill.fill_data) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiColorConvert, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (tde_hal_node_set_base_operate(hardware_node, enBaseMode, enAluMode, &stDrvColorFill) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_base_operate, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    TdeOsiSetExtAlpha(HI_NULL, src_surface, hardware_node);

    if (TdeOsiGetScanInfo(src_surface, &tmp_src_rect, dst_surface, &tmp_dst_rect, opt, &stSrcScanInfo, &stDstScanInfo) <
        0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiGetScanInfo, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    TdeOsiConvertSurface(src_surface, &tmp_src_rect, &stSrcScanInfo, &stDrvSurface, &stSrcOptArea);

    tde_hal_set_src1(hardware_node, &stDrvSurface);

    TdeOsiConvertSurface(dst_surface, &tmp_dst_rect, &stDstScanInfo, &stDrvSurface, &stDstOptArea);

    tde_hal_node_set_tqt(hardware_node, &stDrvSurface, opt->out_alpha_from);

    if ((s32Ret = TdeOsiSetClutOpt(src_surface, dst_surface, &enClutUsage, opt->clut_reload, hardware_node)) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetClutOpt, FAILURE_TAG);
        return s32Ret;
    }

    s32Ret = TdeOsiSetForegroundColorKey(hardware_node, src_surface, opt, enClutUsage);
    if (s32Ret != HI_SUCCESS) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetForegroundColorKey, FAILURE_TAG);
        return s32Ret;
    }

    bSetFileNode = ((opt->resize) || (opt->deflicker_mode != HI_TDE_DEFLICKER_MODE_NONE) ||
                    (src_surface->color_fmt >= HI_TDE_COLOR_FMT_YCbCr422));

    if (bSetFileNode) {
        s32Ret = TdeOsiSetFilterNode(handle, hardware_node, src_surface, &tmp_src_rect, HI_NULL, HI_NULL, dst_surface,
                                     &tmp_dst_rect, opt->deflicker_mode, opt->filter_mode);
        if (s32Ret != HI_SUCCESS) {
            tde_hal_free_node_buf(hardware_node);
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetFilterNode, FAILURE_TAG);
            return s32Ret;
        }
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if ((s32Ret = TdeOsiSetNodeFinish(handle, hardware_node, 0, TDE_NODE_SUBM_ALONE)) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetNodeFinish, FAILURE_TAG);
        return s32Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}
#endif

STATIC hi_s32 TdeOsiRasterFmtCheckAlign(hi_tde_surface *pstSurface)
{
    hi_s32 s32Bpp = 0;
    hi_u32 u32BytePerPixel = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    s32Bpp = TdeOsiGetbppByFmt(pstSurface->color_fmt);
    if ((s32Bpp >= 8) && (s32Bpp != 24)) {
        u32BytePerPixel = ((hi_u32)s32Bpp >> 3);

        TDE_ADDRESS_CHECK_ALIGN(pstSurface->phy_addr, u32BytePerPixel);
        TDE_STRIDE_CHECK_ALIGN(pstSurface->stride, u32BytePerPixel);
    } else if (s32Bpp == 24) {
        TDE_ADDRESS_CHECK_ALIGN(pstSurface->phy_addr, 4);
        TDE_STRIDE_CHECK_ALIGN(pstSurface->stride, 4);
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

STATIC hi_s32 TdeOsiPreCheckSurface(hi_tde_surface *pstSurface, hi_tde_rect *pstRect)
{
    hi_s32 s32Ret = HI_FAILURE;
    hi_bool bUnknownColorFmt = HI_FALSE, bRasterFmt = HI_FALSE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstSurface, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstRect, HI_FAILURE);

    TDE_CHECK_COLORFMT(pstSurface->color_fmt);

    TDE_CHECK_SURFACEPARA(pstSurface, pstRect);

    bUnknownColorFmt = HI_TDE_COLOR_FMT_MAX <= pstSurface->color_fmt;
    if (bUnknownColorFmt) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_COLOR_FMT_MAX);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->color_fmt);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Unkown color format");
        return -1;
    }

    TDE_YCBCR422_FMT_CHECK_ODD(pstSurface->color_fmt, ((hi_u32)(pstRect->pos_x)), pstRect->width);

    bRasterFmt = (pstSurface->color_fmt <= HI_TDE_COLOR_FMT_halfword);
    if (bRasterFmt) {
        if ((s32Ret = TdeOsiRasterFmtCheckAlign(pstSurface)) != HI_SUCCESS) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiRasterFmtCheckAlign, FAILURE_TAG);
            return s32Ret;
        }
    } else {
        TDE_YCBCR_FMT_CHECK_STRIDE(pstSurface->cbcr_stride, pstSurface->color_fmt);
    }

    if (pstRect->pos_x + pstRect->width > pstSurface->width) {
        pstRect->width = pstSurface->width - pstRect->pos_x;
    }

    if (pstRect->pos_y + pstRect->height > pstSurface->height) {
        pstRect->height = pstSurface->height - pstRect->pos_y;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

#ifndef HI_BUILD_IN_BOOT
STATIC INLINE hi_s32 TdeOsiPreCheckSurfaceEX(hi_tde_surface *pstSurface, hi_tde_rect *pstRect)
{
    hi_s32 s32Ret = 0;
    hi_bool bUnknownColorFmt = HI_FALSE;
    hi_bool bRasterFmt = HI_FALSE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (0 == pstSurface->phy_addr) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION,
                             "The surface physical address is NULL:0 == pstSurface->phy_addr");
        return -1;
    }

    TDE_CHECK_COLORFMT(pstSurface->color_fmt);

    TDE_CHECK_SURFACEPARA_EX(pstSurface, pstRect);

    bUnknownColorFmt = HI_TDE_COLOR_FMT_MAX <= pstSurface->color_fmt;
    if (bUnknownColorFmt) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_COLOR_FMT_MAX);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstSurface->color_fmt);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Unkown color format");
        return -1;
    }

    TDE_YCBCR422_FMT_CHECK_ODD(pstSurface->color_fmt, ((hi_u32)(pstRect->pos_x)), pstRect->width);

    bRasterFmt = pstSurface->color_fmt <= HI_TDE_COLOR_FMT_halfword;
    if (bRasterFmt) {
        if ((s32Ret = TdeOsiRasterFmtCheckAlign(pstSurface)) != HI_SUCCESS) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiRasterFmtCheckAlign, FAILURE_TAG);
            return s32Ret;
        }
    } else {
        TDE_YCBCR_FMT_CHECK_STRIDE(pstSurface->cbcr_stride, pstSurface->color_fmt);
    }

    if (pstRect->pos_x + pstRect->width > pstSurface->width) {
        pstRect->width = pstSurface->width - pstRect->pos_x;
    }

    if (pstRect->pos_y + pstRect->height > pstSurface->height) {
        pstRect->height = pstSurface->height - pstRect->pos_y;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

/*****************************************************************************
 * Function:      TdeOsiCheckSurfaceEX
 * Description:   adjust right operate zone, according by the size of bitmap is more than 4095*4095,and less than
 *8190*8190 Input:         pstSurface: bitmap info pstRect: bitmap operate zone Output:        none Return: success/fail
 * Others:        none
 *****************************************************************************/
STATIC INLINE hi_s32 TdeOsiCheckSurfaceEX(hi_tde_surface *pstSurface, hi_tde_rect *pstRect)
{
    hi_s32 s32Ret = HI_FAILURE;
    hi_bool bInvalidOperationArea = HI_FALSE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((s32Ret = TdeOsiPreCheckSurfaceEX(pstSurface, pstRect)) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiPreCheckSurfaceEX, FAILURE_TAG);
        return s32Ret;
    }

    bInvalidOperationArea = ((TDE_MAX_RECT_WIDTH_EX < pstRect->width) || (TDE_MAX_RECT_HEIGHT_EX < pstRect->height));

    if (bInvalidOperationArea) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, TDE_MAX_RECT_WIDTH_EX);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, TDE_MAX_RECT_HEIGHT_EX);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRect->width);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRect->height);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "invalid operation area");
        return -1;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return 0;
}
#endif

/*****************************************************************************
 * Function:      TdeOsiCheckSurface
 * Description:   adjust right operate zone, according by the size of bitmap and operate zone from user upload
 * Input:         pstSurface: bitmap info
 *                pstRect: bitmap operate zone
 * Output:        none
 * Return:        success/fail
 * Others:        none
 *****************************************************************************/
STATIC INLINE hi_s32 TdeOsiCheckSurface(hi_tde_surface *pstSurface, hi_tde_rect *pstRect)
{
    hi_s32 s32Ret = HI_SUCCESS;
    hi_bool bInvalidOperationArea = HI_FALSE;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    s32Ret = TdeOsiPreCheckSurface(pstSurface, pstRect);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiPreCheckSurface, FAILURE_TAG);
        return s32Ret;
    }

    if (NULL != pstRect) {
        bInvalidOperationArea = ((TDE_MAX_RECT_WIDTH < pstRect->width) || (TDE_MAX_RECT_HEIGHT < pstRect->height));
    }

    if (bInvalidOperationArea) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, TDE_MAX_RECT_WIDTH);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, TDE_MAX_RECT_HEIGHT);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRect->width);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pstRect->height);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "invalid operation area");
        return -1;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return 0;
}

#ifndef HI_BUILD_IN_BOOT
/*****************************************************************************
 * Function:      TdeOsiCheckMbSurfaceEx
 * Description:   adjust right operate zone, according by the size of bitmap is more than 4095*4095 and less than
 *8190*8190 Input:         pstSurface: bitmap info pstRect: bitmap operate zone Output:        none Return: success/fail
 * Others:        none
 *****************************************************************************/
STATIC INLINE hi_s32 TdeOsiCheckMbSurfaceEX(hi_tde_mb_surface *pstMbSurface, hi_tde_rect *pstRect)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    TDE_CHECK_MBCOLORFMT(pstMbSurface->mb_color_fmt);

    TDE_CHECK_MBCBCRPARA(pstMbSurface);

    TDE_CHECK_MBSURFACEPARA(pstMbSurface, pstRect, TDE_MAX_RECT_WIDTH_EX, TDE_MAX_RECT_HEIGHT_EX);

    TDE_STRIDE_CHECK_ALIGN(pstMbSurface->y_stride, 4);
    TDE_STRIDE_CHECK_ALIGN(pstMbSurface->cbcr_stride, 4);

    if (pstRect->pos_x + pstRect->width > pstMbSurface->y_width) {
        pstRect->width = pstMbSurface->y_width - pstRect->pos_x;
    }

    if (pstRect->pos_y + pstRect->height > pstMbSurface->y_height) {
        pstRect->height = pstMbSurface->y_height - pstRect->pos_y;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return 0;
}

STATIC TDE_OPERATION_CATEGORY_E TdeOsiSingleSrcOperation(hi_tde_surface *fore_ground_surface,
                                                         hi_tde_rect *fore_ground_rect, hi_tde_surface *dst_surface,
                                                         hi_tde_rect *dst_rect, hi_tde_opt *opt)
{
    hi_tde_surface *pTmpSrc2 = fore_ground_surface;
    hi_tde_rect *pTmpSrc2Rect = fore_ground_rect;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(fore_ground_surface, TDE_OPERATION_BUTT);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(fore_ground_rect, TDE_OPERATION_BUTT);

    if (TdeOsiCheckSurface(pTmpSrc2, pTmpSrc2Rect) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return TDE_OPERATION_BUTT;
    }

    if ((HI_NULL == opt) || (!opt->resize)) {
        TDE_UNIFY_RECT(pTmpSrc2Rect, dst_rect);
    }

    if ((HI_NULL == opt) && (pTmpSrc2->color_fmt == dst_surface->color_fmt)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return TDE_OPERATION_SINGLE_SRC1;
    }

    if (HI_NULL == opt) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, opt);
        return TDE_OPERATION_BUTT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return TDE_OPERATION_SINGLE_SRC2;
}

STATIC TDE_OPERATION_CATEGORY_E TdeOsiDoubleSrcOperation(hi_tde_surface *pstBackGround, hi_tde_rect *back_ground_rect,
                                                         hi_tde_surface *fore_ground_surface,
                                                         hi_tde_rect *fore_ground_rect, hi_tde_rect *dst_rect,
                                                         hi_tde_opt *opt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstBackGround, TDE_OPERATION_BUTT);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(back_ground_rect, TDE_OPERATION_BUTT);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(fore_ground_rect, TDE_OPERATION_BUTT);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(opt, TDE_OPERATION_BUTT);

    TDE_CHECK_NOT_MB(pstBackGround->color_fmt);

    if (TdeOsiCheckSurface(pstBackGround, back_ground_rect) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return TDE_OPERATION_BUTT;
    }

    if (TdeOsiCheckSurface(fore_ground_surface, fore_ground_rect) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return TDE_OPERATION_BUTT;
    }

    if ((back_ground_rect->height != dst_rect->height) || (back_ground_rect->width != dst_rect->width)) { /* * SRC1 has
                                                                                                             not zme * */
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, back_ground_rect->height);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, back_ground_rect->width);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, dst_rect->height);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, dst_rect->width);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "BackGroundRect is not the same with DstRect");
        return TDE_OPERATION_BUTT;
    }

    if (!opt->resize) {
        if (back_ground_rect->height != fore_ground_rect->height) {
            back_ground_rect->height = TDE_MIN(back_ground_rect->height, fore_ground_rect->height);
            dst_rect->height = back_ground_rect->height;
            fore_ground_rect->height = back_ground_rect->height;
        }

        if (back_ground_rect->width != fore_ground_rect->width) {
            back_ground_rect->width = TDE_MIN(back_ground_rect->width, fore_ground_rect->width);
            dst_rect->width = back_ground_rect->width;
            fore_ground_rect->width = back_ground_rect->width;
        }
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return TDE_OPERATION_DOUBLE_SRC;
}

/*****************************************************************************
 * Function:      TdeOsiGetOptCategory
 * Description:   analyze TDE operate type
 * Input:         pstBackGround: background bitmap info
 *                back_ground_rect: background bitmap operate zone
 *                fore_ground_surface: foreground bitmap info
 *                fore_ground_rect: foreground bitmap operate zone
 *                dst_surface: target bitmap info
 *                dst_rect: target bitmap operate zone
 *                opt: operate option
 * Output:        none
 * Return:        TDE operate type
 * Others:        none
 *****************************************************************************/
STATIC TDE_OPERATION_CATEGORY_E TdeOsiGetOptCategory(hi_tde_surface *pstBackGround, hi_tde_rect *back_ground_rect,
                                                     hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                                                     hi_tde_surface *dst_surface, hi_tde_rect *dst_rect,
                                                     hi_tde_opt *opt)
{
    hi_s32 Ret = HI_SUCCESS;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((HI_NULL == dst_surface) || (HI_NULL == dst_rect)) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_rect);
        return TDE_OPERATION_BUTT;
    }

    if (TdeOsiCheckSurface(dst_surface, dst_rect) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return TDE_OPERATION_BUTT;
    }

    if ((HI_NULL == pstBackGround) && (HI_NULL == fore_ground_surface)) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstBackGround);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fore_ground_surface);
        return TDE_OPERATION_BUTT;
    } else if ((HI_NULL != pstBackGround) && (HI_NULL != fore_ground_surface)) {
        Ret = TdeOsiDoubleSrcOperation(pstBackGround, back_ground_rect, fore_ground_surface, fore_ground_rect, dst_rect,
                                       opt);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return Ret;
    } else if (HI_NULL == pstBackGround) {
        Ret = TdeOsiSingleSrcOperation(fore_ground_surface, fore_ground_rect, dst_surface, dst_rect, opt);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return Ret;
    } else {
        Ret = TdeOsiSingleSrcOperation(pstBackGround, back_ground_rect, dst_surface, dst_rect, opt);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return Ret;
    }
}
#endif

/*****************************************************************************
 * Function:      TdeOsiConvertSurface
 * Description:   raster bitmap info by user upload translate to bitmap info which driver and hardware need
 * Input:         pstSur: raster bitmap info by user upload
 *                pstRect: raster bitmap operate zone by user upload
 *                pstScanInfo: scanning direction info
 *                pstDrvSur: bitmap info which driver and hardware need
 * Output:        pstOperationArea: new operate zone fixed by scannning direction
 * Return:        none
 * Others:        none
 *****************************************************************************/
STATIC INLINE hi_void TdeOsiConvertSurface(hi_tde_surface *pstSur, hi_tde_rect *pstRect,
                                           tde_scandirection_mode *pstScanInfo, tde_surface_msg *pstDrvSur,
                                           hi_tde_rect *pstOperationArea)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstSur);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstRect);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstScanInfo);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(pstDrvSur);

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(pstSur->color_fmt, HI_TDE_COLOR_FMT_MAX + 1);
    pstDrvSur->color_fmt = g_enTdeCommonDrvColorFmt[pstSur->color_fmt];

    pstDrvSur->width = pstRect->width;
    pstDrvSur->height = pstRect->height;
    pstDrvSur->pitch = pstSur->stride;
    pstDrvSur->alpha_max_is_255 = pstSur->alpha_max_is_255;
    pstDrvSur->hor_scan = pstScanInfo->hor_scan;
    pstDrvSur->ver_scan = pstScanInfo->ver_scan;
    pstDrvSur->phy_addr = pstSur->phy_addr;
    pstDrvSur->cbcr_phy_addr = pstSur->cbcr_phy_addr;
    pstDrvSur->cbcr_pitch = pstSur->cbcr_stride;
    pstDrvSur->rgb_order = g_enTdeArgbOrder[pstSur->color_fmt];

    pstDrvSur->pos_x = (hi_u32)pstRect->pos_x;
    pstDrvSur->pos_y = (hi_u32)pstRect->pos_y;

    if (NULL != pstOperationArea) {
        pstOperationArea->pos_x = pstRect->pos_x;
        pstOperationArea->pos_y = (hi_s32)pstDrvSur->pos_y;
        pstOperationArea->width = pstDrvSur->width;
        pstOperationArea->height = pstDrvSur->height;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return;
}

/*****************************************************************************
 * Function:      TdeOsiSetClipPara
 * Description:   set clip zone parameter
 * Input:         pstBackGround: background bitmap info
 *                pstBGRect: background bitmap operate zone
 *                fore_ground_surface: foreground bitmap info
 *                pstFGRect: foreground bitmap operate zone
 *                dst_surface:  target bitmap info
 *                dst_rect: target bitmap operate zone
 *                opt: operate option
 *                pstHwNode: hardware operate node
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
STATIC hi_s32 TdeOsiSetClipPara(hi_tde_surface *pstBackGround, hi_tde_rect *pstBGRect,
                                hi_tde_surface *fore_ground_surface, hi_tde_rect *pstFGRect,
                                hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_opt *opt,
                                tde_hardware_node *pstHwNode)
{
#ifdef HI_BUILD_IN_BOOT
    // HI_UNUSED(pstBackGround);
    // HI_UNUSED(pstBGRect);
    // HI_UNUSED(fore_ground_surface);
    // HI_UNUSED(pstFGRect);
    // HI_UNUSED(dst_surface);
    // HI_UNUSED(dst_rect);
    // HI_UNUSED(opt);
    // HI_UNUSED(pstHwNode);
#else
    tde_clip_cmd stClip = {0};
    hi_tde_rect stInterRect = {0};

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(fore_ground_surface, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_surface, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_rect, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(opt, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstHwNode, HI_FAILURE);

    if ((HI_TDE_CLIP_MODE_INSIDE == opt->clip_mode) && (!opt->resize)) {
        if (TdeOsiGetInterRect(dst_rect, &opt->clip_rect, &stInterRect) < 0) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiGetInterRect, FAILURE_TAG);
            return HI_ERR_TDE_CLIP_AREA;
        }

        if ((NULL != pstBackGround) && (NULL != pstBGRect)) {
            pstBGRect->pos_x += stInterRect.pos_x - dst_rect->pos_x;
            pstBGRect->pos_y += stInterRect.pos_y - dst_rect->pos_y;
            pstBGRect->height = stInterRect.height;
            pstBGRect->width = stInterRect.width;
        }

        if (NULL != pstFGRect) {
            pstFGRect->pos_x += stInterRect.pos_x - dst_rect->pos_x;
            pstFGRect->pos_y += stInterRect.pos_y - dst_rect->pos_y;
            pstFGRect->height = stInterRect.height;
            pstFGRect->width = stInterRect.width;
        }

        *dst_rect = stInterRect;
    } else if (HI_TDE_CLIP_MODE_INSIDE == opt->clip_mode) {
        if (TdeOsiGetInterRect(dst_rect, &opt->clip_rect, &stInterRect) < 0) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiGetInterRect, FAILURE_TAG);
            return HI_ERR_TDE_CLIP_AREA;
        }

        stClip.is_inside_clip = HI_TRUE;

        if (opt->clip_rect.pos_x <= dst_rect->pos_x) {
            stClip.clip_start_x = 0;
        } else {
            stClip.clip_start_x = opt->clip_rect.pos_x - dst_rect->pos_x;
        }

        if (opt->clip_rect.pos_y <= dst_rect->pos_y) {
            stClip.clip_start_y = 0;
        } else {
            stClip.clip_start_y = opt->clip_rect.pos_y - dst_rect->pos_y;
        }

        stClip.clip_end_x = stClip.clip_start_x + stInterRect.width - 1;
        stClip.clip_end_y = stClip.clip_start_y + stInterRect.height - 1;

        if (tde_hal_node_set_clipping(pstHwNode, &stClip) < 0) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_clipping, FAILURE_TAG);
            return HI_ERR_TDE_UNSUPPORTED_OPERATION;
        }
    } else if (HI_TDE_CLIP_MODE_OUTSIDE == opt->clip_mode) {
        if (TdeOsiIsRect1InRect2(dst_rect, &opt->clip_rect)) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiIsRect1InRect2, FAILURE_TAG);
            return HI_ERR_TDE_CLIP_AREA;
        }

        if (TdeOsiGetInterRect(dst_rect, &opt->clip_rect, &stInterRect) < 0) {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 0;
        }

        stClip.is_inside_clip = HI_FALSE;
        if (opt->clip_rect.pos_x <= dst_rect->pos_x) {
            stClip.clip_start_x = 0;
        } else {
            stClip.clip_start_x = opt->clip_rect.pos_x - dst_rect->pos_x;
        }

        if (opt->clip_rect.pos_y <= dst_rect->pos_y) {
            stClip.clip_start_y = 0;
        } else {
            stClip.clip_start_y = opt->clip_rect.pos_y - dst_rect->pos_y;
        }
        stClip.clip_end_x = stClip.clip_start_x + stInterRect.width - 1;
        stClip.clip_end_y = stClip.clip_start_y + stInterRect.height - 1;

        if (tde_hal_node_set_clipping(pstHwNode, &stClip) < 0) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_clipping, FAILURE_TAG);
            return HI_ERR_TDE_UNSUPPORTED_OPERATION;
        }
    } else if (opt->clip_mode >= HI_TDE_CLIP_MODE_MAX) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_CLIP_MODE_MAX);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, opt->clip_mode);
        return HI_ERR_TDE_INVALID_PARA;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
#endif
    return 0;
}

#ifndef HI_BUILD_IN_BOOT
STATIC hi_s32 TdeOsiSetPatternClipPara(hi_tde_surface *pstBackGround, hi_tde_rect *pstBGRect,
                                       hi_tde_surface *fore_ground_surface, hi_tde_rect *pstFGRect,
                                       hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_pattern_fill_opt *opt,
                                       tde_hardware_node *pstHwNode)
{
    tde_clip_cmd stClip = {0};
    hi_tde_rect stInterRect = {0};
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(fore_ground_surface, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_surface, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(opt, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(pstHwNode, HI_FAILURE);

    TDE_CHECK_CLIPMODE(opt->clip_mode);

    if (HI_TDE_CLIP_MODE_INSIDE == opt->clip_mode) {
        if (TdeOsiGetInterRect(dst_rect, &opt->clip_rect, &stInterRect) < 0) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiGetInterRect, FAILURE_TAG);
            return HI_ERR_TDE_CLIP_AREA;
        }
        stClip.is_inside_clip = HI_TRUE;
        stClip.clip_start_x = opt->clip_rect.pos_x - dst_rect->pos_x;
        stClip.clip_start_y = opt->clip_rect.pos_y - dst_rect->pos_y;
        stClip.clip_end_x = (opt->clip_rect.pos_x - dst_rect->pos_x) + opt->clip_rect.width - 1;
        stClip.clip_end_y = (opt->clip_rect.pos_y - dst_rect->pos_y) + opt->clip_rect.height - 1;

        if (tde_hal_node_set_clipping(pstHwNode, &stClip) < 0) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_clipping, FAILURE_TAG);
            return HI_ERR_TDE_UNSUPPORTED_OPERATION;
        }
    } else if (HI_TDE_CLIP_MODE_OUTSIDE == opt->clip_mode) {
        if (TdeOsiIsRect1InRect2(dst_rect, &opt->clip_rect)) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiIsRect1InRect2, FAILURE_TAG);
            return HI_ERR_TDE_CLIP_AREA;
        }

        if (TdeOsiGetInterRect(dst_rect, &opt->clip_rect, &stInterRect) < 0) {
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return 0;
        }

        stClip.is_inside_clip = HI_FALSE;

        stClip.clip_start_x = opt->clip_rect.pos_x - dst_rect->pos_x;
        stClip.clip_start_y = opt->clip_rect.pos_y - dst_rect->pos_y;
        stClip.clip_end_x = (opt->clip_rect.pos_x - dst_rect->pos_x) + opt->clip_rect.width - 1;
        stClip.clip_end_y = (opt->clip_rect.pos_y - dst_rect->pos_y) + opt->clip_rect.height - 1;

        if (tde_hal_node_set_clipping(pstHwNode, &stClip) < 0) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_clipping, FAILURE_TAG);
            return HI_ERR_TDE_UNSUPPORTED_OPERATION;
        }
    } else if (opt->clip_mode >= HI_TDE_CLIP_MODE_MAX) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_CLIP_MODE_MAX);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, opt->clip_mode);
        return HI_ERR_TDE_INVALID_PARA;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return 0;
}
#endif

/*****************************************************************************
 Prototype       : TdeOsiSetExtAlpha
 Description     : two alpha values when set ARGB1555 format
 Input           : pstBackGround  **
                   fore_ground_surface  **
                   pstHwNode      **
 Output          : None
 Return Value    :
 Global Variable
    Read Only    :
    Read & Write :
  History
  1.Date         : 2008/6/10
    Author       : executable frame by single
    Modification : Created function

*****************************************************************************/
STATIC INLINE hi_void TdeOsiSetExtAlpha(hi_tde_surface *pstBackGround, hi_tde_surface *fore_ground_surface,
                                        tde_hardware_node *pstHwNode)
{
#ifdef HI_BUILD_IN_BOOT
#else
    tde_src_mode src_mode = TDE_DRV_SRC_NONE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((NULL != fore_ground_surface) && (HI_TDE_COLOR_FMT_ARGB1555 <= fore_ground_surface->color_fmt) &&
        (fore_ground_surface->color_fmt <= HI_TDE_COLOR_FMT_BGRA1555)) {
        tde_hal_node_set_src2_alpha(pstHwNode);
    }

    if ((NULL != pstBackGround) && (HI_TDE_COLOR_FMT_ARGB1555 <= pstBackGround->color_fmt) &&
        (pstBackGround->color_fmt <= HI_TDE_COLOR_FMT_BGRA1555)) {
        tde_hal_node_set_src1_alpha(pstHwNode);
    }

    if ((NULL != fore_ground_surface) && (fore_ground_surface->support_alpha_ext_1555) &&
        (HI_TDE_COLOR_FMT_ARGB1555 <= fore_ground_surface->color_fmt) &&
        (fore_ground_surface->color_fmt <= HI_TDE_COLOR_FMT_BGRA1555)) {
        src_mode = (hi_u32)src_mode | TDE_DRV_SRC_S2;
    }

    if ((NULL != pstBackGround) && (pstBackGround->support_alpha_ext_1555) &&
        (HI_TDE_COLOR_FMT_ARGB1555 <= pstBackGround->color_fmt) &&
        (pstBackGround->color_fmt <= HI_TDE_COLOR_FMT_BGRA1555)) {
        src_mode = (hi_u32)src_mode | TDE_DRV_SRC_S1;
    }

    if ((hi_u32)src_mode & TDE_DRV_SRC_S1) {
        tde_hal_node_set_exp_alpha(pstHwNode, src_mode, pstBackGround->alpha0, pstBackGround->alpha1);
    } else if ((hi_u32)src_mode & TDE_DRV_SRC_S2) {
        tde_hal_node_set_exp_alpha(pstHwNode, src_mode, fore_ground_surface->alpha0, fore_ground_surface->alpha1);
    }
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
#endif
}

/*****************************************************************************
 * Function:      TdeOsiSetBaseOptParaForBlit
 * Description:   encapsulation function used to set operate type
 * Input:         opt: operate option
 *                enOptCategory: operate category
 *                pstHwNode: nareware operate node
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
STATIC hi_s32 TdeOsiSetBaseOptParaForBlit(hi_tde_opt *opt, hi_tde_surface *pstSrc1, hi_tde_surface *pstSrc2,
                                          TDE_OPERATION_CATEGORY_E enOptCategory, tde_hardware_node *pstHwNode)
{
#ifndef HI_BUILD_IN_BOOT
    hi_s32 ret = HI_SUCCESS;
#endif
    tde_base_opt_mode enBaseOpt = {0};
    tde_alu_mode alu_mode = TDE_ALU_NONE;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((HI_NULL == pstHwNode) || (HI_NULL == opt)) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if ((HI_NULL != pstSrc1) && (HI_NULL != pstSrc2)) {
        if ((TDE_COLORFMT_CATEGORY_An == TdeOsiGetFmtCategory(pstSrc2->color_fmt)) &&
            (TDE_COLORFMT_CATEGORY_ARGB == TdeOsiGetFmtCategory(pstSrc1->color_fmt) ||
             TDE_COLORFMT_CATEGORY_YCbCr == TdeOsiGetFmtCategory(pstSrc1->color_fmt))) {
            alu_mode = TDE_SRC1_BYPASS;
        }
    }

#ifndef HI_BUILD_IN_BOOT
    if ((hi_u32)opt->alpha_blending_cmd & HI_TDE_ALPHA_BLENDING_ROP) {
        alu_mode = TDE_ALU_ROP;

        if (tde_hal_node_set_rop(pstHwNode, opt->rop_color, opt->rop_alpha) < 0) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_rop, FAILURE_TAG);
            return HI_ERR_TDE_UNSUPPORTED_OPERATION;
        }
    }

    if ((hi_u32)opt->alpha_blending_cmd & HI_TDE_ALPHA_BLENDING_BLEND) {
        alu_mode = TDE_ALU_BLEND;
        if (tde_hal_node_set_blend(pstHwNode, &opt->blend_opt) < 0) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_blend, FAILURE_TAG);
            return HI_ERR_TDE_UNSUPPORTED_OPERATION;
        }

        if ((hi_u32)opt->alpha_blending_cmd & HI_TDE_ALPHA_BLENDING_ROP) {
            tde_hal_node_enable_alpha_rop(pstHwNode);
        }
    }

    if ((hi_u32)opt->alpha_blending_cmd & HI_TDE_ALPHA_BLENDING_COLORIZE) {
        if (tde_hal_node_set_colorize(pstHwNode, opt->color_resize) < 0) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_colorize, FAILURE_TAG);
            return HI_ERR_TDE_UNSUPPORTED_OPERATION;
        }
    }
#endif
    if (TDE_OPERATION_DOUBLE_SRC == enOptCategory) {
        enBaseOpt = TDE_NORM_BLIT_2OPT;
    } else {
        enBaseOpt = TDE_NORM_BLIT_1OPT;
    }

    tde_hal_node_set_global_alpha(pstHwNode, opt->global_alpha, opt->blend_opt.global_alpha_en);

    if (tde_hal_node_set_base_operate(pstHwNode, enBaseOpt, alu_mode, 0) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_base_operate, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

#ifndef HI_BUILD_IN_BOOT
    ret = tde_set_node_csc(pstHwNode, &(opt->csc_opt));
    if (ret != HI_SUCCESS) {
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
/*****************************************************************************
 * Function:      TdeOsiSetNodeFinish
 * Description:   complete node config, add node to list signed by handle
 * Input:         handle: task handle, which is sumbit list
 *                hardware_node: set node
 *                work_buf_num: temporary buffer number
 *                submit_type: submit node type
 * Output:        none
 * Return:        return slice number
 * Others:        node struct is following:
 *                 ----------------------
 *                 |  software node pointer(4)   |
 *                 ----------------------
 *                 |  config parameter          |
 *                 ----------------------
 *                 |  physical address of next node(4) |
 *                 ----------------------
 *                 |  update flag of next node(4) |
 *                 ----------------------
 *****************************************************************************/
STATIC hi_s32 TdeOsiSetNodeFinish(hi_s32 handle, tde_hardware_node *hardware_node, hi_u32 work_buf_num,
                                  tde_node_submit_type submit_type)
{
    tde_handle_mgr *pHandleMgr = NULL;
    tde_sw_job *pstJob = NULL;
    tde_sw_node *pstCmd = NULL;
    hi_bool bValid = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    bValid = query_handle(handle, &pHandleMgr);
    if (!bValid) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, query_handle HI_FALSE, FAILURE_TAG);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, handle);
        return HI_ERR_TDE_INVALID_HANDLE;
    }

    pstJob = (tde_sw_job *)pHandleMgr->res;
    if (pstJob->has_submitted) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, handle);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "job Handle already submitted");
        return HI_ERR_TDE_INVALID_HANDLE;
    }

    pstCmd = (tde_sw_node *)tde_malloc(sizeof(tde_sw_node));
    if (NULL == pstCmd) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_malloc, FAILURE_TAG);
        return HI_ERR_TDE_NO_MEM;
    }

    if (submit_type != TDE_NODE_SUBM_CHILD) {
        pstJob->cmd_num++;

        if (1 == pstJob->cmd_num) {
            pstJob->first_cmd = pstCmd;

            OSAL_INIT_LIST_HEAD(&pstCmd->list_head);
        }

        pstJob->last_cmd = pstCmd;
    }

#if !defined(HI_BUILD_IN_BOOT) && defined(CONFIG_GFX_PROC_SUPPORT)
    tde_proc_record_node(hardware_node);
#endif

    pstCmd->node_buf.vir_buf = (hi_void *)hardware_node - TDE_NODE_HEAD_BYTE;
    pstCmd->node_buf.node_size = sizeof(tde_hardware_node);
    pstCmd->node_buf.up_data_flag = (0xffffffff) | ((hi_u64)0x000003ff << 32);
    pstCmd->node_buf.phy_addr = wgetphy(pstCmd->node_buf.vir_buf) + TDE_NODE_HEAD_BYTE;

    if (HI_NULL != pstJob->tail_node) {
        /* * 找到Node的Tail位置 * */
        hi_u32 *pNextNodeAddr = (hi_u32 *)pstJob->tail_node->node_buf.vir_buf + (TDE_NODE_HEAD_BYTE >> 2) +
                                ((pstJob->tail_node->node_buf.node_size) >> 2);
        /* * 找到Node的Tail位置后3个字节 * */
        hi_u64 *pNextNodeUpdate = (hi_u64 *)(pNextNodeAddr + 1);
        /* * Tail的第一个字节保存硬件节点的物理地址 * */
        *pNextNodeAddr = pstCmd->node_buf.phy_addr;
        /* * Tail的后三个字节保存硬件节点的更新位 * */
        *pNextNodeUpdate = pstCmd->node_buf.up_data_flag;
    }

    pstCmd->handle = pstJob->handle;
    pstCmd->index = pstJob->cmd_num;
    pstCmd->submit_type = submit_type;
    pstCmd->phy_buf_num = work_buf_num;
    *(((hi_u32 *)pstCmd->node_buf.vir_buf) + 1) = handle;

    osal_list_add_tail(&pstCmd->list_head, &pstJob->first_cmd->list_head);
    pstJob->tail_node = pstCmd;
    pstJob->node_num++;
    if (pstCmd->phy_buf_num != 0) {
        pstJob->aq_use_buf = HI_TRUE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}
/****************************************************************************
                             TDE osi ctl interface initialization
****************************************************************************/

/*****************************************************************************
 * Function:      TdeOsiBeginJob
 * Description:   get TDE task handle
 * Input:         none
 * Output:        none
 * Return:        created task handle
 * Others:        none
 *****************************************************************************/
hi_s32 tde_begin_job(hi_s32 *handle, void *private_data)
{
    hi_s32 Ret = HI_SUCCESS;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    Ret = tde_list_beg_job(handle, private_data);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}
#ifndef HI_BUILD_IN_BOOT
EXPORT_SYMBOL(tde_begin_job);
#endif

hi_s32 drv_tde_begin_job(hi_s32 *handle)
{
    return tde_begin_job(handle, NULL);
}
#ifndef HI_BUILD_IN_BOOT
EXPORT_SYMBOL(drv_tde_begin_job);
#endif

/*****************************************************************************
 * Function:      drv_tde_end_job
 * Description:   submit TDE task
 * Input:         handle: task handle
 *                is_block: if block
 *                time_out: timeout value(unit by 10ms)
 * Output:        none
 * Return:        success / fail
 * Others:        none
 *****************************************************************************/
hi_s32 drv_tde_end_job(hi_s32 handle, hi_bool is_block, hi_u32 time_out, hi_bool is_sync,
                       drv_tde_func_callback func_complete_callback, hi_void *func_para)
{
    hi_s32 Ret = HI_SUCCESS;
#ifndef HI_BUILD_IN_BOOT
    tde_notify_mode notify_type;
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

#ifndef HI_BUILD_IN_BOOT
    if (is_block) {
        if (osal_in_interrupt()) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_in_interrupt, FAILURE_TAG);
            return HI_ERR_TDE_UNSUPPORTED_OPERATION;
        }
        notify_type = TDE_JOB_WAKE_NOTIFY;
    } else {
        notify_type = TDE_JOB_COMPL_NOTIFY;
    }

    Ret = tde_list_submit_job(handle, time_out, func_complete_callback, func_para, notify_type);
#else
    Ret = tde_list_submit_job(handle, time_out, func_complete_callback, func_para, TDE_JOB_WAKE_NOTIFY);
#endif
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

#ifndef HI_BUILD_IN_BOOT
EXPORT_SYMBOL(drv_tde_end_job);

/*****************************************************************************
 * Function:      TdeOsiDelJob
 * Description:   delete created TDE task, only effective for call before endjob
 *                use to release software resource of list of task
 * Input:         handle: task handle
 * Output:        none
 * Return:        success/fail
 * Others:        none
 *****************************************************************************/
hi_s32 drv_tde_cancel_job(hi_s32 handle)
{
    hi_s32 Ret = HI_SUCCESS;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    Ret = tde_list_cancel_job(handle);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}
EXPORT_SYMBOL(drv_tde_cancel_job);

/*****************************************************************************
 * Function:      drv_tde_wait_for_done
 * Description:   wait for completion of submit TDE operate
 * Input:         handle: task handle
 * Output:        none
 * Return:        success/fail
 * Others:        none
 *****************************************************************************/
hi_s32 drv_tde_wait_for_done(hi_s32 handle, hi_u32 time_out)
{
    hi_s32 Ret = HI_SUCCESS;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (osal_in_interrupt()) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_in_interrupt, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    Ret = tde_list_wait_for_done(handle, time_out);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}
EXPORT_SYMBOL(drv_tde_wait_for_done);

/*****************************************************************************
 * Function:      drv_tde_wait_all_done
 * Description:   wait for all TDE operate completion
 * Input:         none
 * Output:        none
 * Return:        success/fail
 * Others:        none
 *****************************************************************************/
hi_s32 drv_tde_wait_all_done(hi_bool is_sync)
{
    hi_s32 Ret = HI_SUCCESS;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (osal_in_interrupt()) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, osal_in_interrupt, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    Ret = tde_list_wait_all_done();
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}
EXPORT_SYMBOL(drv_tde_wait_all_done);

/*****************************************************************************
* Function:      drv_tde_quick_copy
* Description:    quick blit source to target, no any functional operate, the size of source and target are the same
*                format is not MB format
* Input:         pSrc: source bitmap info
*                dst_surface:  target bitmap info
*                func_complete_callback: callback function pointer when operate is completed
                 when it is null, it say to need not notice operate is over
* Output:        none
* Return:        none
* Others:        none
*****************************************************************************/
hi_s32 drv_tde_quick_copy(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                          hi_tde_surface *dst_surface, hi_tde_rect *dst_rect)
{
    hi_s32 Ret = HI_SUCCESS;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(src_surface, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(src_rect, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_surface, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_rect, HI_ERR_TDE_NULL_PTR);
    Ret = drv_tde_blit(handle, NULL, NULL, src_surface, src_rect, dst_surface, dst_rect, NULL);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}
EXPORT_SYMBOL(drv_tde_quick_copy);

/*****************************************************************************
 * Function:      tde_hal_quick_resize
 * Description:   zoom the size of source bitmap to the size aasigned by target bitmap, of which source and target can
 *be the same Input:         pSrc: source bitmap info struct dst_surface: target bitmap info struct
 *                func_complete_callback: callback function pointer when operate is over;if null, to say to no need to
 *notice Output:        none Return:        none
 *****************************************************************************/
hi_s32 tde_hal_quick_resize(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                            hi_tde_surface *dst_surface, hi_tde_rect *dst_rect)
{
    hi_s32 s32Ret = HI_FAILURE;
    hi_tde_opt option = {0};

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    option.resize = HI_TRUE;

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(src_surface, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(src_rect, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_surface, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_rect, HI_ERR_TDE_NULL_PTR);

    if ((s32Ret = drv_tde_blit(handle, NULL, NULL, src_surface, src_rect, dst_surface, dst_rect, &option)) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_blit, FAILURE_TAG);
        return s32Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}
EXPORT_SYMBOL(tde_hal_quick_resize);

/*****************************************************************************
 * Function:      tde_hal_quick_flicker
 * Description:   deflicker source bitmap,output to target bitmap,source and target can be the same
 * Input:         pSrc: source bitmap info struct
 *                dst_surface: terget bitmap info struct
 *                func_complete_callback: callback function pointer when operate is over;if null, to say to no need to
 *notice Output:        none Return:        none Others:        none
 *****************************************************************************/
hi_s32 tde_hal_quick_flicker(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                             hi_tde_surface *dst_surface, hi_tde_rect *dst_rect)
{
    hi_s32 s32Ret = HI_FAILURE;
    hi_tde_opt option = {0};

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    option.deflicker_mode = HI_TDE_DEFLICKER_MODE_BOTH;

    option.resize = HI_TRUE;

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(src_surface, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(src_rect, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_surface, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_rect, HI_ERR_TDE_NULL_PTR);

    if ((s32Ret = drv_tde_blit(handle, NULL, NULL, src_surface, src_rect, dst_surface, dst_rect, &option)) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, drv_tde_blit, FAILURE_TAG);
        return s32Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}
EXPORT_SYMBOL(tde_hal_quick_flicker);

#endif

/*****************************************************************************
 * Function:      drv_tde_quick_fill
 * Description:   quick fill fixed value to target bitmap, fill value is refered to target bitmap
 * Input:         dst_surface: target bitmap info struct
 *                fill_data: fill value
 *                func_complete_callback: callback function pointer when operate is over;if null, to say to no need to
 *notice Output:        none Return:        none Others:        none
 *****************************************************************************/
hi_s32 drv_tde_quick_fill(hi_s32 handle, hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_u32 fill_data)
{
    hi_s32 Ret = HI_SUCCESS;
    hi_tde_fill_color fill_color;
    memset(&fill_color, 0x0, sizeof(fill_color));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_surface, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_rect, HI_ERR_TDE_NULL_PTR);

    fill_color.color_fmt = dst_surface->color_fmt;
    fill_color.color_value = fill_data;

    Ret = TdeOsi1SourceFill(handle, dst_surface, dst_rect, &fill_color, NULL);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return Ret;
}
#ifndef HI_BUILD_IN_BOOT
EXPORT_SYMBOL(drv_tde_quick_fill);
#endif

#ifndef HI_BUILD_IN_BOOT
/*****************************************************************************
 * Function:      TdeOsiSingleSrc1Blit
 * Description:   source1 operate realization
 * Input:         pstBackGround: source1 bitmap info struct
 *                back_ground_rect: source1 bitmap operate zone
 *                dst_surface: target bitmap info struct
 *                dst_rect: target bitmap operate zone
 * Output:        none
 * Return:        success/fail
 * Others:        none
 *****************************************************************************/
STATIC hi_s32 TdeOsiSingleSrc1Blit(hi_s32 handle, hi_tde_surface *pstBackGround, hi_tde_rect *back_ground_rect,
                                   hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_bool mmz_for_src,
                                   hi_bool mmz_for_dst)
{
    hi_s32 s32Ret = HI_SUCCESS;
    tde_hardware_node *hardware_node = NULL;
    tde_surface_msg stSrcDrvSurface = {0};
    tde_surface_msg stDstDrvSurface = {0};
    tde_scandirection_mode stSrcScanInfo = {0};
    tde_scandirection_mode stDstScanInfo = {0};

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((NULL == pstBackGround) || (NULL == back_ground_rect) || (NULL == dst_surface) || (NULL == dst_rect)) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstBackGround);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, back_ground_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_rect);
        return HI_ERR_TDE_NULL_PTR;
    }

    TDE_CHECK_NOT_MB(dst_surface->color_fmt);

    TDE_UNIFY_RECT(back_ground_rect, dst_rect);

    if (tde_hal_init_node(&hardware_node) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_init_node, FAILURE_TAG);
        return HI_ERR_TDE_NO_MEM;
    }

    if (tde_hal_node_set_base_operate(hardware_node, TDE_QUIKE_COPY, TDE_SRC1_BYPASS, 0) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_base_operate, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    if (TdeOsiGetScanInfo(pstBackGround, back_ground_rect, dst_surface, dst_rect, HI_NULL, &stSrcScanInfo,
                          &stDstScanInfo) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiGetScanInfo, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    TdeOsiConvertSurface(pstBackGround, back_ground_rect, &stSrcScanInfo, &stSrcDrvSurface, NULL);
    stSrcDrvSurface.is_cma = mmz_for_src;

    tde_hal_set_src1(hardware_node, &stSrcDrvSurface);

    TdeOsiSetExtAlpha(pstBackGround, HI_NULL, hardware_node);

    TdeOsiConvertSurface(dst_surface, dst_rect, &stDstScanInfo, &stDstDrvSurface, NULL);
    stDstDrvSurface.is_cma = mmz_for_dst;

    tde_hal_node_set_tqt(hardware_node, &stDstDrvSurface, HI_TDE_OUT_ALPHA_FROM_NORM);

    if ((s32Ret = TdeOsiSetNodeFinish(handle, hardware_node, 0, TDE_NODE_SUBM_ALONE)) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetNodeFinish, FAILURE_TAG);
        return s32Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}
#endif

STATIC hi_s32 TdeOsiCheckSingleSr2Para(hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                                       hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_opt *opt)
{
    hi_bool bContainNullPtr = ((NULL == dst_surface) || (NULL == dst_rect) || (NULL == opt) ||
                               (NULL == fore_ground_surface) || (NULL == fore_ground_rect));
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (bContainNullPtr) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, opt);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fore_ground_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fore_ground_rect);
        return HI_ERR_TDE_NULL_PTR;
    }

    if (TdeOsiCheckSingleSrc2Opt(fore_ground_surface->color_fmt, dst_surface->color_fmt, opt) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSingleSrc2Opt, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (opt->resize) {
        if (TdeOsiCheckResizePara(fore_ground_rect->width, fore_ground_rect->height, dst_rect->width,
                                  dst_rect->height) < 0) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckResizePara, FAILURE_TAG);
            return HI_ERR_TDE_MINIFICATION;
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

/*****************************************************************************
 * Function:      TdeOsiSingleSrc2Blit
 * Description:   source2 operate realization
 * Input:         pstBackGround: source2 bitmap info struct
 *                back_ground_rect: source2 bitmap operate zone
 *                dst_surface: target bitmap info struct
 *                dst_rect: target bitmap operate zone
 * Output:        none
 * Return:        success/fail
 * Others:        none
 *****************************************************************************/
hi_s32 TdeOsiSingleSrc2Blit(hi_s32 handle, hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                            hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_opt *opt, hi_bool mmz_for_src,
                            hi_bool mmz_for_dst)
{
    tde_hardware_node *hardware_node = NULL;
    tde_surface_msg stSrcDrvSurface = {0};
    tde_surface_msg stDstDrvSurface = {0};
    tde_scandirection_mode stSrcScanInfo = {0};
    tde_scandirection_mode stDstScanInfo = {0};
    hi_u16 u16Code = 0;
    tde_conv_mode_cmd stConv = {0};
    hi_tde_rect stFGOptArea = {0};
    hi_tde_rect stDstOptArea = {0};
    hi_tde_rect stForeRect = {0};
    hi_tde_rect tmp_dst_rect = {0};
    hi_s32 s32Ret = HI_FAILURE;
    hi_bool bSetFilterNode = HI_FALSE;
    TDE_CLUT_USAGE_E enClutUsage = TDE_CLUT_USAGE_BUTT;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(fore_ground_rect, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_rect, HI_FAILURE);

    TDE_CHECK_NOT_MB_EXCEPT_SP420(dst_surface->color_fmt);

    s32Ret = TdeOsiCheckSingleSr2Para(fore_ground_surface, fore_ground_rect, dst_surface, dst_rect, opt);
    if (HI_SUCCESS != s32Ret) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSingleSr2Para, FAILURE_TAG);
        return s32Ret;
    }

    memcpy(&stForeRect, fore_ground_rect, sizeof(hi_tde_rect));
    memcpy(&tmp_dst_rect, dst_rect, sizeof(hi_tde_rect));

    if (tde_hal_init_node(&hardware_node) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_init_node, FAILURE_TAG);
        return HI_ERR_TDE_NO_MEM;
    }

    if (TdeOsiSetClipPara(NULL, NULL, fore_ground_surface, &stForeRect, dst_surface, &tmp_dst_rect, opt,
                          hardware_node) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetClipPara, FAILURE_TAG);
        return HI_ERR_TDE_CLIP_AREA;
    }

    if (TdeOsiGetScanInfo(fore_ground_surface, &stForeRect, dst_surface, &tmp_dst_rect, opt, &stSrcScanInfo,
                          &stDstScanInfo) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiGetScanInfo, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    TdeOsiConvertSurface(fore_ground_surface, &stForeRect, &stSrcScanInfo, &stSrcDrvSurface, &stFGOptArea);
    stSrcDrvSurface.is_cma = mmz_for_src;
    tde_hal_set_src2(hardware_node, &stSrcDrvSurface);

    TdeOsiConvertSurface(dst_surface, &tmp_dst_rect, &stDstScanInfo, &stDstDrvSurface, &stDstOptArea);

    if (HI_TDE_OUT_ALPHA_FROM_MAX <= opt->out_alpha_from) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_OUT_ALPHA_FROM_MAX);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, opt->out_alpha_from);
        tde_hal_free_node_buf(hardware_node);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (HI_TDE_OUT_ALPHA_FROM_BACKGROUND == opt->out_alpha_from) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_OUT_ALPHA_FROM_BACKGROUND);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, opt->out_alpha_from);
        tde_hal_free_node_buf(hardware_node);
        return HI_ERR_TDE_INVALID_PARA;
    }

    stDstDrvSurface.is_cma = mmz_for_dst;
    tde_hal_node_set_tqt(hardware_node, &stDstDrvSurface, opt->out_alpha_from);

    u16Code = TdeOsiSingleSrc2GetOptCode(fore_ground_surface->color_fmt, dst_surface->color_fmt);

    TdeOsiGetConvbyCode(u16Code, &stConv);

    if (tde_hal_node_set_color_convert(hardware_node, &stConv) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_color_convert, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    if ((s32Ret = TdeOsiSetClutOpt(fore_ground_surface, dst_surface, &enClutUsage, opt->clut_reload, hardware_node)) <
        0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetClutOpt, FAILURE_TAG);
        return s32Ret;
    }

    TdeOsiSetBaseOptParaForBlit(opt, HI_NULL, fore_ground_surface, TDE_OPERATION_SINGLE_SRC2, hardware_node);

    TdeOsiSetExtAlpha(HI_NULL, fore_ground_surface, hardware_node);

    bSetFilterNode = ((opt->resize) || (opt->deflicker_mode != HI_TDE_DEFLICKER_MODE_NONE) ||
                      (fore_ground_surface->color_fmt >= HI_TDE_COLOR_FMT_YCbCr422));

    if (bSetFilterNode) {
        if ((fore_ground_surface->color_fmt >= HI_TDE_COLOR_FMT_YCbCr422) && (!(opt->resize))) {
            fore_ground_rect->height = tmp_dst_rect.height;
            fore_ground_rect->width = tmp_dst_rect.width;
        }
        if ((s32Ret = TdeOsiSetFilterNode(handle, hardware_node, NULL, NULL, fore_ground_surface, fore_ground_rect,
                                          dst_surface, &tmp_dst_rect, opt->deflicker_mode, opt->filter_mode)) < 0) {
            tde_hal_free_node_buf(hardware_node);
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetFilterNode, FAILURE_TAG);
            return s32Ret;
        }
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if ((s32Ret = TdeOsiSetNodeFinish(handle, hardware_node, 0, TDE_NODE_SUBM_ALONE)) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetNodeFinish, FAILURE_TAG);
        return s32Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

#ifndef HI_BUILD_IN_BOOT
STATIC hi_s32 TdeOsiSetColorKey(hi_tde_surface *fore_ground_surface, hi_tde_surface *pstBackGround,
                                tde_hardware_node *hardware_node, hi_tde_color_key color_key_value,
                                hi_tde_color_key_mode color_key_mode, TDE_CLUT_USAGE_E enClutUsage)
{
    tde_color_key_cmd stColorkey;
    tde_color_fmt_category enFmtCategory;
    hi_bool bUnknownFmtCategory;
    hi_bool bColorKeyForegroundBeforeClutMode = (TDE_CLUT_COLOREXPENDING != enClutUsage) &&
                                                (TDE_CLUT_CLUT_BYPASS != enClutUsage);
    hi_bool bSetColorKey = (HI_TDE_COLOR_KEY_MODE_NONE != color_key_mode);

    stColorkey.color_key_value = color_key_value;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (!bSetColorKey) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    switch (color_key_mode) {
        case HI_TDE_COLOR_KEY_MODE_BACKGROUND:
            stColorkey.color_key_mode = TDE_DRV_COLORKEY_BACKGROUND;

            enFmtCategory = TdeOsiGetFmtCategory(pstBackGround->color_fmt);

            break;

        case HI_TDE_COLOR_KEY_MODE_FOREGROUND:

            stColorkey.color_key_mode = (bColorKeyForegroundBeforeClutMode) ? TDE_DRV_COLORKEY_FOREGROUND_AFTER_CLUT
                                                                            : TDE_DRV_COLORKEY_FOREGROUND_BEFORE_CLUT;

            enFmtCategory = TdeOsiGetFmtCategory(fore_ground_surface->color_fmt);

            break;

        default:
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, color_key_mode);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "invalid colorkey mode");
            return HI_ERR_TDE_INVALID_PARA;
    }

    bUnknownFmtCategory = enFmtCategory >= TDE_COLORFMT_CATEGORY_MAX;
    if (bUnknownFmtCategory) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, TDE_COLORFMT_CATEGORY_MAX);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enFmtCategory);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (tde_hal_node_set_colorkey(hardware_node, enFmtCategory, &stColorkey) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_colorkey, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}
#endif

/*****************************************************************************
 * Function:      TdeOsiDoubleSrcBlit
 * Description:   dual source operate realization
 * Input:         pstBackGround: source1 bitmap info struct
 *                back_ground_rect: source1 bitmap operate zone
 *                fore_ground_surface: source2 bitmap info struct
 *                fore_ground_rect: source2 bitmap operate zone
 *                dst_surface: target bitmap info struct
 *                dst_rect: target bitmap operate zone
 *                opt:config parameter
 * Output:        none
 * Return:        success/fail
 *****************************************************************************/
STATIC hi_s32 TdeOsiDoubleSrcBlit(hi_s32 handle, hi_tde_surface *pstBackGround, hi_tde_rect *back_ground_rect,
                                  hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                                  hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_opt *opt)
{
    tde_hardware_node *hardware_node = NULL;
    tde_surface_msg stSrcDrvSurface = {0};
    tde_surface_msg stDstDrvSurface = {0};
    tde_scandirection_mode stSrcScanInfo = {0};
    tde_scandirection_mode stDstScanInfo = {0};
    hi_u16 u16Code = 0;
    hi_tde_rect stBGOptArea = {0};
    hi_tde_rect stFGOptArea = {0};
    hi_tde_rect stDstOptArea = {0};
    hi_s32 s32Ret = HI_FAILURE;
    tde_conv_mode_cmd stConv = {0};
    hi_tde_surface stTempSur = {0};
    hi_tde_rect stForeRect = {0};
    hi_tde_rect tmp_dst_rect = {0};
    hi_tde_rect stBackRect = {0};
    hi_bool bSetFilterNode = HI_FALSE;

    TDE_CLUT_USAGE_E enClutUsage = TDE_CLUT_USAGE_BUTT;

    hi_bool bContainNullPtr = ((NULL == pstBackGround) || (NULL == back_ground_rect) || (NULL == fore_ground_surface) ||
                               (NULL == fore_ground_rect) || (NULL == dst_surface) || (NULL == dst_rect) ||
                               (NULL == opt));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (bContainNullPtr) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstBackGround);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, back_ground_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fore_ground_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fore_ground_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, opt);
        return HI_ERR_TDE_NULL_PTR;
    }

    TDE_CHECK_NOT_MB(dst_surface->color_fmt);

    memcpy(&stForeRect, fore_ground_rect, sizeof(hi_tde_rect));
    memcpy(&stBackRect, back_ground_rect, sizeof(hi_tde_rect));
    memcpy(&tmp_dst_rect, dst_rect, sizeof(hi_tde_rect));

    if (TdeOsiCheckDoubleSrcOpt(pstBackGround->color_fmt, fore_ground_surface->color_fmt, dst_surface->color_fmt, opt) <
        0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckDoubleSrcOpt, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (opt->resize) {
        if (TdeOsiCheckResizePara(stForeRect.width, stForeRect.height, tmp_dst_rect.width, tmp_dst_rect.height) < 0) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckResizePara, FAILURE_TAG);
            return HI_ERR_TDE_MINIFICATION;
        }
    }

    if (tde_hal_init_node(&hardware_node) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_init_node, FAILURE_TAG);
        return HI_ERR_TDE_NO_MEM;
    }

    if (TdeOsiSetClipPara(pstBackGround, &stBackRect, fore_ground_surface, &stForeRect, dst_surface, &tmp_dst_rect, opt,
                          hardware_node) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetClipPara, FAILURE_TAG);
        return HI_ERR_TDE_CLIP_AREA;
    }

    if (TdeOsiGetScanInfo(fore_ground_surface, &stForeRect, dst_surface, &tmp_dst_rect, opt, &stSrcScanInfo,
                          &stDstScanInfo) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiGetScanInfo, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    TdeOsiConvertSurface(pstBackGround, &stBackRect, &stSrcScanInfo, &stSrcDrvSurface, &stBGOptArea);

    memcpy(&stTempSur, pstBackGround, sizeof(hi_tde_surface));

    tde_hal_set_src1(hardware_node, &stSrcDrvSurface);

    TdeOsiConvertSurface(fore_ground_surface, &stForeRect, &stSrcScanInfo, &stSrcDrvSurface, &stFGOptArea);

    tde_hal_set_src2(hardware_node, &stSrcDrvSurface);

    TdeOsiConvertSurface(dst_surface, &tmp_dst_rect, &stDstScanInfo, &stDstDrvSurface, &stDstOptArea);

    if (HI_TDE_OUT_ALPHA_FROM_MAX <= opt->out_alpha_from) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_OUT_ALPHA_FROM_MAX);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, opt->out_alpha_from);
        tde_hal_free_node_buf(hardware_node);
        return HI_ERR_TDE_INVALID_PARA;
    }

    tde_hal_node_set_tqt(hardware_node, &stDstDrvSurface, opt->out_alpha_from);

    u16Code = TdeOsiDoubleSrcGetOptCode(stTempSur.color_fmt, fore_ground_surface->color_fmt, dst_surface->color_fmt);

    TdeOsiGetConvbyCode(u16Code, &stConv);

    if (tde_hal_node_set_color_convert(hardware_node, &stConv) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_color_convert, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    if ((s32Ret = TdeOsiSetClutOpt(fore_ground_surface, dst_surface, &enClutUsage, opt->clut_reload, hardware_node)) <
        0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetClutOpt, FAILURE_TAG);
        return s32Ret;
    }

#ifndef HI_BUILD_IN_BOOT
    s32Ret = TdeOsiSetColorKey(fore_ground_surface, pstBackGround, hardware_node, opt->color_key_value,
                               opt->color_key_mode, enClutUsage);
    if (s32Ret) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetColorKey, FAILURE_TAG);
        return s32Ret;
    }
#endif

    TdeOsiSetBaseOptParaForBlit(opt, &stTempSur, fore_ground_surface, TDE_OPERATION_DOUBLE_SRC, hardware_node);
    TdeOsiSetExtAlpha(&stTempSur, fore_ground_surface, hardware_node);

    bSetFilterNode = ((opt->resize) || (HI_TDE_DEFLICKER_MODE_NONE != opt->deflicker_mode) ||
                      (fore_ground_surface->color_fmt >= HI_TDE_COLOR_FMT_YCbCr422) ||
                      (pstBackGround->color_fmt >= HI_TDE_COLOR_FMT_YCbCr422));

    if (bSetFilterNode) {
        back_ground_rect->height = tmp_dst_rect.height;
        back_ground_rect->width = tmp_dst_rect.width;
        if ((fore_ground_surface->color_fmt >= HI_TDE_COLOR_FMT_YCbCr422) && (!(opt->resize))) {
            fore_ground_rect->height = tmp_dst_rect.height;
            fore_ground_rect->width = tmp_dst_rect.width;
        }
        if ((s32Ret = TdeOsiSetFilterNode(handle, hardware_node, pstBackGround, back_ground_rect, fore_ground_surface,
                                          &stForeRect, dst_surface, &tmp_dst_rect, opt->deflicker_mode,
                                          opt->filter_mode)) < 0) {
            tde_hal_free_node_buf(hardware_node);
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetFilterNode, FAILURE_TAG);
            return s32Ret;
        }
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_SUCCESS;
    }

    if ((s32Ret = TdeOsiSetNodeFinish(handle, hardware_node, 0, TDE_NODE_SUBM_ALONE)) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetNodeFinish, FAILURE_TAG);
        return s32Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

/*****************************************************************************
 * Function:      drv_tde_blit
 * Description:   operate pstBackGround with fore_ground_surface,which result output to dst_surface,operate setting is
 *in opt Input:         enCmd: TDE operate command type pstBackGround: background bitmap info struct
 *                fore_ground_surface: foreground bitmap info struct
 *                dst_surface:  target bitmap info struct
 *                opt:  operate parameter setting struct
 * Output:        none
 * Return:        none
 * Others:        none
 *****************************************************************************/
hi_s32 drv_tde_blit(hi_s32 handle, hi_tde_surface *pstBackGround, hi_tde_rect *back_ground_rect,
                    hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect, hi_tde_surface *dst_surface,
                    hi_tde_rect *dst_rect, hi_tde_opt *opt)
{
#ifndef HI_BUILD_IN_BOOT
    TDE_OPERATION_CATEGORY_E enOptCategory = TDE_OPERATION_BUTT;
#endif
    hi_s32 s32Ret = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

#ifndef HI_BUILD_IN_BOOT
    enOptCategory = TdeOsiGetOptCategory(pstBackGround, back_ground_rect, fore_ground_surface, fore_ground_rect,
                                         dst_surface, dst_rect, opt);
    switch (enOptCategory) {
        case TDE_OPERATION_SINGLE_SRC1: {
            if (NULL == pstBackGround) {
                return TdeOsiSingleSrc1Blit(handle, fore_ground_surface, fore_ground_rect, dst_surface, dst_rect,
                                            HI_FALSE, HI_FALSE);
            } else {
                return TdeOsiSingleSrc1Blit(handle, pstBackGround, back_ground_rect, dst_surface, dst_rect, HI_FALSE,
                                            HI_FALSE);
            }
            break;
        }
        case TDE_OPERATION_SINGLE_SRC2: {
            if (NULL == pstBackGround) {
                if ((s32Ret = TdeOsiSingleSrc2Blit(handle, fore_ground_surface, fore_ground_rect, dst_surface, dst_rect,
                                                   opt, HI_FALSE, HI_FALSE)) < 0) {
                    GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSingleSrc2Blit, FAILURE_TAG);
                    return s32Ret;
                }
            } else {
                if ((s32Ret = TdeOsiSingleSrc2Blit(handle, pstBackGround, back_ground_rect, dst_surface, dst_rect, opt,
                                                   HI_FALSE, HI_FALSE)) < 0) {
                    GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSingleSrc2Blit, FAILURE_TAG);
                    return s32Ret;
                }
            }
            break;
        }
        case TDE_OPERATION_DOUBLE_SRC: {
#endif
            if ((s32Ret = TdeOsiDoubleSrcBlit(handle, pstBackGround, back_ground_rect, fore_ground_surface,
                                              fore_ground_rect, dst_surface, dst_rect, opt)) < 0) {
                GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiDoubleSrcBlit, FAILURE_TAG);
                return s32Ret;
            }
#ifndef HI_BUILD_IN_BOOT
            break;
        }
        default:
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enOptCategory);
            return HI_ERR_TDE_INVALID_PARA;
    }
#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}
#ifndef HI_BUILD_IN_BOOT
EXPORT_SYMBOL(drv_tde_blit);
#endif

STATIC hi_s32 TdeOsiCheckMbBlitPara(hi_tde_mb_surface *mb_surface, hi_tde_rect *mb_rect, hi_tde_surface *dst_surface,
                                    hi_tde_rect *dst_rect, hi_tde_mb_opt *mb_opt)
{
    hi_bool bContainNullPtr = ((HI_NULL == mb_surface) || (HI_NULL == mb_rect) || (HI_NULL == dst_surface) ||
                               (HI_NULL == dst_rect) || (HI_NULL == mb_opt));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (bContainNullPtr) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, mb_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, mb_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, mb_opt);
        return HI_ERR_TDE_NULL_PTR;
    }

#ifndef HI_BUILD_IN_BOOT
    TDE_CHECK_NOT_MB(dst_surface->color_fmt);
    TDE_CHECK_MBCOLORFMT(mb_surface->mb_color_fmt);

    if (TdeOsiCheckSurfaceEX(dst_surface, dst_rect) < 0 || TdeOsiCheckMbSurfaceEX(mb_surface, mb_rect) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurfaceEX, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

#endif

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

STATIC hi_s32 TdeOsiGetDoubleCycleData(hi_tde_rect *mb_rect, hi_u32 *i, hi_u32 *j)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((mb_rect->height > TDE_MAX_RECT_HEIGHT) && (mb_rect->width > TDE_MAX_RECT_WIDTH)) {
        *i = 2;
        *j = 2;
    } else if ((mb_rect->height > TDE_MAX_RECT_HEIGHT) && (mb_rect->width <= TDE_MAX_RECT_WIDTH)) {
        *i = 1;
        *j = 2;

    } else if ((mb_rect->height <= TDE_MAX_RECT_HEIGHT) && (mb_rect->width > TDE_MAX_RECT_WIDTH)) {
        *i = 2;
        *j = 1;
    } else {
        *i = 1;
        *j = 1;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

/*****************************************************************************
 * Function:      TdeOsiMbBlit
 * Description:   Mb blit
 * Input:         handle: task handle
 *                mb_surface: MB bitmap information struct
 *                mb_rect: MB bitmap operate zone
 *                dst_surface: target bitmap information struct
 *                dst_rect:  target bitmap operate zone
 *                mb_opt:  operate parameter setting struct
 * Output:        none
 * Return:        >0: return task id of current operate; <0: fail
 * Others:        none
 *****************************************************************************/
hi_s32 tde_mb_blit(hi_s32 handle, hi_tde_mb_surface *mb_surface, hi_tde_rect *mb_rect, hi_tde_surface *dst_surface,
                   hi_tde_rect *dst_rect, hi_tde_mb_opt *mb_opt)
{
    hi_u32 phy, y_addr, cbcr_phy_addr, u32BytePerPixel, u32CbCrBytePerPixel;
    hi_u32 height, width, i, j, m, n;
    hi_s32 sRet = HI_SUCCESS, s32Bpp = 0;
    hi_u32 u32CbCrHeight = 0;
    hi_bool bTwoBytePerPixel = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    sRet = TdeOsiCheckMbBlitPara(mb_surface, mb_rect, dst_surface, dst_rect, mb_opt);
    if (sRet != HI_SUCCESS) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckMbBlitPara, FAILURE_TAG);
        return sRet;
    }

    if ((dst_rect->height == mb_rect->height) && (dst_rect->width == mb_rect->width)) {
        mb_opt->resize_en = HI_TDE_MB_RESIZE_NONE;
    }

    if (HI_TDE_MB_RESIZE_NONE == mb_opt->resize_en) {
        TDE_UNIFY_RECT(mb_rect, dst_rect);
    }

    if (TdeOsiCheckResizePara(mb_rect->width, mb_rect->height, dst_rect->width, dst_rect->height) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckResizePara, FAILURE_TAG);
        return HI_ERR_TDE_MINIFICATION;
    }

    height = mb_rect->height;
    width = mb_rect->width;
    y_addr = mb_surface->y_addr;
    cbcr_phy_addr = mb_surface->cbcr_phy_addr;
    phy = dst_surface->phy_addr;

    bTwoBytePerPixel = ((mb_surface->mb_color_fmt == HI_TDE_MB_COLOR_FMT_JPG_YCbCr444MBP) ||
                        (mb_surface->mb_color_fmt == HI_TDE_MB_COLOR_FMT_JPG_YCbCr422MBVP));

    u32CbCrBytePerPixel = bTwoBytePerPixel ? 2 : 1;

    s32Bpp = TdeOsiGetbppByFmt(dst_surface->color_fmt);
    if (s32Bpp <= 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiGetbppByFmt, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }
    u32BytePerPixel = ((hi_u32)s32Bpp >> 3);

    u32CbCrHeight = TDE_MAX_SLICE_RECT_HEIGHT;
    if ((mb_surface->mb_color_fmt == HI_TDE_MB_COLOR_FMT_MP1_YCbCr420MBP) ||
        (mb_surface->mb_color_fmt == HI_TDE_MB_COLOR_FMT_MP2_YCbCr420MBP) ||
        (mb_surface->mb_color_fmt == HI_TDE_MB_COLOR_FMT_MP2_YCbCr420MBI) ||
        (mb_surface->mb_color_fmt == HI_TDE_MB_COLOR_FMT_JPG_YCbCr420MBP) ||
        (mb_surface->mb_color_fmt == HI_TDE_MB_COLOR_FMT_JPG_YCbCr422MBVP)) {
        u32CbCrHeight = TDE_MAX_SLICE_RECT_HEIGHT / 2;
    }

    TdeOsiGetDoubleCycleData(mb_rect, &i, &j);

    for (n = 0; n < j; n++) {
        for (m = 0; m < i; m++) {
            if ((i - 1) == m) {
                mb_rect->width = width - m * TDE_MAX_SLICE_RECT_WIDTH;
            } else {
                mb_rect->width = TDE_MAX_SLICE_RECT_WIDTH;
            }

            if (height > TDE_MAX_SLICE_RECT_HEIGHT) {
                mb_rect->height = TDE_MAX_SLICE_RECT_HEIGHT;
            }

            if ((j - 1) == n) {
                mb_rect->height = height - n * TDE_MAX_SLICE_RECT_HEIGHT;
            } else {
                mb_rect->height = TDE_MAX_SLICE_RECT_HEIGHT;
            }
            if ((1 != i) || (1 != j)) {
                dst_rect->width = mb_rect->width;
                dst_rect->height = mb_rect->height;
            }
            mb_surface->y_addr = y_addr + m * TDE_MAX_SLICE_RECT_WIDTH +
                                 n * mb_surface->y_stride * TDE_MAX_SLICE_RECT_HEIGHT;
            dst_surface->phy_addr = phy + m * u32BytePerPixel * TDE_MAX_SLICE_RECT_WIDTH +
                                    n * dst_surface->stride * TDE_MAX_SLICE_RECT_HEIGHT;
            mb_surface->cbcr_phy_addr = cbcr_phy_addr + m * u32CbCrBytePerPixel * TDE_MAX_SLICE_RECT_WIDTH +
                                        n * mb_surface->cbcr_stride * u32CbCrHeight;

            if ((sRet = TdeOsiSetMbPara(handle, mb_surface, mb_rect, dst_surface, dst_rect, mb_opt)) < 0) {
                GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetMbPara, FAILURE_TAG);
                return sRet;
            }
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}
#ifndef HI_BUILD_IN_BOOT
EXPORT_SYMBOL(tde_mb_blit);

/*****************************************************************************
 * Function:      TdeOsiBitmapMaskCheckPara
 * Description:   check for trinal source operate parameter
 * Input:         pstBackGround: background bitmap
 *                back_ground_rect: background operate zone
 *                fore_ground_surface: foreground bitmap
 *                fore_ground_rect: foreground operate zone
 *                mask_surface:  Mask bitmap
 *                mask_rect:  Mask bitmap operate zone
 *                dst_surface:  target bitmap
 *                dst_rect: target bitmap operate zone
 * Output:        none
 * Return:        =0: success; <0: fail
 * Others:        none
 *****************************************************************************/
STATIC INLINE hi_s32 TdeOsiBitmapMaskCheckPara(hi_tde_surface *pstBackGround, hi_tde_rect *back_ground_rect,
                                               hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                                               hi_tde_surface *mask_surface, hi_tde_rect *mask_rect,
                                               hi_tde_surface *dst_surface, hi_tde_rect *dst_rect)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((NULL == pstBackGround) || (NULL == back_ground_rect) || (NULL == fore_ground_surface) ||
        (NULL == fore_ground_rect) || (NULL == mask_surface) || (NULL == mask_rect) || (NULL == dst_surface) ||
        (NULL == dst_rect)) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstBackGround);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, back_ground_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fore_ground_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fore_ground_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, mask_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, mask_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_rect);
        return HI_ERR_TDE_NULL_PTR;
    }

    TDE_CHECK_NOT_MB(fore_ground_surface->color_fmt);
    TDE_CHECK_NOT_MB(pstBackGround->color_fmt);
    TDE_CHECK_NOT_MB(mask_surface->color_fmt);
    TDE_CHECK_NOT_MB(dst_surface->color_fmt);

    if (TdeOsiCheckSurface(pstBackGround, back_ground_rect) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (TdeOsiCheckSurface(fore_ground_surface, fore_ground_rect) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (TdeOsiCheckSurface(mask_surface, mask_rect) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (TdeOsiCheckSurface(dst_surface, dst_rect) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if ((back_ground_rect->width != fore_ground_rect->width) || (fore_ground_rect->width != mask_rect->width) ||
        (mask_rect->width != dst_rect->width) || (back_ground_rect->height != fore_ground_rect->height) ||
        (fore_ground_rect->height != mask_rect->height) || (mask_rect->height != dst_rect->height)) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, back_ground_rect->width);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, back_ground_rect->height);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, fore_ground_rect->width);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, fore_ground_rect->height);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, mask_rect->width);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, mask_rect->height);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Don't support scale");
        return HI_ERR_TDE_INVALID_PARA;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

#ifdef CONFIG_TDE_MASKROP_SUPPORT
/*****************************************************************************
 * Function:      tde_hal_bitmap_mask_rop
 * Description:   Ropmask foreground and mask at firs, and then ropmask background and middle bitmap
 *                output result to target bitmap
 * Input:         pstBackGround: background bitmap info struct
 *                fore_ground_surface: foreground bitmap info struct
 *                pMask: fask code operate bitmap info
 *                dst_surface:  target bitmap info
 *                func_complete_callback: callback function pointer when operate is over;if null, to say to no need to
 *notice Output:        none Return:        none Others:        none
 *****************************************************************************/
hi_s32 tde_hal_bitmap_mask_rop(hi_s32 handle, hi_tde_surface *pstBackGround, hi_tde_rect *back_ground_rect,
                               hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                               hi_tde_surface *mask_surface, hi_tde_rect *mask_rect, hi_tde_surface *dst_surface,
                               hi_tde_rect *dst_rect, hi_tde_rop_mode rop_color, hi_tde_rop_mode rop_alpha)
{
    tde_hardware_node *hardware_node = NULL;
    tde_hardware_node *pstHWNodePass2 = NULL;
    tde_surface_msg stSrcDrvSurface = {0};
    tde_surface_msg stDstDrvSurface = {0};
    tde_scandirection_mode stScanInfo = {0};
    hi_tde_surface stMidSurface = {0};
    hi_tde_rect stMidRect = {0};
    hi_u16 u16Code = 0;
    tde_conv_mode_cmd stConv = {0};
    hi_s32 ret = HI_SUCCESS;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((ret = TdeOsiBitmapMaskCheckPara(pstBackGround, back_ground_rect, fore_ground_surface, fore_ground_rect,
                                         mask_surface, mask_rect, dst_surface, dst_rect)) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiBitmapMaskCheckPara, FAILURE_TAG);
        return ret;
    }

    if (!TdeOsiWhetherContainAlpha(fore_ground_surface->color_fmt)) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiWhetherContainAlpha, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (HI_TDE_COLOR_FMT_A1 != mask_surface->color_fmt) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_COLOR_FMT_A1);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, mask_surface->color_fmt);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (tde_hal_init_node(&hardware_node) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_init_node, FAILURE_TAG);
        return HI_ERR_TDE_NO_MEM;
    }

    stScanInfo.hor_scan = TDE_SCAN_LEFT_RIGHT;
    stScanInfo.ver_scan = TDE_SCAN_UP_DOWN;

    TdeOsiConvertSurface(fore_ground_surface, fore_ground_rect, &stScanInfo, &stSrcDrvSurface, NULL);

    tde_hal_set_src1(hardware_node, &stSrcDrvSurface);

    TdeOsiConvertSurface(mask_surface, mask_rect, &stScanInfo, &stSrcDrvSurface, NULL);

    tde_hal_set_src2(hardware_node, &stSrcDrvSurface);

    memcpy(&stMidSurface, fore_ground_surface, sizeof(hi_tde_surface));

    stMidSurface.phy_addr = tde_list_get_phy_buf(0, stMidSurface.stride * stMidSurface.height);
    if (0 == stMidSurface.phy_addr) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, stMidSurface.phy_addr);
        tde_hal_free_node_buf(hardware_node);
        return HI_ERR_TDE_NO_MEM;
    }
    stMidSurface.clut_phy_addr = 0;

    stMidRect.pos_x = 0;
    stMidRect.pos_y = 0;
    stMidRect.height = fore_ground_rect->height;
    stMidRect.width = fore_ground_rect->width;

    TdeOsiConvertSurface(&stMidSurface, &stMidRect, &stScanInfo, &stDstDrvSurface, NULL);

    tde_hal_node_set_tqt(hardware_node, &stDstDrvSurface, HI_TDE_OUT_ALPHA_FROM_NORM);

    TdeOsiSetExtAlpha(fore_ground_surface, &stMidSurface, hardware_node);

    /* logical operation first passs */
    if (tde_hal_node_set_base_operate(hardware_node, TDE_NORM_BLIT_2OPT, TDE_ALU_MASK_ROP1, 0) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_base_operate, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    if ((ret = TdeOsiSetNodeFinish(handle, hardware_node, 0, TDE_NODE_SUBM_ALONE)) < 0) {
        tde_list_put_phy_buf(1);
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetNodeFinish, FAILURE_TAG);
        return ret;
    }

    if (tde_hal_init_node(&pstHWNodePass2) < 0) {
        tde_list_put_phy_buf(1);
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_init_node, FAILURE_TAG);
        return HI_ERR_TDE_NO_MEM;
    }

    TdeOsiConvertSurface(pstBackGround, back_ground_rect, &stScanInfo, &stSrcDrvSurface, NULL);

    tde_hal_set_src1(pstHWNodePass2, &stSrcDrvSurface);

    tde_hal_set_src2(pstHWNodePass2, &stDstDrvSurface);

    TdeOsiConvertSurface(dst_surface, dst_rect, &stScanInfo, &stDstDrvSurface, NULL);

    tde_hal_node_set_tqt(pstHWNodePass2, &stDstDrvSurface, HI_TDE_OUT_ALPHA_FROM_NORM);

    u16Code = TdeOsiSingleSrc2GetOptCode(fore_ground_surface->color_fmt, dst_surface->color_fmt);

    TdeOsiGetConvbyCode(u16Code, &stConv);

    if (tde_hal_node_set_color_convert(pstHWNodePass2, &stConv) < 0) {
        tde_hal_free_node_buf(hardware_node);
        tde_hal_free_node_buf(pstHWNodePass2);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_color_convert, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    if (tde_hal_node_set_rop(pstHWNodePass2, rop_color, rop_alpha) < 0) {
        tde_hal_free_node_buf(hardware_node);
        tde_hal_free_node_buf(pstHWNodePass2);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_rop, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }
    TdeOsiSetExtAlpha(pstBackGround, &stMidSurface, pstHWNodePass2);

    /* logical operation second passs */
    if (tde_hal_node_set_base_operate(pstHWNodePass2, TDE_NORM_BLIT_2OPT, TDE_ALU_MASK_ROP2, 0) < 0) {
        tde_hal_free_node_buf(hardware_node);
        tde_hal_free_node_buf(pstHWNodePass2);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_base_operate, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    ret = TdeOsiSetNodeFinish(handle, pstHWNodePass2, 1, TDE_NODE_SUBM_ALONE);
    if (ret < 0) {
        tde_hal_free_node_buf(hardware_node);
        tde_hal_free_node_buf(pstHWNodePass2);
        tde_list_put_phy_buf(1);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetNodeFinish, FAILURE_TAG);
        return ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}
EXPORT_SYMBOL(tde_hal_bitmap_mask_rop);
#endif

#ifdef CONFIG_TDE_MASKBLEND_SUPPORT
/*****************************************************************************
 * Function:      tde_hal_bitmap_mask_blend
 * Description:   blendmask foreground and mask bitmap at first,and blend background with middle bitmap
 *                output reslut to target bitmap
 * Input:         pstBackGround: background bitmap info struct
 *                fore_ground_surface: foreground bitmap info struct
 *                pMask: fask code operate bitmap info
 *                dst_surface:  target bitmap info
 *                alpha:  alpha value operated
 *                func_complete_callback: callback function pointer when operate is over;if null, to say to no need to
 *notice Output:        none Return:        none Others:        none
 *****************************************************************************/
hi_s32 tde_hal_bitmap_mask_blend(hi_s32 handle, hi_tde_surface *pstBackGround, hi_tde_rect *back_ground_rect,
                                 hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                                 hi_tde_surface *mask_surface, hi_tde_rect *mask_rect, hi_tde_surface *dst_surface,
                                 hi_tde_rect *dst_rect, hi_u8 alpha, hi_tde_alpha_blending blend_mode)
{
    tde_hardware_node *hardware_node = NULL;
    tde_hardware_node *pstHWNodePass2 = NULL;
    tde_surface_msg stSrcDrvSurface = {0};
    tde_surface_msg stDstDrvSurface = {0};
    tde_scandirection_mode stScanInfo = {0};
    hi_tde_surface stMidSurface = {0};
    hi_tde_rect stMidRect = {0};
    tde_alu_mode enDrvAluMode = TDE_SRC1_BYPASS;
    hi_u16 u16Code = 0;
    tde_conv_mode_cmd stConv = {0};
    hi_tde_blend_opt blend_opt = {0};
    hi_s32 ret = HI_SUCCESS;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((ret = TdeOsiBitmapMaskCheckPara(pstBackGround, back_ground_rect, fore_ground_surface, fore_ground_rect,
                                         mask_surface, mask_rect, dst_surface, dst_rect)) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiBitmapMaskCheckPara, FAILURE_TAG);
        return ret;
    }

    if ((HI_TDE_COLOR_FMT_A1 != mask_surface->color_fmt) && (HI_TDE_COLOR_FMT_A8 != mask_surface->color_fmt)) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HI_TDE_COLOR_FMT_A1);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HI_TDE_COLOR_FMT_A8);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, mask_surface->color_fmt);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (HI_TDE_ALPHA_BLENDING_BLEND == blend_mode) {
        enDrvAluMode = TDE_ALU_BLEND;
    } else {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Alum mode can only be blending in tde_hal_bitmap_mask_blend");
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (tde_hal_init_node(&hardware_node) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_init_node, FAILURE_TAG);
        return HI_ERR_TDE_NO_MEM;
    }

    stScanInfo.hor_scan = TDE_SCAN_LEFT_RIGHT;
    stScanInfo.ver_scan = TDE_SCAN_UP_DOWN;

    TdeOsiConvertSurface(fore_ground_surface, fore_ground_rect, &stScanInfo, &stSrcDrvSurface, NULL);

    tde_hal_set_src1(hardware_node, &stSrcDrvSurface);

    TdeOsiConvertSurface(mask_surface, mask_rect, &stScanInfo, &stSrcDrvSurface, NULL);

    tde_hal_set_src2(hardware_node, &stSrcDrvSurface);

    memcpy(&stMidSurface, fore_ground_surface, sizeof(hi_tde_surface));

    stMidSurface.phy_addr = tde_list_get_phy_buf(0, stMidSurface.stride * stMidSurface.height);
    if (0 == stMidSurface.phy_addr) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, stMidSurface.phy_addr);
        tde_hal_free_node_buf(hardware_node);
        return HI_ERR_TDE_NO_MEM;
    }
    stMidRect.pos_x = 0;
    stMidRect.pos_y = 0;
    stMidRect.height = fore_ground_rect->height;
    stMidRect.width = fore_ground_rect->width;

    TdeOsiConvertSurface(&stMidSurface, &stMidRect, &stScanInfo, &stDstDrvSurface, NULL);

    tde_hal_node_set_tqt(hardware_node, &stDstDrvSurface, HI_TDE_OUT_ALPHA_FROM_NORM);

    TdeOsiSetExtAlpha(fore_ground_surface, &stMidSurface, hardware_node);

    if (tde_hal_node_set_base_operate(hardware_node, TDE_NORM_BLIT_2OPT, TDE_ALU_MASK_BLEND, 0) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_base_operate, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    tde_hal_node_set_global_alpha(hardware_node, 0xff, HI_TRUE);

    if ((ret = TdeOsiSetNodeFinish(handle, hardware_node, 0, TDE_NODE_SUBM_ALONE)) < 0) {
        tde_list_put_phy_buf(1);
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetNodeFinish, FAILURE_TAG);
        return ret;
    }

    if (tde_hal_init_node(&pstHWNodePass2) < 0) {
        tde_list_put_phy_buf(1);
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_init_node, FAILURE_TAG);
        return HI_ERR_TDE_NO_MEM;
    }
    TdeOsiConvertSurface(pstBackGround, back_ground_rect, &stScanInfo, &stSrcDrvSurface, NULL);

    tde_hal_set_src1(pstHWNodePass2, &stSrcDrvSurface);

    tde_hal_set_src2(pstHWNodePass2, &stDstDrvSurface);

    TdeOsiConvertSurface(dst_surface, dst_rect, &stScanInfo, &stDstDrvSurface, NULL);

    tde_hal_node_set_tqt(pstHWNodePass2, &stDstDrvSurface, HI_TDE_OUT_ALPHA_FROM_NORM);

    u16Code = TdeOsiSingleSrc2GetOptCode(fore_ground_surface->color_fmt, dst_surface->color_fmt);

    TdeOsiGetConvbyCode(u16Code, &stConv);

    if (tde_hal_node_set_color_convert(pstHWNodePass2, &stConv) < 0) {
        tde_hal_free_node_buf(hardware_node);
        tde_hal_free_node_buf(pstHWNodePass2);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_color_convert, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    tde_hal_node_set_global_alpha(pstHWNodePass2, alpha, HI_TRUE);

    if (tde_hal_node_set_base_operate(pstHWNodePass2, TDE_NORM_BLIT_2OPT, enDrvAluMode, 0) < 0) {
        tde_hal_free_node_buf(hardware_node);
        tde_hal_free_node_buf(pstHWNodePass2);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_base_operate, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    blend_opt.global_alpha_en = HI_TRUE;
    blend_opt.pixel_alpha_en = HI_TRUE;
    blend_opt.blend_cmd = HI_TDE_BLEND_CMD_NONE;
    if (tde_hal_node_set_blend(pstHWNodePass2, &blend_opt) < 0) {
        tde_hal_free_node_buf(hardware_node);
        tde_hal_free_node_buf(pstHWNodePass2);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_blend, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    TdeOsiSetExtAlpha(pstBackGround, &stMidSurface, pstHWNodePass2);

    ret = TdeOsiSetNodeFinish(handle, pstHWNodePass2, 1, TDE_NODE_SUBM_ALONE);
    if (ret < 0) {
        tde_hal_free_node_buf(hardware_node);
        tde_hal_free_node_buf(pstHWNodePass2);
        tde_list_put_phy_buf(1);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetNodeFinish, FAILURE_TAG);
        return ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}
EXPORT_SYMBOL(tde_hal_bitmap_mask_blend);
#endif

/*****************************************************************************
 * Function:      tde_hal_solid_draw
 * Description:   operate src1 with src2, which result to dst_surface,operate setting is in opt
 *                if src is MB, only support single source operate, just to say to only support pstBackGround or
 *fore_ground_surface Input:         handle: task handle pSrc: background bitmap info struct dst_surface: foreground
 *bitmap info struct fill_color:  fill  color opt: operate parameter setting struct Output:        none Return:
 *HI_SUCCESS/HI_FAILURE Others:        none
 *****************************************************************************/
hi_s32 tde_hal_solid_draw(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                          hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_fill_color *fill_color,
                          hi_tde_opt *opt)
{
    hi_s32 Ret = HI_SUCCESS;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (NULL == src_surface) {
        Ret = TdeOsi1SourceFill(handle, dst_surface, dst_rect, fill_color, opt);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return Ret;
    } else {
        if ((NULL == src_surface) || (NULL == dst_surface) || (NULL == fill_color)) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, src_surface);
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_surface);
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fill_color);
            return HI_ERR_TDE_NULL_PTR;
        }

        TDE_CHECK_NOT_MB(src_surface->color_fmt);
        TDE_CHECK_NOT_MB(dst_surface->color_fmt);
        TDE_CHECK_NOT_MB(fill_color->color_fmt);
        Ret = TdeOsi2SourceFill(handle, src_surface, src_rect, dst_surface, dst_rect, fill_color, opt);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return Ret;
    }
}
EXPORT_SYMBOL(tde_hal_solid_draw);

hi_s32 drv_tde_set_deflicker_level(hi_tde_deflicker_level deflicker_level)
{
    hi_s32 Ret = HI_SUCCESS;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    Ret = tde_hal_set_deflicer_level(deflicker_level);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}
EXPORT_SYMBOL(drv_tde_set_deflicker_level);

hi_s32 TdeOsiGetDeflickerLevel(hi_tde_deflicker_level *deflicker_level)
{
    hi_s32 Ret = HI_SUCCESS;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (HI_NULL == deflicker_level) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, deflicker_level);
        return HI_FAILURE;
    }
    Ret = tde_hal_get_deflicer_level(deflicker_level);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}
EXPORT_SYMBOL(TdeOsiGetDeflickerLevel);

#endif

hi_s32 tde_set_alpha_threshold_value(hi_u8 threshold_value)
{
    hi_s32 Ret = HI_SUCCESS;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    Ret = tde_hal_set_alpha_threshold(threshold_value);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}
#ifndef HI_BUILD_IN_BOOT
EXPORT_SYMBOL(tde_set_alpha_threshold_value);
#endif

hi_s32 tde_set_alpha_threshold_state(hi_bool alpha_threshold_en)
{
    hi_s32 Ret = HI_SUCCESS;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    Ret = tde_hal_set_alpha_threshold_state(alpha_threshold_en);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}

#ifndef HI_BUILD_IN_BOOT
EXPORT_SYMBOL(tde_set_alpha_threshold_state);

hi_s32 tde_get_alpha_threshold_value(hi_u8 *threshold_value)
{
    hi_s32 Ret = HI_SUCCESS;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (HI_NULL == threshold_value) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, threshold_value);
        return HI_FAILURE;
    }
    Ret = tde_hal_get_alpha_threshold(threshold_value);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}
EXPORT_SYMBOL(tde_get_alpha_threshold_value);

hi_s32 tde_get_alpha_threshold_state(hi_bool *alpha_threshold_en)
{
    hi_s32 Ret = HI_SUCCESS;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    if (HI_NULL == alpha_threshold_en) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, alpha_threshold_en);
        return HI_FAILURE;
    }
    Ret = tde_hal_get_alpha_threshold_state(alpha_threshold_en);
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return Ret;
}
/*****************************************************************************
* Function:      TdeOsiCheckSingleSrcPatternOpt
* Description:   check if single source mode fill operate is valid
* Input:         enSrcFmt foreground pixel format
                 enDstFmt  target pixel format
                 opt    operate attibute pointer
* Output:        none
* Return:        0  valid parameter
                 -1 invalid parameter
* Others:        none
*****************************************************************************/
hi_s32 TdeOsiCheckSingleSrcPatternOpt(hi_tde_color_fmt enSrcFmt, hi_tde_color_fmt enDstFmt,
                                      hi_tde_pattern_fill_opt *opt)
{
    TDE_COLORFMT_TRANSFORM_E enColorTransType;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    enColorTransType = TdeOsiGetFmtTransType(enSrcFmt, enDstFmt);
    if (TDE_COLORFMT_TRANSFORM_BUTT == enColorTransType) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, TDE_COLORFMT_TRANSFORM_BUTT);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enColorTransType);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Unkown color transport type");
        return -1;
    }

    if (opt != HI_NULL) {
        TDE_CHECK_COLORKEYMODE(opt->color_key_mode);

        if (HI_TDE_COLOR_KEY_MODE_NONE != opt->color_key_mode) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_COLOR_KEY_MODE_NONE);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, opt->color_key_mode);
            return -1;
        }

        TDE_CHECK_ALUCMD(opt->alpha_blending_cmd);

        if ((hi_u32)(opt->alpha_blending_cmd) & HI_TDE_ALPHA_BLENDING_ROP) {
            if ((!TdeOsiIsSingleSrc2Rop(opt->rop_alpha)) || (!TdeOsiIsSingleSrc2Rop(opt->rop_color))) {
                GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiIsSingleSrc2Rop, FAILURE_TAG);
                return -1;
            }
        } else if (HI_TDE_ALPHA_BLENDING_NONE != opt->alpha_blending_cmd) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_ALPHA_BLENDING_NONE);
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, opt->alpha_blending_cmd);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Alu mode error");
            return -1;
        }
        if (TDE_COLORFMT_TRANSFORM_CLUT_CLUT == enColorTransType) {
            if ((opt->alpha_blending_cmd != HI_TDE_ALPHA_BLENDING_NONE)) {
                GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_ALPHA_BLENDING_NONE);
                GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, opt->alpha_blending_cmd);
                GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "It doesn't ROP/Blend/Colorize");
                return -1;
            }

            if (enSrcFmt != enDstFmt) {
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enSrcFmt);
                GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enDstFmt);
                return -1;
            }
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}
/*****************************************************************************
* Function:      TdeOsiCheckDoubleSrcPatternOpt
* Description:   check if doul source mode fill operate parameter is valid
* Input:         enBackGroundFmt background pixel format
                 enForeGroundFmt foreground pixel format
                 enDstFmt  target pixel format
                 opt     operate attribute pointer
* Output:        none
* Return:        0  valid parameter
                 -1 invalid parameter
* Others:        none
*****************************************************************************/
hi_s32 TdeOsiCheckDoubleSrcPatternOpt(hi_tde_color_fmt enBackGroundFmt, hi_tde_color_fmt enForeGroundFmt,
                                      hi_tde_color_fmt enDstFmt, hi_tde_pattern_fill_opt *opt)
{
    tde_color_fmt_category enBackGroundCategory;
    tde_color_fmt_category enForeGroundCategory;
    tde_color_fmt_category enDstCategory;
    hi_bool bUnknownColorFormat;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(opt, HI_FAILURE);

    TDE_CHECK_DST_FMT(enDstFmt);

    TDE_CHECK_BACKGROUND_FMT(enBackGroundFmt);

    TDE_CHECK_ALUCMD(opt->alpha_blending_cmd);

    enBackGroundCategory = TdeOsiGetFmtCategory(enBackGroundFmt);

    enForeGroundCategory = TdeOsiGetFmtCategory(enForeGroundFmt);

    enDstCategory = TdeOsiGetFmtCategory(enDstFmt);

    bUnknownColorFormat = (TDE_COLORFMT_CATEGORY_BYTE <= enBackGroundCategory) ||
                          (TDE_COLORFMT_CATEGORY_BYTE <= enForeGroundCategory) ||
                          (TDE_COLORFMT_CATEGORY_BYTE <= enDstCategory);

    if (bUnknownColorFormat) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, TDE_COLORFMT_CATEGORY_BYTE);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enBackGroundCategory);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enForeGroundCategory);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, enDstCategory);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Unknown color format");
        return -1;
    }

    TDE_PATTERNBACKGROUNDCATEGORY_CHECK_ARGB(enBackGroundCategory, enForeGroundCategory, enDstFmt);

    TDE_PATTERNFOREGROUNDCATEGORY_CHECK_AN(enForeGroundCategory, enBackGroundFmt, enForeGroundFmt);

    TDE_PATTERNBACKGROUNDCATEGORY_CHECK_CLUT(enBackGroundCategory, enForeGroundCategory, enBackGroundFmt,
                                             enForeGroundFmt, enDstFmt, opt->alpha_blending_cmd, enDstCategory);

    TDE_PATTERNBACKGROUNDCATEGORY_CHECK_AN(enBackGroundCategory, enForeGroundCategory, enDstCategory,
                                           opt->alpha_blending_cmd);

    TDE_PATTERNDSTCATEGORY_CHECK_CLUT(enBackGroundCategory, enForeGroundCategory, enDstCategory);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}
STATIC TDE_PATTERN_OPERATION_CATEGORY_E TdeOsiSingleSrcPatternOperation(
    hi_tde_surface *pstBackGround, hi_tde_rect *back_ground_rect, hi_tde_surface *fore_ground_surface,
    hi_tde_rect *fore_ground_rect, hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_pattern_fill_opt *opt)
{
    hi_tde_surface *pTmpSrc2 = HI_NULL;
    hi_tde_rect *pTmpSrc2Rect = HI_NULL;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (pstBackGround != NULL) {
        if (HI_NULL == back_ground_rect) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, back_ground_rect);
            return TDE_PATTERN_OPERATION_BUTT;
        }

        pTmpSrc2 = pstBackGround;
        pTmpSrc2Rect = back_ground_rect;
    } else if (fore_ground_surface != NULL) {
        if (HI_NULL == fore_ground_rect) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fore_ground_rect);
            return TDE_PATTERN_OPERATION_BUTT;
        }

        if (HI_TDE_COLOR_FMT_YCbCr422 == fore_ground_surface->color_fmt) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_COLOR_FMT_YCbCr422);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, fore_ground_surface->color_fmt);
            return -1;
        }

        pTmpSrc2 = fore_ground_surface;
        pTmpSrc2Rect = fore_ground_rect;
    }

    if (pTmpSrc2 != HI_NULL) {
        TDE_CHECK_NOT_MB(pTmpSrc2->color_fmt);

        if (TdeOsiCheckSurface(pTmpSrc2, pTmpSrc2Rect) < 0) {
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
            return TDE_PATTERN_OPERATION_BUTT;
        }

        if (pTmpSrc2Rect->width > 256) {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, pTmpSrc2Rect->width);
            GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Max pattern width is 256");
            return TDE_PATTERN_OPERATION_BUTT;
        }
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return TDE_PATTERN_OPERATION_SINGLE_SRC;
}

STATIC TDE_PATTERN_OPERATION_CATEGORY_E TdeOsiDoubleSrcPatternOperation(
    hi_tde_surface *pstBackGround, hi_tde_rect *back_ground_rect, hi_tde_surface *fore_ground_surface,
    hi_tde_rect *fore_ground_rect, hi_tde_rect *dst_rect, hi_tde_pattern_fill_opt *opt)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((NULL == back_ground_rect) || (NULL == fore_ground_rect) || (NULL == opt)) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, back_ground_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fore_ground_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, opt);
        return TDE_PATTERN_OPERATION_BUTT;
    }

    TDE_CHECK_NOT_MB(pstBackGround->color_fmt);
    TDE_CHECK_NOT_MB(fore_ground_surface->color_fmt);

    if (HI_TDE_COLOR_FMT_YCbCr422 == fore_ground_surface->color_fmt) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, HI_TDE_COLOR_FMT_YCbCr422);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, fore_ground_surface->color_fmt);
        return -1;
    }

    if ((TdeOsiCheckSurface(pstBackGround, back_ground_rect) < 0) ||
        (TdeOsiCheckSurface(fore_ground_surface, fore_ground_rect) < 0)) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return TDE_PATTERN_OPERATION_BUTT;
    }

    if ((back_ground_rect->height != dst_rect->height) || (back_ground_rect->width != dst_rect->width)) {
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION,
                             "Size of background rect and Dst rect should be the same in two src pattern fill");
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, back_ground_rect->pos_x);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, back_ground_rect->pos_y);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, back_ground_rect->width);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, back_ground_rect->height);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, dst_rect->pos_x);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, dst_rect->pos_y);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, dst_rect->width);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, dst_rect->height);
        return TDE_PATTERN_OPERATION_BUTT;
    }

    if (fore_ground_rect->width > 256) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, fore_ground_rect->width);
        GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "Max pattern width is 256");
        return TDE_PATTERN_OPERATION_BUTT;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return TDE_PATTERN_OPERATION_DOUBLE_SRC;
}

/*****************************************************************************
 * Function:      TdeOsiGetPatternOptCategory
 * Description:   analyze pattern fill operate type
 * Input:         pstBackGround: background bitmap info
 *                back_ground_rect: background bitmap operate zone
 *                fore_ground_surface: foreground bitmap info
 *                fore_ground_rect: foreground bitmap operate zone
 *                dst_surface: target bitmap info
 *                dst_rect: target bitmap operate zone
 *                opt: operate option
 * Output:        none
 * Return:        TDE operate type
 * Others:        none
 *****************************************************************************/
TDE_PATTERN_OPERATION_CATEGORY_E TdeOsiGetPatternOptCategory(hi_tde_surface *pstBackGround,
                                                             hi_tde_rect *back_ground_rect,
                                                             hi_tde_surface *fore_ground_surface,
                                                             hi_tde_rect *fore_ground_rect, hi_tde_surface *dst_surface,
                                                             hi_tde_rect *dst_rect, hi_tde_pattern_fill_opt *opt)
{
    hi_s32 Ret = HI_SUCCESS;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if ((NULL == dst_surface) || (NULL == dst_rect)) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_rect);
        return TDE_PATTERN_OPERATION_BUTT;
    }
    if (HI_TDE_COLOR_FMT_YCbCr422 == dst_surface->color_fmt) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_COLOR_FMT_YCbCr422);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, dst_surface->color_fmt);
        return -1;
    }

    TDE_CHECK_NOT_MB(dst_surface->color_fmt);

    if (TdeOsiCheckSurface(dst_surface, dst_rect) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return TDE_PATTERN_OPERATION_BUTT;
    }

    if ((NULL == pstBackGround) && (NULL == fore_ground_surface)) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstBackGround);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fore_ground_surface);
        return TDE_PATTERN_OPERATION_BUTT;
    } else if ((NULL != pstBackGround) && (NULL != fore_ground_surface)) {
        Ret = TdeOsiDoubleSrcPatternOperation(pstBackGround, back_ground_rect, fore_ground_surface, fore_ground_rect,
                                              dst_rect, opt);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return Ret;
    } else {
        Ret = TdeOsiSingleSrcPatternOperation(pstBackGround, back_ground_rect, fore_ground_surface, fore_ground_rect,
                                              dst_surface, dst_rect, opt);
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return Ret;
    }
}

STATIC hi_bool TdeOsiCheckOverlap(hi_tde_surface *pstSur1, hi_tde_rect *pstRect1, hi_tde_surface *pstSur2,
                                  hi_tde_rect *pstRect2)
{
    hi_u32 u32Rect1StartPhy = 0;
    hi_u32 u32Rect1EndPhy = 0;
    hi_u32 u32Rect2StartPhy = 0;
    hi_u32 u32Rect2EndPhy = 0;
    hi_u32 u32Bpp1 = 0;
    hi_u32 u32Bpp2 = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    {
        if (pstRect2->height < pstRect1->height) {
            pstRect1->height = pstRect2->height;
        }

        if (pstRect2->width < pstRect1->width) {
            pstRect1->width = pstRect2->width;
        }
    }

    u32Bpp1 = TdeOsiGetbppByFmt(pstSur1->color_fmt) / 8;
    u32Bpp2 = TdeOsiGetbppByFmt(pstSur2->color_fmt) / 8;

    u32Rect1StartPhy = pstSur1->phy_addr + (pstRect1->pos_y * pstSur1->stride) + pstRect1->pos_x * u32Bpp1;
    u32Rect1EndPhy = u32Rect1StartPhy + (pstRect1->height - 1) * pstSur1->stride + (pstRect1->width - 1) * u32Bpp1;

    u32Rect2StartPhy = pstSur2->phy_addr + (pstRect2->pos_y * pstSur2->stride) + pstRect2->pos_x * u32Bpp2;
    u32Rect2EndPhy = u32Rect2StartPhy + (pstRect2->height - 1) * pstSur2->stride + (pstRect2->width - 1) * u32Bpp2;

    if (((u32Rect1StartPhy >= u32Rect2StartPhy) && (u32Rect1StartPhy <= u32Rect2EndPhy)) ||
        ((u32Rect1EndPhy >= u32Rect2StartPhy) && (u32Rect1EndPhy <= u32Rect2EndPhy))) {
        GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
        return HI_TRUE;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_FALSE;
}

STATIC hi_s32 TdeOsiCheckSingleSrcPatternFillPara(hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                                                  hi_tde_surface *dst_surface, hi_tde_rect *dst_rect,
                                                  hi_tde_pattern_fill_opt *opt)
{
    hi_bool bContainNullPtr = ((NULL == src_surface) || (NULL == src_rect) || (NULL == dst_surface) ||
                               (NULL == dst_rect));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (bContainNullPtr) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, src_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, src_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_rect);
        return HI_ERR_TDE_NULL_PTR;
    }

    if (TdeOsiCheckSingleSrcPatternOpt(src_surface->color_fmt, dst_surface->color_fmt, opt) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSingleSrcPatternOpt, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (TdeOsiCheckOverlap(src_surface, src_rect, dst_surface, dst_rect)) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckOverlap, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

/*****************************************************************************
* Function:      TdeOsiSingleSrcPatternFill
* Description:   single resource pattern fill
* Input:         handle:task handle
                pstBackGround: background bitmap info
*                back_ground_rect: background bitmap operate zone
*                fore_ground_surface: foreground bitmap info
*                fore_ground_rect: foreground bitmap operate zone
*                dst_surface: target bitmap info
*                dst_rect: target bitmap operate zone
*                opt: operate option
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
hi_s32 TdeOsiSingleSrcPatternFill(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                                  hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_pattern_fill_opt *opt)
{
    tde_hardware_node *hardware_node = NULL;
    tde_base_opt_mode enBaseMode = {0};
    tde_alu_mode enAluMode = {0};
    tde_scandirection_mode stSrcScanInfo = {0};
    tde_scandirection_mode stDstScanInfo = {0};
    tde_surface_msg stSrcDrvSurface = {0};
    tde_surface_msg stDstDrvSurface = {0};
    hi_u16 u16Code = 0;
    tde_conv_mode_cmd stConv = {0};
    TDE_CLUT_USAGE_E enClutUsage = TDE_CLUT_USAGE_BUTT;
    hi_bool bCheckSingleSrc2Rop = HI_TRUE;
    hi_bool bCheckBlend = HI_FALSE;
    hi_s32 s32Ret = HI_SUCCESS;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    s32Ret = TdeOsiCheckSingleSrcPatternFillPara(src_surface, src_rect, dst_surface, dst_rect, opt);
    if (s32Ret != HI_SUCCESS) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSingleSrcPatternFillPara, FAILURE_TAG);
        return s32Ret;
    }

    if (tde_hal_init_node(&hardware_node) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_init_node, FAILURE_TAG);
        return HI_ERR_TDE_NO_MEM;
    }
    enBaseMode = TDE_SINGLE_SRC_PATTERN_FILL_OPT;
    enAluMode = TDE_ALU_NONE;

    if (opt != HI_NULL) {
        if (opt->out_alpha_from >= HI_TDE_OUT_ALPHA_FROM_MAX) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_OUT_ALPHA_FROM_MAX);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, opt->out_alpha_from);
            tde_hal_free_node_buf(hardware_node);
            return HI_ERR_TDE_INVALID_PARA;
        }

        if (opt->out_alpha_from == HI_TDE_OUT_ALPHA_FROM_BACKGROUND) {
            GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_INT, HI_TDE_OUT_ALPHA_FROM_BACKGROUND);
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, opt->out_alpha_from);
            tde_hal_free_node_buf(hardware_node);
            return HI_ERR_TDE_INVALID_PARA;
        }

        s32Ret = TdeOsiSetRop(hardware_node, opt->alpha_blending_cmd, opt->rop_color, opt->rop_alpha, &enAluMode,
                              bCheckSingleSrc2Rop);
        if (s32Ret) {
            tde_hal_free_node_buf(hardware_node);
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetRop, FAILURE_TAG);
            return s32Ret;
        }

        s32Ret = TdeOsiSetBlend(hardware_node, opt->alpha_blending_cmd, opt->blend_opt, &enAluMode, bCheckBlend);
        if (s32Ret) {
            tde_hal_free_node_buf(hardware_node);
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetBlend, FAILURE_TAG);
            return s32Ret;
        }

        s32Ret = TdeOsiSetColorize(hardware_node, opt->alpha_blending_cmd, opt->color_resize);
        if (s32Ret) {
            tde_hal_free_node_buf(hardware_node);
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetColorize, FAILURE_TAG);
            return s32Ret;
        }

        tde_hal_node_set_global_alpha(hardware_node, opt->global_alpha, opt->blend_opt.global_alpha_en);

        if (TdeOsiSetPatternClipPara(HI_NULL, HI_NULL, src_surface, src_rect, dst_surface, dst_rect, opt,
                                     hardware_node) < 0) {
            tde_hal_free_node_buf(hardware_node);
            GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetPatternClipPara, FAILURE_TAG);
            return HI_ERR_TDE_CLIP_AREA;
        }
    }

    if (tde_hal_node_set_base_operate(hardware_node, enBaseMode, enAluMode, HI_NULL) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_base_operate, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    TdeOsiSetExtAlpha(HI_NULL, src_surface, hardware_node);

    stSrcScanInfo.hor_scan = TDE_SCAN_LEFT_RIGHT;
    stSrcScanInfo.ver_scan = TDE_SCAN_UP_DOWN;
    stDstScanInfo.hor_scan = TDE_SCAN_LEFT_RIGHT;
    stDstScanInfo.ver_scan = TDE_SCAN_UP_DOWN;

    TdeOsiConvertSurface(src_surface, src_rect, &stSrcScanInfo, &stSrcDrvSurface, NULL);

    tde_hal_set_src2(hardware_node, &stSrcDrvSurface);

    TdeOsiConvertSurface(dst_surface, dst_rect, &stDstScanInfo, &stDstDrvSurface, NULL);

    (opt != HI_NULL) ? tde_hal_node_set_tqt(hardware_node, &stDstDrvSurface, opt->out_alpha_from)
                        : tde_hal_node_set_tqt(hardware_node, &stDstDrvSurface, HI_TDE_OUT_ALPHA_FROM_FOREGROUND);

    u16Code = TdeOsiSingleSrc2GetOptCode(src_surface->color_fmt, dst_surface->color_fmt);

    TdeOsiGetConvbyCode(u16Code, &stConv);

    if (tde_hal_node_set_color_convert(hardware_node, &stConv) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_color_convert, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    s32Ret = (opt != HI_NULL)
                    ? TdeOsiSetClutOpt(src_surface, dst_surface, &enClutUsage, opt->clut_reload, hardware_node)
                    : TdeOsiSetClutOpt(src_surface, dst_surface, &enClutUsage, HI_TRUE, hardware_node);

    if (s32Ret != HI_SUCCESS) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetClutOpt, FAILURE_TAG);
        return s32Ret;
    }

    if ((s32Ret = TdeOsiSetNodeFinish(handle, hardware_node, 0, TDE_NODE_SUBM_ALONE)) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetNodeFinish, FAILURE_TAG);
        return s32Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

STATIC hi_s32 TdeOsiCheckDoubleSrcPatternFillPara(hi_tde_surface *pstBackGround, hi_tde_rect *back_ground_rect,
                                                  hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                                                  hi_tde_surface *dst_surface, hi_tde_rect *dst_rect,
                                                  hi_tde_pattern_fill_opt *opt)
{
    hi_bool bContainNullPtr = ((HI_NULL == pstBackGround) || (HI_NULL == back_ground_rect) ||
                               (HI_NULL == fore_ground_surface) || (HI_NULL == fore_ground_rect) ||
                               (HI_NULL == dst_surface) || (HI_NULL == dst_rect) || (HI_NULL == opt));

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    if (bContainNullPtr) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstBackGround);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, back_ground_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fore_ground_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, fore_ground_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, opt);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (TdeOsiCheckDoubleSrcPatternOpt(pstBackGround->color_fmt, fore_ground_surface->color_fmt, dst_surface->color_fmt,
                                       opt) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckDoubleSrcPatternOpt, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    if (TdeOsiCheckOverlap(fore_ground_surface, fore_ground_rect, pstBackGround, back_ground_rect) ||
        TdeOsiCheckOverlap(fore_ground_surface, fore_ground_rect, dst_surface, dst_rect)) {
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, pstBackGround);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, back_ground_rect);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_surface);
        GRAPHIC_DFX_MINI_ERR_VALUE(GRAPHIC_DFX_ERR_VOID, dst_rect);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckOverlap, FAILURE_TAG);
        return HI_ERR_TDE_INVALID_PARA;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

/*****************************************************************************
 * Function:      TdeOsiDoubleSrcPatternFill
 * Description:   dual resource pattern fill
 * Input:         handle: task handle
 *                pstBackGround: background bitmap info
 *                back_ground_rect: background bitmap operate zone
 *                fore_ground_surface: foreground bitmap info
 *                fore_ground_rect: foreground bitmap operate zone
 *                dst_surface: target bitmap info
 *                dst_rect: target bitmap operate zone
 *                opt: operate option
 * Output:        none
 * Return:        success/fail
 * Others:        none
 *****************************************************************************/
hi_s32 TdeOsiDoubleSrcPatternFill(hi_s32 handle, hi_tde_surface *pstBackGround, hi_tde_rect *back_ground_rect,
                                  hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                                  hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_pattern_fill_opt *opt)
{
    tde_hardware_node *hardware_node = NULL;
    tde_surface_msg stSrcDrvSurface = {0};
    tde_surface_msg stDstDrvSurface = {0};
    tde_scandirection_mode stSrcScanInfo = {0};
    tde_scandirection_mode stDstScanInfo = {0};
    tde_alu_mode enAluMode = TDE_ALU_NONE;
    TDE_CLUT_USAGE_E enClutUsage = TDE_CLUT_USAGE_BUTT;
    hi_u16 u16Code = 0;
    tde_conv_mode_cmd stConv = {0};
    hi_bool bCheckBlend = HI_TRUE;
    hi_bool bCheckSingleSrc2Rop = HI_FALSE;
    hi_s32 s32Ret = HI_SUCCESS;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    TDE_CHECK_COLORKEYMODE(opt->color_key_mode);
    s32Ret = TdeOsiCheckDoubleSrcPatternFillPara(pstBackGround, back_ground_rect, fore_ground_surface, fore_ground_rect,
                                                 dst_surface, dst_rect, opt);
    if (HI_SUCCESS != s32Ret) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckDoubleSrcPatternFillPara, FAILURE_TAG);
        return s32Ret;
    }

    if (tde_hal_init_node(&hardware_node) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_init_node, FAILURE_TAG);
        return HI_ERR_TDE_NO_MEM;
    }

    s32Ret = TdeOsiSetRop(hardware_node, opt->alpha_blending_cmd, opt->rop_color, opt->rop_alpha, &enAluMode,
                          bCheckSingleSrc2Rop);
    if (s32Ret) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetRop, FAILURE_TAG);
        return s32Ret;
    }

    s32Ret = TdeOsiSetBlend(hardware_node, opt->alpha_blending_cmd, opt->blend_opt, &enAluMode, bCheckBlend);
    if (s32Ret) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetBlend, FAILURE_TAG);
        return s32Ret;
    }

    s32Ret = TdeOsiSetColorize(hardware_node, opt->alpha_blending_cmd, opt->color_resize);
    if (s32Ret) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetColorize, FAILURE_TAG);
        return s32Ret;
    }

    tde_hal_node_set_global_alpha(hardware_node, opt->global_alpha, opt->blend_opt.global_alpha_en);

    if (TdeOsiSetPatternClipPara(pstBackGround, back_ground_rect, fore_ground_surface, fore_ground_rect, dst_surface,
                                 dst_rect, opt, hardware_node) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetPatternClipPara, FAILURE_TAG);
        return HI_ERR_TDE_CLIP_AREA;
    }

    if (tde_hal_node_set_base_operate(hardware_node, TDE_DOUBLE_SRC_PATTERN_FILL_OPT, enAluMode, HI_NULL) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_base_operate, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    TdeOsiSetExtAlpha(pstBackGround, fore_ground_surface, hardware_node);

    stSrcScanInfo.hor_scan = TDE_SCAN_LEFT_RIGHT;
    stSrcScanInfo.ver_scan = TDE_SCAN_UP_DOWN;
    stDstScanInfo.hor_scan = TDE_SCAN_LEFT_RIGHT;
    stDstScanInfo.ver_scan = TDE_SCAN_UP_DOWN;

    TdeOsiConvertSurface(pstBackGround, back_ground_rect, &stSrcScanInfo, &stSrcDrvSurface, NULL);

    tde_hal_set_src1(hardware_node, &stSrcDrvSurface);

    TdeOsiConvertSurface(fore_ground_surface, fore_ground_rect, &stSrcScanInfo, &stSrcDrvSurface, NULL);

    tde_hal_set_src2(hardware_node, &stSrcDrvSurface);

    TdeOsiConvertSurface(dst_surface, dst_rect, &stDstScanInfo, &stDstDrvSurface, NULL);

    tde_hal_node_set_tqt(hardware_node, &stDstDrvSurface, opt->out_alpha_from);

    u16Code = TdeOsiDoubleSrcGetOptCode(pstBackGround->color_fmt, fore_ground_surface->color_fmt,
                                        dst_surface->color_fmt);

    TdeOsiGetConvbyCode(u16Code, &stConv);

    if (tde_hal_node_set_color_convert(hardware_node, &stConv) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, tde_hal_node_set_color_convert, FAILURE_TAG);
        return HI_ERR_TDE_UNSUPPORTED_OPERATION;
    }

    if ((s32Ret = TdeOsiSetClutOpt(fore_ground_surface, pstBackGround, &enClutUsage, opt->clut_reload, hardware_node)) <
        0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetClutOpt, FAILURE_TAG);
        return s32Ret;
    }

    s32Ret = TdeOsiSetColorKey(fore_ground_surface, pstBackGround, hardware_node, opt->color_key_value,
                               opt->color_key_mode, enClutUsage);
    if (s32Ret) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetColorKey, FAILURE_TAG);
        return s32Ret;
    }

    if ((s32Ret = TdeOsiSetNodeFinish(handle, hardware_node, 0, TDE_NODE_SUBM_ALONE)) < 0) {
        tde_hal_free_node_buf(hardware_node);
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiSetNodeFinish, FAILURE_TAG);
        return s32Ret;
    }

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

/*****************************************************************************
* Function:      tde_hal_pattern_fill
* Description:   pattern fill
* Input:         handle:task handle
                 pstBackGround: background bitmap info
*                back_ground_rect: background bitmap operate zone
*                fore_ground_surface: foreground bitmap info
*                fore_ground_rect: foreground bitmap operate zone
*                dst_surface: target bitmap info
*                dst_rect: target bitmap operate zone
*                opt: operate option
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
hi_s32 tde_hal_pattern_fill(hi_s32 handle, hi_tde_surface *pstBackGround, hi_tde_rect *back_ground_rect,
                            hi_tde_surface *fore_ground_surface, hi_tde_rect *fore_ground_rect,
                            hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_pattern_fill_opt *opt)
{
    hi_s32 Ret = HI_SUCCESS;
    TDE_PATTERN_OPERATION_CATEGORY_E enOptCategory;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    enOptCategory = TdeOsiGetPatternOptCategory(pstBackGround, back_ground_rect, fore_ground_surface, fore_ground_rect,
                                                dst_surface, dst_rect, opt);
    switch (enOptCategory) {
        case TDE_PATTERN_OPERATION_SINGLE_SRC: {
            if (HI_NULL != pstBackGround) {
                Ret = TdeOsiSingleSrcPatternFill(handle, pstBackGround, back_ground_rect, dst_surface, dst_rect, opt);
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return Ret;
            } else {
                Ret = TdeOsiSingleSrcPatternFill(handle, fore_ground_surface, fore_ground_rect, dst_surface, dst_rect,
                                                 opt);
                GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
                return Ret;
            }
        }
        case TDE_PATTERN_OPERATION_DOUBLE_SRC: {
            Ret = TdeOsiDoubleSrcPatternFill(handle, pstBackGround, back_ground_rect, fore_ground_surface,
                                             fore_ground_rect, dst_surface, dst_rect, opt);
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return Ret;
        }
        default:
            GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
            return HI_ERR_TDE_INVALID_PARA;
    }
}

/*****************************************************************************
* Function:      drv_tde_calc_scale_rect
* Description:   update zoom rect information
* Input:         src_rect:source bitmap operate zone
                 dst_rect: target bitmap operate zone
*                rect_in_src: source bitmap scale zone
*                rect_in_dst: target bitmap info atfer calculating
* Output:        none
* Return:        success/fail
* Others:        none
*****************************************************************************/
hi_s32 drv_tde_calc_scale_rect(const hi_tde_rect *src_rect, const hi_tde_rect *dst_rect, hi_tde_rect *rect_in_src,
                               hi_tde_rect *rect_in_dst)
{
    UpdateConfig reg;
    UpdateInfo info;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(src_rect, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_rect, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(rect_in_src, HI_FAILURE);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(rect_in_dst, HI_FAILURE);

    reg.ori_in_height = src_rect->height;
    reg.ori_in_width = src_rect->width;
    reg.zme_out_height = dst_rect->height;
    reg.zme_out_width = dst_rect->width;

    reg.update_instart_w = rect_in_src->pos_x;
    reg.update_instart_h = rect_in_src->pos_y;
    reg.update_in_width = rect_in_src->width;
    reg.update_in_height = rect_in_src->height;

    TdeOsiGetHUpdateInfo(&reg, &info, HI_TRUE);
    TdeOsiGetVUpdateInfo(&reg, &info, HI_TRUE, HI_TRUE);

    rect_in_src->pos_x = info.zme_instart_w;
    rect_in_src->pos_y = info.zme_instart_h;
    rect_in_src->width = info.zme_in_width;
    rect_in_src->height = info.zme_in_height;

    rect_in_dst->pos_x = info.zme_outstart_w;
    rect_in_dst->pos_y = info.zme_outstart_h;
    rect_in_dst->width = info.zme_out_width;
    rect_in_dst->height = info.zme_out_height;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return HI_SUCCESS;
}

hi_s32 drv_tde_enable_region_deflicker(hi_bool region_deflicker)
{
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);
    s_bRegionDeflicker = region_deflicker;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);
    return HI_SUCCESS;
}

hi_s32 drv_tde_quick_copyex(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                            hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_bool mmz_for_src,
                            hi_bool mmz_for_dst)
{
    hi_s32 s32Ret = 0;
    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(src_surface, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(src_rect, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_surface, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_rect, HI_ERR_TDE_NULL_PTR);

    if ((s32Ret = TdeOsiCheckSurface(src_surface, src_rect)) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return s32Ret;
    }

    if ((s32Ret = TdeOsiCheckSurface(dst_surface, dst_rect)) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return s32Ret;
    }

    s32Ret = TdeOsiSingleSrc1Blit(handle, src_surface, src_rect, dst_surface, dst_rect, mmz_for_src, mmz_for_dst);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return s32Ret;
}

hi_s32 drv_tde_single_blitex(hi_s32 handle, hi_tde_surface *src_surface, hi_tde_rect *src_rect,
                             hi_tde_surface *dst_surface, hi_tde_rect *dst_rect, hi_tde_opt *opt, hi_bool mmz_for_src,
                             hi_bool mmz_for_dst)
{
    hi_s32 s32Ret = 0;

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_ENTER_FUNC);

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(src_surface, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(src_rect, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_surface, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(dst_rect, HI_ERR_TDE_NULL_PTR);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(opt, HI_ERR_TDE_NULL_PTR);

    if ((s32Ret = TdeOsiCheckSurface(src_surface, src_rect)) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return s32Ret;
    }

    if ((s32Ret = TdeOsiCheckSurface(dst_surface, dst_rect)) < 0) {
        GRAPHIC_DFX_MINI_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, TdeOsiCheckSurface, FAILURE_TAG);
        return s32Ret;
    }

    s32Ret = TdeOsiSingleSrc2Blit(handle, src_surface, src_rect, dst_surface, dst_rect, opt, mmz_for_src, mmz_for_dst);

    GRAPHIC_DFX_DEBUG_FUNC(GRAPHIC_DFX_INNER_EXIT_FUNC);

    return s32Ret;
}
EXPORT_SYMBOL(tde_hal_pattern_fill);
EXPORT_SYMBOL(drv_tde_enable_region_deflicker);
EXPORT_SYMBOL(drv_tde_calc_scale_rect);

#endif
