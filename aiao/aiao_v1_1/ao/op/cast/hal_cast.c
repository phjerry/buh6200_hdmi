/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: ao cast hal functions
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#include "hi_osal.h"
#include "hi_aiao_log.h"

#include "hi_drv_ao.h"
#include "circ_buf.h"

#include "hal_cast.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    aiao_cast_attr user_attr;

    /* internal state */
    aiao_cast_id id;
    aiao_cast_status status;
    circ_buf cb;

    struct osal_list_head list;
} cast_instance;

static OSAL_LIST_HEAD(g_cast_list);

static cast_instance *cast_find_by_id(aiao_cast_id id)
{
    cast_instance *cast = HI_NULL;

    osal_list_for_each_entry(cast, &g_cast_list, list) {
        if (cast->id == id) {
            return cast;
        }
    }

    return HI_NULL;
}

static aiao_cast_id cast_get_free_id(hi_void)
{
    aiao_cast_id id;

    for (id = AIAO_CAST_0; id < AIAO_CAST_MAX; id++) {
        if (cast_find_by_id(id) == HI_NULL) {
            return id;
        }
    }

    return AIAO_CAST_MAX;
}

static hi_s32 cast_check_attr(aiao_cast_attr *attr)
{
    CHECK_AO_NULL_PTR(attr);

    if (!attr->ext_dma_mem.buf_phy_addr || !attr->ext_dma_mem.buf_vir_addr) {
        HI_FATAL_AO("buf_phy_addr(0x%x) buf_vir_addr(0x%x) invalid\n",
            attr->ext_dma_mem.buf_phy_addr, attr->ext_dma_mem.buf_vir_addr);
        return HI_FAILURE;
    }

    if (attr->ext_dma_mem.buf_size < AIAO_CAST_BUFSIZE_MIN) {
        HI_FATAL_AO("buf_size(0x%x) less than min_size(0x%x) invalid\n",
            attr->ext_dma_mem.buf_size, AIAO_CAST_BUFSIZE_MIN);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static inline hi_void cast_start(cast_instance *cast)
{
    cast->status = AIAO_CAST_STATUS_START;
}

static hi_void cast_stop(cast_instance *cast)
{
    if (cast->status == AIAO_CAST_STATUS_STOP) {
        return;
    }

    circ_buf_flush(&cast->cb);
    cast->status = AIAO_CAST_STATUS_STOP;
}

static cast_instance *cast_create_instance(hi_void)
{
    hi_s32 ret;
    aiao_cast_id id;
    cast_instance *cast = HI_NULL;

    id = cast_get_free_id();
    if (id == AIAO_CAST_MAX) {
        HI_ERR_AO("cast_get_free_id failed\n");
        return HI_NULL;
    }

    cast = (cast_instance *)osal_kmalloc(HI_ID_AO, sizeof(cast_instance), OSAL_GFP_KERNEL);
    if (cast == HI_NULL) {
        HI_ERR_AO("osal_kmalloc cast_instance failed\n");
        return HI_NULL;
    }

    ret = memset_s(cast, sizeof(cast_instance), 0, sizeof(cast_instance));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memset_s, ret);
        osal_kfree(HI_ID_AO, cast);
        return HI_NULL;
    }

    cast->status = AIAO_CAST_STATUS_STOP;
    cast->id = id;

    osal_list_add(&cast->list, &g_cast_list);

    return cast;
}

static hi_void cast_destroy_instance(cast_instance *cast)
{
    cast_stop(cast);
    osal_list_del(&cast->list);
    osal_kfree(HI_ID_AO, cast);
}

static hi_s32 cast_set_attr(cast_instance *cast, aiao_cast_attr *attr)
{
    hi_s32 ret;

    if (cast->status != AIAO_CAST_STATUS_STOP) {
        return HI_FAILURE;
    }

    if (attr->ext_dma_mem.buf_size < AIAO_CAST_BUFSIZE_MIN) {
        return HI_FAILURE;
    }

    circ_buf_init(&cast->cb,
                  (hi_u32 *)((hi_u8 *)HI_NULL + attr->ext_dma_mem.wptr_addr),
                  (hi_u32 *)((hi_u8 *)HI_NULL + attr->ext_dma_mem.rptr_addr),
                  (hi_u32 *)((hi_u8 *)HI_NULL + attr->ext_dma_mem.buf_vir_addr),
                  attr->ext_dma_mem.buf_size);

    ret = memcpy_s(&cast->user_attr, sizeof(aiao_cast_attr), attr, sizeof(aiao_cast_attr));
    if (ret != EOK) {
        HI_ERR_PRINT_FUNC_RES(memcpy_s, ret);
        return ret;
    }

    return HI_SUCCESS;
}

