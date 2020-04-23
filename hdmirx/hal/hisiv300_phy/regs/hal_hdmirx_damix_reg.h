/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Register definition of damix module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_DAMIX_REG_H__
#define __HAL_HDMIRX_DAMIX_REG_H__

#define REG_CLK_DETECT0 0x0000
#define REG_XTAL_TIMER_VAL    0xFF       /* mask bit 7:0 */
#define REG_DVI_PRD_SEL       0x700      /* mask bit 10:8 */
#define REG_CLK_STBEXT_SEL    0x7000     /* mask bit 14:12 */
#define REG_TEST_SEL          0x78000    /* mask bit 18:15 */
#define HDMI_ZONE_CTRL        0x380000   /* mask bit 21:19 */
#define HDMI_CLK_STABLE       0x400000   /* mask bit 22 */
#define CLK_DVI_EXIST         0x800000   /* mask bit 23 */
#define HDMI_CLK_STABLE_LANE1 0x1000000  /* mask bit 24 */
#define CLK_DVI_EXIST_LANE1   0x2000000  /* mask bit 25 */
#define HDMI_CLK_STABLE_LANE2 0x4000000  /* mask bit 26 */
#define CLK_DVI_EXIST_LANE2   0x8000000  /* mask bit 27 */
#define HDMI_CLK_STABLE_LANE3 0x10000000 /* mask bit 28 */
#define CLK_DVI_EXIST_LANE3   0x20000000 /* mask bit 29 */

#define REG_CLK_DETECT1 0x0004
#define REG_CLK_JITT_VAL       0xFF       /* mask bit 7:0 */
#define REG_CLK_JITT_SEL       0x100      /* mask bit 8 */
#define REG_CLK_JITT_MODE      0x600      /* mask bit 10:9 */
#define REG_CLK_MEAN_SEL       0x800      /* mask bit 11 */
#define REG_CLK_UNSTABLE_TIMES 0xFF000    /* mask bit 19:12 */
#define REG_CLK_STABLE_TIMES   0xFF00000  /* mask bit 27:20 */
#define REG_DVI_DIV_SEL        0x70000000 /* mask bit 30:28 */

#define REG_CLK_DETECT2 0x0008
#define CLK_CNT3 0xFF       /* mask bit 7:0 */
#define CLK_CNT2 0xFF00     /* mask bit 15:8 */
#define CLK_CNT1 0xFFFF0000 /* mask bit 31:16 */

#define REG_CLK_DETECT3 0x000C
#define HDMI_FRQ_MODE_LOW 0xFFFFFFFF /* mask bit 31:0 */

#define REG_PLL_LOCK0 0x0014
#define REG_PLL_LOCK_TIMES   0x1        /* mask bit 0 */
#define REG_PLL_UNLOCK_SEL   0x2        /* mask bit 1 */
#define REG_PLL_LOCK_SEL     0xC        /* mask bit 3:2 */
#define REG_REF_UNLOCK_TIMER 0xFF0      /* mask bit 11:4 */
#define REG_REF_LOCK_TIMER   0xFF000    /* mask bit 19:12 */
#define REG_FB_PRD_SEL       0x700000   /* mask bit 22:20 */
#define REG_FB_DIV_SEL       0x3800000  /* mask bit 25:23 */
#define REG_PLL_LOCKEXT_SEL  0x1C000000 /* mask bit 28:26 */

#define REG_PLL_LOCK1 0x0018
#define PLL_CNT_LANE0       0x1FFFF    /* mask bit 16:0 */
#define HDMI_CLK_LOCK_LANE2 0x20000    /* mask bit 17 */
#define HDMI_CLK_LOCK_LANE1 0x40000    /* mask bit 18 */
#define HDMI_CLK_LOCK_LANE0 0x80000    /* mask bit 19 */
#define PLL_LOCK            0x100000   /* mask bit 20 */
#define CLK_FB_EXIST_LANE2  0x200000   /* mask bit 21 */
#define CLK_FB_EXIST_LANE1  0x400000   /* mask bit 22 */
#define CLK_FB_EXIST_LANE0  0x800000   /* mask bit 23 */
#define CLK_FB_EXIST_LANE3  0x1000000  /* mask bit 24 */
#define HDMI_CLK_LOCK_LANE3 0x2000000  /* mask bit 25 */
#define HDMI_FRQ_MODE_HIGH  0x1C000000 /* mask bit 28:26 */

#define REG_PLL_LOCK2 0x001C
#define PLL_CNT_LANE1 0x1FFFF /* mask bit 16:0 */

#define REG_PLL_LOCK3 0x0020
#define PLL_CNT_LANE2 0x1FFFF /* mask bit 16:0 */

#define REG_DCDR0 0x0028
#define REG_PHFSM2_TIME_INTERVAL 0xFF      /* mask bit 7:0 */
#define REG_PHFSM1_TIME_INTERVAL 0xFF00    /* mask bit 15:8 */
#define REG_PHFSM0_TIME_INTERVAL 0xFF0000  /* mask bit 23:16 */
#define REG_PHASE_TIME_BASE      0xF000000 /* mask bit 27:24 */

#define REG_DCDR1 0x002C
#define REG_DVI_CH2_PH     0x7F      /* mask bit 6:0 */
#define REG_DVI_CH1_PH     0x3F80    /* mask bit 13:7 */
#define REG_DVI_CH0_PH     0x1FC000  /* mask bit 20:14 */
#define REG_DVI_OVCH2_LE_W 0x200000  /* mask bit 21 */
#define REG_DVI_OVCH1_LE_W 0x400000  /* mask bit 22 */
#define REG_DVI_OVCH0_LE_W 0x800000  /* mask bit 23 */
#define REG_DVI_OVCH2      0x1000000 /* mask bit 24 */
#define REG_DVI_OVCH1      0x2000000 /* mask bit 25 */
#define REG_DVI_OVCH0      0x4000000 /* mask bit 26 */

#define REG_DCDR2 0x0030
#define REG_UPPER_BOUNDARY 0xFFFF     /* mask bit 15:0 */
#define REG_LOWER_BOUNDARY 0xFFFF0000 /* mask bit 31:16 */

#define REG_DCDR3 0x0034
#define REG_SWAP_DCDR_UPDN_POLITY 0x1     /* mask bit 0 */
#define REG_ACCLSB16BIT_EN        0x2     /* mask bit 1 */
#define REG_ACCU_ERR_SEL          0x4     /* mask bit 2 */
#define REG_BOUNDARY_EN           0x8     /* mask bit 3 */
#define REG_ACC8LSB_EXT_EN        0x10    /* mask bit 4 */
#define REG_AUTO_CLR_ENZ          0x20    /* mask bit 5 */
#define REG_PH_UPDATE_TIMER_DIS   0x40    /* mask bit 6 */
#define REG_EARLY_LATE_NO_VOTE    0x80    /* mask bit 7 */
#define REG_DVI_PHASE_THD         0xFF00  /* mask bit 15:8 */
#define REG_DVI_RDST              0x10000 /* mask bit 16 */
#define REG_DELOCKEN              0x20000 /* mask bit 17 */
#define REG_ENABLE_PHASE_STEP     0x40000 /* mask bit 18 */
#define REG_DELAY_PHACC_MODE      0x80000 /* mask bit 19 */

#define REG_DCDR4 0x0038
#define REG_CH0_PHACC    0xFFFF     /* mask bit 15:0 */
#define PHDAC_CODE_LANE0 0x7F0000   /* mask bit 22:16 */
#define PHDAC_CODE_LIN0  0x1F800000 /* mask bit 28:23 */

#define REG_DCDR5 0x003C
#define REG_CH1_PHACC    0xFFFF     /* mask bit 15:0 */
#define PHDAC_CODE_LANE1 0x7F0000   /* mask bit 22:16 */
#define PHDAC_CODE_LIN1  0x1F800000 /* mask bit 28:23 */

#define REG_DCDR6 0x0040
#define REG_CH2_PHACC    0xFFFF     /* mask bit 15:0 */
#define PHDAC_CODE_LANE2 0x7F0000   /* mask bit 22:16 */
#define PHDAC_CODE_LIN2  0x1F800000 /* mask bit 28:23 */

#define REG_AUTOEQ0 0x0048
#define REG_PHASE_STABLE_THRESHOLD        0x3F       /* mask bit 5:0 */
#define REG_MHL_BBCDR_MODE_EN             0x40       /* mask bit 6 */
#define REG_MHL_AUTO_EQ_TRIGGER           0x80       /* mask bit 7 */
#define REG_MHL_CLOCK_LOCK_SEL            0x100      /* mask bit 8 */
#define REG_FINE_SYMBOL_SEL               0x200      /* mask bit 9 */
#define REG_COARSE_SYMBOL_SEL             0x400      /* mask bit 10 */
#define REG_DP_EN                         0x800      /* mask bit 11 */
#define REG_EN_SYMBOL_LOCK_TIMEOUT        0x1000     /* mask bit 12 */
#define REG_SYMBOL_DET_MODE               0x2000     /* mask bit 13 */
#define REG_SYMBOL_TIME_OUT_MODE          0x4000     /* mask bit 14 */
#define REG_HDMI2_ACDR_MODE               0x8000     /* mask bit 15 */
#define REG_BYPASS_SYMBOL_LOCK_DET_COARSE 0x10000    /* mask bit 16 */
#define REG_BYPASS_CR_LOCK_CHK_COARSE     0x20000    /* mask bit 17 */
#define REG_HDMI_FINE_SYMBOL_DET_EN       0x40000    /* mask bit 18 */
#define REG_EN_CK_LOSE_LOCK_TRIGGER       0x80000    /* mask bit 19 */
#define REG_BACKGROUND_EQ_MODE            0x100000   /* mask bit 20 */
#define REG_MHL_AUTO_EQ_EN                0x200000   /* mask bit 21 */
#define REG_AUTO_EQ_RETUNE_EN             0x400000   /* mask bit 22 */
#define REG_EN_AUTO_EQ_SPDUP_BY_OVF       0x800000   /* mask bit 23 */
#define REG_OTP_MUX                       0x1000000  /* mask bit 24 */
#define CFG_FRL_AUTO_EQ_EN_LANE0          0x2000000  /* mask bit 25 */
#define CFG_FRL_AUTO_EQ_EN_LANE1          0x4000000  /* mask bit 26 */
#define CFG_FRL_AUTO_EQ_EN_LANE2          0x8000000  /* mask bit 27 */
#define CFG_FRL_AUTO_EQ_EN_LANE3          0x10000000 /* mask bit 28 */

#define REG_AUTOEQ1 0x004C
#define REG_EQ_FINE_SYMBOL_DET_TIMES   0xFFFF     /* mask bit 15:0 */
#define REG_EQ_COARSE_SYMBOL_DET_TIMES 0xFFFF0000 /* mask bit 31:16 */

#define REG_AUTOEQ2 0x0050
#define REG_UNDER_OVER_TIME    0xFFFF     /* mask bit 15:0 */
#define REG_UNDER_OVER_TIME_EN 0x10000    /* mask bit 16 */
#define REG_PHASE_CHECK_TIME   0x7E0000   /* mask bit 22:17 */
#define REG_SYMBOL_DET_TIME    0x1F800000 /* mask bit 28:23 */

#define REG_AUTOEQ3 0x0054
#define REG_PHASE_CHECK_DURATION 0xFFFF     /* mask bit 15:0 */
#define REG_SYMBOL_DET_DURATION  0xFFFF0000 /* mask bit 31:16 */

#define REG_AUTOEQ4 0x0058
#define REG_EQ_UPDATE_DURATION           0xFFFF     /* mask bit 15:0 */
#define REG_EQ_UNDER_OVER_CHECK_DURATION 0xFFFF0000 /* mask bit 31:16 */

#define REG_AUTOEQ5 0x005C
#define REG_UNDER_OVER_MODE          0x1      /* mask bit 0 */
#define REG_DIS_EQ_OVER_INC          0x2      /* mask bit 1 */
#define REG_EQ_ACCU_CLAMP_MODE       0x4      /* mask bit 2 */
#define REG_DATA_PARSE_MODE          0x8      /* mask bit 3 */
#define REG_FINE_TUNE_CHECK_MODE     0x10     /* mask bit 4 */
#define REG_EN_RESET_UNDER_OVER_TIME 0x20     /* mask bit 5 */
#define REG_EQ_TIMER_1US_SEL         0x40     /* mask bit 6 */
#define REG_EQ_XTAL_FREQ             0x7F80   /* mask bit 14:7 */
#define REG_MAX_SYMBOL_DET_TIMES     0x1F8000 /* mask bit 20:15 */
#define REG_EN_SYMBOL_DET_TIMES_OVER 0x200000 /* mask bit 21 */
#define REG_SYMBOL_LOCK_MODE         0x400000 /* mask bit 22 */
#define REG_PHASE_TIME_OUT_MODE      0x800000 /* mask bit 23 */

#define REG_AUTOEQ6 0x0060
#define REG_HDMI_UNDER_TH         0xFFFF     /* mask bit 15:0 */
#define REG_EQ_ACCU_DEC_THRESHOLD 0x7F0000   /* mask bit 22:16 */
#define REG_EQ_ACCU_INC_THRESHOLD 0x3F800000 /* mask bit 29:23 */

#define REG_AUTOEQ7 0x0064
#define REG_HDMI_PAT_DET_TIMES2 0xFFFF     /* mask bit 15:0 */
#define REG_HDMI_PAT_DET_TIMES1 0xFFFF0000 /* mask bit 31:16 */

#define REG_AUTOEQ8 0x0068
#define REG_EQ_STRENGTH_COARSE_MODE 0x1      /* mask bit 0 */
#define REG_EQ_END_LIMIT            0x3E     /* mask bit 5:1 */
#define REG_EQ_START_LIMIT          0x7C0    /* mask bit 10:6 */
#define REG_EN_BYPASS_LOW_EQ        0x800    /* mask bit 11 */
#define REG_START_EQ_STRENGTH       0x3F000  /* mask bit 17:12 */
#define REG_EQ_STRENGTH_STEP        0x1C0000 /* mask bit 20:18 */

#define REG_AUTOEQ9 0x006C
#define REG_EN_FINE_AABA_SYMBOL_CHECK    0x1      /* mask bit 0 */
#define REG_EQ_FINE_TUNE_THR             0x7E     /* mask bit 6:1 */
#define REG_EQ_STRENGTH_STABLE_THRESHOLD 0x1F80   /* mask bit 12:7 */
#define REG_EQ_UNDER_SETTLE_MODE         0x2000   /* mask bit 13 */
#define REG_EQ_RESULT_CHOICE             0xC000   /* mask bit 15:14 */
#define REG_EQ_FINE_TUNE_TIME            0x3F0000 /* mask bit 21:16 */

#define REG_AUTOEQ10 0x0070
#define REG_EQ_ERROR_STEP           0x3       /* mask bit 1:0 */
#define REG_EQ_ERROR_CHECK_DURATION 0x3FFFC00 /* mask bit 25:10 */
#define REG_HDMI1_DCDR_MODE         0x4000000 /* mask bit 26 */

#define REG_AUTOEQ11 0x0074
#define AUTO_EQ_ST_LANE2 0x1FF     /* mask bit 8:0 */
#define AUTO_EQ_ST_LANE1 0x3FE00   /* mask bit 17:9 */
#define AUTO_EQ_ST_LANE0 0x7FC0000 /* mask bit 26:18 */

#define REG_AUTOEQ12 0x0078
#define EQ_RESULT_LANE2 0x3F     /* mask bit 5:0 */
#define EQ_RESULT_LANE1 0xFC0    /* mask bit 11:6 */
#define EQ_RESULT_LANE0 0x3F000  /* mask bit 17:12 */
#define EQ_RESULT_LANE3 0xFC0000 /* mask bit 23:18 */

#define REG_D2A0 0x0080
#define REG_DA_DIVSEL_IN2    0x3        /* mask bit 1:0 */
#define REG_DA_DIVSEL_IN_EN2 0x4        /* mask bit 2 */
#define REG_DA_DIVSEL_IN1    0x18       /* mask bit 4:3 */
#define REG_DA_DIVSEL_IN_EN1 0x20       /* mask bit 5 */
#define REG_DA_DIVSEL_IN0    0xC0       /* mask bit 7:6 */
#define REG_DA_DIVSEL_IN_EN0 0x100      /* mask bit 8 */
#define REG_DA_DIVSEL_IN3    0x600      /* mask bit 10:9 */
#define REG_DA_DIVSEL_IN_EN3 0x800      /* mask bit 11 */
#define REG_DA_EN_HDMI2      0x1000000  /* mask bit 24 */
#define REG_DA_HDMI2_EN      0x2000000  /* mask bit 25 */
#define REG_DA_PIXCLKO_MD    0xC000000  /* mask bit 27:26 */
#define REG_DA_PIXMD_EN      0x10000000 /* mask bit 28 */

#define REG_D2A1 0x0084
#define REG_DA_DIVSEL_POST2    0x7        /* mask bit 2:0 */
#define REG_DA_DIVSEL_POST_EN2 0x8        /* mask bit 3 */
#define REG_DA_DIVSEL_POST1    0x70       /* mask bit 6:4 */
#define REG_DA_DIVSEL_POST_EN1 0x80       /* mask bit 7 */
#define REG_DA_DIVSEL_POST0    0x700      /* mask bit 10:8 */
#define REG_DA_DIVSEL_POST_EN0 0x800      /* mask bit 11 */
#define REG_DA_DIVSEL_PLL2     0x1F000    /* mask bit 16:12 */
#define REG_DA_DIVSEL_PLL_EN2  0x20000    /* mask bit 17 */
#define REG_DA_DIVSEL_PLL1     0x7C0000   /* mask bit 22:18 */
#define REG_DA_DIVSEL_PLL_EN1  0x800000   /* mask bit 23 */
#define REG_DA_DIVSEL_PLL0     0x1F000000 /* mask bit 28:24 */
#define REG_DA_DIVSEL_PLL_EN0  0x20000000 /* mask bit 29 */

#define REG_D2A2 0x0088
#define REG_DA_EQ_CODE2     0x1F       /* mask bit 4:0 */
#define REG_DA_EQ_CODE_EN2  0x20       /* mask bit 5 */
#define REG_DA_EQ_CODE1     0x7C0      /* mask bit 10:6 */
#define REG_DA_EQ_CODE_EN1  0x800      /* mask bit 11 */
#define REG_DA_EQ_CODE0     0x1F000    /* mask bit 16:12 */
#define REG_DA_EQ_CODE_EN0  0x20000    /* mask bit 17 */
#define REG_DA_EN_PHD_FB    0x3C0000   /* mask bit 21:18 */
#define REG_DA_EN_PHD_FB_EN 0x3C00000  /* mask bit 25:22 */
#define REG_DA_EQ_CODE3     0x7C000000 /* mask bit 30:26 */
#define REG_DA_EQ_CODE_EN3  0x80000000 /* mask bit 31 */

#define REG_D2A3 0x008C
#define REG_DA_FIXED_CODE2    0x1F      /* mask bit 4:0 */
#define REG_DA_HDMI14_EQ_SEL2 0x20      /* mask bit 5 */
#define REG_DA_FIXED_CODE1    0x7C0     /* mask bit 10:6 */
#define REG_DA_HDMI14_EQ_SEL1 0x800     /* mask bit 11 */
#define REG_DA_FIXED_CODE0    0x1F000   /* mask bit 16:12 */
#define REG_DA_HDMI14_EQ_SEL0 0x20000   /* mask bit 17 */
#define REG_DA_DIVSEL_MHL2    0xC0000   /* mask bit 19:18 */
#define REG_DA_DIVSEL_MHL_EN2 0x100000  /* mask bit 20 */
#define REG_DA_DIVSEL_MHL1    0x600000  /* mask bit 22:21 */
#define REG_DA_DIVSEL_MHL_EN1 0x800000  /* mask bit 23 */
#define REG_DA_DIVSEL_MHL0    0x3000000 /* mask bit 25:24 */
#define REG_DA_DIVSEL_MHL_EN0 0x4000000 /* mask bit 26 */

#define REG_D2A4 0x0090
#define REG_DA_ICTRL_PD2    0xFF      /* mask bit 7:0 */
#define REG_DA_ICTRL_PD_EN2 0x100     /* mask bit 8 */
#define REG_DA_ICTRL_PD1    0x1FE00   /* mask bit 16:9 */
#define REG_DA_ICTRL_PD_EN1 0x20000   /* mask bit 17 */
#define REG_DA_ICTRL_PD0    0x3FC0000 /* mask bit 25:18 */
#define REG_DA_ICTRL_PD_EN0 0x4000000 /* mask bit 26 */

#define REG_D2A5 0x0094
#define REG_DA_ICTRL_PFD2    0xFF      /* mask bit 7:0 */
#define REG_DA_ICTRL_PFD_EN2 0x100     /* mask bit 8 */
#define REG_DA_ICTRL_PFD1    0x1FE00   /* mask bit 16:9 */
#define REG_DA_ICTRL_PFD_EN1 0x20000   /* mask bit 17 */
#define REG_DA_ICTRL_PFD0    0x3FC0000 /* mask bit 25:18 */
#define REG_DA_ICTRL_PFD_EN0 0x4000000 /* mask bit 26 */

#define REG_D2A6 0x0098
#define REG_DA_PHDAC_CODE2    0x7F       /* mask bit 6:0 */
#define REG_DA_PHDAC_CODE_EN2 0x80       /* mask bit 7 */
#define REG_DA_PHDAC_CODE1    0x7F00     /* mask bit 14:8 */
#define REG_DA_PHDAC_CODE_EN1 0x8000     /* mask bit 15 */
#define REG_DA_PHDAC_CODE0    0x7F0000   /* mask bit 22:16 */
#define REG_DA_PHDAC_CODE_EN0 0x800000   /* mask bit 23 */
#define REG_DA_PHDAC_CODE3    0x7F000000 /* mask bit 30:24 */
#define REG_DA_PHDAC_CODE_EN3 0x80000000 /* mask bit 31 */

#define REG_D2A7 0x009C
#define REG_DA_SEL_PHD_REG2    0x3        /* mask bit 1:0 */
#define REG_DA_SEL_PHD_REG_EN2 0x4        /* mask bit 2 */
#define REG_DA_SEL_PHD_REG1    0x18       /* mask bit 4:3 */
#define REG_DA_SEL_PHD_REG_EN1 0x20       /* mask bit 5 */
#define REG_DA_SEL_PHD_REG0    0xC0       /* mask bit 7:6 */
#define REG_DA_SEL_PHD_REG_EN0 0x100      /* mask bit 8 */
#define REG_DA_SEL_PHD_REG3    0x600      /* mask bit 10:9 */
#define REG_DA_SEL_PHD_REG_EN3 0x800      /* mask bit 11 */
#define REG_DA_RCTRL_PLL2      0x78000    /* mask bit 18:15 */
#define REG_DA_RCTRL_PLL_EN2   0x80000    /* mask bit 19 */
#define REG_DA_RCTRL_PLL1      0xF00000   /* mask bit 23:20 */
#define REG_DA_RCTRL_PLL_EN1   0x1000000  /* mask bit 24 */
#define REG_DA_RCTRL_PLL0      0x1E000000 /* mask bit 28:25 */
#define REG_DA_RCTRL_PLL_EN0   0x20000000 /* mask bit 29 */

#define REG_ANALOG0 0x00A8
#define RG_SEL_CLKIN_L0    0x3        /* mask bit 1:0 */
#define RG_SEL_CLKIN_L1    0xC        /* mask bit 3:2 */
#define RG_SEL_CLKIN_L2    0x30       /* mask bit 5:4 */
#define RG_TEST_CLK_CH     0x3FC0     /* mask bit 13:6 */
#define RG_TST_SEL_BIST_CH 0x3C000    /* mask bit 17:14 */
#define RG_TST_SEL_BIST_L0 0x3C0000   /* mask bit 21:18 */
#define RG_TST_SEL_BIST_L1 0x3C00000  /* mask bit 25:22 */
#define RG_TST_SEL_BIST_L2 0x3C000000 /* mask bit 29:26 */
#define RG_SEL_CLKIN_L3    0xC0000000 /* mask bit 31:30 */

#define REG_ANALOG1 0x00AC
#define RG_TEST_PLL_L2_LOW 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG2 0x00B0
#define RG_TEST_PLL_L1_LOW 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG3 0x00B4
#define RG_TEST_PLL_L0_LOW 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG4 0x00B8
#define RG_TEST_EQ_L2_LOW 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG5 0x00BC
#define RG_TEST_EQ_L1_LOW 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG6 0x00C0
#define RG_TEST_EQ_L0_LOW 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG7 0x00C4
#define RG_TEST_ATOP_LOW 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG8 0x00C8
#define RG_ISEL_BIST_CH 0x3        /* mask bit 1:0 */
#define RG_ISEL_BIST_L0 0xC        /* mask bit 3:2 */
#define RG_ISEL_BIST_L1 0x30       /* mask bit 5:4 */
#define RG_ISEL_BIST_L2 0xC0       /* mask bit 7:6 */
#define RG_ISEL_EQ_L0   0x300      /* mask bit 9:8 */
#define RG_ISEL_EQ_L1   0xC00      /* mask bit 11:10 */
#define RG_ISEL_EQ_L2   0x3000     /* mask bit 13:12 */
#define RG_PD_BG        0x4000     /* mask bit 14 */
#define RG_PD_CLK       0x8000     /* mask bit 15 */
#define RG_PD_LANE      0xF0000    /* mask bit 19:16 */
#define RG_PD_LDO       0x100000   /* mask bit 20 */
#define RG_PD_PHDAC     0x1E00000  /* mask bit 24:21 */
#define RG_PD_RT        0x1E000000 /* mask bit 28:25 */
#define RG_PD_PLL       0x20000000 /* mask bit 29 */
#define RG_ISEL_EQ_L3   0xC0000000 /* mask bit 31:30 */

#define REG_ANALOG9 0x00CC
#define RG_EN_DRVP         0xF        /* mask bit 3:0 */
#define RG_EN_DRVN         0xF0       /* mask bit 7:4 */
#define RG_EN_CLKO_TMDS_2X 0x100      /* mask bit 8 */
#define RG_EN_CLKO_PIX_2X  0x200      /* mask bit 9 */
#define RG_EN_CLKO_PIX     0x400      /* mask bit 10 */
#define RG_DATI            0x3C000    /* mask bit 17:14 */
#define RG_EN_BIST         0x3C0000   /* mask bit 21:18 */
#define RG_SEL_LDO         0xC00000   /* mask bit 23:22 */
#define RG_RT_CTRL         0xF000000  /* mask bit 27:24 */
#define RG_EN_CAL_AMP      0xF0000000 /* mask bit 31:28 */

#define REG_ANALOG10 0x00D0
#define RG_EN_IDAC_SAR       0xF        /* mask bit 3:0 */
#define RG_EN_IDAC_CAL_IN_L2 0xFC0      /* mask bit 11:6 */
#define RG_EN_IDAC_CAL_IN_L1 0x3F000    /* mask bit 17:12 */
#define RG_EN_IDAC_CAL_IN_L0 0xFC0000   /* mask bit 23:18 */
#define RG_EN_IDAC           0xF000000  /* mask bit 27:24 */
#define RG_EN_IDAC_CAL_OV    0xF0000000 /* mask bit 31:28 */

#define REG_ANALOG11 0x00D4
#define RG_RT_CTRL_CK   0xF0       /* mask bit 7:4 */
#define RG_BG_TEST      0x300      /* mask bit 9:8 */
#define RG_CAL_START    0x1000     /* mask bit 12 */
#define RG_CAL_OV       0x7E000    /* mask bit 18:13 */
#define RG_EN_CAL_OV    0x80000    /* mask bit 19 */
#define RG_EN_SHORT     0xF00000   /* mask bit 23:20 */
#define RG_EN_SCAN      0x7000000  /* mask bit 26:24 */
#define RG_EN_MHL_BIST  0x8000000  /* mask bit 27 */
#define RG_EN_MHL       0x10000000 /* mask bit 28 */
#define RG_EN_LPBK_TEST 0x20000000 /* mask bit 29 */

#define REG_ANALOG12 0x00D8
#define AD_RO_PRBS_ERR_L2   0x1      /* mask bit 0 */
#define AD_RO_PRBS_ERR_L1   0x2      /* mask bit 1 */
#define AD_RO_PRBS_ERR_L0   0x4      /* mask bit 2 */
#define AD_RO_LOCK_L2       0x8      /* mask bit 3 */
#define AD_RO_LOCK_L1       0x10     /* mask bit 4 */
#define AD_RO_LOCK_L0       0x20     /* mask bit 5 */
#define AD_RO_QSCAN_L2      0x40     /* mask bit 6 */
#define AD_RO_QSCAN_L1      0x80     /* mask bit 7 */
#define AD_RO_QSCAN_L0      0x100    /* mask bit 8 */
#define AD_RO_ISCAN_L2      0x200    /* mask bit 9 */
#define AD_RO_ISCAN_L1      0x400    /* mask bit 10 */
#define AD_RO_ISCAN_L0      0x800    /* mask bit 11 */
#define AD_RO_HIGH_L2       0x1000   /* mask bit 12 */
#define AD_RO_HIGH_L1       0x2000   /* mask bit 13 */
#define AD_RO_HIGH_L0       0x4000   /* mask bit 14 */
#define AD_RO_EQ_CAL_END_L2 0x8000   /* mask bit 15 */
#define AD_RO_EQ_CAL_END_L1 0x10000  /* mask bit 16 */
#define AD_RO_EQ_CAL_END_L0 0x20000  /* mask bit 17 */
#define AD_RO_EQ_CAL_END_L3 0x40000  /* mask bit 18 */
#define AD_RO_PRBS_ERR_L3   0x80000  /* mask bit 19 */
#define AD_RO_HIGH_L3       0x100000 /* mask bit 20 */
#define AD_RO_LOCK_L3       0x200000 /* mask bit 21 */

#define REG_ANALOG13 0x00DC
#define AD_RO_EQ_CAL_OUT_L2 0x3F       /* mask bit 5:0 */
#define AD_RO_EQ_CAL_OUT_L1 0xFC0      /* mask bit 11:6 */
#define AD_RO_EQ_CAL_OUT_L0 0x3F000    /* mask bit 17:12 */
#define AD_RO_TSTP          0x3C0000   /* mask bit 21:18 */
#define AD_RO_TSTN          0x3C00000  /* mask bit 25:22 */
#define AD_RO_EQ_CAL_OUT_L3 0xFC000000 /* mask bit 31:26 */

#define REG_ANALOG14 0x00E0
#define AD_RO_SAFF_CAL_L2 0x3F     /* mask bit 5:0 */
#define AD_RO_SAFF_CAL_L1 0xFC0    /* mask bit 11:6 */
#define AD_RO_SAFF_CAL_L0 0x3F000  /* mask bit 17:12 */
#define AD_RO_SAFF_CAL_L3 0xFC0000 /* mask bit 23:18 */

