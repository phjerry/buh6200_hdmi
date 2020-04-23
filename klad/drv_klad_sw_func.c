/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: klad basic function impl.
* Author: sdk team
* Create: 2019-07-28
*/

#include "drv_klad_sw_func.h"

#include <linux/kernel.h>
#include <linux/kthread.h>
#include "linux/delay.h"
#include "linux/bitmap.h"
#include "linux/io.h"
#include "linux/interrupt.h"
#include "linux/ratelimit.h"

#include "hi_drv_module.h"
#include "hi_drv_sys.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_drv_mem.h"
#include "hi_drv_osal.h"
#include "hi_drv_demux.h"

#include "linux/hisilicon/securec.h"
#include "hi_errno.h"
#include "hi_drv_klad.h"
#include "drv_klad_sw_utils.h"
#include "drv_klad_hw.h"
#include "hi_osal.h"

/*
 * API : klad_slot management defination.
 */
static hi_s32 __klad_slot_type(hi_handle handle, hi_u32 *type)
{
    hi_s32 ret;
    struct klad_sw_r_base *base = HI_NULL;

    ret = klad_sw_r_get(handle, &base);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_sw_r_get, ret);
        goto out;
    }

    *type = base->klad_type;

    klad_sw_r_put(base);
out:
    return ret;
}

hi_s32 klad_slot_mgmt_init(hi_void)
{
    hi_s32 ret;
    struct klad_slot_mgmt *mgmt = __get_klad_slot_mgmt();
    struct klad_slot_table *mgmt_slot_table = get_slot_table();

    ret = osal_mutex_init(&mgmt->lock);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = osal_mutex_init(&mgmt_slot_table->lock);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    return mgmt->ops->init(mgmt);
}

hi_void klad_slot_mgmt_exit(hi_void)
{
    struct klad_slot_mgmt *mgmt = __get_klad_slot_mgmt();
    struct klad_slot_table *mgmt_slot_table = get_slot_table();

    if (mgmt->ops->exit(mgmt) != HI_SUCCESS) {
        /*
         * must release all resource.
         */
        HI_FATAL_KLAD("klad mgmt exit with error!\n");
    }
    osal_mutex_destory(&mgmt_slot_table->lock);
    osal_mutex_destory(&mgmt->lock);
}

hi_s32 klad_slot_mgmt_suspend(hi_void)
{
    struct klad_slot_mgmt *mgmt = __get_klad_slot_mgmt();

    return mgmt->ops->suspend(mgmt);
}

hi_s32 klad_slot_mgmt_resume(hi_void)
{
    struct klad_slot_mgmt *mgmt = __get_klad_slot_mgmt();

    return mgmt->ops->resume(mgmt);
}

hi_s32 klad_slot_mgmt_create_slot(hi_handle *handle)
{
    struct klad_slot_mgmt *mgmt = get_klad_slot_mgmt();

    return mgmt->ops->create_slot(mgmt, handle);
}

hi_s32 klad_slot_mgmt_destroy_slot(hi_handle handle)
{
    struct klad_slot_mgmt *mgmt = get_klad_slot_mgmt();

    return mgmt->ops->destroy_slot(mgmt, handle);
}

hi_s32 klad_slot_mgmt_com_create(hi_handle handle)
{
    struct klad_slot_mgmt *mgmt = get_klad_slot_mgmt();

    return mgmt->ops->create_com_instance(mgmt, handle);
}

hi_s32 klad_slot_mgmt_com_destroy(hi_handle handle)
{
    struct klad_slot_mgmt *mgmt = get_klad_slot_mgmt();

    return mgmt->ops->destroy_com_instance(mgmt, handle);
}

hi_s32 klad_slot_mgmt_ta_create(hi_handle handle)
{
    struct klad_slot_mgmt *mgmt = get_klad_slot_mgmt();

    return mgmt->ops->create_ta_instance(mgmt, handle);
}

hi_s32 klad_slot_mgmt_ta_destroy(hi_handle handle)
{
    struct klad_slot_mgmt *mgmt = get_klad_slot_mgmt();

    return mgmt->ops->destroy_ta_instance(mgmt, handle);
}

hi_s32 klad_slot_mgmt_fp_create(hi_handle handle)
{
    struct klad_slot_mgmt *mgmt = get_klad_slot_mgmt();

    return mgmt->ops->create_fp_instance(mgmt, handle);
}

hi_s32 klad_slot_mgmt_fp_destroy(hi_handle handle)
{
    struct klad_slot_mgmt *mgmt = get_klad_slot_mgmt();

    return mgmt->ops->destroy_fp_instance(mgmt, handle);
}

hi_s32 klad_slot_mgmt_nonce_create(hi_handle handle)
{
    struct klad_slot_mgmt *mgmt = get_klad_slot_mgmt();

    return mgmt->ops->create_nonce_instance(mgmt, handle);
}

hi_s32 klad_slot_mgmt_nonce_destroy(hi_handle handle)
{
    struct klad_slot_mgmt *mgmt = get_klad_slot_mgmt();

    return mgmt->ops->destroy_nonce_instance(mgmt, handle);
}

hi_s32 klad_slot_mgmt_clr_create(hi_handle handle)
{
    struct klad_slot_mgmt *mgmt = get_klad_slot_mgmt();

    return mgmt->ops->create_clr_instance(mgmt, handle);
}

hi_s32 klad_slot_mgmt_clr_destroy(hi_handle handle)
{
    struct klad_slot_mgmt *mgmt = get_klad_slot_mgmt();

    return mgmt->ops->destroy_clr_instance(mgmt, handle);
}

hi_s32 klad_slot_mgmt_instance_destroy(hi_handle handle)
{
    hi_s32 ret;
    hi_u32 klad_type = 0;
    struct klad_slot_mgmt *mgmt = get_klad_slot_mgmt();

    ret = __klad_slot_type(handle, &klad_type);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    print_err_hex(klad_type);
    if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_COM) {
        return mgmt->ops->destroy_com_instance(mgmt, handle);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_TA) {
        return mgmt->ops->destroy_ta_instance(mgmt, handle);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_FP) {
        return mgmt->ops->destroy_fp_instance(mgmt, handle);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_NONCE) {
        return mgmt->ops->destroy_nonce_instance(mgmt, handle);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_CLR) {
        return mgmt->ops->destroy_clr_instance(mgmt, handle);
    }
    print_err_code(HI_ERR_KLAD_INVALID_PARAM);
    return HI_ERR_KLAD_INVALID_PARAM;
}


hi_s32 klad_slot_mgmt_instance_create(hi_handle handle, hi_u32 klad_type)
{
    struct klad_slot_mgmt *mgmt = get_klad_slot_mgmt();

    if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_COM) {
        return mgmt->ops->create_com_instance(mgmt, handle);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_TA) {
        return mgmt->ops->create_ta_instance(mgmt, handle);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_FP) {
        return mgmt->ops->create_fp_instance(mgmt, handle);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_NONCE) {
        return mgmt->ops->create_nonce_instance(mgmt, handle);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_CLR) {
        return mgmt->ops->create_clr_instance(mgmt, handle);
    }
    print_err_code(HI_ERR_KLAD_INVALID_PARAM);
    return HI_ERR_KLAD_INVALID_PARAM;
}


hi_s32 klad_slot_get_klad_type(hi_handle handle, hi_u32 *klad_type)
{
    hi_s32 ret;
    struct klad_sw_r_base *base = HI_NULL_PTR;

    ret = klad_sw_r_get(handle, &base);
    if (ret == HI_SUCCESS) {
        *klad_type = base->klad_type;
    }
    return ret;
}

/*
 * API : klad software instance defination.
 */
static struct klad_com_ops g_sw_com_klad_slot_ops;

static hi_s32 __klad_sw_r_com_get(hi_handle handle, struct com_klad_slot **com_klad)
{
    hi_s32 ret;
    ret = klad_sw_r_get(handle, (struct klad_sw_r_base **)com_klad);
    if (ret == HI_SUCCESS) {
        if (&g_sw_com_klad_slot_ops != (*com_klad)->ops) {
            klad_sw_r_put((struct klad_sw_r_base *)(*com_klad));
            ret = HI_ERR_KLAD_INVALID_PARAM;
        }
    }
    return ret;
}

