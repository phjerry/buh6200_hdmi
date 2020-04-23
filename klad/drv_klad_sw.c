/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:keyladder session node list manage.
 * Author: Hisilicon hisecurity team
 * Create: 2019-6-25
 */
#include "drv_klad_sw.h"

#include <linux/delay.h>
#include <linux/interrupt.h>
#include "linux/hisilicon/securec.h"
#include "hi_drv_klad.h"
#include "hi_drv_mem.h"
#include "drv_ioctl_klad.h"
#include "drv_klad_sw_func.h"
#include "drv_klad_sw_utils.h"
#include "drv_klad_hw.h"
#include "drv_rkp.h"

typedef struct {
    hi_u32 cmd;
    hi_s32(*f_driver_cmd_process)(struct file *file, hi_void *arg, hi_u32 len);
} fmw_klad_ioctl_map;

hi_s32 hi_drv_klad_create(hi_handle *create_handle)
{
    hi_s32 ret;

    ret = klad_slot_mgmt_create_slot(create_handle);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_slot_mgmt_create_slot, ret);
    }
    return ret;
}

hi_s32 hi_drv_klad_destroy(hi_handle handle)
{
    hi_s32 ret;

    ret = klad_slot_mgmt_destroy_slot(handle);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_slot_mgmt_destroy_slot, ret);
    }
    return ret;
}

static hi_s32 __sw_klad_set_attr(hi_handle handle, hi_klad_attr *attr)
{
    hi_s32 ret;

    ret = klad_slot_mgmt_instance_create(handle, attr->klad_cfg.klad_type);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_slot_mgmt_instance_create, ret);
        goto out0;
    }
    ret = klad_slot_instance_set_attr(handle, attr);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_slot_instance_set_attr, ret);
        goto out1;
    }
    return HI_SUCCESS;
out1:
    ret = klad_slot_mgmt_instance_destroy(handle);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_slot_mgmt_clr_destroy, ret);
    }
out0:
    return ret;
}

static hi_s32 __sw_klad_get_attr(hi_handle handle, hi_klad_attr *attr)
{
    return klad_slot_instance_get_attr(handle, attr);
}

hi_s32 hi_drv_klad_set_attr(hi_handle handle, hi_klad_attr *attr)
{
    return __sw_klad_set_attr(handle, attr);
}

hi_s32 hi_drv_klad_get_attr(hi_handle handle, hi_klad_attr *attr)
{
    return __sw_klad_get_attr(handle, attr);
}

hi_s32 hi_drv_klad_attach(hi_handle handle, hi_handle target)
{
    return klad_slot_instance_attach(handle, target);
}

hi_s32 hi_drv_klad_detach(hi_handle handle, hi_handle target)
{
    return klad_slot_instance_detach(handle, target);
}

hi_s32 hi_drv_klad_set_session_key(hi_handle handle, hi_klad_session_key *session_key)
{
    return klad_slot_instance_set_session_key(handle, session_key);
}

hi_s32 hi_drv_klad_set_content_key(hi_handle handle, hi_klad_content_key *content_key)
{
    hi_s32 ret;

    ret = klad_slot_instance_set_content_key(handle, content_key);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_slot_instance_set_content_key, ret);
        goto out;
    }
    ret = klad_slot_instance_start(handle);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_slot_instance_start, ret);
    }
out:
    return ret;
}

hi_s32 hi_drv_klad_set_clear_key(hi_handle handle, hi_klad_clear_key *clear_key)
{
    return klad_slot_clr_set_key(handle, clear_key);
}

hi_s32 hi_drv_klad_clear_cw(const klad_clear_cw_param *clear_cw)
{
    return HI_SUCCESS;
}

hi_s32 hi_drv_klad_clear_iv(const klad_clear_iv_param *clr_iv)
{
    if (clr_iv == HI_NULL) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }
    if (is_invalid_ks_handle(clr_iv->ks_handle)) {
        print_err_code(HI_ERR_KLAD_INVALID_PARAM);
        return HI_ERR_KLAD_INVALID_PARAM;
    }
    return hi_drv_hw_clr_iv_process(clr_iv);
}

hi_s32 fmw_klad_release(struct file *filp)
{
    return HI_SUCCESS;
}

static hi_s32 __klad_gen_hkl_attr(hi_klad_attr *attr, struct klad_r_base_attr *base_attr)
{
    base_attr->klad_type = attr->klad_cfg.klad_type;
    base_attr->vendor_id = attr->klad_cfg.owner_id;
    base_attr->is_secure_key = HI_FALSE;
    return HI_SUCCESS;
}

static struct klad_r_base_attr g_module_id_list[] = {
#include KLAD_MODULE_ID_BASIC
};

/*
* If module_id list not provide rootkey slot information. should get from OTP fuse.
*/
static hi_s32 __klad_rk_slot_find(hi_rootkey_select *root_slot)
{
    return rkp_cas_slot_find(root_slot);
}

static hi_s32 __klad_generate_modid_rkslot(struct klad_r_base_attr *list, struct klad_r_base_attr *base_attr)
{
    hi_s32 ret;

    base_attr->module_id[0x0] = list->module_id[0x0];
    base_attr->module_id[0x1] = list->module_id[0x1];
    base_attr->module_id[0x2] = list->module_id[0x2];
    base_attr->module_id[0x3] = list->module_id[0x3];
    base_attr->unique         = list->unique;
    if (list->root_slot >= HI_ROOTKEY_SLOT_MAX) {
        ret = __klad_rk_slot_find(&base_attr->root_slot);
    } else {
        base_attr->root_slot      = list->root_slot;
        ret = HI_SUCCESS;
    }
    print_dbg_hex4(base_attr->module_id[0x0], base_attr->module_id[0x1],
        base_attr->module_id[0x2], base_attr->root_slot);
    return ret;
}

/*
* Keyladder Module id and rootkey slot generation.
*
* Module ID is 128 bits string.
* It is set to 4 32bits registers of logic.
* module_id[3] = bit[0~31]
* module_id[2] = bit[32~63]
* module_id[1] = bit[64~95]
* module_id[0] = bit[96~127]
*/
static hi_s32 __klad_com_gen_moduleid_rkslot(hi_klad_attr *attr, struct klad_r_base_attr *base_attr)
{
    hi_u32 index;

    HI_DBG_KLAD("get moduleid from global list.\n");
    print_dbg_hex2(attr->klad_cfg.klad_type, attr->klad_cfg.owner_id);

    for (index = 0; index < sizeof(g_module_id_list) / sizeof(struct klad_r_base_attr); index++) {
        if (attr->klad_cfg.klad_type == g_module_id_list[index].klad_type &&
            attr->klad_cfg.owner_id == g_module_id_list[index].vendor_id) {
            print_dbg_hex(index);
            return __klad_generate_modid_rkslot(&g_module_id_list[index], base_attr);
        }
    }
    for (index = 0; index < sizeof(g_module_id_list) / sizeof(struct klad_r_base_attr); index++) {
        if (attr->klad_cfg.klad_type == g_module_id_list[index].klad_type &&
            g_module_id_list[index].vendor_id == 0) {
            print_dbg_hex(index);
            return __klad_generate_modid_rkslot(&g_module_id_list[index], base_attr);
        }
    }
    print_err_hex3(attr->klad_cfg.klad_type, attr->klad_cfg.owner_id, HI_ERR_KLAD_NOT_FIND_MODID);
    return HI_ERR_KLAD_NOT_FIND_MODID;
}

#ifdef  HI_TEE_SUPPORT
static hi_void __klad_dym_ree_moduleid(hi_rootkey_attr *rk_attr,
                                       rkp_module_id_0 *module_id0,
                                       rkp_module_id_1 *module_id1,
                                       rkp_module_id_2 *module_id2,
                                       rkp_module_id_3 *module_id3)
{
    module_id0->ree.bits.ree_decrypt = rk_attr->target_feature_support.decrypt_support;
    module_id0->ree.bits.ree_encrypt = rk_attr->target_feature_support.encrypt_support;
    module_id0->ree.bits.ree_content_key_mcipher = rk_attr->target_support.mcipher_support;
    module_id0->ree.bits.ree_content_key_tscipher = rk_attr->target_support.tscipher_support;
    module_id0->ree.bits.ree_destination_sm4 = rk_attr->target_alg_support.sm4_support;
    module_id0->ree.bits.ree_destination_tdes = rk_attr->target_alg_support.tdes_support;
    module_id0->ree.bits.ree_destination_aes = rk_attr->target_alg_support.aes_support;
    module_id0->ree.bits.ree_destination_csa3 = rk_attr->target_alg_support.csa3_support;
    module_id0->ree.bits.ree_destination_csa2 = rk_attr->target_alg_support.csa2_support;
    module_id0->ree.bits.ree_destination_multi2 = 0;
    module_id0->ree.bits.ree_destination_sm3_hmac = rk_attr->target_alg_support.hmac_sm3_support;
    module_id0->ree.bits.ree_destination_sha2_hmac = rk_attr->target_alg_support.hmac_sha_support;
    module_id0->ree.bits.ree_level_up = 0;
    module_id0->ree.bits.ree_stage = rk_attr->level;

    module_id1->ree.bits.ree_klad_aes = rk_attr->alg_support.aes_support;
    module_id1->ree.bits.ree_klad_tdes = rk_attr->alg_support.tdes_support;
    module_id1->ree.bits.ree_klad_sm4 = rk_attr->alg_support.sm4_support;
    module_id1->ree.bits.ree_klad_hkl = 1;
    module_id1->ree.bits.ree_no_restriction = 0;

    module_id2->ree.bits.ree_remap = 0;
    module_id2->ree.bits.ree_flash_prot_en = 0;
    module_id2->ree.bits.ree_allowed_nonce = 0;
    module_id2->ree.bits.ree_c2_checksum_en = 0;
    module_id2->ree.bits.ree_cm_checksum_en = 0;
    module_id2->ree.bits.ree_hdcp_rk = 0;
}
#else
static hi_void __klad_dym_tee_moduleid(hi_rootkey_attr *rk_attr,
                                       rkp_module_id_0 *module_id0,
                                       rkp_module_id_1 *module_id1,
                                       rkp_module_id_2 *module_id2,
                                       rkp_module_id_3 *module_id3)
{
    module_id0->tee.bits.tee_decrypt = rk_attr->target_feature_support.decrypt_support;
    module_id0->tee.bits.tee_encrypt = rk_attr->target_feature_support.encrypt_support;
    module_id0->tee.bits.tee_content_key_mcipher = rk_attr->target_support.mcipher_support;
    module_id0->tee.bits.tee_content_key_tscipher = rk_attr->target_support.tscipher_support;
    module_id0->tee.bits.tee_destination_sm4 = rk_attr->target_alg_support.sm4_support;
    module_id0->tee.bits.tee_destination_tdes = rk_attr->target_alg_support.tdes_support;
    module_id0->tee.bits.tee_destination_aes = rk_attr->target_alg_support.aes_support;
    module_id0->tee.bits.tee_destination_csa3 = rk_attr->target_alg_support.csa3_support;
    module_id0->tee.bits.tee_destination_csa2 = rk_attr->target_alg_support.csa2_support;
    module_id0->tee.bits.tee_destination_multi2 = 0;
    module_id0->tee.bits.tee_destination_sm3_hmac = rk_attr->target_alg_support.hmac_sm3_support;
    module_id0->tee.bits.tee_destination_sha2_hmac = rk_attr->target_alg_support.hmac_sha_support;
    module_id0->tee.bits.tee_level_up = 0;
    module_id0->tee.bits.tee_stage = rk_attr->level;

    module_id1->tee.bits.tee_klad_aes = rk_attr->alg_support.aes_support;
    module_id1->tee.bits.tee_klad_tdes = rk_attr->alg_support.tdes_support;
    module_id1->tee.bits.tee_klad_sm4 = rk_attr->alg_support.sm4_support;
    module_id1->tee.bits.tee_klad_hkl = 1;
    module_id1->tee.bits.tee_no_restriction = 0;

    module_id2->tee.bits.tee_tee_only = 0;
    module_id2->tee.bits.tee_tee_tpp_hpp_access = 0;
    module_id2->tee.bits.tee_remap = 0;
    module_id2->tee.bits.tee_flash_prot_en = 0;
    module_id2->tee.bits.tee_allowed_nonce = 0;
    module_id2->tee.bits.tee_c2_checksum_en = 0;
    module_id2->tee.bits.tee_cm_checksum_en = 0;
    module_id2->tee.bits.tee_hdcp_rk = 0;

    module_id3->u32 = 0;
}
#endif

/* module id come from caller's rootkey attribute */
static hi_s32 __klad_dym_gen_moduleid(hi_rootkey_attr *rk_attr, struct klad_r_base_attr *base_attr)
{
    rkp_module_id_0 module_id0;
    rkp_module_id_1 module_id1;
    rkp_module_id_2 module_id2;
    rkp_module_id_3 module_id3;

    module_id0.u32 = 0;
    module_id1.u32 = 0;
    module_id2.u32 = 0;
    module_id3.u32 = 0;

#ifndef HI_TEE_SUPPORT
    __klad_dym_tee_moduleid(rk_attr, &module_id0, &module_id1, &module_id2, &module_id3);
#else
    __klad_dym_ree_moduleid(rk_attr, &module_id0, &module_id1, &module_id2, &module_id3);
#endif

    base_attr->module_id[0x0] = module_id0.u32;
    base_attr->module_id[0x1] = module_id1.u32;
    base_attr->module_id[0x2] = module_id2.u32;
    base_attr->module_id[0x3] = module_id3.u32;

    return HI_SUCCESS;
}

static hi_s32 __klad_gen_rkp_attr(hi_rootkey_attr *rk_attr, hi_klad_attr *attr, struct klad_r_base_attr *base_attr)
{
    if (attr->klad_cfg.klad_type == HI_KLAD_TYPE_DYNAMIC) {
        base_attr->root_slot = rk_attr->rootkey_sel;
        return __klad_dym_gen_moduleid(rk_attr, base_attr);
    } else {
        return __klad_com_gen_moduleid_rkslot(attr, base_attr);
    }
}

static hi_s32 __klad_com_param_check_session(hi_klad_com_entry *entry, struct klad_r_base_attr *base_attr)
{
    hi_u32 ret = HI_SUCCESS;
    hi_u32 level_index;
    rkp_module_id_0 module_id0;
    hi_klad_level klad_level;

    module_id0.u32 = base_attr->module_id[0];
#ifndef HI_TEE_SUPPORT
    klad_level =  module_id0.tee.bits.tee_stage;
#else
    klad_level =  module_id0.ree.bits.ree_stage;
#endif

    if (klad_level < HI_KLAD_LEVEL2) {
        return HI_SUCCESS;
    }

    /*
    * if first stage session key is not set, but second stage sesssin key seted,
    * the second stage session key will droped, because this is a illegal operation.
    */
    for (level_index = 0; level_index < HI_KLAD_LEVEL_MAX; level_index++) {
        if (entry->session_cnt[level_index] == 0) {
            break;
        }
    }

    /* At this time, level_index is equal to the keyladder stage. */
    if (level_index != klad_level) {
        print_err_hex2(level_index, klad_level);
        return HI_ERR_KLAD_INVALID_LEVEL;
    }

    return ret;
}

static hi_s32 __klad_com_param_check(hi_klad_com_entry *entry, struct klad_r_base_attr *base_attr)
{
    if (entry->target_cnt == 0) {
        return HI_FAILURE;
    }
    if (entry->attr_cnt == 0) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }

    if (__klad_com_param_check_session(entry, base_attr) != HI_SUCCESS) {
        return HI_ERR_KLAD_INVALID_LEVEL;
    }

    /*
    * keyladder stage in module id is not same as session key~, check failed.
    * NOTE:fp keyladder, 3 stage in module id.
    */
    return HI_SUCCESS;
}

hi_s32 fmw_klad_com_startup(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_klad_com_entry *entry = HI_NULL;
    struct klad_r_base_attr key = {0};

    if (arg == HI_NULL || private_data == HI_NULL) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }

    entry = (hi_klad_com_entry *)(arg);

    ret = __klad_gen_rkp_attr(&entry->rk_attr, &entry->attr, &key);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_gen_rkp_attr, ret);
        goto out;
    }

    ret = __klad_com_param_check(entry, &key);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_com_param_check, ret);
        goto out;
    }

    dump_hkl_com(entry);

    ret = hi_drv_hw_com_klad_startup(entry->hw_handle, entry);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_hw_com_klad_startup, ret);
    }
out:
    return ret;
}

static hi_s32 __klad_ta_param_check(hi_klad_ta_entry *entry)
{
    if (entry->target_cnt == 0) {
        return HI_FAILURE;
    }
    if (entry->attr_cnt == 0) {
        return HI_FAILURE;
    }
    if (entry->session_ta_cnt == 0) {
        return HI_FAILURE;
    }
    if (entry->trans_cnt == 0) {
        return HI_FAILURE;
    }
    if (entry->content_ta_cnt == 0) {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 fmw_klad_ta_startup(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_klad_ta_entry *entry = HI_NULL;

    if (arg == HI_NULL || private_data == HI_NULL) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }

    entry = (hi_klad_ta_entry *)(arg);

    ret = __klad_ta_param_check(entry);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_ta_param_check, ret);
        goto out;
    }

    ret = hi_drv_hw_ta_klad_startup(entry->hw_handle, entry);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_hw_ta_klad_startup, ret);
    }
out:
    return ret;
}

static hi_s32 __klad_fp_param_check(hi_klad_fp_entry *entry)
{
    hi_u32 i;

    if (entry->target_cnt == 0) {
        return HI_FAILURE;
    }
    if (entry->attr_cnt == 0) {
        return HI_ERR_KLAD_INVALID_PARAM;
    }
    for (i = 0; i < HI_KLAD_LEVEL_MAX; i++) {
        /*
        * if first stage session key is not set, but second stage sesssin key seted,
        * the second stage session key will droped, because this is a illegal operation.
        */
        if (entry->session_cnt[i] == 0) {
            break;
        }
    }
    /*
    * keyladder stage in module id is not same as session key~, check failed.
    * NOTE:fp keyladder, 3 stage in module id.
    */
    return HI_SUCCESS;
}

hi_s32 fmw_klad_fp_startup(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_klad_fp_entry *entry = HI_NULL;

    if (arg == HI_NULL || private_data == HI_NULL) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }

    entry = (hi_klad_fp_entry *)(arg);

    ret = __klad_fp_param_check(entry);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_fp_param_check, ret);
        goto out;
    }

    ret = hi_drv_hw_fp_klad_startup(entry->hw_handle, entry);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_hw_fp_klad_startup, ret);
    }

out:
    return ret;
}

static hi_s32 __klad_nonce_param_check(hi_klad_nonce_entry *entry)
{
    if (entry->target_cnt == 0) {
        return HI_FAILURE;
    }
    if (entry->attr_cnt == 0) {
        return HI_FAILURE;
    }
    if (entry->session_cnt[0] == 0) {
        return HI_FAILURE;
    }
    if (entry->nonce_cnt == 0) {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 fmw_klad_nonce_startup(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_klad_nonce_entry *entry = HI_NULL;

    if (arg == HI_NULL || private_data == HI_NULL) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }
    entry = (hi_klad_nonce_entry *)(arg);

    ret = __klad_nonce_param_check(entry);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_nonce_param_check, ret);
        goto out;
    }

    ret = hi_drv_hw_nonce_klad_startup(entry->hw_handle, entry);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_hw_nonce_klad_startup, ret);
    }
out:
    return ret;
}

static hi_s32 __klad_clr_param_check(hi_klad_clr_entry *entry)
{
    if (entry->target_cnt == 0) {
        return HI_FAILURE;
    }
    if (entry->attr_cnt == 0) {
        return HI_FAILURE;
    }
    if (entry->clr_cnt == 0) {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 fmw_klad_clr_process(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_klad_clr_entry *entry = HI_NULL;
    struct klad_entry_key key = {0};

    if (arg == HI_NULL || private_data == HI_NULL) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }

    key.session = *(struct klad_hw_session **)private_data;
    entry = (hi_klad_clr_entry *)(arg);

    ret = __klad_clr_param_check(entry);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_nonce_param_check, ret);
        goto out;
    }

    ret = __klad_gen_hkl_attr(&entry->attr, &key.hkl_base_attr);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_gen_hkl_attr, ret);
        goto out;
    }

    dump_hkl_attr(&key.hkl_base_attr);
    dump_hkl_clr_route(entry);

    ret = hi_drv_hw_clr_route_process(&key, entry);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_hw_clr_route_process, ret);
    }
out:
    return ret;
}

hi_s32 fmw_klad_com_create(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_klad_create_attr *hkl_attr = HI_NULL;
    struct klad_entry_key key = {0};

    if ((arg == HI_NULL) || (private_data == HI_NULL)) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }

    key.session = *(struct klad_hw_session **)private_data;
    hkl_attr = (hi_klad_create_attr *)(arg);

    ret = __klad_gen_rkp_attr(&hkl_attr->rk_attr, &hkl_attr->attr, &key.hkl_base_attr);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_gen_rkp_attr, ret);
        goto out;
    }

    ret = __klad_gen_hkl_attr(&hkl_attr->attr, &key.hkl_base_attr);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_gen_hkl_attr, ret);
        goto out;
    }

    dump_hkl_attr(&key.hkl_base_attr);
    dump_hkl_create_attr(hkl_attr);

    ret = hi_drv_hw_com_klad_create(&hkl_attr->hw_handle, &key);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_hw_com_klad_create, ret);
    }
out:
    return ret;
}

hi_s32 fmw_klad_ta_create(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_klad_create_attr *hkl_attr = HI_NULL;
    struct klad_entry_key key = {0};

    if ((arg == HI_NULL) || (private_data == HI_NULL)) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }
    key.session = *(struct klad_hw_session **)private_data;
    hkl_attr = (hi_klad_create_attr *)(arg);

    ret = __klad_com_gen_moduleid_rkslot(&hkl_attr->attr, &key.hkl_base_attr);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_com_gen_moduleid_rkslot, ret);
        goto out;
    }

    ret = __klad_gen_hkl_attr(&hkl_attr->attr, &key.hkl_base_attr);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_gen_hkl_attr, ret);
        goto out;
    }

    ret = hi_drv_hw_ta_klad_create(&hkl_attr->hw_handle, &key);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_hw_ta_klad_create, ret);
    }
out:
    return ret;
}

hi_s32 fmw_klad_fp_create(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_klad_create_attr *hkl_attr = HI_NULL;
    struct klad_entry_key key = {0};

    if ((arg == HI_NULL) || (private_data == HI_NULL)) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }
    key.session = *(struct klad_hw_session **)private_data;
    hkl_attr = (hi_klad_create_attr *)(arg);

    ret = __klad_com_gen_moduleid_rkslot(&hkl_attr->attr, &key.hkl_base_attr);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_com_gen_moduleid_rkslot, ret);
        goto out;
    }

    ret = __klad_gen_hkl_attr(&hkl_attr->attr, &key.hkl_base_attr);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_gen_hkl_attr, ret);
        goto out;
    }

    ret = hi_drv_hw_fp_klad_create(&hkl_attr->hw_handle, &key);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_hw_fp_klad_create, ret);
    }

out:
    return ret;
}

hi_s32 fmw_klad_nonce_create(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_klad_create_attr *hkl_attr = HI_NULL;
    struct klad_entry_key key = {0};

    if ((arg == HI_NULL) || (private_data == HI_NULL)) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }
    key.session = *(struct klad_hw_session **)private_data;
    hkl_attr = (hi_klad_create_attr *)(arg);

    ret = __klad_com_gen_moduleid_rkslot(&hkl_attr->attr, &key.hkl_base_attr);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_com_gen_moduleid_rkslot, ret);
        goto out;
    }

    ret = __klad_gen_hkl_attr(&hkl_attr->attr, &key.hkl_base_attr);
    if (ret != HI_SUCCESS) {
        print_err_func(__klad_gen_hkl_attr, ret);
        goto out;
    }

    ret = hi_drv_hw_nonce_klad_create(&hkl_attr->hw_handle, &key);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_hw_nonce_klad_create, ret);
    }
out:
    return ret;
}

hi_s32 fmw_klad_com_destroy(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    if (arg == HI_NULL || private_data == HI_NULL) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }
    return hi_drv_hw_com_klad_destroy(*(hi_handle *)(arg));
}

hi_s32 fmw_klad_ta_destroy(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    if (arg == HI_NULL || private_data == HI_NULL) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }
    return hi_drv_hw_ta_klad_destroy(*(hi_handle *)(arg));
}

hi_s32 fmw_klad_fp_destroy(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    if (arg == HI_NULL || private_data == HI_NULL) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }

    return hi_drv_hw_fp_klad_destroy(*(hi_handle *)(arg));
}

hi_s32 fmw_klad_nonce_destroy(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    if (arg == HI_NULL || private_data == HI_NULL) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }
    return hi_drv_hw_nonce_klad_destroy(*(hi_handle *)(arg));
}

