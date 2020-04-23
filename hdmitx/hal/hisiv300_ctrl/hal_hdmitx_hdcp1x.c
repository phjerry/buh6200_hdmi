/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: source file of hdcp1.x hal layer
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-12-11
 */

#include "hal_hdmitx_hdcp1x.h"
#include "hal_hdmitx_hdcp_reg.h"
#include "hal_hdmitx_ctrl_reg.h"
#include "hi_osal.h"
#include "hal_hdmitx_io.h"
#include "drv_hdmitx.h"

#define HDCP1X_KEY_CHECK_BIT_CNT 20
#define HDCP1X_KEY_CHECK_TIMEOUT 5

#define REG_HDCP1X_BYTE3_M (0xff << 24)
#define REG_HDCP1X_BYTE2_M (0xff << 16)
#define REG_HDCP1X_BYTE1_M (0xff << 8)
#define REG_HDCP1X_BYTE0_M (0xff << 0)

/*
 * New value can be safely written into this register only after getting command done bit (reg. 0x0F9[0])
 * or waiting at least 0.125s.
 * That means that new value can be safely written into this register only after previous command has been completed.
 * Otherwise; it is possible to get either self-authentication #1 error status bit or to get BIST done bit
 * without actually performing one of the new enabled BIST test.
 */
enum {
    HDCP1X_KEY_CMD_CLR               = 0x00, /* b00000: Clear command register; no action will be taken */
    HDCP1X_KEY_CMD_ALL_BIST_ENABLE   = 0x03, /* 5'bxxx11: will enable all BIST: CRC, self-authentication #1 & #2 */
    HDCP1X_KEY_CMD_CRC_ENABLE        = 0x04, /* 5'bxx100: will enable individual CRC */
    HDCP1X_KEY_CMD_SELF_AUTH1_ENABLE = 0x05, /* 5'bx1x00: will enable individual self-authentication #1 */
    HDCP1X_KEY_CMD_SELF_AUTH2_ENABLE = 0x06, /* 5'b1xx00: will enable individual self_authentication #2 */
};

static hi_bool hdcp1x_hw_crc_finish_check(struct hisilicon_hdmi *hdmi)
{
    hi_s32 err = 0;
    hi_bool ret = HI_FALSE;

    if (hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_CMD_DONE_M)) {
        hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_CMD_DONE_M, HI_TRUE);
        err += hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_BIST0_ERR_M);
        err += hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_BIST1_ERR_M);
        err += hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_BIST2_ERR_M);
        hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_KEY_CMD_M, HDCP1X_KEY_CMD_CLR);
        ret = err ? HI_FALSE : HI_TRUE;
    } else {
        ret = HI_FALSE;
    }

    return ret;
}

static hi_void hdcp1x_hw_get_an(struct hisilicon_hdmi *hdmi, hi_u8 *an_buf, hi_u32 size)
{
    hi_u32 an0, an1;
    hi_u32 i;

    an0 = hdmi_readl(hdmi->hdmi_regs, REG_HDCP1X_ENG_GEN_AN0);
    an1 = hdmi_readl(hdmi->hdmi_regs, REG_HDCP1X_ENG_GEN_AN1);

    for (i = 0; i < 4; i++) { /* lower 4 bytes */
        an_buf[i] = an0 & 0xff;
        an0 = an0 >> 8 ; /* shift 8 bits */
    }
    for (; i < size; i++) {
        an_buf[i] = an1 & 0xff;
        an1 = an1 >> 8 ; /* shift 8 bits */
    }
}

static hi_void hdcp1x_set_mode(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == HI_NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP_FUN_SEL, REG_HDCP_FUN_SEL_M, 0); /* 0-hdcp1.x, 1-hdcp2.x */
}

