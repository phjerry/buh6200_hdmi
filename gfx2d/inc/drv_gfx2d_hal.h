/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: gfx2d hal layer
 * Author: sdk
 * Create: 2019-05-13
 */

#ifndef _GFX2D_HAL_H_
#define _GFX2D_HAL_H_

#include "hi_osal.h"
#include "drv_gfx2d_struct.h"
#include "drv_gfx2d_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* ************************** Structure Definition ******************************************** */
typedef enum {
    GFX2D_HAL_DEV_TYPE_HWC = 0x0,
} GFX2D_HAL_DEV_TYPE_E;

/* ****************************** API declaration ********************************************* */
/* submitting nodes to hardware */ /* CNcomment: 向硬件提交节点 */
typedef hi_s32 (*GFX2D_HAL_SubNode_FN_PTR)(drv_gfx2d_dev_id dev_id, hi_void *pNode);

/* Link two nodes into linked lists according to hardware requirements */
/* CNcomment: 将两个节点按照硬件要求链接成链表 */
typedef hi_s32 (*GFX2D_HAL_LinkNode_FN_PTR)(hi_void *pCurNode, hi_void *pNextNode);

/* set node id */ /* CNcomment: 设置节点ID */
typedef hi_void (*GFX2D_HAL_SetNodeID_FN_PTR)(hi_void *pNode, hi_u32 u32NodeId);

/* release node */ /* CNcomment: 释放节点 */
typedef hi_void (*GFX2D_HAL_FreeNode_FN_PTR)(hi_void *pNode);

/* interrupt handle */ /* CNcomment: 中断处理 */
typedef hi_void (*GFX2D_HAL_NodeIsr_FN_PTR)(hi_void *pNode);

/* interrupt handle */ /* CNcomment: 中断处理 */
typedef hi_void (*GFX2D_HAL_AllNodeIsr_FN_PTR)(hi_void);

/* hardware node operating function set */ /* CNcomment: 硬件节点操作函数集 */
typedef struct {
    GFX2D_HAL_SubNode_FN_PTR pfnSubNode;
    GFX2D_HAL_LinkNode_FN_PTR pfnLinkNode;
    GFX2D_HAL_SetNodeID_FN_PTR pfnSetNodeID;
    GFX2D_HAL_FreeNode_FN_PTR pfnFreeNode;
    GFX2D_HAL_NodeIsr_FN_PTR pfnNodeIsr;
    GFX2D_HAL_AllNodeIsr_FN_PTR pfnAllNodeIsr;
} GFX2D_HAL_NODE_OPS_S;

/* ****************************** API declaration ********************************************* */

hi_s32 GFX2D_HAL_Init(hi_void);

hi_void GFX2D_HAL_Deinit(hi_void);

hi_s32 GFX2D_HAL_Open(hi_void);

hi_void GFX2D_HAL_Close(hi_void);

hi_s32 GFX2D_HAL_Compose(const drv_gfx2d_dev_id dev_id, const drv_gfx2d_compose_list *compose_list,
                         const drv_gfx2d_compose_surface *pstDstSurface, hi_void **ppNode,
                         GFX2D_HAL_DEV_TYPE_E *penNodeType);

#ifdef CONFIG_GFX_PROC_SUPPORT
hi_void GFX2D_HAL_ReadProc(hi_void *p, hi_void *v);
#endif

hi_void GFX2D_HAL_GetNodeOps(const GFX2D_HAL_DEV_TYPE_E enDevType, GFX2D_HAL_NODE_OPS_S **ppstNodeOps);

hi_s32 GFX2D_HAL_GetIntStatus(const GFX2D_HAL_DEV_TYPE_E enDevType);

hi_u32 GFX2D_HAL_GetIsrNum(const drv_gfx2d_dev_id dev_id, const GFX2D_HAL_DEV_TYPE_E enDevType);

hi_u32 GFX2D_HAL_GetBaseAddr(const drv_gfx2d_dev_id dev_id, const GFX2D_HAL_DEV_TYPE_E enDevType);

#ifdef GFX2D_ALPHADETECT_SUPPORT
hi_void GFX2D_HAL_GetTransparent(drv_gfx2d_compose_ext_info *ext_info);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _GFX2D_HAL_H_ */
