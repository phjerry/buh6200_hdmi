/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2016-07-22
 */
/******************************* include files ********************************/
/* common headers */
#include "hi_drv_mem.h"
/* local headers */
#include "strm_mng_listbuf.h"
#include "linux/hisilicon/securec.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/***************************** static definition *****************************/
static buf_mng_global st_bm_param =
{
    .inst_handle = 0,
    .is_init        = 0,
    .inst_head = LIST_HEAD_INIT(st_bm_param.inst_head)
};
/*********************************** code ************************************/
#define HI_KMALLOC_ATOMIC_LISTBUF(size)     HI_KMALLOC(HI_ID_VENC, size, GFP_ATOMIC)
#define HI_KFREE_LISTBUF(addr)       HI_KFREE(HI_ID_VENC, addr)

#define IS_READING(block) (LISTBUF_BLOCK_READING == (block)->status)
#define IS_WRITING(block) (LISTBUF_BLOCK_WRITING == (block)->status)
#define IS_FREE(block) (LISTBUF_BLOCK_FREE == (block)->status)

#define LISTBUF_ASSERT_RETURN(condition, ret_value) do {  \
    if (!(condition))                                                \
    {                                                              \
        HI_ERR_LISTBUF("assert warning\n");\
        return ret_value;                                           \
    }                                                              \
}while (0)

#define LISTBUF_FIND_INST(handle_inst, inst) \
    { \
        struct list_head* pos = HI_NULL; \
        struct list_head* n = HI_NULL; \
        list_buf_inst* tmp = HI_NULL; \
        (inst) = HI_NULL; \
        LISTBUF_SPIN_LOCK(st_bm_param.spin_lock, flags); \
        list_for_each_safe(pos, n, &st_bm_param.inst_head) \
        { \
            tmp = list_entry(pos, list_buf_inst, inst_node); \
            if ((handle_inst) == tmp->buf) \
            { \
                (inst) = tmp; \
                break; \
            } \
        } \
        LISTBUF_SPIN_UNLOCK(st_bm_param.spin_lock, flags); \
    }

/*********************************************/
static hi_void free_block_list(struct list_head* block_head)
{
    struct list_head* pos = HI_NULL;
    struct list_head* n = HI_NULL;
    list_buf_block* block = HI_NULL;

    list_for_each_safe(pos, n, block_head)
    {
        block = list_entry(pos, list_buf_block, block_node);
        list_del(pos);
        HI_KFREE_LISTBUF(block);
    }
}

static list_buf_block* find_list_first_head_frame_block(struct list_head* block_head)
{
    struct list_head* list = block_head->next;
    list_buf_block* block = HI_NULL;
    list_buf_block* tmp = HI_NULL;

    while (block_head != list) {
        tmp = list_entry(list, list_buf_block, block_node);
        if ((tmp->nalu_type == 1) || (tmp->nalu_type == 5)) { /* 5 is one of nalu type */
            block = tmp;
            break;
        }
        list = list->next;
    }

    return block;
}

static list_buf_block* find_list_first_tail_frame_block(struct list_head* block_tail)
{
    struct list_head* list = block_tail->prev;
    list_buf_block* block = HI_NULL;
    list_buf_block* tmp = HI_NULL;

    while (block_tail != list) {
        tmp = list_entry(list, list_buf_block, block_node);
        if ((tmp->nalu_type == 1) || (tmp->nalu_type == 5)) { /* 5 is one of nalu type */
            block = tmp;
            break;
        }
        list = list->prev;
    }

    return block;
}

static list_buf_block* find_all_first_tail_frame_block(list_buf_inst* inst)
{
    list_buf_block* block = HI_NULL;

    block = find_list_first_tail_frame_block(&inst->write_block_head);
    if (block != HI_NULL) {
        return block;
    }

    block = find_list_first_tail_frame_block(&inst->free_block_head);

    if (block != HI_NULL) {
        return block;
    }

    block = find_list_first_tail_frame_block(&inst->read_block_head);

    return block;
}

static list_buf_block* find_all_first_head_frame_block(list_buf_inst* inst)
{
    list_buf_block* block = HI_NULL;

    block = find_list_first_head_frame_block(&inst->read_block_head);
    if (block != HI_NULL) {
        return block;
    }

    block = find_list_first_head_frame_block(&inst->free_block_head);

    if (block != HI_NULL) {
        return block;
    }

    block = find_list_first_head_frame_block(&inst->write_block_head);

    return block;
}


static list_buf_block* find_list_head_block(struct list_head* block_head)
{
    struct list_head* list = block_head->next;
    list_buf_block* block = HI_NULL;

    if (list_empty(block_head)) {
        return HI_NULL;
    } else {
        block = list_entry(list, list_buf_block, block_node);
    }

    return block;
}

hi_s32 list_buf_get_strm_state(hi_handle buf, strm_mng_state* strm_stat_info)
{
    list_buf_inst* inst  = HI_NULL;

    hi_size_t flags;

    LISTBUF_FIND_INST(buf, inst);

    if (inst == HI_NULL || strm_stat_info == HI_NULL) {
        return HI_ERR_LISTBUF_INVALID_PARA;
    }

    LISTBUF_SPIN_LOCK(inst->spin_lock, flags);

    strm_stat_info->strm_head_free = inst->strm_head_free;
    strm_stat_info->strm_tail_free = inst->strm_tail_free;
    strm_stat_info->strm_get_try  = inst->get_try;
    strm_stat_info->strm_get_ok   = inst->get_ok;
    strm_stat_info->strm_put_try  = inst->put_try;
    strm_stat_info->strm_put_ok   = inst->put_ok;
    strm_stat_info->strm_recv_try = inst->recv_try;
    strm_stat_info->strm_recv_ok  = inst->recv_ok;
    strm_stat_info->strm_rls_try  = inst->rls_try;
    strm_stat_info->strm_rls_ok   = inst->rls_ok;

    LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return HI_SUCCESS;
}

/* open vedu use */
hi_void list_buf_init(hi_void)
{
    if (st_bm_param.is_init == 0) {
        /* init global parameter */
        st_bm_param.inst_handle = 0;

        /* init global mutex */
        venc_drv_osal_sem_init(&st_bm_param.sem);
        if (venc_drv_osal_lock_create(&st_bm_param.spin_lock) != HI_SUCCESS) {
            HI_ERR_VENC("st_bm_param.spin_lock init failed!\n");
        }

        INIT_LIST_HEAD(&st_bm_param.inst_head);

        st_bm_param.is_init = 1;
    }

    return;
}

