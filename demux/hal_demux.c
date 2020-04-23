/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux hw hal function impl.
 * Author: sdk
 * Create: 2017-06-05
 */

#include "linux/io.h"
#include "linux/delay.h"
#include "linux/ion.h"
#include "linux/hisilicon/hi_license.h"
#include "hi_reg_common.h"
#include "hi_drv_sys.h"

#include "drv_demux_define.h"
#include "drv_demux_func.h"
#include "drv_demux_reg.h"
#include "hal_demux.h"

#ifndef HI_REG_READ
#define HI_REG_READ(addr, result)  ((result) = *(volatile unsigned int *)(addr))
#endif

#ifndef HI_REG_WRITE
#define HI_REG_WRITE(addr, result)  (*(volatile unsigned int *)(addr) = (result))
#endif

hi_void dmx_hal_init_hw(hi_void)
{
    hi_s32 ret;
    hi_u32 tsi_policy;

    volatile hi_reg_crg *peri_crg = hi_drv_sys_get_crg_reg_ptr();

    DMX_NULL_POINTER_VOID(peri_crg);

    ret = hi_drv_get_license_support(HI_LICENSE_TSI_EN, &tsi_policy);
    if (ret == HI_SUCCESS && (tsi_policy > 0)) {
        hi_u32 i;
        hi_u32 tsi_support_bits = 0;
        hi_u32 tsi_num = 1U << (tsi_policy + 1); /* tsi_policy:1, 4 tsi; tsi_policy:2, 8 tsi; tsi_policy:3, 16 tsi */
        for (i = 0; i < tsi_num; i++) {
            tsi_support_bits |= 1U << i;
        }

        /* enable all tsi port */
        peri_crg->PERI_CRG205.u32 |= tsi_support_bits; /* support 4, 8, 16 */
    } else {
        /* disable all tsi port */
        peri_crg->PERI_CRG205.u32 &= ~0xFFFFU; /* 16 bits means 16 tsi port */
    }

   /* check whether the clock has configured */
    if (peri_crg->PERI_CRG206.bits.pvr_bus_cken == 1 &&
        peri_crg->PERI_CRG206.bits.pvr_dmx_cken == 1) {
        return;
    }

    /* reset demux */
    peri_crg->PERI_CRG206.bits.dmx_srst_req = 1;
    osal_mb();

    peri_crg->PERI_CRG206.bits.pvr_bus_cken     = 1;
    peri_crg->PERI_CRG206.bits.pvr_dmx_cken     = 1;
    peri_crg->PERI_CRG206.bits.pvr_27m_cken     = 1;
    peri_crg->PERI_CRG206.bits.pvr_ts0_cken     = 1;
    peri_crg->PERI_CRG206.bits.pvr_tsout0_cken  = 1;
    peri_crg->PERI_CRG206.bits.pvr_tsout1_cken  = 1;
    peri_crg->PERI_CRG206.bits.dmx_srst_req     = 0;

    osal_mb();

    peri_crg->PERI_CRG207.bits.pvr_ts0_cksel = 1;
    peri_crg->PERI_CRG207.bits.pvr_ts1_cksel = 1;
}

hi_void dmx_hal_deinit_hw(hi_void)
{
    volatile hi_reg_crg *peri_crg = hi_drv_sys_get_crg_reg_ptr();

    DMX_NULL_POINTER_VOID(peri_crg);

    /* reset demux */
    peri_crg->PERI_CRG206.bits.dmx_srst_req = 1;

    osal_mb();

    /* disable all tsi port */
    peri_crg->PERI_CRG205.u32 &= ~0xFFFFU; /* 16 bits means 16 tsi port */

    peri_crg->PERI_CRG206.bits.pvr_bus_cken     = 0;
    peri_crg->PERI_CRG206.bits.pvr_dmx_cken     = 0;
    peri_crg->PERI_CRG206.bits.pvr_27m_cken     = 0;
    peri_crg->PERI_CRG206.bits.pvr_ts0_cken     = 0;
    peri_crg->PERI_CRG206.bits.pvr_tsout0_cken  = 0;
    peri_crg->PERI_CRG206.bits.pvr_tsout1_cken  = 0;
    peri_crg->PERI_CRG206.bits.dmx_srst_req     = 0;

    osal_mb();

    peri_crg->PERI_CRG207.bits.pvr_ts0_cksel  = 0;
    peri_crg->PERI_CRG207.bits.pvr_ts1_cksel  = 0;
}

#ifdef DMX_SMMU_SUPPORT
hi_void dmx_hal_en_mmu(struct dmx_mgmt *mgmt)
{
    hi_u32 i = 0;
    U_NOSEC_MMU_EN nosec_mmu_en;
    U_NOSEC_EADDR_SESSION nosec_eaddr;
    U_NOSEC_R_EADDR_SESSION nosec_r_eaddr;
    hi_ulong cb_ttbr = 0;
    hi_ulong err_rd_addr = 0;
    hi_ulong err_wr_addr = 0;

    hi_drv_nssmmu_get_page_table_addr(&cb_ttbr, &err_rd_addr, &err_wr_addr);
    mgmt->cb_ttbr = cb_ttbr;

    /* hw restrict cb_ttbr must be 16bytes align. */
    WARN_ON(cb_ttbr % 16);
    /* configure the mmu page table base register */
    DMX_WRITE_REG_DAV(mgmt->io_base, MMU_NOSEC_TLB, cb_ttbr >> 4); /* must 16 byte align, shift 4 bits */
    DMX_WRITE_REG_DAV(mgmt->io_base, MMU_NOSEC_EADDR, err_wr_addr >> 12); /* shift 12 bits */
    DMX_WRITE_REG_DAV(mgmt->io_base, MMU_R_NOSEC_EADDR, err_rd_addr >> 12); /* shift 12 bits */

    /* config the err write addr session */
    nosec_eaddr.u32 = 0x0;
    nosec_eaddr.bits.nosec_eaddr_session = (err_wr_addr >> 32) & 0xFU; /* shift 32 bits */
    DMX_WRITE_REG_DAV(mgmt->io_base, NOSEC_EADDR_SESSION, nosec_eaddr.u32);

    /* config the err read addr session */
    nosec_r_eaddr.u32 = 0x0;
    nosec_r_eaddr.bits.nosec_r_eaddr_session = (err_rd_addr >> 32) & 0xFU; /* shift 32 bits */
    DMX_WRITE_REG_DAV(mgmt->io_base, NOSEC_R_EADDR_SESSION, nosec_r_eaddr.u32);

    nosec_mmu_en.u32 = DMX_READ_REG_DAV(mgmt->io_base, NOSEC_MMU_EN);
    nosec_mmu_en.bits.nosec_mmu_en = 1;

    DMX_WRITE_REG_DAV(mgmt->io_base, NOSEC_MMU_EN, nosec_mmu_en.u32);

    DMX_COM_EQUAL(nosec_mmu_en.u32, DMX_READ_REG_DAV(mgmt->io_base, NOSEC_MMU_EN));

    /* clear the play rec buffer mmu cache, total 1024 buf, (0~31)1024/BITS_PER_REG */
    for (i = 0; i <= BITS_PER_REG - 1; i++) {
        DMX_WRITE_REG_BUF(mgmt->io_base, MMU_BUF_DIS(i), 0xFFFFFFFF);
    }

    /* clear the pc read buffer mmu cache, (0~1)two register */
    for (i = 0; i <= 1; i++) {
        DMX_WRITE_REG_BUF(mgmt->io_base, MMU_PC_RDIS_0(i), 0xFFFFFFFF);
    }

    /* clear the pc write buffer mmu cache, (0~1)two register */
    for (i = 0; i <= 1; i++) {
        DMX_WRITE_REG_BUF(mgmt->io_base, MMU_PC_WDIS_0(i), 0xFFFFFFFF);
    }

    /* clear the ip buffer mmu cache */
    DMX_WRITE_REG_RAM(mgmt->io_base, MMU_IP_DIS, 0xFFFFFFFF);

    /* clear the ip desc mmu cache */
    DMX_WRITE_REG_RAM(mgmt->io_base, MMU_IP_DES_DIS, 0xFFFFFFFF);
}

hi_void dmx_hal_dis_mmu(const struct dmx_mgmt *mgmt)
{
    U_NOSEC_MMU_EN nosec_mmu_en;

    nosec_mmu_en.u32 = DMX_READ_REG_DAV(mgmt->io_base, NOSEC_MMU_EN);
    nosec_mmu_en.bits.nosec_mmu_en = 0;

    DMX_WRITE_REG_DAV(mgmt->io_base, NOSEC_MMU_EN, nosec_mmu_en.u32);

    DMX_COM_EQUAL(nosec_mmu_en.u32, DMX_READ_REG_DAV(mgmt->io_base, NOSEC_MMU_EN));
}

hi_void dmx_hal_buf_clr_mmu_cache(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    hi_u32 reg_index;
    hi_u32 bit_offset;
    hi_u32 reg;

    WARN_ON(id >= mgmt->buf_cnt);

    reg_index  = id / BITS_PER_REG;
    bit_offset = id % BITS_PER_REG;

    reg = (1U << bit_offset);

    DMX_WRITE_REG_BUF(mgmt->io_base, MMU_BUF_DIS(reg_index), reg);
}

hi_void dmx_hal_pid_copy_clr_mmu_cache(const struct dmx_mgmt *mgmt, hi_u32 pcid)
{
    hi_u32 reg_index;
    hi_u32 bit_offset;
    hi_u32 reg;

    WARN_ON(pcid >= mgmt->pid_copy_chan_cnt);

    reg_index  = pcid / BITS_PER_REG;
    bit_offset = pcid % BITS_PER_REG;

    reg = (1U << bit_offset);

    DMX_WRITE_REG_BUF(mgmt->io_base, MMU_PC_RDIS_0(reg_index), reg);
    DMX_WRITE_REG_BUF(mgmt->io_base, MMU_PC_WDIS_0(reg_index), reg);
}

hi_void dmx_hal_ram_clr_mmu_cache(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    hi_u32 bit_offset;
    hi_u32 reg;

    WARN_ON(id >= mgmt->ram_port_cnt);

    bit_offset = id % BITS_PER_REG;

    reg = (1U << bit_offset);

    DMX_WRITE_REG_RAM(mgmt->io_base, MMU_IP_DIS, reg);
    DMX_WRITE_REG_RAM(mgmt->io_base, MMU_IP_DES_DIS, reg);
}
#endif

/*
 * general int definition for demux begin.
 */
hi_void dmx_hal_en_all_int(const struct dmx_mgmt *mgmt)
{
    DMX_WRITE_REG_CFG(mgmt->io_base, DMX_IE_ALL_INT_ENABLE, 1);

    DMX_COM_EQUAL(1, DMX_READ_REG_CFG(mgmt->io_base, DMX_IE_ALL_INT_ENABLE));
}

hi_void dmx_hal_dis_all_int(const struct dmx_mgmt *mgmt)
{
    DMX_WRITE_REG_CFG(mgmt->io_base, DMX_IE_ALL_INT_ENABLE, 0);

    DMX_COM_EQUAL(0, DMX_READ_REG_CFG(mgmt->io_base, DMX_IE_ALL_INT_ENABLE));
}

hi_u32  dmx_hal_get_int_flag(const struct dmx_mgmt *mgmt)
{
    U_DMX_IS reg;

    reg.u32 = DMX_READ_REG_CFG(mgmt->io_base, DMX_IS_ALL_INT_STATUS);

    return reg.u32;
}

/*
 * dav int definition for demux end.
 */
hi_void dmx_hal_mask_all_dav_int(const struct dmx_mgmt *mgmt)
{
    U_DAV_INT_MASK reg;

    reg.u32 = DMX_READ_REG_DAV(mgmt->io_base, DAV_INT_MASK);

    reg.bits.dav_int_mask = 1;

    DMX_WRITE_REG_DAV(mgmt->io_base, DAV_INT_MASK, reg.u32);
}

hi_void dmx_hal_un_mask_all_dav_int(const struct dmx_mgmt *mgmt)
{
    U_DAV_INT_MASK reg;

    reg.u32 = DMX_READ_REG_DAV(mgmt->io_base, DAV_INT_MASK);

    reg.bits.dav_int_mask = 0;

    DMX_WRITE_REG_DAV(mgmt->io_base, DAV_INT_MASK, reg.u32);
}

hi_u32  dmx_hal_get_dav_int_flag(const struct dmx_mgmt *mgmt)
{
    U_DAV_INT reg;

    reg.u32 = DMX_READ_REG_DAV(mgmt->io_base, DAV_INT_ALL_INT_STATUS);

    return reg.u32;
}

hi_void dmx_hal_clr_dav_timeout_int_flag(const struct dmx_mgmt *mgmt)
{
    U_DAV_INT reg;

    reg.u32 = DMX_READ_REG_DAV(mgmt->io_base, DAV_INT_ALL_INT_STATUS);

    reg.bits.timeout_int_flg = 1;
    reg.bits.ts_buf_int_flg = 0;
    reg.bits.seop_int_flg = 0;
    reg.bits.prs_ovfl_int_flg = 0;

    DMX_WRITE_REG_DAV(mgmt->io_base, DAV_INT_ALL_INT_STATUS, reg.u32);
}

hi_void dmx_hal_clr_dav_ts_int_flag(const struct dmx_mgmt *mgmt)
{
    U_DAV_INT reg;

    reg.u32 = DMX_READ_REG_DAV(mgmt->io_base, DAV_INT_ALL_INT_STATUS);

    reg.bits.timeout_int_flg = 0;
    reg.bits.ts_buf_int_flg = 1;
    reg.bits.seop_int_flg = 0;
    reg.bits.prs_ovfl_int_flg = 0;

    DMX_WRITE_REG_DAV(mgmt->io_base, DAV_INT_ALL_INT_STATUS, reg.u32);
}

hi_void dmx_hal_clr_dav_seop_int_flag(const struct dmx_mgmt *mgmt)
{
    U_DAV_INT reg;

    reg.u32 = DMX_READ_REG_DAV(mgmt->io_base, DAV_INT_ALL_INT_STATUS);

    reg.bits.timeout_int_flg = 0;
    reg.bits.ts_buf_int_flg = 0;
    reg.bits.seop_int_flg = 1;
    reg.bits.prs_ovfl_int_flg = 0;

    DMX_WRITE_REG_DAV(mgmt->io_base, DAV_INT_ALL_INT_STATUS, reg.u32);
}

hi_void dmx_hal_clr_dav_prs_ovfl_int_flag(const struct dmx_mgmt *mgmt)
{
    U_DAV_INT reg;

    reg.u32 = DMX_READ_REG_DAV(mgmt->io_base, DAV_INT_ALL_INT_STATUS);

    reg.bits.timeout_int_flg = 0;
    reg.bits.ts_buf_int_flg = 0;
    reg.bits.seop_int_flg = 0;
    reg.bits.prs_ovfl_int_flg = 1;

    DMX_WRITE_REG_DAV(mgmt->io_base, DAV_INT_ALL_INT_STATUS, reg.u32);
}

/* demux port hal level functions begin. */
hi_void dmx_hal_dvb_port_set_share_clk(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id, dmx_port share_clk_port)
{
    hi_u32 tsi_group_index;
    U_PERI_CRG207 peri_crg207;

    volatile hi_reg_crg *peri_crg = hi_drv_sys_get_crg_reg_ptr();
    DMX_NULL_POINTER_VOID(peri_crg);

    WARN_ON(dvb_port_id >= mgmt->tsi_port_cnt);

    tsi_group_index = dvb_port_id / 4; /* 4 tsi every group */

    peri_crg207.u32 = peri_crg->PERI_CRG207.u32;

    if (tsi_group_index == 0) { /* 0 means first group */
        /* 1 means tsi0 tsi1 tsi3 share the tsi2 clock and 0 means tsi0 tsi1 tsi3 use their own clock */
        peri_crg207.bits.pvr_tsi0_sel = (share_clk_port == DMX_TSI_PORT_2) ? 1 : 0;
    } else if (tsi_group_index == 1) { /* 1 means second group */
        /* 1 means tsi4 tsi5 tsi7 share the tsi6 clock and 0 means tsi4 tsi5 tsi7 use their own clock */
        peri_crg207.bits.pvr_tsi1_sel = (share_clk_port == DMX_TSI_PORT_6) ? 1 : 0;
    } else if (tsi_group_index == 2) { /* 2 means third group */
        /* 1 means tsi8 tsi9 tsi11 share the tsi10 clock and 0 means tsi8 tsi9 tsi11 use their own clock */
        peri_crg207.bits.pvr_tsi2_sel = (share_clk_port == DMX_TSI_PORT_10) ? 1 : 0;
    } else if (tsi_group_index == 3) { /* 3 means forth group */
        /* 1 means tsi12 tsi13 tsi15 share the tsi14 clock and 0 means tsi12 tsi13 tsi15 use their own clock */
        peri_crg207.bits.pvr_tsi3_sel = (share_clk_port == DMX_TSI_PORT_14) ? 1 : 0;
    } else {
        HI_ERR_DEMUX("Invalid raw tsi port[dvb_port_id]", dvb_port_id);
        return;
    }

    peri_crg->PERI_CRG207.u32 = peri_crg207.u32;

    osal_mb();

    DMX_UNUSED(mgmt);
    DMX_UNUSED(dvb_port_id);
}

hi_void dmx_hal_dvb_port_open(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id)
{
    U_DVB_INF_CTRL dvb_interface;

    WARN_ON(dvb_port_id >= mgmt->tsi_port_cnt);

    dvb_interface.u32 = DMX_READ_REG_INF(mgmt->io_base, DVB_INF_CTRL(dvb_port_id));

    dvb_interface.bits.port_open = 1;
    DMX_WRITE_REG_INF(mgmt->io_base, DVB_INF_CTRL(dvb_port_id), dvb_interface.u32);

    DMX_COM_EQUAL(dvb_interface.u32, DMX_READ_REG_INF(mgmt->io_base, DVB_INF_CTRL(dvb_port_id)));
}

hi_void dmx_hal_dvb_port_close(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id)
{
    U_DVB_INF_CTRL dvb_interface;

    WARN_ON(dvb_port_id >= mgmt->tsi_port_cnt);

    dvb_interface.u32 = DMX_READ_REG_INF(mgmt->io_base, DVB_INF_CTRL(dvb_port_id));

    dvb_interface.bits.port_open = 0;
    DMX_WRITE_REG_INF(mgmt->io_base, DVB_INF_CTRL(dvb_port_id), dvb_interface.u32);

    DMX_COM_EQUAL(dvb_interface.u32, DMX_READ_REG_INF(mgmt->io_base, DVB_INF_CTRL(dvb_port_id)));

    osal_udelay(10); /* 10 us */
}

hi_void dmx_hal_dvb_port_set_inf_ctrl(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id, hi_bool is_serial,
    dmx_sync_mode sync_mode, hi_bool force_valid, hi_bool clk_mode)
{
    U_DVB_INF_CTRL dvb_interface;

    WARN_ON(dvb_port_id >= mgmt->tsi_port_cnt);

    dvb_interface.u32 = DMX_READ_REG_INF(mgmt->io_base, DVB_INF_CTRL(dvb_port_id));

    dvb_interface.bits.sw_dvb_clk_mode = (clk_mode == HI_TRUE) ? 1 : 0;
    dvb_interface.bits.ser_par_sel = (is_serial == HI_TRUE) ? 1 : 0;
    dvb_interface.bits.sync_mode = sync_mode;
    dvb_interface.bits.sw_valid_burst = (force_valid == HI_TRUE) ? 1 : 0;

    DMX_WRITE_REG_INF(mgmt->io_base, DVB_INF_CTRL(dvb_port_id), dvb_interface.u32);

    DMX_COM_EQUAL(dvb_interface.u32, DMX_READ_REG_INF(mgmt->io_base, DVB_INF_CTRL(dvb_port_id)));
}

hi_void dmx_hal_dvb_port_set_inf_sub_ctrl(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id, hi_u32 fifo_rate,
    hi_u32 sync_on, hi_u32 sync_off)
{
    U_DVB_INF_SUB_CTRL dvb_sub_ctrl;

    WARN_ON(dvb_port_id >= mgmt->tsi_port_cnt);

    dvb_sub_ctrl.u32 = DMX_READ_REG_INF(mgmt->io_base, DVB_INF_SUB_CTRL(dvb_port_id));

    if (fifo_rate == 0) {
        dvb_sub_ctrl.bits.fifo_rate_en = 0;
    } else {
        dvb_sub_ctrl.bits.fifo_rate_en = 1;
        dvb_sub_ctrl.bits.fifo_rate = fifo_rate;
    }

    dvb_sub_ctrl.bits.sync_on_th = sync_on;
    dvb_sub_ctrl.bits.sync_off_th = sync_off;

    DMX_WRITE_REG_INF(mgmt->io_base, DVB_INF_SUB_CTRL(dvb_port_id), dvb_sub_ctrl.u32);

    DMX_COM_EQUAL(dvb_sub_ctrl.u32, DMX_READ_REG_INF(mgmt->io_base, DVB_INF_SUB_CTRL(dvb_port_id)));
}

hi_void dmx_hal_dvb_port_set_serial_ctrl(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id,
    dmx_serial_bit_mode bit_mode, hi_u32 bit_select)
{
    U_DVB_SERIAL_CTRL  dvb_ser_ctrl;
    dmx_bit_select ser_bit_select0 = DMX_BIT_SELECT_0;
    dmx_bit_select ser_bit_select1 = DMX_BIT_SELECT_0;

    WARN_ON(dvb_port_id >= mgmt->tsi_port_cnt);

    if (bit_select == 0) {
        ser_bit_select0 = DMX_BIT_SELECT_7; /* data[7] */
        ser_bit_select1 = DMX_BIT_SELECT_6; /* data[6] */
    } else {
        ser_bit_select0 = DMX_BIT_SELECT_0; /* data[0] */
        ser_bit_select1 = DMX_BIT_SELECT_1; /* data[1] */
    }

    dvb_ser_ctrl.u32 = DMX_READ_REG_INF(mgmt->io_base, DVB_SERIAL_CTRL(dvb_port_id));

    if ((bit_mode & DMX_SERIAL_BIT_MASK) == DMX_SERIAL_1BIT) {
        dvb_ser_ctrl.bits.ser_bit_mode = 0;
        dvb_ser_ctrl.bits.ser_bit0_sel = ser_bit_select0;
    } else if ((bit_mode & DMX_SERIAL_BIT_MASK) == DMX_SERIAL_2BIT) {
        dvb_ser_ctrl.bits.ser_bit_mode = 1;
        dvb_ser_ctrl.bits.ser_bit0_sel = ser_bit_select0;
        dvb_ser_ctrl.bits.ser_bit1_sel = ser_bit_select1;
    } else {
        HI_ERR_DEMUX("invalid bit_mode [%#x]", bit_mode);
    }

    DMX_WRITE_REG_INF(mgmt->io_base, DVB_SERIAL_CTRL(dvb_port_id), dvb_ser_ctrl.u32);

    DMX_COM_EQUAL(dvb_ser_ctrl.u32, DMX_READ_REG_INF(mgmt->io_base, DVB_SERIAL_CTRL(dvb_port_id)));
}

