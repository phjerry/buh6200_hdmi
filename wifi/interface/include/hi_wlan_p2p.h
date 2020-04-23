/******************************************************************************
  Copyright (C), 2004-2014, Hisilicon Tech. Co., Ltd.
******************************************************************************
  File Name     : hi_wlan_p2p.h
  Version       : Initial Draft
  Author        : Hisilicon sdk software group
  Created       :
  Last Modified :
  Description   : header file for Wi-Fi P2P component
  Function List :
  History       :
  1.Date        :
  Author        :
  Modification  : Created file
******************************************************************************/

/**
 * \file
 * \brief describle the information about WiFi-Direct component. CNcomment:提供WiFi-Direct组件相关接口、数据结构信息。CNend
 */

#ifndef __HI_WLAN_P2P_H__
#define __HI_WLAN_P2P_H__

#include "hi_wlan_common.h"
#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif

/*************************** Structure Definition ****************************/
/** \addtogroup     WLAN_P2P */
/** @{ */  /** <!-- [WLAN_P2P] */

#define NETWORK_NAME_LEN  256  /** length of P2P device or Group name *//** CNcomment:P2P设备名称或者组名长度 */
#define DEVICE_TYPE_LEN   256  /** length of device type string *//** CNcomment:设备类型长度 */

/** WPS config methods bitmap *//** CNcomment: */
#define WPS_CONFIG_DISPLAY         0x0008
#define WPS_CONFIG_PUSHBUTTON      0x0080
#define WPS_CONFIG_KEYPAD          0x0100

/** Device Capability bitmap *//** CNcomment: */
#define DEVICE_CAPAB_SERVICE_DISCOVERY         1
#define DEVICE_CAPAB_CLIENT_DISCOVERABILITY    1<<1
#define DEVICE_CAPAB_CONCURRENT_OPER           1<<2
#define DEVICE_CAPAB_INFRA_MANAGED             1<<3
#define DEVICE_CAPAB_DEVICE_LIMIT              1<<4
#define DEVICE_CAPAB_INVITATION_PROCEDURE      1<<5

/** Group Capability bitmap *//** CNcomment: */
#define GROUP_CAPAB_GROUP_OWNER                1
#define GROUP_CAPAB_PERSISTENT_GROUP           1<<1
#define GROUP_CAPAB_GROUP_LIMIT                1<<2
#define GROUP_CAPAB_INTRA_BSS_DIST             1<<3
#define GROUP_CAPAB_CROSS_CONN                 1<<4
#define GROUP_CAPAB_PERSISTENT_RECONN          1<<5
#define GROUP_CAPAB_GROUP_FORMATION            1<<6

/** WiFi-Display Device Type value *//** CNcomment: */
#define WFD_SOURCE                 0
#define PRIMARY_SINK               1
#define SECONDARY_SINK             2
#define SOURCE_OR_PRIMARY_SINK     3

/** WiFi-Display information *//** CNcomment:WiFi-Display信息结构体 */
typedef struct
{
    hi_bool wfd_enabled;   /**< is WFD enabled *//** CNcomment:是否开启WFD */
    hi_s32 device_info;    /**< WFD device type *//** CNcomment:WFD设备类型 */
    hi_s32 control_port;   /**< control port *//** CNcomment:控制端口号 */
    hi_s32 max_throughput; /**< the max throughput *//** CNcomment:最大吞吐量 */
} hi_wlan_p2p_wfd_info;

/** WiFi-Direct device information *//** CNcomment:WiFi-Direct设备信息结构体 */
typedef struct
{
    hi_char name[NETWORK_NAME_LEN];         /**< device name *//** CNcomment:设备名称 */
    hi_char bssid[BSSID_LEN+1];             /**< MAC address *//** CNcomment:MAC地址 */
    hi_char device_type[DEVICE_TYPE_LEN];   /**< device type *//** CNcomment:设备类型 */
    hi_s32 config_method;                   /**< WPS config method supported*//** CNcomment:支持的WPS连接方式 */
    hi_s32 device_capab;                    /**< device capability *//** CNcomment:设备能力 */
    hi_s32 group_capab;                     /**< group capability *//** CNcomment:group能力 */
    hi_wlan_p2p_wfd_info wfd_info;          /**< WiFi-Display information *//** CNcomment:WiFi-Display信息 */
} hi_wlan_p2p_device_info;

/** configured network information *//** CNcomment:需要连接的网络信息结构体 */
typedef struct
{
    hi_char bssid[BSSID_LEN+1];   /**< peer's MAC address *//** CNcomment:peer的MAC地址 */
    hi_wlan_wps_method wps_method;/**< WPS config method *//** CNcomment:WPS连接方式 */
    hi_char pin[PIN_CODE_LEN+1];  /**< pin if config method is PIN method *//** CNcomment:PIN码 */
    hi_s32  go_intent;            /**< GO intent *//** CNcomment:GO intent */
} hi_wlan_p2p_config;

/** Group information *//** CNcomment:Group信息结构体 */
typedef struct
{
    hi_char iface[IFNAMSIZ+1];
    hi_bool is_group_owner;
    hi_char network_name[NETWORK_NAME_LEN];
    hi_wlan_p2p_device_info go;
} hi_wlan_p2p_group_info;

/** @}*/  /** <!-- ==== Structure Definition End ====*/

/******************************* API Declaration *****************************/
/** \addtogroup     WLAN_P2P*/
/** @{*/  /** <!-- [WLAN_P2P]*/

/**
\brief: Initialize P2P.CNcomment:初始化P2P CNend
\attention \n
\param    N/A.CNcomment:无 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\see \n
::hi_wlan_p2p_init
*/
hi_s32 hi_wlan_p2p_init(hi_void);

/**
\brief: Deintialize P2P.CNcomment:去初始化P2P CNend
\attention \n
\param  N/A.CNcomment:无 CNend
\retval N/A.CNcomment:无 CNend
\see \n
::hi_wlan_p2p_deinit
*/
hi_void hi_wlan_p2p_deinit(hi_void);

/**
\brief: Open WiFi P2P device.CNcomment:打开WiFi P2P设备 CNend
\attention \n
\param[out] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] name_size  parameter ifname length.CNcomment:ifanme的大小, 如: strlen(ifname)
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_DEVICE_NOT_FOUND
\retval  ::HI_WLAN_LOAD_DRIVER_FAIL
\see \n
::hi_wlan_p2p_open
*/
hi_s32 hi_wlan_p2p_open(hi_char *ifname, hi_s32 name_size);

/**
\brief: Close WiFi P2P device.CNcomment:关闭WiFi P2P设备 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_close
*/
hi_s32 hi_wlan_p2p_close(const hi_char *ifname);

/**
\brief: Start WiFi P2P.CNcomment:启动WiFi P2P CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] event_cb  call back function that receives events.CNcomment:接收事件的回调函数 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_START_SUPPLICANT_FAIL
\retval  ::HI_WLAN_CONNECT_TO_SUPPLICANT_FAIL
\see \n
::hi_wlan_p2p_start
*/
hi_s32 hi_wlan_p2p_start(const hi_char *ifname, hi_wlan_sta_event_callback event_cb);

/**
\brief: Stop WiFi P2P.CNcomment:停用WiFi P2P CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_stop
*/
hi_s32 hi_wlan_p2p_stop(const hi_char *ifname);

/**
\brief: search WiFi P2P devices.CNcomment:搜索WiFi P2P设备 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] timeout timeout of find state.CNcomment:搜索P2P设备的超时时间 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_find
*/
hi_s32 hi_wlan_p2p_find(const hi_char *ifname, hi_s32 timeout);

/**
\brief: Get WiFi P2P devices.CNcomment:获取搜索到得WiFi P2P设备 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[out] dev_list   buffer that save devices searched.CNcomment:保存搜索到的设备的数组 CNend
\param[inout] dev_num  number of devices searched.CNcomment:搜索到的设备数量 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_get_peers
*/
hi_s32 hi_wlan_p2p_get_peers(const hi_char *ifname, hi_wlan_p2p_device_info *dev_list, hi_u32 *dev_num);

/**
\brief: connect WiFi P2P peer.CNcomment:连接WiFi P2P设备 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] cfg  P2P network configuration.CNcomment:P2P网络配置 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_connect
*/
hi_s32 hi_wlan_p2p_connect(const hi_char *ifname, hi_wlan_p2p_config *cfg);

/**
\brief: disconnect WiFi P2P connection.CNcomment:断开WiFi P2P连接 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_disconnect
*/
hi_s32 hi_wlan_p2p_disconnect(const hi_char *ifname);

/**
\brief: Set WiFi P2P device information.CNcomment:设置WiFi P2P设备信息 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] pstDev  P2P device configuration.CNcomment:P2P设备配置 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_set_device_info
*/
hi_s32 hi_wlan_p2p_set_device_info(const hi_char *ifname, hi_wlan_p2p_device_info *dev);

/**
\brief: Set WiFi P2P device information.CNcomment:设置WiFi P2P设备信息 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] name  P2P device configuration.CNcomment:P2P设备配置 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_set_device_name
*/
hi_s32 hi_wlan_p2p_set_device_name(const hi_char *ifname, hi_char *name);

