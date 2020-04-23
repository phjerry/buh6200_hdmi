#ifndef __HI_DRV_PDM_H__
#define __HI_DRV_PDM_H__

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_disp_type.h"
#include "hi_ao_type.h"
#include "hi_drv_frontend.h"
#include "hi_drv_module.h"
#include "hi_drv_mipi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MCE_DEF_BASEPARAM_SIZE      (8 * 1024)
#define MCE_DEF_LOGOPARAM_SIZE      (8 * 1024)
#define MCE_DEF_PLAYPARAM_SIZE      (8 * 1024)
#define ANI_MAX_PIC_SUPPORT         30

#define PARSER_BASE_FLASH_NAME          "baseparam"
#define PARSER_LOGO_FLASH_NAME          "logo"
#define PARSER_FASTPLAY_FLASH_NAME      "fastplay"

#define MCE_BASE_TABLENAME_DISP0        "BASE_TABLE_DISP0"
#define MCE_BASE_TABLENAME_DISP1        "BASE_TABLE_DISP1"

#define MCE_BASE_KEYNAME_ENABLE         "BASE_KEY_ENABLE"

#define MCE_BASE_KEYNAME_FMT            "BASE_KEY_FMT"
#define MCE_BASE_KEYNAME_INTF_HDMI      "BASE_KEY_HDMI0"
#define MCE_BASE_KEYNAME_INTF_YPBPR     "BASE_KEY_YPBPR0"
#define MCE_BASE_KEYNAME_INTF_CVBS      "BASE_KEY_CVBS0"
#define MCE_BASE_KEYNAME_INTF_RGB       "BASE_KEY_RGB0"
#define MCE_BASE_KEYNAME_INTF_SVIDEO    "BASE_KEY_SVIDEO0"
#define MCE_BASE_KEYNAME_INTF_BT656     "BASE_KEY_BT6560"
#define MCE_BASE_KEYNAME_INTF_BT1120    "BASE_KEY_BT11200"
#define MCE_BASE_KEYNAME_INTF_LCD       "BASE_KEY_LCD0"
#define MCE_BASE_KEYNAME_INTF_VGA       "BASE_KEY_VGA0"
#define MCE_BASE_KEYNAME_INTF_MIPI      "BASE_KEY_MIPI0"

#define MCE_BASE_KEYNAME_PF             "BASE_KEY_PF"
#define MCE_BASE_KEYNAME_DISPW          "BASE_KEY_DISPW"
#define MCE_BASE_KEYNAME_DISPH          "BASE_KEY_DISPH"
#define MCE_BASE_KEYNAME_SCRX           "BASE_KEY_SCRX"
#define MCE_BASE_KEYNAME_SCRY           "BASE_KEY_SCRY"
#define MCE_BASE_KEYNAME_SCRW           "BASE_KEY_SCRW"
#define MCE_BASE_KEYNAME_SCRH           "BASE_KEY_SCRH"
#define MCE_BASE_KEYNAME_HULEP          "BASE_KEY_HULEP"
#define MCE_BASE_KEYNAME_SATU           "BASE_KEY_SATU"
#define MCE_BASE_KEYNAME_CONTR          "BASE_KEY_CONTR"
#define MCE_BASE_KEYNAME_BRIG           "BASE_KEY_BRIG"
#define MCE_BASE_KEYNAME_BGCOLOR        "BASE_KEY_BGCOLOR"
#define MCE_BASE_KEYNAME_MACRSN         "BASE_KEY_MACRSN"
#define MCE_BASE_KEYNAME_TIMING         "BASE_KEY_TIMING"
#define MCE_BASE_KEYNAME_GAMA           "BASE_KEY_GAMA"
#define MCE_BASE_KEYNAME_ASPECT         "BASE_KEY_ASPECT"

#define MCE_BASE_KEYNAME_MIPI_LANEID    "BASE_KEY_MIPI_LANEID0"
#define MCE_BASE_KEYNAME_MIPI_OUT_MODE  "BASE_KEY_MIPI_OUT_MODE0"
#define MCE_BASE_KEYNAME_MIPI_VID_MODE  "BASE_KEY_MIPI_VID_MODE0"
#define MCE_BASE_KEYNAME_MIPI_MODE      "BASE_KEY_MIPI_MODE0"
#define MCE_BASE_KEYNAME_MIPI_BPP       "BASE_KEY_MIPI_BPP0"
#define MCE_BASE_KEYNAME_MIPI_BRI       "BASE_KEY_MIPI_BRI0"
#define MCE_BASE_KEYNAME_MIPI_CMD_CNT   "BASE_KEY_MIPI_CMD_CNT0"
#define MCE_BASE_KEYNAME_MIPI_PANEL_CMD "BASE_KEY_MIPI_PANEL"

#define MCE_BASE_KEYNAME_SRC_DISP       "BASE_KEY_SOURCE_DISP"
#define MCE_BASE_KEYNAME_VIRSCW         "BASE_KEY_VIRSCW"
#define MCE_BASE_KEYNAME_VIRSCH         "BASE_KEY_VIRSCH"
#define MCE_BASE_KEYNAME_DISP_L         "BASE_KEY_DISP_L"
#define MCE_BASE_KEYNAME_DISP_T         "BASE_KEY_DISP_T"
#define MCE_BASE_KEYNAME_DISP_R         "BASE_KEY_DISP_R"
#define MCE_BASE_KEYNAME_DISP_B         "BASE_KEY_DISP_B"
#define MCE_LOGO_TABLENAME              "LOGO_TABLE"
#define MCE_LOGO_KEYNAME_FLAG           "LOGO_KEY_FLAG"
#define MCE_LOGO_KEYNAME_CONTLEN        "LOGO_KEY_LEN"

#define MCE_PLAY_TABLENAME              "PLAY_TABLE"
#define MCE_PLAY_KEYNAME_FLAG           "PLAY_KEY_FLAG"
#define MCE_PLAY_KEYNAME_DATALEN        "PLAY_KEY_DATALEN"
#define MCE_PLAY_KEYNAME_PARAM          "PLAY_KEY_PARAM"

#define MCE_BASE_TABLENAME_SOUND0       "BASE_TABLE_SOUND0"
#define MCE_BASE_TABLENAME_SOUND1       "BASE_TABLE_SOUND1"
#define MCE_BASE_TABLENAME_SOUND2       "BASE_TABLE_SOUND2"

#define MCE_BASE_KEYNAME_SND_DAC0       "BASE_KEY_SND_DAC0"
#define MCE_BASE_KEYNAME_SND_I2S0       "BASE_KEY_SND_I2S0"
#define MCE_BASE_KEYNAME_SND_I2S1       "BASE_KEY_SND_I2S1"
#define MCE_BASE_KEYNAME_SND_SPDIF0     "BASE_KEY_SND_SPDIF0"
#define MCE_BASE_KEYNAME_SND_HDMI0      "BASE_KEY_SND_HDMI0"
#define MCE_BASE_KEYNAME_SND_ARC0       "BASE_KEY_SND_ARC0"

#define MCE_BASE_KEYNAME_SND_DAC0_VOLUME    "BASE_KEY_SND_DAC_VOLUME0"
#define MCE_BASE_KEYNAME_SND_I2S0_VOLUME    "BASE_KEY_SND_I2S_VOLUME0"
#define MCE_BASE_KEYNAME_SND_I2S1_VOLUME    "BASE_KEY_SND_I2S_VOLUME1"
#define MCE_BASE_KEYNAME_SND_SPDIF0_VOLUME  "BASE_KEY_SND_SPDIF_VOLUME0"
#define MCE_BASE_KEYNAME_SND_HDMI0_VOLUME   "BASE_KEY_SND_HDMI_VOLUME0"
#define MCE_BASE_KEYNAME_SND_ARC0_VOLUME    "BASE_KEY_SND_ARC_VOLUME0"

/* timing key */
#define MCE_BASE_KEYNAME_TIMING_VFB         "BASE_KEY_TIMING_VFB"   /* VFB */
#define MCE_BASE_KEYNAME_TIMING_VBB         "BASE_KEY_TIMING_VBB"   /* VBB */
#define MCE_BASE_KEYNAME_TIMING_VACT        "BASE_KEY_TIMING_VACT"  /* VACT */

#define MCE_BASE_KEYNAME_TIMING_HFB         "BASE_KEY_TIMING_HFB"   /* HFB */
#define MCE_BASE_KEYNAME_TIMING_HBB         "BASE_KEY_TIMING_HBB"   /* HBB */
#define MCE_BASE_KEYNAME_TIMING_HACT        "BASE_KEY_TIMING_HACT"  /* HACT */

