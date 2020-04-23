/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */
/* common headers */
#include "strm_mng_intf.h"

/* local headers */
#include "strm_mng_listbuf.h"
#include "strm_mng_recyclebuf.h"
#include "linux/hisilicon/securec.h"

#define STRM_MODE_NUM 3
#define STRM_MODE_LIST 0
#define STRM_MODE_RECYCLE 1

venc_strm_fun_ptr g_venc_strm_fun_ptr[STRM_MODE_NUM];

static VOID intf_init_list(VOID)
{
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_init      = list_buf_init;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_deinit    = list_buf_deinit;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_creat    = list_buf_create;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_destroy   = list_buf_destroy;

    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_get_write_buffer = list_buf_get_write_buf;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_put_write_buffer = list_buf_put_write_buf;

    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_put_buffer_for_recode = list_buf_put_buf_for_recode;

    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_acq_read_buffer    = list_buf_acquire_bit_stream;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_rls_read_buffer    = list_buf_release_bit_stream;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_get_acq_stream_info = list_buf_get_acquire_stream_info;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_get_rls_buffer_info = list_buf_get_release_stream_info;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_get_rls_buffer_info_intal = list_buf_get_release_stream_info_intar;

    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_check_buffer_size  = list_buf_check_buffer_size;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_cal_slc_size       = list_buf_cal_slc_size;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_get_strm_state     = list_buf_get_strm_state;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_get_written_len    = list_buf_get_written_len;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_get_bit_buf_size    = list_buf_get_bit_buf_size;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_alloc_stream_buf   = list_buf_alloc_stream_buf;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_alloc_para_set_buf_addr = list_buf_alloc_para_set_buf_addr;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_rls_para_set_buf_addr = list_buf_rls_para_set_buf_addr;

    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_reset_buffer  = list_buf_reset_buffer;

#ifdef VENC_SUPPORT_JPGE
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_jpeg_write_header    = list_buf_write_header;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_put_jpeg_write_buffer = list_buf_put_jpeg_write_buffer;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_put_jpeg_remove_header   = list_buf_jpeg_remove_header;
    g_venc_strm_fun_ptr[STRM_MODE_LIST].fun_strm_get_jpeg_stream      = list_buf_get_jpeg_stream;
#endif
}

static VOID intf_init_loop(VOID)
{
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_init      = recycle_buffer_init;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_deinit    = recycle_buf_deinit;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_creat    = recycle_buf_create;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_destroy   = recycle_buf_destroy;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_get_write_buffer    = recycle_buf_get_write_buf;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_put_write_buffer    = recycle_buf_put_write_buf;

    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_put_buffer_for_recode = recycle_buf_put_buf_for_recode;

    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_acq_read_buffer    = recycle_buf_acq_read_buffer;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_rls_read_buffer    = recycle_buf_rls_read_buffer;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_get_acq_stream_info = recycle_buf_get_acq_strm_info;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_get_rls_buffer_info = recycle_buf_get_rls_strm_info;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_get_rls_buffer_info_intal = recycle_buf_get_rls_strm_info_intar;

    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_check_buffer_size  = recycle_buf_check_buf_size;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_cal_slc_size       = recycle_buf_cal_slc_size;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_get_strm_state     = recycle_buf_get_stream_state;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_get_written_len    = recycle_buf_get_written_length;

    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_reset_buffer       = recycle_buf_reset_buffer;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_get_bit_buf_size     = recycle_buf_get_bit_buf_size;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_alloc_stream_buf    = recycle_buf_alloc_stream_buf;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_alloc_para_set_buf_addr = recycle_buf_alloc_para_set_buf_addr;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_rls_para_set_buf_addr   = recycle_buf_rls_para_set_buf_addr;

#ifdef VENC_SUPPORT_JPGE
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_jpeg_write_header    = recycle_buf_write_header;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_put_jpeg_write_buffer = recycle_buf_put_jpge_write_buffer;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_put_jpeg_remove_header   = recycle_buf_jpge_remove_header;
    g_venc_strm_fun_ptr[STRM_MODE_RECYCLE].fun_strm_get_jpeg_stream      = recycle_buf_get_jpge_stream;

#endif
}

