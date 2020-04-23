/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: klad basic function impl.
* Author: guoqingbo
* Create: 2016-08-12
*/
#include "drv_klad_hw_func.h"

#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include "linux/hisilicon/securec.h"
#include "hi_errno.h"
#include "hi_drv_module.h"
#include "hi_drv_sys.h"
#include "hi_drv_mem.h"
#include "hi_drv_klad.h"

#include "drv_klad_hw_define.h"
#include "drv_hkl.h"
#include "drv_klad_timestamp.h"

static struct klad_r_base_ops g_klad_hw_rbase_ops;

#ifdef HI_KLAD_PERF_SUPPORT
static struct time_ns *__get_timestamp(struct klad_r_base *obj, enum klad_timetamp_tag step)
{
    struct time_ns *t = HI_NULL;

    switch (step) {
        case TIMETAMP_HW_I:
            t = &obj->timestamp.hw_in;
            break;
        case TIMETAMP_CREATE_I:
            t = &obj->timestamp.create_in;
            break;
        case TIMETAMP_CREATE_O:
            t = &obj->timestamp.create_out;
            break;
        case TIMETAMP_OPTN_I:
            t = &obj->timestamp.open_in;
            break;
        case TIMETAMP_OPTN_O:
            t = &obj->timestamp.open_out;
            break;
        case TIMETAMP_RKP_I:
            t = &obj->timestamp.rkp_in;
            break;
        case TIMETAMP_RKP_O:
            t = &obj->timestamp.rkp_out;
            break;
        case TIMETAMP_START_I:
            t = &obj->timestamp.start_in;
            break;
        case TIMETAMP_START_O:
            t = &obj->timestamp.start_out;
            break;
        case TIMETAMP_CLOSE_I:
            t = &obj->timestamp.close_in;
            break;
        case TIMETAMP_CLOSE_O:
            t = &obj->timestamp.close_out;
            break;
        case TIMETAMP_DESTROY_I:
            t = &obj->timestamp.destroy_in;
            break;
        case TIMETAMP_DESTROY_O:
            t = &obj->timestamp.destroy_out;
            break;
        case TIMETAMP_HW_O:
            t = &obj->timestamp.hw_out;
            break;
    }
    return t;
}

#endif

hi_void timestamp_tag(struct klad_r_base *obj, enum klad_timetamp_tag step)
{
#ifdef HI_KLAD_PERF_SUPPORT
    struct time_ns *t = HI_NULL;
    struct timespec tv;

    if (obj == HI_NULL) {
        return ;
    }
    getnstimeofday(&tv);

    t = __get_timestamp(obj, step);
    if (t != HI_NULL) {
        t->tv_sec = tv.tv_sec;
        t->tv_nsec = tv.tv_nsec;
    }
    HI_DBG_KLAD("tag %d,%06ld.%09ld s\n", step, t->tv_sec, t->tv_nsec);
#endif
}

hi_void timestamp_add(struct time_ns *src, struct klad_r_base *obj, enum klad_timetamp_tag step)
{
#ifdef HI_KLAD_PERF_SUPPORT
    struct time_ns *t = HI_NULL;

    if (obj == HI_NULL || src == HI_NULL) {
        return ;
    }

    t = __get_timestamp(obj, step);
    if (t != HI_NULL) {
        t->tv_sec = src->tv_sec;
        t->tv_nsec = src->tv_nsec;
    }
    HI_DBG_KLAD("add %d,%06ld.%09ld s\n", step, t->tv_sec, t->tv_nsec);
#endif
}

hi_void timestamp_clean(struct klad_r_base *obj)
{
#ifdef HI_KLAD_PERF_SUPPORT
    hi_s32 ret;

    if (obj == HI_NULL) {
        return ;
    }

    ret = memset_s(&obj->timestamp, sizeof(struct klad_timestamp), 0, sizeof(struct klad_timestamp));
    if (ret != EOK) {
        HI_ERR_KLAD("Failed to clean timestamp buffer. obj=%p\n", obj);
    }
#endif
}

struct klad_r_base_ops *get_klad_r_base_ops(hi_void)
{
    return &g_klad_hw_rbase_ops;
}

hi_s32 klad_r_get_raw(struct klad_r_base *obj)
{
    if (obj == HI_NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }
    if (obj->ops == HI_NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }
    if (obj->ops->get == HI_NULL) {
        return HI_ERR_KLAD_NULL_PTR;
    }
    return obj->ops->get(obj);
}

hi_void klad_r_put(struct klad_r_base *obj)
{
    if (obj == HI_NULL) {
        return;
    }
    if (obj->ops == HI_NULL) {
        return;
    }
    if (obj->ops->put == HI_NULL) {
        return;
    }
    obj->ops->put(obj);
}

/*
 * R Base functions.
 */
static hi_s32 klad_r_base_get_impl(struct klad_r_base *obj)
{
    WARN_ON(&g_klad_hw_rbase_ops != obj->ops);
    WARN_ON(atomic_read(&obj->ref_count) == 0);

    /* check (v != 0) ? v++, return !0 : return 0. */
    if (!atomic_inc_not_zero(&obj->ref_count)) {
        return HI_FAILURE;
    }

    HI_DBG_KLAD("GET ref_count=%d->%d \n", atomic_read(&obj->ref_count) - 1, atomic_read(&obj->ref_count));
    return HI_SUCCESS;
}

static hi_void klad_r_base_put_impl(struct klad_r_base *obj)
{
    WARN_ON(&g_klad_hw_rbase_ops != obj->ops);
    WARN_ON(atomic_read(&obj->ref_count) == 0);
    HI_DBG_KLAD("PUT ref_count=%d->%d \n", atomic_read(&obj->ref_count), atomic_read(&obj->ref_count) - 1);

    /* 1: v--, 2: (v == 0) ? return !0 : return 0. */
    if (atomic_dec_and_test(&obj->ref_count)) {
        /*
        * obj->release will double check the ref_count.
        * because meybe have another process is creating instance,and found this instance has created.
        * so get the created instance, and add the reference count(obj->ref_count).
        * If we destory the instance, the another process will access a destoryed instance.and will cause an exception
        */
        hi_s32 ret = obj->release(obj);
        WARN_ON(ret != HI_SUCCESS);

        HI_DBG_KLAD("#    robj(0x%p) released.\n", obj);
    }
}

static hi_s32 __klad_hw_find(hi_handle handle, struct klad_r_base **obj)
{
    hi_s32 ret;
    struct klad_mgmt *mgmt = get_klad_mgmt();
    unsigned long mask;
    unsigned long *p = HI_NULL;
    hi_u32 id;

    __mutex_lock(&mgmt->lock);

    id = HI_HANDLE_GET_CHAN_ID(handle);
    if (unlikely(!(id < KLAD_COM_CNT))) {
        HI_ERR_KLAD("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_KLAD_INVALID_PARAM;
        goto out;
    }

    mask = BIT_MASK(id);
    p = ((unsigned long *)mgmt->com_hkl_bitmap) + BIT_WORD(id);
    if (!(*p & mask)) {
        HI_ERR_KLAD("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_KLAD_INVALID_PARAM;
        goto out;
    }

    *obj = (struct klad_r_base *)&mgmt->com_hkl[id];

    ret = HI_SUCCESS;

out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static struct klad_r_base_ops g_klad_hw_rbase_ops = {
    .get       = klad_r_base_get_impl,
    .put       = klad_r_base_put_impl,
};


hi_s32 klad_hw_com_hkl_find(hi_handle handle, struct klad_r_com_hkl **obj)
{
    hi_s32 ret;
    ret = __klad_hw_find(handle, (struct klad_r_base **)obj);
    if (ret == HI_SUCCESS) {
        if ((*obj)->associate_obj != HI_NULL) {
            ret = HI_ERR_KLAD_INVALID_PARAM;
        }
        if ((*obj)->base.handle != handle) {
            ret = HI_ERR_KLAD_INVALID_PARAM;
        }
    }
    return ret;
}

hi_s32 klad_hw_com_hkl_open(struct klad_entry_key *attrs, struct klad_r_com_hkl **obj)
{
    hi_s32 ret;
    struct klad_r_com_hkl *com_hkl = HI_NULL;

    /* 1 Create a com keyaldder hw instance (lock a com keyladder, then start rootkey process).
       2 lock a com keyladder. */
    ret = klad_mgmt_create_com_hkl(attrs, &com_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_mgmt_create_com_hkl, ret);
        goto out0;
    }
    ret = klad_r_get_raw((struct klad_r_base *)com_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_r_get_raw, ret);
        goto out1;
    }

    /* start rootkey process, if it has been started, this method will do nothing. */
    ret = com_hkl->ops->rootkey_process(com_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(com_hkl->ops->rootkey_process, ret);
        goto out2;
    }

    *obj = com_hkl;
    klad_r_put((struct klad_r_base *)com_hkl);

    return HI_SUCCESS;
out2:
    klad_r_put((struct klad_r_base *)com_hkl);
out1:
    klad_r_put((struct klad_r_base *)com_hkl);
out0:
    return ret;
}

hi_s32 klad_hw_com_hkl_start(struct klad_r_com_hkl *com_hkl, const hi_klad_com_entry *instance)
{
    hi_s32 ret;

    ret = klad_r_get_raw((struct klad_r_base *)com_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_r_get_raw, ret);
        goto out0;
    }

    ret = com_hkl->ops->start(com_hkl, instance);
    if (ret != HI_SUCCESS) {
        print_err_func(com_hkl->ops->start, ret);
        goto out1;
    }

out1:
    klad_r_put((struct klad_r_base *)com_hkl);
out0:
    return ret;
}

hi_s32 klad_hw_com_hkl_close(struct klad_r_com_hkl *com_hkl)
{
    klad_r_put((struct klad_r_base *)com_hkl);
    return HI_SUCCESS;
}

hi_s32 klad_hw_ta_hkl_find(hi_handle handle, struct klad_r_ta_hkl **obj)
{
    hi_s32 ret = HI_SUCCESS;
    struct klad_mgmt *mgmt = get_klad_mgmt();

    __mutex_lock(&mgmt->lock);

    if (mgmt->ta_hkl.base.handle != handle) {
        ret = HI_ERR_KLAD_INVALID_PARAM;
        goto out;
    }
    if (mgmt->ta_hkl_state != KLAD_OPENED) {
        ret = HI_ERR_KLAD_INVALID_PARAM;
        goto out;
    }
    *obj = &mgmt->ta_hkl;
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

hi_s32 klad_hw_ta_hkl_open(struct klad_entry_key *attrs, struct klad_r_ta_hkl **obj)
{
    hi_s32 ret;
    struct klad_r_ta_hkl *ta_hkl = HI_NULL;

    /* Create a com keyaldder hw instance (lock a com keyladder, then start rootkey process). */
    ret = klad_mgmt_create_ta_hkl(attrs, &ta_hkl);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = klad_r_get_raw((struct klad_r_base *)ta_hkl);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    /* lock a com keyladder. */
    ret = ta_hkl->ops->open(ta_hkl);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* start rootkey process, if it has been started, this method will do nothing. */
    ret = ta_hkl->ops->rootkey_process(ta_hkl);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    *obj = ta_hkl;
    klad_r_put((struct klad_r_base *)ta_hkl);

    return HI_SUCCESS;
out2:
    ta_hkl->ops->close(ta_hkl);
out1:
    klad_r_put((struct klad_r_base *)ta_hkl);

    klad_r_put((struct klad_r_base *)ta_hkl); /* release if ref_count dec to zero. */
out0:
    return ret;
}

hi_s32 klad_hw_ta_hkl_start(struct klad_r_ta_hkl *ta_hkl, const hi_klad_ta_entry *instance)
{
    hi_s32 ret;

    ret = klad_r_get_raw((struct klad_r_base *)ta_hkl);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = ta_hkl->ops->start(ta_hkl, instance);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

out1:
    klad_r_put((struct klad_r_base *)ta_hkl);
out0:
    return ret;
}

hi_s32 klad_hw_ta_hkl_close(struct klad_r_ta_hkl *ta_hkl)
{
    hi_s32 ret;

    ret = klad_r_get_raw((struct klad_r_base *)ta_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_r_get_raw, ret);
        goto out0;
    }

    ret = ta_hkl->ops->close(ta_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(ta_hkl->ops->close, ret);
        goto out1;
    }

    klad_r_put((struct klad_r_base *)ta_hkl);

out1:
    klad_r_put((struct klad_r_base *)ta_hkl);

out0:
    return ret;
}

