/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "tee_osal.h"
#include "vfmw.h"
#include <hi_tee_module_id.h>
#include "tee_server_comm.h"
#include "dbg.h"

#ifdef PRODUCT_STB
#define SECURE_NOTIFY_IRQ_NUM (146 + 32)

TEE_UUID g_vfmw_uuid = {
    0x3c2bfc84,
    0xc03c,
    0x11e6,
    { 0xa4, 0xa6, 0xce, 0xc0, 0xc9, 0x32, 0xce, 0x01 }
};
#else
#define SECURE_NOTIFY_IRQ_NUM (21 + 32)

TEE_UUID g_vfmw_uuid = {
    0x0D0D0D0D,
    0x0D0D,
    0x0D0D,
    { 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D }
};
#endif

#define MEM_RECORD_LEN VFMW_CHAN_NUM * 32
#define PRN_OS         PRN_ALWS

static tee_irq_spin_lock g_s_os_lock;
static tee_irq_spin_lock g_s_tee_spin_lock[OS_MAX_SPIN_LOCK];
static hi_u16 g_s_spin_lock_used[OS_MAX_SPIN_LOCK] = { 0 };
static vfmw_mem_desc g_s_mem_record[MEM_RECORD_LEN];
static hi_s32 g_s_default_thread_handle;

static hi_s32 tee_get_idle_id(hi_u16 used[], hi_s32 num)
{
    hi_s32 id = 0;
    static hi_u16 unid = 1;

    g_s_os_lock.irq_lockflags = tee_hal_spin_lock_irqsave((struct tee_hal_spinlock *)&g_s_os_lock.irq_lock);

    for (id = 0; id < num; id++) {
        if (used[id] == 0) {
            if (unid >= 0x7000) {
                unid = 1;
            }
            used[id] = unid++;
            break;
        }
    }

    tee_hal_spin_unlock_irqrestore((struct tee_hal_spinlock *)&g_s_os_lock.irq_lock, g_s_os_lock.irq_lockflags);

    if (id < num) {
        return id;
    } else {
        return -1;
    }
}

static hi_s32 tee_vfmw_ioctl(const hi_u32 cmd, hi_void *ioctl_args, size_t size)
{
    hi_s32 ret = OSAL_ERR;
    TEE_IOCTRL_ARGS_ST *args = (TEE_IOCTRL_ARGS_ST *)ioctl_args;

    VFMW_ASSERT_RET(((args != HI_NULL) || (size == 0)), OSAL_ERR);

    switch (cmd) {
        case TEE_VFMW_IOCTL_INIT:
            VFMW_ASSERT_RET((args != HI_NULL), OSAL_ERR);
            ret = ts_comm_vfmw_init(args->arg0);
            break;

        case TEE_VFMW_IOCTL_EXIT:
            ret = ts_comm_vfmw_exit();
            break;

        case TEE_VFMW_IOCTL_RESUME:
            ret = ts_comm_vfmw_resume();
            break;

        case TEE_VFMW_IOCTL_SUSPEND:
            ret = ts_comm_vfmw_suspend();
            break;

        case TEE_VFMW_IOCTL_CONTROL:
            VFMW_ASSERT_RET((args != HI_NULL), OSAL_ERR);
            ret = ts_comm_vfmw_control((hi_s32)args->arg0, args->arg1, args->arg2, args->arg3);
            break;

        case TEE_VFMW_IOCTL_THREAD:
            ret = ts_comm_vfmw_thread();
            break;

        case TEE_VFMW_IOCTL_READ_PROC:
            VFMW_ASSERT_RET((args != HI_NULL), OSAL_ERR);
            ret = ts_comm_vfmw_read_proc((hi_s32)args->arg0, (hi_s32)args->arg1);
            break;

        case TEE_VFMW_IOCTL_WRITE_PROC:
            VFMW_ASSERT_RET((args != HI_NULL), OSAL_ERR);
            ret = ts_comm_vfmw_write_proc((hi_s32)args->arg0, (hi_s32)args->arg1);
            break;
        case TEE_VFMW_IOCTL_CHECK_ADDR:
            VFMW_ASSERT_RET((args != HI_NULL), OSAL_ERR);
            ret = ts_comm_vfmw_check_es_addr(args->arg0, args->arg1);
            break;

        default:
            dprint(PRN_ERROR, "Not support IOCTL 0x%x\n", cmd);
            ret = OSAL_ERR;
    }

    return ret;
}

