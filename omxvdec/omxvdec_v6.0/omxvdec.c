/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: decoder
* Author: sdk
* Create: 2019-04-22
 */

#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
#include <linux/sched/clock.h>
#endif
#include "omxvdec.h"
#include "channel.h"
#include "drv_omxvdec_ext.h"
#include "decoder.h"
#include "linux/hisilicon/securec.h"
#include "hi_vdec_type.h"
#include "hi_osal.h"
#include "dec_sync.h"

/* ================ EXTERN VALUE ================ */
extern hi_u32 g_client_reserved_num;
extern hi_u32 g_seg_size;         // (M)
extern hi_bool g_raw_move_enable;
extern hi_bool g_fast_output_mode;
extern hi_u32 g_max_pts_num;

/* =================== MACRO ==================== */
#if (HI_PROC_SUPPORT == 1)
#define DBG_CMD_NUM 2
#define DBG_CMD_LEN 48

#define DBG_CMD_START "start"
#define DBG_CMD_STOP  "stop"
#define DBG_CMD_ON    "on"
#define DBG_CMD_OFF   "off"

#define DBG_CMD_SET_PRINT   "set_print"
#define DBG_CMD_SET_DISPNUM "set_dispnum"
#define DBG_CMD_SET_SEGSIZE "set_segsize"
#define DBG_CMD_SET_ESSIZE  "set_essize"

#define DBG_CMD_SAVE_RAW  "save_raw"
#define DBG_CMD_SAVE_YUV  "save_yuv"
#define DBG_CMD_SAVE_IMG  "save_img"
#define DBG_CMD_SAVE_NAME "save_name"
#define DBG_CMD_SAVE_PATH "save_path"

#define DBG_CMD_TURN_FRAMERATE  "turn_framerate"
#define DBG_CMD_TURN_RAWMOVE    "turn_rawmove"
#define DBG_CMD_TURN_DEI        "turn_dei"
#define DBG_CMD_TURN_FASTOUTPUT "turn_fastout"
#define DBG_CMD_MAP_FRAME       "map_frm"
#define DBG_CMD_HELP            "help"
#endif

#define OSAL_PROC_NAME_LEN         16

typedef struct omxvdec_osal_proc_cmd_ {
    char name[OSAL_PROC_NAME_LEN];
    int (*handler)(hi_char *str, hi_s32 len);
} omxvdec_osal_proc_cmd;


/* ================ GLOBAL VALUE ================ */
hi_u32 g_trace_option = 0;
hi_bool g_save_raw_enable = HI_FALSE;
hi_bool g_save_yuv_enable = HI_FALSE;
hi_bool g_save_src_yuv_enable = HI_FALSE;
hi_bool g_map_frm_enable = HI_FALSE;
hi_u32 g_save_raw_chan_num = -1;
hi_u32 g_save_yuv_chan_num = -1;
hi_u32 g_save_src_yuv_chan_num = -1;
struct file *g_save_raw_file = HI_NULL;
struct file *g_save_src_yuv_file = HI_NULL;

#if (HI_PROC_SUPPORT == 1)
hi_char g_save_path[PATH_LEN] = { '/', 'm', 'n', 't', '\0' };
hi_char g_save_name[NAME_LEN] = { 'o', 'm', 'x', '\0' };
hi_u32 g_save_num = 0;

#endif

hi_u32 g_es_size = 0; /* 512 and 1024 are resolution */
hi_u32 g_low_delay_count_frame = 100;
extern hi_bool g_fast_output_mode;
OMXVDEC_ENTRY *g_omx_vdec = HI_NULL;
OMXVDEC_FUNC g_st_omx_func = { HI_NULL };
omx_export_func g_omx_export_funcs = {
    .pfn_omx_acquire_stream = decoder_get_stream_ex,
    .pfn_omx_release_stream = decoder_release_stream_ex,
    .pfn_omx_vdec_event_report = channel_vdec_report,
};

/* ================ STATIC VALUE ================ */
static struct class *g_omx_vdec_class = HI_NULL;
static const hi_char g_omx_vdec_drv_name[] = OMXVDEC_NAME;
static dev_t g_omx_vdec_dev_num;

typedef hi_s32 (*FN_IOCTL_HANDLER)(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg);
typedef struct {
    hi_u32 code;
    FN_IOCTL_HANDLER p_handler;
} IOCTL_COMMAND_NODE;

MODULE_DESCRIPTION("omxvdec driver");
MODULE_AUTHOR("hisilicon");
MODULE_LICENSE("GPL");

/* ==========================================================================
 * FUNTION
 * ========================================================================= */
hi_void omxvdec_release_mem(hi_void *p_buffer, e_mem_alloc e_mem_alloc, omx_mem_free_type free_type)
{
    omxvdec_buffer *p_tmp_buffer = HI_NULL;

    OMXVDEC_ASSERT_RETURN_NULL(p_buffer != HI_NULL, "invalid param");

    p_tmp_buffer = (omxvdec_buffer *)p_buffer;

    if (p_tmp_buffer->phys_addr == HI_NULL) {
        omx_print(OMX_WARN, "can NOT releae mem: size:%d addr:0x%llx\n", p_tmp_buffer->size,
                  p_tmp_buffer->phys_addr);

        return;
    }

    switch (e_mem_alloc) {
        case ALLOC_BY_MMZ: {
            hi_drv_omxvdec_unmap_and_free(p_tmp_buffer, free_type);
            break;
        }

        case ALLOC_BY_MMZ_UNMAP: {
            hi_drv_omxvdec_free(p_tmp_buffer, free_type);
            break;
        }

        case ALLOC_BY_SEC: {
            hi_drv_omxvdec_free(p_tmp_buffer, free_type);
            break;
        }

        default:
        {
            omx_print(OMX_FATAL, "invalid e_mem_alloc = %d\n", e_mem_alloc);
            break;
        }
    }

    HI_CHECK_SEC_FUNC(memset_s(p_buffer, sizeof(omxvdec_buffer), 0, sizeof(omxvdec_buffer)));

    return;
}

/* ==========================================================================
 * omxvdec device mode
 * ========================================================================= */
static hi_s32 omxvdec_setup_cdev(OMXVDEC_ENTRY *omxvdec, const struct file_operations *fops)
{
    hi_s32 rc = -ENODEV;
    struct device *dev = HI_NULL;

    g_omx_vdec_class = class_create(THIS_MODULE, "omxvdec_class");
    if (IS_ERR(g_omx_vdec_class)) {
        rc = PTR_ERR(g_omx_vdec_class);
        g_omx_vdec_class = HI_NULL;
        omx_print(OMX_FATAL, "call class_create failed, rc = %d\n", rc);
        return rc;
    }

    rc = alloc_chrdev_region(&g_omx_vdec_dev_num, 0, 1, "hisi video decoder");
    if (rc) {
        omx_print(OMX_FATAL, "call alloc_chrdev_region failed, rc = %d\n", rc);
        goto cls_destroy;
    }

    dev = device_create(g_omx_vdec_class, HI_NULL, g_omx_vdec_dev_num, HI_NULL, OMXVDEC_NAME);
    if (IS_ERR(dev)) {
        rc = PTR_ERR(dev);
        omx_print(OMX_FATAL, "call device_create failed, rc = %d\n", rc);
        goto unregister_region;
    }

    cdev_init(&omxvdec->cdev, fops);
    omxvdec->cdev.owner = THIS_MODULE;
    omxvdec->cdev.ops = fops;
    rc = cdev_add(&omxvdec->cdev, g_omx_vdec_dev_num, 1);
    if (rc < 0) {
        omx_print(OMX_FATAL, "call cdev_add failed, rc = %d\n", rc);
        goto dev_destroy;
    }

    return HI_SUCCESS;

dev_destroy:
    device_destroy(g_omx_vdec_class, g_omx_vdec_dev_num);
unregister_region:
    unregister_chrdev_region(g_omx_vdec_dev_num, 1);
cls_destroy:
    class_destroy(g_omx_vdec_class);
    g_omx_vdec_class = HI_NULL;

    return rc;
}

static hi_s32 omxvdec_cleanup_cdev(OMXVDEC_ENTRY *omxvdec)
{
    if (g_omx_vdec_class == HI_NULL) {
        omx_print(OMX_FATAL, "FATAL: g_omx_vdec_class = HI_NULL");
        return HI_FAILURE;
    } else {
        cdev_del(&omxvdec->cdev);
        device_destroy(g_omx_vdec_class, g_omx_vdec_dev_num);
        unregister_chrdev_region(g_omx_vdec_dev_num, 1);
        class_destroy(g_omx_vdec_class);
        return HI_SUCCESS;
    }
}

/* ==========================================================================
 * hi_char device ops functions
 * ========================================================================= */
static hi_s32 omxvdec_open(struct inode *inode, struct file *fd)
{
    hi_s32 ret = -EBUSY;
    unsigned long flags;
    OMXVDEC_ENTRY *omxvdec = HI_NULL;

    omx_print(OMX_TRACE, "omxvdec prepare to open.\n");

    OMXVDEC_ASSERT_RETURN(((inode != HI_NULL) && (fd != HI_NULL)), "param is null");

    omxvdec = container_of(inode->i_cdev, OMXVDEC_ENTRY, cdev);

    VDEC_DOWN_INTERRUPTIBLE(&omxvdec->dev_sema);
    osal_spin_lock_irqsave(&omxvdec->lock, &flags);
    if (omxvdec->open_count < MAX_OPEN_COUNT) {
        omxvdec->open_count++;

        if (omxvdec->open_count == 1) {
            osal_spin_unlock_irqrestore(&omxvdec->lock, &flags);

            HI_CHECK_SEC_FUNC(memset_s(&g_st_omx_func, sizeof(g_st_omx_func), 0, sizeof(OMXVDEC_FUNC)));

            ret = channel_init();
            if (ret != HI_SUCCESS) {
                omx_print(OMX_FATAL, "init channel failed!\n");
                goto error;
            }

            ret = osal_exportfunc_get(HI_ID_VDEC, (hi_void **)&g_st_omx_func.p_vdec_func);
            if (ret != HI_SUCCESS || g_st_omx_func.p_vdec_func == HI_NULL) {
                omx_print(OMX_FATAL, "get vdec function failed!\n");
                goto error1;
            }
#if 0 /* todo */
            (((drv_vdec_export_func *)(g_st_omx_func.p_vdec_func))->pfn_drv_vdec_set_callback)(&g_omx_export_funcs);
#endif
            osal_spin_lock_irqsave(&omxvdec->lock, &flags);
        }

        fd->private_data = omxvdec;

        omx_print(OMX_TRACE, "omxvdec open ok.\n");
        ret = HI_SUCCESS;
    } else {
        omx_print(OMX_FATAL, "open omxvdec instance:%d too much!\n", omxvdec->open_count);
        ret = -EBUSY;
    }
    osal_spin_unlock_irqrestore(&omxvdec->lock, &flags);
    VDEC_UP_INTERRUPTIBLE(&omxvdec->dev_sema);

    return ret;

error1:
    channel_exit();
error:
    omxvdec->open_count--;
    VDEC_UP_INTERRUPTIBLE(&omxvdec->dev_sema);

    return ret;
}

static hi_s32 omxvdec_release(struct inode *inode, struct file *fd)
{
    unsigned long flags;
    OMXVDEC_ENTRY *omxvdec = HI_NULL;
    OMXVDEC_CHAN_CTX *pchan = HI_NULL;
    OMXVDEC_CHAN_CTX *n = HI_NULL;

    omx_print(OMX_TRACE, "omxvdec prepare to release.\n");
    OMXVDEC_ASSERT_RETURN(fd != HI_NULL, "fd = null");

    omxvdec = fd->private_data;
    if (omxvdec == HI_NULL) {
        omx_print(OMX_FATAL, "omxvdec = null, error!\n");
        return -EFAULT;
    }

    VDEC_DOWN_INTERRUPTIBLE(&omxvdec->dev_sema);

    osal_spin_lock_irqsave(&omxvdec->channel_lock, &flags);
    if (!list_empty(&omxvdec->chan_list)) {
        list_for_each_entry_safe(pchan, n, &omxvdec->chan_list, chan_list)
        {
            if ((pchan->file_dec == (hi_u32 *)fd) && (pchan->state != CHAN_STATE_INVALID)) {
                osal_spin_unlock_irqrestore(&omxvdec->channel_lock, &flags);
                channel_release_inst(pchan, OMX_MEM_FREE_TYPE_ABNORMAL);
                osal_spin_lock_irqsave(&omxvdec->channel_lock, &flags);
            }
        }
    }
    osal_spin_unlock_irqrestore(&omxvdec->channel_lock, &flags);

    osal_spin_lock_irqsave(&omxvdec->lock, &flags);
    if (omxvdec->open_count > 0) {
        omxvdec->open_count--;
    }
    osal_spin_unlock_irqrestore(&omxvdec->lock, &flags);

    if (omxvdec->open_count == 0) {
        channel_exit();
    }

    fd->private_data = HI_NULL;
    HI_CHECK_SEC_FUNC(memset_s(&g_st_omx_func, sizeof(g_st_omx_func), 0, sizeof(OMXVDEC_FUNC)));

    VDEC_UP_INTERRUPTIBLE(&omxvdec->dev_sema);

    omx_print(OMX_TRACE, "omxvdec release ok.\n");

    return 0;
}

static OMXVDEC_CHAN_CTX *omxvdec_ioctl_get_chan_context(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 chan_id;
    OMXVDEC_ENTRY *omxvdec = HI_NULL;

    if (p_msg == HI_NULL || fd == HI_NULL) {
        omx_print(OMX_FATAL, "%s invalid fd: %p, p_msg: %p.\n", __func__, fd, p_msg);

        return HI_NULL;
    }

    omxvdec = fd->private_data;

    if (omxvdec == HI_NULL) {
        omx_print(OMX_FATAL, "invalid param\n");
        return HI_NULL;
    }

    chan_id = p_msg->chan_num;
    if (chan_id < 0 || chan_id >= MAX_CHANNEL_NUM) {
        omx_print(OMX_FATAL, "invalid chan id: %d.\n", chan_id);

        return HI_NULL;
    }

    return channel_find_inst_by_channel_id(omxvdec, chan_id);
}

static hi_s32 omxvdec_ioctl_chan_create(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 chan_id;
    OMXVDEC_DRV_CFG chan_cfg;

    OMXVDEC_ASSERT_RETURN(p_msg->in != HI_NULL, "in is null");
    OMXVDEC_ASSERT_RETURN(p_msg->out != HI_NULL, "out is null");

    HI_CHECK_SEC_FUNC(memset_s(&chan_cfg, sizeof(chan_cfg), 0, sizeof(chan_cfg)));

    if (osal_copy_from_user(&chan_cfg, (hi_void *)(uintptr_t)p_msg->in, sizeof(OMXVDEC_DRV_CFG))) {
        omx_print(OMX_FATAL, "call osal_copy_from_user failed!\n");
        return -EFAULT;
    }

    chan_id = channel_create_inst(fd, &chan_cfg);
    if (chan_id < 0) {
        omx_print(OMX_FATAL, "call channel_create_inst failed!\n");
        return -EFAULT;
    }

    if (osal_copy_to_user((hi_void *)(uintptr_t)p_msg->out, &chan_id, sizeof(hi_s32))) {
        omx_print(OMX_FATAL, "call osal_copy_from_user failed!\n");
        return -EIO;
    }

    return 0;
}

static hi_s32 omxvdec_ioctl_chan_release(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 ret;
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");

    ret = channel_release_inst(pchan, OMX_MEM_FREE_TYPE_NORMAL);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call release failed!\n");

        return -EFAULT;
    }

    return 0;
}

static hi_s32 omxvdec_ioctl_chan_start(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 ret;
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");

    ret = channel_start_inst(pchan);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call start failed!\n");

        return -EFAULT;
    }

    return 0;
}

static hi_s32 omxvdec_ioctl_chan_stop(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 ret;
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");

    ret = channel_stop_inst(pchan);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call stop failed!\n");

        return -EFAULT;
    }

    return 0;
}

static hi_s32 omxvdec_ioctl_chan_pause(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 ret;
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");

    ret = channel_pause_inst(pchan);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call pause failed!\n");

        return -EFAULT;
    }

    return 0;
}

static hi_s32 omxvdec_ioctl_chan_resume(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 ret;
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");

    ret = channel_resume_inst(pchan);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call resume failed!\n");

        return -EFAULT;
    }

    return 0;
}

static hi_s32 omxvdec_ioctl_chan_alloc_buffer(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 ret;
    OMXVDEC_BUF_DESC user_buf;
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");
    OMXVDEC_ASSERT_RETURN(p_msg->in != HI_NULL, "in is null");
    OMXVDEC_ASSERT_RETURN(p_msg->out != HI_NULL, "out is null");

    HI_CHECK_SEC_FUNC(memset_s(&user_buf, sizeof(user_buf), 0, sizeof(user_buf)));

    if (osal_copy_from_user(&user_buf, (hi_void *)(uintptr_t)p_msg->in, sizeof(OMXVDEC_BUF_DESC))) {
        omx_print(OMX_FATAL, "call osal_copy_from_user failed!\n");

        return -EFAULT;
    }

    VDEC_DOWN_INTERRUPTIBLE(&pchan->chan_sema);
    ret = channel_alloc_buf(pchan, &user_buf);
    VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);

    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call channel_alloc_buf failed!\n");

        return -EFAULT;
    }

    if (osal_copy_to_user((hi_void *)(uintptr_t)p_msg->out, &user_buf, sizeof(OMXVDEC_BUF_DESC))) {
        omx_print(OMX_FATAL, "call osal_copy_to_user failed!\n");

        return -EIO;
    }

    return 0;
}

static hi_s32 omxvdec_ioctl_chan_release_buffer(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 ret;
    OMXVDEC_BUF_DESC user_buf;
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");
    OMXVDEC_ASSERT_RETURN(p_msg->in != HI_NULL, "in is null");

    HI_CHECK_SEC_FUNC(memset_s(&user_buf, sizeof(user_buf), 0, sizeof(user_buf)));

    if (osal_copy_from_user(&user_buf, (hi_void *)(uintptr_t)p_msg->in, sizeof(OMXVDEC_BUF_DESC))) {
        omx_print(OMX_FATAL, "call osal_copy_from_user failed!\n");

        return -EFAULT;
    }

    VDEC_DOWN_INTERRUPTIBLE(&pchan->chan_sema);
    ret = channel_release_buf(pchan, &user_buf, OMX_MEM_FREE_TYPE_NORMAL);
    VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);

    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call channel_release_buf failed!\n");

        return -EFAULT;
    }

    return 0;
}

static hi_s32 omxvdec_ioctl_chan_bind_buffer(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 ret;
    OMXVDEC_BUF_DESC user_buf;
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");
    OMXVDEC_ASSERT_RETURN(p_msg->in != HI_NULL, "in is null");

    HI_CHECK_SEC_FUNC(memset_s(&user_buf, sizeof(user_buf), 0, sizeof(user_buf)));

    if (osal_copy_from_user(&user_buf, (hi_void *)(uintptr_t)p_msg->in, sizeof(OMXVDEC_BUF_DESC))) {
        omx_print(OMX_FATAL, "call osal_copy_from_user failed!\n");

        return -EIO;
    }

    ret = channel_bind_user_buffer(pchan, &user_buf);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call bind_buffer failed!\n");

        return -EFAULT;
    }

    return 0;
}

static hi_s32 omxvdec_ioctl_chan_unbind_buffer(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 ret;
    OMXVDEC_BUF_DESC user_buf;
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");
    OMXVDEC_ASSERT_RETURN(p_msg->in != HI_NULL, "in is null");

    HI_CHECK_SEC_FUNC(memset_s(&user_buf, sizeof(user_buf), 0, sizeof(user_buf)));

    if (osal_copy_from_user(&user_buf, (hi_void *)(uintptr_t)p_msg->in, sizeof(OMXVDEC_BUF_DESC))) {
        omx_print(OMX_FATAL, "call osal_copy_from_user failed!\n");

        return -EIO;
    }

    VDEC_DOWN_INTERRUPTIBLE(&pchan->chan_sema);
    ret = channel_unbind_user_buffer(pchan, &user_buf, OMX_MEM_FREE_TYPE_NORMAL);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "case call unbind_buffer failed!\n");
        VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);
        return -EFAULT;
    }

    VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);

    return 0;
}

static hi_s32 omxvdec_ioctl_empty_input_stream(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 ret;
    OMXVDEC_BUF_DESC user_buf;
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");
    OMXVDEC_ASSERT_RETURN(p_msg->in != HI_NULL, "in is null");

    HI_CHECK_SEC_FUNC(memset_s(&user_buf, sizeof(user_buf), 0, sizeof(user_buf)));

    if (osal_copy_from_user(&user_buf, (hi_void *)(uintptr_t)p_msg->in, sizeof(OMXVDEC_BUF_DESC))) {
        omx_print(OMX_FATAL, "call osal_copy_from_user failed!\n");

        return -EIO;
    }

    ret = channel_empty_this_stream(pchan, &user_buf);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call empty_stream failed!\n");

        return -EFAULT;
    }

    return 0;
}

static hi_s32 omxvdec_ioctl_fill_output_frame(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 ret;
    OMXVDEC_BUF_DESC user_buf;
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");
    OMXVDEC_ASSERT_RETURN(p_msg->in != HI_NULL, "in is null");

    HI_CHECK_SEC_FUNC(memset_s(&user_buf, sizeof(user_buf), 0, sizeof(user_buf)));

    if (osal_copy_from_user(&user_buf, (hi_void *)(uintptr_t)p_msg->in, sizeof(OMXVDEC_BUF_DESC))) {
        omx_print(OMX_FATAL, "call osal_copy_from_user failed!\n");

        return -EIO;
    }

    VDEC_DOWN_INTERRUPTIBLE(&pchan->chan_sema);
    user_buf.from_work_queue = HI_FALSE;
    if (pchan->anw_store_meta == HI_TRUE) {
        ret = channel_fill_anw_store_meta(pchan, &user_buf);
    } else {
        ret = channel_fill_this_frame(pchan, &user_buf);
    }
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call fill_frame failed!\n");
        VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);
        return -EFAULT;
    }
    VDEC_UP_INTERRUPTIBLE(&pchan->chan_sema);

    return 0;
}

static hi_s32 omxvdec_ioctl_flush_port(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 ret;
    e_port_dir flush_dir = PORT_DIR_INPUT;
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");
    OMXVDEC_ASSERT_RETURN(p_msg->in != HI_NULL, "in is null");

    if (osal_copy_from_user(&flush_dir, (hi_void *)(uintptr_t)p_msg->in, sizeof(e_port_dir))) {
        omx_print(OMX_FATAL, "call osal_copy_from_user failed!\n");

        return -EFAULT;
    }

    ret = channel_flush_inst(pchan, flush_dir);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "call flush_port failed!\n");

        return -EFAULT;
    }

    return 0;
}

static hi_s32 omxvdec_ioctl_chan_get_msg(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 ret;
    OMXVDEC_MSG_INFO msg = { 0 };
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");
    OMXVDEC_ASSERT_RETURN(p_msg->out != HI_NULL, "out is null");

    ret = channel_get_message(pchan, &msg);
    if (ret != HI_SUCCESS) {
        if (ret == -EAGAIN) {
            return -EAGAIN;
        } else {
            omx_print(OMX_WARN, "get msg error!\n");

            return -EFAULT;
        }
    }

    if (osal_copy_to_user((hi_void *)(uintptr_t)p_msg->out, &msg, sizeof(OMXVDEC_MSG_INFO))) {
        omx_print(OMX_FATAL, "call osal_copy_from_user failed!\n");

        return -EIO;
    }

    return 0;
}

static hi_s32 omxvdec_ioctl_chan_port_enable(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_bool b_port_enable = HI_FALSE;
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");
    OMXVDEC_ASSERT_RETURN(p_msg->in != HI_NULL, "in is null");

    if (osal_copy_from_user(&b_port_enable, (hi_void *)(uintptr_t)p_msg->in, sizeof(hi_bool))) {
        omx_print(OMX_FATAL, "call osal_copy_from_user failed!\n");

        return -EFAULT;
    }

    pchan->port_enable_flag = b_port_enable;
    omx_print(OMX_INFO, "set port enable -> %d!\n", pchan->port_enable_flag);

    return 0;
}

static hi_s32 omxvdec_ioctl_chan_set_format_info(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_s32 ret;
    OMXVDEC_DRV_CFG chan_cfg;
    OMXVDEC_CHAN_CTX *pchan = omxvdec_ioctl_get_chan_context(fd, p_msg);

    OMXVDEC_ASSERT_RETURN(pchan != HI_NULL, "chan context is null");
    OMXVDEC_ASSERT_RETURN(p_msg->in != HI_NULL, "in is null");

    HI_CHECK_SEC_FUNC(memset_s(&chan_cfg, sizeof(chan_cfg), 0, sizeof(chan_cfg)));

    if (osal_copy_from_user(&chan_cfg, (hi_void *)(uintptr_t)p_msg->in, sizeof(OMXVDEC_DRV_CFG))) {
        omx_print(OMX_FATAL, "%s: call osal_copy_from_user failed!\n", __func__);

        return -EFAULT;
    }

    pchan->output_afbc = chan_cfg.output_afbc;

    ret = channel_config_inst(pchan, &chan_cfg);
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "%s: call channel_config_inst failed\n", __func__);

        return -EFAULT;
    }

    return 0;
}

static hi_s32 omxvdec_ioctl_get_buffer_info(struct file *fd, OMXVDEC_IOCTL_MSG *p_msg)
{
    hi_u32 size;
    hi_s32 ret;
    hi_u32 total_size, mmz_size;

    OMXVDEC_ASSERT_RETURN(p_msg != HI_NULL, "p_msg == HI_NULL ");
    OMXVDEC_ASSERT_RETURN(p_msg->chan_num >= 0 && p_msg->chan_num < MAX_CHANNEL_NUM, "chan_id is valid");
    OMXVDEC_ASSERT_RETURN(p_msg->out != HI_NULL, "out is null");

    if (g_es_size != 0) {
        size = g_es_size;
    } else {
        ret = hi_drv_sys_get_mem_cfg(&total_size, &mmz_size);
        if (ret == HI_SUCCESS) {
            if (total_size > 768) { /* 768M */
                size = 10 * 1024 * 1024; /* 2M:2*1024*1024 */
            } else {
                size = 512 * 1024; /* 512k:512*1024 */
            }
        } else {
            size = 0;
            omx_print(OMX_FATAL, "call hi_drv_sys_get_mem_config failed\n");
        }
    }

    if (osal_copy_to_user((hi_void *)(uintptr_t)p_msg->out, &size, sizeof(hi_u32))) {
        omx_print(OMX_FATAL, "call osal_copy_to_user failed!\n");

        return -EIO;
    }

    return 0;
}

static const IOCTL_COMMAND_NODE g_ioctl_command_table[] = {
    { VDEC_IOCTL_CHAN_GET_MSG,         omxvdec_ioctl_chan_get_msg },
    { VDEC_IOCTL_CHAN_CREATE,          omxvdec_ioctl_chan_create },
    { VDEC_IOCTL_CHAN_RELEASE,         omxvdec_ioctl_chan_release },
    { VDEC_IOCTL_CHAN_START,           omxvdec_ioctl_chan_start },
    { VDEC_IOCTL_CHAN_STOP,            omxvdec_ioctl_chan_stop },
    { VDEC_IOCTL_CHAN_PAUSE,           omxvdec_ioctl_chan_pause },
    { VDEC_IOCTL_CHAN_RESUME,          omxvdec_ioctl_chan_resume },
    { VDEC_IOCTL_CHAN_ALLOC_BUF,       omxvdec_ioctl_chan_alloc_buffer },
    { VDEC_IOCTL_CHAN_RELEASE_BUF,     omxvdec_ioctl_chan_release_buffer },
    { VDEC_IOCTL_CHAN_BIND_BUFFER,     omxvdec_ioctl_chan_bind_buffer },
    { VDEC_IOCTL_CHAN_UNBIND_BUFFER,   omxvdec_ioctl_chan_unbind_buffer },
    { VDEC_IOCTL_EMPTY_INPUT_STREAM,   omxvdec_ioctl_empty_input_stream },
    { VDEC_IOCTL_FILL_OUTPUT_FRAME,    omxvdec_ioctl_fill_output_frame },
    { VDEC_IOCTL_FLUSH_PORT,           omxvdec_ioctl_flush_port },
    { VDEC_IOCTL_CHAN_PORT_ENABLE,     omxvdec_ioctl_chan_port_enable },
    { VDEC_IOCTL_CHAN_SET_FORMAT_INFO, omxvdec_ioctl_chan_set_format_info },
    { VDEC_IOCTL_GET_BUFFER_INFO,      omxvdec_ioctl_get_buffer_info},

    { 0, HI_NULL },  // terminal element
};

FN_IOCTL_HANDLER omxvdec_ioctl_get_handler(hi_u32 code)
{
    hi_u32 index = 0;
    FN_IOCTL_HANDLER p_target_handler = HI_NULL;

    while (1) {
        if (g_ioctl_command_table[index].code == 0 || g_ioctl_command_table[index].p_handler == HI_NULL) {
            break;
        }

        if (code == g_ioctl_command_table[index].code) {
            p_target_handler = g_ioctl_command_table[index].p_handler;
            break;
        }

        index++;
    }

    return p_target_handler;
}