hi_void list_buf_deinit(hi_void)
{
    struct list_head *pos = HI_NULL;
    struct list_head *n = HI_NULL;
    struct list_head *head = HI_NULL;
    list_buf_inst *inst = HI_NULL;
    hi_s32 ret = HI_FAILURE;

    if (st_bm_param.is_init) {
        LISTBUF_LOCK(st_bm_param.sem, ret);
        if (ret != HI_SUCCESS) {
            return;
        }
        head = &st_bm_param.inst_head;

        if (!list_empty(head)) {
            list_for_each_safe(pos, n, head) {
                inst = list_entry(pos, list_buf_inst, inst_node);
                LISTBUF_UNLOCK(st_bm_param.sem);
                list_buf_destroy(inst->buf);
                LISTBUF_LOCK(st_bm_param.sem, ret);
                if (ret != HI_SUCCESS) {
                    return;
                }
            }
        }

        venc_drv_osal_lock_destory(st_bm_param.spin_lock);
        st_bm_param.inst_handle = 0;
        st_bm_param.is_init = 0;

        LISTBUF_UNLOCK(st_bm_param.sem);
    }
}

static hi_void init_list_buf_inst_para(list_buf_inst *inst, hi_handle buf, strm_mng_inst_config *config)
{
    inst->buf = buf;

#ifdef HI_TEE_SUPPORT
    inst->tvp = config->tvp;
#endif
    inst->phy_addr = config->phy_addr;
    inst->knl_vir_addr = config->knl_vir_addr;
    inst->para_set_phy_addr  = config->para_set_phy_addr;
    inst->para_set_knl_vir_addr = config->para_set_knl_vir_addr;
    inst->size = config->size;
    HI_INFO_LISTBUF("[bufmng_create] PHY:%x, K-VIR:%llx, SIZE:%d\n",
        inst->phy_addr, inst->knl_vir_addr, inst->size);

    venc_drv_osal_lock_create(&inst->spin_lock);

    /* init block list parameter */
    inst->strm_head_free = config->size;
    inst->strm_tail_free = 0;
    inst->get_try  = 0;
    inst->get_ok   = 0;
    inst->put_try  = 0;
    inst->put_ok   = 0;
    inst->recv_try = 0;
    inst->recv_ok  = 0;
    inst->rls_try  = 0;
    inst->rls_ok   = 0;
    INIT_LIST_HEAD(&inst->write_block_head);
    INIT_LIST_HEAD(&inst->free_block_head);
    INIT_LIST_HEAD(&inst->read_block_head);

    return;
}

hi_s32 list_buf_create(hi_handle buf, strm_mng_inst_config *config)
{
    list_buf_inst *inst = HI_NULL;
    struct list_head* pos = HI_NULL;
    struct list_head* n = HI_NULL;
    list_buf_inst* tmp = HI_NULL;
    hi_s32 ret = HI_FAILURE;

    if (config == HI_NULL) {
        return HI_ERR_LISTBUF_INVALID_PARA;
    }

    list_for_each_safe(pos, n, &st_bm_param.inst_head) {
        tmp = list_entry(pos, list_buf_inst, inst_node);
        if (buf == tmp->buf) {
            HI_ERR_VENC("private handle already exists!\n");
            return HI_FAILURE;
        }
    }

    /* allocate an instance */
    inst = HI_KMALLOC_ATOMIC_LISTBUF(sizeof(list_buf_inst));
    if (inst == HI_NULL) {
        HI_FATAL_LISTBUF("no memory.\n");

        return HI_ERR_LISTBUF_NO_MEMORY;
    }
    memset_s(inst, sizeof(list_buf_inst), 0, sizeof(list_buf_inst));

    LISTBUF_LOCK(st_bm_param.sem, ret);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* init instance parameter */
    init_list_buf_inst_para(inst, buf, config);

    /* add this instance to instance list */
    list_add_tail(&inst->inst_node, &st_bm_param.inst_head);
    st_bm_param.inst_handle++;
    LISTBUF_UNLOCK(st_bm_param.sem);

    return HI_SUCCESS;
}

hi_s32 list_buf_destroy(hi_handle buf)
{
    list_buf_inst *inst = HI_NULL;
    hi_size_t flags;
    hi_s32 ret = HI_FAILURE;

    /* find instance by handle */
    LISTBUF_FIND_INST(buf, inst);

    if (inst == HI_NULL) {
        return HI_ERR_LISTBUF_INVALID_PARA;
    }
    LISTBUF_SPIN_LOCK(inst->spin_lock, flags);

    /* if has blocks, free them */
    if (!list_empty(&inst->write_block_head)) {
        free_block_list(&inst->write_block_head);
    }

    if (!list_empty(&inst->free_block_head)) {
        free_block_list(&inst->free_block_head);
    }

    if (!list_empty(&inst->read_block_head)) {
        free_block_list(&inst->read_block_head);
    }

    LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    LISTBUF_LOCK(st_bm_param.sem, ret);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (st_bm_param.inst_handle == 0) {
        HI_ERR_VENC("destroy failed! there is no channel!");
        LISTBUF_UNLOCK(st_bm_param.sem);

        return HI_FAILURE;
    }

    st_bm_param.inst_handle--;

    /* remove instance from list */
    list_del(&inst->inst_node);

    LISTBUF_UNLOCK(st_bm_param.sem);

    /* free resource */
    HI_KFREE_LISTBUF(inst);

    return HI_SUCCESS;
}

static hi_s32 check_all_list_empty(list_buf_inst* inst)
{
    if (list_empty(&inst->write_block_head)
        && list_empty(&inst->free_block_head)
        && list_empty(&inst->read_block_head)) {
        return HI_SUCCESS;
    } else {
        return HI_FAILURE;
    }
}

static hi_s32 list_buf_get_free_length(list_buf_inst* inst, hi_u32 nalu_type)
{
    list_buf_block* block_tail = HI_NULL;
    list_buf_block* block_read = HI_NULL;
    hi_u32 read_offset;
    hi_u32 write_offset;

    /* if paraset , don't need to calculate write/read offset */
    if ((nalu_type != 1) && (nalu_type != 5)) { /* 5 is one of nalu type */
        return HI_SUCCESS;
    } else if (check_all_list_empty(inst) == HI_SUCCESS) {    /* if empty, write from start */
        inst->strm_head_free = inst->size;
        inst->strm_tail_free = 0;
        return HI_SUCCESS;
    } else {     /* else, find write offset and read offset */
        block_tail = find_all_first_tail_frame_block(inst);

        if (block_tail == HI_NULL) {  /* only have one para set node in writting list */
            inst->strm_head_free = inst->size;
            inst->strm_tail_free = 0;
            return HI_SUCCESS;
        } else {
            write_offset = block_tail->addr + block_tail->size - inst->phy_addr;
            block_read = find_all_first_head_frame_block(inst);
            if (block_read != HI_NULL) {
                read_offset = block_read->addr - inst->phy_addr;
            } else {
                return HI_FAILURE;
            }
        }
    }

    /* normal: write pointer after read pointer, the free area isn't continuous */
    if (read_offset <= write_offset) {
        inst->strm_tail_free = inst->size - write_offset;
        inst->strm_head_free = read_offset;
    } else {     /* reverse: write pointer before read pointer, the free area is continuous , size is enough */
        inst->strm_tail_free = -1;
        inst->strm_head_free = read_offset - write_offset;
    }

    return HI_SUCCESS;
}

static hi_s32 check_free_stream_buffer(hi_u32 read_offset, hi_u32 write_offset, list_buf_inst *inst, strm_mng_buf *buf)
{
    hi_u32 tail_free = 0;
    hi_u32 head_free = 0;
    hi_bool alloc = HI_FALSE;

    /* normal: write pointer after read pointer, the free area isn't continuous */
    if (read_offset <= write_offset) {
        tail_free = inst->size - write_offset;
        head_free = read_offset;

        if (tail_free >= buf->size) {
            alloc = HI_TRUE;
        } else if (head_free >= buf->size) {
            alloc = HI_TRUE;

            /* alloc from head */
            write_offset = 0;
        }
    } else if ((read_offset - write_offset) >= buf->size) {
        /* reverse: write pointer before read pointer, the free area is continuous , size is enough */
        alloc = HI_TRUE;
    }

    /* allocate fail, return */
    if (!alloc) {
        return HI_ERR_LISTBUF_BUFFER_FULL;
    }

    return HI_SUCCESS;
}

static hi_s32 get_free_stream_buffer_addr(list_buf_block *block_tmp, list_buf_inst *inst, strm_mng_buf *buf)
{
    hi_s32 ret;
    hi_u32 read_offset, write_offset;
    list_buf_block *block_tail = HI_NULL;
    list_buf_block *block_read = HI_NULL;

    /* if paraset , don't need to calculate write/read offset */
    if ((buf->nalu_type != 1) && (buf->nalu_type != 5)) { /* 5 is one of nalu type */
        buf->addr = block_tmp->addr = inst->para_set_phy_addr;
        buf->vir_addr = block_tmp->vir_addr = inst->para_set_knl_vir_addr;
        return HI_SUCCESS;
    } else if (check_all_list_empty(inst) == HI_SUCCESS) {     /* if empty, write from start */
        buf->addr = block_tmp->addr = inst->phy_addr;
        buf->vir_addr = block_tmp->vir_addr = inst->knl_vir_addr;

        return HI_SUCCESS;
    } else  {     /* else, find write offset and read offset */
        block_tail = find_all_first_tail_frame_block(inst);

        if (block_tail == HI_NULL) { /* only have one para set node in writting list */
            buf->addr = block_tmp->addr = inst->phy_addr;
            buf->vir_addr = block_tmp->vir_addr = inst->knl_vir_addr;

            return HI_SUCCESS;
        } else {
            write_offset = block_tail->addr + block_tail->size - inst->phy_addr;
            block_read = find_all_first_head_frame_block(inst);

            if (block_read != HI_NULL)  {
                read_offset = block_read->addr - inst->phy_addr;
            } else {
                return HI_ERR_LISTBUF_NO_MEMORY;
            }
        }
    }

    ret = check_free_stream_buffer(read_offset, write_offset, inst, buf);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /* init block parameter */
    buf->addr = block_tmp->addr = inst->phy_addr + write_offset;
    buf->vir_addr = block_tmp->vir_addr = inst->knl_vir_addr + write_offset;

    return HI_SUCCESS;
}
static hi_s32 get_one_write_buffer(hi_handle handle_buf, strm_mng_buf *buf)
{
    list_buf_inst *inst   = HI_NULL;
    list_buf_block block_tmp = {0};
    list_buf_block *block = HI_NULL;
    hi_size_t flags;
    hi_s32 ret;

    LISTBUF_ASSERT_RETURN(buf != HI_NULL, HI_ERR_LISTBUF_INVALID_PARA);

    LISTBUF_FIND_INST(handle_buf, inst);
    LISTBUF_ASSERT_RETURN(inst != HI_NULL, HI_ERR_LISTBUF_INVALID_PARA);

    /* allocate new block */
    block = HI_KMALLOC_ATOMIC_LISTBUF(sizeof(list_buf_block));
    if (block == HI_NULL) {
        return HI_ERR_LISTBUF_NO_MEMORY;
    }
    memset_s(block, sizeof(list_buf_block), 0, sizeof(list_buf_block));

    LISTBUF_SPIN_LOCK(inst->spin_lock, flags);

    if (list_buf_get_free_length(inst, 1) != HI_SUCCESS) {
        LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);
        HI_KFREE_LISTBUF(block);
        return HI_ERR_LISTBUF_INVALID_PARA;
    }

    /* get buffer try times increase */
    inst->get_try++;

    ret = get_free_stream_buffer_addr(&block_tmp, inst, buf);
    if (ret != HI_SUCCESS) {
        LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);
        HI_KFREE_LISTBUF(block);
        return ret;
    }

    block->addr = block_tmp.addr;
    block->vir_addr = block_tmp.vir_addr;
    block->size = buf->size;
    block->nalu_type = buf->nalu_type;
    block->invld_byte = buf->invld_byte;
    block->status = LISTBUF_BLOCK_WRITING;

    /* add block to list */
    list_add_tail(&block->block_node, &inst->write_block_head);
    /* get buffer OK times increase */
    inst->get_ok++;

    LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return HI_SUCCESS;
}

static hi_void copy_stream_header_h265(hi_u32 *header_real_length, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info, hi_u8* stm_header, strm_mng_buf *stream_buf)
{
    *header_real_length = input_info->sps_bits / 8 + input_info->pps_bits / 8 +  /* 8 is bit */
        input_info->vps_bits / 8; /* 8 is bit */
    stream_info->para_set_real_len = *header_real_length;
    stream_info->para_set_len = D_VENC_ALIGN_UP(*header_real_length, VEDU_MMZ_ALIGN);

    memcpy_s(stm_header, input_info->vps_bits / 8, input_info->vps_stream,  /* 8 is bit */
        input_info->vps_bits / 8); /* 8 is bit */
    stm_header += input_info->vps_bits / 8; /* 8 is bit */

    memcpy_s(stm_header, input_info->sps_bits / 8, input_info->sps_stream,  /* 8 is bit */
        input_info->sps_bits / 8); /* 8 is bit */
    stm_header += input_info->sps_bits / 8; /* 8 is bit */

    memcpy_s(stm_header, input_info->pps_bits / 8, input_info->pps_stream,  /* 8 is bit */
        input_info->pps_bits / 8); /* 8 is bit */

    stream_buf->nalu_type = H265_NALU_TYPE_SPS; /* para set only use sps as nalu type */
}

static hi_s32 get_para_set_write_buffer(hi_handle handle, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info)
{
    hi_u32 ret = HI_SUCCESS;
    strm_mng_buf stream_buf;
    hi_u8 stream_header[400]; /* 400 is size of  stream_header */
    hi_u32 header_real_length;
    hi_s32 i_bytes;
    hi_u32* buf = NULL;
    hi_u32 i;
    hi_u8* stm_header = (hi_u8*)stream_header;

    memset_s(stm_header, sizeof(stream_header), 0, sizeof(stream_header));
    memset_s(&buf, sizeof(strm_mng_buf), 0, sizeof(strm_mng_buf));

    for (i = 0; i < 48; i++) { /* 48 is max num */
        stream_info->para_set_array[i] = 0;
    }

    if ((input_info->prepend_sps_pps_enable == HI_TRUE) || (input_info->get_frame_num_ok == 1) ||
        (stream_info->i_frm_insert_by_save_stream)) {
        if (input_info->protocol == VEDU_H264) {
            header_real_length = input_info->sps_bits / 8 + input_info->pps_bits / 8; /* 8 is bit */
            stream_info->para_set_real_len = header_real_length;
            stream_info->para_set_len = D_VENC_ALIGN_UP(header_real_length, VEDU_MMZ_ALIGN);

            memcpy_s(stm_header, input_info->sps_bits / 8, input_info->sps_stream,  /* 8 is bit */
                input_info->sps_bits / 8); /* 8 is bit */
            stm_header += input_info->sps_bits / 8; /* 8 is bit */
            memcpy_s(stm_header, input_info->pps_bits / 8, input_info->pps_stream,  /* 8 is bit */
                input_info->pps_bits / 8); /* 8 is bit */

            stream_buf.nalu_type = H264_NALU_TYPE_SPS;
        } else {
            copy_stream_header_h265(&header_real_length, input_info, stream_info, stm_header, &stream_buf);
        }
        stream_buf.size = stream_info->para_set_len;
        stream_buf.addr = stream_info->slice_buf.start_phy_addr;
        stream_buf.vir_addr = stream_info->slice_buf.start_vir_addr;
        stream_buf.invld_byte = stream_buf.size - header_real_length;
        ret = get_one_write_buffer(handle, &stream_buf);

        if (ret != HI_SUCCESS) {
            stream_info->i_frm_insert_by_save_stream = 0;
            return ret;
        }

        i_bytes = header_real_length;
        buf = (hi_u32*)stream_header;

        for (i = 0; i_bytes > 0; i++, i_bytes -= 4) { /* 4 is calc i_byte num */
            stream_info->para_set_array[i] = buf[i] ;
        }

        stream_info->i_frm_insert_by_save_stream = 0;
    }

    return HI_SUCCESS;
}

static hi_void get_strm_buf_info_for_slice(strm_mng_buf *buf, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info, hi_u32 buf_num)
{
    hi_u32 buffer_size = 0;
    hi_u32 check_buf_size = 0;
    hi_u32 max_buf_size = 0;
    memset_s(buf, sizeof(strm_mng_buf), 0, sizeof(strm_mng_buf));
    if (buf_num == 1) {
        buffer_size = input_info->target_bits * BUF_SIZE_MULT / 8; /* 8 is calc targe_bits */
        check_buf_size = input_info->target_bits * BUF_SIZE_MULT_CHECK / 8; /* 8 is calc targe_bits */
    } else {
        buffer_size = input_info->target_bits * BUF_SIZE_MULT_SPLIT / 8; /* 8 is calc targe_bits */
        check_buf_size = input_info->target_bits * BUF_SIZE_MULT_SPLIT_CHECK / 8; /* 8 is calc targe_bits */
    }

    buf->size = D_VENC_ALIGN_UP(buffer_size / buf_num, 64); /* 64 is calc buffer_size */

    if ((buf->size * buf_num) > stream_info->strm_buf_size) {
        max_buf_size = (stream_info->strm_buf_size - 64 * HI_VENC_SLICE_NUM) / buf_num; /*  64 is buf size */
    } else {
        max_buf_size = buffer_size / buf_num;
    }

    buf->size = D_VENC_ALIGN_UP(max_buf_size, VEDU_MMZ_ALIGN);
    buf->invld_byte = buf->size - max_buf_size;
    stream_info->cur_strm_buf_size = D_VENC_ALIGN_UP(check_buf_size, VEDU_MMZ_ALIGN);

    HI_INFO_VENC("input_info->target_bits = %d, buf->size = %d,cur_strm_buf_size = %d\n",
        input_info->target_bits, buf->size, stream_info->cur_strm_buf_size);
    if (input_info->protocol != VEDU_H265) {
        buf->nalu_type = (input_info->is_intra_pic) ?  H264_NALU_TYPE_IDR : H264_NALU_TYPE_P;
    } else {
        buf->nalu_type = (input_info->is_intra_pic) ?  H265_NALU_TYPE_IDR : H265_NALU_TYPE_P;
    }
}

static hi_void fill_block_stream_info(list_buf_block *block, strm_mng_buf *buf)
{
    block->size  = buf->size;
    block->invld_byte = buf->invld_byte;
    block->frame_end = buf->frame_end;
    block->pts0_ms = buf->pts0_ms;
    block->pts1_ms = buf->pts1_ms;
    block->ext_flag = buf->ext_flag;
    block->ext_fill_len = buf->ext_fill_len;
    block->status = LISTBUF_BLOCK_FREE;
}

static hi_s32 put_one_write_buffer(hi_handle handle_buf, strm_mng_buf *buf)
{
    list_buf_inst *inst   = HI_NULL;
    list_buf_block *block = HI_NULL;
    hi_size_t flags;

    if (buf == HI_NULL) {
        return HI_ERR_LISTBUF_INVALID_PARA;
    }
    LISTBUF_FIND_INST(handle_buf, inst);

    if (inst == HI_NULL) {
        return HI_ERR_LISTBUF_INVALID_PARA;
    }
    LISTBUF_SPIN_LOCK(inst->spin_lock, flags);

    /* put buffer try times increase */
    inst->put_try++;

    /* find the tail block */
    block = find_list_head_block(&inst->write_block_head);
    if (block == HI_NULL) {
        LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);

        return HI_ERR_LISTBUF_WRITE_FREE_ERR;
    }

    if (buf->size > block->size) {
        LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);
        HI_ERR_LISTBUF("put size err:%d>%d\n", buf->size, block->size);
        return HI_ERR_LISTBUF_WRITE_FREE_ERR;
    }

    /* the block must be WRITING status and its address must be right */
    if (IS_WRITING(block) && (buf->addr == block->addr)) {
        /* if size=0, drop this block */
        if (buf->size == 0) {
            /* delete block from list */
            list_del(&block->block_node);

            /* free block resource */
            HI_KFREE_LISTBUF(block);

            inst->put_ok++;
            LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);

            return HI_SUCCESS;
        }

        fill_block_stream_info(block, buf);
    } else {
        LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);
        HI_ERR_LISTBUF("put fail.\n");
        return HI_ERR_LISTBUF_WRITE_FREE_ERR;
    }
    /* delete block from write list, add the free list */
    list_del(&block->block_node);
    list_add_tail(&block->block_node, &inst->free_block_head);

    /* put buffer OK times increase */
    inst->put_ok++;

    LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return HI_SUCCESS;
}

