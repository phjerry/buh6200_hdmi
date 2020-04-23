/******************************************************************************
 *   $ Copyright $ Sony Corporation
 *-----------------------------------------------------------------------------
 *   File Name   : $File: sut_pj987_isdbt.c $
 *   Modified    : $Date: 2018/12/11 $ By $Author: $
 *   Revision    : $Revision: 1.0.0.0 $
 *   Description : SUT-PJ987 Tuner Control Sample Code
 *                 This code is not include I2C communication function.
 *                 It is necessary to add codes for the I2C communication
 *                 properly to execute and compile the code.
 *-----------------------------------------------------------------------------
 * This program may contain information about unfinished products and is subject
 * to change without notice.
 * Sony cannot assume responsibility for any problems arising out of the
 * use of the program
 *****************************************************************************/
/******************************************************************************
 *   includes:
 *    - system includes
 *    - application includes
 *****************************************************************************/
     
#include <linux/delay.h>

#include "sut_pj987_rssi.h"
#include "sut_pj987_i2c.h"

/*-----------------------------------------
 Add include files, if needed
-------------------------------------------*/

/******************************************************************************
 ******************************************************************************
 ******************************************************************************
 * type definition (struct / enum):
 ******************************************************************************
 ******************************************************************************
 *****************************************************************************/

typedef struct sony_helene_adjust_param_t {
    uint8_t OUTLMT;            /**< Addr:0x68 Bit[1:0] : Maximum IF output. (0: 1.5Vp-p, 1: 1.2Vp-p) */
    uint8_t RF_GAIN;           /**< Addr:0x69 Bit[6:4] : RFVGA gain. 0xFF means Auto. (RF_GAIN_SEL = 1) */
    uint8_t IF_BPF_GC;         /**< Addr:0x69 Bit[3:0] : IF_BPF gain. */
    uint8_t RFOVLD_DET_LV1_VL; /**< Addr:0x6B Bit[3:0] : RF overload RF input detect level. (FRF <= 172MHz) */
    uint8_t RFOVLD_DET_LV1_VH; /**< Addr:0x6B Bit[3:0] : RF overload RF input detect level. (172MHz < FRF <= 464MHz) */
    uint8_t RFOVLD_DET_LV1_U;  /**< Addr:0x6B Bit[3:0] : RF overload RF input detect level. (FRF > 464MHz) */
    uint8_t IFOVLD_DET_LV_VL;  /**< Addr:0x6C Bit[2:0] : Internal RFAGC detect level. (FRF <= 172MHz) */
    uint8_t IFOVLD_DET_LV_VH;  /**< Addr:0x6C Bit[2:0] : Internal RFAGC detect level. (172MHz < FRF <= 464MHz) */
    uint8_t IFOVLD_DET_LV_U;   /**< Addr:0x6C Bit[2:0] : Internal RFAGC detect level. (FRF > 464MHz) */
    uint8_t IF_BPF_F0;         /**< Addr:0x6D Bit[5:4] : IF filter center offset. */
    uint8_t BW;                /**< Addr:0x6D Bit[1:0] : 6MHzBW(0x00) or 7MHzBW(0x01) or 8MHzBW(0x02) or 1.7MHzBW(0x03) */
    uint8_t FIF_OFFSET;        /**< Addr:0x6E Bit[4:0] : 5bit signed. IF offset (kHz) = FIF_OFFSET x 50 */
    uint8_t BW_OFFSET;         /**< Addr:0x6F Bit[4:0] : 5bit signed. BW offset (kHz) = BW_OFFSET x 50 (BW_OFFSET x 10 in 1.7MHzBW) */
    uint8_t AGC_SEL;           /**< Addr:0x74 Bit[5:4] : AGC pin select. (0: AGC1, 1: AGC2) 0xFF means Auto (by config flags) */
    uint8_t IF_OUT_SEL;        /**< Addr:0x74 Bit[1:0] : IFOUT pin select. (0: IFOUT1, 1: IFOUT2) 0xFF means Auto. (by config flags) */
    uint8_t IS_LOWERLOCAL;     /**< Addr:0x9C Bit[0]   : Local polarity. (0: Upper Local, 1: Lower Local) */
} sony_helene_adjust_param_t;

