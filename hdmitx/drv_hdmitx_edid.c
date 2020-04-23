/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver edid module main source file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#include <linux/hisilicon/securec.h>
#include "drv_hdmitx_modes.h"
#include "drv_hdmitx.h"
#include "drv_hdmitx_edid.h"
#include "drv_hdmitx_connector.h"

typedef void detailed_cb(struct detailed_timing *timing, void *closure);

#define version_greater(edid, maj, min) \
    (((edid)->version > (maj)) ||  \
        ((edid)->version == (maj) && (edid)->revision > (min)))

#define for_each_cea_db(cea, i, start, end) \
    for ((i) = (start); (i) < (end) && (i) + cea_db_payload_len(&(cea)[(i)]) < (end); \
        (i) += cea_db_payload_len(&(cea)[(i)]) + 1)

#define EDID_EST_TIMINGS      16
#define EDID_STD_TIMINGS      8
#define EDID_DETAILED_TIMINGS 4

#define EDID_VEND_YEAR_BASE       1990
#define EDID_DEFAULT_DVI_MAX_TMDS 225000

/*
 * EDID blocks out in the wild have a variety of bugs, try to collect
 * them here (note that userspace may work around broken monitors first,
 * but fixes should make their way here so that the kernel "just works"
 * on as many displays as possible).
 */
/* First detailed mode wrong, use largest 60Hz mode */
#define EDID_QUIRK_PREFER_LARGE_60 (1 << 0)
/* Reported 135MHz pixel clock is too high, needs adjustment */
#define EDID_QUIRK_135_CLOCK_TOO_HIGH (1 << 1)
/* Prefer the largest mode at 75 Hz */
#define EDID_QUIRK_PREFER_LARGE_75 (1 << 2)
/* Detail timing is in cm not mm */
#define EDID_QUIRK_DETAILED_IN_CM (1 << 3)
/* Detailed timing descriptors have bogus size values, so just take the
 * maximum size and use that.
 */
#define EDID_QUIRK_DETAILED_USE_MAXIMUM_SIZE (1 << 4)
/* use +hsync +vsync for detailed mode */
#define EDID_QUIRK_DETAILED_SYNC_PP (1 << 6)
/* Force reduced-blanking timings for detailed modes */
#define EDID_QUIRK_FORCE_REDUCED_BLANKING (1 << 7)
/* Force 8bpc */
#define EDID_QUIRK_FORCE_8BPC (1 << 8)
/* Force 12bpc */
#define EDID_QUIRK_FORCE_12BPC (1 << 9)
/* Force 6bpc */
#define EDID_QUIRK_FORCE_6BPC (1 << 10)
/* Force 10bpc */
#define EDID_QUIRK_FORCE_10BPC (1 << 11)
/* Non desktop display (i.e. HMD) */
#define EDID_QUIRK_NON_DESKTOP (1 << 12)

struct detailed_mode_closure {
    struct hdmi_connector *connector;
    struct edid *edid;
    hi_bool preferred;
    hi_u32 quirks;
    hi_s32 modes;
};

#define LEVEL_DMT  0
#define LEVEL_GTF  1
#define LEVEL_GTF2 2
#define LEVEL_CVT  3

struct stereo_mandatory_mode {
    hi_s32 width, height, vrefresh;
    hi_bool progressive;
    hi_u32 flags;
};

static const struct stereo_mandatory_mode g_stereo_mandatory_modes[] = {
    { 1920, 1080, 24, true,  HDMI_3D_BZ_TOP_AND_BOTTOM },
    { 1920, 1080, 24, true,  HDMI_3D_BZ_FRAME_PACKING },
    { 1920, 1080, 50, false, HDMI_3D_BZ_SIDE_BY_SIDE_HALF },
    { 1920, 1080, 60, false, HDMI_3D_BZ_SIDE_BY_SIDE_HALF },
    { 1280, 720,  50, true,  HDMI_3D_BZ_TOP_AND_BOTTOM },
    { 1280, 720,  50, true,  HDMI_3D_BZ_FRAME_PACKING },
    { 1280, 720,  60, true,  HDMI_3D_BZ_TOP_AND_BOTTOM },
    { 1280, 720,  60, true,  HDMI_3D_BZ_FRAME_PACKING }
};

/* DDC fetch and block validation */
static const hi_u8 g_edid_header[] = {
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00
};

static hi_s32 g_edid_fixup = 6;

/**
 * hdmi_edid_header_is_valid - sanity check the header of the base EDID block
 * @raw_edid: pointer to raw base EDID block
 *
 * Sanity check the header of the base EDID block.
 *
 * Return: 8 if the header is perfect, down to 0 if it's totally wrong.
 */
hi_s32 hdmi_edid_header_is_valid(const hi_u8 *raw_edid)
{
    hi_s32 i = 0;
    hi_s32 score = 0;

    for (i = 0; i < sizeof(g_edid_header); i++)
        if (raw_edid[i] == g_edid_header[i]) {
            score++;
        }

    return score;
}

static hi_s32 hdmi_edid_block_checksum(const hi_u8 *raw_edid)
{
    hi_s32 i;
    hi_u8 csum = 0;

    for (i = 0; i < EDID_LENGTH; i++) {
        csum += raw_edid[i];
    }

    return csum;
}

hi_bool hdmi_edid_is_zero(const hi_u8 *in_edid, hi_s32 length)
{
    hi_u32 i;
    for (i = 0; i < length; i++) {
        if (in_edid[i] != 0) {
            return HI_FALSE;
        }
    }

    return HI_TRUE;
}

static hi_void print_edid_raw_data(hi_u8 *raw_edid, hi_u32 length)
{
    hi_u32 i;

    for (i = 0; i < length; i++) {
        if (((i % 16) == 0) && (i != 0)) { /* If the value is greater than 16, the line breaks. */
            HI_PRINT("\n");
        }

        HI_PRINT("0x%x\t", raw_edid[i]);
    }
}

void hdmi_bad_edid(hi_u8 *edid, hi_s32 num_blocks)
{
    hi_s32 i;

    if (!edid) {
        return;
    }

    for (i = 0; i < num_blocks; i++) {
        hi_u8 *block = edid + i * EDID_LENGTH;
        hi_s8 prefix[20];

        if (hdmi_edid_is_zero(block, EDID_LENGTH)) {
            if (snprintf_s(prefix, sizeof(prefix), sizeof(prefix) - 1, "\t[%02x] ZERO ", i) == HI_FAILURE) {
                HDMI_ERR("memcpy_s err\n");
                return;
            }
        } else if (!hdmi_edid_block_valid(block, i, false, NULL)) {
            if (snprintf_s(prefix, sizeof(prefix), sizeof(prefix) - 1, "\t[%02x] BAD  ", i) == HI_FAILURE) {
                HDMI_ERR("memcpy_s err\n");
                return;
            }
        } else {
            if (snprintf_s(prefix, sizeof(prefix), sizeof(prefix) - 1, "\t[%02x] GOOD ", i) == HI_FAILURE) {
                HDMI_ERR("memcpy_s err\n");
                return;
            }
        }
    }
}

/*
 * hdmi_edid_block_valid - Sanity check the EDID block (base or extension)
 * @raw_edid: pointer to raw EDID block
 * @block: type of block to validate (0 for base, extension otherwise)
 * @print_bad_edid: if true, dump bad EDID blocks to the console
 * @edid_corrupt: if true, the header or checksum is invalid
 * Validate a base or extension EDID block and optionally dump bad blocks to
 * the console.
 * Return: True if the block is valid, false otherwise.
 */
hi_bool hdmi_edid_block_valid(hi_u8 *raw_edid, hi_s32 block, hi_bool print_bad_edid,
                              hi_bool *edid_corrupt)
{
    hi_u8 csum;
    hi_s32 score;
    struct edid *edid = HI_NULL;

    if (raw_edid == HI_NULL) {
        HDMI_ERR("ptr is null.\n");
        return HI_FALSE;
    }

    edid = (struct edid *)raw_edid;

    if (block == 0) {
        score = hdmi_edid_header_is_valid(raw_edid);
        if (score == 8) {
            if (edid_corrupt) {
                *edid_corrupt = false;
            }
        } else if (score >= g_edid_fixup) {
            /* Displayport Link CTS Core 1.2 rev1.1 test 4.2.2.6
             * The corrupt flag needs to be set here otherwise, the
             * fix-up code here will correct the problem, the
             * checksum is correct and the test fails
             */
            if (edid_corrupt) {
                *edid_corrupt = true;
            }
            HDMI_INFO("Fixing EDID header, your hardware may be failing\n");
            if (memcpy_s(raw_edid, sizeof(struct edid), g_edid_header, sizeof(g_edid_header))) {
                HDMI_ERR("memcpy_s err\n");
                return HI_FALSE;
            }
        } else {
            if (edid_corrupt) {
                *edid_corrupt = true;
            }
            goto bad;
        }
    }

    csum = hdmi_edid_block_checksum(raw_edid);
    if (csum) {
        if (print_bad_edid) {
            HDMI_ERR("EDID checksum is invalid, remainder is %d\n", csum);
        }

        if (edid_corrupt) {
            *edid_corrupt = true;
        }
    }
    /* per-block-type checks */
    switch (raw_edid[0]) {
        case 0: /* base */
            if (edid->version != 1) {
                HDMI_ERR("EDID has major version %d, instead of 1\n", edid->version);
                goto bad;
            }

            if (edid->revision > 4) {
                HDMI_INFO("EDID minor > 4, assuming backward compatibility\n");
            }
            break;

        default:
            break;
    }

    return true;

bad:
    if (print_bad_edid) {
        if (hdmi_edid_is_zero(raw_edid, EDID_LENGTH)) {
            HDMI_ERR("EDID block is all zeroes\n");
        } else {
            HDMI_ERR("Raw EDID:\n");
            print_edid_raw_data(raw_edid, EDID_LENGTH);
        }
    }
    return false;
}

/*
 * hdmi_edid_is_valid - sanity check EDID data
 * @edid: EDID data
 * Sanity-check an entire EDID record (including extensions)
 * Return: True if the EDID data is valid, false otherwise.
 */
hi_bool hdmi_edid_is_valid(struct edid *edid)
{
    hi_s32 i;
    hi_u8 *raw = (hi_u8 *)edid;

    if (edid == HI_NULL) {
        return false;
    }

    for (i = 0; i <= edid->extensions; i++)
        if (!hdmi_edid_block_valid(raw + i * EDID_LENGTH, i, true, NULL)) {
            return false;
        }

    return true;
}

static void cea_for_each_detailed_block(hi_u8 *ext, detailed_cb *cb, void *closure)
{
    hi_s32 i, n;
    hi_u8 d = ext[0x02];
    hi_u8 *det_base = ext + d;

    n = (127 - d) / 18;
    for (i = 0; i < n; i++) {
        cb ((struct detailed_timing *)(det_base + 18 * i), closure);
    }
}

static void vtb_for_each_detailed_block(hi_u8 *ext, detailed_cb *cb, void *closure)
{
    hi_u32 i;
    hi_u32 n = osal_min((hi_s32)ext[0x02], 6); /* The minimum value between extend block's 2th byte and 6. */
    hi_u8 *det_base = ext + 5;

    if (ext[0x01] != 1) {
        return; /* unknown version */
    }

    for (i = 0; i < n; i++) {
        cb ((struct detailed_timing *)(det_base + 18 * i), closure);
    }
}

