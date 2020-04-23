/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: audsp aoe header file
 * Author: Audio
 * Create: 2012-05-30
 * Notes: NA
 * History: 2019-05-30 CSEC Rectification
 */

#ifndef __HI_AUDSP_AOE_H__
#define __HI_AUDSP_AOE_H__

#include "hi_audsp_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define AOE_RUNNING_FLAG 0xa5a5a5a5

/* defines the  status of an ADSP. */
typedef enum {
    ADSP_SYSCOM_CMD_START = 0,  /* start */ /* CNcomment: 运行 */
    ADSP_SYSCOM_CMD_STOP,       /* stop */ /* CNcomment: 停止 */

    ADSP_SYSCOM_CMD_MAX
} adsp_syscom_cmd;

typedef enum {
    ADSP_SYSCOM_CMD_DONE = 0,
    ADSP_SYSCOM_CMD_ERR_START_FAIL = 1,
    ADSP_SYSCOM_CMD_ERR_STOP_FAIL = 2,
    ADSP_SYSCOM_CMD_ERR_CREATE_FAIL = 3,
    ADSP_SYSCOM_CMD_ERR_UNKNOWCMD = 4,
    ADSP_SYSCOM_CMD_ERR_TIMEOUT = 5,
    ADSP_SYSCOM_CMD_ERR_MAX
} adsp_syscom_cmd_ret;

typedef enum {
    ADSP_AOE_CMD_START = 0,  /* start */ /* CNcomment: 运行 */
    ADSP_AOE_CMD_STOP,       /* stop */ /* CNcomment: 停止 */
    ADSP_AOE_CMD_MAX
} adsp_aoesys_cmd;

typedef enum {
    ADSP_AOE_CMD_DONE = 0,
    ADSP_AOE_CMD_ERR_START_FAIL = 1,
    ADSP_AOE_CMD_ERR_STOP_FAIL = 2,
    ADSP_AOE_CMD_ERR_CREATE_FAIL = 3,
    ADSP_AOE_CMD_ERR_UNKNOWCMD = 4,
    ADSP_AOE_CMD_ERR_TIMEOUT = 5,
    ADSP_AOE_CMD_ERR_MAX
} adsp_aoesys_cmd_ret;

/* ADSP common command */
typedef enum {
    ADSP_CMD_START = 0, /* start */ /* CNcomment: 运行 */
    ADSP_CMD_STOP,      /* stop */ /* CNcomment: 停止 */
    ADSP_CMD_PAUSE,
    ADSP_CMD_FLUSH,
    ADSP_CMD_AIP_ADD_MUTE,
    ADSP_CMD_AIP_SET_SPEED,

    ADSP_CMD_MAX
} adsp_cmd_id;

typedef enum {
    ADSP_CMD_DONE = 0,
    ADSP_CMD_ERR_START_FAIL = 1,
    ADSP_CMD_ERR_STOP_FAIL = 2,
    ADSP_CMD_ERR_CREATE_FAIL = 3,
    ADSP_CMD_ERR_UNKNOWCMD = 4,
    ADSP_CMD_ERR_TIMEOUT = 5,
    ADSP_CMD_ERR_MAX
} adsp_cmd_ret;

/* ADSP command Type */
typedef enum {
    ADSP_CMD_TYPE_AIP = 0,
    ADSP_CMD_TYPE_ENGINE,
    ADSP_CMD_TYPE_AOP,

    ADSP_CMD_TYPE_MAX
} adsp_cmd_type;

typedef enum {
    AOE_FORMAT_LPCM = 0, /* linear pcm */ /* CNcomment: LPCM格式 */
    AOE_FORMAT_IEC61937 = 1, /* iec61937 raw data */ /* CNcomment: iec61937协议透传数据 */
    AOE_FORMAT_IEC61937_COMP = 2, /* compressed iec61937 raw data */ /* CNcomment: 经压缩后的iec61937协议透传数据 */
    AOE_FORMAT_MAX = 3
} aoe_data_format;

#define AOE_REG_LENGTH (0xB00)  /* 0xB00 reg for aoe */

#define AOE_MAX_ENGINE_NUM 6
#define AOE_MAX_AOP_NUM    8
#define AOE_MAX_AIP_NUM    8
#define AOE_MAX_CAST_NUM   4

#define AOE_COM_REG_OFFSET    0x0000
#define AOE_ENGINE_REG_OFFSET 0x0040
#define AOE_AIP_REG_OFFSET    0x0340
#define AOE_AOP_REG_OFFSET    0x0640
#define AOE_CAST_REG_OFFSET   0x0A40

#define AOE_COM_REG_BANDSIZE    sizeof(aoe_regs_type)
#define AOE_AIP_REG_BANDSIZE    sizeof(aip_regs_type)
#define AOE_ENGINE_REG_BANDSIZE sizeof(mixer_regs_type)
#define AOE_AOP_REG_BANDSIZE    sizeof(aop_regs_type)
#define AOE_CAST_REG_BANDSIZE   sizeof(cast_regs_type)

#define AOE_COM_REG_BASE (DSP0_SHARESRAM_BASEADDR + AOE_COM_REG_OFFSET)

#define AOE_AIP_VOL_6DB   (0x7f)
#define AOE_AIP_VOL_0DB   (0x79)
#define AOE_AIP_VOL_INFDB (0x28)
#define AOE_AIP_VOL_MAXDB (AOE_AIP_VOL_6DB)
#define AOE_AIP_VOL_MINDB (AOE_AIP_VOL_INFDB)

#define AIP_FIFO_LATENCYMS_DEFAULT     (40)
#define AIP_FIFO_BUFFERSIZE_MS_DEFAULT (80)

#define AIP_CHANNEL_MODE_STEREO       0x0
#define AIP_CHANNEL_MODE_DOUBLE_MONO  0x1
#define AIP_CHANNEL_MODE_DOUBLE_LEFT  0x2
#define AIP_CHANNEL_MODE_DOUBLE_RIGHT 0x3
#define AIP_CHANNEL_MODE_EXCHANGE     0x4
#define AIP_CHANNEL_MODE_ONLY_RIGHT   0x5
#define AIP_CHANNEL_MODE_ONLY_LEFT    0x6
#define AIP_CHANNEL_MODE_MUTED        0x7
#define AIP_CHANNEL_MODE_MAX          0x8

#define AOE_AEF_MAX_CHAN_NUM 4

#define AOE_ENGINE_PROC_SAMPLES 256 /* the samples for each process of engine */
#define AOE_AEF_PERIODBUF_NUM   6   /* aef period buffer number(the same for inbuf & outbuf) */
#define AOE_FIFODELAY_FRAME_NUM 16  /* the frame num of fifo delay(consider of all aef delay) */

typedef enum {
    AOE_AIP_TYPE_PCM_ALSA = 0,  /* ALSA pcm */ /* CNcomment: alsa pcm */
    AOE_AIP_TYPE_PCM_SLAVE,     /* 2.0 normal pcm for slave track */ /* CNcomment: 2.0 普通 PCM */
    AOE_AIP_TYPE_PCM_DMX,       /* downmixer pcm for master track */ /* CNcomment: 下混pcm流 */
    AOE_AIP_TYPE_LBR,
    AOE_AIP_TYPE_HBR,
    AOE_AIP_TYPE_PCM_MAX
} aoe_aip_type;

typedef enum {
    AOE_AIP_CMD_START = 0, /* start */ /* CNcomment: 运行 */
    AOE_AIP_CMD_STOP,      /* stop */ /* CNcomment: 停止 */
    AOE_AIP_CMD_PAUSE,
    AOE_AIP_CMD_FLUSH,

    AOE_AIP_CMD_MAX
} aoe_aip_cmd;

