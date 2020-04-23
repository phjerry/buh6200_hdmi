/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Implementation of packet functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#include "hal_hdmirx_ctrl.h"
#include "hal_hdmirx_video.h"
#include "drv_hdmirx_ctrl.h"
#include "drv_hdmirx_edid.h"
#include "drv_hdmirx_video.h"
#include "drv_hdmirx_audio.h"
#include <securec.h>

static hdmirx_packet_context g_hdmirx_packet_ctx_v2[HI_DRV_HDMIRX_PORT_MAX];
static osal_timeval g_acp_time = { 0 };

static hi_void hdmirx_packet_av_mute_req(hi_drv_hdmirx_port port, hi_bool en, hi_u32 cause)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    if (cause > 0) {
        if (en == HI_TRUE) {
            packet_ctx->av_mask |= cause;
        } else {
            packet_ctx->av_mask &= ~cause;
        }
    }
}

static hi_bool hdmirx_packet_is_check_sum_ok(hi_u8 *data, hi_u32 length, hi_u32 max_len)
{
    hi_u8 i;
    hi_u8 check_sum = 0;

    if (length > max_len) {
        return HI_FAILURE;
    }

    for (i = 0; i < length; i++, data++) {
        check_sum += (hi_u8)((*data) & 0xff);
    }

    return (check_sum == 0);
}

static hi_void hdmirx_packet_set_acp_int_en(hi_drv_hdmirx_port port, hi_bool en)
{
}

hi_bool hdmirxv2_packet_vsif_is_got3d(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);

    return (packet_ctx->vsif.hdmi3d_vsif_received);
}

hi_bool hdmirxv2_packet_vsif_is_got_dolby(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);

    return (packet_ctx->vsif.hdmi_dolby15_received);
}

hi_bool hdmirxv2_packet_vsif_is_got_hdr10_plus(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);

    return (packet_ctx->vsif.hdmi_hdr10_plus_received);
}

hi_hdmirx_source_input_type hdmirxv2_packet_vsif_get_dolby_type(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;
    hi_u32 value;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    if (packet_ctx->vsif.hdmi_dolby15_received == HI_FALSE) {
        return HI_HDMIRX_SOURCE_INPUT_TYPE_UNKNOWN;
    }
    value = packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 3]; /* 3: get dolby type from head 3st byte */
    value = (value >> 1) & 0x1;
    if (value == 1) {
        return HI_HDMIRX_SOURCE_INPUT_TYPE_DOLBY;
    } else {
        return HI_HDMIRX_SOURCE_INPUT_TYPE_SDR;
    }
}

hi_hdmirx_source_input_type hdmirxv2_packet_vsif_get_hdr10_plus_type(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    if (packet_ctx->vsif.hdmi_hdr10_plus_received == HI_FALSE) {
        hi_dbg_hdmirx("hdmirxv2_packet_vsif_get_hdr10_plus_type HI_HDMIRX_SOURCE_INPUT_TYPE_UNKNOWN\n");
        return HI_HDMIRX_SOURCE_INPUT_TYPE_UNKNOWN;
    } else {
        hi_dbg_hdmirx("hdmirxv2_packet_vsif_get_hdr10_plus_type HI_HDMIRX_SOURCE_INPUT_TYPE_HDR10PLUS\n");
        return HI_HDMIRX_SOURCE_INPUT_TYPE_HDR10PLUS;
    }
}

hi_void hdmirxv2_packet_vsif_get_distribution_info(hi_hdmirx_hdr10_plus_dynamic_metadata_info *hdr10_plus_info,
    hdmirx_packet_context *packet_ctx)
{
    hdr10_plus_info->distribution_values[0] =
        packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 5]; /* 5: get distribution_info from head 5st bytes */

    hdr10_plus_info->distribution_values[1] =
        packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 6]; /* 6: get distribution_info from head 6st bytes */

    hdr10_plus_info->distribution_values[2] =             /* 2: set 3st distribution_values */
        packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 7]; /* 7: get distribution_info from head 7st bytes */

    hdr10_plus_info->distribution_values[3] =             /* 3: set 4st distribution_values */
        packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 8]; /* 8: get distribution_info from head 8st bytes */

    hdr10_plus_info->distribution_values[4] =             /* 4: set 5st distribution_values */
        packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 9]; /* 9: get distribution_info from head 9st bytes */

    hdr10_plus_info->distribution_values[5] =              /* 5: set 6st distribution_values */
        packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 10]; /* 10: get distribution_info from head 10st bytes */

    hdr10_plus_info->distribution_values[6] =              /* 6: set 7st distribution_values */
        packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 11]; /* 11: get distribution_info from head 11st bytes */

    hdr10_plus_info->distribution_values[7] =              /* 7: set 8st distribution_values */
        packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 12]; /* 12: get distribution_info from head 12st bytes */

    hdr10_plus_info->distribution_values[8] =              /* 8: set 9st distribution_values */
        packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 13]; /* 13: get distribution_info from head 13st bytes */
}

hi_s32 hdmirxv2_packet_vsif_get_hdr10_plus_info(hi_drv_hdmirx_port port,
    hi_hdmirx_hdr10_plus_dynamic_metadata_info *hdr10_plus_info)
{
    hdmirx_packet_context *packet_ctx;
    hi_u8 i;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    if (packet_ctx->vsif.hdmi_hdr10_plus_received == HI_FALSE) {
        errno_t err_ret = memset_s(hdr10_plus_info, sizeof(hi_hdmirx_hdr10_plus_dynamic_metadata_info),
            0, sizeof(hi_hdmirx_hdr10_plus_dynamic_metadata_info));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            return err_ret;
        }
        return HI_SUCCESS;
    }
    hdr10_plus_info->application_version =
        (packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 3] >> 6) & 0x03; /* 3, 6: 8st vsif data get high 2 bits */

    hdr10_plus_info->targeted_system_display_maximum_luminance =
        (packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 3] >> 1) & 0x1f; /* 3: 8st vsif data get high 7 bits */

    hdr10_plus_info->average_maxrgb =
        packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 4]; /* 4: 9st vsif data */

    hdmirxv2_packet_vsif_get_distribution_info(hdr10_plus_info, packet_ctx);

    hdr10_plus_info->num_bezier_curve_anchors =
        (packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 14] >> 4) & 0x0f; /* 14, 4: 19st vsif data get high 4 bits */

    hdr10_plus_info->knee_point_x =
        ((packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 14] & 0x0f) << 6) & /* 14, 6: 19st vsif data left mv 6bits */
        0x000003c0;

    hdr10_plus_info->knee_point_x +=
        (packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 15] >> 2) & 0x3f; /* 15, 2: 20st vsif data get high 6 bits */

    hdr10_plus_info->knee_point_y =
        ((packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 15] & 0x03) << 8) & /* 15, 8: 20st vsif data left mv 8bits */
        0x00000300;

    hdr10_plus_info->knee_point_y +=
        (packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 16]) & 0xff; /* 16: 21st vsif data */

    for (i = 0; i < HDR_ARRAY_SIZE; i++) {
        hdr10_plus_info->bezier_curve_anchors[i] =
            packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 17 + i]; /* 17: 22st vsif data */
    }
    hdr10_plus_info->graphics_overlayflag =
        (packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 26] >> 7) & 0x1; /* 26, 7: 31st vsif data get high 1 bits */

    hdr10_plus_info->no_delay_flag =
        (packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 26] >> 6) & 0x1; /* 26, 6: 31st vsif data get high 2 bits */
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_packet_vsif_get_dolby_info(hi_drv_hdmirx_port port, hi_hdmirx_dolby_info *dolby_info)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    if (packet_ctx->vsif.hdmi_dolby15_received == HI_FALSE) {
        errno_t err_ret = memset_s(dolby_info, sizeof(hi_hdmirx_dolby_info), 0, sizeof(hi_hdmirx_dolby_info));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            return err_ret;
        }
        return HI_SUCCESS;
    }
    dolby_info->low_latency = packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 3] & 0x1; /* 3: 8st vsif data */
    dolby_info->backlt_ctrl_enable =
        packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 4] >> 7; /* 4, 7: 9st vsif data get high 1 bit */
    dolby_info->auxiliary_enable =
        (packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 4] >> 6) & 0x1; /* 4, 6: 9st vsif data get higher 2st bit */
    if (dolby_info->backlt_ctrl_enable == HI_FALSE) {
        dolby_info->eff_max_luminance = 0;
    } else {
        dolby_info->eff_max_luminance =
            ((packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 4] & 0xf) << 8) + /* 9st v-data low 4 bit left mv 8 bits */
            packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 5]; /* 5: 10st vsif data */
    }
    if (dolby_info->auxiliary_enable == HI_FALSE) {
        dolby_info->auxiliary_debug = 0;
        dolby_info->auxiliary_runmode = 0;
        dolby_info->auxiliary_runversion = 0;
    } else {
        dolby_info->auxiliary_debug = packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 8]; /* 8: 13st vsif data */
        dolby_info->auxiliary_runmode = packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 6]; /* 6: 11st vsif data */
        dolby_info->auxiliary_runversion = packet_ctx->vsif.vsif_data[IF_HEADER_LENGTH + 7]; /* 7: 12st vsif data */
    }
    return HI_SUCCESS;
}

vsif_check_result hdmirx_packet_vsif_get_3d_type(hi_drv_hdmirx_port port, hi_u8 *packet, hi_u32 length)
{
    vsif_check_result analysis = VSIF_OLD_3D;

    /* may be modified by the next few lines. check if new 3D structure field matches
        previously received packet. */
    if (hdmirxv2_video_get_cur3d_structure(port) != (packet[4] >> 4)) { /* 4: 5st vsif data high 4 bits */
        /* 3_d_structure is different; the packet is new 3D. */
        analysis = VSIF_NEW_3D;
    }
    /* side-by-side (half) has at least one additional parameter. */
    /* check if it matches to the previously received one. */
    /* 8: check cur3d structure, 4: length check */
    if ((hdmirxv2_video_get_cur3d_structure(port) == 8) && (length > 4)) {
        /* 3_d_ext_data field */
        if (hdmirxv2_video_get_cur3d_ext_data(port) != (packet[5] >> 4)) { /* 5: 6st vsif data high 4 bits */
            /* 3_d_structure is different; the packet is new 3D. */
            analysis = VSIF_NEW_3D;
        }
    }
    return analysis;
}

static vsif_check_result hdmirx_packet_vsif_get_type(hi_drv_hdmirx_port port, hi_u8 *packet, hi_u32 length)
{
    vsif_check_result analysis = VSIF_NOT_HDMI_VSIF;
    hi_u32 format;
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    /* check HDMI VSIF signature. */
    /* HDMI IEEE registration identifier is 0x000C03 (least significant byte first). */
    if ((packet[0] == 0x03) && (packet[1] == 0x0C) && (packet[2] == 0x00)) { /* 2: 3st vsif data */
        /* HDMI VSIF signature is found. check HDMI format. */
        format = (packet[3] >> 5); /* 3, 5: 4st vsif data get high 3 bits */
        if (format == 2) { /* 2: new 3d type */
            analysis = VSIF_NEW_3D;
            if (packet_ctx->vsif.hdmi3d_vsif_received) {
                analysis = hdmirx_packet_vsif_get_3d_type(port, packet, length);
            }
        }
    }
    if ((packet[0] == 0x46) && (packet[1] == 0xD0) && (packet[2] == 0x00)) { /* 2: 3st vsif data */
        /* HDMI VSIF signature is found. check HDMI format. */
        analysis = VSIF_DOLBY;
    }
    if ((packet[0] == 0x8B) && (packet[1] == 0x84) && (packet[2] == 0x90)) { /* 2: 3st vsif data */
        /* HDMI VSIF signature is found. check HDMI format. */
        analysis = VSIF_HDR10PLUS;
    }
    return analysis;
}

static hi_void hdmirx_packet_vsif_process(hi_drv_hdmirx_port port, hi_u8 *packet, hi_u8 u8_length)
{
    hi_u32 hdmi_format;
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    hdmi_format = (packet[3] >> 5); /* 3, 5: 4st vsif data get high 3 bits */
    switch (hdmi_format) {
        case 1: /* 1: check case */
            /* 4k */
            packet_ctx->vsif.hdmi3d_vsif_received = HI_FALSE;
            hdmirxv2_video_set_cur3d_structure(port, HDMIRX_3D_TYPE_BUTT);
            hdmirxv2_video_set_cur3d_ext_data(port, 0);
            break;
        case 2: /* 2: check case */
            /*
             * 3D format
             * get 3D structure field.timing from video_mode_table[] has to be modified
             * depending on the 3d_structure field.store it for further processing.
             */
            hdmirxv2_video_set_cur3d_structure(port, packet[4] >> 4); /* 4: 5st vsif data get high 4 bits */
            /* side-by-side (half) has at least one additional parameter, collect it. */
            if ((hdmirxv2_video_get_cur3d_structure(port) >= 8) && /* 8: check cur3dstructure */
                (u8_length > 4)) { /* 4: length */
                hdmirxv2_video_set_cur3d_ext_data(port, packet[5] >> 4); /* 5: 6st vsif data get high 4 bits */
            } else {
                hdmirxv2_video_set_cur3d_ext_data(port, 0);
            }

            packet_ctx->vsif.hdmi3d_vsif_received = HI_TRUE;
            if (packet_ctx->avi.avi_received) {
                /* 3D video detected. */
                /* RX video state machine has to be informed as video re-detection may be required. */
                hdmirxv2_video_verify1_p4_format(port, HI_TRUE);
            }
            break;
    }
}

hi_s32 hdmirxv2_packet_dolby_get_meta_data(hi_drv_hdmirx_port port, hi_hdmirx_dolby_vision_info *dolby_info)
{
    hdmirx_packet_context *packet_ctx;
    hi_u8 cnt;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    if (dolby_info == HI_NULL) {
        return HI_FAILURE;
    }
    for (cnt = 0; cnt < 31; cnt++) { /* 31: only set front 31 dolby_info */
        dolby_info->dolby_info[cnt] = (hi_u8)(packet_ctx->vsif.vsif_data[cnt] & 0xff);
    }
    return HI_SUCCESS;
}

hi_s32 hdmirxv2_packet_hdr10_plus_get_meta_data(hi_drv_hdmirx_port port, hi_hdmirx_hdr10_plus_info *hdr10_plus_info)
{
    hdmirx_packet_context *packet_ctx;
    hi_u8 cnt;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    if (hdr10_plus_info == HI_NULL) {
        return HI_FAILURE;
    }
    for (cnt = 0; cnt < 31; cnt++) { /* 31: only set front 31 hdr10_info */
        hdr10_plus_info->hdr10_plus_info[cnt] = (hi_u8)(packet_ctx->vsif.vsif_data[cnt] & 0xff);
    }
    return HI_SUCCESS;
}

