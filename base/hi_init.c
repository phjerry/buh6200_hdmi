#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/semaphore.h>

#include "hi_type.h"

#include "drv_vdec_ext.h"
#include "drv_pmoc_ext.h"
#include "drv_gpio_ext.h"
#include "drv_gpioi2c_ext.h"
#include "drv_i2c_ext.h"
#include "hi_drv_otp.h"
#include "hi_drv_keyslot.h"
#include "hi_drv_klad.h"
#include "hi_drv_demux.h"
#include "hi_drv_avplay.h"
#include "hi_drv_pvr.h"
#include "drv_sync_ext.h"
#include "drv_adec_ext.h"
#include "drv_sci_ext.h"
#include "drv_keyled_ext.h"
#include "drv_ir_ext.h"
#include "drv_hifb_ext.h"
#include "hi_drv_tde.h"
//#include "drv_png_ext.h"
#include "drv_disp_ext.h"
#include "drv_hdmi_module_ext.h"
#include "drv_cipher_ext.h"
#include "drv_vfmw_ext.h"
#include "drv_omxvdec_ext.h"
#include "drv_mce_ext.h"
#include "drv_vpss_ext.h"
#include "drv_venc_ext.h"
#include "drv_pdm_ext.h"
#include "drv_aenc_ext.h"
#include "drv_ao_ext.h"
#include "drv_adsp_ext.h"
#include "hi_drv_sys.h"
#include "hi_drv_jpeg.h"
//#include "drv_vi_ext.h"
#include "drv_pq_ext.h"
#include "drv_vpss_ext.h"
#include "drv_lsadc_ext.h"
#include "drv_spread_ext.h"

#ifdef HI_FRONTEND_SUPPORT
#include "drv_frontend_ext.h"
#endif

#ifdef HI_TSR2RCIPHER_SUPPORT
#include "hi_drv_tsr2rcipher.h"
#endif

#ifdef HI_MIPI_SUPPORT
#include "drv_mipi_ext.h"
#endif

#ifdef HI_PWM_SUPPORT
#include "drv_pwm_ext.h"
#endif

#ifdef HI_GFX2D_SUPPORT
#include "hi_drv_gfx2d.h"
#endif

//#include "drv_ci_ext.h"
//#include "drv_cimaxplus_ext.h"

#ifdef HI_TSIO_SUPPORT
#include "drv_tsio_ext.h"
#endif

#ifdef HI_SM_SUPPORT
#include "drv_sm_ext.h"
#endif

#ifdef HI_LOAD_CAS_IMAGE
#include "drv_casimage_ext.h"
#endif

#if defined(HI_CPU_DVFS_SUPPORT) || defined(HI_TEMP_CTRL_SUPPORT)
#include "drv_dvfs_ext.h"
#endif