static hi_s32 tee_vfmw_ioctl_init(hi_void)
{
    if (0 != tee_hal_module_register(HI_MODULE_ID_VFMW, tee_vfmw_ioctl)) {
        dprint(PRN_ERROR, "VFMW module register failed\n");
        return -1;
    }

    VFMW_CHECK_SEC_FUNC(memset_s(g_s_mem_record, sizeof(g_s_mem_record), 0, sizeof(g_s_mem_record)));

    return 0;
}

static hi_u64 tee_do_div(hi_u64 dividend, hi_u32 divison)
{
    if (divison == 0) {
        return 0;
    }

    return ((hi_u32)dividend) / divison;
}

static hi_void *tee_create_task(hi_void *task_func, hi_void *param, hi_s8 *task_name)
{
    return &g_s_default_thread_handle;
}

static hi_void tee_mb(hi_void)
{
    return;
}

static hi_u8 *tee_map_reg(hi_u32 phy_addr, hi_s32 length)
{
    return (hi_u8 *)(tee_hal_phys_to_virt(phy_addr));
}

static hi_void tee_unmap_reg(hi_u8 *vir_addr)
{
    return;
}

static hi_void tee_udelay(hi_u32 usecs)
{
    return;
}

/*************************EventQueue******************************/
static hi_s32 tee_init_waitque(hi_s32 *handle, hi_s32 init_val)
{
    return OSAL_OK;
}

static hi_s32 tee_wakeup_waitque(hi_s32 handle)
{
    return OSAL_OK;
}

static hi_s32 tee_wait_waitque(hi_s32 handle, hi_s32 wait_time_ms)
{
    return OSAL_OK;
}

static hi_void tee_exit_waitque(hi_s32 handle)
{
}

/**************************SpinLock**************************************/
static hi_s32 tee_spin_lock_init(hi_s32 *handle)
{
    hi_s32 id;

    id = tee_get_idle_id(g_s_spin_lock_used, OS_MAX_SPIN_LOCK);
    if (id < 0) {
        return OSAL_ERR;
    }

    *handle = id + (g_s_spin_lock_used[id] << 16); /* 16 :a number */

    return OSAL_OK;
}

static hi_s32 tee_spin_lock(hi_s32 handle, hi_ulong *flag)
{
    hi_s32 id = handle & 0xffff;
    hi_u32 unid = handle >> 16; /* 16 :a number */

    if (id >= OS_MAX_SPIN_LOCK ||
        g_s_spin_lock_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return OSAL_ERR;
    }

    g_s_tee_spin_lock[id].irq_lockflags = tee_hal_spin_lock_irqsave((struct tee_hal_spinlock *)
                                                                  &g_s_tee_spin_lock[id].irq_lock);

    return OSAL_OK;
}

static hi_s32 tee_spin_unlock(hi_s32 handle, hi_ulong *flag)
{
    hi_s32 id = handle & 0xffff;
    hi_u32 unid = handle >> 16; /* 16 :a number */

    if (id >= OS_MAX_SPIN_LOCK ||
        g_s_spin_lock_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return OSAL_ERR;
    }

    tee_hal_spin_unlock_irqrestore((struct tee_hal_spinlock *)&g_s_tee_spin_lock[id].irq_lock,
                                   g_s_tee_spin_lock[id].irq_lockflags);

    return OSAL_OK;
}

static hi_void tee_spin_lock_exit(hi_s32 handle)
{
    hi_s32 id = handle & 0xffff;
    hi_u32 unid = handle >> 16; /* 16 :a number */

    if (id >= OS_MAX_SPIN_LOCK ||
        g_s_spin_lock_used[id] != unid) {
        dprint(PRN_OS, "%s: handle = %x error \n", __func__, handle);
        return;
    }

    g_s_spin_lock_used[id] = 0;
}

/**************************Time************************************/
static hi_u32 tee_get_time_in_ms(hi_void)
{
    hi_u32 curr_ms;
    hi_u64 tmp;

    tmp = tee_hal_get_system_time_in_us();
    curr_ms = tmp / 1000; /* 1000 :a number */

    return curr_ms;
}

static hi_u32 tee_get_time_in_us(hi_void)
{
    hi_u32 curr_us;
    hi_u64 tmp;

    tmp = tee_hal_get_system_time_in_us();
    curr_us = tmp;

    return curr_us;
}

