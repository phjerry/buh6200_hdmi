/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: pq acm coef
 * Author: pq
 * Create: 2019-10-26
 */

#ifndef __PQ_MNG_ACM_COEF_H__
#define __PQ_MNG_ACM_COEF_H__
#include "pq_hal_acm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define HUE_GAIN 255
#define SAT_GAIN 300
#define LUMA_GAIN 255
#define YHS_CHANNELS 3

hi_s16 g_yhs_by_h[YHS_CHANNELS][COLOR_H_NUM] = {
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        6, 11, 13, 15, 18, 22, 25, 26, 27, 27, 27, 24, 19, 16, 11, 7, 5, 3, 3, 2, 0,
        0, 0, 0, 0, -1, -2, -2, -2, -2, -2, -3, -4, -6, -6, -5, 0, 2, 5, 7, 7, 9, 10,
        11, 12, 11, 12, 15, 17, 19, 17, 14, 11, 9, 8, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6
    },
    {
        120, 114, 108, 106, 106, 112, 118, 126, 133, 135, 141, 141, 137, 128, 122, 124,
        124, 120, 116, 120, 120, 116, 114, 110, 108, 108, 110, 110, 120, 111, 143, 163,
        190, 218, 235, 245, 249, 249, 247, 236, 224, 210, 196, 181, 165, 152, 139, 127,
        114, 102, 90, 88, 90, 92, 100, 112, 124, 137, 145, 151, 151, 151, 141, 131, 120
    },
};

hi_s16 g_green_enhance_mode[YHS_CHANNELS][COLOR_H_NUM] = {
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 60, 120, 180, 250, 250, 250, 250, 250, 240, 210,
        180, 150, 120, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
};

hi_s16 g_blue_enhance_mode[YHS_CHANNELS][COLOR_H_NUM] = {
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        200, 200, 180, 150, 100, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 80, 100, 120, 140, 160, 180, 200,
        200, 200
    },
};
hi_s16 g_bg_enhance_mode[YHS_CHANNELS][COLOR_H_NUM] = {
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        200, 200, 180, 150, 100, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 80, 100, 120, 140, 160, 180, 200,
        200, 200
    },
};

hi_s16 g_flesh_enhance[YHS_CHANNELS][COLOR_H_NUM] = {
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 50, 70,
        90, 105, 90, 70, 45, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    },
};

hi_s16 g_yhs_by_hs_gain[YHS_CHANNELS][COLOR_H_NUM][COLOR_S_NUM] = {
    {
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
        { 0, 3, 6, 8, 11, 22, 33, 43, 53, 64, 75, 85, 95, 106, 117, 128 },
    },
    {
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128 },
    },
    {
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 7, 13, 20, 26, 43, 61, 72, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 5, 9,  14, 18, 36, 56, 70, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 3, 6,  8,  11, 28, 50, 68, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 1, 2,  2,  3,  19, 46, 64, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 1, 2,  3,  4,  19, 45, 64, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 1, 2,  2,  3,  20, 45, 64, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 3, 6,  8,  11, 27, 50, 67, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 5, 9,  14, 18, 35, 55, 70, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 7, 13, 20, 26, 43, 61, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
        { 0, 8, 16, 24, 32, 49, 64, 73, 81, 90, 96, 102, 109, 115, 122, 128 },
    },
};

