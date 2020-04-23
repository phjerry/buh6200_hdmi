/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2008-2019. All rights reserved.
 * Description: Define functions used to test function of frontend driver
 * Author: sdk
 * Create: 2008-06-05
 */

#ifndef __HI_DRV_FRONTEND_H__
#define __HI_DRV_FRONTEND_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define DRV_FRONTEND_NUM                (8)

#define HI_DRV_DISEQC_MSG_MAX_LENGTH     (6)   /** DiSEqC message length */  /** CNcomment:DiSEqC消息长度 */
#define HI_DRV_DISEQC_MAX_REPEAT_TIMES   (4)   /** DiSEqC message max repeat times */ /** CNcomment:DiSEqC消息最大重复发送次数 */
#define HI_DRV_DISEQC_MAX_MOTOR_PISITION (255) /** DiSEqC motor max stored position */ /** CNcomment:DiSEqC马达最大存储星位数 */

#define HI_DRV_MAX_TS_LINE         (11)  /** The NO of ts lines that can be configured */ /** CNcomment:可配置的ts信号线数量 */

/*************************** Structure Definition ****************************/
/** \addtogroup      FRONTEND */
/** @{ */  /** <!-- [FRONTEND] */

/** Signal type of the frontend */
/** CNcomment:前端信号类型 */
typedef enum {
    HI_DRV_FRONTEND_SIG_TYPE_DVB_C   = 0x000001, /** <DVB-C signal */     /** <CNcomment:DVB_C信号 */
    HI_DRV_FRONTEND_SIG_TYPE_DVB_C2  = 0x000002, /** <DVB-C2 signal */    /** <CNcomment:DVB-C2信号 */
    HI_DRV_FRONTEND_SIG_TYPE_ISDB_C  = 0x000004, /** <ISDB-C signal */    /** <CNcomment:ISDB-C信号 */
    HI_DRV_FRONTEND_SIG_TYPE_J83B    = 0x000008, /** <J83B signal */      /** <CNcomment:J83B信号 */
    HI_DRV_FRONTEND_SIG_TYPE_DVB_S   = 0x100001, /** <DVB-S signal */     /** <CNcomment:DVB-S2信号 */
    HI_DRV_FRONTEND_SIG_TYPE_DVB_S2  = 0x100002, /** <DVB-S2 signal */    /** <CNcomment:DVB-S2X信号 */
    HI_DRV_FRONTEND_SIG_TYPE_DVB_S2X = 0x100004, /** <DVB-S2X signal */   /** <CNcomment:地面信号 */
    HI_DRV_FRONTEND_SIG_TYPE_ISDB_S  = 0x100008, /** <ISDB-S signal */    /** <CNcomment:ISDB-S信号 */
    HI_DRV_FRONTEND_SIG_TYPE_ISDB_S3 = 0x100010, /** <ISDB-S3 signal */  /** <CNcomment:ISDB-S3信号 */
    HI_DRV_FRONTEND_SIG_TYPE_ABSS    = 0x100020, /** <ABS-S signal */    /** <CNcomment: 直播星卫星信号 */
    HI_DRV_FRONTEND_SIG_TYPE_DVB_T   = 0x200001, /** <DVB-T signal */    /** <CNcomment: DVB-T信号 */
    HI_DRV_FRONTEND_SIG_TYPE_DVB_T2  = 0x200002, /** <DVB-T2 signal */   /** <CNcomment:DVB-T2信号 */
    HI_DRV_FRONTEND_SIG_TYPE_ISDB_T  = 0x200004, /** <ISDB-T signal */   /** <CNcomment:ISDB-T信号 */
    HI_DRV_FRONTEND_SIG_TYPE_ATSC_T  = 0x200008, /** <ATSC-T signal */   /** <CNcomment:ATSC-T信号 */
    HI_DRV_FRONTEND_SIG_TYPE_ATSC_T3 = 0x200010, /** <ATSC-T3 signal */  /** <CNcomment:ATSC-T3信号 */
    HI_DRV_FRONTEND_SIG_TYPE_DTMB    = 0x200020, /** <DTMB signal */     /** <CNcomment:DTMB信号 */
    HI_DRV_FRONTEND_SIG_TYPE_MAX                 /** <Invalid value */    /** <CNcomment:非法值 */
} hi_drv_frontend_sig_type;

/** type of TUNER device */
/** CNcomment:TUNER设备类型 */
typedef enum {
    HI_DRV_TUNER_DEV_TYPE_XG_3BL        = 0,    /** <XG_3BL,Not supported */     /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_CD1616        = 1,    /** <CD1616,Not supported */     /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_ALPS_TDAE     = 2,    /** <ALPS_TDAE,Not supported */  /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_TDCC          = 3,    /** <TDCC,Not supported */       /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_TDA18250      = 4,    /** <TDA18250 */                 /** <CNcomment:支持TDA18250射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_CD1616_DOUBLE = 5,    /** <CD1616 with double agc, Not supported */ /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_MT2081        = 6,    /** <MT2081,Not supported */     /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_TMX7070X      = 7,    /** <THOMSON7070X, Not supported */  /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_R820C         = 8,    /** <R820C */                    /** <CNcomment:支持R820C射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_MXL203        = 9,    /** <MXL203 */                   /** <CNcomment:支持MXL203射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_AV2011        = 10,   /** <AV2011 */                   /** <CNcomment:支持AV2011射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_SHARP7903     = 11,   /** <SHARP7903,Not supported */  /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_MXL101        = 12,   /** <MXL101,Not supported */     /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_MXL603        = 13,   /** <MXL603 */                   /** <CNcomment:支持MXL603射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_IT9170        = 14,   /** <IT9170,Not supported */     /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_IT9133        = 15,   /** <IT9133,Not supported */     /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_TDA6651       = 16,   /** <TDA6651,Not supported */    /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_TDA18250B     = 17,   /** <TDA18250B */                /** <CNcomment:支持TDA18250B射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_M88TS2022     = 18,   /** <M88TS2022,Not supported */  /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_RDA5815       = 19,   /** <RDA5815 */                  /** <CNcomment:支持RDA5815射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_MXL254        = 20,   /** <MXL254 */                   /** <CNcomment:支持MXL254射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_CXD2861       = 21,   /** <CXD2861,Not supported */    /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_SI2147        = 22,   /** <Si2147 */                   /** <CNcomment:支持Si2147射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_RAFAEL836     = 23,   /** <Rafael836 */                /** <CNcomment:支持Rafael836射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_MXL608        = 24,   /** <MXL608 */                   /** <CNcomment:支持MXL608射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_MXL214        = 25,   /** <MXL214 */                   /** <CNcomment:支持MXL214射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_TDA18280      = 26,   /** <TDA18280 */                 /** <CNcomment:支持TDA18280射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_TDA182I5A     = 27,   /** <TDA182I5A */                /** <CNcomment:支持TDA182I5A芯片 */
    HI_DRV_TUNER_DEV_TYPE_SI2144        = 28,   /** <Si2144 */                   /** <CNcomment:支持Si2144芯片 */
    HI_DRV_TUNER_DEV_TYPE_AV2018        = 29,   /** <AV2018 */                   /** <CNcomment:支持AV2018芯片 */
    HI_DRV_TUNER_DEV_TYPE_MXL251        = 30,   /** <MXL251 */                   /** <CNcomment:支持MXL251芯片 */
    HI_DRV_TUNER_DEV_TYPE_M88TC3800     = 31,   /** <M88TC3800,Not supported */  /** <CNcomment:不支持 */
    HI_DRV_TUNER_DEV_TYPE_MXL601        = 32,   /** < MXL601 */                  /** <CNcomment:支持MXL601射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_MXL683        = 33,   /** <MXL683 */                   /** <CNcomment:支持MXL683射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_AV2026        = 34,   /** <AV2026 */                   /** <CNcomment:支持AV2026射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_R850          = 35,   /** <R850 */                     /** <CNcomment:支持R850射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_R858          = 36,   /** <R858 */                     /** <CNcomment:支持R858射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_MXL541        = 37,   /** <MXL541 */                   /** <CNcomment:支持MXL541射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_MXL581        = 38,   /** <MXL581 */                   /** <CNcomment:支持MXL581射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_MXL582        = 39,   /** <MXL582 */                   /** <CNcomment:支持MXL582射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_MXL661        = 40,   /** <MXL661 */                   /** <CNcomment:支持MXL661射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_RT720         = 41,   /** <RT720 */                    /** <CNcomment:支持RT720射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_CXD2871       = 42,   /** <CXD2871 */                  /** <CNcomment:支持CXD2871射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_SUT_PJ987     = 43,   /** <SUT-PJ987 */                /** <CNcomment:支持SUT-PJ987射频芯片 */
    HI_DRV_TUNER_DEV_TYPE_MAX                   /** <Invalid value */            /** <CNcomment:非法边界值 */
} hi_drv_tuner_dev_type;

/** Type of the demod device */
/** CNcomment:demod设备类型 */
typedef enum {
    HI_DRV_DEMOD_DEV_TYPE_NONE,                 /** <Not supported */             /** <CNcomment:不支持 */
    HI_DRV_DEMOD_DEV_TYPE_3130I       = 0x100,  /** <Internal QAM, same as HI_DRV_DEMOD_DEV_TYPE_INTERNAL0 */
                                                /** <CNcomment:内部QAM,与HI_DRV_DEMOD_DEV_TYPE_INTERNAL0相同 */
    HI_DRV_DEMOD_DEV_TYPE_3130E       = 0x101,  /** <External Hi3130 */           /** <CNcomment:外部QAM hi3130芯片 */
    HI_DRV_DEMOD_DEV_TYPE_J83B        = 0x102,  /** <suppoort j83b */             /** <CNcomment:支持j83b */
    HI_DRV_DEMOD_DEV_TYPE_AVL6211     = 0x103,  /** <Avalink 6211 */              /** <CNcomment:支持Avalink 6211 */
    HI_DRV_DEMOD_DEV_TYPE_MXL101      = 0x104,  /** <Maxlinear mxl101, Not supported */ /** <CNcomment:不支持 */
    HI_DRV_DEMOD_DEV_TYPE_MN88472     = 0x105,  /** <PANASONIC mn88472 */         /** <CNcomment:支持P mn88472 */
    HI_DRV_DEMOD_DEV_TYPE_IT9170      = 0x106,  /** <ITE it9170,Not supported */  /** <CNcomment:不支持 */
    HI_DRV_DEMOD_DEV_TYPE_IT9133      = 0x107,  /** <ITE it9133,Not supported */  /** <CNcomment:不支持 */
    HI_DRV_DEMOD_DEV_TYPE_3136        = 0x108,  /** <External Hi3136 */           /** <CNcomment:外部hi3136芯片 */
    HI_DRV_DEMOD_DEV_TYPE_MXL254      = 0x10A,  /** <External mxl254 */           /** <CNcomment:外部MXL254芯片 */
    HI_DRV_DEMOD_DEV_TYPE_CXD2837     = 0x10B,  /** <Sony cxd2837,Not supported */   /** <CNcomment:不支持 */
    HI_DRV_DEMOD_DEV_TYPE_3137        = 0x10C,  /** <External Hi3137 */           /** <CNcomment:支持外部hi3137芯片 */
    HI_DRV_DEMOD_DEV_TYPE_MXL214      = 0x10D,  /** <External mxl214 */           /** <CNcomment:外部MXL214芯片 */
    HI_DRV_DEMOD_DEV_TYPE_TDA18280    = 0x10E,  /** <External tda18280 */         /** <CNcomment:外部tda18280芯片 */
    HI_DRV_DEMOD_DEV_TYPE_MXL251      = 0x111,  /** <External mxl251 */           /** <CNcomment:外部MXL251芯片 */
    HI_DRV_DEMOD_DEV_TYPE_ATBM888X    = 0x113,  /** <External ATBM888X */         /** <CNcomment:支持外部ATBM888X芯片 */
    HI_DRV_DEMOD_DEV_TYPE_MN88473     = 0x114,  /** <Panasonic 88473 */           /** <CNcomment:支持Panasonic 88473 */
    HI_DRV_DEMOD_DEV_TYPE_MXL683      = 0x115,  /** <External mxl683 */           /** <CNcomment:外部mxl683芯片 */
    HI_DRV_DEMOD_DEV_TYPE_TP5001      = 0x116,  /** <External TP5001 */           /** <CNcomment:支持外部TP5001芯片 */
    HI_DRV_DEMOD_DEV_TYPE_HD2501      = 0x117,  /** <External HD2501 */           /** <CNcomment:支持外部HD2501芯片 */
    HI_DRV_DEMOD_DEV_TYPE_AVL6381     = 0x118,  /** <External AVL6381 */          /** <CNcomment:支持外部AVL6381芯片 */
    HI_DRV_DEMOD_DEV_TYPE_MXL541      = 0x119,  /** <External MXL541 */           /** <CNcomment:支持外部MXL541芯片 */
    HI_DRV_DEMOD_DEV_TYPE_MXL581      = 0x11A,  /** <External MXL581 */           /** <CNcomment:支持外部MXL581芯片 */
    HI_DRV_DEMOD_DEV_TYPE_MXL582      = 0x11B,  /** <External MXL582 */           /** <CNcomment:支持外部MXL582芯片 */
    HI_DRV_DEMOD_DEV_TYPE_INTERNAL0   = 0x11C,  /** <Internal Demod */            /** <CNcomment:支持内部解调芯片 */
    HI_DRV_DEMOD_DEV_TYPE_CXD2856     = 0x11D,  /** <External CXD2856 */          /** <CNcomment:支持外部CXD2856芯片 */
    HI_DRV_DEMOD_DEV_TYPE_CXD2857     = 0x11E,  /** <External CXD2857 */          /** <CNcomment:支持外部CXD2857芯片 */
    HI_DRV_DEMOD_DEV_TYPE_CXD2878     = 0x11F,  /** <External CXD2878 */          /** <CNcomment:支持外部CXD2878芯片 */
    HI_DRV_DEMOD_DEV_TYPE_MAX                   /** <Invalid value */             /** <CNcomment:非法边界值 */
} hi_drv_demod_dev_type;