hi_void dmx_hal_dvb_port_set_parallel_ctrl(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id, hi_u32 bit_select)
{
    U_DVB_PARALLEL_CTRL dvb_para_ctrl;

    WARN_ON(dvb_port_id >= mgmt->tsi_port_cnt);

    dvb_para_ctrl.u32 = DMX_READ_REG_INF(mgmt->io_base, DVB_PARALLEL_CTRL(dvb_port_id));

    if (bit_select == 0) {
        dvb_para_ctrl.bits.parallel_msb_sel = 0; /* data[7] as msb */
    } else {
        dvb_para_ctrl.bits.parallel_msb_sel = 1; /* data[0] as msb */
    }

    DMX_WRITE_REG_INF(mgmt->io_base, DVB_PARALLEL_CTRL(dvb_port_id), dvb_para_ctrl.u32);

    DMX_COM_EQUAL(dvb_para_ctrl.u32, DMX_READ_REG_INF(mgmt->io_base, DVB_PARALLEL_CTRL(dvb_port_id)));
}

hi_void dmx_hal_dvb_port_set_clk_in_pol(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id, hi_bool tuner_in_clk)
{
    U_PERI_CRG205 peri_crg205;

    volatile hi_reg_crg *peri_crg = hi_drv_sys_get_crg_reg_ptr();
    DMX_NULL_POINTER_VOID(peri_crg);

    WARN_ON(dvb_port_id >= mgmt->tsi_port_cnt);

    peri_crg205.u32 = peri_crg->PERI_CRG205.u32;

    if (tuner_in_clk == HI_TRUE) {
        peri_crg205.u32 |= ((1 << 16U) << dvb_port_id);
    } else {
        peri_crg205.u32 &= ~((1 << 16U) << dvb_port_id);
    }
    peri_crg->PERI_CRG205.u32 = peri_crg205.u32;

    osal_mb();

    DMX_UNUSED(mgmt);
}

/* ctrl_value: 0: reset, 1: enable, 2:disable */
hi_void dmx_hal_dvb_set_ts_cnt_ctrl(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id, hi_u32 ctrl_value)
{
    U_TS_COUNT_CTRL ts_cnt_ctrl;

    /* ctrl_value: 0: reset, 1: enable, 2:disable */
    DMX_FATAL_CON_VOID_RETURN(ctrl_value > 2);

    WARN_ON(dvb_port_id >= mgmt->tsi_port_cnt);

    ts_cnt_ctrl.u32 = DMX_READ_REG_INF(mgmt->io_base, TS_COUNT_CTRL(dvb_port_id));

    ts_cnt_ctrl.bits.ts_count_ctrl = ctrl_value;

    DMX_WRITE_REG_INF(mgmt->io_base, TS_COUNT_CTRL(dvb_port_id), ts_cnt_ctrl.u32);

    DMX_COM_EQUAL(ts_cnt_ctrl.u32, DMX_READ_REG_INF(mgmt->io_base, TS_COUNT_CTRL(dvb_port_id)));
}

hi_u32 dmx_hal_dvb_get_ts_cnt(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id)
{
    WARN_ON(dvb_port_id >= mgmt->tsi_port_cnt);

    return DMX_READ_REG_INF(mgmt->io_base, TS_COUNT(dvb_port_id));
}

hi_u32 dmx_hal_dvb_get_err_ts_cnt(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id)
{
    WARN_ON(dvb_port_id >= mgmt->tsi_port_cnt);

    return DMX_READ_REG_INF(mgmt->io_base, ETS_COUNT(dvb_port_id));
}

hi_u32 dmx_hal_dvb_get_sync_err_cnt(const struct dmx_mgmt *mgmt, hi_u32 dvb_port_id)
{
    U_TS_AFIFO_WFULL_STATUS reg;

    WARN_ON(dvb_port_id >= mgmt->tsi_port_cnt);

    reg.bits.sync_err_cnt = DMX_READ_REG_INF(mgmt->io_base, TS_AFIFO_WFULL_STATUS(dvb_port_id));

    return reg.bits.sync_err_cnt;
}

hi_void dmx_hal_dvb_port_set_sync_len(const struct dmx_mgmt *mgmt, hi_u32 tsi_raw_id, hi_u32 tag_len)
{
    U_DVB_SYNC_CTRL0 sync_ctrl0;
    U_DVB_SYNC_CTRL1 sync_ctrl1;

    WARN_ON(tsi_raw_id >= mgmt->tsi_port_cnt);

    sync_ctrl0.u32 = DMX_READ_REG_SWH(mgmt->io_base, DVB_SYNC_CTRL0(tsi_raw_id));
    sync_ctrl0.bits.nosync_188_len = DMX_TS_PKT_SIZE + tag_len;
    sync_ctrl0.bits.nosync_204_len = DMX_TS_FEC_PKT_SIZE + tag_len;
    DMX_WRITE_REG_SWH(mgmt->io_base, DVB_SYNC_CTRL0(tsi_raw_id), sync_ctrl0.u32);

    sync_ctrl1.u32 = DMX_READ_REG_SWH(mgmt->io_base, DVB_SYNC_CTRL1(tsi_raw_id));

    if (tag_len == 0) {
        sync_ctrl1.bits.sw_sync_out_nolimit = 0; /* cut off as 188 */
    } else {
        sync_ctrl1.bits.sw_sync_out_nolimit = 1; /* don't cut off as 188 */
    }

    DMX_WRITE_REG_SWH(mgmt->io_base, DVB_SYNC_CTRL1(tsi_raw_id), sync_ctrl1.u32);
}

hi_void dmx_hal_dvb_port_set_dummy_force(const struct dmx_mgmt *mgmt, hi_u32 tsi_raw_id, hi_bool dummy_force)
{
    U_DVB_DUMMY_CTRL dummy_ctrl;

    WARN_ON(tsi_raw_id >= mgmt->tsi_port_cnt);

    dummy_ctrl.u32 = DMX_READ_REG_INF(mgmt->io_base, DVB_DUMMY_CTRL(tsi_raw_id));

    dummy_ctrl.bits.sw_dummy_force = ((dummy_force == HI_TRUE) ? 1 : 0);

    DMX_WRITE_REG_INF(mgmt->io_base, DVB_DUMMY_CTRL(tsi_raw_id), dummy_ctrl.u32);

    DMX_COM_EQUAL(dummy_ctrl.u32, DMX_READ_REG_INF(mgmt->io_base, DVB_DUMMY_CTRL(tsi_raw_id)));
}

hi_void dmx_hal_dvb_port_set_dummy_sync(const struct dmx_mgmt *mgmt, hi_u32 tsi_raw_id, hi_bool dummy_sync)
{
    U_DVB_DUMMY_CTRL dummy_ctrl;

    WARN_ON(tsi_raw_id >= mgmt->tsi_port_cnt);

    dummy_ctrl.u32 = DMX_READ_REG_INF(mgmt->io_base, DVB_DUMMY_CTRL(tsi_raw_id));

    dummy_ctrl.bits.sw_dummy_sync = ((dummy_sync == HI_TRUE) ? 1 : 0);

    DMX_WRITE_REG_INF(mgmt->io_base, DVB_DUMMY_CTRL(tsi_raw_id), dummy_ctrl.u32);

    DMX_COM_EQUAL(dummy_ctrl.u32, DMX_READ_REG_INF(mgmt->io_base, DVB_DUMMY_CTRL(tsi_raw_id)));
}

/* tag port function */
hi_void dmx_hal_tag_port_set_attr(const struct dmx_mgmt *mgmt, hi_u32 tag_dual_id, hi_u32 tag_idx,
    const dmx_tag_value *tag_value)
{
    WARN_ON(tag_dual_id >= DMX_TAG_DUAL_CNT);
    WARN_ON(tag_idx >= DMX_TAG_OUTPUT_CNT);

    DMX_WRITE_REG_SWH(mgmt->io_base, TSN_TAG_LOW_REG(tag_dual_id, tag_idx), tag_value->low);
    DMX_WRITE_REG_SWH(mgmt->io_base, TSN_TAG_MID_REG(tag_dual_id, tag_idx), tag_value->mid);
    DMX_WRITE_REG_SWH(mgmt->io_base, TSN_TAG_HIGH_REG(tag_dual_id, tag_idx), tag_value->high);
}

hi_void dmx_hal_tag_port_set_ctrl(const struct dmx_mgmt *mgmt, hi_u32 tag_dual_id,
    const dmx_tag_dual_ctrl *tag_dual_ctrl)
{
    U_TAG_MUX_SOURCE mux_source;
    U_TAG_MUX_CONFIG mux_config;

    WARN_ON(tag_dual_id >= DMX_TAG_DUAL_CNT);

    mux_source.u32 = DMX_READ_REG_SWH(mgmt->io_base, TAG_MUX_SOURCE(tag_dual_id));
    if (tag_dual_ctrl->enable == HI_TRUE) {
        mux_source.bits.tag_deal_mux0_num = tag_dual_ctrl->tsi_raw_id;
        mux_source.bits.tag_deal_mux0_type = 1; /* enable dvb */
    } else {
        mux_source.bits.tag_deal_mux0_num = 0;
        mux_source.bits.tag_deal_mux0_type = 0; /* disable dvb */
    }
    DMX_WRITE_REG_SWH(mgmt->io_base, TAG_MUX_SOURCE(tag_dual_id), mux_source.u32);

    mux_config.u32 = DMX_READ_REG_SWH(mgmt->io_base, TAG_MUX_CONFIG(tag_dual_id));
    mux_config.bits.mux_work_en = tag_dual_ctrl->enable;
    mux_config.bits.sync_mode_sel = tag_dual_ctrl->sync_mode;
    mux_config.bits.tag_length = tag_dual_ctrl->tag_len;
    DMX_WRITE_REG_SWH(mgmt->io_base, TAG_MUX_CONFIG(tag_dual_id), mux_config.u32);
}

hi_void dmx_hal_tag_port_set_clk_gt(const struct dmx_mgmt *mgmt, hi_u32 tag_dual_id, hi_bool clk_gt_en)
{
    U_TAG_CLK_GT_CTRL tag_clk_gt;

    WARN_ON(tag_dual_id >= DMX_TAG_DUAL_CNT);

    tag_clk_gt.u32 = DMX_READ_REG_SWH(mgmt->io_base, TAG_CLK_GT_CTRL(tag_dual_id));
    tag_clk_gt.bits.tag_clk_gt_en = (clk_gt_en == HI_TRUE) ? 1 : 0;
    DMX_WRITE_REG_SWH(mgmt->io_base, TAG_CLK_GT_CTRL(tag_dual_id), tag_clk_gt.u32);
}

/*
 * DEMUX RAM port hal level functions begin.
 */
/***********************************************************************************
* Function      : DmxHalRamPortGetDscRdIntStatus
* Description   : get the dsc read finish interrupt
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_ram_port_get_dsc_rd_int_status(const struct dmx_mgmt *mgmt)
{
    U_DESC_RD_INT reg;

    reg.u32 = DMX_READ_REG_RAM(mgmt->io_base, IP_DESC_RD_INT);

    return reg.u32;
}

/***********************************************************************************
* Function      : DmxHalRamPortClsDscRdIntStatus
* Description   : clear the Dsc read finish interrupt
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_ram_port_cls_dsc_rd_int_status(const struct dmx_mgmt *mgmt, hi_u32 status)
{
    U_RAW_DESC_RD_INT reg_clear;
    U_RAW_DESC_RD_INT reg_clearend;

    reg_clear.u32 = status;

    reg_clearend.u32 = reg_clear.u32 & ~(status);

    DMX_WRITE_REG_RAM(mgmt->io_base, IP_RAW_DESC_RD_INT, reg_clear.u32);
}

/***********************************************************************************
* Function      : DmxHalRamPortGetDscTreadIntStatus
* Description   : get the dsc read finish interrupt
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_ram_port_get_dsc_tread_int_status(const struct dmx_mgmt *mgmt)
{
    U_DESC_RD_INT reg;

    reg.u32 = DMX_READ_REG_RAM(mgmt->io_base, IP_TREAD_INT);

    return reg.u32;
}

/***********************************************************************************
* Function      : DmxHalRamPortClsDscTreadIntStatus
* Description   : clear the Dsc read finish interrupt
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_ram_port_cls_dsc_tread_int_status(const struct dmx_mgmt *mgmt, hi_u32 status)
{
    U_RAW_DESC_RD_INT reg_clear;
    U_RAW_DESC_RD_INT reg_clearend;

    reg_clear.u32 = status;

    reg_clearend.u32 = reg_clear.u32 & ~(status);

    DMX_WRITE_REG_RAM(mgmt->io_base, IP_RAW_IP_TREAD_INT, reg_clear.u32);
    DMX_COM_EQUAL(reg_clearend.u32, DMX_READ_REG_RAM(mgmt->io_base, IP_RAW_IP_TREAD_INT));
}

hi_u32 dmx_hal_ram_port_get_ip_loss_int_status(const struct dmx_mgmt *mgmt)
{
    return DMX_READ_REG_RAM(mgmt->io_base, IP_LOSS_INT);
}

hi_void dmx_hal_ram_port_cls_ip_loss_int_status(const struct dmx_mgmt *mgmt, hi_u32 status)
{
    hi_u32 reg_clear, reg_clearend;

    reg_clear = status;
    reg_clearend = reg_clear & ~(status);

    DMX_WRITE_REG_RAM(mgmt->io_base, RAW_IP_LOSS_INT, reg_clear);
    DMX_COM_EQUAL(reg_clearend, DMX_READ_REG_RAM(mgmt->io_base, RAW_IP_LOSS_INT));
}

/***********************************************************************************
* Function      : DmxHalRamPortSetDesc
* Description   : Set ram port descriptors
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
static hi_void __dmx_hal_ram_port_set_desc(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 dsc_phy_addr,
                                           hi_u32 dsc_depth)
{
    U_IP_FQ_BUF ip_fq_buf;
    U_IP_FQ_SESSION ip_fq_session;

    WARN_ON(id == DMX_INVALID_PORT_ID);

    ip_fq_buf.u32 = DMX_READ_REG_RAM(mgmt->io_base, IP_FQ_BUF(id));

    ip_fq_buf.bits.ip_fqsa = (dsc_phy_addr & 0xFFFFFFFF) >> 12; /* 4k align, shift 12 bits  */
    ip_fq_buf.bits.ip_fqsize = dsc_depth - 1;   /* hw rule: -1. */
    DMX_WRITE_REG_RAM(mgmt->io_base, IP_FQ_BUF(id), ip_fq_buf.u32);

    DMX_COM_EQUAL(ip_fq_buf.u32, DMX_READ_REG_RAM(mgmt->io_base, IP_FQ_BUF(id)));

    ip_fq_session.u32 = DMX_READ_REG_RAM(mgmt->io_base, IP_FQ_SESSION(id));
    ip_fq_session.bits.ip_fq_session = (dsc_phy_addr >> 32) & 0xF; /* shift 32 bits  */
    DMX_WRITE_REG_RAM(mgmt->io_base, IP_FQ_SESSION(id), ip_fq_session.u32);

    DMX_COM_EQUAL(ip_fq_session.u32, DMX_READ_REG_RAM(mgmt->io_base, IP_FQ_SESSION(id)));
}

hi_void dmx_hal_ram_port_set_desc(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 dsc_phy_addr, hi_u32 dsc_depth)
{
    WARN_ON(id >= mgmt->ram_port_cnt);
    return __dmx_hal_ram_port_set_desc(mgmt, id, dsc_phy_addr, dsc_depth);
}


/***********************************************************************************
* Function      : DmxHalRamPortSetDscRdInt
* Description   : Set desc_out int enale
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
static hi_void __dmx_hal_ram_port_set_dsc_rd_int(const struct dmx_mgmt *mgmt, hi_u32 id, const hi_bool enable)
{
    U_IP_FQ_BUF ip_fq_buf;

    WARN_ON(id == DMX_INVALID_PORT_ID);

    ip_fq_buf.u32 = DMX_READ_REG_RAM(mgmt->io_base, IP_FQ_BUF(id));

    ip_fq_buf.bits.ip_ip_rd_int_en = enable ? 1 : 0;         /* enable the read finish interrupt */
    DMX_WRITE_REG_RAM(mgmt->io_base, IP_FQ_BUF(id), ip_fq_buf.u32);

    DMX_COM_EQUAL(ip_fq_buf.u32, DMX_READ_REG_RAM(mgmt->io_base, IP_FQ_BUF(id)));
}

hi_void dmx_hal_ram_port_set_dsc_rd_int(const struct dmx_mgmt *mgmt, hi_u32 id, const hi_bool enable)
{
    WARN_ON(id >= mgmt->ram_port_cnt);
    return __dmx_hal_ram_port_set_dsc_rd_int(mgmt, id, enable);
}

/***********************************************************************************
* Function      : DmxHalRamPortSetTreadInt
* Description   : Set tread int enale
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
static hi_void __dmx_hal_ram_port_set_tread_int(const struct dmx_mgmt *mgmt, hi_u32 id, const hi_bool enable)
{
    U_IP_FQ_BUF ip_fq_buf;

    WARN_ON(id == DMX_INVALID_PORT_ID);

    ip_fq_buf.u32 = DMX_READ_REG_RAM(mgmt->io_base, IP_FQ_BUF(id));
    ip_fq_buf.bits.ip_tread_int_en = enable ? 1 : 0;      /* enable the dsc tread interrupt */
    DMX_WRITE_REG_RAM(mgmt->io_base, IP_FQ_BUF(id), ip_fq_buf.u32);

    DMX_COM_EQUAL(ip_fq_buf.u32, DMX_READ_REG_RAM(mgmt->io_base, IP_FQ_BUF(id)));
}

hi_void dmx_hal_ram_port_set_tread_int(const struct dmx_mgmt *mgmt, hi_u32 id, const hi_bool enable)
{
    WARN_ON(id >= mgmt->ram_port_cnt);
    return __dmx_hal_ram_port_set_tread_int(mgmt, id, enable);
}

/***********************************************************************************
* Function      : DmxHalRamPortConfigRate
* Description   : Confiugre Ram Port Rate
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_ram_port_config_rate(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rate)
{
    hi_u32 bete;
    WARN_ON(rate == 0);
    WARN_ON(id >= mgmt->ram_port_cnt);

    /* 1000 means Mbps to Kbps, 8 means 8 bits per byte */
    bete = DMX_DEFAULT_CLOCK_MHZ * 1000 * 8 / rate - 1; /* rate:kbps */

    DMX_WRITE_REG_RAM(mgmt->io_base, IP_RATE_CFG(id), bete);
    DMX_COM_EQUAL(bete, DMX_READ_REG_RAM(mgmt->io_base, IP_RATE_CFG(id)));
}

/***********************************************************************************
* Function      : DmxHalRamPortEnableMmu
* Description   : Enable Ram Port Mmu
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
static hi_void __dmx_hal_ram_port_enable_mmu(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    /* to be add */
}

hi_void dmx_hal_ram_port_enable_mmu(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    WARN_ON(id >= mgmt->ram_port_cnt);
    return __dmx_hal_ram_port_enable_mmu(mgmt, id);
}

/***********************************************************************************
* Function      : DmxHalRamPortStartStream
* Description   : Start pushing stream
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
static hi_void __dmx_hal_ram_port_start_stream(const struct dmx_mgmt *mgmt, hi_u32 id, const hi_bool enable)
{
    U_IP_RX_EN iprx_en;

    WARN_ON(id == DMX_INVALID_PORT_ID);

    iprx_en.u32 = DMX_READ_REG_RAM(mgmt->io_base, IP_RX_EN(id));

    iprx_en.bits.ip_rx_en = enable ? 1 : 0;
    DMX_WRITE_REG_RAM(mgmt->io_base, IP_RX_EN(id), iprx_en.u32);

    DMX_COM_EQUAL(iprx_en.u32, DMX_READ_REG_RAM(mgmt->io_base, IP_RX_EN(id)));
}

hi_void dmx_hal_ram_port_start_stream(const struct dmx_mgmt *mgmt, hi_u32 id, const hi_bool enable)
{
    WARN_ON(id >= mgmt->ram_port_cnt);
    return __dmx_hal_ram_port_start_stream(mgmt, id, enable);
}

hi_void dmx_hal_ram_port_set_dsc(const struct dmx_mgmt *mgmt, hi_u32 id, struct dmx_ram_port_dsc *dsc)
{
    U_RAM_DSC_WORD_1 reg;
    hi_u32 *cur_dsc_addr = dsc->cur_dsc_addr;

    WARN_ON(id >= mgmt->ram_port_cnt);

    reg.u32 = 0;
    reg.bits.iplength = dsc->buf_len;
    reg.bits.desep = (dsc->desep ? 1 : 0);            /* interrupt whe finish read the dsc */
    reg.bits.flush = (dsc->flush_flag ? 1 : 0);        /* flush flag */
    reg.bits.syncdata = (dsc->sync_data_flag ? 1 : 0);  /* sync 16 ts package flag */
    reg.bits.session = (dsc->buf_phy_addr >> 32) & 0xF; /* high 4 bit */
    reg.bits.check_data = RAM_DSC_GUIDE_NUMBER;   /* fixed guide number */

    HI_DBG_DEMUX("**cur_dsc_addr[0] is [0x%x], cur_dsc_addr[1] is [0x%x]\n", dsc->buf_phy_addr, reg.u32);
    *cur_dsc_addr++ = (hi_u32)(dsc->buf_phy_addr & 0xFFFFFFFF); /* RAM port dsc word 0, 32 bit */
    *cur_dsc_addr++ = reg.u32;                    /* RAM port dsc word 1 */

    osal_mb();
}

