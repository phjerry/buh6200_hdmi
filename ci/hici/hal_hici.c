/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: drivers of hal_hici
 * Author: z00268517
 * Create: 2014-08-02
 */
/***************************** Include files  ******************************/
#include "hi_debug.h"
#include "drv_gpio_ext.h"
#include "hal_ci.h"
#include "hal_hici_define.h"
#include "hi_osal.h"
#include "linux/hisilicon/securec.h"

#undef  LOG_D_MODULE_ID
#define LOG_D_MODULE_ID HI_ID_CI

/************************** Macro Definition ******************************/
#ifndef CI_PEACH_TEST
#define CI_TIME_10MS 1              /* 10ms */
#else
#define CI_TIME_10MS 10             /* 10ms */
#endif

#define CI_PCCD_READY_COUNT             5
#define CI_PCCD_DETECT_MAX              5
#define CI_PCCD_DETECT_MIN              3
#define CI_PCCD_RESUME_ABSENT_COUNT     10

#define DAIE                            0x80
#define FRIE                            0x40
#define COM_STAT_REG 1

typedef enum {
    CMD_TYPE_IOW = 0,
    CMD_TYPE_IOR,
    CMD_TYPE_MW,
    CMD_TYPE_MR,
} cmd_type_list;

typedef enum {
    HI_CI_PCCD_RUN_STEP_RESUME,
    HI_CI_PCCD_RUN_STEP_STANDBY,
    HI_CI_PCCD_RUN_STEP_RUNNING,
    HI_CI_PCCD_RUN_STEP_CRAD_BUSY,
    HI_CI_PCCD_RUN_STEP_BUIT,
} hi_ci_pccd_run_step;

#define WRITE_REG(addr, value)        osal_writel((value), (addr))
#define READ_REG(addr)                osal_readl(addr)
#define CI_WRITE_REG(offset, value)   WRITE_REG(g_ci_reg_base + (offset), value)
#define CI_READ_REG(offset)           READ_REG(g_ci_reg_base + (offset))
#define CRG_WRITE_REG(offset, value)  WRITE_REG(g_crg_reg_base + (offset), value)
#define CRG_READ_REG(offset)          READ_REG(g_crg_reg_base + (offset))
#define PERI_WRITE_REG(offset, value) WRITE_REG(g_peri_reg_base + (offset), value)
#define PERI_READ_REG(offset)         READ_REG(g_peri_reg_base + (offset))

#define HI_CI_CHECK_PCCD_VALID(card) do {              \
        if ((card) >= HI_CI_PCCD_MAX) {               \
            HI_LOG_ERR("invalid card id:%d.\n", card); \
            return HI_ERR_CI_INVALID_PARA;                 \
        }                                                  \
    } while (0)

typedef struct {
    hi_bool is_power_ctrl_gpio_used;
    hi_u32 power_ctrl_gpio_no[HI_CI_PCCD_MAX];
    hi_bool ts_by_pass[HI_CI_PCCD_MAX];
    hi_ci_pccd_run_step run_step[HI_CI_PCCD_MAX];
    hi_ci_pccd_status status[HI_CI_PCCD_MAX];
    hi_u32 resume_absent_cnt[HI_CI_PCCD_MAX];
} hici_parameter;

static hi_void *g_ci_reg_base   = HI_NULL;
static hi_void *g_crg_reg_base  = HI_NULL;
static hi_void *g_peri_reg_base = HI_NULL;
static gpio_ext_func* g_gpio_func = HI_NULL;
static hici_parameter g_ci_param[HI_CI_PORT_MAX] = {{0}};

hi_s32 ci_wait_done(hi_u8 *data)
{
    hi_u32 time_out = 20000;
    cmd0_rdata rdata;

    *data = 0x00;
    rdata.u32 = CI_READ_REG(CMD0_RDATA);

    while (time_out > 0) {
        rdata.u32 = CI_READ_REG(CMD0_RDATA);
        if (rdata.bits.done == 1) {
            break;
        }
        time_out--;
    }

    if (time_out == 0) {
        HI_LOG_ERR("wait data read timeout.\n");
        return HI_ERR_CI_TIMEOUT;
    }

    *data = rdata.bits.rdata;

    return HI_SUCCESS;
}