hi_void hdmirx_packet_vsif_update_info(hi_drv_hdmirx_port port, vsif_check_result type,
    hdmirx_packet_context *packet_ctx, hi_u8 *packet)
{
    hi_bool packet_new = HI_FALSE;

    switch (type) {
        case VSIF_NEW_3D:
        case VSIF_NEW_EXTENDED_RESOLUTION:
            packet_new = HI_TRUE;
            /* fall-through */
        case VSIF_OLD_3D:
        case VSIF_OLD_EXTENDED_RESOLUTION:
            packet_ctx->vsif.found_hdmi_vsif = HI_TRUE;
            if (packet_new) {
                hdmirx_packet_vsif_process(port, &packet[IF_HEADER_LENGTH],
                                           packet[IF_LENGTH_INDEX]);
            }
            packet_ctx->vsif.hdmi3d_vsif_received = HI_TRUE;
            packet_ctx->vsif.hdmi_dolby15_received = HI_FALSE;
            packet_ctx->vsif.hdmi_hdr10_plus_received = HI_FALSE;
            break;
        case VSIF_DOLBY:
            packet_ctx->vsif.hdmi3d_vsif_received = HI_FALSE;
            packet_ctx->vsif.hdmi_dolby15_received = HI_TRUE;
            packet_ctx->vsif.hdmi_hdr10_plus_received = HI_FALSE;
            break;
        case VSIF_HDR10PLUS:
            packet_ctx->vsif.hdmi3d_vsif_received = HI_FALSE;
            packet_ctx->vsif.hdmi_dolby15_received = HI_FALSE;
            packet_ctx->vsif.hdmi_hdr10_plus_received = HI_TRUE;
            break;
        default:
            packet_ctx->vsif.hdmi3d_vsif_received = HI_FALSE;
            packet_ctx->vsif.hdmi_dolby15_received = HI_FALSE;
            packet_ctx->vsif.hdmi_hdr10_plus_received = HI_FALSE;
            break;
    }
}

static hi_void hdmirx_packet_vsif_on_receiving(hi_drv_hdmirx_port port, hi_u8 *packet, hi_u32 len)
{
    hi_u32 length;
    hi_bool chk_sum;
    vsif_check_result type;
    hdmirx_packet_context *packet_ctx;
    hi_bool old_dolby15_received;
    hi_bool old_hdr10_plus_received;
    errno_t err_ret;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    length = packet[IF_LENGTH_INDEX];
    packet_ctx->vsif.vsif_received = HI_TRUE;
    old_dolby15_received = packet_ctx->vsif.hdmi_dolby15_received;
    old_hdr10_plus_received = packet_ctx->vsif.hdmi_hdr10_plus_received;

    /* checksum and length verification */
    if ((length >= IF_MIN_VSIF_LENGTH) && (length <= IF_MAX_VSIF_LENGTH)) {
        chk_sum = hdmirx_packet_is_check_sum_ok(packet, length + IF_HEADER_LENGTH, len);
        if (chk_sum == HI_FALSE) {
            hi_err_hdmirx("VSIF check_sum erro!\n");
            return;
        }
        err_ret = memcpy_s(packet_ctx->vsif.vsif_data, sizeof(packet_ctx->vsif.vsif_data),
            packet, (IF_HEADER_LENGTH + length) * sizeof(hi_u8));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            return;
        }
        type = hdmirx_packet_vsif_get_type(port, &packet[IF_HEADER_LENGTH], packet[IF_LENGTH_INDEX]);

        hdmirx_packet_vsif_update_info(port, type, packet_ctx, packet);

        if (packet_ctx->vsif.hdmi_dolby15_received != old_dolby15_received) {
            hdmirxv2_video_set_stream_data(port);
        } else if (packet_ctx->vsif.hdmi_dolby15_received == HI_TRUE) {
            hdmirxv2_video_set_stream_data(port);
        }

        if (packet_ctx->vsif.hdmi_hdr10_plus_received != old_hdr10_plus_received) {
            hdmirxv2_video_set_stream_data(port);
            hi_warn_hdmirx("packet_ctx->vsif.hdmi_hdr10_plus_received = %d\n", \
                packet_ctx->vsif.hdmi_hdr10_plus_received);
        } else if (packet_ctx->vsif.hdmi_hdr10_plus_received == HI_TRUE) {
            /* not to do hdr10+ metadata realtime change do not need notice vicap */
        }
    }
    hdmirxv2_packet_set_int_new_update(port, HDMIRX_PACKET_VSI, HI_TRUE);
}

static hi_void hdmirx_packet_spd_on_receiving(hi_drv_hdmirx_port port, hi_u8 *packet, hi_u32 len)
{
    hi_u32 length;
    hi_bool chk_sum;
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    length = packet[IF_LENGTH_INDEX];

    packet_ctx->spd.spd_received = HI_TRUE;

    if ((length >= IF_MIN_SPD_LENGTH) && (length <= IF_MAX_SPD_LENGTH)) {
        chk_sum = hdmirx_packet_is_check_sum_ok(packet, length + IF_HEADER_LENGTH, len);
        if (chk_sum == HI_FALSE) {
            return;
        }
        /* the packet looks valid. */
        if (osal_memncmp(packet + 4, sizeof(packet_ctx->spd.spd_buffer), /* 4: compare from 5st bytes */
            packet_ctx->spd.spd_buffer, sizeof(packet_ctx->spd.spd_buffer)) != 0) {
            /*
             * the received packet differs from the previous one. copy received packet into
             * the shadow buffer.
             */
            /* it will be used to compare with the newly coming ones. */
            errno_t err_ret = memcpy_s(packet_ctx->spd.spd_buffer,
                sizeof(packet_ctx->spd.spd_buffer), packet + 4, /* 4: copy from 5st bytes */
                SPD_BUFFER_LENGTH * sizeof(hi_u8));
            if (err_ret != EOK) {
                hi_err_hdmirx("secure func call error\n");
                return;
            }
        }
    }
    hdmirxv2_packet_set_int_new_update(port, HDMIRX_PACKET_SPD, HI_TRUE);
}

hi_bool hdmirxv2_packet_spd_is_got(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);

    return (packet_ctx->spd.spd_received);
}

hi_void hdmirxv2_packet_spd_get_vendor_name(hi_drv_hdmirx_port port, hi_u32 *data, hi_u32 len)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    if ((packet_ctx->spd.spd_received == HI_TRUE) && (data != HI_NULL)) {
        errno_t err_ret = memcpy_s(data, len,
            packet_ctx->spd.spd_buffer, SPD_VENDOR_NAME_LEN * sizeof(hi_u8));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            return;
        }
    }
}

hi_void hdmirxv2_packet_spd_get_product_des(hi_drv_hdmirx_port port, hi_u32 *data, hi_u32 len)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    if ((packet_ctx->spd.spd_received == HI_TRUE) && (data != HI_NULL)) {
        errno_t err_ret = memcpy_s(data, len,
            packet_ctx->spd.spd_buffer + SPD_VENDOR_NAME_LEN, SPD_PRODUCT_DES_LEN * sizeof(hi_u8));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            return;
        }
    }
}

hi_void hdmirxv2_packet_spd_reset_data(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;
    errno_t err_ret;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    err_ret = memset_s(packet_ctx->spd.spd_buffer,
        sizeof(packet_ctx->spd.spd_buffer), 0, sizeof(packet_ctx->spd.spd_buffer));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return;
    }
    packet_ctx->spd.spd_received = HI_FALSE;
}

