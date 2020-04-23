#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include "wlan_sm.h"
#include "wlan_util.h"
#include "hi_wlan_sta.h"
#include "securec.h"

#define CMD_REPLY_SIZE  256

hi_s32 hi_wlan_sta_init(hi_void)
{
    return wlan_sm_init();
}

hi_void hi_wlan_sta_deinit(hi_void)
{
    wlan_sm_deinit();
}

hi_s32 hi_wlan_sta_open(hi_char *ifname, hi_u32 name_size)
{
    hi_s32 ret = 0;
    hi_char reply[CMD_REPLY_SIZE] = {0};
    hi_s32 len = sizeof(reply);

    if (ifname == NULL)
        return HI_WLAN_INVALID_PARAMETER;

    ret = wlan_sm_send_message(CMD_STA_OPEN, NULL, reply, &len);
    if (ret != HI_SUCCESS)
        return ret;

    ret = strcpy_s(ifname, name_size, reply);
    if (ret < 0) {
        DBGPRINT(("WiFi: ret=%d file=%s, line=%d, func=%s\n", ret, __FILE__, __LINE__, __FUNCTION__));
    }

    return HI_SUCCESS;
}

hi_s32 hi_wlan_sta_close(const hi_char *ifname)
{
    if (ifname == NULL || *ifname == '\0')
        return HI_WLAN_INVALID_PARAMETER;

    return wlan_sm_send_message(CMD_STA_CLOSE, (hi_void *)ifname, NULL, NULL);
}

hi_s32 hi_wlan_sta_start(const hi_char *ifname, hi_wlan_sta_event_callback event_cb)
{
    if (ifname == NULL || *ifname == '\0')
        return HI_WLAN_INVALID_PARAMETER;

    wlan_sm_register_callback(event_cb);

    return wlan_sm_send_message(CMD_STA_START, (hi_void *)ifname, NULL, NULL);
}

hi_s32 hi_wlan_sta_stop(const hi_char *ifname)
{
    hi_s32 ret;

    if (ifname == NULL || *ifname == '\0')
        return HI_WLAN_INVALID_PARAMETER;

    ret = wlan_sm_send_message(CMD_STA_STOP, (hi_void *)ifname, NULL, NULL);
    if (ret != HI_SUCCESS)
        return ret;

    wlan_sm_unregister_callback();

    return HI_SUCCESS;
}

hi_s32 hi_wlan_sta_start_scan(const hi_char *ifname)
{
    if (ifname == NULL || *ifname == '\0')
        return HI_WLAN_INVALID_PARAMETER;

    return wlan_sm_send_message(CMD_STA_SCAN, (hi_void *)ifname, NULL, NULL);
}

static hi_wlan_security sta_parse_security(hi_char *capability)
{
    hi_wlan_security sec;

    if (capability == NULL)
        return HI_WLAN_SECURITY_MAX;

    if (strstr(capability, "WEP"))
        sec = HI_WLAN_SECURITY_WEP;
    else if (strstr(capability, "WPA-PSK") || strstr(capability, "WPA2-PSK"))
        sec = HI_WLAN_SECURITY_WPA_WPA2_PSK;
    else if (strstr(capability, "WPA-EAP") || strstr(capability, "WPA2-EAP"))
        sec = HI_WLAN_SECURITY_WPA_WPA2_EAP;
    else
        sec = HI_WLAN_SECURITY_OPEN;

    return sec;
}

static
hi_s32 sta_parse_scan_result(hi_char *line, hi_u32 lineSize, hi_wlan_sta_info *ap)
{
#define WPA_SUPPLICANT_8
#define MAX_RSSI    256  // 0 dBm
#define MIN_RSSI    156  // -100 dBm
    hi_s32 results = 0;
    hi_char *result[5] = { NULL };
    hi_s32 rssi;

    if (line == NULL || ap == NULL)
        return -1;

    wlan_util_string_split(line, '\t', result);

    if (result[0] != NULL) {
        results = strncpy_s(ap->bssid, sizeof(ap->bssid), result[0], strlen(result[0])+1);
        if (results < 0) {
            DBGPRINT(("WiFi: results=%d file=%s, line=%d, func=%s\n", results, __FILE__, __LINE__, __FUNCTION__));
        }
    }
    ap->channel = wlan_util_frequency_to_channel(atoi(result[1]));
    rssi = atoi(result[2]);
#ifdef WPA_SUPPLICANT_8
    rssi += 256;
#endif
    if (rssi < MIN_RSSI)
        rssi = MIN_RSSI;
    if (rssi > MAX_RSSI)
        rssi = MAX_RSSI;
    ap->rssi_level = (rssi - MIN_RSSI) * 100 / (MAX_RSSI - MIN_RSSI);
    ap->security = sta_parse_security(result[3]);

    if (result[4] == NULL || strlen(result[4]) > MAX_SSID_LEN)
        return -1;
    results = strncpy_s(ap->ssid, sizeof(ap->ssid), result[4], strlen(result[4])+1);
    if (results < 0) {
        DBGPRINT(("WiFi: results=%d file=%s, line=%d, func=%s\n", results, __FILE__, __LINE__, __FUNCTION__));
    }
    if (ap->ssid[0] == '\0')
        return -1;

    return HI_SUCCESS;
}

