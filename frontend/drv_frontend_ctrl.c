/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: frontend ctrl
 * Author: SDK
 * Created: 2017-06-30
 */

#include "drv_frontend_ctrl.h"

#include "linux/hisilicon/securec.h"
#include "hi_drv_module.h"

#include "drv_lnbctrl.h"

/* include tuner headers */
#if defined(TUNER_DEV_TYPE_MXL608)
#include "mxl608.h"
#endif

#if defined(TUNER_DEV_TYPE_RDA5815)
#include "rda5815.h"
#endif

/* include demod headers */
#if defined(DEMOD_DEV_TYPE_HI3137)
#include "hi3137.h"
#endif

#if defined(DEMOD_DEV_TYPE_INTERNAL0)
#include "hisdtv100.h"
#endif

#if defined(DEMOD_DEV_TYPE_CXD2878)
#include "cxd2878.h"
#endif

#if defined(DEMOD_DEV_TYPE_MXL541)
#include "mxl541.h"
#endif

#if defined(DEMOD_DEV_TYPE_MXL254)
#include "mxl254.h"
#endif

/* include lnb headers */
#if defined(LNB_CTRL_TPS65233)
#include "tps65233.h"
#endif

#if defined(LNB_CTRL_LNBH30)
#include "lnbh30.h"
#endif

#if defined(LNB_CTRL_A8300)
#include "a8300.h"
#endif

#define drv_fe_ctrl_get_ctx(port)       (&(g_fe_ctrl_ctx[port]))
#define drv_fe_ctrl_get_tuner_ops(port) (&(g_fe_ctrl_ctx[port].tuner_ops))
#define drv_fe_ctrl_get_demod_ops(port) (&(g_fe_ctrl_ctx[port].demod_ops))
#define drv_fe_ctrl_get_lnb_ops(port)   (&(g_fe_ctrl_ctx[port].lnb_ops))

static drv_fe_ctrl_func   g_fe_ctrl_func;
static drv_fe_ctrl_ctx    g_fe_ctrl_ctx[DRV_FRONTEND_NUM];

const static drv_fe_ctrl_demod_regist_func_map g_demod_regist_func_map[] = {
#if defined(DEMOD_DEV_TYPE_HI3137)
    { HI_DRV_DEMOD_DEV_TYPE_3137, drv_fe_adp_demod_hi3137_regist_func },
#endif
#if defined(DEMOD_DEV_TYPE_INTERNAL0)
    { HI_DRV_DEMOD_DEV_TYPE_INTERNAL0, drv_fe_adp_demod_hisdtv100_regist_func },
#endif
#if defined(DEMOD_DEV_TYPE_CXD2878)
    { HI_DRV_DEMOD_DEV_TYPE_CXD2856, drv_fe_adp_demod_cxd2878_regist_func },
    { HI_DRV_DEMOD_DEV_TYPE_CXD2857, drv_fe_adp_demod_cxd2878_regist_func },
    { HI_DRV_DEMOD_DEV_TYPE_CXD2878, drv_fe_adp_demod_cxd2878_regist_func },
#endif
#if defined(DEMOD_DEV_TYPE_MXL541)
    { HI_DRV_DEMOD_DEV_TYPE_MXL541, drv_fe_adp_demod_mxl541_regist_func },
#endif
#if defined(DEMOD_DEV_TYPE_MXL254)
    { HI_DRV_DEMOD_DEV_TYPE_MXL214, drv_fe_adp_demod_mxl254_regist_func },
    { HI_DRV_DEMOD_DEV_TYPE_MXL254, drv_fe_adp_demod_mxl254_regist_func },
#endif

    { HI_DRV_DEMOD_DEV_TYPE_MAX, NULL }
};

const static drv_fe_ctrl_tuner_regist_func_map g_tuner_regist_func_map[] = {
#if defined(TUNER_DEV_TYPE_RDA5815)
    { HI_DRV_TUNER_DEV_TYPE_RDA5815, drv_fe_adp_tuner_rda5815_regist_func },
#endif
#if defined(TUNER_DEV_TYPE_MXL608)
    { HI_DRV_TUNER_DEV_TYPE_MXL608, drv_fe_adp_tuner_mxl608_regist_func },
#endif
    { HI_DRV_TUNER_DEV_TYPE_MAX, NULL }
};

const static drv_fe_ctrl_lnb_regist_func_map g_lnb_regist_func_map[] = {
#if defined(LNB_CTRL_TPS65233)
    { HI_DRV_LNB_CTRL_DEV_TYPE_TPS65233, drv_fe_adp_lnb_tps65233_regist_func },
#endif
#if defined(LNB_CTRL_LNBH30)
    { HI_DRV_LNB_CTRL_DEV_TYPE_LNBH30, drv_fe_adp_lnbh30_regist_func },
#endif
#if defined(LNB_CTRL_A8300)
    { HI_DRV_LNB_CTRL_DEV_TYPE_A8300, drv_fe_adp_a8300_regist_func },
#endif
    { HI_DRV_LNB_CTRL_DEV_TYPE_NONE, drv_fe_adp_lnb_none_regist_func }
};

drv_fe_ctrl_ctx *drv_frontend_get_context(hi_u32 port_id)
{
    return (port_id < DRV_FRONTEND_NUM) ? &g_fe_ctrl_ctx[port_id] : NULL;
}

hi_drv_frontend_sig_type drv_frontend_get_sig_type(hi_u32 port_id)
{
    if (port_id < DRV_FRONTEND_NUM) {
        return g_fe_ctrl_ctx[port_id].attr.sig_type;
    } else {
        return HI_DRV_FRONTEND_SIG_TYPE_MAX;
    }
}

drv_fe_ctrl_func *drv_frontend_get_ctrl_func(hi_void)
{
    return &g_fe_ctrl_func;
}

static hi_s32 drv_fe_ctrl_check_attr_inited(hi_u32 port_id)
{
    if (port_id >= DRV_FRONTEND_NUM) {
        LOG_ERR_PRINT("[%d] : port[%d] invalid!\n", current->pid, port_id);
        return HI_ERR_FRONTEND_INVALID_PORT;
    }

    if (g_fe_ctrl_ctx[port_id].attr_inited != HI_TRUE) {
        LOG_ERR_PRINT("[%d] : set attr first!(port:%d)\n", current->pid, port_id);
        return HI_ERR_FRONTEND_NOT_INIT;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_fe_register_lnb_func(drv_fe_ctrl_ctx *fe_ctrl_ctx)
{
    hi_s32 ret, i;
    hi_drv_frontend_sat_attr *sat_attr = NULL;

    drv_fe_check_pointer(fe_ctrl_ctx);

    sat_attr = &fe_ctrl_ctx->sat_attr;

    for (i = 0; i < ARRAY_SIZE(g_lnb_regist_func_map); i++) {
        if (sat_attr->lnb_ctrl_dev == g_lnb_regist_func_map[i].lnb_type
            && g_lnb_regist_func_map[i].drv_fe_adp_lnb_regist_func != NULL) {
            ret = g_lnb_regist_func_map[i].drv_fe_adp_lnb_regist_func(&fe_ctrl_ctx->lnb_ops);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(g_lnb_regist_func_map[i].drv_fe_adp_lnb_regist_func, ret);
            }
            return ret;
        }
    }

    LOG_ERR_PRINT("[%d] : Not support lnb device[%d]\n", current->pid, sat_attr->lnb_ctrl_dev);
    return HI_ERR_FRONTEND_FAILED_SETSATATTR;
}

static hi_s32 drv_fe_register_tuner_func(drv_fe_ctrl_ctx *fe_ctrl_ctx)
{
    hi_s32 ret, i;
    hi_drv_frontend_attr *attr = NULL;

    drv_fe_check_pointer(fe_ctrl_ctx);

    attr = &fe_ctrl_ctx->attr;

    for (i = 0; i < ARRAY_SIZE(g_tuner_regist_func_map); i++) {
        if (attr->tuner_dev_type == g_tuner_regist_func_map[i].tuner_type
            && g_tuner_regist_func_map[i].drv_fe_adp_tuner_regist_func != NULL) {
            ret = g_tuner_regist_func_map[i].drv_fe_adp_tuner_regist_func(&fe_ctrl_ctx->tuner_ops);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(g_tuner_regist_func_map[i].drv_fe_adp_tuner_regist_func, ret);
            }
            return ret;
        }
    }

    LOG_ERR_PRINT("[%d] : Not support tuner device[%d]\n", current->pid, attr->tuner_dev_type);
    return HI_ERR_FRONTEND_FAILED_SELECTTUNER;
}

static hi_s32 drv_fe_register_demod_func(drv_fe_ctrl_ctx *fe_ctrl_ctx)
{
    hi_s32 ret, i;
    hi_drv_frontend_attr *attr = NULL;

    drv_fe_check_pointer(fe_ctrl_ctx);

    attr = &fe_ctrl_ctx->attr;

    for (i = 0; i < ARRAY_SIZE(g_demod_regist_func_map); i++) {
        if (attr->demod_dev_type == g_demod_regist_func_map[i].demod_type
            && g_demod_regist_func_map[i].drv_fe_adp_demod_regist_func != NULL) {
            ret = g_demod_regist_func_map[i].drv_fe_adp_demod_regist_func(&fe_ctrl_ctx->demod_ops);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(g_demod_regist_func_map[i].drv_fe_adp_demod_regist_func, ret);
            }
            return ret;
        }
    }

    LOG_ERR_PRINT("[%d] : Not support demod device[%d]\n", current->pid, attr->demod_dev_type);
    return HI_ERR_FRONTEND_FAILED_SELECTTUNER;
}

static drv_fe_tv_system drv_fe_get_tuner_system(drv_fe_ctrl_ctx *fe_ctrl_ctx)
{
    drv_fe_tv_system en_tuner_system;

    drv_fe_check_pointer(fe_ctrl_ctx);

    switch (fe_ctrl_ctx->attr.sig_type) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C:
            en_tuner_system = DRV_FE_TV_SYSTEM_QAM_8MHz;
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_J83B:
            en_tuner_system = DRV_FE_TV_SYSTEM_QAM_6MHz;
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DTMB:
            en_tuner_system = DRV_FE_TV_SYSTEM_DTMB_8MHz;
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T:
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T2:
            en_tuner_system = DRV_FE_TV_SYSTEM_DVBT_8MHz;
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_T:
            en_tuner_system = DRV_FE_TV_SYSTEM_ISDBT_6MHz;
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ATSC_T:
            en_tuner_system = DRV_FE_TV_SYSTEM_ATSC_6MHz;
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S:
            en_tuner_system = DRV_FE_TV_SYSTEM_SAT;
            break;
        default:
            LOG_ERR_PRINT("[%d] : not support now!\n", current->pid);
            en_tuner_system = DRV_FE_TV_SYSTEM_MAX;
            break;
    }

    return en_tuner_system;
}

static hi_s32 drv_fe_ctrl_init_i2c(hi_void)
{
    hi_s32 ret;

    g_fe_ctrl_func.i2c_func = NULL;
    ret = hi_drv_module_get_func(HI_ID_I2C, (hi_void**)(&g_fe_ctrl_func.i2c_func));
    if (ret != HI_SUCCESS) {
        LOG_ERR_PRINT("[%d] : get i2c function failure!\n", current->pid);
        return HI_FAILURE;
    }

    if ((g_fe_ctrl_func.i2c_func == NULL) ||
        (g_fe_ctrl_func.i2c_func->pfn_i2c_write == NULL) ||
        (g_fe_ctrl_func.i2c_func->pfn_i2c_read == NULL) ||
        (g_fe_ctrl_func.i2c_func->pfn_i2c_read_directly == NULL) ||
        (g_fe_ctrl_func.i2c_func->pfn_i2c_write_nostop == NULL)) {
        LOG_ERR_PRINT("[%d] : I2C not found\n", current->pid);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_fe_ctrl_init_gpio(hi_void)
{
    hi_s32 ret;

    g_fe_ctrl_func.gpio_func = NULL;
    ret = hi_drv_module_get_func(HI_ID_GPIO, (hi_void**)(&g_fe_ctrl_func.gpio_func));
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_module_get_function, ret);
        return ret;
    }

    if ((g_fe_ctrl_func.gpio_func == NULL) ||
        (g_fe_ctrl_func.gpio_func->pfn_gpio_write_bit == NULL) ||
        (g_fe_ctrl_func.gpio_func->pfn_gpio_read_bit == NULL) ||
        (g_fe_ctrl_func.gpio_func->pfn_gpio_direction_set_bit == NULL)) {
        LOG_ERR_PRINT("[%d] : GPIO not found\n", current->pid);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_fe_ctrl_init_gpio_i2c(hi_void)
{
    hi_s32 ret;

    g_fe_ctrl_func.gpio_i2c_func = NULL;
    ret = hi_drv_module_get_func(HI_ID_GPIO_I2C, (hi_void**)&g_fe_ctrl_func.gpio_i2c_func);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(hi_drv_module_get_function, ret);
        return ret;
    }

    if ((g_fe_ctrl_func.gpio_i2c_func == NULL) ||
        (g_fe_ctrl_func.gpio_i2c_func->pfn_gpio_i2c_write_ext == NULL) ||
        (g_fe_ctrl_func.gpio_i2c_func->pfn_gpio_i2c_read_ext == NULL) ||
        (g_fe_ctrl_func.gpio_i2c_func->pfn_gpio_i2c_read_ext_directly == NULL)) {
        LOG_ERR_PRINT("[%d] : GPIO_I2C not found\n", current->pid);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_init(hi_void)
{
    hi_s32 ret;
    hi_s32 i;

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    ret = memset_s(&g_fe_ctrl_ctx[0], (sizeof(drv_fe_ctrl_ctx) * DRV_FRONTEND_NUM),
        0, (sizeof(drv_fe_ctrl_ctx) * DRV_FRONTEND_NUM));
    if (ret != HI_SUCCESS) {
        return HI_ERR_FRONTEND_SECURE_CHECK;
    }

    for (i = 0; i < DRV_FRONTEND_NUM; i++) {
        g_fe_ctrl_ctx[i].attr_set_err_print_enable = HI_TRUE;
    }

    ret = drv_fe_ctrl_init_i2c();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_init_i2c, ret);
        return ret;
    }

    ret = drv_fe_ctrl_init_gpio();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_init_gpio, ret);
        return ret;
    }

    ret = drv_fe_ctrl_init_gpio_i2c();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_init_gpio_i2c, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_void drv_fe_ctrl_deinit(hi_void)
{
    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);
    g_fe_ctrl_func.i2c_func = NULL;
    g_fe_ctrl_func.gpio_func = NULL;
    g_fe_ctrl_func.gpio_i2c_func = NULL;
}

static hi_s32 drv_fe_ctrl_connect_valid(fe_ioctrl_signal *signal, drv_fe_ctrl_ctx *fe_ctrl_ctx)
{
    if ((((signal->sig_type == HI_DRV_FRONTEND_SIG_TYPE_DVB_T) &&
	    (fe_ctrl_ctx->attr.sig_type == HI_DRV_FRONTEND_SIG_TYPE_DVB_T2)) ||
        ((signal->sig_type == HI_DRV_FRONTEND_SIG_TYPE_DVB_T2) &&
		(fe_ctrl_ctx->attr.sig_type == HI_DRV_FRONTEND_SIG_TYPE_DVB_T)))) {
        LOG_DBG_PRINT("Auto to lock the dvbt and dvbt2 signal.\n");
    } else if (signal->sig_type != fe_ctrl_ctx->attr.sig_type) {
        LOG_ERR_PRINT("[%d] : sig_type: %d is not mach with current signal type: %d!\n", current->pid,
            signal->sig_type, fe_ctrl_ctx->attr.sig_type);
        return HI_ERR_FRONTEND_INVALID_SIGTYPE;
    }
    return HI_SUCCESS;
}

static hi_s32 drv_fe_ctrl_connect_execute(fe_ioctrl_signal  *signal, drv_fe_ctrl_ctx *fe_ctrl_ctx)
{
    hi_s32 ret;
    drv_fe_tv_system en_tuner_system;
    drv_fe_tuner_ops *tuner_ops = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    time_cost_define(1);

    drv_fe_check_pointer(fe_ctrl_ctx);

    demod_ops = drv_fe_ctrl_get_demod_ops(signal->port);
    if (demod_ops->is_ctrl_tuner == NULL || !demod_ops->is_ctrl_tuner(signal->port)) {
        tuner_ops = drv_fe_ctrl_get_tuner_ops(signal->port);
        en_tuner_system = drv_fe_get_tuner_system(fe_ctrl_ctx);
        if (en_tuner_system != fe_ctrl_ctx->cur_tuner_system) {
            fe_ctrl_ctx->cur_tuner_system = en_tuner_system;
            ret = tuner_ops->set_system(signal->port, en_tuner_system);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(tuner_ops->set_system, ret);
                return ret;
            }
        }

        if ((tuner_ops->set_freq != NULL) && (signal->sig_type != HI_DRV_FRONTEND_SIG_TYPE_DVB_S)) {
            ret = tuner_ops->set_freq(signal->port, signal->signal.frequency);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(tuner_ops->set_freq, ret);
                return ret;
            }
        }
    }
    /* set demod connect attrs */
    time_cost_start(1);
    drv_fe_check_func_pointer(demod_ops->connect);
    ret = demod_ops->connect(signal->port, &signal->signal);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->connect, ret);
        return ret;
    }
    time_cost_end(1, "demod_connect");

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_connect(fe_ioctrl_signal  *signal)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;

    drv_fe_check_pointer(signal);
    drv_fe_ctrl_check_signal_type(signal->sig_type);
    drv_fe_ctrl_check_port(signal->port);

    ret = drv_fe_ctrl_check_attr_inited(signal->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }
    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(signal->port);

    ret = drv_fe_ctrl_connect_valid(signal, fe_ctrl_ctx);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_connect_valid, ret);
        return ret;
    }

    LOG_DBG_PRINT("[%d]--> %d : START. [%d_khz]\n", current->pid, signal->sig_type, signal->signal.frequency);

    ret = drv_fe_ctrl_connect_execute(signal, fe_ctrl_ctx);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_connect_execute, ret);
        return ret;
    }

    ret = memcpy_s(&fe_ctrl_ctx->connect_attr,  sizeof(frontend_acc_qam_params),
        &signal->signal,  sizeof(frontend_acc_qam_params));
    if (ret != HI_SUCCESS) {
        return HI_ERR_FRONTEND_SECURE_CHECK;
    }

    LOG_DBG_PRINT("[%d]--> SUCCESS.\n", current->pid);

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_close(hi_u32 port)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_tuner_ops *tuner_ops = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    LOG_DBG_PRINT("[%d]--> START : drv_fe_ctrl_close].\n", current->pid);

    drv_fe_ctrl_check_port(port);
    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(port);
    if (!fe_ctrl_ctx->attr_inited) {
        return HI_SUCCESS;
    }

    demod_ops = drv_fe_ctrl_get_demod_ops(port);
    if (demod_ops->is_ctrl_tuner == NULL || !demod_ops->is_ctrl_tuner(port)) {
        tuner_ops = drv_fe_ctrl_get_tuner_ops(port);
        if (tuner_ops->deinit != NULL) {
            ret = tuner_ops->deinit(port);
            if (ret != HI_SUCCESS) {
                HI_WARN_PRINT_FUNC_RES(tuner_ops->deinit, ret);
            }
        }
    }
    if (demod_ops->deinit != NULL) {
        ret = demod_ops->deinit(port);
        if (ret != HI_SUCCESS) {
            HI_WARN_PRINT_FUNC_RES(demod_ops->deinit, ret);
        }
    }
    if (fe_ctrl_ctx->lnb_ops.deinit != NULL) {
        fe_ctrl_ctx->lnb_ops.deinit(port);
    }

    ret = memset_s(fe_ctrl_ctx, sizeof(drv_fe_ctrl_ctx), 0, sizeof(drv_fe_ctrl_ctx));
    if (ret != HI_SUCCESS) {
        LOG_WARN_PRINT("Clear drv_fe_ctrl_ctx error!\n");
    }
    g_fe_ctrl_ctx[port].attr_set_err_print_enable = HI_TRUE;
    g_fe_ctrl_ctx[port].attr_inited = HI_FALSE;

    LOG_DBG_PRINT("[%d]--> SUCCESS.\n", current->pid);

    return HI_SUCCESS;
}

static hi_drv_modulation_type drv_fe_ctrl_convert_qam_type(frontend_qam_type qam_type)
{
    switch (qam_type) {
        case QAM_TYPE_16:
                return HI_DRV_MOD_TYPE_QAM_16;
        case QAM_TYPE_32:
                return HI_DRV_MOD_TYPE_QAM_32;
        case QAM_TYPE_128:
                return HI_DRV_MOD_TYPE_QAM_128;
        case QAM_TYPE_256:
                return HI_DRV_MOD_TYPE_QAM_256;
        case QAM_TYPE_64:
        default:
                return HI_DRV_MOD_TYPE_QAM_64;
    }
    return HI_DRV_MOD_TYPE_QAM_64;
}

static hi_s32 drv_fe_ctrl_get_status_para(fe_ioctrl_status  *status)
{
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(status->port);

    status->connect_para.sig_type = fe_ctrl_ctx->attr.sig_type;
    switch (fe_ctrl_ctx->attr.sig_type) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T:
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T2:
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_T:
        case HI_DRV_FRONTEND_SIG_TYPE_ATSC_T:
        case HI_DRV_FRONTEND_SIG_TYPE_DTMB:
            status->connect_para.connect_para.ter.freq =  fe_ctrl_ctx->connect_attr.frequency;
            status->connect_para.connect_para.ter.band_width = fe_ctrl_ctx->connect_attr.srbw.band_width;
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_C:
        case HI_DRV_FRONTEND_SIG_TYPE_J83B:
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_C:
            status->connect_para.connect_para.cab.freq =  fe_ctrl_ctx->connect_attr.frequency;
            status->connect_para.connect_para.cab.symbol_rate = fe_ctrl_ctx->connect_attr.srbw.symbol_rate;
            status->connect_para.connect_para.cab.mod_type =
                drv_fe_ctrl_convert_qam_type(fe_ctrl_ctx->connect_attr.modpol.qam_type);
            status->connect_para.connect_para.cab.reverse = fe_ctrl_ctx->connect_attr.si;
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S:
            status->connect_para.connect_para.sat.freq =  fe_ctrl_ctx->connect_attr.frequency;
            status->connect_para.connect_para.sat.symbol_rate =  fe_ctrl_ctx->connect_attr.srbw.symbol_rate;
            status->connect_para.connect_para.sat.polar =  fe_ctrl_ctx->connect_attr.modpol.polar;
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S:
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_S3:
            status->connect_para.connect_para.sat.freq = fe_ctrl_ctx->connect_attr.frequency;
            status->connect_para.connect_para.sat.polar =  fe_ctrl_ctx->connect_attr.modpol.polar;
            break;
        default:
            LOG_ERR_PRINT("not support for [sig_type:%d] now.\n", fe_ctrl_ctx->attr.sig_type);
            break;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_get_status(fe_ioctrl_status  *status)
{
    hi_s32 ret;
    drv_fe_demod_ops *demod_ops = NULL;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;

    drv_fe_check_pointer(status);
    drv_fe_ctrl_check_port(status->port);
    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(status->port);

    if (fe_ctrl_ctx->attr_inited != HI_TRUE) {
        if (fe_ctrl_ctx->attr_set_err_print_enable == HI_TRUE) {
            LOG_ERR_PRINT("tuner set error or not set attr!(port:%d)\n", status->port);
            LOG_ERR_PRINT("set tuner cmd:\n");
            LOG_ERR_PRINT("vi /usr/local/cfg/frontend_config.ini\n");
            LOG_ERR_PRINT("modify tuner type and i2c addr\n");
            fe_ctrl_ctx->attr_set_err_print_enable = HI_FALSE;
        }
        return HI_ERR_FRONTEND_NOT_INIT;
    }

    if (fe_ctrl_ctx->print_enable == HI_TRUE) {
        LOG_DBG_PRINT("[%d]--> START.\n", current->pid);
    }

    if (fe_ctrl_ctx->connect_attr.frequency == 0) { /* never connect */
        status->lock_status = HI_DRV_FRONTEND_LOCK_STATUS_DROPPED;
        return HI_SUCCESS;
    }

    demod_ops = drv_fe_ctrl_get_demod_ops(status->port);
    drv_fe_check_func_pointer(demod_ops->get_status);
    ret = demod_ops->get_status(status->port, &status->lock_status);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->get_status, ret);
        return ret;
    }
    fe_ctrl_ctx->resume_info.lock_status = status->lock_status;

    ret = drv_fe_ctrl_get_status_para(status);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_get_status_para, ret);
        return ret;
    }

    if (fe_ctrl_ctx->print_enable == HI_TRUE) {
        LOG_DBG_PRINT("[%d]--> status : %s\n", current->pid,
            (status->lock_status == HI_DRV_FRONTEND_LOCK_STATUS_LOCKED) ? "LOCK" : "NOLOCK");
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_get_signal_strength(fe_ioctrl_data_buf *signal_strength)
{
    hi_s32 ret;
    drv_fe_demod_ops *demod_ops = NULL;
    drv_fe_tuner_ops *tuner_ops = NULL;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;

    drv_fe_check_pointer(signal_strength);
    drv_fe_ctrl_check_port(signal_strength->port);
    ret = drv_fe_ctrl_check_attr_inited(signal_strength->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(signal_strength->port);
    if (fe_ctrl_ctx->connect_attr.frequency == 0) {
        signal_strength->data_buf[1] = 0;
        return HI_ERR_FRONTEND_NOT_CONNECT;
    }

    demod_ops = drv_fe_ctrl_get_demod_ops(signal_strength->port);

    /* use tuner's signal strength first */
    if (demod_ops->is_ctrl_tuner == NULL || !demod_ops->is_ctrl_tuner(signal_strength->port)) {
        tuner_ops = drv_fe_ctrl_get_tuner_ops(signal_strength->port);
        if (tuner_ops->get_signal_strength != NULL) {
            ret = tuner_ops->get_signal_strength(signal_strength->port, signal_strength->data_buf);
            if (ret != HI_SUCCESS) {
                HI_ERR_PRINT_FUNC_RES(tuner_ops->get_signal_strength, ret);
            }
            return ret;
        }
    }

    drv_fe_check_func_pointer(demod_ops->get_signal_strength);
    ret = demod_ops->get_signal_strength(signal_strength->port, signal_strength->data_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->get_signal_strength, ret);
        return ret;
    }

    LOG_DBG_PRINT("strength:%d\n", signal_strength->data_buf[1]);

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_get_ber(fe_ioctrl_ber_per *ber)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(ber);
    drv_fe_ctrl_check_port(ber->port);
    ret = drv_fe_ctrl_check_attr_inited(ber->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(ber->port);
    if (fe_ctrl_ctx->connect_attr.frequency == 0) {
        ber->num.decimal_val = 0;
        ber->num.integer_val = 0;
        ber->num.power = 0;
        return HI_ERR_FRONTEND_NOT_CONNECT;
    }

    demod_ops = drv_fe_ctrl_get_demod_ops(ber->port);
    drv_fe_check_func_pointer(demod_ops->get_ber);
    ret = demod_ops->get_ber(ber->port, &ber->num);
    if ((ret != HI_SUCCESS) && (ret != HI_ERR_FRONTEND_NOT_CONNECT)) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->get_ber, ret);
    }

    return ret;
}

hi_s32 drv_fe_ctrl_get_per(fe_ioctrl_ber_per *per)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    if (per == NULL) {
        LOG_ERR_PRINT("per null pointer\n");
        return HI_ERR_FRONTEND_INVALID_POINT;
    }

    if (per->port >= DRV_FRONTEND_NUM) {
        LOG_ERR_PRINT("invalid port:%d\n", per->port);
        return HI_ERR_FRONTEND_INVALID_PORT;
    }

    if (g_fe_ctrl_ctx[per->port].attr_inited != HI_TRUE) {
        LOG_ERR_PRINT("[%d] : set attr first!(port:%d)\n", current->pid, per->port);
        return HI_ERR_FRONTEND_NOT_INIT;
    }

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(per->port);
    if (fe_ctrl_ctx->connect_attr.frequency == 0) {
        per->num.decimal_val = 0;
        per->num.integer_val = 0;
        per->num.power = 0;
        return HI_ERR_FRONTEND_NOT_CONNECT;
    }

    demod_ops = drv_fe_ctrl_get_demod_ops(per->port);
    if (demod_ops->get_per == NULL) {
        LOG_ERR_PRINT("Not support this function\n");
        return HI_ERR_FRONTEND_INVALID_FUNCTION;
    }

    ret = demod_ops->get_per(per->port, &per->num);
    if ((ret != HI_SUCCESS) && (ret != HI_ERR_FRONTEND_NOT_CONNECT)) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->get_per, ret);
    }

    return ret;
}

hi_s32 drv_fe_ctrl_get_snr(fe_ioctrl_snr *snr)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(snr);
    drv_fe_ctrl_check_port(snr->port);
    ret = drv_fe_ctrl_check_attr_inited(snr->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(snr->port);
    if (fe_ctrl_ctx->connect_attr.frequency == 0) {
        snr->snr = 0;
        return HI_ERR_FRONTEND_NOT_CONNECT;
    }

    demod_ops = drv_fe_ctrl_get_demod_ops(snr->port);
    drv_fe_check_func_pointer(demod_ops->get_snr);
    ret = demod_ops->get_snr(snr->port, &snr->snr);
    if ((ret != HI_SUCCESS) && (ret != HI_ERR_FRONTEND_NOT_CONNECT)) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->get_snr, ret);
    }

    return ret;
}

static hi_s32 drv_fe_ctrl_set_attr_valid(fe_ioctrl_attr *attr)
{
    if (g_fe_ctrl_ctx[attr->port].attr.tuner_dev_type >= HI_DRV_TUNER_DEV_TYPE_MAX) {
        LOG_ERR_PRINT("tuner_dev_type is out of bound\n");
        LOG_ERR_PRINT("tuner_dev_type:%d\n", g_fe_ctrl_ctx[attr->port].attr.tuner_dev_type);
        return HI_ERR_FRONTEND_INVALID_TUNERTYPE;
    }

    if (g_fe_ctrl_ctx[attr->port].attr.demod_dev_type < HI_DRV_DEMOD_DEV_TYPE_3130I) {
        LOG_ERR_PRINT("demod_dev_type:%d is err.\n", g_fe_ctrl_ctx[attr->port].attr.demod_dev_type);
        return HI_ERR_FRONTEND_INVALID_DEMODTYPE;
    }

    if (g_fe_ctrl_ctx[attr->port].attr.demod_dev_type >= HI_DRV_DEMOD_DEV_TYPE_MAX) {
        LOG_ERR_PRINT("demod_dev_type is out of bound\n");
        LOG_ERR_PRINT("demod_dev_type:%d\n", g_fe_ctrl_ctx[attr->port].attr.demod_dev_type);
        return HI_ERR_FRONTEND_INVALID_DEMODTYPE;
    }

    if (attr->attr.tuner_dev_type >= HI_DRV_TUNER_DEV_TYPE_MAX) {
        LOG_ERR_PRINT("tuner_dev_type is out of bound\n");
        LOG_ERR_PRINT("tuner_dev_type:%d\n", attr->attr.tuner_dev_type);
        return HI_ERR_FRONTEND_INVALID_TUNERTYPE;
    }

    if (attr->attr.demod_dev_type < HI_DRV_DEMOD_DEV_TYPE_3130I) {
        LOG_ERR_PRINT("demod_dev_type:%d is err.\n", attr->attr.demod_dev_type);
        return HI_ERR_FRONTEND_INVALID_DEMODTYPE;
    }

    if (attr->attr.demod_dev_type >= HI_DRV_DEMOD_DEV_TYPE_MAX) {
        LOG_ERR_PRINT("demod_dev_type is out of bound\n");
        LOG_ERR_PRINT("demod_dev_type:%d\n", (attr->attr.demod_dev_type - HI_DRV_DEMOD_DEV_TYPE_3130I));
        return HI_ERR_FRONTEND_INVALID_DEMODTYPE;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_fe_ctrl_set_attr_close(fe_ioctrl_attr *attr)
{
    hi_s32 ret;

    LOG_DBG_PRINT("--->[%d] : tuner_id = %d is already used, [tuner : %d, demo : %d]",
        current->pid, attr->port, g_fe_ctrl_ctx[attr->port].attr.tuner_dev_type,
        g_fe_ctrl_ctx[attr->port].attr.demod_dev_type);

    LOG_DBG_PRINT("--->[tuner : %d, demo : %d], please disconnect first.\n",
        attr->attr.tuner_dev_type, attr->attr.demod_dev_type);

    ret = drv_fe_ctrl_close(attr->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_close, ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_fe_ctrl_set_attr_config(fe_ioctrl_attr *attr, drv_fe_ctrl_ctx *fe_ctrl_ctx)
{
    hi_s32 ret;

    /* register demod functions */
    ret = drv_fe_register_demod_func(fe_ctrl_ctx);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_register_demod_func, ret);
        return ret;
    }

    if (fe_ctrl_ctx->demod_ops.is_ctrl_tuner == NULL || !fe_ctrl_ctx->demod_ops.is_ctrl_tuner(attr->port)) {
        /* register tuner functions */
        ret = drv_fe_register_tuner_func(fe_ctrl_ctx);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(drv_fe_register_tuner_func, ret);
            return ret;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 drv_fe_ctrl_reset_demod(hi_u32 reset_gpio_group, hi_u32 reset_gpio_bit)
{
    hi_s32 ret;
    hi_u32 reset_gpio_no = reset_gpio_group * 8 + reset_gpio_bit; /* 8 bit per byte */
    LOG_DBG_PRINT("reset_gpio: GPIO%d_%d\n", reset_gpio_group, reset_gpio_bit);
    if (reset_gpio_no != 0) {
        ret = g_fe_ctrl_func.gpio_func->pfn_gpio_direction_set_bit(reset_gpio_no, HI_FALSE);
        if (ret != HI_SUCCESS) {
            LOG_ERR_PRINT("pfnGpioDirSetBit failed !\n");
            return ret;
        }
        ret = g_fe_ctrl_func.gpio_func->pfn_gpio_write_bit(reset_gpio_no, 0);
        if (ret != HI_SUCCESS) {
            LOG_ERR_PRINT("pfnGpioDirSetBit failed !\n");
            return ret;
        }
        osal_msleep(100); /* delay 100 ms for reset */
        ret = g_fe_ctrl_func.gpio_func->pfn_gpio_write_bit(reset_gpio_no, 1);
        if (ret != HI_SUCCESS) {
            LOG_ERR_PRINT("Demod reset failed !\n");
            return ret;
        }
        osal_msleep(50); /* delay 50 ms for reset */
    }
    return HI_SUCCESS;
}

static hi_s32 drv_fe_ctrl_set_attr_execute(fe_ioctrl_attr *attr, drv_fe_ctrl_ctx *fe_ctrl_ctx)
{
    hi_s32 ret;

    drv_fe_tuner_ops *tuner_ops = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    time_cost_define(1);
    ret = drv_fe_ctrl_reset_demod(attr->attr.ext_dem_reset_gpio_group, attr->attr.ext_dem_reset_gpio_bit);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_reset_demod, ret);
        return ret;
    }
    demod_ops = drv_fe_ctrl_get_demod_ops(attr->port);

    /* init tuner/demod */
    time_cost_start(1);
    drv_fe_check_func_pointer(demod_ops->init);
    ret = demod_ops->init(attr->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->init, ret);
        return ret;
    }
    time_cost_end(1, "demod_init");

    if (demod_ops->is_ctrl_tuner == NULL || !demod_ops->is_ctrl_tuner(attr->port)) {
        tuner_ops = drv_fe_ctrl_get_tuner_ops(attr->port);

        time_cost_start(1);
        drv_fe_check_func_pointer(tuner_ops->init);
        ret = tuner_ops->init(attr->port);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(tuner_ops->init, ret);
            return ret;
        }
        time_cost_end(1, "tuner_init");

        time_cost_start(1);
        fe_ctrl_ctx->cur_tuner_system = drv_fe_get_tuner_system(fe_ctrl_ctx);
        drv_fe_check_func_pointer(tuner_ops->set_system);
        ret = tuner_ops->set_system(attr->port, fe_ctrl_ctx->cur_tuner_system);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(tuner_ops->set_system, ret);
            return ret;
        }
        time_cost_end(1, "tuner_system");
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_set_attr(fe_ioctrl_attr *attr)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;

    drv_fe_check_pointer(attr);
    drv_fe_ctrl_check_signal_type(attr->attr.sig_type);
    drv_fe_ctrl_check_port(attr->port);
    drv_fe_ctrl_check_tuner_type(attr->attr.tuner_dev_type);
    drv_fe_ctrl_check_demod_type(attr->attr.demod_dev_type);

    /* set inited flag, just only one channel works! */
    if (g_fe_ctrl_ctx[attr->port].attr_inited == HI_TRUE) {
        ret = drv_fe_ctrl_set_attr_valid(attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_set_attr_valid, ret);
            return ret;
        }

        ret = drv_fe_ctrl_set_attr_close(attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_set_attr_close, ret);
            return ret;
        }
    }

    LOG_DBG_PRINT("[%d]--> %d : START.\n", current->pid, attr->attr.sig_type);

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(attr->port);
    ret = memcpy_s(&fe_ctrl_ctx->attr,  sizeof(hi_drv_frontend_attr),
        &attr->attr, sizeof(hi_drv_frontend_attr));
    if (ret != HI_SUCCESS) {
        return HI_ERR_FRONTEND_SECURE_CHECK;
    }

    ret = drv_fe_ctrl_set_attr_config(attr, fe_ctrl_ctx);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_set_attr_config, ret);
        return ret;
    }

    ret = drv_fe_ctrl_set_attr_execute(attr, fe_ctrl_ctx);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_set_attr_execute, ret);
        return ret;
    }

    fe_ctrl_ctx->attr_inited = HI_TRUE;
    fe_ctrl_ctx->auto_test   = HI_FALSE;
    fe_ctrl_ctx->attr_set_err_print_enable = HI_TRUE;

    LOG_DBG_PRINT("[%d]--> SUCCESS.\n", current->pid);

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_get_attr(fe_ioctrl_attr *attr)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;

    drv_fe_check_pointer(attr);
    drv_fe_ctrl_check_port(attr->port);
    ret = drv_fe_ctrl_check_attr_inited(attr->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(attr->port);
    ret = memcpy_s(&attr->attr, sizeof(hi_drv_frontend_attr), &fe_ctrl_ctx->attr, sizeof(hi_drv_frontend_attr));
    if (ret != HI_SUCCESS) {
        return HI_ERR_FRONTEND_SECURE_CHECK;
    }

    LOG_DBG_PRINT("[%d]--> SUCCESS.\n", current->pid);

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_get_freq_symb_offset(fe_ioctrl_data_buf *freq_symb_offset)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;
    hi_u32 port_id;
    hi_u32 symb = 0;
    hi_s32 freq_offset = 0;

    drv_fe_check_pointer(freq_symb_offset);
    drv_fe_ctrl_check_port(freq_symb_offset->port);
    ret = drv_fe_ctrl_check_attr_inited(freq_symb_offset->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    port_id = freq_symb_offset->port;
    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(port_id);

    demod_ops = drv_fe_ctrl_get_demod_ops(freq_symb_offset->port);
    drv_fe_check_func_pointer(demod_ops->get_freq_symb_offset);
    ret = demod_ops->get_freq_symb_offset(port_id, &freq_offset, &symb);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->get_freq_symb_offset, ret);
        return ret;
    }
    freq_symb_offset->data_buf[0] = fe_ctrl_ctx->connect_attr.frequency + freq_offset;
    freq_symb_offset->data_buf[1] = symb;

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_get_signal_info(fe_ioctrl_signal_info *signal_info)
{
    hi_s32 ret;
    drv_fe_demod_ops *demod_ops = NULL;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;

    drv_fe_check_pointer(signal_info);
    drv_fe_ctrl_check_port(signal_info->port);
    ret = drv_fe_ctrl_check_attr_inited(signal_info->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    demod_ops = drv_fe_ctrl_get_demod_ops(signal_info->port);
    drv_fe_check_func_pointer(demod_ops->get_signal_info);
    ret = demod_ops->get_signal_info(signal_info->port, &signal_info->info);
    if (ret == HI_ERR_FRONTEND_NOT_CONNECT) {
        HI_WARN_PRINT_FUNC_RES(demod_ops->get_signal_info, ret);
        return ret;
    } else if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->get_signal_info, ret);
        return ret;
    }
    LOG_DBG_PRINT("signal_info->info.sig_type:%d\n", signal_info->info.sig_type);

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(signal_info->port);
    LOG_DBG_PRINT("fe_ctrl_ctx->attr.sig_type:%d\n", fe_ctrl_ctx->attr.sig_type);

    fe_ctrl_ctx->attr.sig_type = signal_info->info.sig_type;
    switch (fe_ctrl_ctx->attr.sig_type) {
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T:
            signal_info->info.signal_info.dvbt.freq = fe_ctrl_ctx->connect_attr.frequency;
            signal_info->info.signal_info.dvbt.band_width = fe_ctrl_ctx->connect_attr.srbw.band_width;
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_T2:
            signal_info->info.signal_info.dvbt2.freq = fe_ctrl_ctx->connect_attr.frequency;
            signal_info->info.signal_info.dvbt2.band_width = fe_ctrl_ctx->connect_attr.srbw.band_width;
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_ISDB_T:
            signal_info->info.signal_info.isdbt.freq = fe_ctrl_ctx->connect_attr.frequency;
            signal_info->info.signal_info.isdbt.band_width = fe_ctrl_ctx->connect_attr.srbw.band_width;
            break;
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S:
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S2:
        case HI_DRV_FRONTEND_SIG_TYPE_DVB_S2X:
            signal_info->info.signal_info.dvbs2x.freq = fe_ctrl_ctx->connect_attr.frequency;
            signal_info->info.signal_info.dvbs2x.symbol_rate = fe_ctrl_ctx->connect_attr.srbw.symbol_rate;
            break;
        default:
            break;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_set_standby(fe_ioctrl_standby *standby)
{
    hi_s32 ret;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(standby);
    drv_fe_ctrl_check_port(standby->port);
    ret = drv_fe_ctrl_check_attr_inited(standby->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    demod_ops = drv_fe_ctrl_get_demod_ops(standby->port);
    drv_fe_check_func_pointer(demod_ops->standby);
    ret = demod_ops->standby(standby->port, standby->standby);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->standby, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_set_ts_out(fe_ioctrl_ts_out *ts_out)
{
    hi_s32 ret;
    drv_fe_demod_ops *demod_ops = NULL;

    if (ts_out == NULL) {
        LOG_ERR_PRINT("ts_out null pointer\n");
        return HI_ERR_FRONTEND_INVALID_POINT;
    }

    if (ts_out->port >= DRV_FRONTEND_NUM) {
        LOG_ERR_PRINT("invalid port:%d\n", ts_out->port);
        return HI_ERR_FRONTEND_INVALID_PORT;
    }

    demod_ops = drv_fe_ctrl_get_demod_ops(ts_out->port);

    if (demod_ops->set_ts_out == NULL) {
        LOG_ERR_PRINT("not support this function\n");
        return HI_ERR_FRONTEND_INVALID_FUNCTION;
    }

    ret = demod_ops->set_ts_out(ts_out->port, &ts_out->ts_out);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->set_ts_out, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_get_ts_out(fe_ioctrl_ts_out *ts_out)
{
    hi_s32 i;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;

    if (ts_out == NULL) {
        LOG_ERR_PRINT("ts_out null pointer\n");
        return HI_ERR_FRONTEND_INVALID_POINT;
    }

    if (ts_out->port >= DRV_FRONTEND_NUM) {
        LOG_ERR_PRINT("invalid port:%d\n", ts_out->port);
        return HI_ERR_FRONTEND_INVALID_PORT;
    }

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(ts_out->port);

    /* If TS has been configured, directly return the saved parameters. */
    if (fe_ctrl_ctx->resume_info.set_ts_out) {
        ts_out->ts_out = fe_ctrl_ctx->resume_info.ts_out;
        return HI_SUCCESS;
    }
    /* return the default parameters. */
    ts_out->ts_out.ts_clk_edge = HI_DRV_DEMOD_TSCLK_EDGE_SINGLE;
    ts_out->ts_out.ts_clk_polar = HI_DRV_DEMOD_TS_CLK_POLAR_RISING;
    ts_out->ts_out.ts_mode = HI_DRV_DEMOD_TS_MODE_SERIAL;
    ts_out->ts_out.ts_format = HI_DRV_DEMOD_TS_FORMAT_TS;
    for (i = 0; i < HI_DRV_MAX_TS_LINE; i++) {
        ts_out->ts_out.ts_pin[i] = i;
    }
    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_get_quality(fe_ioctrl_data *quality)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(quality);
    drv_fe_ctrl_check_port(quality->port);

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(quality->port);

    demod_ops = drv_fe_ctrl_get_demod_ops(quality->port);
    if (demod_ops->get_signal_quality == NULL) {
        return HI_ERR_FRONTEND_INVALID_FUNCTION;
    }
    ret = demod_ops->get_signal_quality(quality->port, &quality->data);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->get_signal_quality, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_get_plp_num(fe_ioctrl_plp_num *plp_num)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;
    hi_u8 plp_number = 0;

    drv_fe_check_pointer(plp_num);
    drv_fe_ctrl_check_port(plp_num->port);
    ret = drv_fe_ctrl_check_attr_inited(plp_num->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(plp_num->port);

    demod_ops = drv_fe_ctrl_get_demod_ops(plp_num->port);
    drv_fe_check_func_pointer(demod_ops->get_plp_num);
    ret = demod_ops->get_plp_num(plp_num->port, &plp_number);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->get_plp_num, ret);
        return ret;
    }
    plp_num->plp_num = plp_number;

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_set_plp_para(fe_ioctrl_set_plp_para *set_plp_para)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(set_plp_para);
    drv_fe_ctrl_check_port(set_plp_para->port);
    ret = drv_fe_ctrl_check_attr_inited(set_plp_para->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(set_plp_para->port);
    drv_fe_ctrl_check_is_dtv_t(fe_ctrl_ctx->attr.sig_type);

    demod_ops = drv_fe_ctrl_get_demod_ops(set_plp_para->port);
    drv_fe_check_func_pointer(demod_ops->set_plp_para);
    ret = demod_ops->set_plp_para(set_plp_para->port, &set_plp_para->plp_para);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->set_plp_para, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_get_plp_info(fe_ioctrl_get_plp_info *get_plp_info)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(get_plp_info);
    drv_fe_ctrl_check_port(get_plp_info->port);
    ret = drv_fe_ctrl_check_attr_inited(get_plp_info->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(get_plp_info->port);
    drv_fe_ctrl_check_is_dtv_t(fe_ctrl_ctx->attr.sig_type);

    demod_ops = drv_fe_ctrl_get_demod_ops(get_plp_info->port);
    drv_fe_check_func_pointer(demod_ops->get_plp_info);
    ret = demod_ops->get_plp_info(get_plp_info->port, get_plp_info->index, &get_plp_info->plp_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->get_plp_info, ret);
        return ret;
    }

    LOG_DBG_PRINT("plp_id:%u, plp_type:%u, plp_grp_id:%u\n",
        get_plp_info->plp_info.plp_id, get_plp_info->plp_info.plp_type,
        get_plp_info->plp_info.plp_grp_id);

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_set_sat_attr(fe_ioctrl_sat_attr *sat_attr)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_lnb_ops *lnb_ops = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(sat_attr);
    drv_fe_ctrl_check_port(sat_attr->port);

    ret = drv_fe_ctrl_check_attr_inited(sat_attr->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    LOG_DBG_PRINT("lnb_ctrl_dev:%d\n", sat_attr->sat_attr.lnb_ctrl_dev);
    LOG_DBG_PRINT("lnb_dev_address:%d\n", sat_attr->sat_attr.lnb_dev_address);

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(sat_attr->port);
    drv_fe_ctrl_check_is_dtv_s(fe_ctrl_ctx->attr.sig_type);

    ret = memcpy_s(&fe_ctrl_ctx->sat_attr,  sizeof(fe_ctrl_ctx->sat_attr),
        &sat_attr->sat_attr, sizeof(hi_drv_frontend_sat_attr));
    if (ret != HI_SUCCESS) {
        return HI_ERR_FRONTEND_SECURE_CHECK;
    }

    demod_ops = drv_fe_ctrl_get_demod_ops(sat_attr->port);
    if (demod_ops->set_sat_attr != NULL) {
        ret = demod_ops->set_sat_attr(sat_attr->port, &sat_attr->sat_attr);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(demod_ops->set_sat_attr, ret);
            return ret;
        }
    } else {
        LOG_INFO_PRINT("This demod not support set_sat_attr function.\n");
    }

    /* Register Lnb Functions For Satellite Signal */
    ret = drv_fe_register_lnb_func(fe_ctrl_ctx);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_register_lnb_func, ret);
        return ret;
    }

    lnb_ops = drv_fe_ctrl_get_lnb_ops(sat_attr->port);
    drv_fe_check_func_pointer(lnb_ops->init);
    ret = lnb_ops->init(sat_attr->port, sat_attr->sat_attr.lnb_i2c_channel,
        sat_attr->sat_attr.lnb_dev_address);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(lnb_ops->init, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_set_blindscan_init(fe_ioctrl_blindscan *blindscan)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(blindscan);
    drv_fe_ctrl_check_port(blindscan->port);

    ret = drv_fe_ctrl_check_attr_inited(blindscan->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(blindscan->port);
    drv_fe_ctrl_check_is_dtv_s(fe_ctrl_ctx->attr.sig_type);

    demod_ops = drv_fe_ctrl_get_demod_ops(blindscan->port);
    drv_fe_check_func_pointer(demod_ops->blindscan_init);
    ret = demod_ops->blindscan_init(blindscan->port, &blindscan->para);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->blindscan_init, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_set_blindscan_action(fe_ioctrl_blindscan_info *blindscan_action)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(blindscan_action);
    drv_fe_ctrl_check_port(blindscan_action->port);

    ret = drv_fe_ctrl_check_attr_inited(blindscan_action->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(blindscan_action->port);
    drv_fe_ctrl_check_is_dtv_s(fe_ctrl_ctx->attr.sig_type);

    demod_ops = drv_fe_ctrl_get_demod_ops(blindscan_action->port);
    drv_fe_check_func_pointer(demod_ops->blindscan_action);
    ret = demod_ops->blindscan_action(blindscan_action->port, &blindscan_action->para);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->blindscan_action, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_set_blindscan_abort(fe_ioctrl_blindscan_abort *blindscan_abort)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(blindscan_abort);
    drv_fe_ctrl_check_port(blindscan_abort->port);

    ret = drv_fe_ctrl_check_attr_inited(blindscan_abort->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(blindscan_abort->port);
    drv_fe_ctrl_check_is_dtv_s(fe_ctrl_ctx->attr.sig_type);

    demod_ops = drv_fe_ctrl_get_demod_ops(blindscan_abort->port);
    drv_fe_check_func_pointer(demod_ops->blindscan_action);
    ret = demod_ops->blindscan_abort(blindscan_abort->port, blindscan_abort->stop_quit);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->blindscan_abort, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_set_lnb_out(fe_ioctrl_lnb_out *lnb_out)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx;
    drv_fe_demod_ops *demod_ops = NULL;
    drv_fe_lnb_ops *lnb_ops = NULL;

    drv_fe_check_pointer(lnb_out);
    drv_fe_ctrl_check_port(lnb_out->port);

    ret = drv_fe_ctrl_check_attr_inited(lnb_out->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(lnb_out->port);
    drv_fe_ctrl_check_is_dtv_s(fe_ctrl_ctx->attr.sig_type);

    lnb_ops = drv_fe_ctrl_get_lnb_ops(lnb_out->port);
    if (lnb_ops->set_lnb_out) {
        drv_fe_check_func_pointer(lnb_ops->set_lnb_out);
        ret = lnb_ops->set_lnb_out(lnb_out->port, lnb_out->out);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(lnb_ops->set_lnb_out, ret);
            return ret;
        }
    } else {
        demod_ops = drv_fe_ctrl_get_demod_ops(lnb_out->port);
        drv_fe_check_func_pointer(demod_ops->set_lnb_out);
        ret = demod_ops->set_lnb_out(lnb_out->port, lnb_out->out);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(demod_ops->set_lnb_out, ret);
            return ret;
        }
    }
    fe_ctrl_ctx->resume_info.lnb_out_level = lnb_out->out;
    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_set_continuous_22k(fe_ioctrl_continuous_22k *continuous_22k)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(continuous_22k);
    drv_fe_ctrl_check_port(continuous_22k->port);

    ret = drv_fe_ctrl_check_attr_inited(continuous_22k->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(continuous_22k->port);
    drv_fe_ctrl_check_is_dtv_s(fe_ctrl_ctx->attr.sig_type);

    demod_ops = drv_fe_ctrl_get_demod_ops(continuous_22k->port);
    drv_fe_check_func_pointer(demod_ops->send_continuous22k);
    ret = demod_ops->send_continuous22k(continuous_22k->port, continuous_22k->continuous_22k);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->send_continuous22k, ret);
        return ret;
    }

    fe_ctrl_ctx->resume_info.continuous22k = continuous_22k->continuous_22k;
    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_set_send_tone(fe_ioctrl_tone *tone)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(tone);
    drv_fe_ctrl_check_port(tone->port);

    ret = drv_fe_ctrl_check_attr_inited(tone->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(tone->port);
    drv_fe_ctrl_check_is_dtv_s(fe_ctrl_ctx->attr.sig_type);

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    demod_ops = drv_fe_ctrl_get_demod_ops(tone->port);
    drv_fe_check_func_pointer(demod_ops->send_tone);
    ret = demod_ops->send_tone(tone->port, tone->tone);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->send_tone, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_set_diseqc_send_msg(fe_ioctrl_diseqc_sendmsg *diseqc_send_msg)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(diseqc_send_msg);
    drv_fe_ctrl_check_port(diseqc_send_msg->port);

    ret = drv_fe_ctrl_check_attr_inited(diseqc_send_msg->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(diseqc_send_msg->port);
    drv_fe_ctrl_check_is_dtv_s(fe_ctrl_ctx->attr.sig_type);

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    demod_ops = drv_fe_ctrl_get_demod_ops(diseqc_send_msg->port);
    drv_fe_check_func_pointer(demod_ops->diseqc_send_msg);
    ret = demod_ops->diseqc_send_msg(diseqc_send_msg->port, &(diseqc_send_msg->send_msg));
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->diseqc_send_msg, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_set_diseqc_recv_msg(fe_ioctrl_diseqc_recvmsg *diseqc_recv_msg)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(diseqc_recv_msg);
    drv_fe_ctrl_check_port(diseqc_recv_msg->port);

    ret = drv_fe_ctrl_check_attr_inited(diseqc_recv_msg->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(diseqc_recv_msg->port);
    drv_fe_ctrl_check_is_dtv_s(fe_ctrl_ctx->attr.sig_type);

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    demod_ops = drv_fe_ctrl_get_demod_ops(diseqc_recv_msg->port);
    drv_fe_check_func_pointer(demod_ops->diseqc_recv_msg);
    ret = demod_ops->diseqc_recv_msg(diseqc_recv_msg->port, &(diseqc_recv_msg->recv_msg));
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->diseqc_recv_msg, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_get_stream_num(fe_ioctrl_get_stream_num *get_stream_num)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(get_stream_num);
    drv_fe_ctrl_check_port(get_stream_num->port);

    ret = drv_fe_ctrl_check_attr_inited(get_stream_num->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(get_stream_num->port);
    drv_fe_ctrl_check_is_dtv_s(fe_ctrl_ctx->attr.sig_type);

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    demod_ops = drv_fe_ctrl_get_demod_ops(get_stream_num->port);
    drv_fe_check_func_pointer(demod_ops->get_stream_num);
    ret = demod_ops->get_stream_num(get_stream_num->port, &(get_stream_num->stream_num));
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->get_stream_num, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_get_isi_id(fe_ioctrl_get_isi_id *get_isi_id)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(get_isi_id);
    drv_fe_ctrl_check_port(get_isi_id->port);

    ret = drv_fe_ctrl_check_attr_inited(get_isi_id->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(get_isi_id->port);
    drv_fe_ctrl_check_is_dtv_s(fe_ctrl_ctx->attr.sig_type);

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    demod_ops = drv_fe_ctrl_get_demod_ops(get_isi_id->port);
    drv_fe_check_func_pointer(demod_ops->get_stream_num);
    ret = demod_ops->get_isi_id(get_isi_id->port, get_isi_id->index, &(get_isi_id->isi_id));
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->get_isi_id, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_set_isi_id(fe_ioctrl_set_isi_id *set_isi_id)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx *fe_ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(set_isi_id);
    drv_fe_ctrl_check_port(set_isi_id->port);

    ret = drv_fe_ctrl_check_attr_inited(set_isi_id->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    fe_ctrl_ctx = drv_fe_ctrl_get_ctx(set_isi_id->port);
    drv_fe_ctrl_check_is_dtv_s(fe_ctrl_ctx->attr.sig_type);

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    demod_ops = drv_fe_ctrl_get_demod_ops(set_isi_id->port);
    drv_fe_check_func_pointer(demod_ops->get_stream_num);
    ret = demod_ops->set_isi_id(set_isi_id->port, set_isi_id->isi_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->set_isi_id, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 drv_fe_ctrl_monitor_layers_config(fe_ioctrl_receive_config *receive_config)
{
    hi_s32 ret;
    drv_fe_demod_ops *demod_ops = NULL;

    drv_fe_check_pointer(receive_config);
    drv_fe_ctrl_check_port(receive_config->port);
    ret = drv_fe_ctrl_check_attr_inited(receive_config->port);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_check_attr_inited, ret);
        return ret;
    }

    LOG_DBG_PRINT("[%d]--> START.\n", current->pid);

    demod_ops = drv_fe_ctrl_get_demod_ops(receive_config->port);
    drv_fe_check_func_pointer(demod_ops->monitor_layers_config);
    ret = demod_ops->monitor_layers_config(receive_config->port, receive_config->receive_config);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(demod_ops->monitor_layers_config, ret);
        return ret;
    }

    return HI_SUCCESS;
}


hi_s32 drv_fe_ctrl_suspend(hi_void)
{
    hi_u32 i;
    drv_fe_ctrl_ctx  *fe_ctrl_ctx = NULL;
    drv_fe_tuner_ops *tuner_ops = NULL;
    drv_fe_demod_ops *demod_ops = NULL;

    for (i = 0; i < DRV_FRONTEND_NUM; i++) {
        fe_ctrl_ctx = drv_fe_ctrl_get_ctx(i);
        if (!fe_ctrl_ctx->attr_inited) {
            continue;
        }

        demod_ops = &fe_ctrl_ctx->demod_ops;
        tuner_ops = &fe_ctrl_ctx->tuner_ops;

        if (demod_ops->is_ctrl_tuner == NULL || !demod_ops->is_ctrl_tuner(i)) {
            if (tuner_ops->deinit != NULL) {
                tuner_ops->deinit(i);
            }
        }

        if (demod_ops->deinit != NULL) {
            demod_ops->deinit(i);
        }

        if ((fe_ctrl_ctx->attr.sig_type == HI_DRV_FRONTEND_SIG_TYPE_DVB_S) ||
            (fe_ctrl_ctx->attr.sig_type == HI_DRV_FRONTEND_SIG_TYPE_ISDB_S) ||
            (fe_ctrl_ctx->attr.sig_type == HI_DRV_FRONTEND_SIG_TYPE_ISDB_S3)) {
            if (fe_ctrl_ctx->lnb_ops.deinit != NULL) {
                fe_ctrl_ctx->lnb_ops.deinit(i);
            }
        }
    }

    HI_PRINT("frontend suspend ok\n");

    return HI_SUCCESS;
}

static hi_s32 drv_fe_ctrl_resume_init(hi_u32 port)
{
    hi_s32 ret;
    drv_fe_ctrl_ctx  *ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;
    drv_fe_tuner_ops *tuner_ops = NULL;

    ctrl_ctx = drv_fe_ctrl_get_ctx(port);
    demod_ops = &ctrl_ctx->demod_ops;

    /* the external demod needs to be reset */
    ret = drv_fe_ctrl_reset_demod(ctrl_ctx->attr.ext_dem_reset_gpio_group, ctrl_ctx->attr.ext_dem_reset_gpio_bit);
    if (ret != HI_SUCCESS) {
        HI_WARN_PRINT_FUNC_RES(demod_ops->init, ret);
    }

    if (demod_ops->init) {
        ret = demod_ops->init(port);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(demod_ops->init, ret);
            return ret;
        }
    }

    if (demod_ops->is_ctrl_tuner == NULL || !demod_ops->is_ctrl_tuner(port)) {
        tuner_ops = &ctrl_ctx->tuner_ops;
        if (tuner_ops->init != NULL) {
            tuner_ops->init(port);
        }
    }

    if (ctrl_ctx->resume_info.set_ts_out && demod_ops->set_ts_out != NULL) {
        demod_ops->set_ts_out(port, &ctrl_ctx->resume_info.ts_out);
    }

    if ((ctrl_ctx->attr.sig_type == HI_DRV_FRONTEND_SIG_TYPE_DVB_S) ||
        (ctrl_ctx->attr.sig_type == HI_DRV_FRONTEND_SIG_TYPE_ISDB_S) ||
        (ctrl_ctx->attr.sig_type == HI_DRV_FRONTEND_SIG_TYPE_ISDB_S3)) {
        if (demod_ops->set_sat_attr != NULL) {
            demod_ops->set_sat_attr(port, &ctrl_ctx->sat_attr);
        }
        if (ctrl_ctx->lnb_ops.init != NULL) {
            ctrl_ctx->lnb_ops.init(port, ctrl_ctx->sat_attr.lnb_i2c_channel, ctrl_ctx->sat_attr.lnb_dev_address);
        }
        if (ctrl_ctx->lnb_ops.set_lnb_out != NULL) {
            ctrl_ctx->lnb_ops.set_lnb_out(port,  ctrl_ctx->resume_info.lnb_out_level);
        }
        if (demod_ops->send_continuous22k != NULL) {
            demod_ops->send_continuous22k(port, ctrl_ctx->resume_info.continuous22k);
        }
    }
    return HI_SUCCESS;
}

static hi_void drv_fe_ctrl_resume_connect(hi_u32 port)
{
    drv_fe_ctrl_ctx  *ctrl_ctx = NULL;
    drv_fe_demod_ops *demod_ops = NULL;
    drv_fe_tuner_ops *tuner_ops = NULL;

    ctrl_ctx = drv_fe_ctrl_get_ctx(port);
    demod_ops = &ctrl_ctx->demod_ops;

    if (demod_ops->is_ctrl_tuner == NULL || !demod_ops->is_ctrl_tuner(port)) {
        tuner_ops = &ctrl_ctx->tuner_ops;
        if (tuner_ops->set_system != NULL) {
            tuner_ops->set_system(port, ctrl_ctx->cur_tuner_system);
        }
        if (tuner_ops->set_freq != NULL) {
            tuner_ops->set_freq(port, ctrl_ctx->connect_attr.frequency);
        }
    }

    if (demod_ops->connect != NULL) {
        demod_ops->connect(port, &ctrl_ctx->connect_attr);
    }
}

hi_s32 drv_fe_ctrl_resume(hi_void)
{
    hi_s32 ret;
    hi_u32 i;
    drv_fe_ctrl_ctx  *ctrl_ctx = NULL;

    for (i = 0; i < DRV_FRONTEND_NUM; i++) {
        ctrl_ctx = drv_fe_ctrl_get_ctx(i);
        if (!ctrl_ctx->attr_inited) {
            continue;
        }

        ret = drv_fe_ctrl_resume_init(i);
        if (ret != HI_SUCCESS) {
            HI_ERR_PRINT_FUNC_RES(drv_fe_ctrl_resume_init, ret);
            continue;
        }

        if (ctrl_ctx->resume_info.lock_status == HI_DRV_FRONTEND_LOCK_STATUS_LOCKED) {
            drv_fe_ctrl_resume_connect(i);
        }
    }

    HI_PRINT("frontend resume ok\n");

    return HI_SUCCESS;
}

