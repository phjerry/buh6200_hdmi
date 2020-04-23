/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2017-2019. All rights reserved.
 * Description: demux basic ioctl cmd.
 * Author: sdk
 * Create: 2017-05-31
 */

#ifndef __DRV_DEMUX_IOCTL_H__
#define __DRV_DEMUX_IOCTL_H__

#include "hi_drv_demux.h"
#include "hi_drv_module.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DMX_CMD_MASK              (0xF0)
#define DMX_GLB_CMD               (0x00)
#define DMX_PORT_CMD              (0x10) /* IF/TSI/TAG/TSO/TSIO */
#define DMX_RAM_CMD               (0x20)
#define DMX_RMX_CMD               (0x30)
#define DMX_BAND_CMD              (0x40)
#define DMX_PIDCH_CMD             (0x50)
#define DMX_PLAYFCT_CMD0          (0x60)
#define DMX_PLAYFCT_CMD1          (0x70)
#define DMX_PLAYFCT_CMD2          (0x80)
#define DMX_RECFCT_CMD0           (0x90)
#define DMX_RECFCT_CMD1           (0xA0)
#define DMX_DSCFCT_CMD0           (0xB0)
#define DMX_DSCFCT_CMD1           (0xC0)
#define DMX_PCRFCT_CMD            (0xD0)

/************* tsi(general) port begin ******************/
typedef struct {
    dmx_port              port;      /* [in] */
    dmx_port_attr         attrs;     /* [out] */
} dmx_get_port_attrs;

typedef dmx_get_port_attrs dmx_set_port_attrs;

/***************** tso port begin ***********************/
typedef struct {
    dmx_port          port;      /* [in] */
    dmx_tso_port_attr attrs;     /* [out] */
} dmx_get_tso_port_attrs;

typedef dmx_get_tso_port_attrs dmx_set_tso_port_attrs;

/***************** tag port begin ***********************/
typedef struct {
    dmx_port port;            /* [in] */
    dmx_tag_port_attr attrs;  /* [out] */
} dmx_get_tag_port_attrs;

typedef dmx_get_tag_port_attrs dmx_set_tag_port_attrs;

/******************** ramport  begin**************************/
typedef struct {
    hi_handle           handle;    /* [in] */
    dmx_port_attr       attrs;     /* [out] */
} dmx_get_ram_port_attrs;

typedef dmx_get_ram_port_attrs dmx_set_ram_port_attrs;

typedef struct {
    dmx_port                port;      /* [in] */
    dmx_ram_port_attr       attrs;     /* [in] */
    hi_handle               handle;    /* [out] */
} dmx_open_ram_port_info;

typedef struct {
    hi_handle            handle;    /* [in] */
    dmx_ram_port_status  status;    /* [out] */
} dmx_get_ram_port_status_info;

typedef struct {
    hi_handle            handle;     /* [in] */
    hi_u32               req_len;    /* [in] */
    hi_u32               time_out;   /* [in] */
    dmx_ram_buffer       buf;        /* [out] */
} dmx_get_ram_buffer_info;

typedef struct {
    hi_handle              handle;     /* [in] */
    hi_u32                 req_len;    /* [in] */
    hi_u32                 time_out;   /* [in] */
    dmx_compat_ram_buffer  buf;        /* [out] */
} dmx_compat_get_ram_buffer_info;

typedef struct {
    hi_handle              handle;    /* [in] */
    dmx_ram_buffer         buf;       /* [in] */
} dmx_push_ram_buffer_info;

typedef struct {
    hi_handle              handle;    /* [in] */
    dmx_compat_ram_buffer  buf;       /* [out] */
} dmx_compat_push_ram_buffer_info;

typedef struct {
    hi_handle              handle;
    hi_u32                 valid_datalen;
    hi_u32                 start_pos;
} dmx_put_ram_buffer_info;

typedef dmx_put_ram_buffer_info dmx_compat_put_ram_buffer_info;

typedef dmx_push_ram_buffer_info dmx_release_ram_buffer_info;
typedef dmx_compat_push_ram_buffer_info dmx_compat_release_ram_buffer_info;

typedef struct {
    hi_handle handle;
    dmx_port port;
} dmx_get_ram_portid_info;

typedef dmx_get_ram_portid_info dmx_get_ram_porthandle_info;

typedef struct {
    hi_handle            handle;     /* [in] */
    hi_u32               buf_size;   /* [out] */
    hi_void              *usr_addr; /*  [out] */
    hi_s64               buf_handle; /* [out] */
} dmx_pre_mmap_info;

typedef struct {
    hi_handle            handle;     /* [in] */
    hi_u32               buf_size;   /* [out] */
    hi_u32               usr_addr;   /* [out] */
    hi_s64               buf_handle; /* [out] */
} dmx_compat_pre_mmap_info;

typedef struct {
    hi_handle            handle;    /* [in] */
    hi_void              *usr_addr; /* [in] */
} dmx_pst_mmap_info;

typedef struct {
    hi_handle            handle;    /* [in] */
    hi_u32               usr_addr;  /* [in] */
} dmx_compat_pst_mmap_info;

typedef struct {
    hi_handle handle;               /* [in] */
    hi_mem_handle_t buf_handle;          /* [out] */
    hi_mem_handle_t flush_buf_handle;    /* [out] */
    hi_mem_handle_t dsc_buf_handle;      /* [out] */
} dmx_get_ram_buf_handle;

typedef struct {
    dmx_port             port;      /* [in] */
    dmx_port_packet_num  port_stat; /* [out] */
}dmx_port_packet_info;

/******************** band  begin**************************/
typedef struct {
    dmx_band                band;      /* [in] */
    dmx_band_attr           attrs;     /* [in] */
    hi_handle               handle;    /* [out] */
} dmx_open_band_info;

typedef struct {
    hi_handle         handle;    /* [in] */
    dmx_port          port;      /* [in] */
} dmx_band_attach_port_info;

typedef struct {
    hi_handle         handle;    /* [in] */
    dmx_band_attr     attrs;     /* [out] */
} dmx_get_band_attr_info;