hi_s32 klad_hw_fp_hkl_find(hi_handle handle, struct klad_r_fp_hkl **obj)
{
    hi_s32 ret = HI_SUCCESS;
    struct klad_mgmt *mgmt = get_klad_mgmt();

    __mutex_lock(&mgmt->lock);

    if (mgmt->fp_hkl.base.handle != handle) {
        ret = HI_ERR_KLAD_INVALID_PARAM;
        goto out;
    }
    if (mgmt->fp_hkl_state != KLAD_OPENED) {
        ret = HI_ERR_KLAD_INVALID_PARAM;
        goto out;
    }
    *obj = &mgmt->fp_hkl;
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

hi_s32 klad_hw_fp_hkl_open(struct klad_entry_key *attrs, struct klad_r_fp_hkl **obj)
{
    hi_s32 ret;
    struct klad_r_fp_hkl *fp_hkl = HI_NULL;

    /* Create a com keyaldder hw instance (lock a com keyladder, then start rootkey process). */
    ret = klad_mgmt_create_fp_hkl(attrs, &fp_hkl);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = klad_r_get_raw((struct klad_r_base *)fp_hkl);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    /* lock a com keyladder. */
    ret = fp_hkl->ops->open(fp_hkl);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* start rootkey process, if it has been started, this method will do nothing. */
    ret = fp_hkl->ops->rootkey_process(fp_hkl);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    *obj = fp_hkl;
    klad_r_put((struct klad_r_base *)fp_hkl);

    return HI_SUCCESS;
out2:
    fp_hkl->ops->close(fp_hkl);
out1:
    klad_r_put((struct klad_r_base *)fp_hkl);

    klad_r_put((struct klad_r_base *)fp_hkl); /* release if ref_count dec to zero. */
out0:
    return ret;
}

hi_s32 klad_hw_fp_hkl_start(struct klad_r_fp_hkl *fp_hkl, const hi_klad_fp_entry *instance)
{
    hi_s32 ret;

    ret = klad_r_get_raw((struct klad_r_base *)fp_hkl);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = fp_hkl->ops->start(fp_hkl, instance);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

out1:
    klad_r_put((struct klad_r_base *)fp_hkl);
out0:
    return ret;
}

hi_s32 klad_hw_fp_hkl_close(struct klad_r_fp_hkl *fp_hkl)
{
    hi_s32 ret;

    ret = klad_r_get_raw((struct klad_r_base *)fp_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_r_get_raw, ret);
        goto out0;
    }

    ret = fp_hkl->ops->close(fp_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(fp_hkl->ops->close, ret);
        goto out1;
    }

    klad_r_put((struct klad_r_base *)fp_hkl);

out1:
    klad_r_put((struct klad_r_base *)fp_hkl);

out0:
    return ret;
}

hi_s32 klad_hw_nonce_hkl_find(hi_handle handle, struct klad_r_nonce_hkl **obj)
{
    hi_s32 ret = HI_SUCCESS;
    struct klad_mgmt *mgmt = get_klad_mgmt();

    __mutex_lock(&mgmt->lock);

    if (mgmt->nonce_hkl.base.handle != handle) {
        ret = HI_ERR_KLAD_INVALID_PARAM;
        goto out;
    }
    if (mgmt->nonce_hkl_state != KLAD_OPENED) {
        ret = HI_ERR_KLAD_INVALID_PARAM;
        goto out;
    }
    *obj = &mgmt->nonce_hkl;
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

hi_s32 klad_hw_nonce_hkl_open(struct klad_entry_key *attrs, struct klad_r_nonce_hkl **obj)
{
    hi_s32 ret;
    struct klad_r_nonce_hkl *nonce_hkl = HI_NULL;

    /* Create a com keyaldder hw instance (lock a com keyladder, then start rootkey process). */
    ret = klad_mgmt_create_nonce_hkl(attrs, &nonce_hkl);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = klad_r_get_raw((struct klad_r_base *)nonce_hkl);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    /* lock a com keyladder. */
    ret = nonce_hkl->ops->open(nonce_hkl);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    /* start rootkey process, if it has been started, this method will do nothing. */
    ret = nonce_hkl->ops->rootkey_process(nonce_hkl);
    if (ret != HI_SUCCESS) {
        goto out2;
    }

    *obj = nonce_hkl;
    klad_r_put((struct klad_r_base *)nonce_hkl);

    return HI_SUCCESS;
out2:
    nonce_hkl->ops->close(nonce_hkl);
out1:
    klad_r_put((struct klad_r_base *)nonce_hkl);

    klad_r_put((struct klad_r_base *)nonce_hkl); /* release if ref_count dec to zero. */
out0:
    return ret;
}

hi_s32 klad_hw_nonce_hkl_start(struct klad_r_nonce_hkl *nonce_hkl, const hi_klad_nonce_entry *instance)
{
    hi_s32 ret;

    ret = klad_r_get_raw((struct klad_r_base *)nonce_hkl);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = nonce_hkl->ops->start(nonce_hkl, instance);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

out1:
    klad_r_put((struct klad_r_base *)nonce_hkl);
out0:
    return ret;
}

hi_s32 klad_hw_nonce_hkl_close(struct klad_r_nonce_hkl *nonce_hkl)
{
    hi_s32 ret;

    ret = klad_r_get_raw((struct klad_r_base *)nonce_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_r_get_raw, ret);
        goto out0;
    }

    ret = nonce_hkl->ops->close(nonce_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(nonce_hkl->ops->close, ret);
        goto out1;
    }

    klad_r_put((struct klad_r_base *)nonce_hkl);

out1:
    klad_r_put((struct klad_r_base *)nonce_hkl);

out0:
    return ret;
}

hi_s32 klad_hw_clr_route_open(struct klad_entry_key *attrs, struct klad_r_clr_route **obj)
{
    hi_s32 ret;
    struct klad_r_clr_route *clr_route = HI_NULL;

    /* Create a com keyaldder hw instance (lock a com keyladder, then start rootkey process). */
    ret = klad_mgmt_create_clr_route(attrs, &clr_route);
    if (ret != HI_SUCCESS) {
        goto out0;
    }
    ret = klad_r_get_raw((struct klad_r_base *)clr_route);
    if (ret != HI_SUCCESS) {
        goto out1;
    }
    /* lock a com keyladder. */
    ret = clr_route->ops->open(clr_route);
    if (ret != HI_SUCCESS) {
        goto out2;
    }
    *obj = clr_route;
    klad_r_put((struct klad_r_base *)clr_route);
    return HI_SUCCESS;
out2:
    klad_r_put((struct klad_r_base *)clr_route);
out1:
    klad_r_put((struct klad_r_base *)clr_route); /* release if ref_count dec to zero. */
out0:
    return ret;
}

hi_s32 klad_hw_clr_route_start(struct klad_r_clr_route *clr_route, const hi_klad_clr_entry *instance)
{
    hi_s32 ret;

    ret = klad_r_get_raw((struct klad_r_base *)clr_route);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = clr_route->ops->start(clr_route, instance);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

out1:
    klad_r_put((struct klad_r_base *)clr_route);
out0:
    return ret;
}

hi_s32 klad_hw_clr_route_close(struct klad_r_clr_route *clr_route)
{
    hi_s32 ret;

    ret = klad_r_get_raw((struct klad_r_base *)clr_route);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_r_get_raw, ret);
        goto out0;
    }

    ret = clr_route->ops->close(clr_route);
    if (ret != HI_SUCCESS) {
        print_err_func(clr_route->ops->close, ret);
        goto out1;
    }

    klad_r_put((struct klad_r_base *)clr_route);

out1:
    klad_r_put((struct klad_r_base *)clr_route);

out0:
    return ret;
}

/*
 * KLAD mgmt helper functions.
 */
hi_s32 klad_mgmt_init(hi_void)
{
    hi_s32 ret;
    struct klad_mgmt *mgmt = __get_klad_mgmt();

    ret = osal_mutex_init(&mgmt->lock);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Init the mutex failed.\n");
        return ret;
    }

    return mgmt->ops->init(mgmt);
}

hi_void klad_mgmt_exit(hi_void)
{
    struct klad_mgmt *mgmt = get_klad_mgmt();

    if (mgmt->ops->exit(mgmt) != HI_SUCCESS) {
        mgmt->ops->show_info(mgmt);

        /*
         * hw_session must release all resource.
         */
        HI_FATAL_KLAD("klad mgmt exit with error!\n");
    }
    osal_mutex_destory(&mgmt->lock);
}

hi_s32 klad_mgmt_suspend(hi_void)
{
    struct klad_mgmt *mgmt = __get_klad_mgmt();

    return mgmt->ops->suspend(mgmt);
}

hi_s32 klad_mgmt_resume(hi_void)
{
    struct klad_mgmt *mgmt = __get_klad_mgmt();

    return mgmt->ops->resume(mgmt);
}

hi_s32 klad_mgmt_create_com_hkl(struct klad_entry_key *attrs, struct klad_r_com_hkl **com_hkl)
{
    struct klad_mgmt *mgmt = get_klad_mgmt();

    return mgmt->ops->create_com_hkl(mgmt, attrs, com_hkl);
}

hi_s32 klad_mgmt_destroy_com_hkl(struct klad_r_base *obj)
{
    struct klad_mgmt *mgmt = get_klad_mgmt();

    return mgmt->ops->destroy_com_hkl(mgmt, (struct klad_r_com_hkl *)obj);
}

hi_s32 klad_mgmt_create_ta_hkl(struct klad_entry_key *attrs, struct klad_r_ta_hkl **ta_hkl)
{
    struct klad_mgmt *mgmt = get_klad_mgmt();

    return mgmt->ops->create_ta_hkl(mgmt, attrs, ta_hkl);
}

hi_s32 klad_mgmt_destroy_ta_hkl(struct klad_r_base *obj)
{
    struct klad_mgmt *mgmt = get_klad_mgmt();

    return mgmt->ops->destroy_ta_hkl(mgmt, (struct klad_r_ta_hkl *)obj);
}

hi_s32 klad_mgmt_create_fp_hkl(struct klad_entry_key *attrs, struct klad_r_fp_hkl **fp_hkl)
{
    struct klad_mgmt *mgmt = get_klad_mgmt();

    return mgmt->ops->create_fp_hkl(mgmt, attrs, fp_hkl);
}

hi_s32 klad_mgmt_destroy_fp_hkl(struct klad_r_base *obj)
{
    struct klad_mgmt *mgmt = get_klad_mgmt();

    return mgmt->ops->destroy_fp_hkl(mgmt, (struct klad_r_fp_hkl *)obj);
}


hi_s32 klad_mgmt_create_nonce_hkl(struct klad_entry_key *attrs, struct klad_r_nonce_hkl **nonce_hkl)
{
    struct klad_mgmt *mgmt = get_klad_mgmt();

    return mgmt->ops->create_nonce_hkl(mgmt, attrs, nonce_hkl);
}

hi_s32 klad_mgmt_destroy_nonce_hkl(struct klad_r_base *obj)
{
    struct klad_mgmt *mgmt = get_klad_mgmt();

    return mgmt->ops->destroy_nonce_hkl(mgmt, (struct klad_r_nonce_hkl *)obj);
}