hi_s32 klad_slot_com_set_rootkey_attr(hi_handle handle, hi_rootkey_attr *rootkey)
{
    hi_s32 ret;
    struct com_klad_slot *com_klad = HI_NULL;

    ret = __klad_sw_r_com_get(handle, &com_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = com_klad->ops->set_rootkey_attr(com_klad, rootkey);

    klad_sw_r_put((struct klad_sw_r_base *)com_klad);
out0:
    return ret;
}

hi_s32 klad_slot_com_set_attr(hi_handle handle, hi_klad_attr *attr)
{
    hi_s32 ret;
    struct com_klad_slot *com_klad = HI_NULL;

    ret = __klad_sw_r_com_get(handle, &com_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = com_klad->ops->set_attr(com_klad, attr);

    klad_sw_r_put((struct klad_sw_r_base *)com_klad);
out0:
    return ret;
}

hi_s32 klad_slot_com_get_attr(hi_handle handle, hi_klad_attr *attr)
{
    hi_s32 ret;
    struct com_klad_slot *com_klad = HI_NULL;

    ret = __klad_sw_r_com_get(handle, &com_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = com_klad->ops->get_attr(com_klad, attr);

    klad_sw_r_put((struct klad_sw_r_base *)com_klad);
out0:
    return ret;
}

hi_s32 klad_slot_com_attach(hi_handle handle, hi_handle target)
{
    hi_s32 ret;
    struct com_klad_slot *com_klad = HI_NULL;

    ret = __klad_sw_r_com_get(handle, &com_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = com_klad->ops->attach(com_klad, target);

    klad_sw_r_put((struct klad_sw_r_base *)com_klad);
out0:
    return ret;
}

hi_s32 klad_slot_com_detach(hi_handle handle, hi_handle target)
{
    hi_s32 ret;
    struct com_klad_slot *com_klad = HI_NULL;

    ret = __klad_sw_r_com_get(handle, &com_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = com_klad->ops->detach(com_klad, target);

    klad_sw_r_put((struct klad_sw_r_base *)com_klad);
out0:
    return ret;
}

hi_s32 klad_slot_com_set_session_key(hi_handle handle, hi_klad_session_key *session_key)
{
    hi_s32 ret;
    struct com_klad_slot *com_klad = HI_NULL;

    ret = __klad_sw_r_com_get(handle, &com_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = com_klad->ops->set_session_key(com_klad, session_key);

    klad_sw_r_put((struct klad_sw_r_base *)com_klad);
out0:
    return ret;
}

hi_s32 klad_slot_com_set_content_key(hi_handle handle, hi_klad_content_key *content_key)
{
    hi_s32 ret;
    struct com_klad_slot *com_klad = HI_NULL;

    ret = __klad_sw_r_com_get(handle, &com_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = com_klad->ops->set_content_key(com_klad, content_key);

    klad_sw_r_put((struct klad_sw_r_base *)com_klad);
out0:
    return ret;
}

hi_s32 klad_slot_com_start(hi_handle handle)
{
    hi_s32 ret;
    struct com_klad_slot *com_klad = HI_NULL;

    ret = __klad_sw_r_com_get(handle, &com_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = com_klad->ops->start(com_klad);

    klad_sw_r_put((struct klad_sw_r_base *)com_klad);
out0:
    return ret;
}

static struct klad_fp_ops g_sw_fp_klad_slot_ops;

static hi_s32 __klad_sw_r_fp_get(hi_handle handle, struct fp_klad_slot **fp_klad)
{
    hi_s32 ret;
    ret = klad_sw_r_get(handle, (struct klad_sw_r_base **)fp_klad);
    if (ret == HI_SUCCESS) {
        if (&g_sw_fp_klad_slot_ops != (*fp_klad)->ops) {
            klad_sw_r_put((struct klad_sw_r_base *)(*fp_klad));
            ret = HI_ERR_KLAD_INVALID_PARAM;
        }
    }
    return ret;
}

hi_s32 klad_slot_fp_set_attr(hi_handle handle, hi_klad_attr *attr)
{
    hi_s32 ret;
    struct fp_klad_slot *fp_klad = HI_NULL;

    ret = __klad_sw_r_fp_get(handle, &fp_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = fp_klad->ops->set_attr(fp_klad, attr);

    klad_sw_r_put((struct klad_sw_r_base *)fp_klad);
out0:
    return ret;
}

hi_s32 klad_slot_fp_get_attr(hi_handle handle, hi_klad_attr *attr)
{
    hi_s32 ret;
    struct fp_klad_slot *fp_klad = HI_NULL;

    ret = __klad_sw_r_fp_get(handle, &fp_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = fp_klad->ops->get_attr(fp_klad, attr);

    klad_sw_r_put((struct klad_sw_r_base *)fp_klad);
out0:
    return ret;
}

hi_s32 klad_slot_fp_attach(hi_handle handle, hi_handle target)
{
    hi_s32 ret;
    struct fp_klad_slot *fp_klad = HI_NULL;

    ret = __klad_sw_r_fp_get(handle, &fp_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = fp_klad->ops->attach(fp_klad, target);

    klad_sw_r_put((struct klad_sw_r_base *)fp_klad);
out0:
    return ret;
}

hi_s32 klad_slot_fp_detach(hi_handle handle, hi_handle target)
{
    hi_s32 ret;
    struct fp_klad_slot *fp_klad = HI_NULL;

    ret = __klad_sw_r_fp_get(handle, &fp_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = fp_klad->ops->detach(fp_klad, target);

    klad_sw_r_put((struct klad_sw_r_base *)fp_klad);
out0:
    return ret;
}

hi_s32 klad_slot_fp_set_session_key(hi_handle handle, hi_klad_session_key *session_key)
{
    hi_s32 ret;
    struct fp_klad_slot *fp_klad = HI_NULL;

    ret = __klad_sw_r_fp_get(handle, &fp_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = fp_klad->ops->set_session_key(fp_klad, session_key);

    klad_sw_r_put((struct klad_sw_r_base *)fp_klad);
out0:
    return ret;
}

hi_s32 klad_slot_fp_set_fp_key(hi_handle handle, hi_klad_nonce_key *fp_key)
{
    hi_s32 ret;
    struct fp_klad_slot *fp_klad = HI_NULL;

    ret = __klad_sw_r_fp_get(handle, &fp_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = fp_klad->ops->set_fp_key(fp_klad, fp_key);

    klad_sw_r_put((struct klad_sw_r_base *)fp_klad);
out0:
    return ret;
}

hi_s32 klad_slot_fp_start(hi_handle handle)
{
    hi_s32 ret;
    struct fp_klad_slot *fp_klad = HI_NULL;

    ret = __klad_sw_r_fp_get(handle, &fp_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = fp_klad->ops->start(fp_klad);

    klad_sw_r_put((struct klad_sw_r_base *)fp_klad);
out0:
    return ret;
}

static struct klad_ta_ops g_sw_ta_klad_slot_ops;

static hi_s32 __klad_sw_r_ta_get(hi_handle handle, struct ta_klad_slot **ta_klad)
{
    hi_s32 ret;
    ret = klad_sw_r_get(handle, (struct klad_sw_r_base **)ta_klad);
    if (ret == HI_SUCCESS) {
        if (&g_sw_ta_klad_slot_ops != (*ta_klad)->ops) {
            klad_sw_r_put((struct klad_sw_r_base *)(*ta_klad));
            ret = HI_ERR_KLAD_INVALID_PARAM;
        }
    }
    return ret;
}

hi_s32 klad_slot_ta_set_attr(hi_handle handle, hi_klad_attr *attr)
{
    hi_s32 ret;
    struct ta_klad_slot *ta_klad = HI_NULL;

    ret = __klad_sw_r_ta_get(handle, &ta_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = ta_klad->ops->set_attr(ta_klad, attr);

    klad_sw_r_put((struct klad_sw_r_base *)ta_klad);
out0:
    return ret;
}

hi_s32 klad_slot_ta_get_attr(hi_handle handle, hi_klad_attr *attr)
{
    hi_s32 ret;
    struct ta_klad_slot *ta_klad = HI_NULL;

    ret = __klad_sw_r_ta_get(handle, &ta_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = ta_klad->ops->get_attr(ta_klad, attr);

    klad_sw_r_put((struct klad_sw_r_base *)ta_klad);
out0:
    return ret;
}

hi_s32 klad_slot_ta_attach(hi_handle handle, hi_handle target)
{
    hi_s32 ret;
    struct ta_klad_slot *ta_klad = HI_NULL;

    ret = __klad_sw_r_ta_get(handle, &ta_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = ta_klad->ops->attach(ta_klad, target);

    klad_sw_r_put((struct klad_sw_r_base *)ta_klad);
out0:
    return ret;
}

hi_s32 klad_slot_ta_detach(hi_handle handle, hi_handle target)
{
    hi_s32 ret;
    struct ta_klad_slot *ta_klad = HI_NULL;

    ret = __klad_sw_r_ta_get(handle, &ta_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = ta_klad->ops->detach(ta_klad, target);

    klad_sw_r_put((struct klad_sw_r_base *)ta_klad);
out0:
    return ret;
}

hi_s32 klad_slot_ta_set_session_key(hi_handle handle, hi_klad_ta_key *ta_key)
{
    hi_s32 ret;
    struct ta_klad_slot *ta_klad = HI_NULL;

    ret = __klad_sw_r_ta_get(handle, &ta_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = ta_klad->ops->set_session_ta_key(ta_klad, ta_key);

    klad_sw_r_put((struct klad_sw_r_base *)ta_klad);
out0:
    return ret;
}

hi_s32 klad_slot_ta_set_trans_data(hi_handle handle, hi_klad_trans_data *trans_data)
{
    hi_s32 ret;
    struct ta_klad_slot *ta_klad = HI_NULL;

    ret = __klad_sw_r_ta_get(handle, &ta_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = ta_klad->ops->set_trans_data(ta_klad, trans_data);

    klad_sw_r_put((struct klad_sw_r_base *)ta_klad);
out0:
    return ret;
}

hi_s32 klad_slot_ta_set_content_key(hi_handle handle, hi_klad_ta_key *ta_key)
{
    hi_s32 ret;
    struct ta_klad_slot *ta_klad = HI_NULL;

    ret = __klad_sw_r_ta_get(handle, &ta_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = ta_klad->ops->set_content_ta_key(ta_klad, ta_key);

    klad_sw_r_put((struct klad_sw_r_base *)ta_klad);
out0:
    return ret;
}

hi_s32 klad_slot_ta_start(hi_handle handle)
{
    hi_s32 ret;
    struct ta_klad_slot *ta_klad = HI_NULL;

    ret = __klad_sw_r_ta_get(handle, &ta_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = ta_klad->ops->start(ta_klad);

    klad_sw_r_put((struct klad_sw_r_base *)ta_klad);
out0:
    return ret;
}

static struct klad_nonce_ops g_sw_nonce_klad_slot_ops;

static hi_s32 __klad_sw_r_nonce_get(hi_handle handle, struct nonce_klad_slot **nonce_klad)
{
    hi_s32 ret;
    ret = klad_sw_r_get(handle, (struct klad_sw_r_base **)nonce_klad);
    if (ret == HI_SUCCESS) {
        if (&g_sw_nonce_klad_slot_ops != (*nonce_klad)->ops) {
            klad_sw_r_put((struct klad_sw_r_base *)(*nonce_klad));
            ret = HI_ERR_KLAD_INVALID_PARAM;
        }
    }
    return ret;
}

hi_s32 klad_slot_nonce_set_attr(hi_handle handle, hi_klad_attr *attr)
{
    hi_s32 ret;
    struct nonce_klad_slot *nonce_klad = HI_NULL;

    ret = __klad_sw_r_nonce_get(handle, &nonce_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = nonce_klad->ops->set_attr(nonce_klad, attr);

    klad_sw_r_put((struct klad_sw_r_base *)nonce_klad);
out0:
    return ret;
}

hi_s32 klad_slot_nonce_get_attr(hi_handle handle, hi_klad_attr *attr)
{
    hi_s32 ret;
    struct nonce_klad_slot *nonce_klad = HI_NULL;

    ret = __klad_sw_r_nonce_get(handle, &nonce_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = nonce_klad->ops->get_attr(nonce_klad, attr);

    klad_sw_r_put((struct klad_sw_r_base *)nonce_klad);
out0:
    return ret;
}

hi_s32 klad_slot_nonce_attach(hi_handle handle, hi_handle target)
{
    hi_s32 ret;
    struct nonce_klad_slot *nonce_klad = HI_NULL;

    ret = __klad_sw_r_nonce_get(handle, &nonce_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = nonce_klad->ops->attach(nonce_klad, target);

    klad_sw_r_put((struct klad_sw_r_base *)nonce_klad);
out0:
    return ret;
}

hi_s32 klad_slot_nonce_detach(hi_handle handle, hi_handle target)
{
    hi_s32 ret;
    struct nonce_klad_slot *nonce_klad = HI_NULL;

    ret = __klad_sw_r_nonce_get(handle, &nonce_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = nonce_klad->ops->detach(nonce_klad, target);

    klad_sw_r_put((struct klad_sw_r_base *)nonce_klad);
out0:
    return ret;
}

hi_s32 klad_slot_nonce_set_session_key(hi_handle handle, hi_klad_session_key *session_key)
{
    hi_s32 ret;
    struct nonce_klad_slot *nonce_klad = HI_NULL;

    ret = __klad_sw_r_nonce_get(handle, &nonce_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = nonce_klad->ops->set_session_key(nonce_klad, session_key);

    klad_sw_r_put((struct klad_sw_r_base *)nonce_klad);
out0:
    return ret;
}

hi_s32 klad_slot_nonce_set_nonce_key(hi_handle handle, hi_klad_nonce_key *nonce_key)
{
    hi_s32 ret;
    struct nonce_klad_slot *nonce_klad = HI_NULL;

    ret = __klad_sw_r_nonce_get(handle, &nonce_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = nonce_klad->ops->set_nonce_key(nonce_klad, nonce_key);

    klad_sw_r_put((struct klad_sw_r_base *)nonce_klad);
out0:
    return ret;
}

hi_s32 klad_slot_nonce_start(hi_handle handle)
{
    hi_s32 ret;
    struct nonce_klad_slot *nonce_klad = HI_NULL;

    ret = __klad_sw_r_nonce_get(handle, &nonce_klad);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = nonce_klad->ops->start(nonce_klad);

    klad_sw_r_put((struct klad_sw_r_base *)nonce_klad);
out0:
    return ret;
}

static struct klad_clr_route_ops g_sw_clr_route_slot_ops;

static hi_s32 __klad_sw_r_clr_get(hi_handle handle, struct clr_route_slot **clr_route)
{
    hi_s32 ret;
    ret = klad_sw_r_get(handle, (struct klad_sw_r_base **)clr_route);
    if (ret == HI_SUCCESS) {
        if (&g_sw_clr_route_slot_ops != (*clr_route)->ops) {
            klad_sw_r_put((struct klad_sw_r_base *)(*clr_route));
            ret = HI_ERR_KLAD_INVALID_PARAM;
        }
    }
    return ret;
}

hi_s32 klad_slot_clr_set_attr(hi_handle handle, hi_klad_attr *attr)
{
    hi_s32 ret;
    struct clr_route_slot *clr_route = HI_NULL;

    ret = __klad_sw_r_clr_get(handle, &clr_route);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = clr_route->ops->set_attr(clr_route, attr);

    klad_sw_r_put((struct klad_sw_r_base *)clr_route);
out0:
    return ret;
}

hi_s32 klad_slot_clr_get_attr(hi_handle handle, hi_klad_attr *attr)
{
    hi_s32 ret;
    struct clr_route_slot *clr_route = HI_NULL;

    ret = __klad_sw_r_clr_get(handle, &clr_route);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = clr_route->ops->get_attr(clr_route, attr);

    klad_sw_r_put((struct klad_sw_r_base *)clr_route);
out0:
    return ret;
}

hi_s32 klad_slot_clr_attach(hi_handle handle, hi_handle target)
{
    hi_s32 ret;
    struct clr_route_slot *clr_route = HI_NULL;

    ret = __klad_sw_r_clr_get(handle, &clr_route);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = clr_route->ops->attach(clr_route, target);

    klad_sw_r_put((struct klad_sw_r_base *)clr_route);
out0:
    return ret;
}

hi_s32 klad_slot_clr_detach(hi_handle handle, hi_handle target)
{
    hi_s32 ret;
    struct clr_route_slot *clr_route = HI_NULL;

    ret = __klad_sw_r_clr_get(handle, &clr_route);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = clr_route->ops->detach(clr_route, target);

    klad_sw_r_put((struct klad_sw_r_base *)clr_route);
out0:
    return ret;
}

hi_s32 klad_slot_clr_set_key(hi_handle handle, hi_klad_clear_key *clr_key)
{
    hi_s32 ret;
    struct clr_route_slot *clr_route = HI_NULL;

    ret = __klad_sw_r_clr_get(handle, &clr_route);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = clr_route->ops->set_clr_key(clr_route, clr_key);

    klad_sw_r_put((struct klad_sw_r_base *)clr_route);
out0:
    return ret;
}

hi_s32 klad_slot_clr_start(hi_handle handle)
{
    hi_s32 ret;
    struct clr_route_slot *clr_route = HI_NULL;

    ret = __klad_sw_r_clr_get(handle, &clr_route);
    if (ret != HI_SUCCESS) {
        goto out0;
    }

    ret = clr_route->ops->start(clr_route);

    klad_sw_r_put((struct klad_sw_r_base *)clr_route);
out0:
    return ret;
}

hi_s32 klad_slot_instance_set_attr(hi_handle handle, hi_klad_attr *attr)
{
    hi_u8 klad = HI_KLAD_TYPE(attr->klad_cfg.klad_type);

    if (klad == HI_KLAD_COM) {
        return klad_slot_com_set_attr(handle, attr);
    } else if (klad == HI_KLAD_TA) {
        return klad_slot_ta_set_attr(handle, attr);
    } else if (klad == HI_KLAD_FP) {
        return klad_slot_fp_set_attr(handle, attr);
    } else if (klad == HI_KLAD_NONCE) {
        return klad_slot_nonce_set_attr(handle, attr);
    } else if (klad == HI_KLAD_CLR) {
        return klad_slot_clr_set_attr(handle, attr);
    }
    print_err_code(HI_ERR_KLAD_INVALID_PARAM);
    return HI_ERR_KLAD_INVALID_PARAM;
}

hi_s32 klad_slot_instance_get_attr(hi_handle handle, hi_klad_attr *attr)
{
    hi_s32 ret;
    hi_u32 klad_type = 0;

    ret = __klad_slot_type(handle, &klad_type);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_COM) {
        return klad_slot_com_get_attr(handle, attr);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_TA) {
        return klad_slot_ta_get_attr(handle, attr);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_FP) {
        return klad_slot_fp_get_attr(handle, attr);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_NONCE) {
        return klad_slot_nonce_get_attr(handle, attr);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_CLR) {
        return klad_slot_clr_get_attr(handle, attr);
    }
    print_err_code(HI_ERR_KLAD_INVALID_PARAM);
    return HI_ERR_KLAD_INVALID_PARAM;
}

hi_s32 klad_slot_instance_attach(hi_handle handle, hi_handle target)
{
    hi_s32 ret;
    hi_u32 klad_type = 0;

    ret = __klad_slot_type(handle, &klad_type);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    print_err_hex(klad_type);
    if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_COM) {
        return klad_slot_com_attach(handle, target);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_TA) {
        return klad_slot_ta_attach(handle, target);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_FP) {
        return klad_slot_fp_attach(handle, target);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_NONCE) {
        return klad_slot_nonce_attach(handle, target);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_CLR) {
        return klad_slot_clr_attach(handle, target);
    }
    print_err_code(HI_ERR_KLAD_INVALID_PARAM);
    return HI_ERR_KLAD_INVALID_PARAM;
}

hi_s32 klad_slot_instance_detach(hi_handle handle, hi_handle target)
{
    hi_s32 ret;
    hi_u32 klad_type = 0;

    ret = __klad_slot_type(handle, &klad_type);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_COM) {
        return klad_slot_com_detach(handle, target);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_TA) {
        return klad_slot_ta_detach(handle, target);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_FP) {
        return klad_slot_fp_detach(handle, target);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_NONCE) {
        return klad_slot_nonce_detach(handle, target);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_CLR) {
        return klad_slot_clr_detach(handle, target);
    }
    print_err_code(HI_ERR_KLAD_INVALID_PARAM);
    return HI_ERR_KLAD_INVALID_PARAM;
}

hi_s32 klad_slot_instance_set_session_key(hi_handle handle, hi_klad_session_key *session_key)
{
    hi_s32 ret;
    hi_u32 klad_type = 0;

    ret = __klad_slot_type(handle, &klad_type);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_COM) {
        return klad_slot_com_set_session_key(handle, session_key);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_FP) {
        return klad_slot_fp_set_session_key(handle, session_key);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_NONCE) {
        return klad_slot_nonce_set_session_key(handle, session_key);
    }
    print_err_code(HI_ERR_KLAD_INVALID_PARAM);
    return HI_ERR_KLAD_INVALID_PARAM;
}

hi_s32 klad_slot_instance_set_content_key(hi_handle handle, hi_klad_content_key *content_key)
{
    hi_s32 ret;
    hi_u32 klad_type = 0;

    ret = __klad_slot_type(handle, &klad_type);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (HI_KLAD_TYPE(klad_type) == HI_KLAD_COM) {
        return klad_slot_com_set_content_key(handle, content_key);
    } else if (HI_KLAD_TYPE(klad_type) == HI_KLAD_CLR) {
        hi_klad_clear_key clear_key = {0};
        clear_key.key_size = content_key->key_size;
        clear_key.odd = content_key->odd;
        if (memcpy_s(clear_key.key, HI_KLAD_MAX_KEY_LEN, content_key->key, HI_KLAD_MAX_KEY_LEN) != EOK) {
            return HI_FAILURE;
        }
        return klad_slot_clr_set_key(handle, &clear_key);
    } else {
        print_err_code(HI_ERR_KLAD_INVALID_PARAM);
        return HI_ERR_KLAD_INVALID_PARAM;
    }
}

hi_s32 klad_slot_instance_start(hi_handle handle)
{
    hi_s32 ret;
    hi_u32 klad_type = 0;

    ret = __klad_slot_type(handle, &klad_type);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_COM) {
        return klad_slot_com_start(handle);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_TA) {
        return klad_slot_ta_start(handle);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_FP) {
        return klad_slot_fp_start(handle);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_NONCE) {
        return klad_slot_nonce_start(handle);
    } else if ((HI_KLAD_TYPE(klad_type)) == HI_KLAD_CLR) {
        return klad_slot_clr_start(handle);
    }
    print_err_code(HI_ERR_KLAD_INVALID_PARAM);
    return HI_ERR_KLAD_INVALID_PARAM;
}

/*
 *  ==================keyaldder soft instance defination.=========================
 * 1: define the methods of all kinds of Keyladder.
 * 2: define the create method and destroy method.
 * 3: In order to reduce memory fragmentation, all the alloced memory block add to the used list(mgmt->xxx_list)
 *    and remove to the free list(mgmt->xxx_empty_list) if destroy the instance.
 */
static hi_s32 __klad_slot_attach_impl(hi_handle *dst_handle, atomic64_t *cnt_ref,  hi_handle src_handle)
{
    if (*dst_handle == HI_INVALID_HANDLE || *dst_handle == 0) {
        *dst_handle = src_handle;
    } else {
        return HI_FAILURE;
    }
    atomic64_inc(cnt_ref);
    return HI_SUCCESS;
}

static hi_s32 __klad_slot_detach_impl(hi_handle *dst_handle,  hi_handle src_handle)
{
    if (*dst_handle == src_handle) {
        *dst_handle = HI_INVALID_HANDLE;
    } else {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 klad_slot_com_set_rk_attr_impl(struct com_klad_slot *instance, hi_rootkey_attr *attr)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);
    ret = memcpy_s(&instance->rk_attr, sizeof(hi_rootkey_attr), attr, sizeof(hi_rootkey_attr));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->rk_attr_cnt);
out:
    __mutex_unlock(&instance->lock);
    return ret;
}

static hi_s32 klad_slot_com_set_attr_impl(struct com_klad_slot *instance, hi_klad_attr *attr)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    instance->base.klad_type = attr->klad_cfg.klad_type;
    print_err_hex(instance->base.klad_type);
    ret = memcpy_s(&instance->attr, sizeof(hi_klad_attr), attr, sizeof(hi_klad_attr));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->attr_cnt);

    ret = HI_SUCCESS;

out:
    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_com_get_attr_impl(struct com_klad_slot *instance, hi_klad_attr *attr)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = memcpy_s(attr, sizeof(hi_klad_attr), &instance->attr, sizeof(hi_klad_attr));
    if (ret != EOK) {
        goto out;
    }
    ret = HI_SUCCESS;

out:
    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_com_attach_impl(struct com_klad_slot *instance, hi_handle target)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = __klad_slot_attach_impl(&instance->target_handle, &instance->target_cnt, target);

    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_com_detach_impl(struct com_klad_slot *instance, hi_handle target)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = __klad_slot_detach_impl(&instance->target_handle, target);

    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_com_set_session_key_impl(struct com_klad_slot *instance, hi_klad_session_key *session_key)
{
    hi_s32 ret;
    hi_klad_level level = session_key->level;

    if (level >= HI_KLAD_LEVEL_MAX) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }

    __mutex_lock(&instance->lock);
    ret = memcpy_s(&instance->session_key[level], sizeof(hi_klad_session_key),
                   session_key, sizeof(hi_klad_session_key));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->session_cnt[level]);
out:
    __mutex_unlock(&instance->lock);
    return ret;
}

static hi_s32 klad_slot_com_set_content_key_impl(struct com_klad_slot *instance, hi_klad_content_key *content_key)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);
    ret = memcpy_s(&instance->content_key, sizeof(hi_klad_content_key),
                   content_key, sizeof(hi_klad_content_key));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->content_cnt);
out:
    __mutex_unlock(&instance->lock);
    return ret;
}

static hi_s32 klad_slot_com_start_impl(struct com_klad_slot *instance)
{
    hi_s32 ret;
    struct klad_entry_key attrs = {0};
    hi_klad_com_entry entry = {0};

    __mutex_lock(&instance->lock);

    ret = hi_drv_hw_com_klad_process(&attrs, &entry);
    if (ret != HI_SUCCESS) {
        HI_FATAL_KLAD("start hw clear route failed.\n");
    }
    __mutex_unlock(&instance->lock);
    return ret;
}

/*
 * com keyaldder soft instance defination.
 */
static struct klad_com_ops g_sw_com_klad_slot_ops = {
    .set_rootkey_attr          = klad_slot_com_set_rk_attr_impl,
    .set_attr                  = klad_slot_com_set_attr_impl,
    .get_attr                  = klad_slot_com_get_attr_impl,
    .attach                    = klad_slot_com_attach_impl,
    .detach                    = klad_slot_com_detach_impl,
    .set_session_key           = klad_slot_com_set_session_key_impl,
    .set_content_key           = klad_slot_com_set_content_key_impl,
    .start                     = klad_slot_com_start_impl,
};


hi_s32 klad_slot_fp_set_attr_impl(struct fp_klad_slot *instance, hi_klad_attr *attr)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    instance->base.klad_type = attr->klad_cfg.klad_type;
    ret = memcpy_s(&instance->attr, sizeof(hi_klad_attr), attr, sizeof(hi_klad_attr));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->attr_cnt);

    ret = HI_SUCCESS;

out:
    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_fp_get_attr_impl(struct fp_klad_slot *instance, hi_klad_attr *attr)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = memcpy_s(attr, sizeof(hi_klad_attr), &instance->attr, sizeof(hi_klad_attr));
    if (ret != EOK) {
        goto out;
    }
    ret = HI_SUCCESS;

out:
    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_fp_attach_impl(struct fp_klad_slot *instance, hi_handle target)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = __klad_slot_attach_impl(&instance->target_handle, &instance->target_cnt, target);

    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_fp_detach_impl(struct fp_klad_slot *instance, hi_handle target)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = __klad_slot_detach_impl(&instance->target_handle, target);

    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_fp_set_session_key_impl(struct fp_klad_slot *instance, hi_klad_session_key *session_key)
{
    hi_s32 ret;
    hi_klad_level level = session_key->level;

    if (level >= HI_KLAD_LEVEL_MAX) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }

    __mutex_lock(&instance->lock);
    ret = memcpy_s(&instance->session_key[level], sizeof(hi_klad_session_key),
                   session_key, sizeof(hi_klad_session_key));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->session_cnt[level]);
out:
    __mutex_unlock(&instance->lock);
    return ret;
}

static hi_s32 klad_slot_fp_set_fp_key_impl(struct fp_klad_slot *instance, hi_klad_nonce_key *fp_key)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);
    ret = memcpy_s(&instance->fp_key, sizeof(hi_klad_fp_key), fp_key, sizeof(hi_klad_fp_key));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->fp_cnt);
out:
    __mutex_unlock(&instance->lock);
    return ret;
}

static hi_s32 klad_slot_fp_start_impl(struct fp_klad_slot *instance)
{
    hi_s32 ret;
    struct klad_entry_key attrs = {0};
    hi_klad_fp_entry entry = {0};

    __mutex_lock(&instance->lock);

    ret = hi_drv_hw_fp_klad_process(&attrs, &entry);
    if (ret != HI_SUCCESS) {
        HI_FATAL_KLAD("start hw clear route failed.\n");
    }

    __mutex_unlock(&instance->lock);
    return ret;
}

/*
 * flash protection keyaldder soft instance defination.
 */
static struct klad_fp_ops g_sw_fp_klad_slot_ops = {
    .set_attr                  = klad_slot_fp_set_attr_impl,
    .get_attr                  = klad_slot_fp_get_attr_impl,
    .attach                    = klad_slot_fp_attach_impl,
    .detach                    = klad_slot_fp_detach_impl,
    .set_session_key           = klad_slot_fp_set_session_key_impl,
    .set_fp_key                = klad_slot_fp_set_fp_key_impl,
    .start                     = klad_slot_fp_start_impl,
};


hi_s32 klad_slot_ta_set_attr_impl(struct ta_klad_slot *instance, hi_klad_attr *attr)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    instance->base.klad_type = attr->klad_cfg.klad_type;
    ret = memcpy_s(&instance->attr, sizeof(hi_klad_attr), attr, sizeof(hi_klad_attr));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->attr_cnt);

    ret = HI_SUCCESS;

out:
    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_ta_get_attr_impl(struct ta_klad_slot *instance, hi_klad_attr *attr)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = memcpy_s(attr, sizeof(hi_klad_attr), &instance->attr, sizeof(hi_klad_attr));
    if (ret != EOK) {
        goto out;
    }
    ret = HI_SUCCESS;

out:
    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_ta_attach_impl(struct ta_klad_slot *instance, hi_handle target)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = __klad_slot_attach_impl(&instance->target_handle, &instance->target_cnt, target);

    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_ta_detach_impl(struct ta_klad_slot *instance, hi_handle target)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = __klad_slot_detach_impl(&instance->target_handle, target);

    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_ta_set_session_key_impl(struct ta_klad_slot *instance, hi_klad_ta_key *ta_key)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);
    ret = memcpy_s(&instance->session_ta_key, sizeof(hi_klad_ta_key), ta_key, sizeof(hi_klad_ta_key));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->session_ta_cnt);
out:
    __mutex_unlock(&instance->lock);
    return ret;
}

static hi_s32 klad_slot_ta_set_trans_data_impl(struct ta_klad_slot *instance, hi_klad_trans_data *trans_data)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);
    ret = memcpy_s(&instance->trans_data, sizeof(hi_klad_trans_data),
                   trans_data, sizeof(hi_klad_trans_data));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->trans_cnt);
out:
    __mutex_unlock(&instance->lock);
    return ret;
}

static hi_s32 klad_slot_ta_set_content_key_impl(struct ta_klad_slot *instance, hi_klad_ta_key *ta_key)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);
    ret = memcpy_s(&instance->content_ta_key, sizeof(hi_klad_ta_key), ta_key, sizeof(hi_klad_ta_key));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->content_ta_cnt);
out:
    __mutex_unlock(&instance->lock);
    return ret;
}

static hi_s32 klad_slot_ta_start_impl(struct ta_klad_slot *instance)
{
    hi_s32 ret;
    struct klad_entry_key attrs = {0};
    hi_klad_ta_entry entry = {0};

    __mutex_lock(&instance->lock);

    ret = hi_drv_hw_ta_klad_process(&attrs, &entry);
    if (ret != HI_SUCCESS) {
        HI_FATAL_KLAD("start hw clear route failed.\n");
    }

    __mutex_unlock(&instance->lock);
    return ret;
}

/*
 * ta keyaldder soft instance defination.
 */
static struct klad_ta_ops g_sw_ta_klad_slot_ops = {
    .set_attr                  = klad_slot_ta_set_attr_impl,
    .get_attr                  = klad_slot_ta_get_attr_impl,
    .attach                    = klad_slot_ta_attach_impl,
    .detach                    = klad_slot_ta_detach_impl,
    .set_session_ta_key        = klad_slot_ta_set_session_key_impl,
    .set_trans_data            = klad_slot_ta_set_trans_data_impl,
    .set_content_ta_key        = klad_slot_ta_set_content_key_impl,
    .start                     = klad_slot_ta_start_impl,
};

static hi_s32 klad_slot_nonce_set_attr_impl(struct nonce_klad_slot *instance, hi_klad_attr *attr)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    instance->base.klad_type = attr->klad_cfg.klad_type;

    ret = memcpy_s(&instance->attr, sizeof(hi_klad_attr), attr, sizeof(hi_klad_attr));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->attr_cnt);

    ret = HI_SUCCESS;

