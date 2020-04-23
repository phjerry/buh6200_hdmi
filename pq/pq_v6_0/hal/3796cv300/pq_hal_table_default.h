/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: pq table default param
 * Author: pq
 * Create: 2016-01-1
 */

#ifndef __PQ_HAL_PQ_TABLE_DEFAULT_H__
#define __PQ_HAL_PQ_TABLE_DEFAULT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* soft table pq bin define */
/* fmd */
#define PQ_SOFT_IP_DETECT    0xffe00001
#define PQ_SOFT_FRMRATETHR_L 0xffe00005
#define PQ_SOFT_FRMRATETHR_H 0xffe00009

/* DB init */
#define PQ_SOFT_DB_NUM_WIN        0xffe00105
#define PQ_SOFT_DB_HYMINBLKNUM    0xffe00109
#define PQ_SOFT_DB_HCMINBLKNUM    0xffe0010d
#define PQ_SOFT_DB_VYMINBLKNUM    0xffe00111
#define PQ_SOFT_DB_KEYFRM_NUM     0xffe00115
#define PQ_SOFT_DB_HYBORDCOUNTHRD 0xffe00119
#define PQ_SOFT_DB_HCBORDCOUNTHRD 0xffe0011d
#define PQ_SOFT_DB_VYBORDCOUNTHRD 0xffe00121
#define PQ_SOFT_DB_FRMWINTMP      0xffe00125
#define PQ_SOFT_DB_FRMSATWINTMP   0xffe00129

/* DB filter */
#define PQ_SOFT_DB_HL_DELTA_STRONG_2  0xffe00141
#define PQ_SOFT_DB_HL_DELTA_STRONG_1  0xffe00142
#define PQ_SOFT_DB_HL_DELTA_ULT_STR_2 0xffe00143
#define PQ_SOFT_DB_HL_DELTA_ULT_STR_1 0xffe00144
#define PQ_SOFT_DB_HL_DELTA_MID_2     0xffe00145
#define PQ_SOFT_DB_HL_DELTA_MID_1     0xffe00146
#define PQ_SOFT_DB_LV_DELTA_2         0xffe00147
#define PQ_SOFT_DB_LV_DELTA_1         0xffe00148
#define PQ_SOFT_DB_HC_DELTA_2         0xffe00149
#define PQ_SOFT_DB_HC_DELTA_1         0xffe0014a
#define PQ_SOFT_DB_STR_FADE_1_2       0xffe00151
#define PQ_SOFT_DB_STR_FADE_1_1       0xffe00152
#define PQ_SOFT_DB_STR_FADE_1_4_2     0xffe00153
#define PQ_SOFT_DB_STR_FADE_1_4_1     0xffe00154
#define PQ_SOFT_DB_STR_FADE_2_2       0xffe00155
#define PQ_SOFT_DB_STR_FADE_2_1       0xffe00156
#define PQ_SOFT_DB_STR_FADE_3_2       0xffe00157
#define PQ_SOFT_DB_STR_FADE_3_1       0xffe00158
#define PQ_SOFT_DB_STR_V_FADE_2       0xffe00159
#define PQ_SOFT_DB_STR_V_FADE_1       0xffe0015a
#define PQ_SOFT_DB_DIR_STR            0xffe00161
#define PQ_SOFT_DB_DIR_GAIN_2         0xffe00163
#define PQ_SOFT_DB_DIR_GAIN_1         0xffe00164

#define PQ_SOFT_DB_DYN_PARA_STR_1 0xffe00171
#define PQ_SOFT_DB_DYN_PARA_STR_2 0xffe00172
#define PQ_SOFT_DB_DYN_PARA_STR_3 0xffe00173
#define PQ_SOFT_DB_DYN_PARA_STR_4 0xffe00174
#define PQ_SOFT_DB_DYN_PARA_STR_5 0xffe00175
#define PQ_SOFT_DB_DYN_PARA_STR_6 0xffe00176
#define PQ_SOFT_DB_DYN_PARA_STR_7 0xffe00177
#define PQ_SOFT_DB_DYN_PARA_STR_8 0xffe00178
#define PQ_SOFT_DB_DYN_PARA_STR_9 0xffe00179

#define PQ_SOFT_DB_DYN_PARA_MID_1 0xffe00181
#define PQ_SOFT_DB_DYN_PARA_MID_2 0xffe00182
#define PQ_SOFT_DB_DYN_PARA_MID_3 0xffe00183
#define PQ_SOFT_DB_DYN_PARA_MID_4 0xffe00184
#define PQ_SOFT_DB_DYN_PARA_MID_5 0xffe00185
#define PQ_SOFT_DB_DYN_PARA_MID_6 0xffe00186
#define PQ_SOFT_DB_DYN_PARA_MID_7 0xffe00187
#define PQ_SOFT_DB_DYN_PARA_MID_8 0xffe00188
#define PQ_SOFT_DB_DYN_PARA_MID_9 0xffe00189

#define PQ_SOFT_DB_DYN_PARA_WEK_1 0xffe00191
#define PQ_SOFT_DB_DYN_PARA_WEK_2 0xffe00192
#define PQ_SOFT_DB_DYN_PARA_WEK_3 0xffe00193
#define PQ_SOFT_DB_DYN_PARA_WEK_4 0xffe00194
#define PQ_SOFT_DB_DYN_PARA_WEK_5 0xffe00195
#define PQ_SOFT_DB_DYN_PARA_WEK_6 0xffe00196
#define PQ_SOFT_DB_DYN_PARA_WEK_7 0xffe00197
#define PQ_SOFT_DB_DYN_PARA_WEK_8 0xffe00198
#define PQ_SOFT_DB_DYN_PARA_WEK_9 0xffe00199

/* color temp */
#define PQ_SOFT_COLOR_TEMP_R 0xffe00201
#define PQ_SOFT_COLOR_TEMP_G 0xffe00202
#define PQ_SOFT_COLOR_TEMP_B 0xffe00203

/* MODULE on/off */
#define PQ_SOFT_MODULE_SR            0xffe10001
#define PQ_SOFT_MODULE_DCI           0xffe10002
#define PQ_SOFT_MODULE_SHARPN        0xffe10003
#define PQ_SOFT_MODULE_CLE           0xffe10004
#define PQ_SOFT_MODULE_DC            0xffe10005
#define PQ_SOFT_MODULE_DR            0xffe10006
#define PQ_SOFT_MODULE_DS            0xffe10007
#define PQ_SOFT_MODULE_ACM          0xffe10008
#define PQ_SOFT_MODULE_FMD          0xffe10009
#define PQ_SOFT_MODULE_FOD          0xffe10010
#define PQ_SOFT_MODULE_TNR          0xffe10011
#define PQ_SOFT_MODULE_SNR          0xffe10012
#define PQ_SOFT_MODULE_DB            0xffe10013
#define PQ_SOFT_MODULE_DM            0xffe10014

pq_bin_phy_reg *pq_table_get_phy_reg_default(hi_void);
pq_bin_phy_reg *pq_table_get_soft_reg_default(hi_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