hi_s32 klad_mgmt_create_clr_route(struct klad_entry_key *attrs, struct klad_r_clr_route **clr_route)
{
    struct klad_mgmt *mgmt = get_klad_mgmt();

    return mgmt->ops->create_clr_route(mgmt, attrs, clr_route);
}

hi_s32 klad_mgmt_destroy_clr_route(struct klad_r_base *obj)
{
    struct klad_mgmt *mgmt = get_klad_mgmt();

    return mgmt->ops->destroy_clr_route(mgmt, (struct klad_r_clr_route *)obj);
}

static inline hi_bool __hkl_is_unlocked(struct klad_r_base *base)
{
    return !base->is_locked;
}

static inline hi_bool __hkl_is_locked(struct klad_r_base *base)
{
    return base->is_locked;
}

static hi_s32 __klad_r_com_hkl_open_impl(struct klad_r_com_hkl *com_hkl)
{
    hi_s32 ret;

    if (__hkl_is_locked(&com_hkl->base)) {
        return HI_SUCCESS;
    }

    ret =  hi_klad_com_lock(&(com_hkl->base.hw_id));
    if (ret == HI_SUCCESS) {
        com_hkl->base.is_locked = HI_TRUE;
    }
    com_hkl->rkp_ready = HI_FALSE;
    return ret;
}

static hi_s32 klad_r_com_hkl_open_impl(struct klad_r_com_hkl *com_hkl)
{
    hi_s32 ret;

    __mutex_lock(&com_hkl->lock);

    timestamp_tag(&com_hkl->base, TIMETAMP_OPTN_I);

    ret = __klad_r_com_hkl_open_impl(com_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_r_com_hkl_open_impl, ret);
    }
    timestamp_tag(&com_hkl->base, TIMETAMP_OPTN_O);

    __mutex_unlock(&com_hkl->lock);

    return ret;
}

/**
* Com keyladder rootkey process, if current keyladder is not com keyladder,
* that means create a cascade keyladder, such as com keyladder+fp keyladder or
* com keyaldder + nonce keyladder, rootkey process will done in the subordinate
* keyladder context, so do nothing in current context.
*/
static hi_s32 klad_r_com_hkl_rkp_impl(struct klad_r_com_hkl *com_hkl)
{
    hi_s32 ret;

    __mutex_lock(&com_hkl->lock);

    timestamp_tag(&com_hkl->base, TIMETAMP_RKP_I);

    WARN_ON(__hkl_is_unlocked(&com_hkl->base));

    if (com_hkl->rkp_ready == HI_TRUE) {
        ret = HI_SUCCESS; /* do nothing~. */
        goto out;
    }
    ret = hi_klad_rkp_entry_start(com_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_klad_rkp_entry_start, ret);
        goto out;
    }

    com_hkl->rkp_ready = HI_TRUE;
    ret = HI_SUCCESS;

out:
    timestamp_tag(&com_hkl->base, TIMETAMP_RKP_O);
    __mutex_unlock(&com_hkl->lock);

    return ret;
}

static hi_void klad_r_com_hkl_rkp_restart(struct klad_r_com_hkl *com_hkl)
{
    hi_s32 ret;

    WARN_ON(__hkl_is_unlocked(&com_hkl->base));

    com_hkl->rkp_ready = HI_FALSE;
    ret = hi_klad_rkp_entry_start(com_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_klad_rkp_entry_start, ret);
        return;
    }
    com_hkl->rkp_ready = HI_TRUE;
}

static hi_s32 klad_r_com_hkl_start_impl(struct klad_r_com_hkl *com_hkl, const hi_klad_com_entry *instance)
{
    hi_s32 ret;

    __mutex_lock(&com_hkl->lock);

    timestamp_tag(&com_hkl->base, TIMETAMP_START_I);

    WARN_ON(__hkl_is_unlocked(&com_hkl->base));

    ret = hi_klad_com_entry_start(com_hkl, instance);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_klad_com_entry_start, ret);
        klad_r_com_hkl_rkp_restart(com_hkl);
    }

    timestamp_tag(&com_hkl->base, TIMETAMP_START_O);

    __mutex_unlock(&com_hkl->lock);

    return ret;
}


static hi_s32 __klad_r_com_hkl_close_impl(struct klad_r_com_hkl *com_hkl)
{
    hi_s32 ret;

    ret = hi_klad_com_unlock(com_hkl->base.hw_id);
    if (ret == HI_SUCCESS) {
        com_hkl->base.is_locked = HI_FALSE;
        com_hkl->rkp_ready = HI_FALSE;
    }

    return ret;
}

static hi_s32 klad_r_com_hkl_close_impl(struct klad_r_com_hkl *com_hkl)
{
    hi_s32 ret;

    __mutex_lock(&com_hkl->lock);

    timestamp_tag(&com_hkl->base, TIMETAMP_CLOSE_I);

    if (__hkl_is_unlocked(&com_hkl->base)) {
        ret = HI_ERR_KLAD_UNLOCKED;
        goto out;
    }

    ret = __klad_r_com_hkl_close_impl(com_hkl);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Unlock com keyladder %d failed.\n", com_hkl->base.hw_id);
    }

out:
    timestamp_tag(&com_hkl->base, TIMETAMP_CLOSE_O);
    __mutex_unlock(&com_hkl->lock);

    return ret;
}

static hi_s32 klad_r_com_hkl_suspend_impl(struct klad_r_com_hkl *com_hkl)
{
    hi_s32 ret;
    __mutex_lock(&com_hkl->lock);

    WARN_ON(__hkl_is_unlocked(&com_hkl->base));

    ret = __klad_r_com_hkl_close_impl(com_hkl);

    __mutex_unlock(&com_hkl->lock);

    return ret;
}

static hi_s32 klad_r_com_hkl_resume_impl(struct klad_r_com_hkl *com_hkl)
{
    hi_s32 ret;
    __mutex_lock(&com_hkl->lock);

    ret = __klad_r_com_hkl_open_impl(com_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_r_com_hkl_open_impl, ret);
        goto out;
    }
    /* If RKP failed, keep klad locked. do not close com_hkl. */
    ret = hi_klad_rkp_entry_start(com_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_klad_rkp_entry_start, ret);
        goto out;
    }
    com_hkl->rkp_ready = HI_TRUE;
out:
    __mutex_unlock(&com_hkl->lock);

    return ret;
}

static struct klad_r_com_hkl_ops g_com_klad_ops = {
    .open              = klad_r_com_hkl_open_impl,
    .rootkey_process   = klad_r_com_hkl_rkp_impl,
    .start             = klad_r_com_hkl_start_impl,
    .close             = klad_r_com_hkl_close_impl,

    .suspend     = klad_r_com_hkl_suspend_impl,
    .resume      = klad_r_com_hkl_resume_impl,
};

static hi_s32 __klad_r_fp_hkl_open_impl(struct klad_r_fp_hkl *fp_hkl)
{
    hi_s32 ret;

    if (__hkl_is_locked(&fp_hkl->base)) {
        return HI_SUCCESS;
    }

    ret = hi_klad_com_lock(&fp_hkl->com_obj->hw_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Lock com keyladder failed.\n");
        goto out;
    }
    ret = hi_klad_fp_lock();
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Lock fp keyladder failed.\n");
        goto out1;
    }
    fp_hkl->base.hw_id = fp_hkl->com_obj->hw_id;
    fp_hkl->base.is_locked = HI_TRUE;

    return HI_SUCCESS;
out1:
    hi_klad_com_unlock(fp_hkl->com_obj->hw_id);
out:
    return ret;
}

static hi_s32 klad_r_fp_hkl_open_impl(struct klad_r_fp_hkl *fp_hkl)
{
    hi_s32 ret;

    __mutex_lock(&fp_hkl->lock);

    timestamp_tag(&fp_hkl->base, TIMETAMP_OPTN_I);

    ret = __klad_r_fp_hkl_open_impl(fp_hkl);

    timestamp_tag(&fp_hkl->base, TIMETAMP_OPTN_O);

    __mutex_unlock(&fp_hkl->lock);

    return ret;
}

static hi_s32 klad_r_fp_hkl_rkp_impl(struct klad_r_fp_hkl *fp_hkl)
{
    hi_s32 ret;

    __mutex_lock(&fp_hkl->lock);

    timestamp_tag(&fp_hkl->base, TIMETAMP_RKP_I);

    WARN_ON(__hkl_is_unlocked(&fp_hkl->base));

    if (fp_hkl->rkp_ready == HI_TRUE) {
        ret = HI_SUCCESS; /* do nothing~. */
        goto out;
    }
    /* do something: */
    fp_hkl->rkp_ready = HI_TRUE;
    ret = HI_SUCCESS;

out:
    timestamp_tag(&fp_hkl->base, TIMETAMP_RKP_O);
    __mutex_unlock(&fp_hkl->lock);

    return ret;
}

static hi_s32 __klad_r_fp_com_hkl_start_impl(struct klad_r_com_hkl *com_hkl)
{
    unused(com_hkl);
    return HI_SUCCESS;
}

static hi_s32 klad_r_fp_hkl_start_impl(struct klad_r_fp_hkl *fp_hkl, const hi_klad_fp_entry *instance)
{
    hi_s32 ret;
    struct klad_r_com_hkl *com_hkl = HI_NULL_PTR;

    unused(instance);
    __mutex_lock(&fp_hkl->lock);

    timestamp_tag(&fp_hkl->base, TIMETAMP_START_I);

    WARN_ON(__hkl_is_unlocked(&fp_hkl->base));

    com_hkl = (struct klad_r_com_hkl *)fp_hkl->com_obj;
    ret = __klad_r_fp_com_hkl_start_impl(com_hkl);
    if (ret != HI_SUCCESS) {
        /* start rkp again. */
        goto out;
    }

    ret = HI_SUCCESS;
out:

    timestamp_tag(&fp_hkl->base, TIMETAMP_START_O);
    __mutex_unlock(&fp_hkl->lock);

    return ret;
}

static hi_s32 __klad_r_fp_hkl_close_impl(struct klad_r_fp_hkl *fp_hkl)
{
    hi_s32 ret;
    hi_s32 ret1;

    ret = hi_klad_com_unlock(fp_hkl->com_obj->hw_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Lock com keyladder failed.\n");
    }
    ret1 = hi_klad_fp_unlock();
    if (ret1 != HI_SUCCESS) {
        HI_ERR_KLAD("Lock fp keyladder failed.\n");
        goto out;
    }
    fp_hkl->base.is_locked = HI_FALSE;
    return ret;
out:
    return ret1;
}

static hi_s32 klad_r_fp_hkl_close_impl(struct klad_r_fp_hkl *fp_hkl)
{
    hi_s32 ret;

    __mutex_lock(&fp_hkl->lock);

    timestamp_tag(&fp_hkl->base, TIMETAMP_CLOSE_I);

    if (__hkl_is_unlocked(&fp_hkl->base)) {
        ret = HI_ERR_KLAD_UNLOCKED;
        goto out;
    }

    ret = __klad_r_fp_hkl_close_impl(fp_hkl);
    if (ret == HI_SUCCESS) {
        fp_hkl->rkp_ready = HI_FALSE;
    } else {
        HI_ERR_KLAD("Unlock fp keyladder %d(func klad %d) failed.\n", fp_hkl->base.hw_id, fp_hkl->com_obj->hw_id);
    }

out:
    timestamp_tag(&fp_hkl->base, TIMETAMP_CLOSE_O);
    __mutex_unlock(&fp_hkl->lock);

    return ret;
}

static hi_s32 klad_r_fp_hkl_suspend_impl(struct klad_r_fp_hkl *fp_hkl)
{
    hi_s32 ret;
    __mutex_lock(&fp_hkl->lock);

    WARN_ON(__hkl_is_unlocked(&fp_hkl->base));

    ret = __klad_r_fp_hkl_close_impl(fp_hkl);

    __mutex_unlock(&fp_hkl->lock);

    return ret;
}