hi_s16 g_yhs_by_hy_gain[YHS_CHANNELS][COLOR_H_NUM][COLOR_Y_NUM] = {
    {
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
    },
    {
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
        { 128, 128, 128, 128, 128, 128, 128, 128, 128,  },
    },
    {
        { 0, 50, 96, 81, 65, 50, 36, 19, 0,  },
        { 0, 50, 96, 81, 65, 53, 36, 19, 0,  },
        { 0, 50, 96, 81, 65, 53, 36, 19, 0,  },
        { 0, 50, 96, 81, 65, 53, 36, 19, 0,  },
        { 0, 37, 68, 96, 73, 53, 36, 19, 0,  },
        { 0, 35, 68, 96, 73, 53, 36, 19, 0,  },
        { 0, 35, 68, 96, 73, 53, 36, 19, 0,  },
        { 0, 35, 68, 96, 73, 53, 36, 19, 0,  },
        { 0, 35, 68, 96, 73, 53, 36, 19, 0,  },
        { 1, 28, 55, 78, 97, 73, 50, 26, 1,  },
        { 0, 29, 56, 78, 96, 73, 51, 26, 1,  },
        { 0, 32, 58, 80, 96, 66, 42, 19, 0,  },
        { 0, 35, 60, 80, 96, 67, 43, 22, 1,  },
        { 1, 33, 62, 82, 96, 68, 43, 21, 0,  },
        { 1, 32, 64, 97, 80, 63, 46, 24, 0,  },
        { 1, 33, 64, 97, 77, 59, 41, 22, 0,  },
        { 0, 34, 66, 97, 80, 64, 46, 23, 0,  },
        { 0, 34, 64, 96, 80, 63, 43, 21, 0,  },
        { 1, 33, 65, 97, 78, 60, 42, 22, 1,  },
        { 0, 33, 64, 96, 78, 60, 42, 21, 1,  },
        { 1, 32, 60, 96, 83, 66, 49, 27, 0,  },
        { 1, 33, 65, 96, 87, 73, 56, 29, 1,  },
        { 1, 31, 55, 78, 96, 69, 46, 21, 0,  },
        { 0, 30, 56, 78, 96, 68, 46, 22, 0,  },
        { 0, 31, 57, 77, 96, 75, 53, 28, 0,  },
        { 0, 24, 48, 66, 82, 96, 63, 31, 1,  },
        { 1, 26, 49, 66, 83, 97, 65, 35, 0,  },
        { 0, 24, 46, 64, 80, 96, 69, 36, 1,  },
        { 0, 19, 38, 57, 76, 97, 71, 40, 1,  },
        { 0, 18, 38, 57, 75, 97, 73, 43, 0,  },
        { 0, 20, 36, 52, 68, 81, 96, 50, 1,  },
        { 0, 21, 38, 54, 68, 82, 97, 50, 0,  },
        { 1, 18, 34, 50, 63, 78, 97, 47, 0,  },
        { 0, 16, 32, 48, 62, 78, 96, 50, 1,  },
        { 1, 18, 35, 51, 65, 82, 97, 50, 0,  },
        { 0, 21, 37, 53, 67, 80, 97, 49, 1,  },
        { 0, 19, 35, 51, 65, 80, 96, 48, 0,  },
        { 0, 17, 32, 48, 65, 79, 96, 50, 1,  },
        { 1, 17, 32, 48, 64, 80, 96, 49, 1,  },
        { 0, 16, 33, 49, 63, 78, 96, 48, 0,  },
        { 0, 17, 32, 49, 65, 79, 97, 50, 1,  },
        { 0, 18, 36, 53, 67, 82, 96, 46, 1,  },
        { 0, 18, 35, 50, 64, 79, 96, 48, 0,  },
        { 0, 18, 35, 50, 64, 79, 96, 48, 0,  },
        { 0, 18, 35, 50, 64, 79, 96, 48, 0,  },
        { 0, 18, 35, 50, 64, 79, 96, 48, 0,  },
        { 0, 18, 35, 50, 64, 79, 96, 48, 0,  },
        { 0, 18, 35, 50, 64, 79, 96, 48, 0,  },
        { 0, 18, 35, 50, 64, 79, 96, 48, 0,  },
        { 0, 18, 35, 50, 64, 79, 96, 48, 0,  },
        { 0, 16, 34, 50, 66, 81, 96, 48, 0,  },
        { 0, 19, 38, 56, 70, 85, 96, 43, 0,  },
        { 1, 16, 31, 49, 67, 97, 75, 43, 1,  },
        { 0, 21, 43, 61, 79, 97, 67, 36, 1,  },
        { 0, 28, 50, 69, 86, 96, 63, 31, 1,  },
        { 1, 26, 49, 72, 96, 75, 50, 25, 1,  },
        { 1, 28, 53, 78, 97, 71, 48, 26, 0,  },
        { 0, 29, 56, 78, 97, 68, 43, 21, 0,  },
        { 1, 32, 64, 97, 79, 63, 42, 21, 0,  },
        { 0, 33, 63, 97, 78, 63, 42, 21, 1,  },
        { 1, 36, 70, 96, 77, 58, 38, 20, 0,  },
        { 0, 50, 96, 83, 71, 58, 41, 21, 0,  },
        { 1, 48, 96, 85, 72, 58, 42, 22, 1,  },
        { 0, 50, 96, 81, 65, 53, 36, 19, 0,  },
        { 0, 50, 96, 81, 65, 50, 36, 19, 0,  },
    },
};

static hue_range g_color_range[COLOR_SEGMENTATION_MAX] = {
    {13, 20},   /* R */
    {35, 46},   /* G */
    {0, 4},     /* B1 */
    {58, 64},   /* B2 */
    {48, 54},   /* CYAN */
    {5, 12},    /* MAGENTA */
    {29, 32},   /* YELLOW */
    {22, 26},   /* FLESH */
};
/********************************************************************
����߽��ķ�Χ:��������ɫ�����ĵ��������ģ���߽�����ʼ������Ϊ0��
�߽�㴦�������жϵ��������ֱ�ӷ���HI_SUCCESS
********************************************************************/
static hue_range g_color_cross_range[COLOR_SEGMENTATION_MAX] = {
    {0, 0},     /* R */
    {33, 34},   /* G */
    {55, 57},   /* B1 */
    {55, 57},   /* B2 */
    {47, 47},   /* CYAN */
    {0, 0},     /* MAGENTA */
    {27, 28},   /* YELLOW */
    {21, 21},   /* FLESH */
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