static VOID intf_init_other(VOID)
{

}

static hi_u32 find_stream_buffer_func(hi_u32 handle, hi_u32 *strm_mode, hi_u32 *private_id)
{
    *strm_mode  = handle >> 16; /* 16 is left move size */
    *private_id = handle & 0xffff;

    if (*strm_mode >= STRM_MODE_NUM) {
        HI_ERR_VENC("the strm mode number %d exceeds the max number %d\n", *strm_mode, STRM_MODE_NUM);

        return HI_ERR_VENC_INVALID_PARA;
    }

    if (*private_id >= HI_VENC_MAX_CHN) {
        HI_ERR_VENC("the channel number %d exceeds the max number %d\n", *private_id, HI_VENC_MAX_CHN);

        return HI_ERR_VENC_INVALID_PARA;
    }

    return HI_SUCCESS;
}

static hi_void init_stream_buffer(hi_u32 strm_mode)
{
    g_venc_strm_fun_ptr[strm_mode].fun_strm_init();
}

static hi_void de_init_stream_buffer(hi_u32 strm_mode)
{
    g_venc_strm_fun_ptr[strm_mode].fun_strm_deinit();
}

hi_s32 strm_init(hi_void)
{
    hi_u32 i = 0;
    hi_u32 count = 0;

    intf_init_list();
    intf_init_loop();
    intf_init_other();

    for (i = 0; i < STRM_MODE_NUM; i++) {
        if (g_venc_strm_fun_ptr[i].fun_strm_init != HI_NULL) {
            init_stream_buffer(i);
        } else {
            count++;
        }
    }

    if (count == STRM_MODE_NUM) {
        HI_ERR_VENC("STRM init is failure! there is no right strm mode!");

        return HI_ERR_VENC_NULL_PTR;
    }

    return HI_SUCCESS;
}

hi_s32 strm_deinit(hi_void)
{
    hi_u32 i = 0;
    hi_u32 count = 0;

    for (i = 0; i < STRM_MODE_NUM; i++) {
        if (g_venc_strm_fun_ptr[i].fun_strm_init != HI_NULL) {
            de_init_stream_buffer(i);
        } else {
            count++;
        }
    }

    if (count == STRM_MODE_NUM) {
        HI_ERR_VENC("STRM de_init is failure! there is no right strm mode!");

        return HI_ERR_VENC_NULL_PTR;
    }

    return HI_SUCCESS;
}

hi_s32 strm_get_match_mode(hi_u32 protocol, hi_handle* ph_strm_mng, hi_bool is_chip_id_v500_r001)
{
    hi_u32 strm_mode = 0;

    if (ph_strm_mng == HI_NULL) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (protocol == VEDU_JPGE) {
#ifdef VENC_SUPPORT_JPGE
        strm_mode = 1;
#endif
    } else {
        if (is_chip_id_v500_r001 == HI_TRUE) {
            strm_mode = 0;
        } else {
            switch (HI_VENC_STRM_MODE) {
                case STRM_MODE_LIST:
                    strm_mode = 0;
                    break;

                case STRM_MODE_RECYCLE:
                    strm_mode = 1;
                    break;

                default:
                    strm_mode = 0xffff;
                    break;
            }
        }
    }

    *ph_strm_mng = strm_mode << 16; /* 16 is right move size */

    if (*ph_strm_mng == 0xffff0000) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 get_config_info(hi_handle strm_mng, strm_mng_stream_info *stream_info, strm_mng_inst_config *config)
{
    /* create inst */
    if (strm_mng == STRM_MODE_LIST) {
        config->size = stream_info->strm_buf_size;
        config->phy_addr = stream_info->stream_mmz_buf.start_phy_addr  + VEDU_MMZ_ALIGN;
        config->knl_vir_addr = stream_info->stream_mmz_buf.start_vir_addr + VEDU_MMZ_ALIGN;
        config->para_set_phy_addr = stream_info->slice_buf.start_phy_addr;
        config->para_set_knl_vir_addr = stream_info->slice_buf.start_vir_addr;
        config->rsv_byte        = 64; /* 64 is rsv byte */
    } else if (strm_mng == STRM_MODE_RECYCLE) {
        config->size = stream_info->strm_buf_size;
        config->phy_addr = stream_info->stream_mmz_buf.start_phy_addr  + VEDU_MMZ_ALIGN;
        config->knl_vir_addr     = stream_info->stream_mmz_buf.start_vir_addr + VEDU_MMZ_ALIGN;
        config->rsv_byte        = 64; /* 64 is rsv byte */
        config->strm_ext_len     = stream_info->strm_buf_ext_len;
    }

    return HI_SUCCESS;
}

hi_s32 strm_create(hi_handle handle, strm_mng_stream_info *stream_info)
{
    hi_s32 ret;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;
    strm_mng_inst_config config;

    if (stream_info == HI_NULL) {
        return HI_ERR_VENC_NULL_PTR;
    }

    memset_s(&config, sizeof(strm_mng_inst_config), 0, sizeof(strm_mng_inst_config));

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    ret = get_config_info(strm_mode, stream_info, &config);
    if (ret != HI_SUCCESS) {
    HI_ERR_VENC("strm_create fail\n");
    return ret ;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_creat == HI_NULL) {
        HI_ERR_VENC("get stream_create function ptr is failure!\n");

        ret = HI_ERR_VENC_NULL_PTR;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_creat(private_id, &config);
    }

    return ret;
}

hi_s32 strm_destroy(hi_u32 handle)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_destroy == HI_NULL) {
        HI_ERR_VENC("get stream_destroy function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_destroy(private_id);
    }

    return ret;
}

hi_s32 strm_get_write_buffer(hi_u32 handle, strm_mng_input_info *input_info, strm_mng_stream_info *stream_info)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if ((input_info == HI_NULL) || (stream_info == HI_NULL)) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_get_write_buffer == HI_NULL) {
        HI_ERR_VENC("get stream_get_write_buffer_frame function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_get_write_buffer(private_id, input_info, stream_info);
    }

    return ret;
}

hi_s32 strm_put_buffer_for_recode(hi_u32 handle, strm_mng_input_info *input_info, strm_mng_stream_info *stream_info)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if ((input_info == HI_NULL) || (stream_info == HI_NULL)) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_put_buffer_for_recode == NULL) {
        HI_ERR_VENC("get stream_put_buffer_for_recode function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_put_buffer_for_recode(private_id, input_info, stream_info);
    }

    return ret;
}

hi_s32 strm_put_write_buffer(hi_u32 handle, strm_mng_input_info *input_info, strm_mng_stream_info *stream_info)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if ((input_info == HI_NULL) || (stream_info == HI_NULL)) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_put_write_buffer == NULL) {
        HI_ERR_VENC("get stream_put_frame_write_buffer function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_put_write_buffer(private_id, input_info, stream_info);
    }

    return ret;
}

hi_s32 strm_acq_read_buffer(hi_handle handle, strm_mng_stream_info *stream_info, vedu_efl_nalu *vedu_packet)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if ((vedu_packet == HI_NULL) || (stream_info == HI_NULL)) {
        return HI_ERR_VENC_NULL_PTR;
    }

    memset_s(vedu_packet, sizeof(vedu_efl_nalu), 0, sizeof(vedu_efl_nalu));

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_acq_read_buffer == NULL) {
        HI_ERR_VENC("get stream_acq_read_buffer function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_acq_read_buffer(private_id, stream_info, vedu_packet);
    }

    return ret;
}

hi_s32 strm_rls_read_buffer(hi_handle handle, strm_mng_stream_info *stream_info, vedu_efl_nalu *vedu_packet)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if ((vedu_packet == HI_NULL) || (stream_info == HI_NULL)) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_rls_read_buffer == NULL) {
        HI_ERR_VENC("get stream_rls_read_buffer function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_rls_read_buffer(private_id, stream_info, vedu_packet);
    }

    return ret;
}

hi_s32 strm_get_acquire_stream_info(handle_protocol *h_protocol, hi_u8* start_vir_addr,
    vedu_efl_nalu *vedu_packet, hi_venc_stream* stream, venc_ioctl_buf_offset *buf_offset)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if ((vedu_packet == HI_NULL) || (stream == HI_NULL) || (start_vir_addr == HI_NULL) || (buf_offset == HI_NULL)) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(h_protocol->handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    h_protocol->handle = private_id;

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_get_acq_stream_info == NULL) {
        HI_ERR_VENC("get stream_get_acquire_stream_info function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_get_acq_stream_info(h_protocol, start_vir_addr,
            vedu_packet, stream, buf_offset);
    }

    return ret;
}

hi_s32 strm_get_release_stream_info(hi_handle handle, hi_u32 protocol, strm_mng_stream_info *stream_info,
    venc_ioctl_acquire_stream* acq_strm, vedu_efl_nalu* vedu_packet)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if ((vedu_packet == HI_NULL) || (stream_info == HI_NULL) || (acq_strm == HI_NULL)) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_get_rls_buffer_info == NULL) {
        HI_ERR_VENC("get stream_get_rlease_stream_info function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_get_rls_buffer_info(private_id, protocol, stream_info,
            acq_strm, vedu_packet);
    }

    return ret;
}

hi_s32 strm_get_release_stream_info_intar(hi_handle handle, hi_venc_stream *stream, vedu_efl_nalu* vedu_packet)
{
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if ((vedu_packet == HI_NULL) || (stream == HI_NULL)) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_get_rls_buffer_info_intal == NULL) {
        HI_ERR_VENC("get stream_get_rlease_stream_info_intar function ptr is failure!\n");
    } else {
        g_venc_strm_fun_ptr[strm_mode].fun_strm_get_rls_buffer_info_intal(private_id, stream, vedu_packet);
    }

    return HI_SUCCESS;
}


hi_s32 strm_check_buffer_size(hi_handle handle, hi_u32 strm_buf_size, hi_u32 cur_strm_buf_size)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_check_buffer_size == NULL) {
        HI_ERR_VENC("get stream_check_buffer_size function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_check_buffer_size(private_id, strm_buf_size, cur_strm_buf_size);
    }

    return ret;
}

hi_u32 strm_cal_slc_size(hi_handle handle, hi_u32 protocol, hi_u32 pic_height, hi_u32 unf_split_size)
{
    hi_u32 slc_size = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        slc_size = 0xffffffff;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_cal_slc_size == NULL) {
        HI_ERR_VENC("get stream_cal_slc_size function ptr is failure!\n");

        slc_size = 0xffffffff;
    } else {
        slc_size = g_venc_strm_fun_ptr[strm_mode].fun_strm_cal_slc_size(private_id, protocol,
            pic_height, unf_split_size);
    }

    return slc_size;
}

hi_s32 strm_get_strm_state(hi_handle handle, strm_mng_state* strm_stat_info)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if (strm_stat_info == HI_NULL) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_get_strm_state == NULL) {
        HI_ERR_VENC("get stream_get_strm_node_state function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_get_strm_state(private_id, strm_stat_info);
    }

    return ret;
}

hi_u32 strm_get_written_length(hi_handle handle)
{
    hi_u32 write_len = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        write_len = 0;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_get_written_len == NULL) {
        HI_ERR_VENC("get stream_get_written_len function ptr is failure!\n");
        write_len = 0;
    } else {
        write_len = g_venc_strm_fun_ptr[strm_mode].fun_strm_get_written_len(private_id);
    }

    return write_len;
}

hi_s32 strm_reset_buffer(HANDLE handle, hi_u32 protocol)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_reset_buffer == NULL) {
        HI_ERR_VENC("get stream_reset_buffer function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_reset_buffer(private_id, protocol);
    }

    return ret;
}

