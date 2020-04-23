/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drv aenc
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 CSEC Rectification
 */

#include "hi_osal.h"

#include "hi_drv_dev.h"

#include "drv_ao_ext.h"
#include "hi_drv_aenc.h"
#include "drv_aenc_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AENC_NAME "HI_AENC"

#define AENC_PROC_NAME_LENGTH 16
#define AENC_PERCENTAGE 100

typedef struct {
    hi_u32 id;
    aenc_proc_item *proc_addr;
    hi_audio_buffer proc_mmz;
    struct osal_list_head list;
} aenc_instance;

static OSAL_LIST_HEAD(g_aenc_list);
static osal_semaphore g_aenc_mutex = {HI_NULL};

#ifdef HI_PROC_SUPPORT
static hi_void aenc_unreg_proc(aenc_instance *aenc);
static hi_void aenc_reg_proc(aenc_instance *aenc);
#endif

static aenc_instance *aenc_find_by_id(hi_u32 id)
{
    aenc_instance *aenc = HI_NULL;

    osal_list_for_each_entry(aenc, &g_aenc_list, list) {
        if (aenc->id == id) {
            return aenc;
        }
    }

    return HI_NULL;
}

static inline hi_u32 aenc_find_free_id(hi_void)
{
    hi_u32 id;

    for (id = 0; id < AENC_INSTANCE_MAXNUM; id++) {
        if (aenc_find_by_id(id) == HI_NULL) {
            return id;
        }
    }

    return AENC_INSTANCE_MAXNUM;
}

static hi_void aenc_destroy_instance(aenc_instance *aenc)
{
    osal_list_del(&aenc->list);
    osal_kfree(HI_ID_AENC, aenc);
}

static aenc_instance *aenc_create_instance(hi_void)
{
    hi_u32 id;
    hi_s32 ret;
    aenc_instance *aenc = HI_NULL;

    id = aenc_find_free_id();
    if (id >= AENC_INSTANCE_MAXNUM) {
        HI_ERR_AENC("aenc_find_free_id failed\n");
        return HI_NULL;
    }

    aenc = (aenc_instance *)osal_kmalloc(HI_ID_AENC, sizeof(*aenc), GFP_KERNEL);
    if (aenc == HI_NULL) {
        HI_ERR_AENC("osal_kmalloc aenc_instance failed\n");
        return HI_NULL;
    }

    ret = memset_s(aenc, sizeof(*aenc), 0, sizeof(*aenc));
    if (ret != EOK) {
        HI_ERR_AENC("call memset_s failed(0x%x)\n", ret);
        osal_kfree(HI_ID_AENC, aenc);
        return HI_NULL;
    }

    aenc->id = id;
    osal_list_add(&aenc->list, &g_aenc_list);

    return aenc;
}

static audio_export_func *aenc_get_audio_mem_func(hi_void)
{
    hi_s32 ret;
    audio_export_func *mem_func = HI_NULL;

    ret = osal_exportfunc_get(HI_ID_AIAO, (hi_void **)&mem_func);
    if (ret != HI_SUCCESS) {
        HI_FATAL_AENC("get audio mem function failed(0x%x)\n", ret);
        return HI_NULL;
    }

    if (mem_func == HI_NULL ||
        mem_func->mmz_alloc == HI_NULL ||
        mem_func->mmz_release == HI_NULL) {
        HI_FATAL_AENC("audio mem function is invalid\n");
        return HI_NULL;
    }

    return mem_func;
}

static hi_s32 aenc_proc_buf_init(aenc_instance *aenc)
{
    hi_s32 ret;
    audio_export_func *mem_func = aenc_get_audio_mem_func();
    hi_char name[AENC_PROC_NAME_LENGTH] = {0};

    if (mem_func == HI_NULL) {
        return HI_ERR_AENC_NULL_PTR;
    }

    snprintf(name, sizeof(name), "%s%02d", "aenc_proc", aenc->id);

    ret = mem_func->mmz_alloc(name, sizeof(aenc_proc_item), HI_FALSE, &aenc->proc_mmz);
    if (ret != HI_SUCCESS) {
        HI_ERR_AENC("aenc alloc mem failed(0x%x)\n", ret);
        return HI_FAILURE;
    }

    aenc->proc_addr = (aenc_proc_item *)aenc->proc_mmz.virt_addr;
    aenc->proc_addr->pcm_ctrl_state = AENC_CMD_CTRL_STOP;
    aenc->proc_addr->save_pcm_cnt = 0;
    aenc->proc_addr->es_ctrl_state = AENC_CMD_CTRL_STOP;
    aenc->proc_addr->save_es_cnt = 0;

    return HI_SUCCESS;
}

static hi_void aenc_proc_buf_deinit(aenc_instance *aenc)
{
    audio_export_func *mem_func = aenc_get_audio_mem_func();
    if (mem_func == HI_NULL) {
        return;
    }

    if (aenc->proc_addr != HI_NULL) {
        mem_func->mmz_release(&aenc->proc_mmz);
    }
}

static hi_s32 aenc_ioctl_proc_init(hi_void *private_data, hi_void *arg)
{
    hi_s32 ret;
    aenc_instance *aenc = HI_NULL;
    aenc_proc_param* proc_param = (aenc_proc_param *)arg;

    if (private_data == HI_NULL) {
        return HI_ERR_AENC_NULL_PTR;
    }

    aenc = aenc_create_instance();
    if (aenc == HI_NULL) {
        HI_ERR_AENC("call aenc_create_instance failed\n");
        return HI_FAILURE;
    }

    ret = aenc_proc_buf_init(aenc);
    if (ret != HI_SUCCESS) {
        HI_ERR_AENC("call aenc_proc_buf_init failed(0x%x)\n", ret);
        aenc_destroy_instance(aenc);
        return HI_FAILURE;
    }

#ifdef HI_PROC_SUPPORT
    aenc_reg_proc(aenc);
#endif

    *((aenc_instance **)private_data) = aenc;
    proc_param->id = aenc->id;
    proc_param->map_fd = aenc->proc_mmz.fd;
    proc_param->proc_phys_addr = aenc->proc_mmz.phys_addr;

    return HI_SUCCESS;
}

static hi_s32 aenc_ioctl_proc_deinit(hi_void *private_data)
{
    aenc_instance *aenc = HI_NULL;

    if (private_data == HI_NULL) {
        return HI_ERR_AENC_NULL_PTR;
    }

    aenc = *((aenc_instance **)private_data);
    if (aenc == HI_NULL) {
        return HI_SUCCESS;
    }

#ifdef HI_PROC_SUPPORT
    aenc_unreg_proc(aenc);
#endif
    aenc_proc_buf_deinit(aenc);
    aenc_destroy_instance(aenc);

    *((aenc_instance **)private_data) = HI_NULL;
    return HI_SUCCESS;
}

static hi_s32 aenc_drv_open(hi_void *private_data)
{
    return HI_SUCCESS;
}

static hi_s32 aenc_drv_release(hi_void *private_data)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_aenc_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AENC("lock g_aenc_mutex failed\n");
        return ret;
    }

    ret = aenc_ioctl_proc_deinit(private_data);
    if (ret != HI_SUCCESS) {
        HI_ERR_AENC("aenc_ioctl_proc_deinit failed!\n");
    }

    osal_sem_up(&g_aenc_mutex);
    return ret;
}

static hi_s32 aenc_process_cmd(hi_void *private_data, hi_u32 cmd, hi_void *arg)
{
    hi_s32 ret;

    switch (cmd) {
        case DRV_AENC_PROC_INIT: {
            ret = aenc_ioctl_proc_init(private_data, arg);
            if (ret != HI_SUCCESS) {
                HI_ERR_AENC("aenc_ioctl_proc_init failed(0x%x)\n", ret);
            }
            break;
        }

        case DRV_AENC_PROC_EXIT: {
            ret = aenc_ioctl_proc_deinit(private_data);
            if (ret != HI_SUCCESS) {
                HI_ERR_AENC("aenc_ioctl_proc_deinit failed(0x%x)\n", ret);
            }
            break;
        }

        default:
        {
            ret = HI_FAILURE;
            HI_ERR_AENC("invalid cmd!\n");
            break;
        }
    }

    return ret;
}

static hi_s32 aenc_drv_ioctl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    ret = osal_sem_down_interruptible(&g_aenc_mutex);
    if (ret != HI_SUCCESS) {
        HI_ERR_AENC("lock g_aenc_mutex failed(0x%x)\n", ret);
        return ret;
    }

    ret = aenc_process_cmd(private_data, cmd, arg);

    osal_sem_up(&g_aenc_mutex);
    return ret;
}