static long omxvdec_ioctl(struct file *fd, unsigned int code, unsigned long arg)
{
    OMXVDEC_IOCTL_MSG vdec_msg;
    hi_void *u_arg = (hi_void *)(uintptr_t)arg;
    FN_IOCTL_HANDLER p_ioctl_handler;
    hi_s32 ret;
    OMXVDEC_ENTRY *omxvdec = HI_NULL;

    if (fd == HI_NULL) {
        omx_print(OMX_FATAL, "%s invalid fd: %p\n", __func__, fd);
        return -EIO;
    }

    omxvdec = fd->private_data;

    if (omxvdec == HI_NULL) {
        omx_print(OMX_FATAL, "invalid param\n");
        return -EIO;
    }

    if ((fd == HI_NULL) || (u_arg == HI_NULL)) {
        omx_print(OMX_FATAL, "param is NULL\n");
        return -EFAULT;
    }

    HI_CHECK_SEC_FUNC(memset_s(&vdec_msg, sizeof(vdec_msg), 0, sizeof(vdec_msg)));

    if (osal_copy_from_user(&vdec_msg, u_arg, sizeof(OMXVDEC_IOCTL_MSG))) {
        omx_print(OMX_FATAL, "call osal_copy_from_user failed! \n");

        return -EFAULT;
    }

    p_ioctl_handler = omxvdec_ioctl_get_handler(code);
    if (p_ioctl_handler == HI_NULL) {
        omx_print(OMX_FATAL, "ERROR cmd %d is not supported!\n", _IOC_NR(code));

        return -ENOTTY;
    }

    VDEC_DOWN_INTERRUPTIBLE(&omxvdec->dev_sema);
    ret = p_ioctl_handler(fd, &vdec_msg);
    VDEC_UP_INTERRUPTIBLE(&omxvdec->dev_sema);

    return ret;
}

static const struct file_operations g_omxvdec_fops = {

    .owner = THIS_MODULE,
    .open = omxvdec_open,
    .unlocked_ioctl = omxvdec_ioctl, /* user space and kernel space same with 32 bit or 64 bit run this function */
#ifdef CONFIG_COMPAT
    .compat_ioctl = omxvdec_ioctl, /* user space 32bit and kernel space 64 bit run this function */
#endif
    .release = omxvdec_release,
};

static hi_s32 omxvdec_probe(struct platform_device *pltdev)
{
    hi_s32 ret;
    OMXVDEC_ENTRY *omxvdec = HI_NULL;

    omx_print(OMX_TRACE, "omxvdec prepare to probe.\n");

    platform_set_drvdata(pltdev, HI_NULL);

    omxvdec = hi_kmalloc_omxvdec(sizeof(OMXVDEC_ENTRY));
    if (omxvdec == HI_NULL) {
        omx_print(OMX_FATAL, "alloc failed!\n");
        return -ENOMEM;
    }

    HI_CHECK_SEC_FUNC(memset_s(omxvdec, sizeof(OMXVDEC_ENTRY), 0, sizeof(OMXVDEC_ENTRY)));

    INIT_LIST_HEAD(&omxvdec->chan_list);
    osal_spin_lock_init(&omxvdec->lock);
    osal_spin_lock_init(&omxvdec->channel_lock);
    osal_spin_lock_init(&omxvdec->stream_lock);
    VDEC_INIT_MUTEX(&omxvdec->dev_sema);

    ret = omxvdec_setup_cdev(omxvdec, &g_omxvdec_fops);
    if (ret < 0) {
        omx_print(OMX_FATAL, "call omxvdec_setup_cdev failed!\n");
        goto cleanup;
    }

    omxvdec->device = &pltdev->dev;
    platform_set_drvdata(pltdev, omxvdec);
    g_omx_vdec = omxvdec;

    omx_print(OMX_TRACE, "omxvdec probe ok.\n");

    return 0;

cleanup:
    hi_kfree_omxvdec(omxvdec);

    return ret;
}

static hi_s32 omxvdec_remove(struct platform_device *pltdev)
{
    OMXVDEC_ENTRY *omxvdec = HI_NULL;

    omx_print(OMX_TRACE, "omxvdec prepare to remove.\n");

    omxvdec = platform_get_drvdata(pltdev);
    if (omxvdec == HI_NULL) {
        omx_print(OMX_ERR, "call platform_get_drvdata err!\n");
    } else if (IS_ERR(omxvdec)) {
        omx_print(OMX_ERR, "call platform_get_drvdata err, errno = %ld!\n", PTR_ERR(omxvdec));
    } else {
        omxvdec_cleanup_cdev(omxvdec);

        hi_kfree_omxvdec(omxvdec);
        g_omx_vdec = HI_NULL;
    }

    platform_set_drvdata(pltdev, HI_NULL);

    omx_print(OMX_TRACE, "remove omxvdec ok.\n");

    return 0;
}

hi_u32 omx_get_time_in_ms(hi_void)
{
    hi_u64 sys_time;

    sys_time = osal_sched_clock();
    sys_time = osal_div_u64(sys_time, 1000000); /* 1000000 is unit conversion */
    return (hi_u32)sys_time;
}

#if (HI_PROC_SUPPORT == 1)

#define CMD_LEN_MAX 32
/* ==========================================================================
 * omxvdec proc entrance
 * ========================================================================= */
static hi_s32 omxvdec_string_to_value(hi_char* str, hi_u32 *data)
{
    hi_u32 i, d, dat, weight;

    dat = 0;
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        i = 2; /* 2 is the initial value */
        weight = 16; /* 16 is the length of the character string */
    } else {
        i = 0;
        weight = 10; /* 10 is the length of the character string */
    }

    for (; i < 10; i++) { /* 10 is the maximum value of i */
        if (str[i] < 0x20) {
            break;
        } else if (weight == 16 && str[i] >= 'a' && str[i] <= 'f') { /* 16 is the length of the character string */
            d = str[i] - 'a' + 10;  /* 10 is gap value */
        } else if (weight == 16 && str[i] >= 'A' && str[i] <= 'F') { /* 16 is the length of the character string */
            d = str[i] - 'A' + 10; /* 10 is gap value */
        } else if (str[i] >= '0' && str[i] <= '9') {
            d = str[i] - '0';
        } else {
            return -1;
        }

        dat = dat * weight + d;
    }

    *data = dat;

    return HI_SUCCESS;
}

hi_void omxvdec_help_proc(hi_void)
{
    HI_PRINT("\n"
                "================== OMXVDEC HELP ===========\n"
                "USAGE:echo [cmd] [para] > /proc/msp/omxvdec\n\n"
                "cmd = set_print,          para = value        :set print   = value\n"
                "cmd = set_dispnum,        para = value        :set disp_num = value\n"
                "cmd = set_segsize,        para = value        :set seg_size = value\n"
                "cmd = set_essize,         para = value        :set es_size = value\n"
                "cmd = set_lowdelay_frame, para = value        :set low_delay_count_frame = value\n");

    HI_PRINT("cmd = save_raw,           para = 0            :save chan 0 raw data\n"
                "cmd = save_yuv,           para = 0            :save chan 0 yuv data\n"
                "cmd = save_img,           para = 0            :save chan 0 decoder output data\n"
                "cmd = save_name,          para = name         :config name to save data\n"
                "cmd = save_path,          para = path         :config path to save data\n");

    HI_PRINT("cmd = turn_framerate,     para = on/off       :enable/disable frame rate limit\n"
                "cmd = turn_rawmove,       para = on/off       :enable/disable raw move\n"
                "cmd = turn_fastout,       para = on/off       :enable/disable fast output\n");

    HI_PRINT("cmd = turn_dfs,           para = on/off       :enable/disable dynamic frame store\n"
                "cmd = turn_compress,      para = on/off       :enable/disable compress\n"
                "cmd = turn_lowdelay,      para = on/off       :enable/disable lowdelay statistics\n"
                "cmd = map_frm             para = on/off       :enable/disable map frame buf\n\n");

    HI_PRINT("trace_option(32bits):\n"
                "1(FATAL)    2(ERR)     4(WARN)      8(INFO)  \n"
                "16(TRACE)  32(INBUF   64(OUTBUF)  128(PP)    \n"
                "8(VERBOSE) 512(PTS)                          \n"
                "-------------------------------------------\n\n");
}

