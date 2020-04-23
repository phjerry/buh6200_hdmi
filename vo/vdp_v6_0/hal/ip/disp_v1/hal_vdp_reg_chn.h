/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_CHN_H__
#define __HAL_VDP_REG_CHN_H__

#include "hi_reg_vdp.h"

hi_void vdp_disp_setdneedmode(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 mode);
hi_void vdp_disp_setintfvsyncblsel(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 intfvsyncblsel);
hi_void vdp_disp_setintfvsyncblinv(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 intfvsyncblinv);
hi_void vdp_disp_setintfllrrmode(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 llrmode);
hi_void vdp_disp_setintfafifopreufthd(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 afifopreufthd);
hi_void vdp_disp_setintfpausemode(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 pausemode);
hi_void vdp_disp_setintfvdacdethigh(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vdacdethigh);
hi_void vdp_disp_setintfdetline(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 detline);
hi_void vdp_disp_setintfdetpixelstat(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 detpixelstat);
hi_void vdp_disp_setintfdetpixelwid(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 detpixelwid);
hi_void vdp_disp_setintfvdacdeten(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vdacdeten);
hi_void vdp_disp_setvbidata(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vbiaddr, hi_u32 vbidata);
hi_void vdp_disp_setvbienable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vbienable);
hi_void vdp_disp_setufoffineen(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 ufofflineen);
hi_void vdp_disp_setdispmode(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 dispmode);
hi_void vdp_disp_setsplitmode(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 splitmode);

hi_void vdp_disp_sethdmimode(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 hdmi_md);
hi_void vdp_disp_setfpintersync(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 fpintersync);
hi_void vdp_disp_setfpgalmten(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 fpgalmten);
hi_void vdp_disp_setfpgalmtwidth(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 fpgalmtwidth);
hi_void vdp_disp_setmirroren(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 mirroren);
hi_void vdp_disp_sethdmi420enable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 enable);
hi_void vdp_disp_sethdmi420csel(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 csel);
hi_void vdp_disp_setregup(vdp_regs_type *vdp_reg, hi_u32 chn_id);
hi_void vdp_disp_setintfenable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 btrue);
hi_void vdp_disp_setintmask(vdp_regs_type *vdp_reg, hi_u32 masktypeen);
hi_void vdp_disp_setintdisable(vdp_regs_type *vdp_reg, hi_u32 masktypeen);
hi_void vdp_disp_bfmclksel(vdp_regs_type *vdp_reg, hi_u32 num);
hi_void vdp_disp_openintf(vdp_regs_type *vdp_reg, hi_u32 chn_id, vdp_sync_info stsyncinfo);
hi_void vdp_disp_setvsync(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 vfb, hi_u32 vbb, hi_u32 vact);
hi_void vdp_disp_setvsyncplus(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 bvfb, hi_u32 bvbb, hi_u32 vact);
hi_void vdp_disp_sethsync(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 hfb, hi_u32 hbb, hi_u32 hact);
hi_void vdp_disp_setpluswidth(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 hpw, hi_u32 vpw);
hi_void vdp_disp_setplusphase(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 ihs, hi_u32 ivs, hi_u32 idv);
hi_void vdp_disp_setsyncinv(vdp_regs_type *vdp_reg, hi_u32 chn_id, vdp_disp_syncinv *eninv);
hi_void vdp_disp_setdateclip0len(vdp_regs_type *vdp_reg, hi_bool enable);
hi_void vdp_disp_setdatecoeff0(vdp_regs_type *vdp_reg, hi_u32 date);
hi_void vdp_disp_setdatecoeff21(vdp_regs_type *vdp_reg, vdp_disp_intf intf);
hi_void vdp_disp_setintfmuxdac(vdp_regs_type *vdp_reg, vdp_disp_intf intf);
hi_void vdp_disp_setdacenable(vdp_regs_type *vdp_reg, hi_bool enable);
hi_void vdp_disp_setdac0ctrl(vdp_regs_type *vdp_reg, hi_bool enable, hi_u32 dacgc);
hi_void vdp_disp_setdac1ctrl(vdp_regs_type *vdp_reg, hi_bool enable, hi_u32 dacgc);
hi_void vdp_disp_setdac2ctrl(vdp_regs_type *vdp_reg, hi_bool enable, hi_u32 dacgc);
hi_void vdp_disp_setdac3ctrl(vdp_regs_type *vdp_reg, hi_bool enable, hi_u32 dacgc);
hi_void vdp_disp_setintfmuxsel(vdp_regs_type *vdp_reg, hi_u32 chn_id, vdp_disp_intf enintf);
hi_void vdp_disp_setchksumen(vdp_regs_type *vdp_reg, hi_u32 chksumen);
hi_u32 vdp_disp_getintsta(vdp_regs_type *vdp_reg, hi_u32 intmask);
hi_void vdp_disp_clearintsta(vdp_regs_type *vdp_reg, hi_u32 intmask);
hi_void vdp_disp_setvtthdmode(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 uthdnum, hi_u32 mode);
hi_void vdp_disp_setvtthd(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 uthdnum, hi_u32 vtthd);
hi_void vdp_disp_setintfhdmiinv(vdp_regs_type *vdp_reg, hi_u32 chn_id, vdp_disp_syncinv eninv);
hi_void vdp_disp_setdhdinv(vdp_regs_type *vdp_reg, hi_u32 chn_id, vdp_disp_syncinv eninv);

