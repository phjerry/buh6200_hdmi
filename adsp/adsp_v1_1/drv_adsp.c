/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drv adsp
 * Author: Audio
 * Create: 2012-05-30
 * Notes: NA
 * History: 2019-05-30 CSEC Rectification
 */

#include "hi_adsp_debug.h"

#include "hi_osal.h"

#include "drv_adsp_private.h"
#include "drv_adsp_ext.h"
#include "hi_audsp_aoe.h"

#include "drv_adsp_hal.h"
#include "drv_adsp_cmd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    adsp_export_func ext_func; /* ADSP extenal functions */
} adsp_global_param;

static hi_s32 adsp_load_firmware(adsp_code_id dsp_code_id);
static hi_s32 adsp_unload_firmware(adsp_code_id dsp_code_id);
static hi_s32 adsp_get_aoe_fw_info(adsp_code_id dsp_code_id, aoe_fw_info *info);

static const adsp_global_param g_adsp_drv = {
    .ext_func = {
        .adsp_load_firmware = adsp_load_firmware,
        .adsp_unload_firmware = adsp_unload_firmware,
        .adsp_get_aoe_fw_info = adsp_get_aoe_fw_info,
        .adsp_write_cmd = adsp_write_cmd,
    }
};

static hi_s32 adsp_load_firmware(adsp_code_id dsp_code_id)
{
    hi_s32 ret;

    switch (dsp_code_id) {
        case ADSP_CODE_AOE:
            ret = adsp_load_aoe_firmware();
            if (ret != HI_SUCCESS) {
                HI_FATAL_PRINT_FUNC_RES(adsp_load_aoe_firmware, ret);
                return ret;
            }

            break;

        default:
            HI_LOG_WARN("unsupport dsp code id\n");
            HI_WARN_PRINT_H32(dsp_code_id);
            return HI_FAILURE;
    }

#ifdef MODULE
    /* increase module ref_count */
    __module_get(THIS_MODULE);
#endif

    return HI_SUCCESS;
}

static hi_s32 adsp_unload_firmware(adsp_code_id dsp_code_id)
{
    hi_s32 ret;

    switch (dsp_code_id) {
        case ADSP_CODE_AOE:
            ret = adsp_unload_aoe_firmware();
            if (ret != HI_SUCCESS) {
                HI_FATAL_PRINT_FUNC_RES(adsp_unload_aoe_firmware, ret);
                return ret;
            }

            break;

        default:
            HI_LOG_WARN("unsupport dsp code id\n");
            HI_WARN_PRINT_H32(dsp_code_id);
            return HI_FAILURE;
    }

#ifdef MODULE
    /* decrease module ref_count */
    module_put(THIS_MODULE);
#endif

    return HI_SUCCESS;
}

static hi_s32 adsp_get_aoe_fw_info(adsp_code_id dsp_code_id, aoe_fw_info *info)
{
    hi_s32 ret;

    switch (dsp_code_id) {
        case ADSP_CODE_AOE:
            ret = adsp_get_aoe_firmware_info(info);
            if (ret != HI_SUCCESS) {
                HI_FATAL_PRINT_FUNC_RES(adsp_get_aoe_firmware_info, ret);
                return ret;
            }

            break;

        default:
            HI_LOG_ERR("unsupport dsp code id\n");
            HI_ERR_PRINT_H32(dsp_code_id);
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 adsp_drv_init(hi_void)
{
    hi_s32 ret;

    ret = osal_exportfunc_register(HI_ID_ADSP, "HI_ADSP",
        (hi_void *)&g_adsp_drv.ext_func);
    if (ret != HI_SUCCESS) {
        HI_FATAL_PRINT_FUNC_RES(osal_exportfunc_register, ret);
    }

    return ret;
}

hi_void adsp_drv_exit(hi_void)
{
    osal_exportfunc_unregister(HI_ID_ADSP);
}

hi_s32 hi_drv_adsp_init(hi_void)
{
    return adsp_drv_init();
}

hi_void hi_drv_adsp_deinit(hi_void)
{
    adsp_drv_exit();
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */
