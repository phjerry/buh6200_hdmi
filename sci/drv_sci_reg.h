/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2008-2019. All rights reserved.
 * Description: the header of sci register driver
 */

#ifndef __DRV_SCI_REG_H__
#define __DRV_SCI_REG_H__

#include "hi_type.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef union {
    struct {
        hi_u32  scidata    : 8;
        hi_u32  parity     : 1;
        hi_u32  Reserved   : 23;
    } bits;

    hi_u32 value;
} sci_data_reg;

typedef union {
    struct {
        hi_u32  sense      : 1;
        hi_u32  order      : 1;
        hi_u32  txparity   : 1;
        hi_u32  txnak      : 1;
        hi_u32  rxparity   : 1;
        hi_u32  rxnak      : 1;
        hi_u32  clkdis     : 1;
        hi_u32  clkval     : 1;
        hi_u32  paritybit  : 1;
        hi_u32  vccen_inv  : 1;
        hi_u32  detect_inv : 1;
        hi_u32  Reserved   : 21;
    } bits;

    hi_u32 value;
} sci_scr0_reg;

typedef union {
    struct {
        hi_u32  atrden     : 1;
        hi_u32  blken      : 1;
        hi_u32  mode       : 1;
        hi_u32  clkzl      : 1;
        hi_u32  bgten      : 1;
        hi_u32  exdbnce    : 1;
        hi_u32  synccard   : 1;
        hi_u32  rstoutctl  : 1;
        hi_u32  vccenoutctl: 1;
        hi_u32  dataoutctl : 1;

        hi_u32  Reserved   : 22;
    } bits;

    hi_u32 value;
} sci_scr1_reg;


typedef union {
    struct {
        hi_u32  startup    : 1;
        hi_u32  finish     : 1;
        hi_u32  wreset     : 1;

        hi_u32  Reserved   : 29;
    } bits;

    hi_u32 value;
} sci_scr2_reg;

typedef union {
    struct {
        hi_u32  clkicc     : 8;

        hi_u32  Reserved   : 24;
    } bits;

    hi_u32 value;
} sci_clk_icc_reg;

typedef union {
    struct {
        hi_u32  value      : 8;

        hi_u32  Reserved   : 24;
    } bits;

    hi_u32 value;
} sci_value_reg;

typedef union {
    struct {
        hi_u32  baud       : 16;

        hi_u32  Reserved   : 16;
    } bits;

    hi_u32 value;
} sci_baud_reg;

typedef union {
    struct {
        hi_u32  rxdmae     : 1;
        hi_u32  txdmae     : 1;

        hi_u32  Reserved   : 30;
    } bits;

    hi_u32 value;
} sci_dmacr_reg;

typedef union {
    struct {
        hi_u32  stable     : 10;

        hi_u32  Reserved   : 22;
    } bits;

    hi_u32 value;
} sci_stable_reg;

typedef union {
    struct {
        hi_u32  atime      : 16;

        hi_u32  Reserved   : 16;
    } bits;

    hi_u32 value;
} sci_atime_reg;

typedef union {
    struct {
        hi_u32  dtime      : 16;

        hi_u32  Reserved   : 16;
    } bits;

    hi_u32 value;
} sci_dtime_reg;

typedef union {
    struct {
        hi_u32  atrstime   : 16;

        hi_u32  Reserved   : 16;
    } bits;

    hi_u32 value;
} sci_atrstime_reg;

typedef union {
    struct {
        hi_u32  atrdtime   : 16;

        hi_u32  Reserved   : 16;
    } bits;

    hi_u32 value;
} sci_atrdtime_reg;

typedef union {
    struct {
        hi_u32  stoptime   : 12;

        hi_u32  Reserved   : 20;
    } bits;

    hi_u32 value;
} sci_stop_time_reg;

typedef union {
    struct {
        hi_u32  starttime  : 12;

        hi_u32  Reserved   : 20;
    } bits;

    hi_u32 value;
} sci_start_time_reg;

typedef union {
    struct {
        hi_u32  txretry    : 3;
        hi_u32  rxretry    : 3;

        hi_u32  Reserved   : 26;
    } bits;

    hi_u32 value;
} sci_retry_reg;


typedef union {
    struct {
        hi_u32  chtimels   : 16;

        hi_u32  Reserved   : 16;
    } bits;

    hi_u32 value;
} sci_chtimels_reg;

typedef union {
    struct {
        hi_u32  chtimems   : 16;

        hi_u32  Reserved   : 16;
    } bits;

    hi_u32 value;
} sci_chtimems_reg;

typedef union {
    struct {
        hi_u32  blktimels  : 16;

        hi_u32  Reserved   : 16;
    } bits;

    hi_u32 value;
} sci_blktimels_reg;

typedef union {
    struct {
        hi_u32  blktimems  : 16;

        hi_u32  Reserved   : 16;
    } bits;

    hi_u32 value;
} sci_blktimems_reg;

typedef union {
    struct {
        hi_u32  scichguard   : 8;

        hi_u32  Reserved     : 24;
    } bits;

    hi_u32 value;
} sci_chguard_reg;

typedef union {
    struct {
        hi_u32  sciblkguard   : 8;

        hi_u32  Reserved      : 24;
    } bits;

    hi_u32 value;
} sci_blkguard_reg;

typedef union {
    struct {
        hi_u32  rxtime    : 16;

        hi_u32  Reserved  : 16;
    } bits;

    hi_u32 value;
} sci_rxtime_reg;

typedef union {
    struct {
        hi_u32  txff       : 1;
        hi_u32  txfe       : 1;
        hi_u32  rxff       : 1;
        hi_u32  rxfe       : 1;

        hi_u32  Reserved   : 28;
    } bits;

    hi_u32 value;
} sci_fifostatus_reg;

typedef union {
    struct {
        hi_u32  txcount    : 9;

        hi_u32  Reserved   : 23;
    } bits;

    hi_u32 value;
} sci_txcount_reg;

typedef union {
    struct {
        hi_u32  rxcount    : 9;

        hi_u32  Reserved   : 23;
    } bits;

    hi_u32 value;
} sci_rxcount_reg;

typedef union {
    struct {
        hi_u32  cardinim    : 1;
        hi_u32  cardoutim   : 1;
        hi_u32  cardupim    : 1;
        hi_u32  carddnim    : 1;
        hi_u32  txerrim     : 1;
        hi_u32  atrstoutim  : 1;
        hi_u32  atrdtoutim  : 1;
        hi_u32  blktoutim   : 1;

        hi_u32  chtoutim    : 1;
        hi_u32  rtoutim     : 1;
        hi_u32  rorim       : 1;
        hi_u32  clkstpim    : 1;
        hi_u32  clkactim    : 1;
        hi_u32  rxtideim    : 1;
        hi_u32  txtideim    : 1;

        hi_u32  Reserved    : 17;
    } bits;

    hi_u32 value;
} sci_imsc_reg;

typedef union {
    struct {
        hi_u32  cardinris    : 1;
        hi_u32  cardoutris   : 1;
        hi_u32  cardupris    : 1;
        hi_u32  carddnris    : 1;
        hi_u32  txerrris     : 1;
        hi_u32  atrstoutris  : 1;
        hi_u32  atrdtoutiris : 1;
        hi_u32  blktoutris   : 1;

        hi_u32  chtoutris    : 1;
        hi_u32  rtoutris     : 1;
        hi_u32  rorris       : 1;
        hi_u32  clkstpris    : 1;
        hi_u32  clkactris    : 1;
        hi_u32  rxtideris    : 1;
        hi_u32  txtideris    : 1;

        hi_u32  Reserved     : 17;
    } bits;

    hi_u32 value;
} sci_ris_reg;

typedef union {
    struct {
        hi_u32  cardinmis    : 1;
        hi_u32  cardoutmis   : 1;
        hi_u32  cardupmis    : 1;
        hi_u32  carddnmis    : 1;
        hi_u32  txerrmis     : 1;
        hi_u32  atrstoutmis  : 1;
        hi_u32  atrdtoutimis : 1;
        hi_u32  blktoutmis   : 1;

        hi_u32  chtoutmis    : 1;
        hi_u32  rtoutmis     : 1;
        hi_u32  rormis       : 1;
        hi_u32  clkstpmis    : 1;
        hi_u32  clkactmis    : 1;
        hi_u32  rxtidemis    : 1;
        hi_u32  txtidemis    : 1;

        hi_u32  Reserved     : 17;
    } bits;

    hi_u32 value;
} sci_mis_reg;

typedef union {
    struct {
        hi_u32  cardinic    : 1;
        hi_u32  cardoutic   : 1;
        hi_u32  cardupic    : 1;
        hi_u32  carddnic    : 1;
        hi_u32  txerric     : 1;
        hi_u32  atrstoutic  : 1;
        hi_u32  atrdtoutiic : 1;
        hi_u32  blktoutic   : 1;

        hi_u32  chtoutic    : 1;
        hi_u32  rtoutic     : 1;
        hi_u32  roric       : 1;
        hi_u32  clkstpic    : 1;
        hi_u32  clkactic    : 1;

        hi_u32  Reserved    : 19;
    } bits;

    hi_u32 value;
} sci_icr_reg;

typedef union {
    struct {
        hi_u32  power          : 1;
        hi_u32  creset         : 1;
        hi_u32  clken          : 1;
        hi_u32  dataen         : 1;
        hi_u32  fcb            : 1;
        hi_u32  nsciclkouten   : 1;
        hi_u32  nsciclken      : 1;
        hi_u32  sciclkout      : 1;

        hi_u32  nscidataouten  : 1;
        hi_u32  nscidataen     : 1;
        hi_u32  cardpresent    : 1;

        hi_u32  Reserved       : 21;
    } bits;

    hi_u32 value;
} sci_syncact_reg;

typedef union {
    struct {
        hi_u32  wdata        : 1;
        hi_u32  wclk         : 1;
        hi_u32  wdataen      : 1;
        hi_u32  wclken       : 1;
        hi_u32  wrst         : 1;
        hi_u32  wfcb         : 1;

        hi_u32  Reserved     : 22;
    } bits;

    hi_u32 value;
} sci_synctx_reg;

typedef union {
    struct {
        hi_u32  rdata        : 1;
        hi_u32  rclk         : 1;

        hi_u32  Reserved     : 30;
    } bits;

    hi_u32 value;
} sci_syncrx_reg;

typedef union {
    struct {
        hi_u32  parity_time  :3;

        hi_u32  Reserved     :29;
    } bits;

    hi_u32 value;
} sci_parity_time_reg;

typedef union {
    struct {
        hi_u32  txtide     : 9;

        hi_u32  Reserved   : 23;
    } bits;

    hi_u32 value;
} sci_txtide_reg;

typedef union {
    struct {
        hi_u32  rxtide     : 9;

        hi_u32  Reserved   : 23;
    } bits;

    hi_u32 value;
} sci_rxtide_reg;

typedef struct {
    sci_data_reg          sci_data;
    sci_scr0_reg          sci_cr0;
    sci_scr1_reg          sci_cr1;
    sci_scr2_reg          sci_cr2;
    sci_clk_icc_reg       sci_clk_icc;
    sci_value_reg         sci_value;
    sci_baud_reg          sci_baud;
/* Physical memory is not continuous, complement */
    hi_u32                sci_reserved0;
    sci_dmacr_reg         sci_dmacr;
    sci_stable_reg        sci_stable;
    sci_atime_reg         sci_atime;
    sci_dtime_reg         sci_dtime;
    sci_atrstime_reg      sci_atr_stime;
    sci_atrdtime_reg      sci_atr_dtime;
    sci_stop_time_reg     sci_stop_time;
    sci_start_time_reg    sci_start_time;
    sci_retry_reg         sci_retry;
    sci_chtimels_reg      sci_chtimels;
    sci_chtimems_reg      sci_chtimems;
    sci_blktimels_reg     sci_blktimels;
    sci_blktimems_reg     sci_blktimems;
    sci_chguard_reg       sci_chguard;
    sci_blkguard_reg      sci_blkguard;
    sci_rxtime_reg        sci_rxtime;
    sci_fifostatus_reg    sci_fifostatus;
    sci_txcount_reg       sci_txcount;
    sci_rxcount_reg       sci_rxcount;
    sci_imsc_reg          sci_imsc;
    sci_ris_reg           sci_ris;
    sci_mis_reg           sci_mis;
    sci_icr_reg           sci_icr;
    sci_syncact_reg       sci_syncact;
    sci_synctx_reg        sci_synctx;
    sci_syncrx_reg        sci_syncrx;
/* Physical memory is not continuous, complement */
    hi_u32                sci_reserved1;
    hi_u32                sci_reserved2;
    sci_parity_time_reg   sci_parity_time;
} sci_reg;

typedef struct {
    sci_rxtide_reg        sci_rxtide;
    sci_txtide_reg        sci_txtide;
} sci_tide_reg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