static void hdmi_for_each_detailed_block(hi_u8 *raw_edid, detailed_cb *cb, void *closure)
{
    hi_s32 i;
    struct edid *edid = (struct edid *)raw_edid;

    if (edid == NULL) {
        return;
    }

    for (i = 0; i < EDID_DETAILED_TIMINGS; i++) {
        cb(&(edid->detailed_timings[i]), closure);
    }

    for (i = 1; i <= raw_edid[0x7e]; i++) {
        hi_u8 *ext = raw_edid + (i * EDID_LENGTH);

        switch (*ext) {
            case CEA_EXT:
                cea_for_each_detailed_block(ext, cb, closure);
                break;
            case VTB_EXT:
                vtb_for_each_detailed_block(ext, cb, closure);
                break;
            default:
                break;
        }
    }
}

static void monitor_name(struct detailed_timing *t, void *data)
{
    if (t->data.other_data.type == EDID_DETAIL_MONITOR_NAME) {
        *(hi_u8 **)data = t->data.other_data.data.str.str;
    }
}

static hi_s32 get_monitor_name(struct edid *edid, hi_s8 name[13], hi_u32 len)
{
    hi_s8 *edid_name = NULL;
    hi_s32 mnl;

    if (!edid || !name) {
        return 0;
    }

    hdmi_for_each_detailed_block((hi_u8 *)edid, monitor_name, &edid_name);
    for (mnl = 0; edid_name && mnl < len; mnl++) {
        if (edid_name[mnl] == 0x0a) {
            break;
        }

        name[mnl] = edid_name[mnl];
    }

    return mnl;
}

/**
 * hdmi_edid_get_monitor_name - fetch the monitor name from the edid
 * @edid: monitor EDID information
 * @name: pointer to a character array to hold the name of the monitor
 * @bufsize: The size of the name buffer (should be at least 14 chars.)
 *
 */
void hdmi_edid_get_monitor_name(struct edid *edid, hi_s8 *name, hi_s32 bufsize)
{
    hi_s32 name_length;
    hi_s8 buf[13];

    if (bufsize <= 0) {
        return;
    }

    name_length = osal_min(get_monitor_name(edid, buf, sizeof(buf)), bufsize - 1);
    if (memcpy_s(name, bufsize, buf, name_length)) {
        HDMI_ERR("memcpy_s err\n");
        return;
    }
    name[name_length] = '\0';
}

static void clear_eld(struct hdmi_connector *connector)
{
    if (memset_s(connector->eld, sizeof(connector->eld), 0, sizeof(connector->eld))) {
        HDMI_ERR("memset_s err\n");
    }
}

/*
 * 0 is reserved.  The spec says 0x01 fill for unused timings.	Some old
 * monitors fill with ascii space (0x20) instead.
 */
static hi_s32 bad_std_timing(hi_u8 a, hi_u8 b)
{
    return (a == 0x00 && b == 0x00) ||
           (a == 0x01 && b == 0x01) ||
           (a == 0x20 && b == 0x20);
}

/**
 * hdmi_mode_std - convert standard mode info (width, height, refresh) into mode
 * @connector: connector of for the EDID block
 * @edid: EDID block to scan
 * @t: standard timing params
 *
 * Take the standard timing params (in this case width, aspect, and refresh)
 * and convert them into a real mode using CVT/GTF/DMT.
 */
static struct hdmi_display_mode *hdmi_mode_std(struct hdmi_connector *connector, struct edid *edid,
                                               struct std_timing *t)
{
    struct hisilicon_hdmi *hdmi_dev = connector->hdmi_dev;
    struct hdmi_display_mode *m = NULL;
    struct hdmi_display_mode *mode = NULL;
    hi_s32 hsize, vsize;
    hi_s32 vrefresh_rate;
    hi_u32 aspect_ratio = (t->vfreq_aspect & EDID_TIMING_ASPECT_MASK) >> EDID_TIMING_ASPECT_SHIFT;
    hi_u32 vfreq = (t->vfreq_aspect & EDID_TIMING_VFREQ_MASK) >> EDID_TIMING_VFREQ_SHIFT;

    if (bad_std_timing(t->hsize, t->vfreq_aspect)) {
        return NULL;
    }

    /* According to the EDID spec, the hdisplay = hsize * 8 + 248 */
    hsize = t->hsize * 8 + 248;
    vrefresh_rate = vfreq + 60;
    /* the vdisplay is calculated based on the aspect ratio */
    if (aspect_ratio == 0) {
        if (edid->revision < 3) {
            vsize = hsize;
        } else {
            vsize = (hsize * 10) / 16;
        }
    } else if (aspect_ratio == 1) {
        vsize = (hsize * 3) / 4;
    } else if (aspect_ratio == 2) {
        vsize = (hsize * 4) / 5;
    } else {
        vsize = (hsize * 9) / 16;
    }

    /* HDTV hack, part 1 */
    if (vrefresh_rate == 60 &&
        ((hsize == 1360 && vsize == 765) ||
         (hsize == 1368 && vsize == 769))) {
        hsize = 1366;
        vsize = 768;
    }

    /*
     * If this connector already has a mode for this size and refresh
     * rate (because it came from detailed or CVT info), use that
     * instead.  This way we don't have to guess at interlace or
     * reduced blanking.
     */
    osal_list_for_each_entry(m, &connector->probed_modes, head)
        if (m->detail.h_active == hsize &&
            m->detail.v_active == vsize &&
            hdmi_mode_vrefresh(m) == vrefresh_rate) {
            return NULL;
        }

    mode = hdmi_mode_create_std_timing(hdmi_dev, hsize, vsize,
                                       vrefresh_rate);
    return mode;
}

/**
 * hdmi_mode_detailed - create a new mode from an EDID detailed timing section
 * @hdmi_hdmi_hdmi_dev: HDMI hdmi_hdmi_hdmi_device (needed to create new mode)
 * @edid: EDID block
 * @timing: EDID detailed timing info
 * @quirks: quirks to apply
 *
 * An EDID detailed timing block contains enough info for us to create and
 * return a new struct hdmi_display_mode.
 */
static struct hdmi_display_mode *hdmi_mode_detailed(struct hisilicon_hdmi *hdmi_dev,
                                                    struct edid *edid, struct detailed_timing *timing, hi_u32 quirks)
{
    struct hdmi_display_mode *mode = NULL;
    struct hdmi_detail *detail = NULL;
    struct detailed_pixel_timing *pt = &timing->data.pixel_data;
    hi_u32 hactive = ((pt->hactive_hblank_hi & 0xf0) << 4) | pt->hactive_lo; /* need left shift 4bits. */
    hi_u32 vactive = ((pt->vactive_vblank_hi & 0xf0) << 4) | pt->vactive_lo; /* need left shift 4bits. */
    hi_u32 hblank = ((pt->hactive_hblank_hi & 0xf) << 8) | pt->hblank_lo; /* need left shift 8bits. */
    hi_u32 vblank = ((pt->vactive_vblank_hi & 0xf) << 8) | pt->vblank_lo; /* need left shift 8bits. */
    hi_u32 hsync_offset = ((pt->hsync_vsync_offset_pulse_width_hi & 0xc0)
                           << 2) |
                          pt->hsync_offset_lo;
    hi_u32 hsync_pulse_width = ((pt->hsync_vsync_offset_pulse_width_hi & 0x30)
                                << 4) |  /* need left shift 4bits. */
                               pt->hsync_pulse_width_lo;
    hi_u32 vsync_offset = ((pt->hsync_vsync_offset_pulse_width_hi & 0xc)
                           << 2) |  /* need left shift 2bits. */
                          (pt->vsync_offset_pulse_width_lo >> 4);
    hi_u32 vsync_pulse_width = ((pt->hsync_vsync_offset_pulse_width_hi & 0x3)
                                << 4) |  /* need left shift 4bits. */
                               (pt->vsync_offset_pulse_width_lo & 0xf);
    hi_u32 pixel_clk;

    if (!hdmi_dev || !hdmi_dev->connector) {
        HDMI_ERR("null!\n");
        return NULL;
    }

    /* ignore tiny modes */
    if (hactive < 64 || vactive < 64) {
        return NULL;
    }

    if (pt->misc & HDMI_EDID_PT_STEREO) {
        HDMI_DBG("stereo mode not supported\n");
        return NULL;
    }
    if (!(pt->misc & HDMI_EDID_PT_SEPARATE_SYNC)) {
        HDMI_DBG("composite sync not supported\n");
    }

    /* it is incorrect if hsync/vsync width is zero */
    if (!hsync_pulse_width || !vsync_pulse_width) {
        HDMI_DBG("Incorrect Detailed timing. Wrong Hsync/Vsync pulse width\n");
        goto err_timing;
    }

    /* add by tangqy */
    if (!hactive || !vactive) {
        HDMI_DBG("Incorrect Detailed timing. Wrong hactive/vactive\n");
        goto err_timing;
    }

    /* add by tangqy */
    if (hsync_offset < pt->hborder ||
        vsync_offset < pt->vborder) {
        HDMI_DBG("Incorrect Detailed timing. Wrong sync_offset/border width\n");
        goto err_timing;
    }

    if (quirks & EDID_QUIRK_135_CLOCK_TOO_HIGH) {
        timing->pixel_clock = cpu_to_le16(1088);
    }

    pixel_clk = le16_to_cpu(timing->pixel_clock) * 10;

    mode = hdmi_mode_create_by_detailed_timing(pixel_clk, hactive, vactive, hblank, vblank);
    if (mode) {
        return mode;
    }

    mode = hdmi_mode_create(hdmi_dev);
    if (!mode) {
        return NULL;
    }

    detail = &mode->detail;

    mode->vic = ++hdmi_dev->connector->detail_vic_base;

    detail->pixel_clock = pixel_clk;

    detail->h_active = hactive;
    detail->h_total = hactive + hblank;
    detail->h_blank = hblank;
    detail->h_front = hsync_offset - pt->hborder;
    detail->h_sync = hsync_pulse_width;
    detail->h_back = hblank - detail->h_sync - detail->h_front;

    detail->v_active = vactive;
    detail->v_total = vactive + vblank;
    detail->v_blank = vblank;
    detail->v_front = vsync_offset - pt->vborder;
    detail->v_sync = vsync_pulse_width;
    detail->v_back = vblank - detail->v_sync - detail->v_front;

    detail->h_pol = (pt->misc & HDMI_EDID_PT_HSYNC_POSITIVE) ? true : false;
    detail->v_pol = (pt->misc & HDMI_EDID_PT_VSYNC_POSITIVE) ? true : false;

    mode->flags |= (pt->misc & HDMI_EDID_PT_HSYNC_POSITIVE) ? HDMI_MODE_FLAG_PHSYNC : HDMI_MODE_FLAG_NHSYNC;
    mode->flags |= (pt->misc & HDMI_EDID_PT_VSYNC_POSITIVE) ? HDMI_MODE_FLAG_PVSYNC : HDMI_MODE_FLAG_NVSYNC;

    return mode;

err_timing:
    return NULL;
}
/*
 * add_established_modes - get est. modes from EDID and add them
 * @connector: connector to add mode(s) to
 * @edid: EDID block to scan
 * Each EDID block contains a bitmap of the supported "established modes" list
 * (defined above).  Tease them out and add them to the global modes list.
 */
static hi_s32 add_established_modes(struct hdmi_connector *connector, struct edid *edid)
{
    struct hisilicon_hdmi *hdmi_dev = connector->hdmi_dev;
    hi_u32 est_bits = edid->established_timings.t1 |
                      (edid->established_timings.t2 << 8) |
                      ((edid->established_timings.mfg_rsvd & 0x80) << 9);
    hi_s32 i;
    hi_s32 modes = 0;
    /*
     *   struct detailed_mode_closure closure = {
     *   .connector = connector,
     *   .edid = edid,
     *   };
     */
    for (i = 0; i <= EDID_EST_TIMINGS; i++) {
        if (est_bits & (1 << i)) {
            struct hdmi_display_mode *newmode;

            newmode = hdmi_mode_create_from_vic(hdmi_dev,
                                                VIC_VESA_800X600P60 + i);
            if (newmode) {
                newmode->parse_type |= MODE_TYPE_ESTABLISHED_TIMINGE;
                hdmi_mode_probed_add(connector, newmode);
                modes++;
            }
        }
    }

    return modes;
}

static void do_standard_modes(struct detailed_timing *timing, void *c)
{
    struct detailed_mode_closure *closure = c;
    struct detailed_non_pixel *data = &timing->data.other_data;
    struct hdmi_connector *connector = closure->connector;
    struct edid *edid = closure->edid;

    if (data->type == EDID_DETAIL_STD_MODES) {
        hi_s32 i;

        for (i = 0; i < 6; i++) {
            struct std_timing *std = HI_NULL;
            struct hdmi_display_mode *newmode = HI_NULL;

            std = &data->data.timings[i];
            newmode = hdmi_mode_std(connector, edid, std);
            if (newmode) {
                newmode->parse_type |= MODE_TYPE_STD_TIMINGE;
                hdmi_mode_probed_add(connector, newmode);
                closure->modes++;
            }
        }
    }
}

/**
 * add_standard_modes - get std. modes from EDID and add them
 * @connector: connector to add mode(s) to
 * @edid: EDID block to scan
 *
 * Standard modes can be calculated using the appropriate standard (DMT,
 * GTF or CVT. Grab them from @edid and add them to the list.
 */
static hi_s32 add_standard_modes(struct hdmi_connector *connector, struct edid *edid)
{
    hi_s32 i;
    hi_s32 modes = 0;

    struct detailed_mode_closure closure = {
        .connector = connector,
        .edid = edid,
    };

    for (i = 0; i < EDID_STD_TIMINGS; i++) {
        struct hdmi_display_mode *newmode;

        newmode = hdmi_mode_std(connector, edid,
                                &edid->standard_timings[i]);
        if (newmode) {
            newmode->parse_type |= MODE_TYPE_STD_TIMINGE;
            hdmi_mode_probed_add(connector, newmode);
            modes++;
        }
    }

    if (version_greater(edid, 1, 0))
        hdmi_for_each_detailed_block((hi_u8 *)edid, do_standard_modes,
                                     &closure);

    /* XXX should also look for standard codes in VTB blocks */
    return modes + closure.modes;
}

static void do_detailed_mode(struct detailed_timing *timing, void *c)
{
    struct detailed_mode_closure *closure = c;
    struct hdmi_display_mode *newmode = NULL;

    if (timing->pixel_clock) {
        if (closure && closure->connector &&
            closure->connector->hdmi_dev)
            newmode = hdmi_mode_detailed(closure->connector->hdmi_dev,
                                         closure->edid, timing,
                                         closure->quirks);
        if (!newmode) {
            return;
        }

        if (closure->preferred) {
            newmode->type |= HDMI_MODE_TYPE_PREFERRED;
        }

        /*
         * Detailed modes are limited to 10kHz pixel clock resolution,
         * so fix up anything that looks like CEA/HDMI mode, but the clock
         * is just slightly off.
         */
        newmode->parse_type |= MODE_TYPE_DETAILED_TIMINGE;
        hdmi_mode_probed_add(closure->connector, newmode);
        closure->modes++;
        closure->preferred = false;
    }
}

/*
 * add_detailed_modes - Add modes from detailed timings
 * @connector: attached connector
 * @edid: EDID block to scan
 * @quirks: quirks to apply
 */
static hi_s32 add_detailed_modes(struct hdmi_connector *connector, struct edid *edid,
                                 hi_u32 quirks)
{
    struct detailed_mode_closure closure = {
        .connector = connector,
        .edid = edid,
        .preferred = true,
        .quirks = quirks,
    };

    if (closure.preferred && !version_greater(edid, 1, 3))
        closure.preferred =
            (edid->features & HDMI_EDID_FEATURE_PREFERRED_TIMING);

    hdmi_for_each_detailed_block((hi_u8 *)edid, do_detailed_mode, &closure);

    return closure.modes;
}

#define AUDIO_BLOCK                  0x01
#define VIDEO_BLOCK                  0x02
#define VENDOR_BLOCK                 0x03
#define SPEAKER_BLOCK                0x04
#define USE_EXTENDED_TAG             0x07
#define EXT_VIDEO_CAPABILITY_BLOCK   0x00
#define EXT_COLORIMETRY_BLOCK        0x05
#define EXT_HDR_STATIC_BLOCK         0x06
#define EXT_HDR_DYNAMIC_BLOCK        0x07
#define EXT_VIDEO_DATA_BLOCK_420     0x0E
#define EXT_VIDEO_CAP_BLOCK_Y420CMDB 0x0F
#define EDID_BASIC_AUDIO             (1 << 6)
#define EDID_CEA_YCRCB444            (1 << 5)
#define EDID_CEA_YCRCB422            (1 << 4)
#define EDID_CEA_VCDB_QS             (1 << 6)
#define EDID_CEA_VCDB_QY             (1 << 7)
/*
 * Search EDID for CEA extension block.
 */
static hi_u8 *hdmi_find_edid_extension(struct edid *edid, hi_s32 ext_id)
{
    hi_u8 *edid_ext = NULL;
    hi_s32 i;

    /* No EDID or EDID extensions */
    if (edid == NULL || edid->extensions == 0) {
        return NULL;
    }

    /* Find CEA extension */
    for (i = 0; i < edid->extensions; i++) {
        edid_ext = (hi_u8 *)edid + EDID_LENGTH * (i + 1);
        if (edid_ext[0] == ext_id) {
            break;
        }
    }

    if (i == edid->extensions) {
        return NULL;
    }

    return edid_ext;
}
static hi_u8 *hdmi_find_cea_extension(struct edid *edid)
{
    return hdmi_find_edid_extension(edid, CEA_EXT);
}

static hi_u8 *hdmi_find_displayid_extension(struct edid *edid)
{
    return hdmi_find_edid_extension(edid, DISPLAYID_EXT);
}

static hi_bool hdmi_valid_cea_vic(hi_u8 vic)
{
    /* 128-192  Forbidden ;220-255  Reserved for the Future by CTA-861-G */
    return (vic > 0 && vic < 128) || (vic > 192 && vic < 220);
}
static hi_bool hdmi_valid_hdmi_vic(hi_u8 hdmi_vic)
{
    return hdmi_vic > 0 && hdmi_vic < 5;
}
static hi_u8 svd_to_vic(hi_u8 svd)
{
    /* 0-6 bit vic, 7th bit native mode indicator */
    if ((svd >= 1 && svd <= 64) || (svd >= 129 && svd <= 192)) {
        return svd & 127;
    }

    return svd;
}

static struct hdmi_display_mode *hdmi_display_mode_from_vic_index(struct hdmi_connector *connector,
                                                                  const hi_u8 *video_db,
                                                                  hi_u8 video_len, hi_u8 video_index)
{
    struct hisilicon_hdmi *hdmi_dev = connector->hdmi_dev;
    struct hdmi_display_mode *newmode = HI_NULL;
    hi_bool native_mode = HI_FALSE;
    hi_u8 vic, svd;

    if (video_db == NULL || video_index >= video_len) {
        return HI_NULL;
    }

    /* CEA modes are numbered 1..127 or 192..219 */
    svd = video_db[video_index];
    if (svd >= 129 && svd <= 192) { /* svd is 129~192, is native mode. */
        native_mode = HI_TRUE;
    }

    vic = svd_to_vic(svd);
    if (!hdmi_valid_cea_vic(vic)) {
        return HI_NULL;
    }

    newmode = hdmi_mode_create_from_vic(hdmi_dev, vic);
    if (!newmode) {
        return HI_NULL;
    }

    if (native_mode) {
        newmode->native_mode = HI_FALSE;
    }

    return newmode;
}

/*
 * do_y420vdb_modes - Parse YCBCR 420 only modes
 * @connector: connector corresponding to the HDMI sink
 * @svds: start of the data block of CEA YCBCR 420 VDB
 * @len: length of the CEA YCBCR 420 VDB
 *
 * Parse the CEA-861-F YCBCR 420 Video Data Block (Y420VDB)
 * which contains modes which can be supported in YCBCR 420
 * output format only.
 */
static hi_s32 do_y420vdb_modes(struct hdmi_connector *connector,
                               const hi_u8 *svds, hi_u8 svds_len)
{
    hi_s32 modes = 0;
    hi_s32 i;
    struct hisilicon_hdmi *hdmi_dev = connector->hdmi_dev;
    struct color_property *color = &connector->color;

    for (i = 0; i < svds_len; i++) {
        hi_u8 vic = svd_to_vic(svds[i]);
        struct hdmi_display_mode *newmode = HI_NULL;

        if (!hdmi_valid_cea_vic(vic)) {
            continue;
        }
        newmode = hdmi_mode_create_from_vic(hdmi_dev, vic);
        if (!newmode) {
            break;
        }

        newmode->parse_type |= MODE_TYPE_Y420VDB;
        hdmi_mode_probed_add(connector, newmode);
        modes++;
    }

    if (modes > 0) {
        color->format.ycbcr420 = true;
        color->depth.y420_24 = true;
    }
    return modes;
}
/*
 * hdmi_add_cmdb_modes - Add a YCBCR 420 mode into bitmap
 * @connector: connector corresponding to the HDMI sink
 * @vic: CEA vic for the video mode to be added in the map
 *
 * Makes an entry for a videomode in the YCBCR 420 bitmap
 */
static void hdmi_add_cmdb_modes(struct hdmi_connector *connector, hi_u8 svd)
{
    hi_u8 vic = svd_to_vic(svd);
    if (!hdmi_valid_cea_vic(vic)) {
        return;
    }
}

static hi_s32 do_cea_modes(struct hdmi_connector *connector, const hi_u8 *db, hi_u8 len)
{
    hi_s32 i;
    hi_s32 modes = 0;
    struct hdmi_hdmi_info *hdmi = NULL;
    struct hdmi_display_mode *mode = NULL;

    if (connector == NULL || db == NULL) {
        HDMI_ERR("Input params is NULL pointer!\n");
        return 0;
    }

    hdmi = &connector->display_info.hdmi;

    for (i = 0; i < len; i++) {
        mode = hdmi_display_mode_from_vic_index(connector, db, len, i);
        if (mode) {
            mode->parse_type |= MODE_TYPE_VDB;
            /*
             * YCBCR420 capability block contains a bitmap which
             * gives the index of CEA modes from CEA VDB, which
             * can support YCBCR 420 sampling output also (apart
             * from RGB/YCBCR444 etc).
             * For example, if the bit 0 in bitmap is set,
             * first mode in VDB can support YCBCR420 output too.
             * Add YCBCR420 modes only if sink is HDMI 2.0 capable.
             */
            if ((i < 64) && (hdmi->y420_cmdb_map & (1ULL << i))) { /* max size is 64. */
                hdmi_add_cmdb_modes(connector, db[i]);
                mode->parse_type |= MODE_TYPE_VDB_Y420CMDB;
            }

            /*
             * Mark the first mode in the video data block of EDID.
             */
            if (i == 0) {
                mode->first_mode = true;
            }

            hdmi_mode_probed_add(connector, mode);
            modes++;
        }
    }

    return modes;
}

static hi_bool stereo_match_mandatory(struct hdmi_display_mode *mode,
                                      const struct stereo_mandatory_mode *stereo_mode)
{
    return mode->detail.h_active == stereo_mode->width &&
           mode->detail.v_active == stereo_mode->height &&
           mode->detail.progressive == stereo_mode->progressive &&
           hdmi_mode_vrefresh(mode) == stereo_mode->vrefresh;
}

static hi_s32 add_hdmi_mandatory_stereo_modes(struct hdmi_connector *connector)
{
    struct hdmi_display_mode *mode = HI_NULL;
    hi_s32 modes = 0;
    hi_s32 i;

    osal_list_for_each_entry(mode, &connector->probed_modes, head)
    {
        for (i = 0; i < ARRAY_SIZE(g_stereo_mandatory_modes); i++) {
            const struct stereo_mandatory_mode *mandatory = HI_NULL;

            if (!stereo_match_mandatory(mode, &g_stereo_mandatory_modes[i])) {
                continue;
            }

            mandatory = &g_stereo_mandatory_modes[i];
            mode->detail.mode_3d |= mandatory->flags;
        }
    }

    return modes;
}

static hi_s32 add_hdmi_mode(struct hdmi_connector *connector, hi_u8 hdmi_vic)
{
    struct hisilicon_hdmi *hdmi_dev = connector->hdmi_dev;
    struct hdmi_display_mode *newmode = NULL;
    hi_u32 vic;

    if (!hdmi_valid_hdmi_vic(hdmi_vic)) {
        HDMI_ERR("Unknown HDMI VIC: %d\n", hdmi_vic);
        return 0;
    }

    /* hdmi_vic to vic,according to HDMI2.1 spec Table 10-2 */
    switch (hdmi_vic) {
        case 1:
            vic = VIC_3840X2160P30_16_9;
            break;
        case 2:
            vic = VIC_3840X2160P25_16_9;
            break;
        case 3:
            vic = VIC_3840X2160P24_16_9;
            break;
        case 4:
            vic = VIC_4096X2160P24_256_135;
            break;
        default:
            return 0;
    }

    newmode = hdmi_mode_create_from_vic(hdmi_dev, vic);
    if (!newmode) {
        return 0;
    }

    newmode->parse_type |= MODE_TYPE_VSDB_4K;
    hdmi_mode_probed_add(connector, newmode);

    return 1;
}

static hi_s32 add_3d_struct_modes(struct hdmi_connector *connector, hi_u16 structure,
                                  const hi_u8 *video_db, hi_u8 video_len, hi_u8 video_index)
{
    struct hdmi_display_mode *newmode = NULL;
    hi_s32 modes = 0;

    if (structure & (1 << 0)) {
        newmode = hdmi_display_mode_from_vic_index(connector, video_db,
                                                   video_len,
                                                   video_index);
        if (newmode) {
            newmode->detail.mode_3d |= HDMI_3D_BZ_FRAME_PACKING;
            hdmi_mode_probed_add(connector, newmode);
            modes++;
        }
    }
    if (structure & (1 << 6)) {
        newmode = hdmi_display_mode_from_vic_index(connector, video_db,
                                                   video_len,
                                                   video_index);
        if (newmode) {
            newmode->detail.mode_3d |= HDMI_3D_BZ_TOP_AND_BOTTOM;
            hdmi_mode_probed_add(connector, newmode);
            modes++;
        }
    }
    if (structure & (1 << 8)) {
        newmode = hdmi_display_mode_from_vic_index(connector, video_db,
                                                   video_len,
                                                   video_index);
        if (newmode) {
            newmode->detail.mode_3d |= HDMI_3D_BZ_SIDE_BY_SIDE_HALF;
            newmode->parse_type |= MODE_TYPE_VSDB_3D;
            hdmi_mode_probed_add(connector, newmode);
            modes++;
        }
    }

    return modes;
}

/*
 * do_hdmi_vsdb_modes - Parse the HDMI Vendor Specific data block
 * @connector: connector corresponding to the HDMI sink
 * @db: start of the CEA vendor specific block
 * @len: length of the CEA block payload, ie. one can access up to db[len]
 *
 * Parses the HDMI VSDB looking for modes to add to @connector. This function
 * also adds the stereo 3d modes when applicable.
 */
static hi_s32 do_hdmi_vsdb_modes(struct hdmi_connector *connector, const hi_u8 *db, hi_u8 len,
                                 const hi_u8 *video_db, hi_u8 video_len)
{
    struct hdmi_display_info *info = &connector->display_info;
    hi_s32 modes = 0;
    hi_s32 offset = 0;
    hi_s32 i;
    hi_s32 multi_present = 0;
    hi_s32 multi_len;
    hi_u8 vic_len, hdmi_3d_len;
    hi_u16 mask;
    hi_u16 structure_all;

    if (len < 8) {
        goto out;
    }

    /* no HDMI_Video_Present */
    if (!(db[8] & (1 << 5))) {
        goto out;
    }

    /* Latency_Fields_Present */
    if (db[8] & (1 << 7)) {
        offset += 2;
    }

    /* I_Latency_Fields_Present */
    if (db[8] & (1 << 6)) {
        offset += 2;
    }

    /*
     * The declared length is not long enough for the 2 first bytes
     * of additional video format capabilities
     */
    if (len < (8 + offset + 2)) {
        goto out;
    }

    /* 3D_Present */
    offset++;
    if (db[8 + offset] & (1 << 7)) {
        modes += add_hdmi_mandatory_stereo_modes(connector);

        /* 3D_Multi_present */
        multi_present = (db[8 + offset] & 0x60) >> 5;
    }

    offset++;
    vic_len = db[8 + offset] >> 5;
    hdmi_3d_len = db[8 + offset] & 0x1f;

    for (i = 0; i < vic_len && len >= (9 + offset + i); i++) {
        hi_u8 vic;

        vic = db[9 + offset + i];
        modes += add_hdmi_mode(connector, vic);
    }
    offset += 1 + vic_len;

    if (multi_present == 1) {
        multi_len = 2;
    } else if (multi_present == 2) { /* present is 2, length is 4. */
        multi_len = 4;
    } else {
        multi_len = 0;
    }

    if (len < (8 + offset + hdmi_3d_len - 1)) {
        goto out;
    }

    if (hdmi_3d_len < multi_len) {
        goto out;
    }

    if (multi_present == 1 || multi_present == 2) {
        /* 3D_Structure_ALL */
        structure_all = (db[8 + offset] << 8) | db[9 + offset];

        /* check if 3D_MASK is present */
        if (multi_present == 2) {
            mask = (db[10 + offset] << 8) | db[11 + offset];
        } else {
            mask = 0xffff;
        }

        for (i = 0; i < 16; i++)
            if (mask & (1 << i))
                modes += add_3d_struct_modes(connector,
                                             structure_all,
                                             video_db,
                                             video_len, i);
    }

    offset += multi_len;

    for (i = 0; i < (hdmi_3d_len - multi_len); i++) {
        hi_s32 vic_index;
        struct hdmi_display_mode *newmode = NULL;
        hi_u32 newflag = 0;
        hi_bool detail_present;

        detail_present = ((db[8 + offset + i] & 0x0f) > 7);

        if (detail_present && (i + 1 == hdmi_3d_len - multi_len)) {
            break;
        }

        /* 2D_VIC_order_X */
        vic_index = db[8 + offset + i] >> 4;

        /* 3D_Structure_X */
        switch (db[8 + offset + i] & 0x0f) {
            case 0:
                newflag = HDMI_3D_BZ_FRAME_PACKING;
                break;
            case 6:
                newflag = HDMI_3D_BZ_TOP_AND_BOTTOM;
                break;
            case 8:
                /* 3D_Detail_X */
                if ((db[9 + offset + i] >> 4) == 1) {
                    newflag = HDMI_3D_BZ_SIDE_BY_SIDE_HALF;
                }
                break;
        }

        if (newflag != 0) {
            newmode = hdmi_display_mode_from_vic_index(connector, video_db, video_len, vic_index);
            if (newmode) {
                newmode->detail.mode_3d |= newflag;
                newmode->parse_type |= MODE_TYPE_VSDB_3D;
                hdmi_mode_probed_add(connector, newmode);
                modes++;
            }
        }

        if (detail_present) {
            i++;
        }
    }

out:
    if (modes > 0) {
        info->has_hdmi_infoframe = true;
    }

    return modes;
}

static hi_s32 cea_db_payload_len(const hi_u8 *db)
{
    return db[0] & 0x1f;
}

static hi_s32 cea_db_extended_tag(const hi_u8 *db)
{
    return db[1];
}

static hi_s32 cea_db_tag(const hi_u8 *db)
{
    return db[0] >> 5;
}

static hi_s32 cea_revision(const hi_u8 *cea)
{
    return cea[1];
}

static hi_s32 cea_db_offsets(const hi_u8 *cea, hi_s32 *start, hi_s32 *end)
{
    /* Data block offset in CEA extension block */
    *start = 4;
    *end = cea[2];
    if (*end == 0) {
        *end = 127;
    }

    if (*end < 4 || *end > 127) {
        return -ERANGE;
    }

    return 0;
}

static hi_bool cea_db_is_hdmi_vsdb(const hi_u8 *db)
{
    hi_s32 hdmi_id;

    if (cea_db_tag(db) != VENDOR_BLOCK) {
        return false;
    }

    if (cea_db_payload_len(db) < 5) {
        return false;
    }

    hdmi_id = db[1] | (db[2] << 8) | (db[3] << 16);

    return hdmi_id == HDMI_IEEE_OUI;
}

static hi_bool cea_db_is_hdmi_forum_vsdb(const hi_u8 *db)
{
    hi_u32 oui;

    if (cea_db_tag(db) != VENDOR_BLOCK) {
        return false;
    }

    if (cea_db_payload_len(db) < 7) {
        return false;
    }

    oui = (db[3] << 16) | (db[2] << 8) | db[1]; /* oui is db[3] << 16 | db[2] << 8 | db[1]. */

    return oui == HDMI_FORUM_IEEE_OUI;
}

static hi_bool cea_db_is_dolbyvision(const hi_u8 *db)
{
    hi_u32 oui;

    if (cea_db_tag(db) != VENDOR_BLOCK) {
        return false;
    }

    if (cea_db_payload_len(db) < 0xb) {
        return false;
    }

    oui = (db[3] << 16) | (db[2] << 8) | db[1]; /* oui is db[3] << 16 | db[2] << 8 | db[1]. */

    return oui == HDMI_DOLBY_IEEE_OUI;
}

static hi_bool cea_db_is_vcdb(const hi_u8 *db)
{
    if (cea_db_tag(db) != USE_EXTENDED_TAG) {
        return false;
    }

    if (cea_db_payload_len(db) != 2) {
        return false;
    }

    if (cea_db_extended_tag(db) != EXT_VIDEO_CAPABILITY_BLOCK) {
        return false;
    }

    return true;
}

static hi_bool cea_db_is_colorimetry_db(const hi_u8 *db)
{
    if (cea_db_tag(db) != USE_EXTENDED_TAG) {
        return false;
    }

    if (cea_db_payload_len(db) != 3) {
        return false;
    }

    if (cea_db_extended_tag(db) != EXT_COLORIMETRY_BLOCK) {
        return false;
    }

    return true;
}

static hi_bool cea_db_is_hdr_static_db(const hi_u8 *db)
{
    if (cea_db_tag(db) != USE_EXTENDED_TAG) {
        return false;
    }

    if (cea_db_payload_len(db) < 2) {
        return false;
    }

    if (cea_db_extended_tag(db) != EXT_HDR_STATIC_BLOCK) {
        return false;
    }

    return true;
}

static hi_bool cea_db_is_hdr_dynamic_db(const hi_u8 *db)
{
    if (cea_db_tag(db) != USE_EXTENDED_TAG) {
        return false;
    }

    if (cea_db_payload_len(db) < 2) {
        return false;
    }

    if (cea_db_extended_tag(db) != EXT_HDR_DYNAMIC_BLOCK) {
        return false;
    }

    return true;
}

static hi_bool cea_db_is_y420cmdb(const hi_u8 *db)
{
    if (cea_db_tag(db) != USE_EXTENDED_TAG) {
        return false;
    }

    if (!cea_db_payload_len(db)) {
        return false;
    }

    if (cea_db_extended_tag(db) != EXT_VIDEO_CAP_BLOCK_Y420CMDB) {
        return false;
    }

    return true;
}

static hi_bool cea_db_is_y420vdb(const hi_u8 *db)
{
    if (cea_db_tag(db) != USE_EXTENDED_TAG) {
        return false;
    }

    if (!cea_db_payload_len(db)) {
        return false;
    }

    if (cea_db_extended_tag(db) != EXT_VIDEO_DATA_BLOCK_420) {
        return false;
    }

    return true;
}

static void hdmi_parse_y420cmdb_bitmap(struct hdmi_connector *connector,
                                       const hi_u8 *db)
{
    struct hdmi_display_info *info = NULL;
    struct color_property *color = NULL;
    struct hdmi_hdmi_info *hdmi = NULL;
    hi_u8 map_len;
    hi_u8 count;
    hi_u64 map = 0;

    if (connector == NULL || db == NULL) {
        HDMI_ERR("Input params is NULL pointer.\n");
        return;
    }

    info = &connector->display_info;
    color = &connector->color;
    hdmi = &info->hdmi;
    map_len = cea_db_payload_len(db) - 1;
    if (map_len == 0) {
        /* All CEA modes support ycbcr420 sampling also. */
        hdmi->y420_cmdb_map = (hi_u64)~0ULL;
        color->format.ycbcr420 = true;
        color->depth.y420_24 = true;
        return;
    }

    /*
     * This map indicates which of the existing CEA block modes
     * from VDB can support YCBCR420 output too. So if bit=0 is
     * set, first mode from VDB can support YCBCR420 output too.
     * We will parse and keep this map, before parsing VDB itself
     * to avoid going through the same block again and again.
     * Spec is not clear about max possible size of this block.
     * Clamping max bitmap block size at 8 bytes. Every byte can
     * address 8 CEA modes, in this way this map can address
     * 8*8 = first 64 SVDs.
     */
    if (map_len > 8) { /* map_len max value is 8. */
        map_len = 8; /* set map_len to 8, when map_len greater than 8. */
    }

    for (count = 0; count < map_len; count++) {
        map |= (hi_u64)db[2 + count] << (8 * count);
    }

    if (map) {
        color->format.ycbcr420 = true;
        color->depth.y420_24 = true;
    }

    hdmi->y420_cmdb_map = map;
}

static hi_s32 add_cea_modes(struct hdmi_connector *connector, struct edid *edid)
{
    const hi_u8 *cea = hdmi_find_cea_extension(edid);
    const hi_u8 *db = NULL;
    const hi_u8 *hdmi = NULL;
    const hi_u8 *video = NULL;
    hi_u8 dbl;
    hi_u8 hdmi_len;
    hi_u8 video_len = 0;
    hi_s32 modes = 0;

    if (cea && cea_revision(cea) >= 3) {
        hi_s32 i, start, end;

        if (cea_db_offsets(cea, &start, &end)) {
            return 0;
        }
        for_each_cea_db(cea, i, start, end) {
            db = &cea[i];
            dbl = cea_db_payload_len(db);

            if (cea_db_tag(db) == VIDEO_BLOCK) {
                video = db + 1;
                video_len = dbl;
                modes += do_cea_modes(connector, video, dbl);
            } else if (cea_db_is_hdmi_vsdb(db)) {
                hdmi = db;
                hdmi_len = dbl;
            } else if (cea_db_is_y420vdb(db)) {
                const hi_u8 *vdb420 = &db[2];

                /* Add 4:2:0(only) modes present in EDID */
                modes += do_y420vdb_modes(connector,
                                          vdb420,
                                          dbl - 1);
            }
        }
    }

    /*
     * We parse the HDMI VSDB after having added the cea modes as we will
     * be patching their flags when the sink supports stereo 3D.
     */
    if (hdmi)
        modes += do_hdmi_vsdb_modes(connector, hdmi, hdmi_len, video,
                                    video_len);

    return modes;
}

static void hdmi_parse_hdmi_vsdb_audio(struct hdmi_connector *connector, const hi_u8 *db)
{
    hi_u8 len;

    len = cea_db_payload_len(db);
    /*
     * if I_Latency_Fields_Present flag == 1(byte8 [6]) then this field only indicates the latency while
     * receiving progressive video formats, otherwise this field indicates the latency when
     * receiving any video format. Value is number of (milliseconds / 2) + 1 with a maximum
     * allowed value of 251 (indicating 500 millisecond duration). A value of 0 indicates that the
     * field is not valid or that the latency is un known. A value of 255 indicates that no video is
     * supported in this device or downstream.
     */
    if (len >= 10) {
        connector->latency.latency_present = (db[8] & (1 << 7)) &&
                                             (!db[9] && (db[9] <= 251)) && (!db[10] && (db[10] <= 251));

        if (!db[9] && (db[9] <= 251)) {
            connector->latency.p_video = (db[9] - 1) * 2;
        }

        if (!db[10] && (db[10] <= 251)) {
            connector->latency.p_audio = (db[10] - 1) * 2;
        }

        connector->latency.i_video = connector->latency.p_video;
        connector->latency.i_audio = connector->latency.p_audio;
    }

    /*
     * I_Latency_Fields_Present shall  be zero if Latency_Fields_Present(byte8 [7]) is zero.
     * spec no mention about value is number of (milliseconds / 2) + 1 with a maximum.
     */
    if ((len >= 12) && (db[8] & (1 << 7)) && (db[8] & (1 << 6))) {
        if (!db[11] && (db[11] <= 251)) {
            connector->latency.i_video = (db[11] - 1) * 2;
        }

        if (!db[12] && (db[12] <= 251)) {
            connector->latency.i_audio = (db[12] - 1) * 2;
        }
    }
}

/**
 * hdmi_detect_monitor_audio - check monitor audio capability
 * @edid: EDID block to scan
 *
 * Monitor should have CEA extension block.
 * If monitor has 'basic audio', but no CEA audio blocks, it's 'basic
 * audio' only. If there is any audio extension block and supported
 * audio format, assume at least 'basic audio' support, even if 'basic
 * audio' is not defined in EDID.
 *
 * Return: True if the monitor supports audio, false otherwise.
 */
hi_bool hdmi_detect_monitor_audio(struct edid *edid)
{
    hi_u8 *edid_ext;
    hi_s32 i, j;
    hi_bool has_audio = false;
    hi_s32 start_offset, end_offset;

    edid_ext = hdmi_find_cea_extension(edid);
    if (!edid_ext) {
        goto end;
    }

    has_audio = ((edid_ext[3] & EDID_BASIC_AUDIO) != 0);

    if (has_audio) {
        HDMI_INFO("Monitor has basic audio support\n");
        goto end;
    }

    if (cea_db_offsets(edid_ext, &start_offset, &end_offset)) {
        goto end;
    }
    for_each_cea_db(edid_ext, i, start_offset, end_offset) {
        if (cea_db_tag(&edid_ext[i]) == AUDIO_BLOCK) {
            has_audio = true;
            for (j = 1; j < cea_db_payload_len(&edid_ext[i]) + 1; j += 3)
                HDMI_INFO("CEA audio format %d\n",
                          (edid_ext[i + j] >> 3) & 0xf);
            goto end;
        }
    }
end:
    return has_audio;
}

static void hdmi_parse_vcdb(struct hdmi_connector *connector, const hi_u8 *db)
{
    struct color_property *color = &connector->color;

    color->quantization.rgb_qs_selecable = !!(db[2] & EDID_CEA_VCDB_QS);
    color->quantization.ycc_qy_selecable = !!(db[2] & EDID_CEA_VCDB_QY);
}

static void hdmi_parse_colorimetry(struct hdmi_connector *connector, const hi_u8 *db)
{
    struct color_property *color = &connector->color;

    color->colorimetry.xvycc601 = !!(db[2] & (1 << 0));
    color->colorimetry.xvycc709 = !!(db[2] & (1 << 1));
    color->colorimetry.sycc601 = !!(db[2] & (1 << 2));
    color->colorimetry.adobe_ycc601 = !!(db[2] & (1 << 3));
    color->colorimetry.adobe_rgb = !!(db[2] & (1 << 4));
    color->colorimetry.bt2020_cycc = !!(db[2] & (1 << 5));
    color->colorimetry.bt2020_ycc = !!(db[2] & (1 << 6));
    color->colorimetry.bt2020_rgb = !!(db[2] & (1 << 7));
    color->colorimetry.dci_p3 = !!(db[3] & (1 << 7));
}

static void hdmi_parse_hdr_static(struct hdmi_connector *connector, const hi_u8 *db)
{
    hi_s32 db_len;
    struct hdr_property *hdr = &connector->hdr;

    db_len = cea_db_payload_len(db);
    if (db_len < 2) {
        return;
    }

    hdr->eotf.traditional_sdr = !!(db[2] & (1 << 0));
    hdr->eotf.traditional_hdr = !!(db[2] & (1 << 1));
    hdr->eotf.st2084_hdr = !!(db[2] & (1 << 2));
    hdr->eotf.hlg = !!(db[2] & (1 << 3));

    hdr->st_metadata.s_type1 = db_len > 2 ? (!!(db[3] & (1 << 0))) : false;
    hdr->st_metadata.max_lum_cv = db_len > 3 ? db[4] : 0;
    hdr->st_metadata.aver_lum_cv = db_len > 4 ? db[5] : 0;
    hdr->st_metadata.min_lum_cv = db_len > 5 ? db[6] : 0;
}

void hdmi_hdr_dynamic_type(struct hdmi_connector *connector, hi_u16 type, hi_u8 version)
{
    struct hdr_property *hdr = &connector->hdr;

    if (type == 0x0001) {
        hdr->dy_metadata.d_type1_support = true;
        hdr->dy_metadata.d_type1_version = version;
    } else if (type == 0x0002) {
        hdr->dy_metadata.d_type2_support = true;
        hdr->dy_metadata.d_type2_version = version;
    } else if (type == 0x0003) {
        hdr->dy_metadata.d_type3_support = true;
    } else if (type == 0x0004) {
        hdr->dy_metadata.d_type4_support = true;
        hdr->dy_metadata.d_type4_version = version;
    } else {
        HDMI_DBG("un-known dynamic hdr type:%d,version:%d\n", type, version);
    }
}

static void hdmi_parse_hdr_dynamic(struct hdmi_connector *connector, const hi_u8 *db)
{
    hi_s32 db_len;
    hi_s32 type_len = 0;
    hi_s32 len = 0;
    hi_u16 dynamic_metadata_type;
    hi_u8 dynamic_version;

    db_len = cea_db_payload_len(db);

    for (len = 1; db_len - len < 3; len += type_len) {
        len++;
        type_len = db[len];
        if (type_len >= 2) {
            len++;
            dynamic_metadata_type = db[len] << 8;
            len++;
            dynamic_metadata_type |= db[len];
            type_len -= 2;
        } else {
            HDMI_INFO("error dynamic !\n");
            break;
        }

        if (type_len) {
            len++;
            dynamic_version = db[len] & 0xf;
            type_len--;
        } else {
            dynamic_version = 0;
        }

        hdmi_hdr_dynamic_type(connector, dynamic_metadata_type, dynamic_version);
    }
}

static void hdmi_parse_dolbyvision(struct hdmi_connector *connector, const hi_u8 *db)
{
    /* hi_u32 i, dolby_ieee_oui; */
    hi_s32 db_len;
    hi_u8 version;
    struct dolby_property *dolby = &connector->dolby;

    db_len = cea_db_payload_len(db);

    version = (db[5] >> 5);

    switch (version) {
        case EDID_DOLBY_VSVDB_VERSION_0:
            if (db_len != EDID_DOLBY_VSVDB_VERSION_0_LEN) {
                break;
            }

            dolby->support_v0 = true;
            dolby->v0.y422_36bit = !!(db[5] & BIT0_MASK);
            dolby->v0.is_2160p60 = !!(db[5] & BIT1_MASK);
            dolby->v0.global_dimming = !!(db[5] & BIT2_MASK);
            dolby->v0.white_x = (db[16] << 4) | ((db[15] >> 4) & EDID_DOLBY_LOWER_NIBBLE_MASK);
            dolby->v0.white_y = (db[17] << 4) | (db[15] & EDID_DOLBY_LOWER_NIBBLE_MASK);
            dolby->v0.dm_minor_ver = (db[21] & EDID_DOLBY_LOWER_NIBBLE_MASK);
            dolby->v0.dm_major_ver = ((db[21] >> 4) & EDID_DOLBY_LOWER_NIBBLE_MASK);
            dolby->v0.target_min_pq = (db[19] << 4) | ((db[18] >> 4) & EDID_DOLBY_LOWER_NIBBLE_MASK);
            dolby->v0.target_max_pq = (db[20] << 4) | (db[18] & EDID_DOLBY_LOWER_NIBBLE_MASK);
            dolby->v0.red_x = (db[7] << 4) | ((db[6] >> 4) & EDID_DOLBY_LOWER_NIBBLE_MASK);
            dolby->v0.red_y = (db[8] << 4) | ((db[6]) & EDID_DOLBY_LOWER_NIBBLE_MASK);
            dolby->v0.green_x = (db[10] << 4) | ((db[9] >> 4) & EDID_DOLBY_LOWER_NIBBLE_MASK);
            dolby->v0.green_y = (db[11] << 4) | (db[9] & EDID_DOLBY_LOWER_NIBBLE_MASK);
            dolby->v0.blue_x = (db[13] << 4) | ((db[12] >> 4) & EDID_DOLBY_LOWER_NIBBLE_MASK);
            dolby->v0.blue_y = (db[14] << 4) | (db[12] & EDID_DOLBY_LOWER_NIBBLE_MASK);
            break;
        case EDID_DOLBY_VSVDB_VERSION_1:
            if (db_len < 11) {
                break;
            }

            dolby->support_v1 = true;
            if (db_len == EDID_DOLBY_VSVDB_VERSION_1_LEN) { /* V1 15Bytes (standard) */
                dolby->v1.y422_36bit = !!(db[5] & BIT0_MASK);
                dolby->v1.is_2160p60 = !!(db[5] & BIT1_MASK);
                dolby->v1.global_dimming = (db[6] & BIT0_MASK);
                dolby->v1.colorimetry = (db[7] & BIT0_MASK);
                dolby->v1.dm_version = ((db[5] >> 2) & EDID_DOLBY_LOWER_3BIT_MASK);
                dolby->v1.low_latency = 0;
                dolby->v1.target_min_lum = ((db[6] >> 1) & EDID_DOLBY_LOWER_7BIT_MASK);
                dolby->v1.target_max_lum = ((db[7] >> 1) & EDID_DOLBY_LOWER_7BIT_MASK);
                dolby->v1.red_x = db[9];
                dolby->v1.red_y = db[10];
                dolby->v1.green_x = db[11];
                dolby->v1.green_y = db[12];
                dolby->v1.blue_x = db[13];
                dolby->v1.blue_y = db[14];
            } else { /* V1 12Bytes (low-latency) */
                dolby->v1.y422_36bit = !!(db[5] & BIT0_MASK);
                dolby->v1.is_2160p60 = !!(db[5] & BIT1_MASK);
                dolby->v1.global_dimming = (db[6] & BIT0_MASK);
                dolby->v1.dm_version = ((db[5] >> 2) & EDID_DOLBY_LOWER_3BIT_MASK);
                dolby->v1.colorimetry = (db[7] & BIT0_MASK);
                dolby->v1.low_latency = db[8] & EDID_DOLBY_LOWER_2BIT_MASK;
                dolby->v1.target_min_lum = ((db[6] >> 1) & EDID_DOLBY_LOWER_7BIT_MASK);
                dolby->v1.target_max_lum = ((db[7] >> 1) & EDID_DOLBY_LOWER_7BIT_MASK);
                dolby->v1.red_x = 0xa0 | ((db[11] >> 3) & EDID_DOLBY_LOWER_5BIT_MASK);
                dolby->v1.red_y = 0x40 | ((db[9] & BIT0_MASK) | ((db[10] & BIT0_MASK) << 1) |
                                          ((db[11] & EDID_DOLBY_LOWER_3BIT_MASK) << 2)); /* need left shift 2bits. */
                dolby->v1.green_x = 0x00 | ((db[9] >> 1) & EDID_DOLBY_LOWER_7BIT_MASK);
                dolby->v1.green_y = 0x80 | ((db[10] >> 1) & EDID_DOLBY_LOWER_7BIT_MASK);
                dolby->v1.blue_x = 0x20 | ((db[8] >> 5) & EDID_DOLBY_LOWER_3BIT_MASK);
                dolby->v1.blue_y = 0x08 | ((db[8] >> 2) & EDID_DOLBY_LOWER_3BIT_MASK);
            }
            break;
        case EDID_DOLBY_VSVDB_VERSION_2:
            if (db_len < EDID_DOLBY_VSVDB_VERSION_2_LEN) {
                break;
            }

            dolby->support_v2 = true;
            dolby->v2.y422_36bit = !!(db[3] & BIT0_MASK);
            dolby->v2.back_light_ctrl = !!(db[3] & BIT1_MASK);
            dolby->v2.global_dimming = !!(db[4] & BIT2_MASK);
            dolby->v2.dm_version = ((db[3] >> 2) & EDID_DOLBY_LOWER_3BIT_MASK);
            dolby->v2.back_lt_min_lum = db[4] & EDID_DOLBY_LOWER_2BIT_MASK;
            dolby->v2.interface = db[5] & EDID_DOLBY_LOWER_2BIT_MASK;
            dolby->v2.y444_rgb_30b36b = ((db[6] & BIT0_MASK) << 1) | (db[7] & BIT0_MASK);
            dolby->v2.target_min_pq_v2 = (db[4] >> 3) & EDID_DOLBY_LOWER_5BIT_MASK;
            dolby->v2.target_max_pq_v2 = (db[5] >> 3) & EDID_DOLBY_LOWER_5BIT_MASK;
            dolby->v2.red_x = 0xa0 | ((db[8] >> 3) & EDID_DOLBY_LOWER_5BIT_MASK);
            dolby->v2.red_y = 0x40 | ((db[9] >> 3) & EDID_DOLBY_LOWER_5BIT_MASK);
            dolby->v2.green_x = 0x00 | ((db[6] >> 1) & EDID_DOLBY_LOWER_7BIT_MASK);
            dolby->v2.green_y = 0x80 | ((db[7] >> 1) & EDID_DOLBY_LOWER_7BIT_MASK);
            dolby->v2.blue_x = 0x20 | (db[8] & EDID_DOLBY_LOWER_3BIT_MASK);
            dolby->v2.blue_y = 0x08 | (db[9] & EDID_DOLBY_LOWER_3BIT_MASK);
            break;
        default:
            break;
    }
}

static void hdmi_parse_hdmi_forum_vsdb(struct hdmi_connector *connector,
                                       const hi_u8 *hf_vsdb)
{
    hi_s32 db_len;
    struct hdmi_display_info *display = &connector->display_info;
    struct scdc_property *scdc = &connector->scdc;
    struct dsc_property *dsc = &connector->dsc;
    struct vrr_property *vrr = &connector->vrr;
    struct color_property *color = &connector->color;

    db_len = cea_db_payload_len(hf_vsdb);
    display->has_hdmi_infoframe = true;

    if (db_len < 7) {
        return;
    }

    if (hf_vsdb[6] & 0x80) {
        scdc->present = true;
        if (hf_vsdb[6] & 0x40) {
            scdc->rr_capable = true;
        }
    }
    display->dvi_dual = !!(hf_vsdb[7] & (0x1 << 1));
    display->max_frl_rate = (hf_vsdb[7] >> 4) & 0xf;
    color->depth.y420_30 = !!(hf_vsdb[7] & HDMI_EDID_YCBCR420_DC_30);
    color->depth.y420_36 = !!(hf_vsdb[7] & HDMI_EDID_YCBCR420_DC_36);
    color->depth.y420_48 = !!(hf_vsdb[7] & HDMI_EDID_YCBCR420_DC_48);

    /*
     * All HDMI 2.0 monitors must support scrambling at rates > 340 MHz.
     * And as per the spec, three factors confirm this:
     * * Availability of a HF-VSDB block in EDID (check)
     * * Non zero Max_TMDS_Char_Rate filed in HF-VSDB (let's check)
     * * SCDC support available (let's check)
     * Lets check it out.
     */
    if (hf_vsdb[5]) {
        /* max clock is 5000 KHz times block value */
        hi_u32 max_tmds_clock = hf_vsdb[5] * 5000;

        if (max_tmds_clock > 340000) {
            display->max_tmds_clock = max_tmds_clock;
            HDMI_DBG("HF-VSDB: max TMDS clock %d kHz\n",
                     display->max_tmds_clock);
        }

        if (scdc->present) {
            /* Few sinks support scrambling for cloks < 340M */
            if ((hf_vsdb[6] & 0x8)) {
                scdc->lte_340mcsc = true;
            }
        }
    }

    if (db_len < 10) {
        return;
    }

    vrr->m_delta = !!(hf_vsdb[8] & (0x1 << 5));
    vrr->cinema_vrr = !!(hf_vsdb[8] & (0x1 << 4));
    vrr->cnm_vrr = !!(hf_vsdb[8] & (0x1 << 3));
    vrr->fva = !!(hf_vsdb[8] & (0x1 << 2));
    vrr->allm = !!(hf_vsdb[8] & (0x1 << 1));
    vrr->fapa_start_locat = !!(hf_vsdb[8] & (0x1 << 0));
    vrr->vrr_min = hf_vsdb[9] & 0x3f;
    vrr->vrr_max = ((hf_vsdb[9] & 0xc0) << 2) | hf_vsdb[10];

    if (db_len < 13) {
        return;
    }

    /* dsc->ccbpci */
    dsc->dsc_10bpc = !!(hf_vsdb[11] & (0x1 << 0));
    dsc->dsc_12bpc = !!(hf_vsdb[11] & (0x1 << 1));
    dsc->dsc_16bpc = !!(hf_vsdb[11] & (0x1 << 2));
    dsc->all_bpp = !!(hf_vsdb[11] & (0x1 << 3));

    dsc->y420 = !!(hf_vsdb[11] & (0x1 << 6));
    dsc->dsc_1p2 = !!(hf_vsdb[11] & (0x1 << 7));
    dsc->dsc_max_rate = (hf_vsdb[12] >> 4) & 0xf;
    dsc->max_slice = hf_vsdb[12] & 0xf;
    dsc->total_chunk_bytes = hf_vsdb[13] & 0x3f;
}

