#ifndef __DRV_PDM_IOCTL_H__
#define __DRV_PDM_IOCTL_H__

//#include "hi_drv_pdm.h"

#ifdef __cplusplus
extern "C" {
#endif
#define BUFFER_NAME_SIZE 16

#define CMD_PDM_GET_DATA                _IOWR(HI_ID_PDM, PDM_IOC_GET_RESERVE_MEM_DATA, pdm_get_data)
#define CMD_PDM_COMPAT_GET_DATA         _IOWR(HI_ID_PDM, PDM_IOC_GET_RESERVE_MEM_DATA, pdm_compat_get_data)
#define CMD_PDM_GET_RESERVE_MEM_INFO    _IOWR(HI_ID_PDM, PDM_IOC_GET_RESERVE_MEM_INFO, pdm_reserve_mem_info)
#define CMD_PDM_SEND_LICENSE_DATA       _IOWR(HI_ID_PDM, PDM_IOC_SEND_LICENSE_DATA, pdm_license_data)

typedef enum {
    PDM_IOC_GET_RESERVE_MEM_DATA = 0,
    PDM_IOC_GET_RESERVE_MEM_INFO,
    PDM_IOC_SEND_LICENSE_DATA,

    PDM_IOC_MAX
} pdm_ioc;

typedef struct {
    hi_char  buf_name[BUFFER_NAME_SIZE];
    hi_u8    *buf_addr;
    hi_u32   buf_lenth;
} pdm_get_data;

typedef struct {
    hi_char buf_name[BUFFER_NAME_SIZE];
    hi_u32  buf_addr;
    hi_u32  buf_lenth;
} pdm_compat_get_data;

typedef struct {
    hi_char buf_name[BUFFER_NAME_SIZE];
    hi_u32  phy_addr;
    hi_u32  size;
} pdm_reserve_mem_info;

typedef struct {
    hi_u64 mmz_handle;
    hi_u32 mmz_length;
} pdm_license_data;

#ifdef __cplusplus
}
#endif

#endif