/** frontend attribution. */
/** CNcomment:前端属性 */
typedef struct {
    hi_drv_frontend_sig_type  sig_type;          /** <Signal type */       /** <CNcomment:信号类型 */
    hi_drv_demod_dev_type     demod_dev_type;    /** <QAM type */          /** <CNcomment:QAM类型 */
    hi_u32      demod_addr;        /* <The i2c address of demod */
    hi_u16      demod_clk;         /* <Demod clock freqency, unit:kHz */
    hi_u16      demod_i2c_channel; /** <I2C channel used by the demod */ /** <CNcomment:DEMOD使用的I2C通道 */
    hi_u16      ext_dem_reset_gpio_group; /** <Demod reset GPIO group NO. */ /** <CNcomment:复位使用的gpio管脚组号 */
    hi_u16      ext_dem_reset_gpio_bit;   /** <Demod reset GPIO bit NO. */ /** <CNcomment:复位使用的gpio管脚号 */
    hi_drv_tuner_dev_type     tuner_dev_type;    /** <Tuner type */        /** <CNcomment:TUNER类型 */
    hi_u32      tuner_addr;        /* The i2c address of tuner */
    hi_u16      tuner_i2c_channel; /** <I2C channel used by the tuner */ /** <CNcomment:TUNER使用的I2C通道 */

    /** <Only valid in Hi3716MV430 DVBT2, 0 is low memory mode, 1 is high memory mode.
         Low memory mode default,COMMON PLP is not supported in this mode. */
    /** <CNcomment: 仅对Hi3716MV430的DVB-T2有效。 0表示低内存模式，1表示高内存模式。默认为低内存模式，此模式不支持COMMON PLP */
    hi_u32                    memory_mode;
    hi_u8                     reserved[20];      /** <Reserved 20 byte for future use */  /* <CNcommit:保留20字节未来使用 */
} hi_drv_frontend_attr;

/** Modulation mode */
/** CNcomment: 调制方式 */
typedef enum {
    HI_DRV_MOD_TYPE_DEFAULT,     /** <Default QAM mode. The default QAM mode is HI_DRV_MOD_TYPE_QAM_64 at present. */
                                 /** <CNcomment:默认的QAM类型, 当前系统默认为HI_DRV_MOD_TYPE_QAM_64 */
    HI_DRV_MOD_TYPE_QAM_16 = 0x100,   /** <16QAM mode */          /** <CNcomment:16QAM对应的枚举值 */
    HI_DRV_MOD_TYPE_QAM_32,           /** <32QAM mode */          /** <CNcomment:32QAM对应的枚举值 */
    HI_DRV_MOD_TYPE_QAM_64,           /** <64QAM mode */          /** <CNcomment:64QAM对应的枚举值 */
    HI_DRV_MOD_TYPE_QAM_128,          /** <128QAM mode */         /** <CNcomment:128QAM对应的枚举值 */
    HI_DRV_MOD_TYPE_QAM_256,          /** <256QAM mode */         /** <CNcomment:256QAM对应的枚举值 */
    HI_DRV_MOD_TYPE_QAM_512,          /** <512QAM mode */         /** <CNcomment:512QAM对应的枚举值 */

    HI_DRV_MOD_TYPE_BPSK = 0x200,     /** <binary phase shift keying (BPSK) mode. */     /** <CNcomment:BPSK对应的枚举值 */
    HI_DRV_MOD_TYPE_QPSK = 0x300,     /** <quaternary phase shift keying (QPSK) mode. */ /** <CNcomment:QPSK对应的枚举值 */
    HI_DRV_MOD_TYPE_DQPSK,
    HI_DRV_MOD_TYPE_8PSK,             /** <8 phase shift keying (8PSK) mode */           /** <CNcomment:8PSK对应的枚举值 */
    HI_DRV_MOD_TYPE_16APSK,           /** <16-Ary Amplitude and Phase Shift Keying (16APSK) mode */
                                      /** <CNcomment:16APSK对应的枚举值 */
    HI_DRV_MOD_TYPE_32APSK,           /** <32-Ary Amplitude and Phase Shift Keying (32APSK) mode */
                                      /** <CNcomment:32APSK对应的枚举值 */
    HI_DRV_MOD_TYPE_64APSK,           /** <64-Ary Amplitude and Phase Shift Keying (64APSK) mode */
                                      /** <CNcomment:64APSK对应的枚举值 */
    HI_DRV_MOD_TYPE_128APSK,          /** <128-Ary Amplitude and Phase Shift Keying (128APSK) mode */
                                      /** <CNcomment:128APSK对应的枚举值 */
    HI_DRV_MOD_TYPE_256APSK,          /** <256-Ary Amplitude and Phase Shift Keying (256APSK) mode */
                                      /** <CNcomment:256APSK对应的枚举值 */
    HI_DRV_MOD_TYPE_8VSB,             /** <(8VSB) mode */           /** <CNcomment:8VSB对应的枚举值 */
    HI_DRV_MOD_TYPE_16VSB,            /** <(16VSB) mode */          /** <CNcomment:16VSB对应的枚举值 */
    HI_DRV_MOD_TYPE_VLSNR_SET1,       /** <(VLSNR_SET1) mode,only used in DVB-S2X */
                                      /** <CNcomment:VLSNR_SET1对应的枚举值，DVB-S2X新增 */
    HI_DRV_MOD_TYPE_VLSNR_SET2,       /** <(VLSNR_SET2) mode,only used in DVB-S2X */
                                      /** <CNcomment:VLSNR_SET2对应的枚举值，DVB-S2X新增 */
    HI_DRV_MOD_TYPE_8APSK,    /** <(8APSK) mode,only used in DVB-S2x */  /** <CNcomment:8APSK对应的枚举值，DVB-S2X新增 */

    /** <Enumeration corresponding to the auto mode. For DVB-S/S2,
         if detect modulation type fail, it will return auto */
    /** <CNcomment:卫星信号调制方式自动检测，如果检测失败返回AUTO */
    HI_DRV_MOD_TYPE_AUTO,

    HI_DRV_MOD_TYPE_MAX              /** <Invalid Modulation mode */       /** <CNcomment:非法的调制类型枚举值 */
} hi_drv_modulation_type;

/** FEC code Rate */
/** CNcomment:FEC码率 */
typedef enum {
    HI_DRV_FRONTEND_FEC_RATE_AUTO = 0,
    HI_DRV_FRONTEND_FEC_RATE_1_2,           /** <1/2 */           /** <CNcomment:1/2码率 */
    HI_DRV_FRONTEND_FEC_RATE_2_3,           /** <2/3 */           /** <CNcomment:2/3码率 */
    HI_DRV_FRONTEND_FEC_RATE_3_4,           /** <3/4 */           /** <CNcomment:3/4码率 */
    HI_DRV_FRONTEND_FEC_RATE_4_5,           /** <4/5 */           /** <CNcomment:4/5码率 */
    HI_DRV_FRONTEND_FEC_RATE_5_6,           /** <5/6 */           /** <CNcomment:5/6码率 */
    HI_DRV_FRONTEND_FEC_RATE_6_7,           /** <6/7 */           /** <CNcomment:6/7码率 */
    HI_DRV_FRONTEND_FEC_RATE_7_8,           /** <7/8 */           /** <CNcomment:7/8码率 */
    HI_DRV_FRONTEND_FEC_RATE_8_9,           /** <8/9 */           /** <CNcomment:8/9码率 */
    HI_DRV_FRONTEND_FEC_RATE_9_10,          /** <9/10 */          /** <CNcomment:9/10码率 */
    HI_DRV_FRONTEND_FEC_RATE_1_4,           /** <1/4 */           /** <CNcomment:1/4码率 */
    HI_DRV_FRONTEND_FEC_RATE_1_3,           /** <1/3 */           /** <CNcomment:1/3码率 */
    HI_DRV_FRONTEND_FEC_RATE_2_5,           /** <2/5 */           /** <CNcomment:2/5码率 */
    HI_DRV_FRONTEND_FEC_RATE_3_5,           /** <3/5 */           /** <CNcomment:3/5码率 */
    HI_DRV_FRONTEND_FEC_RATE_13_45,         /** <13/45 */         /** <CNcomment:13/45码率 */
    HI_DRV_FRONTEND_FEC_RATE_9_20,          /** <9/20 */          /** <CNcomment:9/20码率 */
    HI_DRV_FRONTEND_FEC_RATE_11_20,         /** <11/20 */         /** <CNcomment:11/20码率 */
    HI_DRV_FRONTEND_FEC_RATE_5_9_L,         /** <5/9L */          /** <CNcomment:5/9L码率 */
    HI_DRV_FRONTEND_FEC_RATE_26_45_L,       /** <26/45L */        /** <CNcomment:26/45L码率 */
    HI_DRV_FRONTEND_FEC_RATE_23_36,         /** <23/36 */         /** <CNcomment:23/36码率 */
    HI_DRV_FRONTEND_FEC_RATE_25_36,         /** <25/36 */         /** <CNcomment:25/36码率 */
    HI_DRV_FRONTEND_FEC_RATE_13_18,         /** <13/18 */         /** <CNcomment:13/18码率 */
    HI_DRV_FRONTEND_FEC_RATE_1_2_L,         /** <1/2L */          /** <CNcomment:1/2L码率 */
    HI_DRV_FRONTEND_FEC_RATE_8_15_L,        /** <8/15L */         /** <CNcomment:8/15L码率 */
    HI_DRV_FRONTEND_FEC_RATE_26_45,         /** <26/45 */         /** <CNcomment:26/45码率 */
    HI_DRV_FRONTEND_FEC_RATE_3_5_L,         /** <3/5L */          /** <CNcomment:3/5L码率 */
    HI_DRV_FRONTEND_FEC_RATE_28_45,         /** <28/45 */         /** <CNcomment:28/45码率 */
    HI_DRV_FRONTEND_FEC_RATE_2_3_L,         /** <2/3L */          /** <CNcomment:2/3L码率 */
    HI_DRV_FRONTEND_FEC_RATE_7_9,           /** <7/9 */           /** <CNcomment:7/9码率 */
    HI_DRV_FRONTEND_FEC_RATE_77_90,         /** <77/90 */         /** <CNcomment:77/90码率 */
    HI_DRV_FRONTEND_FEC_RATE_32_45,         /** <32/45 */         /** <CNcomment:32/45码率 */
    HI_DRV_FRONTEND_FEC_RATE_11_15,         /** <11/15 */         /** <CNcomment:11/15码率 */
    HI_DRV_FRONTEND_FEC_RATE_32_45_L,       /** <32/45 */         /** <CNcomment:32/45L码率 */
    HI_DRV_FRONTEND_FEC_RATE_29_45_L,       /** <29/45L */        /** <CNcomment:29/45L码率 */
    HI_DRV_FRONTEND_FEC_RATE_31_45_L,       /** <31/45 */         /** <CNcomment:31/45L码率 */
    HI_DRV_FRONTEND_FEC_RATE_11_15_L,       /** <11/15L */        /** <CNcomment:11/15L码率 */
    HI_DRV_FRONTEND_FEC_RATE_11_45,         /** <11/45 */         /** <CNcomment:11/45码率 */
    HI_DRV_FRONTEND_FEC_RATE_4_15,          /** <4/15 */          /** <CNcomment:4/15码率 */
    HI_DRV_FRONTEND_FEC_RATE_14_45,         /** <14/45 */         /** <CNcomment:14/45码率 */
    HI_DRV_FRONTEND_FEC_RATE_7_15,          /** <7/15 */          /** <CNcomment:7/15码率 */
    HI_DRV_FRONTEND_FEC_RATE_8_15,          /** <8/15 */          /** <CNcomment:8/15码率 */
    HI_DRV_FRONTEND_FEC_RATE_2_9,           /** <2/9 */           /** <CNcomment:2/9码率 */
    HI_DRV_FRONTEND_FEC_RATE_1_5,           /** <2/9 */           /** <CNcomment:2/9码率 */
    HI_DRV_FRONTEND_FEC_RATE_MAX       /** <Invalid value */ /** <CNcomment:非法边界值 */
}  hi_drv_frontend_fec_rate;

/** Demod ADC port select */
/** CNcomment:Demod ADC端口选择 */
typedef enum {
    HI_DRV_DEMOD_ADC_0,          /** <ADC0 */   /** <CNcomment:ADC0端口 */
    HI_DRV_DEMOD_ADC_1,          /** <ADC1 */   /** <CNcomment:ADC1端口 */
    HI_DRV_DEMOD_ADC_MAX
} hi_drv_demod_adc;

/** Tuner RF AGC mode */
/** CNcomment:agc控制模式 */
typedef enum {
    /** < Inverted polarization, default.This setting is used for a tuner whose
          gain decreases with increased AGC voltage */
    /** <CNcomment:agc反向控制模式 */
    HI_DRV_TUNER_RF_AGC_MODE_INVERT,

    /** < Normal polarization. This setting is used for a tuner whose gain increases with increased AGC voltage */
    /** <CNcomment:agc正向控制模式 */
    HI_DRV_TUNER_RF_AGC_MODE_NORMAL,
    HI_DRV_TUNER_RF_AGC_MODE_MAX          /** <Invalid value */    /** <CNcomment:非法边界值 */
} hi_drv_tuner_rf_agc_mode;

/** Tuner IQ spectrum mode */
/** CNcomment:IQ模式 */
typedef enum {
    HI_DRV_TUNER_IQ_SPECTRUM_MODE_NORMAL, /** <The received signal spectrum is not inverted */ /** <CNcomment:IQ不反转 */
    HI_DRV_TUNER_IQ_SPECTRUM_MODE_INVERT, /** <The received signal spectrum is inverted */     /** <CNcomment:IQ反转 */
    HI_DRV_TUNER_IQ_SPECTRUM_MODE_MAX     /** <Invalid value */                           /** <CNcomment:非法边界值 */
} hi_drv_tuner_iq_spectrum_mode;

/** Output mode of the demod */
/** CNcomment:Demod输出模式 */
typedef enum {
    HI_DRV_DEMOD_TS_MODE_DEFAULT               = 0,   /** <Default mode */
                                                             /** <CNcomment:默认模式 */
    HI_DRV_DEMOD_TS_MODE_PARALLEL_MODE_A       = 1,   /** <Parallel mode A */
                                                             /** <CNcomment:并行模式A */
    HI_DRV_DEMOD_TS_MODE_PARALLEL_MODE_B       = 2,   /** <Parallel mode B */
                                                             /** <CNcomment:并行模式B */
    HI_DRV_DEMOD_TS_MODE_SERIAL                = 3,   /** <Serial mode 74.25M */
                                                             /** <CNcomment:串行模74.25M */
    HI_DRV_DEMOD_TS_MODE_SERIAL_50             = 4,   /** <Serial mode 50M */
                                                             /** <CNcomment:串行模50M */
    HI_DRV_DEMOD_TS_MODE_SERIAL_2BIT           = 5,   /** <Serial mode witch 2 bit data line */
                                                             /** <CNcomment:2bit串行 */
    HI_DRV_DEMOD_TS_MODE_FULLBAND_PARALLEL     = 6,   /** <FULLBAND Parallel mode */
                                                             /** <CNcomment:FULLBAND 并行模式 */
    HI_DRV_DEMOD_TS_MODE_FULLBAND_SERIAL_3WIRE = 7,   /** <FULLBAND SERIAL three wire mode */
                                                             /** <CNcomment:FULLBAND 串行三线模式 */
    HI_DRV_DEMOD_TS_MODE_FULLBAND_SERIAL_4WIRE = 8,   /** <FULLBAND SERIAL four wire mode */
                                                             /** <CNcomment:FULLBAND 串行四线模式 */
    HI_DRV_DEMOD_TS_MODE_MAX  /** <Invalid value */  /** <CNcomment:非法值 */
} hi_drv_demod_ts_mode;

/** TS sync head length */
/** CNcomment: TS sync 头长度 */
typedef enum {
    HI_DRV_DEMOD_TS_SYNC_HEAD_AUTO,
    HI_DRV_DEMOD_TS_SYNC_HEAD_8BIT,
    HI_DRV_DEMOD_TS_SYNC_HEAD_MAX
} hi_drv_demod_ts_sync_head;

/** TS clock polarization */
/** CNcomment:TS时钟极性 */
typedef enum {
    HI_DRV_DEMOD_TS_CLK_POLAR_FALLING,       /** <Falling edge */      /** <CNcomment:下降沿 */
    HI_DRV_DEMOD_TS_CLK_POLAR_RISING,        /** <Rising edge */       /** <CNcomment:上升沿 */
    HI_DRV_DEMOD_TS_CLK_POLAR_MAX           /** <Invalid value */     /** <CNcomment:非法边界值 */
} hi_drv_demod_ts_clk_polar;

/** TS clock edge mode */
/** CNcomment:TS时钟边沿模式 */
typedef enum {
    HI_DRV_DEMOD_TSCLK_EDGE_SINGLE,     /** <signal edge */       /** <CNcomment:单沿 */
    HI_DRV_DEMOD_TSCLK_EDGE_DOUBLE,     /** <double edge */       /** <CNcomment:双沿 */
    HI_DRV_DEMOD_TSCLK_EDGE_MAX        /** <Invalid value */     /** <CNcomment:非法边界值 */
} hi_drv_demod_ts_clk_edge;

/** TS format */
/** CNcomment:TS格式 */
typedef enum {
    HI_DRV_DEMOD_TS_FORMAT_TS,              /** <188 */               /** <CNcomment:188字节格式 */
    HI_DRV_DEMOD_TS_FORMAT_TSP,             /** <204 */               /** <CNcomment:204字节格式 */
    HI_DRV_DEMOD_TS_FORMAT_MAX             /** <Invalid value */     /** <CNcomment:非法边界值 */
} hi_drv_demod_ts_format;

/** TS Output order */
/** CNcomment:TS输出线序 */
typedef enum {
    HI_DRV_DEMOD_TS_PIN_DAT0,        /** <data0 */                       /** <CNcomment:数据线0bit */
    HI_DRV_DEMOD_TS_PIN_DAT1,        /** <data1 */                       /** <CNcomment:数据线1bit */
    HI_DRV_DEMOD_TS_PIN_DAT2,        /** <data2 */                       /** <CNcomment:数据线2bit */
    HI_DRV_DEMOD_TS_PIN_DAT3,        /** <data3 */                       /** <CNcomment:数据线3bit */
    HI_DRV_DEMOD_TS_PIN_DAT4,        /** <data4 */                       /** <CNcomment:数据线4bit */
    HI_DRV_DEMOD_TS_PIN_DAT5,        /** <data5 */                       /** <CNcomment:数据线5bit */
    HI_DRV_DEMOD_TS_PIN_DAT6,        /** <data6 */                       /** <CNcomment:数据线6bit */
    HI_DRV_DEMOD_TS_PIN_DAT7,        /** <data7 */                       /** <CNcomment:数据线7bit */
    HI_DRV_DEMOD_TS_PIN_SYNC,        /** <sync */                        /** <CNcomment:sync信号线 */
    HI_DRV_DEMOD_TS_PIN_VLD,         /** <valid */                       /** <CNcomment:valid信号线 */
    HI_DRV_DEMOD_TS_PIN_ERR,         /** <err */                         /** <CNcomment:err信号线 */
    HI_DRV_DEMOD_TS_PIN_MAX          /** <Invalid value */               /** <CNcomment:非法值 */
} hi_drv_demod_ts_pin;

/** ts output port order */
/** CNcomment:ts接口输出线序 */
typedef struct {
    hi_drv_demod_ts_mode       ts_mode;   /** <Output mode of transport streams (TSs) */
                                               /** <CNcomment:TS流输出模式 */
    hi_drv_demod_ts_clk_polar  ts_clk_polar;  /** <TS clock polar */     /** <TS 时钟极性 */
    hi_drv_demod_ts_clk_edge   ts_clk_edge;   /** <TS clock edge mode */ /** <TS 时钟边沿模式 */
    hi_drv_demod_ts_format     ts_format;     /** <TS format */          /** <TS 格式 */
    hi_drv_demod_ts_pin        ts_pin[HI_DRV_MAX_TS_LINE];  /** <ts output port order */    /** <CNcomment:ts接口输出线序 */
} hi_drv_demod_ts_out;

/** Frequency locking status of the tuner */
/** CNcomment:信号锁频状态 */
typedef enum {
    HI_DRV_FRONTEND_LOCK_STATUS_DROPPED = 0,       /** <The signal is not locked. */  /** <CNcomment:信号未锁定 */
    HI_DRV_FRONTEND_LOCK_STATUS_LOCKED,            /** <The signal is locked. */      /** <CNcomment:信号已锁定 */
    HI_DRV_FRONTEND_LOCK_STATUS_MAX               /** <Invalid value */              /** <CNcomment:非法边界值 */
} hi_drv_frontend_lock_status;

typedef enum {
    HI_DRV_FRONTEND_STREAM_ID_TYPE_STREAM_ID = 0,          /** <Stream ID */     /** <CNcomment:流ID */
    HI_DRV_FRONTEND_STREAM_ID_TYPE_RELATIVE_STREAM_INDEX, /** <Relative stream number */ /** <CNcomment:流相对索引号 */
    HI_DRV_FRONTEND_STREAM_ID_TYPE_MAX                   /** <Invalid value */  /** <CNcomment:非法边界值 */
} hi_drv_frontend_stream_id_type;


/** BER and PER expressed scientific notation */
/** CNcomment:误码率和误包率，科学计数法表示 */
typedef struct {
    hi_u16 integer_val;  /** <Integer part */                    /** <CNcomment:整数部分 */
    hi_u16 decimal_val;  /** <Decimal part multiplied by 1000 */ /** <CNcomment:小数部分乘以1000 */
    hi_s32 power;        /** <power value */                     /** <CNcomment:指数部分 */
} hi_drv_frontend_scientific_num;

/** SNR interger part and decimar part multiplied by 100 */
/** CNcomment: 信噪比整数部分和小数部分乘以100 */
typedef struct {
    hi_s16 integer_val;  /** <Integer part */                   /** <CNcomment:整数部分 */
    hi_s16 decimal_val;  /** <Decimal part multiplied by 100 */ /** <CNcomment:小数部分乘以100 */
} hi_drv_frontend_integer_decimal;

/** Sample data, complex format */
/** CNcomment: 采集数据, 复格式 */
typedef struct {
    hi_s32 data_ip;   /* sample data, i component */    /** <CNcomment:采集数据的I分量 */
    hi_s32 data_qp;   /* sample data, q component */    /** <CNcomment:采集数据的Q分量 */
} hi_drv_frontend_sample_data;

/** Sample data length */
/** CNcomment: 采数长度 */
typedef enum {
    HI_DRV_FRONTEND_SAMPLE_DATA_LEN_32,         /** <sample 32 pts */
    HI_DRV_FRONTEND_SAMPLE_DATA_LEN_64,         /** <sample 64 pts */
    HI_DRV_FRONTEND_SAMPLE_DATA_LEN_128,        /** <sample 128 pts */
    HI_DRV_FRONTEND_SAMPLE_DATA_LEN_256,        /** <sample 256 pts */
    HI_DRV_FRONTEND_SAMPLE_DATA_LEN_512,        /** <sample 512 pts */  /** <CNcomment:采集512点 */
    HI_DRV_FRONTEND_SAMPLE_DATA_LEN_1024,       /** <sample 1024 pts */ /** <CNcomment:采集1024点 */
    HI_DRV_FRONTEND_SAMPLE_DATA_LEN_2048,       /** <sample 2048 pts */ /** <CNcomment:采集2048点 */
    HI_DRV_FRONTEND_SAMPLE_DATA_LEN_MAX        /** <Invalid value */   /** <CNcomment:非法边界值 */
} hi_drv_frontend_sample_data_len;

/** Tuner spectrum parameters */
/** CNcomment: Tuner 频谱参数 */
typedef struct {
    hi_u32  freq_start_in_hz;    /** <start frequency */   /** <CNcomment:起始频率 */
    hi_u32  freq_step_in_hz;     /** <step frequency */    /** <CNcomment:步进频率 */
    hi_u32  num_of_freq_steps;   /** <number of steps */   /** <CNcomment:频谱点数 */
    hi_s16* power_data;          /** <spectrum data */     /** <CNcomment:频谱数据 */
} hi_drv_tuner_spectrum_para;

/** Defines the cable transmission signal. */
/** CNcomment:定义CABLE传输信号 */
typedef struct {
    hi_u32                  freq;            /** <Frequency, in kHz */         /** <CNcomment:频率，单位：kHz */
    hi_u32                  symbol_rate;     /** <Symbol rate, in bit/s */     /** <CNcomment:符号率，单位bps */
    hi_drv_modulation_type  mod_type;        /** <QAM mode */                  /** <CNcomment:QAM调制方式 */
    hi_bool                 reverse;         /** <Spectrum reverse mode */     /** <CNcomment:频谱倒置处理方式 */
    hi_u32                  band_width;      /** <bandwidth in KHz */          /** <CNcomment:带宽，单位kHz */
} hi_drv_cab_connect_para;

/** Guard interval of OFDM */
/** CNcomment:多载波调制下的保护间隔 */
typedef enum {
    HI_DRV_FRONTEND_GUARD_INTV_DEFALUT = 0,     /** <default guard interval mode */   /** <CNcomment:保护间隔默认模式 */
    HI_DRV_FRONTEND_GUARD_INTV_1_128,           /** <1/128 */                         /** <CNcomment:保护间隔1/128模式 */
    HI_DRV_FRONTEND_GUARD_INTV_1_32,            /** <1/32 */                          /** <CNcomment:保护间隔1/32模式 */
    HI_DRV_FRONTEND_GUARD_INTV_1_16,            /** <1/16 */                          /** <CNcomment:保护间隔1/16模式 */
    HI_DRV_FRONTEND_GUARD_INTV_1_8,             /** <1/8 */                           /** <CNcomment:保护间隔1/8模式 */
    HI_DRV_FRONTEND_GUARD_INTV_1_4,             /** <1/4 */                           /** <CNcomment:保护间隔1/4模式 */
    HI_DRV_FRONTEND_GUARD_INTV_19_128,          /** <19/128 */                        /** <CNcomment:保护间隔19/128模式 */
    HI_DRV_FRONTEND_GUARD_INTV_19_256,          /** <19/256 */                        /** <CNcomment:保护间隔19/256模式 */
    HI_DRV_FRONTEND_GUARD_INTV_MAX             /** <Invalid value */                 /** <CNcomment:非法边界值 */
} hi_drv_frontend_guard_intv;

/** OFDM Mode, used in multi-carrier modulation */
/** CNcomment:OFDM模式，用于多载波调制模式下 */
typedef enum {
    HI_DRV_FRONTEND_FFT_DEFAULT = 0,   /** <default mode */     /** <CNcomment:默认模式 */
    HI_DRV_FRONTEND_FFT_1K,            /** <1k mode */          /** <CNcomment:1k模式 */
    HI_DRV_FRONTEND_FFT_2K,            /** <2k mode */          /** <CNcomment:2k模式 */
    HI_DRV_FRONTEND_FFT_4K,            /** <4k mode */          /** <CNcomment:4k模式 */
    HI_DRV_FRONTEND_FFT_8K,            /** <8k mode */          /** <CNcomment:8k模式 */
    HI_DRV_FRONTEND_FFT_16K,           /** <16k mode */         /** <CNcomment:16k模式 */
    HI_DRV_FRONTEND_FFT_32K,           /** <32k mode */         /** <CNcomment:32k模式 */
    HI_DRV_FRONTEND_FFT_64K,           /** <64k mode */         /** <CNcomment:64k模式 */
    HI_DRV_FRONTEND_FFT_MAX           /** <Invalid value */    /** <CNcomment:非法边界值 */
} hi_drv_frontend_fft;

/** TS Priority, only used in DVB-T */
/** CNcomment:仅用于DVB-T */
typedef enum {
    HI_DRV_FRONTEND_DVBT_TS_PRIORITY_NONE = 0,      /** <no priority mode */          /** <CNcomment:无优先级模式 */
    HI_DRV_FRONTEND_DVBT_TS_PRIORITY_HP,            /** <high priority mode */        /** <CNcomment:高优先级模式 */
    HI_DRV_FRONTEND_DVBT_TS_PRIORITY_LP,            /** <low priority mode */         /** <CNcomment:低优先级模式 */
    HI_DRV_FRONTEND_DVBT_TS_PRIORITY_MAX           /** <Invalid value */             /** <CNcomment:非法边界值 */
} hi_drv_frontend_dvbt_ts_priority;

/** Hierarchical modulation mode, only used in DVB-T */
/** CNcomment:仅用于DVB-T */
typedef enum {
    HI_DRV_FRONTEND_DVBT_HIERARCHY_DEFAULT = 0, /** <hierarchical modulation default mode */ /** <CNcomment:默认模式 */
    HI_DRV_FRONTEND_DVBT_HIERARCHY_NO,      /** <no hierarchical modulation mode */ /** <CNcomment:不分级别模式 */
    HI_DRV_FRONTEND_DVBT_HIERARCHY_ALHPA1,  /** <hierarchical mode, alpha = 1 */    /** <CNcomment:分级别模式, alpha = 1 */
    HI_DRV_FRONTEND_DVBT_HIERARCHY_ALHPA2,  /** <hierarchical mode, alpha = 2 */    /** <CNcomment:分级别模式, alpha = 2 */
    HI_DRV_FRONTEND_DVBT_HIERARCHY_ALHPA4,  /** <hierarchical mode, alpha = 4 */    /** <CNcomment:分级别模式, alpha = 4 */
    HI_DRV_FRONTEND_DVBT_HIERARCHY_MAX     /** <Invalid value */                   /** <CNcomment:非法边界值 */
} hi_drv_frontend_dvbt_hierarchy;

/** base channel or lite channel, only used in DVB-T2 */
/** CNcomment:仅用于DVB-T2 */
typedef enum {
    HI_DRV_FRONTEND_DVBT2_MODE_BASE = 0,     /** < the channel is base mode */  /** <CNcomment:通道中仅支持base信号 */
    HI_DRV_FRONTEND_DVBT2_MODE_LITE,         /** < the channel is lite mode */  /** <CNcomment:通道中需要支持lite信号 */
    HI_DRV_FRONTEND_DVBT2_MODE_MAX          /** <Invalid value */              /** <CNcomment:非法边界值 */
} hi_drv_frontend_dvbt2_mode;

/** pilot pattern */
/** CNcomment:导频模式 */
typedef enum {
    HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP1 = 0,  /** < pilot pattern pp1 */
    HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP2,      /** < pilot pattern pp2 */
    HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP3,      /** < pilot pattern pp3 */
    HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP4,      /** < pilot pattern pp4 */
    HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP5,      /** < pilot pattern pp5 */
    HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP6,      /** < pilot pattern pp6 */
    HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP7,      /** < pilot pattern pp7 */
    HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_PP8,      /** < pilot pattern pp8 */
    HI_DRV_FRONTEND_DVBT2_PILOT_PATTERN_MAX
} hi_drv_frontend_dvbt2_pilot_pattern;

/** information for carrier mode */
/** CNcomment:载波模式信息 */
typedef enum {
    HI_DRV_FRONTEND_DVBT2_CARRIER_EXTEND = 0,    /** < extend carrier */   /** <CNcomment:扩展载波 */
    HI_DRV_FRONTEND_DVBT2_CARRIER_NORMAL,        /** < normal carrier */   /** <CNcomment:正常载波 */
    HI_DRV_FRONTEND_DVBT2_CARRIER_MAX      /** <Invalid value */     /** <CNcomment:非法边界值 */
} hi_drv_frontend_dvbt2_carrier;

/** information for constellation mode */
/** CNcomment:星座是否旋转 */
typedef enum {
    HI_DRV_FRONTEND_CONSTELLATION_STANDARD = 0,    /** < standard constellation */   /** <CNcomment:不旋转星座 */
    HI_DRV_FRONTEND_CONSTELLATION_ROTATION,        /** < rotation constellation */   /** <CNcomment:旋转星座 */
    HI_DRV_FRONTEND_CONSTELLATION_MAX        /** <Invalid value */             /** <CNcomment:非法边界值 */
} hi_drv_frontend_constellation;

/** FEC frame length */
/** CNcomment:FEC帧长 */
typedef enum {
    HI_DRV_FRONTEND_DVBT2_FEC_FRAME_NORMAL = 0,      /** < normal fec frame */ /** <CNcomment:普通长度的fec帧 */
    HI_DRV_FRONTEND_DVBT2_FEC_FRAME_SHORT,           /** < short fec frame */  /** <CNcomment:fec短帧 */
    HI_DRV_FRONTEND_DVBT2_FEC_FRAME_MAX        /** <Invalid value */     /** <CNcomment:非法边界值 */
} hi_drv_frontend_dvbt2_fec_frame;

/** DVB-T2 data PLP's ISSY type */
/** CNcomment:仅用于DVB-T2 */
typedef enum {
    HI_DRV_FRONTEND_DVBT2_ISSY_NO = 0,   /** < No Input Stream SYnchronizer */        /** <CNcomment:无ISSY */
    HI_DRV_FRONTEND_DVBT2_ISSY_SHORT,    /** < Short Input Stream SYnchronizer */     /** <CNcomment:短ISSY */
    HI_DRV_FRONTEND_DVBT2_ISSY_LONG,     /** < Long Input Stream SYnchronizer */      /** <CNcomment:长ISSY */
    HI_DRV_FRONTEND_DVBT2_ISSY_MAX      /** < Invalid value */                       /** <CNcomment:非法边界值 */
} hi_drv_frontend_dvbt2_issy;

/** Stream type */
/** CNcomment:流类型 */
typedef enum {
    HI_DRV_FRONTEND_DVBT2_STREAM_TYPE_GFPS = 0, /** < Generic Fixed Packet size Stream */ /** <CNcomment:通用固定包长流类型 */
    HI_DRV_FRONTEND_DVBT2_STREAM_TYPE_GCS,      /** < Generic Continuous Stream */        /** <CNcomment:通用连续流类型 */
    HI_DRV_FRONTEND_DVBT2_STREAM_TYPE_GSE,      /** < Generic Stream Encapsulated */      /** <CNcomment:GSE流类型 */
    HI_DRV_FRONTEND_DVBT2_STREAM_TYPE_TS,       /** < Trasport Stream */                  /** <CNcomment:TS流类型 */
    HI_DRV_FRONTEND_DVBT2_STREAM_TYPE_GSE_HEM,  /** < GSE in High Efficiency Mode */      /** <CNcomment:GSE流HEM模式 */
    HI_DRV_FRONTEND_DVBT2_STREAM_TYPE_TS_HEM,   /** < TS in High Efficiency Mode */       /** <CNcomment:TS流HEM模式 */
    HI_DRV_FRONTEND_DVBT2_STREAM_TYPE_MAX      /** < Invalid value */                    /** <CNcomment:非法边界值 */
} hi_drv_frontend_dvbt2_stream_type;

/** PLP type of DVB-T2. */
/** CNcomment:T2下物理层管道类型 */
typedef enum {
    HI_DRV_FRONTEND_DVBT2_PLP_TYPE_COM = 0,       /** <common type */              /** <CNcomment:普通 */
    HI_DRV_FRONTEND_DVBT2_PLP_TYPE_DAT1,          /** <data1 type */               /** <CNcomment:数据1 */
    HI_DRV_FRONTEND_DVBT2_PLP_TYPE_DAT2,          /** <data2 type */               /** <CNcomment:数据2 */
    HI_DRV_FRONTEND_DVBT2_PLP_TYPE_MAX           /** <Invalid value */            /** <CNcomment:非法边界值 */
} hi_drv_frontend_dvbt2_plp_type;

/** Antenna power control */
/** CNcomment:天线供电控制 */
typedef enum {
    HI_DRV_FRONTEND_TER_ANTENNA_POWER_OFF,     /** <Antenna power off */          /** <CNcomment:天线电源关断 */
    HI_DRV_FRONTEND_TER_ANTENNA_POWER_ON,      /** <Antenna power on */           /** <CNcomment:天线电源打开 */
    HI_DRV_FRONTEND_TER_ANTENNA_POWER_MAX     /** <Invalid value */              /** <CNcomment:非法边界值 */
} hi_drv_frontend_ter_antenna_power;

/** configure lock tp PLP parameter,only in DVB-T2 */
/** CNcomment:在DVB-T2时，配置要锁频点的物理层管道属性 */
typedef struct {
    hi_u8    plp_id;           /** <PLP id */                /** <CNcomment:物理层管道ID */
    hi_u8    comm_plp_id;      /** <common PLP id */         /** <CNcomment:共享物理层管道ID */
    hi_u8    combination;      /** <PLP combination */       /** <CNcomment:数据物理层管道和共享物理层管道是否组合标志 */
} hi_drv_frontend_dvbt2_plp_para;

/** PLP information */
/** CNcomment:物理层管道信息 */
typedef struct {
    hi_u8                          plp_id;       /** <PLP id */                /** <CNcomment:物理层管道ID */
    hi_u8                          plp_grp_id;   /** <PLP group id */          /** <CNcomment:物理层管道组ID */
    hi_drv_frontend_dvbt2_plp_type plp_type;     /** <PLP type */              /** <CNcomment:物理层管道类型 */
} hi_drv_frontend_dvbt2_plp_info;

/** Terestrial connect param */
/** CNcomment:地面信号锁台参数 */
typedef struct {
    hi_u32                       freq;            /** <freq in KHz */               /** <CNcomment:频率，单位kHz */
    hi_u32                       band_width;      /** <bandwidth in KHz */          /** <CNcomment:带宽，单位kHz */
    hi_drv_modulation_type       mod_type;        /** <modulation type */           /** <CNcomment:调制方式 */
    hi_bool                      reverse;         /** <Spectrum reverse mode */     /** <CNcomment:频谱翻转处理方式 */
    hi_drv_frontend_dvbt2_mode   channel_mode;    /** <dvb-t2 channel mode */       /** <CNcomment:dvb-t2信道接收模式 */
    hi_drv_frontend_dvbt_ts_priority dvbt_prio;   /** <dvb-t TS priority */         /** <CNcomment:dvb-t码流优先级 */
    hi_drv_frontend_dvbt2_plp_para   plp_param;   /** < PLP parameter */            /** <CNcomment:物理层管道参数 */
} hi_drv_ter_connect_para;

/** Structure of the DVB-T transmission signal's detailed information. */
/** CNcomment:DVB-T信号详细信息 */
typedef struct {
    hi_u32                             freq;              /** <Frequency, in kHz */       /** <CNcomment:频率，单位：kHz */
    hi_u32                             band_width;        /** <Band width, in KHz */      /** <CNcomment:带宽，单位KHz */
    hi_drv_modulation_type             mod_type;          /** <Modulation type */         /** <CNcomment:调制方式 */
    hi_drv_frontend_fec_rate           fec_rate;          /** <FEC rate */                /** <CNcomment:前向纠错码率 */
    hi_drv_frontend_fec_rate           low_pri_fec_rate;  /** <Low priority FEC rate */   /** <CNcomment:低优先级前向纠错码率 */
    hi_drv_frontend_guard_intv         guard_intv;        /** <GI mode */                 /** <CNcomment:保护间隔模式 */
    hi_drv_frontend_fft                fft_mode;          /** <FFT mode */                /** <CNcomment:FFT模式 */
    hi_drv_frontend_dvbt_hierarchy     hier_mod;          /** <Hierarchical Modulation and alpha, only used in DVB-T */
                                                          /** <CNcomment:hierachical模式和alpha值 */
    hi_drv_frontend_dvbt_ts_priority   ts_priority;       /** <The TS priority, only used in DVB-T */
                                                          /** <CNcomment:ts priority, 仅用于DVB-T模式下 */
    hi_u16                             cell_id;           /** < Cell ID */                /** <CNcomment:单元ID */
} hi_drv_frontend_dvbt_signal_info;

/** Structure of the DVB-T2 transmission signal's detailed information. */
/** CNcomment:DVB-T2信号详细信息 */
typedef struct {
    hi_u32                             freq;                /** <Frequency, in kHz */    /** <CNcomment:频率，单位：kHz */
    hi_u32                             band_width;          /** <Band width, in KHz */   /** <CNcomment:带宽，单位KHz */
    hi_drv_modulation_type             mod_type;            /** <Modulation type */      /** <CNcomment:调制方式 */
    hi_drv_frontend_fec_rate           fec_rate;            /** <FEC rate */             /** <CNcomment:前向纠错码率 */
    hi_drv_frontend_guard_intv         guard_intv;          /** <GI mode */              /** <CNcomment:保护间隔模式 */
    hi_drv_frontend_fft                fft_mode;            /** <FFT mode */             /** <CNcomment:FFT模式 */
    hi_drv_frontend_dvbt2_plp_type     plp_type;           /** <PLP type */             /** <CNcomment:物理层管道类型 */
    hi_drv_frontend_dvbt2_pilot_pattern pilot_pattern;      /** <pilot pattern */        /** <CNcomment:导频模式 */
    hi_drv_frontend_dvbt2_carrier      carrier_mode;       /** <carrier mode */         /** <CNcomment:载波模式 */
    hi_drv_frontend_constellation      constellation;  /** <constellation mode */   /** <CNcomment:星座是否旋转 */
    hi_drv_frontend_dvbt2_fec_frame    fec_frame;      /** <FEC frame length */     /** <CNcomment:FEC帧长 */
    hi_drv_frontend_dvbt2_mode         channel_mode;        /** <Base channel or Lite channel */
                                                            /** <CNcomment:Base或Lite模式 */
    hi_drv_frontend_dvbt2_issy         dvbt2_issy;          /** <DVB-T2 ISSY type */     /** <CNcomment:ISSY类型 */
    hi_drv_frontend_dvbt2_stream_type  stream_type;         /** <Stream type */          /** <CNcomment:流类型 */
    hi_u16                             cell_id;             /** < Cell ID */             /** <CNcomment:单元ID */
    hi_u16                             network_id;          /** < NetWork ID */          /** <CNcomment:网络ID */
    hi_u16                             system_id;           /** < System ID */           /** <CNcomment:系统ID */
} hi_drv_frontend_dvbt2_signal_info;

/** ISDBT layer information,each layer can be received indepent */
/** CNcomment:ISDBT层信息，每层都可以独立接收 */
typedef enum {
    HI_DRV_FRONTEND_ISDBT_LAYER_ALL,
    HI_DRV_FRONTEND_ISDBT_LAYER_A,
    HI_DRV_FRONTEND_ISDBT_LAYER_B,
    HI_DRV_FRONTEND_ISDBT_LAYER_C,
    HI_DRV_FRONTEND_ISDBT_LAYER_MAX
} hi_drv_frontend_isdbt_layer;

/** ISDB-T time interleaver type, only used in ISDB-T. */
/** CNcomment:ISDB-T 时域交织类型，仅用于ISDB-T */
typedef enum {
    HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_0,      /** <time interleaver type 1 */   /** <CNcomment:时域交织类型0 */
    HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_1,      /** <time interleaver type 1 */   /** <CNcomment:时域交织类型1 */
    HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_2,      /** <time interleaver type 2 */   /** <CNcomment:时域交织类型2 */
    HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_4,      /** <time interleaver type 4 */   /** <CNcomment:时域交织类型4 */
    HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_8,      /** <time interleaver type 8 */   /** <CNcomment:时域交织类型8 */
    HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_16,     /** <time interleaver type 16 */  /** <CNcomment:时域交织类型16 */
    HI_DRV_FRONTEND_ISDBT_TIME_INTERLEAVER_MAX,   /** <Invalid value */             /** <CNcomment:非法边界值 */
} hi_drv_frontend_isdbt_time_interleaver;

/** Parameter for ISDB-T layer exist or not */
/** CNcomment:ISDB-T模式下，层是否存在信息 */
typedef union {
    struct {
        hi_u8 layer_a_exist:1;  /* [0] */
        hi_u8 layer_b_exist:1;  /* [1] */
        hi_u8 layer_c_exist:1;  /* [2] */
        hi_u8 reserved:5;       /* [7:3] */
    } bits;

    hi_u8 existance_flag;
} hi_drv_frontend_isdbt_layers_identify;

/** Parameter for ISDB-T layer configuration */
/** CNcomment:ISDB-T模式下，层配置信息 */
typedef struct {
    hi_drv_frontend_isdbt_layer isdbt_layer;  /** <ISDB-T layer */ /** <CNcomment:ISDB-T层信息 */
} hi_drv_frontend_isdbt_receive_config;

/** Parameter for ISDB-T layer information */
/** CNcomment:ISDB-T模式下，层 信号属性信息 */
typedef struct {
    hi_u8 layer_seg_num;            /** <layer segment number */            /** <CNcomment:层segment个数 */
    hi_u8 layer_reserved[3];        /** <reserved 3 bytes */
    hi_drv_frontend_isdbt_time_interleaver layer_time_interleaver; /** <layer hierarchy mode */
                                                                   /** <CNcomment:层时间交织模式 */
    hi_drv_frontend_fec_rate  layer_fec_rate; /** <layer FEC rate */        /** <CNcomment:层码率 */
    hi_drv_modulation_type    layer_mod_type; /** <layer modulation type */ /** <CNcomment:层调制模式 */
} hi_drv_frontend_isdbt_layers_info;

/** Parameter for ISDB-T TMCC information */
/** CNcomment:ISDB-T模式下，TMCC 信息 */
typedef struct {
    hi_u8 emergency_flag;   /** <the start flag for emergency alarm broadcasting */   /** <CNcomment:紧急报警播报开始标志 */
    hi_u8 partial_flag;     /** <partial reception flag */                            /** <CNcomment:部分接收标志 */
    hi_u8 phase_shift_corr; /** <phase shift correction value for connected segment transmission */
                            /** <CNcomment:相位偏移值 */
    hi_u8 isdbt_system_id;  /** <system identification[0 = ISDB_T,1 = ISDB_T_SB] */   /** <CNcomment:系统标识 */

    hi_drv_frontend_isdbt_layers_info isdbt_layers_a_info_bits;
    hi_drv_frontend_isdbt_layers_info isdbt_layers_b_info_bits;
    hi_drv_frontend_isdbt_layers_info isdbt_layers_c_info_bits;

    hi_u8 tmcc_reserved[4];    /** <Reserved 4 bytes */                               /** <CNcomment:保留参数4字节 */
} hi_drv_frontend_isdbt_tmcc_info;

/** Structure of the ISDB-T terrestrial transmission signal's detailed information. */
/** CNcomment:地面ISDB-T信号详细信息 */
typedef struct {
    hi_u32                            freq;         /** <Frequency, in kHz */    /** <CNcomment:频率，单位: kHz */
    hi_u32                            band_width;   /** <Band width, in KHz */   /** <CNcomment:带宽，单位KHz */
    hi_drv_frontend_isdbt_layers_identify  isdbt_layers; /** <the transmission and used information of ISDBT layer */
                                                          /** <CNcomment:ISDBT层正在传输使用信息 */
    hi_drv_frontend_isdbt_tmcc_info   isdbt_tmcc_info; /** <the para of ISDBT layer */ /** <CNcomment:ISDBT层参数信息 */
    hi_drv_frontend_guard_intv        guard_intv;      /** <GI mode */            /** <CNcomment:保护间隔模式 */
    hi_drv_frontend_fft               fft_mode;        /** <FFT mode */            /** <CNcomment:FFT模式 */
} hi_drv_frontend_isdbt_signal_info;

/** DTMB carrier type. */
/** CNcomment:DTMB 载波类型 */
typedef enum {
    HI_DRV_FRONTEND_DTMB_CARRIER_UNKNOWN = 0,  /** <unknown type */            /** <CNcomment:未知类型 */
    HI_DRV_FRONTEND_DTMB_CARRIER_SINGLE,       /** <single type */             /** <CNcomment:单载波 */
    HI_DRV_FRONTEND_DTMB_CARRIER_MULTI         /** <multi type */              /** <CNcomment:多载波 */
} hi_drv_frontend_dtmb_carrier;

/** DTMB QAM type. */
/** CNcomment:DTMB 调制类型 */
typedef enum {
    HI_DRV_FRONTEND_DTMB_QAM_UNKNOWN = 0, /** <unknown type */      /** <CNcomment:未知类型 */
    HI_DRV_FRONTEND_DTMB_QAM_4QAM_NR,     /** <4QAM_NR type */      /** <CNcomment:4QAM_NR 调制 */
    HI_DRV_FRONTEND_DTMB_QAM_4QAM,        /** <single type */       /** <CNcomment:4QAM 调制 */
    HI_DRV_FRONTEND_DTMB_QAM_16QAM,       /** <single type */       /** <CNcomment:16QAM 调制 */
    HI_DRV_FRONTEND_DTMB_QAM_32QAM,       /** <single type */       /** <CNcomment:32QAM 调制 */
    HI_DRV_FRONTEND_DTMB_QAM_64QAM        /** <single type */       /** <CNcomment:64QAM 调制 */
} hi_drv_frontend_dtmb_qam;

/** DTMB code rate type. */
/** CNcomment:DTMB 码率类型 */
typedef enum {
    HI_DRV_FRONTEND_DTMB_CODE_RATE_UNKNOWN = 0, /** <unknown type */  /** <CNcomment:未知类型 */
    HI_DRV_FRONTEND_DTMB_CODE_RATE_0_DOT_4,     /** <0.4 type */      /** <CNcomment:0.4 码率 */
    HI_DRV_FRONTEND_DTMB_CODE_RATE_0_DOT_6,     /** <0.6 type */      /** <CNcomment:0.6 码率 */
    HI_DRV_FRONTEND_DTMB_CODE_RATE_0_DOT_8      /** <0.8 type */      /** <CNcomment:0.8 码率 */
} hi_drv_frontend_dtmb_code_rate;

/** DTMB time interleaving type. */
/** CNcomment:DTMB 时域交织类型 */
typedef enum {
    HI_DRV_FRONTEND_DTMB_TIME_INTERLEAVER_UNKNOWN = 0, /** <unknown type */  /** <CNcomment:未知类型 */
    HI_DRV_FRONTEND_DTMB_TIME_INTERLEAVER_240,         /** <240 type */      /** <CNcomment:240 类型 */
    HI_DRV_FRONTEND_DTMB_TIME_INTERLEAVER_720          /** <720 type */      /** <CNcomment:720 类型 */
} hi_drv_frontend_dtmb_time_interleave;

/** DTMB guard interval type. */
/** CNcomment:DTMB 保护间隔类型 */
typedef enum {
    HI_DRV_FRONTEND_DTMB_GI_UNKNOWN = 0,   /** <unknown type */      /** <CNcomment:未知类型 */
    HI_DRV_FRONTEND_DTMB_GI_420,           /** <420 type */          /** <CNcomment:420 类型 */
    HI_DRV_FRONTEND_DTMB_GI_595,           /** <595 type */          /** <CNcomment:595 类型 */
    HI_DRV_FRONTEND_DTMB_GI_945            /** <945 type */          /** <CNcomment:945 类型 */
} hi_drv_frontend_dtmb_guard_interval;

/** DTMB signal information. */
/** CNcomment:DTMB信号信息 */
typedef struct {
    hi_drv_frontend_dtmb_carrier    carrier_mode;    /** <Carrier type */          /** <CNcomment:载波类型 */
    hi_drv_frontend_dtmb_qam       qam_index;       /** <QAM type */              /** <CNcomment:调制类型 */
    hi_drv_frontend_dtmb_code_rate       code_rate;       /** <Code rate type */        /** <CNcomment:码率类型 */
    hi_drv_frontend_dtmb_time_interleave time_interleave; /** <Time interleave type */  /** <CNcomment:时域交织类型 */
    hi_drv_frontend_dtmb_guard_interval  guard_interval;  /** <Guard interval type */   /** <CNcomment:保护时间类型 */
} hi_drv_frontend_dtmb_signal_info;

/** DiSEqC Wave Mode */
/** CNcomment:DiSEqC模式 */
typedef enum {
    HI_DRV_FRONTEND_DISEQC_WAVE_NORMAL,   /** <Waveform produced by demod */              /** <CNcomment:波形由demod产生 */
    HI_DRV_FRONTEND_DISEQC_WAVE_ENVELOPE, /** <Waveform produced by LNB control device */ /** <CNcomment:波形由控制芯片产生 */
    HI_DRV_FRONTEND_DISEQC_WAVE_MAX      /** <Invalid value */                           /** <CNcomment:非法边界值 */
} hi_drv_frontend_diseqc_wave;

/** LNB power supply and control device */
/** CNcomment:LNB供电和控制芯片 */
typedef enum {
    HI_DRV_LNB_CTRL_DEV_TYPE_NONE,           /** <No LNB control device */      /** <CNcomment:无控制芯片 */
    HI_DRV_LNB_CTRL_DEV_TYPE_MPS8125,        /** <MPS8125 */                    /** <CNcomment:MPS8125 */
    HI_DRV_LNB_CTRL_DEV_TYPE_ISL9492,        /** <ISL9492 */                    /** <CNcomment:ISL9492 */
    HI_DRV_LNB_CTRL_DEV_TYPE_A8300,          /** <A8300 */                      /** <CNcomment:A8300 */
    HI_DRV_LNB_CTRL_DEV_TYPE_A8297,          /** <A8297 */                      /** <CNcomment:A8297 */
    HI_DRV_LNB_CTRL_DEV_TYPE_TPS65233,       /** <TPS65233 */                   /** <CNcomment:TPS65233 */
    HI_DRV_LNB_CTRL_DEV_TYPE_LNBH30,         /** <LNBH30 */                     /** <CNcomment:LNBH30 */
    HI_DRV_LNB_CTRL_DEV_TYPE_MAX            /** <Invalid value */              /** <CNcomment:非法边界值 */
} hi_drv_lnb_ctrl_dev_type;

/** Polarization type */
/** CNcomment:极化方式 */
typedef enum {
    HI_DRV_FRONTEND_POLARIZATION_H,     /** <Horizontal Polarization */           /** <CNcomment:水平极化 */
    HI_DRV_FRONTEND_POLARIZATION_V,     /** <Vertical Polarization */             /** <CNcomment:垂直极化 */
    HI_DRV_FRONTEND_POLARIZATION_L,     /** <Left-hand circular Polarization */   /** <CNcomment:左旋圆极化 */
    HI_DRV_FRONTEND_POLARIZATION_R,     /** <Right-hand circular Polarization */  /** <CNcomment:右旋圆极化 */
    HI_DRV_FRONTEND_POLARIZATION_MAX,  /** <Invalid value */                     /** <CNcomment:非法边界值 */
} hi_drv_frontend_polarization;

/** code and modulation mode */
/** CNcomment:code and modulation 模式 */
typedef enum {
    HI_DRV_FRONTEND_CODE_MODULATION_VCM_ACM,       /** < VCM/ACM */      /** <CNcomment:可变码率和调制/自适应码率和调制模式 */
    HI_DRV_FRONTEND_CODE_MODULATION_CCM,           /** < CCM */          /** <CNcomment:固定码率和调制模式 */
    HI_DRV_FRONTEND_CODE_MODULATION_MULTISTREAM,   /** < MultiStream */  /** <CNcomment:不变调制模式下的多流 */
    HI_DRV_FRONTEND_CODE_MODULATION_MAX           /** <Invalid value */ /** <CNcomment:非法边界值 */
} hi_drv_frontend_code_modulation;

/** Roll off */
/** CNcomment:滚降系数 */
typedef enum {
    HI_DRV_FRONTEND_ROLL_OFF_35,        /** <Roll off 0.35 */           /** <CNcomment:滚降系数0.35 */
    HI_DRV_FRONTEND_ROLL_OFF_25,        /** <Roll off 0.25 */           /** <CNcomment:滚降系数0.25 */
    HI_DRV_FRONTEND_ROLL_OFF_20,        /** <Roll off 0.20 */           /** <CNcomment:滚降系数0.20 */
    HI_DRV_FRONTEND_ROLL_OFF_15,        /** <Roll off 0.15 */           /** <CNcomment:滚降系数0.15 */
    HI_DRV_FRONTEND_ROLL_OFF_10,        /** <Roll off 0.10 */           /** <CNcomment:滚降系数0.10 */
    HI_DRV_FRONTEND_ROLL_OFF_05,        /** <Roll off 0.05 */           /** <CNcomment:滚降系数0.05 */
    HI_DRV_FRONTEND_ROLL_OFF_MAX       /** <Invalid value */           /** <CNcomment:非法边界值 */
} hi_drv_frontend_roll_off;

/** Pilot type */
/** CNcomment:导频 */
typedef enum {
    HI_DRV_FRONTEND_PILOT_OFF,         /** <Pilot off */               /** <CNcomment:导频关闭 */
    HI_DRV_FRONTEND_PILOT_ON,          /** <Pilot on */                /** <CNcomment:导频打开 */
    HI_DRV_FRONTEND_PILOT_MAX         /** <Invalid value */           /** <CNcomment:非法边界值 */
} hi_drv_frontend_pilot;

