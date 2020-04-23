/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aoe hal function layer
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_osal.h"
#include "hi_aiao_log.h"

#include "hi_audsp_aoe.h"
#include "hal_aoe_func.h"
#include "drv_adsp_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

static aoe_fw_info g_aoe_fw_info;

static adsp_export_func *i_hal_aoe_get_adsp_func(hi_void)
{
    hi_s32 ret;
    adsp_export_func *adsp_func = HI_NULL;

    ret = osal_exportfunc_get(HI_ID_ADSP, (hi_void **)&adsp_func);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(osal_exportfunc_get, ret);
        return HI_NULL;
    }

    if (adsp_func == HI_NULL) {
        HI_LOG_ERR("adsp function is null\n");
        return HI_NULL;
    }

    if (adsp_func->adsp_load_firmware == HI_NULL) {
        HI_LOG_ERR("adsp function adsp_load_firmware is null\n");
        return HI_NULL;
    }

    if (adsp_func->adsp_unload_firmware == HI_NULL) {
        HI_LOG_ERR("adsp function adsp_unload_firmware is null\n");
        return HI_NULL;
    }

    if (adsp_func->adsp_get_aoe_fw_info == HI_NULL) {
        HI_LOG_ERR("adsp function adsp_get_aoe_fw_info is null\n");
        return HI_NULL;
    }

    if (adsp_func->adsp_write_cmd == HI_NULL) {
        HI_LOG_ERR("adsp function adsp_write_cmd is null\n");
        return HI_NULL;
    }

    return adsp_func;
}

static hi_void i_hal_aoe_unload_firmware(hi_void)
{
    hi_s32 ret;
    adsp_export_func *adsp_func = i_hal_aoe_get_adsp_func();
    if (adsp_func == HI_NULL) {
        HI_LOG_ERR("adsp function is null\n");
        return;
    }

    ret = (adsp_func->adsp_unload_firmware)(ADSP_CODE_AOE);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_unload_firmware, ret);
    }
}

static hi_s32 i_hal_aoe_load_firmware(hi_void)
{
    hi_s32 ret;
    adsp_export_func *adsp_func = i_hal_aoe_get_adsp_func();
    if (adsp_func == HI_NULL) {
        HI_LOG_ERR("adsp function is null\n");
        return HI_ERR_AO_NULL_PTR;
    }

    /* load dsp firmware */
    ret = (adsp_func->adsp_load_firmware)(ADSP_CODE_AOE);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_load_firmware, ret);
        return ret;
    }

    /* get aoe info */
    ret = (adsp_func->adsp_get_aoe_fw_info)(ADSP_CODE_AOE, &g_aoe_fw_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_get_aoe_fw_info, ret);
        i_hal_aoe_unload_firmware();
    }

    return ret;
}

hi_s32 i_hal_aoe_init(hi_void)
{
    hi_s32 ret;

    ret = i_hal_aoe_load_firmware();
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(i_hal_aoe_load_firmware, ret);
    }

    return ret;
}

hi_void i_hal_aoe_deinit(hi_void)
{
    i_hal_aoe_unload_firmware();
}

hi_void i_hal_aoe_aip_get_status_phy(hi_u32 aip, hi_u64 *phy_delay_addr)
{
    *phy_delay_addr =
        g_aoe_fw_info.aoe_reg_phy_addr +
        AOE_AIP_REG_OFFSET +
        AOE_AIP_REG_BANDSIZE * aip +
        offsetof(aip_regs_type, AIP_STATUS0);
}

static hi_void aoe_reg_bit_depth(hi_u32 bit_per_sample, hi_u32 *reg)
{
    switch (bit_per_sample) {
        case HI_BIT_DEPTH_16:
            *reg = 1;
            break;
        case HI_BIT_DEPTH_24:
            *reg = 2;
            break;
        default:
            *reg = 0;
    }

    return;
}

