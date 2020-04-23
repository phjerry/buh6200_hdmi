/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb hal
 * Author: sdk
 * Create: 2016-01-01
 */

/* **************************** add include here************************************************* */
#include "hi_reg_common.h"

#include "drv_hifb_config.h"
#include "drv_hifb_adp.h"
#include "drv_hifb_hal.h"
#include "hi_register_vdp.h"

#ifdef HI_BUILD_IN_BOOT
#include "hifb_debug.h"
#include "hi_common.h"
#include "hi_gfx_debug.h"
#else
#include "drv_hifb_debug.h"
#endif
/* **************************** Macro Definition ************************************************ */
#define STRIDE_ALIGN 16

#define CONFIG_HIFB_CHECK_LAYER_SUPPORT(data)                    \
    if (HI_SUCCESS != hifb_hal_check_layer_id(__LINE__, data)) { \
        return;                                                  \
    }

#define CONFIG_HIFB_CHECK_GP_SUPPORT(data)                    \
    if (HI_SUCCESS != hifb_hal_check_gp_id(__LINE__, data)) { \
        return;                                               \
    }

#define VDP_LAYER_MAX 4
const static hi_u32 g_vdp_layer_reg_offset[VDP_LAYER_MAX] = {
    0x2F000, /* vid0 - vid1  0x2F000 4 reg offset */
    0x00000, /* vid1 - vid1  0x00000 4 reg offset */
    0x01000, /* vid2 - vid1  0x01000 4 reg offset */
    0x02000, /* vid3 - vid1  0x02000 4 reg offset */
};

/* **************************** Structure Definition ******************************************** */

/* **************************** Global Variable declaration ************************************* */
static volatile vdp_regs_type *g_optm_vdp_reg = NULL;

#define drv_hifb_hal_logo_num 2
#ifndef HI_BUILD_IN_BOOT
static hi_bool g_up_mute[drv_hifb_hal_logo_num] = { HI_TRUE, HI_TRUE };
#else
static hi_bool g_up_mute[drv_hifb_hal_logo_num] = { HI_FALSE, HI_FALSE };
#endif

/* **************************** API forward declarations **************************************** */

/* **************************** API realization ************************************************* */

/***************************************************************************************
 * func          : drv_hifb_hal_vdp_initial
 * description   : CNcomment: hal层保存寄存器相关信息 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_s32 drv_hifb_hal_vdp_initial(hi_void)
{
#ifdef HI_BUILD_IN_BOOT
    g_optm_vdp_reg = (volatile vdp_regs_type *)(CONFIG_VDP_REG_BASEADDR);
#else
    g_optm_vdp_reg = (volatile vdp_regs_type *)osal_ioremap_nocache(CONFIG_VDP_REG_BASEADDR, sizeof(vdp_regs_type));
    if (g_optm_vdp_reg == HI_NULL) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, CONFIG_VDP_REG_BASEADDR);
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, CONFIG_VDP_SIZE);
        GRAPHIC_DFX_ERR_FUNC(GRAPHIC_DFX_ERR_FUNCTION, HI_GFX_REG_MAP, FAILURE_TAG);
        return HI_FAILURE;
    }
#endif

    return HI_SUCCESS;
}
hi_void drv_hifb_hal_vdp_deinitial(hi_void)
{
#ifndef HI_BUILD_IN_BOOT
    if (g_optm_vdp_reg != HI_NULL) {
        osal_iounmap((hi_void *)g_optm_vdp_reg);
    }
#endif
    g_optm_vdp_reg = HI_NULL;

    return;
}

hi_u32 optm_vdp_reg_read(hi_u32 *a)
{
    return (*a);
}

hi_void optm_vdp_reg_write(hi_u32 *a, hi_u32 b)
{
    if (NULL != a) {
        *a = b;
    }
    return;
}

hi_s32 hifb_check_address(hi_u32 address)
{
    if ((address <= 0x10000) || (address == 0xffffffff)) {
        /* here print jusk ok */ /* 这个地方不要造成系统异常 */
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_XINT, address);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 hifb_hal_check_layer_id(hi_u32 line, hi_u32 data)
{
#ifdef CONFIG_HIFB_VERSION_2_0
    if ((0 != data) && (1 != data))
#elif defined(CONFIG_HIFB_VERSION_3_0)
    if ((0 != data) && (1 != data) && (2 != data))
#elif defined(CONFIG_HIFB_VERSION_4_0)
    if (data >= 4)
#else
    if (data >= OPTM_GFX_MAX)
#endif
    {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, line);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, data);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 hifb_hal_check_gp_id(hi_u32 line, hi_u32 data)
{
    if (0 != data) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, line);
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_UINT, data);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_u32 hifb_hal_get_gp_id_from_layer_id(hi_u32 data)
{
#ifdef CONFIG_HIFB_VERSION_3_0
    if ((1 == data) || (data >= 4)) {
        return 1;
    }
#elif CONFIG_HIFB_VERSION_4_0
    if (data == 3) {
        return 1;
    }
#else
    if (data >= 4) {
        return 1;
    }
#endif
    return 0;
}

static hi_u32 hifb_hal_get_gp_id(hi_u32 data)
{
    return data;
}

hi_void drv_hifb_hal_set_up_mute(hi_u32 layer_id, hi_bool mute)
{
    hi_u32 gp_id;

    gp_id = hifb_hal_get_gp_id_from_layer_id(layer_id);
    if (gp_id == 0) {
        g_up_mute[0] = mute;
    } else {
        g_up_mute[1] = mute;
    }
}

hi_void drv_hifb_hal_get_up_mute(hi_u32 layer_id, hi_bool *mute)
{
    hi_u32 gp_id;

    if (mute == NULL) {
        return;
    }

    gp_id = hifb_hal_get_gp_id_from_layer_id(layer_id);
    if (gp_id == 0) {
        *mute = g_up_mute[0];
    } else {
        *mute = g_up_mute[1];
    }
}

#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_disp_get_dhd1_field(hi_bool *is_bottom_field)
{
#ifdef CONFIG_HIFB_FIELD_BUFFER_SUPPORT
    volatile u_dhd1_state dhd1_state;

    if ((NULL != is_bottom_field) && (NULL != g_optm_vdp_reg)) {
        dhd1_state.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->dhd1_state.u32))));
        *is_bottom_field = dhd1_state.bits.bottom_field;
    }
#else
#endif
    return;
}

hi_void drv_hifb_hal_disp_get_dhd0_vtthd3(hi_ulong *expect_int_line_nums_for_end_call_back)
{
    /* * 从前消隐开始算的 * */
    if (NULL != expect_int_line_nums_for_end_call_back) {
        *expect_int_line_nums_for_end_call_back = g_optm_vdp_reg->dhd0_vtthd3.bits.vtmgthd3 -
                                                  (g_optm_vdp_reg->dhd0_vsync2.bits.vfb + 1);
    }
    return;
}

hi_void drv_hifb_hal_disp_get_dhd0_vtthd(hi_ulong *expect_int_line_nums_for_vo_call_back)
{
    /* * 从前消隐开始算的 * */
    if (NULL != expect_int_line_nums_for_vo_call_back) {
        *expect_int_line_nums_for_vo_call_back = g_optm_vdp_reg->dhd0_vtthd.bits.vtmgthd2 -
                                                 (g_optm_vdp_reg->dhd0_vsync2.bits.vfb + 1);
    }
    return;
}

hi_void drv_hifb_hal_disp_get_dhd0_state(hi_ulong *actual_int_line_nums_for_call_back,
                                         hi_ulong *hard_int_cnt_for_call_back)
{
    /* * 从后消隐开始算的 * */
    if (NULL != actual_int_line_nums_for_call_back) {
        *actual_int_line_nums_for_call_back = g_optm_vdp_reg->dhd0_state.bits.count_vcnt;
    }
    /* * 从后消隐开始算的 * */
    if (NULL != hard_int_cnt_for_call_back) {
        *hard_int_cnt_for_call_back = g_optm_vdp_reg->dhd0_state.bits.count_int;
    }
    return;
}

hi_void drv_hifb_hal_gfx_get_work_layer_addr(hi_u32 data, hi_u32 *addr)
{
    volatile u_gfx_work_addr gfx_work_addr;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gfx_work_addr.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_work_addr.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    if (NULL != addr) {
        *addr = gfx_work_addr.u32;
    }

    return;
}

hi_void drv_hifb_hal_gfx_get_will_work_layer_addr(hi_u32 data, hi_u32 *addr)
{
    volatile u_gfx_addr_l gfx_addr_l;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gfx_addr_l.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_addr_l.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    if (NULL != addr) {
        *addr = gfx_addr_l.u32;
    }

    return;
}
#endif

/* beg set hdr */
static hi_void hifb_gp_v2_setimrgb2lmsen(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_db_imap_rgb2lms_ctrl gp0_db_imap_rgb2lms_ctrl;

    gp0_db_imap_rgb2lms_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_rgb2lms_ctrl.u32))));

    gp0_db_imap_rgb2lms_ctrl.bits.im_rgb2lms_en = enable;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_rgb2lms_ctrl.u32))),
                       gp0_db_imap_rgb2lms_ctrl.u32);
    return;
}

static hi_void hifb_gp_v2_setimlms2ipten(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_db_imap_lms2ipt_ctrl gp0_db_imap_lms2ipt_ctrl;

    gp0_db_imap_lms2ipt_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_lms2ipt_ctrl.u32))));

    gp0_db_imap_lms2ipt_ctrl.bits.im_lms2ipt_en = enable;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_lms2ipt_ctrl.u32))),
                       gp0_db_imap_lms2ipt_ctrl.u32);
    return;
}

static hi_void hifb_gp_v2_setimtmv1en(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_db_imap_tmap_v1_ctrl gp0_db_imap_tmap_v1_ctrl;

    gp0_db_imap_tmap_v1_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_tmap_v1_ctrl.u32))));

    gp0_db_imap_tmap_v1_ctrl.bits.im_tm_v1_en = enable;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_tmap_v1_ctrl.u32))),
                       gp0_db_imap_tmap_v1_ctrl.u32);
    return;
}

static hi_void hifb_gp_v2_setimcmen(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_db_imap_cacm_reg0 gp0_db_imap_cacm_reg0;

    gp0_db_imap_cacm_reg0.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_cacm_reg0.u32))));

    gp0_db_imap_cacm_reg0.bits.im_cm_en = enable;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_cacm_reg0.u32))),
                       gp0_db_imap_cacm_reg0.u32);
    return;
}

static hi_void hifb_gp_v2_setimladjen(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_db_imap_ctrl gp0_db_imap_ctrl;

    gp0_db_imap_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32))));

    gp0_db_imap_ctrl.bits.im_ladj_en = enable;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32))), gp0_db_imap_ctrl.u32);
    return;
}

static hi_void hifb_gp_v2_setimgammaen(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_db_imap_ctrl gp0_db_imap_ctrl;

    gp0_db_imap_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32))));

    gp0_db_imap_ctrl.bits.im_gamma_en = enable;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32))), gp0_db_imap_ctrl.u32);
    return;
}

static hi_void hifb_gp_v2_setimdegammaen(hi_u32 data, hi_bool enable)
{
    volatile u_db_imap_ctrl gp0_db_imap_ctrl;

    gp0_db_imap_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32))));

    gp0_db_imap_ctrl.bits.im_degamma_en = enable;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32))), gp0_db_imap_ctrl.u32);
    return;
}

static hi_void hifb_gp_v2_setimnormen(hi_u32 data, hi_bool enable)
{
    volatile u_db_imap_ctrl gp0_db_imap_ctrl;

    gp0_db_imap_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32))));

    gp0_db_imap_ctrl.bits.im_norm_en = enable;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32))), gp0_db_imap_ctrl.u32);
    return;
}

static hi_void hifb_gp_v2_setimdemolumaen(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_db_imap_ladj_weight gp0_db_imap_ladj_weight;

    gp0_db_imap_ladj_weight.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ladj_weight.u32))));

    gp0_db_imap_ladj_weight.bits.im_demo_luma_en = enable;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ladj_weight.u32))),
                       gp0_db_imap_ladj_weight.u32);
    return;
}

static hi_void hifb_gp_v2_setimv1hdren(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_db_imap_yuv2rgb_ctrl gp0_db_imap_yuv2rgb_ctrl;

    gp0_db_imap_yuv2rgb_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_ctrl.u32))));

    gp0_db_imap_yuv2rgb_ctrl.bits.im_v1_hdr_en = enable;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_ctrl.u32))),
                       gp0_db_imap_yuv2rgb_ctrl.u32);
    return;
}

static hi_void hifb_gp_v2_setimv0hdren(hi_u32 data, hi_bool enable)
{
    volatile u_db_imap_yuv2rgb_ctrl gp0_db_imap_yuv2rgb_ctrl;

    gp0_db_imap_yuv2rgb_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_ctrl.u32))));

    gp0_db_imap_yuv2rgb_ctrl.bits.im_v0_hdr_en = enable;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_ctrl.u32))),
                       gp0_db_imap_yuv2rgb_ctrl.u32);
    return;
}

static hi_void hifb_gp_v2_setimrshiftrounden(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_db_imap_ctrl gp0_db_imap_ctrl;

    gp0_db_imap_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32))));

    gp0_db_imap_ctrl.bits.im_rshift_round_en = enable;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32))), gp0_db_imap_ctrl.u32);
    return;
}

static hi_void hifb_gp_v2_setimrshiften(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_db_imap_ctrl gp0_db_imap_ctrl;

    gp0_db_imap_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32))));

    gp0_db_imap_ctrl.bits.im_rshift_en = enable;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32))), gp0_db_imap_ctrl.u32);
    return;
}

static hi_void hifb_gp_v2_setimiptinsel(hi_u32 data, hi_bool enable)
{
    volatile u_db_imap_ctrl gp0_db_imap_ctrl;

    gp0_db_imap_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32))));

    gp0_db_imap_ctrl.bits.im_ipt_in_sel = enable;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32))), gp0_db_imap_ctrl.u32);
    return;
}

hi_void hifb_hal_gp_hdr_set(hi_u32 data, hi_bool enable)
{
    hifb_gp_v2_setimrgb2lmsen(data, enable);
    hifb_gp_v2_setimlms2ipten(data, enable);
    hifb_gp_v2_setimtmv1en(data, enable);
    hifb_gp_v2_setimcmen(data, enable);
    hifb_gp_v2_setimladjen(data, enable);
    hifb_gp_v2_setimgammaen(data, enable);
    hifb_gp_v2_setimdegammaen(data, enable);
    hifb_gp_v2_setimnormen(data, enable);
    hifb_gp_v2_setimdemolumaen(data, enable);
    hifb_gp_v2_setimv1hdren(data, enable);
    hifb_gp_v2_setimv0hdren(data, enable);
    hifb_gp_v2_setimrshiftrounden(data, enable);
    hifb_gp_v2_setimrshiften(data, enable);
    hifb_gp_v2_setimiptinsel(data, enable);
} /* end set hdr */

hi_void hifb_hal_set_gp_csc_coef00(hi_u32 data, hi_s32 csc_coef00)
{
    volatile u_gp0_db_imap_yuv2rgb_coef00 gp0_db_imap_yuv2rgb_coef00;

    gp0_db_imap_yuv2rgb_coef00.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef00.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_coef00.bits.imap_m33yuv2rgb_00 = csc_coef00;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef00.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_coef00.u32);
    return;
}

hi_void hifb_hal_set_gp_csc_coef01(hi_u32 data, hi_s32 csc_coef01)
{
    volatile u_gp0_db_imap_yuv2rgb_coef01 gp0_db_imap_yuv2rgb_coef01;
    gp0_db_imap_yuv2rgb_coef01.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef01.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_coef01.bits.imap_m33yuv2rgb_01 = csc_coef01;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef01.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_coef01.u32);
}

hi_void hifb_hal_set_gp_csc_coef02(hi_u32 data, hi_s32 csc_coef02)
{
    volatile u_gp0_db_imap_yuv2rgb_coef02 gp0_db_imap_yuv2rgb_coef02;
    gp0_db_imap_yuv2rgb_coef02.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef02.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_coef02.bits.imap_m33yuv2rgb_02 = csc_coef02;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef02.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_coef02.u32);
}

hi_void hifb_hal_set_gp_csc_coef10(hi_u32 data, hi_s32 csc_coef10)
{
    volatile u_gp0_db_imap_yuv2rgb_coef03 gp0_db_imap_yuv2rgb_coef10;
    gp0_db_imap_yuv2rgb_coef10.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef03.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_coef10.bits.imap_m33yuv2rgb_10 = csc_coef10;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef03.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_coef10.u32);
}

hi_void hifb_hal_set_gp_csc_coef11(hi_u32 data, hi_s32 csc_coef11)
{
    volatile u_gp0_db_imap_yuv2rgb_coef04 gp0_db_imap_yuv2rgb_coef11;
    gp0_db_imap_yuv2rgb_coef11.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef04.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_coef11.bits.imap_m33yuv2rgb_11 = csc_coef11;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef04.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_coef11.u32);
}

hi_void hifb_hal_set_gp_csc_coef12(hi_u32 data, hi_s32 csc_coef12)
{
    volatile u_gp0_db_imap_yuv2rgb_coef05 gp0_db_imap_yuv2rgb_coef12;
    gp0_db_imap_yuv2rgb_coef12.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef05.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_coef12.bits.imap_m33yuv2rgb_12 = csc_coef12;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef05.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_coef12.u32);
}

hi_void hifb_hal_set_gp_csc_coef20(hi_u32 data, hi_s32 csc_coef20)
{
    volatile u_gp0_db_imap_yuv2rgb_coef06 gp0_db_imap_yuv2rgb_coef20;
    gp0_db_imap_yuv2rgb_coef20.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef06.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_coef20.bits.imap_m33yuv2rgb_20 = csc_coef20;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef06.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_coef20.u32);
}

hi_void hifb_hal_set_gp_csc_coef21(hi_u32 data, hi_s32 csc_coef21)
{
    volatile u_gp0_db_imap_yuv2rgb_coef07 gp0_db_imap_yuv2rgb_coef21;
    gp0_db_imap_yuv2rgb_coef21.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef07.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_coef21.bits.imap_m33yuv2rgb_21 = csc_coef21;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef07.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_coef21.u32);
}

hi_void hifb_hal_set_gp_csc_coef22(hi_u32 data, hi_s32 csc_coef22)
{
    volatile u_gp0_db_imap_yuv2rgb_coef08 gp0_db_imap_yuv2rgb_coef22;
    gp0_db_imap_yuv2rgb_coef22.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef08.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_coef22.bits.imap_m33yuv2rgb_22 = csc_coef22;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_coef08.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_coef22.u32);
}

hi_void hifb_hal_set_gp_csc_dc_in0(hi_u32 data, hi_s32 csc_in_dc0)
{
    volatile u_gp0_db_imap_yuv2rgb_in_dc00 gp0_db_imap_yuv2rgb_in_dc0;

    gp0_db_imap_yuv2rgb_in_dc0.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_in_dc00.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_in_dc0.bits.im_v3_yuv2rgb_dc_in_0 = csc_in_dc0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_in_dc00.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_in_dc0.u32);
}

hi_void hifb_hal_set_gp_csc_dc_in1(hi_u32 data, hi_s32 csc_in_dc1)
{
    volatile u_gp0_db_imap_yuv2rgb_in_dc01 gp0_db_imap_yuv2rgb_in_dc1;

    gp0_db_imap_yuv2rgb_in_dc1.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_in_dc01.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_in_dc1.bits.im_v3_yuv2rgb_dc_in_1 = csc_in_dc1;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_in_dc01.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_in_dc1.u32);
}

hi_void hifb_hal_set_gp_csc_dc_in2(hi_u32 data, hi_s32 csc_in_dc2)
{
    volatile u_gp0_db_imap_yuv2rgb_in_dc02 gp0_db_imap_yuv2rgb_in_dc2;

    gp0_db_imap_yuv2rgb_in_dc2.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_in_dc02.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_in_dc2.bits.im_v3_yuv2rgb_dc_in_2 = csc_in_dc2;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_in_dc02.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_in_dc2.u32);
}

hi_void hifb_hal_set_gp_csc_dc_out0(hi_u32 data, hi_s32 csc_out_dc0)
{
    volatile u_gp0_db_imap_yuv2rgb_out_dc00 gp0_db_imap_yuv2rgb_out_dc0;
    gp0_db_imap_yuv2rgb_out_dc0.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_out_dc00.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_out_dc0.bits.imap_v3yuv2rgb_offinrgb_0 = csc_out_dc0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_out_dc00.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_out_dc0.u32);
}

hi_void hifb_hal_set_gp_csc_dc_out1(hi_u32 data, hi_s32 csc_out_dc1)
{
    volatile u_gp0_db_imap_yuv2rgb_out_dc01 gp0_db_imap_yuv2rgb_out_dc1;

    gp0_db_imap_yuv2rgb_out_dc1.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_out_dc01.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_out_dc1.bits.imap_v3yuv2rgb_offinrgb_1 = csc_out_dc1;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_out_dc01.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_out_dc1.u32);
}

hi_void hifb_hal_set_gp_csc_dc_out2(hi_u32 data, hi_s32 csc_out_dc2)
{
    volatile u_gp0_db_imap_yuv2rgb_out_dc02 gp0_db_imap_yuv2rgb_out_dc2;

    gp0_db_imap_yuv2rgb_out_dc2.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_out_dc02.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_out_dc2.bits.imap_v3yuv2rgb_offinrgb_2 = csc_out_dc2;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_out_dc02.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_out_dc2.u32);
}

hi_void hifb_hal_set_gp_csc_scale2p(hi_u32 data, hi_u32 scale)
{
    volatile u_gp0_db_imap_yuv2rgb_scale2p gp0_db_imap_yuv2rgb_scale2p;
    gp0_db_imap_yuv2rgb_scale2p.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_scale2p.u32)) + data * CONFIG_HIFB_G3_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_scale2p.bits.imap_m33yuv2rgb_scale2p = scale;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_scale2p.u32)) +
                                  data * CONFIG_HIFB_G3_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_scale2p.u32);
}

hi_void hifb_hal_set_gp_csc_im_en(hi_u32 data, hi_bool is_enable)
{
    volatile u_gp0_db_imap_ctrl gp0_db_imap_ctrl;

    gp0_db_imap_ctrl.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_ctrl.bits.im_en = (is_enable == HI_TRUE) ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_ctrl.u32)) - data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_ctrl.u32);
}

hi_void hifb_hal_set_gp_csc_im_yuv2rgb_mode(hi_u32 data, hi_u32 mode)
{
    volatile u_gp0_db_imap_yuv2rgb_ctrl gp0_db_imap_yuv2rgb_ctrl;
    gp0_db_imap_yuv2rgb_ctrl.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_ctrl.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_ctrl.bits.im_yuv2rgb_mode = mode;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_ctrl.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_ctrl.u32);
}

hi_void hifb_hal_set_gp_csc_im_yuv2rgb_en(hi_u32 data, hi_bool is_enable)
{
    volatile u_gp0_db_imap_yuv2rgb_ctrl gp0_db_imap_yuv2rgb_ctrl;

    gp0_db_imap_yuv2rgb_ctrl.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_ctrl.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_ctrl.bits.im_yuv2rgb_en = (is_enable == HI_TRUE) ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_ctrl.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_ctrl.u32);
}

hi_void hifb_hal_set_gp_csc_clip_min(hi_u32 data, hi_u32 clip_min)
{
    volatile u_gp0_db_imap_yuv2rgb_min gp0_db_imap_yuv2rgb_min;

    gp0_db_imap_yuv2rgb_min.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_min.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_min.bits.im_yuv2rgb_clip_min = clip_min;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_min.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_min.u32);
}

hi_void hifb_hal_set_gp_csc_clip_max(hi_u32 data, hi_u32 clip_max)
{
    volatile u_db_imap_yuv2rgb_max gp0_db_imap_yuv2rgb_max;

    gp0_db_imap_yuv2rgb_max.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_max.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_max.bits.im_yuv2rgb_clip_max = clip_max;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_max.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_max.u32);
}

hi_void hifb_hal_set_gp1_csc_scale2p(hi_u32 data, hi_u32 scale)
{
    volatile u_vdp_hipp_csc_scale vdp_hipp_csc_scale;
    vdp_hipp_csc_scale.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->vdp_hipp_csc_scale.u32)) + data * CONFIG_HIFB_G3_CSC_OFFSET));
    vdp_hipp_csc_scale.bits.hipp_csc_scale = scale;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->vdp_hipp_csc_scale.u32)) +
                                  data * CONFIG_HIFB_G3_CSC_OFFSET),
                       vdp_hipp_csc_scale.u32);
}

hi_void hifb_hal_set_gp1_csc_clip_max_c(hi_u32 data, hi_u32 clip_max)
{
    volatile u_vdp_hipp_csc_max_c vdp_hipp_csc_max_c;

    vdp_hipp_csc_max_c.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->vdp_hipp_csc_max_c.u32)) + data * CONFIG_HIFB_G3_CSC_OFFSET));
    vdp_hipp_csc_max_c.bits.hipp_csc_max_c = clip_max;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->vdp_hipp_csc_max_c.u32)) +
                                  data * CONFIG_HIFB_G3_CSC_OFFSET),
                       vdp_hipp_csc_max_c.u32);
}

hi_void hifb_hal_set_gp1_csc_clip_max_y(hi_u32 data, hi_u32 clip_max)
{
    volatile u_vdp_hipp_csc_max_y vdp_hipp_csc_max_y;

    vdp_hipp_csc_max_y.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->vdp_hipp_csc_max_y.u32)) + data * CONFIG_HIFB_G3_CSC_OFFSET));
    vdp_hipp_csc_max_y.bits.hipp_csc_max_y = clip_max;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->vdp_hipp_csc_max_y.u32)) +
                                  data * CONFIG_HIFB_G3_CSC_OFFSET),
                       vdp_hipp_csc_max_y.u32);
}

hi_void hifb_hal_set_gp_csc_thr_r(hi_u32 data, hi_u32 thr_r)
{
    volatile u_gp0_db_imap_yuv2rgb_thr gp0_db_imap_yuv2rgb_thr;

    gp0_db_imap_yuv2rgb_thr.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_thr.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_thr.bits.im_yuv2rgb_thr_r = thr_r;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_thr.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_thr.u32);
}

hi_void hifb_hal_set_gp_csc_thr_b(hi_u32 data, hi_u32 thr_b)
{
    volatile u_gp0_db_imap_yuv2rgb_thr gp0_db_imap_yuv2rgb_thr;

    gp0_db_imap_yuv2rgb_thr.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_thr.u32)) - data * CONFIG_HIFB_CSC_OFFSET));
    gp0_db_imap_yuv2rgb_thr.bits.im_yuv2rgb_thr_b = thr_b;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_db_imap_yuv2rgb_thr.u32)) -
                                  data * CONFIG_HIFB_CSC_OFFSET),
                       gp0_db_imap_yuv2rgb_thr.u32);
}

hi_void hifb_hal_set_gp1_csc_dc_out0(hi_u32 data, hi_s32 csc_out_dc0)
{
    volatile u_vdp_hipp_csc_odc0 vdp_hipp_csc_odc0;
    vdp_hipp_csc_odc0.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->vdp_hipp_csc_odc0.u32)) + data * CONFIG_HIFB_G3_CSC_OFFSET));
    vdp_hipp_csc_odc0.bits.hipp_csc_odc0 = csc_out_dc0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->vdp_hipp_csc_odc0.u32)) +
                                  data * CONFIG_HIFB_G3_CSC_OFFSET),
                       vdp_hipp_csc_odc0.u32);
}

hi_void hifb_hal_set_gp1_csc_dc_out1(hi_u32 data, hi_s32 csc_out_dc1)
{
    volatile u_vdp_hipp_csc_odc1 vdp_hipp_csc_odc1;

    vdp_hipp_csc_odc1.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->vdp_hipp_csc_odc1.u32)) + data * CONFIG_HIFB_G3_CSC_OFFSET));
    vdp_hipp_csc_odc1.bits.hipp_csc_odc1 = csc_out_dc1;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->vdp_hipp_csc_odc1.u32)) +
                                  data * CONFIG_HIFB_G3_CSC_OFFSET),
                       vdp_hipp_csc_odc1.u32);
}

hi_void hifb_hal_set_gp1_csc_dc_out2(hi_u32 data, hi_s32 csc_out_dc2)
{
    volatile u_vdp_hipp_csc_odc2 vdp_hipp_csc_odc2;

    vdp_hipp_csc_odc2.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->vdp_hipp_csc_odc2.u32)) + data * CONFIG_HIFB_G3_CSC_OFFSET));
    vdp_hipp_csc_odc2.bits.hipp_csc_odc2 = csc_out_dc2;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->vdp_hipp_csc_odc2.u32)) +
                                  data * CONFIG_HIFB_G3_CSC_OFFSET),
                       vdp_hipp_csc_odc2.u32);
}

/* zme begin */
hi_void hifb_hal_gp_set_zme_ckgt_en(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_zme_hinfo gp0_zme_hinfo;

    gp0_zme_hinfo.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hinfo.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_hinfo.bits.ck_gt_en = enable ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hinfo.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_hinfo.u32);
}

hi_void hifb_hal_gp_set_zme_hsc_en(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_zme_hsp gp0_zme_hsp;

    gp0_zme_hsp.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_hsp.bits.hsc_en = enable ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_hsp.u32);
}

hi_void hifb_hal_gp_set_zme_hamid_en(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_zme_hsp gp0_zme_hsp;

    gp0_zme_hsp.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_hsp.bits.hamid_en = enable ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_hsp.u32);
}

hi_void hifb_hal_gp_set_zme_hlmid_en(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_zme_hsp gp0_zme_hsp;

    gp0_zme_hsp.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_hsp.bits.hlmid_en = enable ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_hsp.u32);
}

hi_void hifb_hal_gp_set_zme_hchmid_en(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_zme_hsp gp0_zme_hsp;

    gp0_zme_hsp.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_hsp.bits.hchmid_en = enable ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_hsp.u32);
}

hi_void hifb_hal_gp_set_zme_hfir_en(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_zme_hsp gp0_zme_hsp;

    gp0_zme_hsp.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_hsp.bits.hfir_en = enable ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_hsp.u32);
}

hi_void hifb_hal_gp_set_zme_hafir_en(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_zme_hsp gp0_zme_hsp;

    gp0_zme_hsp.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_hsp.bits.hafir_en = enable ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_hsp.u32);
}

hi_void hifb_hal_gp_set_zme_hfir_order(hi_u32 data, hi_u32 hfir_order)
{
    volatile u_gp0_zme_hsp gp0_zme_hsp;

    gp0_zme_hsp.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_hsp.bits.hfir_order = hfir_order;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_hsp.u32);
}

hi_void hifb_hal_gp_set_zme_hratio(hi_u32 data, hi_u32 hratio)
{
    volatile u_gp0_zme_hsp gp0_zme_hsp;

    gp0_zme_hsp.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_hsp.bits.hratio = hratio;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_hsp.u32);
}

hi_void hifb_hal_gp_set_zme_out_height(hi_u32 data, hi_u32 out_height)
{
    volatile u_gp0_zme_vinfo gp0_zme_vinfo;

    gp0_zme_vinfo.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vinfo.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_vinfo.bits.out_height = out_height;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vinfo.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_vinfo.u32);
}

hi_void hifb_hal_gp_set_zme_vsc_en(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_zme_vsp gp0_zme_vsp;

    gp0_zme_vsp.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_vsp.bits.vsc_en = enable ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_vsp.u32);
}

hi_void hifb_hal_gp_set_zme_vamid_en(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_zme_vsp gp0_zme_vsp;

    gp0_zme_vsp.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_vsp.bits.vamid_en = enable ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_vsp.u32);
}

hi_void hifb_hal_gp_set_zme_vlmid_en(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_zme_vsp gp0_zme_vsp;

    gp0_zme_vsp.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_vsp.bits.vlmid_en = enable ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_vsp.u32);
}

hi_void hifb_hal_gp_set_zme_vchmid_en(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_zme_vsp gp0_zme_vsp;

    gp0_zme_vsp.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_vsp.bits.vchmid_en = enable ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_vsp.u32);
}

hi_void hifb_hal_gp_set_zme_vfir_en(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_zme_vsp gp0_zme_vsp;

    gp0_zme_vsp.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_vsp.bits.vfir_en = enable ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_vsp.u32);
}

hi_void hifb_hal_gp_set_zme_vafir_en(hi_u32 data, hi_bool enable)
{
    volatile u_gp0_zme_vsp gp0_zme_vsp;

    gp0_zme_vsp.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_vsp.bits.vafir_en = enable ? 1 : 0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vsp.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_vsp.u32);
}

hi_void hifb_hal_gp_set_zme_vratio(hi_u32 data, hi_u32 vratio)
{
    volatile u_gp0_zme_vsr gp0_zme_vsr;

    gp0_zme_vsr.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vsr.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_vsr.bits.vratio = vratio;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vsr.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_vsr.u32);
}

hi_void hifb_hal_gp_set_zme_vtp_offset(hi_u32 data, hi_u32 offset)
{
    volatile u_gp0_zme_voffset gp0_zme_voffset;

    gp0_zme_voffset.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_voffset.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_voffset.bits.vtp_offset = offset;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_voffset.u32)) -
                                  data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_voffset.u32);
}

hi_void hifb_hal_gp_set_zme_vbtm_offset(hi_u32 data, hi_u32 offset)
{
    volatile u_gp0_zme_voffset gp0_zme_voffset;

    gp0_zme_voffset.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_voffset.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET));
    gp0_zme_voffset.bits.vbtm_offset = offset;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_voffset.u32)) -
                                  data * CONFIG_HIFB_GP_ZME_OFFSET),
                       gp0_zme_voffset.u32);
}

hi_void hifb_hal_gp_set_zme_in_width(hi_u32 data, hi_u32 zme_iw, hi_u32 split)
{
    volatile u_gp0_zme_ireso gp0_zme_ireso;

    gp0_zme_ireso.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_ireso.u32)) -
                                     data * CONFIG_HIFB_GP_ZME_OFFSET + split * CONFIG_HIFB_GP0_ZME_SPLIT_OFFSET));

    gp0_zme_ireso.bits.in_width = zme_iw;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_ireso.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET +
                                  split * CONFIG_HIFB_GP0_ZME_SPLIT_OFFSET),
                       gp0_zme_ireso.u32);
}

hi_void hifb_hal_gp_set_zme_out_width(hi_u32 data, hi_u32 zme_ow, hi_u32 split)
{
    volatile u_gp0_zme_hinfo gp0_zme_hinfo;

    gp0_zme_hinfo.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hinfo.u32)) -
                                     data * CONFIG_HIFB_GP_ZME_OFFSET + split * CONFIG_HIFB_GP0_ZME_SPLIT_OFFSET));

    gp0_zme_hinfo.bits.out_width = zme_ow;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hinfo.u32)) - data * CONFIG_HIFB_GP_ZME_OFFSET +
                                  split * CONFIG_HIFB_GP0_ZME_SPLIT_OFFSET),
                       gp0_zme_hinfo.u32);
}

hi_void hifb_hal_gp_set_zme_hor_loffset(hi_u32 data, hi_u32 hor_loffset, hi_u32 split)
{
    volatile u_gp0_zme_hloffset gp0_zme_hloffset;

    gp0_zme_hloffset.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hloffset.u32)) -
                                     data * CONFIG_HIFB_GP_ZME_OFFSET + split * CONFIG_HIFB_GP0_ZME_SPLIT_OFFSET));

    gp0_zme_hloffset.bits.hor_loffset = hor_loffset;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hloffset.u32)) -
                                  data * CONFIG_HIFB_GP_ZME_OFFSET + split * CONFIG_HIFB_GP0_ZME_SPLIT_OFFSET),
                       gp0_zme_hloffset.u32);
}

hi_void hifb_hal_gp_set_zme_hor_coffset(hi_u32 data, hi_u32 hor_coffset, hi_u32 split)
{
    volatile u_gp0_zme_hcoffset gp0_zme_hcoffset;

    gp0_zme_hcoffset.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hcoffset.u32)) -
                                     data * CONFIG_HIFB_GP_ZME_OFFSET + split * CONFIG_HIFB_GP0_ZME_SPLIT_OFFSET));

    gp0_zme_hcoffset.bits.hor_coffset = hor_coffset;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hcoffset.u32)) -
                                  data * CONFIG_HIFB_GP_ZME_OFFSET + split * CONFIG_HIFB_GP0_ZME_SPLIT_OFFSET),
                       gp0_zme_hcoffset.u32);
}

hi_void hifb_hal_gp_set_zme_coef_zme(hi_u32 data, hi_u64 addr)
{
    if (data == 0) {
        volatile u_para_haddr_disp_chn08 para_haddr_disp_chn08;
        volatile u_para_addr_disp_chn08 para_addr_disp_chn08;

        para_haddr_disp_chn08.bits.para_haddr_disp_chn08 = (hi_u32)((addr >> 32) & 0xFFFFFFFF); /* high addr 32 bits */
        para_addr_disp_chn08.bits.para_addr_disp_chn08 = (hi_u32)(addr & 0xFFFFFFFF);
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_haddr_disp_chn08.u32))),
                           para_haddr_disp_chn08.u32);
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_addr_disp_chn08.u32))),
                           para_addr_disp_chn08.u32);
    } else {
        volatile u_para_haddr_vsd_chn00 para_haddr_vsd_chn00;
        volatile u_para_addr_vsd_chn00 para_addr_vsd_chn00;

        para_haddr_vsd_chn00.bits.para_haddr_vsd_chn00 = (hi_u32)((addr >> 32) & 0xFFFFFFFF); /* high addr 32 bits */
        para_addr_vsd_chn00.bits.para_addr_vsd_chn00 = (hi_u32)(addr & 0xFFFFFFFF);
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_haddr_vsd_chn00.u32))),
                           para_haddr_vsd_chn00.u32);
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_addr_vsd_chn00.u32))),
                           para_addr_vsd_chn00.u32);
    }
}

hi_void hifb_hal_gp_set_para_up_zme(hi_u32 data)
{
    if (data == 0) {
        volatile u_para_up_disp para_up_disp;

        para_up_disp.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_disp.u32))));
        para_up_disp.bits.para_up_disp_chn08 = 1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_disp.u32))), para_up_disp.u32);
    } else {
        volatile u_para_up_vsd para_up_vsd;

        para_up_vsd.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vsd.u32))));
        para_up_vsd.bits.para_up_vsd_chn00 = 1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vsd.u32))), para_up_vsd.u32);
    }
}

/***************************************************************************************
 * func        : drv_hifb_hal_cbm_open_mute_bk
 * description : CNcomment: 打开patten调试 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_cbm_open_mute_bk(hi_u32 data)
{
    volatile u_gp0_upd gp0_upd;
    volatile u_gp0_mute_bk gp0_mute_bk;
    hi_u32 gp_num = hifb_hal_get_gp_id(data);
#ifdef CONFIG_HIFB_VERSION_4_0
    volatile u_gp0_f_upd gp0_f_upd;
#endif
    CONFIG_HIFB_CHECK_GP_SUPPORT(gp_num);

    gp0_mute_bk.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_mute_bk.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
    gp0_mute_bk.u32 = 0x800003ff;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_mute_bk.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       gp0_mute_bk.u32);

    gp0_upd.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_upd.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
    gp0_upd.bits.regup = 0x1;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_upd.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       gp0_upd.u32);

#ifdef CONFIG_HIFB_VERSION_4_0
    gp0_f_upd.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_f_upd.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
    gp0_f_upd.bits.regup = 0x1;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_f_upd.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       gp0_f_upd.u32);
#endif
    return;
}

/***************************************************************************************
 * func        : drv_hifb_hal_cbm_close_mute_bk
 * description : CNcomment: 打开patten调试 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_cbm_close_mute_bk(hi_u32 data)
{
    volatile u_gp0_upd gp0_upd;
    volatile u_gp0_mute_bk gp0_mute_bk;
    hi_u32 gp_num = hifb_hal_get_gp_id(data);
#ifdef CONFIG_HIFB_VERSION_4_0
    volatile u_gp0_f_upd gp0_f_upd;
#endif
    CONFIG_HIFB_CHECK_GP_SUPPORT(gp_num);

    gp0_mute_bk.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_mute_bk.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
    gp0_mute_bk.bits.mute_en = 0x0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_mute_bk.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       gp0_mute_bk.u32);

    gp0_upd.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_upd.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
    gp0_upd.bits.regup = 0x1;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_upd.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       gp0_upd.u32);

#ifdef CONFIG_HIFB_VERSION_4_0
    gp0_f_upd.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_f_upd.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
    gp0_f_upd.bits.regup = 0x1;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_f_upd.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       gp0_f_upd.u32);
#endif
    return;
}

/***************************************************************************************
 * func        : drv_hifb_hal_cbm_set_mixer_bkg
 * description : CNcomment: 设置MIXG0叠加背景色寄存器 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_cbm_set_mixer_bkg(OPTM_VDP_CBM_MIX_E u32mixer_id, OPTM_VDP_BKG_S *bkg)
{
    volatile u_mixg0_bkg mixg0_bkg;
    volatile u_mixg0_bkalpha mixg0_bkalpha;

    if ((VDP_CBM_MIXG0 == u32mixer_id) && (NULL != bkg)) {
        /* G0 mixer link */
        mixg0_bkg.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->mixg0_bkg.u32)));
        mixg0_bkg.bits.mixer_bkgy = bkg->u32BkgY;
        mixg0_bkg.bits.mixer_bkgcb = bkg->u32BkgU;
        mixg0_bkg.bits.mixer_bkgcr = bkg->u32BkgV;
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->mixg0_bkg.u32)), mixg0_bkg.u32);

        /* * MIXG0 use SRC OVER alpha * */
        mixg0_bkalpha.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->mixg0_bkalpha.u32)));
        mixg0_bkalpha.bits.mixer_alpha = bkg->u32BkgA;
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->mixg0_bkalpha.u32)), mixg0_bkalpha.u32);
    }

    return;
}

hi_void drv_hifb_hal_cbm_get_mixer_prio(OPTM_VDP_CBM_MIX_E u32mixer_id, hi_u32 *pu32prio)
{
    if ((VDP_CBM_MIXG0 == u32mixer_id) && (NULL != pu32prio)) {
        *pu32prio = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->mixg0_mix.u32)));
    }

    return;
}

