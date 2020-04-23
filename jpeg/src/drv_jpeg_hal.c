/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: jpeg reg parse operation
 */

#include "drv_jpeg_hal.h"
#ifndef HI_BUILD_IN_BOOT
#include "hi_gfx_comm_k.h"
#else
#include <common.h>
#include "hi_common.h"
#endif
#include "hi_debug.h"
#include "drv_jpeg_reg.h"
#include "hi_drv_jpeg.h"

#define JPED_STREAM_BUFFER_ALIGN 128
#define DRV_JPEG_Y_COMPONENT_NUM 0
#define DRV_JPEG_U_COMPONENT_NUM 1
#define DRV_JPEG_V_COMPONENT_NUM 2

#ifdef HI_BUILD_IN_BOOT
#define JPEG_CFG_CLOCK_DELAY_TIME_MS 1000
#endif

static volatile hi_u32 *g_reg_base_address = NULL;
static inline hi_void jpeg_cfg_dqt(hi_jpeg_reg_info *reg_info);
static inline hi_void jpeg_cfg_dht(hi_jpeg_reg_info *reg_info);
static inline hi_void jpeg_cfg_sof(hi_jpeg_reg_info *reg_info);
static inline hi_void jpeg_cfg_output_height(hi_jpeg_reg_info *reg_info);
static hi_void jpeg_cfg_fac(hi_jpeg_reg_info *reg_info);
static inline hi_void jpeg_cfg_dri(hi_jpeg_reg_info *reg_info);
static hi_void jpeg_cfg_mem_type(hi_void);
static hi_void jpeg_cfg_stream_buf(hi_jpeg_reg_info *reg_info);
static hi_void jpeg_cfg_input_stream_buf(hi_u64 start_address, hi_u64 end_address);
static hi_void jpeg_cfg_save_input_stream_buf(hi_u64 start_address, hi_u64 end_address);
static hi_void jpeg_cfg_out_buf(hi_jpeg_reg_info *reg_info);
static hi_void jpeg_cfg_frame_dec_start(hi_jpeg_reg_info *reg_info);
static inline hi_void jpeg_cfg_yuv420sp(hi_jpeg_reg_info *reg_info);
static hi_void jpeg_get_save_stream_buf(hi_jpeg_reg_info *reg_info, hi_u64 *start_save_stream_phy_buf,
                                        hi_u64 *end_save_stream_phy_buf);
static hi_void jpeg_cfg_dec_out_yuv_buf(hi_u64 y_dst_phy_addr, hi_u64 uv_dst_phy_addr);
static hi_void jpeg_get_yuv_out_buf(hi_jpeg_reg_info *reg_info, hi_u64 *y_dst_phy_addr, hi_u64 *uv_dst_phy_addr);

hi_u32 jpeg_reg_read(hi_u32 offset)
{
    if ((g_reg_base_address == NULL) || (offset >= (JPGD_REG_LENGTH - JPEG_REG_CFG_FOUR_BYTES))) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer or offset:%d\n", __FUNCTION__, __LINE__, offset);
        return 0;
    }

    return *((volatile hi_u32 *)(g_reg_base_address + (offset / JPEG_REG_CFG_FOUR_BYTES)));
}

hi_void jpeg_reg_write(hi_u32 offset, hi_u64 data)
{
    if ((g_reg_base_address == NULL) || (offset >= (JPGD_REG_LENGTH - JPEG_REG_CFG_FOUR_BYTES))) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer or offset:%d\n", __FUNCTION__, __LINE__, offset);
        return;
    }
    *((volatile hi_u32 *)(g_reg_base_address + (offset / JPEG_REG_CFG_FOUR_BYTES))) = data;
}

hi_void jpeg_reg_cpy_data(const hi_char *data_buf, hi_u32 data_size, hi_u32 offset)
{
    hi_s32 i;

    if ((g_reg_base_address == NULL) || (data_buf == NULL) ||
        ((data_size + offset) >= (JPGD_REG_LENGTH - JPEG_REG_CFG_FOUR_BYTES))) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer or data_size:%d offset:%d\n", __FUNCTION__, __LINE__,
                 data_size, offset);
        return;
    }

    for (i = 0; i < data_size; i += JPEG_REG_CFG_FOUR_BYTES) {
        *((volatile hi_u32 *)(g_reg_base_address + ((offset + i) / JPEG_REG_CFG_FOUR_BYTES))) =
            *((hi_u32 *)(data_buf + i));
    }

    return;
}

hi_void jpeg_set_reg_base(volatile hi_u32 *reg_base)
{
    g_reg_base_address = reg_base;
}

volatile hi_u32 *jpeg_get_reg_base(hi_void)
{
    return g_reg_base_address;
}

hi_void drv_jpeg_start_decompress(hi_jpeg_reg_info *reg_info)
{
    jpeg_cfg_dqt(reg_info);
    jpeg_cfg_dht(reg_info);
    jpeg_cfg_sof(reg_info);
    jpeg_cfg_output_height(reg_info);
    jpeg_cfg_fac(reg_info);
    jpeg_cfg_dri(reg_info);
    jpeg_cfg_mem_type();
    jpeg_cfg_stream_buf(reg_info);
    jpeg_cfg_out_buf(reg_info);
    jpeg_cfg_yuv420sp(reg_info);
    jpeg_cfg_frame_dec_start(reg_info);
    return;
}

hi_void drv_jpeg_dev_get_status(hi_u32 *status)
{
    if (status != NULL) {
        *status = jpeg_reg_read(JPGD_REG_INT);
    }
    return;
}

hi_void drv_jpeg_dev_clear_status(hi_u32 status)
{
    jpeg_reg_write(JPGD_REG_INT, status);
    return;
}

static inline hi_void jpeg_cfg_yuv420sp(hi_jpeg_reg_info *reg_info)
{
    jpeg_reg_write(JPGD_REG_OUTTYPE, reg_info->output_color_space);
}

static inline hi_void jpeg_cfg_dqt(hi_jpeg_reg_info *reg_info)
{
    jpeg_reg_cpy_data((const hi_char *)reg_info->reg_quant_table, sizeof(reg_info->reg_quant_table), JPGD_REG_QUANT);
}

static inline hi_void jpeg_cfg_dht(hi_jpeg_reg_info *reg_info)
{
    jpeg_reg_cpy_data((const hi_char *)reg_info->reg_dc_table, sizeof(reg_info->reg_dc_table), JPGD_REG_HDCTABLE);

    jpeg_reg_cpy_data((const hi_char *)reg_info->reg_ac_min_table, sizeof(reg_info->reg_ac_min_table),
                      JPGD_REG_HACMINTABLE);

    jpeg_reg_cpy_data((const hi_char *)reg_info->reg_ac_base_table, sizeof(reg_info->reg_ac_base_table),
                      JPGD_REG_HACBASETABLE);

    /* 0x2c0 = ac_max_sum_syms * 4  * */
    jpeg_reg_cpy_data((const hi_char *)reg_info->reg_ac_symbol_table, 0x2c0, JPGD_REG_HACSYMTABLE);
}

static inline hi_void jpeg_cfg_sof(hi_jpeg_reg_info *reg_info)
{
    hi_u32 reg_picture_size;
    hi_u32 reg_dec_stride;
    jpeg_reg_write(JPGD_REG_PICTYPE, reg_info->jpeg_color_space);
    jpeg_reg_write(JPGD_REG_SCALE, reg_info->scale);

    reg_picture_size = reg_info->image_mcu_width | (reg_info->image_mcu_height << JPEG_REG_CFG_SIXTEEN_BITS);
    jpeg_reg_write(JPGD_REG_PICSIZE, reg_picture_size);

    reg_dec_stride = (reg_info->dst_buf_stride[1] << JPEG_REG_CFG_SIXTEEN_BITS) | reg_info->dst_buf_stride[0];
    jpeg_reg_write(JPGD_REG_STRIDE, reg_dec_stride);
}

static inline hi_void jpeg_cfg_output_height(hi_jpeg_reg_info *reg_info)
{
    jpeg_reg_write(JPGD_REG_OUTPUT_HEIGHT, reg_info->output_height);
}

static hi_void jpeg_cfg_fac(hi_jpeg_reg_info *reg_info)
{
    hi_u8 y_factor, u_factor, v_factor;
    hi_u32 reg_factor_value;

    y_factor = (hi_u8)(((reg_info->fac[DRV_JPEG_Y_COMPONENT_NUM][0] << JPEG_REG_CFG_FOUR_BITS) |
                        reg_info->fac[DRV_JPEG_Y_COMPONENT_NUM][1]) &
                       0xff);
    u_factor = (hi_u8)(((reg_info->fac[DRV_JPEG_U_COMPONENT_NUM][0] << JPEG_REG_CFG_FOUR_BITS) |
                        reg_info->fac[DRV_JPEG_U_COMPONENT_NUM][1]) &
                       0xff);
    v_factor = (hi_u8)(((reg_info->fac[DRV_JPEG_V_COMPONENT_NUM][0] << JPEG_REG_CFG_FOUR_BITS) |
                        reg_info->fac[DRV_JPEG_V_COMPONENT_NUM][1]) &
                       0xff);

    reg_factor_value = (y_factor << JPEG_REG_CFG_SIXTEEN_BITS) | (u_factor << JPEG_REG_CFG_EIGHT_BITS) | v_factor;
    jpeg_reg_write(JPGD_REG_SAMPLINGFACTOR, reg_factor_value);
}

static inline hi_void jpeg_cfg_dri(hi_jpeg_reg_info *reg_info)
{
    jpeg_reg_write(JPGD_REG_DRI, reg_info->restart_interval);
}

static hi_void jpeg_cfg_mem_type(hi_void)
{
    /*
     * [0] : 1 MMZ 0 MMU src buffer type
     * [1] : 1 MMZ 0 MMU y buffer type
     * [2] : 1 MMZ 0 MMU uv buffer type
     */
    hi_u32 reg_mem_type = JPEG_STREAM_MEM_MMU_TYPE | JPEG_YOUTPUT_MEM_MMU_TYPE | JPEG_UVOUTPUT_MEM_MMU_TYPE |
                          JPEG_XRGBSAMPLE0_READ_MEM_MMU_TYPE | JPEG_XRGBSAMPLE1_READ_MEM_MMU_TYPE |
                          JPEG_XRGBSAMPLE0_WRITE_MEM_MMU_TYPE | JPEG_XRGBSAMPLE1_WRITE_MEM_MMU_TYPE |
                          JPEG_XRGBOUTPUT_MEM_MMU_TYPE | JPEG_LINENUM_MEM_MMU_TYPE;

#ifdef CONFIG_GFX_MMU_SUPPORT
    reg_mem_type &= ~JPEG_STREAM_MEM_MMU_TYPE;
    reg_mem_type &= ~JPEG_YOUTPUT_MEM_MMU_TYPE;
    reg_mem_type &= ~JPEG_UVOUTPUT_MEM_MMU_TYPE;
    reg_mem_type &= ~JPEG_XRGBSAMPLE0_READ_MEM_MMU_TYPE;
    reg_mem_type &= ~JPEG_XRGBSAMPLE1_READ_MEM_MMU_TYPE;
    reg_mem_type &= ~JPEG_XRGBSAMPLE0_WRITE_MEM_MMU_TYPE;
    reg_mem_type &= ~JPEG_XRGBSAMPLE1_WRITE_MEM_MMU_TYPE;
    reg_mem_type &= ~JPEG_XRGBOUTPUT_MEM_MMU_TYPE;
    reg_mem_type &= ~JPEG_LINENUM_MEM_MMU_TYPE;
#endif

    jpeg_reg_write(JPGD_REG_MMU_BYPASS, reg_mem_type);
}

static hi_void jpeg_cfg_stream_buf(hi_jpeg_reg_info *reg_info)
{
    hi_u64 start_save_stream_phy_buf, end_save_stream_phy_buf;
    if (reg_info->is_out_usr_buf == HI_TRUE) {
        jpeg_get_save_stream_buf(reg_info, &start_save_stream_phy_buf, &end_save_stream_phy_buf);
    } else {
        start_save_stream_phy_buf = reg_info->start_save_stream_phy_buf;
        end_save_stream_phy_buf = reg_info->end_save_stream_phy_buf;
    }
    jpeg_cfg_input_stream_buf(start_save_stream_phy_buf, end_save_stream_phy_buf);
    jpeg_cfg_save_input_stream_buf(start_save_stream_phy_buf, end_save_stream_phy_buf);
    return;
}

static hi_void jpeg_get_save_stream_buf(hi_jpeg_reg_info *reg_info, hi_u64 *start_save_stream_phy_buf,
                                        hi_u64 *end_save_stream_phy_buf)
{
    hi_u64 start_address, end_address, src_phy_buf;
#ifndef HI_BUILD_IN_BOOT
    hi_mem_handle buf_addr = {0};
    unsigned long phy_addr;
    buf_addr.mem_handle = reg_info->src_buf_handle;
    buf_addr.addr_offset = reg_info->src_buf_offset;
    phy_addr = drv_gfx_mem_get_smmu_from_fd(buf_addr, HIGFX_JPGDEC_ID);

    src_phy_buf = phy_addr + reg_info->cur_pos_in_buffer;
#else
    src_phy_buf = reg_info->src_phy_buf + reg_info->cur_pos_in_buffer;
#endif
    if ((start_save_stream_phy_buf == NULL) || (end_save_stream_phy_buf == NULL)) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return;
    }
    start_address = src_phy_buf;
    end_address = start_address + reg_info->src_buf_size;

    *start_save_stream_phy_buf = start_address;
    *end_save_stream_phy_buf = end_address;
    return;
}

static hi_void jpeg_cfg_input_stream_buf(hi_u64 start_address, hi_u64 end_address)
{
    start_address = ((start_address + JPED_STREAM_BUFFER_ALIGN - 1) & (~(JPED_STREAM_BUFFER_ALIGN - 1))) -
                    JPED_STREAM_BUFFER_ALIGN;
    end_address = (end_address + JPED_STREAM_BUFFER_ALIGN - 1) & (~(JPED_STREAM_BUFFER_ALIGN - 1));
    jpeg_reg_write(JPGD_REG_STADDR, start_address);
    jpeg_reg_write(JPGD_REG_ENDADDR, end_address);
    jpeg_reg_write(JPGD_REG_ERR_STREAM_EN, 0x0);
    return;
}

static hi_void jpeg_cfg_save_input_stream_buf(hi_u64 start_address, hi_u64 end_address)
{
    jpeg_reg_write(JPGD_REG_STADD, start_address & 0xffffffff);
    jpeg_reg_write(JPGD_REG_ENDADD, end_address & 0xffffffff);
    return;
}

static hi_void jpeg_cfg_dec_out_yuv_buf(hi_u64 y_dst_phy_addr, hi_u64 uv_dst_phy_addr)
{
    jpeg_reg_write(JPGD_REG_YSTADDR, y_dst_phy_addr);
    jpeg_reg_write(JPGD_REG_UVSTADDR, uv_dst_phy_addr);
    return;
}

static hi_void jpeg_cfg_out_buf(hi_jpeg_reg_info *reg_info)
{
    hi_u64 y_dst_phy_addr, uv_dst_phy_addr;
    if (reg_info->is_out_usr_buf == HI_TRUE) {
        jpeg_get_yuv_out_buf(reg_info, &y_dst_phy_addr, &uv_dst_phy_addr);
    } else {
        y_dst_phy_addr = reg_info->y_dst_phy_addr;
        uv_dst_phy_addr = reg_info->uv_dst_phy_addr;
    }
    jpeg_cfg_dec_out_yuv_buf(y_dst_phy_addr, uv_dst_phy_addr);
    return;
}

static hi_void jpeg_get_yuv_out_buf(hi_jpeg_reg_info *reg_info, hi_u64 *y_dst_phy_addr, hi_u64 *uv_dst_phy_addr)
{
#ifndef HI_BUILD_IN_BOOT
    hi_mem_handle y_buf_addr = {0};
    hi_mem_handle uv_buf_addr = {0};
    if ((y_dst_phy_addr == NULL) || (uv_dst_phy_addr == NULL)) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return;
    }

    y_buf_addr.mem_handle = reg_info->dst_buf_handle;
    y_buf_addr.addr_offset = reg_info->dst_buf_y_offset;
    *y_dst_phy_addr = drv_gfx_mem_get_smmu_from_fd(y_buf_addr, HIGFX_JPGDEC_ID);

    uv_buf_addr.mem_handle = reg_info->dst_buf_handle;
    uv_buf_addr.addr_offset = reg_info->dst_buf_uv_offset;
    *uv_dst_phy_addr = drv_gfx_mem_get_smmu_from_fd(uv_buf_addr, HIGFX_JPGDEC_ID);
#else
    if ((y_dst_phy_addr == NULL) || (uv_dst_phy_addr == NULL)) {
        HI_PRINT("[module-jpeg][err] : %s %d input null pointer\n", __FUNCTION__, __LINE__);
        return;
    }
    *y_dst_phy_addr = reg_info->dst_phy_buf;
    *uv_dst_phy_addr = *y_dst_phy_addr + reg_info->dst_buf_uv_offset;
#endif
    return;
}

static hi_void jpeg_cfg_frame_dec_start(hi_jpeg_reg_info *reg_info)
{
    hi_u32 low_delay_value = 0;
    hi_u32 reg_start_value;

#ifdef CONFIG_JPEG_LOW_DELAY_SUPPORT
    if (reg_info->is_low_delay == HI_TRUE) {
        low_delay_value = 0x8;
    }
#endif

    jpeg_reg_write(JPGD_REG_RESUME, reg_info->resume_value);

#ifdef CONFIG_JPEG_4KDDR_DISABLE
    reg_start_value = 0x5 | low_delay_value;
#else
    reg_start_value = 0x1 | low_delay_value;
#endif
    jpeg_reg_write(JPGD_REG_START, reg_start_value);
}

hi_void drv_jpeg_set_int_mask(hi_u32 int_mask)
{
    jpeg_reg_write(JPGD_REG_INTMASK, int_mask);
    return;
}

#ifdef HI_BUILD_IN_BOOT
hi_void hi_drv_jpeg_dev_open(hi_void)
{
    volatile U_PERI_CRG332 untemp_value;
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
#ifdef CONFIG_GFX_MMU_CLOCK
    volatile U_PERI_CRG180 untemp_smmu_value;
#endif
    if (reg_crg == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d reg_crg is null pointer\n", __FUNCTION__, __LINE__);
        return;
    }

    untemp_value.u32 = reg_crg->PERI_CRG332.u32;
    untemp_value.bits.jpgd_cken = 0x1;
    reg_crg->PERI_CRG332.u32 = untemp_value.u32;
    osal_udelay(JPEG_CFG_CLOCK_DELAY_TIME_MS);
    untemp_value.bits.jpgd_srst_req = 0x0;
    reg_crg->PERI_CRG332.u32 = untemp_value.u32;

#ifdef CONFIG_GFX_MMU_CLOCK
    untemp_smmu_value.u32 = reg_crg->PERI_CRG180.u32;
    untemp_smmu_value.bits.jpgd_smmu_cken = 0x1;
    reg_crg->PERI_CRG180.u32 = untemp_smmu_value.u32;
    osal_udelay(JPEG_CFG_CLOCK_DELAY_TIME_MS);
    untemp_smmu_value.bits.jpgd_smmu_srst_req = 0x0;
    reg_crg->PERI_CRG180.u32 = untemp_smmu_value.u32;
#endif
    return;
}

hi_void hi_drv_jpeg_dev_close(hi_void)
{
    volatile U_PERI_CRG332 untemp_value;
    volatile hi_reg_crg *reg_crg = hi_drv_sys_get_crg_reg_ptr();
#ifdef CONFIG_GFX_MMU_CLOCK
    volatile U_PERI_CRG180 untemp_smmu_value;
#endif
    if (reg_crg == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d reg_crg is null pointer\n", __FUNCTION__, __LINE__);
        return;
    }

    untemp_value.u32 = reg_crg->PERI_CRG332.u32;
    untemp_value.bits.jpgd_srst_req = 0x1;
    reg_crg->PERI_CRG332.u32 = untemp_value.u32;
    osal_udelay(JPEG_CFG_CLOCK_DELAY_TIME_MS);
    untemp_value.bits.jpgd_cken = 0x0;
    reg_crg->PERI_CRG332.u32 = untemp_value.u32;

#ifdef CONFIG_GFX_MMU_CLOCK
    untemp_smmu_value.u32 = reg_crg->PERI_CRG180.u32;
    untemp_smmu_value.bits.jpgd_smmu_srst_req = 0x1;
    reg_crg->PERI_CRG180.u32 = untemp_smmu_value.u32;
    osal_udelay(JPEG_CFG_CLOCK_DELAY_TIME_MS);
    untemp_smmu_value.bits.jpgd_smmu_cken = 0x0;
    reg_crg->PERI_CRG180.u32 = untemp_smmu_value.u32;
#endif

    return;
}

hi_void hi_drv_jpeg_start_decompress(hi_void *arg)
{
    hi_jpeg_reg_info *reg_info = (hi_jpeg_reg_info *)arg;
    if (arg == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d reg_info is null pointer\n", __FUNCTION__, __LINE__);
        return;
    }
    drv_jpeg_start_decompress(reg_info);
    return;
}

hi_void hi_drv_jpeg_get_dec_status(hi_u32 *status)
{
    drv_jpeg_dev_get_status(status);
    return;
}

hi_void hi_drv_jpeg_set_dec_status(hi_u32 status)
{
    drv_jpeg_dev_clear_status(status);
    return;
}

hi_void hi_drv_jpeg_cfg_int_mask(hi_void)
{
    jpeg_reg_write(JPGD_REG_INTMASK, JPGD_REG_INTMASK_VALUE);
    return;
}

hi_void hi_drv_jpeg_set_reg_base(hi_void)
{
    volatile hi_u32 *jpeg_reg_base = NULL;
    jpeg_reg_base = (hi_u32 *)JPGD_REG_BASEADDR;
    if (jpeg_reg_base == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d jpeg_reg_base is null pointer\n", __FUNCTION__, __LINE__);
        return;
    }
    jpeg_set_reg_base(jpeg_reg_base);
}
#endif
