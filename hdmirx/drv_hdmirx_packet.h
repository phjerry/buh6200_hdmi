/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Definition of packet functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#ifndef __DRV_HDMIRX_PACKET_H__
#define __DRV_HDMIRX_PACKET_H__

#include "drv_hdmirx_common.h"
#include "hal_hdmirx_comm.h"

#define AV_MUTE_INP_AV_MUTE_CAME BIT1
/* mute due to RX chip is not ready */
#define AV_MUTE_RX_IS_NOT_READY BIT2
/* mute because of no AVI packet coming and therefore input color space is unknown */
#define AV_MUTE_NO_AVI BIT4
/* mute due to an HDCP error */
#define AV_MUTE_RX_HDCP_ERROR BIT7

#define PACKET_BUFFER_LENGTH 35
#define PACKET_TITLE_INDEX   0
#define IF_HEADER_LENGTH     4
#define IF_LENGTH_INDEX      2

#define IF_MIN_AUDIO_LENGTH 10 /* the 861C standard defines the length as 10 */
#define IF_MAX_AUDIO_LENGTH 10 /* maximum allowed by the chip */

#define IF_MIN_AVI_LENGTH 13 /* the 861C standard defines the length as 13 */
#define IF_MAX_AVI_LENGTH 15 /* maximum allowed by the chip */

#define IF_MIN_MPEG_LENGTH 10 /* the 861C standard defines the length as 10 */
#define IF_MAX_MPEG_LENGTH 27 /* maximum allowed by the chip */

#define IF_MIN_SPD_LENGTH 25 /* the 861C standard defines the length as 25 */
#define IF_MAX_SPD_LENGTH 27 /* maximum allowed by the chip */

#define IF_MIN_VSIF_LENGTH 4  /* minimum length by HDMI 1.4 */
#define IF_MAX_VSIF_LENGTH 27 /* maximum allowed by the chip */

#define SPD_BUFFER_LENGTH 25
#define IF_BUFFER_LENGTH  31
#define AVI_LENGTH        13
#define AVI_VERSION       2

#define VSIF_DEC 1
#define SPD_DEC  0

typedef enum {
    HDMIRX_COLOR_METRY_NOINFO,
    HDMIRX_COLOR_METRY_ITU601,
    HDMIRX_COLOR_METRY_ITU709,
    HDMIRX_COLOR_METRY_EXTENDED, /* if extended, but unknown */
    HDMIRX_COLOR_METRY_XV601 = 10,
    HDMIRX_COLOR_METRY_XV709,
    HDMIRX_COLOR_METRY_SYCC601,
    HDMIRX_COLOR_METRY_ADOBE_YCC601,
    HDMIRX_COLOR_METRY_ADOBE_RGB,
    HDMIRX_COLOR_METRY_BT2020_YCCBCCRC,
    HDMIRX_COLOR_METRY_BT2020_RGB,
    HDMIRX_COLOR_METRY_BT2020_YCBCR,
    HDMIRX_COLOR_METRY_BUTT
} hdmirx_color_metry;

typedef enum {
    VSIF_NOT_HDMI_VSIF,           /* VSIF packet is not HDMI VSIF */
    VSIF_NEW_EXTENDED_RESOLUTION, /* VSIF packet carries extended resolution info: first detection */
    VSIF_OLD_EXTENDED_RESOLUTION, /* VSIF packet carries extended resolution info: no change */
    VSIF_NEW_3D,                  /* VSIF packet with 3D info: first detection */
    VSIF_OLD_3D,                  /* VSIF packet with 3D info: no change from last time */
    VSIF_DOLBY,                   /* VSIF packet with dolby */
    VSIF_HDR10PLUS                /* VSIF packet with hdr10+ */
} vsif_check_result;

typedef enum {
    VSIF_SPD_INIT,
    VSIF_SPD_ANY_VSIF,
    VSIF_SPD_ANY_SPD,
    VSIF_SPD_DELAY,
} hdmirx_vsif_spd_state;

typedef enum {
    ACP_GENERAL_AUDIO = 0,
    ACP_IEC60958,
    ACP_DVD_AUDIO,
    ACP_SUPER_AUDIO_CD,
    ACP_BUTT
} hdmirx_acp_type;

/* for avi infoframe structure --> */
typedef struct {
    hi_u8 avi_data[AVI_LENGTH + IF_HEADER_LENGTH];
    hi_u8 avi_version;
    hi_bool avi_received;
    hi_bool avi_type; /* false: new; true: update */
} hdmirx_avi;

typedef struct {
    hi_bool spd_received; /* set on any SPD reception (even with incorrect check sum) */
    hi_bool spd_type; /* false: new; true: update */
    hi_u8 spd_buffer[SPD_BUFFER_LENGTH];
} hdmirx_spd;

/* for vsif infoframe structure --> */
typedef struct {
    hdmirx_vsif_spd_state vsif_state;
    hi_u16 vsif_timer; /* time unit in ms */
    hi_bool vsif_received; /* set on any VSIF reception */
    hi_bool found_hdmi_vsif;
    hi_bool hdmi3d_vsif_received;
    hi_bool hdmi_dolby15_received;
    hi_u16 no_hdmi_vsif_timeout;
    hi_u8 vsif_data[IF_MAX_VSIF_LENGTH + IF_HEADER_LENGTH];
    hi_bool hdmi_hdr10_plus_received;
    hi_bool vsif_type; /* false: new; true: update */
} hdmirx_vsif;

typedef enum {
    ISRC_ST_INIT,
    ISRC_ST_WAIT_ANY_ISRC1,
    ISRC_ST_WAIT_NEW_ISRC1,
    ISRC_ST_WAIT_ANY_ISRC2,
} hdmirx_isrc_state;

