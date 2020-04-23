/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __VMM_H__
#define __VMM_H__

#include "vfmw.h"

#define VMM_MAX_NAME_LEN           (16)
#define VMM_SUCCESS                (hi_s32)(0)
#define VMM_FAILURE                (hi_s32)(0xFFFFFFFF)
#define ERR_VMM_ALLOC_FAILED       (hi_s32)(0xFF000000)
#define ERR_VMM_INIT_FAILED        (hi_s32)(0xFF000001)
#define ERR_VMM_INVALID_PARA       (hi_s32)(0xFF000002)
#define ERR_VMM_NULL_PTR           (hi_s32)(0xFF000003)
#define ERR_VMM_NOT_SUPPORT        (hi_s32)(0xFF000004)
#define ERR_VMM_FULL               (hi_s32)(0xFF000005)
#define ERR_VMM_EMPTY              (hi_s32)(0xFF000006)
#define ERR_VMM_NOT_INIT           (hi_s32)(0xFF000007)
#define ERR_VMM_NODE_ALREADY_EXIST (hi_s32)(0xFF000008)
#define ERR_VMM_LOCATION_CONFLICT  (hi_s32)(0xFF000009)
#define ERR_VMM_NODE_NOT_EXIST     (hi_s32)(0xFF00000A)
#define ERR_VMM_INVALID_STATE      (hi_s32)(0xFF00000B)

#define VMM_TRUE  (hi_s32)(1)
#define VMM_FALSE (hi_s32)(0)

/******************************************************************************************************************/
/* CMD_TYPE                        |         INPUT_PARAM_TYPE      |          OUTPUT_PARAM_TYPE */
/* ---------------------------------------------------------------------------------------------------------------- */
/* VMM_CMD_Alloc                    |        vmm_buffer             |            vmm_buffer */
/* VMM_CMD_Release                  |        vmm_buffer             |            vmm_buffer */
/* VMM_CMD_ReleaseBurst             |        vmm_rls_burst          |            vmm_rls_burst */
/* ---------------------------------------------------------------------------------------------------------------- */
/* VMM_CMD_BLK_Alloc                |        vmm_buffer             |            vmm_buffer */
/* VMM_CMD_BLK_Release              |        vmm_buffer             |            HI_NULL */
/* VMM_CMD_BLK_ConfigMemPool        |        vmm_buffer             |            HI_NULL */
/* VMM_CMD_BLK_Map                  |        vmm_buffer             |            vmm_buffer */
/* VMM_CMD_BLK_Unmap                |        vmm_buffer             |            HI_NULL */
/* VMM_CMD_BLK_ConfigMemNode        |        vmm_buffer             |            HI_NULL */
/* VMM_CMD_BLK_ReleaseBurst         |        vmm_rls_burst          |            HI_NULL */
/* VMM_CMD_BLK_SetPriorityPolicy    |        undetermined           |            HI_NULL */
/* VMM_CMD_BLK_GetMemNodeInfo       |        undetermined           |            undetermined */
/* VMM_CMD_BLK_Setlocation          |        vmm_attr               |            HI_NULL */
/* VMM_CMD_BLK_CancleCmd            |        vmm_cancel_cmd         |            HI_NULL */
/* ---------------------------------------------------------------------------------------------------------------- */
typedef enum {
    /* memory */
    VMM_CMD_ALLOC = 0,
    VMM_CMD_RELEASE,
    VMM_CMD_RELEASE_BURST,

    VMM_CMD_TYPE_MAX
} vmm_cmd_type;

typedef enum {
    /* memory */
    VMM_CMD_BLK_ALLOC = 0,
    VMM_CMD_BLK_RELEASE,

    /* pre memory */
    VMM_CMD_BLK_CONFIG_MEM_POOL,

    VMM_CMD_BLK_MAP,
    VMM_CMD_BLK_UNMAP,

    /* External configuration frame memory(just one Node) */
    VMM_CMD_BLK_CONFIG_MEM_NODE,

    VMM_CMD_BLK_RELEASE_BURST,

    VMM_CMD_BLK_SET_PRIORITY_POLICY,
    VMM_CMD_BLK_GET_MEM_NODE_INFO,

    /* tracker */
    VMM_CMD_BLK_SET_LOCATION,

    VMM_CMD_BLK_CANCLE_CMD,

    VMM_CMD_BLK_TYPE_MAX
} vmm_cmd_blk_type;

typedef enum {
    VMM_CMD_PRIO_MIN = 0,
    VMM_CMD_PRIO_MAX
} vmm_cmd_prio;

typedef enum {
    /* memory */
    VMM_MEM_TYPE_SYS_ADAPT = 0,
    VMM_MEM_TYPE_FORCE_MMZ,
    VMM_MEM_TYPE_PRE_ALLOC,
    VMM_MEM_TYPE_EXT_CONFIG,
    VMM_MEM_TYPE_MAX
} vmm_mem_type;

typedef enum {
    /* tracker */
    VMM_LOCATION_INSIDE = 0,
    VMM_LOCATION_OUTSIDE,

    VMM_LOCATION_MAX,
} vmm_location;

typedef struct {
    hi_s8 buf_name[VMM_MAX_NAME_LEN];
    hi_u64 dma_buf;
    hi_ulong start_vir_addr;
    UADDR start_phy_addr;
    hi_u32 size;
    hi_u32 sec_flag;
    hi_u32 map;
    hi_u32 cache;
    vmm_mem_type mem_type;
    hi_handle vdec_handle;
    hi_handle ssm_handle;
    hi_u64 sec_info;

    hi_s64 priv_id;
} vmm_buffer;

typedef struct {
    UADDR phy_addr;
    vmm_location location;
} vmm_attr;

typedef struct {
    hi_s64 priv_id;
    hi_u64 priv_mask;
} vmm_rls_burst;

typedef struct {
    vmm_cmd_type cmd_id;
    hi_s64 priv_id;
    hi_u64 priv_mask;
} vmm_cancel_cmd;

typedef hi_s32 (*fn_vmm_callback)(vmm_cmd_type cmd_id, hi_void *param_out, hi_s32 ret_val);

/* FUNTION */
hi_s32 vmm_mod_init(fn_vmm_callback vmm_callback);

hi_s32 vmm_mod_exit(hi_void);

hi_s32 vmm_send_command_block(vmm_cmd_blk_type cmd_id, hi_void *param_in_out);

hi_s32 vmm_send_command(vmm_cmd_type cmd_id, vmm_cmd_prio proir, hi_void *param_in);

#endif /* __VMM_H__ */
