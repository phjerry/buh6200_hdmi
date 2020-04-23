/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel hal common module
* Author: sdk
* Create: 2019-04-03
*/

#ifndef __HAL_PANEL_COMM_H__
#define __HAL_PANEL_COMM_H__

#ifdef __DISP_PLATFORM_BOOT__
#include <common.h>
#endif

#include "hi_type.h"
#include "hal_dphy_reg.h"
#include "hal_vbo_reg.h"
#include "hi_drv_panel.h"
#include "hi_drv_sys.h"
#include "hi_reg_vdp.h"
#include "drv_panel_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PWM_CLOCK             54000000
#define SPREAD_RATION_MAX     31
#define VBO_PHY_CLK_CHANNEL   0X0
#define LVDS_PHY_CLK_CHANNEL  0X10
#define MLVDS_PHY_CLK_CHANNEL 0X8

typedef vdp_regs_type s_panel_vdp_regs_type;

/*******************************************************************************
                                common struct and enum
*******************************************************************************/
typedef enum {
    HAL_PANEL_LINKTYPE_1LINK = 0,
    HAL_PANEL_LINKTYPE_2LINK,
    HAL_PANEL_LINKTYPE_4LINK,
    HAL_PANEL_LINKTYPE_8LINK,
    HAL_PANEL_LINKTYPE_16LINK,
    HAL_PANEL_LINKTYPE_BUTT,
} hal_panel_linktype;

/*******************************************************************************
                                LVDS struct and enum
*******************************************************************************/
typedef enum {
    HAL_PANEL_LVDS_LINKMODE_PHYINOUT_1366_60_HZ,
    HAL_PANEL_LVDS_LINKMODE_PHYOUT_1080_60_HZ,
    HAL_PANEL_LVDS_LINKMODE_PHYOUT_1080_120_HZ,
    HAL_PANEL_LVDS_LINKMODE_TEST_PATTERN,
    HAL_PANEL_LVDS_LINKMODE_BUTT
} hal_panel_lvds_linkmode;

typedef struct {
    hi_u32 link_a_swap;
    hi_u32 link_b_swap;
    hi_u32 link_c_swap;
    hi_u32 link_d_swap;
} hal_panel_lvds_swap;

typedef enum {
    HAL_PANEL_LVDS_DATA_SEND_FMT_VESA = 0x0,
    HAL_PANEL_LVDS_DATA_SEND_FMT_JEIDA = 0x1,
    HAL_PANEL_LVDS_DATA_SEND_FMT_FP = 0x2,
    HAL_PANEL_LVDS_DATA_SEND_FMT_OTHER = 0x3,
    HAL_PANEL_LVDS_DATA_SEND_FMT_BUTT
} hal_panel_lvds_data_send_fmt;

typedef enum {
    HAL_PANEL_LVDS_TEST_MODE_ZERO = 0x0,
    HAL_PANEL_LVDS_TEST_MODE_ONE = 0x1,
    HAL_PANEL_LVDS_TEST_MODE_INV = 0x2,
    HAL_PANEL_LVDS_TEST_MODE_GRAY = 0x3,
    HAL_PANEL_LVDS_TEST_MODE_PRBS7 = 0x4,
    HAL_PANEL_LVDS_TEST_MODE_BUTT
} hal_panel_lvds_test_mode;

typedef enum {
    HAL_PANEL_LVDS_INV_DATA = 0x0,
    HAL_PANEL_LVDS_INV_HSYNC = 0x1,
    HAL_PANEL_LVDS_INV_VSYNC = 0x2,
    HAL_PANEL_LVDS_INV_DE = 0x3,
    HAL_PANEL_LVDS_INV_RESV = 0x4,
    HAL_PANEL_LVDS_INV_BUTT
} hal_panel_lvds_inv;

typedef enum {
    HAL_PANEL_LVDS_VOLTAGE_550MV = 0,
    /* *<voltage value 550MV */ /* *<c_ncomment: 共模电压550MV */
    HAL_PANEL_LVDS_VOLTAGE_600MV,
    /* *<voltage value 600MV */ /* *<c_ncomment: 共模电压600MV */
    HAL_PANEL_LVDS_VOLTAGE_650MV,
    /* *<voltage value 650MV */ /* *<c_ncomment: 共模电压650MV */
    HAL_PANEL_LVDS_VOLTAGE_700MV,
    /* *<voltage value 700MV */ /* *<c_ncomment: 共模电压700MV */
    HAL_PANEL_LVDS_VOLTAGE_800MV,
    /* *<voltage value 800MV */ /* *<c_ncomment: 共模电压800MV */
    HAL_PANEL_LVDS_VOLTAGE_900MV,
    /* *<voltage value 900MV */ /* *<c_ncomment: 共模电压900MV */
    HAL_PANEL_LVDS_VOLTAGE_1000MV,
    /* *<voltage value 1000MV */ /* *<c_ncomment: 共模电压1000MV */
    HAL_PANEL_LVDS_VOLTAGE_1100MV,
    /* *<voltage value 1100MV */ /* *<c_ncomment: 共模电压1100MV */
    HAL_PANEL_LVDS_VOLTAGE_1200MV,
    /* *<voltage value 1200MV */ /* *<c_ncomment: 共模电压1200MV */
    HAL_PANEL_LVDS_VOLTAGE_1300MV,
    /* *<voltage value 1300MV */ /* *<c_ncomment: 共模电压1300MV */
    HAL_PANEL_LVDS_VOLTAGE_1400MV,
    /* *<voltage value 1400MV */ /* *<c_ncomment: 共模电压1400MV */
    HAL_PANEL_LVDS_VOLTAGE_1500MV,
    /* *<voltage value 1500MV */ /* *<c_ncomment: 共模电压1500MV */

    HAL_PANEL_LVDS_VOLTAGE_BUTT,
} hal_panel_lvds_voltage;

