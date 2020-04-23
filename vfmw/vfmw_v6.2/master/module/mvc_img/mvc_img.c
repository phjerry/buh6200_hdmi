/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "mvc_img.h"
#include "vfmw_osal.h"
#include "dbg.h"

static hi_void mvc_img_handle_match_image(vfmw_image *img0, vfmw_image *img1)
{
    img0->disp_info.luma_phy_addr_1 = img1->disp_info.luma_phy_addr;
    img0->disp_info.chrom_phy_addr_1 = img1->disp_info.chrom_phy_addr;
    img0->disp_info.frm_dma_buf_1 = img1->disp_info.frm_dma_buf;
    img0->image_id_1 = img1->image_id;
    img0->frm_pack_type = FRAME_PACKING_TYPE_TIME_INTERLACED;
    img0->last_frame = img1->last_frame;
}

hi_void mvc_img_set_view0_last_frame(vfmw_image *img0)
{
    img0->disp_info.chrom_phy_addr_1 = img0->disp_info.chrom_phy_addr;
    img0->disp_info.luma_phy_addr_1 = img0->disp_info.luma_phy_addr;
    img0->disp_info.frm_dma_buf_1 = img0->disp_info.frm_dma_buf;
    img0->frm_pack_type = FRAME_PACKING_TYPE_TIME_INTERLACED;
    img0->image_id_1 = -1;
}

hi_s32 mvc_img_patch_two_view(vfmw_image *img0, vfmw_image *img1, mvc_img_state *img1_state)
{
    if (img0->view_id > img1->view_id) {
        dprint(PRN_ERROR, "error: view0 id(%d | imgid %d) > view1 id(%d | imgid %d).\n",
               img0->view_id, img0->image_id, img1->view_id, img1->image_id);
        *img1_state = MVC_IMG_REQUE;
        return HI_FAILURE;
    }

    if (img0->view_id == img1->view_id && img1->last_frame != 1) {
        dprint(PRN_ERROR, "error: continuous view id(%d), img0 id(%d) img1 id(%d).\n",
               img0->view_id, img0->image_id, img1->image_id);
        if (img1->view_id == 0) {
            *img1_state = MVC_IMG_REQUE;
        } else {
            *img1_state = MVC_IMG_DISCARD;
        }
        return HI_FAILURE;
    }

    /* here, view1 id is larger than view0 id as expected */
    mvc_img_handle_match_image(img0, img1);

    return HI_SUCCESS;
}


