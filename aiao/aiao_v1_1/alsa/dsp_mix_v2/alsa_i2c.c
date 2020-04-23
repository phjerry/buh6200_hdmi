/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: aiao alsa driver
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 Initial version for Hi3796CV300
 */

#include "hi_osal.h"

#include "hi_drv_audio.h"
#include "drv_gpioi2c_ext.h"
#include "drv_i2c_ext.h"
#include "hi_drv_i2c.h"

#include "alsa_i2c.h"
#include "hi_reg_common.h"

/* #define DEBUG_GPIO_I2C */
#ifdef DEBUG_GPIO_I2C
#define TRC(x...) print(KERN_ALERT x)
#else
#define TRC(x...)
#endif
#define TRP() TRC("%s called\n", __func__)

/* HI_ALSA_TLV320_SUPPORT */
#define SDK_STB_DEMO_GPIO_SETTING 1
#define SDK_TC_DEMO_GPIO_SETTING  0

#define AIC31_I2C_ADDRESS 0x18

#ifdef SDK_STB_DEMO_GPIO_SETTING
#if defined(CHIP_TYPE_HI3716CV200)   || \
    defined(CHIP_TYPE_HI3716MV400)   || \
    defined(CHIP_TYPE_HI3718CV100)   || \
    defined(CHIP_TYPE_HI3719CV100)   || \
    defined(CHIP_TYPE_HI3716CV200ES)
#define SCL_GPIO_GNUM_S40 2
#define SCL_GPIO_PNUM_S40 1

#define SDA_GPIO_GNUM_S40 2
#define SDA_GPIO_PNUM_S40 2
#elif defined(CHIP_TYPE_HI3718MV100)   || \
    defined(CHIP_TYPE_HI3719MV100)     || \
    defined(CHIP_TYPE_HI3719MV100_A)
#define SCL_GPIO_GNUM_S40 3
#define SCL_GPIO_PNUM_S40 4

#define SDA_GPIO_GNUM_S40 3
#define SDA_GPIO_PNUM_S40 5
#endif
#define SDA               (((SDA_GPIO_GNUM_S40)*8) + (SDA_GPIO_PNUM_S40))
#define SCL               (((SCL_GPIO_GNUM_S40)*8) + (SCL_GPIO_PNUM_S40))
#if defined(CHIP_TYPE_HI3716CV200)    || \
    defined(CHIP_TYPE_HI3716MV400)    || \
    defined(CHIP_TYPE_HI3718CV100)    || \
    defined(CHIP_TYPE_HI3719CV100)    || \
    defined(CHIP_TYPE_HI3719MV100_A)  || \
    defined(CHIP_TYPE_HI3716CV200ES)
#define SLAC_RESET_GPIO_GNUM_S40 2
#define SLAC_RESET_GPIO_PNUM_S40 4
#elif defined(CHIP_TYPE_HI3718MV100) || defined(CHIP_TYPE_HI3719MV100)
#define SLAC_RESET_GPIO_GNUM_S40 3
#define SLAC_RESET_GPIO_PNUM_S40 7
#endif

#elif SDK_TC_DEMO_GPIO_SETTING
#define MUTE_GPIO_GNUM_S40       5
#define MUTE_GPIO_PNUM_S40       7
#define SLAC_RESET_GPIO_GNUM_S40 2
#define SLAC_RESET_GPIO_PNUM_S40 1

#define SDA_GPIO_GNUM_S40 2
#define SDA_GPIO_PNUM_S40 3
#define SCL_GPIO_GNUM_S40 2
#define SCL_GPIO_PNUM_S40 4
#define SDA               (((SDA_GPIO_GNUM_S40)*8) + (SDA_GPIO_PNUM_S40))
#define SCL               (((SCL_GPIO_GNUM_S40)*8) + (SCL_GPIO_PNUM_S40))
#else
#define SLAC_RESET_GPIO_GNUM_S40 2
#define SLAC_RESET_GPIO_PNUM_S40 4
#define SDA_GPIO_GNUM_S40        10
#define SDA_GPIO_PNUM_S40        4
#define SCL_GPIO_GNUM_S40        10
#define SCL_GPIO_PNUM_S40        3
#define SDA               (((SDA_GPIO_GNUM_S40)*8) + (SDA_GPIO_PNUM_S40))
#define SCL               (((SCL_GPIO_GNUM_S40)*8) + (SCL_GPIO_PNUM_S40))
#endif

#define hi_gpio_dir_out(x) \
    hi_drv_gpio_set_dir_bit(x, HI_FALSE)

