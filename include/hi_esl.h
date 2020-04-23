/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: Common data types of the system.
* Author: p00370051
* Create: 2016/11/10
* Notes:
* History: 2019-03-29 p00370051 CSEC
*/
#ifndef __HI_ESL_H__
#define __HI_ESL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*
 * Defintion of outstanding
 * CNcomment: 定义系统outstanding  CNend
 */
/* Constant Definition */
/* CNcomment: 常量定义  */
/* read outstanding config */
#define HI_OSTD_READ_VDP   (16)
#define HI_OSTD_READ_VPSS  (16)
#define HI_OSTD_READ_ME    (16) /* MEMC ME read outstanding */
#define HI_OSTD_READ_FI    (16) /* MEMC FI read outstanding */
#define HI_OSTD_READ_VDH   (32)
#define HI_OSTD_READ_VICAP (4)
#define HI_OSTD_READ_GPU   (8)
#define HI_OSTD_READ_AIAO  (8)
#define HI_OSTD_READ_VEDU  (16) /* venc read outstanding */
#define HI_OSTD_READ_TDE   (4)
#define HI_OSTD_READ_DEMUX (8) /* demux read outstanding */
#define HI_OSTD_READ_PGD   (8)
#define HI_OSTD_READ_JPGD  (4)
/* write outstanding config */
#define HI_OSTD_WRITE_VDP   (8)
#define HI_OSTD_WRITE_VPSS  (8)
#define HI_OSTD_WRITE_ME    (8) /* MEMC ME write outstanding */
#define HI_OSTD_WRITE_FI    (8) /* MEMC FI write outstanding */
#define HI_OSTD_WRITE_VDH   (8)
#define HI_OSTD_WRITE_VICAP (8)
#define HI_OSTD_WRITE_GPU   (8)
#define HI_OSTD_WRITE_AIAO  (8)
#define HI_OSTD_WRITE_VEDU  (8) /* venc write outstanding */
#define HI_OSTD_WRITE_TDE   (4)
#define HI_OSTD_WRITE_DEMUX (8) /* demux write outstanding */
#define HI_OSTD_WRITE_PGD   (8)
#define HI_OSTD_WRITE_JPGD  (4)

/* multi id cfg  start          */
#define HI_MULTI_ID_VDP     (1)
#define HI_MULTI_ID_VPSS    (1)
#define HI_MULTI_ID_MEMC_ME (1)
#define HI_MULTI_ID_MEMC_FI (1)
#define HI_MULTI_ID_VDH     (1)
#define HI_MULTI_ID_VICAP   (1)
#define HI_MULTI_ID_GPU     (0)
#define HI_MULTI_ID_AIAO    (0)
#define HI_MULTI_ID_VEDU    (0)
#define HI_MULTI_ID_TDE     (0)
#define HI_MULTI_ID_DEMUX   (0)
#define HI_MULTI_ID_PGD     (0)
#define HI_MULTI_ID_JPGD    (0)

/* compress rate cfg  start     */
#define HI_CMP_RATE_VDH      (300) /* VDH OUTPUT   cmp rate */
#define HI_CMP_RATE_VPSS     (300) /* VPSS OUTPUT  cmp rate */
#define HI_CMP_RATE_VDP_DS   (250) /* V0_F OUTPUT   cmp rate */
#define HI_CMP_RATE_VDP_DSY  (200) /* V0_F OUTPUT  y cmp rate */
#define HI_CMP_RATE_VDP_DSC  (200) /* V0_F OUTPUT  C cmp rate */
#define HI_CMP_RATE_VDP_DS2Y (140) /* V0_F OUTPUT  y cmp rate */
#define HI_CMP_RATE_VDP_DS2C (140) /* V0_F OUTPUT  c cmp rate */
#define HI_CMP_RATE_VDP_OD   (300) /* DHD OD cmp rate */

/** @} */ /** <!-- ==== Structure Definition end ==== */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_ESL_H__ */