/*****************************Sema******************************/
static hi_s32 tee_sema_init(hi_s32 *handle)
{
    return OSAL_OK;
}

static hi_s32 tee_sema_down(hi_s32 handle)
{
    return OSAL_OK;
}

static hi_s32 tee_sema_try(hi_s32 handle)
{
    return OSAL_OK;
}

static hi_void tee_sema_up(hi_s32 handle)
{
    return;
}

static hi_void tee_sema_exit(hi_s32 handle)
{
}

/*****************************Memory********************************************************/
static hi_s32 tee_mem_malloc(hi_s8 *mem_name, hi_s32 len, hi_s32 align, hi_s32 is_cached, vfmw_mem_desc *mem_desc)
{
#ifdef VFMW_MMU_SUPPORT
    TEE_SMMU_BUFFER_S mbuf;

    VFMW_CHECK_SEC_FUNC(memset_s(&mbuf, sizeof(TEE_SMMU_BUFFER_S), 0, sizeof(TEE_SMMU_BUFFER_S)));

    if (DRV_TEE_SMMU_Alloc(mem_name, len, align, &mbuf) != OSAL_OK) {
        VFMW_CHECK_SEC_FUNC(memset_s(mem_desc, sizeof(vfmw_mem_desc), 0, sizeof(vfmw_mem_desc)));
        return OSAL_ERR;
    }

    if (mbuf.u32StartSmmuAddr != 0) {
        if (DRV_TEE_SMMU_MapCpu(&mbuf, is_cached) != OSAL_OK) {
            VFMW_CHECK_SEC_FUNC(memset_s(mem_desc, sizeof(vfmw_mem_desc), 0, sizeof(vfmw_mem_desc)));
            return OSAL_ERR;
        }
    } else {
        VFMW_CHECK_SEC_FUNC(memset_s(mem_desc, sizeof(vfmw_mem_desc), 0, sizeof(vfmw_mem_desc)));
        return OSAL_ERR;
    }

    mem_desc->phy_addr = (UADDR)mbuf.u32StartSmmuAddr;
    mem_desc->vir_addr = PTR_UINT64(mbuf.pu8StartVirAddr);
    mem_desc->length = (hi_s32)mbuf.u32Size;
#else
    TEE_MMZ_BUFFER_S mbuf;

    VFMW_CHECK_SEC_FUNC(memset_s(&mbuf, sizeof(TEE_MMZ_BUFFER_S), 0, sizeof(TEE_MMZ_BUFFER_S)));

    if (DRV_TEE_MMZ_Alloc(mem_name, len, align, 1, &mbuf) != OSAL_OK) {
        VFMW_CHECK_SEC_FUNC(memset_s(mem_desc, sizeof(vfmw_mem_desc), 0, sizeof(vfmw_mem_desc)));
        return OSAL_ERR;
    }

    if (mbuf.u32StartPhyAddr != 0) {
        if (DRV_TEE_MMZ_MapCpu(&mbuf, is_cached) != OSAL_OK) {
            VFMW_CHECK_SEC_FUNC(memset_s(mem_desc, sizeof(vfmw_mem_desc), 0, sizeof(vfmw_mem_desc)));
            return OSAL_ERR;
        }
    } else {
        VFMW_CHECK_SEC_FUNC(memset_s(mem_desc, sizeof(vfmw_mem_desc), 0, sizeof(vfmw_mem_desc)));
        return OSAL_ERR;
    }

    mem_desc->phy_addr = (UADDR)mbuf.u32StartPhyAddr;
    mem_desc->vir_addr = PTR_UINT64(mbuf.pu8StartVirAddr);
    mem_desc->length = (hi_s32)mbuf.u32Size;
#endif

    return OSAL_OK;
}

static hi_s32 tee_mem_free(vfmw_mem_desc *mem_desc)
{
#ifdef VFMW_MMU_SUPPORT
    TEE_SMMU_BUFFER_S mbuf;

    VFMW_CHECK_SEC_FUNC(memset_s(&mbuf, sizeof(TEE_SMMU_BUFFER_S), 0, sizeof(TEE_SMMU_BUFFER_S)));
    mbuf.u32StartSmmuAddr = (hi_u32)mem_desc->phy_addr;
    mbuf.pu8StartVirAddr = UINT64_PTR(mem_desc->vir_addr);
    mbuf.u32Size = (hi_u32)mem_desc->length;
    DRV_TEE_SMMU_UnmapCpu(&mbuf);
    DRV_TEE_SMMU_Free(&mbuf);
#else
    TEE_MMZ_BUFFER_S mbuf;

    VFMW_CHECK_SEC_FUNC(memset_s(&mbuf, sizeof(TEE_MMZ_BUFFER_S), 0, sizeof(TEE_MMZ_BUFFER_S)));
    mbuf.u32StartPhyAddr = (hi_u32)mem_desc->phy_addr;
    mbuf.pu8StartVirAddr = UINT64_PTR(mem_desc->vir_addr);
    mbuf.u32Size = (hi_u32)mem_desc->length;
    DRV_TEE_MMZ_UnmapCpu(&mbuf);
    DRV_TEE_MMZ_Free(&mbuf);
#endif

    return OSAL_OK;
}

static hi_u8 *tee_mem_map(mem_record *mem_rec)
{
#ifdef VFMW_MMU_SUPPORT
    TEE_SMMU_BUFFER_S mbuf;

    VFMW_CHECK_SEC_FUNC(memset_s(&mbuf, sizeof(TEE_SMMU_BUFFER_S), 0, sizeof(TEE_SMMU_BUFFER_S)));
    mbuf.u32StartSmmuAddr = mem_rec->phy_addr;
    mbuf.u32Size = mem_rec->length;

    if (DRV_TEE_SMMU_MapCpu(&mbuf, 0) != HI_SUCCESS) {
        dprint(PRN_ERROR, "vfmw_osal.c, line %d: HI_DRV_SMMU_Map ERR\n", __LINE__);
    }

    return mbuf.pu8StartVirAddr;
#else
    TEE_MMZ_BUFFER_S mbuf;

    VFMW_CHECK_SEC_FUNC(memset_s(&mbuf, sizeof(TEE_MMZ_BUFFER_S), 0, sizeof(TEE_MMZ_BUFFER_S)));
    mbuf.u32StartPhyAddr = mem_rec->phy_addr;
    mbuf.u32Size = mem_rec->length;

    if (DRV_TEE_MMZ_MapCpu(&mbuf) != HI_SUCCESS) {
        dprint(PRN_ERROR, "vfmw_osal.c, line %d: HI_DRV_MMZ_Map ERR\n", __LINE__);
    }

    return mbuf.pu8StartVirAddr;
#endif
}

static hi_void tee_mem_unmap(hi_u8 *p)
{
#ifdef VFMW_MMU_SUPPORT
    TEE_SMMU_BUFFER_S mbuf;

    VFMW_CHECK_SEC_FUNC(memset_s(&mbuf, sizeof(TEE_SMMU_BUFFER_S), 0, sizeof(TEE_SMMU_BUFFER_S)));
    mbuf.pu8StartVirAddr = p;
    DRV_TEE_SMMU_UnmapCpu(&mbuf);
#else
    TEE_MMZ_BUFFER_S mbuf;

    VFMW_CHECK_SEC_FUNC(memset_s(&mbuf, sizeof(TEE_MMZ_BUFFER_S), 0, sizeof(TEE_MMZ_BUFFER_S)));
    mbuf.pu8StartVirAddr = p;
    DRV_TEE_MMZ_UnmapCpu(&mbuf);
#endif
}

static hi_u8 *tee_mem_map_mmz(mem_record *mem_rec)
{
    TEE_MMZ_BUFFER_S mem_buf;

    VFMW_CHECK_SEC_FUNC(memset_s(&mem_buf, sizeof(TEE_MMZ_BUFFER_S), 0, sizeof(TEE_MMZ_BUFFER_S)));
    mem_buf.u32StartPhyAddr = mem_rec->phy_addr;
    mem_buf.u32Size = mem_rec->length;

    if (DRV_TEE_MMZ_MapCpu(&mem_buf, HI_FALSE) != HI_SUCCESS) {
        dprint(PRN_ERROR, "vfmw_osal.c, line %d: HI_DRV_MMZ_Map ERR\n", __LINE__);
    }

    return mem_buf.pu8StartVirAddr;
}

static hi_void tee_mem_unmap_mmz(hi_u8 *p)
{
    TEE_MMZ_BUFFER_S mem_buf;

    VFMW_CHECK_SEC_FUNC(memset_s(&mem_buf, sizeof(TEE_MMZ_BUFFER_S), 0, sizeof(TEE_MMZ_BUFFER_S)));
    mem_buf.pu8StartVirAddr = p;
    DRV_TEE_MMZ_UnmapCpu(&mem_buf);
}