/* global function */
hi_s32 hal_cast_init(hi_void)
{
    return HI_SUCCESS;
}

hi_void hal_cast_deinit(hi_void)
{
    cast_instance *cast = HI_NULL;
    cast_instance *tmp = HI_NULL;

    osal_list_for_each_entry_safe(cast, tmp, &g_cast_list, list) {
        cast_destroy_instance(cast);
    }
}

hi_s32 hal_cast_create(aiao_cast_id *id, aiao_cast_attr *attr)
{
    hi_s32 ret;
    cast_instance *cast = HI_NULL;

    ret = cast_check_attr(attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("cast_check_attr failed\n");
        return ret;
    }

    cast = cast_create_instance();
    if (cast == HI_NULL) {
        HI_ERR_AO("cast_create_instance failed\n");
        return HI_FAILURE;
    }

    ret = cast_set_attr(cast, attr);
    if (ret != HI_SUCCESS) {
        HI_ERR_AO("cast_set_attr failed\n");
        goto ERR_EXIT;
    }

    *id = cast->id;

    return ret;

ERR_EXIT:
    cast_destroy_instance(cast);
    *id = AIAO_CAST_MAX;
    return ret;
}

hi_void hal_cast_destroy(aiao_cast_id id)
{
    cast_instance *cast = cast_find_by_id(id);
    if (cast == HI_NULL) {
        HI_ERR_AO("cast_find_by_id failed\n");
        return;
    }

    cast_destroy_instance(cast);
}

hi_s32 hal_cast_start(aiao_cast_id id)
{
    cast_instance *cast = cast_find_by_id(id);
    if (cast == HI_NULL) {
        HI_ERR_AO("cast_find_by_id failed\n");
        return HI_FAILURE;
    }

    cast_start(cast);

    return HI_SUCCESS;
}

hi_s32 hal_cast_stop(aiao_cast_id id)
{
    cast_instance *cast = cast_find_by_id(id);
    if (cast == HI_NULL) {
        HI_ERR_AO("cast_find_by_id failed\n");
        return HI_FAILURE;
    }

    cast_stop(cast);

    return HI_SUCCESS;
}

hi_u32 hal_cast_query_buf_data(aiao_cast_id id)
{
    cast_instance *cast = cast_find_by_id(id);
    if (cast == HI_NULL) {
        HI_ERR_AO("cast_find_by_id failed\n");
        return 0;
    }

    if (cast->status == AIAO_CAST_STATUS_STOP) {
        return 0;
    }

    return circ_buf_query_busy(&cast->cb);
}

hi_u32 hal_cast_query_buf_free(aiao_cast_id id)
{
    cast_instance *cast = cast_find_by_id(id);
    if (cast == HI_NULL) {
        HI_ERR_AO("cast_find_by_id failed\n");
        return 0;
    }

    if (cast->status == AIAO_CAST_STATUS_STOP) {
        return 0;
    }

    return circ_buf_query_free(&cast->cb);
}

hi_u32 hal_cast_read_data(aiao_cast_id id, hi_u32 *offset, hi_u32 size)
{
    cast_instance *cast = cast_find_by_id(id);
    if (cast == HI_NULL) {
        HI_ERR_AO("cast_find_by_id failed\n");
        return 0;
    }

    if (cast->status == AIAO_CAST_STATUS_STOP) {
        return 0;
    }

    /* read data without updating read ptr */
    return circ_buf_cast_read(&cast->cb, offset, size);
}

hi_u32 hal_cast_release_data(aiao_cast_id id, hi_u32 size)
{
    cast_instance *cast = cast_find_by_id(id);
    if (cast == HI_NULL) {
        HI_ERR_AO("cast_find_by_id failed\n");
        return 0;
    }

    if (cast->status == AIAO_CAST_STATUS_STOP) {
        return 0;
    }

    /* update read ptr */
    return circ_buf_cast_relese(&cast->cb, size);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