static hi_s32 klad_r_fp_hkl_resume_impl(struct klad_r_fp_hkl *fp_hkl)
{
    hi_s32 ret;
    __mutex_lock(&fp_hkl->lock);

    ret = __klad_r_fp_hkl_open_impl(fp_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_r_fp_hkl_open_impl, ret);
        goto out;
    }
    /* start rkp */
out:
    __mutex_unlock(&fp_hkl->lock);

    return ret;
}

static struct klad_r_fp_hkl_ops g_fp_klad_ops = {
    .open              = klad_r_fp_hkl_open_impl,
    .rootkey_process   = klad_r_fp_hkl_rkp_impl,
    .start             = klad_r_fp_hkl_start_impl,
    .close             = klad_r_fp_hkl_close_impl,

    .suspend     = klad_r_fp_hkl_suspend_impl,
    .resume      = klad_r_fp_hkl_resume_impl,
};

static hi_s32 __klad_r_ta_hkl_open_impl(struct klad_r_ta_hkl *ta_hkl)
{
    hi_s32 ret;

    if (__hkl_is_locked(&ta_hkl->base)) {
        return HI_SUCCESS;
    }

    ret = hi_klad_com_lock(&ta_hkl->com_obj->hw_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Lock com keyladder failed.\n");
        goto out;
    }
    ret = hi_klad_ta_lock();
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Lock ta keyladder failed.\n");
        goto out1;
    }
    ta_hkl->base.hw_id = KLAD_ID_TA;
    ta_hkl->base.is_locked = HI_TRUE;

    return HI_SUCCESS;
out1:
    hi_klad_com_unlock(ta_hkl->com_obj->hw_id);
out:
    return ret;
}

static hi_s32 klad_r_ta_hkl_open_impl(struct klad_r_ta_hkl *ta_hkl)
{
    hi_s32 ret;

    __mutex_lock(&ta_hkl->lock);

    timestamp_tag(&ta_hkl->base, TIMETAMP_OPTN_I);

    ret = __klad_r_ta_hkl_open_impl(ta_hkl);

    timestamp_tag(&ta_hkl->base, TIMETAMP_OPTN_O);

    __mutex_unlock(&ta_hkl->lock);

    return ret;
}

static hi_s32 klad_r_ta_hkl_rkp_impl(struct klad_r_ta_hkl *ta_hkl)
{
    hi_s32 ret;

    __mutex_lock(&ta_hkl->lock);

    timestamp_tag(&ta_hkl->base, TIMETAMP_RKP_I);

    WARN_ON(__hkl_is_unlocked(&ta_hkl->base));

    if (ta_hkl->rkp_ready == HI_TRUE) {
        ret = HI_SUCCESS; /* do nothing~. */
        goto out;
    }

    /* do something: */
    ta_hkl->rkp_ready = HI_TRUE;
    ret = HI_SUCCESS;

out:
    timestamp_tag(&ta_hkl->base, TIMETAMP_RKP_O);
    __mutex_unlock(&ta_hkl->lock);

    return ret;
}


static hi_s32 klad_r_ta_hkl_start_impl(struct klad_r_ta_hkl *ta_hkl, const hi_klad_ta_entry *instance)
{
    hi_s32 ret;

    unused(instance);

    __mutex_lock(&ta_hkl->lock);

    timestamp_tag(&ta_hkl->base, TIMETAMP_START_I);

    WARN_ON(__hkl_is_unlocked(&ta_hkl->base));

    ret = HI_SUCCESS;

    timestamp_tag(&ta_hkl->base, TIMETAMP_START_O);
    __mutex_unlock(&ta_hkl->lock);

    return ret;
}

static hi_s32 __klad_r_ta_hkl_close_impl(struct klad_r_ta_hkl *ta_hkl)
{
    hi_s32 ret;
    hi_s32 ret1;

    ret = hi_klad_com_unlock(ta_hkl->com_obj->hw_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Lock com keyladder failed.\n");
    }
    ret1 = hi_klad_ta_unlock();
    if (ret1 != HI_SUCCESS) {
        HI_ERR_KLAD("Lock ta keyladder failed.\n");
        goto out;
    }
    ta_hkl->base.is_locked = HI_FALSE;
    return ret;
out:
    return ret1;
}

static hi_s32 klad_r_ta_hkl_close_impl(struct klad_r_ta_hkl *ta_hkl)
{
    hi_s32 ret;

    __mutex_lock(&ta_hkl->lock);

    timestamp_tag(&ta_hkl->base, TIMETAMP_CLOSE_I);

    if (__hkl_is_unlocked(&ta_hkl->base)) {
        ret = HI_ERR_KLAD_UNLOCKED;
        goto out;
    }

    ret = __klad_r_ta_hkl_close_impl(ta_hkl);
    if (ret == HI_SUCCESS) {
        ta_hkl->rkp_ready = HI_FALSE;
    } else {
        HI_ERR_KLAD("Unlock takeyladder %d(func klad %d) failed.\n", ta_hkl->base.hw_id, ta_hkl->com_obj->hw_id);
    }
out:
    timestamp_tag(&ta_hkl->base, TIMETAMP_CLOSE_O);
    __mutex_unlock(&ta_hkl->lock);

    return ret;
}

static hi_s32 klad_r_ta_hkl_suspend_impl(struct klad_r_ta_hkl *ta_hkl)
{
    hi_s32 ret;
    __mutex_lock(&ta_hkl->lock);

    WARN_ON(__hkl_is_unlocked(&ta_hkl->base));

    ret = __klad_r_ta_hkl_close_impl(ta_hkl);

    __mutex_unlock(&ta_hkl->lock);

    return ret;
}

static hi_s32 klad_r_ta_hkl_resume_impl(struct klad_r_ta_hkl *ta_hkl)
{
    hi_s32 ret;
    __mutex_lock(&ta_hkl->lock);

    WARN_ON(__hkl_is_unlocked(&ta_hkl->base));

    ret = __klad_r_ta_hkl_open_impl(ta_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_r_ta_hkl_open_impl, ret);
        goto out;
    }
    /* start rkp */
out:
    __mutex_unlock(&ta_hkl->lock);

    return ret;
}

static struct klad_r_ta_hkl_ops g_ta_klad_ops = {
    .open              = klad_r_ta_hkl_open_impl,
    .rootkey_process   = klad_r_ta_hkl_rkp_impl,
    .start             = klad_r_ta_hkl_start_impl,
    .close             = klad_r_ta_hkl_close_impl,

    .suspend     = klad_r_ta_hkl_suspend_impl,
    .resume      = klad_r_ta_hkl_resume_impl,
};


static hi_s32 __klad_r_nonce_hkl_open_impl(struct klad_r_nonce_hkl *nonce_hkl)
{
    hi_s32 ret;

    if (__hkl_is_locked(&nonce_hkl->base)) {
        return HI_SUCCESS;
    }

    ret = hi_klad_com_lock(&nonce_hkl->com_obj->hw_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Lock com keyladder failed.\n");
        goto out;
    }

    ret = hi_klad_nonce_lock();
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Lock nonce keyladder failed.\n");
        goto out1;
    }
    nonce_hkl->base.hw_id = nonce_hkl->com_obj->hw_id;
    nonce_hkl->base.is_locked = HI_TRUE;
    return HI_SUCCESS;
out1:
    hi_klad_com_unlock(nonce_hkl->com_obj->hw_id);
out:
    return ret;
}

static hi_s32 klad_r_nonce_hkl_open_impl(struct klad_r_nonce_hkl *nonce_hkl)
{
    hi_s32 ret;

    __mutex_lock(&nonce_hkl->lock);

    timestamp_tag(&nonce_hkl->base, TIMETAMP_OPTN_I);

    ret = __klad_r_nonce_hkl_open_impl(nonce_hkl);

    timestamp_tag(&nonce_hkl->base, TIMETAMP_OPTN_O);

    __mutex_unlock(&nonce_hkl->lock);

    return ret;
}

static hi_s32 klad_r_nonce_hkl_rkp_impl(struct klad_r_nonce_hkl *nonce_hkl)
{
    hi_s32 ret;

    __mutex_lock(&nonce_hkl->lock);

    timestamp_tag(&nonce_hkl->base, TIMETAMP_RKP_I);

    WARN_ON(__hkl_is_unlocked(&nonce_hkl->base));

    if (nonce_hkl->rkp_ready == HI_TRUE) {
        ret = HI_SUCCESS; /* do nothing~. */
        goto out;
    }
    /* do something: */
    nonce_hkl->rkp_ready = HI_TRUE;
    ret = HI_SUCCESS;

out:
    timestamp_tag(&nonce_hkl->base, TIMETAMP_RKP_O);
    __mutex_unlock(&nonce_hkl->lock);

    return ret;
}

static hi_s32 __klad_r_nonce_com_hkl_start_impl(struct klad_r_com_hkl *com_hkl)
{
    unused(com_hkl);

    return HI_SUCCESS;
}

static hi_s32 klad_r_nonce_hkl_start_impl(struct klad_r_nonce_hkl *nonce_hkl,
                                          const hi_klad_nonce_entry *instance)
{
    hi_s32 ret;
    struct klad_r_com_hkl *com_hkl = HI_NULL_PTR;

    unused(instance);

    __mutex_lock(&nonce_hkl->lock);

    timestamp_tag(&nonce_hkl->base, TIMETAMP_START_I);

    WARN_ON(__hkl_is_unlocked(&nonce_hkl->base));

    com_hkl = (struct klad_r_com_hkl *)nonce_hkl->com_obj;
    ret = __klad_r_nonce_com_hkl_start_impl(com_hkl);
    if (ret != HI_SUCCESS) {
        /* start rkp again. */
        goto out;
    }

    ret = HI_SUCCESS;
out:
    timestamp_tag(&nonce_hkl->base, TIMETAMP_START_O);
    __mutex_unlock(&nonce_hkl->lock);

    return ret;
}

static hi_s32 __klad_r_nonce_hkl_close_impl(struct klad_r_nonce_hkl *nonce_hkl)
{
    hi_s32 ret;
    hi_s32 ret1;

    ret = hi_klad_com_unlock(nonce_hkl->com_obj->hw_id);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Lock com keyladder failed.\n");
    }
    ret1 = hi_klad_nonce_unlock();
    if (ret1 != HI_SUCCESS) {
        HI_ERR_KLAD("Lock nonce keyladder failed.\n");
        goto out;
    }
    nonce_hkl->base.is_locked = HI_FALSE;
    return ret;
out:
    return ret1;
}

static hi_s32 klad_r_nonce_hkl_close_impl(struct klad_r_nonce_hkl *nonce_hkl)
{
    hi_s32 ret;

    __mutex_lock(&nonce_hkl->lock);

    timestamp_tag(&nonce_hkl->base, TIMETAMP_CLOSE_I);

    if (__hkl_is_unlocked(&nonce_hkl->base)) {
        ret = HI_ERR_KLAD_UNLOCKED;
        goto out;
    }

    ret = __klad_r_nonce_hkl_close_impl(nonce_hkl);
    if (ret == HI_SUCCESS) {
        nonce_hkl->rkp_ready = HI_FALSE;
    } else {
        HI_ERR_KLAD("Unlock nonce keyladder %d(func %d) failed.\n", nonce_hkl->base.hw_id, nonce_hkl->com_obj->hw_id);
    }
out:
    timestamp_tag(&nonce_hkl->base, TIMETAMP_CLOSE_O);
    __mutex_unlock(&nonce_hkl->lock);

    return ret;
}

static hi_s32 klad_r_nonce_hkl_suspend_impl(struct klad_r_nonce_hkl *nonce_hkl)
{
    hi_s32 ret;
    __mutex_lock(&nonce_hkl->lock);

    WARN_ON(__hkl_is_unlocked(&nonce_hkl->base));

    ret = __klad_r_nonce_hkl_close_impl(nonce_hkl);

    __mutex_unlock(&nonce_hkl->lock);

    return ret;
}

