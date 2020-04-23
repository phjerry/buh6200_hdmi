/******************************************************************************
  Copyright (C), 2004-2014, Hisilicon Tech. Co., Ltd.
******************************************************************************
  File Name     : hi_wlan_sta.h
  Version       : Initial Draft
  Author        : Hisilicon sdk software group
  Created       :
  Last Modified :
  Description   : header file for Wi-Fi Station component
  Function List :
  History       :
  1.Date        :
  Author        :
  Modification  : Created file
******************************************************************************/

/**
 * \file
 * \brief describle the information about WiFi STA component. CNcomment:提供WiFi STA组件相关接口、数据结构信息。CNend
 */

#ifndef __HI_WLAN_STA_H__
#define __HI_WLAN_STA_H__

#include "hi_wlan_common.h"
#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif

/*************************** Structure Definition ****************************/
/** \addtogroup     WLAN_STA */
/** @{ */  /** <!-- [WLAN_STA] */

/** connection state type *//** CNcomment:网络连接状态类型 */
typedef enum
{
    HI_WLAN_STA_STATUS_DISCONNECTED,  /**< not connected to any network *//** CNcomment:网络断开状态 */
    HI_WLAN_STA_STATUS_CONNECTING,    /**< connecting to a network *//** CNcomment:正在连接AP状态 */
    HI_WLAN_STA_STATUS_CONNECTED,     /**< connected to a network *//** CNcomment:连接上AP状态 */
    HI_WLAN_STA_STATUS_MAX,
} hi_wlan_sta_connection_state;

/** access point information *//** CNcomment:AP信息结构体 */
typedef struct
{
    hi_char ssid[MAX_SSID_LEN+1];      /**< AP's SSID *//** CNcomment:AP的SSID */
    hi_char bssid[BSSID_LEN+1];        /**< AP's MAC address *//** CNcomment:AP的MAC地址 */
    hi_u32  rssi_level;                /**< AP's signal level, 0 - 100 *//** CNcomment:AP的信号强度，0 - 100 */
    hi_u32  channel;                   /**< AP's channel number *//** CNcomment:AP的信道 */
    hi_wlan_security security;         /**< AP's security mode *//** CNcomment:AP的安全模式 */
} hi_wlan_sta_info;

/** access point configuration *//** CNcomment:需要连接的AP配置 */
typedef struct
{
    hi_char   ssid[MAX_SSID_LEN+1];      /**< AP's SSID *//** CNcomment:AP的SSID */
    hi_char   bssid[BSSID_LEN+1];        /**< AP's MAC address *//** CNcomment:AP的MAC地址 */
    hi_wlan_security security;           /**< AP's security mode *//** CNcomment:AP的安全模式 */
    hi_char   password[MAX_PWD_LEN+1];   /**< AP's password, if not OPEN mode *//** CNcomment:密码 */
    hi_bool   hidden_ssid;               /**< whether AP hiddens it's SSID *//** CNcomment:AP是否是隐藏SSID */
} hi_wlan_sta_config;

/** network status information *//** CNcomment:网络连接状态信息 */
typedef struct
{
    hi_wlan_sta_connection_state state; /**< connection state *//** CNcomment:网络的连接状态 */
    hi_wlan_sta_info ap;        /**< network information which connected or connecting *//** CNcomment:连接上或者正在连接的AP信息 */
} hi_wlan_sta_connection_status;

/** @}*/  /** <!-- ==== Structure Definition End ====*/

/******************************* API Declaration *****************************/
/** \addtogroup     WLAN_STA*/
/** @{*/  /** <!-- [WLAN_STA]*/

/**
\brief: Initialize STA.CNcomment:初始化STA CNend
\attention \n
\param    N/A.CNcomment:无 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\see \n
::hi_wlan_sta_init
*/
hi_s32 hi_wlan_sta_init(hi_void);

/**
\brief: Deintialize STA.CNcomment:去初始化STA CNend
\attention \n
\param  N/A.CNcomment:无 CNend
\retval N/A.CNcomment:无 CNend
\see \n
::hi_wlan_sta_deinit
*/
hi_void hi_wlan_sta_deinit(hi_void);

/**
\brief: Open WiFi STA device.CNcomment:打开WiFi STA设备 CNend
\attention \n
\param[out] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] name_size  parameter ifname length.CNcomment:ifanme的大小, 如: strlen(ifname)
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_DEVICE_NOT_FOUND
\retval  ::HI_WLAN_LOAD_DRIVER_FAIL
\see \n
::hi_wlan_sta_open
*/
hi_s32 hi_wlan_sta_open(hi_char *ifname, hi_u32 name_size);

/**
\brief: Close WiFi STA device.CNcomment:关闭WiFi STA设备 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_sta_close
*/
hi_s32 hi_wlan_sta_close(const hi_char *ifname);

/**
\brief: Start WiFi STA.CNcomment:启动WiFi STA CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] event_cb  call back function that receives events.CNcomment:接收事件的回调函数 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_START_SUPPLICANT_FAIL
\retval  ::HI_WLAN_CONNECT_TO_SUPPLICANT_FAIL
\see \n
::hi_wlan_sta_start
*/
hi_s32 hi_wlan_sta_start(const hi_char *ifname, hi_wlan_sta_event_callback event_cb);

/**
\brief: Stop WiFi STA.CNcomment:停用WiFi STA CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_sta_stop
*/
hi_s32 hi_wlan_sta_stop(const hi_char *ifname);

/**
\brief: Start to scan.CNcomment:开始扫描 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_SEND_COMMAND_FAIL
\see \n
::hi_wlan_sta_start_scan
*/
hi_s32 hi_wlan_sta_start_scan(const hi_char *ifname);

/**
\brief: Get scan results.CNcomment:获取扫描到的AP CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[out] ap_list AP list.CNcomment: 保存扫描到的AP列表 CNend
\param[inout] ap_num  number of APs.CNcomment: AP列表中AP的数量 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_SEND_COMMAND_FAIL
\see \n
::hi_wlan_sta_get_scan_results
*/
hi_s32 hi_wlan_sta_get_scan_results(const hi_char *ifname,hi_wlan_sta_info *ap_list, hi_u32 *ap_num);

/**
\brief: Connect to AP.CNcomment:开始连接AP CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] sta_cfg  AP configuration try to connect.CNcomment:需要连接的AP的信息 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_SEND_COMMAND_FAIL
\see \n
::hi_wlan_sta_connect
*/
hi_s32 hi_wlan_sta_connect(const hi_char *ifname, hi_wlan_sta_config *sta_cfg);

/**
\brief: Disconnect to AP.CNcomment:断开连接 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_SEND_COMMAND_FAIL
\see \n
::hi_wlan_sta_disconnect
*/
hi_s32 hi_wlan_sta_disconnect(const hi_char *ifname);

/**
\brief: Get current network connection status.CNcomment:获得当前的连接状态信息 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[out] conn_status network connection status.CNcomment:保存连接状态信息 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_SEND_COMMAND_FAIL
\see \n
::hi_wlan_sta_get_connection_status
*/
hi_s32 hi_wlan_sta_get_connection_status(const hi_char *ifname, hi_wlan_sta_connection_status *conn_status);

/**
\brief: WPS connect to AP.CNcomment:开始连接WPS AP CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] wps_method  WPS method.CNcomment:WPS方法 CNend
\param[in] pin  Pin code if WPS method is PIN.CNcomment:WPS pin码 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_SEND_COMMAND_FAIL
\see \n
::hi_wlan_sta_start_wps
*/
hi_s32 hi_wlan_sta_start_wps(const hi_char *ifname, hi_wlan_wps_method wps_method, hi_char *pin);

/**
\brief: Get local WiFi MAC address.CNcomment:获取本地WiFi MAC地址 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[out] pstMac  MAC address of local WiFi.CNcomment:保存本地WiFi MAC地址 CNend
\param[in] macBufSize  parameter ifname length.CNcomment:ifname的大小, 大小一般固定为17 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::HI_WLAN_STA_GetMacAddress
*/
hi_s32 hi_wlan_sta_get_mac(const hi_char *ifname, hi_char *mac, hi_u8 mac_size);

/** @}*/  /** <!-- ==== API Declaration End ====*/

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif
#endif /* __HI_WLAN_STA_H__ */
