/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "stream.h"
#include "dbg.h"
#include "hi_drv_ssm.h"

#define STREAM_BUFFER_POOL_SIZE 10
#define STREAM_BUFFER_NAME      "es_buffer"
#define STREAM_IDR_PTS_THD      40000  /* 40ms */
#define STREAM_INVALID_PTS      0xFFFFFFFF

#define STREAM_FIND_TAIL_BLOCK(inst, block) { \
        block = OS_LIST_ENTRY(inst->list.prev, stream_block, node); \
}

#define STREAM_FIND_HEAD_BLOCK(inst, block) { \
        block = OS_LIST_ENTRY(inst->list.next, stream_block, node); \
}

#define STREAM_FIND_FIRST_FREE_BLOCK(inst, block) { \
        stream_block *tmp = HI_NULL; \
        struct OS_LIST_HEAD *list = inst->list.next; \
        while (list != &(inst->list)) { \
            tmp = OS_LIST_ENTRY(list, stream_block, node); \
            if (tmp->status == STREAM_STATUS_FREE) { \
                block = tmp; \
                break; \
            } \
            list = list->next; \
        } \
}

#define STREAM_FIND_BLOCK_BY_ADDR(addr, inst, block) { \
        struct OS_LIST_HEAD *pos = HI_NULL; \
        struct OS_LIST_HEAD *n = HI_NULL; \
        stream_block *tmp = HI_NULL; \
        OS_LIST_FOR_EACH_SAFE(pos, n, &inst->list) { \
            tmp = OS_LIST_ENTRY(pos, stream_block, node); \
            if (addr == tmp->buf.phy_addr) { \
                block = tmp; \
                break; \
            } \
        } \
}

typedef struct {
    stream_chan chan[VFMW_CHAN_NUM];
    hi_bool init;
    fun_vfmw_event_report event_report_vdec;
    fun_vfmw_event_report event_report_omxvdec;
} stream_entry;

static stream_entry g_stream_entry = {{{0}}, 0, 0, 0};

static stream_entry *stream_get_entry(hi_void)
{
    return &g_stream_entry;
}

stream_chan *stream_get_chan(hi_u32 chan_id)
{
    stream_entry *entry = stream_get_entry();

    return &entry->chan[chan_id];
}

static hi_void stream_lock_chan(hi_u32 chan_id)
{
    stream_chan *chan = stream_get_chan(chan_id);
    OS_SEMA_DOWN(chan->sema);
}

static hi_void stream_unlock_chan(hi_u32 chan_id)
{
    stream_chan *chan = stream_get_chan(chan_id);
    OS_SEMA_UP(chan->sema);
}

static hi_void stream_free_list(stream_chan *chan)
{
    stream_block *block = HI_NULL;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;

    if (!OS_LIST_EMPTY(&chan->list)) {
        OS_LIST_FOR_EACH_SAFE(pos, n, &chan->list) {
            block = OS_LIST_ENTRY(pos, stream_block, node);
            if (chan->mode == STREAM_BUF_ALLOC_OUTER) {
                OS_PUT_PHY(UINT64_PTR(block->buf.dma_buf), block->buf.phy_addr, chan->is_sec);
                OS_PUT_DMA_BUF(UINT64_PTR(block->buf.dma_buf));
            }
            OS_LIST_DEL(pos);
            block->is_used = HI_FALSE;
        }
    }

    return;
}

static stream_block *stream_get_manger_block(stream_chan *chan)
{
    hi_s32 i = 0;
    stream_block *block = HI_NULL;

    if (chan->block == HI_NULL) {
        chan->block = (stream_block **)OS_ALLOC_VIR("stream_block_pool", sizeof(stream_block *) * (chan->max_raw_num));
        if (chan->block == HI_NULL) {
            return HI_NULL;
        }
        VFMW_CHECK_SEC_FUNC(memset_s(chan->block, sizeof(stream_block *) * (chan->max_raw_num),
                                     HI_NULL, sizeof(stream_block *) * (chan->max_raw_num)));
    }

    for (i = 0; i < chan->max_raw_num; i++) {
        if (chan->block[i]  == HI_NULL) {
            break;
        }

        if (chan->block[i]->is_used == HI_FALSE) {
            block = chan->block[i];
            break;
        }
    }

    if (block == HI_NULL && i < chan->max_raw_num) {
        block = OS_ALLOC_VIR("stream_block", sizeof(stream_block));
        if (block != HI_NULL) {
            chan->block[i] = block;
        }
    }

    if (block != HI_NULL) {
        block->is_used = HI_TRUE;
        chan->raw_num++;
    }

    return block;
}

