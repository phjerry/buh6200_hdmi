/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "dbg.h"
#include "usdpool.h"

#define USRDTA_DEFAULR_MEM_POOL_SIZE (4 * 1024)
#define USERDATA_TYPE_INDEX          4
#define USERDATA_IDENTIFIER_DVB1     0x34394147
#define USERDATA_IDENTIFIER_AFD      0x31475444
#define USERDATA_TYPE_DVB1_CC        0x03
#define USERDATA_TYPE_DVB1_BAR       0x06

typedef struct {
    hi_u64 vir_addr;
    hi_vdec_user_data *usr;
    hi_u32 pool_index;
} usrdat_mem_desc;

typedef struct {
    usrdat_chan *chan[VFMW_CHAN_NUM];
    OS_SEMA chan_sema[VFMW_CHAN_NUM];
    hi_bool init;
} usrdat_entry;

static usrdat_entry g_usrdat_entry = {{0}, {0}, 0};

static usrdat_entry *usrdat_get_entry(hi_void)
{
    return &g_usrdat_entry;
}

usrdat_chan *usrdat_get_chan(hi_s32 chan_id)
{
    return g_usrdat_entry.chan[chan_id];
}

static hi_void usrdat_set_chan(hi_s32 chan_id, usrdat_chan *chan)
{
    g_usrdat_entry.chan[chan_id] = chan;
}

static hi_void usrdat_lock_chan(hi_s32 chan_id)
{
    usrdat_entry *entry = usrdat_get_entry();

    OS_SEMA_DOWN(entry->chan_sema[chan_id]);
}

static hi_void usrdat_unlock_chan(hi_s32 chan_id)
{
    usrdat_entry *entry = usrdat_get_entry();

    OS_SEMA_UP(entry->chan_sema[chan_id]);
}

static hi_void usrdat_find_tail_block(usrdat_pool *pool, usrdat_block **block)
{
    (*block) = OS_LIST_ENTRY(pool->list.prev, usrdat_block, node);
}

static hi_void usrdat_find_head_block(usrdat_pool *pool, usrdat_block **block)
{
    (*block) = OS_LIST_ENTRY(pool->list.next, usrdat_block, node);
}

static hi_void usrdat_find_first_free_block(usrdat_pool *pool, usrdat_block **block)
{
    usrdat_block *tmp = HI_NULL;
    struct OS_LIST_HEAD *list = pool->list.next;

    while (list != &(pool->list)) {
        tmp = OS_LIST_ENTRY(list, usrdat_block, node);
        if (tmp->state == USRDAT_BLK_STA_FREE) {
            (*block) = tmp;
            break;
        }

        list = list->next;
    }
}

static hi_void usrdat_find_block_by_addr(hi_u32 offset, usrdat_pool *pool, usrdat_block **block)
{
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;
    usrdat_block *tmp = HI_NULL;

    OS_LIST_FOR_EACH_SAFE(pos, n, &pool->list) {
        tmp = OS_LIST_ENTRY(pos, usrdat_block, node);
        if (offset == tmp->usr_data.usd_offset) {
            (*block) = tmp;
            break;
        }
    }
}

static hi_void usrdat_init_chan(usrdat_chan *chan)
{
    hi_s32 i = 0;

    VFMW_CHECK_SEC_FUNC(memset_s(chan, sizeof(usrdat_chan), 0, sizeof(usrdat_chan)));

    for (i = 0; i < USRDAT_TYPE_NUM; i++) {
        OS_LIST_INIT_HEAD(&chan->pool[i].list);
    }
}

static hi_void usrdat_free_pool_list(usrdat_pool *pool)
{
    usrdat_block *block = HI_NULL;
    struct OS_LIST_HEAD *pos = HI_NULL;
    struct OS_LIST_HEAD *n = HI_NULL;

    if (!OS_LIST_EMPTY(&pool->list)) {
        OS_LIST_FOR_EACH_SAFE(pos, n, &pool->list) {
            block = OS_LIST_ENTRY(pos, usrdat_block, node);
            OS_LIST_DEL(pos);
            block->is_used = HI_FALSE;
        }
    }

    pool->write_offset = 0;
    pool->read_offset = 0;
    pool->freeze_len = 0;
    pool->is_over_flow = HI_FALSE;
    pool->free_len = pool->length;
    pool->usrdata_num = 0;

    return;
}

static usrdat_block *usrdat_get_manger_block(usrdat_pool *pool)
{
    hi_s32 i = 0;
    usrdat_block *block = HI_NULL;

    for (i = 0; i < USRDAT_MAX_KEEP_NUM; i++) {
        if (pool->block[i]  == HI_NULL) {
            break;
        }

        if (pool->block[i]->is_used == HI_FALSE) {
            block = pool->block[i];
            break;
        }
    }

    if (block == HI_NULL && i < USRDAT_MAX_KEEP_NUM) {
        block = OS_ALLOC_VIR("usrdat_block", sizeof(usrdat_block));
        if (block != HI_NULL) {
            pool->block[i] = block;
        }
    }

    if (block != HI_NULL) {
        block->is_used = HI_TRUE;
        pool->usrdata_num++;
    }

    return block;
}

static hi_void usrdat_put_manger_block(usrdat_pool *pool, usrdat_block *block)
{
    block->is_used = HI_FALSE;
    pool->usrdata_num--;
}

static hi_void usrdat_free_manger_block(usrdat_chan *chan)
{
    hi_s32 i = 0;
    hi_s32 j = 0;
    usrdat_pool *pool = HI_NULL;

    for (j = 0; j < USRDAT_TYPE_NUM; j++) {
        pool = &chan->pool[j];
        for (i = 0; i < USRDAT_MAX_KEEP_NUM; i++) {
            if (pool->block[i]  == HI_NULL) {
                break;
            }

            OS_FREE_VIR(pool->block[i]);
            pool->block[i] = HI_NULL;
        }
    }
}

static hi_void usrdat_clear_pool_block(usrdat_pool *pool)
{
    hi_u32 i = 0;

    for (i = 0; i < USRDAT_MAX_KEEP_NUM; i++) {
        if (pool->block[i] != HI_NULL) {
            pool->block[i]->is_used = HI_FALSE;
        }
    }

    pool->is_over_flow = HI_TRUE;
}

static hi_void usrdat_clear_pool(usrdat_pool *pool)
{
    usrdat_free_pool_list(pool);
    pool->read_offset = 0;
    pool->write_offset = 0;
    pool->free_len = pool->length;
    pool->freeze_len = 0;
    pool->is_over_flow = HI_TRUE;
}

static hi_s32 usrdat_get_write_offset(usrdat_pool *pool, hi_u32 *woffset, hi_u32 size)
{
    hi_u32 read_offset = 0;
    hi_u32 write_offset = 0;
    usrdat_block *block_tail = HI_NULL;
    usrdat_block *block_head = HI_NULL;
    hi_bool alloc = HI_FALSE;
    hi_u32 tail_free;
    hi_u32 head_free;

    if (OS_LIST_EMPTY(&pool->list) == HI_TRUE) {
        read_offset = write_offset = 0;
    } else {
        usrdat_find_tail_block(pool, &block_tail);
        if (block_tail->state == USRDAT_BLK_STA_WRITE) {
            return HI_FAILURE;
        }
        write_offset = block_tail->usr_data.usd_offset + block_tail->usr_data.user_data_len;

        usrdat_find_head_block(pool, &block_head);
        read_offset = block_head->usr_data.usd_offset;
    }

    if (write_offset <= read_offset) {
        alloc = HI_TRUE;
    } else {
        tail_free = pool->length - write_offset;
        head_free = pool->free_len - tail_free;

        if (tail_free >= size) {
            alloc = HI_TRUE;
        } else if (head_free >= size) {
            alloc = HI_TRUE;
            write_offset = 0;
            pool->freeze_len = tail_free;
            pool->free_len -= tail_free;
        } else {
            dprint(PRN_DBG, "head free:%x tail free:%x raw len:%x\n", head_free, tail_free, size);
        }
    }

    pool->write_offset = write_offset;
    pool->read_offset = read_offset;

    if (alloc != HI_TRUE) {
        dprint(PRN_DBG, "not enough buf!\n");
        return HI_FAILURE;
    }

    *woffset = write_offset;

    return HI_SUCCESS;
}

