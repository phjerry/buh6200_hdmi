/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver infoframe header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __DRV_HDMI_INFOFRAME_H__
#define __DRV_HDMI_INFOFRAME_H__

#include <hi_type.h>

enum hdmi_packet_type {
    HDMI_PACKET_TYPE_NULL = 0x00,
    HDMI_PACKET_TYPE_AUDIO_CLOCK_REGEN = 0x01,
    HDMI_PACKET_TYPE_AUDIO_SAMPLE = 0x02,
    HDMI_PACKET_TYPE_GENERAL_CONTROL = 0x03,
    HDMI_PACKET_TYPE_ACP = 0x04,
    HDMI_PACKET_TYPE_ISRC1 = 0x05,
    HDMI_PACKET_TYPE_ISRC2 = 0x06,
    HDMI_PACKET_TYPE_ONE_BIT_AUDIO_SAMPLE = 0x07,
    HDMI_PACKET_TYPE_DST_AUDIO = 0x08,
    HDMI_PACKET_TYPE_HBR_AUDIO_STREAM = 0x09,
    HDMI_PACKET_TYPE_GAMUT_METADATA = 0x0a,
    HDMI_PACKET_TYPE_EXTENDED_METADATA = 0x7f
    /* + enum hdmi_infoframe_type */
};

enum hdmi_infoframe_type {
    HDMI_INFOFRAME_TYPE_VENDOR = 0x81,
    HDMI_INFOFRAME_TYPE_AVI = 0x82,
    HDMI_INFOFRAME_TYPE_SPD = 0x83,
    HDMI_INFOFRAME_TYPE_AUDIO = 0x84,
    HDMI_INFOFRAME_TYPE_DRM = 0X87
};

#define HDMI_IEEE_OUI              0x000c03
#define HDMI_FORUM_IEEE_OUI        0xc45dd8
#define HDMI_DOLBY_IEEE_OUI        0x00d046
#define HDMI_INFOFRAME_HEADER_SIZE 4
#define HDMI_AVI_INFOFRAME_SIZE    13
#define HDMI_SPD_INFOFRAME_SIZE    25
#define HDMI_AUDIO_INFOFRAME_SIZE  10
#define HDMI_DRM_INFOFRAME_SIZE    26

