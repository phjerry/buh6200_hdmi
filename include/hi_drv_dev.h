/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description:
 * Author: guoqingbo
 * Create: 2009/12/16
 */

#ifndef __HI_DRV_DEV_H__
#define __HI_DRV_DEV_H__

#define HI_DEV_MAJOR                218

#define HI_DEV_PREFIX               "hi_"

#define HI_MOD_SYS_NAME             "sys"
#define HI_DEV_SYS_NAME             HI_DEV_PREFIX HI_MOD_SYS_NAME
#define HI_DEV_SYS_MINOR            1

#define HI_MOD_LOG_NAME             "log"
#define HI_DEV_LOG_NAME             HI_DEV_PREFIX HI_MOD_LOG_NAME
#define HI_DEV_LOG_MINOR            2

#define HI_MOD_STAT_NAME            "stat"
#define HI_DEV_STAT_NAME            HI_DEV_PREFIX HI_MOD_STAT_NAME
#define HI_DEV_STAT_MINOR           3

#define HI_MOD_CIPHER_NAME          "cipher"
#define HI_DEV_CIPHER_NAME          HI_DEV_PREFIX HI_MOD_CIPHER_NAME
#define HI_DEV_CIPHER_MINOR         4

#define HI_MOD_DISP_NAME            "disp"
#define HI_DEV_DISP_NAME            HI_DEV_PREFIX HI_MOD_DISP_NAME
#define HI_DEV_DISP_MINOR           5

#define HI_MOD_VO_NAME              "vo"
#define HI_DEV_VO_NAME              HI_DEV_PREFIX HI_MOD_VO_NAME
#define HI_DEV_VO_MINOR             6

#define HI_MOD_VPSS_NAME            "vpss"
#define HI_DEV_VPSS_NAME            HI_DEV_PREFIX HI_MOD_VPSS_NAME
#define HI_DEV_VPSS_MINOR           7

#define HI_MOD_VDEC_NAME            "vdec"
#define HI_DEV_VDEC_NAME            HI_DEV_PREFIX HI_MOD_VDEC_NAME
#define HI_DEV_VDEC_MINOR           8

#define HI_MOD_AO_NAME              "ao"
#define HI_DEV_AO_NAME              HI_DEV_PREFIX HI_MOD_AO_NAME
#define HI_DEV_AO_MINOR             9

#define HI_MOD_AI_NAME              "ai"
#define HI_DEV_AI_NAME              HI_DEV_PREFIX HI_MOD_AI_NAME
#define HI_DEV_AI_MINOR             10

#define HI_MOD_AIAO_NAME            "aiao"
#define HI_DEV_AIAO_NAME            HI_DEV_PREFIX HI_MOD_AIAO_NAME
#define HI_DEV_AIAO_MINOR           11

#define HI_MOD_AENC_NAME            "aenc"
#define HI_DEV_AENC_NAME            HI_DEV_PREFIX HI_MOD_AENC_NAME
#define HI_DEV_AENC_MINOR           12

#define HI_MOD_ADSP_NAME            "adsp"
#define HI_DEV_ADSP_NAME            HI_DEV_PREFIX HI_MOD_ADSP_NAME
#define HI_DEV_ADSP_MINOR           13

#define HI_MOD_ADEC_NAME            "adec"
#define HI_DEV_ADEC_NAME            HI_DEV_PREFIX HI_MOD_ADEC_NAME
#define HI_DEV_ADEC_MINOR           14

#define HI_MOD_DEMUX_NAME           "demux"
#define HI_DEV_DEMUX_NAME           HI_DEV_PREFIX HI_MOD_DEMUX_NAME
#define HI_DEV_DEMUX_MINOR          15

#define HI_MOD_JPGD_NAME            "jpgd"
#define HI_DEV_JPGD_NAME            HI_DEV_PREFIX HI_MOD_JPGD_NAME
#define HI_DEV_JPGD_MINOR           16

#define HI_MOD_SYNC_NAME            "sync"
#define HI_DEV_SYNC_NAME            HI_DEV_PREFIX HI_MOD_SYNC_NAME
#define HI_DEV_SYNC_MINOR           17

#define HI_MOD_PVR_NAME             "pvr"
#define HI_DEV_PVR_NAME             HI_DEV_PREFIX HI_MOD_PVR_NAME
#define HI_DEV_PVR_MINOR            18

#define HI_MOD_HDMITX_NAME          "hdmitx"
#define HI_DEV_HDMITX_NAME          HI_DEV_PREFIX HI_MOD_HDMITX_NAME
#define HI_DEV_HDMITX_MINOR         19

#define HI_MOD_VI_NAME              "vi"
#define HI_DEV_VI_NAME              HI_DEV_PREFIX HI_MOD_VI_NAME
#define HI_DEV_VI_MINOR             20

#define HI_MOD_VENC_NAME            "venc"
#define HI_DEV_VENC_NAME            HI_DEV_PREFIX HI_MOD_VENC_NAME
#define HI_DEV_VENC_MINOR           21

#define HI_MOD_IR_NAME              "ir"
#define HI_DEV_IR_NAME              HI_DEV_PREFIX HI_MOD_IR_NAME
#define HI_DEV_IR_MINOR             22

#define HI_MOD_KEYLED_NAME          "keyled"
#define HI_DEV_KEYLED_NAME          HI_DEV_PREFIX HI_MOD_KEYLED_NAME
#define HI_DEV_KEYLED_MINOR         23

#define HI_MOD_I2C_NAME             "i2c"
#define HI_DEV_I2C_NAME             HI_DEV_PREFIX HI_MOD_I2C_NAME
#define HI_DEV_I2C_MINOR            24

#define HI_MOD_FRONTEND_NAME        "frontend"
#define HI_DEV_FRONTEND_NAME        HI_DEV_PREFIX HI_MOD_FRONTEND_NAME
#define HI_DEV_FRONTEND_MINOR       25

#define HI_MOD_SCI_NAME             "sci"
#define HI_DEV_SCI_NAME             HI_DEV_PREFIX HI_MOD_SCI_NAME
#define HI_DEV_SCI_MINOR            26

#define HI_MOD_PM_NAME              "pm"
#define HI_DEV_PM_NAME              HI_DEV_PREFIX HI_MOD_PM_NAME
#define HI_DEV_PM_MINOR             27

#define HI_MOD_OTP_NAME             "otp"
#define HI_DEV_OTP_NAME             HI_DEV_PREFIX HI_MOD_OTP_NAME
#define HI_DEV_OTP_MINOR            28

#define HI_MOD_CI_NAME              "ci"
#define HI_DEV_CI_NAME              HI_DEV_PREFIX HI_MOD_CI_NAME
#define HI_DEV_CI_MINOR             29

#define HI_MOD_MCE_NAME             "mce"
#define HI_DEV_MCE_NAME             HI_DEV_PREFIX HI_MOD_MCE_NAME
#define HI_DEV_MCE_MINOR            30

#define HI_MOD_PDM_NAME             "pdm"
#define HI_DEV_PDM_NAME             HI_DEV_PREFIX HI_MOD_PDM_NAME
#define HI_DEV_PDM_MINOR            31

#define HI_DEV_TDE_MINOR            32
#define HI_DEV_HIFB_MINOR           33
#define HI_DEV_PNG_MINOR            34
#define HI_DEV_JPEG_MINOR           35
#define HI_DEV_JPGE_MINOR           36

#define HI_MOD_WDG_NAME             "wdg"
#define HI_DEV_WDG_NAME             HI_DEV_PREFIX HI_MOD_WDG_NAME
#define HI_DEV_WDG_MINOR            37

#define HI_MOD_GPIO_NAME            "gpio"
#define HI_DEV_GPIO_NAME            HI_DEV_PREFIX HI_MOD_GPIO_NAME
#define HI_DEV_GPIO_MINOR           38

#define HI_MOD_PQ_NAME              "pq"
#define HI_DEV_PQ_NAME              HI_DEV_PREFIX HI_MOD_PQ_NAME
#define HI_DEV_PQ_MINOR             39

#define HI_MOD_SPI_NAME             "spi"
#define HI_DEV_SPI_NAME             HI_DEV_PREFIX HI_MOD_SPI_NAME
#define HI_DEV_SPI_MINOR            40

#define HI_MOD_HDMIRX_NAME          "hdmirx"
#define HI_DEV_HDMIRX_NAME          HI_DEV_PREFIX HI_MOD_HDMIRX_NAME
#define HI_DEV_HDMIRX_MINOR         41

#define HI_MOD_TSIO_NAME            "tsio"
#define HI_DEV_TSIO_NAME            HI_DEV_PREFIX HI_MOD_TSIO_NAME
#define HI_DEV_TSIO_MINOR           42

#define HI_MOD_TSR2RCIPHER_NAME        "tsr2rcipher"
#define HI_DEV_TSR2RCIPHER_NAME        HI_DEV_PREFIX HI_MOD_TSR2RCIPHER_NAME
#define HI_DEV_TSR2RCIPHER_MINOR       43

#define HI_MOD_CERT_NAME            "cert"
#define HI_DEV_CERT_NAME            HI_DEV_PREFIX HI_MOD_CERT_NAME
#define HI_DEV_CERT_MINOR           44

#define HI_MOD_SSM_NAME             "ssm"
#define HI_DEV_SSM_NAME             HI_DEV_PREFIX HI_MOD_SSM_NAME
#define HI_DEV_SSM_MINOR            45

#define HI_MOD_RUNTIMECHECK_NAME    "runtimecheck"
#define HI_DEV_RUNTIMECHECK_NAME    HI_DEV_PREFIX HI_MOD_RUNTIMECHECK_NAME
#define HI_DEV_RUNTIMECHECK_MINOR   46

#define HI_MOD_MAILBOX_NAME         "mailbox"
#define HI_DEV_MAILBOX_NAME         HI_DEV_PREFIX HI_MOD_MAILBOX_NAME
#define HI_DEV_MAILBOX_MINOR        47

#define HI_MOD_CASIMAGE_NAME        "casimage"
#define HI_DEV_CASIMAGE_NAME        HI_DEV_PREFIX HI_MOD_CASIMAGE_NAME
#define HI_DEV_CASIMAGE_MINOR       48

#define HI_MOD_MODULE_NAME          "module"
#define HI_DEV_MODULE_NAME          HI_DEV_PREFIX HI_MOD_MODULE_NAME
#define HI_DEV_MODULE_MINOR         49

#define HI_MOD_KLAD_NAME            "klad"
#define HI_DEV_KLAD_NAME            HI_DEV_PREFIX HI_MOD_KLAD_NAME
#define HI_DEV_KLAD_MINOR           50


#define HI_MOD_KEYSLOT_NAME         "keyslot"
#define HI_DEV_KEYSLOT_NAME         HI_DEV_PREFIX HI_MOD_KEYSLOT_NAME
#define HI_DEV_KEYSLOT_MINOR        51

#define HI_MOD_MIPI_NAME            "mipi"
#define HI_DEV_MIPI_NAME            HI_DEV_PREFIX HI_MOD_MIPI_NAME
#define HI_DEV_MIPI_MINOR           52

#define HI_MOD_RM_NAME              "rm"
#define HI_DEV_RM_NAME              HI_DEV_PREFIX HI_MOD_RM_NAME
#define HI_DEV_RM_MINOR             53

#define HI_MOD_PWM_NAME            "pwm"
#define HI_DEV_PWM_NAME            HI_DEV_PREFIX HI_MOD_PWM_NAME
#define HI_DEV_PWM_MINOR           54

#define HI_MOD_PANEL_NAME           "panel"
#define HI_DEV_PANEL_NAME           HI_DEV_PREFIX HI_MOD_PANEL_NAME
#define HI_DEV_PANEL_MINOR          55

#define HI_MOD_AVPLAY_NAME          "avplay"
#define HI_DEV_AVPLAY_NAME          HI_DEV_PREFIX HI_MOD_AVPLAY_NAME
#define HI_DEV_AVPLAY_MINOR         56

#endif  /* __HI_DRV_DEV_H__ */

