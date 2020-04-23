/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq tool define
 * Author: pq
 * Create: 2016-01-1
 */

#include "drv_pq_table.h"
#include "drv_pq.h"
#include "pq_hal_comm.h"

static hi_bool pq_tools_is_sel_vdp_reg(hi_u32 reg_addr)
{
    hi_u32 offset_addr = reg_addr & REG_OFFSET_ADDR_MASK;

    if ((reg_addr & REG_BASE_ADDR_MASK) != VDP_REGS_ADDR) {
        return HI_FALSE;
    }

    if ((offset_addr >= 0xD800) && (offset_addr <= 0xEE7C)) {
        return HI_TRUE;
    }

    return HI_FALSE;
}

static hi_s32 pq_tools_update_phy_list(hi_u32 addr, hi_u8 lsb, hi_u8 msb, hi_u8 source_mode, hi_u8 output_mode,
                                       hi_u32 value)
{
    hi_u32 i;
    hi_u32 phy_list_num = pq_table_get_phy_list_num();
    pq_bin_phy_reg *phy_reg = pq_table_get_phy_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(phy_reg);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(phy_list_num, PHY_REG_MAX);

    for (i = 0; i < phy_list_num; i++) {
        if ((addr & REG_OFFSET_ADDR_MASK) != phy_reg[i].reg_addr) {
            continue;
        }
        if (lsb != phy_reg[i].lsb) {
            continue;
        }
        if (msb != phy_reg[i].msb) {
            continue;
        }
        if ((phy_reg[i].source_mode != PQ_SOURCE_MODE_NO) && (source_mode != phy_reg[i].source_mode)) {
            continue;
        }
        if ((phy_reg[i].output_mode != PQ_SOURCE_MODE_NO) && (output_mode != phy_reg[i].output_mode)) {
            continue;
        }

        phy_reg[i].value = value;
        return i;
    }

    return HI_FAILURE;
}

static hi_s32 pq_tools_set_soft_table(hi_u32 lut, pq_source_mode source_mode, pq_output_mode output_mode,
                                      hi_u32 value)
{
    hi_u32 i;
    hi_u32 addr;
    hi_u8 source_mode_tmp, output_mode_tmp;
    hi_u32 soft_list_num = pq_table_get_soft_list_num();
    pq_bin_phy_reg *soft_reg = pq_table_get_soft_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(soft_reg);
    PQ_CHECK_OVER_RANGE_RE_FAIL(source_mode, PQ_SOURCE_MODE_MAX);
    PQ_CHECK_OVER_RANGE_RE_FAIL(output_mode, PQ_OUTPUT_MODE_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(soft_list_num, SOFT_REG_MAX);

    for (i = 0; i < soft_list_num; i++) {
        addr = soft_reg[i].reg_addr;
        source_mode_tmp = soft_reg[i].source_mode;
        output_mode_tmp = soft_reg[i].output_mode;

        if (addr != lut) {
            continue;
        }

        if ((source_mode_tmp != PQ_SOURCE_MODE_NO) && (source_mode_tmp != source_mode)) {
            continue;
        }

        if ((output_mode_tmp != PQ_OUTPUT_MODE_NO) && (output_mode_tmp != output_mode)) {
            continue;
        }

        soft_reg[i].value = value;
        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

hi_s32 pq_tools_set_prepare(hi_pq_register *attr)
{
    hi_s32 ret = HI_SUCCESS;
    hi_s32 num;
    pq_source_mode source_mode = pq_comm_get_source_mode();
    pq_output_mode output_mode = pq_comm_get_output_mode();
    pq_bin_phy_reg *phy_reg = pq_table_get_phy_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(phy_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(attr);

    if (pq_hal_is_special_reg(attr->reg_addr) == HI_SUCCESS) {
        return pq_tools_set_soft_table(
                   attr->reg_addr, attr->source_mode, attr->output_mode, attr->value);
    }

    num = pq_tools_update_phy_list(
              attr->reg_addr, attr->lsb, attr->msb, attr->source_mode, attr->output_mode, attr->value);

    if (num == HI_FAILURE) {
        PQPRINT(PQ_PRN_TABLE, "warning! not find register[address:0x%x, bit:%u~%u],source_mode:[%u],output_mode:[%u]\n",
                attr->reg_addr, attr->lsb, attr->msb, attr->source_mode, attr->output_mode);
    } else {
        if ((phy_reg[num].source_mode != PQ_SOURCE_MODE_NO)
            && (attr->source_mode != source_mode)) {
            HI_ERR_PQ("current source mode:[%d], set source mode:[%u],not set physical reg\n",
                      source_mode, attr->source_mode);
            return HI_SUCCESS;
        }

        if ((phy_reg[num].output_mode != PQ_OUTPUT_MODE_NO)
            && (attr->output_mode != output_mode)) {
            HI_ERR_PQ("current output mode:[%d], set output mode:[%u],not set physical reg\n",
                      output_mode, attr->output_mode);
            return HI_SUCCESS;
        }
    }

    return ret;
}
hi_s32 pq_tools_set_reg(hi_pq_register *attr)
{
    hi_s32 ret = HI_SUCCESS;

    /* PQ tool may pass a not 4bytes align addr */
    if (HI_FALSE == REGADDR_IS_ALIGN_4BYTE(attr->reg_addr)) {
        HI_ERR_PQ("reg_addr is not align to 4 bytes! %x\n", attr->reg_addr);
        return HI_FAILURE;
    }

    if (pq_tools_is_sel_vdp_reg(attr->reg_addr) == HI_TRUE) {
        return pq_table_set_sel_vdp_reg(
                   attr->reg_addr, attr->lsb, attr->msb, attr->value);
    }

    if (pq_hal_is_vpss_reg(attr->reg_addr) == HI_SUCCESS) {
        ret = pq_table_set_vpss_reg(
                  attr->reg_addr, attr->lsb, attr->msb, attr->value);
    } else if (pq_hal_is_vdp_reg(attr->reg_addr) == HI_SUCCESS) {
        ret = pq_table_set_vdp_reg(
                  attr->reg_addr, attr->lsb, attr->msb, attr->value);
    } else {
        HI_ERR_PQ("not VPSS/VDP/special register!\n");
        return HI_FAILURE;
    }

    return ret;
}
hi_s32 pq_tools_set_register(hi_pq_register *attr)
{
    hi_s32 ret;

    ret = pq_tools_set_prepare(attr);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_tools_set_prepare(attr);

    return ret;
}

hi_s32 pq_tools_get_register(hi_pq_register *attr)
{
    hi_u32 addr, value;
    hi_u8 lsb, msb, source_mode, output_mode;
    hi_u32 i;
    hi_u32 phy_list_num = pq_table_get_phy_list_num();
    pq_bin_phy_reg *phy_reg = pq_table_get_phy_reg();

    PQ_CHECK_NULL_PTR_RE_FAIL(phy_reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(attr);
    PQ_CHECK_NUM_OVER_RANGE_RE_FAIL(phy_list_num, PHY_REG_MAX);

    if (pq_hal_is_special_reg(attr->reg_addr) == HI_SUCCESS) {
        attr->value = pq_table_get_soft_table(attr->reg_addr, attr->source_mode, attr->output_mode, 0);
        return HI_SUCCESS;
    }

    for (i = 0; i < phy_list_num; i++) {
        addr = phy_reg[i].reg_addr;
        value = phy_reg[i].value;
        lsb = phy_reg[i].lsb;
        msb = phy_reg[i].msb;
        source_mode = phy_reg[i].source_mode;
        output_mode = phy_reg[i].output_mode;

        if (addr != (attr->reg_addr & REG_OFFSET_ADDR_MASK)) {
            continue;
        }

        if (lsb != attr->lsb) {
            continue;
        }

        if (msb != attr->msb) {
            continue;
        }

        if ((source_mode != PQ_SOURCE_MODE_NO) && (source_mode != attr->source_mode)) {
            continue;
        }

        if ((output_mode != PQ_OUTPUT_MODE_NO) && (output_mode != attr->output_mode)) {
            continue;
        }

        attr->value = value;
        return HI_SUCCESS;
    }

    HI_ERR_PQ("error! not find phy register list[address:0x%x, bit:%u~%u],source_mode:[%u], output_mode:[%u]\n",
              attr->reg_addr, attr->lsb, attr->msb, attr->source_mode, attr->output_mode);

    return HI_FAILURE;
}

hi_s32 pq_tools_get_bin_phy_addr(hi_u64 *addr)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(addr);

    drv_pq_get_bin_phy_addr(addr);

    return HI_SUCCESS;
}
