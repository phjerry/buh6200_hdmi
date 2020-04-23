#include <linux/types.h>
#include <linux/ion.h>
#include <linux/dma-buf.h>
#include "hi_osal.h"

/* alloc  */
void *osal_mem_alloc(const char *buf_name, unsigned long len, osal_mem_type mem_type,
                     void *priv_data, unsigned long priv_len)
{
    struct dma_buf *dmabuf = NULL;
    unsigned int heap_id_mask;

    switch (mem_type) {
        case OSAL_MMZ_TYPE: heap_id_mask = 1 << ION_HEAP_ID_CMA; break;
        case OSAL_NSSMMU_TYPE: heap_id_mask = 1 << ION_HEAP_ID_SMMU; break;
        case OSAL_SECSMMU_TYPE: heap_id_mask = 1 << ION_HEAP_ID_SEC_SMMU; break;
        case OSAL_SECMMZ_TYPE: heap_id_mask = 1 << ION_HEAP_ID_SEC_CMA; break;
        default:
        {
            pr_err("err mem type(%d).\n", mem_type);
            return NULL;
        }
    }

    dmabuf = hi_ion_alloc(len, heap_id_mask, 0, buf_name, priv_data, priv_len);
    if (IS_ERR_OR_NULL(dmabuf)) {
        pr_err("osal mem alloc failed!\n");
        return NULL;
    }
    return (void *)dmabuf;
}

/* free  */
void osal_mem_free(void *handle)
{
    struct dma_buf *dmabuf = NULL;

    if (IS_ERR_OR_NULL(handle)) {
        pr_err("osal mem free failed!\n");
        return;
    }

    dmabuf = (struct dma_buf *) handle;
    hi_ion_free(dmabuf);
}

/* map cpu addr  */
void *osal_mem_kmap(void *handle, unsigned long offset, int cache)
{
    void *virt = NULL;
    struct dma_buf *dmabuf = NULL;
    int ret;

    if (IS_ERR_OR_NULL(handle)) {
        pr_err("%s, osal err args!\n", __func__);
        return NULL;
    }
    dmabuf = (struct dma_buf *)handle;
    ret = set_buffer_cached(dmabuf, cache);
    if (ret) {
        pr_err("osal set cache attr failed!\n");
        return NULL;
    }

    virt = dma_buf_kmap(dmabuf, offset >> PAGE_SHIFT);
    if (!virt) {
        set_buffer_cached(dmabuf, !cache);
        pr_err("osal map failed!\n");
        return NULL;
    }

    return virt;
}

/* unmap cpu addr  */
void osal_mem_kunmap(void *handle, void *virt, unsigned long offset)
{
    struct dma_buf *dmabuf = NULL;

    if (IS_ERR_OR_NULL(handle) || !virt) {
        pr_err("%s, osal err args!\n", __func__);
        return;
    }

    dmabuf = (struct dma_buf *)handle;
    dma_buf_kunmap(dmabuf, offset >> PAGE_SHIFT, virt);
}

/* map nssmmu addr */
unsigned long osal_mem_nssmmu_map(void *handle, unsigned long offset)
{
    struct dma_buf *dmabuf = NULL;
    unsigned long nssmmu;

    if (IS_ERR_OR_NULL(handle)) {
        pr_err("%s, osal err args!\n", __func__);
        return 0;
    }

    dmabuf = (struct dma_buf *)handle;
    nssmmu = hi_dma_buf_nssmmu_map(dmabuf, offset >> PAGE_SHIFT);
    if (!nssmmu) {
        pr_err("osal map nssmmu failed!\n");
        return 0;
    }

    return nssmmu;
}

/* unmap nssmmu addr*/
int osal_mem_nssmmu_unmap(void *handle, unsigned long nssmmu, unsigned long offset)
{
    struct dma_buf *dmabuf = NULL;
    int ret;

    if (IS_ERR_OR_NULL(handle)) {
        pr_err("%s, osal err args!\n", __func__);
        return -1;
    }

    dmabuf = (struct dma_buf *)handle;
    ret = hi_dma_buf_nssmmu_unmap(dmabuf, offset >> PAGE_SHIFT, nssmmu);
    if (ret) {
        pr_err("osal unmap nssmmu failed!\n");
        return ret;
    }

    return 0;
}

