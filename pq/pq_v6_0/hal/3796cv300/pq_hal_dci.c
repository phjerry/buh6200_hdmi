/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: dci hal layer source file
 * Author: pq
 * Create: 2019-01-01
 */

#include "pq_hal_comm.h"
#include "pq_hal_dci.h"
#include "pq_hal_dci_regset.h"
#include "hal_pq_ip_coef.h"

static vdp_regs_type *g_vdp_dci_reg = HI_NULL;
static pq_dci_coef_addr g_dci_hal_coef_buf = { 0 };

static dci_lut g_dci_lut = { HI_NULL };
static dci_gen_coef_lut g_gen_coef_lut = { 0 };

static hi_s32 g_hist_bin32_current[DCI_HIST_BIN32_NUM] = { 0 };
static hi_s32 g_hist_bin32_previous[DCI_HIST_BIN32_NUM] = { 0 };
static hi_u32 g_dci_soft_alg_frm_cnt = 0;
static hi_u32 g_dci_soft_alg_hist_sad[DCI_SAD_MAX_CNT] = { 0 };

static dci_bin_weight_lut g_lut_bin_weight_glb = {
    /* glb_0 */
    {16, 16, 16, 16, 13, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /* glb_1 */
    {16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16},
    /* glb_2 */
    {0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
     16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16 },
};

static dci_bin_weight_lut g_lut_bin_weight_lcl = {
    /* lcl_0 */
    {31, 31, 31, 31, 31, 29, 26, 23, 18, 14, 10, 6, 3, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /* lcl_1 */
    {0, 0, 0, 0, 0, 5, 9, 14, 18, 22, 25, 28, 30, 31, 31, 31, 31,
     31, 31, 28, 25, 22, 18, 14, 10, 5, 0, 0, 0, 0, 0, 0},
    /* lcl_2 */
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
     9, 12, 16, 19, 22, 25, 28, 31, 31, 31, 31, 31},
};

static dci_adjust_lut g_lut_glb_max = {
    /* glb_max_0 */
    {0, 0, 0, 17, 36, 56, 71, 74, 78, 84, 89, 88, 84, 76, 70, 67, 56, 52, 45, 41, 37, 31, 28,
     24, 24, 20, 16, 15, 11, 0, 0, 3, 0},
    /* glb_max_1 */
    {4, -4, -14, -22, -30, -39, -39, -37, -29, -23, -15, -4, 4, 13, 24, 36, 50, 60, 72, 84,
     94, 93, 87, 77, 65, 51, 34, 22, 4, 0, 0, 0, 0},
    /* glb_max_2 */
    {0, 0, 0, -10, -26, -42, -55, -71, -79, -87, -88, -90, -86, -84, -79, -77, -72, -69,
     -67, -64, -58, -50, -40, -35, -27, -22, -19, -9, 1, 6, 0, 0, 0},
};

static dci_adjust_lut g_lut_lcl_max = {
    /* lcl_max_0 */
    {0, 0, -11, -18, -13, -1, 16, 28, 38, 36, 37, 38, 39, 38, 35, 38, 39, 35, 34, 31, 32, 33,
     34, 33, 36, 30, 31, 32, 31, 24, 8, 0, 0},
    /* lcl_max_1 */
    {0, 0, 0, -7, -19, -29, -37, -45, -53, -54, -53, -52, -51, -43, -27, -10, 11, 32, 52,
     67, 75, 86, 93, 91, 93, 86, 70, 59, 35, 18, 0, 0, 0},
    /* lcl_max_2 */
    {0, 0, 0, -16, -30, -34, -38, -42, -44, -50, -57, -63, -64, -70, -69, -74, -71, -70,
     -69, -68, -65, -53, -38, -15, 6, 24, 41, 46, 38, 22, 0, 0, 0},
};

static hi_s32 g_lut_bs_delta[LUT_BS_DELTA_NUM] = {
    640, 636, 632, 628, 624, 620, 616, 612, 608, 605, 601, 597, 593, 589, 585, 581, 578, 574, 570, 566,
    563, 559, 555, 551, 548, 544, 540, 537, 533, 529, 526, 522, 518, 515, 511, 508, 504, 501, 497, 494,
    490, 487, 483, 480, 476, 473, 469, 466, 462, 459, 456, 452, 449, 446, 442, 439, 436, 432, 429, 426,
    423, 419, 416, 413, 410, 406, 403, 400, 397, 394, 391, 388, 384, 381, 378, 375, 372, 369, 366, 363,
    360, 357, 354, 351, 348, 345, 342, 339, 336, 334, 331, 328, 325, 322, 319, 316, 314, 311, 308, 305,
    303, 300, 297, 294, 292, 289, 286, 284, 281, 278, 276, 273, 270, 268, 265, 263, 260, 258, 255, 253,
    250, 248, 245, 243, 240, 238, 235, 233, 230, 228, 226, 223, 221, 219, 216, 214, 212, 209, 207, 205,
    203, 200, 198, 196, 194, 191, 189, 187, 185, 183, 181, 179, 176, 174, 172, 170, 168, 166, 164, 162,
    160, 158, 156, 154, 152, 150, 148, 146, 144, 143, 141, 139, 137, 135, 133, 131, 130, 128, 126, 124,
    123, 121, 119, 117, 116, 114, 112, 111, 109, 107, 106, 104, 102, 101, 99, 98, 96, 95, 93, 92,
    90, 89, 87, 86, 84, 83, 81, 80, 78, 77, 76, 74, 73, 72, 70, 69, 68, 66, 65, 64,
    63, 61, 60, 59, 58, 56, 55, 54, 53, 52, 51, 50, 48, 47, 46, 45, 44, 43, 42, 41,
    40, 39, 38, 37, 36, 35, 34, 33, 32, 32, 31, 30, 29, 28, 27, 26, 26, 25, 24, 23,
    23, 22, 21, 20, 20, 19, 18, 18, 17, 16, 16, 15, 14, 14, 13, 13, 12, 12, 11, 11,
    10, 10, 9, 9, 8, 8, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3,
    3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};

static hi_s32 g_lut_voffset[LUT_VOFFSET_NUM] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 24, 25, 26, 27, 28, 29, 31, 32, 33, 34, 36, 37, 38, 39, 41, 42, 43, 44,
    46, 47, 48, 50, 51, 52, 54, 55, 56, 58, 59, 61, 62, 64, 65, 66, 68, 69, 71, 72,
    74, 75, 77, 78, 80, 82, 83, 85, 86, 88, 90, 91, 93, 95, 96, 98, 100, 101, 103, 105,
    107, 108, 110, 112, 114, 116, 118, 119, 121, 123, 125, 127, 129, 131, 133, 135, 137, 139, 141, 143,
    145, 148, 150, 152, 154, 156, 159, 161, 163, 165, 168, 170, 172, 175, 177, 180, 182, 184, 187, 189,
    192, 195, 197, 200, 202, 205, 208, 211, 213, 216, 219, 222, 225, 228, 231, 234, 237, 240, 243, 246,
    249, 252, 255, 259, 262, 265, 269, 272, 275, 279, 282, 286, 290, 293, 297, 301, 304, 308, 312, 316};