typedef enum {
    HAL_PANEL_LVDS_CURRENT_200MV = 0,
    /* *<current value 200MV */ /* *<c_ncomment:驱动电流200MV */
    HAL_PANEL_LVDS_CURRENT_250MV,
    /* *<current value 250MV */ /* *<c_ncomment:驱动电流250MV */
    HAL_PANEL_LVDS_CURRENT_300MV,
    /* *<current value 300MV */ /* *<c_ncomment:驱动电流300MV */
    HAL_PANEL_LVDS_CURRENT_350MV,
    /* *<current value 350MV */ /* *<c_ncomment:驱动电流350MV */
    HAL_PANEL_LVDS_CURRENT_400MV,
    /* *<current value 400MV */ /* *<c_ncomment:驱动电流400MV */
    HAL_PANEL_LVDS_CURRENT_450MV,
    /* *<current value 450MV */ /* *<c_ncomment:驱动电流450MV */
    HAL_PANEL_LVDS_CURRENT_500MV,
    /* *<current value 500MV */ /* *<c_ncomment:驱动电流500MV */
    HAL_PANEL_LVDS_CURRENT_BUTT,
} hal_panel_lvds_current;

typedef enum {
    HAL_PANEL_RGBSWAP_RGB = 0,
    HAL_PANEL_RGBSWAP_RBG = 1,
    HAL_PANEL_RGBSWAP_BGR = 2,
    HAL_PANEL_RGBSWAP_BRG = 3,
    HAL_PANEL_RGBSWAP_GRB = 4,
    HAL_PANEL_RGBSWAP_GBR = 5,
    HAL_PANEL_RGBSWAP_BUTT,
} hal_panel_lvds_rgbswap;

typedef enum {
    HAL_PANEL_BITWIDTH_8BIT = 0,
    HAL_PANEL_BITWIDTH_10BIT,
    HAL_PANEL_BITWIDTH_12BIT,
    HAL_PANEL_BITWIDTH_6BIT,
    HAL_PANEL_BITWIDTH_BUTT,
} hal_panel_bitwidth;

/*******************************************************************************
                                VBO struct and enum
*******************************************************************************/
typedef enum {
    HAL_PANEL_VBO_LAN_NUMBER_1 = 0x1,
    HAL_PANEL_VBO_LAN_NUMBER_2 = 0x2,
    HAL_PANEL_VBO_LAN_NUMBER_4 = 0x4,
    HAL_PANEL_VBO_LAN_NUMBER_8 = 0x8,
    HAL_PANEL_VBO_LAN_NUMBER_16 = 0x10,
    HAL_PANEL_VBO_LAN_NUMBER_BUTT,
} hal_panel_vbo_lan_number;

typedef enum {
    HAL_PANEL_VBO_TEST_MODE_OVERTURN_DATA,
    HAL_PANEL_VBO_TEST_MODE_K285PLUS_DATA,
    HAL_PANEL_VBO_TEST_MODE_K285REDUCE_DATA,
    HAL_PANEL_VBO_TEST_MODE_INCREASE_DATA,
    HAL_PANEL_VBO_TEST_MODE_PRBS10_DATA,
    HAL_PANEL_VBO_TEST_MODE_BUTT,
} hal_panel_vbo_test_mode;

typedef enum {
    HAL_PANEL_VBO_CURRENT_300MV,
    HAL_PANEL_VBO_CURRENT_200MV,
    HAL_PANEL_VBO_CURRENT_250MV,
    HAL_PANEL_VBO_CURRENT_350MV,
    HAL_PANEL_VBO_CURRENT_400MV,
    HAL_PANEL_VBO_CURRENT_BUTT,
} hal_panel_vbo_current;

typedef enum {
    HAL_PANEL_VBO_EMP_0DB,
    HAL_PANEL_VBO_EMP_2DB,
    HAL_PANEL_VBO_EMP_3P5DB,
    HAL_PANEL_VBO_EMP_6DB,
    HAL_PANEL_VBO_EMP_BUTT,
} hal_panel_vbo_emp;

typedef enum {
    HAL_PANEL_VBO_RESISTOR_50OHM,
    HAL_PANEL_VBO_RESISTOR_44OHM,
    HAL_PANEL_VBO_RESISTOR_70OHM,
    HAL_PANEL_VBO_RESISTOR_IMPEDANCE,
    HAL_PANEL_VBO_RESISTOR_BUTT,
} hal_panel_vbo_resistor;

typedef enum {
    HAL_PANEL_VBO_BYTE_NUM_3 = 0x3,
    HAL_PANEL_VBO_BYTE_NUM_4 = 0x4,
    HAL_PANEL_VBO_BYTE_NUM_5 = 0x5,
    HAL_PANEL_VBO_BYTE_NUM_BUTT,
} hal_panel_vbo_byte_num;