#define REG_PRBS0 0x00E8
#define PRBS_ERR_CNT1 0x7FFF     /* mask bit 14:0 */
#define PRBS_PAT1     0x8000     /* mask bit 15 */
#define PRBS_ERR_CNT0 0x7FFF0000 /* mask bit 30:16 */
#define PRBS_PAT0     0x80000000 /* mask bit 31 */

#define REG_PRBS1 0x00EC
#define PRBS_ERR_CNT2    0x7FFF  /* mask bit 14:0 */
#define PRBS_PAT2        0x8000  /* mask bit 15 */
#define REG_PRBS_ERR_CLR 0x10000 /* mask bit 16 */

#define REG_PRBS2 0x00F0
#define PRBS_ERR_CNT3 0x7FFF /* mask bit 14:0 */
#define PRBS_PAT3     0x8000 /* mask bit 15 */

#define DAMIX_REG_VERSION 0x00FC
#define REG_VERSION 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AREG0 0x0100
#define DA_GC_EN_PHD_FB 0x78   /* mask bit 6:3 */
#define DA_GC_CR_LOCK   0x780  /* mask bit 10:7 */
#define DA_GC_EN_HDMI2  0x800  /* mask bit 11 */
#define DA_GC_RESET_VCO 0xF000 /* mask bit 15:12 */

#define REG_AREG1 0x0104
#define DA_GC_ICTRL_PD_L0 0xFF       /* mask bit 7:0 */
#define DA_GC_ICTRL_PD_L1 0xFF00     /* mask bit 15:8 */
#define DA_GC_ICTRL_PD_L2 0xFF0000   /* mask bit 23:16 */
#define DA_GC_ICTRL_PD_L3 0xFF000000 /* mask bit 31:24 */

#define REG_AREG2 0x0108
#define DA_GC_ICTRL_PFD_L0 0xFF       /* mask bit 7:0 */
#define DA_GC_ICTRL_PFD_L1 0xFF00     /* mask bit 15:8 */
#define DA_GC_ICTRL_PFD_L2 0xFF0000   /* mask bit 23:16 */
#define DA_GC_DIVSEL_IN_L0 0x3000000  /* mask bit 25:24 */
#define DA_GC_DIVSEL_IN_L1 0xC000000  /* mask bit 27:26 */
#define DA_GC_DIVSEL_IN_L2 0x30000000 /* mask bit 29:28 */
#define DA_GC_DIVSEL_IN_L3 0xC0000000 /* mask bit 31:30 */

#define REG_AREG3 0x010C
#define DA_GC_RCTRL_PLL_L0 0xF    /* mask bit 3:0 */
#define DA_GC_RCTRL_PLL_L1 0xF0   /* mask bit 7:4 */
#define DA_GC_RCTRL_PLL_L2 0xF00  /* mask bit 11:8 */
#define DA_GC_RCTRL_PLL_L3 0xF000 /* mask bit 15:12 */

#define REG_AREG4 0x0110
#define DA_GC_DIVSEL_POST_L0 0x7        /* mask bit 2:0 */
#define DA_GC_DIVSEL_POST_L1 0x38       /* mask bit 5:3 */
#define DA_GC_DIVSEL_POST_L2 0x1C0      /* mask bit 8:6 */
#define DA_GC_DIVSEL_PLL_L0  0x3E00     /* mask bit 13:9 */
#define DA_GC_DIVSEL_PLL_L1  0x7C000    /* mask bit 18:14 */
#define DA_GC_DIVSEL_PLL_L2  0xF80000   /* mask bit 23:19 */
#define DA_GC_DIVSEL_PLL_L3  0x1F000000 /* mask bit 28:24 */
#define DA_GC_DIVSEL_POST_L3 0xE0000000 /* mask bit 31:29 */

#define REG_AREG5 0x0114
#define DA_GC_SEL_PHD_REG_L0 0x3        /* mask bit 1:0 */
#define DA_GC_SEL_PHD_REG_L1 0xC        /* mask bit 3:2 */
#define DA_GC_SEL_PHD_REG_L2 0x30       /* mask bit 5:4 */
#define DA_GC_DIVSEL_MHL_L0  0xC0       /* mask bit 7:6 */
#define DA_GC_DIVSEL_MHL_L1  0x300      /* mask bit 9:8 */
#define DA_GC_DIVSEL_MHL_L2  0xC00      /* mask bit 11:10 */
#define DA_GC_EQ_CODE_L0     0x1F000    /* mask bit 16:12 */
#define DA_GC_EQ_CODE_L1     0x3E0000   /* mask bit 21:17 */
#define DA_GC_EQ_CODE_L2     0x7C00000  /* mask bit 26:22 */
#define DA_GC_MHL_MODE       0x8000000  /* mask bit 27 */
#define DA_GC_EN_MHL2        0x10000000 /* mask bit 28 */
#define DA_GC_SEL_PHD_REG_L3 0x60000000 /* mask bit 30:29 */

#define REG_TEST_CTRL 0x0118
#define TEST_MUX_4TO1    0x3   /* mask bit 1:0 */
#define TESTOUT_MUX_SEL  0x4   /* mask bit 2 */
#define REG_CLKO_SEL     0x1F0 /* mask bit 8:4 */
#define REG_CLK_FREQ_SEL 0xE00 /* mask bit 11:9 */

#define REG_DCDR_PAHSE_CODE 0x011C
#define DA_GC_PHDAC_CODE_L0 0x7F      /* mask bit 6:0 */
#define DA_GC_PHDAC_CODE_L1 0x3F80    /* mask bit 13:7 */
#define DA_GC_PHDAC_CODE_L2 0x1FC000  /* mask bit 20:14 */
#define DA_GC_PHDAC_CODE_L3 0xFE00000 /* mask bit 27:21 */

#define REG_DCDR_RANGE_CLEAR 0x0120
#define PHASE_CLEAR 0x1 /* mask bit 0 */

#define REG_DCDR_RANGE_L0 0x0124
#define PHASE_MAX_L0      0x3F   /* mask bit 5:0 */
#define PHASE_MIN_L0      0xFC0  /* mask bit 11:6 */
#define PHASE_OVERTURN_L0 0x1000 /* mask bit 12 */

#define REG_DCDR_RANGE_L1 0x0128
#define PHASE_MAX_L1      0x3F   /* mask bit 5:0 */
#define PHASE_MIN_L1      0xFC0  /* mask bit 11:6 */
#define PHASE_OVERTURN_L1 0x1000 /* mask bit 12 */

#define REG_DCDR_RANGE_L2 0x012C
#define PHASE_MAX_L2      0x3F   /* mask bit 5:0 */
#define PHASE_MIN_L2      0xFC0  /* mask bit 11:6 */
#define PHASE_OVERTURN_L2 0x1000 /* mask bit 12 */

#define REG_AUTOEQ20 0x0130
#define AUTO_EQ_ST_LANE3 0x1FF /* mask bit 8:0 */

#define REG_CLK_FREQ_MEAN_LANE23 0x0144
#define CLK_FREQ_MEAN_LANE2 0xFFFF     /* mask bit 15:0 */
#define CLK_FREQ_MEAN_LANE3 0xFFFF0000 /* mask bit 31:16 */

#define DAMIX_REG_LINK_LOSS_RST_EN 0x0148
#define REG_LINK_LOSS_RST_EN 0x1 /* mask bit 0 */

#define REG_CLK_FREQ_MEAN 0x014C
#define CLK_FREQ_MEAN       0xFFFF     /* mask bit 15:0 */
#define CLK_FREQ_MEAN_LANE1 0xFFFF0000 /* mask bit 31:16 */

#define REG_CLK_DET_CFG 0x0150
#define REG_CLK_STABLE_OVR       0x1    /* mask bit 0 */
#define REG_CLK_STABLE_SW        0x2    /* mask bit 1 */
#define REG_PLL_LOCK_OVR         0x4    /* mask bit 2 */
#define REG_PLL_LOCK_SW          0x8    /* mask bit 3 */
#define REG_PLL_LOCK_OVR_LANE1   0x10   /* mask bit 4 */
#define REG_PLL_LOCK_SW_LANE1    0x20   /* mask bit 5 */
#define REG_PLL_LOCK_OVR_LANE2   0x40   /* mask bit 6 */
#define REG_PLL_LOCK_SW_LANE2    0x80   /* mask bit 7 */
#define REG_PLL_LOCK_OVR_LANE3   0x100  /* mask bit 8 */
#define REG_PLL_LOCK_SW_LANE3    0x200  /* mask bit 9 */
#define REG_CLK_STABLE_OVR_LANE1 0x400  /* mask bit 10 */
#define REG_CLK_STABLE_SW_LANE1  0x800  /* mask bit 11 */
#define REG_CLK_STABLE_OVR_LANE2 0x1000 /* mask bit 12 */
#define REG_CLK_STABLE_SW_LANE2  0x2000 /* mask bit 13 */
#define REG_CLK_STABLE_OVR_LANE3 0x4000 /* mask bit 14 */
#define REG_CLK_STABLE_SW_LANE3  0x8000 /* mask bit 15 */

#define REG_SWEEP_EQ_CFG 0x0154
#define REG_EQ_ERROR_START_DELAY 0xFFFF     /* mask bit 15:0 */
#define REG_EQ_ERR_CHECK_MODE    0x10000    /* mask bit 16 */
#define REG_EQ_OFFSET            0x1F00000  /* mask bit 24:20 */
#define REG_EQ_UP_DOWN           0x2000000  /* mask bit 25 */
#define REG_EQ_SWEEP_MODE        0x4000000  /* mask bit 26 */
#define REG_EQ_RANGE_THRESHOLD   0xF8000000 /* mask bit 31:27 */

#define REG_SWEEP_EQ_RESULT 0x0158
#define EQ_SWEEP_MIN_CH0 0x1F       /* mask bit 4:0 */
#define EQ_SWEEP_MAX_CH0 0x3E0      /* mask bit 9:5 */
#define EQ_SWEEP_MIN_CH1 0x7C00     /* mask bit 14:10 */
#define EQ_SWEEP_MAX_CH1 0xF8000    /* mask bit 19:15 */
#define EQ_SWEEP_MIN_CH2 0x1F00000  /* mask bit 24:20 */
#define EQ_SWEEP_MAX_CH2 0x3E000000 /* mask bit 29:25 */