static hi_s16 g_lut_uv_div[LUT_UV_DIV_NUM] = {
    1, 4095, 2048, 1365, 1024, 819, 682, 585, 512, 455, 409, 372, 341, 315, 292, 273, 256, 240, 227, 215,
    204, 195, 186, 178, 170, 163, 157, 151, 146, 141, 136, 132, 128, 124, 120, 117, 113, 110, 107, 105,
    102, 99, 97, 95, 93, 91, 89, 87, 85, 83, 81, 80, 78, 77, 75, 74, 73, 71, 70, 69,
    68, 67, 66, 65};

static hi_s32 g_lut_skin[LUT_SKIN_PRO_NUM] = {
   /* 0    1     2    3   4    5    6    7    8     9    10   11   12   13   14   15 */
    16,  16,  16,  16,  16,  16,  16,  16,  16, 16,  16,  16,  16,  16,  16,  16, /* 0 */
    16,  16,  16,  16,  16,  16,  16,  16,  16, 16,  16,  16,  16,  16,  16,  16, /* 1 */
    16,  16,  16,  16,  16,  16,  16,  16,  16, 16,  16,  16,  16,  16,  16,  16, /* 2 */
    16,  16,  16,  16,  16,  16,  16,  16,  16, 16,  16,  16,  16,  16,  16,  16, /* 3 */
    16,  16,  16,  16,  16,  16,  16,  16,  16, 16,  16,  16,  16,  16,  16,  16, /* 4 */
    16,  16,  16,  16,  16,  16,  16,  16,  16, 16,  16,  16,  16,  16,  16,  16, /* 5 */

    16,  16,  16,  16,  16,  16,  16,  16,  16, 16,  16,  16,  16,  16,  16,  16, /* 6 */
    16,  16,  16,  16,  16,  16,  16,  16,  16, 16,  16,  16,  16,  16,  16,  16, /* 7 */
    16,  16,  16,  16,  16,  16,  16,  16,  8,  16,  16,  16,  16,  16,  16,  16, /* 8 */
    16,  16,  16,  16,  16,  16,   8,  8,   16, 16,  16,  16,  16,  16,  16,  16, /* 9 */
    16,  16,  16,  16,  16,  16,  16,  16,  16, 16,  16,  16,  16,  16,  16,  16, /* 10 */

    16,  16,  16,  16,  16,  16,  16,  16,  16, 16,  16,  16,  16,  16,  16,  16, /* 11 */
    16,  16,  16,  16,  16,  16,  16,  16,  16, 16,  16,  16,  16,  16,  16,  16, /* 12 */
    16,  16,  16,  16,  16,  16,  16,  16,  16, 16,  16,  16,  16,  16,  16,  16, /* 13 */
    16,  16,  16,  16,  16,  16,  16,  16,  16, 16,  16,  16,  16,  16,  16,  16, /* 14 */
    16,  16,  16,  16,  16,  16,  16,  16,  16, 16,  16,  16,  16,  16,  16,  16  /* 15 */
};

static hi_s32 pq_hal_dci_set_para_addr(const pq_dci_coef_addr *dci_coef_buf_addr)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_dci_reg);

    pq_reg_dci_para_haddr_v0_chn15(g_vdp_dci_reg, dci_coef_buf_addr->phy_addr[DCI_COEF_BUF_DCI0]>>32); /* 32:bits */
    pq_reg_dci_para_addr_v0_chn15(g_vdp_dci_reg, dci_coef_buf_addr->phy_addr[DCI_COEF_BUF_DCI0]);

    return HI_SUCCESS;
}

hi_s32 pq_hal_dci_reset_addr(hi_void)
{
    g_vdp_dci_reg = HI_NULL;
    memset(&g_dci_hal_coef_buf, 0x0, sizeof(g_dci_hal_coef_buf));
    memset(&g_dci_lut, 0x0, sizeof(g_dci_lut));

    return HI_SUCCESS;
}

static hi_void pq_hal_dci_init_global_lut(hi_void)
{
    g_dci_lut.bin_weight_lut[0] = &g_lut_bin_weight_glb;
    g_dci_lut.bin_weight_lut[1] = &g_lut_bin_weight_lcl;
    g_dci_lut.adjust_lut[0]     = &g_lut_glb_max;
    g_dci_lut.adjust_lut[1]     = &g_lut_lcl_max;
    g_dci_lut.bs_delta_lut      = &g_lut_bs_delta[0];
    g_dci_lut.v_offset_lut      = &g_lut_voffset[0];
    g_dci_lut.uv_div_lut        = &g_lut_uv_div[0];
    g_dci_lut.skin_pro_lut      = &g_lut_skin[0];

    return;
}

hi_s32 pq_hal_dci_distribute_addr(hi_u8 *vir_addr, dma_addr_t phy_addr)
{
    hi_s32 ret;

    if (vir_addr == HI_NULL || phy_addr == HI_NULL) {
        HI_ERR_PQ("vir_addr or phy_addr invalid!\n");
        return HI_ERR_PQ_INVALID_PARA;
    }

    g_vdp_dci_reg = pq_hal_get_vdp_set_reg();
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_dci_reg);

    g_dci_hal_coef_buf.vir_addr[DCI_COEF_BUF_DCI0] = vir_addr;
    g_dci_hal_coef_buf.phy_addr[DCI_COEF_BUF_DCI0] = phy_addr;

    ret = pq_hal_dci_set_para_addr(&g_dci_hal_coef_buf);
    if (ret != HI_SUCCESS) {
        HI_ERR_PQ("pq_hal_dci_set_para_addr fail!\n");
        pq_hal_dci_reset_addr();
        return ret;
    }

    /* init lut addr */
    pq_hal_dci_init_global_lut();

    return HI_SUCCESS;
}

static hi_void pq_hal_dci_gen_coef(hi_void *coef_array, hi_void *coef_array_new,
                                   hi_u32 len, pq_coef_data_type data_type)
{
    pq_coef_gen_cfg coef_gen = { 0 };

    coef_gen.p_coef         = coef_array;
    coef_gen.p_coef_new     = coef_array_new;
    coef_gen.length         = len;
    coef_gen.coef_data_type = data_type;

    pq_drv_gen_coef(&coef_gen);

    return ;
}

