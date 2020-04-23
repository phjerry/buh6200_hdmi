/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: Register definition of mcu cpu module
 * Author: Hisilicon multimedia interface software group
 * Create: 2019/12/4
 */
#ifndef __HAL_HDMIRX_MCU_CPU_REG_H__
#define __HAL_HDMIRX_MCU_CPU_REG_H__

#define REG_HDCP2X_HW_VER_B0 0x8800
#define HDCP2X_HW_VER_B0 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_HW_VER_B1 0x8804
#define HDCP2X_HW_VER_B1 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_HW_VER_B2 0x8808
#define HDCP2X_HW_VER_B2 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_HW_VER_B3 0x880C
#define HDCP2X_HW_VER_B3 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SW_VER_B0 0x8810
#define HDCP2X_SW_VER_B0 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SW_VER_B1 0x8814
#define HDCP2X_SW_VER_B1 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SW_VER_B2 0x8818
#define HDCP2X_SW_VER_B2 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SW_VER_B3 0x881C
#define HDCP2X_SW_VER_B3 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_INTR_B0 0x8840
#define HDCP2X_SEC_INTR0 0x1  /* mask bit 0 */
#define HDCP2X_SEC_INTR1 0x2  /* mask bit 1 */
#define HDCP2X_SEC_INTR2 0x4  /* mask bit 2 */
#define HDCP2X_SEC_INTR3 0x8  /* mask bit 3 */
#define HDCP2X_SEC_INTR4 0x10 /* mask bit 4 */
#define HDCP2X_SEC_INTR5 0x20 /* mask bit 5 */
#define HDCP2X_SEC_INTR6 0x40 /* mask bit 6 */
#define HDCP2X_SEC_INTR7 0x80 /* mask bit 7 */

#define REG_HDCP2X_SEC_INTR_B1 0x8844
#define HDCP2X_SEC_INTR8  0x1  /* mask bit 0 */
#define HDCP2X_SEC_INTR9  0x2  /* mask bit 1 */
#define HDCP2X_SEC_INTR10 0x4  /* mask bit 2 */
#define HDCP2X_SEC_INTR11 0x8  /* mask bit 3 */
#define HDCP2X_SEC_INTR12 0x10 /* mask bit 4 */
#define HDCP2X_SEC_INTR13 0x20 /* mask bit 5 */
#define HDCP2X_SEC_INTR14 0x40 /* mask bit 6 */
#define HDCP2X_SEC_INTR15 0x80 /* mask bit 7 */

#define REG_HDCP2X_SEC_MASK_B0 0x8848
#define HDCP2X_SEC_MASK0 0x1  /* mask bit 0 */
#define HDCP2X_SEC_MASK1 0x2  /* mask bit 1 */
#define HDCP2X_SEC_MASK2 0x4  /* mask bit 2 */
#define HDCP2X_SEC_MASK3 0x8  /* mask bit 3 */
#define HDCP2X_SEC_MASK4 0x10 /* mask bit 4 */
#define HDCP2X_SEC_MASK5 0x20 /* mask bit 5 */
#define HDCP2X_SEC_MASK6 0x40 /* mask bit 6 */
#define HDCP2X_SEC_MASK7 0x80 /* mask bit 7 */

#define REG_HDCP2X_SEC_MASK_B1 0x884C
#define HDCP2X_SEC_MASK8  0x1  /* mask bit 0 */
#define HDCP2X_SEC_MASK9  0x2  /* mask bit 1 */
#define HDCP2X_SEC_MASK10 0x4  /* mask bit 2 */
#define HDCP2X_SEC_MASK11 0x8  /* mask bit 3 */
#define HDCP2X_SEC_MASK12 0x10 /* mask bit 4 */
#define HDCP2X_SEC_MASK13 0x20 /* mask bit 5 */
#define HDCP2X_SEC_MASK14 0x40 /* mask bit 6 */
#define HDCP2X_SEC_MASK15 0x80 /* mask bit 7 */

#define REG_HDCP2X_MCU_INTR_B0 0x8860
#define HDCP2X_MCU_INTR0 0x1  /* mask bit 0 */
#define HDCP2X_MCU_INTR1 0x2  /* mask bit 1 */
#define HDCP2X_MCU_INTR2 0x4  /* mask bit 2 */
#define HDCP2X_MCU_INTR3 0x8  /* mask bit 3 */
#define HDCP2X_MCU_INTR4 0x10 /* mask bit 4 */
#define HDCP2X_MCU_INTR5 0x20 /* mask bit 5 */
#define HDCP2X_MCU_INTR6 0x40 /* mask bit 6 */
#define HDCP2X_MCU_INTR7 0x80 /* mask bit 7 */