static hi_void stream_put_manger_block(stream_chan *chan, stream_block *block)
{
    block->is_used = HI_FALSE;
    chan->raw_num--;
}

static hi_void stream_free_manger_block(stream_chan *chan)
{
    hi_s32 i = 0;

    if (chan->block != HI_NULL) {
        for (i = 0; i < chan->max_raw_num; i++) {
            if (chan->block[i]  == HI_NULL) {
                break;
            }

            OS_FREE_VIR(chan->block[i]);
            chan->block[i] = HI_NULL;
        }

        OS_FREE_VIR(chan->block);
        chan->block = HI_NULL;
    }

    chan->raw_num = 0;
}

static hi_s32 stream_deq_buffer_internal(stream_chan *chan, vfmw_stream_buf *buffer)
{
    hi_u32 read_offset = 0;
    hi_u32 write_offset = 0;
    stream_block *block_tail = HI_NULL;
    stream_block *block_head = HI_NULL;
    stream_block *block = HI_NULL;
    hi_bool alloc = HI_FALSE;
    hi_u32 tail_free;
    hi_u32 head_free;

    block = stream_get_manger_block(chan);
    if (block == HI_NULL) {
        dprint(PRN_ERROR, "alloc stream block fail!\n");
        return HI_FAILURE;
    }

    if (chan->is_used == HI_FALSE || chan->start_phy_addr == 0) {
        stream_put_manger_block(chan, block);
        dprint(PRN_ERROR, "%s %d chan is not create!\n", __func__, __LINE__);
        return HI_FAILURE;
    }

    if (chan->free_len < buffer->length) {
        stream_put_manger_block(chan, block);
        dprint(PRN_DBG, "es buffer is not enough free_len:%x raw_len:%x!\n",
               chan->free_len, buffer->length);
        return HI_FAILURE;
    }

    if (OS_LIST_EMPTY(&chan->list) == HI_TRUE) {
        read_offset = write_offset = 0;
    } else {
        STREAM_FIND_TAIL_BLOCK(chan, block_tail);
        if (block_tail->status == STREAM_STATUS_WRITE) {
            VFMW_CHECK_SEC_FUNC(memcpy_s(buffer, sizeof(vfmw_stream_buf), &block_tail->buf, sizeof(vfmw_stream_buf)));
            stream_put_manger_block(chan, block);
            return HI_SUCCESS;
        }
        write_offset = block_tail->buf.phy_addr + block_tail->buf.length  - chan->start_phy_addr;

        STREAM_FIND_HEAD_BLOCK(chan, block_head);
        read_offset = block_head->buf.phy_addr - chan->start_phy_addr;
    }

    if (write_offset <= read_offset) {
        alloc = HI_TRUE;
    } else {
        tail_free = chan->length - write_offset;
        head_free = chan->free_len - tail_free;

        if (tail_free >= buffer->length) {
            alloc = HI_TRUE;
        } else if (head_free >= buffer->length) {
            alloc = HI_TRUE;
            write_offset = 0;
            chan->freeze_size = tail_free;
            chan->free_len -= tail_free;
        } else {
            dprint(PRN_DBG, "head free:%x tail free:%x raw len:%x\n", head_free, tail_free, buffer->length);
        }
    }

    chan->write_offst = write_offset;
    chan->read_offst = read_offset;

    if (alloc != HI_TRUE) {
        stream_put_manger_block(chan, block);
        dprint(PRN_DBG, "not enough buf!\n");
        return HI_FAILURE;
    }

    buffer->phy_addr = block->buf.phy_addr = chan->start_phy_addr + write_offset;
    buffer->kern_vir_addr = block->buf.kern_vir_addr = (chan->start_kern_vir_addr == 0) ?
                                                       0 : chan->start_kern_vir_addr + write_offset;
    buffer->usr_vir_addr = block->buf.usr_vir_addr = (chan->start_usr_vir_addr == 0) ?
                                                     0 : (chan->start_usr_vir_addr + write_offset);
    buffer->length = block->buf.length = buffer->length;
    buffer->pts = block->buf.pts = VFMW_INVALID_PTS;
    buffer->is_end_of_frm = block->buf.is_end_of_frm = 0;
    buffer->is_dual_layer = block->buf.is_dual_layer = 0;
    buffer->offset = block->buf.offset = write_offset;
    buffer->handle = block->buf.handle = chan->mem_fd;

    block->status = STREAM_STATUS_WRITE;
    dprint(PRN_DBG, "get es buffer:%llx ker_vir:%llx usr_vir:%llx len:%x\n", buffer->phy_addr, buffer->kern_vir_addr,
           buffer->usr_vir_addr, buffer->length);

    OS_LIST_ADD_TAIL(&block->node, &chan->list);

    return HI_SUCCESS;
}

