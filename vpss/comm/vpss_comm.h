/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: vpss common define
 * Author: zhangjunyu
 * Create: 2016/07/01
 */
#ifndef __DRV_VPSS_COMM_H__
#define __DRV_VPSS_COMM_H__

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"
#include "hi_osal.h"

#include "hi_drv_sys.h"
#include "hi_drv_proc.h"

#include "hi_drv_video.h"
#include "hi_drv_mem.h"
#include "hi_drv_stat.h"
#include "hi_drv_vpss.h"
#include "vpss_define.h"
#include "drv_pq_ext.h"
#include "linux/hisilicon/securec.h"
#include "hi_drv_dev.h"
#include "drv_vpss_mem.h"
#include "drv_vpss_pri.h"
#include "hi_drv_ssm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define OSAL_OK  0
#define OSAL_ERR (-1)

#define EVENT_DONE 1
#define EVENT_UNDO 0

#define vpss_stride_align(w)   (((w) + 63) / 64 * 64)
#define hiceiling(x, a)        (((x) + (a)-1) / (a))

#define vpss_upround_div2align(x) (((x) / 2 + 1) / 2 * 2)
#ifndef HI_VPSS_DRV_USE_GOLDEN
#define vpss_align_stride_8bit(x)       vpss_stride_align(x)
#define vpss_align_stride_10bit_comp(x) vpss_stride_align(hiceiling((x)*10, 8)) /* l0bit½ô´ÕÄ£Ê½ */
#else
#define vpss_align_stride_8bit(x)       (((((x)*8) + 127) / 128) * 16)
#define vpss_align_stride_10bit_comp(x) (((((x)*10) + 127) / 128) * 16)
#endif
#define vpss_align_8bit_ystride(x)       (((((x)*8) + 127) / 128) * 16)
#define vpss_align_10bit_comp_ystride(x) (((((x)*10) + 127) / 128) * 16)
#define vpss_wbc_stt_align_ystride(x, y) ((((((x) * (y) + 7) / 8) + 15) / 16) * 16)
#define vpss_calc_loss_cmp_ystride(x, y, z)   ((((((x) * (y)*1000) / (z)) + 127) / 128) * 16)
#define vpss_calc_lossless_cmp_ystride(x, y)   ((((((x) * (y)*1300) / 1000) + 127) / 128) * 16)

#define VPSS_PROC_MAX_PARAM_NUM           5
#define VPSS_PROC_PARAM_MAX_LENGTH        15
#define VPSS_BITDEPTH_8BIT                8
#define VPSS_BITDEPTH_10BIT               10
#define VPSS_PICTURE_SAVE_PATH_MAX_LENGTH 50
#define VPSS_FILE_OPERATE_RIGHTS          0644

#define vpss_kmalloc(fmt...)              osal_kmalloc(HI_ID_VPSS, fmt)
#define vpss_kfree(ptr)              \
    do {                                  \
        if ((ptr) != HI_NULL) {           \
            osal_kfree(HI_ID_VPSS, ptr); \
            (ptr) = HI_NULL;            \
        }                                 \
    } while (0)

#define vpss_vmalloc(fmt...)         osal_vmalloc(HI_ID_VPSS, fmt)
#define vpss_vfree(ptr)                  \
    do {                                 \
        if ((ptr) != HI_NULL) {          \
            osal_vfree(HI_ID_VPSS, ptr); \
            (ptr) = HI_NULL;             \
        }                                \
    } while (0)

#define HI_ERR_VPSS_DEV_OPEN_ERR     (hi_s32)(0x80920000)
#define HI_ERR_VPSS_DEV_CLOSE_ERR    (hi_s32)(0x80920001)
#define HI_ERR_VPSS_NULL_PTR         (hi_s32)(0x80920002)
#define HI_ERR_VPSS_NO_INIT          (hi_s32)(0x80920003)
#define HI_ERR_VPSS_INVALID_PARA     (hi_s32)(0x80920004)
#define HI_ERR_VPSS_CREATE_ERR       (hi_s32)(0x80920005)
#define HI_ERR_VPSS_DESTROY_ERR      (hi_s32)(0x80920006)
#define HI_ERR_VPSS_INVALID_OPT      (hi_s32)(0x80920007)
#define HI_ERR_VPSS_INST_NOT_EXIST   (hi_s32)(0x80920008)
#define HI_ERR_VPSS_PORT_NOT_EXIST   (hi_s32)(0x80920009)
#define HI_ERR_VPSS_FRAME_INFO_ERROR (hi_s32)(0x8092000A)
#define HI_ERR_VPSS_MALLOC_FAILED    (hi_s32)(0x8092000B)

#define vpss_check_null(ptr)              \
    do {                                  \
        if ((ptr) == HI_NULL) {           \
            vpss_error("para is null\n"); \
            return HI_FAILURE;            \
        }                                 \
    } while (0)


#define vpss_check_null_noret(ptr)              \
            do {                                  \
                if ((ptr) == HI_NULL) {           \
                    vpss_error("para is null\n"); \
                }                                 \
            } while (0)

#define vpss_check_init(init)                      \
    do {                                           \
        if ((init) == HI_FALSE) {                  \
            vpss_error("resource has not init\n"); \
            return HI_FAILURE;                     \
        }                                          \
    } while (0)

#define vpss_assert_ret(expr)                                                                                    \
    do {                                                                                                         \
        if (!(expr)) {                                                                                           \
            osal_printk("\n_assert failed at:\n >file name:%s\n >function:%s\n >line no:%d\n >condition: %s\n",  \
                         __FILE__, __FUNCTION__, __LINE__, #expr);                                               \
            return HI_FAILURE;                                                                                   \
        }                                                                                                        \
    } while (0)

#define vpss_assert_not_ret(expr)                           \
    do{                                                     \
        if (!(expr)) {                                      \
            osal_printk("\nASSERT failed at:\n >File name:%s\n >Function:%s\n >Line No:%d\n >Condition:%s\n",    \
                        __FILE__,__FUNCTION__, __LINE__, #expr);    \
        }                                                   \
    } while (0)

#define vpss_check_null_pointer(ptr)                                     \
    do{                                                                  \
        if ((ptr) == HI_NULL) {                                          \
            vpss_error("vpss para null pointer in %s!\n", __FUNCTION__); \
            return HI_ERR_VPSS_NULL_PTR;                                 \
        }                                                                \
    } while (0)

#define vpss_check_bool(bool)                                                           \
    do{                                                                                 \
        if (((bool) != HI_TRUE) && ((bool) != HI_FALSE)) {                              \
            vpss_error("vpss input invaild bool in %s! bool=%d\n", __FUNCTION__, bool); \
            return HI_FAILURE;                                                          \
        }                                                                               \
    } while (0)

#define vpss_check_enum(var, max)                                                     \
    do{                                                                               \
        if ((var) >= (max)) {                                                         \
            vpss_error("vpss input invaild enum in %s! var %d\n", __FUNCTION__, var); \
            return HI_FAILURE;                                                        \
        }                                                                             \
    } while (0)

#define vpss_check_zero(value)                                                           \
    do{                                                                                 \
        if ((value) == 0) {                              \
            vpss_error("zero %s! value=%d\n", __FUNCTION__, value); \
        }                                                                               \
    } while (0)

typedef struct file file;
typedef struct hi_kern_event {
    osal_wait queue_head;
    hi_s32 flag_1;
    hi_s32 flag_2;
} kern_event;

typedef struct osal_list_head list;

typedef kern_event vpss_comm_event;
typedef osal_spinlock vpss_comm_spin;

#define vpss_fatal(fmt...) \
    HI_FATAL_PRINT(HI_ID_VPSS, fmt)

#define vpss_error(fmt...) \
    HI_ERR_PRINT(HI_ID_VPSS, fmt)

#define vpss_warn(fmt...) \
    HI_WARN_PRINT(HI_ID_VPSS, fmt)

#define vpss_info(fmt...) \
    HI_INFO_PRINT(HI_ID_VPSS, fmt)

#define vpss_dbg(fmt...) \
    HI_INFO_PRINT(HI_ID_VPSS, fmt)

#define vpss_info_func_enter() HI_INFO_PRINT(HI_ID_VPSS, "HI_ID_VPSS ===>[Enter]\n")
#define vpss_info_func_exit()  HI_INFO_PRINT(HI_ID_VPSS, "HI_ID_VPSS <===[Exit]\n")
#define vpss_info_func_trace() HI_INFO_PRINT(HI_ID_VPSS, "HI_ID_VPSS =TRACE=\n")

#define vpss_info_spec_func_enter() HI_ERR_PRINT(HI_ID_VPSS, "HI_ID_VPSS ===>[Enter]\n")
#define vpss_info_spec_func_exit()  HI_ERR_PRINT(HI_ID_VPSS, "HI_ID_VPSS <===[Exit]\n")

typedef enum {
    VPSS_IP_0 = 0,
    VPSS_IP_1,
    VPSS_IP_MAX
} vpss_ip;

typedef struct {
    hi_u32 buf_size;
    hi_u32 stride_y;
    hi_u32 stride_c;
    hi_u32 width;
    hi_u32 height;
} vpss_buffer_attr; /* no mem related */

typedef enum {
    VPSS_MEM_SOURCE_SMMU = 0,
    VPSS_MEM_SOURCE_ION,
    VPSS_MEM_SOURCE_ILLEGAL,
    VPSS_MEM_SOURCE_MAX
} vpss_mem_source;

typedef enum {
    VPSS_FORMAT_SP420 = 0,
    VPSS_FORMAT_SP422,
    VPSS_FORMAT_SP420_CMP,
    VPSS_FORMAT_SP422_CMP,
    VPSS_FORMAT_RGB,
    VPSS_FORMAT_SP444,
    VPSS_FORMAT_MAX,
} vpss_format_type;

typedef struct {
    hi_bool dei_en;
    hi_bool dei_ma_mode;
    hi_bool tnr_en;
    hi_bool tnr_ma_mode;

    hi_bool dm430_en; /* only use one */
    hi_bool hdr_en;
} vpss_perfor_alg_ctrl;

/* file operation */
hi_void *vpss_comm_fopen(const char *filename, int flags, int mode);
void vpss_comm_fclose(hi_void *filp);
hi_s32 vpss_comm_fread(char *buf, unsigned int len, hi_void *filp);
hi_s32 vpss_comm_fwrite(const char *buf, int len, hi_void *filp);
hi_s32 vpss_comm_cmd_check(hi_u32 in_argc, hi_u32 aspect_argc, hi_void *arg, hi_void *private);

/* event operation */
hi_s32 vpss_comm_init_event(vpss_comm_event *event, hi_s32 init_val1, hi_s32 init_val2);
hi_void vpss_comm_give_event(vpss_comm_event *event, hi_s32 init_val1, hi_s32 init_val2);
hi_s32 vpss_comm_wait_event(vpss_comm_event *event, hi_s32 wait_time);
hi_void vpss_comm_reset_event(vpss_comm_event *event, hi_s32 init_val1, hi_s32 init_val2);
hi_void vpss_comm_destory_event(vpss_comm_event *event);

/* spin lock operation */
hi_s32 vpss_comm_init_spin(vpss_comm_spin *lock);
hi_void vpss_comm_down_spin(vpss_comm_spin *lock, unsigned long *flags);
hi_void vpss_comm_up_spin(vpss_comm_spin *lock, unsigned long *flags);
hi_void vpss_comm_destory_spin(vpss_comm_spin *lock);

/* debug operation */
hi_s32 vpss_comm_proc_parse_para(hi_char *proc_para, hi_char (*parg)[VPSS_PROC_PARAM_MAX_LENGTH],
                                 hi_u32 cnt, hi_u32 *arg_num);

hi_s32 vpss_comm_str_to_numb(hi_char *ch_str, hi_u32 *numb);
hi_s32 vpss_comm_save_out_frame_to_file(hi_drv_vpss_video_frame *frame, hi_char *pch_file, hi_u32 filename_size);

hi_s32 vpss_comm_save_frame_to_file(hi_drv_vpss_video_frame *frame, hi_char *pch_file, hi_u32 filename_size);
hi_s32 vpss_comm_cal_buf_size(vpss_buffer_attr *buf_attr, hi_drv_pixel_format pixel_format,
                              hi_drv_compress_info cmp_info, hi_drv_pixel_bitwidth out_bit_width,
                              hi_bool loss_cmp, hi_u32 cmp_rate);

/* time operation */
hi_u64 vpss_comm_get_sched_time(hi_void);

/* mem operation */
hi_s32 vpss_comm_mem_alloc(drv_vpss_mem_attr *in_mem_attr, drv_vpss_mem_info *out_mem_info);
hi_void vpss_comm_mem_free(drv_vpss_mem_info *mem_info);
hi_s32 vpss_comm_mem_map_vir_addr(hi_drv_vpss_video_frame *frame, drv_vpss_mem_info *mem_info);
hi_s32 vpss_comm_mem_umap_vir_addr(hi_drv_vpss_video_frame *frame, drv_vpss_mem_info *mem_info);
hi_s32 vpss_comm_mem_flush(drv_vpss_mem_info *mem_info);

/* event report operation */
hi_void vpss_comm_notify_event(hi_drv_vpss_video_frame *frame, hi_stat_ld_event event_id);

/* pq operation */
hi_s32 vpss_comm_pq_update_pq_reg(hi_drv_pq_vpss_stt_info *vpss_stt_info,
    hi_drv_pq_vpss_cfg_info *vpss_cfg_info, hi_void *vpss_reg);

hi_s32 vpss_comm_pq_update_timing_info(hi_u32 handle_no, hi_drv_pq_vpss_info *timing_info, hi_void *vpss_reg);
hi_s32 vpss_comm_pq_run_alg(hi_drv_pq_vpss_stt_info *info_in, hi_drv_pq_vpss_cfg_info *info_out);
hi_s32 vpss_comm_pq_run_stt_alg(hi_drv_pq_vpss_stt_info *stt_info);
hi_s32 vpss_comm_pq_get_zme_coef(hi_drv_pq_vpss_layer layer_id,
    hi_drv_pq_vpsszme_in *zme_in, hi_drv_pq_vpsszme_out *zme_out);
hi_s32 vpss_comm_pq_get_hdr_cfg(hi_drv_pq_vpss_layer layer_id,
    hi_drv_pq_xdr_frame_info *frame_info, hi_void *vpss_reg);
hi_s32 vpss_comm_increase_frame_refcount(hi_drv_video_frame *frame);
hi_s32 vpss_comm_decrease_frame_refcount(hi_drv_video_frame *frame);
hi_s32 vpss_comm_decrease_frame_refcount_privframe(hi_drv_vpss_video_frame *frame);

hi_bool vpss_comm_is_loss_compress(hi_drv_compress_mode cmp_mode);

hi_void vpss_comm_cvt_comm_to_privite_nomap(hi_drv_video_frame *comm_image, hi_drv_vpss_video_frame *vpss_image);
hi_void vpss_comm_cvt_privite_to_comm_nomap(hi_drv_vpss_video_frame *vpss_image, hi_drv_video_frame *comm_image);
hi_s32 vpss_comm_map_priv_video(hi_drv_video_frame *comm_image, hi_drv_vpss_video_frame *vpss_image);
hi_s32 vpss_comm_umap_priv_video(hi_drv_vpss_video_frame *vpss_image);

hi_void vpss_comm_copy_comm_win_meta_to_vpss_meta(hi_drv_video_frame *comm_image);
hi_void vpss_comm_copy_comm_vpss_meta_to_comm_meta(hi_drv_video_frame *comm_image);
hi_void vpss_comm_copy_priv_win_meta_to_vpss_meta(hi_drv_vpss_video_frame *vpss_image);
hi_void vpss_comm_copy_priv_vpss_meta_to_win_meta(hi_drv_vpss_video_frame *vpss_image);
hi_s32 vpss_comm_attach_buf(hi_bool secure, hi_drv_ssm_buf_attach_info attach_info, hi_u64 *sec_info_addr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif



