/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: proc manage
 * Author: sdk
 * Create: 2019-03-18
 */
#ifdef CONFIG_GFX_PROC_SUPPORT
#include "tde_proc.h"
#include "tde_debug.h"
#include <linux/hisilicon/securec.h>

/* ************************** Structure Definition ***************************************** */
typedef struct {
    hi_u32 u32CurNode;
    tde_hardware_node stTdeHwNode[TDE_MAX_PROC_NUM];
    hi_bool bProcEnable;
} TDE_PROCINFO_S;

TDE_PROCINFO_S *g_pstTdeProcInfo = NULL;

/* ****************************** API release ********************************************** */
hi_void tde_proc_record_node(tde_hardware_node *hardware_node)
{
    if ((!g_pstTdeProcInfo->bProcEnable) || (hardware_node == NULL)) {
        return;
    }

    GRAPHIC_CHECK_ARRAY_OVER_RETURN_NOVALUE(g_pstTdeProcInfo->u32CurNode, TDE_MAX_PROC_NUM);

    if (memcpy_s(&g_pstTdeProcInfo->stTdeHwNode[g_pstTdeProcInfo->u32CurNode], sizeof(tde_hardware_node), hardware_node,
                 sizeof(tde_hardware_node)) != EOK) {
        return;
    }

    g_pstTdeProcInfo->u32CurNode++;
    g_pstTdeProcInfo->u32CurNode = (g_pstTdeProcInfo->u32CurNode) % TDE_MAX_PROC_NUM;

    return;
}

hi_s32 tde_read_proc(hi_void *p, hi_void *v)
{
    hi_u32 j = 0;
    tde_hardware_node *current_node = NULL;
    tde_hardware_node *hw_node = NULL;

    if (p == NULL) {
        return 0;
    }

    if (v == NULL) {
        return 0;
    }

    hw_node = g_pstTdeProcInfo->stTdeHwNode;

    for (j = 0; (j < g_pstTdeProcInfo->u32CurNode) && (j < TDE_MAX_PROC_NUM); j++) {
        current_node = (tde_hardware_node *)&hw_node[j];
        tde_hal_node_print_info(p, current_node);
    }

    osal_proc_print(p, "\n------ handle msg ------\n");
    tde_osi_list_proc(p);

    osal_proc_print(p, "\n------ debug msg ------\n");
    HI_GFX_ProcMsg(p);
    osal_proc_print(p, "\n------ end proc msg ------\n");

    return 0;
}

hi_s32 tde_proc_init(hi_void)
{
    g_pstTdeProcInfo = (TDE_PROCINFO_S *)HI_GFX_VMALLOC(HIGFX_TDE_ID, sizeof(TDE_PROCINFO_S));
    if (g_pstTdeProcInfo == NULL) {
        return HI_FAILURE;
    }

    memset_s(g_pstTdeProcInfo, sizeof(TDE_PROCINFO_S), 0, sizeof(TDE_PROCINFO_S));

    g_pstTdeProcInfo->u32CurNode = 0;
    g_pstTdeProcInfo->bProcEnable = HI_TRUE;

    return HI_SUCCESS;
}

hi_void tde_proc_dinit(hi_void)
{
    if (g_pstTdeProcInfo != NULL) {
        HI_GFX_VFREE(HIGFX_TDE_ID, (hi_void *)g_pstTdeProcInfo);
    }

    g_pstTdeProcInfo = NULL;

    return;
}
#endif