#define hi_gpio_dir_in(x) \
    hi_drv_gpio_set_dir_bit(x, HI_TRUE)

#define hi_gpio_set_val(x, y) \
    hi_drv_gpio_write_bit(x, y)

#define hi_gpio_get_val(x, y) \
    hi_drv_gpio_read_bit(x, y)

static struct i2c_adapter *adapter = HI_NULL;

static inline hi_void i2c_pin_mf_config(hi_void)
{
    /* to_do: need IO_CONFIG for this GPIO pin */
#if SDK_TC_DEMO_GPIO_SETTING
    hi_u32 reg_reset;
#endif
    TRP();
#if SDK_STB_DEMO_GPIO_SETTING
#if defined(CHIP_TYPE_HI3719MV100) || defined(CHIP_TYPE_HI3718MV100) || defined(CHIP_TYPE_HI3719MV100_A)
    g_reg_io->ioshare_reg39.u32 = 0x0;  /* set tlv320 reset pin */
    g_reg_io->ioshare_reg36.u32 = 0x0;  /* set I2C1_SCL pin */
    g_reg_io->ioshare_reg37.u32 = 0x5;  /* set I2C1_SDA pin */

#else
    g_reg_io->ioshare_reg20.u32 = 0x0;  /* set tlv320 reset pin */
    g_reg_io->ioshare_reg17.u32 = 0x0;  /* set I2C2_SCL pin */
    g_reg_io->ioshare_reg18.u32 = 0x0;  /* set I2C2_SDA pin */

#endif
#elif SDK_TC_DEMO_GPIO_SETTING
    g_reg_io->ioshare_reg19.u32 = 0x0;  /* set I2C2_SCL pin */
    g_reg_io->ioshare_reg20.u32 = 0x0;  /* set I2C2_SDA pin */
    g_reg_sys_ctrl->SC_IO_REUSE_SEL.bits.key0_gpio_sel = 0x0;

    reg_reset = g_reg_io->ioshare_reg17.u32;
    g_reg_io->ioshare_reg17.u32 = (reg_reset) & (~0x7);  /* set tlv320 reset pin */

    hi_drv_gpio_set_dir_bit(MUTE_GPIO_GNUM_S40 * 8 + MUTE_GPIO_PNUM_S40, 0);  /* mute control */
    hi_drv_gpio_write_bit(MUTE_GPIO_GNUM_S40 * 8 + MUTE_GPIO_PNUM_S40, 0);
#else
    g_reg_io->ioshare_reg86.u32 = 0x0;  /* set I2C2_SCL pin */
    g_reg_io->ioshare_reg87.u32 = 0x0;  /* set I2C2_SDA pin */

    reg_reset = g_reg_io->ioshare_reg20.u32;
    g_reg_io->ioshare_reg17.u32 = (reg_reset) & (~0x7);  /* set tlv320 reset pin */
#endif
}

static inline hi_void i2c_pin_init(hi_void)
{
    /* this must be compatible with I2C STOP condition.
     * but, hi_gpio_write_bit() requires direction config first.
     * so... this could conflicts with I2C bus spec.
     */
    TRP();
    hi_gpio_dir_out(SDA);
    hi_gpio_dir_out(SCL);
    hi_gpio_set_val(SDA, HI_TRUE);
    hi_gpio_set_val(SCL, HI_TRUE);
}

static inline hi_void i2c_pin_release(hi_void)
{
    TRP();
    hi_gpio_dir_in(SDA);
    hi_gpio_dir_in(SCL);
}

/* prevent typo */
int hi_gpio_i2c_bit_get_sda(hi_void *data)
{
    int val;

    hi_gpio_dir_in(SDA);
    hi_gpio_get_val(SDA, &val);
    return val;
}

hi_void hi_gpio_i2c_bit_set_sda(hi_void *data, int state)
{
    hi_gpio_dir_out(SDA);
    if (state) {
        hi_gpio_set_val(SDA, HI_TRUE);
    } else {
        hi_gpio_set_val(SDA, HI_FALSE);
    }
}

int hi_gpio_i2c_bit_get_scl(hi_void *data)
{
    int val;

    hi_gpio_dir_in(SCL);
    hi_gpio_get_val(SCL, &val);
    return val;
}

hi_void hi_gpio_i2c_bit_set_scl(hi_void *data, int state)
{
    hi_gpio_dir_out(SCL);
    if (state) {
        hi_gpio_set_val(SCL, HI_TRUE);
    } else {
        hi_gpio_set_val(SCL, HI_FALSE);
    }
}

