/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:osal_math
* Author: seg
* Create: 2019-10-11
*/

#include "hi_osal.h"
#include "hi_module.h"
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/mm.h>
#include <linux/memblock.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
#ifndef CONFIG_64BIT
#include <mach/io.h>
#endif
#endif

unsigned int g_kmalloc_used[HI_ID_MAX] = {0};
unsigned int g_vmalloc_used[HI_ID_MAX] = {0};

unsigned int osal_get_kmalloc_used(unsigned int module_id)
{
    if (module_id >= HI_ID_MAX) {
        return 0;
    }

    return g_kmalloc_used[module_id];
}

unsigned int osal_get_vmalloc_used(unsigned int module_id)
{
    if (module_id >= HI_ID_MAX) {
        return 0;
    }

    return g_vmalloc_used[module_id];
}

static unsigned int osal_convert_gfp_flag(unsigned int osal_gfp_flag)
{
    unsigned int gfp_flag;

    switch (osal_gfp_flag) {
        case OSAL_GFP_KERNEL:
            gfp_flag = GFP_KERNEL;
            break;

        case OSAL_GFP_ATOMIC:
            gfp_flag = GFP_ATOMIC;
            break;

        case OSAL_GFP_DMA:
            gfp_flag = GFP_DMA;
            break;

        case OSAL_GFP_ZERO:
            gfp_flag = __GFP_ZERO;
            break;

        default:
            gfp_flag = GFP_KERNEL;
            break;
    }

    return gfp_flag;
}

void *osal_kmalloc(unsigned int module_id, unsigned long size, unsigned int osal_gfp_flag)
{
    unsigned int gfp_flag;

    if (module_id >= HI_ID_MAX) {
        printk("osal_kmalloc module id(%ud) is wrong\n", module_id);
        osal_dump_stack();
        return NULL;
    }

    g_kmalloc_used[module_id]++;
    gfp_flag = osal_convert_gfp_flag(osal_gfp_flag);
    return kmalloc(size, gfp_flag);
}
EXPORT_SYMBOL(osal_kmalloc);

void osal_kfree(unsigned int module_id, const void *addr)
{
    if (module_id >= HI_ID_MAX) {
        printk("osal_kfree module id(%ud) is wrong\n", module_id);
        osal_dump_stack();
        return;
    }

    if (addr != NULL) {
        g_kmalloc_used[module_id]--;
        kfree(addr);
    }
    return;
}
EXPORT_SYMBOL(osal_kfree);

void *osal_vmalloc(unsigned int module_id, unsigned long size)
{
    if (module_id >= HI_ID_MAX) {
        printk("osal_vmalloc module id(%ud) is wrong\n", module_id);
        osal_dump_stack();
        return NULL;
    }

    g_vmalloc_used[module_id]++;

    return vmalloc(size);
}

EXPORT_SYMBOL(osal_vmalloc);

void osal_vfree(unsigned int module_id, const void *addr)
{
    if (module_id >= HI_ID_MAX) {
        printk("osal_vfree module id(%ud) is wrong\n", module_id);
        osal_dump_stack();
        return;
    }

    if (addr != NULL) {
        g_vmalloc_used[module_id]--;
        vfree(addr);
    }
    return;
}

EXPORT_SYMBOL(osal_vfree);

void *osal_ioremap(unsigned long phys_addr, unsigned long size)
{
    return ioremap(phys_addr, size);
}
EXPORT_SYMBOL(osal_ioremap);

void *osal_ioremap_nocache(unsigned long phys_addr, unsigned long size)
{
    return ioremap_nocache(phys_addr, size);
}
EXPORT_SYMBOL(osal_ioremap_nocache);

void *osal_ioremap_cached(unsigned long phys_addr, unsigned long size)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 18, 0)
    return ioremap_cached(phys_addr, size);
#else
    return ioremap_cache(phys_addr, size);
