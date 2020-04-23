/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel define module
* Author: sdk
* Create: 2019-04-03
*/

#ifndef __DRV_PANEL_DEFINE_H__
#define __DRV_PANEL_DEFINE_H__

#include "hi_type.h"
#include "hi_drv_panel.h"
#include "drv_panel_data.h"
#include "drv_panel_proc.h"
#include "hi_debug.h"
#include "hi_module.h"
#include <linux/delay.h>
#include "hi_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef __DISP_PLATFORM_BOOT__
#define panel_msleep(n) mdelay(n)
#else
#define panel_msleep(n) msleep(n) /* WORKING: osal_msleep(n) */
#endif

#undef LOG_MODULE_ID
#define LOG_MODULE_ID  HI_ID_PANEL

#define panel_check_func_ret(func, fmt, ret) \
    do {                                     \
        ret = func;                          \
        if ((ret) != HI_SUCCESS) {             \
            HI_ERR_PRINT_FUNC_RES(fmt, ret); \
        }                                    \
    } while (0)

#define hi_log_fatal(fmt...)   HI_FATAL_PRINT(HI_ID_PANEL, fmt)
#define hi_log_err(fmt...)     HI_ERR_PRINT(HI_ID_PANEL, fmt)
#define hi_log_warn(fmt...)    HI_WARN_PRINT(HI_ID_PANEL, fmt)
#define hi_log_info(fmt...)    HI_INFO_PRINT(HI_ID_PANEL, fmt)
#define hi_log_dbg(fmt...)     HI_DBG_PRINT(HI_ID_PANEL, fmt)

#define hi_err_print_u32(val)  HI_ERR_PRINT_U32(val)
#define hi_err_print_str(val)  HI_ERR_PRINT_STR(val)
#define hi_warn_print_u32(val) HI_WARN_PRINT_U32(val)
#define hi_warn_print_str(val) HI_WARN_PRINT_STR(val)
#define hi_dbg_print_u32(val)  HI_DBG_PRINT_U32(val)
#define hi_dbg_print_str(val)  HI_DBG_PRINT_STR(val)
#define hi_info_print_u32(val) HI_INFO_PRINT_U32(val)
#define hi_info_print_str(val) HI_INFO_PRINT_STR(val)
#define hi_info_func_enter()   HI_INFO_PRINT(HI_ID_PANEL, " ===>[Enter]\n")
#define hi_info_func_exit()    HI_INFO_PRINT(HI_ID_PANEL, " <===[Exit]\n")
#define hi_dbg_func_enter()    HI_DBG_PRINT(HI_ID_PANEL, " ===>[Enter]\n")
#define hi_dbg_func_exit()     HI_DBG_PRINT(HI_ID_PANEL, " <===[Exit]\n")

#define LDM_HORIZONTAL_NUM_MAX              6
#define LDM_VERICAL_NUM_MAX                 2
#define LDM_PROT_DATA_MAX                   8

#define PANEL_PWM_MAX_NUM                  4

#define DIM_BACKLIGHT_MAX                  255

#define PWM_RANGE_MAX                      (DIM_BACKLIGHT_MAX)
#define PWM_RANGE_MIN                      40
#define PWM_DEFAULT_LEVEL                  100

#define DIM_SUP_HOR_MIN_RES 1920
#define DIM_SUP_VER_MIN_RES 1080
#define DIM_SUP_HOR_MAX_RES 3840
#define DIM_SUP_VER_MAX_RES 2160

#define OUT_FRAME_RATE_PRECISION 1000

#define RESOLUTION_RATIO_PRECISION 1000

#define PANEL_HORIZONTAL_TIM_MIN 8
#define PANEL_VERICAL_TIM_MIN    1

#define PANEL_WIDTH_8K   7680
#define PANEL_HEIGHT_4K  4320
#define PANEL_WIDTH_4K   3840
#define PANEL_HEIGHT_2K  2160
#define PANEL_WIDTH_2K   1920
#define PANEL_HEIGHT_1K  1080
#define PANEL_WIDTH_1366 1366
#define PANEL_HEIGHT_768 768
#define PANEL_WIDTH_1280 1280
#define PANEL_HEIGHT_720 720
#define PANEL_HEIGHT_800 800
#define PANEL_MAX_WIDTH  3840
#define PANEL_MAX_HEIGHT 2160
#define PANEL_MIN_WIDTH  100
#define PANEL_MIN_HEIGHT 100
#define PANEL_WIDTH_5K   5120
#define PANEL_WIDTH_1024 1024
#define PANEL_HEIGHT_600 600

#define PANEL_HTOTAL_MAX 4800
#define PANEL_HTOTAL_MIN 4240

#define PANEL_LDM_HORSELIGHT_SPEED    60
#define PANEL_LDM_BLON_DELAY_STANDARD 550
#define PANEL_LDM_BLON_DELAY_TIME     50

