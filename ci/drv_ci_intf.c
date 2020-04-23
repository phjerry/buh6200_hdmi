/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: drivers of drv_osal_sys
 * Author: z00268517
 * Create: 2014-08-02
 */

#include "hi_debug.h"
#include "drv_ci_ioctl.h"
#include "drv_ci_ext.h"
#include "hal_ci.h"
#include "hi_osal.h"
#include "hi_drv_dev.h"
#include "linux/hisilicon/securec.h"

#undef  LOG_D_MODULE_ID
#define LOG_D_MODULE_ID HI_ID_CI
#define CI_SEM_INIT_VAL         1

typedef enum {
    CI_PROC_SETACCESSMODE = 0,
    CI_PROC_TSBYPASSCTRL,
    CI_PROC_IOPRINTCTRL,
    CI_PROC_MAX
} ci_proc_command;

osal_semaphore g_sem_ci;

static osal_dev g_ci_dev;

#define CI_OSAL_STRTOUL_VAL    10
#define BYTE_LEN_16            16
#define CI_PROC_WRITE_7        7
#define CI_PROC_WRITE_8        8
#define CI_OFFSET_1            1

static hi_s32 ci_port_lock(hi_void)
{
    if (g_sem_ci.sem == HI_NULL) {
        HI_LOG_ERR("g_sem_ci sem is null, ci module init failed.\n");
        return HI_ERR_CI_NOT_INIT;
    } else {
        hi_s32 ret;
        ret = osal_sem_down_interruptible(&g_sem_ci);
        if (ret != HI_SUCCESS) {
            hi_err_print_call_fun_err(osal_sem_down_interruptible, ret);
            return HI_ERR_CI_NOT_INIT;
        }
    }

    return HI_SUCCESS;
}

static void ci_port_unlock(hi_void)
{
    if (g_sem_ci.sem == HI_NULL) {
        return;
    }

    osal_sem_up(&g_sem_ci);
}

hi_s32 hi_ci_proc_read(hi_void *p, hi_void *v)
{
    hi_ci_pccd card;
    ci_pccd_debuginfo debug_info;
    hi_s32 ret;

    if (p == HI_NULL) {
        HI_LOG_ERR("CI proc read failed, p is null.\n");
        return HI_ERR_CI_INVALID_PARA;
    }

    ret = ci_port_lock();
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(ci_port_lock, ret);
        return ret;
    }

    for (card = HI_CI_PCCD_A; card < HI_CI_PCCD_B; card++) {
        osal_proc_print(p, "----hisilicon CI[%d] PCCD[%d] info----\n", HI_CI_PORT_0, card);

        debug_info.status = HI_CI_PCCD_STATUS_ABSENT;
        debug_info.ready = HI_CI_PCCD_BUSY;
        debug_info.by_pass = HI_TRUE;

        /* get status */
        ret = drv_ci_pccd_get_debug_info(HI_CI_PORT_0, card, &debug_info);

        switch (ret) {
            case HI_SUCCESS: {
                /* output detect status */
                switch (debug_info.status) {
                    case HI_CI_PCCD_STATUS_PRESENT:
                        osal_proc_print(p, "detect:\t\tpresent\n");
                        break;

                    case HI_CI_PCCD_STATUS_ABSENT:
                    default:
                        osal_proc_print(p, "detect:\t\tabsent\n");
                        break;
                }

                /* TS mode */
                switch (debug_info.by_pass) {
                    case HI_TRUE:
                        osal_proc_print(p, "TS mode:\tby_pass\n");
                        break;

                    case HI_FALSE:
                    default:
                        osal_proc_print(p, "TS mode:\tpass through\n");
                        break;
                }

                /* if card absent, break */
                if (debug_info.status == HI_CI_PCCD_STATUS_ABSENT) {
                    break;
                }

                /* ready or busy */
                switch (debug_info.ready) {
                    case HI_CI_PCCD_READY:
                        osal_proc_print(p, "card:\t\tready\n");
                        break;

                    case HI_CI_PCCD_BUSY:
                    default:
                        osal_proc_print(p, "card:\t\tbusy\n");
                        break;
                }

                osal_proc_print(p, "CI PLUS:\t%c\n", debug_info.is_ciplus ? 'Y' : 'N');
                osal_proc_print(p, "buffer size:\t%d\n", debug_info.buffer_size);
                osal_proc_print(p, "IO read cnt:\t%d\n", debug_info.io_cnt);
                osal_proc_print(p, "DEV TYPE:\thici\n");
                osal_proc_print(p, "power ctrl by gpio:\t%c\n",
                    debug_info.attr.dev_attr.hici.is_power_ctrl_gpio_used ? 'Y' : 'N');
                /* the GPIO to power on/off the device. */
                osal_proc_print(p, "power ctrl gpio:\t%d\n", debug_info.attr.dev_attr.hici.power_ctrl_gpio_no[card]);
            }

            case HI_ERR_CI_UNSUPPORT:
                osal_proc_print(p, "unsupport.\n");
                break;

            case HI_ERR_CI_NOT_INIT:
                osal_proc_print(p, "close.\n");
                break;

            default:
                osal_proc_print(p, "get status fail: 0x%08x.\n", ret);
                break;
        }
    }

    ci_port_unlock();

    return HI_SUCCESS;
}

/* print communication data */
hi_s32 hi_ci_proc_write(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], void *private)
{
    hi_char buffer[BYTE_LEN_16] = { 0 };
    hi_char *p = HI_NULL;
    hi_ci_port port;
    hi_ci_pccd card;
    ci_proc_command cmd;
    hi_u32 param;
    hi_s32 ret;
    hi_ci_pccd_tsctrl_param pccd_param;
    hi_u8 *buf = (hi_u8 *)argv;
    hi_u32 count;

    if ((buf == HI_NULL) || (private == HI_NULL)) {
        osal_printk("CI proc write failed, buf is null.\n");
        return HI_ERR_CI_INVALID_PARA;
    }

    count = *(hi_u32 *)private;

    if (((count != CI_PROC_WRITE_7) && (count != CI_PROC_WRITE_8)) || (count > BYTE_LEN_16)) {
        osal_printk("usage: echo P1 P2 P3 P4 > /proc/msp/ci\n");
        osal_printk("\tP1: CI port, 0 or 1\n");
        osal_printk("\tP2: PC card on CI port, 0 or 1\n");
        osal_printk("\tP3: command, 1 TS bypass control, 2 Print IO data\n");
        osal_printk("\tP4: value\n");
        osal_printk("\t\t_for TS bypass: 0 pass through, 1 bypass\n");
        osal_printk("example:\n");
        osal_printk("\techo 0 0 1 1  > /proc/msp/ci ---> set ts bypass\n");
        osal_printk("\techo 0 0 2 1  > /proc/msp/ci ---> Start print IO data\n");
        osal_printk("\techo 0 0 2 1  > /proc/msp/ci ---> stop print IO data\n");

        return HI_SUCCESS;
    }

    if (osal_copy_from_user(buffer, buf, count)) {
        osal_printk("copy from user failed\n");
        return HI_FAILURE;
    }

    p = buffer;
    port  = (hi_u32)osal_strtoul(p, &p, CI_OSAL_STRTOUL_VAL);
    card  = (hi_u32)osal_strtoul(p + CI_OFFSET_1, &p, CI_OSAL_STRTOUL_VAL);
    cmd   = (hi_u32)osal_strtoul(p + CI_OFFSET_1, &p, CI_OSAL_STRTOUL_VAL);
    param = (hi_u32)osal_strtoul(p + CI_OFFSET_1, &p, CI_OSAL_STRTOUL_VAL);

    ret = ci_port_lock();
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(ci_port_lock, ret);
        return ret;
    }

    switch (cmd) {
        case CI_PROC_TSBYPASSCTRL:
            pccd_param.bypass.ts_bypass = param;
            ret = drv_ci_pccd_ts_ctrl(port, card, HI_CI_PCCD_TSCTRL_BYPASS, &pccd_param);
            if (ret != HI_SUCCESS) {
                osal_printk("ts_ctrl fail: 0x%08x\n", ret);
            }
            break;

        case CI_PROC_IOPRINTCTRL:
            ret = drv_ci_pccd_dbg_io_print_ctrl(port, card, (hi_bool)param);
            if (ret != HI_SUCCESS) {
                osal_printk("IO print control fail: 0x%08x\n", ret);
            }
            break;

        default:
            break;
    }
    p = NULL;

    ci_port_unlock();

    return count;
}

static osal_proc_cmd g_ci_proc[] = {
    {"write", hi_ci_proc_write},
};

hi_s32 hi_ci_local_ioctl(unsigned int cmd, hi_void *arg, hi_void* private_data)
{
    hi_s32 ret;

    if (arg == HI_NULL) {
        HI_LOG_ERR("CI ioctl invalid point.\n");
        return HI_FAILURE;
    }

    ret = ci_port_lock();
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(ci_port_lock, ret);
        return ret;
    }

    switch (cmd) {
        case CMD_CI_OPEN:
            ret = drv_ci_open(*((hi_ci_port *)arg));
            break;

        case CMD_CI_CLOSE:
            ret = drv_ci_close(*((hi_ci_port *)arg));
            break;

        case CMD_CI_PCCD_OPEN: {
                ci_pccd *pccd = (ci_pccd *)arg;
                ret = drv_ci_pccd_open(pccd->port, pccd->card);
            }
            break;

        case CMD_CI_PCCD_CLOSE: {
                ci_pccd *pccd = (ci_pccd *)arg;
                drv_ci_pccd_close(pccd->port, pccd->card);
                ret = HI_SUCCESS;
            }
            break;

        case CMD_CI_PCCD_CTRLPOWER: {
                ci_pccd_ctrl_power *ci_ctrl_power = (ci_pccd_ctrl_power *)arg;
                ret = drv_ci_pccd_ctrl_power(ci_ctrl_power->port, ci_ctrl_power->card, ci_ctrl_power->ctrl_power);
            }
            break;

        case CMD_CI_PCCD_RESET: {
                ci_pccd *pccd = (ci_pccd *)arg;
                ret = drv_ci_pccd_reset(pccd->port, pccd->card);
            }
            break;

        case CMD_CI_PCCD_READY: {
                ci_pccd_ready *ci_ready = (ci_pccd_ready *)arg;
                ret = drv_ci_pccd_is_ready(ci_ready->port, ci_ready->card, &ci_ready->ready);
            }
            break;

        case CMD_CI_PCCD_DETECT: {
                ci_pccd_detect *ci_detect = (ci_pccd_detect *)arg;
                ret = drv_ci_pccd_detect(ci_detect->port, ci_detect->card, &ci_detect->status);
            }
            break;

        case CMD_CI_PCCD_GET_STATUS: {
                ci_pccd_status *ci_status = (ci_pccd_status *)arg;
                ret = drv_ci_pccd_get_status(ci_status->port, ci_status->card, &ci_status->status);
            }
            break;

        case CMD_CI_PCCD_READ: {
                ci_pccd_read *ci_read = (ci_pccd_read *)arg;
                ret = drv_ci_pccd_io_read(ci_read->port, ci_read->card, ci_read->buffer.p,
                    ci_read->buffer_len, &ci_read->read_len);
            }
            break;

        case CMD_CI_PCCD_WRITE: {
                ci_pccd_write *ci_write = (ci_pccd_write *)arg;
                ret = drv_ci_pccd_io_write(ci_write->port, ci_write->card, ci_write->buffer.p,
                                           ci_write->write_len, &ci_write->write_ok_len);
            }
            break;

        case CMD_CI_PCCD_CHECKCIS: {
                ci_pccd *pccd = (ci_pccd *)arg;
                ret = drv_ci_pccd_check_cis(pccd->port, pccd->card);
            }
            break;

        case CMD_CI_PCCD_WRITECOR: {
                ci_pccd *pccd = (ci_pccd *)arg;
                ret = drv_ci_pccd_write_cor(pccd->port, pccd->card);
            }
            break;

        case CMD_CI_PCCD_IORESET: {
                ci_pccd *pccd = (ci_pccd *)arg;
                ret = drv_ci_pccd_io_reset(pccd->port, pccd->card);
            }
            break;

        case CMD_CI_PCCD_NEG_BUFFERSIZE: {
                ci_pccd_buffer_size *ci_buffer_size = (ci_pccd_buffer_size *)arg;
                ret = drv_ci_pccd_neg_buffer_size(ci_buffer_size->port, ci_buffer_size->card,
                    &ci_buffer_size->buffer_size);
            }
            break;

        case CMD_CI_PCCD_TSCTRL: {
                ci_pccd_tsctrl *ci_ts_ctrl = (ci_pccd_tsctrl *)arg;
                ret = drv_ci_pccd_ts_ctrl(ci_ts_ctrl->port, ci_ts_ctrl->card, ci_ts_ctrl->cmd, &ci_ts_ctrl->param);
            }
            break;

        /* added begin 2012-04-24 : support various CI device */
        case CMD_CI_SETATTR: {
                ci_attr *attr = (ci_attr *)arg;
                ret = drv_ci_set_attr(attr->port, attr->attr);
            }
            break;

        case CMD_CI_GETATTR: {
                ci_attr *attr = (ci_attr *)arg;
                ret = drv_ci_get_attr(attr->port, &(attr->attr));
            }
            break;

        case CMD_CI_PCCD_GETCIS: {
                ci_pccd_cis *cis = (ci_pccd_cis *)arg;
                ret = drv_ci_pccd_get_cis(cis->port, cis->card, cis->buffer.p, cis->buffer_len, &cis->cis_len);
            }
            break;

        case CMD_CI_PCCD_WRITECOREX: {
                ci_pccd_cor *cor = (ci_pccd_cor *)arg;
                ret = drv_ci_pccd_write_cor_ex(cor->port, cor->card, cor->addr, cor->data);
            }
            break;

        /* added end 2012-04-24 : support various CI device */
        default:
            HI_LOG_ERR("unknow cmd!\n");
            hi_err_print_h32(_IOC_TYPE(cmd));
            hi_err_print_h32(_IOC_NR(cmd));
            hi_err_print_h32(_IOC_SIZE(cmd));
            ret = HI_FAILURE;
            break;
    }

    ci_port_unlock();

    return ret;
}

static osal_ioctl_cmd g_ci_fops[] = {
    {CMD_CI_OPEN,                   hi_ci_local_ioctl},
    {CMD_CI_CLOSE,                  hi_ci_local_ioctl},
    {CMD_CI_PCCD_OPEN,              hi_ci_local_ioctl},
    {CMD_CI_PCCD_CLOSE,             hi_ci_local_ioctl},
    {CMD_CI_PCCD_CTRLPOWER,         hi_ci_local_ioctl},
    {CMD_CI_PCCD_RESET,             hi_ci_local_ioctl},
    {CMD_CI_PCCD_READY,             hi_ci_local_ioctl},
    {CMD_CI_PCCD_DETECT,            hi_ci_local_ioctl},
    {CMD_CI_PCCD_GET_STATUS,        hi_ci_local_ioctl},
    {CMD_CI_PCCD_READ,              hi_ci_local_ioctl},
    {CMD_CI_PCCD_WRITE,             hi_ci_local_ioctl},
    {CMD_CI_PCCD_CHECKCIS,          hi_ci_local_ioctl},
    {CMD_CI_PCCD_WRITECOR,          hi_ci_local_ioctl},
    {CMD_CI_PCCD_IORESET,           hi_ci_local_ioctl},
    {CMD_CI_PCCD_NEG_BUFFERSIZE,    hi_ci_local_ioctl},
    {CMD_CI_PCCD_TSCTRL,            hi_ci_local_ioctl},
    {CMD_CI_SETATTR,                hi_ci_local_ioctl},
    {CMD_CI_GETATTR,                hi_ci_local_ioctl},
    {CMD_CI_PCCD_GETCIS,            hi_ci_local_ioctl},
    {CMD_CI_PCCD_WRITECOREX,        hi_ci_local_ioctl},
};

static hi_s32 hi_ci_pm_suspend(void *private_data)
{
    hi_s32 ret;

    ret = ci_port_lock();
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(ci_port_lock, ret);
        return ret;
    }

    ret = drv_ci_standby(HI_CI_PORT_0);
    if (ret != HI_SUCCESS) {
        HI_LOG_ERR("drv_ci_standby failed!\n");
        ci_port_unlock();
        return ret;
    }

    ci_port_unlock();

    HI_LOG_NOTICE("CI suspend OK\n");
    return HI_SUCCESS;
}

static hi_s32 hi_ci_pm_resume(void *private_data)
{
    hi_s32 ret;

    ret = ci_port_lock();
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(ci_port_lock, ret);
        return ret;
    }

    ret = drv_ci_resume(HI_CI_PORT_0);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(drv_ci_resume, ret);
        ci_port_unlock();
        return ret;
    }

    ci_port_unlock();

    HI_LOG_NOTICE("CI resume OK\n");
    return HI_SUCCESS;
}

static hi_s32 hi_ci_pm_lowpower_enter(void *private_data)
{
    hi_s32 ret;
    ret = hi_ci_pm_suspend(private_data);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(hi_ci_pm_lowpower_enter, ret);
        return ret;
    }
    HI_LOG_NOTICE("CI lowpower_enter OK\n");
    return HI_SUCCESS;
}

static hi_s32 hi_ci_pm_lowpower_exit(void *private_data)
{
    hi_s32 ret;
    ret = hi_ci_pm_resume(private_data);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(hi_ci_pm_lowpower_exit, ret);
        return ret;
    }
    HI_LOG_NOTICE("CI lowpower_exit OK\n");
    return HI_SUCCESS;
}

static osal_fileops g_dev_ci_fops = {
    .open = HI_NULL,
    .read = HI_NULL,
    .write = HI_NULL,
    .llseek = HI_NULL,
    .release = HI_NULL,
    .poll = HI_NULL,
    .mmap = HI_NULL,
    .cmd_list = g_ci_fops,
    .cmd_cnt  = sizeof(g_ci_fops) / sizeof(osal_ioctl_cmd),
};

static osal_pmops g_ci_drvops = {
    .pm_suspend = hi_ci_pm_suspend,
    .pm_resume = hi_ci_pm_resume,
    .pm_lowpower_enter = hi_ci_pm_lowpower_enter,
    .pm_lowpower_exit = hi_ci_pm_lowpower_exit,
    .pm_poweroff = HI_NULL,
    .private_data = HI_NULL,
};

static hi_s32 drv_ci_register(hi_void)
{
    hi_s32 ret;
    ret = snprintf_s(g_ci_dev.name, sizeof(g_ci_dev.name) - 1, strlen(HI_DEV_CI_NAME), "%s", HI_DEV_CI_NAME);
    if (ret < HI_SUCCESS) {
        hi_err_print_call_fun_err(snprintf_s, ret);
        return HI_ERR_CI_NOT_INIT;
    }

    g_ci_dev.fops = &g_dev_ci_fops;
    g_ci_dev.minor = HI_DEV_CI_MINOR;
    g_ci_dev.pmops = &g_ci_drvops;
    ret = osal_dev_register(&g_ci_dev);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(osal_dev_register, ret);
        return HI_ERR_CI_NOT_INIT;
    }

    ret = osal_exportfunc_register(HI_ID_CI, MODULE_CI, HI_NULL);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(osal_exportfunc_register, ret);
        (hi_void)osal_dev_unregister(&g_ci_dev);
        return HI_ERR_CIPHER_FAILED_INIT;
    }

    return HI_SUCCESS;
}

static hi_s32 drv_ci_unregister(hi_void)
{
    hi_s32 ret;

    ret = osal_exportfunc_unregister(HI_ID_CI);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(osal_exportfunc_register, ret);
        (hi_void)osal_dev_unregister(&g_ci_dev);
        return HI_ERR_CIPHER_FAILED_INIT;
    }

    osal_dev_unregister(&g_ci_dev);
    return HI_SUCCESS;
}

hi_s32 hi_ci_mod_init(hi_void)
{
    hi_s32 ret;

    osal_proc_entry *proc_item = HI_NULL;
    ret = drv_ci_register();
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(drv_ci_register, ret);
        return HI_ERR_CI_NOT_INIT;
    }
    /* register SCI proc function */
    proc_item = osal_proc_add(HI_MOD_CI_NAME, strlen(HI_MOD_CI_NAME));
    if (proc_item == HI_NULL) {
        hi_err_print_call_fun_err(osal_proc_add, HI_ERR_CI_NOT_INIT);
        (hi_void)drv_ci_unregister();
        return HI_ERR_CI_NOT_INIT;
    }

    proc_item->read = hi_ci_proc_read;
    proc_item->cmd_list = g_ci_proc;
    proc_item->cmd_cnt = sizeof(g_ci_proc) / sizeof(osal_proc_cmd);
    ret = osal_sem_init(&g_sem_ci, CI_SEM_INIT_VAL);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(osal_sem_init, ret);
        osal_proc_remove(HI_MOD_CI_NAME, strlen(HI_MOD_CI_NAME));
        (hi_void)drv_ci_unregister();
        return HI_ERR_CI_NOT_INIT;
    }
    ret = drv_ci_init();
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(drv_ci_init, ret);
        osal_sem_destory(&g_sem_ci);
        osal_proc_remove(HI_MOD_CI_NAME, strlen(HI_MOD_CI_NAME));
        (hi_void)drv_ci_unregister();
        return HI_ERR_CI_NOT_INIT;
    }

#ifdef MODULE
    HI_LOG_NOTICE("load hi_ci.ko success.  \t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;
}

hi_void hi_ci_mod_exit(hi_void)
{
    hi_s32 ret;

    drv_ci_de_init();
    osal_sem_destory(&g_sem_ci);
    osal_proc_remove(HI_MOD_CI_NAME, strlen(HI_MOD_CI_NAME));

    ret = drv_ci_unregister();
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(drv_ci_unregister, ret);
        return;
    }

#ifdef MODULE
    HI_LOG_NOTICE("unload hi_ci.ko success.  \t(%s)\n", VERSION_STRING);
#endif

    return;
}

#ifdef MODULE
module_init(hi_ci_mod_init);
module_exit(hi_ci_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");
