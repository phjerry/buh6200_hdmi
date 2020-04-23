/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __EVENT_H__
#define __EVENT_H__

#include "vfmw.h"

/* CNcomment:   有一帧图像进入队列 */
#define REPORT_IMGRDY(chan_id, image_info)                                           \
    do {                                                                             \
        event_report(chan_id, EVNT_NEW_IMAGE, (vfmw_image *)(image_info), sizeof(vfmw_image));   \
    } while (0)

/* CNcomment:   发现user data */
#define REPORT_USRDAT(chan_id, usr_data)                                   \
    do {                                                                   \
        hi_u64 para[1];                                                    \
        para[0] = PTR_UINT64(usr_data);                                  \
        event_report(chan_id, EVNT_USRDAT, (hi_u8 *)para, sizeof(para));   \
    } while (0)

/* CNcomment:   发现一个I帧，准备解码此I帧 */
#define REPORT_FIND_IFRAME(chan_id, stream_size)                                \
    do {                                                                        \
        hi_u32 para[1];                                                         \
        para[0] = stream_size;                                                  \
        event_report(chan_id, EVNT_FIND_IFRAME, (hi_u8 *)para, sizeof(para)); \
    } while (0)

/* CNcomment:   VDM不响应 */
#define REPORT_VDMERR(chan_id)                                    \
    do {                                                          \
        event_report(chan_id, EVNT_VDM_ERR, HI_NULL, 0);     \
    } while (0)

/* CNcomment:   发现不支持的规格 无 */
#define REPORT_UNSUPPORT(chan_id)                                   \
    do {                                                            \
        event_report(chan_id, EVNT_UNSUPPORT, HI_NULL, 0);       \
    } while (0)

/* CNcomment:   码流的语法错误    无 */
#define REPORT_SE_ERR(chan_id)                                    \
    do {                                                          \
        event_report(chan_id, EVNT_SE_ERR, HI_NULL, 0);       \
    } while (0)

/* CNcomment:   发现新的帧结束符    无 */
#define REPORT_MEET_NEWFRM(chan_id)                                \
    do {                                                           \
        event_report(chan_id, EVNT_MEET_NEWFRM, HI_NULL, 0);    \
    } while (0)

/* CNcomment:   码流解析错误 */
#define REPORT_DECSYNTAX_ERR(chan_id)                               \
    do {                                                            \
        event_report(chan_id, EVNT_DECSYNTAX_ERR, HI_NULL, 0);   \
    } while (0)

/* CNcomment:    最后一帧输出结果上报 */
#define REPORT_LAST_FRAME(chan_id, flag)                                       \
    do {                                                                       \
        hi_u32 para[1];                                                        \
        para[0] = (hi_u32)(flag);                                              \
        event_report(chan_id, EVNT_LAST_FRAME, (hi_u8 *)para, sizeof(para));   \
    } while (0)

/* CNcomment:   码流信息上报划分帧存 */
#define REPORT_NEED_ARRANGE(chan_id, frm_num, frm_size, width, height)           \
    do {                                                                         \
        hi_u32 para[4];                                                          \
        para[0] = (hi_u32)(frm_num);                                             \
        para[1] = (hi_u32)(frm_size);                                            \
        para[2] = (hi_u32)(width);                                               \
        para[3] = (hi_u32)(height);                                              \
        event_report(chan_id, EVNT_NEED_ARRANGE, (hi_u8 *)para, sizeof(para));   \
    } while (0)

/* CNcomment:   发现不支持的规格 上报类型及数值 */
#define REPORT_UNSUPPORT_SPEC(chan_id, vfmw_unsupport_spec, data)                  \
    do {                                                                           \
        hi_u32 para[2];                                                            \
        para[0] = vfmw_unsupport_spec;                                             \
        para[1] = data;                                                            \
        event_report(chan_id, EVNT_UNSUPPORT_SPEC, (hi_u8 *)para, sizeof(para));   \
    } while (0)

/* CNcomment:   变分辨率时输出假帧让后级还帧 */
#define REPORT_FAKE_FRAME(chan_id)                               \
    do {                                                         \
        event_report(chan_id, EVNT_FAKE_FRAME, HI_NULL, 0);   \
    } while (0)

#define REPORT_ERR_FRAME(chan_id, frm_type, err_ratio)                                 \
    do {                                                          \
        hi_s8 para[2];                                                               \
        para[0] = frm_type;                                                  \
        para[1] = err_ratio;                                                 \
        event_report(chan_id, EVNT_ERR_FRAME, (hi_s8*)para, sizeof(para));     \
    } while (0)
#define REPORT_REQUIRE_ALLOC_MEM(chan_id, memory)                                                       \
    do {                                                                                                \
        event_report (chan_id, EVNT_ALLOC_MEM, (vfmw_mem_report *)(memory), sizeof(vfmw_mem_report));   \
    } while (0)
#define REPORT_REQUIRE_RELEASE_MEM(chan_id, memory)                                                       \
    do {                                                                                                  \
        event_report (chan_id, EVNT_RELEASE_MEM, (vfmw_mem_report *)(memory), sizeof(vfmw_mem_report));   \
    } while (0)
#define REPORT_REQUIRE_RELEASE_BURST_MEM(chan_id, private_id, private_mask)           \
    do {                                                                              \
        hi_s64 para[2];                                                               \
        para[0] = private_id;                                                         \
        para[1] = private_mask;                                                       \
        event_report(chan_id, EVNT_RELEASE_BURST_MEM, (hi_u8 *)para, sizeof(para));   \
    } while (0)

#define REPORT_REQUIRE_CANCEL_ALLOC(chan_id, private_id, private_mask)           \
    do {                                                                         \
        hi_s64 para[2];                                                          \
        para[0] = private_id;                                                    \
        para[1] = private_mask;                                                  \
        event_report(chan_id, EVNT_CANCEL_ALLOC, (hi_u8 *)para, sizeof(para));   \
    } while (0)

#define REPORT_SET_ADDRESS_IN(chan_id, physical_addr)                              \
    do {                                                                           \
        hi_s64 para[1];                                                            \
        para[0] = physical_addr;                                                   \
        event_report(chan_id, EVNT_SET_ADDRESS_IN, (hi_u8 *)para, sizeof(para));   \
    } while (0)

#define REPORT_FIRST_PTS(chan_id, pts)                              \
    do {                                                                           \
        hi_s64 para[1];                                                            \
        para[0] = pts;                                                   \
        event_report(chan_id, EVNT_FIRST_PTS, (hi_u8 *)para, sizeof(para));   \
    } while (0)

#define REPORT_SECOND_PTS(chan_id, pts)                              \
    do {                                                                           \
        hi_s64 para[1];                                                            \
        para[0] = pts;                                                   \
        event_report(chan_id, EVNT_SECOND_PTS, (hi_u8 *)para, sizeof(para));   \
    } while (0)

#define REPORT_ERR_STD(chan_id, std)                              \
    do {                                                                           \
        hi_u32 para[1];                                                            \
        para[0] = std;                                                   \
        event_report(chan_id, EVNT_VIDSTD_ERROR, (hi_u8 *)para, sizeof(para));   \
    } while (0)

/* CNcomment: 变分辨率重新分割帧存上报 */
#define REPORT_RESOLUTION_CHANGE(chan_id, size, num, width, height, stride, bitdepth, corp_w, crop_h) \
    do {                                                                             \
        hi_u32  para[8];                                                             \
        para[0] = (size);                                                            \
        para[1] = (num);                                                             \
        para[2] = (width);                                                           \
        para[3] = (height);                                                          \
        para[4] = (stride);                                                          \
        para[5] = (bitdepth);                                                        \
        para[6] = (corp_w);                                                          \
        para[7] = (crop_h);                                                           \
        event_report(chan_id, EVNT_RESOLUTION_CHANGE, (hi_u8*)para, sizeof(para));   \
    } while (0)

#define REPORT_HDR_INFO(chan_id, full_range_flag, colour_primaries, matrix_coeffs, transfer_characteristics) \
    do{                                                         \
        hi_u32  para[4];                                    \
        para[0] = full_range_flag;                          \
        para[1] = colour_primaries;                         \
        para[2] = matrix_coeffs;                            \
        para[3] = transfer_characteristics;                 \
        event_report(chan_id, EVNT_HDR_INFO, (hi_u8*)para, sizeof(para));  \
    } while (0)

hi_void event_set_callback(fun_vfmw_event_report report);
hi_void event_report(hi_s32, hi_s32, hi_void *, hi_s32);
hi_s32 event_set_map(hi_s32 chan_id, hi_u32 event_map);

#endif /* __PUBLIC_H__ */
