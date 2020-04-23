/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: drv adsp
 * Author: Audio
 * Create: 2012-05-30
 * Notes: NA
 * History: 2019-05-30 CSEC Rectification
 */

#include "hi_adsp_debug.h"

#include "hi_osal.h"

/* drv headers */
#include "drv_adsp_private.h"
#include "drv_adsp_ext.h"
#include "hi_audsp_aoe.h"

#include "drv_adsp_hal.h"

#define AOE_STATUS_COUNT 4

static hi_void *g_aoe_reg_addr = HI_NULL;

static const hi_u32 adsp_aoe_rate_proc(hi_u32 rate)
{
    hi_u32 g_rate_tab[] = {
        8000,
        11025,
        12000,
        16000,
        22050,
        24000,
        32000,
        44100,
        48000,
        88200,
        96000,
        176400,
        192000,
        288000,
    };

    if (rate < ARRAY_SIZE(g_rate_tab)) {
        return g_rate_tab[rate];
    }

    return g_rate_tab[0];
}

static const hi_u32 adsp_aoe_ch_proc(hi_u32 ch)
{
    switch (ch) {
        case 0x00:
            return HI_AUDIO_CH_MONO;
        case 0x01:
            return HI_AUDIO_CH_STEREO;
        case 0x03:
            return HI_AUDIO_CH_8;
        default:
            return 0;
    }
}

static const hi_u32 adsp_aoe_width_proc(hi_u32 width)
{
    switch (width) {
        case 0x00:
            return HI_BIT_DEPTH_8;
        case 0x01:
            return HI_BIT_DEPTH_16;
        case 0x02:
            return HI_BIT_DEPTH_24;
        default:
            return 0;
    }
}

static const hi_char *adsp_aoe_status_name(hi_u32 cmd, hi_u32 done)
{
    const hi_char *apc_name[AOE_STATUS_COUNT] = {
        "start",
        "stop",
        "pause",
        "flush",
    };

    if ((done == 0) || (cmd >= AOE_STATUS_COUNT)) {
        return "invalid";
    }

    return apc_name[cmd];
}

static hi_u32 adsp_buf_used_size_proc(hi_u32 size, hi_u32 read_ptr, hi_u32 write_ptr)
{
    if (read_ptr <= write_ptr) {
        return write_ptr - read_ptr;
    } else {
        return size - (read_ptr - write_ptr);
    }
}

static hi_u32 adsp_buf_used_perc_proc(hi_u32 size, hi_u32 used_size)
{
    if (size == 0) {
        return 0;
    } else {
        return used_size * ADSP_PERCENTAGE_RATIO / size;
    }
}

static hi_u32 adsp_buf_latency_ms_proc(hi_u32 used_size, hi_u32 frame_size, hi_u32 rate)
{
    if ((frame_size == 0) || (rate == 0)) {
        return 0;
    } else {
        return (used_size / frame_size) * ADSP_MS_PER_SECOND / rate;
    }
}

static hi_u32 adsp_frame_size_proc(hi_u32 bit_width, hi_u32 channel)
{
    if (bit_width == HI_BIT_DEPTH_16) {
        return channel * sizeof(hi_s16);
    } else {
        return channel * sizeof(hi_s32);
    }
}