hi_bool hdmirxv2_packet_avi_is_got(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);

    return (packet_ctx->avi.avi_received);
}

hi_void hdmirxv2_packet_set_int_new_update(hi_drv_hdmirx_port port, hdmirx_ctrl_packet_type type, hi_bool update)
{
    hdmirx_packet_context *packet_ctx = HI_NULL;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    switch (type) {
        case HDMIRX_PACKET_AVI:
            if (packet_ctx->avi.avi_type != update) {
                hi_warn_hdmirx("set port %d avi type :%d", port, update);
            }
            packet_ctx->avi.avi_type = update;
            break;
        case HDMIRX_PACKET_SPD:
            packet_ctx->spd.spd_type = update;
            break;
        case HDMIRX_PACKET_VSI:
            packet_ctx->vsif.vsif_type = update;
            break;
        case HDMIRX_PACKET_UNREC:
            packet_ctx->unrec.unrec_type = update;
            break;
        default:
            update = HI_FALSE;
            break;
    }
}

hi_bool hdmirxv2_packet_get_int_new_update(hi_drv_hdmirx_port port, hdmirx_ctrl_packet_type type)
{
    hdmirx_packet_context *packet_ctx = HI_NULL;
    hi_bool update = HI_FALSE;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    switch (type) {
        case HDMIRX_PACKET_AVI:
            update = packet_ctx->avi.avi_type;
            break;
        case HDMIRX_PACKET_SPD:
            update = packet_ctx->spd.spd_type;
            break;
        case HDMIRX_PACKET_VSI:
            update = packet_ctx->vsif.vsif_type;
            break;
        case HDMIRX_PACKET_UNREC:
            update = packet_ctx->unrec.unrec_type;
            break;
        default:
            update = HI_FALSE;
            break;
    }
    return update;
}

hdmirx_oversample hdmirxv2_packet_avi_get_replication(hi_drv_hdmirx_port port)
{
    hdmirx_oversample rate;
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    if (((packet_ctx->avi.avi_data[4 + IF_HEADER_LENGTH]) & 0x0f) >= /* 4: chech 9st avi data */
        HDMIRX_OVERSAMPLE_MAX) {
        rate = HDMIRX_OVERSAMPLE_NONE;
    } else {
        rate = ((packet_ctx->avi.avi_data[4 + IF_HEADER_LENGTH]) & 0x0f); /* 4: chech 9st avi data */
    }

    return rate;
}

hi_bool hdmirxv2_packet_avi_is_data_valid(hi_drv_hdmirx_port port)
{
    hi_bool valid = HI_FALSE;
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    if (packet_ctx->avi.avi_version >= AVI_VERSION) {
        valid = HI_TRUE;
    }

    return valid;
}

hdmirx_color_space hdmirxv2_packet_avi_get_color_space(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);

    if (hdmirxv2_packet_avi_is_data_valid(port) == HI_FALSE) {
        return HDMIRX_COLOR_SPACE_RGB; /* HDMIRX_COLOR_SPACE_BUTT; */
    }
    return (hdmirx_color_space)((packet_ctx->avi.avi_data[IF_HEADER_LENGTH] >> 5) & 0x03); /* 5: get high 3 bits */
}

hdmirx_rgb_range hdmirxv2_packet_avi_get_rgb_range(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);

    return (hdmirx_rgb_range)
        ((packet_ctx->avi.avi_data[IF_HEADER_LENGTH + IF_LENGTH_INDEX] >> 2) & 0x03); /* 2: get high 6 bits */
}

hdmirx_range hdmirxv2_packet_avi_get_yuv_range(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    hi_u8 tmp;

    packet_ctx = hdmirxv2_packet_get_ctx(port);

    tmp = ((packet_ctx->avi.avi_data[IF_HEADER_LENGTH + 4] >> 6) & 0x03); /* 4, 6: 9st avi data get high 2 bits */

    if (tmp == 1) {
        return HDMIRX_FULL_RANGE;
    } else {
        return HDMIRX_LIMIT_RANGE;
    }
}

hi_bool hdmirxv2_packet_avi_get_itc(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);

    return (hi_bool)((packet_ctx->avi.avi_data[IF_HEADER_LENGTH + IF_LENGTH_INDEX] >> 7) & 0x01); /* 7: get high 1bit */
}

hdmirx_color_metry hdmirxv2_packet_avi_get_color_metry(hi_drv_hdmirx_port port)
{
    hdmirx_color_metry metry;
    hdmirx_color_space in_color_space;
    hi_u32 extend_metry;
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    in_color_space = ((packet_ctx->avi.avi_data[IF_HEADER_LENGTH]) >> 5) & 0x03; /* 5: get high 3 bits */
    metry = packet_ctx->avi.avi_data[1 + IF_HEADER_LENGTH];
    metry = metry >> 6; /* 6: get high 2 bits */
    metry &= 0x03;

    if (metry == HDMIRX_COLOR_METRY_EXTENDED) {
        extend_metry = packet_ctx->avi.avi_data[2 + IF_HEADER_LENGTH]; /* 2: get 7st avi data */
        extend_metry = (extend_metry >> 4); /* 6: get high 4 bits */
        extend_metry &= 0x07;
        if (extend_metry < 2) { /* 2: check value */
            metry = HDMIRX_COLOR_METRY_XV601 + extend_metry;
        } else if (extend_metry == 5) { /* 5: BT2020 ycbcr type */
            metry = HDMIRX_COLOR_METRY_BT2020_YCCBCCRC;
        } else if (extend_metry == 6) { /* 6: BT2020 rgb type */
            if (in_color_space == HDMIRX_COLOR_SPACE_RGB) {
                metry = HDMIRX_COLOR_METRY_BT2020_RGB;
            } else {
                metry = HDMIRX_COLOR_METRY_BT2020_YCBCR;
            }
        } else {
            metry = HDMIRX_COLOR_METRY_NOINFO;
        }
    }
    return metry;
}

static hi_void hdmirx_packet_avi_reset_data(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;
    errno_t err_ret;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    err_ret = memset_s(packet_ctx->avi.avi_data, sizeof(packet_ctx->avi.avi_data),
        0, sizeof(packet_ctx->avi.avi_data));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return;
    }
    packet_ctx->avi.avi_version = 0;
    hdmirxv2_packet_set_int_new_update(port, HDMIRX_PACKET_AVI, HI_FALSE);
}

static hi_void hdmirx_packet_avi_store_data(hi_drv_hdmirx_port port, hi_u8 *data, hi_u32 len)
{
    hdmirx_packet_context *packet_ctx;
    errno_t err_ret;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    err_ret = memcpy_s(packet_ctx->avi.avi_data, sizeof(packet_ctx->avi.avi_data),
        data, (AVI_LENGTH + IF_HEADER_LENGTH)); /* 2: get low 6 bits */
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return;
    }

    packet_ctx->avi.avi_version = data[1];
    if (packet_ctx->avi.avi_version < 2) { /* 2: check avi version */
        packet_ctx->avi.avi_data[3 + IF_HEADER_LENGTH] = 0; /* 3: get 8st avi data, set VIC to 0 */
        packet_ctx->avi.avi_data[4 + IF_HEADER_LENGTH] = 0; /* 4: get 9st avi data, repetition */
    }
}

