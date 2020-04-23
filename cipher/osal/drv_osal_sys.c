/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_osal_sys
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include <asm/uaccess.h>
#include <linux/ion.h>
#include <linux/dma-buf.h>
#include "drv_osal_lib.h"

/************************* Internal Structure Definition *********************/
/** \addtogroup      base type */
/** @{ */ /** <!-- [base] */

/* under TEE, we only can malloc secure mmz at system steup,
 * then map the mmz to Smmu, but the smmu can't map to cpu address,
 * so we must save the cpu address in a static table when malloc and map mmz.
 * when call crypto_mem_map, we try to query the table to get cpu address firstly,
 * if can't get cpu address from the table, then call system api to map it.
 */
#define CRYPTO_MEM_MAP_TABLE_DEPTH 32

/* try to create memory */
#define PHY_MEM_CREATE_TRY_TIME (10)

typedef struct {
    hi_u32 valid;
    compat_addr dma;
    void *via;
} crypto_mem_map_table;

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      base */
/** @{ */ /** <!--[base] */

/* brief allocate and map a mmz or smmu memory
* we can't allocate smmu directly during TEE boot period.
* in addition, the buffer of cipher node list must be mmz.
* so here we have to allocate a mmz memory then map to smmu if necessary.
 */
static hi_s32 crypto_mem_alloc_remap(crypto_mem *mem, hi_u32 type, const char *name, hi_u32 size)
{
    void *kernel_dma_buf = HI_NULL;

    hi_log_func_enter();
    crypto_assert(size != 0);
    crypto_assert(name != HI_NULL);
    crypto_assert(mem != HI_NULL);
    kernel_dma_buf = osal_mem_alloc(name, size, type, NULL, 0);
    if (kernel_dma_buf == HI_NULL) {
        hi_log_print_func_err(osal_mem_alloc, 0);
        return HI_LOG_ERR_MEM;
    }

    mem->dma_virt = osal_mem_kmap(kernel_dma_buf, 0, 0);
    if (mem->dma_virt == NULL) {
        osal_mem_free(kernel_dma_buf);
        return HI_LOG_ERR_MEM;
    }

    if (type == MEM_TYPE_SMMU) {
        ADDR_U64(mem->dma_addr) = osal_mem_nssmmu_map(kernel_dma_buf, 0);
    } else {
        ADDR_U64(mem->dma_addr) = osal_mem_phys(kernel_dma_buf);
    }
    if (ADDR_U64(mem->dma_addr) == 0) {
        osal_mem_kunmap(kernel_dma_buf, mem->dma_virt, 0);
        osal_mem_free(kernel_dma_buf);
        return HI_LOG_ERR_MEM;
    }
    mem->mem_type = type;
    mem->dma_size = size;
    mem->kernel_dma = kernel_dma_buf;
    mem->user_buf = HI_NULL;
    hi_log_debug("MMZ/MMU malloc, MMU 0x%llx, VIA 0x%p, SIZE 0x%x\n",
        ADDR_U64(mem->dma_addr), mem->dma_virt, size);

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* brief release and unmap a mmz or smmu memory */
static hi_s32 crypto_mem_release_unmap(crypto_mem *mem)
{
    hi_s32 ret;

    hi_log_func_enter();

    crypto_assert(mem != HI_NULL);

    hi_log_debug("mem_release_unmap()- dma 0x%x, via 0x%p, size 0x%x\n",
                 ADDR_L32(mem->dma_addr), mem->dma_virt, mem->dma_size);

    if (mem->mem_type == MEM_TYPE_SMMU) {
        ret = osal_mem_nssmmu_unmap(mem->kernel_dma, ADDR_U64(mem->dma_addr), 0);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(osal_mem_nssmmu_unmap, ret);
            return HI_LOG_ERR_MEM;
        }
    }
    osal_mem_kunmap(mem->kernel_dma, mem->dma_virt, 0);
    osal_mem_free(mem->kernel_dma);
    mem->kernel_dma = HI_NULL;
    mem->dma_virt = HI_NULL;

    ret = memset_s(mem, sizeof(crypto_mem), 0, sizeof(crypto_mem));
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(memset_s, ret);
        return ret;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

/* brief map a mmz or smmu memory */
static hi_s32 crypto_mem_map(crypto_mem *mem)
{
    hi_log_func_enter();

    crypto_assert(mem != HI_NULL);

    hi_log_debug("crypto_mem_map()- dma 0x%x, size 0x%x\n",
                 ADDR_L32(mem->dma_addr), mem->dma_size);

    mem->dma_virt = osal_mem_kmap(mem->kernel_dma, 0, 0);
    if (mem->dma_virt == NULL) {
        hi_log_print_func_err(osal_mem_kmap, NULL);
        return HI_LOG_ERR_MEM;
    }

    osal_mem_flush(mem->kernel_dma);

    hi_log_info("crypto_mem_map()- via 0x%p\n", mem->dma_virt);

    hi_log_func_exit();

    return HI_SUCCESS;
}

/* brief unmap a mmz or smmu memory */
static hi_s32 crypto_mem_unmap(crypto_mem *mem)
{
    hi_log_func_enter();

    crypto_assert(mem != HI_NULL);

    hi_log_debug("crypto_mem_unmap()- dma 0x%x, size 0x%x\n",
                 ADDR_L32(mem->dma_addr), mem->dma_size);

    osal_mem_kunmap(mem->kernel_dma, mem->dma_virt, 0);

    hi_log_func_exit();
    return HI_SUCCESS;
}

void crypto_mem_init(void)
{
}

void crypto_mem_deinit(void)
{
}

hi_s32 crypto_mem_create(crypto_mem *mem, hi_u32 type, const char *name, hi_u32 size)
{
    crypto_assert(mem != HI_NULL);

    return crypto_mem_alloc_remap(mem, type, name, size);
}

hi_s32 crypto_mem_destory(crypto_mem *mem)
{
    crypto_assert(mem != HI_NULL);

    return crypto_mem_release_unmap(mem);
}

hi_s32 crypto_mem_open(crypto_mem *mem, compat_addr dma_addr, hi_u32 dma_size)
{
    crypto_assert(mem != HI_NULL);

    mem->dma_addr = dma_addr;
    mem->dma_size = dma_size;

    if (mem->dma_size == 0) {
        return HI_SUCCESS;
    }

    return crypto_mem_map(mem);
}

hi_s32 crypto_mem_close(crypto_mem *mem)
{
    crypto_assert(mem != HI_NULL);

    if (mem->dma_size == 0) {
        return HI_SUCCESS;
    }

    return crypto_mem_unmap(mem);
}

hi_s32 crypto_mem_attach(crypto_mem *mem, void *buffer)
{
    crypto_assert(mem != HI_NULL);

    mem->user_buf = buffer;

    return HI_SUCCESS;
}

hi_s32 crypto_mem_flush(crypto_mem *mem, hi_u32 dma2user, hi_u32 offset, hi_u32 data_size)
{
    hi_s32 ret = HI_FAILURE;

    crypto_assert(mem != HI_NULL);
    crypto_assert(mem->dma_virt != HI_NULL);
    crypto_assert(mem->user_buf != HI_NULL);
    crypto_assert(data_size <= mem->dma_size);

    if (dma2user != 0) {
        ret = memcpy_s((hi_u8 *)mem->user_buf + offset, data_size,
                       (hi_u8 *)mem->dma_virt + offset, data_size);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memcpy_s, ret);
            return ret;
        }
    } else {
        ret = memcpy_s((hi_u8 *)mem->dma_virt + offset, data_size,
                       (hi_u8 *)mem->user_buf + offset, data_size);
        if (ret != HI_SUCCESS) {
            hi_log_print_func_err(memcpy_s, ret);
            return ret;
        }
    }

    return HI_SUCCESS;
}