static hi_void adsp_proc_get_aip_rw_pos(aip_regs_type *aip_reg, hi_u32 *read, hi_u32 *write)
{
    hi_s32 ret;
    hi_u32 *pos = HI_NULL;
    aoe_fw_info fw_info = { HI_TRUE, HI_NULL, 0 };

    *read = 0;
    *write = 0;

    ret = adsp_get_aoe_firmware_info(&fw_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_get_aoe_firmware_info, ret);
        return;
    }

    if (aip_reg->AIP_BUF_SIZE.bits.buff_flag == 0) {
        pos = (hi_u32 *)(&aip_reg->AIP_BUF_RPTR.ADDR_LOW);
        *read = *pos;
        pos = (hi_u32 *)(&aip_reg->AIP_BUF_WPTR.ADDR_LOW);
        *write = *pos;
        return;
    }

    if (fw_info.aoe_sw_flag == HI_FALSE) {
        pos = osal_ioremap_nocache(aip_reg->AIP_BUF_RPTR.ADDR_LOW, sizeof(hi_u32));
        if (pos == HI_NULL) {
            HI_LOG_ERR("call osal_ioremap_nocache failed\n");
            return;
        }

        *read = *pos;
        osal_iounmap(pos);

        pos = osal_ioremap_nocache(aip_reg->AIP_BUF_WPTR.ADDR_LOW, sizeof(hi_u32));
        if (pos == HI_NULL) {
            HI_LOG_ERR("call osal_ioremap_nocache failed\n");
            return;
        }

        *write = *pos;
        osal_iounmap(pos);
    } else {
        adsp_read_addr(aip_reg->AIP_BUF_RPTR, pos);
        *read = *pos;
        adsp_read_addr(aip_reg->AIP_BUF_WPTR, pos);
        *write = *pos;
    }
}

static hi_void adsp_proc_get_aop_rw_pos(aop_regs_type *aop_reg, hi_u32 *read, hi_u32 *write)
{
    hi_s32 ret;
    hi_u32 *pos = HI_NULL;
    aoe_fw_info fw_info = { HI_TRUE, HI_NULL, 0 };

    *read = 0;
    *write = 0;

    ret = adsp_get_aoe_firmware_info(&fw_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_get_aoe_firmware_info, ret);
        return;
    }

    if (aop_reg->AOP_BUF_SIZE.bits.buff_flag == 0) {
        /* aop is cast */
        pos = (hi_u32 *)(&aop_reg->AOP_BUF_RPTR);
        *read = *pos;
        pos = (hi_u32 *)(&aop_reg->AOP_BUF_WPTR);
        *write = *pos;
        return;
    }

    if (fw_info.aoe_sw_flag == HI_FALSE) {
        pos = osal_ioremap_nocache(aop_reg->AOP_BUF_RPTR.ADDR_LOW, sizeof(hi_u32));
        if (pos == HI_NULL) {
            HI_LOG_ERR("call osal_ioremap_nocache failed\n");
            return;
        }

        *read = *pos;
        osal_iounmap(pos);

        pos = osal_ioremap_nocache(aop_reg->AOP_BUF_WPTR.ADDR_LOW, sizeof(hi_u32));
        if (pos == HI_NULL) {
            HI_LOG_ERR("call osal_ioremap_nocache failed\n");
            return;
        }

        *write = *pos;
        osal_iounmap(pos);
    } else {
        adsp_read_addr(aop_reg->AOP_BUF_RPTR, pos);
        *read = *pos;
        adsp_read_addr(aop_reg->AOP_BUF_WPTR, pos);
        *write = *pos;
    }
}

