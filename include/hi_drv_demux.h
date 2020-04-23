/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: Driver define for HiPVRV200
 * Author: sdk
 * Create: 2009-09-27
 */

#ifndef __HI_DRV_DEMUX_H__
#define __HI_DRV_DEMUX_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* defines the capability of the DEMUX module. cncomment:定义demux模块业务功能结构体 */
typedef struct {
    hi_u32 if_port_num;        /*
                                * number of IF ports.
                                * cncomment: IF 端口数量  (与TS in 端口都是通过tuner 连接,但是IF 端口是使用内置 QAM)
                                */
    hi_u32 tsi_port_num;       /*
                                * number of TS in ports.
                                * cncomment: TS in 端口数量 (与if端口都是通过tuner 连接,但是IF 端口是使用内置 QAM)
                                */
    hi_u32 tso_port_num;       /* number of TS out ports. cncomment: TS out  端口数量 */
    hi_u32 tsio_port_num;       /*
                                 * number of TSIO in ports.
                                 * cncomment: TSIO 端口数量 (与if端口都是通过tuner 连接,但是IF 端口是使用内置 QAM)
                                 */
    hi_u32 ram_port_num;      /* number of ram ports. cncomment: ram端口数量 */
    hi_u32 rmx_port_num;      /* number of remux devices. cncomment:rmx 端口数量 */
    hi_u32 dmx_num;           /* number of DEMUX devices. cncomment:设备数量 */
    hi_u32 channel_num;       /*
                               * number of channels, containing the audio and video channels.
                               * cncomment:通道数量，含音视频通道的数量
                               */
    hi_u32 av_channel_num;    /* number of av channels. cncomment:音视频通道数量 */
    hi_u32 filter_num;        /* number of filters. cncomment:过滤器数量 */
    hi_u32 key_num;           /* number of keys. cncomment:密钥区数量 */
    hi_u32 rec_chn_num;       /* number of record channels. cncomment:录制通道的数量 */
    hi_u32 tag_port_num;      /* number of tag ports. cncomment: tag端口数量 */
} dmx_capability;

/* for unf */
typedef enum {
    DMX_IF_PORT_0 = 0x0,
    DMX_IF_PORT_1,
    DMX_IF_PORT_2,
    DMX_IF_PORT_3,
    DMX_IF_PORT_4,
    DMX_IF_PORT_5,
    DMX_IF_PORT_6,
    DMX_IF_PORT_7,
    DMX_IF_PORT_8,
    DMX_IF_PORT_9,
    DMX_IF_PORT_10,
    DMX_IF_PORT_11,
    DMX_IF_PORT_12,
    DMX_IF_PORT_13,
    DMX_IF_PORT_14,
    DMX_IF_PORT_15,
    DMX_IF_PORT_MAX,

    DMX_TSI_PORT_0 = 0x20,
    DMX_TSI_PORT_1,
    DMX_TSI_PORT_2,
    DMX_TSI_PORT_3,
    DMX_TSI_PORT_4,
    DMX_TSI_PORT_5,
    DMX_TSI_PORT_6,
    DMX_TSI_PORT_7,
    DMX_TSI_PORT_8,
    DMX_TSI_PORT_9,
    DMX_TSI_PORT_10,
    DMX_TSI_PORT_11,
    DMX_TSI_PORT_12,
    DMX_TSI_PORT_13,
    DMX_TSI_PORT_14,
    DMX_TSI_PORT_15,
    DMX_TSI_PORT_MAX,

    DMX_RAM_PORT_0 = 0x80,
    DMX_RAM_PORT_1,
    DMX_RAM_PORT_2,
    DMX_RAM_PORT_3,
    DMX_RAM_PORT_4,
    DMX_RAM_PORT_5,
    DMX_RAM_PORT_6,
    DMX_RAM_PORT_7,
    DMX_RAM_PORT_8,
    DMX_RAM_PORT_9,
    DMX_RAM_PORT_10,
    DMX_RAM_PORT_11,
    DMX_RAM_PORT_12,
    DMX_RAM_PORT_13,
    DMX_RAM_PORT_14,
    DMX_RAM_PORT_15,
    DMX_ANY_RAM_PORT,
    DMX_RAM_PORT_MAX,

    DMX_TSIO_PORT_0 = 0xA0,
    DMX_TSIO_PORT_1,
    DMX_TSIO_PORT_MAX,

    DMX_TAG_PORT_0 = 0xB0,
    DMX_TAG_PORT_1,
    DMX_TAG_PORT_2,
    DMX_TAG_PORT_3,
    DMX_TAG_PORT_4,
    DMX_TAG_PORT_5,
    DMX_TAG_PORT_6,
    DMX_TAG_PORT_7,
    DMX_TAG_PORT_MAX,

    DMX_RMX_PORT_0 = 0xC0,
    DMX_RMX_PORT_1,
    DMX_RMX_PORT_2,
    DMX_RMX_PORT_3,
    DMX_RMX_PORT_4,
    DMX_RMX_PORT_5,
    DMX_RMX_PORT_6,
    DMX_RMX_PORT_7,
    DMX_RMX_PORT_8,
    DMX_RMX_PORT_9,
    DMX_RMX_PORT_10,
    DMX_RMX_PORT_11,
    DMX_RMX_PORT_12,
    DMX_RMX_PORT_13,
    DMX_RMX_PORT_14,
    DMX_RMX_PORT_15,
    DMX_RMX_PORT_MAX,

    DMX_TSO_PORT_0 = 0xE0,
    DMX_TSO_PORT_1,
    DMX_TSO_PORT_2,
    DMX_TSO_PORT_3,
    DMX_TSO_PORT_MAX,

    DMX_PORT_MAX
}dmx_port;

/* type of the DEMUX data packet.cncomment:demux数据包的类型 */
typedef enum {
    DMX_DATA_WHOLE = 0,  /*
                          * the data segment contains a complete data packet.
                          * cncomment:此段数据包含完整的数据包, 对于SECTION每个包都是完整的
                          */
    DMX_DATA_HEAD, /*
                    * the data segment contains the head of a data packet, but the data packet may not be complete.
                    * cncomment:此段数据包含数据包的起始，但是不一定是完整的包, 只用于PES数据
                    */
    DMX_DATA_BODY, /*
                    * this type is valid only for the PES data.the data segment contains the body of a data packet.
                    * cncomment:此段数据包含数据包的内容，不包含起始，可能有结尾, 只用于PES数据
                    */
    DMX_DATA_TAIL, /*
                    * this type is valid only for the PES data.the data segment contains the tail of a data packet,
                    * and is used to identify the end of a data packet.
                    * cncomment:此段数据包含数据包的结尾，用于指示可识别的包结束, 只用于PES数据
                    */

    DMX_DATA_MAX
} dmx_data_type;

typedef struct {
    hi_u8  *data;
    hi_u32 length;
    hi_s64 pts;
    hi_u64 offset;
    hi_s64 buf_handle;
    hi_u64 phy_addr;
    hi_u64 ker_vir_addr;
    dmx_data_type data_type;
    /* for pvr playback */
    hi_u32   disp_time;
    hi_u32   disp_enable_flag;
    hi_u32   disp_frame_distance;
    hi_u32   distance_before_first_frame;
    hi_u32   gop_num;
} dmx_buffer;

typedef struct {
    hi_u32 data;
    hi_u32 length;
    hi_s64 pts;
    hi_u64 offset;
    hi_s64 buf_handle;
    hi_u64 phy_addr;
    hi_u64 ker_vir_addr;
    dmx_data_type data_type;
    /* for pvr playback */
    hi_u32   disp_time;
    hi_u32   disp_enable_flag;
    hi_u32   disp_frame_distance;
    hi_u32   distance_before_first_frame;
    hi_u32   gop_num;
} dmx_compat_buffer;

/* secure mode type.cncomment:安全模式类型 */
typedef enum {
    DMX_SECURE_NONE = 0,        /* no security protection. cncomment:无安全保护 */
    DMX_SECURE_TEE,             /* trustedzone security protection. cncomment:trustedzone安全保护 */

    DMX_SECURE_MAX
} dmx_secure_mode;

/***********TSI port****************/
/* TS port mode */
typedef enum {
    DMX_PORT_MODE_EXTERNAL,   /* External TS input mode.CNcomment:外部TS输入 */
    DMX_PORT_MODE_INTERNAL,   /* Internal TS input mode.CNcomment:内部TS输入 */
    DMX_PORT_MODE_RAM,        /* Memory input mode.CNcomment:从内存输入 */

    DMX_PORT_MODE_MAX
} dmx_tsi_port_mode;

/* TSI port type */
typedef enum {
    DMX_PORT_TYPE_PARALLEL_BURST,          /* Parallel burst mode.CNcomment:并行BURST模式 */
    DMX_PORT_TYPE_PARALLEL_VALID,          /* Parallel valid mode.CNcomment:并行VALID模式 */
    DMX_PORT_TYPE_PARALLEL_NOSYNC_188,     /* Self-sync 188 mode.CNcomment:自同步188模式 */
    DMX_PORT_TYPE_PARALLEL_NOSYNC_204,     /* Self-sync 204 mode.CNcomment:自同步204模式 */
    DMX_PORT_TYPE_PARALLEL_NOSYNC_188_204, /* self-sync 188/204 auto-identification mode.
                                            * CNcomment:自同步188/204自动识别模式
                                            */
    DMX_PORT_TYPE_SERIAL,                  /* Serial sync mode, 1bit.CNcomment:串行sync模式, 1bit串行 */
    DMX_PORT_TYPE_USER_DEFINED,            /* User defined mode.CNcomment:用户自定义模式 */
    DMX_PORT_TYPE_SERIAL2BIT,              /* Serial sync mode, 2bit.CNcomment:串行sync模式, 2bit串行 */
    DMX_PORT_TYPE_SERIAL_NOSYNC,           /* Serial nosync mode, 1bit.CNcomment:串行nosync模式, 1bit串行 */
    DMX_PORT_TYPE_SERIAL2BIT_NOSYNC,       /* Serial nosync mode, 2bit.CNcomment:串行nosync模式, 2bit串行 */
    DMX_PORT_TYPE_AUTO,                    /* Auto mode.CNcomment: 自动模式 */
    DMX_PORT_TYPE_PARALLEL_VALID_DUMMY_SYNC, /*
                                              * Parallel valid dummy sync mode.
                                              * CNcomment:并行VALID DUMMY SYNC模式 用于非TS全码流录制
                                              */
    DMX_PORT_TYPE_SERIAL_NOSYNC_NOVALID,     /*
                                              * Serial nosync novalid mode, 1bit.
                                              * CNcomment:串行nosync novalid模式, 1bit串行
                                              */
    DMX_PORT_TYPE_SERIAL2BIT_NOSYNC_NOVALID, /*
                                              * Serial nosync novalid mode, 2bit.
                                              * CNcomment:串行nosync novalid模式, 2bit串行
                                              */

    DMX_PORT_TYPE_MAX
} hi_dmx_port_type;

typedef hi_dmx_port_type dmx_tsi_port_type;

/* TSI port attributes.cncomment:tsi端口属性 */
typedef struct {
    dmx_tsi_port_mode port_mod; /* port mode.readonly. cncomment:端口模式 只读 */
    dmx_tsi_port_type port_type; /* port type. cncomment:端口类型 */
    hi_u32 sync_lost_threshold; /*
                                 * sync loss threshold.the default value is recommended.
                                 * cncomment:同步丢失门限，推荐使用默认值
                                 */
    hi_u32 sync_lock_threshold; /*
                                 * sync lock threshold.the default value is recommended.
                                 * cncomment:同步锁定门限，推荐使用默认值
                                 */
    hi_u32 tuner_in_clk; /* whether to reverse the phase of the clock input from the tuner */
                         /* cncomment:tuner输入时钟是否反相.0: 同相(默认); 1: 反相 */
    hi_u32 serial_bit_select; /*
                               * port line sequence select in parallel mode:
                               * 0: cdata[7] is the most significant bit (MSB) (default).
                               * 1: cdata[0] is the MSB.
                               * in serial mode: 1: cdata[0] is the data line (default). 0: cdata[7] is the data line.
                               * CNcomment:端口线序选择。并行时:0: 表示cdata[7]最高位(默认);1: 表示cdata[0]最高位。
                               * 串行时:1: 表示cdata[0]为数据线(默认); 0: 表示cdata[7]为数据线
                               */

    hi_u32 tuner_err_mod; /*
                           * level mode of the cerr_n line from the tuner to a DEMUX.
                           * 0: A data error occurs when the cerr_n line is high.
                           * 1: A data error occurs when the cerr_n line is low (default).
                           * CNcomment:tuner到DEMUX的cerr_n线电平模式:
                           * 0: 表示cerr_n线为高时表示数据错误;
                           * 1: 表示cerr_n线为低时表示数据错误 (默认)
                           */
    hi_u32 user_define_len1; /*
                              * user defined length1,valid when en_port_type is HI_UNF_DMX_PORT_TYPE_USER_DEFINED,188~255
                              * cncomment:用户自定义长度检测包长1，只有端口类型为用户自定义时有效，有效范围188~255
                              */
    hi_u32 user_define_len2; /*
                              * user defined length2,valid when en_port_type is HI_UNF_DMX_PORT_TYPE_USER_DEFINED,188~255
                              * cncomment:用户自定义长度检测包长2，只有端口类型为用户自定义时有效，有效范围188~255
                              */
    dmx_port serial_port_share_clk; /*
                                     * port of clock that this port shared, only valid for TSI port tpye
                                     * cncomment:指定该端口所复用时钟的对应端口，该成员仅对tsi端口类型有效
                                     */
    hi_u32 tuner_clk_mode; /*
                            * Tuner Clock Mode, the configure must be needed to keep sync with Tuner,
                            * 0: single edge-triggered, 1: double edge-triggered
                            */
} dmx_port_attr;

/*********** ram port ****************/
#define DMX_DEFAULT_RAMPORT_RATE        400000 /* kbps, equal to 400mbps */
#define DMX_TS_PKT_SIZE                 188
#define DMX_TS_FEC_PKT_SIZE             204
#define DMX_TTS_PKT_SIZE                192
#define DMX_RAM_FLUSH_TS_CNT            16
#define DMX_BLK_DEFAULT_PKT_NUM         300
#define DMX_MAX_PID                     0x1FFF