static hi_void aoe_reg_channels(hi_u32 channels, hi_u32 *reg)
{
    switch (channels) {
        case 0x01:
            *reg = 0;
            break;
        case 0x02:
            *reg = 1;
            break;
        case 0x08:
            *reg = 3;
            break;
        default:
            *reg = 0;
    }

    return;
}

static hi_void aoe_reg_sampel_rate(hi_u32 sampel_rate, hi_u32 *reg)
{
    hi_u32 i;
    const hi_u32 fs_table[] = {
        8000, 11025, 12000, 16000, 22050, 24000, 32000,
        44100, 48000, 88200, 96000, 176400, 192000,
    };

    for (i = 0; i < ARRAY_SIZE(fs_table); i++) {
        if (fs_table[i] == sampel_rate) {
            *reg = i;
            return;
        }
    }

    *reg = 0xf;  /* ext fs */
}

hi_void aoe_aip_get_remap_addr(hi_u64 *dsp_remap_addr)
{
#if defined(DSP_DDR_DMAREMAP_SUPPORT)
    if ((*dsp_remap_addr >= DSP_DDR_DMAREMAP_BEG_ADDR) && (*dsp_remap_addr < DSP_DDR_DMAREMAP_END_ADDR)) {
        *dsp_remap_addr += DSP_DDR_DMAREMAP_MAP_ADDR;
    }
#endif
}

static hi_u32 aoe_aip_get_data_format(hi_u32 data_format)
{
    if (data_format == 0) {
        return AOE_FORMAT_LPCM;
    } else {
        return (data_format & 0xff00) ? AOE_FORMAT_IEC61937_COMP : AOE_FORMAT_IEC61937;
    }
}

static hi_void aoe_aip_buf_set_addr(aip_regs_type *aip_reg, aoe_aip_in_buf_attr *buf_in_attr)
{
    if (g_aoe_fw_info.aoe_sw_flag == HI_TRUE) {
        adsp_write_addr(buf_in_attr->rbf_attr.buf_vir_addr, aip_reg->AIP_BUF_ADDR);
        aip_reg->AIP_BUF_PHYADDR = buf_in_attr->rbf_attr.buf_phy_addr;
    } else {
        hi_u64 dsp_remap_addr = buf_in_attr->rbf_attr.buf_phy_addr;
        aoe_aip_get_remap_addr(&dsp_remap_addr);
        adsp_write_addr(dsp_remap_addr, aip_reg->AIP_BUF_ADDR);
        aip_reg->AIP_BUF_PHYADDR = dsp_remap_addr;
    }

    if (buf_in_attr->rbf_attr.buf_wptr_rptr_flag) {
        if (g_aoe_fw_info.aoe_sw_flag == HI_TRUE) {
            adsp_write_addr(buf_in_attr->rbf_attr.buf_vir_wptr, aip_reg->AIP_BUF_WPTR);
            adsp_write_addr(buf_in_attr->rbf_attr.buf_vir_rptr, aip_reg->AIP_BUF_RPTR);
        } else {
            hi_u64 dsp_remap_addr = buf_in_attr->rbf_attr.buf_phy_wptr;
            aoe_aip_get_remap_addr(&dsp_remap_addr);
            adsp_write_addr(dsp_remap_addr, aip_reg->AIP_BUF_WPTR);

            dsp_remap_addr = buf_in_attr->rbf_attr.buf_phy_rptr;
            aoe_aip_get_remap_addr(&dsp_remap_addr);
            adsp_write_addr(dsp_remap_addr, aip_reg->AIP_BUF_RPTR);
        }
    } else {
        aip_reg->AIP_BUF_WPTR.ADDR_HIGH = 0;
        aip_reg->AIP_BUF_WPTR.ADDR_LOW = 0;
        aip_reg->AIP_BUF_RPTR.ADDR_HIGH = 0;
        aip_reg->AIP_BUF_RPTR.ADDR_LOW = 0;
    }

    aip_reg->AIP_BUF_SIZE.bits.buff_flag = buf_in_attr->rbf_attr.buf_wptr_rptr_flag;
    aip_reg->AIP_BUF_SIZE.bits.buff_size = buf_in_attr->rbf_attr.buf_size;
    aip_reg->AIP_EXT3_CTRL.bits.start_threshold = buf_in_attr->start_threshold_ms;
    aip_reg->AIP_EXT3_CTRL.bits.resume_threshold = buf_in_attr->resume_threshold_ms;
    aip_reg->AIP_BUF_TRANS_SIZE.bits.tx_trans_size = 0;
}

static hi_void aoe_aip_buf_set_attr(aip_regs_type *aip_reg, aoe_aip_in_buf_attr *buf_in_attr)
{
    hi_u32 ch;
    hi_u32 rate;
    hi_u32 data_format;
    hi_u32 bit_depth = 0;

    data_format = buf_in_attr->buf_data_format;
    aip_reg->AIP_BUFF_ATTR.bits.aip_format = aoe_aip_get_data_format(data_format);

    aoe_reg_bit_depth(buf_in_attr->buf_bit_per_sample, &bit_depth);
    aip_reg->AIP_BUFF_ATTR.bits.aip_precision = bit_depth;
    aoe_reg_channels(buf_in_attr->buf_channels, &ch);
    aip_reg->AIP_BUFF_ATTR.bits.aip_ch = ch;
    aoe_reg_sampel_rate(buf_in_attr->buf_sample_rate, &rate);
    aip_reg->AIP_BUFF_ATTR.bits.aip_fs = rate;
    aip_reg->AIP_BUFF_ATTR.bits.aip_priority = !!buf_in_attr->mix_priority;
    if (0xf == rate) {
        aip_reg->AIP_SRC_ATTR_EXT.bits.fs_ext = buf_in_attr->buf_sample_rate;
    }

    if (buf_in_attr->fade_enable == HI_TRUE) {
        aip_reg->AIP_CTRL.bits.fade_en = 1;

        aip_reg->AIP_CTRL.bits.fade_in_rate = buf_in_attr->fade_in_ms;
        aip_reg->AIP_CTRL.bits.fade_out_rate = buf_in_attr->fade_out_ms;
    } else {
        aip_reg->AIP_CTRL.bits.fade_en = 0;
    }

    aip_reg->AIP_BUFF_ATTR.bits.aip_pcm_type = buf_in_attr->aip_type;
}

static hi_void aoe_aip_fifo_set_attr(aip_regs_type *aip_reg, aoe_aip_chn_attr *attr)
{
    hi_u32 ch = 0;
    hi_u32 rate = 0;
    hi_u32 data_format;
    hi_u32 bit_depth = 0;

    aoe_reg_bit_depth(attr->fifo_out_attr.fifo_bit_per_sample, &bit_depth);
    aip_reg->AIP_FIFO_ATTR.bits.fifo_precision = bit_depth;
    aoe_reg_channels(attr->fifo_out_attr.fifo_channels, &ch);
    aip_reg->AIP_FIFO_ATTR.bits.fifo_ch = ch;
    aoe_reg_sampel_rate(attr->fifo_out_attr.fifo_sample_rate, &rate);
    aip_reg->AIP_FIFO_ATTR.bits.fifo_fs = rate;

    data_format = attr->fifo_out_attr.fifo_data_format;
    aip_reg->AIP_FIFO_ATTR.bits.fifo_format = aoe_aip_get_data_format(data_format);

    aip_reg->AIP_FIFO_ATTR.bits.fifo_latency = attr->fifo_out_attr.fifo_latency_thd_ms;
    aip_reg->AIP_FIFO_ATTR.bits.fifo_bypass = !!attr->fifo_bypass;
}

hi_void i_hal_aoe_aip_set_attr(aip_regs_type *aip_reg, aoe_aip_chn_attr *attr)
{
    aoe_aip_buf_set_attr(aip_reg, &attr->buf_in_attr);
    aoe_aip_buf_set_addr(aip_reg, &attr->buf_in_attr);
    aoe_aip_fifo_set_attr(aip_reg, attr);
}

#ifdef HI_SND_ADVANCED_SUPPORT
hi_u64 i_hal_aoe_aip_get_reg_addr(aoe_aip_id aip)
{
    return (g_aoe_fw_info.aoe_reg_phy_addr + AOE_AIP_REG_OFFSET + AOE_AIP_REG_BANDSIZE * aip);
}
#endif

aip_regs_type *i_hal_aoe_aip_map_reg(aoe_aip_id aip)
{
    hi_u8 *aoe_reg_addr = (hi_u8 *)g_aoe_fw_info.aoe_reg_vir_addr;
    if (aoe_reg_addr == HI_NULL) {
        HI_LOG_ERR("aoe_reg_addr is NULL\n");
        return HI_NULL;
    }

    if (aip >= AOE_AIP_MAX) {
        HI_LOG_ERR("invalid aip id\n");
        return HI_NULL;
    }

    return (aip_regs_type *)(aoe_reg_addr + AOE_AIP_REG_OFFSET + AOE_AIP_REG_BANDSIZE * aip);
}

hi_void i_hal_aoe_aip_set_prescale(aip_regs_type *aip_reg, hi_u32 int_db, hi_s32 dec_db)
{
    aip_reg->AIP_EXT2_CTRL.bits.prescale_int = int_db;

    if (dec_db >= 0) {
        aip_reg->AIP_EXT2_CTRL.bits.prescale_dec_dir = 0;
        aip_reg->AIP_EXT2_CTRL.bits.prescale_dec = dec_db;
    } else {
        aip_reg->AIP_EXT2_CTRL.bits.prescale_dec_dir = 1;
        aip_reg->AIP_EXT2_CTRL.bits.prescale_dec = -dec_db;
    }
}

hi_void i_hal_aoe_aip_set_adjust_speed(aip_regs_type *aip_reg, hi_s32 adj_speed)
{
    if (adj_speed >= 0) {
        aip_reg->AIP_CTRL.bits.dst_fs_adj_dir = 0;
        aip_reg->AIP_CTRL.bits.dst_fs_adj_step = adj_speed;
    } else {
        aip_reg->AIP_CTRL.bits.dst_fs_adj_dir = 1;
        aip_reg->AIP_CTRL.bits.dst_fs_adj_step = -adj_speed;
    }
}

hi_s32 i_hal_aoe_proc_cmd(adsp_cmd_arg *cmd)
{
    adsp_export_func *adsp_func = i_hal_aoe_get_adsp_func();
    if (adsp_func == HI_NULL) {
        HI_LOG_ERR("adsp function is null\n");
        return HI_ERR_AO_NULL_PTR;
    }

    return adsp_func->adsp_write_cmd(cmd);
}

static adsp_cmd_ret i_hal_aoe_aip_ack_add_mute_cmd(aip_regs_type *aip_reg)
{
    hi_s32 ret;
    DECLARE_WAIT_QUEUE_HEAD(hal_aoe_wait);

    wake_up_interruptible(&hal_aoe_wait);
    ret = wait_event_interruptible_timeout(hal_aoe_wait,
        aip_reg->AIP_ADD_MUTE_CTRL.bits.cmd_done == 1,
        msecs_to_jiffies(DSP_ACK_TIME));
    if (ret <= 0) {
        HI_ERR_PRINT_FUNC_RES(wait_event_interruptible_timeout, ret);
        return ADSP_CMD_ERR_TIMEOUT;
    }

    return (adsp_cmd_ret)aip_reg->AIP_ADD_MUTE_CTRL.bits.cmd_return_value;
}

