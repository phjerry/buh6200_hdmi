/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao proc reference functions
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 initial version for Hi3796CV300
 */

#include "hi_osal.h"
#include "hi_aiao_log.h"

#include "hi_drv_ao.h"
#include "hi_drv_ai.h"
#include "drv_ao_ioctl.h"
#include "drv_ao_ext.h"
#include "drv_ao_private.h"
#include "drv_ao_engine.h"

#include "hi_audsp_aoe.h"
#include "hal_aoe.h"
#include "hal_cast.h"
#include "hal_aiao.h"

#include "drv_ao_op.h"
#include "drv_ao_track.h"
#include "audio_util.h"

#if defined(HI_AUDIO_AI_SUPPORT)
#include "drv_ai_private.h"
#endif

#include "drv_ao_cast.h"
#include "cast_func.h"
#include "drv_ao_hdmi.h"
#include "drv_ao_spdif.h"
#include "drv_ao_avc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct sound_help_info {
    hi_char *item;
    hi_char *describe;
};

struct sound_proc_help_info {
    hi_char *arg1;
    hi_char *argv1;
    hi_char *arg2;
    hi_char *argv2;
    hi_char *describe;
};

static hi_void ao_debug_show_help_example(ao_snd_id sound)
{
    hi_u32 i;

    const struct sound_proc_help_info help_info[] = {
        {
            "help",  "",  "", "",
            "show help information!",
        },

        {
            "save_track", "[track_id]", "[start/stop]", "",
            "save pcm data from track(track_id:0~7,default id is 0)",
        },

        {
            "save_sound", "[start/stop]", "", "",
            "save pcm data from sound",
        },
    };

    for (i = 0; i < sizeof(help_info) / sizeof(help_info[0]); i++) {
        osal_printk("%s%-15s%-15s%-15s%-15s%s%d  %s\n", "echo ",
            help_info[i].arg1, help_info[i].argv1, help_info[i].arg2,
            help_info[i].argv2, "> /proc/msp/sound", sound, help_info[i].describe);
    }
}

static hi_void ao_debug_show_help_avc(ao_snd_id sound)
{
    hi_u32 i;

    const struct sound_proc_help_info help_info[] = {
        {
            "avc", "attacktime", "[20 ~ 2000]", "",
            "auto voice control attacktime set",
        },

        {
            "avc", "releasetime", "[20 ~ 2000]", "",
            "auto voice control releasetime set",
        },

        {
            "avc", "thrhd", "[-39 ~ -17]", "",
            "auto voice control threshold_level set",
        },

        {
            "avc", "limit", "[-16 ~ -6]", "",
            "auto voice control limiter_level set",
        },

        {
            "avc", "gain", "[0 ~ 8]", "",
            "auto voice control limiter_level set",
        },

        {
            "avc", "[off/on/0/1]", "", "",
            "auto voice control switch1:on;0:off",
        },
    };

    for (i = 0; i < sizeof(help_info) / sizeof(help_info[0]); i++) {
        osal_printk("%s%-15s%-15s%-15s%-15s%s%d  %s\n", "echo ",
            help_info[i].arg1, help_info[i].argv1, help_info[i].arg2,
            help_info[i].argv2, "> /proc/msp/sound", sound, help_info[i].describe);
    }
}

static hi_void ao_debug_show_help_hdmi(ao_snd_id sound)
{
    hi_u32 i;

    const struct sound_proc_help_info help_info[] = {
        {"hdmi", "debug", "", "", "enable/disable HDMI debug"},
        {"hdmi", "[pcm/raw/hbr2lbr/auto]",  "", "", "set HDMI output mode to pcm/raw/hbr2lbr/auto"},
    };

    for (i = 0; i < sizeof(help_info) / sizeof(help_info[0]); i++) {
        osal_printk("%s%-15s%-25s%-10s%-10s%s%d  %s\n", "echo ",
            help_info[i].arg1, help_info[i].argv1, help_info[i].arg2,
            help_info[i].argv2, "> /proc/msp/sound", sound, help_info[i].describe);
    }
}