hi_bool omxvdec_check_save_file(hi_s32 handle, e_save_flie save_type)
{
    hi_bool ret = HI_FALSE;

    switch (save_type) {
        case SAVE_FLIE_RAW:
            if (g_save_raw_file != HI_NULL && handle == g_save_raw_chan_num) {
                ret = HI_TRUE;
            }
            break;

        case SAVE_FLIE_YUV:
            if (g_save_yuv_enable != HI_FALSE && handle == g_save_yuv_chan_num) {
                ret = HI_TRUE;
            }
            break;

        case SAVE_FLIE_IMG:
            if (g_save_src_yuv_file != HI_NULL && g_save_src_yuv_chan_num == handle) {
                ret = HI_TRUE;
            }
            break;
    }

    return ret;
}

hi_s32 omxvdec_open_save_file(hi_s32 handle, e_save_flie save_type)
{
    hi_s32 ret = HI_FAILURE;
    hi_char file_path[PATH_LEN];
    hi_s32 ret1;
    switch (save_type) {
        case SAVE_FLIE_RAW:
            ret1 = snprintf_s(file_path, sizeof(file_path), sizeof(file_path) - 1, "%s/%s_%d.raw",
                g_save_path, g_save_name, g_save_num);
            if (ret1 == -1) {
                omx_print(OMX_ERR, "call sec funtion error");
            }

            g_save_raw_file = filp_open(file_path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXO);
            if (IS_ERR(g_save_raw_file)) {
                g_save_raw_file = HI_NULL;
            } else {
                g_save_raw_chan_num = handle;
                ret = HI_SUCCESS;
            }
            break;

        case SAVE_FLIE_IMG:
            ret1 = snprintf_s(file_path, sizeof(file_path), sizeof(file_path) - 1, "%s/%s_%d_src.yuv",
                g_save_path, g_save_name, g_save_num);
            if (ret1 == -1) {
                omx_print(OMX_ERR, "call sec funtion error");
            }

            g_save_src_yuv_file = filp_open(file_path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXO);
            if (IS_ERR(g_save_src_yuv_file)) {
                g_save_src_yuv_file = HI_NULL;
            } else {
                g_save_src_yuv_chan_num = handle;
                ret = HI_SUCCESS;
            }
            break;

        default:

            break;
    }

    if (ret == HI_SUCCESS) {
        omx_print(OMX_ALWS, "open file %s of inst %d success.\n", file_path, handle);
    } else {
        omx_print(OMX_ALWS, "open file %s of inst %d failed.\n", file_path, handle);
    }

    return ret;
}

hi_s32 omxvdec_close_save_file(hi_s32 handle, e_save_flie save_type)
{
    hi_s32 ret = HI_FAILURE;

    switch (save_type) {
        case SAVE_FLIE_RAW:
            if (handle == g_save_raw_chan_num) {
                filp_close(g_save_raw_file, HI_NULL);
                omx_print(OMX_ALWS, "close raw file of inst %d.\n", g_save_raw_chan_num);
                g_save_raw_file = HI_NULL;
                g_save_raw_chan_num = -1;
                ret = HI_SUCCESS;
            }
            break;

        case SAVE_FLIE_IMG:
            if (g_save_src_yuv_chan_num == handle) {
                filp_close(g_save_src_yuv_file, HI_NULL);
                omx_print(OMX_ALWS, "close src yuv of inst %d.\n", g_save_src_yuv_chan_num);
                g_save_src_yuv_file = HI_NULL;
                g_save_src_yuv_chan_num = -1;

                ret = HI_SUCCESS;
            }
            break;

        default:

            break;
    }

    return ret;
}

static hi_s32 omxvdec_proc_cmd_set_print(hi_char *str, hi_s32 len)
{
    hi_u32 Data = 0;

    OMXVDEC_ASSERT_RETURN(len > 0 && len <= CMD_LEN_MAX && str != HI_NULL, " param is invalid ");

    if (omxvdec_string_to_value(str, &Data) != 0) {
        return HI_FAILURE;
    }

    omx_print(OMX_ALWS, "set print: %u\n", Data);
    g_trace_option = Data;

    return HI_SUCCESS;
}

static hi_s32 omxvdec_proc_cmd_set_clientreservednum(hi_char *str, hi_s32 len)
{
    hi_u32 Data = 0;

    OMXVDEC_ASSERT_RETURN(len > 0 && len <= CMD_LEN_MAX && str != HI_NULL, " param is invalid ");

    if (omxvdec_string_to_value(str, &Data) != 0) {
        return HI_FAILURE;
    }

    omx_print(OMX_ALWS, "set client_reserved_num: %u\n", Data);
    g_client_reserved_num = Data;

    return HI_SUCCESS;
}

static hi_s32 omxvdec_proc_cmd_set_segsize(hi_char *str, hi_s32 len)
{
    hi_u32 Data = 0;

    OMXVDEC_ASSERT_RETURN(len > 0 && len <= CMD_LEN_MAX && str != HI_NULL, " param is invalid ");

    if (omxvdec_string_to_value(str, &Data) != 0) {
        return HI_FAILURE;
    }

    if (Data > 32) { /* 32 is data size */
        omx_print(OMX_ALWS, "set seg size: %d too big\n", Data);
        return HI_FAILURE;
    }

    omx_print(OMX_ALWS, "set seg_size: %u\n", Data);
    g_seg_size = Data;

    return HI_SUCCESS;
}

static hi_s32 omxvdec_proc_cmd_set_essize(hi_char *str, hi_s32 len)
{
    hi_u32 Data = 0;

    OMXVDEC_ASSERT_RETURN(len > 0 && len <= CMD_LEN_MAX && str != HI_NULL, " param is invalid ");

    if (omxvdec_string_to_value(str, &Data) != 0) {
        return HI_FAILURE;
    }

    if (Data > (32 * 1024)) { /* 32 * 1024 is data size */
        omx_print(OMX_ALWS, "set es size: %d too big\n", Data);
        return HI_FAILURE;
    }

    omx_print(OMX_ALWS, "set es size: %u kb\n", Data);
    g_es_size = Data * 1024; /* 1024 is unit conversion */

    return HI_SUCCESS;
}

static hi_s32 omxvdec_proc_cmd_save_raw(hi_char *str, hi_s32 len)
{
    hi_u32 data = 0;

    OMXVDEC_ASSERT_RETURN(len > 0 && len <= CMD_LEN_MAX && str != HI_NULL, " param is invalid ");

    if (omxvdec_string_to_value(str, &data) != 0) {
        return HI_FAILURE;
    }

    if (omxvdec_check_save_file(data, SAVE_FLIE_RAW) == HI_FALSE) {
        omx_print(OMX_ALWS, "enable raw save.\n");
        omxvdec_open_save_file(data, SAVE_FLIE_RAW);
        g_save_raw_enable = HI_TRUE;
        g_save_num++;
    } else {
        omx_print(OMX_ALWS, "disable raw save.\n");
        omxvdec_close_save_file(data, SAVE_FLIE_RAW);
        g_save_raw_enable = HI_FALSE;
    }

    return HI_SUCCESS;
}

