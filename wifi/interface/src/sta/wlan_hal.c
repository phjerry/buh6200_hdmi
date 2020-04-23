#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <spawn.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <poll.h>
#include <pthread.h>

#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>

#include "wlan_hal.h"
#include "wlan_util.h"
#include "securec.h"

static const hi_char WPA_SUPPLICANT[] = "wpa_supplicant";
static const hi_char RTL_SUPPLICANT[] = "rtl_supplicant";

/** struct of global data */
typedef struct hiHI_STA_DATA_S {
    hi_s32 device_id;
    HI_WPA_SOCKET_S ctrl_s;
    pthread_mutex_t ctrl_req_mut;
} HI_STA_DATA_S;

static HI_STA_DATA_S *sta = NULL;
static HI_STA_DATA_S *p2p = NULL;
static hi_s32 count = 0;

hi_s32 wlan_hal_init(hi_void)
{
    hi_s32 results = 0;

    sta = malloc(sizeof(HI_STA_DATA_S));
    if (sta == NULL)
        return HI_FAILURE;

    results = memset_s(sta, sizeof(HI_STA_DATA_S), 0, sizeof(HI_STA_DATA_S));
    if (results < EOK) {
        DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
    }
    sta->device_id = -1;

    if (pthread_mutex_init(&sta->ctrl_req_mut, NULL) != 0) {
        DBGPRINT(("WiFi: 'sta->ctrl_req_mut' pthread_mutex_init fail in %s!\n", __func__));
    }

    p2p = malloc(sizeof(HI_STA_DATA_S));
    if (p2p == NULL)
        return HI_FAILURE;

    results = memset_s(p2p, sizeof(HI_STA_DATA_S), 0, sizeof(HI_STA_DATA_S));
    if (results < EOK) {
        DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
    }
    p2p->device_id = -1;

    if (pthread_mutex_init(&p2p->ctrl_req_mut, NULL) != 0) {
        DBGPRINT(("WiFi: 'p2p->ctrl_req_mut' pthread_mutex_init fail in %s!\n", __func__));
    }

    return HI_SUCCESS;
}

hi_void wlan_hal_deinit(hi_void)
{
    if (sta) {
        if (pthread_mutex_destroy(&sta->ctrl_req_mut) != 0) {
            DBGPRINT(("WiFi: 'sta->ctrl_req_mut' pthread_mutex_destroy fail in %s!\n", __func__));
        }
        free(sta);
        sta = NULL;
    }

    if (p2p) {
        if (pthread_mutex_destroy(&p2p->ctrl_req_mut) != 0) {
            DBGPRINT(("WiFi: 'p2p->ctrl_req_mut' pthread_mutex_destroy fail in %s!\n", __func__));
        }
        free(p2p);
        p2p = NULL;
    }
}