#define HDMI_INFOFRAME_SIZE(type) \
    (HDMI_INFOFRAME_HEADER_SIZE + HDMI_##type##_INFOFRAME_SIZE)

struct hdmi_any_infoframe {
    enum hdmi_infoframe_type type;
    hi_u8 version;
    hi_u8 length;
};

enum hdmi_colorspace {
    HDMI_COLORSPACE_RGB,
    HDMI_COLORSPACE_YUV422,
    HDMI_COLORSPACE_YUV444,
    HDMI_COLORSPACE_YUV420,
    HDMI_COLORSPACE_RESERVED4,
    HDMI_COLORSPACE_RESERVED5,
    HDMI_COLORSPACE_RESERVED6,
    HDMI_COLORSPACE_IDO_DEFINED,
};

enum hdmi_scan_mode {
    HDMI_SCAN_MODE_NONE,
    HDMI_SCAN_MODE_OVERSCAN,
    HDMI_SCAN_MODE_UNDERSCAN,
    HDMI_SCAN_MODE_RESERVED,
};

enum hdmi_colorimetry {
    HDMI_COLORIMETRY_NONE,
    HDMI_COLORIMETRY_ITU_601,
    HDMI_COLORIMETRY_ITU_709,
    HDMI_COLORIMETRY_EXTENDED,
};

enum hdmi_picture_aspect {
    HDMI_PICTURE_ASPECT_NONE,
    HDMI_PICTURE_ASPECT_4_3,
    HDMI_PICTURE_ASPECT_16_9,
    HDMI_PICTURE_ASPECT_64_27,
    HDMI_PICTURE_ASPECT_256_135,
    HDMI_PICTURE_ASPECT_RESERVED,
};

enum hdmi_active_aspect {
    HDMI_ACTIVE_ASPECT_16_9_TOP = 2,
    HDMI_ACTIVE_ASPECT_14_9_TOP = 3,
    HDMI_ACTIVE_ASPECT_16_9_CENTER = 4,
    HDMI_ACTIVE_ASPECT_PICTURE = 8,
    HDMI_ACTIVE_ASPECT_4_3 = 9,
    HDMI_ACTIVE_ASPECT_16_9 = 10,
    HDMI_ACTIVE_ASPECT_14_9 = 11,
    HDMI_ACTIVE_ASPECT_4_3_SP_14_9 = 13,
    HDMI_ACTIVE_ASPECT_16_9_SP_14_9 = 14,
    HDMI_ACTIVE_ASPECT_16_9_SP_4_3 = 15,
};

enum hdmi_extended_colorimetry {
    HDMI_EXTENDED_COLORIMETRY_XV_YCC_601,
    HDMI_EXTENDED_COLORIMETRY_XV_YCC_709,
    HDMI_EXTENDED_COLORIMETRY_S_YCC_601,
    HDMI_EXTENDED_COLORIMETRY_OPYCC_601,
    HDMI_EXTENDED_COLORIMETRY_OPRGB,

    /* The following EC values are only defined in CEA-861-F. */
    HDMI_EXTENDED_COLORIMETRY_BT2020_CONST_LUM,
    HDMI_EXTENDED_COLORIMETRY_BT2020,
    HDMI_EXTENDED_COLORIMETRY_RESERVED,
};

enum hdmi_quantization_range {
    HDMI_QUANTIZATION_RANGE_DEFAULT,
    HDMI_QUANTIZATION_RANGE_LIMITED,
    HDMI_QUANTIZATION_RANGE_FULL,
    HDMI_QUANTIZATION_RANGE_RESERVED,
};

/* non-uniform picture scaling */
enum hdmi_nups {
    HDMI_NUPS_UNKNOWN,
    HDMI_NUPS_HORIZONTAL,
    HDMI_NUPS_VERTICAL,
    HDMI_NUPS_BOTH,
};

enum hdmi_ycc_quantization_range {
    HDMI_YCC_QUANTIZATION_RANGE_LIMITED,
    HDMI_YCC_QUANTIZATION_RANGE_FULL,
};

enum hdmi_content_type {
    HDMI_CONTENT_TYPE_GRAPHICS,
    HDMI_CONTENT_TYPE_PHOTO,
    HDMI_CONTENT_TYPE_CINEMA,
    HDMI_CONTENT_TYPE_GAME,
};

struct hdmi_avi_infoframe {
    enum hdmi_infoframe_type type;
    hi_u8 version;
    hi_u8 length;
    enum hdmi_colorspace colorspace;
    enum hdmi_scan_mode scan_mode;
    enum hdmi_colorimetry colorimetry;
    enum hdmi_picture_aspect picture_aspect;
    enum hdmi_active_aspect active_aspect;
    hi_bool itc;
    enum hdmi_extended_colorimetry extended_colorimetry;
    enum hdmi_quantization_range quantization_range;
    enum hdmi_nups nups;
    hi_u8 video_code;
    enum hdmi_ycc_quantization_range ycc_quantization_range;
    enum hdmi_content_type content_type;
    hi_u8 pixel_repeat;
    hi_u16 top_bar;
    hi_u16 bottom_bar;
    hi_u16 left_bar;
    hi_u16 right_bar;
};

hi_s32 hdmi_avi_infoframe_init(struct hdmi_avi_infoframe *frame);
ssize_t hdmi_avi_infoframe_pack(struct hdmi_avi_infoframe *frame, void *buffer,
                                size_t size);
ssize_t hdmi_avi_infoframe_pack_only(const struct hdmi_avi_infoframe *frame,
                                     void *buffer, size_t size);
hi_s32 hdmi_avi_infoframe_check(struct hdmi_avi_infoframe *frame);

enum hdmi_spd_sdi {
    HDMI_SPD_SDI_UNKNOWN,
    HDMI_SPD_SDI_DSTB,
    HDMI_SPD_SDI_DVDP,
    HDMI_SPD_SDI_DVHS,
    HDMI_SPD_SDI_HDDVR,
    HDMI_SPD_SDI_DVC,
    HDMI_SPD_SDI_DSC,
    HDMI_SPD_SDI_VCD,
    HDMI_SPD_SDI_GAME,
    HDMI_SPD_SDI_PC,
    HDMI_SPD_SDI_BD,
    HDMI_SPD_SDI_SACD,
    HDMI_SPD_SDI_HDDVD,
    HDMI_SPD_SDI_PMP,
};

struct hdmi_spd_infoframe {
    enum hdmi_infoframe_type type;
    hi_u8 version;
    hi_u8 length;
    hi_s8 vendor[8]; /* 8: len of vendor */
    hi_s8 product[16]; /* 16: len of vendor */
    enum hdmi_spd_sdi sdi;
};

hi_s32 hdmi_spd_infoframe_init(struct hdmi_spd_infoframe *frame,
                               const hi_s8 *vendor, const hi_s8 *product);
ssize_t hdmi_spd_infoframe_pack(struct hdmi_spd_infoframe *frame, void *buffer,
                                size_t size);
ssize_t hdmi_spd_infoframe_pack_only(const struct hdmi_spd_infoframe *frame,
                                     void *buffer, size_t size);
hi_s32 hdmi_spd_infoframe_check(struct hdmi_spd_infoframe *frame);

enum hdmi_audio_coding_type {
    HDMI_AUDIO_CODING_TYPE_STREAM,
    HDMI_AUDIO_CODING_TYPE_PCM,
    HDMI_AUDIO_CODING_TYPE_AC3,
    HDMI_AUDIO_CODING_TYPE_MPEG1,
    HDMI_AUDIO_CODING_TYPE_MP3,
    HDMI_AUDIO_CODING_TYPE_MPEG2,
    HDMI_AUDIO_CODING_TYPE_AAC_LC,
    HDMI_AUDIO_CODING_TYPE_DTS,
    HDMI_AUDIO_CODING_TYPE_ATRAC,
    HDMI_AUDIO_CODING_TYPE_DSD,
    HDMI_AUDIO_CODING_TYPE_EAC3,
    HDMI_AUDIO_CODING_TYPE_DTS_HD,
    HDMI_AUDIO_CODING_TYPE_MLP,
    HDMI_AUDIO_CODING_TYPE_DST,
    HDMI_AUDIO_CODING_TYPE_WMA_PRO,
    HDMI_AUDIO_CODING_TYPE_CXT,
};

enum hdmi_audio_sample_size {
    HDMI_AUDIO_SAMPLE_SIZE_STREAM,
    HDMI_AUDIO_SAMPLE_SIZE_16,
    HDMI_AUDIO_SAMPLE_SIZE_20,
    HDMI_AUDIO_SAMPLE_SIZE_24,
};

enum hdmi_audio_sample_frequency {
    HDMI_AUDIO_SAMPLE_FREQUENCY_STREAM,
    HDMI_AUDIO_SAMPLE_FREQUENCY_32000,
    HDMI_AUDIO_SAMPLE_FREQUENCY_44100,
    HDMI_AUDIO_SAMPLE_FREQUENCY_48000,
    HDMI_AUDIO_SAMPLE_FREQUENCY_88200,
    HDMI_AUDIO_SAMPLE_FREQUENCY_96000,
    HDMI_AUDIO_SAMPLE_FREQUENCY_176400,
    HDMI_AUDIO_SAMPLE_FREQUENCY_192000,
};

enum hdmi_audio_coding_type_ext {
    /* Refer to Audio Coding Type (CT) field in Data Byte 1 */
    HDMI_AUDIO_CODING_TYPE_EXT_CT,

    /*
	 * The next three CXT values are defined in CEA-861-E only.
	 * They do not exist in older versions, and in CEA-861-F they are
	 * defined as 'Not in use'.
	 */
    HDMI_AUDIO_CODING_TYPE_EXT_HE_AAC,
    HDMI_AUDIO_CODING_TYPE_EXT_HE_AAC_V2,
    HDMI_AUDIO_CODING_TYPE_EXT_MPEG_SURROUND,

    /* The following CXT values are only defined in CEA-861-F. */
    HDMI_AUDIO_CODING_TYPE_EXT_MPEG4_HE_AAC,
    HDMI_AUDIO_CODING_TYPE_EXT_MPEG4_HE_AAC_V2,
    HDMI_AUDIO_CODING_TYPE_EXT_MPEG4_AAC_LC,
    HDMI_AUDIO_CODING_TYPE_EXT_DRA,
    HDMI_AUDIO_CODING_TYPE_EXT_MPEG4_HE_AAC_SURROUND,
    HDMI_AUDIO_CODING_TYPE_EXT_MPEG4_AAC_LC_SURROUND = 10,
};

struct hdmi_audio_infoframe {
    enum hdmi_infoframe_type type;
    hi_u8 version;
    hi_u8 length;
    hi_u8 channels;
    enum hdmi_audio_coding_type coding_type;
    enum hdmi_audio_sample_size sample_size;
    enum hdmi_audio_sample_frequency sample_frequency;
    enum hdmi_audio_coding_type_ext coding_type_ext;
    hi_u8 channel_allocation;
    hi_u8 level_shift_value;
    hi_bool downmix_inhibit;
};

hi_s32 hdmi_audio_infoframe_init(struct hdmi_audio_infoframe *frame);
ssize_t hdmi_audio_infoframe_pack(struct hdmi_audio_infoframe *frame,
                                  void *buffer, size_t size);
ssize_t hdmi_audio_infoframe_pack_only(const struct hdmi_audio_infoframe *frame,
                                       void *buffer, size_t size);
hi_s32 hdmi_audio_infoframe_check(struct hdmi_audio_infoframe *frame);

enum hdmi_3d_structure {
    HDMI_3D_STRUCTURE_INVALID = -1,
    HDMI_3D_STRUCTURE_FRAME_PACKING = 0,
    HDMI_3D_STRUCTURE_FIELD_ALTERNATIVE,
    HDMI_3D_STRUCTURE_LINE_ALTERNATIVE,
    HDMI_3D_STRUCTURE_SIDE_BY_SIDE_FULL,
    HDMI_3D_STRUCTURE_L_DEPTH,
    HDMI_3D_STRUCTURE_L_DEPTH_GFX_GFX_DEPTH,
    HDMI_3D_STRUCTURE_TOP_AND_BOTTOM,
    HDMI_3D_STRUCTURE_SIDE_BY_SIDE_HALF = 8,
};

struct hdmi_vendor_infoframe {
    enum hdmi_infoframe_type type;
    hi_u8 version;
    hi_u8 length;
    hi_u32 oui;
    hi_u8 vic;
    enum hdmi_3d_structure s3d_struct;
    hi_u32 s3d_ext_data;
};

hi_s32 hdmi_vendor_infoframe_init(struct hdmi_vendor_infoframe *frame);
ssize_t hdmi_vendor_infoframe_pack(struct hdmi_vendor_infoframe *frame,
                                   void *buffer, size_t size);
ssize_t hdmi_vendor_infoframe_pack_only(const struct hdmi_vendor_infoframe *frame,
                                        void *buffer, size_t size);
hi_s32 hdmi_vendor_infoframe_check(struct hdmi_vendor_infoframe *frame);

struct hdmi_forum_vendor_infoframe {
    enum hdmi_infoframe_type type;
    hi_u8 version;
    hi_u8 length;
    hi_u32 oui;
    hi_u8 hf_version;
    hi_u8 _3d_valid;
    hi_u8 _3d_f_structure;
    hi_u8 _3d_additional_info_present;
    hi_u8 _3d_disparity_data_present;
    hi_u8 _3d_meta_present;
    hi_u8 _3d_f_ext_data;
    hi_u8 _3d_dual_view;
    hi_u8 _3d_view_dependency;
    hi_u8 _3d_preferred_2d_view;
    hi_u8 _3d_disparity_data_version;
    hi_u8 _3d_disparity_data_length;
    hi_u8 _3d_disparity_data[10]; /* 10: len of 3d_disparity_data */
    hi_u8 _3d_ext_data;
    hi_u8 _3d_metadata_type;
    hi_u8 _3d_metadata_length;
    hi_u8 _3d_metadata[10]; /* 10: len of 3d_metadata */
}; /* for hdmi2.0 */

hi_s32 hdmi_forum_vendor_infoframe_init(struct hdmi_forum_vendor_infoframe *frame);
ssize_t hdmi_forum_vendor_infoframe_pack(struct hdmi_forum_vendor_infoframe *frame,
                                         void *buffer, size_t size);
ssize_t hdmi_forum_vendor_infoframe_pack_only(const struct hdmi_forum_vendor_infoframe *frame,
                                              void *buffer, size_t size);
hi_s32 hdmi_forum_vendor_infoframe_check(struct hdmi_forum_vendor_infoframe *frame);

struct hdmi_dolby_vendor_infoframe {
    enum hdmi_infoframe_type type;
    hi_u8 version;
    hi_u8 length;
    hi_u32 oui;
    hi_u8 low_latency;
    hi_u8 dolbyvision_signal;
    hi_u8 backlt_ctrl_md_present;
    hi_u8 auxiliary_md_present;
    hi_u8 eff_tmax_pq_hi;
    hi_u8 eff_tmax_pq_low;
    hi_u8 auxiliary_runmode;
    hi_u8 auxiliary_runversion;
    hi_u8 auxiliary_debug;
}; /* for dolby vision */

hi_s32 hdmi_dolby_vendor_infoframe_init(struct hdmi_dolby_vendor_infoframe *frame);
ssize_t hdmi_dolby_vendor_infoframe_pack(struct hdmi_dolby_vendor_infoframe *frame,
                                         void *buffer, size_t size);
ssize_t hdmi_dolby_vendor_infoframe_pack_only(const struct hdmi_dolby_vendor_infoframe *frame,
                                              void *buffer, size_t size);
hi_s32 hdmi_dolby_vendor_infoframe_check(struct hdmi_dolby_vendor_infoframe *frame);

union hdmi_vendor_any_infoframe {
    struct {
        enum hdmi_infoframe_type type;
        hi_u8 version;
        hi_u8 length;
        hi_u32 oui;
    } any;
    struct hdmi_vendor_infoframe hdmi;             /* for hdmi1.4 or dolby vision v2 */
    struct hdmi_forum_vendor_infoframe hdmi_forum; /* for hdmi2.0 */
    struct hdmi_dolby_vendor_infoframe hdmi_dolby; /* for dolby vision v3 */
};

enum hdmi_eotf_type {
    HDMI_EOTF_SDR_LUMIN,     /* Traditional gamma - SDR Luminance Range */
    HDMI_EOTF_HDR_LUMIN,     /* Traditional gamma - HDR Luminance Range */
    HDMI_EOTF_SMPTE_ST_2084, /* SMPTE ST 2084 */
    HDMI_EOTF_HLG,           /* Hybrid Log-Gamma(HLG) */
    HDMI_EOTF_FUTURE_USE,    /* Future EOTF */
};

enum hdmi_static_metadata_type {
    HDMI_HDR_METADATA_ID_0, /* Static Metadata Type 1	 */
    HDMI_HDR_METADATA_ID_RESERVED,
};

struct hdmi_meta_descriptor_1st {
    hi_u16 primaries0_x;        /* display_primaries_x[0],range[0,50000],in units of 0.00002 */
    hi_u16 primaries0_y;        /* display_primaries_y[0],range[0,50000],in units of 0.00002 */
    hi_u16 primaries1_x;        /* display_primaries_x[1],range[0,50000],in units of 0.00002 */
    hi_u16 primaries1_y;        /* display_primaries_y[1],range[0,50000],in units of 0.00002 */
    hi_u16 primaries2_x;        /* display_primaries_x[2],range[0,50000],in units of 0.00002 */
    hi_u16 primaries2_y;        /* display_primaries_y[2],range[0,50000],in units of 0.00002 */
    hi_u16 white_point_x;       /* white_point_x,range[0,50000],in units of 0.00002 */
    hi_u16 white_point_y;       /* white_point_y,range[0,50000],in units of 0.00002 */
    hi_u16 max_luminance;       /* max_display_mastering_luminance,in units of 1 cd/m^2 */
    hi_u16 min_luminance;       /* min_display_mastering_luminance,in units of 0.0001 cd/m^2 */
    hi_u16 max_light_level;     /* Maximum Content Light Level,in units of 1 cd/m2 */
    hi_u16 average_light_level; /* Maximum Frame-average Light Level,in units of 1 cd/m^2 */
};

union hdmi_meta_Descriptor {
    struct hdmi_meta_descriptor_1st type1; /* Static Metadata Descriptor Type1 */
};

struct hdmi_drm_infoframe {
    enum hdmi_infoframe_type type;
    hi_u8 version;
    hi_u8 length;
    enum hdmi_eotf_type eotf_type;
    enum hdmi_static_metadata_type metadata_type;
    union hdmi_meta_Descriptor meta_descriptor;
};

hi_s32 hdmi_drm_infoframe_init(struct hdmi_drm_infoframe *frame);
ssize_t hdmi_drm_infoframe_pack(struct hdmi_drm_infoframe *frame,
                                void *buffer, size_t size);
ssize_t hdmi_drm_infoframe_pack_only(const struct hdmi_drm_infoframe *frame,
                                     void *buffer, size_t size);
hi_s32 hdmi_drm_infoframe_check(struct hdmi_drm_infoframe *frame);

/**
 * union hdmi_infoframe - overall union of all abstract infoframe representations
 * @any: generic infoframe
 * @avi: avi infoframe
 * @spd: spd infoframe
 * @vendor: union of all vendor infoframes
 * @audio: audio infoframe
 *
 * This is used by the generic pack function. This works since all infoframes
 * have the same header which also indicates which type of infoframe should be
 * packed.
 */
union hdmi_infoframe {
    struct hdmi_any_infoframe any;
    struct hdmi_avi_infoframe avi;
    struct hdmi_spd_infoframe spd;
    union hdmi_vendor_any_infoframe vendor;
    struct hdmi_audio_infoframe audio;
    struct hdmi_drm_infoframe drm;
};

ssize_t hdmi_infoframe_pack(union hdmi_infoframe *frame, void *buffer,
                            size_t size);
ssize_t hdmi_infoframe_pack_only(const union hdmi_infoframe *frame,
                                 void *buffer, size_t size);
hi_s32 hdmi_infoframe_check(union hdmi_infoframe *frame);
hi_s32 hdmi_infoframe_unpack(union hdmi_infoframe *frame,
                             const void *buffer, size_t size);
void hdmi_infoframe_log(const union hdmi_infoframe *frame);

#define NUM_BUF_RANGES 15

#define HDMI_PACKET_SIZE 31

#define HDMI_ORGANIZATION_ID 1

#define HDMI_CTA_ORGANIZATION_ID 2

#define HDMI_PACKET_HEADER_SIZE 3
#define HDMI_DSF_HEADE_SIZE     7

#define HDMI_VT_EM_DATA_SIZE  4
#define HDMI_DSC_EM_DATA_SIZE 136

#define HDMI_VT_EM_DATA_SET_TAG  0x1
#define HDMI_DSC_EM_DATA_SET_TAG 0x2

#define HDMI_CTA_HDR1_EM_DATA_SIZE 21 /* Need confirm */
#define HDMI_CTA_HDR2_EM_DATA_SIZE 21 /* Need confirm */
#define HDMI_CTA_HDR3_EM_DATA_SIZE 21 /* Need confirm */
#define HDMI_CTA_HDR4_EM_DATA_SIZE 21 /* Need confirm */

#define HDMI_CTA_HDR1_EM_DATA_SET_TAG 0x1
#define HDMI_CTA_HDR2_EM_DATA_SET_TAG 0x2
#define HDMI_CTA_HDR3_EM_DATA_SET_TAG 0x3
#define HDMI_CTA_HDR4_EM_DATA_SET_TAG 0x4

#define HDMI_DSC_EM_DATA_PACKET_NUM 6

struct hdmi_any_extended_metadata {
    enum hdmi_packet_type type;
    hi_u8 first;
    hi_u8 last;
    hi_u8 sequence_index;
    hi_u8 pkt_new;
    hi_u8 pkt_end;
    hi_u8 ds_type;
    hi_u8 afr;
    hi_u8 vfr;
    hi_u8 sync;
    hi_u8 organization_id;
};

struct hdmi_hdmi_vt_ext_metadata {
    enum hdmi_packet_type type;
    hi_u8 first;
    hi_u8 last;
    hi_u8 sequence_index;
    hi_u8 pkt_new;
    hi_u8 pkt_end;
    hi_u8 ds_type;
    hi_u8 afr;
    hi_u8 vfr;
    hi_u8 sync;
    hi_u8 organization_id;
    hi_u16 data_set_tag;
    hi_u16 data_set_length;
    hi_u8 fva_factor_m1;
    hi_u8 m_const;
    hi_u8 vrr_en;
    hi_u8 base_vfront;
    hi_u8 rb;
    hi_u16 base_refresh_rate;
};

hi_s32 hdmi_hdmi_vt_ext_metadata_init(struct hdmi_hdmi_vt_ext_metadata *em_data);
ssize_t hdmi_hdmi_vt_ext_metadata_pack(struct hdmi_hdmi_vt_ext_metadata *em_data,
                                       void *buffer, size_t size);
ssize_t hdmi_hdmi_vt_ext_metadata_pack_only(const struct hdmi_hdmi_vt_ext_metadata *em_data,
                                            void *buffer, size_t size);
hi_s32 hdmi_hdmi_vt_ext_metadata_check(struct hdmi_hdmi_vt_ext_metadata *em_data);

struct rc_parameter_set {
    hi_u8 range_min_qp;
    hi_u8 range_max_qp;
    hi_u8 range_bpg_offset;
};

struct hdmi_hdmi_dsc_ext_metadata {
    enum hdmi_packet_type type;
    hi_u8 first;
    hi_u8 last;
    hi_u8 sequence_index;
    hi_u8 pkt_new;
    hi_u8 pkt_end;
    hi_u8 ds_type;
    hi_u8 afr;
    hi_u8 vfr;
    hi_u8 sync;
    hi_u8 organization_id;
    hi_u16 data_set_tag;
    hi_u16 data_set_length;
    hi_u8 dsc_version_major;
    hi_u8 dsc_version_minor;
    hi_u8 pps_identifier;
    hi_u8 bits_per_component;
    hi_u8 linebuf_depth;
    hi_u8 block_pred_enable;
    hi_u8 convert_rgb;
    hi_u8 simple_422;
    hi_u8 vbr_enable;
    hi_u16 bits_per_pixel;
    hi_u16 pic_height;
    hi_u16 pic_width;
    hi_u16 slice_height;
    hi_u16 slice_width;
    hi_u16 chunk_size;
    hi_u16 initial_xmit_delay;
    hi_u16 initial_dec_delay;
    hi_u8 initial_scale_value;
    hi_u16 scale_increment_interval;
    hi_u16 scale_decrement_interval;
    hi_u8 first_line_bpg_offset;
    hi_u16 nfl_bpg_offset;
    hi_u16 slice_bpg_offset;
    hi_u16 inital_offset;
    hi_u16 final_offset;
    hi_u8 flatness_min_qp;
    hi_u8 flatness_max_qp;
    hi_u16 rc_model_size;
    hi_u8 rc_edge_factor;
    hi_u8 rc_quant_incr_limit0;
    hi_u8 rc_quant_incr_limit1;
    hi_u8 rc_tgt_offset_hi;
    hi_u8 rc_tgt_offset_lo;
    hi_u8 rc_buf_thresh[NUM_BUF_RANGES - 1];
    struct rc_parameter_set rc_parameters[NUM_BUF_RANGES];
    hi_u8 native_420;
    hi_u8 native_422;
    hi_u8 second_line_bpg_offset;
    hi_u16 nsl_bpg_offset;
    hi_u16 second_line_offset_adj;
    hi_u16 hfront;
    hi_u16 hsync;
    hi_u16 hback;
    hi_u16 hcactive;
};

hi_s32 hdmi_hdmi_dsc_ext_metadata_init(struct hdmi_hdmi_dsc_ext_metadata *em_data);
ssize_t hdmi_hdmi_dsc_ext_metadata_pack(struct hdmi_hdmi_dsc_ext_metadata *em_data,
                                        void *buffer, size_t size);
ssize_t hdmi_hdmi_dsc_ext_metadata_pack_only(const struct hdmi_hdmi_dsc_ext_metadata *em_data,
                                             void *buffer, size_t size);
hi_s32 hdmi_hdmi_dsc_ext_metadata_check(struct hdmi_hdmi_dsc_ext_metadata *em_data);

union hdmi_hdmi_extended_metadata {
    struct {
        enum hdmi_packet_type type;
        hi_u8 first;
        hi_u8 last;
        hi_u8 sequence_index;
        hi_u8 pkt_new;
        hi_u8 pkt_end;
        hi_u8 ds_type;
        hi_u8 afr;
        hi_u8 vfr;
        hi_u8 sync;
        hi_u8 organization_id;
        hi_u16 data_set_tag;
        hi_u16 data_set_length;
    } any;
    struct hdmi_hdmi_vt_ext_metadata vt;
    struct hdmi_hdmi_dsc_ext_metadata dsc;
};

struct hdmi_cta_hdr1_ext_metadata {
    enum hdmi_packet_type type;
    hi_u8 first;
    hi_u8 last;
    hi_u8 sequence_index;
    hi_u8 pkt_new;
    hi_u8 pkt_end;
    hi_u8 ds_type;
    hi_u8 afr;
    hi_u8 vfr;
    hi_u8 sync;
    hi_u8 organization_id;
    hi_u16 data_set_tag;
    hi_u16 data_set_length;
};

hi_s32 hdmi_cta_hdr1_ext_metadata_init(struct hdmi_cta_hdr1_ext_metadata *em_data);
ssize_t hdmi_cta_hdr1_ext_metadata_pack(struct hdmi_cta_hdr1_ext_metadata *em_data,
                                        void *buffer, size_t size);
ssize_t hdmi_cta_hdr1_ext_metadata_pack_only(const struct hdmi_cta_hdr1_ext_metadata *em_data,
                                             void *buffer, size_t size);
hi_s32 hdmi_cta_hdr1_ext_metadata_check(struct hdmi_cta_hdr1_ext_metadata *em_data);

struct hdmi_cta_hdr2_ext_metadata {
    enum hdmi_packet_type type;
    hi_u8 first;
    hi_u8 last;
    hi_u8 sequence_index;
    hi_u8 pkt_new;
    hi_u8 pkt_end;
    hi_u8 ds_type;
    hi_u8 afr;
    hi_u8 vfr;
    hi_u8 sync;
    hi_u8 organization_id;
    hi_u16 data_set_tag;
    hi_u16 data_set_length;
};

hi_s32 hdmi_cta_hdr2_ext_metadata_init(struct hdmi_cta_hdr2_ext_metadata *em_data);
ssize_t hdmi_cta_hdr2_ext_metadata_pack(struct hdmi_cta_hdr2_ext_metadata *em_data,
                                        void *buffer, size_t size);
ssize_t hdmi_cta_hdr2_ext_metadata_pack_only(const struct hdmi_cta_hdr2_ext_metadata *em_data,
                                             void *buffer, size_t size);
hi_s32 hdmi_cta_hdr2_ext_metadata_check(struct hdmi_cta_hdr2_ext_metadata *em_data);

struct hdmi_cta_hdr3_ext_metadata {
    enum hdmi_packet_type type;
    hi_u8 first;
    hi_u8 last;
    hi_u8 sequence_index;
    hi_u8 pkt_new;
    hi_u8 pkt_end;
    hi_u8 ds_type;
    hi_u8 afr;
    hi_u8 vfr;
    hi_u8 sync;
    hi_u8 organization_id;
    hi_u16 data_set_tag;
    hi_u16 data_set_length;
};

hi_s32 hdmi_cta_hdr3_ext_metadata_init(struct hdmi_cta_hdr3_ext_metadata *em_data);
ssize_t hdmi_cta_hdr3_ext_metadata_pack(struct hdmi_cta_hdr3_ext_metadata *em_data,
                                        void *buffer, size_t size);
ssize_t hdmi_cta_hdr3_ext_metadata_pack_only(const struct hdmi_cta_hdr3_ext_metadata *em_data,
                                             void *buffer, size_t size);
hi_s32 hdmi_cta_hdr3_ext_metadata_check(struct hdmi_cta_hdr3_ext_metadata *em_data);

struct hdmi_cta_hdr4_ext_metadata {
    enum hdmi_packet_type type;
    hi_u8 first;
    hi_u8 last;
    hi_u8 sequence_index;
    hi_u8 pkt_new;
    hi_u8 pkt_end;
    hi_u8 ds_type;
    hi_u8 afr;
    hi_u8 vfr;
    hi_u8 sync;
    hi_u8 organization_id;
    hi_u16 data_set_tag;
    hi_u16 data_set_length;
};

hi_s32 hdmi_cta_hdr4_ext_metadata_init(struct hdmi_cta_hdr4_ext_metadata *em_data);
ssize_t hdmi_cta_hdr4_ext_metadata_pack(struct hdmi_cta_hdr4_ext_metadata *em_data,
                                        void *buffer, size_t size);
ssize_t hdmi_cta_hdr4_ext_metadata_pack_only(const struct hdmi_cta_hdr4_ext_metadata *em_data,
                                             void *buffer, size_t size);
hi_s32 hdmi_cta_hdr4_ext_metadata_check(struct hdmi_cta_hdr4_ext_metadata *em_data);

union hdmi_cta_extended_metadata {
    struct {
        enum hdmi_packet_type type;
        hi_u8 first;
        hi_u8 last;
        hi_u8 sequence_index;
        hi_u8 pkt_new;
        hi_u8 pkt_end;
        hi_u8 ds_type;
        hi_u8 afr;
        hi_u8 vfr;
        hi_u8 sync;
        hi_u8 organization_id;
        hi_u16 data_set_tag;
        hi_u16 data_set_length;
    } any;
    struct hdmi_cta_hdr1_ext_metadata hdr1;
    struct hdmi_cta_hdr2_ext_metadata hdr2;
    struct hdmi_cta_hdr3_ext_metadata hdr3;
    struct hdmi_cta_hdr4_ext_metadata hdr4;
};

union hdmi_extended_metadata {
    struct hdmi_any_extended_metadata any;
    union hdmi_hdmi_extended_metadata hdmi;
    union hdmi_cta_extended_metadata cta;
};

ssize_t hdmi_extended_metadata_pack(union hdmi_extended_metadata *em_data, void *buffer,
                                    size_t size);
ssize_t hdmi_extended_metadata_pack_only(const union hdmi_extended_metadata *em_data,
                                         void *buffer, size_t size);
hi_s32 hdmi_extended_metadata_check(union hdmi_extended_metadata *em_data);
hi_s32 hdmi_extended_metadata_unpack(union hdmi_extended_metadata *em_data,
                                     const void *buffer, size_t size);
void hdmi_extended_metadata_log(const union hdmi_extended_metadata *em_data);
hi_s32 hdmi_hdmi_any_ex_metadata_unpack(union hdmi_hdmi_extended_metadata *em_data,
                                        const void *buffer, size_t size);
#endif /* __DRV_HDMITX_INFOFRAME_H__ */
