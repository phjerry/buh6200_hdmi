/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description: Module ID definition.
 */

#ifndef __HI_MODULE_H__
#define __HI_MODULE_H__

#define HI_INVALID_MODULE_ID (0xffffffff)
#define HI_MAX_MODULE_NUM 256

typedef enum {
    /* common */ /* CNcomment: 系统通用模块 */
    HI_ID_SYS = 0x00,
    HI_ID_MODULE,
    HI_ID_LOG,
    HI_ID_PROC,
    HI_ID_MEM,
    HI_ID_STAT,
    HI_ID_PDM,
    HI_ID_MEMDEV,
    HI_ID_MDDRC,
    HI_ID_MAILBOX,
    HI_ID_RM,      /* Resource management */
    HI_ID_PM,      /* Power management */
    HI_ID_DVFS,
    HI_ID_RUNTIME, /* Run time check */
    HI_ID_SPREAD,
    HI_ID_CUSTOM,  /* Customize */

    /* Peripheral */ /* CNcomment: 外设相关模块 */
    HI_ID_FLASH = 0x10,
    HI_ID_IR,
    HI_ID_I2C,
    HI_ID_GPIO,
    HI_ID_GPIO_I2C,
    HI_ID_PWM,
    HI_ID_LSADC,
    HI_ID_SPI,
    HI_ID_KEYLED,
    HI_ID_WDG,
    HI_ID_CI,
    HI_ID_SCI,
    HI_ID_BEIDOU,
    HI_ID_BT,
    HI_ID_FRONTEND,

    /* Demux */ /* CNcomment: 解复用相关模块 */
    HI_ID_DEMUX = 0x2c,

    /* Security */ /* CNcomment: 安全相关模块 */
    HI_ID_OTP = 0x30,
    HI_ID_KLAD,
    HI_ID_KEYSLOT,
    HI_ID_CIPHER,
    HI_ID_TSR2RCIPHER,
    HI_ID_CERT,
    HI_ID_TSIO,
    HI_ID_SSM,
    HI_ID_VMX_ULTRA,
    HI_ID_CASIMAGE,
    HI_ID_HDCP,

    /* Audio */ /* CNcomment: 音频相关模块 */
    HI_ID_SIF = 0x40,
    HI_ID_AIAO,
    HI_ID_AI,
    HI_ID_AENC,
    HI_ID_ADEC,
    HI_ID_AFLT, /* HIFI */
    HI_ID_ADSP,
    HI_ID_TTS,
    HI_ID_AO,
    HI_ID_AMP,

    /* Video and input/output */ /* CNcomment: 视频及输入输出相关模块 */
    HI_ID_VFE = 0x50,
    HI_ID_TVD,
    HI_ID_HDDEC,
    HI_ID_VBI,
    HI_ID_VICAP,
    HI_ID_VI,
    HI_ID_VENC,
    HI_ID_VFMW,
    HI_ID_VDEC,
    HI_ID_PQ,
    HI_ID_MEMC,
    HI_ID_FRC,
    HI_ID_VPSS,
    HI_ID_WIN,
    HI_ID_DISP,
    HI_ID_HDMIRX,
    HI_ID_HDMITX,
    HI_ID_PANEL,
    HI_ID_MIPI,
    HI_ID_FDMNG, /* 3D detect */
    HI_ID_DISPMNG,
    HI_ID_DISPCTRL,

    HI_ID_OMXVENC = 0x6A,
    HI_ID_OMXVDEC = 0x6B,

    /* Graphics */ /* CNcomment: 图形相关模块 */
    HI_ID_FB = 0x70,
    HI_ID_GPU,
    HI_ID_TDE,
    HI_ID_GFX2D,
    HI_ID_JPGDEC,
    HI_ID_JPGENC,
    HI_ID_PNG,
    HI_ID_HIGO,

    /* Player */ /* CNcomment: 播放器相关模块 */
    HI_ID_AVPLAY = 0x80,
    HI_ID_SYNC,
    HI_ID_VSYNC,
    HI_ID_ASYNC,
    HI_ID_PVR,

    /* Component */ /* CNcomment: 组件相关模块 */
    HI_ID_SUBT = 0x90,
    HI_ID_TTX,
    HI_ID_CC,
    HI_ID_LOADER,
    HI_ID_KARAOKE,
    HI_ID_VP,

    /* Middleware */ /* CNcomment: 中间件相关模块 */
    HI_ID_NETFLIX = 0xA0,

    /* User definition */ /* CNcomment: 为上层应用保留的自定义区 */
    HI_ID_USR_START = 0xB0,
    HI_ID_USR_END = 0xFE,

    HI_ID_MAX = 0xFF
} hi_mod_id;

#endif /* __HI_MODULE_H__ */