static hi_s32 hdcp1x_load_key(struct hisilicon_hdmi *hdmi)
{
    hi_u32 timeout = 0;

    if (hdmi == HI_NULL) {
        HDMI_ERR("null ptr\n");
        return HI_FAILURE;
    }

    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_CMD_DONE_M, HI_TRUE);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_KEY_LOAD_M, HI_TRUE);

    for (timeout = 0; (timeout < HDCP1X_KEY_CHECK_TIMEOUT); timeout++) {
        if (hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_CMD_DONE_M)) {
            hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_CMD_DONE_M, HI_TRUE);
            break;
        }
        osal_msleep(1);
    }

    if (timeout >= HDCP1X_KEY_CHECK_TIMEOUT) {
        HDMI_ERR("hdcp1x loadkey  err!\n");
        return HI_FAILURE;
    }

    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_BIST0_ERR_M, HI_TRUE);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_BIST1_ERR_M, HI_TRUE);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_BIST2_ERR_M, HI_TRUE);

    /* CRC check type select */
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_KEY_CMD_M, HDCP1X_KEY_CMD_CRC_ENABLE);
    /* check enable */
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_MEM_CTRL, REG_HDCP1X_KEY_CMD_VALID_M, HI_TRUE);

    timeout = 0;
    while (!hdcp1x_hw_crc_finish_check(hdmi) && (timeout++ < HDCP1X_KEY_CHECK_TIMEOUT)) {
        osal_msleep(1);
    }
    if (timeout >= HDCP1X_KEY_CHECK_TIMEOUT) {
        HDMI_ERR("hdcp14 crc err!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void hdcp1x_gerarate_an(struct hisilicon_hdmi *hdmi, hi_u8 *an_buf, hi_u32 size)
{
    if ((hdmi == HI_NULL) || (an_buf == HI_NULL) || (size != HDCP1X_SIZE_8BYTES_AN)) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_ENG_CTRL, REG_HDCP1X_AN_STOP_M, HI_FALSE);
    osal_msleep(1);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_ENG_CTRL, REG_HDCP1X_AN_STOP_M, HI_TRUE);
    osal_msleep(1);

    hdcp1x_hw_get_an(hdmi, an_buf, size);
    return ;
}

static hi_void hdcp1x_set_b_ksv(struct hisilicon_hdmi *hdmi, hi_u8 *bksv, hi_u32 size)
{
    if ((hdmi == HI_NULL) || (bksv == HI_NULL) || (size != HDCP1X_SIZE_5BYTES_KSV)) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_ENG_BKSV0, REG_HDCP1X_KEY_BKSV_BYTE0_M, *bksv++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_ENG_BKSV0, REG_HDCP1X_KEY_BKSV_BYTE1_M, *bksv++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_ENG_BKSV0, REG_HDCP1X_KEY_BKSV_BYTE2_M, *bksv++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_ENG_BKSV0, REG_HDCP1X_KEY_BKSV_BYTE3_M, *bksv++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_ENG_BKSV1, REG_HDCP1X_KEY_BKSV_BYTE4_M, *bksv++);

    if (hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_ENG_STATUS, REG_HDCP1X_RI_RDY_M)) {
        HDMI_ERR("set bksv fail!\n");
    }

    return ;
}

static hi_void hdcp1x_get_a_ksv(struct hisilicon_hdmi *hdmi, hi_u8 *aksv, hi_u32 size)
{
    hi_u8 *tmp = HI_NULL;

    if ((hdmi == HI_NULL) || (aksv == HI_NULL) || (size != HDCP1X_SIZE_5BYTES_KSV)) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    tmp = aksv;
    *tmp++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_KEY_AKSV0, REG_HDCP1X_KEY_AKSV_BYTE0_M);
    *tmp++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_KEY_AKSV0, REG_HDCP1X_KEY_AKSV_BYTE1_M);
    *tmp++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_KEY_AKSV0, REG_HDCP1X_KEY_AKSV_BYTE2_M);
    *tmp++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_KEY_AKSV0, REG_HDCP1X_KEY_AKSV_BYTE3_M);
    *tmp++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_KEY_AKSV1, REG_HDCP1X_KEY_AKSV_BYTE4_M);

    return ;
}

static hi_void hdcp1x_get_a_r0(struct hisilicon_hdmi *hdmi, hi_u8 *r0, hi_u32 size)
{
    if ((hdmi == HI_NULL) || (r0 == HI_NULL) || (size != HDCP1X_SIZE_2BYTES_RI)) {
        HDMI_ERR("null ptr\n");
        return ;
    }

    /* Ri */
    if (!hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_ENG_STATUS, REG_HDCP1X_RI_RDY_M)) {
        HDMI_ERR("hdcp14 hdcp1x_ri_rdy=0\n");
    }

    *r0++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_ENG_RI, REG_HDCP1X_ENG_RI0_M);
    *r0 = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_ENG_RI, REG_HDCP1X_ENG_RI1_M);
    return ;
}

static hi_void hdcp1x_set_b_status(struct hisilicon_hdmi *hdmi, hi_u16 bstatus)
{
    if (hdmi == HI_NULL) {
        HDMI_ERR("null ptr\n");
        return;
    }
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_BSTATUS, REG_HDCP1X_BSTATUS_M, bstatus);
}

static hi_void hdcp1x_set_repeater(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    if (hdmi == HI_NULL) {
        HDMI_ERR("null ptr\n");
        return;
    }
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_SEC_CFG, REG_HDCP1X_SEC_RPT_ON_M, enable);
}

static hi_void hdcp1x_set_encryption(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    if (hdmi == HI_NULL) {
        HDMI_ERR("null ptr\n");
        return;
    }
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_SEC_CFG, REG_HDCP1X_SEC_ENC_EN_M, enable);
}

static hi_void hdcp1x_get_a_vi(struct hisilicon_hdmi *hdmi, hi_u8 *a_vi, hi_u32 size)
{
    if ((hdmi == HI_NULL) || (a_vi == HI_NULL)) {
        HDMI_ERR("null ptr\n");
        return;
    }
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V0, REG_HDCP1X_BYTE0_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V0, REG_HDCP1X_BYTE1_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V0, REG_HDCP1X_BYTE2_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V0, REG_HDCP1X_BYTE3_M);

    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V1, REG_HDCP1X_BYTE0_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V1, REG_HDCP1X_BYTE1_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V1, REG_HDCP1X_BYTE2_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V1, REG_HDCP1X_BYTE3_M);

    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V2, REG_HDCP1X_BYTE0_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V2, REG_HDCP1X_BYTE1_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V2, REG_HDCP1X_BYTE2_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V2, REG_HDCP1X_BYTE3_M);

    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V3, REG_HDCP1X_BYTE0_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V3, REG_HDCP1X_BYTE1_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V3, REG_HDCP1X_BYTE2_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V3, REG_HDCP1X_BYTE3_M);

    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V4, REG_HDCP1X_BYTE0_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V4, REG_HDCP1X_BYTE1_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V4, REG_HDCP1X_BYTE2_M);
    *a_vi++ = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_V4, REG_HDCP1X_BYTE3_M);
}

static hi_void hdcp1x_set_b_ksv_list(struct hisilicon_hdmi *hdmi, hi_u8 *bksv_list, hi_u32 size)
{
    hi_u32 i;

    if ((hdmi == HI_NULL) || (bksv_list == HI_NULL)) {
        HDMI_ERR("null ptr\n");
        return;
    }
    for (i = 0; i < size; i++) {
        hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_KLIST, REG_HDCP1X_RPT_KLIST_M, bksv_list[i]);
    }
}

hi_bool hdcp1x_is_sha_ready(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == HI_NULL) {
        HDMI_ERR("null ptr\n");
        return HI_FALSE;
    }

    return hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_CTRL, REG_HDCP1X_SHA_READY_M);
}

static hi_void hdcp1x_set_no_downstream(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    if (hdmi == HI_NULL) {
        HDMI_ERR("null ptr\n");
        return;
    }
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_CTRL, REG_HDCP1X_SHA_NO_DS_M, enable);
}

static hi_void hdcp1x_set_sha_calculate(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    if (hdmi == HI_NULL) {
        HDMI_ERR("null ptr\n");
        return;
    }
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_CTRL, REG_HDCP1X_SHA_START, enable);
}

static hi_void hdcp1x_set_b_vi(struct hisilicon_hdmi *hdmi, hi_u8 *v_bi, hi_u32 size)
{
    if ((hdmi == HI_NULL) || (v_bi == HI_NULL) || (size != HDCP1X_SIZE_20BYTES_VI)) {
        HDMI_ERR("null ptr\n");
        return;
    }

    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V0, REG_HDCP1X_BYTE0_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V0, REG_HDCP1X_BYTE1_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V0, REG_HDCP1X_BYTE2_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V0, REG_HDCP1X_BYTE3_M, *v_bi++);

    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V1, REG_HDCP1X_BYTE0_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V1, REG_HDCP1X_BYTE1_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V1, REG_HDCP1X_BYTE2_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V1, REG_HDCP1X_BYTE3_M, *v_bi++);

    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V2, REG_HDCP1X_BYTE0_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V2, REG_HDCP1X_BYTE1_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V2, REG_HDCP1X_BYTE2_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V2, REG_HDCP1X_BYTE3_M, *v_bi++);

    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V3, REG_HDCP1X_BYTE0_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V3, REG_HDCP1X_BYTE1_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V3, REG_HDCP1X_BYTE2_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V3, REG_HDCP1X_BYTE3_M, *v_bi++);

    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V4, REG_HDCP1X_BYTE0_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V4, REG_HDCP1X_BYTE1_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V4, REG_HDCP1X_BYTE2_M, *v_bi++);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_RPT_V4, REG_HDCP1X_BYTE3_M, *v_bi++);

    /* enable logic to cmp Vi & Vi' */
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_CHECK, REG_HDCP1X_SHA_CHECK_M, HI_TRUE);
}

hi_bool hdcp1x_is_vi_valid_ok(struct hisilicon_hdmi *hdmi)
{
    if (hdmi == HI_NULL) {
        HDMI_ERR("null ptr\n");
        return HI_FALSE;
    }

    if ((!hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_RESULT, REG_HDCP1X_SHA_INVALID_M)) &&
        hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_RESULT, REG_HDCP1X_SHA_OK_M)) {
        return HI_TRUE;
    }
    return HI_FALSE;
}

static hi_void hdcp1x_clear_ri_err(struct hisilicon_hdmi *hdmi)
{
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_CHK_CTRL, REG_HDCP1X_CHECK_ERRCNT_EN_M, false);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_CHK_CTRL, REG_HDCP1X_CHECK_ERRCNT_EN_M, true);
}

static hi_void hdcp1x_set_ri_auto_validate(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    if (hdmi == HI_NULL) {
        HDMI_ERR("null ptr\n");
        return;
    }
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_CHK_CTRL, REG_HDCP1X_AUTO_CHECK_EN_M, enable);
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP1X_CHK_CTRL, REG_HDCP1X_CHECK_ERRCNT_EN_M, enable);
}

static hi_void hdcp1x_get_ri_validate_result(struct hisilicon_hdmi *hdmi, struct hdcp1x_ri_result *result)
{
    if ((hdmi == HI_NULL) || (result == HI_NULL)) {
        HDMI_ERR("null ptr\n");
        return;
    }

    result->cnt_nodone_err = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_CHK_ERR, REG_HDCP1X_NODONE_ERR_CNT_M);
    result->cnt_notchg_err = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_CHK_ERR, REG_HDCP1X_NOTCHG_ERR_CNT_M);;
    result->cnt_000frm_err = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_CHK_ERR, REG_HDCP1X_000FRM_ERR_CNT_M);;
    result->cnt_127frm_err = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_CHK_ERR, REG_HDCP1X_127FRM_ERR_CNT_M);;
    result->fsm_state = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_CHK_STATE, REG_HDCP1X_FSM_STATE_M);
}

static hi_void hdcp1x_set_1x_srst(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    if (hdmi == HI_NULL) {
        HDMI_ERR("null ptr\n");
        return;
    }
    hdmi_write_bits(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_HDCP1X_SRST_REQ_M, enable);
}

static hi_void hdcp1x_get_hw_status(struct hisilicon_hdmi *hdmi, struct hdcp1x_src_status *status)
{
    if ((hdmi == HI_NULL) || (status == HI_NULL)) {
        HDMI_ERR("null ptr\n");
        return;
    }
    hdcp1x_hw_get_an(hdmi, status->an, sizeof(status->an));
    hdcp1x_get_a_ksv(hdmi, status->a_ksv, sizeof(status->a_ksv));
    hdcp1x_get_a_r0(hdmi, status->a_r0, sizeof(status->a_r0));
    hdcp1x_get_a_vi(hdmi, status->a_vi, sizeof(status->a_vi));
    status->hdcp1x_mode = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP_FUN_SEL, REG_HDCP_FUN_SEL_M);
    status->repeater_en = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SEC_CFG, REG_HDCP1X_SEC_RPT_ON_M);
    status->encrypiton_en = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SEC_CFG, REG_HDCP1X_SEC_ENC_EN_M);
    status->sha_ready = hdcp1x_is_sha_ready(hdmi);
    status->sha_en = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_SHA_CTRL, REG_HDCP1X_SHA_START);
    status->rsrt_en = hdmi_read_bits(hdmi->hdmi_regs, REG_TX_PWD_RST_CTRL, REG_TX_HDCP1X_SRST_REQ_M);
    status->ri_validate_en = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP1X_CHK_CTRL, REG_HDCP1X_AUTO_CHECK_EN_M);
    hdcp1x_get_ri_validate_result(hdmi, &status->ri_result);
}

static hi_void hdcp1x_get_irq_status(struct hisilicon_hdmi *hdmi, hi_u32 *irq_status)
{
    hi_u32 tmp;
    if (hdmi == HI_NULL || irq_status == NULL) {
        HDMI_ERR("null ptr\n");
        return ;
    }
    /* HDCP1X_IRQ_RI1_128FRM */
    tmp = hdmi_read_bits(hdmi->hdmi_regs, REG_HDCP_INTR_STATE, REG_HDCP_INTR_STATE0_M);
    *irq_status = !!tmp;
}

static hi_void hdcp1x_clear_irq_status(struct hisilicon_hdmi *hdmi, hi_u32 status)
{
    if (hdmi == HI_NULL || status == 0) {
        HDMI_WARN("null ptr or status is 0\n");
        return;
    }

    /* HDCP1X_IRQ_MAIN */
    hdmi_write_bits(hdmi->hdmi_regs, REG_PWD_SUB_INTR_STATE, REG_TXHDCP_INTR_STATE_M, HI_TRUE);
    /* HDCP1X_IRQ_RI1_128FRM */
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP_INTR_STATE, REG_HDCP_INTR_STATE0_M, HI_TRUE);
    /* HDCP1X_IRQ_127FRM_ERR */
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP_INTR_STATE, REG_HDCP_INTR_STATE2_M, HI_TRUE);
    /* HDCP1X_IRQ_000FRM_ERR */
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP_INTR_STATE, REG_HDCP_INTR_STATE3_M, HI_TRUE);
    /* HDCP1X_IRQ_NOTCHG_ERR */
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP_INTR_STATE, REG_HDCP_INTR_STATE4_M, HI_TRUE);
    /* HDCP1X_IRQ_NODONE_ERR */
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP_INTR_STATE, REG_HDCP_INTR_STATE5_M, HI_TRUE);
}

static hi_void hdcp1x_set_irq(struct hisilicon_hdmi *hdmi, hi_bool enable)
{
    if (hdmi == HI_NULL) {
        HDMI_ERR("null ptr\n");
        return;
    }

    /* HDCP1X_IRQ_MAIN */
    hdmi_write_bits(hdmi->hdmi_regs, REG_PWD_SUB_INTR_MASK, REG_TXHDCP_INTR_MASK_M, enable);
    /* HDCP1X_IRQ_RI1_128FRM */
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP_INTR_MASK, REG_HDCP_INTR_MASK0_M, enable);
    /* HDCP1X_IRQ_127FRM_ERR */
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP_INTR_MASK, REG_HDCP_INTR_MASK2_M, false);
    /* HDCP1X_IRQ_000FRM_ERR */
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP_INTR_MASK, REG_HDCP_INTR_MASK3_M, false);
    /* HDCP1X_IRQ_NOTCHG_ERR */
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP_INTR_MASK, REG_HDCP_INTR_MASK4_M, false);
    /* HDCP1X_IRQ_NODONE_ERR */
    hdmi_write_bits(hdmi->hdmi_regs, REG_HDCP_INTR_MASK, REG_HDCP_INTR_MASK5_M, false);
}

static struct hdcp1x_hal_ops g_hal_hdcp1x_ops = {
    .set_mode = hdcp1x_set_mode,
    .load_key = hdcp1x_load_key,
    .gerarate_an = hdcp1x_gerarate_an,
    .set_b_ksv = hdcp1x_set_b_ksv,
    .get_a_ksv = hdcp1x_get_a_ksv,
    .get_a_r0 = hdcp1x_get_a_r0,
    .set_b_status = hdcp1x_set_b_status,
    .set_repeater = hdcp1x_set_repeater,
    .set_encryption = hdcp1x_set_encryption,
    .get_a_vi = hdcp1x_get_a_vi,
    .set_b_ksv_list = hdcp1x_set_b_ksv_list,
    .is_sha_ready = hdcp1x_is_sha_ready,
    .set_no_downstream = hdcp1x_set_no_downstream,
    .set_sha_calculate = hdcp1x_set_sha_calculate,
    .set_b_vi = hdcp1x_set_b_vi,
    .is_vi_valid_ok = hdcp1x_is_vi_valid_ok,
    .clear_ri_err = hdcp1x_clear_ri_err,
    .set_ri_auto_validate = hdcp1x_set_ri_auto_validate,
    .get_ri_validate_result = hdcp1x_get_ri_validate_result,
    .set_1x_srst = hdcp1x_set_1x_srst,
    .get_irq_status = hdcp1x_get_irq_status,
    .clear_irq_status = hdcp1x_clear_irq_status,
    .set_irq = hdcp1x_set_irq,
    .get_hw_status = hdcp1x_get_hw_status,
};

struct hdcp1x_hal_ops *hal_hdmitx_hdcp1x_get_ops(hi_void)
{
    return &g_hal_hdcp1x_ops;
}