static void hdmi_parse_hdmi_deep_color_info(struct hdmi_connector *connector,
                                            const hi_u8 *hdmi)
{
    struct color_property *color = &connector->color;
    hi_u32 dc_bpc = 0;

    /* HDMI supports at least 8 bpc */
    color->depth.bpc = 8;

    if (cea_db_payload_len(hdmi) < 6) {
        return;
    }

    if (hdmi[6] & HDMI_EDID_HDMI_DC_30) {
        dc_bpc = 10;
        color->depth.rgb_30 = true;
        HDMI_DBG("%s: HDMI sink does deep color 30.\n",
                 connector->name);
    }

    if (hdmi[6] & HDMI_EDID_HDMI_DC_36) {
        dc_bpc = 12;
        color->depth.rgb_36 = true;
        HDMI_DBG("%s: HDMI sink does deep color 36.\n",
                 connector->name);
    }

    if (hdmi[6] & HDMI_EDID_HDMI_DC_48) {
        dc_bpc = 16;
        color->depth.rgb_48 = true;
        HDMI_DBG("%s: HDMI sink does deep color 48.\n",
                 connector->name);
    }

    if (dc_bpc == 0) {
        HDMI_DBG("%s: No deep color support on this HDMI sink.\n",
                 connector->name);
        return;
    }

    HDMI_DBG("%s: Assigning HDMI sink color depth as %d bpc.\n",
             connector->name, dc_bpc);
    color->depth.bpc = dc_bpc;

    /*
     * Deep color support mandates RGB444 support for all video
     * modes and forbids YCRCB422 support for all video modes per
     * HDMI 1.3 spec.
     */
    color->format.rgb = true;
    color->depth.rgb_24 = true;

    /* YCRCB444 is optional according to spec. */
    if (hdmi[6] & HDMI_EDID_HDMI_DC_Y444) {
        color->format.ycbcr444 = true;
        color->depth.y444_24 = true;
        color->depth.y444_30 = color->depth.rgb_30;
        color->depth.y444_36 = color->depth.rgb_36;
        color->depth.y444_48 = color->depth.rgb_48;
        HDMI_DBG("%s: HDMI sink does YCRCB444 in deep color.\n",
                 connector->name);
    }

    /*
     * Spec says that if any deep color mode is supported at all,
     * then deep color 36 bit must be supported.
     */
    if (!(hdmi[6] & HDMI_EDID_HDMI_DC_36))
        HDMI_DBG("%s: HDMI sink should do DC_36, but does not!\n",
                 connector->name);
}
static void hdmi_parse_hdmi_vsdb_video(struct hdmi_connector *connector, const hi_u8 *db)
{
    struct hdmi_display_info *info = &connector->display_info;
    hi_u8 len = cea_db_payload_len(db);

    /* forum vsdb & vsdb can also set true. add by tangqy */
    info->has_hdmi_infoframe = true;

    if (len >= 6) {
        info->dvi_dual = db[6] & 1;
    }
    if (len >= 7) {
        info->max_tmds_clock = db[7] * 5000;
        if (!info->max_tmds_clock) {
            info->max_tmds_clock = EDID_DEFAULT_DVI_MAX_TMDS;
            HDMI_WARN("zero max_tmds,fix to %u KHz\n", EDID_DEFAULT_DVI_MAX_TMDS);
        }
    }

    HDMI_DBG("HDMI: DVI dual %d, max TMDS clock %d kHz\n",
             info->dvi_dual,
             info->max_tmds_clock);

    hdmi_parse_hdmi_deep_color_info(connector, db);
}

static void hdmi_parse_cea_audio(struct hdmi_connector *connector, struct edid *edid)
{
    hi_u8 *edid_ext;
    hi_u8 byte0 = 0;
    hi_u8 byte1 = 0;
    hi_u8 byte2 = 0;
    hi_s32 i, j, db_len;
    hi_s32 start_offset, end_offset;
    hi_u32 sad_count = 0;
    hi_u8 fmt_code, ext_code;

    struct audio_property *audio = &connector->audio;

    edid_ext = hdmi_find_cea_extension(edid);
    if (!edid_ext) {
        return;
    }

    audio->basic = ((edid_ext[3] & EDID_BASIC_AUDIO) != 0);
    if (!audio->basic) {
        HDMI_DBG("monitor no support basic audio!\n");
    }

    if (cea_db_offsets(edid_ext, &start_offset, &end_offset)) {
        return;
    }

    /* see CTA-861-G 7.5.2 */
    for_each_cea_db(edid_ext, i, start_offset, end_offset) {
        if (cea_db_tag(&edid_ext[i]) == AUDIO_BLOCK) {
            audio->basic = true;
            db_len = cea_db_payload_len(&edid_ext[i]) + 1;
            for (j = 1; (j < db_len) && (sad_count < MAX_SAD_AUDIO_CNT); j += 3) {
                byte0 = edid_ext[i + j + 0];
                byte1 = edid_ext[i + j + 1];
                byte2 = edid_ext[i + j + 2];
                fmt_code = (byte0 & EDID_AUDIO_FORMAT_MASK) >> 3;
                ext_code = fmt_code == 0xf ? (byte2 & EDID_AUDIO_EXT_TYPE_CODE) >> 3 : 0;
                audio->sad[sad_count].fmt_code = fmt_code;
                audio->sad[sad_count].ext_code = ext_code;
                if ((fmt_code == 0) ||
                    (fmt_code == 0xf && (ext_code <= 3 || ext_code == 9 || ext_code > 0xd))) {
                    continue;
                }

                if ((fmt_code >= 1 && fmt_code <= 14) || (fmt_code == 0xf && ext_code <= 10)) {
                    audio->sad[sad_count].max_channel = (byte0 & EDID_AUDIO_CHANNEL_MASK) + 1;
                } else if (fmt_code == 0xf && ext_code == 0xd) {
                    /* max_channel is byte0 & 0x07 >> 0 | byte0 & 0x80) >> 4 | byte1 & 0x80) >> 3 */
                    audio->sad[sad_count].max_channel = (((byte0 & 0x07) >> 0) |
                                                         ((byte0 & 0x80) >> 4) | ((byte1 & 0x80) >> 3)) +
                                                        1;
                } else {
                    audio->sad[sad_count].max_channel = 0;
                }

                audio->sad[sad_count].samp_32k = !!(byte1 & (0x1 << 0));
                audio->sad[sad_count].samp_44p1k = !!(byte1 & (0x1 << 1));
                audio->sad[sad_count].samp_48k = !!(byte1 & (0x1 << 2));
                audio->sad[sad_count].samp_88p2k = !!(byte1 & (0x1 << 3));
                audio->sad[sad_count].samp_96k = !!(byte1 & (0x1 << 4));
                audio->sad[sad_count].samp_176p4k = !!(byte1 & (0x1 << 5));
                audio->sad[sad_count].samp_192k = !!(byte1 & (0x1 << 6));

                if (fmt_code == 0xf) {
                    if ((ext_code >= 4 && ext_code <= 6) || (ext_code >= 8 && ext_code <= 10)) {
                        audio->sad[sad_count].samp_176p4k = false;
                        audio->sad[sad_count].samp_192k = false;
                    } else if (ext_code == 12) {
                        audio->sad[sad_count].samp_32k = false;
                        audio->sad[sad_count].samp_88p2k = false;
                        audio->sad[sad_count].samp_176p4k = false;
                    }
                }

                if (fmt_code == 1 || ext_code == 13) {
                    audio->sad[sad_count].width_16 = !!(byte2 & (0x1 << 0));
                    audio->sad[sad_count].width_20 = !!(byte2 & (0x1 << 1));
                    audio->sad[sad_count].width_24 = !!(byte2 & (0x1 << 2));
                } else {
                    audio->sad[sad_count].width_16 = false;
                    audio->sad[sad_count].width_20 = false;
                    audio->sad[sad_count].width_24 = false;
                }

                if (fmt_code >= 2 && fmt_code <= 8) {
                    audio->sad[sad_count].max_bit_rate = byte2 * 8000;
                } else {
                    audio->sad[sad_count].max_bit_rate = 0;
                }

                if (fmt_code >= 9 && fmt_code <= 13) { /* fmt_code is in range[9-13]. */
                    audio->sad[sad_count].dependent = byte2;
                } else if (ext_code == 11 || ext_code == 12) {
                    audio->sad[sad_count].dependent = byte2 & 0x7;
                } else {
                    audio->sad[sad_count].dependent = 0;
                }

                if (fmt_code == 14) {
                    audio->sad[sad_count].profile = byte2 & 0x7;
                } else {
                    audio->sad[sad_count].profile = 0;
                }

                if (ext_code >= 8 && ext_code <= 10) {
                    audio->sad[sad_count].len_1024_tl = !!(byte2 & (0x1 << 2));
                    audio->sad[sad_count].len_960_tl = !!(byte2 & (0x1 << 1));
                    audio->sad[sad_count].mps_l = !!(byte2 & (0x1 << 0));
                } else if (ext_code >= 4 && ext_code <= 6) {
                    audio->sad[sad_count].len_1024_tl = !!(byte2 & (0x1 << 2));
                    audio->sad[sad_count].len_960_tl = !!(byte2 & (0x1 << 1));
                    audio->sad[sad_count].mps_l = false;
                } else {
                    audio->sad[sad_count].len_1024_tl = false;
                    audio->sad[sad_count].len_960_tl = false;
                    audio->sad[sad_count].mps_l = false;
                }

                sad_count++;
            }
        }
    }
    audio->sad_count = sad_count;
}

static void hdmi_parse_cea_ext(struct hdmi_connector *connector,
                               struct edid *edid)
{
    struct hdmi_display_info *info = &connector->display_info;
    struct color_property *color = &connector->color;
    const hi_u8 *edid_ext;
    hi_s32 i, start, end;

    edid_ext = hdmi_find_cea_extension(edid);
    if (!edid_ext) {
        return;
    }

    info->cea_rev = edid_ext[1];

    /* The existence of a CEA block should imply RGB support */
    color->format.rgb = true;
    color->depth.rgb_24 = true;

    if (edid_ext[3] & EDID_CEA_YCRCB444) {
        color->format.ycbcr444 = true;
        color->depth.y444_24 = true;
    }
    if (edid_ext[3] & EDID_CEA_YCRCB422) {
        color->format.ycbcr422 = true;
    }

    if (cea_db_offsets(edid_ext, &start, &end)) {
        return;
    }

    for_each_cea_db(edid_ext, i, start, end) {
        const hi_u8 *db = &edid_ext[i];

        if (cea_db_is_hdmi_vsdb(db)) {
            hdmi_parse_hdmi_vsdb_video(connector, db);
            hdmi_parse_hdmi_vsdb_audio(connector, db);
        }

        if (cea_db_is_hdmi_forum_vsdb(db)) {
            hdmi_parse_hdmi_forum_vsdb(connector, db);
        }

        if (cea_db_is_y420cmdb(db)) {
            hdmi_parse_y420cmdb_bitmap(connector, db);
        }

        if (cea_db_is_vcdb(db)) {
            hdmi_parse_vcdb(connector, db);
        }

        if (cea_db_is_colorimetry_db(db)) {
            hdmi_parse_colorimetry(connector, db);
        }

        if (cea_db_is_hdr_static_db(db)) {
            hdmi_parse_hdr_static(connector, db);
        }

        if (cea_db_is_hdr_dynamic_db(db)) {
            hdmi_parse_hdr_dynamic(connector, db);
        }

        if (cea_db_is_dolbyvision(db)) {
            hdmi_parse_dolbyvision(connector, db);
        }
    }
}

hi_u32 hdmi_add_display_info(struct hdmi_connector *connector, struct edid *edid)
{
    hi_u16 i, name;
    struct hdmi_display_info *info = &connector->display_info;
    /* hi_u32 quirks; */
    struct color_property *color = &connector->color;
    struct base_property *base = &connector->base;

    info->width_cm = edid->width_cm;
    info->height_cm = edid->height_cm;

    base->revision = edid->revision;
    base->version = edid->version;
    base->ext_block_num = edid->extensions;
    name = edid->mfg_id[0];
    name <<= 8;
    name |= edid->mfg_id[1];
    for (i = 0; i < 3; i++) {
        base->vendor.mfc_name[2 - i] = (name & (0x1f << (5 * i))) >> (5 * i);
        /* 'A'~'Z' is refered to 1~26 */
        if ((base->vendor.mfc_name[2 - i] > 0) && base->vendor.mfc_name[2 - i] < 27) {
            base->vendor.mfc_name[2 - i] += 'A' - 1;
        } else {
            HDMI_WARN("mfc name[%d]=%d,not ref[1,26]\n", i, base->vendor.mfc_name[2 - i]);
        }
    }
    base->vendor.product_code = (edid->prod_code[1] << 8) | edid->prod_code[0];
    base->vendor.serial_num = edid->serial;
    base->vendor.mfc_year = edid->mfg_year + EDID_VEND_YEAR_BASE;
    base->vendor.mfc_week = edid->mfg_week;
    if (edid->revision < 3) {
        return 0;
    }
    hdmi_parse_cea_ext(connector, edid);

    /*
     * Digital sink with "DFP 1.x compliant TMDS" according to EDID 1.3?
     * For such displays, the DFP spec 1.0, section 3.10 "EDID support"
     * tells us to assume 8 bpc color depth if the EDID doesn't have
     * extensions which tell otherwise.
     */
    if ((color->depth.bpc == 0) && (edid->revision < 4) &&
        (edid->input & HDMI_EDID_DIGITAL_TYPE_DVI)) {
        color->depth.bpc = 8;
        HDMI_DBG("%s: Assigning DFP sink color depth as %d bpc.\n",
                 connector->name, color->depth.bpc);
    }

    /* Only defined for 1.4 with digital displays */
    if (edid->revision < 4) {
        return 0;
    }

    switch (edid->input & HDMI_EDID_DIGITAL_DEPTH_MASK) {
        case HDMI_EDID_DIGITAL_DEPTH_6:
            color->depth.bpc = 6;
            break;
        case HDMI_EDID_DIGITAL_DEPTH_8:
            color->depth.bpc = 8;
            break;
        case HDMI_EDID_DIGITAL_DEPTH_10:
            color->depth.bpc = 10;
            break;
        case HDMI_EDID_DIGITAL_DEPTH_12:
            color->depth.bpc = 12;
            break;
        case HDMI_EDID_DIGITAL_DEPTH_14:
            color->depth.bpc = 14;
            break;
        case HDMI_EDID_DIGITAL_DEPTH_16:
            color->depth.bpc = 16;
            break;
        case HDMI_EDID_DIGITAL_DEPTH_UNDEF:
        default:
            color->depth.bpc = 0;
            break;
    }

    HDMI_DBG("%s: Assigning EDID-1.4 digital sink color depth as %d bpc.\n",
             connector->name, color->depth.bpc);

    color->format.rgb = true;
    if (edid->features & HDMI_EDID_FEATURE_RGB_YCRCB444) {
        color->format.ycbcr444 = true;
    }

    if (edid->features & HDMI_EDID_FEATURE_RGB_YCRCB422) {
        color->format.ycbcr422 = true;
    }

    /* return quirks; */
    return 0;
}

static hi_s32 validate_displayid(hi_u8 *displayid, hi_s32 length, hi_s32 idx)
{
    hi_s32 i;
    hi_u8 csum = 0;
    struct displayid_hdr *base;

    base = (struct displayid_hdr *)&displayid[idx];

    HDMI_DBG("base revision 0x%x, length %d, %d %d\n",
             base->rev, base->bytes, base->prod_id, base->ext_count);

    if (base->bytes + 5 > length - idx) {
        return -EINVAL;
    }

    for (i = idx; i <= base->bytes + 5; i++) {
        csum += displayid[i];
    }

    if (csum) {
        HDMI_WARN("DisplayID checksum invalid, remainder is %d\n", csum);
        return -EINVAL;
    }
    return 0;
}
static struct hdmi_display_mode *hdmi_mode_displayid_detailed(struct hisilicon_hdmi *hdmi_dev,
                                                              struct displayid_detailed_timings_1 *timings)
{
    struct hdmi_display_mode *mode = NULL;
    struct hdmi_detail *detail = NULL;
    hi_u32 pixel_clock = (timings->pixel_clock[0] |
                          (timings->pixel_clock[1] << 8) |
                          (timings->pixel_clock[2] << 16));
    hi_u32 hactive = (timings->hactive[0] | (timings->hactive[1] << 8)) + 1; /* need left shift 8bits. */
    hi_u32 hblank = (timings->hblank[0] | (timings->hblank[1] << 8)) + 1; /* need left shift 8bits. */
    hi_u32 hsync = (timings->hsync[0] | ((timings->hsync[1] & 0x7f) << 8)) + 1; /* need left shift 8bits. */
    hi_u32 hsync_width = (timings->hsw[0] | (timings->hsw[1] << 8)) + 1; /* need left shift 8bits. */
    hi_u32 vactive = (timings->vactive[0] | (timings->vactive[1] << 8)) + 1; /* need left shift 8bits. */
    hi_u32 vblank = (timings->vblank[0] | (timings->vblank[1] << 8)) + 1; /* need left shift 8bits. */
    hi_u32 vsync = (timings->vsync[0] | ((timings->vsync[1] & 0x7f) << 8)) + 1; /* need left shift 8bits. */
    hi_u32 vsync_width = (timings->vsw[0] | (timings->vsw[1] << 8)) + 1; /* need left shift 8bits. */
    hi_bool hsync_positive = (timings->hsync[1] >> 7) & 0x1; /* need right shift 7bits. */
    hi_bool vsync_positive = (timings->vsync[1] >> 7) & 0x1; /* need right shift 7bits. */

    /* add by tangqy */
    if (!hactive || !vactive || !pixel_clock ||
        hblank < hsync + hsync_width ||
        vblank < vsync + vsync_width)
        HDMI_WARN("error detail timing info!hactive=%d,vactive=%d,pixel_clock=%d,hblank=%d,vblank=%d\n",
                  hactive, vactive, pixel_clock, hblank, vblank);

    mode = hdmi_mode_create_by_detailed_timing(pixel_clock, hactive, vactive, hblank, vblank);
    if (mode) {
        return mode;
    }

    mode = hdmi_mode_create(hdmi_dev);
    if (!mode) {
        return NULL;
    }

    detail = &mode->detail;
    /* hdmi_dev->connector.detail_vic_base++; */
    mode->vic = ++hdmi_dev->connector->detail_vic_base;

    /* change by tangqy */
    detail->pixel_clock = pixel_clock * 10;

    detail->h_active = hactive;
    detail->h_total = hactive + hblank;
    detail->h_blank = hblank;
    /* note:kernel code 'mode->hsync_start = mode->hdisplay + hsync;' so,here,hsync is the front. */
    detail->h_front = hsync;
    detail->h_sync = hsync_width;
    detail->h_back = hblank - detail->h_sync - detail->h_front;

    detail->v_active = vactive;
    detail->v_total = vactive + vblank;
    detail->v_blank = vblank;
    detail->v_front = vsync;
    detail->v_sync = vsync_width;
    detail->v_back = vblank - detail->v_sync - detail->v_front;

    mode->flags = 0;
    mode->flags |= hsync_positive ? HDMI_MODE_FLAG_PHSYNC : HDMI_MODE_FLAG_NHSYNC;
    mode->flags |= vsync_positive ? HDMI_MODE_FLAG_PVSYNC : HDMI_MODE_FLAG_NVSYNC;
    mode->type = HDMI_MODE_TYPE_DRIVER;

    /* add by tangqy */
    detail->h_pol = hsync_positive;
    detail->v_pol = vsync_positive;

    if (timings->flags & 0x80) {
        mode->type |= HDMI_MODE_TYPE_PREFERRED;
    }
    detail->field_rate = hdmi_mode_vrefresh(mode);
    hdmi_mode_set_name(mode);

    return mode;
}
static hi_s32 add_displayid_detailed_1_modes(struct hdmi_connector *connector,
                                             struct displayid_block *block)
{
    struct displayid_detailed_timing_block *det = (struct displayid_detailed_timing_block *)block;
    hi_s32 i;
    hi_s32 num_timings;
    struct hdmi_display_mode *newmode = HI_NULL;
    hi_s32 num_modes = 0;
    /* blocks must be multiple of 20 bytes length */
    if (block->num_bytes % 20) {
        return 0;
    }

    num_timings = block->num_bytes / 20;
    for (i = 0; i < num_timings; i++) {
        struct displayid_detailed_timings_1 *timings = &det->timings[i];

        newmode = hdmi_mode_displayid_detailed(connector->hdmi_dev,
                                               timings);
        if (!newmode) {
            continue;
        }

        newmode->parse_type |= MODE_TYPE_DETAILED_TIMINGE;
        hdmi_mode_probed_add(connector, newmode);
        num_modes++;
    }
    return num_modes;
}
static hi_s32 add_displayid_detailed_modes(struct hdmi_connector *connector,
                                           struct edid *edid)
{
    hi_u8 *displayid = HI_NULL;
    hi_s32 ret;
    hi_s32 idx = 1;
    hi_s32 length = EDID_LENGTH;
    struct displayid_block *block = HI_NULL;
    hi_s32 num_modes = 0;

    displayid = hdmi_find_displayid_extension(edid);
    if (displayid == HI_NULL) {
        return 0;
    }

    ret = validate_displayid(displayid, length, idx);
    if (ret) {
        return 0;
    }

    idx += sizeof(struct displayid_hdr);
    while (block = (struct displayid_block *)&displayid[idx],
           idx + sizeof(struct displayid_block) <= length &&
           idx + sizeof(struct displayid_block) + block->num_bytes <= length &&
           block->num_bytes > 0) {
        idx += block->num_bytes + sizeof(struct displayid_block);
        switch (block->tag) {
            case DATA_BLOCK_TYPE_1_DETAILED_TIMING:
                num_modes += add_displayid_detailed_1_modes(connector, block);
                break;
            default:
                break;
        }
    }
    return num_modes;
}

/**
 * hdmi_add_edid_modes - add modes from EDID data, if available
 * @connector: connector we're probing
 * @edid: EDID data
 *
 * Add the specified modes to the connector's mode list. Also fills out the
 * &hdmi_display_info structure and ELD in @connector with any information which
 * can be derived from the edid.
 *
 * Return: The number of modes added or 0 if we couldn't find any.
 */
hi_s32 hdmi_add_edid_modes(struct hdmi_connector *connector, struct edid *edid)
{
    hi_s32 num_modes = 0;
    hi_u32 quirks;

    if (edid == NULL) {
        clear_eld(connector);
        return 0;
    }
    if (!hdmi_edid_is_valid(edid)) {
        clear_eld(connector);
        HDMI_DBG("edid isn't valid!\n");
        return 0;
    }

    hdmi_parse_cea_audio(connector, edid);

    connector->display_info.max_tmds_clock = EDID_DEFAULT_DVI_MAX_TMDS;
    /*
     * CEA-861-F adds ycbcr capability map block, for HDMI 2.0 sinks.
     * To avoid multiple parsing of same block, lets parse that map
     * from sink info, before parsing CEA modes.
     */
    quirks = hdmi_add_display_info(connector, edid);

    /*
     * EDID spec says modes should be preferred in this order:
     * - preferred detailed mode
     * - other detailed modes from base block
     * - detailed modes from extension blocks
     * - CVT 3-byte code modes
     * - standard timing codes
     * - established timing codes
     * - modes inferred from GTF or CVT range information
     *
     * We get this pretty much right.
     *
     * XXX order for additional mode types in extension blocks?
     */
    num_modes += add_detailed_modes(connector, edid, quirks);

    num_modes += add_standard_modes(connector, edid);

    num_modes += add_established_modes(connector, edid);

    num_modes += add_cea_modes(connector, edid);

    num_modes += add_displayid_detailed_modes(connector, edid);

    return num_modes;
}





