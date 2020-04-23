/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
* Description:
*/

#ifndef __HI_DRV_SPI_H__
#define __HI_DRV_SPI_H__

#include "hi_type.h"
#include "hi_debug.h"
#include "hi_errno.h"

#define MAX_SPI_CHANNEL 2

#define HI_FATAL_SPI(fmt...)    HI_FATAL_PRINT(HI_ID_SPI, fmt)
#define HI_ERR_SPI(fmt...)      HI_ERR_PRINT(HI_ID_SPI, fmt)
#define HI_WARN_SPI(fmt...)     HI_WARN_PRINT(HI_ID_SPI, fmt)
#define HI_INFO_SPI(fmt...)     HI_INFO_PRINT(HI_ID_SPI, fmt)

hi_s32 hi_drv_spi_init(hi_void);
hi_s32 hi_drv_spi_deinit(hi_void);
hi_s32 hi_drv_spi_open(hi_u8 devId);
hi_s32 hi_drv_spi_close(hi_u8 devId);
hi_void hi_drv_spi_set_cs_gpio(hi_u8 devId);
hi_u8 hi_drv_spi_get_cs_config(hi_void);
hi_void hi_drv_spi_set_cs_level(hi_u8 devId,hi_u32 Level);
hi_void hi_drv_spi_set_cs_config(hi_u8 gpioCs);
hi_s32 hi_drv_spi_set_from(hi_u8 devId,hi_u8 framemode,hi_u8 spo,hi_u8 sph,hi_u8 datawidth);
hi_s32 hi_drv_spi_get_from(hi_u8 devId,hi_u8 *framemode,hi_u8 *spo,hi_u8 *sph,hi_u8 *datawidth);
hi_void hi_drv_spi_set_blend(hi_u8 devId,hi_bool bBigEnd);
hi_bool hi_drv_spi_get_blend(hi_u8 devId);
hi_s32 hi_drv_spi_write_isr(hi_u8 devId,hi_u8 *Send, hi_u32 SendCnt);
hi_s32 hi_drv_spi_write_query(hi_u8 devId,hi_u8 *Send, hi_u32 SendCnt);
hi_s32 hi_drv_spi_read_query(hi_u8 devId, hi_u8 *Read, hi_u32 ReadCnt);
hi_s32 hi_drv_spi_read_isr(hi_u8 devId, hi_u8 *Read, hi_u32 ReadCnt);
hi_s32 hi_drv_spi_read_ex(hi_u8 devId, hi_u8 *Send, hi_u32 SendCnt, hi_u8 *Read, hi_u32 ReadCnt);
hi_s32 hi_drv_spi_read_ex_isr(hi_u8 devId, hi_u8 *Send, hi_u32 SendCnt, hi_u8 *Read, hi_u32 ReadCnt);
hi_void HI_DRV_SPI_Enable(hi_u8 devId);
hi_void HI_DRV_SPI_DisEnable(hi_u8 devId);

hi_s32 hi_drv_spi_set_clk(hi_u8 devId, hi_u8 scr, hi_u8 cpsdvsr);


#endif

