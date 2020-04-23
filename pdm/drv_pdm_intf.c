/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: baseparam database operation function
 * Author: wan
 * Create: 2019-12-6
 */

#include <linux/kernel.h>
#include <linux/hikapi.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include "linux/compat.h"
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include "linux/hisilicon/securec.h"
#include <linux/miscdevice.h>

#include "hi_drv_module.h"
#include "hi_drv_dev.h"
#include "hi_drv_proc.h"
#include "hi_drv_mem.h"
#include <asm/uaccess.h>
#include "hi_drv_pdm.h"
#include "hi_drv_sys.h"
#include "drv_pdm_ext.h"
#include "drv_pdm.h"
#include "drv_pdm_ioctl.h"
#include "hi_osal.h"

#define PDM_NAME "HI_PDM"
#define PDM_TAG_MAX_LEN                512
#define PDM_CONVERSION_BASE_NUM        16
hi_pdm_export_func g_pdm_export_funcs = {
    .pdm_get_disp_param      = hi_drv_pdm_get_disp_param,
    .pdm_release_reserve_mem = hi_drv_pdm_release_reserve_mem,
    .pdm_get_reserve_mem     = hi_drv_pdm_get_reserve_mem,
    .pdm_get_data            = hi_drv_pdm_get_data,
    .pdm_get_sound_param     = hi_drv_pdm_get_sound_param
};

pdm_global g_pdm_global;

#ifdef HI_PROC_SUPPORT
static hi_void pdm_proc_reserve_mem(struct seq_file *p)
{
    hi_u32 i = 0;

    HI_PROC_PRINT(p, "\n-------------------- Reserve Mem Info -------------------------\n");
    HI_PROC_PRINT(p, "Name              phyaddr     size(Byte)       IsRelease\n");

    for (i = 0; i < g_pdm_global.buf_num; i++) {
        HI_PROC_PRINT(p, "%-16s  0x%-8p  %-16d  %d\n",
                      g_pdm_global.buf_info[i].buf_name,
                      g_pdm_global.buf_info[i].phy_addr,
                      g_pdm_global.buf_info[i].lenth,
                      g_pdm_global.buf_info[i].release);
    }
}

static hi_void pdm_proc_disp_timing(struct seq_file *p, hi_drv_disp_timing *timing)
{
    HI_PROC_PRINT(p, " ############ Custom Timing begin ##############\n");
    HI_PROC_PRINT(p, "VFB:             %d   \n", timing->vfb);
    HI_PROC_PRINT(p, "VBB:             %d   \n", timing->vbb);
    HI_PROC_PRINT(p, "VACT:            %d   \n", timing->vact);
    HI_PROC_PRINT(p, "HFB:             %d   \n", timing->hfb);
    HI_PROC_PRINT(p, "HBB:             %d   \n", timing->hbb);
    HI_PROC_PRINT(p, "HACT:            %d   \n", timing->hact);
    HI_PROC_PRINT(p, "VPW:             %d   \n", timing->vpw);
    HI_PROC_PRINT(p, "HPW:             %d   \n", timing->hpw);

    if (timing->idv) {
        HI_PROC_PRINT(p, "IDV:             %s   \n", "true");
    } else {
        HI_PROC_PRINT(p, "IDV:             %s   \n", "false");
    }

    if (timing->ihs) {
        HI_PROC_PRINT(p, "IHS:             %s   \n", "true");
    } else {
        HI_PROC_PRINT(p, "IHS:             %s   \n", "false");
    }

    if (timing->ivs) {
        HI_PROC_PRINT(p, "IVS:             %s   \n", "true");
    } else {
        HI_PROC_PRINT(p, "IVS:             %s   \n", "false");
    }

    if (timing->clkreversal) {
        HI_PROC_PRINT(p, "ClockReversal:       %s   \n", "true");
    } else {
        HI_PROC_PRINT(p, "ClockReversal:       %s   \n", "false");
    }

    HI_PROC_PRINT(p, "pix_freq:             %d   \n", timing->pix_freq);
    HI_PROC_PRINT(p, "refresh_rate:            %d   \n", timing->refresh_rate);

    HI_PROC_PRINT(p, " ############ Custom Timing end ##############\n");
}

