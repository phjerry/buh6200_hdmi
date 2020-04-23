/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drv adec
 * Author: Audio
 * Create: 2012-05-30
 * Notes: NA
 * History: 2019-05-30 CSEC Rectification
 */

#include "hi_osal.h"

#include "hi_drv_dev.h"

#include "drv_ao_ext.h"
#include "drv_adec_ext.h"
#include "hi_drv_adec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define ADEC_NAME "HI_ADEC"
#define ADEC_PROC_NAME_LEN 16
#define ADEC_PERCENTAGE 100

typedef struct {
    hi_u32 id;
    adec_proc_item *proc_addr;
    hi_audio_buffer proc_mmz;
    struct osal_list_head list;
} adec_instance;

static OSAL_LIST_HEAD(g_adec_list);
static osal_semaphore g_adec_mutex = {HI_NULL};

#ifdef HI_PROC_SUPPORT
static hi_void adec_unreg_proc(adec_instance *adec);
static hi_void adec_reg_proc(adec_instance *adec);
#endif

static adec_instance *adec_find_by_id(hi_u32 id)
{
    adec_instance *adec = HI_NULL;

    osal_list_for_each_entry(adec, &g_adec_list, list) {
        if (adec->id == id) {
            return adec;
        }
    }

    return HI_NULL;
}

static inline hi_u32 adec_find_free_id(hi_void)
{
    hi_u32 id;

    for (id = 0; id < ADEC_INSTANCE_MAXNUM; id++) {
        if (adec_find_by_id(id) == HI_NULL) {
            return id;
        }
    }

    return ADEC_INSTANCE_MAXNUM;
}

static hi_void adec_destroy_instance(adec_instance *adec)
{
    osal_list_del(&adec->list);
    osal_kfree(HI_ID_ADEC, adec);
}

static adec_instance *adec_create_instance(hi_void)
{
    hi_u32 id;
    hi_s32 ret;
    adec_instance *adec = HI_NULL;

    id = adec_find_free_id();
    if (id >= ADEC_INSTANCE_MAXNUM) {
        HI_ERR_ADEC("adec_find_free_id failed\n");
        return HI_NULL;
    }

    adec = (adec_instance *)osal_kmalloc(HI_ID_ADEC, sizeof(*adec), GFP_KERNEL);
    if (adec == HI_NULL) {
        HI_ERR_ADEC("osal_kmalloc adec_instance failed\n");
        return HI_NULL;
    }

    ret = memset_s(adec, sizeof(*adec), 0, sizeof(*adec));
    if (ret != EOK) {
        HI_ERR_ADEC("call memset_s failed(0x%x)\n", ret);
        osal_kfree(HI_ID_ADEC, adec);
        return HI_NULL;
    }

    adec->id = id;
    osal_list_add(&adec->list, &g_adec_list);

    return adec;
}

static audio_export_func *adec_get_audio_mem_func(hi_void)
{
    hi_s32 ret;
    audio_export_func *mem_func = HI_NULL;

    ret = osal_exportfunc_get(HI_ID_AIAO, (hi_void **)&mem_func);
    if (ret != HI_SUCCESS) {
        HI_FATAL_ADEC("get audio mem function failed(0x%x)\n", ret);
        return HI_NULL;
    }

    if (mem_func == HI_NULL ||
        mem_func->mmz_alloc == HI_NULL ||
        mem_func->mmz_release == HI_NULL) {
        HI_FATAL_ADEC("audio mem function is invalid\n");
        return HI_NULL;
    }

    return mem_func;
}

static hi_s32 adec_proc_buf_init(adec_instance *adec)
{
    hi_s32 ret;
    audio_export_func *mem_func = adec_get_audio_mem_func();
    hi_char name[ADEC_PROC_NAME_LEN] = {0};

    if (mem_func == HI_NULL) {
        return HI_ERR_ADEC_NULL_PTR;
    }

    snprintf(name, sizeof(name), "%s%02d", "adec_proc", adec->id);

    ret = mem_func->mmz_alloc(name,
        sizeof(adec_proc_item), HI_FALSE, &adec->proc_mmz);
    if (ret != HI_SUCCESS) {
        HI_ERR_ADEC("call mmz_alloc failed(0x%x)\n", ret);
        return HI_FAILURE;
    }

    adec->proc_addr = (adec_proc_item *)adec->proc_mmz.virt_addr;
    adec->proc_addr->pcm_ctrl_state = ADEC_CMD_CTRL_STOP;
    adec->proc_addr->save_pcm_cnt = 0;
    adec->proc_addr->es_ctrl_state = ADEC_CMD_CTRL_STOP;
    adec->proc_addr->save_es_cnt = 0;

    return HI_SUCCESS;
}