/* map secsmmu addr*/
unsigned long osal_mem_secsmmu_map(void *handle, unsigned long offset)
{
    struct dma_buf *dmabuf = NULL;
    unsigned long secsmmu;

    if (IS_ERR_OR_NULL(handle)) {
        pr_err("%s, osal err args!\n", __func__);
        return 0;
    }

    dmabuf = (struct dma_buf *)handle;
    secsmmu = hi_dma_buf_secsmmu_map(dmabuf, offset >> PAGE_SHIFT);
    if (!secsmmu) {
        pr_err("osal map secsmmu failed!\n");
        return 0;
    }

    return secsmmu;
}

/* unmap secsmmu */
int osal_mem_secsmmu_unmap(void *handle, unsigned long secsmmu, unsigned long offset)
{
    struct dma_buf *dmabuf = NULL;
    int ret;

    if (IS_ERR_OR_NULL(handle)) {
        pr_err("%s, osal err args!\n", __func__);
        return 0;
    }

    dmabuf = (struct dma_buf *)handle;
    ret = hi_dma_buf_secsmmu_unmap(dmabuf, offset >> PAGE_SHIFT, secsmmu);
    if (ret) {
        pr_err("osal unmap secsmmu failed!\n");
    }

    return ret;
}

/* get phys_addr by handle */
unsigned long osal_mem_phys(void *handle)
{
    struct dma_buf *dmabuf = NULL;
    unsigned long phys_addr;

    if (IS_ERR_OR_NULL(handle)) {
        pr_err("%s, osal err args!\n", __func__);
        return 0;
    }

    dmabuf = (struct dma_buf *)handle;
    phys_addr = hi_dma_buf_phy_get(dmabuf);
    if (!phys_addr) {
        pr_err("osal get phys addr failed!\n");
        return 0;
    }

    return phys_addr;
}

/* create fd  */
long osal_mem_create_fd(void *handle, int flags)
{
    long fd;
    struct dma_buf *dmabuf = NULL;

    if (IS_ERR_OR_NULL(handle)) {
        pr_err("%s, osal err args!\n", __func__);
        return 0;
    }

    dmabuf = (struct dma_buf *)handle;
    fd = hi_dma_buf_fd(dmabuf, flags);
    if (fd < 0) {
        pr_err("osal create fd failed!\n");
    }
    return fd;
}

/* close fd  */
int osal_mem_close_fd(long fd)
{
    return hi_close_fd(fd);
}

/*
 * The osal_mem_handle_get is to get handle by fd and
 * handle refcount increased at the same time. At last,
 * the refcount should be decreased by osal_mem_ref_put().
 */
void *osal_mem_handle_get(long fd, unsigned int module_id)
{
    struct dma_buf *dmabuf = NULL;

    dmabuf = dma_buf_get(fd);
    if (IS_ERR_OR_NULL(dmabuf)) {
        pr_err("osal get handle failed!\n");
        return NULL;
    }

    pr_debug("%s: module_id (%d)get handle,ref:%pa,!\n", __func__,
             module_id, &(dmabuf->file->f_count.counter));

    return (void *)dmabuf;
}

/* mem ref ++  */
int osal_mem_ref_get(void *handle, unsigned int module_id)
{
    struct dma_buf *dmabuf = NULL;

    if (IS_ERR_OR_NULL(handle)) {
        pr_err("%s, osal err args!\n", __func__);
        return -1;
    }

    dmabuf = (struct dma_buf *)handle;
    get_dma_buf(dmabuf);
    pr_debug("%s: module_id (%d)get handle,ref:%pa,!\n", __func__,
             module_id, &(dmabuf->file->f_count.counter));

    return 0;
}

/* mem ref --  */
void osal_mem_ref_put(void *handle, unsigned int module_id)
{
    struct dma_buf *dmabuf = NULL;

    if (IS_ERR_OR_NULL(handle)) {
        pr_err("%s, osal err args!\n", __func__);
        return;
    }

    dmabuf = (struct dma_buf *)handle;
    dma_buf_put(dmabuf);
    pr_debug("%s: module_id (%d) put handle,ref:%pa,!\n", __func__,
             module_id, &(dmabuf->file->f_count.counter));

    return;
}