/** FEC length */
/** CNcomment:FEC帧 长度 */
typedef enum {
    HI_DRV_FRONTEND_SAT_FEC_FRAME_NORMAL,  /** <nomal FEC Frame */       /** <CNcomment:普通长度FEC帧，适用于DVB-S/S2/S2X */
    HI_DRV_FRONTEND_SAT_FEC_FRAME_SHORT,   /** <short FEC Frame */       /** <CNcomment:短FEC帧，适用于DVB-S2X */
    HI_DRV_FRONTEND_SAT_FEC_FRAME_MEDIUM,  /** <medium FEC Fream */      /** <CNcomment:中等FEC帧，适用于DVB-S2X */
    HI_DRV_FRONTEND_SAT_FEC_FRAME_MAX     /** <Invalid value */         /** <CNcomment:非法边界值 */
} hi_drv_frontend_sat_fec_frame;

/** Stream typef */
/** CNcomment:流类型 */
typedef enum {
    HI_DRV_FRONTEND_SAT_STREAM_TYPE_GENERIC_PACKETIZED,  /** < Generic Packetized Stream */  /** <CNcomment:通用打包流类型 */
    HI_DRV_FRONTEND_SAT_STREAM_TYPE_GENERIC_CONTINUOUS,  /** < Generic Continuous Stream */  /** <CNcomment:通用连续流类型 */
    HI_DRV_FRONTEND_SAT_STREAM_TYPE_GSE_HEM,        /** < GSE in High Efficiency Mode */ /** <CNcomment:GSE流类型HEM模式 */
    HI_DRV_FRONTEND_SAT_STREAM_TYPE_TRANSPORT,      /** < Trensport Stream */            /** <CNcomment:TS流类型 */
    HI_DRV_FRONTEND_SAT_STREAM_TYPE_GSE_LITE,       /** < GSE-Lite */                    /** <CNcomment:GSE-Lite流类型 */
    HI_DRV_FRONTEND_SAT_STREAM_TYPE_GSE_LITE_HEM,   /** < GSE-Lite in High Efficiency Mode */
                                                    /** <CNcomment:GSE-Lite流HEM模式 */
    HI_DRV_FRONTEND_SAT_STREAM_TYPE_T2MI,           /** < T2MI Stream */                  /** <CNcomment:T2MI流类型 */
    HI_DRV_FRONTEND_SAT_STREAM_TYPE_MAX            /** < Invalid value */                /** <CNcomment:非法边界值 */
} hi_drv_frontend_sat_stream_type;

/** Satellite extended attribution */
/** CNcomment:卫星机附加属性 */
typedef struct {
    hi_u16                    tuner_max_lpf;   /** <Tuner max LPF, MHz */  /** <CNcomment:tuner低通滤波器通带频率，单位MHz */
    hi_drv_tuner_rf_agc_mode  rf_agc;          /** <Tuner RF AGC mode */   /** <CNcomment:agc模式 */
    hi_drv_tuner_iq_spectrum_mode    iq_spectrum;   /** <Tuner IQ spectrum mode */     /** <CNcomment:IQ模式 */
    hi_drv_frontend_diseqc_wave diseqc_wave;   /** <DiSEqC Wave Mode */           /** <CNcomment:DiSEqC模式 */
    hi_drv_lnb_ctrl_dev_type  lnb_ctrl_dev;  /** <LNB power supply and control device */ /** <CNcomment:LNB控制芯片 */
    hi_u16                    lnb_i2c_channel;
    hi_u16                    lnb_dev_address; /** <LNB control device address */ /** <CNcomment:LNB控制芯片i2c地址 */
} hi_drv_frontend_sat_attr;

/** Structure of the satellite transmission signal. */
/** CNcomment:定义Satellite传输信号 */
typedef struct {
    hi_u32                         freq;           /** <Downlink frequency, in kHz */ /** <CNcomment:下行频率，单位：kHz */
    hi_u32                         symbol_rate;    /** <Symbol rate, in Symb/s */     /** <CNcomment:符号率，单位：Symb/s */
    hi_drv_frontend_polarization   polar;          /** <Polarization type */          /** <CNcomment:极化方式 */

    /** <Physical layer initial scrambling code, range 0~262141, the default value must be 0,when signal sender
         config some TP using not 0, then receiver must modify this value the same as sender, if not, signal will
         be unlock, the modified value maybe told by signal sender. */
    /** <CNcomment:物理层扰码初始值，范围0~262141.该值的默认设置值为0,个别频点发端没有使用默认值0，
         收端接收时需修改该值和发端一致，否则会引起信号失锁，该值可由发端告知。 */
    hi_u32                         scramble_value;

    hi_drv_frontend_stream_id_type stream_id_type; /** <Stream ID type, only for ISDB-S/S3 */
                                                   /** <CNcomment:流ID 类型，只用于ISDB-S/S3 */
    hi_u16                         stream_id; /** <Stream ID, only for ISDB-S/S3 */ /** <CNcomment:流ID，只用于ISDB-S/S3 */
} hi_drv_sat_connect_para;


/** Structure of the satellite transmission signal's detailed information. */
/** CNcomment:卫星信号详细信息 */
typedef struct {
    hi_u32                             freq;         /** <Downlink frequency, in kHz */ /** <CNcomment:下行频率，单位：kHz */
    hi_u32                             symbol_rate;  /** <Symbol rate, in Symb/s */     /** <CNcomment:符号率，单位Symb/s */
    hi_drv_modulation_type             mod_type;     /** <Modulation type */            /** <CNcomment:调制方式 */
    hi_drv_frontend_polarization       polar;        /** <Polarization type */          /** <CNcomment:极化方式 */
    hi_drv_frontend_fec_rate           fec_rate;     /** <FEC rate */                   /** <CNcomment:前向纠错码率 */
    hi_drv_frontend_code_modulation    code_modulation;
    hi_drv_frontend_roll_off           roll_off;     /** <Rolloff */                    /** <CNcomment:滚降系数 */
    hi_drv_frontend_pilot              pilot;        /** <Pilot */                      /** <CNcomment:导频 */
    hi_drv_frontend_sat_fec_frame fec_frame_mode;  /** <FEC fream mode */          /** <CNcomment:FEC帧长度 */
    hi_drv_frontend_sat_stream_type    stream_type;  /** <Stream type */                /** <CNcomment:流类型 */
} hi_drv_frontend_dvbs2x_signal_info;

/** LNB type */
/** CNcomment:LNB类型 */
typedef enum {
    HI_DRV_FRONTEND_LNB_TYPE_SINGLE_FREQUENCY,   /** <Single LO frequency */       /** <CNcomment:单本振 */
    HI_DRV_FRONTEND_LNB_TYPE_DUAL_FREQUENCY,     /** <Dual LO frequency */         /** <CNcomment:双本振 */
    HI_DRV_FRONTEND_LNB_TYPE_UNICABLE,           /** <Unicable LNB */             /** <CNcomment:unicable高频头 */
    HI_DRV_FRONTEND_LNB_TYPE_TRIPLE_FREQUENCY,   /** <Triple LO frequency */       /** <CNcomment:三本振 */
    HI_DRV_FRONTEND_LNB_TYPE_UNICABLE2,          /** <Unicable2 LNB */             /** <CNcomment:unicable2高频头 */
    HI_DRV_FRONTEND_LNB_TYPE_MAX           /** <Invalid value */             /** <CNcomment:非法边界值 */
} hi_drv_frontend_lnb_type;

/** LNB band type */
/** CNcomment:卫星信号频段 */
typedef enum {
    HI_DRV_FRONTEND_LNB_BAND_C,             /** <C */                        /** <CNcomment:C波段 */
    HI_DRV_FRONTEND_LNB_BAND_KU,            /** <Ku */                       /** <CNcomment:Ku波段 */
    HI_DRV_FRONTEND_LNB_BAND_KA,            /** <Ka */                       /** <CNcomment:Ka波段 */
    HI_DRV_FRONTEND_LNB_BAND_MAX           /** <Invalid value */             /** <CNcomment:非法边界值 */
} hi_drv_frontend_lnb_band;

/** LNB power control */
/** CNcomment:高频头供电控制 */
typedef enum {
    HI_DRV_FRONTEND_LNB_POWER_OFF,          /** <LNB power off */                     /** <CNcomment:关断 */
    HI_DRV_FRONTEND_LNB_POWER_ON,           /** <LNB power auto, 13V/18V, default */  /** <CNcomment:默认的13/18V供电 */
    HI_DRV_FRONTEND_LNB_POWER_ENHANCED,     /** <LNB power auto, 14V/19V, some LNB control device can support. */
                                            /** <CNcomment:加强供电，部分LNB芯片支持 */
    HI_DRV_FRONTEND_LNB_POWER_MAX          /** <Invalid value */                     /** <CNcomment:非法边界值 */
} hi_drv_frontend_lnb_power;

/** LNB 22K tone status, for Ku band LNB */
/** CNcomment:22k信号状态，用于Ku双本振 */
typedef enum {
    HI_DRV_FRONTEND_LNB_22K_OFF,            /** <22k off */                    /** <CNcomment:22k信号关，选择低本振 */
    HI_DRV_FRONTEND_LNB_22K_ON,             /** <22k on */                     /** <CNcomment:22k信号开，选择高本振 */
    HI_DRV_FRONTEND_LNB_22K_MAX            /** <Invalid value */              /** <CNcomment:非法边界值 */
} hi_drv_frontend_lnb_22k;

/** unicable multi-switch port. */
/** CNcomment:unicable开关端口枚举 */
typedef enum {
    HI_DRV_FRONTEND_SAT_POSN_A,    /** <unicable switch port A */    /** <CNcomment:端口A */
    HI_DRV_FRONTEND_SAT_POSN_B,    /** <unicable switch port B */    /** <CNcomment:端口B */
    HI_DRV_FRONTEND_SAT_POSN_MAX   /** <Invalid value */             /** <CNcomment:非法边界值 */
} hi_drv_frontend_sat_posn;

/** LNB configurating parameters */
/** CNcomment:LNB配置参数 */
typedef struct {
    hi_drv_frontend_lnb_type  lnb_type;    /** <LNB type */                              /** <CNcomment:LNB类型 */
    hi_u32           low_lo;      /** < Low Local Oscillator Frequency, MHz */ /** <CNcomment:LNB低本振频率，单位MHz */
    hi_u32           medium_lo;   /** < Medium Local Oscillator Frequency, MHz */ /** <CNcomment:LNB中本振频率，单位MHz */
    hi_u32           high_lo;     /** < High Local Oscillator Frequency, MHz */ /** <CNcomment:LNB高本振频率，单位MHz */
    hi_drv_frontend_lnb_band  lnb_band;    /** < LNB band, C or Ku */                   /** <CNcomment:LNB波段：C或Ku */

    /** Structure of the unicable device attribute. */
    /** CNcomment:卫星Unicable设备属性 */
    hi_u8 ub_num;                          /** < SCR number, 0-31 */             /** <CNcomment:SCR序号，取值为0-31 */
    hi_u32 ub_freq;                        /** < SCR IF frequency, unit MHz */   /** <CNcomment:SCR中频频率， 单位MHz */
    hi_drv_frontend_sat_posn sat_posn;  /** < unicable multi-switch port */   /** <CNcomment:unicable开关端口号 */
    hi_bool ub_bin_protect;                /** < UB with pin protect */          /** <CNcomment:用户频段pin码保护 */
} hi_drv_frontend_lnb_config;

/** Tuner blind scan type */
/** CNcomment:TUNER盲扫方式 */
typedef enum {
    HI_DRV_FRONTEND_BLIND_SCAN_MODE_AUTO = 0,       /** <Blind scan automatically */  /** <CNcomment:自动扫描 */
    HI_DRV_FRONTEND_BLIND_SCAN_MODE_MANUAL,         /** <Blind scan manually */       /** <CNcomment:手动扫描 */
    HI_DRV_FRONTEND_BLIND_SCAN_MODE_MAX            /** <Invalid value */             /** <CNcomment:非法边界值 */
} hi_drv_frontend_blind_scan_mode;

/** Definition of blind scan event type */
/** CNcomment:TUNER盲扫事件 */
typedef enum {
    HI_DRV_FRONTEND_BLIND_SCAN_EVT_STATUS,          /** <New status */        /** <CNcomment:状态变化 */
    HI_DRV_FRONTEND_BLIND_SCAN_EVT_PROGRESS,        /** <New Porgress */     /** <CNcomment:进度变化 */
    HI_DRV_FRONTEND_BLIND_SCAN_EVT_NEWRESULT,       /** <Find new channel */  /** <CNcomment:新频点 */
    HI_DRV_FRONTEND_BLIND_SCAN_EVT_MAX             /** <Invalid value */     /** <CNcomment:非法边界值 */
} hi_drv_frontend_blind_scan_evt;

/** Definition of tuner blind scan status */
/** CNcomment:TUNER盲扫状态 */
typedef enum {
    HI_DRV_FRONTEND_BLIND_SCAN_STATUS_IDLE,         /** <Idel */              /** <CNcomment:空闲 */
    HI_DRV_FRONTEND_BLIND_SCAN_STATUS_SCANNING,     /** <Scanning */          /** <CNcomment:扫描中 */
    HI_DRV_FRONTEND_BLIND_SCAN_STATUS_FINISH,       /** <Finish */            /** <CNcomment:成功完成 */
    HI_DRV_FRONTEND_BLIND_SCAN_STATUS_QUIT,         /** <User quit */         /** <CNcomment:用户退出 */
    HI_DRV_FRONTEND_BLIND_SCAN_STATUS_FAIL,         /** <Scan fail */         /** <CNcomment:扫描失败 */
    HI_DRV_FRONTEND_BLIND_SCAN_STATUS_MAX          /** <Invalid value */     /** <CNcomment:非法边界值 */
} hi_drv_frontend_blind_scan_status;

/** Definition of Unicable user band scan status */
/** CNcomment:Unicable 用户频段盲扫状态 */
typedef enum {
    HI_DRV_UNICABLE_SCAN_STATUS_IDLE,         /** <Idel */              /** <CNcomment:空闲 */
    HI_DRV_UNICABLE_SCAN_STATUS_SCANNING,     /** <Scanning */          /** <CNcomment:扫描中 */
    HI_DRV_UNICABLE_SCAN_STATUS_FINISH,       /** <Finish */            /** <CNcomment:成功完成 */
    HI_DRV_UNICABLE_SCAN_STATUS_QUIT,         /** <User quit */         /** <CNcomment:用户退出 */
    HI_DRV_UNICABLE_SCAN_STATUS_FAIL,         /** <Scan fail */         /** <CNcomment:扫描失败 */
    HI_DRV_UNICABLE_SCAN_STATUS_MAX          /** <Invalid value */     /** <CNcomment:非法边界值 */
} hi_drv_unicable_scan_status;