static hi_void hdmirx_packet_avi_on_receiving(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    packet_ctx->avi.avi_received = HI_TRUE;
    hdmirxv2_video_set861_vic(port, packet_ctx->avi.avi_data[3 + IF_HEADER_LENGTH] & 0x7f); /* 3: get 8st avi data */
    if (packet_ctx->vsif.hdmi3d_vsif_received) {
        hdmirxv2_video_verify1_p4_format(port, HI_TRUE);
    }
    hdmirxv2_packet_set_int_new_update(port, HDMIRX_PACKET_AVI, HI_TRUE);
}

hi_void hdmirxv2_packet_avi_set_no_avi_int_en(hi_drv_hdmirx_port port, hi_bool en)
{
    if (en) {
        hal_ctrl_clear_interrupt(port, INTR_CEA_NO_AVI); /* clear no AVI interrupt if it was raised */
    }
    hdmirxv2_packet_set_int_new_update(port, HDMIRX_PACKET_AVI, en);
}

hi_void hdmirxv2_packet_avi_no_avi_handler(hi_drv_hdmirx_port port)
{
    hdmirx_packet_avi_reset_data(port);

    if (hdmirxv2_ctrl_is_video_on_state(port)) {
        hdmirxv2_ctrl_mode_change(port);
    }

    hdmirx_packet_av_mute_req(port, HI_TRUE, AV_MUTE_NO_AVI);
}

static hi_void hdmirx_packet_aif_on_receiving(hi_drv_hdmirx_port port, hi_u8 *data, hi_u32 length)
{
    hdmirx_packet_context *packet_ctx;
    hi_bool aif_received;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    aif_received = packet_ctx->audio_info_frame_received;
    if (data && (length >= 5)) { /* 5: check length value */
        packet_ctx->audio_info_frame_received = HI_TRUE;
        hdmirxv2_audio_set_ca(port, data[3]); /* 3: aif packet from head 3st data */
        hdmirxv2_audio_set_coding_type(port, (data[0] >> 4) & 0xf); /* 4: get high 4 bits */
    } else {
        packet_ctx->audio_info_frame_received = HI_FALSE;
    }

    if ((packet_ctx->audio_info_frame_received == HI_TRUE) && (aif_received == HI_FALSE)) {
        hdmirxv2_audio_update(port);
    }
}

hi_bool hdmirxv2_packet_aif_is_got(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);

    return packet_ctx->audio_info_frame_received;
}
hi_void hdmirxv2_packet_reset_aud_info_frame_data(hi_drv_hdmirx_port port)
{
    hdmirx_packet_aif_on_receiving(port, NULL, 0);
    hal_ctrl_clear_interrupt(port, HDMIRX_INT_UPDATE_AUD);
}

hi_void hdmirxv2_packet_hdr10_get_etof(hi_drv_hdmirx_port port, hi_u8 *etof)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    if (packet_ctx->unrec.unrec_received) {
        *etof = (packet_ctx->unrec.unrec_buffer[IF_HEADER_LENGTH] & 0x7);
    } else {
        *etof = 0;
    }
}

hi_void hdmirxv2_packet_hdr10_get_ec_value(hi_drv_hdmirx_port port, hi_u8 *color_metry, hi_u8 *extend_color_metry,
    hi_u32 color_metry_len, hi_u32 extend_color_metry_len)
{
    hi_u32 temp;
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);

    temp = packet_ctx->avi.avi_data[1 + IF_HEADER_LENGTH];

    temp = temp >> 6; /* 6: get high 2 bits */
    color_metry[0] = temp & 0x01;
    color_metry[1] = (temp & 0x02) >> 1;

    temp = packet_ctx->avi.avi_data[2 + IF_HEADER_LENGTH]; /* 2: offset */
    temp = (temp >> 4); /* 4: get high 4 bits */
    extend_color_metry[0] = temp & 0x01;
    extend_color_metry[1] = (temp & 0x02) >> 1;
    extend_color_metry[2] = (temp & 0x04) >> 2; /* 2: get high 6 bits set to extend_color_metry 3st data */
}

hi_void hdmirxv2_packet_unrec_reset_data(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;
    errno_t err_ret;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    err_ret = memset_s(packet_ctx->unrec.unrec_buffer, sizeof(packet_ctx->unrec.unrec_buffer),
        0, sizeof(packet_ctx->unrec.unrec_buffer));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return;
    }
    packet_ctx->unrec.unrec_received = HI_FALSE;
    hdmirxv2_packet_set_int_new_update(port, HDMIRX_PACKET_UNREC, HI_FALSE);
}

static hi_void hdmirx_packet_hdr10_on_receiving(hi_drv_hdmirx_port port, hi_u8 *packet, hi_u32 len)
{
    hi_u32 length;
    hi_bool chk_sum;
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    length = packet[IF_LENGTH_INDEX];

    packet_ctx->unrec.unrec_received = HI_TRUE;

    chk_sum = hdmirx_packet_is_check_sum_ok(packet, length + IF_HEADER_LENGTH, len);
    if (chk_sum == HI_FALSE) {
        return;
    }
    /* the packet looks valid. */
    if (osal_memncmp(packet, sizeof(packet_ctx->unrec.unrec_buffer), packet_ctx->unrec.unrec_buffer,
        sizeof(packet_ctx->unrec.unrec_buffer)) != 0) {
        /* the received packet differs from the previous one. copy received packet into the shadow buffer. */
        /* it will be used to compare with the newly coming ones. */
        errno_t err_ret = memcpy_s(packet_ctx->unrec.unrec_buffer, sizeof(packet_ctx->unrec.unrec_buffer),
            packet, sizeof(packet_ctx->unrec.unrec_buffer));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            return;
        }
    }
    hdmirxv2_packet_set_int_new_update(port, HDMIRX_PACKET_UNREC, HI_TRUE);
}

hi_bool hdmirxv2_packet_hdr10_is_got(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    return packet_ctx->unrec.unrec_received;
}

hi_hdmirx_source_input_type hdmirx_packet_hdr10_get_type(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    if (packet_ctx->unrec.unrec_received) {
        hi_u32 tmp;
        tmp = (packet_ctx->unrec.unrec_buffer[IF_HEADER_LENGTH] & 0x7);
        if (tmp == 0) {
            return HI_HDMIRX_SOURCE_INPUT_TYPE_SDR;
        } else if ((tmp == 1) || (tmp == 2)) { /* 2: check HDR10 type */
            return HI_HDMIRX_SOURCE_INPUT_TYPE_HDR10;
        } else if (tmp == 3) { /* 3: check HLG type */
            return HI_HDMIRX_SOURCE_INPUT_TYPE_HLG;
        } else {
            return HI_HDMIRX_SOURCE_INPUT_TYPE_UNKNOWN;
        }
    } else {
        return HI_HDMIRX_SOURCE_INPUT_TYPE_UNKNOWN;
    }
}

hi_hdmirx_source_input_type hdmirx_packet_stream_type(hi_drv_hdmirx_port port)
{
    hi_hdmirx_source_input_type hdr_type;
    hi_hdmirx_source_input_type dolby_type;
    hi_hdmirx_source_input_type hdr10_plus_type;

    hdr_type = hdmirx_packet_hdr10_get_type(port);
    dolby_type = hdmirxv2_packet_vsif_get_dolby_type(port);
    hdr10_plus_type = hdmirxv2_packet_vsif_get_hdr10_plus_type(port);

    if ((hdr_type == HI_HDMIRX_SOURCE_INPUT_TYPE_HDR10) || (hdr_type == HI_HDMIRX_SOURCE_INPUT_TYPE_HLG)) {
        if (hdr10_plus_type == HI_HDMIRX_SOURCE_INPUT_TYPE_HDR10PLUS) {
            return hdr10_plus_type;
        } else {
            return hdr_type;
        }
    } else if (dolby_type == HI_HDMIRX_SOURCE_INPUT_TYPE_DOLBY) {
        return dolby_type;
    } else if ((hdr_type == HI_HDMIRX_SOURCE_INPUT_TYPE_SDR) || (dolby_type == HI_HDMIRX_SOURCE_INPUT_TYPE_SDR)) {
        return HI_HDMIRX_SOURCE_INPUT_TYPE_SDR;
    } else {
        return HI_HDMIRX_SOURCE_INPUT_TYPE_UNKNOWN;
    }
}

hi_s32 hdmirx_packet_hdr10_get_meta_data(hi_drv_hdmirx_port port, hdmirx_hdr10_metadata_stream *data)
{
    hdmirx_packet_context *packet_ctx = hdmirxv2_packet_get_ctx(port);
    hi_u8 *buffer = HI_NULL;
    errno_t err_ret;

    if (packet_ctx->unrec.unrec_received == HI_FALSE) {
        err_ret = memset_s(data, sizeof(hdmirx_hdr10_metadata_stream), 0, sizeof(hdmirx_hdr10_metadata_stream));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            return err_ret;
        }
        return HI_SUCCESS;
    }
    if ((hdmirx_packet_hdr10_get_type(port) != HI_HDMIRX_SOURCE_INPUT_TYPE_HDR10) &&
        (hdmirx_packet_hdr10_get_type(port) != HI_HDMIRX_SOURCE_INPUT_TYPE_HLG)) {
        err_ret = memset_s(data, sizeof(hdmirx_hdr10_metadata_stream), 0, sizeof(hdmirx_hdr10_metadata_stream));
        if (err_ret != EOK) {
            hi_err_hdmirx("secure func call error\n");
            return err_ret;
        }
        return HI_SUCCESS;
    }
    buffer = packet_ctx->unrec.unrec_buffer + IF_HEADER_LENGTH + 2; /* 2: from head offset 2 bytes */
    data->disp_primaries_x0 = buffer[0] + /* buffer 0 set to low 8 bits */
        (buffer[1] << 8); /* buffer 1 set to high 8 bits */
    data->disp_primaries_y0 = buffer[2] + /* buffer 2 set to low 8 bits */
        (buffer[3] << 8); /* buffer 3 set to high 8 bits */
    data->disp_primaries_x1 = buffer[4] + /* buffer 4 set to low 8 bits */
        (buffer[5] << 8); /* buffer 5 set to high 8 bits */
    data->disp_primaries_y1 = buffer[6] + /* buffer 6 set to low 8 bits */
        (buffer[7] << 8); /* buffer 7 set to high 8 bits */
    data->disp_primaries_x2 = buffer[8] + /* buffer 8 set to low 8 bits */
        (buffer[9] << 8); /* buffer 9 set to high 8 bits */
    data->disp_primaries_y2 = buffer[10] + (buffer[11] << 8); /* buf 10 set to low 8 bits, buf 11 set to high 8 bits */
    data->white_point_x = buffer[12] + (buffer[13] << 8); /* buf 12 set to low 8 bits, buf 13 set to high 8 bits */
    data->white_point_y = buffer[14] + (buffer[15] << 8); /* buf 14 set to low 8 bits, buf 15 set to high 8 bits */
    data->max_disp_mastering_luminance =
        (buffer[16] + (buffer[17] << 8)) * 10000; /* buf 16 set to low 8 bits, buf 17 times 10000, set to high 8 bits */

    data->min_disp_mastering_luminance =
        (buffer[18] + (buffer[19] << 8)); /* buf 18 set to low 8 bits, buf 19 set to high 8 bits */

    data->max_content_light_level =
        buffer[20] + (buffer[21] << 8); /* buf 20 set to low 8 bits, buf 21 set to high 8 bits */

    data->max_frm_average_light_level =
        buffer[22] + (buffer[23] << 8); /* buf 22 set to low 8 bits, buf 23 set to high 8 bits */

    return HI_SUCCESS;
}

static hi_void hdmirx_packet_rst_info_frame_data(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    packet_ctx->avi.avi_received = HI_FALSE;
    packet_ctx->vsif.hdmi3d_vsif_received = HI_FALSE;
    packet_ctx->vsif.hdmi_dolby15_received = HI_FALSE;
    packet_ctx->vsif.hdmi_hdr10_plus_received = HI_FALSE;
}

hi_void hdmirxv2_packet_vsif_reset_data(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;
    errno_t err_ret;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    packet_ctx->vsif.hdmi3d_vsif_received = HI_FALSE;
    packet_ctx->vsif.hdmi_dolby15_received = HI_FALSE;
    packet_ctx->vsif.hdmi_hdr10_plus_received = HI_FALSE;
    err_ret = memset_s(packet_ctx->vsif.vsif_data, sizeof(packet_ctx->vsif.vsif_data),
        0, (IF_MAX_VSIF_LENGTH + IF_HEADER_LENGTH) * sizeof(hi_u8));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return;
    }
    hdmirxv2_packet_set_int_new_update(port, HDMIRX_PACKET_VSI, HI_FALSE);
}

hi_void hdmirxv2_packet_reset_data(hi_drv_hdmirx_port port)
{
    hdmirx_packet_context *packet_ctx;
    errno_t err_ret;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    err_ret = memset_s(packet_ctx, sizeof(hdmirx_packet_context), 0, sizeof(hdmirx_packet_context));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return;
    }
    packet_ctx->avi.avi_data[IF_HEADER_LENGTH] = (HDMIRX_COLOR_SPACE_MAX << 5); /* 5: times 32 */
    packet_ctx->avi.avi_data[4 + IF_HEADER_LENGTH] = HDMIRX_OVERSAMPLE_MAX; /* 4: 9st avi data */
    packet_ctx->acp_type = ACP_GENERAL_AUDIO; /* default acp is general */

    /* set the spdif out on the acp */
    hdmirxv2_audio_update_on_acp(port, packet_ctx->acp_type);
    /* turn on the acp int. */
    hdmirx_packet_set_acp_int_en(port, HI_TRUE);

    /* enable the int of v res change */
    /* enable the aec of v res change. */
    /* disable the aec of v res change. */
    hdmirxv2_video_set_res_chg_events_en(port, HI_TRUE);

    /* sw initial */
    hdmirx_packet_av_mute_req(port, HI_FALSE, AV_MUTE_INP_AV_MUTE_CAME | AV_MUTE_NO_AVI);
    hdmirx_packet_rst_info_frame_data(port);
    hdmirx_packet_avi_reset_data(port);
    hdmirxv2_packet_vsif_reset_data(port);
    hdmirxv2_packet_spd_reset_data(port);
    hdmirxv2_packet_unrec_reset_data(port);
}

hi_void hdmirxv2_packet_main_thread(hi_drv_hdmirx_port port)
{
    osal_timeval s_cur_time;
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);

    if (port >= HI_DRV_HDMIRX_PORT_MAX) {
        return;
    }

    if (hdmirxv2_ctrl_is_need2be_done(port) == HI_FALSE) {
        return;
    }
    osal_get_timeofday(&s_cur_time);
    if ((hdmirx_time_diff_ms(s_cur_time, g_acp_time) > 700) /* more than 700 ms no receive acp packet */
        && (packet_ctx->acp_type != ACP_GENERAL_AUDIO)) {
        packet_ctx->acp_type = ACP_GENERAL_AUDIO;
        hdmirxv2_audio_update_on_acp(port, ACP_GENERAL_AUDIO);
    }
    if ((hdmirx_time_diff_ms(s_cur_time, g_acp_time) > 150)) { /* more than 150 ms no receive acp packet */
        hdmirx_packet_set_acp_int_en(port, HI_TRUE);
    }
}

hdmirx_color_metry hdmirx_packet_update_color_metry(hi_drv_hdmirx_port port, hdmirx_color_space in_color_space,
    hi_u8* data, hi_u32 len)
{
    hdmirx_color_metry in_color_metry;
    hi_u32 extend_metry;

    extend_metry = data[2 + IF_HEADER_LENGTH]; /* 2: 7st avi data value indicate extend_metry */
    extend_metry = (extend_metry >> 4); /* 4: remove low 4 bits */
    extend_metry &= 0x07;
    if (extend_metry < 2) { /* 2: HDMIRX_COLOR_METRY_XV601 or HDMIRX_COLOR_METRY_XV709 */
        in_color_metry = HDMIRX_COLOR_METRY_XV601 + extend_metry;
    } else if (extend_metry == 5) { /* 5: HDMIRX_COLOR_METRY_BT2020_YCCBCCRC */
        in_color_metry = HDMIRX_COLOR_METRY_BT2020_YCCBCCRC;
    } else if (extend_metry == 6) { /* 6: HDMIRX_COLOR_METRY_BT2020_RGB or HDMIRX_COLOR_METRY_BT2020_YCBCR */
        if (in_color_space == HDMIRX_COLOR_SPACE_RGB) {
            in_color_metry = HDMIRX_COLOR_METRY_BT2020_RGB;
        } else {
            in_color_metry = HDMIRX_COLOR_METRY_BT2020_YCBCR;
        }
    } else {
        in_color_metry = HDMIRX_COLOR_METRY_NOINFO;
    }
    return in_color_metry;
}

hi_void hdmirx_packet_check_range_change(hi_drv_hdmirx_port port, hdmirx_color_space in_color_space,
    hdmirx_rgb_range rgb_range, hdmirx_range yuv_range, hi_bool *ran_chg)
{
    if (in_color_space == HDMIRX_COLOR_SPACE_RGB) {
        if (rgb_range != hdmirxv2_packet_avi_get_rgb_range(port)) {
            *ran_chg = HI_TRUE;
        }
    } else if (yuv_range != hdmirxv2_packet_avi_get_yuv_range(port)) {
        *ran_chg = HI_TRUE;
    }
}

hi_bool hdmirx_packet_check_change(hi_drv_hdmirx_port port, hdmirx_oversample replication,
    hdmirx_color_space in_color_space)
{
    if ((replication != hdmirxv2_packet_avi_get_replication(port)) ||
        (in_color_space != hdmirxv2_packet_avi_get_color_space(port))) {
        return HI_TRUE;
    }
    return HI_FALSE;
}

hi_void hdmirx_packet_change(hi_drv_hdmirx_port port, hdmirx_oversample replication, hdmirx_color_space in_color_space)
{
    hi_bool packet_change;

    packet_change = hdmirx_packet_check_change(port, replication, in_color_space);
    if (packet_change == HI_TRUE) {
        hi_warn_hdmirx("packet change## replication: %d to %d, colorspace: %d to %d\n",
            hdmirxv2_packet_avi_get_replication(port), replication,
            hdmirxv2_packet_avi_get_color_space(port), in_color_space);
        hdmirxv2_ctrl_change_state(port, HDMIRX_STATE_WAIT);
        hal_ctrl_set_mute_en(port, HDMIRX_MUTE_VDO, HI_TRUE);
        hdmirxv2_ctrl_mode_change(port);
    }
}

hi_void hdmirx_packet_proc_avi(hi_drv_hdmirx_port port)
{
    hi_u8 data[PACKET_BUFFER_LENGTH];
    hi_u32 length;
    hi_u32 ret_len;
    hdmirx_oversample replication;
    hdmirx_color_space in_color_space;
    hdmirx_color_metry in_color_metry;
    hdmirx_rgb_range rgb_range;
    hdmirx_range yuv_range;
    hi_bool ran_chg = HI_FALSE;
    hi_bool check_sum = HI_FALSE;

    /*
     * clear AVI interrupt if it was raised after clearing in the
     * beginning rx_isr_interrupt_handler() and this moment
     */
    hal_ctrl_clear_interrupt(port, HDMIRX_INT_NEW_AVI);

    ret_len = hal_ctrl_get_packet_content(port, HDMIRX_PACKET_AVI, data, PACKET_BUFFER_LENGTH);
    length = data[IF_LENGTH_INDEX];
    if ((length >= IF_MIN_AVI_LENGTH) && (length <= IF_MAX_AVI_LENGTH)) {
        check_sum = hdmirx_packet_is_check_sum_ok(data, length + IF_HEADER_LENGTH, sizeof(data) / sizeof(data[0]));
        if (check_sum == HI_FALSE) {
            hi_err_hdmirx("avi check sum error\n");
            return;
        }
        replication = (data[4 + IF_HEADER_LENGTH] & 0x0f); /* 4: 9st avi data indicate replication */
        in_color_space = ((data[IF_HEADER_LENGTH] >> 5) & 0x03); /* 5: get high 3 bits */
        in_color_metry = ((data[1 + IF_HEADER_LENGTH] >> 6) & 0x03); /* 6: get high 2 bits */
        rgb_range = ((data[IF_HEADER_LENGTH + IF_LENGTH_INDEX] >> 2) & 0x03); /* 2: 7st avi data indicate rgb_range */
        yuv_range = ((data[IF_HEADER_LENGTH + 4] >> 6) & 0x03); /* 4, 6: 9st avi data get high 2 bits */
        if (in_color_metry == HDMIRX_COLOR_METRY_EXTENDED) {
            in_color_metry = hdmirx_packet_update_color_metry(port, in_color_space, data, PACKET_BUFFER_LENGTH);
        }
        hdmirx_packet_check_range_change(port, in_color_space, rgb_range, yuv_range, &ran_chg);
        hdmirx_packet_change(port, replication, in_color_space);
        if ((in_color_metry != hdmirxv2_packet_avi_get_color_metry(port)) || ran_chg) {
            hi_warn_hdmirx("packet change## colormetry: %d to %d\n",
                hdmirxv2_packet_avi_get_color_metry(port), in_color_metry);
            hi_warn_hdmirx("packet rgb_range change from %d to %d\n",
                hdmirxv2_packet_avi_get_rgb_range(port), rgb_range);
            hi_warn_hdmirx("packet yuv_range change from %d to %d\n",
                hdmirxv2_packet_avi_get_yuv_range(port), yuv_range);
            hdmirx_packet_avi_store_data(port, data, PACKET_BUFFER_LENGTH);
                hdmirxv2_video_set_stream_data(port);
        }
        hdmirx_packet_avi_store_data(port, data, PACKET_BUFFER_LENGTH);
        hdmirx_packet_avi_on_receiving(port);
        hdmirx_packet_av_mute_req(port, HI_FALSE, AV_MUTE_NO_AVI);
    }
    return;
}