hi_s32 ci_base_addr_remap(hi_void)
{
    g_ci_reg_base = (hi_void *)osal_ioremap_nocache(CI_PHY_BASE_ADDR, 0x1000);
    if (g_ci_reg_base == HI_NULL) {
        HI_LOG_ERR("CI reg base ioremap_nocache failed.\n");
        return HI_FAILURE;
    }

    g_crg_reg_base = (hi_void *)osal_ioremap_nocache(PERI_CRG_BASE, 0x1000);
    if (g_crg_reg_base == HI_NULL) {
        HI_LOG_ERR("CRG base ioremap_nocache failed.\n");
        osal_iounmap(g_ci_reg_base);
        return HI_FAILURE;
    }

    g_peri_reg_base = (hi_void *)osal_ioremap_nocache(PERI_CTRL_BASE, 0x1000);
    if (g_peri_reg_base == HI_NULL) {
        HI_LOG_ERR("CRG base ioremap_nocache failed.\n");
        osal_iounmap(g_ci_reg_base);
        osal_iounmap(g_crg_reg_base);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void ci_base_addr_unmap(hi_void)
{
    if (g_ci_reg_base != HI_NULL) {
        osal_iounmap(g_ci_reg_base);
        g_ci_reg_base = HI_NULL;
    }
    if (g_crg_reg_base != HI_NULL) {
        osal_iounmap(g_crg_reg_base);
        g_crg_reg_base = HI_NULL;
    }
    if (g_peri_reg_base != HI_NULL) {
        osal_iounmap(g_peri_reg_base);
        g_peri_reg_base = HI_NULL;
    }
}

hi_s32 ci_init(hi_void)
{
    hi_s32 ret;
    peri_crg98 crg98;
    ci_inf_set inf_set;
    cmd_cfg cfg;
    slave_mode mode;

    crg98.u32 = CRG_READ_REG(PERI_CRG98);

    /* open clcok */
    crg98.bits.ci_cken = 1;
    CRG_WRITE_REG(PERI_CRG98, crg98.u32);
    osal_msleep(1);

    /* assert reset */
    crg98.bits.ci_srst_req = 1;
    CRG_WRITE_REG(PERI_CRG98, crg98.u32);
    osal_msleep(1);

    /* remove reset */
    crg98.bits.ci_srst_req = 0;
    CRG_WRITE_REG(PERI_CRG98, crg98.u32);
    osal_msleep(1);

    /* enable pin output */
    inf_set.u32 = CI_READ_REG(CI_INF_SET);
    inf_set.bits.sw_com_oen       = 0;
    inf_set.bits.sw_card2_com_oen = 0;
    inf_set.bits.sw_rst_oen       = 0;
    inf_set.bits.sw_card2_rst_oen = 0;
    CI_WRITE_REG(CI_INF_SET, inf_set.u32);

    /* check ready single but don't check inpackn single */
    cfg.u32 = CI_READ_REG(CMD_CFG);
    cfg.bits.inpackn_check = 0;
    cfg.bits.rdy_check = 1;
    CI_WRITE_REG(CMD_CFG, cfg.u32);

    /* indirect mode */
    mode.u32 = CI_READ_REG(SLAVE_MODE);
    mode.bits.slave_mode = 0;
    CI_WRITE_REG(SLAVE_MODE, mode.u32);

    ret = hal_ci_pccd_ts_by_pass(HI_CI_PORT_0, HI_CI_PCCD_A, HI_TRUE);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(hal_ci_pccd_ts_by_pass, ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_s32 hal_ci_init(hi_void)
{
    hi_s32 ret;
    hi_u32 i;

    ret = ci_base_addr_remap();
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(ci_base_addr_remap, ret);
        return ret;
    }

    ret = ci_init();
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(ci_init, ret);
        return ret;
    }

    g_gpio_func = HI_NULL;
    ret = osal_exportfunc_get(HI_ID_GPIO, (hi_void **)&g_gpio_func);
    if (ret != HI_SUCCESS) {
        hi_err_print_call_fun_err(osal_exportfunc_get, ret);
        return ret;
    }

    CHECK_FUNC_RET(memset_s(&g_ci_param, sizeof(g_ci_param), 0x00, sizeof(g_ci_param)));

    for (i = 0; i < HI_CI_PCCD_MAX; i++) {
        g_ci_param[HI_CI_PORT_0].run_step[i] = HI_CI_PCCD_RUN_STEP_RUNNING;
        g_ci_param[HI_CI_PORT_0].ts_by_pass[i] = HI_TRUE;
        g_ci_param[HI_CI_PORT_0].status[i] = HI_CI_PCCD_STATUS_MAX;
        g_ci_param[HI_CI_PORT_0].is_power_ctrl_gpio_used = HI_FALSE;
    }

    return HI_SUCCESS;
}

hi_void hal_ci_deinit(hi_void)
{
    peri_crg98 crg98;

    crg98.u32 = CRG_READ_REG(PERI_CRG98);

    /* assert reset */
    crg98.bits.ci_srst_req = 1;
    CRG_WRITE_REG(PERI_CRG98, crg98.u32);
    osal_msleep(1);

    /* close clcok */
    crg98.bits.ci_cken = 0;
    CRG_WRITE_REG(PERI_CRG98, crg98.u32);
    osal_msleep(1);

    ci_base_addr_unmap();

    return;
}

hi_s32 hal_ci_device_open(hi_ci_port port)
{
    if (port != HI_CI_PORT_0) {
        HI_LOG_ERR("only support HI_CI_PORT_0 now.\n");
        return HI_ERR_CI_UNSUPPORT;
    }

    return HI_SUCCESS;
}

hi_s32 hal_ci_device_close(hi_ci_port port)
{
    if (port != HI_CI_PORT_0) {
        HI_LOG_ERR("only support HI_CI_PORT_0 now.\n");
        return HI_ERR_CI_UNSUPPORT;
    }

    return HI_SUCCESS;
}

hi_s32 hal_ci_set_attr(hi_ci_port port, hi_ci_attr attr)
{
    if (port != HI_CI_PORT_0) {
        HI_LOG_ERR("only support HI_CI_PORT_0 now.\n");
        return HI_ERR_CI_UNSUPPORT;
    }

    g_ci_param[port].is_power_ctrl_gpio_used = attr.dev_attr.hici.is_power_ctrl_gpio_used;
    g_ci_param[port].power_ctrl_gpio_no[HI_CI_PCCD_A] = attr.dev_attr.hici.power_ctrl_gpio_no[HI_CI_PCCD_A];
    g_ci_param[port].power_ctrl_gpio_no[HI_CI_PCCD_B] = attr.dev_attr.hici.power_ctrl_gpio_no[HI_CI_PCCD_B];

    return HI_SUCCESS;
}

hi_s32 hal_ci_pccd_open(hi_ci_port port, hi_ci_pccd card)
{
    if (port != HI_CI_PORT_0) {
        HI_LOG_ERR("only support HI_CI_PORT_0 now.\n");
        return HI_ERR_CI_UNSUPPORT;
    }

    return HI_SUCCESS;
}

hi_void hal_ci_pccd_close(hi_ci_port port, hi_ci_pccd card)
{
}

static hi_void hal_pccd_select(hi_ci_pccd card)
{
    ci_inf_set inf_set;

    inf_set.u32 = CI_READ_REG(CI_INF_SET);
    inf_set.bits.card2_sel = card;
    CI_WRITE_REG(CI_INF_SET, inf_set.u32);
}

hi_s32 ci_pccd_byte_rw(hi_ci_port port, hi_ci_pccd card, cmd_type_list type, hi_u32 address, hi_u8 *value)
{
    hi_s32 ret;
    cmd0_set cmd;
    hi_u8 out = 0;

    if (port != HI_CI_PORT_0) {
        HI_LOG_ERR("only support HI_CI_PORT_0 now.\n");
        return HI_ERR_CI_UNSUPPORT;
    }
    HI_CI_CHECK_PCCD_VALID(card);

    if (g_ci_param[port].run_step[card] == HI_CI_PCCD_RUN_STEP_RESUME) {
        HI_LOG_ERR("Resume from standby, will re-insert CAM soon.\n");
        return HI_FAILURE;
    }

    if (g_ci_param[port].run_step[card] == HI_CI_PCCD_RUN_STEP_CRAD_BUSY) {
        /* card is busy means that card may be dead,
         * to avoid keep printing error messages,
         * here just return 0 with HI_SUCCESS.
         */
        *value = 0x00;
        return HI_SUCCESS;
    }

    hal_pccd_select(card);
    cmd.u32 = CI_READ_REG(CMD0_SET);
    cmd.bits.cmd_type = type;
    cmd.bits.addr     = address;
    cmd.bits.wdata    = *value;
    CI_WRITE_REG(CMD0_SET, cmd.u32);

    /* wait done */
    ret = ci_wait_done(&out);
    if (ret != HI_SUCCESS) {
        g_ci_param[port].run_step[card] = HI_CI_PCCD_RUN_STEP_CRAD_BUSY;
        hi_err_print_call_fun_err(ci_wait_done, ret);
        return ret;
    }

    *value = out;

    return HI_SUCCESS;
}

hi_s32 hal_ci_pccd_io_read_byte(hi_ci_port port, hi_ci_pccd card, hi_u32 address, hi_u8 *value)
{
    return ci_pccd_byte_rw(port, card, CMD_TYPE_IOR, address, value);
}

hi_s32 hal_ci_pccd_io_write_byte(hi_ci_port port, hi_ci_pccd card, hi_u32 address, hi_u8 value)
{
    if (address == COM_STAT_REG) {
        value |= (DAIE | FRIE);
    }
    return ci_pccd_byte_rw(port, card, CMD_TYPE_IOW, address, &value);
}

hi_s32 hal_ci_pccd_mem_read_byte(hi_ci_port port, hi_ci_pccd card, hi_u32 address, hi_u8 *value)
{
    return ci_pccd_byte_rw(port, card, CMD_TYPE_MR, address, value);
}

hi_s32 hal_ci_pccd_mem_write_byte(hi_ci_port port, hi_ci_pccd card, hi_u32 address, hi_u8 value)
{
    return ci_pccd_byte_rw(port, card, CMD_TYPE_MW, address, &value);
}

hi_u32 ci_pccd0_detect_once(hi_u32 cd2_dis)
{
    dbg_in_sig in_sig;

    in_sig.u32 = CI_READ_REG(DBG_IN_SIG);

    /* check cd1, 0: card present */
    if (in_sig.bits.cd1 == 0x01) {
        /* card0 absent */
        return 0;
    }

    if (cd2_dis == 1) {
        /* disregard cd2, direct return present when cd1 = 0 */
        return 1;
    }

    if (in_sig.bits.cd2 == 0x00) {
        /* card0 present */
        return 1;
    }

    /* card0 absent */
    return 0;
}

hi_u32 ci_pccd1_detect_once(hi_u32 cd2_dis)
{
    ci_inf_set inf_set;

    /* check cd1, 0: card present */
    inf_set.u32 = CI_READ_REG(CI_INF_SET);
    if (inf_set.bits.card2_cd1_n == 0x01) {
        /* card1 absent */
        return 0;
    }

    if (cd2_dis == 1) {
        /* disregard cd2, direct return present when cd1 = 0 */
        return 1;
    }

    if (inf_set.bits.card2_cd2_n == 0x00) {
        /* card1 present */
        return 1;
    }

    /* card1 absent */
    return 0;
}

hi_s32 hal_ci_pccd_detect(hi_ci_port port, hi_ci_pccd card, hi_ci_pccd_status_ptr status)
{
    hi_u32 try_count;
    hi_u32 present_count = 0;
    ci_inf_cmd_mode cmd_mode;

    if (port != HI_CI_PORT_0) {
        HI_LOG_ERR("only support HI_CI_PORT_0 now.\n");
        return HI_ERR_CI_UNSUPPORT;
    }
    HI_CI_CHECK_PCCD_VALID(card);

    if (g_ci_param[port].run_step[card] == HI_CI_PCCD_RUN_STEP_RESUME) {
        /* assert card absent after resume to force a higher level CAM initialization */
        *status = HI_CI_PCCD_STATUS_ABSENT;

        if (g_ci_param[port].resume_absent_cnt[card] == 0) {
            g_ci_param[port].run_step[card] = HI_CI_PCCD_RUN_STEP_RUNNING;
        } else {
            g_ci_param[port].resume_absent_cnt[card] -= 1;
        }
        return HI_SUCCESS;
    }

    cmd_mode.u32 = CI_READ_REG(CI_INF_CMD_MODE);
    for (try_count = 0; try_count < CI_PCCD_DETECT_MAX; try_count++) {
        if (card == HI_CI_PCCD_A) {
            present_count += ci_pccd0_detect_once(cmd_mode.bits.cd2_dis);
        } else {
            present_count += ci_pccd1_detect_once(cmd_mode.bits.cd2_dis);
        }
        osal_msleep(CI_TIME_10MS);
    }

    if (present_count >= CI_PCCD_DETECT_MIN) {
        *status = HI_CI_PCCD_STATUS_PRESENT;
    } else {
        *status = HI_CI_PCCD_STATUS_ABSENT;
    }

    return HI_SUCCESS;
}

hi_s32 hal_ci_pccd_ready_or_busy(hi_ci_port port, hi_ci_pccd card, hi_ci_pccd_ready_ptr ready)
{
    hi_u32 elapsed_time;
    dbg_in_sig in_sig;

    if (port != HI_CI_PORT_0) {
        HI_LOG_ERR("only support HI_CI_PORT_0 now.\n");
        return HI_ERR_CI_UNSUPPORT;
    }
    HI_CI_CHECK_PCCD_VALID(card);

    *ready = HI_CI_PCCD_BUSY;

    /* the host shall explicitly check for the READY signal until it is set by the
     * module or until a timeout of 5s has expired.
     */
    for (elapsed_time = 0; elapsed_time < CI_PCCD_READY_COUNT; elapsed_time++) {
        in_sig.u32 = CI_READ_REG(DBG_IN_SIG);

        if (card == HI_CI_PCCD_A) {
            if (in_sig.bits.rdy == 1) {
                *ready = HI_CI_PCCD_READY;
                break;
            }
        } else {
            if (in_sig.bits.card2_ready == 1) {
                *ready = HI_CI_PCCD_READY;
                break;
            }
        }
        osal_msleep(CI_TIME_10MS);
    }

    return HI_SUCCESS;
}

hi_s32 hal_ci_pccd_reset(hi_ci_port port, hi_ci_pccd card)
{
    ci_inf_set inf_set;

    if (port != HI_CI_PORT_0) {
        HI_LOG_ERR("only support HI_CI_PORT_0 now.\n");
        return HI_ERR_CI_UNSUPPORT;
    }
    HI_CI_CHECK_PCCD_VALID(card);

    inf_set.u32 = CI_READ_REG(CI_INF_SET);

    if (card == HI_CI_PCCD_A) {
        inf_set.bits.set_ci_reset = 1;
        CI_WRITE_REG(CI_INF_SET, inf_set.u32);
        osal_msleep(CI_TIME_10MS);

        inf_set.bits.set_ci_reset = 0;
        CI_WRITE_REG(CI_INF_SET, inf_set.u32);
        osal_msleep(CI_TIME_10MS);
    } else {
        inf_set.bits.set_card2_reset = 1;
        CI_WRITE_REG(CI_INF_SET, inf_set.u32);
        osal_msleep(CI_TIME_10MS);

        inf_set.bits.set_card2_reset = 0;
        CI_WRITE_REG(CI_INF_SET, inf_set.u32);
        osal_msleep(CI_TIME_10MS);
    }

    /* enter running state after reset */
    g_ci_param[HI_CI_PORT_0].run_step[card] = HI_CI_PCCD_RUN_STEP_RUNNING;

    HI_LOG_INFO("reset CAM OK!\n");

    return HI_SUCCESS;
}

/*
 * power ON/OFF
 * notice: Current solution only support power control on CI port, but not for each PCCD.
 * so, if you call power off but some cards are present, will return HI_ERR_CI_CANNOT_POWEROFF.
 */
hi_s32 hal_ci_pccd_ctrl_power(hi_ci_port port, hi_ci_pccd card, hi_ci_pccd_ctrl_power ctrl_power)
{
    hi_s32 ret;
    hi_u32 gpio_no;
    hi_u32 gpio_val;

    if (port != HI_CI_PORT_0) {
        HI_LOG_ERR("only support HI_CI_PORT_0 now.\n");
        return HI_ERR_CI_UNSUPPORT;
    }

    HI_CI_CHECK_PCCD_VALID(card);

    if (g_ci_param[port].is_power_ctrl_gpio_used == HI_FALSE) {
        return HI_SUCCESS;
    }

    if ((g_gpio_func == HI_NULL) || (g_gpio_func->pfn_gpio_direction_set_bit == HI_NULL)
        || (g_gpio_func->pfn_gpio_write_bit == HI_NULL)) {
        HI_LOG_ERR("gpio func invalid.\n");
        return HI_FAILURE;
    }

    gpio_no = g_ci_param[port].power_ctrl_gpio_no[card];
    gpio_val = (ctrl_power == HI_CI_PCCD_CTRLPOWER_ON ? 1 : 0);

    ret = g_gpio_func->pfn_gpio_direction_set_bit(gpio_no, 0);
    osal_msleep(1);
    ret |= g_gpio_func->pfn_gpio_write_bit(gpio_no, gpio_val);
    if (ret != HI_SUCCESS) {
        HI_LOG_ERR("power off device fail, GPIO.\n");
        hi_err_print_h32(gpio_no);
        return ret;
    }

    osal_msleep(CI_TIME_10MS);
    HI_LOG_INFO("power card %d, gpio_no 0x%02x, gpio_val %d\n", card, gpio_no, gpio_val);

    return HI_SUCCESS;
}

hi_s32 hal_ci_pccd_ts_by_pass(hi_ci_port port, hi_ci_pccd card, hi_bool by_pass)
{
    peri_io_oen io_oen;

    if (port != HI_CI_PORT_0) {
        HI_LOG_ERR("only support HI_CI_PORT_0 now.\n");
        return HI_ERR_CI_UNSUPPORT;
    }
    HI_CI_CHECK_PCCD_VALID(card);

    io_oen.u32 = PERI_READ_REG(PERI_IO_OEN);

    if (card == HI_CI_PCCD_A) {
        io_oen.bits.peri_tso_loop_sel = (by_pass == HI_TRUE) ? 1 : 0;
        PERI_WRITE_REG(PERI_IO_OEN, io_oen.u32);
    } else if (card == HI_CI_PCCD_B) {
        /*
         * tuner->demod->card_b->TSI1->demux->decode
         * card B direct link to demod chip, so can't bypass the TS.
         */
        HI_LOG_ERR("card_id == HI_CI_PCCD_B\n");
    } else {
        HI_LOG_ERR("only support card A and card B.\n");
        return HI_ERR_CI_UNSUPPORT;
    }

    g_ci_param[port].ts_by_pass[card] = by_pass;

    return HI_SUCCESS;
}

hi_s32 hal_ci_pccd_get_bypass_mode(hi_ci_port port, hi_ci_pccd card, hi_bool *bypass)
{
    peri_io_oen io_oen;

    if (card == HI_CI_PCCD_A) {
        io_oen.u32 = PERI_READ_REG(PERI_IO_OEN);
        *bypass = (io_oen.bits.peri_tso_loop_sel == 1) ? HI_TRUE : HI_FALSE;
    } else {
        *bypass = HI_FALSE;
    }

    return HI_SUCCESS;
}

/* low power */
hi_s32 hal_ci_standby(hi_ci_port port)
{
    hi_s32 ret;
    hi_ci_pccd card = 0;

    if (port != HI_CI_PORT_0) {
        return HI_SUCCESS;
    }

    for (card = HI_CI_PCCD_A; card < HI_CI_PCCD_MAX; card++) {
        g_ci_param[port].run_step[card] = HI_CI_PCCD_RUN_STEP_STANDBY;
        ret = hal_ci_pccd_ctrl_power(port, card, HI_CI_PCCD_CTRLPOWER_OFF);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    }

    return HI_SUCCESS;
}

/* resume CI */
hi_s32 hal_ci_resume(hi_ci_port port)
{
    hi_s32 ret;
    hi_ci_pccd card = 0;

    if (port != HI_CI_PORT_0) {
        return HI_SUCCESS;
    }

    ret = ci_init();
    if (ret != HI_SUCCESS) {
        return ret;
    }

    for (card = HI_CI_PCCD_A; card < HI_CI_PCCD_MAX; card++) {
        /* during standby CAM may be always power on,
         * but host CI is power down,
         * so the CAM maybe died as not receive any respond from host
         * we need to reset CAM after resume.
         */
        ret = hal_ci_pccd_reset(port, card);
        if (ret != HI_SUCCESS) {
            return ret;
        }
        g_ci_param[port].run_step[card] = HI_CI_PCCD_RUN_STEP_RESUME;
        g_ci_param[port].resume_absent_cnt[card] = CI_PCCD_RESUME_ABSENT_COUNT;
    }

    return HI_SUCCESS;
}

