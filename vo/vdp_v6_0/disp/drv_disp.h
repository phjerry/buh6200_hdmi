/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
* Description: function define
* Author: vdp
* Create: 2019-04-12
 */

#ifndef __DRV_DISP_H__
#define __DRV_DISP_H__

#include "hi_drv_proc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


typedef struct hi_disp_proc_fn_
{
    hi_proc_read_fn  rdproc;
    hi_proc_write_fn wtproc;
}disp_proc_fn;

typedef struct hi_win_proc_fn_{
    hi_proc_read_fn  rd_proc;
    hi_proc_write_fn wt_proc;
    hi_proc_read_fn  win_rd_proc;
    hi_proc_write_fn win_wt_proc;
}win_proc_fn;


typedef struct hi_drv_disp_state_
{
    hi_bool disp_open[HI_DRV_DISPLAY_BUTT];
    hi_handle hcast_handle[HI_DRV_DISPLAY_BUTT];
    hi_void* h_snapshot[HI_DRV_DISPLAY_BUTT];
    hi_handle hvbi[HI_DRV_DISPLAY_BUTT];
}drv_disp_state;

typedef struct hi_drv_disp_global_state_
{
    hi_u32 disp_open_num[HI_DRV_DISPLAY_BUTT];
}drv_disp_global_state;

typedef struct hi_drv_disp_fmt_string_
{
    const hi_char* pfmt_string;
    hi_drv_disp_fmt fmt;
} drv_disp_fmt_string;

typedef struct {
    const hi_char* pbit_width_string;
    hi_drv_pixel_bitwidth bit_width;
} drv_disp_bit_width_string;

typedef struct {
    const hi_char* ppixel_format_string;
    hi_drv_disp_pix_format pixel_format;
} drv_disp_pixel_format_string;

hi_s32 drv_disp_register(hi_void);

hi_s32  disp_file_open(struct inode *finode, struct file  *ffile);
hi_s32  disp_file_close(struct inode *finode, struct file  *ffile);
hi_slong  drv_disp_ioctl( struct file *file, hi_u32 cmd, hi_void *arg);
hi_slong  drv_disp_compat_ioctl( struct file *file, hi_u32 cmd, hi_void *arg);
hi_void drv_disp_un_register(hi_void);
hi_s32 drv_disp_register(hi_void);

hi_s32 drv_disp_start_up(hi_void);
hi_s32 drv_disp_shut_down(hi_void);

hi_s32 drv_disp_resume(hi_void);
hi_s32 drv_disp_suspend(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*  __DRV_DISP_H__  */