static hi_void adec_proc_buf_deinit(adec_instance *adec)
{
    audio_export_func *mem_func = adec_get_audio_mem_func();
    if (mem_func == HI_NULL) {
        return;
    }

    if (adec->proc_addr != HI_NULL) {
        mem_func->mmz_release(&adec->proc_mmz);
    }
}

static hi_s32 adec_ioctl_proc_init(hi_void *private_data, hi_void *arg)
{
    hi_s32 ret;
    adec_instance *adec = HI_NULL;
    adec_proc_param* proc_param = (adec_proc_param *)arg;

    if (private_data == HI_NULL) {
        return HI_ERR_ADEC_NULL_PTR;
    }

    adec = adec_create_instance();
    if (adec == HI_NULL) {
        HI_ERR_ADEC("call adec_create_instance failed\n");
        return HI_FAILURE;
    }

    ret = adec_proc_buf_init(adec);
    if (ret != HI_SUCCESS) {
        HI_ERR_ADEC("call adec_proc_buf_init failed(0x%x)\n", ret);
        adec_destroy_instance(adec);
        return HI_FAILURE;
    }

#ifdef HI_PROC_SUPPORT
    adec_reg_proc(adec);
#endif

    *((adec_instance **)private_data) = adec;
    proc_param->id = adec->id;
    proc_param->map_fd = adec->proc_mmz.fd;
    proc_param->proc_phys_addr = adec->proc_mmz.phys_addr;

    return HI_SUCCESS;
}

static hi_s32 adec_ioctl_proc_deinit(hi_void *private_data)
{
    adec_instance *adec = HI_NULL;

    if (private_data == HI_NULL) {
        return HI_ERR_ADEC_NULL_PTR;
    }

    adec = *((adec_instance **)private_data);
    if (adec == HI_NULL) {
        return HI_SUCCESS;
    }

#ifdef HI_PROC_SUPPORT
    adec_unreg_proc(adec);
#endif
    adec_proc_buf_deinit(adec);
    adec_destroy_instance(adec);

    *((adec_instance **)private_data) = HI_NULL;
    return HI_SUCCESS;
}

static hi_s32 adec_drv_open(hi_void *private_data)
{
    return HI_SUCCESS;
}

static hi_s32 adec_drv_release(hi_void *private_data)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_adec_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_ADEC("lock g_adec_mutex failed\n");
        return ret;
    }

    ret = adec_ioctl_proc_deinit(private_data);
    if (ret != HI_SUCCESS) {
        HI_ERR_ADEC("adec_ioctl_proc_deinit failed!\n");
    }

    osal_sem_up(&g_adec_mutex);
    return ret;
}

static hi_s32 adec_process_cmd(hi_void *private_data, hi_u32 cmd, hi_void *arg)
{
    hi_s32 ret;

    switch (cmd) {
        case DRV_ADEC_PROC_INIT: {
            ret = adec_ioctl_proc_init(private_data, arg);
            if (ret != HI_SUCCESS) {
                HI_ERR_ADEC("adec_ioctl_proc_init failed!\n");
            }
            break;
        }

        case DRV_ADEC_PROC_EXIT: {
            ret = adec_ioctl_proc_deinit(private_data);
            if (ret != HI_SUCCESS) {
                HI_ERR_ADEC("adec_ioctl_proc_deinit failed!\n");
            }
            break;
        }

        default: {
            ret = HI_FAILURE;
            HI_ERR_ADEC("invalid cmd!\n");
            break;
        }
    }

    return ret;
}

static hi_s32 adec_drv_ioctl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_adec_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_ADEC("lock g_adec_mutex failed(0x%x)\n", ret);
        return ret;
    }

    ret = adec_process_cmd(private_data, cmd, arg);

    osal_sem_up(&g_adec_mutex);
    return ret;
}

#ifdef HI_PROC_SUPPORT
static hi_s32 adec_proc_how_help(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    adec_instance *adec = (adec_instance *)private;
    if (adec == HI_NULL) {
        return HI_ERR_ADEC_NULL_PTR;
    }

    osal_printk("\nfunction: save es data before audio decode\n");
    osal_printk("commad:   echo save_es start|stop > /proc/msp/adec%02d\n", adec->id);
    osal_printk("example:  echo save_es start > /proc/msp/adec%02d\n", adec->id);
    osal_printk("\nfunction: save pcm data after audio decode\n");
    osal_printk("commad:   echo save_pcm start|stop > /proc/msp/adec%02d\n", adec->id);
    osal_printk("example:  echo save_pcm start > /proc/msp/adec%02d\n\n", adec->id);

    return HI_SUCCESS;
}

static hi_void adec_read_proc_decoder_info(hi_void *seq_file, adec_proc_item *adec_proc)
{
    osal_proc_print(seq_file, "%-40s:%s\n",
        "state", (adec_proc->adec_work_enable == HI_TRUE) ? "start" : "stop");
    osal_proc_print(seq_file, "%-40s:0x%x\n",
        "codec_id", adec_proc->codec_id);
    osal_proc_print(seq_file, "%-40s:%s\n",
        "decoder_name", adec_proc->codec_type);
    osal_proc_print(seq_file, "%-40s:%s\n",
        "description", adec_proc->codec_description);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "*decode_thread_id", adec_proc->thread_id);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "volume", adec_proc->volume);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "sample_rate", adec_proc->sample_rate);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "bit_depth", adec_proc->bit_width);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "channels", adec_proc->out_channels);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "*pcm_samples", adec_proc->pcm_samples_per_frame);
    osal_proc_print(seq_file, "%-40s:0x%x\n",
        "*bits_byte", adec_proc->bits_out_bytes_per_frame);
    osal_proc_print(seq_file, "%-40s:%s\n\n",
        "packet_decoder", (adec_proc->fmt == HI_TRUE) ? "Yes" : "No");
    osal_proc_print(seq_file, "%-40s:%d\n",
        "*try_decode_times", adec_proc->dbg_try_decode_count);
    osal_proc_print(seq_file, "%-40s:%d/%d\n",
        "frame_num(total/error)", adec_proc->framn_nm, adec_proc->err_frame_num);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "frame_unsupport_num", adec_proc->codec_unsupport_num);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "stream_corrupt_num", adec_proc->stream_corrupt_num);
}

static hi_void adec_read_proc_input_buf(hi_void *seq_file, adec_proc_item *adec_proc)
{
    hi_u32 data_size = 0;
    hi_u32 buf_percent = 0;

    if (adec_proc->buf_size != 0) {
        if (adec_proc->buf_write >= adec_proc->buf_read) {
            data_size = adec_proc->buf_write - adec_proc->buf_read;
        } else {
            data_size = adec_proc->buf_size - adec_proc->buf_read + adec_proc->buf_write;
        }
        buf_percent = data_size * ADEC_PERCENTAGE / adec_proc->buf_size;
    }

    osal_proc_print(seq_file, "%-40s:%d/%d/%u%%\n",
        "stream_buf(total/use/percent)(bytes)", adec_proc->buf_size, data_size, buf_percent);

    osal_proc_print(seq_file, "%-40s:%d/%d\n",
        "stream_buf(read_pos/write_pos)", adec_proc->buf_read, adec_proc->buf_write);
}

static hi_void adec_read_proc_output_buf(hi_void *seq_file, adec_proc_item *adec_proc)
{
    hi_u32 frame_percent = 0;
    hi_u32 frame_num = 0;

    if (adec_proc->frame_size != 0) {
        if (adec_proc->frame_write >= adec_proc->frame_read) {
            frame_num = adec_proc->frame_write - adec_proc->frame_read;
        } else {
            frame_num = adec_proc->frame_size - adec_proc->frame_read + adec_proc->frame_write;
        }

        frame_percent = frame_num * ADEC_PERCENTAGE / adec_proc->frame_size;
    }

    osal_proc_print(seq_file, "%-40s:%d/%d/%u%%\n",
        "out_frame_buf(total/use/percent)", adec_proc->frame_size, frame_num, frame_percent);
}