static hi_s32 pq_hal_dci_gen_bin_weight_coef(hi_void)
{
    hi_u32 i;

    PQ_CHECK_NULL_PTR_RE_FAIL(g_dci_lut.bin_weight_lut[0]); /* 0:glb */
    PQ_CHECK_NULL_PTR_RE_FAIL(g_dci_lut.bin_weight_lut[1]); /* 1:lcl */

    for (i = 0; i < LUT_BIN_WEIGHT_NUM; i++) {
        /* lut_glb_bw */
        g_gen_coef_lut.glb_bw[0].lut0[i] = g_dci_lut.bin_weight_lut[0]->lut0[i];
        g_gen_coef_lut.glb_bw[0].lut1[i] = g_dci_lut.bin_weight_lut[0]->lut1[i];
        g_gen_coef_lut.glb_bw[0].lut2[i] = g_dci_lut.bin_weight_lut[0]->lut2[i];
        /* lut_lcl_bw */
        g_gen_coef_lut.lcl_bw[0].lut0[i] = g_dci_lut.bin_weight_lut[1]->lut0[i];
        g_gen_coef_lut.lcl_bw[0].lut1[i] = g_dci_lut.bin_weight_lut[1]->lut1[i];
        g_gen_coef_lut.lcl_bw[0].lut2[i] = g_dci_lut.bin_weight_lut[1]->lut2[i];
    }

    pq_hal_dci_gen_coef((hi_void *)&g_gen_coef_lut.glb_bw[0].lut0[0], (hi_void *)&g_gen_coef_lut.glb_bw[1].lut0[0],
                        LUT_BIN_WEIGHT_NUM, DRV_COEF_DATA_TYPE_U8);
    pq_hal_dci_gen_coef((hi_void *)&g_gen_coef_lut.glb_bw[0].lut1[0], (hi_void *)&g_gen_coef_lut.glb_bw[1].lut1[0],
                        LUT_BIN_WEIGHT_NUM, DRV_COEF_DATA_TYPE_U8);
    pq_hal_dci_gen_coef((hi_void *)&g_gen_coef_lut.glb_bw[0].lut2[0], (hi_void *)&g_gen_coef_lut.glb_bw[1].lut2[0],
                        LUT_BIN_WEIGHT_NUM, DRV_COEF_DATA_TYPE_U8);

    pq_hal_dci_gen_coef((hi_void *)&g_gen_coef_lut.lcl_bw[0].lut0[0], (hi_void *)&g_gen_coef_lut.lcl_bw[1].lut0[0],
                        LUT_BIN_WEIGHT_NUM, DRV_COEF_DATA_TYPE_U8);
    pq_hal_dci_gen_coef((hi_void *)&g_gen_coef_lut.lcl_bw[0].lut1[0], (hi_void *)&g_gen_coef_lut.lcl_bw[1].lut1[0],
                        LUT_BIN_WEIGHT_NUM, DRV_COEF_DATA_TYPE_U8);
    pq_hal_dci_gen_coef((hi_void *)&g_gen_coef_lut.lcl_bw[0].lut2[0], (hi_void *)&g_gen_coef_lut.lcl_bw[1].lut2[0],
                        LUT_BIN_WEIGHT_NUM, DRV_COEF_DATA_TYPE_U8);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_dci_gen_adjust_lut_coef(hi_void)
{
    hi_u32 i;

    PQ_CHECK_NULL_PTR_RE_FAIL(g_dci_lut.adjust_lut[0]); /* 0:glb */
    PQ_CHECK_NULL_PTR_RE_FAIL(g_dci_lut.adjust_lut[1]); /* 1:lcl */

    for (i = 0; i < LUT_ADJUST_COEF_NUM; i++) {
        if (i % 3 == 0) { /* 3:table num */
            g_gen_coef_lut.lutglb[0].lut[i] = g_dci_lut.adjust_lut[0]->lut0[i / 3]; /* 3:table num */
            g_gen_coef_lut.lutlcl[0].lut[i] = g_dci_lut.adjust_lut[1]->lut0[i / 3]; /* 3:table num */
        } else if (i % 3 == 1) {
            g_gen_coef_lut.lutglb[0].lut[i] = g_dci_lut.adjust_lut[0]->lut1[i / 3]; /* 3:table num */
            g_gen_coef_lut.lutlcl[0].lut[i] = g_dci_lut.adjust_lut[1]->lut1[i / 3]; /* 3:table num */
        } else {
            g_gen_coef_lut.lutglb[0].lut[i] = g_dci_lut.adjust_lut[0]->lut2[i / 3]; /* 3:table num */
            g_gen_coef_lut.lutlcl[0].lut[i] = g_dci_lut.adjust_lut[1]->lut2[i / 3]; /* 3:table num */
        }
    }

    pq_hal_dci_gen_coef((hi_void *)&g_gen_coef_lut.lutglb[0].lut[0], (hi_void *)&g_gen_coef_lut.lutglb[1].lut[0],
                        LUT_ADJUST_COEF_NUM, DRV_COEF_DATA_TYPE_S16);
    pq_hal_dci_gen_coef((hi_void *)&g_gen_coef_lut.lutlcl[0].lut[0], (hi_void *)&g_gen_coef_lut.lutlcl[1].lut[0],
                        LUT_ADJUST_COEF_NUM, DRV_COEF_DATA_TYPE_S16);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_dci_gen_bs_delta_coef(hi_void)
{
    hi_u32 i;

    PQ_CHECK_NULL_PTR_RE_FAIL(g_dci_lut.bs_delta_lut);

    for (i = 0; i < LUT_BS_DELTA_NUM; i++) {
        g_gen_coef_lut.lutbsd[0].lut[i] = g_dci_lut.bs_delta_lut[i];
    }

    pq_hal_dci_gen_coef((hi_void *)&g_gen_coef_lut.lutbsd[0].lut[0], (hi_void *)&g_gen_coef_lut.lutbsd[1].lut[0],
                        LUT_BS_DELTA_NUM, DRV_COEF_DATA_TYPE_U16);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_dci_gen_voffset_coef(hi_void)
{
    hi_u32 i;

    PQ_CHECK_NULL_PTR_RE_FAIL(g_dci_lut.v_offset_lut);

    for (i = 0; i < LUT_VOFFSET_NUM; i++) {
        g_gen_coef_lut.lutvof[0].lut[i] = g_dci_lut.v_offset_lut[i];
    }

    pq_hal_dci_gen_coef((hi_void *)&g_gen_coef_lut.lutvof[0].lut[0], (hi_void *)&g_gen_coef_lut.lutvof[1].lut[0],
                        LUT_VOFFSET_NUM, DRV_COEF_DATA_TYPE_U16);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_dci_gen_uv_div_coef(hi_void)
{
    hi_u32 i;

    PQ_CHECK_NULL_PTR_RE_FAIL(g_dci_lut.uv_div_lut);

    for (i = 0; i < LUT_UV_DIV_NUM; i++) {
        g_gen_coef_lut.lutuvd[0].lut[i] = g_dci_lut.uv_div_lut[i];
    }

    pq_hal_dci_gen_coef((hi_void *)&g_gen_coef_lut.lutuvd[0].lut[0], (hi_void *)&g_gen_coef_lut.lutuvd[1].lut[0],
                        LUT_UV_DIV_NUM, DRV_COEF_DATA_TYPE_U16);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_dci_gen_skin_coef(hi_void)
{
    hi_u32 i;

    PQ_CHECK_NULL_PTR_RE_FAIL(g_dci_lut.skin_pro_lut);

    for (i = 0; i < LUT_SKIN_PRO_NUM; i++) {
        g_gen_coef_lut.lutskp[0].lut[i] = g_dci_lut.skin_pro_lut[i];
    }

    pq_hal_dci_gen_coef((hi_void *)&g_gen_coef_lut.lutskp[0].lut[0], (hi_void *)&g_gen_coef_lut.lutskp[1].lut[0],
                        LUT_SKIN_PRO_NUM, DRV_COEF_DATA_TYPE_U8);

    return HI_SUCCESS;
}

static hi_u8 *pq_hal_dci_send_glb_bw_coef(hi_u8 *addr, hi_u8 *glb_bw_0, hi_u8 *glb_bw_1, hi_u8 *glb_bw_2)
{
    pq_coef_send_cfg coef_send = {0};
    hi_u32 lut_lenght = LUT_BIN_WEIGHT_NUM;
    hi_u32 coef_bit_len = 8;
    hi_void *coef_array[1];

    coef_array[0] = glb_bw_0;
    coef_send.coef_addr        = addr;
    coef_send.sti_type         = PQ_STI_FILE_COEF_DCI;
    coef_send.fp_coef          = HI_NULL;
    coef_send.lut_num          = 1;
    coef_send.burst_num        = 1;
    coef_send.cycle_num        = 16; /* 16: cycle num */
    coef_send.p_coef_array     = coef_array;
    coef_send.lut_length       = &lut_lenght;
    coef_send.coef_bit_length  = &coef_bit_len;
    coef_send.data_type        = DRV_COEF_DATA_TYPE_U8;
    addr = pq_send_coef2ddr(&coef_send);

    coef_array[0] = glb_bw_1;
    coef_send.coef_addr        = addr;
    coef_send.p_coef_array     = coef_array;
    addr = pq_send_coef2ddr(&coef_send);

    coef_array[0] = glb_bw_2;
    coef_send.coef_addr        = addr;
    coef_send.p_coef_array     = coef_array;
    addr = pq_send_coef2ddr(&coef_send);

    return addr;
}

static hi_u8 *pq_hal_dci_send_lcl_bw_coef(hi_u8 *addr, hi_u8 *lcl_bw_0, hi_u8 *lcl_bw_1, hi_u8 *lcl_bw_2)
{
    pq_coef_send_cfg coef_send = { 0 };
    hi_u32 lut_lenght = LUT_BIN_WEIGHT_NUM;
    hi_u32 coef_bit_len = 8;
    hi_void *coef_array[1] = { HI_NULL };

    coef_array[0] = lcl_bw_0;
    coef_send.coef_addr        = addr;
    coef_send.sti_type         = PQ_STI_FILE_COEF_DCI;
    coef_send.fp_coef          = HI_NULL;
    coef_send.lut_num          = 1;
    coef_send.burst_num        = 1;
    coef_send.cycle_num        = 16; /* 16: cycle num */
    coef_send.p_coef_array     = coef_array;
    coef_send.lut_length       = &lut_lenght;
    coef_send.coef_bit_length  = &coef_bit_len;
    coef_send.data_type        = DRV_COEF_DATA_TYPE_U8;
    addr = pq_send_coef2ddr(&coef_send);

    coef_array[0] = lcl_bw_1;
    coef_send.coef_addr        = addr;
    coef_send.p_coef_array     = coef_array;
    addr = pq_send_coef2ddr(&coef_send);

    coef_array[0] = lcl_bw_2;
    coef_send.coef_addr        = addr;
    coef_send.p_coef_array     = coef_array;
    addr = pq_send_coef2ddr(&coef_send);

    return addr;
}

static hi_u8 *pq_hal_dci_send_lut_glb_coef(hi_u8 *addr, hi_s16 *lut_glb)
{
    pq_coef_send_cfg coef_send = { 0 };
    hi_u32 lut_lenght = LUT_ADJUST_COEF_NUM;
    hi_u32 coef_bit_len = 12;
    hi_void *coef_array[1];

    coef_array[0] = lut_glb;
    coef_send.coef_addr        = addr;
    coef_send.sti_type         = PQ_STI_FILE_COEF_DCI;
    coef_send.fp_coef          = HI_NULL;
    coef_send.lut_num          = 1;
    coef_send.burst_num        = 1;
    coef_send.cycle_num        = 9; /* 9: cycle num */
    coef_send.p_coef_array     = coef_array;
    coef_send.lut_length       = &lut_lenght;
    coef_send.coef_bit_length  = &coef_bit_len;
    coef_send.data_type        = DRV_COEF_DATA_TYPE_S16;
    addr = pq_send_coef2ddr(&coef_send);

    return addr;
}

static hi_u8 *pq_hal_dci_send_lut_lcl_coef(hi_u8 *addr, hi_s16 *lut_lcl)
{
    pq_coef_send_cfg coef_send = { 0 };
    hi_u32 lut_lenght = LUT_ADJUST_COEF_NUM;
    hi_u32 coef_bit_len = 12;
    hi_void *coef_array[1];

    coef_array[0] = lut_lcl;
    coef_send.coef_addr        = addr;
    coef_send.sti_type         = PQ_STI_FILE_COEF_DCI;
    coef_send.fp_coef          = HI_NULL;
    coef_send.lut_num          = 1;
    coef_send.burst_num        = 1;
    coef_send.cycle_num        = 9; /* 9: cycle num */
    coef_send.p_coef_array     = coef_array;
    coef_send.lut_length       = &lut_lenght;
    coef_send.coef_bit_length  = &coef_bit_len;
    coef_send.data_type        = DRV_COEF_DATA_TYPE_S16;
    addr = pq_send_coef2ddr(&coef_send);

    return addr;
}

static hi_u8 *pq_hal_dci_send_bs_delta_coef(hi_u8 *addr, hi_u16 *lut_bsd)
{
    pq_coef_send_cfg coef_send = { 0 };
    hi_u32 lut_lenght = LUT_BS_DELTA_NUM;
    hi_u32 coef_bit_len = 12;
    hi_void *coef_array[1];

    coef_array[0] = lut_bsd;
    coef_send.coef_addr        = addr;
    coef_send.sti_type         = PQ_STI_FILE_COEF_DCI;
    coef_send.fp_coef          = HI_NULL;
    coef_send.lut_num          = 1;
    coef_send.burst_num        = 1;
    coef_send.cycle_num        = 10; /* 10: cycle num */
    coef_send.p_coef_array     = coef_array;
    coef_send.lut_length       = &lut_lenght;
    coef_send.coef_bit_length  = &coef_bit_len;
    coef_send.data_type        = DRV_COEF_DATA_TYPE_U16;
    addr = pq_send_coef2ddr(&coef_send);

    return addr;
}

static hi_u8 *pq_hal_dci_send_v_offset_coef(hi_u8 *addr, hi_u16 *lut_vof)
{
    pq_coef_send_cfg coef_send = { 0 };
    hi_u32 lut_lenght = LUT_VOFFSET_NUM;
    hi_u32 coef_bit_len = 12;
    hi_void *coef_array[1];

    coef_array[0] = lut_vof;
    coef_send.coef_addr        = addr;
    coef_send.sti_type         = PQ_STI_FILE_COEF_DCI;
    coef_send.fp_coef          = HI_NULL;
    coef_send.lut_num          = 1;
    coef_send.burst_num        = 1;
    coef_send.cycle_num        = 10; /* 10: cycle num */
    coef_send.p_coef_array     = coef_array;
    coef_send.lut_length       = &lut_lenght;
    coef_send.coef_bit_length  = &coef_bit_len;
    coef_send.data_type        = DRV_COEF_DATA_TYPE_U16;
    addr = pq_send_coef2ddr(&coef_send);

    return addr;
}

static hi_u8 *pq_hal_dci_send_uv_div_coef(hi_u8 *addr, hi_u16 *lut_uvd)
{
    pq_coef_send_cfg coef_send = { 0 };
    hi_u32 lut_lenght = LUT_UV_DIV_NUM;
    hi_u32 coef_bit_len = 12;
    hi_void *coef_array[1];

    coef_array[0] = lut_uvd;
    coef_send.coef_addr        = addr;
    coef_send.sti_type         = PQ_STI_FILE_COEF_DCI;
    coef_send.fp_coef          = HI_NULL;
    coef_send.lut_num          = 1;
    coef_send.burst_num        = 1;
    coef_send.cycle_num        = 10; /* 10: cycle num */
    coef_send.p_coef_array     = coef_array;
    coef_send.lut_length       = &lut_lenght;
    coef_send.coef_bit_length  = &coef_bit_len;
    coef_send.data_type        = DRV_COEF_DATA_TYPE_U16;
    addr = pq_send_coef2ddr(&coef_send);

    return addr;
}

static hi_u8 *pq_hal_dci_send_skin_coef(hi_u8 *addr, hi_u8 *lut_skp)
{
    pq_coef_send_cfg coef_send = { 0 };
    hi_u32 lut_lenght = LUT_SKIN_PRO_NUM;
    hi_u32 coef_bit_len = 8;
    hi_void *coef_array[1];

    coef_array[0] = lut_skp;
    coef_send.coef_addr        = addr;
    coef_send.sti_type         = PQ_STI_FILE_COEF_DCI;
    coef_send.fp_coef          = HI_NULL;
    coef_send.lut_num          = 1;
    coef_send.burst_num        = 1;
    coef_send.cycle_num        = 16; /* 16: cycle num */
    coef_send.p_coef_array     = coef_array;
    coef_send.lut_length       = &lut_lenght;
    coef_send.coef_bit_length  = &coef_bit_len;
    coef_send.data_type        = DRV_COEF_DATA_TYPE_U8;
    addr = pq_send_coef2ddr(&coef_send);

    return addr;
}

static hi_s32 pq_hal_dci_send_coef(dci_gen_coef_lut *coef_to_sent)
{
    hi_u8 *addr = HI_NULL;

    PQ_CHECK_NULL_PTR_RE_FAIL(coef_to_sent);

    addr = g_dci_hal_coef_buf.vir_addr[DCI_COEF_BUF_DCI0];
    PQ_CHECK_NULL_PTR_RE_FAIL(addr);

    addr = pq_hal_dci_send_glb_bw_coef(addr, &coef_to_sent->glb_bw[1].lut0[0],
                                       &coef_to_sent->glb_bw[1].lut1[0], &coef_to_sent->glb_bw[1].lut2[0]);
    addr = pq_hal_dci_send_lcl_bw_coef(addr, &coef_to_sent->lcl_bw[1].lut0[0],
                                       &coef_to_sent->lcl_bw[1].lut1[0], &coef_to_sent->lcl_bw[1].lut2[0]);
    addr = pq_hal_dci_send_lut_glb_coef(addr, &coef_to_sent->lutglb[1].lut[0]);
    addr = pq_hal_dci_send_lut_lcl_coef(addr, &coef_to_sent->lutlcl[1].lut[0]);
    addr = pq_hal_dci_send_bs_delta_coef(addr, &coef_to_sent->lutbsd[1].lut[0]);
    addr = pq_hal_dci_send_v_offset_coef(addr, &coef_to_sent->lutvof[1].lut[0]);
    addr = pq_hal_dci_send_uv_div_coef(addr, &coef_to_sent->lutuvd[1].lut[0]);
    addr = pq_hal_dci_send_skin_coef(addr, &coef_to_sent->lutskp[1].lut[0]);

    return HI_SUCCESS;
}

hi_s32 pq_hal_dci_set_en(hi_bool enable)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_dci_reg);

    pq_reg_dci_set_en(g_vdp_dci_reg, 0, enable);

    return HI_SUCCESS;
}

hi_s32 pq_hal_dci_set_demo_en(hi_bool enable)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_dci_reg);

    pq_reg_dci_set_demo_en(g_vdp_dci_reg, 0, enable);

    return HI_SUCCESS;
}

hi_s32 pq_hal_dci_set_demo_mode(pq_demo_mode demo_mode)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_dci_reg);

    pq_reg_dci_set_demo_mode(g_vdp_dci_reg, 0, demo_mode);
    pq_reg_dci_set_demo_mode_s0(g_vdp_dci_reg, 0, demo_mode);
    pq_reg_dci_set_demo_mode_s1(g_vdp_dci_reg, 0, demo_mode);

    return HI_SUCCESS;
}

hi_s32 pq_hal_dci_set_demo_pos(hi_u32 width, hi_u32 height, hi_u32 demo_pos)
{
    hi_u32  split_width0;
    hi_bool split_en;
    hi_u32  demo_s0_pos;
    hi_u32  demo_s1_pos;

    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_dci_reg);

    /* split. */
    split_en = (width >= 256) ? HI_TRUE : HI_FALSE; /* 256: min width. */
    split_width0 = (width / 2) / 4 * 4; /* 2,4,4: calculate split width. */

    demo_s0_pos = (split_en == HI_FALSE) ? demo_pos : (demo_pos <= split_width0 ? demo_pos : split_width0);
    demo_s1_pos = (split_en == HI_FALSE) ? demo_pos : \
        (demo_pos <= split_width0 ? 32 : demo_pos - split_width0 + 32); /* 32:overlap. */

    pq_reg_dci_set_demo_split_pos(g_vdp_dci_reg, 0, demo_pos);
    pq_reg_dci_set_demo_split_pos_s0(g_vdp_dci_reg, 0, demo_s0_pos);
    pq_reg_dci_set_demo_split_pos_s1(g_vdp_dci_reg, 0, demo_s1_pos);

    return HI_SUCCESS;
}

hi_s32 pq_hal_dci_set_strength(hi_u32 global_gain0, hi_u32 global_gain1, hi_u32 global_gain2)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_dci_reg);

    pq_reg_dci_set_global_gain0(g_vdp_dci_reg, 0, global_gain0);
    pq_reg_dci_set_global_gain1(g_vdp_dci_reg, 0, global_gain1);
    pq_reg_dci_set_global_gain2(g_vdp_dci_reg, 0, global_gain2);

    pq_reg_dci_set_global_gain0_s0(g_vdp_dci_reg, 0, global_gain0);
    pq_reg_dci_set_global_gain1_s0(g_vdp_dci_reg, 0, global_gain1);
    pq_reg_dci_set_global_gain2_s0(g_vdp_dci_reg, 0, global_gain2);

    pq_reg_dci_set_global_gain0_s1(g_vdp_dci_reg, 0, global_gain0);
    pq_reg_dci_set_global_gain1_s1(g_vdp_dci_reg, 0, global_gain1);
    pq_reg_dci_set_global_gain2_s1(g_vdp_dci_reg, 0, global_gain2);

    return HI_SUCCESS;
}

