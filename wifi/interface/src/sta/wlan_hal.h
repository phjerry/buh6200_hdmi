#ifndef __WLAN_STA_H__
#define __WLAN_STA_H__

#include <sys/socket.h>
#include <sys/un.h>

#include "hi_wlan_sta.h"
#include "wlan_util.h"

#define REPLY_SIZE       256   /** the max size of wpa_supplicant event */
#define STRING_REPLY_SIZE    4096  /** the max size of command reply */

/** struct of socket information */
typedef struct hiHI_WPA_SOCKET_S {
    hi_s32 s;
    struct sockaddr_un local;
    struct sockaddr_un remote;
} HI_WPA_SOCKET_S;

typedef struct hiHI_WPA_MESSAGE_S {
    hi_char ifname[IFNAMSIZ+1];
    HI_WPA_SOCKET_S *mon_conn;
}HI_WPA_MESSAGE_S;


/* wlan_hal_init
 * brief: Init hal layer
 * return  0    successfull
 *         -1   fail
 */
hi_s32 wlan_hal_init(hi_void);

/* wlan_hal_deinit
 * brief: Deinit hal layer
 */
hi_void wlan_hal_deinit(hi_void);

/* wlan_load_driver
 * brief: Find WiFi device then load it's driver
 * return  0    successfull
 *         -1   cann't find supported device
 *         -2   failed to load driver
 */
hi_s32 wlan_load_driver(hi_void);

/* wlan_unload_driver
 * brief: Unload WiFi driver
 * param   VOID
 * return  HI_SUCCESS    successfull
 *         HI_FAILURE    failed to unload driver
 */
hi_s32 wlan_unload_driver(hi_void);

/* wlan_start_supplicant
 * brief: start wpa_supplicant process
 * param   mode    sta ro p2p or sta&p2p
 * param   sta_ifname    wlan interface name
 * param   p2p_ifname    p2p interface name
 * param   driver    interface of wpa_supplicant and driver, wext, nl80211 etc.
 * param   sta_config_file    directory of wpa_supplicant.conf
 * param   p2p_config_file    directory of p2p_supplicant.conf
 * return  0    successfull
 *         -1   fail
 */
hi_s32 wlan_start_supplicant(HI_WIFI_MODE_E mode, const hi_char *sta_ifname, const hi_char *p2p_ifname, const hi_char *driver,
                            const hi_char *sta_config_file, const hi_char *p2p_config_file);


/* wlan_stop_supplicant
 * brief: stop wpa_supplicant process
 * param   VOID
 * return  0    successfull
 *         -1   fail
 */
hi_s32 wlan_stop_supplicant(hi_void);

/* wlan_wpa_open_connection
 * brief: connect to wpa_supplicant
 * param   ifname    wlan interface name
 * param   ctrl_iface_dir    directory of control interface
 * return  monitor channel
 *         NULL   fail
 */
HI_WPA_SOCKET_S *wlan_wpa_open_connection(const hi_char *ifname,
                            const hi_char *ctrl_iface_dir);

/* wlan_wpa_close_connection
 * brief: close connection to wpa_supplicant
 * param   control   1 - close control channel
 * param   wpa_s     monitor channel
 * return  VOID
 */
hi_void wlan_wpa_close_connection(hi_s32 control, HI_WPA_SOCKET_S *wpa_s);

/* wlan_wpa_request
 * brief: send command to wpa_supplicant
 * param   cmd     command string that send to wpa_supplicant
 * param   cbuf    buffer wpa_supplicant returned
 * param   size    size of buffer returned
 * return  0    successfull
 *         -1   fail
 */
hi_s32 wlan_wpa_request(hi_char *cmd, hi_char *cbuf, hi_s32 *size);

/* wlan_wpa_read
 * brief: read event from wpa_supplicant
 * param   wpa_s   monitor channel
 * param   event   buffer wpa_supplicant returned
 * param   size    size of buffer returned
 * return  0    successfull
 *         -1   fail
 */
hi_s32 wlan_wpa_read(HI_WPA_SOCKET_S *wpa_s, hi_char *event, hi_s32 *size);

/* wlan_wpa_open_p2p_connection
 * brief: connect to wpa_supplicant
 * param   ifname    wlan interface name
 * param   ctrl_iface_dir    directory of control interface
 * return  monitor channel
 *         NULL   fail
 */
HI_WPA_SOCKET_S *wlan_wpa_open_p2p_connection(const hi_char *ifname,
                     const hi_char *ctrl_iface_dir);

/* wlan_wpa_close_p2p_connection
 * brief: close connection to wpa_supplicant
 * param   control   1 - close control channel
 * param   wpa_s     monitor channel
 * return  VOID
 */
hi_void wlan_wpa_close_p2p_connection(hi_s32 control, HI_WPA_SOCKET_S *wpa_s);


/* wlan_wpa_request_p2p
 * brief: send command to wpa_supplicant
 * param   cmd       command string that send to wpa_supplicant
 * param   cmd_size  size of command
 * param   cbuf      buffer wpa_supplicant returned
 * param   size      size of buffer returned
 * return  0    successfull
 *         -1   fail
 */
hi_s32 wlan_wpa_request_p2p(const hi_char *cmd, hi_u32 cmd_size, hi_char *cbuf, hi_u32 *size);


#endif /* __WLAN_STA_H__ */
