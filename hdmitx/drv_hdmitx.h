/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver main header file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __DRV_HDMITX_H__
#define __DRV_HDMITX_H__

#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include "hi_osal.h"

#include <hi_type.h>
#include <hi_debug.h>

/* switch mode define */
#define HDMI_SWITCH_MODE_TMDS_2_TMDS 0
#define HDMI_SWITCH_MODE_TMDS_2_FRL  1
#define HDMI_SWITCH_MODE_FRL_2_TMDS  2
#define HDMI_SWITCH_MODE_FRL_2_FRL   3

/*
 *  Hdmi has got more than one control point, so it's important to introduce
 *  state machine to manager all the access, only the state machine legal
 *  access is allowed.
 *
 *		state transation is as follow:
 *
 *  Init state:  POWER_OFF
 *
 *             LOGO				DRV_LOADED
 *  POWER_OFF-------->BOOTLOGO_ON    POWER_OFF-------------->DRV_INITED
 *
 *               DRV_LOADED
 *  BOOTLOGO_ON--------------->DRV_INITED
 *
 *               DRV_PROBED
 *  DRV_INITED---------------->DRV_PROBED
 *
 *                   ATTACH
 *  DRV_PROBED------------------------>DRV_ATTACHED
 *
 *                     OFF/ON
 *  DRV_PROBED-------------------------->DRV_PROBED (if signal on, on/off signal)
 *
 *			PREPARE
 *  DRV_ATTACHED--------------------->MODESET_PREPARED
 *
 *			Detach
 *  DRV_ATTACHED--------------------->DRV_PROBED
 *
 *		       OFF/ON
 *  DRV_ATTACHED-------------------->DRV_ATTACHED (if signal on, on/off signal)
 *
 *                       MODESET
 *  MODESET_PREPARED----------------->DRV_ATTACHED
 *
 *                 RESUME
 *  SUSPENDED----------------------->DRV_ATTACHED
 *
 */
typedef enum {
    HDMI_STATE_POWEROFF = 0, /* never used since it's state before driver loaded */
    HDMI_STATE_BOOTLOGO_ON,  /* never used since it's state before driver loaded */
    HDMI_STATE_DRV_INITED,
    HDMI_STATE_DRV_PROBED,
    HDMI_STATE_ATTACHED,
    HDMI_STATE_MODESET_PREPARED,
    HDMI_STATE_SUSPENDED
} hdmi_state;

struct device;
struct cec_adapter;
struct hdmi_connector;
struct hdmi_crg;
struct hdmi_controller;
struct hdmi_ddc;
struct hisilicon_hdmi;
struct hi_display_mode;
struct ao_attr;
struct i2c_adapter;

struct hdmi_boot_info {
    hi_s32 hdmi_id;
    hi_s32 vic; /* The CEA Video ID (VIC) of the current drm display mode. */
    hi_s32 frlrate;
    hi_s32 hdmimode;
};

/*
 * @attach: This callback function is used when bind a vo to a display interface
 * @dettach: This callback function is used when unbind a vo with a display interface
 * @mode_validate: This called is used to validate the if the mode is supported by the interface or not
 *                 before setting the display mode.
 * @prepare: This callback should prepare the display interface for a subsequent modeset
 * @mode_set: This callback is used to update the display mode of an display interface.
 * @display_on: This callback is used to turn on the interface signal to the display
 * @display_off: This callback is used to turn off the interface signal to the display
 */
struct hdmi_vo_ops {
    void (*suspend)(struct hisilicon_hdmi *dev, void *data);
    hi_s32 (*resume)(struct hisilicon_hdmi *dev, void *data);
    hi_s32 (*attach)(struct hisilicon_hdmi *dev, void *data);
    hi_s32 (*detach)(struct hisilicon_hdmi *dev, void *data);
    hi_s32 (*mode_validate)(struct hisilicon_hdmi *dev, hi_s32 mode, struct hi_display_mode *dispaly_mode);
    hi_s32 (*prepare)(struct hisilicon_hdmi *dev, hi_s32 mode, struct hi_display_mode *dispaly_mode);
    hi_s32 (*mode_set)(struct hisilicon_hdmi *dev, hi_s32 mode, struct hi_display_mode *dispaly_mode);
    hi_s32 (*atomic_mode_set)(struct hisilicon_hdmi *dev, hi_s32 mode, struct hi_display_mode *dispaly_mode);
    hi_s32 (*display_on)(struct hisilicon_hdmi *dev);
    hi_s32 (*display_off)(struct hisilicon_hdmi *dev);
};

struct hdmi_ao_ops {
    /* get sink's audio capability */
    hi_s32 (*get_eld)(struct hisilicon_hdmi *dev, void *data, hi_u8 *buf, size_t len);
    /* check audio attr valid */
    hi_s32 (*hw_params_validate)(struct hisilicon_hdmi *dev, struct ao_attr *attr);
    /* set audio attr */
    hi_s32 (*hw_params)(struct hisilicon_hdmi *dev, struct ao_attr *attr);
    /* mute hdmi audio */
    hi_s32 (*digital_mute)(struct hisilicon_hdmi *dev, void *data, hi_bool enable);
    /* detect hdmi hotplug status */
    hi_s32 (*hpd_detect)(struct hisilicon_hdmi *dev, hi_u32 *status);
    /* used to received audio interface broadcast msg */
    hi_s32 (*register_notifier)(struct hisilicon_hdmi *dev, struct notifier_block *nb);
    hi_s32 (*unregister_notifier)(struct hisilicon_hdmi *dev, struct notifier_block *nb);
};

