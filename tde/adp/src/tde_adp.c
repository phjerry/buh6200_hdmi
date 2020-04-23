/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: adp manage
 * Author: sdk
 * Create: 2019-03-18
 */

#include "tde_adp.h"

static hi_u32 g_capability = ROP | ALPHABLEND | COLORIZE | CLUT | COLORKEY | CLIP | DEFLICKER | RESIZE | MIRROR |
                             CSCCOVERT | QUICKFILL | QUICKCOPY | PATTERFILL;

hi_void tde_get_capability(hi_u32 *capability)
{
    if (capability != HI_NULL) {
#ifdef CONFIG_TDE_MASKROP_SUPPORT
        g_capability = g_capability | MASKROP;
#endif
#ifdef CONFIG_TDE_MASKBLEND_SUPPORT
        g_capability = g_capability | MASKBLEND;
#endif
        *capability = g_capability;
    }
}
