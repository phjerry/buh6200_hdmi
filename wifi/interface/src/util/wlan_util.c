#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <spawn.h>
#include <sys/wait.h>
#include <dirent.h>

#include "wlan_util.h"
#include "securec.h"

#define DRIVER_MODULE_LEN_MAX    256

static const hi_char USB_DIR[] = "/sys/bus/usb/devices";
static const hi_char SDIO_DIR[] = "/sys/bus/sdio/devices";
static const hi_char MODULE_FILE[] = "/proc/modules";

static hi_wifi_device_info dev[] = {
    {WIFI_ID_RALINK_RT3070, WIFI_RALINK_RT3070},
    {WIFI_ID_RALINK_RT5370, WIFI_RALINK_RT5370},
    {WIFI_ID_RALINK_RT5372, WIFI_RALINK_RT5372},
    {WIFI_ID_RALINK_RT5572, WIFI_RALINK_RT5572},
    {WIFI_ID_RALINK_MT7601U, WIFI_RALINK_MT7601U},
    {WIFI_ID_ATHEROS_AR9271, WIFI_ATHEROS_AR9271},
    {WIFI_ID_ATHEROS_AR9374, WIFI_ATHEROS_AR9374},
    {WIFI_ID_REALTEK_RTL8188SU, WIFI_REALTEK_RTL8188SU},
    {WIFI_ID_REALTEK_RTL8192SU, WIFI_REALTEK_RTL8192SU},
    {WIFI_ID_REALTEK_RTL8188CUS, WIFI_REALTEK_RTL8188CUS},
    {WIFI_ID_REALTEK_RTL8192CU, WIFI_REALTEK_RTL8192CU},
    {WIFI_ID_REALTEK_RTL8188EUS, WIFI_REALTEK_RTL8188EUS},
    {WIFI_ID_REALTEK_RTL8188ETV, WIFI_REALTEK_RTL8188ETV},
    {WIFI_ID_REALTEK_RTL8192DU_VC, WIFI_REALTEK_RTL8192DU},
    {WIFI_ID_REALTEK_RTL8192DU_VS, WIFI_REALTEK_RTL8192DU},
    {WIFI_ID_REALTEK_RTL8192EU, WIFI_REALTEK_RTL8192EU},
    {WIFI_ID_REALTEK_RTL8812AU, WIFI_REALTEK_RTL8812AU},
    {WIFI_ID_REALTEK_RTL8812AU_VS, WIFI_REALTEK_RTL8812AU},
    {WIFI_ID_ATHEROS_QCA1021G, WIFI_ATHEROS_QCA1021G},
    {WIFI_ID_ATHEROS_QCA1021X, WIFI_ATHEROS_QCA1021X},
    {WIFI_ID_BROADCOM_BCM43236, WIFI_BROADCOM_BCM43236},
    {WIFI_ID_REALTEK_RTL8723BU, WIFI_REALTEK_RTL8723BU},
    {WIFI_ID_MEDIATEK_MT7632U, WIFI_MEDIATEK_MT7632U},
    {WIFI_ID_MEDIATEK_MT7662U, WIFI_MEDIATEK_MT7662U},
    {WIFI_ID_REALTEK_RTL8811CU, WIFI_REALTEK_RTL8811CU},
    {WIFI_ID_REALTEK_RTL8188FU, WIFI_REALTEK_RTL8188FU},
    {WIFI_ID_REALTEK_RTL8822BS, WIFI_REALTEK_RTL8822BS},
    {WIFI_ID_REALTEK_RTL8723DU, WIFI_REALTEK_RTL8723DU}
};

hi_s32 wlan_util_get_sdio_wifi_device(hi_void)
{
    hi_s32 ret = -1;
    hi_s32 results = 0;
    DIR *dir = NULL;
    struct dirent *next = NULL;
    FILE *fp = NULL;
    char file_path[PATH_MAX+1] = {0};

    dir = opendir(SDIO_DIR);
    if (!dir) {
        return -1;
    }

    while ((next = readdir(dir)) != NULL) {
        hi_char line[MAX_LEN_OF_LINE];
        hi_char uevent_file[256] = {0};

        /* read uevent file, uevent's data like below:
         *DRIVER=oal_sdio
         *SDIO_CLASS=07
         *SDIO_ID=0296:5347
         *MODALIAS=sdio:c07v0296d5347
         */
        results = snprintf_s(uevent_file, sizeof(uevent_file), sizeof(uevent_file),
                        "%s/%s/uevent", SDIO_DIR, next->d_name);
        if (results < 0) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }

        if (realpath(uevent_file, file_path) == NULL) {
            DBGPRINT(("WiFi: file path '%s' no exist [%s]!\n", file_path, __func__));
            continue;
        }
        fp = fopen(file_path, "r");
        if (fp == NULL) {
            continue;
        }

        while (fgets(line, sizeof(line), fp)) {
            hi_char *pos = NULL;
            hi_s32 product_vid;
            hi_s32 product_did;
            hi_char device_id[10] = {0};

            pos = strstr(line, "SDIO_ID=");
            if(pos != NULL) {
                hi_s32 i;
                results = sscanf_s(pos + 8, "%x:%x", &product_vid, &product_did);
                if (results <= 0) {
                    DBGPRINT(("WiFi: file=%s, func=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__));
                    continue;
                }

                results = snprintf_s(device_id, sizeof(device_id), sizeof(device_id), "%04x:%04x", product_vid, product_did);
                if (results == -1) {
                    DBGPRINT(("WiFi: file=%s, func=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__));
                }

                for (i = 0; i < sizeof(dev)/sizeof(hi_wifi_device_info); i++) {
                    if (strncmp(device_id, dev[i].usb_id, 9) == 0) {
                        ret = dev[i].id;
                        break;
                    }
                }
            }
            if (ret != -1)
                break;
        }
        fclose(fp);
        if (ret != -1)
            break;
    }

    closedir(dir);

    return ret;
}

hi_s32 wlan_util_get_usb_wifi_device(hi_void)
{
    hi_s32 ret = -1;
    hi_s32 results = 0;
    DIR *dir = NULL;
    struct dirent *next = NULL;
    FILE *fp = NULL;
    char file_path[PATH_MAX+1] = {0};

    dir = opendir(USB_DIR);
    if (!dir) {
        return -1;
    }

    while ((next = readdir(dir)) != NULL) {
        hi_char line[MAX_LEN_OF_LINE] = {0};
        hi_char uevent_file[256] = {0};

        /* read uevent file, uevent's data like below:
         * MAJOR=189
         * MINOR=4
         * DEVNAME=bus/usb/001/005
         * DEVTYPE=usb_device
         * DRIVER=usb
         * DEVICE=/proc/bus/usb/001/005
         * PRODUCT=bda/8176/200
         * TYPE=0/0/0
         * BUSNUM=001
         * DEVNUM=005
         */
        results = snprintf_s(uevent_file, sizeof(uevent_file), sizeof(uevent_file),
                        "%s/%s/uevent", USB_DIR, next->d_name);
        if (results <= 0) {
            DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
        }

        if (realpath(uevent_file, file_path) == NULL) {
            DBGPRINT(("WiFi: file path '%s' no exist [%s]!\n", file_path, __func__));
            continue;
        }
        fp = fopen(file_path, "r");
        if (fp == NULL) {
            continue;
        }

        while (fgets(line, sizeof(line), fp)) {
            hi_char *pos = NULL;
            hi_s32 product_vid;
            hi_s32 product_did;
            hi_s32 product_bcddev;
            hi_char device_id[10] = {0};

            pos = strstr(line, "PRODUCT=");
            if(pos != NULL) {
                hi_s32 i;

                results = sscanf_s(pos + 8, "%x/%x/%x", &product_vid, &product_did, &product_bcddev);
                if (results <= 0) {
                    DBGPRINT(("WiFi: file=%s, func=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__));
                    continue;
                }
                results = snprintf_s(device_id, sizeof(device_id), sizeof(device_id),
                                "%04x:%04x", product_vid, product_did);
                if (results < 0) {
                    DBGPRINT(("WiFi: results=%d file=%s, func=%s, line=%d\n", results, __FILE__, __FUNCTION__, __LINE__));
                }
                for (i = 0; i < sizeof(dev)/sizeof(hi_wifi_device_info); i++) {
                    if(strcmp(device_id, dev[i].usb_id) == 0) {
                        ret = dev[i].id;
                        break;
                    }
                }
            }
            if (ret != -1)
                break;
        }
        fclose(fp);
        if (ret != -1)
            break;
    }

    closedir(dir);

    return ret;
}

hi_s32 wlan_util_get_wifi_device(hi_void)
{
    hi_s32 ret = -1;

    ret = wlan_util_get_usb_wifi_device();
    if (-1 != ret)
        return ret;

    ret = wlan_util_get_sdio_wifi_device();

#ifdef WIFI_DEVICE_RTL8822BS
        DBGPRINT(("No supported usb or pcie device found, set id to chosen RTL8822BS\n"));
        ret = WIFI_REALTEK_RTL8822BS;
#endif

    return ret;
}

hi_s32 wlan_util_insmod_module(hi_char *module, hi_char *module_tag,
                                 hi_char *param)
{
    hi_s32 ret = 0;
//    hi_s32 pid = 0;
//    hi_s32 status = 0;
    hi_char cmd[256] = {0};
//    hi_char *spawn_env[] = {NULL};
    hi_char *spawn_args[] = {"insmod", module, NULL, NULL};
    FILE *proc = NULL;
    hi_char line[DRIVER_MODULE_LEN_MAX+10] = {0};

    /* if module is loaded, return ok */
    if ((proc = fopen(MODULE_FILE, "r")) == NULL) {
        DBGPRINT(("Could not open %s\n", MODULE_FILE));
        return -1;
    }

    while ((fgets(line, sizeof(line), proc)) != NULL) {
        if (strncmp(line, module_tag, strlen(module_tag)) == 0) {
            fclose(proc);
            return 0;
        }
    }

    fclose(proc);

    /* insmod module */
    if (param != NULL) {
        spawn_args[2] = param;
    } else {
        spawn_args[2] = "\0";
    }
    ret = snprintf_s(cmd, sizeof(cmd), sizeof(cmd), "%s %s %s", spawn_args[0], spawn_args[1], spawn_args[2]);
    if (ret == -1) {
        DBGPRINT(("WiFi: file=%s, func=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__));
    }
    DBGPRINT(("WiFi: system cmd = '%s'\n", cmd));
    ret = system(cmd);
#if 0
    ret = posix_spawnp(&pid, spawn_args[0], NULL, NULL,
                   spawn_args, spawn_env);
    if (ret != 0) {
        return -1;
    }

    ret = waitpid(pid, &status, 0);
    if (ret == -1) {
        return -1;
    }
#endif

    return ret;
}

hi_s32 wlan_util_rmmod_module(hi_char *module)
{
    hi_s32 ret = 0;
    hi_s32 pid = 0;
    hi_s32 status = 0;
//    hi_char cmd[256] = {0};
    hi_char *spawn_env[] = {NULL};
    hi_char *spawn_args[] = {"rmmod", module, NULL, NULL};

    /* rmmod module */
#if 0

    ret = snprintf_s(cmd, sizeof(cmd), sizeof(cmd), "%s %s", spawn_args[0], spawn_args[1]);
    if (ret == -1) {
        DBGPRINT(("WiFi: file=%s, func=%s, line=%d\n", __FILE__, __FUNCTION__, __LINE__));
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

    return HI_SUCCESS;
}

hi_s32 wlan_util_get_interface(HI_WIFI_MODE_E mode, const hi_char *dev_file,
                                 hi_char *ifname, hi_u32 nameBufSize)
{
    hi_s32 ret = HI_FAILURE;
    hi_char buff[1024] = {0};
    FILE *fh = NULL;
    hi_char *begin = NULL;
    hi_char *end = NULL;
    hi_char if_prefix[10] = {0};
    hi_s32 len = 0;
    char file[PATH_MAX] = {0};
    hi_s32 results = HI_FAILURE;

    if (ifname == NULL || dev_file == NULL)
        return HI_FAILURE;

    /* STA and AP mode - 'wlan0', P2P mode - 'p2p0' */
    if (mode == WIFI_MODE_P2P) {
        results = strncpy_s(if_prefix, sizeof(if_prefix), "p2p", 4);
        if (results != 0) {
            DBGPRINT(("WiFi: snprintf_s return %d\n", results));
        }
    } else {
        results = strncpy_s(if_prefix, sizeof(if_prefix), "wlan", 5);
        if (results != 0) {
            DBGPRINT(("WiFi: snprintf_s return %d\n", results));
        }
    }
    len = strlen(if_prefix);

    if (NULL == realpath(dev_file, file)) {
        DBGPRINT(("WiFi: file path '%s' not exist!\n", file));
        return HI_FAILURE;
    }

    fh = fopen(file, "r");
    if(fh != NULL) {
        /* Eat 2 lines of header */
        if (fgets(buff, sizeof(buff), fh) == NULL) {
            if (fclose(fh) != 0 ) {
                DBGPRINT(("WiFi: %s close fail in first fgets!\n", file));
            }
            fh = NULL;
            return ret;
        }
        if (fgets(buff, sizeof(buff), fh) == NULL) {
            if (fclose(fh) != 0 ) {
                DBGPRINT(("WiFi: %s close fail in second fgets!\n", file));
            }
            fh = NULL;
            return ret;
        }

        /* Read each device line */
        while(fgets(buff, sizeof(buff), fh)) {
            /* Skip empty or almost empty lines. It seems that in some
             * cases fgets return a line with only a newline. */
            if((buff[0] == '\0') || (buff[1] == '\0'))
                continue;

            begin = buff;
            while(*begin == ' ')
                begin++;

            end = strstr(begin, ": ");
            /* Not found ??? To big ??? */
            if((end == NULL) || (((end - begin) + 1) > (IFNAMSIZ + 1)))
                continue;
            if (strncmp(begin, if_prefix, len) != 0)
                continue;

            /* Copy */
            ret = memcpy_s(ifname, nameBufSize, begin, (end - begin));
            if (ret != 0) {
                DBGPRINT(("WiFi: memcpy_s return %d in '%s'\n", ret, __func__));
            }
            ifname[end - begin] = '\0';
            ret = HI_SUCCESS;
            break;
        }
        if (fclose(fh) != 0 ) {
            DBGPRINT(("WiFi: %s close fail!\n", file));
        }
        fh = NULL;
    }

    return ret;
}

hi_s32 wlan_util_frequency_to_channel(hi_s32 freq)
{
    hi_s32 chn;

    if (freq == 2484)
        chn = 14;
    else if (freq < 2484)
        chn = (freq - 2407)/5;
    else if (freq >= 4910 && freq <= 4980)
        chn = (freq - 4000)/5;
    else
        chn = (freq - 5000)/5;

    return chn;
}

hi_s32 wlan_util_read_line(hi_char *buf, hi_char *line, hi_u32 lineSize)
{
    hi_s32 i = 0;
    hi_char *pos = line;

    if (line == NULL || buf == NULL)
        return 0;

    while (*buf != '\0') {
        i++;
        if (*buf == '\n')
            break;

        if ((line-pos) < (lineSize-1)) {
            *line++ = *buf++;
        } else {
            break;
        }
    }
    *line = '\0';

    return i;
}

hi_void wlan_util_string_split(hi_char *src, hi_char sym, hi_char *ss[])
{
    if (NULL == src || NULL == ss)
        return;

    *ss++ = src;
    while (*src) {
        if (*src == sym) {
            *src++ = '\0';
            *ss++ = src;
        } else
            src++;
    }
}

