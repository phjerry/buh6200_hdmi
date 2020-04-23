/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2009/12/17
 */

#include <linux/hikapi.h>
#include "linux/hisilicon/hi_license.h"

#include "hi_debug.h"
#include "hi_type.h"
#include "hi_osal.h"

#include "hi_reg_common.h"
#include "hi_drv_dev.h"
#include "hi_drv_module.h"
#include "hi_drv_sys.h"
#include "hi_drv_osal.h"
#include "hi_drv_mem.h"
#include "drv_common_ioctl.h"
#include "drv_reg.h"
#include "drv_sys_policy.h"

#define SYS_NS_TO_MS 1000000
#define SYS_REGISTER_SIZE 4

/* for irq request, define the CPU num macro */
#define SYS_FIRST_CPU_INDEX       0
#define SYS_MEDIA_CPU_INDEX       1
#define SYS_GENERAL_DEV_CPU_INDEX 2

static hi_char g_common_version[] __attribute__((used)) = VERSION_STRING;

extern const char *get_sdkversion(void);

static osal_semaphore g_sys_k_mutex;

#define SYS_K_LOCK() do {                                       \
    hi_s32 __ret = osal_sem_down_interruptible(&g_sys_k_mutex); \
    if (__ret != 0) {                                           \
        return __ret;                                           \
    }                                                           \
} while (0)

#define SYS_K_UNLOCK() osal_sem_up(&g_sys_k_mutex)

/* common module use these two macro itself */
#ifndef SYS_REG_READ_BITS
#define SYS_REG_READ_BITS(vir_addr, mask) ({ \
    hi_u32 __value; \
    hi_u32 __reg; \
    __reg = *((volatile unsigned int *)(vir_addr)); \
    __value = ((__reg) & (mask)); \
    __value; \
})
#endif

/* global video codec support status */
static hi_sys_video_codec g_video_codec = SYS_VIDEO_MASK;

hi_s32 drv_sys_init(hi_void)
{
    return osal_sem_init(&g_sys_k_mutex, 1);
}

hi_void drv_sys_exit(hi_void)
{
    osal_sem_destory(&g_sys_k_mutex);
}

hi_void hi_drv_sys_get_chip_version(hi_chip_type *chip_type, hi_chip_version *chip_version)
{
    hi_chip_type    type    = CHIP_TYPE_BUTT;
    hi_chip_version version = CHIP_VERSION_BUTT;
    volatile hi_reg_sys_ctrl *sys_ctrl = drv_sys_get_ctrl_reg_ptr();

    /* chip_type or chip_version maybe NULL, but not both */
    if ((chip_type == HI_NULL) && (chip_version == HI_NULL)) {
        HI_ERR_SYS("invalid input parameter\n");
        return;
    }
    if (sys_ctrl == HI_NULL) {
        HI_ERR_SYS("null pointer!\n");
        return;
    }

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
    if (sys_ctrl->SC_SYSID == 0x37968300) {
        type = CHIP_TYPE_HI3796C;
        version = CHIP_VERSION_V300;
    }
#endif

    if (chip_type) {
        *chip_type = type;
    }

    if (chip_version) {
        *chip_version = version;
    }

    return;
}

hi_chip_package_type hi_drv_sys_get_chip_package_type(hi_void)
{
    volatile hi_reg_sys_ctrl *sys_ctrl = drv_sys_get_ctrl_reg_ptr();

    if (sys_ctrl == HI_NULL) {
        HI_ERR_SYS("null pointer!\n");
        return CHIP_PACKAGE_TYPE_BUTT;
    }

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
    if (sys_ctrl->SC_SYSID == 0x37968300) {
        return CHIP_PACKAGE_TYPE_BGA_35_35;
    }
#endif

    return CHIP_PACKAGE_TYPE_BUTT;
}

hi_u32 hi_drv_sys_get_time_stamp_ms(hi_void)
{
    hi_u64 time_now;

    time_now = osal_sched_clock();

    time_now = osal_div_u64(time_now, SYS_NS_TO_MS);

    return (hi_u32)time_now;
}

/*
 * from datasheet, the value of dolby_flag meaning: 0: support; 1: not support.
 * but we change its meaning for return parameter: 0: not support; 1: support.
 */
hi_s32 hi_drv_sys_get_dolby_support(hi_u32 *dolby_support)
{
    volatile hi_reg_peri *reg_peri = drv_sys_get_peri_reg_ptr();

    if (dolby_support == HI_NULL) {
        HI_ERR_SYS("support_dolby is null\n");
        return HI_FAILURE;
    }

    if (reg_peri == HI_NULL) {
        HI_ERR_SYS("null pointer!\n");
        return HI_FAILURE;
    }

#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
    *dolby_support = !(reg_peri->CHIPSET_INFO.bits.dolby_flag);
#else
    *dolby_support = (reg_peri->PERI_SOC_FUSE_3 & 0x1) == 0 ? 1 : 0;
#endif

    return HI_SUCCESS;
}

hi_s32 hi_drv_sys_get_hdr10_support(hi_u32 *hdr10_support)
{
    volatile hi_reg_peri *reg_peri = drv_sys_get_peri_reg_ptr();

    if (hdr10_support == HI_NULL) {
        HI_ERR_SYS("support_hdr10 is null\n");
        return HI_FAILURE;
    }
    if (reg_peri == HI_NULL) {
        HI_ERR_SYS("null pointer!\n");
        return HI_FAILURE;
    }

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
    switch (reg_peri->PERI_SOC_FUSE_0 & 0x08000000) {
        case 0x0:
            *hdr10_support = 0x1;
            break;
        default:
            *hdr10_support = 0x0;
            break;
    }
#else
    *hdr10_support = 0x0;
#endif

    return HI_SUCCESS;
}