static hi_void pdm_disp_proc_read(struct seq_file *p)
{
    hi_s32 ret;
    hi_u32 i;
    hi_u32 j;
    hi_disp_param disp_param = {0};

    for (i = HI_DRV_DISPLAY_0; i < HI_DRV_DISPLAY_BUTT; i++) {
        memset(&disp_param, 0, sizeof(disp_param));

        ret = drv_pdm_get_disp_param(i, &disp_param);
        if (ret != HI_SUCCESS) {
            continue;
        }

        HI_PROC_PRINT(p, "---------------------------Dispaly%d------------------------------\n", i);

        if (disp_param.format == HI_DRV_DISP_FMT_BUTT) {
            HI_PROC_PRINT(p, "format:              %s   \n", "Custom timing");
            pdm_proc_disp_timing(p, &(disp_param.disp_timing));
        } else {
            HI_PROC_PRINT(p, "format:              %d   \n", disp_param.format);
        }

        HI_PROC_PRINT(p, "source display:          %d\n", disp_param.src_disp);
        HI_PROC_PRINT(p, "background color:        0x%02x%02x%02x\n",
                      disp_param.bg_color.red, disp_param.bg_color.green, disp_param.bg_color.blue);
        HI_PROC_PRINT(p, "HuePlus/Brightness/Contrast/Saturation: %d/%d/%d/%d\n",
                      disp_param.hue_plus, disp_param.brightness, disp_param.contrast, disp_param.saturation);
        HI_PROC_PRINT(p, "virtual screen(Width/Height):        %d/%d\n",
                      disp_param.virt_screen_width, disp_param.virt_screen_height);
        HI_PROC_PRINT(p, "offset(Left/Top/Right/Bottom):       %d/%d/%d/%d\n",
                      disp_param.offset_info.left, disp_param.offset_info.top,
                      disp_param.offset_info.right, disp_param.offset_info.bottom);
        HI_PROC_PRINT(p, "bGammaEnable:        %d\n", disp_param.gamma_enable);
        HI_PROC_PRINT(p, "pixelformat:         %d\n", disp_param.pixel_format);
        HI_PROC_PRINT(p, "aspectRatio:         %dto%d\n", disp_param.aspect_ratio.aspect_ratio_w,
            disp_param.aspect_ratio.aspect_ratio_h);

        for (j = HI_DRV_DISP_INTF_TYPE_HDMI; j < HI_DRV_DISP_INTF_TYPE_MAX; j++) {
            if (disp_param.intf[j].intf_type == HI_DRV_DISP_INTF_TYPE_HDMI) {
                HI_PROC_PRINT(p, "HDMI:            HDMI_%d\n", disp_param.intf[j].un_intf.hdmi);
            } else if (disp_param.intf[j].intf_type == HI_DRV_DISP_INTF_TYPE_LCD) {
                HI_PROC_PRINT(p, "LCD:             LCD_%d\n", disp_param.intf[j].un_intf.lcd);
            } else if (disp_param.intf[j].intf_type == HI_DRV_DISP_INTF_TYPE_BT1120) {
                HI_PROC_PRINT(p, "BT1120:              BT1120_%d\n", disp_param.intf[j].un_intf.bt1120);
            } else if (disp_param.intf[j].intf_type == HI_DRV_DISP_INTF_TYPE_BT656) {
                HI_PROC_PRINT(p, "BT656:           BT656_%d\n", disp_param.intf[j].un_intf.bt656);
            } else if (disp_param.intf[j].intf_type == HI_DRV_DISP_INTF_TYPE_YPBPR) {
                HI_PROC_PRINT(p, "YPbPr(Y/Pb/Pr):          %d/%d/%d\n", disp_param.intf[j].un_intf.ypbpr.dac_y,
                              disp_param.intf[j].un_intf.ypbpr.dac_pb, disp_param.intf[j].un_intf.ypbpr.dac_pr);
            } else if (disp_param.intf[j].intf_type == HI_DRV_DISP_INTF_TYPE_RGB) {
                HI_PROC_PRINT(p, "RGB(R/G/B):          %d/%d/%d\n", disp_param.intf[j].un_intf.rgb.dac_r,
                              disp_param.intf[j].un_intf.rgb.dac_g, disp_param.intf[j].un_intf.rgb.dac_b);
            } else if (disp_param.intf[j].intf_type == HI_DRV_DISP_INTF_TYPE_CVBS) {
                HI_PROC_PRINT(p, "CVBS:            %d\n", disp_param.intf[j].un_intf.cvbs.dac_cvbs);
            } else if (disp_param.intf[j].intf_type == HI_DRV_DISP_INTF_TYPE_SVIDEO) {
                HI_PROC_PRINT(p, "SVIDEO(Y/C):         %d/%d\n", disp_param.intf[j].un_intf.svideo.dac_y,
                              disp_param.intf[j].un_intf.svideo.dac_c);
            } else if (disp_param.intf[j].intf_type == HI_DRV_DISP_INTF_TYPE_VGA) {
                HI_PROC_PRINT(p, "VGA(R/G/B):          %d/%d/%d\n", disp_param.intf[j].un_intf.vga.dac_r,
                              disp_param.intf[j].un_intf.vga.dac_g, disp_param.intf[j].un_intf.vga.dac_b);
            } else if (disp_param.intf[j].intf_type == HI_DRV_DISP_INTF_TYPE_MIPI) {
                HI_PROC_PRINT(p, "MIPI :          id:%d,mode:%d\n", disp_param.intf[j].un_intf.mipi.mipi_id,
                    disp_param.intf[j].un_intf.mipi.mipi_mode);
            }
        }

        if (i == HI_DRV_DISPLAY_0) {
            HI_PROC_PRINT(p, "HDMI VidOutMode:        %d\n", disp_param.vid_out_mode);
            HI_PROC_PRINT(p, "HDMI DeepColorMode:     %d\n", disp_param.deep_color_mode);
        }
    }

    return;
}

