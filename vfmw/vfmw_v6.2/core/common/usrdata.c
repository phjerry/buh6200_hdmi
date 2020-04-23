/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "usrdata.h"
#include "vfmw_define.h"

#ifdef VFMW_CHAN_NUM
#define USD_CHAN_NUM VFMW_CHAN_NUM
#else
#define USD_CHAN_NUM 1
#endif

#if defined(CFG_MAX_USRDAT_BLK_NUM)
#define MAX_USRDAT_BLK_NUM CFG_MAX_USRDAT_BLK_NUM
#else
#define MAX_USRDAT_BLK_NUM (48 * 4)
#endif

typedef struct {
    hi_s32 valid_num;
    vfmw_usrdat *usrdata[MAX_USRDAT_BLK_NUM];
} usd_chan;

typedef struct {
    OS_SEMA sema;
    hi_u32 ctx_size;
    usd_chan *chan[USD_CHAN_NUM];
} usd_entry;

static usd_entry g_usd_entry;

usd_entry *usd_get_entry(hi_void)
{
    return &g_usd_entry;
}
hi_void usd_lock(hi_void)
{
    OS_SEMA_DOWN(usd_get_entry()->sema);
}

hi_void usd_unlock(hi_void)
{
    OS_SEMA_UP(usd_get_entry()->sema);
}

usd_chan *usd_get_chan(hi_s32 chan_id)
{
    if (chan_id < 0 || chan_id >= USD_CHAN_NUM) {
        return HI_NULL;
    }

    return usd_get_entry()->chan[chan_id];
}

hi_s32 usd_get_mem_need_size(vfmw_svr_mem_need *mem)
{
    hi_s32 size;

    size = VFMW_ALIGN_UP(sizeof(vfmw_usrdat), 4) * 16; /* 4 :a number 16 :a number */
    size = VFMW_ALIGN_UP(size, VFMW_MEM_ALIGN);
    size = size * ((MAX_USRDAT_BLK_NUM + 15) / 16); /* 15 :a number 16 :a number */
    mem->chan_cache_size += size;

    mem->chan_cache_size += VFMW_ALIGN_UP(sizeof(usd_chan), VFMW_MEM_ALIGN);

    return USD_OK;
}

hi_s32 usd_open(hi_void)
{
    usd_entry *entry;

    entry = usd_get_entry();
    if (OS_SEMA_INIT(&entry->sema) != OSAL_OK) {
        dprint(PRN_ERROR, "%s SemaInit Error \n", __func__);
        return USD_ERR;
    }

    VFMW_CHECK_SEC_FUNC(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));

    return USD_OK;
}

hi_s32 usd_close(hi_void)
{
    usd_entry *entry;

    entry = usd_get_entry();

    VFMW_CHECK_SEC_FUNC(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));

    OS_SEMA_EXIT(entry->sema);

    return USD_OK;
}

hi_s32 usd_create(hi_s32 chan_id)
{
    hi_s32 ret = USD_ERR;
    hi_s32 size;
    hi_u8 *vir = HI_NULL;
    usd_chan *chan = HI_NULL;
    hi_s32 i;
    osal_mem mem = { 0 };

    size = sizeof(usd_chan);

    mem.len = VFMW_ALIGN_UP(size, 16); /* 16 :a number */
    mem.is_cached = 1;
    mem.type = CACHE_MEM_TYPE;
    mem.id = chan_id;
    OS_MALLOC(&mem); /* USD_Inst */
    vir = mem.vir_addr;

    if (vir == HI_NULL) {
        dprint(PRN_ERROR, "UserData AllocMem(%d) Error\n", size);
        return USD_ERR;
    }

    VFMW_CHECK_SEC_FUNC(memset_s(vir, size, 0, size));

    usd_lock();

    chan = usd_get_chan(chan_id);
    if (chan == HI_NULL) {
        chan = (usd_chan *)vir;
        for (i = 0; i < MAX_USRDAT_BLK_NUM; i++) {
            chan->valid_num = 0;
            chan->usrdata[i] = HI_NULL;
        }

        usd_get_entry()->chan[chan_id] = chan;
        usd_get_entry()->ctx_size += size;

        ret = USD_OK;
    }

    usd_unlock();

    return ret;
}