hi_s32 wlan_load_driver(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    /* get device id */
    sta->device_id = wlan_util_get_wifi_device();
    p2p->device_id = sta->device_id;
    if (sta->device_id < 0) {
        DBGPRINT(("WiFi: Cann't find supported device\n"));
        return HI_WLAN_DEVICE_NOT_FOUND;
    }
    DBGPRINT(("WiFi: Find device %d\n", sta->device_id));
    /* insmod driver */
    switch(sta->device_id) {
        case WIFI_RALINK_RT3070:
        case WIFI_RALINK_RT5370:
        case WIFI_RALINK_RT5372:
        case WIFI_RALINK_RT5572:
            if (wlan_util_insmod_module("/kmod/rtutil5572sta.ko", "rtutil5572sta ", NULL)
              || wlan_util_insmod_module("/kmod/rt5572sta.ko", "rt5572sta ", NULL)
              || wlan_util_insmod_module("/kmod/rtnet5572sta.ko", "rtnet5572sta ", NULL))
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
            if (wlan_util_insmod_module("/kmod/cfg80211_ath6k.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/ath6kl_usb.ko", "ath6kl_usb ",
                                           "reg_domain=0x809c")) {
                sleep(2);
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            }
            break;
        case WIFI_REALTEK_RTL8188SU:
        case WIFI_REALTEK_RTL8192SU:
            if (wlan_util_insmod_module("/kmod/rtl8712u.ko", "rtl8712u ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8188CUS:
        case WIFI_REALTEK_RTL8192CU:
            if (wlan_util_insmod_module("/kmod/8192cu.ko", "8192cu ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8188EUS:
        case WIFI_REALTEK_RTL8188ETV:
        case WIFI_REALTEK_RTL8189ES:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8188eu.ko", "rtl8188eu ", "ifname=wlan0 if2name=p2p0"))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8188FU:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8188fu.ko", "rtl8188fu ", "ifname=wlan0 if2name=p2p0"))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_MEDIATEK_MT7632U:
        case WIFI_MEDIATEK_MT7662U:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/mtprealloc76x2Usta.ko", "mtprealloc76x2Usta ", NULL)
              || wlan_util_insmod_module("/kmod/mt7662u_sta.ko", "mt7662u_sta ", NULL))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8811CU:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8811cu.ko", "rtl8811cu ", "ifname=wlan0 if2name=p2p0"))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8822BS:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8822bs.ko", "rtl8822bs ", "ifname=wlan0 if2name=p2p0"))
                ret = HI_WLAN_LOAD_DRIVER_FAIL;
            break;
        case WIFI_REALTEK_RTL8723DU:
            if (wlan_util_insmod_module("/kmod/cfg80211.ko", "cfg80211 ", NULL)
              || wlan_util_insmod_module("/kmod/rtl8723du.ko", "rtl8723du ", "ifname=wlan0 if2name=p2p0"))
                ret = HI_SUCCESS;
            break;
        default:
            DBGPRINT(("WiFi: device %d is not supported, "
                        "cann't load driver\n", sta->device_id));
            ret = HI_WLAN_DEVICE_NOT_FOUND;
            break;
    }

    if (ret == HI_WLAN_LOAD_DRIVER_FAIL)
        DBGPRINT(("WiFi: Load driver fail\n"));

    return ret;
}

hi_s32 wlan_unload_driver(hi_void)
{
    hi_s32 ret = HI_FAILURE;

    DBGPRINT(("WiFi: Unloading driver\n"));
    sta->device_id = wlan_util_get_wifi_device();
    p2p->device_id = sta->device_id;
    /* rmmod driver */
    switch(sta->device_id) {
        case WIFI_RALINK_RT3070:
        case WIFI_RALINK_RT5370:
        case WIFI_RALINK_RT5372:
        case WIFI_RALINK_RT5572:
            if ((wlan_util_rmmod_module("rtnet5572sta") == 0)
              && (wlan_util_rmmod_module("rt5572sta") == 0)
              && (wlan_util_rmmod_module("rtutil5572sta") == 0))
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
            if ((wlan_util_rmmod_module("ath6kl_usb") == 0)
              && (wlan_util_rmmod_module("cfg80211_ath6k") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_REALTEK_RTL8188SU:
        case WIFI_REALTEK_RTL8192SU:
            if (wlan_util_rmmod_module("rtl8712u") == 0)
                ret = HI_SUCCESS;
            break;
        case WIFI_REALTEK_RTL8188CUS:
        case WIFI_REALTEK_RTL8192CU:
            if (wlan_util_rmmod_module("8192cu") == 0)
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
        case WIFI_MEDIATEK_MT7632U:
        case WIFI_MEDIATEK_MT7662U:
            if ((wlan_util_rmmod_module("mt7662u_sta") == 0)
              && (wlan_util_rmmod_module("cfg80211") == 0))
                ret = HI_SUCCESS;
            break;
        case WIFI_REALTEK_RTL8811CU:
            if ((wlan_util_rmmod_module("rtl8811cu") == 0)
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
                        "cann't unload driver\n", sta->device_id));
            break;
    }

    if (ret == HI_SUCCESS)
        p2p->device_id = sta->device_id = -1;

    return ret;
}

hi_s32 wlan_start_supplicant(HI_WIFI_MODE_E mode, const hi_char *sta_ifname, const hi_char *p2p_ifname, const hi_char *driver,
                            const hi_char *sta_config_file, const hi_char *p2p_config_file)
{
    hi_s32 ret;
    hi_s32 results = 0;
    hi_s32 pid = 0;
    hi_s32 status = 0;
//    hi_char cmd[256];
    hi_char param[9][128] = { {0} };
    hi_char *environment_path = NULL;
    hi_s32 num = 0;
    hi_s32 i = 0;
    hi_char *spawn_env[] = {NULL};
    hi_char *spawn_args[] = {NULL, NULL, NULL, NULL,
                             NULL, NULL, NULL, NULL,
                             NULL, NULL};
    switch(sta->device_id) {
        case WIFI_REALTEK_RTL8188EUS:
        case WIFI_REALTEK_RTL8188ETV:
        case WIFI_REALTEK_RTL8189ES:
        case WIFI_REALTEK_RTL8811CU:
        case WIFI_REALTEK_RTL8188FU:
        case WIFI_REALTEK_RTL8822BS:
        case WIFI_REALTEK_RTL8723DU:
            results = sprintf_s(param[0], sizeof(param[0]), "%s", RTL_SUPPLICANT);
            if (results < EOK) {
                DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
            }
        break;
        default:
            results = sprintf_s(param[0], sizeof(param[0]), "%s", WPA_SUPPLICANT);
            if (results < EOK) {
                DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
            }
        break;
    }
    if (mode == WIFI_MODE_STA) {
        results = sprintf_s(param[1], sizeof(param[1]), "-i%s", sta_ifname);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        results = sprintf_s(param[2], sizeof(param[2]), "-D%s", driver);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        results = sprintf_s(param[3], sizeof(param[3]), "-c%s", sta_config_file);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        num = 4;
    } else if (mode == WIFI_MODE_P2P) {
        results = sprintf_s(param[1], sizeof(param[1]), "-i%s", p2p_ifname);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        results = sprintf_s(param[2], sizeof(param[2]), "-D%s", driver);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        results = sprintf_s(param[3], sizeof(param[3]), "-c%s", p2p_config_file);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        num = 4;
    } else {
        results = sprintf_s(param[1], sizeof(param[1]), "-i%s", p2p_ifname);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        results = sprintf_s(param[2], sizeof(param[2]), "-D%s", driver);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        results = sprintf_s(param[3], sizeof(param[3]), "-c%s", p2p_config_file);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        results = sprintf_s(param[4], sizeof(param[4]), "-N");
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        results = sprintf_s(param[5], sizeof(param[5]), "-i%s", sta_ifname);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        results = sprintf_s(param[6], sizeof(param[6]), "-D%s", driver);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        results = sprintf_s(param[7], sizeof(param[7]), "-c%s", sta_config_file);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        num = 8;
    }
    environment_path = getenv("WPA_SUPPLICANT_DEBUG");
    if (environment_path == NULL) {
        DBGPRINT(("WiFi: getenv 'WPA_SUPPLICANT_DEBUG'no message!\n"));
        results = sprintf_s(param[num], sizeof(param[num]), "-B");
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
    } else {
        DBGPRINT(("WiFi: getenv WPA_SUPPLICANT_DEBUG=%s\n", environment_path));
        results = sprintf_s(param[num], sizeof(param[num]), "-f%s -t -ddd&", environment_path);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
    }

    for (i=0; i<=num; i++) {
        spawn_args[i] = param[i];
    }
#if 0
    if (mode == WIFI_MODE_STA_P2P) {
        results = sprintf_s(cmd, sizeof(cmd), "%s %s %s %s %s %s %s %s %s", spawn_args[0], spawn_args[1], spawn_args[2], spawn_args[3],
            spawn_args[4], spawn_args[5], spawn_args[6], spawn_args[7], spawn_args[8]);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
    } else {
        results = sprintf_s(cmd, sizeof(cmd), "%s %s %s %s %s", spawn_args[0], spawn_args[1], spawn_args[2], spawn_args[3], spawn_args[4]);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
    }
    DBGPRINT(("WiFi: system cmd = '%s'\n", cmd));
    ret = system(cmd);
#endif
    ret = posix_spawnp(&pid, spawn_args[0], NULL, NULL,
                   spawn_args, spawn_env);
    if (ret != 0) {
        return HI_FAILURE;
    }

    ret = waitpid(pid, &status, 0);
    if (ret == -1) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 wlan_stop_supplicant(hi_void)
{
    hi_s32 ret;
    hi_s32 results = 0;
    hi_s32 pid = 0;
    hi_s32 status = 0;
//    hi_char cmd[256] = { 0 };
    hi_char *spawn_env[] = {NULL};
    hi_char param[128] = { 0 };
    hi_char *spawn_args[] = {"killall", NULL, NULL};

    /* get device id */
    sta->device_id = wlan_util_get_wifi_device();
    p2p->device_id = sta->device_id;
    switch(sta->device_id) {
        case WIFI_REALTEK_RTL8188EUS:
        case WIFI_REALTEK_RTL8188ETV:
        case WIFI_REALTEK_RTL8189ES:
        case WIFI_REALTEK_RTL8811CU:
        case WIFI_REALTEK_RTL8188FU:
        case WIFI_REALTEK_RTL8822BS:
        case WIFI_REALTEK_RTL8723DU:
            results = sprintf_s(param, sizeof(param), "%s", RTL_SUPPLICANT);
            if (results < EOK) {
                DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
            }
        break;
        default:
            results = sprintf_s(param, sizeof(param), "%s", WPA_SUPPLICANT);
            if (results < EOK) {
                DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
            }
        break;
    }

    spawn_args[1] = param;
#if 0
    results = sprintf_s(cmd, sizeof(cmd), "%s %s", spawn_args[0], spawn_args[1]);
    if (results < EOK) {
        DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
    }
    DBGPRINT(("WiFi: system cmd = '%s'\n", cmd));
    ret = system(cmd);
#endif
    ret = posix_spawnp(&pid, spawn_args[0], NULL, NULL,
                   spawn_args, spawn_env);
    if (ret != 0) {
        return HI_FAILURE;
    }

    ret = waitpid(pid, &status, 0);
    if (ret == -1) {
        perror("waitpid");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static
hi_s32 wlan_wpa_send_command(HI_WPA_SOCKET_S *wpa_s, const hi_char *cmd,
                          hi_char *reply, hi_s32 *reply_len)
{
    hi_s32 ret, i = 0;

    if (wpa_s == NULL || cmd == NULL)
        return HI_FAILURE;

    //pthread_mutex_lock(&sta->ctrl_req_mut);
    /* Send command to wpa_supplicant, if failed, try 50 times */
send_again:
    ret = send(wpa_s->s, cmd, strlen(cmd), 0);
    if (ret < 0) {
        if (errno == EAGAIN || errno == EBUSY || errno == EWOULDBLOCK) {
            i++;
            if (i >= 50) {
                ret = HI_FAILURE;
                goto exit;
            }

            usleep(100000);
            goto send_again;
        } else {
            ret = HI_FAILURE;
            goto exit;
        }
    }

    if (reply == NULL || reply_len == NULL) {
        ret = HI_SUCCESS;
        goto exit;
    }

    /* Receive command's reply */
    for (i = 0; i < 100; i++) {
        fd_set rfd;
        struct timeval tv;

        FD_ZERO(&rfd);
        FD_SET(wpa_s->s, &rfd);
        tv.tv_sec = 10;
        tv.tv_usec = 0;

        ret = select(wpa_s->s + 1, &rfd, NULL, NULL, &tv);
        if (ret < 0) {
            ret = HI_FAILURE;
            goto exit;
        }

        if (FD_ISSET(wpa_s->s, &rfd)) {
            ret = recv(wpa_s->s, reply, *reply_len, 0);
            if (ret < 0) {
                ret = HI_FAILURE;
                goto exit;
            }

            if (ret > 0 && reply[0] == '<')
                continue;

            *reply_len = ret;
            break;
        } else {
            ret = HI_FAILURE;
            goto exit;
        }
    }

    ret = HI_SUCCESS;
exit:
    //pthread_mutex_unlock(&sta->ctrl_req_mut);
    return ret;
}

hi_s32 wlan_wpa_request(hi_char *cmd, hi_char *cbuf, hi_s32 *size)
{
    hi_char reply[STRING_REPLY_SIZE] = {0};
    hi_s32 reply_len = 0;
    hi_s32 ret = 0;

    if (cmd == NULL)
        return HI_FAILURE;

    reply_len = sizeof(reply);
    ret = wlan_wpa_send_command(&sta->ctrl_s, cmd, reply, &reply_len);
    if (ret == HI_FAILURE || strncmp(reply, "FAIL", 4) == 0) {
        DBGPRINT(("WiFi: '%s' command fail!\n", cmd));
        return HI_FAILURE;
    }

    if (cbuf != NULL && size != NULL) {
        if (*size < reply_len)
            reply_len = *size;

        ret = strncpy_s(cbuf, *size, reply, reply_len);
        if (ret < 0) {
            DBGPRINT(("WiFi: ret=%d file=%s, func=%s, line=%d\n", ret, __FILE__, __FUNCTION__, __LINE__));
        }
        *size = reply_len;
    }

    return HI_SUCCESS;
}

hi_s32 wlan_wpa_read(HI_WPA_SOCKET_S *wpa_s, hi_char *event, hi_s32 *size)
{
    hi_s32 ret = -1;
    hi_s32 results = 0;
    struct pollfd rfds[1];

    if (wpa_s == NULL) {
        return -1;
    }

    if (event == NULL || size == NULL)
        return -1;

    results = memset_s(rfds, sizeof(struct pollfd), 0, 1 * sizeof(struct pollfd));
    if (results < 0) {
        DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
    }
    rfds[0].fd = wpa_s->s;
    rfds[0].events |= POLLIN;

    ret = poll(rfds, 1, -1);
    if (ret < 0) {
        return ret;
    }

    if (rfds[0].revents & POLLIN) {
        hi_char buf[256] = {0};
        size_t len = sizeof(buf) - 1;

        ret = recv(wpa_s->s, buf, len, 0);
        if (ret >= 0) {
            len = ret;
            buf[len] = '\0';
            if (len+1 > *size) {
                if (memcpy_s(event, *size, buf, *size) != 0) {
                    DBGPRINT(("WiFi: file=%s, func=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__));
                }
            } else {
                if (memcpy_s(event, *size, buf, len+1) != 0) {
                    DBGPRINT(("WiFi: file=%s, func=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__));
                }
                *size = len+1;
            }
        } else {
            return -1;
        }
    }
    return 0;
}

HI_WPA_SOCKET_S *wlan_wpa_open_connection(const hi_char *ifname,
                     const hi_char *ctrl_iface_dir)
{
    hi_char *cfile = NULL;
    hi_s32 flen = 0;
    hi_s32 ret = 0;
    HI_WPA_SOCKET_S *mon_s = NULL;
    hi_s32 flags = 0;
    hi_char reply[STRING_REPLY_SIZE] = {0};
    hi_s32 reply_len = sizeof(reply);

    if (ifname == NULL || ctrl_iface_dir == NULL)
        return NULL;

    if (access(ctrl_iface_dir, F_OK) < 0) {
        return NULL;
    }

    flen = strlen(ctrl_iface_dir) + strlen(ifname) + 2;
    cfile = malloc(flen);
    if (cfile == NULL)
        return NULL;
    ret = snprintf_s(cfile, flen, flen, "%s/%s", ctrl_iface_dir, ifname);
    if (ret < 0 || ret >= flen) {
        free(cfile);
        return NULL;
    }

    /* Open control socket to send command to wpa_supplicant,
     * only open once. */
    if (sta->ctrl_s.s == 0) {
        /* Open socket to send command to wpa_supplicant */
        sta->ctrl_s.s = socket(PF_UNIX, SOCK_DGRAM, 0);
        if (sta->ctrl_s.s < 0)
            goto fail;

        sta->ctrl_s.local.sun_family = AF_UNIX;
        ret = snprintf_s(sta->ctrl_s.local.sun_path, sizeof(sta->ctrl_s.local.sun_path),
                            sizeof(sta->ctrl_s.local.sun_path), "%s/wpa_%d_%d",
                            ctrl_iface_dir, (int) getpid(), count++);
        if (ret < 0 || ret >= sizeof(sta->ctrl_s.local.sun_path))
            goto fail;

        if (bind(sta->ctrl_s.s, (struct sockaddr *) &sta->ctrl_s.local,
                    sizeof(sta->ctrl_s.local)) < 0)
            goto fail;

        sta->ctrl_s.remote.sun_family = AF_UNIX;
        ret = strncpy_s(sta->ctrl_s.remote.sun_path, sizeof(sta->ctrl_s.remote.sun_path),
                        cfile, strlen(cfile)+1);
        if (ret < 0) {
            DBGPRINT(("WiFi: ret=%d file=%s, func=%s, line=%d\n", ret, __FILE__, __FUNCTION__, __LINE__));
        }
        if (connect(sta->ctrl_s.s, (struct sockaddr *) &sta->ctrl_s.remote,
                      sizeof(sta->ctrl_s.remote)) < 0)
            goto fail;

        flags = fcntl(sta->ctrl_s.s, F_GETFL);
        if (flags >= 0) {
            flags = (hi_u32)flags | O_NONBLOCK;
            if (fcntl(sta->ctrl_s.s, F_SETFL, flags) < 0) {
                DBGPRINT(("WiFi: fcntl fail\n"));
            }
        }
    }

    /* Open monitor socket to receive wpa_supplicant's event */
    mon_s = (HI_WPA_SOCKET_S *)malloc(sizeof(HI_WPA_SOCKET_S));
    if (mon_s == NULL)
        goto fail;

    mon_s->s = socket(PF_UNIX, SOCK_DGRAM, 0);
    if (mon_s->s < 0)
        goto fail;

    mon_s->local.sun_family = AF_UNIX;
    ret = snprintf_s(mon_s->local.sun_path, sizeof(mon_s->local.sun_path), sizeof(mon_s->local.sun_path),
                     "%s/wpa_%d_%d", ctrl_iface_dir, (int)getpid(), count++);
    if (ret < 0 || ret >= sizeof(mon_s->local.sun_path))
        goto fail;

    if (bind(mon_s->s, (struct sockaddr *) &mon_s->local,
                  sizeof(mon_s->local)) < 0)
        goto fail;

    mon_s->remote.sun_family = AF_UNIX;
    ret = strncpy_s(mon_s->remote.sun_path, sizeof(mon_s->remote.sun_path),
                cfile, strlen(cfile)+1);
    if (ret != 0) {
        DBGPRINT(("WiFi: file=%s, func=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__));
    }
    if (connect(mon_s->s, (struct sockaddr *) &mon_s->remote,
                  sizeof(mon_s->remote)) < 0)
        goto fail;

    flags = fcntl(mon_s->s, F_GETFL);
    if (flags >= 0) {
        flags = (hi_u32)flags | O_NONBLOCK;
        if (fcntl(mon_s->s, F_SETFL, flags) < 0) {
            DBGPRINT(("WiFi: fcntl fail\n"));
        }
    }

    /* Attach mon_s socket to wpa_supplicant */
    ret = wlan_wpa_send_command(mon_s, "ATTACH", reply, &reply_len);
    if (ret == HI_FAILURE || strncmp(reply, "OK", 2) != 0) {
        DBGPRINT(("WiFi: Attach to wpa_supplicant fail\n"));
        goto fail;
    }

    free(cfile);

    return mon_s;
fail:
    if (cfile)
        free(cfile);

    unlink(sta->ctrl_s.local.sun_path);
    if (sta->ctrl_s.s >= 0)
        close(sta->ctrl_s.s);

    ret = memset_s(&sta->ctrl_s, sizeof(HI_WPA_SOCKET_S), 0, sizeof(HI_WPA_SOCKET_S));
    if (ret != 0) {
        DBGPRINT(("WiFi: ret=%d file=%s, func=%s, line=%d\n", ret, __FILE__, __FUNCTION__, __LINE__));
    }

    if (mon_s) {
        unlink(mon_s->local.sun_path);
        if (mon_s->s >= 0)
            close(mon_s->s);

        free(mon_s);
    }

    return NULL;
}

hi_void wlan_wpa_close_connection(hi_s32 control, HI_WPA_SOCKET_S *wpa_s)
{
    /* close control socket */
    if (control && sta->ctrl_s.s > 0) {
        unlink(sta->ctrl_s.local.sun_path);
        close(sta->ctrl_s.s);
        memset_s(&sta->ctrl_s, sizeof(HI_WPA_SOCKET_S), 0, sizeof(HI_WPA_SOCKET_S));
    }

    if (wpa_s) {
        if (wlan_wpa_send_command(wpa_s, "DETACH", NULL, NULL) == HI_FAILURE) {
            DBGPRINT(("WiFi: 'DETACH' wlan_wpa_send_command fail in %s!\n", __func__));
        }

        unlink(wpa_s->local.sun_path);
        if (wpa_s->s > 0)
            close(wpa_s->s);
        free(wpa_s);
    }
}

HI_WPA_SOCKET_S *wlan_wpa_open_p2p_connection(const hi_char *ifname,
                     const hi_char *ctrl_iface_dir)
{
    hi_char *cfile = NULL;
    hi_s32 flen, ret;
    HI_WPA_SOCKET_S *mon_s = NULL;
    hi_s32 flags;
    hi_char reply[STRING_REPLY_SIZE] = {0};
    hi_s32 reply_len = sizeof(reply);
    hi_s32 results = 0;

    if (ifname == NULL || ctrl_iface_dir == NULL)
        return NULL;

    if (access(ctrl_iface_dir, F_OK) < 0) {
        return NULL;
    }

    flen = strlen(ctrl_iface_dir) + strlen(ifname) + 2;
    cfile = malloc(flen);
    if (cfile == NULL)
        return NULL;
    ret = sprintf_s(cfile, flen, "%s/%s", ctrl_iface_dir, ifname);
    if (ret < 0 || ret >= flen) {
        free(cfile);
        return NULL;
    }

    /* Open control socket to send command to wpa_supplicant,
     * only open once. */
    if (p2p->ctrl_s.s == 0) {
        /* Open socket to send command to wpa_supplicant */
        p2p->ctrl_s.s = socket(PF_UNIX, SOCK_DGRAM, 0);
        if (p2p->ctrl_s.s < 0)
            goto fail;

        p2p->ctrl_s.local.sun_family = AF_UNIX;
        ret = sprintf_s(p2p->ctrl_s.local.sun_path, sizeof(p2p->ctrl_s.local.sun_path),
                         "%s/wpa_%d_%d", ctrl_iface_dir, (int) getpid(), count++);
        if (ret < 0 || ret >= sizeof(p2p->ctrl_s.local.sun_path))
            goto fail;

        if (bind(p2p->ctrl_s.s, (struct sockaddr *) &p2p->ctrl_s.local,
                    sizeof(p2p->ctrl_s.local)) < 0)
            goto fail;

        p2p->ctrl_s.remote.sun_family = AF_UNIX;
        ret = strcpy_s(p2p->ctrl_s.remote.sun_path, sizeof(p2p->ctrl_s.remote.sun_path), cfile);
        if (ret != EOK) {
            DBGPRINT(("WiFi: ret=%d file=%s, line=%d, func=%s\n", ret, __FILE__, __LINE__, __FUNCTION__));
        }
        if (connect(p2p->ctrl_s.s, (struct sockaddr *) &(p2p)->ctrl_s.remote,
                      sizeof(p2p->ctrl_s.remote)) < 0)
            goto fail;

        flags = fcntl(p2p->ctrl_s.s, F_GETFL);
        if (flags >= 0) {
            flags = (hi_u32)flags | O_NONBLOCK;
            if (fcntl(p2p->ctrl_s.s, F_SETFL, flags) < 0) {
                DBGPRINT(("WiFi: fcntl fail\n"));
            }
        }
    }

    /* Open monitor socket to receive wpa_supplicant's event */
    mon_s = (HI_WPA_SOCKET_S *)malloc(sizeof(HI_WPA_SOCKET_S));
    if (mon_s == NULL)
        goto fail;

    mon_s->s = socket(PF_UNIX, SOCK_DGRAM, 0);
    if (mon_s->s < 0)
        goto fail;

    mon_s->local.sun_family = AF_UNIX;
    ret = sprintf_s(mon_s->local.sun_path, sizeof(mon_s->local.sun_path),
                     "%s/wpa_%d_%d", ctrl_iface_dir, (int) getpid(), count++);
    if (ret < 0 || ret >= sizeof(mon_s->local.sun_path))
        goto fail;

    if (bind(mon_s->s, (struct sockaddr *) &mon_s->local,
                  sizeof(mon_s->local)) < 0)
        goto fail;

    mon_s->remote.sun_family = AF_UNIX;
    ret = strcpy_s(mon_s->remote.sun_path, sizeof(mon_s->remote.sun_path), cfile);
    if (ret != EOK) {
        DBGPRINT(("WiFi: ret=%d file=%s, line=%d, func=%s\n", ret, __FILE__, __LINE__, __FUNCTION__));
    }

    if (connect(mon_s->s, (struct sockaddr *) &mon_s->remote,
                  sizeof(mon_s->remote)) < 0)
        goto fail;

    flags = fcntl(mon_s->s, F_GETFL);
    if (flags >= 0) {
        flags = (hi_u32)flags | O_NONBLOCK;
        if (fcntl(mon_s->s, F_SETFL, flags) < 0) {
            DBGPRINT(("WiFi: fcntl fail\n"));
        }
    }

    /* Attach mon_s socket to wpa_supplicant */
    ret = wlan_wpa_send_command(mon_s, "ATTACH", reply, &reply_len);
    if (ret == HI_FAILURE || strncmp(reply, "OK", 2) != 0) {
        DBGPRINT(("WiFi: Attach to wpa_supplicant fail\n"));
        goto fail;
    }

    free(cfile);

    return mon_s;
fail:
    if (cfile)
        free(cfile);

    unlink(p2p->ctrl_s.local.sun_path);
    if (p2p->ctrl_s.s >= 0)
        close(p2p->ctrl_s.s);

    results = memset_s(&p2p->ctrl_s, sizeof(HI_WPA_SOCKET_S), 0, sizeof(HI_WPA_SOCKET_S));
    if (results < EOK) {
        DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
    }

    if (mon_s) {
        unlink(mon_s->local.sun_path);
        if (mon_s->s >= 0)
            close(mon_s->s);

        free(mon_s);
    }

    return NULL;
}

hi_void wlan_wpa_close_p2p_connection(hi_s32 control, HI_WPA_SOCKET_S *wpa_s)
{
    hi_s32 results = 0;

    /* close control socket */
    if (control && p2p->ctrl_s.s > 0) {
        unlink(p2p->ctrl_s.local.sun_path);
        close(p2p->ctrl_s.s);
        results = memset_s(&p2p->ctrl_s, sizeof(HI_WPA_SOCKET_S), 0, sizeof(HI_WPA_SOCKET_S));
        if (results != EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, line=%d, func=%s\n", results, __FILE__, __LINE__, __FUNCTION__));
        }
    }

    if (wpa_s) {
        if (wlan_wpa_send_command(wpa_s, "DETACH", NULL, NULL) == HI_FAILURE) {
            DBGPRINT(("WiFi: 'DETACH' wlan_wpa_send_command fail in %s!\n", __func__));
        }

        unlink(wpa_s->local.sun_path);
        if (wpa_s->s > 0)
            close(wpa_s->s);
        free(wpa_s);
    }
}


hi_s32 wlan_wpa_request_p2p(const hi_char *cmd, hi_u32 cmd_size, hi_char *cbuf, hi_u32 *size)
{
    hi_char reply[STRING_REPLY_SIZE] = {0};
    hi_s32 reply_len;
    hi_s32 ret;
    hi_s32 results = 0;

    if (cmd == NULL)
        return HI_FAILURE;

    reply_len = sizeof(reply);
    ret = wlan_wpa_send_command(&p2p->ctrl_s, cmd, reply, &reply_len);
    if (ret == HI_FAILURE || strncmp(reply, "FAIL", 4) == 0) {
        DBGPRINT(("WiFi: '%s' command fail!\n", cmd));
        return HI_FAILURE;
    }

    if (cbuf != NULL && size != NULL) {
        if (*size < reply_len)
            reply_len = *size;

        results = strncpy_s(cbuf, *size, reply, reply_len);
        if (results < EOK) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }
        *size = reply_len;
    }

    return HI_SUCCESS;
}