hi_s32 crypto_mem_phys(crypto_mem *mem, compat_addr *dma_addr)
{
    crypto_assert(mem != HI_NULL);

    dma_addr->phy = ADDR_U64(mem->dma_addr);

    return HI_SUCCESS;
}

void *crypto_mem_virt(crypto_mem *mem)
{
    if (mem == HI_NULL) {
        return HI_NULL;
    }

    return mem->dma_virt;
}

hi_s32 crypto_mem_try_create_max(const char *name, hi_u32 type, hi_u32 max, crypto_mem *mem)
{
    hi_s32 ret = HI_FAILURE;
    hi_u32 i = 0;

    hi_log_func_enter();

    /* Try to alloc memory, halve the length if failed */
    for (i = 0; i < PHY_MEM_CREATE_TRY_TIME; i++) {
        ret = crypto_mem_create(mem, type, name, max);
        if (ret == HI_SUCCESS) {
            return HI_SUCCESS;
        } else {
            /* halve the length */
            max /= 0x02;
        }
    }

    hi_log_func_exit();
    return HI_FAILURE;
}

hi_s32 crypto_copy_from_user(void *to, unsigned long to_len, const void *from, unsigned long from_len)
{
    if (from_len == 0) {
        return HI_SUCCESS;
    }

    hi_log_check_param(to == HI_NULL);
    hi_log_check_param(from == HI_NULL);
    hi_log_check_param(to_len < from_len);

    return osal_copy_from_user(to, from, from_len);
}

hi_s32 crypto_copy_to_user(void *to, unsigned long to_len, const void *from, unsigned long from_len)
{
    if (from_len == 0) {
        return HI_SUCCESS;
    }

    hi_log_check_param(to == HI_NULL);
    hi_log_check_param(from == HI_NULL);
    hi_log_check_param(to_len < from_len);

    return osal_copy_to_user(to, from, from_len);
}

hi_u32 crypto_is_sec_cpu(void)
{
    return module_get_secure();
}

void smmu_get_table_addr(hi_ulong *rdaddr, hi_ulong *wraddr, hi_ulong *table)
{
#ifdef CRYPTO_SMMU_SUPPORT
    osal_mem_get_nssmmu_pgtinfo(table, rdaddr, wraddr);
#else
    *rdaddr = 0x00;
    *wraddr = 0x00;
    *table = 0x00;
#endif
}

void crypto_load_symc_clean_key(hi_u32 chn_num, hi_u32 key[AES_IV_SIZE], hi_u32 odd)
{
    return;
}

/* Implementation that should never be optimized out by the compiler */
void crypto_zeroize(void *buf, u32 len)
{
    volatile unsigned char *p = (unsigned char *)buf;

    if (buf == HI_NULL) {
        return;
    }

    while (len--) {
        *p++ = 0;
    }
}

hi_u64 crypto_bufhandle_to_phys(hi_cipher_mem_handle bufhandle)
{
    struct dma_buf *dmabuf = HI_NULL;
    hi_u64 bufphys;
    if (bufhandle.cipher_mem_handle == 0) {
        return 0x00;
    }
    dmabuf = osal_mem_handle_get(bufhandle.cipher_mem_handle, HI_ID_CIPHER);
#ifdef HI_SMMU_SUPPORT
    bufphys = osal_mem_nssmmu_map(dmabuf, 0) + bufhandle.cipher_addr_offset;
#else
    bufphys = osal_mem_phys(dmabuf) + bufhandle.cipher_addr_offset;
#endif
    return bufphys;
}

hi_s32 crypto_queue_callback_func(const void *param)
{
    if (*(hi_bool *)param != HI_FALSE) {
        return 1;
    }
    return 0;
}

/** @} */ /** <!-- ==== API Code end ==== */
