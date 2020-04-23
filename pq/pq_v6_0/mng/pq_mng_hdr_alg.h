/******************************************************************************
*
* copyright (C) 2016 hisilicon technologies co., ltd.  all rights reserved.
*
* this program is confidential and proprietary to hisilicon  technologies co., ltd. (hisilicon),
*  and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of hisilicon.
*
*****************************************************************************

  file name    : pq_mng_hdr_alg.h
  version       : initial draft
  author        : sdk
  created      : 2016/03/19
  description  :

******************************************************************************/

#ifndef __PQ_MNG_HDR_ALG_H__
#define __PQ_MNG_HDR_ALG_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PQ_HDR_ALG_TM_LUT_SIZE 512

/* PQ source mode */
typedef enum {
    PQ_HDR_ALG_CFG_TYPE_TM = 0,

    PQ_HDR_ALG_CFG_TYPE_MAX
} pq_hdr_alg_cfg_type;

hi_s32 pq_mng_descramble_hdr_cfg(pq_hdr_alg_cfg_type type, hi_void *date);
hi_s32 pq_mng_scramble_hdr_cfg(pq_hdr_alg_cfg_type type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif


