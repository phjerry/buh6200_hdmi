/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-07-23
 */
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/ion.h>

#include "hi_osal.h"
#include "hi_drv_dev.h"
#include "drv_venc_ext.h"
#include "hi_drv_file.h"
#include "hi_drv_module.h"
#include "drv_venc_efl.h"
#include "drv_omxvenc_efl.h"
#include "drv_venc_ioctl.h"
#include "drv_venc.h"
#include "drv_omxvenc.h"
#include "linux/hisilicon/securec.h"
#include "linux/hisilicon/hi_license.h"
#include "hi_drv_sys.h"
#include "drv_venc_ratecontrol_client.h"

#ifdef HI_TEE_SUPPORT
#include "hi_drv_ssm.h"
#endif

#ifdef VENC_SUPPORT_JPGE
#include "drv_jpge_ext.h"
#endif

#ifdef VENC_SUPPORT_JPGE
hi_jpge_export_func *g_jpge_func = HI_NULL;
#endif

extern optm_venc_chn g_venc_chn[HI_VENC_MAX_CHN];
extern osal_spinlock g_send_frame_lock[HI_VENC_MAX_CHN];
extern vedu_osal_event g_rc_event;

#ifndef HI_ADVCA_FUNCTION_RELEASE
#define smooth_printk printk
#else
#define smooth_printk(format, arg...)
#endif

#define D_VENC_GET_CHN(ve_chn, venc_chn) do { \
    if ((venc_chn) == NULL) { \
        (ve_chn) = HI_VENC_MAX_CHN; \
        break; \
    } \
    for ((ve_chn) = 0; (ve_chn) < HI_VENC_MAX_CHN; (ve_chn)++) { \
        if (g_venc_chn[ve_chn].venc_handle == (venc_chn)) { break; } \
    } \
} while (0)

#define D_VENC_GET_CHN_BY_UHND(ve_chn, venc_usr_chn) \
    do { \
        if ((venc_usr_chn) == HI_INVALID_HANDLE) { \
            (ve_chn) = HI_VENC_MAX_CHN; \
            break; \
        } \
        for ((ve_chn) = 0; (ve_chn) < HI_VENC_MAX_CHN; (ve_chn)++) { \
            if (g_venc_chn[ve_chn].user_handle == (venc_usr_chn)) { break; } \
        } \
    } while (0)

#define D_VENC_UPDATE_2(data,curdata)  do {\
          *((data) + 1) = *(data);\
          *(data) = (curdata);\
        }while (0)

typedef struct {
    hi_bool setup;
    dev_t no;
    struct cdev cdev;
    struct device *dev;
    struct class *cls;
} venc_dev;

typedef struct {
    atomic_t open_cnt;
    venc_dev device;
} venc_dev_entry;

static venc_dev_entry g_venc_entry = {0};

static inline venc_dev_entry* venc_dev_get_entry(hi_void)
{
    return &g_venc_entry;
}

hi_bool g_is_venc_chan_alive[HI_VENC_MAX_CHN] = {HI_FALSE};

/* Lock and atomic variable */
osal_atomic g_venc_count;
osal_semaphore g_venc_mutex;
osal_semaphore g_venc_drv_mutex;

/* VENC device open times */
hi_void venc_timer_func(hi_length_t value);

hi_s32 venc_drv_resume(hi_void);
hi_s32 venc_drv_suspend(hi_void);

drv_venc_export_func g_venc_export_funcs = {
    .fn_venc_queue_frame   = venc_drv_efl_q_frame_by_attach,
};

osal_timer g_venc_timer;

hi_u32 g_venc_open_flag = 0;

