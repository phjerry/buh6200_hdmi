/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_ip_fdr.h"

#include "hal_vdp_comm.h"
#include "hal_vdp_ip_para.h"
#include "hal_vdp_ip_coef.h"

#include "hal_vdp_reg_fdr.h"
#include "hal_vdp_reg_region.h"
#include "hal_vdp_reg_vid.h"
#include "hal_vdp_reg_para.h"

#ifdef VDP_CBB_TEST_SUPPORT
#include "hal_vdp_test_fdr.h"
#endif

#define MB_WTH      32
#define CLIP(a)     (((a) >= 0) ? (a) : (0))
#define MIN(a, b)   ((a) < (b) ? (a) : (b))
#define MAX(a, b)   ((a) > (b) ? (a) : (b))
#define ADPQP_TH0   (((3 & 0xff) << 24) | ((3 & 0xff) << 16) | ((10 & 0xff) << 8) | (10 & 0xff))
#define ADPQP_TH1   (((5 & 0xff) << 24) | ((15 & 0xff) << 16) | ((0 & 0xff) << 8) | (0 & 0xff))

static hi_u32 g_fdr_offset_addr[VDP_LAYER_VID_BUTT] = {
    (0x00000 / 4),  /* vid1 - vid1 */ /* 4 is number */
    (0x00200 / 4),  /* vid2 - vid1 */ /* 4 is number */
    (0x00400 / 4),  /* vid3 - vid1 */ /* 4 is number */
    (0x30000 / 4),  /* vid0 - vid1 */ /* 4 is number */
    (0x30300 / 4)   /* DC   - vid1 */ /* 4 is number */
};

#define ADDR_64HIGN_TO_32BIT 32

static hi_void vdp_fdr_set_addr_dcmp(hi_u32 offset, vdp_fdr_addr *fdr_addr, vdp_fdr_info *fdr_info)
{
    if (fdr_info->dcmp_type == VDP_FDR_DCMP_TYPE_LINE) {
        vdp_fdr_vid_setlmheadstride(g_vdp_reg, offset, fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].lum_str);
        vdp_fdr_vid_setchmheadstride(g_vdp_reg, offset, fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].chm_str);
        vdp_fdr_vid_sethvhdaddrl(g_vdp_reg, offset, (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].lum_addr) & 0xffffffff);
        vdp_fdr_vid_sethvhdcaddrl(g_vdp_reg, offset, (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].chm_addr) & 0xffffffff);
        vdp_fdr_vid_sethvhdaddrh(g_vdp_reg, offset,
            (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].lum_addr >> ADDR_64HIGN_TO_32BIT) & 0xffffffff);
        vdp_fdr_vid_sethvhdcaddrh(g_vdp_reg, offset,
            (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].chm_addr >> ADDR_64HIGN_TO_32BIT) & 0xffffffff);
    }

    if ((fdr_info->dcmp_type == VDP_FDR_DCMP_TYPE_SEG) &&
        (fdr_info->data_type == VDP_FDR_VID_DATA_TYPE_RGB888 ||
         fdr_info->data_type == VDP_FDR_VID_DATA_TYPE_ARGB8888)) {
        vdp_fdr_vid_sethvhdaddrl(g_vdp_reg, offset, (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].lum_addr) & 0xffffffff);
        vdp_fdr_vid_sethvhdcaddrl(g_vdp_reg, offset, (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].chm_addr) & 0xffffffff);
        vdp_fdr_vid_sethvhdaddrh(g_vdp_reg, offset,
            (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].lum_addr >> ADDR_64HIGN_TO_32BIT) & 0xffffffff);
        vdp_fdr_vid_sethvhdcaddrh(g_vdp_reg, offset,
            (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].chm_addr >> ADDR_64HIGN_TO_32BIT) & 0xffffffff);
        vdp_fdr_vid_setlmheadstride(g_vdp_reg, offset, (fdr_info->in_rect.w / 32) * 16); /* 32, 16 is number */
        vdp_fdr_vid_setchmheadstride(g_vdp_reg, offset, (fdr_info->in_rect.w / 32) * 16); /* 32, 16 is number */
    }
}

