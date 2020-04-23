/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb proc
 * Author: sdk
 * Create: 2016-01-01
 */

#ifdef CONFIG_GFX_PROC_SUPPORT
#include "hi_osal.h"

#include "drv_hifb_proc.h"
#include "drv_hifb_osr.h"
#include "drv_hifb_mem.h"
#include "drv_hifb_config.h"
#include "drv_hifb_fence.h"
#include "drv_hifb_debug.h"
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
#include "drv_hifb_wbc.h"
#endif
/* **************************** Macro Definition ************************************************** */

/* ************************** Structure Definition ************************************************ */

/* ********************* Global Variable declaration ********************************************** */
static const hi_char *s_pszFmtName[] = {
    "RGB565",   "RGB888",   "KRGB444",  "KRGB555",  "KRGB888", "ARGB4444", "ARGB1555", "ARGB8888", "ARGB8565",
    "RGBA4444", "RGBA5551", "RGBA5658", "RGBA8888", "BGR565",  "BGR888",   "ABGR4444", "ABGR1555", "ABGR8888",
    "ABGR8565", "KBGR444",  "KBGR555",  "KBGR888",  "1BPP",    "2BPP",     "4BPP",     "8BPP",     "ACLUT44",
    "ACLUT88",  "PUYVY",    "PYUYV",    "PYVYU",    "YUV888",  "AYUV8888", "YUVA8888", "BUTT"
};

const static hi_char *gs_LayerName[] = {
    "layer_hd_0", "layer_hd_1", "layer_hd_2", "layer_hd_3", "layer_sd_0", "layer_sd_1",  "layer_sd_2",
    "layer_sd_3", "layer_ad_0", "layer_ad_1", "layer_ad_2", "layer_ad_3", "layer_cursor"
};

extern OPTM_GFX_LAYER_S *g_pstGfxDevice[HIFB_LAYER_ID_BUTT];

/* ****************************** API declaration ************************************************* */
#ifdef CONFIG_HIFB_CAPTURE_BMP_SUPPORT
extern hi_void HI_UNF_HIFB_CaptureImgFromLayer(hi_u32 u32LayerID, hi_u32 LineLength, hi_bool CaptureData);
#endif

extern hi_void DRV_HIFB_SNAPSHOT_CaptureRegister(hi_void);

static hi_void HIFB_PROC_ReadHdMsg(HIFB_PAR_S *pstPar, struct seq_file *p, struct fb_info *info);
static hi_void HIFB_PROC_ReadRefreshMsg(HIFB_PAR_S *pstPar, struct seq_file *p);
static hi_void HIFB_PROC_ReadUpCostTimesMsg(HIFB_PAR_S *pstPar, struct seq_file *p);
static hi_void HIFB_PROC_ReadFenceDebugMsg(HIFB_PAR_S *pstPar, struct seq_file *p);
static hi_void HIFB_PROC_ReadWbcMsg(struct seq_file *p);
static hi_void HIFB_PROC_ReadLogMsg(struct seq_file *p);

static inline hi_void HIFB_PROC_OpenLayer(HIFB_PAR_S *pstPar);
static inline hi_void HIFB_PROC_CloseLayer(HIFB_PAR_S *pstPar);

static hi_void drv_proc_help(hi_void)
{
    osal_printk("------------------HIFB debug option----------------\n");
    osal_printk("echo show          > /proc/msp/hifb0    | show layer\n");
    osal_printk("echo hide          > /proc/msp/hifb0    | hide layer\n");
    osal_printk("echo capture (raw) > /proc/msp/hifb0    | capture image from fb\n");
    osal_printk("echo debug on      > /proc/msp/hifb0    | debug on\n");
    osal_printk("echo debug off     > /proc/msp/hifb0    | debug off\n");
}

static int drv_proc_cmd_help(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    drv_proc_help();
    return HI_SUCCESS;
}

static int drv_proc_cmd_show(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    HIFB_PAR_S *par = NULL;
    hi_s32 cnt;
    struct fb_info *info = NULL;

    info = (struct fb_info *)private;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    par = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(par, HI_FAILURE);

    cnt = osal_atomic_read(&par->stBaseInfo.ref_count);
    if ((cnt == 0) && (par->stBaseInfo.u32LayerID <= HIFB_LAYER_HD_3)) {
        return HI_FAILURE;
    }

    if (par->stProcInfo.bCreateProc == HI_FALSE) {
        return HI_FAILURE;
    }
    HIFB_PROC_OpenLayer(par);

    return HI_SUCCESS;
}

