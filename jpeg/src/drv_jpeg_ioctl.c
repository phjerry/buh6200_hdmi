/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: jpeg ioctl implement
 */

#include "drv_ioctl_jpeg.h"
#include "hi_drv_sys.h"
#include "hi_debug.h"
#include "drv_jpeg_dec.h"
#include "drv_jpeg_dev.h"
#include "drv_jpeg_csc.h"
#include "drv_jpeg_mem.h"

#define JPEG_IOCTL_FUNC_ITEM_NUM_MAX 5

typedef hi_s32 (*drv_jpeg_ioctl_func)(hi_void *arg);

typedef struct {
    hi_u32 cmd;
    drv_jpeg_ioctl_func ioctl_func;
} jpeg_ioctl_func_item;

static jpeg_ioctl_func_item g_drv_jpeg_ctl_func[JPEG_IOCTL_FUNC_ITEM_NUM_MAX] = {
    { DRV_JPEG_CMD_DECODE, drv_jpeg_decode },           { DRV_JPEG_HDEC_CMD_ALLOC_MEM, drv_jpeg_hdec_alloc_mem },
    { DRV_JPEG_HDEC_CMD_DECODE, drv_jpeg_hdec_decode }, { DRV_JPEG_HDEC_CMD_FREE_MEM, drv_jpeg_hdec_free_mem },
    { JPEG_IOCTL_FUNC_ITEM_NUM_MAX - 1, NULL },
};

static hi_slong jpeg_ioctl(struct file *ffile, hi_u32 cmd, hi_void *arg)
{
    hi_s32 ret;
    hi_u8 jpeg_cmd = _IOC_NR(cmd);
    if (jpeg_cmd >= JPEG_IOCTL_FUNC_ITEM_NUM_MAX) {
        HI_PRINT("[module-jpeg][err] : %s %d ->failure jpeg_cmd:%d\n", __FUNCTION__, __LINE__, jpeg_cmd);
        return HI_FAILURE;
    }

    if (g_drv_jpeg_ctl_func[jpeg_cmd].ioctl_func == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d ->failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    if (cmd != g_drv_jpeg_ctl_func[jpeg_cmd].cmd) {
        HI_PRINT("[module-jpeg][err] : %s %d ->failure cmd:%x jpeg_cmd:%d jpeg_ctl_func.cmd:%x\n",
                 __FUNCTION__, __LINE__, cmd, jpeg_cmd, g_drv_jpeg_ctl_func[jpeg_cmd].cmd);
        return HI_FAILURE;
    }

    ret = g_drv_jpeg_ctl_func[jpeg_cmd].ioctl_func(arg);
    return ret;
}

hi_slong drv_jpeg_dev_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg)
{
    hi_s32 ret;

    if (ffile == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    if (ffile->f_path.dentry == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    ret = hi_drv_user_copy(ffile, cmd, arg, jpeg_ioctl);

    return (hi_slong)ret;
}

#ifdef CONFIG_COMPAT
static hi_slong jpeg_ioctl_compat(struct file *ffile, hi_u32 cmd, hi_void *arg)
{
    return jpeg_ioctl(ffile, cmd, arg);
}

hi_slong drv_jpeg_dev_compat_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg)
{
    hi_s32 ret;
    if (ffile == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    if (ffile->f_path.dentry == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }

    ret = hi_drv_user_copy(ffile, cmd, arg, jpeg_ioctl_compat);
    return (hi_slong)ret;
}
#endif