static hi_s32 venc_drv_open(struct inode *finode, struct file  *ffile)
{
    hi_s32 ret, i;
    hi_u32 license_valid = 0;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    if (osal_atomic_inc_return(&g_venc_count) == 1) {
        ret = hi_drv_get_license_support(HI_LICENSE_ENCODE_EN, &license_valid);
        if ((license_valid == 0) || (ret != HI_SUCCESS)) {
            HI_FATAL_VENC("get encode license ret 0x%x , valid = %d, encode not allowed!\n", ret, license_valid);
            goto error_0;
        }

        venc_drv_init_event();
        ret = venc_drv_efl_open_vedu();
        if (ret != HI_SUCCESS) {
            HI_FATAL_VENC("vedu_efl_open_vedu failed, ret=%d\n", ret);
            goto error_0;
        }

        for (i = 0; i < HI_VENC_MAX_CHN; i++) {
            memset_s(&(g_venc_chn[i]), sizeof(optm_venc_chn), 0, sizeof(optm_venc_chn));
            g_venc_chn[i].venc_handle = NULL;
            g_venc_chn[i].user_handle = HI_INVALID_HANDLE;
        }

#ifdef VENC_SUPPORT_JPGE
        ret = hi_drv_module_get_func(HI_ID_JPGENC, (hi_void**)&g_jpge_func);
        if (ret != HI_SUCCESS) {
            HI_ERR_VENC("get_function from JPGE  failed.\n");
            goto error_0;
        }
        g_jpge_func->drv_jpge_module_open();
#endif
        if (osal_timer_init(&g_venc_timer) < 0) {
            HI_ERR_VENC("timer init failed!\n");
            goto error_0;
        }
        g_venc_timer.handler = venc_timer_func;
        osal_timer_set(&g_venc_timer, TIMER_INTERVAL_MS);
        g_venc_open_flag = 1;
    }
    osal_sem_up(&g_venc_mutex);

    return HI_SUCCESS;

error_0:
    osal_atomic_dec_return(&g_venc_count);
    osal_sem_up(&g_venc_mutex);

    return HI_FAILURE;

}

static hi_s32 venc_drv_close(struct inode *finode, struct file  *ffile)
{
    hi_u32 i = 0;
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        if ((g_venc_chn[i].which_file == ffile)
            && (g_venc_chn[i].venc_handle != NULL)) {
            HI_INFO_VENC("try venc_destroy_chn %d/%#p.\n", i, g_venc_chn[i].venc_handle);
            venc_drv_osal_give_event(&g_rc_event);
            g_venc_chn[i].venc_handle->abnormal_exit = HI_TRUE;
            ret = venc_drv_destroy_chn(g_venc_chn[i].venc_handle);
            if (ret != HI_SUCCESS) {
                HI_WARN_VENC("force destroy_chn %d failed, ret=%#x.\n", i, ret);
            }
            g_venc_chn[i].which_file = HI_NULL;
        }
    }

    if (osal_atomic_dec_return(&g_venc_count) == 0) {
        osal_timer_destory(&g_venc_timer);
        venc_drv_efl_close_vedu();
        venc_drv_deinit_event();

#ifdef VENC_SUPPORT_JPGE
        if (g_jpge_func) {
            g_jpge_func->drv_jpge_module_close();
        }
#endif

        /* venc_drv_board_deinit();  change to destroy channel */
        g_venc_open_flag = 0;
    }

    osal_sem_up(&g_venc_mutex);

    return HI_SUCCESS;
}

hi_s32 venc_drv_suspend(hi_void)
{
    hi_u32 i = 0;
    hi_s32 ret;

    if (!g_venc_open_flag) {
        HI_PRINT("VENC suspend OK\n");
        return 0;
    }

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        HI_INFO_VENC("suspend venc channel %d handle %p, invalid = %x\n", i, g_venc_chn[i].venc_handle,
            HI_INVALID_HANDLE);
        if (g_venc_chn[i].venc_handle != NULL && g_venc_chn[i].enable == 1) {

            ret = venc_drv_stop_receive_pic(g_venc_chn[i].venc_handle);
            if (ret != HI_SUCCESS) {
                HI_WARN_VENC("venc_stop_receive_pic %d failed, ret=%#x.\n", i, ret);
            }

            g_is_venc_chan_alive[i] = HI_TRUE;
        } else {
            g_is_venc_chan_alive[i] = HI_FALSE;
        }
    }

    if (osal_atomic_dec_return(&g_venc_count) == 0) {
        venc_drv_efl_suspend_vedu();
    }

    HI_PRINT("VENC suspend OK\n");
    osal_sem_up(&g_venc_mutex);
    return HI_SUCCESS;
}