static hi_void put_para_set_buf(hi_handle handle_buf, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info, hi_u32 para_set_len)
{
    if ((input_info->prepend_sps_pps_enable == HI_TRUE) || (input_info->get_frame_num_ok == 1)) {
        strm_mng_buf buf;
        memset_s(&buf, sizeof(strm_mng_buf), 0, sizeof(strm_mng_buf));
        buf.size = para_set_len;
        buf.addr = stream_info->slice_buf.start_phy_addr;
        buf.invld_byte = buf.size - stream_info->para_set_real_len;
        buf.pts0_ms = input_info->pts0;
        buf.pts1_ms = input_info->pts1;
        buf.frame_end = 0;
        put_one_write_buffer(handle_buf, &buf);
    }
}

static hi_void put_slice_strm_buf_normal(hi_handle handle_buf, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info)
{
    hi_u32 i = 0;
    hi_u32 j = 0;
    hi_u32 real_slice_size = 0;
    hi_u32 end_of_slice = 0;
    hi_u32 buf_num;
    hi_u32 slic_idx;
    strm_mng_buf  *buf = stream_info->buf;

    buf_num = input_info->slc_split_en ? HI_VENC_SLICE_NUM : 1;

    slic_idx = stream_info->slice_idx;

    for (i = slic_idx; i < buf_num; i++) {
        real_slice_size = input_info->slice_length[i];
        end_of_slice    = input_info->slice_is_end[i];
        if (real_slice_size) {
            buf[i].size = D_VENC_ALIGN_UP(input_info->slice_length[i], VEDU_MMZ_ALIGN);
            buf[i].invld_byte = buf[i].size - input_info->slice_length[i];
            buf[i].pts0_ms = input_info->pts0;
            buf[i].pts1_ms = input_info->pts1;
            buf[i].ext_flag   = input_info->ext_flag;
            buf[i].ext_fill_len = input_info->ext_fill_len;
            buf[i].frame_end = end_of_slice;
            stream_info->slice_idx++;
            put_one_write_buffer(handle_buf, &buf[i]);
        }

        if (end_of_slice) {
            for (j = stream_info->slice_idx; j < buf_num; j++) {
                /* buf do not be used */
                buf[j].size = 0;
                put_one_write_buffer(handle_buf, &buf[j]);
            }

            break;
        }

        if ((!real_slice_size) && (!end_of_slice)) {
            break;
        }
    }
}

static hi_void put_slice_strm_buf_abnormal(hi_handle buf, hi_u16 slc_split_en, strm_mng_stream_info *stream_info)
{
    hi_u32 i = 0;
    hi_u32 buf_num;
    hi_u32 ret;

    buf_num = slc_split_en ? HI_VENC_SLICE_NUM : 1;
    for (i = stream_info->slice_idx; i < buf_num; i++) {
        stream_info->buf[i].size = 0;
        ret = put_one_write_buffer(buf, &stream_info->buf[i]);
    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("put one write buffer error\n");
        return;
    }
}
}

hi_s32 list_buf_put_buf_for_recode(hi_handle buf, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info)
{
    stream_info->slice_idx = 0;

    if (input_info->is_intra_pic) {
        put_para_set_buf(buf, input_info, stream_info, 0);
    }

    put_slice_strm_buf_abnormal(buf, input_info->slc_split_en, stream_info);

    return HI_SUCCESS;
}

static hi_s32 get_stream_write_buffer(hi_handle handle_buf, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info)
{
    strm_mng_buf buf;
    hi_u32 buf_num;
    hi_u32 i = 0;
    hi_u32 j = 0;
    hi_u32 ret = HI_SUCCESS;

    *(stream_info->strm_buf_rp_vir_addr) = 0;
    *(stream_info->strm_buf_wp_vir_addr) = 0;
    buf_num = input_info->slc_split_en ? HI_VENC_SLICE_NUM : 1;

    for (i = 0; i < 16; i++) { /* 16 is max szie */
        stream_info->stream_buf_addr_array[i] = 0;
        stream_info->stream_buf_len_array[i] = 0;
    }

    get_strm_buf_info_for_slice(&buf, input_info, stream_info, buf_num);
    for (i = 0; i < buf_num; i++) {
        ret = get_one_write_buffer(handle_buf, &buf);

        if (ret != HI_SUCCESS) {
            if (input_info->is_intra_pic) {
                put_para_set_buf(handle_buf, input_info, stream_info, 0);
            }

            for (j = 0; j < i; j++) {
                memset_s(&buf, sizeof(strm_mng_buf), 0, sizeof(strm_mng_buf));
                buf.addr = stream_info->buf[j].addr;
                buf.size = 0;
                put_one_write_buffer(handle_buf, &buf);
            }

            return ret;
        }
        stream_info->buf[i] = buf;
        stream_info->stream_buf_addr_array[i] = buf.addr;
        stream_info->stream_buf_len_array[i] = buf.size;
    }
    stream_info->first_slc_one_frm = HI_TRUE;

    return HI_SUCCESS;
}

hi_s32 list_buf_get_write_buf(hi_handle handle, strm_mng_input_info *input_info, strm_mng_stream_info *stream_info)
{
    hi_u32 ret = HI_SUCCESS;

    if ((input_info == HI_NULL) || (stream_info == HI_NULL)) {
        return HI_FAILURE;
    }

    if (input_info->is_intra_pic) {
        ret = get_para_set_write_buffer(handle, input_info, stream_info);

        if (ret != HI_SUCCESS) {
            return ret;
        }
    }

    ret =  get_stream_write_buffer(handle, input_info, stream_info);

    return ret;
}

static hi_s32 list_buf_put_write_buf_slices(hi_handle buf, strm_mng_input_info *input_info,
    strm_mng_stream_info *stream_info)
{
    if ((input_info->is_intra_pic) && (stream_info->first_slc_one_frm)) {
        stream_info->first_slc_one_frm = HI_FALSE;
        put_para_set_buf(buf, input_info, stream_info, stream_info->para_set_len);
    }

    put_slice_strm_buf_normal(buf, input_info, stream_info);

    return HI_SUCCESS;
}

static hi_s32 list_buf_put_write_buf_slices_ab(hi_handle buf, strm_mng_input_info* input_info,
    strm_mng_stream_info* stream_info)
{
    if ((input_info->is_intra_pic) && (stream_info->first_slc_one_frm == HI_TRUE)) {
        stream_info->first_slc_one_frm = HI_FALSE;
        put_para_set_buf(buf, input_info, stream_info, 0);
    }

    put_slice_strm_buf_abnormal(buf, input_info->slc_split_en, stream_info);

    stream_info->too_few_buffer_skip++;

    return HI_SUCCESS;
}


hi_s32 list_buf_put_write_buf(hi_handle buf, strm_mng_input_info* input_info, strm_mng_stream_info* stream_info)
{
    if ((input_info == HI_NULL) || (stream_info == HI_NULL)) {
        return HI_FAILURE;
    }

    if (!input_info->venc_buf_full && !input_info->venc_pbit_overflow && !input_info->time_out) {
        list_buf_put_write_buf_slices(buf, input_info, stream_info);
    } else {
        list_buf_put_write_buf_slices_ab(buf, input_info, stream_info);
    }

    return HI_SUCCESS;
}

static hi_s32 acq_one_read_buffer(hi_handle handle_buf, strm_mng_buf *buf)
{
    list_buf_inst* inst   = HI_NULL;
    list_buf_block* block = HI_NULL;
    hi_size_t flags;

    buf->size = 0;

    LISTBUF_FIND_INST(handle_buf, inst);

    if (inst == HI_NULL) {
        return HI_ERR_LISTBUF_INVALID_PARA;
    }

    LISTBUF_SPIN_LOCK(inst->spin_lock, flags);

    /* request buffer try times increase */
    inst->recv_try++;

    /* find first free block */
    block = find_list_head_block(&inst->free_block_head);

    if (block == HI_NULL) {
        LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);
        return HI_ERR_LISTBUF_BUFFER_EMPTY;
    }

    /* read from the block */
    buf->addr = block->addr;
    buf->vir_addr = block->vir_addr;
    buf->size = block->size;
    buf->nalu_type = block->nalu_type;
    buf->invld_byte = block->invld_byte;
    buf->frame_end = block->frame_end;
    buf->pts0_ms  = block->pts0_ms ;
    buf->pts1_ms  = block->pts1_ms ;
    buf->ext_flag    = block->ext_flag;
    buf->ext_fill_len = block->ext_fill_len;

    /* change status */
    block->status = LISTBUF_BLOCK_READING;

    list_del(&block->block_node);
    list_add_tail(&block->block_node, &inst->read_block_head);

    /* request buffer OK times increase */
    inst->recv_ok++;

    LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return HI_SUCCESS;
}

hi_s32 list_buf_get_acquire_stream_info(handle_protocol *h_protocol, hi_u8* start_vir_addr,
    vedu_efl_nalu *vedu_packet, hi_venc_stream* stream, venc_ioctl_buf_offset *buf_offset)
{
    hi_u32 addr_offset = 0;

    LISTBUF_ASSERT_RETURN((vedu_packet != NULL), HI_ERR_LISTBUF_INVALID_PARA);
    LISTBUF_ASSERT_RETURN((stream != NULL), HI_ERR_LISTBUF_INVALID_PARA);
    LISTBUF_ASSERT_RETURN((buf_offset != NULL), HI_ERR_LISTBUF_INVALID_PARA);

    stream->virt_addr = (hi_size_t)(ULONG)(vedu_packet->virt_addr[0]);
    if ((vedu_packet->nalu_type == 1) || (vedu_packet->nalu_type == 5)) {  /* I frame or P frame,5 is type */
        addr_offset = stream->virt_addr - (hi_size_t)(ULONG)start_vir_addr - 64; /* 64 is buf size */
    } else {
        addr_offset = 0;
    }

    stream->slc_len = vedu_packet->slc_len[0];
    stream->frame_end = vedu_packet->frame_end;
    stream->pts_ms = vedu_packet->pts0;


    if (h_protocol->protocol == VEDU_H265) {
#ifdef VENC_SUPPORT_H265
        stream->data_type.h265e_type = trans_nalu_type_num_for_h265(vedu_packet->nalu_type);
#endif
    } else {
        stream->data_type.h264e_type = vedu_packet->nalu_type;
    }

    if (stream->slc_len > 0) {
        buf_offset->strm_buf_offset[0] = addr_offset;
    }

    return HI_SUCCESS;
}

hi_s32 list_buf_acquire_bit_stream(hi_handle handle_buf, strm_mng_stream_info *stream_info, vedu_efl_nalu *vedu_packet)
{
    strm_mng_buf buf = {0};
    hi_s32 ret;

    LISTBUF_ASSERT_RETURN((stream_info != NULL), HI_ERR_LISTBUF_INVALID_PARA);
    LISTBUF_ASSERT_RETURN((vedu_packet != NULL), HI_ERR_LISTBUF_INVALID_PARA);

    memset_s(&buf, sizeof(strm_mng_buf), 0, sizeof(strm_mng_buf));
    ret = acq_one_read_buffer(handle_buf, &buf);

    vedu_packet->virt_addr[0] = (hi_void*)(buf.vir_addr);
    vedu_packet->nalu_type     = buf.nalu_type;
    vedu_packet->slc_len[0]    = buf.size - buf.invld_byte;
    vedu_packet->frame_end    = (buf.frame_end == 0) ? HI_FALSE : HI_TRUE;
    vedu_packet->pts0         = buf.pts0_ms;
    vedu_packet->pts1         = buf.pts1_ms;
    vedu_packet->ext_flag      = buf.ext_flag;
    vedu_packet->ext_fill_len   = buf.ext_fill_len;
    vedu_packet->invld_byte    = buf.invld_byte;
    stream_info->stream_total_byte += vedu_packet->slc_len[0];

    return ret;
}

