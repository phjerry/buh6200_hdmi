#ifndef __DRV_OSAL_TEE_H__
#define __DRV_OSAL_TEE_H__

#include "drv_osal_lib.h"

#define TEEC_CMD_GET_TRNG             0

hi_s32 drv_osal_tee_open_session(void);
hi_s32 drv_osal_tee_command(hi_u32 command, void *params, hi_u32 paramsize);
hi_s32 drv_osal_tee_close_session(void);

#endif

