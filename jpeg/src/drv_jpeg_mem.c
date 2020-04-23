/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: jpeg memory manage
 */

#include "drv_jpeg_mem.h"
#include "hi_debug.h"
#include "hi_gfx_comm_k.h"
#include "drv_jpeg_struct.h"

static hi_s32 jpeg_hdec_alloc_input_buf_mem(jpeg_hdec_mem_info *mem_info);
static hi_s32 jpeg_hdec_config_input_stream_buf_mem(jpeg_hdec_mem_info *mem_info, drv_gfx_mem_info *output_info);
static hi_s32 jpeg_hdec_alloc_yuv_mem(jpeg_hdec_mem_info *mem_info);
static hi_s32 jpeg_hdec_alloc_xrgb_mem(jpeg_hdec_mem_info *mem_info);

static hi_void jpeg_hdec_free_input_buf_mem(jpeg_hdec_mem_info *mem_info);
static hi_void jpeg_hdec_free_yuv_mem(jpeg_hdec_mem_info *mem_info);
static hi_void jpeg_hdec_free_xrgb_mem(jpeg_hdec_mem_info *mem_info);

hi_s32 drv_jpeg_hdec_alloc_mem(hi_void *arg)
{
    hi_s32 ret;
    jpeg_hdec_mem_info *mem_info = (jpeg_hdec_mem_info *)arg;
    if (mem_info == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d input para is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    ret = jpeg_hdec_alloc_input_buf_mem(mem_info);
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-jpeg][err] : %s %d call jpeg_hdec_alloc_input_buf_mem failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    ret = jpeg_hdec_alloc_yuv_mem(mem_info);
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-jpeg][err] : %s %d call jpeg_hdec_alloc_yuv_mem failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    ret = jpeg_hdec_alloc_xrgb_mem(mem_info);
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-jpeg][err] : %s %d call jpeg_hdec_alloc_xrgb_mem failure\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    return ret;
}