typedef struct {
    hi_handle         handle;    /* [in] */
    dmx_band_attr     attrs;     /* [out] */
} dmx_set_band_attr_info;

typedef struct {
    hi_handle            handle;    /* [in] */
    dmx_band_status      status;    /* [out] */
} dmx_get_band_status_info;

/****************** pid_channel  begin************************/
typedef struct {
    hi_handle               band_handle; /* [in] */
    hi_u32                  pid;         /* [in] */
    hi_handle               handle;      /* [out] */
} dmx_create_pid_ch_info;

typedef struct {
    hi_handle               band_handle; /* [in] */
    hi_u32                  pid;         /* [in] */
    hi_handle               handle;      /* [out] */
} dmx_get_pid_ch_handle_info;

typedef struct {
    hi_handle                handle;    /* [in] */
    dmx_pidch_status   status;          /* [out] */
} dmx_get_pid_ch_status_info;

/****************** play_fct  begin************************/
typedef struct {
    dmx_play_attrs          attrs;      /* [in] */
    hi_handle               handle;     /* [out] */
} dmx_create_play_fct_info;

typedef struct {
    hi_handle               handle;     /* [in] */
    dmx_play_attrs          attrs;      /* [out] */
} dmx_get_play_fct_attr_info;

typedef struct {
    hi_handle               handle;     /* [in] */
    dmx_play_attrs          attrs;      /* [in] */
} dmx_set_play_fct_attr_info;

typedef struct {
    hi_handle               handle;     /* [in] */
    dmx_play_status         status;     /* [out] */
} dmx_get_play_fct_status_info;

typedef struct {
    hi_handle               handle;     /* [in] */
    dmx_chan_packet_num     chn_stat;   /* [out] */
} dmx_get_play_fct_packet_info;

typedef struct {
    hi_handle               handle;           /* [in] */
    hi_dmx_scrambled_flag   scramble_flag;    /* [out] */
} dmx_play_fct_get_scrambled_flag_info;

typedef struct {
    hi_handle               handle;         /* [in] */
    hi_handle               pid_ch_handle;  /* [out] */
} dmx_play_fct_get_pid_ch_info;

typedef struct {
    hi_handle               handle;         /* [in] */
    hi_handle               pid_ch_handle;  /* [in] */
} dmx_play_fct_attach_pid_ch_info;

typedef struct {
    dmx_filter_attrs        attrs;         /* [in] */
    hi_handle               flt_handle;    /* [out] */
} dmx_play_fct_create_flt_info;

typedef struct {
    hi_handle               handle;        /* [in] */
    hi_handle               flt_handle;    /* [in] */
} dmx_play_fct_add_flt_info;

typedef dmx_play_fct_add_flt_info dmx_play_fct_del_flt_info;

typedef struct {
    hi_handle               flt_handle;     /* [in] */
    dmx_filter_attrs        attrs;          /* [in] */
} dmx_play_fct_update_flt_info;

typedef struct {
    hi_handle               flt_handle;     /* [in] */
    dmx_filter_attrs        attrs;          /* [out] */
} dmx_play_fct_get_flt_info;

typedef struct {
    hi_handle        flt_handle;          /* [in] */
    hi_handle        play_handle;         /* [out] */
} dmx_play_fct_get_handle_info;

typedef struct {
    hi_handle                handle;        /* [in] */
    hi_u32                   acq_num;       /* [in] */
    hi_u32                   time_out;      /* [in] */
    hi_u32                   acqed_num;     /* [out] */
    dmx_buffer               *play_fct_buf; /* [out] */
} dmx_play_fct_acquire_buf_info;

typedef struct {
    hi_handle                handle;        /* [in] */
    hi_u32                   acq_num;       /* [in] */
    hi_u32                   time_out;      /* [in] */
    hi_u32                   acqed_num;     /* [out] */
    hi_u32                   play_fct_buf;  /* [out] */
} dmx_compat_play_fct_acquire_buf_info;


typedef struct {
    hi_handle                handle;         /* [in] */
    hi_u32                   rel_num;        /* [in] */
    dmx_buffer               *play_fct_buf;  /* [in] */
} dmx_play_fct_release_buf_info;

typedef struct {
    hi_handle                handle;         /* [in] */
    hi_u32                   rel_num;        /* [in] */
    hi_u32                   play_fct_buf;   /* [in] */
} dmx_compat_play_fct_release_buf_info;

typedef struct {
    hi_u32 watch_num;             /* [in] */
    hi_handle *valid_channel;     /* [out] channel has data ready */
    hi_u32 *valid_num;            /* [out] channel has data number */
    hi_u32 time_out_ms;
} dmx_get_data_flag;

typedef struct {
    hi_u32 watch_num;             /* [in] */
    hi_u32 valid_channel;     /* [out] channel has data ready */
    hi_u32 valid_num;            /* [out] channel has data number */
    hi_u32 time_out_ms;
} dmx_compat_get_data_flag;

typedef struct {
    hi_handle *watch_array;           /* [in] */
    hi_u32 watch_num;             /* [in] */
    hi_handle *valid_channel;     /* [out] channel has data ready */
    hi_u32 *valid_num;            /* [out] channel has data number */
    hi_u32 time_out_ms;
} dmx_select_data_flag;

typedef struct {
    hi_u32 watch_array;        /* [in] */
    hi_u32 watch_num;          /* [in] */
    hi_u32 valid_channel;      /* [out] channel has data ready */
    hi_u32 valid_num;          /* [out] channel has data number */
    hi_u32 time_out_ms;
} dmx_compat_select_data_flag;

typedef struct {
    hi_handle handle;       /* [in] */
    hi_handle buf_handle;   /* [out] */
    dmx_play_type type;      /* [out] */
} dmx_play_get_buf_handle;

/****************** rec_fct  begin************************/
typedef struct {
    dmx_rec_attrs           attrs;      /* [in] */
    hi_handle               handle;     /* [out] */
} dmx_create_rec_fct_info;