typedef enum {
    AOE_AIP_CMD_DONE = 0,
    AOE_AIP_CMD_ERR_START_FAIL = 1,
    AOE_AIP_CMD_ERR_STOP_FAIL = 2,
    AOE_AIP_CMD_ERR_CREATE_FAIL = 3,
    AOE_AIP_CMD_ERR_UNKNOWCMD = 4,
    AOE_AIP_CMD_ERR_TIMEOUT = 5,

    AOE_AIP_CMD_ERR_MAX
} aoe_aip_cmd_ret;

/* defines the  status of an ENGINE. */
typedef enum {
    AOE_ENGINE_CMD_START = 0, /* start */ /* CNcomment: 运行 */
    AOE_ENGINE_CMD_STOP,      /* stop */ /* CNcomment: 停止 */

    AOE_ENGINE_CMD_MAX
} aoe_engine_cmd;

typedef enum {
    AOE_ENGINE_CMD_DONE = 0,
    AOE_ENGINE_CMD_ERR_START_FAIL = 1,
    AOE_ENGINE_CMD_ERR_STOP_FAIL = 2,
    AOE_ENGINE_CMD_ERR_CREATE_FAIL = 3,
    AOE_ENGINE_CMD_ERR_UNKNOWCMD = 4,
    AOE_ENGINE_CMD_ERR_TIMEOUT = 5,
    AOE_ENGINE_CMD_ERR_MAX
} aoe_engine_cmd_ret;

/* defines the  status of an AOP. */
typedef enum {
    AOE_AOP_CMD_START = 0, /* start */ /* CNcomment: 运行 */
    AOE_AOP_CMD_STOP,      /* stop */ /* CNcomment: 停止 */

    AOE_AOP_CMD_MAX
} aoe_aop_cmd;

typedef enum {
    AOE_AOP_CMD_DONE = 0,
    AOE_AOP_CMD_ERR_START_FAIL = 1,
    AOE_AOP_CMD_ERR_STOP_FAIL = 2,
    AOE_AOP_CMD_ERR_CREATE_FAIL = 3,
    AOE_AOP_CMD_ERR_UNKNOWCMD = 4,
    AOE_AOP_CMD_ERR_TIMEOUT = 5,
    AOE_AOP_CMD_ERR_MAX
} aoe_aop_cmd_ret;

/* define the union U_AOE_STATUS1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int excute_timeout_cnt : 16;   /* [15..0] */
        unsigned int schedule_timeout_cnt : 16; /* [31..16] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AOE_STATUS1;

/* define the union U_ENGINE_ATTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int precision : 2;      /* [1..0] */
        unsigned int ch : 2;             /* [3..2] */
        unsigned int fs : 4;             /* [7..4] */
        unsigned int format : 2;         /* [9..8] */
        unsigned int aef_delay : 6;      /* [15..10] */
        unsigned int geq_bandnum : 4;    /* [19..16] */
        unsigned int aef_support_mc : 1; /* [20] */
        unsigned int reserved_1 : 11;    /* [31..21] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ENGINE_ATTR;

/* define the union U_ENGINE_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int cmd : 2;                 /* [1..0] */
        unsigned int cmd_done : 1;            /* [2] */
        unsigned int cmd_return_value : 4;    /* [6..3] */
        unsigned int ena_profile : 1;         /* [7] */
        unsigned int ena_geq : 1;             /* [8] */
        unsigned int geq_change_attr : 1;     /* [9] */
        unsigned int ena_sendmute : 1;        /* [10] */
        unsigned int ena_avc : 1;             /* [11] */
        unsigned int avc_change_attr : 1;     /* [12] */
        unsigned int ena_continue_output : 1; /* [13] */
        unsigned int ena_output_atmos : 1;    /* [14] */
        unsigned int reserved_2 : 17;         /* [31..15] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ENGINE_CTRL;

/* define the union U_ENGINE_MIX_SRC */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int aip_fifo_ena : 16; /* [15..0] */
        unsigned int reserved_3 : 16;   /* [31..16] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ENGINE_MIX_SRC;

/* define the union U_ENGINE_ROU_DST */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int aop_buf_ena : 16; /* [15..0] */
        unsigned int reserved_4 : 16;  /* [31..16] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ENGINE_ROU_DST;

/* define the union U_ENGINE_PP_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int pp_cmd : 2;              /* [1..0] */
        unsigned int pp_cmd_done : 1;         /* [2] */
        unsigned int dst_pp_enable_flag : 16; /* [18..3] */
        unsigned int reserved_5 : 13;         /* [31..19] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ENGINE_PP_CTRL;

/* define the union U_ENGINE_STATUS0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int aver_mcps : 8;   /* [7..0] */
        unsigned int peak_mcps : 8;   /* [15..8] */
        unsigned int reserved_7 : 16; /* [31..16] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ENGINE_STATUS0;

/* define the union U_ENGINE_ATT_AEF */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int aef_att_ena : 16; /* [15..0] */
        unsigned int reserved_8 : 16;  /* [31..16] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ENGINE_ATT_AEF;

/* define the union U_ENGINE_AVC_TIME_ATTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int attack_time : 13;  /* [12..0] */
        unsigned int release_time : 13; /* [25..13] */
        unsigned int reserved_9 : 6;    /* [31..26] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ENGINE_AVC_TIME_ATTR;

/* define the union U_ENGINE_AVC_LEVEL_ATTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int limiter_level : 6;   /* [5..0] */
        unsigned int threshold_level : 6; /* [11..6] */
        unsigned int gain : 4;            /* [15..12] */
        unsigned int reserved_10 : 16;    /* [31..16] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ENGINE_AVC_LEVEL_ATTR;

/* define the union U_ENGINE_AEF_BUF_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int periond_size : 24; /* [23..0] */
        unsigned int periond_num : 4;   /* [27..24] */
        unsigned int reserved_13 : 4;   /* [31..28] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ENGINE_AEF_BUF_SIZE;

/* define the union U_ENGINE_AEF_BUF_WRITEIDX */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int periond_write_idx : 8;  /* [7..0] */
        unsigned int periond_write_wrap : 1; /* [8] */
        unsigned int reserved_14 : 23;       /* [31..9] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ENGINE_AEF_BUF_WRITEIDX;

/* define the union U_ENGINE_AEF_BUF_READIDX */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int periond_read_idx : 8;  /* [7..0] */
        unsigned int periond_read_wrap : 1; /* [8] */
        unsigned int reserved_15 : 23;      /* [31..9] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_ENGINE_AEF_BUF_READIDX;

/* define the union U_AIP_BUFF_ATTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int aip_precision : 2; /* [1..0] */
        unsigned int aip_ch : 3;        /* [4..2] */
        unsigned int aip_fs : 4;        /* [8..5] */
        unsigned int aip_format : 2;    /* [10..9] */
        unsigned int aip_pcm_type : 3;  /* [13..11] */
        unsigned int aip_priority : 1;  /* [14] */
        unsigned int reserved_21 : 17;  /* [31..15] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIP_BUFF_ATTR;

/* define the union U_AIP_FIFO_ATTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int fifo_precision : 2;     /* [1..0] */
        unsigned int fifo_ch : 2;            /* [3..2] */
        unsigned int fifo_fs : 4;            /* [7..4] */
        unsigned int fifo_format : 2;        /* [9..8] */
        unsigned int fifo_bypass : 1;        /* [10] */
        unsigned int fifo_latency : 7;       /* [17..11] */
        unsigned int fifo_latency_real : 12; /* [29..18] */
        unsigned int reserved_22 : 2;        /* [31..30] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIP_FIFO_ATTR;

/* define the union U_AIP_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int cmd : 2;              /* [1..0] */
        unsigned int reserved_23 : 1;      /* [2] */
        unsigned int cmd_done : 1;         /* [3] */
        unsigned int cmd_return_value : 4; /* [7..4] */
        unsigned int volume : 7;           /* [14..8] */
        unsigned int fade_en : 1;          /* [15] */
        unsigned int fade_in_rate : 4;     /* [19..16] */
        unsigned int fade_out_rate : 4;    /* [23..20] */
        unsigned int dst_fs_adj_step : 5;  /* [28..24] */
        unsigned int dst_fs_adj_dir : 2;   /* [30..29] */
        unsigned int ena_profile : 1;      /* [31] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIP_CTRL;

/* define the union U_AIP_ADD_MUTE_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int cmd : 2;              /* [1..0] */
        unsigned int cmd_done : 1;         /* [2] */
        unsigned int cmd_return_value : 4; /* [6..3] */
        unsigned int add_mute_ms : 25;     /* [31..7] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIP_ADD_MUTE_CTRL;

/* define the union U_AIP_SRC_ATTR_EXT */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int fs_ext : 24;     /* [23..0] */
        unsigned int reserved_24 : 8; /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIP_SRC_ATTR_EXT;

