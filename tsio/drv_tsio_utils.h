/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description: tsio basic utils impl.
* Author: guoqingbo
* Create: 2016-08-12
*/

#ifndef __DRV_TSIO_UTILS_H__
#define __DRV_TSIO_UTILS_H__

#include "hi_drv_module.h"
#include "drv_tsio_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HI_FATAL_TSIO(fmt...)      HI_FATAL_PRINT(HI_ID_TSIO, fmt)
#define HI_ERR_TSIO(fmt...)        HI_ERR_PRINT(HI_ID_TSIO, fmt)
#define HI_WARN_TSIO(fmt...)       HI_WARN_PRINT(HI_ID_TSIO, fmt)
#define HI_INFO_TSIO(fmt...)       HI_INFO_PRINT(HI_ID_TSIO, fmt)
#define HI_DBG_TSIO(fmt...)        HI_DBG_PRINT(HI_ID_TSIO, fmt)

#define INVALID_PID 0x1fff

#define TS_PKT_SIZE              188
#define BULK_PKT_SIZE            192
#define SP_PKT_SIZE              212

#define TSIO_UNUSED(x) ((x) = (x))

/* memory interface */
#define TSIO_BUF_NAME_LEN  (32)
typedef struct {
    hi_mem_handle_t handle;
    hi_void *tsio_buf_obj;
    hi_u64  buf_phy_addr;
    hi_u8   *buf_vir_addr;
    hi_u32  buf_size;
    hi_bool is_usr_map;
} tsio_mem_info;

hi_u32  tsio_get_queue_lenth(const hi_u32 read, const hi_u32 write, const hi_u32 size);

struct  tsio_slot_table *tsio_get_slot_table(hi_void);
struct  tsio_r_base_ops *get_tsio_r_base_ops(hi_void);


hi_s32  tsio_r_get_raw(struct tsio_r_base *obj);
hi_s32  tsio_r_get(hi_handle handle, struct tsio_r_base **obj);
hi_void tsio_r_put(struct tsio_r_base *obj);

hi_s32  tsio_session_create(struct tsio_session **new_session);
hi_s32  tsio_session_add_slot(struct tsio_session *session, struct tsio_slot *slot);
hi_s32  tsio_session_del_slot(struct tsio_session *session, struct tsio_slot *slot);
hi_s32  tsio_session_destroy(struct tsio_session *session);

hi_s32  tsio_slot_create(struct tsio_r_base *obj, struct tsio_slot **slot);
hi_s32  tsio_slot_find(hi_handle handle, struct tsio_slot **slot);
hi_s32  tsio_slot_get_robj(hi_handle handle, struct tsio_r_base **obj);
hi_s32  tsio_slot_destroy(struct tsio_slot *slot);
hi_s32  tsio_slot_table_init(hi_void);
hi_void tsio_slot_table_destory(hi_void);

hi_s32 tsio_alloc_and_map_mmz(const hi_char *buf_name, hi_u32 buf_size,
    hi_bool is_cached, tsio_mem_info *buf_info_ptr);
hi_void tsio_unmap_and_release_mmz(tsio_mem_info *buf_info_ptr);
hi_s32 tsio_alloc_and_map_buf(const hi_char *buf_name, hi_u32 buf_size,
    hi_bool is_cached, tsio_mem_info *buf_info_ptr);
hi_void tsio_unmap_and_release_buf(tsio_mem_info *buf_info_ptr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_TSIO_UTILS_H__