out:
    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_nonce_get_attr_impl(struct nonce_klad_slot *instance, hi_klad_attr *attr)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = memcpy_s(attr, sizeof(hi_klad_attr), &instance->attr, sizeof(hi_klad_attr));
    if (ret != EOK) {
        goto out;
    }
    ret = HI_SUCCESS;

out:
    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_nonce_attach_impl(struct nonce_klad_slot *instance, hi_handle target)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = __klad_slot_attach_impl(&instance->target_handle, &instance->target_cnt, target);

    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_nonce_detach_impl(struct nonce_klad_slot *instance, hi_handle target)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = __klad_slot_detach_impl(&instance->target_handle, target);

    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_nonce_set_session_key_impl(struct nonce_klad_slot *instance, hi_klad_session_key *session_key)
{
    hi_s32 ret;
    hi_klad_level level = session_key->level;

    if (level >= HI_KLAD_LEVEL_MAX) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }

    __mutex_lock(&instance->lock);
    ret = memcpy_s(&instance->session_key[level], sizeof(hi_klad_session_key),
                   session_key, sizeof(hi_klad_session_key));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->session_cnt[level]);
out:
    __mutex_unlock(&instance->lock);
    return ret;
}

static hi_s32 klad_slot_nonce_set_key_impl(struct nonce_klad_slot *instance, hi_klad_nonce_key *nonce_key)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);
    ret = memcpy_s(&instance->nonce_key, sizeof(hi_klad_nonce_key), nonce_key, sizeof(hi_klad_nonce_key));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->nonce_cnt);
out:
    __mutex_unlock(&instance->lock);
    return ret;
}

static hi_s32 klad_slot_nonce_start_impl(struct nonce_klad_slot *instance)
{
    hi_s32 ret;
    struct klad_entry_key attrs = {0};
    hi_klad_nonce_entry entry = {0};

    __mutex_lock(&instance->lock);

    ret = hi_drv_hw_nonce_klad_process(&attrs, &entry);
    if (ret != HI_SUCCESS) {
        HI_FATAL_KLAD("start hw clear route failed.\n");
    }

    __mutex_unlock(&instance->lock);
    return ret;
}
/*
 * nonce keyaldder soft instance defination.
 */
static struct klad_nonce_ops g_sw_nonce_klad_slot_ops = {
    .set_attr                  = klad_slot_nonce_set_attr_impl,
    .get_attr                  = klad_slot_nonce_get_attr_impl,
    .attach                    = klad_slot_nonce_attach_impl,
    .detach                    = klad_slot_nonce_detach_impl,
    .set_session_key           = klad_slot_nonce_set_session_key_impl,
    .set_nonce_key             = klad_slot_nonce_set_key_impl,
    .start                     = klad_slot_nonce_start_impl,
};

hi_s32 klad_slot_clr_route_set_attr_impl(struct clr_route_slot *instance, hi_klad_attr *attr)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    instance->base.klad_type = attr->klad_cfg.klad_type;

    ret = memcpy_s(&instance->attr, sizeof(hi_klad_attr), attr, sizeof(hi_klad_attr));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->attr_cnt);

    ret = HI_SUCCESS;