static hi_s32 usrdat_get_write_buf(usrdat_chan *chan, hi_u32 size, usrdat_mem_desc *mem)
{
    hi_u32 write_offset = 0;
    usrdat_block *block = HI_NULL;
    hi_s32 ret;
    usrdat_pool *pool = &chan->pool[mem->pool_index];

    if (pool->start_vir_addr == 0) {
        return HI_FAILURE;
    }

    if (pool->free_len < size || pool->usrdata_num >= USRDAT_MAX_KEEP_NUM) {
        usrdat_clear_pool(pool);
        usrdat_clear_pool_block(pool);
    }

    ret = usrdat_get_write_offset(pool, &write_offset, size);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    block = usrdat_get_manger_block(pool);
    if (block == HI_NULL) {
        dprint(PRN_DBG, "alloc usrdat block fail!\n");
        return HI_FAILURE;
    }

    block->usr_data.usd_offset = write_offset;
    block->usr_data.user_data_len = size;
    block->usr_data.is_over_flow = pool->is_over_flow;
    block->state = USRDAT_BLK_STA_WRITE;

    mem->vir_addr = pool->start_vir_addr + write_offset;
    mem->usr = &block->usr_data;
    pool->is_over_flow = HI_FALSE;

    dprint(PRN_DBG, "get buffer pool_index %d offset %x len:%x\n", mem->pool_index, write_offset, size);

    OS_LIST_ADD_TAIL(&block->node, &pool->list);

    return HI_SUCCESS;
}

static hi_s32 usrdat_put_write_buf(usrdat_chan *chan, usrdat_mem_desc *mem)
{
    usrdat_block *block_tail = HI_NULL;
    usrdat_pool *pool = &chan->pool[mem->pool_index];

    usrdat_find_tail_block(pool, &block_tail);

    dprint(PRN_DBG, "put buffer pool_index %d offset:%llx len:%x\n",
           mem->pool_index, mem->usr->usd_offset, mem->usr->user_data_len);

    if (block_tail->state == USRDAT_BLK_STA_WRITE && block_tail->usr_data.usd_offset == mem->usr->usd_offset) {
        if (mem->usr->user_data_len == 0) {
            OS_LIST_DEL(&block_tail->node);
            usrdat_put_manger_block(pool, block_tail);
            return HI_SUCCESS;
        }
        block_tail->state = USRDAT_BLK_STA_FREE;
    } else {
        return HI_FAILURE;
    }
    pool->free_len -= mem->usr->user_data_len;

    return HI_SUCCESS;
}

static hi_vdec_usd_pos usrdat_get_usd_pos(vfmw_usd_type from)
{
    hi_vdec_usd_pos pos = HI_VDEC_USD_POS_UNKNOWN;

    switch (from) {
        case VFMW_USD_INVALID:
            pos = HI_VDEC_USD_POS_UNKNOWN;
            break;
        case VFMW_USD_MP2SEQ:
            pos = HI_VDEC_USD_POS_MPEG2_SEQ;
            break;
        case VFMW_USD_MP2GOP:
            pos = HI_VDEC_USD_POS_MPEG2_GOP;
            break;
        case VFMW_USD_MP2PIC:
            pos = HI_VDEC_USD_POS_MPEG2_FRM;
            break;
        case VFMW_USD_MP4VSOS:
            pos = HI_VDEC_USD_POS_MPEG4_VSOS;
            break;
        case VFMW_USD_MP4VSO:
            pos = HI_VDEC_USD_POS_MPEG4_VSO;
            break;
        case VFMW_USD_MP4VOL:
            pos = HI_VDEC_USD_POS_MPEG4_VOL;
            break;
        case VFMW_USD_MP4GOP:
            pos = HI_VDEC_USD_POS_MPEG4_GOP;
            break;
        case VFMW_USD_H264_REG:
            pos = HI_VDEC_USD_POS_H264_REG;
            break;
        case VFMW_USD_H264_UNREG:
            pos = HI_VDEC_USD_POS_H264_UNREG;
            break;
        case VFMW_USD_AVSSEQ:
            pos = HI_VDEC_USD_POS_AVS_SEQ;
            break;
        case VFMW_USD_AVSPIC:
            pos = HI_VDEC_USD_POS_AVS_FRM;
            break;
        default:
            pos = HI_VDEC_USD_POS_UNKNOWN;
            break;
    }

    return pos;
}