typedef struct {
    hi_handle               handle;     /* [in] */
    dmx_rec_attrs           attrs;      /* [out] */
} dmx_get_rec_fct_attr_info;

typedef struct {
    hi_handle               handle;     /* [in] */
    dmx_rec_attrs           attrs;      /* [in] */
} dmx_set_rec_fct_attr_info;

typedef struct {
    hi_handle               handle;     /* [in] */
    dmx_rec_status          status;     /* [out] */
} dmx_get_rec_fct_status_info;

typedef struct {
    hi_handle               handle;         /* [in] */
    hi_handle               ch_handle;      /* [in] */ /* pid channel handle or band handle */
} dmx_rec_fct_add_del_ch_info;

typedef struct {
    hi_handle               handle;         /* [in] */
    hi_handle               pid_ch_handle;  /* [in] */
} dmx_rec_fct_attach_pid_ch_info;

typedef struct {
    hi_handle               handle;         /* [in] */
    hi_u32                  req_len;        /* [in] */
    hi_u32                  time_out;       /* [in] */
    dmx_buffer              *rec_fct_buf;   /* [out] */
} dmx_rec_fct_acquire_buf_info;

typedef struct {
    hi_handle               handle;         /* [in] */
    hi_u32                  req_len;        /* [in] */
    hi_u32                  time_out;       /* [in] */
    hi_u32                  rec_fct_buf;    /* [out] */
} dmx_compat_rec_fct_acquire_buf_info;


typedef struct {
    hi_handle               handle;         /* [in] */
    dmx_buffer              *rec_fct_buf;   /* [in] */
} dmx_rec_fct_release_buf_info;

typedef struct {
    hi_handle               handle;         /* [in] */
    hi_u32                  rec_fct_buf;    /* [in] */
} dmx_compat_rec_fct_release_buf_info;

typedef struct {
    hi_handle               handle;         /* [in] */
    hi_u32                  req_num;        /* [in] */
    hi_u32                  time_out;       /* [in] */
    hi_u32                  reqed_num;      /* [out] */
    dmx_index_data          *index_data;    /* [out] */
} dmx_rec_index_info, dmx_play_index_info;

typedef struct {
    hi_handle               handle;        /* [in] */
    hi_u32                  req_num;       /* [in] */
    hi_u32                  time_out;      /* [in] */
    hi_u32                  reqed_num;     /* [out] */
    hi_u32                  index_data;    /* [out] */
} dmx_compat_rec_index_info, dmx_compat_play_index_info;

typedef struct {
    hi_handle               handle;        /* [in] */
    hi_u32                  time_out;      /* [in] */
    hi_u32                  index_num;     /* [out] */
    hi_u32                  rec_data_len;  /* [out] */
} dmx_rec_peek_idx_buf_info;

typedef struct {
    hi_handle               handle;
    hi_bool                 eos_flag;
} dmx_rec_set_eos_info;

typedef struct {
    hi_handle handle;       /* [in] */
    hi_handle buf_handle;   /* [out] */
} dmx_rec_get_buf_handle;

/****************** dsc_fct  begin************************/
typedef struct {
    dmx_dsc_attrs           attrs;      /* [in] */
    hi_handle               handle;     /* [out] */
} dmx_create_dsc_fct_info;

typedef struct {
    hi_handle               handle;     /* [in] */
    dmx_dsc_attrs           attrs;      /* [out] */
} dmx_get_dsc_fct_attr_info;

typedef struct {
    hi_handle               handle;     /* [in] */
    dmx_dsc_attrs           attrs;      /* [in] */
} dmx_set_dsc_fct_attr_info;

typedef struct {
    hi_handle               handle;            /* [in] */
    hi_handle               target_handle;     /* [in] */
} dmx_dsc_fct_attach_info, dmx_dsc_fct_detach_info;

typedef struct {
    hi_handle               handle;        /* [in] */
    hi_handle               ks_handle;     /* [out] */
} dmx_dsc_fct_get_ks_handle_info;


typedef struct {
    hi_handle               handle;                 /* [in] */
    hi_u32                  len;                    /* [in] */
    hi_u8                   key[DMX_MAX_KEY_LEN];   /* [in] */
} dmx_dsc_fct_set_key_info;

typedef struct {
    hi_handle               pid_ch_handle;          /* [in] */
    hi_handle               dsc_handle;             /* [out] */
} dmx_dsc_fct_get_key_handle_info;

typedef struct {
    hi_handle               handle;                 /* [in] */
    hi_u32                  len;                    /* [in] */
    hi_u8                   key[DMX_SYS_KEY_LEN];   /* [in] */
} dmx_dsc_fct_set_sys_key_info;

/****************** pcr_fct  begin************************/
typedef struct {
    hi_handle               band_handle;       /* [in] */
    hi_u32                  pid;               /* [in] */
    hi_handle               handle;            /* [out] */
} dmx_create_pcr_fct_info;

typedef struct {
    hi_handle                handle;        /* [in] */
    hi_handle                sync_handle;   /* [in] */
} dmx_pcr_fct_attach_info;

typedef struct {
    hi_handle                handle;    /* [in] */
    dmx_pcr_status           status;    /* [out] */
} dmx_get_pcr_fct_status_info;


typedef struct {
    hi_handle                handle;     /* [in] */
    hi_u64                   pcr_ms;     /* [out] */
    hi_u64                   scr_ms;     /* [out] */
} dmx_pcr_fct_get_pcr_scr_info;

/******************rmx_fct  begin**********************/
typedef struct {
    dmx_rmx_attrs            attrs;             /* [in] */
    hi_handle                rmx_handle;        /* [out] */
} dmx_create_rmx_fct_info;

typedef struct {
    hi_handle                rmx_handle;        /* [in] */
    dmx_rmx_attrs            attrs;             /* [out] */
} dmx_get_rmx_fct_attr_info;

typedef struct {
    hi_handle                rmx_handle;        /* [in] */
    dmx_rmx_attrs            attrs;             /* [in] */
} dmx_set_rmx_fct_attr_info;