static hi_void adec_read_proc_dbg_count(hi_void *seq_file, adec_proc_item *adec_proc)
{
    osal_proc_print(seq_file, "%-40s:%d/%d\n",
        "get_buffer(try/ok)", adec_proc->dbg_get_buf_count_try, adec_proc->dbg_get_buf_count);
    osal_proc_print(seq_file, "%-40s:%d/%d\n",
        "put_buffer(try/ok)", adec_proc->dbg_put_buf_count_try, adec_proc->dbg_put_buf_count);
    osal_proc_print(seq_file, "%-40s:%d/%d\n",
        "send_stream(try/ok)", adec_proc->dbg_send_straem_count_try, adec_proc->dbg_send_straem_count);
    osal_proc_print(seq_file, "%-40s:%d/%d\n",
        "receive_frame(try/ok)", adec_proc->dbg_receive_frame_count_try, adec_proc->dbg_receive_frame_count);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "pts_lost_num", adec_proc->pts_lost);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "*decode_thread_exec_time_out_cnt", adec_proc->thread_exe_time_out_cnt);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "*decode_thread_sche_time_out_cnt", adec_proc->thread_sche_time_out_cnt);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "*decode_thread_sleep_time_ms", adec_proc->adec_system_sleep_time);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "adec_delay_ms", adec_proc->adec_delay_ms);
}

static hi_s32 adec_read_proc(hi_void *seq_file, hi_void *private)
{
    adec_instance *adec = (adec_instance *)private;
    if (adec == HI_NULL) {
        return HI_ERR_ADEC_NULL_PTR;
    }

    if (adec->proc_addr == HI_NULL) {
        osal_proc_print(seq_file, "  ADEC[%02d] PROC not INIT\n", adec->id);
        return HI_SUCCESS;
    }

    osal_proc_print(seq_file, "\n---------------------------");
    osal_proc_print(seq_file, " ADEC[%02d] state ", adec->id);
    osal_proc_print(seq_file, "---------------------------\n\n");

    adec_read_proc_decoder_info(seq_file, adec->proc_addr);
    adec_read_proc_input_buf(seq_file, adec->proc_addr);
    adec_read_proc_output_buf(seq_file, adec->proc_addr);
    adec_read_proc_dbg_count(seq_file, adec->proc_addr);

    return HI_SUCCESS;
}

static hi_s32 adec_write_proc_save_es(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    adec_proc_item *adec_proc = HI_NULL;
    adec_instance *adec = (adec_instance *)private;
    if ((adec == HI_NULL) || (adec->proc_addr == HI_NULL)) {
        HI_ERR_ADEC("ADEC PROC not init\n");
        return HI_ERR_ADEC_NULL_PTR;
    }

    adec_proc = adec->proc_addr;

    if (osal_strncmp("save_es", strlen("save_es"), argv[0], strlen(argv[0])) != 0) {
        return HI_ERR_ADEC_INVALID_PARA;
    }

    if (osal_strncmp("start", strlen("start"), argv[1], strlen(argv[1])) == 0) {
        snprintf(adec_proc->es_file_path, sizeof(adec_proc->es_file_path),
                 "/mnt/adec%02d.es", adec->id);
        if (adec_proc->es_ctrl_state != ADEC_CMD_CTRL_START) {
            adec_proc->save_es_cnt++;
            adec_proc->es_ctrl_state = ADEC_CMD_CTRL_START;
            osal_printk("started saving adec es data to %s\n", adec_proc->es_file_path);
        } else {
            osal_printk("started saving adec es data already.\n");
        }

        return HI_SUCCESS;
    }

    if (osal_strncmp("stop", strlen("stop"), argv[1], strlen(argv[1])) == 0) {
        osal_printk("finished saving adec es data.\n");
        adec_proc->es_ctrl_state = ADEC_CMD_CTRL_STOP;
        return HI_SUCCESS;
    }

    return HI_ERR_ADEC_INVALID_PARA;
}

static hi_s32 adec_write_proc_save_pcm(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    adec_proc_item *adec_proc = HI_NULL;
    adec_instance *adec = (adec_instance *)private;
    if ((adec == HI_NULL) || (adec->proc_addr == HI_NULL)) {
        HI_ERR_ADEC("ADEC PROC not init\n");
        return HI_ERR_ADEC_NULL_PTR;
    }

    adec_proc = adec->proc_addr;

    if (osal_strncmp("save_pcm", strlen("save_pcm"), argv[0], strlen(argv[0])) != 0) {
        return HI_ERR_ADEC_INVALID_PARA;
    }

    if (osal_strncmp("start", strlen("start"), argv[1], strlen(argv[1])) == 0) {
        snprintf(adec_proc->pcm_file_path, sizeof(adec_proc->pcm_file_path),
                 "/mnt/adec%02d.pcm", adec->id);
        if (adec_proc->pcm_ctrl_state != ADEC_CMD_CTRL_START) {
            adec_proc->save_pcm_cnt++;
            adec_proc->pcm_ctrl_state = ADEC_CMD_CTRL_START;
            osal_printk("started saving adec pcm data to %s\n", adec_proc->pcm_file_path);
        } else {
            osal_printk("started saving adec pcm data already.\n");
        }

        return HI_SUCCESS;
    }

    if (osal_strncmp("stop", strlen("stop"), argv[1], strlen(argv[1])) == 0) {
        osal_printk("finished saving adec pcm data.\n");
        adec_proc->pcm_ctrl_state = ADEC_CMD_CTRL_STOP;
        return HI_SUCCESS;
    }

    return HI_ERR_ADEC_INVALID_PARA;
}

