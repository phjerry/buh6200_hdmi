/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: encoder
 * Author: sdk
 * Create: 2012-04-22
 */
#ifndef __VENC_HAL_EXT_H__
#define __VENC_HAL_EXT_H__

#include "drv_venc_osal_ext.h"

#define  VENC_NULL_FUN_PTR    (-1)

typedef enum {
    INTERRUPT_TYPE_PIC_END    = 0,
    INTERRUPT_TYPE_SLICE_END  = 1,
    INTERRUPT_TYPE_TIMEOUT    = 2,
    INTERRUPT_TYPE_ALL        = 3,
    INTERRUPT_TYPE_MAX,
} interrupt_type;

typedef struct {
    hi_u32 protocol;
    hi_u32 profile;
    hi_u32 enc_width;
    hi_u32 enc_height;
    hi_u32 slc_split_en;
    hi_u32 slc_split_mode;
    hi_u32 split_size;
    hi_u32 csc_mode;            /* 0: disable 1:601->709 2:709->601 */
    hi_u32 secure_en;
    hi_u32 low_dly_mode;
    hi_u32 time_out_en;
    hi_u32 pt_bits_en;
    hi_u32 parameter_set_en;
    hi_u32 reg_config_mode;      /* 0: Reg; 1:ddr; */
    hi_u32 lcu_performance_baseline;
    hi_u32 lcu_performance_en;
    hi_bool enable_rcn_ref_share_buf;
} venc_hal_ctrl;

typedef struct {
    hi_u32 store_fmt;           /* 0, semiplannar; 1, package; 2,planer */
    hi_u32 package_sel;
    hi_u32 src_y_addr;
    hi_u32 src_c_addr;
    hi_u32 src_v_addr;                  /* just for input of planner */
    hi_u32 s_stride_y;
    hi_u32 s_stride_c;
    hi_u32 vcpi_recst_ystride;
    hi_u32 vcpi_recst_cstride;
    hi_u32 vcpi_refy_l0_stride;
    hi_u32 vcpi_refc_l0_stride;
    hi_u32 vcpi_refy_l1_stride;
    hi_u32 vcpi_refc_l1_stride;
    hi_u32 pme_stride;
    hi_u32 rcn_idx;             /* 0 or 1, idx for rcn, !idx for ref */
    hi_u32 ref_num;
    hi_u32 long_term_refpic;

    hi_u32 vedu_vcpi_rec_yaddr0_l;
    hi_u32 vedu_vcpi_rec_caddr0_l;
    hi_u32 vedu_vcpi_refy_l0_addr0_l;
    hi_u32 vedu_vcpi_refc_l0_addr0_l;
    hi_u32 vedu_vcpi_refy_l1_addr0_l;
    hi_u32 vedu_vcpi_refc_l1_addr0_l;

    hi_u32 vedu_vcpi_rec_yaddr1_l;
    hi_u32 vedu_vcpi_rec_caddr1_l;
    hi_u32 vedu_vcpi_refy_l0_addr1_l;
    hi_u32 vedu_vcpi_refc_l0_addr1_l;
    hi_u32 vedu_vcpi_refy_l1_addr1_l;
    hi_u32 vedu_vcpi_refc_l1_addr1_l;

    hi_u32 vcpi_rec_luma_length;
    hi_u32 vcpi_rec_chroma_length;
    hi_u32 vcpi_ref0_luma_length;
    hi_u32 vcpi_ref0_chroma_length;
    hi_u32 vcpi_ref1_luma_length;
    hi_u32 vcpi_ref1_chroma_length;

    hi_u32 tmv_addr[2]; /* 2: size of tmv_addr */
    hi_u32 vedu_vcpi_nbi_mvst_addr_l;
    hi_u32 vedu_vcpi_nbi_mvld_addr_l;
    hi_u32 pme_addr[2]; /* 2: size of pme_addr */
    hi_u32 vedu_vcpi_pmeld_l0_addr_l;
    hi_u32 vedu_vcpi_pmeld_l1_addr_l;
    hi_u32 vedu_vcpi_pmest_addr_l;

    hi_u32 vedu_vcpi_pmeinfo_st_addr_l;
    hi_u32 vedu_vcpi_pmeinfo_ld0_addr_l;
    hi_u32 pme_info_addr[3]; /* 3: size of pme_info_addr */
    hi_u32 pme_info_idx;
    hi_u32 pmv_poc[6]; /* 6: size of pmv_poc */
    hi_u32 vcpi_curr_ref_long_flag;

    hi_u32 tunl_cell_addr;

    hi_u32 strm_buf_addr[16]; /* 16: size of strm_buf_addr */
    hi_u32 strm_buf_size[16]; /* 16: size of strm_buf_size */
    hi_u32 strm_buf_rp_addr; /* phy addr for hardware */
    hi_u32 strm_buf_wp_addr;

    hi_u32 para_set_addr;
    hi_u32 para_set_buf_size;
    hi_u32 para_set_reg[48]; /* 48: size of para_set_reg */
    hi_u32 stream_flag;
} venc_hal_buf;