typedef struct {
    hi_handle                rmx_handle;        /* [in] */
    dmx_rmx_status           status;            /* [out] */
} dmx_get_rmx_fct_status_info;

typedef struct {
    hi_handle                rmx_handle;        /* [in] */
    dmx_rmx_pump_attrs       pump_attrs;        /* [in] */
    hi_handle                rmx_pump_handle;   /* [out] */
} dmx_rmx_fct_add_pump_info;

typedef struct {
    hi_handle                rmx_pump_handle;    /* [in] */
    dmx_rmx_pump_attrs       pump_attrs;         /* [out] */
} dmx_get_rmx_pump_attr_info;

typedef struct {
    hi_handle                rmx_pump_handle;   /* [in] */
    dmx_rmx_pump_attrs       pump_attrs;        /* [in] */
} dmx_set_rmx_pump_attr_info;

typedef struct {
    hi_handle                  chan_handle;
    dmx_chan_cc_repeat_mode cc_repeat_mode;
} dmx_set_chan_cc_repeat_info;

typedef struct {
    hi_bool                  no_pusi;         /* [in] */
} dmx_pusi_set_info;

typedef struct {
    hi_u32                   band_handle;     /* [in] */
    hi_bool                  tei;             /* [in] */
} dmx_tei_set_info;

/* global cmd */
#define DMX_IOCTL_GLB_GETCAP               _IOWR(HI_ID_DEMUX, (DMX_GLB_CMD + 0x0), dmx_capability)
#define DMX_IOCTL_GLB_GET_PACKETNUM        _IOWR(HI_ID_DEMUX, (DMX_GLB_CMD + 0x1), dmx_port_packet_info)

/* if_port */
#define DMX_IOCTL_IF_GETPORTATTRS          _IOWR(HI_ID_DEMUX, (DMX_PORT_CMD + 0x0), dmx_get_port_attrs)
#define DMX_IOCTL_IF_SETPORTATTRS          _IOW(HI_ID_DEMUX,  (DMX_PORT_CMD + 0x1), dmx_set_port_attrs)

/* tsi_port */
#define DMX_IOCTL_TSI_GETPORTATTRS         _IOWR(HI_ID_DEMUX, (DMX_PORT_CMD + 0x2), dmx_get_port_attrs)
#define DMX_IOCTL_TSI_SETPORTATTRS         _IOW(HI_ID_DEMUX,  (DMX_PORT_CMD + 0x3), dmx_set_port_attrs)

/* tag port */
#define DMX_IOCTL_TAG_GETPORTATTRS         _IOWR(HI_ID_DEMUX, (DMX_PORT_CMD + 0x4), dmx_get_tag_port_attrs)
#define DMX_IOCTL_TAG_SETPORTATTRS         _IOW(HI_ID_DEMUX,  (DMX_PORT_CMD + 0x5), dmx_set_tag_port_attrs)

/* tso port */
#define DMX_IOCTL_TSO_GETPORTATTRS         _IOWR(HI_ID_DEMUX, (DMX_PORT_CMD + 0x6), dmx_get_tso_port_attrs)
#define DMX_IOCTL_TSO_SETPORTATTRS         _IOW(HI_ID_DEMUX,  (DMX_PORT_CMD + 0x7), dmx_set_tso_port_attrs)

/* ram port */
#define DMX_IOCTL_RAM_OPENPORT             _IOWR(HI_ID_DEMUX, (DMX_RAM_CMD + 0x0), dmx_open_ram_port_info)
#define DMX_IOCTL_RAM_GETPORTATTRS         _IOWR(HI_ID_DEMUX, (DMX_RAM_CMD + 0x1), dmx_get_port_attrs)
#define DMX_IOCTL_RAM_SETPORTATTRS         _IOW(HI_ID_DEMUX,  (DMX_RAM_CMD + 0x2), dmx_set_port_attrs)
#define DMX_IOCTL_RAM_GETPORTSTATUS        _IOWR(HI_ID_DEMUX, (DMX_RAM_CMD + 0x3), dmx_get_ram_port_status_info)
#define DMX_IOCTL_RAM_GETBUFFER            _IOWR(HI_ID_DEMUX, (DMX_RAM_CMD + 0x4), dmx_get_ram_buffer_info)
#define DMX_COMPAT_IOCTL_RAM_GETBUFFER     _IOWR(HI_ID_DEMUX, (DMX_RAM_CMD + 0x4), dmx_compat_get_ram_buffer_info)
#define DMX_IOCTL_RAM_PUSHBUFFER           _IOW(HI_ID_DEMUX,  (DMX_RAM_CMD + 0x5), dmx_push_ram_buffer_info)
#define DMX_COMPAT_IOCTL_RAM_PUSHBUFFER    _IOW(HI_ID_DEMUX,  (DMX_RAM_CMD + 0x5), dmx_compat_push_ram_buffer_info)
#define DMX_IOCTL_RAM_PUTBUFFER            _IOW(HI_ID_DEMUX,  (DMX_RAM_CMD + 0x6), dmx_put_ram_buffer_info)
#define DMX_COMPAT_IOCTL_RAM_PUTBUFFER     _IOW(HI_ID_DEMUX,  (DMX_RAM_CMD + 0x6), dmx_compat_put_ram_buffer_info)
#define DMX_IOCTL_RAM_FLUSHBUFFER          _IOW(HI_ID_DEMUX,  (DMX_RAM_CMD + 0x7), hi_handle)
#define DMX_IOCTL_RAM_RESETBUFFER          _IOW(HI_ID_DEMUX,  (DMX_RAM_CMD + 0x8), hi_handle)
#define DMX_IOCTL_RAM_CLOSEPORT            _IOW(HI_ID_DEMUX,  (DMX_RAM_CMD + 0x9), hi_handle)
#define DMX_IOCTL_RAM_PREMMAP              _IOWR(HI_ID_DEMUX, (DMX_RAM_CMD + 0xA), dmx_pre_mmap_info)
#define DMX_COMPAT_IOCTL_RAM_PREMMAP       _IOWR(HI_ID_DEMUX, (DMX_RAM_CMD + 0xA), dmx_compat_pre_mmap_info)
#define DMX_IOCTL_RAM_PSTMMAP              _IOW(HI_ID_DEMUX,  (DMX_RAM_CMD + 0xB), dmx_pst_mmap_info)
#define DMX_COMPAT_IOCTL_RAM_PSTMMAP       _IOW(HI_ID_DEMUX,  (DMX_RAM_CMD + 0xB), dmx_compat_pst_mmap_info)
#define DMX_IOCTL_RAM_RELEASEBUFFER        _IOW(HI_ID_DEMUX,  (DMX_RAM_CMD + 0xC), dmx_release_ram_buffer_info)
#define DMX_COMPAT_IOCTL_RAM_RELEASEBUFFER _IOW(HI_ID_DEMUX,  (DMX_RAM_CMD + 0xC), dmx_compat_release_ram_buffer_info)
#define DMX_IOCTL_RAM_GETPORTID            _IOWR(HI_ID_DEMUX, (DMX_RAM_CMD + 0xD), dmx_get_ram_portid_info)
#define DMX_IOCTL_RAM_GETPORTHANDLE        _IOWR(HI_ID_DEMUX, (DMX_RAM_CMD + 0xE), dmx_get_ram_portid_info)
#define DMX_IOCTL_RAM_GETBUFHANDLE         _IOWR(HI_ID_DEMUX, (DMX_RAM_CMD + 0xF), dmx_get_ram_buf_handle)

