/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: hal vi
 * Author: sdk
 * Create: 2019-12-14
 */

#ifndef __HAL_VICAP_H__
#define __HAL_VICAP_H__

#include "hi_reg_common.h"
#include "hi_drv_video.h"
#include "hi_drv_pq.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
    HI_DRV_VICAP_PORT0 = 0,
    HI_DRV_VICAP_PORT1,
    HI_DRV_VICAP_PORT_MAX,
} hi_drv_vicap_port;

typedef enum {
    VICAP_DRV_PT_SKIP_TYPE_OFF = 0xFF,
    VICAP_DRV_PT_SKIP_TYPE_2X = 0xAA,
    VICAP_DRV_PT_SKIP_TYPE_4X = 0X11,

    VICAP_DRV_PT_SKIP_TYPE_MAX,
} vicap_drv_pt_skip_type;

typedef enum {
    VICAP_DRV_FRMTYPE_EYE_LEFT = 0,
    VICAP_DRV_FRMTYPE_EYE_RIGHT,

    VICAP_DRV_FRMTYPE_EYE_MAX,
} vicap_drv_frmtype;

typedef enum {
    VICAP_DRV_CHN_CROP_CROP0 = 0,
    VICAP_DRV_CHN_CROP_CROP1,

    VICAP_DRV_CHN_CROP_MAX,
} vicap_drv_chn_crop;

typedef enum {
    VICAP_PROC_SELF_GEN_TIMING_ON = 0,
    VICAP_PROC_SELF_GEN_TIMING_OFF,

    VICAP_PROC_SELF_GEN_TIMING_MAX,
} vicap_proc_self_gen_timing;

#define VICAP_DRV_WAIT_INT_MAX 2000
#define VICAP_DRV_WAIT_INT_SLEEP 5
#define VICAP_DRV_DELAY_TIME_US 20
#define VICAP_DRV_WAIT_SMMU_SLEEP 10
#define VICAP_DRV_WAIT_SMMU_DELAY 1

#define vicap_drv_chn_data_mask(a) ((0xffff << (16 - (a))) & 0xffff) /* 通道数据掩码 */

typedef enum {
    VICAP_DRV_CHN_SKIP_CFG_OFF = 0xFFFFFFFF, /* 不过采样的skip配置 */
    VICAP_DRV_CHN_SKIP_CFG_2X = 0xAAAAAAAA,  /* 2倍过采样的skip配置 */
    VICAP_DRV_CHN_SKIP_CFG_4X = 0x11111111,  /* 4倍过采样的skip配置 */
    VICAP_DRV_CHN_SKIP_CFG_8X = 0x01010101,  /* 8倍过采样的skip配置 */

    VICAP_DRV_CHN_SKIP_CFG_YUV420 = 0xAA00AA00,
    VICAP_DRV_CHN_SKIP_CFG_MAX,
} vicap_drv_chn_skip_cfg;

#define VICAP_HAL_REGS_ADDR 0x00FC0000
#define VICAP_HAL_REGS_SIZE 0x40000
#define VICAP_HAL_REGS_PT_SIZE 0x100

/* 通道中断掩码,掩码0使能场/帧起始中断，掩码1屏蔽场/帧起始中断 */
#define VICAP_HAL_CHN_INT_MASK_STRAT (VICAP_HAL_CHN_INT_FRAMESTART)
#define VICAP_HAL_CHN_INT_MASK_DELAY (VICAP_HAL_CHN_INT_FSTARTDELAY)
#define VICAP_HAL_CHN_INT_MASK0 (VICAP_HAL_CHN_INT_FRAMESTART | VICAP_HAL_CHN_INT_FSTARTDELAY)
#define VICAP_HAL_CHN_INT_MASK1 \
    (VICAP_HAL_CHN_INT_BUFOVF | VICAP_HAL_CHN_INT_FIELDTHROW | VICAP_HAL_CHN_INT_CBUSERR | VICAP_HAL_CHN_INT_YBUSERR)

#define VICAP_HAL_SMMU_INT_MASK                                                           \
    ~(VICAP_HAL_SMMU_INT_BIT_NS_TLBINVALID_WR | VICAP_HAL_SMMU_INT_BIT_NS_TLBINVALID_RD | \
      VICAP_HAL_SMMU_INT_BIT_NS_PTW_TRANS | VICAP_HAL_SMMU_INT_BIT_NS_TLBMISS)

/* Channel field/frame interrupt mask */
#define VICAP_HAL_CHN_INT_MASK_ERR (VICAP_HAL_CHN_INT_BUFOVF | VICAP_HAL_CHN_INT_FIELDTHROW)

/*********************************vicap hal common enum and struct********************************/
typedef enum {
    VICAP_HAL_MIX_MODE_MODE0 = 0,
    VICAP_HAL_MIX_MODE_MODE1,

    VICAP_HAL_MIX_MODE_MAX,
} vicap_hal_mix_mode;

typedef enum {
    VICAP_HAL_MIX_MODE_OE_MODE = 0,
    VICAP_HAL_MIX_MODE_EO_MODE,

    VICAP_HAL_MIX_MODE_OE_MAX,
} vicap_hal_mix_oe_mode;

typedef enum {
    VICAP_HAL_OUTSTANDING_ONE = 1,
    VICAP_HAL_OUTSTANDING_TWO,
    VICAP_HAL_OUTSTANDING_THREE,
    VICAP_HAL_OUTSTANDING_FOUR,
    VICAP_HAL_OUTSTANDING_FIVE,
    VICAP_HAL_OUTSTANDING_SIX,
    VICAP_HAL_OUTSTANDING_SEVEN,
    VICAP_HAL_OUTSTANDING_EIGHT,

    VICAP_HAL_OUTSTANDING_MAX,
} vicap_hal_outstanding;

typedef enum {
    VICAP_HAL_PT_CLK_TVD = 0,    /* Vicap系统时钟为TVD */
    VICAP_HAL_PT_CLK_HDDEC,      /* Vicap系统时钟为HDDEC */
    VICAP_HAL_PT_CLK_HDMI_CTRL0, /* Vicap系统时钟为HDMI_CTRL0,HDMIP2_0 */
    VICAP_HAL_PT_CLK_HDMI_CTRL1, /* Vicap系统时钟为HDMI_CTRL1,HDMIP1_4 */

    VICAP_HAL_PT_CLK_HDMI_CTRL0_CTRL1_OE, /* Vicap系统时钟为HDMI奇偶模式 */
    VICAP_HAL_PT_CLK_HDMI_CTRL0_CTRL1_LR, /* Vicap系统时钟为HDMI左右模式 */

    VICAP_HAL_PT_CLK_MAX,
} vicap_hal_pt_clk_sel;

/* VICAP工作时钟 */
typedef enum {
    VICAP_HAL_CHN_WORKHZ_648HZ = 0, /* VICAP工作时钟 648MHZ */
    VICAP_HAL_CHN_WORKHZ_600HZ,     /* VICAP工作时钟 600MHZ */
    VICAP_HAL_CHN_WORKHZ_392HZ,     /* VICAP工作时钟 392MHZ */
    VICAP_HAL_CHN_WORKHZ_0HZ,

    VICAP_HAL_CHN_WORKHZ_MAX,
} vicap_hal_chn_workhz;

/* HDMI时钟 */
typedef enum {
    VICAP_HAL_CHN_HDMIRX_WORKHZ_NORMAL = 0, /* not special */
    VICAP_HAL_CHN_HDMIRX_WORKHZ_SPECIAL,    /* for 4k@60 422 444 */

    VICAP_HAL_CHN_HDMIRX_WORKHZ_MAX,
} vicap_hal_chn_hdmirx_workhz;

typedef enum {
    VICAP_HAL_FRAME_FILED_TOP = 0,
    VICAP_HAL_FRAME_FILED_BOTTOM,
    VICAP_HAL_FRAME_FILED_FRAME,

    VICAP_HAL_FRAME_FILED_MAX,
} vicap_hal_frame_filed;

/*************************************vicap hal pt enum and struct*********************************/
typedef enum {
    VICAP_HAL_PT_FIELD_SEL_FIELD = 0,   /* 奇偶场信号 */
    VICAP_HAL_PT_FIELD_SEL_VSYNC,       /* 场同步信号 */
    VICAP_HAL_PT_FIELD_SEL_VSYNC_HSYNC, /* 根据vsync和hsync的关系检测 */
    VICAP_HAL_PT_FIELD_SEL_ZERO,        /* FIELD信号始终为0 */

    VICAP_HAL_PT_FIELD_SEL_MAX,
} vicap_hal_pt_field_sel;

typedef enum {
    VICAP_HAL_PT_VSYNC_MODE_OFF = 0,       /* 不处理 */
    VICAP_HAL_PT_VSYNC_MODE_SINGLE_UP,     /* 上升沿触发一次为一个VSYNC */
    VICAP_HAL_PT_VSYNC_MODE_SINGLE_DOUBLE, /* 双沿模式触发一次为一个VSYNC */

    VICAP_HAL_PT_VSYNC_MODE_MAX,
} vicap_hal_pt_vsync_mode;

typedef enum {
    VICAP_HAL_PT_VSYNC_SEL_VSYNC = 0, /* 场同步信号 */
    VICAP_HAL_PT_VSYNC_SEL_FIELD,     /* 奇偶场信号 */
    VICAP_HAL_PT_VSYNC_SEL_ZERO,      /* VSYNC信号始终为0 */

    VICAP_HAL_PT_VSYNC_SEL_MAX,
} vicap_hal_pt_vsync_sel;

typedef enum {
    VICAP_HAL_PT_HSYNC_MODE_OFF = 0,   /* 不处理 */
    VICAP_HAL_PT_HSYNC_MODE_SINGLE_UP, /* 上升沿触发一次为一个HSYNC */

    VICAP_HAL_PT_HSYNC_MODE_MAX,
} vicap_hal_pt_hsync_mode;

typedef enum {
    VICAP_HAL_PT_HSYNC_AND_OFF = 0,  /* 不处理 */
    VICAP_HAL_PT_HSYNC_AND_VSYNCINV, /* hsync与vsync极性相与 */
    VICAP_HAL_PT_HSYNC_XOR_VSYNCINV, /* hsync与vsync极性异或 */

    VICAP_HAL_PT_HSYNC_AND_MAX,
} vicap_hal_pt_hsync_and;

typedef enum {
    VICAP_HAL_PT_HSYNC_SEL_HSYNC = 0, /* 水平同步信号 */
    VICAP_HAL_PT_HSYNC_SEL_DE,        /* 数据有效信号 */
    VICAP_HAL_PT_HSYNC_SEL_ZERO,      /* HSYNC信号始终为0 */

    VICAP_HAL_PT_HSYNC_SEL_MAX,
} vicap_hal_pt_hsync_sel;

typedef enum {
    VICAP_HAL_PT_DE_SEL_DE = 0,    /* 数据有效信号 */
    VICAP_HAL_PT_DE_SEL_HSYNC_AND, /* hsync与vsync极性处理后的结果 */
    VICAP_HAL_PT_DE_SEL_ONE,       /* HSYNC信号始终为1 */
    VICAP_HAL_PT_DE_SEL_ZERO,      /* HSYNC信号始终为0 */

    VICAP_HAL_PT_DE_SEL_MAX,
} vicap_hal_pt_de_sel;

typedef enum {
    VICAP_HAL_PT_INDEX_PT0 = 0,
    VICAP_HAL_PT_INDEX_PT1,
    VICAP_HAL_PT_INDEX_PT2,
    VICAP_HAL_PT_INDEX_PT3,
    VICAP_HAL_PT_INDEX_PT4,
    VICAP_HAL_PT_INDEX_PT5,
    VICAP_HAL_PT_INDEX_PT6,
    VICAP_HAL_PT_INDEX_PT7,

    VICAP_HAL_PT_INDEX_MAX,
} vicap_hal_pt_index;

typedef enum {
    VICAP_HAL_PT_INTF_MODE_FVHDE = 0,
    VICAP_HAL_PT_INTF_MODE_BT656,

    VICAP_HAL_PT_INTF_MODE_MAX
} vicap_hal_pt_intf_mode;

typedef enum {
    VICAP_HAL_PT_GEN_MODE_FVHDE = 0, /*  Dependent on external timing */
    VICAP_HAL_PT_GEN_MODE_AUTO,      /* Automatic generation */

    HAL_PT_GEN_MODE_MAX,
} vicap_hal_pt_gen_mode;

/* port 输入时序FVHDE配置 */
typedef struct {
    hi_bool field_inv;                  /* 奇偶场信号的极性 */
    vicap_hal_pt_field_sel field_sel;   /* 奇偶场信号的选择 */
    vicap_hal_pt_vsync_mode vsync_mode; /* 垂直同步处理模式 */
    hi_bool vsync_inv;                  /* 垂直同步信号极性 */
    vicap_hal_pt_vsync_sel vsync_sel;   /* 垂直同步信号的选择 */
    vicap_hal_pt_hsync_mode hsync_mode; /* 行同步处理模式 */
    vicap_hal_pt_hsync_and hsync_and;   /* 行同步与场同步极性的处理 */
    hi_bool hsync_inv;                  /* 行同步信号的极性 */
    vicap_hal_pt_hsync_sel hsync_sel;   /* 行同步信号的选择 */
    hi_bool de_inv;                     /* 数据有效信号的极性 */
    vicap_hal_pt_de_sel de_sel;         /* 数据有效信号的选择 */
} vicap_hal_pt_sync_cfg;

typedef struct {
    hi_bool enable;
    hi_bool recover_vsync;
    hi_bool recover_hsync;
    vicap_hal_pt_gen_mode gen_mode;
} vicap_hal_pt_gentiming_cfg;

typedef struct {
    hi_bool enable;
    hi_u32 gen_mode;
    hi_bool data0_move;
    hi_bool data1_move;
    hi_bool data2_move;
    hi_bool vsync_reset;
    hi_bool hsync_reset;
    hi_bool vsync_move;
    hi_bool hsync_move;
    hi_bool de_move;
} vicap_hal_pt_gendata_cfg;

typedef enum {
    VICAP_HAL_PT_COMP_MODE_SINGLE = 0, /* 单分量 */
    VICAP_HAL_PT_COMP_MODE_DOUBLE,     /* 双分量 */
    VICAP_HAL_PT_COMP_MODE_THREE,      /* 三分量 */

    VICAP_HAL_PT_COMP_MODE_MAX,
} vicap_hal_pt_comp_mode;

typedef enum {
    VICAP_HAL_PT_GENDATA_TYPE_OFF = 0,
    VICAP_HAL_PT_GENDATA_TYPE_DEFAULT,
    VICAP_HAL_PT_GENDATA_TYPE_PURECOLOUR,
    VICAP_HAL_PT_GENDATA_TYPE_MOVE,

    VICAP_HAL_PT_GENDATA_TYPE_MAX,
} vicap_hal_pt_gendata_type;

typedef struct {
    hi_u32 inc_frame;
    hi_u32 step_frame;
    hi_u32 inc_space;
    hi_u32 step_space;
} vicap_hal_pt_gendata_coef;

typedef struct {
    hi_u32 data0;
    hi_u32 data1;
    hi_u32 data2;
} vicap_hal_pt_gendata_init;

/* port Blanking information of input data */
typedef struct {
    hi_u32 hfb;   /* Horizontal blanking zone width */
    hi_u32 h_act; /* Horizontal effective width */
    hi_u32 hbb;   /* Horizontal back blanking zone width */
    hi_u32 vfb;   /* Vertical front blanking zone height */
    hi_u32 v_act; /* Vertical effective zone width */
    hi_u32 vbb;   /* Vertical back blanking zone width */
    hi_u32 vbfb;  /* Vertical front blanking zone width */
    hi_u32 vbact; /* Vertical back effective zone width */
    hi_u32 vbbb;  /* Vertical back blanking zone width */
} vicap_hal_pt_gentiming_info;

/*************************************vicap hal chn enum and struct*********************************/
typedef enum {
    VICAP_HAL_CHN_CSEQ_CRCB = 0,
    VICAP_HAL_CHN_CSEQ_CBCR,

    VICAP_HAL_CHN_CSEQ_MAX,
} vicap_hal_chn_cseq;

typedef enum {
    VICAP_HAL_DITHER_MODE_DITHER = 0, /* 随机数模式 */
    VICAP_HAL_DITHER_MODE_ROUND,      /* 四舍五入模式 */

    VICAP_HAL_DITHER_MODE_MAX,
} vicap_hal_dither_mode;

typedef struct {
    hi_u32 width;
    hi_u32 height;
} vicap_hal_frame_size;

typedef struct {
    hi_bool enable;
    vicap_hal_chn_cseq c_seq;
} vicap_hal_chn_420_cfg;

typedef struct {
    hi_u32 hcount;
    hi_u32 vcount;
} vicap_hal_chn_count;

typedef struct {
    hi_bool enable;
    hi_bool fpk_interlace_mode; /* v620,标识是否是framepackage 隔行格式 */
    hi_bool mmu_bypass;         /* v620,MMU选择模式 */
    vicap_hal_frame_filed field;
    hi_bool interleave;
    hi_drv_pixel_bitwidth dst_bit_width;
} vicap_hal_chn_wch_cfg;

typedef struct {
    hi_u32 y_addr;
    hi_u32 c_addr;
} vicap_hal_chn_wch_faddr;

typedef struct {
    hi_u32 addr;
    hi_u32 length;
} vicap_hal_chn_dolby_wch_faddr;

/* 通道裁剪配置 */
typedef struct {
    hi_bool crop_en[VICAP_DRV_FRMTYPE_EYE_MAX];       /* 通道有两个裁剪区域的开关 */
    hi_drv_rect crop_rect[VICAP_DRV_FRMTYPE_EYE_MAX]; /* 通道有两个裁剪区域 */
} vicap_hal_chn_crop_cfg;

/* CSC 矩阵系数结构 */
typedef struct {
    hi_s32 csc_coef00;
    hi_s32 csc_coef01;
    hi_s32 csc_coef02;

    hi_s32 csc_coef10;
    hi_s32 csc_coef11;
    hi_s32 csc_coef12;

    hi_s32 csc_coef20;
    hi_s32 csc_coef21;
    hi_s32 csc_coef22;
} vicap_hal_chn_csc_coef;

typedef struct {
    hi_s32 csc_in_dc0;
    hi_s32 csc_in_dc1;
    hi_s32 csc_in_dc2;

    hi_s32 csc_out_dc0;
    hi_s32 csc_out_dc1;
    hi_s32 csc_out_dc2;
} vicap_hal_chn_csc_dccoef;

typedef struct {
    hi_u32 width;
    hi_u32 height;
} vicap_hal_chn_skip_win;

typedef enum {
    VICAP_HAL_CHN_DITHER_10BIT = 0, /* 10bit输出 */
    VICAP_HAL_CHN_DITHER_8BIT,      /* 8bit输出 */

    VICAP_DITHER_MAX,
} vicap_hal_chn_dither_cfg;

/* 缩放属性针对对象 */
typedef enum {
    VICAP_HAL_CHN_SCALE_OBJ_YH = 0,
    VICAP_HAL_CHN_SCALE_OBJ_CH,

    VICAP_HAL_CHN_SCALE_OBJ_MAX,
} vicap_hal_chn_scale_obj;

/* 缩放模式 */
typedef enum {
    VICAP_HAL_CHN_HSCALE_MODE_FILT = 0,
    VICAP_HAL_CHN_HSCALE_MODE_COPY,

    VICAP_HAL_CHN_HSCALE_MODE_MAX,
} vicap_hal_chn_hscale_mode;

typedef struct {
    hi_u32 ratio;
    vicap_hal_chn_hscale_mode chn_hscale_mode;
    hi_bool mid_en;
    hi_bool scale_en;
} vicap_hal_chn_hscale_cfg;

typedef struct {
    hi_u32 y_in_pixel; /* 输入图像Y宽度 */
    hi_u32 y_in_line;  /* 输入图像Y高度 */
    hi_u32 c_in_pixel; /* 输入图像Y宽度 */
    hi_u32 c_in_line;  /* 输入图像Y高度 */
} vicap_hal_chn_scale_in_size;

typedef struct {
    hi_u32 y_out_pixel; /* 输出图像Y宽度 */
    hi_u32 y_out_line;  /* 输入图像Y高度 */
    hi_u32 c_out_pixel; /* 输出图像Y宽度 */
    hi_u32 c_out_line;  /* 输入图像Y高度 */
} vicap_hal_chn_scale_out_size;

typedef struct {
    hi_u32 width;
    hi_u32 height;
} vicap_hal_chn_dstsize;

typedef enum {
    VICAP_HAL_CHN_UPDATE_SCALE_TYPE_PARAM = 0, /* 更新除缩放系数外的其它缩放参数 */
    VICAP_HAL_CHN_UPDATE_SCALE_TYPE_COEF,      /* 更新缩放系数 */

    VICAP_HAL_CHN_UPDATE_SCALE_TYPE_MAX,
} vicap_hal_chn_update_scale_type;

typedef enum {
    VICAP_HAL_CHN_SCALE_COEF_NO = 0,
    VICAP_HAL_CHN_SCALE_COEF_2X,
    VICAP_HAL_CHN_SCALE_COEF_3X,
    VICAP_HAL_CHN_SCALE_COEF_4X,
    VICAP_HAL_CHN_SCALE_COEF_OTHER,

    VICAP_HAL_CHN_SCALE_COEF_MAX,
} vicap_hal_chn_scale_coef;

typedef enum {
    VICAP_HAL_CHN_SCALE_COEF_TYPE_SETTING = 0,
    VICAP_HAL_CHN_SCALE_COEF_TYPE_USING,

    VICAP_HAL_CHN_SCALE_COEF_TYPE_MAX,
} vicap_hal_chn_scale_coef_type;

typedef enum {
    VICAP_HAL_CHN_HFIR_RATIO_NO = (1 << 20),         /* 无水平缩放倍数为2^20 */
    VICAP_HAL_CHN_HFIR_RATIO_2X = ((1 << 20) * 2),   /* 水平缩放倍数为2^20*2 */
    VICAP_HAL_CHN_HFIR_RATIO_3X = ((1 << 20) * 3),   /* 水平缩放倍数为2^20*3 */
    VICAP_HAL_CHN_HFIR_RATIO_4X = ((1 << 20) * 4),   /* 水平缩放倍数为2^20*4 */
    VICAP_HAL_CHN_HFIR_RATIO_16X = ((1 << 20) * 16), /* 水平缩放倍数为2^20*16 */

    VICAP_HAL_CHN_HFIR_RATIO_MAX,
} vicap_hal_chn_hfir_ratio;

