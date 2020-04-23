/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq reg common api
 * Author: pq
 * Create: 2016-01-1
 */

#include "hi_osal.h"
#include "pq_hal_comm.h"
#include "hi_drv_mem.h"

/* VPSS/VDP 本地寄存器，用于初始化参数 */
static vpss_reg_type *g_vpss_reg_local = NULL;

/* VPSS/VDP 虚拟寄存器，用于配置参数 */
static vpss_reg_type *g_vpss_reg_mem[VPSS_HANDLE_NUM] = {NULL};
vdp_regs_type *g_vdp_reg_logic_addr = 0;
vdp_regs_type *g_vdp_reg_logic_set = HI_NULL;
static hi_drv_pq_dbg_vpss_vir_soft *g_pq_vpss_alog_vir_buf = HI_NULL;

static hi_u8 *g_pq_vdp_stt_reg_vir_addr = HI_NULL;

#define PQ_VDP_REG_SIZE sizeof(vdp_regs_type)

/* PQ HAL初始化标志位 */
static hi_bool g_is_hal_init = HI_FALSE;

/* 打印使能开关 */
static hi_u32 g_print_type = PQ_PRN_NOTHING;

/* 打印中间结果的暂存区域 */
#define PRINT_MSG_LEN_MAX 1024
static hi_s8 g_print_msg[PRINT_MSG_LEN_MAX];

/*
 * brief 初始化HAL模块，申请本地寄存器内存;
 * attention
 * param[in] : NA
 * retval :HI_SUCCESS
 */

hi_void pq_comm_all_buf_deinit(hi_void)
{
    pq_kfree_safe(g_vpss_reg_local);

    if (g_vdp_reg_logic_addr != HI_NULL) {
        osal_iounmap(g_vdp_reg_logic_addr);
        g_vdp_reg_logic_addr = HI_NULL;
        g_vdp_reg_logic_set = HI_NULL;
    }

    pq_kfree_safe(g_pq_vpss_alog_vir_buf);

    g_is_hal_init = HI_FALSE;

    return;
}

hi_s32 pq_comm_init(hi_void)
{
    if (g_is_hal_init == HI_TRUE) {
        return HI_SUCCESS;
    }

    g_vpss_reg_local = (vpss_reg_type *)pq_kmalloc(sizeof(vpss_reg_type));
    if (g_vpss_reg_local == HI_NULL) {
        HI_ERR_PQ("sg_pst_vpss_reg_local can not kmalloc!\n");

        g_is_hal_init = HI_FALSE;

        return HI_FAILURE;
    }

    PQ_SAFE_MEMSET(g_vpss_reg_local, 0, sizeof(vpss_reg_type));

    /* add real reg set  */
    g_vdp_reg_logic_addr = osal_ioremap_nocache(VDP_REGS_ADDR, PQ_VDP_REG_SIZE);
    if (g_vdp_reg_logic_addr == HI_NULL) {
        HI_ERR_PQ("g_pstVdpRegLogicAddr remap failed !\n");
        pq_comm_all_buf_deinit();
        return HI_FAILURE;
    }
    g_vdp_reg_logic_set = (vdp_regs_type *)g_vdp_reg_logic_addr;

    g_pq_vpss_alog_vir_buf = (hi_drv_pq_dbg_vpss_vir_soft *)pq_kmalloc(sizeof(hi_drv_pq_dbg_vpss_vir_soft));
    if (g_pq_vpss_alog_vir_buf == HI_NULL) {
        HI_ERR_PQ("g_pq_vpss_alog_vir_buf kmalloc failed !\n");
        pq_comm_all_buf_deinit();
        return HI_FAILURE;
    }

    memset(g_pq_vpss_alog_vir_buf, 0, sizeof(hi_drv_pq_dbg_vpss_vir_soft));

    g_is_hal_init = HI_TRUE;

    return HI_SUCCESS;
}

/*
 * brief 去初始化HAL模块,释放本地寄存器内存;
 * attention
 * param[in] : NA
 * retval :HI_SUCCESS
 */
hi_s32 pq_comm_deinit(hi_void)
{
    hi_u32 i;

    if (g_is_hal_init == HI_FALSE) {
        return HI_SUCCESS;
    }

    for (i = 0; i < VPSS_HANDLE_NUM; i++) {
        g_vpss_reg_mem[i] = NULL;
    }

    pq_comm_all_buf_deinit();

    return HI_SUCCESS;
}
hi_void pq_hal_copy_reg_by_size(hi_u32 *dst_reg, hi_u32 *src_reg, hi_u32 reg_size)
{
    hi_u32 i = 0;

    for (i = 0; i < reg_size; i++) {
        pq_hal_reg_write((hi_u32 *)(dst_reg + i), (*(src_reg + i)));
    }

    return;
}

/*
 * brief 更新VPSS PQ;
 * attention
 * param[in] *p_dst_vpss_addr  :目标地址
 * param[in] *p_src_vpss_addr  :源地址
 * retval :HI_SUCCESS
 */
hi_void pq_hal_update_vpss_pq(vpss_reg_type *dst_vpss_addr, vpss_reg_type *src_vpss_addr)
{
    PQ_CHECK_NULL_PTR_RE_NULL(dst_vpss_addr);
    PQ_CHECK_NULL_PTR_RE_NULL(src_vpss_addr);

    return;
}

/*
 * brief 更新VDP PQ;
 * attention
 * param[in] *p_dst_vdp_addr  :目标地址
 * param[in] *p_src_vdp_addr  :源地址
 * retval :HI_SUCCESS
 */
hi_void pq_hal_update_vdp_pq(vdp_regs_type *dst_vdp_addr, vdp_regs_type *src_vdp_addr)
{
    PQ_CHECK_NULL_PTR_RE_NULL(dst_vdp_addr);
    PQ_CHECK_NULL_PTR_RE_NULL(src_vdp_addr);

    return;
}

/*
 * brief 更新VPSS寄存器;
 * attention
 * param[in] handle_id  :VPSS通道号
 * param[in] *pst_vpss_reg  :由驱动传过来的寄存器指针
 * retval :HI_SUCCESS
 */
hi_s32 pq_hal_update_vpss_reg(hi_u32 handle_id, vpss_reg_type *vpss_reg)
{
    hi_bool reload = HI_FALSE;

    PQ_CHECK_OVER_RANGE_RE_FAIL(handle_id, VPSS_HANDLE_NUM);

    if (g_vpss_reg_mem[handle_id] != vpss_reg) {
        g_vpss_reg_mem[handle_id] = vpss_reg;
        reload = HI_TRUE;
    }

    if (g_vpss_reg_mem[handle_id] != NULL && reload == HI_TRUE) {
        pq_hal_update_vpss_pq(g_vpss_reg_mem[handle_id], g_vpss_reg_local);
    }

    return HI_SUCCESS;
}

hi_s32 pq_hal_check_vpss_valid(hi_u32 handle_id)
{
    PQ_CHECK_OVER_RANGE_RE_FAIL(handle_id, VPSS_HANDLE_NUM);

    /* NULL means reg_mem has been clear */
    if (g_vpss_reg_mem[handle_id] == NULL) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

vpss_reg_type *pq_hal_get_vpss_reg(hi_u32 handle_id)
{
    vpss_reg_type *vpss_reg = HI_NULL;

    if (handle_id >= VPSS_HANDLE_NUM) {
        HI_ERR_PQ("VPSS handle_id=%u is over range!\n", handle_id);
        return NULL;
    }

    if (g_vpss_reg_mem[handle_id] == NULL) {
        if (g_is_hal_init == HI_TRUE) {
            vpss_reg = g_vpss_reg_local;
        } else {
            vpss_reg = HI_NULL;
        }
    } else {
        vpss_reg = g_vpss_reg_mem[handle_id];
    }

    return vpss_reg;
}

/*
 * brief 获取VDP当前寄存器;
 * attention
 * param[in] : NA
 * retval :VDP寄存器指针
 */

hi_s32 pq_hal_write_register(hi_u32 handle_id, hi_u32 reg_addr, hi_u32 value)
{
    vpss_reg_type *vpss_reg = HI_NULL;
    vdp_regs_type *vdp_reg = HI_NULL;
    hi_u32 offset_addr;

    PQ_CHECK_OVER_RANGE_RE_FAIL(handle_id, VPSS_HANDLE_NUM);

    offset_addr = reg_addr & REG_OFFSET_ADDR_MASK;

    if ((pq_hal_is_vpss_reg(reg_addr) == HI_SUCCESS) && (handle_id < VPSS_HANDLE_NUM)) {
        vpss_reg = pq_hal_get_vpss_reg(handle_id);

        if (vpss_reg == HI_NULL) {
            HI_ERR_PQ("VPSS REG is null point!\n");
            return HI_FAILURE;
        }

        *(hi_u32 *)((hi_void *)vpss_reg + offset_addr) = value;
        if ((offset_addr >= PQ_DEI_ME_ADDR_START) && (offset_addr <= PQ_DEI_ME_ADDR_END)) {
            *(hi_u32 *)((hi_void *)vpss_reg + offset_addr + 1 * PQ_DEI_ME_OFFSET) = value;
            *(hi_u32 *)((hi_void *)vpss_reg + offset_addr + 2 * PQ_DEI_ME_OFFSET) = value; /* 2:num */
            *(hi_u32 *)((hi_void *)vpss_reg + offset_addr + 3 * PQ_DEI_ME_OFFSET) = value; /* 3:num */
            *(hi_u32 *)((hi_void *)vpss_reg + offset_addr + 4 * PQ_DEI_ME_OFFSET) = value; /* 4:num */
        }
    } else if (pq_hal_is_vdp_reg(reg_addr) == HI_SUCCESS) {
        vdp_reg = pq_hal_get_vdp_set_reg();
        PQ_CHECK_NULL_PTR_RE_FAIL(vdp_reg);

        *(hi_u32 *)((hi_void *)vdp_reg + offset_addr) = value;
    } else {
    }

    return HI_SUCCESS;
}

/* 读寄存器 */
hi_s32 pq_hal_read_register(hi_u32 handle_id, hi_u32 reg_addr, hi_u32 *value)
{
    vpss_reg_type *vpss_reg = HI_NULL;
    vdp_regs_type *vdp_reg = HI_NULL;
    hi_u32 offset_addr;

    PQ_CHECK_OVER_RANGE_RE_FAIL(handle_id, VPSS_HANDLE_NUM);

    offset_addr = reg_addr & REG_OFFSET_ADDR_MASK;

    if ((pq_hal_is_vpss_reg(reg_addr) == HI_SUCCESS) && (handle_id < VPSS_HANDLE_NUM)) {
        vpss_reg = pq_hal_get_vpss_reg(handle_id);

        if (vpss_reg == NULL) {
            HI_ERR_PQ("VPSS REG is null point!\n");
            return HI_FAILURE;
        }

        *value = *((hi_u32 *)((hi_void *)vpss_reg + offset_addr));
    } else if (pq_hal_is_vdp_reg(reg_addr) == HI_SUCCESS) {
        vdp_reg = pq_hal_get_vdp_set_reg();
        PQ_CHECK_NULL_PTR_RE_FAIL(vdp_reg);

        *value = *((hi_u32 *)((hi_void *)vdp_reg + offset_addr));
    } else {
    }

    return HI_SUCCESS;
}

/* 按照起始BIT位复制U32参数 */
hi_void pq_hal_copy_u32_by_bit(hi_u32 src, hi_u32 src_start_bit, hi_u32 *dst, hi_u32 dst_start_bit, hi_u32 num)
{
    hi_u32 src_tmp;
    hi_u32 dst_tmp;
    hi_u32 dst_tmp_low;
    hi_u32 dst_tmp_high;

    PQ_CHECK_NUM_OVER_RANGE_RE_NULL(src_start_bit + num, PQ_BIT_LEN_MAX);
    PQ_CHECK_NUM_OVER_RANGE_RE_NULL(dst_start_bit + num, PQ_BIT_LEN_MAX);

    /* 保存src需要拷贝的num个bits，并移动到dst指定的位置; 将高位清0 */
    src_tmp = src << (PQ_BIT_LEN_MAX - src_start_bit - num);

    /* 将指定的num位移到低bit位上 */
    src_tmp = src_tmp >> (PQ_BIT_LEN_MAX - num);
    src_tmp = src_tmp << dst_start_bit;

    /* 保存dst的0bit到dst_start_bit位 */
    dst_tmp_low = *dst << (PQ_BIT_LEN_MAX - dst_start_bit); /* 将高位清0 */
    dst_tmp_low = dst_tmp_low >> (PQ_BIT_LEN_MAX - dst_start_bit);

    /* 保存dst的dst_start_bit + num位到32bit */
    dst_tmp_high = *dst >> (dst_start_bit + num);
    dst_tmp_high = dst_tmp_high << (dst_start_bit + num);

    /* 计算结果 */
    dst_tmp = dst_tmp_high | src_tmp | dst_tmp_low;
    *dst = dst_tmp;

    return;
}

/*
 * brief 根据bit位更新U32参数
 * attention
 * param[in] pul_data,uc_bit_no;
 * retval :HI_SUCCESS
 */
hi_void pq_hal_u32_set_bit(hi_u32 *pul_data, hi_u8 bit_no)
{
    hi_u32 bits_mask, data;

    PQ_CHECK_NUM_OVER_RANGE_RE_NULL(bit_no, PQ_BIT_LEN_MAX - 1);

    /* bits mask */
    bits_mask = 0x000;
    bits_mask = bits_mask << bit_no;

    data = *pul_data;
    *pul_data = data | bits_mask;
}

/*
 * brief 将U32对应的bit位清零
 * attention
 * param[in] pul_data,uc_bit_no;
 * retval :HI_SUCCESS
 */
hi_void pq_hal_u32_clear_bit(hi_u32 *pul_data, hi_u8 bit_no)
{
    hi_u32 bits_mask, data;

    PQ_CHECK_NUM_OVER_RANGE_RE_NULL(bit_no, PQ_BIT_LEN_MAX - 1);

    /* bits mask */
    bits_mask = 0x000;
    bits_mask = bits_mask << bit_no;

    data = *pul_data;
    *pul_data = data & (~bits_mask);
}

/*
 * brief 从U32参数根据最高位和最低获取对应的值
 * attention
 * param[in]  ul_data,  uc_max_bit,  uc_min_bit;
 * retval :hi_u32
 */
hi_u32 pq_hal_get_u32_by_bit(hi_u32 data, hi_u8 max_bit, hi_u8 min_bit)
{
    hi_u8 flag_er = HI_FALSE;

    if (max_bit < min_bit) {
        flag_er = HI_TRUE;
    }

    if (max_bit > (PQ_BIT_LEN_MAX - 1) || min_bit > (PQ_BIT_LEN_MAX - 1)) {
        flag_er = HI_TRUE;
    }

    if (flag_er == HI_TRUE) {
        HI_ERR_PQ("ntdriver_bits_u32_get_data bit over range!\n");
        return flag_er;
    }

    data = data << (PQ_BIT_LEN_MAX - 1 - max_bit);
    data = data >> (PQ_BIT_LEN_MAX - 1 - max_bit);
    data = (data >> min_bit);
    return data;
}

/*
 * brief 根据最高位和最低位设置U32参数对应的值
 * attention
 * param[in] pul_data, uc_max_bit, uc_min_bit, ul_value;
 * retval :none
 */
hi_void pq_hal_set_u32_by_bit(hi_u32 *pul_data, hi_u8 max_bit, hi_u8 min_bit, hi_u32 value)
{
    hi_u8 flag_er = HI_FALSE;
    hi_u32 bits_mask, data;

    if (max_bit < min_bit) {
        flag_er = HI_TRUE;
    }

    if (max_bit > (PQ_BIT_LEN_MAX - 1) || min_bit > (PQ_BIT_LEN_MAX - 1)) {
        flag_er = HI_TRUE;
    }

    if (flag_er == HI_TRUE) {
        HI_ERR_PQ("bit over range:maxbit[%u],minbit[%u]\n", max_bit, min_bit);
        return;
    }

    /* bits mask */
    bits_mask = 0xFFFFFFFF;
    bits_mask = bits_mask >> min_bit;
    bits_mask = bits_mask << (PQ_BIT_LEN_MAX - 1 - max_bit + min_bit);
    bits_mask = bits_mask >> (PQ_BIT_LEN_MAX - 1 - max_bit);

    data = *pul_data;
    data = data & (~bits_mask);
    value = (value << min_bit) & bits_mask;
    *pul_data = data | value;
}

hi_s32 pq_hal_is_vpss_reg(hi_u32 reg_addr)
{
    if ((reg_addr & REG_BASE_ADDR_MASK) == VPSS_REGS_ADDR) {
        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

hi_s32 pq_hal_is_vdp_reg(hi_u32 reg_addr)
{
    if ((reg_addr & REG_BASE_ADDR_MASK) == VDP_REGS_ADDR) {
        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

hi_s32 pq_hal_is_special_reg(hi_u32 reg_addr)
{
    if ((reg_addr & REG_BASE_ADDR_MASK) == SPECIAL_REGS_ADDR) {
        return HI_SUCCESS;
    }

    return HI_FAILURE;
}

hi_s32 pq_hal_print_msg(hi_u32 type, const hi_s8 *format, ...)
{
    va_list args;
    hi_s32 n_total_char;

    /* 信息类型过滤 */
    if ((type != PQ_PRN_ALWS) && ((g_print_type & type) == 0)) { /* 该类别打印没有打开 */
        return HI_FAILURE;
    }

    /* 将信息打印成字符串 */
    va_start(args, format);
    n_total_char = vsnprintf(g_print_msg, sizeof(g_print_msg), format, args);
    va_end(args);

    if ((n_total_char <= 0) || (n_total_char >= (PRINT_MSG_LEN_MAX - 1))) {
        return HI_FAILURE;
    }

#ifdef PQ_PROC_CTRL_SUPPORT
    HI_PRINT("%s", g_print_msg);
#endif

    return HI_SUCCESS;
}

hi_s32 pq_hal_set_print_type(hi_u32 type)
{
    g_print_type = type;
    return HI_SUCCESS;
}

hi_void pq_hal_reg_write(volatile hi_u32 *reg_addr, hi_u32 value)
{
    *reg_addr = value;

    return;
}

hi_u32 pq_hal_reg_read(volatile hi_u32 *reg_addr)
{
    return (*reg_addr);
}

hi_u32 pq_new_regread(uintptr_t reg_addr)
{
    return *((volatile hi_u32 *)reg_addr);
}

hi_u32 pq_hal_vpss_reg_read(volatile hi_u32 *a)
{
    return (*(a));
}

hi_void pq_hal_vpss_reg_write(volatile hi_u32 *a, hi_u32 b)
{
    *a = b;
}

hi_void pq_new_regwrite(uintptr_t reg_addr, hi_u32 value)
{
    *((volatile hi_u32 *)reg_addr) = value;
    return;
}

vdp_regs_type *pq_hal_get_vdp_set_reg(hi_void)
{
    vdp_regs_type *vdp_reg = HI_NULL;
    if ((g_is_hal_init == HI_TRUE) && (g_vdp_reg_logic_set != HI_NULL)) {
        vdp_reg = g_vdp_reg_logic_set;
    } else {
        vdp_reg = HI_NULL;
    }

    return vdp_reg;
}

hi_drv_pq_dbg_vpss_vir_soft *pq_hal_get_vpss_vir_soft_reg(hi_void)
{
    if (g_is_hal_init == HI_TRUE) {
        return g_pq_vpss_alog_vir_buf;
    } else {
        return HI_NULL;
    }

    return HI_NULL;
}

static hi_void pq_reg_comm_set_stt_regup(vdp_regs_type *vdp_reg, hi_u32 regup)
{
    u_stt_rupd stt_rupd;

    stt_rupd.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stt_rupd.u32)));
    stt_rupd.bits.stt_rupd = regup;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->stt_rupd.u32)), stt_rupd.u32);

    return;
}

static hi_void pq_reg_comm_set_stt_en(vdp_regs_type *vdp_reg, hi_u32 stt_en)
{
    u_stt_ctrl stt_ctrl;

    stt_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stt_ctrl.u32)));
    stt_ctrl.bits.stt_en = stt_en;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->stt_ctrl.u32)), stt_ctrl.u32);

    return;
}

static hi_void pq_reg_comm_set_stt_addr_low(vdp_regs_type *vdp_reg, hi_u32 addr)
{
    u_stt_addr_l stt_addr_l;

    stt_addr_l.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stt_addr_l.u32)));
    stt_addr_l.bits.stt_addr_l = addr;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->stt_addr_l.u32)), stt_addr_l.u32);

    return;
}

static hi_void pq_reg_comm_set_stt_addr_high(vdp_regs_type *vdp_reg, hi_u32 addr)
{
    u_stt_addr_h stt_addr_h;

    stt_addr_h.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stt_addr_h.u32)));
    stt_addr_h.bits.stt_addr_h = addr;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->stt_addr_h.u32)), stt_addr_h.u32);

    return;
}

static hi_void pq_reg_comm_set_stt_mmu_bypass(vdp_regs_type *vdp_reg, hi_u32 mmu_bypass)
{
    u_stt_smmu_ctrl stt_smmu_ctrl;

    stt_smmu_ctrl.u32 = pq_new_regread((uintptr_t)(&(vdp_reg->stt_smmu_ctrl.u32)));
    stt_smmu_ctrl.bits.stt_bypass = mmu_bypass;
    pq_new_regwrite((uintptr_t)(&(vdp_reg->stt_smmu_ctrl.u32)), stt_smmu_ctrl.u32);

    return;
}

hi_s32 pq_hal_comm_vdp_stt_init(hi_u8 *vir_addr, dma_addr_t phy_addr)
{
    vdp_regs_type *vdp_reg = HI_NULL;

    if (vir_addr == HI_NULL || phy_addr == HI_NULL) {
        HI_ERR_PQ("vir_addr->%#x or phy_addr->%#lx invalid!\n", vir_addr, phy_addr);
        return HI_ERR_PQ_INVALID_PARA;
    }

    vdp_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_reg);

    pq_hal_set_vdp_stt_reg_vir_addr(vir_addr);

    pq_reg_comm_set_stt_en(vdp_reg, HI_TRUE);
    pq_reg_comm_set_stt_mmu_bypass(vdp_reg, HI_TRUE);
    pq_reg_comm_set_stt_addr_low(vdp_reg, (hi_u64)phy_addr);
    pq_reg_comm_set_stt_addr_high(vdp_reg, 0x0);
    return HI_SUCCESS;
}

hi_s32 pq_hal_comm_vdp_stt_update(hi_void)
{
    vdp_regs_type *vdp_reg = HI_NULL;
    vdp_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_reg);

    pq_reg_comm_set_stt_regup(vdp_reg, HI_TRUE);

    return HI_SUCCESS;
}

hi_void pq_hal_set_vdp_stt_reg_vir_addr(hi_u8 *vir_addr)
{
    g_pq_vdp_stt_reg_vir_addr = vir_addr;
    return;
}

hi_u8 *pq_hal_get_vdp_stt_reg_vir_addr(hi_void)
{
    return g_pq_vdp_stt_reg_vir_addr;
}
