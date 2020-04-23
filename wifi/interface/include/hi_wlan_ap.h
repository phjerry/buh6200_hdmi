/******************************************************************************
  Copyright (C), 2004-2014, Hisilicon Tech. Co., Ltd.
******************************************************************************
  File Name     : hi_wlan_ap.h
  Version       : Initial Draft
  Author        : Hisilicon sdk software group
  Created       :
  Last Modified :
  Description   : header file for Wi-Fi SoftAP component
  Function List :
  History       :
  1.Date        :
  Author        :
  Modification  : Created file
******************************************************************************/

/**
 * \file
 * \brief describle the APIs and structs of WiFi SoftAP function. CNcomment:提供WiFi SoftAP功能组件相关接口、数据结构信息。CNend
 */

#ifndef __HI_WLAN_AP_H__
#define __HI_WLAN_AP_H__

#include "hi_wlan_common.h"
#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif

/*************************** Structure Definition ****************************/
/** \addtogroup     WLAN_AP */
/** @{ */  /** <!-- [WLAN_AP] */

/** AP's configuration */
typedef struct
{
    hi_char   ssid[MAX_SSID_LEN+1];      /**< network's SSID *//** CNcomment:SSID */
    hi_s32    channel;                   /**< network's channel *//** CNcomment:信道号 */
    hi_wlan_security security;           /**< network's security mode *//** CNcomment:安全模式 */
    hi_char   password[MAX_PWD_LEN+1];   /**< network's password, if not OPEN mode *//** CNcomment:密码 */
    hi_bool   hidden_ssid;               /**< whether network hiddens it's SSID *//** CNcomment:是否隐藏SSID */
} hi_wlan_ap_config;


/** @}*/  /** <!-- ==== Structure Definition End ====*/

/******************************* API Declaration *****************************/
/** \addtogroup     WLAN_AP*/
/** @{*/  /** <!-- [WLAN_AP]*/

/**
\brief: Initialize SoftAP.CNcomment:初始化SoftAP CNend
\attention \n
\param    N/A.CNcomment:无 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\see \n
::hi_wlan_ap_init
*/
hi_s32 hi_wlan_ap_init(hi_void);

/**
\brief: Deintialize SoftAP.CNcomment:去初始化SoftAP CNend
\attention \n
\param  N/A.CNcomment:无 CNend
\retval N/A.CNcomment:无 CNend
\see \n
::hi_wlan_ap_deinit
*/
hi_void hi_wlan_ap_deinit(hi_void);

/**
\brief: Open WiFi SoftAP device.CNcomment:打开WiFi SoftAP设备 CNend
\attention \n
\param[out] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] name_size  parameter ifname length.CNcomment:ifanme的大小, 如: strlen(ifname)
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_DEVICE_NOT_FOUND
\retval  ::HI_WLAN_LOAD_DRIVER_FAIL
\see \n
::hi_wlan_ap_open
*/
hi_s32 hi_wlan_ap_open(hi_char *ifname, hi_u32 name_size);

/**
\brief: Close WiFi SoftAP device.CNcomment:关闭WiFi SoftAP设备 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_ap_close
*/
hi_s32 hi_wlan_ap_close(const hi_char *ifname);

/**
\brief: start SoftAP with configuration.CNcomment:开启SoftAP CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] ap_cfg  AP's configuration.CNcomment:AP的配置参数 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_START_HOSTAPD_FAIL
\see \n
::hi_wlan_ap_start
*/
hi_s32 hi_wlan_ap_start(const hi_char *ifname, hi_wlan_ap_config *ap_cfg);

/**
\brief: Stop SoftAP.CNcomment:关闭SoftAP CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_ap_stop
*/
hi_s32 hi_wlan_ap_stop(const hi_char *ifname);

/**
\brief: Set SoftAP.CNcomment:设置SoftAP CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] ap_cfg  AP's configuration.CNcomment:AP的配置参数 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_START_HOSTAPD_FAIL
\see \n
::hi_wlan_ap_set_softap
*/
hi_s32 hi_wlan_ap_set_softap(const hi_char *ifname, hi_wlan_ap_config *ap_cfg);

/**
\brief: Get local WiFi MAC address.CNcomment:获取本地WiFi MAC地址 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[out] mac  MAC address of local WiFi.CNcomment:保存本地WiFi MAC地址 CNend
\param[in] mac_size  parameter ifname length.CNcomment:ifname的大小, 大小一般固定为17 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_ap_get_mac
*/
hi_s32 hi_wlan_ap_get_mac(const hi_char *ifname, hi_char *mac, hi_u8 mac_size);

/** @}*/  /** <!-- ==== API Declaration End ====*/

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif
#endif /*__HI_WLAN_AP_H__*/
