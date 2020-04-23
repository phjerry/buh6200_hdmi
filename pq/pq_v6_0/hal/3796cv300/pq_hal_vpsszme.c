/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq hal zme
 * Author: pq
 * Create: 2016-01-1
 */

#include "pq_hal_vpsszme.h"
#include "pq_hal_comm.h"
#include "hal_pq_ip_coef.h"

hi_s16 g_vpsszme_hl_coef[17][8] = { 0 }; /* 17:phase size, 8: tap size */
hi_s16 g_vpsszme_hc_coef[17][8] = { 0 }; /* 17:phase size, 8: tap size */
hi_s16 g_vpsszme_vl_coef[17][8] = { 0 }; /* 17:phase size, 8: tap size */
hi_s16 g_vpsszme_vc_coef[17][8] = { 0 }; /* 17:phase size, 8: tap size */

/*  for ddr */
hi_s16 g_vpsszme_hl_coef_addr[17][8] = { 0 }; /* 17:phase size, 8: tap size */
hi_s16 g_vpsszme_hc_coef_addr[17][8] = { 0 }; /* 17:phase size, 8: tap size */
hi_s16 g_vpsszme_vl_coef_addr[17][8] = { 0 }; /* 17:phase size, 8: tap size */
hi_s16 g_vpsszme_vc_coef_addr[17][8] = { 0 }; /* 17:phase size, 8: tap size */

static hi_void pq_vpsszme_exchange_lut_scaler(hi_u32 phase, hi_u32 tap,
    hi_s16 old_lut[17][8], hi_s16 new_lut[17][8])  /* 17:phase size, 8: tap size */
{
    hi_u32 i, j;

    for (i = 0; i < phase; i++) {
        for (j = 0; j < tap; j++) {
            new_lut[i][j] = old_lut[i][j];
        }
    }
}

static hi_s32 pq_vpsszme_set_coef_h(hi_s16 *phl_coef, hi_s16 *phc_coef, hi_u8 *addr)
{
    pq_coef_send_cfg coef_send;

    void *p_coef_array[1] = { phl_coef };
    hi_u32 lut_length[1] = { 136 };
    hi_u32 coef_bit_length[1] = { 16 };

    PQ_CHECK_NULL_PTR_RE_FAIL(phl_coef);
    PQ_CHECK_NULL_PTR_RE_FAIL(phc_coef);
    PQ_CHECK_NULL_PTR_RE_FAIL(addr);

    coef_send.coef_addr = addr;
    coef_send.sti_type = PQ_STI_FILE_COEF_HZME;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 8; /* 8:cycle_num */
    coef_send.p_coef_array = p_coef_array;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = pq_send_coef2ddr(&coef_send);

    p_coef_array[0] = phc_coef;
    lut_length[0] = 136;     /* 136:lut_length */
    coef_bit_length[0] = 16; /* 16:coef_bit_length */

    coef_send.coef_addr = addr;
    coef_send.cycle_num = 8; /* 8:cycle_num */
    coef_send.p_coef_array = p_coef_array;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = pq_send_coef2ddr(&coef_send);

    return HI_SUCCESS;
}

static hi_s32 pq_vpsszme_set_coef_v(hi_s16 *pvl_coef_lut, hi_s16 *pvc_coef_lut, hi_u8 *addr)
{
    pq_coef_send_cfg coef_send;

    void *p_coef_array[1] = { pvl_coef_lut };
    hi_u32 lut_length[1] = { 136 };     /* 136:lut_length */
    hi_u32 coef_bit_length[1] = { 16 }; /* 16:bit len */

    PQ_CHECK_NULL_PTR_RE_FAIL(pvl_coef_lut);
    PQ_CHECK_NULL_PTR_RE_FAIL(pvc_coef_lut);
    PQ_CHECK_NULL_PTR_RE_FAIL(addr);

    coef_send.coef_addr = addr;
    coef_send.sti_type = PQ_STI_FILE_COEF_VZME;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 8; /* 8:cycle_num */
    coef_send.p_coef_array = p_coef_array;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = pq_send_coef2ddr(&coef_send);

    p_coef_array[0] = pvc_coef_lut;
    lut_length[0] = 136;     /* 136:lut_length */
    coef_bit_length[0] = 16; /* 16:bit len */

    coef_send.coef_addr = addr;
    coef_send.sti_type = PQ_STI_FILE_COEF_VZME;
    coef_send.lut_num = 1;
    coef_send.burst_num = 1;
    coef_send.cycle_num = 8; /* 8:cycle_num */
    coef_send.p_coef_array = p_coef_array;
    coef_send.lut_length = lut_length;
    coef_send.coef_bit_length = coef_bit_length;
    coef_send.data_type = DRV_COEF_DATA_TYPE_S16;

    addr = pq_send_coef2ddr(&coef_send);

    return HI_SUCCESS;
}

hi_void pq_hal_get_vpsszme_coef(hi_u32 offset, hi_u8 *coef_addr_h, hi_u8 *coef_addr_v)
{
    hi_s32 ret;
    hi_u32 phase = 17; /* 17:phase */
    hi_u32 tap = 8; /* 8: tap */

    PQ_CHECK_NULL_PTR_RE_NULL(coef_addr_h);
    PQ_CHECK_NULL_PTR_RE_NULL(coef_addr_v);

    /* H cofe */
    pq_vpsszme_exchange_lut_scaler(phase, tap, g_vpsszme_hl_coef, g_vpsszme_hl_coef_addr);
    pq_vpsszme_exchange_lut_scaler(phase, tap, g_vpsszme_hc_coef, g_vpsszme_hc_coef_addr);

    ret = pq_vpsszme_set_coef_h((hi_s16 *)g_vpsszme_hl_coef_addr, (hi_s16 *)g_vpsszme_hc_coef_addr, coef_addr_h);
    PQ_CHECK_SUCCESS_RETURN_NULL(ret);

    /* V cofe */
    pq_vpsszme_exchange_lut_scaler(phase, tap, g_vpsszme_vl_coef, g_vpsszme_vl_coef_addr);
    pq_vpsszme_exchange_lut_scaler(phase, tap, g_vpsszme_vc_coef, g_vpsszme_vc_coef_addr);

    ret = pq_vpsszme_set_coef_v((hi_s16 *)g_vpsszme_vl_coef_addr, (hi_s16 *)g_vpsszme_vc_coef_addr, coef_addr_v);
    PQ_CHECK_SUCCESS_RETURN_NULL(ret);
}