/***************************************************************************************
* func         : drv_hifb_hal_cbm_set_mixg_prio
* description  : CNcomment: MIXG0优先级配置寄存器。在vsync处更新有效。
                            mixer_prio_x表示第x个优先级所配置的层。
                            该寄存器为即时寄存器。CNend\n
* param[in]    : hi_void
* retval       : NA
* others:      : NA
***************************************************************************************/
hi_void drv_hifb_hal_cbm_set_mixg_prio(OPTM_VDP_CBM_MIX_E u32mixer_id, hi_u32 u32prio)
{
    volatile u_mixg0_mix mixg0_mix;

    if (VDP_CBM_MIXG0 == u32mixer_id) {
        mixg0_mix.u32 = u32prio;
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->mixg0_mix.u32)), mixg0_mix.u32);
    }

    return;
}

/***************************************************************************************
 * func         : drv_hifb_hal_cbm_set_mixer_prio
 * description  : CNcomment: 设置图形层叠加优先级，Z序 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_cbm_set_mixer_prio(OPTM_VDP_CBM_MIX_E u32mixer_id, hi_u32 u32layer_id, hi_u32 u32prio)
{
    volatile u_mixg0_mix mixg0_mix;

    if (VDP_CBM_MIXG0 != u32mixer_id) {
        return;
    }

    switch (u32prio) {
        case 0: {
            mixg0_mix.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->mixg0_mix.u32)));
            mixg0_mix.bits.mixer_prio0 = u32layer_id + 1;
            optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->mixg0_mix.u32)), mixg0_mix.u32);
            break;
        }
        case 1: {
            mixg0_mix.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->mixg0_mix.u32)));
            mixg0_mix.bits.mixer_prio1 = u32layer_id + 1;
            optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->mixg0_mix.u32)), mixg0_mix.u32);
            break;
        }
        case 2: {
            mixg0_mix.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->mixg0_mix.u32)));
            mixg0_mix.bits.mixer_prio2 = u32layer_id + 1;
            optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->mixg0_mix.u32)), mixg0_mix.u32);
            break;
        }
        case 3: {
            mixg0_mix.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->mixg0_mix.u32)));
            mixg0_mix.bits.mixer_prio3 = u32layer_id + 1;
            optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->mixg0_mix.u32)), mixg0_mix.u32);
            break;
        }
        default: {
            GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32prio);
            return;
        }
    }

    return;
}

// -------------------------------------------------------------------
// GFX_BEGIN
// -------------------------------------------------------------------
/***************************************************************************************
 * func         : drv_hifb_hal_gfx_get_layer_enable
 * description  : CNcomment: 从寄存器中获取图层是否使能数据 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_gfx_get_layer_enable(hi_u32 data, hi_u32 *enable)
{
    volatile u_g0_ctrl g0_ctrl;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    g0_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_ctrl.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
    if (NULL != enable) {
        *enable = g0_ctrl.bits.surface_en;
    }

    return;
}
#endif

/***************************************************************************************
 * func         : drv_hifb_hal_gfx_set_layer_enable
 * description  : CNcomment: 设置图层使能 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_layer_enable(hi_u32 data, hi_u32 enable)
{
    volatile u_gfx_addr_l gfx_addr_l;
    volatile u_g0_ctrl g0_ctrl;
    volatile u_gp0_ctrl gp0_ctrl;
#ifdef CONFIG_HIFB_VERSION_4_0
    volatile u_gp0_f_ctrl gp0_f_ctrl;
#endif

    hi_u32 gp_num = hifb_hal_get_gp_id_from_layer_id(data);
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, data);
    GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, enable);

    gfx_addr_l.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_addr_l.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    if (0 == gfx_addr_l.u32) {
        GRAPHIC_DFX_DEBUG_VALUE(GRAPHIC_DFX_DEBUG_INT, gfx_addr_l.u32);
        return;
    }

    g0_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_ctrl.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
    g0_ctrl.bits.surface_en = enable;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_ctrl.u32)) + data * CONFIG_HIFB_CTL_OFFSET),
                       g0_ctrl.u32);

    CONFIG_HIFB_CHECK_GP_SUPPORT(gp_num);
    gp0_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_ctrl.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
    gp0_ctrl.bits.surface_en = enable;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_ctrl.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       gp0_ctrl.u32);

#ifdef CONFIG_HIFB_VERSION_4_0
    gp0_f_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_f_ctrl.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
    gp0_f_ctrl.bits.surface_en = enable;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_f_ctrl.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       gp0_f_ctrl.u32);
#endif
    return;
}

hi_void hifb_hal_set_video_enable(hi_u32 data, hi_u32 enable)
{
    volatile u_v1_ctrl v1_ctrl;
    volatile u_v1_upd v1_upd;

    if (data >= VDP_LAYER_MAX) {
        return;
    }

    v1_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->v1_ctrl.u32)) + g_vdp_layer_reg_offset[data]));

    v1_ctrl.bits.surface_en = enable;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->v1_ctrl.u32)) + g_vdp_layer_reg_offset[data]),
                       v1_ctrl.u32);

    v1_upd.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->v1_upd.u32)) + g_vdp_layer_reg_offset[data]));

    v1_upd.bits.regup = 1;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->v1_upd.u32)) + g_vdp_layer_reg_offset[data]),
                       v1_upd.u32);

    return;
}

/***************************************************************************************
 * func         : drv_hifb_hal_gfx_set_no_sec_flag
 * description  : CNcomment: 设置DDR模式 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_no_sec_flag(hi_u32 data, hi_u32 enable)
{
    volatile u_g0_ctrl g0_ctrl;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    g0_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_ctrl.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
    g0_ctrl.bits.nosec_flag = enable;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_ctrl.u32)) + data * CONFIG_HIFB_CTL_OFFSET),
                       g0_ctrl.u32);

    return;
}

/***************************************************************************************
 * func         : drv_hifb_hal_gfx_set_dcmp_enable
 * description  : CNcomment: 配置解压寄存器，和压缩有关, 现在是无损压缩 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others       : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_dcmp_enable(hi_u32 data, hi_u32 u32b_enable)
{
#ifdef CFG_HI_FB_DECOMPRESS_SUPPORT
    volatile u_gfx_src_info gfx_src_info;
    volatile u_gfx_read_ctrl gfx_read_ctrl;

#ifndef CONFIG_HIFB_VERSION_4_0
    if (data == 0)
#endif
    {
        gfx_src_info.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_info.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        gfx_read_ctrl.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_read_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET));

        gfx_src_info.bits.dcmp_type = (0 == u32b_enable) ? (0) : (1);
        gfx_read_ctrl.bits.addr_map_en = 1;

        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_info.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_src_info.u32);
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_read_ctrl.u32)) +
                                      data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_read_ctrl.u32);
    }
#else
#endif
    return;
}

hi_void drv_hifb_hal_gfx_get_dcmp_enable(hi_u32 data, hi_u32 *enable)
{
#ifdef CFG_HI_FB_DECOMPRESS_SUPPORT
    volatile u_gfx_src_info gfx_src_info;

#ifndef CONFIG_HIFB_VERSION_4_0
    if (data == 0)
#endif
    {
        if (enable != HI_NULL) {
            gfx_src_info.u32 = optm_vdp_reg_read(
                (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_info.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
            *enable = gfx_src_info.bits.dcmp_type;
        }
    }

#else
#endif
    return;
}

#ifndef HI_BUILD_IN_BOOT
/***************************************************************************************
 * func          : OPTM_VDP_GFX_SetDcmpARHeadAddr
 * description   : CNcomment: 2D或3D左眼AR压缩头地址CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_void optm_vdp_gfx_set_dcmp_arhead_addr(hi_u32 data, hi_u32 ar_head_ddr)
{
    volatile u_gfx_head_addr_l gfx_head_addr_l;
#ifdef CONFIG_HIFB_MMU_SUPPORT
    volatile u_gfx_smmu_bypass gfx_smmu_bypass;
#endif
    if (ar_head_ddr != 0) {
        gfx_head_addr_l.u32 = ar_head_ddr;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_head_addr_l.u32)) +
                                      data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_head_addr_l.u32);

#ifdef CONFIG_HIFB_MMU_SUPPORT
        gfx_smmu_bypass.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_smmu_bypass.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
#ifdef CONFIG_GFX_MMU_SUPPORT
        gfx_smmu_bypass.bits.smmu_bypass_h2d = 0;
#else
        gfx_smmu_bypass.bits.smmu_bypass_h2d = 1;
#endif
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_smmu_bypass.u32)) +
                                      data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_smmu_bypass.u32);
#endif
    }

    return;
}

/***************************************************************************************
 * func          : OPTM_VDP_GFX_SetDcmpGBHeadAddr
 * description   : CNcomment: 2D或3D左眼GB压缩头地址CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_void optm_vdp_gfx_set_dcmp_gbhead_addr(hi_u32 data, hi_u32 gb_head_ddr)
{
    volatile u_gfx_head2_addr_l gfx_head2_addr_l;
#ifdef CONFIG_HIFB_MMU_SUPPORT
    volatile u_gfx_smmu_bypass gfx_smmu_bypass;
#endif
    if ((0 == data) && (0 != gb_head_ddr)) {
        gfx_head2_addr_l.u32 = gb_head_ddr;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_head2_addr_l.u32)) +
                                      data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_head2_addr_l.u32);

#ifdef CONFIG_HIFB_MMU_SUPPORT
        gfx_smmu_bypass.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_smmu_bypass.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
#ifdef CONFIG_GFX_MMU_SUPPORT
        gfx_smmu_bypass.bits.smmu_bypass_h3d = 0;
#else
        gfx_smmu_bypass.bits.smmu_bypass_h3d = 1;
#endif
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_smmu_bypass.u32)) +
                                      data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_smmu_bypass.u32);
#endif
    }

    return;
}

/***************************************************************************************
 * func          : OPTM_VDP_GFX_SetDcmpGBDataAddr
 * description   : CNcomment: GB压缩数据地址 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_void optm_vdp_gfx_set_dcmp_gbdata_addr(hi_u32 data, hi_u32 gb_data_ddr)
{
    volatile u_gfx_dcmp_addr_l gfx_dcmp_addr_l;
#ifdef CONFIG_HIFB_MMU_SUPPORT
    volatile u_gfx_smmu_bypass gfx_smmu_bypass;
#endif
    if ((0 == data) && (0 != gb_data_ddr)) {
        gfx_dcmp_addr_l.u32 = gb_data_ddr;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_dcmp_addr_l.u32)) +
                                      data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_dcmp_addr_l.u32);

#ifdef CONFIG_HIFB_MMU_SUPPORT
        gfx_smmu_bypass.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_smmu_bypass.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
#ifdef CONFIG_GFX_MMU_SUPPORT
        gfx_smmu_bypass.bits.smmu_bypass_3d = 0;
#else
        gfx_smmu_bypass.bits.smmu_bypass_3d = 1;
#endif
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_smmu_bypass.u32)) +
                                      data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_smmu_bypass.u32);
#endif
    }

    return;
}
#endif

/***************************************************************************************
 * func         : drv_hifb_hal_gfx_get_layer_addr
 * description  : CNcomment: 获取surface 帧buffer地址 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_gfx_get_layer_addr(hi_u32 data, hi_u32 *addr)
{
    volatile u_gfx_addr_l gfx_addr_l;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gfx_addr_l.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_addr_l.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    if (NULL != addr) {
        *addr = gfx_addr_l.u32;
    }

    return;
}
#endif

/***************************************************************************************
 * func          : drv_hifb_hal_gfx_set_layer_addr_ex
 * description   : CNcomment: 2D或3D左眼显示地址 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_layer_addr_ex(hi_u32 data, hi_u32 l_addr)
{
    volatile u_g0_ctrl g0_ctrl;
    volatile u_gfx_addr_l gfx_addr_l;
#ifdef CONFIG_HIFB_MMU_SUPPORT
    volatile u_gfx_smmu_bypass gfx_smmu_bypass;
#endif
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    g0_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_ctrl.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
    if ((0 == l_addr) && (1 == g0_ctrl.bits.surface_en)) {
        // GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, u32LAddr);
        // GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, G0_CTRL.u32);
        return;
    }

    gfx_addr_l.u32 = l_addr;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_addr_l.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                       gfx_addr_l.u32);

#ifdef CONFIG_HIFB_MMU_SUPPORT
    gfx_smmu_bypass.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_smmu_bypass.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
#ifdef CONFIG_GFX_MMU_SUPPORT
    gfx_smmu_bypass.bits.smmu_bypass_2d = 0;
#else
    gfx_smmu_bypass.bits.smmu_bypass_2d = 1;
#endif
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_smmu_bypass.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                       gfx_smmu_bypass.u32);
#endif

    return;
}

/***************************************************************************************
 * func          : drv_hifb_hal_gfx_get_layer_stride
 * description   : CNcomment: 获取stride CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_gfx_get_layer_stride(hi_u32 data, hi_u32 *stride)
{
    volatile u_gfx_stride gfx_stride;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gfx_stride.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_stride.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    if (NULL != stride) {
        *stride = gfx_stride.bits.surface_stride * STRIDE_ALIGN;
    }

    return;
}
#endif

/***************************************************************************************
 * func        : drv_hifb_hal_gfx_set_layer_stride
 * description : CNcomment: 设置图层stride CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_layer_stride(hi_u32 data, hi_u32 stride)
{
    volatile u_gfx_stride gfx_stride;
    volatile u_gfx_head_stride gfx_head_stride;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    if (0 != stride) {
        gfx_stride.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_stride.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        gfx_stride.bits.surface_stride = stride / STRIDE_ALIGN;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_stride.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_stride.u32);

        gfx_head_stride.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_head_stride.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        gfx_head_stride.bits.head_stride = 1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_head_stride.u32)) +
                                      data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_head_stride.u32);
    }

    return;
}

#if !defined(HI_BUILD_IN_BOOT)
/***************************************************************************************
 * func        : OPTM_VDP_GFX_SetGBdataStride
 * description : CNcomment: 设置压缩数据GB的stride CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
static inline hi_void optm_vdp_gfx_set_gbdata_stride(hi_u32 data, hi_u32 gb_data_stride)
{
    volatile u_gfx_stride gfx_stride;
    volatile u_gfx_head_stride gfx_head_stride;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    if (0 != gb_data_stride) {
        gfx_stride.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_stride.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        gfx_stride.bits.dcmp_stride = gb_data_stride / STRIDE_ALIGN;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_stride.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_stride.u32);

        gfx_head_stride.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_head_stride.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        gfx_head_stride.bits.head2_stride = 1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_head_stride.u32)) +
                                      data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_head_stride.u32);
    }

    return;
}
#endif

hi_void drv_hifb_hal_gfx_set_de_cmp_ddr_info(hi_u32 data, hi_u32 ar_head_ddr, hi_u32 ar_data_ddr, hi_u32 gb_head_ddr,
                                             hi_u32 gb_data_ddr, hi_u32 de_cmp_stride)
{
#if !defined(HI_BUILD_IN_BOOT)
    optm_vdp_gfx_set_dcmp_arhead_addr(data, ar_head_ddr);
    drv_hifb_hal_gfx_set_layer_addr_ex(data, ar_data_ddr);
    optm_vdp_gfx_set_dcmp_gbhead_addr(data, gb_head_ddr);
    optm_vdp_gfx_set_dcmp_gbdata_addr(data, gb_data_ddr);
    drv_hifb_hal_gfx_set_layer_stride(data, de_cmp_stride);
    optm_vdp_gfx_set_gbdata_stride(data, de_cmp_stride);
#else
#endif
    return;
}

/***************************************************************************************
 * func        : drv_hifb_hal_gfx_get_de_cmp_status
 * description : CNcomment: 获取解压处理信息，调试使用 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_get_de_cmp_status(hi_u32 data, hi_bool *is_ar_error, hi_bool *is_gb_error,
                                           hi_bool close_interrupt)
{
    volatile u_gfx_od_dcmp_error_sta error_state;

    error_state.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_error_sta.u32)) + data * CONFIG_HIFB_GFX_OFFSET));

    if (error_state.bits.o_mb_qp_error || error_state.bits.o_pix_consume || error_state.bits.o_pix_forgive ||
        error_state.bits.dcmp_error) {
        if (is_ar_error != HI_NULL) {
            *is_ar_error = HI_TRUE;

            if (close_interrupt == HI_TRUE) {
                GRAPHIC_DFX_ERR_INFO(GRAPHIC_DFX_ERR_INFOMATION, "decompress err");
                drv_hifb_hal_gfx_set_interrupt(HI_FALSE);
            }
        }
        error_state.bits.o_mb_qp_error = 1;
        error_state.bits.o_pix_consume = 1;
        error_state.bits.o_pix_forgive = 1;
        error_state.bits.dcmp_error = 1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_error_sta.u32)) +
                                      data * CONFIG_HIFB_GFX_OFFSET),
                           error_state.u32);
    }

    return;
}

hi_void drv_hifb_hal_set_typ(hi_u32 layer_id, hi_u32 value)
{
    volatile u_gfx_src_info gfx_src_info;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(layer_id);

    gfx_src_info.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_info.u32)) + layer_id * CONFIG_HIFB_GFX_OFFSET));

    gfx_src_info.bits.typ = value;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_info.u32)) + layer_id * CONFIG_HIFB_GFX_OFFSET),
                       gfx_src_info.u32);

    return;
}

hi_void drv_hifb_hal_set_max_mbqp(hi_u32 layer_id, hi_u32 value)
{
    volatile u_gfx_od_dcmp_glb_info gfx_od_dcmp_glb_info;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(layer_id);

    gfx_od_dcmp_glb_info.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_glb_info.u32)) + layer_id * CONFIG_HIFB_GFX_OFFSET));

    gfx_od_dcmp_glb_info.bits.max_mb_qp = value;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_glb_info.u32)) +
                                  layer_id * CONFIG_HIFB_GFX_OFFSET),
                       gfx_od_dcmp_glb_info.u32);

    return;
}

hi_void drv_hifb_hal_set_conv_en(hi_u32 layer_id, hi_u32 value)
{
    volatile u_gfx_od_dcmp_glb_info gfx_od_dcmp_glb_info;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(layer_id);

    gfx_od_dcmp_glb_info.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_glb_info.u32)) + layer_id * CONFIG_HIFB_GFX_OFFSET));

    gfx_od_dcmp_glb_info.bits.conv_en = value;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_glb_info.u32)) +
                                  layer_id * CONFIG_HIFB_GFX_OFFSET),
                       gfx_od_dcmp_glb_info.u32);

    return;
}

hi_void drv_hifb_hal_set_pixel_fmt_od(hi_u32 layer_id, hi_u32 value)
{
    volatile u_gfx_od_dcmp_glb_info gfx_od_dcmp_glb_info;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(layer_id);

    gfx_od_dcmp_glb_info.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_glb_info.u32)) + layer_id * CONFIG_HIFB_GFX_OFFSET));

    gfx_od_dcmp_glb_info.bits.pix_format = value;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_glb_info.u32)) +
                                  layer_id * CONFIG_HIFB_GFX_OFFSET),
                       gfx_od_dcmp_glb_info.u32);

    return;
}

hi_void drv_hifb_hal_set_is_lossless(hi_u32 layer_id, hi_u32 value)
{
    volatile u_gfx_od_dcmp_glb_info gfx_od_dcmp_glb_info;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(layer_id);

    gfx_od_dcmp_glb_info.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_glb_info.u32)) + layer_id * CONFIG_HIFB_GFX_OFFSET));

    gfx_od_dcmp_glb_info.bits.is_lossless = value;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_glb_info.u32)) +
                                  layer_id * CONFIG_HIFB_GFX_OFFSET),
                       gfx_od_dcmp_glb_info.u32);

    return;
}

hi_void drv_hifb_hal_set_cmp_mode(hi_u32 layer_id, hi_u32 value)
{
    volatile u_gfx_od_dcmp_glb_info gfx_od_dcmp_glb_info;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(layer_id);

    gfx_od_dcmp_glb_info.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_glb_info.u32)) + layer_id * CONFIG_HIFB_GFX_OFFSET));

    gfx_od_dcmp_glb_info.bits.cmp_mode = value;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_glb_info.u32)) +
                                  layer_id * CONFIG_HIFB_GFX_OFFSET),
                       gfx_od_dcmp_glb_info.u32);

    return;
}

hi_void drv_hifb_hal_set_bit_depth(hi_u32 layer_id, hi_u32 value)
{
    volatile u_gfx_od_dcmp_glb_info gfx_od_dcmp_glb_info;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(layer_id);

    gfx_od_dcmp_glb_info.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_glb_info.u32)) + layer_id * CONFIG_HIFB_GFX_OFFSET));

    gfx_od_dcmp_glb_info.bits.bit_depth = value;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_glb_info.u32)) +
                                  layer_id * CONFIG_HIFB_GFX_OFFSET),
                       gfx_od_dcmp_glb_info.u32);

    return;
}

hi_void drv_hifb_hal_set_dcmp_type(hi_u32 layer_id, hi_u32 value)
{
    volatile u_gfx_src_info gfx_src_info;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(layer_id);

    gfx_src_info.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_info.u32)) + layer_id * CONFIG_HIFB_GFX_OFFSET));

    gfx_src_info.bits.dcmp_type = value;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_info.u32)) + layer_id * CONFIG_HIFB_GFX_OFFSET),
                       gfx_src_info.u32);

    return;
}

hi_void drv_hifb_hal_set_ice_en(hi_u32 layer_id, hi_u32 value)
{
    volatile u_gfx_od_dcmp_glb_info gfx_od_dcmp_glb_info;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(layer_id);

    gfx_od_dcmp_glb_info.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_glb_info.u32)) + layer_id * CONFIG_HIFB_GFX_OFFSET));

    gfx_od_dcmp_glb_info.bits.ice_en = value;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_od_dcmp_glb_info.u32)) +
                                  layer_id * CONFIG_HIFB_GFX_OFFSET),
                       gfx_od_dcmp_glb_info.u32);

    return;
}

/***************************************************************************************
 * func         : drv_hifb_hal_gfx_set_lut_addr
 * description  : CNcomment: 设置调色板地址 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_lut_addr(hi_u32 data, hi_u32 lut_addr)
{
#ifdef CONFIG_HIFB_VERSION_2_0
    volatile u_para_addr_vhd_chn17 para_addr_vhd_chn17;
    volatile u_para_addr_vhd_chn18 para_addr_vhd_chn18;

    para_addr_vhd_chn17.u32 = lut_addr;
    para_addr_vhd_chn18.u32 = lut_addr;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_addr_vhd_chn17.u32))), para_addr_vhd_chn17.u32);
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_addr_vhd_chn18.u32))), para_addr_vhd_chn18.u32);
#endif

#ifdef CONFIG_HIFB_VERSION_3_0
    volatile u_para_addr_vhd_chn11 para_addr_vhd_chn11;
    volatile u_para_addr_vhd_chn12 para_addr_vhd_chn12;
    volatile u_para_addr_vhd_chn14 para_addr_vhd_chn14;

    if (0 == data) {
        para_addr_vhd_chn11.u32 = lut_addr;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_addr_vhd_chn11.u32))),
                           para_addr_vhd_chn11.u32);
    } else if (1 == data) {
        para_addr_vhd_chn12.u32 = lut_addr;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_addr_vhd_chn12.u32))),
                           para_addr_vhd_chn12.u32);
    } else {
        para_addr_vhd_chn14.u32 = lut_addr;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_addr_vhd_chn14.u32))),
                           para_addr_vhd_chn14.u32);
    }
#endif
    return;
}

/***************************************************************************************
 * func         : drv_hifb_hal_gfx_get_in_data_fmt
 * description  : CNcomment: 获取输入的像素格式 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_gfx_get_in_data_fmt(hi_u32 data, OPTM_VDP_GFX_IFMT_E *data_fmt)
{
    volatile u_gfx_src_info gfx_src_info;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gfx_src_info.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_info.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    if (NULL != data_fmt) {
        *data_fmt = gfx_src_info.bits.ifmt;
    }

    return;
}
#endif

/***************************************************************************************
 * func          : drv_hifb_hal_gfx_set_in_data_fmt
 * description   : CNcomment: 输入数据格式 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_in_data_fmt(hi_u32 data, OPTM_VDP_GFX_IFMT_E data_fmt)
{
    volatile u_gfx_src_info gfx_src_info;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gfx_src_info.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_info.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    gfx_src_info.bits.ifmt = data_fmt;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_info.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                       gfx_src_info.u32);

    return;
}

/***************************************************************************************
* func        : drv_hifb_hal_gfx_set_bit_extend
* description : CNcomment: 设置低位扩展模式
                           所谓低位扩展就是比如ARGB1555,R少了3位，这三位如何补齐
                           是用高一位还高几位还是使用0来补齐 CNend\n
* param[in]   : hi_void
* retval      : NA
* others:     : NA
***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_bit_extend(hi_u32 data, OPTM_VDP_GFX_BITEXTEND_E u32mode)
{
    volatile u_gfx_out_ctrl gfx_out_ctrl;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gfx_out_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_out_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    gfx_out_ctrl.bits.bitext = u32mode;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_out_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                       gfx_out_ctrl.u32);

    return;
}

/***************************************************************************************
 * func         : drv_hifb_hal_gfx_set_color_key
 * description  : CNcomment:设置color key的值CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_color_key(hi_u32 data, hi_u32 bkey_en, OPTM_VDP_GFX_CKEY_S *key)
{
    volatile u_gfx_ckey_max gfx_ckey_max;
    volatile u_gfx_ckey_min gfx_ckey_min;
    volatile u_gfx_out_ctrl gfx_out_ctrl;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    if (NULL != key) {
        gfx_ckey_max.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_ckey_max.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        gfx_ckey_max.bits.key_r_max = key->u32Key_r_max;
        gfx_ckey_max.bits.key_g_max = key->u32Key_g_max;
        gfx_ckey_max.bits.key_b_max = key->u32Key_b_max;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_ckey_max.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_ckey_max.u32);

        gfx_ckey_min.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_ckey_min.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        gfx_ckey_min.bits.key_r_min = key->u32Key_r_min;
        gfx_ckey_min.bits.key_g_min = key->u32Key_g_min;
        gfx_ckey_min.bits.key_b_min = key->u32Key_b_min;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_ckey_min.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_ckey_min.u32);

        gfx_out_ctrl.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_out_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        gfx_out_ctrl.bits.key_en = bkey_en;
        gfx_out_ctrl.bits.key_mode = key->bKeyMode;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_out_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_out_ctrl.u32);
    }

    return;
}

#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_gfx_get_color_key(hi_u32 data, hi_u32 *pbkey_en, OPTM_VDP_GFX_CKEY_S *key)
{
    volatile u_gfx_ckey_max gfx_ckey_max;
    volatile u_gfx_ckey_min gfx_ckey_min;
    volatile u_gfx_out_ctrl gfx_out_ctrl;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gfx_ckey_max.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_ckey_max.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    gfx_ckey_min.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_ckey_min.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    gfx_out_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_out_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET));

    if (NULL != pbkey_en) {
        *pbkey_en = gfx_out_ctrl.bits.key_en;
    }

    if (NULL != key) {
        key->u32Key_r_max = gfx_ckey_max.bits.key_r_max;
        key->u32Key_g_max = gfx_ckey_max.bits.key_g_max;
        key->u32Key_b_max = gfx_ckey_max.bits.key_b_max;

        key->u32Key_r_min = gfx_ckey_min.bits.key_r_min;
        key->u32Key_g_min = gfx_ckey_min.bits.key_g_min;
        key->u32Key_b_min = gfx_ckey_min.bits.key_b_min;

        key->bKeyMode = gfx_out_ctrl.bits.key_mode;
    }

    return;
}
#endif

/***************************************************************************************
* func         : drv_hifb_hal_gfx_set_key_mask
* description  : CNcomment:设置掩码值，将color key的实际值与掩码值做与操作，这样就可以
                           key掉一个范围值，而不只单个值CNend\n
* param[in]    : hi_void
* retval       : NA
* others:      : NA
***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_key_mask(hi_u32 data, OPTM_VDP_GFX_MASK_S *msk)
{
    volatile u_gfx_ckey_mask gfx_ckey_mask;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    if (NULL != msk) {
        gfx_ckey_mask.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_ckey_mask.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        gfx_ckey_mask.bits.key_r_msk = msk->u32Mask_r;
        gfx_ckey_mask.bits.key_g_msk = msk->u32Mask_g;
        gfx_ckey_mask.bits.key_b_msk = msk->u32Mask_b;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_ckey_mask.u32)) +
                                      data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_ckey_mask.u32);
    }

    return;
}

#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_gfx_get_key_mask(hi_u32 data, OPTM_VDP_GFX_MASK_S *msk)
{
    volatile u_gfx_ckey_mask gfx_ckey_mask;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    if (NULL != msk) {
        gfx_ckey_mask.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_ckey_mask.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        msk->u32Mask_r = gfx_ckey_mask.bits.key_r_msk;
        msk->u32Mask_g = gfx_ckey_mask.bits.key_g_msk;
        msk->u32Mask_b = gfx_ckey_mask.bits.key_b_msk;
    }

    return;
}
#endif

/***************************************************************************************
 * func          : drv_hifb_hal_gfx_set_read_mode
 * description   : CNcomment: 设置图层读取数据模式 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_read_mode(hi_u32 data, hi_u32 mode)
{
    volatile u_gfx_read_ctrl gfx_read_ctrl;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);
    gfx_read_ctrl.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_read_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    gfx_read_ctrl.bits.read_mode = mode;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_read_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                       gfx_read_ctrl.u32);
    return;
}

hi_void drv_hifb_hal_gfx_get_read_mode(hi_u32 data, hi_u32 *mode)
{
    volatile u_gfx_read_ctrl gfx_read_ctrl;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gfx_read_ctrl.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_read_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    if (NULL != mode) {
        *mode = gfx_read_ctrl.bits.read_mode;
    }
    return;
}

/***************************************************************************************
 * func          : drv_hifb_hal_gfx_set_para_upd
 * description   : CNcomment: 设置图层更新模式 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_para_upd(hi_u32 data, OPTM_VDP_DISP_COEFMODE_E mode)
{
    volatile u_para_up_vhd para_up_vhd;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    if ((mode != VDP_DISP_COEFMODE_LUT) && (mode != VDP_DISP_COEFMODE_ALL)) {
        return;
    }

#ifdef CONFIG_HIFB_VERSION_2_0
    if (0 == data) {
        para_up_vhd.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))));
        para_up_vhd.bits.para_up_vhd_chn17 = 0x1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))), para_up_vhd.u32);
    } else {
        para_up_vhd.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))));
        para_up_vhd.bits.para_up_vhd_chn18 = 0x1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))), para_up_vhd.u32);
    }
#endif

#ifdef CONFIG_HIFB_VERSION_3_0
    if (0 == data) {
        para_up_vhd.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))));
        para_up_vhd.bits.para_up_vhd_chn11 = 0x1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))), para_up_vhd.u32);
    } else if (1 == data) {
        para_up_vhd.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))));
        para_up_vhd.bits.para_up_vhd_chn12 = 0x1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))), para_up_vhd.u32);
    } else {
        para_up_vhd.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))));
        para_up_vhd.bits.para_up_vhd_chn14 = 0x1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))), para_up_vhd.u32);
    }
#endif

#ifdef CONFIG_HIFB_VERSION_4_0
    if (0 == data) {
        para_up_vhd.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))));
        para_up_vhd.bits.para_up_vhd_chn13 = 0x1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))), para_up_vhd.u32);
    } else if (1 == data) {
        para_up_vhd.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))));
        para_up_vhd.bits.para_up_vhd_chn14 = 0x1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))), para_up_vhd.u32);
    } else {
        para_up_vhd.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))));
        para_up_vhd.bits.para_up_vhd_chn15 = 0x1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32))), para_up_vhd.u32);
    }
#endif

    return;
}

/***************************************************************************************
 * func          : drv_hifb_hal_gfx_set_three_dim_dof_enable
 * description   : CNcomment: 设置景深 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_gfx_set_three_dim_dof_enable(hi_u32 data, hi_u32 enable)
{
    volatile u_g0_dof_ctrl g0_dof_ctrl;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    g0_dof_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_dof_ctrl.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
    g0_dof_ctrl.bits.dof_en = enable;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_dof_ctrl.u32)) + data * CONFIG_HIFB_CTL_OFFSET),
                       g0_dof_ctrl.u32);

    return;
}

hi_void drv_hifb_hal_gfx_set_three_dim_dof_step(hi_u32 data, hi_s32 l_step, hi_s32 r_step)
{
    volatile u_g0_dof_step g0_dof_step;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    g0_dof_step.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_dof_step.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
    g0_dof_step.bits.right_step = r_step;
    g0_dof_step.bits.left_step = l_step;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_dof_step.u32)) + data * CONFIG_HIFB_CTL_OFFSET),
                       g0_dof_step.u32);

    return;
}

/***************************************************************************************
 * func        : drv_hifb_hal_gfx_get_layer_in_rect
 * description : CNcomment: 获取图层的输入数据 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_get_layer_in_rect(hi_u32 data, OPTM_VDP_DISP_RECT_S *in_rect)
{
    /* * LBOX * */
    volatile u_g0_vfpos g0_vfpos;
    volatile u_g0_vlpos g0_vlpos;
    volatile u_g0_dfpos g0_dfpos;
    volatile u_g0_dlpos g0_dlpos;
    volatile u_gfx_src_crop gfx_src_crop;
    volatile u_gfx_ireso gfx_ireso;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    if (NULL != in_rect) {
        g0_vfpos.u32 =
            optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_vfpos.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
        in_rect->u32VX = g0_vfpos.bits.video_xfpos;
        in_rect->u32VY = g0_vfpos.bits.video_yfpos;

        g0_dfpos.u32 =
            optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_dfpos.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
        in_rect->u32DXS = g0_dfpos.bits.disp_xfpos;
        in_rect->u32DYS = g0_dfpos.bits.disp_yfpos;

        g0_dlpos.u32 =
            optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_dlpos.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
        in_rect->u32DXL = g0_dlpos.bits.disp_xlpos + 1;
        in_rect->u32DYL = g0_dlpos.bits.disp_ylpos + 1;

        g0_vlpos.u32 =
            optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_vlpos.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
        in_rect->u32VXL = g0_vlpos.bits.video_xlpos + 1;
        in_rect->u32VYL = g0_vlpos.bits.video_ylpos + 1;

        gfx_src_crop.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_crop.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        in_rect->u32SX = gfx_src_crop.bits.src_crop_x;
        in_rect->u32SY = gfx_src_crop.bits.src_crop_y;

        gfx_ireso.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_ireso.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        in_rect->u32IWth = gfx_ireso.bits.ireso_w + 1;
        in_rect->u32IHgt = gfx_ireso.bits.ireso_h + 1;
    }

    return;
}
#endif