typedef struct {
    hi_bool unrec_received; /* set on any SPD reception (even with incorrect check sum) */
    hi_bool unrec_type; /* false: new; true: update */
    hi_u8 unrec_buffer[31]; /* 31: array size */
} hdmirx_unrec;

typedef struct {
    hdmirx_avi avi;
    hdmirx_spd spd;
    hdmirx_vsif vsif;
    hdmirx_unrec unrec;
    hdmirx_acp_type acp_type;
    hi_u32 av_mask; /* RX AV muting sources */
    hi_bool audio_info_frame_received;
} hdmirx_packet_context;

#endif

#ifndef __DRV_HDMIRXV2_PACKET_H__
#define __DRV_HDMIRXV2_PACKET_H__
#include "hal_hdmirx_comm.h"

#define hdmirxv2_packet_get_ctx(port) (&(g_hdmirx_packet_ctx_v2[port]))
#define HDMIRXV2_PACKET_GET_TIME() (&s_st_packet_time_v2)

hi_void hdmirxv2_packet_avi_set_no_avi_int_en(hi_drv_hdmirx_port port, hi_bool en);
hi_void hdmirxv2_packet_avi_no_avi_handler(hi_drv_hdmirx_port port);
hi_void hdmirxv2_packet_interrupt_handler(hi_drv_hdmirx_port port, hi_u32 inter);
hi_void hdmirxv2_packet_avi_check(hi_drv_hdmirx_port port);
hi_void hdmirxv2_packet_acp_int_handler(hi_drv_hdmirx_port port);

hi_void hdmirxv2_packet_main_thread(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_packet_avi_is_got(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_packet_spd_is_got(hi_drv_hdmirx_port port);
hi_void hdmirxv2_packet_spd_get_vendor_name(hi_drv_hdmirx_port port, hi_u32 *data, hi_u32 len);
hi_void hdmirxv2_packet_spd_get_product_des(hi_drv_hdmirx_port port, hi_u32 *data, hi_u32 len);
hi_void hdmirxv2_packet_spd_reset_data(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_packet_vsif_is_got3d(hi_drv_hdmirx_port port);
hdmirx_oversample hdmirxv2_packet_avi_get_replication(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_packet_avi_is_data_valid(hi_drv_hdmirx_port port);
hdmirx_color_space hdmirxv2_packet_avi_get_color_space(hi_drv_hdmirx_port port);
hdmirx_rgb_range hdmirxv2_packet_avi_get_rgb_range(hi_drv_hdmirx_port port);
hdmirx_range hdmirxv2_packet_avi_get_yuv_range(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_packet_avi_get_itc(hi_drv_hdmirx_port port);
hi_void hdmirxv2_packet_vsif_reset_data(hi_drv_hdmirx_port port);
hi_void hdmirxv2_packet_reset_data(hi_drv_hdmirx_port port);
hdmirx_color_metry hdmirxv2_packet_avi_get_color_metry(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_packet_aif_is_got(hi_drv_hdmirx_port port);
hi_void hdmirxv2_packet_reset_aud_info_frame_data(hi_drv_hdmirx_port port);

hi_void hdmirxv2_packet_initial(hi_drv_hdmirx_port port);
hi_void hdmirxv2_packet_vsif_int_handler(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_packet_hdr10_is_got(hi_drv_hdmirx_port port);
hi_void hdmirxv2_packet_unrec_reset_data(hi_drv_hdmirx_port port);
hi_hdmirx_source_input_type hdmirx_packet_hdr10_get_type(hi_drv_hdmirx_port port);
hi_s32 hdmirx_packet_hdr10_get_meta_data(hi_drv_hdmirx_port port, hdmirx_hdr10_metadata_stream *data);
hi_void hdmirxv2_packet_hdr10_get_etof(hi_drv_hdmirx_port port, hi_u8 *etof);
hi_void hdmirxv2_packet_hdr10_get_ec_value(hi_drv_hdmirx_port port, hi_u8 *c, hi_u8 *ec, hi_u32 len_c, hi_u32 len_ec);
hi_void hdmirxv2_packet_proc_read(hi_drv_hdmirx_port port, hi_void *s);
hi_s32 hdmirxv2_packet_dolby_get_meta_data(hi_drv_hdmirx_port port, hi_hdmirx_dolby_vision_info *dolby_info);
hi_s32 hdmirxv2_packet_hdr10_plus_get_meta_data(hi_drv_hdmirx_port port, hi_hdmirx_hdr10_plus_info *hdr10_plus_info);
hi_hdmirx_source_input_type hdmirx_packet_stream_type(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_packet_vsif_is_got_dolby(hi_drv_hdmirx_port port);
hi_bool hdmirxv2_packet_vsif_is_got_hdr10_plus(hi_drv_hdmirx_port port);
hi_s32 hdmirxv2_packet_vsif_get_dolby_info(hi_drv_hdmirx_port port, hi_hdmirx_dolby_info *dolby_info);
hi_s32 hdmirxv2_packet_vsif_get_hdr10_plus_info(hi_drv_hdmirx_port port,
    hi_hdmirx_hdr10_plus_dynamic_metadata_info *hdr10_plus_info);
hi_void hdmirxv2_packet_set_int_new_update(hi_drv_hdmirx_port port, hdmirx_ctrl_packet_type type, hi_bool update);
hi_bool hdmirxv2_packet_get_int_new_update(hi_drv_hdmirx_port port, hdmirx_ctrl_packet_type type);
#endif
