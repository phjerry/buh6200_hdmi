#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <spawn.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/wireless.h>

#include "hi_wlan_ap.h"
#include "wlan_util.h"
#include "securec.h"

static const hi_char DEV_WIFI_DIR[] = "/dev/wifi";
static const hi_char HOSTAPD_CONFIG_DIR[] = "/dev/wifi";
static const hi_char HOSTAPD_CONFIG_FILE[] = "/dev/wifi/hostapd.conf";
static const hi_char ENTROPY_FILE[] = "/dev/wifi/entropy.bin";

static hi_s32 device_id = -1;

/* For Broadcom's WiFi, after loading driver, before startup SoftAP, must
 * set SoftAP's firmware path to /sys/module/bcmdhd/paramters/firmware_path.
 * The name of firmware file should contain '_apsta'. */
static hi_s32 ap_update_firmware_path(hi_s32 device)
{
    hi_s32 fd;
    hi_char *fbuf = NULL;

    fd = open("/sys/module/bcmdhd/parameters/firmware_path", O_TRUNC | O_WRONLY);
    if (fd < 0) {
        DBGPRINT(("WiFi: '/sys/module/bcmdhd/parameters/firmware_path' open fail in %s!\n", __func__));
        return -1;
    }

    if (device == WIFI_BROADCOM_BCM43236) {
        if (asprintf(&fbuf, "/lib/firmware/brcm/bcm43236-firmware.bin_apsta.trx") < 0) {
            DBGPRINT(("WiFi: asprintf '/lib/firmware/brcm/bcm43236-firmware.bin_apsta.trx' failure!\n"));
            close(fd);
            return -1;
        }

        if (write(fd, fbuf, strlen(fbuf)) < 0) {
            close(fd);
            free(fbuf);
            fbuf = NULL;
            return -1;
        }
    }

    if (fd >= 0) {
        close(fd);
    }
    if (fbuf != NULL) {
        free(fbuf);
        fbuf = NULL;
    }
    return 0;
}

