/******************************************************************************
  Copyright (C), 2004-2014, Hisilicon Tech. Co., Ltd.
******************************************************************************
  File Name     : hi_wlan_common.h
  Version       : Initial Draft
  Author        : Hisilicon sdk software group
  Created       :
  Last Modified :
  Description   : header file for Wi-Fi common component
  Function List :
  History       :
  1.Date        :
  Author        :
  Modification  : Created file
******************************************************************************/

/**
 * \file
 * \brief describle the APIs and structs of WiFi common function. CNcomment:提供WiFi共有功能组件相关接口、数据结构信息。CNend
 */

#ifndef __HI_WLAN_COMMON_H__
#define __HI_WLAN_COMMON_H__

#include "hi_type.h"
#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif

/*************************** Structure Definition ****************************/
/** \addtogroup     WLAN_COMMON */
/** @{ */  /** <!-- [WLAN_COMMON] */

#define MAX_SSID_LEN    32    /** max len of AP's ssid *//** CNcomment:SSID最大长度 */
#define BSSID_LEN       17    /** len of MAC address *//** CNcomment:MAC地址长度 */
#define MAX_PWD_LEN     64    /** max len of password *//** CNcomment:密码最大长度 */

/** invalid parameter */
/** CNcomment:无效的参数 -2 */
#define HI_WLAN_INVALID_PARAMETER           (-2)

/** no supported WiFi device found */
/** CNcomment:没有找到WiFi设备 -3 */
#define HI_WLAN_DEVICE_NOT_FOUND            (-3)

/** load driver fail */
/** CNcomment:加载驱动失败 -4 */
#define HI_WLAN_LOAD_DRIVER_FAIL            (-4)

/** run wpa_supplicant process fail */
/** CNcomment:启动wpa_supplicant失败 -5 */
#define HI_WLAN_START_SUPPLICANT_FAIL       (-5)

/** cann't connect to wpa_supplicant */
/** CNcomment:连接wpa_supplicant失败 -6 */
#define HI_WLAN_CONNECT_TO_SUPPLICANT_FAIL  (-6)

/** cann't send command to wpa_supplicant */
/** CNcomment:发送命令给wpa_supplicant失败 -7 */
#define HI_WLAN_SEND_COMMAND_FAIL           (-7)

/** run hostapd process fail */
/** CNcomment:启动hostapd失败 -8 */
#define HI_WLAN_START_HOSTAPD_FAIL          (-8)

#define PIN_CODE_LEN      8    /** length of wps pin code *//** CNcomment:pin码长度 */

/** network security mode type *//** CNcomment:AP安全模式类型 */
typedef enum
{
    HI_WLAN_SECURITY_OPEN,          /**< OPEN mode, not encrypted *//** CNcomment:OPEN模式 */
    HI_WLAN_SECURITY_WEP,           /**< WEP mode *//** CNcomment:WEP模式 */
    HI_WLAN_SECURITY_WPA_WPA2_PSK,  /**< WPA-PSK/WPA2-PSK mode *//** CNcomment:WPA-PSK或WPA2-PSK模式 */
    HI_WLAN_SECURITY_WPA_WPA2_EAP,  /**< WPA-EAP/WPA2-EAP mode *//** CNcomment:WPA-EAP或WPA2-EAP模式 */
    HI_WLAN_SECURITY_WAPI_PSK,      /**< WAPI-PSK mode *//** CNcomment:WAPI-PSK模式 */
    HI_WLAN_SECURITY_WAPI_CERT,     /**< WAPI-CERT mode *//** CNcomment:WAPI-CERT模式 */
    HI_WLAN_SECURITY_MAX,
} hi_wlan_security;

/** WiFi event type *//** CNcomment:WiFi事件类型 */
typedef enum
{
    HI_WLAN_STA_EVENT_DISCONNECTED,    /**< network disconnected event *//** CNcomment:网络断开事件 */
    HI_WLAN_STA_EVENT_SCAN_RESULTS_AVAILABLE,    /**< scan done event *//** CNcomment:扫描结束事件 */
    HI_WLAN_STA_EVENT_CONNECTING,      /**< try to connect to network event *//** CNcomment:正在连接AP事件 */
    HI_WLAN_STA_EVENT_CONNECTED,       /**< network connected event *//** CNcomment:连接上AP事件 */
    HI_WLAN_STA_EVENT_SUPP_STOPPED,    /**< supplicant abnormal event *//** CNcomment:wpa_supplicant停止事件 */
    HI_WLAN_STA_EVENT_DRIVER_STOPPED,  /**< driver abnormal event *//** CNcomment:驱动退出事件 */
    HI_WLAN_STA_EVENT_WPS_TIMEOUT,         /**< wps timeout event *//** CNcomment:p2p wps超时事件 */
    HI_WLAN_STA_EVENT_WPS_OVERLAP,         /**< wps overlap event *//** CNcomment:p2p wps设备连接事件 */
    HI_WLAN_P2P_EVENT_PEERS_CHANGED,   /**< p2p status event *//** CNcomment:状态变更事件 */
    HI_WLAN_P2P_EVENT_GROUP_STARTED,   /**< p2p group started event *//** CNcomment:建立群组事件 */
    HI_WLAN_P2P_EVENT_GROUP_REMOVED,   /**< p2p group removed event *//** CNcomment:删除群组事件 */
    HI_WLAN_P2P_EVENT_CONNECTED,       /**< p2p connected event *//** CNcomment:设备连接上事件 */
    HI_WLAN_P2P_EVENT_DISCONNECTED,    /**< p2p disconnected event *//** CNcomment:设备未连接事件 */
    HI_WLAN_P2P_EVENT_CONNECTION_REQUESTED,       /**< p2p connection requested event *//** CNcomment:请求连接事件 */
    HI_WLAN_P2P_EVENT_PERSISTENT_GROUPS_CHANGED,  /**< p2p groups changed event *//** CNcomment:群组变更事件 */
    HI_WLAN_P2P_EVENT_INVITATION,      /**< p2p invitation event *//** CNcomment:设备邀请事件 */
    HI_WLAN_P2P_EVENT_DEVICE_FOUND,    /**< p2p device found event *//** CNcomment:设备发现事件 */
    HI_WLAN_P2P_EVENT_NEGOTIATION_FAILURE,        /**< p2p negotiation failure event *//** CNcomment:p2p neg失败事件 */
    HI_WLAN_P2P_EVENT_FORMATION_FAILURE,          /**< p2p formation failure event *//** CNcomment:p2p formation失败事件 */
    HI_WLAN_EVENT_MAX
} hi_wlan_event;

/** WPS method type *//** CNcomment:WPS连接类型 */
typedef enum
{
    HI_WLAN_WPS_PBC,           /**< Push Button method *//** CNcomment:Push Button方式 */
    HI_WLAN_WPS_PIN_DISPLAY,   /**< PIN Display method *//** CNcomment:PIN Display方式 */
    HI_WLAN_WPS_PIN_KEYPAD,    /**< PIN Keypad method *//** CNcomment:PIN Keypad方式 */
    HI_WLAN_WPS_PIN_LABEL,     /**< PIN Label method *//** CNcomment:PIN Keypad方式 */
    HI_WLAN_WPS_MAX
} hi_wlan_wps_method;

typedef hi_void (*hi_wlan_sta_event_callback)(hi_wlan_event event, hi_void *event_data, hi_u32 data_size);

/** @}*/  /** <!-- ==== API Declaration End ====*/

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif
#endif /*__HI_WLAN_AP_H__*/
