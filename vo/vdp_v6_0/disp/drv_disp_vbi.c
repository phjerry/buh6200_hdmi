/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: disp vbi
* Create: 2016-04-12
 */

#include "drv_vbi.h"

/* extern SDATE_VBI_VbiFilterEnable function in hal.*/
#include "vdp_software_selfdefine.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifndef __DISP_PLATFORM_BOOT__
hi_disp_vbi_status g_vbi[(HI_HANDLE)HI_DRV_DISP_VBI_TYPE_BUTT];

ttx_send  g_ttx_send;
vbi_send  g_vbi_send;

hi_void drv_vbi_vbi_isr(hi_void* h_dst, const HI_DRV_DISP_CALLBACK_INFO_S *info)
{
    hi_s32 n_btm_flag = 0;

    if (HI_NULL == info)
    {
        return;
    }

    n_btm_flag = !info->disp_info.bisbottomfield;
    if (g_vbi_send.cc_list_flag != HI_TRUE)
    {
        if (list_empty(&g_vbi_send.cc_block_busy_head))
        {
            sdate_vbi_ccline_config(HI_FALSE, HI_FALSE, 0, 0);
        }
        else
        {
            /*judge whether the first time to enter the node.
            comfire configure both fields CC data in bottom field each time.
        */
            if(g_vbi_send.cc_send_step == CC_SENDSTEP_START)
            {
                if(n_btm_flag)
                {
                    return ;
                }

                g_vbi_send.cc_send_step = CC_SENDSTEP_PROCESSING;
                g_vbi_send.cc_busy_list = g_vbi_send.cc_block_busy_head.next;
                g_vbi_send.cc_busy_block = list_entry(g_vbi_send.cc_busy_list, cc_data_block, List);
                if (HI_NULL == g_vbi_send.cc_busy_block)
                {
                    HI_ERR_DISP("pCcBusyBlock is NULL.\n");
                    return;
                }

                g_vbi_send.cc_send_step = CC_SENDSTEP_PROCESSING;

                sdate_vbi_ccdata_config(g_vbi_send.cc_busy_block->top_data, g_vbi_send.cc_busy_block->bottom_data);
                sdate_vbi_ccline_config(g_vbi_send.cc_busy_block->top_flag,g_vbi_send.cc_busy_block->bottom_flag,g_vbi_send.cc_busy_block->top_line,g_vbi_send.cc_busy_block->bottom_line);


                return;
            }

            list_del(g_vbi_send.cc_busy_list);
            list_add_tail(g_vbi_send.cc_busy_list, &g_vbi_send.cc_block_free_head);

            g_vbi_send.cc_send_step = CC_SENDSTEP_START;

        }
    }

    /*
    //config register directly
    if (g_VbiSend.WssData.bEnable)
    {
    DRV_VOU_WSSConfig(HI_TRUE, g_VbiSend.WssData.u16Data);
    }
    */
    /* WssListFlag = HI_FALSE */
    if (g_vbi_send.wss_list_flag != HI_TRUE)
    {
        if (!list_empty(&g_vbi_send.wss_block_busy_head))
        {
            g_vbi_send.wss_busy_list = g_vbi_send.wss_block_busy_head.next;
            g_vbi_send.wss_busy_block = list_entry(g_vbi_send.wss_busy_list, wss_data_block, List);
            if (HI_NULL == g_vbi_send.wss_busy_block)
            {
                HI_ERR_DISP("pWssBusyBlock is NULL.\n");
                return;
            }

            list_del(g_vbi_send.wss_busy_list);

            if (!g_vbi_send.wss_data.enable)
            {
                sdate_vbi_wssconfig(HI_TRUE, g_vbi_send.wss_busy_block->top_data);
            }
            list_add_tail(g_vbi_send.wss_busy_list, &g_vbi_send.wss_block_free_head);
        }
    }

    return ;
}

hi_void drv_vbi_ttx_isr(hi_void* h_dst, const HI_DRV_DISP_CALLBACK_INFO_S *info)
{
    HI_DRV_DISP_FMT_E format = HI_DRV_DISP_FMT_BUTT;

    if (HI_NULL == info)
    {
        return;
    }

    if (!sdate_vbi_int_status())
    {
        return;
    }
    else
    {
        sdate_vbi_ttx_int_clear();
    }

    if (g_ttx_send.ttx_list_flag)
    {
        g_ttx_send.ttx_busy_flag = HI_FALSE;
        return;
    }

    if (g_ttx_send.ttx_busy_list != HI_NULL) {
        list_add_tail(g_ttx_send.ttx_busy_list, &g_ttx_send.ttx_block_free_head);
        g_ttx_send.ttx_busy_list = NULL;
    }

    if (list_empty(&g_ttx_send.ttx_block_busy_head))
    {
        g_ttx_send.ttx_busy_flag = HI_FALSE;
        return;
    }

    g_ttx_send.ttx_busy_list = g_ttx_send.ttx_block_busy_head.next;
    g_ttx_send.ttx_busy_block = list_entry(g_ttx_send.ttx_busy_list, ttx_data_block, List);
    if (HI_NULL == g_ttx_send.ttx_busy_block)
    {
        HI_ERR_DISP("pWssBusyBlock is NULL.\n");
        return;
    }

    list_del(g_ttx_send.ttx_busy_list);


    if (info->disp_info.fmt_resolution.width == 720
        && info->disp_info.fmt_resolution.height == 576)
    {
        format = HI_DRV_DISP_FMT_PAL;
    }
    else if (info->disp_info.fmt_resolution.width == 720
        && info->disp_info.fmt_resolution.height == 480)
    {
        format = HI_DRV_DISP_FMT_NTSC;
    }
    else
    {
        format = HI_DRV_DISP_FMT_BUTT;
    }

    /*set mode  for NTSC or PAL*/
    if ((format == HI_DRV_DISP_FMT_PAL)
            || (format == HI_DRV_DISP_FMT_PAL_N)
            || (format == HI_DRV_DISP_FMT_PAL_Nc)
       )
    {
        sdate_vbi_ttx_set_mode(0x1);

        /*if 625i select line 7 ~22*/
        g_ttx_send.ttx_busy_block->ttx_data_line = 0xffffffff;/*7 ~22 line */
        g_ttx_send.ttx_busy_block->ttx_data_ext_line = 0x6;/*add sixth line */
    }
    else if ((format == HI_DRV_DISP_FMT_NTSC)
            || (format == HI_DRV_DISP_FMT_NTSC_J)
            || (format == HI_DRV_DISP_FMT_PAL_M))
    {
        sdate_vbi_ttx_set_mode(0x2);

        /*if 525i select line 10 ~21*/
        g_ttx_send.ttx_busy_block->ttx_data_line = 0x7ff87ff8;/*10 ~21*/
        g_ttx_send.ttx_busy_block->ttx_data_ext_line = 0x00;
    }

    g_ttx_send.ttx_busy_block->ttx_pack_conut = 0;

    sdate_vbi_ttx_set_seq(g_ttx_send.ttx_busy_block->ttx_seq_flag);
    osal_udelay(5); /* 5 is delay time */
    sdate_vbi_ttx_line_enable(g_ttx_send.ttx_busy_block->ttx_data_line, g_ttx_send.ttx_busy_block->ttx_data_ext_line);
    osal_udelay(5); /* 5 is delay time */
//    VDP_VBI_TtxSetPackOff(TTX_PACKET_LENGTH);
    sdate_vbi_ttx_set_addr((g_ttx_send.ttx_busy_block->ttx_start_addr), (g_ttx_send.ttx_busy_block->ttx_end_addr));
    osal_udelay(5); /* 5 is delay time */
    sdate_vbi_ttx_set_ready();

    return;
}

hi_s32 drv_vbi_init_ttx(hi_void)
{
    HI_DRV_DISP_CALLBACK_S ttx_isr;

    hi_u32    i = 0;
    hi_s32    ret = HI_SUCCESS;

    if (g_ttx_send.init_flag == VBI_INIT_FLAG){
        HI_WARN_DISP(" Ttx already init.\n");
        return HI_SUCCESS;
    }

    memset((hi_void *)&ttx_isr, 0x0, sizeof(ttx_isr));

    ret = disp_os_mmz_alloc_and_map("VDP_TTXPes", VDP_MEM_TYPE_PHY, TTX_BUFFER_LEN, 0, &g_ttx_send.ttx_pes_buf);
    if (HI_SUCCESS != ret)
    {
        HI_ERR_DISP("malloc TtxPes buf failed.\n");
        return ret;
    }

    ret = disp_os_mmz_alloc_and_map("VDP_TTXData", VDP_MEM_TYPE_PHY, TTX_BLOCK_NUM*BUFFER_LEN, 0, &g_ttx_send.ttx_data_buf);
    if (HI_SUCCESS != ret)
    {
        HI_ERR_DISP("malloc TtxData buf failed.\n");
        disp_os_mmz_unmap_and_release(&g_ttx_send.ttx_pes_buf);
        return ret;
    }

    g_ttx_send.pes_buf_virt_addr = (hi_u8 *)g_ttx_send.ttx_pes_buf.start_vir_addr;
    g_ttx_send.ttx_pes_buf_write = 0;

    g_ttx_send.ttx_buf_virt_addr = (hi_u8 *)g_ttx_send.ttx_data_buf.start_vir_addr;

    INIT_LIST_HEAD(&g_ttx_send.ttx_block_free_head);
    INIT_LIST_HEAD(&g_ttx_send.ttx_block_busy_head);

    g_ttx_send.ttx_busy_list = NULL;
    g_ttx_send.ttx_busy_block = NULL;
    g_ttx_send.ttx_busy_flag = HI_FALSE;
    g_ttx_send.ttx_list_flag = HI_FALSE;

    for (i=0; i<TTX_BLOCK_NUM; i++)
    {
        g_ttx_send.ttx_data_block[i].ttx_data_virt_addr = (hi_u8 *)(g_ttx_send.ttx_data_buf.start_vir_addr + i*BUFFER_LEN);
        g_ttx_send.ttx_data_block[i].ttx_start_addr = g_ttx_send.ttx_data_buf.start_phy_addr + i*BUFFER_LEN;
        g_ttx_send.ttx_data_block[i].ttx_pack_conut = 0;
        list_add_tail(&g_ttx_send.ttx_data_block[i].list, &g_ttx_send.ttx_block_free_head);
    }


    /* setting of TTX mode */

    sdate_vbi_ttx_set_pack_off(TTX_PACKET_LENGTH);
    osal_udelay(20); /* 20 is delay time */

    sdate_vbi_ttx_piority_highest(HI_TRUE);
    osal_udelay(20); /* 20 is delay time */

    sdate_vbi_set_ttx_full_page(HI_FALSE);
    osal_udelay(20); /* 20 is delay time */

    /* VBI data filtering enable */
    sdate_vbi_vbi_filter_enable(HI_TRUE);
    osal_udelay(20); /* 20 is delay time */

    sdate_vbi_ttx_set_mode(0x1);
    osal_udelay(20); /* 20 is delay time */

    ttx_isr.pfdisp_callback = drv_vbi_ttx_isr;
    ttx_isr.ecallbackprior = HI_DRV_DISP_CALLBACK_PRIORTY_MIDDLE;
    ttx_isr.hdst = (hi_void*)HI_INVALID_HANDLE;

    /*if config ttx data at 90% postion, ttx finish will cast more two flieds time.*/
#ifdef VDP_ISOGENY_SUPPORT
    disp_isr_reg_callback(HI_DRV_DISPLAY_1, HI_DRV_DISP_C_INTPOS_0_PERCENT, &ttx_isr);
#else
    disp_isr_reg_callback(HI_DRV_DISPLAY_0, HI_DRV_DISP_C_INTPOS_0_PERCENT, &ttx_isr);
#endif

    sdate_vbi_ttx_enable_int();
    osal_udelay(20); /* 20 is delay time */

    g_ttx_send.init_flag = VBI_INIT_FLAG;

    return HI_SUCCESS;
}


hi_s32 drv_vbi_de_init_ttx(hi_void)
{
    HI_DRV_DISP_CALLBACK_S ttx_isr;

    memset((hi_void *)&ttx_isr, 0x0, sizeof(ttx_isr));
    g_ttx_send.init_flag = 0;
    osal_msleep_uninterruptible(5); /* 5 is time */

    INIT_LIST_HEAD(&g_ttx_send.ttx_block_free_head);
    INIT_LIST_HEAD(&g_ttx_send.ttx_block_busy_head);

    sdate_vbi_ttx_int_clear();

    sdate_vbi_ttx_disable_int();
    sdate_vbi_vbi_filter_enable(HI_FALSE);
    sdate_vbi_ttx_piority_highest(HI_FALSE);

    //before unmap data,sleeping can avoid reading invalid data.
    //waiting for the last IRQ
    osal_msleep_uninterruptible(20); /* 20 is time */

    ttx_isr.pfdisp_callback = drv_vbi_ttx_isr;
    ttx_isr.ecallbackprior = HI_DRV_DISP_CALLBACK_PRIORTY_MIDDLE;
    ttx_isr.hdst = (hi_void*)HI_INVALID_HANDLE;
#ifdef VDP_ISOGENY_SUPPORT
    disp_isr_un_reg_callback(HI_DRV_DISPLAY_1, HI_DRV_DISP_C_INTPOS_0_PERCENT, &ttx_isr);
#else
    disp_isr_un_reg_callback(HI_DRV_DISPLAY_0, HI_DRV_DISP_C_INTPOS_0_PERCENT, &ttx_isr);
#endif

    disp_os_mmz_unmap_and_release(&g_ttx_send.ttx_pes_buf);
    disp_os_mmz_unmap_and_release(&g_ttx_send.ttx_data_buf);

    return HI_SUCCESS;
}


hi_s32 drv_vbi_init_vbi(hi_void)
{
    hi_u32    i = 0;
    hi_s32    ret = HI_SUCCESS;
    HI_DRV_DISP_CALLBACK_S vbi_isr;

    if (g_vbi_send.init_flag == VBI_INIT_FLAG){
        HI_WARN_DISP(" vbi already init.\n");
        return HI_SUCCESS;
    }

    memset((hi_void *)&vbi_isr, 0x0, sizeof(vbi_isr));

    ret = disp_os_mmz_alloc_and_map("VDP_VBIPes", HI_NULL, CC_BUFFER_LEN, 0, &g_vbi_send.vbi_pes_buf);
    if (ret != HI_SUCCESS)
    {
        HI_ERR_DISP("malloc VbiPes buf failed.\n");
        return ret;
    }

    g_vbi_send.pes_buf_virt_addr = (hi_u8 *)g_vbi_send.vbi_pes_buf.start_vir_addr;
    g_vbi_send.vbi_pes_buf_write = 0;

    INIT_LIST_HEAD(&g_vbi_send.cc_block_free_head);
    INIT_LIST_HEAD(&g_vbi_send.cc_block_busy_head);

    for (i=0; i<CC_BLOCK_NUM; i++)
    {
        list_add_tail(&g_vbi_send.cc_data_block[i].list, &g_vbi_send.cc_block_free_head);
    }

    g_vbi_send.cc_busy_list  = NULL;
    g_vbi_send.cc_busy_block = NULL;
    g_vbi_send.cc_list_flag   = HI_FALSE;
    g_vbi_send.cc_send_step   = CC_SENDSTEP_START;


    sdate_vbi_ccset_seq(HI_TRUE);

    INIT_LIST_HEAD(&g_vbi_send.wss_block_free_head);
    INIT_LIST_HEAD(&g_vbi_send.wss_block_busy_head);

    for (i=0; i<CC_BLOCK_NUM; i++)
    {
        list_add_tail(&g_vbi_send.wss_data_block[i].list, &g_vbi_send.wss_block_free_head);
    }

    g_vbi_send.wss_busy_list    = NULL;
    g_vbi_send.wss_busy_block   = NULL;
    g_vbi_send.wss_list_flag     = HI_FALSE;

    g_vbi_send.wss_data.enable = HI_FALSE;
    g_vbi_send.wss_data.data = 0;

    sdate_vbi_wssconfig(g_vbi_send.wss_data.enable, g_vbi_send.wss_data.data);

    vbi_isr.pfdisp_callback = drv_vbi_vbi_isr;
    vbi_isr.ecallbackprior = HI_DRV_DISP_CALLBACK_PRIORTY_MIDDLE;
    vbi_isr.hdst = (hi_void*)HI_INVALID_HANDLE;

#ifdef VDP_ISOGENY_SUPPORT
    disp_isr_reg_callback(HI_DRV_DISPLAY_1, HI_DRV_DISP_C_INTPOS_90_PERCENT, &vbi_isr);
#else
    disp_isr_reg_callback(HI_DRV_DISPLAY_0, HI_DRV_DISP_C_INTPOS_90_PERCENT, &vbi_isr);
#endif
    /*DRV_VOU_WSSSetSeq(HI_TRUE);*/
    g_vbi_send.init_flag = VBI_INIT_FLAG;

    return HI_SUCCESS;
}

hi_s32 drv_vbi_de_init_vbi(hi_void)
{
    HI_DRV_DISP_CALLBACK_S vbi_isr;

    memset((hi_void *)&vbi_isr, 0x0, sizeof(vbi_isr));
    g_vbi_send.init_flag = 0;
    osal_msleep_uninterruptible(5); /* 5 is time */

    disp_os_mmz_unmap_and_release(&g_vbi_send.vbi_pes_buf);

    vbi_isr.pfdisp_callback = drv_vbi_vbi_isr;
    vbi_isr.ecallbackprior = HI_DRV_DISP_CALLBACK_PRIORTY_MIDDLE;
    vbi_isr.hdst = (hi_void*)HI_INVALID_HANDLE;

#ifdef VDP_ISOGENY_SUPPORT
    disp_isr_un_reg_callback(HI_DRV_DISPLAY_1, HI_DRV_DISP_C_INTPOS_90_PERCENT, &vbi_isr);
#else
    disp_isr_un_reg_callback(HI_DRV_DISPLAY_0, HI_DRV_DISP_C_INTPOS_90_PERCENT, &vbi_isr);
#endif
    /* shutdown CC WSS */
    sdate_vbi_ccline_config(HI_FALSE, HI_FALSE, 0, 0);
    sdate_vbi_wssconfig(HI_FALSE, 0);

    return HI_SUCCESS;
}

hi_s32 drv_vbi_create_channel(HI_DRV_DISP_VBI_CFG_S* cfg, HI_HANDLE *ph_vbi)
{
    hi_s32 ret = HI_SUCCESS;

    if ((HI_NULL == cfg) || (HI_NULL == ph_vbi))
    {
        HI_ERR_DISP("para is invalid!\n");
        return HI_ERR_DISP_NULL_PTR;
    }

    switch (cfg->etype)
    {
        case HI_DRV_DISP_VBI_TTX:
            if (g_vbi[HI_DRV_DISP_VBI_TTX].open == HI_TRUE)
            {
                HI_ERR_DISP("DISP:Vbi Channel has been Created !.\n");
                return HI_FAILURE;
            }
            ret = drv_vbi_init_ttx();
            break;
        case HI_DRV_DISP_VBI_CC:
            ret = drv_vbi_init_vbi();
            break;
        case HI_DRV_DISP_VBI_WSS:
        case HI_DRV_DISP_VBI_VCHIP:
            ret = HI_ERR_DISP_INVALID_OPT;
            break;
        default:
            ret = HI_ERR_DISP_INVALID_OPT;
    }

    sdate_vbi_vbi_filter_enable(HI_TRUE);

    if (HI_SUCCESS == ret )
    {
        *ph_vbi = (HI_HANDLE)cfg->etype;

        g_vbi[(HI_HANDLE)(*ph_vbi)].open = HI_TRUE;
        g_vbi[(HI_HANDLE)(*ph_vbi)].vbi_type = cfg->etype;
        g_vbi[(HI_HANDLE)(*ph_vbi)].handle =*ph_vbi;
    }
    else
    {
        *ph_vbi = HI_INVALID_HANDLE;
    }

    return ret;
}

hi_s32 drv_vbi_destory_channel(HI_HANDLE h_vbi)
{
    hi_s32 ret = HI_SUCCESS;

    HI_DRV_DISP_VBI_TYPE_E  vbi_type = HI_DRV_DISP_VBI_TYPE_BUTT;

    vbi_type = (HI_DRV_DISP_VBI_TYPE_E) h_vbi;

    VBI_CHECK_TYPE(vbi_type);

    if (HI_FALSE == g_vbi[h_vbi].open)
    {
        HI_ERR_DISP("DISP: Channel has been Destroy !.\n");
        return HI_ERR_DISP_DESTROY_ERR;
    }

    switch (vbi_type)
    {
        case HI_DRV_DISP_VBI_TTX:

            ret = drv_vbi_de_init_ttx();
            break;
        case HI_DRV_DISP_VBI_CC:
        case HI_DRV_DISP_VBI_WSS:
            ret = drv_vbi_de_init_vbi();
            break;
        default:
            ret = HI_ERR_DISP_INVALID_OPT;
    }

    if (HI_SUCCESS == ret )
    {
        g_vbi[h_vbi].open = HI_FALSE;
        g_vbi[h_vbi].vbi_type = HI_DRV_DISP_VBI_TYPE_BUTT;
        g_vbi[h_vbi].handle = HI_INVALID_HANDLE;
    }

    return ret;
}

