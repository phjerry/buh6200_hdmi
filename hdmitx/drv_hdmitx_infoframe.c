/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hitxv300 infoframe module main source file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#include <linux/hisilicon/securec.h>
#include "drv_hdmitx_infoframe.h"
#include "drv_hdmitx.h"

static hi_u8 hdmi_infoframe_checksum(const hi_u8 *ptr, size_t size)
{
    hi_u8 csum = 0;
    size_t i;

    /* compute checksum */
    for (i = 0; i < size; i++) {
        csum += ptr[i];
    }

    return 256 - csum;
}

static void hdmi_infoframe_set_checksum(void *buffer, size_t size)
{
    hi_u8 *ptr = buffer;

    ptr[3] = hdmi_infoframe_checksum(buffer, size);
}

/*
 * hdmi_avi_infoframe_init() - initialize an HDMI AVI infoframe
 * @frame: HDMI AVI infoframe
 * Returns 0 on success or a negative error code on failure.
 */
hi_s32 hdmi_avi_infoframe_init(struct hdmi_avi_infoframe *frame)
{
    if (memset_s(frame, sizeof(struct hdmi_avi_infoframe),
                 0, sizeof(struct hdmi_avi_infoframe))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    frame->type = HDMI_INFOFRAME_TYPE_AVI;
    frame->version = 2;
    frame->length = HDMI_AVI_INFOFRAME_SIZE;

    return 0;
}
EXPORT_SYMBOL(hdmi_avi_infoframe_init);

static hi_s32 hdmi_avi_infoframe_check_only(const struct hdmi_avi_infoframe *frame)
{
    if (frame->type != HDMI_INFOFRAME_TYPE_AVI ||
        (frame->version != 2 &&
         frame->version != 3 &&
         frame->version != 4) ||
        frame->length != HDMI_AVI_INFOFRAME_SIZE) {
        return -EINVAL;
    }

    if (frame->picture_aspect > HDMI_PICTURE_ASPECT_256_135) {
        return -EINVAL;
    }

    return 0;
}

/*
 * hdmi_avi_infoframe_check() - check a HDMI AVI infoframe
 * @frame: HDMI AVI infoframe
 * Validates that the infoframe is consistent and updates derived fields
 * (eg. length) based on other fields.
 * Returns 0 on success or a negative error code on failure.
 */
hi_s32 hdmi_avi_infoframe_check(struct hdmi_avi_infoframe *frame)
{
    return hdmi_avi_infoframe_check_only(frame);
}
EXPORT_SYMBOL(hdmi_avi_infoframe_check);

/*
 * hdmi_avi_infoframe_pack_only() - write HDMI AVI infoframe to binary buffer
 * @frame: HDMI AVI infoframe
 * @buffer: destination buffer
 * @size: size of buffer
 * Packs the information contained in the @frame structure into a binary
 * representation that can be written into the corresponding controller
 * registers. Also computes the checksum as required by section 5.3.5 of
 * the HDMI 1.4 specification.
 * Returns the number of bytes packed into the binary buffer or a negative
 * error code on failure.
 */
ssize_t hdmi_avi_infoframe_pack_only(const struct hdmi_avi_infoframe *frame,
                                     void *buffer, size_t size)
{
    hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    ret = hdmi_avi_infoframe_check_only(frame);
    if (ret) {
        return ret;
    }

    length = HDMI_INFOFRAME_HEADER_SIZE + frame->length;

    if (size < length) {
        return -ENOSPC;
    }

    if (memset_s(buffer, size, 0, size)) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    ptr[0] = frame->type;
    ptr[1] = frame->version;
    ptr[2] = frame->length;
    ptr[3] = 0; /* checksum */

    /* start infoframe payload */
    ptr += HDMI_INFOFRAME_HEADER_SIZE;

    ptr[0] = ((frame->colorspace & 0x3) << 5) | (frame->scan_mode & 0x3);

    /*
     * Data byte 1, bit 4 has to be set if we provide the active format
     * aspect ratio
     */
    if (frame->active_aspect & 0xf) {
        ptr[0] |= (1 << 4);  /* bit 4 is set to */
    }

    /* Bit 3 and 2 indicate if we transmit horizontal/vertical bar data */
    if (frame->top_bar || frame->bottom_bar) {
        ptr[0] |= (1 << 3);  /* bit 3 is set to 1 */
    }

    if (frame->left_bar || frame->right_bar) {
        ptr[0] |= (1 << 2);  /* bit 2 is set to 1 */
    }

    ptr[1] = ((frame->colorimetry & 0x3) << 6) |
             ((frame->picture_aspect & 0x3) << 4) |
             (frame->active_aspect & 0xf);

    ptr[2] = ((frame->extended_colorimetry & 0x7) << 4) |
             ((frame->quantization_range & 0x3) << 2) |
             (frame->nups & 0x3);

    if (frame->itc) {
        ptr[2] |= BIT(7);
    }

    ptr[3] = frame->video_code;

    ptr[4] = ((frame->ycc_quantization_range & 0x3) << 6) |
             ((frame->content_type & 0x3) << 4) |
             (frame->pixel_repeat & 0xf);

    ptr[5] = frame->top_bar & 0xff;
    ptr[6] = (frame->top_bar >> 8) & 0xff;
    ptr[7] = frame->bottom_bar & 0xff;
    ptr[8] = (frame->bottom_bar >> 8) & 0xff;
    ptr[9] = frame->left_bar & 0xff;
    ptr[10] = (frame->left_bar >> 8) & 0xff;
    ptr[11] = frame->right_bar & 0xff;
    ptr[12] = (frame->right_bar >> 8) & 0xff;

    hdmi_infoframe_set_checksum(buffer, length);

    return length;
}
EXPORT_SYMBOL(hdmi_avi_infoframe_pack_only);

/*
 * hdmi_avi_infoframe_pack() - check a HDMI AVI infoframe, and write it to binary buffer
 * @frame: HDMI AVI infoframe
 * @buffer: destination buffer
 * @size: size of buffer
 * Validates that the infoframe is consistent and updates derived fields
 * (eg. length) based on other fields, after which it packs the information
 * contained in the @frame structure into a binary representation that
 * can be written into the corresponding controller registers. This function
 * also computes the checksum as required by section 5.3.5 of the HDMI 1.4
 * specification.
 * Returns the number of bytes packed into the binary buffer or a negative
 * error code on failure.
 */
ssize_t hdmi_avi_infoframe_pack(struct hdmi_avi_infoframe *frame,
                                void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_avi_infoframe_check(frame);
    if (ret) {
        return ret;
    }

    return hdmi_avi_infoframe_pack_only(frame, buffer, size);
}
EXPORT_SYMBOL(hdmi_avi_infoframe_pack);

/*
 * hdmi_spd_infoframe_init() - initialize an HDMI SPD infoframe
 * @frame: HDMI SPD infoframe
 * @vendor: vendor string
 * @product: product string
 * Returns 0 on success or a negative error code on failure.
 */
hi_s32 hdmi_spd_infoframe_init(struct hdmi_spd_infoframe *frame,
                               const hi_s8 *vendor, const hi_s8 *product)
{
    if (memset_s(frame, sizeof(struct hdmi_spd_infoframe),
                 0, sizeof(struct hdmi_spd_infoframe))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    frame->type = HDMI_INFOFRAME_TYPE_SPD;
    frame->version = 1;
    frame->length = HDMI_SPD_INFOFRAME_SIZE;

    if (strncpy_s(frame->vendor, sizeof(frame->vendor), vendor, sizeof(frame->vendor) - 1)) {
        HDMI_ERR("strncpy_s err\n");
        return HI_FAILURE;
    }
    if (strncpy_s(frame->product, sizeof(frame->product), product, sizeof(frame->product) - 1)) {
        HDMI_ERR("strncpy_s err\n");
        return HI_FAILURE;
    }

    return 0;
}
EXPORT_SYMBOL(hdmi_spd_infoframe_init);

static hi_s32 hdmi_spd_infoframe_check_only(const struct hdmi_spd_infoframe *frame)
{
    if (frame->type != HDMI_INFOFRAME_TYPE_SPD ||
        frame->version != 1 ||
        frame->length != HDMI_SPD_INFOFRAME_SIZE) {
        return -EINVAL;
    }

    return 0;
}

/*
 * hdmi_spd_infoframe_check() - check a HDMI SPD infoframe
 * @frame: HDMI SPD infoframe
 * Validates that the infoframe is consistent and updates derived fields
 * (eg. length) based on other fields.
 * Returns 0 on success or a negative error code on failure.
 */
hi_s32 hdmi_spd_infoframe_check(struct hdmi_spd_infoframe *frame)
{
    return hdmi_spd_infoframe_check_only(frame);
}
EXPORT_SYMBOL(hdmi_spd_infoframe_check);

/*
 * hdmi_spd_infoframe_pack_only() - write HDMI SPD infoframe to binary buffer
 * @frame: HDMI SPD infoframe
 * @buffer: destination buffer
 * @size: size of buffer
 * Packs the information contained in the @frame structure into a binary
 * representation that can be written into the corresponding controller
 * registers. Also computes the checksum as required by section 5.3.5 of
 * the HDMI 1.4 specification.
 * Returns the number of bytes packed into the binary buffer or a negative
 * error code on failure.
 */
ssize_t hdmi_spd_infoframe_pack_only(const struct hdmi_spd_infoframe *frame,
                                     void *buffer, size_t size)
{
    hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    ret = hdmi_spd_infoframe_check_only(frame);
    if (ret) {
        return ret;
    }

    length = HDMI_INFOFRAME_HEADER_SIZE + frame->length;
    if (length > size) {
        return -ENOSPC;
    }

    if (memset_s(buffer, size, 0, size)) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    ptr[0] = frame->type;
    ptr[1] = frame->version;
    ptr[2] = frame->length;
    ptr[3] = 0; /* checksum */

    /* start infoframe payload */
    ptr += HDMI_INFOFRAME_HEADER_SIZE;

    if (memcpy_s(ptr, size - HDMI_INFOFRAME_HEADER_SIZE, frame->vendor, sizeof(frame->vendor))) {
        HDMI_ERR("memcpy_s err\n");
        return HI_FAILURE;
    }
    if (memcpy_s(ptr + 8, size - HDMI_INFOFRAME_HEADER_SIZE - 8, /* 8: vendor len */
                 frame->product, sizeof(frame->product))) {
        HDMI_ERR("memcpy_s err\n");
        return HI_FAILURE;
    }

    ptr[24] = frame->sdi;

    hdmi_infoframe_set_checksum(buffer, length);

    return length;
}
EXPORT_SYMBOL(hdmi_spd_infoframe_pack_only);

/**
 * hdmi_spd_infoframe_pack() - check a HDMI SPD infoframe, and write it to binary buffer
 * @frame: HDMI SPD infoframe
 * @buffer: destination buffer
 * @size: size of buffer
 * Validates that the infoframe is consistent and updates derived fields
 * (eg. length) based on other fields, after which it packs the information
 * contained in the @frame structure into a binary representation that
 * can be written into the corresponding controller registers. This function
 * also computes the checksum as required by section 5.3.5 of the HDMI 1.4
 * specification.
 * Returns the number of bytes packed into the binary buffer or a negative
 * error code on failure.
 */
ssize_t hdmi_spd_infoframe_pack(struct hdmi_spd_infoframe *frame,
                                void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_spd_infoframe_check(frame);
    if (ret) {
        return ret;
    }

    return hdmi_spd_infoframe_pack_only(frame, buffer, size);
}
EXPORT_SYMBOL(hdmi_spd_infoframe_pack);

/*
 * hdmi_audio_infoframe_init() - initialize an HDMI audio infoframe
 * @frame: HDMI audio infoframe
 * Returns 0 on success or a negative error code on failure.
 */
hi_s32 hdmi_audio_infoframe_init(struct hdmi_audio_infoframe *frame)
{
    if (memset_s(frame, sizeof(struct hdmi_audio_infoframe),
                 0, sizeof(struct hdmi_audio_infoframe))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    frame->type = HDMI_INFOFRAME_TYPE_AUDIO;
    frame->version = 1;
    frame->length = HDMI_AUDIO_INFOFRAME_SIZE;

    return 0;
}
EXPORT_SYMBOL(hdmi_audio_infoframe_init);

static hi_s32 hdmi_audio_infoframe_check_only(const struct hdmi_audio_infoframe *frame)
{
    if (frame->type != HDMI_INFOFRAME_TYPE_AUDIO ||
        frame->version != 1 ||
        frame->length != HDMI_AUDIO_INFOFRAME_SIZE) {
        return -EINVAL;
    }

    return 0;
}

/*
 * hdmi_audio_infoframe_check() - check a HDMI audio infoframe
 * @frame: HDMI audio infoframe
 *
 * Validates that the infoframe is consistent and updates derived fields
 * (eg. length) based on other fields.
 *
 * Returns 0 on success or a negative error code on failure.
 */
hi_s32 hdmi_audio_infoframe_check(struct hdmi_audio_infoframe *frame)
{
    return hdmi_audio_infoframe_check_only(frame);
}
EXPORT_SYMBOL(hdmi_audio_infoframe_check);

/*
 * hdmi_audio_infoframe_pack_only()-write HDMI audio infoframe to binary buffer
 * @frame: HDMI audio infoframe
 * @buffer: destination buffer
 * @size: size of buffer
 * Packs the information contained in the @frame structure into a binary
 * representation that can be written into the corresponding controller
 * registers. Also computes the checksum as required by section 5.3.5 of
 * the HDMI 1.4 specification.
 * Returns the number of bytes packed into the binary buffer or a negative
 * error code on failure.
 */
ssize_t hdmi_audio_infoframe_pack_only(const struct hdmi_audio_infoframe *frame,
                                       void *buffer, size_t size)
{
    hi_u8 channels;
    hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    ret = hdmi_audio_infoframe_check_only(frame);
    if (ret) {
        return ret;
    }

    length = HDMI_INFOFRAME_HEADER_SIZE + frame->length;

    if (size < length) {
        return -ENOSPC;
    }

    if (memset_s(buffer, size, 0, size)) {
        HDMI_ERR(KERN_ERR "[hdmitx] memset_s err\n");
        return HI_FAILURE;
    }

    if (frame->channels >= 2) {
        channels = frame->channels - 1;
    } else {
        channels = 0;
    }

    ptr[0] = frame->type;
    ptr[1] = frame->version;
    ptr[2] = frame->length;
    ptr[3] = 0; /* checksum */

    /* start infoframe payload */
    ptr += HDMI_INFOFRAME_HEADER_SIZE;

    ptr[0] = ((frame->coding_type & 0xf) << 4) | (channels & 0x7);
    ptr[1] = ((frame->sample_frequency & 0x7) << 2) |
             (frame->sample_size & 0x3);
    ptr[2] = frame->coding_type_ext & 0x1f;
    ptr[3] = frame->channel_allocation;
    ptr[4] = (frame->level_shift_value & 0xf) << 3;

    if (frame->downmix_inhibit) {
        ptr[4] |= BIT(7);
    }

    hdmi_infoframe_set_checksum(buffer, length);

    return length;
}
EXPORT_SYMBOL(hdmi_audio_infoframe_pack_only);

/*
 * hdmi_audio_infoframe_pack() - check a HDMI Audio infoframe,
 *                               and write it to binary buffer
 * @frame: HDMI Audio infoframe
 * @buffer: destination buffer
 * @size: size of buffer
 * Validates that the infoframe is consistent and updates derived fields
 * (eg. length) based on other fields, after which it packs the information
 * contained in the @frame structure into a binary representation that
 * can be written into the corresponding controller registers. This function
 * also computes the checksum as required by section 5.3.5 of the HDMI 1.4
 * specification.
 * Returns the number of bytes packed into the binary buffer or a negative
 * error code on failure.
 */
ssize_t hdmi_audio_infoframe_pack(struct hdmi_audio_infoframe *frame,
                                  void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_audio_infoframe_check(frame);
    if (ret) {
        return ret;
    }

    return hdmi_audio_infoframe_pack_only(frame, buffer, size);
}
EXPORT_SYMBOL(hdmi_audio_infoframe_pack);

/**
 * hdmi_vendor_infoframe_init() - initialize an HDMI vendor infoframe
 * @frame: HDMI vendor infoframe
 *
 * Returns 0 on success or a negative error code on failure.
 */
hi_s32 hdmi_vendor_infoframe_init(struct hdmi_vendor_infoframe *frame)
{
    if (memset_s(frame, sizeof(struct hdmi_vendor_infoframe),
                 0, sizeof(struct hdmi_vendor_infoframe))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    frame->type = HDMI_INFOFRAME_TYPE_VENDOR;
    frame->version = 1;

    frame->oui = HDMI_IEEE_OUI;

    /*
     * 0 is a valid value for s3d_struct, so we use a special "not set"
     * value
     */
    frame->s3d_struct = HDMI_3D_STRUCTURE_INVALID;
    frame->length = 4;

    return 0;
}
EXPORT_SYMBOL(hdmi_vendor_infoframe_init);

static hi_s32 hdmi_vendor_infoframe_check_only(const struct hdmi_vendor_infoframe *frame)
{
    if (frame->type != HDMI_INFOFRAME_TYPE_VENDOR ||
        frame->version != 1 ||
        frame->oui != HDMI_IEEE_OUI) {
        return -EINVAL;
    }

    /* only one of those can be supplied */
    if (frame->vic != 0 && frame->s3d_struct != HDMI_3D_STRUCTURE_INVALID) {
        return -EINVAL;
    }

    return 0;
}

/**
 * hdmi_vendor_infoframe_check() - check a HDMI vendor infoframe
 * @frame: HDMI infoframe
 *
 * Validates that the infoframe is consistent and updates derived fields
 * (eg. length) based on other fields.
 *
 * Returns 0 on success or a negative error code on failure.
 */
hi_s32 hdmi_vendor_infoframe_check(struct hdmi_vendor_infoframe *frame)
{
    return hdmi_vendor_infoframe_check_only(frame);
}
EXPORT_SYMBOL(hdmi_vendor_infoframe_check);

/**
 * hdmi_vendor_infoframe_pack_only() - write a HDMI vendor infoframe to binary buffer
 * @frame: HDMI infoframe
 * @buffer: destination buffer
 * @size: size of buffer
 *
 * Packs the information contained in the @frame structure into a binary
 * representation that can be written into the corresponding controller
 * registers. Also computes the checksum as required by section 5.3.5 of
 * the HDMI 1.4 specification.
 *
 * Returns the number of bytes packed into the binary buffer or a negative
 * error code on failure.
 */
ssize_t hdmi_vendor_infoframe_pack_only(const struct hdmi_vendor_infoframe *frame,
                                        void *buffer, size_t size)
{
    hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    ret = hdmi_vendor_infoframe_check_only(frame);
    if (ret) {
        return ret;
    }

    length = HDMI_INFOFRAME_HEADER_SIZE + frame->length;

    if (size < length) {
        return -ENOSPC;
    }

    if (memset_s(buffer, size, 0, size)) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    ptr[0] = frame->type;
    ptr[1] = frame->version;
    ptr[2] = frame->length;
    ptr[3] = 0; /* checksum */

    /* HDMI OUI */
    ptr[4] = 0x03;
    ptr[5] = 0x0c;
    ptr[6] = 0x00;

    if (frame->s3d_struct != HDMI_3D_STRUCTURE_INVALID) {
        ptr[7] = 0x2 << 5; /* video format */
        ptr[8] = (frame->s3d_struct & 0xf) << 4;
        if (frame->s3d_struct >= HDMI_3D_STRUCTURE_SIDE_BY_SIDE_HALF) {
            ptr[9] = (frame->s3d_ext_data & 0xf) << 4;
        }
    } else if (frame->vic) {
        ptr[7] = 0x1 << 5; /* video format */
        ptr[8] = frame->vic;
    } else {
        ptr[7] = 0x0 << 5; /* video format */
    }

    hdmi_infoframe_set_checksum(buffer, length);

    return length;
}
EXPORT_SYMBOL(hdmi_vendor_infoframe_pack_only);

/*
 * hdmi_vendor_infoframe_pack() - check a HDMI Vendor infoframe,
 *                                and write it to binary buffer
 * @frame: HDMI Vendor infoframe
 * @buffer: destination buffer
 * @size: size of buffer
 * Validates that the infoframe is consistent and updates derived fields
 * (eg. length) based on other fields, after which it packs the information
 * contained in the @frame structure into a binary representation that
 * can be written into the corresponding controller registers. This function
 * also computes the checksum as required by section 5.3.5 of the HDMI 1.4
 * specification.
 * Returns the number of bytes packed into the binary buffer or a negative
 * error code on failure.
 */
ssize_t hdmi_vendor_infoframe_pack(struct hdmi_vendor_infoframe *frame,
                                   void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_vendor_infoframe_check(frame);
    if (ret) {
        return ret;
    }

    return hdmi_vendor_infoframe_pack_only(frame, buffer, size);
}
EXPORT_SYMBOL(hdmi_vendor_infoframe_pack);

hi_s32 hdmi_forum_vendor_infoframe_init(struct hdmi_forum_vendor_infoframe *frame)
{
    if (memset_s(frame, sizeof(struct hdmi_forum_vendor_infoframe),
                 0, sizeof(struct hdmi_forum_vendor_infoframe))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    frame->type = HDMI_INFOFRAME_TYPE_VENDOR;
    frame->version = 1;

    frame->oui = HDMI_FORUM_IEEE_OUI;

    /*
     * 0 is a valid value for s3d_struct, so we use a special "not set"
     * value
     */
    frame->length = 4;

    return 0;
}
EXPORT_SYMBOL(hdmi_forum_vendor_infoframe_init);

hi_s32 hdmi_forum_vendor_infoframe_check_only(struct hdmi_forum_vendor_infoframe *frame)
{
    /* todo */
    return 0;
}

hi_s32 hdmi_forum_vendor_infoframe_check(struct hdmi_forum_vendor_infoframe *frame)
{
    /* todo */
    return 0;
}
EXPORT_SYMBOL(hdmi_forum_vendor_infoframe_check);

ssize_t hdmi_forum_vendor_infoframe_pack_only(const struct hdmi_forum_vendor_infoframe *frame,
                                              void *buffer, size_t size)
{
    /* todo */
    return 0;
}
EXPORT_SYMBOL(hdmi_forum_vendor_infoframe_pack_only);

ssize_t hdmi_forum_vendor_infoframe_pack(struct hdmi_forum_vendor_infoframe *frame,
                                         void *buffer, size_t size)
{
    /* todo */
    return 0;
}
EXPORT_SYMBOL(hdmi_forum_vendor_infoframe_pack);

hi_s32 hdmi_dolby_vendor_infoframe_init(struct hdmi_dolby_vendor_infoframe *frame)
{
    if (memset_s(frame, sizeof(struct hdmi_dolby_vendor_infoframe),
                 0, sizeof(struct hdmi_dolby_vendor_infoframe))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    frame->type = HDMI_INFOFRAME_TYPE_VENDOR;
    frame->version = 1;

    frame->oui = HDMI_DOLBY_IEEE_OUI;

    frame->length = 0x1b;

    return 0;
}
EXPORT_SYMBOL(hdmi_dolby_vendor_infoframe_init);

hi_s32 hdmi_dolby_vendor_infoframe_check_only(const struct hdmi_dolby_vendor_infoframe *frame)
{
    if (frame->type != HDMI_INFOFRAME_TYPE_VENDOR ||
        frame->version != 1 ||
        frame->oui != HDMI_DOLBY_IEEE_OUI) {
        return -EINVAL;
    }

    if (frame->length != 0x1b) {
        return -EINVAL;
    }

    return 0;
}

hi_s32 hdmi_dolby_vendor_infoframe_check(struct hdmi_dolby_vendor_infoframe *frame)
{
    return hdmi_dolby_vendor_infoframe_check_only(frame);
}
EXPORT_SYMBOL(hdmi_dolby_vendor_infoframe_check);

ssize_t hdmi_dolby_vendor_infoframe_pack_only(const struct hdmi_dolby_vendor_infoframe *frame,
                                              void *buffer, size_t size)
{
    hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    ret = hdmi_dolby_vendor_infoframe_check_only(frame);
    if (ret) {
        return ret;
    }

    length = HDMI_INFOFRAME_HEADER_SIZE + frame->length;

    if (size < length) {
        return -ENOSPC;
    }

    if (memset_s(buffer, size, 0, size)) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    ptr[0] = frame->type;
    ptr[1] = frame->version;
    ptr[2] = frame->length;
    ptr[3] = 0; /* checksum */

    /* IEEE OUI */
    ptr[4] = frame->oui & 0xff;
    ptr[5] = (frame->oui >> 8) & 0xff;
    ptr[6] = (frame->oui >> 16) & 0xff;

    /* payload by dolbyvision */
    ptr[7] = ((frame->dolbyvision_signal & 0x1) << 1) | (frame->low_latency & 0x1);
    ptr[8] = ((frame->backlt_ctrl_md_present & 0x1) << 7) |
             ((frame->auxiliary_md_present & 0x1) << 6) |
             (frame->eff_tmax_pq_hi & 0xf);
    ptr[9] = frame->eff_tmax_pq_low;
    ptr[10] = frame->auxiliary_runmode;
    ptr[11] = frame->auxiliary_runversion;
    ptr[12] = frame->auxiliary_debug;

    hdmi_infoframe_set_checksum(buffer, length);

    return length;
}
EXPORT_SYMBOL(hdmi_dolby_vendor_infoframe_pack_only);

ssize_t hdmi_dolby_vendor_infoframe_pack(struct hdmi_dolby_vendor_infoframe *frame,
                                         void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_dolby_vendor_infoframe_check(frame);
    if (ret) {
        return ret;
    }

    return hdmi_dolby_vendor_infoframe_pack_only(frame, buffer, size);
}
EXPORT_SYMBOL(hdmi_dolby_vendor_infoframe_pack);

hi_s32 hdmi_drm_infoframe_init(struct hdmi_drm_infoframe *frame)
{
    if (memset_s(frame, sizeof(struct hdmi_drm_infoframe),
                 0, sizeof(struct hdmi_drm_infoframe))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    frame->type = HDMI_INFOFRAME_TYPE_DRM;
    frame->version = 1;

    frame->length = HDMI_DRM_INFOFRAME_SIZE;

    return 0;
}
EXPORT_SYMBOL(hdmi_drm_infoframe_init);

hi_s32 hdmi_drm_infoframe_check_only(const struct hdmi_drm_infoframe *frame)
{
    if (frame->type != HDMI_INFOFRAME_TYPE_DRM ||
        frame->version != 1) {
        return -EINVAL;
    }

    if (frame->length != HDMI_DRM_INFOFRAME_SIZE) {
        return -EINVAL;
    }

    if (frame->eotf_type > HDMI_EOTF_HLG ||
        frame->metadata_type != HDMI_HDR_METADATA_ID_0) {
        return -EINVAL;
    }

    return 0;
}

hi_s32 hdmi_drm_infoframe_check(struct hdmi_drm_infoframe *frame)
{
    return hdmi_drm_infoframe_check_only(frame);
}
EXPORT_SYMBOL(hdmi_drm_infoframe_check);

ssize_t hdmi_drm_infoframe_pack_only(const struct hdmi_drm_infoframe *frame,
                                     void *buffer, size_t size)
{
    hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;
    const struct hdmi_meta_descriptor_1st *descriptor_1st = HI_NULL;

    ret = hdmi_drm_infoframe_check_only(frame);
    if (ret) {
        return ret;
    }

    descriptor_1st = &frame->meta_descriptor.type1;

    length = HDMI_INFOFRAME_HEADER_SIZE + frame->length;

    if (size < length) {
        return -ENOSPC;
    }

    if (memset_s(buffer, size, 0, size)) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    ptr[0] = frame->type;
    ptr[1] = frame->version;
    ptr[2] = frame->length;
    ptr[3] = 0; /* checksum */

    ptr[4] = frame->eotf_type;
    ptr[5] = frame->metadata_type;

    ptr[6] = descriptor_1st->primaries0_x & 0xff;
    ptr[7] = (descriptor_1st->primaries0_x >> 8) & 0xff;
    ptr[8] = descriptor_1st->primaries0_y & 0xff;
    ptr[9] = (descriptor_1st->primaries0_y >> 8) & 0xff;

    ptr[10] = descriptor_1st->primaries1_x & 0xff;
    ptr[11] = (descriptor_1st->primaries1_x >> 8) & 0xff;
    ptr[12] = descriptor_1st->primaries1_y & 0xff;
    ptr[13] = (descriptor_1st->primaries1_y >> 8) & 0xff;

    ptr[14] = descriptor_1st->primaries2_x & 0xff;
    ptr[15] = (descriptor_1st->primaries2_x >> 8) & 0xff;
    ptr[16] = descriptor_1st->primaries2_y & 0xff;
    ptr[17] = (descriptor_1st->primaries2_y >> 8) & 0xff;

    ptr[18] = descriptor_1st->white_point_x & 0xff;
    ptr[19] = (descriptor_1st->white_point_x >> 8) & 0xff;
    ptr[20] = descriptor_1st->white_point_y & 0xff;
    ptr[21] = (descriptor_1st->white_point_y >> 8) & 0xff;

    ptr[22] = descriptor_1st->max_luminance & 0xff;
    ptr[23] = (descriptor_1st->max_luminance >> 8) & 0xff;
    ptr[24] = descriptor_1st->min_luminance & 0xff;
    ptr[25] = (descriptor_1st->min_luminance >> 8) & 0xff;

    ptr[26] = descriptor_1st->max_light_level & 0xff;
    ptr[27] = (descriptor_1st->max_light_level >> 8) & 0xff;

    ptr[28] = descriptor_1st->average_light_level & 0xff;
    ptr[29] = (descriptor_1st->average_light_level >> 8) & 0xff;

    hdmi_infoframe_set_checksum(buffer, length);

    return length;
}
EXPORT_SYMBOL(hdmi_drm_infoframe_pack_only);

ssize_t hdmi_drm_infoframe_pack(struct hdmi_drm_infoframe *frame,
                                void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_drm_infoframe_check(frame);
    if (ret) {
        return ret;
    }

    return hdmi_drm_infoframe_pack_only(frame, buffer, size);
}
EXPORT_SYMBOL(hdmi_drm_infoframe_pack);

static hi_s32 hdmi_vendor_any_infoframe_check_only(const union hdmi_vendor_any_infoframe *frame)
{
    if (frame->any.type != HDMI_INFOFRAME_TYPE_VENDOR ||
        frame->any.version != 1) {
        return -EINVAL;
    }

    return 0;
}

/*
 * hdmi_vendor_any_infoframe_check() - check a vendor infoframe
 */
static hi_s32 hdmi_vendor_any_infoframe_check(union hdmi_vendor_any_infoframe *frame)
{
    hi_s32 ret;

    ret = hdmi_vendor_any_infoframe_check_only(frame);
    if (ret) {
        return ret;
    }

    if (frame->any.oui == HDMI_IEEE_OUI) {
        ret = hdmi_vendor_infoframe_check(&frame->hdmi);
    } else if (frame->any.oui == HDMI_FORUM_IEEE_OUI) {
        ret = hdmi_forum_vendor_infoframe_check(&frame->hdmi_forum);
    } else if (frame->any.oui == HDMI_DOLBY_IEEE_OUI) {
        ret = hdmi_dolby_vendor_infoframe_check(&frame->hdmi_dolby);
    } else {
        return -EINVAL;
    }

    return ret;
}

/*
 * hdmi_vendor_any_infoframe_pack_only() - write a vendor infoframe to binary buffer
 */
static ssize_t hdmi_vendor_any_infoframe_pack_only(const union hdmi_vendor_any_infoframe *frame,
                                                   void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_vendor_any_infoframe_check_only(frame);
    if (ret) {
        return ret;
    }

    if (frame->any.oui == HDMI_IEEE_OUI) {
        ret = hdmi_vendor_infoframe_pack_only(&frame->hdmi, buffer, size);
    } else if (frame->any.oui == HDMI_FORUM_IEEE_OUI) {
        ret = hdmi_forum_vendor_infoframe_pack_only(&frame->hdmi_forum, buffer, size);
    } else if (frame->any.oui == HDMI_DOLBY_IEEE_OUI) {
        ret = hdmi_dolby_vendor_infoframe_pack_only(&frame->hdmi_dolby, buffer, size);
    } else {
        return -EINVAL;
    }

    return ret;
}

/* check a vendor infoframe, and write it to binary buffer */
static ssize_t hdmi_vendor_any_infoframe_pack(union hdmi_vendor_any_infoframe *frame,
                                              void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_vendor_any_infoframe_check(frame);
    if (ret) {
        return ret;
    }

    return hdmi_vendor_any_infoframe_pack_only(frame, buffer, size);
}

/*
 * hdmi_infoframe_check() - check a HDMI infoframe
 * @frame: HDMI infoframe
 * Validates that the infoframe is consistent and updates derived fields
 * (eg. length) based on other fields.
 *
 * Returns 0 on success or a negative error code on failure.
 */
hi_s32 hdmi_infoframe_check(union hdmi_infoframe *frame)
{
    switch (frame->any.type) {
        case HDMI_INFOFRAME_TYPE_AVI:
            return hdmi_avi_infoframe_check(&frame->avi);
        case HDMI_INFOFRAME_TYPE_SPD:
            return hdmi_spd_infoframe_check(&frame->spd);
        case HDMI_INFOFRAME_TYPE_AUDIO:
            return hdmi_audio_infoframe_check(&frame->audio);
        case HDMI_INFOFRAME_TYPE_VENDOR:
            return hdmi_vendor_any_infoframe_check(&frame->vendor);
        case HDMI_INFOFRAME_TYPE_DRM:
            return hdmi_drm_infoframe_check(&frame->drm);
        default:
            HDMI_WARN("Bad infoframe type %d\n", frame->any.type);
            return -EINVAL;
    }
}
EXPORT_SYMBOL(hdmi_infoframe_check);

/*
 * hdmi_infoframe_pack_only() - write a HDMI infoframe to binary buffer
 * @frame: HDMI infoframe
 * @buffer: destination buffer
 * @size: size of buffer
 * Packs the information contained in the @frame structure into a binary
 * representation that can be written into the corresponding controller
 * registers. Also computes the checksum as required by section 5.3.5 of
 * the HDMI 1.4 specification.
 * Returns the number of bytes packed into the binary buffer or a negative
 * error code on failure.
 */
ssize_t hdmi_infoframe_pack_only(const union hdmi_infoframe *frame, void *buffer, size_t size)
{
    ssize_t length;

    switch (frame->any.type) {
        case HDMI_INFOFRAME_TYPE_AVI:
            length = hdmi_avi_infoframe_pack_only(&frame->avi,
                                                  buffer, size);
            break;
        case HDMI_INFOFRAME_TYPE_SPD:
            length = hdmi_spd_infoframe_pack_only(&frame->spd,
                                                  buffer, size);
            break;
        case HDMI_INFOFRAME_TYPE_AUDIO:
            length = hdmi_audio_infoframe_pack_only(&frame->audio,
                                                    buffer, size);
            break;
        case HDMI_INFOFRAME_TYPE_VENDOR:
            length = hdmi_vendor_any_infoframe_pack_only(&frame->vendor,
                                                         buffer, size);
            break;
        case HDMI_INFOFRAME_TYPE_DRM:
            length = hdmi_drm_infoframe_pack_only(&frame->drm,
                                                  buffer, size);
            break;
        default:
            HDMI_WARN("Bad infoframe type %d\n", frame->any.type);
            length = -EINVAL;
    }

    return length;
}
EXPORT_SYMBOL(hdmi_infoframe_pack_only);

/*
 * hdmi_infoframe_pack() - check a HDMI infoframe,
 *                         and write it to binary buffer
 * @frame: HDMI infoframe
 * @buffer: destination buffer
 * @size: size of buffer
 * Validates that the infoframe is consistent and updates derived fields
 * (eg. length) based on other fields, after which it packs the information
 * contained in the @frame structure into a binary representation that
 * can be written into the corresponding controller registers. This function
 * also computes the checksum as required by section 5.3.5 of the HDMI 1.4
 * specification.
 * Returns the number of bytes packed into the binary buffer or a negative
 * error code on failure.
 */
ssize_t hdmi_infoframe_pack(union hdmi_infoframe *frame,
                            void *buffer, size_t size)
{
    ssize_t length;

    switch (frame->any.type) {
        case HDMI_INFOFRAME_TYPE_AVI:
            length = hdmi_avi_infoframe_pack(&frame->avi, buffer, size);
            break;
        case HDMI_INFOFRAME_TYPE_SPD:
            length = hdmi_spd_infoframe_pack(&frame->spd, buffer, size);
            break;
        case HDMI_INFOFRAME_TYPE_AUDIO:
            length = hdmi_audio_infoframe_pack(&frame->audio, buffer, size);
            break;
        case HDMI_INFOFRAME_TYPE_VENDOR:
            length = hdmi_vendor_any_infoframe_pack(&frame->vendor,
                                                    buffer, size);
            break;
        case HDMI_INFOFRAME_TYPE_DRM:
            length = hdmi_drm_infoframe_pack(&frame->drm,
                                             buffer, size);
            break;
        default:
            HDMI_WARN("Bad infoframe type %d\n", frame->any.type);
            length = -EINVAL;
    }

    return length;
}
EXPORT_SYMBOL(hdmi_infoframe_pack);

static const hi_s8 *hdmi_infoframe_type_get_name(enum hdmi_infoframe_type type)
{
    if (type < 0x80 || type > 0x9f) {
        return "Invalid";
    }
    switch (type) {
        case HDMI_INFOFRAME_TYPE_VENDOR:
            return "Vendor";
        case HDMI_INFOFRAME_TYPE_AVI:
            return "Auxiliary Video Information (AVI)";
        case HDMI_INFOFRAME_TYPE_SPD:
            return "Source Product Description (SPD)";
        case HDMI_INFOFRAME_TYPE_AUDIO:
            return "Audio";
        case HDMI_INFOFRAME_TYPE_DRM:
            return "Dynamic Range and Mastering (DRM)";
    }
    return "Reserved";
}

static void hdmi_infoframe_log_header(const struct hdmi_any_infoframe *frame)
{
    const hi_s8 *name = HI_NULL;

    name = hdmi_infoframe_type_get_name(frame->type);
    HDMI_INFO("HDMI infoframe: %s, version %u, length %u\n", name, frame->version, frame->length);
}

static const hi_s8 *hdmi_colorspace_get_name(enum hdmi_colorspace colorspace)
{
    switch (colorspace) {
        case HDMI_COLORSPACE_RGB:
            return "RGB";
        case HDMI_COLORSPACE_YUV422:
            return "YCbCr 4:2:2";
        case HDMI_COLORSPACE_YUV444:
            return "YCbCr 4:4:4";
        case HDMI_COLORSPACE_YUV420:
            return "YCbCr 4:2:0";
        case HDMI_COLORSPACE_RESERVED4:
            return "Reserved (4)";
        case HDMI_COLORSPACE_RESERVED5:
            return "Reserved (5)";
        case HDMI_COLORSPACE_RESERVED6:
            return "Reserved (6)";
        case HDMI_COLORSPACE_IDO_DEFINED:
            return "IDO Defined";
    }
    return "Invalid";
}

static const hi_s8 *hdmi_scan_mode_get_name(enum hdmi_scan_mode scan_mode)
{
    switch (scan_mode) {
        case HDMI_SCAN_MODE_NONE:
            return "No Data";
        case HDMI_SCAN_MODE_OVERSCAN:
            return "Overscan";
        case HDMI_SCAN_MODE_UNDERSCAN:
            return "Underscan";
        case HDMI_SCAN_MODE_RESERVED:
            return "Reserved";
    }
    return "Invalid";
}

static const hi_s8 *hdmi_colorimetry_get_name(enum hdmi_colorimetry colorimetry)
{
    switch (colorimetry) {
        case HDMI_COLORIMETRY_NONE:
            return "No Data";
        case HDMI_COLORIMETRY_ITU_601:
            return "ITU601";
        case HDMI_COLORIMETRY_ITU_709:
            return "ITU709";
        case HDMI_COLORIMETRY_EXTENDED:
            return "Extended";
    }
    return "Invalid";
}

static const hi_s8 *hdmi_picture_aspect_get_name(enum hdmi_picture_aspect picture_aspect)
{
    switch (picture_aspect) {
        case HDMI_PICTURE_ASPECT_NONE:
            return "No Data";
        case HDMI_PICTURE_ASPECT_4_3:
            return "4:3";
        case HDMI_PICTURE_ASPECT_16_9:
            return "16:9";
        case HDMI_PICTURE_ASPECT_64_27:
            return "64:27";
        case HDMI_PICTURE_ASPECT_256_135:
            return "256:135";
        case HDMI_PICTURE_ASPECT_RESERVED:
            return "Reserved";
    }
    return "Invalid";
}

static const hi_s8 *hdmi_active_aspect_get_name(enum hdmi_active_aspect active_aspect)
{
    if (active_aspect < 0 || active_aspect > 0xf) {
        return "Invalid";
    }

    switch (active_aspect) {
        case HDMI_ACTIVE_ASPECT_16_9_TOP:
            return "16:9 Top";
        case HDMI_ACTIVE_ASPECT_14_9_TOP:
            return "14:9 Top";
        case HDMI_ACTIVE_ASPECT_16_9_CENTER:
            return "16:9 Center";
        case HDMI_ACTIVE_ASPECT_PICTURE:
            return "Same as Picture";
        case HDMI_ACTIVE_ASPECT_4_3:
            return "4:3";
        case HDMI_ACTIVE_ASPECT_16_9:
            return "16:9";
        case HDMI_ACTIVE_ASPECT_14_9:
            return "14:9";
        case HDMI_ACTIVE_ASPECT_4_3_SP_14_9:
            return "4:3 SP 14:9";
        case HDMI_ACTIVE_ASPECT_16_9_SP_14_9:
            return "16:9 SP 14:9";
        case HDMI_ACTIVE_ASPECT_16_9_SP_4_3:
            return "16:9 SP 4:3";
    }
    return "Reserved";
}

static const hi_s8 *hdmi_extended_colorimetry_get_name(enum hdmi_extended_colorimetry ext_col)
{
    switch (ext_col) {
        case HDMI_EXTENDED_COLORIMETRY_XV_YCC_601:
            return "xvYCC 601";
        case HDMI_EXTENDED_COLORIMETRY_XV_YCC_709:
            return "xvYCC 709";
        case HDMI_EXTENDED_COLORIMETRY_S_YCC_601:
            return "sYCC 601";
        case HDMI_EXTENDED_COLORIMETRY_OPYCC_601:
            return "opYCC 601";
        case HDMI_EXTENDED_COLORIMETRY_OPRGB:
            return "opRGB";
        case HDMI_EXTENDED_COLORIMETRY_BT2020_CONST_LUM:
            return "BT.2020 Constant Luminance";
        case HDMI_EXTENDED_COLORIMETRY_BT2020:
            return "BT.2020";
        case HDMI_EXTENDED_COLORIMETRY_RESERVED:
            return "Reserved";
    }
    return "Invalid";
}

static const hi_s8 *hdmi_quantization_range_get_name(enum hdmi_quantization_range qrange)
{
    switch (qrange) {
        case HDMI_QUANTIZATION_RANGE_DEFAULT:
            return "Default";
        case HDMI_QUANTIZATION_RANGE_LIMITED:
            return "Limited";
        case HDMI_QUANTIZATION_RANGE_FULL:
            return "Full";
        case HDMI_QUANTIZATION_RANGE_RESERVED:
            return "Reserved";
    }
    return "Invalid";
}

static const hi_s8 *hdmi_nups_get_name(enum hdmi_nups nups)
{
    switch (nups) {
        case HDMI_NUPS_UNKNOWN:
            return "Unknown Non-uniform Scaling";
        case HDMI_NUPS_HORIZONTAL:
            return "Horizontally Scaled";
        case HDMI_NUPS_VERTICAL:
            return "Vertically Scaled";
        case HDMI_NUPS_BOTH:
            return "Horizontally and Vertically Scaled";
    }
    return "Invalid";
}

static const hi_s8 *hdmi_ycc_quantization_range_get_name(enum hdmi_ycc_quantization_range qrange)
{
    switch (qrange) {
        case HDMI_YCC_QUANTIZATION_RANGE_LIMITED:
            return "Limited";
        case HDMI_YCC_QUANTIZATION_RANGE_FULL:
            return "Full";
    }
    return "Invalid";
}

static const hi_s8 *hdmi_content_type_get_name(enum hdmi_content_type content_type)
{
    switch (content_type) {
        case HDMI_CONTENT_TYPE_GRAPHICS:
            return "Graphics";
        case HDMI_CONTENT_TYPE_PHOTO:
            return "Photo";
        case HDMI_CONTENT_TYPE_CINEMA:
            return "Cinema";
        case HDMI_CONTENT_TYPE_GAME:
            return "Game";
    }
    return "Invalid";
}

/*
 * hdmi_avi_infoframe_log() - log info of HDMI AVI infoframe
 * @level: logging level
 * @dev: device
 * @frame: HDMI AVI infoframe
 */
static void hdmi_avi_infoframe_log(const struct hdmi_avi_infoframe *frame)
{
    const hi_s8 *colorspace_name = HI_NULL;
    const hi_s8 *scan_mode_name = HI_NULL;
    const hi_s8 *colorimetry_name = HI_NULL;
    const hi_s8 *picture_aspect_name = HI_NULL;
    const hi_s8 *active_aspect_name = HI_NULL;
    const hi_s8 *extended_colorimetry_name = HI_NULL;
    const hi_s8 *quantization_range_name = HI_NULL;
    const hi_s8 *nups_name = HI_NULL;
    const hi_s8 *ycc_quantization_range_name = HI_NULL;
    const hi_s8 *content_type_name = HI_NULL;

    colorspace_name = hdmi_colorspace_get_name(frame->colorspace);
    scan_mode_name = hdmi_scan_mode_get_name(frame->scan_mode);
    colorimetry_name = hdmi_colorimetry_get_name(frame->colorimetry);
    picture_aspect_name = hdmi_picture_aspect_get_name(frame->picture_aspect);
    active_aspect_name = hdmi_active_aspect_get_name(frame->active_aspect);
    extended_colorimetry_name = hdmi_extended_colorimetry_get_name(frame->extended_colorimetry);
    quantization_range_name = hdmi_quantization_range_get_name(frame->quantization_range);
    nups_name = hdmi_nups_get_name(frame->nups);
    ycc_quantization_range_name = hdmi_ycc_quantization_range_get_name(frame->ycc_quantization_range);
    content_type_name = hdmi_content_type_get_name(frame->content_type);

    hdmi_infoframe_log_header((const struct hdmi_any_infoframe *)frame);

    HDMI_INFO("   colorspace: %s\n", colorspace_name);
    HDMI_INFO("   scan mode: %s\n", scan_mode_name);
    HDMI_INFO("   colorimetry: %s\n", colorimetry_name);
    HDMI_INFO("   picture aspect: %s\n", picture_aspect_name);
    HDMI_INFO("   active aspect: %s\n", active_aspect_name);
    HDMI_INFO("   itc: %s\n", frame->itc ? "IT Content" : "No Data");
    HDMI_INFO("   extended colorimetry: %s\n", extended_colorimetry_name);
    HDMI_INFO("   quantization range: %s\n", quantization_range_name);
    HDMI_INFO("   nups: %s\n", nups_name);
    HDMI_INFO("   video code: %u\n", frame->video_code);
    HDMI_INFO("   ycc quantization range: %s\n", ycc_quantization_range_name);
    HDMI_INFO(" hdmi content type: %s\n", content_type_name);
    HDMI_INFO("   pixel repeat: %u\n", frame->pixel_repeat);
    HDMI_INFO("   bar top %u, bottom %u, left %u, right %u\n",
              frame->top_bar, frame->bottom_bar,
              frame->left_bar, frame->right_bar);
}

static const hi_s8 *hdmi_spd_sdi_get_name(enum hdmi_spd_sdi sdi)
{
    if (sdi < 0 || sdi > 0xff) {
        return "Invalid";
    }
    switch (sdi) {
        case HDMI_SPD_SDI_UNKNOWN:
            return "Unknown";
        case HDMI_SPD_SDI_DSTB:
            return "Digital STB";
        case HDMI_SPD_SDI_DVDP:
            return "DVD Player";
        case HDMI_SPD_SDI_DVHS:
            return "D-VHS";
        case HDMI_SPD_SDI_HDDVR:
            return "HDD Videorecorder";
        case HDMI_SPD_SDI_DVC:
            return "DVC";
        case HDMI_SPD_SDI_DSC:
            return "DSC";
        case HDMI_SPD_SDI_VCD:
            return "Video CD";
        case HDMI_SPD_SDI_GAME:
            return "Game";
        case HDMI_SPD_SDI_PC:
            return "PC General";
        case HDMI_SPD_SDI_BD:
            return "Blu-Ray Disc (BD)";
        case HDMI_SPD_SDI_SACD:
            return "Super Audio CD";
        case HDMI_SPD_SDI_HDDVD:
            return "HD DVD";
        case HDMI_SPD_SDI_PMP:
            return "PMP";
    }
    return "Reserved";
}

/*
 * hdmi_spd_infoframe_log() - log info of HDMI SPD infoframe
 * @level: logging level
 * @dev: device
 * @frame: HDMI SPD infoframe
 */
static void hdmi_spd_infoframe_log(const struct hdmi_spd_infoframe *frame)
{
    hi_u8 buf[17] = {0};
    const hi_s8 *name = HI_NULL;

    name = hdmi_spd_sdi_get_name(frame->sdi);
    hdmi_infoframe_log_header((const struct hdmi_any_infoframe *)frame);

    if (strncpy_s(buf, sizeof(buf), frame->vendor, 8)) { /* 8: vendor len */
        HDMI_ERR("strncpy_s err\n");
        return;
    }
    HDMI_INFO("   vendor: %s\n", buf);
    if (strncpy_s(buf, sizeof(buf), frame->product, 16)) { /* 16: product len */
        HDMI_ERR("strncpy_s err\n");
        return;
    }
    HDMI_INFO("   product: %s\n", buf);
    HDMI_INFO("   source device information: %s (0x%x)\n", name, frame->sdi);
}

static const hi_s8 *hdmi_audio_coding_type_get_name(enum hdmi_audio_coding_type coding_type)
{
    switch (coding_type) {
        case HDMI_AUDIO_CODING_TYPE_STREAM:
            return "Refer to Stream Header";
        case HDMI_AUDIO_CODING_TYPE_PCM:
            return "PCM";
        case HDMI_AUDIO_CODING_TYPE_AC3:
            return "AC-3";
        case HDMI_AUDIO_CODING_TYPE_MPEG1:
            return "MPEG1";
        case HDMI_AUDIO_CODING_TYPE_MP3:
            return "MP3";
        case HDMI_AUDIO_CODING_TYPE_MPEG2:
            return "MPEG2";
        case HDMI_AUDIO_CODING_TYPE_AAC_LC:
            return "AAC";
        case HDMI_AUDIO_CODING_TYPE_DTS:
            return "DTS";
        case HDMI_AUDIO_CODING_TYPE_ATRAC:
            return "ATRAC";
        case HDMI_AUDIO_CODING_TYPE_DSD:
            return "One Bit Audio";
        case HDMI_AUDIO_CODING_TYPE_EAC3:
            return "Dolby Digital +";
        case HDMI_AUDIO_CODING_TYPE_DTS_HD:
            return "DTS-HD";
        case HDMI_AUDIO_CODING_TYPE_MLP:
            return "MAT (MLP)";
        case HDMI_AUDIO_CODING_TYPE_DST:
            return "DST";
        case HDMI_AUDIO_CODING_TYPE_WMA_PRO:
            return "WMA PRO";
        case HDMI_AUDIO_CODING_TYPE_CXT:
            return "Refer to CXT";
    }
    return "Invalid";
}

static const hi_s8 *hdmi_audio_sample_size_get_name(enum hdmi_audio_sample_size sample_size)
{
    switch (sample_size) {
        case HDMI_AUDIO_SAMPLE_SIZE_STREAM:
            return "Refer to Stream Header";
        case HDMI_AUDIO_SAMPLE_SIZE_16:
            return "16 bit";
        case HDMI_AUDIO_SAMPLE_SIZE_20:
            return "20 bit";
        case HDMI_AUDIO_SAMPLE_SIZE_24:
            return "24 bit";
    }
    return "Invalid";
}

static const hi_s8 *hdmi_audio_sample_frequency_get_name(enum hdmi_audio_sample_frequency freq)
{
    switch (freq) {
        case HDMI_AUDIO_SAMPLE_FREQUENCY_STREAM:
            return "Refer to Stream Header";
        case HDMI_AUDIO_SAMPLE_FREQUENCY_32000:
            return "32 kHz";
        case HDMI_AUDIO_SAMPLE_FREQUENCY_44100:
            return "44.1 kHz (CD)";
        case HDMI_AUDIO_SAMPLE_FREQUENCY_48000:
            return "48 kHz";
        case HDMI_AUDIO_SAMPLE_FREQUENCY_88200:
            return "88.2 kHz";
        case HDMI_AUDIO_SAMPLE_FREQUENCY_96000:
            return "96 kHz";
        case HDMI_AUDIO_SAMPLE_FREQUENCY_176400:
            return "176.4 kHz";
        case HDMI_AUDIO_SAMPLE_FREQUENCY_192000:
            return "192 kHz";
    }
    return "Invalid";
}

static const hi_s8 *hdmi_audio_coding_type_ext_get_name(enum hdmi_audio_coding_type_ext ctx)
{
    if (ctx < 0 || ctx > 0x1f) {
        return "Invalid";
    }

    switch (ctx) {
        case HDMI_AUDIO_CODING_TYPE_EXT_CT:
            return "Refer to CT";
        case HDMI_AUDIO_CODING_TYPE_EXT_HE_AAC:
            return "HE AAC";
        case HDMI_AUDIO_CODING_TYPE_EXT_HE_AAC_V2:
            return "HE AAC v2";
        case HDMI_AUDIO_CODING_TYPE_EXT_MPEG_SURROUND:
            return "MPEG SURROUND";
        case HDMI_AUDIO_CODING_TYPE_EXT_MPEG4_HE_AAC:
            return "MPEG-4 HE AAC";
        case HDMI_AUDIO_CODING_TYPE_EXT_MPEG4_HE_AAC_V2:
            return "MPEG-4 HE AAC v2";
        case HDMI_AUDIO_CODING_TYPE_EXT_MPEG4_AAC_LC:
            return "MPEG-4 AAC LC";
        case HDMI_AUDIO_CODING_TYPE_EXT_DRA:
            return "DRA";
        case HDMI_AUDIO_CODING_TYPE_EXT_MPEG4_HE_AAC_SURROUND:
            return "MPEG-4 HE AAC + MPEG Surround";
        case HDMI_AUDIO_CODING_TYPE_EXT_MPEG4_AAC_LC_SURROUND:
            return "MPEG-4 AAC LC + MPEG Surround";
    }
    return "Reserved";
}

/*
 * hdmi_audio_infoframe_log() - log info of HDMI AUDIO infoframe
 * @level: logging level
 * @dev: device
 * @frame: HDMI AUDIO infoframe
 */
static void hdmi_audio_infoframe_log(const struct hdmi_audio_infoframe *frame)
{
    const hi_s8 *coding_type_name = HI_NULL;
    const hi_s8 *sample_size_name = HI_NULL;
    const hi_s8 *sample_frequency_name = HI_NULL;
    const hi_s8 *coding_type_ext_name = HI_NULL;

    coding_type_name = hdmi_audio_coding_type_get_name(frame->coding_type);
    sample_size_name = hdmi_audio_sample_size_get_name(frame->sample_size);
    sample_frequency_name = hdmi_audio_sample_frequency_get_name(frame->sample_frequency);
    coding_type_ext_name = hdmi_audio_coding_type_ext_get_name(frame->coding_type_ext);

    hdmi_infoframe_log_header((const struct hdmi_any_infoframe *)frame);

    if (frame->channels) {
        HDMI_INFO("   channels: %u\n", frame->channels - 1);
    } else {
        HDMI_INFO("   channels: Refer to stream header\n");
    }
    HDMI_INFO("   coding type: %s\n", coding_type_name);
    HDMI_INFO("   sample size: %s\n", sample_size_name);
    HDMI_INFO("   sample frequency: %s\n", sample_frequency_name);
    HDMI_INFO("   coding type ext: %s\n", coding_type_ext_name);
    HDMI_INFO("   channel allocation: 0x%x\n", frame->channel_allocation);
    HDMI_INFO("   level shift value: %u dB\n", frame->level_shift_value);
    HDMI_INFO("   downmix inhibit: %s\n", frame->downmix_inhibit ? "Yes" : "No");
}

static const hi_s8 *hdmi_3d_structure_get_name(enum hdmi_3d_structure s3d_struct)
{
    if (s3d_struct < 0 || s3d_struct > 0xf) {
        return "Invalid";
    }

    switch (s3d_struct) {
        case HDMI_3D_STRUCTURE_FRAME_PACKING:
            return "Frame Packing";
        case HDMI_3D_STRUCTURE_FIELD_ALTERNATIVE:
            return "Field Alternative";
        case HDMI_3D_STRUCTURE_LINE_ALTERNATIVE:
            return "Line Alternative";
        case HDMI_3D_STRUCTURE_SIDE_BY_SIDE_FULL:
            return "Side-by-side (Full)";
        case HDMI_3D_STRUCTURE_L_DEPTH:
            return "L + Depth";
        case HDMI_3D_STRUCTURE_L_DEPTH_GFX_GFX_DEPTH:
            return "L + Depth + Graphics + Graphics-depth";
        case HDMI_3D_STRUCTURE_TOP_AND_BOTTOM:
            return "Top-and-Bottom";
        case HDMI_3D_STRUCTURE_SIDE_BY_SIDE_HALF:
            return "Side-by-side (Half)";
        default:
            break;
    }
    return "Reserved";
}

static void hdmi_vendor_infoframe_log(const struct hdmi_vendor_infoframe *frame)
{
    const hi_s8 *name = HI_NULL;

    name = hdmi_3d_structure_get_name(frame->s3d_struct);
    hdmi_infoframe_log_header((const struct hdmi_any_infoframe *)frame);

    if (frame->oui != HDMI_IEEE_OUI) {
        HDMI_INFO("   not a HDMI vendor infoframe\n");
        return;
    }

    HDMI_INFO("   this is a HDMI vendor infoframe\n");
    if (frame->vic == 0 && frame->s3d_struct == HDMI_3D_STRUCTURE_INVALID) {
        HDMI_INFO(" empty frame\n");
        return;
    }

    if (frame->vic) {
        HDMI_INFO(" hdmi VIC: %u\n", frame->vic);
    }
    if (frame->s3d_struct != HDMI_3D_STRUCTURE_INVALID) {
        HDMI_INFO("   3D structure: %s\n", name);
        if (frame->s3d_struct >= HDMI_3D_STRUCTURE_SIDE_BY_SIDE_HALF)
            HDMI_INFO("   3D extension data: %d\n", frame->s3d_ext_data);
    }
}

static void hdmi_forum_vendor_infoframe_log(const struct hdmi_forum_vendor_infoframe *frame)
{
    hdmi_infoframe_log_header((const struct hdmi_any_infoframe *)frame);

    if (frame->oui != HDMI_FORUM_IEEE_OUI) {
        HDMI_INFO("   not a HDMI forum vendor infoframe\n");
        return;
    }

    HDMI_INFO("   this is a HDMI forum vendor infoframe\n");
    /* todo */
}

static void hdmi_dolby_vendor_infoframe_log(const struct hdmi_dolby_vendor_infoframe *frame)
{
    hdmi_infoframe_log_header((const struct hdmi_any_infoframe *)frame);

    if (frame->oui != HDMI_DOLBY_IEEE_OUI) {
        HDMI_INFO("   not a HDMI dolby vendor infoframe\n");
        return;
    }

    HDMI_INFO("   this is a HDMI dolby vendor infoframe\n");
    HDMI_INFO("   low_latency: %s\n", frame->low_latency ? "TRUE" : "FALSE");
    HDMI_INFO("   dolbyvision_signal: %s\n", frame->dolbyvision_signal ? "TRUE" : "FALSE");
    HDMI_INFO("   backlt_ctrl_md_present: %s\n", frame->backlt_ctrl_md_present ? "TRUE" : "FALSE");
    HDMI_INFO("   auxiliary_md_present: %s\n", frame->auxiliary_md_present ? "TRUE" : "FALSE");
    HDMI_INFO("   eff_tmax_pq_hi: %u\n", frame->eff_tmax_pq_hi);
    HDMI_INFO("   eff_tmax_pq_low: %u\n", frame->eff_tmax_pq_low);
    HDMI_INFO("   auxiliary_runmode: %u\n", frame->auxiliary_runmode);
    HDMI_INFO("   auxiliary_runversion: %u\n", frame->auxiliary_runversion);
    HDMI_INFO("   auxiliary_debug: %u\n", frame->auxiliary_debug);
}

/*
 * hdmi_vendor_any_infoframe_log() - log info of any HDMI VENDOR infoframe
 * @level: logging level
 * @dev: device
 * @frame: any HDMI VENDOR infoframe
 */
static void hdmi_vendor_any_infoframe_log(const union hdmi_vendor_any_infoframe *frame)
{
    if (frame->any.oui == HDMI_IEEE_OUI) {
        hdmi_vendor_infoframe_log(&frame->hdmi);
    } else if (frame->any.oui == HDMI_FORUM_IEEE_OUI) {
        hdmi_forum_vendor_infoframe_log(&frame->hdmi_forum);
    } else if (frame->any.oui == HDMI_DOLBY_IEEE_OUI) {
        hdmi_dolby_vendor_infoframe_log(&frame->hdmi_dolby);
    } else {
        hdmi_infoframe_log_header((const struct hdmi_any_infoframe *)frame);
        HDMI_INFO("   unknow HDMI vendor infoframe\n");
    }
}

static const hi_s8 *hdmi_drm_eotf_type_get_name(enum hdmi_eotf_type eotf_type)
{
    if (eotf_type < 0 || eotf_type > 7) {
        return "Invalid";
    }

    switch (eotf_type) {
        case HDMI_EOTF_SDR_LUMIN:
            return "Traditional gamma - SDR Luminance Range";
        case HDMI_EOTF_HDR_LUMIN:
            return "Traditional gamma - HDR Luminance Range";
        case HDMI_EOTF_SMPTE_ST_2084:
            return "SMPTE ST 2084 [40]";
        case HDMI_EOTF_HLG:
            return "Hybrid Log-Gamma (HLG) based on ITU-RBT.2100-0 [50]";
        default:
            break;
    }
    return "Reserved";
}

static const hi_s8 *hdmi_drm_metadta_type_get_name(enum hdmi_static_metadata_type metadata_type)
{
    if (metadata_type < 0 || metadata_type > 7) {
        return "Invalid";
    }

    switch (metadata_type) {
        case HDMI_HDR_METADATA_ID_0:
            return "Static Metadata Type 1";
        default:
            break;
    }
    return "Reserved";
}

static void hdmi_drm_infoframe_log(const struct hdmi_drm_infoframe *frame)
{
    const hi_s8 *metadata_name = HI_NULL;
    const hi_s8 *eotf_name = HI_NULL;

    metadata_name = hdmi_drm_metadta_type_get_name(frame->metadata_type);
    eotf_name = hdmi_drm_eotf_type_get_name(frame->eotf_type);
    hdmi_infoframe_log_header((const struct hdmi_any_infoframe *)frame);

    HDMI_INFO("   eotf type: %s\n", eotf_name);

    HDMI_INFO("   metadata_type: %s\n", metadata_name);

    HDMI_INFO("   display_primaries_x[0]: %u\n", frame->meta_descriptor.type1.primaries0_x);
    HDMI_INFO("   display_primaries_y[0]: %u\n", frame->meta_descriptor.type1.primaries0_y);
    HDMI_INFO("   display_primaries_x[1]: %u\n", frame->meta_descriptor.type1.primaries1_x);
    HDMI_INFO("   display_primaries_y[1]: %u\n", frame->meta_descriptor.type1.primaries1_y);
    HDMI_INFO("   display_primaries_x[2]: %u\n", frame->meta_descriptor.type1.primaries2_x);
    HDMI_INFO("   display_primaries_y[2]: %u\n", frame->meta_descriptor.type1.primaries2_y);
    HDMI_INFO("   white_point_x: %u\n", frame->meta_descriptor.type1.white_point_x);
    HDMI_INFO("   white_point_y: %u\n", frame->meta_descriptor.type1.white_point_y);
    HDMI_INFO("   max_display_mastering_luminance: %u\n", frame->meta_descriptor.type1.max_luminance);
    HDMI_INFO("   min_display_mastering_luminance: %u\n", frame->meta_descriptor.type1.min_luminance);
    HDMI_INFO("   Maximum Content Light Level: %u\n", frame->meta_descriptor.type1.max_light_level);
    HDMI_INFO("   Maximum Frame-average Light Level: %u\n", frame->meta_descriptor.type1.average_light_level);
}

/*
 * hdmi_infoframe_log() - log info of HDMI infoframe
 * @level: logging level
 * @dev: device
 * @frame: HDMI infoframe
 */
void hdmi_infoframe_log(const union hdmi_infoframe *frame)
{
    switch (frame->any.type) {
        case HDMI_INFOFRAME_TYPE_AVI:
            hdmi_avi_infoframe_log(&frame->avi);
            break;
        case HDMI_INFOFRAME_TYPE_SPD:
            hdmi_spd_infoframe_log(&frame->spd);
            break;
        case HDMI_INFOFRAME_TYPE_AUDIO:
            hdmi_audio_infoframe_log(&frame->audio);
            break;
        case HDMI_INFOFRAME_TYPE_VENDOR:
            hdmi_vendor_any_infoframe_log(&frame->vendor);
            break;
        case HDMI_INFOFRAME_TYPE_DRM:
            hdmi_drm_infoframe_log(&frame->drm);
            break;
    }
}
EXPORT_SYMBOL(hdmi_infoframe_log);

/*
 * hdmi_avi_infoframe_unpack() - unpack binary buffer to a HDMI AVI infoframe
 * @frame: HDMI AVI infoframe
 * @buffer: source buffer
 * @size: size of buffer
 *
 * Unpacks the information contained in binary @buffer into a structured
 * @frame of the HDMI Auxiliary Video (AVI) information frame.
 * Also verifies the checksum as required by section 5.3.5 of the HDMI 1.4
 * specification.
 *
 * Returns 0 on success or a negative error code on failure.
 */
static hi_s32 hdmi_avi_infoframe_unpack(struct hdmi_avi_infoframe *frame,
                                        const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    hi_s32 ret;
    hi_u8 check_sum;

    if (size < HDMI_INFOFRAME_SIZE(AVI)) {
        return -EINVAL;
    }

    if (ptr[0] != HDMI_INFOFRAME_TYPE_AVI ||
        (ptr[1] != 2 && ptr[1] != 3 && ptr[1] != 4) || /* [1] = 2 3 4 is version */
        ptr[2] != HDMI_AVI_INFOFRAME_SIZE) {
        return -EINVAL;
    }

    check_sum = hdmi_infoframe_checksum(buffer, HDMI_INFOFRAME_SIZE(AVI));
    if (check_sum != 0) {
        return -EINVAL;
    }

    ret = hdmi_avi_infoframe_init(frame);
    if (ret) {
        return ret;
    }

    ptr += HDMI_INFOFRAME_HEADER_SIZE;

    frame->colorspace = (ptr[0] >> 5) & 0x3;
    if (ptr[0] & 0x10) {
        frame->active_aspect = ptr[1] & 0xf;
    }
    if (ptr[0] & 0x8) {
        frame->top_bar = (ptr[5] << 8) + ptr[6];
        frame->bottom_bar = (ptr[7] << 8) + ptr[8];
    }
    if (ptr[0] & 0x4) {
        frame->left_bar = (ptr[9] << 8) + ptr[10];
        frame->right_bar = (ptr[11] << 8) + ptr[12];
    }
    frame->scan_mode = ptr[0] & 0x3;

    frame->colorimetry = (ptr[1] >> 6) & 0x3;
    frame->picture_aspect = (ptr[1] >> 4) & 0x3;
    frame->active_aspect = ptr[1] & 0xf;

    frame->itc = ptr[2] & 0x80 ? true : false;
    frame->extended_colorimetry = (ptr[2] >> 4) & 0x7;
    frame->quantization_range = (ptr[2] >> 2) & 0x3;
    frame->nups = ptr[2] & 0x3;

    frame->video_code = ptr[3];
    frame->ycc_quantization_range = (ptr[4] >> 6) & 0x3;
    frame->content_type = (ptr[4] >> 4) & 0x3;

    frame->pixel_repeat = ptr[4] & 0xf;

    return 0;
}

/*
 * hdmi_spd_infoframe_unpack() - unpack binary buffer to a HDMI SPD infoframe
 * @frame: HDMI SPD infoframe
 * @buffer: source buffer
 * @size: size of buffer
 *
 * Unpacks the information contained in binary @buffer into a structured
 * @frame of the HDMI Source Product Description (SPD) information frame.
 * Also verifies the checksum as required by section 5.3.5 of the HDMI 1.4
 * specification.
 *
 * Returns 0 on success or a negative error code on failure.
 */
static hi_s32 hdmi_spd_infoframe_unpack(struct hdmi_spd_infoframe *frame,
                                        const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    hi_s32 ret;

    if (size < HDMI_INFOFRAME_SIZE(SPD)) {
        return -EINVAL;
    }

    if (ptr[0] != HDMI_INFOFRAME_TYPE_SPD ||
        ptr[1] != 1 ||
        ptr[2] != HDMI_SPD_INFOFRAME_SIZE) {
        return -EINVAL;
    }

    if (hdmi_infoframe_checksum(buffer, HDMI_INFOFRAME_SIZE(SPD)) != 0) {
        return -EINVAL;
    }

    ptr += HDMI_INFOFRAME_HEADER_SIZE;

    ret = hdmi_spd_infoframe_init(frame, ptr, ptr + 8);
    if (ret) {
        return ret;
    }

    frame->sdi = ptr[24];

    return 0;
}

/*
 * hdmi_audio_infoframe_unpack() - unpack binary buffer to a HDMI AUDIO infoframe
 * @frame: HDMI Audio infoframe
 * @buffer: source buffer
 * @size: size of buffer
 *
 * Unpacks the information contained in binary @buffer into a structured
 * @frame of the HDMI Audio information frame.
 * Also verifies the checksum as required by section 5.3.5 of the HDMI 1.4
 * specification.
 *
 * Returns 0 on success or a negative error code on failure.
 */
static hi_s32 hdmi_audio_infoframe_unpack(struct hdmi_audio_infoframe *frame,
                                          const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    hi_s32 ret;

    if (size < HDMI_INFOFRAME_SIZE(AUDIO)) {
        return -EINVAL;
    }

    if (ptr[0] != HDMI_INFOFRAME_TYPE_AUDIO ||
        ptr[1] != 1 ||
        ptr[2] != HDMI_AUDIO_INFOFRAME_SIZE) {
        return -EINVAL;
    }

    if (hdmi_infoframe_checksum(buffer, HDMI_INFOFRAME_SIZE(AUDIO)) != 0) {
        return -EINVAL;
    }

    ret = hdmi_audio_infoframe_init(frame);
    if (ret) {
        return ret;
    }

    ptr += HDMI_INFOFRAME_HEADER_SIZE;

    frame->channels = ptr[0] & 0x7;
    frame->coding_type = (ptr[0] >> 4) & 0xf;
    frame->sample_size = ptr[1] & 0x3;
    frame->sample_frequency = (ptr[1] >> 2) & 0x7;
    frame->coding_type_ext = ptr[2] & 0x1f;
    frame->channel_allocation = ptr[3];
    frame->level_shift_value = (ptr[4] >> 3) & 0xf;
    frame->downmix_inhibit = ptr[4] & 0x80 ? true : false;

    return 0;
}

static hi_s32 hdmi_vendor_infoframe_unpack(struct hdmi_vendor_infoframe *frame,
                                           const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;
    hi_u8 hdmi_video_format;

    if (size < HDMI_INFOFRAME_HEADER_SIZE) {
        return -EINVAL;
    }

    if (ptr[0] != HDMI_INFOFRAME_TYPE_VENDOR ||
        ptr[1] != 1) {
        return -EINVAL;
    }

    length = ptr[2];

    if (size < HDMI_INFOFRAME_HEADER_SIZE + length) {
        return -EINVAL;
    }

    if (hdmi_infoframe_checksum(buffer,
                                HDMI_INFOFRAME_HEADER_SIZE + length) != 0) {
        return -EINVAL;
    }

    ptr += HDMI_INFOFRAME_HEADER_SIZE;

    /* HDMI OUI */
    if ((ptr[0] != 0x03) ||
        (ptr[1] != 0x0c) ||
        (ptr[2] != 0x00)) {
        return -EINVAL;
    }

    hdmi_video_format = ptr[3] >> 5;

    if (hdmi_video_format > 0x2) {
        return -EINVAL;
    }

    ret = hdmi_vendor_infoframe_init(frame);
    if (ret) {
        return ret;
    }

    frame->length = length;

    if (hdmi_video_format == 0x2) {
        if (length != 5 && length != 6) {
            return -EINVAL;
        }
        frame->s3d_struct = ptr[4] >> 4;
        if (frame->s3d_struct >= HDMI_3D_STRUCTURE_SIDE_BY_SIDE_HALF) {
            if (length != 6) {
                return -EINVAL;
            }
            frame->s3d_ext_data = ptr[5] >> 4;
        }
    } else if (hdmi_video_format == 0x1) {
        if (length != 5 && length != 0x18) { /* length is 0x18 for dolby vision. */
            return -EINVAL;
        }
        frame->vic = ptr[4];
    } else {
        if (length != 4) {
            return -EINVAL;
        }
    }

    return 0;
}

static hi_s32 hdmi_forum_vendor_infoframe_unpack(struct hdmi_forum_vendor_infoframe *frame,
                                                 const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    if (size < HDMI_INFOFRAME_HEADER_SIZE) {
        return -EINVAL;
    }

    if (ptr[0] != HDMI_INFOFRAME_TYPE_VENDOR ||
        ptr[1] != 1) {
        return -EINVAL;
    }

    length = ptr[2];

    if (size < HDMI_INFOFRAME_HEADER_SIZE + length) {
        return -EINVAL;
    }

    if (hdmi_infoframe_checksum(buffer,
                                HDMI_INFOFRAME_HEADER_SIZE + length) != 0) {
        return -EINVAL;
    }

    ptr += HDMI_INFOFRAME_HEADER_SIZE;

    /* HDMI OUI */
    if ((ptr[0] != 0xd8) ||
        (ptr[1] != 0x5d) ||
        (ptr[2] != 0xc4)) {
        return -EINVAL;
    }

    ret = hdmi_forum_vendor_infoframe_init(frame);
    if (ret) {
        return ret;
    }

    frame->length = length;

    return 0;
}

static hi_s32 hdmi_dolby_vendor_infoframe_unpack(struct hdmi_dolby_vendor_infoframe *frame,
                                                 const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    if (size < HDMI_INFOFRAME_HEADER_SIZE) {
        return -EINVAL;
    }

    if (ptr[0] != HDMI_INFOFRAME_TYPE_VENDOR ||
        ptr[1] != 1) {
        return -EINVAL;
    }

    length = ptr[2];

    if (size < HDMI_INFOFRAME_HEADER_SIZE + length) {
        return -EINVAL;
    }

    if (hdmi_infoframe_checksum(buffer,
                                HDMI_INFOFRAME_HEADER_SIZE + length) != 0) {
        return -EINVAL;
    }

    ptr += HDMI_INFOFRAME_HEADER_SIZE;

    /* HDMI OUI */
    if ((ptr[0] != 0x46) ||
        (ptr[1] != 0xd0) ||
        (ptr[2] != 0x00)) {
        return -EINVAL;
    }

    ret = hdmi_dolby_vendor_infoframe_init(frame);
    if (ret) {
        return ret;
    }

    frame->length = length;

    if (length != 0x1b) {
        return -EINVAL;
    }

    frame->low_latency = ptr[3] & 0x1;
    frame->dolbyvision_signal = (ptr[3] >> 1) & 0x1;
    frame->backlt_ctrl_md_present = (ptr[4] >> 7) & 0x1;
    frame->auxiliary_md_present = (ptr[4] >> 6) & 0x1;
    frame->eff_tmax_pq_hi = ptr[4] & 0xf;
    frame->eff_tmax_pq_low = ptr[5];
    frame->auxiliary_runmode = ptr[6];
    frame->auxiliary_runversion = ptr[7];
    frame->auxiliary_debug = ptr[8];

    return 0;
}

/**
 * hdmi_vendor_infoframe_unpack() - unpack binary buffer to a HDMI vendor infoframe
 * @frame: HDMI Vendor infoframe
 * @buffer: source buffer
 * @size: size of buffer
 *
 * Unpacks the information contained in binary @buffer into a structured
 * @frame of the HDMI Vendor information frame.
 * Also verifies the checksum as required by section 5.3.5 of the HDMI 1.4
 * specification.
 *
 * Returns 0 on success or a negative error code on failure.
 */
static hi_s32 hdmi_vendor_any_infoframe_unpack(union hdmi_vendor_any_infoframe *frame,
                                               const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    hi_s32 ret;

    if ((ptr[4] == 0x03) &&
        (ptr[5] == 0x0c) &&
        (ptr[6] == 0x00)) {
        ret = hdmi_vendor_infoframe_unpack(&frame->hdmi, buffer, size);
    } else if ((ptr[4] == 0xd8) &&
               (ptr[5] == 0x5d) &&
               (ptr[6] == 0xc4)) {
        ret = hdmi_forum_vendor_infoframe_unpack(&frame->hdmi_forum, buffer, size);
    } else if ((ptr[4] == 0x46) &&
               (ptr[5] == 0xd0) &&
               (ptr[6] == 0x00)) {
        ret = hdmi_dolby_vendor_infoframe_unpack(&frame->hdmi_dolby, buffer, size);
    } else {
        return -EINVAL;
    }

    return ret;
}

static hi_s32 hdmi_drm_infoframe_unpack(struct hdmi_drm_infoframe *frame,
                                        const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    hi_s32 ret;
    struct hdmi_meta_descriptor_1st *descriptor_1st = &frame->meta_descriptor.type1;

    if (size < HDMI_INFOFRAME_SIZE(DRM)) {
        return -EINVAL;
    }

    if (ptr[0] != HDMI_INFOFRAME_TYPE_DRM ||
        ptr[1] != 1 ||
        ptr[2] != HDMI_DRM_INFOFRAME_SIZE) {
        return -EINVAL;
    }

    if (hdmi_infoframe_checksum(buffer, HDMI_INFOFRAME_SIZE(DRM)) != 0) {
        return -EINVAL;
    }

    ret = hdmi_drm_infoframe_init(frame);
    if (ret) {
        return ret;
    }

    ptr += HDMI_INFOFRAME_HEADER_SIZE;

    frame->eotf_type = ptr[0] & 0x7;
    frame->metadata_type = ptr[1] & 0x7;
    descriptor_1st->primaries0_x = (ptr[3] << 8) | ptr[2];
    descriptor_1st->primaries0_y = (ptr[5] << 8) | ptr[4];
    descriptor_1st->primaries1_x = (ptr[7] << 8) | ptr[6];
    descriptor_1st->primaries1_y = (ptr[9] << 8) | ptr[8];
    descriptor_1st->primaries2_x = (ptr[11] << 8) | ptr[10];
    descriptor_1st->primaries2_y = (ptr[13] << 8) | ptr[12];
    descriptor_1st->white_point_x = (ptr[15] << 8) | ptr[14];
    descriptor_1st->white_point_y = (ptr[17] << 8) | ptr[16];
    descriptor_1st->max_luminance = (ptr[19] << 8) | ptr[18];
    descriptor_1st->min_luminance = (ptr[21] << 8) | ptr[20];
    descriptor_1st->max_light_level = (ptr[23] << 8) | ptr[22];
    descriptor_1st->average_light_level = (ptr[25] << 8) | ptr[24];

    return 0;
}

/**
 * hdmi_infoframe_unpack() - unpack binary buffer to a HDMI infoframe
 * @frame: HDMI infoframe
 * @buffer: source buffer
 * @size: size of buffer
 *
 * Unpacks the information contained in binary buffer @buffer into a structured
 * @frame of a HDMI infoframe.
 * Also verifies the checksum as required by section 5.3.5 of the HDMI 1.4
 * specification.
 *
 * Returns 0 on success or a negative error code on failure.
 */
hi_s32 hdmi_infoframe_unpack(union hdmi_infoframe *frame,
                             const void *buffer, size_t size)
{
    hi_s32 ret;
    const hi_u8 *ptr = buffer;

    if (size < HDMI_INFOFRAME_HEADER_SIZE) {
        return -EINVAL;
    }

    switch (ptr[0]) {
        case HDMI_INFOFRAME_TYPE_AVI:
            ret = hdmi_avi_infoframe_unpack(&frame->avi, buffer, size);
            break;
        case HDMI_INFOFRAME_TYPE_SPD:
            ret = hdmi_spd_infoframe_unpack(&frame->spd, buffer, size);
            break;
        case HDMI_INFOFRAME_TYPE_AUDIO:
            ret = hdmi_audio_infoframe_unpack(&frame->audio, buffer, size);
            break;
        case HDMI_INFOFRAME_TYPE_VENDOR:
            ret = hdmi_vendor_any_infoframe_unpack(&frame->vendor, buffer, size);
            break;
        case HDMI_INFOFRAME_TYPE_DRM:
            ret = hdmi_drm_infoframe_unpack(&frame->drm, buffer, size);
            break;
        default:
            ret = -EINVAL;
            break;
    }

    return ret;
}
EXPORT_SYMBOL(hdmi_infoframe_unpack);

hi_s32 hdmi_hdmi_vt_ext_metadata_init(struct hdmi_hdmi_vt_ext_metadata *em_data)
{
    if (memset_s(em_data, sizeof(struct hdmi_hdmi_vt_ext_metadata),
                 0, sizeof(struct hdmi_hdmi_vt_ext_metadata))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    em_data->type = HDMI_PACKET_TYPE_EXTENDED_METADATA;
    em_data->first = 1;
    em_data->last = 1;
    em_data->sequence_index = 0;
    em_data->pkt_new = 1;
    em_data->pkt_end = 0;
    em_data->ds_type = 0;
    em_data->afr = 0;
    em_data->vfr = 1;
    em_data->sync = 0;
    em_data->organization_id = HDMI_ORGANIZATION_ID;
    em_data->data_set_tag = HDMI_VT_EM_DATA_SET_TAG;
    em_data->data_set_length = HDMI_VT_EM_DATA_SIZE;

    return 0;
}
EXPORT_SYMBOL(hdmi_hdmi_vt_ext_metadata_init);

hi_s32 hdmi_hdmi_vt_ext_metadata_check_only(const struct hdmi_hdmi_vt_ext_metadata *em_data)
{
    if (em_data->type != HDMI_PACKET_TYPE_EXTENDED_METADATA ||
        em_data->first != 1 || em_data->last != 1 ||
        em_data->sequence_index != 0) {
        return -EINVAL;
    }

    if (em_data->pkt_new != 1 || em_data->pkt_end != 0 ||
        em_data->ds_type != 0 || em_data->afr != 0 ||
        em_data->vfr != 1 || em_data->sync != 0) {
        return -EINVAL;
    }

    if (em_data->organization_id != HDMI_ORGANIZATION_ID) {
        return -EINVAL;
    }

    if (em_data->data_set_tag != HDMI_VT_EM_DATA_SET_TAG ||
        em_data->data_set_length != HDMI_VT_EM_DATA_SIZE) {
        return -EINVAL;
    }

    return 0;
}

hi_s32 hdmi_hdmi_vt_ext_metadata_check(struct hdmi_hdmi_vt_ext_metadata *em_data)
{
    return hdmi_hdmi_vt_ext_metadata_check_only(em_data);
}
EXPORT_SYMBOL(hdmi_hdmi_vt_ext_metadata_check);

ssize_t hdmi_hdmi_vt_ext_metadata_pack_only(const struct hdmi_hdmi_vt_ext_metadata *em_data,
                                            void *buffer, size_t size)
{
    hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    ret = hdmi_hdmi_vt_ext_metadata_check_only(em_data);
    if (ret) {
        return ret;
    }

    length = HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE +
             em_data->data_set_length;

    if (size < length) {
        return -ENOSPC;
    }

    if (memset_s(buffer, size, 0, size)) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    /* First packet header */
    ptr[0] = em_data->type;
    ptr[1] = ((em_data->first & 0x1) << 7) | ((em_data->last & 0x1) << 6);
    ptr[2] = em_data->sequence_index;

    /* First packet body */
    /* em data set header */
    ptr[3] = ((em_data->pkt_new & 0x1) << 7) | ((em_data->pkt_end & 0x1) << 6) |
             ((em_data->ds_type & 0x3) << 4) | ((em_data->afr & 0x1) << 3) |
             ((em_data->vfr & 0x1) << 2) | ((em_data->sync & 0x1) << 1);

    /* Reserved */
    ptr[4] = 0;
    ptr[5] = em_data->organization_id;
    ptr[6] = (em_data->data_set_tag >> 8) & 0xff;
    ptr[7] = em_data->data_set_tag & 0xff;
    ptr[8] = (em_data->data_set_length >> 8) & 0xff;
    ptr[9] = em_data->data_set_length & 0xff;

    /* em data set body */
    ptr[10] = ((em_data->fva_factor_m1 & 0xf) << 4) |
              ((em_data->m_const & 0x1) << 1) |
              (em_data->vrr_en & 0x1);
    ptr[11] = em_data->base_vfront;
    ptr[12] = ((em_data->rb & 0x1) << 2) |
              ((em_data->base_refresh_rate >> 8) & 0x3);
    ptr[13] = em_data->base_refresh_rate & 0xff;

    return length;
}
EXPORT_SYMBOL(hdmi_hdmi_vt_ext_metadata_pack_only);

ssize_t hdmi_hdmi_vt_ext_metadata_pack(struct hdmi_hdmi_vt_ext_metadata *em_data,
                                       void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_hdmi_vt_ext_metadata_check(em_data);
    if (ret) {
        return ret;
    }

    return hdmi_hdmi_vt_ext_metadata_pack_only(em_data, buffer, size);
}
EXPORT_SYMBOL(hdmi_hdmi_vt_ext_metadata_pack);

hi_s32 hdmi_hdmi_dsc_ext_metadata_init(struct hdmi_hdmi_dsc_ext_metadata *em_data)
{
    if (memset_s(em_data, sizeof(struct hdmi_hdmi_dsc_ext_metadata),
                 0, sizeof(struct hdmi_hdmi_dsc_ext_metadata))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    em_data->type = HDMI_PACKET_TYPE_EXTENDED_METADATA;
    em_data->first = 1;
    em_data->last = 0;
    em_data->sequence_index = 0;
    em_data->pkt_new = 1;
    em_data->pkt_end = 0;
    em_data->ds_type = 0;
    em_data->afr = 0;
    em_data->vfr = 1;
    em_data->sync = 1;
    em_data->organization_id = HDMI_ORGANIZATION_ID;
    em_data->data_set_tag = HDMI_DSC_EM_DATA_SET_TAG;
    em_data->data_set_length = HDMI_DSC_EM_DATA_SIZE;

    return 0;
}
EXPORT_SYMBOL(hdmi_hdmi_dsc_ext_metadata_init);

hi_s32 hdmi_hdmi_dsc_ext_metadata_check_only(const struct hdmi_hdmi_dsc_ext_metadata *em_data)
{
    if (em_data->type != HDMI_PACKET_TYPE_EXTENDED_METADATA ||
        em_data->first != 1 || em_data->last != 0 ||
        em_data->sequence_index != 0) {
        return -EINVAL;
    }

    if (em_data->ds_type != 0 || em_data->afr != 0 ||
        em_data->vfr != 1 || em_data->sync != 1) {
        return -EINVAL;
    }

    if (em_data->organization_id != HDMI_ORGANIZATION_ID) {
        return -EINVAL;
    }

    if (em_data->data_set_tag != HDMI_DSC_EM_DATA_SET_TAG ||
        em_data->data_set_length != HDMI_DSC_EM_DATA_SIZE) {
        return -EINVAL;
    }

    return 0;
}

hi_s32 hdmi_hdmi_dsc_ext_metadata_check(struct hdmi_hdmi_dsc_ext_metadata *em_data)
{
    return hdmi_hdmi_dsc_ext_metadata_check_only(em_data);
}
EXPORT_SYMBOL(hdmi_hdmi_dsc_ext_metadata_check);

ssize_t hdmi_hdmi_dsc_ext_metadata_pack_only(const struct hdmi_hdmi_dsc_ext_metadata *em_data,
                                             void *buffer, size_t size)
{
    hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    ret = hdmi_hdmi_dsc_ext_metadata_check_only(em_data);
    if (ret) {
        return ret;
    }

    length = HDMI_PACKET_HEADER_SIZE * HDMI_DSC_EM_DATA_PACKET_NUM + HDMI_DSF_HEADE_SIZE + em_data->data_set_length;

    if (size < length) {
        return -ENOSPC;
    }

    if (memset_s(buffer, size, 0, size)) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    /* First packet header */
    ptr[0] = em_data->type;
    ptr[1] = ((em_data->first & 0x1) << 7) | ((em_data->last & 0x1) << 6);
    ptr[2] = em_data->sequence_index;

    /* First packet body */
    /* em data set header */
    ptr[3] = ((em_data->pkt_new & 0x1) << 7) | ((em_data->pkt_end & 0x1) << 6) |
             ((em_data->ds_type & 0x3) << 4) | ((em_data->afr & 0x1) << 3) |
             ((em_data->vfr & 0x1) << 2) | ((em_data->sync & 0x1) << 1);
    ptr[4] = 0; /* Reserved */
    ptr[5] = em_data->organization_id;
    ptr[6] = (em_data->data_set_tag >> 8) & 0xff;
    ptr[7] = em_data->data_set_tag & 0xff;
    ptr[8] = (em_data->data_set_length >> 8) & 0xff;
    ptr[9] = em_data->data_set_length & 0xff;

    /* em data set body */
    ptr[10] = ((em_data->dsc_version_major & 0xf) << 4) |
              (em_data->dsc_version_minor & 0xf); /* pps0 */
    ptr[11] = em_data->pps_identifier;            /* pps1 */
    ptr[12] = 0;                                  /* pps2, reserved */
    ptr[13] = ((em_data->bits_per_component & 0xf) << 4) |
              (em_data->linebuf_depth & 0xf); /* pps3 */
    ptr[14] = ((em_data->block_pred_enable & 0x1) << 5) |
              ((em_data->convert_rgb & 0x1) << 4) |
              ((em_data->simple_422 & 0x1) << 3) |
              ((em_data->vbr_enable & 0x1) << 2) |
              ((em_data->bits_per_pixel >> 8) & 0x3);   /* pps4, bit[7:6] reserved */
    ptr[15] = em_data->bits_per_pixel & 0xff;           /* pps5 */
    ptr[16] = (em_data->pic_height >> 8) & 0xff;        /* pps6 */
    ptr[17] = em_data->pic_height & 0xff;               /* pps7 */
    ptr[18] = (em_data->pic_width >> 8) & 0xff;         /* pps8 */
    ptr[19] = em_data->pic_width & 0xff;                /* pps9 */
    ptr[20] = (em_data->slice_height >> 8) & 0xff;      /* pps10 */
    ptr[21] = em_data->slice_height & 0xff;             /* pps11 */
    ptr[22] = (em_data->slice_width >> 8) & 0xff;       /* pps12 */
    ptr[23] = em_data->slice_width & 0xff;              /* pps13 */
    ptr[24] = (em_data->chunk_size >> 8) & 0xff;        /* pps14 */
    ptr[25] = em_data->chunk_size & 0xff;               /* pps15 */
    ptr[26] = (em_data->initial_xmit_delay >> 8) & 0x3; /* pps16, bit[7:2] reserved */
    ptr[27] = em_data->initial_xmit_delay & 0xff;       /* pps17 */
    ptr[28] = (em_data->initial_dec_delay >> 8) & 0xff; /* pps18 */
    ptr[29] = em_data->initial_dec_delay & 0xff;        /* pps19 */
    ptr[30] = 0;                                        /* pps20, reserved */

    ptr += HDMI_PACKET_SIZE;
    /* Second packet header */
    ptr[0] = em_data->type;
    ptr[1] = 0;
    ptr[2] = em_data->sequence_index + 1;

    /* Second packet body */
    ptr[3] = em_data->initial_scale_value & 0x3f;             /* pps21, bit[7:6] reserved */
    ptr[4] = (em_data->scale_increment_interval >> 8) & 0xff; /* pps22 */
    ptr[5] = em_data->scale_increment_interval & 0xff;        /* pps23 */
    ptr[6] = (em_data->scale_decrement_interval >> 8) & 0xf;  /* pps24, bit[7:4] reserved */
    ptr[7] = em_data->scale_decrement_interval & 0xff;        /* pps25 */
    ptr[8] = 0;                                               /* pps26, reserved */
    ptr[9] = em_data->first_line_bpg_offset & 0x1f;           /* pps27, bit[7:5] reserved */
    ptr[10] = (em_data->nfl_bpg_offset >> 8) & 0xff;          /* pps28 */
    ptr[11] = em_data->nfl_bpg_offset & 0xff;                 /* pps29 */
    ptr[12] = (em_data->slice_bpg_offset >> 8) & 0xff;        /* pps30 */
    ptr[13] = em_data->slice_bpg_offset & 0xff;               /* pps31 */
    ptr[14] = (em_data->inital_offset >> 8) & 0xff;           /* pps32 */
    ptr[15] = em_data->inital_offset & 0xff;                  /* pps33 */
    ptr[16] = (em_data->final_offset >> 8) & 0xff;            /* pps34 */
    ptr[17] = em_data->final_offset & 0xff;                   /* pps35 */
    ptr[18] = em_data->flatness_min_qp & 0x1f;                /* pps36, bit[7:5] reserved */
    ptr[19] = em_data->flatness_max_qp & 0x1f;                /* pps37, bit[7:5] reserved */
    ptr[20] = (em_data->rc_model_size >> 8) & 0xff;           /* pps38 */
    ptr[21] = em_data->rc_model_size & 0xff;                  /* pps39 */
    ptr[22] = em_data->rc_edge_factor & 0xf;                  /* pps40, bit[7:4] reserved */
    ptr[23] = em_data->rc_quant_incr_limit0 & 0x1f;           /* pps41, bit[7:5] reserved */
    ptr[24] = em_data->rc_quant_incr_limit1 & 0x1f;           /* pps42, bit[7:5] reserved */
    ptr[25] = ((em_data->rc_tgt_offset_hi & 0xf) << 4) |
              (em_data->rc_tgt_offset_lo & 0xf); /* pps43 */
    ptr[26] = em_data->rc_buf_thresh[0];         /* pps44 */
    ptr[27] = em_data->rc_buf_thresh[1];         /* pps45 */
    ptr[28] = em_data->rc_buf_thresh[2];         /* pps46 */
    ptr[29] = em_data->rc_buf_thresh[3];         /* pps47 */
    ptr[30] = em_data->rc_buf_thresh[4];         /* pps48 */

    ptr += HDMI_PACKET_SIZE;
    /* Third packet header */
    ptr[0] = em_data->type;
    ptr[1] = 0;
    ptr[2] = em_data->sequence_index + 2;

    /* Third packet body */
    ptr[3] = em_data->rc_buf_thresh[5];   /* pps49 */
    ptr[4] = em_data->rc_buf_thresh[6];   /* pps50 */
    ptr[5] = em_data->rc_buf_thresh[7];   /* pps51 */
    ptr[6] = em_data->rc_buf_thresh[8];   /* pps52 */
    ptr[7] = em_data->rc_buf_thresh[9];   /* pps53 */
    ptr[8] = em_data->rc_buf_thresh[10];  /* pps54 */
    ptr[9] = em_data->rc_buf_thresh[11];  /* pps55 */
    ptr[10] = em_data->rc_buf_thresh[12]; /* pps56 */
    ptr[11] = em_data->rc_buf_thresh[13]; /* pps57 */
    ptr[12] = ((em_data->rc_parameters[0].range_min_qp & 0x1f) << 3) |
              ((em_data->rc_parameters[0].range_max_qp >> 2) & 0x7); /* pps58 */
    ptr[13] = ((em_data->rc_parameters[0].range_max_qp & 0x3) << 6) |
              (em_data->rc_parameters[0].range_bpg_offset & 0x3f); /* pps59 */
    ptr[14] = ((em_data->rc_parameters[1].range_min_qp & 0x1f) << 3) |
              ((em_data->rc_parameters[1].range_max_qp >> 2) & 0x7); /* pps60 */
    ptr[15] = ((em_data->rc_parameters[1].range_max_qp & 0x3) << 6) |
              (em_data->rc_parameters[1].range_bpg_offset & 0x3f); /* pps61 */
    ptr[16] = ((em_data->rc_parameters[2].range_min_qp & 0x1f) << 3) |
              ((em_data->rc_parameters[2].range_max_qp >> 2) & 0x7); /* pps62 */
    ptr[17] = ((em_data->rc_parameters[2].range_max_qp & 0x3) << 6) |
              (em_data->rc_parameters[2].range_bpg_offset & 0x3f); /* pps63 */
    ptr[18] = ((em_data->rc_parameters[3].range_min_qp & 0x1f) << 3) |
              ((em_data->rc_parameters[3].range_max_qp >> 2) & 0x7); /* pps64 */
    ptr[19] = ((em_data->rc_parameters[3].range_max_qp & 0x3) << 6) |
              (em_data->rc_parameters[3].range_bpg_offset & 0x3f); /* pps65 */
    ptr[20] = ((em_data->rc_parameters[4].range_min_qp & 0x1f) << 3) |
              ((em_data->rc_parameters[4].range_max_qp >> 2) & 0x7); /* pps66 */
    ptr[21] = ((em_data->rc_parameters[4].range_max_qp & 0x3) << 6) |
              (em_data->rc_parameters[4].range_bpg_offset & 0x3f); /* pps67 */
    ptr[22] = ((em_data->rc_parameters[5].range_min_qp & 0x1f) << 3) |
              ((em_data->rc_parameters[5].range_max_qp >> 2) & 0x7); /* pps68 */
    ptr[23] = ((em_data->rc_parameters[5].range_max_qp & 0x3) << 6) |
              (em_data->rc_parameters[5].range_bpg_offset & 0x3f); /* pps69 */
    ptr[24] = ((em_data->rc_parameters[6].range_min_qp & 0x1f) << 3) |
              ((em_data->rc_parameters[6].range_max_qp >> 2) & 0x7); /* pps70 */
    ptr[25] = ((em_data->rc_parameters[6].range_max_qp & 0x3) << 6) |
              (em_data->rc_parameters[6].range_bpg_offset & 0x3f); /* pps71 */
    ptr[26] = ((em_data->rc_parameters[7].range_min_qp & 0x1f) << 3) |
              ((em_data->rc_parameters[7].range_max_qp >> 2) & 0x7); /* pps72 */
    ptr[27] = ((em_data->rc_parameters[7].range_max_qp & 0x3) << 6) |
              (em_data->rc_parameters[7].range_bpg_offset & 0x3f); /* pps73 */
    ptr[28] = ((em_data->rc_parameters[8].range_min_qp & 0x1f) << 3) |
              ((em_data->rc_parameters[8].range_max_qp >> 2) & 0x7); /* pps74 */
    ptr[29] = ((em_data->rc_parameters[8].range_max_qp & 0x3) << 6) |
              (em_data->rc_parameters[8].range_bpg_offset & 0x3f); /* pps75 */
    ptr[30] = ((em_data->rc_parameters[9].range_min_qp & 0x1f) << 3) |
              ((em_data->rc_parameters[9].range_max_qp >> 2) & 0x7); /* pps76 */

    ptr += HDMI_PACKET_SIZE;
    /* Fourth packet header */
    ptr[0] = em_data->type;
    ptr[1] = 0;
    ptr[2] = em_data->sequence_index + 3;

    /* Fourth packet body */
    ptr[3] = ((em_data->rc_parameters[9].range_max_qp & 0x3) << 6) |
             (em_data->rc_parameters[9].range_bpg_offset & 0x3f); /* pps77 */
    ptr[4] = ((em_data->rc_parameters[10].range_min_qp & 0x1f) << 3) |
             ((em_data->rc_parameters[10].range_max_qp >> 2) & 0x7); /* pps78 */
    ptr[5] = ((em_data->rc_parameters[10].range_max_qp & 0x3) << 6) |
             (em_data->rc_parameters[10].range_bpg_offset & 0x3f); /* pps79 */
    ptr[6] = ((em_data->rc_parameters[11].range_min_qp & 0x1f) << 3) |
             ((em_data->rc_parameters[11].range_max_qp >> 2) & 0x7); /* pps80 */
    ptr[7] = ((em_data->rc_parameters[11].range_max_qp & 0x3) << 6) |
             (em_data->rc_parameters[11].range_bpg_offset & 0x3f); /* pps81 */
    ptr[8] = ((em_data->rc_parameters[12].range_min_qp & 0x1f) << 3) |
             ((em_data->rc_parameters[12].range_max_qp >> 2) & 0x7); /* pps82 */
    ptr[9] = ((em_data->rc_parameters[12].range_max_qp & 0x3) << 6) |
             (em_data->rc_parameters[12].range_bpg_offset & 0x3f); /* pps83 */
    ptr[10] = ((em_data->rc_parameters[13].range_min_qp & 0x1f) << 3) |
              ((em_data->rc_parameters[13].range_max_qp >> 2) & 0x7); /* pps84 */
    ptr[11] = ((em_data->rc_parameters[13].range_max_qp & 0x3) << 6) |
              (em_data->rc_parameters[13].range_bpg_offset & 0x3f); /* pps85 */
    ptr[12] = ((em_data->rc_parameters[14].range_min_qp & 0x1f) << 3) |
              ((em_data->rc_parameters[14].range_max_qp >> 2) & 0x7); /* pps86 */
    ptr[13] = ((em_data->rc_parameters[14].range_max_qp & 0x3) << 6) |
              (em_data->rc_parameters[14].range_bpg_offset & 0x3f); /* pps87 */
    ptr[14] = ((em_data->native_420 & 0x1) << 1) |
              (em_data->native_422 & 0x1);                   /* pps88, bit[7:2] reserved */
    ptr[15] = em_data->second_line_bpg_offset & 0x1f;        /* pps89, bit[7:5] reserved */
    ptr[16] = (em_data->nsl_bpg_offset >> 8) & 0xff;         /* pps90 */
    ptr[17] = em_data->nsl_bpg_offset & 0xff;                /* pps91 */
    ptr[18] = (em_data->second_line_offset_adj >> 8) & 0xff; /* pps92 */
    ptr[19] = em_data->second_line_offset_adj & 0xff;        /* pps93 */
    /* ptr[20]~ptr[30](pps94~pps104) reserved, must be zero */
    ptr += HDMI_PACKET_SIZE;
    /* Fifth packet header */
    ptr[0] = em_data->type;
    ptr[1] = 0;
    ptr[2] = em_data->sequence_index + 4;

    /* Fifth packet body */
    /* ptr[3]~ptr[25](pps105~pps127) reserved, must be zero */
    ptr[26] = em_data->hfront & 0xff;        /* hfront[7:0] */
    ptr[27] = (em_data->hfront >> 8) & 0xff; /* hfront[15:8] */
    ptr[28] = em_data->hsync & 0xff;         /* hsync[7:0] */
    ptr[29] = (em_data->hsync >> 8) & 0xff;  /* hsync[15:8] */
    ptr[30] = em_data->hback & 0xff;         /* hback[7:0] */

    ptr += HDMI_PACKET_SIZE;
    /* Sixth packet header */
    ptr[0] = em_data->type;
    ptr[1] = 1 << 6;
    ptr[2] = em_data->sequence_index + 5;

    /* Sixth packet body */
    ptr[3] = (em_data->hback >> 8) & 0xff;    /* hback[15:8] */
    ptr[4] = em_data->hcactive & 0xff;        /* hcactive[7:0] */
    ptr[5] = (em_data->hcactive >> 8) & 0xff; /* hcactive[15:8] */
    /* ptr[6]~ptr[30] reserved, must be zero */
    return length;
}
EXPORT_SYMBOL(hdmi_hdmi_dsc_ext_metadata_pack_only);

ssize_t hdmi_hdmi_dsc_ext_metadata_pack(struct hdmi_hdmi_dsc_ext_metadata *em_data,
                                        void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_hdmi_dsc_ext_metadata_check(em_data);
    if (ret) {
        return ret;
    }

    return hdmi_hdmi_dsc_ext_metadata_pack_only(em_data, buffer, size);
}
EXPORT_SYMBOL(hdmi_hdmi_dsc_ext_metadata_pack);

hi_s32 hdmi_hdmi_any_ext_metadata_check_only(const union hdmi_hdmi_extended_metadata *em_data)
{
    if (em_data->any.type != HDMI_PACKET_TYPE_EXTENDED_METADATA ||
        em_data->any.organization_id != HDMI_ORGANIZATION_ID) {
        return -EINVAL;
    }

    return 0;
}

hi_s32 hdmi_hdmi_any_ext_metadata_check(union hdmi_hdmi_extended_metadata *em_data)
{
    hi_s32 ret;

    ret = hdmi_hdmi_any_ext_metadata_check_only(em_data);
    if (ret) {
        return ret;
    }

    if (em_data->any.data_set_tag == HDMI_VT_EM_DATA_SET_TAG) {
        ret = hdmi_hdmi_vt_ext_metadata_check(&em_data->vt);
    } else if (em_data->any.data_set_tag == HDMI_DSC_EM_DATA_SET_TAG) {
        ret = hdmi_hdmi_dsc_ext_metadata_check(&em_data->dsc);
    } else {
        return -EINVAL;
    }

    return ret;
}

ssize_t hdmi_hdmi_any_ext_metadata_pack_only(const union hdmi_hdmi_extended_metadata *em_data,
                                             void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_hdmi_any_ext_metadata_check_only(em_data);
    if (ret) {
        return ret;
    }

    if (em_data->any.data_set_tag == HDMI_VT_EM_DATA_SET_TAG) {
        ret = hdmi_hdmi_vt_ext_metadata_pack_only(&em_data->vt, buffer, size);
    } else if (em_data->any.data_set_tag == HDMI_DSC_EM_DATA_SET_TAG) {
        ret = hdmi_hdmi_dsc_ext_metadata_pack_only(&em_data->dsc, buffer, size);
    } else {
        return -EINVAL;
    }

    return ret;
}

ssize_t hdmi_hdmi_any_ext_metadata_pack(union hdmi_hdmi_extended_metadata *em_data,
                                        void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_hdmi_any_ext_metadata_check(em_data);
    if (ret) {
        return ret;
    }

    return hdmi_hdmi_any_ext_metadata_pack_only(em_data, buffer, size);
}

hi_s32 hdmi_cta_hdr1_ext_metadata_init(struct hdmi_cta_hdr1_ext_metadata *em_data)
{
    if (memset_s(em_data, sizeof(struct hdmi_cta_hdr1_ext_metadata),
                 0, sizeof(struct hdmi_cta_hdr1_ext_metadata))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    em_data->type = HDMI_PACKET_TYPE_EXTENDED_METADATA;
    em_data->first = 1;
    em_data->last = 1; /* need confirm how many packets */
    em_data->sequence_index = 0;
    em_data->pkt_new = 1;
    em_data->pkt_end = 0;
    em_data->ds_type = 1;
    em_data->afr = 0;
    em_data->vfr = 1;
    em_data->sync = 1;
    em_data->organization_id = HDMI_CTA_ORGANIZATION_ID;
    em_data->data_set_tag = HDMI_CTA_HDR1_EM_DATA_SET_TAG;
    em_data->data_set_length = HDMI_CTA_HDR1_EM_DATA_SIZE; /* need confirm */

    return 0;
}
EXPORT_SYMBOL(hdmi_cta_hdr1_ext_metadata_init);

hi_s32 hdmi_cta_hdr1_ext_metadata_check_only(const struct hdmi_cta_hdr1_ext_metadata *em_data)
{
    if (em_data->type != HDMI_PACKET_TYPE_EXTENDED_METADATA ||
        em_data->first != 1 || em_data->last != 1 /* need confirm */ ||
        em_data->sequence_index != 0) {
        return -EINVAL;
    }

    if (em_data->pkt_new != 1 || em_data->pkt_end != 0 ||
        em_data->ds_type != 1 || em_data->afr != 0 ||
        em_data->vfr != 1 || em_data->sync != 1) {
        return -EINVAL;
    }

    if (em_data->organization_id != HDMI_CTA_ORGANIZATION_ID) {
        return -EINVAL;
    }

    if (em_data->data_set_tag != HDMI_CTA_HDR1_EM_DATA_SET_TAG ||
        em_data->data_set_length != HDMI_CTA_HDR1_EM_DATA_SIZE) {
        return -EINVAL;
    }

    return 0;
}

hi_s32 hdmi_cta_hdr1_ext_metadata_check(struct hdmi_cta_hdr1_ext_metadata *em_data)
{
    return hdmi_cta_hdr1_ext_metadata_check_only(em_data);
}
EXPORT_SYMBOL(hdmi_cta_hdr1_ext_metadata_check);

ssize_t hdmi_cta_hdr1_ext_metadata_pack_only(const struct hdmi_cta_hdr1_ext_metadata *em_data,
                                             void *buffer, size_t size)
{
    hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    ret = hdmi_cta_hdr1_ext_metadata_check_only(em_data);
    if (ret) {
        return ret;
    }

    length = HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE + em_data->data_set_length;
    if (length > size) {
        return -ENOSPC;
    }

    if (memset_s(buffer, size, 0, size)) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    /* First packet header */
    ptr[0] = em_data->type;
    ptr[1] = ((em_data->first & 0x1) << 7) | ((em_data->last & 0x1) << 6);
    ptr[2] = em_data->sequence_index;

    /* First packet body */
    /* em data set header */
    ptr[3] = ((em_data->pkt_new & 0x1) << 7) | ((em_data->pkt_end & 0x1) << 6) |
             ((em_data->ds_type & 0x3) << 4) | ((em_data->afr & 0x1) << 3) |
             ((em_data->vfr & 0x1) << 2) | ((em_data->sync & 0x1) << 1);
    ptr[4] = 0; /* Reserved */
    ptr[5] = em_data->organization_id;
    ptr[6] = (em_data->data_set_tag >> 8) & 0xff;
    ptr[7] = em_data->data_set_tag & 0xff;
    ptr[8] = (em_data->data_set_length >> 8) & 0xff;
    ptr[9] = em_data->data_set_length & 0xff;

    /* em data set body */
    return length;
}
EXPORT_SYMBOL(hdmi_cta_hdr1_ext_metadata_pack_only);

ssize_t hdmi_cta_hdr1_ext_metadata_pack(struct hdmi_cta_hdr1_ext_metadata *em_data,
                                        void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_cta_hdr1_ext_metadata_check(em_data);
    if (ret) {
        return ret;
    }

    return hdmi_cta_hdr1_ext_metadata_pack_only(em_data, buffer, size);
}
EXPORT_SYMBOL(hdmi_cta_hdr1_ext_metadata_pack);

hi_s32 hdmi_cta_hdr2_ext_metadata_init(struct hdmi_cta_hdr2_ext_metadata *em_data)
{
    if (memset_s(em_data, sizeof(struct hdmi_cta_hdr2_ext_metadata),
                 0, sizeof(struct hdmi_cta_hdr2_ext_metadata))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    em_data->type = HDMI_PACKET_TYPE_EXTENDED_METADATA;
    em_data->first = 1;
    em_data->last = 1; /* need confirm how many packets */
    em_data->sequence_index = 0;
    em_data->pkt_new = 1;
    em_data->pkt_end = 0;
    em_data->ds_type = 1;
    em_data->afr = 0;
    em_data->vfr = 1;
    em_data->sync = 1;
    em_data->organization_id = HDMI_CTA_ORGANIZATION_ID;
    em_data->data_set_tag = HDMI_CTA_HDR2_EM_DATA_SET_TAG;
    em_data->data_set_length = HDMI_CTA_HDR2_EM_DATA_SIZE; /* need confirm */

    return 0;
}
EXPORT_SYMBOL(hdmi_cta_hdr2_ext_metadata_init);

hi_s32 hdmi_cta_hdr2_ext_metadata_check_only(const struct hdmi_cta_hdr2_ext_metadata *em_data)
{
    if (em_data->type != HDMI_PACKET_TYPE_EXTENDED_METADATA ||
        em_data->first != 1 || em_data->last != 1 /* need confirm */ ||
        em_data->sequence_index != 0) {
        return -EINVAL;
    }

    if (em_data->pkt_new != 1 || em_data->pkt_end != 0 ||
        em_data->ds_type != 1 || em_data->afr != 0 ||
        em_data->vfr != 1 || em_data->sync != 1) {
        return -EINVAL;
    }

    if (em_data->organization_id != HDMI_CTA_ORGANIZATION_ID) {
        return -EINVAL;
    }

    if (em_data->data_set_tag != HDMI_CTA_HDR2_EM_DATA_SET_TAG ||
        em_data->data_set_length != HDMI_CTA_HDR2_EM_DATA_SIZE) {
        return -EINVAL;
    }

    return 0;
}

hi_s32 hdmi_cta_hdr2_ext_metadata_check(struct hdmi_cta_hdr2_ext_metadata *em_data)
{
    return hdmi_cta_hdr2_ext_metadata_check_only(em_data);
}
EXPORT_SYMBOL(hdmi_cta_hdr2_ext_metadata_check);

ssize_t hdmi_cta_hdr2_ext_metadata_pack_only(const struct hdmi_cta_hdr2_ext_metadata *em_data,
                                             void *buffer, size_t size)
{
    hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    ret = hdmi_cta_hdr2_ext_metadata_check_only(em_data);
    if (ret) {
        return ret;
    }

    length = HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE + em_data->data_set_length;

    if (size < length) {
        return -ENOSPC;
    }

    if (memset_s(buffer, size, 0, size)) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    /* First packet header */
    ptr[0] = em_data->type;
    ptr[1] = ((em_data->first & 0x1) << 7) | ((em_data->last & 0x1) << 6);
    ptr[2] = em_data->sequence_index;

    /* First packet body */
    /* em data set header */
    ptr[3] = ((em_data->pkt_new & 0x1) << 7) | ((em_data->pkt_end & 0x1) << 6) |
             ((em_data->ds_type & 0x3) << 4) | ((em_data->afr & 0x1) << 3) |
             ((em_data->vfr & 0x1) << 2) | ((em_data->sync & 0x1) << 1);
    ptr[4] = 0; /* Reserved */
    ptr[5] = em_data->organization_id;
    ptr[6] = (em_data->data_set_tag >> 8) & 0xff;
    ptr[7] = em_data->data_set_tag & 0xff;
    ptr[8] = (em_data->data_set_length >> 8) & 0xff;
    ptr[9] = em_data->data_set_length & 0xff;

    /* em data set body */
    return length;
}
EXPORT_SYMBOL(hdmi_cta_hdr2_ext_metadata_pack_only);

ssize_t hdmi_cta_hdr2_ext_metadata_pack(struct hdmi_cta_hdr2_ext_metadata *em_data,
                                        void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_cta_hdr2_ext_metadata_check(em_data);
    if (ret) {
        return ret;
    }

    return hdmi_cta_hdr2_ext_metadata_pack_only(em_data, buffer, size);
}
EXPORT_SYMBOL(hdmi_cta_hdr2_ext_metadata_pack);

hi_s32 hdmi_cta_hdr3_ext_metadata_init(struct hdmi_cta_hdr3_ext_metadata *em_data)
{
    if (memset_s(em_data, sizeof(struct hdmi_cta_hdr3_ext_metadata),
                 0, sizeof(struct hdmi_cta_hdr3_ext_metadata))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    em_data->type = HDMI_PACKET_TYPE_EXTENDED_METADATA;
    em_data->first = 1;
    em_data->last = 1; /* need confirm how many packets */
    em_data->sequence_index = 0;
    em_data->pkt_new = 1;
    em_data->pkt_end = 0;
    em_data->ds_type = 1;
    em_data->afr = 0;
    em_data->vfr = 1;
    em_data->sync = 1;
    em_data->organization_id = HDMI_CTA_ORGANIZATION_ID;
    em_data->data_set_tag = HDMI_CTA_HDR3_EM_DATA_SET_TAG;
    em_data->data_set_length = HDMI_CTA_HDR3_EM_DATA_SIZE; /* need confirm */

    return 0;
}
EXPORT_SYMBOL(hdmi_cta_hdr3_ext_metadata_init);

hi_s32 hdmi_cta_hdr3_ext_metadata_check_only(const struct hdmi_cta_hdr3_ext_metadata *em_data)
{
    if (em_data->type != HDMI_PACKET_TYPE_EXTENDED_METADATA ||
        em_data->first != 1 || em_data->last != 1 /* need confirm */ ||
        em_data->sequence_index != 0) {
        return -EINVAL;
    }

    if (em_data->pkt_new != 1 || em_data->pkt_end != 0 ||
        em_data->ds_type != 1 || em_data->afr != 0 ||
        em_data->vfr != 1 || em_data->sync != 1) {
        return -EINVAL;
    }

    if (em_data->organization_id != HDMI_CTA_ORGANIZATION_ID) {
        return -EINVAL;
    }

    if (em_data->data_set_tag != HDMI_CTA_HDR3_EM_DATA_SET_TAG ||
        em_data->data_set_length != HDMI_CTA_HDR3_EM_DATA_SIZE) {
        return -EINVAL;
    }

    return 0;
}

hi_s32 hdmi_cta_hdr3_ext_metadata_check(struct hdmi_cta_hdr3_ext_metadata *em_data)
{
    return hdmi_cta_hdr3_ext_metadata_check_only(em_data);
}
EXPORT_SYMBOL(hdmi_cta_hdr3_ext_metadata_check);

ssize_t hdmi_cta_hdr3_ext_metadata_pack_only(const struct hdmi_cta_hdr3_ext_metadata *em_data,
                                             void *buffer, size_t size)
{
    hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    ret = hdmi_cta_hdr3_ext_metadata_check_only(em_data);
    if (ret) {
        return ret;
    }

    length = HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE + em_data->data_set_length;

    if (size < length) {
        return -ENOSPC;
    }

    if (memset_s(buffer, size, 0, size)) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    /* First packet header */
    ptr[0] = em_data->type;
    ptr[1] = ((em_data->first & 0x1) << 7) | ((em_data->last & 0x1) << 6);
    ptr[2] = em_data->sequence_index;

    /* First packet body */
    /* em data set header */
    ptr[3] = ((em_data->pkt_new & 0x1) << 7) | ((em_data->pkt_end & 0x1) << 6) |
             ((em_data->ds_type & 0x3) << 4) | ((em_data->afr & 0x1) << 3) |
             ((em_data->vfr & 0x1) << 2) | ((em_data->sync & 0x1) << 1);
    ptr[4] = 0; /* Reserved */
    ptr[5] = em_data->organization_id;
    ptr[6] = (em_data->data_set_tag >> 8) & 0xff;
    ptr[7] = em_data->data_set_tag & 0xff;
    ptr[8] = (em_data->data_set_length >> 8) & 0xff;
    ptr[9] = em_data->data_set_length & 0xff;

    /* em data set body */
    return length;
}
EXPORT_SYMBOL(hdmi_cta_hdr3_ext_metadata_pack_only);

ssize_t hdmi_cta_hdr3_ext_metadata_pack(struct hdmi_cta_hdr3_ext_metadata *em_data,
                                        void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_cta_hdr3_ext_metadata_check(em_data);
    if (ret) {
        return ret;
    }

    return hdmi_cta_hdr3_ext_metadata_pack_only(em_data, buffer, size);
}
EXPORT_SYMBOL(hdmi_cta_hdr3_ext_metadata_pack);

hi_s32 hdmi_cta_hdr4_ext_metadata_init(struct hdmi_cta_hdr4_ext_metadata *em_data)
{
    if (memset_s(em_data, sizeof(struct hdmi_cta_hdr4_ext_metadata),
                 0, sizeof(struct hdmi_cta_hdr4_ext_metadata))) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    em_data->type = HDMI_PACKET_TYPE_EXTENDED_METADATA;
    em_data->first = 1;
    em_data->last = 1; /* need confirm how many packets */
    em_data->sequence_index = 0;
    em_data->pkt_new = 1;
    em_data->pkt_end = 0;
    em_data->ds_type = 1;
    em_data->afr = 0;
    em_data->vfr = 1;
    em_data->sync = 1;
    em_data->organization_id = HDMI_CTA_ORGANIZATION_ID;
    em_data->data_set_tag = HDMI_CTA_HDR4_EM_DATA_SET_TAG;
    em_data->data_set_length = HDMI_CTA_HDR4_EM_DATA_SIZE; /* need confirm */

    return 0;
}
EXPORT_SYMBOL(hdmi_cta_hdr4_ext_metadata_init);

hi_s32 hdmi_cta_hdr4_ext_metadata_check_only(const struct hdmi_cta_hdr4_ext_metadata *em_data)
{
    if (em_data->type != HDMI_PACKET_TYPE_EXTENDED_METADATA ||
        em_data->first != 1 || em_data->last != 1 /* need confirm */ ||
        em_data->sequence_index != 0) {
        return -EINVAL;
    }

    if (em_data->pkt_new != 1 || em_data->pkt_end != 0 ||
        em_data->ds_type != 1 || em_data->afr != 0 ||
        em_data->vfr != 1 || em_data->sync != 1) {
        return -EINVAL;
    }

    if (em_data->organization_id != HDMI_CTA_ORGANIZATION_ID) {
        return -EINVAL;
    }

    if (em_data->data_set_tag != HDMI_CTA_HDR4_EM_DATA_SET_TAG ||
        em_data->data_set_length != HDMI_CTA_HDR4_EM_DATA_SIZE) {
        return -EINVAL;
    }

    return 0;
}

hi_s32 hdmi_cta_hdr4_ext_metadata_check(struct hdmi_cta_hdr4_ext_metadata *em_data)
{
    return hdmi_cta_hdr4_ext_metadata_check_only(em_data);
}
EXPORT_SYMBOL(hdmi_cta_hdr4_ext_metadata_check);

ssize_t hdmi_cta_hdr4_ext_metadata_pack_only(const struct hdmi_cta_hdr4_ext_metadata *em_data,
                                             void *buffer, size_t size)
{
    hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    ret = hdmi_cta_hdr4_ext_metadata_check_only(em_data);
    if (ret) {
        return ret;
    }

    length = HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE + em_data->data_set_length;

    if (size < length) {
        return -ENOSPC;
    }

    if (memset_s(buffer, size, 0, size)) {
        HDMI_ERR("memset_s err\n");
        return HI_FAILURE;
    }

    /* First packet header */
    ptr[0] = em_data->type;
    ptr[1] = ((em_data->first & 0x1) << 7) | ((em_data->last & 0x1) << 6);
    ptr[2] = em_data->sequence_index;

    /* First packet body */
    /* em data set header */
    ptr[3] = ((em_data->pkt_new & 0x1) << 7) | ((em_data->pkt_end & 0x1) << 6) |
             ((em_data->ds_type & 0x3) << 4) | ((em_data->afr & 0x1) << 3) |
             ((em_data->vfr & 0x1) << 2) | ((em_data->sync & 0x1) << 1);
    ptr[4] = 0; /* Reserved */
    ptr[5] = em_data->organization_id;
    ptr[6] = (em_data->data_set_tag >> 8) & 0xff;
    ptr[7] = em_data->data_set_tag & 0xff;
    ptr[8] = (em_data->data_set_length >> 8) & 0xff;
    ptr[9] = em_data->data_set_length & 0xff;

    /* em data set body */
    return length;
}
EXPORT_SYMBOL(hdmi_cta_hdr4_ext_metadata_pack_only);

ssize_t hdmi_cta_hdr4_ext_metadata_pack(struct hdmi_cta_hdr4_ext_metadata *em_data,
                                        void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_cta_hdr4_ext_metadata_check(em_data);
    if (ret) {
        return ret;
    }

    return hdmi_cta_hdr4_ext_metadata_pack_only(em_data, buffer, size);
}
EXPORT_SYMBOL(hdmi_cta_hdr4_ext_metadata_pack);

hi_s32 hdmi_cta_any_ext_metadata_check_only(const union hdmi_cta_extended_metadata *em_data)
{
    if (em_data->any.type != HDMI_PACKET_TYPE_EXTENDED_METADATA ||
        em_data->any.organization_id != HDMI_CTA_ORGANIZATION_ID) {
        return -EINVAL;
    }

    return 0;
}

hi_s32 hdmi_cta_any_ext_metadata_check(union hdmi_cta_extended_metadata *em_data)
{
    hi_s32 ret;

    ret = hdmi_cta_any_ext_metadata_check_only(em_data);
    if (ret) {
        return ret;
    }

    if (em_data->any.data_set_tag == HDMI_CTA_HDR1_EM_DATA_SET_TAG) {
        ret = hdmi_cta_hdr1_ext_metadata_check(&em_data->hdr1);
    } else if (em_data->any.data_set_tag == HDMI_CTA_HDR2_EM_DATA_SET_TAG) {
        ret = hdmi_cta_hdr2_ext_metadata_check(&em_data->hdr2);
    } else if (em_data->any.data_set_tag == HDMI_CTA_HDR3_EM_DATA_SET_TAG) {
        ret = hdmi_cta_hdr3_ext_metadata_check(&em_data->hdr3);
    } else if (em_data->any.data_set_tag == HDMI_CTA_HDR4_EM_DATA_SET_TAG) {
        ret = hdmi_cta_hdr4_ext_metadata_check(&em_data->hdr4);
    } else {
        return -EINVAL;
    }

    return ret;
}

ssize_t hdmi_cta_any_ext_metadata_pack_only(const union hdmi_cta_extended_metadata *em_data,
                                            void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_cta_any_ext_metadata_check_only(em_data);
    if (ret) {
        return ret;
    }

    if (em_data->any.data_set_tag == HDMI_CTA_HDR1_EM_DATA_SET_TAG) {
        ret = hdmi_cta_hdr1_ext_metadata_pack_only(&em_data->hdr1, buffer, size);
    } else if (em_data->any.data_set_tag == HDMI_CTA_HDR2_EM_DATA_SET_TAG) {
        ret = hdmi_cta_hdr2_ext_metadata_pack_only(&em_data->hdr2, buffer, size);
    } else if (em_data->any.data_set_tag == HDMI_CTA_HDR3_EM_DATA_SET_TAG) {
        ret = hdmi_cta_hdr3_ext_metadata_pack_only(&em_data->hdr3, buffer, size);
    } else if (em_data->any.data_set_tag == HDMI_CTA_HDR4_EM_DATA_SET_TAG) {
        ret = hdmi_cta_hdr4_ext_metadata_pack_only(&em_data->hdr4, buffer, size);
    } else {
        return -EINVAL;
    }

    return ret;
}

ssize_t hdmi_cta_any_ext_metadata_pack(union hdmi_cta_extended_metadata *em_data,
                                       void *buffer, size_t size)
{
    hi_s32 ret;

    ret = hdmi_cta_any_ext_metadata_check(em_data);
    if (ret) {
        return ret;
    }

    return hdmi_cta_any_ext_metadata_pack_only(em_data, buffer, size);
}

hi_s32 hdmi_extended_metadata_check(union hdmi_extended_metadata *em_data)
{
    switch (em_data->any.organization_id) {
        case HDMI_ORGANIZATION_ID:
            return hdmi_hdmi_any_ext_metadata_check(&em_data->hdmi);
        case HDMI_CTA_ORGANIZATION_ID:
            return hdmi_cta_any_ext_metadata_check(&em_data->cta);
        default:
            HDMI_WARN("Bad extended metadata type %d\n", em_data->any.organization_id);
            return -EINVAL;
    }
}
EXPORT_SYMBOL(hdmi_extended_metadata_check);

ssize_t hdmi_extended_metadata_pack_only(const union hdmi_extended_metadata *em_data,
                                         void *buffer, size_t size)
{
    ssize_t length;

    switch (em_data->any.organization_id) {
        case HDMI_ORGANIZATION_ID:
            length = hdmi_hdmi_any_ext_metadata_pack_only(&em_data->hdmi,
                                                          buffer, size);
            break;
        case HDMI_CTA_ORGANIZATION_ID:
            length = hdmi_cta_any_ext_metadata_pack_only(&em_data->cta,
                                                         buffer, size);
            break;
        default:
            HDMI_WARN("Bad extended metadata type %d\n", em_data->any.organization_id);
            length = -EINVAL;
    }

    return length;
}
EXPORT_SYMBOL(hdmi_extended_metadata_pack_only);

ssize_t hdmi_extended_metadata_pack(union hdmi_extended_metadata *em_data, void *buffer,
                                    size_t size)
{
    ssize_t length;

    switch (em_data->any.organization_id) {
        case HDMI_ORGANIZATION_ID:
            length = hdmi_hdmi_any_ext_metadata_pack(&em_data->hdmi,
                                                     buffer, size);
            break;
        case HDMI_CTA_ORGANIZATION_ID:
            length = hdmi_cta_any_ext_metadata_pack(&em_data->cta,
                                                    buffer, size);
            break;
        default:
            HDMI_WARN("Bad extended metadata type %d\n", em_data->any.organization_id);
            length = -EINVAL;
    }

    return length;
}
EXPORT_SYMBOL(hdmi_extended_metadata_pack);

static const hi_s8 *hdmi_extended_metadata_type_get_name(hi_u8 organization_id)
{
    if (organization_id < 0x0 || organization_id > 0xff) {
        return "Invalid";
    }

    switch (organization_id) {
        case HDMI_ORGANIZATION_ID:
            return "HDMI2.1";
        case HDMI_CTA_ORGANIZATION_ID:
            return "CTA861-G";
    }

    return "Reserved";
}

static void hdmi_extended_metadata_log_header(const struct hdmi_any_extended_metadata *em_data)
{
    const hi_s8 *name = HI_NULL;

    name = hdmi_extended_metadata_type_get_name(em_data->organization_id);
    HDMI_INFO("HDMI packet type: extended metadata\n");
    HDMI_INFO("First: %s\n", em_data->first ? "true" : "false");
    HDMI_INFO("Last: %s\n", em_data->last ? "true" : "false");
    HDMI_INFO("Sequence index: %d\n", em_data->sequence_index);
    HDMI_INFO("New: %s\n", em_data->pkt_new ? "true" : "false");
    HDMI_INFO("End: %s\n", em_data->pkt_end ? "true" : "false");
    HDMI_INFO("Ds type: %u\n", em_data->ds_type);
    HDMI_INFO("Afr: %s\n", em_data->afr ? "true" : "false");
    HDMI_INFO("Vfr: %s\n", em_data->vfr ? "true" : "false");
    HDMI_INFO("Sync: %s\n", em_data->sync ? "true" : "false");
    HDMI_INFO("HDMI extended metadata type: %s\n", name);
}

static void hdmi_hdmi_vt_ext_metadata_log(const struct hdmi_hdmi_vt_ext_metadata *em_data)
{
    hdmi_extended_metadata_log_header((const struct hdmi_any_extended_metadata *)em_data);

    if (em_data->data_set_tag != HDMI_VT_EM_DATA_SET_TAG) {
        HDMI_INFO("   not a HDMI Video timing extended metadata\n");
        return;
    }

    HDMI_INFO("   this is a HDMI Video timing extended metadata\n");
    HDMI_INFO("   Data set length: %u\n", em_data->data_set_length);
    HDMI_INFO("   FVA_Factor_M1: %u\n", em_data->fva_factor_m1);
    HDMI_INFO("   M_CONST: %u\n", em_data->m_const);
    HDMI_INFO("   VRR_EN: %u\n", em_data->vrr_en);
    HDMI_INFO("   Base_Vfront: %u\n", em_data->base_vfront);
    HDMI_INFO("   RB: %u\n", em_data->rb);
    HDMI_INFO("   Base_Refresh_Rate: %u\n", em_data->base_refresh_rate);
}

static void hdmi_hdmi_dsc_ext_metadata_log(const struct hdmi_hdmi_dsc_ext_metadata *em_data)
{
    hi_s32 i;

    hdmi_extended_metadata_log_header((const struct hdmi_any_extended_metadata *)em_data);

    if (em_data->data_set_tag != HDMI_DSC_EM_DATA_SET_TAG) {
        HDMI_INFO("   not a HDMI DSC extended metadata\n");
        return;
    }

    HDMI_INFO("   this is a HDMI DSC extended metadata\n");

    HDMI_INFO("   Data set length: %u\n", em_data->data_set_length);
    HDMI_INFO("   DSC version: %u.%u\n", em_data->dsc_version_major,
              em_data->dsc_version_minor);
    HDMI_INFO("   pps_identifier: %u\n", em_data->pps_identifier);
    HDMI_INFO("   bits_per_component: %u\n", em_data->bits_per_component);
    HDMI_INFO("   linebuf_depth: %u\n", em_data->linebuf_depth);
    HDMI_INFO("   block_pred_enable: %u\n", em_data->block_pred_enable);
    HDMI_INFO("   convert_rgb: %u\n", em_data->convert_rgb);
    HDMI_INFO("   simple_422: %u\n", em_data->simple_422);
    HDMI_INFO("   vbr_enable: %u\n", em_data->vbr_enable);
    HDMI_INFO("   bits_per_pixel: %u\n", em_data->bits_per_pixel);
    HDMI_INFO("   pic_height: %u\n", em_data->pic_height);
    HDMI_INFO("   pic_width: %u\n", em_data->pic_width);
    HDMI_INFO("   slice_height: %u\n", em_data->slice_height);
    HDMI_INFO("   slice_width: %u\n", em_data->slice_width);
    HDMI_INFO("   chunk_size: %u\n", em_data->chunk_size);
    HDMI_INFO("   initial_xmit_delay: %u\n", em_data->initial_xmit_delay);
    HDMI_INFO("   initial_dec_delay: %u\n", em_data->initial_dec_delay);
    HDMI_INFO("   initial_scale_value: %u\n", em_data->initial_scale_value);
    HDMI_INFO("   scale_increment_interval: %u\n", em_data->scale_increment_interval);
    HDMI_INFO("   scale_decrement_interval: %u\n", em_data->scale_decrement_interval);
    HDMI_INFO("   first_line_bpg_offset: %u\n", em_data->first_line_bpg_offset);
    HDMI_INFO("   nfl_bpg_offset: %u\n", em_data->nfl_bpg_offset);
    HDMI_INFO("   slice_bpg_offset: %u\n", em_data->slice_bpg_offset);
    HDMI_INFO("   inital_offset: %u\n", em_data->inital_offset);
    HDMI_INFO("   final_offset: %u\n", em_data->final_offset);
    HDMI_INFO("   flatness_min_qp: %u\n", em_data->flatness_min_qp);
    HDMI_INFO("   flatness_max_qp: %u\n", em_data->flatness_max_qp);
    HDMI_INFO("   rc_model_size: %u\n", em_data->rc_model_size);
    HDMI_INFO("   rc_edge_factor: %u\n", em_data->rc_edge_factor);
    HDMI_INFO("   rc_quant_incr_limit0: %u\n", em_data->rc_quant_incr_limit0);
    HDMI_INFO("   rc_quant_incr_limit1: %u\n", em_data->rc_quant_incr_limit1);
    HDMI_INFO("   rc_tgt_offset_hi: %u\n", em_data->rc_tgt_offset_hi);
    HDMI_INFO("   rc_tgt_offset_lo: %u\n", em_data->rc_tgt_offset_lo);
    for (i = 0; i < NUM_BUF_RANGES - 1; i++) {
        HDMI_INFO("   rc_buf_thresh[%d]: %u\n", i, em_data->rc_buf_thresh[i]);
    }

    for (i = 0; i < NUM_BUF_RANGES; i++) {
        HDMI_INFO("   rc_parameters[%d].range_min_qp: %u\n", i,
                  em_data->rc_parameters[i].range_min_qp);
        HDMI_INFO("   rc_parameters[%d].range_max_qp: %u\n", i,
                  em_data->rc_parameters[i].range_max_qp);
        HDMI_INFO("   rc_parameters[%d].range_bpg_offset: %u\n", i,
                  em_data->rc_parameters[i].range_bpg_offset);
    }

    HDMI_INFO("   native_420: %u\n", em_data->native_420);
    HDMI_INFO("   native_422: %u\n", em_data->native_422);
    HDMI_INFO("   second_line_bpg_offset: %u\n", em_data->second_line_bpg_offset);
    HDMI_INFO("   nsl_bpg_offset: %u\n", em_data->nsl_bpg_offset);
    HDMI_INFO("   second_line_offset_adj: %u\n", em_data->second_line_offset_adj);
    HDMI_INFO("   hfront: %u\n", em_data->hfront);
    HDMI_INFO("   hsync: %u\n", em_data->hsync);
    HDMI_INFO("   hback: %u\n", em_data->hback);
    HDMI_INFO("   hcactive: %u\n", em_data->hcactive);
}

static void hdmi_hdmi_any_ext_metadata_log(const union hdmi_hdmi_extended_metadata *em_data)
{
    if (em_data->any.data_set_tag == HDMI_VT_EM_DATA_SET_TAG) {
        hdmi_hdmi_vt_ext_metadata_log(&em_data->vt);
    } else if (em_data->any.data_set_tag == HDMI_DSC_EM_DATA_SET_TAG) {
        hdmi_hdmi_dsc_ext_metadata_log(&em_data->dsc);
    } else {
        hdmi_extended_metadata_log_header((const struct hdmi_any_extended_metadata *)em_data);
        HDMI_INFO("   is unknow HDMI extended metadata\n");
    }
}

static void hdmi_cta_hdr1_ext_metadata_log(const struct hdmi_cta_hdr1_ext_metadata *em_data)
{
    hdmi_extended_metadata_log_header((const struct hdmi_any_extended_metadata *)em_data);

    if (em_data->data_set_tag != HDMI_CTA_HDR1_EM_DATA_SET_TAG) {
        HDMI_INFO("   not a HDMI CTA hdr type1 extended metadata\n");
        return;
    }

    HDMI_INFO("   this is a HDMI CTA hdr type1 extended metadata\n");

    HDMI_INFO("   Data set length: %u\n", em_data->data_set_length);
}

static void hdmi_cta_hdr2_ext_metadata_log(const struct hdmi_cta_hdr2_ext_metadata *em_data)
{
    hdmi_extended_metadata_log_header((const struct hdmi_any_extended_metadata *)em_data);

    if (em_data->data_set_tag != HDMI_CTA_HDR2_EM_DATA_SET_TAG) {
        HDMI_INFO("   not a HDMI CTA hdr type2 extended metadata\n");
        return;
    }

    HDMI_INFO("   this is a HDMI CTA hdr type2 extended metadata\n");

    HDMI_INFO("   Data set length: %u\n", em_data->data_set_length);
}

static void hdmi_cta_hdr3_ext_metadata_log(const struct hdmi_cta_hdr3_ext_metadata *em_data)
{
    if (em_data->data_set_tag != HDMI_CTA_HDR3_EM_DATA_SET_TAG) {
        HDMI_INFO("   not a HDMI CTA hdr type3 extended metadata\n");
        return;
    }

    HDMI_INFO("   this is a HDMI CTA hdr type3 extended metadata\n");

    HDMI_INFO("   Data set length: %u\n", em_data->data_set_length);
}

static void hdmi_cta_hdr4_ext_metadata_log(const struct hdmi_cta_hdr4_ext_metadata *em_data)
{
    if (em_data->data_set_tag != HDMI_CTA_HDR4_EM_DATA_SET_TAG) {
        HDMI_INFO("   not a HDMI CTA hdr type4 extended metadata\n");
        return;
    }

    HDMI_INFO("   this is a HDMI CTA hdr type4 extended metadata\n");

    HDMI_INFO("   Data set length: %u\n", em_data->data_set_length);
}

static void hdmi_cta_any_ext_metadata_log(const union hdmi_cta_extended_metadata *em_data)
{
    if (em_data->any.data_set_tag == HDMI_CTA_HDR1_EM_DATA_SET_TAG) {
        hdmi_cta_hdr1_ext_metadata_log(&em_data->hdr1);
    } else if (em_data->any.data_set_tag == HDMI_CTA_HDR2_EM_DATA_SET_TAG) {
        hdmi_cta_hdr2_ext_metadata_log(&em_data->hdr2);
    } else if (em_data->any.data_set_tag == HDMI_CTA_HDR3_EM_DATA_SET_TAG) {
        hdmi_cta_hdr3_ext_metadata_log(&em_data->hdr3);
    } else if (em_data->any.data_set_tag == HDMI_CTA_HDR4_EM_DATA_SET_TAG) {
        hdmi_cta_hdr4_ext_metadata_log(&em_data->hdr4);
    } else {
        hdmi_extended_metadata_log_header((const struct hdmi_any_extended_metadata *)em_data);
        HDMI_INFO("   is unknow HDMI extended metadata\n");
    }
}

void hdmi_extended_metadata_log(const union hdmi_extended_metadata *em_data)
{
    switch (em_data->any.organization_id) {
        case HDMI_ORGANIZATION_ID:
            hdmi_hdmi_any_ext_metadata_log(&em_data->hdmi);
            break;
        case HDMI_CTA_ORGANIZATION_ID:
            hdmi_cta_any_ext_metadata_log(&em_data->cta);
            break;
        default:
            break;
    }
}
EXPORT_SYMBOL(hdmi_extended_metadata_log);

static hi_s32 hdmi_hdmi_vt_ex_metadata_unpack(struct hdmi_hdmi_vt_ext_metadata *em_data,
                                              const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    if (size < HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE) {
        return -EINVAL;
    }

    if (ptr[0] != HDMI_PACKET_TYPE_EXTENDED_METADATA ||
        ptr[1] != 0xc0 || ptr[2] != 0x00 ||
        ptr[3] != 0x84 || ptr[4] != 0x00 ||
        ptr[5] != HDMI_ORGANIZATION_ID ||
        ptr[6] != 0x00 || ptr[7] != 0x01) {
        return -EINVAL;
    }

    length = ((ptr[8] & 0xff) << 8) | (ptr[9] & 0xff);
    if (length != HDMI_VT_EM_DATA_SIZE) {
        return -EINVAL;
    }

    if (size < (HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE + length)) {
        return -EINVAL;
    }

    ptr += (HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE);

    ret = hdmi_hdmi_vt_ext_metadata_init(em_data);
    if (ret) {
        return ret;
    }

    em_data->data_set_length = length;
    em_data->fva_factor_m1 = (ptr[0] >> 4) & 0xf;
    em_data->m_const = (ptr[0] >> 1) & 0x1;
    em_data->vrr_en = ptr[0] & 0x1;
    em_data->base_vfront = ptr[1];
    em_data->rb = (ptr[2] >> 2) & 0x1;
    em_data->base_refresh_rate = ((ptr[2] & 0x3) << 8) | ptr[3];

    return 0;
}

static hi_s32 hdmi_hdmi_dsc_ex_metadata_unpack(struct hdmi_hdmi_dsc_ext_metadata *em_data,
                                               const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    if (size < HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE) {
        return -EINVAL;
    }

    if (ptr[0] != HDMI_PACKET_TYPE_EXTENDED_METADATA ||
        ptr[1] != 0x80 || ptr[2] != 0x00 ||
        ptr[3] != 0x86 || ptr[4] != 0x00 ||
        ptr[5] != HDMI_ORGANIZATION_ID ||
        ptr[6] != 0x00 || ptr[7] != 0x02) {
        return -EINVAL;
    }

    length = ((ptr[8] & 0xff) << 8) | (ptr[9] & 0xff);

    if (length != HDMI_DSC_EM_DATA_SIZE) {
        return -EINVAL;
    }

    if (size <
        (HDMI_PACKET_HEADER_SIZE *
         HDMI_DSC_EM_DATA_PACKET_NUM +
         HDMI_DSF_HEADE_SIZE + length)) {
        return -EINVAL;
    }

    ret = hdmi_hdmi_dsc_ext_metadata_init(em_data);
    if (ret) {
        return ret;
    }

    em_data->data_set_length = length;
    em_data->dsc_version_major = (ptr[10] >> 4) & 0xf;
    em_data->dsc_version_minor = ptr[10] & 0xf;
    em_data->pps_identifier = ptr[11];
    em_data->bits_per_component = (ptr[13] >> 4) & 0xf;
    em_data->linebuf_depth = ptr[13] & 0xf;
    em_data->block_pred_enable = (ptr[14] >> 5) & 0x1;
    em_data->convert_rgb = (ptr[14] >> 4) & 0x1;
    em_data->simple_422 = (ptr[14] >> 3) & 0x1;
    em_data->vbr_enable = (ptr[14] >> 2) & 0x1;
    em_data->bits_per_pixel = ((ptr[14] & 0x3) << 8) | ptr[15];
    em_data->pic_height = (ptr[16] << 8) | ptr[17];
    em_data->pic_width = (ptr[18] << 8) | ptr[19];
    em_data->slice_height = (ptr[20] << 8) | ptr[21];
    em_data->slice_width = (ptr[22] << 8) | ptr[23];
    em_data->chunk_size = (ptr[24] << 8) | ptr[25];
    em_data->initial_xmit_delay = ((ptr[26] & 0x3) << 8) | ptr[27];
    em_data->initial_dec_delay = (ptr[28] << 8) | ptr[29];

    ptr += HDMI_PACKET_SIZE;
    em_data->initial_scale_value = ptr[3] & 0x3f;
    em_data->scale_increment_interval = (ptr[4] << 8) | ptr[5];
    em_data->scale_decrement_interval = ((ptr[6] & 0xf) << 8) | ptr[7];
    em_data->first_line_bpg_offset = ptr[9] & 0x1f;
    em_data->nfl_bpg_offset = (ptr[10] << 8) | ptr[11];
    em_data->slice_bpg_offset = (ptr[12] << 8) | ptr[13];
    em_data->inital_offset = (ptr[14] << 8) | ptr[15];
    em_data->final_offset = (ptr[16] << 8) | ptr[17];
    em_data->flatness_min_qp = ptr[18] & 0x1f;
    em_data->flatness_max_qp = ptr[19] & 0x1f;
    em_data->rc_model_size = (ptr[20] << 8) | ptr[21];
    em_data->rc_edge_factor = ptr[22] & 0xf;
    em_data->rc_quant_incr_limit0 = ptr[23] & 0x1f;
    em_data->rc_quant_incr_limit1 = ptr[24] & 0x1f;
    em_data->rc_tgt_offset_hi = (ptr[25] >> 4) & 0xf;
    em_data->rc_tgt_offset_lo = ptr[25] & 0xf;
    em_data->rc_buf_thresh[0] = ptr[26];
    em_data->rc_buf_thresh[1] = ptr[27];
    em_data->rc_buf_thresh[2] = ptr[28];
    em_data->rc_buf_thresh[3] = ptr[29];
    em_data->rc_buf_thresh[4] = ptr[30];

    ptr += HDMI_PACKET_SIZE;
    em_data->rc_buf_thresh[5] = ptr[3];
    em_data->rc_buf_thresh[6] = ptr[4];
    em_data->rc_buf_thresh[7] = ptr[5];
    em_data->rc_buf_thresh[8] = ptr[6];
    em_data->rc_buf_thresh[9] = ptr[7];
    em_data->rc_buf_thresh[10] = ptr[8];
    em_data->rc_buf_thresh[11] = ptr[9];
    em_data->rc_buf_thresh[12] = ptr[10];
    em_data->rc_buf_thresh[13] = ptr[11];
    em_data->rc_parameters[0].range_min_qp = (ptr[12] >> 3) & 0x1f;
    em_data->rc_parameters[0].range_max_qp = ((ptr[12] & 0x7) << 2) |
                                             ((ptr[13] >> 6) & 0x3);
    em_data->rc_parameters[0].range_bpg_offset = ptr[13] & 0x3f;

    em_data->rc_parameters[1].range_min_qp = (ptr[14] >> 3) & 0x1f;
    em_data->rc_parameters[1].range_max_qp = ((ptr[14] & 0x7) << 2) |
                                             ((ptr[15] >> 6) & 0x3);
    em_data->rc_parameters[1].range_bpg_offset = ptr[15] & 0x3f;

    em_data->rc_parameters[2].range_min_qp = (ptr[16] >> 3) & 0x1f;
    em_data->rc_parameters[2].range_max_qp = ((ptr[16] & 0x7) << 2) |
                                             ((ptr[17] >> 6) & 0x3);
    em_data->rc_parameters[2].range_bpg_offset = ptr[17] & 0x3f;

    em_data->rc_parameters[3].range_min_qp = (ptr[18] >> 3) & 0x1f;
    em_data->rc_parameters[3].range_max_qp = ((ptr[18] & 0x7) << 2) |
                                             ((ptr[19] >> 6) & 0x3);
    em_data->rc_parameters[3].range_bpg_offset = ptr[19] & 0x3f;

    em_data->rc_parameters[4].range_min_qp = (ptr[20] >> 3) & 0x1f;
    em_data->rc_parameters[4].range_max_qp = ((ptr[20] & 0x7) << 2) |
                                             ((ptr[21] >> 6) & 0x3);
    em_data->rc_parameters[4].range_bpg_offset = ptr[21] & 0x3f;

    em_data->rc_parameters[5].range_min_qp = (ptr[22] >> 3) & 0x1f;
    em_data->rc_parameters[5].range_max_qp = ((ptr[22] & 0x7) << 2) |
                                             ((ptr[23] >> 6) & 0x3);
    em_data->rc_parameters[5].range_bpg_offset = ptr[23] & 0x3f;

    em_data->rc_parameters[6].range_min_qp = (ptr[24] >> 3) & 0x1f;
    em_data->rc_parameters[6].range_max_qp = ((ptr[24] & 0x7) << 2) |
                                             ((ptr[25] >> 6) & 0x3);
    em_data->rc_parameters[6].range_bpg_offset = ptr[25] & 0x3f;

    em_data->rc_parameters[7].range_min_qp = (ptr[26] >> 3) & 0x1f;
    em_data->rc_parameters[7].range_max_qp = ((ptr[26] & 0x7) << 2) |
                                             ((ptr[27] >> 6) & 0x3);
    em_data->rc_parameters[7].range_bpg_offset = ptr[27] & 0x3f;

    em_data->rc_parameters[8].range_min_qp = (ptr[28] >> 3) & 0x1f;
    em_data->rc_parameters[8].range_max_qp = ((ptr[28] & 0x7) << 2) |
                                             ((ptr[29] >> 6) & 0x3);
    em_data->rc_parameters[8].range_bpg_offset = ptr[29] & 0x3f;

    em_data->rc_parameters[9].range_min_qp = (ptr[30] >> 3) & 0x1f;
    em_data->rc_parameters[9].range_max_qp = ((ptr[30] & 0x7) << 2);

    ptr += HDMI_PACKET_SIZE;
    em_data->rc_parameters[9].range_max_qp |= (ptr[3] >> 6) & 0x3;
    em_data->rc_parameters[9].range_bpg_offset = ptr[3] & 0x3f;

    em_data->rc_parameters[10].range_min_qp = (ptr[4] >> 3) & 0x1f;
    em_data->rc_parameters[10].range_max_qp = ((ptr[4] & 0x7) << 2) |
                                              ((ptr[5] >> 6) & 0x3);
    em_data->rc_parameters[10].range_bpg_offset = ptr[5] & 0x3f;

    em_data->rc_parameters[11].range_min_qp = (ptr[6] >> 3) & 0x1f;
    em_data->rc_parameters[11].range_max_qp = ((ptr[6] & 0x7) << 2) |
                                              ((ptr[7] >> 6) & 0x3);
    em_data->rc_parameters[11].range_bpg_offset = ptr[7] & 0x3f;

    em_data->rc_parameters[12].range_min_qp = (ptr[8] >> 3) & 0x1f;
    em_data->rc_parameters[12].range_max_qp = ((ptr[8] & 0x7) << 2) |
                                              ((ptr[9] >> 6) & 0x3);
    em_data->rc_parameters[12].range_bpg_offset = ptr[9] & 0x3f;

    em_data->rc_parameters[13].range_min_qp = (ptr[10] >> 3) & 0x1f;
    em_data->rc_parameters[13].range_max_qp = ((ptr[10] & 0x7) << 2) |
                                              ((ptr[11] >> 6) & 0x3);
    em_data->rc_parameters[13].range_bpg_offset = ptr[11] & 0x3f;

    em_data->rc_parameters[14].range_min_qp = (ptr[12] >> 3) & 0x1f;
    em_data->rc_parameters[14].range_max_qp = ((ptr[12] & 0x7) << 2) |
                                              ((ptr[13] >> 6) & 0x3);
    em_data->rc_parameters[14].range_bpg_offset = ptr[13] & 0x3f;

    em_data->native_420 = (ptr[14] >> 1) & 0x1;
    em_data->native_422 = ptr[14] & 0x1;
    em_data->second_line_bpg_offset = ptr[15] & 0x1f;
    em_data->nsl_bpg_offset = (ptr[16] << 8) | ptr[17];
    em_data->second_line_offset_adj = (ptr[18] << 8) | ptr[19];

    ptr += HDMI_PACKET_SIZE;
    em_data->hfront = (ptr[27] << 8) | ptr[26];
    em_data->hsync = (ptr[29] << 8) | ptr[28];
    em_data->hback = ptr[30];

    ptr += HDMI_PACKET_SIZE;
    em_data->hback |= (ptr[3] << 8);
    em_data->hcactive = (ptr[5] << 8) | ptr[4];

    return 0;
}

hi_s32 hdmi_hdmi_any_ex_metadata_unpack(union hdmi_hdmi_extended_metadata *em_data,
                                        const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    hi_s32 ret;

    if ((ptr[6] == 0x00) && (ptr[7] == 0x01)) {
        ret = hdmi_hdmi_vt_ex_metadata_unpack(&em_data->vt, buffer, size);
    } else if ((ptr[6] == 0x00) && (ptr[7] == 0x02)) {
        ret = hdmi_hdmi_dsc_ex_metadata_unpack(&em_data->dsc, buffer, size);
    } else {
        return -EINVAL;
    }

    return ret;
}

static hi_s32 hdmi_cta_hdr1_ex_metadata_unpack(struct hdmi_cta_hdr1_ext_metadata *em_data,
                                               const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    if (size < HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE) {
        return -EINVAL;
    }

    if (ptr[0] != HDMI_PACKET_TYPE_EXTENDED_METADATA ||
        ptr[1] != 0xc0 /* need confirm */ || ptr[2] != 0x00 ||
        ptr[3] != 0x96 || ptr[4] != 0x00 ||
        ptr[5] != HDMI_CTA_ORGANIZATION_ID ||
        ptr[6] != 0x00 || ptr[7] != 0x01) {
        return -EINVAL;
    }

    length = ((ptr[8] & 0xff) << 8) | (ptr[9] & 0xff);

    if (length != HDMI_CTA_HDR1_EM_DATA_SIZE) {
        return -EINVAL;
    }

    if (size < (HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE + length)) {
        return -EINVAL;
    }

    ret = hdmi_cta_hdr1_ext_metadata_init(em_data);
    if (ret) {
        return ret;
    }

    em_data->data_set_length = length;

    return 0;
}

static hi_s32 hdmi_cta_hdr2_ex_metadata_unpack(struct hdmi_cta_hdr2_ext_metadata *em_data,
                                               const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    if (size < HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE) {
        return -EINVAL;
    }

    if (ptr[0] != HDMI_PACKET_TYPE_EXTENDED_METADATA ||
        ptr[1] != 0xc0 /* need confirm */ || ptr[2] != 0x00 ||
        ptr[3] != 0x96 || ptr[4] != 0x00 ||
        ptr[5] != HDMI_CTA_ORGANIZATION_ID ||
        ptr[6] != 0x00 || ptr[7] != 0x02) {
        return -EINVAL;
    }

    length = ((ptr[8] & 0xff) << 8) | (ptr[9] & 0xff);

    if (length != HDMI_CTA_HDR2_EM_DATA_SIZE) {
        return -EINVAL;
    }

    if (size < (HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE + length)) {
        return -EINVAL;
    }

    ret = hdmi_cta_hdr2_ext_metadata_init(em_data);
    if (ret) {
        return ret;
    }

    em_data->data_set_length = length;

    return 0;
}

static hi_s32 hdmi_cta_hdr3_ex_metadata_unpack(struct hdmi_cta_hdr3_ext_metadata *em_data,
                                               const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    if (size < HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE) {
        return -EINVAL;
    }

    if (ptr[0] != HDMI_PACKET_TYPE_EXTENDED_METADATA ||
        ptr[1] != 0xc0 /* need confirm */ || ptr[2] != 0x00 ||
        ptr[3] != 0x96 || ptr[4] != 0x00 ||
        ptr[5] != HDMI_CTA_ORGANIZATION_ID ||
        ptr[6] != 0x00 || ptr[7] != 0x03) {
        return -EINVAL;
    }

    length = ((ptr[8] & 0xff) << 8) | (ptr[9] & 0xff);

    if (length != HDMI_CTA_HDR1_EM_DATA_SIZE) {
        return -EINVAL;
    }

    if (size < (HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE + length)) {
        return -EINVAL;
    }

    ret = hdmi_cta_hdr3_ext_metadata_init(em_data);
    if (ret) {
        return ret;
    }

    em_data->data_set_length = length;

    return 0;
}

static hi_s32 hdmi_cta_hdr4_ex_metadata_unpack(struct hdmi_cta_hdr4_ext_metadata *em_data,
                                               const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    size_t length;
    hi_s32 ret;

    if (size < HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE) {
        return -EINVAL;
    }

    if (ptr[0] != HDMI_PACKET_TYPE_EXTENDED_METADATA ||
        ptr[1] != 0xc0 /* need confirm */ || ptr[2] != 0x00 ||
        ptr[3] != 0x96 || ptr[4] != 0x00 ||
        ptr[5] != HDMI_CTA_ORGANIZATION_ID ||
        ptr[6] != 0x00 || ptr[7] != 0x04) {
        return -EINVAL;
    }

    length = ((ptr[8] & 0xff) << 8) | (ptr[9] & 0xff);

    if (length != HDMI_CTA_HDR1_EM_DATA_SIZE) {
        return -EINVAL;
    }

    if (size < (HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE + length)) {
        return -EINVAL;
    }

    ret = hdmi_cta_hdr4_ext_metadata_init(em_data);
    if (ret) {
        return ret;
    }

    em_data->data_set_length = length;

    return 0;
}

static hi_s32 hdmi_cta_any_ex_metadata_unpack(union hdmi_cta_extended_metadata *em_data,
                                              const void *buffer, size_t size)
{
    const hi_u8 *ptr = buffer;
    hi_s32 ret;

    if ((ptr[6] == 0x00) && (ptr[7] == 0x01)) {
        ret = hdmi_cta_hdr1_ex_metadata_unpack(&em_data->hdr1, buffer, size);
    } else if ((ptr[6] == 0x00) && (ptr[7] == 0x02)) {
        ret = hdmi_cta_hdr2_ex_metadata_unpack(&em_data->hdr2, buffer, size);
    } else if ((ptr[6] == 0x00) && (ptr[7] == 0x03)) {
        ret = hdmi_cta_hdr3_ex_metadata_unpack(&em_data->hdr3, buffer, size);
    } else if ((ptr[6] == 0x00) && (ptr[7] == 0x04)) {
        ret = hdmi_cta_hdr4_ex_metadata_unpack(&em_data->hdr4, buffer, size);
    } else {
        return -EINVAL;
    }

    return ret;
}

hi_s32 hdmi_extended_metadata_unpack(union hdmi_extended_metadata *em_data,
                                     const void *buffer, size_t size)
{
    hi_s32 ret;
    const hi_u8 *ptr = buffer;

    if (size < HDMI_PACKET_HEADER_SIZE + HDMI_DSF_HEADE_SIZE) {
        return -EINVAL;
    }

    switch (ptr[5]) {
        case HDMI_ORGANIZATION_ID:
            ret = hdmi_hdmi_any_ex_metadata_unpack(&em_data->hdmi, buffer, size);
            break;
        case HDMI_CTA_ORGANIZATION_ID:
            ret = hdmi_cta_any_ex_metadata_unpack(&em_data->cta, buffer, size);
            break;
        default:
            ret = -EINVAL;
            break;
    }

    return ret;
}
EXPORT_SYMBOL(hdmi_extended_metadata_unpack);