#endif
}
EXPORT_SYMBOL(osal_ioremap_cached);

void osal_iounmap(void *addr)
{
    iounmap(addr);
}
EXPORT_SYMBOL(osal_iounmap);

unsigned long osal_copy_from_user(void *to, const void *from, unsigned long n)
{
    return copy_from_user(to, from, n);
}
EXPORT_SYMBOL(osal_copy_from_user);

unsigned long osal_copy_to_user(void *to, const void *from, unsigned long n)
{
    return copy_to_user(to, from, n);
}
EXPORT_SYMBOL(osal_copy_to_user);

int osal_access_ok(int type, const void *addr, unsigned long size)
{
    uintptr_t uaddr = (uintptr_t)addr;
    return access_ok(type, uaddr, size);
}
EXPORT_SYMBOL(osal_access_ok);

void *osal_phys_to_virt(unsigned long addr)
{
    return phys_to_virt(addr);
}
EXPORT_SYMBOL(osal_phys_to_virt);

unsigned long osal_virt_to_phys(const void *virt_addr)
{
    return virt_to_phys(virt_addr);
}
EXPORT_SYMBOL(osal_virt_to_phys);

/*
 * Maps @size from @phys_addr into contiguous kernel virtual space
 * Note:this function only support VM_MAP with PAGE_KERNEL flag
 * */
void *osal_blockmem_vmap(unsigned long phys_addr, unsigned long size)
{
    unsigned int i = 0;
    unsigned int page_count;
    struct page **pages = NULL;
    void *vaddr = NULL;

    if ((phys_addr == 0) || (size == 0)) {
        printk("invalid vmap address: 0x%lX or size:%lu\n", phys_addr, size);
        return NULL;
    }

    page_count = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    pages = vmalloc(page_count * sizeof(struct page *));
    if (!pages) {
        printk("vmap malloc pages failed\n");
        return NULL;
    }

    for (i = 0; i < page_count; i++) {
        pages[i] = phys_to_page(phys_addr + i * PAGE_SIZE);
    }

    vaddr = vmap(pages, page_count, VM_MAP, PAGE_KERNEL);
    if (!vaddr) {
        printk("vmap failed phys_addr:0x%lX, size:%lu\n", phys_addr, size);
    }

    vfree(pages);
    pages = NULL;

    return vaddr;
}
EXPORT_SYMBOL(osal_blockmem_vmap);

/*
 * Free the virtually contiguous memory area starting at @virt_addr
 * which was created from the phys_addr passed to osal_vunmap()
 * Must not be called in interrupt context.
 * */
void osal_blockmem_vunmap(const void *virt_addr)
{
    if (!virt_addr) {
        printk("vumap failed: virt_addr is NULL\n");
        return;
    }

    vunmap(virt_addr);
}
EXPORT_SYMBOL(osal_blockmem_vunmap);

/*
 * Free the reserved memory which has been defined in product
 **/
void osal_blockmem_free(unsigned long phys_addr, unsigned long size)
{
    unsigned int pfn_start;
    unsigned int pfn_end;

    if ((phys_addr == 0) || (size == 0)) {
        printk("Free block memory failed: phys_addr 0x%lX,size %lu\n",
            phys_addr, size);
        return;
    }

    pfn_start = __phys_to_pfn(phys_addr);
    pfn_end = __phys_to_pfn(phys_addr + size);

    for (; pfn_start < pfn_end; pfn_start++) {
        struct page *page = pfn_to_page(pfn_start);
        ClearPageReserved(page);
        init_page_count(page);
        __free_page(page);
        adjust_managed_page_count(page, 1); /* 1 block mem page count */
    }

#ifndef CONFIG_ARCH_DISCARD_MEMBLOCK
    /* kernel need to export this symbol : (void)memblock_free(phys_addr, size); */
#endif

    return;
}
EXPORT_SYMBOL(osal_blockmem_free);
