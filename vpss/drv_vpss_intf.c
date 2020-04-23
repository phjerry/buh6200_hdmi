/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: drv_vpss_intf.c source filevpss_intf
* Author: zhangjunyu
* Create: 2016/06/12
 */

#include "hi_drv_proc.h"
#include "hi_drv_dev.h"
#include "hi_drv_module.h"
#include "hi_drv_vpss.h"
#include "drv_vpss_ext.h"
#include "drv_vpss_ctrl.h"
#include "vpss_comm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

static vpss_export_func g_vpss_export_funcs = {
    .vpss_init = hi_drv_vpss_init,
    .vpss_deinit = hi_drv_vpss_deinit,

    .vpss_get_instance_default_cfg = hi_drv_vpss_get_instance_default_cfg,
    .vpss_create_instance = hi_drv_vpss_create_instance,
    .vpss_destroy_instance = hi_drv_vpss_destroy_instance,
    .vpss_set_instance_cfg = hi_drv_vpss_set_instance_cfg,
    .vpss_get_instance_cfg = hi_drv_vpss_get_instance_cfg,

    .vpss_get_port_default_cfg = hi_drv_vpss_get_port_default_cfg,
    .vpss_create_port = hi_drv_vpss_create_port,
    .vpss_destroy_port = hi_drv_vpss_destroy_port,
    .vpss_get_port_cfg = hi_drv_vpss_get_port_cfg,
    .vpss_set_port_cfg = hi_drv_vpss_set_port_cfg,
    .vpss_enable_port = hi_drv_vpss_enable_port,

    .vpss_send_command = hi_drv_vpss_send_command,

    .vpss_regist_hook_event = hi_drv_vpss_regist_hook_event,
    .vpss_regist_callback = hi_drv_vpss_regist_callback,

    .vpss_queue_frame = hi_drv_vpss_queue_frame,
    .vpss_dequeue_frame = hi_drv_vpss_dequeue_frame,

    .vpss_acquire_frame = hi_drv_vpss_acquire_frame,
    .vpss_release_frame = hi_drv_vpss_release_frame,

    .vpss_suspend = hi_drv_vpss_suspend,
    .vpss_resume = hi_drv_vpss_resume,
};

hi_s32 hi_drv_vpss_init(hi_void)
{
#ifdef __VPSS_UT__
    return vpss_ctrl_init();
#else
    return HI_SUCCESS;
#endif
}

hi_s32 hi_drv_vpss_deinit(hi_void)
{
#ifdef __VPSS_UT__
    return vpss_ctrl_deinit();
#else
    return HI_SUCCESS;
#endif
}