static hi_s32 stream_deq_buffer_outer(stream_chan *chan, vfmw_stream_buf *buffer)
{
    stream_block *head_block = HI_NULL;

    if (chan->is_used == HI_FALSE || OS_LIST_EMPTY(&chan->list) == HI_TRUE) {
        return HI_FAILURE;
    }

    STREAM_FIND_HEAD_BLOCK(chan, head_block);
    if (head_block->status != STREAM_STATUS_WRITE) {
        return HI_FAILURE;
    }

    VFMW_CHECK_SEC_FUNC(memcpy_s(buffer, sizeof(vfmw_stream_buf), &head_block->buf, sizeof(vfmw_stream_buf)));

    OS_PUT_PHY(UINT64_PTR(buffer->dma_buf), buffer->phy_addr, chan->is_sec);
    OS_PUT_DMA_BUF(UINT64_PTR(buffer->dma_buf));

    OS_LIST_DEL(&head_block->node);
    stream_put_manger_block(chan, head_block);

    return HI_SUCCESS;
}

static hi_s32 stream_que_buffer_internal(stream_chan *chan, vfmw_stream_buf *buffer)
{
    stream_block *block_tail = HI_NULL;
    UADDR phy_addr = buffer->phy_addr;

    if (chan->is_used == HI_FALSE) {
        return HI_FAILURE;
    }

    STREAM_FIND_TAIL_BLOCK(chan, block_tail);
    if (block_tail->buf.length < buffer->length) {
        return HI_FAILURE;
    }
    dprint(PRN_DBG, "put es buffer phy_addr:%llx len:%x\n", phy_addr, buffer->length);

    if (block_tail->status == STREAM_STATUS_WRITE &&
        (block_tail->buf.phy_addr == phy_addr || block_tail->buf.offset == buffer->offset)) {
        if (buffer->length == 0) {
            OS_LIST_DEL(&block_tail->node);
            stream_put_manger_block(chan, block_tail);
            return HI_SUCCESS;
        }

        block_tail->buf.is_dual_layer = buffer->is_dual_layer;
        block_tail->buf.is_end_of_frm = buffer->is_end_of_frm;
        block_tail->buf.pts = buffer->pts;
        block_tail->buf.length = buffer->length;
        block_tail->status = STREAM_STATUS_FREE;
    } else {
        return HI_FAILURE;
    }

    chan->free_len -= buffer->length;
    chan->used_len += buffer->length;

    return HI_SUCCESS;
}

static hi_s32 stream_que_buffer_outer(stream_chan *chan, vfmw_stream_buf *buffer)
{
    stream_block *block = HI_NULL;
    hi_void *dma_buf = HI_NULL;

    block = stream_get_manger_block(chan);
    if (block == HI_NULL) {
        return HI_FAILURE;
    }

    VFMW_CHECK_SEC_FUNC(memcpy_s(&block->buf, sizeof(vfmw_stream_buf), buffer, sizeof(vfmw_stream_buf)));

    block->status = STREAM_STATUS_FREE;

    if (chan->is_used == HI_FALSE) {
        stream_put_manger_block(chan, block);
        return HI_FAILURE;
    }

    dma_buf = OS_GET_DMA_BUF(buffer->handle);
    if (dma_buf == HI_NULL) {
        stream_put_manger_block(chan, block);
        dprint(PRN_ERROR, "get_dma_buf fail!\n");
        return HI_FAILURE;
    }
    block->buf.dma_buf = PTR_UINT64(dma_buf);
    block->buf.phy_addr = OS_GET_PHY(dma_buf, chan->is_sec) + buffer->offset;

    OS_LIST_ADD_TAIL(&block->node, &chan->list);

    return HI_SUCCESS;
}

static hi_s32 stream_read_internal(stream_chan *chan, vfmw_stream_buf *buffer)
{
    stream_block *block = HI_NULL;

    if (chan->is_used == HI_FALSE || chan->used_len == 0 || OS_LIST_EMPTY(&chan->list)) {
        return HI_FAILURE;
    }

    STREAM_FIND_FIRST_FREE_BLOCK(chan, block);
    if (block == HI_NULL) {
        return HI_FAILURE;
    }

    buffer->phy_addr = block->buf.phy_addr;
    buffer->kern_vir_addr = block->buf.kern_vir_addr;
    buffer->usr_vir_addr = block->buf.usr_vir_addr;

    dprint(PRN_DBG, "read raw phy_addr:%llx vir_addr:%llx\n", buffer->phy_addr, buffer->kern_vir_addr);

    buffer->pts = block->buf.pts;
    buffer->length = block->buf.length;
    buffer->marker = block->buf.marker;
    buffer->is_dual_layer = block->buf.is_dual_layer;
    buffer->is_end_of_frm = block->buf.is_end_of_frm;
    buffer->handle = block->buf.handle;
    buffer->offset = block->buf.offset;

    block->status = STREAM_STATUS_READ;

    return HI_SUCCESS;
}

static hi_s32 stream_read_outer(stream_chan *chan, vfmw_stream_buf *buffer)
{
    stream_block *block = HI_NULL;

    if (chan->is_used == HI_FALSE || OS_LIST_EMPTY(&chan->list) == HI_TRUE) {
        return HI_FAILURE;
    }

    STREAM_FIND_FIRST_FREE_BLOCK(chan, block);
    if (block == HI_NULL) {
        return HI_FAILURE;
    }

    buffer->phy_addr = block->buf.phy_addr;
    buffer->kern_vir_addr = block->buf.kern_vir_addr;

    buffer->pts = block->buf.pts;
    buffer->length = block->buf.length;
    buffer->marker = block->buf.marker;
    buffer->is_dual_layer = block->buf.is_dual_layer;
    buffer->is_end_of_frm = block->buf.is_end_of_frm;
    buffer->disp_enable_flag = block->buf.disp_enable_flag;
    buffer->disp_frm_distance = block->buf.disp_frm_distance;
    buffer->disp_time = block->buf.disp_time;
    buffer->distance_before_first_frame = block->buf.distance_before_first_frame;
    buffer->gop_num = block->buf.gop_num;

    block->status = STREAM_STATUS_READ;

    return HI_SUCCESS;
}

static hi_s32 stream_release_internal(stream_chan *chan, vfmw_stream_buf *buffer)
{
    stream_block *block = HI_NULL;
    UADDR phy_addr = 0;

    if (chan->is_used == HI_FALSE) {
        return HI_FAILURE;
    }

    phy_addr = buffer->phy_addr;
    dprint(PRN_DBG, "release raw phy_addr:%llx len:%x\n", phy_addr, buffer->length);

    STREAM_FIND_BLOCK_BY_ADDR(phy_addr, chan, block);
    if (block == HI_NULL) {
        dprint(PRN_ERROR, "can not find block!\n");
        return HI_FAILURE;
    }

    if (chan->freeze_size != 0 &&
        block->buf.phy_addr + block->buf.length == chan->start_phy_addr + chan->length - chan->freeze_size) {
        chan->free_len += chan->freeze_size;
        chan->freeze_size = 0;
    }

    if (block->status != STREAM_STATUS_READ) {
        dprint(PRN_ERROR, "block state is %d!\n", block->status);
        return HI_FAILURE;
    }

    chan->used_len -= block->buf.length;
    chan->free_len += block->buf.length;

    OS_LIST_DEL(&block->node);
    stream_put_manger_block(chan, block);

    dprint(PRN_DBG, "length:%x free:%x used:%x freeze:%x\n", chan->length, chan->free_len, chan->used_len,
           chan->freeze_size);

    return HI_SUCCESS;
}