/***************************************************************************************
 * func          : drv_hifb_hal_gfx_set_layer_reso
 * description   : CNcomment: 设置图层分辨率 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_layer_reso(hi_u32 data, OPTM_VDP_DISP_RECT_S *rect)
{
    /* * LBOX * */
    volatile u_g0_vfpos g0_vfpos;
    volatile u_g0_vlpos g0_vlpos;
    volatile u_g0_dfpos g0_dfpos;
    volatile u_g0_dlpos g0_dlpos;
    volatile u_gfx_src_reso gfx_src_reso;
    volatile u_gfx_src_crop gfx_src_crop;
    volatile u_gfx_ireso gfx_ireso;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    if (NULL != rect) {
        g0_vfpos.u32 =
            optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_vfpos.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
        g0_vfpos.bits.video_xfpos = rect->u32VX;
        g0_vfpos.bits.video_yfpos = rect->u32VY;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_vfpos.u32)) + data * CONFIG_HIFB_CTL_OFFSET),
                           g0_vfpos.u32);

        g0_vlpos.u32 =
            optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_vlpos.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
        g0_vlpos.bits.video_xlpos = rect->u32VX + rect->u32OWth;
        g0_vlpos.bits.video_xlpos = (0 == g0_vlpos.bits.video_xlpos) ? (0) : (g0_vlpos.bits.video_xlpos - 1);

        g0_vlpos.bits.video_ylpos = rect->u32VY + rect->u32OHgt;
        g0_vlpos.bits.video_ylpos = (0 == g0_vlpos.bits.video_ylpos) ? (0) : (g0_vlpos.bits.video_ylpos - 1);
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_vlpos.u32)) + data * CONFIG_HIFB_CTL_OFFSET),
                           g0_vlpos.u32);

        g0_dfpos.u32 =
            optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_dfpos.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
        g0_dfpos.bits.disp_xfpos = rect->u32DXS;
        g0_dfpos.bits.disp_yfpos = rect->u32DYS;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_dfpos.u32)) + data * CONFIG_HIFB_CTL_OFFSET),
                           g0_dfpos.u32);

        g0_dlpos.u32 =
            optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_dlpos.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
        g0_dlpos.bits.disp_xlpos = rect->u32DXL;
        g0_dlpos.bits.disp_xlpos = (0 == g0_dlpos.bits.disp_xlpos) ? (0) : (g0_dlpos.bits.disp_xlpos - 1);

        g0_dlpos.bits.disp_ylpos = rect->u32DYL;
        g0_dlpos.bits.disp_ylpos = (0 == g0_dlpos.bits.disp_ylpos) ? (0) : (g0_dlpos.bits.disp_ylpos - 1);
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_dlpos.u32)) + data * CONFIG_HIFB_CTL_OFFSET),
                           g0_dlpos.u32);

        gfx_src_crop.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_crop.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        gfx_src_crop.bits.src_crop_x = rect->u32SX;
        gfx_src_crop.bits.src_crop_y = rect->u32SY;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_crop.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_src_crop.u32);

        gfx_ireso.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_ireso.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        gfx_ireso.bits.ireso_w = rect->u32IWth;
        gfx_ireso.bits.ireso_w = (0 == gfx_ireso.bits.ireso_w) ? (0) : (gfx_ireso.bits.ireso_w - 1);

        gfx_ireso.bits.ireso_h = rect->u32IHgt;
        gfx_ireso.bits.ireso_h = (0 == gfx_ireso.bits.ireso_h) ? (0) : (gfx_ireso.bits.ireso_h - 1);
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_ireso.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_ireso.u32);

        gfx_src_reso.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_reso.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        gfx_src_reso.bits.src_w = rect->u32IWth;
        gfx_src_reso.bits.src_w = (0 == gfx_src_reso.bits.src_w) ? (0) : (gfx_src_reso.bits.src_w - 1);

        gfx_src_reso.bits.src_h = rect->u32IHgt;
        gfx_src_reso.bits.src_h = (0 == gfx_src_reso.bits.src_h) ? (0) : (gfx_src_reso.bits.src_h - 1);
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_src_reso.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                           gfx_src_reso.u32);
    }

    return;
}

