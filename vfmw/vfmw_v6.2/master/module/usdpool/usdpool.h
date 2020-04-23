/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#ifndef __USRDAT_POOL_H__
#define __USRDAT_POOL_H__

#include "vfmw.h"
#include "hi_vdec_type.h"
#include "vfmw_osal.h"

#define USRDAT_MAX_KEEP_NUM      100
#define USRDAT_TYPE_NUM          4
#define USRDAT_CC_INDEX          0
#define USRDAT_AFD_INDEX         1
#define USRDAT_BAR_INDEX         2
#define USRDAT_OTH_INDEX         3
#define USRDAT_INVALID_INDEX     (-1)
#define USRDAT_MAX_CHAN_KEEP_NUM 16

typedef enum {
    USRDAT_BLK_STA_WRITE = 0,
    USRDAT_BLK_STA_FREE,
    USRDAT_BLK_STA_READ,
} usrdat_block_state;

typedef struct {
    hi_vdec_user_data usr_data;
    struct OS_LIST_HEAD node;
    hi_bool is_used;
    usrdat_block_state state;
} usrdat_block;

typedef struct {
    hi_vdec_usd_type type;
    struct OS_LIST_HEAD list;
    hi_u64 dma_buf;
    hi_u64 base_offset;
    UADDR  start_phy_addr;
    hi_u64 start_vir_addr;
    hi_u32 length;
    hi_u32 free_len;
    hi_u32 freeze_len;
    hi_u32 write_offset;
    hi_u32 read_offset;
    hi_bool is_over_flow;
    hi_u32 usrdata_num;
    usrdat_block *block[USRDAT_MAX_KEEP_NUM];
} usrdat_pool;

typedef struct {
    hi_vdec_user_data *chan_usr[USRDAT_MAX_CHAN_KEEP_NUM];
    hi_u32 write_index;
    hi_u32 read_index;
} usrdat_check_desc;

typedef struct {
    hi_u64 dma_buf;
    hi_u64 vir_addr;
    UADDR  phy_addr;
    hi_mem_handle_t fd;
    usrdat_pool pool[USRDAT_TYPE_NUM];
    usrdat_check_desc check_usr;
    hi_u32 usd_pool_size;
    hi_u32 total_usd_num;
    hi_u32 try_acq_cnt;
    hi_u32 acq_ok_cnt;
    hi_u32 try_rls_cnt;
    hi_u32 rls_ok_cnt;
    hi_u32 try_chk_cnt;
    hi_u32 chk_ok_cnt;
} usrdat_chan;

hi_s32 usrdat_init(hi_void);
hi_s32 usrdat_exit(hi_void);
hi_s32 usrdat_create_chan(hi_s32 chan_id);
hi_s32 usrdat_destroy_chan(hi_s32 chan_id);
hi_s32 usrdat_reset_chan(hi_s32 chan_id);
hi_s32 usrdat_config_chan(hi_s32 chan_id, vfmw_chan_cfg *cfg);
hi_s32 usrdat_process(hi_s32 chan_id, vfmw_image *img);
hi_s32 usrdat_acquire(hi_s32 chan_id, hi_void *usrdata);
hi_s32 usrdat_release(hi_s32 chan_id, hi_void *usrdata);
hi_s32 usrdat_check(hi_s32 chan_id, hi_void *usrdata);
hi_s32 usrdat_bind_chan_mem(hi_s32 chan_id, vfmw_mem_report *mem);
usrdat_chan* usrdat_get_chan(hi_s32 chan_id);

#endif
