#include "drv_vdec_convert.h"
#include "drv_vdec_sdk.h"
#include "vdec_priv_type.h"

#define VDEC_EVENT_TABLE_ITEM_SIZE 2

typedef hi_s32 (*fn_evt_process_type)(hi_void *args, hi_s32 len, hi_vdec_event *out);

typedef struct {
    hi_s32 evt_id;
    fn_evt_process_type evt_process;
} vdec_cnvt_evt_item;

static hi_s32 vdec_cnvt_new_image_2_vdec(hi_void *args, hi_s32 len, hi_vdec_event *out)
{
    out->event_id = HI_VDEC_EVT_NEW_FRAME;

    return HI_SUCCESS;
}

static hi_s32 vdec_cnvt_stream_2_vdec(hi_void *args, hi_s32 len, hi_vdec_event *out)
{
    vfmw_stream_buf *stream = (vfmw_stream_buf*)args;

    if (stream == HI_NULL || len != sizeof(vfmw_stream_buf)) {
        VDEC_PRINT_ERR("stream param invalid args %p len %d!\n", args, len);
        return HI_FAILURE;
    }

    out->event_id = HI_VDEC_EVT_NEW_STREAM;
    out->event_param.stream.buf_phy = stream->phy_addr;
    out->event_param.stream.buf_size = stream->length;

    return HI_SUCCESS;
}

static hi_s32 vdec_cnvt_eos_2_vdec(hi_void *args, hi_s32 len, hi_vdec_event *out)
{
    if (args == HI_NULL || len != sizeof(hi_u32)) {
        VDEC_PRINT_ERR("eos param invalid args %p len %d\n", args, len);
        return HI_FAILURE;
    }

    out->event_id = HI_VDEC_EVT_EOS;
    out->event_param.eos_param.last_frame_flag = (((hi_u32*)args)[0] == 0) ? 0 : 1;

    return HI_SUCCESS;
}

static hi_s32 vdec_cnvt_err_frm_2_vdec(hi_void *args, hi_s32 len, hi_vdec_event *out)
{
    hi_s8 vfmw_ipb_type;

    out->event_id = HI_VDEC_EVT_ERR_FRAME;
    vfmw_ipb_type = ((hi_s8*)args)[0];
    if (vfmw_ipb_type == 0) {
        out->event_param.err_frm.err_frm_type = HI_VDEC_I_FRM;
    } else if (vfmw_ipb_type == 1) {
        out->event_param.err_frm.err_frm_type = HI_VDEC_P_FRM;
    } else {
        out->event_param.err_frm.err_frm_type = HI_VDEC_B_FRM;
    }

    out->event_param.err_frm.err_ratio = ((hi_s8*)args)[1];

    return HI_SUCCESS;
}

static hi_s32 vdec_cnvt_err_std_2_vdec(hi_void *args, hi_s32 len, hi_vdec_event *out)
{
    hi_vdec_std std;

    if (args == HI_NULL || len != sizeof(hi_u32)) {
        VDEC_PRINT_ERR("err std invalid args %p len %d\n", args, len);
        return HI_FAILURE;
    }

    std = vdec_cnvt_std_2_vdec(((hi_u32*)args)[0]);

    out->event_id = HI_VDEC_EVT_ERR_STANDARD;
    out->event_param.err_std = std;

    return HI_SUCCESS;
}

static hi_s32 vdec_cnvt_userdata_2_vdec(hi_void *args, hi_s32 len, hi_vdec_event *out)
{
    out->event_id = HI_VDEC_EVT_NEW_USER_DATA;

    return HI_SUCCESS;
}

static hi_s32 vdec_cnvt_unsupport_2_vdec(hi_void *args, hi_s32 len, hi_vdec_event *out)
{
    out->event_id = HI_VDEC_EVT_UNSUPPORT;

    return HI_SUCCESS;
}

static hi_s32 vdec_cnvt_first_pts_2_vdec(hi_void *args, hi_s32 len, hi_vdec_event *out)
{
    if (args == HI_NULL || len != sizeof(hi_s64)) {
        VDEC_PRINT_ERR("first pts param invalid args %p len %d\n", args, len);
        return HI_FAILURE;
    }

    out->event_id = HI_VDEC_EVT_FIRST_PTS;
    out->event_param.first_pts = ((hi_s64 *)args)[0];

    return HI_SUCCESS;
}

static hi_s32 vdec_cnvt_second_pts_2_vdec(hi_void *args, hi_s32 len, hi_vdec_event *out)
{
    if (args == HI_NULL || len != sizeof(hi_s64)) {
        VDEC_PRINT_ERR("second pts param invalid args %p len %d\n", args, len);
        return HI_FAILURE;
    }

    out->event_id = HI_VDEC_EVT_SECOND_PTS;
    out->event_param.second_pts = ((hi_s64 *)args)[0];

    return HI_SUCCESS;
}

static hi_s32 vdec_cnvt_norm_2_vdec(hi_void *args, hi_s32 len, hi_vdec_event *out)
{
    hi_s32 ret;

    if (args == HI_NULL || len != sizeof(hi_vdec_norm_param)) {
        VDEC_PRINT_ERR("norm invalid args %p len %d\n", args, len);
        return HI_FAILURE;
    }

    ret = memcpy_s(&out->event_param.norm_param, sizeof(hi_vdec_norm_param), args, len);
    if (ret != EOK) {
        VDEC_PRINT_ERR("memcpy_s fail!\n");
        return HI_FAILURE;
    }

    out->event_id = HI_VDEC_EVT_NORM_CHG;

    return HI_SUCCESS;
}

static hi_s32 vdec_cnvt_frm_pack_type_2_vdec(hi_void *args, hi_s32 len, hi_vdec_event *out)
{
    if (args == HI_NULL || len != sizeof(hi_u32)) {
        VDEC_PRINT_ERR("frm pack type invalid args %p len %d\n", args, len);
        return HI_FAILURE;
    }

    out->event_id = HI_VDEC_EVT_FRM_PCK_CHG;
    out->event_param.frm_3d_type = *((hi_u32*)args);

    return HI_SUCCESS;
}

/* convert from vfmw to vdec */
hi_vdec_std vdec_cnvt_std_2_vdec(vfmw_vid_std std)
{
    switch (std) {
        case VFMW_MPEG2:
            return HI_VDEC_STD_MPEG2;
        case VFMW_MPEG4:
            return HI_VDEC_STD_MPEG4;
        case VFMW_H263:
            return HI_VDEC_STD_H263;
        case VFMW_SORENSON:
            return HI_VDEC_STD_SORENSON;
        case VFMW_H264:
            return HI_VDEC_STD_H264;
        case VFMW_MVC:
            return HI_VDEC_STD_H264_MVC;
        case VFMW_HEVC:
            return HI_VDEC_STD_H265;
        case VFMW_VP6:
            return HI_VDEC_STD_VP6;
        case VFMW_VP6F:
            return HI_VDEC_STD_VP6F;
        case VFMW_VP6A:
            return HI_VDEC_STD_VP6A;
        case VFMW_VP8:
            return HI_VDEC_STD_VP8;
        case VFMW_VP9:
            return HI_VDEC_STD_VP9;
        case VFMW_AVS:
            return HI_VDEC_STD_AVS;
        case VFMW_AVS2:
            return HI_VDEC_STD_AVS2;
        case VFMW_AVS3:
            return HI_VDEC_STD_AVS3;
        case VFMW_VC1:
            return HI_VDEC_STD_VC1;
        case VFMW_AV1:
            return HI_VDEC_STD_AV1;
        case VFMW_MJPEG:
            return HI_VDEC_STD_MJPEG;
        case VFMW_REAL8:
            return HI_VDEC_STD_REAL8;
        case VFMW_REAL9:
            return HI_VDEC_STD_REAL9;
        case VFMW_DIVX3:
            return HI_VDEC_STD_DIVX3;
        default:
            return HI_VDEC_STD_MAX;
    }
}

hi_vdec_norm_type vdec_cnvt_disp_norm_2_vdec(hi_u32 norm)
{
    switch (norm) {
        case 1: /* 1 is PAL in vfmw */
            return HI_VDEC_NORM_PAL;
        case 2: /* 2 is NTSC in vfmw */
            return HI_VDEC_NORM_NTSC;
        default:
            return HI_VDEC_NORM_MAX;
    }
}

hi_vdec_frm_rate_type vdec_cnvt_frmrate_type_2_vdec(vfmw_pts_framrate_type type)
{
    switch (type) {
        case PTS_FRMRATE_TYPE_PTS:
            return HI_VDEC_FR_TYPE_PTS;
        case PTS_FRMRATE_TYPE_STREAM:
            return HI_VDEC_FR_TYPE_STM;
        case PTS_FRMRATE_TYPE_USER:
            return HI_VDEC_FR_TYPE_USER;
        case PTS_FRMRATE_TYPE_USER_PTS:
            return HI_VDEC_FR_TYPE_USER_PTS;
        default:
            return HI_VDEC_FR_TYPE_PTS;
    }
}

hi_vdec_sampling_type vdec_cnvt_sampling_type_2_vdec(hi_u32 scan_type)
{
    switch (scan_type) {
        case 0: /* 0 is progressive */
            return HI_VDEC_PROGRESSIVE;
        case 1: /* 1 is interlaced */
            return HI_VDEC_INTERLACED;
        default:
            return HI_VDEC_SAMPLE_UNKNOWN;
    }
}

hi_vdec_bit_depth vdec_cnvt_bit_depth_2_vdec(hi_u32 bit_depth)
{
    switch (bit_depth) {
        case 10: /* 10 is 10bit */
            return HI_VDEC_BIT_DEPTH_10;
        case 12: /* 12 is 12bit */
            return HI_VDEC_BIT_DEPTH_12;
        case 8: /* 8 is 8bit */
            return HI_VDEC_BIT_DEPTH_8;
        default:
            return HI_VDEC_BIT_DEPTH_8;
    }
}

hi_vdec_color_space vdec_cnvt_color_space_2_vdec(vfmw_vid_std std, vfmw_color_desc *color_info,
    hi_u32 width, hi_u32 height)
{
    if (std == VFMW_H264 || std == VFMW_HEVC) {
        hi_u8 color_primaries = color_info->colour_primaries;
        hi_u8 matrix_coeffs = color_info->matrix_coeffs;
        hi_u8 full_range_flag = color_info->full_rangeFlag;

        switch (color_primaries) {
            case 1: /* 1 specific value in protrol */
                if (matrix_coeffs == 1) {
                    return (full_range_flag == 1) ? HI_VDEC_CS_BT709_YUV_FULL : HI_VDEC_CS_BT709_YUV_LIMITED;
                }
                break;

            case 5: /* 5 specific value in protrol */
                if (matrix_coeffs == 5) { /* 5 specific value in protrol */
                    return (full_range_flag == 1) ? HI_VDEC_CS_BT601_YUV_FULL : HI_VDEC_CS_BT601_YUV_LIMITED;
                }
                break;

            case 9: /* 9 specific value in protrol */
                if (matrix_coeffs == 9) { /* 9 specific value in protrol */
                    return (full_range_flag == 1) ? HI_VDEC_CS_BT2020_YUV_FULL : HI_VDEC_CS_BT2020_YUV_LIMITED;
                } else if (matrix_coeffs == 10) { /* 10 specific value in protrol */
                    return (full_range_flag == 1) ? HI_VDEC_CS_BT2020_YUV_FULL : HI_VDEC_CS_BT2020_YUV_LIMITED;
                }
                break;

            default:
                break;
        }
    }

    if (width >= 1280 || height >= 720) { /* 1280 720 is the threshold */
        return HI_VDEC_CS_BT709_YUV_LIMITED;
    } else {
        return HI_VDEC_CS_BT601_YUV_LIMITED;
    }
}

hi_s32 vdec_cnvt_event_2_vdec(hi_s32 chan_id, hi_s32 type, hi_void *args, hi_s32 len,
    hi_vdec_event *out)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 i = 0;
    fn_evt_process_type evt_process = HI_NULL;
    vdec_cnvt_evt_item items[] = {
        { EVNT_NEW_IMAGE,     vdec_cnvt_new_image_2_vdec },
        { EVNT_RLS_STREAM,    vdec_cnvt_stream_2_vdec },
        { EVNT_LAST_FRAME,    vdec_cnvt_eos_2_vdec },
        { EVNT_ERR_FRAME,     vdec_cnvt_err_frm_2_vdec },
        { EVNT_VIDSTD_ERROR,  vdec_cnvt_err_std_2_vdec },
        { EVNT_USRDAT,        vdec_cnvt_userdata_2_vdec },
        { EVNT_UNSUPPORT,     vdec_cnvt_unsupport_2_vdec },
        { EVNT_FIRST_PTS,     vdec_cnvt_first_pts_2_vdec },
        { EVNT_SECOND_PTS,    vdec_cnvt_second_pts_2_vdec },
        { EVNT_NORM_CHG,      vdec_cnvt_norm_2_vdec },
        { EVNT_FRM_PCK_CHG,   vdec_cnvt_frm_pack_type_2_vdec },
    };

    out->event_id = HI_VDEC_EVT_MAX_RESERVE;

    for (i = 0; i < sizeof(items) / sizeof(vdec_cnvt_evt_item); i++) {
        if (items[i].evt_id == type) {
            evt_process = items[i].evt_process;
            break;
        }
    }

    if (evt_process != HI_NULL) {
        ret = evt_process(args, len, out);
    }

    return ret;
}

/* convert from vdec to vfmw */
vfmw_vid_std vdec_cnvt_std_2_vfmw(hi_vdec_std std)
{
    switch (std) {
        case HI_VDEC_STD_MPEG1:
        case HI_VDEC_STD_MPEG2:
            return VFMW_MPEG2;
        case HI_VDEC_STD_MPEG4:
            return VFMW_MPEG4;
        case HI_VDEC_STD_H263:
            return VFMW_H263;
        case HI_VDEC_STD_SORENSON:
            return VFMW_SORENSON;
        case HI_VDEC_STD_H264:
            return VFMW_H264;
        case HI_VDEC_STD_H264_MVC:
            return VFMW_MVC;
        case HI_VDEC_STD_H265:
            return VFMW_HEVC;
        case HI_VDEC_STD_VP6:
            return VFMW_VP6;
        case HI_VDEC_STD_VP6F:
            return VFMW_VP6F;
        case HI_VDEC_STD_VP6A:
            return VFMW_VP6A;
        case HI_VDEC_STD_VP8:
            return VFMW_VP8;
        case HI_VDEC_STD_VP9:
            return VFMW_VP9;
        case HI_VDEC_STD_AVS:
            return VFMW_AVS;
        case HI_VDEC_STD_AVS2:
            return VFMW_AVS2;
        case HI_VDEC_STD_AVS3:
            return VFMW_AVS3;
        case HI_VDEC_STD_VC1:
            return VFMW_VC1;
        case HI_VDEC_STD_AV1:
            return VFMW_AV1;
        case HI_VDEC_STD_MJPEG:
            return VFMW_MJPEG;
        case HI_VDEC_STD_REAL8:
            return VFMW_REAL8;
        case HI_VDEC_STD_REAL9:
            return VFMW_REAL9;
        case HI_VDEC_STD_DIVX3:
            return VFMW_DIVX3;
        case HI_VDEC_STD_WMV1:
            return VFMW_WMV1;
        case HI_VDEC_STD_WMV2:
            return VFMW_WMV2;
        case HI_VDEC_STD_WMV3:
            return VFMW_WMV3;
        default:
            return VFMW_STD_MAX;
    }
}

vfmw_dec_mode vdec_cnvt_decmode_2_vfmw(hi_vdec_dec_mode mode)
{
    switch (mode) {
        case HI_VDEC_DEC_MODE_NORMAL:
            return VFMW_DEC_MODE_NORMAL;
        case HI_VDEC_DEC_MODE_IP:
            return VFMW_DEC_MODE_IP;
        case HI_VDEC_DEC_MODE_I:
            return VFMW_DEC_MODE_I;
        case HI_VDEC_DEC_MODE_DROP_INVALID_B:
            return VFMW_DEC_MODE_DROP_INVALID_B;
        case HI_VDEC_DEC_MODE_FIRST_I:
            return VFMW_DEC_MODE_FIRST_I;
        default:
            return VFMW_DEC_MODE_NORMAL;
    }
}

vfmw_cmp vdec_cnvt_cmp_2_vfmw(hi_vdec_cmp_mode mode)
{
    switch (mode) {
        case HI_VDEC_CMP_ADJUST:
            return VFMW_CMP_ADJUST;
        case HI_VDEC_CMP_OFF:
            return VFMW_CMP_OFF;
        case HI_VDEC_CMP_ON:
            return VFMW_CMP_ON;
        default:
            return VFMW_CMP_MAX;
    }
}

hi_u32 vdec_cnvt_cmd_id_2_vfmw(hi_u32 cmd)
{
    switch (cmd) {
        case HI_VDEC_CMD_INIT_INPUT_BUF:
            return VFMW_CID_INIT_ES_BUF;
        case HI_VDEC_CMD_DEINIT_INPUT_BUF:
            return VFMW_CID_DEINIT_ES_BUF;
        case VDEC_PRIVATE_CMD_GET_STREAM:
            return VFMW_CID_GET_USRDEC_STREAM;
        case VDEC_PRIVATE_CMD_PUT_STREAM:
            return VFMW_CID_PUT_USRDEC_STREAM;
        case VDEC_PRIVATE_CMD_GET_FRAME:
            return VFMW_CID_GET_USRDEC_FRAME;
        case VDEC_PRIVATE_CMD_PUT_FRAME:
            return VFMW_CID_PUT_USRDEC_FRAME;
        case VDEC_PRIVATE_CMD_BIND_ES_USR_ADDR:
            return VFMW_CID_BIND_ES_USR_ADDR;
        default:
            return VFMW_CID_MAX;
    }
}

vfmw_flush_type vdec_cnvt_flush_type_2_vfmw(hi_vdec_flush_type type)
{
    switch (type) {
        case HI_VDEC_FLUSH_IN:
            return VFMW_FLUSH_IN;
        case HI_VDEC_FLUSH_OUT:
            return VFMW_FLUSH_OUT;
        case HI_VDEC_FLUSH_ALL:
            return VFMW_FLUSH_ALL;
        default:
            return VFMW_FLUSH_MAX;
    }
}

vfmw_pts_framrate_type vdec_cnvt_frmrate_type_2_vfmw(hi_vdec_frm_rate_type type)
{
    switch (type) {
        case HI_VDEC_FR_TYPE_PTS:
            return PTS_FRMRATE_TYPE_PTS;
        case HI_VDEC_FR_TYPE_STM:
            return PTS_FRMRATE_TYPE_STREAM;
        case HI_VDEC_FR_TYPE_USER:
            return PTS_FRMRATE_TYPE_USER;
        case HI_VDEC_FR_TYPE_USER_PTS:
            return PTS_FRMRATE_TYPE_USER_PTS;
        default:
            return PTS_FRMRATE_TYPE_MAX;
    }
}

hi_u32 vdec_cnvt_event_map_2_vfmw(hi_u32 vdec_event_map)
{
    hi_s32 i;
    hi_u32 vfmw_event_map = 0;
    hi_u32 event_table[][VDEC_EVENT_TABLE_ITEM_SIZE] = {
        { HI_VDEC_EVT_NEW_FRAME, EVNT_NEW_IMAGE },
        { HI_VDEC_EVT_NEW_STREAM, EVNT_RLS_STREAM },
        { HI_VDEC_EVT_EOS, EVNT_LAST_FRAME },
        { HI_VDEC_EVT_ERR_FRAME, EVNT_ERR_FRAME },
        { HI_VDEC_EVT_ERR_STANDARD, EVNT_VIDSTD_ERROR },
        { HI_VDEC_EVT_NEW_USER_DATA, EVNT_USRDAT },
        { HI_VDEC_EVT_UNSUPPORT, EVNT_UNSUPPORT },
        { HI_VDEC_EVT_FIRST_PTS, EVNT_FIRST_PTS },
        { HI_VDEC_EVT_SECOND_PTS, EVNT_SECOND_PTS },
        { HI_VDEC_EVT_NORM_CHG, EVNT_NORM_CHG },
        { HI_VDEC_EVT_FRM_PCK_CHG, EVNT_FRM_PCK_CHG },
        { HI_VDEC_EVT_MAX_RESERVE, EVNT_MAX},
    };

    for (i = 0; event_table[i][0] != HI_VDEC_EVT_MAX_RESERVE; i++) {
        if ((vdec_event_map & event_table[i][0]) != 0) {
            vfmw_event_map |= event_table[i][1];
        }
    }

    return vfmw_event_map;
}

vfmw_yuv_fmt vdec_cnvt_yuv_fmt_2_vfmw(vdec_yuv_fmt vdec_fmt)
{
    switch (vdec_fmt) {
        case VDEC_YUV_SP400:
            return SPYCbCr400;
        case VDEC_YUV_SP411:
            return SPYCbCr411;
        case VDEC_YUV_SP420:
            return SPYCbCr420;
        case VDEC_YUV_SP422_1X2:
            return SPYCbCr422_1X2;
        case VDEC_YUV_SP422_2X1:
            return SPYCbCr422_2X1;
        case VDEC_YUV_SP444:
            return SPYCbCr444;
        case VDEC_YUV_P400:
            return PLNYCbCr400;
        case VDEC_YUV_P411:
            return PLNYCbCr411;
        case VDEC_YUV_P420:
            return PLNYCbCr420;
        case VDEC_YUV_P422_1X2:
            return PLNYCbCr422_1X2;
        case VDEC_YUV_P422_2X1:
            return PLNYCbCr422_2X1;
        case VDEC_YUV_P444:
            return PLNYCbCr444;
        case VDEC_YUV_P410:
            return PLNYCbCr410;
        default:
            return VFMW_YUV_FMT_MAX;
    }
}

hi_void vdec_cnvt_standard_ext(hi_vdec_std std, const hi_vdec_std_ext *in_ext, vfmw_std_ext *out_ext)
{
    if (std == HI_VDEC_STD_VC1) {
        out_ext->vc1_ext.is_adv_profile = in_ext->vc1.is_adv_profile;
        out_ext->vc1_ext.codec_version = in_ext->vc1.codec_version;
    } else if (std == HI_VDEC_STD_VP6 || std == HI_VDEC_STD_VP6A || std == HI_VDEC_STD_VP6F) {
        out_ext->vp6_ext.reversed = in_ext->vp6.need_reverse;
    }
}