typedef enum {
    HAL_PANEL_VBO_DATAMODE_30BIT444,
    HAL_PANEL_VBO_DATAMODE_36BIT444,
    HAL_PANEL_VBO_DATAMODE_24BIT444,
    HAL_PANEL_VBO_DATAMODE_18BIT444,
    HAL_PANEL_VBO_DATAMODE_24BIT422,
    HAL_PANEL_VBO_DATAMODE_20BIT422,
    HAL_PANEL_VBO_DATAMODE_16BIT422,
    HAL_PANEL_VBO_DATAMODE_BUTT,
} hal_panel_vbo_datamode;

typedef enum {
    HAL_PANEL_VBO_LANE_NUM0,
    HAL_PANEL_VBO_LANE_NUM1,
    HAL_PANEL_VBO_LANE_NUM2,
    HAL_PANEL_VBO_LANE_NUM3,
    HAL_PANEL_VBO_LANE_NUM4,
    HAL_PANEL_VBO_LANE_NUM5,
    HAL_PANEL_VBO_LANE_NUM6,
    HAL_PANEL_VBO_LANE_NUM7,
    HAL_PANEL_VBO_LANE_NUM8,
    HAL_PANEL_VBO_LANE_NUM9,
    HAL_PANEL_VBO_LANE_NUM10,
    HAL_PANEL_VBO_LANE_NUM11,
    HAL_PANEL_VBO_LANE_NUM12,
    HAL_PANEL_VBO_LANE_NUM13,
    HAL_PANEL_VBO_LANE_NUM14,
    HAL_PANEL_VBO_LANE_NUM15,
    HAL_PANEL_VBO_LANE_NUM_BUTT
} hal_panel_vbo_lane;

typedef enum {
    HAL_PANEL_VBO_PN_SWAP_NONE = 0x0,
    HAL_PANEL_VBO_PN_SWAP_ALL = 0xff,
    HAL_PANEL_VBO_PN_SWAP_BUTT
} hal_panel_vbo_pn_swap;

typedef enum {
    HAL_PANEL_VBO_PARTITONSEL_ONE_OR_TWO,
    HAL_PANEL_VBO_PARTITONSEL_FOUR,
    HAL_PANEL_VBO_PARTITONSEL_BUTT,
} hal_panel_vbo_partitonsel;

/*******************************************************************************
                                dim struct and enum
*******************************************************************************/
typedef enum {
    HAL_PANEL_2DDIM_DEMO_RIGHT_SCREEN = 0x0,
    HAL_PANEL_2DDIM_DEMO_LEFT_SCREEN = 0x1,
    HAL_PANEL_2DDIM_DEMO_TOP_SCREEN = 0x2,
    HAL_PANEL_2DDIM_DEMO_BOTTOM_SCREEN = 0x3,
    HAL_PANEL_2DDIM_DEMO_WHITE_SCREEN = 0x4,
    HAL_PANEL_2DDIM_DEMO_RIGHT_WHITE_SCREEN = 0x5,
    HAL_PANEL_2DDIM_DEMO_LEFT_WHITE_SCREEN = 0x6,
    HAL_PANEL_2DDIM_DEMO_TOP_WHITE_SCREEN = 0x7,
    HAL_PANEL_2DDIM_DEMO_BOTTOM_WHITE_SCREEN = 0x8,
    HAL_PANEL_2DDIM_DEMO_HORSELIGHT = 0x9,
    HAL_PANEL_2DDIM_DEMO_OFF = 0xa,
    HAL_PANEL_2DDIM_DEMO_MODE_BUTT,
} hal_panel_2_ddim_demo_mode;

typedef enum {
    HAL_PANEL_COM_SPI_MOTOROLA,
    HAL_PANEL_COM_SPI_TI,
    HAL_PANEL_COM_SPI_MICROWIRE,
    HAL_PANEL_COM_SPI_FRAME_BUT
} hal_panel_com_spi_frame_mode;

typedef enum {
    HAL_PANEL_COM_SPI0_SEL,
    HAL_PANEL_COM_SPI1_SEL,
    HAL_PANEL_COM_SPI_SEL_BUTT
} hal_panel_com_spi_sel;

typedef enum {
    HAL_PANEL_LDM_SPI0_SEL,
    HAL_PANEL_LDM_SPI1_SEL,
    HAL_PANEL_LDM_SPI_SEL_BUTT
} hal_panel_ldm_spi_sel;

typedef enum {
    HAL_PANEL_SPI_CLK_PHASE_0,
    HAL_PANEL_SPI_CLK_PHASE_180,
    HAL_PANEL_SPI_CLK_PHASE_BUT
} hal_panel_spi_clk_phase;

typedef enum {
    HAL_PANEL_SPI_FIFO_WATERLINE_1,
    HAL_PANEL_SPI_FIFO_WATERLINE_4,
    HAL_PANEL_SPI_FIFO_WATERLINE_8,
    HAL_PANEL_SPI_FIFO_WATERLINE_16,
    HAL_PANEL_SPI_FIFO_WATERLINE_32,
    HAL_PANEL_SPI_FIFO_WATERLINE_64,
    HAL_PANEL_SPI_FIFO_WATERLINE_BUTT,
} hal_panel_spi_fifo_waterline;

/* spi logical bit width enum */
typedef enum {
    HAL_PANEL_COM_SPI_DWIDTH_8BIT = 7,
    HAL_PANEL_COM_SPI_DWIDTH_9BIT,
    HAL_PANEL_COM_SPI_DWIDTH_10BIT,
    HAL_PANEL_COM_SPI_DWIDTH_11BIT,
    HAL_PANEL_COM_SPI_DWIDTH_12BIT,
    HAL_PANEL_COM_SPI_DWIDTH_13BIT,
    HAL_PANEL_COM_SPI_DWIDTH_14BIT,
    HAL_PANEL_COM_SPI_DWIDTH_15BIT,
    HAL_PANEL_COM_SPI_DWIDTH_16BIT,
    HAL_PANEL_COM_SPI_DWIDTH_BUTT
} hal_panel_com_spi_data_width;

/* spi logical bit width enum */
typedef enum {
    HAL_PANEL_LDM_SPI_DWIDTH_8BIT,
    HAL_PANEL_LDM_SPI_DWIDTH_9BIT,
    HAL_PANEL_LDM_SPI_DWIDTH_10BIT,
    HAL_PANEL_LDM_SPI_DWIDTH_11BIT,
    HAL_PANEL_LDM_SPI_DWIDTH_12BIT,
    HAL_PANEL_LDM_SPI_DWIDTH_13BIT,
    HAL_PANEL_LDM_SPI_DWIDTH_14BIT,
    HAL_PANEL_LDM_SPI_DWIDTH_15BIT,
    HAL_PANEL_LDM_SPI_DWIDTH_16BIT,
    HAL_PANEL_LDM_SPI_DWIDTH_BUTT
} hal_panel_ldm_spi_data_width;

/* local_dimming logical bit width enum */
typedef enum {
    HAL_PANEL_LDM_BIT_WIDTH_12BIT,
    HAL_PANEL_LDM_BIT_WIDTH_11BIT,
    HAL_PANEL_LDM_BIT_WIDTH_10BIT,
    HAL_PANEL_LDM_BIT_WIDTH_9BIT,
    HAL_PANEL_LDM_BIT_WIDTH_8BIT,
    HAL_PANEL_LDM_BIT_WIDTH_7BIT,
    HAL_PANEL_LDM_BIT_WIDTH_6BIT,
    HAL_PANEL_LDM_BIT_WIDTH_5BIT,
    HAL_PANEL_LDM_BIT_WIDTH_16BIT,
    HAL_PANEL_LDM_BIT_WIDTH_BUTT,
} hal_panel_ldm_bit_width_sel;

typedef enum {
    HAL_PANEL_SPI_CS_SEL_0,
    HAL_PANEL_SPI_CS_SEL_1,
    HAL_PANEL_SPI_CS_SEL_2,
    HAL_PANEL_SPI_CS_SEL_3,
    HAL_PANEL_SPI_CS_SEL_BUTT
} hal_panel_spi_cs_sel;

/*******************************************************************************
                                PWM struct and enum
*******************************************************************************/
typedef enum {
    HAL_PANEL_PWM_TYPE_PWM,
    HAL_PANEL_PWM_TYPE_PWM1D1,
    HAL_PANEL_PWM_TYPE_PWM1D2,
    HAL_PANEL_PWM_TYPE_PWM1D3,
    HAL_PANEL_PWM_TYPE_LRSYNC,
    HAL_PANEL_PWM_TYPE_LRGLASS,
    HAL_PANEL_PWM_TYPE_BUTT,
} hal_panel_pwm_type;

/*******************************************************************************
                                PHY struct and enum
*******************************************************************************/
typedef enum {
    HAL_PANEL_APHY_MODE_LVDS_MLVDS = 0x00000000,
    HAL_PANEL_APHY_MODE_P2P_LVDS = 0x00000000,
    HAL_PANEL_APHY_MODE_P2P_CML = 0x55555555,
    HAL_PANEL_APHY_MODE_VBOTX = 0x55555555,

    HAL_PANEL_APHY_MODE_MODE_BUT,
} hal_panel_aphy_power_mode;

typedef enum {
    HAL_PANEL_OVER_SAMPLE_ONE_MULTIPLE = 0x0,
    HAL_PANEL_OVER_SAMPLE_TWO_MULTIPLE = 0x1,
    HAL_PANEL_OVER_SAMPLE_FOUR_MULTIPLE = 0x2,
    HAL_PANEL_OVER_SAMPLE_EIGHT_MULTIPLE = 0x3,
    HAL_PANEL_OVER_SAMPLE_BUTT,
} hal_panel_phy_over_sample;

typedef enum {
    HAL_PANEL_PHY_DIV_IN_6_MULTIPLE = 0x8,
    HAL_PANEL_PHY_DIV_IN_12_MULTIPLE = 0x4,
    HAL_PANEL_PHY_DIV_IN_18_MULTIPLE = 0x1,
    HAL_PANEL_PHY_DIV_IN_24_MULTIPLE = 0x5,
    HAL_PANEL_PHY_DIV_IN_27_MULTIPLE = 0x2,
    HAL_PANEL_PHY_DIV_IN_32_MULTIPLE = 0xf,
    HAL_PANEL_PHY_DIV_IN_36_MULTIPLE = 0x3,
    HAL_PANEL_PHY_DIV_IN_48_MULTIPLE = 0x7,
    HAL_PANEL_PHY_DIV_IN_BUTT,
} hal_panel_phy_div_in;