static hi_void adsp_aoe_proc_read_karaoke(hi_void *seq_file)
{
    hi_u32 used_size;
    hi_u32 frame_size;
    hi_u32 aip = 0;
    hi_u32 read = 0;
    hi_u32 write = 0;
    aip_regs_type *aip_reg = HI_NULL;
    aop_regs_type *aop_reg = HI_NULL;

    for (aip = 0; aip < AOE_MAX_AIP_NUM; aip++) {
        aip_reg = (aip_regs_type *)(g_aoe_reg_addr + AOE_AIP_REG_OFFSET + aip * sizeof(aip_regs_type));

        /* find karaoke aip */
        if (aip_reg->AIP_FIFO_ATTR.bits.fifo_bypass == 1) {
            break;
        }
    }

    if (aip == AOE_MAX_AIP_NUM) {
        return;
    }

    adsp_proc_get_aip_rw_pos(aip_reg, &read, &write);
    used_size = adsp_buf_used_size_proc(
                    aip_reg->AIP_BUF_SIZE.bits.buff_size,
                    read, write);
    frame_size = adsp_frame_size_proc(
                    adsp_aoe_width_proc(aip_reg->AIP_BUFF_ATTR.bits.aip_precision),
                    adsp_aoe_ch_proc(aip_reg->AIP_BUFF_ATTR.bits.aip_ch));
    osal_proc_print(seq_file, "karaoke     :aip(%d), size/used/latency(0x%.6x/%d%%/%dms)\n", aip,
        aip_reg->AIP_BUF_SIZE.bits.buff_size,
        adsp_buf_used_perc_proc(aip_reg->AIP_BUF_SIZE.bits.buff_size, used_size),
        adsp_buf_latency_ms_proc(used_size, frame_size,
            adsp_aoe_rate_proc(aip_reg->AIP_BUFF_ATTR.bits.aip_fs)));

    aop_reg = (aop_regs_type *)(g_aoe_reg_addr + AOE_AOP_REG_OFFSET);

    adsp_proc_get_aop_rw_pos(aop_reg, &read, &write);
    used_size = adsp_buf_used_size_proc(
                    aop_reg->AOP_BUF_SIZE.bits.buff_size,
                    read, write);
    frame_size = adsp_frame_size_proc(
                        adsp_aoe_width_proc(aop_reg->AOP_BUFF_ATTR.bits.buf_precision),
                        adsp_aoe_ch_proc(aop_reg->AOP_BUFF_ATTR.bits.buf_ch));
    osal_proc_print(seq_file, "            :aop(0), size/used/latency(0x%.4x/%d%%/%dms)\n",
        aop_reg->AOP_BUF_SIZE.bits.buff_size,
        adsp_buf_used_perc_proc(aop_reg->AOP_BUF_SIZE.bits.buff_size, used_size),
        adsp_buf_latency_ms_proc(
            used_size, frame_size,
            adsp_aoe_rate_proc(aop_reg->AOP_BUFF_ATTR.bits.buf_fs)));
}