static hi_s32 stream_release_outer(stream_chan *chan, vfmw_stream_buf *buffer)
{
    stream_block *block = HI_NULL;
    UADDR phy_addr;

    phy_addr = buffer->phy_addr;

    if (chan->is_used == HI_FALSE) {
        return HI_FAILURE;
    }

    STREAM_FIND_BLOCK_BY_ADDR(phy_addr, chan, block);
    if (block == HI_NULL) {
        return HI_FAILURE;
    }

    if (block->status != STREAM_STATUS_READ) {
        return HI_FAILURE;
    }

    block->status = STREAM_STATUS_WRITE;

    return HI_SUCCESS;
}

hi_void stream_init(vfmw_init_param *param)
{
    hi_s32 i = 0;
    stream_chan *chan = HI_NULL;
    stream_entry *entry = stream_get_entry();

    if (entry->init == HI_FALSE) {
        for (i = 0; i < VFMW_CHAN_NUM; i++) {
            chan = stream_get_chan(i);
            OS_SEMA_INIT(&chan->sema);
            chan->chan_id = i;
        }
        entry->init = HI_TRUE;
    }

    if (param->adpt_type == ADPT_TYPE_VDEC) {
        entry->event_report_vdec = param->fn_callback;
    } else {
        entry->event_report_omxvdec = param->fn_callback;
    }
}

hi_void stream_exit(hi_void)
{
    hi_s32 i = 0;
    stream_chan *chan = HI_NULL;
    stream_entry *entry = stream_get_entry();

    if (entry->init == HI_TRUE) {
        for (i = 0; i < VFMW_CHAN_NUM; i++) {
            chan = stream_get_chan(i);
            OS_SEMA_EXIT(chan->sema);
        }
        entry->init = HI_FALSE;
    }
}