static hi_s32 omxvdec_proc_cmd_save_yuv(hi_char *str, hi_s32 len)
{
    hi_u32 data = 0;

    OMXVDEC_ASSERT_RETURN(len > 0 && len <= CMD_LEN_MAX && str != HI_NULL, " param is invalid ");

    if (omxvdec_string_to_value(str, &data) != 0) {
        return HI_FAILURE;
    }

    if (omxvdec_check_save_file(data, SAVE_FLIE_YUV) == HI_FALSE) {
        omx_print(OMX_ALWS, "enable yuv save.\n");
        g_save_yuv_enable = HI_TRUE;
        g_save_yuv_chan_num = data;
        g_save_num++;
    } else {
        omx_print(OMX_ALWS, "disable yuv save.\n");
        g_save_yuv_chan_num = -1;
        g_save_yuv_enable = HI_FALSE;
    }

    return HI_SUCCESS;
}

static hi_s32 omxvdec_proc_cmd_save_img(hi_char *str, hi_s32 len)
{
    hi_u32 data = 0;

    OMXVDEC_ASSERT_RETURN(len > 0 && len <= CMD_LEN_MAX && str != HI_NULL, " param is invalid ");

    if (omxvdec_string_to_value(str, &data) != 0) {
        return HI_FAILURE;
    }

    if (omxvdec_check_save_file(data, SAVE_FLIE_IMG) == HI_FALSE) {
        omx_print(OMX_ALWS, "enable image save.\n");
        omxvdec_open_save_file(data, SAVE_FLIE_IMG);
        g_save_src_yuv_enable = HI_TRUE;
        g_save_num++;
    } else {
        omx_print(OMX_ALWS, "disable image save.\n");
        omxvdec_close_save_file(data, SAVE_FLIE_IMG);
        g_save_src_yuv_enable = HI_FALSE;
    }

    return HI_SUCCESS;
}

