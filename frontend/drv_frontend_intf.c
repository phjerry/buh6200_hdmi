/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: frontend interface
 * Author: SDK
 * Created: 2017-06-30
 */
#include "drv_frontend_intf.h"
#include "hi_osal.h"

#include "linux/hisilicon/securec.h"
#include "hi_module.h"
#include "hi_debug.h"
#include "hi_drv_dev.h"
#include "hi_drv_proc.h"
#include "hi_drv_sys.h"

#include "drv_frontend.h"
#include "drv_frontend_ctrl.h"
#include "drv_frontend_proc.h"

static osal_semaphore g_frontend_io_lock;

static osal_ioctl_cmd g_fe_ioctrl_func[] = {
    { FRONTEND_CONNECT_CMD,                  drv_fe_intf_connect },
    { FRONTEND_GET_STATUS_CMD,               drv_fe_intf_get_status },
    { FRONTEND_GET_SIGNALSTRENGTH_CMD,       drv_fe_intf_get_signal_strength },
    { FRONTEND_GET_BER_CMD,                  drv_fe_intf_get_ber },
    { FRONTEND_GET_PER_CMD,                  drv_fe_intf_get_per },
    { FRONTEND_GET_SNR_CMD,                  drv_fe_intf_get_snr },
    { FRONTEND_SET_ATTR_CMD,                 drv_fe_intf_set_attr },
    { FRONTEND_GET_FREQ_SYMB_OFFSET,         drv_fe_intf_get_freq_symb_offset },
    { FRONTEND_GET_SIGANLINFO_CMD,           drv_fe_intf_get_signal_info },
    { FRONTEND_STANDBY_CMD,                  drv_fe_intf_set_standby },
    { FRONTEND_GET_SIGANLQUALITY_CMD,        drv_fe_intf_get_quality },
    { FRONTEND_GET_ATTR_CMD,                 drv_fe_intf_get_attr },
    { FRONTEND_MONITOR_ISDBT_LAYER_CMD,      drv_fe_intf_monitor_layers_config },
    { FRONTEND_SETTSOUT_CMD,                 drv_fe_intf_set_ts_out },
    { FRONTEND_GETTSOUT_CMD,                 drv_fe_intf_get_ts_out },
    { FRONTEND_GET_PLPNUM_CMD,               drv_fe_intf_get_plp_num },
    { FRONTEND_SET_PLP_PARA_CMD,             drv_fe_intf_set_plp_para },
    { FRONTEND_GET_PLP_INFO_CMD,             drv_fe_intf_get_plp_info },
    { FRONTEND_SETSATATTR_CMD,               drv_fe_intf_set_sat_attr },
    { FRONTEND_BLINDSCAN_INIT_CMD,           drv_fe_intf_blindscan_init },
    { FRONTEND_BLINDSCAN_ACTION_CMD,         drv_fe_intf_blindscan_action },
    { FRONTEND_SET_LNBOUT_CMD,               drv_fe_intf_set_lnb_out },
    { FRONTEND_SEND_CONTINUOUS_22K_CMD,      drv_fe_intf_send_continuous_22k },
    { FRONTEND_SEND_TONE_CMD,                drv_fe_intf_send_tone },
    { FRONTEND_DISEQC_SEND_MSG_CMD,          drv_fe_intf_diseqc_send_msg },
    { FRONTEND_DISEQC_RECV_MSG_CMD,          drv_fe_intf_diseqc_recv_msg },
    { FRONTEND_GET_TOTAL_STREAM_CMD,         drv_fe_intf_get_stream_num },
    { FRONTEND_GET_ISI_ID_CMD,               drv_fe_intf_get_isi_id },
    { FRONTEND_SET_ISI_ID_CMD,               drv_fe_intf_set_isi_id },
    { FRONTEND_CLOSE_CMD,                    drv_fe_intf_close }
};

#define FE_IOCTRL_ARR_SIZE    (sizeof(g_fe_ioctrl_func) / sizeof(g_fe_ioctrl_func[0]))

static hi_s32 fe_drv_open(void *private_data)
{
    return HI_SUCCESS;
}

static hi_s32 fe_drv_close(void *private_data)
{
    return HI_SUCCESS;
}

hi_s32 drv_fe_intf_connect(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_signal *pst_signal;

    pst_signal = (fe_ioctrl_signal *)arg;
    ret = drv_fe_ctrl_connect(pst_signal);

    return ret;
}

hi_s32 drv_fe_intf_close(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_data *pst_data;

    pst_data = (fe_ioctrl_data *)arg;
    ret = drv_fe_ctrl_close(pst_data->port);

    return ret;
}

hi_s32 drv_fe_intf_get_status(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_status *pst_status;

    pst_status = (fe_ioctrl_status *)arg;
    ret = drv_fe_ctrl_get_status(pst_status);

    return ret;
}

hi_s32 drv_fe_intf_get_signal_strength(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_data_buf *pst_signal_strength;

    pst_signal_strength = (fe_ioctrl_data_buf *)arg;
    ret = drv_fe_ctrl_get_signal_strength(pst_signal_strength);

    return ret;
}

hi_s32 drv_fe_intf_get_ber(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_ber_per *pst_ber;

    pst_ber = (fe_ioctrl_ber_per *)arg;
    ret = drv_fe_ctrl_get_ber(pst_ber);

    return ret;
}

hi_s32 drv_fe_intf_get_per(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_ber_per *per;

    per = (fe_ioctrl_ber_per *)arg;
    ret = drv_fe_ctrl_get_per(per);

    return ret;
}

hi_s32 drv_fe_intf_get_snr(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_snr *pst_snr;

    pst_snr = (fe_ioctrl_snr *)arg;
    ret = drv_fe_ctrl_get_snr(pst_snr);

    return ret;
}

hi_s32 drv_fe_intf_set_attr(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_attr *pst_attr;

    pst_attr = (fe_ioctrl_attr *)arg;
    ret = drv_fe_ctrl_set_attr(pst_attr);

    return ret;
}

hi_s32 drv_fe_intf_get_freq_symb_offset(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_data_buf *pst_freq_symb_offset;

    pst_freq_symb_offset = (fe_ioctrl_data_buf *)arg;
    ret = drv_fe_ctrl_get_freq_symb_offset(pst_freq_symb_offset);

    return ret;
}

hi_s32 drv_fe_intf_get_signal_info(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_signal_info *pst_signal_info;

    pst_signal_info = (fe_ioctrl_signal_info *)arg;
    ret = drv_fe_ctrl_get_signal_info(pst_signal_info);

    return ret;
}

hi_s32 drv_fe_intf_set_standby(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_standby *pst_standby;

    pst_standby = (fe_ioctrl_standby *)arg;
    ret = drv_fe_ctrl_set_standby(pst_standby);

    return ret;
}

hi_s32 drv_fe_intf_get_quality(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_data *pst_quality;

    pst_quality = (fe_ioctrl_data *)arg;
    ret = drv_fe_ctrl_get_quality(pst_quality);

    return ret;
}

hi_s32 drv_fe_intf_get_attr(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_attr *pst_attr;

    pst_attr = (fe_ioctrl_attr *)arg;
    ret = drv_fe_ctrl_get_attr(pst_attr);

    return ret;
}

hi_s32 drv_fe_intf_monitor_layers_config(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_receive_config *receive_config;

    receive_config = (fe_ioctrl_receive_config *)arg;
    ret = drv_fe_ctrl_monitor_layers_config(receive_config);

    return ret;
}

hi_s32 drv_fe_intf_set_ts_out(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_ts_out *ts_out;

    ts_out = (fe_ioctrl_ts_out *)arg;
    ret = drv_fe_ctrl_set_ts_out(ts_out);

    return ret;
}

hi_s32 drv_fe_intf_get_ts_out(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_ts_out *ts_out;

    ts_out = (fe_ioctrl_ts_out *)arg;
    ret = drv_fe_ctrl_get_ts_out(ts_out);

    return ret;
}

hi_s32 drv_fe_intf_get_plp_num(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_plp_num *plp_num;

    plp_num = (fe_ioctrl_plp_num *)arg;
    ret = drv_fe_ctrl_get_plp_num(plp_num);

    return ret;
}

hi_s32 drv_fe_intf_set_plp_para(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_set_plp_para *set_plp_para;

    set_plp_para = (fe_ioctrl_set_plp_para *)arg;
    ret = drv_fe_ctrl_set_plp_para(set_plp_para);

    return ret;
}

hi_s32 drv_fe_intf_get_plp_info(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_get_plp_info *get_plp_info;

    get_plp_info = (fe_ioctrl_get_plp_info *)arg;
    ret = drv_fe_ctrl_get_plp_info(get_plp_info);

    return ret;
}

hi_s32 drv_fe_intf_set_sat_attr(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_sat_attr *sat_attr;

    sat_attr = (fe_ioctrl_sat_attr *)arg;
    ret = drv_fe_ctrl_set_sat_attr(sat_attr);

    return ret;
}

hi_s32 drv_fe_intf_blindscan_init(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_blindscan *blind_scan;

    blind_scan = (fe_ioctrl_blindscan *)arg;
    ret = drv_fe_ctrl_set_blindscan_init(blind_scan);

    return ret;
}

hi_s32 drv_fe_intf_blindscan_action(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_blindscan_info *blindscan_action;

    blindscan_action = (fe_ioctrl_blindscan_info *)arg;
    ret = drv_fe_ctrl_set_blindscan_action(blindscan_action);

    return ret;
}

hi_s32 drv_fe_intf_blindscan_abort(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_blindscan_abort *blindscan_abort;

    blindscan_abort = (fe_ioctrl_blindscan_abort *)arg;
    ret = drv_fe_ctrl_set_blindscan_abort(blindscan_abort);

    return ret;
}

hi_s32 drv_fe_intf_set_lnb_out(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_lnb_out *lnb_out;

    lnb_out = (fe_ioctrl_lnb_out *)arg;
    ret = drv_fe_ctrl_set_lnb_out(lnb_out);

    return ret;
}

hi_s32 drv_fe_intf_send_continuous_22k(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_continuous_22k *continuous_22k;

    continuous_22k = (fe_ioctrl_continuous_22k *)arg;
    ret = drv_fe_ctrl_set_continuous_22k(continuous_22k);

    return ret;
}

hi_s32 drv_fe_intf_send_tone(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_tone *tone;

    tone = (fe_ioctrl_tone *)arg;
    ret = drv_fe_ctrl_set_send_tone(tone);

    return ret;
}

hi_s32 drv_fe_intf_diseqc_send_msg(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_diseqc_sendmsg *diseqc_send_msg;

    diseqc_send_msg = (fe_ioctrl_diseqc_sendmsg *)arg;
    ret = drv_fe_ctrl_set_diseqc_send_msg(diseqc_send_msg);

    return ret;
}

hi_s32 drv_fe_intf_diseqc_recv_msg(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_diseqc_recvmsg *diseqc_recv_msg;

    diseqc_recv_msg = (fe_ioctrl_diseqc_recvmsg *)arg;
    ret = drv_fe_ctrl_set_diseqc_recv_msg(diseqc_recv_msg);

    return ret;
}

hi_s32 drv_fe_intf_get_stream_num(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_get_stream_num *get_stream_num;

    get_stream_num = (fe_ioctrl_get_stream_num *)arg;
    ret = drv_fe_ctrl_get_stream_num(get_stream_num);

    return ret;
}

hi_s32 drv_fe_intf_get_isi_id(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_get_isi_id *get_isi_id;

    get_isi_id = (fe_ioctrl_get_isi_id *)arg;
    ret = drv_fe_ctrl_get_isi_id(get_isi_id);

    return ret;
}

hi_s32 drv_fe_intf_set_isi_id(unsigned int cmd, void *arg, void *private_data)
{
    hi_s32 ret;
    fe_ioctrl_set_isi_id *set_isi_id;

    set_isi_id = (fe_ioctrl_set_isi_id *)arg;
    ret = drv_fe_ctrl_set_isi_id(set_isi_id);

    return ret;
}

static hi_s32 fe_drv_suspend(void *private_data)
{
    hi_s32 ret;

    ret = drv_fe_ctrl_suspend();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_suspend, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 fe_drv_resume(void *private_data)
{
    hi_s32 ret;

    ret = drv_fe_ctrl_resume();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_resume, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 fe_drv_lowpower_enter(void *private_data)
{
    return 0;
}

hi_s32 fe_drv_lowpower_exit(void *private_data)
{
    return 0;
}

static osal_fileops g_fe_file_ops = {
    .open           = fe_drv_open,
    .release        = fe_drv_close,
    .cmd_list       = g_fe_ioctrl_func,
    .cmd_cnt        = FE_IOCTRL_ARR_SIZE,
};

static osal_pmops g_frontend_pm_ops = {
    .pm_suspend        = fe_drv_suspend,
    .pm_resume         = fe_drv_resume,
    .pm_lowpower_enter   = fe_drv_lowpower_enter,
    .pm_lowpower_exit   = fe_drv_lowpower_exit,
    .pm_poweroff        = NULL,
    .private_data       = NULL,
};

static osal_dev g_fe_dev = {
    .minor      = HI_DEV_FRONTEND_MINOR,
    .fops       = &g_fe_file_ops,
    .pmops     = &g_frontend_pm_ops,
};

static hi_s32 fe_drv_module_init_proc(hi_void)
{
    hi_s32 ret;
    ret = fe_drv_proc_add();
    if (ret != HI_SUCCESS) {
        LOG_ERR_PRINT("fe_drv_proc_add fail!\n");
        osal_exportfunc_unregister(HI_ID_FRONTEND);
        osal_dev_unregister(&g_fe_dev);
        return ret;
    }

    ret = fe_drv_proc_reg_add();
    if (ret != HI_SUCCESS) {
        LOG_ERR_PRINT("fe_drv_proc_reg_add fail!\n");
        osal_exportfunc_unregister(HI_ID_FRONTEND);
        osal_proc_remove("frontend", strlen("frontend"));
        osal_dev_unregister(&g_fe_dev);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 __init fe_drv_module_init(hi_void)
{
    hi_s32 ret;

    ret = osal_sem_init(&g_frontend_io_lock, 1);
    if (ret != HI_SUCCESS) {
        LOG_ERR_PRINT("osal_sem_init fail!\n");
        return ret;
    }

    ret = snprintf_s(g_fe_dev.name, sizeof(g_fe_dev.name),
        sizeof(HI_DEV_FRONTEND_NAME), "%s", HI_DEV_FRONTEND_NAME);
    if (ret < HI_SUCCESS) {
        LOG_ERR_PRINT("snprintf_s fail!\n");
        return ret;
    }

    ret = osal_dev_register(&g_fe_dev);
    if (ret != HI_SUCCESS) {
        LOG_ERR_PRINT("front_end device register fail!\n");
        return ret;
    }

    ret = osal_exportfunc_register(HI_ID_FRONTEND, "HI_FRONTEND", NULL);
    if (ret != HI_SUCCESS) {
        LOG_ERR_PRINT("hi_drv_module_register failed, mode ID = 0x%08X\n", HI_ID_FRONTEND);
        osal_dev_unregister(&g_fe_dev);
        return ret;
    }

#ifdef HI_PROC_SUPPORT
    ret = fe_drv_module_init_proc();
    if (ret != HI_SUCCESS) {
        LOG_ERR_PRINT("fe_drv_module_init_proc fail!\n");
        osal_exportfunc_unregister(HI_ID_FRONTEND);
        osal_dev_unregister(&g_fe_dev);
        return ret;
    }
#endif
    ret = drv_fe_ctrl_init();
    if (ret != HI_SUCCESS) {
        LOG_ERR_PRINT("front_end init fail!\n");
        osal_exportfunc_unregister(HI_ID_FRONTEND);
#ifdef HI_PROC_SUPPORT
        osal_proc_remove("frontend", strlen("frontend"));
        osal_proc_remove("demod_reg", strlen("demod_reg"));
#endif
        osal_dev_unregister(&g_fe_dev);
        return ret;
    }

#ifdef MODULE
    HI_PRINT("Load hi_frontend.ko success.\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;
}

hi_void __exit fe_drv_module_exit(hi_void)
{
    drv_fe_ctrl_deinit();
    osal_sem_destory(&g_frontend_io_lock);
    osal_exportfunc_unregister(HI_ID_FRONTEND);
#ifdef HI_PROC_SUPPORT
    osal_proc_remove("frontend", strlen("frontend"));
    osal_proc_remove("demod_reg", strlen("demod_reg"));
#endif

    osal_dev_unregister(&g_fe_dev);

#ifdef MODULE
    HI_PRINT("remove hi_frontend.ko success.\n");
#endif
}

#ifdef MODULE
module_init(fe_drv_module_init);
module_exit(fe_drv_module_exit);
#endif

MODULE_LICENSE("GPL");

