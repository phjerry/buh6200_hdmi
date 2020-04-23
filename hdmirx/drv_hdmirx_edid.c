/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Implementation of edid functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#include "drv_hdmirx_edid.h"
#include "drv_hdmirx_common.h"
#include <securec.h>

static hdmirx_edid_context g_hdmirx_edid_ctx[HDMIRX_EDID_MAX];

static hi_s32 hdmirx_drv_edid_moni_name(hdmirx_edid_context *edid_ctx, hi_u8 *edid_data, hi_u32 len)
{
    hi_u32 i;
    errno_t err_ret;
    if ((len != MONITORNAME_LEN) || (edid_data == HI_NULL)) {
        hi_err_hdmirx("moni_name fail , len erro!\n");
        return HI_FAILURE;
    }
    err_ret = memset_s(edid_ctx->monitor_name, sizeof(edid_ctx->monitor_name), 0, MONITORNAME_LEN + 1);
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return err_ret;
    }

    for (i = 0; i < MONITORNAME_LEN; i++) {
        if ((edid_data[i] != 0x20) && (edid_data[i] != 0x0A)) {
            edid_ctx->monitor_name[i] = (hi_u8)edid_data[i];
        } else {
            return HI_SUCCESS;
        }
    }
    return HI_SUCCESS;
}

/* 此为前128 字节中 descriptors 的信息 54- 125 */
/* 目前只增加识别monitor name 功能 */
static hi_s32 hdmirx_drv_edid_descriptors(hdmirx_edid_context *edid_ctx, hi_u8 *edid_data, hi_u32 len)
{
    hi_u32 i;
    hi_u8 mon_name_head[] = { 0x0, 0x0, 0x0, 0xfc, 0x0 };
    hi_u32 descriptor_per_len = DESCRIPTOR_PER_LEN;
    if ((len != DESCRIPTOR_LEN) || (edid_data == HI_NULL)) { /* 72: input len must be 72 */
        hi_err_hdmirx("descriptors fail!\n");
        return HI_FAILURE;
    }
    for (i = 0; i < len; i += descriptor_per_len) {
        if (osal_memncmp(edid_data + i, MONITORHEAD_LEN * sizeof(hi_u8), mon_name_head,
            MONITORHEAD_LEN * sizeof(hi_u8)) == 0) { /* 5: compare only 5 * 4 bytes */
            hdmirx_drv_edid_moni_name(edid_ctx, edid_data + i + MONITORHEAD_LEN, descriptor_per_len - MONITORHEAD_LEN);
        }
    }

    return HI_SUCCESS;
}

static hi_s32 hdmirx_drv_edid_manufac_name(hdmirx_edid_context *edid_ctx, hi_u8 *edid_data, hi_u32 len)
{
    hi_u8 tmp;
    errno_t err_ret;
    if ((len != 2) || (edid_data == HI_NULL)) { /* input len must be 2 */
        hi_err_hdmirx("manufac_name fail!");
        return HI_FAILURE;
    }
    err_ret = memset_s(edid_ctx->manufac_name, sizeof(edid_ctx->manufac_name), 0, MANUFAC_NAME + 1);
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return err_ret;
    }
    tmp = ((edid_data[0] & 0xff) << 8) + (edid_data[1] & 0xff);       /* remove high 8 bits */
    edid_ctx->manufac_name[2] = (hi_u8)((tmp & 0x1F) + 0x40);         /* 2: set 2st manufac_name value */
    edid_ctx->manufac_name[1] = (hi_u8)(((tmp >> 5) & 0x1F) + 0x40);  /* remove low 5 bits */
    edid_ctx->manufac_name[0] = (hi_u8)(((tmp >> 10) & 0x1F) + 0x40); /* remove low 10 bits */
    return HI_SUCCESS;
}

/* 8-17 为此产品信息 */
/* 目前只识别 制造商 功能 */
static hi_s32 hdmirx_drv_edid_product_info(hdmirx_edid_context *edid_ctx, hi_u8 *edid_data, hi_u32 len)
{
    hi_s32 ret;
    if ((len != PRODUCT_INFO_LEN) || (edid_data == HI_NULL)) {
        hi_err_hdmirx("product_info fail!\n");
        return HI_FAILURE;
    }
    ret = hdmirx_drv_edid_manufac_name(edid_ctx, edid_data, 2); /* 2: input name length */
    if (ret != HI_SUCCESS) {
        hi_err_hdmirx("hdmirx get manufac_name failure.\n");
    }
    return HI_SUCCESS;
}

