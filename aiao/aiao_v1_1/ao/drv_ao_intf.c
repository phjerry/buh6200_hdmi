/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao device and driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_aiao_log.h"
#include "hi_osal.h"

#include "hi_drv_ao.h"
#include "drv_ao_ext.h"
#include "drv_ao_private.h"

#include "hal_cast.h"

#if defined(HI_AIAO_VERIFICATION_SUPPORT)
#include "drv_aiao_veri.h"
#endif

/* aiao drv config */
#include "drv_ao_cfg.h"
#include "drv_ao.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static const struct {
    hi_u32 cmd;
    hi_s32 (*ioctl)(hi_void *file, hi_void *arg);
} g_fn_ao_ioctl[CMD_ID_AO_MAX] = {
    [CMD_ID_AO_SND_GETDEFOPENATTR] = { CMD_AO_GETSNDDEFOPENATTR, ao_ioctl_get_snd_def_open_attr },
    [CMD_ID_AO_SND_OPEN] = { CMD_AO_SND_OPEN,          ao_ioctl_snd_open },
    [CMD_ID_AO_SND_CLOSE] = { CMD_AO_SND_CLOSE,         ao_ioctl_snd_close },
    [CMD_ID_AO_SND_SETMUTE] = { CMD_AO_SND_SETMUTE,       ao_ioctl_snd_set_mute },
    [CMD_ID_AO_SND_GETMUTE] = { CMD_AO_SND_GETMUTE,       ao_ioctl_snd_get_mute },
    [CMD_ID_AO_SND_SETVOLUME] = { CMD_AO_SND_SETVOLUME,     ao_ioctl_snd_set_volume },
    [CMD_ID_AO_SND_GETVOLUME] = { CMD_AO_SND_GETVOLUME,     ao_ioctl_snd_get_volume },
    [CMD_ID_AO_SND_SETOUTPUTMODE] = { CMD_AO_SND_SETOUTPUTMODE, ao_ioctl_snd_set_output_mode },
    [CMD_ID_AO_SND_GETOUTPUTMODE] = { CMD_AO_SND_GETOUTPUTMODE, ao_ioctl_snd_get_output_mode },

#if defined(HI_SND_SPDIF_SUPPORT)
    [CMD_ID_AO_SND_SETSPDIFSCMSMODE] = { CMD_AO_SND_SETSPDIFSCMSMODE,     ao_ioctl_snd_set_spdif_scms_mode },
    [CMD_ID_AO_SND_GETSPDIFSCMSMODE] = { CMD_AO_SND_GETSPDIFSCMSMODE,     ao_ioctl_snd_get_spdif_scms_mode },
    [CMD_ID_AO_SND_SETSPDIFCATEGORYCODE] = { CMD_AO_SND_SETSPDIFCATEGORYCODE, ao_ioctl_snd_set_spdif_category_code },
    [CMD_ID_AO_SND_GETSPDIFCATEGORYCODE] = { CMD_AO_SND_GETSPDIFCATEGORYCODE, ao_ioctl_snd_get_spdif_category_code },
#endif

#if defined(HI_SND_ADAC_SUPPORT)
    [CMD_ID_AO_SND_SETADACENABLE] = { CMD_AO_SND_SETADACENABLE, ao_ioctl_snd_set_adac_enable },
    [CMD_ID_AO_SND_GETADACENABLE] = { CMD_AO_SND_GETADACENABLE, ao_ioctl_snd_get_adac_enable },
#endif

    [CMD_ID_AO_SND_SETPRECIVOL] = { CMD_AO_SND_SETPRECIVOL, ao_ioctl_snd_set_preci_volume },
    [CMD_ID_AO_SND_GETPRECIVOL] = { CMD_AO_SND_GETPRECIVOL, ao_ioctl_snd_get_preci_volume },

#ifdef __DPT__
    [CMD_ID_AO_SND_SETALSAPRESCALE] = { CMD_AO_SND_SETALSAPRESCALE, ao_ioctl_snd_set_alsa_prescale },
#endif

#ifdef HI_SND_ADVANCED_SUPPORT
    [CMD_ID_AO_SND_SETBALANCE] = { CMD_AO_SND_SETBALANCE,      ao_ioctl_snd_set_balance },
    [CMD_ID_AO_SND_GETBALANCE] = { CMD_AO_SND_GETBALANCE,      ao_ioctl_snd_get_balance },
#endif

#ifdef HI_SND_ARC_SUPPORT
    [CMD_ID_AO_SND_SETARCENABLE] = { CMD_AO_SND_SETARCENABLE,  ao_ioctl_snd_set_arc_enable },
    [CMD_ID_AO_SND_GETARCENABLE] = { CMD_AO_SND_GETARCENABLE,  ao_ioctl_snd_get_arc_enable },
    [CMD_ID_AO_SND_SETARCCAP] = { CMD_AO_SND_SETARCCAP,        ao_ioctl_snd_set_arc_cap },
    [CMD_ID_AO_SND_GETARCCAP] = { CMD_AO_SND_GETARCCAP,        ao_ioctl_snd_get_arc_cap },
#endif

#ifdef HI_SND_AVC_SUPPORT
    [CMD_ID_AO_SND_SETAVCATTR] = { CMD_AO_SND_SETAVCATTR,   ao_ioctl_snd_set_avc_attr },
    [CMD_ID_AO_SND_GETAVCATTR] = { CMD_AO_SND_GETAVCATTR,   ao_ioctl_snd_get_avc_attr },
    [CMD_ID_AO_SND_SETAVCENABLE] = { CMD_AO_SND_SETAVCENABLE, ao_ioctl_snd_set_avc_enable },
    [CMD_ID_AO_SND_GETAVCENABLE] = { CMD_AO_SND_GETAVCENABLE, ao_ioctl_snd_get_avc_enable },
#endif

#ifdef HI_SND_GEQ_SUPPORT
    [CMD_ID_AO_SND_SETGEQATTR] = { CMD_AO_SND_SETGEQATTR,   ao_ioctl_snd_set_geq_attr },
    [CMD_ID_AO_SND_GETGEQATTR] = { CMD_AO_SND_GETGEQATTR,   ao_ioctl_snd_get_geq_attr },
    [CMD_ID_AO_SND_SETGEQENABLE] = { CMD_AO_SND_SETGEQENABLE, ao_ioctl_snd_set_geq_enable },
    [CMD_ID_AO_SND_GETGEQENABLE] = { CMD_AO_SND_GETGEQENABLE, ao_ioctl_snd_get_geq_enable },
    [CMD_ID_AO_SND_SETGEQGAIN] = { CMD_AO_SND_SETGEQGAIN,   ao_ioctl_snd_set_geq_gain },
    [CMD_ID_AO_SND_GETGEQGAIN] = { CMD_AO_SND_GETGEQGAIN,   ao_ioctl_snd_get_geq_gain },
#endif

#ifdef HI_SND_DRC_SUPPORT
    [CMD_ID_AO_SND_SETDRCENABLE] = { CMD_AO_SND_SETDRCENABLE, ao_ioctl_snd_set_drc_enable },
    [CMD_ID_AO_SND_GETDRCENABLE] = { CMD_AO_SND_GETDRCENABLE, ao_ioctl_snd_get_drc_enable },
    [CMD_ID_AO_SND_SETDRCATTR] = { CMD_AO_SND_SETDRCATTR,   ao_ioctl_snd_set_drc_attr },
    [CMD_ID_AO_SND_GETDRCATTR] = { CMD_AO_SND_GETDRCATTR,   ao_ioctl_snd_get_drc_attr },
#endif

#ifdef HI_SND_PEQ_SUPPORT
    [CMD_ID_AO_SND_SETPEQATTR] = { CMD_AO_SND_SETPEQATTR,   ao_ioctl_snd_set_peq_attr },
    [CMD_ID_AO_SND_GETPEQATTR] = { CMD_AO_SND_GETPEQATTR,   ao_ioctl_snd_get_peq_attr },
    [CMD_ID_AO_SND_SETPEQENABLE] = { CMD_AO_SND_SETPEQENABLE, ao_ioctl_snd_set_peq_enable },
    [CMD_ID_AO_SND_GETPEQENABLE] = { CMD_AO_SND_GETPEQENABLE, ao_ioctl_snd_get_peq_enable },
#endif

#ifdef HI_SND_AEF_SUPPORT
    [CMD_ID_AO_SND_ATTACHAEF] = { CMD_AO_SND_ATTACHAEF,         ao_ioctl_snd_attach_aef },
    [CMD_ID_AO_SND_DETACHAEF] = { CMD_AO_SND_DETACHAEF,         ao_ioctl_snd_detach_aef },
    [CMD_ID_AO_SND_SETAEFBYPASS] = { CMD_AO_SND_SETAEFBYPASS,      ao_ioctl_snd_set_aef_bypass },
    [CMD_ID_AO_SND_GETAEFBYPASS] = { CMD_AO_SND_GETAEFBYPASS,      ao_ioctl_snd_get_aef_bypass },
    [CMD_ID_AO_SND_GETAEFBUFATTR] = { CMD_AO_SND_GETAEFBUFATTR,     ao_ioctl_snd_get_aef_buf_attr },
    [CMD_ID_AO_SND_GETDEBUGPARAM] = { CMD_AO_SND_GETDEBUGPARAM,     ao_ioctl_snd_get_debug_attr },
#endif

#ifdef HI_SND_ADVANCED_SUPPORT
    [CMD_ID_AO_SND_SETADOUTPUTENABLE] = { CMD_AO_SND_SETADOUTPUTENABLE, ao_ioctl_snd_set_ad_output_enable },
    [CMD_ID_AO_SND_GETADOUTPUTENABLE] = { CMD_AO_SND_GETADOUTPUTENABLE, ao_ioctl_snd_get_ad_output_enable },

    [CMD_ID_AO_SND_GETXRUNCOUNT] = { CMD_AO_SND_GETXRUNCOUNT, ao_ioctl_snd_get_x_run_count },
#endif

#if defined(HI_SOUND_PORT_DELAY_SUPPORT)
    [CMD_ID_AO_SND_SETSOUNDDELAY] = { CMD_AO_SND_SETSOUNDDELAY, ao_ioctl_snd_set_delay_compensation },
    [CMD_ID_AO_SND_GETSOUNDDELAY] = { CMD_AO_SND_GETSOUNDDELAY, ao_ioctl_snd_get_delay_compensation },
#endif

#ifdef HI_SND_ADVANCED_SUPPORT
    [CMD_ID_AO_SND_SETLOWLATENCY] = { CMD_AO_SND_SETLOWLATENCY, ao_ioctl_snd_set_low_latency },
    [CMD_ID_AO_SND_GETLOWLATENCY] = { CMD_AO_SND_GETLOWLATENCY, ao_ioctl_snd_get_low_latency },

    [CMD_ID_AO_SND_SETEXTDELAYMS] = { CMD_AO_SND_SETEXTDELAYMS, ao_ioctl_snd_set_ext_delay_ms },

    [CMD_ID_AO_SND_DMACREATE] = { CMD_AO_SND_DMACREATE,        ao_ioctl_snd_dma_create },
    [CMD_ID_AO_SND_DMADESTORY] = { CMD_AO_SND_DMADESTORY,       ao_ioctl_snd_dma_destory },
    [CMD_ID_AO_SND_DMASETSAMPLERATE] = { CMD_AO_SND_DMASETSAMPLERATE, ao_ioctl_snd_dma_mode_set_sample_rate },
#endif

#if defined(HI_SND_AR_SUPPORT)
    [CMD_ID_AO_SND_SETCONOUTPUTENABLE] = { CMD_AO_SND_SETCONOUTPUTENABLE, ao_ioctl_snd_set_continue_output },
    [CMD_ID_AO_SND_GETCONOUTPUTENABLE] = { CMD_AO_SND_GETCONOUTPUTENABLE, ao_ioctl_snd_get_continue_output },

    [CMD_ID_AO_SND_CREATESB] = { CMD_AO_SND_CREATESB, ao_ioctl_snd_create_sb },
    [CMD_ID_AO_SND_DESTROYSB] = { CMD_AO_SND_DESTROYSB, ao_ioctl_snd_destroy_sb },

    [CMD_ID_AO_SND_SETRENDERPARAM] = { CMD_AO_SND_SETRENDERPARAM, ao_ioctl_snd_set_render_param },
    [CMD_ID_AO_SND_GETRENDERPARAM] = { CMD_AO_SND_GETRENDERPARAM, ao_ioctl_snd_get_render_param },

    [CMD_ID_AO_SND_SETOUTPUTATMOSENABLE] = { CMD_AO_SND_SETOUTPUTATMOSENABLE, ao_ioctl_snd_set_output_atmos_enable },
    [CMD_ID_AO_SND_GETOUTPUTATMOSENABLE] = { CMD_AO_SND_GETOUTPUTATMOSENABLE, ao_ioctl_snd_get_output_atmos_enable },

    [CMD_ID_AO_SND_SETCONOUTPUTSTATUS] = { CMD_AO_SND_SETCONOUTPUTSTATUS, ao_ioctl_set_con_output_status },
    [CMD_ID_AO_SND_GETCONOUTPUTSTATUS] = { CMD_AO_SND_GETCONOUTPUTSTATUS, ao_ioctl_get_con_output_status },

    [CMD_ID_AO_SND_SETOUTPUTLATENCYMODE] = { CMD_AO_SND_SETOUTPUTLATENCYMODE, ao_ioctl_set_output_latency_mode },
    [CMD_ID_AO_SND_GETOUTPUTLATENCYMODE] = { CMD_AO_SND_GETOUTPUTLATENCYMODE, ao_ioctl_get_output_latency_mode },
#endif

    [CMD_ID_AO_SND_SETALLTRACKPRESCALE] = { CMD_AO_SND_SETALLTRACKPRESCALE,  ao_ioctl_snd_set_all_track_prescale },
    [CMD_ID_AO_SND_GETALLTRACKPRESCALE] = { CMD_AO_SND_GETALLTRACKPRESCALE,  ao_ioctl_snd_get_all_track_prescale },

    /* AO track ioctl */
    [CMD_ID_AO_TRACK_GETDEFATTR] = { CMD_AO_TRACK_GETDEFATTR,     ao_ioctl_track_get_def_attr },
    [CMD_ID_AO_TRACK_CREATE] = { CMD_AO_TRACK_CREATE,         ao_ioctl_track_create },
    [CMD_ID_AO_TRACK_DESTROY] = { CMD_AO_TRACK_DESTROY,        ao_ioctl_track_destory },
    [CMD_ID_AO_TRACK_START] = { CMD_AO_TRACK_START,          ao_ioctl_track_start },
    [CMD_ID_AO_TRACK_STOP] = { CMD_AO_TRACK_STOP,           ao_ioctl_track_stop },
    [CMD_ID_AO_TRACK_PAUSE] = { CMD_AO_TRACK_PAUSE,          ao_ioctl_track_pause },
    [CMD_ID_AO_TRACK_FLUSH] = { CMD_AO_TRACK_FLUSH,          ao_ioctl_track_flush },
    [CMD_ID_AO_TRACK_SENDDATA] = { CMD_AO_TRACK_SENDDATA,       ao_ioctl_track_send_data },
    [CMD_ID_AO_TRACK_SETWEITHT] = { CMD_AO_TRACK_SETWEITHT,      ao_ioctl_track_set_weight },
    [CMD_ID_AO_TRACK_GETWEITHT] = { CMD_AO_TRACK_GETWEITHT,      ao_ioctl_track_get_weight },
    [CMD_ID_AO_TRACK_SETSPEEDADJUST] = { CMD_AO_TRACK_SETSPEEDADJUST, ao_ioctl_track_set_speed_adjust },
    [CMD_ID_AO_TRACK_GETDELAYMS] = { CMD_AO_TRACK_GETDELAYMS,     ao_ioctl_track_get_delay_ms },
    [CMD_ID_AO_TRACK_ISBUFEMPTY] = { CMD_AO_TRACK_ISBUFEMPTY,     ao_ioctl_track_is_buf_empty },
    [CMD_ID_AO_TRACK_SETEOSFLAG] = { CMD_AO_TRACK_SETEOSFLAG,     ao_ioctl_track_set_eos_flag },
    [CMD_ID_AO_TRACK_GETATTR] = { CMD_AO_TRACK_GETATTR,        ao_ioctl_track_get_attr },
    [CMD_ID_AO_TRACK_SETATTR] = { CMD_AO_TRACK_SETATTR,        ao_ioctl_track_set_attr },

#ifdef HI_AUDIO_AI_SUPPORT
    [CMD_ID_AO_TRACK_ATTACHAI] = { CMD_AO_TRACK_ATTACHAI,       ao_ioctl_track_attach_ai },
    [CMD_ID_AO_TRACK_DETACHAI] = { CMD_AO_TRACK_DETACHAI,       ao_ioctl_track_detach_ai },
#endif

    [CMD_ID_AO_TRACK_SETABSGAIN] = { CMD_AO_TRACK_SETABSGAIN,     ao_ioctl_track_set_abs_gain },
    [CMD_ID_AO_TRACK_GETABSGAIN] = { CMD_AO_TRACK_GETABSGAIN,     ao_ioctl_track_get_abs_gain },
    [CMD_ID_AO_TRACK_SETMUTE] = { CMD_AO_TRACK_SETMUTE,        ao_ioctl_track_set_mute },
    [CMD_ID_AO_TRACK_GETMUTE] = { CMD_AO_TRACK_GETMUTE,        ao_ioctl_track_get_mute },
    [CMD_ID_AO_TRACK_SETCHANNELMODE] = { CMD_AO_TRACK_SETCHANNELMODE, ao_ioctl_track_set_channel_mode },
    [CMD_ID_AO_TRACK_GETCHANNELMODE] = { CMD_AO_TRACK_GETCHANNELMODE, ao_ioctl_track_get_channel_mode },
    [CMD_ID_AO_TRACK_SETPRESCALE] = { CMD_AO_TRACK_SETPRESCALE, ao_ioctl_track_set_prescale },
    [CMD_ID_AO_TRACK_GETPRESCALE] = { CMD_AO_TRACK_GETPRESCALE, ao_ioctl_track_get_prescale },

#ifdef __DPT__
    [CMD_ID_AO_TRACK_SETSOURCE] = { CMD_AO_TRACK_SETSOURCE, ao_ioctl_track_set_source },
    [CMD_ID_AO_SND_GETTRACKINFO] = { CMD_AO_SND_GETTRACKINFO,      ao_ioctl_snd_get_track_info },
    [CMD_ID_AO_SND_DUPLICATETRACK] = { CMD_AO_SND_DUPLICATETRACK,    ao_ioctl_snd_track_duplicate },
    [CMD_ID_AO_SND_TRACKCONFIGINIT] = { CMD_AO_SND_TRACKCONFIGINIT,   ao_ioctl_snd_track_config_init },
#endif

#ifdef HI_SND_ADVANCED_SUPPORT
    [CMD_ID_AO_TRACK_SETFIFOLATENCY] = { CMD_AO_TRACK_SETFIFOLATENCY, ao_ioctl_track_set_fifo_latency },
    [CMD_ID_AO_TRACK_GETFIFOLATENCY] = { CMD_AO_TRACK_GETFIFOLATENCY, ao_ioctl_track_get_fifo_latency },

    [CMD_ID_AO_TRACK_SETFIFOBYPASS] = { CMD_AO_TRACK_SETFIFOBYPASS, ao_ioctl_track_set_fifo_bypass },
    [CMD_ID_AO_TRACK_MMAP] = { CMD_AO_TRACK_MMAP,          ao_ioctl_track_mmap },
    [CMD_ID_AO_TRACK_SETPRIORITY] = { CMD_AO_TRACK_SETPRIORITY,   ao_ioctl_track_set_priority },
    [CMD_ID_AO_TRACK_GETPRIORITY] = { CMD_AO_TRACK_GETPRIORITY,   ao_ioctl_track_get_priority },

    [CMD_ID_AO_TRACK_SETRESUMETHRESHOLD] = { CMD_AO_TRACK_SETRESUMETHRESHOLD, ao_ioctl_track_set_resume_threshold_ms },
    [CMD_ID_AO_TRACK_GETRESUMETHRESHOLD] = { CMD_AO_TRACK_GETRESUMETHRESHOLD, ao_ioctl_track_get_resume_threshold_ms },

    [CMD_ID_AO_TRACK_SETFADEATTR] = { CMD_AO_TRACK_SETFADEATTR, ao_ioctl_track_set_fade_attr },
    [CMD_ID_AO_TRACK_SETINFO] = { CMD_AO_TRACK_SETINFO,     ao_ioctl_track_set_info },
    [CMD_ID_AO_TRACK_GETINFO] = { CMD_AO_TRACK_GETINFO,     ao_ioctl_track_get_info },
#endif

    [CMD_ID_AO_SND_SETTRACKMODE] = { CMD_AO_SND_SETTRACKMODE,   ao_ioctl_snd_set_track_mode },
    [CMD_ID_AO_SND_GETTRACKMODE] = { CMD_AO_SND_GETTRACKMODE,   ao_ioctl_snd_get_track_mode },

    [CMD_ID_AO_SND_SETALLTRACKMUTE] = { CMD_AO_SND_SETALLTRACKMUTE, ao_ioctl_snd_set_all_track_mute },
    [CMD_ID_AO_SND_GETALLTRACKMUTE] = { CMD_AO_SND_GETALLTRACKMUTE, ao_ioctl_snd_get_all_track_mute },

#ifdef HI_SND_CAST_SUPPORT
    [CMD_ID_AO_CAST_GETDEFATTR] = { CMD_AO_CAST_GETDEFATTR,   ao_ioctl_cast_get_def_attr },
    [CMD_ID_AO_CAST_CREATE] = { CMD_AO_CAST_CREATE,       ao_ioctl_cast_create },
    [CMD_ID_AO_CAST_DESTROY] = { CMD_AO_CAST_DESTROY,      ao_ioctl_cast_destory },
    [CMD_ID_AO_CAST_SETENABLE] = { CMD_AO_CAST_SETENABLE,    ao_ioctl_cast_set_enable },
    [CMD_ID_AO_CAST_GETENABLE] = { CMD_AO_CAST_GETENABLE,    ao_ioctl_cast_get_enable },
    [CMD_ID_AO_CAST_GETINFO] = { CMD_AO_CAST_GETINFO,      ao_ioctl_cast_get_info },
    [CMD_ID_AO_CAST_SETINFO] = { CMD_AO_CAST_SETINFO,      ao_ioctl_cast_set_info },
    [CMD_ID_AO_CAST_ACQUIREFRAME] = { CMD_AO_CAST_ACQUIREFRAME, ao_ioctl_cast_acquire_frame },
    [CMD_ID_AO_CAST_RELEASEFRAME] = { CMD_AO_CAST_RELEASEFRAME, ao_ioctl_cast_release_frame },
    [CMD_ID_AO_CAST_SETABSGAIN] = { CMD_AO_CAST_SETABSGAIN,   ao_ioctl_cast_set_abs_gain },
    [CMD_ID_AO_CAST_GETABSGAIN] = { CMD_AO_CAST_GETABSGAIN,   ao_ioctl_cast_get_abs_gain },
    [CMD_ID_AO_CAST_SETMUTE] = { CMD_AO_CAST_SETMUTE,      ao_ioctl_cast_set_mute },
    [CMD_ID_AO_CAST_GETMUTE] = { CMD_AO_CAST_GETMUTE,      ao_ioctl_cast_get_mute },
    [CMD_ID_AO_SND_SETALLCASTMUTE] = { CMD_AO_SND_SETALLCASTMUTE,    ao_ioctl_snd_set_all_cast_mute },
    [CMD_ID_AO_SND_GETALLCASTMUTE] = { CMD_AO_SND_GETALLCASTMUTE,    ao_ioctl_snd_get_all_cast_mute },
#endif
};

static hi_s32 ao_process_cmd(hi_void *file, hi_u32 cmd, hi_void *arg)
{
    hi_u32 cmd_type = _IOC_TYPE(cmd);
    hi_u32 cmd_id = _IOC_NR(cmd);

#if defined(HI_AIAO_VERIFICATION_SUPPORT)
    if (cmd_type == HI_ID_AIAO) {
        return aiao_veri_process_cmd(file, cmd, arg);
    }
#endif

    if (cmd_type == HI_ID_AO) {
        if (cmd_id >= CMD_ID_AO_MAX) {
            HI_LOG_WARN("unknown cmd: 0x%x\n", cmd);
            return HI_ERR_AO_INVALID_PARA;
        }

        if (cmd != g_fn_ao_ioctl[cmd_id].cmd) {
            HI_LOG_WARN("unknown cmd: 0x%x\n", cmd);
            return HI_ERR_AO_INVALID_PARA;
        }

        if (g_fn_ao_ioctl[cmd_id].ioctl == HI_NULL) {
            return HI_ERR_AO_NOTSUPPORT;
        }

        return (g_fn_ao_ioctl[cmd_id].ioctl)(file, arg);
    }

    HI_LOG_WARN("unknown command type.\n");
    return HI_ERR_AO_INVALID_PARA;
}

