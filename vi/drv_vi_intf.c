/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drv vi intf
 * Author: sdk
 * Create: 2019-12-14
 */

#include "hi_debug.h"
#include "hi_drv_dev.h"
#include "hi_drv_vi.h"
#include "drv_vi_ioctl.h"

#include "vi_comm.h"
#include "vi_type.h"

#include "drv_vi_ctrl.h"
#include "drv_vi_intf.h"

typedef hi_s32 (*ioctl_func)(hi_void *arg, hi_void *private_data);
typedef struct {
    hi_u32 cmd;
    ioctl_func ioctl_func_p;
} vi_drv_ioctl_func;

typedef struct {
    vi_instance instance[HI_VI_MAX_NUM];
    hi_void *file_handle[HI_VI_MAX_NUM];
    osal_mutex mutex_lock;
} vi_drv_ctx;

VI_STATIC vi_drv_ctx g_vi_ctx = {0};

VI_STATIC vi_instance *vi_get_instance(hi_handle vi_handle)
{
    hi_u32 i;

    for (i = 0; i < HI_VI_MAX_NUM; i++) {
        if (g_vi_ctx.instance[i].vi_handle == vi_handle) {
            break;
        }
    }

    if (i >= HI_VI_MAX_NUM) {
        return HI_NULL;
    }

    return &g_vi_ctx.instance[i];
}

VI_STATIC hi_s32 vi_intf_create(hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_u32 i;
    vi_instance *instance_p = HI_NULL;
    vi_attr *create_p = (vi_attr *)arg;

    vi_drv_func_enter();

    for (i = 0; i < HI_VI_MAX_NUM; i++) {
        if (g_vi_ctx.file_handle[i] == HI_NULL) {
            break;
        }
    }

    if (i >= HI_VI_MAX_NUM) {
        vi_drv_log_err("no free id, create fail\n");
        return HI_FAILURE;
    }

    instance_p = &g_vi_ctx.instance[i];
    instance_p->vi_handle = HI_INVALID_HANDLE;
    instance_p->vo_handle = HI_INVALID_HANDLE;
    instance_p->vi_status = VI_INSTANCE_STATUS_INIT;

    ret = vi_drv_ctrl_create(instance_p, &create_p->attr);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    g_vi_ctx.file_handle[i] = private_data;
    create_p->vi_handle = instance_p->vi_handle;

    vi_drv_func_exit();

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_intf_destroy(hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_u32 i;
    vi_instance *instance_p = HI_NULL;
    hi_handle *vi_handle = (hi_handle *)arg;

    vi_drv_func_enter();

    for (i = 0; i < HI_VI_MAX_NUM; i++) {
        if ((g_vi_ctx.file_handle[i] == private_data) && (g_vi_ctx.instance[i].vi_handle == *vi_handle)) {
            break;
        }
    }

    if (i >= HI_VI_MAX_NUM) {
        vi_drv_log_err("invald handle(0x%x)\n", *vi_handle);
        return HI_ERR_VI_INVALID_PARA;
    }

    instance_p = &g_vi_ctx.instance[i];

    ret = vi_drv_ctrl_destroy(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    g_vi_ctx.file_handle[i] = HI_NULL;

    vi_drv_func_exit();

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_intf_attach(hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    vi_instance *instance_p = HI_NULL;
    vi_attach_attr *attach_attr_p = (vi_attach_attr *)arg;

    vi_drv_func_enter();

    instance_p = vi_get_instance(attach_attr_p->vi_handle);
    if (instance_p == HI_NULL) {
        vi_drv_log_err("invald handle(0x%x)\n", attach_attr_p->vi_handle);
        return HI_ERR_VI_INVALID_PARA;
    }

    ret = vi_drv_ctrl_attach(instance_p, attach_attr_p->vo_handle);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_intf_detach(hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    vi_instance *instance_p = HI_NULL;
    vi_attach_attr *attach_attr_p = (vi_attach_attr *)arg;

    vi_drv_func_enter();

    instance_p = vi_get_instance(attach_attr_p->vi_handle);
    if (instance_p == HI_NULL) {
        vi_drv_log_err("invald handle(0x%x)\n", attach_attr_p->vi_handle);
        return HI_ERR_VI_INVALID_PARA;
    }

    ret = vi_drv_ctrl_detach(instance_p, attach_attr_p->vo_handle);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_intf_start(hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    vi_instance *instance_p = HI_NULL;
    hi_handle *vi_handle = (hi_handle *)arg;

    vi_drv_func_enter();

    instance_p = vi_get_instance(*vi_handle);
    if (instance_p == HI_NULL) {
        vi_drv_log_err("invald handle(0x%x)\n", *vi_handle);
        return HI_ERR_VI_INVALID_PARA;
    }

    ret = vi_drv_ctrl_start(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_intf_stop(hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    vi_instance *instance_p = HI_NULL;
    hi_handle *vi_handle = (hi_handle *)arg;

    vi_drv_func_enter();

    instance_p = vi_get_instance(*vi_handle);
    if (instance_p == HI_NULL) {
        vi_drv_log_err("invald handle(0x%x)\n", *vi_handle);
        return HI_ERR_VI_INVALID_PARA;
    }

    ret = vi_drv_ctrl_stop(instance_p);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_intf_acquire_frame(hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    vi_instance *instance_p = HI_NULL;
    vi_path_info *path_info_p = (vi_path_info *)arg;

    instance_p = vi_get_instance(path_info_p->vi_handle);
    if (instance_p == HI_NULL) {
        vi_drv_log_err("invald handle(0x%x)\n", path_info_p->vi_handle);
        return HI_ERR_VI_INVALID_PARA;
    }

    ret = vi_drv_ctrl_acquire_frame(instance_p, &path_info_p->video_frame);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_intf_release_frame(hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    vi_instance *instance_p = HI_NULL;
    vi_path_info *path_info_p = (vi_path_info *)arg;

    instance_p = vi_get_instance(path_info_p->vi_handle);
    if (instance_p == HI_NULL) {
        vi_drv_log_err("invald handle(0x%x)\n", path_info_p->vi_handle);
        return HI_ERR_VI_INVALID_PARA;
    }

    ret = vi_drv_ctrl_release_frame(instance_p, &path_info_p->video_frame);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_intf_update_nstd(hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    vi_instance *instance_p = HI_NULL;
    vi_nstd_info *nstd_info_p = (vi_nstd_info *)arg;

    vi_drv_func_enter();

    instance_p = vi_get_instance(nstd_info_p->vi_handle);
    if (instance_p == HI_NULL) {
        vi_drv_log_err("invald handle(0x%x)\n", nstd_info_p->vi_handle);
        return HI_ERR_VI_INVALID_PARA;
    }

    ret = vi_drv_ctrl_update_nstd_info(instance_p, nstd_info_p->info);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_intf_set_quantization_range(hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    vi_instance *instance_p = HI_NULL;
    vi_src_quantization_range *range_p = (vi_src_quantization_range *)arg;

    vi_drv_func_enter();

    instance_p = vi_get_instance(range_p->vi_handle);
    if (instance_p == HI_NULL) {
        vi_drv_log_err("invald handle(0x%x)\n", range_p->vi_handle);
        return HI_ERR_VI_INVALID_PARA;
    }

    ret = vi_drv_ctrl_set_src_quantization_range(instance_p, range_p->range);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_intf_set_config(hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    vi_instance *instance_p = HI_NULL;
    vi_config *cfg = (vi_config *)arg;

    vi_drv_func_enter();

    instance_p = vi_get_instance(cfg->vi_handle);
    if (instance_p == HI_NULL) {
        vi_drv_log_err("invald handle(0x%x)\n", cfg->vi_handle);
        return HI_ERR_VI_INVALID_PARA;
    }

    ret = vi_drv_ctrl_set_config(instance_p, &cfg->config);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    vi_drv_func_exit();

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_intf_get_config(hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    vi_instance *instance_p = HI_NULL;
    vi_config *cfg = (vi_config *)arg;

    instance_p = vi_get_instance(cfg->vi_handle);
    if (instance_p == HI_NULL) {
        vi_drv_log_err("invald handle(0x%x)\n", cfg->vi_handle);
        return HI_ERR_VI_INVALID_PARA;
    }

    ret = vi_drv_ctrl_get_config(instance_p, &cfg->config);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_intf_get_attr(hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    vi_instance *instance_p = HI_NULL;
    vi_attr *attr = (vi_attr *)arg;

    instance_p = vi_get_instance(attr->vi_handle);
    if (instance_p == HI_NULL) {
        vi_drv_log_err("invald handle(0x%x)\n", attr->vi_handle);
        return HI_ERR_VI_INVALID_PARA;
    }

    ret = vi_drv_ctrl_get_attr(instance_p, &attr->attr);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_intf_path_updata_dbg_attr(hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    vi_instance *instance_p = HI_NULL;
    vi_path_dbg_info *dbg_info = (vi_path_dbg_info *)arg;

    instance_p = vi_get_instance(dbg_info->vi_handle);
    if (instance_p == HI_NULL) {
        vi_drv_log_err("invald handle(0x%x)\n", dbg_info->vi_handle);
        return HI_ERR_VI_INVALID_PARA;
    }

    ret = vi_drv_ctrl_updata_dbg_info(instance_p, &dbg_info->dbg_info);
    if (ret != HI_SUCCESS) {
        vi_drv_log_err("err:0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}
/* clang-format off */
VI_STATIC  vi_drv_ioctl_func g_vi_ioctl_info[] = {
    { VI_CMD_ACQUIRE_FRAME,                 vi_intf_acquire_frame },
    { VI_CMD_RELEASE_FRAME,                 vi_intf_release_frame },
    { VI_CMD_UPDATA_DBG_INFO,               vi_intf_path_updata_dbg_attr },

    { VI_CMD_CREATE,                        vi_intf_create },
    { VI_CMD_DESTROY,                       vi_intf_destroy },
    { VI_CMD_ATTACH,                        vi_intf_attach },
    { VI_CMD_DETACH,                        vi_intf_detach },
    { VI_CMD_START,                         vi_intf_start },
    { VI_CMD_STOP,                          vi_intf_stop },

    { VI_CMD_UPDATE_NSTD,                   vi_intf_update_nstd },
    { VI_CMD_SET_SRC_QUANTIZATION_RANGE,    vi_intf_set_quantization_range },

    { VI_CMD_SET_CONFIG,                    vi_intf_set_config},
    { VI_CMD_GET_CONFIG,                    vi_intf_get_config},
    { VI_CMD_GET_ATTR,                      vi_intf_get_attr },
};
/* clang-format on */
VI_STATIC hi_s32 vi_intf_process_cmd(unsigned int cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_u32 index;
    hi_u32 max_func_index = sizeof(g_vi_ioctl_info) / sizeof(vi_drv_ioctl_func);

    if (arg == HI_NULL || private_data == HI_NULL) {
        vi_drv_log_err("PTR is NULL.\n");
        return HI_ERR_VI_NULL_PTR;
    }

    ret = osal_mutex_lock_interruptible(&g_vi_ctx.mutex_lock);
    if (ret != 0) {
        return -ERESTARTSYS;
    }

    for (index = 0; index < max_func_index; index++) {
        if (cmd == g_vi_ioctl_info[index].cmd) {
            ret = g_vi_ioctl_info[index].ioctl_func_p(arg, private_data);
            break;
        }
    }

    if (index == max_func_index) {
        vi_drv_log_err("vi command not found, cmd = 0x%x\n", cmd);
        ret = HI_FAILURE;
    }

    osal_mutex_unlock(&g_vi_ctx.mutex_lock);

    return ret;
}
/* clang-format off */
VI_STATIC  osal_ioctl_cmd g_vi_cmd_list[] = {
    { VI_CMD_ACQUIRE_FRAME,                 vi_intf_process_cmd },
    { VI_CMD_RELEASE_FRAME,                 vi_intf_process_cmd },
    { VI_CMD_UPDATA_DBG_INFO,               vi_intf_process_cmd },

    { VI_CMD_CREATE,                        vi_intf_process_cmd },
    { VI_CMD_DESTROY,                       vi_intf_process_cmd },
    { VI_CMD_ATTACH,                        vi_intf_process_cmd },
    { VI_CMD_DETACH,                        vi_intf_process_cmd },
    { VI_CMD_START,                         vi_intf_process_cmd },
    { VI_CMD_STOP,                          vi_intf_process_cmd },

    { VI_CMD_UPDATE_NSTD,                   vi_intf_process_cmd },
    { VI_CMD_SET_SRC_QUANTIZATION_RANGE,    vi_intf_process_cmd },

    { VI_CMD_SET_CONFIG,                    vi_intf_process_cmd},
    { VI_CMD_GET_CONFIG,                    vi_intf_process_cmd},
    { VI_CMD_GET_ATTR,                      vi_intf_process_cmd },
};
/* clang-format on */
VI_STATIC hi_s32 vi_drv_open(hi_void *private_data)
{
    hi_s32 ret;
    hi_u32 i;

    vi_drv_func_enter();

    if (private_data == HI_NULL) {
        vi_drv_log_err("PTR is NULL.\n");
        return HI_ERR_VI_NULL_PTR;
    }

    ret = osal_mutex_lock_interruptible(&g_vi_ctx.mutex_lock);
    if (ret != 0) {
        vi_drv_log_err("%s: osal_mutex_lock_interruptible err!\n", __func__);
        return -ERESTARTSYS;
    }

    for (i = 0; i < HI_VI_MAX_NUM; i++) {
        if (g_vi_ctx.file_handle[i] == HI_NULL) {
            break;
        }
    }

    if (i >= HI_VI_MAX_NUM) {
        vi_drv_log_err("no free id, open fail\n");
        ret = HI_FAILURE;
    } else {
        ret = HI_SUCCESS;
    }

    osal_mutex_unlock(&g_vi_ctx.mutex_lock);

    vi_drv_func_exit();

    return ret;
}

VI_STATIC hi_s32 vi_drv_close(hi_void *private_data)
{
    hi_s32 ret;
    hi_u32 i;

    vi_drv_func_enter();

    if (private_data == HI_NULL) {
        vi_drv_log_err("PTR is NULL.\n");
        return HI_ERR_VI_NULL_PTR;
    }

    ret = osal_mutex_lock_interruptible(&g_vi_ctx.mutex_lock);
    if (ret != 0) {
        vi_drv_log_err("%s: osal_mutex_lock_interruptible err!\n", __func__);
        return -ERESTARTSYS;
    }

    for (i = 0; i < HI_VI_MAX_NUM; i++) {
        if (g_vi_ctx.file_handle[i] == private_data) {
            ret = vi_drv_ctrl_close_path(&g_vi_ctx.instance[i]);
            if (ret != HI_SUCCESS) {
                vi_drv_log_warn("close path ret = %x\n", ret);
            }

            g_vi_ctx.file_handle[i] = HI_NULL;
        }
    }

    osal_mutex_unlock(&g_vi_ctx.mutex_lock);

    vi_drv_func_exit();

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_drv_poll(osal_poll *table, hi_void *private_data)
{
    hi_u32 i;
    hi_u32 poll_mask = 0;
    vi_instance *instance_p = HI_NULL;

    for (i = 0; i < HI_VI_MAX_NUM; i++) {
        if (g_vi_ctx.file_handle[i] == private_data) {
            instance_p = &g_vi_ctx.instance[i];
            if (vi_drv_ctrl_poll(instance_p) == HI_SUCCESS) {
                poll_mask |= OSAL_POLLIN | OSAL_POLLRDNORM;
            }
            break;
        }
    }

    if (instance_p != HI_NULL) {
        osal_poll_wait(table, &instance_p->poll_wait_queue);
    } else {
        poll_mask |= OSAL_POLLERR;
    }

    return poll_mask;
}

VI_STATIC hi_s32 vi_drv_suspend(hi_void *private_data)
{
    hi_s32 ret;

    ret = vi_drv_ctrl_suspend();
    if (ret != HI_SUCCESS) {
        osal_printk("VI suspend failed!\n");
        return ret;
    }
    osal_printk("VI suspend success!\n");

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_drv_resume(hi_void *private_data)
{
    hi_s32 ret;

    ret = vi_drv_ctrl_resume();
    if (ret != HI_SUCCESS) {
        osal_printk("VI resume failed!\n");
        return ret;
    }
    osal_printk("VI resume success!\n");

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_drv_lowpower_enter(hi_void *private_data)
{
    hi_s32 ret;

    ret = vi_drv_ctrl_suspend();
    if (ret != HI_SUCCESS) {
        osal_printk("VI suspend failed!\n");
        return ret;
    }
    osal_printk("VI lowpower enter success!\n");

    return HI_SUCCESS;
}

VI_STATIC hi_s32 vi_drv_lowpower_exit(hi_void *private_data)
{
    hi_s32 ret;

    ret = vi_drv_ctrl_resume();
    if (ret != HI_SUCCESS) {
        osal_printk("VI resume failed!\n");
        return ret;
    }
    osal_printk("VI lowpower exit success!\n");

    return HI_SUCCESS;
}

osal_pmops g_vi_pmops = {
    .pm_suspend = vi_drv_suspend,
    .pm_resume = vi_drv_resume,
    .pm_lowpower_enter = vi_drv_lowpower_enter,
    .pm_lowpower_exit = vi_drv_lowpower_exit,
    .pm_poweroff = NULL,
    .private_data = NULL,
};

osal_fileops g_vi_fileops = {
    .open = vi_drv_open,
    .read = NULL,
    .write = NULL,
    .llseek = NULL,
    .release = vi_drv_close,
    .poll = vi_drv_poll,
    .mmap = NULL,
    .cmd_list = NULL, /* default NULL, set in vi_drv_intf_init */
    .cmd_cnt = 0,     /* default 0, set in vi_drv_intf_init */
};

osal_dev g_vi_device = {
    .name = HI_DEV_VI_NAME,
    .minor = HI_DEV_VI_MINOR,
    .fops = &g_vi_fileops,
    .pmops = &g_vi_pmops,
};

hi_s32 vi_drv_intf_init(hi_void)
{
    hi_s32 ret;
    hi_u32 i;

    g_vi_fileops.cmd_list = g_vi_cmd_list;
    g_vi_fileops.cmd_cnt = sizeof(g_vi_cmd_list) / sizeof(osal_ioctl_cmd);

    for (i = 0; i < HI_VI_MAX_NUM; i++) {
        g_vi_ctx.file_handle[i] = HI_NULL;
        g_vi_ctx.instance[i].vi_handle = HI_INVALID_HANDLE;
        g_vi_ctx.instance[i].vo_handle = HI_INVALID_HANDLE;
        g_vi_ctx.instance[i].vi_status = VI_INSTANCE_STATUS_INIT;
    }

    ret = osal_mutex_init(&g_vi_ctx.mutex_lock);
    if (ret < 0) {
        vi_drv_log_err("osal_mutex_init failed\n");
        return HI_FAILURE;
    }

    ret = vi_drv_ctrl_init();
    if (ret != HI_SUCCESS) {
        vi_drv_log_fatal("vi_drv_ctrl_init failed.\n");
        osal_mutex_destory(&g_vi_ctx.mutex_lock);
        return HI_FAILURE;
    }

    ret = osal_dev_register(&g_vi_device);
    if (ret != HI_SUCCESS) {
        vi_drv_log_fatal("register VI failed.\n");
        vi_drv_ctrl_deinit();
        osal_mutex_destory(&g_vi_ctx.mutex_lock);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 vi_drv_intf_deinit(hi_void)
{
    osal_dev_unregister(&g_vi_device);

    vi_drv_ctrl_deinit();

    osal_mutex_destory(&g_vi_ctx.mutex_lock);

    return HI_SUCCESS;
}
