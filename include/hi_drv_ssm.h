/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: struct defines for ssm driver.
 */

#ifndef __HI_DRV_SSM_H__
#define __HI_DRV_SSM_H__

#include "hi_type.h"
#include "hi_drv_module.h"

typedef enum {
    LOGIC_MOD_ID_DEMUX,
    LOGIC_MOD_ID_TSCIPHER,
    LOGIC_MOD_ID_CIPHER,
    LOGIC_MOD_ID_VMCU,
    LOGIC_MOD_ID_VDH,
    LOGIC_MOD_ID_AUD_DSP,
    LOGIC_MOD_ID_VENC,
    LOGIC_MOD_ID_VPSS,
    LOGIC_MOD_ID_VDP,
    LOGIC_MOD_ID_GPU,
    LOGIC_MOD_ID_HWC,
    LOGIC_MOD_ID_JPEG_DEC,
    LOGIC_MOD_ID_JPEG_ENC,
    LOGIC_MOD_ID_NPU,
    LOGIC_MOD_ID_MAX,
} hi_logic_mod_id;

typedef enum {
    HI_SSM_BUFFER_ID_INVALID = 0,
    HI_SSM_BUFFER_ID_INTERNAL_BUF_DMX,
    HI_SSM_BUFFER_ID_INTERNAL_BUF_TSCIPHER,
    HI_SSM_BUFFER_ID_CIPHER_CENC_BUF,
    HI_SSM_BUFFER_ID_INTERNAL_BUF_MCIPHER,
    HI_SSM_BUFFER_ID_VID_RAWLIST_MCU_ONLY,
    HI_SSM_BUFFER_ID_VID_SEGLIST_MCU_ONLY,
    HI_SSM_BUFFER_ID_VID_STDCTX_MCU_ONLY,
    HI_SSM_BUFFER_ID_VID_PICMSG_MCU_ONLY,
    HI_SSM_BUFFER_ID_VID_SLICEMSG_MCU_ONLY,
    HI_SSM_BUFFER_ID_VID_METADATA_MCU_ONLY,
    HI_SSM_BUFFER_ID_VID_SCDRAW_BUF,
    HI_SSM_BUFFER_ID_VID_SCDSEG_BUF,
    HI_SSM_BUFFER_ID_VID_SCDMSG,
    HI_SSM_BUFFER_ID_VID_VDHPMV_BUF,
    HI_SSM_BUFFER_ID_VID_VDHEXT_BUF_VID_ONLY,
    HI_SSM_BUFFER_ID_VID_FRMBIN_VDH_ONLY,
    HI_SSM_BUFFER_ID_INTERNAL_BUF_VDEC,

    HI_SSM_BUFFER_ID_INTERNAL_BUF_AUDDSP,

    HI_SSM_BUFFER_ID_INTERNAL_BUF_VENC,

    HI_SSM_BUFFER_ID_INTERNAL_BUF_VPSS,

    HI_SSM_BUFFER_ID_VDP_SD_WRITEBACK_ONLY,
    HI_SSM_BUFFER_ID_INTERNAL_BUF_VDP,

    HI_SSM_BUFFER_ID_INTERNAL_BUF_GPU,

    HI_SSM_BUFFER_ID_INTERNAL_BUF_HWC,

    HI_SSM_BUFFER_ID_INTERNAL_BUF_JPEG_DEC,

    HI_SSM_BUFFER_ID_INTERNAL_BUF_JPEG_ENC,

    HI_SSM_BUFFER_ID_INTERNAL_BUF_NPU,

    /* external buffer */
    HI_SSM_BUFFER_ID_DMX_VID_ES_BUF,

    HI_SSM_BUFFER_ID_DMX_AUD_ES_BUF,

    HI_SSM_BUFFER_ID_MCIPHER_VID_ES_BUF,

    HI_SSM_BUFFER_ID_MCIPHER_AUD_ES_BUF,

    HI_SSM_BUFFER_ID_MCIPHER_TS_BUF,

    HI_SSM_BUFFER_ID_PVR_RECORD_TS_BUF,

    HI_SSM_BUFFER_ID_PVR_PLAYBACK_TS_BUF,

    HI_SSM_BUFFER_ID_VID_FRM_BUF,

    HI_SSM_BUFFER_ID_VPSS_OUTPUT_BUF,

    HI_SSM_BUFFER_ID_VDP_OUTPUT_BUF,

    HI_SSM_BUFFER_ID_SECURE_INFOR_BUF,

    HI_SSM_BUFFER_ID_VIDEO_CAPTURE_ENCODE_OUTPUT_BUF,

    HI_SSM_BUFFER_ID_TRANSCODE_ENCODE_OUTPUT_BUF,
    HI_SSM_BUFFER_ID_MIRA_ENCODE_OUTPUT_BUF,

    HI_SSM_BUFFER_ID_GRAPHIC_OUPUT_BUF,

    HI_SSM_BUFFER_ID_NPU_OUTPUT_BUF,

    HI_SSM_BUFFER_ID_MAX
} hi_drv_ssm_buffer_id;

typedef enum {
    HI_SSM_BUFFER_TYPE_MODULE_INTERNAL_BUFFER = 0, /* Secure buffer which is only used within module */
    HI_SSM_BUFFER_TYPE_FRAME_BUFFER,               /* Secure frame buffer */
    HI_SSM_BUFFER_TYPE_SESSION_BELONGED_BUFFER     /* Secure buffer connected to session, e.g. ES buffer. */
} hi_drv_ssm_buffer_type;

typedef enum {
    HI_SSM_INTENT_WATCH,
    HI_SSM_INTENT_RECORD,
    HI_SSM_INTENT_EXPORT,
    HI_SSM_INTENT_MAX
} hi_ssm_intent;

typedef struct {
    hi_handle module_handle;
} hi_ssm_resource_info;

typedef struct {
    hi_handle                session_handle;
    hi_s32                   dma_buf_handle;
    hi_drv_ssm_buffer_id     buf_id;
    hi_handle                module_handle;
} hi_ssm_buffer_attach_info;

/* driver use this struct */
typedef struct {
    hi_handle               session_handle;
    struct dma_buf         *dma_buf_addr;
    hi_drv_ssm_buffer_id    buf_id;
    hi_handle               module_handle;
} hi_drv_ssm_buf_attach_info;

typedef struct {
    hi_handle               session_handle;  /* session handle, invalid for frame buffer, should be set 0xFFFFFFFF */
    hi_handle               module_handle;   /* module handle which module the buffer will be attached to */
    hi_drv_ssm_buffer_id    buf_id;          /* buffer ID */
    hi_u64                  sec_info_addr;   /* Output parameter: Secure Info Addr, only valid for frame buffer */
} hi_tee_ssm_buf_attach_pre_params;

#ifdef SSM_TEST_SUPPORT
typedef struct {
    hi_handle               session_handle;
    hi_handle               module_handle;
    hi_drv_ssm_buffer_id    buf_id;
    hi_s32                  dma_buf_handle;
} hi_ssm_buffer_check_info;
#endif

hi_s32 hi_drv_ssm_attach_buffer(hi_drv_ssm_buf_attach_info attach_info, hi_u64 *sec_info_addr);

hi_s32 hi_drv_ssm_iommu_config(hi_logic_mod_id mod_id);

hi_s32 ssm_mod_init(hi_void);

hi_void ssm_mod_exit(hi_void);

#endif