hi_s32 usd_destroy(hi_s32 chan_id)
{
    hi_s32 i;
    hi_void *vir = HI_NULL;
    usd_chan *chan = HI_NULL;
    osal_mem mem = { 0 };

    usd_lock();

    chan = usd_get_chan(chan_id);
    if (chan == HI_NULL) {
        usd_unlock();
        return USD_ERR;
    }

    for (i = 0; i < MAX_USRDAT_BLK_NUM; i++) {
        if (chan->usrdata[i] != HI_NULL &&
            chan->usrdata[i]->is_mem_start == 1) {
            if (vir != HI_NULL) {
                mem.vir_addr = (hi_u8 *)(vir);
                mem.is_cached = 1;
                OS_FREE(&mem);
            }
            vir = chan->usrdata[i];
        }
    }

    if (vir != HI_NULL) {
        mem.vir_addr = (hi_u8 *)(vir);
        mem.is_cached = 1;
        OS_FREE(&mem);
    }

    mem.vir_addr = (hi_u8 *)(chan);
    mem.is_cached = 1;
    OS_FREE(&mem);

    usd_get_entry()->chan[chan_id] = HI_NULL;
    usd_get_entry()->ctx_size -= sizeof(usd_chan);
    usd_unlock();

    return USD_OK;
}

hi_s32 usd_reset(hi_s32 chan_id)
{
    hi_s32 i;
    usd_chan *chan = HI_NULL;

    chan = usd_get_entry()->chan[chan_id];
    if (chan != HI_NULL) {
        for (i = 0; i < MAX_USRDAT_BLK_NUM; i++) {
            if (chan->usrdata[i] != HI_NULL) {
                chan->usrdata[i]->from = VFMW_USD_INVALID;
                chan->usrdata[i]->dnr_used_flag = 0;
            }
        }
    }

    return USD_OK;
}

vfmw_usrdat *usd_get(hi_s32 chan_id)
{
    hi_s32 i;
    hi_s32 idx;
    hi_s32 blk_num;
    hi_s32 blk_size;
    hi_u8 *vir = HI_NULL;
    usd_chan *chan = HI_NULL;
    vfmw_usrdat *usrdata = HI_NULL;
    osal_mem mem = { 0 };

    chan = usd_get_entry()->chan[chan_id];
    if (chan == HI_NULL) {
        return HI_NULL;
    }

    for (i = 0; i < MAX_USRDAT_BLK_NUM; i++) {
        if (chan->usrdata[i] != HI_NULL &&
            chan->usrdata[i]->from == VFMW_USD_INVALID &&
            chan->usrdata[i]->dnr_used_flag == 0) {
            usrdata = chan->usrdata[i];
            usrdata->dnr_used_flag = 0;
            usrdata->pts = 0;
            usrdata->data_size = 0;
            usrdata->seq_cnt = usrdata->seq_img_cnt = 0;
            usrdata->is_registered = 0;
            usrdata->itu_t_t35_country_code = 0;
            usrdata->itu_t_t35_country_code_extension_byte = 0;
            usrdata->itu_t_t35_provider_code = 0;
            return usrdata;
        }
    }

    if (chan->valid_num >= MAX_USRDAT_BLK_NUM) {
        return HI_NULL;
    }

    blk_num = 16; /* 16 is blk num */
    blk_size = VFMW_ALIGN_UP(sizeof(vfmw_usrdat), 4); /* 4 :a number */
    if (blk_num > MAX_USRDAT_BLK_NUM - chan->valid_num) {
        blk_num = MAX_USRDAT_BLK_NUM - chan->valid_num;
    }

    mem.len = VFMW_ALIGN_UP(blk_num * blk_size, 16); /* 16 :a number */
    mem.is_cached = 1;
    mem.type = CACHE_MEM_TYPE;
    mem.id = chan_id;
    OS_MALLOC(&mem); /* USD_BLK */
    vir = mem.vir_addr;

    if (vir == HI_NULL) {
        return HI_NULL;
    }

    VFMW_CHECK_SEC_FUNC(memset_s(vir, blk_num * blk_size, 0, blk_num * blk_size));

    idx = chan->valid_num;
    for (i = 0; i < blk_num; i++, idx++) {
        chan->usrdata[idx] = (vfmw_usrdat *)(vir + i * blk_size);
        chan->usrdata[idx]->is_mem_start = 0;
        chan->usrdata[idx]->from = VFMW_USD_INVALID;
        chan->usrdata[idx]->dnr_used_flag = VFMW_USD_INVALID;
        chan->usrdata[idx]->dma_buf = mem.dma_buf;
        chan->usrdata[idx]->offset = i * blk_size;
    }

    idx = chan->valid_num;
    chan->valid_num += blk_num;

    usrdata = chan->usrdata[idx];
    usrdata->is_mem_start = 1;
    usrdata->dnr_used_flag = 0;
    usrdata->pts = 0;
    usrdata->data_size = 0;
    usrdata->seq_cnt = 0;
    usrdata->seq_img_cnt = 0;
    usrdata->is_registered = 0;
    usrdata->itu_t_t35_country_code = 0;
    usrdata->itu_t_t35_country_code_extension_byte = 0;
    usrdata->itu_t_t35_provider_code = 0;

    return usrdata;
}

hi_void usd_release(hi_s32 chan_id, hi_void *usrdata)
{
    if (usrdata != HI_NULL) {
        ((vfmw_usrdat *)usrdata)->from = VFMW_USD_INVALID;
    }

    return;
}