/* define the union U_AIP_BUF_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int buff_size : 24;  /* [23..0] */
        unsigned int buff_flag : 1;   /* [24] */
        unsigned int reserved_27 : 7; /* [31..25] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIP_BUF_SIZE;

/* define the union U_AIP_BUF_TRANS_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int tx_trans_size : 24; /* [23..0] */
        unsigned int reserved_32 : 8;    /* [31..24] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIP_BUF_TRANS_SIZE;

/* define the union U_AIP_EXT_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int volume_l : 8;          /* [7..0] */
        unsigned int volume_r : 8;          /* [15..8] */
        unsigned int mute : 1;              /* [16] */
        unsigned int channel_mode : 3;      /* [19..17] */
        unsigned int dre_dither_decbps : 3; /* [22..20] */
        unsigned int dre : 2;               /* [24..23] */
        unsigned int dre_thrhd : 7;         /* [31..25] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIP_EXT_CTRL;

/* define the union U_AIP_STATUS0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int aver_mcps : 8;      /* [7..0] */
        unsigned int peak_mcps : 8;      /* [15..8] */
        unsigned int track_delayms : 10; /* [25..16] */
        unsigned int reserved_34 : 6;    /* [31..26] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIP_STATUS0;

/* define the union U_AIP_FIFO_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int aip_fifo_size : 16;          /* [15..0] */
        unsigned int aip_fifo_underflow_cnt : 16; /* [31..16] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIP_FIFO_SIZE;

/* define the union U_AIP_EXT2_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int prescale_int : 8;     /* [7..0] */
        unsigned int prescale_dec : 3;     /* [10..8] */
        unsigned int prescale_dec_dir : 1; /* [11] */
        unsigned int dest_aop : 4;         /* [15..12] */
        unsigned int skip_ms : 8;          /* [23..16] */
        unsigned int is_duplicate : 1;     /* [24] */
        unsigned int reserved_35 : 7;      /* [31..25] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIP_EXT2_CTRL;

/* define the union U_AIP_EXT3_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int start_threshold : 10;  /* [9..0] */
        unsigned int resume_threshold : 10; /* [19..10] */
        unsigned int eos : 1;               /* [20] */
        unsigned int reserved : 11;         /* [31..21] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AIP_EXT3_CTRL;

/* define the union U_AOP_BUFF_ATTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int buf_precision : 2; /* [1..0] */
        unsigned int buf_ch : 2;        /* [3..2] */
        unsigned int buf_fs : 4;        /* [7..4] */
        unsigned int buf_format : 2;    /* [9..8] */
        unsigned int buf_priority : 1;  /* [10] */
        unsigned int buf_delay : 9;     /* [19..11] */
        unsigned int buf_latency : 12;  /* [31..20] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AOP_BUFF_ATTR;

/* define the union U_AOP_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int cmd : 2;              /* [1..0] */
        unsigned int cmd_done : 1;         /* [2] */
        unsigned int cmd_return_value : 4; /* [6..3] */
        unsigned int ena_profile : 1;      /* [7] */
        unsigned int ena_aef_bypass : 1;   /* [8] */
        unsigned int ena_drc : 1;          /* [9] */
        unsigned int ena_ad : 1;           /* [10] */
        unsigned int ena_peq : 1;          /* [11] */
        unsigned int peq_bandnum : 4;      /* [15..12] */
        unsigned int peq_change_attr : 1;  /* [16] */
        unsigned int drc_change_attr : 1;  /* [17] */
        unsigned int ena_add_mute : 1;     /* [18] */
        unsigned int cast_flag : 1;        /* [19] */
        unsigned int audiodelay : 1;       /* [20] */
        unsigned int realdelayms : 9;      /* [29..21] */
        unsigned int reserved_38 : 2;      /* [31..30] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AOP_CTRL;

/* define the union U_AOP_STATUS0 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int aver_mcps : 8;    /* [7..0] */
        unsigned int peak_mcps : 8;    /* [15..8] */
        unsigned int reserved_39 : 16; /* [31..16] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AOP_STATUS0;

/* define the union U_AOP_EXT_CTRL */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int volume_l : 8;        /* [7..0] */
        unsigned int volume_r : 8;        /* [15..8] */
        unsigned int volumedec_l : 3;     /* [18..16] */
        unsigned int volumedec_dir_l : 1; /* [19] */
        unsigned int volumedec_r : 3;     /* [22..20] */
        unsigned int volumedec_dir_r : 1; /* [23] */
        unsigned int balance_val : 6;     /* [29..24] */
        unsigned int balance_dir : 1;     /* [30] */
        unsigned int mute : 1;            /* [31] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AOP_EXT_CTRL;

/* define the union U_AOP_BUF_SIZE */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int buff_size : 24;         /* [23..0] */
        unsigned int buff_flag : 1;          /* [24] */
        unsigned int buff_underflow_cnt : 7; /* [31..25] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AOP_BUF_SIZE;

/* define the union U_AOP_DRC_TIME_ATTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int attack_time : 13;  /* [12..0] */
        unsigned int release_time : 13; /* [25..13] */
        unsigned int reserved_40 : 6;   /* [31..26] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AOP_DRC_TIME_ATTR;

/* define the union U_AOP_DRC_LEVEL_ATTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int start_level : 7;   /* [6..0] */
        unsigned int targert_level : 7; /* [13..7] */
        unsigned int reserved_41 : 18;  /* [31..14] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AOP_DRC_LEVEL_ATTR;

/* define the union U_AOP_PEQ_BAND_TYPE1 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int band1_type : 4; /* [3..0] */
        unsigned int band2_type : 4; /* [7..4] */
        unsigned int band3_type : 4; /* [11..8] */
        unsigned int band4_type : 4; /* [15..12] */
        unsigned int band5_type : 4; /* [19..16] */
        unsigned int band6_type : 4; /* [23..20] */
        unsigned int band7_type : 4; /* [27..24] */
        unsigned int band8_type : 4; /* [31..28] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AOP_PEQ_BAND_TYPE1;

/* define the union U_AOP_PEQ_BAND_TYPE2 */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int band9_type : 4;   /* [3..0] */
        unsigned int band10_type : 4;  /* [7..4] */
        unsigned int reserved_42 : 24; /* [31..8] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_AOP_PEQ_BAND_TYPE2;

/* define the union U_EQ_BAND_ATTR */
typedef union {
    /* define the struct bits */
    struct {
        unsigned int gain_value : 7; /* [6..0] */
        unsigned int gain_dir : 1;   /* [7] */
        unsigned int q_value : 8;    /* [15..8] */
        unsigned int freq : 16;      /* [31..16] */
    } bits;

    /* define an unsigned member */
    unsigned int u32;
} U_EQ_BAND_ATTR;

typedef struct {
    aoe_addr_type addr;
    unsigned int  size;
    unsigned int  write;
    unsigned int  read;
} cmd_fifo_reg;

#define AOE_COM_REG_RESERVED_NUM 7

/* define the global struct */
typedef struct {
    volatile unsigned int AOE_MAGIC;
    volatile unsigned int AOE_VERSION;
    volatile unsigned int AOE_STATUS0;
    volatile U_AOE_STATUS1 AOE_STATUS1;
    cmd_fifo_reg          aoe_cmd_fifo;
    volatile unsigned int aoe_reserved[AOE_COM_REG_RESERVED_NUM];
} aoe_regs_type;

/* MIXER struct */
typedef struct {
    volatile U_ENGINE_ATTR ENGINE_ATTR;
    volatile U_ENGINE_CTRL ENGINE_CTRL;
    volatile U_ENGINE_MIX_SRC ENGINE_MIX_SRC;
    volatile U_ENGINE_ROU_DST ENGINE_ROU_DST;
    volatile U_ENGINE_PP_CTRL ENGINE_PP_CTRL;
    volatile unsigned int ENGINE_PP_ADDR;
    volatile U_ENGINE_STATUS0 ENGINE_STATUS0;
    volatile U_ENGINE_ATT_AEF ENGINE_ATT_AEF;
    volatile U_EQ_BAND_ATTR ENGINE_GEQ_BAND1_ATTR;
    volatile U_EQ_BAND_ATTR ENGINE_GEQ_BAND2_ATTR;
    volatile U_EQ_BAND_ATTR ENGINE_GEQ_BAND3_ATTR;
    volatile U_EQ_BAND_ATTR ENGINE_GEQ_BAND4_ATTR;
    volatile U_EQ_BAND_ATTR ENGINE_GEQ_BAND5_ATTR;
    volatile U_EQ_BAND_ATTR ENGINE_GEQ_BAND6_ATTR;
    volatile U_EQ_BAND_ATTR ENGINE_GEQ_BAND7_ATTR;
    volatile U_EQ_BAND_ATTR ENGINE_GEQ_BAND8_ATTR;
    volatile U_EQ_BAND_ATTR ENGINE_GEQ_BAND9_ATTR;
    volatile U_EQ_BAND_ATTR ENGINE_GEQ_BAND10_ATTR;
    volatile U_ENGINE_AVC_TIME_ATTR ENGINE_AVC_TIME_ATTR;
    volatile U_ENGINE_AVC_LEVEL_ATTR ENGINE_AVC_LEVEL_ATTR;
    volatile unsigned int ENGINE_RESERVED[0x80 / 4 - 20];
} mixer_regs_type, engine_regs_type;

/* AIP struct */
typedef struct {
    volatile U_AIP_BUFF_ATTR AIP_BUFF_ATTR;
    volatile U_AIP_FIFO_ATTR AIP_FIFO_ATTR;
    volatile U_AIP_CTRL AIP_CTRL;
    volatile U_AIP_SRC_ATTR_EXT AIP_SRC_ATTR_EXT;
    aoe_addr_type AIP_BUF_ADDR;
    volatile U_AIP_BUF_SIZE AIP_BUF_SIZE;
    aoe_addr_type AIP_BUF_WPTR;
    aoe_addr_type AIP_BUF_RPTR;
    volatile U_AIP_BUF_TRANS_SIZE AIP_BUF_TRANS_SIZE;
    volatile U_AIP_EXT_CTRL AIP_EXT_CTRL;
    volatile unsigned int AIP_BUF_PHYADDR;
    volatile U_AIP_STATUS0 AIP_STATUS0;
    volatile unsigned int AIP_FIFO_ADDR;
    volatile U_AIP_FIFO_SIZE AIP_FIFO_SIZE;
    volatile unsigned int AIP_FIFO_WPTR;
    volatile unsigned int AIP_FIFO_RPTR;
    volatile U_AIP_EXT2_CTRL AIP_EXT2_CTRL;
    volatile unsigned int AIP_FSADJ_CNT;
    volatile U_AIP_EXT3_CTRL AIP_EXT3_CTRL;
    volatile unsigned int AIP_ADD_MUTE_CNT;
    volatile U_AIP_ADD_MUTE_CTRL AIP_ADD_MUTE_CTRL;
    /* volatile unsigned int AIP_RESERVED[0x60 / 4 - 23]; max = 24, aip register is full */
} aip_regs_type;

