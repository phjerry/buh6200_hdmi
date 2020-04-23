/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "vfmw_pdt.h"
#include "dbg.h"
#include "vfmw_intf.h"
#include "vfmw_define.h"
#include "vfmw_sys.h"
#include "hi_drv_sys.h"
#include <linux/module.h>
#include <linux/hisilicon/hi_license.h>
#include <hi_osal.h>
#ifndef VFMW_MDC_SUPPORT
#ifdef VFMW_AV1_SUPPORT
#include "vdh_av1.h"
#endif
#include "vctrl.h"
#endif

#define VFMW_SEG_BUF_MAX 0x1000000
#define VFMW_SEG_BUF_MIN 0xc00000

#define PMC_ADDR       0x00a15000
#define PMC_LEN        0x1000
#define HPM6_CTRL0     0x5c0
#define HPM6_CTRL1     0x5c4
#define HPM6_CTRL2     0x5c8
#define HPM6_CTRL3     0x5cc
#define PWM5_MUX_CTRL  0xe10
#define PWM6_CTRL0     0xea0
#define PWM6_CTRL0_VAL 0x004300dd
#define HPM6_CTRL3_VAL 0x03000000
#define HPM6_CTRL0_VAL 0x04000001
#define HPM_BIT_OFS    12
#define HPM_VALUE      0x3ff

#define OPT_ADDR     0x00b00300
#define OPT_LEN      0x100
#define OPT_VDH_HMP  0x18
#define OPT_COMP_EN  0x40
#define OPT_COMP_VAL 0x47

#define GEN_ADDR  0x00841874
#define GEN_LEN   0x4
#define CONST_VAL 0xffff
#define RATIO_BIT 16
#define RATIO_ABS 1300
#define CONST_ABS 1423

#define VOL_MAX  1000
#define VOL_MIN  400
#define VOL_STEP 110

#define PWM_BIT 16
#define PWM_OFS 0xdd

static pdt_license g_vdh_license;
static vdh_glb_reg_info g_vdh_glb_reg;

static vdh_glb_reg_info *pdt_get_glb_reg(hi_void)
{
    return &g_vdh_glb_reg;
}

#ifndef VFMW_MDC_SUPPORT
hi_s32 pdt_get_slot_wh(mem_req_info *req)
{
    hi_s32 alignw = 64; /* 64 w align */
    hi_s32 alignh = 16; /* 16 h align */

    if (req->vid_std == VFMW_AV1 ||
        req->vid_std == VFMW_AVS3) {
        alignw = 128; /* 128 :a number */
    }

    /* field need 32 align */
    if (req->vid_std == VFMW_H264 ||
        req->vid_std == VFMW_AVS ||
        req->vid_std == VFMW_VC1 ||
        req->vid_std == VFMW_MPEG2) {
        alignh = 32; /* 32 h align */
    }

    req->slot_width = VFMW_ALIGN_UP(req->dec_width, alignw);
    req->slot_height = VFMW_ALIGN_UP(req->dec_height, alignh);

    req->disp_slot_width = VFMW_ALIGN_UP(req->disp_width, alignw);
    req->disp_slot_height = VFMW_ALIGN_UP(req->disp_height, alignh);

    return HI_SUCCESS;
}

hi_s32 pdt_get_dec_cmp_flag(mem_req_info *req, hi_bool is_need_disp)
{
    hi_s32 compress = 0;
    vfmw_vid_std vid_std = req->vid_std;
    hi_s32 width = req->dec_width;
    hi_s32 hidth = req->dec_height;

    if (is_need_disp == HI_FALSE && req->cmp_mode != VFMW_CMP_ADJUST) {
        if (req->cmp_mode == VFMW_CMP_ON) {
            compress = 1;
        } else if (req->cmp_mode == VFMW_CMP_OFF) {
            compress = 0;
        } else {
            compress = -1;
        }

        return compress;
    }

    if (width * hidth >= 3840 * 2160 && /* 3840 :a number 2160 :a number */
        (vid_std == VFMW_H264 ||
         vid_std == VFMW_HEVC ||
         vid_std == VFMW_AVS2 ||
         vid_std == VFMW_AVS3)) {
        compress = 1;
    } else if (vid_std == VFMW_VP9) {
        compress = 1;
    }

    return compress;
}

hi_s32 pdt_get_disp_cmp_flag(mem_req_info *req)
{
    hi_s32 compress_en = 0;
    hi_s32 width;
    hi_s32 height;
    vfmw_vid_std vid_std;

    vid_std = req->vid_std;
    width = req->dec_width;
    height = req->dec_height;

    if (req->cmp_mode == VFMW_CMP_ON) {
        compress_en = 1;
    } else if (req->cmp_mode == VFMW_CMP_OFF) {
        compress_en = 0;
    } else { /* Adjust */
        if (width * height >= 3840 * 2160 && /* 3840 :a number 2160 :a number */
            (vid_std == VFMW_H264 ||
             vid_std == VFMW_HEVC ||
             vid_std == VFMW_AVS2 ||
             vid_std == VFMW_AVS3)) {
            compress_en = 1;
        } else if (vid_std == VFMW_VP9) {
            compress_en = 1;
        }
    }

    return compress_en;
}

hi_s32 pdt_get_disp_bit_depth(mem_req_info *req, hi_bool compress)
{
    hi_u8 src_bit_depth = req->bit_depth;
    hi_s8 disp_bit_depth = -1;

    if (req->disp_bit_depth_mode == VFMW_BIT_DEPTH_NORMAL) {
        disp_bit_depth = (src_bit_depth == 9) ? 10 : src_bit_depth; /* 9 :a number 10 :a number */
    } else if (req->disp_bit_depth_mode == VFMW_BIT_DEPTH_8BIT_ONLY) {
        disp_bit_depth = 8; /* 8 :a number */
    } else if (req->disp_bit_depth_mode == VFMW_BIT_DEPTH_16BIT_EXPANDED) {
        if (compress == HI_TRUE && src_bit_depth > 8) { /* 8 :a number */
            dprint(PRN_ERROR, "%s 16bit expand only support linear!\n", __func__);
            return -1;
        }

        disp_bit_depth = (src_bit_depth > 8) ? 16 : 8; /* 8 :a number 16 :a number 8 :a number */
    }

    return disp_bit_depth;
}

hi_s32 pdt_get_dec_bit_depth(mem_req_info *req)
{
    return (req->bit_depth > 8) ? 10 : 8; /* 8 :a number 10 :a number 8 :a number */
}

hi_s32 pdt_is_share_fs(vfmw_vid_std vid_std)
{
    if (vid_std == VFMW_VP6 || vid_std == VFMW_VP6A || vid_std == VFMW_VP6F || vid_std == VFMW_VP9 ||
        vid_std == VFMW_MPEG2 || vid_std == VFMW_MPEG4 || vid_std == VFMW_H263 || vid_std == VFMW_SORENSON ||
        vid_std == VFMW_VC1) {
        return 1;
    }

    return 0;
}

hi_s32 pdt_get_mem_req(mem_req_info *req, mem_need_info *need)
{
    hi_u32 width;
    hi_u32 height;
    hi_u32 disp_width;
    hi_u32 disp_height;
    hi_u32 bit_depth;
    hi_u32 dec_num;
    hi_u32 mult_coef;
    hi_u32 divi_coef;
    hi_u32 mv_width;
    hi_u32 mv_height;
    hi_bool is_need_disp = req->vid_std == VFMW_AV1 ? HI_TRUE : HI_FALSE;
    vctrl_dflt *dflt = vctrl_get_dflt();

#ifdef VDH_V5R7B5_ESCHIP
    is_need_disp = 0;
#endif

    need->dec_info.compress_en = pdt_get_dec_cmp_flag(req, is_need_disp);
    if (need->dec_info.compress_en < 0) {
        dprint(PRN_ERROR, "Get dec compress error!\n");
        return HI_FAILURE;
    }

    if (is_need_disp == HI_FALSE) {
        dec_num = req->need_dec_num + req->extra_fs_num;
    } else {
        dec_num = req->need_dec_num;
    }

    bit_depth = pdt_get_dec_bit_depth(req);
    need->dec_info.bit_depth = bit_depth;

    pdt_get_slot_wh(req);
    width = (dflt->fix_frame_width != -1) ? dflt->fix_frame_width : req->slot_width;
    height = (dflt->fix_frame_height != -1) ? dflt->fix_frame_height : req->slot_height;

    need->dec_info.dec_num = (dflt->fix_frame_num != -1) ? dflt->fix_frame_num : dec_num;
    need->dec_info.dec_size = width * height * 3 / 2 * bit_depth / 8; /* 3 :a number 2 :a number 8 :a number */
    need->dec_info.dec_width = width;
    need->dec_info.dec_height = height;
    need->dec_info.dec_stride = width;

    if (need->dec_info.compress_en == 1) {
        need->dec_info.head_stride = VFMW_ALIGN_UP(width, 2048) >> 5; /* 2048 :a number 5 :a number */
        /* 64 :a number 2 :a number */
        need->dec_info.head_size = (VFMW_ALIGN_UP(height, 64) * need->dec_info.head_stride) >> 2;
        need->dec_info.dec_size += need->dec_info.head_size * 3 / 2; /* 3 :a number 2 :a number */
    } else {
        need->dec_info.head_stride = 0;
        need->dec_info.head_size = 0;
    }

#ifdef VFMW_MODULE_LOWDLY_SUPPORT
    if (is_need_disp == HI_FALSE) {
        need->dec_info.dec_size += 32; /* 32 :a number */
    }
#endif

#ifdef VFMW_AV1_SUPPORT
    if (req->vid_std == VFMW_AV1) {
        need->dec_info.dec_size += AV1_CDF_LEN;
        need->dec_info.dec_size += AV1_SEGID_LEN(req->slot_width, req->slot_height);
    }
#endif

    need->pmv_num = (dflt->fix_frame_num != -1) ? dflt->fix_frame_num : req->need_pmv_num;
    need->min_parallel_num = 1;
    if (width >= 7680 || height >= 4320) { /* 7680 4320 is 8K width and height */
        need->pmv_num++;
        need->min_parallel_num++;
    }

    mv_width = VFMW_ALIGN_UP(width, 64); /* 64 :a number */
    mv_height = VFMW_ALIGN_UP(height, 64); /* 64 :a number */

    if (req->vid_std == VFMW_AVS3) {
        mult_coef = 1;
        divi_coef = 32; /* 32 :a number */
    } else if (req->vid_std == VFMW_AV1) {
        mult_coef = 9; /* 9 :a number */
        divi_coef = 2 * 64; /* 2 :a number 64 :a number */
    } else if (req->vid_std == VFMW_AVS2 ||
               req->vid_std == VFMW_HEVC) {
        mult_coef = 1;
        divi_coef = 16; /* 16 :a number */
    } else if (req->vid_std == VFMW_VP9) {
        mult_coef = 1;
        divi_coef = 4; /* 4 :a number */
        mv_width = VP9_MAX_PIX_WIDTH;
        mv_height = VP9_MAX_PIX_HEIGHT;
    } else {
        mult_coef = 1;
        divi_coef = 4; /* 4 :a number */
    }

    need->pmv_size = mv_width * mv_height * mult_coef / divi_coef;

    /* Fix PMV prefetch error */
    need->pmv_size += 256; /* 256 :a number */

    /* Normal attachment memory */
    need->nor_size = 0;
    if (req->need_mtdt == 1) {
        need->nor_size += 4 * 1024; /* 4 :a number 1024 :a number */
    }

    /* Secure attachment memory */
    need->sec_size = 0;

    need->attach = ATTACH_NONE;
    if (need->nor_size != 0 || need->sec_size != 0) {
        if (is_need_disp == HI_TRUE) {
            need->attach = ATTACH_DISP;
        } else {
            need->attach = ATTACH_DEC;
        }
    }

    need->disp_info.is_disp_need = is_need_disp;
    if (is_need_disp == HI_TRUE) {
        need->disp_info.is_disp_need = 1;
        need->disp_info.disp_num = req->extra_fs_num + req->need_disp_num;

        if (req->is_hdr || req->is_omx || req->alloc_type == 1) {
            need->disp_info.disp_num += 2; /* 2 :a number */
        }
        need->disp_info.compress_en = pdt_get_disp_cmp_flag(req);
        need->disp_info.bit_depth = pdt_get_disp_bit_depth(req, need->disp_info.compress_en);
        disp_width = req->disp_slot_width;
        disp_height = req->disp_slot_height;
        /* 3 :a number 2 :a number 8 :a number */
        need->disp_info.disp_size = disp_width * disp_height * 3 / 2 * need->disp_info.bit_depth / 8;
        need->disp_info.disp_width = disp_width;
        need->disp_info.disp_height = disp_height;
        need->disp_info.disp_stride = disp_width;

        if (need->disp_info.compress_en == 1) {
            need->disp_info.head_stride = VFMW_ALIGN_UP(width, 2048) >> 5; /* 2048 :a number 5 :a number */
            /* 64 :a number 2 :a number */
            need->disp_info.head_size = (VFMW_ALIGN_UP(height, 64) * need->disp_info.head_stride) >> 2;
            need->disp_info.disp_size += need->disp_info.head_size * 3 / 2; /* 3 :a number 2 :a number */
        } else {
            need->disp_info.head_stride = 0;
            need->disp_info.head_size = 0;
        }

#ifdef VFMW_MODULE_LOWDLY_SUPPORT
        need->disp_info.disp_size += 32; /* 32 :a number */
#endif
        if (pdt_is_share_fs(req->vid_std) == 1) {
            need->dec_info.dec_num = need->disp_info.disp_num = MAX(need->dec_info.dec_num, need->disp_info.disp_num);
        }
    }

    return HI_SUCCESS;
}
#endif