typedef struct {
    hi_u32 intra_pic;
    hi_u32 target_bits;         /* targetBits of each frame */
    hi_u32 start_qp;
    hi_u32 min_qp;
    hi_u32 max_qp;
    hi_s32 i_qp_delta;
    hi_s32 p_qp_delta;
    hi_s32 avbr;
    hi_u32 pic_type;
} venc_hal_rc;

typedef struct {
    hi_u32 src_y_bypass;
    hi_u32 src_u_bypass;
    hi_u32 src_v_bypass;

    hi_u32 smmu_ns_page_base_addr;
    hi_u32 smmu_ns_err_write_addr;
    hi_u32 smmu_ns_err_read_addr;

    hi_u32 smmu_s_page_base_addr;
    hi_u32 smmu_s_err_write_addr;
    hi_u32 smmu_s_err_read_addr;
} venc_hal_smmu;

typedef struct {
    hi_u32 slc_hdr_stream [4];      /* 4: size of slc_hdr_stream */
    hi_u32 reorder_stream[2];       /* 2: size of reorder_stream */
    hi_u32 marking_stream[2];       /* 2: size of marking_stream */
    hi_u32 slc_hdr_bits[2];         /* 2: size of slc_hdr_bits, *****h264:only use slc_hdr_bits[0], h265 use two */
    hi_u32 cabac_slc_hdr_part2_seg[8]; /* 8: size of cabac_slc_hdr_part2_seg, for h265 slchead */
    hi_u32 slc_hdr_part1;
} venc_hal_stream;

typedef struct {
    hi_u32 pts0;
    hi_u32 pts1;
    hi_u32 ext_flag;
    hi_u32 ext_fill_len;
    hi_u32 ddr_reg_cfg_phy_addr;
    VIRT_ADDR ddr_reg_cfg_vir_addr;
} venc_hal_ext;

typedef struct {
    hi_u32 *reg_base;

    venc_hal_ctrl ctrl_cfg;

    venc_hal_buf buf_cfg;

    venc_hal_rc rc_cfg;

    venc_hal_smmu smmu_cfg;

    venc_hal_stream stream_cfg;

    venc_hal_ext ext_cfg;
} venc_hal_cfg;

typedef struct {
    /* status */
    hi_u32 venc_time_out;
    hi_u32 venc_slice_int;
    hi_u32 venc_end_of_pic;
    hi_u32 venc_buf_full;
    hi_u32 venc_pbit_overflow;

    /* rc */
    hi_u32 pic_bits;
    hi_u32 mean_qp;
    hi_s32 num_i_mb_cur_frm;
    hi_u32 mhb_bits;
    hi_u32 txt_bits;
    hi_u32 madi_val;
    hi_u32 madp_val;

    /* stream */
    hi_u32  slice_length[16]; /* 16: size of slice_length */
    hi_u32  slice_is_end[16]; /* 16: size of slice_is_end */
} venc_hal_read;

typedef struct {
    hi_u32 tlb_miss_stat_ns;
    hi_u32 ptw_trans_stat_ns;
    hi_u32 tlb_invalid_stat_ns_rd;
    hi_u32 tlb_invalid_stat_ns_wr;
    hi_u32 tlb_miss_stat_s;
    hi_u32 ptw_trans_stat_s;
    hi_u32 tlb_invalid_stat_s_rd;
    hi_u32 tlb_invalid_stat_s_wr;

    hi_u32 fault_addr_rd_ns;
    hi_u32 fault_addr_wr_ns;
    hi_u32 fault_addr_rd_s;
    hi_u32 fault_addr_wr_s;
} venc_hal_read_smmu;