hi_void hdmirx_packet_proc_aif(hi_drv_hdmirx_port port)
{
    hi_u8 data[PACKET_BUFFER_LENGTH];
    hi_u32 length;
    hi_u32 ret_len;
    hi_bool check_sum = HI_FALSE;

    ret_len = hal_ctrl_get_packet_content(port, HDMIRX_PACKET_AIF, data, PACKET_BUFFER_LENGTH);
    length = data[IF_LENGTH_INDEX];
    if ((length >= IF_MIN_AUDIO_LENGTH) && (length <= IF_MAX_AUDIO_LENGTH)) {
        check_sum = hdmirx_packet_is_check_sum_ok(data, length + IF_HEADER_LENGTH,
            sizeof(data) / sizeof(data[0]));
        if (check_sum == HI_FALSE) {
            return;
        }
        hdmirx_packet_aif_on_receiving(port, &data[IF_HEADER_LENGTH], length);
    }
}

hi_void hdmirx_packet_proc_spd(hi_drv_hdmirx_port port)
{
    hi_u8 data[PACKET_BUFFER_LENGTH];
    hi_u32 ret_len;

    ret_len = hal_ctrl_get_packet_content(port, HDMIRX_PACKET_SPD, data, PACKET_BUFFER_LENGTH);
    hdmirx_packet_spd_on_receiving(port, data, PACKET_BUFFER_LENGTH);
}

hi_void hdmirx_packet_proc_vsif(hi_drv_hdmirx_port port)
{
    hi_u8 data[PACKET_BUFFER_LENGTH];
    hi_u32 ret_len;

    ret_len = hal_ctrl_get_packet_content(port, HDMIRX_PACKET_VSI, data, PACKET_BUFFER_LENGTH);
    hdmirx_packet_vsif_on_receiving(port, data, PACKET_BUFFER_LENGTH);
}

hi_void hdmirx_packet_proc_acp(hi_drv_hdmirx_port port)
{
    hi_u8 data[PACKET_BUFFER_LENGTH];
    hdmirx_packet_context *packet_ctx;
    hi_u32 ret_len;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    ret_len = hal_ctrl_get_packet_content(port, HDMIRX_PACKET_ACP, data, PACKET_BUFFER_LENGTH);
    packet_ctx->acp_type = (hdmirx_acp_type)data[1];
    osal_get_timeofday(&g_acp_time);

    /*
     * do not check ACP interrupt several video frames.
     * otherwise if it comes every frame, TX will fail to change packet data so often.
     */
    hdmirx_packet_set_acp_int_en(port, HI_FALSE);

    hdmirxv2_audio_update_on_acp(port, packet_ctx->acp_type);
}

hi_void hdmirx_packet_proc_hdr(hi_drv_hdmirx_port port)
{
    hi_u8 data[PACKET_BUFFER_LENGTH];
    hi_u8 data_avi[PACKET_BUFFER_LENGTH];
    hi_u32 length;
    hi_u32 ret_len;
    hi_u32 if_max_hdr_length = 26; /* 26: if length check threshold */

    osal_msleep(50); /* 50: delay time */
    ret_len = hal_ctrl_get_packet_content(port, HDMIRX_PACKET_HDR, data, PACKET_BUFFER_LENGTH);
    ret_len = hal_ctrl_get_packet_content(port, HDMIRX_PACKET_AVI, data_avi, PACKET_BUFFER_LENGTH);
    length = data[IF_LENGTH_INDEX];
    if (length <= if_max_hdr_length) {
        hdmirx_packet_hdr10_on_receiving(port, data, PACKET_BUFFER_LENGTH);
        hdmirx_packet_avi_store_data(port, data_avi, PACKET_BUFFER_LENGTH);
        hdmirxv2_video_set_stream_data(port);
    }
}


hi_void hdmirxv2_packet_interrupt_handler(hi_drv_hdmirx_port port, hi_u32 inter)
{
    if (inter & INTR_CEA_UPDATE_AVI) {
        hdmirx_packet_proc_avi(port);
    }
    if (inter & INTR_CEA_UPDATE_AIF) {
        hdmirx_packet_proc_aif(port);
    }
    if (inter & INTR_CEA_UPDATE_SPD) {
        hdmirx_packet_proc_spd(port);
    }
    if (inter & INTR_CEA_UPDATE_MPEG) {
    }
    if (inter & INTR_CEA_UPDATE_HDR) {
        hdmirx_packet_proc_hdr(port);
    }
}

hi_void hdmirxv2_packet_avi_check(hi_drv_hdmirx_port port)
{
    hdmirx_color_space in_color_space;
    hdmirx_packet_context *packet_ctx;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    if (packet_ctx->avi.avi_received == HI_TRUE) {
        in_color_space = ((packet_ctx->avi.avi_data[IF_HEADER_LENGTH] >> 5) & 0x03); /* 5: get high 3 bits */
        if (hal_ctrl_check_avi_color_chg(port, in_color_space) == HI_TRUE) {
            hdmirx_packet_proc_vsif(port);
        }
    }
}

hi_void hdmirxv2_packet_vsif_int_handler(hi_drv_hdmirx_port port)
{
    hdmirx_packet_proc_vsif(port);
}

hi_void hdmirxv2_packet_acp_int_handler(hi_drv_hdmirx_port port)
{
    hdmirx_packet_proc_acp(port);
}
hi_void hdmirxv2_packet_initial(hi_drv_hdmirx_port port)
{
    hdmirxv2_packet_reset_data(port);
    hdmirx_packet_av_mute_req(port, HI_FALSE, AV_MUTE_INP_AV_MUTE_CAME | AV_MUTE_NO_AVI);
}

hi_void hdmirxv2_packet_proc_read(hi_drv_hdmirx_port port, hi_void *s)
{
    hdmirx_packet_context *packet_ctx;
    hi_u8 packet_data[PACKET_BUFFER_LENGTH];
    hi_u8 i;
    hi_u32 ret_len = 0;

    packet_ctx = hdmirxv2_packet_get_ctx(port);
    osal_proc_print(s, "AVI type:%d\n", packet_ctx->avi.avi_type);
    if ((hdmirxv2_packet_avi_is_got(port) == HI_TRUE) && (hal_video_get_hdmi_mode(port) == HI_TRUE)) {
        osal_proc_print(s, "AVI Data:");
        for (i = 0; i < AVI_LENGTH + IF_HEADER_LENGTH; i++) {
            osal_proc_print(s, "%02x  ", packet_ctx->avi.avi_data[i]);
        }
        osal_proc_print(s, "\n");
    }
    if ((packet_ctx->vsif.vsif_received == HI_TRUE) &&
        (hal_video_get_hdmi_mode(port) == HI_TRUE)) {
        osal_proc_print(s, "VSIF Data:");
        ret_len = hal_ctrl_get_packet_content(port, HDMIRX_PACKET_VSI, packet_data, PACKET_BUFFER_LENGTH);
        for (i = 0; i < IF_BUFFER_LENGTH; i++) {
            osal_proc_print(s, "%02x  ", packet_data[i]);
        }
        osal_proc_print(s, "\n");
    }
    if ((hdmirxv2_packet_hdr10_is_got(port) == HI_TRUE) &&
        (hal_video_get_hdmi_mode(port) == HI_TRUE)) {
        osal_proc_print(s, "HDR Data:");
        for (i = 0; i < 31; i++) { /* 31: print front 31 bytes */
            osal_proc_print(s, "%02x  ", packet_ctx->unrec.unrec_buffer[i]);
        }
        osal_proc_print(s, "\n");
    }
}
