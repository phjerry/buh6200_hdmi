/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: tsio hw hal function impl.
* Author: guoqingbo 00277009
* Create: 2016-09-07
*/

#include "linux/io.h"
#include "hi_reg_common.h"
#include "hi_drv_sys.h"

#include "hal_tsio.h"
#include "drv_tsio_reg.h"
#include "drv_tsio_utils.h"

#define TSIO_BITS_PER_REG 32
#define NUM_SIZE 32

static spinlock_t g_tsio_hal_lock = __SPIN_LOCK_UNLOCKED(g_tsio_hal_lock);

static inline hi_void en_pcr_proof(const struct tsio_mgmt *mgmt)
{
    U_TIMER_CTRL reg1;

    reg1.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_TIMER_CTL);
    reg1.bits.timer_en = 1;
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_TIMER_CTL, reg1.u32);
}

hi_s32 tsio_hal_init_ctrl(const struct tsio_mgmt *mgmt)
{
    volatile hi_reg_peri *reg_peri = hi_drv_sys_get_peri_reg_ptr();
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
#ifdef TSIO_PHY_LOOPBACK_SUPPORT
    U_DBG_SC_CTS reg1;
    U_DBG_SC_GEN_OPEN reg2;
#endif

    /* some chips not include tsio hw, so check it first. */
    if (unlikely(reg_peri->PERI_SOC_FUSE_2 & 0x100000)) {
        HI_ERR_TSIO("TSIO not enabled for this chip.\n");
        return HI_ERR_TSIO_NOT_SUPPORT;
    }

    /* do reset tsio ctrl */
    reg_crg->PERI_CRG338.bits.tsio_srst_req = 1;
    reg_crg->PERI_CRG338.bits.tsio_cken     = 1;

    osal_mb();

    reg_crg->PERI_CRG338.bits.tsio_srst_req = 0;

    /* wait ctrl crg reset finished. */
    osal_udelay(100); /* delay 100us */

    en_pcr_proof(mgmt);

#ifdef TSIO_PHY_LOOPBACK_SUPPORT
    reg1.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DBG_SC_CTS);
    reg1.bits.dbg_sc_cts_en = 1;
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DBG_SC_CTS, reg1.u32);

    reg2.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DBG_SC_GEN_OPEN);
    reg2.bits.dbg_sc_gen_open = 1;
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DBG_SC_GEN_OPEN, reg2.u32);
#endif

    return HI_SUCCESS;
}

hi_void tsio_hal_de_init_ctrl(const struct tsio_mgmt *mgmt)
{
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
    U_PERI_CRG468 peri_crg468;
    unsigned long start, end;

    reg_crg->PERI_CRG338.bits.tsio_srst_req = 1;

    osal_mb();

    start = jiffies;
    end = start + HZ; /* 1s */
    do {
        peri_crg468 = reg_crg->PERI_CRG468;
    } while (peri_crg468.bits.tsio_srst_ok != 1 && time_in_range(jiffies, start, end));

    if (peri_crg468.bits.tsio_srst_ok != 1) {
        HI_ERR_TSIO("tsio ctrl reset failed.\n");
        goto out;
    }

    reg_crg->PERI_CRG338.bits.tsio_cken = 0;

    osal_mb();

out:
    return;
}

#ifndef TSIO_PHY_LOOPBACK_SUPPORT
static hi_void setup_phy_configure(const struct tsio_mgmt *mgmt, hi_u32 offset,
    hi_u32 swing, hi_u32 pre_emphasis, hi_u32 slew, hi_u32 skew)
{
    U_PHY_OFFSET_CTL reg1;
    U_PHY_SWING_CTL reg2;
    U_PHY_PRE_EMPHASIS reg3;
    U_PHY_SLEW_CTL reg4;
    U_PHY_CLK_DATA_SKEW reg5;

    /* offset */
    reg1.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_OFFSET_CTL);

    reg1.bits.offset_ctl = offset;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PHY_OFFSET_CTL, reg1.u32);

    /* swing */
    reg2.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_SWING_CTL);

    reg2.bits.swing_ctl = swing;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PHY_SWING_CTL, reg2.u32);

    /* pre emphasis */
    reg3.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_PRE_EMPHASIS);

    reg3.bits.pre_emphasis = pre_emphasis;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PHY_PRE_EMPHASIS, reg3.u32);

    /* slew */
    reg4.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_SLEW_CTL);

    reg4.bits.slew_ctl = slew;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PHY_SLEW_CTL, reg4.u32);

    /* skew */
    reg5.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_CLK_DATA_SKEW);

    reg5.bits.skew_ctl = skew;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PHY_CLK_DATA_SKEW, reg5.u32);
}
#endif

hi_void tsio_hal_init_phy(const struct tsio_mgmt *mgmt)
{
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
    U_PHY_CTRL reg1;
    U_PHY_INIT_REG reg2;
#ifdef TSIO_PHY_LOOPBACK_SUPPORT
    U_PHY_BIST_REG reg3;
#else
    U_PHY_SYNC_LIMIT reg4;
    U_PHY_RESYNC_CTRL resync_ctrl;
    U_PHY_SYNC_LIMIT sync_limit;
#endif
    unsigned long start, end;

    /* phy crg reset. */
    reg_crg->PERI_CRG338.bits.tsio_phy_srst_req = 1;
    reg_crg->PERI_CRG338.bits.tsio_phy_cken     = 1;

    if (mgmt->band_width == TSIO_BW_400M) {
        reg_crg->PERI_CRG338.bits.tsio_phy_clk_sel = 3; /* 3 means 400M bandwidth */
    } else if (mgmt->band_width == TSIO_BW_200M) {
        reg_crg->PERI_CRG338.bits.tsio_phy_clk_sel = 2; /* 2 means 200M bandwidth */
    } else if (mgmt->band_width == TSIO_BW_100M) {
        reg_crg->PERI_CRG338.bits.tsio_phy_clk_sel = 1; /* 1 means 100M bandwidth */
    } else if (mgmt->band_width == TSIO_BW_50M) {
        reg_crg->PERI_CRG338.bits.tsio_phy_clk_sel = 0; /* 0 means 100M bandwidth */
    } else {
        HI_FATAL_TSIO("mgmt band_width mismatch, mgmt band_width is:%u!\n", mgmt->band_width);
        goto out;
    }

    osal_mb();

    reg_crg->PERI_CRG338.bits.tsio_phy_srst_req = 0;

    /* wait phy crg reset finished. */
    osal_udelay(100); /* delay 100us */

    /* phy self reset. */
    reg1.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_CTRL);

    reg1.bits.phy_rst_n = 0;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PHY_CTRL, reg1.u32);

    reg1.bits.phy_rst_n = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PHY_CTRL, reg1.u32);

    /* wait phy reset finished */
    start = jiffies;
    end = start + HZ; /* 1s */
    do {
        reg1.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_CTRL);

        osal_udelay(10); /* delay 10us */
    } while (reg1.bits.phy_ready == 0 && time_in_range(jiffies, start, end));

    if (reg1.bits.phy_ready == 0) {
        HI_ERR_TSIO("tsio phy reset failed.\n");
        goto out;
    }