static hi_void vdp_fdr_set_addr(hi_u32 offset, vdp_fdr_addr *fdr_addr, vdp_fdr_info *fdr_info)
{
    vdp_fdr_vid_setvhdaddrl(g_vdp_reg, offset, (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].lum_addr) & 0xffffffff);
    vdp_fdr_vid_setvhdcaddrl(g_vdp_reg, offset, (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].chm_addr) & 0xffffffff);
    vdp_fdr_vid_setvhdnaddrl(g_vdp_reg, offset, (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA_3D].lum_addr) & 0xffffffff);
    vdp_fdr_vid_setvhdncaddrl(g_vdp_reg, offset, (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA_3D].chm_addr) & 0xffffffff);
    vdp_fdr_vid_sethvhdaddrl(g_vdp_reg, offset, (fdr_addr->addr[VDP_FDR_VID_ADDR_HEAD].lum_addr) & 0xffffffff);
    vdp_fdr_vid_sethvhdcaddrl(g_vdp_reg, offset, (fdr_addr->addr[VDP_FDR_VID_ADDR_HEAD].chm_addr) & 0xffffffff);
    vdp_fdr_vid_setbvhdaddrl(g_vdp_reg, offset, (fdr_addr->addr[VDP_FDR_VID_ADDR_2B].lum_addr) & 0xffffffff);
    vdp_fdr_vid_setbvhdcaddrl(g_vdp_reg, offset, (fdr_addr->addr[VDP_FDR_VID_ADDR_2B].chm_addr) & 0xffffffff);
    vdp_fdr_vid_setvhdaddrh(g_vdp_reg, offset,
        (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].lum_addr >> ADDR_64HIGN_TO_32BIT) & 0xffffffff);
    vdp_fdr_vid_setvhdcaddrh(g_vdp_reg, offset,
        (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].chm_addr >> ADDR_64HIGN_TO_32BIT) & 0xffffffff);
    vdp_fdr_vid_setvhdnaddrh(g_vdp_reg, offset,
        (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA_3D].lum_addr >> ADDR_64HIGN_TO_32BIT) & 0xffffffff);
    vdp_fdr_vid_setvhdncaddrh(g_vdp_reg, offset,
        (fdr_addr->addr[VDP_FDR_VID_ADDR_DATA_3D].chm_addr >> ADDR_64HIGN_TO_32BIT) & 0xffffffff);
    vdp_fdr_vid_sethvhdaddrh(g_vdp_reg, offset,
        (fdr_addr->addr[VDP_FDR_VID_ADDR_HEAD].lum_addr >> ADDR_64HIGN_TO_32BIT) & 0xffffffff);
    vdp_fdr_vid_sethvhdcaddrh(g_vdp_reg, offset,
        (fdr_addr->addr[VDP_FDR_VID_ADDR_HEAD].chm_addr >> ADDR_64HIGN_TO_32BIT) & 0xffffffff);

    vdp_fdr_vid_setbvhdaddrh(g_vdp_reg, offset,
        (fdr_addr->addr[VDP_FDR_VID_ADDR_2B].lum_addr >> ADDR_64HIGN_TO_32BIT) & 0xffffffff);
    vdp_fdr_vid_setbvhdcaddrh(g_vdp_reg, offset,
        (fdr_addr->addr[VDP_FDR_VID_ADDR_2B].chm_addr >> ADDR_64HIGN_TO_32BIT) & 0xffffffff);

    /* left & right use same stride */
    vdp_fdr_vid_setlmstride(g_vdp_reg, offset, fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].lum_str);
    vdp_fdr_vid_setchmstride(g_vdp_reg, offset, fdr_addr->addr[VDP_FDR_VID_ADDR_DATA].chm_str);
    vdp_fdr_vid_setlmheadstride(g_vdp_reg, offset, fdr_addr->addr[VDP_FDR_VID_ADDR_HEAD].lum_str);
    vdp_fdr_vid_setchmheadstride(g_vdp_reg, offset, fdr_addr->addr[VDP_FDR_VID_ADDR_HEAD].chm_str);
    vdp_fdr_vid_setlmtilestride(g_vdp_reg, offset, fdr_addr->addr[VDP_FDR_VID_ADDR_2B].lum_str);
    vdp_fdr_vid_setchmtilestride(g_vdp_reg, offset, fdr_addr->addr[VDP_FDR_VID_ADDR_2B].chm_str);

    vdp_fdr_set_addr_dcmp(offset, fdr_addr, fdr_info);
    return;
}

static hi_void vdp_fdr_set_src_info(hi_u32 offset, vdp_fdr_info *fdr_info)
{
    vdp_fdr_vid_setsrccropx(g_vdp_reg, offset, fdr_info->in_rect.x);
    vdp_fdr_vid_setsrccropy(g_vdp_reg, offset, fdr_info->in_rect.y);
    vdp_fdr_vid_setiresow(g_vdp_reg, offset, fdr_info->in_rect.w - 1);
    vdp_fdr_vid_setiresoh(g_vdp_reg, offset, fdr_info->in_rect.h - 1);
    vdp_fdr_vid_setsrcw(g_vdp_reg, offset, fdr_info->src_rect.w - 1);
    vdp_fdr_vid_setsrch(g_vdp_reg, offset, fdr_info->src_rect.h - 1);

    vdp_fdr_vid_setdatafmt(g_vdp_reg, offset, fdr_info->data_fmt);
    vdp_fdr_vid_setdatatype(g_vdp_reg, offset, fdr_info->data_type);

    vdp_fdr_vid_setdatawidth(g_vdp_reg, offset, fdr_info->data_width);
    vdp_fdr_vid_setdispmode(g_vdp_reg, offset, fdr_info->src_disp_mode);

    return;
}

static hi_void vdp_fdr_set_ctrl_info(hi_u32 offset, vdp_fdr_info *fdr_info)
{
    /* mute */
    vdp_fdr_vid_setmuteen(g_vdp_reg, offset, fdr_info->mute_en);
    vdp_fdr_vid_setmutereqen(g_vdp_reg, offset, fdr_info->mute_req_en);
    vdp_fdr_vid_setmutey(g_vdp_reg, offset, fdr_info->mute_bkg.y);
    vdp_fdr_vid_setmutecb(g_vdp_reg, offset, fdr_info->mute_bkg.u);
    vdp_fdr_vid_setmutecr(g_vdp_reg, offset, fdr_info->mute_bkg.v);
    vdp_fdr_vid_setrmode3d(g_vdp_reg, offset, fdr_info->rmode_3d);

    /* smmu */
    vdp_fdr_vid_setlmbypass2d(g_vdp_reg, offset, !fdr_info->smmu_en);
    vdp_fdr_vid_setchmbypass2d(g_vdp_reg, offset, !fdr_info->smmu_en);
    vdp_fdr_vid_setlmbypass3d(g_vdp_reg, offset, !fdr_info->smmu_en);
    vdp_fdr_vid_setchmbypass3d(g_vdp_reg, offset, !fdr_info->smmu_en);

    vdp_fdr_vid_setlmrmode(g_vdp_reg, offset, fdr_info->rmode);
    vdp_fdr_vid_setchmrmode(g_vdp_reg, offset, fdr_info->rmode);
    vdp_fdr_vid_setchmcopyen(g_vdp_reg, offset, fdr_info->chm_copy_en);

    /* draw hor & ver */
    vdp_fdr_vid_setdrawpixelmode(g_vdp_reg, offset, fdr_info->draw_pixel_mode);
    vdp_fdr_vid_setlmdrawmode(g_vdp_reg, offset, fdr_info->draw_mode);
    vdp_fdr_vid_setchmdrawmode(g_vdp_reg, offset, fdr_info->draw_mode);

    vdp_fdr_vid_setflipen(g_vdp_reg, offset, fdr_info->flip_en);
    vdp_fdr_vid_setuvorderen(g_vdp_reg, offset, fdr_info->uv_order);

    return;
}

static hi_void vdp_fdr_set_tunl(hi_u32 offset, vdp_fdr_info *fdr_info)
{
    /* tunl */
    vdp_fdr_vid_setreqldmode(g_vdp_reg, offset, fdr_info->tunl_info.req_ld_mode);
    vdp_fdr_vid_settunlinterval(g_vdp_reg, offset, fdr_info->tunl_info.tunl_interval);
    vdp_fdr_vid_settunlthd(g_vdp_reg, offset, 0x100);
    vdp_fdr_vid_settunladdrl(g_vdp_reg, offset, (fdr_info->tunl_info.tunl_addr) & 0xffffffff);
    vdp_fdr_vid_settunladdrh(g_vdp_reg, offset, (fdr_info->tunl_info.tunl_addr >> ADDR_64HIGN_TO_32BIT) & 0xffffffff);

    return;
}

static hi_void vdp_fdr_set_dcmp_line(vdp_fdr_info *fdr_info, vdp_dcmp_line_input *y_cfg, vdp_dcmp_line_input *c_cfg)
{
    hi_u32 bit_depth;

    /* y */
    y_cfg->cmp_mode = 0;
    y_cfg->chroma_en = 0;
    y_cfg->is_lossless = fdr_info->dcmp_line_cfg.lossless_y;
    y_cfg->bit_depth = fdr_info->data_width;
    bit_depth = y_cfg->bit_depth * 2 + 8; /* 2, 8 is number */
    y_cfg->budget_mb_bits = MB_WTH * bit_depth * 1000 / fdr_info->dcmp_line_cfg.cmp_ratio_y; /* 1000 is number */
    y_cfg->budget_mb_bits = y_cfg->is_lossless ? MB_WTH * bit_depth : y_cfg->budget_mb_bits;
    y_cfg->max_mb_qp = y_cfg->is_lossless ? 0 : bit_depth - (y_cfg->budget_mb_bits - 32) / MB_WTH; /* 32 is number */
    y_cfg->smooth_deltabits_thr = 85; /* 85 is number */
    y_cfg->adpqp_thr0 = ADPQP_TH0;
    y_cfg->adpqp_thr1 = ADPQP_TH1;

    /* c */
    c_cfg->cmp_mode = 0;
    c_cfg->chroma_en = 1;
    c_cfg->is_lossless = fdr_info->dcmp_line_cfg.lossless_c;
    c_cfg->bit_depth = fdr_info->data_width;
    bit_depth = c_cfg->bit_depth * 2 + 8; /* 2, 8 is number */
    c_cfg->budget_mb_bits = MB_WTH * bit_depth * 1000 / fdr_info->dcmp_line_cfg.cmp_ratio_c; /* 1000 is number */
    c_cfg->budget_mb_bits = c_cfg->is_lossless ? MB_WTH * bit_depth : c_cfg->budget_mb_bits;
    c_cfg->max_mb_qp = c_cfg->is_lossless ? 0 : bit_depth - (c_cfg->budget_mb_bits - 32) / MB_WTH; /* 32 is number */
    c_cfg->smooth_deltabits_thr = 85; /* 85 is number */
    c_cfg->adpqp_thr0 = ADPQP_TH0;
    c_cfg->adpqp_thr1 = ADPQP_TH1;

    return;
}