static hi_void pdm_proc_sound_info(struct seq_file *p, hi_u32 dev_num,
                                   hi_drv_pdm_sound_param *sound_param,
                                   hi_bool volume_exist_in_base)
{
    hi_u32 i;
    hi_char *m_clk[] = {
        "128*fs",
        "256*fs",
        "384*fs",
        "512*fs",
        "768*fs",
        "1024*fs"
    };

    HI_PROC_PRINT(p, "---------------------------Sound%d------------------------------\n", dev_num);

    for (i = 0; i < sound_param->port_num; i++) {
        if (sound_param->out_port[i].port == HI_AO_PORT_DAC0) {
            if (volume_exist_in_base) {
                HI_PROC_PRINT(p, "DAC:                 %s       VOL:%3d\n", "DAC0",
                              sound_param->audio_volume[i]);
            } else {
                HI_PROC_PRINT(p, "DAC:                 %s\n", "DAC0");
            }
        }

        if (sound_param->out_port[i].port == HI_AO_PORT_SPDIF0) {
            if (volume_exist_in_base) {
                HI_PROC_PRINT(p, "SPDIF:               %s     VOL:%3d\n",
                              "SPDIF0", sound_param->audio_volume[i]);
            } else {
                HI_PROC_PRINT(p, "SPDIF:               %s\n", "SPDIF0");
            }
        }

        if (sound_param->out_port[i].port == HI_AO_PORT_HDMI0) {
            if (volume_exist_in_base) {
                HI_PROC_PRINT(p, "HDMI:                %s      VOL:%3d\n",
                              "HDMI0", sound_param->audio_volume[i]);
            } else {
                HI_PROC_PRINT(p, "HDMI:                %s\n", "HDMI0");
            }
        }

        if (sound_param->out_port[i].port == HI_AO_PORT_ARC0) {
            if (volume_exist_in_base) {
                HI_PROC_PRINT(p, "ARC:                 %s       VOL:%3d\n", "ARC0",
                              sound_param->audio_volume[i]);
            } else {
                HI_PROC_PRINT(p, "ARC:                 %s\n", "ARC0");
            }
        }

        if ((sound_param->out_port[i].port == HI_AO_PORT_I2S0) ||
                (sound_param->out_port[i].port == HI_AO_PORT_I2S1)) {
            HI_PROC_PRINT(p, "I2S%d attr:              \n",
                          sound_param->out_port[i].port - HI_AO_PORT_I2S0);

            if (volume_exist_in_base) {
                HI_PROC_PRINT(p, " VOL :                   %3d\n", sound_param->audio_volume[i]);
            }

            if (sound_param->out_port[i].un_attr.i2s_attr.attr.master) {
                HI_PROC_PRINT(p, " Master:             TRUE\n");
            } else {
                HI_PROC_PRINT(p, " Master:             FALSE\n");
            }

            if (sound_param->out_port[i].un_attr.i2s_attr.attr.pcm_sample_rise_edge) {
                HI_PROC_PRINT(p, " PcmSampleRiseEdge:      TRUE\n");
            } else {
                HI_PROC_PRINT(p, " PcmSampleRiseEdge:      FALSE\n");
            }

            HI_PROC_PRINT(p, " Master colock:          %s\n",
                          m_clk[sound_param->out_port[i].un_attr.i2s_attr.attr.mclk]);

            HI_PROC_PRINT(p, " Bit colock:         %d DIV\n",
                          sound_param->out_port[i].un_attr.i2s_attr.attr.bclk);

            HI_PROC_PRINT(p, " Channel:            %d\n",
                          sound_param->out_port[i].un_attr.i2s_attr.attr.channel);

            if (sound_param->out_port[i].un_attr.i2s_attr.attr.i2s_mode == HI_I2S_STD_MODE) {
                HI_PROC_PRINT(p, " Mode:           standard\n");
            } else {
                HI_PROC_PRINT(p, " Mode:           pcm\n");
            }

            HI_PROC_PRINT(p, " Bit Depth:          %d\n",
                          sound_param->out_port[i].un_attr.i2s_attr.attr.bit_depth);

            HI_PROC_PRINT(p, " PCM Delay Cycle:        %d\n",
                          sound_param->out_port[i].un_attr.i2s_attr.attr.pcm_delay_cycle);
        }
    }

    return;
}