hi_void i2c_pins_init(hi_void)
{
    i2c_pin_mf_config();
    i2c_pin_init();
}

int init_i2c(hi_void)
{
    int err;
    struct i2c_client *client = HI_NULL;
    struct i2c_board_info info;
    struct i2c_algo_bit_data *bit_data = HI_NULL;

    TRP();
    i2c_pin_mf_config();
    i2c_pin_init();

    adapter = HI_KZALLOC(HI_ID_AO, sizeof(struct i2c_adapter), OSAL_GFP_KERNEL);
    if (adapter == HI_NULL) {
        return -ENOMEM;
    }

    bit_data = HI_KZALLOC(HI_ID_AO, sizeof(struct i2c_algo_bit_data), OSAL_GFP_KERNEL);
    if (bit_data == HI_NULL) {
        osal_kfree(HI_ID_AO, adapter);
        adapter = HI_NULL;
        return -ENOMEM;
    }

    bit_data->setsda = hi_gpio_i2c_bit_set_sda;
    bit_data->getsda = hi_gpio_i2c_bit_get_sda;
    bit_data->setscl = hi_gpio_i2c_bit_set_scl;
    bit_data->getscl = hi_gpio_i2c_bit_get_scl;
    bit_data->udelay = 10; /* 100 k_hz */
    bit_data->timeout = msecs_to_jiffies(100);
    bit_data->data = adapter;

    snprintf(adapter->name, sizeof(adapter->name), "tlv320");
    adapter->owner = THIS_MODULE;
    adapter->class = I2C_CLASS_HWMON;
    adapter->algo_data = bit_data;

    err = i2c_bit_add_bus(adapter);
    if (err) {
        osal_kfree(HI_ID_AO, bit_data);
        osal_kfree(HI_ID_AO, adapter);
        adapter = HI_NULL;
        return err;
    }

    memset(&info, 0, sizeof(struct i2c_board_info));
    info.addr = AIC31_I2C_ADDRESS;
    strlcpy(info.type, "tlv320aic3x", I2C_NAME_SIZE);

    client = i2c_new_device(adapter, &info);
    i2c_put_adapter(adapter);
    if (client == HI_NULL) {
        osal_kfree(HI_ID_AO, bit_data);
        osal_kfree(HI_ID_AO, adapter);
        adapter = HI_NULL;
        return -ENOTTY;
    }

    return 0;
}

int remove_i2c(hi_void)
{
    int err = 0;
    TRP();

    struct i2c_algo_bit_data *bit_data = (struct i2c_algo_bit_data *)adapter->algo_data;

    i2c_del_adapter(adapter);

    if (bit_data != HI_NULL) {
        osal_kfree(HI_ID_AO, bit_data);
    }

    if (adapter != HI_NULL) {
        HI_KFREE(HI_ID_AO, adapter);
        adapter = HI_NULL;
    }

    return err;
}

int hi_gpio_i2c_pdrv_suspend(hi_void)
{
    i2c_pin_release();
    return 0;
}

int hi_gpio_i2c_pdrv_resume(hi_void)
{
    i2c_pin_init();
    return 0;
}

hi_void godbox_aic31_reset(hi_void)
{
    TRP();
    hi_drv_gpio_set_dir_bit(SLAC_RESET_GPIO_GNUM_S40 * 8 + SLAC_RESET_GPIO_PNUM_S40, 0);
    hi_drv_gpio_write_bit(SLAC_RESET_GPIO_GNUM_S40 * 8 + SLAC_RESET_GPIO_PNUM_S40, 0);
    ndelay(20);

    hi_drv_gpio_write_bit(SLAC_RESET_GPIO_GNUM_S40 * 8 + SLAC_RESET_GPIO_PNUM_S40, 1);
    ndelay(10);
}

#if SDK_TC_DEMO_GPIO_SETTING
hi_void godbox_aic31_mute(int mute)
{
    hi_drv_gpio_set_dir_bit(MUTE_GPIO_GNUM_S40 * 8 + MUTE_GPIO_PNUM_S40, 0);
    if (mute) {
        hi_drv_gpio_write_bit(MUTE_GPIO_GNUM_S40 * 8 + MUTE_GPIO_PNUM_S40, 1);
    } else {
        hi_drv_gpio_write_bit(MUTE_GPIO_GNUM_S40 * 8 + MUTE_GPIO_PNUM_S40, 0);
    }
}
#endif
