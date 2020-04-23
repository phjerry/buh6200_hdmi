/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: implement of aiao driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_osal.h"
#include "hi_aiao_log.h"

#include "hi_drv_dev.h"
#include "audio_mem.h"
#include "drv_ao_ext.h"

#ifdef HI_AUDIO_AI_SUPPORT
/* if module is enabled, we include the head files */
#include "drv_ai_ext.h"
#else
/* if module is disalbed, we define the module function NULL to make caller clean */
#define ai_drv_mod_init HI_NULL
#define ai_drv_mod_exit HI_NULL
#endif

#ifdef HI_AIAO_TIMER_SUPPORT
#include "drv_timer_private.h"
#else
#define timer_drv_init HI_NULL
#define timer_drv_deinit HI_NULL
#endif

#ifdef HI_ALSA_AO_SUPPORT
#include "alsa_aiao.h"
#else
#define ao_alsa_mod_init HI_NULL
#define ao_alsa_mod_exit HI_NULL
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

static const audio_export_func audio_mem_func = {
    .mmz_alloc        = hi_drv_audio_mmz_alloc,
    .mmz_release      = hi_drv_audio_mmz_release,
    .sec_mmz_alloc    = hi_drv_audio_sec_mmz_alloc,
    .sec_mmz_release  = hi_drv_audio_sec_mmz_release,

    .smmu_alloc       = hi_drv_audio_smmu_alloc,
    .smmu_release     = hi_drv_audio_smmu_release,
    .sec_smmu_alloc   = hi_drv_audio_sec_smmu_alloc,
    .sec_smmu_release = hi_drv_audio_sec_smmu_release,
};

typedef struct {
    hi_s32  (*mod_init)(hi_void);
    hi_void (*mod_exit)(hi_void);
} aiao_mod_map;

static const aiao_mod_map g_aiao_mod_map[] = {
    { ai_drv_mod_init,  ai_drv_mod_exit },
    { ao_drv_mod_init,  ao_drv_mod_exit },
    { timer_drv_init,   timer_drv_deinit },
    { ao_alsa_mod_init, ao_alsa_mod_exit },
};

hi_s32 aiao_drv_mod_init(hi_void)
{
    hi_s32 ret;
    hi_s32 module_id;
    hi_s32 (*mod_init)(hi_void) = HI_NULL;

    ret = osal_exportfunc_register(HI_ID_AIAO, "HI_AIAO", (hi_void *)&audio_mem_func);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(osal_exportfunc_register, ret);
        return ret;
    }

    for (module_id = 0; module_id < ARRAY_SIZE(g_aiao_mod_map); module_id++) {
        mod_init = g_aiao_mod_map[module_id].mod_init;
        if (mod_init == HI_NULL) {
            continue;
        }

        ret = mod_init();
        if (ret != HI_SUCCESS) {
            HI_FATAL_PRINT_FUNC_RES(mod_init, ret);
            HI_FATAL_PRINT_U32(module_id);
            goto out;
        }
    }

#ifdef MODULE
    HI_PRINT("Load hi_aiao.ko success.  \t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;

out:
    for (; module_id >= 0; module_id--) {
        hi_void (*mod_exit)(hi_void) = g_aiao_mod_map[module_id].mod_exit;
        if (mod_exit == HI_NULL) {
            continue;
        }

        mod_exit();
    }

    osal_exportfunc_unregister(HI_ID_AIAO);

    return ret;
}

hi_void aiao_drv_mod_exit(hi_void)
{
    hi_s32 ret;
    hi_s32 module_id = ARRAY_SIZE(g_aiao_mod_map) - 1;
    hi_void (*mod_exit)(hi_void) = HI_NULL;

    for (; module_id >= 0; module_id--) {
        mod_exit = g_aiao_mod_map[module_id].mod_exit;
        if (mod_exit == HI_NULL) {
            continue;
        }

        mod_exit();
    }

    ret = osal_exportfunc_unregister(HI_ID_AIAO);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(osal_exportfunc_unregister, ret);
    }

    HI_LOG_INFO(" **** aiao_drv_mod_exit OK  **** \n");
}

#ifdef MODULE
module_init(aiao_drv_mod_init);
module_exit(aiao_drv_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