hi_void i_hal_aoe_aip_set_add_mute_ms(aip_regs_type *aip_reg, hi_u32 mute_ms)
{
    adsp_cmd_ret ack;

    aip_reg->AIP_ADD_MUTE_CTRL.bits.add_mute_ms = mute_ms;
    aip_reg->AIP_ADD_MUTE_CTRL.bits.cmd = ADSP_CMD_START;
    aip_reg->AIP_ADD_MUTE_CTRL.bits.cmd_done = 0;

    ack = i_hal_aoe_aip_ack_add_mute_cmd(aip_reg);
    if (ack != ADSP_CMD_DONE) {
        HI_ERR_PRINT_FUNC_RES(i_hal_aoe_aip_ack_add_mute_cmd, ack);
    }
}

/* aop func */
aop_regs_type *i_hal_aoe_aop_map_reg(aoe_aop_id aop)
{
    hi_u8 *aoe_reg_addr = (hi_u8 *)g_aoe_fw_info.aoe_reg_vir_addr;
    if (aoe_reg_addr == HI_NULL) {
        HI_LOG_ERR("aoe_reg_addr is NULL\n");
        return HI_NULL;
    }

    if (aop >= AOE_AOP_MAX) {
        HI_LOG_ERR("invalid aop id\n");
        return HI_NULL;
    }

    return (aop_regs_type *)(aoe_reg_addr + AOE_AOP_REG_OFFSET + AOE_AOP_REG_BANDSIZE * aop);
}

hi_void i_hal_aoe_aop_set_preci_vol(aop_regs_type *aop_reg, hi_u32 int_db, hi_s32 dec_db)
{
    aop_reg->AOP_EXT_CTRL.bits.volume_l = int_db;
    aop_reg->AOP_EXT_CTRL.bits.volume_r = int_db;
    if (dec_db >= 0) {
        aop_reg->AOP_EXT_CTRL.bits.volumedec_dir_l = 0;
        aop_reg->AOP_EXT_CTRL.bits.volumedec_dir_r = 0;
        aop_reg->AOP_EXT_CTRL.bits.volumedec_l = dec_db;
        aop_reg->AOP_EXT_CTRL.bits.volumedec_r = dec_db;
    } else {
        aop_reg->AOP_EXT_CTRL.bits.volumedec_dir_l = 1;
        aop_reg->AOP_EXT_CTRL.bits.volumedec_dir_r = 1;
        aop_reg->AOP_EXT_CTRL.bits.volumedec_l = -dec_db;
        aop_reg->AOP_EXT_CTRL.bits.volumedec_r = -dec_db;
    }
}

