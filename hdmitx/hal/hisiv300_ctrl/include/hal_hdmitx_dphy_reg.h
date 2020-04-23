/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: header file of hdmitx dphy reg
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#ifndef __HAL_HDMITX_DPHY_REG__H__
#define __HAL_HDMITX_DPHY_REG__H__

/* tx_dphy_reg module field info */
#define REG_T2GPPORT0           0x0
#define REG_GPPORT0             0xffff

#define REG_T2GPPORT1           0x4
#define REG_GPPORT1             0xffff

#define REG_PHY_CSEN            0x8
#define REG_STB_CS_EN_M         0xffff

#define REG_PHY_WR              0xC
#define REG_STB_WEN_M           (0x1 << 0)

#define REG_RESETN              0x10
#define REG_STB_RESETN_M        (0x1 << 0)

#define REG_PHY_ADDR            0x14
#define REG_STB_ADDR_M          0xf

#define REG_PHY_WDATA           0x18
#define REG_STB_WDATA_M         0xff

#define REG_PHY_RDATA           0x1C
#define REG_STB_RDATA_M         0xff

#define REG_ZCALREG             0x20
#define REG_RXSENSE             (0xf << 8)
#define REG_ZCALSUB             (0x3 << 6)
#define REG_ZCALDONE            (0x1 << 5)
#define REG_ZCAL                (0x1f << 0)

#define REG_ZCALCLK             0x24
#define REG_CALCLK_M            (0x1 << 0)

#define REG_SHORTDET            0x28
#define REG_CLKSHORTDET         (0x1 << 3)
#define REG_C2SHORTDET          (0x1 << 2)
#define REG_C1SHORTDET          (0x1 << 1)
#define REG_C0SHORTDET          (0x1 << 0)

#define REG_DET                 0x2C
#define REG_T2_LKVDETHIGH       (0x1 << 16)
#define REG_T2_LKVDETLOW        (0x1 << 15)
#define REG_T2_PLLLKVDET2       (0x1 << 14)
#define REG_T2_PLLLKCDET        (0x1 << 13)
#define REG_T2_PLLLKVDETL       (0x1 << 12)

#define REG_FDSRCPARAM              0x30

#define REG_FDSRCFREQ               0x34

#define REG_FDSRCRES                0x38
#define REG_SRC_CNT_OUT             (0xfffff << 4)
#define REG_SRC_DET_STAT            0xf

#define REG_CTSET0                  0x3C
#define REG_CTSET0_UNUSED_M         (0x3 << 2)
#define REG_I_RUN_M                 (0x1 << 1)
#define REG_I_ENABLE_M              (0x1 << 0)

#define REG_CTSET1                  0x40
#define REG_CTSET1_UNUSED           (0x3ffff << 14)
#define REG_I_MPLL_CTLCK            (0x1 << 13)
#define REG_I_MPLL_DIVN             (0x7 << 10)
#define REG_I_MPLL_FCON             (0x3ff << 0)

#define REG_FCCNTR0                 0x48
#define REG_I_REF_CNT_LEN_M         (0xff << 24)
#define REG_I_VCO_END_WAIT_LEN_M    (0xff << 16)
#define REG_I_VCO_ST_WAIT_LEN_M     (0xff << 8)
#define REG_I_DECI_CNT_LEN_M        (0xff << 0)


#define REG_FCOPT                   0x4C
#define REG_FCOPT_UNUSED_M          (0x3 << 14)
#define REG_I_DECI_TRY_SEL_M        (0x1 << 13)
#define REG_I_CT_IDX_SEL_M          (0x1 << 12)
#define REG_FCOPT_UNUSED_2_M        (0x7 << 9)
#define REG_I_CT_EN_M               (0x1 << 8)
#define REG_FCOPT_UNUSED_1_M        (0xff << 4)
#define REG_I_CT_MODE_M             (0x3 << 2)
#define REG_I_CLKDET_SEL_M          (0x1 << 1)
#define REG_I_CT_SEL_M              (0x1 << 0)

#define REG_FCSTAT                  0x50
#define REG_FCON_INIT_M             (0x3ff << 20)
#define REG_FCSTAT_UNUSED_3_M       (0x3 << 18)
#define REG_CONFIN_STAT_M           (0xf << 12)
#define REG_FCSTAT_UNUSED_2_M       (0x3 << 10)
#define REG_PLLVCO_CLK_STAT_M       (0x1 << 9)
#define REG_REF_CLK_STAT_M          (0x1 << 8)
#define REG_FCSTAT_UNUSED_1_M       (0x1 << 7)
#define REG_DIVN_M                  (0x7 << 4)
#define REG_ERROR_M                 (0x1 << 3)
#define REG_DONE_M                  (0x1 << 2)
#define REG_BUSY_M                  (0x1 << 1)
#define REG_CLK_OK_M                (0x1 << 0)

#define REG_FCCNTVAL0               0x54
#define REG_O_CNT_REF_M             (0xffff << 0)

#define REG_FCCNTVAL1               0x58
#define REG_O_CNT_MPLL_M            (0xffff << 0)

#define REG_FCRESVAL                0x5C
#define REG_O_FCON_MPLL             (0x3ff << 4)
#define REG_FCRESVAL_UNUSED         (0x1 << 3)
#define REG_O_DIVN_MPLL             (0x7 << 0)

#define REG_FCDSTEPSET              0x60
#define REG_UP_SAMPLER_RATIO_SEL_M  (0x1 << 4)
#define REG_FCDSTEPSET_UNUSED_M     (0x1 << 3)
#define REG_P_DIVN_H20_M            (0x7 << 0)

#define REG_FCDSTEPTH               0x64
#define REG_I_DECI_SEL              (0x1 << 1)
#define REG_I_H2_SEL                (0x1 << 0)

#define REG_FCDSTEPTH0              0x68
#define REG_I_DECI4X_TH             (0xffff << 16)
#define REG_I_DECI2X_TH             (0xffff << 0)

#define REG_FCDSTEPTH1              0x6C
#define REG_I_DECI16X_TH            (0xffff << 16)
#define REG_I_DECI8X_TH             (0xffff << 0)

#define REG_FCCNTR1                 0x70
#define REG_I_REF_CNT_M             (0xffff << 0)

#define REG_FCCONTINSET0            0x74
#define REG_P_CONTIN_UPD_STEP_M     (0xf << 4)
#define REG_FCCONTINSET0_UNUSED_M   (0x1 << 3)
#define REG_P_CONTIN_UPD_POL_M      (0x1 << 2)
#define REG_P_CONTIN_UPD_OPT_M      (0x1 << 1)
#define REG_P_CONTIN_UPD_EN_M       (0x1 << 0)

#define REG_FCCONTINSET1            0x78
#define REG_P_CONTIN_UPD_TIME_M     (0xf << 28)
#define REG_P_CONTIN_UPD_RATE_M     (0xfffff << 0)

#define REG_FCCONTINSET2            0x7C
#define REG_P_CONTIN_UPD_TH_UP_M    (0x3ff << 12)
#define REG_FCCONTINSET2_UNUSED_M   (0x3 << 10)
#define REG_P_CONTIN_UPD_TH_DN_M    (0x3ff << 0)

#define REG_FDIVSET0                0x90
#define REG_EN_SDM_M                (0x1 << 3)
#define REG_EN_MOD_M                (0x1 << 2)
#define REG_EN_CTRL_M               (0x1 << 1)
#define REG_INIT_M                  (0x1 << 0)

#define REG_FDIVSET1                0x94
#define REG_STEP_N                  (0xffff << 16)
#define REG_STEP_T                  (0xff << 8)
#define REG_STEP_D                  (0xff << 0)

#define REG_FDIVSET2                0x98
#define REG_DN                      (0x1 << 1)
#define REG_UP                      (0x1 << 0)

#define REG_FDIVSET3                0x9C
#define REG_MOD_N_M                 (0xffff << 16)
#define REG_MOD_T_M                 (0xff << 8)
#define REG_MOD_LEN_M               (0xff << 0)


#define REG_FDIVSET4                0xA0
#define REG_MOD_D_M                 (0xffff << 0)

#define REG_FDIVSET5                0xA4
#define REG_FDIVSET5_UNUSED         (0x3 << 2)
#define REG_P_MOD_DN                (0x1 << 0)
#define REG_P_MOD_UP                (0x1 << 0)

#define REG_FDIVSTAT0               0xA8
#define REG_STC_CNT                 (0xffff << 4)
#define REG_STC_UN                  (0x1 << 3)
#define REG_STC_OV                  (0x1 << 2)
#define REG_STC_DIR                 (0x1 << 1)
#define REG_STC_RUN                 (0x1 << 0)

#define REG_FDIVSTAT1               0xAC
#define REG_I_FDIV_IN_N_M           (0xff << 24)
#define REG_I_FDIV_IN_X_M           (0xffffff << 0)

#define REG_FDIVSTAT2               0xB0
#define REG_DIV_OUT_M               (0xffffff << 0)

#define REG_FDIVSTAT3               0xB4
#define REG_DIV_SDM                 (0xffff << 0)

#define REG_FDIVSTAT4               0xB8
#define REG_STM_CNT                 (0xffff << 8)
#define REG_FDIVSTAT4_UNUSED        (0x7 << 5)
#define REG_STM_UN                  (0x1 << 4)
#define REG_STM_OV                  (0x1 << 3)
#define REG_STM_PH                  (0x3 << 1)
#define REG_STM_RUN                 (0x1 << 0)

#define REG_FDIVMANUAL              0xBC
#define REG_I_VIC_M                 (0xff << 16)
#define REG_I_DC_SEL_M              (0x3 << 14)
#define REG_I_REF_CNT_DIV_M         (0x3 << 12)
#define REG_I_MDIV_M                (0xf << 8)
#define REG_FDIVMANUAL_UNUSED_M     (0x1 << 7)
#define REG_I_DIVN_M                (0x7 << 4)
#define REG_I_MANUAL_EN_M           (0xf << 0)

#define REG_REFCLKSEL               0xC0
#define REG_P_PR_ENC_VAL            (0x3 << 6)
#define REG_REFCLKSEL_UNUSED_2      (0x1 << 4)
#define REG_T2_PIXELCLKSEL          (0x1 << 4)
#define REG_REFCLKSEL_UNUSED_1      (0x1 << 3)
#define REG_I_REF_CLK_SEL_M         (0x1 << 2)
#define REG_T2_REFCLKSEL2           (0x1 << 1)
#define REG_T2_REFCLKSEL            (0x1 << 0)

#define REG_FDPLLPARAM              0x100
#define REG_FDPLLPARAM_UNUSED       (0x7 << 17)
#define REG_PLL_ENABLE              (0x1 << 16)
#define REG_PLL_LOCK_CNT            (0xff << 8)
#define REG_PLL_LOCK_VAL            (0xff << 0)

#define REG_FDPLLFREQ               0x104
#define REG_PLL_FREQ_EXT            (0xffff << 8)
#define REG_FDPLLFREQ_UNUSED_2      (0x3 << 6)
#define REG_PLL_FREQ_OPT            (0x3 << 4)
#define REG_FDPLLFREQ_UNUSED_1      (0x1 << 3)
#define REG_PLL_CNT_OPT             (0x7 << 0)

#define REG_FDPLLRES                0x108
#define REG_PLL_CNT_OUT             (0xfffff << 4)
#define REG_PLL_DET_STAT            (0xf << 0)

#define REG_FCGSET                  0x120
#define REG_P_FCG_LOCK_EN_M         (0x1 << 3)
#define REG_P_FCG_DITHER_EN_M       (0x1 << 2)
#define REG_P_FCG_DIF_EN_M          (0x1 << 1)
#define REG_P_FCG_EN_M              (0x1 << 0)

#define REG_FCGCNT                  0x124
#define REG_P_CNT1_TARGET_M         (0xffff << 16)
#define REG_P_TMDS_CNT_VAL_M        (0xffff << 0)

#define REG_FCGPARAM                0x128
#define REG_P_LOCK_MODE_M           (0x1 << 22)
#define REG_P_KI_M                  (0x3f << 16)
#define REG_P_LOCK_TH_M             (0xff << 8)
#define REG_P_LOCK_CNT_M            (0xff << 0)

#define REG_FCGSTATE                0x12C
#define REG_O_DLF_UN_M              (0x1 << 2)
#define REG_O_DLF_OV_M              (0x1 << 1)
#define REG_O_DLF_LOCK_M            (0x1 << 0)

#define REG_TXTELOSET               0x200
#define REG_P_TEST_4TO1_MUX_SEL3    (0x3 << 26)
#define REG_P_TEST_4TO1_MUX_SEL2    (0x3 << 24)
#define REG_P_TEST_4TO1_MUX_SEL1    (0x3 << 22)
#define REG_P_TEST_4TO1_MUX_SEL0    (0x3 << 20)
#define REG_P_CH_TEST_EN            (0x1 << 19)
#define REG_P_TEST_PAT_TYPE         (0x7 << 16)
#define REG_P_PRBS_CLR_H21          (0xf << 12)
#define REG_P_CH_EN_H21             (0xf << 8)
#define REG_P_PRBS_CLR_H20          (0xf << 4)
#define REG_P_CH_EN_H20             (0xf << 0)

#define REG_TXTELOCONT0             0x204
#define REG_P_TEST_PAT_CH1_L        (0x3ff << 20)
#define REG_P_TEST_PAT_CH0          (0xfffff << 0)

#define REG_TXTELOCONT1             0x208
#define REG_P_TEST_PAT_CH2          (0x3ff << 10)
#define REG_P_TEST_PAT_CH1_H        (0x3ff << 0)

#define REG_TXTELOCONT2             0x20C
#define REG_P_TEST_PAT_CH3          (0xfffff << 0)

#define REG_TXFIFOSET0              0x210
#define REG_TXFIFOSET0_UNUSED_M     (0x3ff << 2)
#define REG_P_ENABLE_H20_M          (0x1 << 1)
#define REG_P_PR_EN_H20_M           (0x1 << 0)

#define REG_TXFIFOSET1              0x214
#define REG_P_CH_SEL1_H20           (0xff << 0)
#define REG_P_DATA_SWAP1_H20        (0xf << 0)
#define REG_P_POL_INV1_H20          (0xf << 0)
#define REG_P_CH_SEL0_H20           (0xff << 0)
#define REG_P_DATA_SWAP0_H20        (0xf << 4)
#define REG_P_POL_INV0_H20          (0xf << 0)

#define REG_REG_TXFIFOSTAT0         0x218
#define REG_TXFIFOSTAT0_UNUSED      (0xfff << 12)
#define REG_O_PR_FIFO_STATE_H20     (0xfff << 0)

#define REG_TXFIFOSTAT1             0x21C
#define REG_TXFIFOSTAT1_UNUSED_1    (0xfff << 12)
#define REG_TXFIFOSTAT1_UNUSED_0    (0xfff << 0)

#define REG_TXFIFOSTAT2             0x220
#define REG_TXFIFOSTAT2_UNUSED      (0xfff << 12)
#define REG_O_TX_FIFO_STATE_H20     (0xfff << 0)

#define REG_TXFIFOSTAT3             0x224
#define REG_TXFIFOSTAT3_UNUSED_1    (0xfff << 12)
#define REG_TXFIFOSTAT3_UNUSED_0    (0xfff << 0)

#define REG_DATACLKINV              0x228
#define REG_P_DATACLKINV            (0x1 << 0)

#define REG_TXDATAOUTSEL            0x22C
#define REG_P_CH_OUT_SEL_M          (0x1 << 0)

#define REG_HDMI_MODE               0x230
#define REG_REG_HDMI_MODE_EN_M      (0x1 << 0)

#define REG_CLK_DATA_1              0x234
#define REG_REG_SW_CLK_EN           (0x1 << 20)
#define REG_REG_CLK_DATA_PHASE1     (0x3ff << 10)
#define REG_REG_CLK_DATA_PHASE0     (0x3ff << 0)

#define REG_CLK_DATA_2              0x238
#define REG_REG_CLK_DATA_PHASE3     (0x3ff << 10)
#define REG_REG_CLK_DATA_PHASE2     (0x3ff << 0)

#define REG_CFG18TO20               0x23C
#define REG_REG_STATUS_WRST         (0x1 << 4)
#define REG_REG_STATUS_RRST         (0x1 << 3)
#define REG_REG_RD_BYPASS           (0x1 << 2)
#define REG_REG_18TO20_FIFO_WR_RST  (0x1 << 1)
#define REG_REG_18TO20_FIFO_RD_RST  (0x1 << 0)

#define REG_FIFOSTAT18TO20          0x240
#define REG_AFULL_STATUS            (0x1 << 3)
#define REG_FULL_STATUS             (0x1 << 2)
#define REG_AEMPTY_STATUS           (0x1 << 1)
#define REG_EMPTY_STATUS            (0x1 << 0)

#define REG_HSSET                   0x260
#define REG_P_HSSET_M               (0x3 << 0)

#define REG_HSRXSENSE               0x264
#define REG_W_HSRXSENSE             (0x3 << 0)

#define REG_HSFIFOSTAT              0x268
#define REG_O_FIFO_STATE_HS         (0xfff << 0)

#define REG_INTRSTAT                    0x300
#define REG_CT_FCON_INTR                (0x1 << 8)
#define REG_LKVDETLOW_INTR              (0x1 << 7)
#define REG_LKVDETHIGH_INTR             (0x1 << 6)
#define REG_TRINSMITTER_FIFO_FULL_INTR  (0x1 << 5)
#define REG_TRINSMITTER_FIFO_EMPTY_INTR (0x1 << 4)
#define REG_UP_SAMPLE_FIFO_FULL_INTR    (0x1 << 3)
#define REG_UP_SAMPLE_FIFO_EMPTY_INTR   (0x1 << 2)
#define REG_HS_FIFO_FULL_INTR           (0x1 << 1)
#define REG_HS_FIFO_EMPTY_INTR          (0x1 << 0)

#define REG_INTRMASK                            0x304
#define REG_CT_FCON_INTR_MASK                   (0x1 << 8)
#define REG_LKVDETLOW_INTR_MASK                 (0x1 << 7)
#define REG_LKVDETHIGH_INTR_MASK                (0x1 << 6)
#define REG_TRINSMITTER_FIFO_FULL_INTR_MASK     (0x1 << 5)
#define REG_TRINSMITTER_FIFO_EMPTY_INTR_MASK    (0x1 << 4)
#define REG_UP_SAMPLE_FIFO_FULL_INTR_MASK       (0x1 << 3)
#define REG_UP_SAMPLE_FIFO_EMPTY_INTR_MASK      (0x1 << 2)
#define REG_HS_FIFO_FULL_INTR_MASK              (0x1 << 1)
#define REG_HS_FIFO_EMPTY_INTR_MASK             (0x1 << 0)

#define REG_INTRSET                             0x308
#define REG_LKVDETLOW_INTR_EN                   (0x1 << 31)
#define REG_LKVDETLOW_TRIGER_TYPE               (0x7 << 28)
#define REG_LKVDETHIGH_INTR_EN                  (0x1 << 27)
#define REG_LKVDETHIGH_TRIGER_TYPE              (0x7 << 24)
#define REG_TRANSMITTER_FIFO_FULL_INTR_EN       (0x1 << 23)
#define REG_TRANSMITTER_FIFO_FULL_TRIGER_TYPE   (0x7 << 20)
#define REG_TRANSMITTER_FIFO_EMPTY_INTR_EN      (0x1 << 19)
#define REG_TRANSMITTER_FIFO_EMPTY_TRIGER_TYPE  (0x7 << 16)
#define REG_UP_SAMPLE_FIFO_FULL_INTR_EN         (0x1 << 15)
#define REG_UP_SAMPLE_FIFO_FULL_TRIGER_TYPE     (0x7 << 12)
#define REG_UP_SAMPLE_FIFO_EMPTY_INTR_EN        (0x1 << 11)
#define REG_UP_SAMPLE_FIFO_EMPTY_TRIGER_TYPE    (0x7 << 8)
#define REG_HS_FIFO_FULL_INTR_EN                (0x1 << 7)
#define REG_HS_FIFO_FULL_TRIGER_TYPE            (0x7 << 4)
#define REG_HS_FIFO_EMPTY_INTR_EN               (0x1 << 3)
#define REG_HS_FIFO_EMPTY_TRIGER_TYPE           (0x7 << 0)

#define REG_CLKSET                  0x30C
#define REG_P_MODCLK_SEL_M          (1 << 6)

#define REG_SWRESET                 (0x310)
#define REG_GLOBAL_RESET_M          (0x1 << 31)
#define REG_DAC_CLOCK_GAT_M         (0x1 << 14)

#define REG_GLUESET0                0x314

#define REG_GLUESET1                0x318
#define REG_GLUESET1_UNUSED_2       (0xfff << 20)
#define REG_CLK_SEL                 (0xf << 16)
#define REG_CLK11_DIV               (0xf << 12)
#define REG_CLK10_DIV               (0xf << 8)
#define REG_GLUESET1_UNUSED_1       (0xf << 4)
#define REG_CLK8_DIV                (0xf << 0)

#define REG_CT_INTRSET              0x31C
#define REG_CT_FCON_INTR_EN         (0x1 << 3)
#define REG_CT_FCON_TRIGER_TYPE     (0x7 << 0)

#define REG_HW_INFO                 0x400
#define REG_INFO_M                  (0xffffffff << 0)

#define REG_HW_VERS                 0x404
#define REG_HW_VERS_UNUSED_2        (0x3ffffff << 6)
#define REG_HDMI21_COMPLIANCE       (0x1 << 5)
#define REG_HDMI20_COMPLIANCE       (0x1 << 4)
#define REG_HW_VERS_UNUSED_1        (0xF << 0)

#define REG_RASMODE                 0x420
#define REG_RAS_MODE_M              (0xffffffff << 0)

#define REG_RFSMODE                 0x424
#define REG_RFS_MODE_M              (0xffffffff << 0)

#define REG_RFTMODE                 0x428
#define REG_RFT_MODE_M              (0xffffffff << 0)

#define REG_FFE3_CFG                    0x500
#define REG_CFG_FFE3_DRV_POST2_M        (0x3f << 24)
#define REG_CFG_FFE3_DRV_POST1_M        (0x3f << 16)
#define REG_CFG_FFE3_DRV_M_M            (0x3f << 8)
#define REG_CFG_FFE3_DRV_PRE_M          (0x3f << 0)

#define REG_FFE2_CFG                    0x504
#define REG_CFG_FFE2_DRV_POST2_M        (0x3f << 24)
#define REG_CFG_FFE2_DRV_POST1_M        (0x3f << 16)
#define REG_CFG_FFE2_DRV_M_M            (0x3f << 8)
#define REG_CFG_FFE2_DRV_PRE_M          (0x3f << 0)

#define REG_FFE1_CFG                    0x508
#define REG_CFG_FFE1_DRV_POST2_M        (0x3f << 24)
#define REG_CFG_FFE1_DRV_POST1_M        (0x3f << 16)
#define REG_CFG_FFE1_DRV_M_M            (0x3f << 8)
#define REG_CFG_FFE1_DRV_PRE_M          (0x3f << 0)

#define REG_FFE0_CFG                    0x50C
#define REG_CFG_FFE0_DRV_POST2_M        (0x3f << 24)
#define REG_CFG_FFE0_DRV_POST1_M        (0x3f << 16)
#define REG_CFG_FFE0_DRV_M_M            (0x3f << 8)
#define REG_CFG_FFE0_DRV_PRE_M          (0x3f << 0)

#define REG_TMDS_DRV_CFG_CH0            0x510
#define REG_CFG_HDMI_FFE_SEL_M          (0x1 << 30)
#define REG_CFG_DRV_POST2_CH0_M         (0x3f << 24)
#define REG_CFG_DRV_POST1_CH0_M         (0x3f << 16)
#define REG_CFG_DRV_M_CH0_M             (0x3f << 8)
#define REG_CFG_DRV_PRE_CH0_M           (0x3f << 0)

#define REG_TMDS_DRV_CFG_CH1            0x514
#define REG_CFG_DRV_POST2_CH1_M         (0x3f << 24)
#define REG_CFG_DRV_POST1_CH1_M         (0x3f << 16)
#define REG_CFG_DRV_M_CH1_M             (0x3f << 8)
#define REG_CFG_DRV_PRE_CH1_M           (0x3f << 0)

#define REG_TMDS_DRV_CFG_CH2            0x518
#define REG_CFG_DRV_POST2_CH2_M         (0x3f << 24)
#define REG_CFG_DRV_POST1_CH2_M         (0x3f << 16)
#define REG_CFG_DRV_M_CH2_M             (0x3f << 8)
#define REG_CFG_DRV_PRE_CH2_M           (0x3f << 0)

#define REG_TMDS_DRV_CFG_CH3            0x51C
#define REG_CFG_DRV_POST2_CH3_M         (0x3f << 24)
#define REG_CFG_DRV_POST1_CH3_M         (0x3f << 16)
#define REG_CFG_DRV_M_CH3_M             (0x3f << 8)
#define REG_CFG_DRV_PRE_CH3_M           (0x3f << 0)

#define REG_HDMI_OE_CFG                 0x520
#define REG_CFG_OE_SYNC_EN_M            (1 << 4)
#define REG_CFG_HDMI_OE_CH3_M           (1 << 3)
#define REG_CFG_HDMI_OE_CH2_M           (1 << 2)
#define REG_CFG_HDMI_OE_CH1_M           (1 << 1)
#define REG_CFG_HDMI_OE_CH0_M           (1 << 0)

#define REG_DCC_QEC_CFG0                0x524
#define REG_CFG_DCC_INIT_VALUE          (0x3f << 24)
#define REG_CFG_CHECK_DURATION          (0x3f << 16)
#define REG_CFG_WORK_WAIT_TIME          (0xfff << 4)
#define REG_CFG_WORK_WAIT_SEL           (1 << 3)
#define REG_CFG_ADJ_MODE                (3 << 0)

#define REG_DCC_QEC_CFG1                0x528
#define REG_CFG_CHANGE_INTERVAL         (0xf << 20)
#define REG_CFG_QEC_INIT_VALUE          (0x1f << 12)
#define REG_CFG_R2RDAC_INIT_VALUE       (0x3ff << 0)

#define REG_DCC_CFG                     0x52C
#define REG_CFG_DCC_FINE_TUNE_DURATION  (0x3f << 12)
#define REG_CFG_DCC_FINE_TUNE_SEL       (0xf << 4)
#define REG_CFG_DCC_STABLE_CNT          (0xf << 4)
#define REG_CFG_DCC_STEP                (0x7 << 1)
#define REG_CFG_DCC_QUIT_MODE           (1 << 0)

#define REG_QEC_CFG                     0x530
#define REG_CFG_QEC_FINE_TUNE_DURATION  (0x3f << 12)
#define REG_CFG_QEC_FINE_TUNE_SEL       (0xf << 4)
#define REG_CFG_QEC_STABLE_CNT          (0xf << 4)
#define REG_CFG_QEC_STEP                (0x7 << 1)
#define REG_CFG_QEC_QUIT_MODE           (1 << 0)

#define REG_DCC_OVERWRITE_CFG_CH0       0x534
#define REG_CFG_DCC3_OVR_CH0_M          (1 << 30)
#define REG_CFG_DCC3_VALUE_CH0_M        (0x3f << 24)
#define REG_CFG_DCC2_OVR_CH0_M          (1 << 22)
#define REG_CFG_DCC2_VALUE_CH0_M        (0x3f << 16)
#define REG_CFG_DCC1_OVR_CH0_M          (1 << 14)
#define REG_CFG_DCC1_VALUE_CH0_M        (0x3f << 8)
#define REG_CFG_DCC0_OVR_CH0_M          (1 << 6)
#define REG_CFG_DCC0_VALUE_CH0_M        (0x3f << 0)

#define REG_QEC_OVERWRITE_CFG_CH0       0x538
#define REG_CFG_QEC3_OVR_CH0_M          (1 << 29)
#define REG_CFG_QEC3_VALUE_CH0_M        (0x1f << 24)
#define REG_CFG_QEC2_OVR_CH0_M          (1 << 21)
#define REG_CFG_QEC2_VALUE_CH0_M        (0x1f << 16)
#define REG_CFG_QEC1_OVR_CH0_M          (1 << 13)
#define REG_CFG_QEC1_VALUE_CH0_M        (0x1f << 8)
#define REG_CFG_QEC0_OVR_CH0_M          (1 << 5)
#define REG_CFG_QEC0_VALUE_CH0_M        (0x1f << 0)

#define REG_R2RDAC_OVERWRITE_CFG_CH0    0x53C
#define REG_CFG_R2RDAC_OVR_CH0          (1 << 10)
#define REG_CFG_R2RDAC_VALUE_CH0        (0x3ff << 0)

#define REG_DCC_QEC_SW_CFG_CH0          0x540
#define REG_CFG_CNT_EN_OVR_CH0          (1 << 11)
#define REG_CFG_CNT_EN_VALUE_CH0        (1 << 10)
#define REG_CFG_CLK_SWAP_OVR_CH0        (1 << 9)
#define REG_CFG_CLK_SWAP_VALUE_CH0      (1 << 8)
#define REG_CFG_QEC_EN_OVR_CH0          (1 << 7)
#define REG_CFG_QEC_EN_VALUE_CH0        (1 << 6)
#define REG_CFG_CLK_SEL_OVR_CH0         (1 << 4)
#define REG_CFG_CLK_SEL_VALUE_CH0       (0xf << 0)

#define REG_DCC_QEC_CNT_CH0             0x544
#define REG_PLL_LOCK_CH0                (1 << 8)
#define REG_COUNTER_CH0                 (0xff << 0)

#define REG_DCC_TARGET_VALUE_CFG_CH0    0x548
#define REG_CFG_DCC_TARGET_VALUE_CH0    (0x3ff << 0)

#define REG_DCC_VALUE_CH0               0x54C
#define REG_DCC3_CH0                    (0x3f << 24)
#define REG_DCC2_CH0                    (0x3f << 16)
#define REG_DCC1_CH0                    (0x3f << 8)
#define REG_DCC0_CH0                    (0x3f << 0)

#define REG_COARSE_DCC_VALUE_CH0        0x550
#define REG_DCC3_COARSE_CH0             (0x3f << 24)
#define REG_DCC2_COARSE_CH0             (0x3f << 16)
#define REG_DCC1_COARSE_CH0             (0x3f << 8)
#define REG_DCC0_COARSE_CH0             (0x3f << 0)

#define REG_QEC_CFG1_CH0                0x554
#define REG_CFG_QEC_TARGET_OFFSET_CH0   (0x1f << 16)
#define REG_CFG_QEC_OFFSET_CK3_CH0      (0xf << 12)
#define REG_CFG_QEC_OFFSET_CK2_CH0      (0xf << 8)
#define REG_CFG_QEC_OFFSET_CK0_CH0      (0xf << 0)

#define REG_QEC_HW_CH0                  0x558
#define REG_QEC3_CH0                    (0x1f << 24)
#define REG_QEC2_CH0                    (0x1f << 16)
#define REG_QEC1_CH0                    (0x1f << 8)
#define REG_QEC0_CH0                    (0x1f << 0)

#define REG_QEC_VALUE_CH0               0x55C
#define REG_QEC1_CK3_CH0                (0x1f << 25)
#define REG_QEC0_CK3_CH0                (0x1f << 20)
#define REG_QEC1_CK2_CH0                (0x1f << 15)
#define REG_QEC0_CK2_CH0                (0x1f << 10)
#define REG_QEC1_CK0_CH0                (0x1f << 5)
#define REG_QEC0_CK0_CH0                (0x1f << 0)

#define REG_COARSE_QEC_VALUE_CH0        0x560
#define REG_COARSE_QEC1_COARSE_CK3_CH0  (0x1f << 25)
#define REG_COARSE_QEC0_COARSE_CK3_CH0  (0x1f << 20)
#define REG_COARSE_QEC1_COARSE_CK2_CH0  (0x1f << 15)
#define REG_COARSE_QEC0_COARSE_CK2_CH0  (0x1f << 10)
#define REG_COARSE_QEC1_COARSE_CK0_CH0  (0x1f << 5)
#define REG_COARSE_QEC0_COARSE_CK0_CH0  (0x1f << 0)

#define REG_DCC_QEC_STATE_CH0           0x564
#define REG_QEC_CURR_ST_CH0             (0x7 << 4)
#define REG_DCC_CURR_ST_CH0             (0x7 << 0)

#define REG_DCC_QEC_INTR_CLR_CH0        0x568
#define REG_CFG_DCC_QEC_INTR_CLR_CH0    (1 << 0)

#define REG_DCC_EXCEPTION_EVENT_CH0     0x56C
#define REG_DCC_FINE_TUNE_INTR_CH0      (0xff << 8)
#define REG_DCC_COARSE_TUNE_INTR_CH0    (0xff << 0)

#define REG_QEC_EXCEPTION_EVENT_CH0     0x570
#define REG_QEC_FINE_TUNE_INTR_CH0      (0xfff << 12)
#define REG_QEC_COARSE_TUNE_INTR_CH0    (0xfff << 8)

#define REG_DCC_OVERWRITE_CFG_CH1       0x58C
#define REG_CFG_DCC3_OVR_CH1_M          (1 << 30)
#define REG_CFG_DCC3_VALUE_CH1_M        (0x3f << 24)
#define REG_CFG_DCC2_OVR_CH1_M          (1 << 22)
#define REG_CFG_DCC2_VALUE_CH1_M        (0x3f << 16)
#define REG_CFG_DCC1_OVR_CH1_M          (1 << 14)
#define REG_CFG_DCC1_VALUE_CH1_M        (0x3f << 8)
#define REG_CFG_DCC0_OVR_CH1_M          (1 << 6)
#define REG_CFG_DCC0_VALUE_CH1_M        (0x3f << 0)

#define REG_QEC_OVERWRITE_CFG_CH1       0x590
#define REG_CFG_QEC3_OVR_CH1_M          (1 << 29)
#define REG_CFG_QEC3_VALUE_CH1_M        (0x1f << 24)
#define REG_CFG_QEC2_OVR_CH1_M          (1 << 21)
#define REG_CFG_QEC2_VALUE_CH1_M        (0x1f << 16)
#define REG_CFG_QEC1_OVR_CH1_M          (1 << 13)
#define REG_CFG_QEC1_VALUE_CH1_M        (0x1f << 8)
#define REG_CFG_QEC0_OVR_CH1_M          (1 << 5)
#define REG_CFG_QEC0_VALUE_CH1_M        (0x1f << 0)

#define REG_R2RDAC_OVERWRITE_CFG_CH1    0x594
#define REG_CFG_R2RDAC_OVR_CH1          (1 << 10)
#define REG_CFG_R2RDAC_VALUE_CH1        (0x3ff << 0)

#define REG_DCC_QEC_SW_CFG_CH1          0x598
#define REG_CFG_CNT_EN_OVR_CH1          (1 << 11)
#define REG_CFG_CNT_EN_VALUE_CH1        (1 << 10)
#define REG_CFG_CLK_SWAP_OVR_CH1        (1 << 9)
#define REG_CFG_CLK_SWAP_VALUE_CH1      (0xf << 4)
#define REG_CFG_QEC_EN_OVR_CH1          (1 << 7)
#define REG_CFG_QEC_EN_VALUE_CH1        (1 << 6)
#define REG_CFG_CLK_SEL_OVR_CH1         (1 << 4)
#define REG_CFG_CLK_SEL_VALUE_CH1       (0xf << 0)

#define REG_DCC_QEC_CNT_CH1             0x59C
#define REG_PLL_LOCK_CH1                (1 << 8)
#define REG_COUNTER_CH1                 (0xff << 0)

#define REG_DCC_TARGET_VALUE_CFG_CH1    0x5A0
#define REG_CFG_DCC_TARGET_VALUE_CH1    (0x3ff << 0)

#define REG_DCC_VALUE_CH1               0x5A4
#define REG_DCC3_CH1                    (0x3f << 24)
#define REG_DCC2_CH1                    (0x3f << 16)
#define REG_DCC1_CH1                    (0x3f << 8)
#define REG_DCC0_CH1                    (0x3f << 0)

#define REG_COARSE_DCC_VALUE_CH1        0x5A8
#define REG_DCC3_COARSE_CH1             (0x3f << 24)
#define REG_DCC2_COARSE_CH1             (0x3f << 16)
#define REG_DCC1_COARSE_CH1             (0x3f << 8)
#define REG_DCC0_COARSE_CH1             (0x3f << 0)

#define REG_QEC_CFG1_CH1                0x5AC
#define REG_CFG_QEC_TARGET_OFFSET_CH1   (0x1f << 16)
#define REG_CFG_QEC_OFFSET_CK3_CH1      (0xf << 12)
#define REG_CFG_QEC_OFFSET_CK2_CH1      (0xf << 8)
#define REG_CFG_QEC_OFFSET_CK0_CH1      (0xf << 0)

#define REG_QEC_HW_CH1                  0x5B0
#define REG_QEC3_CH1                    (0x1f << 24)
#define REG_QEC2_CH1                    (0x1f << 16)
#define REG_QEC1_CH1                    (0x1f << 8)
#define REG_QEC0_CH1                    (0x1f << 0)

#define REG_QEC_VALUE_CH1               0x5B4
#define REG_QEC1_CK3_CH1                (0x1f << 25)
#define REG_QEC0_CK3_CH1                (0x1f << 20)
#define REG_QEC1_CK2_CH1                (0x1f << 15)
#define REG_QEC0_CK2_CH1                (0x1f << 10)
#define REG_QEC1_CK0_CH1                (0x1f << 5)
#define REG_QEC0_CK0_CH1                (0x1f << 0)

#define REG_COARSE_QEC_VALUE_CH1        0x5B8
#define REG_QEC1_COARSE_CK3_CH1         (0x1f << 25)
#define REG_QEC0_COARSE_CK3_CH1         (0x1f << 20)
#define REG_QEC1_COARSE_CK2_CH1         (0x1f << 15)
#define REG_QEC0_COARSE_CK2_CH1         (0x1f << 10)
#define REG_QEC1_COARSE_CK0_CH1         (0x1f << 5)
#define REG_QEC0_COARSE_CK0_CH1         (0x1f << 0)

#define REG_DCC_QEC_STATE_CH1           0x5BC
#define REG_QEC_CURR_ST_CH1             (0x7 << 4)
#define REG_DCC_CURR_ST_CH1             (0x7 << 0)

#define REG_DCC_QEC_INTR_CLR_CH1        0x5C0
#define REG_CFG_DCC_QEC_INTR_CLR_CH1    (1 << 0)


#define REG_DCC_EXCEPTION_EVENT_CH1     0x5C4
#define REG_DCC_FINE_TUNE_INTR_CH1      (0xff << 8)
#define REG_DCC_COARSE_TUNE_INTR_CH1    (0xff << 0)


#define REG_QEC_EXCEPTION_EVENT_CH1     0x5C8
#define REG_QEC_FINE_TUNE_INTR_CH1      (0xfff << 12)
#define REG_QEC_COARSE_TUNE_INTR_CH1    (0xfff << 8)