/** Structure of satellite TP */
/** CNcomment:TUNER扫出TP信息 */
typedef struct {
    hi_u32                        freq;            /** <Downlink frequency, in kHz */    /** <CNcomment:下行频率，单位：kHz */
    hi_u32                        symbol_rate;     /** <Symbol rate, in bit/s */         /** <CNcomment:符号率，单位bps */
    hi_drv_frontend_polarization  ploar;           /** <Polarization type */             /** <CNcomment:极化方式 */
    hi_u8                         cbs_reliablity;  /** <TP reliability */                /** <CNcomment:TP的可靠度 */
} hi_drv_frontend_sat_tp_info;

/** Notify structure of tuner blind scan */
/** CNcomment:TUNER盲扫通知信息 */
typedef union {
    hi_drv_frontend_blind_scan_status* status;             /** <Scanning status */            /** <CNcomment:盲扫状态 */
    hi_u16*                            progress_percent;   /** <Scanning progress */          /** <CNcomment:盲扫进度 */
    hi_drv_frontend_sat_tp_info*       result;           /** <Scanning result */            /** <CNcomment:盲扫结果 */
} hi_drv_frontend_blind_scan_notify;

/** Parameter of the satellite tuner blind scan */
/** CNcomment:卫星TUNER盲扫参数 */
typedef struct {
    /** <LNB Polarization type, only take effect in manual blind scan mode */
    /** <CNcomment:LNB极化方式，自动扫描模式设置无效 */
    hi_drv_frontend_polarization ploar;

    /** <LNB 22K signal status, for Ku band LNB which has dual LO, 22K ON will select high LO and 22K off select low LO,
        only take effect in manual blind scan mode*/
    /** <CNcomment:LNB 22K状态，对于Ku波段双本振LNB，ON选择高本振，OFF选择低本振，自动扫描模式设置无效 */
    hi_drv_frontend_lnb_22k      lnb_22k;

    /** <Blind scan start IF, in kHz, only take effect in manual blind scan mode */
    /** <CNcomment:盲扫起始频率(中频)，单位：kHz，自动扫描模式设置无效 */
    hi_u32                         start_freq;

    /** <Blind scan stop IF, in kHz, only take effect in manual blind scan mode */
    /** <CNcomment:盲扫结束频率(中频)，单位：kHz，自动扫描模式设置无效 */
    hi_u32                         stop_freq;

    /** <The execution of the blind scan may change the 13/18V or 22K status.
        If you use any DiSEqC device which need send command when 13/18V or 22K status change,
        you should registe a callback here. Otherwise, you can set NULL here.*/
    /** <CNcomment:盲扫过程可能会切换极化方式和22K，如果你用了某些DiSEqC设备需要设置13/18V和22K的，
        请注册这个回调，如果没有用，请可传NULL */
    hi_void (*diseqc_set)(hi_u32 port, hi_drv_frontend_polarization ploar,
                            hi_drv_frontend_lnb_22k lnb_22k);

    /** <Callback when scan status change, scan progress change or find new channel. */
    /** <CNcomment:扫描状态或进度百分比发生变化时、发现新的频点时回调 */
    hi_void (*evt_notify)(hi_u32 port, hi_drv_frontend_blind_scan_evt ent, hi_drv_frontend_blind_scan_notify *notify);
} hi_drv_frontend_sat_blind_scan_para;

typedef enum {
    HI_DRV_UNICABLE_SCAN_EVT_STATUS,          /** <New status */        /** <CNcomment:状态变化 */
    HI_DRV_UNICABLE_SCAN_EVT_PROGRESS,        /** <New Porgress */     /** <CNcomment:进度变化 */
    HI_DRV_UNICABLE_SCAN_EVT_MAX             /** <Invalid value */     /** <CNcomment:非法边界值 */
} hi_drv_unicable_scan_user_band_evt;

typedef union {
    hi_drv_unicable_scan_status* status;           /** <Scanning status */   /** <CNcomment:盲扫状态 */
    hi_u16*                      progress_percent; /** <Scanning progress */ /** <CNcomment:盲扫进度 */
} hi_drv_unicable_scan_user_band_notify;

/** Structure of terrestrial scan */
/** CNcomment:配置TUNER扫描DVB-T/T2信号 */
typedef struct {
    hi_void (*evt_notify)(hi_u32 port, hi_drv_unicable_scan_user_band_evt evt,
        hi_drv_unicable_scan_user_band_notify *notify);
} hi_drv_unicable_scan_para;

/** Parameter of the tuner blind scan */
/** CNcomment:TUNER盲扫参数 */
typedef struct {
    hi_drv_frontend_blind_scan_mode mode;            /** <Scanning mode */            /** <CNcomment:盲扫模式 */
    union {
        hi_drv_frontend_sat_blind_scan_para sat;    /** <Scanning parameter */        /** <CNcomment:盲扫参数 */
    } scan_para;
} hi_drv_frontend_blind_scan_para;

/** 0/12V switch */
/** CNcomment:0/12V开关 */
typedef enum {
    HI_DRV_FRONTEND_SWITCH_0_12V_NONE,         /** < None, default */            /** <CNcomment:不接开关状态 */
    HI_DRV_FRONTEND_SWITCH_0_12V_0,            /** < 0V */                       /** <CNcomment:0V状态 */
    HI_DRV_FRONTEND_SWITCH_0_12V_12,           /** < 12V */                      /** <CNcomment:12V状态 */
    HI_DRV_FRONTEND_SWITCH_0_12V_MAX          /** <Invalid value */             /** <CNcomment:非法边界值 */
} hi_drv_frontend_switch_0_12v_sta;

/** 22KHz switch */
/** CNcomment:22K开关 */
typedef enum {
    HI_DRV_FRONTEND_SWITCH_22K_NONE,           /** < None, default */            /** <CNcomment:不接开关状态 */
    HI_DRV_FRONTEND_SWITCH_22K_0,              /** < 0 */                        /** <CNcomment:0kHz端口 */
    HI_DRV_FRONTEND_SWITCH_22K_22,             /** < 22KHz */                    /** <CNcomment:22kHz端口 */
    HI_DRV_FRONTEND_SWITCH_22K_MAX            /** <Invalid value */             /** <CNcomment:非法边界值 */
} hi_drv_frontend_switch_22k_sta;

/** Tone burst switch */
/** CNcomment:Tone burst开关 */
typedef enum {
    HI_DRV_FRONTEND_SWITCH_TONEBURST_NONE,     /** < Don't send tone burst, default */ /** <CNcomment:不接开关状态 */
    HI_DRV_FRONTEND_SWITCH_TONEBURST_0,        /** < Tone burst 0 */                   /** <CNcomment:0 port */
    HI_DRV_FRONTEND_SWITCH_TONEBURST_1,        /** < Tone burst 1 */                   /** <CNcomment:1 port */
    HI_DRV_FRONTEND_SWITCH_TONEBURST_MAX      /** <Invalid value */                   /** <CNcomment:非法边界值 */
} hi_drv_frontend_switch_tone_burst_sta;

/** DiSEqC Level */
/** CNcomment:DiSEqC设备版本 */
typedef enum {
    HI_DRV_FRONTEND_DISEQC_LEVEL_1_X,      /** <1.x, one way */                  /** <CNcomment:1.x，单向 */
    HI_DRV_FRONTEND_DISEQC_LEVEL_2_X,      /** <2.x, two way, support reply */   /** <CNcomment:2.x，双向，支持Reply */
    HI_DRV_FRONTEND_DISEQC_LEVEL_MAX      /** <Invalid value */                 /** <CNcomment:非法边界值 */
} hi_drv_frontend_diseqc_level;

/** Receive status of DiSEqC reply massage */
/** CNcomment:DiSEqC消息接收状态 */
typedef enum {
    HI_DRV_FRONTEND_DISEQC_RECV_STATUS_OK,        /** <Receive successfully */          /** <CNcomment:接收成功 */
    HI_DRV_FRONTEND_DISEQC_RECV_STATUS_UNSUPPORT, /** <Device don't support reply */  /** <CNcomment:设备不支持回传 */
    HI_DRV_FRONTEND_DISEQC_RECV_STATUS_TIMEOUT,   /** <Receive timeout */             /** <CNcomment:接收超时 */
    HI_DRV_FRONTEND_DISEQC_RECV_STATUS_ERROR,     /** <Receive fail */                  /** <CNcomment:接收出错 */
    HI_DRV_FRONTEND_DISEQC_RECV_STATUS_MAX       /** <Invalid value */                 /** <CNcomment:非法边界值 */
} hi_drv_frontend_diseqc_recv_status;

/** Structure of the DiSEqC send massage */
/** CNcomment:DiSEqC发送消息结构 */
typedef struct {
    hi_drv_frontend_diseqc_level    level;       /** <Device level */                     /** <CNcomment:器件版本 */
    hi_drv_frontend_switch_tone_burst_sta tone_burst;  /** <Tone Burst */                 /** <CNcomment:tone信号状态 */
    hi_u8                           msg[HI_DRV_DISEQC_MSG_MAX_LENGTH]; /** <Message data */ /** <CNcomment:消息字 */
    hi_u8                           length;       /** <Message length */                    /** <CNcomment:信息长度 */
    hi_u8                           repeat_times; /** <Message repeat times */             /** <CNcomment:重传次数 */
} hi_drv_frontend_diseqc_send_msg;

/** Structure of the DiSEqC reply massage */
/** CNcomment:DiSEqC接收消息结构 */
typedef struct {
    hi_drv_frontend_diseqc_recv_status status;     /** <Recieve status */         /** <CNcomment:接收状态 */
    hi_u8       msg[HI_DRV_DISEQC_MSG_MAX_LENGTH]; /** <Recieve message data */   /** <CNcomment:接收数据缓存 */
    hi_u8       length;                            /** <Recieve message length */ /** <CNcomment:接收数据长度 */
} hi_drv_frontend_diseqc_recv_msg;

/** DiSEqC Switch port */
/** CNcomment:DiSEqC开关端口枚举 */
typedef enum {
    HI_DRV_FRONTEND_DISEQC_SWITCH_NONE = 0,         /** <none */                /** <CNcomment:不接开关 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_1,           /** <port1 */               /** <CNcomment:端口1 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_2,           /** <port2 */               /** <CNcomment:端口2 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_3,           /** <port3 */               /** <CNcomment:端口3 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_4,           /** <port4 */               /** <CNcomment:端口4 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_5,           /** <port5 */               /** <CNcomment:端口5 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_6,           /** <port6 */               /** <CNcomment:端口6 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_7,           /** <port7 */               /** <CNcomment:端口7 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_8,           /** <port8 */               /** <CNcomment:端口8 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_9,           /** <port9 */               /** <CNcomment:端口9 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_10,          /** <port10 */              /** <CNcomment:端口10 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_11,          /** <port11 */              /** <CNcomment:端口11 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_12,          /** <port12 */              /** <CNcomment:端口12 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_13,          /** <port13 */              /** <CNcomment:端口13 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_14,          /** <port14 */              /** <CNcomment:端口14 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_15,          /** <port15 */              /** <CNcomment:端口15 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_16,          /** <port16 */              /** <CNcomment:端口16 */
    HI_DRV_FRONTEND_DISEQC_SWITCH_PORT_MAX         /** <Invalid value */       /** <CNcomment:非法边界值 */
} hi_drv_frontend_diseqc_switch_port;

/** Parameter for DiSEqC 1.0/2.0 switch
   Some DiSEqC device need set polarization(13/18V) and 22K, you can set them here. */
/** CNcomment:DiSEqC 1.0/2.0 开关参数
   有些DiSEqC设备需要设置极化方式和22K的，如果用了这种设备，需要在这里设置 */
typedef struct {
    hi_drv_frontend_diseqc_level       level;   /** <DiSEqC device level */           /** <CNcomment:器件版本 */
    hi_drv_frontend_diseqc_switch_port port;    /** <DiSEqC switch port */            /** <CNcomment:选通端口号 */
    hi_drv_frontend_polarization       ploar;   /** <Polarization type */            /** <CNcomment:极化方式 */
    hi_drv_frontend_lnb_22k            lnb_22k; /** <22K status */                    /** <CNcomment:22k状态 */
} hi_drv_frontend_diseqc_switch_4_port_para;

/** Parameter for DiSEqC 1.1/2.1 switch */
/** CNcomment:DiSEqC 1.1/2.1 开关参数 */
typedef struct {
    hi_drv_frontend_diseqc_level       level;  /** <DiSEqC device level */            /** <CNcomment:器件版本 */
    hi_drv_frontend_diseqc_switch_port port;   /** <DiSEqC switch port */             /** <CNcomment:选通端口号 */
} hi_drv_frontend_diseqc_switch_16_port_para;

/** DiSEqC motor limit setting */
/** CNcomment:DiSEqC马达极限设置 */
typedef enum {
    HI_DRV_FRONTEND_DISEQC_LIMIT_OFF,              /** <Disable Limits */                /** <CNcomment:无限制 */
    HI_DRV_FRONTEND_DISEQC_LIMIT_EAST,             /** <Set East Limit */                /** <CNcomment:东向限制 */
    HI_DRV_FRONTEND_DISEQC_LIMIT_WEST,             /** <Set West Limit */                /** <CNcomment:西向限制 */
    HI_DRV_FRONTEND_DISEQC_LIMIT_MAX              /** <Invalid value */                 /** <CNcomment:非法边界值 */
} hi_drv_frontend_diseqc_limit;

/** Difinition of DiSEqC motor move direction */
/** CNcomment:DiSEqC马达移动方向 */
typedef enum {
    HI_DRV_FRONTEND_DISEQC_MOVE_DIR_EAST,          /** <Move east */                     /** <CNcomment:向东移动 */
    HI_DRV_FRONTEND_DISEQC_MOVE_DIR_WEST,          /** <Move west */                     /** <CNcomment:向西移动 */
    HI_DRV_FRONTEND_DISEQC_MOVE_DIR_MAX           /** <Invalid value */                 /** <CNcomment:非法边界值 */
} hi_drv_frontend_diseqc_move_dir;

