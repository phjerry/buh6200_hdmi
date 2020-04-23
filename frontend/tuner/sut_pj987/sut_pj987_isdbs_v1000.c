/******************************************************************************
 *   $ Copyright $ Sony Corporation
 *-----------------------------------------------------------------------------
 *   File Name   : $File: sut_pj987_isdbs.c $
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
 
#include "sut_pj987_isdbs_v1000.h"

#include <linux/delay.h>

#include "sut_pj987_rssi.h"
#include "sut_pj987_i2c.h"

/*************************************************
 Block ID definition for I2C access
**************************************************/
typedef enum {
    SAT_1ST_TUNER = 0, /* Tag for 1st Tuner Block            */
    SAT_2ND_TUNER,     /* Tag for 2nd Tuner Block            */
    SAT_3RD_TUNER,     /* Tag for 3rd Tuner Block            */
    SAT_4TH_TUNER      /* Tag for 4th Tuner Block            */
} BLOCKID;

/*************************************************
 Tuner Setting
**************************************************/
/* If it is necessary to use IQ output limitation (1.2Vpp) */
/* please enable #define IQOUT_LIMITATION_1_2V             */
/* #define IQOUT_LIMITATION_1_2V */
#ifdef IQOUT_LIMITATION_1_2V
#define IQOUTLMT (0x01)
#else
#define IQOUTLMT (0x00)
#endif

/*************************************************
 stdout function for debug
**************************************************/
/* #define DBG_PRINT_OUT */
#ifdef  DBG_PRINT_OUT
#define TUPD_PRINTF( LOG_NORMAL,...) printf(__VA_ARGS__)
#else
#define TUPD_PRINTF( LOG_NORMAL,...)
#endif

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

void isdbs_x_read_agc(uint16_t tuner_block_id, uint8_t* icoffset, uint8_t* ifagcreg , uint8_t* rfagcreg){

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
    *icoffset = (val >> 4);

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

int16_t isdbs_tuner_calcRSSI(uint16_t tuner_block_id, uint8_t icoffset , uint8_t ifagcreg , uint8_t rfagcreg){

    uint8_t i;
    int32_t ifgain, rfgain, icgain, moduleoffset;
    int32_t rssi_x100;
    sony_ModuleParam* table;
    uint8_t lna;
	uint8_t tmp[3];
	uint32_t freq;
    int32_t rfgainmax_x100;
    int32_t agcreg_x140;
    uint8_t size;

    static const sony_ModuleParam RFGAIN_MAX_SAT_LNAON[] = {
		{ 3300000, 4900},
		{ 3250000, 4900},
		{ 3200000, 4930},
		{ 3050000, 4960},
		{ 2950000, 4990},
		{ 2850000, 5020},
		{ 2750000, 5090},
		{ 2650000, 5130},
		{ 2430000, 5160},
		{ 2200000, 5230},
		{ 2150000, 5230},
		{ 2100000, 4160},
		{ 2050000, 4220},
		{ 2000000, 4270},
		{ 1950000, 4320},
		{ 1900000, 4360},
		{ 1800000, 4390},
		{ 1700000, 4440},
		{ 1600000, 4480},
		{ 1500000, 4500},
		{ 1300000, 4520},
		{ 1100000, 4490},
		{ 1000000, 4460},
		{  950000, 4430},
		{  900000, 4450},
                                         };

    static const sony_ModuleParam RFGAIN_MAX_SAT_LNAOFF[] = {
		{ 3300000, 3130},
		{ 3250000, 3130},
		{ 3200000, 3170},
		{ 3050000, 3220},
		{ 2950000, 3250},
		{ 2850000, 3290},
		{ 2750000, 3350},
		{ 2650000, 3370},
		{ 2430000, 3390},
		{ 2200000, 3500},
		{ 2150000, 3550},
		{ 2100000, 3600},
		{ 2050000, 3650},
		{ 2000000, 3700},
		{ 1950000, 3760},
		{ 1900000, 3790},
		{ 1800000, 3840},
		{ 1700000, 3900},
		{ 1600000, 3950},
		{ 1500000, 3990},
		{ 1300000, 4030},
		{ 1100000, 4010},
		{ 1000000, 3980},
		{  950000, 3950},
		{  900000, 3990},
                                         };

    reg_read( tuner_block_id, 0x10, tmp, 3  );
    freq  = (uint32_t)((tmp[0] & 0xff) <<  2)
          + (uint32_t)((tmp[1] & 0xff) << 10)
          + (uint32_t)((tmp[2] & 0x0f) << 18);

    agcreg_x140 = ifagcreg * 140;

    if(freq <= 2150000) {
    	reg_read ( tuner_block_id, 0x0C, &lna , 1);
    	lna &= 0x01;
    }else{
    	reg_read ( tuner_block_id, 0x0D, &lna , 1);
    	lna = (lna & 0x10) >> 4;
    }

    if(lna){
        if(agcreg_x140 > 10200){
            ifgain = 1558;
        }else if(agcreg_x140 > 7650){
            ifgain = 2285 - (7271 * (agcreg_x140 - 7650) + 12750) / 25500; /* Round */
        }else if(agcreg_x140 > 2550){
            ifgain = 3768 - (7415 * (agcreg_x140 - 2550) + 12750) / 25500; /* Round */
        }else{
            ifgain = 4574 - (8060 * (agcreg_x140) + 12750) / 25500; /* Round */
        }
    }else if(freq <= 2150000){
        if(agcreg_x140 > 10200){
            ifgain = 1438;
        }else if(agcreg_x140 > 7650){
            ifgain = 2165 - (7271 * (agcreg_x140 - 7650) + 12750) / 25500; /* Round */
        }else if(agcreg_x140 > 2550){
            ifgain = 3768 - (7415 * (agcreg_x140 - 2550) + 12750) / 25500; /* Round */
        }else{
            ifgain = 4574 - (8060 * (agcreg_x140) + 12750) / 25500; /* Round */
        }
    }else{
        if(agcreg_x140 > 9435){
            ifgain = 1776;
        }else if(agcreg_x140 > 7650){
            ifgain = 2285 - (7271 * (agcreg_x140 - 7650) + 12750) / 25500; /* Round */
        }else if(agcreg_x140 > 2550){
            ifgain = 3768 - (7415 * (agcreg_x140 - 2550) + 12750) / 25500; /* Round */
        }else{
            ifgain = 4574 - (8060 * (agcreg_x140) + 12750) / 25500; /* Round */
        }
    }

    if(lna){
        i = 0;
        while( freq < RFGAIN_MAX_SAT_LNAON[i].freq ){
            i++;
        }
        rfgainmax_x100 = RFGAIN_MAX_SAT_LNAON[i].param;

		if(freq > 2150000){
			if(agcreg_x140 < 5100){
				rfgain = rfgainmax_x100 -        (   15 * (agcreg_x140)         + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 6375){
				rfgain = rfgainmax_x100 -    3 - ( 3500 * (agcreg_x140 -  5100) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 7650){
				rfgain = rfgainmax_x100 -  178 - ( 7080 * (agcreg_x140 -  6375) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 9690){
				rfgain = rfgainmax_x100 -  532 - (10130 * (agcreg_x140 -  7650) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 11475){
				rfgain = rfgainmax_x100 - 1342 - ( 8000 * (agcreg_x140 -  9690) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 12750){
				rfgain = rfgainmax_x100 - 1902 - (11620 * (agcreg_x140 - 11475) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 14025){
				rfgain = rfgainmax_x100 - 2438 - (14822 * (agcreg_x140 - 12750) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 15300){
				rfgain = rfgainmax_x100 - 3224 - ( 8482 * (agcreg_x140 - 14025) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 16575){
				rfgain = rfgainmax_x100 - 3648 - (10950 * (agcreg_x140 - 15300) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 19125){
				rfgain = rfgainmax_x100 - 4196 - ( 7160 * (agcreg_x140 - 16575) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 20910){
				rfgain = rfgainmax_x100 - 4912 - ( 4472 * (agcreg_x140 - 19125) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 22185){
				rfgain = rfgainmax_x100 - 5225 - ( 7182 * (agcreg_x140 - 20910) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 28050){
				rfgain = rfgainmax_x100 - 5584 - ( 9900 * (agcreg_x140 - 22185) + 12750) / 25500; /* Round */
			}else{
				rfgain = rfgainmax_x100 - 7861;
			}
		} else {
		    if(agcreg_x140 < 2550){
				rfgain = rfgainmax_x100 -        (  190 * (agcreg_x140)         + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 5100){
				rfgain = rfgainmax_x100 -   19 - (  910 * (agcreg_x140 -  2550) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 6375){
				rfgain = rfgainmax_x100 -  110 - ( 5300 * (agcreg_x140 -  5100) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 7650){
				rfgain = rfgainmax_x100 -  375 - ( 9480 * (agcreg_x140 -  6375) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 8925){
				rfgain = rfgainmax_x100 -  849 - (12040 * (agcreg_x140 -  7650) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 10200){
				rfgain = rfgainmax_x100 - 1451 - ( 9700 * (agcreg_x140 -  8925) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 11475){
				rfgain = rfgainmax_x100 - 1936 - ( 9000 * (agcreg_x140 - 10200) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 12750){
				rfgain = rfgainmax_x100 - 2386 - (11962 * (agcreg_x140 - 11475) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 14025){
				rfgain = rfgainmax_x100 - 2986 - (10188 * (agcreg_x140 - 12750) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 20400){
				rfgain = rfgainmax_x100 - 3496 - ( 9948 * (agcreg_x140 - 14025) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 21675){
				rfgain = rfgainmax_x100 - 5983 - (15007 * (agcreg_x140 - 20400) + 12750) / 25500; /* Round */
			}else if(agcreg_x140 < 24225){
				rfgain = rfgainmax_x100 - 6764 - ( 9660 * (agcreg_x140 - 21675) + 12750) / 25500; /* Round */
			}else{
				rfgain = rfgainmax_x100 - 7730;
			}
		}
    }else{
        i = 0;
        while( freq < RFGAIN_MAX_SAT_LNAOFF[i].freq ){
            i++;
        }
        rfgainmax_x100 = RFGAIN_MAX_SAT_LNAOFF[i].param;

		if(freq > 2150000){
            if(agcreg_x140 <  3825){
                rfgain = rfgainmax_x100        - (  145 * (agcreg_x140)         + 12750) / 25500; /* Round */
            }else if(agcreg_x140 <  5100){
                rfgain = rfgainmax_x100 -   22 - ( 1640 * (agcreg_x140 -  3825) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 <  6375){
                rfgain = rfgainmax_x100 -  104 - ( 5360 * (agcreg_x140 -  5100) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 7650){
                rfgain = rfgainmax_x100 -  372 - ( 6220 * (agcreg_x140 -  6375) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 <  8925){
                rfgain = rfgainmax_x100 -  683 - ( 9050 * (agcreg_x140 -  7650) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 12750){
                rfgain = rfgainmax_x100 - 1140 - ( 6770 * (agcreg_x140 -  8925) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 14025){
                rfgain = rfgainmax_x100 - 2150 - ( 5730 * (agcreg_x140 - 12750) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 15300){
                rfgain = rfgainmax_x100 - 2440 - ( 6100 * (agcreg_x140 - 14025) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 17850){
                rfgain = rfgainmax_x100 - 2740 - ( 8480 * (agcreg_x140 - 15300) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 19125){
                rfgain = rfgainmax_x100 - 3590 - ( 9470 * (agcreg_x140 - 17850) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 21675){
                rfgain = rfgainmax_x100 - 4060 - ( 6260 * (agcreg_x140 - 19125) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 25500){
                rfgain = rfgainmax_x100 - 4690 - ( 9580 * (agcreg_x140 - 21675) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 28050){
                rfgain = rfgainmax_x100 - 6130 - ( 8120 * (agcreg_x140 - 25500) + 12750) / 25500; /* Round */
            }else{
                rfgain = rfgainmax_x100 - 6940;
            }
		} else {
            if(agcreg_x140 < 5100){
                rfgain = rfgainmax_x100        - (  145 * (agcreg_x140)         + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 6375){
                rfgain = rfgainmax_x100 -   29 - ( 3500 * (agcreg_x140 -  5100) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 7650){
                rfgain = rfgainmax_x100 -  195 - ( 5040 * (agcreg_x140 -  6375) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 8925){
                rfgain = rfgainmax_x100 -  447 - ( 9050 * (agcreg_x140 -  7650) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 10200){
                rfgain = rfgainmax_x100 -  900 - ( 6498 * (agcreg_x140 -  8925) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 11475){
                rfgain = rfgainmax_x100 - 1224 - ( 8798 * (agcreg_x140 - 10200) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 12750){
                rfgain = rfgainmax_x100 - 1664 - ( 9240 * (agcreg_x140 - 11475) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 14025){
                rfgain = rfgainmax_x100 - 2126 - (10744 * (agcreg_x140 - 12750) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 16575){
                rfgain = rfgainmax_x100 - 2663 - ( 8320 * (agcreg_x140 - 14025) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 17850){
                rfgain = rfgainmax_x100 - 3496 - ( 8040 * (agcreg_x140 - 16575) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 21675){
                rfgain = rfgainmax_x100 - 3898 - ( 7088 * (agcreg_x140 - 17850) + 12750) / 25500; /* Round */
            }else if(agcreg_x140 < 24225){
                rfgain = rfgainmax_x100 - 4961 - ( 9036 * (agcreg_x140 - 21675) + 12750) / 25500; /* Round */
            }else{
                rfgain = rfgainmax_x100 - 5864;
            }
		}
    }

    moduleoffset = 0;
    table = (sony_ModuleParam*)DEFAULT;
    size = sizeof(DEFAULT) / sizeof(sony_ModuleParam);

    if(tuner_block_id == SAT_1ST_TUNER) {
        table = (sony_ModuleParam*)PJ987_SAT_1ST_RSSI_TABLE;
        size = sizeof(PJ987_SAT_1ST_RSSI_TABLE) / sizeof(sony_ModuleParam);
    }
    if(tuner_block_id == SAT_2ND_TUNER) {
        table = (sony_ModuleParam*)PJ987_SAT_2ND_RSSI_TABLE;
        size = sizeof(PJ987_SAT_2ND_RSSI_TABLE) / sizeof(sony_ModuleParam);
    }
    if(tuner_block_id == SAT_3RD_TUNER) {
        table = (sony_ModuleParam*)PJ987_SAT_3RD_RSSI_TABLE;
        size = sizeof(PJ987_SAT_3RD_RSSI_TABLE) / sizeof(sony_ModuleParam);
    }
    if(tuner_block_id == SAT_4TH_TUNER) {
        table = (sony_ModuleParam*)PJ987_SAT_4TH_RSSI_TABLE;
        size = sizeof(PJ987_SAT_4TH_RSSI_TABLE) / sizeof(sony_ModuleParam);
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
void isdbs_x_pon(uint16_t tuner_block_id)
{
    uint8_t t_addr;
    uint8_t t_data[3];
    uint8_t data_length;

	TUPD_PRINTF( LOG_NORMAL,"Enter x_pon()...\n");

    t_addr    = 0x41;
    t_data[0] = 0x07;
    t_data[1] = 0x00;
    t_data[2] = 0x05;
    data_length = 3;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    t_addr    = 0x45;
	t_data[0] = 0x01;
	t_data[1] = 0x00;
	data_length = 2;
	reg_write( tuner_block_id, t_addr , t_data, data_length );

	t_addr    = 0x0C;
	t_data[0] = 0x14;
    t_data[1] = 0x00;
    data_length = 2;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    t_addr    = 0x7B;
    t_data[0] = 0x02;
    t_data[1] = 0x01;
    data_length = 2;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    TUPD_PRINTF( LOG_NORMAL," Done. \n");
    return;
}


/******************************************************************************
 * x_tune
 *-----------------------------------------------------------------------------
 * Input        : frequency
 * Output       : none
 * Return       : none
 * Description  :
 *****************************************************************************/
void isdbs_x_tune(uint16_t tuner_block_id, uint32_t frequency, RECEIVE_MODE mode)
{
    uint8_t t_addr;
    uint8_t t_data[6];
    uint8_t data_length;

    TUPD_PRINTF( LOG_NORMAL,"Enter x_tune()...\n");

    reg_write_val( tuner_block_id, 0x15, 0x12 );
	reg_write_val( tuner_block_id, 0x43, (frequency <= 2150000) ? 0x04 : 0x05 );
	reg_write_val( tuner_block_id, 0x45, (frequency <= 2150000) ? 0x01 : 0x00 );

    t_addr    = 0x6A;
    t_data[0] = 0x00;
    t_data[1] = 0x00;
    data_length = 2;
    reg_write( tuner_block_id, t_addr, t_data, data_length );

    reg_write_val( tuner_block_id, 0x75, 0xF9 );
    reg_write_val( tuner_block_id, 0x40, 0x07 );
    reg_write_val( tuner_block_id, 0x41, 0x07 );
    reg_write_val( tuner_block_id, 0x48, 0x07 );
    reg_write_val( tuner_block_id, 0x01, (frequency <= 2150000) ? 0x01 : 0x03);

    t_addr    = 0x04;
    t_data[0] = 0xC4;
    t_data[1] = 0x40;
    data_length = 2;
    reg_write( tuner_block_id, t_addr, t_data, data_length );

    t_addr    = 0x06;
    t_data[0] = 0x03;
	t_data[1] = ( mode == SONY_SAT_SYSTEM_ISDBS ) ? 0x80 : 0x00;
	t_data[2] = ( mode == SONY_SAT_SYSTEM_ISDBS ) ? 0x70 : 0xB4;
	t_data[3] = ( mode == SONY_SAT_SYSTEM_ISDBS ) ? 0x1E : 0x78;
	t_data[4] = ( mode == SONY_SAT_SYSTEM_ISDBS ) ? 0x02 : 0x08;
	t_data[5] = ( mode == SONY_SAT_SYSTEM_ISDBS ) ? 0x24 : 0x30;
    data_length = 6;
    reg_write( tuner_block_id, t_addr, t_data, data_length );

    t_addr    = 0x0C;
    if(frequency <= 2150000) {
        t_data[0] = 0xFE;
        t_data[1] = 0x02;
    }else{
        t_data[0] = 0xFC;
        t_data[1] = 0x22;
    }
    t_data[2] = 0x9E;
    t_data[3] = 0x16;
    data_length = 4;
    reg_write( tuner_block_id, t_addr, t_data, data_length );

    t_addr    = 0x10;
    t_data[0] = (uint8_t)((( frequency >> 2 ) & 0x0000ff));
    t_data[1] = (uint8_t)((( frequency >> 2 ) & 0x00ff00) >>  8);
    t_data[2] = (uint8_t)((( frequency >> 2 ) & 0x0f0000) >> 16);
    t_data[3] = 0xFF;
    t_data[4] = IQOUTLMT;
    t_data[5] = 0x01;
    data_length = 6;
    reg_write( tuner_block_id, t_addr , t_data, data_length );

    waitms(10);

    reg_write_val( tuner_block_id, 0x05, 0x00 );
    reg_write_val( tuner_block_id, 0x04, 0xC0 );

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
void isdbs_x_fin(uint16_t tuner_block_id)
{
    uint8_t t_addr;
    uint8_t t_data[3];
    uint8_t data_length;

    TUPD_PRINTF( LOG_NORMAL,"Enter x_fin()...\n");

    reg_write_val( tuner_block_id, 0x15, 0x12 );
    reg_write_val( tuner_block_id, 0x43, 0x05 );
    reg_write_val( tuner_block_id, 0x45, 0x01 );

    t_addr    = 0x0C;
    t_data[0] = 0x14;
    t_data[1] = 0x00;
    t_data[2] = 0x00;
    data_length = 3;
    reg_write( tuner_block_id, t_addr, t_data, data_length );

    reg_write_val( tuner_block_id, 0x01, 0x00 );
    reg_write_val( tuner_block_id, 0x05, 0x00 );
    reg_write_val( tuner_block_id, 0x04, 0xC0 );

    TUPD_PRINTF( LOG_NORMAL," Done. \n");
    return;
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
    return sut_pj987_reg_read(id, subadr, data, len);
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
