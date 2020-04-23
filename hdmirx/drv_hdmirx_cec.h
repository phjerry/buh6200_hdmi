/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Definition of cec functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#ifndef __DRV_HDMIRX_CEC_H__
#define __DRV_HDMIRX_CEC_H__

#include "hi_type.h"
#include "drv_hdmirx_struct.h"

#define TASK_QUEUE_LENGTH       6
#define CEC_MAX_CMD_SIZE        16 /* according to spec, max length is 14,but logic can read max len is 15 */
#define MAKE_SRCDEST(src, dest) ((((src) << 4) & 0xF0) | ((dest)&0x0F))
#define CEC_QUEUE_MAX_SIZE  10

typedef enum {
    HI_DRV_CEC_LOGICALADD_TV = 0X00,
    /* <TV */ /* c_ncomment:<电视 */
    HI_DRV_CEC_LOGICALADD_RECORDDEV_1 = 0X01,
    /* <record device 1 */ /* c_ncomment:<录像机 1 */
    HI_DRV_CEC_LOGICALADD_RECORDDEV_2 = 0X02,
    /* <record device 2 */ /* c_ncomment:<录像机 2 */
    HI_DRV_CEC_LOGICALADD_TUNER_1 = 0X03,
    /* <tuner 1 */ /* c_ncomment:<高频头 1 */
    HI_DRV_CEC_LOGICALADD_PLAYDEV_1 = 0X04,
    /* <play device 1 */ /* c_ncomment:<播放设备 1 */
    HI_DRV_CEC_LOGICALADD_AUDIOSYSTEM = 0X05,
    /* <audio system */ /* c_ncomment:<音频系统 */
    HI_DRV_CEC_LOGICALADD_TUNER_2 = 0X06,
    /* <tuner 2 */ /* c_ncomment:<高频头 2 */
    HI_DRV_CEC_LOGICALADD_TUNER_3 = 0X07,
    /* <tuner 3 */ /* c_ncomment:<高频头 3 */
    HI_DRV_CEC_LOGICALADD_PLAYDEV_2 = 0X08,
    /* <play device 2 */ /* c_ncomment:<播放设备 2 */
    HI_DRV_CEC_LOGICALADD_RECORDDEV_3 = 0X09,
    /* <record device 3 */ /* c_ncomment:<录像机 3 */
    HI_DRV_CEC_LOGICALADD_TUNER_4 = 0X0A,
    /* <tuner 4 */ /* c_ncomment:<高频头 4 */
    HI_DRV_CEC_LOGICALADD_PLAYDEV_3 = 0X0B,
    /* <play device 3 */ /* c_ncomment:<播放设备 3 */
    HI_DRV_CEC_LOGICALADD_RESERVED_1 = 0X0C,
    /* <reserved 1 */ /* c_ncomment:<保留项 1 */
    HI_DRV_CEC_LOGICALADD_RESERVED_2 = 0X0D,
    /* <reserved 2 */ /* c_ncomment:<保留项 2 */
    HI_DRV_CEC_LOGICALADD_SPECIALUSE = 0X0E,
    /* <special use */ /* c_ncomment:<特殊用途 */
    HI_DRV_CEC_LOGICALADD_BROADCAST = 0X0F,
    /* <broadcast */ /* c_ncomment:<广播 */
    HI_DRV_CEC_LOGICALADD_BUTT
} hi_drv_cec_logicaladd;

typedef enum hi_hdmirx_cec_tx_status {
    HDMIRX_CEC_TX_STATUS_SENDACKED = 0,
    HDMIRX_CEC_TX_STATUS_SENDFAILED,

    HDMIRX_CEC_TX_STATUS_BUTT,
} hdmirx_cec_tx_status;

typedef struct hi_hdmirx_cec_message {
    hi_u8 src_dest_addr; /* source in upper nibble, dest in lower nibble */
    hi_drv_hdmirx_cec_opcode opcode;
    hi_u32 args[CEC_MAX_CMD_SIZE];
    hi_u8 arg_count;
    /* hi_u16 u16_msg_id; */
} hdmirx_cec_message;

typedef struct {
    hdmirx_cec_message ast_msg_queue_base[CEC_QUEUE_MAX_SIZE];
    hi_u32 front;
    hi_u32 rear;
} hdmirx_cec_messege_queue;

typedef struct hi_hdmirx_cec_msg_logger {
    hdmirx_cec_message *cec_msg;
    hi_bool tx; /* 判断是发送还是接收 */
} hdmirx_cec_msg_log;
typedef enum hi_hdmirx_cec_state {
    HDMIRX_CEC_STATE_IDLE = 0,
    HDMIRX_CEC_STATE_SENDING,
    HDMIRX_CEC_STATE_QUERY,

    HDMIRX_CEC_STATE_BUTT,
} hdmirx_cec_state;

typedef struct hi_hdmirx_cec_context {
    hi_bool enable;
    hi_bool qurey_by_user;
    hi_u8 tv_loggic_addr;
    hi_u32 query_cnt;
    hdmirx_cec_tx_status tx_state;
    hdmirx_cec_state cur_state;
    hdmirx_cec_messege_queue msg_queue;
    hi_drv_hdmirx_cec_opcode cur_opcode;
    hdmirx_cec_message cur_cec_msg;
    hi_u8 cur_msg_cnt;
} hdmirx_cec_context;

typedef struct {
    hi_drv_hdmirx_cec_opcode opcode;
    const hi_s8* dbg_info;
} hi_hdmirx_cec_opcode_dbg_info;

extern hi_void hdmirxv2_cec_send_message(hi_drv_hdmirx_port port, hdmirx_cec_message *st_cec_msg);
extern hi_void hdmirxv2_cec_main_loop(hi_drv_hdmirx_port port);
extern hi_void hdmirxv2_cec_enable(hi_drv_hdmirx_port port, hi_bool enable);
extern hi_void hdmirxv2_cec_init(hi_drv_hdmirx_port port);
extern hi_void hdmirxv2_cec_de_init(hi_void);
extern hi_s32 hdmirxv2_cec_receive_msg(hi_drv_hdmirx_port port, hdmirx_cec_message *cec_msg);
extern hi_u32 hdmirxv2_cec_en_queue(hdmirx_cec_message *cec_msg);
extern hdmirx_cec_context *hdmirxv2_cec_get_cec_ctx(hi_void);
extern hi_void hdmirxv2_cec_tx_isr(hi_drv_hdmirx_port port);
extern hi_s32 hdmirxv2_cec_standby_enable(hi_bool b_standby);
extern hi_void hdmirxv2_cec_proc_read(hi_void *s);

#endif

