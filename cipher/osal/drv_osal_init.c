/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_osal_init
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include <linux/device.h>
#include <linux/cdev.h>
#include "hi_drv_sys.h"
#include "drv_osal_lib.h"
#include "drv_cipher_ext.h"
#include "drv_symc.h"
#include "drv_hash.h"
#include "drv_hdcp.h"
#include "cryp_symc.h"
#include "cryp_hash.h"
#include "drv_osal_tee.h"

/************************ Internal Structure Definition *********************/

#define SYMC_ALG_MODE_CNT 48
#define HASH_MODE_CNT     8
#define HDCP_KEY_SEL_CNT  4

#define HI_MOD_CIPHER "cipher"

hi_s32 crypto_ioctl(hi_u32 cmd, hi_void *argp);
hi_s32 crypto_entry(void);
hi_s32 crypto_exit(void);
hi_s32 crypto_release(void);
hi_s32 crypto_recover_hdcp_key(void);

static struct class *g_cipher_class = HI_NULL;
static dev_t g_cipher_devno;
static struct cdev *g_cipher_cdev = HI_NULL;
static struct device *g_cipher_dev = HI_NULL;

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Code *****************************/
/** \addtogroup      link */
/** @{ */ /** <!-- [link] */

hi_cipher_export_fun cipher_export_funcs = {
    .hi_cipher_create_handle = hi_drv_cipher_create_handle,
    .hi_cipher_destroy_handle = hi_drv_cipher_destroy_handle,
    .hi_cipher_get_key_slot_handle = hi_drv_cipher_get_key_slot_handle,
    .hi_cipher_config_chn = hi_drv_cipher_config_chn,
    .hi_cipher_encrypt = hi_drv_cipher_encrypt,
    .hi_cipher_decrypt = hi_drv_cipher_decrypt,
    .hi_cipher_get_random_number = hi_drv_cipher_get_random_number,
    .hi_cipher_load_hdcp_key = hi_drv_cipher_load_hdcp_key,
    .hi_cipher_soft_reset = hi_drv_cipher_soft_reset,
    .hi_cipher_hash_start = hi_drv_cipher_hash_start,
    .hi_cipher_hash_update = hi_drv_cipher_hash_update,
    .hi_cipher_hash_finish = hi_drv_cipher_hash_finish,
};