#ifdef HI_PROC_SUPPORT
static hi_char *aenc_proc_get_attach_source_name(aenc_source_type type)
{
    hi_char *aenc_name[ANEC_SOURCE_MAX + 1] = {
        "ai",
        "cast",
        "vir_track",
        "none",
    };

    return aenc_name[type];
}

static hi_void aenc_read_proc_encoder_info(hi_void *seq_file, aenc_proc_item *aenc_proc)
{
    hi_char *attach_name = aenc_proc_get_attach_source_name(aenc_proc->attach.type);
    osal_proc_print(seq_file, "%-40s:%s\n",
        "status", (aenc_proc->aenc_work_enable == HI_TRUE) ? "start" : "stop");
    osal_proc_print(seq_file, "%-40s:0x%x\n",
        "codec ID", aenc_proc->codec_id);
    osal_proc_print(seq_file, "%-40s:%s\n",
        "description", aenc_proc->codec_type);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "sample_rate", aenc_proc->sample_rate);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "channels", aenc_proc->channels);
    osal_proc_print(seq_file, "%-40s:%d\n",
        "bit_width", aenc_proc->bit_width);
    if (aenc_proc->attach.type == ANEC_SOURCE_MAX) {
        osal_proc_print(seq_file, "%-40s:%s\n",
            "attach_source", attach_name);
    } else {
        osal_proc_print(seq_file, "%-40s:%s%02d\n",
            "attach_source", attach_name, (aenc_proc->attach.src & 0xff));
    }
    osal_proc_print(seq_file, "\n");
    osal_proc_print(seq_file, "%-40s:%d\n",
        "try_encode_times", aenc_proc->dbg_try_encode_count);
    osal_proc_print(seq_file, "%-40s:%d/%d\n",
        "encode_frame_num(total/error)", aenc_proc->enc_frame, aenc_proc->err_frame);
}

static hi_void aenc_read_proc_input_buf(hi_void *seq_file, aenc_proc_item *aenc_proc)
{
    hi_u32 data_size = 0;
    hi_u32 buf_percent = 0;

    if (aenc_proc->in_buf_size != 0) {
        if (aenc_proc->in_buf_write >= aenc_proc->in_buf_read) {
            data_size = aenc_proc->in_buf_write - aenc_proc->in_buf_read;
        } else {
            data_size = aenc_proc->in_buf_size + aenc_proc->in_buf_write - aenc_proc->in_buf_read;
        }
        buf_percent = data_size * AENC_PERCENTAGE / aenc_proc->in_buf_size;
    }

    osal_proc_print(seq_file, "%-40s:%d/%d/%d%%\n",
        "frame_buf(total/use/percent)(bytes)", aenc_proc->in_buf_size, data_size, buf_percent);
}

static hi_void aenc_read_proc_output_buf(hi_void *seq_file, aenc_proc_item *aenc_proc)
{
    hi_u32 frame_percent = 0;
    hi_u32 frame_num = 0;

    if (aenc_proc->out_frame_num) {
        if (aenc_proc->out_frame_w_idx >= aenc_proc->out_frame_r_idx) {
            frame_num = aenc_proc->out_frame_w_idx - aenc_proc->out_frame_r_idx;
        } else {
            frame_num = aenc_proc->out_frame_num - aenc_proc->out_frame_r_idx + aenc_proc->out_frame_w_idx;
        }

        frame_percent = frame_num * AENC_PERCENTAGE / aenc_proc->out_frame_num;
    }

    osal_proc_print(seq_file, "%-40s:%d/%d/%d%%\n",
        "stream_buf(total/use/percent)(bytes)", aenc_proc->out_frame_num, frame_num, frame_percent);
}

static hi_void aenc_read_proc_dbg_count(hi_void *seq_file, aenc_proc_item *aenc_proc)
{
    osal_proc_print(seq_file, "%-40s:%d/%d\n",
        "send_frame(try/ok)", aenc_proc->dbg_send_buf_count_try, aenc_proc->dbg_send_buf_count);
    osal_proc_print(seq_file, "%-40s:%d/%d\n",
        "receive_stream(try/ok)", aenc_proc->dbg_receive_stream_count_try, aenc_proc->dbg_receive_stream_count);
    osal_proc_print(seq_file, "%-40s:%d/%d\n",
        "release_stream(try/ok)", aenc_proc->dbg_release_stream_count_try, aenc_proc->dbg_release_stream_count);
}