static hi_u8 *tee_mem_map_cache(mem_record *mem_rec)
{
    return HI_NULL;
}

static hi_u8 *tee_nsmem_map(UADDR phy_addr, hi_u32 size)
{
#ifdef VFMW_MMU_SUPPORT
    TEE_SMMU_BUFFER_S mem_buf;

    VFMW_CHECK_SEC_FUNC(memset_s(&mem_buf, sizeof(TEE_SMMU_BUFFER_S), 0, sizeof(TEE_SMMU_BUFFER_S)));
    mem_buf.u32StartSmmuAddr = phy_addr;
    mem_buf.u32Size = size;

    if (DRV_TEE_NSSMMU_MapCpu(&mem_buf, 0) != HI_SUCCESS) {
        dprint(PRN_ERROR, "vfmw_osal.c, line %d: HI_DRV_SMMU_Map ERR\n", __LINE__);
    }

    return mem_buf.pu8StartVirAddr;
#else
    TEE_MMZ_BUFFER_S mem_buf;

    VFMW_CHECK_SEC_FUNC(memset_s(&mem_buf, sizeof(TEE_MMZ_BUFFER_S), 0, sizeof(TEE_MMZ_BUFFER_S)));
    mem_buf.u32StartPhyAddr = phy_addr;
    mem_buf.u32Size = size;

    if (DRV_TEE_NSMMZ_MapCpu(&mem_buf, 0) != HI_SUCCESS) {
        dprint(PRN_ERROR, "vfmw_osal.c, line %d: HI_DRV_MMZ_Map ERR\n", __LINE__);
    }

    return mem_buf.pu8StartVirAddr;
#endif
}

static hi_void tee_nsmem_unmap(hi_u8 *p)
{
#ifdef VFMW_MMU_SUPPORT
    TEE_SMMU_BUFFER_S mem_buf;

    VFMW_CHECK_SEC_FUNC(memset_s(&mem_buf, sizeof(TEE_SMMU_BUFFER_S), 0, sizeof(TEE_SMMU_BUFFER_S)));
    mem_buf.pu8StartVirAddr = p;
    DRV_TEE_NSSMMU_UmapCpu(&mem_buf);
#else
    TEE_MMZ_BUFFER_S mem_buf;

    VFMW_CHECK_SEC_FUNC(memset_s(&mem_buf, sizeof(TEE_MMZ_BUFFER_S), 0, sizeof(TEE_MMZ_BUFFER_S)));
    mem_buf.pu8StartVirAddr = p;
    DRV_TEE_NSMMZ_UmapCpu(&mem_buf);
#endif
}

static hi_void tee_get_page_table_addr(UADDR *ptaddr, UADDR *err_rdaddr, UADDR *err_wraddr)
{
#ifdef VFMW_MMU_SUPPORT
    DRV_TEE_SMMU_GetPgtbladdr(err_rdaddr, err_wraddr, ptaddr);
#endif
}

static hi_void tee_flush_cache(hi_void *vir, UADDR phy_addr, hi_u32 len)
{
#ifdef VFMW_MMU_SUPPORT
    DRV_TEE_MEM_FlushCache(vir, len);
#endif
}

static hi_s32 tee_request_irq(hi_u32 irq_num, hi_void *handler, hi_s32 flags, const char *name, hi_void *dev)
{
    hi_u32 ret;

    ret = tee_hal_request_irq(irq_num, (hi_void *)handler, 0, (hi_void *)flags);
    if (ret != 0) {
        dprint(PRN_ERROR, "tee_hal_request_irq failed, irq_num=%d\n", irq_num);
    }

    return ret;
}

static hi_s32 tee_delete_irq(hi_u32 irq_num, const char *name, hi_void *dev)
{
    tee_hal_delete_irq(irq_num);

    return 0;
}

static hi_s32 tee_get_unid(hi_void)
{
    return 1;
}

static hi_s32 tee_atomic_init(hi_s32 *handle, hi_s32 val)
{
    return OSAL_OK;
}

static hi_void tee_atomic_exit(hi_s32 handle)
{
}

static hi_void tee_atomic_set(hi_s32 handle, hi_s32 val)
{
}

static hi_s32 tee_atomic_read(hi_s32 handle)
{
    return OSAL_OK;
}

static hi_s32 tee_atomic_inc_return(hi_s32 handle)
{
    return OSAL_OK;
}

static hi_s32 tee_atomic_dec_return(hi_s32 handle)
{
    return OSAL_OK;
}

static hi_s8 *tee_ioremap(UADDR phy_addr, hi_u32 len)
{
    return HI_NULL;
}

static hi_void tee_iounmap(hi_u8 *vir_addr)
{
    return;
}

static hi_void *tee_fopen(const char *file_name, int flags, int mode)
{
    tee_file *file = HI_NULL;

    if (file == HI_NULL) {
        return HI_NULL;
    } else {
        return file;
    }
}

static hi_void tee_fclose(hi_void *fp)
{
    if (fp) {
    }
}

static hi_s32 tee_fread(char *buf, hi_u32 len, hi_void *fp)
{
    if (fp = HI_NULL) {
        return -1;
    }

    return 0;
}

static hi_s32 tee_fwrite(char *buf, hi_u32 len, hi_void *fp)
{
    size_t writelen;

    if (fp = HI_NULL) {
        return -1;
    }

    return writelen;
}

static vfmw_mem_desc *tee_find_empty_record_slot(hi_void)
{
    hi_s32 i = 0;

    for (i = 0; i < MEM_RECORD_LEN; i++) {
        if (g_s_mem_record[i].phy_addr == 0) {
            break;
        }
    }

    if (i < MEM_RECORD_LEN) {
        return &g_s_mem_record[i];
    }

    return HI_NULL;
}

static vfmw_mem_desc *tee_find_mem_record_item(hi_void *vir)
{
    hi_u64 vir_addr;
    hi_s32 i = 0;

    vir_addr = PTR_UINT64(vir);

    for (i = 0; i < MEM_RECORD_LEN; i++) {
        if (g_s_mem_record[i].vir_addr == vir_addr) {
            break;
        }
    }

    if (i < MEM_RECORD_LEN) {
        return &g_s_mem_record[i];
    }

    return HI_NULL;
}

static hi_void *tee_alloc_vir_mem(hi_s32 size)
{
    vfmw_mem_desc *mem_record = HI_NULL;
    hi_s32 ret = OSAL_ERR;

    if (size <= 0) {
        return HI_NULL;
    }

    mem_record = tee_find_empty_record_slot();
    if (mem_record == HI_NULL) {
        return HI_NULL;
    }

    ret = tee_mem_malloc("Tee_Vfmw_Vir", size, 4, 1, mem_record); /* 4 :a number */
    if (ret != OSAL_OK) {
        return HI_NULL;
    }

    return UINT64_PTR(mem_record->vir_addr);
}

static hi_void tee_free_vir_mem(hi_void *vir)
{
    vfmw_mem_desc *mem_record;

    mem_record = tee_find_mem_record_item(vir);
    if (mem_record == HI_NULL) {
        dprint(PRN_ERROR, "Free VirMem Failed!\n");
        return;
    }

    (hi_void)tee_mem_free(mem_record);

    VFMW_CHECK_SEC_FUNC(memset_s(mem_record, sizeof(vfmw_mem_desc), 0, sizeof(vfmw_mem_desc)));
}

static hi_s32 tee_get_vir_mem_num(hi_void)
{
    return 0;
}

extern hi_s32 tee_vsnprintf(hi_char *str, hi_u32 size, const hi_char *format, va_list ap);

static hi_void tee_dump_proc(hi_void *page, hi_s32 page_count, hi_s32 *used_bytes, hi_s8 from_shr,
                             const hi_s8 *format, ...)
{
    va_list arg_list;

    va_start(arg_list, format);
    (*used_bytes) += tee_vsnprintf((page + (*used_bytes)), (page_count - (*used_bytes)), format, arg_list);
    va_end(arg_list);
}

hi_s32 tee_malloc(osal_mem *mem)
{
    hi_s32 ret = 0;
    vfmw_mem_desc vfmw_mem_desc = { 0 };
    hi_void *vir = HI_NULL;

    vfmw_mem_desc.length = mem->len;

    if (mem->is_cached == 0) {
        ret = tee_mem_malloc("vfmw_mem", mem->len, 16, 0, &vfmw_mem_desc); /* 16 :a number */
        mem->phy_addr = vfmw_mem_desc.phy_addr;
        mem->vir_addr = UINT64_PTR(vfmw_mem_desc.vir_addr);
        mem->len = vfmw_mem_desc.length;
    } else {
        vir = tee_alloc_vir_mem(mem->len);
        ret = (vir == HI_NULL) ? OSAL_ERR : OSAL_OK;
        mem->phy_addr = PTR_UINT64(vir);
        mem->vir_addr = (hi_u8 *)(vir);
    }

    return ret;
}

