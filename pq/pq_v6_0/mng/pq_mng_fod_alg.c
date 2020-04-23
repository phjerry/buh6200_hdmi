/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2018. All rights reserved.
 * Description: 场序检测算法软算法
 * Author: m
 * Create: 2019-10-16
 * Notes:
 */

#include <linux/hisilicon/securec.h>
#include "pq_hal_comm.h"
#include "pq_mng_fod_alg.h"
#include "drv_pq.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define FIELD_TOP_FIRST                     0
#define FIELD_BOTTOM_FIRST                  1
#define FIELD_UNKNOWN                       2

#define REF_TOP_FIRST                       0
#define REF_BOTTOM_FIRST                    1

#define FOD_ENABLE                          1
#define FOD_TOP_FIRST                       2
#define FOD_BOTTOM_FIRST                    3
#define LITTLE_MOVE_THD                     20
/* definition of thresholds */
#define SADDIFF_ACC_THR                     1000
#define FIELD_ORDER_THR                     200
#define SINGLE_FRAME_MAX                    200

#define optm_alg_abs(x)                     (((x) < 0) ? -(x) : (x))
#define optm_alg_min2(x, y)                 (((x) < (y)) ? (x) : (y))
#define optm_alg_max2(x, y)                 (((x) > (y)) ? (x) : (y))
#define optm_alg_min3(x, y, z)              (optm_alg_min2(optm_alg_min2((x), (y)), (z)))
#define optm_alg_clip3(low, high, x)        (optm_alg_max2(optm_alg_min2((x), (high)), (low)))
#define optm_alg_round(x)                   ((((x) % 10) > 4) ? ((x) / 10 + 1) * 10 : (x))
#define PRE_FEILD_NUM  5

static hi_drv_pq_vpss_fod_sw_stat_para g_pq_fod_total_sw_para_n[VPSS_HANDLE_NUM];
static hi_pq_fod_mode g_pq_fod_mode = PQ_FOD_ENABLE_AUTO;
static hi_u32 g_field_order      = 2;
static hi_u32 g_force_field_order = 2;

typedef struct {
    /* data */
    hi_s32 z14;
    hi_s32 z32;
    hi_s32 z34;
} fod_lasi_info_struct;

typedef struct {
    /* data */
    hi_s32 is_biggest[3]; /* 3 idx 0--14 1--32 2--34 */
    hi_s32 is_smallest[3]; /* 3 idx 0--14 1--32 2--34 */

} fod_lasi_big_small_stat;

static hi_void pq_mng_fod_get_stt_info(hi_drv_pq_vpss_fod_sw_in_para *p_fod_swin_para, vpss_stt_reg_type *p_stt_reg)
{
    p_fod_swin_para->lasi_stat_1.lasi_cnt14 = p_stt_reg->vpss_wstt_pd_lasi_cnt_00.bits.lasi_14_cnt_0;
    p_fod_swin_para->lasi_stat_1.lasi_cnt32 = p_stt_reg->vpss_wstt_pd_lasi_cnt_01.bits.lasi_32_cnt_0;
    p_fod_swin_para->lasi_stat_1.lasi_cnt34 = p_stt_reg->vpss_wstt_pd_lasi_cnt_02.bits.lasi_34_cnt_0;

    p_fod_swin_para->lasi_stat_2.lasi_cnt14 = p_stt_reg->vpss_wstt_pd_lasi_cnt_10.bits.lasi_14_cnt_1;
    p_fod_swin_para->lasi_stat_2.lasi_cnt32 = p_stt_reg->vpss_wstt_pd_lasi_cnt_11.bits.lasi_32_cnt_1;
    p_fod_swin_para->lasi_stat_2.lasi_cnt34 = p_stt_reg->vpss_wstt_pd_lasi_cnt_12.bits.lasi_34_cnt_1;

    p_fod_swin_para->still_blk_info.blk_sad[0] = p_stt_reg->vpss_wstt_pd_stlblk_sad_0.bits.stlblk_sad0;    /* 0 idx */
    p_fod_swin_para->still_blk_info.blk_sad[1] = p_stt_reg->vpss_wstt_pd_stlblk_sad_1.bits.stlblk_sad1;    /* 1 idx */
    p_fod_swin_para->still_blk_info.blk_sad[2] = p_stt_reg->vpss_wstt_pd_stlblk_sad_2.bits.stlblk_sad2;    /* 2 idx */
    p_fod_swin_para->still_blk_info.blk_sad[3] = p_stt_reg->vpss_wstt_pd_stlblk_sad_3.bits.stlblk_sad3;    /* 3 idx */
    p_fod_swin_para->still_blk_info.blk_sad[4] = p_stt_reg->vpss_wstt_pd_stlblk_sad_4.bits.stlblk_sad4;    /* 4 idx */
    p_fod_swin_para->still_blk_info.blk_sad[5] = p_stt_reg->vpss_wstt_pd_stlblk_sad_5.bits.stlblk_sad5;    /* 5 idx */
    p_fod_swin_para->still_blk_info.blk_sad[6] = p_stt_reg->vpss_wstt_pd_stlblk_sad_6.bits.stlblk_sad6;    /* 6 idx */
    p_fod_swin_para->still_blk_info.blk_sad[7] = p_stt_reg->vpss_wstt_pd_stlblk_sad_7.bits.stlblk_sad7;    /* 7 idx */
    p_fod_swin_para->still_blk_info.blk_sad[8] = p_stt_reg->vpss_wstt_pd_stlblk_sad_8.bits.stlblk_sad8;    /* 8 idx */
    p_fod_swin_para->still_blk_info.blk_sad[9] = p_stt_reg->vpss_wstt_pd_stlblk_sad_9.bits.stlblk_sad9;    /* 9 idx */
    p_fod_swin_para->still_blk_info.blk_sad[10] = p_stt_reg->vpss_wstt_pd_stlblk_sad_10.bits.stlblk_sad10; /* 10 idx */
    p_fod_swin_para->still_blk_info.blk_sad[11] = p_stt_reg->vpss_wstt_pd_stlblk_sad_11.bits.stlblk_sad11; /* 11 idx */
    p_fod_swin_para->still_blk_info.blk_sad[12] = p_stt_reg->vpss_wstt_pd_stlblk_sad_12.bits.stlblk_sad12; /* 12 idx */
    p_fod_swin_para->still_blk_info.blk_sad[13] = p_stt_reg->vpss_wstt_pd_stlblk_sad_13.bits.stlblk_sad13; /* 13 idx */
    p_fod_swin_para->still_blk_info.blk_sad[14] = p_stt_reg->vpss_wstt_pd_stlblk_sad_14.bits.stlblk_sad14; /* 14 idx */
    p_fod_swin_para->still_blk_info.blk_sad[15] = p_stt_reg->vpss_wstt_pd_stlblk_sad_15.bits.stlblk_sad15; /* 15 idx */

    return;
}

static hi_void pq_mng_fod_detect_still_blk(reg_stillblk_stat *p_still_blk_info, hi_s32 still_blk_thr,
    fod_ctrl_para *p_fod_ctrl_para)
{
    hi_s32 i;
    hi_s32 still_blk_cnt = 0;

    for (i = 0; i < 16; i++) {  /* 0 16 range  */
        p_fod_ctrl_para->sad_reg[i][2] = p_fod_ctrl_para->sad_reg[i][1];  /* 1 2 idx */
        p_fod_ctrl_para->sad_reg[i][1] = p_fod_ctrl_para->sad_reg[i][0];  /* 0 1 idx */
        p_fod_ctrl_para->sad_reg[i][0] = p_still_blk_info->blk_sad[i];  /* 0 idx */
    }

    for (i = 0; i < 16; i++) { /* 0 16 idx */
        /* consecutive three fld in same blk all still, count ++ */
        if ((p_fod_ctrl_para->sad_reg[i][2] < still_blk_thr) &&  /* 2 idx */
            (p_fod_ctrl_para->sad_reg[i][1] < still_blk_thr)  /* 2 1 */
            && (p_fod_ctrl_para->sad_reg[i][0] < still_blk_thr)) { /* 0 idx */
            still_blk_cnt++;  /* CNCommet: 闈欐鍖哄煙 */
        }
    }

    p_still_blk_info->still_blk_cnt = still_blk_cnt;

    return;
}

hi_s32 pq_mng_get_fod_static_info(reg_lasi_stat *p_lasi_stat, alg_dei_drv_para *p_dei_drv_para,
    fod_ctrl_para *p_fod_ctrl_para, fod_lasi_info_struct *p_fod_lasi_info, hi_s32 *p_return_flag)
{
    hi_s32 z32 = 0;
    hi_s32 z14 = 0;
    hi_s32 z34 = 0;

    if ((p_lasi_stat == HI_NULL) || (p_dei_drv_para == HI_NULL) ||
        (p_fod_ctrl_para == HI_NULL) || (p_fod_lasi_info == HI_NULL) || (p_return_flag == HI_NULL)) {
        HI_ERR_PQ("ptr is NULL!\n");
        return 0;
    }

    if (p_dei_drv_para->dei_md_lum == ALG_DEI_MODE_3FLD) { /* when 3 field, no FOD detect */
        p_fod_ctrl_para->field_order = FIELD_UNKNOWN;
        *p_return_flag = 1; /* 1 */
        return p_fod_ctrl_para->field_order;
    } else if ((p_dei_drv_para->dei_md_lum == ALG_DEI_MODE_4FLD) &&
               (((p_fod_ctrl_para->bt_mode == FIELD_TOP_FIRST) && (p_fod_ctrl_para->ref_fld == REF_TOP_FIRST)) ||
                ((p_fod_ctrl_para->bt_mode == FIELD_BOTTOM_FIRST) && (p_fod_ctrl_para->ref_fld == REF_BOTTOM_FIRST)))) {
        *p_return_flag = 1;
        return p_fod_ctrl_para->field_order;
    } else {
        if (p_fod_ctrl_para->is_snd_fld == 0) {
            *p_return_flag = 1;
            return p_fod_ctrl_para->field_order; /* 淇濊瘉鍦哄簭妫�娴嬪彧浼氬湪涓�甯т腑鐨勭浜屽満鏀瑰彉 */
        }
        if (p_fod_ctrl_para->bt_mode == FIELD_BOTTOM_FIRST) {
            z14 = p_lasi_stat->lasi_cnt32;
            z32 = p_lasi_stat->lasi_cnt14;
            z34 = p_lasi_stat->lasi_cnt34;
        } else {
            z14 = p_lasi_stat->lasi_cnt14;
            z32 = p_lasi_stat->lasi_cnt32;
            z34 = p_lasi_stat->lasi_cnt34;
        }
    }

    *p_return_flag = 0;
    p_fod_lasi_info->z14 = z14;
    p_fod_lasi_info->z32 = z32;
    p_fod_lasi_info->z34 = z34;

    return p_fod_ctrl_para->field_order;
}

hi_s32 pq_mng_get_fod_blv_percent(alg_dei_drv_para *p_dei_drv_para, hi_s32 still_blk_cnt, hi_s32 z32, hi_s32 z14, 
    hi_s32 z34)
{
    hi_s32 width = p_dei_drv_para->frm_width;
    hi_s32 height = p_dei_drv_para->frm_height;
    hi_s32 mot_mb_num = (width * height) >> 9;  /* 9  */
    hi_s32 zthr, zmax, zmin;
    hi_s32 fac;
    hi_s32 signal = 0;
    hi_s32 percent = 0;

    zthr = mot_mb_num / 32;  /* 32  */
    zthr = (zthr * (16 - still_blk_cnt) / 16) * 3 / 2;  /* 16 3 2  */

    if (zthr < 6) {  /* 6  */
        zthr = 6;  /* 6 */
    }

    if ((width == 1920) && (height == 1080)) {  /* 1920 1080 fhd */
        if (optm_alg_min2(z14, z32) > z34) {
            fac = optm_alg_clip3(0, SINGLE_FRAME_MAX, optm_alg_abs(z14 - z32) - optm_alg_min2(z14, z32) - z34);
        } else {
            fac = 0;
        }

        fac = fac / zthr;
        signal = (z14 > z32) ? 1 : (-1);  /* 1 -1 signal */
        fac = signal * fac;
    } else {
        fac = (z14 - z32) / zthr;
    }

    zmin = (z14 < z32) ? z14 : z32;
    zmax = (z14 < z32) ? z32 : z14;

    if (zmax > 100) {                         /* 100 th */
        percent = (zmax - zmin) * 100 / zmax; /* 100 ratio */

        if (percent < 20) { /* 20 is blv limit */
            fac = 0;
        }
    }

    fac = optm_alg_clip3((-1) * SINGLE_FRAME_MAX, SINGLE_FRAME_MAX, fac);
    return fac;
}

static hi_void pq_mng_fod_upt_fac_basedon_bs(fod_lasi_big_small_stat *p_lasi_big_small_stat, hi_s32 *p_upt_fac)
{
    hi_s32 is_lasi_34_smallest = p_lasi_big_small_stat->is_smallest[2]; /* 2 */
    hi_s32 is_lasi_34_biggest = p_lasi_big_small_stat->is_biggest[2]; /* 2 */
    hi_s32 is_lasi_32_smallest = p_lasi_big_small_stat->is_smallest[1]; /* 1 */
    hi_s32 is_lasi_32_biggest = p_lasi_big_small_stat->is_biggest[1]; /* 1 */
    hi_s32 is_lasi_14_smallest = p_lasi_big_small_stat->is_smallest[0]; /* 0 */
    hi_s32 is_lasi_14_biggest = p_lasi_big_small_stat->is_biggest[0]; /* 0 */

    if (is_lasi_34_smallest) {
    } else if (is_lasi_34_biggest) {
        *p_upt_fac = 0;
    } else if (is_lasi_32_smallest && is_lasi_14_biggest) {
    } else if (is_lasi_14_smallest && is_lasi_32_biggest) {
    } else if (is_lasi_32_smallest || is_lasi_14_smallest) {
        *p_upt_fac = 0;
    }

    return;
}

hi_s32 pq_mng_fod_upt_single_percent(fod_lasi_info_struct *p_lasi_info, hi_s32 frm_width, hi_s32 frm_height, hi_s32 fac)
{
    hi_s32 z14 = p_lasi_info->z14;
    hi_s32 z32 = p_lasi_info->z32;
    hi_s32 z34 = p_lasi_info->z34;
    hi_s32 upt_fac = 0;
    hi_s32 islasi14biggest, islasi32biggest, islasi34biggest;
    hi_s32 islasi14smallest, islasi32smallest, islasi34smallest;
    hi_s32 biggerintwo, smallerintwo;
    hi_s32 num_diff_ratio = (z34 == 0) ? 1 : 5; /* 1 5 ratio */
    fod_lasi_big_small_stat p_lasi_big_small_stat = {0};

    biggerintwo = optm_alg_max2(z32, z34);
    islasi14biggest = (z14 > (biggerintwo + optm_alg_max2(biggerintwo / num_diff_ratio, 50))) ? 1 : 0; /* 1 0 */

    biggerintwo = optm_alg_max2(z14, z34);
    islasi32biggest = (z32 > (biggerintwo + optm_alg_max2(biggerintwo / num_diff_ratio, 50))) ? 1 : 0; /* 1 0 */

    biggerintwo = optm_alg_max2(z14, z32);
    islasi34biggest = (z34 > (biggerintwo + optm_alg_max2(biggerintwo / num_diff_ratio, 50))) ? 1 : 0; /* 1 0 */

    /* smallest lasi */
    smallerintwo = optm_alg_min2(z32, z34);
    islasi14smallest = (z14 < (smallerintwo - optm_alg_max2(smallerintwo / num_diff_ratio, 50))) ? 1 : 0; /* 1 0 */

    smallerintwo = optm_alg_min2(z14, z34);
    islasi32smallest = (z32 < (smallerintwo - optm_alg_max2(smallerintwo / num_diff_ratio, 50))) ? 1 : 0; /* 1 0 */

    smallerintwo = optm_alg_min2(z14, z32);
    islasi34smallest = (z34 < (smallerintwo - optm_alg_max2(smallerintwo / num_diff_ratio, 50))) ? 1 : 0; /* 1 0 */

    if ((frm_width == 1920) && (frm_height == 1080)) { /* 1920 1080 fhd  */
        if (z34 != 0) {
            if (islasi14biggest && (!islasi32smallest)) {
                upt_fac = (fac < 50) ? 50 : fac; /* 50 limit */
            } else if (islasi32biggest && (!islasi14smallest)) {
                upt_fac = (fac < -50) ? fac : (-50); /* -50 limit */
            }
        }
    } else {
        if (islasi14biggest && (!islasi32smallest)) {
            if (optm_alg_abs(fac) > LITTLE_MOVE_THD) {
                upt_fac = (fac < 50) ? 50 : fac; /* 50 limit */
            }
        } else if (islasi32biggest && (!islasi14smallest)) {
            if (optm_alg_abs(fac) > LITTLE_MOVE_THD) {
                upt_fac = (fac < -50) ? fac : (-50); /* -50 limit */
            }
        }
    }
    p_lasi_big_small_stat.is_biggest[0] = islasi14biggest; /* 0 */
    p_lasi_big_small_stat.is_biggest[1] = islasi32biggest; /* 1 */
    p_lasi_big_small_stat.is_biggest[2] = islasi34biggest; /* 2 */
    p_lasi_big_small_stat.is_smallest[0] = islasi14smallest; /* 0 */
    p_lasi_big_small_stat.is_smallest[1] = islasi32smallest; /* 1 */
    p_lasi_big_small_stat.is_smallest[2] = islasi34smallest; /* 2 */

    pq_mng_fod_upt_fac_basedon_bs(&p_lasi_big_small_stat, &upt_fac);

    /* lasi34、lasi14、lasi32大小相当，原则上应该是不进行FOD检测的，此处是将fac值设置上限(条件6) */
    if ((!islasi32smallest) && (!islasi14smallest) && (!islasi34smallest) &&
        (!islasi32biggest) && (!islasi14biggest) && (!islasi34biggest)) {
        upt_fac = optm_alg_clip3(-50, 50, upt_fac);
    }

    if (optm_alg_abs(fac) < NOISE_THR) {
        upt_fac = 0;
    }

    return upt_fac;
}

hi_s32 pq_mng_fod_tmp_upt_percent(fod_ctrl_para *p_fod_ctrl_para, hi_s32 fac, hi_s32 fod_inc_ratio,
    hi_s32 fod_inc_th, hi_s32 *p_tmp_stat)
{
    hi_s32 update_fac = fac;
    hi_s32 hist_fac_cnt_pos = 0;
    hi_s32 hist_fac_cnt_neg = 0;
    hi_s32 hist_fac_cnt_zero, i, flag;
    hi_s32 *p_arr_fac_frm_sdtv = p_fod_ctrl_para->arr_fac_frm_sdtv;

    for (i = 7; i > 0; i--) { /* 7 0 range */
        p_arr_fac_frm_sdtv[i] = p_arr_fac_frm_sdtv[i - 1];

        if (p_arr_fac_frm_sdtv[i] > 0) {
            hist_fac_cnt_pos++;
        }

        if (p_arr_fac_frm_sdtv[i] < 0) {
            hist_fac_cnt_neg++;
        }
    }

    hist_fac_cnt_zero = 7 - (hist_fac_cnt_pos + hist_fac_cnt_neg); /* 7 is max */
    p_arr_fac_frm_sdtv[0] = fac;

    /* dec ratio */
    if (p_arr_fac_frm_sdtv[0] * (hist_fac_cnt_pos - hist_fac_cnt_neg) < 0) { /* 0 */
        if (optm_alg_max2(hist_fac_cnt_pos, hist_fac_cnt_neg) > 5) {  /* 5 blv limit */
            update_fac = 0;                                           /* 0 */
            flag = 11;                                                /* 11 flag */
        } else if (optm_alg_max2(hist_fac_cnt_pos, hist_fac_cnt_neg) > 3) { /* 3 not confirm limit */
            update_fac = fac / 2;                                      /* 2 is ratio */
            flag = 12;                                                /* 12 flag */
        } else {
            flag = 13;                                                /* 13 flag */
        }
    } else if ((p_arr_fac_frm_sdtv[0] * (hist_fac_cnt_pos - hist_fac_cnt_neg) > 0) \
               && (optm_alg_max2(hist_fac_cnt_pos, hist_fac_cnt_neg) > hist_fac_cnt_zero)) { /* acc ratio */
        if ((optm_alg_max2(hist_fac_cnt_pos, hist_fac_cnt_neg) > fod_inc_th) \
            && (optm_alg_min2(hist_fac_cnt_neg, hist_fac_cnt_pos) == 0)) {
            update_fac = fod_inc_ratio * fac;
            flag = 21;                                                /* 21 flag */
        } else if (optm_alg_max2(hist_fac_cnt_pos, hist_fac_cnt_neg) > 3) { /* 3 not confirm limit */
            flag = 22;                                                /* 22 flag */
        } else {
            update_fac = fac / 2; /* 2 ratio */
            flag = 23;           /* 23 flag */
        }
    } else {
        flag = 31; /* 31 flag */
    }

    *p_tmp_stat = flag;
    return update_fac;
}

    /* FOD检测优化方案 */
    /*****************************************************************************************************************
    1、在原算法基础上，小运动时，只要跳跃出现了，且满足规律，那么fac设置为最小为50 的值，可以解决小运动收敛慢的的问题
    2、lasi34最小的时候，不需要检查跳跃性（可以解决逐行源加字幕不能锁定的问题）
    3、加入pulldown信息，当pld模式下时，lasi34可以不是最小（pulldown模式下lasi34不一定最小，此时需要考虑三场一周期的情况）
    4、隔行源的情况下，lasi14或者lasi32不应该是最小的，三场一周期时，lasi14或者lasi32可能会最小，此时不进行检测，
       也就是非pulldown源时，lasi14或者lasi32最小时，fac=0
    5、lasi34最大时，不进行统计（可以解决横条纹纹理丰富时场序误检的情况）（如：主持人背景闪烁码流）
    6、横条纹纹理丰富时，可能lasi34最大，此时不检测，可能lasi34、lasi14、lasi32大小相当，此时检测的fac值要给上限，不超过50
    7、三场一周期时，若是三场一周期：
        （1）当lasi14最小时，lasi32≈lasi34，若lasi32≈lasi34不成立，则表明不是三场一周期，
        （2）当lasi32最小时，lasi14≈lasi34，若lasi14≈lasi34不成立，则表明不是三场一周期
    *******************************************************************************************************************/

static hi_s32 pq_mng_fod_detect_fld_order(reg_lasi_stat *p_lasi_stat, hi_s32 still_blk_cnt,
    alg_dei_drv_para *p_dei_drv_para, fod_ctrl_para *p_fod_ctrl_para)
{
    hi_s32 fac, z32, z14, z34;

    hi_s32 frm_width, frm_height; /* TODO ADD WIDTH HEIGHT */
    hi_s32 tmp_return_flag = 0;
    fod_lasi_info_struct p_fod_lasi_info = {0};
    hi_drv_pq_vpss_timming vpss_timming = {0};

    drv_pq_get_vpss_timming(&vpss_timming);

    frm_width = vpss_timming.width;
    frm_height = vpss_timming.height;

    if ((p_lasi_stat == HI_NULL) || (p_dei_drv_para == HI_NULL) || (p_fod_ctrl_para == HI_NULL)) {
        HI_ERR_PQ("ptr is NULL!\n");
        return 0;
    }

    pq_mng_get_fod_static_info(p_lasi_stat, p_dei_drv_para, p_fod_ctrl_para, &p_fod_lasi_info, &tmp_return_flag);

    z14 = p_fod_lasi_info.z14;
    z32 = p_fod_lasi_info.z32;
    z34 = p_fod_lasi_info.z34;
    if (1 == tmp_return_flag) {
        return p_fod_ctrl_para->field_order;
    }
    fac = pq_mng_get_fod_blv_percent(p_dei_drv_para, still_blk_cnt, z32, z14, z34);
    fac = pq_mng_fod_upt_single_percent(&p_fod_lasi_info, frm_width, frm_height, fac);
    /* temporal acc or dec ratio */
   /* yanxiumei  修改，解决字幕左右抖动的问题 */
#ifdef PQ_DPT_V900_SUPPORT
    if (p_pq_ctx->ast_vpss_timing_info[0].input_src == HI_DRV_SOURCE_DTV) {
        fod_acc_flag = 0;

        if ((frm_width == 1920) && (frm_height == 1080)) { /* 1920 1080 fhd */
            fod_acc_flag = 1;                              /* 1 in FHD always open */
            fod_inc_ratio = 2;                             /* 2 */
            fod_inc_th = 6;                                /* 6 */
        } else {
            if (fac != 0) {     /* 0 idx */
                fod_acc_flag = 1; /* 1 only when fac sure, open */
            }

            fod_inc_ratio = 3; /* 3 */
            fod_inc_th = 5;    /* 5 */
        }

        if (fod_acc_flag != 0) { /* 0 */
            fac = pq_mng_fod_tmp_upt_percent(p_fod_ctrl_para, fac, fod_inc_ratio, fod_inc_th, &flag);
        }
    }
#endif

    p_fod_ctrl_para->sad_diff_acc += fac;

    /* clip */
    p_fod_ctrl_para->sad_diff_acc = optm_alg_clip3((-1) * SADDIFF_ACC_THR, SADDIFF_ACC_THR,
                                                p_fod_ctrl_para->sad_diff_acc);

    /* get field order */
    if ((p_fod_ctrl_para->sad_diff_acc) > FIELD_ORDER_THR) {
        p_fod_ctrl_para->field_order = FIELD_TOP_FIRST;
    } else if ((p_fod_ctrl_para->sad_diff_acc) < -FIELD_ORDER_THR) {
        p_fod_ctrl_para->field_order = FIELD_BOTTOM_FIRST;
    } else {
        ;
    }

    return p_fod_ctrl_para->field_order;
}

static hi_void pq_mng_fod_calc_result(hi_drv_pq_vpss_fod_sw_in_para *p_cur_fod_swin_para,
    hi_drv_pq_vpss_fod_sw_stat_para *p_fod_swstat_para, alg_dei_drv_para *p_dei_drv_para,
    hi_drv_pq_vpss_fod_sw_out_para *p_fod_out_para)
{
    hi_s32 field_order = 2; /* 2 */
    fod_ctrl_para *p_fod_ctrl_para = &(p_fod_swstat_para->fod_ctrl_para);
    reg_lasi_stat st_cur_lasi_stat;
    hi_s32 s32_ret;

    s32_ret = memset_s(&st_cur_lasi_stat, sizeof(reg_lasi_stat), 0, sizeof(reg_lasi_stat));
    if (s32_ret != 0) {
        HI_ERR_PQ("st_cur_lasi_stat memset_s result is %d\n", s32_ret);
    }

    st_cur_lasi_stat.lasi_cnt14 = p_cur_fod_swin_para->lasi_stat_1.lasi_cnt14
                                 + p_cur_fod_swin_para->lasi_stat_2.lasi_cnt14;
    st_cur_lasi_stat.lasi_cnt32 = p_cur_fod_swin_para->lasi_stat_1.lasi_cnt32
                                 + p_cur_fod_swin_para->lasi_stat_2.lasi_cnt32;
    st_cur_lasi_stat.lasi_cnt34 = p_cur_fod_swin_para->lasi_stat_1.lasi_cnt34
                                 + p_cur_fod_swin_para->lasi_stat_2.lasi_cnt34;

    p_fod_swstat_para->still_blk_thd = (p_dei_drv_para->frm_width * p_dei_drv_para->frm_height) >> 11;  /* 11 */

    /* use curfod sad info, get curfod stillblkcnt */
    pq_mng_fod_detect_still_blk(&(p_cur_fod_swin_para->still_blk_info), p_fod_swstat_para->still_blk_thd,
        p_fod_ctrl_para);

    /* get pulldown detect */
    if (p_dei_drv_para->fod_enable == FOD_ENABLE) {
        field_order = pq_mng_fod_detect_fld_order(&st_cur_lasi_stat,
            p_cur_fod_swin_para->still_blk_info.still_blk_cnt, p_dei_drv_para, p_fod_ctrl_para);
    } else if (p_dei_drv_para->fod_enable == FOD_TOP_FIRST) {
        field_order = FIELD_TOP_FIRST;
    } else if (p_dei_drv_para->fod_enable == FOD_BOTTOM_FIRST) {
        field_order = FIELD_BOTTOM_FIRST;
    } else {
        field_order = FIELD_UNKNOWN;
    }

    p_fod_out_para->field_order = field_order;
    return;
}

static hi_s32 pq_mng_fod_calc_set(hi_drv_pq_vpss_fod_sw_stat_para *p_fod_swstat_para,
    hi_drv_pq_vpss_fod_sw_in_para *p_cur_fod_swin_para, alg_dei_drv_para *p_dei_drv_para,
    hi_drv_pq_vpss_fod_sw_out_para *p_fod_out_para)
{
    fod_ctrl_para *pst_fod_ctrl_para = &(p_fod_swstat_para->fod_ctrl_para);
    hi_bool is_snd_fld;
    hi_s32 dei_md_lum = p_dei_drv_para->dei_md_lum;
    hi_s32 ret = HI_SUCCESS;

    /* 第一次调用时无统计值，取默认值，主要为DirMch赋值 */
    /* 读统计信息----配置Fod输出逻辑 */
    if (p_fod_swstat_para->init_flag == 0) {
        p_fod_out_para->field_order = FIELD_UNKNOWN;
        p_fod_swstat_para->init_flag++;
        ret = memcpy_s(&(p_fod_swstat_para->rtl_out_para_bak), sizeof(hi_drv_pq_vpss_fod_sw_out_para),
            p_fod_out_para, sizeof(hi_drv_pq_vpss_fod_sw_out_para));
        if (ret != 0) {
            HI_ERR_PQ("p_fod_swstat_para->st_rtl_outpara_bak memcpy_s result is %d\n", ret);
        }

        return HI_SUCCESS;
    }
    ret = memset_s(p_fod_out_para, sizeof(hi_drv_pq_vpss_fod_sw_out_para), 0, sizeof(hi_drv_pq_vpss_fod_sw_out_para));
    if (ret != 0) {
        HI_ERR_PQ("p_fod_out_para memset_s result is %d\n", ret);
    }

    /*    T 0      T1      T2      T3
     *      B0      B1      B2      B3
    * 逻辑第一次上报的结果为B0为参考场时的数据, 因此第一次pstFodCtx 的参考场应该为B0;
    * 下一次配置给逻辑的参考为T1; 无论是否为离线模式, 都应该保证上述条件
    * Is2ndFld,NxtIs2ndFld,分别表示计算统计信息时逻辑的参考场及下一次配置的是否为一帧中的第二场
    * 场序检测的结果只会在一帧中的第二场上报(B1), 而驱动也只会在一帧的第二场采用
    * 场序检测的结果(B2), 因此T2场的场序依旧使用的是之前的场序
    * 驱动传的RefFld, 以及场序均为读统计信息时的那一场的场序和参考场,
    * 在离线情况下, 在计算统计信息的下一场读统计信息, 在线情况下, 在计算统计信息的下两场读统计信息
    * 最开始的初始
    * pstDeiDrvPara->BtMode  = pstStreamInfo->u32RealTopFirst
    * 驱动中的Btmod 表示top是为1 bot是表示 0，逻辑中的btmod 是 0表示top 1表示 bot
    * 软算法计算完fieldorder 之后，驱动进行了一次反转，
    * 为了软算法中的代码和算法模型中一直，所以需要把驱动中的botmod的反正，在这个函数中进行
    */

    if (p_dei_drv_para->bt_mode == 1) { /* 1 */
        p_dei_drv_para->bt_mode = 0; /* 0 */
    } else {
        p_dei_drv_para->bt_mode = 1; /* 1 */
    }

    pst_fod_ctrl_para->bt_mode = p_dei_drv_para->bt_mode;
    pst_fod_ctrl_para->ref_fld = p_dei_drv_para->ref_fld;

    if (((pst_fod_ctrl_para->bt_mode == FIELD_TOP_FIRST) && (pst_fod_ctrl_para->ref_fld == REF_TOP_FIRST))
        || ((pst_fod_ctrl_para->bt_mode == FIELD_BOTTOM_FIRST) && (pst_fod_ctrl_para->ref_fld == REF_BOTTOM_FIRST))) {
        is_snd_fld = 0; /* TF, TR */
    } else {
        is_snd_fld = 1; /* TF, BR */
    }

    pst_fod_ctrl_para->is_snd_fld = is_snd_fld;
    if ((ALG_DEI_MODE_5FLD == dei_md_lum) || (ALG_DEI_MODE_4FLD == dei_md_lum)) {
        pq_mng_fod_calc_result(p_cur_fod_swin_para, p_fod_swstat_para, p_dei_drv_para, p_fod_out_para);
    }

    ret = memcpy_s(&(p_fod_swstat_para->rtl_out_para_bak), sizeof(hi_drv_pq_vpss_fod_sw_out_para), p_fod_out_para,
        sizeof(hi_drv_pq_vpss_fod_sw_out_para));
    if (ret != 0) {
        HI_ERR_PQ("p_fod_swstat_para->st_rtl_outpara_bak memcpy_s result is %d\n", ret);
    }

    /* CNCommet: 涓轰簡涓嶅鍏朵粬绠楁硶閫犳垚褰卞搷锛岄渶瑕佹妸涔嬪墠椹卞姩涓弽杞殑btmod 鍙嶈浆鍥炴潵 */
    if (p_dei_drv_para->bt_mode == 1) {
        p_dei_drv_para->bt_mode = 0;
    } else {
        p_dei_drv_para->bt_mode = 1;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_mng_fod_upt_fod_frm_info(alg_dei_drv_para *p_dei_drv_para, hi_drv_pq_vpss_stt_info *p_fod_reg_in)
{
    p_dei_drv_para->dei_md_lum = ALG_DEI_MODE_4FLD;
    p_dei_drv_para->dei_md_chr = ALG_DEI_MODE_4FLD;
    p_dei_drv_para->frm_width = p_fod_reg_in->width;
    p_dei_drv_para->frm_height = p_fod_reg_in->height;
    p_dei_drv_para->bt_mode = (p_fod_reg_in->vdec_info.field_order == HI_DRV_PQ_FIELD_TOP_FIRST) ? 1 : 0;
    p_dei_drv_para->ref_fld = p_fod_reg_in->ref_field;

    p_dei_drv_para->fod_enable = (hi_s32)g_pq_fod_mode;
    p_dei_drv_para->pld22_enable = HI_TRUE;
    p_dei_drv_para->pld32_enable = HI_TRUE;

    return HI_SUCCESS;
}

static hi_s32 pq_mng_fod_core(hi_drv_pq_vpss_fod_sw_in_para *p_cur_fod_swin_para, hi_drv_pq_vpss_stt_info *p_fod_reg_in,
    hi_drv_pq_vpss_fod_sw_out_para *p_fod_swout_result)
{
    hi_drv_pq_vpss_fod_sw_stat_para *p_fod_swstat_para = &(g_pq_fod_total_sw_para_n[p_fod_reg_in->handle_id]);
    alg_dei_drv_para st_dei_drv_para;
    hi_s32 ret;

    ret = memset_s(&st_dei_drv_para, sizeof(st_dei_drv_para), 0, sizeof(st_dei_drv_para));
    if (ret != 0) {
        HI_ERR_PQ("st_dei_drv_para memset_s result is %d\n", ret);
    }

    ret = pq_mng_fod_upt_fod_frm_info(&st_dei_drv_para, p_fod_reg_in);
    PQ_CHECK_RETURN_SUCCESS(ret);
    /* in fact st_dei_drv_para is get from driver */
    ret = pq_mng_fod_calc_set(p_fod_swstat_para, p_cur_fod_swin_para, &st_dei_drv_para, p_fod_swout_result);

    return ret;
}

hi_s32 pq_mng_fod_init_param(hi_u32 handle_no)
{
    hi_drv_pq_vpss_fod_sw_stat_para *p_fod_swstat_para = &(g_pq_fod_total_sw_para_n[handle_no]);
    fod_ctrl_para *p_fod_ctrl_para = &(p_fod_swstat_para->fod_ctrl_para);
    hi_s32 s32_ret;

    p_fod_swstat_para->init_flag = 0;
    p_fod_swstat_para->still_blk_thd = 0;

    s32_ret = memset_s(p_fod_ctrl_para, sizeof(fod_ctrl_para), 0, sizeof(fod_ctrl_para));
    if (s32_ret != 0) {
        HI_ERR_PQ("p_fod_ctrl_para memset_s result is %d\n", s32_ret);
    }

    p_fod_ctrl_para->field_order = FIELD_UNKNOWN;

    return HI_SUCCESS;
}

hi_s32 pq_mng_field_order_dect(hi_drv_pq_vpss_stt_info *p_fod_calc, hi_s32 *p_field_order)
{
    static hi_drv_pq_vpss_fod_sw_in_para st_cur_fod_swin_para = { { 0 } };
    hi_s32 s32_ret;
    hi_drv_pq_vpss_fod_sw_out_para st_fod_swout_result;
    static hi_u8 pre_fld_order[6] = { 0 };  /* 6 */
    static hi_u8 pre_blv_order = 0; /* initial fod order topfirst */
    hi_u8 i, blv_fld_order_cnt;

    PQ_CHECK_NULL_PTR_RE_FAIL(p_fod_calc);
    PQ_CHECK_NULL_PTR_RE_FAIL(p_field_order);

    s32_ret = memset_s(&st_fod_swout_result, sizeof(hi_drv_pq_vpss_fod_sw_out_para), 0,
        sizeof(hi_drv_pq_vpss_fod_sw_out_para));
    if (s32_ret != 0) {
        HI_ERR_PQ("st_fod_swout_result memset_s result is %d\n", s32_ret);
        return HI_FAILURE;
    }

    for (i = PRE_FEILD_NUM; i > 0; i--) {
        pre_fld_order[i] = pre_fld_order[i - 1];
    }
    pre_fld_order[0] = p_fod_calc->vdec_info.frame_fmt;
    blv_fld_order_cnt = 0;

    for (i = PRE_FEILD_NUM; i > 0; i--) {
        if (pre_fld_order[i] == pre_fld_order[0]) {
            blv_fld_order_cnt = blv_fld_order_cnt + 1;
        }
    }

    /* From VPSS Get FodSttTablePara */
    /* CNCommet: 寰楀埌閫昏緫瀵勫瓨鍣?淇℃伅 鍖呮嫭lasi14銆?2銆?4銆乻tillblkinfo */
    pq_mng_fod_get_stt_info(&st_cur_fod_swin_para, p_fod_calc->vpss_stt_reg);
    /* Clac to pstFodResult */
    pq_mng_fod_core(&st_cur_fod_swin_para, p_fod_calc, &st_fod_swout_result);
    g_field_order = st_fod_swout_result.field_order;

    if (st_fod_swout_result.field_order == 2) { /* 2 */
        /* check decoder info correct or not, if correct, update, else no update */
        if (blv_fld_order_cnt >= PRE_FEILD_NUM) {
            pre_blv_order = p_fod_calc->vdec_info.field_order;
        }
        st_fod_swout_result.field_order = pre_blv_order;
    } else {
        pre_blv_order = st_fod_swout_result.field_order; /* if fod sure, update PreBlvOrder */
    }

    if (g_force_field_order != 2) { /* 2 */
        st_fod_swout_result.field_order = g_force_field_order;
    }
    *p_field_order = st_fod_swout_result.field_order;
    return HI_SUCCESS;
}

hi_s32 pq_mng_get_fod_detect_info(hi_u32 *field_order)
{
    *field_order = g_field_order;
    return HI_SUCCESS;
}

hi_s32 pq_mng_set_fod_mode(hi_u32 field_order)
{
    if (field_order == PQ_FOD_ENABLE_AUTO) {
        g_force_field_order = g_field_order;
    } else if (field_order == PQ_FOD_UNKOWN) {
        g_force_field_order = field_order;
    } else if (field_order >= PQ_FOD_MODE_MAX) {
        HI_ERR_PQ("field_order overrange\n", field_order);
        return HI_FAILURE;
    } else {
        g_force_field_order = field_order;
    }

    return HI_SUCCESS;
}

hi_s32 pq_mng_get_fod_mode(hi_u32 *field_order)
{
    *field_order = g_force_field_order;
    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