static hi_void ao_debug_show_help_spdif(ao_snd_id sound)
{
    hi_u32 i;

    const struct sound_proc_help_info help_info[] = {
        {"spdif", "[pcm/raw]", "", "", "set SPDIF output mode to pcm/raw"},
    };

    for (i = 0; i < sizeof(help_info) / sizeof(help_info[0]); i++) {
        osal_printk("%s%-15s%-25s%-10s%-10s%s%d  %s\n", "echo ",
            help_info[i].arg1, help_info[i].argv1, help_info[i].arg2,
            help_info[i].argv2, "> /proc/msp/sound", sound, help_info[i].describe);
    }
}

static hi_void ao_debug_show_help_track(ao_snd_id sound)
{
    hi_u32 i;

    const struct sound_proc_help_info help_info[] = {
        {
            "TRACK", "[track_id]",  "weight",  "[-70~0]dB",
            "set track L/R weight(track_id:0~7,default id is 0)",
        },

        {
            "TRACK", "[track_id]",  "mute",  "[on/off]",
            "set track mute on or off(track_id:0~7,default id is 0)",
        },
    };

    for (i = 0; i < sizeof(help_info) / sizeof(help_info[0]); i++) {
        osal_printk("%s%-15s%-15s%-15s%-15s%s%d  %s\n", "echo ",
            help_info[i].arg1, help_info[i].argv1, help_info[i].arg2,
            help_info[i].argv2, "> /proc/msp/sound", sound, help_info[i].describe);
    }
}

static hi_void ao_debug_show_help_out_port(ao_snd_id sound)
{
    hi_u32 i;

    const struct sound_proc_help_info help_info[] = {
        {
            "outport", "[ID]",  "mute",  "[-70~0]dB",
            "set outport mute(outport:DAC0/SPDIF0/HDMI0/...;0:off;1:on)",
        },

        {
            "outport", "[ID]",  "precivolume",  "[-81~18]",
            "set outport precivolume(outport:DAC0/SPDIF0/HDMI0/.../ALLPORT;step:0.125dB)",
        },

        {
            "outport", "[ID]",  "volume",  "[-70~0]dB",
            "set outport volume(outport:DAC0/SPDIF0/HDMI0/.../ALLPORT)",
        },

        {
            "outport", "[ID]",  "delaycomps",  "[0~500]",
            "set outport delay compensation(outport:DAC0/SPDIF0/HDMI0/.../ALLPORT;step:1ms)",
        },
    };

    for (i = 0; i < sizeof(help_info) / sizeof(help_info[0]); i++) {
        osal_printk("%s%-15s%-15s%-15s%-15s%s%d  %s\n", "echo ",
            help_info[i].arg1, help_info[i].argv1, help_info[i].arg2,
            help_info[i].argv2, "> /proc/msp/sound", sound, help_info[i].describe);
    }
}

static hi_void ao_debug_show_help_drc(ao_snd_id sound)
{
    hi_u32 i;

    const struct sound_proc_help_info help_info[] = {
        {
            "outport", "[ID]",  "drc",  "[0/1]",
            "dynamic range control switch(outport:DAC0/SPDIF0/HDMI0/.../ALLPORT;1:on;0:off)",
        },

        {
            "outport", "[ID]",  "drc",  "attacktime[20~2000]",
            "dynamic range control attacktime set(outport:DAC0/SPDIF0/HDMI0/.../ALLPORT;unit:ms)",
        },

        {
            "outport", "[ID]",  "drc",  "releasetime[20~2000]",
            "dynamic range control releasetime set(outport:DAC0/SPDIF0/HDMI0/.../ALLPORT;unit:ms)",
        },

        {
            "outport", "[ID]",  "drc",  "thrhd[-80 ~ -1]",
            "dynamic range control thrhd set(outport:DAC0/SPDIF0/HDMI0/.../ALLPORT)",
        },

        {
            "outport", "[ID]",  "drc",  "limit[-80 ~ -1]",
            "dynamic range limit set(outport:DAC0/SPDIF0/HDMI0/.../ALLPORT)",
        },
    };

    for (i = 0; i < sizeof(help_info) / sizeof(help_info[0]); i++) {
        osal_printk("%s%-15s%-15s%-10s%-20s%s%d  %s\n", "echo ",
            help_info[i].arg1, help_info[i].argv1, help_info[i].arg2,
            help_info[i].argv2, "> /proc/msp/sound", sound, help_info[i].describe);
    }
}