struct hisilicon_hdmi {
    struct device *dev;
    /* This is for IOTCL */
    struct miscdevice miscdev;

    /* weather the hdmi is attached */
    hi_bool attached;

    hi_u32 id;
    hi_s8 name[16]; /* name max size is 16. */

    struct hdmi_ddc *ddc;

    /* Phy reference */
    struct hdmitx_phy *phy;

    /* cec device */
    struct drv_hdmitx_cec_device *cec;

    /* Hdmi crg module, this may change with soc */
    void __iomem *aon_crg_regs;
    void __iomem *ctrl_crg_regs;
    void __iomem *phy_crg_regs;
    void __iomem *ppll_crg_regs;
    void __iomem *ppll_state_regs;
    void __iomem *sysctrl_regs;
    struct hdmi_crg *crg;
    hi_bool is_hdmi21;
    /* Hdmi controller version */
    hi_u32 version;
    /* Hdmi irq */
    hi_s32 irq;
    /* pwd irq */
    hi_s32 pwd_irq;
    /* Hdmi controller caps */
    hi_u32 caps;
    /* Hdmi controller extend caps */
    hi_u32 caps2;
    /* Hdmi reg base */
    void __iomem *hdmi_regs;
    void __iomem *hdmi_aon_regs;
    /* Hdmi controller, include video && audio && hpd */
    struct hdmi_controller *ctrl;
    /* Hdmi connector reference, software concept of the connector */
    struct hdmi_connector *connector;

    /* Notifier head, now only for ao */
    struct raw_notifier_head notifier_list;

    /* Hdmi hpd poll work */
    struct delayed_work hpd_poll_work;

    /* Hdmi hdcp information */
    struct hdmi_hdcp *hdcp;

    /* Hdmi boot status info passed from boot */
    struct hdmi_boot_info info;

    struct hdmi_vo_ops *vo_ops;

    struct hdmi_ao_ops *ao_ops;
    struct hdmi_debug_info *debug_info;
};

/* Controller version defination */
#define HI_HDMI_CONTROLLER_VERSION_V1_0 0x10
#define HI_HDMI_CONTROLLER_VERSION_V1_1 0x11

/*
 *  hdmi source capability -caps
 *  bit0: cec_support        0: no support; 1: support.
 *  bit1: hdcp14_support     0: no support; 1: support.
 *  bit2: hdcp2x_support     0: no support; 1: support.
 *  bit3: dvi_support        0: no support; 1: support.
 *  bit4: hdmi_support       0: no support; 1: support.
 *  bit5: max_tmds_clock     0: 340M;       1: 600M.
 *  bit6: scdc_present       0: no support; 1: support.
 *  bit7: scdc_lte_340mcsc   0: no support; 1: support.
 *  bit8: bpc_30             0: no support; 1: support.
 *  bit9: bpc_36             0: no support; 1: support.
 *  bit10: bpc_48            0: no support; 1: support.
 *  bit11-14: max_frl_rate   0: no support; 1: 3G3L;  2: 6G3L; 3: 6G4L;
 *                          4: 8G4L; 5: 10G4L; 6: 12G4L; [7~15]: RESVER.
 *  bit15-17: ffe_levels     0: FFE0; 1: FFE1; 2: FFE2; 3: FFE3; [4~7]: RESVER.
 *  bit18: dsc_support       0: no support; 1: support;
 *  bit19: native_y420       0: no support; 1: support;
 *  bit20: dsc_10bpc         0: no support; 1: support;
 *  bit21: dsc_12bpc         0: no support; 1: support;
 *  bit22-25: max_slice_count    0: no support;  1: 1 slices;  2: 2 slices;
 *                              3: 4 slices;  4: 8 slices;  5: 12 slices;
 *                              6: 16 slices;  [7~15]: RESVER.
 *  bit26-27: max_pixel_clk_per_slice  0: 340;   1: 400;  [2~3]: RESVER.
 *  bit28: rgb2yuv             0: no support;  1: support.
 *  bit29: ycbcr444            0: no support;  1: support.
 *  bit30: ycbcr422            0: no support;  1: support.
 *  bit31: ycbcr420            0: no support;  1: support.
 */

