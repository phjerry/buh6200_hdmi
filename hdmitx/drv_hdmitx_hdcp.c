/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver hdcp source file.
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-11-15
 */

#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/hisilicon/securec.h>

#include "hal_hdmitx_phy.h"
#include "drv_hdmitx_ddc.h"
#include "drv_hdmitx_hdcp.h"
#include "drv_hdmitx_connector.h"
#include "drv_hdmitx.h"

#define HDCP_EVENT_LEN 20
#define HDCP1X_KEY_CHECK_BIT_CNT             20
#define HDCP1X_KEY_CHECK_TIMEOUT             5
#define HDCP1X_SHA_WAIT_TIMEOUT              30
#define HDCP1X_BR0_WAIT_TIMEOUT              100
#define HDCP1X_REAUTH_WAIT_MS_AN_FAIL        12
#define HDCP1X_REAUTH_WAIT_MS_R0_FAIL        2100
#define HDCP1X_POLL_INTERVAL                 10
#define HDCP1X_RI_ERROR_CNT                  3
#ifndef HDMI_FPGA_SUPPORT
#define HDCP1X_REAUTH_WAIT_MS_ENCYP          100
#define HDCP1X_REAUTH_WAIT_MS_BKSV_FAIL      2100
#define HDCP1X_BKSVLIST_TIMEOUT              5000
#define HDCP1X_REAUTH_WAIT_MS_RI_FAIL        2130
#define HDCP1X_REAUTH_WAIT_DECRYPTION_REAUTH 210
#else
#define HDCP1X_REAUTH_WAIT_MS_ENCYP          100  /* FPGA value,un-test */
#define HDCP1X_REAUTH_WAIT_MS_RI_FAIL        213  /* FPGA value,1a-07a */
#define HDCP1X_BKSVLIST_TIMEOUT              482  /* FPGA value,1b-03 */
#define HDCP1X_REAUTH_WAIT_MS_BKSV_FAIL      210  /* FPGA value,1a-07 */
#define HDCP1X_REAUTH_WAIT_DECRYPTION_REAUTH 100
#endif

enum hdcp_event_num {
    HDCP_FAIL,
    HDCP_SUCCEES,
    HDCP_CHANGE,
    HDCP_EVENT_BUTT,
};

static hi_void hdcp_reauth_process(struct hdmi_hdcp *hdcp);

static void hdcp_notify_event(struct hdmi_hdcp *hdcp, enum hdcp_event_num number)
{
    char event_buf[HDCP_EVENT_LEN];

    if (number >= HDCP_EVENT_BUTT) {
        HDMI_ERR("err event num:%d\n", number);
    }

    memset_s(event_buf, sizeof(event_buf), '\0', sizeof(event_buf));

    HDMI_INFO("version=%d,success=%d,start=%d,err_code=0x%02x,reauth_time=%d\n",
        hdcp->status.work_version, hdcp->status.auth_success, hdcp->status.auth_start,
        hdcp->status.err_code, hdcp->status.cur_reauth_times);

    snprintf_s(event_buf, HDCP_EVENT_LEN, HDCP_EVENT_LEN,
        "HDCP=%d HDMI=%d", number, hdcp->hdmi->id);

    hdmi_sysfs_event(hdcp->hdmi, event_buf, sizeof(event_buf));
}

static hi_bool hdcp_1x_ksv_check(hi_u8 *ksv_data)
{
    hi_u8 i, j;
    hi_u8 byte;
    hi_u32 set_bit_cnt;
    hi_bool ret;

    for (i = 0, set_bit_cnt = 0; i < HDCP1X_SIZE_5BYTES_KSV; i++) {
        byte = ksv_data[i];
        for (j = 0; j < 8; j++) { /* one byte is 8 bit  */
            if (byte & 0x01) {
                set_bit_cnt++;
            }
            byte >>= 1;
        }
    }
    ret = !!(set_bit_cnt == HDCP1X_KEY_CHECK_BIT_CNT);

    return ret;
}

static hi_s32 hdcp_1x_part1_1(struct hdmi_hdcp *hdcp)
{
    struct hdcp1x_sink_status *sink_1x = &hdcp->sink_st_1x;

    HDMI_INFO("part 1 start\n");

    if (hi_hdmi_hdcp_read(hdcp->hdmi->ddc, HDCP1X_OFFSET_BKSV, sink_1x->b_ksv, HDCP1X_SIZE_5BYTES_KSV)) {
        HDMI_ERR("read bksv fail!\n");
        if (hdcp->cap_sink.support_1x) {
            hdcp->cap_sink.support_1x = false;
            hdcp_notify_event(hdcp, HDCP_CHANGE);
        }
        hdcp->status.err_code = HDCP_1X_BSKV_FAIL;
        hdcp->cur_delay_time = HDCP1X_REAUTH_WAIT_MS_BKSV_FAIL;
        goto fail_exit;
    }

    if (!hdcp_1x_ksv_check(sink_1x->b_ksv)) {
        HDMI_ERR("bksv check fail!\n");
        hdcp->status.err_code = HDCP_1X_BSKV_FAIL;
        hdcp->cur_delay_time = HDCP1X_REAUTH_WAIT_MS_BKSV_FAIL;
        goto fail_exit;
    }

    /* Here, srm check bksv if it is needed */
    if (hi_hdmi_hdcp_read(hdcp->hdmi->ddc, HDCP1X_OFFSET_BCAPS, &sink_1x->bcaps_p1.byte, HDCP1X_SIZE_1BYTES_BCAPS)) {
        HDMI_ERR("read bcap fail!\n");
        hdcp->status.err_code = HDCP_1X_BCAP_FAIL;
        hdcp->cur_delay_time = 0;
        goto fail_exit;
    }

    HDMI_INFO("bcap: %02x\n", sink_1x->bcaps_p1.byte);

    return HI_SUCCESS;

fail_exit:
    HDMI_ERR("part 1 fail\n");
    return HI_FAILURE;
}

static hi_s32 hdcp_1x_part1_2(struct hdmi_hdcp *hdcp)
{
    hi_u32 i;
    struct hdcp1x_sink_status *sink_1x = &hdcp->sink_st_1x;
    struct hdcp1x_src_status *src_1x = &hdcp->src_st_1x;

    hdcp->hal_1x_ops->gerarate_an(hdcp->hdmi, src_1x->an, HDCP1X_SIZE_8BYTES_AN);

    if (hi_hdmi_hdcp_write(hdcp->hdmi->ddc, HDCP1X_OFFSET_AN, src_1x->an, HDCP1X_SIZE_8BYTES_AN)) {
        HDMI_ERR("write an fail!\n");
        hdcp->status.err_code = HDCP_ERR_DDC;
        hdcp->cur_delay_time = HDCP1X_REAUTH_WAIT_MS_AN_FAIL;
        goto fail_exit;
    }

    hdcp->hal_1x_ops->get_a_ksv(hdcp->hdmi, src_1x->a_ksv, HDCP1X_SIZE_5BYTES_KSV);

    if (!hdcp_1x_ksv_check(src_1x->a_ksv)) {
        HDMI_ERR("aksv check fail!\n");
        hdcp->status.err_code = HDCP_ERR_INVALID_KEY;
        hdcp->cur_delay_time = 0;
        goto fail_exit;
    }

    if (hi_hdmi_hdcp_write(hdcp->hdmi->ddc, HDCP1X_OFFSET_AKSV, src_1x->a_ksv, HDCP1X_SIZE_5BYTES_KSV)) {
        HDMI_ERR("write aksv fail!\n");
        hdcp->status.err_code = HDCP_ERR_DDC;
        hdcp->cur_delay_time = 0;
        goto fail_exit;
    }

    HDMI_INFO("repeater=%d\n", sink_1x->bcaps_p1.u8.repeater);

    hdcp->hal_1x_ops->set_repeater(hdcp->hdmi, sink_1x->bcaps_p1.u8.repeater);
    hdcp->hal_1x_ops->set_b_ksv(hdcp->hdmi, sink_1x->b_ksv, HDCP1X_SIZE_5BYTES_KSV);

    for (i = 0; i < HDCP1X_BR0_WAIT_TIMEOUT; i += HDCP1X_POLL_INTERVAL) {
        osal_msleep(HDCP1X_POLL_INTERVAL);
        if (!hdcp->status.auth_start) {
            HDMI_INFO("auth_start=0,eixt hdcp!\n");
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;

fail_exit:
    HDMI_ERR("part 1 fail\n");
    return HI_FAILURE;
}

static hi_s32 hdcp_1x_part1_3(struct hdmi_hdcp *hdcp)
{
    hi_u32 i;
    struct hdcp1x_sink_status *sink_1x = &hdcp->sink_st_1x;
    struct hdcp1x_src_status *src_1x = &hdcp->src_st_1x;

    if (hi_hdmi_hdcp_read(hdcp->hdmi->ddc, HDCP1X_OFFSET_BRI, sink_1x->b_r0, HDCP1X_SIZE_2BYTES_RI)) {
        HDMI_ERR("read r0' fail!\n");
        hdcp->status.err_code = HDCP_ERR_DDC;
        hdcp->cur_delay_time = HDCP1X_REAUTH_WAIT_MS_R0_FAIL;
        goto fail_exit;
    }

    hdcp->hal_1x_ops->get_a_r0(hdcp->hdmi, src_1x->a_r0, HDCP1X_SIZE_2BYTES_RI);

    HDMI_INFO("r0 : %02x %02x\n", src_1x->a_r0[0], src_1x->a_r0[1]);    /* byte 0 1 */
    HDMI_INFO("r0': %02x %02x\n", sink_1x->b_r0[0], sink_1x->b_r0[1]);  /* byte 0 1 */

    if (src_1x->a_r0[0] != sink_1x->b_r0[0] || src_1x->a_r0[1] != sink_1x->b_r0[1]) { /* r0 r1 */
        HDMI_ERR("r0 != r0'\n");
        hdcp->status.err_code = HDCP_1X_INTEGRITY_FAIL_R0;
        hdcp->cur_delay_time = HDCP1X_REAUTH_WAIT_MS_R0_FAIL;
        goto fail_exit;
    }

    hdcp->hal_1x_ops->set_encryption(hdcp->hdmi, hdcp->status.auth_start); /* enable encrypiton */

    for (i = 0; i < HDCP1X_REAUTH_WAIT_MS_ENCYP; i += HDCP1X_POLL_INTERVAL) {
        if (!hdcp->status.auth_start) {
            HDMI_INFO("auth_start=0,eixt hdcp!\n");
            return HI_FAILURE;
        }
        osal_msleep(HDCP1X_POLL_INTERVAL);
    }

    HDMI_INFO("part 1 success\n");

    return HI_SUCCESS;

fail_exit:
    HDMI_ERR("part 1 fail\n");
    return HI_FAILURE;
}


static hi_s32 hdcp_1x_part2_1(struct hdmi_hdcp *hdcp)
{
    hi_u32 timeout;
    struct hdcp1x_sink_status *sink_1x = &hdcp->sink_st_1x;

    HDMI_INFO("part2 start\n");

    for (sink_1x->bcaps_p2.byte = 0, timeout = 0;
        (!sink_1x->bcaps_p2.u8.ksv_fifo_ready) && timeout < HDCP1X_BKSVLIST_TIMEOUT;
        timeout += HDCP1X_POLL_INTERVAL) {
        if (hi_hdmi_hdcp_read(hdcp->hdmi->ddc, HDCP1X_OFFSET_BCAPS,
            &sink_1x->bcaps_p2.byte, HDCP1X_SIZE_1BYTES_BCAPS)) {
            HDMI_ERR("read bcaps fail\n");
            break;
        }

        HDMI_INFO("bcaps: 0x%02x\n", sink_1x->bcaps_p2.byte);

        if (!hdcp->status.auth_start) {
            HDMI_INFO("auth_start=0,eixt!\n");
            return HI_FAILURE;
        }
        osal_msleep(HDCP1X_POLL_INTERVAL);
    }

    if (!sink_1x->bcaps_p2.u8.ksv_fifo_ready) {
        HDMI_ERR("bksv list not ready.wait timeout=%u(limit: < 5000)\n", timeout);
        hdcp->status.err_code = HDCP_1X_WATCHDOG_TIMEOUT;
        hdcp->cur_delay_time = 0;
        goto fail_exit;
    }

    return HI_SUCCESS;

fail_exit:
    HDMI_ERR("part 2 fail\n");
    return HI_FAILURE;
}

static hi_s32 hdcp_1x_part2_2(struct hdmi_hdcp *hdcp)
{
    hi_u8 bstatus[HDCP1X_SIZE_2BYTES_BSTATUS];
    struct hdcp1x_sink_status *sink_1x = &hdcp->sink_st_1x;

    if (hi_hdmi_hdcp_read(hdcp->hdmi->ddc, HDCP1X_OFFSET_BSTATUS, bstatus, HDCP1X_SIZE_2BYTES_BSTATUS)) {
        HDMI_ERR("read bstatus fail\n");
        hdcp->status.err_code = HDCP_ERR_DDC;
        hdcp->cur_delay_time = HDCP1X_REAUTH_WAIT_DECRYPTION_REAUTH;
        goto fail_exit;
    }
    sink_1x->bstatus.word = bstatus[1];  /* byte 1 */
    sink_1x->bstatus.word <<= 8;         /* hight 8 bit */
    sink_1x->bstatus.word |= bstatus[0]; /* byte 0 */
    HDMI_INFO("bstatus: 0x%04x (depth=%d,dev_cnt=%d)\n", sink_1x->bstatus.word,
        sink_1x->bstatus.u16.depth, sink_1x->bstatus.u16.device_cnt);

    hdcp->hal_1x_ops->set_b_status(hdcp->hdmi, sink_1x->bstatus.word);
    if (sink_1x->bstatus.u16.max_devs_exceeded || sink_1x->bstatus.u16.max_cascade_exceeded) {
        HDMI_ERR("exeeded topology fail\n");
        hdcp->status.err_code = HDCP_1X_EXCEEDED_TOPOLOGY;
        hdcp->cur_delay_time = HDCP1X_REAUTH_WAIT_DECRYPTION_REAUTH;
        goto fail_exit;
    }
    hdcp->hal_1x_ops->set_no_downstream(hdcp->hdmi, sink_1x->bstatus.u16.depth <= 1); /* <= 1 no downstream */
    hdcp->hal_1x_ops->set_sha_calculate(hdcp->hdmi, true);
    sink_1x->bksv_list_size = sink_1x->bstatus.u16.device_cnt * HDCP1X_SIZE_5BYTES_KSV;
    if (hi_hdmi_hdcp_read(hdcp->hdmi->ddc, HDCP1X_OFFSET_KSV_FIFO,
        sink_1x->bksv_list_data, sink_1x->bksv_list_size)) {
        HDMI_ERR("read bksv list fail\n");
        hdcp->status.err_code = HDCP_ERR_DDC;
        hdcp->cur_delay_time = HDCP1X_REAUTH_WAIT_DECRYPTION_REAUTH;
        goto fail_exit;
    }
    hdcp->hal_1x_ops->set_b_ksv_list(hdcp->hdmi, sink_1x->bksv_list_data, sink_1x->bksv_list_size);
    return HI_SUCCESS;

fail_exit:
    HDMI_ERR("part 2 fail\n");
    return HI_FAILURE;
}

static hi_s32 hdcp_1x_part2_3(struct hdmi_hdcp *hdcp)
{
    hi_u32 timeout;

    struct hdcp1x_sink_status *sink_1x = &hdcp->sink_st_1x;
    struct hdcp1x_src_status *src_1x = &hdcp->src_st_1x;

    for (src_1x->sha_ready = false, timeout = 0;
        !src_1x->sha_ready && timeout < HDCP1X_SHA_WAIT_TIMEOUT;
        timeout += HDCP1X_POLL_INTERVAL) {
        src_1x->sha_ready = hdcp->hal_1x_ops->is_sha_ready(hdcp->hdmi);
        if (!hdcp->status.auth_start) {
            HDMI_INFO("auth_start=0,exit!\n");
            return HI_FAILURE;
        }
        osal_msleep(HDCP1X_POLL_INTERVAL);
    }

    if (!src_1x->sha_ready) {
        HDMI_WARN("sha timeout\n");
    }

    if (hi_hdmi_hdcp_read(hdcp->hdmi->ddc, HDCP1X_OFFSET_BVH0,
        sink_1x->b_vi, HDCP1X_SIZE_20BYTES_VI)) {
        HDMI_ERR("read b_vi fail\n");
        hdcp->status.err_code = HDCP_ERR_DDC;
        hdcp->cur_delay_time = HDCP1X_REAUTH_WAIT_DECRYPTION_REAUTH;
        goto fail_exit;
    }

    hdcp->hal_1x_ops->get_a_vi(hdcp->hdmi, src_1x->a_vi, HDCP1X_SIZE_20BYTES_VI);
    hdcp->hal_1x_ops->set_b_vi(hdcp->hdmi, sink_1x->b_vi, HDCP1X_SIZE_20BYTES_VI);

    if (!hdcp->hal_1x_ops->is_vi_valid_ok(hdcp->hdmi)) {
        HDMI_ERR("vi valid fail!\n");
        hdcp->status.err_code = HDCP_1X_VI_CHCECK_FAIL;
        hdcp->cur_delay_time = HDCP1X_REAUTH_WAIT_DECRYPTION_REAUTH;
        goto fail_exit;
    }
    HDMI_INFO("part 2 success!\n");
    return HI_SUCCESS;

fail_exit:
    HDMI_ERR("part 2 fail\n");
    return HI_FAILURE;
}

static hi_void hdcp_1x_part3_1(struct hdmi_hdcp *hdcp)
{
    HDMI_INFO("part 3 start\n");
    hdcp->hal_1x_ops->set_irq(hdcp->hdmi, true);
    hdcp->hal_1x_ops->set_ri_auto_validate(hdcp->hdmi, true);
}

static void hdcp_1x_part3_2_irq_handle(struct hdmi_hdcp *hdcp)
{
    hi_u32 irq_status = 1;
    struct hdcp1x_ri_result result;

    hdcp->hal_1x_ops->get_irq_status(hdcp->hdmi, &irq_status);
    hdcp->hal_1x_ops->get_ri_validate_result(hdcp->hdmi, &result);
    if (result.cnt_000frm_err || result.cnt_127frm_err ||
        result.cnt_nodone_err || result.cnt_notchg_err) {
        HDMI_WARN("fsm_state=%d, 000frm_err=%d, 127frm_err=%d, nodone_err=%d, notchg_err=%d\n",
            result.fsm_state, result.cnt_000frm_err, result.cnt_127frm_err,
            result.cnt_nodone_err, result.cnt_notchg_err);

        hdcp->hal_1x_ops->clear_ri_err(hdcp->hdmi);
        hdcp->hdcp1x_ri_err_cnt++;

        if (hdcp->hdcp1x_ri_err_cnt >= HDCP1X_RI_ERROR_CNT) {
            HDMI_ERR("ri check fail.hdcp1x_ri_err_cnt=%d\n", hdcp->hdcp1x_ri_err_cnt);
            hdcp->hdcp1x_ri_err_cnt = 0;
            hdcp->status.err_code = HDCP_1X_INTEGRITY_FAIL_RI;
            hdcp->cur_delay_time = HDCP1X_REAUTH_WAIT_MS_RI_FAIL;
            hdcp_reauth_process(hdcp);

            return;
        } else {
            goto clear_irq_exit;
        }
    }

    hdcp->hdcp1x_ri_err_cnt = 0;

clear_irq_exit:
    hdcp->hal_1x_ops->clear_irq_status(hdcp->hdmi, irq_status);
    hdcp->hal_1x_ops->set_irq(hdcp->hdmi, hdcp->status.auth_start);
}

static void hdcp_1x_stop_process(struct hdmi_hdcp *hdcp)
{
    hdcp->hal_1x_ops->set_encryption(hdcp->hdmi, false);
    hdcp->hal_1x_ops->set_irq(hdcp->hdmi, false);
    hdcp->hal_1x_ops->clear_irq_status(hdcp->hdmi, 0xffffffff);
    hdcp->hal_1x_ops->set_repeater(hdcp->hdmi, false);
    hdcp->hal_1x_ops->set_sha_calculate(hdcp->hdmi, false);
    hdcp->hal_1x_ops->set_ri_auto_validate(hdcp->hdmi, false);
}

static void hdcp_1x_start_process(struct hdmi_hdcp *hdcp)
{
    if (!hdcp->status.auth_start) {
        HDMI_INFO("auth_start=0,exit\n");
        hdcp_1x_stop_process(hdcp);
        return ;
    }

    hdcp->hal_1x_ops->set_mode(hdcp->hdmi);
    hdcp->hal_1x_ops->set_1x_srst(hdcp->hdmi, false);

    if (hdcp->hal_1x_ops->load_key(hdcp->hdmi)) {
        HDMI_ERR("key1x crc check fail! load_1x_key=%d\n", hdcp->load_1x_key);
        hdcp->status.err_code = HDCP_ERR_INVALID_KEY;
        hdcp->cur_delay_time = 0;
        goto is_re_auth;
    }

    /* part 1 */
    if (hdcp_1x_part1_1(hdcp)) {
        goto is_re_auth;
    }
    if (hdcp_1x_part1_2(hdcp)) {
        goto is_re_auth;
    }
    if (hdcp_1x_part1_3(hdcp)) {
        goto is_re_auth;
    }

    /* part 2 */
    if (!hdcp->sink_st_1x.bcaps_p1.u8.repeater) {
        HDMI_INFO("part2 skip, no repeater!\n");
    } else {
        if (hdcp_1x_part2_1(hdcp)) {
            goto is_re_auth;
        }
        if (hdcp_1x_part2_2(hdcp)) {
            goto is_re_auth;
        }
        if (hdcp_1x_part2_3(hdcp)) {
            goto is_re_auth;
        }
    }
    /* part 3 */
    hdcp_1x_part3_1(hdcp);
    hdcp->status.cur_reauth_times = 0;
    hdcp->status.auth_success = true;
    hdcp_notify_event(hdcp, HDCP_SUCCEES);
    return ;
    /* re-auth */
is_re_auth:
    hdcp_reauth_process(hdcp);
}

static void hdcp_2x_stop_process(struct hdmi_hdcp *hdcp)
{
    union hdcp2x_irq irq_status;
    hi_u32 timeout;

    hdcp->hal_2x_ops->set_encryption(hdcp->hdmi, false);
    hdcp->hal_2x_ops->set_mcu_start(hdcp->hdmi, false);
    osal_msleep(100);  /* wait 100ms for de-encryption */
    for (timeout = 0; timeout < 50; timeout++) { /* 50ms is timeout */
        hdcp->hal_2x_ops->get_irq_status(hdcp->hdmi, &irq_status.u32_data);
        if (irq_status.bytes.stop_done) {
            break;
        }
        osal_msleep(1); /* sleep 1 ms for every time */
    }

    HDMI_INFO("stop hdcp2x.irq=0x%08x,stop_done=%d, timeout=%d\n",
        irq_status.u32_data, irq_status.bytes.stop_done, timeout);

    hdcp->hal_2x_ops->set_irq(hdcp->hdmi, false);
    hdcp->hal_2x_ops->clear_irq(hdcp->hdmi, irq_status.u32_data);
    hdcp->hal_2x_ops->set_mcu_srst(hdcp->hdmi, true);
}

static void hdcp_2x_start_process(struct hdmi_hdcp *hdcp)
{
    hi_u8 version[HDCP2X_SIZE_SW_VERSION];
    if (!hdcp->status.auth_start) {
        HDMI_INFO("auth_start=0,exit!\n");
        return ;
    }

    hdcp->hal_2x_ops->set_2x_srst(hdcp->hdmi, false);
    hdcp->hal_1x_ops->set_1x_srst(hdcp->hdmi, true);
    hdcp->hal_2x_ops->get_sw_version(hdcp->hdmi, version, HDCP2X_SIZE_SW_VERSION);
    HDMI_INFO("mcu sw version:20%02d.%02d.%02d %02d\n",
        version[0], version[1], version[2], version[3]); /* byte 0 1 2 3 */
    /* set stream message */
    hdcp->hal_2x_ops->set_stream_msg(hdcp->hdmi, &hdcp->src_st_2x.str_msg);
    /* clear irq */
    hdcp->hal_2x_ops->clear_irq(hdcp->hdmi, 0xffffffff);
    /* mode select */
    hdcp->hal_2x_ops->set_mode(hdcp->hdmi);
    /* de-mcu-srst */
    hdcp->hal_2x_ops->set_mcu_srst(hdcp->hdmi, false);
    /* enalbe irq */
    hdcp->hal_2x_ops->set_irq(hdcp->hdmi, true);
    /* note that sync mode true will make encryption not in time */
    hdcp->hal_2x_ops->set_sync_mode(hdcp->hdmi, true);
    hdcp->hal_2x_ops->set_encryption(hdcp->hdmi, true);
    /* mcu start */
    hdcp->hal_2x_ops->set_mcu_start(hdcp->hdmi, true);
}

static void hdcp_2x_irq_recv_id_list(struct hdmi_hdcp *hdcp)
{
    struct hdcp2x_sink_status *sink_2x = &hdcp->sink_st_2x;

    hdcp->hal_2x_ops->get_rx_info(hdcp->hdmi, &sink_2x->rx_info);
    hdcp->hal_2x_ops->get_recv_id_list(hdcp->hdmi, sink_2x->recv_id_list,
        HDCP2X_RECVID_SIZE * sink_2x->rx_info.dev_cnt);
    HDMI_INFO("recieve id list ready!\n");

    if (sink_2x->rx_info.max_devs_exceeded ||
        sink_2x->rx_info.max_cascade_exceeded) {
        hdcp->status.err_code = HDCP_2X_EXCEEDED_TOPOLOGY;
        hdcp->status.auth_success = false;
        HDMI_WARN("max_devs_exceeded=%d,max_cascade_exceeded=%d\n",
            sink_2x->rx_info.max_devs_exceeded, sink_2x->rx_info.max_cascade_exceeded);
    }
}

static void hdcp_2x_irq_handle(struct hdmi_hdcp *hdcp)
{
    union hdcp2x_irq irq_status;
    struct hdcp2x_sink_status *sink_2x = NULL;

    sink_2x = &hdcp->sink_st_2x;
    hdcp->hal_2x_ops->get_irq_status(hdcp->hdmi, &irq_status.u32_data);
    HDMI_INFO("irq status:0x%08x\n", irq_status.u32_data);
    if (!hdcp->status.auth_start) {
        HDMI_INFO("exit hdcp.auth_start=%d\n", hdcp->status.auth_start);
        return ;
    }

    if (irq_status.bytes.revid_ready) {
        hdcp->hal_2x_ops->get_recv_id(hdcp->hdmi, sink_2x->recv_id, sizeof(sink_2x->recv_id));
        HDMI_INFO("recieve id ready\n"); /* 3 4 is array subscript */
    }

    if (irq_status.bytes.auth_done) {
        hdcp->status.err_code = HDCP_ERR_NONE;
        hdcp->status.auth_success = true;
        hdcp->status.cur_reauth_times = 0;
        HDMI_INFO("auth success.cur_reauth_times=%d,clear to 0.\n", hdcp->status.cur_reauth_times);
        hdcp_notify_event(hdcp, HDCP_SUCCEES);
    }

    if (irq_status.bytes.revid_list_ready) {
        hdcp_2x_irq_recv_id_list(hdcp);
    }

    if (irq_status.bytes.reauth_requset) {
        hdcp->status.err_code = HDCP_2X_REAUTH_REQ;
        hdcp->cur_delay_time = hdcp->start_delay_time;
        HDMI_WARN("sink reauth_requset\n");
        hdcp_reauth_process(hdcp);
        return ;
    }

    if (irq_status.bytes.auth_fail) {
        hdcp->status.err_code = HDCP_2X_V_MISMATCH;
        hdcp->cur_delay_time = hdcp->start_delay_time;
        HDMI_WARN("auth fail.\n");
        hdcp_reauth_process(hdcp);
        return ;
    }

    /* clear irq */
    hdcp->hal_2x_ops->clear_irq(hdcp->hdmi, irq_status.u32_data);
    /* enable irq */
    hdcp->hal_2x_ops->set_irq(hdcp->hdmi, true);
    return ;
}

static hi_void hdcp_reauth_process(struct hdmi_hdcp *hdcp)
{
    hdcp->status.auth_success = false;

    if (hdcp->status.work_version == HDCP_VERSION_HDCP1X) {
        hdcp_1x_stop_process(hdcp);
    } else if (hdcp->status.work_version == HDCP_VERSION_HDCP22 ||
        hdcp->status.work_version == HDCP_VERSION_HDCP23) {
        hdcp_2x_stop_process(hdcp);
    } else {
        hdcp_1x_stop_process(hdcp);
        hdcp_2x_stop_process(hdcp);
        hdcp->status.auth_start = false;
        HDMI_INFO("work version none!exit hdcp.\n");
        return ;
    }

    HDMI_INFO("hdcp1x fail.cur_reauth_times=%d,usr_reauth_times=%d\n",
        hdcp->status.cur_reauth_times, hdcp->usr_reauth_times);

    if (!hdcp->status.auth_start) {
        HDMI_INFO("auth_start=0.exit hdcp!\n");
        return ;
    } else if ((hdcp->usr_reauth_times != HDCP_DEFAULT_REAUTH_TIMES &&
        hdcp->status.cur_reauth_times >= hdcp->usr_reauth_times)) {
        hdcp->status.auth_start = false;
        hdcp_notify_event(hdcp, HDCP_FAIL);
    } else {
        hdcp->status.cur_reauth_times++;
        hdcp_notify_event(hdcp, HDCP_FAIL);
        HDMI_INFO("reauth start!cur_delay_time=%d\n", hdcp->cur_delay_time);
        schedule_delayed_work(&hdcp->start_work, hdcp->cur_delay_time);
    }
}

static irqreturn_t hdcp_irq_server(hi_s32 irq, void *data)
{
    struct hdmi_hdcp *hdcp = (struct hdmi_hdcp *)data;
    hi_u32 irq_status = 0;

    if (hdcp == NULL) {
        return IRQ_HANDLED;
    }

    if (hdcp->status.work_version == HDCP_VERSION_HDCP1X) {
        hdcp->hal_1x_ops->get_irq_status(hdcp->hdmi, &irq_status);
        if (irq_status) {
            /* disable irq. It should be enable after irq handle finish. */
            hdcp->hal_1x_ops->set_irq(hdcp->hdmi, false);
            schedule_delayed_work(&hdcp->irq_handle, 0);
        }
    } else if (hdcp->status.work_version == HDCP_VERSION_HDCP22 ||
        hdcp->status.work_version == HDCP_VERSION_HDCP23) {
        hdcp->hal_2x_ops->get_irq_status(hdcp->hdmi, &irq_status);
        if (irq_status) {
            /* disable irq. It should be enable after irq handle finish. */
            hdcp->hal_2x_ops->set_irq(hdcp->hdmi, false);
            schedule_delayed_work(&hdcp->irq_handle, 0);
        }
    }

    return irq_status ? IRQ_HANDLED : IRQ_NONE;
}

static void hdcp_irq_handle(struct work_struct *work)
{
    struct hdmi_hdcp *hdcp = NULL;

    if (work == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdcp = container_of(to_delayed_work(work), struct hdmi_hdcp, irq_handle);

    if (hdcp->status.work_version == HDCP_VERSION_HDCP1X) {
        hdcp_1x_part3_2_irq_handle(hdcp);
    } else if (hdcp->status.work_version == HDCP_VERSION_HDCP22 ||
        hdcp->status.work_version == HDCP_VERSION_HDCP23) {
        hdcp_2x_irq_handle(hdcp);
    }
}

static void hdcp_stop_work_queue(struct hdmi_hdcp *hdcp)
{
    if (hdcp == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdcp->status.auth_start = false;
    hdcp_1x_stop_process(hdcp);
    hdcp_2x_stop_process(hdcp);
    cancel_delayed_work_sync(&hdcp->irq_handle);
    cancel_delayed_work_sync(&hdcp->start_work);
    HDMI_INFO("stop hdcp\n");
}

static void hdcp_start_workqueue(struct work_struct *work)
{
    struct hdmi_hdcp *hdcp = NULL;

    if (work == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdcp = container_of(to_delayed_work(work), struct hdmi_hdcp, start_work);

    if (hdcp->status.work_version == HDCP_VERSION_NONE) {
        HDMI_INFO("start none,version=%d\n", hdcp->status.work_version);
        return ;
    }

    hdcp->status.err_code = HDCP_ERR_NONE;

    if (hdcp->status.work_version == HDCP_VERSION_HDCP1X) {
        if (!hdcp->load_1x_key) {
            HDMI_ERR("un-load cipher key 1x!\n");
            hdcp->status.auth_start = false;
            hdcp->status.err_code = HDCP_ERR_NO_KEY;
            hdcp_notify_event(hdcp, HDCP_FAIL);
            return ;
        }
        if (memset_s(&hdcp->sink_st_1x, sizeof(struct hdcp1x_sink_status), 0, sizeof(struct hdcp1x_sink_status))) {
            HDMI_ERR("memset_s fail.\n");
        }
        if (memset_s(&hdcp->src_st_1x, sizeof(struct hdcp1x_src_status), 0, sizeof(struct hdcp1x_src_status))) {
            HDMI_ERR("memset_s fail.\n");
        }
        hdcp_1x_stop_process(hdcp);
        HDMI_INFO("HDMI%d start hdcp1x\n", hdcp->hdmi->id);
        hdcp_1x_start_process(hdcp);
    } else if (hdcp->status.work_version == HDCP_VERSION_HDCP22 ||
        hdcp->status.work_version == HDCP_VERSION_HDCP23) {
        if (!hdcp->load_mcu_code2x) {
            HDMI_INFO("un-load mcu code 2x!\n");
            hdcp->status.auth_start = false;
            hdcp->status.err_code = HDCP_ERR_INVALID_KEY;
            hdcp_notify_event(hdcp, HDCP_FAIL);
            return ;
        }

        if (memset_s(&hdcp->sink_st_2x, sizeof(struct hdcp2x_sink_status), 0, sizeof(struct hdcp2x_sink_status))) {
            HDMI_ERR("memset_s fail.\n");
        }

        HDMI_INFO("HDMI%d start hdcp2x\n", hdcp->hdmi->id);
        hdcp_2x_start_process(hdcp);
    }
}

static hi_u32 hdcp_select_work_version(struct hdmi_hdcp *hdcp, hi_u32 mode)
{
    hi_u32 work_version =  HDCP_VERSION_NONE;

    switch (mode) {
        case HDCP_MODE_AUTO:
            if (hdcp->cap_sink.support_23 && hdcp->cap_src.support_23) {
                work_version =  HDCP_VERSION_HDCP23;
            } else if (hdcp->cap_sink.support_22 && hdcp->cap_src.support_22) {
                work_version =  HDCP_VERSION_HDCP22;
            } else if (hdcp->cap_sink.support_1x && hdcp->cap_src.support_1x) {
                work_version =  HDCP_VERSION_HDCP1X;
            } else {
                hdcp->status.err_code = HDCP_ERR_NO_CAP;
            }
            break;
        case HDCP_MODE_1X:
            if (hdcp->cap_sink.support_1x && hdcp->cap_src.support_1x) {
                work_version =  HDCP_VERSION_HDCP1X;
            } else {
                hdcp->status.err_code = HDCP_1X_NO_CAP;
            }
            break;
        case HDCP_MODE_22:
            if (hdcp->cap_sink.support_22 && hdcp->cap_src.support_22) {
                work_version =  HDCP_VERSION_HDCP22;
            } else {
                hdcp->status.err_code = HDCP_2X_NO_CAP;
            }
            break;
        case HDCP_MODE_23:
            if (hdcp->cap_sink.support_23 && hdcp->cap_src.support_23) {
                work_version =  HDCP_VERSION_HDCP23;
            } else {
                hdcp->status.err_code = HDCP_2X_NO_CAP;
            }
            break;
        default:
            hdcp->status.err_code = HDCP_ERR_NO_CAP;
            break;
    }

    return work_version;
}

static void hdcp_data_init(struct hisilicon_hdmi *hdmi, struct hdmi_hdcp *hdcp)
{
    if (hdmi == NULL || hdcp == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }
    hdcp->hdmi = hdmi;
    mutex_init(&hdcp->mutex);
    INIT_DELAYED_WORK(&hdcp->start_work, hdcp_start_workqueue);
    INIT_DELAYED_WORK(&hdcp->irq_handle, hdcp_irq_handle);

    memset_s(hdcp->name, HDCP_NAME_SIZE, '\0', HDCP_NAME_SIZE);
    snprintf_s(hdcp->name, HDCP_NAME_SIZE, HDCP_NAME_SIZE, "hdcp_irq");
    if (request_irq(hdmi->pwd_irq, hdcp_irq_server, IRQF_SHARED, hdcp->name, hdcp)) {
        HDMI_ERR("hdcp request_irq fail!\n");
    }

    hdcp->usr_mode = HDCP_MODE_BUTT;
    hdcp->usr_reauth_times = HDCP_DEFAULT_REAUTH_TIMES;
    hdcp->status.err_code = HDCP_ERR_UNDO;

    hdcp->cap_src.support_1x = true;
    hdcp->cap_src.support_22 = true;
    hdcp->cap_src.support_23 = true;

    hdcp->src_st_2x.str_msg.k0 = 0x1; /* 0x1 is k0 */
    hdcp->src_st_2x.str_msg.k1 = 0x0; /* 0x0 is k1 */
    hdcp->src_st_2x.str_msg.stream_id = 0x0; /* 0x0 is stream id */
    hdcp->src_st_2x.str_msg.stream_type = 0x1; /* 0x1 is stream type */
    hdcp->start_delay_time = 10; /* 10ms is start delay time */
}

hi_s32 drv_hdmitx_hdcp_init(struct hisilicon_hdmi *hdmi)
{
    struct hdmi_hdcp *hdcp = NULL;
    hi_s32 ret;

    if (hdmi == NULL || hdmi->ddc == NULL) {
        HDMI_ERR("null ptr\n");
        return HI_FAILURE;
    }

    if (hdmi->hdcp) {
        HDMI_WARN("multi-init\n");
        return HI_SUCCESS;
    }

    hdcp = kzalloc(sizeof(struct hdmi_hdcp), GFP_KERNEL);
    if (hdcp == NULL) {
        HDMI_ERR("kzalloc fail!\n");
        return HI_FAILURE;
    }
    hdmi->hdcp  = hdcp;
    hdcp->name = kzalloc(HDCP_NAME_SIZE, GFP_KERNEL);
    if (hdcp->name == NULL) {
        HDMI_ERR("kzalloc fail!\n");
        goto kfree_hdcp_eixt;
    }

    hdcp_data_init(hdmi, hdcp);
    hdcp->hal_1x_ops = hal_hdmitx_hdcp1x_get_ops();
    hdcp->hal_2x_ops = hal_hdmitx_hdcp2x_get_ops();
    if (hdcp->hal_1x_ops == NULL || hdcp->hal_2x_ops == NULL) {
        HDMI_ERR("null ptr");
        goto kfree_name_eixt;
    }
    ret = hdcp->hal_2x_ops->load_mcu_code(hdmi);
    HDMI_INFO("load mcu code2x ret=%d(0 is succeess)!\n", ret);
    hdcp->load_mcu_code2x = !ret;
    return HI_SUCCESS;
kfree_name_eixt:
    if (hdmi->hdcp->name) {
        kfree(hdmi->hdcp->name);
    }
kfree_hdcp_eixt:
    if (hdmi->hdcp) {
        kfree(hdmi->hdcp);
        hdmi->hdcp = NULL;
    }
    return HI_FAILURE;
}

void drv_hdmitx_hdcp_deinit(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == NULL || hdmi->hdcp == NULL) {
        HDMI_ERR("null ptr!\n");
        return ;
    }
    mutex_lock(&hdmi->hdcp->mutex);
    if (hdmi->hdcp->status.auth_start) {
        hdcp_stop_work_queue(hdmi->hdcp);
    }
    hdmi->hdcp->hal_2x_ops->set_mcu_srst(hdmi, true);
    hdmi->hdcp->hal_1x_ops->set_1x_srst(hdmi, true);
    free_irq(hdmi->pwd_irq, hdmi->hdcp);
    mutex_unlock(&hdmi->hdcp->mutex);
    if (hdmi->hdcp->name) {
        kfree(hdmi->hdcp->name);
        hdmi->hdcp->name = NULL;
    }
    kfree(hdmi->hdcp);
    hdmi->hdcp = NULL;
}

void drv_hdmitx_hdcp_on(struct hdmi_hdcp *hdcp)
{
    hi_u32 mode, work_version;;

    if (hdcp == NULL) {
        return ;
    }
    mutex_lock(&hdcp->mutex);
    if (!hdcp->usr_start) {
        goto unlock_exit;
    }

    mode = hdcp->usr_mode;
    HDMI_INFO("hdcp on, user mode=%d\n", mode);

    /* hotplug check */
    if (hdcp->hdmi->connector->status == HPD_PLUGOUT) {
        HDMI_WARN("fail,cable plug out\n");
        hdcp->status.err_code = HDCP_ERR_PLUG_OUT;
        hdcp_notify_event(hdcp, HDCP_FAIL);
        goto unlock_exit;
    }

    /* version capability check */
    work_version = hdcp_select_work_version(hdcp, mode);
    if (work_version == HDCP_VERSION_NONE) {
        HDMI_ERR("no support mode:%d\n", mode);
        hdcp_notify_event(hdcp, HDCP_FAIL);
        goto unlock_exit;
    }

    /* hdcp start check */
    if (hdcp->status.work_version == work_version && hdcp->status.auth_start) {
        HDMI_WARN("busy,no need to re-start!\n");
        goto unlock_exit;
    } else if(hdcp->status.work_version != work_version && hdcp->status.auth_start) {
        HDMI_INFO("version change:%d->%d\n", hdcp->status.work_version, work_version);
        hdcp_stop_work_queue(hdcp);
    }

    /* update status */
    hdcp->status.cur_reauth_times = 0;
    hdcp->status.work_version = work_version;
    HDMI_INFO("work_version:%d\n", work_version);

    /* phy oe check */
    if (!hal_phy_is_on(hdcp->hdmi->phy)) {
        HDMI_WARN("fail,phy no signal\n");
        hdcp->status.err_code = HDCP_ERR_NO_SIGNAL;
        hdcp_notify_event(hdcp, HDCP_FAIL);
        goto unlock_exit;
    }

    /* start auth */
    hdcp->status.auth_start = true;
    hdcp->cur_delay_time = hdcp->start_delay_time;
    schedule_delayed_work(&hdcp->start_work, hdcp->start_delay_time);

unlock_exit:
    mutex_unlock(&hdcp->mutex);
}

void drv_hdmitx_hdcp_off(struct hdmi_hdcp *hdcp)
{
    if (hdcp == NULL) {
        return ;
    }
    mutex_lock(&hdcp->mutex);
    HDMI_INFO("hdcp off!\n");
    if (hdcp->status.auth_start) {
        hdcp_stop_work_queue(hdcp);
    }
    mutex_unlock(&hdcp->mutex);
}

void drv_hdmitx_hdcp_start_auth(struct hdmi_hdcp *hdcp, hi_u32 mode)
{
    if (hdcp == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }
    mutex_lock(&hdcp->mutex);
    hdcp->usr_start = true;
    hdcp->usr_mode = mode;
    mutex_unlock(&hdcp->mutex);
    HDMI_INFO("user start hdcp!\n");

    if (hdcp->hdmi->connector->status != HPD_PLUGOUT && hal_phy_is_on(hdcp->hdmi->phy)) {
        drv_hdmitx_hdcp_on(hdcp);
    }
}

void drv_hdmitx_hdcp_stop_auth(struct hdmi_hdcp *hdcp)
{
    if (hdcp == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    if (hdcp->usr_start) {
        drv_hdmitx_hdcp_off(hdcp);
    }

    mutex_lock(&hdcp->mutex);
    hdcp->status.auth_start = false;
    hdcp->status.auth_success = false;
    hdcp->status.cur_reauth_times = 0;
    hdcp->status.work_version = HDCP_VERSION_NONE;
    hdcp->status.err_code = HDCP_ERR_UNDO;

    hdcp->usr_start = false;
    hdcp->usr_mode = HDCP_MODE_BUTT;
    mutex_unlock(&hdcp->mutex);
    HDMI_INFO("user stop hdcp!\n");
}

void drv_hdmitx_hdcp_clear_cap(struct hdmi_hdcp *hdcp)
{
    if (hdcp == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }
    mutex_lock(&hdcp->mutex);
    hdcp->cap_sink.support_22 = false;
    hdcp->cap_sink.support_23 = false;
    mutex_unlock(&hdcp->mutex);
}

hi_s32 drv_hdmitx_hdcp_get_cap(struct hdmi_hdcp *hdcp, struct hdcp_cap *cap)
{
    if (hdcp == NULL || cap == NULL) {
        HDMI_ERR("null ptr\n");
        return -1;
    }
    mutex_lock(&hdcp->mutex);
    cap->support_1x = hdcp->cap_sink.support_1x && hdcp->cap_src.support_1x;
    cap->support_22 = hdcp->cap_sink.support_22 && hdcp->cap_src.support_22;
    cap->support_23 = hdcp->cap_sink.support_23 && hdcp->cap_src.support_23;
    mutex_unlock(&hdcp->mutex);

    return 0;
}

hi_s32 drv_hdmitx_hdcp_get_hw_cap(struct hdmi_hdcp *hdcp)
{
    if (hdcp == NULL) {
        HDMI_ERR("null ptr\n");
        return -1;
    }

    mutex_lock(&hdcp->mutex);
    hdcp->hal_2x_ops->get_sink_capability(hdcp->hdmi, &hdcp->cap_sink);
    hdcp->cap_sink.support_1x = true;
    HDMI_INFO("sink support 1x,22,23=%d,%d,%d\n",
        hdcp->cap_sink.support_1x, hdcp->cap_sink.support_22, hdcp->cap_sink.support_23);
    mutex_unlock(&hdcp->mutex);
    return 0;
}

void drv_hdmitx_hdcp_set_reauth_times(struct hdmi_hdcp *hdcp, hi_u32 reut_times)
{
    if (hdcp == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    mutex_lock(&hdcp->mutex);
    hdcp->usr_reauth_times = reut_times;
    HDMI_INFO("user set reauth times:%d\n", hdcp->usr_reauth_times);
    mutex_unlock(&hdcp->mutex);
}

hi_s32 drv_hdmitx_hdcp_get_user_status(struct hdmi_hdcp *hdcp, struct hdcp_usr_status *status)
{
    if (hdcp == NULL || status == NULL) {
        HDMI_ERR("null ptr\n");
        return -EINVAL;
    }
    mutex_lock(&hdcp->mutex);
    *status = hdcp->status;
    mutex_unlock(&hdcp->mutex);
    return 0;
}

hi_s32 drv_hdmitx_hdcp_get_hw_status(struct hdmi_hdcp *hdcp)
{
    if (hdcp == NULL) {
        HDMI_ERR("null ptr\n");
        return -EINVAL;
    }

    mutex_lock(&hdcp->mutex);
    HDMI_INFO("no support now\n");
    mutex_unlock(&hdcp->mutex);

    return 0;
}

hi_s32 drv_hdmitx_hdcp_loadkey(struct hdmi_hdcp *hdcp, hi_u32 key_version)
{
    if (hdcp == HI_NULL) {
        HDMI_ERR("null ptr\n");
        return -EINVAL;
    }

    mutex_lock(&hdcp->mutex);
    if (key_version == HDCP_VERSION_HDCP1X) {
        if (hdcp->hal_1x_ops->load_key(hdcp->hdmi)) {
            hdcp->load_1x_key = false;
            HDMI_ERR("loadkey crc fail\n");
            goto ret_fail;
        }

        hdcp->hal_1x_ops->get_a_ksv(hdcp->hdmi, hdcp->src_st_1x.a_ksv, HDCP1X_SIZE_5BYTES_KSV);
        if (!hdcp_1x_ksv_check(hdcp->src_st_1x.a_ksv)) {
            hdcp->load_1x_key = false;
            HDMI_ERR("aksv check fail\n");
            goto ret_fail;
        }
        hdcp->load_1x_key = true;
        HDMI_INFO("load hdcp1x key success\n");
    } else {
        HDMI_WARN("no support key version:%d now\n", key_version);
    }
    mutex_unlock(&hdcp->mutex);

    return HI_SUCCESS;

ret_fail:
    mutex_unlock(&hdcp->mutex);
    return HI_FAILURE;
}

/*
 * note : When plug out hdmi cable, mcu should stop in time.
 * Or it may sent message lead to unexpected resualt.So this function
 * should call in hotplug irq server, without sleep.
 */
hi_void drv_hdmitx_hdcp_plugout_handle(struct hdmi_hdcp *hdcp)
{
    if (hdcp == NULL) {
        return ;
    }
    hdcp->hal_1x_ops->set_encryption(hdcp->hdmi, false);
    hdcp->hal_2x_ops->set_encryption(hdcp->hdmi, false);
    hdcp->hal_2x_ops->set_mcu_start(hdcp->hdmi, false);
}

