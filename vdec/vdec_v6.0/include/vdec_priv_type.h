
#ifndef __VDEC_PRIV_TYPE_H__
#define __VDEC_PRIV_TYPE_H__

#include "hi_vdec_type.h"

#define VDEC_VERSION "2019112100"

#define VDEC_MAX_CHAN_NUM  16
#define VDEC_DFT_MAX_RAW_NUM 4096

#define VDEC_MODULE_NAME   "VDEC"

#define INTF

#define VDEC_ASSERT(cond) \
do { \
    if (!cond) { \
        VDEC_PRINT_ERR("conditon:%s is not matched.\n", #cond); \
        return; \
    } \
} while (0)

#define VDEC_ASSERT_RET(cond) \
do { \
    if (!(cond)) { \
        VDEC_PRINT_ERR("conditon:%s is not matched.\n", #cond); \
        return HI_FAILURE; \
    } \
} while (0)

#define VDEC_CHECK_CHAN_ID(handle) \
do { \
    if ((handle) < 0 || (handle) >= VDEC_MAX_CHAN_NUM) { \
        VDEC_PRINT_ERR("handle %d invalid.\n", handle); \
        return HI_FAILURE; \
    } \
} while (0)

#define VDEC_POS() VDEC_PRINT_ERR("location\n")

#define VDEC_MAKE_HANDLE(chan_id) HI_HANDLE_INIT(HI_ID_VDEC, 0, chan_id)
#define VDEC_GET_CHANID_BY_HANDLE(handle) HI_HANDLE_GET_CHAN_ID(handle)

#define VDEC_UINT64_PTR(ptr) ((hi_void *)(hi_uintptr_t)(ptr))
#define VDEC_PTR_UINT64(ptr) ((hi_u64)(hi_uintptr_t)(ptr))

#define VDEC_INVALID_PTS   ((hi_s64)(-1))

typedef enum {
    VDEC_YUV_NONE = 0,
    VDEC_YUV_SP400,
    VDEC_YUV_SP411,
    VDEC_YUV_SP420,
    VDEC_YUV_SP422_1X2,
    VDEC_YUV_SP422_2X1,
    VDEC_YUV_SP444,
    VDEC_YUV_P400,
    VDEC_YUV_P411,
    VDEC_YUV_P420,
    VDEC_YUV_P422_1X2,
    VDEC_YUV_P422_2X1,
    VDEC_YUV_P444,
    VDEC_YUV_P410,
    VDEC_YUV_MAX
} vdec_yuv_fmt;

typedef struct {
    hi_bool is_frame_valid;
    hi_u32 width;
    hi_u32 height;
    hi_u32 bit_depth;
    hi_u32 luma_stride;
    hi_u32 chrom_stride;
    hi_u64 phy_addr;
    hi_u32 line_num_size;
    hi_u64 chrom_offset;
    hi_u64 line_num_offset;
    hi_s64 pts;
    hi_mem_handle_t mem_handle;
    hi_mem_size_t mem_size;
    vdec_yuv_fmt fmt;
} vdec_usrdec_frame;

typedef enum {
    VDEC_PRIVATE_CMD_START = HI_VDEC_CMD_EXT_START,
    VDEC_PRIVATE_CMD_GET_STREAM,
    VDEC_PRIVATE_CMD_PUT_STREAM,
    VDEC_PRIVATE_CMD_GET_FRAME,
    VDEC_PRIVATE_CMD_PUT_FRAME,
    VDEC_PRIVATE_CMD_BIND_ES_USR_ADDR,
    VDEC_PRIVATE_CMD_END = HI_VDEC_CMD_EXT_END,
} vdec_private_cmd;

#endif

