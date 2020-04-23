/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2007-2019. All rights reserved.
 * Description: system interface.
 * Author: guoqingbo
 * Create: 2007-1-31
 */

#ifndef __HI_DRV_SYS_H__
#define __HI_DRV_SYS_H__

#include "hi_type.h"
#include "hi_debug.h"

#include "hi_drv_module.h"

#include "hi_reg_common.h"

#define HI_FATAL_SYS(fmt...) HI_FATAL_PRINT(HI_ID_SYS, fmt)
#define HI_ERR_SYS(fmt...)   HI_ERR_PRINT(HI_ID_SYS, fmt)
#define HI_WARN_SYS(fmt...)  HI_WARN_PRINT(HI_ID_SYS, fmt)
#define HI_INFO_SYS(fmt...)  HI_INFO_PRINT(HI_ID_SYS, fmt)

typedef enum {
    CHIP_TYPE_HI3716M,
    CHIP_TYPE_HI3716H,
    CHIP_TYPE_HI3716C,
    CHIP_TYPE_HI3716D,

    CHIP_TYPE_HI3720,
    CHIP_TYPE_HI3712,
    CHIP_TYPE_HI3715,

    CHIP_TYPE_HI3718M,
    CHIP_TYPE_HI3718C,
    CHIP_TYPE_HI3719M,
    CHIP_TYPE_HI3719C,
    CHIP_TYPE_HI3719M_A,

    CHIP_TYPE_HI3796C   = 0x20,
    CHIP_TYPE_HI3798C,
    CHIP_TYPE_HI3796M,
    CHIP_TYPE_HI3798M,

    CHIP_TYPE_HI3796C_A = 0x40,
    CHIP_TYPE_HI3798C_A,
    CHIP_TYPE_HI3798C_B,

    CHIP_TYPE_HI3798M_A,
    CHIP_TYPE_HI3798M_H,

    CHIP_TYPE_BUTT
} hi_chip_type;

typedef enum {
    CHIP_VERSION_V100 = 0x100,
    CHIP_VERSION_V101 = 0x101,
    CHIP_VERSION_V110 = 0x110,
    CHIP_VERSION_V200 = 0x200,
    CHIP_VERSION_V210 = 0x210,
    CHIP_VERSION_V300 = 0x300,
    CHIP_VERSION_V310 = 0x310,
    CHIP_VERSION_V400 = 0x400,
    CHIP_VERSION_V410 = 0x410,
    CHIP_VERSION_V420 = 0x420,
    CHIP_VERSION_V430 = 0x430,
    CHIP_VERSION_V450 = 0x450,

    CHIP_VERSION_BUTT
} hi_chip_version;

typedef enum {
    CHIP_PACKAGE_TYPE_BGA_15_15 = 0,
    CHIP_PACKAGE_TYPE_BGA_16_16,
    CHIP_PACKAGE_TYPE_BGA_19_19,
    CHIP_PACKAGE_TYPE_BGA_23_23,
    CHIP_PACKAGE_TYPE_BGA_31_31,
    CHIP_PACKAGE_TYPE_QFP_216,
    CHIP_PACKAGE_TYPE_BGA_21_21,
    CHIP_PACKAGE_TYPE_BGA_14_14,
    CHIP_PACKAGE_TYPE_BGA_35_35,
    CHIP_PACKAGE_TYPE_BUTT
} hi_chip_package_type;

typedef struct {
    hi_bool dolby_support;
    hi_bool dts_support;
    hi_bool advca_support;
    hi_bool macrovision_support;
    hi_bool hdr10_support;
    hi_bool dolbyvision_support;
    hi_bool dd_support;
    hi_bool dbx_support;
    hi_bool smp_support;
} hi_sys_chip_cap;

typedef enum {
    SYS_VIDEO_MPEG2 = 0x1,
    SYS_VIDEO_MPEG4 = 0x2,
    SYS_VIDEO_VC1   = 0x4,
    SYS_VIDEO_H264  = 0x8,
    SYS_VIDEO_MVC   = 0x10,
    SYS_VIDEO_H265  = 0x20,
    SYS_VIDEO_AVS   = 0x40,
    SYS_VIDEO_DIVX  = 0x80,
    SYS_VIDEO_REAL8 = 0x100,
    SYS_VIDEO_REAL9 = 0x200,
    SYS_VIDEO_AVS2  = 0x400,
    SYS_VIDEO_MASK  = 0x7FF,
} hi_sys_video_codec;