static hi_s32 ap_load_driver(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    device_id = wlan_util_get_wifi_device();
    if (device_id < 0) {
        DBGPRINT(("WiFi: Cann't find supported device\n"));
        return HI_WLAN_DEVICE_NOT_FOUND;
    }
    DBGPRINT(("WiFi: Find device %d\n", device_id));

    /* insmod driver */
    switch(device_id) {
        case WIFI_RALINK_RT3070:
        case WIFI_RALINK_RT5370:
        case WIFI_RALINK_RT5372:
            if (wlan_util_insmod_module("/kmod/rtutil5370ap.ko", "rtutil5370ap ", NULL)
              || wlan_util_insmod_module("/kmod/rt5370ap.ko", "rt5370ap ", NULL)
              || wlan_util_insmod_module("/kmod/rtnet5370ap.ko", "rtnet5370ap ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_RALINK_RT5572:
            if (wlan_util_insmod_module("/kmod/rtutil5572ap.ko", "rtutil5572ap ", NULL)
              || wlan_util_insmod_module("/kmod/rt5572ap.ko", "rt5572ap ", NULL)
              || wlan_util_insmod_module("/kmod/rtnet5572ap.ko", "rtnet5572ap ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_RALINK_MT7601U:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/mtprealloc.ko", "mtprealloc ", NULL)
              || wlan_util_insmod_module("/kmod/mt7601Usta.ko", "mt7601Usta ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_ATHEROS_AR9271:
            if (wlan_util_insmod_module("/kmod/compat.ko", "compat ", NULL)
              || wlan_util_insmod_module("/kmod/cfg80211_ath9k.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/mac80211.ko", "mac80211 ", NULL)
              || wlan_util_insmod_module("/kmod/ath.ko", "ath ", NULL)
              || wlan_util_insmod_module("/kmod/ath9k_hw.ko", "ath9k_hw ", NULL)
              || wlan_util_insmod_module("/kmod/ath9k_common.ko", "ath9k_common ", NULL)
              || wlan_util_insmod_module("/kmod/ath9k_htc.ko", "ath9k_htc ", NULL)
              || wlan_util_insmod_module("/kmod/ath9k.ko", "ath9k ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_ATHEROS_AR9374:
        case WIFI_ATHEROS_QCA1021G:
        case WIFI_ATHEROS_QCA1021X:
            if (wlan_util_insmod_module("/kmod/cfg80211_ath6k.ko", "cfg80211_ath6k ", NULL)
              || wlan_util_insmod_module("/kmod/ath6kl_usb.ko", "ath6kl_usb ",
                                           "reg_domain=0x809c"))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8188SU:
        case WIFI_REALTEK_RTL8192SU:
            if (wlan_util_insmod_module("/kmod/rtl8712u.ko", "rtl8712u ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8188CUS:
        case WIFI_REALTEK_RTL8192CU:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8192cu.ko", "rtl8192cu ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8188EUS:
        case WIFI_REALTEK_RTL8188ETV:
        case WIFI_REALTEK_RTL8189ES:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8188eu.ko", "rtl8188eu ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8188FU:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8188fu.ko", "rtl8188fu ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8192DU:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8192du.ko", "rtl8192du ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8192EU:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8192eu.ko", "rtl8192eu ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8812AU:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8812au.ko", "rtl8812au ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8811CU:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8811cu.ko", "rtl8811cu ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_BROADCOM_BCM43236:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/bcmdhd.ko", "bcmdhd ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            else {
                sleep(2);
                ap_update_firmware_path(device_id);
            }
            break;
        case WIFI_REALTEK_RTL8723BU:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8723bu.ko", "rtl8723bu ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_MEDIATEK_MT7632U:
        case WIFI_MEDIATEK_MT7662U:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/mtprealloc76x2Usta.ko", "mtprealloc76x2Usta ", NULL)
              || wlan_util_insmod_module("/kmod/mt7662u_sta.ko", "mt7662u_sta ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8822BS:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8822bs.ko", "rtl8822bs ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8723DU:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8723du.ko", "rtl8723du ", "ifname=wlan0 if2name=p2p0"))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        default:
            DBGPRINT(("WiFi: device %d is not supported, "
                        "cann't load driver\n", device_id));
            ret = HI_WLAN_DEVICE_NOT_FOUND;
            break;
    }

    if (ret == HI_WLAN_LOAD_DRIVER_FAIL)
        DBGPRINT(("WiFi: Load driver fail\n"));

    return ret;
}

static hi_s32 ap_unload_driver(hi_void)
{
    hi_s32 ret = HI_FAILURE;

    DBGPRINT(("WiFi: Unloading driver\n"));
    /* rmmod driver */
    switch(device_id) {
        case WIFI_RALINK_RT3070:
        case WIFI_RALINK_RT5370:
        case WIFI_RALINK_RT5372:
            if ((wlan_util_rmmod_module("rtnet5370ap") == 0)
              && (wlan_util_rmmod_module("rt5370ap") == 0)
              && (wlan_util_rmmod_module("rtutil5370ap") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_RALINK_RT5572:
            if ((wlan_util_rmmod_module("rtnet5572ap") == 0)
              && (wlan_util_rmmod_module("rt5572ap") == 0)
              && (wlan_util_rmmod_module("rtutil5572ap") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_RALINK_MT7601U:
            if ((wlan_util_rmmod_module("mt7601Usta") == 0)
              && (wlan_util_rmmod_module("cfg80211") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_ATHEROS_AR9271:
            if ((wlan_util_rmmod_module("ath9k") == 0)
              && (wlan_util_rmmod_module("ath9k_htc") == 0)
              && (wlan_util_rmmod_module("ath9k_common") == 0)
              && (wlan_util_rmmod_module("ath9k_hw") == 0)
              && (wlan_util_rmmod_module("ath") == 0)
              && (wlan_util_rmmod_module("mac80211") == 0)
              && (wlan_util_rmmod_module("cfg80211_ath9k") == 0)
              && (wlan_util_rmmod_module("compat") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_ATHEROS_AR9374:
        case WIFI_ATHEROS_QCA1021G:
        case WIFI_ATHEROS_QCA1021X:
            if ((wlan_util_rmmod_module("ath6kl_usb") == 0)
              && (wlan_util_rmmod_module("cfg80211_ath6k") == 0)) {
                sleep(2);
                ret = HI_SUCCESS;
            }
            break;
        case WIFI_REALTEK_RTL8188SU:
        case WIFI_REALTEK_RTL8192SU:
            if (wlan_util_rmmod_module("rtl8712u") == 0)
                ret = HI_SUCCESS;
            break;
        case WIFI_REALTEK_RTL8188CUS:
        case WIFI_REALTEK_RTL8192CU:
            if ((wlan_util_rmmod_module("rtl8192cu") == 0)
              && (wlan_util_rmmod_module("cfg80211") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_REALTEK_RTL8188EUS:
        case WIFI_REALTEK_RTL8188ETV:
        case WIFI_REALTEK_RTL8189ES:
            if ((wlan_util_rmmod_module("rtl8188eu") == 0)
              && (wlan_util_rmmod_module("cfg80211") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_REALTEK_RTL8188FU:
            if ((wlan_util_rmmod_module("rtl8188fu") == 0)
              && (wlan_util_rmmod_module("cfg80211") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_REALTEK_RTL8192DU:
            if ((wlan_util_rmmod_module("rtl8192du") == 0)
              && (wlan_util_rmmod_module("cfg80211") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_REALTEK_RTL8192EU:
            if ((wlan_util_rmmod_module("rtl8192eu") == 0)
              && (wlan_util_rmmod_module("cfg80211") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_REALTEK_RTL8812AU:
            if ((wlan_util_rmmod_module("rtl8812au") == 0)
              && (wlan_util_rmmod_module("cfg80211") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_REALTEK_RTL8811CU:
            if ((wlan_util_rmmod_module("rtl8811cu") == 0)
              && (wlan_util_rmmod_module("cfg80211") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_BROADCOM_BCM43236:
            if ((wlan_util_rmmod_module("bcmdhd") == 0)
              && (wlan_util_rmmod_module("cfg80211") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_REALTEK_RTL8723BU:
            if ((wlan_util_rmmod_module("rtl8723bu") == 0)
              && (wlan_util_rmmod_module("cfg80211") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_MEDIATEK_MT7632U:
        case WIFI_MEDIATEK_MT7662U:
            if ((wlan_util_rmmod_module("mt7662u_sta") == 0)
              && (wlan_util_rmmod_module("cfg80211") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_REALTEK_RTL8822BS:
            if ((wlan_util_rmmod_module("rtl8822bs") == 0)
              && (wlan_util_rmmod_module("cfg80211") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_REALTEK_RTL8723DU:
            if ((wlan_util_rmmod_module("rtl8723du") == 0)
              && (wlan_util_rmmod_module("cfg80211") == 0))
                ret = HI_SUCCESS;
            break;
        default:
            DBGPRINT(("WiFi: device %d is not supported, "
                        "cann't unload driver\n", device_id));
            break;
    }

    if (ret == HI_SUCCESS)
        device_id = -1;

    return ret;
}

/* configure softap by sending private ioctls to driver directly */
static hi_s32 ap_config_with_iwpriv_cmd(hi_s32 s, const hi_char *ifname,
                           hi_wlan_ap_config *ap_cfg)
{
    hi_char tBuf[4096];
    struct iwreq wrq;
    struct iw_priv_args *priv_ptr = NULL;
    hi_s32 i, j;
    hi_s32 cmd = 0, sub_cmd = 0;
    hi_char mBuf[256];
    hi_s32 results = 0;

    /* get all private commands that driver supported */
    results = strncpy_s(wrq.ifr_name, sizeof(wrq.ifr_name), ifname, strlen(ifname)+1);
    if (results < EOK) {
        DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
    }
    wrq.u.data.pointer = tBuf;
    wrq.u.data.length = sizeof(tBuf) / sizeof(struct iw_priv_args);
    wrq.u.data.flags = 0;
    if (ioctl(s, SIOCGIWPRIV, &wrq) < 0) {
        return HI_FAILURE;
    }

    /* if driver don't support 'set' command, return failure */
    priv_ptr = (struct iw_priv_args *)wrq.u.data.pointer;
    for(i = 0; i < wrq.u.data.length; i++) {
        if (strcmp(priv_ptr[i].name, "set") == 0) {
            cmd = priv_ptr[i].cmd;
            break;
        }
    }
    if (i == wrq.u.data.length) {
        return HI_FAILURE;
    }

    /* get the 'set' command's ID */
    if (cmd < SIOCDEVPRIVATE) {
        for(j = 0; j < i; j++) {
            if ((priv_ptr[j].set_args == priv_ptr[i].set_args)
                && (priv_ptr[j].get_args == priv_ptr[i].get_args)
                && (priv_ptr[j].name[0] == '\0'))
                break;
        }
        if (j == i) {
            return HI_FAILURE;
        }
        sub_cmd = cmd;
        cmd = priv_ptr[j].cmd;
    }

    /* configure AP, order should be as follow
     *   0. WirelessMode
     *   1. Channel
     *   2. AuthMode
     *   3. EncrypType
     * for WPAPSK/WPA2PSK:
     *   4. SSID (must after AuthMode and before Password)
     *   5. Password
     * for WEP:
     *   4. Password
     *   5. SSID (must set lastly)
     */
    results = strncpy_s(wrq.ifr_name, sizeof(wrq.ifr_name), ifname, strlen(ifname)+1);
    if (results < EOK) {
        DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
    }
    wrq.u.data.pointer = mBuf;
    wrq.u.data.flags = sub_cmd;

    /* configure WirelessMode */
    if (device_id == WIFI_RALINK_RT5572) {
        results = sprintf_s(mBuf, sizeof(mBuf), "WirelessMode=5");
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
    } else {
        results = sprintf_s(mBuf, sizeof(mBuf), "WirelessMode=9");
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
    }
    wrq.u.data.length = strlen(mBuf) + 1;
    if(ioctl(s, cmd, &wrq) < 0)
        return HI_FAILURE;

    /* configure Channel */
    results = sprintf_s(mBuf, sizeof(mBuf), "Channel=%d", ap_cfg->channel);
    if (results < EOK) {
        DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
    }
    wrq.u.data.length = strlen(mBuf) + 1;
    if(ioctl(s, cmd, &wrq) < 0)
        return HI_FAILURE;

    /* configure AuthMode */
    if (ap_cfg->security == HI_WLAN_SECURITY_OPEN) {
        results = sprintf_s(mBuf, sizeof(mBuf), "AuthMode=OPEN");
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
    } else if(ap_cfg->security == HI_WLAN_SECURITY_WEP) {
        results = sprintf_s(mBuf, sizeof(mBuf), "AuthMode=WEPAUTO");
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
    } else if (ap_cfg->security == HI_WLAN_SECURITY_WPA_WPA2_PSK) {
        results = sprintf_s(mBuf, sizeof(mBuf), "AuthMode=WPA2PSK");
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
    }
    wrq.u.data.length = strlen(mBuf) + 1;
    if(ioctl(s, cmd, &wrq) < 0)
        return HI_FAILURE;

    /* configure EncrypType */
    if (ap_cfg->security == HI_WLAN_SECURITY_OPEN) {
        results = sprintf_s(mBuf, sizeof(mBuf), "EncrypType=NONE");
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
    } else if (ap_cfg->security == HI_WLAN_SECURITY_WEP) {
        results = sprintf_s(mBuf, sizeof(mBuf), "EncrypType=WEP");
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
    } else if (ap_cfg->security == HI_WLAN_SECURITY_WPA_WPA2_PSK) {
        results = sprintf_s(mBuf, sizeof(mBuf), "EncrypType=AES");
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
    }
    wrq.u.data.length = strlen(mBuf) + 1;
    if(ioctl(s, cmd, &wrq) < 0)
        return HI_FAILURE;

    /* configure password of WEP */
    if (ap_cfg->security == HI_WLAN_SECURITY_WEP) {
        results = sprintf_s(mBuf, sizeof(mBuf), "DefaultKeyID=1");
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        wrq.u.data.length = strlen(mBuf) + 1;
        if(ioctl(s, cmd, &wrq) < 0)
            return HI_FAILURE;

        results = sprintf_s(mBuf, sizeof(mBuf), "Key1=%s", ap_cfg->password);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        wrq.u.data.length = strlen(mBuf) + 1;
        if(ioctl(s, cmd, &wrq) < 0)
            return HI_FAILURE;
    }

    /* configure hide SSID */
    results = sprintf_s(mBuf, sizeof(mBuf), "HideSSID=%d", ap_cfg->hidden_ssid ? 1 : 0);
    if (results < EOK) {
        DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
    }
    wrq.u.data.length = strlen(mBuf) + 1;
    if(ioctl(s, cmd, &wrq) < 0)
        return HI_FAILURE;

    /* configure SSID */
    if (snprintf_s(mBuf, sizeof(mBuf), sizeof(mBuf), "SSID=%s", ap_cfg->ssid) < 0) {
        DBGPRINT(("WiFi: file=%s, func=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__));
    }
    wrq.u.data.length = strlen(mBuf) + 1;
    if(ioctl(s, cmd, &wrq) < 0)
        return HI_FAILURE;

    /* configure password of WPAPSK/WPA2PSK */
    if (ap_cfg->security == HI_WLAN_SECURITY_WPA_WPA2_PSK) {
        results = snprintf_s(mBuf, sizeof(mBuf), sizeof(ap_cfg->password), "WPAPSK=%s", ap_cfg->password);
        if (results < 0) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        wrq.u.data.length = strlen(mBuf) + 1;
        if(ioctl(s, cmd, &wrq) < 0)
            return HI_FAILURE;

        if (device_id == WIFI_RALINK_MT7601U) {
            /* for MT7601U, configure SSID again */
            results = snprintf_s(mBuf, sizeof(mBuf), sizeof(ap_cfg->ssid), "SSID=%s", ap_cfg->ssid);
            if (results < 0) {
                DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
            }
            wrq.u.data.length = strlen(mBuf) + 1;
            if(ioctl(s, cmd, &wrq) < 0)
                return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_s32 ap_start_hostapd(const hi_char *config_file, const hi_char *entropy_file)
{
    hi_s32 ret = 0;
    hi_s32 results = 0;
    hi_s32 pid = 0;
    hi_s32 status = 0;
//    hi_char cmd[256] = {0};
    hi_char param[2][256] = {{0}};
    hi_char *spawn_env[] = {NULL};
    hi_char *spawn_args[] = {"hostapd", NULL, NULL, NULL, NULL};

    results = snprintf_s(param[0], sizeof(param[0]), sizeof(param[0]), "%s", config_file);
    if (results == -1) {
        DBGPRINT(("WiFi: results=%d file=%s,line=%d,func=%s\n", results, __FILE__, __LINE__, __FUNCTION__));
    }
    results = snprintf_s(param[1], sizeof(param[1]), sizeof(param[1]), "-e%s", entropy_file);
    if (results == -1) {
        DBGPRINT(("WiFi: results=%d file=%s,line=%d,func=%s\n", results, __FILE__, __LINE__, __FUNCTION__));
    }
    spawn_args[1] = "-B";
    spawn_args[2] = param[1];
    spawn_args[3] = param[0];
#if 0

    results = snprintf_s(cmd, sizeof(cmd), sizeof(cmd), "%s %s %s %s", spawn_args[0], spawn_args[1], spawn_args[2], spawn_args[3]);
    if (results == -1) {
        DBGPRINT(("WiFi: results=%d file=%s,line=%d,func=%s\n", results, __FILE__, __LINE__, __FUNCTION__));
    }
    DBGPRINT(("WiFi: system cmd = '%s'\n", cmd));
    ret = system(cmd);
#endif

    ret = posix_spawnp(&pid, spawn_args[0], NULL, NULL,
                   spawn_args, spawn_env);
    if (ret != 0) {
        return -1;
    }

    ret = waitpid(pid, &status, 0);
    if (ret == -1) {
        return -1;
    }

    return ret;
}

static hi_s32 ap_kill_hostapd(hi_void)
{
    hi_s32 ret = 0;
    hi_s32 pid = 0;
    hi_s32 status = 0;
//    hi_char cmd[256] = {0};
    hi_char *spawn_env[] = {NULL};
    hi_char *spawn_args[] = {"killall", NULL, NULL};

    spawn_args[1] = "hostapd";
#if 0

    ret = snprintf_s(cmd, sizeof(cmd), sizeof(cmd), "%s %s", spawn_args[0], spawn_args[1]);
    if (ret < 0) {
        DBGPRINT(("WiFi: ret=%d file=%s, func=%s, line=%d\n", ret, __FILE__, __FUNCTION__, __LINE__));
    }
    DBGPRINT(("WiFi: system cmd = '%s'\n", cmd));
    ret = system(cmd);
#endif

    ret = posix_spawnp(&pid, spawn_args[0], NULL, NULL,
                   spawn_args, spawn_env);
    if (ret != 0) {
        return -1;
    }

    ret = waitpid(pid, &status, 0);
    if (ret == -1) {
        perror("waitpid");
        return -1;
    }

    return ret;
}

static hi_s32 ap_update_hostapd_config_file(const hi_char *ifname,
            hi_wlan_ap_config *ap_cfg, const hi_char *config_file)
{
    hi_s32 ret = 0;
    hi_s32 results = 0;
    hi_s32 fd = -1;
    hi_char *fbuf = NULL;
    hi_char *wbuf = NULL;
    DIR *dir = NULL;
    hi_s32 broadcast_ssid = 0;
    hi_char hw_mode;
    hi_char ht40[32];
    hi_char file_path[PATH_MAX+1] = {0};

    /* ensure /dev/wifi exist */
    dir = opendir(DEV_WIFI_DIR);
    if (!dir) {
        if (mkdir(DEV_WIFI_DIR, 0666) < 0) {
            DBGPRINT(("WiFi: Create '%s' fail\n", DEV_WIFI_DIR));
            return -1;
        }
    }
    closedir(dir);

    /* ensure hostapd configure file directory exist */
    dir = opendir(HOSTAPD_CONFIG_DIR);
    if (!dir) {
        if (mkdir(HOSTAPD_CONFIG_DIR, 0666) < 0) {
            DBGPRINT(("WiFi: Create '%s' fail\n", HOSTAPD_CONFIG_DIR));
            return -1;
        }
    }
    closedir(dir);

    /* open configure file, if not exist, create it */
    if (realpath(config_file, file_path) == NULL) {
        DBGPRINT(("WiFi: file path '%s' no exist and create it [%s]!\n", file_path, __func__));
    }
    fd = open(file_path, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd < 0) {
        DBGPRINT(("WiFi: Cann't open configure file '%s'\n", file_path));
        return -1;
    }

    /* set broadcast ssid */
    if (ap_cfg->hidden_ssid == HI_TRUE) {
        DBGPRINT(("WiFi: Enable hidden SSID\n"));
        if (device_id == WIFI_ATHEROS_AR9374
          || device_id == WIFI_ATHEROS_QCA1021G
          || device_id == WIFI_ATHEROS_QCA1021X)
            broadcast_ssid = 2;     // clear SSID (ASCII 0)
        else
            broadcast_ssid = 1;     // send empty (length 0) SSID
    }

    /* set HT40 capability */
    results = memset_s(ht40, sizeof(ht40), 0, sizeof(ht40));
    if (results < 0) {
        DBGPRINT(("WiFi: results=%d file=%s,line=%d,func=%s\n", results, __FILE__, __LINE__, __FUNCTION__));
    }
    if (ap_cfg->channel >= 36) {
        hi_u32 i;
        hi_u32 ht40plus[] = {36, 44, 52, 60, 100, 108, 116, 124,
                             132, 149, 157};
        hi_u32 ht40minus[] = {40, 48, 56, 64, 104, 112, 120, 128,
                              136, 153, 161};

        hw_mode = 'a';

        for (i = 0; i < sizeof(ht40plus)/sizeof(ht40plus[0]); i++)
            if (ap_cfg->channel == ht40plus[i]) {
                results = strcpy_s(ht40, sizeof(ht40), "[SHORT-GI-40][HT40+]");
                if (results < 0) {
                    DBGPRINT(("WiFi: results=%d file=%s,line=%d,func=%s\n", results, __FILE__, __LINE__, __FUNCTION__));
                }
                break;
            }

        for (i = 0; i < sizeof(ht40minus)/sizeof(ht40minus[0]); i++)
            if (ap_cfg->channel == ht40minus[i]) {
                results = strcpy_s(ht40, sizeof(ht40), "[SHORT-GI-40][HT40-]");
                if (results < 0) {
                    DBGPRINT(("WiFi: results=%d file=%s,line=%d,func=%s\n", results, __FILE__, __LINE__, __FUNCTION__));
                }
                break;
            }
    } else {
        hw_mode = 'g';
        if (ap_cfg->channel > 7) {
            results = strcpy_s(ht40, sizeof(ht40), "[SHORT-GI-40][HT40-]");
            if (results < EOK) {
                DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
            }
        } else {
            results = strcpy_s(ht40, sizeof(ht40), "[SHORT-GI-40][HT40+]");
            if (results < EOK) {
                DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
            }
        }
    }

    /* Broadcom's WiFi doesn't support 40M bandwidth in 2.4G, and cann't be set
     * HT40 capability in 5G
     */
    if (device_id == WIFI_BROADCOM_BCM43236) {
        results = memset_s(ht40, sizeof(ht40), 0, sizeof(ht40));
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
    }

    /* set common paramters */
    if (ap_cfg->security == HI_WLAN_SECURITY_WEP){
        /* WEP is not supported in 802.11n */
        if (asprintf(&wbuf, "interface=%s\n"
                    "driver=%s\n"
                    "ctrl_interface=/dev/wifi/hostapd\n"
                    "ssid=%s\n"
                    "channel=%d\n"
                    "ignore_broadcast_ssid=%d\n"
                    "hw_mode=%c\n",
                    ifname, "nl80211", ap_cfg->ssid, ap_cfg->channel,
                    broadcast_ssid, hw_mode) < 0) {
            DBGPRINT(("WiFi: asprintf ap_cfg->security HI_WLAN_SECURITY_WEP failure\n"));
            close(fd);
            free(wbuf);
            return -1;
        }
    } else {
        if (asprintf(&wbuf, "interface=%s\n"
                    "driver=%s\n"
                    "ctrl_interface=/dev/wifi/hostapd\n"
                    "ssid=%s\n"
                    "channel=%d\n"
                    "ignore_broadcast_ssid=%d\n"
                    "hw_mode=%c\n"
                    "ieee80211n=1\n"
                    "ht_capab=[SHORT-GI-20]%s\n",
                    ifname, "nl80211", ap_cfg->ssid, ap_cfg->channel,
                    broadcast_ssid, hw_mode,
                    ht40) < 0) {
            DBGPRINT(("WiFi: asprintf ap_cfg->security failure\n"));
            close(fd);
            free(wbuf);
            return -1;
        }
    }

    /* set auth mode */
    if (ap_cfg->security == HI_WLAN_SECURITY_WEP) {
        hi_s32 pwd_len = strlen(ap_cfg->password);

        if (pwd_len == 5 || pwd_len == 13) {
            if (asprintf(&fbuf, "%swep_default_key=0\n"
                            "wep_key0=\"%s\"\n", wbuf, ap_cfg->password) < 0) {
                DBGPRINT(("WiFi: asprintf HI_WLAN_SECURITY_WEP pwd_len failure\n"));
                close(fd);
                free(wbuf);
                free(fbuf);
                return -1;
            }
        } else {
            if (asprintf(&fbuf, "%swep_default_key=0\n"
                            "wep_key0=%s\n", wbuf, ap_cfg->password) < 0) {
                DBGPRINT(("WiFi: asprintf HI_WLAN_SECURITY_WEP pwd failure\n"));
                close(fd);
                free(wbuf);
                free(fbuf);
                return -1;
            }
        }
    } else if (ap_cfg->security == HI_WLAN_SECURITY_WPA_WPA2_PSK) {
        if (device_id == WIFI_BROADCOM_BCM43236) {
            if (asprintf(&fbuf, "%swpa=2\n"
                            "auth_algs=3"
                            "wpa_key_mgmt=WPA-PSK\n"
                            "wpa_pairwise=TKIP CCMP\n"
                            "rsn_pairwise=TKIP CCMP\n"
                            "wpa_passphrase=%s\n", wbuf, ap_cfg->password) < 0) {
                DBGPRINT(("WiFi: WIFI_BROADCOM_BCM43236 device asprintf HI_WLAN_SECURITY_WPA_WPA2_PSK failure\n"));
                close(fd);
                free(wbuf);
                free(fbuf);
                return -1;
            }
        } else {
            if (asprintf(&fbuf, "%swpa=3\n"
                            "wpa_key_mgmt=WPA-PSK\n"
                            "wpa_pairwise=TKIP CCMP\n"
//                            "rsn_pairwise=CCMP\n"
                            "wpa_passphrase=%s\n", wbuf, ap_cfg->password) < 0) {
                DBGPRINT(("WiFi: asprintf HI_WLAN_SECURITY_WPA_WPA2_PSK failure\n"));
                close(fd);
                free(wbuf);
                free(fbuf);
                return -1;
            }
       }
    } else {
        if (asprintf(&fbuf, "%s", wbuf) < 0) {
            DBGPRINT(("WiFi: asprintf ap_cfg->security failure\n"));
            close(fd);
            free(wbuf);
            free(fbuf);
            return -1;
        }
    }

    if (write(fd, fbuf, strlen(fbuf)) < 0) {
        DBGPRINT(("WiFi: Cann't write configuration to '%s'\n", file_path));
        ret = -1;
    }
    close(fd);
    free(wbuf);
    free(fbuf);

    if (chmod(file_path, 0666) < 0) {
        DBGPRINT(("WiFi: Failed to change '%s' to 0666\n", file_path));
        unlink(file_path);
        ret = -1;
    }

    return ret;
}

int ensure_entropy_file_exists(const hi_char *entropy_file)
{
    hi_s32 ret = 0;
    hi_s32 fd = -1;
    char *fbuf = NULL;
    char file[PATH_MAX+1] = {0};
    DIR *dirptr = NULL;
    static unsigned char dummy_key[21] = { 0x02, 0x11, 0xbe, 0x33, 0x43, 0x35,
                                           0x68, 0x47, 0x84, 0x99, 0xa9, 0x2b,
                                           0x1c, 0xd3, 0xee, 0xff, 0xf1, 0xe2,
                                           0xf3, 0xf4, 0xf5 };

    if (NULL == realpath(entropy_file, file)) {
        DBGPRINT(("WiFi: file path '%s' error\n", file));
        return -1;
    }

    dirptr = opendir(file);
    if(!dirptr) {
        DBGPRINT(("WiFi: entropy file already exists\n"));
        return 0;
    }
    closedir(dirptr);

    fd = open(file, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fd < 0) {
        DBGPRINT(("WiFi: Cann't open entropy file '%s'\n", file));
        return -1;
    }
    if (asprintf(&fbuf, "%s", dummy_key) < 0) {
        DBGPRINT(("WiFi: asprintf dummy_key '%s' failure!\n", dummy_key));
        close(fd);
        free(fbuf);
        return -1;
    }

    if (write(fd, fbuf, strlen(fbuf)) < 0) {
        DBGPRINT(("WiFi: Cann't write configuration to '%s'\n", file));
        ret = -1;
    }
    close(fd);
    free(fbuf);

    if (chmod(file, 0666) < 0) {
        DBGPRINT(("WiFi: Failed to change '%s' to 0666\n", file));
        unlink(file);
        ret = -1;
    }

    return ret;
}

hi_s32 hi_wlan_ap_init(hi_void)
{
    // TODO
    return HI_SUCCESS;
}

hi_void hi_wlan_ap_deinit(hi_void)
{
    // TODO
}

hi_s32 hi_wlan_ap_open(hi_char *ifname, hi_u32 name_size)
{
    hi_s32 ret = HI_FAILURE;
    hi_char iface[IFNAMSIZ + 1];
    hi_s32 count;

    if (ifname == NULL)
        return HI_WLAN_INVALID_PARAMETER;

    ret = ap_load_driver();
    if (ret != HI_SUCCESS)
        return ret;
    DBGPRINT(("WiFi: Driver loaded successfully\n"));

    /* when driver startup, a new wireless network interface will be
     * created, wait 5s for the interface created successfully */
    for (count = 0; count < 50; count++) {
        ret = wlan_util_get_interface(WIFI_MODE_AP, PROC_NET_DEV, iface, sizeof(iface));
        if (ret == HI_FAILURE) {
            ret = wlan_util_get_interface(WIFI_MODE_AP, PROC_NET_WIRELESS, iface, sizeof(iface));
        }
        if (ret == HI_SUCCESS) {
            DBGPRINT(("WiFi: Get interface '%s'\n", iface));
            ret = strncpy_s(ifname, name_size, iface, strlen(iface)+1);
            if (ret < 0) {
                DBGPRINT(("WiFi: ret=%d file=%s,line=%d,func=%s\n", ret, __FILE__, __LINE__, __FUNCTION__));
            }

            return HI_SUCCESS;
        }
        usleep(100000);
    }
    DBGPRINT(("WiFi: Failed to get interface, driver initialized fail!\n"));
    ap_unload_driver();

    return HI_FAILURE;
}

hi_s32 hi_wlan_ap_close(const hi_char *ifname)
{
    hi_s32 ret;

    if (ifname == NULL || *ifname == '\0')
        return HI_WLAN_INVALID_PARAMETER;

    /* unload WiFi driver */
    ret = ap_unload_driver();

    return ret;
}

hi_s32 hi_wlan_ap_start(const hi_char *ifname, hi_wlan_ap_config *ap_cfg)
{
    hi_s32 ret = HI_FAILURE;
    hi_s32 results = 0;

    if (ifname == NULL || *ifname == '\0' || ap_cfg == NULL)
        return HI_WLAN_INVALID_PARAMETER;

    /* check configures */
    if (strlen(ap_cfg->ssid) == 0)
        return HI_WLAN_INVALID_PARAMETER;

    /* channel, 11bg: 1 - 14, 11a: 36 - 165 */
    if (ap_cfg->channel < 1 || ap_cfg->channel > 165)
        return HI_WLAN_INVALID_PARAMETER;

    if (ap_cfg->security >= HI_WLAN_SECURITY_MAX)
        return HI_WLAN_INVALID_PARAMETER;

    if (device_id == WIFI_RALINK_RT3070 || device_id == WIFI_RALINK_RT5370
      || device_id == WIFI_RALINK_RT5372 || device_id == WIFI_RALINK_RT5572) {
        struct ifreq ifr;
        hi_s32 s = -1;

        /* For MTK WiFi, don't use hostapd, driver reads RT2870AP.dat and
         * configures WiFi to AP mode while intialize. After initialization
         * complete, configure WiFi interface up will startup AP. Then
         * reconfigure AP by private commands.
         */
        results = memset_s(&ifr, sizeof(struct ifreq), 0, sizeof(struct ifreq));
        if (results < 0) {
            DBGPRINT(("WiFi: results=%d file=%s,line=%d,func=%s\n", results, __FILE__, __LINE__, __FUNCTION__));
        }
        results = strncpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), ifname, strlen(ifname)+1);
        if (results < 0) {
            DBGPRINT(("WiFi: results=%d file=%s,line=%d,func=%s\n", results, __FILE__, __LINE__, __FUNCTION__));
        }

        if((s = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) {
            if(ioctl(s, SIOCGIFFLAGS, &ifr) >= 0) {
                ifr.ifr_flags = (ifr.ifr_flags | IFF_UP);
                if (ioctl(s, SIOCSIFFLAGS, &ifr) >= 0) {
                    ret = ap_config_with_iwpriv_cmd(s, ifname, ap_cfg);
                }
            }
            close(s);
        }

    } else {
        /* startup AP by hostapd. hostapd will configure WiFi to AP mode, then
         * start it */
        ret = ap_update_hostapd_config_file(ifname, ap_cfg, HOSTAPD_CONFIG_FILE);
        if (-1 == ret) {
            DBGPRINT(("WiFi: update hostapd config file fail\n"));
            return ret;
        }

        ret = ensure_entropy_file_exists(ENTROPY_FILE);
        if (-1 == ret) {
            DBGPRINT(("WiFi: creat entropy file fail\n"));
        }

        ret = ap_start_hostapd(HOSTAPD_CONFIG_FILE, ENTROPY_FILE);
        if (ret < 0) {
            DBGPRINT(("WiFi: start hostapd fail\n"));
            return HI_WLAN_START_HOSTAPD_FAIL;
        }
    }

    DBGPRINT(("WiFi: SoftAP started\n"));

    return HI_SUCCESS;
}

hi_s32 hi_wlan_ap_stop(const hi_char *ifname)
{
    struct ifreq ifr;
    hi_s32 s = -1;
    hi_s32 ret = 0;

    if (ifname == NULL || *ifname == '\0')
        return HI_WLAN_INVALID_PARAMETER;

    /* configure WiFi interface down */
    ret = memset_s(&ifr, sizeof(struct ifreq), 0, sizeof(struct ifreq));
    if (ret < 0) {
        DBGPRINT(("WiFi: ret=%d file=%s, func=%s, line=%d\n", ret, __FILE__, __FUNCTION__, __LINE__));
    }
    ret = strncpy_s(ifr.ifr_name, sizeof(ifr.ifr_name), ifname, strlen(ifname)+1);
    if (ret < 0) {
        DBGPRINT(("WiFi: ret=%d file=%s, func=%s, line=%d\n", ret, __FILE__, __FUNCTION__, __LINE__));
    }

    if((s = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) {
        if(ioctl(s, SIOCGIFFLAGS, &ifr) >= 0) {
            ifr.ifr_flags = (ifr.ifr_flags & (~IFF_UP));
            ioctl(s, SIOCSIFFLAGS, &ifr);
        }
        close(s);
    }
    usleep(200000);

    if (device_id == WIFI_RALINK_RT3070 || device_id == WIFI_RALINK_RT5370
      || device_id == WIFI_RALINK_RT5372 || device_id == WIFI_RALINK_RT5572) {
        goto stopped;
    }

    if (ap_kill_hostapd() < 0) {
        DBGPRINT(("WiFi: Kill hostapd fail\n"));
        return HI_FAILURE;
    }

stopped:
    DBGPRINT(("WiFi: SoftAP stopped\n"));
    return HI_SUCCESS;
}

hi_s32 hi_wlan_ap_set_softap(const hi_char *ifname, hi_wlan_ap_config *ap_cfg)
{
    hi_s32 ret = 0;
    hi_char iface[IFNAMSIZ + 1] = {0};

    if (ifname == NULL || *ifname == '\0' || ap_cfg == NULL)
        return HI_WLAN_INVALID_PARAMETER;

    DBGPRINT(("WiFi: Must stop SoftAP and close WiFi before setting it\n"));
    /* to set AP, we must restart it */
    ret = hi_wlan_ap_stop(ifname);
    if (ret != HI_SUCCESS) {
        DBGPRINT(("WiFi: Stop SoftAP fail\n"));
        return HI_FAILURE;
    }

    ret = hi_wlan_ap_close(ifname);
    if (ret != HI_SUCCESS) {
        DBGPRINT(("WiFi: Close WiFi fail\n"));
        return HI_FAILURE;
    }

    /* wait for driver deinitialization */
    sleep(1);
    DBGPRINT(("WiFi: Closed, then restart it\n"));

    ret = memset_s(iface, sizeof(iface), 0, sizeof(iface));
    if (ret != 0) {
        DBGPRINT(("WiFi: file=%s, func=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__));
    }
    ret = hi_wlan_ap_open(iface, sizeof(iface));
    if (ret != HI_SUCCESS) {
        DBGPRINT(("WiFi: Open SoftAP fail\n"));
        return HI_FAILURE;
    }
    if (strcmp(iface, ifname) != 0) {
        DBGPRINT(("WiFi: Fail, new interface is '%s', expect for '%s'\n",
                      ifname, iface));
        return HI_FAILURE;
    }

    return hi_wlan_ap_start(ifname, ap_cfg);
}

hi_s32 hi_wlan_ap_get_mac(const hi_char *ifname, hi_char *mac, hi_u8 mac_size)
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