/* AOP struct */
typedef struct {
    volatile U_AOP_BUFF_ATTR AOP_BUFF_ATTR;
    volatile U_AOP_CTRL AOP_CTRL;
    volatile U_AOP_STATUS0 AOP_STATUS0;
    volatile U_AOP_EXT_CTRL AOP_EXT_CTRL;
    aoe_addr_type AOP_BUF_ADDR;
    volatile U_AOP_BUF_SIZE AOP_BUF_SIZE;
    aoe_addr_type AOP_BUF_WPTR;
    aoe_addr_type AOP_BUF_RPTR;
    volatile U_AOP_DRC_TIME_ATTR AOP_DRC_TIME_ATTR;
    volatile U_AOP_DRC_LEVEL_ATTR AOP_DRC_LEVEL_ATTR;
    volatile U_AOP_PEQ_BAND_TYPE1 AOP_PEQ_BAND_TYPE1;
    volatile U_AOP_PEQ_BAND_TYPE2 AOP_PEQ_BAND_TYPE2;
    volatile U_EQ_BAND_ATTR AOP_PEQ_BAND1_ATTR;
    volatile U_EQ_BAND_ATTR AOP_PEQ_BAND2_ATTR;
    volatile U_EQ_BAND_ATTR AOP_PEQ_BAND3_ATTR;
    volatile U_EQ_BAND_ATTR AOP_PEQ_BAND4_ATTR;
    volatile U_EQ_BAND_ATTR AOP_PEQ_BAND5_ATTR;
    volatile U_EQ_BAND_ATTR AOP_PEQ_BAND6_ATTR;
    volatile U_EQ_BAND_ATTR AOP_PEQ_BAND7_ATTR;
    volatile U_EQ_BAND_ATTR AOP_PEQ_BAND8_ATTR;
    volatile U_EQ_BAND_ATTR AOP_PEQ_BAND9_ATTR;
    volatile U_EQ_BAND_ATTR AOP_PEQ_BAND10_ATTR;
    volatile unsigned int AOP_ADDMUTE_CNT;
    volatile unsigned int AOP_RESERVED[0x80 / 4 - 26];
} aop_regs_type;

typedef struct {
    volatile unsigned int CAST_AOP_ID;
    volatile unsigned int CAST_FIFO_SIZE;
    volatile unsigned int CAST_FIFO_WPTR;
    volatile unsigned int CAST_FIFO_RPTR;
    volatile unsigned int CAST_FIFO_TRY_CNT;
    volatile unsigned int CAST_FIFO_OK_CNT;
    volatile unsigned int CAST_FIFO_FORCE_READ_CNT;
    volatile unsigned int CAST_BUF_WRITEOK_CNT;
    volatile unsigned int CAST_BUF_WRITEMUTE_CNT;
    volatile unsigned int CAST_BUF_OVERWRITE_CNT;
    volatile unsigned int CAST_BUF_WRITETOTAL_CNT;
    volatile unsigned int CAST_RESERVED0;
} cast_regs_type;

hi_s32 aoe_sw_engine_create(hi_void *dsp_reg);
hi_s32 aoe_sw_engine_destory(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