typedef struct {
    hi_u8  *data;
    hi_u32 length;
    hi_u64 phy_addr;
    hi_u64 offset;
    hi_s64 buf_handle;
} dmx_ram_buffer;

typedef struct {
    hi_u32 data;
    hi_u32 length;
    hi_u64 phy_addr;
    hi_u64 offset;
    hi_s64 buf_handle;
} dmx_compat_ram_buffer;

typedef enum {
    DMX_TS_CRC_DIS,
    DMX_TS_CRC_EN_SCB_DIS,
    DMX_TS_CRC_EN_SCB_EN,

    DMX_TS_CRC_SCB_MAX
} dmx_ts_crc_scb;

/* TS buffer attribute */
typedef struct {
    dmx_secure_mode   secure_mode;         /* secure indication */
    hi_u32            buffer_size;         /* buffer size */
    hi_u32            max_data_rate;       /* kbps */
    dmx_ts_crc_scb    crc_scb;             /* whether the crc or scb is enabled. */
} dmx_ram_port_attr;

/* status of a TS buffer of a DEMUX */
typedef struct {
    dmx_port   port_id;
    hi_u32     buffer_size; /* buffer size.cncomment:缓冲区大小 */
    hi_u32     buffer_used; /* used buffer size.cncomment:缓冲区已使用大小,该值比实际使用值大0x100字节 */
    hi_u32     buffer_blk_size;
    hi_u32     pkt_num;
    dmx_secure_mode secure_mode;  /* secure indication.cncomment:安全标示 */
} dmx_ram_port_status;

/***********tag port****************/
typedef enum {
    DMX_TAG_HEAD_SYNC = 0x0,       /* tag sync signal at tag head.CNcomment:sync信号在tag头 */
    DMX_NORMAL_HEAD_SYNC = 0x1,    /* tag sync signal at 47 heade.CNcomment:sync信号在47头 */
} hi_dmx_tag_sync_mode;

#define DMX_MAX_TAG_LENGTH 12
typedef struct {
    dmx_port             ts_src;
    hi_u8                tag[DMX_MAX_TAG_LENGTH];
    hi_u32               tag_len;
    hi_bool              enable;
    hi_dmx_tag_sync_mode sync_mod;
} dmx_tag_port_attr;

/* 与HI_UNF_DMX_TSO_CLK_MODE_E一一对应 */
typedef enum {
    DMX_TSO_CLK_MODE_NORMAL,  /* Normal clock: clock always active.CNcomment:普通模式的时钟,时钟平率均匀 */
    DMX_TSO_CLK_MODE_JITTER,  /*
                               * Jittered clock: clock active only when outputing data.
                               * CNcomment:Jittered 模式时钟,时钟频率不均匀,此时只有在有数据输出时才会出时钟
                               * (valid信号为高的情况下)
                               */
    DMX_TSO_CLK_MODE_MAX
} hi_dmx_tso_clk_mode;

/* same as HI_UNF_DMX_TSO_VALID_MODE_E */
typedef enum {
    DMX_TSO_VALID_ACTIVE_OUTPUT, /* Valid signal high when outputing datas.CNcomment:valid信号在输出数据时为高 */
    DMX_TSO_VALID_ACTIVE_HIGH,  /* Valid signal always high.CNcomment:valid信号总是为高 */
    DMX_TSO_VALID_ACTIVE_MAX
} hi_dmx_tso_valid_mode;

/* same as HI_UNF_DMX_TSO_SERIAL_BIT_E */
typedef enum {
    DMX_TSO_SERIAL_BIT_0 = 0x0, /*
                                 * Serial output data using data[0] as signal line.
                                 * CNcomment:使用data[0]作为串行信号线输出
                                 */
    DMX_TSO_SERIAL_BIT_7 = 0x7, /*
                                 * Serial output data using data[7] as signal line.
                                 * CNcomment:使用data[7]作为串行信号线输出
                                 */
    DMX_TSO_SERIAL_BIT_MAX
} hi_dmx_tso_serial_bit;

/* same as HI_UNF_DMX_TSO_CLK_E */
typedef enum {
    DMX_TSO_CLK_100M,           /* TS out mode clock frequency 100M.CNcomment:TS输出模块时钟频率选择为100M */
    DMX_TSO_CLK_150M,           /* TS out mode clock frequency 150M.CNcomment:TS输出模块时钟频率选择为150M */
    DMX_TSO_CLK_1200M,          /* TS out mode clock frequency 1200M.CNcomment:TS输出模块时钟频率选择为1200M */
    DMX_TSO_CLK_1500M,          /* TS out mode clock frequency 1500M.CNcomment:TS输出模块时钟频率选择为1500M */
    DMX_TSO_CLK_MAX
} hi_dmx_tso_clk;

/* TS Out port attrs */
typedef struct {
    hi_bool                     enable;       /* Port enable, default value HI_TRUE means enable */
    hi_bool                     clk_reverse;  /*
                                               * Clock phase reverse, default value HI_FALSE means do not reverse the
                                               * phase of clock
                                               */
    dmx_port                    ts_source;    /*
                                               * Source of this TS Out port,can choose from DMX_PORT_IF_0 to
                                               * DMX_PORT_TSI_9
                                               */
    hi_dmx_tso_clk_mode         clk_mode;     /* Clock mode: HI_UNF_DMX_TSO_CLK_MODE_NORMAL is the default value */
    hi_dmx_tso_valid_mode       valid_mode;   /*
                                               * Wether valid signal always enable : HI_UNF_DMX_TSO_VALID_ACTIVE_OUTPUT
                                               * is the default value
                                               */
    hi_bool                     bit_sync;     /*
                                               * The sync signal duration : HI_TRUE: only valid when output the first
                                               * bit(default). HI_FALSE: keep when outputing the whole byte
                                               */
    hi_dmx_port_type            port_type;    /*
                                               * Port out put mode: serial(HI_UNF_DMX_PORT_TYPE_SERIAL),
                                               * parallel(HI_UNF_DMX_PORT_TYPE_PARALLEL_BURST) or serial 2bits
                                               * (HI_UNF_DMX_PORT_TYPE_SERIAL2BIT) mode. default is serial
                                               * (HI_UNF_DMX_PORT_TYPE_SERIAL).
                                               */
    hi_dmx_tso_serial_bit       bit_selector; /*
                                               * Port line sequence select In serial mode.only valid when using serial
                                               * out put mode,HI_UNF_DMX_TSO_SERIAL_BIT_7 is the default value.
                                               */
    hi_bool                     out_mode;     /*
                                               * Out put byte endian .only valid when using serial out put mode:
                                               * HI_FALSE: first output MSB (default). HI_TRUE:  first output LSB
                                               */
    hi_dmx_tso_clk              en_clk;       /* TS out mode clock frequency,default is HI_UNF_DMX_TSO_CLK_150M */
    hi_u32                      clk_div;      /*
                                               * TS out mode clock frequency divider,must be times of 2,and must meet
                                               * (2 <= u32ClkDiv <= 32) .default is 2
                                               */
} dmx_tso_port_attr;

