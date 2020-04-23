/*------------------------------------------------------------------------------
  Copyright 2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/12/13
  Modification ID : e402a4dcb0e51a00c34b174334c082c1211eebf1
------------------------------------------------------------------------------*/
#include "sony_stdlib.h"
#include "sony_demod_isdbc_chbond.h"
#include "sony_demod_isdbc_monitor.h"

/*------------------------------------------------------------------------------
 Functions
------------------------------------------------------------------------------*/
sony_result_t sony_demod_isdbc_chbond_Enable (sony_demod_t * pDemod,
                                              uint8_t enable)
{
    uint8_t enableStreamIn = 0;
    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_Enable");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Set SLV-X Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /*
     * slave    Bank    Addr    Bit      default     Value          Name
     * --------------------------------------------------------------------------------
     * <SLV-X>   00h     C1h    [2:0]    8'h00       8'h0x      OREG_STREAMIN_EN
     *
     * Bit[0]: Stream input0 Bit[1]: Stream input1 Bit[2]: Stream input2
     * 1: Stream input enble 0:Stream input disable
     */
    if (enable) {
        enableStreamIn = pDemod->chbondStreamIn;
    }
    /* Set Stream Input */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVX, 0xC1, enableStreamIn) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* Set SLV-T Bank : 0x03 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (enable) {
        uint8_t data[2] = {0x01, 0x09};
        /* Channel Bonding initial setting */
        /* slave    Bank    Addr    Bit    default     Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   03h     17h     [0]      8'h00      8'h01      OREG_SEL_FRAMEVALID
         * <SLV-T>   03h     18h     [3]      8'h01      8'h09      OREG_IGNORE_STREAMTYPE
         */
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x17, data, 2) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }
    /* Bonding mode setting */
    /*
     * slave    Bank    Addr    Bit    default     Value          Name              meaning
     * --------------------------------------------------------------------------------
     * <SLV-T>   03h     b1h    [0]    8'h00       8'h0x      OREG_XCHBOND_ON       0: Channel Bonding Off 1: Channel Bonding On
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xB1, enable) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_demod_isdbc_chbond_SetTSMFConfig (sony_demod_t * pDemod,
                                                     sony_isdbc_tsid_type_t tsidType,
                                                     uint16_t tsid,
                                                     uint16_t networkId)
{
    uint8_t data[4];

    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_SetTSMFConfig");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONY_DEMOD_STATE_SLEEP) && (pDemod->state != SONY_DEMOD_STATE_ACTIVE)) {
        /* This api is accepted in Sleep and Active states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->chbondConfig != SONY_DEMOD_CHBOND_CONFIG_ISDBC) {
        /* For ISDB-C channel bonding only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if ((tsidType == SONY_ISDBC_TSID_TYPE_RELATIVE_TS_NUMBER) && (tsid > 0x0F || tsid == 0)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Set SLV-T Bank : 0x03 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (tsidType == SONY_ISDBC_TSID_TYPE_TSID) {
        /*
         * slave    Bank    Addr    Bit    default     Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   03h     10h    [7:0]     8'h00      8'hxx      OREG_TSMF_TS_ID[15:8]
         * <SLV-T>   03h     11h    [7:0]     8'h00      8'hxx      OREG_TSMF_TS_ID[7:0]
         * <SLV-T>   03h     12h    [7:0]     8'h00      8'hxx      OREG_TSMF_ORIG_NETWORK_ID[15:8]
         * <SLV-T>   03h     13h    [7:0]     8'h00      8'hxx      OREG_TSMF_ORIG_NETWORK_ID[7:0]
         *
         * OREG_TSMF_TS_ID[15:0]           : the TS ID of the desired TS
         * OREG_TSMF_ORIG_NETWORK_ID[15:0] : the Original network ID of the desired TS
         */

        data[0] = (uint8_t)((tsid >> 8) & 0xFF);
        data[1] = (uint8_t)(tsid & 0xFF);
        data[2] = (uint8_t)((networkId >> 8) & 0xFF);
        data[3] = (uint8_t)(networkId & 0xFF);
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x10, data, 4) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }

        /*
         * slave    Bank    Addr    Bit    default     Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   03h     3fh    [4:0]     8'h00      8'h00      OREG_TSMF_RELATIVE_TS_NUM_ON0,OREG_TSMF_RELATIVE_TS_NUM0[3:0]    from Ext0
         * <SLV-T>   03h     56h    [4:0]     8'h00      8'h00      OREG_TSMF_RELATIVE_TS_NUM_ON1,OREG_TSMF_RELATIVE_TS_NUM1[3:0]    from Ext1
         * <SLV-T>   03h     57h    [4:0]     8'h00      8'h00      OREG_TSMF_RELATIVE_TS_NUM_ON2,OREG_TSMF_RELATIVE_TS_NUM2[3:0]    from Ext2
         * <SLV-T>   03h     58h    [4:0]     8'h00      8'h00      OREG_TSMF_RELATIVE_TS_NUM_ON3,OREG_TSMF_RELATIVE_TS_NUM3[3:0]    from internal dmd
         *
         * OREG_TSMF_RELATIVE_TS_NUMx[3:0] : the Relative TS number(1~15) of the desired TS
         * OREG_TSMF_RELATIVE_TS_NUM_ONx :
         * 0: TS select by the TS ID and the original network ID,
         * 1: TS select by the relative TS number
         */
        data[0] = data[1] = data[2] = data[3] = 0x00;
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x3F, data[0]) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x56, &data[1], 3) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    } else {
        /*
         * slave    Bank    Addr    Bit    default     Value          Name
         * ----------------------------------------------------------------------------------
         * <SLV-T>   03h     3fh    [4:0]     8'h00      8'h1x      OREG_TSMF_RELATIVE_TS_NUM_ON0,OREG_TSMF_RELATIVE_TS_NUM0[3:0]    from Ext0
         * <SLV-T>   03h     56h    [4:0]     8'h00      8'h1x      OREG_TSMF_RELATIVE_TS_NUM_ON1,OREG_TSMF_RELATIVE_TS_NUM1[3:0]    from Ext1
         * <SLV-T>   03h     57h    [4:0]     8'h00      8'h1x      OREG_TSMF_RELATIVE_TS_NUM_ON2,OREG_TSMF_RELATIVE_TS_NUM2[3:0]    from Ext2
         * <SLV-T>   03h     58h    [4:0]     8'h00      8'h1x      OREG_TSMF_RELATIVE_TS_NUM_ON3,OREG_TSMF_RELATIVE_TS_NUM3[3:0]    from internal dmd
         *
         * OREG_TSMF_RELATIVE_TS_NUMx[3:0] : the Relative TS number(1~15) of the desired TS
         * OREG_TSMF_RELATIVE_TS_NUM_ONx :
         * 0: TS select by the TS ID and the original network ID,
         * 1: TS select by the relative TS number
         */
        data[0] = data[1] = data[2] = data[3] = 0x10 | (uint8_t)(tsid & 0x0F);
        if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x3F, data[0]) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
        if (pDemod->pI2c->WriteRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x56, &data[1], 3) != SONY_RESULT_OK) {
            SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
        }
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_demod_isdbc_chbond_SoftReset (sony_demod_t * pDemod)
{
    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_SoftReset");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if ((pDemod->state != SONY_DEMOD_STATE_ACTIVE) && (pDemod->state != SONY_DEMOD_STATE_SLEEP)) {
        /* This api is accepted in Sleep and Active states only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if (pDemod->chbondConfig != SONY_DEMOD_CHBOND_CONFIG_ISDBC) {
        /* For ISDB-C channel bonding only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x00 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x00) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /*
     * slave    Bank    Addr    Bit    default     Value          Name
     * --------------------------------------------------------------------------------
     * <SLV-T>   00h     FAh    [0]    8'h00       8'h01      OREG_SRST_LCHBOND
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0xFA, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_demod_isdbc_chbond_monitor_BondStat (sony_demod_t * pDemod,
                                                        sony_demod_isdbc_chbond_state_t * pState)
{
    uint8_t data;

    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_monitor_BondStat");

    if ((!pDemod) || (!pState)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pDemod->system != SONY_DTV_SYSTEM_ISDBC) || (pDemod->chbondConfig != SONY_DEMOD_CHBOND_CONFIG_ISDBC)) {
        /* Not ISDB-C or channel boding is not enabled */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x03 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /*
     * slave    Bank    Addr    Bit              Name                          meaning
     * -------------------------------------------------------------------------------------------------------------------------------
     * <SLV-T>   03h     20h     [6]        IREG_BONDOK                0: Bonding NG                 1: Bonding OK
     * <SLV-T>   03h     20h     [4]        IREG_GROUPID_CHK           0: different values or no valid carrier     1: same values
     * <SLV-T>   03h     20h     [3]        IREG_STREAMTYPE_CHK        0: different values or no valid carrier     1: same values
     * <SLV-T>   03h     20h     [1]        IREG_NUMCARRIERS_CHK       0: different values or no valid carrier     1: same values
     * <SLV-T>   03h     20h     [0]        IREG_CARRSEQ_CHK           0: duplication or no valid carrier          1: no duplication
     * <SLV-T>   03h     20h     [2]        IREG_CAL_NUMCARRIERS_CHK   0: different values or no valid carrier     1: same values
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x20, &data, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    pState->bondOK = (data >> 6) & 0x01;
    pState->groupIDCheck = (data >> 4) & 0x01;
    pState->streamTypeCheck = (data >> 3) & 0x01;
    pState->numCarriersCheck = (data >> 1) & 0x01;
    pState->carrierSeqCheck = data & 0x01;
    pState->calcNumCarriersCheck = (data >> 2) & 0x01;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_demod_isdbc_chbond_monitor_BondStat_hold (sony_demod_t * pDemod,
                                                             sony_demod_isdbc_chbond_state_t * pState)
{
    uint8_t data;

    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_monitor_BondStat_hold");

    if ((!pDemod) || (!pState)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pDemod->system != SONY_DTV_SYSTEM_ISDBC) && (pDemod->chbondConfig) != SONY_DEMOD_CHBOND_CONFIG_ISDBC) {
        /* Not ISDB-C and Channel Boding is not enable */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x03 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /*
     * slave    Bank    Addr    Bit              Name                          meaning
     * ----------------------------------------------------------------------------------------------------------------------------------
     * <SLV-T>   03h     22h     [6]        IREG_BOND_NG_HLD           0: Bonding OK              1: Bonding NG
     * <SLV-T>   03h     22h     [4]        IREG_GROUPID_NG_HLD        0: same values             1: different values or no valid carrier
     * <SLV-T>   03h     22h     [3]        IREG_STREAMTYPE_NG_HLD     0: same values             1: different values or no valid carrier
     * <SLV-T>   03h     22h     [1]        IREG_NUMCARR_NG_HLD        0: same values             1: different values or no valid carrier
     * <SLV-T>   03h     22h     [0]        IREG_CARRIER_SEQ_NG_HLD    0: no duplication           1: duplication or no valid carrier
     * <SLV-T>   03h     22h     [2]        IREG_CAL_NUMCARRIERS_NG_HLD  0: same values           1: different values or no valid carrier
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x22, &data, 1) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    data ^= 0x7F;
    pState->bondOK = ((data >> 6) & 0x01);
    pState->groupIDCheck = ((data >> 4) & 0x01);
    pState->streamTypeCheck = ((data >> 3) & 0x01);
    pState->numCarriersCheck = ((data >> 1) & 0x01);
    pState->carrierSeqCheck = (data & 0x01);
    pState->calcNumCarriersCheck = ((data >> 2) & 0x01);

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_demod_isdbc_chbond_ClearBondStat (sony_demod_t * pDemod)
{
    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_ClearBondStat");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pDemod->chbondConfig != SONY_DEMOD_CHBOND_CONFIG_ISDBC) {
        /* For ISDB-C channel bonding only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x03 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /*
     * slave    Bank    Addr    Bit    default     Value          Name
     * -----------------------------------------------------------------------------------------------
     * <SLV-T>   03h     1bh     [0]    1'b0        1'b1        OREG_CLR_BONDMON
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x1B, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_demod_isdbc_chbond_monitor_TLVConvError (sony_demod_t * pDemod,
                                                            uint8_t * pError)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data;
    sony_demod_isdbc_chbond_state_t chbondState;

    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_monitor_TLVConvError");

    if ((!pDemod) || (!pError)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pDemod->system != SONY_DTV_SYSTEM_ISDBC) && (pDemod->chbondConfig) != SONY_DEMOD_CHBOND_CONFIG_ISDBC) {
        /* Not ISDB-C and Channel Boding is not enable */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    result = sony_demod_isdbc_chbond_monitor_BondStat (pDemod, &chbondState);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (result);
    }

    if (!chbondState.bondOK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /* Set SLV-T Bank : 0x03 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /*
     * slave    Bank    Addr    Bit    default     Value          Name                                  meaning
     * -------------------------------------------------------------------------------------------------------------------------------
     * <SLV-T>   03h     16h    [0]     1'b1       1'bx      OREG_VAR_TLV_EN      0: TS or Fixed length TLV,  1: Variable length TLV
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x16, &data, 1) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (!(data & 0x01)) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }
    /*
     * slave    Bank    Addr    Bit              Name                   meaning
     * -----------------------------------------------------------------------------------------------
     * <SLV-T>   03h     24h     [4]        IREG_TLVERR_ALL_HLD         0: No TLV error  1: TLV error
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x24, &data, 1) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    SLVT_UnFreezeReg (pDemod);

    *pError = (data >> 4) & 0x01;
    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_demod_isdbc_chbond_ClearTLVConvError (sony_demod_t * pDemod)
{
    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_ClearTLVConvError");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pDemod->chbondConfig != SONY_DEMOD_CHBOND_CONFIG_ISDBC) {
        /* For ISDB-C channel bonding only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x03 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /*
     * slave    Bank    Addr    Bit    default     Value          Name
     * -----------------------------------------------------------------------------------------------
     * <SLV-T>   03h     19h     [0]    1'b0        1'b1        OREG_TLVCHK_RST
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x19, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_demod_isdbc_chbond_monitor_CarrierDelay (sony_demod_t * pDemod,
                                                            uint32_t * pDelay)
{
    uint8_t data[3];

    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_monitor_CarrierDelay");

    if ((!pDemod) || (!pDelay)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pDemod->system != SONY_DTV_SYSTEM_ISDBC) && (pDemod->chbondConfig) != SONY_DEMOD_CHBOND_CONFIG_ISDBC) {
        /* Not ISDB-C and Channel Boding is not enable */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x03 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /*
     * slave    Bank    Addr    Bit              Name
     * -----------------------------------------------------------------------------------------------
     * <SLV-T>   03h     1dh     [7:0]      IREG_MON_VALID,2'd0,IREG_FDELAY[20:16]
     * <SLV-T>   03h     1eh     [7:0]      IREG_FDELAY[15:8]
     * <SLV-T>   03h     1fh     [7:0]      IREG_FDELAY[7:0]
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x1D, data, 3) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    if (!(data[0] & 0x80)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    *pDelay = ((data[0] & 0x1F) << 16) | (data[1] << 8) | data[2];
    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_demod_isdbc_chbond_monitor_EWSChange (sony_demod_t * pDemod,
                                                         uint8_t * pEWSChange)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data;
    sony_demod_isdbc_chbond_state_t chbondState;

    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_monitor_EWSChange");

    if ((!pDemod) || (!pEWSChange)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pDemod->system != SONY_DTV_SYSTEM_ISDBC) && (pDemod->chbondConfig) != SONY_DEMOD_CHBOND_CONFIG_ISDBC) {
        /* Not ISDB-C and Channel Boding is not enable */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    result = sony_demod_isdbc_chbond_monitor_BondStat (pDemod, &chbondState);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (result);
    }

    if (!chbondState.bondOK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /* Set SLV-T Bank : 0x03 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /*
     * slave    Bank    Addr    Bit              Name             meaning
     * -----------------------------------------------------------------------------------------------
     * <SLV-T>   03h     52h     [4]        IREG_EWS0            from Ext0
     * <SLV-T>   03h     52h     [5]        IREG_EWS1            from Ext1
     * <SLV-T>   03h     52h     [6]        IREG_EWS2            from Ext2
     * <SLV-T>   03h     52h     [7]        IREG_EWS3            from internal dmd
     * IREG_EWSX 0:Emergancy boot control is not detected. 1:Emergancy boot control is detected
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x52, &data, 1) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    SLVT_UnFreezeReg (pDemod);

    *pEWSChange = (data >> 4) & 0x0F;
    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_demod_isdbc_chbond_ClearEWSChange (sony_demod_t * pDemod)
{
    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_ClearEWSChange");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pDemod->chbondConfig != SONY_DEMOD_CHBOND_CONFIG_ISDBC) {
        /* For ISDB-C channel bonding only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x03 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /*
     * slave    Bank    Addr    Bit    default     Value          Name
     * -----------------------------------------------------------------------------------------------
     * <SLV-T>   03h     53h     [0]    1'b0        1'b1        OREG_CLR_EWS
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x53, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_demod_isdbc_chbond_monitor_VersionChange (sony_demod_t * pDemod,
                                                             uint8_t * pVersionChange)
{
    sony_result_t result = SONY_RESULT_OK;
    uint8_t data;
    sony_demod_isdbc_chbond_state_t chbondState;

    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_monitor_VersionChange");

    if ((!pDemod) || (!pVersionChange)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pDemod->system != SONY_DTV_SYSTEM_ISDBC) && (pDemod->chbondConfig) != SONY_DEMOD_CHBOND_CONFIG_ISDBC) {
        /* Not ISDB-C and Channel Boding is not enable */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    result = sony_demod_isdbc_chbond_monitor_BondStat (pDemod, &chbondState);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (result);
    }

    if (!chbondState.bondOK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /* Set SLV-T Bank : 0x03 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /*
     * slave    Bank    Addr    Bit              Name             meaning
     * -----------------------------------------------------------------------------------------------
     * <SLV-T>   03h     52h     [0]        IREG_VERNUMCHG0      from Ext0
     * <SLV-T>   03h     52h     [1]        IREG_VERNUMCHG1      from Ext1
     * <SLV-T>   03h     52h     [2]        IREG_VERNUMCHG2      from Ext2
     * <SLV-T>   03h     52h     [3]        IREG_VERNUMCHG3      from internal dmd
     * IREG_VERNUMCHGx 0:TSMF version number change is not detected. 1:TSMF version number change is detected
     */
    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x52, &data, 1) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    SLVT_UnFreezeReg (pDemod);

    *pVersionChange = data & 0x0F;
    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_demod_isdbc_chbond_ClearVersionChange (sony_demod_t * pDemod)
{
    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_ClearVersionChange");

    if (!pDemod) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    if (pDemod->chbondConfig != SONY_DEMOD_CHBOND_CONFIG_ISDBC) {
        /* For ISDB-C channel bonding only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Set SLV-T Bank : 0x03 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /*
     * slave    Bank    Addr    Bit    default     Value          Name
     * -----------------------------------------------------------------------------------------------
     * <SLV-T>   03h     54h     [0]    1'b0        1'b1        OREG_CLR_VERNUMCHG
     */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x54, 0x01) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}


sony_result_t sony_demod_isdbc_chbond_monitor_StreamType (sony_demod_t * pDemod,
                                                          sony_isdbc_chbond_stream_type_t * pStreamType)
{
    sony_result_t result = SONY_RESULT_OK;
    sony_demod_isdbc_chbond_state_t chbondState;
    uint8_t data;

    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_monitor_StreamType");

    if ((!pDemod) || (!pStreamType)){
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    /* Software state check */
    if (pDemod->state != SONY_DEMOD_STATE_ACTIVE) {
        /* This api is accepted in Active state only */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    if ((pDemod->system != SONY_DTV_SYSTEM_ISDBC) && (pDemod->chbondConfig) != SONY_DEMOD_CHBOND_CONFIG_ISDBC) {
        /* Not ISDB-C and Channel Boding is not enable */
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_SW_STATE);
    }

    /* Freeze registers */
    if (SLVT_FreezeReg (pDemod) != SONY_RESULT_OK) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    result = sony_demod_isdbc_chbond_monitor_BondStat (pDemod, &chbondState);
    if (result != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (result);
    }

    if (!chbondState.bondOK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_HW_STATE);
    }

    /* Set SLV-T Bank : 0x03 */
    if (pDemod->pI2c->WriteOneRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x00, 0x03) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    /* slave    Bank    Addr    Bit              Name             meaning
     * ---------------------------------------------------------------------------------------------------------------------------------------
     * <SLV-T>   03h     25h     [6]        IREG_STREAMTYPE      0:TS output, 1:TLV output
     */

    if (pDemod->pI2c->ReadRegister (pDemod->pI2c, pDemod->i2cAddressSLVT, 0x25, &data, 1) != SONY_RESULT_OK) {
        SLVT_UnFreezeReg (pDemod);
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_I2C);
    }

    SLVT_UnFreezeReg (pDemod);

    if (data & 0x40) {
        *pStreamType = SONY_ISDBC_CHBOND_STREAM_TYPE_TLV;
    } else {
        *pStreamType = SONY_ISDBC_CHBOND_STREAM_TYPE_TS_NONE;
    }

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}

