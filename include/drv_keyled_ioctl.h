 /*
  * Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
  * Description:
  */

#ifndef  __DRV_KEYLED_IOCTL_H__
#define  __DRV_KEYLED_IOCTL_H__

typedef struct {
    hi_u32 key_code;
    hi_u32 key_status;
} get_key_value;

typedef enum {
    HI_KEYLED_CATHODE = 0x0,
    HI_KEYLED_ANODE,
    HI_KEYLED_LEDTYPE_MAX
} hi_keyled_led_type;

typedef enum {
    HI_KEYLED_CS_LOW = 0x0,
    HI_KEYLED_CS_HIGH,
    HI_KEYLED_CSTYPE_MAX
} hi_keyled_cs_type;

typedef enum {
    HI_KEYLED_4LED = 0x0,
    HI_KEYLED_5LED,
    HI_KEYLED_LEDNUM_MAX
} hi_keyled_led_num;

typedef enum {
    HI_KEYLED_2MUL4 = 0x0,
    HI_KEYLED_1MUL8,
    HI_KEYLED_KEYSCAN_MAX
} hi_keyled_key_scan;

typedef enum {
    HI_KEYLED_1MUL8_LOW = 0x0,
    HI_KEYLED_1MUL8_HIGH,
    HI_KEYLED_KEYLEVEL_MAX
} hi_keyled_key_level;

typedef struct {
    hi_u8 led_num;
    hi_u8 *led_data_ptr;
} hi_keyled_display;


typedef struct {
    hi_keyled_key_scan key_scan_mode;
    hi_keyled_led_num led_num;
    hi_keyled_key_level key_level;
} hi_keyled_mode;

#define HI_KEYLED_KEY_OPEN_CMD              _IO(HI_ID_KEYLED, 1)
#define HI_KEYLED_KEY_CLOSE_CMD             _IO(HI_ID_KEYLED, 2)
#define HI_KEYLED_KEY_RESET_CMD             _IO(HI_ID_KEYLED, 3)
#define HI_KEYLED_KEY_GET_VALUE_CMD         _IOR(HI_ID_KEYLED, 4, get_key_value)
#define HI_KEYLED_SET_BLOCK_TIME_CMD        _IOW(HI_ID_KEYLED, 5, hi_u32)
#define HI_KEYLED_SET_IS_KEYUP_CMD          _IOW(HI_ID_KEYLED, 6, hi_u32)
#define HI_KEYLED_SET_IS_REPKEY_CMD         _IOW(HI_ID_KEYLED, 7, hi_u32)
#define HI_KEYLED_SET_REPKEY_TIME_CMD       _IOW(HI_ID_KEYLED, 8, hi_u32)

#define HI_KEYLED_LED_OPEN_CMD              _IO(HI_ID_KEYLED, 11)
#define HI_KEYLED_LED_CLOSE_CMD             _IO(HI_ID_KEYLED, 12)
#define HI_KEYLED_DISPLAY_CODE_CMD          _IOW(HI_ID_KEYLED, 13, hi_u32)
#define HI_KEYLED_DISPLAY_TIME_CMD          _IOW(HI_ID_KEYLED, 14, hi_unf_keyled_time)
#define HI_KEYLED_SET_FLASH_PIN_CMD         _IOW(HI_ID_KEYLED, 15, hi_u32)
#define HI_KEYLED_CONFIG_FLASH_FREQ_CMD     _IOW(HI_ID_KEYLED, 16, hi_unf_keyled_level)

#define HI_KEYLED_SELECT_CMD                _IOW(HI_ID_KEYLED, 17, hi_unf_keyled_type)
#define HI_KEYLED_SET_MODE_CMD              _IOW(HI_ID_KEYLED, 18, hi_keyled_mode)
#define HI_KEYLED_DISPLAY_MULITLED_CMD      _IOW(HI_ID_KEYLED, 19, hi_keyled_display)
#define HI_KEYLED_SETLOCK_CMD               _IOW(HI_ID_KEYLED, 20, hi_u32)

#endif  /*  __DRV_KEYLED_IOCTL_H__ */