#define REG_DCC_OVERWRITE_CFG_CH2       0x5E4
#define REG_CFG_DCC3_OVR_CH2_M          (1 << 30)
#define REG_CFG_DCC3_VALUE_CH2_M        (0x3f << 24)
#define REG_CFG_DCC2_OVR_CH2_M          (1 << 22)
#define REG_CFG_DCC2_VALUE_CH2_M        (0x3f << 16)
#define REG_CFG_DCC1_OVR_CH2_M          (1 << 14)
#define REG_CFG_DCC1_VALUE_CH2_M        (0x3f << 8)
#define REG_CFG_DCC0_OVR_CH2_M          (1 << 6)
#define REG_CFG_DCC0_VALUE_CH2_M        (0x3f << 0)

#define REG_QEC_OVERWRITE_CFG_CH2       0x5E8
#define REG_CFG_QEC3_OVR_CH2_M          (1 << 29)
#define REG_CFG_QEC3_VALUE_CH2_M        (0x1f << 24)
#define REG_CFG_QEC2_OVR_CH2_M          (1 << 21)
#define REG_CFG_QEC2_VALUE_CH2_M        (0x1f << 16)
#define REG_CFG_QEC1_OVR_CH2_M          (1 << 13)
#define REG_CFG_QEC1_VALUE_CH2_M        (0x1f << 8)
#define REG_CFG_QEC0_OVR_CH2_M          (1 << 5)
#define REG_CFG_QEC0_VALUE_CH2_M        (0x1f << 0)

#define REG_R2RDAC_OVERWRITE_CFG_CH2    0x5EC
#define REG_CFG_R2RDAC_OVR_CH2          (1 << 10)
#define REG_CFG_R2RDAC_VALUE_CH2        (0x3ff << 0)

#define REG_DCC_QEC_SW_CFG_CH2          0x5F0
#define REG_CFG_CNT_EN_OVR_CH2          (1 << 11)
#define REG_CFG_CNT_EN_VALUE_CH2        (1 << 10)
#define REG_CFG_CLK_SWAP_OVR_CH2        (1 << 9)
#define REG_CFG_CLK_SWAP_VALUE_CH2      (1 << 8)
#define REG_CFG_QEC_EN_OVR_CH2          (1 << 7)
#define REG_CFG_QEC_EN_VALUE_CH2        (1 << 6)
#define REG_CFG_CLK_SEL_OVR_CH2         (1 << 4)
#define REG_CFG_CLK_SEL_VALUE_CH2       (0xf << 0)

#define REG_DCC_QEC_CNT_CH2             0x5F4
#define REG_PLL_LOCK_CH2                (1 << 8)
#define REG_COUNTER_CH2                 (0xff << 0)

#define REG_DCC_TARGET_VALUE_CFG_CH2    0x5F8
#define REG_CFG_DCC_TARGET_VALUE_CH2    (0x3ff << 0)

#define REG_DCC_VALUE_CH2               0x5FC
#define REG_DCC3_CH2                    (0x3f << 24)
#define REG_DCC2_CH2                    (0x3f << 16)
#define REG_DCC1_CH2                    (0x3f << 8)
#define REG_DCC0_CH2                    (0x3f << 0)

#define REG_COARSE_DCC_VALUE_CH2        0x600
#define REG_DCC3_COARSE_CH2             (0x3f << 24)
#define REG_DCC2_COARSE_CH2             (0x3f << 16)
#define REG_DCC1_COARSE_CH2             (0x3f << 8)
#define REG_DCC0_COARSE_CH2             (0x3f << 0)

#define REG_QEC_CFG1_CH2                0x604
#define REG_CFG_QEC_TARGET_OFFSET_CH2   (0x1f << 16)
#define REG_CFG_QEC_OFFSET_CK3_CH2      (0xf << 12)
#define REG_CFG_QEC_OFFSET_CK2_CH2      (0xf << 8)
#define REG_CFG_QEC_OFFSET_CK0_CH2      (0xf << 0)

#define REG_QEC_HW_CH2                  0x608
#define REG_QEC3_CH2                    (0x1f << 24)
#define REG_QEC2_CH2                    (0x1f << 16)
#define REG_QEC1_CH2                    (0x1f << 8)
#define REG_QEC0_CH2                    (0x1f << 0)

#define REG_QEC_VALUE_CH2               0x60C
#define REG_QEC1_CK3_CH2                (0x1f << 25)
#define REG_QEC0_CK3_CH2                (0x1f << 20)
#define REG_QEC1_CK2_CH2                (0x1f << 15)
#define REG_QEC0_CK2_CH2                (0x1f << 10)
#define REG_QEC1_CK0_CH2                (0x1f << 5)
#define REG_QEC0_CK0_CH2                (0x1f << 0)

#define REG_COARSE_QEC_VALUE_CH2        0x610
#define REG_QEC1_COARSE_CK3_CH2         (0x1f << 25)
#define REG_QEC0_COARSE_CK3_CH2         (0x1f << 20)
#define REG_QEC1_COARSE_CK2_CH2         (0x1f << 15)
#define REG_QEC0_COARSE_CK2_CH2         (0x1f << 10)
#define REG_QEC1_COARSE_CK0_CH2         (0x1f << 5)
#define REG_QEC0_COARSE_CK0_CH2         (0x1f << 0)

#define REG_DCC_QEC_STATE_CH2           0x614
#define REG_QEC_CURR_ST_CH2             (0x7 << 4)
#define REG_DCC_CURR_ST_CH2             (0x7 << 0)

#define REG_DCC_QEC_INTR_CLR_CH2        0x618
#define REG_CFG_DCC_QEC_INTR_CLR_CH2    (1 << 0)

#define REG_DCC_EXCEPTION_EVENT_CH2     0x61C
#define REG_DCC_FINE_TUNE_INTR_CH2      (0xff << 8)
#define REG_DCC_COARSE_TUNE_INTR_CH2    (0xff << 0)

#define REG_QEC_EXCEPTION_EVENT_CH2     0x620
#define REG_QEC_FINE_TUNE_INTR_CH2      (0xfff << 12)
#define REG_QEC_COARSE_TUNE_INTR_CH2    (0xfff << 8)

#define REG_DCC_OVERWRITE_CFG_CH3       0x63C
#define REG_CFG_DCC3_OVR_CH3_M          (1 << 30)
#define REG_CFG_DCC3_VALUE_CH3_M        (0x3f << 24)
#define REG_CFG_DCC2_OVR_CH3_M          (1 << 22)
#define REG_CFG_DCC2_VALUE_CH3_M        (0x3f << 16)
#define REG_CFG_DCC1_OVR_CH3_M          (1 << 14)
#define REG_CFG_DCC1_VALUE_CH3_M        (0x3f << 8)
#define REG_CFG_DCC0_OVR_CH3_M          (1 << 6)
#define REG_CFG_DCC0_VALUE_CH3_M        (0x3f << 0)

#define REG_QEC_OVERWRITE_CFG_CH3       0x640
#define REG_CFG_QEC3_OVR_CH3_M          (1 << 29)
#define REG_CFG_QEC3_VALUE_CH3_M        (0x1f << 24)
#define REG_CFG_QEC2_OVR_CH3_M          (1 << 21)
#define REG_CFG_QEC2_VALUE_CH3_M        (0x1f << 16)
#define REG_CFG_QEC1_OVR_CH3_M          (1 << 13)
#define REG_CFG_QEC1_VALUE_CH3_M        (0x1f << 8)
#define REG_CFG_QEC0_OVR_CH3_M          (1 << 5)
#define REG_CFG_QEC0_VALUE_CH3_M        (0x1f << 0)

#define REG_R2RDAC_OVERWRITE_CFG_CH3    0x644
#define REG_CFG_R2RDAC_OVR_CH3          (1 << 10)
#define REG_CFG_R2RDAC_VALUE_CH3        (0x3ff << 0)

#define REG_DCC_QEC_SW_CFG_CH3          0x648
#define REG_CFG_CNT_EN_OVR_CH3          (1 << 11)
#define REG_CFG_CNT_EN_VALUE_CH3        (1 << 10)
#define REG_CFG_CLK_SWAP_OVR_CH3        (1 << 9)
#define REG_CFG_CLK_SWAP_VALUE_CH3      (1 << 8)
#define REG_CFG_QEC_EN_OVR_CH3          (1 << 7)
#define REG_CFG_QEC_EN_VALUE_CH3        (1 << 6)
#define REG_CFG_CLK_SEL_OVR_CH3         (1 << 4)
#define REG_CFG_CLK_SEL_VALUE_CH3       (0xf << 0)

#define REG_DCC_QEC_CNT_CH3             0x64C
#define REG_PLL_LOCK_CH3                (1 << 8)
#define REG_COUNTER_CH3                 (0xff << 0)

#define REG_DCC_TARGET_VALUE_CFG_CH3    0x650
#define REG_CFG_DCC_TARGET_VALUE_CH3    (0x3ff << 0)

#define REG_DCC_VALUE_CH3               0x654
#define REG_DCC3_CH3                    (0x3f << 24)
#define REG_DCC2_CH3                    (0x3f << 16)
#define REG_DCC1_CH3                    (0x3f << 8)
#define REG_DCC0_CH3                    (0x3f << 0)

#define REG_COARSE_DCC_VALUE_CH3        0x658
#define REG_DCC3_COARSE_CH3             (0x3f << 24)
#define REG_DCC2_COARSE_CH3             (0x3f << 16)
#define REG_DCC1_COARSE_CH3             (0x3f << 8)
#define REG_DCC0_COARSE_CH3             (0x3f << 0)

#define REG_QEC_CFG1_CH3                0x65C
#define REG_CFG_QEC_TARGET_OFFSET_CH3   (0x1f << 16)
#define REG_CFG_QEC_OFFSET_CK3_CH3      (0xf << 12)
#define REG_CFG_QEC_OFFSET_CK2_CH3      (0xf << 8)
#define REG_CFG_QEC_OFFSET_CK0_CH3      (0xf << 0)

#define REG_QEC_HW_CH3                  0x660
#define REG_QEC3_CH3                    (0x1f << 24)
#define REG_QEC2_CH3                    (0x1f << 16)
#define REG_QEC1_CH3                    (0x1f << 8)
#define REG_QEC0_CH3                    (0x1f << 0)

#define REG_QEC_VALUE_CH3               0x664
#define REG_QEC1_CK3_CH3                (0x1f << 25)
#define REG_QEC0_CK3_CH3                (0x1f << 20)
#define REG_QEC1_CK2_CH3                (0x1f << 15)
#define REG_QEC0_CK2_CH3                (0x1f << 10)
#define REG_QEC1_CK0_CH3                (0x1f << 5)
#define REG_QEC0_CK0_CH3                (0x1f << 0)

#define REG_COARSE_QEC_VALUE_CH3        0x668
#define REG_QEC1_COARSE_CK3_CH3         (0x1f << 25)
#define REG_QEC0_COARSE_CK3_CH3         (0x1f << 20)
#define REG_QEC1_COARSE_CK2_CH3         (0x1f << 15)
#define REG_QEC0_COARSE_CK2_CH3         (0x1f << 10)
#define REG_QEC1_COARSE_CK0_CH3         (0x1f << 5)
#define REG_QEC0_COARSE_CK0_CH3         (0x1f << 0)

#define REG_DCC_QEC_STATE_CH3           0x66C
#define REG_QEC_CURR_ST_CH3             (0x7 << 4)
#define REG_DCC_CURR_ST_CH3             (0x7 << 0)

#define REG_DCC_QEC_INTR_CLR_CH3        0x670
#define REG_CFG_DCC_QEC_INTR_CLR_CH3    (1 << 0)

#define REG_DCC_EXCEPTION_EVENT_CH3     0x674
#define REG_DCC_FINE_TUNE_INTR_CH3      (0xff << 8)
#define REG_DCC_COARSE_TUNE_INTR_CH3    (0xff << 0)

#define REG_QEC_EXCEPTION_EVENT_CH3     0x678
#define REG_QEC_FINE_TUNE_INTR_CH3      (0xfff << 12)
#define REG_QEC_COARSE_TUNE_INTR_CH3    (0xfff << 8)

#define REG_FFE_EN_CFG                  0x67C
#define REG_CFG_C3_PRE_EN_M             (1 << 11)
#define REG_CFG_C3_POST1_EN_M           (1 << 10)
#define REG_CFG_C3_POST2_EN_M           (1 << 9)
#define REG_CFG_C2_PRE_EN_M             (1 << 8)
#define REG_CFG_C2_POST1_EN_M           (1 << 7)
#define REG_CFG_C2_POST2_EN_M           (1 << 6)
#define REG_CFG_C1_PRE_EN_M             (1 << 5)
#define REG_CFG_C1_POST1_EN_M           (1 << 4)
#define REG_CFG_C1_POST2_EN_M           (1 << 3)
#define REG_CFG_C0_PRE_EN_M             (1 << 2)
#define REG_CFG_C0_POST1_EN_M           (1 << 1)
#define REG_CFG_C0_POST2_EN_M           (1 << 0)

#define REG_STBOPT                      0x700
#define REG_STB_CS_SEL_M                (0x1 << 15)
#define REG_STB_ACC_SEL_M               (0x1 << 14)
#define REG_STB_DELAY0_M                (0xf << 10)
#define REG_STB_DELAY1_M                (0xf << 6)
#define REG_STB_DELAY2_M                (0xf << 2)
#define REG_REQ_LENGTH_M                (0x3 << 0)

#define REG_STBREQ                      0x704
#define REG_REQ_DONE                    (0x1 << 0)

#endif /* __HAL_HDMITX_DPHY_REG__H__ */