hi_void dmx_hal_ram_port_add_dsc(const struct dmx_mgmt *mgmt, const hi_u32 id, const hi_u32 desc_num)
{
    U_IP_DESC_ADD ipdesc_add;
    U_IP_DESC_ADD ipdesc_addend;

    WARN_ON(id >= mgmt->ram_port_cnt);

    ipdesc_add.u32 = DMX_READ_REG_RAM(mgmt->io_base, IP_DESC_ADD(id));

    ipdesc_add.bits.desc_add = desc_num;
    ipdesc_add.bits.desc_addvld = HI_TRUE;

    DMX_WRITE_REG_RAM(mgmt->io_base, IP_DESC_ADD(id), ipdesc_add.u32);

    ipdesc_addend.u32 = ipdesc_add.u32;
    ipdesc_addend.bits.desc_addvld = HI_FALSE;
    DMX_COM_EQUAL(ipdesc_addend.u32, DMX_READ_REG_RAM(mgmt->io_base, IP_DESC_ADD(id)));
}

/***********************************************************************************
* Function      : DmxHalRamPortGetCurReadIdx
* Description   : Get ram port current Read index
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_ram_port_get_cur_read_idx(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_IP_FQ_PTR ipfq_ptr;

    WARN_ON(id >= mgmt->ram_port_cnt);

    ipfq_ptr.u32 = DMX_READ_REG_RAM(mgmt->io_base, IP_FQ_PTR(id));

    return ipfq_ptr.bits.ip_fq_rptr;
}

/***********************************************************************************
* Function      : DmxHalRamPortGetCurWriteIdx
* Description   : Get ram port current Write index
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_ram_port_get_cur_write_idx(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_IP_FQ_PTR ipfq_ptr;

    WARN_ON(id >= mgmt->ram_port_cnt);

    ipfq_ptr.u32 = DMX_READ_REG_RAM(mgmt->io_base, IP_FQ_PTR(id));

    return ipfq_ptr.bits.ip_fq_wptr;
}

/***********************************************************************************
* Function      : DmxHalRamPortEnabled
* Description   : judge the Ramport Enabled or not
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_bool dmx_hal_ram_port_enabled(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_IP_RX_EN iprx_en;

    WARN_ON(id >= mgmt->ram_port_cnt);

    iprx_en.u32 = DMX_READ_REG_RAM(mgmt->io_base, IP_RX_EN(id));

    return ((iprx_en.bits.ip_rx_en == 1) ? HI_TRUE : HI_FALSE);
}

/***********************************************************************************
* Function      : DmxHalRamPortEnabled
* Description   : set the checkcode when init the demux
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_ram_port_set_check_code(const struct dmx_mgmt *mgmt)
{
    U_IP_FQ_CHECK_CODE ipfq_checkcode;

    ipfq_checkcode.u32 = DMX_READ_REG_RAM(mgmt->io_base, IP_FQ_CHECK_CODE);
    ipfq_checkcode.bits.ip_fq_check_code = RAM_DSC_GUIDE_NUMBER;

    DMX_WRITE_REG_RAM(mgmt->io_base, IP_FQ_CHECK_CODE, ipfq_checkcode.u32);

    DMX_COM_EQUAL(ipfq_checkcode.u32, DMX_READ_REG_RAM(mgmt->io_base, IP_FQ_CHECK_CODE));
}

/******************************************************************************
* Function      :  DmxHalRamPortClrPort
* Description   :  Clear the Ram Port
* Input         : PortId
* Output        :
* Return        :
* Others:       :
*******************************************************************************/
hi_void dmx_hal_ram_port_clr_port(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    hi_s32 i = 0;
    hi_u32 reg;

    WARN_ON(id >= mgmt->ram_port_cnt);

    reg = DMX_READ_REG_RAM(mgmt->io_base, IP_CLR_CHN_REQ);

    reg = 1U << id;

    DMX_WRITE_REG_RAM(mgmt->io_base, IP_CLR_CHN_REQ, reg);

    reg = DMX_READ_REG_RAM(mgmt->io_base, RAW_IP_CLR_INT);
    while (((reg & (1U << id)) != (1U << id)) && (i++ <= DMX_CHAN_CLEAR_TIMEOUT_CNT)) {
        msleep(1);
        reg = DMX_READ_REG_RAM(mgmt->io_base, RAW_IP_CLR_INT);
    }

    if (i >= DMX_CHAN_CLEAR_TIMEOUT_CNT) {
        HI_ERR_DEMUX("ramport(%u) chan clear time out.\n", id);
    }

    reg = (1U << id);
    DMX_WRITE_REG_RAM(mgmt->io_base, RAW_IP_CLR_INT, reg);
}

/******************************************************************************
* Function      :  DmxHalRamPortSetSyncLen
* Description   :  Set the sync len
* Input         : PortId
* Output        :
* Return        :
* Others:       :
*******************************************************************************/
hi_void dmx_hal_ram_port_set_sync_len(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 ram_len1, hi_u32 ram_len2)
{
    U_IP_SYNC_LEN ip_sync_len;

    WARN_ON(id >= mgmt->ram_port_cnt);

    ip_sync_len.u32 = DMX_READ_REG_RAM(mgmt->io_base, IP_SYNC_LEN(id));
    ip_sync_len.bits.ip_nosync_region = DMX_RAM_AUTO_REGION;
    ip_sync_len.bits.ip_nosync_step   = DMX_RAM_AUTO_STEP_2;
    ip_sync_len.bits.ip_nosync_188_len =  ram_len1;
    ip_sync_len.bits.ip_nosync_204_len =  ram_len2;

    DMX_WRITE_REG_RAM(mgmt->io_base, IP_SYNC_LEN(id), ip_sync_len.u32);

    /* after clear ramport, hardware set the bit as 0 */
    DMX_COM_EQUAL(ip_sync_len.u32, DMX_READ_REG_RAM(mgmt->io_base, IP_SYNC_LEN(id)));
}

/******************************************************************************
* Function      :  DmxHalRamPortSetIPCfg
* Description   :  Set the IP CFG Register
* Input         : PortId
* Output        :
* Return        :
* Others:       :
*******************************************************************************/
hi_void dmx_hal_ram_port_set_ipcfg(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 sync_type, hi_u32 sync_th,
                                   hi_u32 lost_th, hi_bool sync_inten, hi_bool loss_inten)
{
    U_IP_CFG ip_cfg;

    WARN_ON(id >= mgmt->ram_port_cnt);

    ip_cfg.u32 = DMX_READ_REG_RAM(mgmt->io_base, IP_CFG(id));

    ip_cfg.bits.sync_type = sync_type;
    ip_cfg.bits.sync_th   = sync_th;
    ip_cfg.bits.lost_th   = lost_th;
    ip_cfg.bits.ip_sync_int_en = sync_inten;
    ip_cfg.bits.ip_loss_int_en = loss_inten;

    DMX_WRITE_REG_RAM(mgmt->io_base, IP_CFG(id), ip_cfg.u32);

    /* after clear ramport, hardware set the bit as 0 */
    DMX_COM_EQUAL(ip_cfg.u32, DMX_READ_REG_RAM(mgmt->io_base, IP_CFG(id)));
}

/******************************************************************************
* Function      :  DmxHalRamPortSetVirRamPort
* Description   :  Set the virtual Ramport
* Input         : PortId
* Output        :
* Return        :
* Others:       :
*******************************************************************************/
hi_void dmx_hal_ram_port_set_vir_ram_port(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    hi_u32 reg;

    WARN_ON(id >= mgmt->ram_port_cnt);

    reg = DMX_READ_REG_RAM(mgmt->io_base, VIR_CHN_FLG);

    reg |= 1 << id;

    DMX_WRITE_REG_RAM(mgmt->io_base, VIR_CHN_FLG, reg);

    DMX_COM_EQUAL(reg, DMX_READ_REG_RAM(mgmt->io_base, VIR_CHN_FLG));
}

/******************************************************************************
* Function      :  DmxHalRamPortClsVirRamPort
* Description   :  Clear the virtual Ramport
* Input         : PortId
* Output        :
* Return        :
* Others:       :
*******************************************************************************/
hi_void dmx_hal_ram_port_cls_vir_ram_port(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    hi_u32 reg;

    WARN_ON(id >= mgmt->ram_port_cnt);

    reg = DMX_READ_REG_RAM(mgmt->io_base, VIR_CHN_FLG);

    reg &= ~(1 << id);

    DMX_WRITE_REG_RAM(mgmt->io_base, VIR_CHN_FLG, reg);

    DMX_COM_EQUAL(reg, DMX_READ_REG_RAM(mgmt->io_base, VIR_CHN_FLG));
}

/******************************************************************************
* Function      :  DmxHalRamPortClsApStatus
* Description   :  Clear the ap status of Ramport
* Input         : PortId
* Output        :
* Return        :
* Others:       :
*******************************************************************************/
hi_void dmx_hal_ram_port_cls_ap_status(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    hi_u32 reg;
    hi_u32 reg_clearend;

    WARN_ON(id >= mgmt->ram_port_cnt);

    reg = (1 << id);
    reg_clearend = reg & ~(1 << id);

    DMX_WRITE_REG_RAM(mgmt->io_base, AP_CLEAR2, reg);

    DMX_COM_EQUAL(reg_clearend, DMX_READ_REG_RAM(mgmt->io_base, AP_CLEAR2));
}

/***********************************************************************************
* Function      : DmxHalRamPortConfig
* Description   : Configure RamPort
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_ram_port_config(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 dsc_phy_addr, hi_u32 dsc_depth,
                                hi_u32 rate)
{
    WARN_ON(id >= mgmt->ram_port_cnt);

    dmx_hal_ram_port_set_desc(mgmt, id, dsc_phy_addr, dsc_depth);

    dmx_hal_ram_port_config_rate(mgmt, id, rate);

    /* dsc read finish interrupt enable */
    dmx_hal_ram_port_set_dsc_rd_int(mgmt, id, HI_TRUE);

    /* dsc tread interrupt enable */
    dmx_hal_ram_port_set_tread_int(mgmt, id, HI_TRUE);

    /* set default sync len */
    dmx_hal_ram_port_set_sync_len(mgmt, id, 188, 204); /* ram len 188&204 */

    /* set default ipcfg register, sync_type as 3:auto check, sync_th is 7, lost_th is 0 */
    dmx_hal_ram_port_set_ipcfg(mgmt, id, 3, 7, 0, HI_FALSE, HI_FALSE);

    dmx_hal_ram_port_start_stream(mgmt, id, HI_TRUE);
}

/***********************************************************************************
* Function      : DmxHalRamPortDeConfig
* Description   : Deconfigure RamPort
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_ram_port_de_config(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    WARN_ON(id >= mgmt->ram_port_cnt);

    /* dsc tread interrupt disable */
    dmx_hal_ram_port_set_tread_int(mgmt, id, HI_FALSE);

    /* dsc read finish interrupt disable */
    dmx_hal_ram_port_set_dsc_rd_int(mgmt, id, HI_FALSE);

    dmx_hal_ram_port_start_stream(mgmt, id, HI_FALSE);
}

/* enable the CRC check of input ts packets, and disable the sync and lost check */
hi_void dmx_hal_ram_port_ip_crc_en(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_IP_RX_EN iprx_en;

    WARN_ON(id == DMX_INVALID_PORT_ID);

    iprx_en.bits.ip_crc_en = 1;
    DMX_WRITE_REG_RAM(mgmt->io_base, IP_RX_EN(id), iprx_en.u32);

    DMX_COM_EQUAL(iprx_en.u32, DMX_READ_REG_RAM(mgmt->io_base, IP_RX_EN(id)));
}

hi_u32 dmx_hal_ram_port_get_ts_packets_cnt(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    WARN_ON(id >= mgmt->ram_port_cnt);

    return DMX_READ_REG_RAM(mgmt->io_base, IP_PKT_CNT(id));
}

/* DEMUX RmxFct hal level functions begin. */
hi_void dmx_hal_rmx_set_port(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_src_port_id,
                             enum dmx_port_type_e port_mode, hi_u32 port_id)
{
    U_RMX_SRC_SEL rmx_src_sel;

    WARN_ON(id >= mgmt->rmx_fct_cnt);
    WARN_ON(rmx_src_port_id >= mgmt->rmx_port_cnt);

    rmx_src_sel.u32 = DMX_READ_REG_SWH(mgmt->io_base, RMX_SRC_SEL(id, rmx_src_port_id));

    rmx_src_sel.bits.remux_mux_num  = rmx_src_port_id;
    rmx_src_sel.bits.remux_mux_type = port_mode;

    DMX_WRITE_REG_SWH(mgmt->io_base, RMX_SRC_SEL(id, rmx_src_port_id), rmx_src_sel.u32);

    DMX_COM_EQUAL(rmx_src_sel.u32, DMX_READ_REG_SWH(mgmt->io_base, RMX_SRC_SEL(id, rmx_src_port_id)));
}

hi_void dmx_hal_rmx_unset_port(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_src_port_id)
{
    WARN_ON(id >= mgmt->rmx_fct_cnt);
    WARN_ON(rmx_src_port_id >= mgmt->rmx_port_cnt);

    DMX_WRITE_REG_SWH(mgmt->io_base, RMX_SRC_SEL(id, rmx_src_port_id), 0);

    DMX_COM_EQUAL(0, DMX_READ_REG_SWH(mgmt->io_base, RMX_SRC_SEL(id, rmx_src_port_id)));
}

hi_void dmx_hal_rmx_set_port_all_pass(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_src_port_id)
{
    hi_u32 value;

    WARN_ON(id >= mgmt->rmx_fct_cnt);
    WARN_ON(rmx_src_port_id >= mgmt->rmx_port_cnt);

    value = DMX_READ_REG_SWH(mgmt->io_base, RMX_ALL_PARSS(id));
    value |= (1 << rmx_src_port_id);
    DMX_WRITE_REG_SWH(mgmt->io_base, RMX_ALL_PARSS(id), value);

    DMX_COM_EQUAL(value, DMX_READ_REG_SWH(mgmt->io_base, RMX_ALL_PARSS(id)));
}

hi_void dmx_hal_rmx_un_set_port_all_pass(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_src_port_id)
{
    hi_u32 value;

    WARN_ON(id >= mgmt->rmx_fct_cnt);
    WARN_ON(rmx_src_port_id >= mgmt->rmx_port_cnt);

    value = DMX_READ_REG_SWH(mgmt->io_base, RMX_ALL_PARSS(id));

    value &= ~(1 << rmx_src_port_id);

    DMX_WRITE_REG_SWH(mgmt->io_base, RMX_ALL_PARSS(id), value);

    DMX_COM_EQUAL(value, DMX_READ_REG_SWH(mgmt->io_base, RMX_ALL_PARSS(id)));
}

hi_void dmx_hal_rmx_en_detect_port(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_src_port_id)
{
    U_RMX0_FULL_CNT rmx_full_cnt;

    WARN_ON(id >= mgmt->rmx_fct_cnt);
    WARN_ON(rmx_src_port_id >= mgmt->rmx_port_cnt);

    rmx_full_cnt.u32 = DMX_READ_REG_SWH(mgmt->io_base, RMX0_FULL_CNT(id, rmx_src_port_id));

    rmx_full_cnt.bits.sw_rmx_full_cnten0 = 1;

    DMX_WRITE_REG_SWH(mgmt->io_base, RMX0_FULL_CNT(id, rmx_src_port_id), rmx_full_cnt.u32);

    DMX_COM_EQUAL(rmx_full_cnt.u32, DMX_READ_REG_SWH(mgmt->io_base, RMX0_FULL_CNT(id, rmx_src_port_id)));
}

hi_void dmx_hal_rmx_dis_detect_port(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_src_port_id)
{
    U_RMX0_FULL_CNT rmx_full_cnt;

    WARN_ON(id >= mgmt->rmx_fct_cnt);
    WARN_ON(rmx_src_port_id >= mgmt->rmx_port_cnt);

    rmx_full_cnt.u32 = DMX_READ_REG_SWH(mgmt->io_base, RMX0_FULL_CNT(id, rmx_src_port_id));

    rmx_full_cnt.bits.sw_rmx_full_cnten0 = 0;

    DMX_WRITE_REG_SWH(mgmt->io_base, RMX0_FULL_CNT(id, rmx_src_port_id), rmx_full_cnt.u32);

    DMX_COM_EQUAL(rmx_full_cnt.u32, DMX_READ_REG_SWH(mgmt->io_base, RMX0_FULL_CNT(id, rmx_src_port_id)));
}

hi_u32 dmx_hal_rmx_get_port_overflow_count(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_src_port_id)
{
    U_RMX0_FULL_CNT rmx_full_cnt;

    WARN_ON(id >= mgmt->rmx_fct_cnt);
    WARN_ON(rmx_src_port_id >= mgmt->rmx_port_cnt);

    rmx_full_cnt.u32 = DMX_READ_REG_SWH(mgmt->io_base, RMX0_FULL_CNT(id, rmx_src_port_id));

    return rmx_full_cnt.bits.dbg_full_err_cnt0;
}

hi_void dmx_hal_rmx_set_pid_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 pump_id, hi_u32 rmx_src_port_id,
                                hi_u32 pid)
{
    U_RMX_PID_TABLE0 rmx_pid_tab;

    WARN_ON(id >= mgmt->rmx_fct_cnt);
    WARN_ON(rmx_src_port_id >= mgmt->rmx_port_cnt);
    WARN_ON(pump_id >= mgmt->pump_total_cnt / mgmt->rmx_fct_cnt);

    rmx_pid_tab.u32 = DMX_READ_REG_SWH(mgmt->io_base, RMX_PID_TABLE0(id, pump_id));

    rmx_pid_tab.bits.original_pid = pid;
    rmx_pid_tab.bits.remap_source_id = rmx_src_port_id;
    rmx_pid_tab.bits.pidtab_en = 1;

    DMX_WRITE_REG_SWH(mgmt->io_base, RMX_PID_TABLE0(id, pump_id), rmx_pid_tab.u32);

    DMX_COM_EQUAL(rmx_pid_tab.u32, DMX_READ_REG_SWH(mgmt->io_base, RMX_PID_TABLE0(id, pump_id)));
}

hi_void dmx_hal_rmx_set_remap_pid_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 pump_id, hi_u32 rmx_src_port_id,
                                      hi_u32 pid, hi_u32 new_pid)
{
    U_RMX_PID_TABLE0 rmx_pid_tab;

    WARN_ON(id >= mgmt->rmx_fct_cnt);
    WARN_ON(rmx_src_port_id >= mgmt->rmx_port_cnt);
    WARN_ON(pump_id >= mgmt->pump_total_cnt / mgmt->rmx_fct_cnt);

    rmx_pid_tab.u32 = DMX_READ_REG_SWH(mgmt->io_base, RMX_PID_TABLE0(id, pump_id));

    rmx_pid_tab.bits.original_pid = pid;
    rmx_pid_tab.bits.remap_source_id = rmx_src_port_id;
    rmx_pid_tab.bits.pid_remap_pid = new_pid;
    rmx_pid_tab.bits.pidtab_en = 1;
    rmx_pid_tab.bits.pid_remap_mode = 1;

    DMX_WRITE_REG_SWH(mgmt->io_base, RMX_PID_TABLE0(id, pump_id), rmx_pid_tab.u32);

    DMX_COM_EQUAL(rmx_pid_tab.u32, DMX_READ_REG_SWH(mgmt->io_base, RMX_PID_TABLE0(id, pump_id)));
}

hi_void dmx_hal_rmx_un_set_pid_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 pump_id)
{
    WARN_ON(id >= mgmt->rmx_fct_cnt);
    WARN_ON(pump_id >= mgmt->pump_total_cnt / mgmt->rmx_fct_cnt);

    DMX_WRITE_REG_SWH(mgmt->io_base, RMX_PID_TABLE0(id, pump_id), 0);

    DMX_COM_EQUAL(0, DMX_READ_REG_SWH(mgmt->io_base, RMX_PID_TABLE0(id, pump_id)));
}

hi_void dmx_hal_rmx_config(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_src_port_id)
{
    dmx_hal_rmx_en_detect_port(mgmt, id, rmx_src_port_id);
}

hi_void dmx_hal_rmx_de_config(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rmx_src_port_id)
{
    dmx_hal_rmx_dis_detect_port(mgmt, id, rmx_src_port_id);
}

/*
 * DEMUX SWH(Band) hal level functions begin.
 */
/***********************************************************************************
* Function      : DmxHalBandEnAllTSRec
* Description   : enable all ts record
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_band_en_all_tsrec(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_DMX_BAND_CTRL reg;
    WARN_ON(id >= mgmt->band_total_cnt);

    reg.u32 = DMX_READ_REG_SWH(mgmt->io_base, DMX_BAND_CTRL(id));
    reg.bits.full_ts_en = HI_TRUE;

    DMX_WRITE_REG_SWH(mgmt->io_base, DMX_BAND_CTRL(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SWH(mgmt->io_base, DMX_BAND_CTRL(id)));
}

/***********************************************************************************
* Function      : DmxHalBandDisAllTSRec
* Description   : Disable all ts record
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_band_dis_all_tsrec(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_DMX_BAND_CTRL reg;
    WARN_ON(id >= mgmt->band_total_cnt);

    reg.u32 = DMX_READ_REG_SWH(mgmt->io_base, DMX_BAND_CTRL(id));
    reg.bits.full_ts_en = HI_FALSE;

    DMX_WRITE_REG_SWH(mgmt->io_base, DMX_BAND_CTRL(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SWH(mgmt->io_base, DMX_BAND_CTRL(id)));
}

/***********************************************************************************
* Function      : DmxHalBandAttachPort
* Description   : Attach band to port
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_band_attach_port(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 port_type, hi_u32 port_id)
{
    U_DMX_BAND_CTRL reg;
    WARN_ON(id >= mgmt->band_total_cnt);

    reg.u32 = DMX_READ_REG_SWH(mgmt->io_base, DMX_BAND_CTRL(id));
    reg.bits.dmx_mux_type = port_type;
    reg.bits.dmx_mux_num  = port_id;

    DMX_WRITE_REG_SWH(mgmt->io_base, DMX_BAND_CTRL(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SWH(mgmt->io_base, DMX_BAND_CTRL(id)));
}

/***********************************************************************************
* Function      : DmxHalBandDetachPort
* Description   : Detach port from band
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_band_detach_port(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_DMX_BAND_CTRL reg;

    WARN_ON(id >= mgmt->band_total_cnt);

    reg.u32 = DMX_READ_REG_SWH(mgmt->io_base, DMX_BAND_CTRL(id));
    reg.bits.dmx_mux_type = 0;

    DMX_WRITE_REG_SWH(mgmt->io_base, DMX_BAND_CTRL(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SWH(mgmt->io_base, DMX_BAND_CTRL(id)));
}

hi_u32 dmx_hal_band_get_qam_ts_cnt(const struct dmx_mgmt *mgmt, hi_u32 if_id)
{
    U_DBG_QAM0_STATE reg;

    WARN_ON(if_id >= mgmt->if_port_cnt);

    reg.u32 = DMX_READ_REG_SWH(mgmt->io_base, DBG_QAM0_STATE(if_id));

    return reg.bits.ts_cnt;
}

/***********************************************************************************
* Function      : DmxHalBandDetachPort
* Description   : Detach port from band
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_band_set_pcr_scr(const struct dmx_mgmt *mgmt, hi_bool pcr_scr_en)
{
    U_DMX_PCR_SCR_CTRL reg;

    reg.u32 = DMX_READ_REG_SWH(mgmt->io_base, DMX_PCR_SCR_CTRL);

    reg.bits.pcr_scr_en = HI_TRUE == pcr_scr_en ? 0x1U : 0x0U;
    reg.bits.scr_div_sel = 0x0U;   /* 27M */
    DMX_WRITE_REG_SWH(mgmt->io_base, DMX_PCR_SCR_CTRL, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SWH(mgmt->io_base, DMX_PCR_SCR_CTRL));
}

