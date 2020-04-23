/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "dbg.h"
#include "vfmw_pdt.h"
#include "vfmw_define.h"

#define TMP_MSG_SIZE 1024

#define TRACE_MEM_SIZE    (16 * 1024 * 2)
#define MAX_FUNC_NAME_LEN 15

#define VALID_FLAG_0 0x1a2a3a4a
#define VALID_FLAG_1 0x55657585
#define VALID_FLAG_2 0x0f152f35
#define VALID_FLAG_3 0x4a5f6a7f

typedef struct {
    hi_char func[MAX_FUNC_NAME_LEN + 1];
    hi_s32 line;
    hi_s32 data;
    hi_u32 time;
} trace_obj;

typedef struct {
    hi_s32 max_obj_num_minus4;
    hi_s32 obj_num;
    hi_u32 valid_flag[4]; /* 4 :a number */
    UADDR trace_phy;
    trace_obj *first_obj;
} trace_entity;

hi_u32 g_print_word = (1 << PRN_FATAL);
print_dev g_print_out = PRN_DEV_COM;

static trace_entity *g_tracer = HI_NULL;
static UADDR g_tracer_phy = 0;

hi_s32 str_to_val(hi_char *str, hi_u32 *data)
{
    hi_u32 i, d, dat, weight;

    dat = 0;

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        i = 2; /* 0x start need skip 2 chars */
        weight = 16; /* 0x start is hex so weight is 16 */
    } else {
        i = 0;
        weight = 10; /* none start is decimal so weight is 10 */
    }

    for (; i < 10; i++) { /* up to max bit hex has 10 chars a hexnum total */
        if (str[i] < 0x20) { /* 0x20 is uplimit of invalid char */
            break;
        } else if (weight == 16 && str[i] >= 'a' && str[i] <= 'f') { /* 16 is hex */
            d = str[i] - 'a' + 10; /* 'a' means 10 */
        } else if (weight == 16 && str[i] >= 'A' && str[i] <= 'F') { /* 16 is hex */
            d = str[i] - 'A' + 10; /* 'A' means 10 */
        } else if (str[i] >= '0' && str[i] <= '9') {
            d = str[i] - '0';
        } else {
            return -1; /* invalid num return -1 means Fail */
        }

        dat = dat * weight + d;
    }

    *data = dat;

    return 0;
}

hi_s32 dbg_create_tracer(hi_void)
{
    hi_s32 ret;
    vfmw_mem_desc tmp_desc;

    if (g_tracer) {
        return VFMW_DBG_OK;
    }

#ifdef VFMW_MMU_SUPPORT
    tmp_desc.mode = MEM_MMU_MMU;
#else
    tmp_desc.mode = MEM_CMA_CMA;
#endif

    ret = OS_KMEM_ALLOC("VFMW_Tracer", TRACE_MEM_SIZE, 16, 0, &tmp_desc); /* 16 :a number */
    if (ret != OSAL_OK) {
        return VFMW_DBG_ERR;
    }

    g_tracer_phy = tmp_desc.phy_addr;
    g_tracer = UINT64_PTR(tmp_desc.vir_addr);

    VFMW_CHECK_SEC_FUNC(memset_s(UINT64_PTR(tmp_desc.vir_addr), TRACE_MEM_SIZE, 0, TRACE_MEM_SIZE));

    g_tracer->first_obj = UINT64_PTR(tmp_desc.vir_addr + sizeof(trace_entity));
    g_tracer->trace_phy = (g_tracer_phy + sizeof(trace_entity));
    g_tracer->max_obj_num_minus4 = (tmp_desc.length - sizeof(trace_entity)) / sizeof(trace_obj) - 4; /* 4 :a number */
    g_tracer->obj_num = 0;

    g_tracer->valid_flag[0] = VALID_FLAG_0;
    g_tracer->valid_flag[1] = VALID_FLAG_1;
    g_tracer->valid_flag[2] = VALID_FLAG_2; /* 2 :a number */
    g_tracer->valid_flag[3] = VALID_FLAG_3; /* 3 :a number */

    dprint(PRN_ALWS, "==== mem_phy, trace_phy, trace_max = 0x%x, 0x%x,%d ====\n",
           g_tracer_phy, g_tracer->trace_phy, g_tracer->max_obj_num_minus4);

    return VFMW_DBG_OK;
}

hi_void dbg_destroy_tracer(hi_void)
{
    vfmw_mem_desc tmp_desc;

    if (g_tracer == HI_NULL) {
        return;
    }

    if (g_tracer_phy == 0) {
        dprint(PRN_ERROR, "%s, Phyaddr == 0\n", __func__);
        return;
    }

    VFMW_CHECK_SEC_FUNC(memset_s(&tmp_desc, sizeof(tmp_desc), 0, sizeof(tmp_desc)));
    tmp_desc.phy_addr = g_tracer_phy;
    tmp_desc.vir_addr = PTR_UINT64(g_tracer);
    tmp_desc.length = TRACE_MEM_SIZE;

    OS_KMEM_FREE(&tmp_desc);
    dprint(PRN_ALWS, "===== destroy tracer: phy, vir = 0x%x,%p =====\n", g_tracer_phy, g_tracer);

    g_tracer = HI_NULL;

    return;
}

hi_void dbg_add_trace(hi_char *trace_string, hi_s32 trace_line, hi_s32 trace_data)
{
    trace_obj *obj = HI_NULL;

    if (g_tracer == HI_NULL) {
        return;
    }

    obj = g_tracer->first_obj + (g_tracer->obj_num % g_tracer->max_obj_num_minus4);
    g_tracer->obj_num++;

    obj->line = trace_line;
    obj->data = trace_data;

    obj->time = OS_GET_TIME_US();
    OS_STRLCPY(obj->func, trace_string, MAX_FUNC_NAME_LEN);

    return;
}

hi_void dbg_print_tracer(hi_void)
{
    hi_s32 i;
    trace_entity *tracer = HI_NULL;
    hi_u32 *valid_flag = HI_NULL;
    hi_s32 obj_num;
    trace_obj *obj = HI_NULL;

    if (g_tracer_phy == 0) {
        dprint(PRN_ALWS, "g_tracer_phy is invalid\n", g_tracer_phy);
        return;
    }

    if (g_tracer == HI_NULL) {
        tracer = (trace_entity *)OS_IOREMAP(g_tracer_phy, TRACE_MEM_SIZE);
        if (tracer == HI_NULL) {
            dprint(PRN_ALWS, "map vir address for '%x' failed!!!\n", g_tracer_phy);
            return;
        }

        tracer->first_obj = (trace_obj *)((hi_u8 *)tracer + sizeof(trace_entity));
        dprint(PRN_ALWS, "tracer memory '0x%x' is mapped to '%p'\n", g_tracer_phy, tracer);
    } else {
        tracer = g_tracer;
    }

    if (tracer == HI_NULL) {
        dprint(PRN_ALWS, "map vir address for '%x' failed!!!\n", g_tracer_phy);
        return;
    }

    valid_flag = &tracer->valid_flag[0];

    if (valid_flag[0] != VALID_FLAG_0 || valid_flag[1] != VALID_FLAG_1 ||
        valid_flag[2] != VALID_FLAG_2 || valid_flag[3] != VALID_FLAG_3) { /* 2 :a number 3 :a number */
        dprint(PRN_ALWS, "no valid trace info stored in '%x'!!!\n", g_tracer_phy);
        return;
    }

    dprint(PRN_ALWS, "====================== tracer context =====================\n");
    dprint(PRN_ALWS, "%-25s :%x\n", "first_obj", tracer->first_obj);
    dprint(PRN_ALWS, "%-25s :%p\n", "trace_phy", tracer->trace_phy);
    dprint(PRN_ALWS, "%-25s :%d\n", "max_obj_num_minus4", tracer->max_obj_num_minus4);
    dprint(PRN_ALWS, "%-25s :%d\n", "obj_num", tracer->obj_num);

    /* only print those msg we added */
    obj_num = tracer->obj_num;
    for (i = 0; i < tracer->max_obj_num_minus4 && i < tracer->obj_num; i++) {
        obj_num = (obj_num - 1 + tracer->max_obj_num_minus4) % tracer->max_obj_num_minus4;
        obj = tracer->first_obj + obj_num;

        dprint(PRN_ALWS, "-%05d-", i);
        dprint(PRN_ALWS, "func:%-15s;", obj->func);
        dprint(PRN_ALWS, " line:%5d;", obj->line);
        dprint(PRN_ALWS, " data:0x%08x(%-10d);", obj->data, obj->data);
        dprint(PRN_ALWS, " time:%u\n", obj->time);
    }

    if (g_tracer == HI_NULL) {
        OS_IOUNMAP(tracer);
    }

    return;
}

STATIC hi_s32 dbg_save_linear(hi_s32 chan_id, vfmw_image *image, OS_FILE *save_file)
{
    hi_s32 len;
    hi_u8 *tmp_mem = HI_NULL;
    hi_s32 width;
    hi_s32 height;
    hi_s32 bit_depth;
    hi_s32 luma_size;
    hi_s32 stride;
    hi_s32 pix_byte;

    width = image->image_width;
    height = image->image_height;
    stride = image->disp_info.y_stride;
    bit_depth = image->bit_depth;
    pix_byte = (bit_depth > 8) ? 2 : 1; /* 8: default 8bit, 2: 2 bytes per pixel, 1: 1 byte per pixel */
    luma_size = width * height * pix_byte;

    dprint(PRN_ALWS, "save linear width %d, height %d, bit_depth %d, stride %d.\n", width, height, bit_depth, stride);

    tmp_mem = OS_ALLOC_VIR("SaveYuv_TMP", luma_size * 3 / 2); /* 3: y/u/v, 2: uv half of y size */
    if (tmp_mem == HI_NULL) {
        dprint(PRN_ALWS, "AllocVirMem Fail\n");
        return VFMW_DBG_ERR;
    }

    pdt_yuv_2d(image, tmp_mem);

    len = OS_FWRITE(tmp_mem, luma_size * 3 / 2, save_file); /* 3 :a number 2 :a number */
    if (len < luma_size * 3 / 2) { /* 3 :a number 2 :a number */
        dprint(PRN_ALWS, "%s write yuv data failed.\n", __func__);
    }

    OS_FREE_VIR(tmp_mem);

    return VFMW_DBG_OK;
}

STATIC hi_s32 dbg_save_compress(hi_s32 chan_id, vfmw_image *image, OS_FILE *save_file)
{
    hi_u8 *vaddr = HI_NULL;
    hi_u32 size;
    hi_u16 slot_wid;
    hi_u16 slot_hig;
    hi_u8 uv_order = 0;
    hi_u8 pic_struct = 0;
    hi_u8 chroma_idc = 1;
    hi_u8 head[32] = { 0 }; /* 32 :a number */
    hi_u64 chrom_vir_addr;
    hi_u64 luma_2bit_vir_addr = 0;
    hi_u64 chrom_2bit_vir_addr = 0;
    hi_u32 allign_h = 16; /* 16 is allign */

    dprint(PRN_ALWS, "SaveCmp width %d, height %d, bit_depth %d.\n",
           image->image_width, image->image_height, image->bit_depth);

    if (image->vid_std == VFMW_H264 ||
        image->vid_std == VFMW_AVS ||
        image->vid_std == VFMW_VC1 ||
        image->vid_std == VFMW_MPEG2) {
        allign_h = 32; /* 32 h align */
    }

    slot_wid = image->disp_info.y_stride;
    slot_hig = VFMW_ALIGN_UP(image->image_height, allign_h); /* 16 is allign */

    *(hi_u16 *)(head + 0) = image->image_width;
    *(hi_u16 *)(head + 2) = image->image_height; /* 2 :a number */
    *(hi_u16 *)(head + 4) = slot_wid; /* 4 :a number */
    *(hi_u16 *)(head + 6) = slot_hig; /* 6 :a number */
    *(hi_u32 *)(head + 8) = image->disp_info.y_stride * 16; /* 8 :a number 16 :a number */
    *(hi_u8 *)(head + 12) = pic_struct; /* 12 :a number */
    *(hi_u8 *)(head + 13) = chroma_idc; /* 13 :a number */
    *(hi_u8 *)(head + 14) = image->bit_depth; /* 14 :a number */
    *(hi_u8 *)(head + 15) = image->bit_depth; /* 15 :a number */
    *(hi_u8 *)(head + 16) = image->disp_info.compress_en; /* 16 :a number */
    *(hi_u8 *)(head + 18) = uv_order; /* 18 :a number */

    OS_FWRITE(head, sizeof(head), save_file);

    vaddr = UINT64_PTR(image->disp_info.luma_vir_addr);
    size = image->disp_info.head_info_size;
    OS_FWRITE(vaddr, size, save_file);

    vaddr += image->disp_info.head_info_size;
    size = slot_wid * slot_hig;
    OS_FWRITE(vaddr, size, save_file);
    chrom_vir_addr = image->disp_info.luma_vir_addr + (image->disp_info.chrom_phy_addr -
                                                       image->disp_info.luma_phy_addr);

    if (image->bit_depth > 8) { /* 8 :a number */
        luma_2bit_vir_addr = image->disp_info.luma_vir_addr + image->disp_info.luma_phy_addr_2bit -
                             image->disp_info.luma_phy_addr;
        chrom_2bit_vir_addr = luma_2bit_vir_addr + (image->disp_info.chrom_phy_addr_2bit -
                                                    image->disp_info.luma_phy_addr_2bit);
    }

    vaddr = UINT64_PTR(chrom_vir_addr);
    size = image->disp_info.head_info_size / 2; /* 2 :a number */
    OS_FWRITE(vaddr, size, save_file);

    vaddr += image->disp_info.head_info_size / 2; /* 2 :a number */
    size = slot_wid * slot_hig / 2; /* 2 :a number */
    OS_FWRITE(vaddr, size, save_file);

    if (image->bit_depth > 8) { /* 8 :a number */
        vaddr = UINT64_PTR(luma_2bit_vir_addr);
        size = slot_wid * slot_hig / 4; /* 4 :a number */
        OS_FWRITE(vaddr, size, save_file);

        vaddr = UINT64_PTR(chrom_2bit_vir_addr);
        size = slot_wid * slot_hig / 2 / 4; /* 2 :a number 4 :a number */
        OS_FWRITE(vaddr, size, save_file);
    }

    return VFMW_DBG_OK;
}

hi_s32 dbg_save_yuv(hi_s32 chan_id, vfmw_image *image, OS_FILE *save_file)
{
    hi_s32 ret = VFMW_DBG_ERR;
    hi_bool is_map = HI_FALSE;
    mem_record mem = { 0 };

    if (image == HI_NULL || save_file == HI_NULL) {
        return VFMW_DBG_ERR;
    }

    if (image->disp_info.luma_phy_addr == 0) {
        dprint(PRN_ALWS, "luma_phy_addr = 0\n");
        return VFMW_DBG_ERR;
    }

    if (image->disp_info.luma_vir_addr == 0) {
        mem.dma_buf = image->disp_info.frm_dma_buf;
        mem.phy_addr = image->disp_info.luma_phy_addr;
        mem.vir_addr = OS_KMMAP_CACHE(&mem);
        image->disp_info.luma_vir_addr = PTR_UINT64(mem.vir_addr);
        if (image->disp_info.luma_vir_addr == 0) {
            dprint(PRN_ALWS, "luma_vir_addr = 0\n");
            return VFMW_DBG_ERR;
        }
        is_map = HI_TRUE;
    }

    if (image->disp_info.linear_en == 1) {
        ret = dbg_save_linear(chan_id, image, save_file);
    } else {
        if (image->disp_info.compress_en == 0) {
            ret = dbg_save_linear(chan_id, image, save_file);
        } else {
            ret = dbg_save_compress(chan_id, image, save_file);
        }
    }

    if (is_map == HI_TRUE) {
        OS_UNMAP(&mem);
        image->disp_info.luma_vir_addr = 0;
    }

    return VFMW_DBG_OK;
}

