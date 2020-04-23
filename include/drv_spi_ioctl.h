/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
* Description:
*/

#ifndef __DRV_SPI_IOCTL_H__
#define __DRV_SPI_IOCTL_H__

#include "hi_type.h"

typedef struct
{
    hi_u8   devId;
    hi_u8  *sData;
    hi_u32  sDataCnt;
} SPI_DATA_S;

typedef struct
{
    hi_u8   devId;
    hi_u32  sData;
    hi_u32  sDataCnt;
} SPI_DATA_COMPAT_S;

typedef struct {
    hi_u8     devId;
    hi_u8     *sData;
    hi_u32    sDataCnt;
    hi_u8     *rData;
    hi_u32    rDataCnt;
} SPI_DATAEX_S;

typedef struct {
    hi_u8     devId;
    hi_u32    sData;
    hi_u32    sDataCnt;
    hi_u32    rData;
    hi_u32    rDataCnt;
} SPI_DATAEX_COMPAT_S;

typedef struct
{
    hi_u8   devId;
    hi_u8   mode;
    hi_u8   spo;
    hi_u8   sph;
    hi_u8   dss;
    hi_u8   cscfg;
} SPI_FFORM_S;

typedef struct {
    hi_u8   devId;
    hi_bool   set_bend;
} SPI_BLEND_S;

typedef struct {
    hi_u8   devId;
    hi_u8   setLoop;
} SPI_LOOP_S;

#define CMD_SPI_READ            _IOWR(HI_ID_SPI, 0x1, SPI_DATA_S)
#define CMD_SPI_WRITE           _IOWR(HI_ID_SPI, 0x2, SPI_DATA_S)
#define CMD_SPI_SET_ATTR        _IOWR(HI_ID_SPI, 0x3, SPI_FFORM_S)
#define CMD_SPI_GET_ATTR        _IOWR(HI_ID_SPI, 0x4, SPI_FFORM_S)
#define CMD_SPI_OPEN            _IOW (HI_ID_SPI, 0x5, hi_u32)
#define CMD_SPI_CLOSE           _IOW (HI_ID_SPI, 0x6, hi_u32)
#define CMD_SPI_SET_BLEND       _IOWR(HI_ID_SPI, 0x7, SPI_BLEND_S)
#define CMD_SPI_GET_BLEND       _IOWR(HI_ID_SPI, 0x8, SPI_BLEND_S)
#define CMD_SPI_SET_CLK         _IOW (HI_ID_SPI, 0x9, hi_u32)
#define CMD_SPI_READEX 			_IOWR(HI_ID_SPI, 0xa, SPI_DATAEX_S)
#define CMD_SPI_SET_LOOP        _IOW (HI_ID_SPI, 0xb, SPI_LOOP_S)
#define CMD_SPI_RW_LOOP         _IOWR(HI_ID_SPI, 0xc, SPI_DATAEX_S)

#endif