/*************************************************
 Block ID definition for I2C access
**************************************************/

typedef enum {
    TERR_1ST_TUNER = 0,   /* Tag for 1st Tuner Block                      */
    TERR_2ND_TUNER,       /* Tag for 2nd Tuner Block for dual tuner model */
    TERR_3RD_TUNER,       /* Tag for 3rd Tuner Block for triple tuner model */
    TERR_4TH_TUNER        /* Tag for 4th Tuner Block for triple tuner model */
} BLOCKID;

typedef enum {
    PORT1 = 0,            /* use 1 port. use only Dualport model */
    PORT2                 /* use 2 port. use Single  & Dualport model */
} IFOUT_AGC_PORT;

typedef enum {
    TERR_ATT_ENABLE = 0,  /* use TERR_ATT Enable        */
    TERR_ATT_DISABLE      /* use TERR_ATT Disable       */
} TERR_ATT_STATUS;

typedef enum {
    SONY_HELENE_TV_SYSTEM_UNKNOWN =  0,
    SONY_HELENE_DTV_ISDBT_6,            /* ISDB-T 6MHzBW */
    SONY_TV_SYSTEM_NUM
} RECEIVE_MODE;

/*************************************************
 Number of demod setting
**************************************************/

/*************************************************
 Tuner Setting
**************************************************/
/* If it is necessary to use IF output limitation (1.2Vpp) */
/* please enable #define IFOUT_LIMITATION_1_2V            */
/* #define IFOUT_LIMITATION_1_2V */
#ifdef IFOUT_LIMITATION_1_2V
#define IFOUTLMT (0x01)
#else
#define IFOUTLMT (0x00)
#endif
/*********************************************
 If you modify Tuner status, please modify
 following parameter.
*********************************************/

#define AUTO         (0xFF)
#define OFFSET(ofs)  ((uint8_t)(ofs) & 0x1F)
#define BW_6         (0x00)
#define BW_7         (0x01)
#define BW_8         (0x02)
#define BW_1_7       (0x03)

/* Optimized table for sony module */
static const sony_helene_adjust_param_t g_param_table_sony_module[SONY_TV_SYSTEM_NUM] = {
    /*
      OUTLMT       IF_BPF_GC                                          BW              BW_OFFSET           IF_OUT_SEL
        |    RF_GAIN  |     RFOVLD_DET_LV1    IFOVLD_DET_LV  IF_BPF_F0 |   FIF_OFFSET     |        AGC_SEL   |  IS_LOWERLOCAL
        |       |     |    (VL)  (VH)  (U)   (VL)  (VH)  (U)    |      |       |          |          |       |     |          */
    {IFOUTLMT, AUTO, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, BW_6,  OFFSET(0),  OFFSET(0),  PORT2, PORT2, 0x00}, /**< SONY_ASCOT3_TV_SYSTEM_UNKNOWN */
    {IFOUTLMT, AUTO, 0x07, 0x0D, 0x0D, 0x0D, 0x03, 0x03, 0x03, 0x00, BW_6,  OFFSET(-9), OFFSET(-5), PORT2, PORT2, 0x00}, /**< SONY_HELENE_DTV_ISDBT_6   (ISDB-T 6MHzBW) */
};

static const int16_t demod_iftgt[SONY_TV_SYSTEM_NUM] = {
       0, /* SONY_ASCOT3_TV_SYSTEM_UNKNOWN */
    -450  /* SONY_HELENE_DTV_ISDBT_6       */    /* This params devend on reveiving demodulator. Please adjust to fit RSSI value. */
};

/*************************************************
 stdout function for debug
**************************************************/
/* #define DBG_PRINT_OUT */
#ifdef  DBG_PRINT_OUT
#define TUPD_PRINTF( LOG_NORMAL,...) printf(__VA_ARGS__)
#else
#define TUPD_PRINTF( LOG_NORMAL,...)
#endif

/******************************************************************************
 ******************************************************************************
 ******************************************************************************
 * constants & variable:
 *  - constant
 *  - global variable
 *  - static variable
 ******************************************************************************
 ******************************************************************************
 *****************************************************************************/