typedef struct {
    hi_bool policy_valid;
    hi_u32 policy_value;
} hi_sys_policy;

typedef enum {
    HI_SYS_POLICY_TYPE_EXTRA_BUFF = 0,
    HI_SYS_POLICY_TYPE_CTS_FLAG = 1,
    HI_SYS_POLICY_TYPE_GTS_FLAG = 2,
    HI_SYS_POLICY_TYPE_DISP_WIDTH = 3,
    HI_SYS_POLICY_TYPE_VCODEC_WIDTH = 4,
    HI_SYS_POLICY_TYPE_BUTT
} hi_sys_policy_type;

hi_void hi_drv_sys_get_chip_version(hi_chip_type *chip_type, hi_chip_version *chip_version);
hi_chip_package_type hi_drv_sys_get_chip_package_type(hi_void);

hi_u32  hi_drv_sys_get_time_stamp_ms(hi_void);
hi_s32  hi_drv_sys_get_dolby_support(hi_u32 *dolby_support);
hi_s32  hi_drv_sys_get_dts_support(hi_u32 *dts_support);
hi_s32  hi_drv_sys_get_advca_support(hi_u32 *advca_support);
hi_s32  hi_drv_sys_get_rovi_support(hi_u32 *rovi_support);
hi_s32  hi_drv_sys_get_hdr10_support(hi_u32 *hdr10_support);
hi_s32  hi_drv_sys_get_dolby_vision_support(hi_u32 *dolby_vision_support);
hi_s32  hi_drv_sys_get_mem_cfg(hi_u32 *mem_size, hi_u32 *mmz_size);
hi_u64  hi_drv_sys_get_die_id_64bit(hi_void);
hi_u32  hi_drv_sys_get_die_id(hi_void);
hi_s32  hi_drv_sys_set_irq_affinity(const hi_mod_id module_id, const hi_u32 irq_num, const hi_char *name);
hi_s32  hi_drv_sys_get_cpu_secure_mode(hi_bool *secure_mode);
hi_s32  hi_drv_sys_get_flag(hi_u32 *flag);

/* return value: -1 : unknown; 0 - External Amplifier, 1 - Internal Amplifier */
hi_s32  hi_drv_sys_get_audio_op_mode(hi_void);

/* return value: -1 : unknown; 0 - Full Driver; 1 - Quarter Driver */
hi_s32  hi_drv_sys_get_disp_vdac_drive_mode(hi_void);

hi_s32  hi_drv_tee_common_init(hi_void);
hi_void hi_drv_tee_common_exit(hi_void);
hi_s32  hi_drv_sys_get_sos_version(hi_char *version, hi_u32 version_len);
hi_s32  hi_drv_sys_get_smp_support(hi_bool *smp_support);
hi_s32  hi_drv_sys_disable_video_codec(hi_sys_video_codec video_codec);
hi_sys_video_codec hi_drv_sys_get_video_codec_support(hi_void);
hi_s32  hi_drv_sys_write_register(const hi_u32 reg_addr, const hi_u32 value);
hi_s32  hi_drv_sys_read_register(const hi_u32 reg_addr, hi_u32 *value);
hi_bool hi_drv_sys_check_phy_addr(hi_ulong addr);
hi_s32  hi_drv_sys_get_irq_by_name(hi_char *name);
hi_s32 hi_drv_sys_set_policy(hi_sys_policy_type policy_type, hi_sys_policy *policy);
hi_s32 hi_drv_sys_get_policy(hi_sys_policy_type policy_type, hi_sys_policy *policy);


volatile hi_reg_sys_ctrl *hi_drv_sys_get_ctrl_reg_ptr(hi_void);
volatile hi_reg_peri     *hi_drv_sys_get_peri_reg_ptr(hi_void);
volatile hi_reg_io       *hi_drv_sys_get_io_reg_ptr(hi_void);
volatile hi_reg_crg      *hi_drv_sys_get_crg_reg_ptr(hi_void);

hi_s32  drv_common_module_init(hi_void);
hi_void drv_common_module_exit(hi_void);

#ifdef __KERNEL__

#include <linux/fs.h>

typedef hi_slong (*hi_ioctl)(struct file *file, hi_u32 cmd, hi_void *arg);
hi_slong hi_drv_user_copy(struct file *file, hi_u32 cmd, hi_ulong arg, hi_ioctl func);

#endif

#endif /* __HI_DRV_SYS_H__ */

