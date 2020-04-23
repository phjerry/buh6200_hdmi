/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
 * Description :Add proc node in virtual fs.
 * Author : Linux SDK team
 * Created : 2019-06-22
 */
#include "drv_ks_proc.h"

#include "hi_drv_dev.h"
#include "hi_osal.h"
#include "drv_ks_define.h"
#include "drv_ks_func.h"
#include "hal_ks.h"

static hi_void _ks_proc_get_help(hi_void)
{
    hi_drv_proc_echo_helper("\nUsage as following: \n");
    hi_drv_proc_echo_helper("    cat /proc/msp/keyslot                Display all proc information \n");
    hi_drv_proc_echo_helper("    echo help > /proc/msp/keyslot        Display help infomation for ks proc module \n");
    hi_drv_proc_echo_helper("    echo clean > /proc/msp/keyslot       Unlock all keyslot \n");
    hi_drv_proc_echo_helper("    echo lock   module slot > /proc/msp/keyslot Lock keyslot \n");
    hi_drv_proc_echo_helper("    echo unlock module slot > /proc/msp/keyslot Unlock keyslot \n");
    hi_drv_proc_echo_helper("                module:  0:tscipher, 1:mcipher, 2:hmac\n");
    hi_drv_proc_echo_helper("                slot  :  mcipher: 0~15, tscipher: 0~255, hmac: 15 \n");
    return;
}

#define STR_LEN 16
static hi_s32 _ks_proc_lock_process(hi_keyslot_type slot_ind, hi_u32 slot_num)
{
    hi_s32 ret;
    hi_char module_name[HI_KEYSLOT_TYPE_MAX][STR_LEN] = {"tscipher", "mcipher", "hmac"};

    if (slot_ind >= HI_KEYSLOT_TYPE_MAX) {
        return HI_FAILURE;
    }
    ret = ks_mgmt_lock(slot_ind, slot_num);
    if ((ret < HI_ERR_KS_LOCKED_CPUX + KS_STAT_MAX) && (ret >= HI_ERR_KS_LOCKED_CPUX)) {
        hi_drv_proc_echo_helper("%s has been locked..\n", module_name[slot_ind]);
        return HI_SUCCESS;
    }
    if (ret != HI_SUCCESS) {
        hi_drv_proc_echo_helper("lock %s slot %d failed.\n", module_name[slot_ind], slot_num);
    } else {
        hi_drv_proc_echo_helper("lock %s slot %d successfull.\n", module_name[slot_ind], slot_num);
    }
    return HI_SUCCESS;
}

static hi_s32 _ks_proc_unlock_process(hi_keyslot_type slot_ind, hi_u32 slot_num)
{
    hi_s32 ret;
    hi_char module_name[HI_KEYSLOT_TYPE_MAX][STR_LEN] = {"tscipher", "mcipher", "hmac"};

    if (slot_ind >= HI_KEYSLOT_TYPE_MAX) {
        return HI_FAILURE;
    }

    ret = ks_mgmt_unlock(slot_ind, slot_num);
    if ((ret < HI_ERR_KS_LOCKED_CPUX + KS_STAT_MAX) && (ret >= HI_ERR_KS_LOCKED_CPUX)) {
        hi_drv_proc_echo_helper("%s has been locked by other CPU or unlocked..\n", module_name[slot_ind]);
        return HI_SUCCESS;
    }
    if (ret != HI_SUCCESS) {
        hi_drv_proc_echo_helper("Unlock %s slot %d failed.\n", module_name[slot_ind], slot_num);
    } else {
        hi_drv_proc_echo_helper("Unlock %s slot %d successfull.\n", module_name[slot_ind], slot_num);
    }
    return HI_SUCCESS;
}

static hi_s32 _ks_proc_phrase_lock_param(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX],
                                         hi_keyslot_type *mod, hi_u32 *slot)
{
    hi_keyslot_type slot_ind;
    hi_u32 slot_num;

    slot_ind = (hi_keyslot_type)osal_strtoul(argv[0x1], HI_NULL, HI_NULL); /* 0x1: module */
    if (slot_ind >= HI_KEYSLOT_TYPE_MAX) {
        hi_drv_proc_echo_helper("invalid module param.\n");
        return HI_ERR_OTP_PTR_NULL;
    }

    slot_num = (hi_u32)osal_strtoul(argv[0x2], HI_NULL, HI_NULL); /* 0x2: slot */

    if (((slot_ind == HI_KEYSLOT_TYPE_TSCIPHER) && (slot_num > HI_KEYSLOT_TSCIPHER_MAX)) ||
        ((slot_ind == HI_KEYSLOT_TYPE_MCIPHER) && (slot_num > HI_KEYSLOT_MCIPHER_MAX)) ||
        ((slot_ind == HI_KEYSLOT_TYPE_HMAC) && (slot_num != HI_KEYSLOT_MCIPHER_MAX))) {
        hi_drv_proc_echo_helper("invalid slot num param.\n");
        return HI_FAILURE;
    }

    *mod = slot_ind;
    *slot = slot_num;

    return HI_SUCCESS;
}

static hi_s32 _ks_proc_lock(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX])
{
    hi_s32 ret;
    hi_u32 slot = 0;
    hi_keyslot_type mod = HI_KEYSLOT_TYPE_MAX;

    ret = _ks_proc_phrase_lock_param(argc, argv, &mod, &slot);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    return _ks_proc_lock_process(mod, slot);

}

static hi_s32 _ks_proc_unlock(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX])
{
    hi_s32 ret;
    hi_u32 slot = 0;
    hi_keyslot_type mod = HI_KEYSLOT_TYPE_MAX;

    ret = _ks_proc_phrase_lock_param(argc, argv, &mod, &slot);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    return _ks_proc_unlock_process(mod, slot);
}

static hi_s32 __ks_proc_unlock_slot(const hi_keyslot_type slot_ind, const hi_u32 slot_num)
{
    ks_slot_stat state;

    state = hal_ks_status(slot_ind, slot_num);
#ifdef HI_TEE_SUPPORT
    if (state != KS_STAT_REE_LOCK) {
        return HI_ERR_KS_LOCKED_CPUX + state;
    }
#else
    if (state != KS_STAT_TEE_LOCK) {
        return HI_ERR_KS_LOCKED_CPUX + state;
    }
#endif
    return hal_ks_unlock(slot_ind, slot_num);
}

static hi_s32 _ks_proc_unlock_slot(hi_keyslot_type slot_ind, hi_u32 slot_num)
{
    hi_s32 ret;
    hi_char module_name[HI_KEYSLOT_TYPE_MAX][STR_LEN] = {"tscipher", "mcipher", "hmac"};

    if (slot_ind >= HI_KEYSLOT_TYPE_MAX) {
        return HI_FAILURE;
    }

    ret = __ks_proc_unlock_slot(slot_ind, slot_num);
    if ((ret < HI_ERR_KS_LOCKED_CPUX + KS_STAT_MAX) && (ret >= HI_ERR_KS_LOCKED_CPUX)) {
        return HI_SUCCESS;
    }
    if (ret != HI_SUCCESS) {
        hi_drv_proc_echo_helper("Unlock %s slot %d failed.\n", module_name[slot_ind], slot_num);
    } else {
        hi_drv_proc_echo_helper("Unlock %s slot %d successfull.\n", module_name[slot_ind], slot_num);
    }
    return HI_SUCCESS;
}

static hi_s32 _ks_proc_clean(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX])
{
    hi_u32 start_slot;
    hi_s32 ret;

    for (start_slot = 0; start_slot < HI_KEYSLOT_TSCIPHER_MAX ; start_slot++) {
        ret = _ks_proc_unlock_slot(HI_KEYSLOT_TYPE_TSCIPHER, start_slot);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    }
    for (start_slot = 0; start_slot < HI_KEYSLOT_MCIPHER_MAX ; start_slot++) {
        ret = _ks_proc_unlock_slot(HI_KEYSLOT_TYPE_MCIPHER, start_slot);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    }
    for (start_slot = HI_KEYSLOT_MCIPHER_MAX; start_slot < HI_KEYSLOT_MCIPHER_MAX + HI_KEYSLOT_HMAC_MAX; start_slot++) {
        ret = _ks_proc_unlock_slot(HI_KEYSLOT_TYPE_HMAC, start_slot);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    }
    return HI_SUCCESS;
}

static hi_s32 _proc_help(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (argv == HI_NULL) {
        return HI_ERR_KS_PTR_NULL;
    }

    _ks_proc_get_help();

    return HI_SUCCESS;
}

static hi_s32 _proc_unlock(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;

    if (argv == HI_NULL) {
        return HI_ERR_KS_PTR_NULL;
    }

    if (argc < 0x3) { /* 0x3: unlock module slot */
        return HI_ERR_KS_INVALID_PARAM;
    }

    ret = _ks_proc_unlock(argc, argv);
    if (ret != HI_SUCCESS) {
        _ks_proc_get_help();
    }

    return ret;
}

static hi_s32 _proc_lock(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;

    if (argv == HI_NULL) {
        return HI_ERR_KS_PTR_NULL;
    }

    if (argc < 0x3) { /* 0x3: unlock module slot */
        return HI_ERR_KS_INVALID_PARAM;
    }

    ret = _ks_proc_lock(argc, argv);
    if (ret != HI_SUCCESS) {
        _ks_proc_get_help();
    }

    return ret;
}

static hi_s32 _proc_clean(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;

    if (argv == HI_NULL) {
        return HI_ERR_KS_PTR_NULL;
    }

    if (argc < 0x1) { /* 0x1: clean */
        return HI_ERR_KS_INVALID_PARAM;
    }

    ret = _ks_proc_clean(argc, argv);
    if (ret != HI_SUCCESS) {
        _ks_proc_get_help();
    }

    return ret;
}

typedef enum {
    KS_PROC_BEGIN = 0,
    KS_PROC_PRINT,
    KS_PROC_DROP,
} ks_proc_stat;

static hi_s32 _ks_proc_read(hi_void *seqfile, hi_void *private)
{
    hi_u32 i;
    ks_slot_stat stat;
    ks_proc_stat dump_stat = KS_PROC_BEGIN;

    if (seqfile == HI_NULL) {
        return HI_ERR_KS_PTR_NULL;
    }

    hi_drv_proc_echo_helper("|----LOCK STAT  0:UNLOCK 1:REE 2:TEE 3:HPP---------|\n");
    hi_drv_proc_echo_helper("|--------------------HMAC(%03d)(all slot)-----------|\n", HI_KEYSLOT_HMAC_MAX);
    for (i = 0; i < HI_KEYSLOT_HMAC_MAX; i++) {
        stat = hal_ks_status(HI_KEYSLOT_TYPE_HMAC, i + HI_KEYSLOT_MCIPHER_MAX);
        hi_drv_proc_echo_helper("|index=%03d          cpu_lock:%d\n", i + HI_KEYSLOT_MCIPHER_MAX, stat);
    }

    hi_drv_proc_echo_helper("|--------------------MCIPHER(%03d)(all slot)--------|\n", HI_KEYSLOT_MCIPHER_MAX);
    for (i = 0; i < HI_KEYSLOT_MCIPHER_MAX; i++) {
        stat = hal_ks_status(HI_KEYSLOT_TYPE_MCIPHER, i);
        if ((stat == 0) && (dump_stat == KS_PROC_BEGIN)) {
            dump_stat = KS_PROC_DROP;
            hi_drv_proc_echo_helper("|        ...\n", i, stat);
            continue;
        }
        if ((stat == 0) && (dump_stat == KS_PROC_DROP)) {
            continue;
        }
        dump_stat = KS_PROC_BEGIN;
        hi_drv_proc_echo_helper("|index=%03d          cpu_lock:%d\n", i, stat);
    }
    hi_drv_proc_echo_helper("|--------------------TSCIPHER(%03d)(locked slot)----|\n", HI_KEYSLOT_TSCIPHER_MAX);
    dump_stat = KS_PROC_BEGIN;
    for (i = 0; i < HI_KEYSLOT_TSCIPHER_MAX; i++) {
        stat = hal_ks_status(HI_KEYSLOT_TYPE_TSCIPHER, i);
        if ((stat == 0) && (dump_stat == KS_PROC_BEGIN)) {
            dump_stat = KS_PROC_DROP;
            hi_drv_proc_echo_helper("|        ...\n", i, stat);
            continue;
        }
        if ((stat == 0) && (dump_stat == KS_PROC_DROP)) {
            continue;
        }
        dump_stat = KS_PROC_BEGIN;
        hi_drv_proc_echo_helper("|index=%03d          cpu_lock:%d\n", i, stat);
    }
    hi_drv_proc_echo_helper("|--------------------END---------------------------|\n");
    return HI_SUCCESS;
}

static osal_proc_cmd g_proc_cmd[] = {
    { "help",   _proc_help },
    { "unlock", _proc_unlock },
    { "lock",   _proc_lock },
    { "clean",  _proc_clean },
};

hi_s32 ks_register_proc(hi_void)
{
    osal_proc_entry *item;

    item = osal_proc_add(HI_MOD_KEYSLOT_NAME, sizeof(HI_MOD_KEYSLOT_NAME));
    if (item == NULL) {
        print_err_func(osal_proc_add, HI_FAILURE);
        return HI_FAILURE;
    }

    item->read = _ks_proc_read;
    item->cmd_list = g_proc_cmd;
    item->cmd_cnt = sizeof(g_proc_cmd) / sizeof(osal_proc_cmd);

    return HI_SUCCESS;
}

hi_void ks_remove_proc(hi_void)
{
    osal_proc_remove(HI_MOD_KEYSLOT_NAME, strlen(HI_MOD_KEYSLOT_NAME));
    return;
}