/* rmxfct */
#define DMX_IOCTL_RMXFCT_CREATE          _IOWR(HI_ID_DEMUX, (DMX_RMX_CMD + 0x0), dmx_create_rmx_fct_info)
#define DMX_IOCTL_RMXFCT_OPEN            _IOWR(HI_ID_DEMUX, (DMX_RMX_CMD + 0x1), hi_handle)
#define DMX_IOCTL_RMXFCT_GETATTRS        _IOWR(HI_ID_DEMUX, (DMX_RMX_CMD + 0x2), dmx_get_rmx_fct_attr_info)
#define DMX_IOCTL_RMXFCT_SETATTRS        _IOW(HI_ID_DEMUX,  (DMX_RMX_CMD + 0x3), dmx_set_rmx_fct_attr_info)
#define DMX_IOCTL_RMXFCT_GETSTATUS       _IOWR(HI_ID_DEMUX, (DMX_RMX_CMD + 0x4), dmx_get_rmx_fct_status_info)
#define DMX_IOCTL_RMXFCT_ADDPUMP         _IOWR(HI_ID_DEMUX, (DMX_RMX_CMD + 0x5), dmx_rmx_fct_add_pump_info)
#define DMX_IOCTL_RMXFCT_DELPUMP         _IOW(HI_ID_DEMUX,  (DMX_RMX_CMD + 0x6), hi_handle)
#define DMX_IOCTL_RMXFCT_DELALLPUMP      _IOW(HI_ID_DEMUX,  (DMX_RMX_CMD + 0x7), hi_handle)
#define DMX_IOCTL_RMXFCT_GETPUMPATTRS    _IOWR(HI_ID_DEMUX, (DMX_RMX_CMD + 0x8), dmx_get_rmx_pump_attr_info)
#define DMX_IOCTL_RMXFCT_SETPUMPATTRS    _IOW(HI_ID_DEMUX,  (DMX_RMX_CMD + 0x9), dmx_set_rmx_pump_attr_info)
#define DMX_IOCTL_RMXFCT_CLOSE           _IOW(HI_ID_DEMUX,  (DMX_RMX_CMD + 0xA), hi_handle)
#define DMX_IOCTL_RMXFCT_DESTROY         _IOW(HI_ID_DEMUX,  (DMX_RMX_CMD + 0xB), hi_handle)

/* band */
#define DMX_IOCTL_BAND_OPEN              _IOWR(HI_ID_DEMUX, (DMX_BAND_CMD + 0x0), dmx_open_band_info)
#define DMX_IOCTL_BAND_REFINC            _IOWR(HI_ID_DEMUX, (DMX_BAND_CMD + 0x1), dmx_open_band_info)
#define DMX_IOCTL_BAND_ATTACHPORT        _IOW(HI_ID_DEMUX,  (DMX_BAND_CMD + 0x2), dmx_band_attach_port_info)
#define DMX_IOCTL_BAND_DETACHPORT        _IOW(HI_ID_DEMUX,  (DMX_BAND_CMD + 0x3), hi_handle)
#define DMX_IOCTL_BAND_GETATTRS          _IOWR(HI_ID_DEMUX, (DMX_BAND_CMD + 0x4), dmx_get_band_attr_info)
#define DMX_IOCTL_BAND_SETATTRS          _IOW(HI_ID_DEMUX,  (DMX_BAND_CMD + 0x5), dmx_set_band_attr_info)
#define DMX_IOCTL_BAND_GETSTATUS         _IOWR(HI_ID_DEMUX, (DMX_BAND_CMD + 0x6), dmx_get_band_status_info)
#define DMX_IOCTL_BAND_CLOSE             _IOW(HI_ID_DEMUX,  (DMX_BAND_CMD + 0x7), hi_handle)
#define DMX_IOCTL_BAND_REFDEC            _IOW(HI_ID_DEMUX,  (DMX_BAND_CMD + 0x8), hi_handle)
#define DMX_IOCTL_BAND_TEI_SET           _IOW(HI_ID_DEMUX, (DMX_BAND_CMD + 0x9), dmx_tei_set_info)

/* pid_channel */
#define DMX_IOCTL_PIDCHANNEL_CREATE         _IOWR(HI_ID_DEMUX, (DMX_PIDCH_CMD + 0x0), dmx_create_pid_ch_info)
#define DMX_IOCTL_PIDCHANNEL_REFINC         _IOWR(HI_ID_DEMUX, (DMX_PIDCH_CMD + 0x1), dmx_create_pid_ch_info)
#define DMX_IOCTL_PIDCHANNEL_GETHANDLE      _IOWR(HI_ID_DEMUX, (DMX_PIDCH_CMD + 0x2), dmx_get_pid_ch_handle_info)
#define DMX_IOCTL_PIDCHANNEL_GETSTATUS      _IOWR(HI_ID_DEMUX, (DMX_PIDCH_CMD + 0x3), dmx_get_pid_ch_status_info)
#define DMX_IOCTL_PIDCHANNEL_LOCKOUT        _IOW(HI_ID_DEMUX,  (DMX_PIDCH_CMD + 0x4), hi_handle)
#define DMX_IOCTL_PIDCHANNEL_UNLOCKOUT      _IOW(HI_ID_DEMUX,  (DMX_PIDCH_CMD + 0x5), hi_handle)
#define DMX_IOCTL_PIDCHANNEL_DESTROY        _IOW(HI_ID_DEMUX,  (DMX_PIDCH_CMD + 0x6), hi_handle)
#define DMX_IOCTL_PIDCHANNEL_REFDEC         _IOW(HI_ID_DEMUX,  (DMX_PIDCH_CMD + 0x7), hi_handle)
#define DMX_IOCTL_PIDCHANNEL_GETFREECNT     _IOWR(HI_ID_DEMUX, (DMX_PIDCH_CMD + 0x8), hi_u32)

/* playfct */
#define DMX_IOCTL_PLAYFCT_CREATE             _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD0 + 0x0), dmx_create_play_fct_info)
#define DMX_IOCTL_PLAYFCT_OPEN               _IOW(HI_ID_DEMUX,  (DMX_PLAYFCT_CMD0 + 0x1), hi_handle)
#define DMX_IOCTL_PLAYFCT_GETATTRS           _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD0 + 0x2), dmx_get_play_fct_attr_info)
#define DMX_IOCTL_PLAYFCT_SETATTRS           _IOW(HI_ID_DEMUX,  (DMX_PLAYFCT_CMD0 + 0x3), dmx_set_play_fct_attr_info)
#define DMX_IOCTL_PLAYFCT_GETSTATUS          _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD0 + 0x4), dmx_get_play_fct_status_info)
#define DMX_IOCTL_PLAYFCT_ATTACHPIDCH        _IOW(HI_ID_DEMUX,  (DMX_PLAYFCT_CMD0 + 0x5), dmx_play_fct_attach_pid_ch_info)
#define DMX_IOCTL_PLAYFCT_DETACHPIDCH        _IOW(HI_ID_DEMUX,  (DMX_PLAYFCT_CMD0 + 0x6), hi_handle)
#define DMX_IOCTL_PLAYFCT_CREATEFILTER       _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD0 + 0x7), dmx_play_fct_create_flt_info)
#define DMX_IOCTL_PLAYFCT_ADDFILTER          _IOW(HI_ID_DEMUX,  (DMX_PLAYFCT_CMD0 + 0x8), dmx_play_fct_add_flt_info)
#define DMX_IOCTL_PLAYFCT_DELFILTER          _IOW(HI_ID_DEMUX,  (DMX_PLAYFCT_CMD0 + 0x9), dmx_play_fct_del_flt_info)
#define DMX_IOCTL_PLAYFCT_DELALLFILTER       _IOW(HI_ID_DEMUX,  (DMX_PLAYFCT_CMD0 + 0xA), hi_handle)
#define DMX_IOCTL_PLAYFCT_GETFILTER          _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD0 + 0xB), dmx_play_fct_get_flt_info)
#define DMX_IOCTL_PLAYFCT_UPDATEFILTER       _IOW(HI_ID_DEMUX,  (DMX_PLAYFCT_CMD0 + 0xC), dmx_play_fct_update_flt_info)
#define DMX_IOCTL_PLAYFCT_GET_FREEFLT_CNT    _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD0 + 0xD), hi_u32)
#define DMX_IOCTL_PLAYFCT_DESTROYFILTER      _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD0 + 0xE), hi_handle)
#define DMX_IOCTL_PLAYFCT_ACQUIREBUF         _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD0 + 0xF), dmx_play_fct_acquire_buf_info)
#define DMX_COMPAT_IOCTL_PLAYFCT_ACQUIREBUF  _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD0 + 0xF), dmx_compat_play_fct_acquire_buf_info)

#define DMX_IOCTL_PLAYFCT_RELEASEBUF         _IOW(HI_ID_DEMUX,  (DMX_PLAYFCT_CMD1 + 0x0), dmx_play_fct_release_buf_info)
#define DMX_COMPAT_IOCTL_PLAYFCT_RELEASEBUF  _IOW(HI_ID_DEMUX,  (DMX_PLAYFCT_CMD1 + 0x0), dmx_compat_play_fct_release_buf_info)
#define DMX_IOCTL_PLAYFCT_RESETBUF           _IOW(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0x1), hi_handle)
#define DMX_IOCTL_PLAYFCT_STARTINDEX         _IOW(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0x2), hi_handle)
#define DMX_IOCTL_PLAYFCT_RECVINDEX          _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0x3), dmx_play_index_info)
#define DMX_COMPAT_IOCTL_PLAYFCT_RECVINDEX   _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0x3), dmx_compat_play_index_info)
#define DMX_IOCTL_PLAYFCT_STOPINDEX          _IOW(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0x4), hi_handle)
#define DMX_IOCTL_PLAYFCT_PEEKBUF            _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0x5), hi_handle)
#define DMX_IOCTL_PLAYFCT_CLOSE              _IOW(HI_ID_DEMUX,  (DMX_PLAYFCT_CMD1 + 0x6), hi_handle)
#define DMX_IOCTL_PLAYFCT_DESTROY            _IOW(HI_ID_DEMUX,  (DMX_PLAYFCT_CMD1 + 0x7), hi_handle)
#define DMX_IOCTL_PLAYFCT_GETHANDLE          _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0x8), dmx_play_fct_get_handle_info)
#define DMX_IOCTL_PLAYFCT_GET_DATAHANDLE     _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0x9), dmx_get_data_flag)
#define DMX_COMPAT_IOCTL_PLAYFCT_GET_DATAHANDLE _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0x9), dmx_compat_get_data_flag)
#define DMX_IOCTL_PLAYFCT_PREMMAP            _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0xA), dmx_pre_mmap_info)
#define DMX_COMPAT_IOCTL_PLAYFCT_PREMMAP     _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0xA), dmx_compat_pre_mmap_info)
#define DMX_IOCTL_PLAYFCT_PSTMMAP            _IOW(HI_ID_DEMUX,  (DMX_PLAYFCT_CMD1 + 0xB), dmx_pst_mmap_info)
#define DMX_COMPAT_IOCTL_PLAYFCT_PSTMMAP     _IOW(HI_ID_DEMUX,  (DMX_PLAYFCT_CMD1 + 0xB), dmx_compat_pst_mmap_info)
#define DMX_IOCTL_PLAYFCT_SETEOS             _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0xC), hi_handle)
#define DMX_IOCTL_PLAYFCT_SELECT_DATAHANDLE _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0xD), dmx_select_data_flag)
#define DMX_COMPAT_IOCTL_PLAYFCT_SELECT_DATAHANDLE _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0xD), dmx_compat_select_data_flag)
#define DMX_IOCTL_PLAYFCT_GETPIDCH           _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0xE), dmx_play_fct_get_pid_ch_info)
#define DMX_IOCTL_PLAYFCT_GET_SCRAMBLEDFLAG  _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD1 + 0xF), dmx_play_fct_get_scrambled_flag_info)

#define DMX_IOCTL_PLAYFCT_GET_BUFHANDLE      _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD2 + 0x0), dmx_play_get_buf_handle)
#define DMX_IOCTL_PLAYFCT_GETPACKET          _IOWR(HI_ID_DEMUX, (DMX_PLAYFCT_CMD2 + 0x1), dmx_get_play_fct_packet_info)
#define DMX_IOCTL_PLAYFCT_CC_REPEAT_SET      _IOW(HI_ID_DEMUX, (DMX_PLAYFCT_CMD2 + 0x2), dmx_set_chan_cc_repeat_info)
#define DMX_IOCTL_PLAYFCT_PUSI_SET           _IOW(HI_ID_DEMUX, (DMX_PLAYFCT_CMD2 + 0x3), dmx_pusi_set_info)

/* recfct */
#define DMX_IOCTL_RECFCT_CREATE             _IOWR(HI_ID_DEMUX, (DMX_RECFCT_CMD0 + 0x0), dmx_create_rec_fct_info)
#define DMX_IOCTL_RECFCT_OPEN               _IOW(HI_ID_DEMUX,  (DMX_RECFCT_CMD0 + 0x1), hi_handle)
#define DMX_IOCTL_RECFCT_GETATTRS           _IOWR(HI_ID_DEMUX, (DMX_RECFCT_CMD0 + 0x2), dmx_get_rec_fct_attr_info)
#define DMX_IOCTL_RECFCT_SETATTRS           _IOW(HI_ID_DEMUX,  (DMX_RECFCT_CMD0 + 0x3), dmx_set_rec_fct_attr_info)
#define DMX_IOCTL_RECFCT_GETSTATUS          _IOWR(HI_ID_DEMUX, (DMX_RECFCT_CMD0 + 0x4), dmx_get_rec_fct_status_info)
#define DMX_IOCTL_RECFCT_ADDCH              _IOW(HI_ID_DEMUX,  (DMX_RECFCT_CMD0 + 0x5), dmx_rec_fct_add_del_ch_info)
#define DMX_IOCTL_RECFCT_DELCH              _IOW(HI_ID_DEMUX,  (DMX_RECFCT_CMD0 + 0x6), dmx_rec_fct_add_del_ch_info)
#define DMX_IOCTL_RECFCT_DELALLCH           _IOW(HI_ID_DEMUX,  (DMX_RECFCT_CMD0 + 0x7), hi_handle)
#define DMX_IOCTL_RECFCT_ACQUIREBUF         _IOWR(HI_ID_DEMUX, (DMX_RECFCT_CMD0 + 0x8), dmx_rec_fct_acquire_buf_info)
#define DMX_COMPAT_IOCTL_RECFCT_ACQUIREBUF  _IOWR(HI_ID_DEMUX, (DMX_RECFCT_CMD0 + 0x8), dmx_compat_rec_fct_acquire_buf_info)
#define DMX_IOCTL_RECFCT_RELEASEBUF         _IOW(HI_ID_DEMUX,  (DMX_RECFCT_CMD0 + 0x9), dmx_rec_fct_release_buf_info)
#define DMX_COMPAT_IOCTL_RECFCT_RELEASEBUF  _IOW(HI_ID_DEMUX,  (DMX_RECFCT_CMD0 + 0x9), dmx_compat_rec_fct_release_buf_info)
#define DMX_IOCTL_RECFCT_RECVINDEX          _IOWR(HI_ID_DEMUX, (DMX_RECFCT_CMD0 + 0xA), dmx_rec_index_info)
#define DMX_COMPAT_IOCTL_RECFCT_RECVINDEX   _IOWR(HI_ID_DEMUX, (DMX_RECFCT_CMD0 + 0xA), dmx_compat_rec_index_info)
#define DMX_IOCTL_RECFCT_PEEK_IDX_BUF       _IOWR(HI_ID_DEMUX, (DMX_RECFCT_CMD0 + 0xB), dmx_rec_peek_idx_buf_info)
#define DMX_IOCTL_RECFCT_CLOSE              _IOW(HI_ID_DEMUX,  (DMX_RECFCT_CMD0 + 0xC), hi_handle)
#define DMX_IOCTL_RECFCT_DESTROY            _IOW(HI_ID_DEMUX,  (DMX_RECFCT_CMD0 + 0xD), hi_handle)
#define DMX_IOCTL_RECFCT_PREMMAP            _IOWR(HI_ID_DEMUX, (DMX_RECFCT_CMD0 + 0xE), dmx_pre_mmap_info)
#define DMX_COMPAT_IOCTL_RECFCT_PREMMAP     _IOWR(HI_ID_DEMUX, (DMX_RECFCT_CMD0 + 0xE), dmx_compat_pre_mmap_info)
#define DMX_IOCTL_RECFCT_PSTMMAP            _IOW(HI_ID_DEMUX,  (DMX_RECFCT_CMD0 + 0xF), dmx_pst_mmap_info)
#define DMX_COMPAT_IOCTL_RECFCT_PSTMMAP     _IOW(HI_ID_DEMUX,  (DMX_RECFCT_CMD0 + 0xF), dmx_compat_pst_mmap_info)
#define DMX_IOCTL_RECFCT_SETEOS             _IOWR(HI_ID_DEMUX, (DMX_RECFCT_CMD1 + 0x0), dmx_rec_set_eos_info)
#define DMX_IOCTL_RECFCT_GETBUFHANDLE       _IOWR(HI_ID_DEMUX, (DMX_RECFCT_CMD1 + 0x1), dmx_rec_get_buf_handle)