static hi_void adsp_aoe_proc_read_aip(hi_void *seq_file, mixer_regs_type *engine_reg)
{
    hi_u32 aip = 0;
    hi_u32 used_size = 0;
    hi_u32 frame_size = 0;

    hi_u32 read = 0;
    hi_u32 write = 0;

    hi_void *tmp_addr = HI_NULL;
    aip_regs_type *aip_reg = HI_NULL;

    for (aip = 0; aip < AOE_MAX_AIP_NUM; aip++) {
        if (!((engine_reg->ENGINE_MIX_SRC.bits.aip_fifo_ena) & (1L << aip))) {
            continue;
        }
        aip_reg = (aip_regs_type *)(g_aoe_reg_addr + AOE_AIP_REG_OFFSET +
                                      aip * sizeof(aip_regs_type));
        osal_proc_print(seq_file,
            "aip(%d)      :status(%s), mute(%s), vol(%ddB/%ddB), prescale(%s%d.%ddB),"
            " ch_mode(%d), speed_adj_cnt(%d), add_mute_cnt(%d)\n",
            aip, adsp_aoe_status_name(aip_reg->AIP_CTRL.bits.cmd, aip_reg->AIP_CTRL.bits.cmd_done),
            (aip_reg->AIP_EXT_CTRL.bits.mute == 0) ? "off" : "on",
            (hi_s32)(aip_reg->AIP_EXT_CTRL.bits.volume_l) - 0x79,
            (hi_s32)(aip_reg->AIP_EXT_CTRL.bits.volume_r) - 0x79,
            ((aip_reg->AIP_EXT2_CTRL.bits.prescale_int == 0x79) &&
            (aip_reg->AIP_EXT2_CTRL.bits.prescale_dec_dir == 1)) ? "-" : "",
            (hi_s32)(aip_reg->AIP_EXT2_CTRL.bits.prescale_int) - 0x79,
            aip_reg->AIP_EXT2_CTRL.bits.prescale_dec * 125,
            aip_reg->AIP_EXT_CTRL.bits.channel_mode, aip_reg->AIP_FSADJ_CNT,
            aip_reg->AIP_ADD_MUTE_CNT);

        adsp_proc_get_aip_rw_pos(aip_reg, &read, &write);

        used_size = adsp_buf_used_size_proc(
                        aip_reg->AIP_BUF_SIZE.bits.buff_size,
                        read, write);
        frame_size = adsp_frame_size_proc(
                        adsp_aoe_width_proc(aip_reg->AIP_BUFF_ATTR.bits.aip_precision),
                        adsp_aoe_ch_proc(aip_reg->AIP_BUFF_ATTR.bits.aip_ch));
        adsp_read_addr(aip_reg->AIP_BUF_ADDR, tmp_addr);
        osal_proc_print(seq_file,
            "            :buff format(%s), rate(%.6d), ch(%d), width(%d), addr(0x%p),"
            " size/used/latency(0x%.6x/%d%%/%dms), rptr/wptr(0x%.6x/0x%.6x)\n",
            (aip_reg->AIP_BUFF_ATTR.bits.aip_format == 0) ? "pcm" : "iec61937",
            adsp_aoe_rate_proc(aip_reg->AIP_BUFF_ATTR.bits.aip_fs),
            adsp_aoe_ch_proc(aip_reg->AIP_BUFF_ATTR.bits.aip_ch),
            adsp_aoe_width_proc(aip_reg->AIP_BUFF_ATTR.bits.aip_precision),
            tmp_addr, aip_reg->AIP_BUF_SIZE.bits.buff_size,
            adsp_buf_used_perc_proc(aip_reg->AIP_BUF_SIZE.bits.buff_size, used_size),
            adsp_buf_latency_ms_proc(
            used_size, frame_size,
            adsp_aoe_rate_proc(aip_reg->AIP_BUFF_ATTR.bits.aip_fs)),
            read,
            write);

        if (engine_reg->ENGINE_ATTR.bits.format == 0) {
            used_size = adsp_buf_used_size_proc(
                            aip_reg->AIP_FIFO_SIZE.bits.aip_fifo_size,
                            aip_reg->AIP_FIFO_RPTR, aip_reg->AIP_FIFO_WPTR);
            frame_size = adsp_frame_size_proc(
                            adsp_aoe_width_proc(aip_reg->AIP_FIFO_ATTR.bits.fifo_precision),
                            adsp_aoe_ch_proc(aip_reg->AIP_FIFO_ATTR.bits.fifo_ch));
            osal_proc_print(seq_file,
                "            :fifo format(%s), rate(%.6d), ch(%d), width(%d), addr(0x%.8x),"
                " size/used/latency/underflow(0x%.4x/%d%%/%dms/%d), rptr/wptr(0x%.4x/0x%.4x)\n",
                (aip_reg->AIP_FIFO_ATTR.bits.fifo_format == 0) ? "pcm" : "iec61937",
                adsp_aoe_rate_proc(aip_reg->AIP_FIFO_ATTR.bits.fifo_fs),
                adsp_aoe_ch_proc(aip_reg->AIP_FIFO_ATTR.bits.fifo_ch),
                adsp_aoe_width_proc(aip_reg->AIP_FIFO_ATTR.bits.fifo_precision),
                aip_reg->AIP_FIFO_ADDR, aip_reg->AIP_FIFO_SIZE.bits.aip_fifo_size,
                adsp_buf_used_perc_proc(aip_reg->AIP_FIFO_SIZE.bits.aip_fifo_size, used_size),
                adsp_buf_latency_ms_proc(used_size, frame_size,
                    adsp_aoe_rate_proc(aip_reg->AIP_FIFO_ATTR.bits.fifo_fs)),
                aip_reg->AIP_FIFO_SIZE.bits.aip_fifo_underflow_cnt,
                aip_reg->AIP_FIFO_RPTR, aip_reg->AIP_FIFO_WPTR);
        }
    }
}