#define REG_FIXED_EQ_CFG0 0x015C
#define REG_EQ_FIXED0 0x1F       /* mask bit 4:0 */
#define REG_EQ_FIXED1 0x3E0      /* mask bit 9:5 */
#define REG_EQ_FIXED2 0x7C00     /* mask bit 14:10 */
#define REG_EQ_FIXED3 0xF8000    /* mask bit 19:15 */
#define REG_EQ_FIXED4 0x1F00000  /* mask bit 24:20 */
#define REG_EQ_FIXED5 0x3E000000 /* mask bit 29:25 */

#define REG_FIXED_EQ_CFG1 0x0160
#define REG_EQ_FIXED6        0x1F   /* mask bit 4:0 */
#define REG_EQ_FIXED7        0x3E0  /* mask bit 9:5 */
#define REG_EQ_SEL           0x1C00 /* mask bit 12:10 */
#define REG_FIXED_SCAN_EQ_EN 0x2000 /* mask bit 13 */

#define REG_HDMI14_DET_CFG0 0x0164
#define REG_HDMI14_42D 0xFFFF     /* mask bit 15:0 */
#define REG_HDMI14_42U 0xFFFF0000 /* mask bit 31:16 */

#define REG_HDMI14_DET_CFG1 0x0168
#define REG_HDMI14_75D 0xFFFF     /* mask bit 15:0 */
#define REG_HDMI14_75U 0xFFFF0000 /* mask bit 31:16 */

#define REG_HDMI14_DET_CFG2 0x016C
#define REG_HDMI14_150D 0xFFFF     /* mask bit 15:0 */
#define REG_HDMI14_150U 0xFFFF0000 /* mask bit 31:16 */

#define REG_HDMI14_DET_CFG3 0x0170
#define REG_HDMI14_250D 0xFFFF     /* mask bit 15:0 */
#define REG_HDMI14_250U 0xFFFF0000 /* mask bit 31:16 */

#define REG_HDMI20_DET_CFG0 0x0174
#define REG_HDMI20_100D 0xFFFF     /* mask bit 15:0 */
#define REG_HDMI20_100U 0xFFFF0000 /* mask bit 31:16 */

#define REG_MHLPP_DET_CFG0 0x0178
#define REG_MHLPP_62D 0xFFFF     /* mask bit 15:0 */
#define REG_MHLPP_62U 0xFFFF0000 /* mask bit 31:16 */

#define REG_MHL24_DET_CFG0 0x017C
#define REG_MHL24_50D 0xFFFF     /* mask bit 15:0 */
#define REG_MHL24_50U 0xFFFF0000 /* mask bit 31:16 */

#define REG_CLK_FREQ_CFG 0x0180
#define REG_FREQ_JITT 0xFFFF /* mask bit 15:0 */

#define REG_CLK_FREQ_DET 0x0184
#define CLK_FREQ_HW     0xFFFF  /* mask bit 15:0 */
#define CLK_FREQ_STABLE 0x10000 /* mask bit 16 */

#define REG_PRBS_INV_CFG 0x0188
#define REG_PRBS_INV_CH0 0x1 /* mask bit 0 */
#define REG_PRBS_INV_CH1 0x2 /* mask bit 1 */
#define REG_PRBS_INV_CH2 0x4 /* mask bit 2 */
#define REG_PRBS_INV_CH3 0x8 /* mask bit 3 */

#define REG_FIXED_EQ_STATE 0x018C
#define FIXED_EQ_STATE 0x7 /* mask bit 2:0 */

#define REG_DAMIX_INTR_MASK 0x0190
#define DAMIX_INTR_STAT0_MASK  0x1       /* mask bit 0 */
#define DAMIX_INTR_STAT1_MASK  0x2       /* mask bit 1 */
#define DAMIX_INTR_STAT2_MASK  0x4       /* mask bit 2 */
#define DAMIX_INTR_STAT3_MASK  0x8       /* mask bit 3 */
#define DAMIX_INTR_STAT4_MASK  0x10      /* mask bit 4 */
#define DAMIX_INTR_STAT5_MASK  0x20      /* mask bit 5 */
#define DAMIX_INTR_STAT6_MASK  0x40      /* mask bit 6 */
#define DAMIX_INTR_STAT7_MASK  0x80      /* mask bit 7 */
#define DAMIX_INTR_STAT8_MASK  0x100     /* mask bit 8 */
#define DAMIX_INTR_STAT9_MASK  0x200     /* mask bit 9 */
#define DAMIX_INTR_STAT10_MASK 0x400     /* mask bit 10 */
#define DAMIX_INTR_STAT11_MASK 0x800     /* mask bit 11 */
#define DAMIX_INTR_STAT12_MASK 0x1000    /* mask bit 12 */
#define DAMIX_INTR_STAT13_MASK 0x2000    /* mask bit 13 */
#define DAMIX_INTR_STAT14_MASK 0x4000    /* mask bit 14 */
#define DAMIX_INTR_STAT15_MASK 0x8000    /* mask bit 15 */
#define DAMIX_INTR_STAT16_MASK 0x10000   /* mask bit 16 */
#define DAMIX_INTR_STAT17_MASK 0x20000   /* mask bit 17 */
#define DAMIX_INTR_STAT18_MASK 0x40000   /* mask bit 18 */
#define DAMIX_INTR_STAT19_MASK 0x80000   /* mask bit 19 */
#define DAMIX_INTR_STAT20_MASK 0x100000  /* mask bit 20 */
#define DAMIX_INTR_STAT21_MASK 0x200000  /* mask bit 21 */
#define DAMIX_INTR_STAT22_MASK 0x400000  /* mask bit 22 */
#define DAMIX_INTR_STAT23_MASK 0x800000  /* mask bit 23 */
#define DAMIX_INTR_STAT24_MASK 0x1000000 /* mask bit 24 */
#define DAMIX_INTR_STAT25_MASK 0x2000000 /* mask bit 25 */

#define REG_DAMIX_INTR_STATE 0x0194
#define DAMIX_INTR_STAT0  0x1       /* mask bit 0 */
#define DAMIX_INTR_STAT1  0x2       /* mask bit 1 */
#define DAMIX_INTR_STAT2  0x4       /* mask bit 2 */
#define DAMIX_INTR_STAT3  0x8       /* mask bit 3 */
#define DAMIX_INTR_STAT4  0x10      /* mask bit 4 */
#define DAMIX_INTR_STAT5  0x20      /* mask bit 5 */
#define DAMIX_INTR_STAT6  0x40      /* mask bit 6 */
#define DAMIX_INTR_STAT7  0x80      /* mask bit 7 */
#define DAMIX_INTR_STAT8  0x100     /* mask bit 8 */
#define DAMIX_INTR_STAT9  0x200     /* mask bit 9 */
#define DAMIX_INTR_STAT10 0x400     /* mask bit 10 */
#define DAMIX_INTR_STAT11 0x800     /* mask bit 11 */
#define DAMIX_INTR_STAT12 0x1000    /* mask bit 12 */
#define DAMIX_INTR_STAT13 0x2000    /* mask bit 13 */
#define DAMIX_INTR_STAT14 0x4000    /* mask bit 14 */
#define DAMIX_INTR_STAT15 0x8000    /* mask bit 15 */
#define DAMIX_INTR_STAT16 0x10000   /* mask bit 16 */
#define DAMIX_INTR_STAT17 0x20000   /* mask bit 17 */
#define DAMIX_INTR_STAT18 0x40000   /* mask bit 18 */
#define DAMIX_INTR_STAT19 0x80000   /* mask bit 19 */
#define DAMIX_INTR_STAT20 0x100000  /* mask bit 20 */
#define DAMIX_INTR_STAT21 0x200000  /* mask bit 21 */
#define DAMIX_INTR_STAT22 0x400000  /* mask bit 22 */
#define DAMIX_INTR_STAT23 0x800000  /* mask bit 23 */
#define DAMIX_INTR_STAT24 0x1000000 /* mask bit 24 */
#define DAMIX_INTR_STAT25 0x2000000 /* mask bit 25 */

#define REG_CLK_DETECT2_LANE1 0x0198
#define CLK_CNT3_LANE1 0xFF       /* mask bit 7:0 */
#define CLK_CNT2_LANE1 0xFF00     /* mask bit 15:8 */
#define CLK_CNT1_LANE1 0xFFFF0000 /* mask bit 31:16 */

#define REG_CLK_DETECT2_LANE2 0x019C
#define CLK_CNT3_LANE2 0xFF       /* mask bit 7:0 */
#define CLK_CNT2_LANE2 0xFF00     /* mask bit 15:8 */
#define CLK_CNT1_LANE2 0xFFFF0000 /* mask bit 31:16 */

#define REG_CLK_DETECT2_LANE3 0x01A0
#define CLK_CNT3_LANE3 0xFF       /* mask bit 7:0 */
#define CLK_CNT2_LANE3 0xFF00     /* mask bit 15:8 */
#define CLK_CNT1_LANE3 0xFFFF0000 /* mask bit 31:16 */

#define REG_ANALOG21 0x01A4
#define RG_TEST_PLL_L2_HIGH 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG22 0x01A8
#define RG_TEST_PLL_L1_HIGH 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG23 0x01AC
#define RG_TEST_PLL_L0_HIGH 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG24 0x01B0
#define RG_TEST_EQ_L2_HIGH 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG25 0x01B4
#define RG_TEST_EQ_L1_HIGH 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG26 0x01B8
#define RG_TEST_EQ_L0_HIGH 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG27 0x01BC
#define RG_TEST_EQ_L3_LOW 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG28 0x01C0
#define RG_TEST_EQ_L3_HIGH 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG29 0x01C4
#define RG_TEST_PLL_L3_LOW 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG30 0x01C8
#define RG_TEST_PLL_L3_HIGH 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG31 0x01CC
#define RG_TEST_ATOP_HIGH 0xFFFFFFFF /* mask bit 31:0 */

#define REG_ANALOG32 0x01D0
#define AD_RO_TEST 0xFFFFFFFF /* mask bit 31:0 */

#define REG_AREG11 0x01D4
#define DA_GC_BW_SEL_L0    0x7        /* mask bit 2:0 */
#define DA_GC_BW_SEL_L1    0x38       /* mask bit 5:3 */
#define DA_GC_BW_SEL_L2    0x1C0      /* mask bit 8:6 */
#define DA_GC_BW_SEL_L3    0xE00      /* mask bit 11:9 */
#define DA_GC_ICTRL_PFD_L3 0xFF000    /* mask bit 19:12 */
#define DA_GC_EQ_CODE_L3   0x1F00000  /* mask bit 24:20 */
#define DA_GC_EN_FRL       0x2000000  /* mask bit 25 */
#define FRL_LANE_SEL_HW    0x4000000  /* mask bit 26 */
#define FRL_RATE_HW        0x78000000 /* mask bit 30:27 */

