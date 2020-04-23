#ifndef __DRV_OMXVDEC_EXT_H__
#define __DRV_OMXVDEC_EXT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include "hi_type.h"

/* omxvdec msg response types */
#define VDEC_MSG_RESP_BASE              (0x10000)
#define VDEC_MSG_RESP_OPEN              (VDEC_MSG_RESP_BASE + 0x1)
#define VDEC_MSG_RESP_START_DONE        (VDEC_MSG_RESP_BASE + 0x2)
#define VDEC_MSG_RESP_STOP_DONE         (VDEC_MSG_RESP_BASE + 0x3)
#define VDEC_MSG_RESP_PAUSE_DONE        (VDEC_MSG_RESP_BASE + 0x4)
#define VDEC_MSG_RESP_RESUME_DONE       (VDEC_MSG_RESP_BASE + 0x5)
#define VDEC_MSG_RESP_FLUSH_INPUT_DONE  (VDEC_MSG_RESP_BASE + 0x6)
#define VDEC_MSG_RESP_FLUSH_OUTPUT_DONE (VDEC_MSG_RESP_BASE + 0x7)
#define VDEC_MSG_RESP_INPUT_DONE        (VDEC_MSG_RESP_BASE + 0x8)
#define VDEC_MSG_RESP_OUTPUT_DONE       (VDEC_MSG_RESP_BASE + 0x9)
#define VDEC_MSG_RESP_MSG_STOP_DONE     (VDEC_MSG_RESP_BASE + 0xa)

/* omxvdec error code types */
#define VDEC_S_ERR_BASE                 (0x20000)
#define VDEC_ERR_FAIL                   (VDEC_S_ERR_BASE + 0x01)
#define VDEC_ERR_DYNAMIC_ALLOC_FAIL     (VDEC_S_ERR_BASE + 0x02)
#define VDEC_ERR_ILLEGAL_OP             (VDEC_S_ERR_BASE + 0x03)
#define VDEC_ERR_ILLEGAL_PARM           (VDEC_S_ERR_BASE + 0x04)
#define VDEC_ERR_BAD_POINTER            (VDEC_S_ERR_BASE + 0x05)
#define VDEC_ERR_BAD_HANDLE             (VDEC_S_ERR_BASE + 0x06)
#define VDEC_ERR_NOT_SUPPORTED          (VDEC_S_ERR_BASE + 0x07)
#define VDEC_ERR_BAD_STATE              (VDEC_S_ERR_BASE + 0x08)
#define VDEC_ERR_BUSY                   (VDEC_S_ERR_BASE + 0x09)
#define VDEC_ERR_HW_FATAL               (VDEC_S_ERR_BASE + 0x0a)
#define VDEC_ERR_BITSTREAM_ERR          (VDEC_S_ERR_BASE + 0x0b)
#define VDEC_ERR_QEMPTY                 (VDEC_S_ERR_BASE + 0x0c)
#define VDEC_ERR_QFULL                  (VDEC_S_ERR_BASE + 0x0d)
#define VDEC_ERR_INPUT_NOT_PROCESSED    (VDEC_S_ERR_BASE + 0x0e)
#define VDEC_ERR_INDEX_NOMORE           (VDEC_S_ERR_BASE + 0x0f)
#define VDEC_ERR_SYNTAX_ERR             (VDEC_S_ERR_BASE + 0x10)
#define VDEC_ERR_RATIO_NOTZERO          (VDEC_S_ERR_BASE + 0x11)

#define VDEC_EVT_REPORT_BASE            (0x30000)
#define VDEC_EVT_REPORT_IMG_SIZE_CHG	(VDEC_EVT_REPORT_BASE + 0x1)
#define VDEC_EVT_REPORT_FRAME_RATE_CHG	(VDEC_EVT_REPORT_BASE + 0x2)
#define VDEC_EVT_REPORT_SCAN_TYPE_CHG	(VDEC_EVT_REPORT_BASE + 0x3)
#define VDEC_EVT_REPORT_HW_ERROR        (VDEC_EVT_REPORT_BASE + 0x4)
#define VDEC_EVT_REPORT_CROP_RECT_CHG	(VDEC_EVT_REPORT_BASE + 0x5)
#define VDEC_EVT_REPORT_DEC_SIZE_CHG    (VDEC_EVT_REPORT_BASE + 0x6)
#define VDEC_EVT_REPORT_HDR_INFO        (VDEC_EVT_REPORT_BASE + 0x7)


typedef hi_s32  (*fn_omx_acquire_stream)(hi_s32 chan_id, hi_void* stream_data);
typedef hi_s32  (*fn_omx_release_stream)(hi_s32 chan_id, hi_void* stream_data);
typedef hi_s32  (*fn_omx_vdec_event_report)(hi_s32 chan_id, hi_u32 event_id, hi_s32 result, hi_void* priv_data);

typedef struct
{
    fn_omx_acquire_stream      pfn_omx_acquire_stream;
    fn_omx_release_stream      pfn_omx_release_stream;
    fn_omx_vdec_event_report  pfn_omx_vdec_event_report;
} omx_export_func;

hi_s32  drv_omxvdec_mod_init(hi_void);
hi_void drv_omxvdec_mod_exit(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_OMXVDEC_EXT_H__ */