hi_void vdp_disp_sethbisyncenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 hbisyncenable);
hi_void vdp_disp_sethbihcenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 hbihcenable);
hi_void vdp_disp_sethbiosdenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 hbiosdenable);
hi_void vdp_disp_setvbienable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vbienable);
hi_void vdp_disp_setvbidata(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vbiaddr, hi_u32 vbidata);
hi_void vdp_disp_sethbihcthd(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 hbihcthd);
hi_void vdp_disp_sethbiosdthd(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 hbiosdthd);
hi_void vdp_disp_settwodiv4pen(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 twodiv4pen);

/* cbar_begin */
hi_void vdp_disp_setcbarenable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 btrue);
hi_void vdp_disp_setcbarsel(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 cbar_sel);
hi_void vdp_disp_setcbarmode(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 _cbar_mode);
hi_void vdp_disp_setintfclkmux(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 clkmux);
hi_void vdp_disp_setrgbfixmux(vdp_regs_type *vdp_reg, hi_u32 rgb_fix_mux);
hi_void vdp_disp_setfixr(vdp_regs_type *vdp_reg, hi_u32 fix_r);
hi_void vdp_disp_setfixg(vdp_regs_type *vdp_reg, hi_u32 fix_g);
hi_void vdp_disp_setfixb(vdp_regs_type *vdp_reg, hi_u32 fix_b);

/* ipu_begin */
hi_void vdp_disp_setclipcoef(vdp_regs_type *vdp_reg, hi_u32 data, vdp_disp_clip stclipdata);
hi_void vdp_disp_setgammainkenable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 bgammainken);
hi_void vdp_disp_setgammainkselenable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 bgammainksel);
hi_void vdp_disp_setgammainkcrossenable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 bgammainkcrossen);
hi_void vdp_disp_setgammainkfmt(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 bgammainkfmt);
hi_void vdp_disp_setgammainkcolormode(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 gammacolormode);
hi_void vdp_disp_setgammainkpos(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 gammaxpos, hi_u32 gammaypos);
hi_void vdp_disp_setchnenable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 en);
hi_void vdp_disp_setmirrorenable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 mirren);
hi_void vdp_disp_setprotointerenable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 enable);
hi_void vdp_disp_setcoefreadenable(vdp_regs_type *vdp_reg, hi_u32 id, hi_u32 para);
hi_void vdp_disp_setcoefreaddisable(vdp_regs_type *vdp_reg, hi_u32 id, hi_u32 para);


hi_void vdp_disp_setregupsameparaload(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 mode);

hi_void vdp_disp_setreqpos(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 startpos);
hi_void vdp_disp_setstartpos(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 startpos);
hi_void vdp_disp_setfistartpos(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 startpos);
hi_void vdp_disp_settimingstartpos(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 startpos);
hi_void vdp_disp_setreqstartpos(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 startpos);
hi_void vdp_disp_setparaloadpos(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 startpos);
hi_void vdp_disp_setvsynctemode(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vsynctemode);
hi_void vdp_disp_settconprethd(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 prethd);
hi_void vdp_disp_setintfmultichnmode(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 multichn_mode);
hi_void vdp_disp_setintfmirroren(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 mirroren);

hi_void vdp_disp_setchnenable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 en);
hi_void vdp_disp_setmirrorenable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 mirren);
hi_void vdp_disp_setupdmode(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 data);
hi_void vdp_disp_setmplusselect(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 data);
hi_void vdp_disp_setp2ienable(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 data);
hi_void vdp_disp_setsttmmubypass(vdp_regs_type *vdp_reg, hi_u32 sttbypass);
hi_void vdp_disp_setablsttaddrhigh(vdp_regs_type *vdp_reg, hi_u32 addr);
hi_void vdp_disp_setablsttaddrlow(vdp_regs_type *vdp_reg, hi_u32 addr);
hi_void vdp_disp_setreqstartlinepos(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 reqstartlinepos);
hi_void vdp_disp_setreqstartpos1(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 reqstartpos1);
hi_void vdp_disp_setprestartlinepos(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 prestartlinepos);
hi_void vdp_disp_setprestartpos(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 prestartpos);
hi_void vdp_disp_setrmbusy(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 rmbusy);
hi_void vdp_disp_setparabusymode(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_u32 parabusymode);

hi_void vdp_disp_setclipenable(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 clipen);
hi_void vdp_disp_setintfdvmux(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 dvmux);
hi_void vdp_disp_setintfnoactivepos(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 intfnoactivepos);
hi_void vdp_disp_setintfrgbfixmux(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 rgbfixmux);
hi_void vdp_disp_setintffixr(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 fixr);
hi_void vdp_disp_setintffixg(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 fixg);
hi_void vdp_disp_setintffixb(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 fixb);
hi_void vdp_disp_setintfvimeasureen(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vimeasureen);
hi_void vdp_disp_setintflockcnten(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 lockcnten);
hi_void vdp_disp_setintfvdpmeasureen(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 vdpmeasureen);

hi_u32 vdp_disp_getdhdstate(vdp_regs_type *vdp_reg, hi_u32 data, hi_u32 *pu32vback_blank, hi_u32 *pu32count_vcnt);
hi_bool vdp_disp_getintfenable(vdp_regs_type *vdp_reg, hi_u32 chn_id);
hi_void vdp_disp_get_state(vdp_regs_type *vdp_reg, hi_u32 chn_id, hi_bool *btm, hi_u32 *vcnt, hi_u32 *int_cnt);
hi_void vdp_disp_get_gfx_state(vdp_regs_type *vdp_reg, hi_u32 *total_pixel, hi_u32 *zero_pixel);
#endif
