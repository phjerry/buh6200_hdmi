#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/module.h>

#include "drv_vdec_sdk.h"
#include "drv_vdec_ioctl.h"
#include "drv_vdec_proc.h"
#include "drv_vdec.h"
#include "vdec_priv_type.h"
#include "drv_vdec_intf.h"

#define VDEC_DEV_MAX_CMD_NUM    18

typedef hi_s32 (*fn_ioctl_handler)(struct file *fd, unsigned long arg);
typedef struct {
    hi_u32 cmd_id;
    fn_ioctl_handler func;
} vdec_dev_cmd_node;

typedef struct {
    hi_bool setup;
    dev_t no;
    struct cdev cdev;
    struct device *dev;
    struct class *cls;
} vdec_dev;

typedef struct {
    atomic_t open_cnt;
    vdec_dev device;
} vdec_dev_entry;

static vdec_dev_entry g_vdec_entry = {0};

static inline vdec_dev_entry* vdec_dev_get_entry(hi_void)
{
    return &g_vdec_entry;
}

static hi_s32 vdec_dev_create(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_create param;
    hi_vdec_opt_destroy option;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_create));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_create(&param.chan_id, &param.option, filp, sizeof(struct file));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec create fail!\n");
        return HI_FAILURE;
    }

    ret = copy_to_user(UINT_2_PTR(arg), &param, sizeof(vdec_ioctl_create));
    if (ret != HI_SUCCESS) {
        (hi_void)drv_vdec_destroy(param.chan_id, &option);
        VDEC_PRINT_ERR("copy to user fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_destroy(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_destroy param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_destroy));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_destroy(param.chan_id, &param.option);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec destroy fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_start(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_start param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_start));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_start(param.chan_id, &param.option);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec start fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_stop(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_stop param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_stop));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_stop(param.chan_id, &param.option);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec stop fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_reset(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_reset param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_reset));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_reset(param.chan_id, &param.option);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec reset fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_flush(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_flush param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_flush));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_flush(param.chan_id, &param.option);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec flush fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_decode(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_decode param;
    drv_vdec_decode_param dec_param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_decode));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    dec_param.is_user_space = HI_TRUE;
    dec_param.frame = &param.frame;
    dec_param.stream = &param.stream;
    dec_param.frame_size = sizeof(param.frame);
    dec_param.ext_frm_info = &param.ext_info;
    dec_param.time_out = param.option.time_out;

    ret = drv_vdec_decode(param.chan_id, &dec_param);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec decode fail!\n");
        return HI_FAILURE;
    }

    ret = copy_to_user(UINT_2_PTR(arg), &param, sizeof(vdec_ioctl_decode));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy to user fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_get_attr(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_get_attr param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_get_attr));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_get_attr(param.chan_id, &param.attr);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec get attr fail!\n");
        return HI_FAILURE;
    }

    ret = copy_to_user(UINT_2_PTR(arg), &param, sizeof(vdec_ioctl_get_attr));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy to user fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_set_attr(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_set_attr param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_set_attr));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_set_attr(param.chan_id, param.vdec_handle, &param.attr);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec set attr fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_register_event(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_register param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_register));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_register_event(param.chan_id, param.event_map);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec register event fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_listen_event(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_listen param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_listen));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_listen_event(param.chan_id, &param.option, &param.event);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec listen event fail!\n");
        return HI_FAILURE;
    }

    ret = copy_to_user(UINT_2_PTR(arg), &param, sizeof(vdec_ioctl_listen));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy to user fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_queue_stream(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_que_stream param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_que_stream));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_queue_stream(param.chan_id, &param.option, &param.stream);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_DBG("vdec queue stream fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_dequeue_stream(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_deq_stream param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_deq_stream));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_dequeue_stream(param.chan_id, &param.option, &param.stream);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_DBG("vdec dequeue stream fail!\n");
        return HI_FAILURE;
    }

    ret = copy_to_user(UINT_2_PTR(arg), &param, sizeof(vdec_ioctl_deq_stream));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy to user fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_acq_frame(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_acq_frame param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_acq_frame));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_acquire_frame(param.chan_id, &param.option, &param.frame, sizeof(param.frame), &param.ext_frm_info);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_INFO("vdec acquire frame fail!\n");
        return HI_FAILURE;
    }

    ret = copy_to_user(UINT_2_PTR(arg), &param, sizeof(vdec_ioctl_acq_frame));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy to user fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_rls_frame(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_rel_frame param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_rel_frame));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_release_frame(param.chan_id, &param.option, &param.frame, sizeof(param.frame));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec release frame fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_get_param(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_param param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_param));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_get_param(param.chan_id, param.param_id, param.param, param.param_size);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec get param %d fail!\n", param.param_id);
        return HI_FAILURE;
    }

    ret = copy_to_user(UINT_2_PTR(arg), &param, sizeof(vdec_ioctl_param));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy to user fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_set_param(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_param param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_param));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_set_param(param.chan_id, param.param_id, param.param, param.param_size);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec set param %d fail!\n", param.param_id);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_command(struct file *filp, unsigned long arg)
{
    hi_s32 ret;
    vdec_ioctl_command param;

    ret = copy_from_user(&param, UINT_2_PTR(arg), sizeof(vdec_ioctl_command));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy from user fail!\n");
        return HI_FAILURE;
    }

    ret = drv_vdec_command(param.chan_id, param.cmd_id, param.param, param.param_size);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = copy_to_user(UINT_2_PTR(arg), &param, sizeof(vdec_ioctl_command));
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("copy to user fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static long vdec_dev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    hi_s32 ret = HI_FAILURE;

    VDEC_ASSERT_RET(filp != HI_NULL);
    VDEC_ASSERT_RET(arg != 0);

    switch (cmd) {
        case UMAPC_VDEC_CREATE:
            ret = vdec_dev_create(filp, arg);
            break;
        case UMAPC_VDEC_DESTROY:
            ret = vdec_dev_destroy(filp, arg);
            break;
        case UMAPC_VDEC_START:
            ret = vdec_dev_start(filp, arg);
            break;
        case UMAPC_VDEC_STOP:
            ret = vdec_dev_stop(filp, arg);
            break;
        case UMAPC_VDEC_RESET:
            ret = vdec_dev_reset(filp, arg);
            break;
        case UMAPC_VDEC_FLUSH:
            ret = vdec_dev_flush(filp, arg);
            break;
        case UMAPC_VDEC_DECODE:
            ret = vdec_dev_decode(filp, arg);
            break;
        case UMAPC_VDEC_GET_ATTR:
            ret = vdec_dev_get_attr(filp, arg);
            break;
        case UMAPC_VDEC_SET_ATTR:
            ret = vdec_dev_set_attr(filp, arg);
            break;
        case UMAPC_VDEC_REGISTER_EVENT:
            ret = vdec_dev_register_event(filp, arg);
            break;
        case UMAPC_VDEC_LISTEN_EVENT:
            ret = vdec_dev_listen_event(filp, arg);
            break;
        case UMAPC_VDEC_QUEUE_STREAM:
            ret = vdec_dev_queue_stream(filp, arg);
            break;
        case UMAPC_VDEC_DEQUEUE_STREAM:
            ret = vdec_dev_dequeue_stream(filp, arg);
            break;
        case UMAPC_VDEC_ACQUIRE_FRAME:
            ret = vdec_dev_acq_frame(filp, arg);
            break;
        case UMAPC_VDEC_RELEASE_FRAME:
            ret = vdec_dev_rls_frame(filp, arg);
            break;
        case UMAPC_VDEC_GET_PARAM:
            ret = vdec_dev_get_param(filp, arg);
            break;
        case UMAPC_VDEC_SET_PARAM:
            ret = vdec_dev_set_param(filp, arg);
            break;
        case UMAPC_VDEC_COMMAND:
            ret = vdec_dev_command(filp, arg);
            break;
        default:
            VDEC_PRINT_ERR("Unsuppot ioctl command 0x%x\n", cmd);
            break;
    }

    return ret;
}