/**
\brief: Get persistent groups saved.CNcomment:获得保存的Persistent group CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[out] group_list  save persistent groups.CNcomment:保存Persistent Group的数组 CNend
\param[inout] group_num  number of persistent groups.CNcomment:persistent group的数量 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_get_persistent_groups
*/
hi_s32 hi_wlan_p2p_get_persistent_groups(const hi_char *ifname, hi_wlan_p2p_group_info *group_list,
                                                          hi_s32 *group_num);

/**
\brief: delete saved persistent group.CNcomment:删除保存的Persistent Group CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] group  persistent group to be deleted.CNcomment:要删除的Persistent Group CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_delete_persistent_group
*/
hi_s32 hi_wlan_p2p_delete_persistent_group(const hi_char *ifname, hi_wlan_p2p_group_info *group);

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
::hi_wlan_p2p_get_mac
*/
hi_s32 hi_wlan_p2p_get_mac(const hi_char *ifname, hi_char *mac, hi_u8 mac_size);

/**
\brief: Open WiFi STA + P2P device.CNcomment:打开WiFi P2P设备 CNend
\attention \n
\param[out] sta_ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in]  sta_ifname_size  sta_ifname buffer size.CNcomment:sta_ifname缓存大小 CNend
\param[out] p2p_ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: p2p0 CNend
\param[in]  p2p_ifname_size  p2p_ifname buffer size.CNcomment:p2p_ifname CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_DEVICE_NOT_FOUND
\retval  ::HI_WLAN_LOAD_DRIVER_FAIL
\see \n
::hi_wlan_sta_p2p_open
*/
hi_s32 hi_wlan_sta_p2p_open(hi_char * sta_ifname, hi_u8 sta_ifname_size, hi_char * p2p_ifname,
                                      hi_u8 p2p_ifname_size);

/**
\brief: Start WiFi STA + P2P.CNcomment:启动WiFi P2P CNend
\attention \n
\param[in] sta_ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] p2p_ifname  WiFi network interface name.CNcomment:WiFi Direct网络接口名, 如: p2p0 CNend
\param[in] pfnEventCb  call back function that receives events.CNcomment:接收事件的回调函数 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\retval  ::HI_WLAN_START_SUPPLICANT_FAIL
\retval  ::HI_WLAN_CONNECT_TO_SUPPLICANT_FAIL
\see \n
::HI_WLAN_STA_P2P_Start
*/
hi_s32 hi_wlan_sta_p2p_start(const hi_char *sta_ifname, const hi_char *p2p_ifname,
                                       hi_wlan_sta_event_callback event_cb);

/**
\brief: Close WiFi STA + P2P device.CNcomment:关闭WiFi P2P设备 CNend
\attention \n
\param[in] sta_ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: wlan0 CNend
\param[in] p2p_ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: p2p0 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_sta_p2p_close
*/
hi_s32 hi_wlan_sta_p2p_close(const hi_char *sta_ifname, const hi_char *p2p_ifname);

/**
\brief: WiFi P2P listen.CNcomment:WiFi P2P设备进入listen状态 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: p2p0 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_listen
*/
hi_s32 hi_wlan_p2p_listen(const hi_char *ifname);

/**
\brief: WiFi P2P flush.CNcomment:WiFi P2P设备退出listen状态 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: p2p0 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_flush
*/
hi_s32 hi_wlan_p2p_flush(const hi_char *ifname);

/**
\brief: WiFi P2P add group.CNcomment:WiFi P2P设备进入go状态 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: p2p0 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_group_add
*/
hi_s32 hi_wlan_p2p_group_add(const hi_char *ifname);

/**
\brief: Register event callback function.CNcomment:注册事件回调函数 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: p2p0 CNend
\param[in] event_cb  call back function that receives events.CNcomment:接收事件的回调函数 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_event_register
*/
hi_s32 hi_wlan_p2p_event_register(const hi_char *ifname, hi_wlan_sta_event_callback event_cb);

/**
\brief: Unregister event callback function.CNcomment:注销注册的事件回调函数 CNend
\attention \n
\param[in] ifname  WiFi network interface name.CNcomment:WiFi网络接口名, 如: p2p0 CNend
\param[in] event_cb  call back function that receives events.CNcomment:接收事件的回调函数 CNend
\retval  ::HI_SUCCESS
\retval  ::HI_FAILURE
\retval  ::HI_WLAN_INVALID_PARAMETER
\see \n
::hi_wlan_p2p_event_unregister
*/
hi_s32 hi_wlan_p2p_event_unregister(const hi_char *ifname, hi_wlan_sta_event_callback event_cb);

/** @}*/  /** <!-- ==== API Declaration End ====*/

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif
#endif /* __HI_WLAN_P2P_H__ */