typedef struct {
    void (*pfun_hal_clear_int)(interrupt_type type);
    void (*pfun_hal_disable_int)(interrupt_type type);

    void (*pfun_hal_read_reg)(venc_hal_read *read_back);
    void (*pfun_hal_read_smmu_reg)(venc_hal_read_smmu *read_back);
    void (*pfun_hal_read_cfg_reg)(venc_hal_cfg *reg_cfg);

    hi_s32 (*pfun_hal_init)(void);
    void  (*pfun_hal_deinit)(void);

    void (*pfun_hal_open_hardware)(void);
    void (*pfun_hal_close_hardware)(void);
    void (*pfun_hal_reset_hardware)(void);
} venc_hal_fun_ptr;

extern venc_hal_fun_ptr g_venc_hal_fun_ptr;

#define VENC_HAL_CLEAR_INT(interrupt_type) do { \
    if (NULL != g_venc_hal_fun_ptr.pfun_hal_clear_int) { \
        g_venc_hal_fun_ptr.pfun_hal_clear_int(interrupt_type); \
    }  \
} while (0)

#define VENC_HAL_DISABLE_INT(interrupt_type) do { \
    if(NULL != g_venc_hal_fun_ptr.pfun_hal_disable_int) { \
        g_venc_hal_fun_ptr.pfun_hal_disable_int(interrupt_type); \
    }  \
} while (0)

#define VENC_HAL_DEINIT() do { \
    if(NULL != g_venc_hal_fun_ptr.pfun_hal_deinit) { \
        g_venc_hal_fun_ptr.pfun_hal_deinit(); \
    }  \
} while (0)

#define VENC_HAL_INIT() \
    (g_venc_hal_fun_ptr.pfun_hal_init ? \
    g_venc_hal_fun_ptr.pfun_hal_init() : \
    VENC_NULL_FUN_PTR)

#define VENC_HAL_READ_REG(read_back) \
    (g_venc_hal_fun_ptr.pfun_hal_read_reg ? \
    g_venc_hal_fun_ptr.pfun_hal_read_reg(read_back) : \
    VENC_NULL_FUN_PTR)

#define VENC_HAL_READ_REG_SMMU(read_back) \
    (g_venc_hal_fun_ptr.pfun_hal_read_smmu_reg ? \
    g_venc_hal_fun_ptr.pfun_hal_read_smmu_reg(read_back) : \
    VENC_NULL_FUN_PTR)

#define VENC_HAL_CFG_REG(reg_cfg) \
    (g_venc_hal_fun_ptr.pfun_hal_read_cfg_reg ? \
    g_venc_hal_fun_ptr.pfun_hal_read_cfg_reg(reg_cfg) : \
    VENC_NULL_FUN_PTR)

#define VENC_PDT_OPEN_HARDWARE() do { \
    if(NULL != g_venc_hal_fun_ptr.pfun_hal_open_hardware) { \
        g_venc_hal_fun_ptr.pfun_hal_open_hardware(); \
    }  \
} while (0)

#define VENC_PDT_CLOSE_HARDWARE() do { \
    if(NULL != g_venc_hal_fun_ptr.pfun_hal_close_hardware)  { \
        g_venc_hal_fun_ptr.pfun_hal_close_hardware(); \
    }  \
} while (0)

#define VENC_PDT_RESET_HARDWARE() do { \
    if(NULL != g_venc_hal_fun_ptr.pfun_hal_reset_hardware) { \
        g_venc_hal_fun_ptr.pfun_hal_reset_hardware(); \
    }  \
} while (0)

void drv_venc_hal_v400_r008_c02_clr_int(interrupt_type type);
void drv_venc_hal_v400_r008_c02_disable_int(interrupt_type type);
void drv_venc_hal_v400_r008_c02_read_reg_smmu(venc_hal_read_smmu* read_back);
void drv_venc_hal_v400_r008_c02_read_reg(venc_hal_read *read_back);
void drv_venc_hal_v400_r008_c02_cfg_reg(venc_hal_cfg *reg_cfg);
hi_s32 drv_venc_hal_v400_r008_c02_init(void);
void drv_venc_hal_v400_r008_c02_deinit(void);

#endif  /* __VENC_HAL_EXT_H__ */