typedef struct {
    hi_u32 ratio;
    hi_bool scale_en;
} vicap_hal_chn_vscale_cfg;

typedef enum {
    VICAP_HAL_CHN_VFIR_RATIO_NO = 1,   /* 无垂直缩放 */
    VICAP_HAL_CHN_VFIR_RATIO_16X = 16, /* 垂直缩放最大倍数 */

    VICAP_HAL_CHN_VFIR_RATIO_MAX,
} vicap_hal_chn_vfir_ratio;

typedef struct {
    hi_u32 in_img_width;              /* 输入图像宽度 */
    hi_u32 in_img_height;             /* 输入图像高度 */
    hi_u32 out_img_width;             /* 输出图像宽度 */
    hi_u32 out_img_height;            /* 输入图像高度 */
    hi_drv_pixel_format pixel_format; /* 像素格式 */
    hi_drv_pixel_format video_fmt;    /* 视频存储格式 */
} vicap_hal_chn_scaler_attr;

/* 中断使能掩码或中断状态位 */
typedef enum {
    VICAP_HAL_CHN_INT_FRAMESTART = 0x0001, /* [0]场/帧起始中断 */
    VICAP_HAL_CHN_INT_CC = 0x0002,         /* [1]获取完毕中断状态 */
    VICAP_HAL_CHN_INT_BUFOVF = 0x0004,     /* [2]内部FIFO溢出错误中断状态 */
    VICAP_HAL_CHN_INT_FIELDTHROW = 0x0008, /* [3]场数据丢失中断 */
    VICAP_HAL_CHN_INT_UPDATECFG = 0x0010,  /* [4]工作寄存器更新中断 */
    VICAP_HAL_CHN_INT_CBUSERR = 0x0020,    /* [5]C数据通路总线错误中断状态 */
    VICAP_HAL_CHN_INT_YBUSERR = 0x0040,    /* [6]Y数据通路总线错误中断状态 */

    VICAP_HAL_CHN_DOLBY_CRC_ERROR = 0x0080, /* [7]metadata数据crc校验错误中断状态 */
    VICAP_HAL_CHN_DOLBY_WRITE_CC = 0x0100,  /* [8]metadata数据写ddr完成中断状态 */
    VICAP_HAL_CHN_DOLBY_BUFOVF = 0x0200,    /* [9]metadata数据内部fifo溢出中断状态 */
    VICAP_HAL_CHN_DOLBY_EOS = 0x0400,       /* [10]退出dolby模式中断状态 */

    VICAP_HAL_CHN_INT_FSTARTDELAY = 0x8000,   /* [15]延时之后的场/帧起始中断 */
    VICAP_HAL_CHN_INT_FSTARTDELAY1 = 0x10000, /* [16]延时之后的场/帧起始中断 */
    VICAP_HAL_CHN_INT_FSTARTDELAY2 = 0x20000, /* [17]延时之后的场/帧起始中断 */

    VICAP_HAL_CHN_HDR_ECC = 0x40000,             /* [18]HDR信息帧ECC错误 */
    VICAP_HAL_CHN_HDR_PATH_ERROR = 0x80000,      /* [19]HDR信息帧通路错误 */
    VICAP_HAL_CHN_HDR_CONTINUOUS_ERR = 0x100000, /* [20]HDR信息帧连续帧丢失错误 */
    VICAP_HAL_CHN_HDR_CURR_ERROR = 0x200000,     /* [21]HDR信息帧当前帧丢失错误 */
    VICAP_HAL_CHN_HDR_SPAN_ERROR = 0x400000,     /* [22]HDR信息帧跨帧错误 */
    VICAP_HAL_CHN_HDR_SEQUENCE_ERR = 0x800000,   /* [23]HDR信息帧帧序错误 */
    VICAP_HAL_CHN_METADATA_CC = 0x4000000,       /* [26]动态Metadata获取完成中断 */
    VICAP_HAL_CHN_METADATA_OVERFLOW = 0x8000000, /* [27]动态Metadata溢出错误中断 */

    VICAP_HAL_CHN_INT_MAX,
} vicap_hal_chn_int;

/*************************************vicap dolby hal chn enum and struct*********************************/
typedef enum {
    VICAP_HAL_CHN_DOLBY_DATA_MODE_YUV = 0, /* Dolby 输入为rgb */
    VICAP_HAL_CHN_DOLBY_DATA_MODE_RGB,     /* Dolby 输入为yuv */

    VICAP_HAL_CHN_DOLBY_DATA_MODE_MAX,
} vicap_hal_chn_dolby_data_mode;

/*************************************vicap dolby hal chn enum and struct*********************************/
typedef enum {
    VICAP_HAL_SMMU_STATUS_BUSY = 0,
    VICAP_HAL_SMMU_STATUS_IDLE,

    VICAP_HAL_SMMU_STATUS_MAX,
} vicap_hal_smmu_status;

typedef enum {
    VICAP_HAL_SMMU_SAFE_TYPE_SAFE = 0,
    VICAP_HAL_SMMU_SAFE_TYPE_UNSAFE = 1,

    VICAP_HAL_SMMU_SAFE_TYPE__MAX,
} vicap_hal_smmu_safe_type;

