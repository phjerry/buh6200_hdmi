/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VFMW_DEBUG__
#define __VFMW_DEBUG__

#include "vfmw.h"
#include "vfmw_osal.h"

#define VFMW_DBG_OK  0
#define VFMW_DBG_ERR (-1)

typedef enum {
    PRN_DEV_COM,
    PRN_DEV_LOG,
    PRN_DEV_MAX,
} print_dev;

extern hi_u32 g_print_word;
extern print_dev g_print_out;

#ifdef VFMW_DPRINT_SUPPORT
#if defined(ENV_ARMLINUX_KERNEL) || defined(VFMW_MDC_SUPPORT)
#define dprint(type, fmt, arg...) dbg_print(type, fmt, ##arg)
#else
#define dprint(type, fmt, arg...)                                                               \
    do {                                                                                        \
        if ((type == PRN_ALWS) || ((g_print_word & (1 << type)) != 0)) { \
            if (g_print_out == PRN_DEV_COM) {                                                   \
                OS_PRINT(fmt, ##arg);                                                           \
            } else if (g_print_out == PRN_DEV_LOG) {                                            \
                OS_WR_LOG(fmt, ##arg);                                                          \
            }                                                                                   \
        }                                                                                       \
    } while (0)
#endif
#else
#define dprint(type, fmt, arg...)
#endif

#define dcheck(type) ((g_print_word & (1 << type)) != 0)

typedef enum {
    PRN_FATAL = 0,  /* 0.  fatal error */
    PRN_ERROR,      /* 1.  normal error */
    PRN_WARN,       /* 2.  warnning message */
    PRN_DBG,        /* 3.  debug message */
    PRN_VDMREG,     /* 4.  VDM register info */
    PRN_DNMSG,      /* 5.  down message info */
    PRN_RPMSG,      /* 6.  repair message info */
    PRN_UPMSG,      /* 7.  up message info */
    PRN_STREAM,     /* 8.  stream info */
    PRN_CTRL,       /* 9.  control info */
    PRN_SCD_REGMSG, /* 10. SCD register info */
    PRN_REF,        /* 11. reference pictures info */
    PRN_SEQ,        /* 12. sequence syntax info */
    PRN_PIC,        /* 13. picture syntax info */
    PRN_SLICE,      /* 14. slice syntax info */
    PRN_SEI,        /* 15. SEI syntax info */
    PRN_KFUNC,      /* 16. trace key function timestamp info */
    PRN_MDC,        /* 17. */
    PRN_REV_18,     /* 18. */
    PRN_REV_19,     /* 19. */
    PRN_REV_20,     /* 20. */
    PRN_STM_BLK,    /* 21. stm module block info */
    PRN_SYN_BLK,    /* 22. syn module block info */
    PRN_DEC_BLK,    /* 23. dec module block info */
    PRN_PTS,        /* 24. pts info */
    PRN_REV_25,     /* 25. */
    PRN_REV_26,     /* 26. */
    PRN_REV_27,     /* 27. */
    PRN_REV_28,     /* 28. */
    PRN_REV_29,     /* 29. */
    PRN_REV_30,     /* 30. */
    PRN_REV_31,     /* 31. */
    PRN_ALWS = 32   /* 32. unlimit print level */
} print_type;

hi_s32 str_to_val(hi_char *, hi_u32 *);
hi_s32 dbg_create_tracer(hi_void);
hi_void dbg_destroy_tracer(hi_void);
hi_void dbg_print_tracer(hi_void);
hi_void dbg_add_trace(hi_char *, hi_s32, hi_s32);
hi_s32 dbg_save_yuv(hi_s32, vfmw_image *, OS_FILE *);
hi_s32 dbg_save_mtdt(hi_s32, vfmw_image *, OS_FILE *);
hi_s32 dbg_set_print_word(hi_s32);
hi_s32 dbg_get_print_word(hi_void);
char *dbg_show_std(vfmw_vid_std, vfmw_std_ext *);
#if defined(ENV_ARMLINUX_KERNEL) || defined(VFMW_MDC_SUPPORT)
hi_s32 dbg_print(hi_u32, const hi_char *, ...);
#endif

#define REC_POS_DBG(data) dbg_add_trace((hi_s8 *)__func__, __LINE__, (hi_s32)(data))
#define POS()             dprint(PRN_ALWS, "POS: %s %d\n", __func__, __LINE__);

#define DPRINT_DBG(fmt...)
#define DPRINT_INFO(fmt...)
#define DPRINT_WARN(fmt...)
#define DPRINT_ERR(fmt...)
#define DPRINT_FATAL(fmt...)

#define PrintS32(val) dprint(PRN_ALWS, "%s %d: %s = %d\n", __func__, __LINE__, #val, val)
#define PrintU32(val) dprint(PRN_ALWS, "%s %d: %s = %u\n", __func__, __LINE__, #val, val)
#define PrintS64(val) dprint(PRN_ALWS, "%s %d: %s = %lld\n", __func__, __LINE__, #val, val)
#define PrintU64(val) dprint(PRN_ALWS, "%s %d: %s = %llu\n", __func__, __LINE__, #val, val)
#define PrintH32(val) dprint(PRN_ALWS, "%s %d: %s = 0x%08X\n", __func__, __LINE__, #val, val)
#define PrintH64(val) dprint(PRN_ALWS, "%s %d: %s = 0x%016llX\n", __func__, __LINE__, #val, val)
#define PrintStr(val) dprint(PRN_ALWS, "%s %d: %s = %s\n", __func__, __LINE__, #val, val)
#define PrintPtr(val) dprint(PRN_ALWS, "%s %d: %s = %p\n", __func__, __LINE__, #val, val)
#define PrintFlt(val) dprint(PRN_ALWS, "%s %d: %s = %f\n", __func__, __LINE__, #val, val)
#define PrintFuncErr(func, err_code) dprint(PRN_ERROR, "Call %s return [0x%08X]\n", #func, err_code);

#define VFMW_ASSERT(cond)                                           \
    do {                                                            \
        if (!(cond)) {                                              \
            dprint(PRN_ERROR, "[%s %d] Assert Warning: condition %s not match.\n", __func__, __LINE__, #cond); \
            return;                                                 \
        }                                                           \
    } while (0)

#define VFMW_ASSERT_PRNT(cond, fmt, arg...) \
    do {                                    \
        if (!(cond)) {                      \
            dprint(PRN_ERROR, fmt, ##arg);  \
            return;                         \
        }                                   \
    } while (0)

#define VFMW_ASSERT_GOTO(cond, tag)                                 \
    do {                                                            \
        if (!(cond)) {                                              \
            dprint(PRN_ERROR, "[%s %d] Assert Warning: condition %s not match.\n", __func__, __LINE__, #cond); \
            goto tag;                                               \
        }                                                           \
    } while (0)

#define VFMW_ASSERT_GOTO_PRNT(cond, tag, fmt, arg...) \
    do {                                              \
        if (!(cond)) {                                \
            dprint(PRN_ERROR, fmt, ##arg);            \
            goto tag;                                 \
        }                                             \
    } while (0)

#define VFMW_ASSERT_RET(cond, ret)                                 \
    do {                                                           \
        if (!(cond)) {                                             \
            dprint(PRN_ERROR, "[%s %d] Assert Warning: condition %s not match.\n", __func__, __LINE__, #cond); \
            return ret;                                            \
        }                                                          \
    } while (0)

#define VFMW_ASSERT_RET_PRNT(cond, ret, fmt, arg...) \
    do {                                             \
        if (!(cond)) {                               \
            dprint(PRN_ERROR, fmt, ##arg);           \
            return ret;                              \
        }                                            \
    } while (0)

#endif