hi_s32 stream_create_chan(hi_u32 chan_id, vfmw_chan_option *option)
{
    stream_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    if (chan->is_used == HI_TRUE) {
        stream_unlock_chan(chan_id);
        dprint(PRN_ERROR, "%s %d chan %d is busy!\n", __func__, __LINE__, chan_id);
        return HI_FAILURE;
    }

    chan->is_used = HI_TRUE;
    chan->chan_id = chan_id;
    chan->adpt_type = option->adpt_type;
    chan->mode = STREAM_BUF_ALLOC_OUTER;
    chan->freeze_size = 0;
    chan->free_len = 0;
    chan->length = 0;
    chan->used_len = 0;
    chan->start_phy_addr = 0;
    chan->start_kern_vir_addr = 0;
    chan->start_usr_vir_addr = 0;
    chan->is_sec = option->is_sec_mode;
    chan->vdec_handle = option->vdec_handle;
    chan->ssm_handle = option->ssm_handle;
    chan->try_read_cnt = 0;
    chan->try_release_cnt = 0;
    chan->read_ok_cnt = 0;
    chan->release_ok_cnt = 0;
    chan->try_dequeue_cnt = 0;
    chan->dequeue_ok_cnt = 0;
    chan->try_queue_cnt = 0;
    chan->queue_ok_cnt = 0;
    chan->eos_flag = HI_FALSE;
    chan->raw_num = 0;
    chan->block = HI_NULL;
    OS_LIST_INIT_HEAD(&chan->list);

    stream_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_void stream_destroy_chan(hi_u32 chan_id)
{
    stream_chan *chan = HI_NULL;
    vfmw_mem_desc mem = {0};

    VFMW_ASSERT(chan_id < VFMW_CHAN_NUM);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    chan->is_used = HI_FALSE;
    stream_free_list(chan);
    stream_free_manger_block(chan);

    if (chan->start_phy_addr != 0) {
        mem.phy_addr = chan->start_phy_addr;
        mem.vir_addr = chan->start_kern_vir_addr;
        mem.length = chan->length;
        mem.dma_buf = chan->mem_dma_buf;
        mem.mode = chan->is_sec ? MEM_MMU_SEC : MEM_MMU_MMU;
        OS_KMEM_FREE(&mem);
        chan->start_phy_addr = 0;
        chan->start_kern_vir_addr = 0;
        chan->start_usr_vir_addr = 0;
        chan->length = 0;
    }

    stream_unlock_chan(chan_id);
}

hi_s32 stream_alloc_buffer(hi_u32 chan_id, hi_u64 *size)
{
    hi_s32 ret;
    vfmw_mem_desc mem = {0};
    stream_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET((*size) > 0, HI_FAILURE);

    chan = stream_get_chan(chan_id);
    mem.mode = chan->is_sec ? MEM_MMU_SEC : MEM_MMU_MMU;
    mem.vdec_handle = chan->vdec_handle;
    mem.ssm_handle = chan->ssm_handle;

    ret = OS_KMEM_ALLOC(STREAM_BUFFER_NAME, (*size), 0, 0, &mem);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "%s %d alloc es buffer fail!\n", __func__, __LINE__);
        return HI_FAILURE;
    }

    stream_lock_chan(chan_id);

    if (chan->is_used == HI_FALSE) {
        stream_unlock_chan(chan_id);
        OS_KMEM_FREE(&mem);
        dprint(PRN_ERROR, "%s %d chan %d is not create!\n", __func__, __LINE__, chan_id);
        return HI_FAILURE;
    }
    chan->start_phy_addr = mem.phy_addr;
    chan->start_kern_vir_addr = mem.vir_addr;
    chan->length = mem.length;
    chan->mem_dma_buf = mem.dma_buf;
    chan->mem_fd = OS_GET_MEM_FD(UINT64_PTR(chan->mem_dma_buf));
    chan->write_offst = 0;
    chan->read_offst = 0;
    chan->free_len = mem.length;
    chan->used_len = 0;
    chan->freeze_size = 0;
    chan->mode = STREAM_BUF_ALLOC_INNER;
    chan->eos_flag = HI_FALSE;
    (*size) = PTR_UINT64(mem.dma_buf);
    dprint(PRN_DBG, "alloc es buffer phy_addr:%llx vir_addr:%llx len:%x\n", chan->start_phy_addr,
           chan->start_kern_vir_addr,
           chan->length);

    stream_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 stream_free_buffer(hi_u32 chan_id)
{
    stream_chan *chan = HI_NULL;
    vfmw_mem_desc mem = {0};

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    if (chan->is_used == HI_FALSE) {
        stream_unlock_chan(chan_id);
        dprint(PRN_ERROR, "%s %d chan %d is not create!\n", __func__, __LINE__, chan_id);
        return HI_FAILURE;
    }
    if (chan->start_phy_addr != 0) {
        mem.phy_addr = chan->start_phy_addr;
        mem.vir_addr = chan->start_kern_vir_addr;
        mem.length = chan->length;
        mem.dma_buf = chan->mem_dma_buf;
        mem.mode = chan->is_sec ? MEM_MMU_SEC : MEM_MMU_MMU;
        OS_PUT_MEM_FD(chan->mem_fd);
        OS_KMEM_FREE(&mem);
        chan->start_phy_addr = 0;
        chan->start_kern_vir_addr = 0;
        chan->start_usr_vir_addr = 0;
        chan->length = 0;
    }

    stream_free_list(chan);
    stream_free_manger_block(chan);

    stream_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 stream_bind_usr_addr(hi_u32 chan_id, hi_u64 usr_addr)
{
    stream_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    if (chan->is_used == HI_FALSE) {
        stream_unlock_chan(chan_id);
        dprint(PRN_ERROR, "%s %d chan %d is not create!\n", __func__, __LINE__, chan_id);
        return HI_FAILURE;
    }

    chan->start_usr_vir_addr = usr_addr;
    dprint(PRN_DBG, "bind user es buffer:%llx\n", chan->start_usr_vir_addr);

    stream_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 stream_dequeue_buffer(hi_u32 chan_id, vfmw_stream_buf *raw_buf)
{
    hi_s32 ret = HI_FAILURE;
    stream_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(raw_buf != HI_NULL, HI_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    chan->try_dequeue_cnt++;

    if (chan->mode == STREAM_BUF_ALLOC_INNER) {
        ret = stream_deq_buffer_internal(chan, raw_buf);
    } else {
        ret = stream_deq_buffer_outer(chan, raw_buf);
    }

    if (ret == HI_SUCCESS) {
        chan->dequeue_ok_cnt++;
    }

    stream_unlock_chan(chan_id);

    return ret;
}

hi_s32 stream_queue_buffer(hi_u32 chan_id, vfmw_stream_buf *raw_buf)
{
    hi_s32 ret = HI_FAILURE;
    stream_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(raw_buf != HI_NULL, HI_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    chan->try_queue_cnt++;

    if (chan->mode == STREAM_BUF_ALLOC_INNER) {
        ret = stream_que_buffer_internal(chan, raw_buf);
    } else {
        ret = stream_que_buffer_outer(chan, raw_buf);
    }

    if (ret == HI_SUCCESS) {
        chan->queue_ok_cnt++;
    }

    stream_unlock_chan(chan_id);

    return ret;
}

hi_s32 stream_read_buffer(hi_u32 chan_id, vfmw_stream_buf *buffer)
{
    hi_s32 ret = HI_FAILURE;
    stream_chan *chan = HI_NULL;
    hi_s64 target_pts;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(buffer != HI_NULL, HI_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    chan->try_read_cnt++;
    if (chan->mode == STREAM_BUF_ALLOC_INNER) {
        ret = stream_read_internal(chan, buffer);
    } else {
        ret = stream_read_outer(chan, buffer);
    }

    if (ret != HI_SUCCESS) {
        if (chan->eos_flag == HI_TRUE) {
            chan->eos_flag = HI_FALSE;
            chan->read_ok_cnt++;
            stream_unlock_chan(chan_id);
            return STREAM_EOS;
        }
        stream_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    if (chan->en_idr_pts == HI_TRUE) {
        target_pts = chan->idr_pts > STREAM_IDR_PTS_THD ? chan->idr_pts - STREAM_IDR_PTS_THD : chan->idr_pts;
        if (buffer->pts >= target_pts && buffer->pts != VFMW_INVALID_PTS) {
            chan->en_idr_pts = HI_FALSE;
        } else {
            if (chan->mode == STREAM_BUF_ALLOC_OUTER) {
                stream_release_outer(chan, buffer);
            } else {
                stream_release_internal(chan, buffer);
            }
            stream_unlock_chan(chan_id);
            return HI_FAILURE;
        }
    }

    chan->read_ok_cnt++;

    stream_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 stream_release_buffer(hi_u32 chan_id, vfmw_stream_buf *buffer)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 evt_ret = HI_SUCCESS;
    stream_chan *chan = HI_NULL;
    stream_entry *entry = stream_get_entry();

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(buffer != HI_NULL, HI_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    chan->try_release_cnt++;

    if (chan->mode == STREAM_BUF_ALLOC_INNER) {
        ret = stream_release_internal(chan, buffer);
    } else {
        ret = stream_release_outer(chan, buffer);
    }

    if (ret == HI_SUCCESS) {
        chan->release_ok_cnt++;
        if (chan->adpt_type == ADPT_TYPE_VDEC && entry->event_report_vdec != HI_NULL &&
            (chan->evt_map & EVNT_RLS_STREAM) != 0) {
            evt_ret = entry->event_report_vdec(chan_id, EVNT_RLS_STREAM, buffer, sizeof(vfmw_stream_buf));
        } else if (entry->event_report_omxvdec != HI_NULL && (chan->evt_map & EVNT_RLS_STREAM) != 0) {
            evt_ret = entry->event_report_omxvdec(chan_id, EVNT_RLS_STREAM, buffer, sizeof(vfmw_stream_buf));
        }

        if (evt_ret != HI_SUCCESS) {
            dprint(PRN_ERROR, "%s chan %d call event call baclk fail!\n", __func__, chan_id);
        }
    }

    stream_unlock_chan(chan_id);

    return ret;
}

hi_s32 stream_send_eos(hi_u32 chan_id)
{
    stream_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);

    stream_lock_chan(chan_id);
    chan = stream_get_chan(chan_id);
    chan->eos_flag = HI_TRUE;
    stream_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 stream_reset(hi_u32 chan_id)
{
    stream_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);

    stream_lock_chan(chan_id);
    chan = stream_get_chan(chan_id);
    stream_free_list(chan);
    chan->eos_flag = HI_FALSE;
    chan->freeze_size = 0;
    chan->free_len = chan->length;
    chan->used_len = 0;
    chan->read_offst = 0;
    chan->write_offst = 0;
    chan->try_read_cnt = 0;
    chan->try_release_cnt = 0;
    chan->read_ok_cnt = 0;
    chan->release_ok_cnt = 0;
    chan->raw_num = 0;
    stream_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 stream_get_es_info(hi_u32 chan_id, vfmw_stream_buf *raw_buf)
{
    stream_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(raw_buf != HI_NULL, HI_FAILURE);

    stream_lock_chan(chan_id);
    chan = stream_get_chan(chan_id);

    raw_buf->phy_addr = chan->start_phy_addr;
    raw_buf->length = chan->length;

    stream_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 stream_attach_es_buf(hi_u32 chan_id, vfmw_stream_buf *raw_buf)
{
#ifdef VFMW_SEC_SUPPORT
    hi_s32 ret;
    stream_chan *chan = HI_NULL;
    hi_drv_ssm_buf_attach_info attach_info;
    hi_u64 sec_info_addr;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(raw_buf != HI_NULL, HI_FAILURE);

    stream_lock_chan(chan_id);
    chan = stream_get_chan(chan_id);
    attach_info.module_handle = chan->vdec_handle;
    attach_info.session_handle = chan->ssm_handle;
    stream_unlock_chan(chan_id);

    attach_info.buf_id = HI_SSM_BUFFER_ID_DMX_VID_ES_BUF;
    attach_info.dma_buf_addr = UINT64_PTR(raw_buf->dma_buf);

    dprint(PRN_DBG, "vdec %x ssm %x buf %x fd %llx dma %llx\n",
           attach_info.module_handle, attach_info.session_handle,
           attach_info.buf_id, raw_buf->handle, attach_info.dma_buf_addr);

    ret = hi_drv_ssm_attach_buffer(attach_info, &sec_info_addr);
    if (ret != HI_SUCCESS) {
        dprint(PRN_ERROR, "vfmw attach esbuf fail\n");
    }
#endif

    return HI_SUCCESS;
}

hi_s32 stream_set_idr_pts(hi_u32 chan_id, hi_s64 idr_pts)
{
    stream_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);

    stream_lock_chan(chan_id);
    chan = stream_get_chan(chan_id);
    chan->en_idr_pts = HI_TRUE;
    chan->idr_pts = idr_pts;

    stream_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 stream_get_chan_status(hi_u32 chan_id, vfmw_chan_info *status)
{
    stream_chan *chan = HI_NULL;
    stream_block *block = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(status != HI_NULL, HI_FAILURE);

    stream_lock_chan(chan_id);
    chan = stream_get_chan(chan_id);

    status->is_end_of_stm = chan->eos_flag;
    status->free_stm_buf_size = chan->free_len;
    status->total_stm_buf_size = chan->length;
    status->used_stm_buf_size = chan->used_len;
    status->undecode_stm_num = 0;
    status->undecode_stm_size = 0;

    if (OS_LIST_EMPTY(&chan->list) != HI_TRUE) {
        OS_LIST_FOR_EACH_ENTRY(block, &chan->list, node) {
            if (block->status == STREAM_STATUS_FREE || block->status == STREAM_STATUS_READ) {
                status->undecode_stm_num++;
                status->undecode_stm_size += block->buf.length;
            }
        }
    }

    stream_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 stream_set_evt_map(hi_u32 chan_id, hi_u32 evt_map)
{
    stream_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    chan->evt_map = evt_map;

    stream_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 stream_config(hi_u32 chan_id, vfmw_chan_cfg *cfg)
{
    stream_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(cfg != HI_NULL, HI_FAILURE);

    stream_lock_chan(chan_id);

    chan = stream_get_chan(chan_id);
    chan->max_raw_num = cfg->max_raw_packet_num;

    stream_unlock_chan(chan_id);

    return HI_SUCCESS;
}
