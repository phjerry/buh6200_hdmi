/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_VID_H__
#define __HAL_VDP_REG_VID_H__

#include "vdp_chip_define.h"
#include "hi_reg_vdp.h"

hi_void vdp_vid_setvmxppcmode(vdp_regs_type *vdp_reg, hi_u32 data, xdp_vmx_ppc_mode envmxppcmode);
hi_void vdp_vid_set8kcleparasrc4kenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vid_set8kclmparasrc4kenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vid_set8ksrpwrupingenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vid_set8ksrpwrdownenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vid_set4ksrpwrupingenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vid_set4ksrpwrdownenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vid_sethdrpwrdownenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vid_set8ksrtwochnwth(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 leftwth, hi_u32 rightwth);

/* v0 layer link drv */
hi_void vdp_vid_set8ksrmuxdisable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vid_set4ksrmuxdisable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vid_set4kzmemuxdisable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vid_settvdbhdrmuxenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vid_setdbhdrapprovemuxenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vid_sethdroutmaplinkctrl(vdp_regs_type *vdp_reg, xdp_hdr_outmap_link enhdroutmaplink);
hi_void vdp_vid_setdcilinkctrl(vdp_regs_type *vdp_reg, xdp_dci_link endcilink);
hi_void vdp_vid_sethdrlinkctrl(vdp_regs_type *vdp_reg, xdp_hdr_link enhdrlink);
hi_void vdp_vid_set8kzmelinkctrl(vdp_regs_type *vdp_reg, xdp_8kzme_link kzmelink);
hi_void vdp_vid_set8ksrlinkctrl(vdp_regs_type *vdp_reg, xdp_8ksr_link ksrlink);
hi_void vdp_vid_setacmlinkctrl(vdp_regs_type *vdp_reg, xdp_acm_link enacmlink);
hi_void vdp_vid_setdbhdrmuxenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);

/* vid ctrl */
hi_void vdp_vid_setlayerenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vid_setlayergalpha(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 alpha0);
hi_void vdp_vid_setnosecflag(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vid_setregup(vdp_regs_type *vdp_reg, hi_u32 data);

/* lbox */
hi_void vdp_vid_setvideopos(vdp_regs_type *vdp_reg, hi_u32 data, vdp_rect strect);
hi_void vdp_vid_setdisppos(vdp_regs_type *vdp_reg, hi_u32 data, vdp_rect strect);
hi_void vdp_vid_setlayerbkg(vdp_regs_type *vdp_reg, hi_u32 data, vdp_bkg stbkg);
hi_void vdp_vid_setlboxmutebkg(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 mute_en, vdp_bkg stmutebkg);

hi_void vdp_vid_setupdmode(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 mode);

hi_void vdp_vid_setsttregup(vdp_regs_type *vdp_reg, hi_u32 regup);
hi_void vdp_vid_setsttupdmode(vdp_regs_type *vdp_reg, hi_u32 mode);
hi_void vdp_vid_setsttupfieldmode(vdp_regs_type *vdp_reg, hi_u32 fieldmode);
hi_void vdp_vid_setstten(vdp_regs_type *vdp_reg, hi_u32 stten);
hi_void vdp_vid_setsttaddrlow(vdp_regs_type *vdp_reg, hi_u32 addr);
hi_void vdp_vid_setsttaddrhigh(vdp_regs_type *vdp_reg, hi_u32 addr);
hi_void vdp_vid_setsttmmubypass(vdp_regs_type *vdp_reg, hi_u32 mmubypass);
hi_void vdp_vid_setlumhistenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);

hi_void vdp_vp_updatelayerenable(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void vdp_vp_setvideopos(vdp_regs_type *vdp_reg, hi_u32 data, vdp_rect strect);
hi_void vdp_vp_setdisppos(vdp_regs_type *vdp_reg, hi_u32 data, vdp_rect strect);
hi_void vdp_vp_setinreso(vdp_regs_type *vdp_reg, hi_u32 data, vdp_rect strect);
hi_void vdp_vp_setregup(vdp_regs_type *vdp_reg, hi_u32 data);
hi_void vdp_vp_setlayergalpha(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 alpha);
hi_void vdp_vp_setlayerbkg(vdp_regs_type *vdp_reg, hi_u32 data, vdp_bkg stbkg);

hi_void vdp_vp_setthreedimdofenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);
hi_void vdp_vp_setthreedimdofstep(vdp_regs_type *vdp_reg, hi_u32 data, hi_s32 lstep, hi_s32 rstep);
hi_void vdp_vp_setthreedimdofbkg(vdp_regs_type *vdp_reg, hi_u32 data, vdp_bkg stbkg);
hi_void vdp_vid_setvidpressctrlmindrdynum(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 mindrdynum);
hi_void vdp_vid_setvidpressctrlmindrdynumscope(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 mindrdynumscop);
hi_void vdp_vid_setvidpressctrlminnodrdynum(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 minnodrdynum);
hi_void vdp_vid_setvidpressctrlminnodrdynumscope(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 minnodrdynumscop);
hi_void vdp_vid_setvidpressctrlenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 enable);

hi_bool vdp_vid_get_wager_mark_status(vdp_regs_type *vdp_reg, hi_u32 data);

#endif