hi_s32 dbg_save_mtdt(hi_s32 chan_id, vfmw_image *image, OS_FILE *save_file)
{
    hi_u8 *vir_addr;
    hi_u32 size;

    vir_addr = UINT64_PTR(image->hdr_input.hdr_metadata_vir_addr);
    size = image->hdr_input.hdr_metadata_size;

    if (vir_addr == HI_NULL) {
        dprint(PRN_ALWS, "metadata vir addr = 0\n");
        return VFMW_DBG_ERR;
    }

    if (size == 0) {
        dprint(PRN_ALWS, "metadata size = 0\n");
        return VFMW_DBG_ERR;
    }

    OS_FWRITE(vir_addr, size, save_file);
    dprint(PRN_ALWS, "Save metadata size %d.\n", size);

    return VFMW_DBG_OK;
}

hi_s32 dbg_set_print_word(hi_s32 word)
{
    g_print_word = word;

    return VFMW_DBG_OK;
}

hi_s32 dbg_get_print_word(hi_void)
{
    return g_print_word;
}

char *dbg_show_std(vfmw_vid_std std, vfmw_std_ext *ext)
{
    switch (std) {
        case VFMW_H264:
            return "H264";
        case VFMW_VC1:
            if (ext == HI_NULL) {
                return "VC1";
            } else {
                return (ext->vc1_ext.is_adv_profile) ? "VC1ap" : "VC1smp";
            }
            /* fall-through */
        case VFMW_MPEG4:
            return "MPEG4";
        case VFMW_MPEG2:
            return "MPEG2";
        case VFMW_DIVX3:
            return "DIVX3";
        case VFMW_AVS:
            return "AVS";
        case VFMW_JPEG:
            return "JPEG";
        case VFMW_REAL8:
            return "REAL8";
        case VFMW_REAL9:
            return "REAL9";
        case VFMW_VP6:
            return "VP6";
        case VFMW_VP6F:
            return "VP6F";
        case VFMW_VP6A:
            return "VP6A";
        case VFMW_VP8:
            return "VP8";
        case VFMW_VP9:
            return "VP9";
        case VFMW_H263:
            return "H263";
        case VFMW_SORENSON:
            return "SORENSON";
        case VFMW_MVC:
            return "MVC";
        case VFMW_HEVC:
            return "HEVC";
        case VFMW_MJPEG:
            return "MJPEG";
        case VFMW_AVS2:
            return "AVS2";
        case VFMW_AVS3:
            return "AVS3";
        default:
            return "NA";
    }
}

#if defined(ENV_ARMLINUX_KERNEL) || defined(VFMW_MDC_SUPPORT)
#ifdef ENV_ARMLINUX_KERNEL
#include "linux_proc.h"
#else
#ifdef VFMW_MDC_SUPPORT
#include "stdio.h"
#define OS_VA_LIST  va_list
#define OS_VA_START va_start
#define OS_VA_END   va_end
#endif
#endif

static hi_s8 g_tmp_msg[TMP_MSG_SIZE];

VFMW_ACC hi_s32 dbg_print(hi_u32 type, const hi_char *format, ...)
{
    OS_VA_LIST args;
    hi_s32 nchar;

    if ((type != PRN_ALWS) && ((g_print_word & (1 << type)) == 0)) {
        return -1;
    }

    OS_VA_START(args, format);
    nchar = vsnprintf_s(g_tmp_msg, sizeof(g_tmp_msg), sizeof(g_tmp_msg) - 1, format, args);
    g_tmp_msg[sizeof(g_tmp_msg) - 1] = '\0';
    OS_VA_END(args);

    if ((nchar <= 0) || (nchar > TMP_MSG_SIZE)) {
        return -1;
    }

    if (g_print_out == PRN_DEV_COM) {
        OS_PRINT("%s", g_tmp_msg);
    } else if (g_print_out == PRN_DEV_LOG) {
        OS_WR_LOG("%s", g_tmp_msg);
    }

    return 0;
}
#endif


