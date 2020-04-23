/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:Define public macros for klad drivers.
 * Author: Linux SDK team
 * Create: 2019/06/22
 */
#ifndef __DRV_KLAD_HW_DEFINE_H__
#define __DRV_KLAD_HW_DEFINE_H__

#include "drv_klad_com.h"
#include "hi_drv_module.h"
#include "drv_klad_hw.h"
#include "drv_klad_hw_config.h"
#include "drv_rkp.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* end of #ifdef __cplusplus */

struct klad_r_base;
struct klad_r_com_hkl;
struct klad_r_ta_hkl;
struct klad_r_nonce_hkl;
struct klad_r_fp_hkl;
struct klad_r_clr_route;
struct klad_mgmt;

/*
 * Resource base definition.
 */
struct klad_r_base_ops {
    hi_s32(*get)(struct klad_r_base *obj);
    hi_void(*put)(struct klad_r_base *obj);
};

struct klad_hw_session;

/*
 * handles associated with the session.
 */
enum  klad_hw_session_state {
    KLAD_HW_SESSION_INACTIVED = 0x0,  /* support remove slot only. */
    KLAD_HW_SESSION_ACTIVED,          /* support add or remove slot. */
};

struct klad_hw_session_ops {
    hi_s32(*add_slot)(struct klad_hw_session *session, struct klad_r_base *slot);
    hi_s32(*del_slot)(struct klad_hw_session *session, struct klad_r_base *slot);
    hi_void(*release)(struct klad_hw_session *session);  /* release all R obj from session. */
    hi_s32(*suspend)(struct klad_hw_session *session);
    hi_s32(*resume)(struct klad_hw_session *session);
};

struct klad_hw_session {
    atomic_t                     ref_count;
    osal_mutex                   lock;
    enum  klad_hw_session_state  state;
    struct list_head             head;

    struct klad_hw_session_ops   *ops;
};

enum klad_timetamp_tag {
    TIMETAMP_HW_I       = 0x0,

    TIMETAMP_CREATE_I,  /* 1 */
    TIMETAMP_CREATE_O,

    TIMETAMP_OPTN_I,    /* 3 */
    TIMETAMP_OPTN_O,

    TIMETAMP_RKP_I,     /* 5 */
    TIMETAMP_RKP_O,

    TIMETAMP_START_I,   /* 7 */
    TIMETAMP_START_O,

    TIMETAMP_CLOSE_I,   /* 9 */
    TIMETAMP_CLOSE_O,

    TIMETAMP_DESTROY_I, /* 11 */
    TIMETAMP_DESTROY_O,

    TIMETAMP_HW_O,      /* 13 */
} ;

#ifdef HI_KLAD_PERF_SUPPORT
struct klad_timestamp {
    struct time_ns hw_in;

    struct time_ns create_in;
    struct time_ns create_out;

    struct time_ns open_in;
    struct time_ns open_out;

    struct time_ns rkp_in;
    struct time_ns rkp_out;

    struct time_ns start_in;
    struct time_ns start_out;

    struct time_ns close_in;
    struct time_ns close_out;

    struct time_ns destroy_in;
    struct time_ns destroy_out;

    struct time_ns hw_out;
};
#endif

/* The enum is come from RKP register RKP_KLAD_CHOOSE_xxx.xxx_klad_sel */
typedef enum  {
    KLAD_ID_COM0                 = 0x0,
    KLAD_ID_COM1                 = 0x1,
    KLAD_ID_COM2                 = 0x2,
    KLAD_ID_COM3                 = 0x3,
    KLAD_ID_COM4                 = 0x4,
    KLAD_ID_COM5                 = 0x5,
    KLAD_ID_COM6                 = 0x6,
    KLAD_ID_COM7                 = 0x7,
    KLAD_ID_TA                   = 0x8, /* TA is fixed 8. */
    KLAD_ID_MAX,
} klad_hw_id;

struct klad_r_base {
    atomic_t                ref_count;
    struct klad_r_base_ops  *ops;
    struct klad_hw_session  *hw_session;
    hi_s32(*release)(struct klad_r_base *obj);

    /* instance */
    hi_u32                  id; /* index in mgmt->com_hkl_bitmap */
    hi_handle               handle; /* come from id */

    /*
    * logic resource. locked com or TA keyladder index.
    * Note: RKP only send key to com keyaldder or TA keyladder. bacause FP keyladder is come from
    * COM + FP keyladder. NONCE keyladder come from COM + NONCE keyladder.
    * Clear Keyladder need not hw_id, logic send to clear keyladder automatically.
    */
    klad_hw_id              hw_id;
    hi_bool                 is_locked;

    struct klad_mgmt        *mgmt;
#ifdef HI_KLAD_PERF_SUPPORT
    struct klad_timestamp timestamp;
#endif
};

hi_void timestamp_tag(struct klad_r_base *obj, enum klad_timetamp_tag step);
hi_void timestamp_add(struct time_ns *src, struct klad_r_base *obj, enum klad_timetamp_tag step);

/*
 * common hisilicon keyladder definition.
 */
#define TO_COM_HKL_HW_ID(id)      (id)

#define is_com_hkl(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct klad_r_base *base = (struct klad_r_base*)(obj); \
    struct klad_r_com_hkl *com_hkl = container_of(base, struct klad_r_com_hkl, base); \
    WARN_ON(get_klad_r_base_ops() != base->ops); \
    if (&g_com_klad_ops == com_hkl->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

struct klad_r_com_hkl_ops {
    hi_s32(*open)(struct klad_r_com_hkl *com_hkl);
    hi_s32(*rootkey_process)(struct klad_r_com_hkl *com_hkl);
    hi_s32(*start)(struct klad_r_com_hkl *com_hkl, const hi_klad_com_entry *instance);
    hi_s32(*close)(struct klad_r_com_hkl *com_hkl);

    hi_s32(*suspend)(struct klad_r_com_hkl *com_hkl);
    hi_s32(*resume)(struct klad_r_com_hkl *com_hkl);
};

struct klad_r_com_hkl {
    struct klad_r_base         base;   /* !!! it must be first entry. */
    struct klad_r_com_hkl_ops  *ops;
    osal_mutex                 lock;
    struct list_head           node;

    hi_bool                    rkp_ready;

    struct klad_r_base         *associate_obj; /* Associate other keyladder */

    struct klad_r_base_attr    base_attr;
};

/*
 * ta keyladder definition.
 */
#define TO_TA_HKL_HW_ID(id)      (id)

#define is_ta_hkl(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct klad_r_base *base = (struct klad_r_base*)(obj); \
    struct klad_r_ta_hkl *ta_hkl = container_of(base, struct klad_r_ta_hkl, base); \
    WARN_ON(get_klad_r_base_ops() != base->ops); \
    if (&g_ta_klad_ops == ta_hkl->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

struct klad_r_ta_hkl_ops {
    hi_s32(*open)(struct klad_r_ta_hkl *ta_hkl);
    hi_s32(*rootkey_process)(struct klad_r_ta_hkl *com_hkl);
    hi_s32(*start)(struct klad_r_ta_hkl *com_hkl, const hi_klad_ta_entry *instance);
    hi_s32(*close)(struct klad_r_ta_hkl *ta_hkl);

    hi_s32(*suspend)(struct klad_r_ta_hkl *ta_hkl);
    hi_s32(*resume)(struct klad_r_ta_hkl *ta_hkl);
};

struct klad_r_ta_hkl {
    struct klad_r_base         base;   /* !!! it must be first entry. */
    struct klad_r_ta_hkl_ops   *ops;
    osal_mutex                 lock;
    struct list_head           node;

    hi_bool                    rkp_ready;
    struct klad_r_base         *com_obj; /* Associate ta keyladder to a common keyladder */

    struct klad_r_base_attr    base_attr;
};

/*
 * flash protection keyladder definition.
 */
#define TO_FP_HKL_HW_ID(id)      (id)

#define is_fp_hkl(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct klad_r_base *base = (struct klad_r_base*)(obj); \
    struct klad_r_fp_hkl *fp_hkl = container_of(base, struct klad_r_fp_hkl, base); \
    WARN_ON(get_klad_r_base_ops() != base->ops); \
    if (&g_fp_klad_ops == fp_hkl->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

struct klad_r_fp_hkl_ops {
    hi_s32(*open)(struct klad_r_fp_hkl *fp_hkl);
    hi_s32(*rootkey_process)(struct klad_r_fp_hkl *fp_hkl);
    hi_s32(*start)(struct klad_r_fp_hkl *fp_hkl, const hi_klad_fp_entry *instance);
    hi_s32(*close)(struct klad_r_fp_hkl *fp_hkl);

    hi_s32(*suspend)(struct klad_r_fp_hkl *fp_hkl);
    hi_s32(*resume)(struct klad_r_fp_hkl *fp_hkl);
};

struct klad_r_fp_hkl {
    struct klad_r_base         base;   /* !!! it must be first entry. */
    struct klad_r_fp_hkl_ops   *ops;
    osal_mutex                 lock;
    struct list_head           node;

    hi_bool                    rkp_ready;
    struct klad_r_base         *com_obj; /* Associate ta keyladder to a common keyladder */

    struct klad_r_base_attr    base_attr;
};

/*
 * nonce keyladder definition.
 */
#define TO_NONCE_HKL_HW_ID(id)      (id)

#define is_nonce_hkl(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct klad_r_base *base = (struct klad_r_base*)(obj); \
    struct klad_r_nonce_hkl *nonce_hkl = container_of(base, struct klad_r_nonce_hkl, base); \
    WARN_ON(get_klad_r_base_ops() != base->ops); \
    if (&g_nonce_klad_ops == nonce_hkl->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

struct klad_r_nonce_hkl_ops {
    hi_s32(*open)(struct klad_r_nonce_hkl *nonce_hkl);
    hi_s32(*rootkey_process)(struct klad_r_nonce_hkl *nonce_hkl);
    hi_s32(*start)(struct klad_r_nonce_hkl *nonce_hkl, const hi_klad_nonce_entry *instance);
    hi_s32(*close)(struct klad_r_nonce_hkl *nonce_hkl);

    hi_s32(*suspend)(struct klad_r_nonce_hkl *nonce_hkl);
    hi_s32(*resume)(struct klad_r_nonce_hkl *nonce_hkl);
};

struct klad_r_nonce_hkl {
    struct klad_r_base          base;   /* !!! it must be first entry. */
    struct klad_r_nonce_hkl_ops *ops;
    osal_mutex                  lock;
    struct list_head            node;

    hi_bool                     rkp_ready;
    struct klad_r_base          *com_obj; /* Associate ta keyladder to a common keyladder */

    struct klad_r_base_attr     base_attr;
};

/*
 * clear route definition.
 */
#define TO_CLR_ROUTE_HW_ID(id)      (id)

#define is_clr_route(obj) ({ \
    hi_bool ret_ = HI_FALSE; \
    struct klad_r_base *base = (struct klad_r_base*)(obj); \
    struct klad_r_clr_route *clr_route = container_of(base, struct klad_r_clr_route, base); \
    WARN_ON(get_klad_r_base_ops() != base->ops); \
    if (&g_clr_route_ops == clr_route->ops) \
        ret_ = HI_TRUE; \
    ret_; \
})

struct klad_r_clr_route_ops {
    hi_s32(*open)(struct klad_r_clr_route *clr_route);
    hi_s32(*start)(struct klad_r_clr_route *clr_route, const hi_klad_clr_entry *instance);
    hi_s32(*close)(struct klad_r_clr_route *clr_route);

    hi_s32(*suspend)(struct klad_r_clr_route *clr_route);
    hi_s32(*resume)(struct klad_r_clr_route *clr_route);
};

struct klad_r_clr_route {
    struct klad_r_base          base;   /* !!! it must be first entry. */
    struct klad_r_clr_route_ops *ops;
    osal_mutex                  lock;
    struct list_head            node;

    hi_u32                      klad_type;
};

/*
 * global klad resource managation.
 */
enum  klad_mgmt_state {
    KLAD_MGMT_CLOSED = 0x0,
    KLAD_MGMT_OPENED,
};

enum  klad_state {
    KLAD_CLOSED = 0x0,
    KLAD_OPENED,
};

struct klad_mgmt_ops {
    hi_s32(*init)(struct klad_mgmt *mgmt);
    hi_s32(*exit)(struct klad_mgmt *mgmt);

    hi_s32(*suspend)(struct klad_mgmt *mgmt);
    hi_s32(*resume)(struct klad_mgmt *mgmt);

    hi_s32(*create_com_hkl)(struct klad_mgmt *mgmt, struct klad_entry_key *hkl_attr,
                            struct klad_r_com_hkl **com_hkl);
    hi_s32(*destroy_com_hkl)(struct klad_mgmt *mgmt, struct klad_r_com_hkl *com_hkl);

    hi_s32(*create_ta_hkl)(struct klad_mgmt *mgmt, struct klad_entry_key *hkl_attr,
                           struct klad_r_ta_hkl **ta_hkl);
    hi_s32(*destroy_ta_hkl)(struct klad_mgmt *mgmt, struct klad_r_ta_hkl *ta_hkl);

    hi_s32(*create_fp_hkl)(struct klad_mgmt *mgmt, struct klad_entry_key *hkl_attr,
                           struct klad_r_fp_hkl **fp_hkl);
    hi_s32(*destroy_fp_hkl)(struct klad_mgmt *mgmt, struct klad_r_fp_hkl *fp_hkl);

    hi_s32(*create_nonce_hkl)(struct klad_mgmt *mgmt, struct klad_entry_key *hkl_attr,
                              struct klad_r_nonce_hkl **nonce_hkl);
    hi_s32(*destroy_nonce_hkl)(struct klad_mgmt *mgmt, struct klad_r_nonce_hkl *nonce_hkl);

    hi_s32(*create_clr_route)(struct klad_mgmt *mgmt, struct klad_entry_key *hkl_attr,
                              struct klad_r_clr_route **clr_route);
    hi_s32(*destroy_clr_route)(struct klad_mgmt *mgmt, struct klad_r_clr_route *clr_route);

    /* debug helper */
    hi_void(*show_info)(struct klad_mgmt *mgmt);
};

struct klad_mgmt {
    osal_mutex              lock;
    enum  klad_mgmt_state   state;
    atomic_t                ref_count;
    struct klad_mgmt_ops    *ops;

    hi_void                 *io_base;

    osal_mutex              com_hkl_lock;
    osal_spinlock           com_hkl_lock2;
    hi_u32                  com_hkl_cnt;
    struct list_head        com_hkl_head;
    struct klad_r_com_hkl   com_hkl[KLAD_COM_CNT];
    DECLARE_BITMAP(com_hkl_bitmap, KLAD_COM_CNT);

    enum  klad_state        ta_hkl_state;
    struct klad_r_ta_hkl    ta_hkl;

    enum  klad_state        fp_hkl_state;
    struct klad_r_fp_hkl    fp_hkl;

    enum  klad_state        nonce_hkl_state;
    struct klad_r_nonce_hkl nonce_hkl;

    enum  klad_state        clr_route_state;
    struct klad_r_clr_route clr_route;

    struct task_struct     *monitor; /* */
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_KLAD_HW_DEFINE_H__