static hi_void vdp_fdr_set_dcmp(hi_u32 offset, vdp_fdr_info *fdr_info)
{
    vdp_dcmp_line_input line_dcmp_y;
    vdp_dcmp_line_input line_dcmp_c;

    vdp_fdr_vid_setdcmptype(g_vdp_reg, offset, fdr_info->dcmp_type); /* dcmp_type => dcmp_en */

    if (fdr_info->dcmp_type == VDP_FDR_DCMP_TYPE_SEG) {
    } else if (fdr_info->dcmp_type == VDP_FDR_DCMP_TYPE_FRM) {
        /* unsupport */
    } else if (fdr_info->dcmp_type == VDP_FDR_DCMP_TYPE_LINE) {
        vdp_fdr_set_dcmp_line(fdr_info, &line_dcmp_y, &line_dcmp_c);

        vdp_fdr_vid_setstridelum(g_vdp_reg, offset, 0);
        vdp_fdr_vid_setbitdepthlum(g_vdp_reg, offset, line_dcmp_y.bit_depth);
        vdp_fdr_vid_setstrideenlum(g_vdp_reg, offset, 0);
        vdp_fdr_vid_setmaxmbqplum(g_vdp_reg, offset, line_dcmp_y.max_mb_qp);
        vdp_fdr_vid_setchromaenlum(g_vdp_reg, offset, line_dcmp_y.chroma_en);
        vdp_fdr_vid_setcmpmodelum(g_vdp_reg, offset, line_dcmp_y.cmp_mode);
        vdp_fdr_vid_setislosslesslum(g_vdp_reg, offset, line_dcmp_y.is_lossless);
        vdp_fdr_vid_seticeenlum(g_vdp_reg, offset, fdr_info->dcmp_line_cfg.enable);
        vdp_fdr_vid_setframeheightlum(g_vdp_reg, offset, fdr_info->src_rect.h - 1);
        vdp_fdr_vid_setframewidthlum(g_vdp_reg, offset, fdr_info->src_rect.w - 1);
        vdp_fdr_vid_setadpqpthr0lum(g_vdp_reg, offset, line_dcmp_y.adpqp_thr0);
        vdp_fdr_vid_setadpqpthr1lum(g_vdp_reg, offset, line_dcmp_y.adpqp_thr1);
        vdp_fdr_vid_setsmoothdeltabitsthrlum(g_vdp_reg, offset, line_dcmp_y.smooth_deltabits_thr);

        vdp_fdr_vid_setstridechm(g_vdp_reg, offset, 0);
        vdp_fdr_vid_setbitdepthchm(g_vdp_reg, offset, line_dcmp_c.bit_depth);
        vdp_fdr_vid_setstrideenchm(g_vdp_reg, offset, 0);
        vdp_fdr_vid_setmaxmbqpchm(g_vdp_reg, offset, line_dcmp_c.max_mb_qp);
        vdp_fdr_vid_setchromaenchm(g_vdp_reg, offset, line_dcmp_c.chroma_en);
        vdp_fdr_vid_setcmpmodechm(g_vdp_reg, offset, line_dcmp_c.cmp_mode);
        vdp_fdr_vid_setislosslesschm(g_vdp_reg, offset, line_dcmp_c.is_lossless);
        vdp_fdr_vid_seticeenchm(g_vdp_reg, offset, fdr_info->dcmp_line_cfg.enable);

        if (fdr_info->in_fmt == VDP_FDR_VID_IFMT_SP_422) {
            vdp_fdr_vid_setframeheightchm(g_vdp_reg, offset, fdr_info->src_rect.h - 1);
        } else {
            vdp_fdr_vid_setframeheightchm(g_vdp_reg, offset, (fdr_info->src_rect.w / 2) - 1); /* 2 is number */
        }

        vdp_fdr_vid_setframewidthchm(g_vdp_reg, offset, fdr_info->src_rect.w - 1);
        vdp_fdr_vid_setadpqpthr0chm(g_vdp_reg, offset, line_dcmp_c.adpqp_thr0);
        vdp_fdr_vid_setadpqpthr1chm(g_vdp_reg, offset, line_dcmp_c.adpqp_thr1);
        vdp_fdr_vid_setsmoothdeltabitsthrchm(g_vdp_reg, offset, line_dcmp_c.smooth_deltabits_thr);
    } else {
        /* unsupport */
    }

    return;
}

static hi_void vdp_fdr_set_coef(hi_void)
{
    vdp_coef_send_cfg stCoefSend;
    void *p_coef_array[1];
    hi_u32 lut_length[1];
    hi_u32 coef_bit_length[1];
    struct file *fp_region_coef = NULL;

    coef_bit_length[0] = 32; /* 32 is number */
    lut_length[0] = FDR_REGION_OFFSET * FDR_REGION_NUM_MAX;
    p_coef_array[0] = g_vdp_coef_buf_addr.vir_addr[VDP_COEF_BUF_REGION_V1];

    stCoefSend.coef_addr         = g_vdp_coef_buf_addr.vir_addr[VDP_COEF_BUF_REGION_V1];
    stCoefSend.cycle_num         = 4; /* 4 is number */
    stCoefSend.lut_num           = 1;
    stCoefSend.burst_num         = 1;
    stCoefSend.p_coef_array      = p_coef_array;
    stCoefSend.lut_length        = lut_length;
    stCoefSend.coef_bit_length   = coef_bit_length;
    stCoefSend.data_type         = DRV_COEF_DATA_TYPE_U32;
    stCoefSend.fp_coef           = fp_region_coef;
    vdp_ip_coef_sendcoef(&stCoefSend);

    return;
}

static hi_void vdp_fdr_set_mrg(hi_u32 offset, vdp_fdr_info *fdr_info)
{
    hi_u32 i = 0;

    offset = 0;

    vdp_fdr_vid_setmrgenable(g_vdp_reg, offset, fdr_info->mrg_mode_en);
    vdp_fdr_vid_setglbmmubypass(g_vdp_reg, offset, !(fdr_info->smmu_en));

    for (i = 0; i < fdr_info->mrg_total_num; i++, offset += FDR_REGION_OFFSET) {
        vdp_fdr_vid_setmrgen(g_vdp_reg, offset, fdr_info->mrg_info[i].enable);
        vdp_fdr_vid_setmrgmuteen(g_vdp_reg, offset, fdr_info->mrg_info[i].mute_en);
        vdp_fdr_vid_setmrgcropen(g_vdp_reg, offset, fdr_info->mrg_info[i].crop_en);
        vdp_fdr_vid_setmrgxpos(g_vdp_reg, offset, fdr_info->mrg_info[i].disp_rect.x);
        vdp_fdr_vid_setmrgypos(g_vdp_reg, offset, fdr_info->mrg_info[i].disp_rect.y);
        vdp_fdr_vid_setmrgwidth(g_vdp_reg, offset, fdr_info->mrg_info[i].disp_rect.w - 1);
        vdp_fdr_vid_setmrgheight(g_vdp_reg, offset, fdr_info->mrg_info[i].disp_rect.h - 1);
        vdp_fdr_vid_setmrgsrcwidth(g_vdp_reg, offset, fdr_info->mrg_info[i].in_rect.w - 1);
        vdp_fdr_vid_setmrgsrcheight(g_vdp_reg, offset, fdr_info->mrg_info[i].in_rect.h - 1);
        vdp_fdr_vid_setmrgsrchoffset(g_vdp_reg, offset, fdr_info->mrg_info[i].in_rect.x);
        vdp_fdr_vid_setmrgsrcvoffset(g_vdp_reg, offset, fdr_info->mrg_info[i].in_rect.y);
        vdp_fdr_vid_setmrgyaddr(g_vdp_reg, offset,
            (fdr_info->mrg_info[i].addr[VDP_FDR_VID_ADDR_DATA].lum_addr) >> 4); /* 4 is number */
        vdp_fdr_vid_setmrgcaddr(g_vdp_reg, offset,
            (fdr_info->mrg_info[i].addr[VDP_FDR_VID_ADDR_DATA].chm_addr) >> 4); /* 4 is number */
        vdp_fdr_vid_setmrgyhaddr(g_vdp_reg, offset,
            (fdr_info->mrg_info[i].addr[VDP_FDR_VID_ADDR_DATA].lum_addr) >> 4); /* 4 is number */
        vdp_fdr_vid_setmrgchaddr(g_vdp_reg, offset,
            (fdr_info->mrg_info[i].addr[VDP_FDR_VID_ADDR_DATA].chm_addr) >> 4); /* 4 is number */
        vdp_fdr_vid_setmrgystride(g_vdp_reg, offset, fdr_info->mrg_info[i].addr[VDP_FDR_VID_ADDR_DATA].lum_str);
        vdp_fdr_vid_setmrgcstride(g_vdp_reg, offset, fdr_info->mrg_info[i].addr[VDP_FDR_VID_ADDR_DATA].chm_str);
        vdp_fdr_vid_setmrgyhstride(g_vdp_reg, offset, (fdr_info->mrg_info[i].addr[VDP_FDR_VID_ADDR_DATA].lum_str));
        vdp_fdr_vid_setmrgchstride(g_vdp_reg, offset, (fdr_info->mrg_info[i].addr[VDP_FDR_VID_ADDR_DATA].chm_str));
    }

    vdp_fdr_set_coef();
    vdp_para_setparaupvhdchn(g_vdp_reg, VDP_COEF_BUF_REGION_V1);

    return;
}

hi_void vdp_ip_fdr_mac_init(hi_u32 layer, vdp_fdr_init_info *mac_info)
{
    hi_u32 offset = g_fdr_offset_addr[layer];

    /* mac */
    vdp_fdr_vid_setprerden(g_vdp_reg, offset, mac_info->pre_rd_en);
    vdp_fdr_vid_setoflmaster(g_vdp_reg, offset, mac_info->ofl_master);
    vdp_fdr_vid_setreqlen(g_vdp_reg, offset, mac_info->req_len);
    vdp_fdr_vid_setreqctrl(g_vdp_reg, offset, mac_info->req_ctrl);

    return;
}

hi_void vdp_ip_fdr_set(hi_u32 layer, vdp_fdr_info *fdr_info)
{
    hi_u32 offset = g_fdr_offset_addr[layer];

    vdp_fdr_set_ctrl_info(offset, fdr_info);
    vdp_fdr_set_src_info(offset, fdr_info);
    vdp_fdr_set_dcmp(offset, fdr_info);

    if (fdr_info->tunl_info.tunl_en == HI_TRUE) {
        vdp_fdr_set_tunl(offset, fdr_info);
    }

    if (fdr_info->mrg_mode_en && layer == VDP_LAYER_VID1) {
        vdp_fdr_set_mrg(offset, fdr_info);
    }

#ifdef VDP_CBB_TEST_SUPPORT
    if (fdr_info->cbb_test != HI_NULL) {
        vdp_fdr_test(offset, fdr_info);
    }
#endif

    return;
}