static hi_void vdec_dev_read_proc(hi_void *pf)
{
    vdec_dev_entry *entry = vdec_dev_get_entry();
    vdec_dev *device = &entry->device;

    PROC_PRINT(pf, VP_HEAD, "DEV");
    PROC_PRINT(pf, VP_D_D,  "open count", atomic_read(&entry->open_cnt), "setup", device->setup);
    PROC_PRINT(pf, "\n");
}

static hi_s32 vdec_dev_proc_read(hi_void *pf, hi_void *data)
{
    vdec_dev_read_proc(pf);

    vdec_proc_read(pf);

    return HI_SUCCESS;
}

hi_s32 vdec_dev_proc_init(hi_void)
{
    hi_s32 ret;
    hi_u32 cmd_cnt;
    osal_proc_cmd *cmd_list = HI_NULL;

    ret = vdec_proc_get_cmd((hi_void **)&cmd_list, &cmd_cnt);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec_proc_get_cmd fail!\n");
        return HI_FAILURE;
    }

    ret = vdec_drv_create_proc(VDEC_PROC_NAME, &vdec_dev_proc_read, cmd_list, cmd_cnt);
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("create vdec proc fail!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void vdec_dev_proc_deinit(hi_void)
{
    vdec_drv_destroy_proc(VDEC_PROC_NAME);
}

static hi_s32 vdec_dev_open(struct inode *node, struct file *filp)
{
    hi_s32 ret;
    vdec_dev_entry *entry = vdec_dev_get_entry();

    if (atomic_inc_return(&entry->open_cnt) == 1) {
        ret = drv_vdec_open();
        if (ret != HI_SUCCESS) {
            VDEC_PRINT_ERR("vdec open fail!\n");
            atomic_dec(&entry->open_cnt);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_close(struct inode *node, struct file *filp)
{
    vdec_dev_entry *entry = vdec_dev_get_entry();

    drv_vdec_release(filp, sizeof(struct file));

    if (atomic_dec_return(&entry->open_cnt) == 0) {
        drv_vdec_close();
    }

    return HI_SUCCESS;
}

static struct file_operations g_vdec_ops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = vdec_dev_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl   = vdec_dev_ioctl,
#endif
    .open           = vdec_dev_open,
    .release        = vdec_dev_close,
};

static hi_s32 vdec_dev_setup(vdec_dev *device)
{
    hi_s32 ret;
    struct device *dev = HI_NULL;
    struct class *cls = HI_NULL;

    cls = class_create(THIS_MODULE, "vdec_class");
    if (IS_ERR(cls)) {
        return HI_FAILURE;
    }

    ret = alloc_chrdev_region(&device->no, 0, 1, "hisi_vdec");
    if (ret != 0) {
        goto destroy_class;
    }

    dev = device_create(cls, HI_NULL, device->no, HI_NULL, VDEC_DEV_NAME);
    if (IS_ERR(dev)) {
        goto unregister_region;
    }

    cdev_init(&device->cdev, &g_vdec_ops);
    device->cdev.owner = THIS_MODULE;
    device->cdev.ops = &g_vdec_ops;
    ret = cdev_add(&device->cdev, device->no, 1);
    if (ret < 0) {
        goto destroy_device;
    }

    device->cls = cls;
    device->dev = dev;
    device->setup = HI_TRUE;

    return HI_SUCCESS;

destroy_device:
    device_destroy(cls, device->no);
unregister_region:
    unregister_chrdev_region(device->no, 1);
destroy_class:
    class_destroy(cls);

    return HI_FAILURE;
}

static hi_s32 vdec_dev_clean(vdec_dev *device)
{
    if (device->setup == HI_FALSE) {
        return HI_SUCCESS;
    }

    device->setup = HI_FALSE;

    if (device->cls == HI_NULL) {
        return HI_FAILURE;
    }

    cdev_del(&device->cdev);
    device_destroy(device->cls, device->no);
    unregister_chrdev_region(device->no, 1);
    class_destroy(device->cls);

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_probe(struct platform_device *plt_dev)
{
    hi_s32 ret;
    vdec_dev_entry *entry = vdec_dev_get_entry();
    vdec_dev *device = &entry->device;

    VDEC_PRINT_INFO("Prepare to probe vdec.\n");

    platform_set_drvdata(plt_dev, HI_NULL);

    ret = vdec_dev_setup(device);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    device->dev = &plt_dev->dev;
    platform_set_drvdata(plt_dev, device);

    VDEC_PRINT_INFO("Probe vdec success.\n");

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_remove(struct platform_device *plt_dev)
{
    vdec_dev *device = HI_NULL;

    VDEC_PRINT_INFO("Prepare to remove vdec.\n");

    device = platform_get_drvdata(plt_dev);
    if (IS_ERR(device)) {
        return HI_FAILURE;
    }

    vdec_dev_clean(device);
    platform_set_drvdata(plt_dev, HI_NULL);

    VDEC_PRINT_INFO("Remove vdec success.\n");

    return HI_SUCCESS;
}

static hi_s32 vdec_dev_suspend(struct platform_device *plt_dev, pm_message_t state)
{
    hi_s32 ret;

    ret = drv_vdec_suspend();
    if (ret != HI_SUCCESS) {
        HI_PRINT("vdec suspend err.\n");
    } else {
        HI_PRINT("vdec suspend ok.\n");
    }

    return ret;
}

static hi_s32 vdec_dev_resume(struct platform_device *plt_dev)
{
    hi_s32 ret;

    ret = drv_vdec_resume();
    if (ret != HI_SUCCESS) {
        HI_PRINT("vdec resume err.\n");
    } else {
        HI_PRINT("vdec resume ok.\n");
    }

    return ret;
}

static hi_void vdec_dev_release(struct device *dev)
{
    return;
}

static struct platform_driver g_vdec_driver = {
    .probe   = vdec_dev_probe,
    .remove  = vdec_dev_remove,
    .suspend = vdec_dev_suspend,
    .resume  = vdec_dev_resume,
    .driver  = {
        .name  = VDEC_DEV_NAME,
        .owner = THIS_MODULE,
    },
};

static struct platform_device g_vdec_device = {
    .name = VDEC_DEV_NAME,
    .id = -1,
    .dev = {
        .platform_data = HI_NULL,
        .release = vdec_dev_release,
    },
};

hi_s32 drv_vdec_init_module(hi_void)
{
    hi_s32 ret;

    ret = platform_device_register(&g_vdec_device);
    if (ret < 0) {
        return ret;
    }

    ret = platform_driver_register(&g_vdec_driver);
    if (ret < 0) {
        platform_device_unregister(&g_vdec_device);
        return ret;
    }

    ret = vdec_dev_proc_init();
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec proc create fail!\n");
    }

#ifndef HI_MCE_SUPPORT
    ret = vdec_intf_init();
    if (ret != HI_SUCCESS) {
        platform_device_unregister(&g_vdec_device);
        platform_driver_unregister(&g_vdec_driver);
        VDEC_PRINT_ERR("vdec_intf_init fail!\n");
        return ret;
    }

    drv_vdec_init();
#endif
    HI_PRINT("Load hi_vdec.ko success.\t(%s)\n", VERSION_STRING);

    return HI_SUCCESS;
}

hi_void drv_vdec_exit_module(hi_void)
{
    platform_driver_unregister(&g_vdec_driver);
    platform_device_unregister(&g_vdec_device);

#ifndef HI_MCE_SUPPORT
    drv_vdec_deinit();

    vdec_intf_exit();
#endif
    vdec_dev_proc_deinit();

    HI_PRINT("Unload hi_vdec.ko success.\n");
}

#ifdef MODULE
module_init(drv_vdec_init_module);
module_exit(drv_vdec_exit_module);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");