out:
    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_clr_route_get_attr_impl(struct clr_route_slot *instance, hi_klad_attr *attr)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = memcpy_s(attr, sizeof(hi_klad_attr), &instance->attr, sizeof(hi_klad_attr));
    if (ret != EOK) {
        goto out;
    }
    ret = HI_SUCCESS;

out:
    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_clr_route_attach_impl(struct clr_route_slot *instance, hi_handle target)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = __klad_slot_attach_impl(&instance->target_handle, &instance->target_cnt, target);

    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_clr_route_detach_impl(struct clr_route_slot *instance, hi_handle target)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);

    ret = __klad_slot_detach_impl(&instance->target_handle, target);

    __mutex_unlock(&instance->lock);

    return ret;
}

static hi_s32 klad_slot_clr_route_set_clr_key_impl(struct clr_route_slot *instance, hi_klad_clear_key *clr_key)
{
    hi_s32 ret;

    __mutex_lock(&instance->lock);
    ret = memcpy_s(&instance->clr_key, sizeof(hi_klad_clear_key), clr_key, sizeof(hi_klad_clear_key));
    if (ret != EOK) {
        goto out;
    }
    atomic64_inc(&instance->clr_cnt);
out:
    __mutex_unlock(&instance->lock);
    return ret;
}

static hi_s32 klad_slot_clr_route_start_impl(struct clr_route_slot *instance)
{
    hi_s32 ret;
    struct klad_entry_key attrs = {0};
    hi_klad_clr_entry entry = {0};

    __mutex_lock(&instance->lock);

    ret = hi_drv_hw_clr_route_process(&attrs, &entry);
    if (ret != HI_SUCCESS) {
        HI_FATAL_KLAD("start hw clear route failed.\n");
    }
    __mutex_unlock(&instance->lock);
    return ret;
}

/*
 * clear route keyaldder soft instance defination.
 */
static struct klad_clr_route_ops g_sw_clr_route_slot_ops = {
    .set_attr                  = klad_slot_clr_route_set_attr_impl,
    .get_attr                  = klad_slot_clr_route_get_attr_impl,
    .attach                    = klad_slot_clr_route_attach_impl,
    .detach                    = klad_slot_clr_route_detach_impl,
    .set_clr_key               = klad_slot_clr_route_set_clr_key_impl,
    .start                     = klad_slot_clr_route_start_impl,
};

/*
 *  ==================keyaldder software slot management defination.=========================
 * 1: define the methods of klad_slot management.
 *    create keyladder slot and destroy keyladder slot.
 */
static hi_s32 __klad_slot_mgmt_init_impl(struct klad_slot_mgmt *mgmt)
{
    WARN_ON(0 != atomic_read(&mgmt->ref_count));
    WARN_ON(mgmt->state != KLAD_SLOT_MGMT_CLOSED);

    if (osal_mutex_init(&mgmt->slot_lock) != HI_SUCCESS) {
        return HI_FAILURE;
    }
    INIT_LIST_HEAD(&mgmt->slot_head);
    INIT_LIST_HEAD(&mgmt->slot_empty_head);

    mgmt->state = KLAD_SLOT_MGMT_OPENED;

    return HI_SUCCESS;
}

static hi_s32 klad_slot_mgmt_init_impl(struct klad_slot_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    __mutex_lock(&mgmt->lock);

    if (mgmt->state == KLAD_SLOT_MGMT_CLOSED) {
        ret = __klad_slot_mgmt_init_impl(mgmt);
        if (ret == HI_SUCCESS) {
            atomic_inc(&mgmt->ref_count);
        }
    } else if (mgmt->state == KLAD_SLOT_MGMT_OPENED) {
        WARN_ON(atomic_read(&mgmt->ref_count) == 0);

        atomic_inc(&mgmt->ref_count);

        ret = HI_SUCCESS;
    } else {
        HI_FATAL_KLAD("mgmt state unknown, mgmt state is:%u\n", mgmt->state);
    }

    __mutex_unlock(&mgmt->lock);

    return ret;
}

static hi_s32 __klad_slot_mgmt_exit_impl(struct klad_slot_mgmt *mgmt)
{
    osal_mutex_destory(&mgmt->slot_lock);
    return HI_SUCCESS;
}

static hi_s32 _klad_slot_mgmt_exit_impl(struct klad_slot_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    if (mgmt->state == KLAD_SLOT_MGMT_OPENED) {
        WARN_ON(atomic_read(&mgmt->ref_count) == 0);
        if (atomic_read(&mgmt->ref_count) == 1) {
            ret = __klad_slot_mgmt_exit_impl(mgmt);
            if (ret == HI_SUCCESS) {
                atomic_dec(&mgmt->ref_count);
            }
        } else {
            atomic_dec(&mgmt->ref_count);
            ret = HI_SUCCESS;
        }
    } else if (mgmt->state == KLAD_SLOT_MGMT_CLOSED) {
        WARN_ON(atomic_read(&mgmt->ref_count));
        ret = HI_SUCCESS;
    } else {
        HI_FATAL_KLAD("mgmt state unknown, mgmt state is:%u\n", mgmt->state);
    }
    return HI_SUCCESS;
}

#define TEN_MSECS 10