#define REG_D2A10 0x01D8
#define REG_DA_CR_LOCK       0xF        /* mask bit 3:0 */
#define REG_DA_CR_LOCK_EN    0xF0       /* mask bit 7:4 */
#define REG_DA_BW_SEL3       0x700      /* mask bit 10:8 */
#define REG_DA_BW_SEL_EN3    0x800      /* mask bit 11 */
#define REG_DA_BW_SEL2       0x7000     /* mask bit 14:12 */
#define REG_DA_BW_SEL_EN2    0x8000     /* mask bit 15 */
#define REG_DA_BW_SEL1       0x70000    /* mask bit 18:16 */
#define REG_DA_BW_SEL_EN1    0x80000    /* mask bit 19 */
#define REG_DA_BW_SEL0       0x700000   /* mask bit 22:20 */
#define REG_DA_BW_SEL_EN0    0x800000   /* mask bit 23 */
#define REG_DA_RCTRL_PLL3    0xF000000  /* mask bit 27:24 */
#define REG_DA_RCTRL_PLL_EN3 0x10000000 /* mask bit 28 */

#define REG_D2A11 0x01DC
#define REG_DA_DIVSEL_POST3    0x7       /* mask bit 2:0 */
#define REG_DA_DIVSEL_POST_EN3 0x8       /* mask bit 3 */
#define REG_DA_DIVSEL_PLL3     0x1F0     /* mask bit 8:4 */
#define REG_DA_DIVSEL_PLL_EN3  0x200     /* mask bit 9 */
#define REG_DA_ICTRL_PD3       0x3FC00   /* mask bit 17:10 */
#define REG_DA_ICTRL_PD_EN3    0x40000   /* mask bit 18 */
#define REG_DA_ICTRL_PFD3      0x7F80000 /* mask bit 26:19 */
#define REG_DA_ICTRL_PFD_EN3   0x8000000 /* mask bit 27 */

#define REG_D2A12 0x01E0
#define REG_DA_RESET_VCO         0xF       /* mask bit 3:0 */
#define REG_DA_RESET_VCO_EN      0xF0      /* mask bit 7:4 */
#define RG_EN_IDAC_CAL_IN_L3     0x3F00    /* mask bit 13:8 */
#define REG_DAMIX_CLK_DET_SEL    0x10000   /* mask bit 16 */
#define REG_DAMIX_CLK_DET_SEL_EN 0x20000   /* mask bit 17 */
#define REG_EN_EDFE              0x40000   /* mask bit 18 */
#define REG_DAMIX_FRL_RATE       0xF00000  /* mask bit 23:20 */
#define REG_DAMIX_FRL_RATE_EN    0x1000000 /* mask bit 24 */
#define RG_SEL_CAL               0xE000000 /* mask bit 27:25 */

#define REG_ANALOG33 0x01E4
#define RG_PD_LDO_NODIE      0x1 /* mask bit 0 */
#define RG_EN_LDO_NODIE_TEST 0x2 /* mask bit 1 */
#define RG_SEL_LDO_NODIE     0xC /* mask bit 3:2 */

#define REG_PLL_LOCK4 0x01F4
#define PLL_CNT_LANE3 0x1FFFF /* mask bit 16:0 */

#define REG_SWEEP_EQ_RANGE_CH3 0x01F8
#define EQ_SWEEP_MIN_CH3 0x1F  /* mask bit 4:0 */
#define EQ_SWEEP_MAX_CH3 0x3E0 /* mask bit 9:5 */

#define REG_CLK_FREQ_DET_LANE1 0x01FC
#define CLK_FREQ_HW_LANE1     0xFFFF  /* mask bit 15:0 */
#define CLK_FREQ_STABLE_LANE1 0x10000 /* mask bit 16 */

#define REG_CLK_FREQ_DET_LANE2 0x0200
#define CLK_FREQ_HW_LANE2     0xFFFF  /* mask bit 15:0 */
#define CLK_FREQ_STABLE_LANE2 0x10000 /* mask bit 16 */

#define REG_CLK_FREQ_DET_LANE3 0x0204
#define CLK_FREQ_HW_LANE3     0xFFFF  /* mask bit 15:0 */
#define CLK_FREQ_STABLE_LANE3 0x10000 /* mask bit 16 */

#define REG_FINE_EQ_RANGE0 0x0208
#define FINE_EQ_MAX_LANE0 0x1F       /* mask bit 4:0 */
#define FINE_EQ_MIN_LANE0 0x1F00     /* mask bit 12:8 */
#define FINE_EQ_MAX_LANE1 0x1F0000   /* mask bit 20:16 */
#define FINE_EQ_MIN_LANE1 0x1F000000 /* mask bit 28:24 */

#define REG_FINE_EQ_RANGE1 0x020C
#define FINE_EQ_MAX_LANE2 0x1F       /* mask bit 4:0 */
#define FINE_EQ_MIN_LANE2 0x1F00     /* mask bit 12:8 */
#define FINE_EQ_MAX_LANE3 0x1F0000   /* mask bit 20:16 */
#define FINE_EQ_MIN_LANE3 0x1F000000 /* mask bit 28:24 */

#define REG_FRL_AUTO_EQ_CFG0_LANE0 0x0210
#define CFG_BYPASS_COARSE_TUNE_LANE0     0x1     /* mask bit 0 */
#define CFG_BYPASS_PHASE_LOCK_LANE0      0x2     /* mask bit 1 */
#define CFG_COARSE_TUNE_CONDITIONS_LANE0 0xFC    /* mask bit 7:2 */
#define CFG_SWEEP_EQ_CONDITIONS_LANE0    0x3F00  /* mask bit 13:8 */
#define CFG_FRL_EQ_MODE                  0x4000  /* mask bit 14 */
#define CFG_EQ_CLK_LOCK_WAIT_SEL         0x8000  /* mask bit 15 */
#define CFG_SYMBOL_TIME_OUT_SEL          0x10000 /* mask bit 16 */

#define REG_FRL_AUTO_EQ_CFG1_LANE0 0x0214
#define CFG_COARSE_SUCCESS_TIME_LANE0 0xFFFF  /* mask bit 15:0 */
#define CFG_FRL_FINE_EQ_CLR_LANE0     0x10000 /* mask bit 16 */

#define REG_FRL_AUTO_EQ_CFG0_LANE1 0x0218
#define CFG_BYPASS_COARSE_TUNE_LANE1     0x1    /* mask bit 0 */
#define CFG_BYPASS_PHASE_LOCK_LANE1      0x2    /* mask bit 1 */
#define CFG_COARSE_TUNE_CONDITIONS_LANE1 0xFC   /* mask bit 7:2 */
#define CFG_SWEEP_EQ_CONDITIONS_LANE1    0x3F00 /* mask bit 13:8 */

#define REG_FRL_AUTO_EQ_CFG1_LANE1 0x021C
#define CFG_COARSE_SUCCESS_TIME_LANE1 0xFFFF  /* mask bit 15:0 */
#define CFG_FRL_FINE_EQ_CLR_LANE1     0x10000 /* mask bit 16 */

#define REG_FRL_AUTO_EQ_CFG0_LANE2 0x0220
#define CFG_BYPASS_COARSE_TUNE_LANE2     0x1    /* mask bit 0 */
#define CFG_BYPASS_PHASE_LOCK_LANE2      0x2    /* mask bit 1 */
#define CFG_COARSE_TUNE_CONDITIONS_LANE2 0xFC   /* mask bit 7:2 */
#define CFG_SWEEP_EQ_CONDITIONS_LANE2    0x3F00 /* mask bit 13:8 */

#define REG_FRL_AUTO_EQ_CFG1_LANE2 0x0224
#define CFG_COARSE_SUCCESS_TIME_LANE2 0xFFFF  /* mask bit 15:0 */
#define CFG_FRL_FINE_EQ_CLR_LANE2     0x10000 /* mask bit 16 */

#define REG_FRL_AUTO_EQ_CFG0_LANE3 0x0228
#define CFG_BYPASS_COARSE_TUNE_LANE3     0x1    /* mask bit 0 */
#define CFG_BYPASS_PHASE_LOCK_LANE3      0x2    /* mask bit 1 */
#define CFG_COARSE_TUNE_CONDITIONS_LANE3 0xFC   /* mask bit 7:2 */
#define CFG_SWEEP_EQ_CONDITIONS_LANE3    0x3F00 /* mask bit 13:8 */

#define REG_FRL_AUTO_EQ_CFG1_LANE3 0x022C
#define CFG_COARSE_SUCCESS_TIME_LANE3 0xFFFF  /* mask bit 15:0 */
#define CFG_FRL_FINE_EQ_CLR_LANE3     0x10000 /* mask bit 16 */

#define REG_FRL_AUTO_ST 0x0230
#define CHARACTER_LOCK_CH0       0x1       /* mask bit 0 */
#define LANE0_LOCK               0x2       /* mask bit 1 */
#define LANE0_LTP58_DUTY_OK      0x4       /* mask bit 2 */
#define CHARACTER_LOCK_CH1       0x8       /* mask bit 3 */
#define LANE1_LOCK               0x10      /* mask bit 4 */
#define LANE1_LTP58_DUTY_OK      0x20      /* mask bit 5 */
#define CHARACTER_LOCK_CH2       0x40      /* mask bit 6 */
#define LANE2_LOCK               0x80      /* mask bit 7 */
#define LANE2_LTP58_DUTY_OK      0x100     /* mask bit 8 */
#define CHARACTER_LOCK_CH3       0x200     /* mask bit 9 */
#define LANE3_LOCK               0x400     /* mask bit 10 */
#define LANE3_LTP58_DUTY_OK      0x800     /* mask bit 11 */
#define LTP2COARSE_LN_BIT_CHK_OK 0xF000    /* mask bit 15:12 */
#define LTP2COARSE_LN_PAT_CHK_OK 0xF0000   /* mask bit 19:16 */
#define LTP2SWEEP_LN_BIT_CHK_OK  0xF00000  /* mask bit 23:20 */
#define LTP2SWEEP_LN_PAT_CHK_OK  0xF000000 /* mask bit 27:24 */

#define REG_PRBS_DET_CFG 0x0234
#define CFG_PRBS_MODE_LANE0 0x3   /* mask bit 1:0 */
#define CFG_PRBS_MODE_LANE1 0xC   /* mask bit 3:2 */
#define CFG_PRBS_MODE_LANE2 0x30  /* mask bit 5:4 */
#define CFG_PRBS_MODE_LANE3 0xC0  /* mask bit 7:6 */
#define CFG_PRBS_DET_EN     0x100 /* mask bit 8 */

