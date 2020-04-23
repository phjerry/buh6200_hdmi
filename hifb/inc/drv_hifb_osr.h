/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: hifb osr header
 * Author: sdk
 * Create: 2016-01-01
 */

#ifndef __DRV_HIFB_OSR_H__
#define __DRV_HIFB_OSR_H__

/* ********************************add include here********************************************** */
#include "hi_type.h"
#include <linux/fb.h>
#include "drv_hifb_common.h"
#include "drv_hifb_scrolltext.h"
#include "drv_hifb_config.h"
#include "drv_hifb_blit.h"
#include "drv_hifb_proc.h"
#include "drv_hifb_adp.h"
#ifdef CONFIG_HIFB_FENCE_SUPPORT
#include <linux/dma-fence.h>
#endif
#include "hi_osal.h"

/************************************************************************************************/

/************************************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* **************************** Macro Definition ************************************************ */
#define HIFB_DEFLICKER_LEVEL_MAX 5
#define HIFB_MAX_LAYER_NUM HIFB_LAYER_ID_BUTT
#define HIFB_MAX_PIXFMT_NUM 28
#define FBIOGET_FSCREENINFO_HIFB 0x4693
#define HIFB_REFRESH_MODE_WITH_PANDISPLAY HIFB_LAYER_BUF_STANDARD
#define CONFIG_HIFB_STEREO_WAITE_TIME ((2000 * HZ) / 1000)

/* ************************** Structure Definition ********************************************** */

typedef enum {
    HIFB_ANTIFLICKER_NONE, /* no antiflicker.If scan mode is progressive, hifb will set antiflicker mode to none */
    HIFB_ANTIFLICKER_TDE,  /* tde antiflicker mode, it's effect for 1buf or 2buf only */
    HIFB_ANTIFLICKER_VO,   /* vo antiflicker mode, need hardware supprot */
    HIFB_ANTIFLICKER_BUTT
} HIFB_LAYER_ANTIFLICKER_MODE_E;

typedef enum {
    HIFB_LAYER_TYPE_HD,
    HIFB_LAYER_TYPE_SD,
    HIFB_LAYER_TYPE_AD,
    HIFB_LAYER_TYPE_CURSOR,
    HIFB_LAYER_TYPE_BUTT,
} HIFB_LAYER_TYPE_E;

typedef struct {
    hi_bool bBlitBlock; /* * 是否阻塞                                      * */
    hi_u32 BlockTime;   /* * 阻塞时间                                      * */
    hi_u32 StartTimeMs;
    hi_u32 RefreshFrame;                      /* * 保存调用刷新的次数，帧率会根据这个来计算      * */
    hi_u32 flip_fps_cnt;                      /* * 显示帧率                                      * */
    hi_u32 FlipFps;                           /* * 显示帧率                                      * */
    hi_u32 DrawFps;                           /* * 调用刷新的绘制帧率                            * */
    hi_u32 PreRefreshTimeMs;                  /* * 上一次进入刷新接口的时间                      * */
    hi_u32 RunRefreshTimeMs;                  /* * 刷新函数执行的时间                            * */
    hi_u32 TwiceRefreshTimeMs;                /* * 上一次进入刷新接口和当前进入刷新接口的时间差  * */
    hi_u32 PreTimeMs;                         /* * 上一次进入VO中断的时间                        * */
    hi_u32 MaxTimeMs;                         /* * 上一次进入VO中断和当前进入VO中断的时间差      * */
    hi_u32 RunMaxTimeMs;                      /* * VO中断函数执行的最长时间                      * */
    hi_ulong RepeatFrameCnt;                  /* * 重复帧次数                                    * */
    hi_ulong VoSoftCallBackCnt;               /* * VO中断函数执行的次数                          * */
    hi_ulong ARDataDecompressErrCnt;          /* * 解压错误次数                                  * */
    hi_ulong GBDataDecompressErrCnt;          /* * 解压错误次数                                  * */
    hi_ulong ExpectIntLineNumsForVoCallBack;  /* * 90%期望上报的行数                             * */
    hi_ulong ExpectIntLineNumsForEndCallBack; /* * FrameEnde期望上报的行数                       * */
    hi_ulong ActualIntLineNumsForVoCallBack;  /* * 90%实际上报的行数                             * */
    hi_ulong HardIntCntForVoCallBack;         /* * 90%中断函数硬件的中断个数                     * */
} HIFB_FRAME_INFO_S;

