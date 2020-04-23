/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_REG_MASTER_H__
#define __HAL_VDP_REG_MASTER_H__

#include "hi_reg_vdp.h"

hi_void vdp_master_setmstr2woutstanding(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mstr2_woutstanding);
hi_void vdp_master_setmstr2routstanding(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mstr2_routstanding);
hi_void vdp_master_setmstr1woutstanding(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mstr1_woutstanding);
hi_void vdp_master_setmstr1routstanding(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mstr1_routstanding);
hi_void vdp_master_setmstr0woutstanding(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mstr0_woutstanding);
hi_void vdp_master_setmstr0routstanding(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mstr0_routstanding);
hi_void vdp_master_setwportsel(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 wport_sel);
hi_void vdp_master_setcasmmubypass(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 cas_mmu_bypass);
hi_void vdp_master_setsplittype(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 split_type);
hi_void vdp_master_setmidenable(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mid_enable);
hi_void vdp_master_setarbmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 arb_mode);
hi_void vdp_master_setsplitmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 split_mode);
hi_void vdp_master_setrchnprio(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 id, hi_u32 prio);
hi_void vdp_master_setwchnprio(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 id, hi_u32 prio);
hi_void vdp_master_setrchnsel(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 id, hi_u32 sel);
hi_void vdp_master_setwchnsel(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 id, hi_u32 sel);
hi_void vdp_master_setbuserrorclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bus_error_clr);
hi_void vdp_master_setmst2werror(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst2_w_error);
hi_void vdp_master_setmst2rerror(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst2_r_error);
hi_void vdp_master_setmst1werror(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_w_error);
hi_void vdp_master_setmst1rerror(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_r_error);
hi_void vdp_master_setmst0werror(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_w_error);
hi_void vdp_master_setmst0rerror(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_r_error);
hi_void vdp_master_setsrc0status0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src0_status0);
hi_void vdp_master_setsrc0status1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src0_status1);
hi_void vdp_master_setsrc1status0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src1_status0);
hi_void vdp_master_setsrc1status1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src1_status1);
hi_void vdp_master_setsrc2status0(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src2_status0);
hi_void vdp_master_setsrc2status1(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 src2_status1);
hi_void vdp_master_setaxidetenable(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 axi_det_enable);
hi_void vdp_master_setaxidetclr(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 axi_det_clr);
hi_void vdp_master_setmacdebuginfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mac_debug_info);
hi_void vdp_master_setmst0rdinfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_rd_info);
hi_void vdp_master_setmst0wrinfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_wr_info);
hi_void vdp_master_setmst1rdinfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_rd_info);
hi_void vdp_master_setmst1wrinfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_wr_info);
hi_void vdp_master_setmst2rdinfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst2_rd_info);
hi_void vdp_master_setmst2wrinfo(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst2_wr_info);
hi_void vdp_master_setmst0rdmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_rd_max);
hi_void vdp_master_setmst0rdaver(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_rd_aver);
hi_void vdp_master_setmst0rdcmd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_rd_cmd);
hi_void vdp_master_setmst0wrmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_wr_max);
hi_void vdp_master_setmst0wraver(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_wr_aver);
hi_void vdp_master_setmst0wrcmd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst0_wr_cmd);
hi_void vdp_master_setmst1rdmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_rd_max);
hi_void vdp_master_setmst1rdaver(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_rd_aver);
hi_void vdp_master_setmst1rdcmd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_rd_cmd);
hi_void vdp_master_setmst1wrmax(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_wr_max);
hi_void vdp_master_setmst1wraver(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_wr_aver);
hi_void vdp_master_setmst1wrcmd(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 mst1_wr_cmd);
hi_void vdp_master_setbypassflag(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bypass_flag);
hi_void vdp_master_setawvaliddelaymode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 awvalid_delay_mode);
hi_void vdp_master_setawvaliddelaycfg(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 awvalid_delay_cfg);
hi_void vdp_master_setarvaliddelaymode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 arvalid_delay_mode);
hi_void vdp_master_setarvaliddelaycfg(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 arvalid_delay_cfg);
hi_void vdp_master_setwvaliddelaymode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 wvalid_delay_mode);
hi_void vdp_master_setwvaliddelaycfg(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 wvalid_delay_cfg);
hi_void vdp_master_setrvaliddelaymode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 rvalid_delay_mode);
hi_void vdp_master_setrvaliddelaycfg(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 rvalid_delay_cfg);
hi_void vdp_master_setbvaliddelaymode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bvalid_delay_mode);
hi_void vdp_master_setbvaliddelaycfg(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 bvalid_delay_cfg);
hi_void vdp_master_setaxipressst(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 axi_press_st);

hi_void vdp_master_setqosmap(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 id, hi_u32 chn_qos);
hi_void vdp_master_setqosmode(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 qos_mode);
hi_void vdp_master_setqosconfigure(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 qos_cfg);
hi_void vdp_master_setqosid(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 chn_id, hi_u32 mmu_id);
hi_void vdp_master_vid_setreqbalance(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 lum_req, hi_u32 chm_req);
hi_void vdp_master_gfx_setreqbalance(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 gfx_req);
hi_void vdp_master_setqos(vdp_regs_type *vdp_reg, hi_u32 offset, hi_u32 qos0, hi_u32 qos1);
#endif