hi_u32 strm_get_bit_buf_size(HANDLE handle, hi_u32 *strm_buf_size, hi_u32 slc_split_en,
    hi_u32 luma_size, hi_u32 *ext_len)
{
    hi_s32 bit_buf_size = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if ((strm_buf_size == HI_NULL) || (ext_len == HI_NULL)) {
        return 0;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return 0;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_get_bit_buf_size == NULL) {
        HI_ERR_VENC("get stream_get_bit_buf_size function ptr is failure!\n");
        bit_buf_size = 0;
    } else {
        *strm_buf_size = D_VENC_ALIGN_UP(*strm_buf_size, VEDU_MMZ_ALIGN);

        bit_buf_size = g_venc_strm_fun_ptr[strm_mode].fun_strm_get_bit_buf_size(private_id,
            *strm_buf_size, slc_split_en, luma_size, ext_len);
    }

    return bit_buf_size;
}

hi_s32 strm_alloc_stream_buf(HANDLE handle, venc_buffer *stream_mmz_buf,
    strm_mng_alloc_info *alloc_info, hi_bool enable_rcn_ref_share_buf)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if (stream_mmz_buf == HI_NULL || alloc_info == HI_NULL) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_alloc_stream_buf == NULL) {
        HI_ERR_VENC("get stream_alloc_stream_buf function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_alloc_stream_buf(private_id, stream_mmz_buf,
            alloc_info, enable_rcn_ref_share_buf);
    }

    return ret;
}

hi_s32 strm_rls_stream_buf(HANDLE handle, venc_buffer *stream_mmz_buf, hi_u32 protocol)
{
    if (stream_mmz_buf == HI_NULL) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (protocol == VEDU_JPGE) {
#ifdef VENC_SUPPORT_JPGE
        hi_drv_venc_mmz_unmap_and_release(stream_mmz_buf);
#endif
    } else {
        hi_drv_venc_unmap_and_release(stream_mmz_buf);
    }

    return HI_SUCCESS;
}

hi_s32 strm_alloc_write_read_ptr_buf(HANDLE handle, venc_buffer* wp_rp_mmz_buf)
{
    hi_s32 ret;
    hi_u32 bit_buf_size = 4096;

    if (wp_rp_mmz_buf == HI_NULL) {
        return HI_ERR_VENC_NULL_PTR;
    }

    ret = hi_drv_venc_mmz_alloc_and_map("venc_wp_rp_ptr_buf", 0,
        bit_buf_size, 64, wp_rp_mmz_buf); /* 64 is buf size */

    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("wp_rp_ptr_buf hi_drv_venc_alloc_and_map failed\n");
    }

    return ret;
}

hi_s32 strm_rls_write_read_ptr_buf(HANDLE handle, venc_buffer* wp_rp_mmz_buf)
{
    if (wp_rp_mmz_buf == HI_NULL) {
        return HI_ERR_VENC_NULL_PTR;
    }

    hi_drv_venc_mmz_unmap_and_release(wp_rp_mmz_buf);

    return HI_SUCCESS;
}

hi_s32 strm_alloc_para_set_buf_addr(HANDLE handle, venc_buffer *para_set_buf, hi_u32 *para_set_addr)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if ((para_set_buf == HI_NULL) || (para_set_addr == HI_NULL)) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_alloc_para_set_buf_addr == NULL) {
        HI_ERR_VENC("get stream_alloc_para_set_buf_addr function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_alloc_para_set_buf_addr(private_id, para_set_buf, para_set_addr);
    }

    return ret;
}

hi_s32 strm_rls_para_set_buf_addr(HANDLE handle, venc_buffer *para_set_buf)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if (para_set_buf == HI_NULL) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if ((strm_mode == 0) && (g_venc_strm_fun_ptr[strm_mode].fun_strm_rls_para_set_buf_addr == NULL)) {
        HI_ERR_VENC("get stream_rls_para_set_buf_addr function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_rls_para_set_buf_addr(private_id, para_set_buf);
    }

    return ret;
}

#ifdef VENC_SUPPORT_JPGE
hi_s32 strm_jpge_write_header(HANDLE handle, strm_mng_input_info* input_info, strm_mng_stream_info *stream_info)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if ((input_info == HI_NULL) || (stream_info == HI_NULL)) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_jpeg_write_header == NULL) {
        HI_ERR_VENC("get stream_jpge_write_header function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_jpeg_write_header(private_id, input_info, stream_info);
    }

    return ret;
}