static int drv_proc_cmd_hide(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    HIFB_PAR_S *par = NULL;
    hi_s32 cnt;
    struct fb_info *info = NULL;

    info = (struct fb_info *)private;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    par = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(par, HI_FAILURE);

    cnt = osal_atomic_read(&par->stBaseInfo.ref_count);
    if ((cnt == 0) && (par->stBaseInfo.u32LayerID <= HIFB_LAYER_HD_3)) {
        return HI_FAILURE;
    }

    if (par->stProcInfo.bCreateProc == HI_FALSE) {
        return HI_FAILURE;
    }
    HIFB_PROC_CloseLayer(par);

    return HI_SUCCESS;
}

static int drv_proc_cmd_debug(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    HIFB_PAR_S *par = NULL;
    struct fb_info *info = NULL;

    info = (struct fb_info *)private;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    par = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(par, HI_FAILURE);

    if ((argc >= 2) && osal_strncmp("on", strlen("on"), argv[1], strlen("on")) == 0) { /* 2, 1, input parameter */
        par->stProcInfo.bTrace = HI_TRUE;
    } else if ((argc >= 2) && osal_strncmp("off", strlen("off"), argv[1], strlen("off")) == 0) { /* 2, 1, input
                                                                                                    parameter */
        par->stProcInfo.bTrace = HI_FALSE;
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static int drv_proc_cmd_capture(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    HIFB_PAR_S *par = NULL;
    hi_s32 cnt;
    struct fb_info *info = (struct fb_info *)private;

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    par = (HIFB_PAR_S *)info->par;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(par, HI_FAILURE);

    cnt = osal_atomic_read(&par->stBaseInfo.ref_count);
    if ((cnt == 0) && (par->stBaseInfo.u32LayerID <= HIFB_LAYER_HD_3)) {
        return HI_FAILURE;
    }

    if (par->stProcInfo.bCreateProc == HI_FALSE) {
        return HI_FAILURE;
    }

    if ((argc >= 2) && osal_strncmp("raw", strlen("raw"), argv[1], strlen("raw")) == 0) { /* 2, 1, input parameter */
        HI_UNF_HIFB_CaptureImgFromLayer(par->stBaseInfo.u32LayerID, info->fix.line_length, HI_TRUE);
    } else {
        HI_UNF_HIFB_CaptureImgFromLayer(par->stBaseInfo.u32LayerID, info->fix.line_length, HI_FALSE);
    }

    return HI_SUCCESS;
}

hi_s32 hifb_read_proc(hi_void *p, hi_void *v)
{
    struct fb_info *info = HI_NULL;
    HIFB_PAR_S *par = NULL;
    hi_s32 ret;

    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(p, HI_FAILURE);
    info = (struct fb_info *)(v);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);
    par = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(par, HI_FAILURE);

    HIFB_PROC_ReadHdMsg(par, p, info);
    HIFB_PROC_ReadRefreshMsg(par, p);

    if (HI_TRUE == par->stProcInfo.bTrace) {
        HIFB_PROC_ReadUpCostTimesMsg(par, p);
        HIFB_PROC_ReadFenceDebugMsg(par, p);
        HIFB_PROC_ReadWbcMsg(p);
        HIFB_PROC_ReadLogMsg(p);
    }
    return ret;
}

static osal_proc_cmd g_fb_proc_cmd[] = {
    { "help", drv_proc_cmd_help },       { "show", drv_proc_cmd_show },
    { "hide", drv_proc_cmd_hide },       { "debug", drv_proc_cmd_debug }, /* debug on/off */
    { "capture", drv_proc_cmd_capture }, { "capture", drv_proc_cmd_capture },
};

hi_s32 DRV_HIFB_PROC_Create(hi_u32 LayerID)
{
    hi_char entry_name[256] = {'\0'};
    struct fb_info *info = NULL;
    HIFB_PAR_S *par = NULL;
    osal_proc_entry *fb_proc_entry = NULL;

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_VALUE(LayerID, HI_FAILURE);
    info = s_stLayer[LayerID].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(info, HI_FAILURE);

    par = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_VALUE(par, HI_FAILURE);
    if (HI_TRUE == par->stProcInfo.bCreateProc) {
        return HI_SUCCESS;
    }

    snprintf(entry_name, sizeof(entry_name), "hifb%d", LayerID);
    entry_name[sizeof(entry_name) - 1] = '\0';

    fb_proc_entry = osal_proc_add(entry_name, strlen(entry_name));
    if (fb_proc_entry == HI_NULL) {
        return HI_FAILURE;
    }
    fb_proc_entry->read = hifb_read_proc;
    fb_proc_entry->cmd_cnt = sizeof(g_fb_proc_cmd) / sizeof(osal_proc_cmd);
    fb_proc_entry->cmd_list = g_fb_proc_cmd;
    fb_proc_entry->private = (hi_void *)info;

    par->stProcInfo.bCreateProc = HI_TRUE;

    return HI_SUCCESS;
}

hi_void DRV_HIFB_PROC_Destory(hi_u32 LayerID)
{
    HIFB_PAR_S *par = NULL;
    struct fb_info *info = NULL;
    hi_char entry_name[256] = {'\0'};

    GRAPHIC_CHECK_LAYERID_SUPPORT_RETURN_NOVALUE(LayerID);
    info = s_stLayer[LayerID].pstInfo;
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(info);

    par = (HIFB_PAR_S *)(info->par);
    GRAPHIC_CHECK_NULLPOINTER_RETURN_NOVALUE(par);
    if (HI_FALSE == par->stProcInfo.bCreateProc) {
        return;
    }

    snprintf(entry_name, sizeof(entry_name), "hifb%d", LayerID);
    entry_name[sizeof(entry_name) - 1] = '\0';

    osal_proc_remove(entry_name, strlen(entry_name));

    par->stProcInfo.bCreateProc = HI_FALSE;

    return;
}

static inline hi_void HIFB_PROC_CloseLayer(HIFB_PAR_S *pstPar)
{
    if ((NULL != pstPar) && (HI_TRUE == pstPar->stExtendInfo.bShow)) {
        pstPar->stRunInfo.bModifying = HI_TRUE;
        pstPar->stExtendInfo.bShow = HI_FALSE;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_SHOW;
        pstPar->stRunInfo.bModifying = HI_FALSE;
    }

    return;
}

static inline hi_void HIFB_PROC_OpenLayer(HIFB_PAR_S *pstPar)
{
    if ((NULL != pstPar) && (HI_FALSE == pstPar->stExtendInfo.bShow)) {
        pstPar->stRunInfo.bModifying = HI_TRUE;
        pstPar->stExtendInfo.bShow = HI_TRUE;
        pstPar->stRunInfo.u32ParamModifyMask |= HIFB_LAYER_PARAMODIFY_SHOW;
        pstPar->stRunInfo.bModifying = HI_FALSE;
    }

    return;
}

static hi_void HIFB_PROC_ReadHdMsg(HIFB_PAR_S *pstPar, struct seq_file *p, struct fb_info *info)
{
    hi_u32 BufNum = 0;
    hi_u32 DispBufStride = 0;
    hi_u32 DispBufSize = 0;
    hi_u32 CmpStride = 0;
    hi_bool bHDR = HI_FALSE;
    hi_bool bPreMult = HI_FALSE;
    hi_bool bDePreMult = HI_FALSE;
    hi_bool bMask = HI_FALSE;
    HIFB_RECT stOutputRect = {0};
    HIFB_RECT stDispRect = {0};

    if (pstPar->stBaseInfo.u32LayerID >= sizeof(gs_LayerName) / sizeof(hi_char *)) {
        return;
    }
    if (pstPar->stExtendInfo.enColFmt >= sizeof(s_pszFmtName) / sizeof(hi_char *)) {
        return;
    }

    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetOutRect(pstPar->stBaseInfo.u32LayerID, &stOutputRect);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetDispFMTSize(pstPar->stBaseInfo.u32LayerID, &stDispRect);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetLayerPreMult(pstPar->stBaseInfo.u32LayerID, &bPreMult, &bDePreMult);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetLayerHdr(pstPar->stBaseInfo.u32LayerID, &bHDR);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetGPMask(pstPar->stBaseInfo.u32LayerID, &bMask);

    DispBufStride = CONIFG_HIFB_GetMaxStride(pstPar->stExtendInfo.DisplayWidth, info->var.bits_per_pixel, &CmpStride,
                                             CONFIG_HIFB_STRIDE_ALIGN);
    DispBufSize = HI_HIFB_GetMemSize(DispBufStride, pstPar->stExtendInfo.DisplayHeight);

    osal_proc_print(p, "************************** hd layer information *****************************\n");
    osal_proc_print(p, "LayerId                      \t : %s\n", gs_LayerName[pstPar->stBaseInfo.u32LayerID]);
    osal_proc_print(p, "ShowState                    \t : %s\n", pstPar->stExtendInfo.bShow ? "ON" : "OFF");
    osal_proc_print(p, "layer mask                   \t : %s\n", (bMask == HI_TRUE) ? ("true") : ("false"));
    osal_proc_print(p, "compress mode                \t : %d\n", pstPar->compress_mode);
    osal_proc_print(p, "IsStereo                     \t : %s\n", pstPar->st3DInfo.IsStereo ? "yes" : "no");
    osal_proc_print(p, "HDR                          \t : %s\n", bHDR ? "yes" : "no");
    osal_proc_print(p, "Enable Premult               \t : %s\n", bPreMult ? "enable" : "disable");
    osal_proc_print(p, "Enable DePremult             \t : %s\n", bDePreMult ? "enable" : "disable");

    osal_proc_print(p, "[xres,yres]                  \t : (%d, %d)\n", info->var.xres, info->var.yres);
    osal_proc_print(p, "(xoffset,yoffset)            \t : (%d, %d)\n", info->var.xoffset, info->var.yoffset);
    osal_proc_print(p, "[xres_virtual,yres_virtual]  \t : (%d, %d)\n", info->var.xres_virtual, info->var.yres_virtual);
    osal_proc_print(p, "GP0 Output                   \t : (%d, %d,  %d, %d)\n", stDispRect.x, stDispRect.y,
                    stDispRect.w, stDispRect.h);
    osal_proc_print(p, "Input ColorFormat            \t : %s\n", s_pszFmtName[pstPar->stExtendInfo.enColFmt]);
    osal_proc_print(p, "Cur HD Disp Addr             \t : 0x%x\n", pstPar->stRunInfo.CurScreenAddr);
    osal_proc_print(p, "fence and reality stride     \t : [%d, %d]\n", pstPar->stProcInfo.HwcRefreshInDispStride,
                    info->fix.line_length);
    osal_proc_print(p, "One HD Buffer Size           \t : %d\n", DispBufSize);
    osal_proc_print(p, "Total HD Buffer MemSize      \t : %d KB\n", info->fix.smem_len / 1024);

    for (BufNum = 0; BufNum < CONFIG_HIFB_LAYER_BUFFER_MAX_NUM; BufNum++) {
        osal_proc_print(p, "HD %d BUFFER ADDR            \t : [0X%lX,0X%lX]\n", BufNum,
                        (unsigned long)info->fix.smem_start + BufNum * DispBufSize,
                        (unsigned long)pstPar->u32SmemStartPhy + BufNum * DispBufSize);
    }

    osal_proc_print(p, "**************************************************************************\n");
    return;
}

static hi_void HIFB_PROC_ReadRefreshMsg(HIFB_PAR_S *pstPar, struct seq_file *p)
{
    osal_proc_print(p, "************************** refresh information ***************************\n");

    osal_proc_print(p, "sync type                  \t : %s\n", pstPar->hifb_sync ? "fence" : "vblank");

    osal_proc_print(p, "refresh rate                  \t : %d\n", pstPar->stFrameInfo.FlipFps);

    osal_proc_print(p, "frame rate                   \t : %d\n", pstPar->stFrameInfo.DrawFps);

    if (HI_TRUE == pstPar->bDiscardFrame) {
        osal_proc_print(p, "whether discard frame      \t : yes\n");
    } else {
        osal_proc_print(p, "whether discard frame      \t : %s\n",
                        (pstPar->bProcDiscardFrame == HI_TRUE) ? "yes" : "no");
    }

    osal_proc_print(p, "repeat refresh frame times \t : %ld\n", pstPar->stFrameInfo.RepeatFrameCnt);

    if (HI_TRUE == pstPar->hifb_sync) {
        osal_proc_print(p, "refresh mode               \t : %s\n", "fb-hwc-refresh");
        goto FINISH_EXIT;
    }

    switch (pstPar->stExtendInfo.enBufMode) {
        case HIFB_LAYER_BUF_DOUBLE:
            osal_proc_print(p, "refresh mode               \t : %s\n", "(fb-double)-(higo-over-or-butt)");
            break;
        case HIFB_LAYER_BUF_ONE:
            osal_proc_print(p, "refresh mode               \t : %s\n", "(fb-one)-(higo-double)");
            break;
        case HIFB_LAYER_BUF_NONE:
            osal_proc_print(p, "refresh mode               \t : %s\n", "(fb-zero)-(higo-normal)");
            break;
        case HIFB_LAYER_BUF_DOUBLE_IMMEDIATE:
            osal_proc_print(p, "refresh mode               \t : %s\n", "(fb-doulbe-immediate)(sync)-(higo-flip)");
            break;
        case HIFB_LAYER_BUF_STANDARD:
            osal_proc_print(p, "refresh mode               \t : %s\n", "pandisplay");
            break;
        case HIFB_LAYER_BUF_FENCE:
            osal_proc_print(p, "refresh mode               \t : %s\n", "(fb-double)-(higo-fence)");
            break;
        default:
            break;
    }

FINISH_EXIT:
    osal_proc_print(p, "**************************************************************************\n");
    return;
}

static hi_void HIFB_PROC_ReadUpCostTimesMsg(HIFB_PAR_S *pstPar, struct seq_file *p)
{
    osal_proc_print(p, "************************** refresh cost times ****************************\n");

    /* * 刷新函数执行的时间 * */
    osal_proc_print(p, "run refresh func cost times            \t : %dms\n", pstPar->stFrameInfo.RunRefreshTimeMs);

    /* * 刷新函数前后进入的时间差 * */
    osal_proc_print(p, "into refresh func cur - pre cost times \t : %dms\n", pstPar->stFrameInfo.TwiceRefreshTimeMs);

    if (HI_TRUE == pstPar->stFrameInfo.bBlitBlock) {
        /* * 硬件搬移是否阻塞 * */
        osal_proc_print(p, "hard blit whether block                \t : yes\n");
        /* * 阻塞时间 * */
        osal_proc_print(p, "hard blit block times                  \t : %dms\n", pstPar->stFrameInfo.BlockTime);
    } else {
        /* * 硬件搬移是否阻塞 * */
        osal_proc_print(p, "hard blit whether block                \t : no\n");
    }

    osal_proc_print(p, "**************************************************************************\n");
    return;
}

static hi_void HIFB_PROC_ReadFenceDebugMsg(HIFB_PAR_S *pstPar, struct seq_file *p)
{
#ifdef CONFIG_HIFB_FENCE_SUPPORT
    drv_hifb_fence sync_info = {{0}};

    drv_hifb_fence_get_sync_info(&sync_info);
#endif
    osal_proc_print(p, "************************** fence information *****************************\n");

    osal_proc_print(p, "vo callback expect interrupt lines     \t : %ld\n",
                    pstPar->stFrameInfo.ExpectIntLineNumsForVoCallBack);

    osal_proc_print(p, "vo callback actual interrupt lines     \t : %ld\n",
                    pstPar->stFrameInfo.ActualIntLineNumsForVoCallBack);

    osal_proc_print(p, "vo callback run max cost times         \t : %dms\n", pstPar->stFrameInfo.RunMaxTimeMs);

    osal_proc_print(p, "vo callback cur - pre max cost times   \t : %dms\n", pstPar->stFrameInfo.MaxTimeMs);

    osal_proc_print(p, "vo callback hard interupt times        \t : %ld\n",
                    pstPar->stFrameInfo.HardIntCntForVoCallBack);

    osal_proc_print(p, "run vo callback func times             \t : %ld\n", pstPar->stFrameInfo.VoSoftCallBackCnt);

#ifdef CONFIG_HIFB_FENCE_SUPPORT
    osal_proc_print(p, "frame callback expect interrupt lines  \t : %ld\n",
                    pstPar->stFrameInfo.ExpectIntLineNumsForEndCallBack);

    osal_proc_print(p, "frame callback actual interrupt lines  \t : %ld\n", sync_info.actual_int_line_num_end_callback);

    osal_proc_print(p, "frame callback run max cost times      \t : %dms\n", sync_info.run_max_time_ms);

    osal_proc_print(p, "frame callback cur - pre max cost times\t : %dms\n", sync_info.max_time_ms);

    osal_proc_print(p, "frame callback hard interupt times     \t : %ld\n", sync_info.hard_int_cnt_end_callback);

    osal_proc_print(p, "run frame callback func times          \t : %ld\n", sync_info.end_soft_callback_cnt);

    osal_proc_print(p, "refresh times                          \t : %d\n", pstPar->FenceRefreshCount);

    osal_proc_print(p, "updata regiter times                   \t : %d\n", sync_info.reg_update_cnt);

    osal_proc_print(p, "ar data decompress err times           \t : %ld\n", pstPar->stFrameInfo.ARDataDecompressErrCnt);

    osal_proc_print(p, "gb data decompress err times           \t : %ld\n", pstPar->stFrameInfo.GBDataDecompressErrCnt);

    osal_proc_print(p, "fence[release_fd][fence_value][time_line]\t : [%d][%d][%d]\n", sync_info.release_fence_fd,
                    sync_info.fence_value, sync_info.time_line);
#endif

    osal_proc_print(p, "**************************************************************************\n");

    return;
}

#ifndef CONFIG_HIFB_WBC_UNSUPPORT
extern hi_void DRV_HIFB_WBC_GetWriteBuffer(hi_u32 *pAddr, hi_u32 *pStride);
extern DRV_HIFB_WBC_GFX_S g_stGfxWbc2;
#endif
static hi_void HIFB_PROC_ReadWbcMsg(struct seq_file *p)
{
#ifndef CONFIG_HIFB_WBC_UNSUPPORT
    hi_bool is_mask = HI_FALSE;
    hi_bool layer_enable = HI_FALSE;
    hi_u32 read_mode = 0;
    hi_u32 wbc_work_enable = 0;
    hi_u32 wbc_work_state = 0;
    hi_u32 wbc_read_addr = 0;
    hi_u32 wbc_write_addr = 0;
    hi_u32 wbc_write_stride = 0;
    HIFB_RECT rect = {0};

    DRV_HIFB_WBC_GetWriteBuffer(&wbc_write_addr, &wbc_write_stride);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetLayerAddr(HIFB_LAYER_SD_0, &wbc_read_addr);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetEnable(HIFB_LAYER_SD_0, &layer_enable);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetLayerRect(HIFB_LAYER_SD_0, &rect);
    g_stDrvAdpCallBackFunction.DRV_HIFB_ADP_GetGPMask(HIFB_LAYER_SD_0, &is_mask);
    drv_hifb_hal_gfx_get_read_mode(g_pstGfxDevice[HIFB_LAYER_SD_0]->enGfxHalId, &read_mode);
    DRV_HIFB_HAL_WBC_GP_GetWorkEnable(g_stGfxWbc2.enWbcHalId, &wbc_work_enable, &wbc_work_state);

    osal_proc_print(p, "*************************** wbc msg **************************************\n");
    osal_proc_print(p, "layer enable        \t : %s\n", (layer_enable == HI_TRUE) ? ("true") : ("false"));
    osal_proc_print(p, "wbc work enable     \t : %s\n", (wbc_work_enable == 1) ? ("true") : ("false"));
    osal_proc_print(p, "wbc work state      \t : %s\n", (wbc_work_state == 0) ? ("finish") : ("busy"));
    osal_proc_print(p, "layer mask          \t : %s\n", (is_mask == HI_TRUE) ? ("true") : ("false"));
    osal_proc_print(p, "wbc buf num         \t : %d\n", g_stGfxWbc2.WbcBufNum);
    osal_proc_print(p, "wbc buf size        \t : %d\n", g_stGfxWbc2.wbc_buf_mgr.buf_size);
    osal_proc_print(p, "layer read buffer   \t : 0X%X\n", wbc_read_addr);
    osal_proc_print(p, "wbc_gp write buffer \t : 0X%X\n", wbc_write_addr);
    osal_proc_print(p, "wbc_gp write stride \t : %d\n", wbc_write_stride);
    osal_proc_print(p, "layer resolution    \t : [%d,%d][%d,%d]\n", rect.x, rect.y, rect.w, rect.h);
    osal_proc_print(p, "layer data fmt is   \t : AYCbCr8888\n");
    osal_proc_print(p, "layer read mod      \t : %d\n", read_mode);
    switch (g_stGfxWbc2.wbc_buf_mgr.cur_buf_mode) {
        case HIFB_WBC_BUFFER_MODE_FIELD:
            osal_proc_print(p, "wbc buf mod         \t : %s\n", "field");
            break;
        case HIFB_WBC_BUFFER_MODE_SINGLE_FRAME:
            osal_proc_print(p, "wbc buf mod         \t : %s\n", "one frame");
            break;
        default:
            osal_proc_print(p, "wbc buf mod         \t : %s\n", "doule frame");
            break;
    }
#else
#endif
    return;
}

static hi_void HIFB_PROC_ReadLogMsg(struct seq_file *p)
{
    osal_proc_print(p, "*************************** debug msg ************************************\n");
    HI_GFX_ProcMsg(p);
    osal_proc_print(p, "**************************************************************************\n");
    return;
}
#endif