/******************************************************************************
 ******************************************************************************
 ******************************************************************************
 * function prototype:
 ******************************************************************************
 ******************************************************************************
 ******************************************************************************/

/*************************************************
 I2C access function
**************************************************/

static uint8_t reg_write_val(uint16_t id, uint8_t subadr, uint8_t val);
static uint8_t reg_read_val(uint16_t id, uint8_t subadr, uint8_t* val);
static uint8_t reg_write(uint16_t id, uint8_t subadr, uint8_t *data, uint8_t len);
static uint8_t reg_read(uint16_t id, uint8_t subadr, uint8_t *data, uint8_t len);


/*************************************************
 Wait
**************************************************/
static uint8_t waitms(uint32_t ms);

/*************************************************
 Tuning Sequece
**************************************************/
static void   tuner_init_setparam(uint32_t frequency , uint8_t *buf);

void isdbt_x_read_agc(uint16_t tuner_block_id, uint8_t* icoffset, uint8_t* ifagcreg , uint8_t* rfagcreg)
{

    uint8_t t_addr;
    uint8_t t_data[2];
    uint8_t data_length;
    uint8_t val;

    t_addr    = 0x87;
    t_data[0] = 0xC4;
    t_data[1] = 0x41;
    data_length = 2;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

	t_addr    = 0x17;
	t_data[0] = 0x7E;
	t_data[1] = 0x06;
	data_length = 2;
	reg_write( tuner_block_id, t_addr , t_data, data_length );
	waitms(1);
	reg_read_val ( tuner_block_id, 0x19, &val );
    *icoffset = (val & 0x0f);

    t_addr    = 0x59;
    t_data[0] = 0x05;
    t_data[1] = 0x01;
    data_length = 2;
    reg_write( tuner_block_id, t_addr , t_data, data_length );
    reg_read_val ( tuner_block_id, 0x5B, &val );
    *ifagcreg = val;

    t_addr    = 0x59;
    t_data[0] = 0x03;
    t_data[1] = 0x01;
    data_length = 2;
    reg_write( tuner_block_id, t_addr , t_data, data_length );
    reg_read_val ( tuner_block_id, 0x5B, &val );
    *rfagcreg = val;

    reg_write_val( tuner_block_id, 0x59, 0x04 );
    reg_write_val( tuner_block_id, 0x88, 0x00 );
    reg_write_val( tuner_block_id, 0x87, 0xC0 );

    return;

}

int16_t isdbt_tuner_calcRSSI(uint16_t tuner_block_id, uint8_t icoffset , uint8_t ifagcreg , uint8_t rfagcreg)
{

    uint8_t i;
    int32_t ifgain, rfgain, icgain, moduleoffset;
    int32_t rssi_x100;
    int32_t if_bpf_gc_x100;
    int32_t rfgainmax_x100;
    int32_t agcreg_x140;

    sony_ModuleParam* table;
    uint8_t lna;
    uint8_t bpf;
	uint8_t tmp[3];
	uint32_t freq;

    uint8_t size;

    static const int16_t  BPFGAIN[] = {-6,-4,-2, 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 20, 20};

    static const sony_ModuleParam RFGAIN_MAX_TERR[] = {
		{  950000, 4300},
		{  900000, 4480},
		{  868000, 4600},
		{  766000, 4630},
		{  664000, 4630},
		{  600000, 4580},
		{  532000, 4530},
		{  464000, 4450},
		{  400000, 4750},
		{  350000, 4750},
		{  320000, 4740},
		{  250000, 4840},
		{  225000, 4770},
		{  200000, 4680},
		{  172000, 4580},
		{  165000, 4610},
		{  142000, 4770},
		{  125000, 4890},
		{   86000, 4960},
		{   65000, 4920},
		{   55000, 4800},
		{       0, 4690},
    };

    reg_read( tuner_block_id, 0x70, tmp, 3  );
    freq  = (uint32_t)((tmp[0] & 0xff) <<  0)
          + (uint32_t)((tmp[1] & 0xff) <<  8)
          + (uint32_t)((tmp[2] & 0x0f) << 16);

    reg_read ( tuner_block_id, 0x69, &bpf , 1);
    reg_read ( TERR_1ST_TUNER, 0x85, &lna , 1);

    agcreg_x140 = ifagcreg * 140;
    if_bpf_gc_x100 = BPFGAIN[ ( bpf & 0x0F ) ] * 100;

    if(agcreg_x140 > 10200){
        ifgain = 815 + if_bpf_gc_x100;
    }else if(agcreg_x140 > 7650){
        ifgain = 1517 + if_bpf_gc_x100 - (700 * (agcreg_x140 - 7650) + 1275) / 2550; /* Round */
    }else{
        ifgain = 3856 + if_bpf_gc_x100 - (7797 * agcreg_x140 + 12750) / 25500; /* Round */
    }

    i = 0;
    while( freq < RFGAIN_MAX_TERR[i].freq ){
        i++;
    }
    rfgainmax_x100 = RFGAIN_MAX_TERR[i].param;

    if(ifagcreg > rfagcreg){
        agcreg_x140 = ifagcreg * 140;
    } else {
        agcreg_x140 = rfagcreg * 140;
    }

    if(agcreg_x140 < 3825){
        rfgain = rfgainmax_x100;
    }else if(agcreg_x140 < 6375){
        rfgain = rfgainmax_x100        - (  160 * (agcreg_x140 -  3825) + 12750) / 25500; /* Round */
    }else if(agcreg_x140 < 7650){
        rfgain = rfgainmax_x100 -   16 - ( 1340 * (agcreg_x140 -  6375) + 12750) / 25500; /* Round */
    }else if(agcreg_x140 < 8925){
        rfgain = rfgainmax_x100 -   83 - ( 3440 * (agcreg_x140 -  7650) + 12750) / 25500; /* Round */
    }else if(agcreg_x140 < 10200){
        rfgain = rfgainmax_x100 -  258 - ( 5421 * (agcreg_x140 -  8925) + 12750) / 25500; /* Round */
    }else if(agcreg_x140 < 11475){
        rfgain = rfgainmax_x100 -  529 - ( 7451 * (agcreg_x140 - 10200) + 12750) / 25500; /* Round */
    }else if(agcreg_x140 < 15300){
        rfgain = rfgainmax_x100 -  902 - ( 8253 * (agcreg_x140 - 11475) + 12750) / 25500; /* Round */
    }else if(agcreg_x140 < 19125){
        rfgain = rfgainmax_x100 - 2139 - ( 6979 * (agcreg_x140 - 15300) + 12750) / 25500; /* Round */
    }else if(agcreg_x140 < 24225){
        rfgain = rfgainmax_x100 - 3186 - ( 7468 * (agcreg_x140 - 19125) + 12750) / 25500; /* Round */
    }else if(agcreg_x140 < 26775){
        rfgain = rfgainmax_x100 - 4680 - ( 5674 * (agcreg_x140 - 24225) + 12750) / 25500; /* Round */
    }else if(agcreg_x140 < 29325){
        rfgain = rfgainmax_x100 - 5247 - (14592 * (agcreg_x140 - 26775) + 12750) / 25500; /* Round */
    }else if(agcreg_x140 < 31875){
        rfgain = rfgainmax_x100 - 6717 - (16676 * (agcreg_x140 - 29325) + 12750) / 25500; /* Round */
    }else{
        rfgain = rfgainmax_x100 - 8384;
    }

    moduleoffset = 0;
    table = (sony_ModuleParam*)DEFAULT;
    size = sizeof(DEFAULT) / sizeof(sony_ModuleParam);

   	if(lna){
		if (tuner_block_id == TERR_2ND_TUNER) {
             table = (sony_ModuleParam*)PJ987_TERR_2ND_LNAON_RSSI_TABLE;
             size = sizeof(PJ987_TERR_2ND_LNAON_RSSI_TABLE) / sizeof(sony_ModuleParam);
        }
        else if (tuner_block_id == TERR_3RD_TUNER) {
             table = (sony_ModuleParam*)PJ987_TERR_3RD_LNAON_RSSI_TABLE;
             size = sizeof(PJ987_TERR_3RD_LNAON_RSSI_TABLE) / sizeof(sony_ModuleParam);
        }
		else if (tuner_block_id == TERR_4TH_TUNER) {
             table = (sony_ModuleParam*)PJ987_TERR_4TH_LNAON_RSSI_TABLE;
             size = sizeof(PJ987_TERR_4TH_LNAON_RSSI_TABLE) / sizeof(sony_ModuleParam);
        }
    }else{
        if (tuner_block_id == TERR_2ND_TUNER) {
            table = (sony_ModuleParam*)PJ987_TERR_2ND_LNAOFF_RSSI_TABLE;
            size = sizeof(PJ987_TERR_2ND_LNAOFF_RSSI_TABLE) / sizeof(sony_ModuleParam);
        }
        else if (tuner_block_id == TERR_3RD_TUNER) {
            table = (sony_ModuleParam*)PJ987_TERR_3RD_LNAOFF_RSSI_TABLE;
            size = sizeof(PJ987_TERR_3RD_LNAOFF_RSSI_TABLE) / sizeof(sony_ModuleParam);
        }
        else if (tuner_block_id == TERR_4TH_TUNER) {
            table = (sony_ModuleParam*)PJ987_TERR_4TH_LNAOFF_RSSI_TABLE;
            size = sizeof(PJ987_TERR_4TH_LNAOFF_RSSI_TABLE) / sizeof(sony_ModuleParam);
        }
    }
    
    i = 0;
    while( freq < table[i].freq ){
        if(i >= size - 1 ) break;
        i++;
    }
    moduleoffset = table[i].param;

    icgain = (icoffset >= 8) ? ((icoffset - 16) * 100) : (icoffset * 100);

    rssi_x100 = - ifgain - rfgain - icgain + moduleoffset;

    return (int16_t)rssi_x100;

}

/******************************************************************************
 * Register Setting (raleted HELENE/HELENE2P Application Note basically.)
 *****************************************************************************/

/******************************************************************************
 * x_pon
 *-----------------------------------------------------------------------------
 * Input        : none
 * Output       : none
 * Return       : none
 *****************************************************************************/
void isdbt_x_pon(uint16_t tuner_block_id)
{

    uint8_t t_addr;
    uint8_t t_data[16];
    uint8_t data_length;

    TUPD_PRINTF( LOG_NORMAL,"Enter x_pon()...\n");

    t_addr     = 0x99;
    t_data[0]  = 0x7A;
    t_data[1]  = 0x01;
    data_length = 2;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    t_addr     = 0x81;
    t_data[0]  = 0x18; // 0x81
    t_data[1]  = ( tuner_block_id == TERR_1ST_TUNER ) ? 0x84 : 0x00;
    t_data[2]  = ( tuner_block_id == TERR_1ST_TUNER ) ? 0xA4 : 0x00;
    t_data[3]  = ( tuner_block_id == TERR_1ST_TUNER ) ? 0x80 : 0x00;
    t_data[4]  = ( tuner_block_id == TERR_1ST_TUNER ) ? 0x01 : 0x00;
    t_data[5]  = 0x10;
    t_data[6]  = 0xC4;
    t_data[7]  = 0x40;
    t_data[8]  = 0x10; // 0x89
    t_data[9]  = 0x00;
    t_data[10] = 0x45;
    t_data[11] = 0x75;
    t_data[12] = 0x07; // 0x8d
    t_data[13] = 0x08;
    t_data[14] = 0x00;
    t_data[15] = 0x00;
    data_length = 16;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    t_addr     = 0x91;
    t_data[0]  = 0x10; // 0x91
    t_data[1]  = 0x20;
    t_data[2]  = 0x0A;
    t_data[3]  = 0x00;
    data_length = 4;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    reg_write_val( tuner_block_id, 0x9B, 0x00 );

    waitms(10);

    reg_write_val( tuner_block_id, 0x74, 0x12 );
    reg_write_val( tuner_block_id, 0x67, 0x00 );
    t_addr    = 0x5E;
    t_data[0] = 0x15;
    t_data[1] = 0x00;
    t_data[2] = 0x00;
    data_length = 3;
    reg_write( tuner_block_id, t_addr , t_data, data_length );
    reg_write_val( tuner_block_id, 0x88, 0x00 );
    reg_write_val( tuner_block_id, 0x87, 0xC0 );

    reg_write_val( tuner_block_id, 0x80, 0x01 );

    TUPD_PRINTF( LOG_NORMAL," Done. \n");

    return;
}


