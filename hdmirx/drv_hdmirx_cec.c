/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2014-2019. All rights reserved.
 * Description: Implementation of cec functions
 * Author: Hisilicon multimedia interface software group
 * Create: 2014/12/27
 */
#include "drv_hdmirx_cec.h"
#include "drv_hdmirx_ctrl.h"
#include "drv_hdmirx_common.h"
#include <securec.h>

#if SUPPORT_CEC

hdmirx_cec_context g_cec_ctx;

static hi_hdmirx_cec_opcode_dbg_info g_opcode_dbg_info_map[] = {
    {HI_DRV_HDMIRX_CEC_OPCODE_SENDPING,                      " PING!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_FEATURE_ABORT,                 " FEATURE_ABORT!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_IMAGE_VIEW_ON,                 " IMAGE_VIEW_ON!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_TEXT_VIEW_ON,                  " TEXT_VIEW_ON!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_SET_MENU_LANGUAGE,             " SET_MENU_LANGUAGE!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_STANDBY,                       " STANDBY!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_USER_CONTROL_PRESSED,          " USER_CONTROL_PRESSED!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_USER_CONTROL_RELEASED,         " USER_CONTROL_RELEASED!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_GIVE_OSD_NAME,                 " GIVE_OSD_NAME!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_SET_OSD_NAME,                  " SET_OSD_NAME!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_SET_OSD_STRING,                " SET_OSD_STRING!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST,     " SYSTEM_AUDIO_MODE_REQUEST!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_GIVE_AUDIO_STATUS,             " GIVE_AUDIO_STATUS!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_SET_SYSTEM_AUDIO_MODE,         " SET_SYSTEM_AUDIO_MODE!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_REPORT_AUDIO_STATUS,           " REPORT_AUDIO_STATUS!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS, " GIVE_SYSTEM_AUDIO_MODE_STATUS!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS,      " SYSTEM_AUDIO_MODE_STATUS!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_ROUTING_CHANGE,                " ROUTING_CHANGE!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_ROUTING_INFORMATION,           " ROUTING_INFORMATION!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_ACTIVE_SOURCE,                 " ACTIVE_SOURCE!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_GIVE_PHYSICAL_ADDRESS,         " GIVE_PHYSICAL_ADDRESS!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_REPORT_PHYSICAL_ADDRESS,       " REPORT_PHYSICAL_ADDRESS!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_REQUEST_ACTIVE_SOURCE,         " REQUEST_ACTIVE_SOURCE!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_SET_STREAM_PATH,               " SET_STREAM_PATH!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_DEVICE_VENDOR_ID,              " DEVICE_VENDOR_ID!! \n"},

    {HI_DRV_HDMIRX_CEC_OPCODE_VENDOR_COMMAND,                " VENDOR_COMMAND!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN,     " VENDOR_REMOTE_BUTTON_DOWN!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP,       " VENDOR_REMOTE_BUTTON_UP!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_GIVE_DEVICE_VENDOR_ID,         " GIVE_DEVICE_VENDOR_ID!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_MENU_REQUEST,                  " MENU_REQUEST!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_MENU_STATUS,                   " MENU_STATUS!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_GIVE_DEVICE_POWER_STATUS,      " GIVE_DEVICE_POWER_STATUS!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_REPORT_POWER_STATUS,           " REPORT_POWER_STATUS!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_GET_MENU_LANGUAGE,             " GET_MENU_LANGUAGE!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_SET_AUDIO_RATE,                " SET_AUDIO_RATE!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_INACTIVE_SOURCE,               " INACTIVE_SOURCE!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_CEC_VERSION,                   " CEC_VERSION!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_GET_CEC_VERSION,               " GET_CEC_VERSION!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_VENDOR_COMMAND_WITH_ID,        " VENDOR_COMMAND_WITH_ID!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_REPORT_SHORT_AUDIO,            " REPORT_SHORT_AUDIO!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_REQUEST_SHORT_AUDIO,           " REQUEST_SHORT_AUDIO!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_INITIATE_ARC,                  " INITIATE_ARC!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_REPORT_ARC_INITIATED,          " REPORT_ARC_INITIATED!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_REPORT_ARC_TERMINATED,         " REPORT_ARC_TERMINATED!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_REQUEST_ARC_INITIATION,        " REQUEST_ARC_INITIATION!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_REQUEST_ARC_TERMINATION,       " REQUEST_ARC_TERMINATION!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_TERMINATE_ARC,                 " TERMINATE_ARC!! \n"},
    {HI_DRV_HDMIRX_CEC_OPCODE_ABORT,                         " ABORT!! \n"}
};

static hi_s32 cec_queue_init(hdmirx_cec_messege_queue *pst_msg_que)
{
    errno_t err_ret;
    pst_msg_que->front = 0;
    pst_msg_que->rear = 0;

    err_ret = memset_s(pst_msg_que->ast_msg_queue_base, CEC_QUEUE_MAX_SIZE * sizeof(hdmirx_cec_message), 0,
        CEC_QUEUE_MAX_SIZE * sizeof(hdmirx_cec_message));
    if (err_ret != EOK) {
        hi_err_hdmirx("secure func call error\n");
        return err_ret;
    }
    return HI_SUCCESS;
}

static hi_bool cec_queue_is_empty(hdmirx_cec_messege_queue *pst_msg_que)
{
    if (pst_msg_que->front == pst_msg_que->rear) {
        return HI_TRUE;
    } else {
        return HI_FALSE;
    }
}

static hi_s32 cec_en_queue(hdmirx_cec_messege_queue *pst_msg_que, hdmirx_cec_message cec_msg)
{
    if ((pst_msg_que->rear + 1) % CEC_QUEUE_MAX_SIZE == pst_msg_que->front) {
        hi_err_hdmirx("Queue Is Full!!\n");
        return HI_FAILURE;
    }

    pst_msg_que->ast_msg_queue_base[pst_msg_que->rear] = cec_msg;
    pst_msg_que->rear = (pst_msg_que->rear + 1) % CEC_QUEUE_MAX_SIZE;

    return HI_SUCCESS;
}

static hi_s32 cec_de_queue(hdmirx_cec_messege_queue *pst_msg_que, hdmirx_cec_message *cec_msg)
{
    if (pst_msg_que->front == pst_msg_que->rear) {
        hi_err_hdmirx("Queue Is Empty\n");
        return HI_FAILURE;
    }

    *cec_msg = pst_msg_que->ast_msg_queue_base[pst_msg_que->front];
    pst_msg_que->front = (pst_msg_que->front + 1) % CEC_QUEUE_MAX_SIZE;

    return HI_SUCCESS;
}

hdmirx_cec_context *hdmirxv2_cec_get_cec_ctx(hi_void)
{
    return &g_cec_ctx;
}

static hi_void hdmirxv2_cec_set_logic_addr(hi_drv_hdmirx_port port, hi_drv_cec_logicaladd logic_addr)
{
    hi_u32 capture_addr[2]; /* 2: array size */
    hi_u8 capture_addr_sel = 0x01;

    capture_addr[0] = 0;
    capture_addr[1] = 0;
    if (logic_addr == 0xFF) {
        logic_addr = HI_DRV_CEC_LOGICALADD_BROADCAST; /* unregistered LA */
    } else if (logic_addr < 8) { /* 8: logic address check threshold */
        capture_addr_sel = capture_addr_sel << logic_addr;
        capture_addr[0] = capture_addr_sel;
    } else {
        capture_addr_sel = capture_addr_sel << (logic_addr - 8); /* 8: move left less 8 bits */
        capture_addr[1] = capture_addr_sel;
    }

    /* set capture address */
    hdmirxv2_hal_cec_set_capture_id(port, capture_addr, sizeof(capture_addr) / sizeof(capture_addr[0]));
    hdmirxv2_hal_cec_set_initiator_id(port, logic_addr);

    g_cec_ctx.tv_loggic_addr = logic_addr;
}

hi_void hdmirxv2_cec_enable(hi_drv_hdmirx_port port, hi_bool enable)
{
    g_cec_ctx.enable = enable;
    if (enable == HI_TRUE) {
        hdmirxv2_hal_cec_enable(port);
    }
}

static hi_void hdmirxv2_cec_translate_opcode_name(hdmirx_cec_message *cec_msg)
{
    hi_u32 index;
    hi_u32 op_code = (hi_u32)(cec_msg->opcode);
    hi_u32 max_index = (hi_u32)(sizeof(g_opcode_dbg_info_map) / sizeof(g_opcode_dbg_info_map));
    for (index = 0; index < max_index; ++index) {
        if (g_opcode_dbg_info_map[index].opcode == op_code) {
            break;
        }
    }
    if (index < max_index) {
        hi_dbg_hdmirx(g_opcode_dbg_info_map[index].dbg_info);
    }
}

hi_void hdmirxv2_cec_message_log(hdmirx_cec_msg_log *cec_log)
{
    hi_u16 i;
    hdmirx_cec_message *cec_msg = cec_log->cec_msg;

    hi_bool tx = cec_log->tx;
    hi_dbg_hdmirx("\n");
    if (tx) {
        hi_dbg_hdmirx("TV send msg");
        hi_dbg_hdmirx_print_u32(cec_msg->src_dest_addr);
        hi_dbg_hdmirx_print_u32(cec_msg->opcode);
    } else {
        hi_dbg_hdmirx("TV received msg");
        hi_dbg_hdmirx_print_u32(cec_msg->src_dest_addr);
        hi_dbg_hdmirx_print_u32(cec_msg->opcode);
    }

    if (cec_msg->arg_count > 14) { /* 14: argument count check theshold */
        cec_msg->arg_count = 14; /* 14: set argument count to 14 */
    }
    for (i = 0; i < cec_msg->arg_count; i++) {
        hi_dbg_hdmirx_print_u32(cec_msg->args[i]);
    }

    /* add human readable command name  */
    hdmirxv2_cec_translate_opcode_name(cec_msg);
}

static hi_void hdmirxv2_cec_soft_init(hi_void)
{
    hi_s32 ret;

    ret = cec_queue_init(&g_cec_ctx.msg_queue);
    if (ret == HI_FAILURE) {
        hi_err_hdmirx("CEC Queue Init Failed!!!\n");
        return;
    }
    g_cec_ctx.cur_state = HDMIRX_CEC_STATE_SENDING;
    g_cec_ctx.tx_state = HDMIRX_CEC_TX_STATUS_BUTT;
    g_cec_ctx.query_cnt = 0;
    g_cec_ctx.cur_msg_cnt = 0;
}

static hi_void hdmirxv2_cec_soft_de_init(hi_void)
{
}

hi_void hdmirxv2_cec_init(hi_drv_hdmirx_port port)
{
    hdmirxv2_hal_cec_reset(port);
    hdmirxv2_hal_cec_de_reset(port);
    hdmirxv2_cec_set_logic_addr(port, HI_DRV_CEC_LOGICALADD_TV);
    hdmirxv2_cec_soft_init();

    hdmirxv2_cec_enable(port, HI_TRUE);

    hdmirxv2_hal_cec_clr_auto_abort(port);

    /* now set to feature abort opcode */
    hdmirxv2_hal_cec_set_abort_opcode(port, CEC_OP_ABORT_31, 0x80); /* HI_UNF_HDMIRX_CEC_OPCODE_ABORT */

#if (!defined(CHIP_TYPE_hi3751v810))
    hdmirxv2_hal_cec_ack_enable(port, HI_FALSE);
#endif
}

hi_void hdmirxv2_cec_de_init(hi_void)
{
    hdmirxv2_cec_soft_de_init();
}

hi_bool hdmirxv2_cec_is_int(hi_drv_hdmirx_port port)
{
    return hdmirxv2_hal_cec_is_int(port);
}

hi_void hdmirxv2_cec_tx_isr(hi_drv_hdmirx_port port)
{
    if (hdmirxv2_hal_read_fld_align(port, CEC_INT_STATUS_0, tx_transmit_buffer_change)) {
        hi_dbg_hdmirx("msg send ok\n");
        g_cec_ctx.cur_msg_cnt = 0;
        g_cec_ctx.tx_state = HDMIRX_CEC_TX_STATUS_SENDACKED;
        g_cec_ctx.cur_state = HDMIRX_CEC_STATE_IDLE;
        g_cec_ctx.query_cnt = 0;
        hdmirxv2_hal_write_fld_align(port, CEC_INT_STATUS_0, tx_transmit_buffer_change, 1);
    }

    if (hdmirxv2_hal_read_fld_align(port, CEC_INT_STATUS_1, tx_frame_retx_count_exceed)) {
        hi_dbg_hdmirx("CEC msg send fail\n");
        g_cec_ctx.cur_msg_cnt = 0;
        hdmirxv2_hal_cec_flush_tx(port);
        g_cec_ctx.tx_state = HDMIRX_CEC_TX_STATUS_SENDFAILED;
        g_cec_ctx.cur_state = HDMIRX_CEC_STATE_IDLE;
        g_cec_ctx.query_cnt = 0;
        hdmirxv2_hal_write_fld_align(port, CEC_INT_STATUS_1, tx_frame_retx_count_exceed, 1);
    }

    if (hdmirxv2_hal_read_fld_align(port, CEC_INT_STATUS_0, tx_transmit_buffer_full)) {
        hi_dbg_hdmirx("[CEC] tx_transmit_buffer_full!!!!\n");
        hdmirxv2_hal_cec_flush_tx(port);
        if (g_cec_ctx.cur_msg_cnt > 3) { /* when buffer full, current msg send failed for 3 times, abandon */
            hi_dbg_hdmirx("tx_transmit_buffer_full,fail!!!!\n");
            g_cec_ctx.cur_state = HDMIRX_CEC_STATE_IDLE;
            g_cec_ctx.tx_state = HDMIRX_CEC_TX_STATUS_SENDFAILED;
            g_cec_ctx.cur_msg_cnt = 0;
        } else {
            hi_dbg_hdmirx("tx_transmit_buffer_full,try again!!!!\n");
            g_cec_ctx.cur_state = HDMIRX_CEC_STATE_SENDING;
            hdmirxv2_cec_en_queue(&g_cec_ctx.cur_cec_msg);
            g_cec_ctx.cur_msg_cnt++;
        }
        osal_msleep(10); /* 10: sleep 10 ms */
        g_cec_ctx.query_cnt = 0;
        hdmirxv2_hal_write_fld_align(port, CEC_INT_STATUS_0, tx_transmit_buffer_full, 1);
    }
}

hi_void hdmirxv2_cec_rx_isr(hi_drv_hdmirx_port port)
{
    hi_bool rx_not_empty;

    if ((hdmirxv2_cec_is_int(port) == HI_FALSE) || (g_cec_ctx.enable == HI_FALSE)) {
        return;
    }
    rx_not_empty = (hi_bool)hdmirxv2_hal_read_fld_align(port, CEC_INT_STATUS_0, rx_fifo_not_empty);
    if (rx_not_empty) {
        hdmirxv2_hal_write_fld_align(port, CEC_INT_STATUS_0, rx_fifo_not_empty, 1);
    }
}

hi_u32 hdmirxv2_cec_en_queue(hdmirx_cec_message *cec_msg)
{
    hi_u32 ret;
    g_cec_ctx.cur_state = HDMIRX_CEC_STATE_SENDING;
    ret = cec_en_queue(&g_cec_ctx.msg_queue, *cec_msg);
    if (ret == HI_FAILURE) {
        hi_err_hdmirx("en_queue ERROR!!\n");
    }
    return ret;
}

hi_u32 hdmirxv2_cec_de_queue(hi_drv_hdmirx_port port)
{
    hi_u32 ret = 0;
    errno_t err_ret;
    hdmirx_cec_message st_cec_msg;
    if (!cec_queue_is_empty(&g_cec_ctx.msg_queue)) {
        ret = cec_de_queue(&g_cec_ctx.msg_queue, &st_cec_msg);
        if (ret == HI_FAILURE) {
            hi_err_hdmirx("de_queue ERROR!!\n");
        } else {
            hdmirxv2_hal_cec_flush_tx(port);
            osal_msleep(10); /* 10: sleep 10 ms */
            hdmirxv2_cec_send_message(port, &st_cec_msg);
            err_ret = memcpy_s(&g_cec_ctx.cur_cec_msg, sizeof(g_cec_ctx.cur_cec_msg), &st_cec_msg, sizeof(st_cec_msg));
            if (err_ret != EOK) {
                hi_err_hdmirx("secure func call error\n");
                return err_ret;
            }
            g_cec_ctx.cur_opcode = st_cec_msg.opcode;
            hi_dbg_hdmirx_print_u32(g_cec_ctx.cur_opcode);
            g_cec_ctx.cur_state = HDMIRX_CEC_STATE_QUERY;
        }
    }

    return ret;
}

hi_void hdmirxv2_cec_send_message(hi_drv_hdmirx_port port, hdmirx_cec_message *cec_msg)
{
    hdmirx_cec_msg_log cec_log;
    hi_u8 cec_cmd_count;
    hdmirxv2_hal_cec_clear_tx_int(port);
    if (cec_msg->opcode == HI_DRV_HDMIRX_CEC_OPCODE_SENDPING) {
        hdmirxv2_hal_cec_set_tx_retry_limit(port, 1);
        hi_dbg_hdmirx("[CEC]send ping!! \n");
        hdmirxv2_hal_cec_send_poll(port, cec_msg->src_dest_addr);
        osal_msleep(1);
    } else {
        cec_cmd_count = cec_msg->arg_count;
        if (cec_cmd_count > CEC_MAX_CMD_SIZE) {
            hi_dbg_hdmirx("cec_msg->arg_count(%d) out of range\n", cec_cmd_count);
            cec_cmd_count = CEC_MAX_CMD_SIZE;
        }
        hdmirxv2_hal_cec_set_dest_id(port, cec_msg->src_dest_addr & 0x0F);
        hdmirxv2_hal_cec_set_tx_cmd(port, ((hi_u32)cec_msg->opcode) & 0xff);
        hdmirxv2_hal_cec_set_tx_oprd(port, cec_msg->args, cec_cmd_count);
        hdmirxv2_hal_cec_set_tx_oprd_cnt(port, cec_cmd_count);
        hdmirxv2_hal_cec_send_cmd(port);
        osal_msleep(1);
    }

    cec_log.cec_msg = cec_msg;
    cec_log.tx = HI_TRUE;
    hdmirxv2_cec_message_log(&cec_log);
    g_cec_ctx.cur_state = HDMIRX_CEC_STATE_QUERY;

    return;
}

hi_s32 hdmirxv2_cec_receive_msg(hi_drv_hdmirx_port port, hdmirx_cec_message *cec_msg)
{
    hi_u8 arg_cnt;
    hdmirx_cec_msg_log cec_log;
    hi_u16 frame_cnt;

    frame_cnt = hdmirxv2_hal_cec_get_frame_wt_rd(port); /* returns the number of frames awaiting reading in the FIFO */

    arg_cnt = hdmirxv2_hal_cec_get_rx_cnt(port);

    /* CEC_RX_COUNT  BIT 7 */
    if (frame_cnt == 0) {
        hdmirxv2_hal_cec_clr_cur_frame(port); /* clear current frame from rx FIFO */
        return HI_FAILURE;
    }

    cec_msg->arg_count = (arg_cnt & cec_reg_rx_cmd_byte_cnt); /* CEC_RX_COUNT  BIT 3_0 */
    cec_msg->src_dest_addr = hdmirxv2_hal_cec_get_rx_header(port);
    cec_msg->opcode = hdmirxv2_hal_cec_get_rx_cmd(port);
    if (cec_msg->arg_count > 0) {
        hdmirxv2_hal_cec_get_rx_oprd(port, cec_msg->args, cec_msg->arg_count, CEC_MAX_CMD_SIZE);
    }

    hdmirxv2_hal_cec_clr_cur_frame(port); /* clear current frame from rx FIFO */
    cec_log.cec_msg = cec_msg;
    cec_log.tx = HI_FALSE;
    hdmirxv2_cec_message_log(&cec_log);

    return HI_SUCCESS;
}

hi_void hdmirxv2_cec_main_loop(hi_drv_hdmirx_port port)
{
    switch (g_cec_ctx.cur_state) {
        case HDMIRX_CEC_STATE_SENDING: {
            hdmirxv2_cec_de_queue(port);
            break;
        }
        case HDMIRX_CEC_STATE_QUERY: {
            hdmirxv2_cec_tx_isr(port);
            if (g_cec_ctx.query_cnt <= 30) { /* msg send 30 times, if keep fail, abondon */
                g_cec_ctx.query_cnt++;
            } else {
                g_cec_ctx.query_cnt = 0;
                g_cec_ctx.cur_state = HDMIRX_CEC_STATE_IDLE;
                g_cec_ctx.tx_state = HDMIRX_CEC_TX_STATUS_BUTT;
            }
            break;
        }
        case HDMIRX_CEC_STATE_IDLE: {
            hdmirxv2_hal_cec_set_tx_retry_limit(port, 4); /* 4: retry count */
        }
        /* fall-through */
        default:
        {
            break;
        }
    }
    osal_msleep(5); /* 5: sleep 5 ms */
}

hi_s32 hdmirxv2_cec_standby_enable(hi_bool enable)
{
    hdmirxv2_cec_hal_pm_reg_write(0x0, enable);
    return HI_SUCCESS;
}

hi_void hdmirxv2_cec_proc_read(hi_void *s)
{
    osal_proc_print(s, "\n---------------CEC info---------------\n");
    osal_proc_print(s, "CEC enable           :   %s\n", g_cec_ctx.enable ? "yes" : "no");
    osal_proc_print(s, "Current status       :   %d\n", g_cec_ctx.cur_state);
    osal_proc_print(s, "Current opcode       :   0x%x\n", g_cec_ctx.cur_opcode);
}
#endif