static osal_proc_cmd g_adec_proc_cmd[] = {
    { "save_es", adec_write_proc_save_es },
    { "save_pcm", adec_write_proc_save_pcm },
    { "help", adec_proc_how_help },
};

static hi_void adec_unreg_proc(adec_instance *adec)
{
    hi_char buf[ADEC_PROC_NAME_LEN] = { 0 };
    snprintf(buf, sizeof(buf), "adec%02d", adec->id);
    osal_proc_remove(buf, strlen(buf));
}

static hi_void adec_reg_proc(adec_instance *adec)
{
    hi_char buf[ADEC_PROC_NAME_LEN] = { 0 };
    osal_proc_entry *item = HI_NULL;

    /* create proc */
    snprintf(buf, sizeof(buf), "adec%02d", adec->id);
    item = osal_proc_add(buf, strlen(buf));
    if (item == HI_NULL) {
        HI_ERR_ADEC("call osal_proc_add failed\n");
        return;
    }

    /* set functions */
    item->read = adec_read_proc;
    item->cmd_list = g_adec_proc_cmd;
    item->cmd_cnt = sizeof(g_adec_proc_cmd) / sizeof(g_adec_proc_cmd[0]);
    item->private = adec;

    HI_INFO_ADEC("create adec proc entry OK!\n");
}
#endif

static osal_ioctl_cmd g_adec_cmd_list[] = {
    { DRV_ADEC_PROC_INIT, adec_drv_ioctl },
    { DRV_ADEC_PROC_EXIT, adec_drv_ioctl },
};

static osal_fileops g_adec_fops = {
    .open     = adec_drv_open,
    .write    = HI_NULL,
    .llseek   = HI_NULL,
    .release  = adec_drv_release,
    .poll     = HI_NULL,
    .mmap     = HI_NULL,
    .cmd_list = g_adec_cmd_list,
    .cmd_cnt  = sizeof(g_adec_cmd_list) / sizeof(g_adec_cmd_list[0]),
};

static osal_dev g_adec_dev = {
    .name   = HI_DEV_ADEC_NAME,
    .minor  = HI_DEV_ADEC_MINOR,
    .fops   = &g_adec_fops,
    .pmops  = HI_NULL,
    .dev    = HI_NULL,
};

hi_s32 adec_drv_mod_init(hi_void)
{
    hi_s32 ret;

    ret = osal_sem_init(&g_adec_mutex, 1);
    if (ret != HI_SUCCESS) {
        HI_ERR_ADEC("call osal_sem_init failed\n");
        return ret;
    }

    ret = osal_exportfunc_register(HI_ID_ADEC, ADEC_NAME, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_ADEC("call osal_exportfunc_register failed(0x%x)\n", ret);
        goto out0;
    }

    ret = osal_dev_register(&g_adec_dev);
    if (ret != HI_SUCCESS) {
        HI_FATAL_ADEC("call osal_dev_register failed(0x%x)\n", ret);
        goto out1;
    }

#ifdef MODULE
    HI_PRINT("Load hi_adec.ko success.  \t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;

out1:
    osal_exportfunc_unregister(HI_ID_ADEC);

out0:
    osal_sem_destory(&g_adec_mutex);
    return ret;
}

hi_void adec_drv_mod_exit(hi_void)
{
#ifndef HI_MCE_SUPPORT
    osal_exportfunc_unregister(HI_ID_ADEC);
#endif

    osal_dev_unregister(&g_adec_dev);
    osal_sem_destory(&g_adec_mutex);
}

#ifdef MODULE
module_init(adec_drv_mod_init);
module_exit(adec_drv_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
