/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#include "hal_vdp_ip_coef.h"
#include "hal_vdp_ip_para.h"

#define COEF_BIT_32 32
#define COEF_BIT_64 64
#define COEF_BIT_96 96
#define COEF_BIT_128 128

static hi_void vdp_coef_push128(vdp_coef_data *data, hi_u32 coef_data, hi_u32 bit_len)
{
    coef_data = coef_data & (0xFFFFFFFF >> (COEF_BIT_32 - bit_len));

    if (data->depth < COEF_BIT_32) {
        if ((data->depth + bit_len) <= COEF_BIT_32) {
            data->data0 = (coef_data << data->depth) | data->data0;
        } else {
            data->data0 = (coef_data << data->depth) | data->data0;
            data->data1 = coef_data >> (COEF_BIT_32 - data->depth % COEF_BIT_32);
        }
    } else if (data->depth >= COEF_BIT_32 && data->depth < COEF_BIT_64) {
        if ((data->depth + bit_len) <= COEF_BIT_64) {
            data->data1 = (coef_data << (data->depth % COEF_BIT_32)) | data->data1;
        } else {
            data->data1 = (coef_data << (data->depth % COEF_BIT_32)) | data->data1;
            data->data2 = coef_data >> (COEF_BIT_32 - data->depth % COEF_BIT_32);
        }
    } else if (data->depth >= COEF_BIT_64 && data->depth < COEF_BIT_96) {
        if ((data->depth + bit_len) <= COEF_BIT_96) {
            data->data2 = (coef_data << (data->depth % COEF_BIT_32)) | data->data2;
        } else {
            data->data2 = (coef_data << (data->depth % COEF_BIT_32)) | data->data2;
            data->data3 = coef_data >> (COEF_BIT_32 - data->depth % COEF_BIT_32);
        }
    } else if (data->depth >= COEF_BIT_96) {
        if ((data->depth + bit_len) <= COEF_BIT_128) {
            data->data3 = (coef_data << (data->depth % COEF_BIT_32)) | data->data3;
        }
    }

    data->depth = data->depth + bit_len;

    if (data->depth > COEF_BIT_128) {
        VDP_ERROR("ip coef error.\n");
    }
}

static hi_u32 vdp_coef_findmax(hi_u32 *array, hi_u32 num)
{
    hi_u32 ii;
    hi_u32 data = array[0];

    for (ii = 1; ii < num; ii++) {
        if (data < array[ii]) {
            data = array[ii];
        }
    }
    return data;
}

static hi_u32 vdp_coef_writeddr(vdp_coef_file_type type, struct file *fp, hi_u8 *addr, vdp_coef_data *data)
{
    hi_u32 i = 0;
    hi_u32 data_arr[4] = {data->data0, data->data1, data->data2, data->data3}; /* 4 is number */
    hi_u8 *tmp_addr = 0;
    hi_u32 tmp_data = 0;

    for (i = 0; i < 4; i++) { /* 4 is number */
        tmp_addr = addr + i * 4; /* 4 is number */
        tmp_data = data_arr[i];
        *(hi_u32 *)tmp_addr = tmp_data;
    }

    return 0;
}

static hi_u32 vdp_ip_coef_sendcoef_get_data(vdp_coef_data_type type, hi_u32 n,
                                            hi_u32 coef_cnt, hi_u8 **coef_array)
{
    hi_u32 tmp_data = 0;

    if (type == DRV_COEF_DATA_TYPE_S16) {
        tmp_data = ((hi_s16 **)coef_array)[n][coef_cnt];
    } else if (type == DRV_COEF_DATA_TYPE_U16) {
        tmp_data = ((hi_u16 **)coef_array)[n][coef_cnt];
    } else if (type == DRV_COEF_DATA_TYPE_U32) {
        tmp_data = ((hi_u32 **)coef_array)[n][coef_cnt];
    } else if (type == DRV_COEF_DATA_TYPE_S32) {
        tmp_data = ((hi_s32 **)coef_array)[n][coef_cnt];
    } else if (type == DRV_COEF_DATA_TYPE_S8) {
        tmp_data = ((hi_s8 **)coef_array)[n][coef_cnt];
    } else if (type == DRV_COEF_DATA_TYPE_U8) {
        tmp_data = ((hi_u8 **)coef_array)[n][coef_cnt];
    }

    return tmp_data;
}

hi_void vdp_ip_coef_sendcoef_cnt(hi_u32 coef_cnt, vdp_coef_send_cfg *cfg, vdp_coef_data *data)
{
    hi_u32 tmp_data;
    hi_u32 n;

    for (n = 0; n < cfg->lut_num; n++) {
        if (coef_cnt < cfg->lut_length[n]) {
            tmp_data = vdp_ip_coef_sendcoef_get_data(cfg->data_type, n, coef_cnt, (hi_u8**)(cfg->p_coef_array));
        } else {
            tmp_data = 0;
        }
        vdp_coef_push128(data, tmp_data, cfg->coef_bit_length[n]);
    }
}

hi_u8* vdp_ip_coef_sendcoef(vdp_coef_send_cfg *cfg)
{
    hi_u32 i, k, m;
    hi_u32 addr_offset = 0;
    hi_u8 *addr_base = cfg->coef_addr;
    hi_u8 *addr = addr_base;

    hi_u32 max_len;
    hi_u32 coef_cnt;

    hi_u32 cycle_num;
    hi_u32 total_bit_len;
    hi_u32 total_burst_num;

    vdp_coef_data data;

    addr_base = cfg->coef_addr;
    addr = addr_base;
    cycle_num = cfg->cycle_num;

    for (i = 0; i < cfg->lut_num; i++) {
        total_bit_len = total_bit_len + cfg->coef_bit_length[i];
    }

    /* send data */
    max_len = vdp_coef_findmax(cfg->lut_length, cfg->lut_num);
    total_burst_num = (max_len + cycle_num - 1) / cycle_num;

    for (k = 0; k < total_burst_num; k++) {
        memset((void *)&data, 0, sizeof(data));
        for (m = 0; m < cycle_num; m++) {
            coef_cnt = k * cycle_num + m;
            vdp_ip_coef_sendcoef_cnt(coef_cnt, cfg, &data);
        }
        addr = addr_base + addr_offset;
        addr_offset = addr_offset + 16; /* 16 is number */
        vdp_coef_writeddr(cfg->sti_type, cfg->fp_coef, addr, &data);
    }

    return (addr_base + addr_offset);
}

