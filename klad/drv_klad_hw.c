/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:
 * Author : Linux SDK team
 * Create: 2019/06/22
 */

#include "drv_klad_hw.h"

#include <linux/mm.h>
#include "linux/hisilicon/securec.h"

#include "drv_ioctl_klad.h"
#ifdef HI_PROC_SUPPORT
#include "drv_klad_proc.h"
#endif
#include "drv_klad_hw_func.h"
#include "hal_klad.h"
#include "drv_hkl.h"
#include "drv_rkp.h"
#include "drv_klad_hw_utils.h"
#include "hi_drv_log.h"

hi_void timestamp(struct time_ns *time)
{
#ifdef HI_KLAD_PERF_SUPPORT
    struct timespec ts;

    getnstimeofday(&ts);
    time->tv_sec = ts.tv_sec;
    time->tv_nsec = ts.tv_nsec;

#endif
}

hi_void get_cost(hi_char *str, struct time_ns *time_b, struct time_ns *time_e)
{
#ifdef HI_KLAD_PERF_SUPPORT
    if (time_b->tv_sec ==  time_e->tv_sec) {
        HI_WARN_KLAD("%06ld.%09ld s-->%06ld.%09ld s, cost:%03ld.%06ld ms <<%s\n",
                     time_b->tv_sec, time_b->tv_nsec, time_e->tv_sec, time_e->tv_nsec,
                     (time_e->tv_nsec - time_b->tv_nsec) / TIME_MS2NS,
                     (time_e->tv_nsec - time_b->tv_nsec) % TIME_MS2NS,
                     str);
    } else {
        HI_WARN_KLAD("%06ld.%09ld s-->%06ld.%09ld s, cost:%03ld.%06ld ms <<%s\n",
                     time_b->tv_sec, time_b->tv_nsec, time_e->tv_sec, time_e->tv_nsec,
                     ((time_e->tv_sec - time_b->tv_sec) * TIME_S2NS + time_e->tv_nsec - time_b->tv_nsec) / TIME_MS2NS,
                     ((time_e->tv_sec - time_b->tv_sec) * TIME_S2NS + time_e->tv_nsec - time_b->tv_nsec) % TIME_MS2NS,
                     str);
    }
#endif
}

hi_void get_curr_cost(hi_char *str, struct time_ns *time_b)
{
#ifdef HI_KLAD_PERF_SUPPORT
    struct time_ns time_e;

    timestamp(&time_e);
    get_cost(str, time_b, &time_e);
#endif
}

hi_s32 hi_drv_hw_com_klad_process(struct klad_entry_key *attrs, hi_klad_com_entry *entry)
{
    hi_s32 ret;
    struct klad_r_com_hkl *obj = HI_NULL;
    struct time_ns time_b = {0};

    timestamp(&time_b);
    ret = klad_hw_com_hkl_open(attrs, &obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_com_hkl_open, ret);
        goto out;
    }
    ret = klad_hw_com_hkl_start(obj, entry);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_com_hkl_start, ret);
        goto out1;
    }
out1:
    if (klad_hw_com_hkl_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw com hkl instance failed.\n");
    }
    timestamp_tag(&obj->base, TIMETAMP_HW_O);
    timestamp_add(&time_b, &obj->base, TIMETAMP_HW_I);
out:
    return ret;
}

hi_s32 hi_drv_hw_fp_klad_process(struct klad_entry_key *attrs, hi_klad_fp_entry *entry)
{
    hi_s32 ret;
    struct klad_r_fp_hkl *obj = HI_NULL;
    struct time_ns time_b;

    timestamp(&time_b);
    ret = klad_hw_fp_hkl_open(attrs, &obj);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    ret = klad_hw_fp_hkl_start(obj, entry);
    if (ret != HI_SUCCESS) {
        goto out1;
    }
out1:
    if (klad_hw_fp_hkl_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw com hkl instance failed.\n");
    }
    timestamp_tag(&obj->base, TIMETAMP_HW_O);
    timestamp_add(&time_b, &obj->base, TIMETAMP_HW_I);
out:
    return ret;
}

hi_s32 hi_drv_hw_ta_klad_process(struct klad_entry_key *attrs, hi_klad_ta_entry *entry)
{
    hi_s32 ret;
    struct klad_r_ta_hkl *obj = HI_NULL;
    struct time_ns time_b;

    timestamp(&time_b);
    ret = klad_hw_ta_hkl_open(attrs, &obj);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    ret = klad_hw_ta_hkl_start(obj, entry);
    if (ret != HI_SUCCESS) {
        goto out1;
    }
out1:
    if (klad_hw_ta_hkl_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw ta hkl instance failed.\n");
    }
    timestamp_tag(&obj->base, TIMETAMP_HW_O);
    timestamp_add(&time_b, &obj->base, TIMETAMP_HW_I);
out:
    return ret;
}

hi_s32 hi_drv_hw_nonce_klad_process(struct klad_entry_key *attrs, hi_klad_nonce_entry *entry)
{
    hi_s32 ret;
    struct klad_r_nonce_hkl *obj = HI_NULL;
    struct time_ns time_b;

    timestamp(&time_b);
    ret = klad_hw_nonce_hkl_open(attrs, &obj);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    ret = klad_hw_nonce_hkl_start(obj, entry);
    if (ret != HI_SUCCESS) {
        goto out1;
    }
out1:
    if (klad_hw_nonce_hkl_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw nonce hkl instance failed.\n");
    }
    timestamp_tag(&obj->base, TIMETAMP_HW_O);
    timestamp_add(&time_b, &obj->base, TIMETAMP_HW_I);
out:
    return ret;
}

hi_s32 hi_drv_hw_clr_route_process(struct klad_entry_key *attrs, hi_klad_clr_entry *entry)
{
    hi_s32 ret;
    struct klad_r_clr_route *obj = HI_NULL;
    struct time_ns time_b;

    timestamp(&time_b);
    ret = klad_hw_clr_route_open(attrs, &obj);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    ret = klad_hw_clr_route_start(obj, entry);
    if (ret != HI_SUCCESS) {
        goto out1;
    }

out1:
    if (klad_hw_clr_route_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw clear route instance failed.\n");
    }
    timestamp_tag(&obj->base, TIMETAMP_HW_O);
    timestamp_add(&time_b, &obj->base, TIMETAMP_HW_I);
out:
    return ret;
}

hi_s32 hi_drv_hw_clr_iv_process(const klad_clear_iv_param *clr_iv)
{
    clear_iv hkl_iv;
    hi_u32 i;

    hkl_iv.key_slot_num = HANDLE_2_ID(clr_iv->ks_handle);
    hkl_iv.is_odd = clr_iv->is_odd;

    if (is_tscipher_ks_handle(clr_iv->ks_handle)) {
        hkl_iv.port_sel = PORT_SEL_TSCIPHER;
    } else if (is_mcipher_ks_handle(clr_iv->ks_handle)) {
        hkl_iv.port_sel = PORT_SEL_MCIPHER;
    } else {
        print_err_hex2(clr_iv->ks_handle, HI_ERR_KLAD_INVALID_HANDLE);
        return HI_ERR_KLAD_INVALID_HANDLE;
    }
    /* IV needs reverse order */
    for (i = 0; i < HKL_KEY_LEN; i++) {
        hkl_iv.iv[i] = clr_iv->iv[HKL_KEY_LEN - i - 1];
    }
    return hi_klad_clriv_process(&hkl_iv);
}

hi_s32 hi_drv_hw_com_klad_create(hi_handle *handle, struct klad_entry_key *attrs)
{
    hi_s32 ret;
    struct klad_r_com_hkl *obj = HI_NULL;
    struct time_ns time_b;

    if (handle == HI_NULL || attrs == HI_NULL) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }

    timestamp(&time_b);

    ret = klad_hw_com_hkl_open(attrs, &obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_com_hkl_open, ret);
        goto out;
    }

    ret = klad_hw_session_add_slot(attrs->session, (struct klad_r_base *)obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_session_add_slot, ret);
        goto out1;
    }

    *handle = obj->base.handle;
    timestamp_add(&time_b, &obj->base, TIMETAMP_HW_I);

    return HI_SUCCESS;
out1:
    if (klad_hw_com_hkl_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw com hkl instance failed.\n");
    }
    timestamp_add(&time_b, &obj->base, TIMETAMP_HW_I);
out:
    return ret;
}

hi_s32 hi_drv_hw_com_klad_startup(hi_handle handle, hi_klad_com_entry *entry)
{
    hi_s32 ret;
    struct klad_r_com_hkl *obj = HI_NULL;

    ret = klad_hw_com_hkl_find(handle, &obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_com_hkl_find, ret);
        goto out;
    }

    ret = klad_hw_com_hkl_start(obj, entry);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_com_hkl_start, ret);
    }

out:
    return ret;
}

hi_s32 hi_drv_hw_com_klad_destroy(hi_handle handle)
{
    hi_s32 ret;
    struct klad_r_com_hkl *obj = HI_NULL;

    ret = klad_hw_com_hkl_find(handle, &obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_com_hkl_find, ret);
        goto out;
    }

    if (klad_hw_session_del_slot(((struct klad_r_base *)obj)->hw_session, (struct klad_r_base *)obj) != HI_SUCCESS) {
        print_err_func(klad_hw_session_del_slot, ret);
    }

    if (klad_hw_com_hkl_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw com hkl instance failed.\n");
    }

out:
    timestamp_tag(&obj->base, TIMETAMP_HW_O);
    return ret;
}

hi_s32 hi_drv_hw_fp_klad_create(hi_handle *handle, struct klad_entry_key *attrs)
{
    hi_s32 ret;
    struct klad_r_fp_hkl *obj = HI_NULL;
    struct time_ns time_b;

    if (handle == HI_NULL || attrs == HI_NULL) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }

    timestamp(&time_b);

    ret = klad_hw_fp_hkl_open(attrs, &obj);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    *handle = obj->base.handle;

    ret = klad_hw_session_add_slot(attrs->session, (struct klad_r_base *)obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_session_add_slot, ret);
        goto out1;
    }

    timestamp_add(&time_b, &obj->base, TIMETAMP_HW_I);

    return HI_SUCCESS;
out1:
    if (klad_hw_fp_hkl_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw fp hkl instance failed.\n");
    }
    timestamp_add(&time_b, &obj->base, TIMETAMP_HW_I);
out:
    return ret;
}

hi_s32 hi_drv_hw_fp_klad_startup(hi_handle handle, hi_klad_fp_entry *entry)
{
    hi_s32 ret;
    struct klad_r_fp_hkl *obj = HI_NULL;

    ret = klad_hw_fp_hkl_find(handle, &obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_fp_hkl_find, ret);
        goto out;
    }

    ret = klad_hw_fp_hkl_start(obj, entry);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_fp_hkl_start, ret);
    }
out:
    return ret;
}

hi_s32 hi_drv_hw_fp_klad_destroy(hi_handle handle)
{
    hi_s32 ret;
    struct klad_r_fp_hkl *obj = HI_NULL;

    ret = klad_hw_fp_hkl_find(handle, &obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_fp_hkl_find, ret);
        goto out;
    }
    if (klad_hw_fp_hkl_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw fp hkl instance failed.\n");
    }
    if (klad_hw_session_del_slot(((struct klad_r_base *)obj)->hw_session, (struct klad_r_base *)obj) != HI_SUCCESS) {
        print_err_func(klad_hw_session_add_slot, ret);
    }
out:
    timestamp_tag(&obj->base, TIMETAMP_HW_O);
    return ret;
}


hi_s32 hi_drv_hw_ta_klad_create(hi_handle *handle, struct klad_entry_key *attrs)
{
    hi_s32 ret;
    struct klad_r_ta_hkl *obj = HI_NULL;
    struct time_ns time_b;

    if (handle == HI_NULL || attrs == HI_NULL) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }

    timestamp(&time_b);
    ret = klad_hw_ta_hkl_open(attrs, &obj);
    if (ret != HI_SUCCESS) {
        goto out;
    }
    *handle = obj->base.handle;

    ret = klad_hw_session_add_slot(attrs->session, (struct klad_r_base *)obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_session_add_slot, ret);
        goto out1;
    }

    timestamp_add(&time_b, &obj->base, TIMETAMP_HW_I);
    return HI_SUCCESS;
out1:
    if (klad_hw_ta_hkl_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw ta hkl instance failed.\n");
    }
    timestamp_add(&time_b, &obj->base, TIMETAMP_HW_I);
out:
    return ret;
}

hi_s32 hi_drv_hw_ta_klad_startup(hi_handle handle, hi_klad_ta_entry *entry)
{
    hi_s32 ret;
    struct klad_r_ta_hkl *obj = HI_NULL;

    ret = klad_hw_ta_hkl_find(handle, &obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_ta_hkl_find, ret);
        goto out;
    }

    ret = klad_hw_ta_hkl_start(obj, entry);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_ta_hkl_start, ret);
    }

out:
    return ret;
}

hi_s32 hi_drv_hw_ta_klad_destroy(hi_handle handle)
{
    hi_s32 ret;
    struct klad_r_ta_hkl *obj = HI_NULL;
    struct time_ns time_b;

    timestamp(&time_b);
    ret = klad_hw_ta_hkl_find(handle, &obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_ta_hkl_find, ret);
        goto out;
    }
    if (klad_hw_ta_hkl_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw ta hkl instance failed.\n");
    }
    if (klad_hw_session_del_slot(((struct klad_r_base *)obj)->hw_session, (struct klad_r_base *)obj) != HI_SUCCESS) {
        print_err_func(klad_hw_session_add_slot, ret);
    }
out:
    timestamp_tag(&obj->base, TIMETAMP_HW_O);
    timestamp_add(&time_b, &obj->base, TIMETAMP_HW_I);
    return ret;
}


hi_s32 hi_drv_hw_nonce_klad_create(hi_handle *handle, struct klad_entry_key *attrs)
{
    hi_s32 ret;
    struct klad_r_nonce_hkl *obj = HI_NULL;
    struct time_ns time_b;

    if (handle == HI_NULL || attrs == HI_NULL) {
        print_err_code(HI_ERR_KLAD_NULL_PTR);
        return HI_ERR_KLAD_NULL_PTR;
    }

    timestamp(&time_b);
    ret = klad_hw_nonce_hkl_open(attrs, &obj);
    if (ret != HI_SUCCESS) {
        goto out;
    }

    *handle = obj->base.handle;

    ret = klad_hw_session_add_slot(attrs->session, (struct klad_r_base *)obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_session_add_slot, ret);
        goto out1;
    }

    timestamp_add(&time_b, &obj->base, TIMETAMP_HW_I);
    return HI_SUCCESS;
out1:
    if (klad_hw_nonce_hkl_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw nonce hkl instance failed.\n");
    }
    timestamp_add(&time_b, &obj->base, TIMETAMP_HW_I);
out:
    return ret;
}

hi_s32 hi_drv_hw_nonce_klad_startup(hi_handle handle, hi_klad_nonce_entry *entry)
{
    hi_s32 ret;
    struct klad_r_nonce_hkl *obj = HI_NULL;

    ret = klad_hw_nonce_hkl_find(handle, &obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_nonce_hkl_find, ret);
        goto out;
    }

    ret = klad_hw_nonce_hkl_start(obj, entry);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_nonce_hkl_start, ret);
    }
out:
    return ret;
}

hi_s32 hi_drv_hw_nonce_klad_destroy(hi_handle handle)
{
    hi_s32 ret;
    struct klad_r_nonce_hkl *obj = HI_NULL;

    ret = klad_hw_nonce_hkl_find(handle, &obj);
    if (ret != HI_SUCCESS) {
        print_err_func(klad_hw_nonce_hkl_find, ret);
        goto out;
    }
    if (klad_hw_nonce_hkl_close(obj) != HI_SUCCESS) {
        HI_FATAL_KLAD("close hw nonce hkl instance failed.\n");
    }
    if (klad_hw_session_del_slot(((struct klad_r_base *)obj)->hw_session, (struct klad_r_base *)obj) != HI_SUCCESS) {
        print_err_func(klad_hw_session_add_slot, ret);
    }
out:
    timestamp_tag(&obj->base, TIMETAMP_HW_O);
    return ret;
}

hi_s32 hi_drv_hw_klad_init(hi_void)
{
    return HI_SUCCESS;
}

hi_void hi_drv_hw_klad_exit(hi_void)
{
    return;
}

static hi_bool is_debug_level_valid(hi_void)
{
    hi_log_level log_level = HI_LOG_LEVEL_DBG;
    hi_s32 ret;

    ret = hi_drv_log_get_level(HI_ID_KLAD, &log_level);
    if (ret != HI_SUCCESS) {
        print_err_func(hi_drv_log_get_level, ret);
    }

    if (log_level < HI_LOG_LEVEL_DBG) {
        return HI_FALSE;
    }

    return HI_TRUE;
}

hi_void hi_klad_dump_hex(const hi_char *str, const hi_u8 *buf, hi_u32 len)
{
    const hi_u32 lwidth = 0x200; /* the length can be updated. */
    hi_s8 str_buf[lwidth];
    hi_s8 *p = str_buf;
    hi_s8 *q = str_buf + lwidth;
    hi_u32 i;

    if (!is_debug_level_valid()) {
        return;
    }

    p = str_buf;
    if (snprintf_s(p, q - p, q - p - 1, "%s :", str) < 0) {
        return;
    }
    p += strlen(str) + 0x2; /* table + ':', add 2 bytes. */

    if (buf == NULL) {
        if (snprintf_s(p, q - p, q - p - 1, "*NULL*\n") < 0) {
            return;
        }
    } else if (len < (lwidth / 0x2)) {
        for (i = 0; i < len; i++, p += 0x2) {
            if (snprintf_s(p, q - p, q - p - 1, "%02x", buf[i]) < 0) {
                return;
            }
        }
        *p = '\n';
        p++;
        *p = 0; /* end string with null char */
    } else {
        for (i = 0; i < (lwidth / 0x4) - 0x1; i++, p += 0x2) {
            if (snprintf_s(p, q - p, q - p - 1, "%02x", buf[i]) < 0) {
                return;
            }
        }
        if (snprintf_s(p, q - p, q - p - 1, " ... ") < 0) {
            return;
        }
        p += 0x5;
        for (i = len - (lwidth / 0x4) + 0x1; i < len; i++, p += 0x2) {
            if (snprintf_s(p, q - p, q - p - 1, "%02x", buf[i]) < 0) {
                return;
            }
        }
        *p = '\n';
        p++;
        *p = 0; /* end string with null char */
    }
    HI_DBG_KLAD("%s", str_buf);
    return;
}

hi_void dump_hkl_attr(struct klad_r_base_attr *attrs)
{
    print_dbg_hex(attrs->root_slot);
    print_dbg_hex(attrs->unique);
    print_dbg_hex(attrs->is_secure_key);
    print_dbg_hex(attrs->klad_type);
    print_dbg_hex(attrs->vendor_id);
    print_dbg_hex(attrs->module_id[0]);
    print_dbg_hex(attrs->module_id[0x1]);
    print_dbg_hex(attrs->module_id[0x2]);
    print_dbg_hex(attrs->module_id[0x3]);
}

hi_void dump_hkl_clr_route(const hi_klad_clr_entry *entry)
{
    print_dbg_hex(entry->target_handle);
    hi_klad_dump_hex("entry->attr", (hi_u8 *)&entry->attr, sizeof(hi_klad_attr));
    hi_klad_dump_hex("entry->clr_key", (hi_u8 *)&entry->clr_key, sizeof(hi_klad_clear_key));
}

hi_void dump_hkl_com(const hi_klad_com_entry *entry)
{
    hi_u32 i;

    print_dbg_hex(entry->target_handle);
    hi_klad_dump_hex("entry->rootkey", (hi_u8 *)&entry->rk_attr, sizeof(hi_rootkey_attr));
    hi_klad_dump_hex("entry->attr", (hi_u8 *)&entry->attr, sizeof(hi_klad_attr));
    for (i = 0; i < HI_KLAD_LEVEL_MAX; i++) {
        hi_klad_dump_hex("entry->session_key", (hi_u8 *)&entry->session_key[i], sizeof(hi_klad_session_key));
    }
    hi_klad_dump_hex("entry->content_key", (hi_u8 *)&entry->content_key, sizeof(hi_klad_content_key));
}


hi_void dump_hkl_create_attr(const hi_klad_create_attr *attr)
{
    hi_klad_dump_hex("entry->rootkey", (hi_u8 *)&attr->rk_attr, sizeof(hi_rootkey_attr));
    hi_klad_dump_hex("entry->attr", (hi_u8 *)&attr->attr, sizeof(hi_klad_attr));
}