static hi_s32 omxvdec_proc_cmd_save_name(hi_char *str, hi_s32 len)
{
    hi_s32 ret;

    OMXVDEC_ASSERT_RETURN(len > 0 && len <= CMD_LEN_MAX && str != HI_NULL, " param is invalid ");

    ret = strncpy_s(g_save_name, NAME_LEN, str, NAME_LEN - 1);
    if (ret == HI_SUCCESS) {
        omx_print(OMX_ALWS, "save_name: %s\n", g_save_name);
    } else {
        omx_print(OMX_ALWS, "invalid name \"%s\".\n", str);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 omxvdec_proc_cmd_save_path(hi_char *str, hi_s32 len)
{
    hi_s32 ret;

    OMXVDEC_ASSERT_RETURN(len > 0 && len <= CMD_LEN_MAX && str != HI_NULL, " param is invalid ");

    ret = strncpy_s(g_save_path, PATH_LEN, str, PATH_LEN - 1);
    if (ret == HI_SUCCESS) {
        omx_print(OMX_ALWS, "save_path: %s\n", g_save_path);
    } else {
        omx_print(OMX_ALWS, "invalid path \"%s\".\n", str);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 omxvdec_proc_cmd_turn_rawmove(hi_char *str, hi_s32 len)
{
    OMXVDEC_ASSERT_RETURN(len > 0 && len < CMD_LEN_MAX && str != HI_NULL, " param is invalid ");

    if (!strncmp(str, DBG_CMD_ON, sizeof(DBG_CMD_ON))) {
        omx_print(OMX_ALWS, "enable raw move.\n");
        g_raw_move_enable = HI_TRUE;
    } else if (!strncmp(str, DBG_CMD_OFF, sizeof(DBG_CMD_OFF))) {
        omx_print(OMX_ALWS, "disable raw move.\n");
        g_raw_move_enable = HI_FALSE;
    } else {
        omx_print(OMX_ALWS, "unkown command \"%s\".\n", str);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 omxvdec_proc_cmd_turn_fastoutput(hi_char *str, hi_s32 len)
{
    OMXVDEC_ASSERT_RETURN(len > 0 && len <= CMD_LEN_MAX && str != HI_NULL, " param is invalid ");

    if (!strncmp(str, DBG_CMD_ON, sizeof(DBG_CMD_ON))) {
        omx_print(OMX_ALWS, "enable fast_output_mode.\n");
        g_fast_output_mode = HI_TRUE;
    } else if (!strncmp(str, DBG_CMD_OFF, sizeof(DBG_CMD_OFF))) {
        omx_print(OMX_ALWS, "disable fast_output_mode.\n");
        g_fast_output_mode = HI_FALSE;
    } else {
        omx_print(OMX_ALWS, "unkown command \"%s\".\n", str);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 omxvdec_proc_cmd_map_frame(hi_char *str, hi_s32 len)
{
    OMXVDEC_ASSERT_RETURN(len > 0 && len <= CMD_LEN_MAX && str != HI_NULL, " param is invalid ");

    if (!strncmp(str, DBG_CMD_ON, sizeof(DBG_CMD_ON))) {
        g_map_frm_enable = HI_TRUE;
    } else if (!strncmp(str, DBG_CMD_OFF, sizeof(DBG_CMD_OFF))) {
        g_map_frm_enable = HI_FALSE;
    } else {
        omx_print(OMX_ALWS, "unkown command \"%s\".\n", str);

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 omxvdec_proc_cmd_help(hi_char *str, hi_s32 len)
{
    omxvdec_help_proc();

    return HI_SUCCESS;
}

static hi_s32 omxvdec_write_proc(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    char *str1 = HI_NULL;
    char *str2 = HI_NULL;

    if (argc >= 1) { /* 1 arg num */
        str1 = argv[0]; /* 0 arg idx */
        if (argc >= 2) { /* 2 arg num */
            str2 = argv[1]; /* 1 arg idx */
        }

        if (!strncmp(str1, DBG_CMD_SET_PRINT, CMD_LEN_MAX)) {
            omxvdec_proc_cmd_set_print(str2, CMD_LEN_MAX);
        } else if (!strncmp(str1, DBG_CMD_SET_DISPNUM, CMD_LEN_MAX)) {
            omxvdec_proc_cmd_set_clientreservednum(str2, CMD_LEN_MAX);
        } else if (!strncmp(str1, DBG_CMD_SET_SEGSIZE, CMD_LEN_MAX)) {
            omxvdec_proc_cmd_set_segsize(str2, CMD_LEN_MAX);
        } else if (!strncmp(str1, DBG_CMD_SET_ESSIZE, CMD_LEN_MAX)) {
            omxvdec_proc_cmd_set_essize(str2, CMD_LEN_MAX);
        } else if (!strncmp(str1, DBG_CMD_SAVE_RAW, CMD_LEN_MAX)) {
            omxvdec_proc_cmd_save_raw(str2, CMD_LEN_MAX);
        } else if (!strncmp(str1, DBG_CMD_SAVE_YUV, CMD_LEN_MAX)) {
            omxvdec_proc_cmd_save_yuv(str2, CMD_LEN_MAX);
        } else if (!strncmp(str1, DBG_CMD_SAVE_IMG, CMD_LEN_MAX)) {
            omxvdec_proc_cmd_save_img(str2, CMD_LEN_MAX);
        } else if (!strncmp(str1, DBG_CMD_SAVE_NAME, CMD_LEN_MAX)) {
            omxvdec_proc_cmd_save_name(str2, CMD_LEN_MAX);
        } else if (!strncmp(str1, DBG_CMD_SAVE_PATH, CMD_LEN_MAX)) {
            omxvdec_proc_cmd_save_path(str2, CMD_LEN_MAX);
        } else if (!strncmp(str1, DBG_CMD_TURN_RAWMOVE, CMD_LEN_MAX)) {
            omxvdec_proc_cmd_turn_rawmove(str2, CMD_LEN_MAX);
        } else if (!strncmp(str1, DBG_CMD_TURN_FASTOUTPUT, CMD_LEN_MAX)) {
            omxvdec_proc_cmd_turn_fastoutput(str2, CMD_LEN_MAX);
        } else if (!strncmp(str1, DBG_CMD_MAP_FRAME, CMD_LEN_MAX)) {
            omxvdec_proc_cmd_map_frame(str2, CMD_LEN_MAX);
        } else {
            omxvdec_proc_cmd_help(str1, CMD_LEN_MAX);
        }
    } else {
        omxvdec_proc_cmd_help(str1, CMD_LEN_MAX);
    }

    return HI_SUCCESS;
}

osal_proc_cmd g_omx_proc_command_table[] = {
    { DBG_CMD_SET_PRINT,          omxvdec_write_proc },
    { DBG_CMD_SET_DISPNUM,        omxvdec_write_proc },
    { DBG_CMD_SET_SEGSIZE,        omxvdec_write_proc },
    { DBG_CMD_SET_ESSIZE,         omxvdec_write_proc },

    { DBG_CMD_SAVE_RAW,  omxvdec_write_proc },
    { DBG_CMD_SAVE_YUV,  omxvdec_write_proc },
    { DBG_CMD_SAVE_IMG,  omxvdec_write_proc },
    { DBG_CMD_SAVE_NAME, omxvdec_write_proc },
    { DBG_CMD_SAVE_PATH, omxvdec_write_proc },

    { DBG_CMD_TURN_RAWMOVE,    omxvdec_write_proc },
    { DBG_CMD_TURN_FASTOUTPUT, omxvdec_write_proc },
    { DBG_CMD_MAP_FRAME,       omxvdec_write_proc },
    { DBG_CMD_HELP,            omxvdec_write_proc },
};

static hi_s32 omxvdec_read_proc(hi_void *p, hi_void *v)
{
    unsigned long flags;
    OMXVDEC_CHAN_CTX *pchan = HI_NULL;

    if (g_omx_vdec == HI_NULL) {
        omx_print(OMX_ERR, "warnning: g_omx_vdec = HI_NULL\n");
        return -1;
    }

    if (p == HI_NULL) {
        omx_print(OMX_ALWS, "invalid  param!\n");
        return -1;
    }

    proc_print(p, "\n");
    proc_print(p, "============ OMXVDEC INFO ============\n");
    proc_print(p, "%-25s :%d\n", "version", OMXVDEC_VERSION);
    proc_print(p, "%-25s :%d\n", "active_chan_num", g_omx_vdec->total_chan_num);

    proc_print(p, "%-25s :%d\n", "print", g_trace_option);
    proc_print(p, "%-25s :%d\n", "save_raw_enable", g_save_raw_enable);
    proc_print(p, "%-25s :%d\n", "save_yuv_enable", g_save_yuv_enable);
    proc_print(p, "%-25s :%d\n", "save_src_yuv_enable", g_save_src_yuv_enable);
    proc_print(p, "%-25s :%s\n", "save_name", g_save_name);
    proc_print(p, "%-25s :%s\n", "save_path", g_save_path);
    proc_print(p, "%-25s :%d\n", "client_reserve_num", g_client_reserved_num);
    proc_print(p, "%-25s :%d\n", "seg_size(M)", g_seg_size);

    proc_print(p, "%-25s :%d\n", "raw_move_enable", g_raw_move_enable);
    proc_print(p, "%-25s :%d\n", "fast_output_mode", g_fast_output_mode);
    proc_print(p, "\n");

    if (g_omx_vdec->total_chan_num != 0) {
        osal_spin_lock_irqsave(&g_omx_vdec->channel_lock, &flags);
        list_for_each_entry(pchan, &g_omx_vdec->chan_list, chan_list)
        {
            channel_proc_entry(p, pchan);
        }
        osal_spin_unlock_irqrestore(&g_omx_vdec->channel_lock, &flags);
    }

    return 0;
}

hi_s32 omxvdec_init_proc(hi_void)
{
    osal_proc_entry *pst_item = HI_NULL;

    /* Create proc */
    pst_item = osal_proc_add("omxvdec", strlen("omxvdec"));
    if (pst_item == HI_NULL) {
        omx_print(OMX_FATAL, "Create omxvdec proc entry fail!\n");
        return HI_FAILURE;
    }

    /* set functions */
    pst_item->read  = omxvdec_read_proc;
    pst_item->cmd_list = g_omx_proc_command_table;
    pst_item->cmd_cnt = sizeof(g_omx_proc_command_table) / sizeof(osal_proc_cmd);

    return HI_SUCCESS;
}

hi_void omxvdec_exit_proc(hi_void)
{
    osal_proc_remove("omxvdec", strlen("omxvdec"));

    return;
}
#endif

static hi_s32 omxvdec_suspend(struct platform_device *pltdev, pm_message_t state)
{
    return HI_SUCCESS;
}

static hi_s32 omxvdec_resume(struct platform_device *pltdev)
{
    return HI_SUCCESS;
}

static hi_void g_omxvdec_device_release(struct device *dev)
{
    return;
}

static struct platform_driver g_omxvdec_driver = {

    .probe = omxvdec_probe,
    .remove = omxvdec_remove,
    .suspend = omxvdec_suspend,
    .resume = omxvdec_resume,
    .driver = {
        .name = (hi_char*)g_omx_vdec_drv_name,
        .owner = THIS_MODULE,
    },
};

static struct platform_device g_omxvdec_device = {

    .name = g_omx_vdec_drv_name,
    .id = -1,
    .dev = {
        .platform_data = HI_NULL,
        .release = g_omxvdec_device_release,
    },
};

hi_s32 drv_omxvdec_mod_init(hi_void)
{
    hi_s32 ret;

    ret = platform_device_register(&g_omxvdec_device);
    if (ret < 0) {
        omx_print(OMX_FATAL, "call platform_device_register failed!\n");
        return ret;
    }

    ret = platform_driver_register(&g_omxvdec_driver);
    if (ret < 0) {
        omx_print(OMX_FATAL, "call platform_driver_register failed!\n");
        platform_device_unregister(&g_omxvdec_device);
        return ret;
    }

#if (HI_PROC_SUPPORT == 1)
    ret = omxvdec_init_proc();
    if (ret != HI_SUCCESS) {
        omx_print(OMX_FATAL, "omxvdec_init_proc failed!\n");
        platform_driver_unregister(&g_omxvdec_driver);
        platform_device_unregister(&g_omxvdec_device);
        return ret;
    }

#ifdef MODULE
    HI_PRINT("load hi_omxvdec.ko success.\t(%s)\n", VERSION_STRING);
#endif
#endif

    return HI_SUCCESS;
}

hi_void drv_omxvdec_mod_exit(hi_void)
{
    platform_driver_unregister(&g_omxvdec_driver);
    platform_device_unregister(&g_omxvdec_device);

#if (HI_PROC_SUPPORT == 1)
    omxvdec_exit_proc();

#ifdef MODULE
    HI_PRINT("unload hi_omxvdec.ko success.\t(%s)\n", VERSION_STRING);
#endif
#endif
}

#ifdef MODULE
module_init(drv_omxvdec_mod_init);
module_exit(drv_omxvdec_mod_exit);
#else
EXPORT_SYMBOL(drv_omxvdec_mod_init);
EXPORT_SYMBOL(drv_omxvdec_mod_exit);
#endif

