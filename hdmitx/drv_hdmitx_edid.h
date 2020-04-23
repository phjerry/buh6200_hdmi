/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver edid header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */
#ifndef __DRV_HDMITX_EDID_H__
#define __DRV_HDMITX_EDID_H__

#include <hi_type.h>

struct hdmi_connector;
struct hisilicon_hdmi;

#define DATA_BLOCK_PRODUCT_ID                 0x00
#define DATA_BLOCK_DISPLAY_PARAMETERS         0x01
#define DATA_BLOCK_COLOR_CHARACTERISTICS      0x02
#define DATA_BLOCK_TYPE_1_DETAILED_TIMING     0x03
#define DATA_BLOCK_TYPE_2_DETAILED_TIMING     0x04
#define DATA_BLOCK_TYPE_3_SHORT_TIMING        0x05
#define DATA_BLOCK_TYPE_4_DMT_TIMING          0x06
#define DATA_BLOCK_VESA_TIMING                0x07
#define DATA_BLOCK_CEA_TIMING                 0x08
#define DATA_BLOCK_VIDEO_TIMING_RANGE         0x09
#define DATA_BLOCK_PRODUCT_SERIAL_NUMBER      0x0a
#define DATA_BLOCK_GP_ASCII_STRING            0x0b
#define DATA_BLOCK_DISPLAY_DEVICE_DATA        0x0c
#define DATA_BLOCK_INTERFACE_POWER_SEQUENCING 0x0d
#define DATA_BLOCK_TRANSFER_CHARACTERISTICS   0x0e
#define DATA_BLOCK_DISPLAY_INTERFACE          0x0f
#define DATA_BLOCK_STEREO_DISPLAY_INTERFACE   0x10
#define DATA_BLOCK_TILED_DISPLAY              0x12

#define DATA_BLOCK_VENDOR_SPECIFIC 0x7f

#define PRODUCT_TYPE_EXTENSION    0
#define PRODUCT_TYPE_TEST         1
#define PRODUCT_TYPE_PANEL        2
#define PRODUCT_TYPE_MONITOR      3
#define PRODUCT_TYPE_TV           4
#define PRODUCT_TYPE_REPEATER     5
#define PRODUCT_TYPE_DIRECT_DRIVE 6

struct displayid_hdr {
    hi_u8 rev;
    hi_u8 bytes;
    hi_u8 prod_id;
    hi_u8 ext_count;
} __packed;

struct displayid_block {
    hi_u8 tag;
    hi_u8 rev;
    hi_u8 num_bytes;
} __packed;

struct displayid_tiled_block {
    struct displayid_block base;
    hi_u8 tile_cap;
    hi_u8 topo[3];
    hi_u8 tile_size[4];
    hi_u8 tile_pixel_bezel[5];
    hi_u8 topology_id[8];
} __packed;

struct displayid_detailed_timings_1 {
    hi_u8 pixel_clock[3];
    hi_u8 flags;
    hi_u8 hactive[2];
    hi_u8 hblank[2];
    hi_u8 hsync[2];
    hi_u8 hsw[2];
    hi_u8 vactive[2];
    hi_u8 vblank[2];
    hi_u8 vsync[2];
    hi_u8 vsw[2];
} __packed;

struct displayid_detailed_timing_block {
    struct displayid_block base;
    struct displayid_detailed_timings_1 timings[0];
};

#define EDID_LENGTH 128
#define DDC_ADDR    0x50
#define DDC_ADDR2   0x52 /* E-DDC 1.2 - where DisplayID can hide */

#define CEA_EXT       0x02
#define VTB_EXT       0x10
#define DI_EXT        0x40
#define LS_EXT        0x50
#define MI_EXT        0x60
#define DISPLAYID_EXT 0x70

struct est_timings {
    hi_u8 t1;
    hi_u8 t2;
    hi_u8 mfg_rsvd;
} __attribute__((packed));

/* 00=16:10, 01=4:3, 10=5:4, 11=16:9 */
#define EDID_TIMING_ASPECT_SHIFT 6
#define EDID_TIMING_ASPECT_MASK  (0x3 << EDID_TIMING_ASPECT_SHIFT)