hi_s32 hi_drv_vpss_create_instance(hi_handle *instance_id, hi_drv_vpss_instance_cfg *instance_cfg)
{
    hi_s32 ret;
    vpss_check_null_pointer(instance_id);
    vpss_check_null_pointer(instance_cfg);
    vpss_info_func_enter();
    ret = vpss_ctrl_create_instance(instance_cfg, instance_id);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_destroy_instance(hi_handle instance_id)
{
    hi_s32 ret;
    vpss_info_func_enter();
    ret = vpss_ctrl_destory_instance(instance_id);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_get_instance_default_cfg(hi_drv_vpss_instance_cfg *instance_cfg)
{
    hi_s32 ret;
    vpss_check_null_pointer(instance_cfg);
    vpss_info_func_enter();
    ret = vpss_ctrl_get_default_cfg(instance_cfg);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_set_instance_cfg(hi_handle instance_id, hi_drv_vpss_instance_cfg *instance_cfg)
{
    hi_s32 ret;
    vpss_check_null_pointer(instance_cfg);
    vpss_info_func_enter();
    ret = vpss_ctrl_set_vpss_cfg(instance_id, instance_cfg);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_get_instance_cfg(hi_handle instance_id, hi_drv_vpss_instance_cfg *instance_cfg)
{
    hi_s32 ret;
    vpss_check_null_pointer(instance_cfg);
    vpss_info_func_enter();
    ret = vpss_ctrl_get_vpss_cfg(instance_id, instance_cfg);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_get_port_default_cfg(hi_drv_vpss_port_cfg *port_cfg)
{
    hi_s32 ret;
    vpss_check_null_pointer(port_cfg);
    vpss_info_func_enter();
    ret = vpss_ctrl_get_default_port_cfg(port_cfg);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_create_port(hi_handle instance_id, hi_drv_vpss_port_cfg *port_cfg, hi_handle *port_id)
{
    hi_s32 ret;
    vpss_check_null_pointer(port_cfg);
    vpss_check_null_pointer(port_id);
    vpss_info_func_enter();
    ret = vpss_ctrl_create_port(instance_id, port_cfg, port_id);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_destroy_port(hi_handle port_id)
{
    hi_s32 ret;
    vpss_info_func_enter();
    ret = vpss_ctrl_destroy_port(port_id);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_get_port_cfg(hi_handle port_id, hi_drv_vpss_port_cfg *port_cfg)
{
    hi_s32 ret;
    vpss_check_null_pointer(port_cfg);
    vpss_info_func_enter();
    ret = vpss_ctrl_get_port_cfg(port_id, port_cfg);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_set_port_cfg(hi_handle port_id, hi_drv_vpss_port_cfg *port_cfg)
{
    hi_s32 ret;
    vpss_check_null_pointer(port_cfg);
    vpss_info_func_enter();
    ret = vpss_ctrl_set_port_cfg(port_id, port_cfg);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_enable_port(hi_handle port_id, hi_bool enable)
{
    hi_s32 ret;
    vpss_info_func_enter();
    ret = vpss_ctrl_enable_port(port_id, enable);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_send_command(hi_handle instance_id, hi_drv_vpss_user_command user_command, hi_void *args)
{
    hi_s32 ret;
    vpss_info_func_enter();
    ret = vpss_ctrl_send_command(instance_id, user_command, args);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_regist_hook_event(hi_handle instance_id, hi_handle dst_id, fn_vpss_event_callback event_callback)
{
    hi_s32 ret;
    vpss_info_func_enter();
    ret = vpss_ctrl_regist_hook_event(instance_id, dst_id, event_callback);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_regist_callback(hi_handle h_port, hi_handle h_sink,
    hi_handle h_src, hi_drv_vpss_callback_func *fn_vpss_callback)
{
    hi_s32 ret;

    vpss_check_null_pointer(fn_vpss_callback);
    vpss_info_func_enter();
    ret = vpss_ctrl_regist_callback(h_port, h_sink, h_src, fn_vpss_callback);
    vpss_info_func_exit();

    return ret;
}

hi_s32 hi_drv_vpss_acquire_frame(hi_handle port_id, hi_drv_video_frame *vpss_frame)
{
    hi_s32 ret;
    vpss_check_null_pointer(vpss_frame);
    vpss_info_func_enter();
    ret = vpss_ctrl_acquire_frame(port_id, vpss_frame);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_release_frame(hi_handle port_id, hi_drv_video_frame *vpss_frame)
{
    hi_s32 ret;
    vpss_check_null_pointer(vpss_frame);
    vpss_info_func_enter();
    ret = vpss_ctrl_release_frame(port_id, vpss_frame);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_queue_frame(hi_handle instance_id, hi_drv_video_frame *image)
{
    hi_s32 ret;
    vpss_check_null_pointer(image);
    vpss_info_func_enter();
    ret = vpss_ctrl_queue_frame(instance_id, image);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_dequeue_frame(hi_handle instance_id, hi_drv_video_frame *image)
{
    hi_s32 ret;
    vpss_check_null_pointer(image);
    vpss_info_func_enter();
    ret = vpss_ctrl_dequeue_frame(instance_id, image);
    vpss_info_func_exit();
    return ret;
}

hi_s32 hi_drv_vpss_suspend(hi_void)
{
    osal_printk("vpss suspend start...\n");
    vpss_info_func_enter();
    vpss_ctrl_dev_suspend();
    vpss_info_func_exit();
    osal_printk("vpss suspend finish\n");
    return HI_SUCCESS;
}

hi_s32 hi_drv_vpss_resume(hi_void)
{
    osal_printk("vpss resume start...\n");
    vpss_info_func_enter();
    vpss_ctrl_dev_resume();
    vpss_info_func_exit();
    osal_printk("vpss resume finish\n");
    return HI_SUCCESS;
}

static osal_dev g_vpss_dev = {
    .minor = HI_DEV_VPSS_MINOR,
    .name = HI_DEV_VPSS_NAME,
};

hi_s32 vpss_drv_mod_init(hi_void)
{
    hi_s32 ret;

    vpss_info_func_enter();

    ret = osal_exportfunc_register(HI_ID_VPSS, "HI_VPSS", &g_vpss_export_funcs);
    if (ret != HI_SUCCESS) {
        vpss_fatal("register VPSS failed.\n");
        return HI_FAILURE;
    }

    ret = osal_dev_register(&g_vpss_dev);
    if (ret != HI_SUCCESS) {
        vpss_fatal("register device failed\n");
        osal_exportfunc_unregister(HI_ID_VPSS);
        return HI_FAILURE;
    }

    vpss_ctrl_context_init();

    if (vpss_ctrl_init() != HI_SUCCESS) {
        vpss_ctrl_context_deinit();
        osal_dev_unregister(&g_vpss_dev);
        osal_exportfunc_unregister(HI_ID_VPSS);
        vpss_error("vpss_ctrl_init failed.\n");
        return HI_FAILURE;
    }

    vpss_info_func_exit();
#ifdef MODULE
    osal_printk("Load hi_vpss.ko success.\t(%s)\n", VERSION_STRING);
#endif
    return HI_SUCCESS;
}

hi_void vpss_drv_mod_exit(hi_void)
{
    vpss_info_func_enter();
    vpss_ctrl_deinit();
    osal_dev_unregister(&g_vpss_dev);
    osal_exportfunc_unregister(HI_ID_VPSS);
    vpss_info_func_exit();
#ifdef MODULE
    osal_printk("Unload hi_vpss.ko success.\t(%s)\n", VERSION_STRING);
#endif
}

#ifdef MODULE
module_init(vpss_drv_mod_init);
module_exit(vpss_drv_mod_exit);
#else
EXPORT_SYMBOL(vpss_drv_mod_init);
EXPORT_SYMBOL(vpss_drv_mod_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("HISILICON");

EXPORT_SYMBOL(hi_drv_vpss_init);
EXPORT_SYMBOL(hi_drv_vpss_deinit);
EXPORT_SYMBOL(hi_drv_vpss_get_instance_default_cfg);
EXPORT_SYMBOL(hi_drv_vpss_create_instance);
EXPORT_SYMBOL(hi_drv_vpss_destroy_instance);
EXPORT_SYMBOL(hi_drv_vpss_set_instance_cfg);
EXPORT_SYMBOL(hi_drv_vpss_get_instance_cfg);
EXPORT_SYMBOL(hi_drv_vpss_get_port_default_cfg);
EXPORT_SYMBOL(hi_drv_vpss_create_port);
EXPORT_SYMBOL(hi_drv_vpss_destroy_port);
EXPORT_SYMBOL(hi_drv_vpss_get_port_cfg);
EXPORT_SYMBOL(hi_drv_vpss_set_port_cfg);
EXPORT_SYMBOL(hi_drv_vpss_enable_port);
EXPORT_SYMBOL(hi_drv_vpss_send_command);
EXPORT_SYMBOL(hi_drv_vpss_regist_hook_event);
EXPORT_SYMBOL(hi_drv_vpss_regist_callback);

EXPORT_SYMBOL(hi_drv_vpss_acquire_frame);
EXPORT_SYMBOL(hi_drv_vpss_release_frame);

EXPORT_SYMBOL(hi_drv_vpss_queue_frame);
EXPORT_SYMBOL(hi_drv_vpss_dequeue_frame);

EXPORT_SYMBOL(hi_drv_vpss_suspend);
EXPORT_SYMBOL(hi_drv_vpss_resume);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