static hi_s32 list_buf_rls_read_buffer(hi_handle handle_buf, strm_mng_buf *buf)
{
    hi_u32 phy_addr;
    list_buf_inst *inst   = HI_NULL;
    list_buf_block *block = HI_NULL;
    hi_u32 nalu_type;
    hi_size_t flags;

    if (buf == HI_NULL) {
        return HI_ERR_LISTBUF_INVALID_PARA;
    }

    LISTBUF_FIND_INST(handle_buf, inst);

    if (inst == HI_NULL) {
        return HI_ERR_LISTBUF_INVALID_PARA;
    }

    LISTBUF_SPIN_LOCK(inst->spin_lock, flags);

    /* release buffer try times increase */
    inst->rls_try++;
    nalu_type = buf->nalu_type;

    /* find block by address */
    if ((nalu_type == 1) || (nalu_type == 5)) { /* 5 is one of nalu type */
        phy_addr = buf->vir_addr - inst->knl_vir_addr + inst->phy_addr; /* buf why has no phyaddr */
    } else {
        phy_addr = inst->para_set_phy_addr;
    }

    block = find_list_head_block(&inst->read_block_head);

    if ((block == HI_NULL) || (phy_addr != block->addr)) {
        LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);

        return HI_ERR_LISTBUF_READ_FREE_ERR;
    }

    /* support free out-of-order */
    if (IS_READING(block)) {
        /* delete block from list */
        list_del(&block->block_node);

        /* free block resource */
        HI_KFREE_LISTBUF(block);
    } else {
        LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);

        return HI_ERR_LISTBUF_READ_FREE_ERR;
    }

    /* release buffer OK times increase */
    if (list_buf_get_free_length(inst, nalu_type) != HI_SUCCESS) {
        LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);

        return HI_ERR_LISTBUF_READ_FREE_ERR;
    }

    inst->rls_ok++;
    LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return HI_SUCCESS;
}

hi_s32 list_buf_get_release_stream_info(hi_handle buf, hi_u32 protocol, strm_mng_stream_info *stream_info,
                                    venc_ioctl_acquire_stream* acq_strm, vedu_efl_nalu* vedu_packet)
{
    hi_u32 nalu_type = 0;
    hi_void*  base = HI_NULL;
    hi_venc_stream  stream;

    LISTBUF_ASSERT_RETURN((stream_info != NULL), HI_ERR_LISTBUF_INVALID_PARA);
    LISTBUF_ASSERT_RETURN((acq_strm != NULL), HI_ERR_LISTBUF_INVALID_PARA);
    LISTBUF_ASSERT_RETURN((vedu_packet != NULL), HI_ERR_LISTBUF_INVALID_PARA);

    base = stream_info->stream_mmz_buf.start_vir_addr + VEDU_MMZ_ALIGN;
    stream = acq_strm->stream;

    if (protocol == VEDU_H264) {
        if (stream.data_type.h264e_type == HI_VENC_H264E_NALU_SPS) {
            stream.virt_addr  = (hi_size_t)(stream_info->slice_buf.start_vir_addr + acq_strm->h264_stream_off);
        } else {
            stream.virt_addr  = (hi_size_t)(base + acq_strm->h264_stream_off);
        }
        nalu_type = stream.data_type.h264e_type;
    } else {
#ifdef VENC_SUPPORT_H265
        if (stream.data_type.h265e_type == HI_VENC_H265E_NALU_SPS) {
            stream.virt_addr  = (hi_size_t)(stream_info->slice_buf.start_vir_addr + acq_strm->h264_stream_off);
        } else {
            stream.virt_addr  = (hi_size_t)(base + acq_strm->h264_stream_off);
        }
        nalu_type = inverse_trans_nalu_type_num_for_h265(stream.data_type.h265e_type);
#endif
    }

    vedu_packet->virt_addr[0] = (hi_void*)(ULONG)stream.virt_addr;
    vedu_packet->nalu_type     = nalu_type;

    return HI_SUCCESS;
}

hi_s32 list_buf_release_bit_stream(hi_handle handle_buf, strm_mng_stream_info *stream_info, vedu_efl_nalu* vedu_packet)
{
    strm_mng_buf buf;
    hi_s32 ret;

    LISTBUF_ASSERT_RETURN((stream_info != NULL), HI_ERR_LISTBUF_INVALID_PARA);
    LISTBUF_ASSERT_RETURN((vedu_packet != NULL), HI_ERR_LISTBUF_INVALID_PARA);

    buf.vir_addr = (hi_u8*)vedu_packet->virt_addr[0];
    buf.nalu_type = vedu_packet->nalu_type;
    ret = list_buf_rls_read_buffer(handle_buf, &buf);

    return ret;
}

static hi_s32 calculate_write_read_offset(hi_u32 *read_offset, hi_u32 *write_offset, list_buf_block *block_tail,
                                          list_buf_inst *inst)
{
    list_buf_block *block_read = HI_NULL;
    *write_offset = block_tail->addr + block_tail->size - inst->phy_addr;
    block_read = find_all_first_head_frame_block(inst);

    if (block_read != HI_NULL) {
        *read_offset = block_read->addr - inst->phy_addr;
    } else {
        return HI_ERR_LISTBUF_BUFFER_FULL;
    }

    return HI_SUCCESS;
}

static hi_s32 check_buffer_size(hi_handle handle_buf, strm_mng_buf *buf)
{
    list_buf_inst *inst   = HI_NULL;
    list_buf_block *block_tail = HI_NULL;
    hi_u32 read_offset;
    hi_u32 write_offset;
    hi_u32 tail_free;
    hi_u32 head_free;
    hi_bool alloc = HI_FALSE;
    hi_size_t flags;
    hi_s32 ret = HI_FAILURE;

    LISTBUF_FIND_INST(handle_buf, inst);
    LISTBUF_ASSERT_RETURN(inst != HI_NULL, HI_ERR_LISTBUF_INVALID_PARA);

    LISTBUF_SPIN_LOCK(inst->spin_lock, flags);

    /* buffer full */
    if (check_all_list_empty(inst) == HI_SUCCESS) {
        LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);

        return HI_SUCCESS;
    } else {     /* else, find write offset and read offset */
        block_tail = find_all_first_tail_frame_block(inst);

        if (block_tail == HI_NULL) { /* only have one para set node in writting list */
            LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);
            return HI_SUCCESS;
        } else {
            ret = calculate_write_read_offset(&read_offset, &write_offset, block_tail, inst);
            if (ret != HI_SUCCESS) {
                LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);
                return HI_ERR_LISTBUF_BUFFER_FULL;
            }
        }
    }

    /* normal: write pointer after read pointer, the free area isn't continuous */
    if (read_offset <= write_offset) {
        tail_free = inst->size - write_offset;
        head_free = read_offset;
        if (tail_free >= buf->size) {
            alloc = HI_TRUE;
        } else if (head_free >= buf->size) {
            alloc = HI_TRUE;
        }
    } else if ((read_offset - write_offset) >= buf->size) {
    /* reverse: write pointer before read pointer, the free area is continuous , size is enough */
        alloc = HI_TRUE;
    }

    /* allocate fail, return */
    if (!alloc) {
        LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);
        return HI_ERR_LISTBUF_BUFFER_FULL;
    }
    LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return HI_SUCCESS;
}