static hi_void adsp_aoe_proc_read_aop_cast(hi_void *seq_file, hi_u32 aop)
{
    hi_u32 cast_id;
    hi_u32 used_size = 0;
    hi_u32 frame_size;
    aop_regs_type *aop_reg = HI_NULL;
    cast_regs_type *cast_reg = HI_NULL;

    aop_reg = (aop_regs_type *)(g_aoe_reg_addr + AOE_AOP_REG_OFFSET + aop * sizeof(aop_regs_type));
    if (aop_reg->AOP_CTRL.bits.cast_flag == 0) {
        /* aop is not cast, return */
        return;
    }
    frame_size = adsp_frame_size_proc(
                    adsp_aoe_width_proc(aop_reg->AOP_BUFF_ATTR.bits.buf_precision),
                    adsp_aoe_ch_proc(aop_reg->AOP_BUFF_ATTR.bits.buf_ch));

    for (cast_id = 0; cast_id < AOE_MAX_CAST_NUM; cast_id++) {
        cast_reg = (cast_regs_type *)(g_aoe_reg_addr + AOE_CAST_REG_OFFSET +
                                        cast_id * sizeof(cast_regs_type));
        if (cast_reg->CAST_AOP_ID != aop) {
            continue;
        }

        osal_proc_print(seq_file,
            "            :buff_writ_cnt(total/nomal/add_mute)(%.6u/%.6u/%.6u), buff_over_write_cnt(%.6u)\n",
            cast_reg->CAST_BUF_WRITETOTAL_CNT, cast_reg->CAST_BUF_WRITEOK_CNT,
            cast_reg->CAST_BUF_WRITEMUTE_CNT, cast_reg->CAST_BUF_OVERWRITE_CNT);

        used_size = adsp_buf_used_size_proc(
                        cast_reg->CAST_FIFO_SIZE, cast_reg->CAST_FIFO_RPTR,
                        cast_reg->CAST_FIFO_WPTR);
        osal_proc_print(seq_file,
            "            :FIFO size/used/latency(0x%.4x/%d%%/%dms), rptr/wptr(0x%.4x/0x%.4x),"
            " fifo_write(try/OK)(%.6u/%.6u), force_fifo_read_cnt(%.6u)\n",
            cast_reg->CAST_FIFO_SIZE,
            adsp_buf_used_perc_proc(cast_reg->CAST_FIFO_SIZE, used_size),
            adsp_buf_latency_ms_proc(
            used_size, frame_size,
            adsp_aoe_rate_proc(aop_reg->AOP_BUFF_ATTR.bits.buf_fs)),
            cast_reg->CAST_FIFO_RPTR,
            cast_reg->CAST_FIFO_WPTR,
            cast_reg->CAST_FIFO_TRY_CNT,
            cast_reg->CAST_FIFO_OK_CNT,
            cast_reg->CAST_FIFO_FORCE_READ_CNT);
    }
}