static hi_void pq_hal_dci_set_link_type(vdp_regs_type *vdp_reg, pq_dci_link_type link_type)
{
    hi_u32 link_mode;

    link_mode = (link_type == DCI_LINK_BEFORE_SR) ? 0 : 1;
    pq_reg_dci_set_link_ctrl(vdp_reg, link_mode);

    return;
}

static hi_s32 pq_hal_dci_set_coef(vdp_regs_type *vdp_reg)
{
    hi_s32 ret;

    /* gen coef. */
    ret = pq_hal_dci_gen_bin_weight_coef();
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_dci_gen_adjust_lut_coef();
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_dci_gen_bs_delta_coef();
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_dci_gen_voffset_coef();
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_dci_gen_uv_div_coef();
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_dci_gen_skin_coef();
    PQ_CHECK_RETURN_SUCCESS(ret);

    /* send coef. */
    ret = pq_hal_dci_send_coef(&g_gen_coef_lut);
    PQ_CHECK_RETURN_SUCCESS(ret);

    pq_reg_dci_setparaupv0chn(vdp_reg, DCI_COEF_BUF_DCI0);

    return HI_SUCCESS;
}

hi_s32 pq_hal_dci_get_mean_value(hi_u32 *mean_value)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(mean_value);
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_dci_reg);

    pq_reg_dci_get_mean_value(g_vdp_dci_reg, 0, mean_value);

    return HI_SUCCESS;
}