hi_void i_hal_aoe_aop_set_attr(aop_regs_type *aop_reg, aoe_aop_chn_attr *attr)
{
    hi_u32 rate, bit_depth, ch, data_format;

    if (g_aoe_fw_info.aoe_sw_flag == HI_TRUE) {
        adsp_write_addr(attr->rbf_out_attr.rbf_attr.buf_vir_addr, aop_reg->AOP_BUF_ADDR);
    } else {
        hi_u64 dsp_remap_addr = attr->rbf_out_attr.rbf_attr.buf_phy_addr;
#if defined(DSP_DDR_DMAREMAP_SUPPORT)
        if ((dsp_remap_addr >= DSP_DDR_DMAREMAP_BEG_ADDR) && (dsp_remap_addr < DSP_DDR_DMAREMAP_END_ADDR)) {
            dsp_remap_addr += DSP_DDR_DMAREMAP_MAP_ADDR;
        }
#endif

        adsp_write_addr(dsp_remap_addr, aop_reg->AOP_BUF_ADDR);
    }

    if (attr->rbf_out_attr.rbf_attr.buf_wptr_rptr_flag) {
        if (g_aoe_fw_info.aoe_sw_flag == HI_TRUE) {
            adsp_write_addr(attr->rbf_out_attr.rbf_attr.buf_vir_wptr, aop_reg->AOP_BUF_WPTR);
            adsp_write_addr(attr->rbf_out_attr.rbf_attr.buf_vir_rptr, aop_reg->AOP_BUF_RPTR);
        } else {
            hi_u64 dsp_remap_addr;
            dsp_remap_addr = attr->rbf_out_attr.rbf_attr.buf_phy_wptr;
#if defined(DSP_DDR_DMAREMAP_SUPPORT)
            if ((dsp_remap_addr >= DSP_DDR_DMAREMAP_BEG_ADDR) && (dsp_remap_addr < DSP_DDR_DMAREMAP_END_ADDR)) {
                dsp_remap_addr += DSP_DDR_DMAREMAP_MAP_ADDR;
            }
#endif

            adsp_write_addr(dsp_remap_addr, aop_reg->AOP_BUF_WPTR);

            dsp_remap_addr = attr->rbf_out_attr.rbf_attr.buf_phy_rptr;
#if defined(DSP_DDR_DMAREMAP_SUPPORT)
            if ((dsp_remap_addr >= DSP_DDR_DMAREMAP_BEG_ADDR) && (dsp_remap_addr < DSP_DDR_DMAREMAP_END_ADDR)) {
                dsp_remap_addr += DSP_DDR_DMAREMAP_MAP_ADDR;
            }
#endif

            adsp_write_addr(dsp_remap_addr, aop_reg->AOP_BUF_RPTR);
        }
    } else {
        aop_reg->AOP_BUF_WPTR.ADDR_LOW = 0;
        aop_reg->AOP_BUF_WPTR.ADDR_HIGH = 0;
        aop_reg->AOP_BUF_RPTR.ADDR_LOW = 0;
        aop_reg->AOP_BUF_RPTR.ADDR_HIGH = 0;
    }

    aop_reg->AOP_BUF_SIZE.bits.buff_size = attr->rbf_out_attr.rbf_attr.buf_size;
    aop_reg->AOP_BUF_SIZE.bits.buff_flag = attr->rbf_out_attr.rbf_attr.buf_wptr_rptr_flag; /* buf_wptr_rptr_flag */

    /* set fifo attr */
    data_format = attr->rbf_out_attr.buf_data_format;
    aop_reg->AOP_BUFF_ATTR.bits.buf_format = aoe_aip_get_data_format(data_format);
    aoe_reg_bit_depth(attr->rbf_out_attr.buf_bit_per_sample, &bit_depth);
    aop_reg->AOP_BUFF_ATTR.bits.buf_precision = bit_depth;
    aoe_reg_channels(attr->rbf_out_attr.buf_channels, &ch);
    aop_reg->AOP_BUFF_ATTR.bits.buf_ch = ch;
    aoe_reg_sampel_rate(attr->rbf_out_attr.buf_sample_rate, &rate);
    aop_reg->AOP_BUFF_ATTR.bits.buf_fs = rate;

    aop_reg->AOP_BUFF_ATTR.bits.buf_priority = !!attr->rbf_hw_priority;

    aop_reg->AOP_CTRL.bits.cast_flag = !!attr->is_cast;
    aop_reg->AOP_CTRL.bits.ena_add_mute = !!attr->add_mute;  /* only for cast aop */
    aop_reg->AOP_BUFF_ATTR.bits.buf_latency = attr->rbf_out_attr.buf_latency_thd_ms;
}

/* ENGINE function */
engine_regs_type *i_hal_aoe_engine_map_reg(aoe_engine_id engine)
{
    hi_u8 *aoe_reg_addr = (hi_u8 *)g_aoe_fw_info.aoe_reg_vir_addr;
    if (aoe_reg_addr == HI_NULL) {
        HI_LOG_ERR("aoe_reg_addr is NULL\n");
        return HI_NULL;
    }

    if (engine >= AOE_ENGINE_MAX) {
        HI_LOG_ERR("invalid engine id\n");
        return HI_NULL;
    }

    return (engine_regs_type *)(aoe_reg_addr + AOE_ENGINE_REG_OFFSET + AOE_ENGINE_REG_BANDSIZE * engine);
}