static hi_void adsp_aoe_proc_read_aop(hi_void *seq_file, mixer_regs_type *engine_reg)
{
    hi_u32 used_size = 0;
    hi_u32 frame_size = 0;
    hi_u32 aop = 0;
    hi_u32 read = 0;
    hi_u32 write = 0;
    hi_void *tmp_addr = HI_NULL;
    aop_regs_type *aop_reg = HI_NULL;

    for (aop = 0; aop < AOE_MAX_AOP_NUM; aop++) {
        if (!((engine_reg->ENGINE_ROU_DST.bits.aop_buf_ena) & (1L << aop))) {
            continue;
        }

        aop_reg = (aop_regs_type *)(g_aoe_reg_addr + AOE_AOP_REG_OFFSET +
                                      aop * sizeof(aop_regs_type));
        if (aop_reg->AOP_CTRL.bits.cast_flag == 1) {  /* aop is cast */
            osal_proc_print(seq_file,
                "aop(%d,cast) :status(%s), add_mute(%s), aef_bypass(%s), vol(%ddB/%ddB),"
                " mute(%s), latency(%dms), delay_comps(%dms)\n",
                aop, adsp_aoe_status_name(aop_reg->AOP_CTRL.bits.cmd, aop_reg->AOP_CTRL.bits.cmd_done),
                (aop_reg->AOP_CTRL.bits.ena_add_mute == 1) ? "on" : "off",
                (aop_reg->AOP_CTRL.bits.ena_aef_bypass == 0) ? "off" : "on",
                (hi_s32)(aop_reg->AOP_EXT_CTRL.bits.volume_l) - 0x79,
                (hi_s32)(aop_reg->AOP_EXT_CTRL.bits.volume_l) - 0x79,
                (aop_reg->AOP_EXT_CTRL.bits.mute == 0) ? "off" : "on",
                aop_reg->AOP_BUFF_ATTR.bits.buf_latency,
                aop_reg->AOP_CTRL.bits.realdelayms);
        } else {
            osal_proc_print(seq_file,
                "aop(%d)      :status(%s), aef_bypass(%s), vol(%s%d.%ddB/%s%d.%ddB), balance(%d), "
                "mute(%s), latency(%dms), AD(%s), delay_comps(%dms)\n",
                aop, adsp_aoe_status_name(aop_reg->AOP_CTRL.bits.cmd, aop_reg->AOP_CTRL.bits.cmd_done),
                (aop_reg->AOP_CTRL.bits.ena_aef_bypass == 0) ? "off" : "on",
                ((aop_reg->AOP_EXT_CTRL.bits.volume_l == 0x79) &&
                (aop_reg->AOP_EXT_CTRL.bits.volumedec_dir_l == 1)) ? "-" : "",
                (hi_s32)(aop_reg->AOP_EXT_CTRL.bits.volume_l) - 0x79,
                aop_reg->AOP_EXT_CTRL.bits.volumedec_l * 125,
                ((aop_reg->AOP_EXT_CTRL.bits.volume_r == 0x79) &&
                (aop_reg->AOP_EXT_CTRL.bits.volumedec_dir_r == 1)) ? "-" : "",
                (hi_s32)(aop_reg->AOP_EXT_CTRL.bits.volume_r) - 0x79,
                aop_reg->AOP_EXT_CTRL.bits.volumedec_r * 125,
                (aop_reg->AOP_EXT_CTRL.bits.balance_dir == 0) ? aop_reg->AOP_EXT_CTRL.bits.balance_val :
                -aop_reg->AOP_EXT_CTRL.bits.balance_val,
                (aop_reg->AOP_EXT_CTRL.bits.mute == 0) ? "off" : "on",
                aop_reg->AOP_BUFF_ATTR.bits.buf_latency,
                (aop_reg->AOP_CTRL.bits.ena_ad == 0) ? "off" : "on",
                aop_reg->AOP_CTRL.bits.realdelayms);
        }

        adsp_proc_get_aop_rw_pos(aop_reg, &read, &write);
        used_size = adsp_buf_used_size_proc(
                        aop_reg->AOP_BUF_SIZE.bits.buff_size,
                        read, write);
        frame_size = adsp_frame_size_proc(
                        adsp_aoe_width_proc(aop_reg->AOP_BUFF_ATTR.bits.buf_precision),
                        adsp_aoe_ch_proc(aop_reg->AOP_BUFF_ATTR.bits.buf_ch));
        adsp_read_addr(aop_reg->AOP_BUF_ADDR, tmp_addr);
        osal_proc_print(seq_file,
            "            :buff format(%s), rate(%.6d), ch(%d), width(%d), addr(0x%p), "
            "size/used/latency(0x%.4x/%d%%/%dms), rptr/wptr(0x%.4x/0x%.4x)\n",
            aop_reg->AOP_BUFF_ATTR.bits.buf_format == 0 ? "pcm" : "iec61937",
            adsp_aoe_rate_proc(aop_reg->AOP_BUFF_ATTR.bits.buf_fs),
            adsp_aoe_ch_proc(aop_reg->AOP_BUFF_ATTR.bits.buf_ch),
            adsp_aoe_width_proc(aop_reg->AOP_BUFF_ATTR.bits.buf_precision), tmp_addr,
            aop_reg->AOP_BUF_SIZE.bits.buff_size,
            adsp_buf_used_perc_proc(aop_reg->AOP_BUF_SIZE.bits.buff_size, used_size),
            adsp_buf_latency_ms_proc(
                used_size, frame_size,
                adsp_aoe_rate_proc(aop_reg->AOP_BUFF_ATTR.bits.buf_fs)),
            read, write);
        if (aop_reg->AOP_CTRL.bits.audiodelay) {
            osal_proc_print(seq_file, "            :add_mute_cnt(%.6u)\n", aop_reg->AOP_ADDMUTE_CNT);
        }

        adsp_aoe_proc_read_aop_cast(seq_file, aop);
    }
}

static hi_void adsp_aoe_proc_read_engine(hi_void *seq_file,
    hi_u32 engine_id, mixer_regs_type *engine_reg)
{
    osal_proc_print(seq_file,
        "ENGINE(%d)   :status(%s), format(%s), rate(%d), ch(%d), width(%d), aef_mask(0x%x)",
        engine_id,
        adsp_aoe_status_name(
            engine_reg->ENGINE_CTRL.bits.cmd,
            engine_reg->ENGINE_CTRL.bits.cmd_done),
        engine_reg->ENGINE_ATTR.bits.format == 0 ? "pcm" : "iec61937",
        adsp_aoe_rate_proc(engine_reg->ENGINE_ATTR.bits.fs),
        adsp_aoe_ch_proc(engine_reg->ENGINE_ATTR.bits.ch),
        adsp_aoe_width_proc(engine_reg->ENGINE_ATTR.bits.precision),
        engine_reg->ENGINE_ATT_AEF.bits.aef_att_ena);

    if (engine_reg->ENGINE_ATT_AEF.bits.aef_att_ena & 0xf0) {
        osal_proc_print(seq_file, ", fifo_delay_num(%d)\n", engine_reg->ENGINE_ATTR.bits.aef_delay);
    } else {
        osal_proc_print(seq_file, "\n");
    }
}

static hi_void adsp_aoe_proc(hi_void *seq_file, hi_void *private)
{
    hi_u32 i;
    hi_s32 ret;
    aoe_regs_type *com_reg = HI_NULL;
    mixer_regs_type *engine_reg = HI_NULL;
    aoe_fw_info fw_info = { HI_TRUE, HI_NULL, 0 };

    ret = adsp_get_aoe_firmware_info(&fw_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_get_aoe_firmware_info, ret);
        return;
    }

    osal_proc_print(seq_file,
        "\n------------------------------------"
        "  AOE status  "
        "------------------------------------\n");

    com_reg = (aoe_regs_type *)g_aoe_reg_addr;
    osal_proc_print(seq_file,
        "common_status   :execute_time_out_cnt(%d), schedule_time_out_cnt(%d), AOE(%s), "
        "aoe_reg_phys_addr(0x%llx)\n",
        com_reg->AOE_STATUS1.bits.excute_timeout_cnt,
        com_reg->AOE_STATUS1.bits.schedule_timeout_cnt,
        (fw_info.aoe_sw_flag == HI_TRUE) ? "sw" : "dsp",
        fw_info.aoe_reg_phy_addr);

    for (i = 0; i < AOE_MAX_ENGINE_NUM; i++) {
        engine_reg = (mixer_regs_type *)(g_aoe_reg_addr + AOE_ENGINE_REG_OFFSET + i * sizeof(mixer_regs_type));
        if (((engine_reg->ENGINE_CTRL.bits.cmd == 0) ||
             (engine_reg->ENGINE_CTRL.bits.cmd == 0x02)) &&
            (engine_reg->ENGINE_CTRL.bits.cmd_done == 0x01)) {
                adsp_aoe_proc_read_engine(seq_file, i, engine_reg);
                adsp_aoe_proc_read_aip(seq_file, engine_reg);
                adsp_aoe_proc_read_aop(seq_file, engine_reg);

            osal_proc_print(seq_file, "\n");
        }
    }

    adsp_aoe_proc_read_karaoke(seq_file);
}

static hi_void adsp_com_proc(hi_void *seq_file, hi_void *private)
{
    hi_u32 j;
    hi_s32 ret;
    aoe_fw_info fw_info = { HI_TRUE, HI_NULL, 0 };
    volatile adsp_chn_regs_type *adsp_sys = HI_NULL;

    ret = adsp_get_aoe_firmware_info(&fw_info);
    if (ret != HI_SUCCESS) {
        HI_ERR_PRINT_FUNC_RES(adsp_get_aoe_firmware_info, ret);
        return;
    }

    adsp_sys = (volatile adsp_chn_regs_type *)(fw_info.aoe_reg_vir_addr + DSP0_SHARESRAM_SYS_OFFSET);

    osal_proc_print(seq_file,
        "------------------------------------"
        "  common status  "
        "------------------------------------\n");
    osal_proc_print(seq_file, "debug_value     :");
    for (j = 0; j < DSP_DEBUG_REG_NUM; j++) {
        osal_proc_print(seq_file, "debug%d(0x%x/%d)", j, adsp_sys->ADSP_DEBUG[j], adsp_sys->ADSP_DEBUG[j]);
        if ((j + 1) % 4) {  /* a line 4 reg */
            osal_proc_print(seq_file, ", ");
        } else {
            osal_proc_print(seq_file, "\n");
        }
    }

    osal_proc_print(seq_file, "\n");
}

hi_s32 adsp_drv_read_proc(hi_void *seq_file, hi_void *private)
{
    if (g_aoe_reg_addr == HI_NULL) {
        osal_proc_print(seq_file, "\n-------------  ADSP NOT OPEN  -------------\n");
        return HI_SUCCESS;
    }

    adsp_aoe_proc(seq_file, private);

    adsp_com_proc(seq_file, private);

    return HI_SUCCESS;
}

hi_void adsp_drv_register_proc(hi_void)
{
    osal_proc_entry *item = osal_proc_add("adsp", strlen("adsp"));
    if (item == HI_NULL) {
        HI_LOG_FATAL("create adsp proc entry fail!\n");
        return;
    }

    item->read = adsp_drv_read_proc;
    item->cmd_list = HI_NULL;
    item->cmd_cnt = 0;
    item->private = HI_NULL;
}

hi_void adsp_drv_unregister_proc(hi_void)
{
    osal_proc_remove("adsp", strlen("adsp"));
}

hi_void adsp_proc_deinit(hi_void)
{
    g_aoe_reg_addr = HI_NULL;
}

hi_s32 adsp_proc_init(hi_u8 *aoe_reg_vir_addr)
{
    if (aoe_reg_vir_addr == HI_NULL) {
        HI_LOG_ERR("DSP share sram ptr is NULL\n");
        return HI_FAILURE;
    }

    g_aoe_reg_addr = aoe_reg_vir_addr + DSP0_SHARESRAM_AOE_OFFSET;

    return HI_SUCCESS;
}
