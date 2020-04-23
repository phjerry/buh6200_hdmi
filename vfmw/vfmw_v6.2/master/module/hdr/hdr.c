/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "hdr_ext.h"
#include "dbg.h"
#include "linux_proc.h"
#include "hi_drv_video.h"

/* -----------------------------MACRO  ------------------------------------- */
#define HDR_OK  (0)
#define HDR_ERR (-1)

/* -----------------------------LOCAL DEFINITION---------------------------- */
#define MAX_VID_STD_NAME (20)
#define MAX_HDR_CHAN_NUM (2)

typedef struct {
    hi_drv_hdr_type en_src_frame_type;
    hi_drv_color_descript colour_info;
    hi_drv_hdr_metadata un_hdr_info;
} hdr_output;

typedef struct {
    hi_s32 chan_id;
    vfmw_vid_std vid_std;
    vfmw_hdr_info input;
    hdr_output output;
} hdr_proc;

hdr_proc g_hdr_chan[MAX_HDR_CHAN_NUM];

/* ----------------------------INTERNAL FUNCTION DECLEAR----------------------- */
STATIC hi_void hdr_check_gamma_type_hevc(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    hi_drv_color_transfer_curve transfer = HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR;

    switch (hdr_input->colour_info.transfer_characteristics) {
        case 11: /* 11 :a number */
            transfer = HI_DRV_COLOR_TRANSFER_TYPE_XVYCC;
            break;

        case 14: /* 14 :a number */
            if (hdr_input->comapitibility_info.hdr_transfer_characteristic_idc == 18) { /* 18 :a number */
                transfer = HI_DRV_COLOR_TRANSFER_TYPE_HLG;
            }
            break;

        case 16: /* 16 :a number */
            transfer = HI_DRV_COLOR_TRANSFER_TYPE_PQ;
            break;

        case 18: /* 18 :a number */
            transfer = HI_DRV_COLOR_TRANSFER_TYPE_HLG;
            break;

        default:
            break;
    }

    hdr_output_info->colour_info.transfer_type = transfer;

    return;
}

hi_void hdr_check_gamma_type_avs2(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    hi_drv_color_transfer_curve gamma_type = HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR;
    hi_u32 transfer_characteristics = hdr_input->colour_info.transfer_characteristics;

    switch (transfer_characteristics) {
        case 12: /* 12 :a number */
            gamma_type = HI_DRV_COLOR_TRANSFER_TYPE_PQ;
            break;

        case 14: /* 14 :a number */
            gamma_type = HI_DRV_COLOR_TRANSFER_TYPE_HLG;
            break;

        default:
            break;
    }

    hdr_output_info->colour_info.transfer_type = gamma_type;

    return;
}

hi_void hdr_get_color_space_hevc(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    hi_u8 matrix_coeffs = hdr_input->colour_info.matrix_coeffs;
    hi_u8 full_range_flag = hdr_input->colour_info.full_rangeFlag;

    hdr_output_info->colour_info.color_space = HI_DRV_COLOR_CS_YUV;
    hdr_output_info->colour_info.quantify_range = (full_range_flag == 1) ? HI_DRV_COLOR_FULL_RANGE
                                                  : HI_DRV_COLOR_LIMITED_RANGE;

    switch (matrix_coeffs) {
        case 1: /* 1 :a number */
            hdr_output_info->colour_info.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
            break;

        case 5: /* 5 :a number */
            hdr_output_info->colour_info.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT601_625;
            break;

        case 6: /* 6 is HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 */
            hdr_output_info->colour_info.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT601_525;
            break;

        case 9: /* 9 :a number */
            hdr_output_info->colour_info.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT;
            break;

        case 10: /* 10 :a number */
            hdr_output_info->colour_info.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT2020_CONSTANT;
            break;

        default:
            break;
    }

    return;
}

hi_void hdr_get_color_space_avs2(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    hi_u8 matrix_coeffs = hdr_input->colour_info.matrix_coeffs;
    hi_u8 full_range_flag = hdr_input->colour_info.full_rangeFlag;

    hdr_output_info->colour_info.color_space = HI_DRV_COLOR_CS_YUV;
    hdr_output_info->colour_info.quantify_range = (full_range_flag == 1) ? HI_DRV_COLOR_FULL_RANGE :
                                                  HI_DRV_COLOR_LIMITED_RANGE;

    switch (matrix_coeffs) {
        case 1: /* 1 :a number */
            hdr_output_info->colour_info.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT709;
            break;

        case 5: /* 5 :a number */
            hdr_output_info->colour_info.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT601_625;
            break;

        case 6: /* 6 means HI_DRV_COLOR_MATRIX_COEFFS_BT601_525 */
            hdr_output_info->colour_info.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT601_525;
            break;

        case 8: /* 8 :a number */
            hdr_output_info->colour_info.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT;
            break;

        case 9: /* 9 :a number */
            hdr_output_info->colour_info.matrix_coef = HI_DRV_COLOR_MATRIX_COEFFS_BT2020_CONSTANT;
            break;

        default:
            break;
    }

    return;
}

hi_void hdr_get_color_prime(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    hi_drv_color_primary color_prime = HI_DRV_COLOR_PRIMARY_BT709;
    hi_u8 colour_primaries = hdr_input->colour_info.colour_primaries;

    switch (colour_primaries) {
        case 1: /* 1 :a number */
            color_prime = HI_DRV_COLOR_PRIMARY_BT709;
            break;

        case 5: /* 5 :a number */
            color_prime = HI_DRV_COLOR_PRIMARY_BT601_625;
            break;

        case 6: /* 6 :a number */
            color_prime = HI_DRV_COLOR_PRIMARY_BT601_525;
            break;

        case 9: /* 9 :a number */
            color_prime = HI_DRV_COLOR_PRIMARY_BT2020;
            break;

        default:
            break;
    }

    hdr_output_info->colour_info.color_primary = color_prime;

    return;
}

static hi_void hdr_get_gamma_type_info(hi_drv_color_transfer_curve type, hi_char *gamma_type, hi_u32 size)
{
    switch (type) {
        case HI_DRV_COLOR_TRANSFER_TYPE_GAMMA_SDR:
            if (snprintf_s(gamma_type, size, size, "GAMMA_SDR") < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;

        case HI_DRV_COLOR_TRANSFER_TYPE_PQ:
            if (snprintf_s(gamma_type, size, size, "PQ") < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;

        case HI_DRV_COLOR_TRANSFER_TYPE_HLG:
            if (snprintf_s(gamma_type, size, size, "HLG") < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;

        default:
            if (snprintf_s(gamma_type, size, size, "UNKNOWN(%d)", type) < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;
    }

    return;
}

static hi_void hdr_get_color_space_info(hi_drv_color_space color_space_type, hi_char *color_space, hi_u32 size)
{
    switch (color_space_type) {
        case HI_DRV_COLOR_CS_YUV:
            if (snprintf_s(color_space, size, size, "YUV") < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;

        case HI_DRV_COLOR_CS_RGB:
            if (snprintf_s(color_space, size, size, "RGB") < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;

        default:
            if (snprintf_s(color_space, size, size, "UNKNOWN(%d)", color_space_type) < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;
    }

    return;
}

static hi_void hdr_get_luminace_sys_info(hi_drv_color_matrix_coeffs luminace_sys_type, hi_s8 *luminace_sys,
                                         hi_u32 size)
{
    switch (luminace_sys_type) {
        case HI_DRV_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT:
            if (snprintf_s(luminace_sys, size, size, "BT2020_NON_CONSTANCE") < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;

        case HI_DRV_COLOR_MATRIX_COEFFS_BT2020_CONSTANT:
            if (snprintf_s(luminace_sys, size, size, "BT2020_CONSTANCE") < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;

        case HI_DRV_COLOR_MATRIX_COEFFS_BT709:
            if (snprintf_s(luminace_sys, size, size, "BT709") < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;

        default:
            if (snprintf_s(luminace_sys, size, size, "HI_DRV_LUMINACE_SYS_BUTT(%d)", luminace_sys_type) < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;
    }

    return;
}

static hi_void hdr_get_color_prime_info(hi_drv_color_primary color_prime_type, hi_s8 *color_prime, hi_u32 size)
{
    switch (color_prime_type) {
        case HI_DRV_COLOR_PRIMARY_BT709:
            if (snprintf_s(color_prime, size, size, "PRIMARY_BT709") < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;

        case HI_DRV_COLOR_PRIMARY_BT601_525:
            if (snprintf_s(color_prime, size, size, "PRIMARY_BT601_525") < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;

        case HI_DRV_COLOR_PRIMARY_BT601_625:
            if (snprintf_s(color_prime, size, size, "PRIMARY_BT601_625") < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;

        case HI_DRV_COLOR_PRIMARY_BT2020:
            if (snprintf_s(color_prime, size, size, "PRIMARY_BT2020") < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;

        default:
            if (snprintf_s(color_prime, size, size, "HI_DRV_PRIMARY_COLOR_BUTT(%d)", color_prime_type) < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            break;
    }

    return;
}

STATIC hi_void hdr_check_type_hevc(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    hi_drv_hdr_type src_frame_type = HI_DRV_HDR_TYPE_SDR;
    hdr_input->is_backwards_compatible = 0;

    switch (hdr_input->colour_info.transfer_characteristics) {
        case 14: /* 14 :a number */
            if (hdr_input->comapitibility_info.hdr_transfer_characteristic_idc == 18) { /* 18 :a number */
                src_frame_type = HI_DRV_HDR_TYPE_HLG;
                hdr_input->is_backwards_compatible = 1;
            } else {
                src_frame_type = HI_DRV_HDR_TYPE_SDR;
            }
            break;

        case 16: /* 16 :a number */
            src_frame_type = HI_DRV_HDR_TYPE_HDR10;
            break;

        case 18: /* 18 :a number */
            src_frame_type = HI_DRV_HDR_TYPE_HLG;
            break;

        default:
            src_frame_type = HI_DRV_HDR_TYPE_SDR;
            break;
    }

    if (hdr_input->be_dv_hdr_chan == 1) {
        if (hdr_input->dv_capacity == DV_BL_DECODING_SUPPORT || hdr_input->dv_capacity == DV_EL_DECODING_SUPPORT) {
            src_frame_type = HI_DRV_HDR_TYPE_DOLBYVISION;
        } else {
            dprint(PRN_ERROR, "[%s %d] invalid dv_capacity = %d !!\n", __func__, __LINE__, hdr_input->dv_capacity);
        }
    }

    if (hdr_input->s1_hdr1_metadata_available) {
        src_frame_type = HI_DRV_HDR_TYPE_JTP_SL_HDR;
    }

#ifdef HI_HDR10PLUS_SUPPORT
    if (hdr_input->dynamic_metadata_valid) {
        src_frame_type = HI_DRV_HDR_TYPE_HDR10PLUS;
    }
#endif

    hdr_output_info->en_src_frame_type = src_frame_type;

    hdr_check_gamma_type_hevc(hdr_input, hdr_output_info);

    return;
}

STATIC hi_void hdr_check_type_h264(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    hi_drv_hdr_type src_frame_type = HI_DRV_HDR_TYPE_SDR;
    hdr_input->is_backwards_compatible = 0;

    if (hdr_input->be_dv_hdr_chan == 1) {
        if (hdr_input->dv_capacity == DV_BL_DECODING_SUPPORT || hdr_input->dv_capacity == DV_EL_DECODING_SUPPORT) {
            src_frame_type = HI_DRV_HDR_TYPE_DOLBYVISION;
        } else {
            dprint(PRN_ERROR, "[%s %d] invalid dv_capacity = %d !!\n", __func__, __LINE__, hdr_input->dv_capacity);
        }
    }

    hdr_output_info->en_src_frame_type = src_frame_type;

    return;
}

STATIC hi_void hdr_check_type_avs2(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    hi_drv_hdr_type src_frame_type = HI_DRV_HDR_TYPE_SDR;
    hi_u32 transfer_characteristics = hdr_input->colour_info.transfer_characteristics;

    hdr_input->is_backwards_compatible = 0;

    switch (transfer_characteristics) {
        case 12: /* 12 :a number */
            if (hdr_input->s1_hdr1_metadata_available) {
                src_frame_type = HI_DRV_HDR_TYPE_JTP_SL_HDR;
            } else {
                src_frame_type = HI_DRV_HDR_TYPE_HDR10;
            }
            break;

        case 14: /* 14 :a number */
            src_frame_type = HI_DRV_HDR_TYPE_HLG;
            break;

        default:
            src_frame_type = HI_DRV_HDR_TYPE_SDR;
            break;
    }

    hdr_output_info->en_src_frame_type = src_frame_type;

    hdr_check_gamma_type_avs2(hdr_input, hdr_output_info);

    return;
}

STATIC hi_void hdr_copy_hdr10_metadata(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    if (hdr_input->mastering_available == 1) {
        hdr_output_info->un_hdr_info.hdr10_info.mastering_available = hdr_input->mastering_available;
        VFMW_CHECK_SEC_FUNC(memcpy_s(&(hdr_output_info->un_hdr_info.hdr10_info.mastering_info),
            sizeof(hdr_output_info->un_hdr_info.hdr10_info.mastering_info),
            &(hdr_input->mastering_display_colour_volume), sizeof(mastering_display_colour_volums)));
    }

    if (hdr_input->content_available == 1) {
        hdr_output_info->un_hdr_info.hdr10_info.content_available = hdr_input->content_available;
        VFMW_CHECK_SEC_FUNC(memcpy_s(&(hdr_output_info->un_hdr_info.hdr10_info.content_info),
            sizeof(hdr_output_info->un_hdr_info.hdr10_info.content_info),
            &(hdr_input->content_light_level), sizeof(content_light_level_info)));
    }

    return;
}

STATIC hi_void hdr_copy_hlg_metadata(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    if (hdr_input->mastering_available == 1) {
        hdr_output_info->un_hdr_info.hlg_info.mastering_available = hdr_input->mastering_available;
        VFMW_CHECK_SEC_FUNC(memcpy_s(&(hdr_output_info->un_hdr_info.hlg_info.mastering_info),
            sizeof(hdr_output_info->un_hdr_info.hlg_info.mastering_info),
            &(hdr_input->mastering_display_colour_volume), sizeof(mastering_display_colour_volums)));
    }

    if (hdr_input->content_available == 1) {
        hdr_output_info->un_hdr_info.hlg_info.content_available = hdr_input->content_available;
        VFMW_CHECK_SEC_FUNC(memcpy_s(&(hdr_output_info->un_hdr_info.hlg_info.content_info),
                                     sizeof(hdr_output_info->un_hdr_info.hlg_info.content_info),
                                     &(hdr_input->content_light_level), sizeof(content_light_level_info)));
    }

    return;
}

#ifdef HI_HDR10PLUS_SUPPORT
STATIC hi_void hdr_copy_hdr10_plus_static_metadata(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    if (hdr_input->mastering_available == 1) {
        hdr_output_info->un_hdr_info.hdr10_plus_info.static_metadata.mastering_available =
            hdr_input->mastering_available;
        VFMW_CHECK_SEC_FUNC(memcpy_s(&(hdr_output_info->un_hdr_info.hdr10_plus_info.static_metadata.mastering_info),
            sizeof(hdr_output_info->un_hdr_info.hdr10_plus_info.static_metadata.mastering_info),
            &(hdr_input->mastering_display_colour_volume), sizeof(mastering_display_colour_volums)));
    }

    if (hdr_input->content_available == 1) {
        hdr_output_info->un_hdr_info.hdr10_plus_info.static_metadata.content_available = hdr_input->content_available;
        VFMW_CHECK_SEC_FUNC(memcpy_s(&(hdr_output_info->un_hdr_info.hdr10_plus_info.static_metadata.content_info),
                                     sizeof(hdr_output_info->un_hdr_info.hdr10_plus_info.static_metadata.content_info),
                                     &(hdr_input->content_light_level), sizeof(content_light_level_info)));
    }
}
#endif

STATIC hi_void hdr_get_metadata_hevc(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    if (hdr_output_info->en_src_frame_type == HI_DRV_HDR_TYPE_HDR10) {
        hdr_copy_hdr10_metadata(hdr_input, hdr_output_info);
    } else if (hdr_output_info->en_src_frame_type == HI_DRV_HDR_TYPE_HLG) {
        hdr_output_info->un_hdr_info.hlg_info.backwards_compatible = hdr_input->is_backwards_compatible;
        hdr_copy_hlg_metadata(hdr_input, hdr_output_info);
    } else if (hdr_output_info->en_src_frame_type == HI_DRV_HDR_TYPE_JTP_SL_HDR) {
        if (hdr_input->s1_hdr1_metadata_available == 1) {
#if 0
            hdr_output_info->un_hdr_info.stTechnicolorInfo.bMetadataValid = 1;
            hdr_output_info->en_src_frame_type = HI_DRV_VIDEO_FRAME_TYPE_SDR;
            hdr_output_info->un_hdr_info.stTechnicolorInfo.u32Length = hdr_input->hdr_metadata_size;
            hdr_output_info->un_hdr_info.stTechnicolorInfo.u32PhyAddr = hdr_input->hdr_metadata_phy_addr;
#endif
        }
    }
#ifdef HI_HDR10PLUS_SUPPORT
    else if (hdr_output_info->en_src_frame_type == HI_DRV_HDR_TYPE_HDR10PLUS) {
        hdr_copy_hdr10_plus_static_metadata(hdr_input, hdr_output_info);

        if ((hdr_input->mastering_available == 1) || (hdr_input->content_available == 1)) {
            hdr_output_info->un_hdr_info.hdr10_plus_info.static_metadata_available = HI_TRUE;
        }

        /* deal with address info in vpp. */
        if (hdr_input->dynamic_metadata_valid == 1) {
            hdr_output_info->un_hdr_info.hdr10_plus_info.dynamic_metadata_available = HI_TRUE;
            hdr_output_info->un_hdr_info.hdr10_plus_info.dynamic_metadata.data_length = hdr_input->hdr_metadata_size;
        }
    }
#endif
    else if (hdr_output_info->en_src_frame_type == HI_DRV_HDR_TYPE_DOLBYVISION) {
        hdr_output_info->un_hdr_info.dolby_info.metadata.data_length = hdr_input->hdr_metadata_size;
        /* deal with address info in vpp. */
        hdr_output_info->un_hdr_info.dolby_info.compatible = hdr_input->dv_compatible;

        if (hdr_input->hdr_metadata_size > 0) {
            hdr_output_info->un_hdr_info.dolby_info.metadata_valid = HI_TRUE;
        } else {
            hdr_output_info->un_hdr_info.dolby_info.metadata_valid = HI_FALSE;
            if (hdr_input->mastering_available == 1) {
                hdr_output_info->un_hdr_info.hdr10_info.mastering_available = hdr_input->mastering_available;
                VFMW_CHECK_SEC_FUNC(memcpy_s(&(hdr_output_info->un_hdr_info.hdr10_info.mastering_info),
                    sizeof(hdr_output_info->un_hdr_info.hdr10_info.mastering_info),
                    &(hdr_input->mastering_display_colour_volume), sizeof(mastering_display_colour_volums)));
            }

            if (hdr_input->content_available == 1) {
                hdr_output_info->un_hdr_info.hdr10_info.content_available = hdr_input->content_available;
                VFMW_CHECK_SEC_FUNC(memcpy_s(&(hdr_output_info->un_hdr_info.hdr10_info.content_info),
                                             sizeof(hdr_output_info->un_hdr_info.hdr10_info.content_info),
                                             &(hdr_input->content_light_level), sizeof(content_light_level_info)));
            }
        }
    }
}

static hi_void hdr_get_metadata_h264(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    /* deal with address info in vpp. */
    if (hdr_output_info->en_src_frame_type == HI_DRV_HDR_TYPE_DOLBYVISION) {
        hdr_output_info->un_hdr_info.dolby_info.metadata.data_length = hdr_input->hdr_metadata_size;

        if (hdr_input->hdr_metadata_size > 0) {
            hdr_output_info->un_hdr_info.dolby_info.metadata_valid = HI_TRUE;
        } else {
            hdr_output_info->un_hdr_info.dolby_info.metadata_valid = HI_FALSE;
        }

        hdr_output_info->un_hdr_info.dolby_info.compatible = hdr_input->dv_compatible;
    }

    return;
}

STATIC hi_void hdr_get_metadata_avs2(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    if (hdr_output_info->en_src_frame_type == HI_DRV_HDR_TYPE_HDR10) {
        if (hdr_input->mastering_available == 1) {
            hdr_output_info->un_hdr_info.hdr10_info.mastering_available = hdr_input->mastering_available;
            VFMW_CHECK_SEC_FUNC(memcpy_s(&(hdr_output_info->un_hdr_info.hdr10_info.mastering_info),
                sizeof(hdr_output_info->un_hdr_info.hdr10_info.mastering_info),
                &(hdr_input->mastering_display_colour_volume), sizeof(mastering_display_colour_volums)));
        }

        if (hdr_input->content_available == 1) {
            hdr_output_info->un_hdr_info.hdr10_info.content_available = hdr_input->content_available;
            VFMW_CHECK_SEC_FUNC(memcpy_s(&(hdr_output_info->un_hdr_info.hdr10_info.content_info),
                                         sizeof(hdr_output_info->un_hdr_info.hdr10_info.content_info),
                                         &(hdr_input->content_light_level), sizeof(content_light_level_info)));
        }
    } else if (hdr_output_info->en_src_frame_type == HI_DRV_HDR_TYPE_JTP_SL_HDR) {
        hdr_output_info->un_hdr_info.sl_hdr_info.static_metadata_available = hdr_input->mastering_available;
        hdr_output_info->un_hdr_info.sl_hdr_info.dynamic_metadata_available = hdr_input->s1_hdr1_metadata_available;
        if (hdr_input->mastering_available == 1) {
            hdr_output_info->un_hdr_info.sl_hdr_info.static_metadata.mastering_available =
                hdr_input->mastering_available;
            VFMW_CHECK_SEC_FUNC(memcpy_s(&(hdr_output_info->un_hdr_info.sl_hdr_info.static_metadata.mastering_info),
                sizeof(hdr_output_info->un_hdr_info.sl_hdr_info.static_metadata.mastering_info),
                &(hdr_input->mastering_display_colour_volume), sizeof(mastering_display_colour_volums)));
        }

        if (hdr_input->content_available == 1) {
            hdr_output_info->un_hdr_info.sl_hdr_info.static_metadata.content_available = hdr_input->content_available;
            VFMW_CHECK_SEC_FUNC(memcpy_s(&(hdr_output_info->un_hdr_info.sl_hdr_info.static_metadata.content_info),
                                         sizeof(hdr_output_info->un_hdr_info.sl_hdr_info.static_metadata.content_info),
                                         &(hdr_input->content_light_level), sizeof(content_light_level_info)));
        }

        if (hdr_input->s1_hdr1_metadata_available == 1) {
            hdr_output_info->un_hdr_info.sl_hdr_info.dynamic_metadata.data_length = hdr_input->hdr_metadata_size;
            /* deal with address info in vpp. */
        }
    }
}

STATIC hi_void hdr_judge_dv_mix_stream_type(vfmw_hdr_info *hdr_input, hdr_output *hdr_output_info)
{
    if (hdr_input->hdr_metadata_size == 0 &&
        hdr_input->dv_capacity == DV_BL_DECODING_SUPPORT &&
        ((hdr_input->ves_imp == DV_SINGLE_VES_IMP && hdr_input->layer_imp == DV_SINGLE_LAYER_IMP) ||
         (hdr_input->ves_imp == DV_SINGLE_VES_IMP && hdr_input->layer_imp == DV_DUAL_LAYER_IMP))) {
        if (hdr_input->colour_info.transfer_characteristics == 16) { /* 16 :a number */
            hdr_output_info->en_src_frame_type = HI_DRV_HDR_TYPE_HDR10;
        } else {
            hdr_output_info->en_src_frame_type = HI_DRV_HDR_TYPE_SDR;
        }
    }

    return;
}

STATIC hi_void hdr_fill_output_hdr_info(vfmw_vid_std vid_std, vfmw_hdr_info *hdr_input,
                                        hdr_output *hdr_output_info)
{
    switch (vid_std) {
        case VFMW_H264:
            hdr_check_type_h264(hdr_input, hdr_output_info);
            hdr_get_color_space_hevc(hdr_input, hdr_output_info);
            if (hdr_output_info->en_src_frame_type != HI_DRV_HDR_TYPE_SDR) {
                hdr_get_metadata_h264(hdr_input, hdr_output_info);
            }
            break;

        case VFMW_HEVC:
            hdr_check_type_hevc(hdr_input, hdr_output_info);
            hdr_get_color_space_hevc(hdr_input, hdr_output_info);
            hdr_get_color_prime(hdr_input, hdr_output_info);
            if (hdr_output_info->en_src_frame_type != HI_DRV_HDR_TYPE_SDR) {
                hdr_get_metadata_hevc(hdr_input, hdr_output_info);
            }
            break;

        case VFMW_AVS2:
        case VFMW_AVS3:
            hdr_check_type_avs2(hdr_input, hdr_output_info);
            hdr_get_color_space_avs2(hdr_input, hdr_output_info);
            hdr_get_color_prime(hdr_input, hdr_output_info);
            if (hdr_output_info->en_src_frame_type != HI_DRV_HDR_TYPE_SDR) {
                hdr_get_metadata_avs2(hdr_input, hdr_output_info);
            }
            break;
        case VFMW_MPEG2:
            break;
        default:
            /* do nothing */
            break;
    }

    hdr_judge_dv_mix_stream_type(hdr_input, hdr_output_info);
    return;
}

#ifdef VFMW_PROC_SUPPORT
hi_u8 g_HDREnable = 1;

STATIC hi_void hdr_record_proc_info(hi_s32 chan_id, vfmw_image *image, hdr_output *hdr_output_info)
{
    hi_u8 pos;
    vfmw_hdr_info *hdr_input = &image->hdr_input;

    pos = (hdr_output_info->en_src_frame_type == HI_DRV_HDR_TYPE_DOLBYVISION) ? 1 : 0;
    VFMW_CHECK_SEC_FUNC(memset_s(&g_hdr_chan[pos], sizeof(hdr_proc), 0, sizeof(hdr_proc)));
    g_hdr_chan[pos].chan_id = chan_id;
    g_hdr_chan[pos].vid_std = image->vid_std;

    VFMW_CHECK_SEC_FUNC(memcpy_s(&g_hdr_chan[pos].input, sizeof(vfmw_hdr_info), hdr_input, sizeof(vfmw_hdr_info)));
    VFMW_CHECK_SEC_FUNC(memcpy_s(&g_hdr_chan[pos].output, sizeof(hdr_output), hdr_output_info, sizeof(hdr_output)));

    return;
}

STATIC hi_void hdr_show_standard(vfmw_vid_std vid_std, hi_s8 *str_vid_std, hi_u32 array_size)
{
    hi_s8 *str_vid_std_temp = HI_NULL;

    switch (vid_std) {
        case VFMW_H264:
            str_vid_std_temp = "H264";
            break;

        case VFMW_AVS2:
            str_vid_std_temp = "AVS2";
            break;

        case VFMW_AVS3:
            str_vid_std_temp = "AVS3";
            break;

        case VFMW_VP9:
            str_vid_std_temp = "VP9";
            break;

        case VFMW_HEVC:
            str_vid_std_temp = "HEVC";
            break;

        case VFMW_STD_MAX:
            str_vid_std_temp = "RESERVED";
            break;

        default:
            *str_vid_std = '\0';
            break;
    }

    if (str_vid_std_temp) {
        OS_STRLCPY(str_vid_std, str_vid_std_temp, OS_STRLEN(str_vid_std_temp) + 1);
    }

    return;
}

STATIC hi_void hdr_read_hdr10_metadata_info(hi_void *p, hi_drv_hdr_static_metadata *hdr10_info)
{
    if (hdr10_info->mastering_available == 1) {
        OS_PROC_PRINT(p, "%-35s : %d\n", "bMasteringAvailable", hdr10_info->mastering_available);
        OS_PROC_PRINT(p, "%-35s : (%d, %d, %d)\n", "display_primaries_x",
                      hdr10_info->mastering_info.display_primaries_x[0],
                      hdr10_info->mastering_info.display_primaries_x[1],
                      hdr10_info->mastering_info.display_primaries_x[2]); /* 2 :a number */

        OS_PROC_PRINT(p, "%-35s : (%d, %d, %d)\n", "display_primaries_y",
                      hdr10_info->mastering_info.display_primaries_y[0],
                      hdr10_info->mastering_info.display_primaries_y[1],
                      hdr10_info->mastering_info.display_primaries_y[2]); /* 2 :a number */

        OS_PROC_PRINT(p, "%-35s : (%d, %d)\n", "WhitePoint",
                      hdr10_info->mastering_info.white_point_x,
                      hdr10_info->mastering_info.white_point_y);

        OS_PROC_PRINT(p, "%-35s : %d\n", "max_disp_mastering_luminance",
                      hdr10_info->mastering_info.max_display_mastering_luminance);
        OS_PROC_PRINT(p, "%-35s : %d\n", "min_disp_mastering_luminance",
                      hdr10_info->mastering_info.min_display_mastering_luminance);
    }

    if (hdr10_info->content_available == 1) {
        OS_PROC_PRINT(p, "%-35s : %d\n", "max_content_light_level",
                      hdr10_info->content_info.max_content_light_level);
        OS_PROC_PRINT(p, "%-35s : %d\n", "max_pic_average_light_level",
                      hdr10_info->content_info.max_pic_average_light_level);
    }

    return;
}

STATIC hi_void hdr_read_hlg_metadata_info(hi_void *p, hi_drv_hdr_hlg_metadata *hlg_info)
{
    OS_PROC_PRINT(p, "%-35s : %d\n", "BackWardsCompatible", hlg_info->backwards_compatible);

    if (hlg_info->mastering_available == 1) {
        OS_PROC_PRINT(p, "%-35s : %d\n", "bMasteringAvailable", hlg_info->mastering_available);
        OS_PROC_PRINT(p, "%-35s : (%d, %d, %d)\n", "display_primaries_x",
                      hlg_info->mastering_info.display_primaries_x[0],
                      hlg_info->mastering_info.display_primaries_x[1],
                      hlg_info->mastering_info.display_primaries_x[2]); /* 2 :a number */

        OS_PROC_PRINT(p, "%-35s : (%d, %d, %d)\n", "display_primaries_y",
                      hlg_info->mastering_info.display_primaries_y[0],
                      hlg_info->mastering_info.display_primaries_y[1],
                      hlg_info->mastering_info.display_primaries_y[2]); /* 2 :a number */

        OS_PROC_PRINT(p, "%-35s : (%d, %d)\n", "WhitePoint",
                      hlg_info->mastering_info.white_point_x,
                      hlg_info->mastering_info.white_point_y);

        OS_PROC_PRINT(p, "%-35s : %d\n", "max_disp_mastering_luminance",
                      hlg_info->mastering_info.max_display_mastering_luminance);
        OS_PROC_PRINT(p, "%-35s : %d\n", "min_disp_mastering_luminance",
                      hlg_info->mastering_info.min_display_mastering_luminance);
    }

    if (hlg_info->content_available == 1) {
        OS_PROC_PRINT(p, "%-35s : %d\n", "max_content_light_level",
                      hlg_info->content_info.max_content_light_level);
        OS_PROC_PRINT(p, "%-35s : %d\n", "max_pic_average_light_level",
                      hlg_info->content_info.max_pic_average_light_level);
    }

    return;
}

#ifdef HI_HDR10PLUS_SUPPORT
#ifndef ENV_SOS_KERNEL
STATIC hi_void hdr_read_hdr10_plus_dynamic_metadata_info(hi_void *p,
                                                         hi_drv_video_hdr10_plus_dynamic_metadata *dynamic_metadata)
{
    hi_u8 i = 0;
    hi_u8 j = 0;
    hi_u8 w = 0;

    OS_PROC_PRINT(p, "%-35s\n", "---DynamicMetadata Start---");
    OS_PROC_PRINT(p, "%-35s : %d\n", "Windows Num", dynamic_metadata->num_windows);

    for (w = 1; w < dynamic_metadata->num_windows; w++) {
        OS_PROC_PRINT(p, "%-35s : %d\n", "WindowUpperLeftCornerX", dynamic_metadata->window_upper_left_corner_x[w]);
        OS_PROC_PRINT(p, "%-35s : %d\n", "WindowUpperLeftCornerY", dynamic_metadata->window_upper_left_corner_y[w]);
        OS_PROC_PRINT(p, "%-35s : %d\n", "WindowLowerRightCornerX", dynamic_metadata->window_lower_right_corner_x[w]);
        OS_PROC_PRINT(p, "%-35s : %d\n", "WindowLowerRightCornerY", dynamic_metadata->window_lower_right_corner_y[w]);
        OS_PROC_PRINT(p, "%-35s : %d\n", "CenterOfEllipseX", dynamic_metadata->center_of_ellipse_x[w]);
        OS_PROC_PRINT(p, "%-35s : %d\n", "CenterOfEllipseY", dynamic_metadata->center_of_ellipse_y[w]);
        OS_PROC_PRINT(p, "%-35s : %d\n", "RotationAngle", dynamic_metadata->rotation_angle[w]);
        OS_PROC_PRINT(p, "%-35s : %d\n", "SemimajorAxisInternalEllipse",
                      dynamic_metadata->semimajor_axis_internal_ellipse[w]);
        OS_PROC_PRINT(p, "%-35s : %d\n", "SemimajorAxisExternalEllipse",
                      dynamic_metadata->semimajor_axis_external_ellipse[w]);
        OS_PROC_PRINT(p, "%-35s : %d\n", "SemiminorAxisExternalEllipse",
                      dynamic_metadata->semiminor_axis_external_ellipse[w]);
        OS_PROC_PRINT(p, "%-35s : %d\n", "OverlapProcessOption", dynamic_metadata->overlap_process_option[w]);
    }

    OS_PROC_PRINT(p, "%-35s : %d\n", "T_DisplayMaximumLuminance", dynamic_metadata->target_display_maximum_luminance);
    OS_PROC_PRINT(p, "%-35s : %d\n", "T_Display_APL_Flag", dynamic_metadata->target_display_apl_flag);

    if (dynamic_metadata->target_display_apl_flag) {
        OS_PROC_PRINT(p, "%-35s : %d\n", "NumRows_T_Display_APL", dynamic_metadata->num_rows_target_display_apl);
        OS_PROC_PRINT(p, "%-35s : %d\n", "NumCols_T_Display_APL", dynamic_metadata->num_cols_target_display_apl);

        for (i = 0; i < dynamic_metadata->num_rows_target_display_apl; i++) {
            for (j = 0; j < dynamic_metadata->num_cols_target_display_apl; j++) {
                OS_PROC_PRINT(p, "%-35s : %d\n", "T_display_APL", dynamic_metadata->target_display_apl[i][j]);
            }
        }
    }

    for (w = 0; w < dynamic_metadata->num_windows; w++) {
        for (i = 0; i < 3; i++) { /* 3 :a number */
            OS_PROC_PRINT(p, "%-35s : %d\n", "MaxScl", dynamic_metadata->max_scl[w][i]);
        }

        OS_PROC_PRINT(p, "%-35s : %d\n", "AverageMaxrgb", dynamic_metadata->average_maxrgb[w]);
        OS_PROC_PRINT(p, "%-35s : %d\n", "NumDistMaxrgbPercentiles", dynamic_metadata->num_dist_maxrgb_percentiles[w]);

        for (i = 0; i < dynamic_metadata->num_dist_maxrgb_percentiles[w]; i++) {
            OS_PROC_PRINT(p, "%-35s[%d] : %d\n", "DistMaxrgbPercentages", i,
                          dynamic_metadata->dist_maxrgb_percentages[w][i]);
        }

        for (i = 0; i < dynamic_metadata->num_dist_maxrgb_percentiles[w]; i++) {
            OS_PROC_PRINT(p, "%-35s[%d] : %d\n", "DistMaxrgbPercentiles", i,
                          dynamic_metadata->dist_maxrgb_percentiles[w][i]);
        }

        OS_PROC_PRINT(p, "%-35s : %d\n", "FractionBrightPixels", dynamic_metadata->fraction_bright_pixels[w]);
    }

    OS_PROC_PRINT(p, "%-35s : %d\n", "M_Display_APL_Flag", dynamic_metadata->master_display_apl_flag);

    if (dynamic_metadata->b_M_Display_APL_Flag) {
        OS_PROC_PRINT(p, "%-35s : %d\n", "NumRows_M_Display_APL", dynamic_metadata->num_rows_master_display_apl);
        OS_PROC_PRINT(p, "%-35s : %d\n", "NumCols_M_Display_APL", dynamic_metadata->num_cols_master_display_apl);

        for (i = 0; i < dynamic_metadata->num_rows_master_display_apl; i++) {
            for (j = 0; j < dynamic_metadata->num_cols_master_display_apl; j++) {
                OS_PROC_PRINT(p, "%-35s : %d\n", "M_Display_APL", dynamic_metadata->master_display_apl[i][j]);
            }
        }
    }

    for (w = 0; w < dynamic_metadata->num_windows; w++) {
        OS_PROC_PRINT(p, "%-35s : %d\n", "ToneMappingFlag", dynamic_metadata->tone_mapping_flag[w]);
        if (dynamic_metadata->bToneMappingFlag[w]) {
            OS_PROC_PRINT(p, "%-35s : %d\n", "KneePointX", dynamic_metadata->knee_point_x[w]);
            OS_PROC_PRINT(p, "%-35s : %d\n", "KneePointY", dynamic_metadata->knee_point_y[w]);
            OS_PROC_PRINT(p, "%-35s : %d\n", "NumBezierCurveAnchors", dynamic_metadata->num_bezier_curve_anchors[w]);

            for (i = 0; i < dynamic_metadata->num_bezier_curve_anchors[w]; i++) {
                OS_PROC_PRINT(p, "%-35s : %d\n", "bezierCurveAnchors", dynamic_metadata->bezier_curve_anchors[w][i]);
            }
        }

        OS_PROC_PRINT(p, "%-35s : %d\n", "ColorSaturationMappingFlag",
            dynamic_metadata->color_saturation_mapping_flag[w]);

        if (dynamic_metadata->color_saturation_mapping_flag[w]) {
            OS_PROC_PRINT(p, "%-35s : %d\n", "ColorSaturationWeight", dynamic_metadata->color_saturation_weight[w]);
        }
    }

    OS_PROC_PRINT(p, "%-35s\n", "---DynamicMetadata End---");

    return;
}
#endif
#endif

STATIC hi_void hdr_read_slh_drm_metadata_info(hi_void *p, hi_drv_hdr_sl_metadata *sl_hdr_info)
{
    mem_record mem_record = { 0 };
    hi_drv_sl_hdr_metadata *dynamic_metadata = HI_NULL;

    OS_PROC_PRINT(p, "%-35s : %d\n", "staticMetadataAvailable", sl_hdr_info->static_metadata_available);
    OS_PROC_PRINT(p, "%-35s : %d\n", "dynamicMetadataAvailable", sl_hdr_info->dynamic_metadata_available);

    if (sl_hdr_info->static_metadata_available == 1) {
        hdr_read_hdr10_metadata_info(p, &(sl_hdr_info->static_metadata));
    }

    if (sl_hdr_info->dynamic_metadata_available == 1) {
        mem_record.phy_addr = sl_hdr_info->dynamic_metadata.mem_addr.mem_handle;
        mem_record.length = sl_hdr_info->dynamic_metadata.data_length;
        dynamic_metadata = (hi_drv_sl_hdr_metadata *)OS_MMAP(&mem_record);
        if (dynamic_metadata == HI_NULL) {
            dprint(PRN_ERROR, "[%s %d] dynamic_metadata is null\n", __func__, __LINE__);
            return;
        }

        OS_PROC_PRINT(p, "%-35s : %d\n", "PartID", dynamic_metadata->part_id);
        OS_PROC_PRINT(p, "%-35s : %d\n", "MajorSpecVersionID", dynamic_metadata->major_spec_version_id);
        OS_PROC_PRINT(p, "%-35s : %d\n", "MinorSpecVersionID", dynamic_metadata->minor_spec_version_id);
        OS_PROC_PRINT(p, "%-35s : %d\n", "payload_mode", dynamic_metadata->payload_mode);

        OS_PROC_PRINT(p, "%-35s : %d\n", "hdr_pic_colour_space", dynamic_metadata->hdr_pic_colour_space);
        OS_PROC_PRINT(p, "%-35s : %d\n", "HdrDisplayColourSpace", dynamic_metadata->hdr_display_colour_space);
        OS_PROC_PRINT(p, "%-35s : %d\n", "HdrDisplayMaxLuminance", dynamic_metadata->hdr_display_max_luminance);
        OS_PROC_PRINT(p, "%-35s : %d\n", "HdrDisplayMinLuminance", dynamic_metadata->hdr_display_min_luminance);

        OS_PROC_PRINT(p, "%-35s : %d\n", "sdr_pic_colour_space", dynamic_metadata->sdr_pic_colour_space);
        OS_PROC_PRINT(p, "%-35s : %d\n", "SdrDisplayColourSpace", dynamic_metadata->sdr_display_colour_space);
        OS_PROC_PRINT(p, "%-35s : %d\n", "SdrDisplayMaxLuminance", dynamic_metadata->sdr_display_max_luminance);
        OS_PROC_PRINT(p, "%-35s : %d\n", "SdrDisplayMinLuminance", dynamic_metadata->sdr_display_min_luminance);

        OS_PROC_PRINT(p, "%-35s : (%d, %d, %d)\n", "MatrixCoefficient",
                      dynamic_metadata->matrix_coefficient[0],
                      dynamic_metadata->matrix_coefficient[1],
                      dynamic_metadata->matrix_coefficient[2]); /* 2 :a number */

        OS_PROC_PRINT(p, "%-35s : (%d, %d)\n", "ChromaToLumaInjection",
                      dynamic_metadata->chroma_to_luma_injection[0],
                      dynamic_metadata->chroma_to_luma_injection[1]);

        OS_PROC_PRINT(p, "%-35s : (%d, %d, %d)\n", "MatrixCoefficient",
                      dynamic_metadata->k_coefficient[0],
                      dynamic_metadata->k_coefficient[1],
                      dynamic_metadata->k_coefficient[2]); /* 2 :a number */

        OS_PROC_PRINT(p, "%-35s : %d\n", "tm_input_signal_black_level_offset",
                      dynamic_metadata->u.variables.tm_input_signal_black_level_offset);

        OS_PROC_PRINT(p, "%-35s : %d\n", "tm_input_signal_white_level_offset",
                      dynamic_metadata->u.variables.tm_input_signal_white_level_offset);
        OS_PROC_PRINT(p, "%-35s : %d\n", "shadow_gain", dynamic_metadata->u.variables.shadow_gain);
        OS_PROC_PRINT(p, "%-35s : %d\n", "highlight_gain", dynamic_metadata->u.variables.highlight_gain);
        OS_PROC_PRINT(p, "%-35s : %d\n", "mid_tone_width_adj_factor",
                      dynamic_metadata->u.variables.mid_tone_width_adj_factor);
        OS_PROC_PRINT(p, "%-35s : %d\n", "tm_output_fine_tuning_num_val",
                      dynamic_metadata->u.variables.tm_output_fine_tuning_num_val);
        OS_PROC_PRINT(p, "%-35s : %d\n", "saturation_gain_num_val",
            dynamic_metadata->u.variables.saturation_gain_num_val);

        OS_UNMAP(&mem_record);
    }

    return;
}

static hi_void hdr_read_video_frame_type(struct seq_file *p, hi_drv_hdr_type src_frame_type)
{
    switch (src_frame_type) {
        case HI_DRV_HDR_TYPE_SDR:
            OS_PROC_PRINT(p, "%-35s : SDR\n", "SrcFrmType");
            break;
#if 0
        case HI_DRV_HDR_TYPE_DOLBYVISION:
            OS_PROC_PRINT(p, "%-35s : DOLBY_BL\n", "SrcFrmType");
            break;
#endif
        case HI_DRV_HDR_TYPE_DOLBYVISION:
            OS_PROC_PRINT(p, "%-35s : DOLBY_EL\n", "SrcFrmType");
            break;

        case HI_DRV_HDR_TYPE_HDR10:
            OS_PROC_PRINT(p, "%-35s : HDR10\n", "SrcFrmType");
            break;
#ifdef HI_HDR10PLUS_SUPPORT
        case HI_DRV_HDR_TYPE_HDR10PLUS:
            OS_PROC_PRINT(p, "%-35s : HDR10+\n", "SrcFrmType");
            break;
#endif
        case HI_DRV_HDR_TYPE_HLG:
            OS_PROC_PRINT(p, "%-35s : HLG\n", "SrcFrmType");
            break;

        case HI_DRV_HDR_TYPE_JTP_SL_HDR:
            OS_PROC_PRINT(p, "%-35s : JTP_SL\n", "SrcFrmType");
            break;

        default:
            OS_PROC_PRINT(p, "%-35s : UNKNOWN\n", "SrcFrmType");

            break;
    }

    return;
}

STATIC hi_void hdr_read_input_hdr_info(hi_void *p, vfmw_vid_std vid_std, vfmw_hdr_info *hdr_input)
{
    hi_s8 str_vid_std[MAX_VID_STD_NAME];

    hdr_show_standard(vid_std, str_vid_std, MAX_VID_STD_NAME);

    OS_PROC_PRINT(p, "%-35s : %s\n", "Cfg standard", str_vid_std);
    OS_PROC_PRINT(p, "%-35s : 0x%llx\n", "hdr_metadata_phy_addr", (hi_u64)hdr_input->hdr_metadata_phy_addr);
    OS_PROC_PRINT(p, "%-35s : 0x%llx\n", "hdr_metadata_vir_addr", hdr_input->hdr_metadata_vir_addr);
    OS_PROC_PRINT(p, "%-35s : 0x%x\n", "hdr_metadata_size", hdr_input->hdr_metadata_size);

    OS_PROC_PRINT(p, "%-35s : %d\n", "full_rangeFlag", hdr_input->colour_info.full_rangeFlag);
    OS_PROC_PRINT(p, "%-35s : %d\n", "colour_primaries", hdr_input->colour_info.colour_primaries);
    OS_PROC_PRINT(p, "%-35s : %d\n", "matrix_coeffs", hdr_input->colour_info.matrix_coeffs);
    OS_PROC_PRINT(p, "%-35s : %d\n", "transfer_characteristics", hdr_input->colour_info.transfer_characteristics);

    OS_PROC_PRINT(p, "%-35s : %d\n", "mastering_available", hdr_input->mastering_available);

    if (hdr_input->mastering_available == 1) {
        OS_PROC_PRINT(p, "%-35s : (%d, %d, %d)\n", "display_primaries_x",
                      hdr_input->mastering_display_colour_volume.display_primaries_x[0],
                      hdr_input->mastering_display_colour_volume.display_primaries_x[1],
                      hdr_input->mastering_display_colour_volume.display_primaries_x[2]); /* 2 :a number */

        OS_PROC_PRINT(p, "%-35s : (%d, %d, %d)\n", "display_primaries_y",
                      hdr_input->mastering_display_colour_volume.display_primaries_y[0],
                      hdr_input->mastering_display_colour_volume.display_primaries_y[1],
                      hdr_input->mastering_display_colour_volume.display_primaries_y[2]); /* 2 :a number */

        OS_PROC_PRINT(p, "%-35s : (%d, %d)\n", "WhitePoint",
                      hdr_input->mastering_display_colour_volume.white_point_x,
                      hdr_input->mastering_display_colour_volume.white_point_x);

        OS_PROC_PRINT(p, "%-35s : %d\n", "max_disp_mastering_luminance",
                      hdr_input->mastering_display_colour_volume.max_disp_mastering_luminance);
        OS_PROC_PRINT(p, "%-35s : %d\n", "min_disp_mastering_luminance",
                      hdr_input->mastering_display_colour_volume.min_disp_mastering_luminance);
    }

    OS_PROC_PRINT(p, "%-35s : %d\n", "content_available", hdr_input->content_available);

    if (hdr_input->content_available == 1) {
        OS_PROC_PRINT(p, "%-35s : %d\n", "max_content_light_level",
                      hdr_input->content_light_level.max_content_light_level);
        OS_PROC_PRINT(p, "%-35s : %d\n", "max_pic_average_light_level",
                      hdr_input->content_light_level.max_pic_average_light_level);
    }

    OS_PROC_PRINT(p, "%-35s : %d\n", "hdr_transfer_characteristic_idc",
                  hdr_input->comapitibility_info.hdr_transfer_characteristic_idc);
    OS_PROC_PRINT(p, "%-35s : %d\n", "s1_hdr1_metadata_available", hdr_input->s1_hdr1_metadata_available);

#ifdef DOLBY_VISION_HDR_SUPPORT
    OS_PROC_PRINT(p, "%-35s : %d\n", "be_dv_hdr_chan", hdr_input->be_dv_hdr_chan);
    OS_PROC_PRINT(p, "%-35s : %d\n", "dv_capacity", hdr_input->dv_capacity);
    OS_PROC_PRINT(p, "%-35s : %d\n", "ves_imp", hdr_input->ves_imp);
    OS_PROC_PRINT(p, "%-35s : %d\n", "layer_imp", hdr_input->layer_imp);
    OS_PROC_PRINT(p, "%-35s : %d\n", "dv_compatible", hdr_input->dv_compatible);
#endif

    return;
}

STATIC hi_void hdr_read_output_hdr_info(hi_void *p, hdr_output *hdr_output_info)
{
#ifdef HI_HDR10PLUS_SUPPORT
    hi_drv_video_hdr10_plus_dynamic_metadata *pMetaData = HI_NULL;
    mem_record mem_record = { 0 };
#endif

    hi_s8 aszGammaType[32] = { 0 }; /* 32 :a number */
    hi_s8 aszColorSpace[32] = { 0 }; /* 32 :a number */
    hi_s8 aszLuminaceSys[32] = { 0 }; /* 32 :a number */
    hi_s8 aszColorPrime[32] = { 0 }; /* 32 :a number */

    hdr_get_gamma_type_info(hdr_output_info->colour_info.transfer_type, aszGammaType, sizeof(aszGammaType));
    hdr_get_color_space_info(hdr_output_info->colour_info.color_space, aszColorSpace, sizeof(aszColorSpace));
    hdr_get_luminace_sys_info(hdr_output_info->colour_info.matrix_coef, aszLuminaceSys, sizeof(aszLuminaceSys));
    hdr_get_color_prime_info(hdr_output_info->colour_info.color_primary, aszColorPrime, sizeof(aszColorPrime));

    hdr_read_video_frame_type(p, hdr_output_info->en_src_frame_type);

    OS_PROC_PRINT(p, "%-35s : %d\n", "bFullRange", hdr_output_info->colour_info.quantify_range);
    OS_PROC_PRINT(p, "%-35s : %s\n", "GammaType", aszGammaType);
    OS_PROC_PRINT(p, "%-35s : %s\n", "ColorSpace", aszColorSpace);
    OS_PROC_PRINT(p, "%-35s : %s\n", "LuminaceSys", aszLuminaceSys);
    OS_PROC_PRINT(p, "%-35s : %s\n", "ColorPrimaries", aszColorPrime);

    switch (hdr_output_info->en_src_frame_type) {
        case HI_DRV_HDR_TYPE_SDR:
            break;
#if 0
        case HI_DRV_HDR_TYPE_DOLBYVISION:
            OS_PROC_PRINT(p, "%-35s : %d\n", "b_compatible", hdr_output_info->un_hdr_info.dolby_info.compatible);
            OS_PROC_PRINT(p, "%-35s : %d\n", "bMetadataValid", hdr_output_info->un_hdr_info.dolby_info.metadata_valid);
            OS_PROC_PRINT(p, "%-35s : 0x%llx\n", "u32Addr",
                          hdr_output_info->un_hdr_info.dolby_info.metadata.mem_addr.mem_handle);
            OS_PROC_PRINT(p, "%-35s : %d\n", "u32Length", hdr_output_info->un_hdr_info.dolby_info.metadata.data_length);
            break;
#endif
        case HI_DRV_HDR_TYPE_DOLBYVISION:
            OS_PROC_PRINT(p, "%-35s : %d\n", "b_compatible", hdr_output_info->un_hdr_info.dolby_info.compatible);
            OS_PROC_PRINT(p, "%-35s : %d\n", "bMetadataValid", hdr_output_info->un_hdr_info.dolby_info.metadata_valid);
            OS_PROC_PRINT(p, "%-35s : 0x%llx\n", "u32Addr",
                          hdr_output_info->un_hdr_info.dolby_info.metadata.mem_addr.mem_handle);
            OS_PROC_PRINT(p, "%-35s : %d\n", "u32Length", hdr_output_info->un_hdr_info.dolby_info.metadata.data_length);
            break;

        case HI_DRV_HDR_TYPE_HDR10:
            hdr_read_hdr10_metadata_info(p, &(hdr_output_info->un_hdr_info.hdr10_info));
            break;
#ifdef HI_HDR10PLUS_SUPPORT
        case HI_DRV_HDR_TYPE_HDR10PLUS:
            OS_PROC_PRINT(p, "%-35s : %d\n", "bStaticMetadataValid",
                          hdr_output_info->un_hdr_info.hdr10_plus_info.static_metadata_valid);
            OS_PROC_PRINT(p, "%-35s : %d\n", "dynamic_metadata_valid",
                          hdr_output_info->un_hdr_info.hdr10_plus_info.dynamic_metadata_valid);
            hdr_read_hdr10_metadata_info(p, &(hdr_output_info->un_hdr_info.hdr_info.hdr10_plus_info.static_metadata));

            OS_PROC_PRINT(p, "%-35s : 0x%x\n", "metadata u32Addr",
                          hdr_output_info->un_hdr_info.hdr10_plus_info.dynamic_metadata.mem_addr.mem_handle);
            OS_PROC_PRINT(p, "%-35s : %d\n", "metadata u32Length",
                          hdr_output_info->un_hdr_info.hdr10_plus_info.dynamic_metadata.data_length);

#ifndef ENV_SOS_KERNEL
            mem_record.phy_addr = hdr_output_info->un_hdr_info.hdr10_plus_info.dynamic_metadata.mem_addr.mem_handle;
            mem_record.length = hdr_output_info->un_hdr_info.hdr10_plus_info.dynamic_metadata.data_length;
            pMetaData = (hi_drv_video_hdr10_plus_dynamic_metadata *)OS_MMAP(&mem_record);
            if (pMetaData == HI_NULL) {
                dprint(PRN_ERROR, "%s, line %d, mmap failed\n", __func__, __LINE__);
                return;
            }

            hdr_read_hdr10_plus_dynamic_metadata_info(p, pMetaData);
            OS_UNMAP(&mem_record);
#endif
            break;
#endif
        case HI_DRV_HDR_TYPE_HLG:
            hdr_read_hlg_metadata_info(p, &(hdr_output_info->un_hdr_info.hlg_info));
            break;

        case HI_DRV_HDR_TYPE_JTP_SL_HDR:
            hdr_read_slh_drm_metadata_info(p, &(hdr_output_info->un_hdr_info.sl_hdr_info));
            break;

        default:
            OS_PROC_PRINT(p, "%-35s : UNKNOWN\n", "SrcFrmType");

            break;
    }
}

STATIC hi_void hdr_read_info(hi_void *buf, hdr_proc *hdr_info)
{
    OS_PROC_PRINT(buf, "Input Info:\n");
    hdr_read_input_hdr_info(buf, hdr_info->vid_std, &(hdr_info->input));

    OS_PROC_PRINT(buf, "Output Info:\n");
    hdr_read_output_hdr_info(buf, &(hdr_info->output));

    return;
}

STATIC hi_s32 hdr_read_proc(hi_void *buf, hi_s32 chan_id)
{
    hi_s32 Used = 0;
    hi_s32 Len = 0;
    VFMW_ASSERT_RET(buf != HI_NULL, HDR_ERR);
    OS_DUMP_PROC(buf, Len, &Used, 0, LOG_FMT_HEAD, "HDR");

    if (g_HDREnable == 0) {
        OS_DUMP_PROC(buf, Len, &Used, 0, "  Hdr proc disable.\n\n");
        return HDR_ERR;
    }

    if (chan_id == g_hdr_chan[0].chan_id) {
        hdr_read_info(buf, &g_hdr_chan[0]);
    } else if (chan_id == g_hdr_chan[1].chan_id) {
        hdr_read_info(buf, &g_hdr_chan[1]);
    } else {
        OS_DUMP_PROC(buf, Len, &Used, 0, "  Hdr proc chan %d not exist.\n\n");
        return HDR_ERR;
    }

    return HDR_OK;
}

STATIC hi_s32 hdr_write_proc(cmd_str_ptr buf, hi_u32 count)
{
    hi_u32 data = 0;
    hi_s8 *cmd_id = ((*buf)[0]);
    hi_s8 *pValue = ((*buf)[1]);

    VFMW_ASSERT_RET_PRNT(count >= 2, HDR_ERR, "Cmd param count %d invalid.\n", count); /* 2 :a number */

    if (!OS_STRNCMP(cmd_id, LOG_CMD_READ_HDR, OS_STRLEN(cmd_id))) {
        str_to_val(pValue, &data);

        VFMW_ASSERT_RET_PRNT(data == 0 || data == 1, HDR_ERR, "Input data %d should be in range[0,1].\n", data);

        g_HDREnable = data;
        dprint(PRN_ALWS, "Set read hdr = %d.\n", data);
    } else {
        dprint(PRN_ALWS, "Unkown commad!\n");
    }

    return count;
}
#endif

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
hi_void hdr_info_init(hi_void)
{
    return;
}

hi_void hdr_info_exit(hi_void)
{
    return;
}

STATIC hi_u32 hdr_check_vid_std(vfmw_vid_std vid_std)
{
    if (vid_std == VFMW_H264 ||
        vid_std == VFMW_HEVC ||
        vid_std == VFMW_AVS2 ||
        vid_std == VFMW_MPEG2 ||
        vid_std == VFMW_AVS3) {
        return 1;
    }

    return 0;
}

hi_s32 hdr_info_convert(hi_s32 chan_id, vfmw_image *image)
{
    vfmw_hdr_info *hdr_input = HI_NULL;
    hi_drv_video_frame *frame = HI_NULL;
    hdr_output hdr_output_info;
    vfmw_vid_std vid_std;

    VFMW_ASSERT_RET((chan_id >= 0 && chan_id < VFMW_CHAN_NUM), HI_FAILURE);
    VFMW_ASSERT_RET(image != HI_NULL, HI_FAILURE);

    vid_std = image->vid_std;

    if (hdr_check_vid_std(vid_std) == 0) {
        return HI_SUCCESS;
    }

    hdr_input = &(image->hdr_input);

    frame = (hi_drv_video_frame *)(hi_ulong)(image->frame_ptr);
    VFMW_ASSERT_RET(frame != HI_NULL, HI_FAILURE);

    VFMW_CHECK_SEC_FUNC(memset_s(&hdr_output_info, sizeof(hdr_output_info), 0, sizeof(hdr_output_info)));

    hdr_fill_output_hdr_info(vid_std, hdr_input, &hdr_output_info);

    if (sizeof(frame->hdr_info) != sizeof(hdr_output_info.un_hdr_info)) {
        dprint(PRN_ERROR, "%s ERROR: size1 %d != size2 %d!\n", __func__, sizeof(frame->hdr_info),
               sizeof(hdr_output_info.un_hdr_info));

        return HI_FAILURE;
    }

    VFMW_CHECK_SEC_FUNC(memcpy_s(&(frame->hdr_info), sizeof(frame->hdr_info),
                                 &(hdr_output_info.un_hdr_info), sizeof(hdr_output_info.un_hdr_info)));

    frame->hdr_type = hdr_output_info.en_src_frame_type;

    VFMW_CHECK_SEC_FUNC(memcpy_s(&(frame->color_space), sizeof(frame->color_space),
                                 &(hdr_output_info.colour_info), sizeof(hdr_output_info.colour_info)));

#ifdef VFMW_PROC_SUPPORT
    if (g_HDREnable == 1) {
        hdr_record_proc_info(chan_id, image, &hdr_output_info);
    }
#endif

    return HI_SUCCESS;
}

hi_s32 hdr_info_read(hi_void *buf, hi_s32 chan_id)
{
#ifdef VFMW_PROC_SUPPORT
    hdr_read_proc(buf, chan_id);
#endif

    return HI_SUCCESS;
}

hi_s32 hdr_info_write(cmd_str_ptr buffer, hi_u32 count)
{
#ifdef VFMW_PROC_SUPPORT
    hdr_write_proc(buffer, count);
#endif

    return HI_SUCCESS;
}