#define PANEL_DIM_DOWN_SAMPLE_PARA 2

#define PANEL_BL_FREQ_MIN 48
#define PANEL_BL_FREQ_MAX 60000

#define PANEL_SPREAD_RATIO_MIN 0
#define PANEL_SPREAD_RATIO_MAX 31

#define PANEL_SPREAD_NOTTCON_MIN 2
#define PANEL_SPREAD_NOTTCON_MAX 5
#define PANEL_SPREAD_TCON_MIN    2
#define PANEL_SPREAD_TCON_MAX    15

#define PANEL_PIXELCLK_MAX       600000000
#define PANEL_PIXELCLK_TOLERENCE 1000000

#define TIMMING_CHECK_NOTE_COUNT 50

#define PANEL_INFO_ARRY_MEM_CNT  7
#define DEFAULT_PANEL_INDEX      1
#define TCON_BIN_MAP_PANEL_INDEX 3
#define DEFAULT_TCON_DATA_INDEX  1

#define I2C_MAX_DATAS 0xff

#define EVERY_GPIO_GROUP_8_BITS 8
#define GPIO_NUM_MAX_VALUE      25

#define PANEL_BOOL_MAX_VALUE 1

#define PANEL_LOCKN_DBG_MAX_VALUE 2

#define VSYNC_TO_SYNC_SIG_PHASE 0x1000

#define RESOLUTION_FRAMERATE_MAPTO_ISCTIM 20

#define PANEL_SPI_WAIT_OUT_CNT 10000

#define DRV_PANEL_SUSPEND_WAIT_RESUME_CNT 300

#define PANEL_DEFAULT_OUT_FRAME_RATE (DRV_PANEL_FRAME_RATE_60HZ)
#define PANEL_CEDS_28S_VALUE         1680
#define PANEL_CEDS_THRESHOLD         40
#define LANE_NUM                     16
#define RESERVED_NUM                 9

#define PANEL_TCON_PRECHARGECN     8
#define PANEL_TCON_SIG_MAX_DELAY   1000
#define PANEL_WORK_CAPABILITY_LINE 20

#define LOW_POWER_LANE_USED_8LINK  0xFFFA197
#define LOW_POWER_LANE_USED_16LINK 0xFFF0000
#define LOW_POWER_LANE_USED_COPY   0xFFF0000 /* for ARGB888 mode */

#define VBO_MIN_SPREAD_FREQ (HI_DRV_PANEL_SSFREQ_46P875KHZ)
#define VBO_MAX_SPREAD_FREQ (HI_DRV_PANEL_SSFREQ_MAX - 1)

#define clip3(x, low, high) (((x) < (low)) ? (low) : (((x) > (high)) ? (high) : (x)))

#define get_out_frame_rate(frm_rate) ((frm_rate) / (OUT_FRAME_RATE_PRECISION))

#define count_out_frame_rate(clk, ht, vt) \
    (((clk) / (ht) / (vt)) * (OUT_FRAME_RATE_PRECISION))

#define get_vtotal(pix_clk, frm_rate, hotal) \
    ((pix_clk) / (get_out_frame_rate(frm_rate)) / (hotal))

#define multiple_check(multiple, divisor) (((multiple) % (divisor) == 0) ? HI_TRUE : HI_FALSE)

#ifndef __DISP_PLATFORM_BOOT__
#define PANEL_SPINIRQLOCK_T osal_spinlock
#else
#define PANEL_SPINIRQLOCK_T hi_u32
#endif

#ifndef __DISP_PLATFORM_BOOT__
#define panel_spin_irqlock_init(g_st_irq_spin_lock)     osal_spin_lock_init(&(g_st_irq_spin_lock))
#define panel_spin_irqlock(g_st_irq_spin_lock, flags)   osal_spin_lock_irqsave(&(g_st_irq_spin_lock), &(flags))
#define panel_spin_irqunlock(g_st_irq_spin_lock, flags) osal_spin_unlock_irqrestore(&(g_st_irq_spin_lock), &(flags))
#else
#define panel_spin_irqlock_init(g_st_irq_spin_lock)
#define panel_spin_irqlock(g_st_irq_spin_lock, flags)
#define panel_spin_irqunlock(g_st_irq_spin_lock, flags)
#endif

#define LVDS_PHY_TXPLL_DIVSEL_FB      0x7A
#define LVDS_PHY_TXPLL_ICCP_CTRL      0x2
#define LVDS_PHY_TXPLL_DIVSEL_IN      0x1
#define LVDS_PHY_TXPLL_DIVSEL_IN_1366 0x0

#define VBO_PHY_TXPLL_DIVSEL_FB_8BIT  0x79
#define VBO_PHY_TXPLL_ICCP_CTRL_8BIT  0x2
#define VBO_PHY_TXPLL_DIVSEL_FB_10BIT 0x60
#define VBO_PHY_TXPLL_ICCP_CTRL_10BIT 0x3
#define VBO_PHY_TXPLL_DIVSEL_IN       0x1

#define MLVDS_PHY_TXPLL_DIVSEL_FB           0x186
#define MLVDS_PHY_TXPLL_DIVSEL_IN           0x1
#define MLVDS_PHY_TXPLL_ICP_CTRL            0x5
#define MLVDS_PHY_TXPLL_ICP_CTRL_FHD_8BIT   0x2
#define MLVDS_PHY_TXPLL_DIVSEL_IN_1280_8BIT 0x3
#define MLVDS_PHY_TXPLL_DIVSEL_FB_FHD_8BIT  0x86
#define MLVDS_PHY_TXPLL_DIVSEL_FB_1280_8BIT 0x7
#define MLVDS_PHY_TXPLL_DIVSEL_FB_1366_6BIT 0x87

#define PANEL_PARSE_PROC_LEN                10
#define PANEL_OUTFRMRATE_RATE               2
#define PANEL_SPI_DELAY_20                  20
#define PANEL_DELAY_100                     100
#define PWM_DEFAULT_NUM                     0
#define PANEL_MSLEEP_10                     10
#define PANEL_MSLEEP_20                     20
#define PANEL_MSLEEP_50                     50
#define PANEL_MSLEEP_1000                   1000
#define COVER_INT_TO_STR_NUM                10
#define HIST_GRAM_MAX_NUM                   32
#define TCON_PERI_DATA_LEN                  80
#define TCON_I2C_NUM_MAX                     7


/*******************************************************************************
                                LVDS struct and enum
*******************************************************************************/

typedef enum {
    PANEL_VOLTAGE_400MV,
    /* < voltage value 400MV */ /* <c_ncomment: ��ģ��ѹ400MV */
    PANEL_VOLTAGE_450MV,
    /* < voltage value 450MV */ /* <c_ncomment: ��ģ��ѹ450MV */
    PANEL_VOLTAGE_500MV,
    /* < voltage value 500MV */ /* <c_ncomment: ��ģ��ѹ500MV */
    PANEL_VOLTAGE_550MV,
    /* < voltage value 550MV */ /* <c_ncomment: ��ģ��ѹ550MV */
    PANEL_VOLTAGE_600MV,
    /* < voltage value 600MV */ /* <c_ncomment: ��ģ��ѹ600MV */
    PANEL_VOLTAGE_650MV,
    /* < voltage value 650MV */ /* <c_ncomment: ��ģ��ѹ650MV */
    PANEL_VOLTAGE_700MV,
    /* < voltage value 700MV */ /* <c_ncomment: ��ģ��ѹ700MV */
    PANEL_VOLTAGE_800MV,
    /* < voltage value 800MV */ /* <c_ncomment: ��ģ��ѹ800MV */
    PANEL_VOLTAGE_900MV,
    /* < voltage value 900MV */ /* <c_ncomment: ��ģ��ѹ900MV */
    PANEL_VOLTAGE_1000MV,
    /* < voltage value 1000MV */ /* <c_ncomment: ��ģ��ѹ1000MV */
    PANEL_VOLTAGE_1100MV,
    /* < voltage value 1100MV */ /* <c_ncomment: ��ģ��ѹ1100MV */
    PANEL_VOLTAGE_1150MV,
    /* < voltage value 1150MV */ /* <c_ncomment: ��ģ��ѹ1150MV */
    PANEL_VOLTAGE_1200MV,
    /* < voltage value 1200MV */ /* <c_ncomment: ��ģ��ѹ1200MV */
    PANEL_VOLTAGE_1250MV,
    /* < voltage value 1250MV */ /* <c_ncomment: ��ģ��ѹ1250MV */
    PANEL_VOLTAGE_1300MV,
    /* < voltage value 1300MV */ /* <c_ncomment: ��ģ��ѹ1300MV */
    PANEL_VOLTAGE_1400MV,
    /* < voltage value 1400MV */ /* <c_ncomment: ��ģ��ѹ1400MV */
    PANEL_VOLTAGE_1500MV,
    /* < voltage value 1500MV */ /* <c_ncomment: ��ģ��ѹ1500MV */
    PANEL_VOLTAGE_MAX,
    /* <invalid value */ /* <c_ncomment:�Ƿ��߽�ֵ */
} panel_voltage_type;

typedef enum {
    PANEL_CURRENT_200MV,
    /* <current value 200MV */ /* <c_ncomment:��������200MV */
    PANEL_CURRENT_250MV,
    /* <current value 250MV */ /* <c_ncomment:��������250MV */
    PANEL_CURRENT_300MV,
    /* <current value 300MV */ /* <c_ncomment:��������300MV */
    PANEL_CURRENT_350MV,
    /* <current value 350MV */ /* <c_ncomment:��������350MV */
    PANEL_CURRENT_400MV,
    /* <current value 400MV */ /* <c_ncomment:��������400MV */
    PANEL_CURRENT_450MV,
    /* <current value 450MV */ /* <c_ncomment:��������450MV */
    PANEL_CURRENT_500MV,
    /* <current value 500MV */ /* <c_ncomment:��������500MV */
    PANEL_CURRENT_MAX,
    /* <invalid value */ /* <c_ncomment:�Ƿ��߽�ֵ */
} panel_current_type;

typedef enum {
    PANEL_EMP_0DB,
    /* <pre-emphasis is 0DB */ /* <c_ncomment:Ԥ����Ϊ0DB */
    PANEL_EMP_1DB,
    /* <pre-emphasis is 1B */ /* <c_ncomment:Ԥ����Ϊ1DB */
    PANEL_EMP_2DB,
    /* <pre-emphasis is 2DB */ /* <c_ncomment:Ԥ����Ϊ2DB */
    PANEL_EMP_3DB,
    /* <pre-emphasis is 3DB */ /* <c_ncomment:Ԥ����Ϊ3DB */
    PANEL_EMP_4DB,
    /* <pre-emphasis is 4DB */ /* <c_ncomment:Ԥ����Ϊ4DB */
    PANEL_EMP_5DB,
    /* <pre-emphasis is 5DB */ /* <c_ncomment:Ԥ����Ϊ5DB */
    PANEL_EMP_6DB,
    /* <pre-emphasis is 6DB */ /* <c_ncomment:Ԥ����Ϊ6DB */
    PANEL_EMP_7DB,
    /* <pre-emphasis is 7DB */ /* <c_ncomment:Ԥ����Ϊ7DB */
    PANEL_EMP_8DB,
    /* <pre-emphasis is 8DB */ /* <c_ncomment:Ԥ����Ϊ8DB */
    PANEL_EMP_9DB,
    /* <pre-emphasis is 9DB */ /* <c_ncomment:Ԥ����Ϊ9DB */
    PANEL_EMP_MAX,
    /* <invalid value */ /* <c_ncomment:�Ƿ��߽�ֵ */
} panel_emp_type;

typedef enum {
    PANEL_PORT_MODE_1_PORT,
    PANEL_PORT_MODE_2_PORT,
    PANEL_PORT_MODE_4_PORT,
    PANEL_PORT_MODE_6_PORT,
    PANEL_PORT_MODE_8_PORT,
    PANEL_PORT_MODE_12_PORT,
    PANEL_PORT_MODE_MAX,
} panel_port_mode;

typedef enum {
    PANEL_PAIR_MODE_1_PAIR,
    PANEL_PAIR_MODE_2_PAIR,
    PANEL_PAIR_MODE_3_PAIR,
    PANEL_PAIR_MODE_6_PAIR,
    PANEL_PAIR_MODE_MAX,
} panel_pair_mode;

typedef enum {
    PANEL_LVDS_SSFREQ_93P75KHZ = 2,
    PANEL_LVDS_SSFREQ_62P5KHZ = 3,
    PANEL_LVDS_SSFREQ_46P875KHZ = 4,
    PANEL_LVDS_SSFREQ_37P5KHZ = 5,
    PANEL_LVDS_SSFREQ_MAX,
} panel_lvds_ssfreq;

typedef enum {
    PANEL_VBO_SSFREQ_46P875KHZ = 2,
    PANEL_VBO_SSFREQ_31P250KHZ,
    PANEL_VBO_SSFREQ_23P438KHZ,
    PANEL_VBO_SSFREQ_18P750KHZ,

    PANEL_VBO_SSFREQ_93P875KHZ,
    PANEL_VBO_SSFREQ_62P5KHZ,
    PANEL_VBO_SSFREQ_37P5KHZ,
    PANEL_VBO_SSFREQ_26P786KHZ,
    PANEL_VBO_SSFREQ_20P833KHZ,
    PANEL_VBO_SSFREQ_MAX,
} panel_vbo_ssfreq;

typedef struct {
    hi_bool h_sync_output;
    hi_bool data_invert;
    hi_bool resv_invert;
    hi_drv_panel_bit_depth bit_width;
    hi_drv_panel_link_type link_type;
    hi_drv_panel_lvds_format fmt;           /* LVDS data send format */
    hi_drv_panel_lvds_link_map link_map;    /* LVDS link map */
    panel_current_type drv_current;              /* LVDS driver current */
    panel_voltage_type com_voltage;              /* LVDS com_voltage */
    hi_u32 spread_ratio;                    /* LVDS spread ratio from 0 to 31 */
    panel_lvds_ssfreq spread_freq;          /* LVDS spread frequence */
} panel_lvds_comp_attr;

/*******************************************************************************
                               VBO struct and enum
*******************************************************************************/
typedef enum {
    PANEL_VBO_DATAMODE_30BIT444,
    PANEL_VBO_DATAMODE_36BIT444,
    PANEL_VBO_DATAMODE_24BIT444,
    PANEL_VBO_DATAMODE_18BIT444,
    PANEL_VBO_DATAMODE_24BIT422,
    PANEL_VBO_DATAMODE_20BIT422,
    PANEL_VBO_DATAMODE_16BIT422,
    PANEL_VBO_DATAMODE_MAX,
} panel_vbo_data_mode;

typedef enum {
    DRV_PANEL_VBO_RESISTOR_50OHM = 0,
    DRV_PANEL_VBO_RESISTOR_44OHM,
    DRV_PANEL_VBO_RESISTOR_70OHM,
    DRV_PANEL_VBO_RESISTOR_IMPEDANCE,
    DRV_PANEL_VBO_RESISTOR_BUTT,
} panel_vbo_resistor;

typedef struct {
    /* 0��lane0~3 output left half screen data��1: lane4~7 output right half screen data */
    hi_bool group_swap;
    hi_bool left_internal_swap;                   /* lane0~3 internal swap */
    hi_bool right_internal_swap;                  /* lane4~7 internal swap */
    hi_bool bit0_at_high_bit;                      /* if big endian or little endian, default false, little endian */
    hi_bool locken_sw_mode; /* if use software locken mode, else used hardware locken mode, default false */
    hi_bool locken_high;                         /* if use software locken mode, can set high unlock or set low lock */
    hi_bool pn_swap;                             /* set vbo signal P/N swap, default false */
    hi_bool rising_edge; /* set vbo clock edge ctrl, true means rising edge, false means fall edge */
    panel_vbo_data_mode data_mode;      /* vbone date mode */
    hi_drv_panel_byte_num byte_num;  /* vbone byte per pixel */
    panel_current_type current_en;            /* vbone swing */
    panel_emp_type emphasis;               /* vbone pre-emphasis */
    panel_vbo_resistor resistor;         /* vbone resistor */
    hi_drv_panel_link_type link_type;          /* vbone link type, map to vbo lan number */
    hi_u32 spread_ratio;                       /* vb1 spread ratio from 0 to 31 */
    panel_vbo_ssfreq spread_freq;      /* vb1 spread frequence */
    hi_drv_panel_division_type division_type; /* panel division type */
} panel_vbo_comp_attr;

/*******************************************************************************
                                        other struct and enum
*******************************************************************************/
#define BIN_USAGE_TCON_REG 7
#define HEADER_OFFSET      280 /* 24+32*8 = 280 */

typedef struct {
    hi_u32 magic_num;
    hi_char au8_name[PANEL_NAME_MAX_LEN];
    hi_u16 u16_section_num;
    hi_u32 size;
    hi_u32 file_header_check_sum;
} panel_tcon_bin_head;

typedef struct {
    hi_u32 module_offset;
    hi_u32 module_size;
} tcon_module;

typedef struct {
    hi_u32 address;
    hi_u32 value;
    hi_u32 eg_mask;
} drv_tcon_reg_info;

typedef struct {
    hi_char ch_name[28];    /* 28: default parm */
    drv_tcon_reg_info *ctrl_reg_info_p;
    hi_u32 ctrl_reg_num;
    drv_tcon_reg_info *intf_reg_info_p;
    hi_u32 intf_reg_num;
} drv_tcon_data;

typedef struct {
    hi_bool peri_on;
    hi_u32 dev_addr;
    hi_u32 i2c_num;
    hi_u32 data_num;
    hi_bool vcom_control;
    hi_u32 vcom_pos;
    hi_u8 peri_ic[80];  /* 80: default parm */
} drv_tcon_peri_info;

typedef struct {
    hi_u32 reg_num;
    drv_tcon_reg_info *reg_data_p;
} drv_panel_tctrl_reg_info;

typedef enum {
    DRV_PANEL_ASPECT_4_3_PRECISION = 1400,
    DRV_PANEL_ASPECT_14_9_PRECISION = 1580,
    DRV_PANEL_ASPECT_16_10_PRECISION = 1700,
    DRV_PANEL_ASPECT_16_9_PRECISION = 2000,
    DRV_PANEL_ASPECT_21_9_PRECISION = 2400,
    DRV_PANEL_ASPECT_PRECISION_BUT,
} drv_panel_aspect_precision;

typedef struct {
    hi_u32 min_level[PANEL_PWM_MAX_NUM];
    hi_u32 max_level[PANEL_PWM_MAX_NUM];
    hi_u32 def_level[PANEL_PWM_MAX_NUM];
} drv_panel_def_bl_info;

typedef struct {
    hi_u32 frm_rate;
    hi_u32 dim_freq;
    hi_drv_panel_timing *timing_p;
} drv_panel_frame_rate_info;

typedef enum {
    DRV_PANEL_FRAME_RATE_20HZ = 20000,
    DRV_PANEL_FRAME_RATE_24HZ = 24000,
    DRV_PANEL_FRAME_RATE_25HZ = 25000,
    DRV_PANEL_FRAME_RATE_30HZ = 30000,
    DRV_PANEL_FRAME_RATE_48HZ = 48000,
    DRV_PANEL_FRAME_RATE_50HZ = 50000,
    DRV_PANEL_FRAME_RATE_60HZ = 60000,
    DRV_PANEL_FRAME_RATE_96HZ = 96000,
    DRV_PANEL_FRAME_RATE_100HZ = 100000,
    DRV_PANEL_FRAME_RATE_120HZ = 120000,

    DRV_PANEL_FRAME_RATE_BUTT
} drv_panel_frame_rate;

typedef enum {
    DRV_PANEL_EVEN_ALIGN = 2,
    DRV_PANEL_FOUR_ALIGN = 4,
    DRV_PANEL_EIGHT_ALIGN = 8,
    DRV_PANEL_ALIGN_BUTT,
} drv_panel_align_mode;

typedef struct {
    hi_drv_panel_timing *timing_p;
    hi_u32 panel_width;
    hi_u32 panel_height;
    hi_u16 u16_hsync_width;
    hi_u16 u16_vsync_width;
} drv_panel_timmint_check;

typedef struct {
    hi_bool power_on;
    hi_bool tcon_enable;
    hi_bool intf_enable;
    hi_bool back_light_enable;
} drv_panel_power_state;

typedef struct {
    /* while pre state is lock, current change to unlock, unclock count will increase */
    hi_bool pre_cdr_lock_state;
    hi_bool cdr_lock_state;
    hi_u32 cdr_un_lock_cnt;
} drv_panel_vbo_lock_state;

typedef struct {
    hi_bool intf_enable;
    hi_bool spread_enable;
    hi_u32 lvds_spread_ratio; /* <LVDS spread ratio from 0 to 31 */
    panel_lvds_ssfreq lvds_spread_freq;
    panel_current_type lvds_drv_current;
    panel_voltage_type lvds_com_voltage;
    hi_u32 vbo_spread_ratio; /* <vb1 spread ratio from 0 to 31 */
    panel_vbo_ssfreq vbo_spread_freq;
    panel_current_type vbo_current;
    panel_emp_type vbo_emphasis;
} hi_drv_panel_intf_attr;


typedef struct {
    hi_drv_panel_intf_type intf_type;
    hi_drv_panel_bit_depth bit_width;
    hi_drv_panel_intf_attr intf_attr;
    panel_vbo_comp_attr vbo_comp_attr;
    drv_panel_vbo_lock_state vbo_lock_stat;
    panel_lvds_comp_attr lvds_comp_attr;
} drv_panel_intf_attr;

typedef struct {
    hi_u32 func_call_cnt;
    hi_u32 timming_check_cnt;
} drv_panel_vdp_getcfg_state;

typedef struct {
    hi_bool dbg_tim_en;
    hi_bool dbg_tim_re_init;
    hi_bool use_debg_tim;

    hi_u32 pixel_clk;
    hi_u32 out_frm_frame;

    /* horizontal timming */
    hi_u32 htotal;
    hi_u32 width;
    hi_u16 u16_hsync_fp;
    hi_u16 u16_hsync_width;

    /* vertical timming */
    hi_u32 vtotal;
    hi_u32 height;
    hi_u16 u16_vsync_fp;
    hi_u16 u16_vsync_width;
} drv_panel_dbg_tim_info;

typedef struct {
    hi_bool fs_sig_invert;
    hi_bool fs_sig_init_high;
    hi_bool fs_sig_ref_sync_rise;
    hi_bool fs_sig_enable;
    hi_u32 fs_sig_ref_sync_phase;

    hi_bool glass_sig_invert;
    hi_bool glass_sig_init_high;
    hi_bool glass_sig_ref_fs_sig_in;
    hi_bool glass_sig_enable;
    hi_u32 glass_sig_ref_fs_sig_phase;
} drv_panel_3_d_fs_sig_info;

typedef struct { /* for tcon phy vir */
    hi_bool ssc_en;
    hi_u32 ssc_freq;
    hi_u32 ssc_ratio;
    hi_u32 au32_com_vlotage[LANE_NUM];
    hi_u32 au32_drv_current[LANE_NUM];
    hi_u32 au32_pre_emphasis[LANE_NUM];
    hi_u32 lane_used;
    hi_u32 pre_charge_cnt;
    hi_u32 tconio_delay[18]; /* 18 is index */
    hi_bool inv_28s_enable;
    hi_u32  reserved2[RESERVED_NUM];  /* 10 is index */
} drv_panel_tcon_info;

typedef struct {
    hi_u8 au8_com_voltage[LANE_NUM];
    hi_u8 au8_drv_current[LANE_NUM];
    hi_u8 au8_pre_emphasis[LANE_NUM];
} drv_panel_combo_attr;

typedef enum {
    DRV_PANEL_TCON_TIMING_2D = 0,
    DRV_PANEL_TCON_TIMING_3D,
    DRV_PANEL_TCON_TIMING_BUTT,
} drv_panel_tcon_timing;

typedef struct {
    hi_u32  i2c_cmd_num;                /* <I2c Cmd Numbers to be send when poweron */
    hi_u32  i2c_cmd_len;               /* <Data Numbers of One I2c Cmd */
    hi_u8   i2c_data[120];            /* <I2c Cmds Datas. 120:I2C max cmds data length */
    hi_u32  tcon_to_i2c_delay;          /* <Tcon poweron To starting sending I2c Cmds Delay Time(ms) */
    hi_u32  i2c_iterval_delay;         /* <Two I2c Cmds Sending Iterval Delay Time(ms) */
} panel_i2c_info;

typedef struct {
    hi_u32  gpio_num;                 /* <Gpio pin numbers to be pulled when poweron */
    hi_u8   gpio_data[8][3];          /* <Gpio Pin Datas. 8, 3: gpio pin max matrix */
} panel_gpio_info;

typedef struct {
    hi_bool ldm_en;        /* <Support Localdiming or not */
    hi_u8 bit_width;
    hi_u32 matrix_num;        /* <localdiming regular matrix numbers */

    hi_u8 ldm_act_num[LDM_HORIZONTAL_NUM_MAX][LDM_VERICAL_NUM_MAX]; /* <localdiming actual regular matrix datas */

    hi_bool checksum_in_head;          /* <Checksum in Prot Head or not */
    hi_u8   checksum_pos;              /* <Checksum Position in Prot Head or Tail */
    hi_u8   prot_head_byte_num;        /* <Prot Head Datas Numbers */
    hi_u8   prot_tail_byte_num;        /* <Prot Tail Datas Numbers */
    hi_u8   prot_head_data[8];         /* <Prot Head Datas. 8: prot head max length */
    hi_u8   prot_tail_data[8];         /* <Prot Tail Datas. 8: prot tail max length */
    hi_bool dynamic_backlight_en;
    hi_bool ldmspi_en;                 /* <CNcomment:ldmspi enable */
    hi_u32  vsync_to_cs_time;          /* <CNcomment:Vsync2cs time */
    hi_u32  cs_to_data_time;           /* <CNcomment:cs2data time */
    hi_u32  data_to_data_time;         /* <CNcomment:data2data time */
    hi_u32  data_to_cs_time;           /* <CNcomment:data2cs time */
    hi_u32  clk_freq;                  /* <CNcomment:clkfreq */
} panel_ldm_info;

typedef struct {
    hi_u8 pwm_invert[4]; /* 4:pwm default param */
    hi_u8 pwm_duty[4];  /* 4:pwm default param */
    hi_u8 pwm_freq[4];  /* 4:pwm default param */
} panel_pwm_info;

typedef struct {
    hi_u16  timming_change_step;                /* <timming change step */
    hi_bool frmrate_switch_black;               /* <timming change black */
    hi_drv_panel_flip_type    panel_flip_type;  /* <panel flip type */
    hi_drv_panel_byte_num     pixel_byte_num;   /* <pixel byte number */
    hi_bool support_other_frmrate;              /* <other frame rate support */
    hi_drv_panel_timing       timing_other;     /* <Panel timing info other */
    hi_u16  backlight_freq_other;               /* <blacklight frequence other */
    panel_i2c_info      i2c_info;
    panel_gpio_info      gpio_info;
    panel_ldm_info      ldm_info;
    panel_pwm_info      pwm_info;
    hi_u8   reserved[16];    /* 16:reserved byte */
} panel_advance_info;


typedef struct {
    hi_u32 panel_width;      /* < panel horizontal resulotion */
    hi_u32 panel_height;     /* < panel vertical resulotion */
    hi_u32 frame_rate_max;   /* < panel max output frame rate */
    hi_bool support_other;   /* < whether panel support other hz */
    hi_drv_panel_3d_type disp_3d_type;   /* < panel output 3D type(including 2D type,& resolution) */
    hi_drv_panel_intf_type intf_type;    /* < interface type */
    hi_drv_panel_bit_depth panel_bit_depth;

    hi_drv_panel_timing timing60_hz;  /* <panel 60_hz/120_hz timing info */
    hi_drv_panel_timing timing50_hz;  /* <panel 50_hz/100_hz timing info */
    hi_drv_panel_timing timing_other; /* <panel other_frm_rate timing info */

    hi_u16 hsync_width;    /* < hsync width */
    hi_u16 vsync_width;    /* < vsync width */

    hi_bool pwm_postive;        /* < panel backlight pwm postive */
    hi_u32 dimming_freq60hz;    /* < dimming frequence 60_hz/120_hz */
    hi_u32 dimming_freq50hz;    /* < dimming frequence 50_hz/100_hz */
    hi_u32 dimming_freq_other;  /* < dimming frequence  other_frm_rate */

    hi_s32 intf_signal_on_delay;  /* < open interface signal delay */
    hi_u32 bl_on_delay;           /* < open backlight delay */
    hi_u32 bl_off_delay;          /* < close interface signal delay */
    hi_u32 intf_signal_off_delay; /* < close backlight delay */

    hi_u32 bl_min_level[PANEL_PWM_MAX_NUM]; /* < min backlight level */
    hi_u32 bl_max_level[PANEL_PWM_MAX_NUM]; /* < max backlight level */
    hi_u32 bl_def_level[PANEL_PWM_MAX_NUM]; /* < default backlight lever */

    hi_bool localdiming_support; /* < support localdiming or not */

    hi_u32 matrix_num;           /* < localdiming regular matrix numbers */
    hi_u8 ldm_act_num[LDM_HORIZONTAL_NUM_MAX][LDM_VERICAL_NUM_MAX];
    hi_bool checksum_in_head;    /* < checksum in prot head or not */
    hi_u8 checksum_pos;          /* < checksum position in prot head or tail */
    hi_u8 prot_head_length;      /* < prot head datas numbers */
    hi_u8 prot_tail_length;      /* < prot tail datas numbers */
    hi_u8 reserve0;
    hi_u8 prot_head_data[LDM_PROT_DATA_MAX]; /* < 8: prot head datas */
    hi_u8 prot_tail_data[LDM_PROT_DATA_MAX]; /* < 8:prot head datas */

    hi_u32 ldm_bit_width;
    hi_drv_panel_division_type division_type;

    hi_drv_panel_flip_type panel_flip_type;       /* <panel flip type */
    hi_bool frm_rate_switch_black;                         /* <whether set black when timing change */

    drv_tcon_peri_info ast_tcon_peri_info[2];     /* 2: default parm */
    hi_u32 au32_save_peri_info[2];                /* 2: default parm */
} drv_reg_panel_info;


typedef struct hi_drv_panel_image {
    hi_u32 cur_index;
    hi_u32 total_num;
    hi_drv_panel_info base_info;
    panel_advance_info advance_info;
    drv_panel_def_bl_info def_bl_info;
    drv_reg_panel_info reg_panel_info;
    drv_panel_tcon_info tcon_info;
} drv_panel_image;

typedef struct {
    hi_drv_panel_osd_mode osd_mode;
    hi_bool display_3d_flag;
    hi_u32 src_frm_rate;
    hi_u32 disp_expect_width;
    hi_u32 disp_expect_height;
    hi_bool scene_change;
    hi_bool dolby_vison;
    hi_u32 dolby_expect_back_light;
} hi_drv_disp_expect_info;

typedef struct hi_drv_panel_context {
    hi_bool init;
    hi_bool boot_or_resuming;

    drv_panel_image image_info;
    /* vbo and lvds sence */
    drv_panel_intf_attr intf_ctx;

    hi_drv_disp_expect_info disp_expect_info;
    hi_drv_panel_cfg panel_cfg;
    /* combo attr for unf */
    drv_panel_combo_attr combo_attr;
    /* default info about outframerate */
    drv_panel_frame_rate_info frm_rate_info;

    drv_panel_vdp_getcfg_state cfg_state;
    drv_panel_dbg_tim_info dbg_tim_info;
    drv_panel_power_state power_state;
    drv_panel_3_d_fs_sig_info st3_d_fs_sig_info;

    hi_u32 lane_used;
    hi_bool misc_panel;
    hi_bool ldm_regist;
    hi_u32 isr_start_line;
    hi_u32 isr_finish_line;
    hi_bool dolby_scene_dim_enable;
    hi_bool dolby_en_test_flg;
    hi_u32 dolby_test_level;
    hi_void *panel_res_th_id;
} drv_panel_contex;

typedef struct {
    drv_panel_power_state power_state;
} drv_panel_run_info;

typedef struct {
    /* define in baseparam.img */
    hi_u32 disp_id;
    panel_id_index panel_id;
    hi_u32 panel_index;

    /* define in panel.bin */
    drv_panel_file_data file_data;

    /* internal info */
    drv_panel_run_info   run_info;
    drv_panel_debug_info debug_info;

    /* external info */
    hi_drv_disp_expect_info disp_expect_info;
    hi_drv_panel_info panel_info;
} drv_panel_context;

typedef struct {
    hi_bool init;
    hi_u32 context_num;
    drv_panel_context *context;
} drv_panel_global_context;

typedef hi_void (*fn_process)(hi_char *, hi_char *, hi_char *);
typedef struct hi_drv_panel_proc {
    hi_u8 *src_str_p;
    fn_process fn_process;
} drv_panel_proc;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

