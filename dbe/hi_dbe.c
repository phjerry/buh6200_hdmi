/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2015-2019. All rights reserved.
 * Description: wrap GPU external buffer into dmabuf fd
 * Author: guofeng
 * Create: 2015-09-29
 *
 * This program is confidential and proprietary to Hisilicon  Technologies Co.,
 * Ltd. (Hisilicon), and may not be copied, reproduced, modified, disclosed to
 * others, published or used, in whole or in part, without the express prior
 * written permission of Hisilicon.
 */

#include "hi_dbe.h"
#include "hi_debug.h"

#include <linux/dma-buf.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/atomic.h>

#include <linux/hisilicon/hisi_iommu.h>
#include <linux/hisilicon/securec.h>

#define HI_DBE_PAGE_SIZE 4096
#define HI_DBE_MAX_SIZE  0x40000000

#define dbe_error() \
    pr_err("DBE: Error, func = %s, line = %d\n", __func__, __LINE__)

struct hidbe_alloc {
    /* the real alloc */
    int dbe_pages;
    struct page **pages;

    /* the debug usage tracking */
    int dbe_attached_devices;
    int dbe_device_mappings;
    int dbe_cpu_mappings;

    bool bde_contiguous;
    dma_addr_t dbe_contig_dma_addr;
};

static struct miscdevice g_dbe_device;

static int hidbe_attach(struct dma_buf *pdbe_buf, struct device *pdbe_dev,
                        struct dma_buf_attachment *pdbe_attachment)
{
    struct hidbe_alloc *pdbe_alloc = NULL;

    if ((pdbe_buf == NULL) || (pdbe_buf->priv == NULL)) {
        dbe_error();
        return -EFAULT;
    }

    pdbe_alloc = pdbe_buf->priv;
    /* dma_buf is externally locked during call */
    pdbe_alloc->dbe_attached_devices++;

    return 0;
}

static void hidbe_detach(struct dma_buf *pdbe_buf,
                         struct dma_buf_attachment *pdbe_attachment)
{
    struct hidbe_alloc *pdbe_alloc = NULL;

    if ((pdbe_buf == NULL) || (pdbe_buf->priv == NULL)) {
        dbe_error();
        return;
    }

    pdbe_alloc = pdbe_buf->priv;

    /* dma_buf is externally locked during call */
    pdbe_alloc->dbe_attached_devices--;
}

static struct sg_table *hidbe_map(struct dma_buf_attachment *pdbe_attachment, enum dma_data_direction dbe_direction)
{
    struct sg_table *pdbe_sg = NULL;
    struct scatterlist *pdbe_iter = NULL;
    struct hidbe_alloc *pdbe_alloc = NULL;
    int i;
    int ret;

    if ((pdbe_attachment == NULL) || (pdbe_attachment->dmabuf == NULL) || (pdbe_attachment->dmabuf->priv == NULL)) {
        dbe_error();
        return ERR_PTR(-EFAULT);
    }

    pdbe_alloc = pdbe_attachment->dmabuf->priv;

    /*
     * if the ARCH can't chain we can't have allocs larger than a single
     * sg can hold
     */
    pdbe_sg = kmalloc(sizeof(struct sg_table), GFP_KERNEL);
    if (pdbe_sg == NULL) {
        return ERR_PTR(-ENOMEM);
    }

    if (memset_s(pdbe_sg, sizeof(struct sg_table), 0, sizeof(struct sg_table)) != 0) {
        return ERR_PTR(-EFAULT);
    }

    /*
     * from here we access the allocation object, so lock the
     * dmabuf pointing to it
     */
    mutex_lock(&pdbe_attachment->dmabuf->lock);

    if (pdbe_alloc->bde_contiguous) {
        ret = sg_alloc_table(pdbe_sg, 1, GFP_KERNEL);
    } else {
        ret = sg_alloc_table(pdbe_sg, pdbe_alloc->dbe_pages, GFP_KERNEL);
    }

    if (ret) {
        mutex_unlock(&pdbe_attachment->dmabuf->lock);
        kfree(pdbe_sg);
        return ERR_PTR(ret);
    }

    if (pdbe_alloc->bde_contiguous) {
        sg_dma_len(pdbe_sg->sgl) = pdbe_alloc->dbe_pages * PAGE_SIZE;
        sg_set_page(pdbe_sg->sgl, pfn_to_page(PFN_DOWN(pdbe_alloc->dbe_contig_dma_addr)),
                    pdbe_alloc->dbe_pages * PAGE_SIZE, 0);
        sg_dma_address(pdbe_sg->sgl) = pdbe_alloc->dbe_contig_dma_addr;
    } else {
        for_each_sg(pdbe_sg->sgl, pdbe_iter, pdbe_alloc->dbe_pages, i)
            sg_set_page(pdbe_iter, pdbe_alloc->pages[i], PAGE_SIZE, 0);
    }

    if (!dma_map_sg(pdbe_attachment->dev, pdbe_sg->sgl, pdbe_sg->nents, dbe_direction)) {
        mutex_unlock(&pdbe_attachment->dmabuf->lock);
        sg_free_table(pdbe_sg);
        kfree(pdbe_sg);
        return ERR_PTR(-ENOMEM);
    }

    pdbe_alloc->dbe_device_mappings++;
    mutex_unlock(&pdbe_attachment->dmabuf->lock);
    return pdbe_sg;
}

static void hidbe_unmap(struct dma_buf_attachment *pdbe_attachment,
                        struct sg_table *pdbe_sg, enum dma_data_direction dbe_direction)
{
    struct hidbe_alloc *pdbe_alloc = NULL;

    if ((pdbe_attachment == NULL) || (pdbe_attachment->dmabuf == NULL) || (pdbe_attachment->dmabuf->priv == NULL)) {
        dbe_error();
        return;
    }

    pdbe_alloc = pdbe_attachment->dmabuf->priv;

    dma_unmap_sg(pdbe_attachment->dev, pdbe_sg->sgl,
                 pdbe_sg->nents, dbe_direction);
    sg_free_table(pdbe_sg);
    kfree(pdbe_sg);

    mutex_lock(&pdbe_attachment->dmabuf->lock);
    pdbe_alloc->dbe_device_mappings--;
    mutex_unlock(&pdbe_attachment->dmabuf->lock);
}

static void hidbe_release(struct dma_buf *pdbe_buf)
{
    struct hidbe_alloc *pdbe_alloc = NULL;

    if ((pdbe_buf == NULL) || (pdbe_buf->priv == NULL)) {
        dbe_error();
        return;
    }

    pdbe_alloc = pdbe_buf->priv;

    kfree(pdbe_alloc->pages);
    kfree(pdbe_alloc);
}

static void hidbe_mmap_open(struct vm_area_struct *pdbe_vma)
{
    struct dma_buf *pdbe_dma_buf = NULL;
    struct hidbe_alloc *pdbe_alloc = NULL;

    if ((pdbe_vma == NULL) || (pdbe_vma->vm_private_data == NULL)) {
        dbe_error();
        return;
    }

    pdbe_dma_buf = pdbe_vma->vm_private_data;
    pdbe_alloc = pdbe_dma_buf->priv;

    if (pdbe_alloc == NULL) {
        dbe_error();
        return;
    }

    mutex_lock(&pdbe_dma_buf->lock);
    pdbe_alloc->dbe_cpu_mappings++;
    mutex_unlock(&pdbe_dma_buf->lock);
}

static void hidbe_mmap_close(struct vm_area_struct *pdbe_vma)
{
    struct dma_buf *pdbe_dma_buf = NULL;
    struct hidbe_alloc *pdbe_alloc = NULL;

    if ((pdbe_vma == NULL) || (pdbe_vma->vm_private_data == NULL)) {
        dbe_error();
        return;
    }

    pdbe_dma_buf = pdbe_vma->vm_private_data;
    pdbe_alloc = pdbe_dma_buf->priv;

    if (pdbe_alloc == NULL) {
        dbe_error();
        return;
    }

    if (pdbe_alloc->dbe_cpu_mappings <= 0) {
        dbe_error();
        return;
    }

    mutex_lock(&pdbe_dma_buf->lock);
    pdbe_alloc->dbe_cpu_mappings--;
    mutex_unlock(&pdbe_dma_buf->lock);
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
static int hidbe_mmap_fault(struct vm_area_struct *pdbe_vma,
                            struct vm_fault *pdbe_vmf)
#else
static int hidbe_mmap_fault(struct vm_fault *pdbe_vmf)
#endif
{
    struct hidbe_alloc *pdbe_alloc = NULL;
    struct dma_buf *pdbe_dmabuf = NULL;
    struct page *pdbe_pageptr = NULL;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
    if ((pdbe_vma == NULL) || (pdbe_vma->vm_private_data == NULL)) {
        dbe_error();
        return -EFAULT;
    }

    pdbe_dmabuf = pdbe_vma->vm_private_data;
#else
    if ((pdbe_vmf == NULL) || (pdbe_vmf->vma == NULL)) {
        dbe_error();
        return -EFAULT;
    }

    pdbe_dmabuf = pdbe_vmf->vma->vm_private_data;
#endif
    pdbe_alloc = pdbe_dmabuf->priv;
    if (pdbe_alloc == NULL) {
        dbe_error();
        return -EFAULT;
    }

    if (pdbe_vmf->pgoff > pdbe_alloc->dbe_pages) {
        return VM_FAULT_SIGBUS;
    }

    pdbe_pageptr = pdbe_alloc->pages[pdbe_vmf->pgoff];

    if (pdbe_pageptr == NULL) {
        return -EFAULT;
    }

    get_page(pdbe_pageptr);
    pdbe_vmf->page = pdbe_pageptr;

    return 0;
}

struct vm_operations_struct g_dbe_vm_ops = {
    .open = hidbe_mmap_open,
    .close = hidbe_mmap_close,
    .fault = hidbe_mmap_fault
};

static int hidbe_mmap(struct dma_buf *pdbe_dmabuf,
                      struct vm_area_struct *pdbe_vma)
{
    if ((pdbe_dmabuf == NULL) || (pdbe_vma == NULL)) {
        dbe_error();
        return -EFAULT;
    }

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 7, 0))
    pdbe_vma->vm_flags |= VM_IO | VM_DONTEXPAND | VM_DONTDUMP;