hi_void drv_vbi_ttx_odd_line_parse(ttx_data_block* ttx_block, hi_u8 line_num)
{
    hi_u8  u8_temp_num = line_num & 0x1f;

    if (HI_NULL == ttx_block)
    {
        HI_ERR_DISP("pTtxBlock is NULL.\n");
        return;
    }

    if ((u8_temp_num >= 7 ) && ((u8_temp_num) <= 22))
    {
        ttx_block->ttx_data_line = ttx_block->ttx_data_line | (0x00010000 << (u8_temp_num - 7));
    }
    else if (6 == u8_temp_num)
    {
        ttx_block->ttx_data_ext_line = ttx_block->ttx_data_ext_line | 0x4;
    }

    return;
}

hi_void drv_vbi_ttx_even_line_parse(ttx_data_block* ttx_block, hi_u8 line_num)
{
    hi_u8  u8_temp_num = line_num & 0x1f;

    if (HI_NULL == ttx_block)
    {
        HI_ERR_DISP("pTtxBlock is NULL.\n");
        return;
    }

    if ((u8_temp_num >= 7 ) && (u8_temp_num <= 22))
    {
        ttx_block->ttx_data_line = ttx_block->ttx_data_line | (0x00000001 << (u8_temp_num - 7));
    }
    else  if (5 == u8_temp_num)
    {
        ttx_block->ttx_data_ext_line = ttx_block->ttx_data_ext_line | 0x1;
    }
    else  if (6 == u8_temp_num)
    {
        ttx_block->ttx_data_ext_line = ttx_block->ttx_data_ext_line | 0x2;
    }

    return;
}

hi_void drv_vbi_ttx_line_parse(ttx_data_block* ttx_block, hi_u8 line_num)
{
    if (HI_NULL == ttx_block)
    {
        HI_ERR_DISP("pTtxBlock is NULL.\n");
        return;
    }

    if (line_num & 0x20)
    {
        drv_vbi_ttx_odd_line_parse(ttx_block, line_num);
    }
    else
    {
        drv_vbi_ttx_even_line_parse(ttx_block, line_num);
    }

    return;
}

hi_void drv_vbi_ttx_data_parse(hi_u8* pes_buf, hi_u32 ttx_data_len)
{
    struct list_head *list = HI_NULL;
    ttx_data_block  *ttx_block = HI_NULL;
    hi_u8 *dst_buf = HI_NULL, *src_buf = HI_NULL;
    hi_u32 i = 0, j = 0, k = 0;

    if (HI_NULL == pes_buf)
    {
        HI_ERR_DISP("pPesBuf is NULL!\n");
        return;
    }

    list = g_ttx_send.ttx_block_free_head.next;
    ttx_block = list_entry(list, ttx_data_block, List);
    if (HI_NULL == ttx_block)
    {
        HI_ERR_DISP("pTtxBlock is NULL.\n");
        return;
    }

    ttx_block->ttx_data_line = 0x0;
    ttx_block->ttx_data_ext_line = 0x0;

    k=0;
    for (i=0; i<ttx_data_len/46; i++)
    {
        if ((*(pes_buf + i*TTX_RAW_PACKET_LENGTH) == 0x02)  /*EBG no subtitle data*/
          ||(*(pes_buf + i*TTX_RAW_PACKET_LENGTH) == 0x03)  /*EBG  subtitle data */
          ||(*(pes_buf + i*TTX_RAW_PACKET_LENGTH) == 0xc0)  /*Inverted Teletext  */
          ||(*(pes_buf + i*TTX_RAW_PACKET_LENGTH) == 0xc1)  /*unknow */
          ||(*(pes_buf + i*TTX_RAW_PACKET_LENGTH) == 0xc3)  /*VPS data*/
           )
        {
            /*memcpy((pTtxBlock->pTtxDataVirtAddr + i*TTX_PACKET_LENGTH),
            pPesBuf + i*TTX_RAW_PACKET_LENGTH), TTX_RAW_PACKET_LENGTH);*/

           // pu8DstBuf = pTtxBlock->pTtxDataVirtAddr + k*TTX_PACKET_LENGTH;

            if( ttx_block->ttx_pack_conut >= 39)
            {
                ttx_block->ttx_pack_conut = 39;
            }

            dst_buf = ttx_block->ttx_data_virt_addr + ttx_block->ttx_pack_conut*TTX_PACKET_LENGTH;
            src_buf = pes_buf + i*TTX_RAW_PACKET_LENGTH + 4;

            for(j=0; j<10; j++)
            {
                *dst_buf++ = *src_buf++;
                *dst_buf++ = *src_buf++;
                *dst_buf++ = *src_buf++;
                *dst_buf++ = *src_buf++;
                dst_buf   += 12;
            }

            *dst_buf++ = *src_buf++;
            *dst_buf++ = *src_buf++;
            k++;
            ttx_block->ttx_pack_conut++;

            if( ttx_block->ttx_pack_conut >= 39)
            {
                break;
            }

            drv_vbi_ttx_line_parse(ttx_block, *(pes_buf + i*46 + 2));
        }
        else
        {
            continue;
        }
    }

    /* check the sending order of graph-text data */
    if ((*(pes_buf + 3) == 0xE4))
    {
        ttx_block->ttx_seq_flag = HI_TRUE;
    }
    else
    {
        ttx_block->ttx_seq_flag = HI_FALSE;
    }

    //printk("TtxPackConut ---->%d\n",pTtxBlock->TtxPackConut);
   if (ttx_block->ttx_pack_conut >15 )
     {
        ttx_block->ttx_end_addr = ttx_block->ttx_start_addr + (ttx_block->ttx_pack_conut-1)*TTX_PACKET_LENGTH;

        g_ttx_send.ttx_list_flag = HI_TRUE;

        list_move_tail(list, &g_ttx_send.ttx_block_busy_head);

        g_ttx_send.ttx_list_flag = HI_FALSE;
     }

    return ;
}

hi_void drv_vbi_vbi_data_parse(hi_u8 *pes_buf, hi_bool both_field, HI_DRV_DISP_FMT_E fmt)
{
    struct list_head  *list = HI_NULL;
    cc_data_block   *cc_block = HI_NULL;
    wss_data_block  *wss_block = HI_NULL;

    if (HI_NULL == pes_buf)
    {
        HI_ERR_DISP("pPesBuf is NULL!\n");
        return;
    }

    /* CC data-flag */
    if (pes_buf[0] == 0xc5)
    {
        list = g_vbi_send.cc_block_free_head.next;
        cc_block = list_entry(list, cc_data_block, List);
        if (HI_NULL == cc_block)
        {
            HI_ERR_DISP("pCCBlock is NULL.\n");
            return;
        }

        if (both_field == HI_TRUE)
        {
            cc_block->top_flag = HI_TRUE;
            cc_block->top_data = pes_buf[4];
            cc_block->top_data = cc_block->top_data << 8;
            cc_block->top_data = cc_block->top_data + pes_buf[3];

            cc_block->bottom_flag = HI_TRUE;

            if (fmt < HI_DRV_DISP_FMT_NTSC)
            {
                cc_block->top_line = 21;
                cc_block->bottom_line = 334;
            }
            else
            {
                cc_block->top_line = 21;
                cc_block->bottom_line = 284;
            }

            cc_block->bottom_data = pes_buf[9];
            cc_block->bottom_data = cc_block->bottom_data << 8;
            cc_block->bottom_data = cc_block->bottom_data + pes_buf[8];
        }
        else
        {
            /* top field data */
            if (pes_buf[2] & 0x20)
            {
                cc_block->bottom_flag = HI_FALSE;
                cc_block->top_flag = HI_TRUE;

                if (fmt < HI_DRV_DISP_FMT_NTSC)
                {
                    cc_block->top_line = 21;

                }
                else
                {
                    cc_block->top_line = 21;
                }

                cc_block->top_data = pes_buf[4];
                cc_block->top_data = cc_block->top_data << 8;
                cc_block->top_data = cc_block->top_data + pes_buf[3];
            }
            else
            {
                cc_block->top_flag = HI_FALSE;
                cc_block->bottom_flag = HI_TRUE;

                if (fmt < HI_DRV_DISP_FMT_NTSC)
                {
                    cc_block->bottom_line = 334;

                }
                else
                {
                    cc_block->bottom_line = 284;
                }

                cc_block->bottom_data = pes_buf[4];
                cc_block->bottom_data = cc_block->bottom_data << 8;
                cc_block->bottom_data = cc_block->bottom_data + pes_buf[3];
            }
        }

        g_vbi_send.cc_list_flag = HI_TRUE;
        list_move_tail(list, &g_vbi_send.cc_block_busy_head);
        g_vbi_send.cc_list_flag = HI_FALSE;
    }

    /*WSS data flag*/
    else
    {
        list = g_vbi_send.wss_block_free_head.next;
        wss_block = list_entry(list, wss_data_block, List);
        if (HI_NULL == wss_block)
        {
            HI_ERR_DISP("pWSSBlock is NULL.\n");
            return;
        }

        wss_block->top_data = pes_buf[4];
        wss_block->top_data = wss_block->top_data << 8;
        wss_block->top_data = wss_block->top_data + pes_buf[3];
        g_vbi_send.wss_list_flag = HI_TRUE;
        list_move_tail(list, &g_vbi_send.wss_block_busy_head);
        g_vbi_send.wss_list_flag = HI_FALSE;
    }

    return;
}