/***************************************************************************************
 * func         : drv_hifb_hal_gfx_set_reg_up
 * description  : CNcomment: 设置图层更新使能寄存器，更新完之后硬件自动清0 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_reg_up(hi_u32 data)
{
    volatile u_g0_upd g0_upd;
    hi_u32 gp_id;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gp_id = hifb_hal_get_gp_id_from_layer_id(data);
    if ((gp_id == 0) && (g_up_mute[0] == HI_TRUE)) {
        return;
    }
#ifdef CONFIG_HIFB_WBC_UNSUPPORT
    if ((gp_id == 1) && (g_up_mute[1] == HI_TRUE)) {
        return;
    }
#endif

    g0_upd.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_upd.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
    g0_upd.bits.regup = 0x1;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_upd.u32)) + data * CONFIG_HIFB_CTL_OFFSET),
                       g0_upd.u32);

    return;
}

/***************************************************************************************
 * func        : drv_hifb_hal_gfx_set_layer_bkg
 * description : CNcomment: 设置图层背景色 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_layer_bkg(hi_u32 data, OPTM_VDP_BKG_S *bkg)
{
    volatile u_g0_bk g0_bk;
    volatile u_g0_alpha g0_alpha;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    if (NULL != bkg) {
        g0_bk.u32 =
            optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_bk.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        g0_bk.bits.vbk_y = bkg->u32BkgY;
        g0_bk.bits.vbk_cb = bkg->u32BkgU;
        g0_bk.bits.vbk_cr = bkg->u32BkgV;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_bk.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                           g0_bk.u32);

        g0_alpha.u32 =
            optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_alpha.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
        g0_alpha.bits.vbk_alpha = bkg->u32BkgA;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_alpha.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                           g0_alpha.u32);
    }

    return;
}

/***************************************************************************************
 * func         : drv_hifb_hal_gfx_set_layer_galpha
 * description  : CNcomment: 设置图层全局alpha CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_layer_galpha(hi_u32 data, hi_u32 alpha0)
{
    volatile u_g0_ctrl g0_ctrl;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    g0_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_ctrl.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
    g0_ctrl.bits.galpha = alpha0;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_ctrl.u32)) + data * CONFIG_HIFB_CTL_OFFSET),
                       g0_ctrl.u32);

    return;
}

#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_gfx_get_layer_galpha(hi_u32 data, hi_u8 *alpha0)
{
    volatile u_g0_ctrl g0_ctrl;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    g0_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_ctrl.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
    if (NULL != alpha0) {
        *alpha0 = g0_ctrl.bits.galpha;
    }

    return;
}
#endif

/***************************************************************************************
 * func          : drv_hifb_hal_gfx_set_palpha
 * description   : CNcomment: 设置pixle alpha值 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_palpha(hi_u32 data, hi_u32 alpha_en, hi_u32 arange, hi_u32 alpha0, hi_u32 alpha1)
{
    volatile u_gfx_out_ctrl gfx_out_ctrl;
    volatile u_gfx_1555_alpha gfx_1555_alpha;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gfx_1555_alpha.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_1555_alpha.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    gfx_1555_alpha.bits.alpha_0 = alpha0;
    gfx_1555_alpha.bits.alpha_1 = alpha1;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_1555_alpha.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                       gfx_1555_alpha.u32);

    gfx_out_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_out_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    gfx_out_ctrl.bits.palpha_en = alpha_en;
    gfx_out_ctrl.bits.palpha_range = arange;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_out_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                       gfx_out_ctrl.u32);

    return;
}

#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_gfx_get_palpha(hi_u32 data, hi_u32 *alpha_en, hi_u32 *arange, hi_u8 *alpha0, hi_u8 *alpha1)
{
    volatile u_gfx_out_ctrl gfx_out_ctrl;
    volatile u_gfx_1555_alpha gfx_1555_alpha;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gfx_1555_alpha.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_1555_alpha.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    if (NULL != alpha0) {
        *alpha0 = gfx_1555_alpha.bits.alpha_0;
    }
    if (NULL != alpha1) {
        *alpha1 = gfx_1555_alpha.bits.alpha_1;
    }

    gfx_out_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_out_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    if (NULL != arange) {
        *arange = gfx_out_ctrl.bits.palpha_range;
    }
    if (NULL != alpha_en) {
        *alpha_en = gfx_out_ctrl.bits.palpha_en;
    }

    return;
}

/***************************************************************************************
 * func          : drv_hifb_hal_gfx_set_layer_naddr
 * description   : CNcomment: 3D右眼显示地址 CNend\n
 * param[in]     : hi_void
 * retval        : NA
 * others:       : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_layer_naddr(hi_u32 data, hi_u32 n_addr)
{
    volatile u_gfx_naddr_l gfx_naddr_l;
#ifdef CONFIG_HIFB_MMU_SUPPORT
    volatile u_gfx_smmu_bypass gfx_smmu_bypass;
#endif
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gfx_naddr_l.u32 = n_addr;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_naddr_l.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                       gfx_naddr_l.u32);

#ifdef CONFIG_HIFB_MMU_SUPPORT
    gfx_smmu_bypass.u32 = optm_vdp_reg_read(
        (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_smmu_bypass.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
#ifdef CONFIG_GFX_MMU_SUPPORT
    gfx_smmu_bypass.bits.smmu_bypass_3d = 0;
#else
    gfx_smmu_bypass.bits.smmu_bypass_3d = 1;
#endif
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_smmu_bypass.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                       gfx_smmu_bypass.u32);
#endif

    return;
}
#endif

/***************************************************************************************
* func         : drv_hifb_hal_gfx_set_pre_mult_enable
* description  : CNcomment: 叠加寄存器预乘使能 CNend\n
* param[in]    : hi_void
                 HDR:   GP0   : 只有G0或G1:    depremult_en = 图层预乘开关一致
                                    G0+G1 :    depremult_en = 1
                 NoHDR: GP0   :  depremult_en = 0
                -----------------------------------------------------------------------
                 G0/G1 : 预乘开关跟随源
* retval       : NA
* others:      : NA
***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_pre_mult_enable(hi_u32 data, hi_u32 enable, hi_bool hdr)
{
    volatile u_gfx_out_ctrl gfx_out_ctrl;
    volatile u_gp0_ctrl gp0_ctrl;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gfx_out_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_out_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    gfx_out_ctrl.bits.premulti_en = enable;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_out_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET),
                       gfx_out_ctrl.u32);

    gp0_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_ctrl.u32))));

    gp0_ctrl.bits.depremult = 1;

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_ctrl.u32))), gp0_ctrl.u32);
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_f_ctrl.u32))), gp0_ctrl.u32);

    return;
}

hi_void drv_hifb_hal_gfx_get_pre_mult_enable(hi_u32 data, hi_u32 *enable, hi_bool *de_pre_mult)
{
    volatile u_gfx_out_ctrl gfx_out_ctrl;
    volatile u_gp0_ctrl gp0_ctrl;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    gfx_out_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_out_ctrl.u32)) + data * CONFIG_HIFB_GFX_OFFSET));
    if (NULL != enable) {
        *enable = gfx_out_ctrl.bits.premulti_en;
    }

    gp0_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_ctrl.u32))));
    if (NULL != de_pre_mult) {
        *de_pre_mult = gp0_ctrl.bits.depremult;
    }

    return;
}

hi_void drv_hifb_hal_gfx_read_register(hi_u32 offset, hi_u32 *reg_buf)
{
#if !defined(HI_LOG_SUPPORT) || (HI_LOG_SUPPORT != 0)
#else
    hi_u32 Index = 0;
    hi_u32 Loop = (NULL == reg_buf) ? (16) : (CONFIG_VDP_SIZE / 4);
    hi_u32 Value = 0x0;

    if (offset >= CONFIG_VDP_SIZE) {
        return;
    }

    if (NULL != reg_buf) {
        for (Index = 0; Index < Loop; Index++) {
            Value = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->voctrl.u32)) + Index * 4));
            *(reg_buf + Index) = Value;
        }
        return;
    }

    for (Index = 0; Index < Loop; Index++) {
        GRAPHIC_COMM_PRINT("%04x: ", (offset + Index * 16));
        Value = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->voctrl.u32)) + offset + Index * 16));
        GRAPHIC_COMM_PRINT("%08x ", Value);
        Value = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->voctrl.u32)) + offset + Index * 16 + 0x4));
        GRAPHIC_COMM_PRINT("%08x ", Value);
        Value = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->voctrl.u32)) + offset + Index * 16 + 0x8));
        GRAPHIC_COMM_PRINT("%08x ", Value);
        Value = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->voctrl.u32)) + offset + Index * 16 + 0xc));
        GRAPHIC_COMM_PRINT("%08x ", Value);
        GRAPHIC_COMM_PRINT("\n");
    }
#endif
    return;
}

hi_void drv_hifb_hal_gfx_write_register(hi_u32 offset, hi_u32 value)
{
    hi_u32 pre_value = 0x0;
    hi_u32 cur_value = 0x0;

    if ((offset < 0xc) || ((offset > 0x10) && (offset < 0x5000)) || (offset >= 0xc000)) {
        return;
    }

    pre_value = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->voctrl.u32)) + offset));

    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->voctrl.u32)) + offset), value);

    cur_value = optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->voctrl.u32)) + offset));

#if defined(HI_LOG_SUPPORT) && (0 == HI_LOG_SUPPORT)
    GRAPHIC_COMM_PRINT("\n%04x: 0x%x - > 0x%x\n", offset, pre_value, cur_value);
#endif
    return;
}

#if !defined(HI_BUILD_IN_BOOT) && defined(CONFIG_HIFB_LOWPOWER_SUPPORT)
hi_void drv_hifb_hal_gfx_set_low_power_info(hi_u32 data, hi_u32 *low_power_info)
{
    return;
}

hi_void drv_hifb_hal_gfx_enable_layer_low_power(hi_u32 data, hi_bool en_low_power)
{
    return;
}

hi_void drv_hifb_hal_gfx_enable_gp_low_power(hi_u32 data, hi_bool en_low_power)
{
    return;
}
#endif

/***************************************************************************************
 * func         : drv_hifb_hal_gfx_set_upd_mode
 * description  : CNcomment: 设置图层更新数据模式 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gfx_set_upd_mode(hi_u32 data, hi_u32 mode)
{
    volatile u_g0_ctrl g0_ctrl;
    CONFIG_HIFB_CHECK_LAYER_SUPPORT(data);

    g0_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_ctrl.u32)) + data * CONFIG_HIFB_CTL_OFFSET));
    g0_ctrl.bits.rgup_mode = mode;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->g0_ctrl.u32)) + data * CONFIG_HIFB_CTL_OFFSET),
                       g0_ctrl.u32);

    return;
}

// -------------------------------------------------------------------
// GP_BEGIN
// -------------------------------------------------------------------
hi_void drv_hifb_hal_gp_set_para_upd(hi_u32 data, OPTM_VDP_GP_PARA_E mode)
{
    volatile u_para_up_vhd para_up_vhd;
    hi_u32 gp_num = hifb_hal_get_gp_id(data);
    CONFIG_HIFB_CHECK_GP_SUPPORT(gp_num);

    para_up_vhd.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
#ifdef CONFIG_HIFB_VERSION_2_0
    para_up_vhd.bits.para_up_vhd_chn16 = 0x1;
#endif

#ifdef CONFIG_HIFB_VERSION_3_0
    para_up_vhd.bits.para_up_vhd_chn04 = 0x1;
#endif
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->para_up_vhd.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       para_up_vhd.u32);

    return;
}

/***************************************************************************************
 * func         : drv_hifb_hal_gp_set_read_mode
 * description  : CNcomment: 设置GP读取数据模式 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gp_set_read_mode(hi_u32 data, hi_u32 mode)
{
#ifdef CONFIG_HIFB_VERSION_4_0
#else
    volatile u_gp0_ctrl GP0_CTRL;
    hi_u32 GpNum = hifb_hal_get_gp_id(data);
    CONFIG_HIFB_CHECK_GP_SUPPORT(GpNum);

    GP0_CTRL.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_ctrl.u32)) + GpNum * CONFIG_HIFB_GP_OFFSET));
    GP0_CTRL.bits.read_mode = mode;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_ctrl.u32)) + GpNum * CONFIG_HIFB_GP_OFFSET),
                       GP0_CTRL.u32);
#endif
    return;
}

hi_void drv_hifb_hal_gp_set_up_mode(hi_u32 data, hi_u32 mode)
{
    volatile u_gp0_ctrl gp0_ctrl;
    hi_u32 gp_num = hifb_hal_get_gp_id(data);
#ifdef CONFIG_HIFB_VERSION_4_0
    volatile u_gp0_f_ctrl gp0_f_ctrl;
#endif
    CONFIG_HIFB_CHECK_GP_SUPPORT(gp_num);

    gp0_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_ctrl.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
    gp0_ctrl.bits.rgup_mode = mode;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_ctrl.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       gp0_ctrl.u32);

#ifdef CONFIG_HIFB_VERSION_4_0
    gp0_f_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_f_ctrl.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
    gp0_f_ctrl.bits.rgup_mode = mode;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_f_ctrl.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       gp0_f_ctrl.u32);
#endif
    return;
}

/***************************************************************************************
 * func         : drv_hifb_hal_gp_get_rect
 * description  : CNcomment: 获取GP大小 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
#ifndef HI_BUILD_IN_BOOT
hi_void drv_hifb_hal_gp_get_rect(hi_u32 data, OPTM_VDP_DISP_RECT_S *rect)
{
    volatile u_gp0_ireso gp0_ireso;
    volatile u_gp0_zme_vinfo gp0_zme_vinfo;
    volatile u_gp0_zme_hinfo gp0_zme_hinfo;
    hi_u32 gp_num = hifb_hal_get_gp_id(data);
    CONFIG_HIFB_CHECK_GP_SUPPORT(gp_num);

    if (NULL != rect) {
        gp0_ireso.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_ireso.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
        rect->u32IWth = gp0_ireso.bits.iw + 1;
        rect->u32IHgt = gp0_ireso.bits.ih + 1;

        gp0_zme_vinfo.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vinfo.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
        gp0_zme_hinfo.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hinfo.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
        rect->u32OWth = gp0_zme_hinfo.bits.out_width + 1;
        rect->u32OHgt = gp0_zme_vinfo.bits.out_height + 1;
    }

    return;
}
#endif

/***************************************************************************************
 * func         : drv_hifb_hal_gp_set_layer_reso
 * description  : CNcomment: 设置GP输入输出分辨率 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gp_set_layer_reso(hi_u32 data, OPTM_VDP_DISP_RECT_S *rect)
{
    volatile u_gp0_vfpos gp0_vfpos;
    volatile u_gp0_vlpos gp0_vlpos;
    volatile u_gp0_dfpos gp0_dfpos;
    volatile u_gp0_dlpos gp0_dlpos;
    volatile u_gp0_ireso gp0_ireso;
    volatile u_gp0_zme_vinfo gp0_zme_vinfo;
    volatile u_gp0_zme_hinfo gp0_zme_hinfo;
    hi_u32 gp_num = hifb_hal_get_gp_id(data);
    CONFIG_HIFB_CHECK_GP_SUPPORT(gp_num);

    if (NULL != rect) {
        /* * video position * */
        gp0_vfpos.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_vfpos.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
        gp0_vfpos.bits.video_xfpos = rect->u32VX;
        gp0_vfpos.bits.video_yfpos = rect->u32VY;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_vfpos.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                           gp0_vfpos.u32);

        gp0_vlpos.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_vlpos.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
        gp0_vlpos.bits.video_xlpos = rect->u32VX + rect->u32OWth - 1;
        gp0_vlpos.bits.video_ylpos = rect->u32VY + rect->u32OHgt - 1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_vlpos.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                           gp0_vlpos.u32);

        gp0_dfpos.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_dfpos.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
        gp0_dfpos.bits.disp_xfpos = rect->u32DXS;
        gp0_dfpos.bits.disp_yfpos = rect->u32DYS;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_dfpos.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                           gp0_dfpos.u32);

        gp0_dlpos.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_dlpos.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
        gp0_dlpos.bits.disp_xlpos = rect->u32DXL - 1;
        gp0_dlpos.bits.disp_ylpos = rect->u32DYL - 1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_dlpos.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                           gp0_dlpos.u32);

        gp0_ireso.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_ireso.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
        gp0_ireso.bits.iw = rect->u32IWth - 1;
        gp0_ireso.bits.ih = rect->u32IHgt - 1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_ireso.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                           gp0_ireso.u32);

        gp0_zme_vinfo.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vinfo.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
        gp0_zme_hinfo.u32 = optm_vdp_reg_read(
            (hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hinfo.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
        gp0_zme_hinfo.bits.out_width = rect->u32OWth - 1;
        gp0_zme_vinfo.bits.out_height = rect->u32OHgt - 1;
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_vinfo.u32)) +
                                      gp_num * CONFIG_HIFB_GP_OFFSET),
                           gp0_zme_vinfo.u32);
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_zme_hinfo.u32)) +
                                      gp_num * CONFIG_HIFB_GP_OFFSET),
                           gp0_zme_hinfo.u32);
    }

    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_HAL_GP_SetLayerGalpha
 * description  : CNcomment: 设置全局alpha CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gp_set_layer_galpha(hi_u32 data, hi_u32 alpha)
{
    volatile u_gp0_ctrl gp0_ctrl;
    hi_u32 gp_num = hifb_hal_get_gp_id(data);
#ifdef CONFIG_HIFB_VERSION_4_0
    volatile u_gp0_ctrl gp0_f_ctrl;
#endif
    CONFIG_HIFB_CHECK_GP_SUPPORT(gp_num);

    gp0_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_ctrl.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
    gp0_ctrl.bits.galpha = alpha;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_ctrl.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       gp0_ctrl.u32);

#ifdef CONFIG_HIFB_VERSION_4_0
    gp0_f_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_f_ctrl.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET));
    gp0_f_ctrl.bits.galpha = alpha;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_f_ctrl.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       gp0_f_ctrl.u32);
#endif
    return;
}

/***************************************************************************************
 * func        : drv_hifb_hal_gp_set_reg_up
 * description : CNcomment: 更新GP寄存器 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_gp_set_reg_up(hi_u32 data)
{
    volatile u_gp0_upd gp0_upd;
    hi_u32 gp_num = hifb_hal_get_gp_id(data);
#ifdef CONFIG_HIFB_VERSION_4_0
    volatile u_gp0_f_upd gp0_f_upd;
#endif
    CONFIG_HIFB_CHECK_GP_SUPPORT(gp_num);

    if ((data == 0) && (g_up_mute[0] == HI_TRUE)) {
        return;
    }
#ifdef CONFIG_HIFB_WBC_UNSUPPORT
    if ((data == 1) && (g_up_mute[1] == HI_TRUE)) { /* 1 is gp1 */
        return;
    }
#endif

    gp0_upd.bits.regup = 0x1;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_upd.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       gp0_upd.u32);

#ifdef CONFIG_HIFB_VERSION_4_0
    gp0_f_upd.bits.regup = 0x1;
    optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gp0_f_upd.u32)) + gp_num * CONFIG_HIFB_GP_OFFSET),
                       gp0_f_upd.u32);