static hi_s32 usrdat_get_pool_index(hi_vdec_usd_type type)
{
    switch (type) {
        case HI_VDEC_USD_TYPE_DVB1_CC:

            return USRDAT_CC_INDEX;
        case HI_VDEC_USD_TYPE_AFD:
            return USRDAT_AFD_INDEX;
        case HI_VDEC_USD_TYPE_DVB1_BAR:
            return USRDAT_BAR_INDEX;
        default:
            return USRDAT_OTH_INDEX;
    }
}

static hi_void usrdat_free_chan_list(usrdat_chan *chan)
{
    hi_s32 i = 0;

    for (i = 0; i < USRDAT_TYPE_NUM; i++) {
        usrdat_free_pool_list(&chan->pool[i]);
    }

    chan->total_usd_num = 0;
    chan->try_acq_cnt = 0;
    chan->acq_ok_cnt = 0;
}

static hi_void usrdat_unbind_chan_mem(usrdat_chan *chan)
{
    mem_record mem_rec = {0};

    OS_PUT_PHY(UINT64_PTR(chan->dma_buf), chan->phy_addr, 0);
    mem_rec.dma_buf = chan->dma_buf;
    mem_rec.vir_addr = UINT64_PTR(chan->vir_addr);
    OS_UNMAP_MEM(&mem_rec);
    OS_PUT_DMA_BUF(UINT64_PTR(chan->dma_buf));
}

static hi_vdec_usd_type usrdat_get_type(vfmw_usrdat *usr)
{
    hi_u32 id;
    hi_u32 type;
    hi_vdec_usd_type usr_type;

    id = *((hi_u32 *)usr->data);
    type = usr->data[USERDATA_TYPE_INDEX];

    if (id == USERDATA_IDENTIFIER_DVB1 && type == USERDATA_TYPE_DVB1_CC) {
        usr_type = HI_VDEC_USD_TYPE_DVB1_CC;
    } else if (id == USERDATA_IDENTIFIER_AFD) {
        usr_type = HI_VDEC_USD_TYPE_AFD;
    } else {
        usr_type = HI_VDEC_USD_TYPE_UNKNOWN;
    }

    return usr_type;
}

static hi_void usrdat_set_desc(hi_vdec_user_data *usrdat, vfmw_usrdat *usr)
{
    usrdat->broadcast = HI_VDEC_BROADCAST_DVB;
    usrdat->type = usrdat_get_type(usr);
    usrdat->is_top_field_first = usr->top_field_first;
    usrdat->pts = usr->pts;
    usrdat->seq_cnt = usr->seq_cnt;
    usrdat->seq_frm_cnt = usr->seq_img_cnt;
    usrdat->position = usrdat_get_usd_pos(usr->from);
}

static hi_void usrdat_split_usd_buf(usrdat_chan *chan)
{
    hi_u32 i = 0;
    hi_u32 pool_size = chan->usd_pool_size / USRDAT_TYPE_NUM;

    for (i = 0; i < USRDAT_TYPE_NUM; i++) {
        chan->pool[i].dma_buf = chan->dma_buf;
        chan->pool[i].free_len = chan->pool[i].length = pool_size;
        chan->pool[i].start_phy_addr = chan->phy_addr + i * pool_size;
        chan->pool[i].start_vir_addr = chan->vir_addr + i * pool_size;
        chan->pool[i].base_offset = i * pool_size;
    }

    return;
}

static hi_s32 usrdat_release_block(hi_vdec_user_data *usr, usrdat_pool *pool)
{
    usrdat_block *block = HI_NULL;

    usrdat_find_block_by_addr(usr->usd_offset, pool, &block);
    if (block == HI_NULL) {
        dprint(PRN_ERROR, "can not find block!\n");
        return HI_FAILURE;
    }

    if (block->state != USRDAT_BLK_STA_READ) {
        dprint(PRN_ERROR, "block state is %d!\n", block->state);
        return HI_FAILURE;
    }

    if (pool->freeze_len != 0 &&
        block->usr_data.usd_offset + block->usr_data.user_data_len == pool->length - pool->freeze_len) {
        pool->free_len += pool->freeze_len;
        pool->freeze_len = 0;
    }

    pool->free_len += block->usr_data.user_data_len;

    OS_LIST_DEL(&block->node);
    usrdat_put_manger_block(pool, block);

    return HI_SUCCESS;
}

hi_s32 usrdat_init(hi_void)
{
    hi_s32 i;
    usrdat_entry *entry = usrdat_get_entry();

    if (entry->init == HI_FALSE) {
        for (i = 0; i < VFMW_CHAN_NUM; i++) {
            OS_SEMA_INIT(&entry->chan_sema[i]);
        }
        entry->init = HI_TRUE;
    }

    return HI_SUCCESS;
}

hi_s32 usrdat_exit(hi_void)
{
    hi_s32 i;
    usrdat_entry *entry = usrdat_get_entry();

    if (entry->init == HI_TRUE) {
        for (i = 0; i < VFMW_CHAN_NUM; i++) {
            OS_SEMA_EXIT(entry->chan_sema[i]);
        }
        entry->init = HI_FALSE;
    }

    return HI_SUCCESS;
}

hi_s32 usrdat_create_chan(hi_s32 chan_id)
{
    usrdat_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, HI_FAILURE);

    usrdat_lock_chan(chan_id);

    chan = usrdat_get_chan(chan_id);
    if (chan != HI_NULL) {
        dprint(PRN_ERROR, "chan is busy!\n");
        usrdat_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    chan = OS_ALLOC_VIR("usrdat chan", sizeof(usrdat_chan));
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "alloc usrdat chan fail!\n");
        usrdat_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    usrdat_init_chan(chan);
    usrdat_set_chan(chan_id, chan);

    usrdat_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 usrdat_destroy_chan(hi_s32 chan_id)
{
    usrdat_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, HI_FAILURE);

    usrdat_lock_chan(chan_id);

    chan = usrdat_get_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_DBG, "chan not create!\n");
        usrdat_unlock_chan(chan_id);
        return HI_SUCCESS;
    }

    usrdat_free_chan_list(chan);
    usrdat_free_manger_block(chan);
    usrdat_unbind_chan_mem(chan);

    OS_FREE_VIR(chan);

    usrdat_set_chan(chan_id, HI_NULL);

    usrdat_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 usrdat_reset_chan(hi_s32 chan_id)
{
    usrdat_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, HI_FAILURE);

    usrdat_lock_chan(chan_id);

    chan = usrdat_get_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "chan not create!\n");
        usrdat_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    usrdat_free_chan_list(chan);

    usrdat_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 usrdat_config_chan(hi_s32 chan_id, vfmw_chan_cfg *cfg)
{
    usrdat_chan *chan = HI_NULL;

    VFMW_ASSERT_RET(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, HI_FAILURE);

    usrdat_lock_chan(chan_id);

    chan = usrdat_get_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "usrdat chan %d not created!\n", chan_id);
        usrdat_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    chan->usd_pool_size = cfg->usd_pool_size;

    usrdat_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 usrdat_process(hi_s32 chan_id, vfmw_image *img)
{
    hi_s32 i = 0;
    usrdat_mem_desc mem = {0};
    hi_s32 ret = HI_FAILURE;
    usrdat_chan *chan = HI_NULL;
    vfmw_usrdat *usr = HI_NULL;
    hi_vdec_usd_type usr_type;

    VFMW_ASSERT_RET((chan_id >= 0 && chan_id < VFMW_CHAN_NUM), HI_FAILURE);

    usrdat_lock_chan(chan_id);

    chan = usrdat_get_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "usrdat chan %d not create!\n", chan_id);
        usrdat_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    for (i = 0; i < VFMW_MAX_USD_NUM; i++) {
        if (img->usrdat[i] != 0) {
            usr = (vfmw_usrdat *)(UINT64_PTR(img->usrdat[i]));

            usr_type = usrdat_get_type(usr);
            mem.pool_index = usrdat_get_pool_index(usr_type);
            ret = usrdat_get_write_buf(chan, usr->data_size, &mem);
            if (ret != HI_SUCCESS) {
                usrdat_unlock_chan(chan_id);
                return HI_FAILURE;
            }

            VFMW_CHECK_SEC_FUNC(memcpy_s(UINT64_PTR(mem.vir_addr), usr->data_size, usr->data, usr->data_size));

            usrdat_set_desc(mem.usr, usr);

            ret = usrdat_put_write_buf(chan, &mem);
            if (ret != HI_SUCCESS) {
                usrdat_unlock_chan(chan_id);
                return HI_FAILURE;
            }

            if ((chan->check_usr.write_index + 1) % USRDAT_MAX_CHAN_KEEP_NUM != chan->check_usr.read_index) {
                chan->check_usr.chan_usr[chan->check_usr.write_index] = mem.usr;
                chan->check_usr.write_index = (chan->check_usr.write_index + 1) % USRDAT_MAX_CHAN_KEEP_NUM;
            }

            dprint(PRN_DBG, "process userdata type %d", mem.usr->type);

            chan->total_usd_num++;
        }
    }

    usrdat_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 usrdat_acquire(hi_s32 chan_id, hi_void *usrdata)
{
    hi_vdec_user_data *usr = (hi_vdec_user_data *)usrdata;
    usrdat_block *block = HI_NULL;
    usrdat_chan *chan = HI_NULL;
    usrdat_pool *pool = HI_NULL;
    hi_s32 pool_index;

    VFMW_ASSERT_RET(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(usrdata != HI_NULL, HI_FAILURE);

    usrdat_lock_chan(chan_id);

    chan = usrdat_get_chan(chan_id);
    if (chan == HI_NULL) {
        usrdat_unlock_chan(chan_id);
        dprint(PRN_ERROR, "usrdat chan %d not create!\n", chan_id);
        return HI_FAILURE;
    }

    pool_index = usrdat_get_pool_index(usr->type);
    if (pool_index == USRDAT_INVALID_INDEX) {
        usrdat_unlock_chan(chan_id);
        dprint(PRN_ERROR, "usrdat pool index invalid!\n");
        return HI_FAILURE;
    }

    chan->try_acq_cnt++;

    pool = &chan->pool[pool_index];

    usrdat_find_first_free_block(pool, &block);
    if (block == HI_NULL) {
        usrdat_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    VFMW_CHECK_SEC_FUNC(memcpy_s(usrdata, sizeof(hi_vdec_user_data), &block->usr_data, sizeof(hi_vdec_user_data)));
    usr->fd = chan->fd;
    usr->usd_offset = pool->base_offset + usr->usd_offset;

    block->state = USRDAT_BLK_STA_READ;
    chan->acq_ok_cnt++;

    dprint(PRN_DBG, "get userdata offset:%x len:%x\n", usr->usd_offset, usr->user_data_len);

    usrdat_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 usrdat_release(hi_s32 chan_id, hi_void *usrdata)
{
    hi_vdec_user_data usr_tmp;
    hi_vdec_user_data *usr = &usr_tmp;
    usrdat_chan *chan = HI_NULL;
    usrdat_pool *pool = HI_NULL;
    hi_s32 pool_index;

    VFMW_ASSERT_RET(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(usrdata != HI_NULL, HI_FAILURE);

    usrdat_lock_chan(chan_id);

    chan = usrdat_get_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "usrdat chan %d not create!\n", chan_id);
        usrdat_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    if (memcpy_s(usr, sizeof(hi_vdec_user_data), usrdata, sizeof(hi_vdec_user_data)) != EOK) {
        dprint(PRN_ERROR, "memcpy_s fail!\n");
        usrdat_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    pool_index = usrdat_get_pool_index(usr->type);
    if (pool_index == USRDAT_INVALID_INDEX) {
        dprint(PRN_ERROR, "usrdat pool index invalid!\n");
        usrdat_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    pool = &chan->pool[pool_index];

    chan->try_rls_cnt++;

    usr->usd_offset -= pool->base_offset;

    if (usrdat_release_block(usr, pool) != HI_SUCCESS) {
        dprint(PRN_ERROR, "release block fail!\n");
        usrdat_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    chan->rls_ok_cnt++;

    dprint(PRN_DBG, "release userdata offset:%x len:%x\n", usr->usd_offset, usr->user_data_len);

    usrdat_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 usrdat_check(hi_s32 chan_id, hi_void *usrdata)
{
    hi_vdec_user_data *usr = (hi_vdec_user_data *)usrdata;
    usrdat_chan *chan = HI_NULL;
    usrdat_block *block = HI_NULL;
    usrdat_pool *pool = HI_NULL;
    hi_s32 pool_index;

    VFMW_ASSERT_RET(chan_id >= 0 && chan_id < VFMW_CHAN_NUM, HI_FAILURE);
    VFMW_ASSERT_RET(usrdata != HI_NULL, HI_FAILURE);

    usrdat_lock_chan(chan_id);

    chan = usrdat_get_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "usrdat chan %d not create!\n", chan_id);
        usrdat_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    chan->try_chk_cnt++;

    if (chan->check_usr.read_index == chan->check_usr.write_index) {
        dprint(PRN_ERROR, "chan %d userdata empty.\n", chan_id);
        usrdat_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    VFMW_CHECK_SEC_FUNC(memcpy_s(usr, sizeof(hi_vdec_user_data),
                                 chan->check_usr.chan_usr[chan->check_usr.read_index], sizeof(hi_vdec_user_data)));
    pool_index = usrdat_get_pool_index(usr->type);
    if (pool_index < 0) {
        usrdat_unlock_chan(chan_id);
        dprint(PRN_ERROR, "pool index error!\n");
        return HI_FAILURE;
    }
    pool = &chan->pool[pool_index];
    usrdat_find_block_by_addr(usr->usd_offset, pool, &block);
    if (block == HI_NULL) {
        usrdat_unlock_chan(chan_id);
        dprint(PRN_ERROR, "chan keep usrblock does not exist in pool!\n");
        return HI_FAILURE;
    }

    chan->check_usr.read_index = (chan->check_usr.read_index + 1) % USRDAT_MAX_CHAN_KEEP_NUM;

    chan->chk_ok_cnt++;
    usrdat_unlock_chan(chan_id);

    return HI_SUCCESS;
}

hi_s32 usrdat_bind_chan_mem(hi_s32 chan_id, vfmw_mem_report *mem)
{
    hi_void *dma_buf = HI_NULL;
    usrdat_chan *chan = HI_NULL;
    hi_u8 *vir = HI_NULL;
    UADDR phy_addr;
    mem_record mem_rec = {0};

    usrdat_lock_chan(chan_id);

    chan = usrdat_get_chan(chan_id);
    if (chan == HI_NULL) {
        dprint(PRN_ERROR, "usrdat chan %d not create!\n", chan_id);
        usrdat_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    dma_buf = OS_GET_DMA_BUF(mem->mem_fd);
    mem_rec.dma_buf = PTR_UINT64(dma_buf);
    mem_rec.is_cached = 0;
    vir = OS_MAP_MEM(&mem_rec);
    if (vir == HI_NULL) {
        OS_PUT_DMA_BUF(dma_buf);
        usrdat_unlock_chan(chan_id);
        return HI_FAILURE;
    }

    phy_addr = OS_GET_PHY(dma_buf, HI_FALSE);

    chan->dma_buf = PTR_UINT64(dma_buf);
    chan->vir_addr = PTR_UINT64(vir);
    chan->fd  = mem->mem_fd;
    chan->phy_addr = phy_addr;
    chan->usd_pool_size = mem->size;

    usrdat_split_usd_buf(chan);

    usrdat_unlock_chan(chan_id);

    return HI_SUCCESS;
}