typedef struct {
    hi_u32 ts_packet_cnt;          /* Number of TS packets received from the TS port */
    hi_u32 err_ts_packet_cnt;      /* Number of error TS packets received from the TS port */
    hi_u32 ts_sync_byte_err_cnt;   /* Number of sync byte error TS packets received from the TS port */
    hi_u32 ts_sync_loss_cnt;       /* Number of sync loss TS packets received from the TS port */
    hi_u32 ts_packet_drained_cnt;  /* Number of Drained TS packets received from the TS port */
    hi_u32 packet_dis_cc_cnt;      /* Number of Discontinuity TS packets received from the TS port */
    hi_u32 packet_dis_cc_cnt_ca;   /* Number of Discontinuity TS packets received before descrambler from the TS port,
                                    * the value is same as u32PackDisCCCnt.
                                    */
} dmx_port_packet_num;


/* can't open same band in different process */
typedef enum {
    DMX_BAND_0 = 0x0,
    DMX_BAND_1,
    DMX_BAND_2,
    DMX_BAND_3,
    DMX_BAND_4,
    DMX_BAND_5,
    DMX_BAND_6,
    DMX_BAND_7,
    DMX_BAND_8,
    DMX_BAND_9,
    DMX_BAND_10,
    DMX_BAND_11,
    DMX_BAND_12,
    DMX_BAND_13,
    DMX_BAND_14,
    DMX_BAND_15,
    DMX_ANY_BAND = 0x10,

    DMX_BAND_MAX
} dmx_band;

typedef struct {
    hi_u32  band_attr;
} dmx_band_attr;

typedef struct {
    hi_u32      band_status;
    dmx_band    band;
    dmx_port    port_id;
} dmx_band_status;

typedef struct {
    hi_handle   dsc_handle;
    hi_handle   band_handle;
    hi_u32      pid;
} dmx_pidch_status;

/* same as HI_UNF_VCODEC_TYPE_E */
typedef enum {
    DMX_VCODEC_TYPE_MPEG2,
    DMX_VCODEC_TYPE_MPEG4,       /* MPEG4 DIVX4 DIVX5 */
    DMX_VCODEC_TYPE_AVS,
    DMX_VCODEC_TYPE_H263,
    DMX_VCODEC_TYPE_H264,
    DMX_VCODEC_TYPE_REAL8,
    DMX_VCODEC_TYPE_REAL9,
    DMX_VCODEC_TYPE_VC1,
    DMX_VCODEC_TYPE_VP6,
    DMX_VCODEC_TYPE_VP6F,
    DMX_VCODEC_TYPE_VP6A,
    DMX_VCODEC_TYPE_MJPEG,
    DMX_VCODEC_TYPE_SORENSON,    /* SORENSON SPARK */
    DMX_VCODEC_TYPE_DIVX3,
    DMX_VCODEC_TYPE_RAW,
    DMX_VCODEC_TYPE_JPEG,        /* Used only for VENC */
    DMX_VCODEC_TYPE_VP8,
    DMX_VCODEC_TYPE_MSMPEG4V1,   /* MS private MPEG4 */
    DMX_VCODEC_TYPE_MSMPEG4V2,
    DMX_VCODEC_TYPE_MSVIDEO1,    /* MS video */
    DMX_VCODEC_TYPE_WMV1,
    DMX_VCODEC_TYPE_WMV2,
    DMX_VCODEC_TYPE_RV10,
    DMX_VCODEC_TYPE_RV20,
    DMX_VCODEC_TYPE_SVQ1,        /* Apple video */
    DMX_VCODEC_TYPE_SVQ3,        /* Apple video */
    DMX_VCODEC_TYPE_H261,
    DMX_VCODEC_TYPE_VP3,
    DMX_VCODEC_TYPE_VP5,
    DMX_VCODEC_TYPE_CINEPAK,
    DMX_VCODEC_TYPE_INDEO2,
    DMX_VCODEC_TYPE_INDEO3,
    DMX_VCODEC_TYPE_INDEO4,
    DMX_VCODEC_TYPE_INDEO5,
    DMX_VCODEC_TYPE_MJPEGB,
    DMX_VCODEC_TYPE_MVC,
    DMX_VCODEC_TYPE_HEVC,
    DMX_VCODEC_TYPE_DV,
    DMX_VCODEC_TYPE_VP9,
    DMX_VCODEC_TYPE_MAX
} dmx_vcodec_type;

typedef enum {
    DMX_PLAY_TYPE_SEC       = 0x1,  /* section play channel */
    DMX_PLAY_TYPE_PES       = 0x2,  /* pes play channel */
    DMX_PLAY_TYPE_AUD       = 0x4,  /* audio play channel */
    DMX_PLAY_TYPE_VID       = 0x8,  /* video play channel */
    DMX_PLAY_TYPE_TS        = 0x10, /* post play channel */
    DMX_PLAY_TYPE_REC       = 0x20, /* keep adaptive with tee side, not use currently */
    DMX_PLAY_TYPE_SCD       = 0x40, /* keep adaptive with tee side, not use currently */
    DMX_PLAY_TYPE_MAX       = 0x7F
} dmx_play_type;

typedef enum {
    DMX_DATA_MODE_STREAM,
    DMX_DATA_MODE_PACKET,
    DMX_DATA_MODE_MAX
} dmx_data_mode;

typedef enum {
    DMX_FLT_CRC_MODE_FORBID                 =    0,
    DMX_FLT_CRC_MODE_FORCE_AND_DISCARD      =    1,
    DMX_FLT_CRC_MODE_FORCE_AND_SEND         =    2,
    DMX_FLT_CRC_MODE_BY_SYNTAX_AND_DISCARD  =    3,
    DMX_FLT_CRC_MODE_BY_SYNTAX_AND_SEND     =    4,
    DMX_FLT_CRC_MODE_MAX
} dmx_flt_crc_mode;

typedef struct {
    dmx_play_type      type;
    dmx_flt_crc_mode   crc_mode;
    hi_bool            live_play;
    dmx_data_mode      data_mode;
    dmx_vcodec_type    vcodec_type;
    dmx_secure_mode    secure_mode;  /* TEE or REE */
    hi_s64             buf_handle;
    hi_u32             buf_size;
} dmx_play_attrs;

typedef enum {
    HI_DMX_SCRAMBLED_FLAG_TS,  /* TS data is scrambled */
    HI_DMX_SCRAMBLED_FLAG_PES, /* PES data is scrambled */
    HI_DMX_SCRAMBLED_FLAG_NO,  /* Data is not scrambled */

    HI_DMX_SCRAMBLED_FLAG_MAX
} hi_dmx_scrambled_flag;

/* channel status */
typedef enum {
    DMX_CHAN_CLOSE = 0x0,
    DMX_CHAN_PLAY_EN = 0x1,
    DMX_CHAN_REC_EN = 0x2,
    DMX_CHAN_PLAY_REC_EN = 0x3,
} dmx_chan_status;

/* status of the channel */
typedef struct {
    hi_u32 ts_cnt;        /* play channel's ts packet number */
    hi_u32 cc_disc_cnt;   /* CC Discontinuity Count */
} dmx_chan_packet_num;

typedef struct {
    hi_bool is_opened;
    hi_u32 buf_size;      /* buffer size */
    hi_u32 buf_used_size; /* buffer used size */
    hi_u32 read_ptr;      /* buffer read pointer */
    hi_u32 write_ptr;     /* buffer write pointer */
    hi_u64 buf_phy_addr;
    hi_u64 offset;
    hi_s64 buf_handle;
    hi_u64 buf_ker_vir_addr;
} dmx_play_status;

/* demux index */
/* same as HI_UNF_VIDEO_FRAME_TYPE_E */
typedef enum {
    DMX_FRAME_TYPE_UNKNOWN,   /* Unknown.CNcomment: 未知的帧类型 */
    DMX_FRAME_TYPE_I,         /* I frame.CNcomment: I帧 */
    DMX_FRAME_TYPE_P,         /* P frame.CNcomment: P帧 */
    DMX_FRAME_TYPE_B,         /* B frame.CNcomment: B帧 */
    DMX_FRAME_TYPE_IDR,       /* IDR frame.CNcomment: IDR帧 */
    DMX_FRAME_TYPE_BLA,       /* BLA frame.CNcomment: BLA帧 */
    DMX_FRAME_TYPE_CRA,       /* CRA frame.CNcomment: CRA帧 */
    DMX_FRAME_TYPE_MAX
} hi_dmx_video_frame_type;

/* index data */
typedef struct {
    hi_dmx_video_frame_type   frame_type;
    hi_s64                    pts_us;
    hi_u64                    global_offset;
    hi_u32                    frame_size;
    hi_u32                    data_time_ms;

    /* hevc private */
    hi_s16                    cur_poc;
    hi_u16                    ref_poc_cnt;
    hi_s16                    ref_poc[16]; /* according to hevc protocol, max reference poc is 16. */
} dmx_index_data;

#define DMX_FILTER_MAX_DEPTH            16
typedef struct {
    hi_u32 depth;
    hi_u8  match[DMX_FILTER_MAX_DEPTH];
    hi_u8  mask[DMX_FILTER_MAX_DEPTH];
    hi_u8  negate[DMX_FILTER_MAX_DEPTH];
} dmx_filter_attrs;

/* type of record */
typedef enum {
    DMX_REC_TYPE_SELECT_PID,
    DMX_REC_TYPE_ALL_PID,
    DMX_REC_TYPE_ALL_DATA,
    DMX_REC_TYPE_MAX
} dmx_rec_type;

/* TS stream type. */
typedef enum {
    DMX_TS_PACKET_188,           /* ts stream packet size of 188 bytes.CNcomment: ts码流包大小为188字节 */
    DMX_TS_PACKET_192,           /* ts stream packet size of 192 bytes.CNcomment: ts码流包大小为192字节 */
    DMX_TS_PACKET_MAX
} dmx_ts_packet_type;

/* type of index */
typedef enum {
    DMX_REC_INDEX_TYPE_NONE,     /* no index is created.cncomment: 不建索引 */
    DMX_REC_INDEX_TYPE_VIDEO,    /* video index.cncomment: 视频索引 */
    DMX_REC_INDEX_TYPE_AUDIO,    /* audio index.cncomment: 音频索引 */
    DMX_REC_INDEX_TYPE_MAX
} dmx_rec_index_type;

typedef struct {
    hi_bool                   descramed;
    hi_u32                    rec_buf_size;
    dmx_rec_type              rec_type;
    dmx_rec_index_type        index_type;
    hi_u32                    index_src_pid;
    dmx_ts_crc_scb            crc_scb; /*
                                        * if crc_scb == DMX_TS_CRC_EN_SCB_EN keep the scrambling ctrl bits to 1,
                                        * after descrambled
                                        */
    dmx_vcodec_type           vcodec_type;
    dmx_ts_packet_type        ts_packet_type;
    dmx_secure_mode           secure_mode;
} dmx_rec_attrs;

/* record data */
typedef struct {
    hi_u8  *p_data_addr;      /* data address */
    hi_u32  u32data_phy_addr; /* data physical address */
    hi_u32  u32len;           /* data length */
    hi_bool last_data_flag;
} dmx_rec_data;

typedef struct {
    hi_u32  buf_size;     /* buffer size */
    hi_u32  used_size;    /* used buffer */
} dmx_rec_status;

#define DMX_MAX_IDX_ACQUIRED_EACH_TIME 256

typedef struct {
    hi_u32           index_num;
    hi_u32           rec_data_cnt;
    dmx_index_data   index[DMX_MAX_IDX_ACQUIRED_EACH_TIME];
    dmx_buffer       rec_data[2];
} dmx_rec_data_index;

#define DMX_POLL_IN 0x0001

typedef struct {
    hi_handle handle;
    hi_u32    revents;
} dmx_poll_handle;

#define DMX_MIN_KEY_LEN            8
#define DMX_MAX_KEY_LEN            16
#define DMX_SYS_KEY_LEN            32

typedef enum {
    DMX_CA_NORMAL = 0,    /* common CA */
    DMX_CA_ADVANCE,       /* advanced CA */

    DMX_CA_MAX
} dmx_dsc_ca_type;

typedef enum {
    DMX_DSC_KEY_EVEN = 0,
    DMX_DSC_KEY_ODD,
    DMX_DSC_KEY_SYS,
} dmx_dsc_key_type;

/* CA entropy reduction mode*/
typedef enum {
    DMX_CA_ENTROPY_CLOSE = 0,  /* 64bit */
    DMX_CA_ENTROPY_OPEN,       /* 48bit */

    DMX_CA_ENTROPY_MAX
} dmx_dsc_entropy;

/* type of the descrambler protocol. */
typedef enum {
    DMX_DESCRAMBLER_TYPE_CSA2 = 0,       /* CSA2.0 */
    DMX_DESCRAMBLER_TYPE_CSA3,          /* CSA3.0 */
    DMX_DESCRAMBLER_TYPE_AES_IPTV,          /* AES IPTV of SPE */
    DMX_DESCRAMBLER_TYPE_AES_ECB,          /* SPE AES ECB */
    DMX_DESCRAMBLER_TYPE_AES_CI,          /* SPE AES CIPLUS */
    DMX_DESCRAMBLER_TYPE_DES_CI,          /* DES CIPLUS */
    DMX_DESCRAMBLER_TYPE_DES_ECB,          /* DES ECB */
    DMX_DESCRAMBLER_TYPE_DES_CBC,          /* DES CBC */
    DMX_DESCRAMBLER_TYPE_AES_NS,          /* AES ns-mode, AES NOVEL */
    DMX_DESCRAMBLER_TYPE_SMS4_NS,          /* SMS4 ns-mode, SMS4 NOVEL */
    DMX_DESCRAMBLER_TYPE_SMS4_IPTV,          /* SMS4 IPTV */
    DMX_DESCRAMBLER_TYPE_SMS4_ECB,          /* SMS4 ECB */
    DMX_DESCRAMBLER_TYPE_SMS4_CBC,          /* SMS4 CBC */
    DMX_DESCRAMBLER_TYPE_AES_CBC,          /* AES CBC */
    DMX_DESCRAMBLER_TYPE_DES_IPTV,            /* DES IPTV */
    DMX_DESCRAMBLER_TYPE_TDES_IPTV,           /* TDES IPTV */
    DMX_DESCRAMBLER_TYPE_TDES_ECB,            /* TDES ECB */
    DMX_DESCRAMBLER_TYPE_TDES_CBC,            /* TDES CBC */
    DMX_DESCRAMBLER_TYPE_AES_ECB_L,           /* AES_ECB_L the clear stream left in the leading */
    DMX_DESCRAMBLER_TYPE_AES_CBC_L,           /* AES_CBC_L the clear stream left in the leading */
    DMX_DESCRAMBLER_TYPE_ASA,                 /* ASA 64/128 algorithm only for NOC3.X */
    DMX_DESCRAMBLER_TYPE_AES_CISSA,        /* common IPTV software-oriented scrambling algorithm (CISSA)  for NOC3.X */
    DMX_DESCRAMBLER_TYPE_MULTI2_ECB,
    DMX_DESCRAMBLER_TYPE_MULTI2_CBC,
    DMX_DESCRAMBLER_TYPE_MULTI2_IPTV,

    DMX_DESCRAMBLER_TYPE_MAX
} dmx_dsc_key_alg;

typedef struct {
    dmx_dsc_ca_type    ca_type;               /* whether the descrambler adopts advanced CA. */
    dmx_dsc_entropy    entropy_reduction;     /* CA entropy reduction mode,for CSA2.0 */
    dmx_dsc_key_alg    alg;
    hi_bool            keyslot_create_en;     /* Whether the keysloy will be created, when create descrambler. */
} dmx_dsc_attrs;

typedef struct {
    dmx_band       band;
    dmx_port       port;
    hi_u32         pid;
    hi_handle      sync_handle;
} dmx_pcr_status;

typedef enum {
    DMX_RMX_PUMP_TYPE_PID,
    DMX_RMX_PUMP_TYPE_REMAP_PID,
    DMX_RMX_PUMP_TYPE_ALLPASS_PORT,

    HI_MPI_RMX_PUMP_TYPE_MAX
} dmx_rmx_pump_type;

typedef struct {
    dmx_rmx_pump_type pump_type;
    dmx_port in_port_id;
    hi_u32 pid;
    hi_u32 remap_pid;
} dmx_rmx_pump_attrs;

typedef struct {
    dmx_port out_port_id;
} dmx_rmx_attrs;

typedef struct {
    dmx_port out_port_id;
}dmx_rmx_status;

typedef enum {
    HI_DMX_EVT_ERR,
    HI_DMX_EVT_NEW_PES,
    HI_DMX_EVT_EOS,
    HI_DMX_EVT_PCR,
    HI_DMX_EVT_OVFL
} hi_dmx_evt_id;

typedef struct {
    hi_handle handle;
    hi_u64 pcr_value;
    hi_u64 scr_value;
} dmx_sync_pcr;

typedef hi_s32 (*drv_dmx_callback)(hi_handle user_handle, hi_dmx_evt_id evt_id, const void *param, hi_u32 param_size);

typedef struct {
    hi_handle user_handle;
    drv_dmx_callback cb;
} dmx_event_callback;

typedef struct {
    hi_s32 (*dmx_get_capability)(dmx_capability *cap);
    hi_s32 (*dmx_get_tuner_port_pkt_cnt)(const hi_u32 dvb_port_id, hi_u32 *ts_pkt_cnt, hi_u32 *err_pkt_cnt);
    hi_s32 (*dmx_set_callback)(hi_handle handle, hi_handle user_handle, drv_dmx_callback cb);
} demux_func_export;

typedef enum {
    HI_DMX_BUFFER_ATTACH_TSR2RCIPHER,
    HI_DMX_BUFFER_ATTACH_CIPHER,
    HI_DMX_BUFFER_ATTACH_MAX
} dmx_buffer_attach_type;

typedef enum {
    HI_DMX_CHAN_CC_REPEAT_MODE_RSV = 0x0,     /* Receive CC repeat ts packet */
    HI_DMX_CHAN_CC_REPEAT_MODE_DROP,          /* Drop CC repeat ts packet */
    HI_DMX_CHAN_CC_REPEAT_MODE_MAX
} dmx_chan_cc_repeat_mode;

typedef enum {
    HI_DMX_INVOKE_TYPE_CHAN_CC_REPEAT_SET = 0,  /* dmx set channel extra attr,param:HI_UNF_DMX_CHAN_CC_REPEAT_SET_S */
    HI_DMX_INVOKE_TYPE_PUSI_SET,                /* dmx set PUSI flag,param:HI_UNF_DMX_PUSI_SET_S */
    HI_DMX_INVOKE_TYPE_TEI_SET,                 /* dmx set TEI flag,param:HI_UNF_DMX_TEI_SET_S */
    HI_DMX_INVOKE_TYPE_TSI_ATTACH_TSO,          /* Attach TSI with TSO,param:HI_UNF_DMX_TSI_ATTACH_TSO_S */
    HI_DMX_INVOKE_TYPE_BUTT
} dmx_invoke_type;

/* global begin */
hi_s32 hi_drv_dmx_set_callback(hi_handle handle, hi_handle user_handle, drv_dmx_callback cb);
hi_s32 hi_drv_dmx_get_capability(dmx_capability *cap);

struct dmx_session;
hi_s32 hi_drv_dmx_create_session(struct dmx_session **new_session);
hi_s32 hi_drv_dmx_destroy_session(struct dmx_session *session);

/*********tsiport begin************/
hi_s32 hi_drv_dmx_tsi_get_port_attrs(dmx_port port, dmx_port_attr *attrs);
hi_s32 hi_drv_dmx_tsi_set_port_attrs(dmx_port port, const dmx_port_attr *attrs);

/*********if port begin************/
hi_s32 hi_drv_dmx_if_get_port_attrs(dmx_port port, dmx_port_attr *attrs);
hi_s32 hi_drv_dmx_if_set_port_attrs(dmx_port port, const dmx_port_attr *attrs);

/*********tso port begin************/
hi_s32 hi_drv_dmx_tso_get_port_attrs(dmx_port port, dmx_tso_port_attr *attrs);
hi_s32 hi_drv_dmx_tso_set_port_attrs(dmx_port port, const dmx_tso_port_attr *attrs);

/*********tag port begin************/
hi_s32 hi_drv_dmx_tag_get_port_attrs(dmx_port port, dmx_tag_port_attr *attrs);
hi_s32 hi_drv_dmx_tag_set_port_attrs(dmx_port port, const dmx_tag_port_attr *attrs);

