
/*
* Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: function define
* Author: disp cast
* Create: 2016-04-12
 */


#include "drv_disp_com.h"
#include "drv_disp_cast.h"
#include "drv_venc_ext.h"
#include "hi_drv_module.h"
#include "drv_disp_priv.h"
#include "drv_display.h"
#include "hi_drv_stat.h"
#include "drv_disp_hal_adp.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */


#ifdef CHIP_TYPE_hi3798mv310
#define VDP_DHD0_WBC_WIDTH_MAX 1280
#define VDP_DHD0_WBC_HEIGHT_MAX 720
#else
#define VDP_DHD0_WBC_WIDTH_MAX 1920
#define VDP_DHD0_WBC_HEIGHT_MAX 1080
#endif

cast_release_ptr g_release_ptr_array[DISP_CAST_BUFFER_MAX_NUMBER];

#define CHECK_NULL_POINTER(ptr) \
{                                \
    if (HI_NULL == (ptr))                    \
    {                            \
        DISP_ERROR("WIN Input null pointer in %s!\n", __FUNCTION__); \
        return HI_ERR_VO_NULL_PTR;  \
    }                             \
}

static hi_s32 cast_check_cfg(HI_DRV_DISP_CAST_CFG_S * cfg, HI_DISP_DISPLAY_INFO_S *info)
{
    CHECK_NULL_POINTER(cfg);
    CHECK_NULL_POINTER(info);

    if (  (cfg->width < DISP_CAST_MIN_W)
        ||(cfg->width > DISP_CAST_MAX_W)
        ||(cfg->height < DISP_CAST_MIN_H)
        ||(cfg->height > DISP_CAST_MAX_H)
        ||( (cfg->width & 0x1)  != 0)
        ||( (cfg->height & 0x3) != 0)
        )
    {
        DISP_ERROR("Cast w= %d or h=%d invalid\n", cfg->width, cfg->height);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (  (cfg->format != HI_DRV_PIX_FMT_NV21)
        &&(cfg->format != HI_DRV_PIX_FMT_NV12)
        &&(cfg->format != HI_DRV_PIX_FMT_NV16_2X1)
        &&(cfg->format != HI_DRV_PIX_FMT_NV61_2X1)
        &&(cfg->format != HI_DRV_PIX_FMT_YUYV)
        &&(cfg->format != HI_DRV_PIX_FMT_YVYU)
        &&(cfg->format != HI_DRV_PIX_FMT_UYVY)
        )
    {
        DISP_ERROR("Cast pixfmt = %d invalid\n", cfg->format);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if (  (cfg->buf_number < DISP_CAST_BUFFER_MIN_NUMBER)
        ||(cfg->buf_number > DISP_CAST_BUFFER_MAX_NUMBER) )
    {
        DISP_ERROR("Cast u32BufNumber =%d invalid\n", cfg->buf_number);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if ((cfg->buseralloc > HI_TRUE)
        ||(cfg->low_delay > HI_TRUE))
    {
        DISP_ERROR("bUserAlloc: %d or bLowDelay =%d invalid.\n", cfg->buseralloc, cfg->low_delay);
        return HI_ERR_DISP_INVALID_PARA;
    }

    if(cfg->buseralloc)
    {
        DISP_ERROR("Cast not support User Alloc memory\n");
        return HI_ERR_DISP_NOT_SUPPORT;
    }

    if (cfg->crop)
    {
        /*do not crop too much, the remain size must > 64*64.*/
        if ((cfg->crop_rect.u32bottomoffset + cfg->crop_rect.u32topoffset)
                > (info->fmt_resolution.height - 64)
                || (cfg->crop_rect.u32leftoffset + cfg->crop_rect.u32rightoffset)
                > (info->fmt_resolution.width - 64))
        {
            DISP_ERROR("Cast crop too much, remain size must be larger than 64.\n");
            return HI_ERR_DISP_NOT_SUPPORT;
        }

        /*not allow to give a zme out larger than 4.*/
        if ((cfg->crop_rect.u32bottomoffset + cfg->crop_rect.u32topoffset)
                > cfg->height / 4
                || (cfg->crop_rect.u32leftoffset + cfg->crop_rect.u32rightoffset)
                > cfg->width / 4)
        {
            DISP_ERROR("Cast crop too much, the max zme out scale is 4.\n");
            return HI_ERR_DISP_NOT_SUPPORT;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 cast_set_frame_demo_part_a(disp_cast *cast_ptr,
                                    HI_DRV_DISP_CAST_CFG_S *cfg,
                                    disp_cast_attr *attr)
{
    HI_DRV_VIDEO_FRAME_S *frame = HI_NULL;
    HI_DRV_VIDEO_PRIVATE_S *priv = HI_NULL;

    // set frame demo
    DISP_MEMSET(attr, 0, sizeof(disp_cast_attr));

    attr->out.width  = (hi_s32)cfg->width;
    attr->out.height = (hi_s32)cfg->height;

    attr->crop = (hi_s32)cfg->crop;
    if  (attr->crop)
    {
        attr->crop_rect = cfg->crop_rect;
    }
    frame = &attr->frame_demo;
    frame->frm_type = HI_DRV_FT_NOT_STEREO;
    frame->pix_format = cfg->format;

    frame->bprogressive = HI_TRUE;
    frame->width  = cfg->width;
    frame->height = cfg->height;
    frame->disp_rect = attr->out;

    priv = (HI_DRV_VIDEO_PRIVATE_S *)&(frame->u32priv[0]);

    priv->u32playtime = 1;

    return HI_SUCCESS;
}

#define CAST_ERROR_DEAL(a, flag)  do{                    \
        if (HI_SUCCESS !=   (a))                    \
        {               \
            return HI_FAILURE;\
        }\
    }while(0);

static hi_s32 cast_get_frame(disp_cast *cast,
                     hi_u32 *buf_id,
                     hi_u32 buf_type,
                     HI_DRV_VIDEO_FRAME_S *frame_info)
{
    CHECK_NULL_POINTER(cast);
    CHECK_NULL_POINTER(buf_id);
    CHECK_NULL_POINTER(frame_info);

    if(buf_type == 1)
    {
        CAST_ERROR_DEAL(bp_get_full_buf(&cast->bp, buf_id), 0);
        CAST_ERROR_DEAL(bp_del_full_buf(&cast->bp, *buf_id), 1);
        CAST_ERROR_DEAL(bp_get_frame(&cast->bp, *buf_id, frame_info), 1);
    } else {
        CAST_ERROR_DEAL(bp_get_empty_buf(&cast->bp, buf_id), 0);
        CAST_ERROR_DEAL(bp_del_empty_buf(&cast->bp, *buf_id), 1);
        CAST_ERROR_DEAL(bp_get_frame(&cast->bp, *buf_id, frame_info), 1);
    }
    return HI_SUCCESS;
}

static hi_bool cast_check_output_size_change(disp_cast *cast,
                                          HI_DRV_VIDEO_FRAME_S *frame)
{
    if ((frame->width == cast->attr.out.width)
        && (frame->height == cast->attr.out.height))
    {
        return HI_FALSE;
    }
    else
    {
        return HI_TRUE;
    }
}

static hi_s32 cast_kthread_re_allocate(hi_void *castmp)
{
    hi_u32 i = 0;
    //HI_DRV_VIDEO_FRAME_S stFrame;
    disp_cast *cast = HI_NULL;

    //memset((void*)&stFrame, 0, sizeof(HI_DRV_VIDEO_FRAME_S));
    cast = (disp_cast *)castmp;

    while (!kthread_should_stop())
    {
        for (i = 0; i < DISP_CAST_BUFFER_MAX_NUMBER; i++)
        {
            /*need to reallocate.*/
            if (osal_atomic_read(&g_release_ptr_array[i].at_release_node_status) == CAST_RETRIVE_NODE_REALLOCATE)
            {
                if (bp_re_alloc_buf(&cast->bp, g_release_ptr_array[i].buf_id))
                    break;

                /*set the node to normal status.*/
                osal_atomic_set(&g_release_ptr_array[i].at_release_node_status, CAST_RETRIVE_NODE_READY_TO_RETURN);
            }
        }

        osal_msleep_uninterruptible(10); /* 10 is time */
    }

    return HI_SUCCESS;
}

static hi_s32 cast_return_frame_to_list(disp_cast *cast)
{
    hi_u32 i = 0;
    hi_s32 n_ret = HI_SUCCESS;

    for (i = 0; i < DISP_CAST_BUFFER_MAX_NUMBER; i++)
    {
        if (osal_atomic_read(&g_release_ptr_array[i].at_release_node_status) == CAST_RETRIVE_NODE_READY_TO_RETURN)
        {
            n_ret = bp_set_buf_reading(&cast->bp, g_release_ptr_array[i].buf_id);
            if (n_ret)
                break;

             n_ret = bp_add_empty_buf(&cast->bp, g_release_ptr_array[i].buf_id);
             if (n_ret)
                break;

             n_ret = bp_set_buf_empty(&cast->bp, g_release_ptr_array[i].buf_id);
             if (n_ret)
                break;

            if (!g_release_ptr_array[i].internal_release)
               cast->cast_release_ok_cnt ++;

            osal_atomic_set(&g_release_ptr_array[i].at_release_node_status, CAST_RETRIVE_NODE_EMPTY);
        }
    }

    return HI_SUCCESS;
}

static hi_s32 cast_create_kthread(disp_cast *cast)
{
    cast->k_thread_release_frame = kthread_create(cast_kthread_re_allocate, cast, "HI_DISP_CastRelease");

    if (IS_ERR(cast->k_thread_release_frame))
    {
        DISP_ERROR("Cast create release thread failed!\n");
        return HI_ERR_DISP_CREATE_ERR;
    }
    else
    {
        wake_up_process(cast->k_thread_release_frame);
    }

    return HI_SUCCESS;
}

hi_s32 disp_cast_create(HI_DRV_DISPLAY_E disp,
                       HI_DISP_DISPLAY_INFO_S *info,
                       HI_DRV_DISP_CAST_CFG_S *cfg,
                       disp_cast** cast_ptr)
{
    HI_DRV_DISP_CALLBACK_TYPE_E call_type;
    HI_DRV_DISP_CALLBACK_S cb1, cb2;
    disp_cast *cast = HI_NULL;
    buf_alloc alloc;
    hi_s32 n_ret = HI_FAILURE;
    disp_intf_operation *function = HI_NULL;

    CHECK_NULL_POINTER(cast_ptr);

    // check cfg
    if( cast_check_cfg(cfg, info) )
    {
        DISP_ERROR("Cast config invalid!\n");
        return HI_ERR_DISP_INVALID_PARA;
    }

    // alloc mem
    cast = (disp_cast *)DISP_MALLOC( sizeof(disp_cast) );
    if (cast == HI_NULL)
    {
        DISP_ERROR("Cast malloc failed!\n");
        return HI_ERR_DISP_MALLOC_FAILED;
    }

    DISP_MEMSET(cast, 0, sizeof(disp_cast));

    // set attr
    if (HI_SUCCESS != cast_set_frame_demo_part_a(cast, cfg, &cast->attr))
    {
        DISP_ERROR("Call CastSetFrameDemoPartA failed!\n");
        goto _ERR_MALLOC;
    }

    // get hal operation
    n_ret = disp_hal_get_operation(&function);
    if (n_ret)
    {
        DISP_ERROR("Cast get hal operation failed!\n");
        goto _ERR_MALLOC;
    }
    memcpy(&cast->intf_opt, function, sizeof(disp_intf_operation));

    // get wbclayer
    n_ret = cast->intf_opt.pf_acquire_wbc_by_chn(disp, &cast->wbc);
    if (n_ret)
    {
        DISP_ERROR("Cast get wbc layer failed!\n");
        goto _ERR_MALLOC;
    }

    // create buffer
    alloc.fb_alloc_mem  = !cfg->buseralloc;
    alloc.data_format  = cfg->format;
    alloc.buf_width  = cfg->width;
    alloc.buf_height = cfg->height;
    alloc.buf_stride = cfg->buf_stride;
    alloc.buf_size = cfg->buf_size;
    n_ret = bp_create(cfg->buf_number, &alloc, &cast->bp);
    if (n_ret)
    {
        DISP_ERROR("Cast alloc buffer failed!\n");
        goto _ERR_MALLOC;
    }

    // register callback
    cb1.hdst = (hi_void *)cast;
    cb1.pfdisp_callback = disp_cast_cb_genarate_frame;
	cb1.ecallbackprior  =  HI_DRV_DISP_CALLBACK_PRIORTY_MIDDLE;
    n_ret = disp_isr_reg_callback(disp, HI_DRV_DISP_C_INTPOS_0_PERCENT, &cb1);
    if (n_ret)
    {
        DISP_ERROR("Cast register work callback failed!\n");
        goto _ERR_BP;
    }

    cb2.hdst = (hi_void*)cast;
    cb2.pfdisp_callback = disp_cast_push_frame;
	cb2.ecallbackprior  =  HI_DRV_DISP_CALLBACK_PRIORTY_MIDDLE;
    if (cfg->low_delay)
    {
        call_type = HI_DRV_DISP_C_DHD0_WBC;
    }
    else
    {
        call_type = HI_DRV_DISP_C_INTPOS_0_PERCENT;
    }

    n_ret = disp_isr_reg_callback(disp, call_type, &cb2);
    if (n_ret)
    {
        DISP_ERROR("Cast register work callback failed!\n");
        goto __ERR_REG_CALLBACK1;
    }

    cast->last_cfg_buf_id= 0;
    cast->last_frame_buf_id = 0;


    cast->to_get_disp_info = HI_TRUE;
    cast->open = HI_TRUE;
    cast->low_delay = cfg->low_delay;
    cast->disp = disp;
    cast->ref = 1;
    *cast_ptr = cast;


    n_ret = cast_create_kthread(cast);
    if (n_ret)
        goto __ERR_REG_CALLBACK2;

    memset((void*)g_release_ptr_array,
            0,
            sizeof(cast_release_ptr )* DISP_CAST_BUFFER_MAX_NUMBER);

    DISP_PRINT("DISP_CastCreate ok\n");
    return HI_SUCCESS;

__ERR_REG_CALLBACK2:
    disp_isr_un_reg_callback(disp, call_type, &cb2);
__ERR_REG_CALLBACK1:
    disp_isr_un_reg_callback(disp, HI_DRV_DISP_C_INTPOS_0_PERCENT, &cb1);
 _ERR_BP:
    bp_destroy(&cast->bp);
_ERR_MALLOC:
    DISP_FREE(cast);
    return n_ret;
}

hi_s32 disp_cast_destroy(hi_void *cast_ptr)
{
    HI_DRV_DISP_CALLBACK_TYPE_E call_type;
    HI_DRV_DISP_CALLBACK_S cb1,cb2;
    disp_cast *cast;

    cast = (disp_cast *)cast_ptr;

    if (cast->k_thread_release_frame != HI_NULL)
    {
        kthread_stop(cast->k_thread_release_frame);
        cast->k_thread_release_frame = NULL;
    }

    // set disable
    cast->enable = HI_FALSE;
    /* release  buffer and stop intr func is asynchronus,
      so we should wait. after intr stop, then release the mmz mem,
       to avoid writing after release. */
    osal_msleep_uninterruptible(100); /* 100 is time */

    cb1.hdst = (hi_void*)cast_ptr;
	cb1.ecallbackprior = HI_DRV_DISP_CALLBACK_PRIORTY_MIDDLE;
    cb1.pfdisp_callback = disp_cast_cb_genarate_frame;
    (hi_void)disp_isr_un_reg_callback(cast->disp, HI_DRV_DISP_C_INTPOS_0_PERCENT, &cb1);

    if (cast->low_delay)
    {
        call_type = HI_DRV_DISP_C_DHD0_WBC;
    }
    else
    {
        call_type = HI_DRV_DISP_C_INTPOS_0_PERCENT;
    }

    cb2.hdst = (hi_void *)cast_ptr;
	cb2.ecallbackprior = HI_DRV_DISP_CALLBACK_PRIORTY_MIDDLE;
    cb2.pfdisp_callback = disp_cast_push_frame;
    (hi_void)disp_isr_un_reg_callback(cast->disp, call_type, &cb2);
    osal_msleep_uninterruptible(60); /* 60 is time */

    // destroy buffer
    bp_destroy(&cast->bp);

    cast->intf_opt.pf_release_wbc(cast->wbc);

    // free mem
    DISP_FREE(cast);

    return HI_SUCCESS;
}


hi_s32 disp_cast_set_enable(hi_void* cast_ptr, hi_bool enable)
{
    disp_cast *cast;

    cast = (disp_cast *)cast_ptr;

    cast->enable = enable;

    DISP_PRINT("DISP_CastSetEnable  bEnable = 0x%x\n", (hi_u32)enable);

    return HI_SUCCESS;
}

hi_s32 disp_cast_get_enable(hi_void* cast_ptr, hi_bool *enable)
{
    disp_cast *cast;

    cast = (disp_cast *)cast_ptr;

    *enable = cast->enable;

    return HI_SUCCESS;
}

hi_s32	disp_cast_set_frame_pts(HI_DRV_VIDEO_FRAME_S *cast_frame,mira_get_pts pts_flag)
{
    hi_u32 cur_time = hi_drv_sys_get_time_stamp_ms();

    cast_frame->u32pts    = cur_time;
    cast_frame->u32srcpts = cur_time;

    return HI_SUCCESS;}

static hi_s32 cast_acquire_frame(disp_cast *cast, HI_DRV_VIDEO_FRAME_S *cast_frame)
{
    hi_s32 n_ret;
    hi_u32 buf_id = 0;
    n_ret = cast_get_frame(cast, &buf_id, 1, cast_frame);
    if (n_ret)
    {
        DISP_WARN("Cast get id failed!\n");
        return n_ret;
    }

    disp_cast_set_frame_pts(cast_frame, MIRA_SET_AQUIRE_PTS);
    return HI_SUCCESS;
}

hi_void cast_send_time_stamps(disp_cast *cast, HI_DRV_VIDEO_FRAME_S *cast_frame, hi_stat_ld_event event)
{
    hi_stat_ld_event_info evt;
    hi_u32 tmp_time = hi_drv_sys_get_time_stamp_ms();

    evt.ld_event = event;
    evt.frame = cast_frame->frame_index;
    evt.handle = (HI_ID_DISP << 16) | ((hi_u32)cast->disp);
    evt.time = tmp_time;
    hi_drv_stat_ld_notify_event(&evt);

    return;
}

static HI_DRV_VIDEO_FRAME_S g_cast_frame;

static hi_s32 cast_push_frame_to_back_mod(disp_cast *cast)
{
    hi_s32 n_ret = HI_SUCCESS;
    //HI_DRV_VIDEO_FRAME_S stCastFrame;
    FN_VENC_PUT_FRAME  venc_queue_pfn  = NULL;
    hi_u32 buf_id = 0;

    /*if this is null, means get frame though unf api, not attach mode.*/
    if (cast->attach_pairs[0].queue_frm)
    {
        while(1)
        {
            cast->cast_acquire_try_cnt ++;

            CAST_ERROR_DEAL(bp_get_full_buf(&cast->bp, &buf_id), 0);
            CAST_ERROR_DEAL(bp_del_full_buf(&cast->bp, buf_id), 1);
            CAST_ERROR_DEAL(bp_get_frame(&cast->bp, buf_id, &g_cast_frame), 1);

            venc_queue_pfn = cast->attach_pairs[0].queue_frm;

            n_ret = venc_queue_pfn(cast->attach_pairs[0].h_sink, &g_cast_frame);
            if (n_ret == HI_SUCCESS)
            {
                cast_send_time_stamps(cast, &g_cast_frame, HI_STAT_LD_EVENT_CAST_FRM_OUT);
                cast->cast_acquire_ok_cnt ++;
            }
            else
            {
                disp_cast_release_frame(cast, &g_cast_frame);
                break;
            }
        }
    }

    return n_ret;
}

hi_s32 disp_cast_acquire_frame(disp_cast *cast_ptr, HI_DRV_VIDEO_FRAME_S *cast_frame)
{
    disp_cast *cast;
    hi_s32 ret = 0;

    cast = (disp_cast *)cast_ptr;

    if(cast->attach_pairs[0].queue_frm)
    {
        DISP_ERROR("attach mode is going on ,you can't acquire through unf api.");
        return HI_FAILURE;
    }

    ret = cast_acquire_frame(cast, cast_frame);

    cast->cast_acquire_try_cnt ++;
    if (HI_SUCCESS == ret)
    {
        cast_send_time_stamps(cast, cast_frame, HI_STAT_LD_EVENT_CAST_FRM_OUT);
        cast->cast_acquire_ok_cnt ++;
    }

#if 0
       DISP_ERROR("cast acquire bufid=0x%x, w=%d,h=%d,index=%d, y=0x%x, c=0x%x\n",
       u32BufId,
       cast_frame->width,
       cast_frame->height,
       cast_frame->u32frmcnt,
       cast_frame->stbufaddr[0].phy_addr_y,
       cast_frame->stbufaddr[0].phy_addr_c
       );
#endif

   return ret;
}

hi_s32 disp_cast_get_buf_id(disp_cast *cast_ptr,
                        HI_DRV_VIDEO_FRAME_S *cast_frame,
                        hi_u32 *buf_id)
{
    hi_s32 ret;

    ret = bp_get_idfrom_frm_info(&(cast_ptr->bp),cast_frame,buf_id);
    if (ret != HI_SUCCESS)
    {
        DISP_ERROR("Get BufId Failed\n");
        return HI_FAILURE;

    }
    return HI_SUCCESS;
}


hi_s32 cast_add_node2_reallocate_list(hi_u32 buf_id,
                                   hi_bool internal_release,
                                   hi_bool need_re_allocate)
{
    hi_u32 i = 0;

    for (i = 0; i < DISP_CAST_BUFFER_MAX_NUMBER; i++)
    {
        /*search the empty node.*/
        if (!osal_atomic_read(&g_release_ptr_array[i].at_release_node_status))
        {
            g_release_ptr_array[i].buf_id = buf_id;
            g_release_ptr_array[i].internal_release = internal_release;

            if (need_re_allocate)
            {
                //need to reallocate.
                osal_atomic_set(&g_release_ptr_array[i].at_release_node_status, CAST_RETRIVE_NODE_REALLOCATE);
            }
            else
            {
                /*set the flag, just put it to nomal empty list, no
                 *necessary to reallocate.
                 */
                osal_atomic_set(&g_release_ptr_array[i].at_release_node_status, CAST_RETRIVE_NODE_READY_TO_RETURN);
            }

            break;
        }
    }
    return HI_SUCCESS;
}

hi_s32 disp_cast_release_frame(disp_cast *cast_ptr, HI_DRV_VIDEO_FRAME_S *cast_frame)
{
    disp_cast *cast;
    hi_u32 buf_id;
    hi_s32 n_ret = HI_SUCCESS;
    hi_bool need_re_allocate = HI_FALSE;

    cast = (disp_cast *)cast_ptr;
    cast->cast_release_try_cnt ++;

    n_ret = disp_cast_get_buf_id(cast_ptr, cast_frame, &buf_id);
    if (n_ret)
    {
        DISP_WARN("Cast release frame invalid!\n");
        return n_ret;
    }

    if (cast_check_output_size_change(cast, cast_frame))
    {
        need_re_allocate = HI_TRUE;
    }
    else
    {
        need_re_allocate = HI_FALSE;
    }

    n_ret = cast_add_node2_reallocate_list(buf_id, HI_FALSE, need_re_allocate);
    if (HI_SUCCESS != n_ret)
    {
        DISP_ERROR("Call Cast_AddNode2ReallocateList failed!\n");
        return n_ret;
    }

    return HI_SUCCESS;
}

hi_s32 disp_set_frame_demo_part_b(disp_cast *cast_ptr,
                             hi_u32 rate,
                             HI_DRV_COLOR_SPACE_E color_space,
                             HI_DRV_VIDEO_FRAME_S *frame)
{
    disp_cast *cast;
    HI_DRV_VIDEO_PRIVATE_S *priv;

    cast = (disp_cast *)cast_ptr;
    priv = (HI_DRV_VIDEO_PRIVATE_S *)&(frame->u32priv[0]);

    //venc and vpss and others,they want a rate * 1000;
    frame->u32framerate = rate * 10;
    priv->color_space = color_space;

    return HI_SUCCESS;
}

hi_void disp_cast_cbset_disp_mode(disp_cast *cast_ptr,
                               const HI_DRV_DISP_CALLBACK_INFO_S *info)
{
    disp_cast *cast;
    hi_u32 rate;

    cast = (disp_cast *)cast_ptr;

    // set display info
    cast->disp_info = info->disp_info;

    cast->attr.in.x = cast->attr.crop_rect.u32leftoffset;
    cast->attr.in.y = cast->attr.crop_rect.u32topoffset;

    cast->attr.in.width = info->disp_info.fmt_resolution.width
                                    - cast->attr.crop_rect.u32leftoffset
                                    - cast->attr.crop_rect.u32rightoffset;

    cast->attr.in.height = info->disp_info.fmt_resolution.height
                                    - cast->attr.crop_rect.u32topoffset
                                    - cast->attr.crop_rect.u32bottomoffset;



#if defined(CHIP_TYPE_hi3798cv200)
    if ((info->disp_info.fmt >= HI_DRV_DISP_FMT_PAL  )
        && (info->disp_info.fmt <= HI_DRV_DISP_FMT_1440x480i_60 ))
    {
        cast->attr.in.x = cast->attr.in.x*2;
        cast->attr.in.width = cast->attr.in.width*2;
    }
#endif

    cast->attr.interlace = info->disp_info.interlace;
    cast->attr.in_color_space = info->disp_info.color_space;
    //Todo
    if (cast->attr.out.height >= 720
        && cast->attr.out.width >= 1280)
    {
        cast->attr.out_color_space = HI_DRV_CS_BT709_YUV_LIMITED;
    }
    else
    {
        cast->attr.out_color_space = HI_DRV_CS_BT601_YUV_LIMITED;
    }

    rate = info->disp_info.refresh_rate;
    cast->attr.in_rate = rate;

    cast->periods = 1;
    while(rate > DISP_CAST_MAX_FRAME_RATE)
    {
        cast->periods = cast->periods << 1;
        rate = rate >> 1;
    }

    cast->attr.out_rate = rate;
    if (HI_SUCCESS != disp_set_frame_demo_part_b(cast_ptr, rate, cast->attr.out_color_space, &cast->attr.frame_demo))
    {
        DISP_ERROR("Call DispSetFrameDemoPartB failed!\n");
    }

    DISP_PRINT("CAST: iw=%d, ih=%d, ow=%d, oh=%d, or=%d\n ",
                cast->attr.in.width,
                cast->attr.in.height,
                cast->attr.out.width,
                cast->attr.out.height,
                rate);
    return ;
}


hi_s32 disp_cast_set_frame_info(disp_cast *cast, HI_DRV_VIDEO_FRAME_S *cur_frame)
{
    HI_DRV_VIDEO_PRIVATE_S *priv_frame = HI_NULL;
    HI_DRV_VIDEO_FRAME_S *frame = HI_NULL;

    cast->attr.frame_demo.frame_index = cast->frame_cnt;
    cast->attr.frame_demo.stbufaddr[0] = cur_frame->stbufaddr[0];

    frame = &cast->attr.frame_demo;
    frame->width  = cur_frame->width;
    frame->height = cur_frame->height;
    frame->disp_rect = cur_frame->disp_rect;

    frame->enfieldmode = HI_DRV_FIELD_ALL;
    frame->htunnelsrc = (HI_ID_DISP << 16) | ((hi_u32)cast->disp);

    if (cast->low_delay)
    {
        frame->u32tunnelphyaddr = cur_frame->u32tunnelphyaddr;
    }
    else
    {
        frame->u32tunnelphyaddr = 0;
    }

    priv_frame = (HI_DRV_VIDEO_PRIVATE_S *)&(cast->attr.frame_demo.u32priv[0]);
    priv_frame->u32frmcnt   = cast->frame_cnt;
    priv_frame->u32bufferid = cast->last_cfg_buf_id;

    cast->attr.frame_demo.u32pts = hi_drv_sys_get_time_stamp_ms();

    return HI_SUCCESS;
}


hi_s32 disp_cast_send_task(disp_cast *cast)
{
    disp_cast_attr *attr = HI_NULL;
    HI_RECT_S out;
    hi_u32 *u32_tunnel_vir_addr = HI_NULL;

    attr = &cast->attr;

    out.width = attr->frame_demo.width;
    out.height = attr->frame_demo.height;
    out.x  = 0;
    out.y  = 0;

    // config pixformat
    cast->intf_opt.pf_set_wbc_pix_fmt(cast->wbc, attr->frame_demo.pix_format);
    cast->intf_opt.pf_set_wbc_iorect(cast->wbc, &cast->disp_info, &attr->in, &out);

    cast->intf_opt.pf_set_wbc3_dinfo(cast->wbc, &cast->disp_info, &attr->in, &out);
    // config csc
    cast->intf_opt.pf_set_wbc_color_space(cast->wbc, attr->in_color_space, HI_DRV_CS_BT709_YUV_LIMITED);

    // config addr
    cast->intf_opt.pf_set_wbc_addr(cast->wbc, &(attr->frame_demo.stbufaddr[0]));
    cast->intf_opt.pf_set_wbc_low_dly_enable(cast->wbc, cast->low_delay);

    if (cast->low_delay)
    {
        u32_tunnel_vir_addr = (hi_u32*)phys_to_virt(attr->frame_demo.u32tunnelphyaddr);

        *u32_tunnel_vir_addr = 0;
        cast->intf_opt.pf_set_wbc_partfns_line_num(cast->wbc,
                                    (out.height *
                                    DISP_CAST_LOWDLY_THRESHOLD_NUMERATOR) / DISP_CAST_LOWDLY_THRESHOLD_DENOMINATOR);


        cast->intf_opt.pf_set_wbc_line_num_interval(cast->wbc, DISP_CAST_LOWDLY_LINENUM_INTERVAL);
        cast->intf_opt.pf_set_wbc_line_addr(cast->wbc, attr->frame_demo.u32tunnelphyaddr);
    }

	#ifdef	CFG_VDP_MMU_SUPPORT
    cast->intf_opt.pf_set_wbc_smmu_bypass(cast->wbc, HI_FALSE);
    #else
    cast->intf_opt.pf_set_wbc_smmu_bypass(cast->wbc, HI_TRUE);
    #endif
    // set enable
    cast->intf_opt.pf_set_wbc_enable(cast->wbc, HI_TRUE);
    cast->intf_opt.pf_update_wbc(cast->wbc);

    return HI_SUCCESS;
}

hi_void disp_cast_push_frame(hi_void *cast_ptr, const HI_DRV_DISP_CALLBACK_INFO_S *info)
{
    disp_cast *cast = HI_NULL;
    hi_u32 buf_id, buf_state = 0;
    hi_s32 n_ret;
    hi_u32 frame_wbc_stage = 0;

#ifdef CHIP_TYPE_hi3798mv310
    hi_bool interrupt_valid = HI_FALSE;
    hi_u32 wbc_line_num = 0;

    n_ret = disp_hal_get_port_int_valid_and_wbc_line_num(&interrupt_valid, &wbc_line_num);
    if ((HI_SUCCESS != n_ret) || (HI_FALSE == interrupt_valid))
    {
        DISP_INFO("no response the err Interrunpt, WbcLine:%d\n", wbc_line_num);
        return;
    }

#endif

    cast = (disp_cast *)cast_ptr;

    if ( (info->eeventtype == HI_DRV_DISP_C_PREPARE_CLOSE)
        ||(info->eeventtype == HI_DRV_DISP_C_PREPARE_TO_PEND)
       )
    {
        cast->masked  = HI_TRUE;
    }
    else
    {
        cast->masked  = HI_FALSE;
    }

    if(!cast->enable || cast->masked)
    {
        return;
    }

    if (osal_atomic_read(&cast->buf_busy))
    {
        return;
    }
    else
    {
        osal_atomic_set(&cast->buf_busy, HI_TRUE);
    }

    {
        cast->cast_intr_cnt++;
        while (1)
        {
            buf_state = cast->low_delay ? CAST_BUFFER_STATE_IN_CFGLIST_WRITING:CAST_BUFFER_STATE_IN_CFGLIST_WRITE_FINISH;

            n_ret = bp_get_cfg_writing_buf(&cast->bp,
                                       &buf_id,
                                       buf_state,
                                       &frame_wbc_stage);
            if (n_ret)
            {
                DISP_WARN("Cast Get cfg  buf failed!\n");
                break;
            }

            (hi_void)bp_del_cfg_writing_buf(&cast->bp,buf_id);
            n_ret = bp_add_full_buf(&cast->bp, buf_id);
            if(n_ret)
            {
                (hi_void)bp_set_buf_reading(&cast->bp, buf_id);
                (hi_void)bp_add_empty_buf(&cast->bp, buf_id);
                DISP_ERROR("Cast ADD buf failed!\n");
                break;
            }
        }

        if (HI_SUCCESS != cast_push_frame_to_back_mod(cast))
        {
            DISP_WARN("Call Cast_PushFrameToBackMod failed!\n");
        }
    }

    osal_atomic_set(&cast->buf_busy, HI_FALSE);

    return;
}

HI_DRV_VIDEO_FRAME_S g_cur_frame;

hi_void disp_cast_cb_genarate_frame(hi_void *cast_ptr, const HI_DRV_DISP_CALLBACK_INFO_S *info)
{
    //HI_DRV_VIDEO_FRAME_S stCurFrame;
    disp_cast *cast = HI_NULL;
    hi_s32 n_ret;

    if (HI_NULL == cast_ptr)
    {
        DISP_ERROR("cast_ptr is null.\n");
        return;
    }

    cast = (disp_cast *)cast_ptr;

    if ( (info->eeventtype == HI_DRV_DISP_C_PREPARE_CLOSE)
        ||(info->eeventtype == HI_DRV_DISP_C_PREPARE_TO_PEND)
       )
    {
        cast->masked  = HI_TRUE;
        cast->to_get_disp_info = HI_TRUE;
    }
    else
    {
        cast->masked  = HI_FALSE;
    }

    if (info->eeventtype == HI_DRV_DISP_C_OPEN)
    {
        cast->to_get_disp_info = HI_TRUE;
    }

    if (cast->to_get_disp_info)
    {
        disp_cast_cbset_disp_mode(cast_ptr, info);

        cast->to_get_disp_info = HI_FALSE;
    }

    if (cast->schedule_wbc)
    {
        cast->intf_opt.pf_set_wbc_enable(cast->wbc, HI_FALSE);
        cast->intf_opt.pf_update_wbc(cast->wbc);
        cast->schedule_wbc_status = HI_TRUE;
        return ;
    }

    // check state
    if(!cast->enable || cast->masked)
    {
        cast->intf_opt.pf_set_wbc_enable(cast->wbc, HI_FALSE);
        cast->intf_opt.pf_set_wbc_low_dly_enable(DISP_WBC_00, HI_FALSE);
        cast->intf_opt.pf_update_wbc(cast->wbc);
        return;
    }


    /*when isr and process conflict, try to lock.*/
    if (osal_atomic_read(&cast->buf_busy))
    {
        return;
    }
    else
    {
        osal_atomic_set(&cast->buf_busy, HI_TRUE);
    }

    if (info->eeventtype == HI_DRV_DISP_C_VT_INT)
    {
        hi_u32 buf_id;

        /*stop wbc first, because in 50hz condition. every 2 times, one wbc occurs.*/
        cast->intf_opt.pf_set_wbc_enable(cast->wbc, HI_FALSE);
        cast->intf_opt.pf_update_wbc(cast->wbc);

        /*retrieve all the frame release by venc or reallocated frame.*/
        (hi_void)cast_return_frame_to_list(cast);

        cast->task_count++;

        if (0 == cast->periods || 0 == cast->disp_info.refresh_rate)
        {
            DISP_ERROR("Denominator may be zero !\n");
            return;
        }
        /*since we may get 25pfs in 50hz fmt, so a interval is necessary.*/
        if ( ((cast->task_count % cast->periods) == 0)
             && (cast->periods > 1)
            )
        {
            osal_atomic_set(&cast->buf_busy, HI_FALSE);
            return;
        }

        n_ret = cast_get_frame(cast, &buf_id, 0, &g_cur_frame);
        if (n_ret)
        {
            DISP_WARN("Cast get empty id failed!\n");
            osal_atomic_set(&cast->buf_busy, HI_FALSE);
            return;
        }

        while(cast_check_output_size_change(cast, &g_cur_frame))
        {
            cast_add_node2_reallocate_list(buf_id, HI_TRUE, HI_TRUE);
            n_ret = cast_get_frame(cast, &buf_id, 0, &g_cur_frame);
            if (n_ret)
            {
                osal_atomic_set(&cast->buf_busy, HI_FALSE);
                DISP_WARN("Cast get empty id failed!\n");
                return;
            }
        }

        n_ret = bp_add_cfg_writing_buf(&cast->bp, buf_id);
        if (n_ret)
        {
            osal_atomic_set(&cast->buf_busy, HI_FALSE);
            DISP_WARN("Cast add to cfg list  failed!\n");
            return;
        }

        cast->last_cfg_buf_id = buf_id;
        cast->frame_cnt++;

        (hi_void)disp_cast_set_frame_info(cast, &g_cur_frame);

        (hi_void)disp_cast_set_frame_pts(&cast->attr.frame_demo, MIRA_SET_CREATE_PTS);

        cast->attr.frame_demo.u32pts += 100000 / cast->disp_info.refresh_rate;
        cast->attr.frame_demo.u32srcpts = cast->attr.frame_demo.u32pts;

        (hi_void)disp_cast_send_task(cast);

        /*all the frame node's stage ++ */
        bp_increase_all_cfg_writing_state(&cast->bp, HI_NULL);

        /*get the frame writing now, and send time stamp for stastics.*/
        n_ret = bp_get_cfg_writing_buf_just_writing(&cast->bp, &buf_id, CAST_BUFFER_STATE_IN_CFGLIST_WRITING);
        if ((!n_ret) && (!bp_get_frame(&cast->bp, buf_id, &g_cur_frame)))
        {
                cast_send_time_stamps(cast, &g_cur_frame, HI_STAT_LD_EVENT_CAST_FRM_BEGIN);
        }
        (hi_void) bp_set_frame(&cast->bp, cast->last_cfg_buf_id, &cast->attr.frame_demo);
    }

    osal_atomic_set(&cast->buf_busy, HI_FALSE);
    return;
}

hi_s32 disp_cast_attach_sink(disp_cast *cast_ptr, HI_HANDLE h_sink)
{
    hi_mod_id mod_id;
    disp_cast *cast = HI_NULL;
    hi_s32 ret;
    VENC_EXPORT_FUNC_S *ven_func = HI_NULL;
    hi_u32  attach_index  = 0, cnt = 0;

    cast = (disp_cast *)cast_ptr;
    mod_id = (hi_mod_id)((h_sink & 0xff0000) >> 16);

    while (osal_atomic_read(&cast->buf_busy)) {
        osal_msleep_uninterruptible(10); /* 10 is time */
        cnt ++;
        if (cnt > 200)
        {
            DISP_ERROR("DISP_Cast_AttachSink timeout.\n");
            return HI_ERR_DISP_TIMEOUT;
        }
    }
    osal_atomic_set(&cast->buf_busy, HI_TRUE);

    if (HI_ID_VENC == mod_id) {

        ret = HI_DRV_MODULE_GetFunction(mod_id,(hi_void**)&(ven_func));
        if (HI_SUCCESS != ret) {
            DISP_ERROR("Get null venc ptr when cast.\n");
            osal_atomic_set(&cast->buf_busy, HI_FALSE);
            return HI_ERR_DISP_NULL_PTR;
        }

        for (attach_index = 0; attach_index < DISPLAY_ATTACH_CNT_MAX; attach_index++) {
            if (cast->attach_pairs[attach_index].h_sink == h_sink) {
                cast->attached = HI_TRUE;
                osal_atomic_set(&cast->buf_busy, HI_FALSE);
                return HI_SUCCESS;
            }
        }

        for (attach_index = 0; attach_index < DISPLAY_ATTACH_CNT_MAX; attach_index++) {
            if (cast->attach_pairs[attach_index].h_sink == 0) {
                cast->attach_pairs[attach_index].h_sink = h_sink;
                cast->attach_pairs[attach_index].queue_frm = ven_func->pfnvencqueueframe;
                cast->attach_pairs[attach_index].dequeue_frame = HI_NULL;
                break;
            }
        }

        if (attach_index == DISPLAY_ATTACH_CNT_MAX) {
            ret = HI_FAILURE;
        }
        else{
            ret = HI_SUCCESS;
            cast->attached = HI_TRUE;
        }
    } else {
       ret = HI_FAILURE;
    }

    /*we support dynamic attach and dettach without disabling cast(as a producer),
      when attach again,  we should reset all the buffer, or else
      there is no empty buffer node, because cast enabled and full buf full.*/
    if (ret == HI_SUCCESS) {
        /* osal_msleep_uninterruptible 60 ms, for when reset we should wait until wbc stop to write back. */
        osal_msleep_uninterruptible(60);

        (hi_void)bp_reset(&cast->bp);
        /*because in the first, venc and cast output resolutin
         *may not consistent.
         */
        (hi_void)bp_re_alloc_all_buf(&cast->bp);
        memset((void*)g_release_ptr_array,
        0,
        sizeof(cast_release_ptr) * DISP_CAST_BUFFER_MAX_NUMBER);
    }

    osal_atomic_set(&cast->buf_busy, HI_FALSE);
    return ret;
}

hi_s32 disp_cast_de_attach_sink(disp_cast *cast_ptr, HI_HANDLE h_sink)
{
    hi_mod_id mod_id;
    disp_cast *cast;
    hi_s32 ret;
    hi_u32  attach_index  = 0, cnt = 0;

    cast = (disp_cast *)cast_ptr;
    mod_id = (hi_mod_id)((h_sink & 0xff0000) >> 16);

    while (osal_atomic_read(&cast->buf_busy)) {
        osal_msleep_uninterruptible(10); /* 10 is time */
        cnt ++;
        if (cnt > 200) { /* 200 is count */
            DISP_ERROR("DISP_Cast_AttachSink timeout.\n");
            return HI_ERR_DISP_TIMEOUT;
        }
    }
    osal_atomic_set(&cast->buf_busy, HI_TRUE);

    if ( HI_ID_VENC == mod_id ) {

        for (attach_index = 0; attach_index < DISPLAY_ATTACH_CNT_MAX; attach_index++) {
            if (cast->attach_pairs[attach_index].h_sink == h_sink) {
                cast->attach_pairs[attach_index].h_sink = 0;
                cast->attach_pairs[attach_index].queue_frm = HI_NULL;
                cast->attach_pairs[attach_index].dequeue_frame = HI_NULL;
                 break;
            }
        }

        if (attach_index == DISPLAY_ATTACH_CNT_MAX) {
            ret = HI_FAILURE;
        }
        else {
            ret = HI_SUCCESS;
            cast->attached = HI_FALSE;
        }

    } else {
       ret = HI_FAILURE;
    }

    /*as a result of asynchrounous bettween Cast and venc,
     *we should wait ,because if venc detach and return, cast may keep
     * writing  to venc.
     */
    osal_msleep_uninterruptible(40); /* 40 is time */
    osal_atomic_set(&cast->buf_busy, HI_FALSE);

    return ret;
}

static hi_void cast_update_attr(disp_cast *cast)
{
    cast->bp.alloc.buf_width = cast->attr.out.width;
    cast->bp.alloc.buf_height = cast->attr.out.height;

    return;
}

hi_s32 disp_cast_set_attr(disp_cast *cast_ptr, HI_DRV_DISP_Cast_Attr_S *cast_attr)
{
    disp_cast *cast = HI_NULL;

    cast = (disp_cast *)cast_ptr;

    cast->attr.out.width  = cast_attr->width;
    cast->attr.out.height = cast_attr->height;

    cast_update_attr(cast);

    return HI_SUCCESS;
}

hi_s32 disp_cast_get_attr(disp_cast *cast_ptr, HI_DRV_DISP_Cast_Attr_S *cast_attr)
{
    disp_cast *cast = HI_NULL;

    cast = (disp_cast *)cast_ptr;

    cast_attr->width = cast->attr.out.width;
    cast_attr->height = cast->attr.out.height;

    return HI_SUCCESS;
}

#define SNAPSHOT_MAGIC (0x534e4150uL) /* ASCII code of "SNAP" */

static hi_u32 g_frame_cnt = 1;

hi_s32 DISP_Acquire_Snapshot(HI_DRV_DISPLAY_E disp, hi_void **snapshot_handle, HI_DRV_VIDEO_FRAME_S *frame)
{
    buf_alloc alloc;
    hi_s32 ret;
    HI_DRV_VIDEO_FRAME_S* video_frame = frame;
    disp_intf_operation *func = HI_NULL;
    disp_wbc wbc;
    HI_DISP_DISPLAY_INFO_S info;
    hi_u32 buf_id = 0;
    disp_snapshot_priv_frame *priv = HI_NULL;
    hi_s64 pts;
    disp_snapshot *snapshot = HI_NULL;
    hi_bool  buf_alloc = 0;
    HI_RECT_S output_size = {0};
    hi_u32  pixel_repeat_times = 0;

    *snapshot_handle  = 0;
    memset((hi_void*)&info, 0, sizeof(HI_DISP_DISPLAY_INFO_S));

    snapshot = (disp_snapshot*)DISP_MALLOC(sizeof(disp_snapshot));
    if (snapshot == HI_NULL)
    {
        DISP_ERROR("snapshot malloc failed!\n");
        return HI_ERR_DISP_MALLOC_FAILED;
    }

    DISP_MEMSET(snapshot, 0, sizeof(disp_snapshot));
    ret = disp_hal_get_operation(&func);
    if (ret)
    {
        goto __ERR_EXIT__;
    }

    ret = func->pf_acquire_wbc_by_chn(disp, &wbc);
    if (ret) {
        if (wbc >= DISP_WBC_BUTT)
            goto __ERR_EXIT__;
    }

    (hi_void)disp_get_display_info( disp, &info);
    output_size = info.fmt_resolution;

    if (output_size.width > VDP_DHD0_WBC_WIDTH_MAX)
    {
        output_size.width = VDP_DHD0_WBC_WIDTH_MAX;
    }

    if (output_size.height > VDP_DHD0_WBC_HEIGHT_MAX)
    {
        output_size.height = VDP_DHD0_WBC_HEIGHT_MAX;
    }

    alloc.fb_alloc_mem = HI_TRUE;
    alloc.data_format = HI_DRV_PIX_FMT_NV21;
    alloc.buf_height = output_size.height;
    alloc.buf_width =  output_size.width;
    alloc.buf_stride = 0;
    ret = bp_create(1, &alloc, &snapshot->bp);
    if (ret)
    {
        goto __ERR_EXIT__;
    }

    buf_alloc = 1;
    ret = bp_get_empty_buf(&snapshot->bp, &buf_id);
    if (ret)
    {
        goto __ERR_EXIT__;
    }

    ret = bp_del_empty_buf(&snapshot->bp, buf_id);
    if (ret)
    {
        goto __ERR_EXIT__;
    }

    ret = bp_get_frame(&snapshot->bp, buf_id, video_frame);
    if (ret)
    {
        goto __ERR_EXIT__;
    }

    video_frame->width  = output_size.width;
    video_frame->height = output_size.height;
    video_frame->u32aspectwidth = 16;
    video_frame->u32aspectheight = 9;
    video_frame->u32framerate = 0;
    video_frame->pix_format = HI_DRV_PIX_FMT_NV21;
    video_frame->bprogressive = HI_TRUE;
    video_frame->enfieldmode = HI_DRV_FIELD_ALL;
    video_frame->btopfieldfirst = 0;
    video_frame->disp_rect = output_size;
    video_frame->frm_type = HI_DRV_FT_NOT_STEREO;
    video_frame->frame_index = 0;
    memset(video_frame->u32priv, 0, sizeof(video_frame->u32priv));
    // config pixformat
    func->pf_set_wbc_pix_fmt(wbc, HI_DRV_PIX_FMT_NV21);

    /*FIXME:  ourrect  why be stInfo.stFmtResolution? error?*/

    (hi_void)func->pf_get_pixel_repeat_times_according_fmt(HI_DRV_DISPLAY_0,
                                              info.fmt,
                                              &pixel_repeat_times);

    info.stpixelfmtresolution.width = info.stpixelfmtresolution.width * pixel_repeat_times;
    info.stpixelfmtresolution.x = info.stpixelfmtresolution.x * pixel_repeat_times;

    func->pf_set_wbc_iorect(wbc, &info, &info.stpixelfmtresolution, &output_size);

    func->pf_set_wbc3_dinfo(wbc, &info, &info.stpixelfmtresolution, &output_size);

    // config csc
    func->pf_set_wbc_color_space(wbc, info.color_space, HI_DRV_CS_BT709_YUV_LIMITED);

    // config addr
    func->pf_set_wbc_addr(wbc, &(video_frame->stbufaddr[0]));

#ifdef  CFG_VDP_MMU_SUPPORT
    func->pf_set_wbc_smmu_bypass(wbc, HI_FALSE);
#else
    func->pf_set_wbc_smmu_bypass(wbc, HI_TRUE);
#endif

    // set enable FIXME!!!
    func->pf_set_wbc_enable(wbc, HI_TRUE);
    func->pf_update_wbc(wbc);

    /*we should wait util wbc finished ,else acquire api may get useless data.*/
    if (!info.refresh_rate) {
        osal_msleep_uninterruptible(60); /* 60 is time */
    }
    else {
        osal_msleep_uninterruptible((100000 / info.refresh_rate + 1) * 2); /* 100000 is time, 2 is double */
    }

    /* set pts. */
    pts = hi_drv_sys_get_time_stamp_ms();

    frame->u32pts    = pts;
    frame->u32srcpts = pts;

    /* construct some private info. */
    priv = (disp_snapshot_priv_frame*)&(frame->u32priv[0]);
    priv->priv_info.u32frmcnt = g_frame_cnt ++;
    priv->magic = SNAPSHOT_MAGIC;

    snapshot->work = 1;

    *snapshot_handle  = (hi_void*)snapshot;
    return HI_SUCCESS;

__ERR_EXIT__:
    if (buf_alloc)
        bp_destroy(&snapshot->bp);

    DISP_FREE(snapshot);
    return HI_FAILURE;
}

hi_s32 DISP_Release_Snapshot(HI_DRV_DISPLAY_E disp, hi_void* snapshot_handle, HI_DRV_VIDEO_FRAME_S *frame)
{
    disp_snapshot_priv_frame *priv = HI_NULL;
    disp_snapshot *snapshot = HI_NULL;

    CHECK_NULL_POINTER(snapshot_handle);
    CHECK_NULL_POINTER(frame);

    priv = (disp_snapshot_priv_frame *)&(frame->u32priv[0]);
    if (priv->magic != SNAPSHOT_MAGIC)
    {
        DISP_ERROR("Magic Invalid\n");
            return HI_ERR_DISP_INVALID_PARA;
    }

    snapshot = (disp_snapshot *)snapshot_handle;

     if (snapshot->work == 0)
        return HI_SUCCESS;

    snapshot->work = 0;
    bp_destroy(&snapshot->bp);
    DISP_FREE(snapshot);
    return 0;
}


hi_s32 disp_snapshot_destroy(hi_void* snapshot_ptr)
{
    disp_snapshot *snapshot = HI_NULL;

    CHECK_NULL_POINTER(snapshot_ptr);

    snapshot = (disp_snapshot *)snapshot_ptr;
    if (snapshot->work == 0)
        return HI_SUCCESS;

    /*when we destroy, wbc may be going on,so we wait.*/
    osal_msleep_uninterruptible(40); /* 40 is time */

    snapshot->work = 0;
    bp_destroy(&snapshot->bp);
    DISP_FREE(snapshot);

    return HI_SUCCESS;
}

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */
