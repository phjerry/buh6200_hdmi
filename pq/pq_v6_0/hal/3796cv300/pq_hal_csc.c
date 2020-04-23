/******************************************************************************
*
* copyright (C) 2014-2015 hisilicon technologies co., ltd.  all rights reserved.
*
* this program is confidential and proprietary to hisilicon  technologies co., ltd. (hisilicon),
*  and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of hisilicon.
*
*****************************************************************************

  file name     : pq_hal_csc.c
  version       : initial draft
  author        : sdk
  created       : 2013/10/15
  description   :

******************************************************************************/
#include "hi_drv_mem.h"
#include "pq_hal_csc.h"
#include "pq_hal_comm.h"

typedef enum {
    PQ_VDP_LAYER_WBC_HD0 = 0,
    PQ_VDP_LAYER_WBC_GP0 = 1,
    PQ_VDP_LAYER_WBC_G0 = 2,
    PQ_VDP_LAYER_WBC_VP0 = 3,
    PQ_VDP_LAYER_WBC_G4 = 3,
    PQ_VDP_LAYER_WBC_ME = 5,
    PQ_VDP_LAYER_WBC_FI = 6,
    PQ_VDP_LAYER_WBC_BMP = 7,
    PQ_VDP_LAYER_HC_BMP = 8,
    PQ_VDP_LAYER_WBC_TNR_REF = 9,
    PQ_VDP_LAYER_WBC_TNR_MAD = 10,
    PQ_VDP_LAYER_WBC_TNR_STT3 = 11,
    PQ_VDP_LAYER_WBC_DBM_STT1 = 12,
    PQ_VDP_LAYER_WBC_DBM_STT2 = 13,

    PQ_VDP_LAYER_WBC_MAX

} pq_vdp_layer_wbc;

hi_s32 pq_hal_get_wbc_point_sel(hi_void)
{
    return WBC_POINT_AFTER_CSC;
}