static hi_s32 aenc_read_proc(hi_void *seq_file, hi_void *private)
{
    aenc_instance *aenc = (aenc_instance *)private;
    if (aenc == HI_NULL) {
        return HI_ERR_ADEC_NULL_PTR;
    }

    if (aenc->proc_addr == HI_NULL) {
        osal_proc_print(seq_file, "  AENC[%02d] PROC not INIT\n", aenc->id);
        return HI_SUCCESS;
    }

    osal_proc_print(seq_file, "\n---------------------------");
    osal_proc_print(seq_file, " AENC[%02d] state ", aenc->id);
    osal_proc_print(seq_file, "---------------------------\n\n");

    aenc_read_proc_encoder_info(seq_file, aenc->proc_addr);
    aenc_read_proc_input_buf(seq_file, aenc->proc_addr);
    aenc_read_proc_output_buf(seq_file, aenc->proc_addr);
    aenc_read_proc_dbg_count(seq_file, aenc->proc_addr);

    return HI_SUCCESS;
}

static hi_s32 aenc_proc_how_help(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    aenc_instance *aenc = (aenc_instance *)private;
    if (aenc == HI_NULL) {
        return HI_ERR_ADEC_NULL_PTR;
    }

    osal_printk("\nfunction: save pcm data before audio encode\n");
    osal_printk("commad:   echo save_pcm start|stop > /proc/msp/aenc%02d\n", aenc->id);
    osal_printk("example:  echo save_pcm start > /proc/msp/aenc%02d\n", aenc->id);
    osal_printk("\nfunction: save es data after audio encode \n");
    osal_printk("commad:   echo save_es start|stop > /proc/msp/aenc%02d\n", aenc->id);
    osal_printk("example:  echo save_es start > /proc/msp/aenc%02d\n\n", aenc->id);

    return HI_SUCCESS;
}

static hi_s32 aenc_write_proc_save_es(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    aenc_proc_item *aenc_proc = HI_NULL;
    aenc_instance *aenc = (aenc_instance *)private;
    if ((aenc == HI_NULL) || (aenc->proc_addr == HI_NULL)) {
        HI_ERR_AENC("AENC PROC not init\n");
        return HI_ERR_AENC_NULL_PTR;
    }

    aenc_proc = aenc->proc_addr;

    if (osal_strncmp("save_es", strlen("save_es"), argv[0], strlen(argv[0])) != 0) {
        return HI_ERR_AENC_INVALID_PARA;
    }

    if (osal_strncmp("start", strlen("start"), argv[1], strlen(argv[1])) == 0) {
        snprintf(aenc_proc->file_path, sizeof(aenc_proc->file_path),
                 "/mnt/aenc%02d.es", aenc->id);
        if (aenc_proc->es_ctrl_state != AENC_CMD_CTRL_START) {
            aenc_proc->save_es_cnt++;
            aenc_proc->es_ctrl_state = AENC_CMD_CTRL_START;
            osal_printk("started saving aenc es data to %s\n", aenc_proc->file_path);
        } else {
            osal_printk("started saving aenc es data already.\n");
        }

        return HI_SUCCESS;
    }

    if (osal_strncmp("stop", strlen("stop"), argv[1], strlen(argv[1])) == 0) {
        osal_printk("finished saving aenc es data.\n");
        aenc_proc->es_ctrl_state = AENC_CMD_CTRL_STOP;
        return HI_SUCCESS;
    }

    return HI_ERR_AENC_INVALID_PARA;
}

static hi_s32 aenc_write_proc_save_pcm(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    aenc_proc_item *aenc_proc = HI_NULL;
    aenc_instance *aenc = (aenc_instance *)private;
    if ((aenc == HI_NULL) || (aenc->proc_addr == HI_NULL)) {
        HI_ERR_AENC("AENC PROC not init\n");
        return HI_ERR_AENC_NULL_PTR;
    }

    aenc_proc = aenc->proc_addr;

    if (osal_strncmp("save_pcm", strlen("save_pcm"), argv[0], strlen(argv[0])) != 0) {
        return HI_ERR_ADEC_INVALID_PARA;
    }

    if (osal_strncmp("start", strlen("start"), argv[1], strlen(argv[1])) == 0) {
        snprintf(aenc_proc->file_path, sizeof(aenc_proc->file_path),
                 "/mnt/aenc%02d.pcm", aenc->id);
        if (aenc_proc->pcm_ctrl_state != AENC_CMD_CTRL_START) {
            aenc_proc->save_pcm_cnt++;
            aenc_proc->pcm_ctrl_state = AENC_CMD_CTRL_START;
            osal_printk("started saving aenc pcm data to %s\n", aenc_proc->file_path);
        } else {
            osal_printk("started saving aenc pcm data already.\n");
        }

        return HI_SUCCESS;
    }

    if (osal_strncmp("stop", strlen("stop"), argv[1], strlen(argv[1])) == 0) {
        osal_printk("finished saving aenc pcm data.\n");
        aenc_proc->pcm_ctrl_state = AENC_CMD_CTRL_STOP;
        return HI_SUCCESS;
    }

    return HI_ERR_AENC_INVALID_PARA;
}

