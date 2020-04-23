/*
* Copyright (C) Hisilicon Technologies Co., Ltd. 2016-2019. All rights reserved.
* Description:
* Author: guoqingbo
* Create: 2016-08-12
*/

#ifndef __DRV_TSIO_IOCTL_H__
#define __DRV_TSIO_IOCTL_H__

#include "hi_drv_demux.h"
#include "hi_drv_module.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* mpi begin */
/*
 * capacity definitions.
 */
typedef struct {
    hi_u32 tsi_port_cnt;
    hi_u32 ram_port_cnt;
    hi_u32 pid_chan_cnt;
    hi_u32 raw_chan_cnt;
    hi_u32 sp_chan_cnt;
    hi_u32 ivr_chan_cnt;
    hi_u32 sec_eng_cnt;
} tsio_capability;

/*
 * port resource definitions.
 */
typedef enum {
    TSIO_ANY_TSI_PORT = 0x0,
    TSIO_TSI_PORT_0,
    TSIO_TSI_PORT_1,
    TSIO_TSI_PORT_2,
    TSIO_TSI_PORT_3,
    TSIO_TSI_PORT_4,
    TSIO_TSI_PORT_5,
    TSIO_TSI_PORT_6,
    TSIO_TSI_PORT_7,
    TSIO_TSI_PORT_BUTT,

    TSIO_ANY_RAM_PORT = 0x32,
    TSIO_RAM_PORT_0,
    TSIO_RAM_PORT_1,
    TSIO_RAM_PORT_2,
    TSIO_RAM_PORT_3,
    TSIO_RAM_PORT_4,
    TSIO_RAM_PORT_5,
    TSIO_RAM_PORT_6,
    TSIO_RAM_PORT_7,
    TSIO_RAM_PORT_8,
    TSIO_RAM_PORT_9,
    TSIO_RAM_PORT_10,
    TSIO_RAM_PORT_11,
    TSIO_RAM_PORT_12,
    TSIO_RAM_PORT_13,
    TSIO_RAM_PORT_14,
    TSIO_RAM_PORT_15,
    TSIO_RAM_PORT_BUTT,
} tsio_port;

/*
 * sid resource definitions.
 */
typedef enum {
    TSIO_SID_0 = 0,
    TSIO_SID_1,
    TSIO_SID_2,
    TSIO_SID_3,
    TSIO_SID_4,
    TSIO_SID_5,
    TSIO_SID_6,
    TSIO_SID_7,
    TSIO_SID_8,
    TSIO_SID_9,
    TSIO_SID_10,
    TSIO_SID_11,
    TSIO_SID_12,
    TSIO_SID_13,
    TSIO_SID_14,
    TSIO_SID_15,
    TSIO_SID_16,
    TSIO_SID_17,
    TSIO_SID_18,
    TSIO_SID_19,
    TSIO_SID_20,
    TSIO_SID_21,
    TSIO_SID_22,
    TSIO_SID_23,
    TSIO_SID_24,
    TSIO_SID_25,
    TSIO_SID_26,
    TSIO_SID_27,
    TSIO_SID_28,
    TSIO_SID_29,
    TSIO_SID_30,
    TSIO_SID_31,
    TSIO_SID_32,
    TSIO_SID_33,
    TSIO_SID_34,
    TSIO_SID_35,
    TSIO_SID_36,
    TSIO_SID_37,
    TSIO_SID_38,
    TSIO_SID_39,
    TSIO_SID_40,
    TSIO_SID_41,
    TSIO_SID_42,
    TSIO_SID_43,
    TSIO_SID_44,
    TSIO_SID_45,
    TSIO_SID_46,
    TSIO_SID_47,
    TSIO_SID_48,
    TSIO_SID_49,
    TSIO_SID_50,
    TSIO_SID_51,
    TSIO_SID_52,
    TSIO_SID_53,
    TSIO_SID_54,
    TSIO_SID_55,
    TSIO_SID_56,
    TSIO_SID_57,
    TSIO_SID_58,
    TSIO_SID_59,
    TSIO_SID_60,
    TSIO_SID_61,
    TSIO_SID_62,
    TSIO_SID_63,
    TSIO_SID_BUTT,
} tsio_sid;

#define TSIO_ANY_SID (-1)

/*
 * global config definitions.
 */
typedef enum {
    TSIO_BW_50M = 1,
    TSIO_BW_100M,
    TSIO_BW_200M,
    TSIO_BW_400M,
    TSIO_BW_BUTT,
} tsio_bw; /* mbps */

typedef enum {
    TSIO_SYNC_THRES_3 = 3,
    TSIO_SYNC_THRES_4,
    TSIO_SYNC_THRES_5,
    TSIO_SYNC_THRES_6,
    TSIO_SYNC_THRES_7,
    TSIO_SYNC_THRES_8,
    TSIO_SYNC_THRES_BUTT,
} tsio_sync_thres;

typedef struct {
    tsio_bw  band_width;
    tsio_sid stuff_sid;
    hi_u32   sync_thres;
} tsio_config;

#define TSIO_BW_DEFAULT            TSIO_BW_400M
#define TSIO_STUFF_SID_DEFAULT     TSIO_SID_63
#define TSIO_SYNC_THRES_DEFAULT    TSIO_SYNC_THRES_8

/*
 * tsi port definitions.
 */
typedef struct {
    dmx_port source_port;
} tsio_tsi_port_attrs;

typedef struct {
    dmx_port source_port;
    hi_u32      dmx_if_port_cnt;
    hi_u32      dmx_tsi_port_cnt;
} tsio_tsi_port_attrs_ex;

typedef struct {
    tsio_port port_id;
    hi_u32    pkt_num;
} tsio_tsi_port_status;

/*
 * ram port definitions.
 */
typedef enum {
    TSIO_SOURCE_TS,
    TSIO_SOURCE_BULK
} tsio_source_type;

typedef enum {
    TSIO_KEY_EVEN,
    TSIO_KEY_ODD,
} tsio_key_type;

typedef struct {
    hi_u8  *data;
    hi_u32 length;
    hi_mem_handle_t buf_handle;
    hi_mem_size_t buf_offset;
} tsio_buffer;

typedef struct {
    hi_u32 data;
    hi_u32 length;
    hi_mem_handle_t buf_handle;
    hi_mem_size_t buf_offset;
} tsio_compat_buffer;

typedef struct {
    tsio_source_type source_type;
    hi_u32           buffer_size;
    hi_u32           max_data_rate;  /* kbps */
} tsio_ram_port_attrs;

typedef struct {
    tsio_source_type source_type;
    hi_u32           buffer_size;
    hi_u32           max_data_rate;  /* kbps */
    hi_u32           pace;         /* kernel drv not support float operation, it need calc pace in user and pass it to drv */
} tsio_ram_port_attrs_ex;

typedef struct {
    tsio_port port_id;

    hi_u32    buffer_blk_size;
    hi_u32    buffer_used;
    hi_u32    buffer_size;

    hi_u32    pkt_num;
} tsio_ram_port_status;

typedef struct {
    hi_u32 pid;
} tsio_pid_channel_status;

/*
 * secure engine definitions.
 */
typedef enum {
    TSIO_SECURE_ENGINE_OUTPUT2DMX,
    TSIO_SECURE_ENGINE_OUTPUT2RAM,
} tsio_secure_engine_output_mod;

typedef enum {
    TSIO_SECURE_ENGINE_CLOSED,
    TSIO_SECURE_ENGINE_OPENED,
} tsio_secure_engine_state;

typedef struct {
    tsio_secure_engine_output_mod mod;

    union {
        struct {
            dmx_port dest_port;
        };

        struct {
            hi_u32 buffer_size;
        };
    };
} tsio_secure_engine_attrs;

typedef struct {
    tsio_sid                        sid;
    tsio_secure_engine_state        state;

    hi_u32                          buffer_used;
    hi_u32                          buffer_size;

    hi_u32                          overflow_cnt;
    hi_u32                          dma_end_int_cnt;
} tsio_secure_engine_status;

/*
 * CC definitions.
 */
#define MIN_CC_CMD_LEN             2       /* 2 bytes */
#define MAX_CC_CMD_LEN             258     /* 258 bytes */

/* mpi end */

#define TSIO_CMD_MASK              (0xF0)
#define TSIO_GBL_CMD               (0x00)
#define TSIO_TSI_PORT_CMD          (0x10)
#define TSIO_RAM_PORT_CMD          (0x20)
#define TSIO_CHAN_CMD              (0x30)
#define TSIO_SE_CMD                (0x40)
#define TSIO_CC_CMD                (0x50)


typedef struct {
    tsio_config config;             /* [in] */
} tsio_config_info;

#define TSIO_IOCTL_GBL_GETCFG          _IOWR(HI_ID_TSIO, 0x0, tsio_config_info)
#define TSIO_IOCTL_GBL_SETCFG          _IOW(HI_ID_TSIO, 0x1, tsio_config_info)
#define TSIO_IOCTL_GBL_GETCAP          _IOWR(HI_ID_TSIO, 0x2, tsio_capability)
#define TSIO_IOCTL_GBL_GETOUTOFSYNCCNT _IOWR(HI_ID_TSIO, 0x3, hi_u32)

typedef struct {
    tsio_port           port;      /* [in] */
    tsio_tsi_port_attrs attrs;     /* [in] */
    hi_handle           handle;    /* [out] */
} tsio_open_tsi_port_info;

typedef struct {
    hi_handle           handle;    /* [in] */
    tsio_tsi_port_attrs attrs;     /* [out] */
} tsio_get_tsi_port_attr_info;

typedef tsio_get_tsi_port_attr_info tsio_set_tsi_port_attr_info;

typedef struct {
    hi_handle            handle;    /* [in] */
    tsio_tsi_port_status status;    /* [out] */
} tsio_get_tsi_port_status_info;

#define TSIO_IOCTL_TSI_OPENPORT        _IOWR(HI_ID_TSIO, 0x10, tsio_open_tsi_port_info)
#define TSIO_IOCTL_TSI_GETPORTATTRS    _IOWR(HI_ID_TSIO, 0x11, tsio_get_tsi_port_attr_info)
#define TSIO_IOCTL_TSI_SETPORTATTRS    _IOW(HI_ID_TSIO,  0x12, tsio_set_tsi_port_attr_info)
#define TSIO_IOCTL_TSI_GETPORTSTATUS   _IOWR(HI_ID_TSIO, 0x13, tsio_get_tsi_port_status_info)
#define TSIO_IOCTL_TSI_CLOSEPORT       _IOW(HI_ID_TSIO,  0x14, hi_handle)


typedef struct {
    tsio_port              port;      /* [in] */
    tsio_ram_port_attrs_ex attrs;     /* [in] */
    hi_handle              handle;    /* [out] */
} tsio_open_ram_port_info;

typedef struct {
    hi_handle              handle;    /* [in] */
    tsio_ram_port_attrs    attrs;     /* [out] */
} tsio_get_ram_port_attr_info;

typedef struct {
    hi_handle              handle;    /* [in] */
    tsio_ram_port_attrs_ex attrs;     /* [out] */
} tsio_set_ram_port_attr_info;

typedef struct {
    hi_handle            handle;    /* [in] */
    tsio_ram_port_status status;    /* [out] */
} tsio_get_ram_port_status_info;

typedef struct {
    hi_handle            handle;         /* [in] */
    tsio_key_type        key;            /* [in] */
    hi_u8                iv[16];         /* [in] */
} tsio_begin_bulk_seg_info;

typedef struct {
    hi_handle            handle;    /* [in] */
    hi_u32               req_len;    /* [in] */
    hi_u32               time_out;   /* [in] */
    tsio_buffer          buf;       /* [out] */
} tsio_get_ram_buffer_info;

typedef struct {
    hi_handle            handle;    /* [in] */
    hi_u32               req_len;    /* [in] */
    hi_u32               time_out;   /* [in] */
    tsio_compat_buffer   buf;       /* [out] */
} tsio_compat_get_ram_buffer_info;

typedef struct {
    hi_handle            handle;    /* [in] */
    tsio_buffer          buf;       /* [in] */
} tsio_push_ram_buffer_info;

typedef struct {
    hi_handle            handle;    /* [in] */
    tsio_compat_buffer   buf;       /* [out] */
} tsio_compat_push_ram_buffer_info;

typedef tsio_push_ram_buffer_info tsio_put_ram_buffer_info;

typedef tsio_compat_push_ram_buffer_info tsio_compat_put_ram_buffer_info;

typedef struct {
    hi_handle            handle;        /* [in] */
    hi_handle            buf_handle;    /* [out] */
    hi_u32               buf_size;      /* [out] */
    hi_void              *usr_addr;     /* [out] */
} tsio_pre_mmap_info;

typedef struct {
    hi_handle            handle;        /* [in] */
    hi_handle            buf_handle;    /* [out] */
    hi_u32               buf_size;      /* [out] */
    hi_u32               usr_addr;      /* [out] */
} tsio_compat_pre_mmap_info;

typedef struct {
    hi_handle            handle;    /* [in] */
    hi_void              *usr_addr;  /* [in] */
} tsio_pst_mmap_info;

typedef struct {
    hi_handle            handle;    /* [in] */
    hi_u32               usr_addr;  /* [in] */
} tsio_compat_pst_mmap_info;

#define TSIO_IOCTL_RAM_OPENPORT               _IOWR(HI_ID_TSIO, 0x20, tsio_open_ram_port_info)
#define TSIO_IOCTL_RAM_GETPORTATTRS           _IOWR(HI_ID_TSIO, 0x21, tsio_get_ram_port_attr_info)
#define TSIO_IOCTL_RAM_SETPORTATTRS           _IOW(HI_ID_TSIO,  0x22, tsio_set_ram_port_attr_info)
#define TSIO_IOCTL_RAM_GETPORTSTATUS          _IOWR(HI_ID_TSIO, 0x23, tsio_get_ram_port_status_info)
#define TSIO_IOCTL_RAM_BEGINBULKSEG           _IOW(HI_ID_TSIO,  0x24, tsio_begin_bulk_seg_info)
#define TSIO_IOCTL_RAM_ENDBULKSEG             _IOW(HI_ID_TSIO,  0x25, hi_handle)
#define TSIO_IOCTL_RAM_GETBUFFER              _IOWR(HI_ID_TSIO, 0x26, tsio_get_ram_buffer_info)
#define TSIO_COMPAT_IOCTL_RAM_GETBUFFER       _IOWR(HI_ID_TSIO, 0x26, tsio_compat_get_ram_buffer_info)
#define TSIO_IOCTL_RAM_PUSHBUFFER             _IOW(HI_ID_TSIO,  0x27, tsio_push_ram_buffer_info)
#define TSIO_COMPAT_IOCTL_RAM_PUSHBUFFER      _IOW(HI_ID_TSIO,  0x27, tsio_compat_push_ram_buffer_info)
#define TSIO_IOCTL_RAM_PUTBUFFER              _IOW(HI_ID_TSIO,  0x28, tsio_put_ram_buffer_info)
#define TSIO_COMPAT_IOCTL_RAM_PUTBUFFER       _IOW(HI_ID_TSIO,  0x28, tsio_compat_put_ram_buffer_info)
#define TSIO_IOCTL_RAM_RESETBUFFER            _IOW(HI_ID_TSIO,  0x29, hi_handle)
#define TSIO_IOCTL_RAM_CLOSEPORT              _IOW(HI_ID_TSIO,  0x2a, hi_handle)
#define TSIO_IOCTL_RAM_PREMMAP                _IOWR(HI_ID_TSIO, 0x2b, tsio_pre_mmap_info)
#define TSIO_COMPAT_IOCTL_RAM_PREMMAP        _IOWR(HI_ID_TSIO, 0x2b, tsio_compat_pre_mmap_info)
#define TSIO_IOCTL_RAM_PSTMMAP                _IOW(HI_ID_TSIO,  0x2c, tsio_pst_mmap_info)
#define TSIO_COMPAT_IOCTL_RAM_PSTMMAP         _IOW(HI_ID_TSIO,  0x2c, tsio_compat_pst_mmap_info)


