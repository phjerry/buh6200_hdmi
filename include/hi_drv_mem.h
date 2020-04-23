/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : hi_drv_mem.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2006/05/17
  Description   :
  History       :
  1.Date        : 2006/05/17
    Author      :
    Modification: Created file

******************************************************************************/
#ifndef __HI_DRV_MEM_H__
#define __HI_DRV_MEM_H__

#include "hi_type.h"
#include "hi_debug.h"

#ifdef __KERNEL__

#include <linux/slab.h>
#include <linux/vmalloc.h>

#define HI_KZALLOC(module_id, size, flags)      kzalloc(size, flags)
#define HI_KMALLOC(module_id, size, flags)      kmalloc(size, flags)
#define HI_KFREE(module_id, addr)               kfree(addr)
#define HI_VMALLOC(module_id, size)             vmalloc(size)
#define HI_VFREE(module_id, addr)               vfree(addr)

hi_void*    hi_kzalloc(hi_u32 module_id, hi_u32 size, hi_s32 flags);
hi_void*    hi_kmalloc(hi_u32 module_id, hi_u32 size, hi_s32 flags);
hi_void     hi_kfree(hi_u32 module_id, hi_void *ptr);

hi_void*    hi_vmalloc(hi_u32 module_id, hi_u32 size);
hi_void     hi_vfree(hi_u32 module_id, hi_void *ptr);
#endif

#endif /* End of #ifndef __HI_DRV_MEM_H__ */

