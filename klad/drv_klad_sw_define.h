/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:Define public macros for klad drivers.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#ifndef __DRV_KLAD_SW_DEFINE_H__
#define __DRV_KLAD_SW_DEFINE_H__

#include "drv_klad_com.h"
#include "hi_drv_klad.h"
#include "hi_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

#define KLAD_SW_SLOT_MAX_CNT 256 /* Support 256 instance at the same time, It is up to 0xffff */

struct klad_sw_session;
struct klad_slot;
struct klad_sw_r_base;
struct com_klad_slot;
struct ta_klad_slot;
struct nonce_klad_slot;
struct fp_klad_slot;
struct clr_route_slot;
struct klad_slot_mgmt;

/*
 * general definition.
 */
struct obj_node_helper {
    hi_void             *key;
    struct list_head    node;
};

/*
 * handles associated with the session.
 */
enum  klad_sw_session_state {
    KLAD_SW_SESSION_INACTIVED = 0x0,  /* support remove slot only. */
    KLAD_SW_SESSION_ACTIVED,          /* support add or remove slot. */
};

struct klad_sw_session_ops {
    hi_s32(*add_slot)(struct klad_sw_session *session, struct klad_slot *slot);
    hi_s32(*del_slot)(struct klad_sw_session *session, struct klad_slot *slot);
    hi_void(*release)(struct klad_sw_session *session);  /* release all R obj from session. */
    hi_s32(*suspend)(struct klad_sw_session *session);
    hi_s32(*resume)(struct klad_sw_session *session);
};

struct klad_sw_session {
    atomic_t                     ref_count;
    osal_mutex                   lock;
    enum  klad_sw_session_state  state;
    struct list_head             head;

    struct klad_sw_session_ops   *ops;
};

/*
 * slot definition.
 */
struct klad_slot {
    osal_mutex              lock;
    struct klad_sw_r_base   *obj;  /* Point to the slot instance. */
    struct klad_sw_session  *sw_session; /* Point to the session head. */
    struct list_head        node; /* list node. add into sesssion list. */

    hi_handle handle;
    hi_void(*release)(hi_handle handle);
};

struct klad_slot_table {
    osal_mutex       lock;
    hi_u32           slot_cnt;
    DECLARE_BITMAP(slot_bitmap, KLAD_SW_SLOT_MAX_CNT);
    struct klad_slot table[KLAD_SW_SLOT_MAX_CNT];
};

/*
 * Resource base definition.
 */
struct klad_sw_r_base_ops {
    hi_s32(*get)(struct klad_sw_r_base *obj);
    hi_void(*put)(struct klad_sw_r_base *obj);
};

struct klad_sw_r_base {
    atomic_t                  ref_count;
    struct klad_sw_r_base_ops *ops;
    hi_s32(*release)(struct klad_sw_r_base *obj);

    struct klad_slot_mgmt     *mgmt;
    hi_u32                    klad_type;
    struct list_head          node;       /* SW klad slot node, add node to mgmt->xxx_head. */
    struct list_head          *free_list; /* Piont to free list(mgmt->xxx_empty_head). */
};


/*
 * software com keyladder instances definition.
 */