#define REG_FREQ_DET_HW_0 0x0238
#define CFG_FREQ_DET_CLR 0x1 /* mask bit 0 */

#define REG_FREQ_DET_HW_1 0x023C
#define FREQ_DET_HW 0xFFFFF /* mask bit 19:0 */

#define REG_FREQ_DET_HW_2 0x0240
#define FREQ_DET_HW_MAX 0xFFFFF /* mask bit 19:0 */

#define REG_FREQ_DET_HW_3 0x0244
#define FREQ_DET_HW_MIN 0xFFFFF /* mask bit 19:0 */

#define REG_APHY_IN_DATA_CFG 0x0254
#define CFG_APHY_DATA_REVERT_EN 0x1 /* mask bit 0 */
#define CFG_APHY_DATA_INVERT_EN 0x2 /* mask bit 1 */

#define REG_DFE_CFG0_LANE0 0x0258
#define CFG_DTAP_SW_LANE0           0x3F       /* mask bit 5:0 */
#define CFG_DTAP_OVR_LANE0          0x40       /* mask bit 6 */
#define CFG_TTAP_SW_LANE0           0x3F00     /* mask bit 13:8 */
#define CFG_TTAP_OVR_LANE0          0x4000     /* mask bit 14 */
#define CFG_DFE_EN_SW_LANE0         0x10000    /* mask bit 16 */
#define CFG_DFE_EN_OVR_LANE0        0x20000    /* mask bit 17 */
#define CFG_DFE_JUDGE_MASK_LANE0    0x3C0000   /* mask bit 21:18 */
#define CFG_DFE_MODE_LANE0          0xC00000   /* mask bit 23:22 */
#define CFG_DFE_SW_START_LANE0      0x1000000  /* mask bit 24 */
#define CFG_DFE_DONE_MODE_LANE0     0x2000000  /* mask bit 25 */
#define CFG_DFE_UNSTABLE_MODE_LANE0 0x4000000  /* mask bit 26 */
#define CFG_DFE_ST_CLR_LANE0        0x8000000  /* mask bit 27 */
#define CFG_DFE_VALUE_CLR_LANE0     0x10000000 /* mask bit 28 */

#define REG_DFE_CFG1_LANE0 0x025C
#define CFG_DTAP_THRESHOLD_LANE0      0xFFF      /* mask bit 11:0 */
#define CFG_TTAP_THRESHOLD_LANE0      0xFFF000   /* mask bit 23:12 */
#define CFG_DFE_STABLE_THRESHOD_LANE0 0x3F000000 /* mask bit 29:24 */

#define REG_DFE_CFG2_LANE0 0x0260
#define CFG_DFE_WAIT_CNT_LANE0    0x3FF   /* mask bit 9:0 */
#define CFG_DFE_TIMEOUT_CNT_LANE0 0xFFC00 /* mask bit 19:10 */

#define REG_DFE_CFG3_LANE0 0x0264
#define CFG_DFE_CHECK_PERIOD_LANE0       0xFFFF     /* mask bit 15:0 */
#define CFG_DFE_START_DELAY_PERIOD_LANE0 0xFFFF0000 /* mask bit 31:16 */

#define REG_DFE_VALUE_CFG_LANE0 0x0268
#define CFG_TTAP_DEFAULT_VALUE_LANE0 0x3F    /* mask bit 5:0 */
#define CFG_DTAP_DEFAULT_VALUE_LANE0 0xFC0   /* mask bit 11:6 */
#define CFG_ETAP_SW_LANE0            0x3F000 /* mask bit 17:12 */
#define CFG_ETAP_OVR_LANE0           0x40000 /* mask bit 18 */

#define REG_DFE_ST_LANE0 0x026C
#define TTAP_OF_ONCE_ST_LANE0       0x1   /* mask bit 0 */
#define TTAP_OF_TWICE_ST_LANE0      0x2   /* mask bit 1 */
#define TTAP_TIMEOUT_ST_LANE0       0x4   /* mask bit 2 */
#define TTAP_UNSTABLE_ST_LANE0      0x8   /* mask bit 3 */
#define TTAP_OF_AFTER_DONE_ST_LANE0 0x10  /* mask bit 4 */
#define DTAP_OF_ONCE_ST_LANE0       0x20  /* mask bit 5 */
#define DTAP_OF_TWICE_ST_LANE0      0x40  /* mask bit 6 */
#define DTAP_TIMEOUT_ST_LANE0       0x80  /* mask bit 7 */
#define DTAP_UNSTABLE_ST_LANE0      0x100 /* mask bit 8 */
#define DTAP_OF_AFTER_DONE_ST_LANE0 0x200 /* mask bit 9 */
#define TTAP_DTAP_OF_TWICE_ST_LANE0 0x400 /* mask bit 10 */

#define REG_DFE_VALUE_RD0_LANE0 0x0270
#define DTAP_MAX_LANE0    0x3F       /* mask bit 5:0 */
#define DTAP_MIN_LANE0    0x3F00     /* mask bit 13:8 */
#define DTAP_HW_LANE0     0x3F0000   /* mask bit 21:16 */
#define DA_GC_DFE_DTAP_L0 0x3F000000 /* mask bit 29:24 */
#define RG_EN_DDFE_L0     0x40000000 /* mask bit 30 */

#define REG_DFE_VALUE_RD1_LANE0 0x0274
#define DA_GC_DFE_VTH_L0  0x3F   /* mask bit 5:0 */
#define DA_GC_DFE_ETAP_L0 0x3F00 /* mask bit 13:8 */

#define REG_DFE_VALUE_RD2_LANE0 0x0278
#define TTAP_MAX_LANE0 0x3F     /* mask bit 5:0 */
#define TTAP_MIN_LANE0 0x3F00   /* mask bit 13:8 */
#define TTAP_HW_LANE0  0x3F0000 /* mask bit 21:16 */

#define REG_DFE_CFG0_LANE1 0x027C
#define CFG_DTAP_SW_LANE1           0x3F       /* mask bit 5:0 */
#define CFG_DTAP_OVR_LANE1          0x40       /* mask bit 6 */
#define CFG_TTAP_SW_LANE1           0x3F00     /* mask bit 13:8 */
#define CFG_TTAP_OVR_LANE1          0x4000     /* mask bit 14 */
#define CFG_DFE_EN_SW_LANE1         0x10000    /* mask bit 16 */
#define CFG_DFE_EN_OVR_LANE1        0x20000    /* mask bit 17 */
#define CFG_DFE_JUDGE_MASK_LANE1    0x3C0000   /* mask bit 21:18 */
#define CFG_DFE_MODE_LANE1          0xC00000   /* mask bit 23:22 */
#define CFG_DFE_SW_START_LANE1      0x1000000  /* mask bit 24 */
#define CFG_DFE_DONE_MODE_LANE1     0x2000000  /* mask bit 25 */
#define CFG_DFE_UNSTABLE_MODE_LANE1 0x4000000  /* mask bit 26 */
#define CFG_DFE_ST_CLR_LANE1        0x8000000  /* mask bit 27 */
#define CFG_DFE_VALUE_CLR_LANE1     0x10000000 /* mask bit 28 */

#define REG_DFE_CFG1_LANE1 0x0280
#define CFG_DTAP_THRESHOLD_LANE1      0xFFF      /* mask bit 11:0 */
#define CFG_TTAP_THRESHOLD_LANE1      0xFFF000   /* mask bit 23:12 */
#define CFG_DFE_STABLE_THRESHOD_LANE1 0x3F000000 /* mask bit 29:24 */

#define REG_DFE_CFG2_LANE1 0x0284
#define CFG_DFE_WAIT_CNT_LANE1    0x3FF   /* mask bit 9:0 */
#define CFG_DFE_TIMEOUT_CNT_LANE1 0xFFC00 /* mask bit 19:10 */

#define REG_DFE_CFG3_LANE1 0x0288
#define CFG_DFE_CHECK_PERIOD_LANE1       0xFFFF     /* mask bit 15:0 */
#define CFG_DFE_START_DELAY_PERIOD_LANE1 0xFFFF0000 /* mask bit 31:16 */

#define REG_DFE_VALUE_CFG_LANE1 0x028C
#define CFG_TTAP_DEFAULT_VALUE_LANE1 0x3F    /* mask bit 5:0 */
#define CFG_DTAP_DEFAULT_VALUE_LANE1 0xFC0   /* mask bit 11:6 */
#define CFG_ETAP_SW_LANE1            0x3F000 /* mask bit 17:12 */
#define CFG_ETAP_OVR_LANE1           0x40000 /* mask bit 18 */

#define REG_DFE_ST_LANE1 0x0290
#define TTAP_OF_ONCE_ST_LANE1       0x1   /* mask bit 0 */
#define TTAP_OF_TWICE_ST_LANE1      0x2   /* mask bit 1 */
#define TTAP_TIMEOUT_ST_LANE1       0x4   /* mask bit 2 */
#define TTAP_UNSTABLE_ST_LANE1      0x8   /* mask bit 3 */
#define TTAP_OF_AFTER_DONE_ST_LANE1 0x10  /* mask bit 4 */
#define DTAP_OF_ONCE_ST_LANE1       0x20  /* mask bit 5 */
#define DTAP_OF_TWICE_ST_LANE1      0x40  /* mask bit 6 */
#define DTAP_TIMEOUT_ST_LANE1       0x80  /* mask bit 7 */
#define DTAP_UNSTABLE_ST_LANE1      0x100 /* mask bit 8 */
#define DTAP_OF_AFTER_DONE_ST_LANE1 0x200 /* mask bit 9 */
#define TTAP_DTAP_OF_TWICE_ST_LANE1 0x400 /* mask bit 10 */

#define REG_DFE_VALUE_RD0_LANE1 0x0294
#define DTAP_MAX_LANE1    0x3F       /* mask bit 5:0 */
#define DTAP_MIN_LANE1    0x3F00     /* mask bit 13:8 */
#define DTAP_HW_LANE1     0x3F0000   /* mask bit 21:16 */
#define DA_GC_DFE_DTAP_L1 0x3F000000 /* mask bit 29:24 */
#define RG_EN_DDFE_L1     0x40000000 /* mask bit 30 */

#define REG_DFE_VALUE_RD1_LANE1 0x0298
#define DA_GC_DFE_VTH_L1  0x3F   /* mask bit 5:0 */
#define DA_GC_DFE_ETAP_L1 0x3F00 /* mask bit 13:8 */