hi_s32 hi_wlan_sta_get_scan_results(const hi_char *ifname,hi_wlan_sta_info *ap_list, hi_u32 *ap_num)
{
    hi_s32 ret = HI_SUCCESS;
    hi_char results[4096] = {0};
    hi_s32 size;
    hi_char *pos = results;
    hi_wlan_sta_info ap;
    hi_char line[256];
    hi_s32 i = 0;

    if (ifname == NULL || *ifname == '\0' || ap_list == NULL || ap_num == NULL)
        return HI_WLAN_INVALID_PARAMETER;

    if (*ap_num == 0)
        return HI_SUCCESS;

    /* Request stirng of scan results from wpa_supplicant */
    ret = wlan_sm_sta_scan_results(results, sizeof(results));
    if (ret != HI_SUCCESS)
        return HI_FAILURE;

    /* The first line is "bssid / frequency / signal level / flags / ssid"
     * ignore this line */
    size = wlan_util_read_line(pos, line, sizeof(line));
    if (size == 0) {
        DBGPRINT(("WiFi: anything read for wlan_util_read_line in %s\n", __func__));
    }
    pos += size;

    /* parse scan results, one line a AP */
    while ((size = wlan_util_read_line(pos, line, sizeof(line)))) {
        pos += size;
        if (!sta_parse_scan_result(line, sizeof(line), &ap))
            ap_list[i++] = ap;
        if (i >= *ap_num)
            break;
    }
    *ap_num = i;

    return ret;
}

hi_s32 hi_wlan_sta_connect(const hi_char *ifname, hi_wlan_sta_config *sta_cfg)
{
    hi_s32 ret;
    hi_wlan_sta_connection_status con;

    if (ifname == NULL || *ifname == '\0' || sta_cfg == NULL)
        return HI_WLAN_INVALID_PARAMETER;

    /* check connection, if connected, disconnect to AP firstly */
    ret = hi_wlan_sta_get_connection_status(ifname, &con);
    if (ret == HI_SUCCESS) {
        if (con.state == HI_WLAN_STA_STATUS_CONNECTED) {
            hi_wlan_sta_disconnect(ifname);
        }
    }

    return wlan_sm_send_message(CMD_STA_CONNECT, sta_cfg, NULL, NULL);
}

hi_s32 hi_wlan_sta_disconnect(const hi_char *ifname)
{
    if (ifname == NULL || *ifname == '\0')
        return HI_WLAN_INVALID_PARAMETER;

    return wlan_sm_send_message(CMD_STA_DISCONNECT, NULL, NULL, NULL);
}

hi_s32 hi_wlan_sta_get_connection_status(const hi_char *ifname, hi_wlan_sta_connection_status *conn_status)
{
    if (ifname == NULL || *ifname == '\0' || conn_status == NULL)
        return HI_WLAN_INVALID_PARAMETER;

    return wlan_sm_sta_connection_status(conn_status);
}

hi_s32 hi_wlan_sta_start_wps(const hi_char *ifname, hi_wlan_wps_method wps_method, hi_char *pin)
{
    hi_s32 ret = HI_WLAN_INVALID_PARAMETER;

    if (ifname == NULL)
        return HI_WLAN_INVALID_PARAMETER;

    if (HI_WLAN_WPS_PIN_KEYPAD == wps_method && pin == NULL)
        return HI_WLAN_INVALID_PARAMETER;

    if (HI_WLAN_WPS_PBC == wps_method)
        ret = wlan_sm_start_wps_pbc(NULL);

    if (HI_WLAN_WPS_PIN_KEYPAD == wps_method)
        ret = wlan_sm_start_wps_pin_keypad(pin);

    return ret;
}

hi_s32 hi_wlan_sta_get_mac(const hi_char *ifname, hi_char *mac, hi_u8 mac_size)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 results = 0;
    hi_s32 s = -1;
    struct ifreq ifr;

    if (ifname == NULL || *ifname == '\0' || mac == NULL)
        return HI_WLAN_INVALID_PARAMETER;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0)
        return HI_FAILURE;

    results = memset_s(&ifr, sizeof(struct ifreq), 0, sizeof(struct ifreq));
    if (results != EOK) {
        DBGPRINT(("WiFi: results=%d file=%s, line=%d, func=%s\n", results, __FILE__, __LINE__, __FUNCTION__));
    }
    results = strcpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), ifname);
    if (results != EOK) {
        DBGPRINT(("WiFi: results=%d file=%s, line=%d, func=%s\n", results, __FILE__, __LINE__, __FUNCTION__));
    }

    if(ioctl(s, SIOCGIFHWADDR, &ifr) >= 0) {
        results = sprintf_s(mac, mac_size, MACSTR, MAC2STR(ifr.ifr_hwaddr.sa_data));
        if (results < 0) {
            DBGPRINT(("WiFi: results=%d file=%s, line=%d, func=%s\n", results, __FILE__, __LINE__, __FUNCTION__));
        }
        ret = HI_SUCCESS;
    }

    close(s);
    return ret;
}