#ifdef __DPT__
static hi_void ao_debug_show_help_peq(ao_snd_id sound)
{
    hi_u32 i;

    const struct sound_proc_help_info help_info[] = {
        {
            "outport [ID]",  "peq",  "[0/1]", "",
            "parametric equalization switch set(outport:DAC0/SPDIF0/HDMI0/.../ALLPORT;1:on;0:off)",
        },

        {
            "outport [ID]",  "peq",  "band[1 ~ 10][type/freq/gain/Q][value]", "",
            "parametric equalization band set(outport:DAC0/SPDIF0/HDMI0/.../ALLPORT)",
        },
    };

    for (i = 0; i < sizeof(help_info) / sizeof(help_info[0]); i++) {
        osal_printk("%s%-15s%-5s%-39s%-1s%s%d  %s\n", "echo ", help_info[i].arg1, help_info[i].argv1, help_info[i].arg2,
            help_info[i].argv2, "> /proc/msp/sound", sound, help_info[i].describe);
    }

    osal_printk("%10s%s\n%10s%s\n", "", "type value:LP/HP/LS/HS/PK", "", "freq value:depend on peq type(unit:Hz)");
    osal_printk("%15s%s\n", "", "LP:50~22000");
    osal_printk("%15s%s\n", "", "HP:20~4000");
    osal_printk("%15s%s\n", "", "LS:20~4000");
    osal_printk("%15s%s\n", "", "HS:4000~22000");
    osal_printk("%15s%s\n", "", "PK:20~22000");

    osal_printk("%10s%s\n", "", "Q value:depend on peq type");
    osal_printk("%15s%s\n", "", "LP:7");
    osal_printk("%15s%s\n", "", "HP:7");
    osal_printk("%15s%s\n", "", "LS:7~10");
    osal_printk("%15s%s\n", "", "HS:7~10");
    osal_printk("%15s%s\n", "", "LP:50~22000");
    osal_printk("%15s%s\n", "", "PK:5~100");

    osal_printk("%10s%s\n", "", "gain value:-15.0~15.0(the number of decimal places is not greater than 3)");
}
#endif

static hi_s32 ao_debug_show_help(ao_snd_id sound)
{
    ao_debug_show_help_example(sound);
    ao_debug_show_help_track(sound);
    ao_debug_show_help_hdmi(sound);
    ao_debug_show_help_spdif(sound);
    ao_debug_show_help_avc(sound);
    ao_debug_show_help_out_port(sound);
    ao_debug_show_help_drc(sound);

#ifdef __DPT__
    ao_debug_show_help_peq(sound);
#endif

    osal_printk("\n");
    osal_printk("You can use 'echo cmd > /proc/msp/sound0' for more details about every command.\n");
    osal_printk("You can use 'echo cmd argv... > /proc/msp/sound0' execute a debug command.\n");

    return HI_SUCCESS;
}

static inline hi_s32 drv_ao_cast_get_def_attr(hi_void *file, hi_ao_cast_attr *cast_attr)
{
    return ao_ioctl_cast_get_def_attr(file, cast_attr);
}

static hi_s32 drv_ao_cast_create(hi_void *file, ao_snd_id sound, hi_ao_cast_attr *cast_attr, hi_handle *h_cast)
{
    hi_s32 ret;
    ao_cast_create_param create_param = {
        .sound = sound,
        .h_cast = HI_INVALID_HANDLE,
    };

    ret = memcpy_s(&create_param.cast_attr, sizeof(hi_ao_cast_attr), cast_attr, sizeof(hi_ao_cast_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    ret = ao_ioctl_cast_create(file, &create_param);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_ioctl_cast_create, ret);
        return ret;
    }

    *h_cast = create_param.h_cast;

    return ret;
}

static inline hi_s32 drv_ao_cast_destroy(hi_void *file, hi_handle h_cast)
{
    return ao_ioctl_cast_destory(file, &h_cast);
}

static inline hi_s32 drv_ao_cast_set_enable(hi_void *file, hi_handle h_cast, hi_bool enable)
{
    ao_cast_enable_param enable_param = {
        .h_cast = h_cast,
        .cast_enable = enable,
    };

    return ao_ioctl_cast_set_enable(file, &enable_param);
}