#define REG_HDCP2X_MCU_INTR_B1 0x8864
#define HDCP2X_MCU_INTR8  0x1  /* mask bit 0 */
#define HDCP2X_MCU_INTR9  0x2  /* mask bit 1 */
#define HDCP2X_MCU_INTR10 0x4  /* mask bit 2 */
#define HDCP2X_MCU_INTR11 0x8  /* mask bit 3 */
#define HDCP2X_MCU_INTR12 0x10 /* mask bit 4 */
#define HDCP2X_MCU_INTR13 0x20 /* mask bit 5 */
#define HDCP2X_MCU_INTR14 0x40 /* mask bit 6 */
#define HDCP2X_MCU_INTR15 0x80 /* mask bit 7 */

#define REG_HDCP2X_MCU_MASK_B0 0x8870
#define HDCP2X_MCU_MASK0 0x1  /* mask bit 0 */
#define HDCP2X_MCU_MASK1 0x2  /* mask bit 1 */
#define HDCP2X_MCU_MASK2 0x4  /* mask bit 2 */
#define HDCP2X_MCU_MASK3 0x8  /* mask bit 3 */
#define HDCP2X_MCU_MASK4 0x10 /* mask bit 4 */
#define HDCP2X_MCU_MASK5 0x20 /* mask bit 5 */
#define HDCP2X_MCU_MASK6 0x40 /* mask bit 6 */
#define HDCP2X_MCU_MASK7 0x80 /* mask bit 7 */

#define REG_HDCP2X_MCU_MASK_B1 0x8874
#define HDCP2X_MCU_MASK8  0x1  /* mask bit 0 */
#define HDCP2X_MCU_MASK9  0x2  /* mask bit 1 */
#define HDCP2X_MCU_MASK10 0x4  /* mask bit 2 */
#define HDCP2X_MCU_MASK11 0x8  /* mask bit 3 */
#define HDCP2X_MCU_MASK12 0x10 /* mask bit 4 */
#define HDCP2X_MCU_MASK13 0x20 /* mask bit 5 */
#define HDCP2X_MCU_MASK14 0x40 /* mask bit 6 */
#define HDCP2X_MCU_MASK15 0x80 /* mask bit 7 */

#define REG_HDCP2X_MCU_STATE_B0 0x8880
#define HDCP2X_MCU_STATE_B0 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_MCU_STATE_B1 0x8884
#define HDCP2X_MCU_STATE_B1 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_MCU_STATE_B2 0x8888
#define HDCP2X_MCU_STATE_B2 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_MCU_STATE_B3 0x888C
#define HDCP2X_MCU_STATE_B3 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_AUTH_STATE0 0x8898
#define HDCP2X_AUTH_STATE0 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_AUTH_STATE1 0x889C
#define HDCP2X_AUTH_STATE1 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_AUTH_STATE2 0x88A0
#define HDCP2X_AUTH_STATE2 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_AUTH_STATE3 0x88A4
#define HDCP2X_AUTH_STATE3 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN0 0x8900
#define HDCP2X_GEN_IN0 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN1 0x8904
#define HDCP2X_GEN_IN1 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN2 0x8908
#define HDCP2X_GEN_IN2 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN3 0x890C
#define HDCP2X_GEN_IN3 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN4 0x8910
#define HDCP2X_GEN_IN4 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN5 0x8914
#define HDCP2X_GEN_IN5 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN6 0x8918
#define HDCP2X_GEN_IN6 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN7 0x891C
#define HDCP2X_GEN_IN7 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN8 0x8920
#define HDCP2X_GEN_IN8 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN9 0x8924
#define HDCP2X_GEN_IN9 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN10 0x8928
#define HDCP2X_GEN_IN10 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN11 0x892C
#define HDCP2X_GEN_IN11 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN12 0x8930
#define HDCP2X_GEN_IN12 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN13 0x8934
#define HDCP2X_GEN_IN13 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN14 0x8938
#define HDCP2X_GEN_IN14 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN15 0x893C
#define HDCP2X_GEN_IN15 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN16 0x8940
#define HDCP2X_GEN_IN16 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN17 0x8944
#define HDCP2X_GEN_IN17 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN18 0x8948
#define HDCP2X_GEN_IN18 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN19 0x894C
#define HDCP2X_GEN_IN19 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN20 0x8950
#define HDCP2X_GEN_IN20 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN21 0x8954
#define HDCP2X_GEN_IN21 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN22 0x8958
#define HDCP2X_GEN_IN22 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN23 0x895C
#define HDCP2X_GEN_IN23 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN24 0x8960
#define HDCP2X_GEN_IN24 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN25 0x8964
#define HDCP2X_GEN_IN25 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN26 0x8968
#define HDCP2X_GEN_IN26 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN27 0x896C
#define HDCP2X_GEN_IN27 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN28 0x8970
#define HDCP2X_GEN_IN28 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN29 0x8974
#define HDCP2X_GEN_IN29 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN30 0x8978
#define HDCP2X_GEN_IN30 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_IN31 0x897C
#define HDCP2X_GEN_IN31 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT0 0x8980
#define HDCP2X_GEN_OUT0 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT1 0x8984
#define HDCP2X_GEN_OUT1 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT2 0x8988
#define HDCP2X_GEN_OUT2 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT3 0x898C
#define HDCP2X_GEN_OUT3 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT4 0x8990
#define HDCP2X_GEN_OUT4 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT5 0x8994
#define HDCP2X_GEN_OUT5 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT6 0x8998
#define HDCP2X_GEN_OUT6 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT7 0x899C
#define HDCP2X_GEN_OUT7 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT8 0x89A0
#define HDCP2X_GEN_OUT8 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT9 0x89A4
#define HDCP2X_GEN_OUT9 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT10 0x89A8
#define HDCP2X_GEN_OUT10 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT11 0x89AC
#define HDCP2X_GEN_OUT11 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT12 0x89B0
#define HDCP2X_GEN_OUT12 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT13 0x89B4
#define HDCP2X_GEN_OUT13 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT14 0x89B8
#define HDCP2X_GEN_OUT14 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT15 0x89BC
#define HDCP2X_GEN_OUT15 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT16 0x89C0
#define HDCP2X_GEN_OUT16 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT17 0x89C4
#define HDCP2X_GEN_OUT17 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT18 0x89C8
#define HDCP2X_GEN_OUT18 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT19 0x89CC
#define HDCP2X_GEN_OUT19 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT20 0x89D0
#define HDCP2X_GEN_OUT20 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT21 0x89D4
#define HDCP2X_GEN_OUT21 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT22 0x89D8
#define HDCP2X_GEN_OUT22 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT23 0x89DC
#define HDCP2X_GEN_OUT23 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT24 0x89E0
#define HDCP2X_GEN_OUT24 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT25 0x89E4
#define HDCP2X_GEN_OUT25 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT26 0x89E8
#define HDCP2X_GEN_OUT26 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT27 0x89EC
#define HDCP2X_GEN_OUT27 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT28 0x89F0
#define HDCP2X_GEN_OUT28 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT29 0x89F4
#define HDCP2X_GEN_OUT29 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT30 0x89F8
#define HDCP2X_GEN_OUT30 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_GEN_OUT31 0x89FC
#define HDCP2X_GEN_OUT31 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_IN0 0x8A40
#define HDCP2X_SEC_IN0 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_IN1 0x8A44
#define HDCP2X_SEC_IN1 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_IN2 0x8A48
#define HDCP2X_SEC_IN2 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_IN3 0x8A4C
#define HDCP2X_SEC_IN3 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_IN4 0x8A50
#define HDCP2X_SEC_IN4 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_IN5 0x8A54
#define HDCP2X_SEC_IN5 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_IN6 0x8A58
#define HDCP2X_SEC_IN6 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_IN7 0x8A5C
#define HDCP2X_SEC_IN7 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_OUT0 0x8A60
#define HDCP2X_SEC_OUT0 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_OUT1 0x8A64
#define HDCP2X_SEC_OUT1 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_OUT2 0x8A68
#define HDCP2X_SEC_OUT2 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_OUT3 0x8A6C
#define HDCP2X_SEC_OUT3 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_OUT4 0x8A70
#define HDCP2X_SEC_OUT4 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_OUT5 0x8A74
#define HDCP2X_SEC_OUT5 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_OUT6 0x8A78
#define HDCP2X_SEC_OUT6 0xFF /* mask bit 7:0 */

#define REG_HDCP2X_SEC_OUT7 0x8A7C
#define HDCP2X_SEC_OUT7 0xFF /* mask bit 7:0 */

#endif /* __HAL_HDMIRX_MCU_CPU_REG_H__ */