static hi_s32 klad_r_nonce_hkl_resume_impl(struct klad_r_nonce_hkl *nonce_hkl)
{
    hi_s32 ret;
    __mutex_lock(&nonce_hkl->lock);

    ret = __klad_r_nonce_hkl_open_impl(nonce_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_r_nonce_hkl_open_impl, ret);
        goto out;
    }
    /* start rkp */
out:
    __mutex_unlock(&nonce_hkl->lock);

    return ret;
}

static struct klad_r_nonce_hkl_ops g_nonce_klad_ops = {
    .open              = klad_r_nonce_hkl_open_impl,
    .rootkey_process   = klad_r_nonce_hkl_rkp_impl,
    .start             = klad_r_nonce_hkl_start_impl,
    .close             = klad_r_nonce_hkl_close_impl,

    .suspend     = klad_r_nonce_hkl_suspend_impl,
    .resume      = klad_r_nonce_hkl_resume_impl,
};


static hi_s32 __klad_r_clr_route_open_impl(struct klad_r_clr_route *clr_route)
{
    return HI_SUCCESS;
}

static hi_s32 klad_r_clr_route_open_impl(struct klad_r_clr_route *clr_route)
{
    hi_s32 ret;

    __mutex_lock(&clr_route->lock);

    timestamp_tag(&clr_route->base, TIMETAMP_OPTN_I);

    ret = __klad_r_clr_route_open_impl(clr_route);

    timestamp_tag(&clr_route->base, TIMETAMP_OPTN_O);

    __mutex_unlock(&clr_route->lock);

    return ret;
}

static hi_s32 klad_r_clr_route_start_impl(struct klad_r_clr_route *clr_route,
                                          const hi_klad_clr_entry *instance)
{
    hi_s32 ret;

    __mutex_lock(&clr_route->lock);

    timestamp_tag(&clr_route->base, TIMETAMP_START_I);

    ret = hi_klad_clr_entry_start(clr_route, instance);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_klad_clr_entry_start, ret);
    }

    timestamp_tag(&clr_route->base, TIMETAMP_START_O);

    __mutex_unlock(&clr_route->lock);

    return ret;
}

static hi_s32 __klad_r_clr_route_close_impl(const struct klad_r_clr_route *clr_route)
{
    unused(clr_route);

    return 0;
}

static hi_s32 klad_r_clr_route_close_impl(struct klad_r_clr_route *clr_route)
{
    hi_s32 ret;

    __mutex_lock(&clr_route->lock);

    timestamp_tag(&clr_route->base, TIMETAMP_CLOSE_I);

    ret = __klad_r_clr_route_close_impl(clr_route);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Unlock clear keyladder failed.\n");
    }

    timestamp_tag(&clr_route->base, TIMETAMP_CLOSE_O);
    __mutex_unlock(&clr_route->lock);

    return ret;
}

static hi_s32 klad_r_clr_route_suspend_impl(struct klad_r_clr_route *clr_route)
{
    hi_s32 ret;
    __mutex_lock(&clr_route->lock);

    WARN_ON(__hkl_is_unlocked(&clr_route->base));

    ret = __klad_r_clr_route_close_impl(clr_route);

    __mutex_unlock(&clr_route->lock);

    return ret;
}

static hi_s32 klad_r_clr_route_resume_impl(struct klad_r_clr_route *clr_route)
{
    hi_s32 ret;

    __mutex_lock(&clr_route->lock);

    ret = __klad_r_clr_route_open_impl(clr_route);

    __mutex_unlock(&clr_route->lock);

    return ret;
}

static struct klad_r_clr_route_ops g_clr_route_ops = {
    .open              = klad_r_clr_route_open_impl,
    .start             = klad_r_clr_route_start_impl,
    .close             = klad_r_clr_route_close_impl,

    .suspend     = klad_r_clr_route_suspend_impl,
    .resume      = klad_r_clr_route_resume_impl,
};

static hi_void __klad_mgmt_init_impl(struct klad_mgmt *mgmt)
{
    hi_s32 ret;
    hi_u32 i;

    mgmt->com_hkl_cnt = KLAD_COM_CNT;
    ret = osal_mutex_init(&mgmt->com_hkl_lock);
    if (ret != HI_SUCCESS) {
        return;
    }

    for (i = 0; i < KLAD_COM_CNT; i++) {
        ret = osal_mutex_init(&mgmt->com_hkl[i].lock);
        if (ret != HI_SUCCESS) {
            return;
        }
    }

    INIT_LIST_HEAD(&mgmt->com_hkl_head);
    bitmap_zero(mgmt->com_hkl_bitmap, mgmt->com_hkl_cnt);

    ret = osal_spin_lock_init(&mgmt->com_hkl_lock2);
    if (ret != HI_SUCCESS) {
        return;
    }
    ret = osal_mutex_init(&mgmt->ta_hkl.lock);
    if (ret != HI_SUCCESS) {
        return;
    }
    ret = osal_mutex_init(&mgmt->fp_hkl.lock);
    if (ret != HI_SUCCESS) {
        return;
    }
    ret = osal_mutex_init(&mgmt->nonce_hkl.lock);
    if (ret != HI_SUCCESS) {
        return;
    }
    ret = osal_mutex_init(&mgmt->clr_route.lock);
    if (ret != HI_SUCCESS) {
        return;
    }
    mgmt->ta_hkl_state = KLAD_CLOSED;
    mgmt->fp_hkl_state = KLAD_CLOSED;
    mgmt->nonce_hkl_state = KLAD_CLOSED;
    mgmt->clr_route_state = KLAD_CLOSED;

    /* add_debug_proc(mgmt); */
    return;
}

static inline hi_void __klad_mgmt_exit_impl(struct klad_mgmt *mgmt)
{
    osal_spin_lock_destory(&mgmt->com_hkl_lock2);
    osal_mutex_destory(&mgmt->com_hkl_lock);

    return;
}

static hi_s32 klad_monitor(hi_void *ptr)
{
    while (!osal_kthread_should_stop()) {
        /* do something~ */
        osal_msleep_uninterruptible(5); /* 5ms */
    }
    return HI_SUCCESS;
}

static hi_s32 _klad_mgmt_init_impl(struct klad_mgmt *mgmt)
{
    hi_s32 ret;

    WARN_ON(0 != atomic_read(&mgmt->ref_count));
    WARN_ON(mgmt->state != KLAD_MGMT_CLOSED);

    __klad_mgmt_init_impl(mgmt);

    mgmt->io_base = osal_ioremap_nocache(KLAD_REG_BASE, KLAD_REG_RANGE);
    if (mgmt->io_base == HI_NULL) {
        HI_FATAL_KLAD("osal_ioremap_nocache error!\n");
        ret = HI_ERR_KLAD_INVALID_PARAM;
        goto out0;
    }

    mgmt->monitor = kthread_create(klad_monitor, mgmt, "klad_monitor");
    WARN_ON(IS_ERR(mgmt->monitor));

    wake_up_process(mgmt->monitor);

    mgmt->state = KLAD_MGMT_OPENED;

    return HI_SUCCESS;

out0:
    __klad_mgmt_exit_impl(mgmt);
    return ret;
}

static hi_s32 klad_mgmt_init_impl(struct klad_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    __mutex_lock(&mgmt->lock);

    if (mgmt->state == KLAD_MGMT_CLOSED) {
        ret = _klad_mgmt_init_impl(mgmt);
        if (ret == HI_SUCCESS) {
            atomic_inc(&mgmt->ref_count);
        }
    } else if (mgmt->state == KLAD_MGMT_OPENED) {
        WARN_ON(atomic_read(&mgmt->ref_count) == 0);

        atomic_inc(&mgmt->ref_count);

        ret = HI_SUCCESS;
    } else {
        HI_FATAL_KLAD("mgmt state unknown, mgmt state is:%u\n", mgmt->state);
    }

    __mutex_unlock(&mgmt->lock);

    return ret;
}

static hi_s32 _klad_mgmt_exit_impl(struct klad_mgmt *mgmt)
{
    WARN_ON(atomic_read(&mgmt->ref_count) != 1);
    WARN_ON(mgmt->state != KLAD_MGMT_OPENED);

    kthread_stop(mgmt->monitor);
    osal_iounmap(mgmt->io_base);

    __klad_mgmt_exit_impl(mgmt);

    mgmt->state = KLAD_MGMT_CLOSED;

    return HI_SUCCESS;
}

static hi_s32 __klad_mgmt_rel_and_exit(struct klad_mgmt *mgmt)
{
    if (list_empty(&mgmt->com_hkl_head) && (mgmt->ta_hkl_state == KLAD_CLOSED) &&
            (mgmt->fp_hkl_state == KLAD_CLOSED) && (mgmt->nonce_hkl_state == KLAD_CLOSED) &&
            (mgmt->clr_route_state == KLAD_CLOSED)) {
        return _klad_mgmt_exit_impl(mgmt);
    }
    return _klad_mgmt_exit_impl(mgmt);
}

static hi_s32 _klad_mgmt_rel_and_exit(struct klad_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    if (mgmt->state == KLAD_MGMT_OPENED) {
        WARN_ON(atomic_read(&mgmt->ref_count) == 0);

        if (atomic_read(&mgmt->ref_count) == 1) {
            ret = __klad_mgmt_rel_and_exit(mgmt);
            if (ret == HI_SUCCESS) {
                atomic_dec(&mgmt->ref_count);
            }
        } else {
            atomic_dec(&mgmt->ref_count);

            ret = HI_SUCCESS;
        }
    } else if (mgmt->state == KLAD_MGMT_CLOSED) {
        WARN_ON(atomic_read(&mgmt->ref_count));

        ret = HI_SUCCESS;
    } else {
        HI_FATAL_KLAD("mgmt state unknown, mgmt state is:%u\n", mgmt->state);
    }
    return ret;
}

#define TEN_MSECS 10

static hi_s32 klad_mgmt_exit_impl(struct klad_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long start = osal_get_jiffies();
    unsigned long end = start + HZ; /* 1s */

    do {
        __mutex_lock(&mgmt->lock);

        ret = _klad_mgmt_rel_and_exit(mgmt);

        __mutex_unlock(&mgmt->lock);

        osal_msleep_uninterruptible(TEN_MSECS);

    } while (ret != HI_SUCCESS && time_in_range((unsigned long)osal_get_jiffies(), start, end));

    return ret;
}

static hi_s32 __klad_mgmt_suspend_impl(struct klad_mgmt *mgmt)
{
    hi_s32 ret;
    struct list_head *node = HI_NULL;

    __mutex_lock(&mgmt->com_hkl_lock);
    list_for_each(node, &mgmt->com_hkl_head) {
        struct klad_r_com_hkl *hkl = list_entry(node, struct klad_r_com_hkl, node);
        ret = hkl->ops->suspend(hkl);
        if (ret != HI_SUCCESS) {
            print_err_func_hex3(hkl->ops->suspend, hkl->base.handle, hkl->base.hw_id, ret);
        }
    }
    __mutex_unlock(&mgmt->com_hkl_lock);

    if (mgmt->ta_hkl_state == KLAD_OPENED) {
        struct klad_r_ta_hkl *hkl = &mgmt->ta_hkl;

        ret = hkl->ops->suspend(hkl);
        if (ret != HI_SUCCESS) {
            print_err_func_hex3(hkl->ops->suspend, hkl->base.handle, hkl->base.hw_id, ret);
        }
    }

    if (mgmt->fp_hkl_state == KLAD_OPENED) {
        struct klad_r_fp_hkl *hkl = &mgmt->fp_hkl;

        ret = hkl->ops->suspend(hkl);
        if (ret != HI_SUCCESS) {
            print_err_func_hex3(hkl->ops->suspend, hkl->base.handle, hkl->base.hw_id, ret);
        }
    }

    if (mgmt->nonce_hkl_state == KLAD_OPENED) {
        struct klad_r_nonce_hkl *hkl = &mgmt->nonce_hkl;

        ret = hkl->ops->suspend(hkl);
        if (ret != HI_SUCCESS) {
            print_err_func_hex3(hkl->ops->suspend, hkl->base.handle, hkl->base.hw_id, ret);
        }
    }
#if HI_INT_SUPPORT
    hkl_int_deinit();
    rkp_int_deinit();
#endif
    return HI_SUCCESS;
}

/*
 * Success must be returned regardless of whether MGMT is initialized.
 * otherwise it will affect the entire standby process.
 */
static hi_s32 klad_mgmt_suspend_impl(struct klad_mgmt *mgmt)
{
    hi_s32 ret;

    __mutex_lock(&mgmt->lock);

    if (mgmt->state != KLAD_MGMT_OPENED) {
        ret = HI_ERR_KLAD_NOT_INIT;
        goto out;
    }

    atomic_inc(&mgmt->ref_count);

    ret = __klad_mgmt_suspend_impl(mgmt);

    atomic_dec(&mgmt->ref_count);
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 __klad_mgmt_resume_impl(struct klad_mgmt *mgmt)
{
    hi_s32 ret;
    struct list_head *node = HI_NULL;

#if HI_INT_SUPPORT
    hkl_int_init();
    rkp_int_init();
#endif
#ifndef HI_TEE_SUPPORT
    hkl_perm_multi2_cfg();
#endif

    __mutex_lock(&mgmt->com_hkl_lock);
    list_for_each(node, &mgmt->com_hkl_head) {
        struct klad_r_com_hkl *hkl = list_entry(node, struct klad_r_com_hkl, node);
        ret = hkl->ops->resume(hkl);
        if (ret != HI_SUCCESS) {
            print_err_func_hex3(hkl->ops->suspend, hkl->base.handle, hkl->base.hw_id, ret);
        }
    }
    __mutex_unlock(&mgmt->com_hkl_lock);

    if (mgmt->ta_hkl_state == KLAD_OPENED) {
        struct klad_r_ta_hkl *hkl = &mgmt->ta_hkl;

        ret = hkl->ops->resume(hkl);
        if (ret != HI_SUCCESS) {
            print_err_func_hex3(hkl->ops->suspend, hkl->base.handle, hkl->base.hw_id, ret);
        }
    }

    if (mgmt->fp_hkl_state == KLAD_OPENED) {
        struct klad_r_fp_hkl *hkl = &mgmt->fp_hkl;

        ret = hkl->ops->resume(hkl);
        if (ret != HI_SUCCESS) {
            print_err_func_hex3(hkl->ops->suspend, hkl->base.handle, hkl->base.hw_id, ret);
        }
    }

    if (mgmt->nonce_hkl_state == KLAD_OPENED) {
        struct klad_r_nonce_hkl *hkl = &mgmt->nonce_hkl;

        ret = hkl->ops->resume(hkl);
        if (ret != HI_SUCCESS) {
            print_err_func_hex3(hkl->ops->suspend, hkl->base.handle, hkl->base.hw_id, ret);
        }
    }
    return HI_SUCCESS;
}

/*
 * Success must be returned regardless of whether MGMT is initialized.
 * otherwise it will affect the entire standby process.
 */
static hi_s32 klad_mgmt_resume_impl(struct klad_mgmt *mgmt)
{
    hi_s32 ret;

    __mutex_lock(&mgmt->lock);

    if (mgmt->state != KLAD_MGMT_OPENED) {
        ret = HI_ERR_KLAD_NOT_INIT;
        goto out;
    }

    atomic_inc(&mgmt->ref_count);

    ret = __klad_mgmt_resume_impl(mgmt);

    atomic_dec(&mgmt->ref_count);

out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_void __klad_mgmt_gen_base_attr(struct klad_r_base_attr *base_attr, struct klad_entry_key *hkl_attr)
{
    hi_s32 ret;

    ret = memcpy_s(base_attr, sizeof(struct klad_r_base_attr),
        &hkl_attr->hkl_base_attr, sizeof(struct klad_r_base_attr));
    if (ret != EOK) {
        print_err_func(memcpy_s, ret);
    }
}

static hi_bool __klad_mgmt_same_klad(struct klad_r_base_attr *src, struct klad_r_base_attr *dst)
{
    if ((src->klad_type == dst->klad_type) &&
        (src->vendor_id == dst->vendor_id) &&
        (src->module_id[0] == dst->module_id[0]) &&
        (src->module_id[0x1] == dst->module_id[0x1]) &&
        (src->module_id[0x2] == dst->module_id[0x2]) &&
        (src->module_id[0x3] == dst->module_id[0x3])) {
        return HI_TRUE;
    }
    return HI_FALSE;
}

static hi_s32 __klad_mgmt_com_hkl_find(struct klad_mgmt *mgmt,
                                       struct klad_entry_key *hkl_attr,
                                       struct klad_r_com_hkl **com_hkl)
{
    hi_s32 ret = HI_FAILURE;
    struct list_head *com_hkl_node = HI_NULL;

    __mutex_lock(&mgmt->com_hkl_lock);
    list_for_each(com_hkl_node, &mgmt->com_hkl_head) {
        struct klad_r_com_hkl *hkl = list_entry(com_hkl_node, struct klad_r_com_hkl, node);

        if (__klad_mgmt_same_klad(&hkl->base_attr, &hkl_attr->hkl_base_attr)) {
            *com_hkl = hkl;
            ret = HI_SUCCESS;
            goto out;
        }
    }
out:
    __mutex_unlock(&mgmt->com_hkl_lock);
    return ret;
}

static hi_s32 __klad_mgmt_destroy_com_hkl_impl(struct klad_mgmt *mgmt, struct klad_r_com_hkl *com_hkl)
{
    hi_s32 ret;
    unsigned long mask, *p;

    mask = BIT_MASK(com_hkl->base.id);
    p = ((unsigned long *)mgmt->com_hkl_bitmap) + BIT_WORD(com_hkl->base.id);
    if (!(*p & mask)) {
        HI_ERR_KLAD("com hkl(%d) is invalid.\n", com_hkl->base.id);
        ret = HI_ERR_KLAD_INVALID_PARAM;
        goto out;
    }
    clear_bit(com_hkl->base.id, mgmt->com_hkl_bitmap);
    HI_DBG_KLAD("#    robj(0x%p) deortroyed(%d).\n", com_hkl, com_hkl->base.hw_id);

    klad_timestamp_queue(com_hkl->base.hw_id, com_hkl->base.handle, &com_hkl->base);

    osal_mutex_destory(&com_hkl->lock);

    list_del(&com_hkl->node);
    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 klad_mgmt_destroy_com_hkl_impl(struct klad_mgmt *mgmt, struct klad_r_com_hkl *com_hkl)
{
    hi_s32 ret;

    __mutex_lock(&mgmt->lock);
    if (mgmt->state != KLAD_MGMT_OPENED) {
        HI_ERR_KLAD("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }
    /*
    * ********************* NOTE ********************
    * If one process have generate mgmt->lock and found keyladder instance has created.
    * At the same time other process close the instance, ref_count has reduced to 0(call api klad_r_put->...->release)
    * obj->release will be called, but can not get mgmt->lock,so this process will wait mgmt->lock be unlocked.
    * and then destory the instance,if we don't check ref_count again,  the first process will acess the relased.
    * instance.
    */
    if (atomic_read(&com_hkl->base.ref_count) != 0) {
        ret = HI_SUCCESS;
        goto out;
    }

    ret = com_hkl->ops->close(com_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(com_hkl->ops->close, ret);
    }

    ret = __klad_mgmt_destroy_com_hkl_impl(mgmt, com_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_mgmt_destroy_com_hkl_impl, ret);
    }

out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 __klad_mgmt_create_com_hkl_impl(struct klad_mgmt *mgmt,
                                              struct klad_entry_key *hkl_attr,
                                              struct klad_r_com_hkl **com_hkl)
{
    hi_s32 ret;
    hi_u32 id;
    struct klad_r_com_hkl *new_com_hkl = HI_NULL;

    ret = __klad_mgmt_com_hkl_find(mgmt, hkl_attr, com_hkl);
    if (ret == HI_SUCCESS) { /* current com hw keyladder has been created. */
        atomic_inc(&(*com_hkl)->base.ref_count);
        timestamp_clean(&(*com_hkl)->base);
        goto out;
    }
    id = find_first_zero_bit(mgmt->com_hkl_bitmap, mgmt->com_hkl_cnt);
    if (!(id < mgmt->com_hkl_cnt)) {
        HI_ERR_KLAD("there is no available com klad now!\n");
        ret = HI_ERR_KLAD_NO_RESOURCE;
        goto out;
    }
    new_com_hkl = &mgmt->com_hkl[id];

    if (memset_s(new_com_hkl, sizeof(struct klad_r_com_hkl), 0, sizeof(struct klad_r_com_hkl)) != EOK) {
        print_err_func(memset_s, HI_ERR_KLAD_SEC_FAILED);
        ret = HI_ERR_KLAD_SEC_FAILED;
        goto out;
    }

    new_com_hkl->base.ops     = get_klad_r_base_ops();
    new_com_hkl->base.release = klad_mgmt_destroy_com_hkl;
    new_com_hkl->base.id      = id;
    new_com_hkl->base.handle  = HI_HANDLE_INIT(HI_ID_KLAD, HI_KLAD_COM, id);
    new_com_hkl->base.mgmt    = mgmt;
    osal_mutex_init(&new_com_hkl->lock);
    new_com_hkl->ops = &g_com_klad_ops;

    __klad_mgmt_gen_base_attr(&new_com_hkl->base_attr, hkl_attr);

    atomic_set(&new_com_hkl->base.ref_count, 1);
    list_add_tail(&new_com_hkl->node, &mgmt->com_hkl_head);

    *com_hkl = new_com_hkl;

    HI_DBG_KLAD("#    robj(0x%p) created(%d).\n", new_com_hkl, new_com_hkl->base.hw_id);

    set_bit(id, mgmt->com_hkl_bitmap);
    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 klad_mgmt_create_com_hkl_impl(struct klad_mgmt *mgmt,
                                            struct klad_entry_key *hkl_attr,
                                            struct klad_r_com_hkl **com_hkl)
{
    hi_s32 ret;
    struct time_ns time_b = {0};

    __mutex_lock(&mgmt->lock);

    timestamp(&time_b);

    if (mgmt->state != KLAD_MGMT_OPENED) {
        HI_ERR_KLAD("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = __klad_mgmt_create_com_hkl_impl(mgmt, hkl_attr, com_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_mgmt_create_com_hkl_impl, ret);
        goto out;
    }

    ret = (*com_hkl)->ops->open(*com_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(com_hkl->ops->open, ret);
        goto out1;
    }

    __mutex_unlock(&mgmt->lock);
    return ret;
out1:
    ret = __klad_mgmt_destroy_com_hkl_impl(mgmt, *com_hkl);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_mgmt_destroy_com_hkl_impl, ret);
    }

out:
    timestamp_tag(&(*com_hkl)->base, TIMETAMP_CREATE_O);
    timestamp_add(&time_b, &(*com_hkl)->base, TIMETAMP_CREATE_I);

    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 __klad_mgmt_create_ta_hkl_impl(struct klad_mgmt *mgmt,
                                             struct klad_entry_key *hkl_attr,
                                             struct klad_r_com_hkl *com_hkl,
                                             struct klad_r_ta_hkl **ta_hkl)
{
    hi_s32 ret;
    struct klad_r_ta_hkl *new_ta_hkl = HI_NULL;

    if (mgmt->ta_hkl_state == KLAD_OPENED) {
        if (__klad_mgmt_same_klad(&mgmt->ta_hkl.base_attr, &hkl_attr->hkl_base_attr)) {
            *ta_hkl = &mgmt->ta_hkl;
            ret = HI_SUCCESS;
        } else {
            ret = HI_ERR_KLAD_NO_RESOURCE;
        }
        goto out;
    }

    new_ta_hkl = &mgmt->ta_hkl;
    if (memset_s(new_ta_hkl, sizeof(struct klad_r_ta_hkl), 0, sizeof(struct klad_r_ta_hkl)) != EOK) {
        ret = HI_ERR_KLAD_SEC_FAILED;
        goto out;
    }
    new_ta_hkl->base.ops     = get_klad_r_base_ops();
    new_ta_hkl->base.release = klad_mgmt_destroy_com_hkl;
    new_ta_hkl->base.id      = 1; /* Only one keyladder support. */
    new_ta_hkl->base.handle  = HI_HANDLE_INIT(HI_ID_KLAD, HI_KLAD_TA, 1);
    new_ta_hkl->base.mgmt    = mgmt;

    osal_mutex_init(&new_ta_hkl->lock);
    new_ta_hkl->ops = &g_ta_klad_ops;

    new_ta_hkl->com_obj = (struct klad_r_base *)com_hkl;
    com_hkl->associate_obj = (struct klad_r_base *)new_ta_hkl;

    __klad_mgmt_gen_base_attr(&new_ta_hkl->base_attr, hkl_attr);

    atomic_set(&new_ta_hkl->base.ref_count, 1);
    mgmt->ta_hkl_state = KLAD_OPENED;
    *ta_hkl = new_ta_hkl;

    HI_DBG_KLAD("#    robj(0x%p) created.\n", new_ta_hkl);
    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 klad_mgmt_create_ta_hkl_impl(struct klad_mgmt *mgmt,
                                           struct klad_entry_key *hkl_attr,
                                           struct klad_r_ta_hkl **ta_hkl)
{
    hi_s32 ret;
    struct klad_r_com_hkl *com_hkl = HI_NULL;
    struct time_ns time_b = {0};

    __mutex_lock(&mgmt->lock);

    timestamp(&time_b);

    if (mgmt->state != KLAD_MGMT_OPENED) {
        HI_ERR_KLAD("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = __klad_mgmt_create_com_hkl_impl(mgmt, hkl_attr, &com_hkl);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = __klad_mgmt_create_ta_hkl_impl(mgmt, hkl_attr, com_hkl, ta_hkl);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    timestamp_tag(&(*ta_hkl)->base, TIMETAMP_CREATE_O);
    timestamp_add(&time_b, &(*ta_hkl)->base, TIMETAMP_CREATE_I);

    __mutex_unlock(&mgmt->lock);
    return HI_SUCCESS;
out1:
    if (__klad_mgmt_destroy_com_hkl_impl(mgmt, com_hkl) != HI_SUCCESS) {
        HI_ERR_KLAD("failed to destroy com keyladder hw instance(%d).\n",
                    com_hkl->base.hw_id);
    }
out:
    timestamp_tag(&(*ta_hkl)->base, TIMETAMP_CREATE_O);
    timestamp_add(&time_b, &(*ta_hkl)->base, TIMETAMP_CREATE_I);

    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 __klad_mgmt_destroy_ta_hkl_impl(struct klad_mgmt *mgmt, struct klad_r_ta_hkl *ta_hkl)
{
    hi_s32 ret;

    mgmt->ta_hkl_state = KLAD_CLOSED;

    klad_timestamp_queue(ta_hkl->base.hw_id, ta_hkl->base.handle, &ta_hkl->base);

    osal_mutex_destory(&ta_hkl->lock);

    ret = memset_s(ta_hkl, sizeof(struct klad_r_ta_hkl), 0, sizeof(struct klad_r_ta_hkl));
    if (ret != EOK) {
        HI_DBG_KLAD("clear ta hkl buffer failed.\n");
        goto out;
    }
    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 klad_mgmt_destroy_ta_hkl_impl(struct klad_mgmt *mgmt, struct klad_r_ta_hkl *ta_hkl)
{
    hi_s32 ret;

    __mutex_lock(&mgmt->lock);
    if (mgmt->state != KLAD_MGMT_OPENED) {
        HI_ERR_KLAD("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }
    /*
    * ********************* NOTE ********************
    * If one process have generate mgmt->lock and found keyladder instance has created.
    * At the same time other process close the instance, ref_count has reduced to 0(call api klad_r_put->...->release)
    * obj->release will be called, but can not get mgmt->lock,so this process will wait mgmt->lock be unlocked.
    * and then destory the instance,if we don't check ref_count again,  the first process will acess the relased.
    * instance.
    */
    if (atomic_read(&ta_hkl->base.ref_count) != 0) {
        ret = HI_SUCCESS;
        goto out;
    }
    ret = __klad_mgmt_destroy_com_hkl_impl(mgmt, (struct klad_r_com_hkl *)(ta_hkl->com_obj));
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Failed to destroy com keyladder.\n");
    }

    ret = __klad_mgmt_destroy_ta_hkl_impl(mgmt, ta_hkl);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Failed to destroy ta keyladder.\n");
    }
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 __klad_mgmt_create_fp_hkl_impl(struct klad_mgmt *mgmt,
                                             struct klad_entry_key *hkl_attr,
                                             struct klad_r_com_hkl *com_hkl,
                                             struct klad_r_fp_hkl **fp_hkl)
{
    hi_s32 ret;
    struct klad_r_fp_hkl *new_fp_hkl = HI_NULL;

    if (mgmt->fp_hkl_state == KLAD_OPENED) {
        if (__klad_mgmt_same_klad(&mgmt->fp_hkl.base_attr, &hkl_attr->hkl_base_attr)) {
            atomic_inc(&mgmt->fp_hkl.base.ref_count);
            *fp_hkl = &mgmt->fp_hkl;
            ret = HI_SUCCESS;
        } else {
            ret = HI_ERR_KLAD_NO_RESOURCE;
        }
        goto out;
    }

    new_fp_hkl = &mgmt->fp_hkl;
    if (memset_s(new_fp_hkl, sizeof(struct klad_r_fp_hkl), 0, sizeof(struct klad_r_fp_hkl)) != EOK) {
        ret = HI_ERR_KLAD_SEC_FAILED;
        goto out;
    }
    new_fp_hkl->base.ops     = get_klad_r_base_ops();
    new_fp_hkl->base.release = klad_mgmt_destroy_fp_hkl;
    new_fp_hkl->base.id      = 1; /* Only one keyladder support. */
    new_fp_hkl->base.handle  = HI_HANDLE_INIT(HI_ID_KLAD, HI_KLAD_FP, 1);
    new_fp_hkl->base.mgmt    = mgmt;

    osal_mutex_init(&new_fp_hkl->lock);
    new_fp_hkl->ops = &g_fp_klad_ops;

    new_fp_hkl->com_obj = (struct klad_r_base *)com_hkl;
    com_hkl->associate_obj = (struct klad_r_base *)new_fp_hkl;

    __klad_mgmt_gen_base_attr(&new_fp_hkl->base_attr, hkl_attr);

    mgmt->fp_hkl_state = KLAD_OPENED;
    atomic_set(&new_fp_hkl->base.ref_count, 1);
    *fp_hkl = new_fp_hkl;

    HI_DBG_KLAD("#    robj(0x%p) created.\n", new_fp_hkl);
    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 klad_mgmt_create_fp_hkl_impl(struct klad_mgmt *mgmt,
                                           struct klad_entry_key *hkl_attr,
                                           struct klad_r_fp_hkl **fp_hkl)
{
    hi_s32 ret;
    struct klad_r_com_hkl *com_hkl = HI_NULL;
    struct time_ns time_b = {0};

    __mutex_lock(&mgmt->lock);

    timestamp(&time_b);

    if (mgmt->state != KLAD_MGMT_OPENED) {
        HI_ERR_KLAD("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = __klad_mgmt_create_com_hkl_impl(mgmt, hkl_attr, &com_hkl);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = __klad_mgmt_create_fp_hkl_impl(mgmt, hkl_attr, com_hkl, fp_hkl);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    timestamp_tag(&(*fp_hkl)->base, TIMETAMP_CREATE_O);
    timestamp_add(&time_b, &(*fp_hkl)->base, TIMETAMP_CREATE_I);

    __mutex_unlock(&mgmt->lock);
    return HI_SUCCESS;
out1:
    if (__klad_mgmt_destroy_com_hkl_impl(mgmt, com_hkl) != HI_SUCCESS) {
        HI_ERR_KLAD("failed to destroy com keyladder hw instance(%d).\n",
                    com_hkl->base.hw_id);
    }
out:

    timestamp_tag(&(*fp_hkl)->base, TIMETAMP_CREATE_O);
    timestamp_add(&time_b, &(*fp_hkl)->base, TIMETAMP_CREATE_I);
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 __klad_mgmt_destroy_fp_hkl_impl(struct klad_mgmt *mgmt, struct klad_r_fp_hkl *fp_hkl)
{
    hi_s32 ret;

    mgmt->fp_hkl_state = KLAD_CLOSED;
    klad_timestamp_queue(fp_hkl->base.hw_id, fp_hkl->base.handle, &fp_hkl->base);

    osal_mutex_destory(&fp_hkl->lock);

    ret = memset_s(fp_hkl, sizeof(struct klad_r_fp_hkl), 0, sizeof(struct klad_r_fp_hkl));
    if (ret != EOK) {
        HI_DBG_KLAD("clear fp hkl buffer failed.\n");
        goto out;
    }
    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 klad_mgmt_destroy_fp_hkl_impl(struct klad_mgmt *mgmt, struct klad_r_fp_hkl *fp_hkl)
{
    hi_s32 ret;

    __mutex_lock(&mgmt->lock);
    if (mgmt->state != KLAD_MGMT_OPENED) {
        HI_ERR_KLAD("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }
    /*
    * ********************* NOTE ********************
    * If one process have generate mgmt->lock and found keyladder instance has created.
    * At the same time other process close the instance, ref_count has reduced to 0(call api klad_r_put->...->release)
    * obj->release will be called, but can not get mgmt->lock,so this process will wait mgmt->lock be unlocked.
    * and then destory the instance,if we don't check ref_count again,  the first process will acess the relased.
    * instance.
    */
    if (atomic_read(&fp_hkl->base.ref_count) != 0) {
        ret = HI_SUCCESS;
        goto out;
    }
    ret = __klad_mgmt_destroy_com_hkl_impl(mgmt, (struct klad_r_com_hkl *)fp_hkl->com_obj);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Failed to destroy com keyladder.\n");
    }

    ret = __klad_mgmt_destroy_fp_hkl_impl(mgmt, fp_hkl);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Failed to destroy fp keyladder.\n");
    }
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 __klad_mgmt_create_nonce_hkl_impl(struct klad_mgmt *mgmt,
                                                struct klad_entry_key *hkl_attr,
                                                struct klad_r_com_hkl *com_hkl,
                                                struct klad_r_nonce_hkl **nonce_hkl)
{
    hi_s32 ret;
    struct klad_r_nonce_hkl *new_nonce_hkl = HI_NULL;

    if (mgmt->nonce_hkl_state == KLAD_OPENED) {
        if (__klad_mgmt_same_klad(&mgmt->nonce_hkl.base_attr, &hkl_attr->hkl_base_attr)) {
            atomic_inc(&mgmt->nonce_hkl.base.ref_count);
            *nonce_hkl = &mgmt->nonce_hkl;
            ret = HI_SUCCESS;
        } else {
            ret = HI_ERR_KLAD_NO_RESOURCE;
        }
        goto out;
    }
    new_nonce_hkl = &mgmt->nonce_hkl;
    if (memset_s(new_nonce_hkl, sizeof(struct klad_r_nonce_hkl), 0, sizeof(struct klad_r_nonce_hkl)) != EOK) {
        ret = HI_ERR_KLAD_SEC_FAILED;
        goto out;
    }
    new_nonce_hkl->base.ops     = get_klad_r_base_ops();
    new_nonce_hkl->base.release = klad_mgmt_destroy_nonce_hkl;
    new_nonce_hkl->base.id      = 1; /* Only one keyladder support. */
    new_nonce_hkl->base.handle  = HI_HANDLE_INIT(HI_ID_KLAD, HI_KLAD_NONCE, 1);
    new_nonce_hkl->base.mgmt    = mgmt;

    osal_mutex_init(&new_nonce_hkl->lock);
    new_nonce_hkl->ops = &g_nonce_klad_ops;

    new_nonce_hkl->com_obj = (struct klad_r_base *)com_hkl;
    com_hkl->associate_obj = (struct klad_r_base *)new_nonce_hkl;

    __klad_mgmt_gen_base_attr(&new_nonce_hkl->base_attr, hkl_attr);

    atomic_set(&new_nonce_hkl->base.ref_count, 1);
    mgmt->nonce_hkl_state = KLAD_OPENED;

    *nonce_hkl = new_nonce_hkl;

    HI_DBG_KLAD("#    robj(0x%p) created.\n", new_nonce_hkl);
    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 klad_mgmt_create_nonce_hkl_impl(struct klad_mgmt *mgmt,
                                              struct klad_entry_key *hkl_attr,
                                              struct klad_r_nonce_hkl **nonce_hkl)
{
    hi_s32 ret;
    struct klad_r_com_hkl *com_hkl = HI_NULL;
    struct time_ns time_b = {0};

    __mutex_lock(&mgmt->lock);

    timestamp(&time_b);

    if (mgmt->state != KLAD_MGMT_OPENED) {
        HI_ERR_KLAD("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = __klad_mgmt_create_com_hkl_impl(mgmt, hkl_attr, &com_hkl);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = __klad_mgmt_create_nonce_hkl_impl(mgmt, hkl_attr, com_hkl, nonce_hkl);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

    timestamp_tag(&(*nonce_hkl)->base, TIMETAMP_CREATE_O);
    timestamp_add(&time_b, &(*nonce_hkl)->base, TIMETAMP_CREATE_I);

    __mutex_unlock(&mgmt->lock);
    return HI_SUCCESS;
out1:
    if (__klad_mgmt_destroy_com_hkl_impl(mgmt, com_hkl) != HI_SUCCESS) {
        HI_ERR_KLAD("failed to destroy com keyladder hw instance(%d).\n",
                    com_hkl->base.hw_id);
    }
out:
    timestamp_tag(&(*nonce_hkl)->base, TIMETAMP_CREATE_O);
    timestamp_add(&time_b, &(*nonce_hkl)->base, TIMETAMP_CREATE_I);

    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 __klad_mgmt_destroy_nonce_hkl_impl(struct klad_mgmt *mgmt, struct klad_r_nonce_hkl *nonce_hkl)
{
    hi_s32 ret;

    mgmt->nonce_hkl_state = KLAD_CLOSED;

    klad_timestamp_queue(nonce_hkl->base.hw_id, nonce_hkl->base.handle, &nonce_hkl->base);

    osal_mutex_destory(&nonce_hkl->lock);

    ret = memset_s(&nonce_hkl, sizeof(struct klad_r_nonce_hkl), 0, sizeof(struct klad_r_nonce_hkl));
    if (ret != EOK) {
        HI_DBG_KLAD("clear ta hkl buffer failed.\n");
        goto out;
    }
    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 klad_mgmt_destroy_nonce_hkl_impl(struct klad_mgmt *mgmt, struct klad_r_nonce_hkl *nonce_hkl)
{
    hi_s32 ret;

    __mutex_lock(&mgmt->lock);
    if (mgmt->state != KLAD_MGMT_OPENED) {
        HI_ERR_KLAD("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }
    /*
    * If one process have generate mgmt->lock and found keyladder instance has created.
    * At the same time other process close the instance, ref_count has reduced to 0(call api klad_r_put->...->release)
    * obj->release will be called, but can not get mgmt->lock,so this process will wait mgmt->lock be unlocked.
    * and then destory the instance,if we don't check ref_count again,  the first process will acess the relased.
    * instance.
    */
    if (atomic_read(&nonce_hkl->base.ref_count) != 0) {
        ret = HI_SUCCESS;
        goto out;
    }
    ret = __klad_mgmt_destroy_com_hkl_impl(mgmt, (struct klad_r_com_hkl *)nonce_hkl->com_obj);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Failed to destroy com keyladder.\n");
    }

    ret = __klad_mgmt_destroy_nonce_hkl_impl(mgmt, nonce_hkl);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Failed to destroy fp keyladder.\n");
    }
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 __klad_mgmt_create_clr_route_impl(struct klad_mgmt *mgmt,
                                                struct klad_entry_key *hkl_attr,
                                                struct klad_r_clr_route **clr_route)
{
    hi_s32 ret;
    struct klad_r_clr_route *new_clr_route = HI_NULL;

    if (mgmt->clr_route_state == KLAD_OPENED) {
        if (mgmt->clr_route.klad_type == hkl_attr->hkl_base_attr.klad_type) {
            atomic_inc(&mgmt->clr_route.base.ref_count);
            *clr_route = &mgmt->clr_route;
            ret = HI_SUCCESS;
        } else {
            ret = HI_ERR_KLAD_NO_RESOURCE;
        }
        goto out;
    }

    new_clr_route = &mgmt->clr_route;
    if (memset_s(new_clr_route, sizeof(struct klad_r_clr_route), 0, sizeof(struct klad_r_clr_route)) != EOK) {
        ret = HI_ERR_KLAD_SEC_FAILED;
        goto out;
    }
    new_clr_route->base.ops     = get_klad_r_base_ops();
    new_clr_route->base.release = klad_mgmt_destroy_clr_route;
    new_clr_route->base.id      = 1; /* Only one keyladder support. */
    new_clr_route->base.handle  = HI_HANDLE_INIT(HI_ID_KLAD, HI_KLAD_CLR, 1);
    new_clr_route->base.mgmt    = mgmt;

    if (osal_mutex_init(&new_clr_route->lock) != HI_SUCCESS) {
        HI_ERR_KLAD("Init mutex failed.\n");
        return HI_FAILURE;
    }

    new_clr_route->ops = &g_clr_route_ops;

    new_clr_route->klad_type = hkl_attr->hkl_base_attr.klad_type;

    mgmt->clr_route_state = KLAD_OPENED;
    atomic_set(&new_clr_route->base.ref_count, 1);

    *clr_route = new_clr_route;

    HI_DBG_KLAD("#    robj(0x%p) created.\n", new_clr_route);
    ret = HI_SUCCESS;

out:
    return ret;
}

static hi_s32 klad_mgmt_create_clr_route_impl(struct klad_mgmt *mgmt,
                                              struct klad_entry_key *hkl_attr,
                                              struct klad_r_clr_route **clr_route)
{
    hi_s32 ret;
    struct time_ns time_b = {0};

    __mutex_lock(&mgmt->lock);

    timestamp(&time_b);

    if (mgmt->state != KLAD_MGMT_OPENED) {
        HI_ERR_KLAD("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = __klad_mgmt_create_clr_route_impl(mgmt, hkl_attr, clr_route);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_mgmt_create_clr_route_impl, ret);
    }
out:

    timestamp_tag(&(*clr_route)->base, TIMETAMP_CREATE_O);
    timestamp_add(&time_b, &(*clr_route)->base, TIMETAMP_CREATE_I);
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 __klad_mgmt_destroy_clr_route_impl(struct klad_mgmt *mgmt, struct klad_r_clr_route *clr_route)
{
    hi_s32 ret;

    mgmt->clr_route_state = KLAD_CLOSED;

    klad_timestamp_queue(clr_route->base.hw_id, clr_route->base.handle, &clr_route->base);

    osal_mutex_destory(&clr_route->lock);

    ret = memset_s(clr_route, sizeof(struct klad_r_clr_route), 0, sizeof(struct klad_r_clr_route));
    if (ret != EOK) {
        HI_DBG_KLAD("clear ta hkl buffer failed.\n");
        goto out;
    }
    ret = HI_SUCCESS;
out:
    return ret;
}

static hi_s32 klad_mgmt_destroy_clr_route_impl(struct klad_mgmt *mgmt, struct klad_r_clr_route *clr_route)
{
    hi_s32 ret;

    __mutex_lock(&mgmt->lock);
    if (mgmt->state != KLAD_MGMT_OPENED) {
        HI_ERR_KLAD("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }
    /*
    * ********************* NOTE ********************
    * If one process have generate mgmt->lock and found keyladder instance has created.
    * At the same time other process close the instance, ref_count has reduced to 0(call api klad_r_put->...->release)
    * obj->release will be called, but can not get mgmt->lock,so this process will wait mgmt->lock be unlocked.
    * and then destory the instance,if we don't check ref_count again,  the first process will acess the relased.
    * instance.
    */
    if (atomic_read(&clr_route->base.ref_count) != 0) {
        ret = HI_SUCCESS;
        goto out;
    }

    ret = __klad_mgmt_destroy_clr_route_impl(mgmt, clr_route);
    if (ret != HI_SUCCESS) {
        HI_ERR_KLAD("Failed to destroy clr keyladder.\n");
    }
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_void __klad_mgmt_show_info_impl(struct klad_mgmt *mgmt)
{
    struct list_head *node = HI_NULL;

    HI_ERR_KLAD("* mgmt refer count:%d.\n", atomic_read(&mgmt->ref_count));

    list_for_each(node, &mgmt->com_hkl_head) {
        (void *)list_entry(node, struct klad_r_com_hkl, node);
    }

    return;
}

static hi_void klad_mgmt_show_info_impl(struct klad_mgmt *mgmt)
{
    __mutex_lock(&mgmt->lock);

    if (mgmt->state != KLAD_MGMT_OPENED) {
        HI_ERR_KLAD("mgmt has not opened.\n");
        goto out;
    }

    __klad_mgmt_show_info_impl(mgmt);

out:
    __mutex_unlock(&mgmt->lock);
}

static struct klad_mgmt_ops g_klad_mgmt_ops = {
    .init                = klad_mgmt_init_impl,
    .exit                = klad_mgmt_exit_impl,

    .suspend             = klad_mgmt_suspend_impl,
    .resume              = klad_mgmt_resume_impl,

    .create_com_hkl      = klad_mgmt_create_com_hkl_impl,
    .destroy_com_hkl     = klad_mgmt_destroy_com_hkl_impl,

    .create_ta_hkl       = klad_mgmt_create_ta_hkl_impl,
    .destroy_ta_hkl      = klad_mgmt_destroy_ta_hkl_impl,

    .create_fp_hkl       = klad_mgmt_create_fp_hkl_impl,
    .destroy_fp_hkl      = klad_mgmt_destroy_fp_hkl_impl,

    .create_nonce_hkl    = klad_mgmt_create_nonce_hkl_impl,
    .destroy_nonce_hkl   = klad_mgmt_destroy_nonce_hkl_impl,

    .create_clr_route    = klad_mgmt_create_clr_route_impl,
    .destroy_clr_route   = klad_mgmt_destroy_clr_route_impl,

    .show_info           = klad_mgmt_show_info_impl,
};

static struct klad_mgmt g_klad_mgmt = {
    .lock      = {0},
    .ref_count = ATOMIC_INIT(0),
    .state     = KLAD_MGMT_CLOSED,
    .ops       = &g_klad_mgmt_ops,
};

struct klad_mgmt *__get_klad_mgmt(hi_void)
{
    return &g_klad_mgmt;
}

/*
 * it needs to be guaranteed that TsioMgmt has inited here.
 */
struct klad_mgmt *get_klad_mgmt(hi_void)
{
    struct klad_mgmt *mgmt = __get_klad_mgmt();

    WARN_ON(atomic_read(&mgmt->ref_count) == 0);

    return mgmt;
}