hi_s32 strm_put_jpge_write_buffer(hi_handle handle, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if ((input_info == HI_NULL) || (stream_info == HI_NULL)) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_put_jpeg_write_buffer == NULL) {
        HI_ERR_VENC("get stream_put_jpge_write_buffer function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_put_jpeg_write_buffer(private_id, input_info, stream_info);
    }

    return ret;
}

hi_s32 strm_jpge_remove_header(hi_handle handle, strm_mng_input_info *input_info, strm_mng_stream_info *stream_info)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if ((input_info == HI_NULL) || (stream_info == HI_NULL)) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_put_jpeg_remove_header == NULL) {
        HI_ERR_VENC("get stream_jpge_remove_header function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_put_jpeg_remove_header(private_id, input_info, stream_info);
    }

    return ret;
}

hi_s32 strm_get_jpge_stream(hi_handle handle, strm_mng_stream_info *stream_info,
    vedu_efl_nalu *vedu_packet, hi_venc_stream *stream, venc_ioctl_buf_offset *buf_offset)
{
    hi_s32 ret = 0;
    hi_u32 strm_mode  = 0;
    hi_u32 private_id = 0;

    if ((vedu_packet == HI_NULL) || (stream_info == HI_NULL) || (stream == HI_NULL) || (buf_offset == HI_NULL)) {
        return HI_ERR_VENC_NULL_PTR;
    }

    if (find_stream_buffer_func(handle, &strm_mode, &private_id) != HI_SUCCESS) {
        return HI_ERR_VENC_INVALID_PARA;
    }

    if (g_venc_strm_fun_ptr[strm_mode].fun_strm_get_jpeg_stream == NULL) {
        HI_ERR_VENC("get stream_get_jpge_stream function ptr is failure!\n");

        ret = HI_FAILURE;
    } else {
        ret = g_venc_strm_fun_ptr[strm_mode].fun_strm_get_jpeg_stream(private_id, stream_info,
            vedu_packet, stream, buf_offset);
    }

    return ret;
}

#endif

#ifdef VENC_SUPPORT_H265
hi_venc_h265e_nalu_type trans_nalu_type_num_for_h265(hi_u32 nalu_type)
{
    hi_venc_h265e_nalu_type  hevce_type;

    switch (nalu_type) {
        case H265_NALU_TYPE_P:
            hevce_type = HI_VENC_H265E_NALU_PSLICE;
            break;

        case H265_NALU_TYPE_IDR:
            hevce_type = HI_VENC_H265E_NALU_ISLICE;
            break;

        case H265_NALU_TYPE_VPS:
            hevce_type = HI_VENC_H265E_NALU_VPS;
            break;

        case H265_NALU_TYPE_SPS:
            hevce_type = HI_VENC_H265E_NALU_SPS;
            break;

        case H265_NALU_TYPE_PPS:
            hevce_type = HI_VENC_H265E_NALU_PPS;
            break;

        case H265_NALU_TYPE_SEI:
            hevce_type = HI_VENC_H265E_NALU_SEI;
            break;

        default:
            hevce_type = HI_VENC_H265E_NALU_MAX;
            break;
    }

    return hevce_type;
}

hi_u32 inverse_trans_nalu_type_num_for_h265(hi_venc_h265e_nalu_type hevce_type)
{
    hi_u32  nalu_type = 0;

    switch (hevce_type) {
        case HI_VENC_H265E_NALU_PSLICE:
            nalu_type = H265_NALU_TYPE_P;
            break;

        case HI_VENC_H265E_NALU_ISLICE:
            nalu_type = H265_NALU_TYPE_IDR;
            break;

        case HI_VENC_H265E_NALU_VPS:
            nalu_type = H265_NALU_TYPE_VPS;
            break;

        case HI_VENC_H265E_NALU_SPS:
            nalu_type = H265_NALU_TYPE_SPS;
            break;

        case HI_VENC_H265E_NALU_PPS:
            nalu_type = H265_NALU_TYPE_PPS;
            break;

        case HI_VENC_H265E_NALU_SEI:
            nalu_type = H265_NALU_TYPE_SEI;
            break;

        default:
            nalu_type = 0;
            break;
    }

    return nalu_type;
}
#endif