static hi_s32 drv_ao_cast_acquire_frame(hi_void *file, hi_handle h_cast, ao_frame *frame)
{
    hi_s32 ret;
    ao_cast_data_param data_param = {
        .h_cast = h_cast,
    };

    ao_lock();
    ret = ao_ioctl_cast_acquire_frame(file, &data_param);
    ao_unlock();

    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_ioctl_cast_acquire_frame, ret);
        return ret;
    }

    ret = memcpy_s(frame, sizeof(ao_frame), &data_param.ao_frame, sizeof(ao_frame));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_ao_cast_release_frame(hi_void *file, hi_handle h_cast, ao_frame *frame)
{
    hi_s32 ret;
    ao_cast_data_param data_param = {
        .h_cast = h_cast,
    };

    ret = memcpy_s(&data_param.ao_frame, sizeof(ao_frame), frame, sizeof(ao_frame));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    ao_lock();
    ret = ao_ioctl_cast_release_frame(file, &data_param);
    ao_unlock();

    return ret;
}

static hi_s32 snd_proc_destroy_cast(snd_card_state *card)
{
    hi_s32 ret;
    hi_handle h_cast = card->snd_proc_cast.h_cast;
    hi_void *file = &card->snd_proc_cast.file;

    ret = drv_ao_cast_set_enable(file, h_cast, HI_FALSE);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("drv_ao_cast_set_enable failed(0x%x)\n", ret);
    }

    ret = drv_ao_cast_destroy(file, h_cast);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("drv_ao_cast_destroy failed(0x%x)\n", ret);
    }

    return ret;
}

static hi_s32 snd_proc_create_cast(snd_card_state *card)
{
    hi_s32 ret;
    hi_handle h_cast = HI_INVALID_HANDLE;
    hi_ao_cast_attr cast_attr;
    ao_snd_id sound = snd_card_get_snd(card);
    hi_void *file = &card->snd_proc_cast.file;

    ret = drv_ao_cast_get_def_attr(file, &cast_attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("drv_ao_cast_get_def_attr failed(0x%x)\n", ret);
        return ret;
    }

    ret = drv_ao_cast_create(file, sound, &cast_attr, &h_cast);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("drv_ao_cast_create failed(0x%x)\n", ret);
        return ret;
    }

    ret = drv_ao_cast_set_enable(file, h_cast, HI_TRUE);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("drv_ao_cast_set_enable failed(0x%x)\n", ret);
        goto out;
    }

    card->snd_proc_cast.h_cast = h_cast;

    return HI_SUCCESS;

out:
    ret = drv_ao_cast_destroy(file, h_cast);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("drv_ao_cast_destroy failed(0x%x)\n", ret);
    }

    return HI_FAILURE;
}

static hi_s32 snd_proc_cast_thread(hi_void *arg)
{
    snd_card_state *card = (snd_card_state *)arg;
    hi_handle h_cast = card->snd_proc_cast.h_cast;

    hi_void *file = &card->snd_proc_cast.file;
    hi_void *write_file = card->snd_proc_cast.write_file;

    /* warning: do not return or break from the while() */
    while (!osal_kthread_should_stop()) {
        hi_s32 ret;
        hi_u32 pcm_size;
        ao_frame cast_frame;
        hi_s8 *cast_data = HI_NULL;

        ret = drv_ao_cast_acquire_frame(file, h_cast, &cast_frame);
        if (ret != HI_SUCCESS) {
            osal_msleep(5);
            continue;
        }

        if (cast_frame.pcm_samples == 0) {
            osal_msleep(5);
            continue;
        }

        pcm_size = cast_frame.pcm_samples * autil_calc_frame_size(cast_frame.channels, cast_frame.bit_depth);
        cast_data = (hi_s8 *)HI_NULL + cast_frame.pcm_buffer;

        ret = osal_klib_fwrite(cast_data, pcm_size, write_file);
        if (ret <= 0) {
            HI_ERR_AO("osal_klib_fwrite failed\n");
        }

        ret = drv_ao_cast_release_frame(file, h_cast, &cast_frame);
        if (ret != HI_SUCCESS) {
            HI_ERR_AO("drv_ao_cast_release_frame failed(0x%x)\n", ret);
        }
    }

    return HI_SUCCESS;
}

static hi_void snd_proc_save_data_stop(snd_card_state *card)
{
    hi_s32 ret;

    if (card->snd_proc_cast.thread == HI_NULL) {
        return;
    }

    osal_kthread_destroy(card->snd_proc_cast.thread, 1);

    ret = snd_proc_destroy_cast(card);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("drv_ao_cast_destroy failed(0x%x)\n", ret);
        return;
    }

    if (card->snd_proc_cast.write_file != HI_NULL) {
        osal_klib_fclose(card->snd_proc_cast.write_file);
    }

    card->snd_proc_cast.thread = HI_NULL;
    card->snd_proc_cast.write_file = HI_NULL;
}

static hi_s32 snd_proc_save_data_start(snd_card_state *card)
{
    hi_s32 ret;
    osal_task *thread = HI_NULL;
    hi_void *write_file = HI_NULL; /* file handle used to save cast file */
    hi_char sz_path[AO_SOUND_PATH_NAME_MAXLEN + AO_SOUND_FILE_NAME_MAXLEN] = { 0 };

    ao_snd_id sound = snd_card_get_snd(card);
    osal_rtc_time now = { 0 };
    osal_timeval tv = { 0, 0 };

    if (card->snd_proc_cast.thread != HI_NULL) {
        return HI_FAILURE;
    }

    ret = osal_klib_get_store_path(sz_path, AO_SOUND_PATH_NAME_MAXLEN);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("osal_klib_get_store_path failed(0x%x)\n", ret);
        return HI_FAILURE;
    }

    osal_get_timeofday(&tv);
    osal_rtc_time_to_tm(tv.tv_sec, &now);

    ret = snprintf(sz_path, sizeof(sz_path), "%s/sound%d_%02u_%02u_%02u.pcm", sz_path, (hi_u32)sound, now.tm_hour,
        now.tm_min, now.tm_sec);
    if (ret < 0) {
        HI_ERR_PRINT_FUNC_RES(snprintf, ret);
        return ret;
    }

    write_file = osal_klib_fopen(sz_path, OSAL_O_RDWR | OSAL_O_CREAT | OSAL_O_APPEND, OSAL_O_ACCMODE);
    if (write_file == HI_NULL) {
        HI_ERR_AO("osal_klib_fopen open %s failed\n", sz_path);
        return HI_FAILURE;
    }

    ret = snd_proc_create_cast(card);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("snd_proc_create_cast failed(0x%x)\n", ret);
        goto out0;
    }

    thread = osal_kthread_create(snd_proc_cast_thread, card, "snd_save_data", 0);
    if (thread == HI_NULL) {
        HI_ERR_AO("creat snd_proc_save_thread failed\n");
        goto out1;
    }

    card->snd_proc_cast.thread = thread;
    card->snd_proc_cast.write_file = write_file;

    osal_printk("started saving sound data to %s\n", sz_path);
    return HI_SUCCESS;

out1:
    ret = snd_proc_destroy_cast(card);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("snd_proc_destroy_cast failed(0x%x)\n", ret);
    }

out0:
    osal_klib_fclose(write_file);

    return HI_FAILURE;
}

hi_s32 snd_write_proc(snd_card_state *card, hi_bool bypass, snd_debug_cmd_ctrl cmd)
{
    hi_s32 ret;

    CHECK_AO_NULL_PTR(card);

    if ((cmd == SND_DEBUG_CMD_CTRL_START) && (card->save_state == SND_DEBUG_CMD_CTRL_STOP)) {
        ret = snd_proc_save_data_start(card);
        if (ret != HI_SUCCESS) {
            HI_ERR_AO("snd_proc_save_data_start failed(0x%x)\n", ret);
            return HI_FAILURE;
        }
    } else if ((cmd == SND_DEBUG_CMD_CTRL_START) && (card->save_state == SND_DEBUG_CMD_CTRL_START)) {
        osal_printk("started saving sound data already.\n");
    }

    if ((cmd == SND_DEBUG_CMD_CTRL_STOP) && (card->save_state == SND_DEBUG_CMD_CTRL_START)) {
        snd_proc_save_data_stop(card);
        osal_printk("finished saving sound data.\n");
    } else if ((cmd == SND_DEBUG_CMD_CTRL_STOP) && (card->save_state == SND_DEBUG_CMD_CTRL_STOP)) {
        ao_snd_id sound = snd_card_get_snd(card);
        osal_printk("no sound data saved, please start saving.\n");
        ao_debug_show_help(sound);
    }

    card->save_state = cmd;

    return HI_SUCCESS;
}

static hi_s32 ao_read_snd_proc(hi_void *p, snd_card_state *card)
{
    hi_u32 i;
    hi_ao_attr *snd_attr = &card->user_open_param;

    osal_proc_print(p, "\n-------------------------------------------");
    osal_proc_print(p, "  SOUND[%d]  status  ", card->sound);
    osal_proc_print(p, "-----------------------------------------------\n");

    osal_proc_print(p, "sample_rate   :%d\n", snd_attr->sample_rate);

    if (card->spdif_passthrough != TYPE_MAX) {
        osal_proc_print(p, "SPDIF status :user_set_mode(%s) data_format(%s)\n",
            autil_ouput_mode_to_name(card->user_spdif_mode), autil_format_to_name(card->spdif_data_format));
    }

    if (card->hdmi_passthrough != TYPE_MAX) {
        osal_proc_print(p, "HDMI status  :user_set_mode(%s) data_format(%s)\n",
            autil_ouput_mode_to_name(card->user_hdmi_mode), autil_format_to_name(card->hdmi_data_format));
    }

    if (snd_get_op_handle_by_out_port(card, HI_AO_PORT_DAC0) != HI_NULL) {
        osal_proc_print(p, "ADAC AMP type:%s\n", autil_amp_type_to_name());
    }

    if (card->dma_mode == HI_TRUE) {
        osal_proc_print(p, "\n_snd DMA mode :on\n");
    }

    osal_proc_print(p, "-------------------------------------------");
    osal_proc_print(p, "  out_port status  ");
    osal_proc_print(p, "-------------------------------------------\n");

    for (i = 0; i < snd_attr->port_num; i++) {
        snd_read_op_proc(p, card, snd_attr->outport[i].port);
    }

    if (card->snd_cast_init_flag) {
        osal_proc_print(p, "-------------------------------------------");
        osal_proc_print(p, " cast status  ");
        osal_proc_print(p, "-------------------------------------------\n");
        cast_read_proc(p, card);
    }

    if (card->snd_track_init_flag) {
        osal_proc_print(p, "-------------------------------------------");
        osal_proc_print(p, " track status  ");
        osal_proc_print(p, "-------------------------------------------\n");
        track_read_proc(p, card);
    }

    return HI_SUCCESS;
}

static hi_s32 ao_drv_read_proc(hi_void *file, hi_void *private)
{
    snd_card_state *card = (snd_card_state *)private;
    if (card == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    return ao_read_snd_proc(file, card);
}

static hi_bool ao_proc_judge_outport(hi_char *pc_buf)
{
    if ((pc_buf == strstr(pc_buf, "DAC0")) ||
        (pc_buf == strstr(pc_buf, "DAC1")) ||
        (pc_buf == strstr(pc_buf, "DAC2")) ||
        (pc_buf == strstr(pc_buf, "DAC3")) ||
        (pc_buf == strstr(pc_buf, "I2S0")) ||
        (pc_buf == strstr(pc_buf, "I2S1")) ||
        (pc_buf == strstr(pc_buf, "SPDIF0")) ||
        (pc_buf == strstr(pc_buf, "HDMI0")) ||
        (pc_buf == strstr(pc_buf, "HDMI1")) ||
        (pc_buf == strstr(pc_buf, "ARC0")) ||
        (pc_buf == strstr(pc_buf, "ALLPORT"))) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

hi_s32 ao_write_proc_save_track(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 track_id;
    snd_debug_cmd_ctrl ctrl_cmd;
    hi_char *cmd = HI_NULL;
    const hi_char *pc_start_cmd = "start";
    const hi_char *pc_stop_cmd = "stop";
    snd_card_state *card = (snd_card_state *)private;
    if (card == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    if (argc != 3) { /* 3 is argc num. */
        return HI_FAILURE;
    }

    cmd = argv[1];

    if (cmd[0] < '0' || cmd[0] > '9') {
        return HI_FAILURE;
    }

    track_id = (hi_u32)simple_strtoul(cmd, &cmd, 10);
    if (track_id >= AO_MAX_TOTAL_TRACK_NUM) {
        return HI_FAILURE;
    }

    if (argv[2] == strstr(argv[2], pc_start_cmd)) { /* 2 is argv num. */
        ctrl_cmd = SND_DEBUG_CMD_CTRL_START;
    } else if (argv[2] == strstr(argv[2], pc_stop_cmd)) { /* 2 is argv num. */
        ctrl_cmd = SND_DEBUG_CMD_CTRL_STOP;
    } else {
        return HI_FAILURE;
    }

    return track_write_proc_save_data(card, track_id, ctrl_cmd);
}

static hi_s32 ao_write_proc_save_sound(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    snd_debug_cmd_ctrl ctrl_cmd;
    hi_bool bypass = HI_FALSE;
    const hi_char *pc_start_cmd = "start";
    const hi_char *pc_stop_cmd = "stop";
    const hi_char *pc_aef_cmd = "aef";
    snd_card_state *card = (snd_card_state *)private;
    if (card == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    if (argc == 2) { /* 2 is argc num. */
        if (argv[1] == strstr(argv[1], pc_start_cmd)) {
            ctrl_cmd = SND_DEBUG_CMD_CTRL_START;
        } else if (argv[1] == strstr(argv[1], pc_stop_cmd)) {
            ctrl_cmd = SND_DEBUG_CMD_CTRL_STOP;
        } else {
            return HI_FAILURE;
        }
    } else if (argc == 3) { /* 3 is argc num. */
        if (argv[1] == strstr(argv[1], pc_aef_cmd)) {
            bypass = HI_FALSE;
        } else {
            return HI_FAILURE;
        }

        if (argv[2] == strstr(argv[2], pc_start_cmd)) { /* 2 is argv num. */
            ctrl_cmd = SND_DEBUG_CMD_CTRL_START;
        } else if (argv[2] == strstr(argv[2], pc_stop_cmd)) { /* 2 is argv num. */
            ctrl_cmd = SND_DEBUG_CMD_CTRL_STOP;
        } else {
            return HI_FAILURE;
        }
    } else {
        return HI_FAILURE;
    }


    if (snd_write_proc(card, HI_TRUE, ctrl_cmd) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 ao_write_proc_set_outport(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    snd_card_state *card = (snd_card_state *)private;
    if (card == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    if (ao_proc_judge_outport(argv[1]) == HI_TRUE) {
        if (snd_write_op_proc(card, argv) != HI_SUCCESS) {
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 ao_write_proc_show_help(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    snd_card_state *card = (snd_card_state *)private;
    if (card == HI_NULL) {
        return HI_ERR_AO_NULL_PTR;
    }

    return ao_debug_show_help(card->sound);
}

static osal_proc_cmd g_write_ao_proc[] = {
    { "help", ao_write_proc_show_help },
    { "save_sound", ao_write_proc_save_sound },
    { "save_track", ao_write_proc_save_track },
    { "track", ao_write_proc_set_track },

#if defined(HI_SND_HDMI_SUPPORT)
    { "hdmi", ao_write_proc_set_hdmi },
#endif

#if defined(HI_SND_SPDIF_SUPPORT)
    { "spdif", ao_write_proc_set_spdif },
#endif

#ifdef HI_SND_AVC_SUPPORT
    { "avc", ao_write_proc_set_avc },
#endif

    { "outport", ao_write_proc_set_outport },
};

hi_s32 ao_reg_proc(snd_card_state *card)
{
    hi_s32 ret;
    hi_char buf[16] = { 0 };
    osal_proc_entry *item = HI_NULL;

    /* create proc */
    ret = snprintf(buf, sizeof(buf), "sound%d", card->sound);
    if (ret < 0) {
        HI_ERR_PRINT_FUNC_RES(snprintf, ret);
        return ret;
    }

    item = osal_proc_add(buf, strlen(buf));
    if (item == HI_NULL) {
        HI_LOG_FATAL("create sound proc entry fail!\n");
        return HI_FAILURE;
    }

    /* set functions */
    item->read = ao_drv_read_proc;
    item->cmd_cnt = sizeof(g_write_ao_proc) / sizeof(osal_proc_cmd);
    item->cmd_list = g_write_ao_proc;
    item->private = card;

    HI_LOG_INFO("create sound%d proc entry OK!\n", card->sound);

    return HI_SUCCESS;
}

hi_void ao_unreg_proc(snd_card_state *card)
{
    hi_s32 ret;
    hi_char buf[16] = { 0 };

    snd_proc_save_data_stop(card);

    ret = snprintf(buf, sizeof(buf), "sound%d", card->sound);
    if (ret < 0) {
        HI_ERR_PRINT_FUNC_RES(snprintf, ret);
        return;
    }

    osal_proc_remove(buf, strlen(buf));
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