hi_s32 tee_free(osal_mem *mem)
{
    hi_s32 ret = 0;
    vfmw_mem_desc vfmw_mem_desc = { 0 };

    if (mem->is_cached == 0) {
        vfmw_mem_desc.length = mem->len;
        vfmw_mem_desc.vir_addr = PTR_UINT64(mem->vir_addr);
        vfmw_mem_desc.phy_addr = mem->phy_addr;
        ret = tee_mem_free(&vfmw_mem_desc);
    } else {
        tee_free_vir_mem(mem->vir_addr);
        ret = OSAL_OK;
    }

    return ret;
}

hi_u8 *tee_pre_alloc_map(osal_mem *mem)
{
    mem_record mem_record = { 0 };

    mem_record.length = mem->len;
    mem_record.phy_addr = mem->phy_addr;

    mem->vir_addr = tee_mem_map(&mem_record);

    return mem->vir_addr;
}

hi_void tee_pre_alloc_unmap(osal_mem *mem)
{
    tee_mem_unmap(mem->vir_addr);
}

hi_void osal_intf_init(hi_void)
{
    vfmw_osal_ops *ops = &g_vfmw_osal_ops;

    VFMW_CHECK_SEC_FUNC(memset_s(ops, 0, sizeof(vfmw_osal_ops)));

    ops->get_time_in_ms = tee_get_time_in_ms;
    ops->get_time_in_us = tee_get_time_in_us;
    ops->os_spin_lock_init = tee_spin_lock_init;
    ops->os_spin_lock = tee_spin_lock;
    ops->os_spin_unlock = tee_spin_unlock;
    ops->os_spin_lock_exit = tee_spin_lock_exit;
    ops->sema_init = tee_sema_init;
    ops->sema_down = tee_sema_down;
    ops->sema_up = tee_sema_up;
    ops->sema_try = tee_sema_try;
    ops->sema_exit = tee_sema_exit;
    ops->os_memcpy = __asm_memcpy;
    ops->os_memmov = __asm_memmove;
    ops->os_memcmp = memcmp;
    ops->print = (fn_osal_print)tee_hal_printf;
    ops->strncmp = (fn_osal_strncmp)strncmp;
    ops->strlen = (fn_osal_strlen)strlen;
    ops->strstr = (fn_osal_strstr)strstr;
    ops->msleep = tee_udelay;
    ops->os_mb = tee_mb;
    ops->udelay = tee_udelay;
    ops->event_init = tee_init_waitque;
    ops->event_give = tee_wakeup_waitque;
    ops->event_wait = tee_wait_waitque;
    ops->event_exit = tee_exit_waitque;
    ops->flush_cache = tee_flush_cache;
    ops->mem_alloc = tee_mem_malloc;
    ops->mem_free = tee_mem_free;
    ops->reg_map = tee_map_reg;
    ops->reg_unmap = tee_unmap_reg;
    ops->mmap = tee_mem_map;
    ops->munmap = tee_mem_unmap;
    ops->mmap_cache = tee_mem_map_cache;
    ops->mmap_mmz = tee_mem_map_mmz;
    ops->munmap_mmz = tee_mem_unmap_mmz;
    ops->ns_mmap = tee_nsmem_map;
    ops->ns_munmap = tee_nsmem_unmap;
    ops->get_page_table_addr = tee_get_page_table_addr;
    ops->create_thread = tee_create_task;
    ops->request_irq = tee_request_irq;
    ops->free_irq = tee_delete_irq;
    ops->wakeup_ree = tee_wakeup_ree;
    ops->get_unid = tee_get_unid;
    ops->atomic_init = tee_atomic_init;
    ops->atomic_exit = tee_atomic_exit;
    ops->atomic_set = tee_atomic_set;
    ops->atomic_read = tee_atomic_read;
    ops->atomic_inc_ret = tee_atomic_inc_return;
    ops->atomic_dec_ret = tee_atomic_dec_return;
    ops->file_open = tee_fopen;
    ops->file_close = tee_fclose;
    ops->file_read = tee_fread;
    ops->file_write = tee_fwrite;
    ops->ioremap = tee_ioremap;
    ops->iounmap = tee_iounmap;
    ops->alloc_vir_mem = tee_alloc_vir_mem;
    ops->free_vir_mem = tee_free_vir_mem;
    ops->get_vir_mem_num = tee_get_vir_mem_num;
    ops->dump_proc = tee_dump_proc;
    ops->do_div = tee_do_div;
    ops->malloc = tee_malloc;
    ops->free = tee_free;
    ops->pre_alloc_map = tee_pre_alloc_map;
    ops->pre_alloc_unmap = tee_pre_alloc_unmap;

    return OSAL_OK;
}

