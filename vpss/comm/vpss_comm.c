/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: vpss common
 * Author: zhangjunyu
 * Create: 2016/07/01
 */
#include "vpss_comm.h"
#include "hi_drv_sys.h"
#include "vpss_define.h"
#include "hi_drv_vpss.h"
#include "hi_drv_module.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/************************************************************************/
/* file operation */
/************************************************************************/
static hi_s8 g_vpss_save_path[VPSS_FILE_NAMELENGTH] = "/tmp";
static pq_export_func *g_pq_ext_func = HI_NULL;

hi_void *vpss_comm_fopen(const char *filename, int flags, int mode)
{
    hi_void *filp = osal_klib_fopen(filename, flags, mode);
    return (IS_ERR(filp)) ? HI_NULL : filp;
}

void vpss_comm_fclose(hi_void *filp)
{
    if (filp != HI_NULL) {
        osal_klib_fclose(filp);
    }
}

hi_s32 vpss_comm_fread(char *buf, unsigned int len, hi_void *filp)
{
    hi_s32 ret;

    if (filp == NULL) {
        vpss_error("File is null.\n");
        return -ENOENT;
    }

    ret = osal_klib_fread(buf, len, filp);
    if (ret < 0) {
        vpss_error("fwrite fail!\n");
        return -EACCES;
    }

    return HI_SUCCESS;
}

hi_s32 vpss_comm_fwrite(const char *buf, int len, hi_void *filp)
{
    int writelen;

    if (filp == NULL) {
        vpss_error("File is null.\n");
        return -ENOENT;
    }

    if (buf == NULL) {
        vpss_error("Buf is null.\n");
        return -ENOENT;
    }

    writelen = osal_klib_fwrite(buf, len, filp);
    if (len != writelen) {
        vpss_error("line %d: fwrite fail!\n", __LINE__);
        return -EACCES;
    }

    return HI_SUCCESS;
}

