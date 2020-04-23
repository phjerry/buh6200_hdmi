/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: header file for driver and api code
 */

#ifndef __HI_DRV_PMOC_H__
#define __HI_DRV_PMOC_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define HI_PMOC_WAKEUP_GPIO_MAXNUM 4
/* GPIO: REG_GPIO0 ~ REG_GPIO22, REG_AON_GPIO0 ~ REG_AON_GPIO4 */
#define HI_PMOC_WAKEUP_GPIO_GROUP_MIN 23
#define HI_PMOC_WAKEUP_GPIO_GROUP_MAX 27

#define HI_PMOC_WAKEUP_IRKEY_MAXNUM 10
#define HI_PMOC_POWEROFF_GPIO_MAXNUM 4

#define HI_PMOC_FILTER_MIN_OFFSET 12
#define HI_PMOC_FILTER_VALUE_COUNT 31
#define HI_PMOC_FILTER_COUNT 4
#define HI_PMOC_HDMITX_ID_MAXNUM 4

#define HI_PMOC_DISPLAY_MAX_DIGIT 9999

#define HI_PMOC_DISPLAY_MAX_HOUR 23
#define HI_PMOC_DISPLAY_MAX_MINUTE 59
#define HI_PMOC_DISPLAY_MAX_SECOND 59

#define HI_PMOC_WAKEUP_GPIO_BIT_MIN 0
#define HI_PMOC_WAKEUP_GPIO_BIT_MAX 7

/* wakeup source */
typedef enum {
    HI_PMOC_WAKEUP_TYPE_IR = 0,
    HI_PMOC_WAKEUP_TYPE_KEYLED,
    HI_PMOC_WAKEUP_TYPE_GPIO,
    HI_PMOC_WAKEUP_TYPE_LSADC,
    HI_PMOC_WAKEUP_TYPE_UART,
    HI_PMOC_WAKEUP_TYPE_ETH,
    HI_PMOC_WAKEUP_TYPE_USB,
    HI_PMOC_WAKEUP_TYPE_VGA = 0x10,
    HI_PMOC_WAKEUP_TYPE_SCART,
    HI_PMOC_WAKEUP_TYPE_HDMIRX_PLUGIN,
    HI_PMOC_WAKEUP_TYPE_HDMIRX_CEC,
    HI_PMOC_WAKEUP_TYPE_HDMITX_CEC,
    HI_PMOC_WAKEUP_TYPE_TIMEOUT = 0x20,
    HI_PMOC_WAKEUP_TYPE_MAX
} hi_pmoc_wakeup_src;

typedef enum {
    HI_PMOC_WAKEUP_TO_DDR = 0,
    HI_PMOC_WAKEUP_RESET,
    HI_PMOC_WAKEUP_MAX
} hi_pmoc_wakeup_type;

typedef enum {
    HI_PMOC_CEC_CONTROL_TYPE_BILATERAL = 0,
    HI_PMOC_CEC_CONTROL_TYPE_TV_TO_CHIP,
    HI_PMOC_CEC_CONTROL_TYPE_CHIP_TO_TV,
    HI_PMOC_CEC_CONTROL_TYPE_MAX
} hi_pmoc_cec_control_type;

typedef enum {
    HI_PMOC_ACTIVE_STANDBY_MODLE1,
    HI_PMOC_ACTIVE_STANDBY_MODLE2,
    HI_PMOC_ACTIVE_STANDBY_MODLE_MAX
} hi_pmoc_active_standby_modle;

typedef struct {
    hi_u32 mask_bytes;
    hi_u8 offset;
    hi_u8 value[HI_PMOC_FILTER_VALUE_COUNT];
    hi_bool filter_valid;
} hi_pmoc_wakeup_frame;

typedef union {
    struct {
        hi_u32 ir_type;
        hi_u32 ir_num;
        hi_u32 ir_low_val[HI_PMOC_WAKEUP_IRKEY_MAXNUM];
        hi_u32 ir_high_val[HI_PMOC_WAKEUP_IRKEY_MAXNUM];
    } ir_param;
    struct {
        hi_u32 keyled_type;
        hi_u32 wakeup_key;
    } keyled_param;
    struct {
        hi_u32 num;
        hi_u8 group[HI_PMOC_WAKEUP_GPIO_MAXNUM];
        hi_u8 bit[HI_PMOC_WAKEUP_GPIO_MAXNUM];
        hi_u8 interrupt_type[HI_PMOC_WAKEUP_GPIO_MAXNUM];
    } gpio_param;
    struct {
        hi_u32 keypad_mask;
    } lsadc_param;
    struct {
        hi_u8 wakeup_key;
    } uart_param;
    struct {
        hi_u32 index;  /* 0: eth0, 1: eth1 */
        hi_bool unicast_packet_enable;
        hi_bool magic_packet_enable;
        hi_bool wakeup_frame_enable;
        hi_pmoc_wakeup_frame frame[HI_PMOC_FILTER_COUNT];
        hi_u32 time_to_passive_standby;
    } eth_param;
    struct {
        hi_u32 usb_wakeup_mask;
    } usb_param;
    struct {
        hi_u32 port;
    } hdmirx_plugin_param;
    struct {
        hi_bool id[HI_PMOC_HDMITX_ID_MAXNUM];
        hi_pmoc_cec_control_type cec_control[HI_PMOC_HDMITX_ID_MAXNUM];
    } hdmitx_cec_param;
    struct {
        hi_u32 suspend_period;
        hi_bool pvr_enable;
    } timeout_param;
} hi_pmoc_suspend_param;

typedef struct {
    hi_pmoc_wakeup_src source;
    union {
        struct {
            hi_u32 ir_low_val;
            hi_u32 ir_high_val;
        } ir_param;
        struct {
            hi_u8 group;
            hi_u8 bit;
        } gpio_param;
    } wakeup_param;
} hi_pmoc_wakeup_attr;

typedef struct {
    hi_u32 hour;
    hi_u32 minute;
    hi_u32 second;
} hi_pmoc_display_time;

typedef enum {
    HI_PMOC_NO_DISPLAY = 0,
    HI_PMOC_DISPLAY_DIGIT,
    HI_PMOC_DISPLAY_TIME,
    HI_PMOC_DISPLAY_TYPE_MAX
} hi_pmoc_display_type;

typedef struct {
    hi_u32 keyled_type;
    hi_pmoc_display_type display_type;
    hi_u32 display_value;
} hi_pmoc_display_param;

typedef struct {
    hi_u32 num;
    hi_u8 group[HI_PMOC_POWEROFF_GPIO_MAXNUM];
    hi_u8 bit[HI_PMOC_POWEROFF_GPIO_MAXNUM];
    hi_bool level[HI_PMOC_POWEROFF_GPIO_MAXNUM];
} hi_pmoc_poweroff_gpio_param;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* End of #ifndef __HI_DRV_PMOC_H__*/