typedef enum {
    VICAP_HAL_SMMU_INT_BIT_NS_TLBMISS = 0x0001,       /* [0]非安全操作时页表未命中中断屏蔽位 */
    VICAP_HAL_SMMU_INT_BIT_NS_PTW_TRANS = 0x0002,     /* [1]非安全操作时页表预取错误中断屏蔽位 */
    VICAP_HAL_SMMU_INT_BIT_NS_TLBINVALID_RD = 0x0004, /* [2]非安全读操作时页表无效中断屏蔽位 */
    VICAP_HAL_SMMU_INT_BIT_NS_TLBINVALID_WR = 0x0008, /* [3]非安全写操作时页表无效中断屏蔽位 */

    VICAP_HAL_SMMU_INT_BIT_MAX,
} vicap_hal_smmu_int_bit_ns;

typedef struct {
    hi_bool enable;
    hi_u32 ink_sel;
    hi_u32 pix_fmt;
    hi_u32 color_mode;
    hi_u32 x_pos;
    hi_u32 y_pos;
} vicap_hal_chn_ink_ctrl;

typedef struct {
    hi_u32 data1;
    hi_u32 data2;
    hi_u32 data3;
} vicap_hal_chn_ink_data;

typedef struct {
    hi_u32 max_metadata_length;
    hi_bool en_hdr_det;
    hi_bool en_hdr_wr;
    hi_bool en_hdr_en;
} vicap_hal_chn_hdr_ctrl;

hi_s32 vicap_hal_init(hi_void);
hi_void vicap_hal_de_init(hi_void);

hi_void vicap_hal_crg_set_bus_clk_en(hi_bool enable);
hi_void vicap_hal_crg_set_ppc_clk_en(hi_bool enable);
hi_void vicap_hal_crg_set_smmu_clk_en(hi_bool enable);
hi_void vicap_hal_crg_set_ppc_clk(vicap_hal_chn_workhz chn_work_hz);

hi_void vicap_hal_crg_set_bus_reset(hi_bool enable);
hi_void vicap_hal_crg_set_smmu_reset(hi_bool enable);
hi_void vicap_hal_crg_set_hdmirx2_vi_clk(vicap_hal_chn_hdmirx_workhz chn_hdmirx_work_hz);

hi_void vicap_hal_global_set_power_mode(hi_u32 vicap_id, hi_bool low_power);
hi_void vicap_hal_global_set_mix_mode(hi_u32 vicap_id, hi_bool mixen, vicap_hal_mix_mode mix_mode);
hi_void vicap_hal_global_set_axi_cfg(hi_u32 vicap_id, vicap_hal_outstanding out_standing);

hi_void vicap_hal_global_set_mac_cfg(hi_u32 vicap_id, hi_bool lower);

hi_void vicap_hal_global_set_pt_sel(hi_u32 vicap_id);

hi_void vicap_hal_global_set_apb_time_out(hi_u32 vicap_id);

hi_void vicap_hal_global_set_vicap_int(hi_u32 vicap_id);

hi_void vicap_hal_global_set_pt_int_mask(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_bool valid);

hi_void vicap_hal_global_set_chn_int_mask(hi_u32 vicap_id, hi_bool valid);

hi_void vicap_hal_pt_set_intf_mod(hi_u32 vicap_id, vicap_hal_pt_index pt_index, vicap_hal_pt_intf_mode pt_intf_mod);

hi_u32 vicap_hal_pt_get_en_status(hi_u32 vicap_id, vicap_hal_pt_index pt_index);

hi_void vicap_hal_pt_set_en(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_bool enalbe);

hi_void vicap_hal_pt_set_offset(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_u32 *offset, hi_u32 *comp_mask,
                                hi_u32 comp_len);

hi_void vicap_hal_pt_set_unitf_timing_cfg(hi_u32 vicap_id, vicap_hal_pt_index pt_index,
                                          vicap_hal_pt_sync_cfg *sync_cfg);

hi_void VICAP_HAL_PT_SetGenTimingCfg(vicap_hal_pt_index pt_index);

hi_void vicap_hal_pt_set_unitf_data_cfg(hi_u32 vicap_id, vicap_hal_pt_index pt_index, vicap_hal_pt_comp_mode comp_mode);

hi_void vicap_hal_pt_set_gen_intf(hi_u32 vicap_id, vicap_hal_pt_index pt_index, vicap_hal_pt_gentiming_info intf_cfg);

hi_void vicap_hal_pt_set_gen_timing(hi_u32 vicap_id, vicap_hal_pt_index pt_index,
                                    vicap_hal_pt_gentiming_cfg gen_timing_cfg);

hi_void vicap_hal_pt_set_gen_data_cfg(hi_u32 vicap_id, vicap_hal_pt_index pt_index,
                                      vicap_hal_pt_gendata_cfg gen_data_cfg);

hi_void vicap_hal_pt_set_gen_data_coef(hi_u32 vicap_id, vicap_hal_pt_index pt_index,
                                       vicap_hal_pt_gendata_coef gen_data_coef);

hi_void vicap_hal_pt_set_gen_data_init(hi_u32 vicap_id, vicap_hal_pt_index pt_index,
                                       vicap_hal_pt_gendata_init gen_data_init);

hi_void vicap_hal_pt_set_yuv444_cfg(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_bool upto_yuv444);

hi_void vicap_hal_pt_set_fstart_dly(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_u32 pt_fstart_dly);

hi_void vicap_hal_pt_set_hor_skip(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_u32 skip_cfg);

vicap_hal_frame_filed vicap_hal_pt_get_field(hi_u32 vicap_id, vicap_hal_pt_index pt_index);

hi_void vicap_hal_pt_get_frame_size(hi_u32 vicap_id, vicap_hal_pt_index pt_index, vicap_hal_frame_size *st_frame_size);

hi_void vicap_hal_pt_set_clear_int(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_u32 clr_int);