sony_result_t sony_demod_isdbc_chbond_monitor_TSMFHeaderExt (sony_demod_t * pDemod,
                                                             sony_isdbc_tsmf_header_t * pTSMFHeader,
                                                             sony_isdbc_chbond_tsmf_header_ext_t * pTSMFHeaderExt)
{
    int i;
    SONY_TRACE_ENTER ("sony_demod_isdbc_chbond_monitor_TSMFHeaderExt");

    if ((!pDemod) || (!pTSMFHeader) || (!pTSMFHeaderExt)) {
        SONY_TRACE_RETURN (SONY_RESULT_ERROR_ARG);
    }

    sony_memcpy(&pTSMFHeaderExt->aceewData[0], &pTSMFHeader->privateData[0], 26);
    for (i = 0; i < 8; i++) {
        pTSMFHeaderExt->streamType[i] = (sony_isdbc_chbond_stream_type_t)((pTSMFHeader->privateData[26] >> (7 - i)) & 0x01);
    }
    for (i = 0; i < 7; i++) {
        pTSMFHeaderExt->streamType[i + 8] = (sony_isdbc_chbond_stream_type_t)((pTSMFHeader->privateData[27] >> (7 - i)) & 0x01);
    }
    pTSMFHeaderExt->groupID = pTSMFHeader->privateData[28];
    pTSMFHeaderExt->numCarriers = pTSMFHeader->privateData[29];
    pTSMFHeaderExt->carrierSequence = pTSMFHeader->privateData[30];
    pTSMFHeaderExt->numFrames = (pTSMFHeader->privateData[31] >> 4) & 0x0F;
    pTSMFHeaderExt->framePosition = pTSMFHeader->privateData[31] & 0x0F;

    SONY_TRACE_RETURN (SONY_RESULT_OK);
}
