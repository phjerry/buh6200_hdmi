/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_IP_DISPCHN_H__
#define __HAL_VDP_IP_DISPCHN_H__

#include "vdp_chip_define.h"
#include "hi_drv_disp.h"

typedef enum {
    /* CV300 CBM0 */
    VDP_DISPCHN_CBM0_VP0 = 0x1,
    VDP_DISPCHN_CBM0_GP0 = 0x2,
    VDP_DISPCHN_CBM0_GFX2 = 0x3,

    /* CV300 CBM1 */
    VDP_DISPCHN_CBM1_VID3 = 0x1,
    VDP_DISPCHN_CBM1_GFX3 = 0x2,
    VDP_DISPCHN_CBM1_VID2 = 0x3,
    VDP_DISPCHN_CBM1_GFX2 = 0x4,

    /* CV300 MIXV0 */
    VDP_DISPCHN_MIX0_VID0 = 0x1,
    VDP_DISPCHN_MIX0_VID1 = 0x2,
    VDP_DISPCHN_MIX0_VID2 = 0x3,

    /* CV300 MIXG0 */
    VDP_DISPCHN_MIX0_GFX0 = 0x1,
    VDP_DISPCHN_MIX0_GFX1 = 0x2,
    VDP_DISPCHN_MIX0_GFX2 = 0x3,

    VDP_DISPCHN_CBM_BUTT
} vdp_dispchn_cbm_layer;

typedef enum {
    /* zreo means no layer driver */
    VDP_DISPCHN_PRIO_MIXV0_VID0 = 0x1,
    VDP_DISPCHN_PRIO_MIXV0_VID1 = 0x2,
    VDP_DISPCHN_PRIO_MIXV0_VID2 = 0x3,

    VDP_DISPCHN_PRIO_MIXG0_GFX0 = 0x1,
    VDP_DISPCHN_PRIO_MIXG0_GFX1 = 0x2,
    VDP_DISPCHN_PRIO_MIXG0_GFX2 = 0x3,

    /* DHD0 with VP/GP */
    VDP_DISPCHN_PRIO_CBM0_VP0 = 0x1,
    VDP_DISPCHN_PRIO_CBM0_GP0 = 0x2,
    VDP_DISPCHN_PRIO_CBM0_GFX2 = 0x3,

    /* DHD1 */
    VDP_DISPCHN_PRIO_CBM1_VID3 = 0x1,
    VDP_DISPCHN_PRIO_CBM1_GFX3 = 0x2,
    VDP_DISPCHN_PRIO_CBM1_VID2 = 0x3,
    VDP_DISPCHN_PRIO_CBM1_GFX2 = 0x4,
} vdp_dispchn_prio;

typedef enum {
    VDP_DISPCHN_INT_ON = 0,
    VDP_DISPCHN_INT_OFF,
    VDP_DISPCHN_INT_CLEAN,
    VDP_DISPCHN_INT_BUTT
} vdp_dispchn_voint_status;

typedef enum {
    VDP_DISPCHN_MULTICHN_1_PIXEL = 0,
    VDP_DISPCHN_MULTICHN_2_PIXEL,
    VDP_DISPCHN_MULTICHN_4_PIXEL,
    VDP_DISPCHN_MULTICHN_8_PIXEL,
    VDP_DISPCHN_MULTICHN_BUTT,
} vdp_dispchn_multichn;

typedef struct {
    hi_drv_disp_fmt disp_fmt;
    vdp_disp_mode disp_mode;
    vdp_sync_info timing;
    vdp_intf_split_mode split_mode; /* CNcomment:Ò»ÅÄ¼¸ÏñËØ */
} vdp_dispchn_info;

hi_void vdp_ip_dispchn_init(hi_void);
hi_void vdp_ip_dispchn_set_timming(vdp_dispchn_chn chn_id, vdp_dispchn_info *dispchn_info);
hi_void vdp_ip_dispchn_set_vtthd(vdp_dispchn_chn chn_id, hi_u32 num, hi_u32 thd, hi_u32 mode);
hi_void vdp_ip_dispchn_set_intf(vdp_dispchn_chn chn, vdp_disp_intf intf);
hi_void vdp_ip_dispchn_sync_inv(vdp_disp_intf intf, vdp_disp_syncinv *syncinv);
hi_void vdp_ip_dispchn_attach_intf(vdp_dispchn_chn chn_id, vdp_disp_intf intf);
hi_void vdp_ip_dispchn_date_coef(vdp_disp_intf intf, hi_drv_disp_fmt disp_fmt);
hi_u32 vdp_ip_dispchn_get_int_mask_state(hi_void);
hi_u32 vdp_ip_dispchn_get_int_state(hi_void);

hi_void vdp_ip_dispchn_set_mask(vdp_dispchn_voint_status status, vdp_intmask mask);

hi_void vdp_ip_dispchn_set_cbm_bkg(vdp_dispchn_cbm_mix chn_id, vdp_bkg *vdp_bkg);
hi_void vdp_ip_dispchn_set_enable(vdp_dispchn_chn chn_id, hi_bool enable);
hi_bool vdp_ip_dispchn_get_enable(vdp_dispchn_chn chn_id);
hi_void vdp_ip_dispchn_set_colobar(vdp_dispchn_chn chn_id, hi_bool enable);

hi_void vdp_ip_dispchn_regup(vdp_dispchn_chn chn);
hi_void vdp_ip_dispchn_dhd0_mask_enalbe(hi_bool enable);
hi_void vdp_ip_dispchn_get_state(vdp_dispchn_chn chn_id, hi_bool *btm, hi_u32 *vcnt, hi_u32 *int_cnt);
hi_void vdp_ip_dispchn_set_cbm_mix(hi_u32 mixv_layer, hi_u32 vid_layer, hi_u32 pro);
hi_void vdp_ip_dispchn_get_gfx_state(hi_u32 *total_pixel, hi_u32 *zero_pixel);
hi_void vdp_ip_dispchn_get_checksum(vdp_dispchn_chn chn, hi_u32 *r, hi_u32 *g, hi_u32 *b);

#endif