static hi_s32 pdm_proc_read(hi_void *p, hi_void *v)
{
    hi_s32                   ret = HI_FAILURE;
    hi_drv_pdm_sound_param *sound_param = HI_NULL;
    hi_u32                   i = 0;

    pdm_disp_proc_read((struct seq_file *)p);

    sound_param = HI_KMALLOC(HI_ID_PDM, sizeof(hi_drv_pdm_sound_param), GFP_KERNEL);
    if (sound_param == HI_NULL) {
        return 0;
    }

    for (i = AO_SND_0; i < AO_SND_MAX; i++) {
        hi_bool volume_exist_in_base = HI_FALSE;

        memset(sound_param, 0, sizeof(hi_drv_pdm_sound_param));

        ret = hi_drv_pdm_get_sound_param_ex(i, sound_param, &volume_exist_in_base);
        if (ret != HI_SUCCESS) {
            continue;
        }

        if (sound_param->port_num > sizeof(sound_param->out_port) / sizeof(sound_param->out_port[0])) {
            continue;
        }

        pdm_proc_sound_info((struct seq_file *)p, i, sound_param, volume_exist_in_base);
    }

    HI_KFREE(HI_ID_PDM, sound_param);

    pdm_proc_reserve_mem((struct seq_file *)p);

    return 0;
}
#endif

/*
 * tag format is version=1.0.0.0  fb=0x85000000,0x10000  baseparam=0x86000000,0x2000 бнбн
 */