hi_s32 vicap_hal_pt_get_int_status(hi_u32 vicap_id, vicap_hal_pt_index pt_index);

hi_void vicap_hal_pt_set_int_mask(hi_u32 vicap_id, vicap_hal_pt_index pt_index, hi_u32 pt_int_mask);

hi_void vicap_hal_chn_set_dolby_chk_en(hi_u32 vicap_id, hi_bool enable);

hi_void vicap_hal_chn_set_dolby_data_mode(hi_u32 vicap_id, vicap_hal_chn_dolby_data_mode hdmi_input_data_mode);

hi_void vicap_hal_chn_set_dolby_tunnel_en(hi_u32 vicap_id, hi_bool enable);

hi_void vicap_hal_chn_set_dolby_write_metadata_en(hi_u32 vicap_id, hi_bool enable);

hi_void vicap_hal_chn_set_dolby_wch_faddr(hi_u32 vicap_id, hi_u32 dobly_addr);

hi_u32 vicap_hal_chn_get_dolby_cur_frm_pack_num(hi_u32 vicap_id);

hi_u32 vicap_hal_chn_get_en_status(hi_u32 vicap_id);

hi_void vicap_hal_chn_set_en(hi_u32 vicap_id, hi_bool chn_en);

hi_void vicap_hal_chn_set_reg_newer(hi_u32 vicap_id);

hi_void vicap_hal_chn_set_adapter_cfg(hi_u32 vicap_id);

hi_void vicap_hal_chn_get_count(hi_u32 vicap_id, vicap_hal_chn_count *st_chn_count);

hi_void vicap_hal_chn_set_vdly_cfg(hi_u32 vicap_id, hi_u32 chn_v_dly);

hi_void vicap_hal_chn_set_fstart_dly_cfg(hi_u32 vicap_id, hi_u32 chn_v_dly);

hi_void vicap_hal_chn_set_remix420_cfg(hi_u32 vicap_id, vicap_hal_chn_420_cfg *pst420_cfg);

hi_void vicap_hal_chn_set_remix420_size(hi_u32 vicap_id, vicap_hal_frame_size *st_frame_size);

hi_void vicap_hal_chn_set_wch_ycfg(hi_u32 vicap_id, vicap_hal_chn_wch_cfg *wch_cfg);
hi_void vicap_hal_chn_get_wch_ycfg(hi_u32 vicap_id, vicap_hal_chn_wch_cfg *wch_cfg);

hi_void vicap_hal_chn_set_wch_ccfg(hi_u32 vicap_id, vicap_hal_chn_wch_cfg *wch_cfg);
hi_void vicap_hal_chn_get_wch_ccfg(hi_u32 vicap_id, vicap_hal_chn_wch_cfg *wch_cfg);

hi_void vicap_hal_chn_set_wch_ysize(hi_u32 vicap_id, vicap_hal_chn_dstsize *y_dest_size);
hi_void vicap_hal_chn_set_wch_csize(hi_u32 vicap_id, vicap_hal_chn_dstsize *c_dest_size);

hi_void vicap_hal_chn_set_wch_faddr(hi_u32 vicap_id, vicap_hal_chn_wch_faddr *vicap_addr);

hi_void vicap_hal_chn_set_wch_ystride(hi_u32 vicap_id, hi_u32 stride);
hi_void vicap_hal_chn_set_wch_cstride(hi_u32 vicap_id, hi_u32 stride);

hi_void vicap_hal_chn_set_clear_int(hi_u32 vicap_id, hi_u32 clr_int);
hi_s32 vicap_hal_chn_get_int_status(hi_u32 vicap_id);
hi_void vicap_hal_chn_set_int_mask(hi_u32 vicap_id, hi_u32 int_mask);

hi_s32 vicap_hal_chn_get_line_num(hi_u32 vicap_id);

hi_void vicap_hal_chn_set_crop(hi_u32 vicap_id, vicap_hal_chn_crop_cfg *crop_cfg);
hi_void vicap_hal_chn_set_crop_win(hi_u32 vicap_id, hi_u32 win_width, hi_u32 win_height);
hi_void vicap_hal_chn_get_crop_status(hi_u32 vicap_id, vicap_hal_chn_crop_cfg *crop_cfg);

hi_void vicap_hal_chn_set_csc_cfg(hi_u32 vicap_id, hi_bool enable_csc);
hi_void vicap_hal_chn_get_csc_cfg(hi_u32 vicap_id, hi_bool *pen_enable_csc);
hi_void vicap_hal_chn_set_csc_param(hi_u32 vicap_id, vicap_hal_chn_csc_coef *vicap_csc_coef,
                                    vicap_hal_chn_csc_dccoef *vicap_cscdc_coef);

hi_void vicap_hal_chn_set_skip_ycfg(hi_u32 vicap_id, hi_u32 skip_cfg);
hi_void vicap_hal_chn_set_skip_ccfg(hi_u32 vicap_id, hi_u32 skip_cfg);

hi_void vicap_hal_chn_set_skip_ywin(hi_u32 vicap_id, vicap_hal_chn_skip_win *st_skip_win);
hi_void vicap_hal_chn_set_skip_cwin(hi_u32 vicap_id, vicap_hal_chn_skip_win *st_skip_win);

hi_void vicap_hal_chn_set_dither_en(hi_u32 vicap_id, hi_bool enable);
hi_void vicap_hal_chn_set_dither_cfg(hi_u32 vicap_id, vicap_hal_chn_dither_cfg dither_cfg);
hi_void vicap_hal_chn_get_dither_cfg(hi_u32 vicap_id, hi_bool *en_dither_cfg);

hi_void vicap_hal_chn_set_coef_yupdate(hi_u32 vicap_id, hi_bool valid);
hi_void vicap_hal_chn_set_coef_cupdate(hi_u32 vicap_id, hi_bool valid);

