/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: function define
 * Author: image
 * Create: 2019-04-12
 */

#ifndef __HAL_VDP_IP_ZME_H__
#define __HAL_VDP_IP_ZME_H__

#include "vdp_chip_define.h"

#define VDP_8K_ZME_SPLIT_MAX 4
#define VDP_8K_SR_SPLIT_MAX  2

typedef enum {
    VDP_ZME_4K_ZME = 0,
    VDP_ZME_8K_ZME,
    VDP_ZME_4K_SR,
    VDP_ZME_8K_SR,
    VDP_ZME_NODE_BUTT,
} vdp_zme_node;

typedef struct {
    hi_u32 in_w[VDP_8K_ZME_SPLIT_MAX];
    hi_u32 out_w[VDP_8K_ZME_SPLIT_MAX];
    hi_s32 h_loffset[VDP_8K_ZME_SPLIT_MAX];
    hi_s32 h_coffset[VDP_8K_ZME_SPLIT_MAX];
} vdp_8k_zme_split_out;

typedef struct {
    hi_u32 demo_en[VDP_8K_SR_SPLIT_MAX];
    hi_u32 demo_mode[VDP_8K_SR_SPLIT_MAX];
    hi_u32 demo_pos[VDP_8K_SR_SPLIT_MAX];
    hi_u32 dbsr_demo_en[VDP_8K_SR_SPLIT_MAX];
    hi_u32 dbsr_demo_mode[VDP_8K_SR_SPLIT_MAX];
    hi_u32 dbsr_demo_pos[VDP_8K_SR_SPLIT_MAX];
    hi_u32 c0_graph_split[VDP_8K_SR_SPLIT_MAX];
    hi_u32 c1_graph_split[VDP_8K_SR_SPLIT_MAX];
} vdp_8k_sr_split_out;

typedef struct {
    hi_s32 h_loffset;
    hi_s32 h_coffset;

    hi_s32 v_loffset;
    hi_s32 v_coffset;
    hi_s32 v_bloffset;
    hi_s32 v_bcoffset;
} vdp_zme_offset;

typedef struct {
    hi_u32 link;

    hi_u32 in_width;
    hi_u32 in_height;

    hi_u32 out_width;
    hi_u32 out_height;

    hi_u32 in_pix_fmt;  /* video format for zme input : 0-420; 1-422; 2-444 */
    hi_u32 out_pix_fmt; /* video format for zme output: 0-420; 1-422; 2-444 */
    hi_u32 v_out_pix_fmt;

    hi_bool v_enable;
    hi_bool h_enable;

    hi_bool h_lfir_en;
    hi_bool h_cfir_en;
    hi_bool v_lfir_en;
    hi_bool v_cfir_en;

    hi_bool h_med_en;
    hi_bool v_med_en;

    hi_bool in_mode;  /* Frame format for zme Intput: 0-field; 1-frame */
    hi_bool out_mode; /* Frame format for zme Output: 0-field; 1-frame */

    vdp_zme_offset offset;
} vdp_zme_image_info;

typedef struct {
    vdp_zme_image_info image_info[VDP_ZME_NODE_BUTT];
    hi_bool cle_8k_enable;
    hi_bool cle_4k_enable;
} vdp_zme_info;

hi_void vdp_ip_zme_set(hi_u32 layer, vdp_zme_info *zme_info);

#endif

