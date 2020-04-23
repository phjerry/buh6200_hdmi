/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */
#ifndef __VENC_PUBLIC_H__
#define __VENC_PUBLIC_H__

enum {
    VEDU_H264 = 0,
    VEDU_JPGE = 1,
    VEDU_H265 = 2,
};

enum {
    STREAM_LIST,
    STREAM_RECYCLE,
    STREAM_MAX
};

enum {
    VEDU_H264_BASELINE_PROFILE = 0,
    VEDU_H264_MAIN_PROFILE = 1,
    VEDU_H264_EXTENDED_PROFILE = 2,
    VEDU_H264_HIGH_PROFILE = 3,
    VEDU_H264_PROFILE_BUTT = 4
};

enum {
    VENC_GOPMODE_NORMALP = 0,
    VENC_GOPMODE_DUALP = 1,
    VENC_GOPMODE_BIPREDB = 2,
    VENC_GOPMODE_MAX,
};

typedef enum {
    GOP_SINGLE_P = 0,
    GOP_IBBP,
    GOP_2P,
    GOP_BUTT,
} gop_type_enum;

typedef enum {
    PIC_BOT = 0,
    PIC_TOP,
    PIC_FRAME,
    PIC_IDR,
    PIC_INTRA,
    PIC_INTER = 5,  /* single P */
    PIC_INTERSP, /* dual P */
    PIC_INTERBP, /* dual P && big P */
    PIC_BIINTER, /* B slice */
    PIC_INTRA_P, /* Two Pass P */
    PIC_TYPE_BUTT
} pic_type;

#define CONFIG_HI_VENC_RCNREF_SHARE_SUPPORT

#endif  /* __VENC_PUBLIC_H__ */