hi_s32 vpss_comm_cmd_check(hi_u32 in_argc, hi_u32 aspect_argc, hi_void *arg, hi_void *private)
{
    if (in_argc != aspect_argc) {
        vpss_error("in_argc is %d, aspect_argc is %d\n", in_argc, aspect_argc);
        return HI_FAILURE;
    }

    if (private == HI_NULL) {
        vpss_error("private is null\n");
        return HI_FAILURE;
    }

    if (arg == HI_NULL) {
        vpss_error("arg is null\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/************************************************************************/
/* event operation */
/************************************************************************/
hi_s32 vpss_comm_init_event(vpss_comm_event *event, hi_s32 init_val1, hi_s32 init_val2)
{
    event->flag_1 = init_val1;
    event->flag_2 = init_val2;

    return osal_wait_init(&(event->queue_head));
}

hi_void vpss_comm_give_event(vpss_comm_event *event, hi_s32 init_val1, hi_s32 init_val2)
{
    event->flag_1 = init_val1;
    event->flag_2 = init_val2;
    osal_wait_wakeup(&(event->queue_head));

    return;
}

int vpss_event_flag(const void *event)
{
    if ((((vpss_comm_event *)event)->flag_1 != 0 || ((vpss_comm_event *)event)->flag_2 != 0)) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

hi_s32 vpss_comm_wait_event(vpss_comm_event *event, hi_s32 wait_time)
{
    int l_ret;

    l_ret = osal_wait_timeout_uninterruptible(&(event->queue_head), vpss_event_flag, event, wait_time);
    if (l_ret == 0 || event->flag_2 == 1 || l_ret < 0) {
        return OSAL_ERR;
    } else {
        return OSAL_OK;
    }
}

hi_void vpss_comm_reset_event(vpss_comm_event *event, hi_s32 init_val1, hi_s32 init_val2)
{
    event->flag_1 = init_val1;
    event->flag_2 = init_val2;

    return;
}

hi_void vpss_comm_destory_event(vpss_comm_event *event)
{
    osal_wait_destroy(&(event->queue_head));

    return;
}

/************************************************************************/
/* spin lock operation */
/************************************************************************/
hi_s32 vpss_comm_init_spin(vpss_comm_spin *lock)
{
    return osal_spin_lock_init(lock);
}

hi_void vpss_comm_down_spin(vpss_comm_spin *lock, unsigned long *flags)
{
    osal_spin_lock_irqsave(lock, flags);
    return;
}

hi_void vpss_comm_up_spin(vpss_comm_spin *lock, unsigned long *flags)
{
    osal_spin_unlock_irqrestore(lock, flags);
    return;
}

hi_void vpss_comm_destory_spin(vpss_comm_spin *lock)
{
    osal_spin_lock_destory(lock);
    lock->lock = HI_NULL;

    return;
}

hi_s32 vpss_comm_transfor10bit_tobit(hi_u8 *pu10_ydata, hi_u8 *ydata, hi_u32 stride, hi_u32 width)
{
    hi_u8 *tmp = HI_NULL;
    hi_u32 i, j, tmp_0;

    tmp = vpss_vmalloc(stride * VPSS_BITDEPTH_8BIT);
    if (tmp == HI_NULL) {
        return HI_FAILURE;
    }

    for (i = 0; i < stride; i++) {
        for (j = 0; j < VPSS_BITDEPTH_8BIT; j++) {
            tmp[i * VPSS_BITDEPTH_8BIT + j] = (pu10_ydata[i] >> j) & 0x1;
        }
    }

    for (i = 0; i < width; i++) {
        tmp_0 = 0;

        for (j = 0; j < VPSS_BITDEPTH_8BIT; j++) {
            tmp_0 |= tmp[i * VPSS_BITDEPTH_10BIT + j + 2] << j;  // drop the low 2 bit when 10 bit turn to 8 bit
        }

        ydata[i] = tmp_0;
    }

    vpss_vfree(tmp);
    return HI_SUCCESS;
}

static hi_void vpss_comm_one_line10_to8_bit(int type, hi_char *in_addr, hi_u32 width, hi_char *out_addr)
{
    hi_u32 i;
    hi_u32 j;
    hi_u32 cnt;
    hi_char *tmp_mem = HI_NULL;
    hi_u8 vpss_temp[10] = { 0 };  /* create 10 bytes array */
    tmp_mem = in_addr;
    cnt = hiceiling(width, 4);  /* 4个像素一循环，占5 byte,不足四个像素也占5byte处理 */

    for (i = 0; i < cnt; i++) {
        for (j = 0; j < 5; j++) {                   /* cycle every 5 bytes */
            vpss_temp[j] = *(tmp_mem + i * 5 + j);  /* cycle every 5 bytes */
        }

        if (type == 0) {
            *out_addr = ((vpss_temp[1] << 6) & 0xc0) | ((vpss_temp[0] >> 2) & 0x3f);  /* idx 0 high 6 bit,idx 1 low 2 */
            out_addr++;
            *out_addr = ((vpss_temp[2] << 4) & 0xf0) | ((vpss_temp[1] >> 4) & 0x0f);  /* idx 1 high 4 bit,idx 2 low 4 */
            out_addr++;
            *out_addr = ((vpss_temp[3] << 2) & 0xfc) | ((vpss_temp[2] >> 6) & 0x03);  /* idx 2 high 2 bit,idx 3 low 6 */
            out_addr++;
            *out_addr = vpss_temp[4] & 0xff;  /* idx 4 all 8 bit */
            out_addr++;
        } else if (type == 1) {
            *out_addr = ((vpss_temp[2] << 4) & 0xf0) | ((vpss_temp[1] >> 4) & 0x0f);  /* idx 1 high 4 bit,idx 2 low 4 */
            out_addr++;
            *out_addr = vpss_temp[4] & 0xff;  /* idx 4 all 8 bit */
            out_addr++;
        } else {
            *out_addr = ((vpss_temp[1] << 6) & 0xc0) | ((vpss_temp[0] >> 2) & 0x3f);  /* idx 0 high 6 bit,idx 1 low 2 */
            out_addr++;
            *out_addr = ((vpss_temp[3] << 2) & 0xfc) | ((vpss_temp[2] >> 6) & 0x03);  /* idx 2 high 2 bit,idx 3 low 6 */
            out_addr++;
        }
    }
}

/* mem comm */
hi_s32 vpss_comm_mem_alloc(drv_vpss_mem_attr *in_mem_attr, drv_vpss_mem_info *out_mem_info)
{
    hi_s32 ret;
    drv_vpss_mem_info mem_info = { 0 };

    if (in_mem_attr == HI_NULL || out_mem_info == HI_NULL) {
        vpss_error("VPSS_COMM_AllocateSmmuMem para is null !!!!!!!!!!\n");
        return HI_FAILURE;
    }

    ret = drv_vpss_mem_alloc(in_mem_attr, &mem_info);
    if (ret != HI_SUCCESS) {
        vpss_error("malloc failed !!!!!!!!!!\n");
        return HI_FAILURE;
    }

    out_mem_info->mode = in_mem_attr->mode;
    out_mem_info->is_cache = in_mem_attr->is_cache;
    out_mem_info->size = mem_info.size; /* after align size */
    out_mem_info->phy_addr = mem_info.phy_addr;
    out_mem_info->vir_addr = mem_info.vir_addr;
    out_mem_info->dma_handle = mem_info.dma_handle;

    if ((mem_info.vir_addr != HI_NULL) &&
        ((in_mem_attr->mode == OSAL_MMZ_TYPE) || (in_mem_attr->mode == OSAL_NSSMMU_TYPE))) {
        memset(out_mem_info->vir_addr, 0, out_mem_info->size);
    }

    return ret;
}

hi_void vpss_comm_mem_free(drv_vpss_mem_info *mem_info)
{
    hi_s32 ret;

    vpss_check_null_noret(mem_info);

    ret = drv_vpss_mem_free(mem_info);
    if (ret != HI_SUCCESS) {
        vpss_error("drv_vpss_mem_free faild! ret = %d\n", ret);
    }

    return;
}

hi_s32 vpss_comm_mem_map_vir_addr(hi_drv_vpss_video_frame *frame, drv_vpss_mem_info *mem_info)
{
    hi_s32 ret;

    vpss_check_null(frame);
    vpss_check_null(mem_info);

    if (frame->comm_frame.secure == HI_TRUE) {
        vpss_error("secure cannot map vir addr !!! \n");
        return HI_FAILURE;
    }

    ret = drv_vpss_mem_map_vir_addr((struct dma_buf *)frame->comm_frame.buf_addr[0].dma_handle, mem_info);
    if (ret != HI_SUCCESS) {
        vpss_error("ret = %d\n", ret);
        return ret;
    }

    return ret;
}

hi_s32 vpss_comm_mem_umap_vir_addr(hi_drv_vpss_video_frame *frame, drv_vpss_mem_info *mem_info)
{
    hi_s32 ret;

    vpss_check_null(mem_info);
    ret = drv_vpss_mem_unmap_vir_addr((struct dma_buf *)frame->comm_frame.buf_addr[0].dma_handle, mem_info->vir_addr);
    if (ret != HI_SUCCESS) {
        vpss_error("ret = %d\n", ret);
    }

    return ret;
}

hi_s32 vpss_comm_mem_flush(drv_vpss_mem_info *mem_info)
{
    return HI_SUCCESS;
}

static void write_sp420_y_data(file *file_handle, hi_u8 *mem_addr, hi_u8 *store_y_data, hi_drv_vpss_video_frame *frame)
{
    hi_u32 i;
    file *fp = file_handle;
    hi_u8 *ptr = mem_addr;
    hi_u8 *y_data = store_y_data;

    for (i = 0; i < frame->comm_frame.height; i++) {
        memcpy(y_data, ptr, sizeof(hi_u8) * frame->vpss_buf_addr[0].stride_y);

        if (vpss_comm_fwrite(y_data, frame->comm_frame.width, fp) != HI_SUCCESS) {
            vpss_error("fwrite fail!\n");
        }

        ptr += frame->vpss_buf_addr[0].stride_y;
    }
}

static void write_sp420_uv_data(hi_void *file_handle, hi_u8 *mem_addr,
    hi_u8 *store_u_data, hi_u8 *store_v_data, hi_drv_vpss_video_frame *frame)
{
    hi_u32 i, j;
    hi_u8 *ptr = mem_addr;
    hi_void *fp = file_handle;
    hi_u8 *u_data = store_u_data;
    hi_u8 *data = store_v_data;
    hi_u32 width = frame->comm_frame.width;
    hi_u32 height = frame->comm_frame.height;

    for (i = 0; i < height / 2; i++) {     /* the outside loop times is height / 2 */
        for (j = 0; j < width / 2; j++) {  /* the inside loop times is width / 2 */
            if (frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV21) {
                data[i * width / 2 + j] = ptr[2 * j];        /* get v data by 2*j index */
                u_data[i * width / 2 + j] = ptr[2 * j + 1];  /* get u data by 2*j + 1 index */
            } else {
                u_data[i * width / 2 + j] = ptr[2 * j];    /* get u data by 2*j index */
                data[i * width / 2 + j] = ptr[2 * j + 1];  /* get v data by 2*j + 1index */
            }
        }

        ptr += frame->vpss_buf_addr[0].stride_c;
    }

    if (vpss_comm_fwrite(u_data, width * height / 0x2 / 0x2, fp) != HI_SUCCESS) {
        vpss_error("fwrite fail!\n");
    }

    if (vpss_comm_fwrite(data, width * height / 0x2 / 0x2, fp) != HI_SUCCESS) {
        vpss_error("fwrite fail!\n");
    }
}

static hi_s32 write_sp420_to_file_8bit(char *file_name, hi_u8 *mem_addr, hi_drv_vpss_video_frame *frame)
{
    hi_void *fp = HI_NULL;
    hi_u8 *y_data = HI_NULL;
    hi_u8 *u_data = HI_NULL;
    hi_u8 *data = HI_NULL;
    hi_u8 *y_addr = mem_addr;
    hi_u8 *addr = mem_addr + frame->vpss_buf_addr[0].phy_addr_c - frame->vpss_buf_addr[0].phy_addr_y;

    u_data = vpss_vmalloc(frame->comm_frame.width * frame->comm_frame.height / 0x2 / 0x2);
    if (u_data == HI_NULL) {
        return HI_FAILURE;
    }

    data = vpss_vmalloc(frame->comm_frame.width * frame->comm_frame.height / 0x2 / 0x2);
    if (data == HI_NULL) {
        vpss_vfree(u_data);
        return HI_FAILURE;
    }

    y_data = vpss_vmalloc(frame->vpss_buf_addr[0].stride_y);
    if (y_data == HI_NULL) {
        vpss_vfree(u_data);
        vpss_vfree(data);
        return HI_FAILURE;
    }

    fp = vpss_comm_fopen(file_name, OSAL_O_RDWR | OSAL_O_CREAT | OSAL_O_APPEND, VPSS_FILE_OPERATE_RIGHTS);
    if (fp == HI_NULL) {
        vpss_vfree(u_data);
        vpss_vfree(data);
        vpss_vfree(y_data);
        return HI_FAILURE;
    }

    write_sp420_y_data(fp, y_addr, y_data, frame);
    write_sp420_uv_data(fp, addr, u_data, data, frame);
    vpss_comm_fclose(fp);
    vpss_vfree(u_data);
    vpss_vfree(data);
    vpss_vfree(y_data);

    return HI_SUCCESS;
}

static hi_s32 save_sp420_to_file_8bit(hi_drv_vpss_video_frame *frame, hi_char *pch_file)
{
    char str[VPSS_PICTURE_SAVE_PATH_MAX_LENGTH] = { 0 };
    unsigned char *vir_addr = HI_NULL;
    drv_vpss_mem_info mem_info = { 0 };
    hi_s32 ret;
    snprintf(str, VPSS_PICTURE_SAVE_PATH_MAX_LENGTH, "%s/%s", g_vpss_save_path, pch_file);

    if (HI_SUCCESS == vpss_comm_mem_map_vir_addr(frame, &mem_info)) {
        vir_addr = mem_info.vir_addr;
    } else {
        vpss_error("address is not valid!\n");
        return HI_FAILURE;
    }

    if (vir_addr == HI_NULL) {
        vpss_error("address is not valid!\n");
    } else {
        ret = write_sp420_to_file_8bit(str, vir_addr, frame);
        if (ret != HI_SUCCESS) {
            vpss_error("write file '%s' fail!\n", str);
            vpss_comm_mem_umap_vir_addr(frame, &mem_info);
            return HI_FAILURE;
        }

        osal_printk("%s,%d; 2d image saved to '%s' \n", __FUNCTION__, __LINE__, str);
        osal_printk("W=%-6d H=%-6d format=%-6d bit_width=%-6d ys=0x%-6x cs=0x%-6x\n",
            frame->comm_frame.width, frame->comm_frame.height, frame->comm_frame.pixel_format,
            frame->comm_frame.bit_width, frame->vpss_buf_addr[0].stride_y, frame->vpss_buf_addr[0].stride_c);
        vpss_comm_mem_umap_vir_addr(frame, &mem_info);
    }

    return HI_SUCCESS;
}

static hi_s32 save_out_sp420_to_file_8bit(hi_drv_vpss_video_frame *frame, hi_char *pch_file)
{
    char str[VPSS_PICTURE_SAVE_PATH_MAX_LENGTH] = { 0 };
    unsigned char *vir_addr = HI_NULL;
    drv_vpss_mem_info mem_info = { 0 };
    hi_s32 ret;

    snprintf(str, VPSS_PICTURE_SAVE_PATH_MAX_LENGTH, "%s/%s", g_vpss_save_path, pch_file);

    if (HI_SUCCESS == vpss_comm_mem_map_vir_addr(frame, &mem_info)) {
        vir_addr = mem_info.vir_addr;
    } else {
        vpss_error("address is not valid!\n");
        return HI_FAILURE;
    }

    if (vir_addr == HI_NULL) {
        vpss_error("address is not valid!\n");
    } else {
        ret = write_sp420_to_file_8bit(str, vir_addr, frame);
        if (ret != HI_SUCCESS) {
            vpss_error("write file '%s' fail!\n", str);
            vpss_comm_mem_umap_vir_addr(frame, &mem_info);
            return HI_FAILURE;
        }

        osal_printk("%s,%d; 2d image saved to '%s' \n", __FUNCTION__, __LINE__, str);
        osal_printk("W=%-6d H=%-6d format=%-6d bit_width=%-6d ys=0x%-6x cs=0x%-6x\n",
            frame->comm_frame.width, frame->comm_frame.height, frame->comm_frame.pixel_format,
            frame->comm_frame.bit_width, frame->vpss_buf_addr[0].stride_y, frame->vpss_buf_addr[0].stride_c);
        vpss_comm_mem_umap_vir_addr(frame, &mem_info);
    }

    return HI_SUCCESS;
}

static hi_s32 write_sp420_y_data_10bit(hi_void *file_handle, hi_u8 *mem_addr,
    hi_u8 *store_y_data, hi_drv_vpss_video_frame *frame)
{
    hi_u32 i;
    hi_u8 *y_data_10bit = HI_NULL;
    hi_u8 *y_data = store_y_data;
    hi_u8 *ptr = mem_addr;
    hi_void *fp = file_handle;
    hi_u32 width = frame->comm_frame.width;
    hi_u32 height = frame->comm_frame.height;
    hi_s32 ret;

    y_data_10bit = vpss_vmalloc(frame->vpss_buf_addr[0].stride_y);
    if (y_data_10bit == HI_NULL) {
        return HI_FAILURE;
    }

    for (i = 0; i < height; i++) {
        memcpy(y_data_10bit, ptr, frame->vpss_buf_addr[0].stride_y);
        ret = vpss_comm_transfor10bit_tobit(y_data_10bit, y_data, frame->vpss_buf_addr[0].stride_y, width);
        if (ret != HI_SUCCESS) {
            vpss_error("line %d: transfor10bit_tobit fail!\n", __LINE__);
        }

        if (vpss_comm_fwrite(y_data, width, fp) != HI_SUCCESS) {
            vpss_error("fwrite fail!\n");
        }

        ptr += frame->vpss_buf_addr[0].stride_y;
    }

    vpss_vfree(y_data_10bit);

    return HI_SUCCESS;
}

static hi_s32 write_sp420_uv_data_10bit(hi_void *file_handle, hi_u8 *mem_addr, hi_u8 *store_u_data,
    hi_u8 *store_v_data, hi_drv_vpss_video_frame *frame)
{
    hi_u8 *uv_data_10bit = HI_NULL;
    hi_u32 i;
    hi_void *fp = file_handle;
    hi_u8 *u_data = store_u_data;
    hi_u8 *data = store_v_data;
    hi_u8 *ptr = mem_addr;
    hi_u32 width = frame->comm_frame.width;
    hi_u32 height = frame->comm_frame.height;

    uv_data_10bit = vpss_vmalloc(frame->vpss_buf_addr[0].stride_c);
    if (uv_data_10bit == HI_NULL) {
        return HI_FAILURE;
    }

    for (i = 0; i < height / 2; i++) {  /* loop height / 2 times */
        memcpy(uv_data_10bit, ptr, frame->vpss_buf_addr[0].stride_c);
        vpss_comm_one_line10_to8_bit(1, uv_data_10bit, width,  /* 1 means get  u data from uv data */
                                     u_data + i * width / 2);  /* 1/2 used to cal */
        vpss_comm_one_line10_to8_bit(2, uv_data_10bit, width,  /* 2 means get  v data from uv data */
                                     data + i * width / 2);    /* 1/2 used to cal */
        ptr += frame->vpss_buf_addr[0].stride_c;
    }

    if (frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV21) {
        if (vpss_comm_fwrite(u_data, width * height / 0x2 / 0x2, fp) != HI_SUCCESS) {
            vpss_error("fwrite fail!\n");
        }
        if (vpss_comm_fwrite(data, width * height / 0x2 / 0x2, fp) != HI_SUCCESS) {
            vpss_error("fwrite fail!\n");
        }
    } else {
        if (vpss_comm_fwrite(data, width * height / 0x2 / 0x2, fp) != HI_SUCCESS) {
            vpss_error("fwrite fail!\n");
        }
        if (vpss_comm_fwrite(u_data, width * height / 0x2 / 0x2, fp) != HI_SUCCESS) {
            vpss_error("fwrite fail!\n");
        }
    }

    vpss_vfree(uv_data_10bit);

    return HI_SUCCESS;
}

static hi_s32 write_sp420_to_file_10bit(const char *file_name, hi_u8 *mem_addr, hi_drv_vpss_video_frame *frame)
{
    hi_u8 *u_data = HI_NULL;
    hi_u8 *data = HI_NULL;
    hi_u8 *y_data = HI_NULL;
    hi_void *fp = HI_NULL;
    hi_s32 ret;
    hi_u8 *y_addr = mem_addr;
    hi_u8 *addr = mem_addr + frame->vpss_buf_addr[0].phy_addr_c - frame->vpss_buf_addr[0].phy_addr_y;

    u_data = vpss_vmalloc(frame->comm_frame.width * frame->comm_frame.height / 0x2 / 0x2);
    if (u_data == HI_NULL) {
        return HI_FAILURE;
    }

    data = vpss_vmalloc(frame->comm_frame.width * frame->comm_frame.height / 0x2 / 0x2);
    if (data == HI_NULL) {
        goto VPSS_FREE_U_DATA;
    }

    y_data = vpss_vmalloc(frame->comm_frame.width);
    if (y_data == HI_NULL) {
        goto VPSS_FREE_UV_DATA;
    }

    fp = vpss_comm_fopen(file_name, OSAL_O_RDWR | OSAL_O_CREAT | OSAL_O_APPEND, VPSS_FILE_OPERATE_RIGHTS);
    if (fp == HI_NULL) {
        goto VPSS_FREE_YUV_DATA;
    }

    ret = write_sp420_y_data_10bit(fp, y_addr, y_data, frame);
    if (ret != HI_SUCCESS) {
        goto VPSS_FREE_YUV_AND_FILE;
    }

    ret = write_sp420_uv_data_10bit(fp, addr, u_data, data, frame);
    if (ret != HI_SUCCESS) {
        goto VPSS_FREE_YUV_AND_FILE;
    }

    vpss_comm_fclose(fp);
    vpss_vfree(u_data);
    vpss_vfree(data);
    vpss_vfree(y_data);

    return HI_SUCCESS;

VPSS_FREE_YUV_AND_FILE:
    vpss_comm_fclose(fp);
VPSS_FREE_YUV_DATA:
    vpss_vfree(y_data);
VPSS_FREE_UV_DATA:
    vpss_vfree(data);
VPSS_FREE_U_DATA:
    vpss_vfree(u_data);
    return HI_FAILURE;
}

static hi_s32 save_sp420_to_file_10bit(hi_drv_vpss_video_frame *frame, hi_char *pch_file)
{
    char str[VPSS_PICTURE_SAVE_PATH_MAX_LENGTH] = { 0 };
    unsigned char *ptr = HI_NULL;
    unsigned char *org_addr = HI_NULL;
    drv_vpss_mem_info mem_info = { 0 };
    hi_s32 ret;
    snprintf(str, VPSS_PICTURE_SAVE_PATH_MAX_LENGTH, "%s/%s", g_vpss_save_path, pch_file);

    if (HI_SUCCESS == vpss_comm_mem_map_vir_addr(frame, &mem_info)) {
        ptr = mem_info.vir_addr;
        org_addr = mem_info.vir_addr;
    } else {
        vpss_error("address is not valid!\n");
    }

    if (ptr == HI_NULL) {
        vpss_error("address is not valid!\n");
    } else {
        ret = write_sp420_to_file_10bit(str, ptr, frame);
        if (ret != HI_SUCCESS) {
            vpss_error("write file '%s' fail!\n", str);
            vpss_comm_mem_umap_vir_addr(frame, &mem_info);
            return HI_FAILURE;
        }

        osal_printk("%s,%d; 2d image saved to '%s' \n", __FUNCTION__, __LINE__, str);
        osal_printk("W=%-6d H=%-6d format=%-6d bit_width=%-6d ys=0x%-6x cs=0x%-6x\n",
            frame->comm_frame.width, frame->comm_frame.height, frame->comm_frame.pixel_format,
            frame->comm_frame.bit_width, frame->vpss_buf_addr[0].stride_y, frame->vpss_buf_addr[0].stride_c);
        vpss_comm_mem_umap_vir_addr(frame, &mem_info);
    }

    return HI_SUCCESS;
}

static void write_sp422_y_data(hi_void *file_handle, hi_u8 *mem_addr, hi_u8 *store_y_data,
    hi_drv_vpss_video_frame *frame)
{
    hi_u32 i;
    hi_void *fp = file_handle;
    hi_u8 *ptr = mem_addr;
    hi_u8 *y_data = store_y_data;

    for (i = 0; i < frame->comm_frame.height; i++) {
        memcpy(y_data, ptr, sizeof(hi_u8) * frame->vpss_buf_addr[0].stride_y);

        if (vpss_comm_fwrite(y_data, frame->comm_frame.width, fp) != HI_SUCCESS) {
            vpss_error("fwrite fail!\n");
        }

        ptr += frame->vpss_buf_addr[0].stride_y;
    }
}

static void write_sp422_uv_data(hi_void *file_handle, hi_u8 *mem_addr, hi_u8 *store_u_data,
    hi_u8 *store_v_data, hi_drv_vpss_video_frame *frame)
{
    hi_u32 i, j;
    hi_u8 *ptr = mem_addr;
    hi_void *fp = file_handle;
    hi_u8 *u_data = store_u_data;
    hi_u8 *data = store_v_data;
    hi_u32 width = frame->comm_frame.width;
    hi_u32 height = frame->comm_frame.height;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width / 2; j++) {  /* the inside loop times is width / 2 */
            if (frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV61_2X1) {
                data[i * width / 2 + j] = ptr[2 * j];        /* get v data by 2*j index */
                u_data[i * width / 2 + j] = ptr[2 * j + 1];  /* get u data by 2*j+1 index */
            } else {
                u_data[i * width / 2 + j] = ptr[2 * j];    /* get u data by 2*j index */
                data[i * width / 2 + j] = ptr[2 * j + 1];  /* get v data by 2*j+1 index */
            }
        }

        ptr += frame->vpss_buf_addr[0].stride_c;
    }

    if (vpss_comm_fwrite(u_data, width * height / 0x2, fp) != HI_SUCCESS) {
        vpss_error("fwrite fail!\n");
    }

    if (vpss_comm_fwrite(data, width * height / 0x2, fp) != HI_SUCCESS) {
        vpss_error("fwrite fail!\n");
    }
}

static hi_s32 write_sp422_to_file_8bit(const char *file_name, hi_u8 *mem_addr, hi_drv_vpss_video_frame *frame)
{
    hi_u8 *u_data = HI_NULL;
    hi_u8 *data = HI_NULL;
    hi_u8 *y_data = HI_NULL;
    hi_void *fp = HI_NULL;
    hi_u8 *y_addr = mem_addr;
    hi_u8 *addr = mem_addr + frame->vpss_buf_addr[0].phy_addr_c - frame->vpss_buf_addr[0].phy_addr_y;

    u_data = vpss_vmalloc(frame->comm_frame.width * frame->comm_frame.height / 0x2);
    if (u_data == HI_NULL) {
        return HI_FAILURE;
    }

    data = vpss_vmalloc(frame->comm_frame.width * frame->comm_frame.height / 0x2);
    if (data == HI_NULL) {
        vpss_vfree(u_data);
        return HI_FAILURE;
    }

    y_data = vpss_vmalloc(frame->vpss_buf_addr[0].stride_y);
    if (y_data == HI_NULL) {
        vpss_vfree(u_data);
        vpss_vfree(data);
        return HI_FAILURE;
    }

    fp = vpss_comm_fopen(file_name, OSAL_O_RDWR | OSAL_O_CREAT | OSAL_O_APPEND, VPSS_FILE_OPERATE_RIGHTS);
    if (fp == HI_NULL) {
        vpss_error("[%s][%d] open file '%s' fail!\n", __FUNCTION__, __LINE__, file_name);
        vpss_vfree(u_data);
        vpss_vfree(data);
        vpss_vfree(y_data);
        return HI_FAILURE;
    }

    write_sp422_y_data(fp, y_addr, y_data, frame);
    write_sp422_uv_data(fp, addr, u_data, data, frame);
    vpss_comm_fclose(fp);
    vpss_vfree(u_data);
    vpss_vfree(data);
    vpss_vfree(y_data);

    return HI_SUCCESS;
}

static hi_s32 save_sp422_to_file_8bit(hi_drv_vpss_video_frame *frame, hi_char *pch_file)
{
    char str[VPSS_PICTURE_SAVE_PATH_MAX_LENGTH] = { 0 };
    unsigned char *ptr = HI_NULL;
    drv_vpss_mem_info mem_info = { 0 };
    hi_s32 ret;
    snprintf(str, VPSS_PICTURE_SAVE_PATH_MAX_LENGTH, "%s/%s", g_vpss_save_path, pch_file);

    if (HI_SUCCESS == vpss_comm_mem_map_vir_addr(frame, &mem_info)) {
        ptr = mem_info.vir_addr;
    } else {
        vpss_error("address is not valid!\n");
    }

    if (ptr == HI_NULL) {
        vpss_error("address is not valid!\n");
    } else {
        ret = write_sp422_to_file_8bit(str, ptr, frame);
        if (ret != HI_SUCCESS) {
            vpss_error("write file '%s' fail!\n", str);
            vpss_comm_mem_umap_vir_addr(frame, &mem_info);
            return HI_FAILURE;
        }

        osal_printk("%s,%d; 2d image saved to '%s' \n", __FUNCTION__, __LINE__, str);
        osal_printk("W=%-6d H=%-6d format=%-6d bit_width=%-6d ys=0x%-6x cs=0x%-6x\n",
            frame->comm_frame.width, frame->comm_frame.height, frame->comm_frame.pixel_format,
            frame->comm_frame.bit_width, frame->vpss_buf_addr[0].stride_y, frame->vpss_buf_addr[0].stride_c);
        vpss_comm_mem_umap_vir_addr(frame, &mem_info);
    }

    return HI_SUCCESS;
}

static hi_s32 write_sp422_y_data_10bit(hi_void *file_handle, hi_u8 *mem_addr,
    hi_u8 *store_y_data, hi_drv_vpss_video_frame *frame)
{
    hi_u32 i;
    hi_u8 *y_data_10bit = HI_NULL;
    hi_u8 *y_data = store_y_data;
    hi_u8 *ptr = mem_addr;
    hi_void *fp = file_handle;
    hi_u32 width = frame->comm_frame.width;
    hi_u32 height = frame->comm_frame.height;
    hi_s32 ret;

    y_data_10bit = vpss_vmalloc(frame->vpss_buf_addr[0].stride_y);
    if (y_data_10bit == HI_NULL) {
        return HI_FAILURE;
    }

    for (i = 0; i < height; i++) {
        memcpy(y_data_10bit, ptr, frame->vpss_buf_addr[0].stride_y);
        ret = vpss_comm_transfor10bit_tobit(y_data_10bit, y_data, frame->vpss_buf_addr[0].stride_y, width);
        if (ret != HI_SUCCESS) {
            vpss_error("line %d: transfor10bit_tobit fail!\n", __LINE__);
        }

        if (vpss_comm_fwrite(y_data, width, fp) != HI_SUCCESS) {
            vpss_error("fwrite fail!\n");
        }

        ptr += frame->vpss_buf_addr[0].stride_y;
    }

    vpss_vfree(y_data_10bit);

    return HI_SUCCESS;
}

static hi_s32 write_sp422_uv_data_10bit(hi_void *file_handle, hi_u8 *mem_addr, hi_u8 *store_u_data,
    hi_u8 *store_v_data, hi_drv_vpss_video_frame *frame)
{
    hi_u8 *uv_data_10bit = HI_NULL;
    hi_u32 i;
    hi_void *fp = file_handle;
    hi_u8 *u_data = store_u_data;
    hi_u8 *data = store_v_data;
    hi_u8 *ptr = mem_addr;
    hi_u32 width = frame->comm_frame.width;
    hi_u32 height = frame->comm_frame.height;

    uv_data_10bit = vpss_vmalloc(frame->vpss_buf_addr[0].stride_c);
    if (uv_data_10bit == HI_NULL) {
        return HI_FAILURE;
    }

    for (i = 0; i < height; i++) {
        memcpy(uv_data_10bit, ptr, frame->vpss_buf_addr[0].stride_c);
        vpss_comm_one_line10_to8_bit(1, uv_data_10bit, width,  /* 1 means get  u data from uv data */
                                     u_data + i * width / 2);  /* 1/2 used to cal */
        vpss_comm_one_line10_to8_bit(2, uv_data_10bit, width,  /* 2 means get  v data from uv data */
                                     data + i * width / 2);    /* 1/2 used to cal */
        ptr += frame->vpss_buf_addr[0].stride_c;
    }

    if (vpss_comm_fwrite(u_data, width * height / 0x2, fp) != HI_SUCCESS) {
        vpss_error("fwrite fail!\n");
    }

    if (vpss_comm_fwrite(data, width * height / 0x2, fp) != HI_SUCCESS) {
        vpss_error("fwrite fail!\n");
    }

    vpss_vfree(uv_data_10bit);

    return HI_SUCCESS;
}

static hi_s32 write_sp422_to_file_10bit(const char *file_name, hi_u8 *mem_addr, hi_drv_vpss_video_frame *frame)
{
    hi_u8 *u_data = HI_NULL;
    hi_u8 *data = HI_NULL;
    hi_u8 *y_data = HI_NULL;
    hi_void *fp = HI_NULL;
    hi_s32 ret;
    hi_u8 *y_addr = mem_addr;
    hi_u8 *addr = mem_addr + frame->vpss_buf_addr[0].phy_addr_c - frame->vpss_buf_addr[0].phy_addr_y;

    u_data = vpss_vmalloc(frame->comm_frame.width * frame->comm_frame.height / 0x2);
    if (u_data == HI_NULL) {
        return HI_FAILURE;
    }

    data = vpss_vmalloc(frame->comm_frame.width * frame->comm_frame.height / 0x2);
    if (data == HI_NULL) {
        goto VPSS_FREE_U_DATA;
    }

    y_data = vpss_vmalloc(frame->comm_frame.width);
    if (y_data == HI_NULL) {
        goto VPSS_FREE_UV_DATA;
    }

    fp = vpss_comm_fopen(file_name, OSAL_O_RDWR | OSAL_O_CREAT | OSAL_O_APPEND, VPSS_FILE_OPERATE_RIGHTS);
    if (fp == HI_NULL) {
        goto VPSS_FREE_YUV_DATA;
    }

    ret = write_sp422_y_data_10bit(fp, y_addr, y_data, frame);
    if (ret != HI_SUCCESS) {
        goto VPSS_FREE_YUV_AND_FILE;
    }

    ret = write_sp422_uv_data_10bit(fp, addr, u_data, data, frame);
    if (ret != HI_SUCCESS) {
        goto VPSS_FREE_YUV_AND_FILE;
    }

    vpss_comm_fclose(fp);
    vpss_vfree(u_data);
    vpss_vfree(data);
    vpss_vfree(y_data);

    return HI_SUCCESS;

VPSS_FREE_YUV_AND_FILE:
    vpss_comm_fclose(fp);
VPSS_FREE_YUV_DATA:
    vpss_vfree(y_data);
VPSS_FREE_UV_DATA:
    vpss_vfree(data);
VPSS_FREE_U_DATA:
    vpss_vfree(u_data);
    return HI_FAILURE;
}

static hi_s32 save_sp422_to_file_10bit(hi_drv_vpss_video_frame *frame, hi_char *pch_file)
{
    char str[VPSS_PICTURE_SAVE_PATH_MAX_LENGTH] = { 0 };
    unsigned char *vir_addr = HI_NULL;
    drv_vpss_mem_info mem_info = { 0 };
    hi_s32 ret;
    snprintf(str, VPSS_PICTURE_SAVE_PATH_MAX_LENGTH, "%s/%s", g_vpss_save_path, pch_file);

    if (HI_SUCCESS == vpss_comm_mem_map_vir_addr(frame, &mem_info)) {
        vir_addr = mem_info.vir_addr;
    } else {
        vpss_error("address is not valid!\n");
    }

    if (vir_addr == HI_NULL) {
        vpss_error("address is not valid!\n");
    } else {
        ret = write_sp422_to_file_10bit(str, vir_addr, frame);
        if (ret != HI_SUCCESS) {
            vpss_error("write file '%s' fail!\n", str);
            vpss_comm_mem_umap_vir_addr(frame, &mem_info);
            return HI_FAILURE;
        }

        osal_printk("%s,%d; 2d image saved to '%s' \n", __FUNCTION__, __LINE__, str);
        osal_printk("W=%-6d H=%-6d format=%-6d bit_width=%-6d ys=0x%-6x cs=0x%-6x\n",
            frame->comm_frame.width, frame->comm_frame.height, frame->comm_frame.pixel_format,
            frame->comm_frame.bit_width, frame->vpss_buf_addr[0].stride_y, frame->vpss_buf_addr[0].stride_c);
        vpss_comm_mem_umap_vir_addr(frame, &mem_info);
    }

    return HI_SUCCESS;
}

static hi_s32 save_sp420_to_file(hi_drv_vpss_video_frame *frame, hi_char *pch_file)
{
    hi_s32 ret;

    if (HI_DRV_PIXEL_BITWIDTH_8BIT == frame->comm_frame.bit_width) {
        ret = save_sp420_to_file_8bit(frame, pch_file);
    } else if (HI_DRV_PIXEL_BITWIDTH_10BIT == frame->comm_frame.bit_width) {
        ret = save_sp420_to_file_10bit(frame, pch_file);
    } else {
        vpss_error("vpss can't support this bitwidth =%d savetofile\n", frame->comm_frame.bit_width);
        ret = HI_FAILURE;
    }

    return ret;
}

static hi_s32 save_out_sp420_to_file(hi_drv_vpss_video_frame *frame, hi_char *pch_file)
{
    hi_s32 ret;

    if (HI_DRV_PIXEL_BITWIDTH_8BIT == frame->comm_frame.bit_width) {
        ret = save_out_sp420_to_file_8bit(frame, pch_file);
    } else if (HI_DRV_PIXEL_BITWIDTH_10BIT == frame->comm_frame.bit_width) {
        ret = save_sp420_to_file_10bit(frame, pch_file);
    } else {
        vpss_error("vpss can't support this bitwidth =%d savetofile\n", frame->comm_frame.bit_width);
        ret = HI_FAILURE;
    }

    return ret;
}

static hi_s32 save_sp422_to_file(hi_drv_vpss_video_frame *frame, hi_char *pch_file)
{
    hi_s32 ret;

    if (HI_DRV_PIXEL_BITWIDTH_8BIT == frame->comm_frame.bit_width) {
        ret = save_sp422_to_file_8bit(frame, pch_file);
    } else if (HI_DRV_PIXEL_BITWIDTH_10BIT == frame->comm_frame.bit_width) {
        ret = save_sp422_to_file_10bit(frame, pch_file);
    } else {
        vpss_error("vpss can't support this bitwidth =%d savetofile\n", frame->comm_frame.bit_width);
        ret = HI_FAILURE;
    }

    return ret;
}

hi_s32 vpss_comm_save_out_frame_to_file(hi_drv_vpss_video_frame *frame, hi_char *pch_file, hi_u32 filename_size)
{
    hi_s32 ret;

    if (filename_size > VPSS_FILE_NAMELENGTH) {
        vpss_error("file name is too long %d\n", filename_size);
        return HI_FAILURE;
    }

    if ((strlen(pch_file) >= filename_size) ||
        (strlen(pch_file) == 0)) {
        vpss_error("file name is not right %d\n", strlen(pch_file));
        return HI_FAILURE;
    }

    if ((frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV21) ||
        (frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV12)) {
        ret = save_out_sp420_to_file(frame, pch_file);
    } else if ((frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV61) ||
               (frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV16) ||
               (frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV16_2X1) ||
               (frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV61_2X1)) {
        ret = save_sp422_to_file(frame, pch_file);
    } else {
        vpss_error("vpss can't support this pixfmt=%d savetofile\n", frame->comm_frame.pixel_format);
        ret = HI_FAILURE;
    }

    return ret;
}

hi_s32 vpss_comm_save_frame_to_file(hi_drv_vpss_video_frame *frame, hi_char *pch_file, hi_u32 filename_size)
{
    hi_s32 ret;

    if (filename_size > VPSS_FILE_NAMELENGTH) {
        vpss_error("file name is too long %d\n", filename_size);
        return HI_FAILURE;
    }

    if ((strlen(pch_file) >= filename_size) ||
        (strlen(pch_file) == 0)) {
        vpss_error("file name is not right %d\n", strlen(pch_file));
        return HI_FAILURE;
    }

    if ((frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV21) ||
        (frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV12)) {
        ret = save_sp420_to_file(frame, pch_file);
    } else if ((frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV61) ||
               (frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV16) ||
               (frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV16_2X1) ||
               (frame->comm_frame.pixel_format == HI_DRV_PIXEL_FMT_NV61_2X1)) {
        ret = save_sp422_to_file(frame, pch_file);
    } else {
        vpss_error("vpss can't support this pixfmt=%d savetofile\n", frame->comm_frame.pixel_format);
        ret = HI_FAILURE;
    }

    return ret;
}

hi_s32 vpss_comm_calc_cmp_y_stride(hi_u32 *stride_y, hi_u32 width,
                                   hi_drv_pixel_bitwidth out_bit_width, hi_bool loss_cmp, hi_u32 cmp_rate)
{
    hi_u32 dep;

    if (HI_DRV_PIXEL_BITWIDTH_8BIT == out_bit_width) {
        dep = VPSS_BITDEPTH_8BIT;
    } else if (HI_DRV_PIXEL_BITWIDTH_10BIT == out_bit_width) {
        dep = VPSS_BITDEPTH_10BIT;
    } else {
        vpss_error("vpss can't support this bitwidth %d\n", out_bit_width);
        *stride_y = 0;
        return HI_FAILURE;
    }

    if (loss_cmp == HI_TRUE) {
        *stride_y = vpss_calc_loss_cmp_ystride(width, dep, cmp_rate);
    } else {
        *stride_y = vpss_calc_lossless_cmp_ystride(width, dep);
    }
    return HI_SUCCESS;
}

hi_void vpss_comm_get_buffer_size(vpss_format_type typ, hi_u32 stride_y, hi_u32 height,
                                  hi_u32 *stride_c, hi_u32 *size)
{
    if (typ != VPSS_FORMAT_SP444) {
        *stride_c = stride_y;

        if (typ == VPSS_FORMAT_SP420 || typ == VPSS_FORMAT_SP420_CMP) {
            *size = height * stride_y * 3 / 2;  // 3 /2 * stride * h when 420
        } else if (typ == VPSS_FORMAT_SP422 || typ == VPSS_FORMAT_SP422_CMP) {
            *size = height * stride_y * 2;  // 2 * stride * h when 422
        } else if (typ == VPSS_FORMAT_RGB) {
            *size = height * stride_y;
        }
    } else {
        *stride_c = stride_y * 2;       // 2 used to cal
        *size = height * stride_y * 3;  // 3 used to cal buffer size when 444 format
    }
}

vpss_format_type vpss_comm_get_pix_format_type(hi_drv_pixel_format pixel_format, hi_drv_compress_info cmp_info)
{
    vpss_format_type type;

    switch (pixel_format) {
        case HI_DRV_PIXEL_FMT_NV12:
        case HI_DRV_PIXEL_FMT_NV21:
            /* tile or not : sp420 */
            type = VPSS_FORMAT_SP420;

            if (cmp_info.cmp_mode != HI_DRV_COMPRESS_MODE_OFF) {
                type = VPSS_FORMAT_SP420_CMP;
            }

            break;
        case HI_DRV_PIXEL_FMT_NV16:
        case HI_DRV_PIXEL_FMT_NV61:
        case HI_DRV_PIXEL_FMT_NV16_2X1:
        case HI_DRV_PIXEL_FMT_NV61_2X1:
            type = VPSS_FORMAT_SP422;

            if (cmp_info.cmp_mode != HI_DRV_COMPRESS_MODE_OFF) {
                type = VPSS_FORMAT_SP422_CMP;
            }

            break;
        case HI_DRV_PIXEL_FMT_ARGB8888:
        case HI_DRV_PIXEL_FMT_ABGR8888:
            type = VPSS_FORMAT_RGB;
            break;
        case HI_DRV_PIXEL_FMT_NV42:
            type = VPSS_FORMAT_SP444;
            break;
        default:
            type = VPSS_FORMAT_MAX;
            break;
    }

    return type;
}


hi_bool vpss_comm_is_loss_compress(hi_drv_compress_mode cmp_mode)
{
    if (cmp_mode == HI_DRV_COMPRESS_MODE_SEG_LOSS
        || cmp_mode == HI_DRV_COMPRESS_MODE_LINE_LOSS
        || cmp_mode == HI_DRV_COMPRESS_MODE_FRM_LOSS) {
        return HI_TRUE;
    }

    return HI_FALSE;
}


hi_s32 vpss_comm_cal_buf_size(vpss_buffer_attr *buf_attr, hi_drv_pixel_format pixel_format,
                              hi_drv_compress_info cmp_info, hi_drv_pixel_bitwidth out_bit_width,
                              hi_bool loss_cmp, hi_u32 cmp_rate)
{
    hi_u32 ret_size = 0;
    hi_u32 stride_y = 0;
    hi_u32 stride_c = 0;
    hi_u32 width, height;
    vpss_format_type format_bype = vpss_comm_get_pix_format_type(pixel_format, cmp_info);
    width = buf_attr->width;
    height = buf_attr->height;
    // we always malloc FHD buffer,because malloc function use too much time
#ifdef DTP
#ifndef HI_VPSS_DRV_USE_GOLDEN
    width = VPSS_WIDTH_ALLOC_BUFFER;
    height = VPSS_HEIGHT_ALLOC_BUFFER;
#endif
#endif

    switch (format_bype) {
        case VPSS_FORMAT_SP420:
        case VPSS_FORMAT_SP422:
            if (out_bit_width == HI_DRV_PIXEL_BITWIDTH_8BIT) {
                stride_y = vpss_align_8bit_ystride(width);
            } else {
                stride_y = vpss_align_10bit_comp_ystride(width);
            }

            break;
        case VPSS_FORMAT_SP420_CMP:
        case VPSS_FORMAT_SP422_CMP:
            vpss_comm_calc_cmp_y_stride(&stride_y, width, out_bit_width, loss_cmp, cmp_rate);
            break;
        case VPSS_FORMAT_RGB:
            stride_y = vpss_align_8bit_ystride(width * 4);  /* 4 used to cal */
            break;
        case VPSS_FORMAT_SP444:
            stride_y = vpss_align_8bit_ystride(width);
            break;
        default:
            buf_attr->buf_size = 0;
            buf_attr->stride_y = 0;
            buf_attr->stride_c = 0;
            vpss_error("unsupport pixel_format %d.\n", pixel_format);
            return HI_FAILURE;
    }

    vpss_comm_get_buffer_size(format_bype, stride_y, height, &stride_c, &ret_size);
    buf_attr->buf_size = ret_size;
    buf_attr->stride_y = stride_y;
    buf_attr->stride_c = stride_c;
    return HI_SUCCESS;
}

hi_u64 vpss_comm_get_sched_time(hi_void)
{
    return osal_sched_clock();
}

hi_void vpss_comm_notify_event(hi_drv_vpss_video_frame *frame, hi_stat_ld_event event_id)
{
    hi_stat_ld_event_info evt;
    hi_u32 tmp_time;
    tmp_time = hi_drv_sys_get_time_stamp_ms();
    evt.ld_event = event_id;
    evt.frame = frame->comm_frame.frame_index;
    /* todo */
#ifdef DPT
    evt.handle = frame->tunnel_src;
#endif
    evt.time = tmp_time;
    hi_drv_stat_ld_notify_event(&evt);
    return;
}

/* PQ operation */
hi_s32 vpss_comm_pq_update_timing_info(hi_u32 handle_no, hi_drv_pq_vpss_info *timing_info, hi_void *vpss_reg)
{
    hi_s32 ret;
    hi_drv_pq_vpss_scene_mode scence_mode;
#ifndef HI_VPSS_DRV_USE_PQ
    return HI_SUCCESS;
#endif

    ret = osal_exportfunc_get(HI_ID_PQ, (hi_void **)&g_pq_ext_func);
    if (ret != HI_SUCCESS) {
        vpss_error("osal_exportfunc_get HI_ID_PQ failed %08x \r\n", ret);
        return HI_FAILURE;
    }

    vpss_check_null(g_pq_ext_func);
    vpss_check_null(g_pq_ext_func->pq_init_vpss_alg);

    scence_mode.scene_mode = HI_DRV_PQ_SCENE_MODE_NORMAL;
    ret = g_pq_ext_func->pq_init_vpss_alg(scence_mode, timing_info, vpss_reg);

    return ret;
}

hi_s32 vpss_comm_pq_update_pq_reg(hi_drv_pq_vpss_stt_info *vpss_stt_info,
    hi_drv_pq_vpss_cfg_info *vpss_cfg_info, hi_void *vpss_reg)
{
#ifdef TOBEMODIFY
    hi_s32 ret;
#ifndef HI_VPSS_DRV_USE_PQ
    return HI_SUCCESS;
#endif

    ret = osal_exportfunc_get(HI_ID_PQ, (hi_void **)&g_pq_ext_func);
    if (ret != HI_SUCCESS) {
        vpss_error("osal_exportfunc_get HI_ID_PQ failed %08x \r\n", ret);
        return HI_FAILURE;
    }

    vpss_check_null(g_pq_ext_func);
    vpss_check_null(g_pq_ext_func->pq_update_vpss_alg);

    return g_pq_ext_func->pq_update_vpss_alg(vpss_stt_info, vpss_cfg_info);
#else
    return HI_SUCCESS;
#endif
}

hi_s32 vpss_comm_pq_run_alg(hi_drv_pq_vpss_stt_info *info_in, hi_drv_pq_vpss_cfg_info *info_out)
{
    hi_s32 ret;
#ifndef HI_VPSS_DRV_USE_PQ
    return HI_SUCCESS;
#endif

    ret = osal_exportfunc_get(HI_ID_PQ, (hi_void **)&g_pq_ext_func);
    if (ret != HI_SUCCESS) {
        vpss_error("osal_exportfunc_get HI_ID_PQ failed %08x \r\n", ret);
        return HI_FAILURE;
    }

    vpss_check_null(g_pq_ext_func);
    vpss_check_null(g_pq_ext_func->pq_update_vpss_alg);

    ret = g_pq_ext_func->pq_update_vpss_alg(info_in, info_out);

    return ret;
}

hi_s32 vpss_comm_pq_run_stt_alg(hi_drv_pq_vpss_stt_info *stt_info)
{
    hi_s32 ret;
#ifndef HI_VPSS_DRV_USE_PQ
    return HI_SUCCESS;
#endif

    ret = osal_exportfunc_get(HI_ID_PQ, (hi_void **)&g_pq_ext_func);
    if (ret != HI_SUCCESS) {
        vpss_error("osal_exportfunc_get HI_ID_PQ failed %08x \r\n", ret);
        return HI_FAILURE;
    }

    vpss_check_null(g_pq_ext_func);
    vpss_check_null(g_pq_ext_func->pq_update_vpss_stt_info);

    return g_pq_ext_func->pq_update_vpss_stt_info(stt_info); /* todo */
}

hi_s32 vpss_comm_pq_get_zme_coef(hi_drv_pq_vpss_layer layer_id,
    hi_drv_pq_vpsszme_in *zme_in, hi_drv_pq_vpsszme_out *zme_out)
{
    hi_s32 ret;
#ifndef HI_VPSS_DRV_USE_PQ
    return HI_SUCCESS;
#endif

    ret = osal_exportfunc_get(HI_ID_PQ, (hi_void **)&g_pq_ext_func);
    if (ret != HI_SUCCESS) {
        vpss_error("osal_exportfunc_get HI_ID_PQ failed %08x \r\n", ret);
        return HI_FAILURE;
    }

    vpss_check_null(g_pq_ext_func);
    vpss_check_null(g_pq_ext_func->pq_get_vpss_zme_coef);

    ret = g_pq_ext_func->pq_get_vpss_zme_coef(layer_id, zme_in, zme_out);

    return ret;
}

hi_s32 vpss_comm_pq_get_hdr_cfg(hi_drv_pq_vpss_layer layer_id,
    hi_drv_pq_xdr_frame_info *frame_info, hi_void *vpss_reg)
{
    hi_s32 ret;
#ifndef HI_VPSS_DRV_USE_PQ
    return HI_SUCCESS;
#endif

    ret = osal_exportfunc_get(HI_ID_PQ, (hi_void **)&g_pq_ext_func);
    if (ret != HI_SUCCESS) {
        vpss_error("hi_drv_module_get_func HI_ID_PQ failed %08x \r\n", ret);
        return HI_FAILURE;
    }

    vpss_check_null(g_pq_ext_func);
    vpss_check_null(g_pq_ext_func->pq_set_vpss_hdr_cfg);

    ret = g_pq_ext_func->pq_set_vpss_hdr_cfg(layer_id, frame_info, vpss_reg);

    return ret;
}

hi_s32 vpss_comm_increase_frame_refcount(hi_drv_video_frame *frame)
{
    return HI_SUCCESS;
}

hi_s32 vpss_comm_decrease_frame_refcount(hi_drv_video_frame *frame)
{
    return HI_SUCCESS;
}

hi_s32 vpss_comm_decrease_frame_refcount_privframe(hi_drv_vpss_video_frame *frame)
{
    return HI_SUCCESS;
}

hi_s32 vpss_get_frame_phy_addr(vpss_dma_para_out *phy_addr_info, hi_drv_vpss_video_frame *vpss_image,
                               hi_bool is_high_bit_set,  hi_bool is_head)
{
    drv_vpss_mem_info mem_info = {0};
    hi_s32 ret;

    ret = drv_vpss_mem_map_dma_handle(phy_addr_info->dma_handle, &mem_info);
    if (ret != HI_SUCCESS) {
        vpss_error("get phy addr failed! secure:%d dma_handle:0x%llx \n",
            vpss_image->comm_frame.secure, phy_addr_info->dma_handle);
        return ret;
    }

    if (is_high_bit_set == HI_TRUE) {
        if (is_head == HI_TRUE) {
            vpss_image->vpss_buf_addr[0].phy_addr_y_head = mem_info.phy_addr + phy_addr_info->addr_offset_y;
            vpss_image->vpss_buf_addr[0].phy_addr_c_head = mem_info.phy_addr + phy_addr_info->addr_offset_c;
        } else {
            vpss_image->vpss_buf_addr[0].phy_addr_y = mem_info.phy_addr + phy_addr_info->addr_offset_y;
            vpss_image->vpss_buf_addr[0].vir_addr_y = (hi_u64)mem_info.vir_addr + phy_addr_info->addr_offset_y;
            vpss_image->vpss_buf_addr[0].phy_addr_c = mem_info.phy_addr + phy_addr_info->addr_offset_c;
            vpss_image->vpss_buf_addr[0].vir_addr_c = (hi_u64)mem_info.vir_addr + phy_addr_info->addr_offset_c;
        }
    } else {
        vpss_image->vpss_buf_addr_lb[0].phy_addr_y = mem_info.phy_addr + phy_addr_info->addr_offset_y;
        vpss_image->vpss_buf_addr_lb[0].vir_addr_y = (hi_u64)mem_info.vir_addr + phy_addr_info->addr_offset_y;
        vpss_image->vpss_buf_addr_lb[0].phy_addr_c = mem_info.phy_addr + phy_addr_info->addr_offset_c;
        vpss_image->vpss_buf_addr_lb[0].vir_addr_c = (hi_u64)mem_info.vir_addr + phy_addr_info->addr_offset_c;
    }

    return ret;
}

static hi_s32 vpss_cvt_dma_to_phy(hi_drv_video_frame *comm_image, hi_drv_vpss_video_frame *vpss_image)
{
    hi_s32 ret = HI_SUCCESS;
    vpss_dma_para_out phy_addr_info = { 0 };
    hi_bool is_high_bit_set = HI_FALSE;
    hi_bool is_head = HI_FALSE;

    if (comm_image->buf_addr[0].dma_handle != 0) {
        phy_addr_info.dma_handle = (struct dma_buf *)comm_image->buf_addr[0].dma_handle;

        /* high 8bit */
        phy_addr_info.addr_offset_y    = comm_image->buf_addr[0].y_offset;
        phy_addr_info.addr_offset_c    = comm_image->buf_addr[0].c_offset;

        vpss_image->vpss_buf_addr[0].stride_y = comm_image->buf_addr[0].stride_y;
        vpss_image->vpss_buf_addr[0].stride_c = comm_image->buf_addr[0].stride_c;
        vpss_image->vpss_buf_addr[0].stride_cr = comm_image->buf_addr[0].stride_cr;
        is_high_bit_set = HI_TRUE;
        is_head = HI_FALSE;
        ret = vpss_get_frame_phy_addr(&phy_addr_info, vpss_image, is_high_bit_set, is_head);
        if (ret != HI_SUCCESS) {
            vpss_error(" get phy addr fail \n");
            return HI_FAILURE;
        }

        /* head */
        phy_addr_info.addr_offset_y    = comm_image->buf_addr[0].y_head_offset;
        phy_addr_info.addr_offset_c    = comm_image->buf_addr[0].c_head_offset;

        vpss_image->vpss_buf_addr[0].head_stride = comm_image->buf_addr[0].head_stride;
        vpss_image->vpss_buf_addr[0].head_size   = comm_image->buf_addr[0].head_size;
        is_high_bit_set = HI_TRUE;
        is_head = HI_TRUE;
        ret = vpss_get_frame_phy_addr(&phy_addr_info, vpss_image, is_high_bit_set, is_head);
        if (ret != HI_SUCCESS) {
            vpss_error(" get head phy addr fail \n");
            return HI_FAILURE;
        }
    }

    if (comm_image->buf_addr_lb[0].dma_handle != 0) {
        phy_addr_info.dma_handle = (struct dma_buf *)comm_image->buf_addr_lb[0].dma_handle;

        phy_addr_info.addr_offset_y  = comm_image->buf_addr_lb[0].y_offset;
        phy_addr_info.addr_offset_c  = comm_image->buf_addr_lb[0].c_offset;

        vpss_image->vpss_buf_addr_lb[0].stride_y = comm_image->buf_addr_lb[0].stride_y;
        vpss_image->vpss_buf_addr_lb[0].stride_c = comm_image->buf_addr_lb[0].stride_c;
        vpss_image->vpss_buf_addr_lb[0].stride_cr = comm_image->buf_addr_lb[0].stride_cr;

        is_high_bit_set = HI_FALSE;
        is_head = HI_FALSE;
        ret = vpss_get_frame_phy_addr(&phy_addr_info, vpss_image, is_high_bit_set, is_head);
        if (ret != HI_SUCCESS) {
            vpss_error(" get lb phy addr fail \n");
            return HI_FAILURE;
        }
    }

    return ret;
}

hi_s32 vpss_comm_map_priv_video(hi_drv_video_frame *comm_image, hi_drv_vpss_video_frame *vpss_image)
{
    hi_s32 ret;

    ret = vpss_cvt_dma_to_phy(comm_image, vpss_image);
    if (ret != HI_SUCCESS) {
        vpss_error("vpss_cvt_dma_to_phy failed\n");
        return ret;
    }

    return ret;
}

hi_s32 vpss_comm_umap_priv_video(hi_drv_vpss_video_frame *vpss_image)
{
    hi_s32 ret = HI_SUCCESS;
    vpss_dma_para_umap dma_info = { 0 };

    if (vpss_image->comm_frame.buf_addr[0].dma_handle != 0) {
        dma_info.dma_handle = (struct dma_buf *)vpss_image->comm_frame.buf_addr[0].dma_handle;
        dma_info.phy_addr = (dma_addr_t)vpss_image->vpss_buf_addr[0].phy_addr_y_head;
        dma_info.vir_addr = vpss_image->vpss_buf_addr[0].vir_addr_y;
        ret = drv_vpss_mem_unmap_dma_handle(&dma_info);
        if (ret != HI_SUCCESS) {
            vpss_error(" umap phy addr fail \n");
            return HI_FAILURE;
        }
    }

    if (vpss_image->comm_frame.buf_addr_lb[0].dma_handle != 0) {
        dma_info.dma_handle = (struct dma_buf *)vpss_image->comm_frame.buf_addr_lb[0].dma_handle;
        dma_info.phy_addr = (dma_addr_t)vpss_image->vpss_buf_addr_lb[0].phy_addr_y_head;
        dma_info.vir_addr = vpss_image->vpss_buf_addr_lb[0].vir_addr_y;
        ret = drv_vpss_mem_unmap_dma_handle(&dma_info);
        if (ret != HI_SUCCESS) {
            vpss_error(" umap lb phy addr fail \n");
            return HI_FAILURE;
        }
    }

    return ret;
}

hi_void vpss_comm_cvt_comm_to_privite_nomap(hi_drv_video_frame *comm_image, hi_drv_vpss_video_frame *vpss_image)
{
    memcpy(vpss_image, comm_image, sizeof(hi_drv_video_frame));

    return;
}

hi_void vpss_comm_cvt_privite_to_comm_nomap(hi_drv_vpss_video_frame *vpss_image, hi_drv_video_frame *comm_image)
{
    memcpy(comm_image, vpss_image, sizeof(hi_drv_video_frame));

    return;
}

hi_void vpss_comm_copy_comm_win_meta_to_vpss_meta(hi_drv_video_frame *comm_image)
{
    comm_image->video_private.vpss_private.size = comm_image->video_private.win_private.size;
    memcpy((hi_drv_win_vpss_meta_info *)comm_image->video_private.vpss_private.data,
           (hi_drv_win_vpss_meta_info *)comm_image->video_private.win_private.data,
           comm_image->video_private.win_private.size);

    return;
}

hi_void vpss_comm_copy_comm_vpss_meta_to_comm_meta(hi_drv_video_frame *comm_image)
{
    comm_image->video_private.win_private.size = comm_image->video_private.vpss_private.size;
    memcpy((hi_drv_win_vpss_meta_info *)comm_image->video_private.win_private.data,
           (hi_drv_win_vpss_meta_info *)comm_image->video_private.vpss_private.data,
           comm_image->video_private.vpss_private.size);

    return;
}

hi_void vpss_comm_copy_priv_win_meta_to_vpss_meta(hi_drv_vpss_video_frame *vpss_image)
{
    vpss_image->comm_frame.video_private.vpss_private.size = vpss_image->comm_frame.video_private.win_private.size;
    memcpy((hi_drv_win_vpss_meta_info *)vpss_image->comm_frame.video_private.vpss_private.data,
           (hi_drv_win_vpss_meta_info *)vpss_image->comm_frame.video_private.win_private.data,
           vpss_image->comm_frame.video_private.win_private.size);

    return;
}

hi_void vpss_comm_copy_priv_vpss_meta_to_win_meta(hi_drv_vpss_video_frame *vpss_image)
{
    vpss_image->comm_frame.video_private.win_private.size = vpss_image->comm_frame.video_private.vpss_private.size;
    memcpy((hi_drv_win_vpss_meta_info *)vpss_image->comm_frame.video_private.win_private.data,
           (hi_drv_win_vpss_meta_info *)vpss_image->comm_frame.video_private.vpss_private.data,
           vpss_image->comm_frame.video_private.vpss_private.size);

    return;
}

hi_s32 vpss_comm_attach_buf(hi_bool secure, hi_drv_ssm_buf_attach_info attach_info, hi_u64 *sec_info_addr)
{
    if (secure != HI_TRUE) {
        return HI_SUCCESS;
    }

#ifdef HI_TEE_SUPPORT
    {
        hi_s32 ret;

        ret = hi_drv_ssm_attach_buffer(attach_info, sec_info_addr);
        if (ret != HI_SUCCESS) {
            vpss_error("attach buf failed ! dma_handle:0x%llx type:%d \n",
                (hi_u64)attach_info.dma_buf_addr, attach_info.module_handle);
            return HI_FAILURE;
        }
    }
#endif

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

