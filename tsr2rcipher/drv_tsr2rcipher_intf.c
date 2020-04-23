/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: tsr2rcipher interface of module.
 */

#include <linux/module.h>

#include "hi_type.h"
#include "hi_osal.h"

#include "hi_drv_dev.h"
#include "hi_drv_mem.h"
#include "hi_drv_proc.h"
#include "hi_drv_sys.h"

#include "hi_drv_tsr2rcipher.h"

#include "drv_tsr2rcipher_ioctl.h"
#include "drv_tsr2rcipher_utils.h"
#include "drv_tsr2rcipher_define.h"
#include "drv_tsr2rcipher_func.h"

static hi_s32 tsr2rcipher_open(hi_void *private_data)
{
    hi_s32 ret;
    struct tsr2rcipher_session *session = HI_NULL;

    ret = hi_drv_tsr2rcipher_create_session(&session);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("create session failed!\n");
        goto out;
    }

    *((struct tsr2rcipher_session **)private_data) = session;

out:
    return ret;
}

static hi_s32 tsr2rcipher_release(hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    if (*((struct tsr2rcipher_session **)private_data) != HI_NULL) {
        struct tsr2rcipher_session *session = *((struct tsr2rcipher_session **)private_data);

        ret = hi_drv_tsr2rcipher_destroy_session(session);
        if (ret == HI_SUCCESS) {
            *((struct tsr2rcipher_session **)private_data) = HI_NULL;
        }
    }

    return ret;
}

static hi_s32 tsr2rcipher_ioctl_gbl_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case TSR2RCIPHER_IOCTL_GETCAP: {
            tsr2rcipher_capability *info = (tsr2rcipher_capability *)arg;
            ret = hi_drv_tsr2rcipher_get_capability(info);
            break;
        }
        default: {
            HI_ERR_TSR2RCIPHER("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}

static hi_s32 tsr2rcipher_ioctl_ch_impl(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret = HI_FAILURE;

    switch (cmd) {
        case TSR2RCIPHER_IOCTL_CREATE: {
            tsr2rcipher_create_info *info = (tsr2rcipher_create_info *)arg;
            ret = hi_drv_tsr2rcipher_create(&info->tsc_attr, &info->handle,
                *((struct tsr2rcipher_session **)private_data));
            break;
        }
        case TSR2RCIPHER_IOCTL_DESTROY: {
            hi_handle *handle = (hi_handle *)arg;
            ret = hi_drv_tsr2rcipher_destroy(*handle);
            break;
        }
        case TSR2RCIPHER_IOCTL_GETATTR: {
            tsr2rcipher_get_attr_info *info = (tsr2rcipher_get_attr_info *)arg;
            ret = hi_drv_tsr2rcipher_get_attr(info->handle, &info->tsc_attr);
            break;
        }
        case TSR2RCIPHER_IOCTL_SETATTR: {
            tsr2rcipher_set_attr_info *info = (tsr2rcipher_set_attr_info *)arg;
            ret = hi_drv_tsr2rcipher_set_attr(info->handle, &info->tsc_attr);
            break;
        }
        case TSR2RCIPHER_IOCTL_GETKS: {
            tsr2rcipher_get_ks_handle *info = (tsr2rcipher_get_ks_handle *)arg;
            ret = hi_drv_tsr2rcipher_get_keyslot_handle(info->tsc_handle, &info->ks_handle);
            break;
        }
        case TSR2RCIPHER_IOCTL_ATTACHKS: {
            tsr2rcipher_attach_ks *info = (tsr2rcipher_attach_ks *)arg;
            ret = hi_drv_tsr2rcipher_attach_keyslot(info->tsc_handle, info->ks_handle);
            break;
        }
        case TSR2RCIPHER_IOCTL_DETACHKS: {
            tsr2rcipher_detach_ks *info = (tsr2rcipher_detach_ks *)arg;
            ret = hi_drv_tsr2rcipher_detach_keyslot(info->tsc_handle, info->ks_handle);
            break;
        }
        case TSR2RCIPHER_IOCTL_SETIV: {
            tsr2rcipher_set_iv_info *info = (tsr2rcipher_set_iv_info *)arg;
            ret = hi_drv_tsr2rcipher_set_iv(info->handle, info->type, info->iv, info->len);
            break;
        }
        case TSR2RCIPHER_IOCTL_ENCRYPT: {
            tsr2rcipher_deal_data_info *info = (tsr2rcipher_deal_data_info *)arg;
            ret = hi_drv_tsr2rcipher_encrypt(info->handle, info->src_mem_handle, info->dst_mem_handle, info->data_len);
            break;
        }
        case TSR2RCIPHER_IOCTL_DECRYPT: {
            tsr2rcipher_deal_data_info *info = (tsr2rcipher_deal_data_info *)arg;
            ret = hi_drv_tsr2rcipher_decrypt(info->handle, info->src_mem_handle, info->dst_mem_handle, info->data_len);
            break;
        }
        default: {
            HI_ERR_TSR2RCIPHER("unknown cmd: 0x%x\n", cmd);
            break;
        }
    }

    return ret;
}

static hi_s32 tsr2rcipher_suspend(hi_void *private_data)
{
    hi_s32 ret;

    ret = hi_drv_tsr2rcipher_suspend();
    if (ret == HI_SUCCESS) {
        HI_PRINT("tsr2rcipher suspend ok.\n");
    }

    return ret;
}

static hi_s32 tsr2rcipher_resume(hi_void *private_data)
{
    hi_s32 ret;

    ret = hi_drv_tsr2rcipher_resume();
    if (ret == HI_SUCCESS) {
        HI_PRINT("tsr2rcipher resume ok.\n");
    }

    return ret;
}

static osal_ioctl_cmd g_tsr2rcipher_ioctl_info[] = {
    {TSR2RCIPHER_IOCTL_GETCAP,   tsr2rcipher_ioctl_gbl_impl},
    {TSR2RCIPHER_IOCTL_CREATE,   tsr2rcipher_ioctl_ch_impl},
    {TSR2RCIPHER_IOCTL_DESTROY,  tsr2rcipher_ioctl_ch_impl},
    {TSR2RCIPHER_IOCTL_GETATTR,  tsr2rcipher_ioctl_ch_impl},
    {TSR2RCIPHER_IOCTL_SETATTR,  tsr2rcipher_ioctl_ch_impl},
    {TSR2RCIPHER_IOCTL_SETIV,    tsr2rcipher_ioctl_ch_impl},
    {TSR2RCIPHER_IOCTL_ENCRYPT,  tsr2rcipher_ioctl_ch_impl},
    {TSR2RCIPHER_IOCTL_DECRYPT,  tsr2rcipher_ioctl_ch_impl},
    {TSR2RCIPHER_IOCTL_GETKS,    tsr2rcipher_ioctl_ch_impl},
    {TSR2RCIPHER_IOCTL_ATTACHKS, tsr2rcipher_ioctl_ch_impl},
    {TSR2RCIPHER_IOCTL_DETACHKS, tsr2rcipher_ioctl_ch_impl}
};

static osal_fileops g_tsr2rcipher_fops = {
    .open     = tsr2rcipher_open,
    .release  = tsr2rcipher_release,
    .cmd_list = g_tsr2rcipher_ioctl_info,
    .cmd_cnt  = sizeof(g_tsr2rcipher_ioctl_info) / sizeof(osal_ioctl_cmd),
};

static osal_pmops g_tsr2rcipher_pm_ops = {
    .pm_suspend = tsr2rcipher_suspend,
    .pm_resume = tsr2rcipher_resume,
};

static osal_dev g_tsr2rcipher_dev = {
    .name = HI_DEV_TSR2RCIPHER_NAME,
    .minor = HI_DEV_TSR2RCIPHER_MINOR,
    .fops = &g_tsr2rcipher_fops,
    .pmops = &g_tsr2rcipher_pm_ops,
};

static fn_tsr2rcipher_export_func g_tsr2rcipher_export_funcs = {
    .pfn_tsr2rcipher_get_capability = hi_drv_tsr2rcipher_get_capability,
};

static hi_s32 tsr2rcipher_dev_register(hi_void)
{
    return osal_dev_register(&g_tsr2rcipher_dev);
}

static hi_void tsr2rcipher_dev_unregister(hi_void)
{
    osal_dev_unregister(&g_tsr2rcipher_dev);
}

hi_s32 tsr2rcipher_mod_init(hi_void)
{
    hi_s32 ret;

    ret = tsr2rcipher_dev_register();
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("register tsr2rcipher dev failed.\n");
        goto out0;
    }

    ret = osal_exportfunc_register(HI_ID_TSR2RCIPHER, TSR2RCIPHER_NAME, (hi_void *)&g_tsr2rcipher_export_funcs);
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("register tsr2rcipher module failed.\n");
        goto out1;
    }

    ret = tsr2rcipher_slot_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("slot init failed.\n");
        goto out2;
    }

    ret = tsr2rcipher_mgmt_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_TSR2RCIPHER("mgmt init failed!\n");
        goto out3;
    }

#ifdef MODULE
    HI_PRINT("Load hi_tsr2rcipher.ko success.\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;

out3:
    tsr2rcipher_slot_exit();
out2:
    osal_exportfunc_unregister(HI_ID_TSR2RCIPHER);
out1:
    tsr2rcipher_dev_unregister();
out0:
    return ret;
}

hi_void tsr2rcipher_mod_exit(hi_void)
{
    tsr2rcipher_mgmt_exit();

    tsr2rcipher_slot_exit();

    osal_exportfunc_unregister(HI_ID_TSR2RCIPHER);

    tsr2rcipher_dev_unregister();

    HI_PRINT("remove hi_tsr2rcipher.ko success.\n");
}

#ifdef MODULE
module_init(tsr2rcipher_mod_init);
module_exit(tsr2rcipher_mod_exit);
#endif

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");