/*********ramport begin************/
hi_s32 hi_drv_dmx_ram_open_port(dmx_port port, const dmx_ram_port_attr *attrs, hi_handle *handle, struct dmx_session *session);
hi_s32 hi_drv_dmx_ram_get_port_attrs(hi_handle handle, dmx_port_attr *attrs);
hi_s32 hi_drv_dmx_ram_set_port_attrs(hi_handle handle, const dmx_port_attr *attrs);
hi_s32 hi_drv_dmx_ram_get_port_status(hi_handle handle, dmx_ram_port_status *status);
hi_s32 hi_drv_dmx_ram_get_buffer(hi_handle handle, hi_u32 req_len, dmx_ram_buffer *buf, hi_u32 time_out/* ms */);
hi_s32 hi_drv_dmx_ram_push_buffer(hi_handle handle, dmx_ram_buffer *buf);
hi_s32 hi_drv_dmx_ram_put_buffer(hi_handle handle, hi_u32 valid_datalen, hi_u32 start_pos);
hi_s32 hi_drv_dmx_ram_release_buffer(hi_handle handle, dmx_ram_buffer *buf);
hi_s32 hi_drv_dmx_ram_flush_buffer(hi_handle handle);
hi_s32 hi_drv_dmx_ram_reset_buffer(hi_handle handle);
hi_s32 hi_drv_dmx_ram_close_port(hi_handle handle);
hi_s32 hi_drv_dmx_ram_pre_mmap(hi_handle handle, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr);
hi_s32 hi_drv_dmx_ram_pst_mmap(hi_handle handle, hi_void *buf_usr_addr);
hi_s32 hi_drv_dmx_ram_get_portid(hi_handle handle, dmx_port *port);
hi_s32 hi_drv_dmx_ram_get_porthandle(dmx_port port, hi_handle *handle);
hi_s32 hi_drv_dmx_port_get_packet_num(dmx_port port, dmx_port_packet_num *port_stat);
hi_s32 hi_drv_dmx_ram_get_bufhandle(hi_handle handle,
    hi_mem_handle_t *buf_handle, hi_mem_handle_t *dsc_buf_handle, hi_mem_handle_t *flush_buf_handle);

/*********rmx begin************/
hi_s32 hi_drv_dmx_rmx_create(const dmx_rmx_attrs *attrs, hi_handle *rmx_handle, struct dmx_session *session);
hi_s32 hi_drv_dmx_rmx_open(hi_handle rmx_handle);
hi_s32 hi_drv_dmx_rmx_get_attrs(hi_handle rmx_handle, dmx_rmx_attrs *attrs);
hi_s32 hi_drv_dmx_rmx_set_attrs(hi_handle rmx_handle, const dmx_rmx_attrs *attrs);
hi_s32 hi_drv_dmx_rmx_get_status(hi_handle rmx_handle, dmx_rmx_status *status);
hi_s32 hi_drv_dmx_rmx_add_pump(hi_handle rmx_handle, dmx_rmx_pump_attrs *pump_attrs, hi_handle *rmx_pump_handle, struct dmx_session *session);
hi_s32 hi_drv_dmx_rmx_del_pump(hi_handle rmx_pump_handle);
hi_s32 hi_drv_dmx_rmx_del_all_pump(hi_handle rmx_handle);
hi_s32 hi_drv_dmx_rmx_get_pump_attrs(hi_handle rmx_pump_handle, dmx_rmx_pump_attrs *pump_attrs);
hi_s32 hi_drv_dmx_rmx_set_pump_attrs(hi_handle rmx_pump_handle, const dmx_rmx_pump_attrs *pump_attrs);
hi_s32 hi_drv_dmx_rmx_close(hi_handle rmx_handle);
hi_s32 hi_drv_dmx_rmx_destroy(hi_handle rmx_handle);

/*********band begin************/
hi_s32 hi_drv_dmx_band_open(dmx_band port, const dmx_band_attr *attrs, hi_handle *handle, struct dmx_session *session);
hi_s32 hi_drv_dmx_band_ref_inc(dmx_band band, const dmx_band_attr *attrs, hi_handle *handle, struct dmx_session *session);
hi_s32 hi_drv_dmx_band_attach_port(hi_handle handle, dmx_port port);
hi_s32 hi_drv_dmx_band_detach_port(hi_handle handle);
hi_s32 hi_drv_dmx_band_get_attrs(hi_handle handle, dmx_band_attr *attrs);
hi_s32 hi_drv_dmx_band_set_attrs(hi_handle handle, const dmx_band_attr *attrs);
hi_s32 hi_drv_dmx_band_get_status(hi_handle handle, dmx_band_status *status);
hi_s32 hi_drv_dmx_band_close(hi_handle handle);
hi_s32 hi_drv_dmx_band_ref_dec(hi_handle handle);
hi_s32 hi_drv_dmx_band_tei_set(hi_handle handle, hi_bool tei);

/*********pid_ch begin************/
hi_s32 hi_drv_dmx_pid_ch_create(hi_handle band_handle, hi_u32 pid, hi_handle *handle, struct dmx_session *session);
hi_s32 hi_drv_dmx_pid_ch_ref_inc(hi_handle band_handle, hi_u32 pid, hi_handle *handle, struct dmx_session *session);
hi_s32 hi_drv_dmx_pid_ch_get_handle(hi_handle band_handle, hi_u32 pid, hi_handle *handle);
hi_s32 hi_drv_dmx_pid_ch_get_status(hi_handle handle, dmx_pidch_status *status);
hi_s32 hi_drv_dmx_pid_ch_lock_out(hi_handle handle);
hi_s32 hi_drv_dmx_pid_ch_un_lock_out(hi_handle handle);
hi_s32 hi_drv_dmx_pid_ch_destroy(hi_handle handle);
hi_s32 hi_drv_dmx_pid_ch_ref_dec(hi_handle handle);
hi_s32 hi_drv_dmx_pid_ch_get_free_cnt(hi_u32 *free_cnt);

/*********play_fct begin************/
hi_s32 hi_drv_dmx_play_create(const dmx_play_attrs *attrs, hi_handle *handle, struct dmx_session *session);
hi_s32 hi_drv_dmx_play_open(hi_handle handle);
hi_s32 hi_drv_dmx_play_get_attrs(hi_handle handle, dmx_play_attrs *attrs);
hi_s32 hi_drv_dmx_play_set_attrs(hi_handle handle, const dmx_play_attrs *attrs);
hi_s32 hi_drv_dmx_play_get_status(hi_handle handle, dmx_play_status *status);
hi_s32 hi_drv_dmx_play_get_scrambled_flag(hi_handle handle, hi_dmx_scrambled_flag *scramble_flag);
hi_s32 hi_drv_dmx_play_get_packet_num(hi_handle handle, dmx_chan_packet_num *chn_stat);
hi_s32 hi_drv_dmx_play_get_pid_ch(hi_handle handle, hi_handle *pid_handle);
hi_s32 hi_drv_dmx_play_attach_pid_ch(hi_handle handle, hi_handle pid_handle);
hi_s32 hi_drv_dmx_play_detach_pid_ch(hi_handle handle);
hi_s32 hi_drv_dmx_play_create_filter(const dmx_filter_attrs *attrs, hi_handle *flt_handle,
    struct dmx_session *session);
hi_s32 hi_drv_dmx_play_add_filter(hi_handle handle, hi_handle flt_handle);
hi_s32 hi_drv_dmx_play_del_filter(hi_handle handle, hi_handle flt_handle);
hi_s32 hi_drv_dmx_play_del_all_filter(hi_handle handle);
hi_s32 hi_drv_dmx_play_update_filter(hi_handle flt_handle, const dmx_filter_attrs *attrs);
hi_s32 hi_drv_dmx_play_get_filter(hi_handle flt_handle, dmx_filter_attrs *attrs);
hi_s32 hi_drv_dmx_play_get_handle_by_filter(hi_handle flt_handle, hi_handle *play_handle);
hi_s32 hi_drv_dmx_play_get_free_filter_cnt(hi_u32 *free_flt_cnt);
hi_s32 hi_drv_dmx_play_destroy_filter(hi_handle flt_handle);
hi_s32 hi_drv_dmx_play_get_bufhandle(hi_handle handle, hi_handle *buf_handle, dmx_play_type *type);

hi_s32 hi_drv_dmx_play_acquire_buf(hi_handle handle, hi_u32 acq_num, hi_u32 time_out, hi_u32 *acqed_num,
    dmx_buffer *play_fct_buf);
hi_s32 hi_drv_dmx_play_release_buf(hi_handle handle, hi_u32 rel_num, dmx_buffer *play_fct_buf);
hi_s32 hi_drv_dmx_play_reset_buf(hi_handle handle);

hi_s32 hi_drv_dmx_play_start_idx(hi_handle handle);
hi_s32 hi_drv_dmx_play_recv_idx(hi_handle handle, hi_u32 req_num, hi_u32 time_out, hi_u32 *reqed_num,
    dmx_index_data *index);
hi_s32 hi_drv_dmx_play_stop_idx(hi_handle handle);

hi_s32 hi_drv_dmx_play_close(hi_handle handle);
hi_s32 hi_drv_dmx_play_destroy(hi_handle handle);

hi_s32 hi_drv_dmx_play_pre_mmap(hi_handle handle, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr);
hi_s32 hi_drv_dmx_play_pst_mmap(hi_handle handle, hi_void *buf_usr_addr);

hi_s32 hi_drv_dmx_play_get_data_handle(hi_handle *valid_array, hi_u32 *valid_num, hi_u32 watch_num,
    hi_u32 time_out_ms);
hi_s32 hi_drv_dmx_play_select_data_handle(hi_handle *watch_array, hi_u32 watch_num,
    hi_handle *valid_array, hi_u32 *valid_num, hi_u32 time_out_ms);
hi_s32 hi_drv_dmx_play_set_eos_flag(hi_handle handle);
hi_s32 hi_drv_dmx_play_cc_repeat_set(hi_handle handle, dmx_chan_cc_repeat_mode mode);
hi_s32 hi_drv_dmx_play_pusi_set(hi_bool no_pusi);

/*********rec_fct begin************/
hi_s32 hi_drv_dmx_rec_create(const dmx_rec_attrs *attrs, hi_handle *handle, struct dmx_session *session);
hi_s32 hi_drv_dmx_rec_open(hi_handle handle);
hi_s32 hi_drv_dmx_rec_get_attrs(hi_handle handle, dmx_rec_attrs *attrs);
hi_s32 hi_drv_dmx_rec_set_attrs(hi_handle handle, const dmx_rec_attrs *attrs);
hi_s32 hi_drv_dmx_rec_set_eos_flag(hi_handle handle, hi_bool eos_flag);
hi_s32 hi_drv_dmx_rec_get_status(hi_handle handle, dmx_rec_status *status);
hi_s32 hi_drv_dmx_rec_add_ch(hi_handle handle, hi_handle ch_handle);
hi_s32 hi_drv_dmx_rec_del_ch(hi_handle handle, hi_handle ch_handle);
hi_s32 hi_drv_dmx_rec_del_all_ch(hi_handle handle);

hi_s32 hi_drv_dmx_rec_acquire_buf(hi_handle handle, hi_u32 req_len, hi_u32 time_out, dmx_buffer *rec_fct_buf);
hi_s32 hi_drv_dmx_rec_release_buf(hi_handle handle, dmx_buffer *rec_fct_buf);

hi_s32 hi_drv_dmx_rec_recv_idx(hi_handle handle, hi_u32 req_num, hi_u32 time_out, hi_u32 *reqed_num,
    dmx_index_data *index);
hi_s32 hi_drv_dmx_rec_peek_idx_and_buf(hi_handle handle, hi_u32 time_out, hi_u32 *index_num, hi_u32 *rec_data_len);

hi_s32 hi_drv_dmx_rec_close(hi_handle handle);
hi_s32 hi_drv_dmx_rec_destroy(hi_handle handle);

hi_s32 hi_drv_dmx_rec_pre_mmap(hi_handle handle, hi_s64 *buf_handle, hi_u32 *buf_size, hi_void **buf_usr_addr);
hi_s32 hi_drv_dmx_rec_pst_mmap(hi_handle handle, hi_void *buf_usr_addr);
hi_s32 hi_drv_dmx_rec_get_bufhandle(hi_handle handle, hi_handle *buf_handle);

/*********dsc_fct begin************/
hi_s32 hi_drv_dmx_dsc_create(const dmx_dsc_attrs *attrs, hi_handle *handle, struct dmx_session *session);
hi_s32 hi_drv_dmx_dsc_get_attrs(hi_handle handle, dmx_dsc_attrs *attrs);
hi_s32 hi_drv_dmx_dsc_set_attrs(hi_handle handle, const dmx_dsc_attrs *attrs);
hi_s32 hi_drv_dmx_dsc_attach(hi_handle handle, hi_handle ch_handle);
hi_s32 hi_drv_dmx_dsc_detach(hi_handle handle, hi_handle ch_handle);
hi_s32 hi_drv_dmx_dsc_attach_keyslot(hi_handle handle, hi_handle ks_handle);
hi_s32 hi_drv_dmx_dsc_detach_keyslot(hi_handle handle);
hi_s32 hi_drv_dmx_dsc_get_keyslot_handle(hi_handle handle, hi_handle *ks_handle);
hi_s32 hi_drv_dmx_dsc_set_even_key(hi_handle handle, const hi_u8 *key, hi_u32 len);
hi_s32 hi_drv_dmx_dsc_set_odd_key(hi_handle handle, const hi_u8 *key, hi_u32 len);
hi_s32 hi_drv_dmx_dsc_set_sys_key(hi_handle handle, const hi_u8 *key, hi_u32 len);
hi_s32 hi_drv_dmx_dsc_set_even_iv(hi_handle handle, const hi_u8 *iv, hi_u32 len);
hi_s32 hi_drv_dmx_dsc_set_odd_iv(hi_handle handle, const hi_u8 *iv, hi_u32 len);
hi_s32 hi_drv_dmx_dsc_destroy(hi_handle handle);
hi_s32 hi_drv_dmx_dsc_get_dsc_key_handle(hi_handle pid_ch_handle, hi_handle *dsc_handle);
hi_s32 hi_drv_dmx_dsc_get_free_cnt(hi_u32 *free_cnt);

/*********pcr_fct begin************/
hi_s32 hi_drv_dmx_pcr_create(hi_handle band_handle, hi_u32 pid, hi_handle *handle, struct dmx_session *session);
hi_s32 hi_drv_dmx_pcr_attach(hi_handle handle, hi_handle sync_handle);
hi_s32 hi_drv_dmx_pcr_detach(hi_handle handle);
hi_s32 hi_drv_dmx_pcr_get_status(hi_handle handle, dmx_pcr_status *status);
hi_s32 hi_drv_dmx_pcr_get_pcr_scr(hi_handle handle, hi_u64 *pcr_ms, hi_u64 *scr_ms);
hi_s32 hi_drv_dmx_pcr_destroy(hi_handle handle);

hi_s32 hi_drv_dmx_suspend(hi_void);
hi_s32 hi_drv_dmx_resume(hi_void);

/* for demux module load and unload */
hi_s32  dmx_mod_init(hi_void);
hi_void dmx_mod_exit(hi_void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
