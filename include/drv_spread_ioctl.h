/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: spread private Head File
 * Author: BSP
 * Create: 2019-11-29
 */

#ifndef __DRV_SPREAD_IOCTL_H__
#define __DRV_SPREAD_IOCTL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum ioctl_spread {
    IOC_SET_DDRSPREADEN = 0,
    IOC_SET_DDRSPREADRATIO,
    IOC_SET_DDRSPREADFREQ,

    IOC_SET_GMACCLKEN,
    IOC_SET_GMACSPREADEN,
    IOC_SET_GMACSPREADRATIO,
    IOC_SET_GMACSPREADFREQ,

    IOC_SET_EMMCCLKEN,
    IOC_SET_EMMCSPREADEN,
    IOC_SET_EMMCSPREADRATIO,
    IOC_SET_EMMCSPREADFREQ,

    IOC_SET_CICLKEN,
};

#define CMD_SPREAD_SET_DDR_ENABLE      _IOW(HI_ID_SPREAD, IOC_SET_DDRSPREADEN, hi_bool)
#define CMD_SPREAD_SET_DDR_RATIO       _IOW(HI_ID_SPREAD, IOC_SET_DDRSPREADRATIO, hi_u32)
#define CMD_SPREAD_SET_DDR_FREQ        _IOW(HI_ID_SPREAD, IOC_SET_DDRSPREADFREQ, hi_u32)

#define CMD_SPREAD_SET_GMAC_CLKEN      _IOW(HI_ID_SPREAD, IOC_SET_GMACCLKEN, hi_bool)
#define CMD_SPREAD_SET_GMAC_ENABLE     _IOW(HI_ID_SPREAD, IOC_SET_GMACSPREADEN, hi_bool)
#define CMD_SPREAD_SET_GMAC_RATIO      _IOW(HI_ID_SPREAD, IOC_SET_GMACSPREADRATIO, hi_u32)
#define CMD_SPREAD_SET_GMAC_FREQ       _IOW(HI_ID_SPREAD, IOC_SET_GMACSPREADFREQ, hi_u32)

#define CMD_SPREAD_SET_EMMC_CLKEN      _IOW(HI_ID_SPREAD, IOC_SET_EMMCCLKEN, hi_bool)
#define CMD_SPREAD_SET_EMMC_ENABLE     _IOW(HI_ID_SPREAD, IOC_SET_EMMCSPREADEN, hi_bool)
#define CMD_SPREAD_SET_EMMC_RATIO      _IOW(HI_ID_SPREAD, IOC_SET_EMMCSPREADRATIO, hi_u32)
#define CMD_SPREAD_SET_EMMC_FREQ       _IOW(HI_ID_SPREAD, IOC_SET_EMMCSPREADFREQ, hi_u32)

#define CMD_SPREAD_SET_CI_CLKEN        _IOW(HI_ID_SPREAD, IOC_SET_CICLKEN, hi_u32)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DRV_SPREAD_IOCTL_H__ */