hi_void vicap_hal_chn_set_coef_yresl(hi_u32 vicap_id, hi_u32 value);
hi_void vicap_hal_chn_set_coef_cresl(hi_u32 vicap_id, hi_u32 value);

hi_void vicap_hal_chn_set_hscale_yen(hi_u32 vicap_id, hi_bool enable);
hi_void vicap_hal_chn_get_hscale_ycfg(hi_u32 vicap_id, vicap_hal_chn_hscale_cfg *chn_h_scale_cfg);
hi_void vicap_hal_chn_set_hscale_ycfg(hi_u32 vicap_id, vicap_hal_chn_hscale_cfg *chn_hscale_cfg);

hi_void vicap_hal_chn_set_hscale_cen(hi_u32 vicap_id, hi_bool enable);
hi_void vicap_hal_chn_set_hscale_ccfg(hi_u32 vicap_id, vicap_hal_chn_hscale_cfg *chn_hscale_cfg);
hi_void vicap_hal_chn_get_hscale_ccfg(hi_u32 vicap_id, vicap_hal_chn_hscale_cfg *chn_h_scale_cfg);

hi_void vicap_hal_chn_set_vscale_yen(hi_u32 vicap_id, hi_bool enable);
hi_void vicap_hal_chn_set_vscale_ycfg(hi_u32 vicap_id, vicap_hal_chn_vscale_cfg *chn_vscale_cfg);
hi_void vicap_hal_chn_get_vscale_ycfg(hi_u32 vicap_id, vicap_hal_chn_vscale_cfg *chn_v_scale_cfg);

hi_void vicap_hal_chn_set_vscale_cen(hi_u32 vicap_id, hi_bool enable);
hi_void vicap_hal_chn_set_vscale_ccfg(hi_u32 vicap_id, vicap_hal_chn_vscale_cfg *chn_vscale_cfg);
hi_void vicap_hal_chn_get_vscale_ccfg(hi_u32 vicap_id, vicap_hal_chn_vscale_cfg *chn_v_scale_cfg);

hi_void vicap_hal_chn_set_scale_hfir_in_size(hi_u32 vicap_id, vicap_hal_chn_scale_in_size *scale_in_size);
hi_void vicap_hal_chn_set_scale_hfir_out_size(hi_u32 vicap_id, vicap_hal_chn_scale_out_size *scale_out_size);
hi_s32 vicap_hal_chn_set_hscale_coef8_phase(hi_u32 vicap_id, vicap_hal_chn_scale_obj scaler_obj,
                                            vicap_hal_chn_scale_coef coef);

hi_void vicap_hal_chn_set_ink_ctrl(hi_u32 vicap_id, vicap_hal_chn_ink_ctrl *ink_ctrl_info);

hi_void vicap_hal_chn_get_ink_data(hi_u32 vicap_id, vicap_hal_chn_ink_data *ink_data_info);

hi_void vicap_hal_chn_set_metadata_wch_addr_h(hi_u32 vicap_id, hi_u32 metadata_wch_addr_h);

hi_void vicap_hal_chn_set_metadata_wch_addr_l(hi_u32 vicap_id, hi_u32 metadata_wch_addr_l);

hi_void vicap_hal_chn_set_hdr_ctrl(hi_u32 vicap_id, vicap_hal_chn_hdr_ctrl *chn_hdr_ctrl);

hi_void vicap_hal_chn_set_fstart_dly(hi_u32 vicap_id, hi_u32 f_start_dly_cycle);

hi_void vicap_hal_chn_get_hdr_status0(hi_u32 vicap_id, hi_u32 *chn_hdr_status0);
hi_u32 vicap_hal_chn_get_ycheck_sum(hi_u32 vicap_id);

hi_u32 vicap_hal_chn_get_ccheck_sum(hi_u32 vicap_id);
hi_void vicap_hal_chn_enable_tee_int(hi_u32 vicap_id, hi_bool enable);

hi_void vicap_hal_smmu_set_ptw(hi_u32 vicap_id, hi_u32 ptw);

hi_void vicap_hal_smmu_set_int_en(hi_u32 vicap_id, hi_bool en);

hi_void vicap_hal_smmu_set_global_bypass(hi_u32 vicap_id, hi_bool en);
hi_void vicap_hal_smmu_get_global_bypass(hi_u32 vicap_id, hi_bool *en);

hi_void vicap_hal_smmu_set_clk_gate_en(hi_u32 vicap_id, hi_bool en);

hi_void vicap_hal_smmu_set_int_mask(hi_u32 vicap_id, vicap_hal_smmu_safe_type type, hi_u32 int_mask);

hi_void vicap_hal_smmu_get_int_raw(hi_u32 vicap_id, vicap_hal_smmu_safe_type type, hi_u32 *int_bit);

hi_void vicap_hal_smmu_get_int_status(hi_u32 vicap_id, vicap_hal_smmu_safe_type type, hi_u32 *int_bit);
hi_void vicap_hal_smmu_set_int_clr(hi_u32 vicap_id, vicap_hal_smmu_safe_type type, hi_u32 int_bit);
hi_void vicap_hal_smmu_set_table_base_addr(hi_u32 vicap_id, vicap_hal_smmu_safe_type type, hi_u32 base_addr);

hi_void vicap_hal_smmu_set_err_read_addr(hi_u32 vicap_id, vicap_hal_smmu_safe_type type, hi_u32 addr);

hi_void vicap_hal_smmu_set_err_write_addr(hi_u32 vicap_id, vicap_hal_smmu_safe_type type, hi_u32 addr);

hi_s32 vicap_hal_chn_get_int_mask(hi_u32 vicap_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HAL_VICAP_H__ */
