/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: tianlai adac codec driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_osal.h"

#include "hal_tianlai_adac.h"
#include "hal_tianlai_adac_sv100.h"

#include "audio_util.h"

static hi_u32 adac_get_version(hi_void)
{
#if defined(HI_TIANLAI_V500)
    return ADAC_TIANLAI_V500;
#elif defined(HI_TIANLAI_V600) || defined(HI_TIANLAI_V700)
    if (autil_is_chip_mv300() == HI_FALSE) {
        return ADAC_TIANLAI_V600;
    } else {
        return ADAC_TIANLAI_V700;
    }
#elif defined(HI_TIANLAI_V730)
    return ADAC_TIANLAI_V730;
#elif defined(HI_TIANLAI_SV100)
    return ADAC_TIANLAI_SV100;
#else
    return ADAC_TIANLAI_NULL;
#endif
}

static adac_platform_driver *adac_match_driver(hi_void)
{
    struct osal_list_head head;
    adac_platform_driver *adac_driver = HI_NULL;

    OSAL_INIT_LIST_HEAD(&head);

    adac_tianlai_sv100_register_driver(&head);

    osal_list_for_each_entry(adac_driver, &head, node) {
        if (adac_driver->version == adac_get_version()) {
            return adac_driver;
        }
    }

    return HI_NULL;
}

#ifdef HI_SND_MUTECTL_SUPPORT
hi_void adac_tianlai_fast_power_enable(hi_bool enable)
{
    adac_platform_driver *adac_driver = adac_match_driver();

    if (adac_driver != HI_NULL && adac_driver->fast_power_enable != HI_NULL) {
        adac_driver->fast_power_enable(enable);
    }
}
#endif

static hi_void adac_init_work_func(osal_workqueue *work)
{
    adac_platform_driver *adac_driver = adac_match_driver();

    if (adac_driver != HI_NULL && adac_driver->init != HI_NULL) {
        adac_driver->init(HI_SAMPLE_RATE_48K, HI_FALSE);
    }
}

static hi_void adac_deinit_work_func(osal_workqueue *work)
{
    adac_platform_driver *adac_driver = adac_match_driver();

    if (adac_driver != HI_NULL && adac_driver->deinit != HI_NULL) {
        adac_driver->deinit(HI_FALSE);
    }
}

static osal_workqueue g_adac_work = { HI_NULL, HI_NULL };

hi_void adac_tianlai_init(hi_u32 sample_rate, hi_bool resume)
{
    adac_tianlai_flush_work();
    osal_workqueue_init(&g_adac_work, adac_init_work_func);
    osal_workqueue_schedule(&g_adac_work);
}

hi_void adac_tianlai_deinit(hi_bool suspend)
{
    adac_tianlai_flush_work();
    osal_workqueue_init(&g_adac_work, adac_deinit_work_func);
    osal_workqueue_schedule(&g_adac_work);
}

hi_void adac_tianlai_flush_work(hi_void)
{
    if ((g_adac_work.work == HI_NULL) || (g_adac_work.handler == HI_NULL)) {
        return;
    }

    osal_workqueue_flush(&g_adac_work);
    osal_workqueue_destroy(&g_adac_work);
    g_adac_work.work = HI_NULL;
    g_adac_work.handler = HI_NULL;
}