#ifdef TSIO_PHY_LOOPBACK_SUPPORT
    /* do loop back */
    reg3.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_BIST_REG);

    reg3.bits.internal_loopback = 1;
    reg3.bits.pattern_sel       = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PHY_BIST_REG, reg3.u32);

    /* wait sync finished */
    start = jiffies;
    end = start + HZ; /* 1s */
    do {
        reg2.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_INIT_REG);

        osal_udelay(10); /* delay 10us */
    } while (reg2.bits.sync_ready == 0 && time_in_range(jiffies, start, end));

    if (reg2.bits.sync_ready == 0) {
        HI_ERR_TSIO("tsio phy sync failed.\n");
        goto out;
    }
#else
    reg4.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_SYNC_LIMIT);

    reg4.bits.sync_cnt = mgmt->sync_thres;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PHY_SYNC_LIMIT, reg4.u32);

    /* setup phy configuration */
    if (mgmt->band_width == TSIO_BW_400M) {
        setup_phy_configure(mgmt, TSIO_PHY_100MHZ_OFFSET, TSIO_PHY_100MHZ_SWING, TSIO_PHY_100MHZ_PRE_EMPHASIS,
                            TSIO_PHY_100MHZ_SLEW, TSIO_PHY_100MHZ_SKEW);
    } else if (mgmt->band_width == TSIO_BW_200M) {
        setup_phy_configure(mgmt, TSIO_PHY_50MHZ_OFFSET, TSIO_PHY_50MHZ_SWING, TSIO_PHY_50MHZ_PRE_EMPHASIS,
                            TSIO_PHY_50MHZ_SLEW, TSIO_PHY_50MHZ_SKEW);
    } else if (mgmt->band_width == TSIO_BW_100M) {
        setup_phy_configure(mgmt, TSIO_PHY_25MHZ_OFFSET, TSIO_PHY_25MHZ_SWING, TSIO_PHY_25MHZ_PRE_EMPHASIS,
                            TSIO_PHY_25MHZ_SLEW, TSIO_PHY_25MHZ_SKEW);
    } else if (mgmt->band_width == TSIO_BW_50M) {
        setup_phy_configure(mgmt, TSIO_PHY_12MHZ_OFFSET, TSIO_PHY_12MHZ_SWING, TSIO_PHY_12MHZ_PRE_EMPHASIS,
                            TSIO_PHY_12MHZ_SLEW, TSIO_PHY_12MHZ_SKEW);
    }
    /* start phy */
    reg1.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_CTRL);

    reg1.bits.init_start = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PHY_CTRL, reg1.u32);

    /* wait phy start finished */
    start = jiffies;
    end = start + HZ; /* 1s */
    do {
        reg2.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_INIT_REG);

        osal_udelay(10); /* delay 10us */
    } while (reg2.bits.training_finish == 0 && time_in_range(jiffies, start, end));

    if (reg2.bits.training_pattern_received == 0) {
        HI_ERR_TSIO("tsio phy training failed.\n");
        goto out;
    }

    /* wait sync finished */
    start = jiffies;
    end = start + HZ; /* 1s */
    do {
        reg2.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_INIT_REG);

        osal_udelay(10); /* delay 10us */
    } while (reg2.bits.sync_finish == 0 && time_in_range(jiffies, start, end));

    if (reg2.bits.sync_ready == 0) {
        HI_ERR_TSIO("tsio phy sync failed.\n");
        goto out;
    }

    /* enable resync */
    resync_ctrl.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_RESYNC_CTRL);
    resync_ctrl.bits.resync_en = 1;
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PHY_RESYNC_CTRL, resync_ctrl.u32);

    /* set sync limit */
    sync_limit.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_SYNC_LIMIT);
    sync_limit.bits.sync_cnt = 8; /* Number of syncs is 8 */
    sync_limit.bits.sync_time = 415; /* Sync time is 415 */
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PHY_SYNC_LIMIT, sync_limit.u32);
#endif

    /* check phy final status */
    reg1.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_CTRL);

    if (reg1.bits.init_fail_status) {
        HI_ERR_TSIO("tsio phy start failed.\n");
        goto out;
    }

out:
    return;
}

hi_void tsio_hal_de_init_phy(const struct tsio_mgmt *mgmt)
{
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();

    reg_crg->PERI_CRG338.bits.tsio_phy_srst_req = 1;
    reg_crg->PERI_CRG338.bits.tsio_phy_cken     = 0;

    osal_mb();
}

hi_void tsio_hal_en_pid_channel(const struct tsio_mgmt *mgmt, const struct tsio_en_pidch_para *para)
{
    U_PID_TABLE reg;
    hi_u32 id = para->id;
    hi_u32 pid = para->pid;
    hi_u32 port_id = para->port_id;
    hi_u32 SID = para->sid;
    hi_bool is_live_ts = para->is_live_ts;

    TSIO_FATAL_ERROR(id >= mgmt->pid_channel_cnt);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PID_TABLE(TO_PIDCHN_HW_ID(id)));

    reg.bits.service_id    = SID;
    if (is_live_ts) {
        reg.bits.tsid      = TO_TSIPORT_HW_ID(port_id);
        reg.bits.tsid_type = 0;
    } else {
        reg.bits.tsid      = TO_RAMPORT_HW_ID(port_id);
        reg.bits.tsid_type = 1;
    }
    reg.bits.pid           = pid;
    reg.bits.pid_table_en  = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PID_TABLE(TO_PIDCHN_HW_ID(id)), reg.u32);
}