/* flush mem*/
void osal_mem_flush(void *handle)
{
    struct dma_buf *dmabuf = NULL;
    int ret;

    if (IS_ERR_OR_NULL(handle)) {
        pr_err("%s, osal err args!\n", __func__);
        return;
    }
    dmabuf = (struct dma_buf *)handle;

    ret = dma_buf_begin_cpu_access(dmabuf, DMA_FROM_DEVICE);
    if (ret) {
        pr_err("%s, osal flush failed!\n", __func__);
    }
    return;
}

/* check buffer attr */
osal_mem_type osal_mem_get_attr(void *handle)
{
    struct dma_buf *dmabuf = NULL;
    unsigned int heap_id;
    osal_mem_type mem_type;

    if (IS_ERR_OR_NULL(handle)) {
        pr_err("%s, osal err args!\n", __func__);
        return OSAL_ERROR_TYPE;
    }
    dmabuf = (struct dma_buf *)handle;
    heap_id = get_heap_id_mask(dmabuf);

    switch (heap_id) {
        case ION_HEAP_ID_CMA: mem_type = OSAL_MMZ_TYPE; break;
        case ION_HEAP_ID_SMMU: mem_type = OSAL_NSSMMU_TYPE; break;
        case ION_HEAP_ID_SEC_SMMU: mem_type = OSAL_SECSMMU_TYPE; break;
        case ION_HEAP_ID_SEC_CMA: mem_type = OSAL_SECMMZ_TYPE;break;
        default: mem_type = OSAL_ERROR_TYPE; break;
    }

    return mem_type;
}

/* get mem handle by addr  */
void *osal_mem_get_handle(unsigned long addr, unsigned long size, osal_mem_type mem_type)
{
    struct dma_buf *dmabuf = NULL;
    unsigned int heap_id;

    switch (mem_type) {
        case OSAL_MMZ_TYPE: heap_id = ION_HEAP_ID_CMA; break;
        case OSAL_NSSMMU_TYPE: heap_id =  ION_HEAP_ID_SMMU; break;
        case OSAL_SECSMMU_TYPE: heap_id = ION_HEAP_ID_SEC_SMMU; break;
        case OSAL_SECMMZ_TYPE: heap_id =  ION_HEAP_ID_SEC_CMA; break;
        default:
        {
            pr_err("err mem type(%d).\n", mem_type);
            return NULL;
        }
    }

    dmabuf = hi_ion_get_dmabuf(addr, size, heap_id);
    if (!dmabuf) {
        pr_err("%s: osal get handle failed!\n", __func__);
        return NULL;
    }

    return (void *)dmabuf;
}

void osal_mem_get_nssmmu_pgtinfo(unsigned long *pt_addr,
                                 unsigned long *err_rd_addr,
                                 unsigned long *err_wr_addr)
{
    hi_drv_nssmmu_get_page_table_addr(pt_addr, err_rd_addr, err_wr_addr);
}

EXPORT_SYMBOL(osal_mem_alloc);
EXPORT_SYMBOL(osal_mem_free);
EXPORT_SYMBOL(osal_mem_kmap);
EXPORT_SYMBOL(osal_mem_kunmap);
EXPORT_SYMBOL(osal_mem_nssmmu_map);
EXPORT_SYMBOL(osal_mem_nssmmu_unmap);
EXPORT_SYMBOL(osal_mem_secsmmu_map);
EXPORT_SYMBOL(osal_mem_secsmmu_unmap);
EXPORT_SYMBOL(osal_mem_phys);
EXPORT_SYMBOL(osal_mem_create_fd);
EXPORT_SYMBOL(osal_mem_close_fd);
EXPORT_SYMBOL(osal_mem_handle_get);
EXPORT_SYMBOL(osal_mem_ref_get);
EXPORT_SYMBOL(osal_mem_ref_put);
EXPORT_SYMBOL(osal_mem_flush);
EXPORT_SYMBOL(osal_mem_get_attr);
EXPORT_SYMBOL(osal_mem_get_handle);
EXPORT_SYMBOL(osal_mem_get_nssmmu_pgtinfo);