typedef struct {
    hi_u32 u32StereoMemStart;
    hi_u32 u32StereoMemLen;
} HIFB_3DMEM_INFO_S;

typedef struct {
    hi_bool BegFreeStereoMem;
    hi_bool BegUpStereoInfo;
    hi_bool IsStereo;
    hi_s32 s32StereoDepth;
    hi_u32 u32rightEyeAddr;
    hi_u32 u32DisplayAddr[CONFIG_HIFB_LAYER_BUFFER_MAX_NUM];
    osal_spinlock StereoLock;
    osal_wait WaiteFinishUpStereoInfoMutex;
    osal_wait WaiteFinishFreeStereoMemMutex;
    HIFB_STEREO_MODE_E StereoMode;
    HIFB_RECT st3DUpdateRect;
    HIFB_SURFACE_S st3DSurface;
    drv_hifb_mem_info mem_info_3d;
} HIFB_3D_PAR_S;

typedef struct {
    HIFB_RECT stCmpRect;
    HIFB_RECT stUpdateRect;
    hi_u32 u32DisplayAddr[CONFIG_HIFB_LAYER_BUFFER_MAX_NUM];
    HIFB_SURFACE_S stCanvasSur;
    HIFB_BUFFER_S stUserBuffer;
} HIFB_DISP_INFO_S;

typedef struct {
    hi_bool bModifying;
    hi_u32 u32ParamModifyMask;
    hi_bool bNeedFlip;
    hi_bool bFliped;
    hi_u32 u32IndexForInt;
    hi_u32 u32BufNum;
    hi_u32 StereoBufNum;
    hi_u32 PreRefreshAddr; /* *上次送显的地址，用来判断是否有送重复帧的情况* */
    hi_u32 CurScreenAddr;
    hi_u32 LastScreenAddr;
    hi_s32 s32RefreshHandle;
} HIFB_RTIME_INFO_S;

typedef struct {
    hi_bool bShow;        /* show status */
    hi_u32 DisplayWidth;  /* width  of layer's display buffer */
    hi_u32 DisplayHeight; /* height of layer's display buffer */
    hi_u32 DisplayStride;
    DRV_HIFB_COLOR_FMT_E enColFmt; /* color format */
    HIFB_LAYER_BUF_E enBufMode;    /* refresh mode */
    HIFB_POINT_S stPos;            /* beginning position of layer */
    HIFB_ALPHA_S stAlpha;          /* alpha attribution */
    HIFB_COLORKEYEX_S stCkey;      /* colorkey attribution */
    hifb_hwc_layer_private_data private_data;
} HIFB_EXTEND_INFO_S;

typedef struct {
    hi_u32 u32LayerID;     /* layer id */
    osal_atomic ref_count; /* framebuffer reference count */
    osal_spinlock lock;    /* using in 2buf refresh */
    hi_bool bPreMul;
    hi_bool bNeedAntiflicker;
    hi_u32 u32HDflevel;                                /* horizontal deflicker level */
    hi_u32 u32VDflevel;                                /* vertical deflicker level */
    hi_uchar ucHDfcoef[HIFB_DEFLICKER_LEVEL_MAX - 1];  /* horizontal deflicker coefficients */
    hi_uchar ucVDfcoef[HIFB_DEFLICKER_LEVEL_MAX - 1];  /* vertical deflicker coefficients */
    HIFB_LAYER_ANTIFLICKER_LEVEL_E enAntiflickerLevel; /* antiflicker level */
    HIFB_LAYER_ANTIFLICKER_MODE_E enAntiflickerMode;   /* antiflicker mode */
} HIFB_BASE_INFO_S;

typedef struct {
    hi_bool start_release;
    drv_hifb_mem_info mem_info;
    osal_workqueue dis_buff_work;
} drv_hifb_disp_tmp_buf;

#ifdef CONFIG_HIFB_FENCE_SUPPORT
/* **************************** Macro Definition ************************************************* */
#define DRV_HIFB_FENCE_NAME "hifb_fence"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
#define DRV_HIFB_FENCE_TIMELINE_NUM 32
#define DRV_HIFB_FENCE_ROTATE_NUM 4
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
#define hifb_fence dma_fence
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
#define hifb_fence fence
#elif LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
#define hifb_fence sync_fence
#endif

/* ************************** Structure Definition *********************************************** */
typedef struct {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
    hi_char timeline_name[DRV_HIFB_FENCE_TIMELINE_NUM];
    hi_s32 create_fence_index;
    hi_s32 destroy_fence_index;
    hi_u32 fence_seqno;
    hi_u64 fence_context;
    spinlock_t fence_lock;
    struct hifb_fence *fence_array[DRV_HIFB_FENCE_ROTATE_NUM];
#endif
    hi_u8 reg_update_cnt;
    hi_u32 fence_value;
    hi_u32 time_line;
    hi_s32 release_fence_fd;
    hi_u32 frame_end_flag;
    hi_u32 pre_time_ms; /* pre time in frame end */               /* 上一次进入FrameEnd中断的时间 */
    hi_u32 max_time_ms; /* the max time of current - pre */       /* 上一次进入FrameEnd中断和当前进入VO中断的时间差 */
    hi_u32 run_max_time_ms; /* the max time of frame end run */   /* FrameEnd中断函数执行的最长时间 */
    hi_ulong end_soft_callback_cnt; /* frame end running times */ /* FrameEnd中断函数执行的次数 */
    hi_ulong actual_int_line_num_end_callback; /* actual int line num of frame end */ /* FrameEnd实际上报的行数 */
    hi_ulong hard_int_cnt_end_callback; /* hard int cnt of frame end */ /* FrameEnd中断函数硬件的中断个数 */
    osal_spinlock lock;
    osal_wait frame_end_event;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0)
    struct sw_sync_timeline *time_line;
#endif
} drv_hifb_fence;
#endif /* * CONFIG_HIFB_FENCE_SUPPORT * */

typedef struct {
    hi_bool up_mute;
    hi_bool is_first_pandisplay;
    hi_bool bFrameBufferRegister;
    hi_bool bPanFlag;
    hi_bool bVblank;
    hi_bool hifb_sync;
    drv_hifb_compress_mode compress_mode;
    hi_bool bProcDiscardFrame;
    hi_bool bDiscardFrame;
    hi_bool bEndFenceRefresh;
    hi_u8 FenceRefreshCount;
    hi_u32 DiscardFrameTimes;
    hi_u32 u32SmemStartPhy;
    drv_hifb_mem_info mem_info;
    HIFB_BASE_INFO_S stBaseInfo;
    HIFB_EXTEND_INFO_S stExtendInfo;

    HIFB_3D_PAR_S st3DInfo;
    HIFB_DISP_INFO_S stDispInfo;

    HIFB_RTIME_INFO_S stRunInfo;
    HIFB_FRAME_INFO_S stFrameInfo;
#ifdef CONFIG_GFX_PROC_SUPPORT
    DRV_HIFB_PROC_S stProcInfo;
#endif
    drv_hifb_disp_tmp_buf disp_tmp_buf;

#ifdef CONFIG_HIFB_LOWPOWER_SUPPORT
    HIFB_ADP_LOWPOWER_INFO_S stLowPowerInfo;
#endif

#ifdef CONFIG_HIFB_FENCE_SUPPORT
    struct workqueue_struct *pFenceRefreshWorkqueue;
    osal_wait WaiteEndFenceRefresh;
    drv_hifb_fence fence_sync_info;
#endif
    HIFB_HWC_LAYERINFO_S hwc_layer_info[CONFIG_HIFB_LAYER_BUFFER_MAX_NUM];
    hi_u8 sync_times[CONFIG_HIFB_LAYER_BUFFER_MAX_NUM];
    hi_u8 hwc_refresh_cnt;
    hi_u8 windex;
    hi_u8 rindex;
} HIFB_PAR_S;

typedef struct {
    struct fb_info *pstInfo;
    hi_u32 u32LayerSize;
} HIFB_LAYER_S;

typedef struct {
    struct fb_bitfield stRed;   /* bitfield in fb mem if true color, */
    struct fb_bitfield stGreen; /* else only length is significant */
    struct fb_bitfield stBlue;
    struct fb_bitfield stTransp; /* transparency    */
} HIFB_ARGB_BITINFO_S;

#ifdef CONFIG_COMPAT
struct fb_fix_compat_screeninfo {
    hi_char id[16];
    hi_u32 smem_start;
    hi_u32 smem_len;
    hi_u32 type;
    hi_u32 type_aux;
    hi_u32 visual;
    hi_u16 xpanstep;
    hi_u16 ypanstep;
    hi_u16 ywrapstep;
    hi_u32 line_length;
    hi_u32 mmio_start;
    hi_u32 mmio_len;
    hi_u32 accel;
    hi_u16 capabilities;
    hi_u16 reserved[2];
};
#endif

/* ********************* Global Variable declaration ******************************************** */

extern HIFB_DRV_OPS_S g_stDrvAdpCallBackFunction;
extern HIFB_DRV_TDEOPS_S g_stGfx2dCallBackFunction;
extern HIFB_LAYER_S s_stLayer[HIFB_MAX_LAYER_NUM];

extern HIFB_ARGB_BITINFO_S s_stArgbBitField[HIFB_MAX_PIXFMT_NUM];

#ifdef CONFIG_HIFB_SCROLLTEXT_SUPPORT
extern HIFB_SCROLLTEXT_INFO_S s_stTextLayer[HIFB_LAYER_ID_BUTT];
#endif

#ifdef CONFIG_HIFB_FENCE_SUPPORT
hi_s32 drv_hifb_fence_init(HIFB_PAR_S *par);
hi_void drv_hifb_fence_deinit(HIFB_PAR_S *par);
hi_s32 drv_hifb_fence_create(hi_u32 layer_id);
hi_s32 drv_hifb_fence_wait(struct hifb_fence *fence, long timeout);
hi_void drv_hifb_fence_inc_refresh_time(hi_bool layer_enable, hi_u32 layer_id);
hi_s32 drv_hifb_fence_get_fd(struct hifb_fence *fence);
hi_s32 drv_hifb_fence_destory(struct hifb_fence *fence);
hi_void drv_hifb_fence_signal(hi_u32 layer_id);
hi_void drv_hifb_fence_put(struct hifb_fence *fence); /* for sync_fence put */
struct hifb_fence *drv_hifb_fd_get_fence(hi_s32 fd);

hi_void drv_hifb_fence_wait_refresh_end(hi_bool should_wait);
hi_void drv_hifb_fence_get_sync_info(drv_hifb_fence *sync_info);
hi_void drv_hifb_fence_frame_end_callback(hi_void *para1, hi_void *para2);
hi_void drv_hifb_fence_get_fence_value(hi_u32 *fence_value, hi_u32 layer_id);
hi_void drv_hifb_fence_get_timeline_value(hi_u32 *time_line_value, hi_u32 layer_id);
#endif

/* ****************************** API declaration *********************************************** */
hi_void drv_hifb_get_layer(hi_u32 layer_id, HIFB_LAYER_S **layer);
hi_u32 DRV_HIFB_GetPixDepth(hi_u32 BitsPerPixel);
hi_void DRV_HIFB_WaitVBlank(HIFB_LAYER_ID_E enLayerId);
hi_s32 DRV_HIFB_UpStereoData(HIFB_LAYER_ID_E enLayerId, HIFB_BUFFER_S *pstSrcBuffer, HIFB_BLIT_OPT_S *pstBlitOpt);
hi_s32 DRV_HIFB_BlitFinishCallBack(hi_void *pParaml, hi_void *pParamr);
hi_void DRV_HIFB_SetDecmpLayerInfo(hi_u32 LayerId);
hi_void DRV_HIFB_Lock(osal_spinlock *pLock, hi_ulong *pLockParFlag);
hi_void DRV_HIFB_UnLock(osal_spinlock *pLock, hi_ulong *pLockParFlag);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