/* bit0: cec */
#define CEC_SUPPORT_MASK        (1 << 0)
#define CEC_SUPPORT_SHIFT       0
/* bit1~2:hdcp */
#define HDCP14_SUPPORT_MASK     (1 << 1)
#define HDCP14_SUPPORT_SHIFT    1
#define HDCP2x_SUPPORT_MASK     (1 << 2)
#define HDCP2x_SUPPORT_SHIFT    2
/* bit3~5:tmds */
#define DVI_SUPPORT_MASK        (1 << 3)
#define DVI_SUPPORT_SHIFT       3
#define HDMI_SUPPORT_MASK       (1 << 4)
#define HDMI_SUPPORT_SHIFT      4
#define MAX_TMDS_CLOCK_MASK     (1 << 5)
#define MAX_TMDS_CLOCK_SHIFT    5
/* bit6~7:scdc */
#define SCDC_PRESENT_MASK       (1 << 6)
#define SCDC_PRESENT_SHIFT      6
#define SCDC_LTE_340MCSC_MASK   (1 << 7)
#define SCDC_LTE_340MCSC_SHIFT  7
/* bit8~10:deepcolor */
#define BPC_30_MASK             (1 << 8)
#define BPC_30_SHIFT            8
#define BPC_36_MASK             (1 << 9)
#define BPC_36_SHIFT            9
#define BPC_48_MASK             (1 << 10)
#define BPC_48_SHIFT            10
/* bit11~17:frl */
#define MAX_FRL_RATE_MASK       (0xf << 11)
#define MAX_FRL_RATE_SHIFT      11
#define FFE_LEVELS_MASK         (0x7 << 15)
#define FFE_LEVELS_SHIFT        15
/* bit18~27:dsc */
#define DSC_SUPPORT_MASK        (1 << 18)
#define DSC_SUPPORT_SHIFT       18
#define NATIVE_Y420_MASK        (1 << 19)
#define NATIVE_Y420_SHIFT       19
#define DSC_10BPC_MASK          (1 << 20)
#define DSC_10BPC_SHIFT         20
#define DSC_12BPC_MASK          (1 << 21)
#define DSC_12BPC_SHIFT         21
#define MAX_SLICE_COUNT_MASK    (0xf << 22)
#define MAX_SLICE_COUNT_SHIFT   22
#define MAX_PIXEL_CLK_PER_SLICE_MASK    (0x3 << 26)
#define MAX_PIXEL_CLK_PER_SLICE_SHIFT   26
/* bit28~31:csc */
#define RGB2YUV_MASK           (1 << 28)
#define RGB2YUV_SHIFT           28
#define YCBCR444_MASK          (1 << 29)
#define YCBCR444_SHIFT          29
#define YCBCR422_MASK          (1 << 30)
#define YCBCR422_SHIFT          30
#define YCBCR420_MASK          (1 << 31)
#define YCBCR420_SHIFT          31

/*
 *  hdmi source capability 2 -caps2
 *  bit0: yuv2rgb                   0: no support;  1: support.
 *  bit1: dither_support            0: no support;  1: support.
 *  bit2-31: reserved.
 */

/* bit0~1: csc */
#define YUV2RGB_MASK           (1 << 0)
#define YUV2RGB_SHIFT          0
#define DITHER_SUPPORT_MASK    (1 << 1)
#define DITHER_SUPPORT_SHIFT   1

/* dsc caps max slice count define */
#define DSC_CAP_MAX_SLICE_CNT_0  0
#define DSC_CAP_MAX_SLICE_CNT_1  1
#define DSC_CAP_MAX_SLICE_CNT_2  2
#define DSC_CAP_MAX_SLICE_CNT_3  3
#define DSC_CAP_MAX_SLICE_CNT_4  4
#define DSC_CAP_MAX_SLICE_CNT_5  5
#define DSC_CAP_MAX_SLICE_CNT_6  6


#define HDMI_PROTOCAL_VERSION_1_4 0x1
#define HDMI_PROTOCAL_VERSION_2_0 0x2
#define HDMI_PROTOCAL_VERSION_2_1 0x3

#define HDMI_HDCP_DISABLE 0x0
#define HDMI_HDCP_1_4     0x1
#define HDMI_HDCP_2_2     0x2
#define HDMI_HDCP_2_3     0x4

#define HDMI_DBG(fmt...)   HI_DBG_PRINT(HI_ID_HDMITX, fmt)
#define HDMI_INFO(fmt...)  HI_INFO_PRINT(HI_ID_HDMITX, fmt)
#define HDMI_WARN(fmt...)  HI_WARN_PRINT(HI_ID_HDMITX, fmt)
#define HDMI_ERR(fmt...)   HI_ERR_PRINT(HI_ID_HDMITX, fmt)
#define HDMI_FATAL(fmt...) HI_FATAL_PRINT(HI_ID_HDMITX, fmt)

void hdmi_sysfs_event(struct hisilicon_hdmi *hdmi, char *event, hi_u32 size);
hi_s32 hdmi_phy_fcg_set(struct hisilicon_hdmi *hdmi);
hi_s32 hdmi_soft_reset(struct hisilicon_hdmi *hdmi, hi_bool fast);
hi_void hdmi_crg_set(struct hisilicon_hdmi *hdmi);
hi_s32 hdmi_frl_work_en(struct hisilicon_hdmi *hdmi);
hi_void hdmi_phy_on(struct hisilicon_hdmi *hdmi);
hi_void hdmi_phy_off(struct hisilicon_hdmi *hdmi);
hi_void hdmi_ao_notifiers(struct hisilicon_hdmi *hdmi, hi_u32 val);
#endif
