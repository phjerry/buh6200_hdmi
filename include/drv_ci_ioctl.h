/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: drivers of drv_osal_sys
 * Author: ÕÔ¹ðºé z00268517
 * Create: 2014-08-02
 */
#ifndef __DRV_CI_IOCTL_H__
#define __DRV_CI_IOCTL_H__

#include "hi_mpi_ci.h"
#include "hi_drv_ci.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum hi_ci_pccd_volt {
    CI_PCCD_VOLT_5V,  /* 5V DC, default */
    CI_PCCD_VOLT_3V3, /* 3V3 DC */
    CI_PCCD_VOLT_MAX
} ci_pccd_volt;

typedef enum hi_ci_pccd_speed {
    CI_PCCD_SPEED_600NS, /* 600ns, default */
    CI_PCCD_SPEED_250NS, /* 250ns */
    CI_PCCD_SPEED_200NS, /* 200ns */
    CI_PCCD_SPEED_150NS, /* 150ns */
    CI_PCCD_SPEED_100NS, /* 100ns */
    CI_PCCD_SPEED_MAX
} ci_pccd_speed;

/* ! \union of compat addr */
typedef union {
    hi_void *p;                /* !<  virtual address */
    unsigned long long phy; /* !<  physical address */
    unsigned int word[2];   /* !<  2 word of address */
} ci_compat_addr;

typedef struct {
    ci_pccd_volt volt;   /* PCMCIA card voltage, 5v or 3v3 DC */
    ci_pccd_speed speed; /* PCMCIA card attribute read/write speed */
    hi_bool is_ciplus;
    hi_u32 ciprof;
} ci_pccd_attr;

typedef struct {
    hi_bool b_ci0;
    hi_bool b_ci1;
} ci_dev_state;

typedef struct {
    hi_ci_port port;
    hi_ci_pccd card;
    hi_u32 io_base_address;
    hi_u32 attr_base_address;
} ci_card_parameter;

typedef struct {
    hi_ci_port port;
    hi_ci_pccd card;
    hi_ci_pccd_ctrl_power ctrl_power;
    hi_u32 reserve;
} ci_pccd_ctrl_power;

typedef struct {
    hi_ci_port port;
    hi_ci_pccd card;
} ci_pccd;

/* added begin 2012-04-24: support various CI device */
typedef struct {
    hi_ci_port port;
    hi_ci_attr attr;
} ci_attr;

/* added end 2012-04-24: support various CI device */
typedef struct {
    hi_ci_port port;
    hi_ci_pccd card;
    hi_u32 ready;
    hi_u32 reserve;
} ci_pccd_ready;

typedef struct {
    hi_ci_port port;
    hi_ci_pccd card;
    hi_ci_pccd_status status;
    hi_u32 reserve;
} ci_pccd_detect;

typedef struct {
    hi_ci_port port;
    hi_ci_pccd card;
    hi_u8 status;
    hi_u8 reserve0;
    hi_u8 reserve1;
    hi_u8 reserve2;
    hi_u32 reserve3;
} ci_pccd_status;

typedef struct {
    hi_ci_port port;
    hi_ci_pccd card;
    ci_compat_addr buffer;
    hi_u32 buffer_len;
    hi_u32 read_len;
    hi_u32 reserve;
} ci_pccd_read;

typedef struct {
    hi_ci_port port;
    hi_ci_pccd card;
    ci_compat_addr buffer;
    hi_u32 write_ok_len;
    hi_u32 write_len;
} ci_pccd_write;

typedef struct {
    hi_ci_port port;
    hi_ci_pccd card;
    hi_u16 buffer_size;
    hi_u16 reserve0;
    hi_u32 reserve1;
} ci_pccd_buffer_size;

typedef struct {
    hi_ci_port port;
    hi_ci_pccd card;
    hi_ci_pccd_tsctrl cmd;
    hi_ci_pccd_tsctrl_param param;
} ci_pccd_tsctrl;

typedef struct {
    hi_ci_port port;
    hi_ci_pccd card;
    ci_compat_addr buffer;
    hi_u32 buffer_len;
    hi_u32 cis_len;
    hi_u32 reserve;
} ci_pccd_cis;

typedef struct {
    hi_ci_port port;
    hi_ci_pccd card;
    hi_u16 addr;
    hi_u8 data;
} ci_pccd_cor;

#define CMD_CI_OPEN  _IOW(HI_ID_CI, 0x1, hi_unf_ci_port)
#define CMD_CI_CLOSE _IOW(HI_ID_CI, 0x2, hi_unf_ci_port)

#define CMD_CI_PCCD_OPEN  _IOW(HI_ID_CI, 0x3, ci_pccd)
#define CMD_CI_PCCD_CLOSE _IOW(HI_ID_CI, 0x4, ci_pccd)

#define CMD_CI_PCCD_CTRLPOWER      _IOW(HI_ID_CI, 0x5, ci_pccd_ctrl_power)
#define CMD_CI_PCCD_RESET          _IOW(HI_ID_CI, 0x6, ci_pccd)
#define CMD_CI_PCCD_READY          _IOWR(HI_ID_CI, 0x7, ci_pccd_ready)
#define CMD_CI_PCCD_DETECT         _IOWR(HI_ID_CI, 0x8, ci_pccd_detect)
#define CMD_CI_PCCD_GET_STATUS     _IOWR(HI_ID_CI, 0xa, ci_pccd_status)
#define CMD_CI_PCCD_READ           _IOWR(HI_ID_CI, 0xb, ci_pccd_read)
#define CMD_CI_PCCD_WRITE          _IOWR(HI_ID_CI, 0xc, ci_pccd_write)
#define CMD_CI_PCCD_CHECKCIS       _IOW(HI_ID_CI, 0xd, ci_pccd)
#define CMD_CI_PCCD_WRITECOR       _IOW(HI_ID_CI, 0xe, ci_pccd)
#define CMD_CI_PCCD_IORESET        _IOW(HI_ID_CI, 0xf, ci_pccd)
#define CMD_CI_PCCD_NEG_BUFFERSIZE _IOWR(HI_ID_CI, 0x10, ci_pccd_buffer_size)
#define CMD_CI_PCCD_TSCTRL         _IOWR(HI_ID_CI, 0x11, ci_pccd_tsctrl)

/* added begin 2012-04-24: support various CI device */
#define CMD_CI_SETATTR _IOW(HI_ID_CI, 0x12, ci_attr)
#define CMD_CI_GETATTR _IOWR(HI_ID_CI, 0x13, ci_attr)
/* added end 2012-04-24: support various CI device */
#define CMD_CI_PCCD_GETCIS     _IOWR(HI_ID_CI, 0x14, ci_pccd_cis)
#define CMD_CI_PCCD_WRITECOREX _IOW(HI_ID_CI, 0x15, ci_pccd_cor)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* End of #ifndef __DRV_CI_IOCTL_H__ */

