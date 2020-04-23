/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: functions define
* Author: vdp
* Create: 2019-06-28
 */

#ifndef __DRV_WIN_H__
#define __DRV_WIN_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

hi_s32 drv_win_register(hi_void);
hi_void drv_win_un_register(hi_void);
hi_s32 drv_win_open(struct inode *finode, struct file *ffile);
hi_s32 drv_win_close(struct inode *finode, struct file *ffile);
hi_slong drv_win_ioctl(struct file *file, hi_u32 cmd, hi_void *arg);
hi_s32 drv_win_init(hi_void);
hi_void drv_win_deinit(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*  __DRV_WIN_H__  */