#define MCE_BASE_KEYNAME_TIMING_VPW         "BASE_KEY_TIMING_VPW"   /* VPW */
#define MCE_BASE_KEYNAME_TIMING_HPW         "BASE_KEY_TIMING_HPW"   /* HPW */
#define MCE_BASE_KEYNAME_TIMING_IDV         "BASE_KEY_TIMING_IDV"   /* IDV */
#define MCE_BASE_KEYNAME_TIMING_IHS         "BASE_KEY_TIMING_IHS"   /* IHS */
#define MCE_BASE_KEYNAME_TIMING_IVS         "BASE_KEY_TIMING_IVS"   /* IVS */


#define MCE_BASE_KEYNAME_TIMING_CLKR        "BASE_KEY_TIMING_CLKR"  /* ClockReversal */
#define MCE_BASE_KEYNAME_TIMING_DW          "BASE_KEY_TIMING_DW"    /* DataWidth */
#define MCE_BASE_KEYNAME_TIMING_ITFMT       "BASE_KEY_TIMING_ITFMT" /* ItfFormat */

#define MCE_BASE_KEYNAME_TIMING_DITE        "BASE_KEY_TIMING_DITE"  /* DitherEnable */
#define MCE_BASE_KEYNAME_TIMING_CLK0        "BASE_KEY_TIMING_CLK0"  /* ClkPara0 */
#define MCE_BASE_KEYNAME_TIMING_CLK1        "BASE_KEY_TIMING_CLK1"  /* ClkPara1 */

#define MCE_BASE_KEYNAME_TIMING_INTERL      "BASE_KEY_TIMING_INTERL"    /* bInterlace */
#define MCE_BASE_KEYNAME_TIMING_FIXF        "BASE_KEY_TIMING_FIXF"      /* PixFreq */
#define MCE_BASE_KEYNAME_TIMING_VERTF       "BASE_KEY_TIMING_VERTF"     /* VertFreq */
#define MCE_BASE_KEYNAME_TIMING_AW          "BASE_KEY_TIMING_AW"        /* AspectRatioW */
#define MCE_BASE_KEYNAME_TIMING_AH          "BASE_KEY_TIMING_AH"        /* AspectRatioH */
#define MCE_BASE_KEYNAME_TIMING_USEGAMMA    "BASE_KEY_TIMING_USEGAMMA"  /* bUseGamma */
#define MCE_BASE_KEYNAME_TIMING_RSV0        "BASE_KEY_TIMING_RSV0"      /* Reserve0 */
#define MCE_BASE_KEYNAME_TIMING_RSV1        "BASE_KEY_TIMING_RSV1"      /* Reserve1 */
#define MCE_BASE_KEYNAME_TIMING_REFR_RATE   "BASE_KEY_TIMING_REFR_RATE" /* refresh rate */

#define MCE_BASE_TABLENAME_PMOC             "BASE_TABLE_PMOC"
#define MCE_BASE_KEYNAME_PMOC_FLAG          "BASE_KEY_PMOC_FLAG"

#define MCE_BASE_TABLENAME_HDMI             "BASE_TABLE_HDMI"
#define MCE_BASE_KEYNAME_EDID_LENTH         "BASE_KEY_EDID_LENTH"
#define MCE_BASE_KEYNAME_EDID_CONTENT       "BASE_KEY_EDID_CONTENT"
#define MCE_BASE_KEYNAME_HDMI_VIDEOMODE     "BASE_KER_HDMI_VIDEOMODE"
#define MCE_BASE_KEYNAME_HDMI_DEEPCOLOR     "BASE_KER_HDMI_DEEPCOLOR"

#define MCE_BASE_TABLENAME_VERSION          "BASE_TABLE_VERSION"
#define MCE_BASE_KEYNAME_UNF_VERSION        "BASE_KEY_UNF_VERSION"

#define HI_FATAL_PDM(format...)    HI_FATAL_PRINT(HI_ID_PDM, format)
#define HI_ERR_PDM(format...)      HI_ERR_PRINT(HI_ID_PDM, format)
#define HI_WARN_PDM(format...)     HI_WARN_PRINT(HI_ID_PDM, format)
#define HI_INFO_PDM(format...)     HI_INFO_PRINT(HI_ID_PDM, format)

#define PDM_BASEPARAM_BUFNAME   "baseparam"