hi_s32 pdm_get_tag_buf(hi_void)
{
    hi_char pdm_tag[PDM_TAG_MAX_LEN] = {0};
    hi_u32  pdm_len;
    hi_char tmp_buf[32]; /* the tmp_buf max len is 32 */
    hi_char *p = HI_NULL;
    hi_char *q = HI_NULL;
    hi_s32  ret = HI_FAILURE;
    hi_u32  buf_len = 0;

    pdm_len = get_param_data("pdm_tag", pdm_tag, PDM_TAG_MAX_LEN);
    if (pdm_len >= PDM_TAG_MAX_LEN) {
        return HI_FAILURE;
    }

    pdm_tag[PDM_TAG_MAX_LEN - 1] = '\0';

    p = pdm_tag;

    g_pdm_global.buf_num = 0;

    while (*p != '\0') {
        p = strstr(p, " ");
        if (p == 0) {
            return HI_SUCCESS;
        }

        q = strstr(p, "=");
        if (q == 0) {
            return HI_SUCCESS;
        }

        p++;

        if ((q - p) > 16) { /* the "=" less than 16 byte */
            return HI_FAILURE;
        }

        buf_len = sizeof(g_pdm_global.buf_info[g_pdm_global.buf_num].buf_name);
        ret = memcpy_s(g_pdm_global.buf_info[g_pdm_global.buf_num].buf_name, buf_len, p, q - p);
        if (ret != HI_SUCCESS) {
            HI_ERR_PDM("memcpy_s failed !\n");
            return ret;
        }

        p = q + 1;
        q = strstr(p, ",");
        if (q == 0) {
            return HI_FAILURE;
        }

        memset(tmp_buf, 0x0, sizeof(tmp_buf));

        if ((q - p) > sizeof(tmp_buf)) {
            return HI_FAILURE;
        }

        ret = memcpy_s(tmp_buf, sizeof(tmp_buf), p, q - p);
        if (ret != HI_SUCCESS) {
            HI_ERR_PDM("memcpy_s failed !\n");
            return ret;
        }

        g_pdm_global.buf_info[g_pdm_global.buf_num].phy_addr =
            (hi_u8 *)(uintptr_t)osal_strtoul(tmp_buf, NULL, PDM_CONVERSION_BASE_NUM);

        p = q + 1;
        q = strstr(p, " ");
        if (q == 0) {
            q = pdm_tag + pdm_len;
        }

        memset(tmp_buf, 0x0, sizeof(tmp_buf));

        if ((q - p) > sizeof(tmp_buf)) {
            return HI_FAILURE;
        }

        ret = memcpy_s(tmp_buf, sizeof(tmp_buf), p, q - p);
        if (ret != HI_SUCCESS) {
            HI_ERR_PDM("memcpy_s failed !\n");
            return ret;
        }

        g_pdm_global.buf_info[g_pdm_global.buf_num].lenth = osal_strtoul(tmp_buf, NULL, PDM_CONVERSION_BASE_NUM);

        g_pdm_global.buf_num++;
    }

    return HI_SUCCESS;
}

hi_s32 hi_drv_pdm_init(hi_void)
{
    hi_s32 ret;

    ret = osal_exportfunc_register(HI_ID_PDM, PDM_NAME, (hi_void *)&g_pdm_export_funcs);
    if (ret != HI_SUCCESS) {
        HI_FATAL_PDM("ERR: hi_drv_module_register!\n");
        return ret;
    }

    memset(&g_pdm_global, 0x0, sizeof(pdm_global));

    osal_sem_init(&g_pdm_global.pdm_mutex, 1);

    ret = pdm_get_tag_buf();
    if (ret != HI_SUCCESS) {
        memset(g_pdm_global.buf_info, 0x0, sizeof(pdm_buf_info) * PDM_MAX_BUF_NUM);
    }

    return ret;
}

hi_s32 pdm_drv_open(struct inode *finode, struct file   *ffile)
{
    return HI_SUCCESS;
}
hi_s32 pdm_drv_close(struct inode *finode, struct file   *ffile)
{
    return HI_SUCCESS;
}
hi_slong pdm_ioctl(struct file *file, unsigned int cmd, hi_void *arg)
{
    hi_s32 ret = HI_SUCCESS;

    switch (cmd) {
        case CMD_PDM_GET_DATA: {
            hi_u8 *data_addr = NULL;
            hi_u32 data_len = 0;
            pdm_get_data *buffer_info = NULL;

            buffer_info = (pdm_get_data *)arg;

            if ((buffer_info == NULL) || (buffer_info->buf_addr == NULL)) {
                HI_ERR_PDM("para is invalid!\n");
                return HI_ERR_PDM_PTR_NULL;
            }

            ret = hi_drv_pdm_get_data(buffer_info->buf_name, &data_addr, &data_len);
            if (ret != HI_SUCCESS) {
                HI_ERR_PDM("drv_pdm_get_data err! ret:0x%x\n", ret);
                return ret;
            }

            if (data_len > buffer_info->buf_lenth) {
                data_len = buffer_info->buf_lenth;
            }

            if (osal_copy_to_user((void __user *)buffer_info->buf_addr, data_addr, data_len)) {
                HI_ERR_PDM("copy_to_user failed!\n");
                return -EFAULT;
            }

            break;
        }
        case CMD_PDM_GET_RESERVE_MEM_INFO: {
            pdm_reserve_mem_info *reserve_mem_info = NULL;
            reserve_mem_info = (pdm_reserve_mem_info *)arg;

            if (reserve_mem_info == NULL) {
                HI_ERR_PDM("reserve_mem_info is NULL!\n");
                return HI_ERR_PDM_PTR_NULL;
            }

            ret = hi_drv_pdm_get_reserve_mem(reserve_mem_info->buf_name, &reserve_mem_info->phy_addr,
                                             &reserve_mem_info->size);
            if (ret != HI_SUCCESS) {
                HI_ERR_PDM("hi_drv_pdm_get_reserve_mem fail 0x%x\n", ret);
                return ret;
            }

            break;
        }

        case CMD_PDM_SEND_LICENSE_DATA: {
            pdm_license_data *lic_data = HI_NULL;

            lic_data = (pdm_license_data*)arg;

            if (lic_data == HI_NULL) {
                HI_ERR_PDM("pdm_license_data is NULL!\n");
                return HI_ERR_PDM_PTR_NULL;
            }

            ret = hi_drv_pdm_set_license_data(lic_data->mmz_handle, lic_data->mmz_length);
            if (ret != HI_SUCCESS) {
                HI_ERR_PDM("call hi_drv_pdm_set_license_data fail :%x\n", ret);
                return ret;
            }

            break;
        }
        default:
            HI_ERR_PDM("cmd err 0x%x\n", cmd);
            return -ENOIOCTLCMD;
    }
    return ret;
}

#ifdef CONFIG_COMPAT
hi_slong pdm_compat_ioctl(struct file *file, unsigned int cmd, hi_void *arg)
{
    hi_s32  ret = HI_SUCCESS;

    switch (cmd) {
        case CMD_PDM_COMPAT_GET_DATA: {
            hi_u8 *data_addr = NULL;
            hi_u32 data_len = 0;
            pdm_compat_get_data *buffer_info = NULL;

            buffer_info = (pdm_compat_get_data *)arg;

            if ((buffer_info == NULL) || (compat_ptr(buffer_info->buf_addr)) == NULL) {
                HI_ERR_PDM("para is invalid!\n");
                return HI_ERR_PDM_PTR_NULL;
            }

            ret = hi_drv_pdm_get_data(buffer_info->buf_name, &data_addr, &data_len);
            if (ret != HI_SUCCESS) {
                HI_ERR_PDM("drv_pdm_get_data err! ret:0x%x\n", ret);
                return ret;
            }

            if (data_len > buffer_info->buf_lenth) {
                data_len = buffer_info->buf_lenth;
            }

            if (osal_copy_to_user(compat_ptr(buffer_info->buf_addr), data_addr, data_len)) {
                HI_ERR_PDM("copy_to_user failed ret %d!\n", ret);
                return -EFAULT;
            }

            break;
        }
        case CMD_PDM_GET_RESERVE_MEM_INFO: {
            pdm_reserve_mem_info *reserve_mem_info = NULL;
            reserve_mem_info = (pdm_reserve_mem_info *)arg;

            if (reserve_mem_info == NULL) {
                HI_ERR_PDM("reserve_mem_info is NULL!\n");
                return HI_ERR_PDM_PTR_NULL;
            }

            ret = hi_drv_pdm_get_reserve_mem(reserve_mem_info->buf_name, &reserve_mem_info->phy_addr,
                                             &reserve_mem_info->size);
            if (ret != HI_SUCCESS) {
                HI_ERR_PDM("hi_drv_pdm_get_reserve_mem fail 0x%x\n", ret);
                return ret;
            }

            break;
        }
        case CMD_PDM_SEND_LICENSE_DATA: {
            pdm_license_data *lic_data = HI_NULL;

            lic_data = (pdm_license_data*)arg;

            if (lic_data == HI_NULL) {
                HI_ERR_PDM("pdm_license_data is NULL!\n");
                return HI_ERR_PDM_PTR_NULL;
            }

            ret = hi_drv_pdm_set_license_data(lic_data->mmz_handle, lic_data->mmz_length);
            if (ret != HI_SUCCESS) {
                HI_ERR_PDM("call hi_drv_pdm_set_license_data fail :%x\n", ret);
                return ret;
            }

            break;
        }
        default:
            HI_ERR_PDM("cmd err 0x%x\n", cmd);
            return -ENOIOCTLCMD;
    }
    return ret;
}
#endif