#else
    pdbe_vma->vm_flags |= VM_RESERVED | VM_IO | VM_DONTEXPAND;
#endif
    pdbe_vma->vm_ops = &g_dbe_vm_ops;
    pdbe_vma->vm_private_data = pdbe_dmabuf;

    /* we fault in the pages on access call open to do the ref-counting */
    g_dbe_vm_ops.open(pdbe_vma);

    return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
static void *hidbe_kmap_atomic(struct dma_buf *pdbe_buf,
                               unsigned long dbe_page_num)
{
    /* IGNORE */
    return NULL;
}
#endif

static void *hidbe_kmap(struct dma_buf *pdbe_buf, unsigned long dbe_page_num)
{
    /* IGNORE */
    return NULL;
}

static void hidbe_kunmap(struct dma_buf *buf,
                         unsigned long page_num, void *addr)
{
    /* IGNORE */
    return;
}

static struct dma_buf_ops g_dbe_ops = {
    /* real handlers */
    .attach = hidbe_attach,
    .detach = hidbe_detach,
    .map_dma_buf = hidbe_map,
    .unmap_dma_buf = hidbe_unmap,
    .release = hidbe_release,
    .mmap = hidbe_mmap,

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 12, 0)
    .kmap = hidbe_kmap,
    .kunmap = hidbe_kunmap,
    .kmap_atomic = hidbe_kmap_atomic
#else
    .map = hidbe_kmap,
    .unmap = hidbe_kunmap,

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
    .map_atomic = hidbe_kmap_atomic
#endif

#endif
};

static int hidbe_ioctl_wrap_func(struct hidbe_ioctl_wrap __user *pdbe_buf)
{
    struct hidbe_ioctl_wrap dbe_alloc_req;
    struct hidbe_alloc *pdbe_alloc = NULL;
    struct dma_buf *pdbe_dma_buf = NULL;
    int i = 0;
    int fd = -1;
    dma_addr_t phy_addr;
    dma_addr_t mmu_addr;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)
    struct dma_buf_export_info exp_info;
#endif

    if (pdbe_buf == NULL) {
        dev_err(g_dbe_device.this_device,
                "%s: input is null", __func__);
        goto no_input;
    }

    if (copy_from_user(&dbe_alloc_req, pdbe_buf, sizeof(dbe_alloc_req))) {
        dev_err(g_dbe_device.this_device,
                "%s: couldn't get user data", __func__);
        goto no_input;
    }

    /* buffer address and size should be 0x1000 - 0xffffffff */
    if (dbe_alloc_req.dbe_size <= 0 ||
        dbe_alloc_req.dbe_size > HI_DBE_MAX_SIZE) {
        dev_err(g_dbe_device.this_device,
                "%s: no size specified", __func__);
        goto invalid_size;
    }

    if (dbe_alloc_req.dbe_phyaddr <= 0 ||
        dbe_alloc_req.dbe_phyaddr > HI_DBE_MAX_SIZE) {
        dev_err(g_dbe_device.this_device,
                "%s: no address specified", __func__);
        goto invalid_size;
    }

    /* buffer size need to transform into page size */
    dbe_alloc_req.dbe_size =
        (dbe_alloc_req.dbe_size + HI_DBE_PAGE_SIZE - 1) /
        HI_DBE_PAGE_SIZE;

    /* buffer physical address must be 4k aligned */
    if (dbe_alloc_req.dbe_phyaddr % HI_DBE_PAGE_SIZE) {
        dev_err(g_dbe_device.this_device,
                "%s: physical address is not 4K aligned", __func__);
        goto invalid_phyaddr;
    }

    pdbe_alloc = kzalloc(sizeof(struct hidbe_alloc), GFP_KERNEL);
    if (pdbe_alloc == NULL) {
        dev_err(g_dbe_device.this_device,
                "%s: couldn't alloc object", __func__);
        goto no_alloc_object;
    }

    pdbe_alloc->dbe_pages = dbe_alloc_req.dbe_size;
    pdbe_alloc->bde_contiguous = !dbe_alloc_req.dbe_smmuflag;

    pdbe_alloc->pages = kcalloc(pdbe_alloc->dbe_pages, sizeof(char *),
                                GFP_KERNEL);
    if (pdbe_alloc->pages == NULL) {
        dev_err(g_dbe_device.this_device,
                "%s: couldn't alloc %d page structures",
                __func__, pdbe_alloc->dbe_pages);
        goto free_alloc_object;
    }

    if (dbe_alloc_req.dbe_smmuflag == 1) {
        mmu_addr = dbe_alloc_req.dbe_phyaddr;

        for (i = 0; i < pdbe_alloc->dbe_pages; i++) {
            phy_addr = hisi_iommu_domain_iova_to_phys(mmu_addr);

            /* when mmu_addr  is unnormal */
            if (phy_addr == 0) {
                goto no_export;
            }

            pdbe_alloc->pages[i] = pfn_to_page(PFN_DOWN(phy_addr));
            mmu_addr += PAGE_SIZE;
        }
    } else {
        phy_addr = dbe_alloc_req.dbe_phyaddr;

        for (i = 0; i < pdbe_alloc->dbe_pages; i++) {
            pdbe_alloc->pages[i] = pfn_to_page(PFN_DOWN(phy_addr));
            phy_addr += PAGE_SIZE;
        }

        pdbe_alloc->dbe_contig_dma_addr = dbe_alloc_req.dbe_phyaddr;
    }

    /* alloc ready, let's export it */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)
    exp_info.priv = (void *)pdbe_alloc;
    exp_info.ops = &g_dbe_ops;
    exp_info.size = ((unsigned int)pdbe_alloc->dbe_pages) << PAGE_SHIFT;
    exp_info.flags = O_CLOEXEC | O_RDWR;
    exp_info.resv = NULL;
    exp_info.owner = NULL;
    exp_info.exp_name = NULL;

    pdbe_dma_buf = dma_buf_export(&exp_info);

#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 17, 0)) && (LINUX_VERSION_CODE < KERNEL_VERSION(4, 4, 0))
    pdbe_dma_buf = dma_buf_export(pdbe_alloc, &g_dbe_ops,
                                  pdbe_alloc->dbe_pages << PAGE_SHIFT, O_CLOEXEC | O_RDWR, NULL);
#else
    pdbe_dma_buf = dma_buf_export(pdbe_alloc, &g_dbe_ops,
                                  pdbe_alloc->dbe_pages << PAGE_SHIFT, O_CLOEXEC | O_RDWR);
#endif

    if (IS_ERR_OR_NULL(pdbe_dma_buf)) {
        dev_err(g_dbe_device.this_device,
                "%s: couldn't export dma_buf", __func__);
        goto no_export;
    }

    /* get fd for buf */
    fd = dma_buf_fd(pdbe_dma_buf, O_CLOEXEC);

    if (fd < 0) {
        dev_err(g_dbe_device.this_device,
                "%s: couldn't get fd from dma_buf", __func__);
        goto no_fd;
    }

    return fd;

no_fd:
    dma_buf_put(pdbe_dma_buf);

no_export:
    kfree(pdbe_alloc->pages);

free_alloc_object:
    kfree(pdbe_alloc);

no_alloc_object:
invalid_size:
invalid_phyaddr:
no_input:
    return -EFAULT;
}

static long hidbe_ioctl(struct file *pdbe_file, unsigned int dbe_cmd,
                        unsigned long dbe_arg)
{
    switch (dbe_cmd) {
        /* support physical address and smmu address */
        case DBE_COMMAND_WRAP_ADDRESS:
            /* fall-through */
            return hidbe_ioctl_wrap_func ((struct hidbe_ioctl_wrap __user *)(uintptr_t) dbe_arg);

        default:
            return -ENOTTY;
    }
}

static const struct file_operations g_dbe_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = hidbe_ioctl,
    .compat_ioctl = hidbe_ioctl,
};

static int __init hidbe_init(void)
{
    int res;

    g_dbe_device.minor = MISC_DYNAMIC_MINOR;
    g_dbe_device.name = "hi_dbe";
    g_dbe_device.fops = &g_dbe_fops;

    res = misc_register(&g_dbe_device);
    if (res) {
        HI_PRINT("Misc device registration failed of 'hi_dbe'\n");
        return res;
    }
    g_dbe_device.this_device->coherent_dma_mask = DMA_BIT_MASK(32);

    HI_PRINT("Load hi_dbe.ko success.  \t(%s)\n", VERSION_STRING);

    return 0;
}

static void __exit hidbe_exit(void)
{
    misc_deregister(&g_dbe_device);

    HI_PRINT("remove hi_dbe.ko success.\n");
}

module_init(hidbe_init);
module_exit(hidbe_exit);
MODULE_LICENSE("GPL");