hi_void pdt_set_vdh_vmin(hi_void)
{
    hi_u32 test_hmp;
    hi_u32 otp_hmp;
    hi_u32 comp_en;
    hi_u32 comp_val;
    hi_u32 data;
    hi_u32 hmp;
    hi_u32 vmin;
    hi_u32 pwm;
    hi_u32 const_abs;
    hi_u32 ratio_abs;
    hi_u8 *gen_vir = HI_NULL;
    hi_u8 *pmc_vir = HI_NULL;
    hi_u8 *opt_vir = HI_NULL;

    /* test hpm */
    pmc_vir = OS_KMAP_REG(PMC_ADDR, PMC_LEN);
    *(volatile hi_u32 *)(pmc_vir + PWM6_CTRL0) = PWM6_CTRL0_VAL;
    *(volatile hi_u32 *)(pmc_vir + HPM6_CTRL3) = HPM6_CTRL3_VAL;
    *(volatile hi_u32 *)(pmc_vir + HPM6_CTRL0) = HPM6_CTRL0_VAL;
    OS_UDELAY(10000); /* 10000us = 10ms wait ready */

    data = *(volatile hi_u32 *)(pmc_vir + HPM6_CTRL1);
    test_hmp = data & HPM_VALUE;
    test_hmp += (data >> HPM_BIT_OFS) & HPM_VALUE;
    data = *(volatile hi_u32 *)(pmc_vir + HPM6_CTRL2);
    test_hmp += data & HPM_VALUE;
    test_hmp += (data >> HPM_BIT_OFS) & HPM_VALUE;
    test_hmp = test_hmp / 4; /* div 4 get avg val */

    /* opt hpm */
    opt_vir = OS_KMAP_REG(OPT_ADDR, OPT_LEN);
    otp_hmp = *(volatile hi_u32 *)(opt_vir + OPT_VDH_HMP);
    otp_hmp = (otp_hmp >> 8) & HPM_VALUE; /* 8 hmp ofs */

    if (otp_hmp == 0) {
        hmp = test_hmp;
    } else {
        hmp = MIN(test_hmp, otp_hmp);
    }

    comp_val = 0;
    comp_en = *(volatile hi_u32 *)(opt_vir + OPT_COMP_EN);
    comp_en = (comp_en >> 4) & 0x1; /* 4 comp en ofs */
    if (comp_en) {
        comp_val = *(volatile hi_u32 *)(opt_vir + OPT_COMP_VAL);
        if ((comp_val >> 3) & 1) { /* 3 comp en ofs */
            comp_val = ((comp_val & 7) + 1) * 10; /* 7:3bit, 10:mv */
        }
    }

    /* ratio para */
    gen_vir = OS_KMAP_REG(GEN_ADDR, GEN_LEN);
    const_abs = *(volatile hi_u32 *)(gen_vir) & CONST_VAL;
    ratio_abs = *(volatile hi_u32 *)(gen_vir) >> RATIO_BIT;
    OS_KUNMAP_REG(gen_vir);

    if (const_abs == 0 || ratio_abs == 0) {
        const_abs = CONST_ABS;
        ratio_abs = RATIO_ABS;
    }

    vmin = const_abs - ratio_abs * hmp / 1000; /* div 1000 to float */
    vmin += comp_val;
    vmin = MIN(VOL_MAX, MAX(vmin, VOL_MIN));

    /* vmin to pwm */
    pwm = 1 + (VOL_MAX - vmin) * VOL_STEP / VOL_MIN * 2; /* 2 double */
    pwm = (pwm << 16) + 0xdd; /* 16 bit 0xdd ofs */
    *(volatile hi_u32 *)(pmc_vir + PWM6_CTRL0) = pwm;
    OS_UDELAY(10); /* 10 :a number */
    *(volatile hi_u32 *)(pmc_vir + PWM5_MUX_CTRL) = 0x6; /* 0x6 ctrl pwm6 */
    OS_UDELAY(10); /* 10 :a number */

    dprint(PRN_DBG, "test %d otp %d hmp %d comp_en %d comp_val %d\n",
           test_hmp, otp_hmp, hmp, comp_en, comp_val, const_abs, ratio_abs);
    dprint(PRN_DBG, "const_abs %d ratio_abs %d vdh vmin %d pwm %x\n",
           const_abs, ratio_abs, vmin, pwm);

    OS_KUNMAP_REG(opt_vir);
    OS_KUNMAP_REG(pmc_vir);
}

hi_void pdt_enable_vdh(hi_void)
{
#ifndef HI_FPGA_SUPPORT
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG202.u32 = 0x3fe3; /* crg config bug dapt to vdh 0x3fe3 */
#endif
}

hi_void pdt_disable_vdh(hi_void)
{
#ifndef HI_FPGA_SUPPORT
    hi_drv_sys_get_crg_reg_ptr()->PERI_CRG202.bits.vdh_cken = 0;
#endif
}

hi_s32 pdt_open_hardware(hi_void)
{
    UADDR phy_addr;
    hi_u8 *vir_addr = HI_NULL;
#ifdef VFMW_MMU_SUPPORT
    UADDR cb_ttbr;
    UADDR err_read_addr;
    UADDR err_write_addr;
    hi_u32 i;
#endif
    vdh_glb_reg_info *glb_reg = HI_NULL;

    glb_reg = pdt_get_glb_reg();

    pdt_enable_vdh();

    /* vdh crg */
    phy_addr = VDH_CRG_REG_PHY_ADDR;
    vir_addr = OS_KMAP_REG(phy_addr, 0x3000); /* 0x3000 length long enough */
    if (vir_addr == HI_NULL) {
        dprint(PRN_ERROR, "%s Map Vdh Crg Fail\n", __func__);
        return HI_FAILURE;
    }

    glb_reg->vdh_crg_phy = phy_addr;
    glb_reg->vdh_crg_vir = vir_addr;

#ifdef VDH_V5R7B5_ESCHIP
    *(volatile hi_u32 *)(vir_addr + 0x4) = 0xaabf;
#endif

    for (i = 0; i < VDH_MMU_NUM; i++) {
        *(volatile hi_u32 *)(vir_addr + 0x1C00 + 0x100 * i) = 0x1f7; /* 0x1C00 0x1f7 is outstanding defult config */
    }

#ifdef VFMW_MMU_SUPPORT
    phy_addr = MMU_REG_PHY_ADDR;
    vir_addr = OS_KMAP_REG(phy_addr, MMU_REG_LENGTH * VDH_MMU_NUM);
    if (vir_addr == HI_NULL) {
        dprint(PRN_ERROR, "%s Map Vdh Mmu Fail\n", __func__);
        return HI_FAILURE;
    }

    glb_reg->vdh_mmu_phy = phy_addr;
    glb_reg->vdh_mmu_vir = vir_addr;

    OS_GET_MMU_TABLE(&cb_ttbr, &err_read_addr, &err_write_addr);

    for (i = 0; i < VDH_MMU_NUM; i++) {
        /* SRC */
        *(volatile hi_u32 *)(vir_addr + i * MMU_REG_LENGTH + VREG_OFS_MMU_SRC) =
            (0 << VERG_MMU_SRC_GLB_BYPASS_BIT);

        /* CTRL */
        *(volatile hi_u32 *)(vir_addr + i * MMU_REG_LENGTH + VREG_OFS_MMU_CTRL) =
            (0 << VERG_MMU_CTRL_PT_TYPE_BIT) | /* 0: 4K */
            (1 << VERG_MMU_CTRL_INT_EN_BIT)  |
            (0x3 << VERG_MMU_CTRL_PTW_PF_BIT);

        /* INT PT ADDR */
        *(volatile hi_u32 *)(vir_addr + i * MMU_REG_LENGTH + VREG_OFS_MMU_INT_MASK) = 0;

        *(volatile hi_u32 *)(vir_addr + i * MMU_REG_LENGTH + VREG_OFS_MMU_CB_TTBR) = (hi_u32)cb_ttbr;
        /* 32 :a number */
        *(volatile hi_u32 *)(vir_addr + i * MMU_REG_LENGTH + VREG_OFS_MMU_CB_TTBR_H) = ((hi_u64)cb_ttbr) >> 32;
        *(volatile hi_u32 *)(vir_addr + i * MMU_REG_LENGTH + VREG_OFS_MMU_ERR_RD_ADDR) = (hi_u32)err_read_addr;
        *(volatile hi_u32 *)(vir_addr + i * MMU_REG_LENGTH + VREG_OFS_MMU_ERR_RD_ADDR_H) =
            ((hi_u64)err_read_addr) >> 32; /* 32 :a number */
        *(volatile hi_u32 *)(vir_addr + i * MMU_REG_LENGTH + VREG_OFS_MMU_ERR_WR_ADDR) =
            (hi_u32)err_write_addr;
        *(volatile hi_u32 *)(vir_addr + i * MMU_REG_LENGTH + VREG_OFS_MMU_ERR_WR_ADDR_H) =
            ((hi_u64)err_write_addr) >> 32; /* 32 :a number */
    }
#endif

    return HI_SUCCESS;
}

hi_s32 pdt_close_hardware(hi_void)
{
    vdh_glb_reg_info *glb_reg = HI_NULL;

    glb_reg = pdt_get_glb_reg();

    /* vdh crg */
    OS_KUNMAP_REG(glb_reg->vdh_crg_vir);
    glb_reg->vdh_crg_vir = HI_NULL;

    /* vdh mmu */
    OS_KUNMAP_REG(glb_reg->vdh_mmu_vir);
    glb_reg->vdh_mmu_vir = HI_NULL;

    return HI_SUCCESS;
}

hi_s32 pdt_init(hi_void)
{
    hi_s32 ret;

    ret = pdt_open_hardware();

    return ret;
}

hi_s32 pdt_exit(hi_void)
{
    hi_s32 ret;

    ret = pdt_close_hardware();

    return ret;
}

hi_s32 pdt_suspend(hi_void *args)
{
    pdt_disable_vdh();

    return HI_SUCCESS;
}

hi_s32 pdt_resume(hi_void *args)
{
    pdt_enable_vdh();

    return HI_SUCCESS;
}

hi_s32 pdt_set_clk(hi_u32 *clk_cfg)
{
    dprint(PRN_ALWS, "%s not complete yet.\n", __func__);

    return HI_SUCCESS;
}

hi_s32 pdt_reset_vdh(hi_void)
{
    hi_s32 i;
    hi_s32 ret;
    UADDR phy_addr;
    hi_u8 *vir_addr = HI_NULL;
    hi_u32 reg_ofs;
    hi_u32 rst_req;
    hi_u32 rst_ok;

    ret = HI_FAILURE;

    pdt_enable_vdh();

    phy_addr = VDH_CRG_REG_PHY_ADDR;
    vir_addr = OS_KMAP_REG(phy_addr, 0x20);
    if (vir_addr == HI_NULL) {
        dprint(PRN_ERROR, "%s Map Vdh Crg Fail\n", __func__);
        return HI_FAILURE;
    }

    reg_ofs = VDM_RST_REG_PHY_ADDR - VDH_CRG_REG_PHY_ADDR;
    rst_req = *(volatile hi_u32 *)(vir_addr + reg_ofs);
    ((vcrg_vdh_srst_req *)(&rst_req))->vdh_all_srst_req = 1;
    *(volatile hi_u32 *)(vir_addr + reg_ofs) = rst_req;
    OS_MB();

    for (i = 0; i < 1000; i++) { /* 1000 :a number */
        OS_UDELAY(1);
        reg_ofs = VDM_RST_OK_PHY_ADDR - VDH_CRG_REG_PHY_ADDR;
        rst_ok = *(volatile hi_u32 *)(vir_addr + reg_ofs);
        if (((vcrg_vdh_srst_ok *)(&rst_ok))->vdh_all_srst_ok == 1) {
            ret = HI_SUCCESS;
            break;
        }
    }

    if (i == 1000) { /* 1000 :a number */
        dprint(PRN_ERROR, "reset VDH ERROR!!!\n");
    }

    reg_ofs = VDM_RST_REG_PHY_ADDR - VDH_CRG_REG_PHY_ADDR;
    rst_req = *(volatile hi_u32 *)(vir_addr + reg_ofs);
    ((vcrg_vdh_srst_req *)(&rst_req))->vdh_all_srst_req = 0;
    *(volatile hi_u32 *)(vir_addr + reg_ofs) = rst_req;
    OS_MB();

    OS_KUNMAP_REG(vir_addr);

    return ret;
}

hi_s32 pdt_init_license(hi_void)
{
    hi_s32 ret;
    hi_u32 value;
    pdt_license *license = HI_NULL;

    ret = hi_drv_get_license_support(HI_LICENSE_DECODE_CAP, &value);
    if (ret != 0) {
        dprint(PRN_FATAL, "vfmw get dec cap err\n");
        return HI_FAILURE;
    }

    license = &g_vdh_license;
    switch (value) {
        case 3: /* 3 mean 8 core */
            license->core_num = 8; /* 8 core */
            break;
        case 2: /* 2 mean 4 core */
            license->core_num = 4; /* 4 core */
            break;
        case 1: /* 1 mean 2 core */
            license->core_num = 2; /* 2 core */
            break;
        default:
            license->core_num = 1;
            break;
    }

    ret = hi_drv_get_license_support(HI_LICENSE_DECODE_FORMAT, &value);
    if (ret != 0) {
        dprint(PRN_FATAL, "vfmw get dec fmt err\n");
        return HI_FAILURE;
    }

    if (value & 1) {
        license->avs3_en = 1;
    }

    if ((value >> 1) & 1) {
        license->av1_en = 1;
    }

    license->valid = 1;

    dprint(PRN_DBG, "vfmw license avs3 %d av1 %d core %d\n",
           license->avs3_en, license->av1_en, license->core_num);

    return HI_SUCCESS;
}

hi_s32 pdt_get_license(pdt_license *license)
{
    if (g_vdh_license.valid == 0) {
        dprint(PRN_ERROR, "%s not valid\n", __func__);
        return HI_FAILURE;
    }

    *license = g_vdh_license;

    return HI_SUCCESS;
}

hi_u32 g_row_map_table_y[2][4][16] = { /* 2 :a number 4 :a number 16 :a number */
    {   { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        { 4,  5, 6, 7, 0, 1, 2, 3, 12, 13, 14, 15, 8, 9, 10, 11 },
        { 8,  9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7 },
        { 12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3 }
    },
    {   { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
        { 4,  5, 6, 7, 0, 1, 2, 3, 12, 13, 14, 15, 8, 9, 10, 11 },
        { 8,  9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7 },
        { 12, 13, 14, 15, 8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3 }
    }
};

hi_u32 g_row_map_table_uv[2][4][8] = { /* 2 :a number 4 :a number 8 :a number */
    {
        { 0, 1, 2, 3, 4, 5, 6, 7 },
        { 4, 5, 6, 7, 0, 1, 2, 3 },
        { 0, 1, 2, 3, 4, 5, 6, 7 },
        { 4, 5, 6, 7, 0, 1, 2, 3 },
    },
    {
        { 0, 1, 2, 3, 4, 5, 6, 7 },
        { 4, 5, 6, 7, 0, 1, 2, 3 },
        { 0, 1, 2, 3, 4, 5, 6, 7 },
        { 4, 5, 6, 7, 0, 1, 2, 3 },
    }
};

hi_u32 pdt_get_y8b_data(pdt_yuv *yuv, hi_s32 h, hi_s32 w)
{
    hi_u32 data;
    hi_u32 stride;
    hi_u32 src_idx;
    hi_u32 dst_idx;
    hi_u32 tilex;
    hi_u32 tiley;
    hi_u8 *src = HI_NULL;
    hi_u8 *dst = HI_NULL;

    stride = yuv->image_stride << 4; /* 4 :a number */
    src = yuv->luma_vir_addr;

    if (yuv->mem_format == 0) {
        dst = src + stride / 16 * h + w; /* 16 :a number */
    } else {
        tiley = (h / 16) % 2; /* 16 :a number 2 :a number */
        tilex = (w / 64) % 4; /* 64 :a number 4 :a number */
        src_idx = h % 16; /* 16 :a number */
        dst_idx = g_row_map_table_y[tiley][tilex][src_idx];
        /* 16 :a number 64 :a number 64 :a number 64 :a number 16 :a number */
        dst = src + stride * (h / 16) + dst_idx * 64 + (w / 64) * 64 * 16;
        dst += w % 64; /* 64 :a number */
    }

    data = dst[0];

    return data;
}

hi_u32 pdt_get_c8b_data(pdt_yuv *yuv, hi_s32 h, hi_s32 w)
{
    hi_u32 data;
    hi_u32 stride;
    hi_u32 src_idx;
    hi_u32 dst_idx;
    hi_u32 tilex;
    hi_u32 tiley;
    hi_u8 *src = HI_NULL;
    hi_u8 *dst = HI_NULL;

    w = w * 2; /* 2 :a number */
    stride = yuv->image_stride << 4; /* 4 :a number */
    src = yuv->chrom_vir_addr;

    if (yuv->mem_format == 0) {
        dst = src + stride / 16 * h + w; /* 16 :a number */
    } else {
        tiley = (h / 8) % 2; /* 8 :a number 2 :a number */
        tilex = (w / 64) % 4; /* 64 :a number 4 :a number */
        src_idx = h % 8; /* 8 :a number */
        dst_idx = g_row_map_table_uv[tiley][tilex][src_idx];
        /* 2 :a number 8 :a number 64 :a number 64 :a number 64 :a number 8 :a number */
        dst = src + (stride / 2) * (h / 8) + dst_idx * 64 + (w / 64) * 64 * 8;
        dst += w % 64; /* 64 :a number */
    }

    data = (dst[0] << 8) + dst[1]; /* 8 :a number */

    return data;
}

hi_u32 pdt_get_u8b_data(pdt_yuv *yuv, hi_s32 h, hi_s32 w)
{
    return pdt_get_c8b_data(yuv, h, w) & 0xff;
}

hi_u32 pdt_get_v8b_data(pdt_yuv *yuv, hi_s32 h, hi_s32 w)
{
    return (pdt_get_c8b_data(yuv, h, w) >> 8) & 0xff; /* 8 :a number */
}

hi_u32 pdt_get_ynb_data(pdt_yuv *yuv, hi_s32 h, hi_s32 w)
{
    hi_u32 data;
    hi_u32 stride;
    hi_u8 *src;
    hi_u8 *dst;

    stride = yuv->image_stride_2bit << 5; /* 5 :a number */
    src = yuv->luma_2bit_vir_addr;
    /* 32 :anumber 4 :a number 4 :a number 4 :a number 16 :a number 64 :a number 64 :a number 4 :a number 4 :a number */
    dst = src + (stride / 32) * 4 * (h / 4) + (h % 4) * 16 + (w / 64) * 64 / 4 * 4;
    dst += (w % 64) >> 2; /* 64 :a number 2 :a number */

    data = (dst[0] >> ((w % 4) * 2)) & 0x3; /* 4 :a number 2 :a number */

    return data;
}

hi_u32 pdt_get_cnb_data(pdt_yuv *yuv, hi_s32 h, hi_s32 w)
{
    hi_u32 data;
    hi_u32 stride;
    hi_u8 *src;
    hi_u8 *dst;

    w = w * 2; /* 2 :a number */
    stride = yuv->image_stride_2bit << 5; /* 5 :a number */
    src = yuv->chrom_2bit_vir_addr;
    /* 32 :anumber 4 :a number 4 :a number 4 :a number 16 :a number 64 :a number 64 :a number 4 :a number 4 :a number */
    dst = src + (stride / 32) * 4 * (h / 4) + (h % 4) * 16 + (w / 64) * 64 / 4 * 4;
    dst += (w % 64) >> 2; /* 64 :a number 2 :a number */

    data = (dst[0] >> ((w % 4) * 2)) & 0xf; /* 4 :a number 2 :a number */

    return data;
}

hi_u32 pdt_get_unb_data(pdt_yuv *yuv, hi_s32 h, hi_s32 w)
{
    return (pdt_get_cnb_data(yuv, h, w) >> 2) & 3; /* 2 :a number 3 :a number */
}

hi_u32 pdt_get_vnb_data(pdt_yuv *yuv, hi_s32 h, hi_s32 w)
{
    return pdt_get_cnb_data(yuv, h, w) & 3; /* 3 :a number */
}

hi_u32 pdt_gety_data(pdt_yuv *yuv, hi_s32 h, hi_s32 w)
{
    hi_u32 data;

    data = pdt_get_y8b_data(yuv, h, w);
    if (yuv->bit_depth > 8) { /* 8 :a number */
        data <<= 2; /* 2 :a number */
        data += pdt_get_ynb_data(yuv, h, w);
    }

    return data;
}

hi_u32 pdt_getu_data(pdt_yuv *yuv, hi_s32 h, hi_s32 w)
{
    hi_u32 data;

    data = pdt_get_u8b_data(yuv, h, w);
    if (yuv->bit_depth > 8) { /* 8 :a number */
        data <<= 2; /* 2 :a number */
        data += pdt_get_unb_data(yuv, h, w);
    }

    return data;
}

hi_u32 pdt_getv_data(pdt_yuv *yuv, hi_s32 h, hi_s32 w)
{
    hi_u32 data;

    data = pdt_get_v8b_data(yuv, h, w);
    if (yuv->bit_depth > 8) { /* 8 :a number */
        data <<= 2; /* 2 :a number */
        data += pdt_get_vnb_data(yuv, h, w);
    }

    return data;
}

hi_s32 pdt_yuv_2d(vfmw_image *img, hi_u8 *dst)
{
    hi_s32 w, h;
    hi_u8 *ydst;
    hi_u8 *udst;
    hi_u8 *vdst;
    hi_u16 *y16_dst;
    hi_u16 *u16_dst;
    hi_u16 *v16_dst;
    hi_s32 width;
    hi_s32 height;
    hi_s32 bit_depth;
    hi_s32 ysize;
    hi_s32 pix_byte;
    pdt_yuv yuv_info;
    hi_u64 chrom_vir_addr;
    hi_u64 luma_2bit_vir_addr = 0;
    hi_u64 chrom_2bit_vir_addr = 0;

    width = img->image_width;
    height = img->image_height;
    bit_depth = img->bit_depth;
    pix_byte = (bit_depth > 8) ? 2 : 1; /* 8 :a number 2 :a number */
    ysize = width * height * pix_byte;

    ydst = dst;
    udst = ydst + ysize;
    vdst = udst + ysize / 4; /* 4 :a number */
    y16_dst = (hi_u16 *)ydst;
    u16_dst = (hi_u16 *)udst;
    v16_dst = (hi_u16 *)vdst;

    if (img->disp_info.linear_en == 1) {
        yuv_info.mem_format = 0;
    } else {
        yuv_info.mem_format = 1;
    }
    yuv_info.bit_depth = img->bit_depth;
    yuv_info.image_stride = img->disp_info.y_stride;
    yuv_info.image_stride_2bit = img->disp_info.stride_2bit;
    yuv_info.luma_vir_addr = UINT64_PTR(img->disp_info.luma_vir_addr);

    chrom_vir_addr = img->disp_info.luma_vir_addr + (img->disp_info.chrom_phy_addr -
                                                     img->disp_info.luma_phy_addr);

    if (img->bit_depth > 8 && img->disp_info.luma_vir_addr != 0) { /* 8 :a number */
        luma_2bit_vir_addr = img->disp_info.luma_vir_addr + img->disp_info.luma_phy_addr_2bit -
                             img->disp_info.luma_phy_addr;
        chrom_2bit_vir_addr = luma_2bit_vir_addr + (img->disp_info.chrom_phy_addr_2bit -
                                                    img->disp_info.luma_phy_addr_2bit);
    }

    yuv_info.chrom_vir_addr = UINT64_PTR(chrom_vir_addr);
    yuv_info.luma_2bit_vir_addr = UINT64_PTR(luma_2bit_vir_addr);
    yuv_info.chrom_2bit_vir_addr = UINT64_PTR(chrom_2bit_vir_addr);

    if (bit_depth <= 8) { /* 8 :a number */
        for (h = 0; h < height; h++) {
            for (w = 0; w < width; w++) {
                ydst[h * width + w] = pdt_gety_data(&yuv_info, h, w);
            }
        }

        for (h = 0; h < height / 2; h++) { /* 2 :a number */
            for (w = 0; w < width / 2; w++) { /* 2 :a number */
                udst[h * width / 2 + w] = pdt_getu_data(&yuv_info, h, w); /* 2 :a number */
                vdst[h * width / 2 + w] = pdt_getv_data(&yuv_info, h, w); /* 2 :a number */
            }
        }
    } else {
        for (h = 0; h < height; h++) {
            for (w = 0; w < width; w++) {
                y16_dst[h * width + w] = pdt_gety_data(&yuv_info, h, w);
            }
        }

        for (h = 0; h < height / 2; h++) { /* 2 :a number */
            for (w = 0; w < width / 2; w++) { /* 2 :a number */
                u16_dst[h * width / 2 + w] = pdt_getu_data(&yuv_info, h, w); /* 2 :a number */
                v16_dst[h * width / 2 + w] = pdt_getv_data(&yuv_info, h, w); /* 2 :a number */
            }
        }
    }

    return HI_SUCCESS;
}

hi_s32 pdt_get_dec_cap(vfmw_cap *cap)
{
    hi_s32 std_cnt = 0;
    hi_s32 max_wid = 4096;
    hi_s32 max_hig = 2304;

    VFMW_CHECK_SEC_FUNC(memset_s(cap, sizeof(vfmw_cap), 0, sizeof(vfmw_cap)));

    cap->max_chan_num = VFMW_CHAN_NUM;
    cap->max_bit_rate = 50; /* 50 :a number */
    cap->max_rame_width = max_wid;
    cap->max_frame_height = max_hig;
    cap->max_pixel_per_sec = max_wid * max_hig * 30 * 3 / 2; /* 30 :a number 3 :a number 2 :a number */

#ifdef VFMW_H264_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_H264;
#endif
#ifdef VFMW_MPEG2_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_MPEG2;
#endif
#ifdef VFMW_MPEG4_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_MPEG4;
#endif
#ifdef VFMW_AVS_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_AVS;
#endif
#ifdef VFMW_REAL8_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_REAL8;
#endif
#ifdef VFMW_REAL9_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_REAL9;
#endif
#ifdef VFMW_VC1_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_VC1;
#endif
#ifdef VFMW_DIVX3_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_DIVX3;
#endif
#ifdef VFMW_H263_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_H263;
#endif
#ifdef VFMW_SORENSON_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_SORENSON;
#endif
#ifdef VFMW_VP6_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_VP6;
    cap->supported_std[std_cnt++] = VFMW_VP6F;
    cap->supported_std[std_cnt++] = VFMW_VP6A;
#endif
#ifdef VFMW_VP8_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_VP8;
#endif
#ifdef VFMW_MVC_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_MVC;
#endif
#ifdef VFMW_HEVC_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_HEVC;
#endif
#ifdef VFMW_VP9_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_VP9;
#endif
#ifdef VFMW_AVS2_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_AVS2;
#endif
#ifdef VFMW_AV1_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_AV1;
#endif
#ifdef VFMW_AVS3_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_AVS3;
#endif
#ifdef VFMW_WMV1_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_WMV1;
#endif
#ifdef VFMW_WMV2_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_WMV2;
#endif
#ifdef VFMW_WMV3_SUPPORT
    cap->supported_std[std_cnt++] = VFMW_WMV3;
#endif

    cap->supported_std[std_cnt++] = VFMW_STD_MAX;

    return HI_SUCCESS;
}

hi_s32 pdt_get_seg_buf_size(hi_void *args)
{
    vfmw_vid_std std = VFMW_STD_START;
    hi_s32 size;

    if (args != HI_NULL) {
        std = *(vfmw_vid_std *)(args);
    }

    switch (std) {
        case VFMW_HEVC:
        case VFMW_AVS2:
        case VFMW_AVS3:
        case VFMW_AV1:
        case VFMW_VP9:
        case VFMW_H264:
            size = VFMW_SEG_BUF_MAX;
            break;
        default:
            size = VFMW_SEG_BUF_MIN;
    }

    return size;
}

hi_s32 pdt_get_attr(attr_id attr_id, hi_void *args)
{
    hi_s32 ret = HI_FAILURE;

    switch (attr_id) {
        case ATTR_GET_CAP:
            ret = pdt_get_dec_cap((vfmw_cap *)args);
            break;
        case ATTR_GET_SEG_BUF_SIZE:
            ret = pdt_get_seg_buf_size(args);
            break;
        case ATTR_GET_LICENSE:
            ret = pdt_get_license((pdt_license *)args);
            break;

        default:
            dprint(PRN_ERROR, "%s unkown attr_id %d\n", __func__, attr_id);
            break;
    }

    return ret;
}

hi_s32 pdt_set_attr(attr_id attr_id, hi_void *args)
{
    hi_s32 ret = HI_FAILURE;

    switch (attr_id) {
        case ATTR_SET_CLK:
            ret = pdt_set_clk((hi_u32 *)args);
            break;

        case ATTR_RESET_VDH:
            ret = pdt_reset_vdh();
            break;

        default:
            dprint(PRN_ERROR, "%s unkown attr_id %d\n", __func__, attr_id);
            break;
    }

    return ret;
}

hi_s32 pdt_get_dps_num(vfmw_vid_std std, hi_s32 *num)
{
    hi_s32 dps_num = 1;

    if (std == VFMW_HEVC || std == VFMW_H264 ||
        std == VFMW_AVS2 || std == VFMW_VP9 ||
        std == VFMW_AV1 || std == VFMW_MPEG2 ||
        std == VFMW_AVS3) {
        dps_num = MAX_DPS_NUM;
    }

    *num = dps_num;

    return HI_SUCCESS;
}

hi_s32 drv_vfmw_mod_init(hi_void)
{
    hi_s32 ret;

#ifndef VFMW_KO2_SUPPORT
    ret = osal_exportfunc_register(HI_ID_VFMW, "HI_VFMW", &g_vfmw_export_funcs);
#else
    ret = osal_exportfunc_register(HI_ID_VDEC, "HI_VDEC", &g_vfmw_export_funcs);
#endif
    if (ret != HI_SUCCESS) {
#ifdef MODULE
        HI_PRINT("hi_drv_module_register Failed\n");
#endif
        return ret;
    }

    vfmw_mod_init();

    pdt_set_vdh_vmin();

    pdt_init_license();

#ifdef MODULE
#ifndef VFMW_KO2_SUPPORT
    HI_PRINT("Load hi_vfmw.ko success.\t(%s)\n", VERSION_STRING);
#else
    HI_PRINT("Load hi_vfmw2.ko success.\t(%s)\n", VERSION_STRING);
#endif
#endif

    return HI_SUCCESS;
}

hi_void drv_vfmw_mod_exit(hi_void)
{
    vfmw_mod_exit();

#ifndef VFMW_KO2_SUPPORT
    osal_exportfunc_unregister(HI_ID_VFMW);
#else
    osal_exportfunc_unregister(HI_ID_VDEC);
#endif

#ifdef MODULE
#ifndef VFMW_KO2_SUPPORT
    HI_PRINT("Unload hi_vfmw.ko success.\n");
#else
    HI_PRINT("Unload hi_vfmw2.ko success.\n");
#endif
#endif

    return;
}

#ifdef MODULE
module_init(drv_vfmw_mod_init);
module_exit(drv_vfmw_mod_exit);
#else
EXPORT_SYMBOL(drv_vfmw_mod_init);
EXPORT_SYMBOL(drv_vfmw_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");


