/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
* Description:head file of gpio
*/
#ifndef __HI_DRV_GPIO_H__
#define __HI_DRV_GPIO_H__

#include "hi_type.h"
#include "hi_debug.h"

#define HI_FATAL_GPIO(fmt...)   HI_FATAL_PRINT(HI_ID_GPIO, fmt)
#define HI_ERR_GPIO(fmt...)     HI_ERR_PRINT(HI_ID_GPIO, fmt)
#define HI_WARN_GPIO(fmt...)    HI_WARN_PRINT(HI_ID_GPIO, fmt)
#define HI_INFO_GPIO(fmt...)    HI_INFO_PRINT(HI_ID_GPIO, fmt)

#define HI_GPIO_GROUP_NUM 28

#define HI_GPIO_BIT_NUM 8

#define HI_GPIO_MAX_BIT_NUM (HI_GPIO_GROUP_NUM * HI_GPIO_BIT_NUM)

#define HI_GPIO_SPACE_SIZE  0x1000

#define     HI_GPIO_IRQ_NO (62 + 32)
#define     HI_GPIO_0_ADDR 0xA50000
#define     HI_GPIO_1_ADDR 0xA51000
#define     HI_GPIO_2_ADDR 0xA52000
#define     HI_GPIO_3_ADDR 0xA53000
#define     HI_GPIO_4_ADDR 0xA54000
#define     HI_GPIO_5_ADDR 0xA55000
#define     HI_GPIO_6_ADDR 0xA56000
#define     HI_GPIO_7_ADDR 0xA57000
#define     HI_GPIO_8_ADDR 0xA58000
#define     HI_GPIO_9_ADDR 0xA59000
#define     HI_GPIO_10_ADDR 0xA5A000
#define     HI_GPIO_11_ADDR 0xA5B000
#define     HI_GPIO_12_ADDR 0xA5C000
#define     HI_GPIO_13_ADDR 0xA5D000
#define     HI_GPIO_14_ADDR 0xA5E000
#define     HI_GPIO_15_ADDR 0xA5F000
#define     HI_GPIO_16_ADDR 0xA60000
#define     HI_GPIO_17_ADDR 0xA61000
#define     HI_GPIO_18_ADDR 0xA62000
#define     HI_GPIO_19_ADDR 0xA63000
#define     HI_GPIO_20_ADDR 0xA64000
#define     HI_GPIO_21_ADDR 0xA65000
#define     HI_GPIO_22_ADDR 0xA66000
#define     HI_GPIO_23_ADDR 0x861000
#define     HI_GPIO_24_ADDR 0x862000
#define     HI_GPIO_25_ADDR 0x863000
#define     HI_GPIO_26_ADDR 0x864000
#define     HI_GPIO_27_ADDR 0xA5D000

#define     HI_GPIO_DIR_REG     0x400
#define     HI_GPIO_IS_REG      0x404
#define     HI_GPIO_IBE_REG     0x408
#define     HI_GPIO_IEV_REG     0x40c
#define     HI_GPIO_IE_REG      0x410
#define     HI_GPIO_RIS_REG     0x414
#define     HI_GPIO_MIS_REG     0x418
#define     HI_GPIO_IC_REG      0x41c

#define     HI_GPIO_OUTPUT  1
#define     HI_GPIO_INPUT   0
#define     REG_USR_ADDR(RegAddr) * ((volatile hi_u32 *)(RegAddr))
#define     HI_GPIO_ADDR_SIZE   0x1000

typedef struct {
    hi_u32 gpio_num;
    hi_u32 timeout_ms;
} gpio_interrupt;

typedef struct {
    hi_u8 gpio_group_num;     /* gpio group number */
    hi_u8 gpio_max_num;       /* gpio max number */
} gpio_get_gpio_num;

typedef enum {
    HI_GPIO_INTTYPE_UP,     /**<spring by the up edge */                 /**< CNcomment:上升沿触发 */
    HI_GPIO_INTTYPE_DOWN,   /**<spring by the down edge */               /**< CNcomment:下降沿触发 */
    HI_GPIO_INTTYPE_UPDOWN, /**<spring by both the up and down edge */   /**< CNcomment:双沿触发 */
    HI_GPIO_INTTYPE_HIGH,   /**<spring by the high level */              /**< CNcomment:高电平触发 */
    HI_GPIO_INTTYPE_LOW,    /**<spring by the low level */               /**< CNcomment:低电平触发 */
    HI_GPIO_INTTYPE_MAX     /**<Invalid value */                         /**< CNcomment:非法边界值 */
} hi_gpio_interrupt_type;

typedef enum {
    HI_GPIO_OUTPUTTYPE_CMOS,
    HI_GPIO_OUTPUTTYPE_OD,
    HI_GPIO_OUTPUTTYPE_MAX
}hi_gpio_outputtype;

typedef struct {
    hi_u32                gpio_num;
    hi_u32                enable;
    hi_gpio_interrupt_type    interrupt_type; /* gpio interrupt trigger mode */
} gpio_data;

hi_s32    hi_drv_gpio_init(hi_void);
hi_void   hi_drv_gpio_deinit(hi_void);

hi_s32 hi_drv_gpio_set_direction_bit(hi_u32 gpio_num, hi_u32 dir_bit);
hi_s32 hi_drv_gpio_get_direction_bit(hi_u32 gpio_num, hi_u32 *pdir_bit);
hi_s32 hi_drv_gpio_write_bit(hi_u32 gpio_num, hi_u32 bit_value);
hi_s32 hi_drv_gpio_read_bit(hi_u32 gpio_num, hi_u32 *pbit_value);
hi_s32 hi_drv_gpio_get_gpio_num(gpio_get_gpio_num *gpio_num);

hi_s32 hi_drv_gpio_set_interrupt_type(hi_u32 gpio_num, hi_gpio_interrupt_type interrupt_trigger_mode);
hi_s32 hi_drv_gpio_set_bit_interrupt_enable(hi_u32 gpio_num, hi_bool enable);
hi_s32 hi_drv_gpio_register_server_func(hi_u32 gpio_num, hi_void (*func)(hi_u32));
hi_s32 hi_drv_gpio_unregister_server_func(hi_u32 gpio_num);
hi_s32 hi_drv_gpio_clear_group_interrupt(hi_u32 group_num);
hi_s32 hi_drv_gpio_clear_bit_interrupt(hi_u32 gpio_num);

#endif /* End of #ifndef __HI_DRV_GPIO_H__ */
