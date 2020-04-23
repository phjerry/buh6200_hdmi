/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux utils export functions.
 * Author: sdk
 * Create: 2017-06-01
 */
#ifndef __DRV_DEMUX_UTILS_H__
#define __DRV_DEMUX_UTILS_H__

#include "hi_osal.h"
#include "hi_type.h"
#include "hi_errno.h"
#include "drv_demux_config.h"
#include "hi_drv_module.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*
 * HANDLE macro
 */
#define DMX_HANDLE_MAGIC     (0xB)
#define ID_2_HANDLE(id)      ((HI_ID_DEMUX << 24) | (DMX_HANDLE_MAGIC << 20) | (id & 0xFFFFF))
#define HANDLE_2_ID(handle)  ((handle) & 0xFFFFF)
#define CHECK_HANDLE(handle) ({                                                                   \
    hi_s32 ret = HI_SUCCESS;                                                                       \
    if (((handle >> 24) & 0xFF) != HI_ID_DEMUX || ((handle >> 20) & 0xF) != DMX_HANDLE_MAGIC)  { \
        ret = HI_ERR_DMX_INVALID_PARA;                                                             \
    }                                                                                              \
    ret;                                                                                           \
})

#define KS_HANDLE_2_ID(handle)  ((handle) & 0x0000FFFF)
#define CHECK_KEYSLOT_HANDLE(handle) ({               \
    hi_s32 ret = HI_SUCCESS;                          \
    if (((handle >> 24) & 0xFF) != HI_ID_KEYSLOT) { \
        ret = HI_ERR_DMX_INVALID_PARA;                \
    }                                                 \
    ret;                                              \
})

/*
 * log output and control functions.
 */
#define HI_FATAL_DEMUX(fmt...)      HI_FATAL_PRINT  (HI_ID_DEMUX, fmt)
#define HI_ERR_DEMUX(fmt...)        HI_ERR_PRINT    (HI_ID_DEMUX, fmt)
#define HI_WARN_DEMUX(fmt...)       HI_WARN_PRINT   (HI_ID_DEMUX, fmt)
#define HI_INFO_DEMUX(fmt...)       HI_INFO_PRINT   (HI_ID_DEMUX, fmt)
#define HI_DBG_DEMUX(fmt...)        HI_DBG_PRINT    (HI_ID_DEMUX, fmt)
#define HI_TRACE_DEMUX(fmt...)      HI_TRACE_PRINT  (HI_ID_DEMUX, fmt)

/* obj resource debug info */
#define hi_log_func_enter()                      HI_TRACE_DEMUX(" =====>[enter]\n")
#define hi_log_func_exit()                       HI_TRACE_DEMUX(" =====>[exit]\n")

/* control function entry by proc file system */
#define DMX_TRACE_LEVEL_DEFAULT      0

typedef enum {
    DMX_TRACE_GLB        = 0x1,
    DMX_TRACE_MGMT       = 0x2,
    DMX_TRACE_RAM_PORT   = 0x4,
    DMX_TRACE_PID        = 0x8,
    DMX_TRACE_BAND       = 0x10,
    DMX_TRACE_PLAY       = 0x20,
    DMX_TRACE_REC        = 0x40,
    DMX_TRACE_RMX        = 0x80,
    DMX_TRACE_MASK       = 0xFF,
} DMX_TRACE_LEVEL;

typedef enum {
    DMX_TRACE_CMD_SAVE_ES       = 1,
    DMX_TRACE_CMD_SAVE_ALLTS,
    DMX_TRACE_CMD_SAVE_IPTS,
    DMX_TRACE_CMD_SAVE_DMXTS,
    DMX_TRACE_CMD_ALL_TRACE,
    DMX_TRACE_CMD_GLB_TRACE,
    DMX_TRACE_CMD_MGMT_TRACE,
    DMX_TRACE_CMD_RAMPORT_TRACE,
    DMX_TRACE_CMD_PID_TRACE,
    DMX_TRACE_CMD_BAND_TRACE
} DMX_TARCE_CMD;

typedef enum {
    DMX_TRACE_CMD_STOP        = 0,
    DMX_TRACE_CMD_START       = 1,
} DMX_TRACE_CMD_CTRl;

#define DMX_GLB_TRACE_ENTER() do { \
    if ((dmx_drv_utils_get_trace_level() & DMX_TRACE_MASK) == DMX_TRACE_GLB) { \
        hi_log_func_enter(); \
    }\
} while (0)

#define DMX_GLB_TRACE_EXIT() do { \
    if ((dmx_drv_utils_get_trace_level() & DMX_TRACE_MASK) == DMX_TRACE_GLB) { \
        hi_log_func_exit(); \
    }\
} while (0)

#define DMX_MGMT_TRACE_ENTER() do { \
    if ((dmx_drv_utils_get_trace_level() & DMX_TRACE_MASK) == DMX_TRACE_MGMT) { \
        hi_log_func_enter(); \
    }\
} while (0)

#define DMX_MGMT_TRACE_EXIT() do { \
    if ((dmx_drv_utils_get_trace_level() & DMX_TRACE_MASK) == DMX_TRACE_MGMT) { \
        hi_log_func_exit(); \
    }\
} while (0)

#define DMX_RAM_TRACE_ENTER() do { \
    if ((dmx_drv_utils_get_trace_level() & DMX_TRACE_MASK) == DMX_TRACE_RAM_PORT) { \
        hi_log_func_enter(); \
    }\
} while (0)

#define DMX_RAM_TRACE_EXIT() do { \
    if ((dmx_drv_utils_get_trace_level() & DMX_TRACE_MASK) == DMX_TRACE_RAM_PORT) { \
        hi_log_func_exit(); \
    }\
} while (0)

#define DMX_PID_TRACE_ENTER() do { \
    if ((dmx_drv_utils_get_trace_level() & DMX_TRACE_MASK) == DMX_TRACE_PID) { \
        hi_log_func_enter(); \
    }\
} while (0)

#define DMX_PID_TRACE_EXIT() do { \
    if ((dmx_drv_utils_get_trace_level() & DMX_TRACE_MASK) == DMX_TRACE_PID) { \
        hi_log_func_exit(); \
    }\
} while (0)

#define DMX_BAND_TRACE_ENTER() do { \
    if ((dmx_drv_utils_get_trace_level() & DMX_TRACE_MASK) == DMX_TRACE_BAND) { \
        hi_log_func_enter(); \
    }\
} while (0)

#define DMX_BAND_TRACE_EXIT() do { \
    if ((dmx_drv_utils_get_trace_level() & DMX_TRACE_MASK) == DMX_TRACE_BAND) { \
        hi_log_func_exit(); \
    }\
} while (0)

#define DMX_INVALID_PORT_GOTO(port, out_flag) do { \
    if (unlikely((port) < DMX_IF_PORT_0 || (port > DMX_PORT_MAX))) {\
        HI_ERR_DEMUX("Invalid port[%#x]!\n", port);                  \
        ret =  HI_ERR_DMX_INVALID_PARA;                              \
        goto out_flag;                                               \
    }\
} while (0)

#define DMX_INVALID_TAG_PORT_GOTO(port, out_flag) do { \
    if (unlikely((port) < DMX_TAG_PORT_0 || (port >= DMX_TAG_PORT_MAX))) {\
        HI_ERR_DEMUX("Invalid tag port[%#x]!\n", port);                  \
        ret =  HI_ERR_DMX_INVALID_PARA;                              \
        goto out_flag;                                               \
    }\
} while (0)

#define DMX_NULL_POINTER_VOID(p) do { \
    if (unlikely((p) == HI_NULL)) {      \
        HI_ERR_DEMUX("null pointer!\n"); \
        return; \
    }\
} while (0)

#define DMX_NULL_POINTER_RETURN(p) do {  \
    if (unlikely((p) == HI_NULL)) {      \
        HI_ERR_DEMUX("null pointer!\n"); \
        return HI_ERR_DMX_NULL_PTR; \
    }\
} while (0)

#define DMX_NULL_POINTER_RETURN_NULL(p) do {  \
        if (unlikely((p) == HI_NULL)) {      \
            HI_ERR_DEMUX("null pointer!\n"); \
            return HI_NULL; \
        }\
    } while (0)

#define DMX_NULL_POINTER_GOTO(p, out_flag) do { \
    if (unlikely((p) == HI_NULL)) {             \
        HI_ERR_DEMUX("null pointer!\n");        \
        ret =  HI_ERR_DMX_NULL_PTR;             \
        goto out_flag;                          \
    }\
} while (0)

#define DMX_NULL_POINTER_BREAK(p) do {   \
    if (unlikely((p) == HI_NULL)) {      \
        HI_ERR_DEMUX("null pointer!\n"); \
        break;                   \
    }\
} while (0)

hi_s32 drv_dmx_osr_debug_ctrl(hi_u32 cmd, DMX_TRACE_CMD_CTRl cmd_ctrl, hi_u32 param);

#define DMX_UNUSED(x) ((x) = (x))

#define DMX_FATAL_CON_VOID_RETURN(condition) do { \
    if (unlikely(condition)) { \
        HI_ERR_DEMUX("DEMUX ASSERT ERROR: %s\n", #condition); \
        return;  \
    } \
} while (0)

#define DMX_FATAL_CON_VOID_GOTO(condition, out_flag) do { \
    if (unlikely(condition)) { \
        HI_ERR_DEMUX("DEMUX ASSERT ERROR: %s\n", #condition); \
        goto out_flag;  \
    } \
} while (0)


#define DMX_FATAL_CON_RETURN(condition, err_code) do { \
    if (unlikely(condition)) { \
        HI_ERR_DEMUX("DEMUX ASSERT ERROR: %s\n", #condition); \
        return err_code;  \
    } \
} while (0)

#define DMX_FATAL_CON_GOTO(condition, err_code, out_flag) do { \
    if (unlikely(condition)) { \
        HI_ERR_DEMUX("DEMUX ASSERT ERROR: %s\n", #condition); \
        ret = err_code; \
        goto out_flag;  \
    } \
} while (0)

#define dmx_dofunc_no_return(func) do { \
    hi_s32 ret_val = HI_SUCCESS; \
    ret_val = (func); \
    if (ret_val != HI_SUCCESS) { \
        HI_ERR_DEMUX("call %s failed, ret = 0x%x\n", # func, ret_val); \
    } \
} while (0)

#define dmx_call_func_err(func, err_code)        HI_ERR_DEMUX("call [%s] return [0x%08x]\n", #func, err_code)

#define dmx_dofunc_warn_no_return(func) do { \
    hi_s32 ret_val = HI_SUCCESS; \
    ret_val = (func); \
    if (ret_val != HI_SUCCESS) { \
        HI_WARN_DEMUX("call %s failed, ret = 0x%x\n", # func, ret_val); \
    } \
} while (0)

#define dmx_dofunc_warn_return(func, ret) do { \
    (ret) = (func); \
    if ((ret) != HI_SUCCESS) { \
        HI_WARN_DEMUX("call %s failed, ret = 0x%x\n", # func, (ret)); \
        return (ret); \
    } \
} while (0)

#define dmx_dofunc_err_return(func, ret) do { \
    (ret) = (func); \
    if ((ret) != HI_SUCCESS) { \
        HI_ERR_DEMUX("call %s failed, ret = 0x%x\n", # func, (ret)); \
        return (ret); \
    } \
} while (0)

#define dmx_dofunc_goto(func, ret, switch_point) do { \
    (ret) = (func); \
    if ((ret) != HI_SUCCESS) { \
        HI_ERR_DEMUX("call %s failed, ret = 0x%x\n", # func, (ret)); \
        goto switch_point; \
    } \
} while (0)

/* memory interface */
#define DMX_BUF_NAME_LEN  (32)
typedef struct {
    hi_mem_handle_t handle;
    hi_void *dmx_buf_obj;
    hi_char buf_name[DMX_BUF_NAME_LEN];
    hi_bool user_map_flag;  /* mmap in user space or not */
    hi_u8   *buf_vir_addr;
    hi_u32  buf_size;
    hi_u64  buf_phy_addr;
} dmx_mem_info;

/*
 * general link list obj helper.
 */
struct obj_node_helper {
    hi_void             *key;
    struct list_head    node;
};

/*
 * Resource base definition.
 */
struct dmx_r_base;

struct dmx_r_base_ops {
    hi_s32(*get)(struct dmx_r_base *obj);
    hi_void(*put)(struct dmx_r_base *obj);
};

struct dmx_r_base {
    osal_atomic            ref_count;
    struct dmx_r_base_ops  *ops;
    hi_s32(*release)(struct dmx_r_base *obj);

    hi_u32                 id;
    struct dmx_mgmt       *mgmt;
};

/*
 * slot definition.
 */
struct dmx_session;
struct dmx_slot {
    osal_mutex              lock;
    struct dmx_session      *session;
    struct dmx_r_base       *obj;

    hi_handle handle;
    hi_s32(*release)(hi_handle handle);  /* private : maybe changed for different handle object. */
};

struct dmx_slot_table {
    osal_mutex       lock;
    hi_u32           slot_cnt;
    DECLARE_BITMAP(slot_bitmap, DMX_MAX_SLOT_CNT);
    struct dmx_slot table[DMX_MAX_SLOT_CNT];
};

/*
 * session definition.
 */
enum  dmx_session_state {
    DMX_SESSION_INACTIVED = 0x0,  /* support remove slot only. */
    DMX_SESSION_ACTIVED,          /* support add or remove slot. */
};

struct dmx_session_ops {
    hi_s32(*add_slot)(struct dmx_session *session, struct dmx_slot *slot);
    hi_s32(*del_slot)(struct dmx_session *session, struct dmx_slot *slot);
    hi_void(*release)(struct dmx_session *session);  /* release all R obj from session. */
    hi_s32(*suspend)(struct dmx_session *session);
    hi_s32(*resume)(struct dmx_session *session);
};

struct dmx_session {
    osal_atomic                  ref_count;
    osal_mutex                   lock;
    enum  dmx_session_state      state;
    struct list_head             head;

    struct dmx_session_ops       *ops;
};

/*
 * export functions.
 */
struct dmx_r_base_ops *get_dmx_rbase_ops(hi_void);
hi_s32  dmx_r_get_raw(struct dmx_r_base *obj);
hi_s32  dmx_r_get(hi_handle handle, struct dmx_r_base **obj);
hi_void dmx_r_put(struct dmx_r_base *obj);

hi_s32 dmx_slot_create(struct dmx_r_base *obj, struct dmx_slot **slot);
hi_s32 dmx_slot_find(hi_handle handle, struct dmx_slot **slot);
hi_s32 dmx_slot_get_robj(hi_handle handle, struct dmx_r_base **obj);
hi_s32 dmx_slot_destroy(struct dmx_slot *slot);

hi_s32 dmx_session_create(struct dmx_session **new_session);
hi_s32 dmx_session_add_slot(struct dmx_session *session, struct dmx_slot *slot);
hi_s32 dmx_session_del_slot(struct dmx_session *session, struct dmx_slot *slot);
hi_s32 dmx_session_destroy(struct dmx_session *session);
hi_s32 dmx_slot_table_init(hi_void);
hi_void dmx_slot_table_destory(hi_void);
/*
 * general common functions.
 */
hi_u32 dmx_get_queue_lenth(const hi_u32 read, const hi_u32 write, const hi_u32 size);

DMX_TRACE_LEVEL dmx_drv_utils_get_trace_level(hi_void);
hi_s32  dmx_drv_utils_set_trace_level(DMX_TRACE_LEVEL dmx_trace_level);
hi_void dmx_drv_utils_enable_trace_level(DMX_TRACE_LEVEL dmx_trace_level);
hi_void dmx_drv_utils_disable_trace_level(DMX_TRACE_LEVEL dmx_trace_level);

/* memory infomation functions */
hi_s32  dmx_alloc_and_map_buf(const hi_char *buf_name, hi_u32 buf_size, hi_bool is_cached, dmx_mem_info *buf_info_ptr);
hi_void dmx_unmap_and_release_buf(dmx_mem_info *buf_info_ptr);

hi_s32  dmx_alloc_and_map_mmz(const hi_char *buf_name, hi_u32 buf_size, hi_bool is_cached, dmx_mem_info *buf_info_ptr);
hi_void dmx_unmap_and_release_mmz(dmx_mem_info *buf_info_ptr);

hi_s32  dmx_tee_alloc_buf(const hi_char *buf_name, hi_u32 buf_size, dmx_mem_info *buf_info_ptr);
hi_void dmx_tee_release_buf(const dmx_mem_info *buf_info_ptr);

hi_s32 dmx_get_buf_fd_by_phyaddr(hi_u64 phy_addr, hi_u32 buf_size, hi_mem_handle_t *fd);
hi_s32 dmx_put_buf_fd(hi_mem_handle_t fd);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_DEMUX_UTILS_H__