hi_s32 venc_drv_resume(hi_void)
{
    hi_s32 ret, i;
    hi_ulong ptaddr = 0;
    hi_ulong err_rdaddr = 0;
    hi_ulong err_wraddr = 0;

    if (!g_venc_open_flag) {
        HI_PRINT("VENC resume OK\n");
        return 0;
    }

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    if (osal_atomic_inc_return(&g_venc_count) == 1) {
        /* venc_drv_board_init();   ==>rmove to the */
        venc_drv_init_event();
        ret = venc_drv_efl_resume_vedu();
        if (ret != HI_SUCCESS) {
            HI_FATAL_VENC("vedu_efl_open_vedu failed, ret=%d\n", ret);
            osal_atomic_dec_return(&g_venc_count);
            osal_sem_up(&g_venc_mutex);
            return HI_FAILURE;
        }
    }

    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        if (g_is_venc_chan_alive[i]) {
#ifdef HI_SMMU_SUPPORT
            hi_drv_nssmmu_get_page_table_addr((hi_ulong *)&ptaddr, (hi_ulong *)&err_rdaddr, (hi_ulong *)&err_wraddr);
            g_venc_chn[i].venc_handle->hal.smmu_page_base_addr = ptaddr;
            g_venc_chn[i].venc_handle->hal.smmu_err_read_addr = err_rdaddr;
            g_venc_chn[i].venc_handle->hal.smmu_err_write_addr = err_wraddr;
#endif
            HI_INFO_VENC(" h %d, fr %d. gop %d\n ", g_venc_chn[i].chn_user_cfg.config.height,
                g_venc_chn[i].chn_user_cfg.config.target_frame_rate, g_venc_chn[i].chn_user_cfg.config.gop);
            ret = venc_drv_start_receive_pic(g_venc_chn[i].venc_handle);
            if (ret != HI_SUCCESS) {
                HI_FATAL_VENC(KERN_ERR "resume venc_start_receive_pic %d failed.\n", i);
                continue;
            }
        }
    }

    HI_PRINT("VENC resume OK\n");
    osal_sem_up(&g_venc_mutex);
    return HI_SUCCESS;
}