typedef enum {
    HAL_PANEL_PHY_DIV_FEEDBACK_12_MULTIPLE = 0x27,
    HAL_PANEL_PHY_DIV_FEEDBACK_14_MULTIPLE = 0x1a,
    HAL_PANEL_PHY_DIV_FEEDBACK_15_MULTIPLE = 0x19,
    HAL_PANEL_PHY_DIV_FEEDBACK_16_MULTIPLE = 0xa6,
    HAL_PANEL_PHY_DIV_FEEDBACK_20_MULTIPLE = 0x0,
    HAL_PANEL_PHY_DIV_FEEDBACK_24_MULTIPLE = 0xa7,
    HAL_PANEL_PHY_DIV_FEEDBACK_27_MULTIPLE = 0x4f,
    HAL_PANEL_PHY_DIV_FEEDBACK_28_MULTIPLE = 0x2,
    HAL_PANEL_PHY_DIV_FEEDBACK_30_MULTIPLE = 0x1,
    HAL_PANEL_PHY_DIV_FEEDBACK_32_MULTIPLE = 0x1a6,
    HAL_PANEL_PHY_DIV_FEEDBACK_40_MULTIPLE = 0x20,
    HAL_PANEL_PHY_DIV_FEEDBACK_45_MULTIPLE = 0x9,
    HAL_PANEL_PHY_DIV_FEEDBACK_50_MULTIPLE = 0x78,
    HAL_PANEL_PHY_DIV_FEEDBACK_54_MULTIPLE = 0xcf,
    HAL_PANEL_PHY_DIV_FEEDBACK_56_MULTIPLE = 0x22,
    HAL_PANEL_PHY_DIV_FEEDBACK_60_MULTIPLE = 0x21,
    HAL_PANEL_PHY_DIV_FEEDBACK_80_MULTIPLE = 0xa0,
    HAL_PANEL_PHY_DIV_FEEDBACK_BUTT,
} hal_panel_phy_div_feedback;

typedef enum {
    HAL_PANEL_PHY_DIV_ICP_CURRENT_0POINT5UA = 0x0,
    HAL_PANEL_PHY_DIV_ICP_CURRENT_1UA = 0x1,
    HAL_PANEL_PHY_DIV_ICP_CURRENT_1POINT5UA = 0x2,
    HAL_PANEL_PHY_DIV_ICP_CURRENT_2UA = 0x3,
    HAL_PANEL_PHY_DIV_ICP_CURRENT_3UA = 0x5,
    HAL_PANEL_PHY_DIV_ICP_CURRENT_3POINT5UA = 0x6,
    HAL_PANEL_PHY_DIV_ICP_CURRENT_4UA = 0x7,
    HAL_PANEL_PHY_DIV_ICP_CURRENT_4POINT5UA = 0x8,
    HAL_PANEL_PHY_DIV_ICP_CURRENT_5POINT5UA = 0xa,
    HAL_PANEL_PHY_DIV_ICP_CURRENT_6UA = 0xb,
    HAL_PANEL_PHY_DIV_ICP_CURRENT_7UA = 0xd,
    HAL_PANEL_PHY_DIV_ICP_CURRENT_8UA = 0xf,
    HAL_PANEL_PHY_DIV_ICP_CURRENT_BUTT,
} hal_panel_phy_icp_current;

typedef enum {
    HAL_PANEL_INTFTYPE_LVDS,
    HAL_PANEL_INTFTYPE_VBONE,
    HAL_PANEL_INTFTYPE_MINILVDS,
    HAL_PANEL_INTFTYPE_EPI,
    HAL_PANEL_INTFTYPE_ISP,
    HAL_PANEL_INTFTYPE_CEDS,
    HAL_PANEL_INTFTYPE_CHPI,
    HAL_PANEL_INTFTYPE_CMPI,
    HAL_PANEL_INTFTYPE_CSPI,
    HAL_PANEL_INTFTYPE_USIT,
    HAL_PANEL_INTFTYPE_MIPI,
    HAL_PANEL_INTFTYPE_BUTT,
} hal_panel_intftype;

typedef enum {
    HAL_PANEL_PHY_PREDRIVER_1MA = 0x22222222,
    HAL_PANEL_PHY_PREDRIVER_2MA = 0x44444444,
    HAL_PANEL_PHY_PREDRIVER_2POINT5MA = 0x55555555,
    HAL_PANEL_PHY_PREDRIVER_3MA = 0x66666666,
    HAL_PANEL_PHY_PREDRIVER_3POINT5MA = 0x77777777,
    HAL_PANEL_PHY_PREDRIVER_4MA = 0x88888888,
    HAL_PANEL_PHY_PREDRIVER_4POINT5MA = 0x95959595,
    HAL_PANEL_PHY_PREDRIVER_BUTT,
} hal_panel_phy_predriver;

typedef enum {
    HAL_PANEL_PHY_PREEMP_PREDRIVER_0POINT5MA = 0x22222222,
    HAL_PANEL_PHY_PREEMP_PREDRIVER_1MA = 0x44444444,
    HAL_PANEL_PHY_PREEMP_PREDRIVER_1POINT25MA = 0x55555555,
    HAL_PANEL_PHY_PREEMP_PREDRIVER_1POINT5MA = 0x66666666,
    HAL_PANEL_PHY_PREEMP_PREDRIVER_1POINT75MA = 0x77777777,
    HAL_PANEL_PHY_PREEMP_PREDRIVER_2MA = 0x85858585,
    HAL_PANEL_PHY_PREEMP_PREDRIVER_2POINT25MA = 0x95959595,
    HAL_PANEL_PHY_PREEMP_PREDRIVER_BUTT,
} hal_panel_phy_preemp_predriver;

typedef enum {
    HAL_PANEL_PHY_PREDRV_RESIST_LANE0_TO_LANE3_125OHM = 0x618,
    HAL_PANEL_PHY_PREDRV_RESIST_LANE0_TO_LANE3_200OHM = 0xaeb,
    HAL_PANEL_PHY_PREDRV_RESIST_LANE0_TO_LANE3_BUTT,
} hal_panel_phy_predrv_resist_lane0_to_lane3;

typedef enum {
    HAL_PANEL_PHY_PREDRV_RESIST_LANE4_TO_LANE7_125OHM = 0x0,
    HAL_PANEL_PHY_PREDRV_RESIST_LANE4_TO_LANE7_200OHM = 0x6db,
    HAL_PANEL_PHY_PREDRV_RESIST_LANE4_TO_LANE7_BUTT,
} hal_panel_phy_predrv_resist_lane4_to_lane7;

typedef enum {
    HAL_PANEL_PHY_PREDRV_RESIST_LANE8_TO_LANE11_125OHM = 0x0,
    HAL_PANEL_PHY_PREDRV_RESIST_LANE8_TO_LANE11_200OHM = 0x6db,
    HAL_PANEL_PHY_PREDRV_RESIST_LANE8_TO_LANE11_BUTT,
} hal_panel_phy_predrv_resist_lane8_to_lane11;

typedef enum {
    HAL_PANEL_PHY_PREDRV_RESIST_LANE12_TO_LANE15_125OHM = 0x0,
    HAL_PANEL_PHY_PREDRV_RESIST_LANE12_TO_LANE15_200OHM = 0x6db,
    HAL_PANEL_PHY_PREDRV_RESIST_LANE12_TO_LANE15_BUTT,
} hal_panel_phy_predrv_resist_lane12_to_lane15;

typedef enum {
    HAL_PANEL_PHY_PREEMP_RESISTANCE_125OHM = 0x041,
    HAL_PANEL_PHY_PREEMP_RESISTANCE_170OHM = 0x451,
    HAL_PANEL_PHY_PREEMP_RESISTANCE_250OHM = 0x6db,
    HAL_PANEL_PHY_PREEMP_RESISTANCE_500OHM = 0x75d,
    HAL_PANEL_PHY_PREEMP_RESISTANCE_BUTT,
} hal_panel_phy_preemp_resistance;

typedef enum {
    HAL_PANEL_PHY_PREDRIVER_RESISTANCE_62POINT5OHM = 0x041,
    HAL_PANEL_PHY_PREDRIVER_RESISTANCE_85OHM = 0x451,
    HAL_PANEL_PHY_PREDRIVER_RESISTANCE_125OHM = 0x6db,
    HAL_PANEL_PHY_PREDRIVER_RESISTANCE_250OHM = 0x75d,
    HAL_PANEL_PHY_PREDRIVER_RESISTANCE_BUTT,
} hal_panel_phy_predriver_resistance;


typedef enum {
    HAL_PANEL_DPHY_BITWIDTH_LVDS = 7,
    HAL_PANEL_DPHY_BITWIDTH_VBO = 10,
    HAL_PANEL_DPHY_BITWIDTH_MLVDS_6BIT = 12,
    HAL_PANEL_DPHY_BITWIDTH_MLVDS_8BIT = 16,
    HAL_PANEL_DPHY_BITWIDTH_CHPI_OR_USIT_8BIT = 20,
    HAL_PANEL_DPHY_BITWIDTH_EPI_10BIT = 24,
    HAL_PANEL_DPHY_BITWIDTH_ISP_OR_CMPI_8BIT = 27,
    HAL_PANEL_DPHY_BITWIDTH_EPI8BIT_OR_CEDS = 28,
    HAL_PANEL_DPHY_BITWIDTH_BUTT,
} hal_panel_dphy_bitwidth;

typedef enum {
    HAL_PANEL_CURRENT_200MV,
    /* *<current value 200MV */ /* *<c_ncomment:驱动电流200MV */
    HAL_PANEL_CURRENT_250MV,
    /* *<current value 250MV */ /* *<c_ncomment:驱动电流250MV */
    HAL_PANEL_CURRENT_300MV,
    /* *<current value 300MV */ /* *<c_ncomment:驱动电流300MV */
    HAL_PANEL_CURRENT_350MV,
    /* *<current value 350MV */ /* *<c_ncomment:驱动电流350MV */
    HAL_PANEL_CURRENT_400MV,
    /* *<current value 400MV */ /* *<c_ncomment:驱动电流400MV */
    HAL_PANEL_CURRENT_450MV,
    /* *<current value 450MV */ /* *<c_ncomment:驱动电流450MV */
    HAL_PANEL_CURRENT_500MV,
    /* *<current value 500MV */ /* *<c_ncomment:驱动电流500MV */
    HAL_PANEL_CURRENT_BUTT,
} hal_panel_dphy_current;

typedef enum {
    HAL_PANEL_VOLTAGE_550MV,
    /* *<voltage value 550MV */ /* *<c_ncomment: 共模电压550MV */
    HAL_PANEL_VOLTAGE_600MV,
    /* *<voltage value 600MV */ /* *<c_ncomment: 共模电压600MV */
    HAL_PANEL_VOLTAGE_650MV,
    /* *<voltage value 650MV */ /* *<c_ncomment: 共模电压650MV */
    HAL_PANEL_VOLTAGE_700MV,
    /* *<voltage value 700MV */ /* *<c_ncomment: 共模电压700MV */
    HAL_PANEL_VOLTAGE_800MV,
    /* *<voltage value 800MV */ /* *<c_ncomment: 共模电压800MV */
    HAL_PANEL_VOLTAGE_900MV,
    /* *<voltage value 900MV */ /* *<c_ncomment: 共模电压900MV */
    HAL_PANEL_VOLTAGE_1000MV,
    /* *<voltage value 1000MV */ /* *<c_ncomment: 共模电压1000MV */
    HAL_PANEL_VOLTAGE_1100MV,
    /* *<voltage value 1100MV */ /* *<c_ncomment: 共模电压1100MV */
    HAL_PANEL_VOLTAGE_1200MV,
    /* *<voltage value 1200MV */ /* *<c_ncomment: 共模电压1200MV */
    HAL_PANEL_VOLTAGE_1300MV,
    /* *<voltage value 1300MV */ /* *<c_ncomment: 共模电压1300MV */
    HAL_PANEL_VOLTAGE_1400MV,
    /* *<voltage value 1400MV */ /* *<c_ncomment: 共模电压1400MV */
    HAL_PANEL_VOLTAGE_1500MV,
    /* *<voltage value 1500MV */ /* *<c_ncomment: 共模电压1500MV */
    HAL_PANEL_VOLTAGE_BUTT,
    /* *<invalid value */ /* *<c_ncomment:非法边界值 */
} hal_panel_voltage;

typedef enum {
    HAL_PANEL_PANEL_EMP_0DB,
    /* *<pre-emphasis is 0DB */ /* *<c_ncomment:预加重为0DB */
    HAL_PANEL_PANEL_EMP_1DB,
    /* *<pre-emphasis is 1B */ /* *<c_ncomment:预加重为1DB */
    HAL_PANEL_PANEL_EMP_2DB,
    /* *<pre-emphasis is 2DB */ /* *<c_ncomment:预加重为2DB */
    HAL_PANEL_PANEL_EMP_3DB,
    /* *<pre-emphasis is 3DB */ /* *<c_ncomment:预加重为3DB */
    HAL_PANEL_PANEL_EMP_4DB,
    /* *<pre-emphasis is 4DB */ /* *<c_ncomment:预加重为4DB */
    HAL_PANEL_PANEL_EMP_5DB,
    /* *<pre-emphasis is 5DB */ /* *<c_ncomment:预加重为5DB */
    HAL_PANEL_PANEL_EMP_6DB,
    /* *<pre-emphasis is 6DB */ /* *<c_ncomment:预加重为6DB */
    HAL_PANEL_PANEL_EMP_7DB,
    /* *<pre-emphasis is 7DB */ /* *<c_ncomment:预加重为7DB */
    HAL_PANEL_PANEL_EMP_8DB,
    /* *<pre-emphasis is 8DB */ /* *<c_ncomment:预加重为8DB */
    HAL_PANEL_PANEL_EMP_9DB,
    /* *<pre-emphasis is 9DB */ /* *<c_ncomment:预加重为9DB */
    HAL_PANEL_PANEL_EMP_BUTT,
    /* *<invalid value */ /* *<c_ncomment:非法边界值 */
} hal_panel_panel_emp;

typedef struct {
    hal_panel_phy_over_sample aphy_over_sample;
    hal_panel_phy_over_sample dphy_over_sample;
    hal_panel_phy_div_in aphy_div_in;
    hal_panel_phy_div_feedback aphy_div_fb;
    hal_panel_phy_icp_current aphy_icp_current;
} hal_combo_phy_clkcfg;

typedef struct {
    /* condition */
    hi_u32 panel_width;
    hi_u32 panel_height;
    hi_drv_panel_intf_type intf_type;
    hi_drv_panel_bit_depth panel_bit_depth;
    hi_drv_panel_link_type panel_link_type;
    panel_port_mode port_mode;
    panel_pair_mode pair_mode;
    hi_u32 min_pixel_clk;
    hi_u32 max_pixel_clk;
    /* result */
    hal_combo_phy_clkcfg phy_clk_cfg;
} hal_combo_phy_clkform;

typedef enum {
    HAL_PANEL_COMBOPHY_EYESATT_SWING,
    HAL_PANEL_COMBOPHY_EYESATT_EMPHASIS,
    HAL_PANEL_COMBOPHY_EYESATT_BUTT,
} hal_panel_combophy_eyesatt;
/*******************************************************************************
                                MLVDS struct and enum
*******************************************************************************/
typedef struct {
    hi_u32 link0_ch1_swap;
    hi_u32 link0_ch2_swap;
    hi_u32 link0_ch3_swap;
    hi_u32 link0_ch4_swap;
    hi_u32 link0_ch5_swap;
    hi_u32 link0_ch6_swap;
} hal_mlvds_1_port_pairswap;

typedef struct {
    hi_u32 link0_ch1_swap;
    hi_u32 link0_ch2_swap;
    hi_u32 link0_ch3_swap;

    hi_u32 link1_ch1_swap;
    hi_u32 link1_ch2_swap;
    hi_u32 link1_ch3_swap;
} hal_mlvds_2_port_pairswap;

typedef enum {
    VDP_DITHER_MODE_TYP,
    VDP_DITHER_MODE_TYP1,
    VDP_DITHER_MODE_RAND,
    VDP_DITHER_MODE_MAX,
    VDP_DITHER_MODE_MIN,
    VDP_DITHER_MODE_ZERO,
    VDP_DITHER_MODE_BUTT
} vdp_disp_dither_mode;