typedef struct {
    hi_handle           pt_handle;   /* [in] */
    hi_u32              pid;        /* [in] */
    hi_handle           handle;     /* [out] */
} tsio_create_pid_channel_info;

typedef struct {
    hi_handle               handle; /* [in] */
    tsio_pid_channel_status status; /* [out] */
} tsio_get_pid_channel_status_info;

#define TSIO_IOCTL_PID_CREATECHANNEL    _IOWR(HI_ID_TSIO, 0x30, tsio_create_pid_channel_info)
#define TSIO_IOCTL_PID_GETCHANNELSTATUS _IOWR(HI_ID_TSIO, 0x31, tsio_get_pid_channel_status_info)
#define TSIO_IOCTL_PID_DESTROYCHANNEL   _IOW(HI_ID_TSIO,  0x32, hi_handle)

typedef struct {
    hi_handle           pt_handle;   /* [in] */
    hi_handle           handle;     /* [out] */
} tsio_create_raw_channel_info;

#define TSIO_IOCTL_RAW_CREATECHANNEL    _IOWR(HI_ID_TSIO, 0x33, tsio_create_raw_channel_info)
#define TSIO_IOCTL_RAW_DESTROYCHANNEL   _IOW(HI_ID_TSIO,  0x34, hi_handle)

typedef struct {
    hi_handle           pt_handle;   /* [in] */
    hi_handle           handle;     /* [out] */
} tsio_create_sp_channel_info;

#define TSIO_IOCTL_SP_CREATECHANNEL    _IOWR(HI_ID_TSIO, 0x35, tsio_create_sp_channel_info)
#define TSIO_IOCTL_SP_DESTROYCHANNEL   _IOW(HI_ID_TSIO,  0x36, hi_handle)

typedef struct {
    hi_handle           pt_handle;   /* [in] */
    hi_handle           handle;     /* [out] */
} tsio_create_ivr_channel_info;

#define TSIO_IOCTL_IVR_CREATECHANNEL    _IOWR(HI_ID_TSIO, 0x37, tsio_create_ivr_channel_info)
#define TSIO_IOCTL_IVR_DESTROYCHANNEL   _IOW(HI_ID_TSIO,  0x38, hi_handle)


typedef struct {
    tsio_sid                 sid;       /* [in] */
    tsio_secure_engine_attrs attrs;     /* [in] */
    hi_handle                handle;    /* [out] */
} tsio_create_se_info;

typedef struct {
    hi_handle                handle;    /* [in] */
    tsio_secure_engine_attrs attrs;     /* [out] */
} tsio_get_se_attr_info;

typedef tsio_get_se_attr_info tsio_set_se_attr_info;

typedef struct {
    hi_handle                 handle;    /* [in] */
    tsio_secure_engine_status status;    /* [out] */
} tsio_get_se_status_info;;

typedef struct {
    hi_handle                 handle;    /* [in] */
    hi_handle                 ch_handle;  /* [in] */
} tsio_add_se_channel_info;

typedef tsio_add_se_channel_info tsio_del_se_channel_info;