hi_bool  vbi_cc_check_whether_it_is_ebudata(hi_u32 tmp_index, hi_u32 pes_head_len)
{
    if (((g_vbi_send.pes_buf_virt_addr[tmp_index + pes_head_len + 9] >= 0x10)
         && (g_vbi_send.pes_buf_virt_addr[tmp_index + pes_head_len + 9] <= 0x1f))
        || ((g_vbi_send.pes_buf_virt_addr[tmp_index + pes_head_len + 9] >= 0x99)
            && (g_vbi_send.pes_buf_virt_addr[tmp_index + pes_head_len + 9] <= 0x9b))
       )
    {
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_bool vbi_cc_check_data_packet_head(hi_u32 tmp_index)
{
    if ((g_vbi_send.pes_buf_virt_addr[tmp_index + 1] == 0x00)
        && (g_vbi_send.pes_buf_virt_addr[tmp_index + 2] == 0x01)
        && (g_vbi_send.pes_buf_virt_addr[tmp_index + 3] == 0xbd))
    {
        return  HI_TRUE;
    }

    return HI_FALSE;
}

hi_bool  vbi_cc_data_correct(hi_u32 tmp_index, hi_u32  pes_head_len, hi_u32 vbi_data_len,    hi_bool*  both_field)
{
    hi_bool     data_correct_flag = HI_TRUE;

    if (g_vbi_send.pes_buf_virt_addr[tmp_index + pes_head_len + 10] == 0xc5)
    {
        /* CC data may contain two fields */
        if (vbi_data_len >= 10)
        {
            if (g_vbi_send.pes_buf_virt_addr[tmp_index + pes_head_len + 15] == 0xc5)
            {
                data_correct_flag = HI_TRUE;
                * both_field = HI_TRUE;
            }
            else if (g_vbi_send.pes_buf_virt_addr[tmp_index + pes_head_len + 15] == 0xff)
            {
                data_correct_flag = HI_TRUE;
                *both_field = HI_FALSE;
            }
        }
        else
        {
            data_correct_flag = HI_TRUE;
            *both_field = HI_FALSE;
        }
    }
    /*WSS data flag*/
    else if (g_vbi_send.pes_buf_virt_addr[tmp_index + pes_head_len + 10] == 0xc4)
    {
        data_correct_flag = HI_TRUE;
    }

    return data_correct_flag;
}

hi_void  vbi_ebu_data_packe_parse(hi_u32 tmp_index, HI_DRV_DISP_FMT_E fmt, hi_u32 pes_head_len, hi_u32 pes_len)
{
    hi_bool     data_correct_flag = HI_TRUE;
    hi_bool     both_field = HI_FALSE;
    hi_u32       vbi_data_len = 0;

    if (pes_head_len < (pes_len - 4))
    {
        /* query whether it is EBU data */
        if (HI_TRUE == vbi_cc_check_whether_it_is_ebudata(tmp_index, pes_head_len))
        {
            data_correct_flag = HI_FALSE;
            vbi_data_len = pes_len - pes_head_len - 4;

            /* the length of the data must >= 5*/
            if (vbi_data_len >= 5)
            {
                /* CC data flag */
                data_correct_flag = vbi_cc_data_correct(tmp_index, pes_head_len, vbi_data_len, &both_field);

                /* if the data is valid, analyze, copy it and add it to queue */
                if (HI_TRUE == data_correct_flag)
                {
                    drv_vbi_vbi_data_parse((g_vbi_send.pes_buf_virt_addr + tmp_index + pes_head_len + 10),
                                         both_field, fmt);
                }
            }

        }
    }
}

hi_s32 drv_vbi_send_cc_wss_data(HI_DRV_DISP_FMT_E fmt, HI_DRV_DISP_VBI_DATA_S* vbi_data)
{
    hi_u32      tmp_index = 0;
    hi_u32      pes_len_low8 = 0, pes_len_high8 = 0, pes_len = 0, pes_head_len = 0;
    hi_bool     send_flag = HI_FALSE;
    hi_s32      ret = HI_SUCCESS;
    hi_u64      buf_end_addr = 0;

    if (HI_NULL == vbi_data)
    {
        HI_ERR_DISP("pstVbiData ptr is null.\n");
        return  HI_ERR_DISP_NULL_PTR;
    }


    if (g_vbi_send.init_flag != VBI_INIT_FLAG)
    {
        DISP_ERROR("Channel not inited!\n");
        return HI_ERR_DISP_NO_INIT;
    }

    buf_end_addr = (g_vbi_send.vbi_pes_buf_write + vbi_data->u32datalen);
    if (buf_end_addr > CC_BUFFER_LEN
        || vbi_data->u32datalen > CC_BUFFER_LEN
        ||  g_vbi_send.vbi_pes_buf_write > CC_BUFFER_LEN)
    {
        DISP_ERROR("The Vbi Pes buffer is overflow!\n");
        return  HI_ERR_DISP_INVALID_PARA;
    }
    else
    {
        if (HI_NULL == vbi_data->pu8dataaddr)
        {
            HI_ERR_DISP("pstVbiData->pu8DataAddr ptr is null.\n");
            return  HI_ERR_DISP_NULL_PTR;
        }

        ret = osal_copy_from_user((g_vbi_send.pes_buf_virt_addr+g_vbi_send.vbi_pes_buf_write),
                                  (hi_void __user *)vbi_data->pu8dataaddr, vbi_data->u32datalen);
        if (ret != HI_SUCCESS)
        {
            HI_ERR_DISP("copy data from user failed.\n");
            return ret;
        }

        /* move write-pointer to the tail of buffer */
        g_vbi_send.vbi_pes_buf_write = g_vbi_send.vbi_pes_buf_write + vbi_data->u32datalen;
        send_flag = HI_TRUE;
    }

    while(tmp_index < g_vbi_send.vbi_pes_buf_write)
    {
        if (list_empty(&g_vbi_send.cc_block_free_head))
        {
            HI_WARN_DISP("The CC buffer is overflow!\n");

            if (tmp_index != 0)
            {
                /* copy the data left to the top of buffer */
                memcpy(g_vbi_send.pes_buf_virt_addr, (g_vbi_send.pes_buf_virt_addr + tmp_index), g_vbi_send.vbi_pes_buf_write - tmp_index);

                /* move the write-pointer to the tail of buffer */
                g_vbi_send.vbi_pes_buf_write = g_vbi_send.vbi_pes_buf_write - tmp_index;
            }

            break;
        }

        if (list_empty(&g_vbi_send.wss_block_free_head))
        {
            HI_ERR_DISP("The WSS buffer is overflow!\n");

            if (tmp_index != 0)
            {
                /* copy the data left to the top of buffer */
                memcpy(g_vbi_send.pes_buf_virt_addr, (g_vbi_send.pes_buf_virt_addr + tmp_index),
                        g_vbi_send.vbi_pes_buf_write - tmp_index);

                /* move the write-pointer to the tail of buffer */
                g_vbi_send.vbi_pes_buf_write = g_vbi_send.vbi_pes_buf_write - tmp_index;
            }

            break;
        }

        /* the length of the data is too small */
        if ((tmp_index+VBI_PES_MIN_LEN) > g_vbi_send.vbi_pes_buf_write)
        {
            HI_ERR_DISP("The Vbi Pes data is not enough!\n");

            if (tmp_index != 0)
            {
                /* copy the data left to the top of buffer */
                memcpy(g_vbi_send.pes_buf_virt_addr, (g_vbi_send.pes_buf_virt_addr + tmp_index),
                        g_vbi_send.vbi_pes_buf_write - tmp_index);

                /* move the write-pointer to the tail of buffer */
                g_vbi_send.vbi_pes_buf_write = g_vbi_send.vbi_pes_buf_write - tmp_index;
            }

            break;
        }

        if (0x00 == g_vbi_send.pes_buf_virt_addr[tmp_index])
        {
            if (HI_TRUE == vbi_cc_check_data_packet_head(tmp_index))
            {
                /* get the length of PES packet */
                pes_len_high8 = g_vbi_send.pes_buf_virt_addr[tmp_index + 4];
                pes_len_low8 = g_vbi_send.pes_buf_virt_addr[tmp_index + 5];
                pes_len = (pes_len_high8 << 8) + pes_len_low8;

                if (pes_len <= VBI_PES_MAX_LEN)
                {
                    /* buffer has a complete PES packet */
                    if ((g_vbi_send.vbi_pes_buf_write - tmp_index - 6) >= pes_len)
                    {
                        pes_head_len = g_vbi_send.pes_buf_virt_addr[tmp_index + 8];

                        /* make sure that ebudata flag exists */
                        vbi_ebu_data_packe_parse(tmp_index, fmt, pes_head_len, pes_len);
                        /* TmpIndex, leap over current packet */
                        tmp_index = tmp_index + pes_len + 6;
                        continue;
                    }
                    else
                    {
                        HI_WARN_DISP("The Vbi Pes Packet is not enough!\n");
                        if (tmp_index != 0)
                        {
                            /* copy the left data to the top of buffer */
                            memcpy(g_vbi_send.pes_buf_virt_addr, (g_vbi_send.pes_buf_virt_addr + tmp_index),
                                    g_vbi_send.vbi_pes_buf_write- tmp_index);

                            /* move the write-pointer to the tail of buffer */
                            g_vbi_send.vbi_pes_buf_write = g_vbi_send.vbi_pes_buf_write - tmp_index;
                        }
                        break;
                    }
                }
                else
                {
                    HI_WARN_DISP("The Vbi Pes length data is error, clear buffer!\n");
                    g_vbi_send.vbi_pes_buf_write = 0;
                    break;
                }

            }
        }

        tmp_index++;
    }

    if (tmp_index == g_vbi_send.vbi_pes_buf_write)
    {
        g_vbi_send.vbi_pes_buf_write = 0;
    }

    return HI_SUCCESS;
}

hi_void vbi_ttx_data_correct_and_parse(hi_u32 ttx_data_len, hi_u32 tmp_index)
{
    hi_u32   i = 0;
    hi_bool  data_correct_flag = HI_TRUE;

    for (i = 0; i < ttx_data_len / 46; i++)
    {
        if ((g_ttx_send.pes_buf_virt_addr[tmp_index + i * 46 + 46] == 0x02)
            || (g_ttx_send.pes_buf_virt_addr[tmp_index + i * 46 + 46] == 0x03)
            || (g_ttx_send.pes_buf_virt_addr[tmp_index + i * 46 + 46] == 0xc0)
            || (g_ttx_send.pes_buf_virt_addr[tmp_index + i * 46 + 46] == 0xc1)
            || (g_ttx_send.pes_buf_virt_addr[tmp_index + i * 46 + 46] == 0xc3)
           )
        {
            data_correct_flag = HI_TRUE;
            continue;
        }
        else if (g_ttx_send.pes_buf_virt_addr[tmp_index + i * 46 + 46] == 0xff)
        {
            break;
        }
    }

    /* if the data is valid, analyze, copy it and add it to the queue */
    if ((HI_TRUE == data_correct_flag) && (i != 0))
    {
        drv_vbi_ttx_data_parse((g_ttx_send.pes_buf_virt_addr + tmp_index + 46), ttx_data_len);
    }

    return;
}

hi_void vbi_start_asending_of_ttx_data(hi_void)
{
    if (HI_FALSE == g_ttx_send.ttx_busy_flag)
    {
        if (g_ttx_send.ttx_busy_list != HI_NULL)
        {
            list_add_tail(g_ttx_send.ttx_busy_list, &g_ttx_send.ttx_block_free_head);
            g_ttx_send.ttx_busy_list = NULL;
        }

        /* Not-null of the linked list */
        if (!list_empty(&g_ttx_send.ttx_block_busy_head))
        {
            g_ttx_send.ttx_busy_list = g_ttx_send.ttx_block_busy_head.next;
            g_ttx_send.ttx_busy_block = list_entry(g_ttx_send.ttx_busy_list, ttx_data_block, List);
            list_del(g_ttx_send.ttx_busy_list);

            sdate_vbi_ttx_set_seq(g_ttx_send.ttx_busy_block->ttx_seq_flag);
            /*first config 10 ~21 line ,in order to increase ttx send. */
            g_ttx_send.ttx_busy_block->ttx_data_line = 0x7ff87ff8;
            sdate_vbi_ttx_line_enable(g_ttx_send.ttx_busy_block->ttx_data_line, g_ttx_send.ttx_busy_block->ttx_data_ext_line);
            sdate_vbi_ttx_set_addr((g_ttx_send.ttx_busy_block->ttx_start_addr), (g_ttx_send.ttx_busy_block->ttx_end_addr));
            sdate_vbi_ttx_set_ready();

            g_ttx_send.ttx_busy_flag = HI_TRUE;
        }
    }

    return;
}

static hi_s32 vbi_copy_data_from_user( hi_bool*  send_flag, HI_DRV_DISP_TTX_DATA_S* ttx_data)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u64      buf_end_addr = 0;

    if ((HI_NULL == send_flag)
        || (HI_NULL == ttx_data))
    {
        HI_ERR_DISP("para ptr is NULL.\n");
        return HI_ERR_DISP_NULL_PTR;
    }

    if (g_vbi[HI_DRV_DISP_VBI_TTX].open != HI_TRUE)
    {
        HI_ERR_DISP("Ttx channel isn't opened\n");
        return HI_FAILURE;
    }

    buf_end_addr = (g_ttx_send.ttx_pes_buf_write + ttx_data->u32datalen);
    if (buf_end_addr > TTX_BUFFER_LEN
        || ttx_data->u32datalen > TTX_BUFFER_LEN
        || g_ttx_send.ttx_pes_buf_write > TTX_BUFFER_LEN)
    {
        HI_WARN_DISP("The Ttx Pes buffer is overflow!\n");
        //return HI_ERR_DISP_INVALID_PARA;
    }
    else
    {
        if (HI_NULL == ttx_data->pu8dataaddr)
        {
            HI_ERR_DISP("pstTtxData->pu8DataAddr ptr is null.\n");
            return  HI_ERR_DISP_NULL_PTR;
        }

        ret = osal_copy_from_user((g_ttx_send.pes_buf_virt_addr + g_ttx_send.ttx_pes_buf_write),
                                  (hi_void __user*)ttx_data->pu8dataaddr, ttx_data->u32datalen);
        if (ret != HI_SUCCESS)
        {
            HI_ERR_DISP("copy data from user failed.\n");
            return ret;
        }

        /* move write-pointer to the tail of buffer */
        g_ttx_send.ttx_pes_buf_write = g_ttx_send.ttx_pes_buf_write + ttx_data->u32datalen;
        *send_flag = HI_TRUE;
    }

    return HI_SUCCESS;
}

hi_bool  vbi_check_whether_it_is_ebudata(hi_u32 tmp_index)
{
    hi_bool  ret = HI_FALSE;

    if (((g_ttx_send.pes_buf_virt_addr[tmp_index + 45] >= 0x10) && (g_ttx_send.pes_buf_virt_addr[tmp_index + 45] <= 0x1f))
        || ((g_ttx_send.pes_buf_virt_addr[tmp_index + 45] >= 0x99) && (g_ttx_send.pes_buf_virt_addr[tmp_index + 45] <= 0x9b))
       )
    {
        ret = HI_TRUE;
    }

    return ret;
}

hi_s32 drv_vbi_send_ttx_data(HI_DRV_DISP_TTX_DATA_S* ttx_data)
{
    hi_u32      tmp_index = 0;
    hi_u32      pes_len_low8 = 0, pes_len_high8 = 0, pes_len = 0, ttx_data_len = 0;
    hi_bool     send_flag = HI_FALSE;

    if (HI_NULL == ttx_data)
    {
        HI_ERR_DISP("pstTtxData is NULL.\n");
        return HI_ERR_DISP_NULL_PTR;
    }

    if (HI_SUCCESS != vbi_copy_data_from_user(&send_flag, ttx_data))
    {
        return HI_FAILURE;
    }

    if (g_ttx_send.init_flag != VBI_INIT_FLAG)
    {
        DISP_ERROR("Channel not inited!\n");
        return HI_ERR_DISP_NO_INIT;
    }

    while (tmp_index < g_ttx_send.ttx_pes_buf_write)
    {
        if (list_empty(&g_ttx_send.ttx_block_free_head))
        {
            HI_WARN_DISP("The Ttx buffer is overflow!\n");

            if (tmp_index != 0)
            {
                /* copy the data left to the top of the buffer */
                memcpy(g_ttx_send.pes_buf_virt_addr, (g_ttx_send.pes_buf_virt_addr + tmp_index), g_ttx_send.ttx_pes_buf_write - tmp_index);

                /* move the write-pointer to the tail of the buffer */
                g_ttx_send.ttx_pes_buf_write = g_ttx_send.ttx_pes_buf_write - tmp_index;
            }

            break;
        }

        /* the size of data is too little */
        if ((tmp_index + TTX_PES_MIN_LEN) > g_ttx_send.ttx_pes_buf_write)
        {
            HI_WARN_DISP("The Ttx Pes data is not enough!\n");

            if (tmp_index != 0)
            {
                /* copy the data left to the tail of the buffer */
                memcpy(g_ttx_send.pes_buf_virt_addr, (g_ttx_send.pes_buf_virt_addr + tmp_index), g_ttx_send.ttx_pes_buf_write - tmp_index);

                /* move the write-pointer to the tail of the buffer*/
                g_ttx_send.ttx_pes_buf_write = g_ttx_send.ttx_pes_buf_write - tmp_index;
            }

            break;
        }

        if (0x00 == g_ttx_send.pes_buf_virt_addr[tmp_index])
        {
            if ((g_ttx_send.pes_buf_virt_addr[tmp_index + 1] == 0x00) && (g_ttx_send.pes_buf_virt_addr[tmp_index + 2] == 0x01) && (g_ttx_send.pes_buf_virt_addr[tmp_index + 3] == 0xbd))
            {
                /* get the PES packet length */
                pes_len_high8 = g_ttx_send.pes_buf_virt_addr[tmp_index + 4];
                pes_len_low8 = g_ttx_send.pes_buf_virt_addr[tmp_index + 5];
                pes_len = (pes_len_high8 << 8) + pes_len_low8;

                if (pes_len <= TTX_PES_MAX_LEN)
                {
                    /* In buffer, there is a complete PES packet */
                    if ((g_ttx_send.ttx_pes_buf_write - tmp_index - 6) >= pes_len)
                    {
                        /* query whether it is EBU data */
                        if (HI_TRUE == vbi_check_whether_it_is_ebudata(tmp_index))
                        {
                            ttx_data_len = pes_len - 40;
                            /* check whether each TTX packet is valid or not */
                            vbi_ttx_data_correct_and_parse(ttx_data_len, tmp_index);
                        }

                        /* TmpIndex, jump current packet */
                        tmp_index = tmp_index + pes_len + 6;
                        continue;

                    }
                    else
                    {
                        HI_WARN_DISP("The Ttx Pes Packet is not enough!\n");

                        if (tmp_index != 0)
                        {
                            /* copy the data left to the top of buffer */
                            memcpy(g_ttx_send.pes_buf_virt_addr, (g_ttx_send.pes_buf_virt_addr + tmp_index), g_ttx_send.ttx_pes_buf_write - tmp_index);

                            /* move the write-pointer to the tail of buffer */
                            g_ttx_send.ttx_pes_buf_write = g_ttx_send.ttx_pes_buf_write - tmp_index;
                        }

                        break;
                    }
                }
                else
                {
                    HI_ERR_DISP("The Ttx Pes length data is error, clear buffer!\n");
                    g_ttx_send.ttx_pes_buf_write = 0;
                    break;
                }

            }
        }

        tmp_index++;
    }

    if (tmp_index == g_ttx_send.ttx_pes_buf_write)
    {
        g_ttx_send.ttx_pes_buf_write = 0;
    }

    /* start a sending of TTX data */
    vbi_start_asending_of_ttx_data();

    if (HI_TRUE == send_flag)
    {
        return HI_SUCCESS;
    }
    else
    {
        return HI_FAILURE;
    }
}

hi_s32 drv_vbi_send_data(HI_HANDLE h_vbi, HI_DRV_DISP_FMT_E fmt,HI_DRV_DISP_VBI_DATA_S* vbi_data)
{
    hi_s32 ret = HI_SUCCESS;
    HI_DRV_DISP_TTX_DATA_S ttx_data;
    HI_DRV_DISP_VBI_TYPE_E  vbi_type = HI_DRV_DISP_VBI_TYPE_BUTT;

    memset((hi_void *)&ttx_data, 0x0, sizeof(ttx_data));

    if (HI_NULL == vbi_data)
    {
        HI_ERR_DISP("pstVbiData is NULL.\n");
        return HI_ERR_DISP_NULL_PTR;
    }

    vbi_type = (HI_DRV_DISP_VBI_TYPE_E) h_vbi;
    VBI_CHECK_TYPE(vbi_type);

    switch (vbi_data->etype)
    {
        case HI_DRV_DISP_VBI_TTX:
            ttx_data.pu8dataaddr = vbi_data->pu8dataaddr;
            ttx_data.u32datalen = vbi_data->u32datalen;
            ret = drv_vbi_send_ttx_data(&ttx_data);
            break;
        case HI_DRV_DISP_VBI_CC:
            ret = drv_vbi_send_cc_wss_data(fmt,vbi_data);
            break;
        case HI_DRV_DISP_VBI_WSS:
            /*not support!*/
            break;
        case HI_DRV_DISP_VBI_VCHIP:
        default:
            return HI_ERR_DISP_INVALID_OPT;
    }

    return ret;
}

hi_s32 drv_vbi_init()
{
    hi_u32 cnt = 0;
    hi_disp_vbi_status *status = HI_NULL;

    for (cnt = 0; cnt < HI_DRV_DISP_VBI_TYPE_BUTT; cnt++)
    {
        status = &(g_vbi[cnt]);
        if (HI_NULL == status)
        {
            HI_ERR_DISP("pstStatus is NULL.\n");
            return HI_ERR_DISP_NULL_PTR;
        }

        status->vbi_type = HI_DRV_DISP_VBI_TYPE_BUTT;
        status->open = HI_FALSE;
        status->handle = HI_INVALID_HANDLE;
    }

    return HI_SUCCESS;
}

hi_s32 drv_vbi_de_init()
{
    hi_u32 cnt = 0;
    hi_s32 ret = HI_SUCCESS;
    hi_disp_vbi_status *status = HI_NULL;

    for (cnt = 0; cnt < HI_DRV_DISP_VBI_TYPE_BUTT; cnt++)
    {
        status = &(g_vbi[cnt]);
        if (HI_NULL == status)
        {
            HI_ERR_DISP("pstStatus is NULL.\n");
            return HI_ERR_DISP_NULL_PTR;
        }

        if (status->open)
        {
            switch (status->vbi_type)
            {
                case HI_DRV_DISP_VBI_TTX:
                    ret = drv_vbi_de_init_ttx();
                    break;
                case HI_DRV_DISP_VBI_CC:
                case HI_DRV_DISP_VBI_WSS:
                    ret = drv_vbi_de_init_vbi();
                    break;
                default:
                    ret = HI_ERR_DISP_INVALID_OPT;
            }
        }

        status->vbi_type = HI_DRV_DISP_VBI_TYPE_BUTT;
        status->open = HI_FALSE;
        status->handle = HI_INVALID_HANDLE;
    }

    return HI_SUCCESS;
}
#else
hi_void drv_vbi_vbi_isr(hi_void* h_dst, const HI_DRV_DISP_CALLBACK_INFO_S *info)
{
    return;
}
hi_void drv_vbi_ttx_isr(hi_void* h_dst, const HI_DRV_DISP_CALLBACK_INFO_S *info)
{
    return;
}
hi_s32 drv_vbi_init_ttx(hi_void)
{
    return HI_SUCCESS;
}

hi_s32 drv_vbi_de_init_ttx(hi_void)
{
    return HI_SUCCESS;
}

hi_s32 drv_vbi_init_vbi(hi_void)
{
    return HI_SUCCESS;
}

hi_s32 drv_vbi_de_init_vbi(hi_void)
{
    return HI_SUCCESS;
}

hi_s32 drv_vbi_create_channel(HI_DRV_DISP_VBI_CFG_S* cfg, HI_HANDLE *ph_vbi)
{
    return HI_SUCCESS;
}

hi_s32 drv_vbi_destory_channel(HI_HANDLE h_vbi)
{
    return HI_SUCCESS;
}

hi_s32 drv_vbi_send_data(HI_HANDLE h_vbi, HI_DRV_DISP_FMT_E fmt,HI_DRV_DISP_VBI_DATA_S* vbi_data)
{
    return HI_SUCCESS;
}


hi_s32 drv_vbi_init()
{
    return HI_SUCCESS;
}

hi_s32 drv_vbi_de_init()
{
    return HI_SUCCESS;
}

#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