#define REG_DFE_VALUE_RD2_LANE1 0x029C
#define TTAP_MAX_LANE1 0x3F     /* mask bit 5:0 */
#define TTAP_MIN_LANE1 0x3F00   /* mask bit 13:8 */
#define TTAP_HW_LANE1  0x3F0000 /* mask bit 21:16 */

#define REG_DFE_CFG0_LANE2 0x02A0
#define CFG_DTAP_SW_LANE2           0x3F       /* mask bit 5:0 */
#define CFG_DTAP_OVR_LANE2          0x40       /* mask bit 6 */
#define CFG_TTAP_SW_LANE2           0x3F00     /* mask bit 13:8 */
#define CFG_TTAP_OVR_LANE2          0x4000     /* mask bit 14 */
#define CFG_DFE_EN_SW_LANE2         0x10000    /* mask bit 16 */
#define CFG_DFE_EN_OVR_LANE2        0x20000    /* mask bit 17 */
#define CFG_DFE_JUDGE_MASK_LANE2    0x3C0000   /* mask bit 21:18 */
#define CFG_DFE_MODE_LANE2          0xC00000   /* mask bit 23:22 */
#define CFG_DFE_SW_START_LANE2      0x1000000  /* mask bit 24 */
#define CFG_DFE_DONE_MODE_LANE2     0x2000000  /* mask bit 25 */
#define CFG_DFE_UNSTABLE_MODE_LANE2 0x4000000  /* mask bit 26 */
#define CFG_DFE_ST_CLR_LANE2        0x8000000  /* mask bit 27 */
#define CFG_DFE_VALUE_CLR_LANE2     0x10000000 /* mask bit 28 */

#define REG_DFE_CFG1_LANE2 0x02A4
#define CFG_DTAP_THRESHOLD_LANE2      0xFFF      /* mask bit 11:0 */
#define CFG_TTAP_THRESHOLD_LANE2      0xFFF000   /* mask bit 23:12 */
#define CFG_DFE_STABLE_THRESHOD_LANE2 0x3F000000 /* mask bit 29:24 */

#define REG_DFE_CFG2_LANE2 0x02A8
#define CFG_DFE_WAIT_CNT_LANE2    0x3FF   /* mask bit 9:0 */
#define CFG_DFE_TIMEOUT_CNT_LANE2 0xFFC00 /* mask bit 19:10 */

#define REG_DFE_CFG3_LANE2 0x02AC
#define CFG_DFE_CHECK_PERIOD_LANE2       0xFFFF     /* mask bit 15:0 */
#define CFG_DFE_START_DELAY_PERIOD_LANE2 0xFFFF0000 /* mask bit 31:16 */

#define REG_DFE_VALUE_CFG_LANE2 0x02B0
#define CFG_TTAP_DEFAULT_VALUE_LANE2 0x3F    /* mask bit 5:0 */
#define CFG_DTAP_DEFAULT_VALUE_LANE2 0xFC0   /* mask bit 11:6 */
#define CFG_ETAP_SW_LANE2            0x3F000 /* mask bit 17:12 */
#define CFG_ETAP_OVR_LANE2           0x40000 /* mask bit 18 */

#define REG_DFE_ST_LANE2 0x02B4
#define TTAP_OF_ONCE_ST_LANE2       0x1   /* mask bit 0 */
#define TTAP_OF_TWICE_ST_LANE2      0x2   /* mask bit 1 */
#define TTAP_TIMEOUT_ST_LANE2       0x4   /* mask bit 2 */
#define TTAP_UNSTABLE_ST_LANE2      0x8   /* mask bit 3 */
#define TTAP_OF_AFTER_DONE_ST_LANE2 0x10  /* mask bit 4 */
#define DTAP_OF_ONCE_ST_LANE2       0x20  /* mask bit 5 */
#define DTAP_OF_TWICE_ST_LANE2      0x40  /* mask bit 6 */
#define DTAP_TIMEOUT_ST_LANE2       0x80  /* mask bit 7 */
#define DTAP_UNSTABLE_ST_LANE2      0x100 /* mask bit 8 */
#define DTAP_OF_AFTER_DONE_ST_LANE2 0x200 /* mask bit 9 */
#define TTAP_DTAP_OF_TWICE_ST_LANE2 0x400 /* mask bit 10 */

#define REG_DFE_VALUE_RD0_LANE2 0x02B8
#define DTAP_MAX_LANE2    0x3F       /* mask bit 5:0 */
#define DTAP_MIN_LANE2    0x3F00     /* mask bit 13:8 */
#define DTAP_HW_LANE2     0x3F0000   /* mask bit 21:16 */
#define DA_GC_DFE_DTAP_L2 0x3F000000 /* mask bit 29:24 */
#define RG_EN_DDFE_L2     0x40000000 /* mask bit 30 */

#define REG_DFE_VALUE_RD1_LANE2 0x02BC
#define DA_GC_DFE_VTH_L2  0x3F   /* mask bit 5:0 */
#define DA_GC_DFE_ETAP_L2 0x3F00 /* mask bit 13:8 */

#define REG_DFE_VALUE_RD2_LANE2 0x02C0
#define TTAP_MAX_LANE2 0x3F     /* mask bit 5:0 */
#define TTAP_MIN_LANE2 0x3F00   /* mask bit 13:8 */
#define TTAP_HW_LANE2  0x3F0000 /* mask bit 21:16 */

#define REG_DFE_CFG0_LANE3 0x02C4
#define CFG_DTAP_SW_LANE3           0x3F       /* mask bit 5:0 */
#define CFG_DTAP_OVR_LANE3          0x40       /* mask bit 6 */
#define CFG_TTAP_SW_LANE3           0x3F00     /* mask bit 13:8 */
#define CFG_TTAP_OVR_LANE3          0x4000     /* mask bit 14 */
#define CFG_DFE_EN_SW_LANE3         0x10000    /* mask bit 16 */
#define CFG_DFE_EN_OVR_LANE3        0x20000    /* mask bit 17 */
#define CFG_DFE_JUDGE_MASK_LANE3    0x3C0000   /* mask bit 21:18 */
#define CFG_DFE_MODE_LANE3          0xC00000   /* mask bit 23:22 */
#define CFG_DFE_SW_START_LANE3      0x1000000  /* mask bit 24 */
#define CFG_DFE_DONE_MODE_LANE3     0x2000000  /* mask bit 25 */
#define CFG_DFE_UNSTABLE_MODE_LANE3 0x4000000  /* mask bit 26 */
#define CFG_DFE_ST_CLR_LANE3        0x8000000  /* mask bit 27 */
#define CFG_DFE_VALUE_CLR_LANE3     0x10000000 /* mask bit 28 */

#define REG_DFE_CFG1_LANE3 0x02C8
#define CFG_DTAP_THRESHOLD_LANE3      0xFFF      /* mask bit 11:0 */
#define CFG_TTAP_THRESHOLD_LANE3      0xFFF000   /* mask bit 23:12 */
#define CFG_DFE_STABLE_THRESHOD_LANE3 0x3F000000 /* mask bit 29:24 */

#define REG_DFE_CFG2_LANE3 0x02CC
#define CFG_DFE_WAIT_CNT_LANE3    0x3FF   /* mask bit 9:0 */
#define CFG_DFE_TIMEOUT_CNT_LANE3 0xFFC00 /* mask bit 19:10 */

#define REG_DFE_CFG3_LANE3 0x02D0
#define CFG_DFE_CHECK_PERIOD_LANE3       0xFFFF     /* mask bit 15:0 */
#define CFG_DFE_START_DELAY_PERIOD_LANE3 0xFFFF0000 /* mask bit 31:16 */

#define REG_DFE_VALUE_CFG_LANE3 0x02D4
#define CFG_TTAP_DEFAULT_VALUE_LANE3 0x3F    /* mask bit 5:0 */
#define CFG_DTAP_DEFAULT_VALUE_LANE3 0xFC0   /* mask bit 11:6 */
#define CFG_ETAP_SW_LANE3            0x3F000 /* mask bit 17:12 */
#define CFG_ETAP_OVR_LANE3           0x40000 /* mask bit 18 */

#define REG_DFE_ST_LANE3 0x02D8
#define TTAP_OF_ONCE_ST_LANE3       0x1   /* mask bit 0 */
#define TTAP_OF_TWICE_ST_LANE3      0x2   /* mask bit 1 */
#define TTAP_TIMEOUT_ST_LANE3       0x4   /* mask bit 2 */
#define TTAP_UNSTABLE_ST_LANE3      0x8   /* mask bit 3 */
#define TTAP_OF_AFTER_DONE_ST_LANE3 0x10  /* mask bit 4 */
#define DTAP_OF_ONCE_ST_LANE3       0x20  /* mask bit 5 */
#define DTAP_OF_TWICE_ST_LANE3      0x40  /* mask bit 6 */
#define DTAP_TIMEOUT_ST_LANE3       0x80  /* mask bit 7 */
#define DTAP_UNSTABLE_ST_LANE3      0x100 /* mask bit 8 */
#define DTAP_OF_AFTER_DONE_ST_LANE3 0x200 /* mask bit 9 */
#define TTAP_DTAP_OF_TWICE_ST_LANE3 0x400 /* mask bit 10 */

#define REG_DFE_VALUE_RD0_LANE3 0x02DC
#define DTAP_MAX_LANE3    0x3F       /* mask bit 5:0 */
#define DTAP_MIN_LANE3    0x3F00     /* mask bit 13:8 */
#define DTAP_HW_LANE3     0x3F0000   /* mask bit 21:16 */
#define DA_GC_DFE_DTAP_L3 0x3F000000 /* mask bit 29:24 */
#define RG_EN_DDFE_L3     0x40000000 /* mask bit 30 */

#define REG_DFE_VALUE_RD1_LANE3 0x02E0
#define DA_GC_DFE_VTH_L3  0x3F   /* mask bit 5:0 */
#define DA_GC_DFE_ETAP_L3 0x3F00 /* mask bit 13:8 */

#define REG_DFE_VALUE_RD2_LANE3 0x02E4
#define TTAP_MAX_LANE3 0x3F     /* mask bit 5:0 */
#define TTAP_MIN_LANE3 0x3F00   /* mask bit 13:8 */
#define TTAP_HW_LANE3  0x3F0000 /* mask bit 21:16 */

#endif /* __HAL_HDMIRX_DAMIX_REG_H__ */