hi_s32 pq_hal_dci_set_scd_en(hi_bool enable)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_dci_reg);

    pq_reg_dci_set_scd_en(g_vdp_dci_reg, 0, enable);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_dci_get_sad_para(hi_u32 *sad_thrsh, hi_u32 *sad_shift)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(sad_thrsh);
    PQ_CHECK_NULL_PTR_RE_FAIL(sad_shift);
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_dci_reg);

    pq_reg_dci_get_scd_sad_thrsh(g_vdp_dci_reg, 0, sad_thrsh);
    pq_reg_dci_get_scd_sad_shift(g_vdp_dci_reg, 0, sad_shift);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_dci_get_hist_bin32(hi_u32 *hist_bin32_array)
{
    hi_u8 *vdp_stt_base_addr = HI_NULL;
    hi_u32 *read_ptr = HI_NULL;
    hi_u32 index;
    PQ_CHECK_NULL_PTR_RE_FAIL(hist_bin32_array);

    vdp_stt_base_addr = pq_hal_get_vdp_stt_reg_vir_addr();
    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_stt_base_addr);

    read_ptr = (hi_u32 *)(vdp_stt_base_addr + 0x300); /* 0x300:first addr of 32bin hist. */
    for (index = 0; index < DCI_HIST_BIN32_NUM; index++) {
        hist_bin32_array[index] = *read_ptr;
        read_ptr++;
    }

    return HI_SUCCESS;
}

static hi_s32 pq_hal_dci_get_normalize_hist_bin32(hi_u32 *hist_bin32_array, hi_u32 win_size)
{
    hi_u8 *vdp_stt_base_addr = HI_NULL;
    hi_u32 *read_ptr = HI_NULL;
    hi_u32 index;
    PQ_CHECK_NULL_PTR_RE_FAIL(hist_bin32_array);
    PQ_CHECK_ZERO_RE_FAIL(win_size);

    vdp_stt_base_addr = pq_hal_get_vdp_stt_reg_vir_addr();
    PQ_CHECK_NULL_PTR_RE_FAIL(vdp_stt_base_addr);

    read_ptr = (hi_u32 *)(vdp_stt_base_addr + 0x300); /* 0x300:first addr of 32bin hist. */
    for (index = 0; index < DCI_HIST_BIN32_NUM; index++) {
        hist_bin32_array[index] = ((*read_ptr) * 256) / win_size +  /* 256:normalize num */
                                  ((((*read_ptr) * 256) % win_size) >= (win_size / 2) ? 1 : 0); /* 256,2:normalize. */
        read_ptr++;
    }

    return HI_SUCCESS;
}

static hi_u32 pq_hal_dci_soft_alg_sad_average(hi_void)
{
    hi_u32 total_value;
    total_value = g_dci_soft_alg_hist_sad[1] + g_dci_soft_alg_hist_sad[2] + /* 1,2: index num */
                  g_dci_soft_alg_hist_sad[3] + g_dci_soft_alg_hist_sad[4] + /* 3,4: index num */
                  g_dci_soft_alg_hist_sad[5] + g_dci_soft_alg_hist_sad[6];  /* 5,6: index num */

    return (total_value / 6); /* 6:total sad num. */
}

static hi_s32 pq_hal_dci_soft_alg_scd_policy(hi_bool *scd_flag)
{
    hi_s32 ret;
    hi_s32 index;
    hi_u32 sad_thrsh;
    hi_u32 sad_shift;

    PQ_CHECK_NULL_PTR_RE_FAIL(scd_flag);

    /* get thrsh and shift. */
    ret = pq_hal_dci_get_sad_para(&sad_thrsh, &sad_shift);
    PQ_CHECK_RETURN_SUCCESS(ret);

    /* get hist bin32. */
    ret = pq_hal_dci_get_hist_bin32(&g_hist_bin32_current[0]);
    PQ_CHECK_RETURN_SUCCESS(ret);

    for (index = 5; index >= 0; index--) { /* 5,0: para. */
        g_dci_soft_alg_hist_sad[index + 1] = g_dci_soft_alg_hist_sad[index];
    }

    g_dci_soft_alg_hist_sad[0] = 0;

    if (g_dci_soft_alg_frm_cnt > 0) {
        for (index = 0; index < DCI_HIST_BIN32_NUM; index++) {
            g_dci_soft_alg_hist_sad[0] += (pq_abs(g_hist_bin32_current[index] - g_hist_bin32_previous[index])
                                          >> sad_shift);
        }
    }

    *scd_flag = HI_FALSE;
    if (g_dci_soft_alg_frm_cnt >= DCI_SAD_MAX_CNT) {
        if ((g_dci_soft_alg_hist_sad[0] > sad_thrsh) &&
            (g_dci_soft_alg_hist_sad[0] > pq_hal_dci_soft_alg_sad_average())) {
            *scd_flag = HI_TRUE;
        }
    }

    /* record current hist bin */
    for (index = 0; index < DCI_HIST_BIN32_NUM; index++) {
        g_hist_bin32_previous[index] = g_hist_bin32_current[index];
    }

    /* count frm num */
    g_dci_soft_alg_frm_cnt++;
    g_dci_soft_alg_frm_cnt = (g_dci_soft_alg_frm_cnt > DCI_SAD_MAX_CNT) ? DCI_SAD_MAX_CNT : g_dci_soft_alg_frm_cnt;
    return HI_SUCCESS;
}

hi_s32 pq_hal_dci_get_histgram(hi_pq_dci_histgram *dci_hist, hi_u32 win_size)
{
    hi_s32 ret;
    PQ_CHECK_NULL_PTR_RE_FAIL(dci_hist);

    if (dci_hist->dci_histgram_32.histgram_site == DCI_HISTGRAM_SITE_VO) {
        if (dci_hist->dci_histgram_32.normalize == HI_FALSE) {
            /* ONLY support 32bin histgram currently. */
            ret = pq_hal_dci_get_hist_bin32(&dci_hist->dci_histgram_32.histgram[0]);
        } else {
            ret = pq_hal_dci_get_normalize_hist_bin32(&dci_hist->dci_histgram_32.histgram[0], win_size);
        }
    } else {
        HI_ERR_PQ("Not support!\n");
        ret = HI_FAILURE;
    }

    return ret;
}

static hi_s32 pq_hal_dci_get_min_black_thrsh(vdp_regs_type *reg, hi_u32 *min_black_thrsh)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(min_black_thrsh);

    pq_reg_dci_get_bs_min_black_thrsh(reg, 0, min_black_thrsh);
    return HI_SUCCESS;
}

