/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: vpss_define.h hander file vpss define
 * Author: zhangjunyu
 * Create: 2016/07/01
 */
#ifndef __VPSS_DEFINE_H__
#define __VPSS_DEFINE_H__

#ifdef DPT
#include "hi_board.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VPSS0_IRQ_NUM 114
#define VPSS1_IRQ_NUM 115

#define VPSS_FILE_NAMELENGTH 255
#define VPSS_ISLOSS_CMP      HI_TRUE
#define VPSS_UHD_SUPPORT_NR  HI_FALSE

#define VPSS_HEIGHT_ALIGN 0xfffffffc
#define VPSS_WIDTH_ALIGN  0xfffffffe

#define VPSS_DETECTPORT_WIDTH  704
#define VPSS_DETECTPORT_HEIGHT 480
#define VPSS_CMP_MIN_WIDTH     960
#define VPSS_SD_WIDTH          960

#define VPSS_WIDTH_ALLOC_BUFFER  1920
#define VPSS_HEIGHT_ALLOC_BUFFER 1088
#define VPSS_WIDTH_FHD           1920
#define VPSS_HEIGHT_FHD          1080
#define VPSS_HEIGHT_FHD_1088     1088

#define VPSS_WIDTH_UHD           4096
#define VPSS_HEIGHT_UHD          2304

#define VPSS_FHD_DATA_SIZE       (1920 * 1088)
#define VPSS_INSTANCE_MAX_NUM    DEF_HI_DRV_VPSS_INSTANCE_MAX_NUM
#define VPSS_PORT_MAX_NUM        DEF_HI_DRV_VPSS_MAX_PORT_NUM
#define VPSS_TASK_MAX_NUM        2
#define VPSS_BUFFER_MAX          2
#define VPSS_PTS_NODE_NUM        3
#define VPSS_RESO_ALIGN          2
#define VPSS_RESO_ALIGN_4        4

#define VPSS_SUPPORT_RATE_MAX     (500 * 1000)
#define VPSS_DEVICE_STATE_INIT    0
#define VPSS_DEVICE_STATE_SUSPEND 3
#define VPSS_DEVICE_STATE_DEINIT  4
#ifdef HI_FPGA
#define VPSS_LOGIC_TIMEOUT_NS 15000000000
#else
#define VPSS_LOGIC_TIMEOUT_NS 200000000
#endif
#define VPSS_THREAD_TIMEOUT_NS       30000000
#define porthandle_to_vpssid(h_port) ((h_port) >> 8)
#define porthandle_to_portid(h_port) ((h_port) & 0xff)

#define VPSS_THREAD_UNSLEEP          0
#define VPSS_THREAD_SLEEP            1
#define VPSS_THREAD_POS_INIT         0
#define VPSS_THREAD_POS_WORK         1
#define VPSS_THREAD_POS_IDLE         2
#define VPSS_SUPPORT_CF_UPSAMP       (HI_FALSE)
#define VPSS_SUPPORT_NR_FOURPIX_MODE (HI_FALSE)

/* todo */
#define HI_BOARD_VPSS_WBC_CMP_RATIO 3000
#define HI_BOARD_VPSS_NR_CMP_RATIO  2000

#define VPSS_SUPPORT_CMP_RATE    (HI_BOARD_VPSS_WBC_CMP_RATIO)
#define VPSS_SUPPORT_CMP_LOSSLESS_RATE 1000

#define VPSS_SUPPORT_CMP_NR_RATE (HI_BOARD_VPSS_NR_CMP_RATIO)
#define VPSS_WAIT_MAX_TIME_MS    10000
#define VPSS_SOFTRESET_WAIT_MAX_TIME_US 5
#define VPSS_ME_SCAN_NUM         5

#define VPSS_THREAD_WAIT_IDLE_MS 10

#define VPSS_MIN_HW_BUFFER_INDEX 0
#define VPSS_MAX_HW_BUFFER_INDEX 31

#define DEF_OUT_NODE_MAX_NUM   20
#define VPSS_REG_SHIFT         32
#define VPSS_MAX_ZME_MULTIPLY  16

#define VPSS_DM430_WIDTH       1920
#define VPSS_DM430_HEIGHT      1088
#define VPSS_DM430_MAX_RATE    60

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif





