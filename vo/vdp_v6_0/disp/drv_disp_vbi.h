/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: disp vbi
* Create: 2016-04-12
 */
#ifndef __DRV_VBI_H__
#define __DRV_VBI_H__


#include "hi_type.h"

#ifndef __DISP_PLATFORM_BOOT__
#include "hi_drv_mem.h"
#include "drv_disp_hal.h"
#include "drv_disp_isr.h"
#endif

#include "hi_drv_disp.h"

/* ckeck the validity of VBI channel */
#define VBI_CHECK_TYPE(vbi_type) \
do {                             \
    if (vbi_type >= HI_DRV_DISP_VBI_VCHIP) \
    {                               \
        HI_ERR_DISP("VBI: Type is not support!.\n"); \
        return HI_ERR_DISP_INVALID_PARA;\
    }                               \
} while (0)


#define    BUFFER_LEN             (0x1C00)
#define    TTX_BUFFER_LEN         HI_DISP_TTX_INBUFFERSIZE
#define    CC_BUFFER_LEN          HI_DISP_CC_INBUFFERSIZE

#define    TTX_BLOCK_NUM          (40)
#define    TTX_PES_MIN_LEN        (92)
#define    TTX_PES_MAX_LEN        (1656-6)
#define    TTX_RAW_PACKET_LENGTH  (46)
#define    TTX_PACKET_LENGTH      (176)

#define    CC_BLOCK_NUM           (20)
#define    WSS_BLOCK_NUM          (20)
#define    VBI_PES_MIN_LEN        (15)
#define    VBI_PES_MAX_LEN        (184-6)


#define    VBI_INIT_FLAG         0xdeadbeef



typedef enum tag_cc_send_step
{
    CC_SENDSTEP_START  = 0x0000,
    CC_SENDSTEP_PROCESSING,
    CC_SENDSTEP_END
} cc_send_step;


typedef struct hi_wss_data_block_
{
#ifndef __DISP_PLATFORM_BOOT__
    struct list_head list;
    hi_u16   top_data;
#endif
}wss_data_block;

typedef struct hi_ttx_data_block_
{
#ifndef __DISP_PLATFORM_BOOT__
    struct list_head list;
    hi_u8    *ttx_data_virt_addr;             /* virtual address of BUFFER kernel */
    hi_u32   ttx_start_addr;                  /* start physical address of TTX data */
    hi_u32   ttx_end_addr;                    /* end physical address of TTX data */
    hi_u32   ttx_data_line;                   /* transform line of TTX data */
    hi_u32   ttx_data_ext_line;                /* extended transform line of TTX data */
    hi_bool  ttx_seq_flag;                    /* sending order of TTX data, HI_TRUE is sent from MSB */
    hi_u8    ttx_pack_conut;
#endif
}ttx_data_block;

typedef struct hi_cc_data_block_
{
#ifndef __DISP_PLATFORM_BOOT__
    struct list_head list;
    hi_bool  top_flag;
    hi_bool  bottom_flag;
    hi_u16   top_line;
    hi_u16   bottom_line;
    hi_u16   top_data;
    hi_u16   bottom_data;
#endif
}cc_data_block;

typedef struct hi_disp_vbi_status_
{
    HI_DRV_DISP_VBI_TYPE_E vbi_type;
    hi_bool         open;
    HI_HANDLE   handle;
}hi_disp_vbi_status;

typedef struct hi_disp_vbi_wss_data_
{
    hi_bool enable;                            /**<WSS configure enable HI_TRUE: enable,HI_FALSE: disnable*//**<CNcomment:WSS配置使能。HI_TRUE：使能；HI_FALSE：禁止*/
    hi_u16  data;                            /**<Wss data */ /**<CNcomment:Wss数据*/
}wss_data;

typedef struct ttx_send
{
#ifndef __DISP_PLATFORM_BOOT__
    disp_mmz_buf       ttx_pes_buf;
    hi_u8              *pes_buf_virt_addr;
    hi_u32             ttx_pes_buf_write;

    disp_mmz_buf       ttx_data_buf;
    hi_u8              *ttx_buf_virt_addr;

    struct list_head   ttx_block_free_head;
    struct list_head   ttx_block_busy_head;
    ttx_data_block   ttx_data_block[TTX_BLOCK_NUM];
    struct list_head   *ttx_busy_list;
    ttx_data_block   *ttx_busy_block;
    hi_bool            ttx_busy_flag;
    hi_bool            ttx_list_flag;

    hi_u32      init_flag;
#endif
}ttx_send;

typedef struct vbi_send
{
#ifndef __DISP_PLATFORM_BOOT__
    disp_mmz_buf           vbi_pes_buf;
    hi_u8                  *pes_buf_virt_addr;
    hi_u32                 vbi_pes_buf_write;

    struct list_head       cc_block_free_head;
    struct list_head       cc_block_busy_head;
    cc_data_block        cc_data_block[CC_BLOCK_NUM];
    struct list_head       *cc_busy_list;
    cc_data_block        *cc_busy_block;
    hi_bool                cc_list_flag;
    cc_send_step           cc_send_step;

    struct list_head       wss_block_free_head;
    struct list_head       wss_block_busy_head;
    wss_data_block       wss_data_block[WSS_BLOCK_NUM];
    struct list_head       *wss_busy_list;
    wss_data_block       *wss_busy_block;
    hi_bool                wss_list_flag;

    wss_data  wss_data;
    hi_u32      init_flag;
#endif
}vbi_send;

typedef struct cc_send
{
   hi_bool top_flag;
   hi_bool bottom_flag;
   hi_u16 top_data;
   hi_u16 bottom_data;
   hi_u16 top_line;
   hi_u16 bottom_line;
}cc_send;


hi_s32 drv_vbi_create_channel(HI_DRV_DISP_VBI_CFG_S* cfg, HI_HANDLE *ph_vbi);

hi_s32 drv_vbi_destory_channel(HI_HANDLE h_vbi);

hi_s32 drv_vbi_send_data(HI_HANDLE h_vbi, HI_DRV_DISP_FMT_E fmt,HI_DRV_DISP_VBI_DATA_S* vbi_data);

hi_s32 drv_vbi_init(hi_void);

hi_s32 drv_vbi_de_init(hi_void);

#endif