typedef enum {
    PDM_ENC_ORG_FMT_3840X2160_24 = 0x100,
    PDM_ENC_ORG_FMT_3840X2160_25,
    PDM_ENC_ORG_FMT_3840X2160_30,
    PDM_ENC_ORG_FMT_3840X2160_50,
    PDM_ENC_ORG_FMT_3840X2160_60,

    PDM_ENC_ORG_FMT_4096X2160_24,
    PDM_ENC_ORG_FMT_4096X2160_25,
    PDM_ENC_ORG_FMT_4096X2160_30,
    PDM_ENC_ORG_FMT_4096X2160_50,
    PDM_ENC_ORG_FMT_4096X2160_60,

    PDM_ENC_ORG_FMT_3840X2160_23_976,
    PDM_ENC_ORG_FMT_3840X2160_29_97,
    PDM_ENC_ORG_FMT_720P_59_94,
    PDM_ENC_ORG_FMT_1080P_59_94,
    PDM_ENC_ORG_FMT_1080P_29_97,
    PDM_ENC_ORG_FMT_1080P_23_976,
    PDM_ENC_ORG_FMT_1080i_59_94,
    PDM_ENC_ORG_FMT_MAX
} pdm_enc_org_fmt;

typedef enum {
    HI_PF_ARG8888,
    HI_PF_RGB888,
    HI_PF_ARG1555,
    HI_PF_RGB565,
    HI_PF_MAX,
} hi_pixel_format;

typedef struct {
    hi_u32                          disp_enable;
    hi_drv_display                  src_disp;
    hi_drv_disp_fmt                 format;
    hi_u32                          brightness;
    hi_u32                          contrast;
    hi_u32                          saturation;
    hi_u32                          hue_plus;
    hi_bool                         gamma_enable;
    hi_drv_disp_color               bg_color;
    hi_drv_disp_intf                intf[HI_DRV_DISP_INTF_TYPE_MAX];
    hi_drv_disp_timing              disp_timing;
    hi_pixel_format                 pixel_format;
    hi_drv_disp_offset              offset_info;
    hi_u32                          virt_screen_width;
    hi_u32                          virt_screen_height;
    hi_drv_aspect_ratio             aspect_ratio;
    hi_drv_disp_pix_format          vid_out_mode;
    hi_drv_pixel_bitwidth           deep_color_mode;
    hi_drv_mipi_attr_set            mipi_pdm_attr;
    hi_drv_disp_output_color_space  output_color_gamut;
    hi_u32                          vic;
    hi_drv_disp_out_type            hdr_type;
    hi_u32                          hdr_match_content;
    hi_u32                          enSceneMode;
} hi_disp_param;

typedef struct {
    hi_u32                      check_flag;
    hi_u32                      logo_len;
} hi_logo_param;

#define HI_FLASH_NAME_LEN   32

/* Flash pation info for mce use */
typedef struct {
    hi_char             name[HI_FLASH_NAME_LEN];
    hi_u32              offset;
    hi_u32              size;
    hi_bool             shared;  /* whether share one partition with other data */
} pdm_flash_info;

typedef struct {
    hi_u32               port_num;
    hi_ao_port_attr      out_port[HI_AO_OUTPUT_PORT_MAX];
    hi_u32               audio_volume[HI_AO_OUTPUT_PORT_MAX];
} hi_drv_pdm_sound_param;


hi_s32 hi_drv_pdm_init(hi_void);
hi_s32 hi_drv_pdm_deinit(hi_void);
hi_s32 hi_drv_pdm_get_disp_param(hi_drv_display disp, hi_disp_param *disp_param);
hi_s32 hi_drv_pdm_get_sound_param(ao_snd_id sound, hi_drv_pdm_sound_param *sound_param);

/*
 * Ex interface for compatibility, there is no volume in old baseparam image,
 * pbVolumeExistInBase can distinguish a image is an old baseparam or a new baseparam;
 * and there are volume in both old and new fastplay image, fastplay will select using
 * volume in baseparam first, if baseparam is old image, fastplay will using volume in fastplay image
 */
hi_s32 hi_drv_pdm_get_sound_param_ex(ao_snd_id sound, hi_drv_pdm_sound_param *sound_param,
                                     hi_bool *volume_exist_in_base);
hi_s32 hi_drv_pdm_release_reserve_mem(const hi_char *buf_name);
hi_s32 hi_drv_pdm_get_reserve_mem(const hi_char *buf_name, hi_u32 *resever_addr, hi_u32 *resever_buf_size);
hi_s32 hi_drv_pdm_get_data(const hi_char *buf_name, hi_u8 **data_addr, hi_u32 *data_len);
hi_s32 hi_drv_pdm_set_license_data(hi_u64 lic_handle, hi_u32 lic_len);

#ifdef __cplusplus
}
#endif

#endif