static hi_s32 klad_slot_mgmt_exit_impl(struct klad_slot_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;
    unsigned long start = osal_get_jiffies();
    unsigned long end = start + HZ; /* 1s */

    do {
        __mutex_lock(&mgmt->lock);

        ret = _klad_slot_mgmt_exit_impl(mgmt);

        __mutex_unlock(&mgmt->lock);

        osal_msleep_uninterruptible(TEN_MSECS);

    } while (ret != HI_SUCCESS && time_in_range((unsigned long)osal_get_jiffies(), start, end));

    return ret;
}

static hi_s32 __klad_slot_mgmt_suspend_impl(struct klad_slot_mgmt *mgmt)
{
    return HI_SUCCESS;
}

/*
 * Success must be returned regardless of whether MGMT is initialized.
 * otherwise it will affect the entire standby process.
 */
static hi_s32 klad_slot_mgmt_suspend_impl(struct klad_slot_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    __mutex_lock(&mgmt->lock);

    if (mgmt->state != KLAD_SLOT_MGMT_OPENED) {
        ret = HI_ERR_KLAD_NOT_INIT;
    } else {
        atomic_inc(&mgmt->ref_count);
        ret = HI_SUCCESS;
    }

    __mutex_unlock(&mgmt->lock);

    if (ret == HI_SUCCESS) {
        __klad_slot_mgmt_suspend_impl(mgmt);

        atomic_dec(&mgmt->ref_count);
    }

    HI_PRINT("KLAD suspend ok\n");

    return HI_SUCCESS;
}

static hi_s32 __klad_slot_mgmt_resume_impl(struct klad_slot_mgmt *mgmt)
{
    return HI_SUCCESS;
}

/*
 * Success must be returned regardless of whether MGMT is initialized.
 * otherwise it will affect the entire standby process.
 */
static hi_s32 klad_slot_mgmt_resume_impl(struct klad_slot_mgmt *mgmt)
{
    hi_s32 ret = HI_FAILURE;

    __mutex_lock(&mgmt->lock);

    if (mgmt->state != KLAD_SLOT_MGMT_OPENED) {
        ret = HI_ERR_KLAD_NOT_INIT;
    } else {
        atomic_inc(&mgmt->ref_count);
        ret = HI_SUCCESS;
    }

    __mutex_unlock(&mgmt->lock);

    if (ret == HI_SUCCESS) {
        __klad_slot_mgmt_resume_impl(mgmt);

        atomic_dec(&mgmt->ref_count);
    }

    HI_PRINT("KLAD resume ok\n");

    return HI_SUCCESS;
}

