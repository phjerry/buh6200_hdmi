/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: panel module
* Author: sdk
* Create: 2019-12-15
*/

#ifndef __DRV_PANEL_DATA_H__
#define __DRV_PANEL_DATA_H__

#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
} drv_panel_data_base;

typedef struct {
} drv_panel_data_advance;

typedef struct {
} drv_panel_data_combo;

typedef struct {
} drv_panel_data_gpio;

typedef struct {
} drv_panel_data_i2c_tcon;

typedef struct {
} drv_panel_data_localdimming;

typedef struct {
} drv_panel_data_misc;

typedef struct {
} drv_panel_data_pwm;

typedef struct {
} drv_panel_data_i2c_tconless;

typedef struct {
} drv_panel_data_tcontiming;

typedef struct {
} drv_panel_data_tconmapping;

typedef struct {
} drv_panel_data_mipi;

typedef struct {
} drv_panel_data_mlvds;

typedef struct {
} drv_panel_data_epi;

typedef struct {
} drv_panel_data_ceds;

typedef struct {
} drv_panel_data_cspi;

typedef struct {
} drv_panel_data_usit;

typedef struct {
    drv_panel_data_base         *data_base;
    drv_panel_data_advance      *data_advance;
    drv_panel_data_combo        *data_combo;
    drv_panel_data_gpio         *data_gpio;
    drv_panel_data_i2c_tcon     *data_i2c_tcon;
    drv_panel_data_localdimming *data_localdimming;
    drv_panel_data_misc         *data_misc;
    drv_panel_data_pwm          *data_pwm;
    drv_panel_data_i2c_tconless *data_i2c_tconless;
    drv_panel_data_tcontiming   *data_tcontiming;
    drv_panel_data_tconmapping  *data_tconmapping;
    drv_panel_data_mipi         *data_mipi;
    drv_panel_data_mlvds        *data_mlvds;
    drv_panel_data_epi          *data_epi;
    drv_panel_data_ceds         *data_ceds;
    drv_panel_data_cspi         *data_cspi;
    drv_panel_data_usit         *data_usit;
} drv_panel_file_data;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
