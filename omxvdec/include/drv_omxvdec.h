
/*
 * Copyright (c) (2014 - ...) Device Chipset Source Algorithm and Chipset Platform Development Dept
 * Hisilicon. All rights reserved.
 *
 * File:    drv_omxvdec.h
 *
 * Purpose: omxvdec export header file
 *
 * Author:  sdk
 *
 * Date:    26, 11, 2014
 *
 * NOTICE !!
 * 该文件同时被component 和driver 引用
 * 如修改，两边均需重新编译!!!
 */

#ifndef __DRV_OMXVDEC_H__
#define __DRV_OMXVDEC_H__

#include <linux/ioctl.h>

#include "vfmw_ext.h"
#include "hi_drv_video.h"
#include "drv_omxvdec_ext.h"
#include "hi_drv_mem.h"

#define OMXVDEC_VERSION (2019112100)

#define OMXVDEC_NAME "hi_omxvdec"
#define DRIVER_PATH  "/dev/hi_omxvdec"

#define PATH_LEN (64)
#define NAME_LEN (64)

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) > (b)) ? (b) : (a))

#define UHD_8K_FRAME_WIDTH  (8192)
#define UHD_8K_FRAME_HEIGHT (4352)
#define UHD_FRAME_WIDTH     (4096)
#define UHD_FRAME_HEIGHT    (2304)
#define HD_FRAME_WIDTH      (1920)
#define HD_FRAME_HEIGHT     (1088)
#define SD_FRAME_WIDTH      (1280)
#define SD_FRAME_HEIGHT     (768)

#define MAX_SUPPORT_WIDTH  (UHD_8K_FRAME_WIDTH)
#define MAX_SUPPORT_HEIGHT (UHD_8K_FRAME_HEIGHT)

#define DEFAULT_FRAME_WIDTH  (HD_FRAME_WIDTH)
#define DEFAULT_FRAME_HEIGHT (HD_FRAME_HEIGHT)

#define DEFAULT_BIT_DEPTH (8)
#define BIT_DEPTH_10      (10)

#define HI_OMX_ALIGN_10BIT(w) (((w)*10 + 8 - 1) / (8))

#define DEFAULT_FRAME_SIZE(w, h) (((w) * (h)*3) / 2)
#define FRAME_SIZE_10BIT(w, h)   ((((w) * (h)*3) / 2) * 5 / 4)
#define HI_OMX_GET_STRIDE(w)     (((w) + (16 - 1)) & (~(16 - 1)))

#define MAX_IN_BUF_SLOT_NUM (10)

// the max num can be recorded by component, default DEF_MAX_OUT_BUF_CNT + 2, can be changed if needed
#define MAX_OUT_BUF_SLOT_NUM (30)

#define hi_kmalloc_omxvdec(size)        osal_kmalloc(HI_ID_VDEC, size, OSAL_GFP_KERNEL)
#define hi_kmalloc_atomic_omxvdec(size) osal_kmalloc(HI_ID_VDEC, size, OSAL_GFP_ATOMIC)
#define hi_kfree_omxvdec(addr)          osal_kfree(HI_ID_VDEC, addr)

#define hi_vmalloc_omxvdec(size) osal_vmalloc(HI_ID_VDEC, size)
#define hi_vfree_omxvdec(addr)   osal_vfree(HI_ID_VDEC, addr)

typedef enum  {
    OMX_PIX_FMT_NV12, /* YUV 4:2:0 semiplanar */
    OMX_PIX_FMT_NV21, /* YVU 4:2:0 semiplanar */
    OMX_PIX_FMT_YUVAFBC,
    OMX_PIX_BUTT
} omx_pix_format;

enum {
    VDEC_S_SUCCESS = 0,
    VDEC_S_FAILED = 1,
};

/* ========================================================
 * internal struct enum definition
 * ======================================================== */
typedef enum {
    PORT_DIR_INPUT,
    PORT_DIR_OUTPUT,
    PORT_DIR_BOTH = 0xFFFFFFFF
} e_port_dir;

typedef enum {
    OMX_ALLOCATE_USR,
    OMX_USE_OTHER,
    OMX_USER_BIGBUF,
    OMX_USE_NATIVE,
    OMX_USE_SECURE,
    OMX_USE_WITH_HFBC,
    OMX_ALLOCATE_SECURE,
    OMX_ALLOCATE_WITH_META,
    OMX_BUTT_TYPE,
} e_buffer_type;

typedef vfmw_vid_std VDEC_CODEC_TYPE;

typedef struct {
    hi_bool is_tvp;
    hi_bool b_vpss_bypass;
    hi_bool output_afbc;
    hi_handle ssm_handle;
    hi_u32 output_view;
    hi_u32 output_bit_depth;
    hi_u32 cfg_width;
    hi_u32 cfg_height;
    hi_u32 cfg_stride;
    hi_u32 cfg_inbuf_num;
    hi_u32 cfg_outbuf_num;
    hi_u32 cfg_rotation;
    omx_pix_format cfg_color_format;
    hi_bool m_use_native_buf;
    hi_u32 cfg_framerate;
    hi_bool is_10bit;
    vfmw_chan_cfg chan_cfg;
    hi_u32 tunnel_mode_enable;
    hi_u32 tunnel_attach;
    hi_bool is_net_work;
    hi_bool android_test;
    hi_bool b_is_gst;
    hi_bool b_is_on_live;
    hi_bool b_meta_data_alloc;
    hi_drv_hdr_type src_frame_type;
    hi_drv_raw_clolr_descript colour_info;
    hi_drv_hdr_static_metadata hdr10_info;
    hi_bool set_corlour_info;
    hi_bool set_hdr_static_info;
    hi_bool anw_store_meta;
} OMXVDEC_DRV_CFG;

typedef struct {
    hi_s32 video_driver_fd;
    hi_s32 chan_handle;
    OMXVDEC_DRV_CFG drv_cfg;
    hi_void *yuv_fp;
    hi_u8 *chrom_l;
    hi_u32 chrom_l_size;
} OMXVDEC_DRV_CONTEXT;

/* video frame buffer description */
typedef struct {
    hi_u32 phyaddr_y;
    hi_u32 phyaddr_c;
    hi_u32 stride;
    hi_u32 width;
    hi_u32 height;
    hi_bool save_yuv;
    hi_char save_path[PATH_LEN];
} omxvdec_frame;

typedef struct {
    hi_u32 width;
    hi_u32 height;
    hi_u32 stride;
    hi_u32 bit_depth;
    hi_u32 size;
    hi_u32 min_num;
    hi_u32 max_num;
} OMXVDEC_IMG_SIZE;

typedef struct {
    hi_u32 frame_width;
    hi_u32 frame_height;
    hi_u32 frame_stride;
    hi_u32 frame_size;
    hi_u32 max_frame_num;
    hi_u32 min_frame_num;
} OMXVDEC_DEC_SIZE;

typedef struct {
    hi_u8 m_range;
    hi_u8 m_primaries;
    hi_u8 m_transfer;
    hi_u8 m_matrix_coeffs;
} OMXVDEC_DEC_COLOR_ASPECTS;

typedef struct {
    hi_u32 phyaddr;
    hi_u32 buffer_len;
    hi_u32 align;
    hi_u32 data_offset;
    hi_u32 data_len;
    hi_u32 private_phyaddr;
    hi_virt_addr_t private_user_viraddr;
    hi_u32 private_len;
    hi_bool is_overlay;
    hi_u32 flags;
    hi_s64 timestamp;
    e_buffer_type buffer_type;
    e_buffer_type meta_type;
    e_port_dir dir;
    // @f00241306 for 64 bit
    hi_virt_addr_t bufferaddr;
    hi_virt_addr_t client_data;
    omxvdec_frame out_frame;
    hi_u32 frame_rate;
    hi_u32 framebuffer_fd;
    hi_u32 private_fd;
    hi_u32 private_offset;
    hi_s32 hfbc_offset;
    hi_bool is_sec;
    hi_u32 max_num;
    hi_bool is_first_buf;
    OMXVDEC_DEC_COLOR_ASPECTS dec_color_aspects;
    hi_drv_hdr_metadata hdr_info;
    hi_drv_hdr_type src_frame_type;
    hi_bool from_work_queue;
    hi_s32 release_fd;
    hi_u32 fd;
    hi_u64 dma_buf;
    hi_s64 metadata_fd;
    hi_u64 private_dma_buf;
} OMXVDEC_BUF_DESC;

typedef struct {
    hi_u32 status_code;
    hi_u32 msgcode;
    hi_u32 msgdatasize;

    union {
        OMXVDEC_IMG_SIZE img_size;
        OMXVDEC_DEC_SIZE dec_size;
        OMXVDEC_DEC_COLOR_ASPECTS color_aspects_info;
        OMXVDEC_BUF_DESC buf;
    } msgdata;
} OMXVDEC_MSG_INFO;

/* ========================================================
 * IOCTL for interaction with omx components
 * ======================================================== */
typedef struct {
    hi_u64 in;
    hi_u64 out;
    hi_s32 chan_num;
} OMXVDEC_IOCTL_MSG;

#define VDEC_IOCTL_MAGIC 'v'

#define VDEC_IOCTL_CHAN_CREATE _IO(VDEC_IOCTL_MAGIC, 1)

#define VDEC_IOCTL_CHAN_RELEASE _IO(VDEC_IOCTL_MAGIC, 2)

#define VDEC_IOCTL_SET_EXTRADATA _IO(VDEC_IOCTL_MAGIC, 3)

#define VDEC_IOCTL_GET_EXTRADATA _IO(VDEC_IOCTL_MAGIC, 4)

#define VDEC_IOCTL_FLUSH_PORT _IO(VDEC_IOCTL_MAGIC, 5)

#define VDEC_IOCTL_CHAN_BIND_BUFFER _IO(VDEC_IOCTL_MAGIC, 6)

#define VDEC_IOCTL_CHAN_UNBIND_BUFFER _IO(VDEC_IOCTL_MAGIC, 7)

#define VDEC_IOCTL_FILL_OUTPUT_FRAME _IO(VDEC_IOCTL_MAGIC, 8)

#define VDEC_IOCTL_EMPTY_INPUT_STREAM _IO(VDEC_IOCTL_MAGIC, 9)

#define VDEC_IOCTL_CHAN_START _IO(VDEC_IOCTL_MAGIC, 10)

#define VDEC_IOCTL_CHAN_STOP _IO(VDEC_IOCTL_MAGIC, 11)

#define VDEC_IOCTL_CHAN_PAUSE _IO(VDEC_IOCTL_MAGIC, 12)

#define VDEC_IOCTL_CHAN_RESUME _IO(VDEC_IOCTL_MAGIC, 13)

#define VDEC_IOCTL_CHAN_GET_MSG _IO(VDEC_IOCTL_MAGIC, 14)

#define VDEC_IOCTL_CHAN_STOP_MSG _IO(VDEC_IOCTL_MAGIC, 15)

#define VDEC_IOCTL_CHAN_ALLOC_BUF _IO(VDEC_IOCTL_MAGIC, 16)

#define VDEC_IOCTL_CHAN_RELEASE_BUF _IO(VDEC_IOCTL_MAGIC, 17)

#define VDEC_IOCTL_CHAN_SET_FORMAT_INFO _IO(VDEC_IOCTL_MAGIC, 20)

#define VDEC_IOCTL_CHAN_PORT_ENABLE _IO(VDEC_IOCTL_MAGIC, 22)

#define VDEC_IOCTL_CHAN_RECORD_OCCUPIED_FRAME _IO(VDEC_IOCTL_MAGIC, 23)

#define VDEC_IOCTL_GET_BUFFER_INFO _IO(VDEC_IOCTL_MAGIC, 24)

#endif