#endif
    return;
}

// -------------------------------------------------------------------
// WBC_GFX_BEGIN
// -------------------------------------------------------------------

/***************************************************************************************
 * func         : DRV_HIFB_HAL_WBC_SetThreeMd
 * description  : CNcomment:  设置回写模式 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_wbc_set_three_md(OPTM_VDP_LAYER_WBC_E layer, hi_u32 mode)
{
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    volatile U_WBC_GP_CTRL wbc_gp_ctrl;

    if (OPTM_VDP_LAYER_WBC_GP0 == layer) {
        wbc_gp_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_ctrl.u32)));
        wbc_gp_ctrl.bits.three_d_mode = mode;
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_ctrl.u32)), wbc_gp_ctrl.u32);
    }
#else
#endif
    return;
}

/***************************************************************************************
 * func        : drv_hifb_hal_wbc_gp_set_enable
 * description : CNcomment: 配置回写使能寄存器 CNend\n
 * param[in]   : hi_void
 * retval      : NA
 * others:     : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_wbc_gp_set_enable(OPTM_VDP_LAYER_WBC_E layer, hi_u32 enable)
{
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    volatile U_WBC_GP_CTRL wbc_gp_ctrl;

    if (OPTM_VDP_LAYER_WBC_GP0 == layer) {
        wbc_gp_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_ctrl.u32)));
        wbc_gp_ctrl.bits.wbc_en = enable;
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_ctrl.u32)), wbc_gp_ctrl.u32);
    }
#else
#endif
    return;
}

hi_void drv_hifb_hal_wbc_gp_get_work_enable(OPTM_VDP_LAYER_WBC_E layer, hi_u32 *enable, hi_u32 *wbc_state)
{
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    volatile U_WBC_GP_CTRL wbc_gp_ctrl;
    volatile U_GFX_WBC_STA gfx_wbc_sta;

    if ((OPTM_VDP_LAYER_WBC_GP0 == layer) && (NULL != enable)) {
        wbc_gp_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_ctrl_work.u32)));
        *enable = wbc_gp_ctrl.bits.wbc_en;
    }

    if ((OPTM_VDP_LAYER_WBC_GP0 == layer) && (NULL != wbc_state)) {
        gfx_wbc_sta.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->gfx_wbc_sta.u32)));
        *wbc_state = gfx_wbc_sta.u32;
    }
#else
#endif
    return;
}

#ifndef HI_BUILD_IN_BOOT
/***************************************************************************************
 * func         : drv_hifb_hal_wbc_gp_get_enable
 * description  : CNcomment: 回写是否使能 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_wbc_gp_get_enable(OPTM_VDP_LAYER_WBC_E layer, hi_u32 *enable)
{
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    volatile U_WBC_GP_CTRL wbc_gp_ctrl;

    if ((OPTM_VDP_LAYER_WBC_GP0 == layer) && (NULL != enable)) {
        wbc_gp_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_ctrl.u32)));
        *enable = wbc_gp_ctrl.bits.wbc_en;
    }
#else
#endif
    return;
}
#endif

/***************************************************************************************
 * func         : DRV_HIFB_HAL_WBC_SetOutMod
 * description  : CNcomment:WBC输出模式。 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_wbc_set_out_mod(OPTM_VDP_LAYER_WBC_E layer, hi_u32 out_mode)
{
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    volatile U_WBC_GP_CTRL wbc_gp_ctrl;

    if (OPTM_VDP_LAYER_WBC_GP0 == layer) {
        wbc_gp_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_ctrl.u32)));
        wbc_gp_ctrl.bits.mode_out = out_mode;
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_ctrl.u32)), wbc_gp_ctrl.u32);
    }
#else
#endif
    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_HAL_WBC_SetRegUp
 * description  : CNcomment:更新回写寄存器 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_wbc_set_reg_up(OPTM_VDP_LAYER_WBC_E layer)
{
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    hi_s32 ret = HI_SUCCESS;
    hi_u32 wbc_write_address = 0;
    volatile U_WBC_GP_UPD wbc_gp_upd;

    if (OPTM_VDP_LAYER_WBC_GP0 != layer) {
        return;
    }

    wbc_write_address = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->gfx_wbc_yaddr_l.u32)));
    ret = hifb_check_address(wbc_write_address);
    if (ret != HI_SUCCESS) {
        return;
    }

    wbc_gp_upd.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_upd.u32)));
    wbc_gp_upd.bits.regup = 0x1;
    optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_upd.u32)), wbc_gp_upd.u32);
#else
#endif
    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_HAL_WBC_SetOutFmt
 * description  : CNcomment:WBC的输出数据格式。 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_wbc_set_out_fmt(OPTM_VDP_LAYER_WBC_E layer, OPTM_VDP_WBC_OFMT_E intf_fmt)
{
    /* * logic register default use ARGB8888, so not need set * */
    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_HAL_WBC_SetLayerAddr
 * description  : CNcomment: 设置WBC回写地址 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_wbc_set_layer_addr(OPTM_VDP_LAYER_WBC_E layer, hi_u32 addr, hi_u32 stride)
{
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    volatile U_GFX_WBC_YSTRIDE gfx_wbc_ystride;
#ifdef CONFIG_HIFB_MMU_SUPPORT
    volatile U_GFX_WBC_SMMU_BYPASS gfx_wbc_smmu_bypass;
#endif

    if ((OPTM_VDP_LAYER_WBC_GP0 == layer) && (0 != addr)) {
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->gfx_wbc_yaddr_l.u32)), addr);
        gfx_wbc_ystride.bits.wbc_ystride = stride;
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->gfx_wbc_ystride.u32)), gfx_wbc_ystride.u32);
#ifdef CONFIG_HIFB_MMU_SUPPORT
        gfx_wbc_smmu_bypass.u32 =
            optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->gfx_wbc_smmu_bypass.u32))));
#ifdef CONFIG_GFX_MMU_SUPPORT
        gfx_wbc_smmu_bypass.bits.mmu_3d_bypass = 0;
        gfx_wbc_smmu_bypass.bits.mmu_2d_bypass = 0;
#else
        gfx_wbc_smmu_bypass.bits.mmu_3d_bypass = 1;
        gfx_wbc_smmu_bypass.bits.mmu_2d_bypass = 1;
#endif
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->gfx_wbc_smmu_bypass.u32)), gfx_wbc_smmu_bypass.u32);
#endif
    }
#else
#endif
    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_HAL_WBC_GetLayerAddr
 * description  : CNcomment: 读取WBC回写地址 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_wbc_get_layer_addr(OPTM_VDP_LAYER_WBC_E layer, hi_u32 *addr, hi_u32 *stride)
{
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    if ((OPTM_VDP_LAYER_WBC_GP0 == layer) && (NULL != addr) && (NULL != stride)) {
        *addr = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->gfx_wbc_yaddr_l.u32)));
        *stride = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->gfx_wbc_ystride.u32)));
    }
#else
#endif
    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_HAL_WBC_SetLayerReso
 * description  : CNcomment: 设置回写标清图层分辨率 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_wbc_set_layer_reso(OPTM_VDP_LAYER_WBC_E layer, OPTM_VDP_DISP_RECT_S *rect)
{
    /* * logic register from wbc_gp zme, so not need set * */
    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_HAL_WBC_SetDitherMode
 * description  : CNcomment: Dither输出模式选择。 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_wbc_set_dither_mode(OPTM_VDP_LAYER_WBC_E layer, OPTM_VDP_DITHER_E dither_mode)
{
    /* * not need set * */
    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_HAL_WBC_SetCropReso
 * description  : CNcomment: 设置回写标清图层裁剪分辨率 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_wbc_set_crop_reso(OPTM_VDP_LAYER_WBC_E layer, OPTM_VDP_DISP_RECT_S *rect)
{
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    volatile U_WBC_G0_FCROP wbc_g0_fcrop;
    volatile U_WBC_G0_LCROP wbc_g0_lcrop;

    if ((layer == OPTM_VDP_LAYER_WBC_GP0) && (NULL != rect)) {
        wbc_g0_fcrop.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_g0_fcrop.u32)));
        wbc_g0_fcrop.bits.wfcrop = rect->u32DXS;
        wbc_g0_fcrop.bits.hfcrop = rect->u32DYS;
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_g0_fcrop.u32)), wbc_g0_fcrop.u32);

        wbc_g0_lcrop.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_g0_lcrop.u32)));
        wbc_g0_lcrop.bits.wlcrop = rect->u32DXL - 1;
        wbc_g0_lcrop.bits.hlcrop = rect->u32DYL - 1;
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_g0_lcrop.u32)), wbc_g0_lcrop.u32);
    }
#else
#endif
    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_HAL_WBC_LBoxEnable
 * description  : CNcomment: LBox使能 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_wbc_lbox_enable(OPTM_VDP_LAYER_WBC_E layer, hi_bool enable)
{
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    volatile U_WBC_GP_LBX_CTRL wbc_gp_lbx_ctrl;

    if (layer == OPTM_VDP_LAYER_WBC_GP0) {
        wbc_gp_lbx_ctrl.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_lbx_ctrl.u32)));
        wbc_gp_lbx_ctrl.bits.lbx_en = enable;
        wbc_gp_lbx_ctrl.bits.lbx_ck_gt_en = enable;
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_lbx_ctrl.u32)), wbc_gp_lbx_ctrl.u32);
    }
#else
#endif
    return;
}

/***************************************************************************************
 * func         : DRV_HIFB_HAL_WBC_SetLBoxReso
 * description  : CNcomment: 设置LBox分辨率 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_wbc_set_lbox_reso(OPTM_VDP_LAYER_WBC_E layer, OPTM_VDP_DISP_RECT_S *rect)
{
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    volatile U_WBC_GP_LBX_ORESO wbc_gp_lbx_oreso;
    volatile U_WBC_GP_LBX_VFPOS wbc_gp_lbx_vfpos;
    volatile U_WBC_GP_LBX_VLPOS wbc_gp_lbx_vlpos;

    if ((layer == OPTM_VDP_LAYER_WBC_GP0) && (NULL != rect)) {
        wbc_gp_lbx_oreso.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_lbx_oreso.u32)));
        wbc_gp_lbx_oreso.bits.ow = rect->u32OWth;
        wbc_gp_lbx_oreso.bits.oh = rect->u32OHgt;
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_lbx_oreso.u32)), wbc_gp_lbx_oreso.u32);

        wbc_gp_lbx_vfpos.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_lbx_vfpos.u32)));
        wbc_gp_lbx_vfpos.bits.video_xfpos = rect->u32DXS;
        wbc_gp_lbx_vfpos.bits.video_yfpos = rect->u32DYS;
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_lbx_vfpos.u32)), wbc_gp_lbx_vfpos.u32);

        wbc_gp_lbx_vlpos.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_lbx_vlpos.u32)));
        wbc_gp_lbx_vlpos.bits.video_xlpos = rect->u32DXL;
        wbc_gp_lbx_vlpos.bits.video_ylpos = rect->u32DYL;
        optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->wbc_gp_lbx_vlpos.u32)), wbc_gp_lbx_vlpos.u32);
    }
#else
#endif
    return;
}

// -------------------------------------------------------------------
// WBC_GFX_END
// -------------------------------------------------------------------
OPTM_VDP_DISP_MODE_E drv_hifb_hal_disp_get_disp_mode(hi_u32 data)
{
    volatile u_dhd0_ctrl dhd0_ctrl;

    if ((data >= OPTM_CHN_MAX) || (NULL == g_optm_vdp_reg)) {
        GRAPHIC_DFX_ERR_VALUE(GRAPHIC_DFX_ERR_UINT, data);
        return VDP_DISP_MODE_2D;
    }

    dhd0_ctrl.u32 =
        optm_vdp_reg_read((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->dhd0_ctrl.u32)) + data * CONFIG_HIFB_CHN_OFFSET));

    return (OPTM_VDP_DISP_MODE_E)dhd0_ctrl.bits.disp_mode;
}

#ifndef HI_BUILD_IN_BOOT
/***************************************************************************************
 * func         : drv_hifb_hal_disp_get_int_signal
 * description  : CNcomment: 获取中断状态 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_u32 drv_hifb_hal_disp_get_int_signal(hi_u32 u32intmask)
{
    volatile u_vointsta vointsta;

    vointsta.u32 = optm_vdp_reg_read((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->vointsta.u32)));

    return (vointsta.u32 & u32intmask);
}

/***************************************************************************************
 * func         : drv_hifb_hal_disp_clear_int_signal
 * description  : CNcomment: 清除中断状态 CNend\n
 * param[in]    : hi_void
 * retval       : NA
 * others:      : NA
 ***************************************************************************************/
hi_void drv_hifb_hal_disp_clear_int_signal(hi_u32 u32intmask)
{
    optm_vdp_reg_write((hi_u32 *)(uintptr_t)(&(g_optm_vdp_reg->vomskintsta.u32)), u32intmask);
}
#endif

hi_void drv_hifb_hal_gfx_set_interrupt(hi_bool open)
{
    if (HI_TRUE == open) {
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->vointmsk.u32))), 0x37);
    } else {
        optm_vdp_reg_write((hi_u32 *)((uintptr_t)(&(g_optm_vdp_reg->vointmsk.u32))), 0x0);
    }
}
