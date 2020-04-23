/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "vmm_ext.h"
#include "vmm_dbg.h"
#include "vmm_core_imp.h"

hi_s32 vmm_mod_init(fn_vmm_callback vmm_callback)
{
    D_VMM_CHECK_PTR_RET(vmm_callback, ERR_VMM_NULL_PTR);

    return vmm_core_init(vmm_callback);
}

hi_s32 vmm_mod_exit(hi_void)
{
    return vmm_core_deinit();
}

hi_s32 vmm_send_command(vmm_cmd_type cmd_id, vmm_cmd_prio proir, hi_void *param_in)
{
    D_VMM_CHECK_PTR_RET(param_in, ERR_VMM_NULL_PTR);

    if (cmd_id >= VMM_CMD_TYPE_MAX) {
        PRINT(PRN_ERROR, "%s invalid command %d\n", __func__, cmd_id);
        return ERR_VMM_INVALID_PARA;
    }

    return vmm_core_add_msg(cmd_id, proir, param_in);
}

hi_s32 vmm_send_command_block(vmm_cmd_blk_type cmd_id, hi_void *param_in_out)
{
    D_VMM_CHECK_PTR_RET(param_in_out, ERR_VMM_NULL_PTR);

    if (cmd_id >= VMM_CMD_BLK_TYPE_MAX) {
        PRINT(PRN_ERROR, "%s invalid command %d\n", __func__, cmd_id);
        return ERR_VMM_INVALID_PARA;
    }

    return vmm_core_send_cmd_block(cmd_id, param_in_out);
}