static osal_proc_cmd g_aenc_proc_cmd[] = {
    { "save_es", aenc_write_proc_save_es },
    { "save_pcm", aenc_write_proc_save_pcm },
    { "help", aenc_proc_how_help },
};

static hi_void aenc_unreg_proc(aenc_instance *aenc)
{
    hi_char buf[AENC_PROC_NAME_LENGTH] = { 0 };
    snprintf(buf, sizeof(buf), "aenc%02d", aenc->id);
    osal_proc_remove(buf, strlen(buf));
}

static hi_void aenc_reg_proc(aenc_instance *aenc)
{
    hi_char buf[AENC_PROC_NAME_LENGTH] = { 0 };
    osal_proc_entry *item = HI_NULL;

    /* create proc */
    snprintf(buf, sizeof(buf), "aenc%02d", aenc->id);
    item = osal_proc_add(buf, strlen(buf));
    if (item == HI_NULL) {
        HI_FATAL_AENC("create aenc proc entry fail!\n");
        return;
    }

    /* set functions */
    item->read = aenc_read_proc;
    item->cmd_list = g_aenc_proc_cmd;
    item->cmd_cnt = sizeof(g_aenc_proc_cmd) / sizeof(g_aenc_proc_cmd[0]);
    item->private = aenc;

    HI_INFO_AENC("create aenc proc entry OK!\n");
}
#endif

static osal_ioctl_cmd g_aenc_cmd_list[] = {
    { DRV_AENC_PROC_INIT, aenc_drv_ioctl },
    { DRV_AENC_PROC_EXIT, aenc_drv_ioctl },
};

static osal_fileops g_aenc_fops = {
    .open     = aenc_drv_open,
    .write    = HI_NULL,
    .llseek   = HI_NULL,
    .release  = aenc_drv_release,
    .poll     = HI_NULL,
    .mmap     = HI_NULL,
    .cmd_list = g_aenc_cmd_list,
    .cmd_cnt  = sizeof(g_aenc_cmd_list) / sizeof(g_aenc_cmd_list[0]),
};

static osal_dev g_aenc_dev = {
    .name   = HI_DEV_AENC_NAME,
    .minor  = HI_DEV_AENC_MINOR,
    .fops   = &g_aenc_fops,
    .pmops  = HI_NULL,
    .dev    = HI_NULL,
};

hi_s32 aenc_drv_mod_init(hi_void)
{
    hi_s32 ret;

    ret = osal_sem_init(&g_aenc_mutex, 1);
    if (ret != HI_SUCCESS) {
        HI_ERR_AENC("call osal_sem_init failed\n");
        return ret;
    }

    ret = osal_exportfunc_register(HI_ID_AENC, AENC_NAME, HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_ERR_AENC("call osal_exportfunc_register failed(0x%x)\n", ret);
        goto out0;
    }

    ret = osal_dev_register(&g_aenc_dev);
    if (ret != HI_SUCCESS) {
        HI_ERR_AENC("call osal_dev_register failed(0x%x)\n", ret);
        goto out1;
    }

#ifdef MODULE
    HI_PRINT("Load hi_aenc.ko success.  \t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;

out1:
    osal_exportfunc_unregister(HI_ID_AENC);

out0:
    osal_sem_destory(&g_aenc_mutex);
    return ret;
}

hi_void aenc_drv_mod_exit(hi_void)
{
    osal_dev_unregister(&g_aenc_dev);
    osal_exportfunc_unregister(HI_ID_AENC);
    osal_sem_destory(&g_aenc_mutex);
}

#ifdef MODULE
module_init(aenc_drv_mod_init);
module_exit(aenc_drv_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