static long pdm_drv_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg)
{
    hi_s32 ret;

    ret = hi_drv_user_copy(ffile, cmd, arg, pdm_ioctl);

    return ret;
}

#ifdef CONFIG_COMPAT
static long pdm_drv_compat_ioctl(struct file *ffile, unsigned int cmd, unsigned long arg)
{
    hi_s32 ret;

    ret = hi_drv_user_copy(ffile, cmd, arg, pdm_compat_ioctl);

    return ret;
}
#endif

static struct file_operations g_pdm_fops = {
    .owner          =   THIS_MODULE,
    .open           =   pdm_drv_open,
    .unlocked_ioctl =   pdm_drv_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl   =   pdm_drv_compat_ioctl,
#endif
    .release        =   pdm_drv_close,
};

static struct miscdevice g_pdm_misc_device = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = HI_DEV_PDM_NAME,
    .fops   = &g_pdm_fops,
};

hi_s32 hi_drv_pdm_deinit(hi_void)
{
    misc_deregister(&g_pdm_misc_device);
    osal_exportfunc_unregister(HI_ID_PDM);

    return HI_SUCCESS;
}

hi_s32 pdm_drv_mod_init(hi_void)
{
#ifdef HI_PROC_SUPPORT
    hi_char proc_name[] = HI_MOD_PDM_NAME;
    osal_proc_entry *proc_item = HI_NULL;
#endif
    hi_u32 ret;

#ifndef HI_MCE_SUPPORT
    hi_drv_pdm_init();
#endif

#ifdef HI_PROC_SUPPORT
    proc_item = osal_proc_add(proc_name, strlen(proc_name));
    if (proc_item != HI_NULL) {
        proc_item->read  = pdm_proc_read;
    }

#endif

    ret = misc_register(&g_pdm_misc_device);
    if (ret != HI_SUCCESS) {
        HI_FATAL_PDM("register device failed\n");
        return HI_FAILURE;
    }
    return 0;
}

hi_void pdm_drv_mod_exit(hi_void)
{
#ifndef HI_MCE_SUPPORT
        hi_s32 ret;
#endif

#ifdef HI_PROC_SUPPORT
    hi_char proc_name[] = HI_MOD_PDM_NAME;

    hi_drv_proc_remove_module(proc_name);
#endif

#ifndef HI_MCE_SUPPORT
    ret = hi_drv_pdm_deinit();
    if (ret != HI_SUCCESS) {
        HI_ERR_PDM("hi_drv_pdm_deinit() err 0x%x\n", ret);
        return;
    }
#endif

    return;
}

#ifdef MODULE
module_init(pdm_drv_mod_init);
module_exit(pdm_drv_mod_exit);
#endif

EXPORT_SYMBOL(pdm_drv_mod_init);
EXPORT_SYMBOL(pdm_drv_mod_exit);

MODULE_AUTHOR("HISILICON");
MODULE_LICENSE("GPL");
