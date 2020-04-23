#ifndef __WLAN_UTIL_H__
#define __WLAN_UTIL_H__

#include "hi_type.h"

#define MAX_LEN_OF_LINE 256
#define IFNAMSIZ        16

#define PROC_NET_WIRELESS "/proc/net/wireless"
#define PROC_NET_DEV "/proc/net/dev"

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#ifdef DEBUG
#define DBGPRINT(Fmt)   printf Fmt
#else
#define DBGPRINT(Fmt)
#endif

/** USB ID information */
typedef struct {
    hi_char usb_id[9];
    hi_s32 id;
} hi_wifi_device_info;

/** USB ID of WiFi device */
#define WIFI_ID_RALINK_RT3070        "148f:3070"
#define WIFI_ID_RALINK_RT5370        "148f:5370"
#define WIFI_ID_RALINK_RT5372        "148f:5372"
#define WIFI_ID_RALINK_RT5572        "148f:5572"
#define WIFI_ID_RALINK_MT7601U       "148f:7601"
#define WIFI_ID_ATHEROS_AR9271       "0cf3:9271"
#define WIFI_ID_ATHEROS_AR9374       "0cf3:9374"
#define WIFI_ID_REALTEK_RTL8188SU    "0bda:8171"
#define WIFI_ID_REALTEK_RTL8192SU    "0bda:8172"
#define WIFI_ID_REALTEK_RTL8188CUS   "0bda:8176"
#define WIFI_ID_REALTEK_RTL8192CU    "0bda:8178"
#define WIFI_ID_REALTEK_RTL8188EUS   "0bda:8179"
#define WIFI_ID_REALTEK_RTL8188ETV   "0bda:0179"
#define WIFI_ID_REALTEK_RTL8192DU_VC "0bda:8193"
#define WIFI_ID_REALTEK_RTL8192DU_VS "0bda:8194"
#define WIFI_ID_REALTEK_RTL8192EU    "0bda:818b"
#define WIFI_ID_REALTEK_RTL8812AU    "0bda:8812"
#define WIFI_ID_REALTEK_RTL8812AU_VS "0bda:881a"
#define WIFI_ID_ATHEROS_QCA1021G     "0cf3:1023"
#define WIFI_ID_ATHEROS_QCA1021X     "0cf3:1022"
#define WIFI_ID_BROADCOM_BCM43236    "0a5c:bd17"
#define WIFI_ID_REALTEK_RTL8723BU    "0bda:b720"
#define WIFI_ID_MEDIATEK_MT7632U     "0e8d:760a"
#define WIFI_ID_MEDIATEK_MT7662U     "0e8d:76a0"
#define WIFI_ID_REALTEK_RTL8189ES    "0bda:8189"
#define WIFI_ID_REALTEK_RTL8811CU    "0bda:c811"
#define WIFI_ID_REALTEK_RTL8188FU    "0bda:f179"
#define WIFI_ID_REALTEK_RTL8822BS    "024C:B822"
#define WIFI_ID_REALTEK_RTL8723DU    "0bda:d723"

/** ID of WiFi device */
enum wifi_id {
    WIFI_RALINK_RT3070 = 0,
    WIFI_RALINK_RT5370 = 1,
    WIFI_RALINK_RT5372 = 2,
    WIFI_RALINK_RT5572 = 3,
    WIFI_RALINK_MT7601U = 4,
    WIFI_ATHEROS_AR9271 = 5,
    WIFI_ATHEROS_AR9374 = 6,
    WIFI_REALTEK_RTL8188SU = 7,
    WIFI_REALTEK_RTL8192SU = 8,
    WIFI_REALTEK_RTL8188CUS = 9,
    WIFI_REALTEK_RTL8192CU = 10,
    WIFI_REALTEK_RTL8188EUS = 11,
    WIFI_REALTEK_RTL8188ETV = 12,
    WIFI_REALTEK_RTL8192DU = 13,
    WIFI_REALTEK_RTL8192EU = 14,
    WIFI_ATHEROS_QCA1021G = 15,
    WIFI_ATHEROS_QCA1021X = 16,
    WIFI_BROADCOM_BCM43236 = 17,
    WIFI_REALTEK_RTL8812AU = 18,
    WIFI_REALTEK_RTL8723BU = 19,
    WIFI_MEDIATEK_MT7632U = 20,
    WIFI_MEDIATEK_MT7662U = 21,
    WIFI_REALTEK_RTL8189ES = 22,
    WIFI_REALTEK_RTL8811CU = 23,
    WIFI_REALTEK_RTL8188FU = 24,
    WIFI_REALTEK_RTL8822BS = 25,
    WIFI_REALTEK_RTL8723DU = 26,
   INVALID_DEVICE,
};

/** WiFi mode */
typedef enum hiHI_WIFI_MODE_E {
    WIFI_MODE_STA = 0,
    WIFI_MODE_AP = 1,
    WIFI_MODE_P2P = 2,
    WIFI_MODE_STA_P2P = 3,
    WIFI_MODE_INVALID,
} HI_WIFI_MODE_E;

/* wlan_util_get_wifi_device
 * brief: get the ID of WiFi device
 * return  id   ID of WiFi device
 *         -1   cann't find supported device
 */
hi_s32 wlan_util_get_wifi_device(hi_void);

/* wlan_util_insmod_module
 * brief: insmod kernel module
 * param   module   module file
 * param   module_tag   module tag
 * param   param    paramters with module
 * return  0    successfull
 *         -1   fail
 */
hi_s32 wlan_util_insmod_module(hi_char *module, hi_char *module_tag,
                                 hi_char *param);

/* wlan_util_rmmod_module
 * brief: rmmod kernel module
 * param   module   module name
 * return  0    successfull
 *         -1   fail
 */
hi_s32 wlan_util_rmmod_module(hi_char *module);

/* wlan_util_get_interface
 * brief: get wlan interface name
 * param[out]   ifname   wlan interface name
 * param[in]   nameBufSize   ifname buffer size
 * return  0    successfull
 *         -1   fail
 */
hi_s32 wlan_util_get_interface(HI_WIFI_MODE_E mode, const hi_char *dev_file,
                                 hi_char *ifname, hi_u32 nameBufSize);

/* wlan_util_frequency_to_channel
 * brief: change 80211 frequency to channel number
 * param   80211 frequency (MHz)
 * return  channel number
 */
hi_s32 wlan_util_frequency_to_channel(hi_s32 freq);

/* wlan_util_read_line
 * brief: read the first line from the buf
 * param     buf    buffer
 * param[out]   line   the first line
 * param[in]    lineSize   the line buffer size
 * return  size  size of the line
 */
hi_s32 wlan_util_read_line(hi_char *buf, hi_char *line, hi_u32 lineSize);

/* wlan_util_string_split
 * brief: split string
 * param[in]    src    buffer
 * param[in]    sym    symbol to split
 * param[out]   ss  splited strings
 * return
 */
hi_void wlan_util_string_split(hi_char *src, hi_char sym, hi_char *ss[]);
#endif /* __WLAN_UTIL_H__ */