/******* proc function begin ********/
#ifdef HI_PROC_SUPPORT
hi_s32 symc_proc_read(hi_void *seqfile, hi_void *v)
{
    symc_chn_status status[CRYPTO_HARD_CHANNEL_MAX];
    symc_module_status module;
    hdcp_module_status hdcp;
    const char *alg_str[SYMC_ALG_COUNT] = { " DES", "TDES", " AES", " SM4",  " SM1", " DMA" };
    const char *mode_str[SYMC_MODE_COUNT] = { "ECB", "CBC", "CFB", "OFB",  "CTR", "CCM",  "GCM", "CTS" };
    const char *klen_str[SYMC_KEY_LEN_COUNT] = { "DEF", "192", "256", "128" };
    const char *hash_str[HASH_MODE_COUNT] = { "SHA1", "SHA224", "SHA256", "SHA384",  "SHA512", "SM3" };
    const char *hdcp_str[HDCP_KEY_SEL_CNT] = { "hdcp_root_key", "hisi_key", "host_key", "host_key" };
    struct seq_file *p = HI_NULL;
    hi_u8 alg[SYMC_ALG_MODE_CNT] = { 0 };
    hi_u8 mode[SYMC_ALG_MODE_CNT] = { 0 };
    hi_u32 count;
    unsigned int i = 0;

    if (seqfile == HI_NULL) {
        return HI_FAILURE;
    }
    p = (struct seq_file *)seqfile;

    memset(status, 0, sizeof(status));
    memset(&module, 0, sizeof(module));
    memset(&hdcp, 0, sizeof(hdcp));

#ifdef CHIP_HDCP_SUPPORT
    drv_hdcp_proc_status(&hdcp);
#endif
    drv_symc_proc_status(status, sizeof(status), &module);

    HI_PROC_PRINT(p, "---------------------------------------------------------------------------------------------\n");
    HI_PROC_PRINT(p, "|                                      C I P H E R - I N F O                                |\n");
    HI_PROC_PRINT(p, "---------------------------------------------------------------------------------------------\n");
    HI_PROC_PRINT(p, "RESET     :\t\t%s\n", module.allow_reset ? "Allowed" : "Forbidden");
    HI_PROC_PRINT(p, "EXP-CPU   :\t\t%s\n", module.sec_cpu ? "Secure" : "Non-secure");
    HI_PROC_PRINT(p, "NAME      :\t\t%s\n", module.name);
    HI_PROC_PRINT(p, "INT-NUM   :\t\t%d\n", module.int_num);
    HI_PROC_PRINT(p, "INT-EN    :\t\t%d\n", module.int_en);
    HI_PROC_PRINT(p, "SMMU-BASE :\t\t0x%08x\n", module.smmu_base);
    HI_PROC_PRINT(p, "ERR-CODE  :\t\t0x%08x\n", module.err_code);
    HI_PROC_PRINT(p, "RX-READ   :\t\t%s\n", hdcp.rx_read ? "enable" : "disable");
    HI_PROC_PRINT(p, "TX-READ   :\t\t%s\n", hdcp.tx_read ? "enable" : "disable");
    HI_PROC_PRINT(p, "HDCP-KEY  :\t\t%s\n", hdcp_str[hdcp.key_sel]);
    HI_PROC_PRINT(p, "HDCP-CRC4 :\t\t0x%02x\n", hdcp.crc4);

    HI_PROC_PRINT(p, "\n------------------------------------------ CIPHER CONFIG ----------------------------------\n");
    HI_PROC_PRINT(p, "Chnid Status Decrypt Alg  Mode KeyLen  Addr(in / out)     "
        "KeyFrom INT-RAW INT-EN INT_STATUS Owner\n");
    for (i = 0; i < CRYPTO_HARD_CHANNEL_MAX; i++) {
        if (((0x1 << i) & CIPHER_HARD_CHANNEL_MASK) == 0x00) {
            continue;
        }

        kapi_symc_get_user_uuid(i, &status[i].owner);

        HI_PROC_PRINT(p, " %d    %s    %s   %s  %s   %s   %08x/%08x     %s      %d       %d      %d       %08x\n",
                      i,
                      status[i].open ? "open " : "close",
                      status[i].decrypt ? "DEC" : "ENC",
                      alg_str[status[i].alg],
                      mode_str[status[i].mode],
                      klen_str[status[i].klen],
                      status[i].inaddr,
                      status[i].outaddr,
                      status[i].ksrc ? "HW" : "SW",
                      status[i].int_raw,
                      status[i].int_en,
                      status[i].int_status,
                      status[i].owner);
    }

    HI_PROC_PRINT(p, "\n------------------------------------------ CIPHER STATUS ----------------------------------\n");
    HI_PROC_PRINT(p, "Chnid SEC SMMU IN-NODE_PTR(r/w/d)     OUT-NODE_PTR(r/w/d)    IVOUT\n");
    for (i = 0; i < CRYPTO_HARD_CHANNEL_MAX; i++) {
        if (((0x1 << i) & CIPHER_HARD_CHANNEL_MASK) == 0x00) {
            continue;
        }

        kapi_symc_get_user_uuid(i, &status[i].owner);

        HI_PROC_PRINT(p, " %d     %d   %d   %08x(%03d/%03d/%03d)  %08x(%03d/%03d/%03d)  %s\n",
                      i,
                      status[i].secure,
                      status[i].smmu_enable,
                      status[i].in_node_head,
                      status[i].in_node_rptr,
                      status[i].in_node_wptr,
                      status[i].in_node_depth,
                      status[i].out_node_head,
                      status[i].out_node_rptr,
                      status[i].out_node_wptr,
                      status[i].out_node_depth,
                      status[i].iv);
    }

    count = SYMC_ALG_MODE_CNT;
    cryp_get_symc_alg(alg, mode, &count);
    HI_PROC_PRINT(p, "\n---------------------------------------- CIPHER ALGORITHM -----------------------------------");
    for (i = 0; i < count; i++) {
        if ((i % 12 == 0)) { /* 12 alg in one line */
            HI_PROC_PRINT(p, "\n");
        }
        HI_PROC_PRINT(p, "%s-%s ", alg_str[alg[i]], mode_str[mode[i]]);
    }
    HI_PROC_PRINT(p, "\n");
    count = HASH_MODE_CNT;
    cryp_get_hash_mode(mode, &count);
    for (i = 0; i < count; i++) {
        HI_PROC_PRINT(p, "%-8s", hash_str[mode[i]]);
    }
    HI_PROC_PRINT(p, "\n");

    return HI_SUCCESS;
}


static osal_proc_cmd g_proc_cmd[] = {
};

static hi_s32 symc_proc_init(hi_void)
{
    osal_proc_entry *item;

    item = osal_proc_add(HI_MOD_CIPHER_NAME, sizeof(HI_MOD_CIPHER_NAME));
    if (item == HI_NULL) {
        return HI_FAILURE;
    }

    item->read = symc_proc_read;
    item->cmd_list = g_proc_cmd;
    item->cmd_cnt = sizeof(g_proc_cmd) / sizeof(osal_proc_cmd);

    return HI_SUCCESS;
}

static hi_void symc_proc_deinit(hi_void)
{
    osal_proc_remove(HI_MOD_CIPHER_NAME, sizeof(HI_MOD_CIPHER_NAME));
}
#endif
/******* proc function end ********/
static int hi_cipher_suspend(struct device *dev)
{
    drv_symc_suspend();

#ifdef CHIP_HASH_SUPPORT
    drv_hash_suspend();
#endif

    HI_PRINT("CIPHER suspend OK\n");

    return HI_SUCCESS;
}

static int hi_cipher_resume(struct device *dev)
{
    hi_s32 ret;

    ret = drv_symc_resume();
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

#ifdef CHIP_HASH_SUPPORT
    drv_hash_resume();
#endif

    HI_PRINT("CIPHER resume OK\n");

    return HI_SUCCESS;
}

static struct dev_pm_ops g_cipher_pm_ops = {
    .suspend    = hi_cipher_suspend,
    .resume     = hi_cipher_resume,
};

hi_slong _crypto_ioctl(struct file *file, hi_u32 cmd, hi_void *arg)
{
    return crypto_ioctl(cmd, arg);
}

static long hi_cipher_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg)
{
    long ret;

    ret = (long)hi_drv_user_copy(ffile, cmd, arg, _crypto_ioctl);

    return ret;
}

static hi_s32 hi_cipher_release(struct inode *inode, struct file *file)
{
    return crypto_release();
}

static struct file_operations g_cipher_fops = {
    .owner = THIS_MODULE,
    .open = HI_NULL,
    .release = hi_cipher_release,
    .unlocked_ioctl = hi_cipher_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl = hi_cipher_ioctl,
#endif
};

static hi_s32 cipher_register_dev(hi_void)
{
    hi_s32 ret;

    ret = alloc_chrdev_region(&g_cipher_devno, 0, 1, "cipher");
    if (ret != HI_SUCCESS) {
        hi_log_error("cipher alloc chrdev region failed\n");

        return HI_FAILURE;
    }

    g_cipher_cdev = cdev_alloc();
    if (IS_ERR(g_cipher_cdev)) {
        hi_log_error("cipher alloc cdev failed, err(%d)\n", PTR_ERR(g_cipher_cdev));

        ret = HI_FAILURE;
        goto out0;
    }

    cdev_init(g_cipher_cdev, &g_cipher_fops);
    g_cipher_cdev->owner = THIS_MODULE;

    ret = cdev_add(g_cipher_cdev, g_cipher_devno, 1);
    if (ret != HI_SUCCESS) {
        hi_log_error("cipher add cdev failed, ret(%d).\n", ret);
        ret = HI_FAILURE;
        goto out1;
    }

    g_cipher_class = class_create(THIS_MODULE, "cipher_class");
    if (IS_ERR(g_cipher_class)) {
        hi_log_error("cipher create dev class failed, err(%d)\n", PTR_ERR(g_cipher_class));

        ret = HI_FAILURE;
        goto out2;
    }

    g_cipher_class->pm = &g_cipher_pm_ops;

    g_cipher_dev = device_create(g_cipher_class, HI_NULL, g_cipher_devno, HI_NULL, HI_DEV_CIPHER_NAME);
    if (IS_ERR(g_cipher_dev)) {
        hi_log_error("cipher create dev failed, err(%d)\n", PTR_ERR(g_cipher_dev));
        ret = HI_FAILURE;

        goto out3;
    }

    return HI_SUCCESS;

out3:
    class_destroy(g_cipher_class);
    g_cipher_class = HI_NULL;
out2:
    cdev_del(g_cipher_cdev);
out1:
    osal_kfree(HI_ID_CIPHER, g_cipher_cdev);
    g_cipher_cdev = HI_NULL;
out0:
    unregister_chrdev_region(g_cipher_devno, 1);

    return ret;
}

static hi_void cipher_unregister_dev(hi_void)
{
    device_destroy(g_cipher_class, g_cipher_devno);
    class_destroy(g_cipher_class);
    cdev_del(g_cipher_cdev);
    osal_kfree(HI_ID_CIPHER, g_cipher_cdev);
    unregister_chrdev_region(g_cipher_devno, 1);

    g_cipher_dev = HI_NULL;
    g_cipher_class = HI_NULL;
    g_cipher_cdev = HI_NULL;
    g_cipher_devno = 0;

    return;
}

hi_s32 cipher_mod_init(hi_void)
{
    hi_s32 ret;

    ret = cipher_register_dev();
    if (ret != HI_SUCCESS) {
        hi_log_error("register cipher device failed\n");
        goto out;
    }

#ifdef MODULE
    HI_PRINT("load hi_cipher.ko success.  \t(%s)\n", VERSION_STRING);
#endif

out:
    return HI_SUCCESS;
}

hi_void cipher_mod_exit(hi_void)
{
    cipher_unregister_dev();

    HI_PRINT("remove hi_cipher.ko success.\n");
}

hi_s32 cipher_drv_mod_init(hi_void)
{
    hi_s32 ret;

    ret = cipher_register_dev();
    if (ret != HI_SUCCESS) {
        hi_log_error("register cipher device failed\n");
        return ret;
    }

    ret = osal_exportfunc_register(HI_ID_CIPHER, CIPHER_NAME, &cipher_export_funcs);
    if (HI_SUCCESS != ret) {
        hi_log_print_func_err(osal_exportfunc_register, ret);
        goto error;
    }

    ret = crypto_entry();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(crypto_entry, ret);
        goto error;
    }

    ret = drv_osal_tee_open_session();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_osal_tee_open_session, ret);
        (hi_void) crypto_exit();
        goto error;
    }

    /******* proc function begin ********/
#ifdef HI_PROC_SUPPORT
    ret = symc_proc_init();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(symc_proc_init, ret);
        goto error;
    }
#endif
    /******* proc function end ********/
#ifdef MODULE
    HI_PRINT("Load hi_cipher.ko success.\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;

error:
    cipher_unregister_dev();

    return ret;
}

hi_void cipher_drv_mod_exit(hi_void)
{
    hi_s32 ret;

    ret = crypto_exit();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(crypto_exit, ret);
        return;
    }

    ret = drv_osal_tee_close_session();
    if (ret != HI_SUCCESS) {
        hi_log_print_func_err(drv_osal_tee_close_session, ret);
        return;
    }

    /******* proc function begin ********/
#ifdef HI_PROC_SUPPORT
    symc_proc_deinit();
#endif
    /******* proc function end ********/
    cipher_unregister_dev();

    return;
}

#ifdef MODULE
module_init(cipher_drv_mod_init);
module_exit(cipher_drv_mod_exit);
#endif

EXPORT_SYMBOL(hi_cipher_suspend);
EXPORT_SYMBOL(hi_cipher_resume);

MODULE_AUTHOR("Hi3720 MPP GRP");
MODULE_LICENSE("GPL");

/** @} */ /** <!-- ==== Structure Definition end ==== */
