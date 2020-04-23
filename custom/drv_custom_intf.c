/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2018. All rights reserved.
 * Description: for kernel customization
 * Author: lilele l00494535
 * Create: 2014-06-14
 * History: 2019-04-04 l00494535 CSEC
 */

#include <linux/module.h>
#include "hi_type.h"
#include "hi_osal.h"
#include "hi_module.h"
#include "hi_log.h"
#include "hi_board.h"
#include "hi_drv_i2c.h"
#include "hi_drv_gpio.h"
#include "hi_drv_panel.h"
#include "hi_drv_gpio.h"
#include "hi_drv_pdm.h"
#include "hi_drv_custom.h"

#include "drv_pdm_ext.h"

#define HI_BOARD_PANEL_I2C_DEV_ADDR_EXT 0x40

#define custom_log(msg, args...)                                        \
    do {                                                                \
        HI_PRINT("CUSTOM func[%s] line[%d] :", __FUNCTION__, __LINE__); \
        HI_PRINT(msg, ##args);                                          \
    } while (0)

#define custom_check_func_ret(ret, func)  \
    do {                                     \
        (ret) = (func);                       \
        if ((ret) != HI_SUCCESS) {          \
            custom_log(#func " failure!\n"); \
        }                                    \
    } while (0)

#define custom_check_func(ret, func)      \
    do {                                     \
        (ret) = (func);                       \
        if ((ret) != HI_SUCCESS) {          \
            custom_log(#func " failure!\n"); \
        }                                    \
    } while (0)

#define custom_check_pointer_ret(p)              \
    do {                                         \
        if ((p) == NULL) {                         \
            custom_log(#p "is null pointer!\n"); \
        }                                        \
    } while (0)

#define custom_ms_delay(ms) \
    do {                    \
        osal_msleep_uninterruptible(ms);         \
    } while (0)

custom_memec g_frc_memc = { 3, 0, { 0, 0, 0, 0, 0, 0 }, 1 };
hi_u8(*g_panel_fhd60_data)
[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = HI_NULL;
hi_u8(*g_panel_fhd120_data)
[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = HI_NULL;
hi_u8(*g_panel_4k30_data)
[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = HI_NULL;
hi_u8(*g_panel_4k60_data)
[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = HI_NULL;

/* ==========INX  MISC PANEL I2C CMD START =============
 * inx timing switch to FHD I2C command
 */
static hi_u8 g_panel_fhd_data_inx[][HI_BOARD_PANEL_I2C_DATA_LEN + 1] = {
    { 0x40, 0x01, 0x02, 0x00, 0x00, 0x11, 0x00 },
    { 0x30, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00 },
    { 0x61, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x81, 0x05, 0x07, 0x80, 0x04, 0x38, 0x00 },
    { 0x13, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 },
};

/* inx timing switch to 4k30 I2C command */
static hi_u8 g_panel_4k30_data_inx[][HI_BOARD_PANEL_I2C_DATA_LEN + 1] = {
    { 0x80, 0x01, 0x02, 0x00, 0x00, 0x55, 0x00 },
    { 0x80, 0x02, 0x00, 0x00, 0x02, 0x01, 0x03 },
    { 0x61, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0xC1, 0x05, 0x0F, 0x00, 0x08, 0x70, 0x00 },
    { 0x13, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 },

};

/* inx timing switch to 4k60 I2C command */
static hi_u8 g_panel_4k60_data_inx[][HI_BOARD_PANEL_I2C_DATA_LEN + 1] = {
    { 0x20, 0x01, 0x12, 0x00, 0x40, 0xFF, 0x00 },
    { 0xC0, 0x02, 0x00, 0x20, 0x31, 0x64, 0x75 },
    { 0x61, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0xC1, 0x05, 0x0F, 0x00, 0x08, 0x70, 0x00 },
    { 0x13, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 },

};


/* ==========M5  MISC PANEL I2C CMD START =============
 * M5 timing switch to FHD60 I2C command
 */
static hi_u8 g_panel_fhd60_data_m5[][HI_BOARD_PANEL_I2C_DATA_LEN + 1] = {
    { 0xF1, 0x0A, 0x00, 0x01, 0x04, 0x00, 0x00 },
};

/* M5 timing switch to FHD120 I2C command */
static hi_u8 g_panel_fhd120_data_m5[][HI_BOARD_PANEL_I2C_DATA_LEN + 1] = {
    { 0xe1, 0x0A, 0x00, 0x01, 0x03, 0x00, 0x00 },
};

/* M5 timing switch to 4k30 I2C command */
static hi_u8 g_panel_4k30_data_m5[][HI_BOARD_PANEL_I2C_DATA_LEN + 1] = {
    { 0xD1, 0x0A, 0x00, 0x01, 0x02, 0x00, 0x00 },
};

/* inx timing switch to 4k60 I2C command */
static hi_u8 g_panel_4k60_data_m5[][HI_BOARD_PANEL_I2C_DATA_LEN + 1] = {
    { 0xC1, 0x0A, 0x01, 0x01, 0x00, 0x00, 0x00 },

};

/* ==========MFC9 PANEL I2C CMD START =============
  * MFC9 timing switch to FHD120 I2C command
  */
static hi_u8 g_panel_fhd120_mfc9[][HI_BOARD_PANEL_I2C_DATA_LEN + 1] = {
    { 0x2D, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00 },
    { 0x81, 0x05, 0x07, 0x80, 0x04, 0x38, 0x00 },
};

/* MFC9 timing switch to 4k60 I2C command */
static hi_u8 g_panel_4k2k60_mfc9[][HI_BOARD_PANEL_I2C_DATA_LEN + 1] = {
    { 0x1D, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0xc1, 0x05, 0x0F, 0x00, 0x08, 0x70, 0x00 },
    { 0x13, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x2D, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x30, 0x01, 0x02, 0x80, 0x00, 0x00, 0x00 },
    { 0x30, 0x02, 0xff, 0x9b, 0x8a, 0x57, 0x46 },
    { 0x61, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00 },

};

hi_s32 custom_reset_usb3_dev(hi_void *args)
{
    hi_u32 ret;

    gpio_ext_func *gpio_func = NULL;
    custom_check_func_ret (ret, osal_exportfunc_get(HI_ID_GPIO, (hi_void **)(&gpio_func)));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    custom_check_pointer_ret(gpio_func->pfn_gpio_dir_set_bit);
    if (gpio_func->pfn_gpio_dir_set_bit == NULL) {
        return HI_FAILURE;
    }
    custom_check_pointer_ret(gpio_func->pfn_gpio_write_bit);
    if (gpio_func->pfn_gpio_write_bit == NULL) {
        return HI_FAILURE;
    }

    custom_check_func_ret(ret, gpio_func->pfn_gpio_dir_set_bit(HI_BOARD_USB3DEV_PWR_GPIONUM, HI_BOARD_GPIO_OUT));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    custom_check_func_ret(ret, gpio_func->pfn_gpio_write_bit(HI_BOARD_USB3DEV_PWR_GPIONUM,
        HI_BOARD_USB3DEV_POWERDOWN_OUTPUT_VALUE));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    osal_msleep(1000); /* sleep 1000ms */
    custom_check_func_ret(ret, gpio_func->pfn_gpio_write_bit(HI_BOARD_USB3DEV_PWR_GPIONUM,
        !HI_BOARD_USB3DEV_POWERDOWN_OUTPUT_VALUE));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    return ret;
}

static hi_s32 custom_write_i2c(hi_u32 i2c_num, hi_u8 i2c_dev_addr, hi_u32 i2c_reg_addr,
    hi_u32 i2c_reg_addr_byte_num, hi_u8 *data_ptr, hi_u32 data_len)
{
    hi_s32 ret;
    i2c_ext_func *i2c_func = NULL;

    custom_check_func_ret (ret, osal_exportfunc_get(HI_ID_I2C, (hi_void **)(&i2c_func)));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    custom_check_pointer_ret(i2c_func->pfn_i2c_write);
    if (i2c_func->pfn_i2c_write == NULL) {
        return HI_FAILURE;
    }

    custom_check_func_ret(ret,
                          i2c_func->pfn_i2c_write(i2c_num, i2c_dev_addr, i2c_reg_addr,
                          i2c_reg_addr_byte_num, data_ptr, data_len));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    custom_ms_delay(10);

    return HI_SUCCESS;
}
#if 0

static hi_s32 custom_read_i2c(hi_u32 i2c_num, hi_u8 i2c_dev_addr,
    hi_u32 i2c_reg_addr, hi_u32 i2c_reg_addr_byte_num, hi_u8 *data_ptr, hi_u32 data_len)
{
    hi_s32 ret;
    i2c_ext_func *i2c_func = NULL;

    custom_check_func_ret (ret, osal_exportfunc_get(HI_ID_I2C, (hi_void **)(&i2c_func)));
    custom_check_pointer_ret(i2c_func->pfn_i2c_read);

    custom_check_func_ret(ret,
                          i2c_func->pfn_i2c_read(i2c_num, i2c_dev_addr, i2c_reg_addr,
                          i2c_reg_addr_byte_num, data_ptr, data_len));

    custom_ms_delay(10);

    return HI_SUCCESS;
}
#endif
/* ========= panel functions ========= */
typedef enum {
    CUSTOM_PANEL_INDEX_AUO_21_1920X1080_60HZ2D,
    CUSTOM_PANEL_INDEX_LG_37_1920X1080_60HZ_2D,
    CUSTOM_PANEL_INDEX_LG_32_1366X768_60HZ_2D,
    CUSTOM_PANEL_INDEX_LG_001_1920X1080_60HZ_3D_LR,
    CUSTOM_PANEL_INDEX_LG_002_1920X1080_60HZ_3D_SBS,
    CUSTOM_PANEL_INDEX_LG_003_1920X1080_60HZ_3D_LR,
    CUSTOM_PANEL_INDEX_AUO_V7500_1920X1080_60HZ_3D,
    CUSTOM_PANEL_INDEX_INX_001_1920X1080_120HZ_3D_FS,
    CUSTOM_PANEL_INDEX_CMO_V500_1920X1080_60HZ_3D_LR,
    CUSTOM_PANEL_INDEX_AUO_V6500_1920X1080_60HZ_3D,
    CUSTOM_PANEL_INDEX_VGA_1366X768_60HZ_2D,
    CUSTOM_PANEL_INDEX_CMO_3840X2160_60HZ_2D,
    CUSTOM_PANEL_INDEX_LG_3840X2160_60HZ_2D,
    CUSTOM_PANEL_INDEX_CSOT_3840X2160_60HZ_2D,
    CUSTOM_PANEL_INDEX_LG_3840X2160_LDM_13_8_104_AREA,
    CUSTOM_PANEL_INDEX_LG_3840X2160_LDM_18_10_108_AREA,
    CUSTOM_PANEL_INDEX_CMO_3840X2160_60HZ_3D_LR_FOR_TEST,
    CUSTOM_PANEL_INDEX_LG_3840X2160_60HZ_3D_LR_FOR_TEST,
    CUSTOM_PANEL_INDEX_CSOT_3840X2160_60HZ_3D_FS_FOR_TEST,
    CUSTOM_PANEL_INDEX_CMO_3840X2160_60HZ_2D_MISC,
    CUSTOM_PANEL_INDEX_VBO_HDMI_1920X1080_60HZ_2D,
    CUSTOM_PANEL_INDEX_M5V1_3840X2160_60HZ_2D_MISC,
    CUSTOM_PANEL_INDEX_INX_3840X2160_LDM_2_8_16_AREA,
    CUSTOM_PANEL_INDEX_LG_3840X2160_LDM_20_12_240_AREA,
    CUSTOM_PANEL_INDEX_CMO_OVERDELAY_3840X2160_60HZ_2D,
    CUSTOM_PANEL_INDEX_INX_3840X2160_60HZ_3D_FS,
    CUSTOM_PANEL_INDEX_LG_PWMINV_3840X2160_60HZ_2D,

    CUSTOM_PANEL_INDEX_BUTT
} g_custom_panel_index;

/* ========= panel functions ========= */
static hi_s32 custom_set_panel_fhd60_data(hi_u32 index)
{
    hi_u32 i;
    hi_u32 cmd_num;

    switch (index) {
        case CUSTOM_PANEL_INDEX_CMO_3840x2160_60HZ_2D_MISC: {
            g_panel_fhd60_data = g_panel_fhd_data_inx;
            cmd_num = sizeof(g_panel_fhd_data_inx) / sizeof(g_panel_fhd_data_inx[0]);
            if (cmd_num != 0) {
                for (i = 0; i < cmd_num; i++) {
                    custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
                        g_panel_fhd60_data[i][0], 1,
                        (hi_u8 *)(&g_panel_fhd60_data[i][1]), HI_BOARD_PANEL_I2C_DATA_LEN);
                }
            }
            break;
        }
        case CUSTOM_PANEL_INDEX_M5V1_3840x2160_60HZ_2D_MISC: {
            g_panel_fhd60_data = g_panel_fhd60_data_m5;
            cmd_num = sizeof(g_panel_fhd60_data_m5) / sizeof(g_panel_fhd60_data_m5[0]);
            if (cmd_num != 0) {
                for (i = 0; i < cmd_num; i++) {
                    custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
                        g_panel_fhd60_data[i][0], 1,
                        (hi_u8 *)(&g_panel_fhd60_data[i][1]), HI_BOARD_PANEL_I2C_DATA_LEN);
                }
            }
            break;
        }
        case CUSTOM_PANEL_INDEX_INX_3840x2160_60HZ_3D_FS: {
            // call function
            g_panel_fhd60_data = HI_NULL;
            break;
        }
        default:
        {
            g_panel_fhd60_data = HI_NULL;
            break;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 custom_set_panel_fhd120_data(hi_u32 index)
{
    hi_u32 i;
    hi_u32 cmd_num;

    switch (index) {
        case CUSTOM_PANEL_INDEX_CMO_3840x2160_60HZ_2D_MISC: {
            g_panel_fhd120_data = HI_NULL;

            break;
        }
        case CUSTOM_PANEL_INDEX_M5V1_3840x2160_60HZ_2D_MISC: {
            g_panel_fhd120_data = g_panel_fhd120_data_m5;
            cmd_num = sizeof(g_panel_fhd120_data_m5) / sizeof(g_panel_fhd120_data_m5[0]);
            if (cmd_num != 0) {
                for (i = 0; i < cmd_num; i++) {
                    custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
                        g_panel_fhd120_data[i][0], 1,
                        (hi_u8 *)(&g_panel_fhd120_data[i][1]), HI_BOARD_PANEL_I2C_DATA_LEN);
                }
            }
            break;
        }
        case CUSTOM_PANEL_INDEX_INX_3840x2160_60HZ_3D_FS: {
            // call function
            g_panel_fhd120_data = g_panel_fhd120_mfc9;
            cmd_num = sizeof(g_panel_fhd120_mfc9) / sizeof(g_panel_fhd120_mfc9[0]);
            if (cmd_num != 0) {
                for (i = 0; i < cmd_num; i++) {
                    custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR_EXT,
                        g_panel_fhd120_data[i][0], 1,
                        (hi_u8 *)(&g_panel_fhd120_data[i][1]), HI_BOARD_PANEL_I2C_DATA_LEN);
                }
            }
            break;
        }
        default:
        {
            g_panel_fhd120_data = HI_NULL;
            break;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 custom_set_panel_4k2k30_data(hi_u32 index)
{
    hi_u32 i;
    hi_u32 cmd_num;

    switch (index) {
        case CUSTOM_PANEL_INDEX_CMO_3840x2160_60HZ_2D_MISC: {
            g_panel_4k30_data = g_panel_4k30_data_inx;
            cmd_num = sizeof(g_panel_4k30_data_inx) / sizeof(g_panel_4k30_data_inx[0]);
            if (cmd_num != 0) {
                for (i = 0; i < cmd_num; i++) {
                    custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
                        g_panel_4k30_data[i][0], 1,
                        (hi_u8 *)(&g_panel_4k30_data[i][1]), HI_BOARD_PANEL_I2C_DATA_LEN);
                }
            }
            break;
        }
        case CUSTOM_PANEL_INDEX_M5V1_3840x2160_60HZ_2D_MISC: {
            g_panel_4k30_data = g_panel_4k30_data_m5;
            cmd_num = sizeof(g_panel_4k30_data_m5) / sizeof(g_panel_4k30_data_m5[0]);
            if (cmd_num != 0) {
                for (i = 0; i < cmd_num; i++) {
                    custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
                        g_panel_4k30_data[i][0], 1,
                        (hi_u8 *)(&g_panel_4k30_data[i][1]), HI_BOARD_PANEL_I2C_DATA_LEN);
                }
            }
            break;
        }
        case CUSTOM_PANEL_INDEX_INX_3840x2160_60HZ_3D_FS: {
            // call function
            g_panel_4k30_data = HI_NULL;

            break;
        }
        default:
        {
            g_panel_4k30_data = HI_NULL;
            break;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 custom_set_panel_4k2k60_data(hi_u32 index)
{
    hi_u32 i;
    hi_u32 cmd_num;

    switch (index) {
        case CUSTOM_PANEL_INDEX_CMO_3840x2160_60HZ_2D_MISC: {
            g_panel_4k60_data = g_panel_4k60_data_inx;
            cmd_num = sizeof(g_panel_4k60_data_inx) / sizeof(g_panel_4k60_data_inx[0]);
            if (cmd_num != 0) {
                for (i = 0; i < cmd_num; i++) {
                    custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
                        g_panel_4k60_data[i][0], 1,
                        (hi_u8 *)(&g_panel_4k60_data[i][1]), HI_BOARD_PANEL_I2C_DATA_LEN);
                }
            }
            break;
        }
        case CUSTOM_PANEL_INDEX_M5V1_3840x2160_60HZ_2D_MISC: {
            g_panel_4k60_data = g_panel_4k60_data_m5;
            cmd_num = sizeof(g_panel_4k60_data_m5) / sizeof(g_panel_4k60_data_m5[0]);
            if (cmd_num != 0) {
                for (i = 0; i < cmd_num; i++) {
                    custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
                        g_panel_4k60_data[i][0], 1,
                        (hi_u8 *)(&g_panel_4k60_data[i][1]), HI_BOARD_PANEL_I2C_DATA_LEN);
                }
            }
            break;
        }
        case CUSTOM_PANEL_INDEX_INX_3840x2160_60HZ_3D_FS: {
            // call function
            g_panel_4k60_data = g_panel_4k2k60_mfc9;
            cmd_num = sizeof(g_panel_4k2k60_mfc9) / sizeof(g_panel_4k2k60_mfc9[0]);
            if (cmd_num != 0) {
                for (i = 0; i < cmd_num; i++) {
                    custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR_EXT,
                        g_panel_4k60_data[i][0], 1,
                        (hi_u8 *)(&g_panel_4k60_data[i][1]), HI_BOARD_PANEL_I2C_DATA_LEN);
                }
            }
            break;
        }
        default:
        {
            g_panel_4k60_data = HI_NULL;
            break;
        }
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_custom_set_panel_mode(hi_drv_panel_timming timming_type)
{
    hi_s32 ret;
    hi_u32 total_index;
    hi_u32 cur_index;
    pdm_export_func *pdm_func = NULL;

    /* get the pointer of PDM's functions */
    custom_check_func_ret (ret, osal_exportfunc_get(HI_ID_PDM, (hi_void **)(&pdm_func)));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    custom_check_pointer_ret(pdm_func->pdm_get_panel_index_ptr);
    if (pdm_func->pdm_get_panel_index_ptr == NULL) {
        return HI_FAILURE;
    }

    /* get panel index */
    custom_check_func_ret(ret, pdm_func->pdm_get_panel_index_ptr(&total_index, &cur_index));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    switch (timming_type) {
        case HI_DRV_PANEL_TIMMING_FHD_60HZ: {
            custom_set_panel_fhd60_data(cur_index);
            break;
        }
        case HI_DRV_PANEL_TIMMING_FHD_120HZ: {
            custom_set_panel_fhd120_data(cur_index);
            break;
        }
        case HI_DRV_PANEL_TIMMING_4K2K_30HZ: {
            custom_set_panel_4k2k30_data(cur_index);
            break;
        }
        case HI_DRV_PANEL_TIMMING_4K2K_60HZ: {
            custom_set_panel_4k2k60_data(cur_index);
            break;
        }
        default:
            break;
    }

    return HI_SUCCESS;
}

/*******************************************************************
                                  M5 CONTROL I2C code
********************************************************************/
#if USE_MX_FUNCTION
static hi_s32 hi_drv_custom_cal_i2c_cmd_check_sum(hi_u8 i2c_data[], int size)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u8 i = 0;
    hi_u8 check_sum = 0x00;

    for (i = 1; i < size; i++) {
        check_sum = check_sum + i2c_data[i];
    }

    i2c_data[0] = (i2c_data[0] & 0x0f) + ((check_sum & 0xf) << 4); /* use 0 && 4 items to calculate checksum */

    return ret;
}

hi_s32 hi_drv_custom_set_m5_panel_power(hi_bool power_on)
{
    hi_u8 m5_panel_power_on_data[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = { 0xf1, 0x2e, 0x01, 0x00, 0x00, 0x00, 0x00 };
    hi_u8 m5_panel_power_off_data[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = { 0xe1, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x00 };

    if (power_on) {
        return custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
            m5_panel_power_on_data[0], 1, (hi_u8 *)(&m5_panel_power_on_data[1]), HI_BOARD_PANEL_I2C_DATA_LEN);
    } else {
        return custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
            m5_panel_power_off_data[0], 1, (hi_u8 *)(&m5_panel_power_off_data[1]), HI_BOARD_PANEL_I2C_DATA_LEN);
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_custom_set_m5_ldm_strength_level(hi_u32 strength_level)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u8 m5_ldm_strength_level[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = { 0x92, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00 };

    if (strength_level <= 2) { /* 2 means high , 1 means mid , 0 means low  */
        m5_ldm_strength_level[2] = (hi_u8)strength_level; /* 2 items in m5_ldm_strength_level */
        hi_drv_custom_cal_i2c_cmd_check_sum(m5_ldm_strength_level, HI_BOARD_PANEL_I2C_DATA_LEN + 1);

        return custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
            m5_ldm_strength_level[0], 1, (hi_u8 *)(&m5_ldm_strength_level[1]),
            HI_BOARD_PANEL_I2C_DATA_LEN);
    }

    return ret;
}

hi_s32 hi_drv_custom_set_m5_local_dimming_demo_mode(hi_u32 demo_mode)
{
    hi_u8 m5_local_dimming_demo_mode[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = { 0xf1, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x00 };

    if (demo_mode < 11) { /* 11 means HI_DRV_PANEL_LDM_DEMO_BUTT */
        m5_local_dimming_demo_mode[2] = (hi_u8)demo_mode; /* 2 items in m5_local_dimming_demo_mode */
        hi_drv_custom_cal_i2c_cmd_check_sum(m5_local_dimming_demo_mode, HI_BOARD_PANEL_I2C_DATA_LEN + 1);

        return custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
            m5_local_dimming_demo_mode[0], 1, (hi_u8 *)(&m5_local_dimming_demo_mode[1]),
            HI_BOARD_PANEL_I2C_DATA_LEN);
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_custom_get_m5_memc_level(hi_u32 *memc_level)
{
    if (memc_level == HI_NULL) {
        custom_log("get M5 memc level failed, param point is NULL!\n");
        return HI_FAILURE;
    }

    *memc_level = g_frc_memc.frc_memc_level;

    return HI_SUCCESS;
}
hi_s32 hi_drv_custom_set_m5_memc_level(hi_u32 memc_level)
{
    hi_s32 ret = HI_SUCCESS;

    hi_u8 m5_memc_level[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = { 0x92, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00 };

    if (memc_level <= 2) { /* 0 1 2 means memc motion smooth level */
        m5_memc_level[2] = (hi_u8)memc_level; /* 2 items in m5_memc_level */
        hi_drv_custom_cal_i2c_cmd_check_sum(m5_memc_level, HI_BOARD_PANEL_I2C_DATA_LEN + 1);

        ret = custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
            m5_memc_level[0], 1, (hi_u8 *)(&m5_memc_level[1]), HI_BOARD_PANEL_I2C_DATA_LEN);
        if (ret == HI_SUCCESS) {
            g_frc_memc.frc_memc_level = memc_level;
            return HI_SUCCESS;
        }
    }

    return HI_FAILURE;
}
hi_s32 hi_drv_custom_get_m5_memc_demo_mode(hi_u32 *memc_demo_mode)
{
    hi_s32 ret = HI_SUCCESS;
    if (memc_demo_mode == HI_NULL) {
        custom_log("get M5 memc demo mode enable failed, param point is NULL!\n");
        return HI_FAILURE;
    }
    *memc_demo_mode = g_frc_memc.frc_memc_demo_mode;
    return ret;
}

hi_s32 hi_drv_custom_set_m5_memc_demo_mode(hi_u32 memc_demo_mode)
{
    hi_s32 ret = HI_SUCCESS;
    // {off}, {right demo}, {left demo}
    hi_u8 m5_memc_demo_mode[][HI_BOARD_PANEL_I2C_DATA_LEN + 1] = {
        { 0x32, 0x1f, 0x04, 0x00, 0x00, 0x00, 0x00 },
        { 0x42, 0x1f, 0x04, 0x01, 0x00, 0x00, 0x00 },
        { 0x52, 0x1f, 0x04, 0x01, 0x01, 0x00, 0x00 }
    };

    if (memc_demo_mode <= 2) { /* 2 means mid , 1 means low ,0 means off */
        ret = custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
            m5_memc_demo_mode[memc_demo_mode][0], 1,
            (hi_u8 *)(&m5_memc_demo_mode[memc_demo_mode][1]), HI_BOARD_PANEL_I2C_DATA_LEN);
        if (ret == HI_SUCCESS) {
            g_frc_memc.frc_memc_demo_mode = memc_demo_mode;
        }
    } else {
        return HI_FAILURE;
    }

    return ret;
}

hi_s32 hi_drv_custom_get_m5_osd_protection(hi_bool *is_protect, hi_u32 *osd_part,
    hi_u32 *xpos, hi_u32 *ypos, hi_u32 *wpos, hi_u32 *hpos)
{
    hi_s32 ret = HI_SUCCESS;
    if ((is_protect == HI_NULL)
        || (osd_part == HI_NULL)
        || (xpos == HI_NULL)
        || (ypos == HI_NULL)
        || (wpos == HI_NULL)
        || (hpos == HI_NULL)) {
        custom_log("get M5 osd protection failed, param point is NULL!\n");
        return HI_FAILURE;
    }
    *is_protect = g_frc_memc.st_frc_osd_protection.is_protect;
    *hpos = g_frc_memc.st_frc_osd_protection.hpos;
    *osd_part = g_frc_memc.st_frc_osd_protection.osd_part;
    *wpos = g_frc_memc.st_frc_osd_protection.wpos;
    *xpos = g_frc_memc.st_frc_osd_protection.xpos;
    *ypos = g_frc_memc.st_frc_osd_protection.ypos;
    return ret;
}

static hi_s32 custom_set_m5_osd_protection_check(hi_u8 region[], hi_u32 region_size,
    hi_u8 xy_pos[], hi_u32 xy_pos_size,
    hi_u8 wh_pos[], hi_u32 wh_pos_size)
{
    hi_s32 ret;

    hi_drv_custom_cal_i2c_cmd_check_sum(region, region_size);
    hi_drv_custom_cal_i2c_cmd_check_sum(xy_pos, xy_pos_size);
    hi_drv_custom_cal_i2c_cmd_check_sum(wh_pos, wh_pos_size);

    custom_check_func(ret,
                        custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
                        region[0], 1, (hi_u8 *)(&region[1]),
                        HI_BOARD_PANEL_I2C_DATA_LEN));

    custom_check_func(ret,
                        custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
                        xy_pos[0], 1, (hi_u8 *)(&xy_pos[1]),
                        HI_BOARD_PANEL_I2C_DATA_LEN));

    custom_check_func(ret,
                        custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
                        wh_pos[0], 1, (hi_u8 *)(&wh_pos[1]),
                        HI_BOARD_PANEL_I2C_DATA_LEN));
    return;
}

hi_s32 hi_drv_custom_set_m5_osd_protection(hi_bool is_protect, hi_u32 osd_part,
    hi_u32 xpos, hi_u32 ypos, hi_u32 wpos, hi_u32 hpos)
{
    hi_s32 ret;
    hi_u8 m5_osd_protect_region[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = { 0xb1, 0x1a, 0x00, 0x01, 0x00, 0x00, 0x00 };
    hi_u8 m5_osd_protect_xy_pos[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = { 0x81, 0x1b, 0x05, 0x02, 0x3b, 0x01, 0x23 };
    hi_u8 m5_osd_protect_wh_pos[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = { 0x51, 0x1c, 0x05, 0x00, 0x0f, 0x00, 0x25 };

    if (osd_part > M5_OSD_REGION_NUM_MUX || xpos > M5_OSD_REGION_POS_MUX
        || ypos > M5_OSD_REGION_POS_MUX || wpos > M5_OSD_REGION_POS_MUX
        || hpos > M5_OSD_REGION_POS_MUX) {
        return HI_SUCCESS;
    }

    if (is_protect != HI_TRUE && is_protect != HI_FALSE) {
        return HI_SUCCESS;
    }

    /* 1. set m5_osd_protect_region or not, set which region */
    if (is_protect == HI_FALSE) {
        m5_osd_protect_region[3] = 0x00; /* 3 items in m5_osd_protect_region */
    }
    m5_osd_protect_region[2] = (hi_u8)osd_part;               /* 2 items in m5_osd_protect_region */
    /* 2. set m5_osd_protect_region x_ypos */
    m5_osd_protect_xy_pos[2] = (hi_u8)osd_part;               /* 2 items in m5_osd_protect_xy_pos */
    m5_osd_protect_xy_pos[3] = (hi_u8)((xpos >> 8) & 0xff);   /* xpos high 8bit store to  m5_osd_protect_xy_pos[3] */
    m5_osd_protect_xy_pos[4] = (hi_u8)(xpos & 0xff);          /* 4 items in m5_osd_protect_xy_pos */
    m5_osd_protect_xy_pos[5] = (hi_u8)((ypos >> 8) & 0xff);   /* ypos high 8bit store to  m5_osd_protect_xy_pos[5] */
    m5_osd_protect_xy_pos[6] = (hi_u8)(ypos & 0xff);          /* 6 items in m5_osd_protect_xy_pos */
    /* 3. set m5_osd_protect_region w_hpos */
    m5_osd_protect_wh_pos[2] = (hi_u8)osd_part;                /* 2 items in m5_osd_protect_wh_pos */
    m5_osd_protect_wh_pos[3] = (hi_u8)((wpos >> 8) & 0xff);   /* wpos high 8bit store to m5_osd_protect_wh_pos[3] */
    m5_osd_protect_wh_pos[4] = (hi_u8)(wpos & 0xff);          /* 4 items in m5_osd_protect_wh_pos */
    m5_osd_protect_wh_pos[5] = (hi_u8)((hpos >> 8) & 0xff);   /* hpos high 8bit store to m5_osd_protect_wh_pos[5] */
    m5_osd_protect_wh_pos[6] = (hi_u8)(hpos & 0xff);          /* 6 items in m5_osd_protect_wh_pos */

    ret = custom_set_m5_osd_protection_check(m5_osd_protect_region, HI_BOARD_PANEL_I2C_DATA_LEN + 1,
        m5_osd_protect_xy_pos, HI_BOARD_PANEL_I2C_DATA_LEN + 1,
        m5_osd_protect_wh_pos, HI_BOARD_PANEL_I2C_DATA_LEN + 1);
    if (ret == HI_SUCCESS) {
        g_frc_memc.st_frc_osd_protection.is_protect = is_protect;
        g_frc_memc.st_frc_osd_protection.hpos = hpos;
        g_frc_memc.st_frc_osd_protection.osd_part = osd_part;
        g_frc_memc.st_frc_osd_protection.wpos = wpos;
        g_frc_memc.st_frc_osd_protection.xpos = xpos;
        g_frc_memc.st_frc_osd_protection.ypos = ypos;
    } else {
        return HI_FAILURE;
    }
    return ret;
}
hi_s32 hi_drv_custom_get_m5_memc_mode_enable(hi_bool *memc_mode_enable)
{
    hi_s32 ret = HI_SUCCESS;
    if (memc_mode_enable == HI_NULL) {
        custom_log("get M5 memc mode enable failed, param point is NULL!\n");
        return HI_FAILURE;
    }
    *memc_mode_enable = g_frc_memc.frc_memc_mode_enable;
    return ret;
}

hi_s32 hi_drv_custom_set_m5_memc_mode_enable(hi_bool memc_mode_enable)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u8 m5_memc_mode_enable_data[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = { 0xd1, 0x1d, 0x00, 0x00, 0x00, 0x00, 0x00 };
    hi_u8 m5_memc_mode_disable_data[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = { 0xe1, 0x1d, 0x01, 0x00, 0x00, 0x00, 0x00 };

    if (memc_mode_enable) {
        ret = custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
            m5_memc_mode_enable_data[0], 1,
            (hi_u8 *)(&m5_memc_mode_enable_data[1]), HI_BOARD_PANEL_I2C_DATA_LEN);
        if (ret == HI_SUCCESS) {
            g_frc_memc.frc_memc_mode_enable = HI_TRUE;
        }
    } else {
        ret = custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
            m5_memc_mode_disable_data[0], 1,
            (hi_u8 *)(&m5_memc_mode_disable_data[1]), HI_BOARD_PANEL_I2C_DATA_LEN);
        if (ret == HI_SUCCESS) {
            g_frc_memc.frc_memc_mode_enable = HI_FALSE;
        }
    }

    return ret;
}

hi_s32 hi_drv_custom_get_m5_video_time_delay_ms(hi_u32 *video_time_delay_ms)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u8 tmp_video_time_delay_ms = 0;
    hi_u8 m5_video_time_delay_ms_data[HI_BOARD_PANEL_I2C_DATA_LEN + 1] = { 0xd1, 0x2d, 0x00, 0x00, 0x00, 0x00, 0x00 };

    custom_check_func(ret,
                      custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
                                     m5_video_time_delay_ms_data[0], 1, (hi_u8 *)(&m5_video_time_delay_ms_data[1]),
                                     HI_BOARD_PANEL_I2C_DATA_LEN));

    mdelay(30); /* mdelay 30 ms */
    custom_check_func(ret,
                      custom_read_i2c(HI_BOARD_PANEL_I2C_CHN, 0x39, 0, 0, &tmp_video_time_delay_ms, 1));

    *video_time_delay_ms = tmp_video_time_delay_ms;

    return ret;
}
#endif
/*******************************************************************
                                        I2C code
********************************************************************/
static hi_s32 drv_custom_send_cmo_4k_i2c(hi_void)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 u32i = 0;
    hi_u8 panel_data[][HI_BOARD_PANEL_I2C_DATA_LEN + 1] = {
        { 0x20, 0x01, 0x12, 0x00, 0x40, 0xFF, 0x00 },
        { 0xC0, 0x02, 0x00, 0x20, 0x31, 0x64, 0x75 },
        { 0x61, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00 },
        { 0x23, 0x1B, 0x10, 0x27, 0xC2, 0xEE, 0xD0 },
        { 0x33, 0x1B, 0x10, 0x27, 0x40, 0x71, 0xD0 },
        { 0x33, 0x1B, 0x10, 0x27, 0xC3, 0xEE, 0xD0 },
        { 0xC1, 0x05, 0x0F, 0x00, 0x08, 0x70, 0x00 },
        { 0x13, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00 },
        { 0x33, 0x1C, 0x13, 0x00, 0x04, 0x40, 0x40 },
        { 0x3D, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00 },
        { 0x33, 0x1C, 0x13, 0x00, 0x04, 0x00, 0x40 },
        { 0x9D, 0x0A, 0x40, 0xFF, 0x00, 0x00, 0x00 },
        { 0xAD, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00 },
        { 0xfd, 0x0d, 0x02, 0x00, 0x00, 0x00, 0x00 }
    };

    for (u32i = 0; u32i < sizeof(panel_data) / sizeof(panel_data[0]); u32i++) {
        ret = custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, HI_BOARD_PANEL_I2C_DEV_ADDR,
            panel_data[u32i][0], 1, (hi_u8 *)(&panel_data[u32i][1]),
            HI_BOARD_PANEL_I2C_DATA_LEN);
        if (ret != HI_SUCCESS) {
            hi_err_print_call_fun_err(drv_custom_send_cmo_4k_i2c, HI_FAILURE);
            hi_err_print_u32(u32i);
        }

        custom_ms_delay(10);
    }

    return ret;
}

hi_s32 drv_custom_send_panel_info(hi_drv_panel_advance_info *advance_info)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 cmd_index = 0;
    hi_u8 *i2c_data = advance_info->i2c_data;

    for (cmd_index = 0; cmd_index < advance_info->i2c_cmd_num; cmd_index++) {
        ret = custom_write_i2c(HI_BOARD_PANEL_I2C_CHN, *i2c_data,
            *(i2c_data + 1), 1, (i2c_data + 3), *(i2c_data + 2)); /* 3 is data offset, 2 is len offset */
        if (ret != HI_SUCCESS) {
            hi_err_print_call_fun_err(drv_custom_send_panel_info, HI_FAILURE);
            hi_err_print_u32(cmd_index);
        }

        /* every cmd contains as max as 12 data */
        i2c_data += EVERY_I2C_CMD_12_DATAS;
        custom_ms_delay(advance_info->i2c_iterval_delay);
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_custom_send_panel_i2c(hi_u32 panel_index, hi_drv_panel_advance_info *advance_info)
{
    hi_s32 ret = HI_SUCCESS;

    // :TODO:// while hitool modify, if condition code must update
    if ((advance_info->i2c_cmd_num == 0) || (advance_info->i2c_cmd_len < 3)) { /* 0 is i2c_dev_addr , 3 is data_ptr */
        // :TODO:// if need send i2c, add this
        switch (panel_index) {
                /* i2c test case */;
            case CUSTOM_PANEL_INDEX_CMO_3840x2160_60HZ_2D:
                ret = drv_custom_send_cmo_4k_i2c();
                break;
            default:
                break;
        }
    } else {
        ret = drv_custom_send_panel_info(advance_info);
    }

    return ret;
}

/*******************************************************************
                                        GPIO code
********************************************************************/
static hi_s32 drv_custom_pull_lg_gpio(hi_void)
{
    hi_s32 ret = HI_SUCCESS;
#if HI_BOARD_PANEL_LG_PULL_GPIO
    gpio_ext_func *gpio_func = HI_NULL;

    custom_check_func_ret (ret, osal_exportfunc_get(HI_ID_GPIO, (hi_void **)(&gpio_func)));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    custom_check_pointer_ret(gpio_func->gpio_dir_set_bit);
    if (gpio_func->gpio_dir_set_bit == NULL) {
        return HI_FAILURE;
    }

    custom_check_pointer_ret(gpio_func->gpio_write_bit);
    if (gpio_func->gpio_write_bit == NULL) {
        return HI_FAILURE;
    }

    /* set gplus_en pin */
    custom_check_func_ret(ret, gpio_func->gpio_dir_set_bit(HI_BOARD_PANEL_3DMODE_GPIONUM, HI_BOARD_GPIO_OUT));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    custom_check_func_ret(ret, gpio_func->gpio_write_bit(HI_BOARD_PANEL_3DMODE_GPIONUM,
        HI_BOARD_PANEL_3DMODE_ON));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    /* set Data format pin: 2 devision */
    custom_check_func_ret(ret, gpio_func->gpio_dir_set_bit(HI_BOARD_PANEL_SYNC_3D_IN_GPIONUM, HI_BOARD_GPIO_OUT));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    custom_check_func_ret(ret, gpio_func->gpio_write_bit(HI_BOARD_PANEL_SYNC_3D_IN_GPIONUM,
        HI_BOARD_PANEL_SYNC_3D_IN_ON));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
#endif
    return ret;
}

/* if have some special GPIO need to pull, add it */
hi_s32 hi_drv_custom_pull_panel_gpio(hi_u32 panel_index)
{
    // :TODO:// if need pull gpio, add this
    switch (panel_index) {
        /* custom pull gpio test */
        case CUSTOM_PANEL_INDEX_LG_3840x2160_60HZ_2D:
            return drv_custom_pull_lg_gpio();
        default:
            break;
    }

    return HI_SUCCESS;
}

#ifdef HI_BOARD_PDM_SCENE_USE_GPIO
/* for panel/pq/vo to query tcon scene */
hi_s32 hi_drv_custom_get_tcon_scene(custom_pdm_scene *tcon_scene)
{
    hi_s32 ret;

    if (tcon_scene == NULL) {
        HI_PRINT("invalid parameter\n");
        return HI_FAILURE;
    }

    *tcon_scene = CUSTOM_PDM_SCENE_BUTT;

    hi_u32 value = 0;
    gpio_ext_func *gpio_func = HI_NULL;
    custom_check_func_ret (ret, osal_exportfunc_get(HI_ID_GPIO, (hi_void **)(&gpio_func)));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = gpio_func->pfn_gpio_dir_set_bit(HI_BOARD_PDM_SCENE_GPIONUM, HI_BOARD_GPIO_INPUT);
    if (ret != HI_SUCCESS) {
        HI_PRINT("call gpio_dir_set_bit fail\n");
        return HI_FAILURE;
    }

    ret = gpio_func->pfn_gpio_read_bit(HI_BOARD_PDM_SCENE_GPIONUM, &value);
    if (ret != HI_SUCCESS) {
        HI_PRINT("call gpio_read_bit fail , read value error\n");
        return HI_FAILURE;
    }

    if (value == HI_BOARD_PDM_SCENE_TCON) {
        *tcon_scene = CUSTOM_PDM_SCENE_TCON;
    } else {
        *tcon_scene = CUSTOM_PDM_SCENE_PANEL;
    }
    return HI_SUCCESS;
}
#else
hi_s32 hi_drv_custom_get_tcon_scene(custom_pdm_scene *tcon_scene)
{
    hi_s32 ret;
    hi_u32 tcon_index = 0;
    hi_u32 panel_total_index = 0;
    hi_u32 panel_index_cur = 0;
    pdm_export_func *pdm_func = HI_NULL;

    if (tcon_scene == NULL) {
        HI_PRINT("invalid parameter\n");
        return HI_FAILURE;
    }

    *tcon_scene = CUSTOM_PDM_SCENE_BUTT;

    /* get the pointer of PDM's functions */
    custom_check_func_ret (ret, osal_exportfunc_get(HI_ID_PDM, (hi_void **)(&pdm_func)));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    custom_check_pointer_ret(pdm_func->pdm_get_tcon_index_ptr);
    if (pdm_func->pdm_get_tcon_index_ptr == NULL) {
        return HI_FAILURE;
    }
    /* get panel index */
    custom_check_func_ret(ret, pdm_func->pdm_get_tcon_index_ptr(&tcon_index));
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = pdm_func->pdm_get_panel_index_ptr(&panel_total_index, &panel_index_cur);
    if (ret != HI_SUCCESS) {
        HI_PRINT("call hi_drv_pdm_get_panel_index fail , read value error\n");
        return HI_FAILURE;
    }

    if (tcon_index != CUSTOM_PDM_INVALID_NUM
        && panel_index_cur == CUSTOM_PDM_INVALID_NUM) {
        *tcon_scene = CUSTOM_PDM_SCENE_TCON;
    } else if (tcon_index == CUSTOM_PDM_INVALID_NUM
               && panel_index_cur != CUSTOM_PDM_INVALID_NUM
               && panel_index_cur < panel_total_index) {
        *tcon_scene = CUSTOM_PDM_SCENE_PANEL;
    } else if (tcon_index != CUSTOM_PDM_INVALID_NUM
               && panel_index_cur != CUSTOM_PDM_INVALID_NUM
               && panel_index_cur < panel_total_index) {
        *tcon_scene = CUSTOM_PDM_SCENE_DOUBLE;
    }

    return HI_SUCCESS;
}
#endif

hi_s32 hi_drv_custom_get_parse_param_mode(hi_drv_panel_parse_param *parse_mode)
{
    hi_s32 ret;
    custom_pdm_scene tcon_scene;

    if (parse_mode == HI_NULL) {
        custom_log("get parse param mode failed, param point is NULL!");
        return HI_FAILURE;
    }

    ret = hi_drv_custom_get_tcon_scene(&tcon_scene);
    if (ret != HI_SUCCESS) {
        custom_log("call get tcon_scene function failed!!");
        return HI_FAILURE;
    }

    switch (tcon_scene) {
        case CUSTOM_PDM_SCENE_TCON:
            *parse_mode = HI_DRV_PANEL_PARSE_PARAM_TCONBIN;
            break;

        case CUSTOM_PDM_SCENE_PANEL:
            *parse_mode = HI_DRV_PANEL_PARSE_PARAM_PANELIMG;
            break;

        case CUSTOM_PDM_SCENE_DOUBLE:
            *parse_mode = HI_DRV_PANEL_PARSE_PARAM_ALL;
            break;

        default:
            custom_log("call get tcon_scene function failed!!");
            return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_custom_set_tcon_pin_default_status(hi_void)
{
    hi_u32 i = 0;
    hi_s32 ret;
    gpio_ext_func *gpio_func = HI_NULL;

    hi_u32 tcon_gpio[][3] = {            /* 3 itemsin tcon_gpio */
        /* GPIO_GRP, GPIO_BIT, GPIO_H/L */
        { 0, 3, HI_BOARD_GPIO_LOW },
        { 1, 4, HI_BOARD_GPIO_LOW },
        { 1, 5, HI_BOARD_GPIO_LOW },
        { 1, 6, HI_BOARD_GPIO_LOW },
    };

    ret = osal_exportfunc_get(HI_ID_GPIO, (hi_void **)(&gpio_func));

    for (i = 0; i < sizeof(tcon_gpio) / sizeof(tcon_gpio[0]); i++) {
        if ((HI_NULL != gpio_func)
            && (HI_NULL != gpio_func->pfn_gpio_dir_set_bit)
            && (HI_NULL != gpio_func->pfn_gpio_write_bit)) {
            ret = gpio_func->pfn_gpio_dir_set_bit(((tcon_gpio[i][0] * 8) +  /* group num * 8 */
             tcon_gpio[i][1]), HI_BOARD_GPIO_OUT);
            ret |= gpio_func->pfn_gpio_write_bit(((tcon_gpio[i][0] * 8) +  /* group num * 8 */
             tcon_gpio[i][1]), tcon_gpio[i][2]); /* tcon_gpio[i][2] is bit value */
        }
    }

    return ret;
}

osal_task *g_resume_thread = NULL;

static custom_ext_func g_custom_export_funcs = {
#if !USE_MX_FUNCTION
    .set_panel_mode = hi_drv_custom_set_panel_mode,
    .send_panel_i2c = hi_drv_custom_send_panel_i2c,
    .pull_panel_gpio = hi_drv_custom_pull_panel_gpio,
    .get_parse_param_mode = hi_drv_custom_get_parse_param_mode,
    .set_frc_panel_power = HI_NULL,
    .set_frc_ldm_strength_level = HI_NULL,
    .set_frc_local_dimming_demo_mode = HI_NULL,
    .set_frc_memc_level = HI_NULL,
    .set_frc_memc_demo_mode = HI_NULL,
    .set_frc_osd_protection = HI_NULL,
    .set_frc_memc_mode_enable = HI_NULL,
    .get_frc_video_time_delay_ms = HI_NULL,
    .get_frc_memc_level = HI_NULL,
    .get_frc_memc_demo_mode = HI_NULL,
    .get_frc_osd_protection = HI_NULL,
    .get_frc_memc_mode_enable = HI_NULL,
    .set_tcon_pin_default_status = hi_drv_custom_set_tcon_pin_default_status,
#else
    .set_panel_mode = hi_drv_custom_set_panel_mode,
    .send_panel_i2c = hi_drv_custom_send_panel_i2c,
    .pull_panel_gpio = hi_drv_custom_pull_panel_gpio,
    .get_parse_param_mode = hi_drv_custom_get_parse_param_mode,
    .set_frc_panel_power = hi_drv_custom_set_m5_panel_power,
    .set_frc_ldm_strength_level = hi_drv_custom_set_m5_ldm_strength_level,
    .set_frc_local_dimming_demo_mode = hi_drv_custom_set_m5_local_dimming_demo_mode,
    .set_frc_memc_level = hi_drv_custom_set_m5_memc_level,
    .set_frc_memc_demo_mode = hi_drv_custom_set_m5_memc_demo_mode,
    .set_frc_osd_protection = hi_drv_custom_set_m5_osd_protection,
    .set_frc_memc_mode_enable = hi_drv_custom_set_m5_memc_mode_enable,
    .get_frc_video_time_delay_ms = hi_drv_custom_get_m5_video_time_delay_ms,
    .get_frc_memc_level = hi_drv_custom_get_m5_memc_level,
    .get_frc_memc_demo_mode = hi_drv_custom_get_m5_memc_demo_mode,
    .get_frc_osd_protection = hi_drv_custom_get_m5_osd_protection,
    .get_frc_memc_mode_enable = hi_drv_custom_get_m5_memc_mode_enable,
    .set_tcon_pin_default_status = hi_drv_custom_set_tcon_pin_default_status,
#endif
};

hi_s32 custom_drv_suspend(hi_void *private_data)
{
    if (g_resume_thread != NULL) {
        osal_kthread_destroy(g_resume_thread, 0);
        g_resume_thread = NULL;
    }

    return HI_SUCCESS;
}

hi_s32 custom_drv_resume(hi_void *private_data)
{
    g_resume_thread = osal_kthread_create(custom_reset_usb3_dev, NULL, "reset USB3 task", 0);
    return HI_SUCCESS;
}

hi_s32 custom_drv_lowpower_enter(hi_void *private_data)
{
    HI_LOG_NOTICE("custom lowpower enter OK\n");
    return HI_SUCCESS;
}

hi_s32 custom_drv_lowpower_exit(hi_void *private_data)
{
    HI_LOG_NOTICE("custom lowpower exit OK\n");
    return HI_SUCCESS;
}

static osal_pmops g_custom_pmops = {
    .pm_suspend = custom_drv_suspend,
    .pm_resume = custom_drv_resume,
    .pm_lowpower_enter = custom_drv_lowpower_enter,
    .pm_lowpower_exit = custom_drv_lowpower_exit,
    .pm_poweroff = NULL,
    .private_data = NULL,
};

static osal_dev g_custom_device = {
    .name  = UMAP_DEVNAME_CUSTOM,
    .minor = UMAP_MIN_MINOR_CUSTOM,
    .fops = NULL,
    .pmops = &g_custom_pmops,
};

hi_s32 custom_drv_proc_read(hi_void *s, hi_void *private)
{
    osal_proc_print(s, "\n------------------board info------------------\n");
    osal_proc_print(s, "board name : %s\n", HI_BOARD_NAME);

    return HI_SUCCESS;
}

hi_s32 __init custom_drv_module_init(hi_void)
{
    hi_s32 ret;
    osal_proc_entry *proc_entry = HI_NULL;

    ret = osal_dev_register(&g_custom_device);
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("add custom device failed.\n");
        return HI_FAILURE;
    }

    proc_entry = osal_proc_add(HI_MOD_CUSTOM, strlen(HI_MOD_CUSTOM));
    if (proc_entry == NULL) {
        HI_LOG_FATAL("add custom proc failed.\n");
        osal_dev_unregister(&g_custom_device);
        return HI_FAILURE;
    }

    proc_entry->cmd_cnt = 0;
    proc_entry->cmd_list = NULL;
    proc_entry->read = custom_drv_proc_read;

    ret = osal_exportfunc_register(HI_ID_CUSTOM, "HI_CUSTOM", (hi_void *)(&g_custom_export_funcs));
    if (ret != HI_SUCCESS) {
        HI_LOG_FATAL("register custom exportfun failed.\n");
        osal_proc_remove("custom", strlen("custom"));
        osal_dev_unregister(&g_custom_device);
        return HI_FAILURE;
    }

#ifdef MODULE
    HI_PRINT("load hi_custom.ko success.\t\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;
}

hi_void __exit custom_drv_module_exit(hi_void)
{
    osal_exportfunc_unregister(HI_ID_CUSTOM);
    osal_proc_remove(HI_MOD_CUSTOM, strlen(HI_MOD_CUSTOM));
    osal_dev_unregister(&g_custom_device);

#ifdef MODULE
    HI_PRINT("remove hi_custom.ko success.\n");
#endif
}

#ifdef MODULE
module_init(custom_drv_module_init);
module_exit(custom_drv_module_exit);
#endif

MODULE_LICENSE("GPL");