static hi_s32 pq_hal_dci_s_cfg(vdp_regs_type *reg, pq_dci_coordinate *basic_coor)
{
    hi_u32 input_full_range  = 1;
    hi_u32 output_full_range = 1;
    hi_u32 bs_min_black_thrsh = 30;
    hi_u32 global_total_pix_num;
    hi_u32 global_total_pix_num_bld;

    PQ_CHECK_NULL_PTR_RE_FAIL(reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(basic_coor);

    if (pq_hal_dci_get_min_black_thrsh(reg, &bs_min_black_thrsh) != HI_SUCCESS) {
        bs_min_black_thrsh = 1; /* 1:default value. */
    }
    global_total_pix_num = (basic_coor->global_in_hor_end - basic_coor->global_in_hor_start + 1) *
                           (basic_coor->global_in_ver_end - basic_coor->global_in_ver_start + 1);
    global_total_pix_num_bld = (global_total_pix_num * bs_min_black_thrsh) >> 14; /* 14:shift right num */

    pq_reg_dci_set_dci_masic_en(reg, 0, HI_FALSE);
    pq_reg_dci_set_dci_ck_gt_en(reg, 0, HI_FALSE);
    pq_reg_dci_set_dci_dither_en(reg, 0, HI_FALSE);
    pq_reg_dci_set_input_full_range(reg, 0, input_full_range);
    pq_reg_dci_set_output_full_range(reg, 0, output_full_range);

    pq_reg_dci_set_global_in_hor_end(reg, 0, basic_coor->global_in_hor_end);
    pq_reg_dci_set_global_in_hor_start(reg, 0, basic_coor->global_in_hor_start);
    pq_reg_dci_set_global_in_ver_end(reg, 0, basic_coor->global_in_ver_end);
    pq_reg_dci_set_global_in_ver_start(reg, 0, basic_coor->global_in_ver_start);
    pq_reg_dci_set_global_out_hor_end(reg, 0, basic_coor->global_out_hor_end);
    pq_reg_dci_set_global_out_hor_start(reg, 0, basic_coor->global_out_hor_start);
    pq_reg_dci_set_global_out_ver_end(reg, 0, basic_coor->global_out_ver_end);
    pq_reg_dci_set_global_out_ver_start(reg, 0, basic_coor->global_out_ver_start);
    pq_reg_dci_set_local_hor_end(reg, 0, basic_coor->local_hor_end);
    pq_reg_dci_set_local_hor_start(reg, 0, basic_coor->local_hor_start);
    pq_reg_dci_set_local_ver_end(reg, 0, basic_coor->local_ver_end);
    pq_reg_dci_set_local_ver_start(reg, 0, basic_coor->local_ver_start);
    pq_reg_dci_set_cg_div_width(reg, 0, 0);
    pq_reg_dci_set_cg_div_height(reg, 0, 0);
    pq_reg_dci_set_ncount_rshf(reg, 0, 0);
    pq_reg_dci_set_glb_adj_lcl_gain(reg, 0, 16); /* 16: glb_adj_lcl_gain value */
    pq_reg_dci_set_global_total_pix_num(reg, 0,  global_total_pix_num);
    pq_reg_dci_set_global_total_pix_num_bld(reg, 0,  global_total_pix_num_bld);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_dci_s0_cfg(vdp_regs_type *reg, pq_dci_coordinate *s0_coor)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(s0_coor);

    pq_reg_dci_set_global_in_hor_end_s0(reg, 0, s0_coor->global_in_hor_end);
    pq_reg_dci_set_global_in_hor_start_s0(reg, 0, s0_coor->global_in_hor_start);
    pq_reg_dci_set_global_in_ver_end_s0(reg, 0, s0_coor->global_in_ver_end);
    pq_reg_dci_set_global_in_ver_start_s0(reg, 0, s0_coor->global_in_ver_start);
    pq_reg_dci_set_global_out_hor_end_s0(reg, 0, s0_coor->global_out_hor_end);
    pq_reg_dci_set_global_out_hor_start_s0(reg, 0, s0_coor->global_out_hor_start);
    pq_reg_dci_set_global_out_ver_end_s0(reg, 0, s0_coor->global_out_ver_end);
    pq_reg_dci_set_global_out_ver_start_s0(reg, 0, s0_coor->global_out_ver_start);
    pq_reg_dci_set_local_hor_end_s0(reg, 0, s0_coor->local_hor_end);
    pq_reg_dci_set_local_hor_start_s0(reg, 0, s0_coor->local_hor_start);
    pq_reg_dci_set_local_ver_end_s0(reg, 0, s0_coor->local_ver_end);
    pq_reg_dci_set_local_ver_start_s0(reg, 0, s0_coor->local_ver_start);

    return HI_SUCCESS;
}

static hi_s32 pq_hal_dci_s1_cfg(vdp_regs_type *reg, pq_dci_coordinate *s1_coor)
{
    PQ_CHECK_NULL_PTR_RE_FAIL(reg);
    PQ_CHECK_NULL_PTR_RE_FAIL(s1_coor);

    pq_reg_dci_set_global_in_hor_end_s1(reg, 0, s1_coor->global_in_hor_end);
    pq_reg_dci_set_global_in_hor_start_s1(reg, 0, s1_coor->global_in_hor_start);
    pq_reg_dci_set_global_in_ver_end_s1(reg, 0, s1_coor->global_in_ver_end);
    pq_reg_dci_set_global_in_ver_start_s1(reg, 0, s1_coor->global_in_ver_start);
    pq_reg_dci_set_global_out_hor_end_s1(reg, 0, s1_coor->global_out_hor_end);
    pq_reg_dci_set_global_out_hor_start_s1(reg, 0, s1_coor->global_out_hor_start);
    pq_reg_dci_set_global_out_ver_end_s1(reg, 0, s1_coor->global_out_ver_end);
    pq_reg_dci_set_global_out_ver_start_s1(reg, 0, s1_coor->global_out_ver_start);
    pq_reg_dci_set_local_hor_end_s1(reg, 0, s1_coor->local_hor_end);
    pq_reg_dci_set_local_hor_start_s1(reg, 0, s1_coor->local_hor_start);
    pq_reg_dci_set_local_ver_end_s1(reg, 0, s1_coor->local_ver_end);
    pq_reg_dci_set_local_ver_start_s1(reg, 0, s1_coor->local_ver_start);

    return HI_SUCCESS;
}

static hi_void pq_hal_dci_assign_basic_coor(hi_u32 width, hi_u32 height, pq_dci_coordinate *basic_coor)
{
    basic_coor->global_in_hor_end = width - 1;
    basic_coor->global_in_hor_start = 0;
    basic_coor->global_in_ver_end = height - 1;
    basic_coor->global_in_ver_start = 0;
    basic_coor->global_out_hor_end = width - 1;
    basic_coor->global_out_hor_start = 0;
    basic_coor->global_out_ver_end = height - 1;
    basic_coor->global_out_ver_start = 0;
    basic_coor->local_hor_end = width - 1;
    basic_coor->local_hor_start = 0;
    basic_coor->local_ver_end = height - 1;
    basic_coor->local_ver_start = 0;

    return;
}

static hi_void pq_hal_dci_assign_s0_coor(hi_bool sp_en, hi_u32 sp_w,
                                         pq_dci_coordinate *basic_coor, pq_dci_coordinate *s0_coor)
{
    hi_u32 gbal_i_h_end = basic_coor->global_in_hor_end;
    hi_u32 gbal_i_h_srt = basic_coor->global_in_hor_start;
    hi_u32 gbal_i_v_end = basic_coor->global_in_ver_end;
    hi_u32 gbal_i_v_srt = basic_coor->global_in_ver_start;
    hi_u32 gbal_o_h_end = basic_coor->global_out_hor_end;
    hi_u32 gbal_o_h_srt = basic_coor->global_out_hor_start;
    hi_u32 gbal_o_v_end = basic_coor->global_out_ver_end;
    hi_u32 gbal_o_v_srt = basic_coor->global_out_ver_start;
    hi_u32 lcal_h_end   = basic_coor->local_hor_end;
    hi_u32 lcal_h_start = basic_coor->local_hor_start;
    hi_u32 lcal_v_end   = basic_coor->local_ver_end;
    hi_u32 lcal_v_start = basic_coor->local_ver_start;
    hi_u32 overlap = 32;

    s0_coor->global_in_hor_end = sp_en ? (gbal_i_h_end >= sp_w - 1 ? sp_w - 1 : gbal_i_h_end) : gbal_i_h_end; /* 1:w */
    s0_coor->global_in_hor_start = sp_en ? (gbal_i_h_srt <= sp_w - 1 ? gbal_i_h_srt : sp_w) : gbal_i_h_srt; /* 1:w */
    s0_coor->global_in_ver_end = gbal_i_v_end;
    s0_coor->global_in_ver_start = gbal_i_v_srt;

    s0_coor->global_out_hor_end = sp_en ? (gbal_o_h_end >= sp_w - 1 ? sp_w - 1 : gbal_o_h_end) : gbal_o_h_end; /* 1:w */
    s0_coor->global_out_hor_start = sp_en ? (gbal_o_h_srt <= sp_w - 1 ? gbal_o_h_srt : sp_w) : gbal_o_h_srt; /* 1:w */
    s0_coor->global_out_ver_end = gbal_o_v_end;
    s0_coor->global_out_ver_start = gbal_o_v_srt;

    s0_coor->local_hor_end = sp_en ? (lcal_h_end >= sp_w - 1 ? sp_w + overlap - 1 : lcal_h_end) : lcal_h_end; /* 1:w */
    s0_coor->local_hor_start = sp_en ?
        (lcal_h_start <= sp_w - 1 ? lcal_h_start : sp_w + overlap) : lcal_h_start; /* 1:w */
    s0_coor->local_ver_end = lcal_v_end;
    s0_coor->local_ver_start = lcal_v_start;

    return;
}

static hi_void pq_hal_dci_assign_s1_coor(hi_bool sp_en, hi_u32 sp_w,
                                         pq_dci_coordinate *basic_coor, pq_dci_coordinate *s1_coor)
{
    hi_u32 gbal_i_h_end = basic_coor->global_in_hor_end;
    hi_u32 gbal_i_h_srt = basic_coor->global_in_hor_start;
    hi_u32 gbal_i_v_end = basic_coor->global_in_ver_end;
    hi_u32 gbal_i_v_srt = basic_coor->global_in_ver_start;
    hi_u32 gbal_o_h_end = basic_coor->global_out_hor_end;
    hi_u32 gbal_o_h_srt = basic_coor->global_out_hor_start;
    hi_u32 gbal_o_v_end = basic_coor->global_out_ver_end;
    hi_u32 gbal_o_v_srt = basic_coor->global_out_ver_start;
    hi_u32 lcal_h_end   = basic_coor->local_hor_end;
    hi_u32 lcal_h_start = basic_coor->local_hor_start;
    hi_u32 lcal_v_end   = basic_coor->local_ver_end;
    hi_u32 lcal_v_start = basic_coor->local_ver_start;
    hi_u32 overlap = 32;

    s1_coor->global_in_hor_end = sp_en ? (gbal_i_h_end <= sp_w - 1 ? 1 : gbal_i_h_end - sp_w) : gbal_i_h_end; /* 1:w */
    s1_coor->global_in_hor_start = sp_en ?
        (gbal_i_h_end <= sp_w - 1 ? 2 : gbal_i_h_srt >= sp_w ? gbal_i_h_srt - sp_w : 0) : gbal_i_h_srt ;  /* 1,2:w */
    s1_coor->global_in_ver_end = gbal_i_v_end;
    s1_coor->global_in_ver_start = gbal_i_v_srt;

    s1_coor->global_out_hor_end = sp_en ? (gbal_o_h_end <= sp_w - 1 ? 1 : gbal_o_h_end - sp_w) : gbal_o_h_end; /* 1:w */
    s1_coor->global_out_hor_start = sp_en ?
        (gbal_o_h_end <= sp_w - 1 ? 2 : gbal_o_h_srt >= sp_w ? gbal_o_h_srt - sp_w : 0) : gbal_o_h_srt; /* 1,2:w */
    s1_coor->global_out_ver_end = gbal_o_v_end;
    s1_coor->global_out_ver_start = gbal_o_v_srt;

    s1_coor->local_hor_end = sp_en ? (lcal_h_end <= sp_w - 1 ? 1 : lcal_h_end - sp_w + overlap) : lcal_h_end; /* 1:w */
    s1_coor->local_hor_start = (sp_en == HI_FALSE) ? lcal_h_start : \
        (lcal_h_end <= sp_w - 1 ? 2 : lcal_h_start >= sp_w ? lcal_h_start - sp_w + overlap : overlap); /* 1,2:w */
    s1_coor->local_ver_end = lcal_v_end;
    s1_coor->local_ver_start = lcal_v_start;

    return;
}

static hi_s32 pq_hal_dci_set_cfg(vdp_regs_type *reg, hi_u32 width, hi_u32 height)
{
    hi_s32 ret;
    hi_bool split_en;
    hi_u32  split_width0;
    pq_dci_coordinate basic_coor;
    pq_dci_coordinate s0_coor;
    pq_dci_coordinate s1_coor;

    PQ_CHECK_NULL_PTR_RE_FAIL(reg);

    /* split. */
    split_en = (width >= 256) ? HI_TRUE : HI_FALSE; /* 256: min width. */
    split_width0 = (width / 2) / 4 * 4; /* 2,4,4: calculate split width. */

    pq_hal_dci_assign_basic_coor(width, height, &basic_coor);
    pq_hal_dci_assign_s0_coor(split_en, split_width0, &basic_coor, &s0_coor);
    pq_hal_dci_assign_s1_coor(split_en, split_width0, &basic_coor, &s1_coor);

    /* S config */
    ret = pq_hal_dci_s_cfg(reg, &basic_coor);
    PQ_CHECK_RETURN_SUCCESS(ret);

    /* S0 config */
    ret = pq_hal_dci_s0_cfg(reg, &s0_coor);
    PQ_CHECK_RETURN_SUCCESS(ret);

    /* S1 config */
    ret = pq_hal_dci_s1_cfg(reg, &s1_coor);
    PQ_CHECK_RETURN_SUCCESS(ret);
    return HI_SUCCESS;
}

hi_s32 pq_hal_dci_update_cfg(pq_dci_cfg *dci_cfg)
{
    hi_s32 ret;
    hi_bool scd_flag;

    PQ_CHECK_NULL_PTR_RE_FAIL(dci_cfg);
    PQ_CHECK_NULL_PTR_RE_FAIL(g_vdp_dci_reg);

    if (dci_cfg->width == 0 || dci_cfg->height == 0) {
        HI_ERR_PQ("Invalid w->%d, or h->%d.\n", dci_cfg->width, dci_cfg->height);
        return HI_ERR_PQ_INVALID_PARA;
    }

    ret = pq_hal_dci_soft_alg_scd_policy(&scd_flag);
    PQ_CHECK_RETURN_SUCCESS(ret);

    pq_hal_dci_set_link_type(g_vdp_dci_reg, dci_cfg->link_type);
    pq_reg_dci_set_scd_flag(g_vdp_dci_reg, 0, scd_flag);

    ret = pq_hal_dci_set_cfg(g_vdp_dci_reg, dci_cfg->width, dci_cfg->height);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_dci_set_coef(g_vdp_dci_reg);
    PQ_CHECK_RETURN_SUCCESS(ret);

    ret = pq_hal_dci_set_para_addr(&g_dci_hal_coef_buf);
    PQ_CHECK_RETURN_SUCCESS(ret);

    return HI_SUCCESS;
}