hi_void vdp_ip_fdr_addr(hi_u32 layer, vdp_fdr_addr *fdr_addr, vdp_fdr_info *fdr_info)
{
    hi_u32 offset = g_fdr_offset_addr[layer];

    vdp_fdr_set_addr(offset, fdr_addr, fdr_info);

    return;
}

static hi_bool vdp_ip_fdr_get_ldcmp_error(vdp_dispchn_chn disp_id)
{
    hi_bool v0_err = HI_FALSE;
    hi_bool v1_err = HI_FALSE;
    hi_bool v3_err = HI_FALSE;

    u_v0_ctrl v0_ctrl;
    u_v1_ctrl v1_ctrl;
    u_v3_ctrl v3_ctrl;

    u_vid_src_info vid_src_info;
    u_vdp_core_v1_vid_src_info v1_vid_src_info;
    u_vdp_core_v3_vid_src_info v3_vid_src_info;

    hi_u32 v0_sta = 0;
    hi_u32 v1_sta = 0;
    hi_u32 v3_sta = 0;

    if (disp_id == VDP_DISPCHN_CHN_DHD0) {
        v0_sta = vdp_regread((uintptr_t)(&(g_vdp_reg->vid_line_ldcmp_error_sta0.u32)));
        v0_sta |= vdp_regread((uintptr_t)(&(g_vdp_reg->vid_line_ldcmp_error_sta1.u32)));
        v0_sta |= vdp_regread((uintptr_t)(&(g_vdp_reg->vid_line_ldcmp_error_sta2.u32)));
        v0_sta |= vdp_regread((uintptr_t)(&(g_vdp_reg->vid_line_ldcmp_error_sta3.u32)));

        v1_sta = vdp_regread((uintptr_t)(&(g_vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta0.u32)));
        v1_sta |= vdp_regread((uintptr_t)(&(g_vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta1.u32)));
        v1_sta |= vdp_regread((uintptr_t)(&(g_vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta2.u32)));
        v1_sta |= vdp_regread((uintptr_t)(&(g_vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta3.u32)));

        v0_ctrl.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v0_ctrl.u32)));
        v1_ctrl.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v1_ctrl.u32)));

        vid_src_info.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->vid_src_info.u32)));
        v1_vid_src_info.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->vdp_core_v1_vid_src_info.u32)));

        /* v0 line dcmp */
        if ((v0_ctrl.bits.surface_en == 0x1) && (vid_src_info.bits.dcmp_type == 0x2) && ((v0_sta & 0x7) != 0)) {
            v0_err = HI_TRUE;
        }

        /* v1 line dcmp */
        if ((v1_ctrl.bits.surface_en == 0x1) && (v1_vid_src_info.bits.dcmp_type == 0x2) && ((v1_sta & 0x7) != 0)) {
            v1_err = HI_TRUE;
        }
    } else if (disp_id == VDP_DISPCHN_CHN_DHD1) {
        v3_sta = vdp_regread((uintptr_t)(&(g_vdp_reg->vdp_core_v3_vid_line_cdcmp_error_sta.u32)));
        v3_sta |= vdp_regread((uintptr_t)(&(g_vdp_reg->vdp_core_v3_vid_line_ldcmp_error_sta0.u32)));

        v3_ctrl.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v3_ctrl.u32)));

        v3_vid_src_info.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->vdp_core_v3_vid_src_info.u32)));

        /* v3 line dcmp */
        if ((v3_ctrl.bits.surface_en == 0x1) && (v3_vid_src_info.bits.dcmp_type == 0x2) && ((v3_sta & 0x7) != 0)) {
            v3_err = HI_TRUE;
        }
    }

    return v0_err | v1_err | v3_err;
}