static hi_s32 venc_ioctl_create_chan(struct file *file, hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;

    venc_ioctl_create *create_info = (venc_ioctl_create *)arg;
    vedu_efl_enc_para* kern_venc_chn = NULL;
    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    ret = venc_drv_create_chn(&kern_venc_chn, &create_info->attr, &create_info->strm_buf_info,
        create_info->omx_chan, file);

    if (kern_venc_chn == HI_NULL) {
        osal_sem_up(&g_venc_mutex);

        return ret;
    }

    D_VENC_GET_CHN(index, kern_venc_chn);

    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CREATE_ERR;
    }
    g_venc_chn[index].user_handle = GET_VENC_CHHANDLE(index);
    create_info->chan_id = g_venc_chn[index].user_handle;
    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_destroy(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    hi_handle *ph_venc_chn = (hi_handle *)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, *ph_venc_chn);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_destroy_chn(g_venc_chn[index].venc_handle);

    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_attach_input(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_attach *attach_info = (venc_ioctl_attach*)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, attach_info->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_attach_input(g_venc_chn[index].venc_handle, attach_info->source_handle, attach_info->mode_id);
    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_detach_input(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_attach *attach_info = (venc_ioctl_attach*)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, attach_info->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    attach_info->mode_id = (hi_mod_id)((g_venc_chn[index].source & 0xff0000) >> 16); /* 16 is left move size */
    ret = venc_drv_detach_input(g_venc_chn[index].venc_handle, g_venc_chn[index].source, attach_info->mode_id);
    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_set_src(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_set_source *set_src = (venc_ioctl_set_source*)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, set_src->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_set_src_info(g_venc_chn[index].venc_handle, &(set_src->venc_source));
    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_acq_stream(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_acquire_stream *acq_strm = (venc_ioctl_acquire_stream*)arg;
    vedu_efl_enc_para* temp_handle = NULL;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, acq_strm->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }
    temp_handle = g_venc_chn[index].venc_handle;
    osal_sem_up(&g_venc_mutex);

    ret = venc_drv_quire_stream(temp_handle,
        &(acq_strm->stream),
        acq_strm->block_flag,
        &(acq_strm->buf_offset));

    return ret;
}

static hi_s32 venc_ioctl_release_stream(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_acquire_stream *acq_strm = (venc_ioctl_acquire_stream*)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, acq_strm->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_release_stream(g_venc_chn[index].venc_handle, acq_strm);
    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_start_recv_pic(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    hi_handle *handle = (hi_handle*)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, *handle);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_start_receive_pic(g_venc_chn[index].venc_handle);
    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_stop_recv_pic(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    hi_handle *handle = (hi_handle*)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, *handle);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_stop_receive_pic(g_venc_chn[index].venc_handle);
    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_get_chn_attr(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_create *create_info = (venc_ioctl_create *)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, create_info->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_get_attr(g_venc_chn[index].venc_handle, &(create_info->attr));
    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_request_i_frame(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    hi_handle *handle = (hi_handle*)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, *handle);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    if (g_venc_chn[index].jpge == HI_INVALID_HANDLE) {
        ret = venc_drv_request_i_frame(g_venc_chn[index].venc_handle);
    }
    osal_sem_up(&g_venc_mutex);

    return ret;
}


static hi_s32 venc_ioctl_que_frame(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_queue_frame *queue_frame_info = (venc_ioctl_queue_frame *)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, queue_frame_info->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);
        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    if (g_venc_chn[index].omx_chn) {
        ret = venc_drv_queue_frame_omx(g_venc_chn[index].venc_handle, &(queue_frame_info->venc_frame_omx));
    } else {
        ret = venc_drv_queue_frame(g_venc_chn[index].venc_handle, &queue_frame_info->venc_frame);
    }

    osal_sem_up(&g_venc_mutex);

    return ret;
}


static hi_s32 venc_ioctl_dequeue_frame(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_queue_frame *queue_frame_info = (venc_ioctl_queue_frame*)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, queue_frame_info->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_dequeue_frame(g_venc_chn[index].venc_handle, &(queue_frame_info->venc_frame));

    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_get_capability(hi_void *arg)
{
    hi_s32 ret;
    hi_venc_cap *_capability = (hi_venc_cap*)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    ret = venc_drv_get_capability(_capability);
    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_set_rate_control_type(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_rate_control *rc_type = (venc_ioctl_rate_control *)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, rc_type->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    if (g_venc_chn[index].chn_user_cfg.standard == HI_VENC_STD_JPEG) {
        HI_ERR_VENC("current channel(%d) encode type = %d, not support set rate_control type(%d)!\n",
                    index, g_venc_chn[index].chn_user_cfg.standard, rc_type->rc_type);
        ret = HI_ERR_VENC_NOT_SUPPORT;
    } else {
        ret = venc_drv_set_rate_control_type(g_venc_chn[index].venc_handle, rc_type->rc_type);
    }

    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_get_rate_control_type(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_rate_control *rc_type = (venc_ioctl_rate_control *)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, rc_type->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    if (g_venc_chn[index].chn_user_cfg.standard == HI_VENC_STD_JPEG) {
        HI_ERR_VENC("current channel(%d) encode type = %d not support get rate_control type\n",
                    index, g_venc_chn[index].chn_user_cfg.standard, rc_type->rc_type);
        rc_type->rc_type = HI_VENC_CONTROL_RATE_MAX;
        ret = HI_ERR_VENC_NOT_SUPPORT;
    } else {
        ret = venc_drv_get_rate_control_type(g_venc_chn[index].venc_handle, &rc_type->rc_type);
    }

    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_set_in_frm_rate_type(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_frame_rate_type *in_frm_rate_type = (venc_ioctl_frame_rate_type *)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, in_frm_rate_type->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_set_in_frm_rate_type(g_venc_chn[index].venc_handle, in_frm_rate_type->frm_rate_type);

    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_get_in_frm_rate_type(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_frame_rate_type *in_frm_rate_type = (venc_ioctl_frame_rate_type *)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, in_frm_rate_type->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_get_in_frm_rate_type(g_venc_chn[index].venc_handle, &in_frm_rate_type->frm_rate_type);

    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_get_message(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_get_msg *message_info   = (venc_ioctl_get_msg *)arg;
    vedu_efl_enc_para* temp_handle = NULL;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, message_info->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    temp_handle = g_venc_chn[index].venc_handle;
    osal_sem_up(&g_venc_mutex);

    ret = venc_drv_get_message_omx(temp_handle, &(message_info->msg_info_omx));

    return ret;
}

static hi_s32 venc_ioctrl_mmz_map(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_mmz_phy *mmz_phy_info = (venc_ioctl_mmz_phy *)arg;
    venc_ioctl_mmz_map mmb_info;

    mmb_info.dir = mmz_phy_info->dir;
    mmb_info.buf_handle = mmz_phy_info->buffer_handle;
    mmb_info.buf_size = mmz_phy_info->buf_size;

    D_VENC_GET_CHN_BY_UHND(index, mmz_phy_info->chan_id);
    D_VENC_CHECK_CHN(index);

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    ret = venc_drv_mmz_map_omx(g_venc_chn[index].venc_handle, &mmb_info);
    osal_sem_up(&g_venc_mutex);
    if (HI_SUCCESS == ret) {
        mmz_phy_info->vir_to_phy_offset = (hi_size_t)(uintptr_t)mmb_info.virt_addr -
            (hi_size_t)(uintptr_t)mmb_info.phys_addr;
    }
    mmz_phy_info->phy_addr = mmb_info.phys_addr;

    return ret;
}

static hi_s32 venc_ioctl_mmz_ummap(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_mmz_phy* mmz_phy_info = (venc_ioctl_mmz_phy*)arg;
    venc_ioctl_mmz_map mmb_info;

    mmb_info.dir = mmz_phy_info->dir;
    mmb_info.buf_handle = mmz_phy_info->buffer_handle;
    mmb_info.buf_size = mmz_phy_info->buf_size;
    D_VENC_GET_CHN_BY_UHND(index, mmz_phy_info->chan_id);
    D_VENC_CHECK_CHN(index);

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    ret = venc_drv_mmz_ummap_omx(g_venc_chn[index].venc_handle, &mmb_info);
    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_queue_stream(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_queue_frame *que_strm = (venc_ioctl_queue_frame*)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, que_strm->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_queue_stream_omx(g_venc_chn[index].venc_handle, &(que_strm->venc_frame_omx));
    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctrl_flush_port(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_flush_port *flush_port = (venc_ioctl_flush_port*)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, flush_port->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_flush_port_omx(g_venc_chn[index].venc_handle, flush_port->port_index, flush_port->inter_flush);
    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_rc_get_task(hi_void *arg)
{
    hi_s32 ret;

    venc_ioctl_rc_info *rc_info = (venc_ioctl_rc_info *)arg;

    ret = rc_client_get_task(rc_info);

    return ret;
}

static hi_s32 venc_ioctl_rc_set_result(hi_void *arg)
{
    hi_s32 ret;

    venc_ioctl_rc_info *rc_info = (venc_ioctl_rc_info *)arg;
    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    ret = rc_client_set_result(rc_info);

    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_set_config(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index;
    venc_ioctl_chan_info *chan_info = (venc_ioctl_chan_info *)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, chan_info->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);
        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_set_config(g_venc_chn[index].venc_handle, &chan_info->config);
    osal_sem_up(&g_venc_mutex);

    return ret;
}

static hi_s32 venc_ioctl_get_config(hi_void *arg)
{
    hi_s32 ret;
    hi_u32 index = 0;
    venc_ioctl_chan_info *chan_info = (venc_ioctl_chan_info *)arg;

    ret = osal_sem_down_interruptible(&g_venc_mutex);
    if (ret != HI_SUCCESS) {
        HI_WARN_VENC("call osal_sem_down_interruptible err!\n");
        return ret;
    }

    D_VENC_GET_CHN_BY_UHND(index, chan_info->chan_id);
    if (index >= HI_VENC_MAX_CHN) {
        osal_sem_up(&g_venc_mutex);

        return HI_ERR_VENC_CHN_NOT_EXIST;
    }

    ret = venc_drv_get_config(g_venc_chn[index].venc_handle, &(chan_info->config));
    osal_sem_up(&g_venc_mutex);

    return ret;
}

hi_slong venc_ioctl(struct file *file, unsigned int cmd, hi_void *arg)
{
    hi_slong ret = HI_FAILURE;

    if (arg == NULL) {
        HI_WARN_VENC("Arg is null!\n");
        return HI_ERR_VENC_INVALID_PARA;
    }

    switch (cmd) {
        case CMD_VENC_CREATE_CHN:
            ret = venc_ioctl_create_chan(file, arg);
            break;

        case CMD_VENC_DESTROY_CHN:
            ret = venc_ioctl_destroy(arg);
            break;

        case CMD_VENC_ATTACH_INPUT:
            ret = venc_ioctl_attach_input(arg);
            break;

        case CMD_VENC_DETACH_INPUT:
            ret = venc_ioctl_detach_input(arg);
            break;

        case CMD_VENC_SET_SRC:
            ret = venc_ioctl_set_src(arg);
            break;

        case CMD_VENC_ACQUIRE_STREAM:
            ret = venc_ioctl_acq_stream(arg);
            break;

        case CMD_VENC_RELEASE_STREAM:
            ret = venc_ioctl_release_stream(arg);
            break;

        case CMD_VENC_START_RECV_PIC:
            ret = venc_ioctl_start_recv_pic(arg);
            break;

        case CMD_VENC_STOP_RECV_PIC:
            ret = venc_ioctl_stop_recv_pic(arg);
            break;

        case CMD_VENC_GET_CHN_ATTR:
            ret = venc_ioctl_get_chn_attr(arg);
            break;

        case CMD_VENC_REQUEST_I_FRAME:
            ret = venc_ioctl_request_i_frame(arg);
            break;

        case CMD_VENC_QUEUE_FRAME:                 /* both omxvenc & venc use!! */
            ret = venc_ioctl_que_frame(arg);
            break;

        case CMD_VENC_DEQUEUE_FRAME:
            ret = venc_ioctl_dequeue_frame(arg);
            break;

        case CMD_VENC_GET_CAP:
            ret = venc_ioctl_get_capability(arg);
            break;

        case CMD_VENC_GET_RC_TYPE:
            ret = venc_ioctl_get_rate_control_type(arg);
            break;

        case CMD_VENC_SET_RC_TYPE:
            ret = venc_ioctl_set_rate_control_type(arg);
            break;

        case CMD_VENC_GET_INPUT_FRMRATE_TYPE:
            ret = venc_ioctl_get_in_frm_rate_type(arg);
            break;

        case CMD_VENC_SET_INPUT_FRMRATE_TYPE:
            ret = venc_ioctl_set_in_frm_rate_type(arg);
            break;

        case CMD_VENC_GET_MSG:
            ret = venc_ioctl_get_message(arg);
            break;

        case CMD_VENC_MMZ_MAP:
            ret = venc_ioctrl_mmz_map(arg);
            break;

        case CMD_VENC_MMZ_UMMAP:
            ret = venc_ioctl_mmz_ummap(arg);
            break;

        case CMD_VENC_QUEUE_STREAM:                    /* just omxvenc use this api */
            ret = venc_ioctl_queue_stream(arg);
            break;

        case CMD_VENC_FLUSH_PORT:
            ret = venc_ioctrl_flush_port(arg);
            break;

        case CMD_VENC_RC_GET_TASK:
            ret = venc_ioctl_rc_get_task(arg);
            break;

        case CMD_VENC_RC_SET_RESULT:
            ret = venc_ioctl_rc_set_result(arg);
            break;

        case CMD_VENC_SET_CHN_CFG:
            ret = venc_ioctl_set_config(arg);
            break;

        case CMD_VENC_GET_CHN_CFG:
            ret = venc_ioctl_get_config(arg);
            break;

        default:
            HI_ERR_VENC("venc cmd unknown:%x\n", cmd);
            break;
    }

    return ret;
}


static long venc_drv_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    long ret;

    ret = (long)hi_drv_user_copy(file, cmd, arg, venc_ioctl);
    return ret;
}

static struct file_operations g_venc_ops = {
    .owner            = THIS_MODULE,
    .open            = venc_drv_open,
    .unlocked_ioctl = venc_drv_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl   = venc_drv_ioctl,
#endif
    .release        = venc_drv_close,
};

static hi_s32 venc_dev_setup(venc_dev *device)
{
    hi_s32 ret;
    struct device *dev = HI_NULL;
    struct class *cls;

    cls = class_create(THIS_MODULE, "venc_class");
    if (IS_ERR(cls)) {
        return HI_FAILURE;
    }

    ret = alloc_chrdev_region(&device->no, 0, 1, "hisi_venc");
    if (ret != 0) {
        goto destroy_class;
    }

    dev = device_create(cls, HI_NULL, device->no, HI_NULL, VENC_DEV_NAME);
    if (IS_ERR(dev)) {
        goto unregister_region;
    }

    cdev_init(&device->cdev, &g_venc_ops);
    device->cdev.owner = THIS_MODULE;
    device->cdev.ops = &g_venc_ops;
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

static hi_s32 venc_dev_clean(venc_dev *device)
{
    if (device->setup == HI_FALSE) {        return HI_SUCCESS;
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

static hi_s32 venc_dev_probe(struct platform_device *plt_dev)
{
    hi_s32 ret;
    venc_dev_entry *entry = venc_dev_get_entry();
    venc_dev *device = &entry->device;

    HI_INFO_VENC("prepare to probe venc.\n");

    platform_set_drvdata(plt_dev, HI_NULL);

    ret = venc_dev_setup(device);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    device->dev = &plt_dev->dev;
    platform_set_drvdata(plt_dev, device);

    if (osal_atomic_init(&g_venc_count) < 0) {
        return HI_FAILURE;
    }
    osal_atomic_set(&g_venc_count, 0);

    if (osal_sem_init(&g_venc_mutex, 1) < 0) {
        return HI_FAILURE;
    }

    if (osal_sem_init(&g_venc_drv_mutex, 1) < 0) {
        return HI_FAILURE;
    }

    HI_INFO_VENC("probe venc success.\n");

    return HI_SUCCESS;
}

static hi_s32 venc_dev_remove(struct platform_device *plt_dev)
{
    venc_dev *device = HI_NULL;
    hi_s32 ret;
    HI_INFO_VENC("prepare to remove venc.\n");

    device = platform_get_drvdata(plt_dev);
    if (IS_ERR(device)) {
        return HI_FAILURE;
    }

    ret = venc_dev_clean(device);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("venc_dev_clean FAIL \n");
        return ret;
    }
    platform_set_drvdata(plt_dev, HI_NULL);

    osal_atomic_destory(&g_venc_count);
    osal_sem_destory(&g_venc_mutex);
    osal_sem_destory(&g_venc_drv_mutex);

    HI_INFO_VENC("remove venc success.\n");

    return HI_SUCCESS;
}

static hi_s32 venc_dev_suspend(struct platform_device *plt_dev, pm_message_t state)
{
    return venc_drv_suspend();
}

static hi_s32 venc_dev_resume(struct platform_device *plt_dev)
{
    return venc_drv_resume();
}

static hi_void venc_dev_release(struct device *dev)
{
    return;
}

static struct platform_driver g_venc_driver = {
    .probe   = venc_dev_probe,
    .remove  = venc_dev_remove,
    .suspend = venc_dev_suspend,
    .resume  = venc_dev_resume,
    .driver  = {
        .name  = VENC_DEV_NAME,
        .owner = THIS_MODULE,
    },
};

static struct platform_device g_venc_device = {
    .name = VENC_DEV_NAME,
    .id = -1,
    .dev = {
        .platform_data = NULL,
        .release = venc_dev_release,
    },
};

hi_s32 drv_venc_init_module(hi_void)
{
    hi_s32 ret;

    ret = venc_drv_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("init drv fail!\n");
        return HI_FAILURE;
    }

    ret = platform_device_register(&g_venc_device);
    if (ret < 0) {
        return ret;
    }

    ret = platform_driver_register(&g_venc_driver);
    if (ret < 0) {
        platform_device_unregister(&g_venc_device);
        return ret;
    }

#ifdef HI_TEE_SUPPORT
    hi_drv_ssm_iommu_config(LOGIC_MOD_ID_VENC);
#endif

    HI_PRINT("load hi_venc.ko success.\n");

    return HI_SUCCESS;
}

hi_void drv_venc_exit_module(hi_void)
{
    platform_driver_unregister(&g_venc_driver);
    platform_device_unregister(&g_venc_device);

    venc_drv_exit();

    HI_PRINT("unload hi_venc.ko success.\n");
}

hi_void record_state_info_to_chan(vedu_state_info *stat_info, hi_u32 i)
{
    /* video encoder does frame rate control by this value */
    g_venc_chn[i].last_sec_encoded_fps = stat_info->get_frame_num_ok - stat_info->quick_encode_skip - \
                                            stat_info->err_cfg_skip - stat_info->frm_rc_ctrl_skip - \
                                            stat_info->same_pts_skip - \
                                            stat_info->too_few_buffer_skip - stat_info->too_many_bits_skip - \
                                            g_venc_chn[i].frame_num_last_encoded;
    g_venc_chn[i].last_sec_input_fps = stat_info->get_frame_num_ok - g_venc_chn[i].frame_num_last_input;
    g_venc_chn[i].last_sec_kbps = stat_info->stream_total_byte - g_venc_chn[i].total_byte_last_encoded;
    g_venc_chn[i].last_sec_try_num = stat_info->get_frame_num_try - g_venc_chn[i].last_try_num_total;
    g_venc_chn[i].last_sec_ok_num = stat_info->get_frame_num_ok - g_venc_chn[i].last_ok_num_total;
    g_venc_chn[i].last_sec_put_num = stat_info->put_frame_num_ok - g_venc_chn[i].last_put_num_total;
    /* save value for next calculation */
    g_venc_chn[i].frame_num_last_input    = stat_info->get_frame_num_ok;
    g_venc_chn[i].frame_num_last_encoded  = stat_info->get_frame_num_ok - stat_info->quick_encode_skip -
                                            stat_info->err_cfg_skip - \
                                            stat_info->frm_rc_ctrl_skip - stat_info->same_pts_skip - \
                                            stat_info->too_few_buffer_skip - stat_info->too_many_bits_skip ;
    g_venc_chn[i].total_byte_last_encoded = stat_info->stream_total_byte;
    g_venc_chn[i].last_try_num_total      = stat_info->get_frame_num_try;
    g_venc_chn[i].last_ok_num_total       = stat_info->get_frame_num_ok;
    g_venc_chn[i].last_put_num_total      = stat_info->put_frame_num_ok;
}

hi_void record_water_line_to_chan(vedu_efl_enc_para *enc_chan_para,  hi_u32 i)
{
    hi_u32 last_sec_bits;
    hi_u32 bit_rate;
    hi_s32 rc_ratio = 0;
    if (g_venc_chn[i].src_mod_id != HI_ID_MAX) {
        enc_chan_para->rc.water_line = enc_chan_para->rc.water_line_init;
    }

    last_sec_bits = g_venc_chn[i].last_sec_kbps * 8; /* 8: byte to bit */
    bit_rate = enc_chan_para->rc.bit_rate;

    if (enc_chan_para->rc_dbg_print == 1) {
        if (bit_rate >= last_sec_bits) {
            rc_ratio = (bit_rate - last_sec_bits) * 100 / (bit_rate); /* 100: calculate ratio */
            rc_ratio = (-1) * rc_ratio;
        } else {
            rc_ratio = (last_sec_bits - bit_rate) * 100 / (bit_rate); /* 100: calculate ratio */
        }

        HI_ERR_VENC("Targetbitrate is %d, CurBitRate is %d, FrameRate is %d, rc_ratio is %d%%\n",
                    bit_rate, last_sec_bits, g_venc_chn[i].last_sec_encoded_fps, rc_ratio);
    }
}

hi_void venc_timer_func(hi_length_t value)
{
    hi_u32 i = 0;
    hi_s32 ret = HI_FAILURE;
    vedu_state_info stat_info;
    vedu_efl_enc_para *enc_chan_para = NULL;
    unsigned long flags;

    for (i = 0; i < HI_VENC_MAX_CHN; i++) {
        osal_spin_lock_irqsave(&g_send_frame_lock[i], &flags);

        if ((g_venc_chn[i].venc_handle != NULL) && (venc_drv_check_chn_state_safe(i) == HI_TRUE)) {
            ret = venc_drv_efl_query_stat_info(g_venc_chn[i].venc_handle, &stat_info);
            if (ret == HI_SUCCESS) {
                record_state_info_to_chan(&stat_info, i);
                enc_chan_para = (vedu_efl_enc_para*)g_venc_chn[i].venc_handle;
                D_VENC_UPDATE_2(enc_chan_para->last_sec_input_frm_rate, g_venc_chn[i].last_sec_input_fps);
                enc_chan_para->rc.last_sec_frame_cnt = 0;
                enc_chan_para->rc.frm_rate_sata_error_flag = 0;

                record_water_line_to_chan(enc_chan_para, i);
            }
        }
        osal_spin_unlock_irqrestore(&g_send_frame_lock[i], &flags);
    }

    g_venc_timer.handler = venc_timer_func;
    osal_timer_set(&g_venc_timer, TIMER_INTERVAL_MS);

    return;
}

#ifdef MODULE
module_init(drv_venc_init_module);
module_exit(drv_venc_exit_module);
#else
EXPORT_SYMBOL(drv_venc_init_module);
EXPORT_SYMBOL(drv_venc_exit_module);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");
