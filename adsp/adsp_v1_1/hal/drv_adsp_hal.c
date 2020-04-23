/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: adsp hal driver
 * Author: Audio
 * Create: 2019-10-30
 * Notes: NA
 * History: 2019-10-30 put sw and hw adsp driver together
 */

#include "hi_adsp_debug.h"

#include "drv_adsp_hw.h"
#include "drv_adsp_sw.h"

#include "linux/hisilicon/hi_license.h"

enum {
    HI_LICENSE_AIDSP_EN_MASK = 0x1,
    HI_LICENSE_ADSP_EN_MASK  = 0x2,
};

static hi_bool check_adsp_support(hi_void)
{
#ifdef HI_SND_DSP_SUPPORT
    hi_s32 ret;
    hi_u32 value = 0;

    ret = hi_drv_get_license_support(HI_LICENSE_ADSP_EN, &value);
    if (ret != 0) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_get_license_support, ret);
        return HI_FALSE;
    }

    HI_DBG_PRINT_H32(value);

    return !!(value & HI_LICENSE_ADSP_EN_MASK);
#else
    return HI_FALSE;
#endif
}

static adsp_driver *adsp_get_driver(hi_void)
{
    adsp_driver *ops = HI_NULL;

    if (check_adsp_support() == HI_TRUE) {
        ops = hw_adsp_get_driver();
    } else {
        ops = sw_adsp_get_driver();
    }

    if (ops == HI_NULL ||
        ops->load_fw == HI_NULL ||
        ops->load_aoe == HI_NULL ||
        ops->unload_fw == HI_NULL ||
        ops->unload_aoe == HI_NULL ||
        ops->get_aoe_info == HI_NULL) {
        return HI_NULL;
    }

    return ops;
}

hi_s32 adsp_load_aoe_firmware(hi_void)
{
    hi_s32 ret;
    adsp_driver *ops = adsp_get_driver();
    if (ops == HI_NULL) {
        return HI_FAILURE;
    }

    ret = ops->load_fw();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ops->load_fw, ret);
        return ret;
    }

    ret = ops->load_aoe();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ops->load_aoe, ret);
        goto out;
    }

    return HI_SUCCESS;

out:
    ops->unload_fw();
    return ret;
}

hi_s32 adsp_unload_aoe_firmware(hi_void)
{
    hi_s32 ret;
    adsp_driver *ops = adsp_get_driver();
    if (ops == HI_NULL) {
        return HI_FAILURE;
    }

    ret = ops->unload_aoe();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ops->unload_aoe, ret);
        return ret;
    }

    ops->unload_fw();

    return HI_SUCCESS;
}

hi_s32 adsp_get_aoe_firmware_info(aoe_fw_info *info)
{
    hi_s32 ret;
    adsp_driver *ops = adsp_get_driver();
    if (ops == HI_NULL) {
        return HI_FAILURE;
    }

    ret = ops->get_aoe_info(info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ops->get_aoe_info, ret);
        return ret;
    }

    return HI_SUCCESS;
}
