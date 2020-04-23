#include "drv_vdec_sdk.h"
#include "hi_drv_stat.h"
#include "hi_osal.h"
#include "vdec_priv_type.h"
#include <linux/ion.h>
#include <linux/dma-buf.h>

hi_s32 vdec_drv_get_vfmw_func(hi_void **func)
{
    return osal_exportfunc_get(HI_ID_VFMW, func);
}

hi_s32 vdec_drv_register(hi_void *func)
{
    return osal_exportfunc_register(HI_ID_VDEC, VDEC_MODULE_NAME, func);
}

hi_void vdec_drv_unregister(hi_void)
{
    osal_exportfunc_unregister(HI_ID_VDEC);
}

hi_void vdec_drv_stat_event(hi_u32 event_id, hi_u32 value)
{
    hi_drv_stat_event(event_id, value);
}

hi_s32 vdec_drv_create_proc(hi_char *name, fn_vdec_proc_read read_fn, osal_proc_cmd *cmd_list, hi_u32 cmd_cnt)
{
    osal_proc_entry *item = HI_NULL;

    item = osal_proc_add(name, strlen(name));
    if (item == HI_NULL) {
        return HI_FAILURE;
    }

    item->read = read_fn;
    item->cmd_list = cmd_list;
    item->cmd_cnt = cmd_cnt;

    return HI_SUCCESS;
}
hi_void vdec_drv_destroy_proc(hi_char *name)
{
    osal_proc_remove(name, strlen(name));
}

hi_s64 vdec_drv_get_fd(hi_void *buf)
{
    if (buf == HI_NULL) {
        return HI_INVALID_HANDLE;
    }

    return osal_mem_create_fd(buf, O_CLOEXEC);
}

hi_void vdec_drv_put_fd(hi_s64 fd)
{
    if (fd == -1) {
        return;
    }

    osal_mem_close_fd(fd);
}

hi_void*  vdec_drv_get_dma_buf(hi_s64 fd)
{
    hi_void *buf;

    buf = osal_mem_handle_get(fd, HI_ID_VDEC);
    if (IS_ERR(buf)) {
        return HI_NULL;
    }

    return buf;
}

hi_void vdec_drv_put_dma_buf(hi_void *buf)
{
    if (buf == HI_NULL) {
        return;
    }

    osal_mem_ref_put(buf, HI_ID_VDEC);
}

hi_u64 vdec_drv_get_phy(hi_void *buf, hi_bool is_sec)
{
    hi_bool is_smmu = HI_FALSE;

    if (buf == HI_NULL) {
        return 0;
    }

#ifdef HI_SMMU_SUPPORT
    is_smmu = HI_TRUE;
#endif

    if (is_smmu == HI_TRUE && is_sec == HI_FALSE) {
        return osal_mem_nssmmu_map(buf, 0);
    } else if (is_smmu == HI_TRUE && is_sec == HI_TRUE) {
        return osal_mem_secsmmu_map(buf, 0);
    } else if (is_smmu == HI_FALSE && is_sec == HI_FALSE) {
        return osal_mem_phys(buf);
    } else {
        return 0;
    }
}

hi_void vdec_drv_put_phy(hi_void *buf, hi_u64 phy, hi_bool is_sec)
{
    hi_bool is_smmu = HI_FALSE;

    if (buf == HI_NULL) {
        return;
    }

#ifdef HI_SMMU_SUPPORT
    is_smmu = HI_TRUE;
#endif

    if (is_smmu == HI_TRUE && is_sec == HI_FALSE) {
        osal_mem_nssmmu_unmap(buf, phy, 0);
    } else if (is_smmu == HI_TRUE && is_sec == HI_TRUE) {
        osal_mem_secsmmu_unmap(buf, phy, 0);
    }
}