static hi_s32 hdmirx_drv_edid_cea_audio(hdmirx_edid_context *edid_ctx, hi_u8 *edid_data, hi_u32 len)
{
    hi_u32 tmp, i;
    if ((len % 3 != 0) || (edid_data == HI_NULL) || (len > EDID_LENGTH)) { /* input length must be div by 3 */
        hi_err_hdmirx("audio EDID erro!!!\n");
        return HI_FAILURE;
    }

    tmp = 0;
    for (i = 0; i < len; i += 3) {                         /* 3: index add 3 every time */
        if (((edid_data[i] >> 3) & 0xf) > 0) {             /* 3: remove low 3 bits */
            tmp |= 1 << (((edid_data[i] >> 3) & 0xf) - 1); /* 3: remove low 3 bits */
        }
    }

    edid_ctx->audio_data = tmp;
    return HI_SUCCESS;
}

hi_void hdmirx_drv_edid_cea_process(hdmirx_edid_type edid_type, hi_u32 cea_data_len, hi_u8 *edid_ext)
{
    hi_s32 i;
    if (edid_ext == HI_NULL) {
        hi_err_hdmirx("input ptr is null!\n");
        return;
    }
    for (i = 0; i < cea_data_len;) {
        switch ((edid_ext[i] >> 5)) { /* 5: remove low 5 bits, start to check the value */
            case HDMIRX_EDID_CEA_TAG_AUDIO:
                if ((i + 1 + (edid_ext[i] & 0x1F)) < 124) { /* 124: value check threshold */
                    hdmirx_drv_edid_cea_audio(&g_hdmirx_edid_ctx[edid_type], \
                        edid_ext + i + 1, (edid_ext[i] & 0x1F));
                }
                break;
            case HDMIRX_EDID_CEA_TAG_VIDEO:

                break;
            case HDMIRX_EDID_CEA_TAG_VSDB:

                break;
            case HDMIRX_EDID_CEA_TAG_SPEAK:

                break;
            case HDMIRX_EDID_CEA_TAG_VESA:

                break;
            case HDMIRX_EDID_CEA_TAG_EXT:

                break;
            default:
                break;
        }
        i = i + 1 + (edid_ext[i] & 0x1F);
    }
    g_hdmirx_edid_ctx[edid_type].active = HI_TRUE;
}
hi_void hdmirx_drv_process_edid(hdmirx_edid_type edid_type, hi_u8 *edid_data)
{
    errno_t err_ret;
    hi_u32 ret;
    hi_u8 *edid_ext = HI_NULL;
    hi_u32 cea_data_len;
    err_ret = memset_s(&g_hdmirx_edid_ctx[edid_type], sizeof(hdmirx_edid_context), 0, sizeof(hdmirx_edid_context));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        osal_kfree(HI_ID_HDMIRX, edid_data);
        return;
    }
    ret = hdmirx_drv_edid_product_info(&g_hdmirx_edid_ctx[edid_type],
        edid_data + PRODUCT_INFO_OFFSET, PRODUCT_INFO_LEN);
    if (ret != HI_SUCCESS) {
        hi_err_hdmirx("hdmirx get product info failure.\n");
    }
    ret = hdmirx_drv_edid_descriptors(&g_hdmirx_edid_ctx[edid_type],
        edid_data + DESCRIPTOR_OFFSET, DESCRIPTOR_LEN);
    if (ret != HI_SUCCESS) {
        hi_err_hdmirx("hdmirx get descriptors failure.\n");
    }

    /* 后 128 字节数据分析 */
    /* 数据从 第四个开始 */
    edid_ext = edid_data + 128 + 4; /* start from end of 128st byte, skip front 4 byte head */
    cea_data_len = *(edid_data + 2 + 128) - 4; /* 2 + 128 : 131st byte means cea data len, remove front 4 bytes */
    if (cea_data_len > 124) { /* cea len bigger than 124 is invalid */
        hi_err_hdmirx("EDID CEA len erro !\n");
        return;
    }
    hdmirx_drv_edid_cea_process(edid_type, cea_data_len, edid_ext);
}

hi_s32 hdmirx_drv_edid_register(hdmirx_edid_type edid_type, hi_u8 *edid_data_in, hi_u32 edid_length)
{
    hi_u8 edid_head[] = { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 };
    hi_u8 *edid_data = HI_NULL;
    errno_t err_ret;
    if (edid_type >= HDMIRX_EDID_MAX) {
        hi_err_hdmirx("EDID type erro!!!\n");
        return HI_FAILURE;
    }
    if (edid_length != EDID_LENGTH) {
        hi_err_hdmirx("EDID length erro!!!\n");
        return HI_FAILURE;
    }
    if (edid_data_in == HI_NULL) {
        hi_err_hdmirx("EDID data is null!!!\n");
        return HI_FAILURE;
    }
    if (osal_memncmp(edid_data_in, EDID_HEAD_LENGTH * sizeof(hi_u8),
        edid_head, EDID_HEAD_LENGTH * sizeof(hi_u8)) != 0) { /* 8: compare front 8 * 4 bits */
        hi_err_hdmirx("register fail !\n");
        return HI_FAILURE;
    }
    edid_data = (hi_u8*)osal_kmalloc(HI_ID_HDMIRX, HDMIRX_EDID_LENGTH * sizeof(hi_u8),
        OSAL_GFP_KERNEL); /* 256: array size */
    if (edid_data == HI_NULL) {
        hi_err_hdmirx("kmalloc failed!\n");
        return HI_FAILURE;
    }
    err_ret = memset_s(edid_data, HDMIRX_EDID_LENGTH * sizeof(hi_u8), 0, HDMIRX_EDID_LENGTH * sizeof(hi_u8));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        osal_kfree(HI_ID_HDMIRX, edid_data);
        return err_ret;
    }
    err_ret = memcpy_s(edid_data, HDMIRX_EDID_LENGTH * sizeof(hi_u8),
        edid_data_in, HDMIRX_EDID_LENGTH * sizeof(hi_u8));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        osal_kfree(HI_ID_HDMIRX, edid_data);
        return err_ret;
    }
    hdmirx_drv_process_edid(edid_type, edid_data);
    osal_kfree(HI_ID_HDMIRX, edid_data);
    return HI_SUCCESS;
}

