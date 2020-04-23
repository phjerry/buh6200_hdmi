#ifndef __COMM_SHARE_H__
#define __COMM_SHARE_H__

#include "vfmw.h"

#define SHR_IMG_QUEUE_SIZE    64
#define SHR_EVT_QUEUE_SIZE    128
#define SHR_STM_QUEUE_SIZE    128

#define SHR_IMG_READY_STATE   2
#define SHR_IMG_ERROR_STATE   3
#define SHR_EVT_MAX_PARA_SIZE 1024
#ifdef VFMW_PROC_SUPPORT
#define SHR_PROC_SIZE         (100 * 1024)
#else
#define SHR_PROC_SIZE         0
#endif

#define SHR_IMG_INC(x) (((x) + 1) % SHR_IMG_QUEUE_SIZE)
#define SHR_IMG_DEC(x) (((x) == 0) ? (SHR_IMG_QUEUE_SIZE - 1) : (((x) - 1) % SHR_IMG_QUEUE_SIZE))

#define SHR_EVT_INC(x) (((x) + 1) % SHR_EVT_QUEUE_SIZE)
#define SHR_EVT_DEC(x) (((x) == 0) ? (SHR_EVT_QUEUE_SIZE - 1) : (((x) - 1) % SHR_EVT_QUEUE_SIZE))

#define SHR_STM_INC(x) (((x) + 1) % SHR_STM_QUEUE_SIZE)
#define SHR_STM_DEC(x) (((x) == 0) ? (SHR_STM_QUEUE_SIZE - 1) : (((x) - 1) % SHR_STM_QUEUE_SIZE))

typedef struct {
    hi_s32 ready;
    vfmw_image img;
} shr_img;

typedef struct {
    hi_s32 release;
    hi_s32 image_id;
} shr_img_rels;

typedef struct {
    hi_s32 head_c;
    hi_s32 head_s;
    shr_img img_que[SHR_IMG_QUEUE_SIZE];
    shr_img_rels img_rels[SHR_IMG_QUEUE_SIZE];
} shr_img_list;

typedef struct {
    hi_s32 valid;
    hi_s32 type;
    hi_u8  para[SHR_EVT_MAX_PARA_SIZE];
    hi_u32 param_len;
    vfmw_adpt_type adpt_type; /*  channel type vdec/omxvdec  */
} shr_evt;

typedef struct {
    hi_s32 head;
    hi_s32 tail;
    shr_evt event[SHR_EVT_QUEUE_SIZE];
} shr_evt_list;

typedef struct {
    hi_s32 head_c;
    hi_s32 head_s;
    hi_s32 tail_s;
    hi_s32 tail_c;
    vfmw_stream_data stm_data[SHR_STM_QUEUE_SIZE];
} shr_stm_list;

typedef struct {
    UADDR proc_phy_addr;
    hi_u64 proc_vir_addr;
#ifndef VFMW_MDC_SUPPORT
    UADDR chan_option_phy_addr;
    hi_u64 chan_option_vir_addr;
#else
    UADDR comm_shrmem_phy_addr;
    hi_u64 comm_shrmem_vir_addr;

    /*
    used for vdec create chan addr for tmp
    size : 128 bytes per chan
    maybe delete after vdec rebuilt
    */
    UADDR create_args_phy_addr;
    hi_u64 create_args_vir_addr;
#endif
    vfmw_mem_desc glb_shr_mem_desc;
} shr_glb_mem;

#define SHR_IMG_LIST_SIZE    (sizeof(shr_img_list))
#define SHR_EVT_LIST_SIZE    (sizeof(shr_evt_list))
#define SHR_STM_LIST_SIZE    (sizeof(shr_stm_list))
#define SHR_STA_MEM_SIZE     (sizeof(vfmw_chan_info))
#define SHR_CHAN_OPTION_SIZE (sizeof(vfmw_chan_option))
#ifdef VFMW_MDC_SUPPORT
#define SHR_GLBMEM_SIZE      (SHR_PROC_SIZE + MDC_COMM_SHARE_MEM_LENGTH + MDC_COMM_TMP_SHR_MEM_LENGTH)
#else
#define SHR_GLBMEM_SIZE      (SHR_CHAN_OPTION_SIZE + SHR_PROC_SIZE)
#endif

#endif

