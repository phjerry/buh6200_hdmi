/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "local.h"
#include "vctrl.h"
#include "dbg.h"
#ifdef VFMW_VMM_SUPPORT
#include "vmm_ext.h"
#endif

typedef struct {
    hi_s32 ref_count;
    fun_vfmw_event_report fp_vdec_report;
    fun_vfmw_event_report fp_omx_report;
} local_entry;

static local_entry g_local_entry;

static INLINE local_entry *local_get_entry(hi_void)
{
    return &g_local_entry;
}

#ifdef VFMW_VMM_SUPPORT
static hi_void local_proc_alloc_mem(hi_void *args, hi_u32 len)
{
    hi_s32 ret;
    vmm_cmd_prio proir;
    vfmw_mem_report *mem_report_info;
    vmm_buffer vmm_buf;

    if (0) {
        dprint(PRN_ERROR, "%s ERROR: Arg len %d != %d\n", __func__, len, sizeof(vfmw_mem_report));
        return;
    }

    mem_report_info = (vfmw_mem_report *)args;

    VFMW_CHECK_SEC_FUNC(memset_s(&vmm_buf, sizeof(vmm_buffer), 0, sizeof(vmm_buffer)));

    if (snprintf_s(vmm_buf.buf_name, sizeof(vmm_buf.buf_name),
                   sizeof(vmm_buf.buf_name), "%s", mem_report_info->buf_name) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    vmm_buf.buf_name[sizeof(vmm_buf.buf_name) - 1] = '\0';
    vmm_buf.map = mem_report_info->is_map;
    vmm_buf.cache = mem_report_info->is_cache;
    vmm_buf.size = mem_report_info->size;
    vmm_buf.priv_id = mem_report_info->unique_id;
    vmm_buf.mem_type = VMM_MEM_TYPE_SYS_ADAPT;
    vmm_buf.sec_flag = 0;

    proir = (mem_report_info->phy_addr == 1) ? VMM_CMD_PRIO_MAX : VMM_CMD_PRIO_MIN;

    ret = vmm_send_command(VMM_CMD_ALLOC, proir, &vmm_buf);
    if (ret != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s ERROR: VMM_CMD_ALLOC %s size %d failed!\n",
               __func__, vmm_buf.buf_name, vmm_buf.size);
    }

    return;
}