hi_s32 hdmirx_drv_edid_log(hdmirx_edid_type edid_type)
{
    hdmirx_edid_context edid_ctx;

    if (edid_type >= HDMIRX_EDID_MAX) {
        hi_err_hdmirx("edid_type ERROR!!");
        return HI_FAILURE;
    }

    edid_ctx = g_hdmirx_edid_ctx[edid_type];
    if (edid_ctx.active == HI_FALSE) {
        osal_printk("no EDID DATA!\n");
        return HI_SUCCESS;
    }
    osal_printk("%s EDID info:\n", (edid_type == HDMIRX_EDID_MHL) ? "MHL" : "HDMI");
    osal_printk("manu_facturer name :%s\n", edid_ctx.manufac_name);
    osal_printk("monitor name      :%s\n", edid_ctx.monitor_name);
    osal_printk("pcm support: %s\n", (edid_ctx.audio_data & b_pcm_support) ? "yes" : "no");
    osal_printk("ac3 support: %s\n", (edid_ctx.audio_data & b_ac3_support) ? "yes" : "no");
    osal_printk("dts support: %s\n", (edid_ctx.audio_data & b_dts_support) ? "yes" : "no");
    osal_printk("DD+ support: %s\n", (edid_ctx.audio_data & b_e_ac3_support) ? "yes" : "no");
    return HI_SUCCESS;
}

hi_s32 hdmirx_drv_edid_get_info(hdmirx_edid_type edid_type, hdmirx_edid_context *edid_ctx)
{
    if (edid_type >= HDMIRX_EDID_MAX) {
        hi_err_hdmirx("EDID type erro!!!\n");
        return HI_FAILURE;
    }
    *edid_ctx = g_hdmirx_edid_ctx[edid_type];
    if (edid_ctx->active == HI_FALSE) {
        return HI_FAILURE;
    } else {
        return HI_SUCCESS;
    }
}

hi_s32 hdmirx_drv_edid_get_edid_type(hi_u8 *edid_data, hdmirx_edid_type *edid_type)
{
    hi_u8 *edid_ext = HI_NULL;
    hi_u32 i, cea_data_len;

    if ((edid_data == NULL) || (edid_type == NULL)) {
        hi_err_hdmirx("EDID type/data erro!!!\n");
        return HI_FAILURE;
    }

    *edid_type = HDMIRX_EDID_14;
    /* 后 128 字节数据分析 */
    /* 数据从 第四个开始 */
    edid_ext = edid_data + 128 + 4; /* 128: addr offset, this pos start analyze, skip front 4 bytes */
    cea_data_len = *(edid_data + 2 + 128) - 4; /* 2 + 128 : 131st byte means cea data len, remove front 4 bytes */
    if (cea_data_len > 124) { /* cea len bigger than 124 is invalid */
        hi_err_hdmirx("EDID CEA len erro !\n");
        return HI_FAILURE;
    }
    for (i = 0; i < cea_data_len;) {
        switch ((edid_ext[i] >> 5)) { /* remove low 5 bits */
            case HDMIRX_EDID_CEA_TAG_AUDIO:

                break;
            case HDMIRX_EDID_CEA_TAG_VIDEO:

                break;
            case HDMIRX_EDID_CEA_TAG_VSDB:
                if (((i + 1) < 124) && (edid_ext[i + 1] == 0xD8)) { /* array index is little than 124 */
                    hi_dbg_hdmirx_print_u32(edid_ext[i + 1]);
                    hi_dbg_hdmirx("2.0 EDID!!\n");
                    *edid_type = HDMIRX_EDID_20;
                }
                break;
            case HDMIRX_EDID_CEA_TAG_SPEAK:

                break;
            case HDMIRX_EDID_CEA_TAG_VESA:

                break;
            case HDMIRX_EDID_CEA_TAG_EXT:

                break;
            default:
                break;
        }
        i = i + 1 + (edid_ext[i] & 0x1F);
    }

    return HI_SUCCESS;
}