/** Parameter for DiSEqC motor store position */
/** CNcomment:天线存储位置参数 */
typedef struct {
    hi_drv_frontend_diseqc_level level;        /** <DiSEqC device level */               /** <CNcomment:器件版本 */
    hi_u32                        pos;         /** <Index of position, 0-255 */          /** <CNcomment:位置序号 */
} hi_drv_frontend_diseqc_position;

/** Parameter for DiSEqC motor limit setting */
/** CNcomment:天线Limit设置参数 */
typedef struct {
    hi_drv_frontend_diseqc_level level;        /** <DiSEqC device level */               /** <CNcomment:器件版本 */
    hi_drv_frontend_diseqc_limit limit;        /** <Limit setting */                     /** <CNcomment:限制设定 */
} hi_drv_frontend_diseqc_limit_para;

/** Parameter for DiSEqC motor running */
/** CNcomment:DiSEqC马达转动参数 */
typedef struct {
    hi_drv_frontend_diseqc_level    level;    /** <DiSEqC device level */           /** <CNcomment:器件版本 */
    hi_drv_frontend_diseqc_move_dir dir;      /** <Moving direction */              /** <CNcomment:转动方向 */
    hi_u32                    running_steps;  /** <0 mean running continus;1~128 mean running steps every time. */
                                              /** <CNcomment:0表示持续转动；1~128之间表示每次转动step数， */
} hi_drv_frontend_diseqc_run;

/** Parameter for DiSEqC motor recalculate */
/** CNcomment:DiSEqC天线重计算参数 */
typedef struct {
    hi_drv_frontend_diseqc_level level;        /** <DiSEqC device level */           /** <CNcomment:器件版本 */
    hi_u8                        para1;        /** <Parameter 1 */                   /** <CNcomment:参数1 */
    hi_u8                        para2;        /** <Parameter 2 */                   /** <CNcomment:参数2 */
    hi_u8                        para3;        /** <Parameter 3 */                   /** <CNcomment:参数3 */
    hi_u8                        reserve;      /** <Reserve */                       /** <CNcomment:保留参数 */
} hi_drv_frontend_diseqc_recalculation;

/** Parameter for USALS */
/** CNcomment:USALS 参数 */
typedef struct {
    hi_u16 local_longitude;   /** <local longitude, is 10*longitude, in param, E:0-1800, W:1800-3600(3600-longtitude) */
                              /** <CNcomment:本地经度，单位0.1度，东经取值范围0-1800，西经取值范围1800-3600，值为3600-经度值 */
    hi_u16 local_latitude;    /** <local latitude, is 10*latitude, in param N:0-900, S:900-1800(1800-latitude) */
                              /** <CNcomment:本地纬度，单位0.1度，北纬取值范围0-900，南纬取值范围900-1800，值为1800-纬度值 */
    hi_u16 sat_longitude;     /** <sat longitude, is 10*longitude, in param, E:0-1800, W:1800-3600(3600-longtitude) */
                              /** <CNcomment:卫星经度，单位0.1度，东经取值范围0-1800，西经取值范围1800-3600，值为3600-经度值 */
    hi_u16 angular;           /** <calculate result, out param */
                              /** <CNcomment:计算结果，输出参数 */
} hi_drv_frontend_diseqc_usals_para;

/** Parameter for USALS goto angular */
/** CNcomment:USALS角度参数 */
typedef struct {
    hi_drv_frontend_diseqc_level level;      /** <DiSEqC device level */   /** <CNcomment:器件版本 */
    hi_u16                      angular;     /** <Angular, calculated by hi_drv_frontend_diseqc_calc_angular() */
                                             /** <CNcomment:角度值，可通过函数hi_drv_frontend_diseqc_calc_angular计算得到 */
} hi_drv_frontend_diseqc_usals_angular;

/** Unicable user band */
/** CNcomment:Unicable 用户频段 */
typedef struct {
    hi_u32          scr_no;       /** <Unicable user band number */   /** <CNcomment:Unicable 用户频段号 */
    hi_s32          center_freq;  /** <Unicable user band center frequency, Unit MHz */
                                  /** <CNcomment:Unicable 用户频段中心频率，单位MHz */
} hi_drv_unicable_scr_ub;

/** Unicable channel change parameter */
/** CNcomment:Unicable 通道切换参数 */
typedef struct {
    hi_u8 ub_num;     /** <Unicable user band number */   /** <CNcomment:Unicable 用户频段号 */
    hi_u8 bank;       /** <the signal bank which carries the desired channel */ /** <CNcomment:信号bank承载所需的信道 */
    hi_u16 t_val;     /** <the tuning word calculated by the receive */ /** <CNcomment:接收端计算的调优值 */
} hi_drv_unicable_channel_para;

/** Unicable2 channel change parameter */
/** CNcomment:Unicable2 通道切换参数 */
typedef struct {
    hi_u8 ub_num;     /** <Unicable user band number */   /** <CNcomment:Unicable 用户频段号 */
    hi_u16 t_val;     /** <the tuning word calculated by the receive */ /** <CNcomment:接收端计算的调优值 */
    hi_u8 uncommit_switch; /** <uncommit switch */
    hi_u8 commit_switch; /** <commit switch */
} hi_drv_unicable2_channel_para;

/** Unicable2 channel change parameter with pin protection */
/** CNcomment:Unicable2 带pin码保护的通道切换参数 */
typedef struct {
    hi_u8 ub_num;          /** <Unicable user band number */   /** <CNcomment:Unicable 用户频段号 */
    hi_u16 t_val;        /** <the tuning word calculated by the receive */ /** <CNcomment:接收端计算的调优值 */
    hi_u8 uncommit_switch; /** <uncommit switch */
    hi_u8 commit_switch;   /** <commit switch */
    hi_u8 pin;             /** <pin protection */       /** <CNcomment:pin码保护 */
} hi_drv_unicable2_channel_pin_para;

/** Unicable2 switch user band */
/** CNcomment:Unicable2 切换用户频段 */
typedef struct {
    hi_u8 ub_num;          /** <Unicable user band number */   /** <CNcomment:Unicable 用户频段号 */
    hi_u8 uncommit_switch; /** <uncommit switch */
    hi_u8 commit_switch;   /** <commit switch */
} hi_drv_unicable2_switch_para;

/** tuner attribution */
/** CNcomment:ATV制式信息 */
typedef enum {
    HI_DRV_ATV_SYSTEM_PAL_BG = 0,       /** <PAL BG TV sytem */  /** <CNcomment:PAL BG电视制式 */
    HI_DRV_ATV_SYSTEM_PAL_DK,           /** <PAL DK TV sytem */  /** <CNcomment:PAL DK电视制式 */
    HI_DRV_ATV_SYSTEM_PAL_I,            /** <PAL I TV sytem */   /** <CNcomment:PAL I电视制式 */
    HI_DRV_ATV_SYSTEM_PAL_M,            /** <PAL M TV sytem */   /** <CNcomment:PAL M电视制式 */
    HI_DRV_ATV_SYSTEM_PAL_N,            /** <PAL N TV sytem */   /** <CNcomment:PAL N电视制式 */
    HI_DRV_ATV_SYSTEM_SECAM_BG,         /** <SECAM BG TV sytem */  /** <CNcomment:SECAM BG电视制式 */
    HI_DRV_ATV_SYSTEM_SECAM_DK,         /** <SECAM DK TV sytem */  /** <CNcomment:SECAM DK电视制式 */
    HI_DRV_ATV_SYSTEM_SECAM_L_PRIME,    /** <SECAM L PRIME TV sytem */  /** <CNcomment:SECAM L PRIME电视制式 */
    HI_DRV_ATV_SYSTEM_SECAM_LL,         /** <SECAM LL TV sytem */  /** <CNcomment:SECAM LL电视制式 */
    HI_DRV_ATV_SYSTEM_NTSC_M,           /** <NTSC M TV sytem */    /** <CNcomment:NTSC M电视制式 */
    HI_DRV_ATV_SYSTEM_NTSC_I,           /** <NTSC I TV sytem */    /** <CNcomment:NTSC I 电视制式 */
    HI_DRV_ATV_SYSTEM_NTSC_DK,          /** <NTSC DK TV sytem */    /** <CNcomment:NTSC DK电视制式 */
    HI_DRV_ATV_SYSTEM_MAX              /** <Invalid value */      /** <CNcomment:非法边界值 */
} hi_drv_atv_system;

typedef enum {
    HI_DRV_ATV_SIF_BW_WIDE = 0,  /** <For Auto Scan Mode */  /** <CNcomment:自动盲扫模式 */
    HI_DRV_ATV_SIF_BW_NORMAL,    /** <For Normal Play Mode */ /** <CNcomment:正常播放模式 */
    HI_DRV_ATV_SIF_BW_NARROW,    /** <Not Recommended */  /** <CNcomment:不推荐 */
    HI_DRV_ATV_SIF_BW_MAX        /** <Invalid value */ /** <CNcomment:非法边界值 */
} hi_drv_atv_sif_bw;

typedef enum {
    HI_DRV_ATV_CONNECT_WORK_MODE_NORMAL = 0, /** <TVD work mode normal */ /** <CNcomment:connect 正常工作模式 */
    HI_DRV_ATV_CONNECT_WORK_MODE_CHAN_SCAN,  /** <TVD RF channel scan mode */ /** <CNcomment:connect RF搜台模式 */
    HI_DRV_ATV_CONNECT_WORK_MODE_MAX,        /** <Invalid value */ /** <CNcomment:非法边界值 */
} hi_drv_atv_connect_work_mode;

/** Tuner step for ATV */
/** CNcomment:TUNER 步长，ATV用 */
typedef enum {
    HI_DRV_FRONTEDN_ATV_STEP_50KHZ = 0, /** <Frequency step is 50KHz */      /** <CNcomment:50KHz步长 */
    HI_DRV_FRONTEDN_ATV_STEP_62_5KHZ,   /** <Frequency step is 62.5KHz */    /** <CNcomment:62.5KHz步长 */
    HI_DRV_FRONTEDN_ATV_STEP_31_25KHZ,  /** <Frequency step is 31.25KHz */   /** <CNcomment:31.25Hz步长 */
    HI_DRV_FRONTEDN_ATV_STEP_MAX,      /** <Invalid value */                /** <CNcomment:非法边界值 */
} hi_drv_frontend_atv_step;

/** Band for ATV */
/** CNcomment:ATV波段 */
typedef enum {
    HI_DRV_FRONTEND_ATV_BAND_VHF_LOW = 0,   /** <Low VHF band */             /** <CNcomment: 低甚高频段 */
    HI_DRV_FRONTEND_ATV_BAND_VHF_HIGH,      /** <High VHF band */            /** <CNcomment: 高甚高频段 */
    HI_DRV_FRONTEND_ATV_BAND_UHF,           /** <UHF band */                 /** <CNcomment: 超高频段 */
    HI_DRV_FRONTEND_ATV_BAND_ALL,           /** <Include all three bands */  /** <CNcomment: 包含上面的三个波段 */

    HI_DRV_FRONTEND_ATV_BAND_MAX,   /** <Invalid value */                   /** <CNcomment:非法边界值 */
} hi_drv_frontend_atv_band;

/** The frequency range */
/** CNcomment: ATV波段频率范围 */
typedef struct {
    hi_u32 start_freq;    /** <Start frequency(KHz) */      /** <CNcomment:起始频率 KHz */
    hi_u32 end_freq;      /** <End frequency(KHz) */        /** <CNcomment:结束频率 KHz */
} hi_drv_frontend_atv_band_range;

typedef struct {
    hi_u32                      freq;    /* 频率，单位khz */
    hi_drv_atv_system         system;   /* ATV制式 */
    hi_drv_atv_sif_bw         sif_bw;    /* Atv sif bandwidth */
    hi_drv_atv_connect_work_mode   connect_work_mode; /* connect mode :scan or switch */
} hi_drv_atv_connect_para;

/** ATV signal information. */
/** CNcomment:ATV信号信息 */
typedef struct {
    hi_bool vif_lock;           /** < IF locked information or not */       /* 中频是否锁定 */
    hi_bool afc_win;            /** < In the AFC window or not */           /* 是否在AFC窗口里面 */
    hi_bool carr_det;           /** < Detected the audio carrier or not */  /* 调频伴音载波检测 */
    hi_s32  afc_freq;         /** < AFC frequency information */          /* AFC频率值，单位Khz */
} hi_drv_frontend_atv_signal_info;

/** Frequency locking parameters of the tuner */
/** CNcomment:TUNER锁频参数 */
typedef struct {
    hi_drv_frontend_sig_type sig_type;    /** <Signal type */                       /** <CNcomment:信号类型 */

    union {
        hi_drv_cab_connect_para cab;      /** <Cable transmission signal */         /** <CNcomment:CABLE传输信号 */
        hi_drv_ter_connect_para ter;      /** <Terrestrial transmission signal */   /** <CNcomment:地面传输信号 */
        hi_drv_sat_connect_para sat;      /** <Satellite transmission signal */     /** <CNcomment:卫星传输信号 */
        hi_drv_atv_connect_para atv;      /** <ATV transmission signal */     /** <CNcomment:模拟电视传输信号 */
    } connect_para;
} hi_drv_frontend_connect_para;

/** signal information. */
/** CNcomment:TUNER信号属性 */
typedef struct {
    hi_drv_frontend_sig_type sig_type;              /** <Signal transmission type */    /** <CNcomment:信号类型 */

    union {
        hi_drv_frontend_dvbs2x_signal_info dvbs2x;  /** <Signal info of satellite */ /** <CNcomment:卫星信号信息 */
        hi_drv_frontend_dvbt_signal_info   dvbt;    /** <Signal info of DVB-T */     /** <CNcomment:地面dvbt信号信息 */
        hi_drv_frontend_dvbt2_signal_info  dvbt2;   /** <Signal info of DVB-T2 */    /** <CNcomment:地面dvbt2信号信息 */
        hi_drv_frontend_isdbt_signal_info  isdbt;   /** <Signal info of ISDB-T  */   /** <CNcomment:地面ISDB-T信号信息 */
        hi_drv_frontend_dtmb_signal_info   dtmb;    /** <Signal info of dtmb */      /** <CNcomment:地面dtmb信号信息 */
        hi_drv_frontend_atv_signal_info    atv;
    } signal_info;
} hi_drv_frontend_signal_info;

/** @} */  /** <!-- ==== Structure Definition end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_DRV_ECS_TYPE_H__ */