static hi_s32 ao_drv_ioctl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;

    ao_lock();

    ret = ao_process_cmd(private_data, cmd, arg);

    ao_unlock();

    return ret;
}

static const aiao_export_func g_aiao_func = {
    .ao_drv_resume = ao_drv_resume,
    .ao_drv_suspend = ao_drv_suspend,
    .ao_drv_get_pll_cfg = ao_drv_get_pll_cfg,
};

hi_s32 ao_drv_init(hi_void)
{
    hi_s32 ret;

    ret = osal_exportfunc_register(HI_ID_AO, "HI_AO", (hi_void *)&g_aiao_func);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(osal_exportfunc_register, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_void ao_drv_exit(hi_void)
{
    osal_exportfunc_unregister(HI_ID_AO);
}

static hi_s32 ao_suspend(hi_void *private_data)
{
    return ao_drv_suspend();
}

static hi_s32 ao_resume(hi_void *private_data)
{
    return ao_drv_resume();
}

static osal_ioctl_cmd g_ao_cmd_list[CMD_ID_AO_MAX];

static osal_fileops g_ao_fops = {
    .open = ao_drv_open,
    .read = HI_NULL,
    .write = HI_NULL,
    .llseek = HI_NULL,
    .release = ao_drv_release,
    .poll = HI_NULL,
    .mmap = HI_NULL,
    .cmd_list = g_ao_cmd_list,
    .cmd_cnt = CMD_ID_AO_MAX,
};

static osal_pmops g_ao_pm_ops = {
    .pm_suspend = ao_suspend,
    .pm_resume = ao_resume,
    .pm_lowpower_enter = HI_NULL,
    .pm_lowpower_exit = HI_NULL,
    .pm_poweroff = HI_NULL,
};

/* ao device */
static osal_dev g_ao_dev = {
    .name   = HI_DEV_AO_NAME,
    .minor  = HI_DEV_AO_MINOR,
    .fops   = &g_ao_fops,
    .pmops  = &g_ao_pm_ops,
};

static hi_void ao_set_ioctl_cmd_list(osal_ioctl_cmd *ao_cmd_list)
{
    int i;

    for (i = 0; i < CMD_ID_AO_MAX; i++) {
        ao_cmd_list[i].cmd = g_fn_ao_ioctl[i].cmd;
        ao_cmd_list[i].handler = ao_drv_ioctl;
    }
}

static hi_s32 ao_drv_register_dev(hi_void)
{
    hi_s32 ret;

    HI_FUNC_ENTER();

    ao_set_ioctl_cmd_list(g_ao_cmd_list);

    /* register ao device */
    ret = osal_dev_register(&g_ao_dev);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(osal_dev_register, ret);
        return ret;
    }

    HI_FUNC_EXIT();
    return HI_SUCCESS;
}

static hi_void ao_drv_unregister_dev(hi_void)
{
    HI_FUNC_ENTER();
    osal_dev_unregister(&g_ao_dev);
    HI_FUNC_EXIT();
}

hi_s32 ao_drv_mod_init(hi_void)
{
    hi_s32 ret;

    ao_osal_init();

#ifndef HI_MCE_SUPPORT
    ret = ao_drv_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_drv_init, ret);
        ao_osal_deinit();
        return ret;
    }
#endif

    ret = ao_drv_register_dev();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(ao_drv_register_dev, ret);
#ifndef HI_MCE_SUPPORT
        ao_drv_exit();
#endif
        ao_osal_deinit();
        return ret;
    }

    return HI_SUCCESS;
}

hi_void ao_drv_mod_exit(hi_void)
{
    ao_drv_unregister_dev();
#ifndef HI_MCE_SUPPORT
    ao_drv_exit();
#endif

    ao_osal_deinit();
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