#ifdef HI_TEE_SUPPORT
#include "hi_drv_ssm.h"
#endif
extern hi_s32 pq_drv_mod_init(hi_void);
extern hi_s32 __init hdmi_module_init(void);
extern hi_s32 vdp_drv_mod_init(hi_void);
extern hi_s32 otp_mod_init(hi_void);
#if defined(HI_LOADER_APPLOADER)
hi_void HI_LoaderInit(hi_void)
{
    drv_common_module_init();

#ifdef HI_TEE_SUPPORT
    ssm_mod_init();
#endif
    gpio_drv_module_init();

#ifdef HI_GPIOI2C_SUPPORT
    gpio_i2c_drv_module_init();
#endif

#ifndef HI_IR_TYPE_LIRC
    ir_drv_module_init();
#endif

    pdm_drv_mod_init();

#ifdef HI_MAILBOX_SUPPORT
    MAILBOX_ModInit();
#endif

    PQ_DRV_ModInit();

    cipher_drv_mod_init();

    drv_hdmi_mod_init();

#ifdef HI_MIPI_SUPPORT
    drv_mipi_mod_init();
#endif

    hi_drv_disp_mod_init();

#ifdef HI_RM_SUPPORT
    rm_drv_mod_init();
#endif

    hi_drv_tde_module_init();

    hi_drv_jpeg_module_init();

    HIFB_DRV_ModInit();

    hi_drv_otp_mod_init();

    hi_drv_ks_mod_init();

    hi_drv_klad_mod_init();

    i2c_drv_module_init();

#ifdef HI_FRONTEND_SUPPORT
    fe_drv_module_init();
#endif

#ifdef HI_DEMUX_SUPPORT
    dmx_mod_init();
#endif

#ifdef HI_KEYLED_SUPPORT
    keyled_drv_module_init();
#endif

    /* Please put pmoc in last one for stability */
    pmoc_drv_mod_init();
}
#elif defined(HI_RECOVERY_SUPPORT)
hi_void HI_RecoveryInit(hi_void)
{
    drv_common_module_init();

#ifdef HI_TEE_SUPPORT
    ssm_mod_init();
#endif

    gpio_drv_module_init();

#ifdef HI_GPIOI2C_SUPPORT
    gpio_i2c_drv_module_init();
#endif

#ifndef HI_IR_TYPE_LIRC
    ir_drv_module_init();
#endif

    pdm_drv_mod_init();

    pq_drv_mod_init();

    cipher_drv_mod_init();

    hdmi_module_init();

#ifdef HI_MIPI_SUPPORT
    drv_mipi_mod_init();
#endif

    hi_drv_disp_mod_init();

#ifdef HI_RM_SUPPORT
    rm_drv_mod_init();
#endif

    hi_drv_tde_module_init();

    hi_drv_jpeg_module_init();

    HIFB_DRV_ModInit();


#ifdef HI_PNG_SUPPORT
    PNG_DRV_ModInit();
#endif

    hi_drv_otp_mod_init();

    hi_drv_klad_mod_init();

#ifdef HI_GFX2D_SUPPORT
    hi_drv_gfx2d_module_init();
#endif

#ifdef HI_KEYLED_SUPPORT
    keyled_drv_module_init();
#endif
}
#else
hi_void HI_SystemInit(hi_void)
{
    drv_common_module_init();

#ifdef HI_TEE_SUPPORT
    ssm_mod_init();
#endif

    gpio_drv_module_init();

#ifdef HI_GPIOI2C_SUPPORT
    gpio_i2c_drv_module_init();
#endif

#ifndef HI_IR_TYPE_LIRC
    ir_drv_module_init();
#endif

    pdm_drv_mod_init();
    vpss_drv_mod_init();
    pq_drv_mod_init();

    cipher_drv_mod_init();

    hdmi_module_init();

#ifdef HI_MIPI_SUPPORT
    drv_mipi_mod_init();
#endif

    hi_drv_disp_mod_init();

#ifdef HI_RM_SUPPORT
    rm_drv_mod_init();
#endif

    hi_drv_tde_module_init();

    hi_drv_jpeg_module_init();

    HIFB_DRV_ModInit();


#ifdef HI_PNG_SUPPORT
    PNG_DRV_ModInit();
#endif

#ifdef HI_GFX2D_SUPPORT
    hi_drv_gfx2d_module_init();
#endif

    hi_drv_otp_mod_init();

    hi_drv_ks_mod_init();

#ifdef HI_MAILBOX_SUPPORT
    mailbox_mod_init();
#endif

    hi_drv_klad_mod_init();

    i2c_drv_module_init();

#ifdef HI_SPI_BUS_SUPPORT
    SPI_DRV_ModInit();
#endif

#ifdef HI_FRONTEND_SUPPORT
    fe_drv_module_init();
#endif

#ifdef HI_DEMUX_SUPPORT
    dmx_mod_init();
#endif

#ifdef HI_TSIO_SUPPORT
    tsio_mod_init();
#endif

#ifdef HI_TSR2RCIPHER_SUPPORT
    tsr2rcipher_mod_init();
#endif

#ifdef HI_PWM_SUPPORT
    PWM_DRV_ModInit();
#endif

#ifdef HI_SM_SUPPORT
    SM_DRV_ModInit();
#endif

#ifdef HI_PVR_SUPPORT
    pvr_drv_mod_init();
#endif

    sync_drv_mod_init();

    avplay_drv_mod_init();

    adsp_drv_mod_init();

    aiao_drv_mod_init();

    adec_drv_mod_init();

    drv_vfmw_mod_init();

    drv_vdec_init_module();

#ifdef HI_SCI_SUPPORT
    sci_drv_module_init();
#endif

#ifdef HI_OMX_SUPPORT
    drv_omxvdec_mod_init();
#endif

//    MCE_DRV_ModInit();

#ifdef HI_JPGE_SUPPORT
//    hi_drv_jpge_module_init();
#endif

#ifdef HI_KEYLED_SUPPORT
    //keyled_drv_mode_init();
#endif

#ifdef HI_AENC_SUPPORT
    aenc_drv_mod_init();
#endif

#ifdef HI_VENC_SUPPORT
    drv_venc_init_module();
#endif

#ifdef HI_VI_SUPPORT
//    VI_DRV_ModInit();
#endif

    /* Please put pmoc in last one for stability */
    pmoc_drv_mod_init();

#if defined(HI_CPU_DVFS_SUPPORT) || defined(HI_TEMP_CTRL_SUPPORT)
    dvfs_drv_mod_init();
#endif

#ifdef HI_LSADC_SUPPORT
    lsadc_drv_mod_init();
#endif

#ifdef HI_SPREAD_SUPPORT
    spread_drv_mod_init();
#endif
}
#endif

hi_s32 hi_load_modules(hi_void)
{
#if defined(HI_LOADER_APPLOADER)
    HI_LoaderInit();
#elif defined(HI_RECOVERY_SUPPORT)
    HI_RecoveryInit();
#else
    HI_SystemInit();
#endif

    return HI_SUCCESS;
}

late_initcall(hi_load_modules);

