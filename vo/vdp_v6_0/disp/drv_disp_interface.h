#ifndef __DRV_DISP_INTERFACE_H__
#define __DRV_DISP_INTERFACE_H__
#include "hi_drv_disp.h"
#include "drv_disp_ext.h"

#include "hi_drv_module.h"
#include "hi_drv_proc.h"
#include "hi_drv_sys.h"
#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "hi_drv_osal.h"
#include "hi_drv_mem.h"

#include "drv_disp.h"
#include "drv_disp_ioctl.h"
#include "drv_display.h"

#include "drv_win_ext.h"
#include "drv_disp_isr.h"

#include "hi_drv_stat.h"
#include "hal_disp_intf.h"
#define HI_DISP_VDAC_MAX_NUMBER 4
#define HI_DISP_VDAC_INVALID_ID 0xff

typedef enum {
    DRV_DISP_KEEP_SAME_ENABLE_STATUS    = 0,
    DRV_DISP_SET_SAME_DISABLE_STATUS     = 1,
} drv_disp_intf_enable_status;

hi_s32 drv_disp_intf_init(hi_void);
hi_s32 drv_disp_intf_deinit(hi_void);

hi_s32 drv_disp_intf_attach(hi_drv_display disp, hi_drv_disp_intf intf);
hi_s32 drv_disp_intf_detach(hi_drv_display disp, hi_drv_disp_intf intf);

hi_s32 drv_disp_intf_check_output_validate(hi_drv_display disp,
                                           hi_drv_disp_intf_info *intf_info,
                                           hi_drv_disp_format_param *format_param);
hi_s32 drv_disp_intf_output_prepare(hi_drv_display disp,
                                    hi_drv_disp_intf_info *intf_info,
                                    hi_drv_disp_format_param *format_param);
hi_s32 drv_disp_intf_output_config(hi_drv_display disp,
                                   hi_drv_disp_intf_info *intf_info,
                                   hi_drv_disp_format_param *format_param);
hi_s32 drv_disp_intf_output_enable(hi_drv_display disp,
                                   hi_drv_disp_format_param *format_param,
                                   hi_bool enable);

hi_bool drv_disp_intf_check_existed_status(hi_drv_display disp, hi_drv_disp_format_param *format_param);
hi_bool drv_disp_intf_check_same_param(hi_drv_display disp, hi_drv_disp_format_param *format_param);
hi_s32 drv_disp_intf_set_attr(hi_drv_display disp,
                              hi_drv_disp_format_param *format_param);
hi_s32 drv_disp_intf_get_attr(hi_drv_display disp,
                              hi_drv_disp_format_param *format_param);

/*all*/
hi_s32 drv_disp_intf_suspend(hi_drv_display disp);
hi_s32 drv_disp_intf_resume(hi_drv_display disp);
hi_s32 drv_disp_intf_get_status(hi_drv_display disp, disp_get_intf *intf);
hi_s32 drv_disp_intf_get_enable(hi_drv_display disp, hi_drv_disp_intf drv_intf, hi_bool *enable);
hi_s32 drv_disp_set_intf_format(hi_drv_display disp, disp_timing_info *timing_info);
hi_s32 drv_disp_intf_set_enable_status(hi_drv_display disp,
                                       hi_drv_disp_format_param *format_param);

hi_s32 drv_disp_intf_get_proc_info(struct seq_file *p, hi_drv_display disp);
hi_s32 drv_disp_intf_get_capability(hi_drv_display disp, hi_drv_disp_intf drv_intf, hi_bool *is_support);

#endif
