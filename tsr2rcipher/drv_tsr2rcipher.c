/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: tsr2rcipher export interface definition.
 */

#include "hi_type.h"

#include "hi_drv_tsr2rcipher.h"

#include "drv_tsr2rcipher_func.h"
#include "drv_tsr2rcipher_utils.h"

hi_s32 hi_drv_tsr2rcipher_get_capability(tsr2rcipher_capability *cap)
{
    hi_s32 ret;

    if (cap == HI_NULL) {
        HI_ERR_TSR2RCIPHER("invalid parameter.\n");
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    ret = tsr2rcipher_mgmt_get_cap(cap);
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_get_capability);

hi_s32 hi_drv_tsr2rcipher_create_session(struct tsr2rcipher_session **new_session)
{
    hi_s32 ret;

    if (new_session == HI_NULL) {
        HI_ERR_TSR2RCIPHER("invalid parameter.\n");
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    ret = tsr2rcipher_session_create(new_session);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_create_session);

hi_s32 hi_drv_tsr2rcipher_destroy_session(struct tsr2rcipher_session *session)
{
    hi_s32 ret;

    if (session == HI_NULL) {
        HI_ERR_TSR2RCIPHER("invalid parameter.\n");
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    ret = tsr2rcipher_session_destroy(session);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_destroy_session);

hi_s32 hi_drv_tsr2rcipher_create(const tsr2rcipher_attr *tsc_attr, hi_handle *handle, struct tsr2rcipher_session *session)
{
    hi_s32 ret;

    if ((handle == HI_NULL) || (session == HI_NULL) || (tsc_attr == HI_NULL)) {
        HI_ERR_TSR2RCIPHER("invalid parameter.\n");
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    ret = tsr2rcipher_ch_create(tsc_attr, handle, session);
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_create);

hi_s32 hi_drv_tsr2rcipher_get_attr(hi_handle handle, tsr2rcipher_attr *tsc_attr)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("invalid handle(0x%x).\n", handle);
        goto out;
    }

    if (tsc_attr == HI_NULL) {
        HI_ERR_TSR2RCIPHER("invalid parameter.\n");
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    ret = tsr2rcipher_ch_get_attr(handle, tsc_attr);
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_get_attr);

hi_s32 hi_drv_tsr2rcipher_set_attr(hi_handle handle, const tsr2rcipher_attr *tsc_attr)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("invalid handle(0x%x).\n", handle);
        goto out;
    }

    if (tsc_attr == HI_NULL) {
        HI_ERR_TSR2RCIPHER("invalid parameter.\n");
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    ret = tsr2rcipher_ch_set_attr(handle, tsc_attr);
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_set_attr);

hi_s32 hi_drv_tsr2rcipher_get_keyslot_handle(hi_handle tsc_handle, hi_handle *ks_handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(tsc_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("invalid handle(0x%x).\n", tsc_handle);
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    if (ks_handle == HI_NULL) {
        HI_ERR_TSR2RCIPHER("invalid parameter.\n");
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    ret = tsr2rcipher_ch_get_ks_handle(tsc_handle, ks_handle);
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_get_keyslot_handle);

hi_s32 hi_drv_tsr2rcipher_attach_keyslot(hi_handle tsc_handle, hi_handle ks_handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(tsc_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("invalid tsr2rcipher handle(0x%x).\n", tsc_handle);
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    ret = CHECK_KS(ks_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("invalid keyslot handle(0x%x).\n", ks_handle);
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    ret = tsr2rcipher_ch_attach_ks(tsc_handle, ks_handle);
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_attach_keyslot);

hi_s32 hi_drv_tsr2rcipher_detach_keyslot(hi_handle tsc_handle, hi_handle ks_handle)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(tsc_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("invalid tsr2rcipher handle(0x%x).\n", tsc_handle);
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    ret = CHECK_KS(ks_handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("invalid keyslot handle(0x%x).\n", ks_handle);
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    ret = tsr2rcipher_ch_detach_ks(tsc_handle, ks_handle);
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_detach_keyslot);

hi_s32 hi_drv_tsr2rcipher_set_iv(hi_handle handle, tsr2rcipher_iv_type iv_type, hi_u8 *iv, hi_u32 iv_len)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    if ((iv == HI_NULL) || (iv_len < TSR2RCIPHER_MIN_IV_LEN) || (iv_len > TSR2RCIPHER_MAX_IV_LEN) ||
        (iv_type >= TSR2RCIPHER_IV_MAX)) {
        HI_ERR_TSR2RCIPHER("invalid parameter. IV[%#x], ivlen[%#x], ivtype[%#x]\n", iv, iv_len, iv_type);
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    ret = tsr2rcipher_ch_set_iv(handle, iv_type, iv, iv_len);
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_set_iv);

static hi_s32 drv_tsr2rcipher_trim_data_args(hi_mem_handle_t src_buf_handle, hi_mem_handle_t dst_buf_handle, hi_u32 len)
{
    if ((len < TSR2RCIPHER_TS_PACKAGE_LEN) || (len > TSR2RCIPHER_MAX_SIZE_PRE_DESC) ||
        (len % TSR2RCIPHER_TS_PACKAGE_LEN)) {
        HI_ERR_TSR2RCIPHER("data len = 0x%x is invalid, correct rang is (0x%x ~ 0x%x ), or not 188 times\n",
                        len, TSR2RCIPHER_TS_PACKAGE_LEN, TSR2RCIPHER_MAX_SIZE_PRE_DESC);
        return HI_ERR_TSR2RCIPHER_INVALID_PARA;
    }

    if (src_buf_handle < 0 || dst_buf_handle < 0) {
        HI_ERR_TSR2RCIPHER("src_buf_handle[%d] or dst_buf_handle[%d] is invalid!\n",
                            src_buf_handle, dst_buf_handle);
        return HI_ERR_TSR2RCIPHER_INVALID_PARA;
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_tsr2rcipher_encrypt(hi_handle handle, tsr2rcipher_mem_handle src_mem_handle, tsr2rcipher_mem_handle dst_mem_handle, hi_u32 data_len)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("invalid handle(0x%x).\n", handle);
        goto out;
    }

    ret = drv_tsr2rcipher_trim_data_args(src_mem_handle.mem_handle, dst_mem_handle.mem_handle, data_len);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = tsr2rcipher_ch_encrypt(handle, src_mem_handle, dst_mem_handle, data_len);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_encrypt);

hi_s32 hi_drv_tsr2rcipher_decrypt(hi_handle handle, tsr2rcipher_mem_handle src_mem_handle, tsr2rcipher_mem_handle dst_mem_handle, hi_u32 data_len)
{
    hi_s32 ret;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("invalid handle(0x%x).\n", handle);
        goto out;
    }

    ret = drv_tsr2rcipher_trim_data_args(src_mem_handle.mem_handle, dst_mem_handle.mem_handle, data_len);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = tsr2rcipher_ch_decrypt(handle, src_mem_handle, dst_mem_handle, data_len);

out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_decrypt);

hi_s32 hi_drv_tsr2rcipher_destroy(hi_handle handle)
{
    hi_s32 ret = HI_FAILURE;

    ret = CHECK_HANDLE(handle);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("invalid handle(0x%x).\n", handle);
        ret = HI_ERR_TSR2RCIPHER_INVALID_PARA;
        goto out;
    }

    ret = tsr2rcipher_ch_destroy(handle);
out:
    return ret;
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_destroy);

hi_s32 hi_drv_tsr2rcipher_suspend(hi_void)
{
    return tsr2rcipher_mgmt_suspend();
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_suspend);

hi_s32 hi_drv_tsr2rcipher_resume(hi_void)
{
    return tsr2rcipher_mgmt_resume();
}
EXPORT_SYMBOL(hi_drv_tsr2rcipher_resume);