#define is_sw_com(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct klad_sw_r_base *base = (struct klad_sw_r_base*)(obj); \
    struct com_klad_slot *slot = container_of(base, struct com_klad_slot, base); \
    WARN_ON(get_klad_sw_r_base_ops() != base->ops); \
    if (&g_sw_com_klad_ops == slot->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

struct klad_com_ops {
    hi_s32(*set_rootkey_attr)(struct com_klad_slot *slot, hi_rootkey_attr *rootkey);
    hi_s32(*set_attr)(struct com_klad_slot *slot, hi_klad_attr *attr);
    hi_s32(*get_attr)(struct com_klad_slot *slot, hi_klad_attr *attr);
    hi_s32(*attach)(struct com_klad_slot *slot, hi_handle target);
    hi_s32(*detach)(struct com_klad_slot *slot, hi_handle target);
    hi_s32(*set_session_key)(struct com_klad_slot *slot, hi_klad_session_key *session_key);
    hi_s32(*set_content_key)(struct com_klad_slot *slot, hi_klad_content_key *content_key);
    hi_s32(*start)(struct com_klad_slot *slot);
};

struct com_klad_slot {
    struct klad_sw_r_base      base;   /* !!! it must be first entry. */
    struct klad_com_ops        *ops;
    osal_mutex                 lock;

    atomic64_t target_cnt;
    hi_handle target_handle;

    atomic64_t rk_attr_cnt;
    hi_rootkey_attr rk_attr;

    atomic64_t attr_cnt;
    hi_klad_attr attr;

    atomic64_t session_cnt[HI_KLAD_LEVEL_MAX];
    hi_klad_session_key session_key[HI_KLAD_LEVEL_MAX];

    atomic64_t content_cnt;
    hi_klad_content_key content_key;
};

/*
 * software ta keyladder instances definition.
 */
#define is_sw_ta(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct klad_sw_r_base *base = (struct klad_sw_r_base*)(obj); \
    struct ta_klad_slot *slot = container_of(base, struct ta_klad_slot, base); \
    WARN_ON(get_klad_sw_r_base_ops() != base->ops); \
    if (&g_sw_ta_klad_ops == slot->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

struct klad_ta_ops {
    hi_s32(*set_attr)(struct ta_klad_slot *slot, hi_klad_attr *attr);
    hi_s32(*get_attr)(struct ta_klad_slot *slot, hi_klad_attr *attr);
    hi_s32(*attach)(struct ta_klad_slot *slot, hi_handle target);
    hi_s32(*detach)(struct ta_klad_slot *slot, hi_handle target);
    hi_s32(*set_session_ta_key)(struct ta_klad_slot *slot, hi_klad_ta_key *ta_key);
    hi_s32(*set_trans_data)(struct ta_klad_slot *slot, hi_klad_trans_data *trans_data);
    hi_s32(*set_content_ta_key)(struct ta_klad_slot *slot, hi_klad_ta_key *ta_key);
    hi_s32(*start)(struct ta_klad_slot *slot);
};

struct ta_klad_slot {
    struct klad_sw_r_base      base;   /* !!! it must be first entry. */
    struct klad_ta_ops         *ops;
    osal_mutex                 lock;

    atomic64_t target_cnt;
    hi_handle target_handle;

    atomic64_t attr_cnt;
    hi_klad_attr attr;

    atomic64_t session_ta_cnt;
    hi_klad_ta_key session_ta_key;

    atomic64_t trans_cnt;
    hi_klad_trans_data trans_data;

    atomic64_t content_ta_cnt;
    hi_klad_ta_key content_ta_key;
};

/*
 * software nonce keyladder instances definition.
 */
#define is_sw_nonce(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct klad_sw_r_base *base = (struct klad_sw_r_base*)(obj); \
    struct nonce_klad_slot *slot = container_of(base, struct nonce_klad_slot, base); \
    WARN_ON(get_klad_sw_r_base_ops() != base->ops); \
    if (&g_sw_nonce_klad_ops == slot->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

struct klad_nonce_ops {
    hi_s32(*set_attr)(struct nonce_klad_slot *slot, hi_klad_attr *attr);
    hi_s32(*get_attr)(struct nonce_klad_slot *slot, hi_klad_attr *attr);
    hi_s32(*attach)(struct nonce_klad_slot *slot, hi_handle target);
    hi_s32(*detach)(struct nonce_klad_slot *slot, hi_handle target);
    hi_s32(*set_session_key)(struct nonce_klad_slot *slot, hi_klad_session_key *session_key);
    hi_s32(*set_nonce_key)(struct nonce_klad_slot *slot, hi_klad_nonce_key *nonce_key);
    hi_s32(*start)(struct nonce_klad_slot *slot);
};

struct nonce_klad_slot {
    struct klad_sw_r_base      base;   /* !!! it must be first entry. */
    struct klad_nonce_ops      *ops;
    osal_mutex                 lock;

    atomic64_t target_cnt;
    hi_handle target_handle;

    atomic64_t attr_cnt;
    hi_klad_attr attr;

    atomic64_t session_cnt[HI_KLAD_LEVEL_MAX];
    hi_klad_session_key session_key[HI_KLAD_LEVEL_MAX];

    atomic64_t nonce_cnt;
    hi_klad_nonce_key nonce_key;
};

/*
 * software flash protectino keyladder instances definition.
 */
#define is_sw_fp(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct klad_sw_r_base *base = (struct klad_sw_r_base*)(obj); \
    struct fp_klad_slot *slot = container_of(base, struct fp_klad_slot, base); \
    WARN_ON(get_klad_sw_r_base_ops() != base->ops); \
    if (&g_sw_fp_klad_ops == slot->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

struct klad_fp_ops {
    hi_s32(*set_attr)(struct fp_klad_slot *slot, hi_klad_attr *attr);
    hi_s32(*get_attr)(struct fp_klad_slot *slot, hi_klad_attr *attr);
    hi_s32(*attach)(struct fp_klad_slot *slot, hi_handle target);
    hi_s32(*detach)(struct fp_klad_slot *slot, hi_handle target);
    hi_s32(*set_session_key)(struct fp_klad_slot *slot, hi_klad_session_key *session_key);
    hi_s32(*set_fp_key)(struct fp_klad_slot *slot, hi_klad_nonce_key *fp_key);
    hi_s32(*start)(struct fp_klad_slot *slot);
};

struct fp_klad_slot {
    struct klad_sw_r_base      base;   /* !!! it must be first entry. */
    struct klad_fp_ops         *ops;
    osal_mutex                 lock;

    atomic64_t target_cnt;
    hi_handle target_handle;

    atomic64_t attr_cnt;
    hi_klad_attr attr;

    atomic64_t session_cnt[HI_KLAD_LEVEL_MAX];
    hi_klad_session_key session_key[HI_KLAD_LEVEL_MAX];

    atomic64_t fp_cnt;
    hi_klad_fp_key fp_key;
};

/*
 * software clear route keyladder instances definition.
 */
#define is_sw_clr_route(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct klad_sw_r_base *base = (struct klad_sw_r_base*)(obj); \
    struct clr_route_slot *slot = container_of(base, struct clr_route_slot, base); \
    WARN_ON(get_klad_sw_r_base_ops() != base->ops); \
    if (&g_sw_clr_route_ops == slot->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

struct klad_clr_route_ops {
    hi_s32(*set_attr)(struct clr_route_slot *slot, hi_klad_attr *attr);
    hi_s32(*get_attr)(struct clr_route_slot *slot, hi_klad_attr *attr);
    hi_s32(*attach)(struct clr_route_slot *slot, hi_handle target);
    hi_s32(*detach)(struct clr_route_slot *slot, hi_handle target);
    hi_s32(*set_clr_key)(struct clr_route_slot *slot, hi_klad_clear_key *clr_key);
    hi_s32(*start)(struct clr_route_slot *slot);
};

struct clr_route_slot {
    struct klad_sw_r_base      base;   /* !!! it must be first entry. */
    struct klad_clr_route_ops  *ops;
    osal_mutex                 lock;

    atomic64_t target_cnt;
    hi_handle target_handle;

    atomic64_t attr_cnt;
    hi_klad_attr attr;

    atomic64_t clr_cnt;
    hi_klad_clear_key clr_key;
};

struct klad_slot_instance {
    union { /* !!! it must be first entry. */
        struct com_klad_slot   com_slot;
        struct ta_klad_slot    ta_slot;
        struct nonce_klad_slot nonce_slot;
        struct fp_klad_slot    fp_slot;
        struct clr_route_slot  clr_slot;
    } obj;
    hi_klad_type               klad;
};

/*
 * global software klad slot resource managation.
 */
enum  klad_slot_mgmt_state {
    KLAD_SLOT_MGMT_CLOSED = 0x0,
    KLAD_SLOT_MGMT_OPENED,
};

struct klad_slot_mgmt_ops {
    hi_s32(*init)(struct klad_slot_mgmt *mgmt);
    hi_s32(*exit)(struct klad_slot_mgmt *mgmt);

    hi_s32(*suspend)(struct klad_slot_mgmt *mgmt);
    hi_s32(*resume)(struct klad_slot_mgmt *mgmt);

    hi_s32(*create_slot)(struct klad_slot_mgmt *mgmt, hi_handle *handle);
    hi_s32(*destroy_slot)(struct klad_slot_mgmt *mgmt, hi_handle handle);

    hi_s32(*create_com_instance)(struct klad_slot_mgmt *mgmt, hi_handle handle);
    hi_s32(*destroy_com_instance)(struct klad_slot_mgmt *mgmt, hi_handle handle);

    hi_s32(*create_ta_instance)(struct klad_slot_mgmt *mgmt, hi_handle handle);
    hi_s32(*destroy_ta_instance)(struct klad_slot_mgmt *mgmt, hi_handle handle);

    hi_s32(*create_nonce_instance)(struct klad_slot_mgmt *mgmt, hi_handle handle);
    hi_s32(*destroy_nonce_instance)(struct klad_slot_mgmt *mgmt, hi_handle handle);

    hi_s32(*create_fp_instance)(struct klad_slot_mgmt *mgmt, hi_handle handle);
    hi_s32(*destroy_fp_instance)(struct klad_slot_mgmt *mgmt, hi_handle handle);

    hi_s32(*create_clr_instance)(struct klad_slot_mgmt *mgmt, hi_handle handle);
    hi_s32(*destroy_clr_instance)(struct klad_slot_mgmt *mgmt, hi_handle handle);
};

struct klad_slot_mgmt {
    osal_mutex                 lock;
    enum  klad_slot_mgmt_state state;
    atomic_t                   ref_count;
    struct klad_slot_mgmt_ops  *ops;

    osal_mutex                 slot_lock;
    struct list_head           slot_head;
    struct list_head           slot_empty_head;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_KLAD_SW_DEFINE_H__