/***********************************************************************************
* Function      : dmx_hal_band_clear_chans
* Description   : Detach port from band
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_band_clear_chan(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    hi_s32 i = 0;
    hi_u32 reg;
    hi_u32 reg_index;
    hi_u32 bit_offset;
    U_DMX_BAND_CTRL band_ctrl;

    WARN_ON(id >= mgmt->band_total_cnt);

    band_ctrl.u32 = DMX_READ_REG_SWH(mgmt->io_base, DMX_BAND_CTRL(id));

    band_ctrl.bits.clear_pkt_req = 1;
    DMX_WRITE_REG_SWH(mgmt->io_base, DMX_BAND_CTRL(id), band_ctrl.u32);

    reg_index  = id / BITS_PER_REG;
    bit_offset = id % BITS_PER_REG;

    reg = DMX_READ_REG_DAV(mgmt->io_base, RAW_CLR0_PKT_INT(reg_index));
    while (((reg & (1U << bit_offset)) != (1U << bit_offset)) && (i++ <= DMX_CHAN_CLEAR_TIMEOUT_CNT)) {
        msleep(1);
        reg = DMX_READ_REG_DAV(mgmt->io_base, RAW_CLR0_PKT_INT(reg_index));
    }

    if (i >= DMX_CHAN_CLEAR_TIMEOUT_CNT) {
        HI_ERR_DEMUX("band clear chan time out, id(%u).\n", id);
    }

    reg = (1U << bit_offset);
    DMX_WRITE_REG_BUF(mgmt->io_base, RAW_CLR0_PKT_INT(reg_index), reg);

    return;
}

/***********************************************************************************
* Function      : dmx_hal_pcr_int_en
* Description   : pcr interrupt enale
* Input         : PcrId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pcr_int_en(const struct dmx_mgmt *mgmt, hi_u32 pcr_id)
{
    hi_u32 reg;

    WARN_ON(pcr_id >= mgmt->pcr_fct_cnt);

    reg = DMX_READ_REG_SWH(mgmt->io_base, IENA_SWH_PCR_CATCH);

    reg |= 1U << pcr_id;   /* enable the PCR interrupt */
    DMX_WRITE_REG_SWH(mgmt->io_base, IENA_SWH_PCR_CATCH, reg);

    DMX_COM_EQUAL(reg, DMX_READ_REG_SWH(mgmt->io_base, IENA_SWH_PCR_CATCH));
}

/***********************************************************************************
* Function      : DmxHalPcrIntDis
* Description   : pcr interrupt enale
* Input         : PcrId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pcr_int_dis(const struct dmx_mgmt *mgmt, hi_u32 pcr_id)
{
    hi_u32 reg;

    WARN_ON(pcr_id >= mgmt->pcr_fct_cnt);

    reg = DMX_READ_REG_SWH(mgmt->io_base, IENA_SWH_PCR_CATCH);

    reg &= ~(1U << pcr_id);   /* disable the PCR interrupt */
    DMX_WRITE_REG_SWH(mgmt->io_base, IENA_SWH_PCR_CATCH, reg);

    DMX_COM_EQUAL(reg, DMX_READ_REG_SWH(mgmt->io_base, IENA_SWH_PCR_CATCH));
}

/***********************************************************************************
* Function      : DmxHalPcrGetIntStatus
* Description   : get pcr interrupt status
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_pcr_get_int_status(const struct dmx_mgmt *mgmt)
{
    hi_u32 reg;

    reg = DMX_READ_REG_SWH(mgmt->io_base, ISTA_SWH_PCR_CATCH);

    return reg;
}

/***********************************************************************************
* Function      : DmxHalPcrClsIntStatus
* Description   : cls pcr interrupt status
* Input         : PcrId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pcr_cls_int_status(const struct dmx_mgmt *mgmt, hi_u32 status)
{
    hi_u32 reg = 0;
    hi_u32 reg_clear = 0;

    reg |= status;
    reg_clear &= ~(status);

    DMX_WRITE_REG_SWH(mgmt->io_base, IRAW_SWH_PCR_CATCH, reg);
    DMX_COM_EQUAL(reg_clear, DMX_READ_REG_SWH(mgmt->io_base, IRAW_SWH_PCR_CATCH));
}

/***********************************************************************************
* Function      : DmxHalPcrEn
* Description   : Set PCR Pid source
* Input         : PcrId, PcrPid etc
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pcr_en(const struct dmx_hal_pcr_info *pcr_info)
{
    U_DMX_PCR_CTRL pcr_set;

    WARN_ON(pcr_info->pcr_id >= pcr_info->mgmt->pcr_fct_cnt);
    WARN_ON(pcr_info->band >= pcr_info->mgmt->band_cnt);

    pcr_set.u32 = DMX_READ_REG_SWH(pcr_info->mgmt->io_base, DMX_PCR_CTRL(pcr_info->pcr_id));
    pcr_set.bits.pcr_pid = pcr_info->pcr_pid & 0x1fff;

    if (pcr_info->pcr_src == DMX_PCR_SRC_TYPE_PORT) {
        pcr_set.bits.pcr_ts_mux_mode = DMX_PCR_SRC_TYPE_PORT;
        pcr_set.bits.pcr_ts_mux_type = pcr_info->port_type;
        pcr_set.bits.pcr_ts_mux_num  = pcr_info->raw_port_id;
    } else {
        pcr_set.bits.pcr_ts_mux_mode = DMX_PCR_SRC_TYPE_BAND;
        pcr_set.bits.pcr_ts_mux_num  = pcr_info->band + 1;
    }

    DMX_WRITE_REG_SWH(pcr_info->mgmt->io_base, DMX_PCR_CTRL(pcr_info->pcr_id), pcr_set.u32);

    DMX_COM_EQUAL(pcr_set.u32, DMX_READ_REG_SWH(pcr_info->mgmt->io_base, DMX_PCR_CTRL(pcr_info->pcr_id)));
}

/***********************************************************************************
* Function      : DmxHalPcrDis
* Description   : Set PCR Pid source
* Input         : PcrId, PcrPid etc
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pcr_dis(const struct dmx_mgmt *mgmt, const hi_u32 pcr_id)
{
    U_DMX_PCR_CTRL pcr_set;

    WARN_ON(pcr_id >= mgmt->pcr_fct_cnt);

    pcr_set.u32 = DMX_READ_REG_SWH(mgmt->io_base, DMX_PCR_CTRL(pcr_id));
    pcr_set.bits.pcr_pid = 0x1fff;

    pcr_set.bits.pcr_ts_mux_mode = DMX_PCR_SRC_TYPE_BAND;
    pcr_set.bits.pcr_ts_mux_num  = 0x0;

    DMX_WRITE_REG_SWH(mgmt->io_base, DMX_PCR_CTRL(pcr_id), pcr_set.u32);

    DMX_COM_EQUAL(pcr_set.u32, DMX_READ_REG_SWH(mgmt->io_base, DMX_PCR_CTRL(pcr_id)));
}

/***********************************************************************************
* Function      : DmxHalPcrGetPcrValue
* Description   : Get PCR value
* Input         : PcrId, PcrVal
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pcr_get_pcr_value(const struct dmx_mgmt *mgmt, const hi_u32 pcr_id, hi_u64 *pcr_val)
{
    hi_u64 value0;
    hi_u64 value1;

    WARN_ON(pcr_id >= mgmt->pcr_fct_cnt);

    /* PCR value(extend bit and high bit)register 0,0x520+0x20*j */
    /* pcr_extra_8_0 16:8    RO  0x000   PCR extend bit */
    /* reserved       7:1    RO  0x00    reserved */
    /* pcr_base_32      0    RO  0x0     pcr_base the highest bit */
    /* PCR value(low 32 bits) register 1 0x524+0x20*j */
    /* pcr_base_31_0 31:0    RO  0x00000000  pcr_base low 32 bits */
    value0 = DMX_READ_REG_SWH(mgmt->io_base, DMX_PCR_VALUE0(pcr_id));
    value1 = DMX_READ_REG_SWH(mgmt->io_base, DMX_PCR_VALUE1(pcr_id));
    /* shift 8&32 bits, 300 is pcr param */
    *pcr_val = ((value0 & 0x1ff00) >> 8) + (((value0 & 0x1) << 32) + value1) * 300;
}

/***********************************************************************************
* Function      : DmxHalGetScrValue
* Description   : Get SCR value
* Input         : PcrId, ScrVal
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pcr_get_scr_value(const struct dmx_mgmt *mgmt, const hi_u32 pcr_id, hi_u64 *scr_val)
{
    hi_u64 value0;
    hi_u64 value1;

    WARN_ON(pcr_id >= mgmt->pcr_fct_cnt);

    /* when PCR incoming scrvalue(extend bit and high bit) register 0    0x528+0x20*j */
    /* scr_extra_8_0 16:8    RO  0x000   SCR extend bit */
    /* reserved  7:1 RO  0x00    reserved */
    /* scr_base_32   0   RO  0x0 scr_base the highest bit */
    /* when PCR incoming SCR value(low 32 bits) register 1   0x52C+0x20*j */
    /* scr_base_31_0 31:0    RO  0x00000000  scr_base low 32 bits */
    value0 = DMX_READ_REG_SWH(mgmt->io_base, DMX_PCR_SCR_VALUE0(pcr_id));
    value1 = DMX_READ_REG_SWH(mgmt->io_base, DMX_PCR_SCR_VALUE1(pcr_id));

    *scr_val = ((value0 & 0x1ff00) >> 8) + (((value0 & 0x1) << 32) + value1) * 300;
}

/***********************************************************************************
* Function      : DmxHalPcrGetCurScr
* Description   : Get current SCR value
* Input         : PcrId, ScrVal
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pcr_get_cur_scr(const struct dmx_mgmt *mgmt, hi_u32 *scr_clk)
{
    *scr_clk = DMX_READ_REG(mgmt->io_base, DMX_PCR_SCR_SET1);
}

/*  DEMUX PAR hal level functions start. */
hi_u32 dmx_hal_get_par_int_type(const struct dmx_mgmt *mgmt)
{
    U_PAR_INT_TYPE reg;

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PAR_INT_TYPE);

    return reg.u32;
}

hi_bool dmx_hal_par_check_cc_int(const struct dmx_mgmt *mgmt)
{
    U_PAR_INT_TYPE par_type;

    par_type.u32 = DMX_READ_REG_PAR(mgmt->io_base, PAR_INT_TYPE);

    if (par_type.bits.par_cc_int) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_void dmx_hal_par_set_cc_int(const struct dmx_mgmt *mgmt, hi_bool cc_en)
{
    DMX_WRITE_REG_PAR(mgmt->io_base, IENA0_CC_CHECK, cc_en);
}

hi_u32 dmx_hal_par_get_cc_int_status(const struct dmx_mgmt *mgmt)
{
    return DMX_READ_REG_PAR(mgmt->io_base, ISTA0_CC_CHECK);
}

hi_void dmx_hal_par_cls_cc_int_status(const struct dmx_mgmt *mgmt, hi_u32 status)
{
    hi_u32 reg_clear, reg_clearend;

    reg_clear = status;
    reg_clearend = reg_clear & ~(status);

    DMX_WRITE_REG_PAR(mgmt->io_base, IRAW0_CC_CHECK, reg_clear);
    DMX_COM_EQUAL(reg_clearend, DMX_READ_REG_RAM(mgmt->io_base, IRAW0_CC_CHECK));
}

hi_u32 dmx_hal_par_get_tei_int_status(const struct dmx_mgmt *mgmt)
{
    return DMX_READ_REG_PAR(mgmt->io_base, ISTA0_TEI_CHECK);
}

hi_void dmx_hal_par_cls_tei_int_status(const struct dmx_mgmt *mgmt, hi_u32 status)
{
    hi_u32 reg_clear, reg_clearend;

    reg_clear = status;
    reg_clearend = reg_clear & ~(status);

    DMX_WRITE_REG_PAR(mgmt->io_base, IRAW0_TEI_CHECK, reg_clear);
    DMX_COM_EQUAL(reg_clearend, DMX_READ_REG_RAM(mgmt->io_base, IRAW0_TEI_CHECK));
}

/*
* DEMUX SCD hal level functions begin.
*/
/* Whether to open the timestamp 27MHz clock counter count */
hi_void dmx_hal_scd_set_tts_27m_en(const struct dmx_mgmt *mgmt, hi_bool tts_27m_en)
{
    U_TTS_RLD_CFG reg;

    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TTS_RLD_CFG);
    reg.bits.tts_27m_en = tts_27m_en;

    DMX_WRITE_REG_SCD(mgmt->io_base, TTS_RLD_CFG, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SCD(mgmt->io_base, TTS_RLD_CFG));
}

/***********************************************************************************
* Function      : DmxHalPidSetScdTab
* Description   : SCD Table set register, enable/disable and set the buffer id
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_scd_set_ts_rec_cfg(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool ts_rec_en, hi_u32 buf_id)
{
    U_REC_CFG_H32 reg;

    WARN_ON(id >= mgmt->scd_chan_cnt);

    /* disable */
    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id));
    reg.bits.rec_chn_en = 0;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id), reg.u32);
    osal_mb();

    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id));
    /* configure */
    reg.bits.chn_mode = 0x0; /* record */
    reg.bits.rec_bufid  = buf_id;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id), reg.u32);
    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id)));
    osal_mb();

    /* enable */
    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id));
    reg.bits.rec_chn_en = ts_rec_en;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id), reg.u32);
    /* only compare the bit 31 */
    DMX_COM_EQUAL(reg.u32 >> 31U, DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id)) >> 31U);
}

/* Whether to force the TS packet sync header (0x47) to be replaced with CRC8 */
hi_void dmx_hal_scd_set_rec_chn_crc(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool crc_en)
{
    U_REC_CFG_H32 reg;

    WARN_ON(id >= mgmt->scd_chan_cnt);

    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id));
    reg.bits.chn_crc_en = crc_en;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id)));
}

/* Whether to add a four-byte timestamp before each recorded TS header */
hi_void dmx_hal_scd_set_rec_avpes_len_dis(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool avpes_len_dis)
{
    U_REC_CFG_H32 reg;

    WARN_ON(id >= mgmt->scd_chan_cnt);

    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id));
    reg.bits.avpes_len_dis = avpes_len_dis;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id)));
}

/* Timestamp mode select register MSB */
hi_void dmx_hal_scd_set_rec_avpes_cut_dis(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool avpes_cut_dis)
{
    U_REC_CFG_H32 reg;

    WARN_ON(id >= mgmt->scd_chan_cnt);

    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id));
    reg.bits.avpes_cut_dis = avpes_cut_dis;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id)));
}

/* Timestamp mode select register LSB */
hi_void dmx_hal_scd_set_rec_avpes_drop_en(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool avpes_drop_en)
{
    U_REC_CFG_H32 reg;

    WARN_ON(id >= mgmt->scd_chan_cnt);

    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id));
    reg.bits.avpes_drop_en = avpes_drop_en;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id)));
}

/* Whether to keep the scrambling ctrl bits to 1,after descrambled */
hi_void dmx_hal_scd_set_rec_ctrl_mode(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool ctrl_mode)
{
    U_REC_CFG_H32 reg;

    WARN_ON(id >= mgmt->scd_chan_cnt);

    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id));
    reg.bits.ctrl_mode = ctrl_mode;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id)));
}

hi_void dmx_hal_scd_set_av_pes_cfg(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 chan_en, hi_u32 mode,
                                   hi_u32 pesh_id_ena)
{
    U_REC_CFG_H32 reg;

    WARN_ON(id >= mgmt->scd_chan_cnt);

    /* disable */
    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id));
    reg.bits.rec_chn_en = 0;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id), reg.u32);
    osal_mb();

    /* configure */
    reg.bits.chn_mode  = mode;
    reg.bits.ctrl_mode = 0x0;
    reg.bits.ctrl_edit_dis = 0x0;
    reg.bits.chn_crc_en = 0x0;
    reg.bits.rec_bufid = pesh_id_ena; /* mq enable or not */
    reg.bits.rec_cnt_h8 = 0x0;
    reg.bits.avpes_len_dis = 0x1; /* close the pes len check when avpes channel */
    reg.bits.avpes_cut_dis = 0x1; /* don't drop the pes when pes len invalid */

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id), reg.u32);
    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id)));
    osal_mb();

    /* enable */
    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id));
    reg.bits.rec_chn_en = chan_en;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id), reg.u32);
    /* only compare the bit 31 */
    DMX_COM_EQUAL(reg.u32 >> 31U, DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(id)) >> 31U);
}

/***********************************************************************************
* Function      : dmx_hal_scd_get_ts_cnt
* Description   : get ths total rec ts cnt
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_u64 dmx_hal_scd_get_rec_ts_cnt(const struct dmx_mgmt *mgmt, hi_u32 rec_id)
{
    hi_u64 ts_cnt;
    hi_u64 ts_cnt_h8;
    U_REC_CFG_H32 reg_h;
    hi_u32 lts_cnt;

    WARN_ON(rec_id >= mgmt->rec_chan_cnt);

    reg_h.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_H32(rec_id));
    lts_cnt   = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_CFG_L32(rec_id));

    ts_cnt_h8 = reg_h.bits.rec_cnt_h8;
    ts_cnt = (ts_cnt_h8 << 32) | lts_cnt;

    return ts_cnt;
}

hi_void dmx_hal_scd_set_buf_id(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 buf_id)
{
    U_SCD_CFG_H32 reg;

    WARN_ON(id >= mgmt->scd_chan_cnt);

    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id));

    reg.bits.ts_scd_bufid    = buf_id;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id)));
}

hi_void dmx_hal_scd_en(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool scd_en)
{
    U_SCD_CFG_H32 reg;

    WARN_ON(id >= mgmt->scd_chan_cnt);

    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id));

    reg.bits.scd_chn_en = scd_en ? 1 : 0;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id)));
}

hi_void dmx_hal_scd_set_rec_tab(const struct dmx_mgmt *mgmt, hi_u32 id,
    hi_bool tpit_en, hi_bool pes_en, hi_bool es_long_en)
{
    U_SCD_CFG_H32 reg;

    WARN_ON(id >= mgmt->scd_chan_cnt);

    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id));

    reg.bits.idx_mode        = 0x0; /* rec index mode */
    reg.bits.scd_tpit_en     = tpit_en ? 1 : 0;
    reg.bits.scd_pes_en      = pes_en ? 1 : 0;
    reg.bits.scd_es_long_en  = es_long_en ? 1 : 0;
    reg.bits.scd_es_short_en = 0;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id)));
}

hi_void dmx_hal_scd_set_play_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool pes_en, hi_bool es_long_en)
{
    U_SCD_CFG_H32 reg;

    WARN_ON(id >= mgmt->scd_chan_cnt);

    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id));
    reg.bits.idx_mode        = 0x1; /* play index mode */
    reg.bits.scd_tpit_en     = 0x0;
    reg.bits.scd_pes_en      = pes_en ? 1 : 0;
    reg.bits.scd_es_long_en  = es_long_en ? 1 : 0;
    reg.bits.scd_es_short_en = 0x0;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id)));
}

hi_void dmx_hal_scd_set_flt_en(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool flt_en)
{
    U_SCD_CFG_H32 reg;
    hi_u32 reg_value = 0;

    WARN_ON(id >= mgmt->scd_chan_cnt);

    reg.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id));

    if (flt_en == HI_TRUE) {
        reg.bits.scd_flth_en = 0xFF;
        reg_value  = 0xFFFFFFFF;
    } else {
        reg.bits.scd_flth_en = 0x0;
        reg_value  = 0x0;
    }
    DMX_WRITE_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id), reg.u32);
    DMX_WRITE_REG_SCD(mgmt->io_base, TS_SCD_CFG_L32(id), reg_value);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_SCD(mgmt->io_base, TS_SCD_CFG_H32(id)));
    DMX_COM_EQUAL(reg_value, DMX_READ_REG_SCD(mgmt->io_base, TS_SCD_CFG_L32(id)));
}

hi_s32 dmx_hal_set_scd_range_filter(const struct dmx_mgmt *mgmt, hi_u32 flt_id, hi_u8 high, hi_u8 low, hi_u8 mask)
{
    U_FLT_CFG filter;

    WARN_ON(flt_id >= DMX_REC_SCD_RANGE_FILTER_CNT);

    filter.u32 = DMX_READ_REG_SCD(mgmt->io_base, FLT_CFG(flt_id));

    filter.bits.flt_inv = 0x0;
    filter.bits.flt_value_lo = low;
    filter.bits.flt_value_hi = high;
    filter.bits.flt_mask = mask;

    DMX_WRITE_REG_SCD(mgmt->io_base, FLT_CFG(flt_id), filter.u32);

    DMX_COM_EQUAL(filter.u32, DMX_READ_REG_SCD(mgmt->io_base, FLT_CFG(flt_id)));

    return HI_SUCCESS;
}

hi_void dmx_hal_scd_clear_chan(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool is_index)
{
    hi_s32 i = 0;
    U_TS_REC_FLUSH reg_flush;

    reg_flush.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_FLUSH);

    reg_flush.bits.ts_flush_type = (is_index == HI_TRUE) ? 1 : 0;
    reg_flush.bits.ts_flush_id = id;
    reg_flush.bits.ts_flush_en = 1;

    DMX_WRITE_REG_SCD(mgmt->io_base, TS_REC_FLUSH, reg_flush.u32);

    reg_flush.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_FLUSH);

    while ((reg_flush.bits.ts_flush_done != 1) && (i++ <= DMX_CHAN_CLEAR_TIMEOUT_CNT)) {
        if (reg_flush.bits.flush_error == 1) {
            HI_ERR_DEMUX("scd clear chan failed, is_index(%u), id(%u).\n", is_index, id);
            break;
        }
        msleep(1);
        reg_flush.u32 = DMX_READ_REG_SCD(mgmt->io_base, TS_REC_FLUSH);
    }

    if (i >= DMX_CHAN_CLEAR_TIMEOUT_CNT) {
        HI_ERR_DEMUX("scd clear chan time out, is_index(%u), id(%u).\n", is_index, id);
    }

    return;
}

/*
* DEMUX PAR(PidCh) hal level functions begin.
*/
/***********************************************************************************
* Function      : dmx_hal_pid_tab_flt_set
* Description   : Pidtable filter enable set register
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_tab_flt_set(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 band, hi_u32 pid, hi_u32 mark_id)
{
    U_PID_TAB_FILTER reg;
    WARN_ON(id >= mgmt->raw_pid_channel_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_FILTER(id));
    reg.bits.mark_id    = mark_id;
    reg.bits.dmx_id     = band;
    reg.bits.pid        = pid;

    DMX_WRITE_REG_PAR(mgmt->io_base, PID_TAB_FILTER(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_FILTER(id)));
}


/***********************************************************************************
* Function      : dmx_hal_pid_tab_flt_en
* Description   : Pidtable filter enable set register
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_tab_flt_en(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_PID_TAB_FILTER reg;
    WARN_ON(id >= mgmt->raw_pid_channel_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_FILTER(id));
    reg.bits.pid_tab_en = 0x1;

    DMX_WRITE_REG_PAR(mgmt->io_base, PID_TAB_FILTER(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_FILTER(id)));
}

/***********************************************************************************
* Function      : dmx_hal_pid_tab_flt_dis
* Description   : Pidtable filter disable set register
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_tab_flt_dis(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_PID_TAB_FILTER reg;
    WARN_ON(id >= mgmt->raw_pid_channel_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_FILTER(id));
    reg.bits.pid_tab_en = 0x0;

    DMX_WRITE_REG_PAR(mgmt->io_base, PID_TAB_FILTER(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_FILTER(id)));
}

/***********************************************************************************
* Function      : DmxHalPidTabCtlEnSet
* Description   : Pidtable control enable set register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_tab_ctl_en_set(const struct dmx_mgmt *mgmt, hi_u32 id, enum dmx_pid_chn_flag_e en_ch_type)
{
    U_PID_TAB_CTRL reg;
    WARN_ON(id >= mgmt->raw_pid_channel_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_CTRL(id));
    reg.u32 |= en_ch_type;

    DMX_WRITE_REG_PAR(mgmt->io_base, PID_TAB_CTRL(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_CTRL(id)));
}

/***********************************************************************************
* Function      : DmxHalPidTabCtlDisSet
* Description   : Pidtable control disable set register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_tab_ctl_dis_set(const struct dmx_mgmt *mgmt, hi_u32 id, enum dmx_pid_chn_flag_e en_ch_type)
{
    U_PID_TAB_CTRL reg;
    WARN_ON(id >= mgmt->raw_pid_channel_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_CTRL(id));
    reg.u32 &= ~en_ch_type;

    DMX_WRITE_REG_PAR(mgmt->io_base, PID_TAB_CTRL(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_CTRL(id)));
}

/***********************************************************************************
* Function      : dmx_hal_pid_tab_set_sub_play_chan_id
* Description   : Pidtable set subplaychan id
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_tab_set_sub_play_chan_id(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 sub_play_chan_id)
{
    U_PID_TAB_SUB_ID reg;

    WARN_ON(id >= mgmt->raw_pid_channel_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id));
    reg.bits.whole_sec_av_id = sub_play_chan_id;

    DMX_WRITE_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id)));
}

/***********************************************************************************
* Function      : DmxHalPidTabSetPCId
* Description   : Pidtable set pid copy channel id
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_tab_set_pcid(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 pcid)
{
    U_PID_TAB_SUB_ID reg;

    WARN_ON(id >= mgmt->raw_pid_channel_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id));
    reg.bits.pid_copy_id = pcid;

    DMX_WRITE_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id)));
}

/***********************************************************************************
* Function      : DmxHalPidTabSetCCDrop
* Description   : set the cc error and repeat drop flag
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_tab_set_ccdrop(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool ccerr_drop, hi_bool ccrepeat_drop)
{
    U_PID_TAB_SUB_ID reg;

    WARN_ON(id >= mgmt->raw_pid_channel_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id));
    reg.bits.cc_err_drop = ccerr_drop ? 1 : 0;
    reg.bits.cc_repeat_drop = ccrepeat_drop ? 1 : 0;

    DMX_WRITE_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id)));
}

/***********************************************************************************
* Function      : dmx_hal_pid_set_rec_tab
* Description   : Rec Table set register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_set_rec_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 rec_id)
{
    U_PID_TAB_REC_SCD reg;

    WARN_ON(id >= mgmt->raw_pid_channel_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_REC_SCD(id));
    reg.bits.rec_id = rec_id;

    DMX_WRITE_REG_PAR(mgmt->io_base, PID_TAB_REC_SCD(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_REC_SCD(id)));
}

/***********************************************************************************
* Function      : dmx_hal_pid_set_rec_dsc_mode
* Description   : Rec Table set register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_set_rec_dsc_mode(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool descramed)
{
    U_PID_TAB_SUB_ID reg;

    WARN_ON(id >= mgmt->raw_pid_channel_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id));
    if (descramed == HI_TRUE) {
        reg.bits.dsc_rec_mode = 0x0;  /* record clear stream */
    } else {
        reg.bits.dsc_rec_mode = 0x1;  /* record original scrambled stream */
    }

    DMX_WRITE_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id)));
}

/***********************************************************************************
* Function      : DmxHalPidSetScdTab
* Description   : SCD Table set register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_set_scd_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 ts_scd_id, hi_u32 pes_scd_id)
{
    U_PID_TAB_REC_SCD reg;

    WARN_ON(id >= mgmt->raw_pid_channel_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_REC_SCD(id));
    reg.bits.pes_scd_id = pes_scd_id;
    reg.bits.scd_id     = ts_scd_id;

    DMX_WRITE_REG_PAR(mgmt->io_base, PID_TAB_REC_SCD(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_REC_SCD(id)));
}

/***********************************************************************************
* Function      : DmxHalPidSetWholeTSTab
* Description   : Whole Ts set register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_set_whole_tstab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 buf_id, hi_bool no_afcheck)
{
    U_WHOLE_TS_TAB reg;

    WARN_ON(id >= mgmt->ts_chan_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, WHOLE_TS_TAB(id));
    reg.bits.whole_ts_buf_id = buf_id;
    reg.bits.whole_af_check_dis = no_afcheck;

    DMX_WRITE_REG_PAR(mgmt->io_base, WHOLE_TS_TAB(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, WHOLE_TS_TAB(id)));
}

/***********************************************************************************
* Function      : DmxHalPidSetAvPesTab
* Description   : AvPes table register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_set_av_pes_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 buf_id,
    enum dmx_pid_flt_rec_e en_flt_rec, hi_bool pusi_en, hi_bool av_pes_len_chk)
{
    U_AV_PES_TAB reg;

    WARN_ON(id >= (mgmt->av_pes_chan_cnt + mgmt->rec_chan_cnt));

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, AV_PES_TAB(id));
    reg.bits.av_pes_buf_id = buf_id;
    reg.bits.av_pusi_en = pusi_en;
    reg.bits.flt_rec_sel = en_flt_rec;
    reg.bits.av_pes_len_det = av_pes_len_chk;

    DMX_WRITE_REG_PAR(mgmt->io_base, AV_PES_TAB(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, AV_PES_TAB(id)));
}

/***********************************************************************************
* Function      : DmxHalPidSetPesSecTab
* Description   : Pes section table register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_set_pes_sec_tab(const struct dmx_mgmt *mgmt, hi_u32 id, enum dmx_pid_pes_sec_type_e en_pes_sec_type,
    hi_bool pusi_en, hi_bool pes_len_chk)
{
    U_PES_SEC_TAB0 reg;

    WARN_ON(id >= mgmt->pes_sec_chan_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PES_SEC_TAB0(id));

    reg.bits.data_type = en_pes_sec_type;
    reg.bits.pes_sec_pusi_en = pusi_en;
    reg.bits.pes_sec_len_det = pes_len_chk;

    DMX_WRITE_REG_PAR(mgmt->io_base, PES_SEC_TAB0(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, PES_SEC_TAB0(id)));
}

/***********************************************************************************
* Function      : DmxHalPidWholeTSOutFltSel
* Description   : whole ts out select
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_whole_tsout_flt_sel(const struct dmx_mgmt *mgmt, hi_bool flt_en)
{
    U_WHOLE_TS_OUT_SEL reg;

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, WHOLE_TS_OUT_SEL);
    reg.bits.whole_ts_out_sel =  flt_en ? 1 : 0;

    DMX_WRITE_REG_PAR(mgmt->io_base, WHOLE_TS_OUT_SEL, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, WHOLE_TS_OUT_SEL));
}

/***********************************************************************************
* Function      : DmxHalPidSetTsParse
* Description   : Ts parse set
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_set_ts_parse(const struct dmx_mgmt *mgmt, hi_bool af_chk, hi_bool all_af_drop)
{
    U_TS_PARSER_CTRL reg;

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, TS_PARSER_CTRL);
    reg.bits.af_check_dis =  af_chk ? 1 : 0;
    reg.bits.all_af_drop  =  all_af_drop ? 1 : 0;

    DMX_WRITE_REG_PAR(mgmt->io_base, TS_PARSER_CTRL, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, TS_PARSER_CTRL));
}

hi_void dmx_hal_pid_clear_ram_state(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_STATE_RAM_ID reg_ram_id;
    U_STATE_RAM_CTRL ram_state_ctrl;

    WARN_ON(id >= mgmt->raw_pid_channel_cnt);

    reg_ram_id.u32 = DMX_READ_REG_PAR(mgmt->io_base, STATE_RAM_ID);

    reg_ram_id.bits.state_ram_id = id;

    DMX_WRITE_REG_PAR(mgmt->io_base, STATE_RAM_ID, reg_ram_id.u32);

    ram_state_ctrl.u32 = DMX_READ_REG_PAR(mgmt->io_base, STATE_RAM_CTRL);

    ram_state_ctrl.bits.state_data = 0x3ff;

    DMX_WRITE_REG_PAR(mgmt->io_base, STATE_RAM_CTRL, ram_state_ctrl.u32);

    return;
}

/***********************************************************************************
* Function      : DmxHalPidTabCtlCwEnSet
* Description   : Pidtable control enable cw set register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_cw_en_set(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool cw_en)
{
    U_PID_TAB_CTRL reg;
    WARN_ON(id >= mgmt->raw_pid_channel_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_CTRL(id));
    reg.bits.cw_en = cw_en ? 1 : 0;

    DMX_WRITE_REG_PAR(mgmt->io_base, PID_TAB_CTRL(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_CTRL(id)));
}

/***********************************************************************************
* Function      : DmxHalPidSetCwId
* Description   :  CwId set register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_set_cw_id(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 cw_id)
{
    U_PID_TAB_SUB_ID reg;
    WARN_ON(id >= mgmt->pid_channel_cnt);
    WARN_ON(cw_id >= mgmt->dsc_fct_cnt);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id));
    reg.bits.cw_id = cw_id;

    DMX_WRITE_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_TAB_SUB_ID(id)));
}

/***********************************************************************************
* Function      : DmxHalPidEnPCMarkValid
* Description   : Pid Copy mark valid enable register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_en_pcmark_valid(const struct dmx_mgmt *mgmt, hi_u32 pcid, hi_u32 mark_valid)
{
    U_PID_COPY_TAB_0 pc_tab0;
    WARN_ON(pcid >= mgmt->pid_copy_chan_cnt);

    pc_tab0.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_COPY_TAB_0(pcid));

    pc_tab0.bits.mark_valid |= (1U << mark_valid);
    DMX_WRITE_REG_PAR(mgmt->io_base, PID_COPY_TAB_0(pcid), pc_tab0.u32);

    DMX_COM_EQUAL(pc_tab0.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_COPY_TAB_0(pcid)));
}

/***********************************************************************************
* Function      : DmxHalPidDisPCMarkValid
* Description   : Pid Copy mark valid disable register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_dis_pcmark_valid(const struct dmx_mgmt *mgmt, hi_u32 pcid, hi_u32 mark_valid)
{
    U_PID_COPY_TAB_0 pc_tab0;
    WARN_ON(pcid >= mgmt->pid_copy_chan_cnt);

    pc_tab0.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_COPY_TAB_0(pcid));

    pc_tab0.bits.mark_valid &= ~(1U << mark_valid);
    DMX_WRITE_REG_PAR(mgmt->io_base, PID_COPY_TAB_0(pcid), pc_tab0.u32);

    DMX_COM_EQUAL(pc_tab0.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_COPY_TAB_0(pcid)));
}

/***********************************************************************************
* Function      : DmxHalPidEnPCMarkValid
* Description   : Pid Copy mark valid enable register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_set_pcbuf_id(const struct dmx_mgmt *mgmt, hi_u32 pcid, hi_u32 buf_id)
{
    U_PID_COPY_TAB_1 pc_tab1;
    WARN_ON(pcid >= mgmt->pid_copy_chan_cnt);

    pc_tab1.u32 = DMX_READ_REG_PAR(mgmt->io_base, PID_COPY_TAB_1(pcid));

    pc_tab1.bits.copy_buf_id = buf_id;
    DMX_WRITE_REG_PAR(mgmt->io_base, PID_COPY_TAB_1(pcid), pc_tab1.u32);

    DMX_COM_EQUAL(pc_tab1.u32, DMX_READ_REG_PAR(mgmt->io_base, PID_COPY_TAB_1(pcid)));
}

/***********************************************************************************
* Function      : DmxHalPidSetDscType
* Description   : descrambler type set register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_set_dsc_type(const struct dmx_mgmt *mgmt, hi_u32 dsc_id, hi_bool ts_desc_en, hi_bool pes_desc_en)
{
    U_CW_TAB reg;
    hi_u32 offset;
    WARN_ON(dsc_id >= mgmt->dsc_fct_cnt);

    /* cw 0~127 at address of CW_TAB0(dsc_id),cw 128~255 at address of CW_TAB1(dsc_id) */
    offset = dsc_id > 127 ? CW_TAB1(dsc_id) : CW_TAB0(dsc_id);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, offset);
    reg.bits.ts_descram = ts_desc_en ? 1 : 0;
    reg.bits.pes_descram = pes_desc_en ? 1 : 0;

    DMX_WRITE_REG_PAR(mgmt->io_base, CW_TAB0(dsc_id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, offset));
}

hi_void dmx_hal_pid_get_dsc_type(const struct dmx_mgmt *mgmt, hi_u32 dsc_id, hi_dmx_scrambled_flag *scramble_flag)
{
    U_CW_TAB reg;
    hi_u32 offset;
    WARN_ON(dsc_id >= mgmt->dsc_fct_cnt);

    /* cw 0~127 at address of CW_TAB0(dsc_id),cw 128~255 at address of CW_TAB1(dsc_id) */
    offset = dsc_id > 127 ? CW_TAB1(dsc_id) : CW_TAB0(dsc_id);

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, offset);

    if (reg.bits.ts_descram) {
        *scramble_flag = HI_DMX_SCRAMBLED_FLAG_TS;
    } else if (reg.bits.pes_descram) {
        *scramble_flag = HI_DMX_SCRAMBLED_FLAG_PES;
    } else {
        *scramble_flag = HI_DMX_SCRAMBLED_FLAG_NO;
    }
}

hi_void dmx_hal_pid_ch_config(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 band, hi_u32 pid, hi_u32 mark_id)
{
    /* set the pid channel */
    dmx_hal_pid_tab_flt_set(mgmt, id, band, pid, mark_id);
    /* diable pid copy */
    dmx_hal_pid_tab_ctl_dis_set(mgmt, id, DMX_PID_CHN_PIDCOPY_FLAG);
    /* whole ts send to flt */
    dmx_hal_pid_whole_tsout_flt_sel(mgmt, HI_TRUE);
    /* disable the cc err and cc repeat drop function */
    dmx_hal_pid_tab_set_ccdrop(mgmt, id, HI_FALSE, HI_FALSE);
    /* disable the af check and disable drop all af field */
    dmx_hal_pid_set_ts_parse(mgmt, HI_TRUE, HI_FALSE);
}

hi_void dmx_hal_pid_ch_de_config(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    dmx_hal_pid_tab_flt_dis(mgmt, id);

    /* clear the pid channel */
    dmx_hal_pid_clear_ram_state(mgmt, id);
}

hi_u32 dmx_hal_pid_ch_get_id(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    /* get pid channel id */
    return DMX_READ_REG_PAR(mgmt->io_base, DBG_PID_CNT_CHN(id));
}

hi_u32 dmx_hal_pid_ch_pkt_cnt(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_DBG_PID_CNT_CHN reg;

    /* check if id and channel id in register is correct. */
    reg.u32 = dmx_hal_pid_ch_get_id(mgmt, id);

    if (reg.bits.dbg_pid_cnt_chn == id) {
        return DMX_READ_REG_PAR(mgmt->io_base, DBG_PID_PKT_CNT(id));
    } else {
        HI_ERR_DEMUX("channel id unmatch with register value!\n");
        return 0;
    }
}

/***********************************************************************************
* Function      : DmxHalFullTSOutFltSel
* Description   : full ts out select
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_set_full_tsout_flt(const struct dmx_mgmt *mgmt, enum dmx_full_ts_out_type_e full_ts_out)
{
    U_WHOLE_TS_OUT_SEL reg;

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, WHOLE_TS_OUT_SEL);

    reg.bits.full_ts_out_sel  =  full_ts_out;

    DMX_WRITE_REG_PAR(mgmt->io_base, WHOLE_TS_OUT_SEL, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, WHOLE_TS_OUT_SEL));
}

/***********************************************************************************
* Function      : DmxHalSetFullTSCtlTab
* Description   : full ts ctrl tab set
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_set_full_tsctl_tab(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 buf_id, hi_bool ccpush_save,
                                   hi_bool tei_drop)
{
    U_DMX_TS_CTRL_TAB reg;

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, DMX_TS_CTRL_TAB(id));

    reg.bits.full_ts_buf_id      = buf_id;
    reg.bits.tei_drop            = tei_drop ? 1 : 0;
    reg.bits.cc_err_pusi_save    = ccpush_save ? 1 : 0;
    reg.bits.cc_repeat_pusi_save = ccpush_save ? 1 : 0;

    DMX_WRITE_REG_PAR(mgmt->io_base, DMX_TS_CTRL_TAB(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, DMX_TS_CTRL_TAB(id)));
}

hi_void dmx_hal_set_ts_tei_ctl(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool tei_drop)
{
    U_DMX_TS_CTRL_TAB reg;

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, DMX_TS_CTRL_TAB(id));

    reg.bits.tei_drop = tei_drop ? 1 : 0;

    DMX_WRITE_REG_PAR(mgmt->io_base, DMX_TS_CTRL_TAB(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, DMX_TS_CTRL_TAB(id)));
}

hi_void dmx_hal_set_ts_pusi_ctl(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool ccpusi_save)
{
    U_DMX_TS_CTRL_TAB reg;

    reg.u32 = DMX_READ_REG_PAR(mgmt->io_base, DMX_TS_CTRL_TAB(id));

    reg.bits.cc_repeat_pusi_save = ccpusi_save;
    reg.bits.cc_err_pusi_save = ccpusi_save;

    DMX_WRITE_REG_PAR(mgmt->io_base, DMX_TS_CTRL_TAB(id), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_PAR(mgmt->io_base, DMX_TS_CTRL_TAB(id)));
}

/* DEMUX FLT hal level functions begin. */
/***********************************************************************************
* Function      : DmxHalFltUnLock
* Description   :  Unlock Flt
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_flt_un_lock(const struct dmx_mgmt *mgmt)
{
    /* configure the flt pes section id */
    U_DMX_PES_SEC_ID reg;

    /* unlock the bit */
    reg.u32 = DMX_READ_REG_FLT(mgmt->io_base, DMX_PES_SEC_ID);;

    reg.bits.pes_sec_id_lock = 0;

    DMX_WRITE_REG_FLT(mgmt->io_base, DMX_PES_SEC_ID, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_FLT(mgmt->io_base, DMX_PES_SEC_ID));
}

/***********************************************************************************
* Function      : DmxHalFltLock
* Description   :  Lock Flt
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_flt_lock(const struct dmx_mgmt *mgmt)
{
    U_DMX_PES_SEC_ID reg;

    /* lock the bit */
    reg.u32 = DMX_READ_REG_FLT(mgmt->io_base, DMX_PES_SEC_ID);

    reg.bits.pes_sec_id_lock = 1;

    DMX_WRITE_REG_FLT(mgmt->io_base, DMX_PES_SEC_ID, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_FLT(mgmt->io_base, DMX_PES_SEC_ID));
}

/***********************************************************************************
* Function      : DmxHalFltEnable
* Description   :  Flt id enable
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_flt_enable(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    hi_u32 reg;
    WARN_ON(id >= mgmt->flt_cnt);

    reg = DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_EN);

    reg |= 1 << id;

    DMX_WRITE_REG_FLT(mgmt->io_base, DMX_FILTER_EN, reg);

    DMX_COM_EQUAL(reg, DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_EN));
}

/***********************************************************************************
* Function      : DmxHalFltDisable
* Description   :  Flt id disable
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_flt_disable(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    hi_u32 reg;
    WARN_ON(id >= mgmt->flt_cnt);

    reg = DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_EN);

    reg &= ~(1 << id);
    DMX_WRITE_REG_FLT(mgmt->io_base, DMX_FILTER_EN, reg);

    DMX_COM_EQUAL(reg, DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_EN));
}


/***********************************************************************************
* Function      : DmxHalFltSetPesSecId
* Description   :  Flt set the pes section id
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_flt_set_pes_sec_id(const struct dmx_mgmt *mgmt, hi_u32 pes_sec_id)
{
    U_DMX_PES_SEC_ID reg;

    reg.u32 = DMX_READ_REG_FLT(mgmt->io_base, DMX_PES_SEC_ID);

    /* configure the flt pes section id */
    reg.bits.pes_sec_id     = pes_sec_id;

    DMX_WRITE_REG_FLT(mgmt->io_base, DMX_PES_SEC_ID, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_FLT(mgmt->io_base, DMX_PES_SEC_ID));
}

/***********************************************************************************
* Function      : DmxHalFltSetBufId
* Description   :  Set the Flt Buffer Id
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_flt_set_buf_id(const struct dmx_mgmt *mgmt, hi_u32 buf_id)
{
    U_DMX_FILTER_BUF_ID reg;

    reg.u32 = DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_BUF_ID);
    /* set the buf_id */
    reg.bits.flt_buf_id     = buf_id;

    DMX_WRITE_REG_FLT(mgmt->io_base, DMX_FILTER_BUF_ID, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_BUF_ID));
}

hi_void dmx_hal_flt_set_ctl_attrs(const struct dmx_mgmt *mgmt, hi_u32 flt_min, hi_u32 flt_num, hi_bool err_pes_drop)
{
    U_DMX_FILTER_CTRL reg;

    reg.u32 = DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_CTRL);

    reg.bits.flt_min              = flt_min;
    reg.bits.flt_num              = flt_num;
    reg.bits.pes_len_err_drop_dis = err_pes_drop;

    DMX_WRITE_REG_FLT(mgmt->io_base, DMX_FILTER_CTRL, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_CTRL));
}

hi_void dmx_hal_flt_set_ctl_crc(const struct dmx_mgmt *mgmt, dmx_flt_crc_mode crc_mode)
{
    U_DMX_FILTER_CTRL reg;

    reg.u32 = DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_CTRL);

    reg.bits.crc_mode = crc_mode;

    DMX_WRITE_REG_FLT(mgmt->io_base, DMX_FILTER_CTRL, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_CTRL));
}

/***********************************************************************************
* Function      : DmxHalFltSetFltId
* Description   :  Set the Flt Id register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_flt_set_flt_id(const struct dmx_mgmt *mgmt, hi_u32 index, hi_u32 flt_id)
{
    U_DMX_FILTER_ID reg;

    reg.u32 = DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_ID(index));

    reg.bits.fit_id = flt_id;

    DMX_WRITE_REG_FLT(mgmt->io_base, DMX_FILTER_ID(index), reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_ID(index)));
}

/***********************************************************************************
* Function      : DmxHalFltSetFltByte
* Description   :  Set the Flt Byte register
* Input         :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_flt_set_flt_byte(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 byte_inx,
    hi_bool wdata_mode, hi_u32 wdata_content, hi_u32 wdata_mask)
{
    U_DMX_FILTER reg;

    reg.u32 = DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER(byte_inx));

    reg.bits.filter_id     = id;
    reg.bits.wdata_mode    = wdata_mode;    /* 0(HI_FALSE):match pass; 1(HI_TRUE):match unpass */
    reg.bits.wdata_content = wdata_content;
    reg.bits.wdata_mask    = wdata_mask;

    DMX_WRITE_REG_FLT(mgmt->io_base, DMX_FILTER(byte_inx), reg.u32);

    /* mask some ro bits and reserved bits */
    DMX_COM_EQUAL(0x3ff1ffff & reg.u32, 0x3ff1ffff & DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER(byte_inx)));
}

hi_void dmx_hal_flt_attach_config(const struct dmx_mgmt *mgmt, hi_u32 index, hi_u32 flt_id, hi_u32 pes_sec_id,
                                  hi_u32 buf_id, dmx_flt_crc_mode crc_mode)
{
    /* attention: here must the sequence: unlock->set pessecid->lock */
    /* unlock the flt */
    dmx_hal_flt_un_lock(mgmt);
    /* configure the pessec channel */
    dmx_hal_flt_set_pes_sec_id(mgmt, pes_sec_id);
    /* lock the flt */
    dmx_hal_flt_lock(mgmt);

    /* set the buffer id */
    dmx_hal_flt_set_buf_id(mgmt, buf_id);

    /* set the filter id */
    dmx_hal_flt_set_flt_id(mgmt, index, flt_id);

    /* configure crc mode */
    dmx_hal_flt_set_ctl_crc(mgmt, crc_mode);

    /* enable the filter */
    dmx_hal_flt_enable(mgmt, index);
}

hi_void dmx_hal_flt_detach_config(const struct dmx_mgmt *mgmt, hi_u32 index, hi_u32 flt_id, hi_u32 pes_sec_id)
{
    /* attention: here must the sequence: unlock->set pessecid->lock */
    /* unlock the flt */
    dmx_hal_flt_un_lock(mgmt);
    /* configure the pessec channel */
    dmx_hal_flt_set_pes_sec_id(mgmt, pes_sec_id);
    /* lock the flt */
    dmx_hal_flt_lock(mgmt);

    /* set the buffer id */
    dmx_hal_flt_set_buf_id(mgmt, 0);

    /* set the filter id */
    dmx_hal_flt_set_flt_id(mgmt, index, 0);

    /* configure crc mode */
    dmx_hal_flt_set_ctl_crc(mgmt, DMX_FLT_CRC_MODE_MAX);

    /* enable the filter */
    dmx_hal_flt_disable(mgmt, index);
}

hi_void dmx_hal_flt_attrs_config(const struct dmx_mgmt *mgmt, hi_u32 pes_sec_id, hi_u32 flt_num)
{
    /* attention: here must the sequence: unlock->set pessecid->lock */
    /* unlock the flt */
    dmx_hal_flt_un_lock(mgmt);
    /* configure the pessec channel */
    dmx_hal_flt_set_pes_sec_id(mgmt, pes_sec_id);
    /* lock the flt */
    dmx_hal_flt_lock(mgmt);

    /* set the ctl register  */
    dmx_hal_flt_set_ctl_attrs(mgmt, 0, flt_num, HI_TRUE);
}

hi_void dmx_hal_flt_clear_pes_sec_chan(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    hi_s32 i = 0;
    U_DBG_FLT_FSM_RAM_ID reg_id;
    U_CLEAR_FLT_FSM reg_fsm;

    reg_id.u32 = DMX_READ_REG_FLT(mgmt->io_base, DBG_FLT_FSM_RAM_ID);

    reg_id.bits.dbg_flt_fsm_id = id;

    DMX_WRITE_REG_FLT(mgmt->io_base, DBG_FLT_FSM_RAM_ID, reg_id.u32);

    DMX_COM_EQUAL(reg_id.u32, DMX_READ_REG_FLT(mgmt->io_base, DBG_FLT_FSM_RAM_ID));

    reg_fsm.u32 = DMX_READ_REG_FLT(mgmt->io_base, CLEAR_FLT_FSM);

    reg_fsm.bits.clr_flt_fsm_req = 1;

    DMX_WRITE_REG_FLT(mgmt->io_base, CLEAR_FLT_FSM, reg_fsm.u32);

    reg_fsm.u32 = DMX_READ_REG_FLT(mgmt->io_base, CLEAR_FLT_FSM);
    while (reg_fsm.bits.clr_flt_fsm_req && (i++ <= DMX_CHAN_CLEAR_TIMEOUT_CNT)) {
        msleep(1);
        reg_fsm.u32 = DMX_READ_REG_FLT(mgmt->io_base, CLEAR_FLT_FSM);
    }

    if (i >= DMX_CHAN_CLEAR_TIMEOUT_CNT) {
        HI_ERR_DEMUX("pes/sec(%u) clear chan time out.\n", id);
    }

    return;
}

hi_void dmx_hal_flt_clear_av_pes_chan(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    hi_s32 i = 0;
    U_DBG_AV_FSM_RAM_ID reg_id;
    U_CLEAR_AV_FSM reg_fsm;

    reg_id.u32 = DMX_READ_REG_FLT(mgmt->io_base, DBG_AV_FSM_RAM_ID);

    reg_id.bits.dbg_av_fsm_id = id;

    DMX_WRITE_REG_FLT(mgmt->io_base, DBG_AV_FSM_RAM_ID, reg_id.u32);

    DMX_COM_EQUAL(reg_id.u32, DMX_READ_REG_FLT(mgmt->io_base, DBG_AV_FSM_RAM_ID));

    reg_fsm.u32 = DMX_READ_REG_FLT(mgmt->io_base, CLEAR_AV_FSM);

    reg_fsm.bits.clr_av_fsm_req = 1;

    DMX_WRITE_REG_FLT(mgmt->io_base, CLEAR_AV_FSM, reg_fsm.u32);

    reg_fsm.u32 = DMX_READ_REG_FLT(mgmt->io_base, CLEAR_AV_FSM);
    while (reg_fsm.bits.clr_av_fsm_req && (i++ <= DMX_CHAN_CLEAR_TIMEOUT_CNT)) {
        msleep(1);
        reg_fsm.u32 = DMX_READ_REG_FLT(mgmt->io_base, CLEAR_AV_FSM);
    }

    if (i >= DMX_CHAN_CLEAR_TIMEOUT_CNT) {
        HI_ERR_DEMUX("av_pes(%u) clear chan time out.\n", id);
    }

    return;
}

static hi_void dmx_hal_flt_disable_all_flt(const struct dmx_mgmt *mgmt)
{
    hi_u32 reg;

    reg = DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_EN);

    reg = 0;
    DMX_WRITE_REG_FLT(mgmt->io_base, DMX_FILTER_EN, reg);

    DMX_COM_EQUAL(reg, DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_EN));
}

static hi_void dmx_hal_flt_set_no_flt_mod(const struct dmx_mgmt *mgmt, hi_bool is_drop)
{
    U_DMX_FILTER_CTRL reg;

    reg.u32 = DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_CTRL);

    reg.bits.flt_min              = 0;
    reg.bits.flt_num              = 1;
    reg.bits.no_flt_mode          = is_drop;

    DMX_WRITE_REG_FLT(mgmt->io_base, DMX_FILTER_CTRL, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_FLT(mgmt->io_base, DMX_FILTER_CTRL));
}

hi_void dmx_hal_flt_set_sec_no_pusi(const struct dmx_mgmt *mgmt, hi_bool  no_pusi)
{
    U_DMX_SEC_GLOBAL_CTRL reg;

    reg.u32 = DMX_READ_REG_FLT(mgmt->io_base, DMX_SEC_GLOBAL_CTRL);
    reg.bits.new_sec_nopusi = no_pusi;
    reg.bits.new_sec_pusi_point = no_pusi;
    reg.bits.new_sec_pusi_nopint = no_pusi;
    DMX_WRITE_REG_FLT(mgmt->io_base, DMX_SEC_GLOBAL_CTRL, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_FLT(mgmt->io_base, DMX_SEC_GLOBAL_CTRL));
}

hi_void dmx_hal_flt_set_pes_default_attr(const struct dmx_mgmt *mgmt, hi_u32 pes_sec_id, hi_u32 buf_id)
{
    /* attention: here must the sequence: unlock->set pessecid->lock */
    /* unlock the flt */
    dmx_hal_flt_un_lock(mgmt);
    /* configure the pessec channel */
    dmx_hal_flt_set_pes_sec_id(mgmt, pes_sec_id);
    /* lock the flt */
    dmx_hal_flt_lock(mgmt);

    /* set the ctl register */
    dmx_hal_flt_set_no_flt_mod(mgmt, HI_FALSE);
    dmx_hal_flt_set_buf_id(mgmt, buf_id);
    dmx_hal_flt_disable_all_flt(mgmt);
}

hi_void dmx_hal_flt_set_sec_default_attr(const struct dmx_mgmt *mgmt, hi_u32 pes_sec_id, hi_u32 buf_id)
{
    /* attention: here must the sequence: unlock->set pessecid->lock */
    /* unlock the flt */
    dmx_hal_flt_un_lock(mgmt);
    /* configure the pessec channel */
    dmx_hal_flt_set_pes_sec_id(mgmt, pes_sec_id);
    /* lock the flt */
    dmx_hal_flt_lock(mgmt);

    /* set the ctl register  */
    dmx_hal_flt_set_no_flt_mod(mgmt, HI_TRUE);
    dmx_hal_flt_set_buf_id(mgmt, buf_id);
    dmx_hal_flt_disable_all_flt(mgmt);

    /* set the default no_pusi attr */
    dmx_hal_flt_set_sec_no_pusi(mgmt, HI_TRUE);
}

/* DEMUX DVA(BUF) hal level functions begin. */
/***********************************************************************************
* Function       : DmxHalBufGetTimeoutIntStatus
* Description   : get the timeout interrupt status
* Input           :
* Output         :
* Return         :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_buf_get_timeout_int_status(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_REG_STATE_RAM state;

    WARN_ON(id >= mgmt->buf_cnt);

    state.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));

    return state.bits.timeout_int;
}

/***********************************************************************************
* Function      : DmxHalBufClsTimeoutIntStatus
* Description   : clear the timeout interrupt status
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_cls_timeout_int_status(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_REG_STATE_RAM state;

    WARN_ON(id >= mgmt->buf_cnt);

    state.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));

    state.bits.raw_ts_int = 0;
    state.bits.raw_seop_int = 0;
    state.bits.raw_ovfl_int = 0;
    state.bits.raw_timeout_int = 1;
    state.bits.raw_prs_clr_int = 0;

    DMX_WRITE_REG_BUF(mgmt->io_base, REG_STATE_RAM(id), state.u32);
}

/***********************************************************************************
* Function       : DmxHalBufGetTsCntIntStatus
* Description   : get the tscnt interrupt status
* Input           :
* Output         :
* Return         :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_buf_get_ts_cnt_int_status(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_REG_STATE_RAM state;

    WARN_ON(id >= mgmt->buf_cnt);

    state.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));

    return state.bits.ts_int;
}

/***********************************************************************************
* Function      : DmxHalBufClsTimeoutIntStatus
* Description   : clear the tscnt interrupt status
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_cls_ts_cnt_int_status(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_REG_STATE_RAM state;

    WARN_ON(id >= mgmt->buf_cnt);

    state.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));

    state.bits.raw_ts_int = 1;
    state.bits.raw_seop_int = 0;
    state.bits.raw_ovfl_int = 0;
    state.bits.raw_timeout_int = 0;
    state.bits.raw_prs_clr_int = 0;

    DMX_WRITE_REG_BUF(mgmt->io_base, REG_STATE_RAM(id), state.u32);
}

/***********************************************************************************
* Function       : DmxHalBufGetSeopIntStatus
* Description   : get the seop interrupt status
* Input           :
* Output         :
* Return         :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_buf_get_seop_int_status(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_REG_STATE_RAM state;

    WARN_ON(id >= mgmt->buf_cnt);

    state.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));

    return state.bits.seop_int;
}

/***********************************************************************************
* Function      : DmxHalBufClsSeopIntStatus
* Description   : clear the seop interrupt status
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_cls_seop_int_status(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_REG_STATE_RAM state;

    WARN_ON(id >= mgmt->buf_cnt);

    state.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));

    state.bits.raw_ts_int = 0;
    state.bits.raw_seop_int = 1;
    state.bits.raw_ovfl_int = 0;
    state.bits.raw_timeout_int = 0;
    state.bits.raw_prs_clr_int = 0;

    DMX_WRITE_REG_BUF(mgmt->io_base, REG_STATE_RAM(id), state.u32);
}

hi_u32 dmx_hal_buf_get_ovfl_int_status(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_REG_STATE_RAM state;

    WARN_ON(id >= mgmt->buf_cnt);

    state.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));

    return state.bits.ovfl_int;
}

hi_void dmx_hal_buf_cls_ovfl_int_status(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_REG_STATE_RAM state;

    WARN_ON(id >= mgmt->buf_cnt);

    state.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));

    state.bits.raw_ts_int = 0;
    state.bits.raw_seop_int = 0;
    state.bits.raw_ovfl_int = 1;
    state.bits.raw_timeout_int = 0;
    state.bits.raw_prs_clr_int = 0;

    DMX_WRITE_REG_BUF(mgmt->io_base, REG_STATE_RAM(id), state.u32);
}

/***********************************************************************************
* Function      : DmxHalBufGetRawApIntStatus
* Description   : clear the Raw anti pressure interrupt status
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_buf_get_raw_ap_int_status(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_SEOP_ADDR reg;

    WARN_ON(id >= mgmt->buf_cnt);

    reg.u32 = DMX_READ_REG_BUF(mgmt->io_base, SEOP_ADDR(id));

    return reg.bits.ap_flg;
}

/***********************************************************************************
* Function      : DmxHalBufClsApIntStatus
* Description   : clear the anti pressure interrupt status
* Input         : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_cls_ap_int_status(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_SEOP_ADDR reg;

    WARN_ON(id >= mgmt->buf_cnt);

    reg.u32 = DMX_READ_REG_BUF(mgmt->io_base, SEOP_ADDR(id));

    reg.bits.ap_flg = 0x1;  /* clear the ap flag */

    DMX_WRITE_REG_BUF(mgmt->io_base, SEOP_ADDR(id), reg.u32);
}

/***********************************************************************************
* Function      : DmxHalBufSetInt
* Description  : Enable or disable the buffer interrupt, depend on input threshold value
* Input          : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
/*
timeout_int [12:10] 8 level
000:1.308ms
001:2.616ms
010:5.232ms
011:10.464ms
100:20.928ms
101:41.856ms
110:83.712ms
111:167.424ms
*/
hi_void dmx_hal_buf_set_int_th(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 timeout_th, hi_u32 ts_th,
    hi_u32 seop_th, hi_bool timeout_int, hi_bool ovfl_int)
{
    U_INT_SET int_set;

    WARN_ON(id >= mgmt->buf_cnt);

    int_set.u32 = DMX_READ_REG_BUF(mgmt->io_base, INT_SET(id));

    int_set.bits.timeout_int_en = (timeout_int == HI_TRUE) ? 1 : 0;
    int_set.bits.timeout_int_th = timeout_th;

    int_set.bits.ts_int_th      = ts_th;
    int_set.bits.ts_int_en      = (ts_th > 0) ? 1 : 0;

    int_set.bits.seop_int_th    = seop_th;
    int_set.bits.seop_int_en    = (seop_th > 0) ? 1 : 0;
    int_set.bits.ovfl_int_en    = (ovfl_int == HI_TRUE) ? 1 : 0;

    DMX_WRITE_REG_BUF(mgmt->io_base, INT_SET(id), int_set.u32);

    DMX_COM_EQUAL(int_set.u32, DMX_READ_REG_BUF(mgmt->io_base, INT_SET(id)));
}

/***********************************************************************************
* Function      : DmxHalBufSetBaseIntTh
* Description  : Set the buffer base interrupt threshold
* Input          : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_set_base_int_th(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 base_ts_th, hi_u32 base_seop_th)
{
    U_BUF_INT_SET buf_int_set;

    WARN_ON(id >= mgmt->buf_cnt);

    buf_int_set.u32 = DMX_READ_REG_BUF(mgmt->io_base, BUF_INT_SET);

    buf_int_set.bits.ts_buf_int_set   = base_ts_th;
    buf_int_set.bits.seop_buf_int_set = base_seop_th;

    DMX_WRITE_REG_BUF(mgmt->io_base, BUF_INT_SET, buf_int_set.u32);

    DMX_COM_EQUAL(buf_int_set.u32, DMX_READ_REG_BUF(mgmt->io_base, BUF_INT_SET));
}

/***********************************************************************************
* Function      : DmxHalBufEnMqPes
* Description  : Enable mq and pes head upload
* Input          : BufId
* Output        :
* Return         :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_en_mq_pes(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_REG_STATE_RAM state_ram;

    WARN_ON(id >= mgmt->buf_cnt);

    state_ram.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));
    state_ram.bits.mq_en        = 1;   /* enable mq upload */
    state_ram.bits.pesd_en      = 1;   /* enable pesd upload */

    DMX_WRITE_REG_BUF(mgmt->io_base, REG_STATE_RAM(id), state_ram.u32);

    DMX_COM_EQUAL(state_ram.u32, DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id)));
}

/***********************************************************************************
* Function      : DmxHalBufDisMqPes
* Description  : Disable mq and pes head upload
* Input          : BufId
* Output        :
* Return         :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_dis_mq_pes(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_REG_STATE_RAM state_ram;

    WARN_ON(id >= mgmt->buf_cnt);

    state_ram.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));
    state_ram.bits.mq_en        = 0;   /* disable mq upload */
    state_ram.bits.pesd_en      = 0;   /* disable pesd upload */

    DMX_WRITE_REG_BUF(mgmt->io_base, REG_STATE_RAM(id), state_ram.u32);

    DMX_COM_EQUAL(state_ram.u32, DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id)));
}

/***********************************************************************************
* Function      : DmxHalBufGetDataState
* Description  : Get  Buf data type
* Input          : BufId
* Output        :
* Return         :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_buf_get_data_type(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_REG_STATE_RAM state_ram;

    WARN_ON(id >= mgmt->buf_cnt);

    state_ram.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));

    return state_ram.bits.state_data_type;
}

/***********************************************************************************
* Function      : DmxHalBufGetSeopAddr
* Description  : Get  seop address
* Input          : BufId
* Output        :
* Return         :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_buf_get_seop_addr(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_SEOP_ADDR seop_addr;

    WARN_ON(id >= mgmt->buf_cnt);

    seop_addr.u32 = DMX_READ_REG_BUF(mgmt->io_base, SEOP_ADDR(id));

    return seop_addr.bits.seop_addr;
}

hi_s32 dmx_hal_buf_set_tsio_ap(const struct dmx_mgmt *mgmt, hi_u32 chan_id, hi_bool enable, hi_u32 tsio_chan)
{
    U_DMX2TSIO_AP_SET set;

    if (chan_id >= 128) {
        HI_ERR_DEMUX("buf id(%u) is larger than 128, can't set tsio ap.\n", chan_id);
        return HI_FAILURE;
    }

    set.u32 = DMX_READ_REG_BUF(mgmt->io_base, DMX2TSIO_AP_SET(chan_id / 2));
    if ((chan_id % 2) == 0) {
        set.bits.dmx2tsio_ap_set_0 = (enable == HI_TRUE) ? (set.bits.dmx2tsio_ap_set_0 | (1 << tsio_chan)) :
            (set.bits.dmx2tsio_ap_set_0 & ~(1 << tsio_chan));
    } else {
        set.bits.dmx2tsio_ap_set_1 = (enable == HI_TRUE) ? (set.bits.dmx2tsio_ap_set_1 | (1 << tsio_chan)) :
            (set.bits.dmx2tsio_ap_set_1 & ~(1 << tsio_chan));
    }
    DMX_WRITE_REG_BUF(mgmt->io_base, DMX2TSIO_AP_SET(chan_id / 2), set.u32);

    return HI_SUCCESS;
}

hi_void dmx_hal_buf_pc_set_tsio_ap(const struct dmx_mgmt *mgmt, hi_u32 chan_id, hi_bool enable, hi_u32 tsio_chan)
{
    U_DMX2TSIO_PC_AP_SET set;

    set.u32 = DMX_READ_REG_BUF(mgmt->io_base, DMX2TSIO_PC_AP_SET(chan_id / 2));
    if ((chan_id % 2) == 0) {
        set.bits.dmx2tsio_pc_ap_set_0 = (enable == HI_TRUE) ? (set.bits.dmx2tsio_pc_ap_set_0 | (1 << tsio_chan)) :
            (set.bits.dmx2tsio_pc_ap_set_0 & ~(1 << tsio_chan));
    } else {
        set.bits.dmx2tsio_pc_ap_set_1 = (enable == HI_TRUE) ? (set.bits.dmx2tsio_pc_ap_set_1 | (1 << tsio_chan)) :
            (set.bits.dmx2tsio_pc_ap_set_1 & ~(1 << tsio_chan));
    }
    DMX_WRITE_REG_BUF(mgmt->io_base, DMX2TSIO_PC_AP_SET(chan_id / 2), set.u32);

    return;
}

hi_void dmx_hal_buf_clr_tsio_ap(const struct dmx_mgmt *mgmt, hi_u32 tsio_chan)
{
    U_DMX2TSIO_AP_CLEAR set;

    set.u32 = 0;
    set.bits.dmx2tsio_ap_clear |= (1 << tsio_chan);
    DMX_WRITE_REG_BUF(mgmt->io_base, DMX2TSIO_AP_CLEAR, set.u32);

    return;
}

hi_u32 dmx_hal_buf_get_pc_tsio_ap_status(const struct dmx_mgmt *mgmt, hi_u32 tsio_chan)
{
    U_DMX2TSIO_AP_FLG status;

    status.u32 = DMX_READ_REG_BUF(mgmt->io_base, DMX2TSIO_AP_FLG);

    return (status.bits.dmx2tsio_pc_ap_flg & (1 << tsio_chan));
}

hi_u32 dmx_hal_buf_get_tx_tsio_ap_status(const struct dmx_mgmt *mgmt, hi_u32 tsio_chan)
{
    U_DMX2TSIO_AP_FLG status;

    status.u32 = DMX_READ_REG_BUF(mgmt->io_base, DMX2TSIO_AP_FLG);

    return (status.bits.dmx2tsio_tx_ap_flg & (1 << tsio_chan));
}

/***********************************************************************************
* Function      : DmxHalMqSetAddrDepth
* Description  : Set mq desc address and depth
* Input          : BufId
* Output        :
* Return         :
* Others:
***********************************************************************************/
hi_void dmx_hal_mq_set_addr_depth(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 dsc_addr, hi_u32 dsc_depth)
{
    U_MQ_BUF mq_buf;
    U_MQ_SESSION mq_session;

    WARN_ON(id >= mgmt->buf_cnt);

    mq_buf.u32 = DMX_READ_REG_BUF(mgmt->io_base, MQ_BUF(id));

    mq_buf.bits.mqsa   = (dsc_addr & 0xFFFFFFFF) >> 12; /* 4k align  */
    mq_buf.bits.mqsize = dsc_depth - 1;  /* hw rule: -1. */

    DMX_WRITE_REG_BUF(mgmt->io_base, MQ_BUF(id), mq_buf.u32);
    DMX_COM_EQUAL(mq_buf.u32, DMX_READ_REG_BUF(mgmt->io_base, MQ_BUF(id)));

    mq_session.u32 = DMX_READ_REG_BUF(mgmt->io_base, MQ_SESSION);
    mq_session.bits.mq_session = (dsc_addr >> 32) & 0xF;

    DMX_WRITE_REG_BUF(mgmt->io_base, MQ_SESSION, mq_session.u32);
    DMX_COM_EQUAL(mq_session.u32, DMX_READ_REG_BUF(mgmt->io_base, MQ_SESSION));
}

/***********************************************************************************
* Function      : dmx_hal_mq_set_ap_th
* Description  : Set mq ap threshold
* Input          : BufId
* Output        :
* Return         :
* Others:
***********************************************************************************/
hi_void dmx_hal_mq_set_ap_th(const struct dmx_mgmt *mgmt, hi_u32 ap_th)
{
    U_MQ_AP_TH mq_ap_th;

    mq_ap_th.u32 = DMX_READ_REG_BUF(mgmt->io_base, MQ_AP_TH);

    if (ap_th < 88) {  /* the minimum of not ap threshold is 88 */
        ap_th = 88;
    }

    mq_ap_th.bits.mq_ap_th = ap_th;

    DMX_WRITE_REG_BUF(mgmt->io_base, MQ_AP_TH, mq_ap_th.u32);

    DMX_COM_EQUAL(mq_ap_th.u32, DMX_READ_REG_BUF(mgmt->io_base, MQ_AP_TH));
}

/***********************************************************************************
* Function      : DmxHalMqGetCurReadIdx
* Description  : Get current mq Read index
* Input          : BufId
* Output        :
* Return         :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_mq_get_cur_read_idx(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_MQ_PTR mq_ptr;

    WARN_ON(id >= mgmt->buf_cnt);

    mq_ptr.u32 = DMX_READ_REG_BUF(mgmt->io_base, MQ_PTR(id));

    return mq_ptr.bits.mqrptr;
}

/***********************************************************************************
* Function      : DmxHalMqSetReadIdx
* Description  : Set current Mq Read index
* Input          : BufId
* Output        :
* Return         :
* Others:
***********************************************************************************/
hi_void dmx_hal_mq_set_read_idx(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 read_idx)
{
    U_MQ_PTR mq_ptr;

    WARN_ON(id >= mgmt->buf_cnt);

    mq_ptr.u32 = DMX_READ_REG_BUF(mgmt->io_base, MQ_PTR(id));

    mq_ptr.bits.mqrptr = read_idx;

    DMX_WRITE_REG_BUF(mgmt->io_base, MQ_PTR(id), mq_ptr.u32);

    DMX_COM_EQUAL((mq_ptr.u32 & 0x00FFF000), (DMX_READ_REG_BUF(mgmt->io_base, MQ_PTR(id)) & 0x00FFF000));
}

/***********************************************************************************
* Function       : DmxHalMqGetCurWriteIdx
* Description   : Get current Mq Write index
* Input           : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_mq_get_cur_write_idx(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_MQ_PTR mq_ptr;

    WARN_ON(id >= mgmt->buf_cnt);
    /* attention: before read 'write' point, must read 'end' point */
    DMX_READ_REG_BUF(mgmt->io_base, MQ_PTR(id));

    mq_ptr.u32 = DMX_READ_REG_BUF(mgmt->io_base, MQ_PTR(id));

    return mq_ptr.bits.mqwptr;
}

/***********************************************************************************
* Function       : DmxHalMqConfig
* Description   : Config the Mq regitster
* Input           : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_mq_config(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 dsc_addr, hi_u32 dsc_depth)
{
    /* enable mq and pes head upload */
    dmx_hal_buf_en_mq_pes(mgmt, id);
    /* set the mq dec address and dscdepth */
    dmx_hal_mq_set_addr_depth(mgmt, id, dsc_addr, dsc_depth);
}

/***********************************************************************************
* Function       : DmxHalMqDeConfig
* Description   : DeConfig the Mq regitster
* Input           : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_mq_de_config(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    dmx_hal_buf_dis_mq_pes(mgmt, id);
    /* set the mq dec address and dscdepth */
    dmx_hal_mq_set_addr_depth(mgmt, id, 0, 0);
}

/***********************************************************************************
* Function      : DmxHalBufEnbale
* Description  : Eanble the buffer
* Input          : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_enable(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_REG_STATE_RAM state_ram;

    WARN_ON(id >= mgmt->buf_cnt);

    state_ram.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));

    state_ram.bits.state_ram_en = 1;

    DMX_WRITE_REG_BUF(mgmt->io_base, REG_STATE_RAM(id), state_ram.u32);

    DMX_COM_EQUAL(state_ram.u32, DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id)));
}

/***********************************************************************************
* Function      : DmxHalBufDisable
* Description  : Disable the buffer
* Input          : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_disable(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_REG_STATE_RAM state_ram;

    WARN_ON(id >= mgmt->buf_cnt);

    state_ram.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));

    state_ram.bits.state_ram_en = 0;

    DMX_WRITE_REG_BUF(mgmt->io_base, REG_STATE_RAM(id), state_ram.u32);

    /* only compare the state_ram_en, the 0 bit of the REG_STATE_RAM register */
    DMX_COM_EQUAL(state_ram.u32 & 0x1U, DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id)) & 0x1U);
}

/***********************************************************************************
* Function      : DmxHalBufSetBpTh
* Description  : Set the buffer backpress threshold
* Input          : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_set_ap_th(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 ap_th)
{
    U_INT_SET int_set;

    WARN_ON(id >= mgmt->buf_cnt);

    int_set.u32 = DMX_READ_REG_BUF(mgmt->io_base, INT_SET(id));

    int_set.bits.ap_th = ap_th;

    DMX_WRITE_REG_BUF(mgmt->io_base, INT_SET(id), int_set.u32);

    DMX_COM_EQUAL(int_set.u32, DMX_READ_REG_BUF(mgmt->io_base, INT_SET(id)));
}

/***********************************************************************************
* Function      : DmxHalBufSetStartAddr
* Description  : Set current Buf Start Address
* Input          : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_set_start_addr(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 start_addr)
{
    U_BUFSA buf_saddr;

    WARN_ON(id >= mgmt->buf_cnt);

    buf_saddr.u32 = DMX_READ_REG_BUF(mgmt->io_base, BUFSA(id));
    buf_saddr.bits.buf_session = (start_addr >> 32) & 0xF;
    buf_saddr.bits.bufsa = (start_addr & 0xFFFFFFFF) >> 12; /* 4k align  */

    DMX_WRITE_REG_BUF(mgmt->io_base, BUFSA(id), buf_saddr.u32);

    DMX_COM_EQUAL(buf_saddr.u32, DMX_READ_REG_BUF(mgmt->io_base, BUFSA(id)));
}

/***********************************************************************************
* Function      : dmx_hal_buf_set_buf_size
* Description  : Set current buf Size
* Input          : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_set_buf_size(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 buf_size)
{
    U_BUFSIZE rbuf_size;

    WARN_ON(id >= mgmt->buf_cnt);

    rbuf_size.u32 = DMX_READ_REG_BUF(mgmt->io_base, BUFSIZE(id));

    rbuf_size.bits.bufsize = (buf_size >> 12) - 1; /* 4k align , hw rule: -1. */

    DMX_WRITE_REG_BUF(mgmt->io_base, BUFSIZE(id), rbuf_size.u32);

    DMX_COM_EQUAL(rbuf_size.u32, DMX_READ_REG_BUF(mgmt->io_base, BUFSIZE(id)));
}

/***********************************************************************************
* Function      : DmxHalBufGetCurReadIdx
* Description  : Get current buf Read index
* Input          : BufId
* Output        :
* Return         :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_buf_get_cur_read_idx(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_BUFRPTR buf_rptr;

    WARN_ON(id >= mgmt->buf_cnt);

    buf_rptr.u32 = DMX_READ_REG_BUF(mgmt->io_base, BUFRPTR(id));

    return buf_rptr.bits.bufrptr;
}

/***********************************************************************************
* Function      : DmxHalBufSetCurReadIdx
* Description  : Set current buf Read index
* Input          : BufId
* Output        :
* Return         :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_set_read_idx(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 read_idx)
{
    U_BUFRPTR buf_rptr;

    WARN_ON(id >= mgmt->buf_cnt);

    buf_rptr.u32 = DMX_READ_REG_BUF(mgmt->io_base, BUFRPTR(id));

    buf_rptr.bits.bufrptr = read_idx;

    DMX_WRITE_REG_BUF(mgmt->io_base, BUFRPTR(id), buf_rptr.u32);

    DMX_COM_EQUAL(buf_rptr.u32, DMX_READ_REG_BUF(mgmt->io_base, BUFRPTR(id)));
}

/***********************************************************************************
* Function      : DmxHalBufGetCurWriteIdx
* Description   : Get current buf Write index
* Input         : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_buf_get_cur_write_idx(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_BUFWPTR buf_wptr;

    WARN_ON(id >= mgmt->buf_cnt);
    /* attention: before read 'write' point, must read 'end' point */
    DMX_READ_REG_BUF(mgmt->io_base, BUFEPTR(id));

    buf_wptr.u32 = DMX_READ_REG_BUF(mgmt->io_base, BUFWPTR(id));

    return buf_wptr.bits.bufwptr;
}

/***********************************************************************************
* Function      : DmxHalBufGetCurEndIdx
* Description   : Get buf current end index
* Input         : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_buf_get_cur_end_idx(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    U_BUFEPTR buf_eptr;

    WARN_ON(id >= mgmt->buf_cnt);

    buf_eptr.u32 = DMX_READ_REG_BUF(mgmt->io_base, BUFEPTR(id));

    return buf_eptr.bits.bufeptr;
}

/***********************************************************************************
* Function      : DmxHalBufSetBufSecAttrs
* Description   : Set buf secure attr
* Input         : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_set_buf_sec_attrs(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool secure)
{
    U_BUF_SET buf_set;

    WARN_ON(id >= mgmt->buf_cnt);

    buf_set.u32 = DMX_READ_REG_BUF(mgmt->io_base, BUF_SET(id));

    if (secure == HI_TRUE) {
        buf_set.bits.buf_lock     = 0x1;   /* lock */
        buf_set.bits.buf_sec_attr = 0x5;   /* secure */
    } else {
        buf_set.bits.buf_lock     = 0x0;   /* unlock */
        buf_set.bits.buf_sec_attr = 0xA;   /* non secure */
    }

    DMX_WRITE_REG_BUF(mgmt->io_base, BUF_SET(id), buf_set.u32);

    DMX_COM_EQUAL(buf_set.u32, DMX_READ_REG_BUF(mgmt->io_base, BUF_SET(id)));
}

/***********************************************************************************
* Function      : DmxHalBufOpen
* Description  : Open the buffer
* Input          : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_open(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u32 time_int_th, hi_u32 ts_cnt_int_th,
                         hi_u32 seop_int_th)
{
    /* set the timoutth, ts_th and seop_th */
    if (time_int_th > 0) {
        /* 4, 3, 1 for other occasion */
        dmx_hal_buf_set_int_th(mgmt, id, 4, ts_cnt_int_th, seop_int_th, HI_TRUE, HI_TRUE);
    } else {
        /* 0,0,1 for secpes and avpes, 0,0,0 for ts  */
        dmx_hal_buf_set_int_th(mgmt, id, 0, ts_cnt_int_th, seop_int_th, HI_FALSE, HI_TRUE);
    }

    /* set the base ts_buf_int_set 128 for record buffer(128 * ts_int_th(2) * 188 = 47KB) and seop_th as 1 */
    dmx_hal_buf_set_base_int_th(mgmt, id, 128, 1);

    /* configure the read index */
    dmx_hal_buf_set_read_idx(mgmt, id, 0);

    /* enable the buffer */
    dmx_hal_buf_enable(mgmt, id);
}

/***********************************************************************************
* Function      : DmxHalBufClose
* Description  : Close the buffer
* Input          : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_close(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    /* set the timoutth, ts_th and seop_th */
    dmx_hal_buf_set_int_th(mgmt, id, 0, 0, 0, HI_FALSE, HI_FALSE);
    /* set the base ts_th and seop_th */
    dmx_hal_buf_set_base_int_th(mgmt, id, 0, 0);

    /* disable the buffer */
    dmx_hal_buf_disable(mgmt, id);
}

/***********************************************************************************
* Function      : DmxHalBufConfig
* Description  : total configure Buf
* Input          : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_config(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 buf_start_addr, hi_u32 buf_size,
    hi_bool secure)
{
    /* configure the buffer start address */
    if (secure != HI_TRUE) { /* secure buffer will be configured in tee side */
        dmx_hal_buf_set_start_addr(mgmt, id, buf_start_addr);

        /* configure the buffer size */
        dmx_hal_buf_set_buf_size(mgmt, id, buf_size);

        /* configure the secure attr */
        dmx_hal_buf_set_buf_sec_attrs(mgmt, id, secure);

       /* configure the read index, just for 96cv300 ES chipset */
        dmx_hal_buf_set_read_idx(mgmt, id, 0);
    }

    /* configure buffer ap threshold  16K, must more than 12KB */
    dmx_hal_buf_set_ap_th(mgmt, id, BUF_AP_THRESHOLD >> 11);
}

/***********************************************************************************
* Function      : DmxHalBufDeConfig
* Description  : total configure Buf
* Input          : PortId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_buf_de_config(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 buf_start_addr, hi_u32 buf_size,
    hi_bool secure)
{
    /* close for ctrl+c */
    /* disable the buffer */
    dmx_hal_buf_disable(mgmt, id);

    /* set the timoutth, ts_th and seop_th */
    dmx_hal_buf_set_int_th(mgmt, id, 0, 0, 0, HI_FALSE, HI_FALSE);
    /* set the base ts_th and seop_th */
    dmx_hal_buf_set_base_int_th(mgmt, id, 0, 0);

    if (secure != HI_TRUE) { /* secure buffer will be configured in tee side */
        /* configure the buffer start address */
        dmx_hal_buf_set_start_addr(mgmt, id, buf_start_addr);

        /* configure the buffer size */
        dmx_hal_buf_set_buf_size(mgmt, id, buf_size);

        /* configure the secure attr */
        dmx_hal_buf_set_buf_sec_attrs(mgmt, id, HI_FALSE);

        /* configure the read index, just for 96cv300 ES chipset */
        dmx_hal_buf_set_read_idx(mgmt, id, 0);
    }
    /* deconfigure buffer ap threshold */
    dmx_hal_buf_set_ap_th(mgmt, id, 0);
}

/***********************************************************************************
* Function      : DmxHalPidCopyBufLock
* Description  : Lock PidCopy Buf
* Input          : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_copy_buf_lock(const struct dmx_mgmt *mgmt)
{
    U_PC_BUF_ID_LOCK pcbuf_lock;

    pcbuf_lock.u32 = DMX_READ_REG_BUF(mgmt->io_base, PC_BUF_ID_LOCK);

    pcbuf_lock.bits.pc_buf_id_lock = 0x1;

    DMX_WRITE_REG_BUF(mgmt->io_base, PC_BUF_ID_LOCK, pcbuf_lock.u32);

    DMX_COM_EQUAL(pcbuf_lock.u32, DMX_READ_REG_BUF(mgmt->io_base, PC_BUF_ID_LOCK));
}

/***********************************************************************************
* Function      : DmxHalPidCopyBufSetId
* Description  : PidCopy Buf set id register
* Input          : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_copy_buf_set_id(const struct dmx_mgmt *mgmt, hi_u32 pcid)
{
    U_PC_BUF_ID_SET pcbuf_setid;
    WARN_ON(pcid >= mgmt->pid_copy_chan_cnt);

    pcbuf_setid.u32 = DMX_READ_REG_BUF(mgmt->io_base, PC_BUF_ID_SET);

    pcbuf_setid.bits.pc_buf_id = pcid;

    DMX_WRITE_REG_BUF(mgmt->io_base, PC_BUF_ID_SET, pcbuf_setid.u32);

    DMX_COM_EQUAL(pcbuf_setid.u32, DMX_READ_REG_BUF(mgmt->io_base, PC_BUF_ID_SET));
}

/***********************************************************************************
* Function      : DmxHalPidCopyBufUnLock
* Description  : Lock PidCopy Buf
* Input          : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_copy_buf_un_lock(const struct dmx_mgmt *mgmt)
{
    U_PC_BUF_ID_LOCK pcbuf_lock;

    pcbuf_lock.u32 = DMX_READ_REG_BUF(mgmt->io_base, PC_BUF_ID_LOCK);

    pcbuf_lock.bits.pc_buf_id_lock = 0x0;

    DMX_WRITE_REG_BUF(mgmt->io_base, PC_BUF_ID_LOCK, pcbuf_lock.u32);

    DMX_COM_EQUAL(pcbuf_lock.u32, DMX_READ_REG_BUF(mgmt->io_base, PC_BUF_ID_LOCK));
}

/***********************************************************************************
* Function      : DmxHalPidCopyBufSetStartAddr
* Description  : Set PidCopy Buf Start Address
* Input          : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_copy_buf_set_start_addr(const struct dmx_mgmt *mgmt, hi_u64 start_addr)
{
    U_PC_BUFSA pcbuf_saddr;

    pcbuf_saddr.u32 = DMX_READ_REG_BUF(mgmt->io_base, PC_BUFSA);

    pcbuf_saddr.bits.pc_session = (start_addr >> 32U) & 0xFU;
    pcbuf_saddr.bits.pc_bufsa = (start_addr >> 12U) & 0xFFFFFU; /* 4k align  */

    DMX_WRITE_REG_BUF(mgmt->io_base, PC_BUFSA, pcbuf_saddr.u32);

    DMX_COM_EQUAL(pcbuf_saddr.u32, DMX_READ_REG_BUF(mgmt->io_base, PC_BUFSA));
}

