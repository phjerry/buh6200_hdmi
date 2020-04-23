#ifndef __HI_DRV_MCE_H__
#define __HI_DRV_MCE_H__

#include "hi_drv_demux.h"
#include "hi_drv_audio.h"
#include "hi_drv_pdm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_FATAL_MCE(format...)    HI_FATAL_PRINT(HI_ID_FASTPLAY, format)
#define HI_ERR_MCE(format...)      HI_ERR_PRINT(HI_ID_FASTPLAY, format)
#define HI_WARN_MCE(format...)     HI_WARN_PRINT(HI_ID_FASTPLAY, format)
#define HI_INFO_MCE(format...)     HI_INFO_PRINT(HI_ID_FASTPLAY, format)

#define HI_MCE_THREAD_EXIT          (-1)
#define HI_MCE_THREAD_SLEEP         0
#define HI_MCE_THREAD_RUNNING       1
#define HI_MTD_NAME_LEN             32


typedef enum {
    MCE_IOC_STOP_MCE,
    MCE_IOC_EXIT_MCE,
    MCE_IOC_CLEAR_LOGO,
    MCE_IOC_MAX
} mce_ioc;

/* Defines the video display mode after an AVPLAY is stopped */
typedef enum {
    HI_DRV_AVPLAY_STOP_MODE_STILL = 0,  /* <The last frame is still after an AVPLAY is stopped. */
    HI_DRV_AVPLAY_STOP_MODE_BLACK = 1,  /* <The blank screen is displayed after an AVPLAY is stopped */
    HI_DRV_AVPLAY_STOP_MODE_BUTT
} hi_drv_mce_avplay_stop_mode;

/* Mode of Play Control */
typedef enum {
    HI_DRV_MCE_PLAYCTRL_BY_TIME,    /* play control by time */
    HI_DRV_MCE_PLAYCTRL_BY_COUNT,   /* play control by count */
    HI_DRV_MCE_PLAYCTRL_BUTT
} hi_drv_mce_playctrl_mode;

/* Defines the parameter of fastplay stop */
typedef struct {
    hi_drv_mce_avplay_stop_mode stop_mode;     /* <Mode of Stop,see ::HI_UNF_AVPLAY_STOP_MODE_E */
    hi_drv_mce_playctrl_mode    ctrl_mode;     /* <Mode of Play Control */
    hi_u32                      play_time_ms;    /* <Play time */
    hi_u32                      play_count;   /* <Play count */
} hi_drv_mce_stop_param;

/* Defines the parameter of fastplay exit */
typedef struct {
    hi_handle   new_win_handle;          /* <Handle of new window */
} hi_drv_mce_exit_param;

typedef struct {
    hi_handle               avplay_handle;
    hi_handle               window_handle;
    hi_handle               sound_handle;
    hi_disp_param           disp_param;
    hi_mce_param            mce_param;

    hi_bool                 play_stop;
    hi_drv_mce_stop_param   stop_param;

    hi_bool                 mce_exit;

    hi_u32                  begin_time;
    hi_u32                  end_time;

    hi_bool                 play_end;

    hi_s32                  ts_thread_status;
    hi_bool                 animation_thread_stop;

    hi_u8                   *play_data_addr;

#ifdef HI_GRADUAL_VOLUME_SUPPORT
    hi_bool                 gradual_vol_thread_stop;
    struct task_struct      *gradual_vol_task;
#endif
    struct task_struct      *play_task;

    hi_bool                 recv_video_frame;
} mce_info;


#define HI_MCE_STOP_FASTPLAY_CMD        _IOW(HI_ID_FASTPLAY, MCE_IOC_STOP_MCE, HI_UNF_MCE_STOPPARM_S)
#define HI_MCE_EXIT_FASTPLAY_CMD        _IOW(HI_ID_FASTPLAY, MCE_IOC_EXIT_MCE, HI_UNF_MCE_EXITPARAM_S)
#define HI_MCE_CLEAR_LOGO_CMD           _IO(HI_ID_FASTPLAY, MCE_IOC_CLEAR_LOGO)

hi_s32 hi_drv_mce_stop(hi_drv_mce_stop_param *stop_param);
hi_s32 hi_drv_mce_exit(hi_drv_mce_exit_param *exit_param);

#if    !defined(CHIP_TYPE_HI3716MV410) && \
       !defined(CHIP_TYPE_HI3716MV420) && \
       !defined(CHIP_TYPE_HI3798CV200) && \
       !defined(CHIP_TYPE_HI3798MV200) && \
       !defined(CHIP_TYPE_HI3798MV300) && \
       !defined(CHIP_TYPE_HI3796MV200) && \
       !defined(CHIP_TYPE_HI3716MV450)
#define MCE_ACTIVE_MAIN_AND_SLAVE_WIN_SUPPORT
#endif

#if    defined(CHIP_TYPE_HI3716MV450) || \
       defined(CHIP_TYPE_HI3796MV200) || \
       defined(CHIP_TYPE_HI3798CV200) || \
       defined(CHIP_TYPE_HI3798MV200) || \
       defined(CHIP_TYPE_HI3798MV300) || \
       defined(CHIP_TYPE_HI3798MV100) || \
       defined(CHIP_TYPE_HI3796MV100)
#define MCE_FRAME_RATE_ADJUST_SUPPORT
#endif

#ifdef __cplusplus
}
#endif

#endif