hi_s32 list_buf_check_buffer_size(hi_handle handle_buf, hi_u32 strm_buf_size, hi_u32 cur_strm_buf_size)
{
    strm_mng_buf buf;

    memset_s(&buf, sizeof(strm_mng_buf), 0, sizeof(strm_mng_buf));
    buf.size = D_VENC_ALIGN_UP(cur_strm_buf_size, VEDU_MMZ_ALIGN);

    if (buf.size > strm_buf_size) {
        buf.size = D_VENC_ALIGN_DOWN(strm_buf_size, VEDU_MMZ_ALIGN);
    }

    if (check_buffer_size(handle_buf, &buf) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_u32 list_buf_get_written_len(hi_handle buf)
{
    list_buf_inst *inst   = HI_NULL;
    struct list_head *free_block_head = HI_NULL;
    struct list_head *list = HI_NULL;
    list_buf_block* block = HI_NULL;
    hi_size_t flags;
    hi_u32 strm_len = 0;

    LISTBUF_FIND_INST(buf, inst);
    LISTBUF_ASSERT_RETURN(inst != HI_NULL, 0);

    LISTBUF_SPIN_LOCK(inst->spin_lock, flags);

    free_block_head = &inst->free_block_head;

    if (!list_empty(free_block_head)) {
        list = inst->free_block_head.next;

        while (free_block_head != list) {
            block = list_entry(list, list_buf_block, block_node);
            strm_len += block->size - block->invld_byte;

            list = list->next;
        }
    }

    LISTBUF_SPIN_UNLOCK(inst->spin_lock, flags);

    return strm_len;
}

hi_u32 list_buf_cal_slc_size(hi_handle buf, hi_u32 protocol, hi_u32 pic_height, hi_u32 unf_split_size)
{
    hi_u32 slice_num;
    hi_u32 line_height;
    hi_u32 ever_slice_line_num;
    hi_u32 pic_height_align;
    const hi_u32 lcu_size_hevc = 32;
    const hi_u32 lcu_size_h264 = 16;
    hi_u32 slc_size;

    line_height = (protocol == VEDU_H264) ? lcu_size_h264 : lcu_size_hevc;
    pic_height_align = D_VENC_ALIGN_UP(pic_height, line_height);
    slice_num = (pic_height_align +  line_height - 1) / line_height;
    ever_slice_line_num = (slice_num + HI_VENC_SLICE_NUM - 1) / HI_VENC_SLICE_NUM;

    /* the info about splitsize logic gets is the real value minus 1 */
    slc_size = ever_slice_line_num - 1;

    return slc_size;
}

hi_u32 list_buf_get_bit_buf_size(hi_handle buf, hi_u32 stream_buf_size,
    hi_u32 slc_split_en, hi_u32 luma_size, hi_u32 *ext_len)
{
    return stream_buf_size;
}

hi_s32 list_buf_alloc_stream_buf(hi_handle buf, venc_buffer* stream_mmz_buf,
    strm_mng_alloc_info *alloc_info, hi_bool enable_rcn_ref_share_buf)
{
    hi_s32 ret;
    hi_u32 alloc_buf_size = 0;

    if (stream_mmz_buf == HI_NULL || alloc_info == HI_NULL) {
        return HI_FAILURE;
    }

    if (enable_rcn_ref_share_buf == HI_TRUE) {
        alloc_buf_size = alloc_info->rcn_size + alloc_info->rcn_block_size + alloc_info->bit_buf_size +
            alloc_info->me_mv_size;
    } else {
        alloc_buf_size = alloc_info->rcn_size + alloc_info->bit_buf_size + alloc_info->me_mv_size;
    }

    ret = hi_drv_venc_alloc_and_map("venc_steam_buf", 0, alloc_buf_size, VEDU_MMZ_ALIGN, stream_mmz_buf);

    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("hi_drv_venc_alloc_and_map failed\n");
    }

    return ret;
}

static hi_s32 alloc_para_set_buf(venc_buffer* slice_mmz_buf)
{
    hi_s32 ret;
    const hi_u32 bit_buf_size = 4096;

    ret = hi_drv_venc_mmz_alloc_and_map("venc_para_set_buf", 0, bit_buf_size, VEDU_MMZ_ALIGN, slice_mmz_buf);

    if (ret != HI_SUCCESS) {
        HI_ERR_VENC("para_set_buf hi_drv_venc_alloc_and_map failed\n");
    }

    return ret;
}

hi_s32 list_buf_alloc_para_set_buf_addr(hi_handle buf, venc_buffer *para_set_buf, hi_u32 *para_set_addr)
{
    const hi_u32 bit_buf_size = 4096;

    LISTBUF_ASSERT_RETURN(para_set_buf != HI_NULL, HI_ERR_LISTBUF_INVALID_PARA);
    LISTBUF_ASSERT_RETURN(para_set_addr != HI_NULL, HI_ERR_LISTBUF_INVALID_PARA);

    if (alloc_para_set_buf(para_set_buf) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    memset_s(para_set_buf->start_vir_addr, bit_buf_size, 0, bit_buf_size);
    *para_set_addr = para_set_buf->start_phy_addr;

    return HI_SUCCESS;
}

hi_s32 list_buf_rls_para_set_buf_addr(hi_handle buf, venc_buffer* para_set_buf)
{
    LISTBUF_ASSERT_RETURN(para_set_buf != HI_NULL, HI_ERR_LISTBUF_INVALID_PARA);

    hi_drv_venc_mmz_unmap_and_release(para_set_buf);

    return HI_SUCCESS;
}

hi_s32 list_buf_reset_buffer(hi_handle buf, hi_u32 protocol)
{
    return HI_SUCCESS;
}

hi_s32 list_buf_get_release_stream_info_intar(hi_handle buf, hi_venc_stream *stream, vedu_efl_nalu* vedu_packet)
{
    return HI_SUCCESS;
}

#ifdef VENC_SUPPORT_JPGE
hi_s32 list_buf_write_header(hi_handle handle, strm_mng_input_info* input_info, strm_mng_stream_info* stream_info)
{
    return HI_SUCCESS;
}

hi_s32 list_buf_put_jpeg_write_buffer(hi_handle handle, strm_mng_input_info* input_info,
    strm_mng_stream_info* stream_info)
{
    return HI_SUCCESS;
}

hi_s32 list_buf_jpeg_remove_header(hi_handle handle, strm_mng_input_info* input_info,
    strm_mng_stream_info* stream_info)
{
    return HI_SUCCESS;
}

hi_s32 list_buf_get_jpeg_stream(hi_handle handle, strm_mng_stream_info* stream_info, vedu_efl_nalu* vedu_packet,
                             hi_venc_stream* stream, venc_ioctl_buf_offset* buf_offset)
{
    return HI_SUCCESS;
}
#endif