static hi_void local_proc_rels_mem(hi_void *args, hi_u32 len)
{
    hi_s32 ret;
    vfmw_mem_report *mem_report_info;
    vmm_buffer vmm_buf;

    if (0) {
        dprint(PRN_ERROR, "%s ERROR: Arg len %d != %d\n",
               __func__, len, sizeof(vfmw_mem_report));
        return;
    }

    mem_report_info = (vfmw_mem_report *)args;

    VFMW_CHECK_SEC_FUNC(memset_s(&vmm_buf, sizeof(vmm_buffer), 0, sizeof(vmm_buffer)));

    if (snprintf_s(vmm_buf.buf_name, sizeof(vmm_buf.buf_name),
                   sizeof(vmm_buf.buf_name), "%s", mem_report_info->buf_name) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    vmm_buf.buf_name[sizeof(vmm_buf.buf_name) - 1] = '\0';
    vmm_buf.dma_buf = mem_report_info->dma_buf;
    vmm_buf.start_phy_addr = mem_report_info->phy_addr;
    vmm_buf.start_vir_addr = mem_report_info->vir_addr;
    vmm_buf.map = mem_report_info->is_map;
    vmm_buf.cache = mem_report_info->is_cache;
    vmm_buf.size = mem_report_info->size;
    vmm_buf.priv_id = mem_report_info->unique_id;
    vmm_buf.mem_type = VMM_MEM_TYPE_SYS_ADAPT;
    vmm_buf.sec_flag = 0;

    ret = vmm_send_command(VMM_CMD_RELEASE, VMM_CMD_PRIO_MIN, &vmm_buf);
    if (ret != VMM_SUCCESS) {
        dprint(PRN_ERROR, "%s ERROR: VMM_CMD_RELEASE %s phy 0x%x failed!\n",
               __func__, vmm_buf.buf_name, vmm_buf.start_phy_addr);
    }

    return;
}

static hi_s32 local_proc_inner_event(hi_s32 type, hi_void *args, hi_u32 len)
{
    switch (type) {
        case EVNT_ALLOC_MEM:
            local_proc_alloc_mem(args, len);
            break;

        case EVNT_RELEASE_MEM:
            local_proc_rels_mem(args, len);
            break;

        case EVNT_RELEASE_BURST_MEM:
            break;

        case EVNT_CANCEL_ALLOC:
            break;

        case EVNT_SET_ADDRESS_IN:
            break;

        default:
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}
#endif

static hi_s32 local_set_callback(vfmw_adpt_type type, fun_vfmw_event_report fp_event_report)
{
    local_entry *entry = local_get_entry();

    VFMW_ASSERT_RET(fp_event_report != HI_NULL, HI_FAILURE);

    switch (type) {
        case ADPT_TYPE_VDEC:
            entry->fp_vdec_report = fp_event_report;
            break;

        case ADPT_TYPE_OMX:
            entry->fp_omx_report = fp_event_report;
            break;

        default:
            dprint(PRN_ERROR, "%s Unkown Adapter type: %d\n", __func__, type);
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 local_event_report(hi_s32 instance_id, hi_s32 type, hi_void *args, hi_s32 len)
{
    hi_s32 ret;
    vfmw_adpt_type adpt_type = ADPT_TYPE_MAX;
    local_entry *entry = local_get_entry();

    ret = vctrl_control(instance_id, VFMW_CID_INQUIRE_ADPT_TYPE, &adpt_type);
    VFMW_ASSERT_RET(ret == HI_SUCCESS, HI_FAILURE);

#ifdef VFMW_VMM_SUPPORT
    ret = local_proc_inner_event(type, args, 0);
    if (ret == HI_SUCCESS) {
        return HI_SUCCESS;
    }
#endif

    switch (adpt_type) {
        case ADPT_TYPE_VDEC:
            VFMW_ASSERT_RET_PRNT(entry->fp_vdec_report != HI_NULL, HI_FAILURE, "ERROR: fp_vdec_report = HI_NULL\n");
            ret = entry->fp_vdec_report(instance_id, type, args, len);
            break;
        case ADPT_TYPE_OMX:
            VFMW_ASSERT_RET_PRNT(entry->fp_omx_report != HI_NULL, HI_FAILURE, "ERROR: fp_omx_report = HI_NULL\n");
            ret = entry->fp_omx_report(instance_id, type, args, len);
            break;
        default:
            dprint(PRN_ERROR, "Unsupport adpt_type %d.\n", adpt_type);
            break;
    }

    return ret;
}

hi_s32 local_open(hi_void)
{
    local_entry *entry = local_get_entry();

    VFMW_CHECK_SEC_FUNC(memset_s(entry, sizeof(local_entry), 0, sizeof(local_entry)));

    return vctrl_open();
}

hi_s32 local_close(hi_void)
{
    return vctrl_close();
}

hi_s32 local_init(hi_void *args, hi_u32 len)
{
    hi_s32 ret = HI_FAILURE;
    local_entry *entry = local_get_entry();
    vfmw_init_param *param = (vfmw_init_param *)args;

    if (entry->ref_count > 0) {
        goto exit;
    }

    ret = vctrl_init(args);

    vctrl_set_event_interface(&local_event_report);

exit:
    ret = local_set_callback(param->adpt_type, param->fn_callback);
    if (ret == HI_SUCCESS) {
        entry->ref_count++;
        VFMW_ASSERT_RET_PRNT(entry->ref_count < 0x7fffffff,
                             HI_FAILURE, "ref_count %d invalid!\n", entry->ref_count);
    }

    return ret;
}

hi_s32 local_exit(hi_void *args)
{
    local_entry *entry = local_get_entry();

    if (entry->ref_count <= 0) {
        return HI_FAILURE;
    }

    entry->ref_count--;

    if (entry->ref_count > 0) {
        return HI_SUCCESS;
    }

    return vctrl_exit(args);
}

hi_s32 local_suspend(hi_void *args)
{
    return vctrl_suspend(args);
}

hi_s32 local_resume(hi_void *args)
{
    return vctrl_resume(args);
}

hi_s32 local_get_image(hi_s32 chan_id, hi_void *image)
{
    return vctrl_get_image(chan_id, image);
}

hi_s32 local_check_image(hi_s32 chan_id, hi_void *image)
{
    return vctrl_check_image(chan_id, image);
}

hi_s32 local_release_image(hi_s32 chan_id, const hi_void *image)
{
    return vctrl_release_image(chan_id, image);
}

hi_s32 local_control(hi_s32 chan_id, hi_s32 cmd, hi_void *args, hi_u32 len)
{
    return vctrl_control(chan_id, cmd, args);
}