/* dscfct */
#define DMX_IOCTL_DSCFCT_CREATE            _IOWR(HI_ID_DEMUX, (DMX_DSCFCT_CMD0 + 0x0), dmx_create_dsc_fct_info)
#define DMX_IOCTL_DSCFCT_GETATTRS          _IOWR(HI_ID_DEMUX, (DMX_DSCFCT_CMD0 + 0x1), dmx_get_dsc_fct_attr_info)
#define DMX_IOCTL_DSCFCT_SETATTRS          _IOW(HI_ID_DEMUX,  (DMX_DSCFCT_CMD0 + 0x2), dmx_set_dsc_fct_attr_info)
#define DMX_IOCTL_DSCFCT_ATTACH            _IOW(HI_ID_DEMUX,  (DMX_DSCFCT_CMD0 + 0x3), dmx_dsc_fct_attach_info)
#define DMX_IOCTL_DSCFCT_DETACH            _IOW(HI_ID_DEMUX,  (DMX_DSCFCT_CMD0 + 0x4), dmx_dsc_fct_detach_info)
#define DMX_IOCTL_DSCFCT_ATTACH_KEYSLOT    _IOW(HI_ID_DEMUX,  (DMX_DSCFCT_CMD0 + 0x5), dmx_dsc_fct_attach_info)
#define DMX_IOCTL_DSCFCT_DETACH_KEYSLOT    _IOW(HI_ID_DEMUX,  (DMX_DSCFCT_CMD0 + 0x6), dmx_dsc_fct_detach_info)
#define DMX_IOCTL_DSCFCT_GET_KS_HANDLE     _IOWR(HI_ID_DEMUX, (DMX_DSCFCT_CMD0 + 0x7), dmx_dsc_fct_get_ks_handle_info)
#define DMX_IOCTL_DSCFCT_SETEVENKEY        _IOW(HI_ID_DEMUX,  (DMX_DSCFCT_CMD0 + 0x8), dmx_dsc_fct_set_key_info)
#define DMX_IOCTL_DSCFCT_SETODDKEY         _IOW(HI_ID_DEMUX,  (DMX_DSCFCT_CMD0 + 0x9), dmx_dsc_fct_set_key_info)
#define DMX_IOCTL_DSCFCT_SETEVENIV         _IOW(HI_ID_DEMUX,  (DMX_DSCFCT_CMD0 + 0xA), dmx_dsc_fct_set_key_info)
#define DMX_IOCTL_DSCFCT_SETODDIV          _IOW(HI_ID_DEMUX,  (DMX_DSCFCT_CMD0 + 0xB), dmx_dsc_fct_set_key_info)
#define DMX_IOCTL_DSCFCT_DESTROY           _IOW(HI_ID_DEMUX,  (DMX_DSCFCT_CMD0 + 0xC), hi_handle)
#define DMX_IOCTL_DSCFCT_GET_FREE_CNT      _IOR(HI_ID_DEMUX,  (DMX_DSCFCT_CMD0 + 0xD), hi_u32)
#define DMX_IOCTL_DSCFCT_GET_KEY_HANDLE    _IOWR(HI_ID_DEMUX, (DMX_DSCFCT_CMD0 + 0xE), dmx_dsc_fct_get_key_handle_info)
#define DMX_IOCTL_DSCFCT_SETSYSKEY         _IOW(HI_ID_DEMUX,  (DMX_DSCFCT_CMD0 + 0xF), dmx_dsc_fct_set_sys_key_info)

/* pcrfct */
#define DMX_IOCTL_PCRFCT_CREATE             _IOWR(HI_ID_DEMUX, (DMX_PCRFCT_CMD + 0x0), dmx_create_pcr_fct_info)
#define DMX_IOCTL_PCRFCT_ATTACH             _IOW(HI_ID_DEMUX,  (DMX_PCRFCT_CMD + 0x1), dmx_pcr_fct_attach_info)
#define DMX_IOCTL_PCRFCT_DETACH             _IOW(HI_ID_DEMUX,  (DMX_PCRFCT_CMD + 0x2), hi_handle)
#define DMX_IOCTL_PCRFCT_GETSTATUS          _IOWR(HI_ID_DEMUX, (DMX_PCRFCT_CMD + 0x3), dmx_get_pcr_fct_status_info)
#define DMX_IOCTL_PCRFCT_GETPCRSCR          _IOWR(HI_ID_DEMUX, (DMX_PCRFCT_CMD + 0x4), dmx_pcr_fct_get_pcr_scr_info)
#define DMX_IOCTL_PCRFCT_DESTROY            _IOW(HI_ID_DEMUX,  (DMX_PCRFCT_CMD + 0x5), hi_handle)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