hi_void tsio_hal_dis_pid_channel(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    U_PID_TABLE reg;

    TSIO_FATAL_ERROR(id >= mgmt->pid_channel_cnt);

    reg.u32 = 0;

    reg.bits.pid_table_en  = 0;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PID_TABLE(TO_PIDCHN_HW_ID(id)), reg.u32);
}

hi_void tsio_hal_en_tsi_port(const struct tsio_mgmt *mgmt, hi_u32 id,
    hi_u32 dvb_port_id, tsio_live_port_type port_type)
{
    hi_u8 mask = 0;

    TSIO_FATAL_ERROR(id >= mgmt->tsi_port_cnt);

    if (port_type == TSIO_LIVE_PORT_IF) {
        mask = 0xc0;
    } else if (port_type == TSIO_LIVE_PORT_TSI) {
        mask = 0x20;
    }

    if (id < 4) { /* tsio port is 4, [0...3] */
        U_TSIO2DMX_SW_TS_SEL0 reg;

        reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_TSIO2DMX_SW_TS_SEL0);

        switch (id) {
            case 0: /* 0 mens first port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_0 = (dvb_port_id | mask);
                break;
            case 1: /* 1 mens second port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_1 = (dvb_port_id | mask);
                break;
            case 2: /* 2 mens third port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_2 = (dvb_port_id | mask);
                break;
            case 3: /* 3 mens fourth port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_3 = (dvb_port_id | mask);
                break;
            default:
                HI_FATAL_TSIO("id is mismatch, id is: %u!\n", id);
                return;
        }

        TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_TSIO2DMX_SW_TS_SEL0, reg.u32);
    } else { /* [4...7] */
        U_TSIO2DMX_SW_TS_SEL1 reg;

        reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_TSIO2DMX_SW_TS_SEL1);

        switch (id) {
            case 4: /* 4 mens fifth port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_4 = (dvb_port_id | mask);
                break;
            case 5: /* 5 mens sixth port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_5 = (dvb_port_id | mask);
                break;
            case 6: /* 6 mens seventh port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_6 = (dvb_port_id | mask);
                break;
            case 7: /* 7 mens eighth port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_7 = (dvb_port_id | mask);
                break;
            default:
                HI_FATAL_TSIO("id is mismatch, id is: %u!\n", id);
                return;
        }

        TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_TSIO2DMX_SW_TS_SEL1, reg.u32);
    }
}

hi_void tsio_hal_dis_tsi_port(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    TSIO_FATAL_ERROR(id >= mgmt->tsi_port_cnt);

    if (id < 4) { /* tsio port is 4, [0...3] */
        U_TSIO2DMX_SW_TS_SEL0 reg;

        reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_TSIO2DMX_SW_TS_SEL0);

        switch (id) {
            case 0: /* 0 mens first port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_0 = 0;
                break;
            case 1: /* 1 mens second port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_1 = 0;
                break;
            case 2: /* 2 mens third port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_2 = 0;
                break;
            case 3: /* 3 mens fourth port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_3 = 0;
                break;
            default:
                HI_FATAL_TSIO("id is mismatch, id is: %u!\n", id);
                return;
        }

        TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_TSIO2DMX_SW_TS_SEL0, reg.u32);
    } else { /* [4...7] */
        U_TSIO2DMX_SW_TS_SEL1 reg;

        reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_TSIO2DMX_SW_TS_SEL1);

        switch (id) {
            case 4: /* 4 mens fifth port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_4 = 0;
                break;
            case 5: /* 5 mens sixth port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_5 = 0;
                break;
            case 6: /* 6 mens seventh port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_6 = 0;
                break;
            case 7: /* 7 mens eighth port id */
                reg.bits.tsio2dmx_sw_ts_sel_cfg_7 = 0;
                break;
            default:
                HI_FATAL_TSIO("id is mismatch, id is: %u!\n", id);
                return;
        }

        TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_TSIO2DMX_SW_TS_SEL1, reg.u32);
    }
}

hi_void tsio_hal_send_ccout(const struct tsio_mgmt *mgmt, hi_u32 cclen)
{
    U_CC_LEN reg;

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_CC_LEN);

    reg.bits.cc_send_rdy    = 1;
    reg.bits.cc_send_length = cclen;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_CC_LEN, reg.u32);
}

hi_void tsio_hal_set_ccslot(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 ccdata)
{
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_CC_DATA(id), ccdata);
}

hi_void tsio_hal_get_ccresp_len(const struct tsio_mgmt *mgmt, hi_u32 *ccresp_len)
{
    U_CC_RAM_LEN reg;

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_CC_RAM_LEN);

    *ccresp_len = reg.bits.cc_rsv_length;
}

hi_void tsio_hal_get_ccslot(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 *ccdata)
{
    *ccdata = TSIO_READ_REG(mgmt->io_base, TSIO_REG_CC_RAM_DATA(id));
}

hi_void tsio_hal_recv_ccdone(const struct tsio_mgmt *mgmt)
{
    U_CC_RAM_RDONE reg;

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_CC_RAM_RDONE);

    reg.bits.cc_ram_rdone = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_CC_RAM_RDONE, reg.u32);
}

hi_u32 tsio_hal_get_cconflict_status(const struct tsio_mgmt *mgmt)
{
    return TSIO_READ_REG(mgmt->io_base, TSIO_REG_CC_REV_HOLD_CONFLICT);
}

hi_void tsio_hal_clr_cconflict_status(const struct tsio_mgmt *mgmt)
{
    U_CC_REV_HOLD_CONFLICT reg;

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_CC_REV_HOLD_CONFLICT);

    reg.bits.cc_rev_hold_conflict = 0;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_CC_REV_HOLD_CONFLICT, reg.u32);
}

hi_void tsio_hal_en_stuff_srv(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    U_SID_TABLE reg1;
    U_STUFF_SID reg2;

    TSIO_FATAL_ERROR(id >= mgmt->se_cnt);

    reg1.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_SID_TABLE(TO_SE_HW_ID(id)));

    reg1.bits.trans_type   = 0;
    reg1.bits.sid_table_en = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_SID_TABLE(TO_SE_HW_ID(id)), reg1.u32);

    reg2.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_STUFF_SID);

    reg2.bits.stuff_sid = TO_SE_HW_ID(id);

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_STUFF_SID, reg2.u32);
}

hi_void tsio_hal_en2dmx_srv(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 dmx_port_id)
{
    U_SID_TABLE reg;

    TSIO_FATAL_ERROR(id >= mgmt->se_cnt);
    TSIO_FATAL_ERROR(dmx_port_id >= mgmt->tsi_port_cnt);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_SID_TABLE(TO_SE_HW_ID(id)));

    reg.bits.outport_id   = dmx_port_id;
    reg.bits.trans_type   = 1;
    reg.bits.outport_en   = 1;
    reg.bits.sp_save      = 0;
    reg.bits.sid_table_en = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_SID_TABLE(TO_SE_HW_ID(id)), reg.u32);
}

hi_void tsio_hal_dis2dmx_srv(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    TSIO_FATAL_ERROR(id >= mgmt->se_cnt);

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_SID_TABLE(TO_SE_HW_ID(id)), 0);
}

hi_void tsio_hal_en2ram_srv(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    U_SID_TABLE reg;

    TSIO_FATAL_ERROR(id >= mgmt->se_cnt);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_SID_TABLE(TO_SE_HW_ID(id)));

    reg.bits.buf_id       = TO_SE_HW_ID(id);
    reg.bits.trans_type   = 1;
    reg.bits.dma_en       = 1;
    reg.bits.sp_save      = 0;
    reg.bits.sid_table_en = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_SID_TABLE(TO_SE_HW_ID(id)), reg.u32);
}

hi_void tsio_hal_dis2ram_srv(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    TSIO_FATAL_ERROR(id >= mgmt->se_cnt);

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_SID_TABLE(TO_SE_HW_ID(id)), 0);
}

hi_void tsio_hal_en_sp_save(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    U_SID_TABLE reg;

    TSIO_FATAL_ERROR(id >= mgmt->se_cnt);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_SID_TABLE(TO_SE_HW_ID(id)));

    reg.bits.sp_save = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_SID_TABLE(TO_SE_HW_ID(id)), reg.u32);
}

hi_void tsio_hal_dis_sp_save(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    U_SID_TABLE reg;

    TSIO_FATAL_ERROR(id >= mgmt->se_cnt);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_SID_TABLE(TO_SE_HW_ID(id)));

    reg.bits.sp_save = 0;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_SID_TABLE(TO_SE_HW_ID(id)), reg.u32);
}

hi_u32 tsio_hal_get_srv_pkt_count(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    TSIO_FATAL_ERROR(id >= mgmt->se_cnt);

    return TSIO_READ_REG(mgmt->io_base, TSIO_REG_SID_COUNTER(TO_SE_HW_ID(id)));
}

hi_void tsio_hal_en_all_int(const struct tsio_mgmt *mgmt)
{
    U_TSIO_IE reg1;
    U_RX_PARSER_ERR_IE reg2;
    U_DMA_CTRL reg3;
    U_DMA_COAL_CFG reg4;
    U_DMA_GLB_STAT reg5;

    /* general */
    reg1.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_INT_IE);

    reg1.bits.tx_cc_send_done_ie        = 1;
    reg1.bits.rx_cc_done_ie             = 1;
    reg1.bits.rx_cts_ie                 = 0;
    reg1.bits.rx_route_fifo_overflow_ie = 1;
    reg1.bits.tsio_ie                   = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_INT_IE, reg1.u32);

    /* rx parser err */
    reg2.u32 = 0;

    /* reg2.bits.rx_phy_sp_err_ie        = 1; no need this irq after phy irq enabled. */
    reg2.bits.rx_fifo_overflow_ie     = 1;
    reg2.bits.rx_sp_sync_err_ie       = 1;
    reg2.bits.rx_sp_rfu0_err_ie       = 1;

    /*
     * for TS based CC it maybe trigger DMA END interrupt, refer to SC FPGA userguide 2.4.
     * but the original hw design considers this to be an exception.
     * so we mask this interrupt.
     */
    reg2.bits.rx_sp_dma_end_err_ie    = 0;

    /*
     * tsid and scgen irq cause system hang when change stuff sid.
     */
    reg2.bits.rx_sp_tsid_err_ie       = 0;
    reg2.bits.rx_sp_sc_gen_err_ie     = 0;

    reg2.bits.rx_sp_encry_en_err_ie   = 1;
    reg2.bits.rx_sp_soc_define_err_ie = 1;
    reg2.bits.rx_sp_rfu1_err_ie       = 1;
    reg2.bits.rx_sp_rfu2_err_ie       = 1;
    reg2.bits.rx_sp_stuff_load_err_ie = 1;
    reg2.bits.rx_cc_err_type_ie       = 0xf;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_RX_PARSER_ERR_INT_IE, reg2.u32);

    /* tx rr err */
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_TX_RR_ERR_INT_IE, 0xFFFFFFFF);

    /* pid filter err */
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PID_FILTER_ERR_INT_IE, 0xFFFFFFFF);

    /* DMA */
    reg3.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_CTRL);

    reg3.bits.chnl_pend_int_en      = 1;
    reg3.bits.obuf_pack_int_en      = 1;
    reg3.bits.obuf_nr_int_en        = 1;
    reg3.bits.dma_err_int_en        = 1;
    reg3.bits.dma_end_int_en        = 1;
    reg3.bits.dma_flush_int_en      = 1;
    reg3.bits.dmux_pend_en          = 1;
    reg3.bits.des_end_en            = 1;
    reg3.bits.dma_bid_err_en        = 1;
    reg3.bits.dma_live_oflw_err_en  = 1;
    reg3.bits.chk_code_err_en       = 1;
    reg3.bits.obuf_oflw_err_en      = 1;
    reg3.bits.des_type_err_en       = 1;
    reg3.bits.ichl_wptr_oflw_err_en = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_CTRL, reg3.u32);

    /* default cal time cycle 0.5ms = 1000 * 0.5us;  0.5us ~= 1s/27mhz/14division. */
    reg4.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_COAL_CFG);

    reg4.bits.coal_time_cyc = 10000; /* 5ms: 10000 * 0.5us */

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_COAL_CFG, reg4.u32);

    /* umask DMA int */
    reg5.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_GBL_STAT);

    reg5.bits.dma_int_msk = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_GBL_STAT, reg5.u32);
}

hi_void tsio_hal_dis_all_int(const struct tsio_mgmt *mgmt)
{
    U_DMA_GLB_STAT reg1;
    U_TSIO_IE reg2;

    /* DMA */
    reg1.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_GBL_STAT);

    reg1.bits.dma_int_msk = 0;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_GBL_STAT, reg1.u32);

    /* general */
    reg2.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_INT_IE);

    reg2.bits.tsio_ie = 0;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_INT_IE, reg2.u32);
}

hi_void tsio_hal_en_phy_int(const struct tsio_mgmt *mgmt)
{
    U_PHY_MISC reg;

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_MISC);

    reg.bits.int_mask = 0;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PHY_MISC, reg.u32);
}

hi_void tsio_hal_dis_phy_int(const struct tsio_mgmt *mgmt)
{
    U_PHY_MISC reg;

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PHY_MISC);

    reg.bits.int_mask = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PHY_MISC, reg.u32);
}

hi_void tsio_hal_mask_all_dma_int(const struct tsio_mgmt *mgmt)
{
    U_DMA_GLB_STAT reg;
    hi_size_t lock_flag;

    spin_lock_irqsave(&g_tsio_hal_lock, lock_flag);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_GBL_STAT);

    reg.bits.dma_int_msk = 0;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_GBL_STAT, reg.u32);

    spin_unlock_irqrestore(&g_tsio_hal_lock, lock_flag);
}

hi_void tsio_hal_un_mask_all_dma_int(const struct tsio_mgmt *mgmt)
{
    U_DMA_GLB_STAT reg;
    hi_size_t lock_flag;

    spin_lock_irqsave(&g_tsio_hal_lock, lock_flag);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_GBL_STAT);

    reg.bits.dma_int_msk = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_GBL_STAT, reg.u32);

    spin_unlock_irqrestore(&g_tsio_hal_lock, lock_flag);
}

hi_u32 tsio_hal_get_int_flag(const struct tsio_mgmt *mgmt)
{
    U_TSIO_MIS reg;

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_INT_MIS);

    return reg.u32;
}

hi_void tsio_hal_clr_tx_cc_done_int(const struct tsio_mgmt *mgmt)
{
    U_TSIO_RIS reg;
    hi_size_t lock_flag;

    spin_lock_irqsave(&g_tsio_hal_lock, lock_flag);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_INT_RIS);

    reg.bits.tx_cc_send_done_int = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_INT_RIS, reg.u32);

    spin_unlock_irqrestore(&g_tsio_hal_lock, lock_flag);
}

hi_void tsio_hal_clr_rx_cc_done_int(const struct tsio_mgmt *mgmt)
{
    U_TSIO_RIS reg;
    hi_size_t lock_flag;

    spin_lock_irqsave(&g_tsio_hal_lock, lock_flag);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_INT_RIS);

    reg.bits.rx_cc_done_int = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_INT_RIS, reg.u32);

    spin_unlock_irqrestore(&g_tsio_hal_lock, lock_flag);
}

hi_void tsio_hal_clr_rx_cts_int(const struct tsio_mgmt *mgmt)
{
    U_TSIO_RIS reg;
    hi_size_t lock_flag;

    spin_lock_irqsave(&g_tsio_hal_lock, lock_flag);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_INT_RIS);

    reg.bits.rx_cts_int = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_INT_RIS, reg.u32);

    spin_unlock_irqrestore(&g_tsio_hal_lock, lock_flag);
}

hi_void tsio_hal_clr_rx_route_fifo_ovfl_int(const struct tsio_mgmt *mgmt)
{
    U_TSIO_RIS reg;
    hi_size_t lock_flag;

    spin_lock_irqsave(&g_tsio_hal_lock, lock_flag);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_INT_RIS);

    reg.bits.rx_route_fifo_overflow_int = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_INT_RIS, reg.u32);

    spin_unlock_irqrestore(&g_tsio_hal_lock, lock_flag);
}

hi_u32 tsio_hal_get_org_rx_parser_err_int_flag(const struct tsio_mgmt *mgmt)
{
    U_TSIO_MIS reg;
    hi_size_t lock_flag;

    spin_lock_irqsave(&g_tsio_hal_lock, lock_flag);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_RX_PARSER_ERR_INT_RIS);

    spin_unlock_irqrestore(&g_tsio_hal_lock, lock_flag);

    return reg.u32;
}

hi_u32 tsio_hal_get_rx_parser_err_int_flag(const struct tsio_mgmt *mgmt)
{
    U_TSIO_MIS reg;
    hi_size_t lock_flag;

    spin_lock_irqsave(&g_tsio_hal_lock, lock_flag);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_RX_PARSER_ERR_INT_MIS);

    spin_unlock_irqrestore(&g_tsio_hal_lock, lock_flag);

    return reg.u32;
}

hi_void tsio_hal_clr_rx_parser_err_int_flag(const struct tsio_mgmt *mgmt, hi_u32 flag)
{
    hi_size_t lock_flag;

    spin_lock_irqsave(&g_tsio_hal_lock, lock_flag);

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_RX_PARSER_ERR_INT_RIS, flag);

    spin_unlock_irqrestore(&g_tsio_hal_lock, lock_flag);
}

hi_u32 tsio_hal_get_tx_rr_err_int_flag(const struct tsio_mgmt *mgmt)
{
    U_TSIO_MIS reg;

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_TX_RR_ERR_INT_MIS);

    return reg.u32;
}

hi_void tsio_hal_clr_tx_rr_err_int_flag(const struct tsio_mgmt *mgmt, hi_u32 flag)
{
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_TX_RR_ERR_INT_RIS, flag);
}

hi_u32 tsio_hal_get_pid_filter_err_int_flag(const struct tsio_mgmt *mgmt)
{
    U_TSIO_MIS reg;

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_PID_FILTER_ERR_INT_MIS);

    return reg.u32;
}

hi_void tsio_hal_clr_pid_filter_err_int_flag(const struct tsio_mgmt *mgmt, hi_u32 flag)
{
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_PID_FILTER_ERR_INT_RIS, flag);
}

hi_u32 tsio_hal_get_dma_int_flag(const struct tsio_mgmt *mgmt)
{
    U_DMA_GLB_STAT reg;
    hi_size_t lock_flag;

    spin_lock_irqsave(&g_tsio_hal_lock, lock_flag);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_GBL_STAT);

    spin_unlock_irqrestore(&g_tsio_hal_lock, lock_flag);

    return reg.u32;
}

hi_u32 tsio_hal_get_dma_des_end_status(const struct tsio_mgmt *mgmt)
{
    return TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_DES_END_INT);
}

hi_void tsio_hal_clr_dma_des_end_status(const struct tsio_mgmt *mgmt, hi_u32 status)
{
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_DES_END_INT, status);
}

hi_u32 tsio_hal_get_dma_chn_pend_status(const struct tsio_mgmt *mgmt)
{
    hi_u32 val;
    hi_size_t lock_flag;

    spin_lock_irqsave(&g_tsio_hal_lock, lock_flag);

    val = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_PEND_INT);

    spin_unlock_irqrestore(&g_tsio_hal_lock, lock_flag);

    return val;
}

hi_void tsio_hal_clr_dma_chn_pend_status(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    hi_u32 flags;
    hi_size_t lock_flag;

    TSIO_FATAL_ERROR(id >= mgmt->ram_port_cnt);

    spin_lock_irqsave(&g_tsio_hal_lock, lock_flag);

    flags = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_PEND_INT);
    if (flags & (0x1 << TO_RAMPORT_HW_ID(id))) {
        TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_PEND_INT, 0x1 << TO_RAMPORT_HW_ID(id));
    }

    spin_unlock_irqrestore(&g_tsio_hal_lock, lock_flag);
}

hi_u32 tsio_hal_get_dma_pack_int_status_l(const struct tsio_mgmt *mgmt)
{
    return TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_PACK_INT_L);
}

hi_void tsio_hal_clr_dma_pack_int_status_l(const struct tsio_mgmt *mgmt, hi_u32 status)
{
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_PACK_INT_L, status);
}

hi_u32 tsio_hal_get_dma_pack_int_status_h(const struct tsio_mgmt *mgmt)
{
    return TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_PACK_INT_H);
}

hi_void tsio_hal_clr_dma_pack_int_status_h(const struct tsio_mgmt *mgmt, hi_u32 status)
{
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_PACK_INT_H, status);
}

hi_u32 tsio_hal_get_dma_end_int_status_l(const struct tsio_mgmt *mgmt)
{
    return TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_END_INT_L);
}

hi_void tsio_hal_clr_dma_end_int_status_l(const struct tsio_mgmt *mgmt, hi_u32 status)
{
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_END_INT_L, status);
}

hi_u32 tsio_hal_get_dma_end_int_status_h(const struct tsio_mgmt *mgmt)
{
    return TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_END_INT_H);
}

hi_void tsio_hal_clr_dma_end_int_status_h(const struct tsio_mgmt *mgmt, hi_u32 status)
{
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_END_INT_H, status);
}

hi_u32 tsio_hal_get_dma_obuf_ovflw_status_l(const struct tsio_mgmt *mgmt)
{
    return TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_OBUF_OVFLW_L);
}

hi_void tsio_hal_clr_dma_obuf_ovflw_status_l(const struct tsio_mgmt *mgmt, hi_u32 status)
{
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_OBUF_OVFLW_L, status);
}

hi_u32 tsio_hal_get_dma_obuf_ovflw_status_h(const struct tsio_mgmt *mgmt)
{
    return TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_OBUF_OVFLW_H);
}

hi_void tsio_hal_clr_dma_obuf_ovflw_status_h(const struct tsio_mgmt *mgmt, hi_u32 status)
{
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_OBUF_OVFLW_H, status);
}

hi_u32 tsio_hal_get_dma_flush_status_l(const struct tsio_mgmt *mgmt)
{
    return TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_FLUSH_L);
}

hi_void tsio_hal_clr_dma_flush_status_l(const struct tsio_mgmt *mgmt, hi_u32 status)
{
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_FLUSH_L, status);
}

hi_u32 tsio_hal_get_dma_flush_status_h(const struct tsio_mgmt *mgmt)
{
    return TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_FLUSH_H);
}

hi_void tsio_hal_clr_dma_flush_status_h(const struct tsio_mgmt *mgmt, hi_u32 status)
{
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_FLUSH_H, status);
}

hi_void tsio_hal_set_dma_cnt_unit(const struct tsio_mgmt *mgmt)
{
    U_DMA_CNT_UNIT reg;

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_CNT_UNIT);
    /* bus freq 392MHZ, 0.125us, equal 49 cycle, 49-1= 48  */
    reg.bits.pulse_cyc = 48;
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_CNT_UNIT, reg.u32);

    return;
}

hi_void tsio_hal_en_mmu(const struct tsio_mgmt *mgmt)
{
    U_DMA_CTRL reg;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_TLB_BASE, mgmt->cb_ttbr);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_CTRL);

    reg.bits.dma_mmu_en = 1;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_CTRL, reg.u32);
}

static inline hi_void set_ram_port_rate(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 pace)
{
    U_DMA_CHNL_PACE reg;

    TSIO_FATAL_ERROR(id >= mgmt->ram_port_cnt);
    TSIO_FATAL_ERROR(!(pace <= 255)); /* max rate 255 */

    /* hw request reset to 0 firstly */
    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_PACE(TO_RAMPORT_HW_ID(id)));

    reg.bits.dma_chnl_pace = 0;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_PACE(TO_RAMPORT_HW_ID(id)), reg.u32);

    /* config new pace */
    reg.bits.dma_chnl_pace = pace;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_PACE(TO_RAMPORT_HW_ID(id)), reg.u32);
}

hi_void tsio_hal_en_ram_port(const struct tsio_mgmt *mgmt, hi_u32 id,
    hi_u64 dsc_phy_addr, hi_u32 dsc_depth, hi_u32 pace)
{
    U_DMA_CHNL_DEPTH depth;
    U_DMA_CHNL_DIS dis_chnl;

    TSIO_FATAL_ERROR(id >= mgmt->ram_port_cnt);

    /* config dsc base addr */
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DQCT_TAB_ADDR(TO_RAMPORT_HW_ID(id)), (hi_u32)(dsc_phy_addr & 0xffffffff));
    /* config dsc high 4 bit addr */
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DQCT_TAB_ADDR_SESSION_ID(TO_RAMPORT_HW_ID(id)),
        (hi_u32)((dsc_phy_addr >> TSIO_BITS_PER_REG) & 0xf));

    /* config dsc depth */
    depth.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_DEPTH(TO_RAMPORT_HW_ID(id)));

    depth.bits.dma_chnl_depth = dsc_depth - 1; /* hw rule: -1. */

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_DEPTH(TO_RAMPORT_HW_ID(id)), depth.u32);

    /* config max data rate with pace value. */
    set_ram_port_rate(mgmt, id, pace);

    /* invlidate old mmu map */
    dis_chnl.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_DIS);

    dis_chnl.bits.dma_pi_mmu_dis |= (1 << TO_RAMPORT_HW_ID(id));

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_DIS, dis_chnl.u32);
}

hi_void tsio_hal_dis_ram_port(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    U_DMA_CHNL_DIS dis_chnl;

    TSIO_FATAL_ERROR(id >= mgmt->ram_port_cnt);

    dis_chnl.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_DIS);

    dis_chnl.bits.dma_chanls_dis |= (1 << TO_RAMPORT_HW_ID(id));

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_DIS, dis_chnl.u32);
}

hi_bool tsio_hal_ram_port_enabled(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    hi_u32 reg;

    TSIO_FATAL_ERROR(id >= mgmt->ram_port_cnt);

    reg = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_STAT);

    return (reg & (1 << TO_RAMPORT_HW_ID(id))) ? HI_TRUE : HI_FALSE;
}

hi_void tsio_hal_set_ram_port_rate(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 pace)
{
    TSIO_FATAL_ERROR(id >= mgmt->ram_port_cnt);

    /* config max data rate with pace value. */
    set_ram_port_rate(mgmt, id, pace);
}

hi_void tsio_hal_setup_ts_dsc(const struct tsio_mgmt *mgmt,
    const struct tsio_dsc_base_info *base_info, hi_bool desepon, hi_u32 *cur_dsc_addr)
{
    U_DMA_DSC_WORD_0 reg;
    hi_u32 id = base_info->id;
    hi_u64 buf_phy_addr = base_info->buf_phy_addr;
    hi_u32 pkt_cnt = base_info->pkt_cnt;

    TSIO_FATAL_ERROR(id >= mgmt->ram_port_cnt);

    reg.u32 = 0;

    reg.bits.playnums = pkt_cnt - 1; /* [0, 255] */
    reg.bits.tstype   = 0x1;        /* 0:live ts; 1:dma ts; 2:dma bulk */
    reg.bits.tsid     = 0x80 + TO_RAMPORT_HW_ID(id);   /* 0x80 is ram port base */
    reg.bits.desep    = desepon == HI_TRUE ? 1 : 0;

    *cur_dsc_addr++ = reg.u32;
    *cur_dsc_addr++ = (hi_u32)(buf_phy_addr & 0xffffffff);
    *cur_dsc_addr++ = 0;
    /* 28..31 is the high 4 bit of 36 bit phy addr */
    *cur_dsc_addr++ = (((hi_u32)(buf_phy_addr >> TSIO_BITS_PER_REG) & 0xf) << TSIO_DSC_GUIDE_NUM_LEN)
        + RAM_DSC_GUIDE_NUMBER;

    osal_mb();
}

hi_void tsio_hal_setup_bulk_dsc(const struct tsio_mgmt *mgmt,
    const struct tsio_dsc_base_info *base_info, hi_bool desepon, hi_u32 *cur_dsc_addr, hi_u32 sid)
{
    U_DMA_DSC_WORD_0 reg;
    hi_u32 id = base_info->id;
    hi_u64 buf_phy_addr = base_info->buf_phy_addr;
    hi_u32 pkt_cnt = base_info->pkt_cnt;

    TSIO_FATAL_ERROR(id >= mgmt->ram_port_cnt);
    TSIO_FATAL_ERROR(sid >= mgmt->se_cnt);

    reg.u32 = 0;

    reg.bits.playnums = pkt_cnt - 1; /* [0, 255] */
    reg.bits.sid      = TO_SE_HW_ID(sid);
    reg.bits.tstype   = 0x2;        /* 0:live ts; 1:dma ts; 2:dma bulk */
    reg.bits.tsid     = 0x80 + TO_RAMPORT_HW_ID(id);   /* 0x80 is ram port base */
    reg.bits.desep    = desepon == HI_TRUE ? 1 : 0;

    *cur_dsc_addr++ = reg.u32;
    *cur_dsc_addr++ = (hi_u32)(buf_phy_addr & 0xffffffff);
    *cur_dsc_addr++ = 0;
    /* 28..31 is the high 4 bit of 36 bit phy addr */
    *cur_dsc_addr++ = (((hi_u32)(buf_phy_addr >> TSIO_BITS_PER_REG) & 0xf) << TSIO_DSC_GUIDE_NUM_LEN)
        + RAM_DSC_GUIDE_NUMBER;

    osal_mb();
}

hi_void tsio_hal_setup_bulk_flush_dsc(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 *cur_dsc_addr, hi_u32 sid)
{
    U_DMA_DSC_WORD_0 reg;

    TSIO_FATAL_ERROR(id >= mgmt->ram_port_cnt);
    TSIO_FATAL_ERROR(sid >= mgmt->se_cnt);

    reg.u32 = 0;

    reg.bits.playnums = 0; /* [0, 255] */
    reg.bits.sid      = TO_SE_HW_ID(sid);
    reg.bits.tstype   = 0x2;        /* 0:live ts; 1:dma ts; 2:dma bulk */
    reg.bits.tsid     = 0x80 + TO_RAMPORT_HW_ID(id);   /* 0x80 is ram port base */
    reg.bits.flush    = 1;
    reg.bits.desep    = 1;

    *cur_dsc_addr++ = reg.u32;
    *cur_dsc_addr++ = 0;
    *cur_dsc_addr++ = 0;
    *cur_dsc_addr++ = RAM_DSC_GUIDE_NUMBER;

    osal_mb();
}

hi_void tsio_hal_add_dsc(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 write_idx)
{
    U_DMA_SLOT_PI_W reg;
    hi_u32 en_chnl;

    TSIO_FATAL_ERROR(id >= mgmt->ram_port_cnt);

    /* enable ramport delay to add dsc for the accuracy of rate calculation */
    en_chnl = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_STAT);
    if (unlikely(!(en_chnl & (1 << TO_RAMPORT_HW_ID(id))))) {
        /* en ram port */
        en_chnl = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_EN);

        en_chnl |= (1 << TO_RAMPORT_HW_ID(id));

        TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_CHNL_EN, en_chnl);
    }

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_SLOT_PI_W);

    reg.bits.sw_pi_w_bid = TO_RAMPORT_HW_ID(id);
    reg.bits.sw_pi_wptr  = write_idx;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_SLOT_PI_W, reg.u32);
}

hi_u32 tsio_hal_get_ram_port_cur_read_idx(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    U_DMA_SLOT_PI_R reg;

    TSIO_FATAL_ERROR(id >= mgmt->ram_port_cnt);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_SLOT_PI_R(TO_RAMPORT_HW_ID(id)));

    return reg.bits.hw_pi_rptr;
}

hi_u32 tsio_hal_get_ram_port_cur_write_idx(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    U_DMA_SLOT_PI_R reg;

    TSIO_FATAL_ERROR(id >= mgmt->ram_port_cnt);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_SLOT_PI_R(TO_RAMPORT_HW_ID(id)));

    return reg.bits.hw_pi_wptr;
}

hi_void tsio_hal_en_obuf(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u64 buf_phy_addr, hi_u32 buf_size)
{
    U_DMA_OBUF_LEN len;
    U_DMA_OBUF_THRED thred;

    TSIO_FATAL_ERROR(id >= mgmt->se_cnt);

    /* config buf base addr */
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_OBUF_ADDR(TO_SE_HW_ID(id)), buf_phy_addr);
    /* config buf base high 4 bit addr */
    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_OBUF_ADDR_HIGH(TO_SE_HW_ID(id)),
        (hi_u32)((buf_phy_addr >> TSIO_BITS_PER_REG) & 0xf));

    /* config buf len */
    len.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_OBUF_LEN(TO_SE_HW_ID(id)));

    len.bits.dma_obuf_length = buf_size;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_OBUF_LEN(TO_SE_HW_ID(id)), len.u32);

    /* config thresh */
    thred.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_OBUF_THRED(TO_SE_HW_ID(id)));

    thred.bits.dma_obuf_thresh = DEFAULT_SE_OBUF_THRESH;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_OBUF_THRED(TO_SE_HW_ID(id)), thred.u32);

    /* invlidate old mmu map */
    if (TO_SE_HW_ID(id) < NUM_SIZE) {
        TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_MMU_DIS_L, (1 << TO_SE_HW_ID(id)));
    } else {
        TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_MMU_DIS_H, (1 << TO_SE_HW_ID(id - NUM_SIZE)));
    }

    /* en obuf. */
    if (TO_SE_HW_ID(id) < NUM_SIZE) {
        TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_OBUF_ENB_L, (1 << TO_SE_HW_ID(id)));
    } else {
        TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_OBUF_ENB_H, (1 << TO_SE_HW_ID(id - NUM_SIZE)));
    }
}

hi_void tsio_hal_dis_obuf(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    TSIO_FATAL_ERROR(id >= mgmt->se_cnt);

    /* en obuf. */
    if (TO_SE_HW_ID(id) < NUM_SIZE) {
        TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_OBUF_DIS_L, (1 << TO_SE_HW_ID(id)));
    } else {
        TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_OBUF_DIS_H, (1 << TO_SE_HW_ID(id - NUM_SIZE)));
    }
}

hi_u32 tsio_hal_get_obuf_write(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    U_DMA_SLOT_PO_W reg;

    TSIO_FATAL_ERROR(id >= mgmt->se_cnt);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_SLOT_PO_W(TO_SE_HW_ID(id)));

    return reg.bits.hw_po_wptr;
}

hi_u32 tsio_hal_get_obuf_read(const struct tsio_mgmt *mgmt, hi_u32 id)
{
    U_DMA_SLOT_PO_R reg;

    TSIO_FATAL_ERROR(id >= mgmt->se_cnt);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_SLOT_PO_R(TO_SE_HW_ID(id)));

    return reg.bits.sw_po_rptr;
}

hi_void tsio_hal_set_obuf_read(const struct tsio_mgmt *mgmt, hi_u32 id, hi_u32 cur_read)
{
    U_DMA_SLOT_PO_R reg;

    TSIO_FATAL_ERROR(id >= mgmt->se_cnt);

    reg.u32 = TSIO_READ_REG(mgmt->io_base, TSIO_REG_DMA_SLOT_PO_R(TO_SE_HW_ID(id)));

    reg.bits.sw_po_rptr = cur_read;

    TSIO_WRITE_REG(mgmt->io_base, TSIO_REG_DMA_SLOT_PO_R(TO_SE_HW_ID(id)), reg.u32);
}

