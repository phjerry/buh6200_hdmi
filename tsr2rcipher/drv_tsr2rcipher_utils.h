/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: tsr2rcipher utils export functions.
 */

#ifndef __DRV_TSR2RCIPHER_UTILS_H__
#define __DRV_TSR2RCIPHER_UTILS_H__

#include "hi_type.h"

#include "drv_tsr2rcipher_config.h"
#include "hi_drv_module.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TSR2RCIPHER_UNUSED(x) ((x) = (x))

/* log output and control functions. */
#define HI_FATAL_TSR2RCIPHER(fmt...) HI_FATAL_PRINT(HI_ID_TSR2RCIPHER, fmt)
#define HI_ERR_TSR2RCIPHER(fmt...)   HI_ERR_PRINT(HI_ID_TSR2RCIPHER, fmt)
#define HI_WARN_TSR2RCIPHER(fmt...)  HI_WARN_PRINT(HI_ID_TSR2RCIPHER, fmt)
#define HI_INFO_TSR2RCIPHER(fmt...)  HI_INFO_PRINT(HI_ID_TSR2RCIPHER, fmt)
#define HI_DBG_TSR2RCIPHER(fmt...)   HI_DBG_PRINT(HI_ID_TSR2RCIPHER, fmt)

/* obj resource debug info */
#ifdef HI_LOG_D_FUNCTRACE
#define HI_LOG_FUNC_ENTER() HI_DBG_TSR2RCIPHER(" =====>[enter]\n")
#define HI_LOG_FUNC_EXIT()  HI_DBG_TSR2RCIPHER(" =====>[exit]\n")
#else
#define HI_LOG_FUNC_ENTER()
#define HI_LOG_FUNC_EXIT()
#endif

#define TSR2RCIPHER_DBG_ENTER() HI_LOG_FUNC_ENTER()
#define TSR2RCIPHER_DBG_EXIT()  HI_LOG_FUNC_EXIT()

#define TSR2RCIPHER_CHECK_POINTER_RETURN(p) do { \
    if ((p) == HI_NULL) {                        \
        HI_ERR_TSR2RCIPHER("null pointer!\n");   \
        return HI_ERR_TSR2RCIPHER_NULL_PTR;      \
    }                               \
} while (0)

#define TSR2RCIPHER_CHECK_POINTER_GOTO_OUT(p) do { \
    if ((p) == HI_NULL) {                          \
        HI_ERR_TSR2RCIPHER("null pointer!\n");     \
        ret =  HI_ERR_TSR2RCIPHER_NULL_PTR;        \
        goto out;                                  \
    }                                              \
} while (0)

#define TSR2RCIPHER_CHECK_POINTER_BREAK(p) do { \
    if ((p) == HI_NULL) {                       \
        HI_ERR_TSR2RCIPHER("null pointer!\n");  \
        break;                                  \
    }                                           \
} while (0)

#define TSR2RCIPHER_CHECK_CONDITION_RETURN_VOID(condition) do { \
    if (condition) {                                            \
        HI_FATAL_TSR2RCIPHER("fatal error!\n");                 \
        return;                                                 \
    }                                                           \
} while (0)

#define TSR2RCIPHER_CHECK_CONDITION_RETURN_VALUE(condition, value) do { \
    if (condition) {                                                    \
        HI_FATAL_TSR2RCIPHER("fatal error!\n");                         \
        return value;                                                   \
    }                                                                   \
} while (0)

#define tsc_dofunc_no_return(func) do {                                   \
    hi_s32 ret_ = (func);                                                 \
    if (ret_ != HI_SUCCESS) {                                             \
        HI_ERR_TSR2RCIPHER("call %s failed, ret = 0x%x\n", # func, ret_); \
    }                                                                     \
} while (0)

/* HANDLE macro */
#define TSR2RCIPHER_HANDLE_MAGIC (0xB)
#define ID_2_HANDLE(id)          ((HI_ID_TSR2RCIPHER << 24) | (TSR2RCIPHER_HANDLE_MAGIC << 16) | ((id) & 0x0000ffff))
#define HANDLE_2_ID(handle)      ((handle) & 0x0000ffff)
#define CHECK_HANDLE(handle) ({                                                                                                 \
    hi_s32 ret_ = HI_SUCCESS;                                                                                                   \
    if ((((handle) >> 24) & 0x000000ff) != HI_ID_TSR2RCIPHER || (((handle) >> 16) & 0x000000ff) != TSR2RCIPHER_HANDLE_MAGIC) { \
        ret_ = HI_ERR_TSR2RCIPHER_INVALID_PARA;                                                                                 \
    }                                                                                                                           \
    ret_;                                                                                                                       \
})
#define CHECK_KS(handle) ({                                  \
    hi_s32 ret_ = HI_SUCCESS;                                \
    if ((((handle) >> 24) & 0x000000ff) != HI_ID_KEYSLOT) { \
        ret_ = HI_ERR_TSR2RCIPHER_INVALID_PARA;              \
    }                                                        \
    ret_;                                                    \
})

/* general link list obj helper. */
struct obj_node_helper {
    hi_void         *key;
    struct list_head node;
};

/* Resource base definition. */
struct tsr2rcipher_r_base {
    osal_atomic                    ref_count;
    struct tsr2rcipher_r_base_ops *ops;
    hi_s32(*release)(struct tsr2rcipher_r_base *obj);
    hi_u32                         id;
    struct tsr2rcipher_mgmt       *mgmt;
};

struct tsr2rcipher_r_base_ops {
    hi_s32(*get)(struct tsr2rcipher_r_base *obj);
    hi_void(*put)(struct tsr2rcipher_r_base *obj);
};

enum tsr2rcipher_session_state {
    TSR2RCIPHER_SESSION_INACTIVED = 0x0, /* support remove slot only. */
    TSR2RCIPHER_SESSION_ACTIVED,         /* support add or remove slot. */
};

/* slot definition. */
struct tsr2rcipher_session {
    osal_atomic                     ref_count;
    osal_mutex                      lock;
    enum tsr2rcipher_session_state  state;
    struct list_head                head;
    struct tsr2rcipher_session_ops *ops;
};

struct tsr2rcipher_slot {
    osal_mutex                  lock;
    struct tsr2rcipher_session *session;
    struct tsr2rcipher_r_base  *obj;
    hi_handle handle;
    hi_s32(*release)(hi_handle handle); /* private: maybe changed for different handle object. */
};

struct tsr2rcipher_slot_table {
    osal_mutex       lock; /* consider rw lock */
    hi_u32           slot_cnt;
    DECLARE_BITMAP(slot_bitmap, TSR2RCIPHER_MAX_SLOT_CNT);
    struct tsr2rcipher_slot table[TSR2RCIPHER_MAX_SLOT_CNT];
};

/* session definition. */
struct tsr2rcipher_session_ops {
    hi_s32(*add_slot)(struct tsr2rcipher_session *session, struct tsr2rcipher_slot *slot);
    hi_s32(*del_slot)(struct tsr2rcipher_session *session, struct tsr2rcipher_slot *slot);
    hi_void(*release)(struct tsr2rcipher_session *session); /* release all R obj from session. */
    hi_s32(*suspend)(struct tsr2rcipher_session *session);
    hi_s32(*resume)(struct tsr2rcipher_session *session);
};

/* export functions. */
struct  tsr2rcipher_r_base_ops *get_tsr2rcipher_rbase_ops(hi_void);
hi_s32  tsr2rcipher_r_get_raw(struct tsr2rcipher_r_base *obj);
hi_s32  tsr2rcipher_r_get(hi_handle handle, struct tsr2rcipher_r_base **obj);
hi_void tsr2rcipher_r_put(struct tsr2rcipher_r_base *obj);

hi_s32 tsr2rcipher_slot_create(struct tsr2rcipher_r_base *obj, struct tsr2rcipher_slot **slot);
hi_s32 tsr2rcipher_slot_find(hi_handle handle, struct tsr2rcipher_slot **slot);
hi_s32 tsr2rcipher_slot_get_robj(hi_handle handle, struct tsr2rcipher_r_base **obj);
hi_s32 tsr2rcipher_slot_destroy(struct tsr2rcipher_slot *slot);

hi_s32 tsr2rcipher_session_create(struct tsr2rcipher_session **new_session);
hi_s32 tsr2rcipher_session_add_slot(struct tsr2rcipher_session *session, struct tsr2rcipher_slot *slot);
hi_s32 tsr2rcipher_session_del_slot(struct tsr2rcipher_session *session, struct tsr2rcipher_slot *slot);
hi_s32 tsr2rcipher_session_destroy(struct tsr2rcipher_session *session);

/* general common functions. */
hi_u32 tsr2rcipher_get_queue_lenth(const hi_u32 read, const hi_u32 write, const hi_u32 size);

hi_s32 tsr2rcipher_slot_init(hi_void);
hi_s32 tsr2rcipher_slot_exit(hi_void);

#ifdef __cplusplus
}
#endif

#endif  /* __DRV_TSR2RCIPHER_UTILS_H__ */
