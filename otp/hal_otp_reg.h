/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description:OTP register and structure definition.
 * Author: Linux SDK team
 * Create: 2019/06/21
 */
#ifndef __HAL_OTP_REG_H__
#define __HAL_OTP_REG_H__

#ifdef TCPU_SUPPORT
#define  CPU_OFF                0x400
#else
#define  CPU_OFF                0
#endif
#define PERM_REG_RANGE          0x1000
#define PERM_REG_BASE           (0x00b04000 + CPU_OFF)

#define OTP_RW_CTRL             (PERM_REG_BASE + 0x00)        /* OTP MEM read and write control */
#define OTP_RDATA               (PERM_REG_BASE + 0x20)
#define OTP_WDATA               (PERM_REG_BASE + 0x24)
#define OTP_RADDR               (PERM_REG_BASE + 0x28)
#define OTP_WADDR               (PERM_REG_BASE + 0x2C)
#define OTP_CTR_ST0             (PERM_REG_BASE + 0x34)        /* TP controller status */
#define OTP_SH_UPDATE           (PERM_REG_BASE + 0x40)        /* shadow register update request */

#define OTP_OPT_READ            0X01
#define OTP_OPT_WRITE           0X02
typedef union {
    struct {
        hi_u32 start              : 1;  /* [0] start OTP controller to operate. */
        hi_u32 wr_sel             : 3;  /* [1-3] read or write selection. 3'b001:read mode; 3'b010:write mode */
        hi_u32 reserved           : 28; /* [4-31] */
    } bits;
    hi_u32 u32;
} otp_rw_ctrl;

typedef union {
    struct {
        hi_u32 otp_init_rdy       : 1; /* [0] OTP initialization finish flag. 0:not finished; 1:finished */
        hi_u32 soft_req_otp_rdy   : 1; /* [1] */
        hi_u32 reserved0          : 2; /* [2-3] */
        hi_u32 err                : 1; /* [4] hostCPU read or the program permission check fail.
                                          If secret_status_disable does not equal 0x42, this bit will always be 0. */
        hi_u32 prm_rd_fail        : 1; /* [5] after hostCPU program, data read back check fail.
                                          If secret_status_disable does not equal 0x42, this bit will always be 0. */
        hi_u32 rd_fail            : 1; /* [6] hostCPU read fail, read operation abnormal.
                                          If secret_status_disable does not equal 0x42, this bit will always be 0. */
        hi_u32 prog_disable       : 1; /* [7] OTP fuse otp_global_lock_en is 1 and hcpu start programing OTP. */
        hi_u32 otp_init_err       : 1; /* [8] OTP initialization error flag. */
        hi_u32 reserved1          : 23; /* [9-31] */
    } bits;
    hi_u32 u32;
} otp_ctr_st0;

#endif /* __HAL_OTP_REG_H__ */