hi_void osal_intf_exit(hi_void)
{
    VFMW_CHECK_SEC_FUNC(memset_s(&g_vfmw_osal_ops, 0, sizeof(vfmw_osal_ops)));
}

hi_s32 tee_check_vfmw_uuid(hi_void)
{
    hi_bool valid = HI_FALSE;
    hi_s32 ret;

    TEE_UUID cur_uuid;
    VFMW_CHECK_SEC_FUNC(memset_s(&cur_uuid, 0x00, sizeof(cur_uuid)));
    ret = tee_hal_current_uuid(&cur_uuid);
    if (ret == HI_SUCCESS) {
        if (memcmp(&cur_uuid, &g_vfmw_uuid, sizeof(g_vfmw_uuid)) == 0) {
            valid = HI_TRUE;
        }
    }

    return valid;
}

hi_bool tee_trust_ta_check_uuid(hi_void)
{
    hi_bool valid = HI_FALSE;
    hi_s32 ret;

    TEE_UUID pr_uuid = {
        0x79b77788, 0x9789, 0x4a7a,
        { 0xa2, 0xbe, 0xb6, 0x01, 0x55, 0xee, 0xf5, 0xf8 }
    }; /* PlayReady TA UUID */
    TEE_UUID ww_uuid = {
        0x79b77788, 0x9789, 0x4a7a,
        { 0xa2, 0xbe, 0xb6, 0x01, 0x55, 0xee, 0xf5, 0xf9 }
    }; /* WideWine TA UUID */
    TEE_UUID svc_id = {
        0x99985ef1, 0xd62d, 0x4524,
        { 0x9d, 0xd1, 0xd9, 0x83, 0x45, 0x48, 0xd9, 0x8e }
    };

    TEE_UUID cur_uuid;
    VFMW_CHECK_SEC_FUNC(memset_s(&cur_uuid, 0x00, sizeof(cur_uuid)));
    ret = tee_hal_current_uuid(&cur_uuid);
    if (ret == HI_SUCCESS) {
        if (memcmp(&cur_uuid, &pr_uuid, sizeof(pr_uuid)) == 0) {
            valid = HI_TRUE;
        }
        if (memcmp(&cur_uuid, &ww_uuid, sizeof(ww_uuid)) == 0) {
            valid = HI_TRUE;
        }
        if (memcmp(&cur_uuid, &svc_id, sizeof(svc_id)) == 0) {
            valid = HI_TRUE;
        }
    }

    return valid;
}

/********************ǰ밲ȫַӳ*****************************/
/****************SecMode:0 ӳ䰲ȫڴ;1 ӳǰڴ************************/
hi_u32 tee_mmap_section(hi_u32 phy, hi_s32 size, hi_u32 sec_mode, hi_u32 cached)
{
    hi_u32 is_secure = (sec_mode == MEM_MAP_TYPE_NSEC) ? 0 : 1;
    hi_u32 is_cached = (cached == MEM_MAP_TYPE_NCACHE) ? 0 : 1;
    hi_u32 vir_addr;

    vir_addr = (hi_u32)tee_hal_remap(phy, size, is_secure, is_cached);
    if (vir_addr == 0) {
        dprint(PRN_ERROR, "%s %d, map_section_entry failed \n", __func__, __LINE__);
        return HI_NULL;
    }

    return vir_addr;
}

hi_s32 tee_unmap_section(hi_u32 vir_addr, hi_s32 size)
{
    tee_hal_unmap((hi_void *)vir_addr, size);

    return 0;
}

hi_void tee_wakeup_ree(hi_void)
{
    hi_u32 hw_num = SECURE_NOTIFY_IRQ_NUM;

    tee_hal_raise_pi(hw_num);

    return;
}

tee_hal_driver_init_late(tee_vfmw_ioctl_init);