typedef struct {
    hi_handle                 handle;    /* [in] */
    hi_u32                    req_len;    /* [in] */
    hi_u32                    time_out;   /* [in] */
    tsio_buffer               buf;       /* [out] */
} tsio_acq_se_buffer_info;

typedef struct {
    hi_handle                 handle;    /* [in] */
    hi_u32                    req_len;    /* [in] */
    hi_u32                    time_out;   /* [in] */
    tsio_compat_buffer        buf;       /* [out] */
} tsio_compat_acq_se_buffer_info;

typedef tsio_acq_se_buffer_info tsio_rel_se_buffer_info;

typedef tsio_compat_acq_se_buffer_info tsio_compat_rel_se_buffer_info;

#define TSIO_IOCTL_SE_CREATE            _IOWR(HI_ID_TSIO, 0x40, tsio_create_se_info)
#define TSIO_IOCTL_SE_OPEN              _IOW(HI_ID_TSIO,  0x41, hi_handle)
#define TSIO_IOCTL_SE_GETATTRS          _IOWR(HI_ID_TSIO, 0x42, tsio_get_se_attr_info)
#define TSIO_IOCTL_SE_SETATTRS          _IOW(HI_ID_TSIO,  0x43, tsio_set_se_attr_info)
#define TSIO_IOCTL_SE_GETSTATUS         _IOWR(HI_ID_TSIO, 0x44, tsio_get_se_status_info)
#define TSIO_IOCTL_SE_ADDCHANNEL        _IOW(HI_ID_TSIO,  0x45, tsio_add_se_channel_info)
#define TSIO_IOCTL_SE_DELCHANNEL        _IOW(HI_ID_TSIO,  0x46, tsio_del_se_channel_info)
#define TSIO_IOCTL_SE_DELALLCHANNELS    _IOW(HI_ID_TSIO,  0x47, hi_handle)
#define TSIO_IOCTL_SE_ACQBUFFER         _IOWR(HI_ID_TSIO, 0x48, tsio_acq_se_buffer_info)
#define TSIO_COMPAT_IOCTL_SE_ACQBUFFER  _IOWR(HI_ID_TSIO, 0x48, tsio_compat_acq_se_buffer_info)
#define TSIO_IOCTL_SE_RELBUFFER         _IOW(HI_ID_TSIO,  0x49, tsio_rel_se_buffer_info)
#define TSIO_COMPAT_IOCTL_SE_RELBUFFER  _IOW(HI_ID_TSIO,  0x49, tsio_compat_rel_se_buffer_info)
#define TSIO_IOCTL_SE_CLOSE             _IOW(HI_ID_TSIO,  0x4a, hi_handle)
#define TSIO_IOCTL_SE_DESTROY           _IOW(HI_ID_TSIO,  0x4b, hi_handle)
#define TSIO_IOCTL_SE_PREMMAP           _IOWR(HI_ID_TSIO, 0x4c, tsio_pre_mmap_info)
#define TSIO_COMPAT_IOCTL_SE_PREMMAP   _IOWR(HI_ID_TSIO, 0x4c, tsio_compat_pre_mmap_info)
#define TSIO_IOCTL_SE_PSTMMAP           _IOW(HI_ID_TSIO,  0x4d, tsio_pst_mmap_info)
#define TSIO_COMPAT_IOCTL_SE_PSTMMAP    _IOW(HI_ID_TSIO,  0x4d, tsio_compat_pst_mmap_info)


typedef struct {
    hi_uchar                  cmd[MAX_CC_CMD_LEN + 1];  /* [in] */
    hi_u32                    cmd_len;                   /* [in] */
    hi_uchar                  resp[MAX_CC_CMD_LEN + 1]; /* [out */
    hi_u32                    resp_len;                  /* [in/out] */
    hi_u32                    time_out;                  /* [in] */
} tsio_send_ccmd_info;

#define TSIO_IOCTL_CC_SEND              _IOWR(HI_ID_TSIO, 0x50, tsio_send_ccmd_info)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // __DRV_TSIO_IOCTL_H__
