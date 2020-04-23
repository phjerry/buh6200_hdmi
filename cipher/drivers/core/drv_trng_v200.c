/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drivers of drv_trng_v200
 * Author: zhaoguihong
 * Create: 2019-06-18
 */
#include "drv_trng_v200.h"
#include "drv_trng.h"

#ifdef CHIP_TRNG_VER_V200

/*************************** Internal Structure Definition ****************************/
/** \addtogroup      cipher drivers */
/** @{ */ /** <!-- [cipher] */

/* ! Define the osc sel */
#define TRNG_OSC_SEL   0x02
#define TRNG_POWER_ON  0x05
#define TRNG_POWER_OFF 0x0a

/** @} */ /** <!-- ==== Structure Definition end ==== */

/******************************* API Declaration *****************************/
/** \addtogroup      trng */
/** @{ */ /** <!--[trng] */

hi_s32 drv_trng_init(void)
{
    hisec_com_trng_ctrl ctrl;
    static hi_u32 last = 0x0A;

    hi_log_func_enter();

#ifdef TRNG_POWER_CTR_SUPPORT
    {
        hisec_com_trng_power_st power;

        /* power reduce enalbe */
        power.u32 = trng_read(HISEC_COM_TRNG_POWER_EN);
        if (power.bits.trng_power_en != TRNG_POWER_ON) {
            power.bits.trng_power_en = TRNG_POWER_ON;
            trng_write(HISEC_COM_TRNG_POWER_EN, power.u32);
        }
    }
#endif

    ctrl.u32 = trng_read(HISEC_COM_TRNG_CTRL);
    if (ctrl.u32 != last) {
        module_enable(CRYPTO_MODULE_ID_TRNG);

        ctrl.bits.mix_enable = 0x00;
        ctrl.bits.drop_enable = 0x00;
        ctrl.bits.pre_process_enable = 0x00;
        ctrl.bits.post_process_enable = 0x00;
        ctrl.bits.post_process_depth = 0x00;
        ctrl.bits.drbg_enable = 0x01;
        ctrl.bits.osc_sel = TRNG_OSC_SEL;
        trng_write(HISEC_COM_TRNG_CTRL, ctrl.u32);
        last = ctrl.u32;
    }

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_trng_deinit(void)
{
    hi_log_func_enter();

    hi_log_func_exit();
    return HI_SUCCESS;
}

hi_s32 drv_trng_randnum(hi_u32 *randnum, hi_u32 timeout)
{
    hisec_com_trng_data_st stat;
    hi_u32 times = 0;

    hi_log_func_enter();

    crypto_assert(randnum != HI_NULL);

    if (timeout == 0) { /* unblock */
        /* trng number is valid ? */
        stat.u32 = trng_read(HISEC_COM_TRNG_DATA_ST);
        if (stat.bits.trng_fifo_data_cnt == 0x00) {
            return HI_ERR_CIPHER_NO_AVAILABLE_RNG;
        }
    } else { /* block */
        while (times++ < timeout) {
            /* trng number is valid ? */
            stat.u32 = trng_read(HISEC_COM_TRNG_DATA_ST);
            if (stat.bits.trng_fifo_data_cnt > 0x00) {
                break;
            }
        }

        /* time out */
        if (times >= timeout) {
            return HI_ERR_CIPHER_NO_AVAILABLE_RNG;
        }
    }

    /* read valid randnum */
    *randnum = trng_read(HISEC_COM_TRNG_FIFO_DATA);
    hi_log_debug("randnum: 0x%x\n", *randnum);

    hi_log_func_exit();
    return HI_SUCCESS;
}

/** @} */ /** <!-- ==== API declaration end ==== */

#endif  // End of CHIP_TRNG_VER_V200