/***********************************************************************************
* Function      : DmxHalBufSetBufSize
* Description  : Set PidCopy Buf Size
* Input          : BufId
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_copy_buf_set_buf_size(const struct dmx_mgmt *mgmt, hi_u32 buf_size)
{
    U_PC_STATE_RAM buf_state;

    buf_state.u32 = DMX_READ_REG_BUF(mgmt->io_base, PC_STATE_RAM);

    buf_state.bits.pc_bufsize = (buf_size >> 12) - 1; /* 4k align , hw rule: -1. */

    DMX_WRITE_REG_BUF(mgmt->io_base, PC_STATE_RAM, buf_state.u32);

    DMX_COM_EQUAL(buf_state.u32, DMX_READ_REG_BUF(mgmt->io_base, PC_STATE_RAM));
}

/***********************************************************************************
* Function      : DmxHalBufGetCurReadIdx
* Description  : Get current pid copy buf Read index
* Input          : BufId
* Output        :
* Return         :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_pid_copy_get_cur_read_idx(const struct dmx_mgmt *mgmt, hi_u32 pcid)
{
    U_PC_BUFRPTR pcbuf_rptr;

    WARN_ON(pcid >= mgmt->pid_copy_chan_cnt);

    dmx_hal_pid_copy_buf_un_lock(mgmt);
    /* set the pidcopy buf id */
    dmx_hal_pid_copy_buf_set_id(mgmt, pcid);
    /* lock */
    dmx_hal_pid_copy_buf_lock(mgmt);

    pcbuf_rptr.u32 = DMX_READ_REG_BUF(mgmt->io_base, PC_BUFRPTR);

    return pcbuf_rptr.bits.pc_bufrptr;
}

/***********************************************************************************
* Function      : DmxHalBufGetCurReadIdx
* Description  : Get current pid copy buf Read index
* Input          : BufId
* Output        :
* Return         :
* Others:
***********************************************************************************/
hi_u32 dmx_hal_pid_copy_get_cur_write_idx(const struct dmx_mgmt *mgmt, hi_u32 pcid)
{
    U_PC_BUFWPTR pcbuf_wptr;

    WARN_ON(pcid >= mgmt->pid_copy_chan_cnt);

    dmx_hal_pid_copy_buf_un_lock(mgmt);
    /* set the pidcopy buf id */
    dmx_hal_pid_copy_buf_set_id(mgmt, pcid);
    /* lock */
    dmx_hal_pid_copy_buf_lock(mgmt);

    pcbuf_wptr.u32 = DMX_READ_REG_BUF(mgmt->io_base, PC_BUFWPTR);

    return pcbuf_wptr.bits.pc_bufwptr;
}

/***********************************************************************************
* Function      : DmxHalPidCopyTxEnSet
* Description  : Enable or disable Tx PidCopy
* Input          :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_copy_tx_en_set(const struct dmx_mgmt *mgmt, hi_bool tx_pc_en)
{
    U_PC_STATE_RAM buf_state;

    buf_state.u32 = DMX_READ_REG_BUF(mgmt->io_base, PC_STATE_RAM);

    buf_state.bits.pc_state_ram_en = HI_TRUE == tx_pc_en ? 1 : 0;
    buf_state.bits.pc_ovfl_int_en = HI_TRUE == tx_pc_en ? 1 : 0;

    DMX_WRITE_REG_BUF(mgmt->io_base, PC_STATE_RAM, buf_state.u32);

    DMX_COM_EQUAL(buf_state.u32, DMX_READ_REG_BUF(mgmt->io_base, PC_STATE_RAM));
}

/***********************************************************************************
* Function      : DmxHalPidCopySecEnSet
* Description  : Enable or disable Secure attri PidCopy
* Input          :  1: no secure, 0:secure
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_copy_sec_en_set(const struct dmx_mgmt *mgmt, hi_u32 pcid, hi_bool sec_pc_en)
{
    U_PC_BUFCFG buf_cfg;
    DMX_UNUSED(pcid);

    buf_cfg.u32 = DMX_READ_REG_BUF(mgmt->io_base, PC_BUFCFG);

    if (sec_pc_en == HI_TRUE) {
        buf_cfg.bits.pc_lock = 0x1;     /* lock */
        buf_cfg.bits.pc_sec_attr = 0x5; /* secure */
    } else {
        buf_cfg.bits.pc_lock = 0x0;     /* unlock */
        buf_cfg.bits.pc_sec_attr = 0xA; /* non secure */
    }

    DMX_WRITE_REG_BUF(mgmt->io_base, PC_BUFCFG, buf_cfg.u32);

    DMX_COM_EQUAL(buf_cfg.u32, DMX_READ_REG_BUF(mgmt->io_base, PC_BUFCFG));
}

/***********************************************************************************
* Function      : DmxHalPidCopyRxEnSet
* Description  : Enable or disable Rx PidCopy
* Input          :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_copy_rx_en_set(const struct dmx_mgmt *mgmt, hi_u32 pcid, hi_bool rx_pc_en)
{
    hi_u32 reg_index;
    hi_u32 bit_offset;
    hi_u32 reg;

    WARN_ON(pcid >= mgmt->pid_copy_chan_cnt);

    reg_index  = pcid / BITS_PER_REG;
    bit_offset = pcid % BITS_PER_REG;

    reg = DMX_READ_REG_BUF(mgmt->io_base, PC_RX_EN0(reg_index));
    if (rx_pc_en == HI_TRUE) {
        reg |= 1U << bit_offset;
    } else {
        reg &= ~(1U << bit_offset);
    }
    DMX_WRITE_REG_BUF(mgmt->io_base, PC_RX_EN0(reg_index), reg);

    DMX_COM_EQUAL(reg, DMX_READ_REG_BUF(mgmt->io_base, PC_RX_EN0(reg_index)));
}

/***********************************************************************************
* Function      : DmxHalPidCopyApEnSet
* Description  : Enable or disable Ap to IP of PidCopy
* Input          : ApTh: KB for anti pressure
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_copy_ap_en_set(const struct dmx_mgmt *mgmt, hi_u32 ap_th)
{
    U_PC_AP_TH reg;

    reg.u32 = DMX_READ_REG_BUF(mgmt->io_base, PC_AP_TH);

    reg.bits.pc_ap_th = ap_th;

    DMX_WRITE_REG_BUF(mgmt->io_base, PC_AP_TH, reg.u32);
}

/******************************************************************************
* Function      :  DmxHalPidCopyGetApStatus
* Description   :  Get the pid copy ap status of Ramport
* Input         :
* Output        :
* Return        :
* Others:       :
*******************************************************************************/
hi_u32 dmx_hal_pid_copy_get_raw_ap_ram_int_status(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    hi_u32 reg;

    WARN_ON(id >= mgmt->ram_port_cnt);

    reg = DMX_READ_REG_BUF(mgmt->io_base, RAW_PC_AP_IP_INT);

    return (reg & (1U << id));
}

/******************************************************************************
* Function      :  DmxHalPidCopyClsApStatus
* Description   :  Clear the pid copy ap status of Ramport
* Input         :
* Output        :
* Return        :
* Others:       :
*******************************************************************************/
hi_void dmx_hal_pid_copy_cls_ap_ram_int_status(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    hi_u32 reg;
    hi_u32 reg_clearend;

    WARN_ON(id >= mgmt->ram_port_cnt);

    reg = (1U << id);
    reg_clearend = reg & ~(1U << id);

    DMX_WRITE_REG_BUF(mgmt->io_base, RAW_PC_AP_IP_INT, reg);

    DMX_COM_EQUAL(reg_clearend, DMX_READ_REG_BUF(mgmt->io_base, RAW_PC_AP_IP_INT));
}

/******************************************************************************
* Function      :  DmxHalPidCopyClsApStatus
* Description   :  Clear the ap status of Pid Copy
* Input         : PortId
* Output        :
* Return        :
* Others:       :
*******************************************************************************/
hi_void dmx_hal_pid_copy_cls_ap_status(const struct dmx_mgmt *mgmt, hi_u32 pcid)
{
    hi_u32 reg_clearend;
    hi_u32 reg_index;
    hi_u32 bit_offset;
    hi_u32 reg;

    WARN_ON(pcid >= mgmt->pid_copy_chan_cnt);

    reg_index  = pcid / BITS_PER_REG;
    bit_offset = pcid % BITS_PER_REG;

    reg = (1U << bit_offset);
    reg_clearend = reg & ~(1U << bit_offset);

    DMX_WRITE_REG_BUF(mgmt->io_base, AP_CLEAR0(reg_index), reg);

    DMX_COM_EQUAL(reg_clearend, DMX_READ_REG_BUF(mgmt->io_base, AP_CLEAR0(reg_index)));
}

hi_void dmx_hal_pid_copy_clear_chan(const struct dmx_mgmt *mgmt, hi_u32 pcid)
{
    hi_s32 i = 0;
    hi_u32 reg_index;
    hi_u32 bit_offset;
    hi_u32 reg;
    U_PC_CLR_CHN_CFG reg_clr;

    WARN_ON(pcid >= mgmt->pid_copy_chan_cnt);

    reg_clr.u32 = DMX_READ_REG_DAV(mgmt->io_base, PC_CLR_CHN_CFG);

    reg_clr.bits.pc_clr_chn_id = pcid;
    reg_clr.bits.pc_clr_chn_start = 1;

    DMX_WRITE_REG_DAV(mgmt->io_base, PC_CLR_CHN_CFG, reg_clr.u32);

    reg_index  = pcid / BITS_PER_REG;
    bit_offset = pcid % BITS_PER_REG;

    reg = DMX_READ_REG_DAV(mgmt->io_base, RAW_PC0_CLR_INT(reg_index));
    while (((reg & (1U << bit_offset)) != (1U << bit_offset)) && (i++ <= DMX_CHAN_CLEAR_TIMEOUT_CNT)) {
        msleep(1);
        reg = DMX_READ_REG_DAV(mgmt->io_base, RAW_PC0_CLR_INT(reg_index));
    }

    if (i >= DMX_CHAN_CLEAR_TIMEOUT_CNT) {
        HI_ERR_DEMUX("pid copy clear chan time out, id(%u).\n", pcid);
    }

    reg = (1U << bit_offset);
    DMX_WRITE_REG_DAV(mgmt->io_base, RAW_PC0_CLR_INT(reg_index), reg);

    return;
}

hi_void dmx_hal_dav_tx_clear_chan(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    hi_s32 i = 0;
    U_PRS_CLR_CHN_CFG chan_cfg;
    U_REG_STATE_RAM ram_state;

    chan_cfg.u32 = DMX_READ_REG_BUF(mgmt->io_base, PRS_CLR_CHN_CFG);

    chan_cfg.bits.prs_clr_chn_id = id;
    chan_cfg.bits.prs_clr_chn_start = 1;

    DMX_WRITE_REG_BUF(mgmt->io_base, PRS_CLR_CHN_CFG, chan_cfg.u32);

    ram_state.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));
    while ((ram_state.bits.raw_prs_clr_int != 1) && (i++ <= DMX_CHAN_CLEAR_TIMEOUT_CNT)) {
        msleep(1);
        ram_state.u32 = DMX_READ_REG_BUF(mgmt->io_base, REG_STATE_RAM(id));
    }

    if (i >= DMX_CHAN_CLEAR_TIMEOUT_CNT) {
        HI_ERR_DEMUX("dav tx clear chan time out, id(%u).\n", id);
    }

    ram_state.bits.raw_prs_clr_int = 1;
    DMX_WRITE_REG_BUF(mgmt->io_base, REG_STATE_RAM(id), ram_state.u32);

    return;
}


/***********************************************************************************
* Function      : DmxHalPidCopyBufConfig
* Description  : Configure the Pid Copy
* Input          :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_copy_config(const struct dmx_mgmt *mgmt, hi_u32 id, hi_u64 start_addr, hi_u32 buf_size)
{
    /* attention: here must the sequence: unlock->set pidcopyid->lock */
    /* unlock */
    dmx_hal_pid_copy_buf_un_lock(mgmt);
    /* set the pidcopy buf id */
    dmx_hal_pid_copy_buf_set_id(mgmt, id);
    /* lock */
    dmx_hal_pid_copy_buf_lock(mgmt);

    /* configure the pid copy buffer start address */
    dmx_hal_pid_copy_buf_set_start_addr(mgmt, start_addr);

    /* configure the pid copy buffer  size */
    dmx_hal_pid_copy_buf_set_buf_size(mgmt, buf_size);

    /* configure the pid copy ap as  6KB size, set once is OK */
    dmx_hal_pid_copy_ap_en_set(mgmt, 6);

    /* configure the pid copy as none secure attribute */
    dmx_hal_pid_copy_sec_en_set(mgmt, id, HI_FALSE);

    /* enable the rx pid copy featuer */
    dmx_hal_pid_copy_rx_en_set(mgmt, id, HI_TRUE);

    /* enable the tx pid copy featuer */
    dmx_hal_pid_copy_tx_en_set(mgmt, HI_TRUE);
}

/***********************************************************************************
* Function      : DmxHalPidDeCopyConfig
* Description  : DeConfigure the Pid Copy
* Input          :
* Output        :
* Return        :
* Others:
***********************************************************************************/
hi_void dmx_hal_pid_copy_de_config(const struct dmx_mgmt *mgmt, hi_u32 id)
{
    /* attention: here must the sequence: unlock->set pidcopyid->lock */
    /* unlock */
    dmx_hal_pid_copy_buf_un_lock(mgmt);
    /* set the pidcopy buf id */
    dmx_hal_pid_copy_buf_set_id(mgmt, id);
    /* lock */
    dmx_hal_pid_copy_buf_lock(mgmt);

    /* deconfigure the pid copy buffer start address */
    dmx_hal_pid_copy_buf_set_start_addr(mgmt, 0);

    /* deconfigure the pid copy buffer  size */
    dmx_hal_pid_copy_buf_set_buf_size(mgmt, 0);

    /* disable the tx pid copy featuer */
    dmx_hal_pid_copy_tx_en_set(mgmt, HI_FALSE);

    /* disable the rx pid copy featuer */
    dmx_hal_pid_copy_rx_en_set(mgmt, id, HI_FALSE);
}

/***********************************************************************************
* Function       : DmxHalMDSCSetEvenOdd
* Description   : set mdsc iv/cw key even or odd when encrypted
* Input           :
* Output         :
* Return         :
* Others:
***********************************************************************************/
hi_void dmx_hal_mdscset_encrypt_even_odd(const struct dmx_mgmt *mgmt, hi_u32 id, dmx_dsc_key_type even_odd)
{
    hi_u32 reg_index;
    hi_u32 bit_offset;
    hi_u32 reg;

    WARN_ON(id >= mgmt->dsc_fct_cnt);

    reg_index  = id / BITS_PER_REG;
    bit_offset = id % BITS_PER_REG;

    reg = DMX_READ_REG_MDSC(mgmt->mdsc_base, KEY_ENCRPTY_SEL(reg_index));

    if (even_odd == DMX_DSC_KEY_ODD) {
        reg |= 1 << bit_offset;
    } else {
        reg &= ~(1 << bit_offset);
    }

    DMX_WRITE_REG_MDSC(mgmt->mdsc_base, KEY_ENCRPTY_SEL(reg_index), reg);

    DMX_COM_EQUAL(reg, DMX_READ_REG_MDSC(mgmt->mdsc_base, KEY_ENCRPTY_SEL(reg_index)));
}

/***********************************************************************************
* Function       : DmxHalSetEntropyReduction
* Description   : Set the valid bits of EntropyReduction
* Input           :0:48 bit mode(EntropyReduction);1:64bit mode(bypass)
* Output         :
* Return         :
* Others:
***********************************************************************************/
hi_void dmx_hal_mdscset_entropy_reduction(const struct dmx_mgmt *mgmt, hi_u32 id, dmx_dsc_entropy entropy_reduction)
{
    hi_u32 reg;
    hi_u32 reg_index;
    hi_u32 bit_offset;

    WARN_ON(id >= mgmt->dsc_fct_cnt);

    /* CSA2 ENTROPY CLOSE register only support CW0~63 */
    if (id >= 64) {
        HI_ERR_DEMUX("Do not support such cw! id = %u\n", id);
        return;
    }

    reg_index  = id / BITS_PER_REG;
    bit_offset = id % BITS_PER_REG;

    reg = DMX_READ_REG_MDSC(mgmt->mdsc_base, CSA2_ENTROPY_CLOSE(reg_index));
    if (entropy_reduction == DMX_CA_ENTROPY_CLOSE) {
        reg |= 1 << bit_offset;
    } else {
        reg &= ~(1 << bit_offset);
    }

    DMX_WRITE_REG_MDSC(mgmt->mdsc_base, CSA2_ENTROPY_CLOSE(reg_index), reg);

    DMX_COM_EQUAL(reg, DMX_READ_REG_MDSC(mgmt->mdsc_base, CSA2_ENTROPY_CLOSE(reg_index)));
}

/***********************************************************************************
* Function       : DmxHalMDSCSetEn
* Description   : Set the Mdsc Enable register
* Input           :
* Output         :
* Return         :
* Others:
***********************************************************************************/
hi_void dmx_hal_mdscset_en(const struct dmx_mgmt *mgmt, hi_bool ca_en, hi_bool ts_ctrl_dsc_change_en, hi_bool cw_iv_en)
{
    U_MDSC_EN reg;

    reg.u32 = DMX_READ_REG_MDSC(mgmt->mdsc_base, MDSC_EN);

    reg.bits.ca_en                 = ca_en ? 1 : 0;
    reg.bits.ts_ctrl_dsc_change_en = ts_ctrl_dsc_change_en  ? 1 : 0;
    reg.bits.cw_iv_en              = cw_iv_en ? 1 : 0;

    DMX_WRITE_REG_MDSC(mgmt->mdsc_base, MDSC_EN, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_MDSC(mgmt->mdsc_base, MDSC_EN));
}

/***********************************************************************************
* Function       : DmxHalMDSCDisCpdCore
* Description   : The cpd core disable register
* Input           : HI_FALSE:not disable core,HI_TRUE:disable core
* Output         :
* Return         :
* Others:
***********************************************************************************/
hi_void dmx_hal_mdscdis_cpd_core(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool dis_core)
{
    U_MDSC_CPD_CORE_DISABLE reg;

    WARN_ON(id >= mgmt->dsc_fct_cnt);

    reg.u32 = DMX_READ_REG_MDSC(mgmt->mdsc_base, MDSC_CPD_CORE_DISABLE);

    reg.bits.cpd_core_disable = dis_core ? ~(1 << 8) : 0;

    DMX_WRITE_REG_MDSC(mgmt->mdsc_base, MDSC_CPD_CORE_DISABLE, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_MDSC(mgmt->mdsc_base, MDSC_CPD_CORE_DISABLE));
}

/***********************************************************************************
* Function       : DmxHalMDSCDisCaCore
* Description   : The ca core disable register
* Input           : HI_FALSE:not disable core,HI_TRUE:disable core
* Output         :
* Return         :
* Others:
***********************************************************************************/
hi_void dmx_hal_mdscdis_ca_core(const struct dmx_mgmt *mgmt, hi_bool dis_core)
{
    U_MDSC_CA_CORE_DISABLE reg;

    reg.u32 = DMX_READ_REG_MDSC(mgmt->mdsc_base, MDSC_CA_CORE_DISABLE);

    reg.bits.ca_core_disable = dis_core ? ~(1 << 28) : 0;

    DMX_WRITE_REG_MDSC(mgmt->mdsc_base, MDSC_CA_CORE_DISABLE, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_MDSC(mgmt->mdsc_base, MDSC_CA_CORE_DISABLE));
}

/***********************************************************************************
* Function       : DmxHalMDSCDisCpsCore
* Description   : The cps core disable register
* Input           : HI_FALSE:not disable core,HI_TRUE:disable core
* Output         :
* Return         :
* Others:
***********************************************************************************/
hi_void dmx_hal_mdscdis_cps_core(const struct dmx_mgmt *mgmt, hi_u32 id, hi_bool dis_core)
{
    U_MDSC_CPS_CORE_DISABLE reg;

    WARN_ON(id >= mgmt->dsc_fct_cnt);

    reg.u32 = DMX_READ_REG_MDSC(mgmt->mdsc_base, MDSC_CPS_CORE_DISABLE);

    reg.bits.cps_core_disable = dis_core ? ~(1 << 8) : 0;

    DMX_WRITE_REG_MDSC(mgmt->mdsc_base, MDSC_CPS_CORE_DISABLE, reg.u32);

    DMX_COM_EQUAL(reg.u32, DMX_READ_REG_MDSC(mgmt->mdsc_base, MDSC_CPS_CORE_DISABLE));
}

hi_void dmx_hal_mdsc_multi2_sys_key_cfg(const struct dmx_mgmt *mgmt, hi_u8 *key, hi_u32 key_len)
{
    /* multi2 system key must be 32 Bytes */
    if (key_len != DMX_SYS_KEY_LEN) {
        HI_ERR_DEMUX("multi2 key len is not correct! key_len=%u Bytes\n", key_len);
        return;
    }
    /* write multi2 sys key0 */
    DMX_WRITE_REG_MDSC(mgmt->mdsc_base, MULTI2_SYS_KEY0, *(hi_u32*)key);

    /* write multi2 sys key1 */
    DMX_WRITE_REG_MDSC(mgmt->mdsc_base, MULTI2_SYS_KEY1, *(hi_u32*)(key + 4));

    /* write multi2 sys key2 */
    DMX_WRITE_REG_MDSC(mgmt->mdsc_base, MULTI2_SYS_KEY2, *(hi_u32*)(key + 8));

    /* write multi2 sys key3 */
    DMX_WRITE_REG_MDSC(mgmt->mdsc_base, MULTI2_SYS_KEY3, *(hi_u32*)(key + 12));

    /* write multi2 sys key4 */
    DMX_WRITE_REG_MDSC(mgmt->mdsc_base, MULTI2_SYS_KEY4, *(hi_u32*)(key + 16));

    /* write multi2 sys key5 */
    DMX_WRITE_REG_MDSC(mgmt->mdsc_base, MULTI2_SYS_KEY5, *(hi_u32*)(key + 20));

    /* write multi2 sys key6 */
    DMX_WRITE_REG_MDSC(mgmt->mdsc_base, MULTI2_SYS_KEY6, *(hi_u32*)(key + 24));

    /* write multi2 sys key7 */
    DMX_WRITE_REG_MDSC(mgmt->mdsc_base, MULTI2_SYS_KEY7, *(hi_u32*)(key + 28));
}