static hi_bool vdp_ip_fdr_get_tdcmp_error(hi_void)
{
    hi_bool v0_err = HI_FALSE;

    u_v0_ctrl v0_ctrl;
    u_vid_src_info vid_src_info;

    hi_u32 v0_debug;

    v0_ctrl.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->v0_ctrl.u32)));
    vid_src_info.u32 = vdp_regread((uintptr_t)(&(g_vdp_reg->vid_src_info.u32)));

    v0_debug = vdp_regread((uintptr_t)(&(g_vdp_reg->vid_tile_ldcmp_debug0.u32)));
    v0_debug |= vdp_regread((uintptr_t)(&(g_vdp_reg->vid_tile_ldcmp_debug1.u32)));

    v0_debug = vdp_regread((uintptr_t)(&(g_vdp_reg->vid_tile_cdcmp_debug0.u32)));
    v0_debug |= vdp_regread((uintptr_t)(&(g_vdp_reg->vid_tile_cdcmp_debug1.u32)));

    if ((v0_ctrl.bits.surface_en == 1) && (vid_src_info.bits.dcmp_type == 1) && (v0_debug & 0xE1) != 0) {
        v0_err = HI_TRUE;
    }

    return v0_err;
}

hi_s32 vdp_ip_fdr_get_dcmp_error(vdp_dispchn_chn disp_id)
{
    hi_bool ldcmp_err;
    hi_bool tdcmp_err = HI_FALSE;

    ldcmp_err = vdp_ip_fdr_get_ldcmp_error(disp_id);

    if (disp_id == VDP_DISPCHN_CHN_DHD0) {
        tdcmp_err = vdp_ip_fdr_get_tdcmp_error();
    }

    if (ldcmp_err == HI_TRUE || tdcmp_err == HI_TRUE) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_void vdp_ip_fdr_clean_dcmp_error(vdp_dispchn_chn disp_id)
{
    if (disp_id == VDP_DISPCHN_CHN_DHD0) {
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->vid_line_ldcmp_error_sta0.u32)), 0xffffff);
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->vid_line_ldcmp_error_sta1.u32)), 0xffffff);
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->vid_line_ldcmp_error_sta2.u32)), 0xffffff);
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->vid_line_ldcmp_error_sta3.u32)), 0xffffff);

        vdp_regwrite((uintptr_t)(&(g_vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta0.u32)), 0xffffff);
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta1.u32)), 0xffffff);
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta2.u32)), 0xffffff);
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->vdp_core_v1_vid_line_ldcmp_error_sta3.u32)), 0xffffff);

        vdp_regwrite((uintptr_t)(&(g_vdp_reg->vid_tile_ldcmp_debug0.u32)), 0xffffff);
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->vid_tile_ldcmp_debug1.u32)), 0xffffff);
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->vid_tile_cdcmp_debug0.u32)), 0xffffff);
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->vid_tile_cdcmp_debug1.u32)), 0xffffff);
    } else {
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->vdp_core_v3_vid_line_ldcmp_error_sta0.u32)), 0xffffff);
        vdp_regwrite((uintptr_t)(&(g_vdp_reg->vdp_core_v3_vid_line_cdcmp_error_sta.u32)), 0xffffff);
    }
}

hi_void vdp_ip_fdr_testpattern(hi_u32 layer, vdp_fdr_testpattern *testpattern_info)
{
    hi_u32 offset = g_fdr_offset_addr[layer];

    /* testpatten */
    vdp_fdr_vid_settestpatternen(g_vdp_reg, offset, testpattern_info->pattern_en);
    vdp_fdr_vid_settpmode(g_vdp_reg, offset, testpattern_info->demo_mode);
    vdp_fdr_vid_settpcolormode(g_vdp_reg, offset, testpattern_info->color_mode);
    vdp_fdr_vid_settplinew(g_vdp_reg, offset, testpattern_info->line_width);
    vdp_fdr_vid_settpspeed(g_vdp_reg, offset, testpattern_info->speed);
    vdp_fdr_vid_settpseed(g_vdp_reg, offset, testpattern_info->seed);

    return;
}

hi_void vdp_ip_fdr_disable_layer(hi_u32 layer, hi_u32 region_index)
{
    if (layer == VDP_LAYER_VID0) {
        vdp_vid_setlayerenable(g_vdp_reg, layer, HI_FALSE);
    } else if (layer == VDP_LAYER_VID1) {
        vdp_fdr_vid_setmrgen(g_vdp_reg, FDR_REGION_OFFSET * region_index, HI_FALSE);
        vdp_para_setparaupvhdchn(g_vdp_reg, VDP_COEF_BUF_REGION_V1);
    } else if (layer == VDP_LAYER_VID3) {
        vdp_vid_setlayerenable(g_vdp_reg, layer, HI_FALSE);
    }

    return;
}