static hi_s32 klad_slot_mgmt_create_slot_impl(struct klad_slot_mgmt *mgmt, hi_handle *handle)
{
    hi_s32 ret;
    struct klad_slot *new_slot = HI_NULL;

    __mutex_lock(&mgmt->lock);

    if (mgmt->state != KLAD_SLOT_MGMT_OPENED) {
        HI_ERR_KLAD("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = klad_slot_create(&new_slot);
    if (ret == HI_SUCCESS) {
        HI_INFO_KLAD("klad slot create succefull.\n");
        *handle = new_slot->handle;
    }
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 klad_slot_mgmt_destroy_slot_impl(struct klad_slot_mgmt *mgmt, hi_handle handle)
{
    hi_s32 ret;
    struct klad_slot *slot = HI_NULL;

    __mutex_lock(&mgmt->lock);

    if (mgmt->state != KLAD_SLOT_MGMT_OPENED) {
        HI_ERR_KLAD("mgmt has not opened.\n");
        ret = HI_FAILURE;
        goto out;
    }

    ret = klad_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = klad_slot_destroy(slot);
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 __klad_slot_mgmt_instance_create_impl(struct klad_slot_mgmt *mgmt, struct klad_slot_instance **slot_inst)
{
    hi_s32 ret = HI_SUCCESS;
    struct klad_slot_instance *entry = HI_NULL_PTR;
    struct list_head *node = HI_NULL_PTR;

    __mutex_lock(&mgmt->slot_lock);

    if (!list_empty(&mgmt->slot_empty_head)) {
        entry = (struct klad_slot_instance *)list_first_entry(&mgmt->slot_empty_head, struct klad_sw_r_base, node);
        ret = memset_s(entry, sizeof(struct klad_slot_instance), 0, sizeof(struct klad_slot_instance));
        if (ret != EOK) {
            goto out;
        }
        list_del(&mgmt->slot_empty_head);
    } else {
        entry = osal_kmalloc(HI_ID_KLAD, sizeof(struct klad_slot_instance), OSAL_GFP_KERNEL);
        if (entry == HI_NULL) {
            ret = HI_ERR_KLAD_NO_MEMORY;
            goto out;
        }

        ret = memset_s(entry, sizeof(struct klad_slot_instance), 0, sizeof(struct klad_slot_instance));
        if (ret != EOK) {
            HI_ERR_KLAD("Call memset_s failed.\n");
            ret = HI_FAILURE;
            goto out;
        }
    }
    node = &((struct klad_sw_r_base *)entry)->node;
    INIT_LIST_HEAD(node);
    list_add_tail(node, &mgmt->slot_head);
    *slot_inst = entry;
out:
    __mutex_unlock(&mgmt->slot_lock);

    return ret;
}

static hi_void _clr_instance_mgmt_init_impl(struct klad_slot_mgmt *mgmt, struct clr_route_slot *instance)
{
    atomic_set(&instance->base.ref_count, 1);
    instance->base.ops     = get_klad_sw_r_base_ops();
    instance->base.release = HI_NULL;
    instance->base.mgmt    = mgmt;
    instance->base.klad_type = HI_KLAD_CLR;
    instance->base.free_list = &mgmt->slot_empty_head;

    osal_mutex_init(&instance->lock);
    instance->ops = &g_sw_clr_route_slot_ops;

    atomic64_set(&instance->target_cnt, 0);
    atomic64_set(&instance->attr_cnt, 0);
    atomic64_set(&instance->clr_cnt, 0);

    return;
}

static hi_s32 clr_instance_mgmt_create_impl(struct klad_slot_mgmt *mgmt, hi_handle handle)
{
    hi_s32 ret;
    struct klad_slot_instance *instance  = HI_NULL;

    __mutex_lock(&mgmt->lock);

    ret = __klad_slot_mgmt_instance_create_impl(mgmt, &instance);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    instance->klad = HI_KLAD_CLR;
    _clr_instance_mgmt_init_impl(mgmt, &instance->obj.clr_slot);

    ret = klad_slot_bind(handle, &instance->obj.clr_slot.base);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    ret = HI_SUCCESS;
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_void _nonce_instance_mgmt_init_impl(struct klad_slot_mgmt *mgmt, struct nonce_klad_slot *instance)
{
    hi_s32 i;

    atomic_set(&instance->base.ref_count, 1);
    instance->base.ops     = get_klad_sw_r_base_ops();
    instance->base.release = HI_NULL;
    instance->base.mgmt    = mgmt;
    instance->base.klad_type = HI_KLAD_NONCE;
    instance->base.free_list = &mgmt->slot_empty_head;

    osal_mutex_init(&instance->lock);
    instance->ops = &g_sw_nonce_klad_slot_ops;

    atomic64_set(&instance->target_cnt, 0);
    atomic64_set(&instance->attr_cnt, 0);
    for (i = 0; i < HI_KLAD_LEVEL_MAX; i++) {
        atomic64_set(&instance->session_cnt[i], 0);
    }

    atomic64_set(&instance->nonce_cnt, 0);

    return;
}

static hi_s32 nonce_instance_mgmt_create_impl(struct klad_slot_mgmt *mgmt, hi_handle handle)
{
    hi_s32 ret;
    struct klad_slot_instance *instance  = HI_NULL;

    __mutex_lock(&mgmt->lock);

    ret = __klad_slot_mgmt_instance_create_impl(mgmt, &instance);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    instance->klad = HI_KLAD_NONCE;
    _nonce_instance_mgmt_init_impl(mgmt, &instance->obj.nonce_slot);

    ret = klad_slot_bind(handle, &instance->obj.nonce_slot.base);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    ret = HI_SUCCESS;
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_void _ta_instance_mgmt_init_impl(struct klad_slot_mgmt *mgmt, struct ta_klad_slot *instance)
{
    atomic_set(&instance->base.ref_count, 1);
    instance->base.ops     = get_klad_sw_r_base_ops();
    instance->base.release = HI_NULL;
    instance->base.mgmt    = mgmt;
    instance->base.klad_type = HI_KLAD_TA;
    instance->base.free_list = &mgmt->slot_empty_head;

    osal_mutex_init(&instance->lock);
    instance->ops = &g_sw_ta_klad_slot_ops;

    atomic64_set(&instance->target_cnt, 0);
    atomic64_set(&instance->attr_cnt, 0);
    atomic64_set(&instance->session_ta_cnt, 0);
    atomic64_set(&instance->trans_cnt, 0);
    atomic64_set(&instance->trans_cnt, 0);

    return;
}

static hi_s32 ta_instance_mgmt_create_impl(struct klad_slot_mgmt *mgmt, hi_handle handle)
{
    hi_s32 ret;
    struct klad_slot_instance *instance  = HI_NULL;

    __mutex_lock(&mgmt->lock);

    ret = __klad_slot_mgmt_instance_create_impl(mgmt, &instance);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    instance->klad = HI_KLAD_TA;
    _ta_instance_mgmt_init_impl(mgmt, &instance->obj.ta_slot);

    ret = klad_slot_bind(handle, &instance->obj.ta_slot.base);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    ret = HI_SUCCESS;
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_void _fp_instance_mgmt_init_impl(struct klad_slot_mgmt *mgmt, struct fp_klad_slot *instance)
{
    hi_s32 i;

    atomic_set(&instance->base.ref_count, 1);
    instance->base.ops     = get_klad_sw_r_base_ops();
    instance->base.release = HI_NULL;
    instance->base.mgmt    = mgmt;
    instance->base.klad_type = HI_KLAD_FP;
    instance->base.free_list = &mgmt->slot_empty_head;

    osal_mutex_init(&instance->lock);
    instance->ops = &g_sw_fp_klad_slot_ops;

    atomic64_set(&instance->target_cnt, 0);
    atomic64_set(&instance->attr_cnt, 0);
    for (i = 0; i < HI_KLAD_LEVEL_MAX; i++) {
        atomic64_set(&instance->session_cnt[i], 0);
    }

    atomic64_set(&instance->fp_cnt, 0);

    return;
}

static hi_s32 fp_instance_mgmt_create_impl(struct klad_slot_mgmt *mgmt, hi_handle handle)
{
    hi_s32 ret;
    struct klad_slot_instance *instance  = HI_NULL;

    __mutex_lock(&mgmt->lock);

    ret = __klad_slot_mgmt_instance_create_impl(mgmt, &instance);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    instance->klad = HI_KLAD_FP;
    _fp_instance_mgmt_init_impl(mgmt, &instance->obj.fp_slot);

    ret = klad_slot_bind(handle, &instance->obj.fp_slot.base);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    ret = HI_SUCCESS;
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_void _com_instance_mgmt_init_impl(struct klad_slot_mgmt *mgmt, struct com_klad_slot *instance)
{
    hi_s32 i;

    atomic_set(&instance->base.ref_count, 1);
    instance->base.ops     = get_klad_sw_r_base_ops();
    instance->base.release = HI_NULL;
    instance->base.mgmt    = mgmt;
    instance->base.klad_type = HI_KLAD_COM;
    instance->base.free_list = &mgmt->slot_empty_head;

    osal_mutex_init(&instance->lock);
    instance->ops = &g_sw_com_klad_slot_ops;

    atomic64_set(&instance->target_cnt, 0);
    atomic64_set(&instance->rk_attr_cnt, 0);
    atomic64_set(&instance->attr_cnt, 0);
    for (i = 0; i < HI_KLAD_LEVEL_MAX; i++) {
        atomic64_set(&instance->session_cnt[i], 0);
    }

    atomic64_set(&instance->content_cnt, 0);
    return;
}

static hi_s32 com_instance_mgmt_create_impl(struct klad_slot_mgmt *mgmt, hi_handle handle)
{
    hi_s32 ret;
    struct klad_slot_instance *instance  = HI_NULL;

    __mutex_lock(&mgmt->lock);

    ret = __klad_slot_mgmt_instance_create_impl(mgmt, &instance);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_slot_mgmt_instance_create_impl, ret);
        goto out;
    }

    instance->klad = HI_KLAD_COM;
    _com_instance_mgmt_init_impl(mgmt, &instance->obj.com_slot);

    ret = klad_slot_bind(handle, &instance->obj.com_slot.base);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_slot_bind, ret);
        goto out;
    }
    ret = HI_SUCCESS;
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static hi_s32 __klad_slot_mgmt_instance_destroy_impl(struct klad_slot_mgmt *mgmt, struct klad_slot *slot)
{
    hi_s32 ret;

    __mutex_lock(&slot->lock);

    __mutex_lock(&mgmt->slot_lock);

    list_del(&slot->obj->node);
    list_add_tail(&slot->obj->node, slot->obj->free_list);

    switch (slot->obj->klad_type) {
        case HI_KLAD_COM: {
            struct com_klad_slot *com = (struct com_klad_slot *)slot->obj;

            osal_mutex_destory(&com->lock);

            ret = HI_SUCCESS;
        }
            break;
        case HI_KLAD_TA: {
            struct ta_klad_slot *ta = (struct ta_klad_slot *)slot->obj;

            osal_mutex_destory(&ta->lock);

            ret = HI_SUCCESS;
        }
            break;
        case HI_KLAD_FP: {
            struct fp_klad_slot *fp = (struct fp_klad_slot *)slot->obj;

            osal_mutex_destory(&fp->lock);

            ret = HI_SUCCESS;
        }
            break;
        case HI_KLAD_NONCE: {
            struct nonce_klad_slot *nonce = (struct nonce_klad_slot *)slot->obj;

            osal_mutex_destory(&nonce->lock);

            ret = HI_SUCCESS;
        }
            break;
        case HI_KLAD_CLR: {
            struct clr_route_slot *clr = (struct clr_route_slot *)slot->obj;

            osal_mutex_destory(&clr->lock);

            ret = HI_SUCCESS;
        }
            break;
        default:
            HI_ERR_KLAD("This klad type does not exist.\n");
            ret = HI_FAILURE;
            break;
    }

    __mutex_unlock(&mgmt->slot_lock);

    __mutex_unlock(&slot->lock);
    return ret;
}

static hi_s32 klad_slot_mgmt_instance_destroy_impl(struct klad_slot_mgmt *mgmt, hi_handle handle)
{
    hi_s32 ret;
    struct klad_slot *slot = HI_NULL;

    __mutex_lock(&mgmt->lock);

    ret = klad_slot_find(handle, &slot);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    ret = __klad_slot_mgmt_instance_destroy_impl(mgmt, slot);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    ret = HI_SUCCESS;
out:
    __mutex_unlock(&mgmt->lock);
    return ret;
}

static struct klad_slot_mgmt_ops g_klad_slot_mgmt_ops = {
    .init                      = klad_slot_mgmt_init_impl,
    .exit                      = klad_slot_mgmt_exit_impl,

    .suspend                   = klad_slot_mgmt_suspend_impl,
    .resume                    = klad_slot_mgmt_resume_impl,

    .create_slot               = klad_slot_mgmt_create_slot_impl,
    .destroy_slot              = klad_slot_mgmt_destroy_slot_impl,

    .create_com_instance       = com_instance_mgmt_create_impl,
    .destroy_com_instance      = klad_slot_mgmt_instance_destroy_impl,

    .create_fp_instance        = fp_instance_mgmt_create_impl,
    .destroy_fp_instance       = klad_slot_mgmt_instance_destroy_impl,

    .create_ta_instance        = ta_instance_mgmt_create_impl,
    .destroy_ta_instance       = klad_slot_mgmt_instance_destroy_impl,

    .create_nonce_instance     = nonce_instance_mgmt_create_impl,
    .destroy_nonce_instance    = klad_slot_mgmt_instance_destroy_impl,

    .create_clr_instance       = clr_instance_mgmt_create_impl,
    .destroy_clr_instance      = klad_slot_mgmt_instance_destroy_impl,
};

static struct klad_slot_mgmt g_klad_slot_mgmt = {
    .lock       = {0},
    .ref_count  = ATOMIC_INIT(0),
    .state      = KLAD_SLOT_MGMT_CLOSED,
    .ops        = &g_klad_slot_mgmt_ops,
};

struct klad_slot_mgmt *__get_klad_slot_mgmt(hi_void)
{
    return &g_klad_slot_mgmt;
}

/*
 * it needs to be guaranteed that software keyladder slot management has inited here.
 */
struct klad_slot_mgmt *get_klad_slot_mgmt(hi_void)
{
    struct klad_slot_mgmt *mgmt = __get_klad_slot_mgmt();

    WARN_ON(atomic_read(&mgmt->ref_count) == 0);

    return mgmt;
}
