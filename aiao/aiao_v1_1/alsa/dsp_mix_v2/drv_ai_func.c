/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ai alsa
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hal_aiao_common.h"
#include "hal_aiao.h"

#include "hi_drv_ai.h"

#include "drv_ai_private.h"
#include "drv_ai_ioctl.h"

#include "drv_ai_func.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int hi_ai_alsa_update_readptr(int handle, unsigned int *write_pos)
{
    aiao_port_id port = AIAO_PORT_MAX;
    ai_get_aiao_port_id(handle, &port);
    return hal_aiao_alsa_update_rptr(port, HI_NULL, *write_pos);
}

void hi_ai_alsa_query_writepos(int handle, unsigned int *pos)
{
    aiao_port_id port = AIAO_PORT_MAX;
    ai_get_aiao_port_id(handle, &port);
    *pos = hal_aiao_alsa_query_write_pos(port);
}

void hi_ai_alsa_query_readpos(int handle, unsigned int *pos)
{
    aiao_port_id port = AIAO_PORT_MAX;
    ai_get_aiao_port_id(handle, &port);
    *pos = hal_aiao_alsa_query_read_pos(port);
}

int hi_ai_alsa_flush_buffer(int handle)
{
    aiao_port_id port = AIAO_PORT_MAX;
    ai_get_aiao_port_id(handle, &port);
    return hal_aiao_p_alsa_flush(port);
}

int hi_ai_alsa_get_proc_func(aiao_isr_func **fun)
{
    return ai_get_isr_func(fun);
}

int hi_ai_alsa_get_attr(hi_ai_port ai_port, hi_ai_attr *ai_attr)
{
    return hi_drv_ai_get_default_attr(ai_port, ai_attr);
}

int hi_ai_alsa_open(ai_drv_create_param *ai_param, hi_void *file)
{
    int ret;

    ret = hi_drv_ai_init();
    if (ret != HI_SUCCESS) {
        return ret;
    }
    ret = hi_drv_ai_open(file);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = hi_drv_ai_create(ai_param, file);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    return ret;
}

int hi_ai_alsa_set_enable(int handle, hi_bool enable)
{
    return hi_drv_ai_set_enable(handle, enable);
}

int hi_ai_alsa_close(int handle, hi_void *file)
{
    int ret;

    ret = hi_drv_ai_destroy(handle);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = hi_drv_ai_release(file);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    hi_drv_ai_deinit();

    return ret;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