typedef enum {
    DITHER_MODE_10BIT,
    DITHER_MODE_8BIT,
    DITHER_MODE_BUTT
} dither_mode;

typedef enum {
    DITHER_OWIDTH_MODE_5BIT,
    DITHER_OWIDTH_MODE_6BIT,
    DITHER_OWIDTH_MODE_7BIT,
    DITHER_OWIDTH_MODE_8BIT,
    DITHER_OWIDTH_MODE_9BIT,
    DITHER_OWIDTH_MODE_10BIT,
    DITHER_OWIDTH_MODE_BUTT
} dither_owidth_mode;

typedef enum {
    DITHER_IWIDTH_MODE_8BIT,
    DITHER_IWIDTH_MODE_9BIT,
    DITHER_IWIDTH_MODE_10BIT,
    DITHER_IWIDTH_MODE_11BIT,
    DITHER_IWIDTH_MODE_12BIT,
    DITHER_IWIDTH_MODE_BUTT
} dither_iwidth_mode;
typedef enum {
    DITHER_DOMAIN_MODE_SPACE,
    DITHER_DOMAIN_MODE_TIME,
    DITHER_DOMAIN_MODE_BUTT
} dither_domain_mode;

typedef enum {
    DITHER_TAP_MODE_7,
    DITHER_TAP_MODE_15,
    DITHER_TAP_MODE_23,
    DITHER_TAP_MODE_31,
    DITHER_TAP_MODE_BUTT
} dither_tap_mode;
typedef enum {
    DITHER_IO_MODE_12_10 = 1,
    DITHER_IO_MODE_12_8 = 2,
    DITHER_IO_MODE_10_8 = 3,
    DITHER_IO_MODE_10_6 = 4,
    DITHER_IO_MODE_10_9_6 = 5,
    DITHER_IO_MODE_10_8_6 = 6,
    DITHER_IO_MODE_BUTT
} dither_io_mode;

typedef enum {
    COMBO_DPHY_CKSEL_CLK_P2P_PACK,
    COMBO_DPHY_CKSEL_CLK_MLVDS_PPC,
    COMBO_DPHY_CKSEL_CLK_LVDS_PPC,
    COMBO_DPHY_CKSEL_CLK_VBO_PARA,
    COMBO_DPHY_CKSEL_BUTT
} combo_dphy_cksel;

typedef enum {
    COMBO_APHY_REF_CKSEL_CLK_P2P_TCON,
    COMBO_APHY_REF_CKSEL_CLK_MLVDS_PPC,
    COMBO_APHY_REF_CKSEL_CLK_LVDS_PPC,
    COMBO_APHY_REF_CKSEL_CLK_VBO_PPC,
    COMBO_APHY_REF_CKSEL_BUTT
} combo_aphy_ref_cksel;

typedef enum  {
    COMBO_APHY_REG_PLL_CKSEL_CLK_VO_HD0_INI,
    COMBO_APHY_REG_PLL_CKSEL_CLK_VO_HD0_FIX_INI,
    COMBO_APHY_REG_PLL_CKSEL_PRIV_MUX_SEL,
    COMBO_APHY_REG_PLL_CKSEL_BUTT
} combo_aphy_ref_pll_cksel;

typedef struct {
    hi_u32 dither_en;
    hi_u32 dither_mode;
    hi_u32 dither_round;
    hi_u32 dither_round_unlim;
    hi_u32 i_data_width_dither;
    hi_u32 o_data_width_dither;
    hi_u32 dither_domain_mode;
    hi_u32 dither_tap_mode;
    hi_u32 dither_sed_y0;
    hi_u32 dither_sed_u0;
    hi_u32 dither_sed_v0;
    hi_u32 dither_sed_w0;
    hi_u32 dither_sed_y1;
    hi_u32 dither_sed_u1;
    hi_u32 dither_sed_v1;
    hi_u32 dither_sed_w1;
    hi_u32 dither_sed_y2;
    hi_u32 dither_sed_u2;
    hi_u32 dither_sed_v2;
    hi_u32 dither_sed_w2;
    hi_u32 dither_sed_y3;
    hi_u32 dither_sed_u3;
    hi_u32 dither_sed_v3;
    hi_u32 dither_sed_w3;
    hi_u32 dither_thr_max;
    hi_u32 dither_thr_min;
    dither_io_mode dither_io_mode;
} disp_dither_cfg;

typedef enum {
    HAL_PANEL_SORTTYPE_8LINK,
    HAL_PANEL_SORTTYPE_16LINK,
    HAL_PANEL_SORTTYPE_COPY,
    HAL_PANLE_SORTTYPE_DIV_ONE,
    HAL_PANLE_SORTTYPE_DIV_TWO_FOUR,
    HAL_PANEL_SORTTYPE_BUTT,
} hal_panel_sorttype;

hi_u32 panel_reg_read(volatile hi_u32 *a);

hi_void panel_reg_write(volatile hi_u32 *a, hi_u32 b);

hi_void panel_reg_write_mask(hi_u32 addr, hi_u32 val, hi_u32 mask);

hi_s32 panel_reg_init(hi_void);

hi_void panel_reg_de_init(hi_void);


volatile s_dphy_regs_type* panel_dphy_reg(hi_void);
volatile s_vbotx_regs_type* panel_vbotx_reg(hi_void);
volatile s_panel_vdp_regs_type* panel_vdp_reg(hi_void);

hi_void panel_tcon_reg_cfg_check(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