hi_s32 hi_drv_sys_get_dolby_vision_support(hi_u32 *dolby_vision_support)
{
    volatile hi_reg_peri *reg_peri = drv_sys_get_peri_reg_ptr();

    if (dolby_vision_support == HI_NULL) {
        HI_ERR_SYS("support_dolby_vision is null\n");
        return HI_FAILURE;
    }
    if (reg_peri == HI_NULL) {
        HI_ERR_SYS("null pointer!\n");
        return HI_FAILURE;
    }

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
    switch (reg_peri->PERI_SOC_FUSE_0 & 0x10000000) {
        case 0x0:
            *dolby_vision_support = 0x1;
            break;
        default:
            *dolby_vision_support = 0x0;
            break;
    }
#else
    *dolby_vision_support = 0x0;
#endif

    return HI_SUCCESS;
}

/* 1: support; 0: not_support */
hi_s32 hi_drv_sys_get_dts_support(hi_u32 *dts_support)
{
    volatile hi_reg_peri *reg_peri = drv_sys_get_peri_reg_ptr();

    if (dts_support == HI_NULL) {
        HI_ERR_SYS("support_dts is null\n");
        return HI_FAILURE;
    }
    if (reg_peri == HI_NULL) {
        HI_ERR_SYS("null pointer!\n");
        return HI_FAILURE;
    }

#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
    *dts_support = reg_peri->CHIPSET_INFO.bits.dts_flag;
#else
    *dts_support = (reg_peri->PERI_SOC_FUSE_3 & 0x4) == 0 ? 0 : 1;
#endif

    return HI_SUCCESS;
}

/* 1: support; 0: not_support */
hi_s32 hi_drv_sys_get_rovi_support(hi_u32 *rovi_support)
{
    volatile hi_reg_peri *reg_peri = drv_sys_get_peri_reg_ptr();

    if (rovi_support == HI_NULL) {
        HI_ERR_SYS("support_rovi is null\n");
        return HI_FAILURE;
    }
    if (reg_peri == HI_NULL) {
        HI_ERR_SYS("null pointer!\n");
        return HI_FAILURE;
    }

#if !defined(CHIP_TYPE_HI3796CV300) && !defined(CHIP_TYPE_HI3751V900)
    *rovi_support = reg_peri->PERI_SOC_FUSE_0.bits.mven;
#else
    *rovi_support = (reg_peri->PERI_SOC_FUSE_0 & 0x01000000) == 0 ? 0 : 1;
#endif

    return HI_SUCCESS;
}

/* 1: support; 0: not_support */
hi_s32 hi_drv_sys_get_advca_support(hi_u32 *advca_support)
{
    hi_u32 advca_flag;
    hi_u32 *advca_vir_addr = HI_NULL;
    hi_u8 *otp_base_vir_addr = drv_sys_get_otp_base_vir_addr();

#define SYS_CHIP_NON_ADVCA 0x2a13c812
#define SYS_CHIP_ADVCA     0x6edbe953

    if (advca_support == HI_NULL) {
        HI_ERR_SYS("support_advca is null\n");
        return HI_FAILURE;
    }
    if (otp_base_vir_addr == HI_NULL) {
        HI_ERR_SYS("null pointer!\n");
        return HI_FAILURE;
    }

#if defined(CHIP_TYPE_HI3798CV200) || \
    defined(CHIP_TYPE_HI3798MV200) || \
    defined(CHIP_TYPE_HI3798MV300) || \
    defined(CHIP_TYPE_HI3798MV310)
#define SYS_CHIP_ID_WORD_ADDR_OFFSET (0x0128)
#else
#define SYS_CHIP_ID_WORD_ADDR_OFFSET (0x0060)
#endif

    advca_vir_addr = (hi_u32 *)(otp_base_vir_addr + SYS_CHIP_ID_WORD_ADDR_OFFSET);
    if (advca_vir_addr == HI_NULL) {
        HI_ERR_SYS("pu32AdvcaVirAddr invalid!\n");
        return HI_FAILURE;
    }

    advca_flag = *((volatile unsigned int*)advca_vir_addr);
    if (advca_flag == SYS_CHIP_ADVCA) {
        *advca_support = 1;
    } else if (advca_flag == SYS_CHIP_NON_ADVCA) {
        *advca_support = 0;
    } else {
        HI_ERR_SYS("invalid advca flag\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_sys_get_mem_cfg(hi_u32 *mem_size, hi_u32 *mmz_size)
{
    hi_s32 ret;

    if (mem_size == HI_NULL) {
        HI_ERR_SYS("mem_size is null\n");
        return HI_FAILURE;
    }

    if (mmz_size == HI_NULL) {
        HI_ERR_SYS("mmz_size is null\n");
        return HI_FAILURE;
    }

    ret = get_mem_size(mem_size, HIKAPI_GET_RAM_SIZE);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("get ram size failed\n");
        return HI_FAILURE;
    }

    ret = get_mem_size(mmz_size, HIKAPI_GET_CMA_SIZE);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("get mmz size failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_sys_get_smp_support(hi_bool *smp_support)
{
    if (smp_support == HI_NULL) {
        HI_ERR_SYS("attr is null\n");
        return HI_FAILURE;
    }

#if defined(HI_SMP_SUPPORT)
    *smp_support = HI_TRUE;
#else
    *smp_support = HI_FALSE;
#endif

    return HI_SUCCESS;
}

hi_s32 hi_drv_sys_disable_video_codec(hi_sys_video_codec video_codec)
{
    SYS_K_LOCK();

    g_video_codec = (hi_u32)g_video_codec & (~((hi_u32)video_codec));

    SYS_K_UNLOCK();

    return HI_SUCCESS;
}

hi_sys_video_codec hi_drv_sys_get_video_codec_support(hi_void)
{
    return g_video_codec;
}

typedef struct {
    hi_u32 start_addr;
    hi_u32 end_addr;
} hi_sys_addr_range;

static hi_sys_addr_range g_sys_reg_range[] = {
    {HI_REG_SYS_BASE_ADDR,  HI_REG_SYS_END_ADDR},
    {HI_REG_PERI_BASE_ADDR, HI_REG_PERI_END_ADDR},
    {HI_REG_IO_BASE_ADDR,   HI_REG_IO_END_ADDR},
    {HI_REG_CRG_BASE_ADDR,  HI_REG_CRG_END_ADDR},
    {HI_REG_DDR_BASE_ADDR,  HI_REG_DDR_END_ADDR},
    {HI_REG_OTP_BASE_ADDR,  HI_REG_OTP_END_ADDR},
};

static hi_s32 drv_sys_check_reg_addr(hi_u32 reg_addr, hi_bool is_write)
{
    hi_s32 i = 0;
    hi_u32 reg_range_cnt;

    /* attention: write not support the otp register */
    reg_range_cnt = sizeof(g_sys_reg_range) / sizeof(hi_sys_addr_range);
    if (is_write == HI_TRUE) {
        reg_range_cnt--;
    }

    for (i = 0; i < reg_range_cnt; i++) {
        if ((g_sys_reg_range[i].start_addr <= reg_addr) && ((g_sys_reg_range[i].end_addr & (~0x3)) >= reg_addr)) {
            return HI_SUCCESS;
        }
    }

    return HI_FAILURE;
}

hi_s32 hi_drv_sys_write_register(const hi_u32 reg_addr, const hi_u32 value)
{
    hi_s32 ret;
    hi_void *virt = HI_NULL;

    ret = drv_sys_check_reg_addr(reg_addr, HI_TRUE);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("invalid reg_addr 0x%x\n", reg_addr);
        return HI_FAILURE;
    }

    SYS_K_LOCK();

    virt = osal_ioremap(reg_addr, SYS_REGISTER_SIZE);
    osal_writel(value, virt);
    osal_iounmap(virt);

    SYS_K_UNLOCK();

    return HI_SUCCESS;
}

hi_s32 hi_drv_sys_read_register(const hi_u32 reg_addr, hi_u32 *value)
{
    hi_s32 ret;
    hi_void *virt = HI_NULL;

    if (value == HI_NULL) {
        HI_ERR_SYS("Null point!\n");
        return HI_FAILURE;
    }

    ret = drv_sys_check_reg_addr(reg_addr, HI_FALSE);
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("invalid reg_addr 0x%x\n", reg_addr);
        return HI_FAILURE;
    }

    SYS_K_LOCK();

    virt = osal_ioremap(reg_addr, SYS_REGISTER_SIZE);
    *value = osal_readl(virt);
    osal_iounmap(virt);

    SYS_K_UNLOCK();

    return HI_SUCCESS;
}

volatile hi_reg_sys_ctrl *hi_drv_sys_get_ctrl_reg_ptr(hi_void)
{
    return drv_sys_get_ctrl_reg_ptr();
}

volatile hi_reg_peri *hi_drv_sys_get_peri_reg_ptr(hi_void)
{
    return drv_sys_get_peri_reg_ptr();
}

volatile hi_reg_io *hi_drv_sys_get_io_reg_ptr(hi_void)
{
    return drv_sys_get_io_reg_ptr();
}

volatile hi_reg_crg *hi_drv_sys_get_crg_reg_ptr(hi_void)
{
    return drv_sys_get_crg_reg_ptr();
}

/* define otp map offset */
#define SYS_SC_ID_OFFSET 0x10
#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
#define SYS_OTP_MAP_LOTID0  0x300
#define SYS_OTP_MAP_LOTID4  0x304
#define SYS_SITE_NUM_OFFSET 0x310
#endif

hi_u64 hi_drv_sys_get_die_id_64bit(hi_void)
{
    hi_u64 die_id;
    /* for 57 low bits */
    hi_u64 lot_id_sum;
    hi_u64 lot_id_0;
    hi_u64 lot_id_1;
    hi_u64 wafer_id;
    hi_u64 die_xy;
    hi_u64 low_57_bits;
    /* for 7 high bits */
    hi_u64 sc_sys_id;
    hi_u64 high_7_bits;
    hi_u64 high_1_to_0_bits;
    hi_u64 high_4_to_2_bits;
    hi_u64 high_5_bit;
    hi_u64 high_6_bit;
    volatile hi_reg_sys_ctrl *sys_ctrl = drv_sys_get_ctrl_reg_ptr();
    hi_u8 *otp_base_vir_addr = drv_sys_get_otp_base_vir_addr();

    if (sys_ctrl == HI_NULL || otp_base_vir_addr == HI_NULL) {
        HI_ERR_SYS("null pointer!\n");
        return 0;
    }

    SYS_K_LOCK();

    lot_id_0 = (hi_u64)SYS_REG_READ_BITS(otp_base_vir_addr + SYS_OTP_MAP_LOTID0, 0xfffffff0) >> 0x4;
    lot_id_1 = SYS_REG_READ_BITS(otp_base_vir_addr + SYS_OTP_MAP_LOTID4, 0xff);
    lot_id_1 = lot_id_1 << 28; /* shift 28 */
    lot_id_sum = lot_id_0 | lot_id_1; /* lot_id_sum is 36 bit */
    /* wafer_id is 5 bit */
    wafer_id = ((hi_u64)SYS_REG_READ_BITS(otp_base_vir_addr + SYS_OTP_MAP_LOTID4, 0x1f00) >> 0x8);
    /* die_xy is 16 bit */
    die_xy = ((hi_u64)SYS_REG_READ_BITS(otp_base_vir_addr + SYS_OTP_MAP_LOTID4, 0x1fffe000) >> 13); /* shift 13 bits */

    /* put lot_id_sum wafer_id and die_xy together */
    low_57_bits = lot_id_sum | (wafer_id << 36) | (die_xy << 41); /* wafer_id shift 36, die_xy shift 41 */

    /* get the high 7 bits */
    sc_sys_id = SYS_REG_READ_BITS((hi_u8 *)sys_ctrl + SYS_SC_ID_OFFSET, 0xffffffff);
    high_1_to_0_bits = (sc_sys_id & 0x30) >> 0x4;
    high_4_to_2_bits = (sc_sys_id & 0x700) >> 0x8;
    high_5_bit = (sc_sys_id & 0x80000) >> 19; /* shift 19 bits  */
    high_6_bit = (sc_sys_id & 0x800000) >> 23; /* shift 23 bits */
    high_7_bits = (high_6_bit << 0x6) | (high_5_bit << 0x5) | (high_4_to_2_bits << 0x2) | high_1_to_0_bits;

    /* get the 64 bits chip dieID */
    die_id = (high_7_bits << 57) | low_57_bits; /* shift 57 */

    SYS_K_UNLOCK();

    return die_id;
}

/* get the register value on address 0x253(bit[31~30]) of otp */
extern char *get_chiptrim_value(char *buf, unsigned int buflen, unsigned int offset);
extern int get_chiptrim_size(void);

hi_u32 hi_drv_sys_get_die_id(hi_void)
{
    hi_u32 die_id;

#if defined(CHIP_TYPE_HI3798MV200) || defined(CHIP_TYPE_HI3798MV300) || \
    defined(CHIP_TYPE_HI3798MV310) || defined(CHIP_TYPE_HI3796MV200) || \
    defined(CHIP_TYPE_HI3716MV430) || defined(CHIP_TYPE_HI3716MV450)

    hi_u32 wafer_id;
    hi_u32 die_xy;
    hi_u32 die_x_7_bits;
    hi_u32 die_y_7_bits;
    hi_u32 shadow_tmp;
    hi_u32 hour;
    hi_u32 minute;
    hi_u32 site_num     = 0;
    hi_u8 *otp_base_vir_addr = drv_sys_get_otp_base_vir_addr();

    if (otp_base_vir_addr == HI_NULL) {
        HI_ERR_SYS("null pointer!\n");
        return 0;
    }

    SYS_K_LOCK();

    /* get wafer_id and die_xy register value */
    /* wafer_id is 5 bit */
    wafer_id = ((hi_u32)SYS_REG_READ_BITS(otp_base_vir_addr + SYS_OTP_MAP_LOTID4, 0x1f00) >> 0x8);
    /* die_xy is 16 bit */
    die_xy = ((hi_u32)SYS_REG_READ_BITS(otp_base_vir_addr + SYS_OTP_MAP_LOTID4, 0x1fffe000) >> 13); /* shift 13 bits */
    die_x_7_bits = die_xy & 0x7f;
    die_y_7_bits = (die_xy & 0x7f00) >> 0x8;

    /* get hour and minute register value */
    shadow_tmp = (hi_u32)SYS_REG_READ_BITS(otp_base_vir_addr + SYS_OTP_MAP_LOTID8, 0x03ff8000) >> 15; /* shift 15 bit */
    hour = shadow_tmp & 0x1f;
    minute = (shadow_tmp & 0x7e0) >> 0x5;

#if (!defined(CHIP_TYPE_HI3796MV200) && !defined(CHIP_TYPE_HI3716MV450) && !defined(CHIP_TYPE_HI3716MV430))
    /* get site_num, the site_num is bit 30 and bit 31 in offset 4, and the data length is 4 */
    if (unlikely(get_chiptrim_size() < 0x8)) {
        SYS_K_UNLOCK();
        HI_ERR_SYS("no enough data size for SiteNum\n");
        return HI_FAILURE;
    }

    if (unlikely(!get_chiptrim_value((hi_char *)(&site_num), 0x4, 0x4))) {
        SYS_K_UNLOCK();
        HI_ERR_SYS("get SiteNum error\n");
        return HI_FAILURE;
    }
#else
    site_num = SYS_REG_READ_BITS((hi_u8 *)otp_base_vir_addr + SYS_SITE_NUM_OFFSET, 0xFFFFFFFF);
#endif

    site_num = (site_num & 0xc0000000) >> 30; /* shift 30 bits */

    /* put site_num ... wafer_id and die_xy together */
    die_id = (site_num << 30) | (hour << 25) | (minute << 19) | /* shift 30, 25, 19 bits */
              (wafer_id << 14) | (die_x_7_bits << 7) | die_y_7_bits; /* shift 14, 7 bits */

    SYS_K_UNLOCK();
#else
    die_id = 0x0;
#endif

    return die_id;
}

hi_s32 hi_drv_sys_set_irq_affinity(const hi_mod_id module_id, const hi_u32 irq_num, const hi_char *name)
{
    hi_s32 ret;
    hi_s32 cpu_index = 0;
    hi_u32 irq;

    if (module_id >= HI_ID_MAX) {
        HI_ERR_SYS("Invalid module id!\n");
        return HI_FAILURE;
    }

    irq = hi_drv_sys_get_irq_by_name((hi_char *)name);
    if (irq == -1) {
        HI_ERR_SYS("hi_drv_sys_get_irq_by_name:%s failed.\n", name);
        return HI_FAILURE;
    }

    /* only SMP cores and core numbers must be more than or equal to 2 */
    if (num_online_cpus() < 0x2) {
        HI_INFO_SYS("No need to set irq affinity!\n");
        return HI_SUCCESS;
    }

#if (!defined(CHIP_TYPE_HI3716MV430) && !defined(CHIP_TYPE_HI3716MV450)) /* exclude 16mv430 and 16mv450 chipset */
    switch (module_id) {
        case HI_ID_IR:
        case HI_ID_I2C:
        case HI_ID_SCI:
        case HI_ID_WDG:
        case HI_ID_GPIO:
        case HI_ID_GPIO_I2C:
        case HI_ID_KEYLED: {
            cpu_index = SYS_GENERAL_DEV_CPU_INDEX;
            HI_INFO_SYS("Gereral device interrupts use CPU %d!\n", SYS_GENERAL_DEV_CPU_INDEX);
            break;
        }

#ifdef HI_FPGA_SUPPORT
#ifdef HI_TEE_SUPPORT
        /* TO avoid VPSS's reg r/w operation collision with VDH. (they are both on FPGA4) */
        case HI_ID_VFMW:
        case HI_ID_VPSS: {
            cpu_index = 0;
            break;
        }
#endif
#endif
        default:
            cpu_index = SYS_MEDIA_CPU_INDEX;
            HI_INFO_SYS("Media interrupts use CPU %d!\n", SYS_MEDIA_CPU_INDEX);
            break;
    }
#else /* only for 2 core cpu */
    switch (module_id) {
        case HI_ID_WIN:
        case HI_ID_DISP: {
            cpu_index = SYS_MEDIA_CPU_INDEX;
            HI_INFO_SYS("Vdp interrupts use CPU %d!\n", SYS_MEDIA_CPU_INDEX);
            break;
        }
        default:
            cpu_index = SYS_FIRST_CPU_INDEX;
            HI_INFO_SYS("Gereral interrupts CPU %d!\n", SYS_FIRST_CPU_INDEX);
            break;
    }
#endif

    if (!cpu_online(cpu_index)) {
        HI_ERR_SYS("cpu num %d is offline, irq num is %u, name is %s!\n", cpu_index, irq, name);
        return HI_FAILURE;
    }

    ret = irq_set_affinity(irq, cpumask_of(cpu_index));
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("cpumask error, cpu num is %d, irq num is %u, name is %s!\n", cpu_index, irq, name);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_sys_get_cpu_secure_mode(hi_bool *secure_mode)
{
    hi_u32 reg_value;
    hi_u8 *otp_base_vir_addr = drv_sys_get_otp_base_vir_addr();

    if (secure_mode == HI_NULL) {
        HI_ERR_SYS("secure_mode is null\n");
        return HI_FAILURE;
    }
    if (otp_base_vir_addr == HI_NULL) {
        HI_ERR_SYS("null pointer!\n");
        return HI_FAILURE;
    }

#if defined(CHIP_TYPE_HI3796CV300) || defined(CHIP_TYPE_HI3751V900)
    reg_value = *((volatile unsigned int *)(otp_base_vir_addr + 0x4));

    /* TEE disable, boot kernel all run in secure mode */
    if (((reg_value >> 0x8) & 0xff) == 0x42) {
        *secure_mode = HI_TRUE;
    } else {
        /* TEE enable, boot kernel all run in Non-secure mode, only secure os run in secure mode */
        *secure_mode = HI_FALSE;
    }

    return HI_SUCCESS;
#else
    HI_UNUSED(reg_value);
    HI_ERR_SYS("only 96mv200 support get cpu secure mode!\n");
    return HI_FAILURE;
#endif
}

/*
 * value: 0: normal mode, other: special mode.
 * EPLL0 address: 0xF8A22030, 0x12560418: special mode, other: normal mode.
 */
hi_s32 hi_drv_sys_get_flag(hi_u32 *flag)
{
    return HI_FAILURE;
}

hi_s32 hi_drv_sys_get_audio_op_mode(hi_void)
{
    return -1;
}

hi_s32 hi_drv_sys_get_disp_vdac_drive_mode(hi_void)
{
    return -1;
}

hi_bool hi_drv_sys_check_phy_addr(hi_ulong addr)
{
#if defined(CHIP_TYPE_HI3716MV430)
    if ((addr == 0) || (addr == (hi_ulong) - 1) || (addr < __pa((uintptr_t)_etext))) {
        HI_WARN_SYS("invalid address 0x%lx.\n", addr);
        return HI_FALSE;
    } else {
        return HI_TRUE;
    }
#else
    /* only 16mv430 support check physical address, other chipset just return HI_TRUE. */
    return HI_TRUE;
#endif
}

hi_slong hi_drv_user_copy(struct file *file, hi_u32 cmd, hi_ulong arg, hi_ioctl func)
{
    hi_char  sbuf[128]; /* array num is 128 */
    hi_void  *mbuf = NULL;
    hi_void  *parg = NULL;
    hi_s32   err  = -EINVAL;

    /*  Copy arguments into temp kernel buffer  */
    switch (_IOC_DIR(cmd)) {
        case _IOC_NONE:
            parg = NULL;
            break;
        case _IOC_READ:
        case _IOC_WRITE:
        case (_IOC_WRITE | _IOC_READ):
            if (_IOC_SIZE(cmd) <= sizeof(sbuf)) {
                parg = sbuf;
            } else {
                hi_u32 buff_size = _IOC_SIZE(cmd);
                /* too big to allocate from stack */
                mbuf = HI_KZALLOC(HI_ID_MEM, buff_size, GFP_KERNEL);
                if (NULL == mbuf) {
                    HI_FATAL_SYS("malloc cmd buffer failed\n");
                    return -ENOMEM;
                }
                parg = mbuf;
            }

            err = -EFAULT;
            if (_IOC_DIR(cmd) & _IOC_WRITE) {
                if ((void __user *)(uintptr_t)arg == HI_NULL) {
                    HI_FATAL_SYS("arg is null\n");
                    goto out;
                }

                if (osal_copy_from_user(parg, (void __user *)(uintptr_t)arg, _IOC_SIZE(cmd))) {
                    HI_FATAL_SYS("osal_copy_from_user failed, when use ioctl, \
                            the para must be a address, cmd=0x%x\n", cmd);
                    goto out;
                }
            }
            break;
    }

    /* call driver */
    err = func(file, cmd, (parg));
    if (err == -ENOIOCTLCMD) {
        err = -EINVAL;
    }
    if (err < 0) {
        goto out;
    }

    /*  Copy results into user buffer  */
    switch (_IOC_DIR(cmd)) {
        case _IOC_READ:
        case (_IOC_WRITE | _IOC_READ):
            if (((void __user *)(uintptr_t)arg != HI_NULL) && (parg != HI_NULL)) {
                if (osal_copy_to_user((void __user *)(uintptr_t)arg, parg, _IOC_SIZE(cmd))) {
                    HI_FATAL_SYS("osal_copy_to_user failed, when use ioctl, \
                            the para must be a address, cmd=0x%x\n", cmd);
                    err = -EFAULT;
                }
            } else {
                HI_FATAL_SYS("parg is null!\n");
            }
            break;
    }

out:
    if (mbuf != HI_NULL) {
        HI_KFREE(HI_ID_MEM, mbuf);
        mbuf = HI_NULL;
    }
    return err;
}

hi_s32 drv_sys_get_boot_version(hi_char *version, hi_u32 len)
{
    const hi_u8 *boot_version = get_sdkversion();

    if (version == HI_NULL || len == 0) {
        HI_WARN_SYS("drv_sys_get_boot_version failure line:%d\n", __LINE__);
        return HI_FAILURE;
    }

    if (boot_version != HI_NULL) {
        if (len > strlen(boot_version)) {
            len = strlen(boot_version);
        } else {
            HI_ERR_SYS("drv_sys_get_boot_version input buf too short\n");
            return HI_FAILURE;
        }

        memcpy(version, boot_version, len);

        version[len] = '\0';

        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

hi_s32 drv_sys_ioctl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = -ENOIOCTLCMD;
    hi_sys_version *sys_version = HI_NULL;

    switch (cmd) {
        case SYS_GET_VERSION: {
            sys_version = (hi_sys_version *)arg;

            hi_drv_sys_get_chip_version(&sys_version->chip_type, &sys_version->chip_version);
            drv_sys_get_boot_version(sys_version->boot_version, sizeof(sys_version->boot_version));
            hi_drv_sys_get_sos_version(sys_version->sos_version, sizeof(sys_version->sos_version));

            ret = HI_SUCCESS;
            break;
        }
        case SYS_GET_TIME_STAMP_MS: {
            hi_u32 *time_ms = (hi_u32 *)arg;
            *time_ms = hi_drv_sys_get_time_stamp_ms();
            ret = HI_SUCCESS;
            break;
        }
        case SYS_GET_DOLBY_SUPPORT: {
            ret = hi_drv_sys_get_dolby_support((hi_u32 *)arg);
            break;
        }
        case SYS_GET_DTS_SUPPORT: {
            ret = hi_drv_sys_get_dts_support((hi_u32 *)arg);
            break;
        }
        case SYS_GET_ADVCA_SUPPORT: {
            ret = hi_drv_sys_get_advca_support((hi_u32 *)arg);
            break;
        }
        case SYS_GET_MACRO_VISION_SUPPORT: {
            ret = hi_drv_sys_get_rovi_support((hi_u32 *)arg);
            break;
        }
        case SYS_GET_HDR10_SUPPORT: {
            ret = hi_drv_sys_get_hdr10_support((hi_u32 *)arg);
            break;
        }
        case SYS_GET_DOLBY_VISION_SUPPORT: {
            ret = hi_drv_sys_get_dolby_vision_support((hi_u32 *)arg);
            break;
        }
        case SYS_GET_DDR_CFG: {
            sys_mem_info *mem = (sys_mem_info*)arg;
            hi_u32 mem_size = 0;
            hi_u32 mmz_size = 0;

            if (mem == HI_NULL) {
                ret = HI_FAILURE;
                break;
            }

            ret = hi_drv_sys_get_mem_cfg(&mem_size, &mmz_size);
            if (ret == HI_SUCCESS) {
                mem->mem_size = mem_size;
                mem->mmz_size = mmz_size;
            }

            break;
        }
        case SYS_GET_DIE_ID_64: {
            hi_u64 *die_id = (hi_u64*)arg;

            if (die_id != HI_NULL) {
                *die_id = hi_drv_sys_get_die_id_64bit();
                ret = HI_SUCCESS;
            }
            break;
        }
        case SYS_GET_DIE_ID_32: {
            hi_u32 *die_id = (hi_u32*)arg;

            if (die_id != HI_NULL) {
                *die_id = hi_drv_sys_get_die_id();
                ret = HI_SUCCESS;
            }
            break;
        }
        case SYS_GET_CHIP_PACKAGE_TYPE: {
            hi_chip_package_type *package_type = (hi_chip_package_type *)arg;
            *package_type = hi_drv_sys_get_chip_package_type();
            ret = HI_SUCCESS;
            break;
        }
        case SYS_GET_SMP_SUPPORT: {
            ret = hi_drv_sys_get_smp_support((hi_bool *)arg);
            break;
        }
        case SYS_DISABLE_VIDEO_CODEC: {
            ret = hi_drv_sys_disable_video_codec(*((hi_sys_video_codec *)arg));
            break;
        }
        case SYS_WRITE_REGISTER: {
            hi_sys_io_reg *reg = (hi_sys_io_reg *)arg;

            ret = hi_drv_sys_write_register(reg->base_addr, reg->reg_value);

            break;
        }
        case SYS_READ_REGISTER: {
            hi_sys_io_reg *reg = (hi_sys_io_reg *)arg;

            ret = hi_drv_sys_read_register(reg->base_addr, &reg->reg_value);

            break;
        }
        default:
            HI_ERR_SYS("unknown command 0x%x\n", cmd);
    }

    return ret;
}

#ifdef HI_PROC_SUPPORT
static hi_void drv_sys_get_chip_name(hi_chip_type chip_type, hi_char **chip_name)
{
    if (chip_name == HI_NULL) {
        HI_ERR_SYS("Invalid parament pChipName[%#x]!\n", chip_name);
        return;
    }

    switch (chip_type) {
        case CHIP_TYPE_HI3716M: {
            *chip_name = "Hi3716M";
            break;
        }
        case CHIP_TYPE_HI3798C: {
            *chip_name = "Hi3798C";
            break;
        }
        case CHIP_TYPE_HI3796M: {
            *chip_name = "Hi3796M";
            break;
        }
        case CHIP_TYPE_HI3798M: {
            *chip_name = "Hi3798M";
            break;
        }
        case CHIP_TYPE_HI3798M_H: {
            *chip_name = "Hi3798M_H";
            break;
        }
        case CHIP_TYPE_HI3796C: {
            *chip_name = "Hi3796C";
            break;
        }
        default:
            *chip_name = "UNKNOWN";
    }

    return;
}

static hi_void drv_sys_get_chip_package_type(hi_chip_package_type package_type, hi_char **package_name)
{
    if (package_name == HI_NULL) {
        HI_ERR_SYS("Invalid parament pPackageName[%#x]!\n", package_name);
        return;
    }

    switch (package_type) {
        case CHIP_PACKAGE_TYPE_BGA_14_14: {
            *package_name = "BGA 14*14";
            break;
        }
        case CHIP_PACKAGE_TYPE_BGA_15_15: {
            *package_name = "BGA 15*15";
            break;
        }
        case CHIP_PACKAGE_TYPE_BGA_16_16: {
            *package_name = "BGA 16*16";
            break;
        }
        case CHIP_PACKAGE_TYPE_BGA_19_19: {
            *package_name = "BGA 19*19";
            break;
        }
        case CHIP_PACKAGE_TYPE_BGA_21_21: {
            *package_name = "BGA 21*21";
            break;
        }
        case CHIP_PACKAGE_TYPE_BGA_23_23: {
            *package_name = "BGA 23*23";
            break;
        }
        case CHIP_PACKAGE_TYPE_BGA_31_31: {
            *package_name = "BGA 31*31";
            break;
        }
        case CHIP_PACKAGE_TYPE_BGA_35_35: {
            *package_name = "BGA 35*35";
            break;
        }
        case CHIP_PACKAGE_TYPE_QFP_216: {
            *package_name = "QFP 216";
            break;
        }
        default:
            *package_name = "UNKNOWN";
    }

    return;
}

static hi_s32 drv_sys_proc_show(hi_void *s, hi_void *arg)
{
    hi_chip_type         chip_type = CHIP_TYPE_BUTT;
    hi_chip_version      chip_version = 0;
    hi_chip_package_type package_type;
    hi_u32               dolby_support;
    hi_u32               hdr10_support;
    hi_u32               dolby_vision_support;
    hi_u32               dts_support;
    hi_u32               rovi_support;
    hi_u32               advca_support;
    hi_char             *chip_name = HI_NULL;
    hi_char             *package_name = HI_NULL;
    hi_u32               license_value;

#if defined(COMMON_TEE_SUPPORT) && defined(ADVCA_TYPE)
    hi_u8                version[96]; /* array num is 96 */
#endif

    hi_drv_sys_get_chip_version(&chip_type, &chip_version);

    osal_proc_print(s, "%s\n", g_common_version);

    drv_sys_get_chip_name(chip_type, &chip_name);

    if (chip_type == CHIP_TYPE_HI3798M_H) {
        osal_proc_print(s, "CHIP_VERSION: Hi3798MV%xH\n", chip_version);
    } else {
        osal_proc_print(s, "CHIP_VERSION: %sV%x\n", chip_name, chip_version);
    }

    package_type = hi_drv_sys_get_chip_package_type();
    if (package_type == CHIP_PACKAGE_TYPE_BUTT) {
        HI_WARN_SYS("invalid package type.\n");
    }
    drv_sys_get_chip_package_type(package_type, &package_name);
    osal_proc_print(s, "PACKAGE_TYPE: %s\n", package_name);

    osal_proc_print(s, "\n--------------------FEATURE LICENSE--------------------\n");

    /* 30 character before ":" and 3 space append */
    if (hi_drv_sys_get_dolby_support(&dolby_support) == HI_SUCCESS) {
        osal_proc_print(s, "DOLBY\033[25C:\033[3C%s\n", (dolby_support) ? "YES" : "NO");
    }

    if (hi_drv_sys_get_dts_support(&dts_support) == HI_SUCCESS) {
        osal_proc_print(s, "DTS\033[27C:\033[3C%s\n", (dts_support) ? "YES" : "NO");
    }

    if (hi_drv_sys_get_advca_support(&advca_support) == HI_SUCCESS) {
        osal_proc_print(s, "ADVCA\033[25C:\033[3C%s\n", (advca_support) ? "YES" : "NO");
    }

    if (hi_drv_sys_get_rovi_support(&rovi_support) == HI_SUCCESS) {
        osal_proc_print(s, "ROVI(Macrovision)\033[13C:\033[3C%s\n", (rovi_support) ? "YES" : "NO");
    }

    if (hi_drv_sys_get_hdr10_support(&hdr10_support) == HI_SUCCESS) {
        osal_proc_print(s, "HDR10\033[25C:\033[3C%s\n", (hdr10_support) ? "YES" : "NO");
    }

    if (hi_drv_sys_get_dolby_vision_support(&dolby_vision_support) == HI_SUCCESS) {
        osal_proc_print(s, "DolbyVision\033[19C:\033[3C%s\n", (dolby_vision_support) ? "YES" : "NO");
    }

#if defined(COMMON_TEE_SUPPORT) && defined(ADVCA_TYPE)
    if (hi_drv_sys_get_sos_version(version, sizeof(version)) == HI_SUCCESS) {
        osal_proc_print(s, "SecureOSVersion\033[15C:\033[3C%s\n", version);
    }
#endif

    if (hi_drv_get_license_support(HI_LICENSE_CPU_CAP, &license_value) == HI_SUCCESS) {
        osal_proc_print(s, "CPU\033[27C:\033[3C%s\n", (license_value == 0) ? "4 CORE" :
            ((license_value == 1) ? "8 CORE" : "NOT SUPPORT"));
    }

    if (hi_drv_get_license_support(HI_LICENSE_GPU_CAP, &license_value) == HI_SUCCESS) {
        osal_proc_print(s, "GPU\033[27C:\033[3C%s\n", (license_value == 0) ? "2 CORE" :
            ((license_value == 1) ? "6 CORE" : "NOT SUPPORT"));
    }

    if (hi_drv_get_license_support(HI_LICENSE_NPU_CAP, &license_value) == HI_SUCCESS) {
        osal_proc_print(s, "NPU\033[27C:\033[3C%s\n", (license_value == 1) ? "ENABLE" : "DISABLE");
    }

    if (hi_drv_get_license_support(HI_LICENSE_DECODE_CAP, &license_value) == HI_SUCCESS) {
        osal_proc_print(s, "DECODE\033[24C:\033[3C%s\n",
            (license_value == 0) ? "4KP60" :
            ((license_value == 1) ? "4KP120" :
            ((license_value == 0x2) ? "8KP60" :
            ((license_value == 0x3) ? "8KP120" : "NOT SUPPORT"))));
    }

    if (hi_drv_get_license_support(HI_LICENSE_DECODE_FORMAT, &license_value) == HI_SUCCESS) {
        osal_proc_print(s, "DECODE AVS3\033[19C:\033[3C%s\n", (license_value & 0x01) ? "ENABLE" : "DISABLE");
        osal_proc_print(s, "DECODE AV1\033[20C:\033[3C%s\n", (license_value & 0x02) ? "ENABLE" : "DISABLE");
    }

    if (hi_drv_get_license_support(HI_LICENSE_ADSP_EN, &license_value) == HI_SUCCESS) {
        osal_proc_print(s, "AIDSP\033[25C:\033[3C%s\n", (license_value & 0x01) ? "ENABLE" : "DISABLE");
        osal_proc_print(s, "ADSP\033[26C:\033[3C%s\n", (license_value & 0x02) ? "ENABLE" : "DISABLE");
    }

    if (hi_drv_get_license_support(HI_LICENSE_ENCODE_EN, &license_value) == HI_SUCCESS) {
        osal_proc_print(s, "ENCODE\033[24C:\033[3C%s\n", (license_value == 1) ? "ENABLE" : "DISABLE");
    }

    if (hi_drv_get_license_support(HI_LICENSE_DISPLAY2_EN, &license_value) == HI_SUCCESS) {
        osal_proc_print(s, "DISPLAY2\033[22C:\033[3C%s\n", (license_value == 1) ? "ENABLE" : "DISABLE");
    }

    if (hi_drv_get_license_support(HI_LICENSE_HDMI_RX_EN, &license_value) == HI_SUCCESS) {
        osal_proc_print(s, "HDMI RX\033[23C:\033[3C%s\n", (license_value == 1) ? "ENABLE" : "DISABLE");
    }

    if (hi_drv_get_license_support(HI_LICENSE_TSI_EN, &license_value) == HI_SUCCESS) {
        osal_proc_print(s, "TSI\033[27C:\033[3C%s\n",
            (license_value == 0) ? "DISABLE" :
            ((license_value == 1) ? "4 PORT" :
            ((license_value == 0x2) ? "8 PORT" :
            ((license_value == 0x3) ? "16 PORT" : "NOT SUPPORT"))));
    }

    if (hi_drv_get_license_support(HI_LICENSE_PCIE_EN, &license_value) == HI_SUCCESS) {
        osal_proc_print(s, "PCIE\033[26C:\033[3C%s\n", (license_value == 1) ? "ENABLE" : "DISABLE");
    }

    if (hi_drv_get_license_support(HI_LICENSE_SATA_EN, &license_value) == HI_SUCCESS) {
        osal_proc_print(s, "SATA\033[26C:\033[3C%s\n", (license_value == 1) ? "ENABLE" : "DISABLE");
    }

    if (hi_drv_get_license_support(HI_LICENSE_USB3_EN, &license_value) == HI_SUCCESS) {
        osal_proc_print(s, "USB3.0\033[24C:\033[3C%s\n", (license_value == 1) ? "ENABLE" : "DISABLE");
    }

    return HI_SUCCESS;
}

hi_s32 drv_sys_add_proc(hi_void)
{
    osal_proc_entry *fn_set = HI_NULL;

    fn_set = osal_proc_add(HI_MOD_SYS_NAME, strlen(HI_MOD_SYS_NAME));
    if (fn_set == HI_NULL) {
        HI_ERR_SYS("add proc failed!\n");
        return HI_FAILURE;
    }

    fn_set->read = drv_sys_proc_show;

    return HI_SUCCESS;
}

hi_void drv_sys_remove_proc(hi_void)
{
    osal_proc_remove(HI_MOD_SYS_NAME, strlen(HI_MOD_SYS_NAME));
}
#endif

EXPORT_SYMBOL(hi_drv_sys_get_chip_version);
EXPORT_SYMBOL(hi_drv_sys_get_chip_package_type);
EXPORT_SYMBOL(hi_drv_sys_get_time_stamp_ms);
EXPORT_SYMBOL(hi_drv_sys_get_dolby_support);
EXPORT_SYMBOL(hi_drv_sys_get_hdr10_support);
EXPORT_SYMBOL(hi_drv_sys_get_dolby_vision_support);
EXPORT_SYMBOL(hi_drv_sys_get_dts_support);
EXPORT_SYMBOL(hi_drv_sys_get_advca_support);
EXPORT_SYMBOL(hi_drv_sys_get_rovi_support);
EXPORT_SYMBOL(hi_drv_sys_get_mem_cfg);
EXPORT_SYMBOL(hi_drv_sys_get_die_id_64bit);
EXPORT_SYMBOL(hi_drv_sys_get_die_id);
EXPORT_SYMBOL(hi_drv_sys_set_irq_affinity);
EXPORT_SYMBOL(hi_drv_sys_get_cpu_secure_mode);
EXPORT_SYMBOL(hi_drv_sys_get_flag);
EXPORT_SYMBOL(hi_drv_sys_get_audio_op_mode);
EXPORT_SYMBOL(hi_drv_sys_get_disp_vdac_drive_mode);
EXPORT_SYMBOL(hi_drv_sys_disable_video_codec);
EXPORT_SYMBOL(hi_drv_sys_get_video_codec_support);
EXPORT_SYMBOL(hi_drv_sys_check_phy_addr);
EXPORT_SYMBOL(hi_drv_user_copy);
EXPORT_SYMBOL(hi_drv_sys_write_register);
EXPORT_SYMBOL(hi_drv_sys_read_register);
EXPORT_SYMBOL(hi_drv_sys_get_ctrl_reg_ptr);
EXPORT_SYMBOL(hi_drv_sys_get_peri_reg_ptr);
EXPORT_SYMBOL(hi_drv_sys_get_io_reg_ptr);
EXPORT_SYMBOL(hi_drv_sys_get_crg_reg_ptr);