hi_s32 drv_jpeg_hdec_free_mem(hi_void *arg)
{
    jpeg_hdec_mem_info *mem_info = (jpeg_hdec_mem_info *)arg;
    if (mem_info == NULL) {
        HI_PRINT("[module-jpeg][err] : %s %d input para is null\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    jpeg_hdec_free_input_buf_mem(mem_info);
    jpeg_hdec_free_yuv_mem(mem_info);
    jpeg_hdec_free_xrgb_mem(mem_info);
    return HI_SUCCESS;
}

static hi_s32 jpeg_hdec_alloc_input_buf_mem(jpeg_hdec_mem_info *mem_info)
{
    hi_s32 ret;
    hi_u32 mem_size;
    drv_gfx_mem_info input_mem_info = {0};
    hi_u32 buf_size = mem_info->input_mem_info.stream_mem.buf_size;
    mem_size = (buf_size + JPEG_INPUT_DATA_BUFFER_RESERVE_SIZE) * JPEG_INPUT_DATA_BUFFER_NUM;
    if (mem_size <= 0) {
        HI_PRINT("[module-jpeg][err] : %s %d alloc input mem size is invalid\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    ret = drv_gfx_mem_alloc(&input_mem_info, "jpeg_stream_mem", HI_TRUE, HI_FALSE, mem_size);
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-jpeg][err] : %s %d call drv_gfx_mem_alloc failure\n", __FUNCTION__, __LINE__);
        return ret;
    }
    return jpeg_hdec_config_input_stream_buf_mem(mem_info, &input_mem_info);
}

static hi_s32 jpeg_hdec_config_input_stream_buf_mem(jpeg_hdec_mem_info *mem_info, drv_gfx_mem_info *input_mem_info)
{
    hi_u32 i;
    hi_u64 phy_buf = input_mem_info->phy_addr;
    hi_u32 buf_size = mem_info->input_mem_info.stream_mem.buf_size;
    hi_u32 total_size = buf_size + JPEG_INPUT_DATA_BUFFER_RESERVE_SIZE;
    for (i = 0; i < JPEG_INPUT_DATA_BUFFER_NUM; i++) {
        mem_info->buf_info.save_buf[i].phy_buf = phy_buf + i * total_size;
        mem_info->buf_info.save_buf[i].buf_size = total_size;
    }
    mem_info->output_mem_info.stream_mem.fd = osal_mem_create_fd(input_mem_info->dmabuf, OSAL_O_CLOEXEC);
    mem_info->output_mem_info.stream_mem.buf_size = total_size;
    return HI_SUCCESS;
}

static hi_s32 jpeg_hdec_alloc_yuv_mem(jpeg_hdec_mem_info *mem_info)
{
    hi_s32 ret;
    hi_u32 mem_size;
    drv_gfx_mem_info yuv_mem_info = {0};
    mem_size = mem_info->input_mem_info.yuv_mem.y_size + mem_info->input_mem_info.yuv_mem.uv_size;
    if (mem_size == 0) {
        HI_PRINT("[module-jpeg][err] : %s %d alloc yuv mem size is invalid\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    ret = drv_gfx_mem_alloc(&yuv_mem_info, "jpeg_yuv_mem", HI_TRUE, HI_FALSE, mem_size);
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-jpeg][err] : %s %d call drv_gfx_mem_alloc failure\n", __FUNCTION__, __LINE__);
        return ret;
    }
    mem_info->yuv_surface.phy_buf[0] = yuv_mem_info.phy_addr;
    mem_info->yuv_surface.phy_buf[1] = mem_info->yuv_surface.phy_buf[0] + mem_info->input_mem_info.yuv_mem.y_size;

    mem_info->output_mem_info.yuv_mem.fd = osal_mem_create_fd(yuv_mem_info.dmabuf, OSAL_O_CLOEXEC);
    mem_info->output_mem_info.yuv_mem.buf_size = mem_size;
    return ret;
}

static hi_s32 jpeg_hdec_alloc_xrgb_mem(jpeg_hdec_mem_info *mem_info)
{
    hi_s32 ret;
    hi_u32 mem_size;
    drv_gfx_mem_info xrgb_mem_info = {0};
    mem_size = mem_info->input_mem_info.xrgb_mem.output_stride * mem_info->input_mem_info.xrgb_mem.output_height;
    if (mem_size == 0) {
        HI_PRINT("[module-jpeg][err] : %s %d alloc xrgb mem size is invalid\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    ret = drv_gfx_mem_alloc(&xrgb_mem_info, "jpeg_xrgb_mem", HI_TRUE, HI_FALSE, mem_size);
    if (ret != HI_SUCCESS) {
        HI_PRINT("[module-jpeg][err] : %s %d call drv_gfx_mem_alloc failure\n", __FUNCTION__, __LINE__);
        return ret;
    }
    mem_info->xrgb_surface.phy_buf[0] = xrgb_mem_info.phy_addr;

    mem_info->output_mem_info.xrgb_mem.fd = osal_mem_create_fd(xrgb_mem_info.dmabuf, OSAL_O_CLOEXEC);
    mem_info->output_mem_info.xrgb_mem.buf_size = mem_size;
    return ret;
}

static hi_void jpeg_hdec_free_input_buf_mem(jpeg_hdec_mem_info *mem_info)
{
    hi_u32 fd = mem_info->output_mem_info.stream_mem.fd;
    hi_void *dmabuf = osal_mem_handle_get(fd, ConvertID(HIGFX_JPGDEC_ID));
    if (dmabuf == NULL) {
        return;
    }
    osal_mem_ref_put(dmabuf, ConvertID(HIGFX_JPGDEC_ID));
    osal_mem_free(dmabuf);
    return;
}

static hi_void jpeg_hdec_free_yuv_mem(jpeg_hdec_mem_info *mem_info)
{
    hi_u32 fd = mem_info->output_mem_info.yuv_mem.fd;
    hi_void *dmabuf = osal_mem_handle_get(fd, ConvertID(HIGFX_JPGDEC_ID));
    if (dmabuf == NULL) {
        return;
    }
    osal_mem_ref_put(dmabuf, ConvertID(HIGFX_JPGDEC_ID));
    osal_mem_free(dmabuf);
    return;
}

static hi_void jpeg_hdec_free_xrgb_mem(jpeg_hdec_mem_info *mem_info)
{
    hi_u32 fd = mem_info->output_mem_info.xrgb_mem.fd;
    hi_void *dmabuf = osal_mem_handle_get(fd, ConvertID(HIGFX_JPGDEC_ID));
    if (dmabuf == NULL) {
        return;
    }
    osal_mem_ref_put(dmabuf, ConvertID(HIGFX_JPGDEC_ID));
    osal_mem_free(dmabuf);
    return;
}