hi_void i_hal_aoe_engine_set_attr(engine_regs_type *engine_reg, aoe_engine_attr *attr)
{
    hi_u32 rate, bit_depth, ch, data_format;

    aoe_reg_bit_depth(attr->bit_per_sample, &bit_depth);
    engine_reg->ENGINE_ATTR.bits.precision = bit_depth;
    aoe_reg_channels(attr->channels, &ch);
    engine_reg->ENGINE_ATTR.bits.ch = ch;
    aoe_reg_sampel_rate(attr->sample_rate, &rate);
    engine_reg->ENGINE_ATTR.bits.fs = rate;

    data_format = attr->data_format;
    engine_reg->ENGINE_ATTR.bits.format = aoe_aip_get_data_format(data_format);
}

#ifdef HI_SND_GEQ_SUPPORT
hi_void i_hal_aoe_engine_set_geq_attr(engine_regs_type *engine_reg, hi_ao_geq_attr *geq_attr)
{
    hi_u32 i;
    U_EQ_BAND_ATTR *geq_reg = (U_EQ_BAND_ATTR *)(&engine_reg->ENGINE_GEQ_BAND1_ATTR);

    engine_reg->ENGINE_ATTR.bits.geq_bandnum = geq_attr->band_num;
    for (i = 0; i < geq_attr->band_num; i++) {
        geq_reg->bits.freq = geq_attr->param[i].freq;

        if (geq_attr->param[i].gain < 0) {
            geq_reg->bits.gain_dir = 1;
            geq_reg->bits.gain_value = (-(geq_attr->param[i].gain)) / 125;
        } else {
            geq_reg->bits.gain_dir = 0;
            geq_reg->bits.gain_value = (geq_attr->param[i].gain) / 125;
        }
        geq_reg++;
    }
}

hi_void i_hal_aoe_engine_set_geq_gain(engine_regs_type *engine_reg, hi_u32 band, hi_s32 gain)
{
    U_EQ_BAND_ATTR *geq_reg = (U_EQ_BAND_ATTR *)(&engine_reg->ENGINE_GEQ_BAND1_ATTR);

    geq_reg += band;

    if (gain < 0) {
        geq_reg->bits.gain_dir = 1;
        geq_reg->bits.gain_value = -(gain) / 125;
    } else {
        geq_reg->bits.gain_dir = 0;
        geq_reg->bits.gain_value = gain / 125;
    }
}
#endif

#ifdef HI_SND_PEQ_SUPPORT
hi_void i_hal_aoe_aop_set_peq_attr(aop_regs_type *aop_reg, hi_ao_peq_attr *peq_attr)
{
    hi_u32 i;
    U_EQ_BAND_ATTR *peq_reg = (U_EQ_BAND_ATTR *)(&aop_reg->AOP_PEQ_BAND1_ATTR);

    aop_reg->AOP_CTRL.bits.peq_bandnum = peq_attr->band_num;
    for (i = 0; i < peq_attr->band_num; i++) {
        peq_reg->bits.freq = peq_attr->param[i].freq;
        peq_reg->bits.q_value = peq_attr->param[i].q;

        if (peq_attr->param[i].gain < 0) {
            peq_reg->bits.gain_dir = 1;
            peq_reg->bits.gain_value = (-(peq_attr->param[i].gain)) / 125;
        } else {
            peq_reg->bits.gain_dir = 0;
            peq_reg->bits.gain_value = (peq_attr->param[i].gain) / 125;
        }

        if (i < 8) {
            aop_reg->AOP_PEQ_BAND_TYPE1.u32 &= (~(0xf << (4 * i)));
            aop_reg->AOP_PEQ_BAND_TYPE1.u32 |= (peq_attr->param[i].type << (4 * i));
        } else {
            aop_reg->AOP_PEQ_BAND_TYPE2.u32 &= (~(0xf << (4 * (i - 8))));
            aop_reg->AOP_PEQ_BAND_TYPE2.u32 |= (peq_attr->param[i].type << (4 * (i - 8)));
        }

        peq_reg++;
    }
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