/* need to add 60 */
#define EDID_TIMING_VFREQ_SHIFT 0
#define EDID_TIMING_VFREQ_MASK  (0x3f << EDID_TIMING_VFREQ_SHIFT)

struct std_timing {
    hi_u8 hsize; /* need to multiply by 8 then add 248 */
    hi_u8 vfreq_aspect;
} __attribute__((packed));

#define HDMI_EDID_PT_HSYNC_POSITIVE (1 << 1)
#define HDMI_EDID_PT_VSYNC_POSITIVE (1 << 2)
#define HDMI_EDID_PT_SEPARATE_SYNC  (3 << 3)
#define HDMI_EDID_PT_STEREO         (1 << 5)
#define HDMI_EDID_PT_INTERLACED     (1 << 7)

/* If detailed data is pixel timing */
struct detailed_pixel_timing {
    hi_u8 hactive_lo;
    hi_u8 hblank_lo;
    hi_u8 hactive_hblank_hi;
    hi_u8 vactive_lo;
    hi_u8 vblank_lo;
    hi_u8 vactive_vblank_hi;
    hi_u8 hsync_offset_lo;
    hi_u8 hsync_pulse_width_lo;
    hi_u8 vsync_offset_pulse_width_lo;
    hi_u8 hsync_vsync_offset_pulse_width_hi;
    hi_u8 width_mm_lo;
    hi_u8 height_mm_lo;
    hi_u8 width_height_mm_hi;
    hi_u8 hborder;
    hi_u8 vborder;
    hi_u8 misc;
} __attribute__((packed));

/* If it's not pixel timing, it'll be one of the below */
struct detailed_data_string {
    hi_u8 str[13];
} __attribute__((packed));

struct detailed_data_monitor_range {
    hi_u8 min_vfreq;
    hi_u8 max_vfreq;
    hi_u8 min_hfreq_khz;
    hi_u8 max_hfreq_khz;
    hi_u8 pixel_clock_mhz; /* need to multiply by 10 */
    hi_u8 flags;
    union {
        struct {
            hi_u8 reserved;
            hi_u8 hfreq_start_khz; /* need to multiply by 2 */
            hi_u8 c;               /* need to divide by 2 */
            __le16 m;
            hi_u8 k;
            hi_u8 j; /* need to divide by 2 */
        } __attribute__((packed)) gtf2;
        struct {
            hi_u8 version;
            hi_u8 data1; /* high 6 bits: extra clock resolution */
            hi_u8 data2; /* plus low 2 of above: max hactive */
            hi_u8 supported_aspects;
            hi_u8 flags; /* preferred aspect and blanking support */
            hi_u8 supported_scalings;
            hi_u8 preferred_refresh;
        } __attribute__((packed)) cvt;
    } formula;
} __attribute__((packed));

struct detailed_data_wpindex {
    hi_u8 white_yx_lo; /* Lower 2 bits each */
    hi_u8 white_x_hi;
    hi_u8 white_y_hi;
    hi_u8 gamma; /* need to divide by 100 then add 1 */
} __attribute__((packed));

struct detailed_data_color_point {
    hi_u8 windex1;
    hi_u8 wpindex1[3];
    hi_u8 windex2;
    hi_u8 wpindex2[3];
} __attribute__((packed));

struct cvt_timing {
    hi_u8 code[3];
} __attribute__((packed));

struct detailed_non_pixel {
    hi_u8 pad1;
    hi_u8 type;
    /*
     *  ff=serial, fe=string, fd=monitor range, fc=monitor name
     *  fb=color point data, fa=standard timing data,
     *  f9=undefined, f8=mfg. reserved
     */
    hi_u8 pad2;
    union {
        struct detailed_data_string str;
        struct detailed_data_monitor_range range;
        struct detailed_data_wpindex color;
        struct std_timing timings[6];
        struct cvt_timing cvt[4];
    } data;
} __attribute__((packed));

#define EDID_DETAIL_EST_TIMINGS     0xf7
#define EDID_DETAIL_CVT_3BYTE       0xf8
#define EDID_DETAIL_COLOR_MGMT_DATA 0xf9
#define EDID_DETAIL_STD_MODES       0xfa
#define EDID_DETAIL_MONITOR_CPDATA  0xfb
#define EDID_DETAIL_MONITOR_NAME    0xfc
#define EDID_DETAIL_MONITOR_RANGE   0xfd
#define EDID_DETAIL_MONITOR_STRING  0xfe
#define EDID_DETAIL_MONITOR_SERIAL  0xff

struct detailed_timing {
    __le16 pixel_clock; /* need to multiply by 10 KHz */
    union {
        struct detailed_pixel_timing pixel_data;
        struct detailed_non_pixel other_data;
    } data;
} __attribute__((packed));

#define HDMI_EDID_INPUT_SERRATION_VSYNC (1 << 0)
#define HDMI_EDID_INPUT_SYNC_ON_GREEN   (1 << 1)
#define HDMI_EDID_INPUT_COMPOSITE_SYNC  (1 << 2)
#define HDMI_EDID_INPUT_SEPARATE_SYNCS  (1 << 3)
#define HDMI_EDID_INPUT_BLANK_TO_BLACK  (1 << 4)
#define HDMI_EDID_INPUT_VIDEO_LEVEL     (3 << 5)
#define HDMI_EDID_INPUT_DIGITAL         (1 << 7)
#define HDMI_EDID_DIGITAL_DEPTH_MASK    (7 << 4)
#define HDMI_EDID_DIGITAL_DEPTH_UNDEF   (0 << 4)
#define HDMI_EDID_DIGITAL_DEPTH_6       (1 << 4)
#define HDMI_EDID_DIGITAL_DEPTH_8       (2 << 4)
#define HDMI_EDID_DIGITAL_DEPTH_10      (3 << 4)
#define HDMI_EDID_DIGITAL_DEPTH_12      (4 << 4)
#define HDMI_EDID_DIGITAL_DEPTH_14      (5 << 4)
#define HDMI_EDID_DIGITAL_DEPTH_16      (6 << 4)
#define HDMI_EDID_DIGITAL_DEPTH_RSVD    (7 << 4)
#define HDMI_EDID_DIGITAL_TYPE_UNDEF    (0)
#define HDMI_EDID_DIGITAL_TYPE_DVI      (1)
#define HDMI_EDID_DIGITAL_TYPE_HDMI_A   (2)
#define HDMI_EDID_DIGITAL_TYPE_HDMI_B   (3)
#define HDMI_EDID_DIGITAL_TYPE_MDDI     (4)
#define HDMI_EDID_DIGITAL_TYPE_DP       (5)

#define HDMI_EDID_FEATURE_DEFAULT_GTF      (1 << 0)
#define HDMI_EDID_FEATURE_PREFERRED_TIMING (1 << 1)
#define HDMI_EDID_FEATURE_STANDARD_COLOR   (1 << 2)
/* If analog */
/* 00:mono, 01:rgb, 10:non-rgb, 11:unknown */
#define HDMI_EDID_FEATURE_DISPLAY_TYPE (3 << 3)
/* If digital */
#define HDMI_EDID_FEATURE_COLOR_MASK   (3 << 3)
#define HDMI_EDID_FEATURE_RGB          (0 << 3)
#define HDMI_EDID_FEATURE_RGB_YCRCB444 (1 << 3)
#define HDMI_EDID_FEATURE_RGB_YCRCB422 (2 << 3)
#define HDMI_EDID_FEATURE_RGB_YCRCB    (3 << 3) /* both 4:4:4 and 4:2:2 */

#define HDMI_EDID_FEATURE_PM_ACTIVE_OFF (1 << 5)
#define HDMI_EDID_FEATURE_PM_SUSPEND    (1 << 6)
#define HDMI_EDID_FEATURE_PM_STANDBY    (1 << 7)

#define HDMI_EDID_HDMI_DC_48   (1 << 6)
#define HDMI_EDID_HDMI_DC_36   (1 << 5)
#define HDMI_EDID_HDMI_DC_30   (1 << 4)
#define HDMI_EDID_HDMI_DC_Y444 (1 << 3)

/* YCBCR 420 deep color modes */
#define HDMI_EDID_YCBCR420_DC_48   (1 << 2)
#define HDMI_EDID_YCBCR420_DC_36   (1 << 1)
#define HDMI_EDID_YCBCR420_DC_30   (1 << 0)
#define HDMI_EDID_YCBCR420_DC_MASK (HDMI_EDID_YCBCR420_DC_48 | \
                                    HDMI_EDID_YCBCR420_DC_36 | \
                                    HDMI_EDID_YCBCR420_DC_30)

/* ELD Header Block */
#define HDMI_ELD_HEADER_BLOCK_SIZE 4

#define HDMI_ELD_VER         0
#define HDMI_ELD_VER_SHIFT   3
#define HDMI_ELD_VER_MASK    (0x1f << 3)
#define HDMI_ELD_VER_CEA861D (2 << 3) /* supports 861D or below */
#define HDMI_ELD_VER_CANNED  (0x1f << 3)

#define HDMI_ELD_BASELINE_ELD_LEN 2 /* in dwords! */

/* ELD Baseline Block for ELD_Ver == 2 */
#define HDMI_ELD_CEA_EDID_VER_MNL       4
#define HDMI_ELD_CEA_EDID_VER_SHIFT     5
#define HDMI_ELD_CEA_EDID_VER_MASK      (7 << 5)
#define HDMI_ELD_CEA_EDID_VER_NONE      (0 << 5)
#define HDMI_ELD_CEA_EDID_VER_CEA861    (1 << 5)
#define HDMI_ELD_CEA_EDID_VER_CEA861A   (2 << 5)
#define HDMI_ELD_CEA_EDID_VER_CEA861BCD (3 << 5)
#define HDMI_ELD_MNL_SHIFT              0
#define HDMI_ELD_MNL_MASK               (0x1f << 0)

#define HDMI_ELD_SAD_COUNT_CONN_TYPE 5
#define HDMI_ELD_SAD_COUNT_SHIFT     4
#define HDMI_ELD_SAD_COUNT_MASK      (0xf << 4)
#define HDMI_ELD_CONN_TYPE_SHIFT     2
#define HDMI_ELD_CONN_TYPE_MASK      (3 << 2)
#define HDMI_ELD_CONN_TYPE_HDMI      (0 << 2)
#define HDMI_ELD_CONN_TYPE_DP        (1 << 2)
#define HDMI_ELD_SUPPORTS_AI         (1 << 1)
#define HDMI_ELD_SUPPORTS_HDCP       (1 << 0)

#define HDMI_ELD_AUD_SYNCH_DELAY     6    /* in units of 2 ms */
#define HDMI_ELD_AUD_SYNCH_DELAY_MAX 0xfa /* 500 ms */

#define HDMI_ELD_SPEAKER      7
#define HDMI_ELD_SPEAKER_MASK 0x7f
#define HDMI_ELD_SPEAKER_RLRC (1 << 6)
#define HDMI_ELD_SPEAKER_FLRC (1 << 5)
#define HDMI_ELD_SPEAKER_RC   (1 << 4)
#define HDMI_ELD_SPEAKER_RLR  (1 << 3)
#define HDMI_ELD_SPEAKER_FC   (1 << 2)
#define HDMI_ELD_SPEAKER_LFE  (1 << 1)
#define HDMI_ELD_SPEAKER_FLR  (1 << 0)

#define HDMI_ELD_PORT_ID     8 /* offsets 8..15 inclusive */
#define HDMI_ELD_PORT_ID_LEN 8

#define HDMI_ELD_MANUFACTURER_NAME0 16
#define HDMI_ELD_MANUFACTURER_NAME1 17

#define HDMI_ELD_PRODUCT_CODE0 18
#define HDMI_ELD_PRODUCT_CODE1 19

/* offsets 20..(20+mnl-1) inclusive */
#define HDMI_ELD_MONITOR_NAME_STRING 20

#define HDMI_ELD_CEA_SAD(mnl, sad) (20 + (mnl) + 3 * (sad))

#define HDMI_IEEE_OUI       0x000c03
#define HDMI_FORUM_IEEE_OUI 0xc45dd8
#define HDMI_DOLBY_IEEE_OUI 0x00d046

#define EDID_DOLBY_VSVDB_VERSION_0_LEN 0x19
#define EDID_DOLBY_VSVDB_VERSION_1_LEN 0x0e
#define EDID_DOLBY_VSVDB_VERSION_2_LEN 0x0b
#define EDID_DOLBY_VSVDB_VERSION_0     0x00
#define EDID_DOLBY_VSVDB_VERSION_1     0x01
#define EDID_DOLBY_VSVDB_VERSION_2     0x02

#define EDID_DOLBY_LOWER_2BIT_MASK   0x03
#define EDID_DOLBY_LOWER_3BIT_MASK   0x07
#define EDID_DOLBY_LOWER_NIBBLE_MASK 0x0F
#define EDID_DOLBY_LOWER_5BIT_MASK   0x1F
#define EDID_DOLBY_LOWER_7BIT_MASK   0x7F

/* AUDIO DATA BLOCK */
#define EDID_AUDIO_FORMAT_MASK   (0xf << 3)
#define EDID_AUDIO_CHANNEL_MASK  (0x7 << 0)
#define EDID_AUDIO_EXT_TYPE_CODE (0x1f << 3)

#define BIT0_MASK 0x01
#define BIT1_MASK 0x02
#define BIT2_MASK 0x04
#define BIT3_MASK 0x08
#define BIT4_MASK 0x10
#define BIT5_MASK 0x20
#define BIT6_MASK 0x40
#define BIT7_MASK 0x80

#define MODE_TYPE_DETAILED_TIMINGE    (1 << 0)
#define MODE_TYPE_ESTABLISHED_TIMINGE (1 << 1)
#define MODE_TYPE_STD_TIMINGE         (1 << 2)
#define MODE_TYPE_VSDB_4K             (1 << 3)
#define MODE_TYPE_VSDB_3D             (1 << 4)
#define MODE_TYPE_VDB                 (1 << 5)
#define MODE_TYPE_VDB_Y420CMDB        (1 << 6)
#define MODE_TYPE_Y420VDB             (1 << 7)

struct edid {
    hi_u8 header[8];
    /* Vendor & product info */
    hi_u8 mfg_id[2];
    hi_u8 prod_code[2];
    hi_u32 serial;
    hi_u8 mfg_week;
    hi_u8 mfg_year;
    /* EDID version */
    hi_u8 version;
    hi_u8 revision;
    /* Display info: */
    hi_u8 input;
    hi_u8 width_cm;
    hi_u8 height_cm;
    hi_u8 gamma; /* real value = (gamma+100)/100 ,range [1.0~3.54] */
    hi_u8 features;
    /* Color characteristics */
    hi_u8 red_green_lo;
    hi_u8 black_white_lo;
    hi_u8 red_x;
    hi_u8 red_y;
    hi_u8 green_x;
    hi_u8 green_y;
    hi_u8 blue_x;
    hi_u8 blue_y;
    hi_u8 white_x;
    hi_u8 white_y;
    /* Est. timings and mfg rsvd timings */
    struct est_timings established_timings;
    /* Standard timings 1-8 */
    struct std_timing standard_timings[8];
    /* Detailing timings 1-4 */
    struct detailed_timing detailed_timings[4];
    /* Number of 128 byte ext. blocks */
    hi_u8 extensions;
    /* Checksum */
    hi_u8 checksum;
} __attribute__((packed));

#define EDID_PRODUCT_ID(e) ((e)->prod_code[0] | ((e)->prod_code[1] << 8))

/*
 * hdmi_eld_mnl - Get ELD monitor name length in bytes.
 * @eld: pointer to an eld memory structure with mnl set
 */
static inline hi_s32 hdmi_eld_mnl(const hi_u8 *eld)
{
    return (eld[HDMI_ELD_CEA_EDID_VER_MNL] & HDMI_ELD_MNL_MASK) >> HDMI_ELD_MNL_SHIFT;
}

/*
 * hdmi_eld_sad - Get ELD SAD structures.
 * @eld: pointer to an eld memory structure with sad_count set
 */
static inline const hi_u8 *hdmi_eld_sad(const hi_u8 *eld)
{
    hi_u32 ver, mnl;

    ver = (eld[HDMI_ELD_VER] & HDMI_ELD_VER_MASK) >> HDMI_ELD_VER_SHIFT;
    if (ver != 2 && ver != 31) {
        return NULL;
    }

    mnl = hdmi_eld_mnl(eld);
    if (mnl > 16) {
        return NULL;
    }

    return eld + HDMI_ELD_CEA_SAD(mnl, 0);
}

/*
 * hdmi_eld_sad_count - Get ELD SAD count.
 * @eld: pointer to an eld memory structure with sad_count set
 */
static inline hi_s32 hdmi_eld_sad_count(const hi_u8 *eld)
{
    return (eld[HDMI_ELD_SAD_COUNT_CONN_TYPE] & HDMI_ELD_SAD_COUNT_MASK) >>
           HDMI_ELD_SAD_COUNT_SHIFT;
}

/*
 * hdmi_eld_calc_baseline_block_size - Calculate baseline block size in bytes
 * @eld: pointer to an eld memory structure with mnl and sad_count set
 * This is a helper for determining the payload size of the baseline block, in
 * bytes, for e.g. setting the Baseline_ELD_Len field in the ELD header block.
 */
static inline hi_s32 hdmi_eld_calc_baseline_block_size(const hi_u8 *eld)
{
    return HDMI_ELD_MONITOR_NAME_STRING - HDMI_ELD_HEADER_BLOCK_SIZE +
           hdmi_eld_mnl(eld) + hdmi_eld_sad_count(eld) * 3;
}

/*
 * hdmi_eld_size - Get ELD size in bytes
 * @eld: pointer to a complete eld memory structure
 * The returned value does not include the vendor block. It's vendor specific,
 * and comprises of the remaining bytes in the ELD memory buffer after
 * hdmi_eld_size() bytes of header and baseline block.
 * The returned value is guaranteed to be a multiple of 4.
 */
static inline hi_s32 hdmi_eld_size(const hi_u8 *eld)
{
    return HDMI_ELD_HEADER_BLOCK_SIZE + eld[HDMI_ELD_BASELINE_ELD_LEN] * 4;
}

/* Short Audio Descriptor */
struct cea_sad {
    hi_u8 format;
    hi_u8 channels; /* max number of channels - 1 */
    hi_u8 freq;
    hi_u8 byte2; /* meaning depends on format */
};

/* Force 8bpc */
#define EDID_QUIRK_FORCE_8BPC (1 << 8)
/* Force 12bpc */
#define EDID_QUIRK_FORCE_12BPC (1 << 9)
/* Force 6bpc */
#define EDID_QUIRK_FORCE_6BPC (1 << 10)
/* Force 10bpc */
#define EDID_QUIRK_FORCE_10BPC (1 << 11)
/* Force 0 audio data during boot */
#define EDID_QUIRK_FORCE_BOOT_AUDIO (1 << 12)

hi_bool hdmi_edid_is_zero(const hi_u8 *in_edid, hi_s32 length);
void hdmi_bad_edid(hi_u8 *edid, hi_s32 num_blocks);
hi_bool hdmi_edid_block_valid(hi_u8 *raw_edid, hi_s32 block, hi_bool print_bad_edid,
                              hi_bool *edid_corrupt);

/*
 * edid_parse_property - parse edid to property
 * @connector: connector we're probing
 * Return: 0 for success or -1 for fail.
 */
/* hi_s32 edid_parse_property(hdmi_connector *connector); */
hi_s32 hdmi_add_edid_modes(struct hdmi_connector *connector, struct edid *edid);

#endif