/******************************************************************************
 * x_tune
 *-----------------------------------------------------------------------------
 * Input        : TuningParam* pParam
 * Output       : none
 * Return       : none
 * Description  : based on A3 Application Note x_tune()
 *****************************************************************************/
void isdbt_x_tune(uint16_t tuner_block_id, uint32_t frequency)
{
    uint8_t t_addr;
    uint8_t t_data[8];
    uint8_t data_length;
    uint8_t param[28];

    TUPD_PRINTF( LOG_NORMAL,"Enter x_tune()... \n");

    tuner_init_setparam(frequency , param );

    t_addr     = 0x74;
    t_data[0]  = 0x12;     // 0x74
    t_data[1]  = param[0];
    t_data[2]  = param[1];
    t_data[3]  = param[2];
    t_data[4]  = param[3]; // 0x78
    data_length = 5;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    reg_write_val( tuner_block_id, 0x8B, param[26] );

    t_addr     = 0x8D;
    t_data[0]  = param[27];
    t_data[1]  = param[25];
    data_length = 2;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    reg_write_val( tuner_block_id, 0x79, 0xA1 );
    reg_write_val( tuner_block_id, 0x7D, 0x00 );

    t_addr     = 0x91;
    t_data[0]  = param[23];
    t_data[1]  = param[24];
    data_length = 2;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    t_addr     = 0x9C;
    t_data[0]  = param[4];
    t_data[1]  = param[5];
    data_length = 2;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    t_addr     = 0x87;
    t_data[0]  = 0xC4; // 0x87
    t_data[1]  = 0x40;
    data_length = 2;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    t_addr     = 0x5E;
    t_data[0]  = 0xEE;
    t_data[1]  = 0x02;
    t_data[2]  = 0x9E;
    data_length = 3;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    t_addr     = 0x61;
    t_data[0]  = param[6];
    t_data[1]  = param[7];   // 0x62
    t_data[2]  = param[8];
    t_data[3]  = param[9];
    t_data[4]  = param[10];
    t_data[5]  = param[11];  // 0x66
    t_data[6]  = param[12];
    data_length = 7;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    t_addr     = 0x68;
    t_data[0]  = param[13];  // 0x68
    t_data[1]  = param[22];
    t_data[2]  = param[14];
    t_data[3]  = param[15];
    t_data[4]  = param[16];  // 0x6c
    t_data[5]  = param[17];
    t_data[6]  = param[18];
    t_data[7]  = param[19];
    data_length = 8;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    t_addr     = 0x70;
    t_data[0]  = (uint8_t)((frequency & 0x0000ff));
    t_data[1]  = (uint8_t)((frequency & 0x00ff00) >>  8);
    t_data[2]  = (uint8_t)((frequency & 0x0f0000) >> 16);
    t_data[3]  = param[20];
    t_data[4]  = param[21];
    data_length = 5;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    TUPD_PRINTF( LOG_NORMAL," Done. \n");

    return;
}

/******************************************************************************
 * x_tune_end
 *-----------------------------------------------------------------------------
 * Input        : none
 * Output       : none
 * Return       : none
 * Description  : A3 Application Note x_tune_end(), sequence number 8-10.
 *****************************************************************************/

void isdbt_x_tune_end(uint16_t tuner_block_id)
{

    TUPD_PRINTF( LOG_NORMAL,"Enter x_tune_end()... \n");

    reg_write_val( tuner_block_id, 0x88, 0x00 );
    reg_write_val( tuner_block_id, 0x87, 0xC0 );

    TUPD_PRINTF( LOG_NORMAL," Done. \n");

    return;
}

/******************************************************************************
 * x_fin()
 *-----------------------------------------------------------------------------
 * Input        : none
 * Output       : none
 * Return       : none
 *****************************************************************************/
void isdbt_x_fin(uint16_t tuner_block_id)
{
    uint8_t t_addr;
    uint8_t t_data[3];
    uint8_t data_length;

    TUPD_PRINTF( LOG_NORMAL,"Enter x_fin()...\n");

    reg_write_val( tuner_block_id, 0x74, 0x12 );
    reg_write_val( tuner_block_id, 0x67, 0x00 );

    t_addr    = 0x5E;
    t_data[0] = 0x15;
    t_data[1] = 0x00;
    t_data[2] = 0x00;
    data_length = 3;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    reg_write_val( tuner_block_id, 0x88, 0x00 );
    reg_write_val( tuner_block_id, 0x87, 0xC0 );

    TUPD_PRINTF( LOG_NORMAL," Done. \n");

    return;
}

/******************************************************************************
 * tuner_gpio
 *-----------------------------------------------------------------------------
 * Input        : uint8_t value
 * Output       : none
 * Return       : none
 * Description  : control GPIO.
 *****************************************************************************/
void isdbt_tuner_gpio(uint16_t tuner_block_id, uint8_t val)
{

    TUPD_PRINTF( LOG_NORMAL,"Enter tuner_gpio()...\n");

    reg_write_val( tuner_block_id, 0x85, val );

    TUPD_PRINTF( LOG_NORMAL," Done. \n");

    return;
}


/******************************************************************************
 * tuner_init_setparam
 *-----------------------------------------------------------------------------
 * Input        : RECEIVE_MODE m_mode
 * Output       : none
 * Return       : uint8_t *param
 * Description  : select RF Data
 *****************************************************************************/
static void tuner_init_setparam(uint32_t frequency , uint8_t *buf)
{

    const sony_helene_adjust_param_t *p;
    uint8_t i;
    uint8_t tmp;

    p = &g_param_table_sony_module[SONY_HELENE_DTV_ISDBT_6];


    /* Register 0x75-0x78 */
    buf[0]  = 0xF9;
    buf[1]  = 0x0F;
    buf[2]  = 0x25;
    buf[3]  = 0x44;

    /* Register 0x9c-0x9d */
    buf[4]  = 0x90;
    buf[5]  = 0x00;

    /* Register 0x61-0x68 */
    buf[6]  = 0x67;
    buf[7]  = 0x03;
    buf[8]  = 0x38;
    buf[9]  = 0x1E;
    buf[10] = 0x02;
    buf[11] = 0x24;
    buf[12] = 0x00;
    buf[13] = p->OUTLMT;

    /* Register 0x6a-0x6f */
    buf[14] = 0x00;
    tmp     = 0x30;
    if( frequency <= 172000 ){
        buf[15] = p->RFOVLD_DET_LV1_VL;
        buf[16] = tmp + p->IFOVLD_DET_LV_VL;
    }else if ( frequency <= 464000 ){
        buf[15] = p->RFOVLD_DET_LV1_VH;
        buf[16] = tmp + p->IFOVLD_DET_LV_VH;
    }else{
        buf[15] = p->RFOVLD_DET_LV1_U;
        buf[16] = tmp + p->IFOVLD_DET_LV_U;
    }
    buf[17] = ((p->IF_BPF_F0) << 4) + p->BW;
    buf[18] = p->FIF_OFFSET;
    buf[19] = p->BW_OFFSET;

    /* Register 0x73-0x74 */
    buf[20] = 0xFF;
    buf[21] = ((p->AGC_SEL) << 4) + p->IF_OUT_SEL;

    /* Register 0x69 */
    if(p->RF_GAIN == AUTO) buf[22] = 0x80                + p->IF_BPF_GC;
    else                   buf[22] = ((p->RF_GAIN) << 4) + p->IF_BPF_GC;

    /* Register 0x91-0x92 */
    buf[23] = 0x0A;
    buf[24] = 0x0F;

    /* Register 0x8E */
    buf[25] = 0x08;

    /* Register 0x8B */
    buf[26] = 0x45;

    /* Register 0x8D */
    buf[27] = 0x00;

    TUPD_PRINTF( LOG_NORMAL,"\n------reg  setting------\n");
    TUPD_PRINTF( LOG_NORMAL,"    ");
    for(i= 0;i<= 3;i++) TUPD_PRINTF( LOG_NORMAL,"0x%x ",buf[i]); TUPD_PRINTF( LOG_NORMAL,"\n    ");
    for(i=23;i<=26;i++) TUPD_PRINTF( LOG_NORMAL,"0x%x ",buf[i]); TUPD_PRINTF( LOG_NORMAL,"\n    ");
    for(i= 4;i<= 5;i++) TUPD_PRINTF( LOG_NORMAL,"0x%x ",buf[i]); TUPD_PRINTF( LOG_NORMAL,"\n    ");
    for(i= 6;i<=13;i++) TUPD_PRINTF( LOG_NORMAL,"0x%x ",buf[i]); TUPD_PRINTF( LOG_NORMAL,"\n    ");
    for(i=22;i<=22;i++) TUPD_PRINTF( LOG_NORMAL,"0x%x ",buf[i]);
    for(i=14;i<=19;i++) TUPD_PRINTF( LOG_NORMAL,"0x%x ",buf[i]); TUPD_PRINTF( LOG_NORMAL,"\n    ");
    for(i=20;i<=21;i++) TUPD_PRINTF( LOG_NORMAL,"0x%x ",buf[i]); TUPD_PRINTF( LOG_NORMAL,"\n");
    TUPD_PRINTF( LOG_NORMAL,"\n------------------------\n");

    return ;

}

/******************************************************************************
 * reg_write_val
 *-----------------------------------------------------------------------------
 * int16_t    id      : Block ID (Slave Addr information etc)
 * uint8_t    subadr  : sub address
 * uint8_t    val     : data value
 *
 * retrun           : write data to i2c access (use value as Input).
 *****************************************************************************/
static uint8_t reg_write_val(uint16_t id, uint8_t subadr, uint8_t val){
    reg_write(id, subadr, &val, 1);
    return 0;
}


/******************************************************************************
 * reg_write
 *-----------------------------------------------------------------------------
 * int16_t    id      : Block ID
 * uint8_t    subadr  : sub address
 * uint8_t*   data    : pointer to data buffer to send
 * uint8_t    len     : data byte length to send
 *
 * retrun           : write data to i2c access (use pointer as Input).
 *****************************************************************************/
static uint8_t reg_write(uint16_t id, uint8_t subadr, uint8_t *data, uint8_t len){
    return sut_pj987_reg_write(id, subadr, data, len);
}

/******************************************************************************
 * reg_read_val
 *-----------------------------------------------------------------------------
 * int16_t    id      : Block ID (Slave Addr information etc)
 * uint8_t    subadr  : sub address
 * uint8_t    val     : data value
 *
 * retrun           : write data to i2c access (use value as Input).
 *****************************************************************************/
static uint8_t reg_read_val(uint16_t id, uint8_t subadr, uint8_t* val){
    reg_read(id, subadr, val, 1);
    return 0;
}

/******************************************************************************
 * reg_read
 *-----------------------------------------------------------------------------
 * int16_t    id      : Block ID
 * uint8_t    subadr  : sub address
 * uint8_t*   data    : pointer to data buffer to send
 * uint8_t    len     : data byte length to send
 *
 * retrun           : write data to i2c access (use pointer as Input).
 *****************************************************************************/
static uint8_t reg_read(uint16_t id, uint8_t subadr, uint8_t *data, uint8_t len){
    return sut_pj987_reg_write(id, subadr, data, len);
}


/******************************************************************************
 * waitms
 *-----------------------------------------------------------------------------
 * Input : uint32_t ms
 * Output: none
 * Return: void
 *****************************************************************************/
static uint8_t waitms(uint32_t ms) {

        /*------------------------------------------------
            Add code for wait ( milliseconds unit )
        ------------------------------------------------*/
    msleep(ms);
    return 0;
}

/******************************************************************************
 * History
 *
 * 2018-09-17 v 0.1.0.0 initial
 * 2018-12-11 v 1.0.0.0 Added RSSI function
 ******************************************************************************/
