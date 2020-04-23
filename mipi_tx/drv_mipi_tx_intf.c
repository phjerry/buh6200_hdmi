/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description: drv mipi tx inftace module
* Author: sdk
* Create: 2019-11-20
*/
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "hi_drv_sys.h"
#include "linux/hisilicon/securec.h"
#include "hi_type.h"
#include "hi_errno.h"
#include "hi_drv_mipi.h"
#include "hi_osal.h"
#include "drv_pdm_ext.h"
#include "drv_mipi_ext.h"
#include "mipi_tx_hal.h"
#include "drv_mipi_tx_define.h"
#include "drv_mipi_tx_ioctl.h"
#include "mipi_panel_func.h"

#ifdef HI_MIPI_DSC_SUPPORTED
#include "mipi_dsc_ctrl.h"
#include "mipi_dsc_helper.h"
#endif

#define MIPI_NAME "HI_MIPI"
static dev_t g_mipi_devno;
static struct class *g_mipi_class = HI_NULL;
static struct cdev *g_mipi_cdev = HI_NULL;
static struct device *g_mipi_dev = HI_NULL;

#define INDEX_MIPI0 0
#define INDEX_MIPI1 1
mipi_tx_dev_cfg g_mipi_tx_dev_ctx[MAX_CTRL_NUM]; /* ctx,context */
mipi_tx_dev_phy g_mipi_tx_phy_ctx_intf[MAX_CTRL_NUM]; /* ctx,context, get from mipi_tx_hal.c */
hi_drv_mipi_pdm_attr g_mipi_pdm_attr[MAX_CTRL_NUM];

static hi_void assemble_dev_cfg(
    hi_drv_mipi_id id, hi_drv_mipi_mode mipi_mode, const hi_drv_disp_intf_info *intf_info, mipi_tx_dev_cfg *cfg)
{
    int i;

    if (cfg == HI_NULL) {
        hi_err_mipi("ptr is null.\n");
    }

    /* cfg->phy_data_rate need to be calculated separately(add a function) */
    cfg->phy_data_rate = 999; /* default rate 999, optimize later */

    for (i = 0; i < MAX_LANE_NUM; i++) {
        cfg->lane_id[i] = g_mipi_pdm_attr[id].attr_from_pdm.lane_id[i];
    }

    /* cfg from vdp */
    cfg->id = id;
    cfg->mipi_mode = mipi_mode;
    cfg->pixel_clk = intf_info->disp_timing.static_timing.timing.pix_freq;
    cfg->sync_info.vid_pkt_size = intf_info->disp_timing.static_timing.timing.hact;
    if (mipi_mode == HI_DRV_MIPI_MODE_DOUBLE) {
        cfg->sync_info.vid_pkt_size = cfg->sync_info.vid_pkt_size / 2; /* Longitudinally divided into 2 parts */

        if (cfg->pixel_clk % 2 == 1) { /* remainder of 2 */
            cfg->pixel_clk++;
        }
        cfg->pixel_clk = cfg->pixel_clk / 2; /* Longitudinally divided into 2 parts */
    }
    cfg->sync_info.vid_hsa_pixels = intf_info->disp_timing.static_timing.timing.hpw;
    cfg->sync_info.vid_hbp_pixels = intf_info->disp_timing.static_timing.timing.hbb - cfg->sync_info.vid_hsa_pixels;
    cfg->sync_info.vid_hfp_pixels = intf_info->disp_timing.static_timing.timing.hfb;
    cfg->sync_info.vid_hline_pixels = cfg->sync_info.vid_pkt_size + cfg->sync_info.vid_hsa_pixels \
                                      + cfg->sync_info.vid_hbp_pixels + cfg->sync_info.vid_hfp_pixels;
    cfg->sync_info.vid_active_lines = intf_info->disp_timing.static_timing.timing.vact;
    cfg->sync_info.vid_vsa_lines = intf_info->disp_timing.static_timing.timing.vpw;
    cfg->sync_info.vid_vbp_lines = intf_info->disp_timing.static_timing.timing.vbb - cfg->sync_info.vid_vsa_lines;
    cfg->sync_info.vid_vfp_lines = intf_info->disp_timing.static_timing.timing.vfb;

    /* cfg from pdm */
    cfg->output_mode = g_mipi_pdm_attr[id].attr_from_pdm.output_mode;
    cfg->video_mode = g_mipi_pdm_attr[id].attr_from_pdm.video_mode;

    /* need to fix, add rgb16/18/24, use the source from VDP */
    if (cfg->mipi_mode == HI_DRV_MIPI_MODE_SINGLE_DSC) {
        cfg->output_format = MIPI_DSI_COMPRESS;
    } else {
        cfg->output_format = MIPI_RGB_24_BIT;
    }

    /* parameter need by cmd mode */
    cfg->sync_info.edpi_cmd_size = 0; /* looks like this is the momory size of write cmd, check how to use it */
}

#ifdef HI_MIPI_DSC_SUPPORTED
static hi_void assemble_dsc_param(hi_drv_mipi_id id, para_input_s *inpara)
{
    if (inpara == HI_NULL) {
        hi_err_mipi("inpara null.\n");
    }

    inpara->is_encoder = 0x1; /* encoder: 1, decoder: 0 */
    inpara->convert_rgb = 0x1; /* rgb do conversion: 1 - yes, 0 - no */
    inpara->native_420 = 0; /* yuv420 or not, 1 - yes 0 - no */
    inpara->native_422 = 0; /* yuv422 or not, 1 - yes 0 - no */
    inpara->bits_per_component = 8; /* bit depth, in dsc mode, only support 8 bits */

    /* YUV or RGB: min_bpp: 8*16,   max_bpp: 48*bpc-1 */
    /* YUV422:     min_bpp: 7*16*2, max_bpp: (32*bpc-1)*2 */
    /* YUV420:     min_bpp: 6*16*2, max_bpp: (24*bpc-1)*2 */
    /* cmp_ratio: */
    /* YUV or RGB: cmp_ratio = bpc * 3 / (bpp / 16) */
    /* YUV422:     cmp_ratio = bpc * 2 / (bpp / 16 / 2) */
    /* YUV420:     cmp_ratio = bpc * 1.5 / (bpp / 16 / 2) */
    inpara->bits_per_pixel = 8 * 16; /* mipi tx only support RGB format, default set bpp to minimum value 8*16 */
    inpara->pic_width = g_mipi_tx_dev_ctx[id].sync_info.vid_pkt_size;
    inpara->pic_height = g_mipi_tx_dev_ctx[id].sync_info.vid_active_lines;
    inpara->slice_width = inpara->pic_width;
    inpara->slice_height = 0; /* do not use: inpara->slice_height = inpara->pic_height; */
}
#endif

static hi_s32 get_dev_type(hi_void)
{
    hi_drv_mipi_id id0 = g_mipi_pdm_attr[0].id;
    hi_drv_mipi_id id1 = g_mipi_pdm_attr[1].id;
    hi_drv_mipi_mode mipi_mode = g_mipi_pdm_attr[0].attr_from_pdm.mipi_mode;

    if (id0 == HI_DRV_MIPI_ID_0) {
        if (mipi_mode == HI_DRV_MIPI_MODE_DOUBLE || id1 == HI_DRV_MIPI_ID_1) {
            return MIPI_DEV_TYPE_CTR0_CTR1;
        }

        if (mipi_mode == HI_DRV_MIPI_MODE_SINGLE_DSC) {
            return MIPI_DEV_TYPE_CTR0_DSC;
        }

        return MIPI_DEV_TYPE_CTR0;
    }

    return MIPI_DEV_TYPE_CTR1;
}

static hi_s32 exec_on_cmd(hi_drv_mipi_id id, const mipi_panel_cmd *power_on_cmd, hi_s32 cmd_index)
{
    hi_s32 k;
    hi_s32 ret;
    mipi_cmd_info cmd_info;

    cmd_info.id = id;
    cmd_info.data_type = power_on_cmd[cmd_index].data_type;
    cmd_info.in_param_size = power_on_cmd[cmd_index].cmd_len;

    if (cmd_info.in_param_size != 0) {
        cmd_info.in_params = kzalloc(cmd_info.in_param_size, GFP_KERNEL);
    }

    for (k = 0; k < cmd_info.in_param_size; k++) {
        cmd_info.in_params[k] = power_on_cmd[cmd_index].cmd[k];
    }

    cmd_info.out_param_size = 0;
    cmd_info.out_params = HI_NULL;

    ret = mipi_tx_exec_cmd(&cmd_info);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("cmd%d(%x) exec err.\n", cmd_index, cmd_info.data_type);
        goto out;
    }

    if (cmd_info.in_param_size != 0) {
        kfree(cmd_info.in_params);
    }
    mdelay(power_on_cmd[cmd_index].delay);

    return HI_SUCCESS;
out:
    if (cmd_info.in_params != HI_NULL) {
        kfree(cmd_info.in_params);
    }
    return HI_FAILURE;
}

hi_s32 exec_panel_init_cmd(hi_drv_mipi_id id)
{
    hi_s32 i;
    hi_s32 ret;
    hi_s32 cmd_num, cmd_len;
    mipi_panel_cmd *power_on_cmd = HI_NULL;

    ret = get_power_on_cmd_num((panel_id)id, &cmd_num);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("get_power_on_cmd err id = %d.\n", id);
        return HI_FAILURE;
    }

    if (cmd_num == 0) {
        hi_err_mipi("power on cmd num config or get cmd_num err.\n", id);
        return HI_SUCCESS;
    }

    power_on_cmd = kzalloc(cmd_num * (sizeof(mipi_panel_cmd)), GFP_KERNEL);
    if (power_on_cmd == HI_NULL) {
        hi_err_mipi("alloc on_cmd space err.\n", id);
        kfree(power_on_cmd);
        return HI_FAILURE;
    }

    ret = get_power_on_cmd((panel_id)id, power_on_cmd, &cmd_len);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("get_power_on_cmd err id = %d.\n", id);
        return HI_FAILURE;
    }

    for (i = 0; i < cmd_num; i++) {
        ret = exec_on_cmd(id, power_on_cmd, i);
        if (ret != HI_SUCCESS) {
            hi_err_mipi("power_on_cmd(%d) exec err.\n", i);
        }
    }

    kfree(power_on_cmd);

    return HI_SUCCESS;
}

hi_s32 check_mipi_mode(hi_drv_mipi_id id, hi_drv_mipi_mode mipi_mode)
{
    return (g_mipi_pdm_attr[id].attr_from_pdm.mipi_mode == mipi_mode) ? HI_SUCCESS : HI_FALSE;
}

hi_s32 mipi_tx_enable(hi_drv_mipi_id id, hi_drv_mipi_mode mipi_mode)
{
#ifdef HI_MIPI_DSC_SUPPORTED
    hi_s32 ret;
    para_input_s inpara;
#endif
    mipi_dev_type dev_type = (id == HI_DRV_MIPI_ID_1) ? MIPI_DEV_TYPE_CTR1 : MIPI_DEV_TYPE_CTR0;

    if (g_mipi_tx_dev_ctx[id].id == HI_MIPI_ID_MAX) {
        hi_err_mipi("mipi(%d) is not initialized, enable failed.\n", id);
        return HI_FAILURE;
    }

    if (mipi_tx_register_irq(dev_type) != HI_SUCCESS) {
        hi_err_mipi("mipi(%d) register irq error. \n", id);
    }

    mipi_tx_drv_enable_input(id, g_mipi_tx_dev_ctx[id].output_mode);
#ifdef HI_MIPI_DSC_SUPPORTED
    if (g_mipi_tx_dev_ctx[id].mipi_mode == HI_DRV_MIPI_MODE_SINGLE_DSC) {
        assemble_dsc_param(id, &inpara);

        ret = hi_mipi_dsc_enable(id, &inpara);
        if (ret != HI_SUCCESS) {
            hi_err_mipi("enable dsc(%d) failed.\n", id);
            return HI_FAILURE;
        }
    }
#endif
    return HI_SUCCESS;
}

hi_s32 mipi_tx_disable(hi_drv_mipi_id id, hi_drv_mipi_mode mipi_mode)
{
    mipi_dev_type dev_type = (id == HI_DRV_MIPI_ID_1) ? MIPI_DEV_TYPE_CTR1 : MIPI_DEV_TYPE_CTR0;

    if (g_mipi_tx_dev_ctx[id].id == HI_MIPI_ID_MAX) {
        hi_err_mipi("mipi(%d) is not initialized, need not disable.\n", id);
        return HI_SUCCESS;
    }

    mipi_tx_unregister_irq(dev_type);

    mipi_tx_drv_disable_input(id, g_mipi_tx_dev_ctx[id].output_mode);
#ifdef HI_MIPI_DSC_SUPPORTED
    if (g_mipi_tx_dev_ctx[id].mipi_mode == HI_DRV_MIPI_MODE_SINGLE_DSC) {
        hi_mipi_dsc_disable(id); /* suppose that mipi id and dsc id are paired */
    }
#endif
    return HI_SUCCESS;
}

static hi_s32 mipi_set_dev_cfg_help(hi_s32 index, const mipi_tx_dev_cfg *dev_cfg)
{
    hi_s32 ret;

    /* set controller config */
    ret = mipi_tx_drv_set_controller_cfg(index, dev_cfg);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("mipi set ctr(%x) err. ret(%x)\n", index, ret);
        return ret;
    }

    /* set phy config */
    ret = mipi_tx_drv_set_phy_cfg(index, dev_cfg);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("mipi set phy(%x) err. ret(%x)\n", index, ret);
        return ret;
    }

    /* update mipi tx dev context */
    ret = memcpy_s(&(g_mipi_tx_dev_ctx[index]), sizeof(mipi_tx_dev_cfg), dev_cfg, sizeof(mipi_tx_dev_cfg));
    if (ret != HI_SUCCESS) {
        hi_err_mipi("update mipi tx dev context err. ret(%x)\n", ret);
    }

    return ret;
}

hi_s32 mipi_tx_set_dev_cfg(hi_drv_mipi_id id, hi_drv_mipi_mode mipi_mode, const hi_drv_disp_intf_info *intf_info)
{
    hi_s32 ret, i;
    mipi_tx_dev_cfg temp_cfg;

    if (id == HI_DRV_MIPI_ID_1 && mipi_mode == HI_DRV_MIPI_MODE_DOUBLE) {
        assemble_dev_cfg(HI_DRV_MIPI_ID_0, mipi_mode, intf_info, &temp_cfg);
        for (i = 0; i < MAX_LANE_NUM; i++) {
            temp_cfg.lane_id[i] = g_mipi_pdm_attr[HI_DRV_MIPI_ID_0].attr_from_pdm.lane_id[i];
        }
        temp_cfg.id = HI_DRV_MIPI_ID_1;
    } else {
        assemble_dev_cfg(id, mipi_mode, intf_info, &temp_cfg);
    }

    hi_info_mipi("config mipi(%d), mode(%d).\n", id, mipi_mode);

    ret = mipi_set_dev_cfg_help(id, &temp_cfg);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("config mipi(%d) err.\n", id);
    }
#ifdef HI_MIPI_DSC_SUPPORTED
    if (mipi_mode == HI_DRV_MIPI_MODE_SINGLE_DSC) {
        ret = hi_mipi_dsc_init(id); /* suppose that mipi id and dsc id are paired */
        if (ret != HI_SUCCESS) {
            hi_err_mipi("config mipi(%d) dsc err.\n", id);
        }
    }
#endif
    return ret;
}

static mipi_func_export g_mipi_export_func = {
    .intf_suspend        = hi_drv_mipi_suspend,
    .intf_resume         = hi_drv_mipi_resume,
    .intf_check_validate = hi_drv_mipi_check_validate,
    .intf_enable         = hi_drv_mipi_enable,
    .intf_detach         = hi_drv_mipi_detach,
    .intf_attach         = hi_drv_mipi_attach,
    .intf_prepare        = hi_drv_mipi_prepare,
    .intf_config         = hi_drv_mipi_config,
    .intf_atomic_config  = hi_drv_mipi_atomic_config,
};

hi_s32 mipi_tx_exec_cmd(hi_void *arg)
{
    mipi_cmd_info *cmd_info = HI_NULL;
    if (arg == HI_NULL) {
        hi_err_mipi("cmd_info is null.\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    cmd_info = (mipi_cmd_info *)arg;

    if (cmd_info->id != g_mipi_tx_dev_ctx[cmd_info->id].id) {
        hi_err_mipi("controller not init %d %d.\n", cmd_info->id, g_mipi_tx_dev_ctx[cmd_info->id].id);
        return HI_ERR_MIPI_NOT_INIT;
    }

    return mipi_tx_drv_set_cmd_info(cmd_info);
}

static hi_s32 mipi_tx_attr_check(const mipi_attr_pair *attr_pair)
{
    if (attr_pair == HI_NULL) {
        hi_err_mipi("get attr_pair is null.\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    if (attr_pair->attr_type >= ATTR_TYPE_MAX) {
        hi_err_mipi("attr type(%d) err.\n", attr_pair->attr_type);
        return HI_ERR_MIPI_INVALID_PARA;
    }

    if (attr_pair->id >= HI_MIPI_ID_MAX) {
        hi_err_mipi("mipi id(%d) error.\n", attr_pair->id);
        return HI_ERR_MIPI_INVALID_PARA;
    }

    if (g_mipi_tx_dev_ctx[attr_pair->id].id != attr_pair->id) {
        hi_err_mipi("mipi(%d) not initilized.\n", attr_pair->id);
        return HI_ERR_MIPI_NOT_INIT;
    }

    return HI_SUCCESS;
}

static hi_s32 mipi_tx_get_attr(hi_void *arg)
{
    hi_s32 ret;
    hi_drv_mipi_id id;
    mipi_attr_pair *attr_pair = HI_NULL;

    if (arg == HI_NULL) {
        hi_err_mipi("get attr_pair is null.\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    attr_pair = (mipi_attr_pair *)arg;
    ret = mipi_tx_attr_check(attr_pair);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("attr check failed.\n");
        return ret;
    }

    id = attr_pair->id;
    if (attr_pair->attr_type == DSC_STATUS) {
        ; /* dsc check should write a function alone */
        return HI_SUCCESS;
    }

    if (attr_pair->attr_type == BACK_LIGHT) {
        ; /* back light adjustment should write a function alone */
        return HI_SUCCESS;
    }

    if (attr_pair->attr_type == VIDEO_MODE) {
        attr_pair->attr.video_mode = g_mipi_tx_dev_ctx[id].video_mode;
    }

    if (attr_pair->attr_type == OUTPUT_MODE) {
        attr_pair->attr.output_mode = g_mipi_tx_dev_ctx[id].output_mode;
    }

    return HI_SUCCESS;
}

static hi_s32 set_attr_set_attr_help(mipi_attr_pair *attr_pair, hi_bool *set_flag)
{
    hi_drv_mipi_id id;
    hi_mipi_output_mode output_mode;
    *set_flag = HI_FALSE;

    if (attr_pair == HI_NULL || set_flag == HI_NULL) {
        hi_info_mipi("attr same do not need configure.\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    id = attr_pair->id;
    output_mode = attr_pair->attr.output_mode;
    if (attr_pair->attr_type == VIDEO_MODE) {
        if (attr_pair->attr.video_mode == g_mipi_tx_dev_ctx[id].video_mode) {
            hi_info_mipi("attr same do not need configure.\n");
            return HI_SUCCESS;
        }

        /* set video_mode: csi and dsi(cmd) must set video_mode to BURST_MODE, do not support change */
        if (output_mode == OUTPUT_MODE_CSI || output_mode == OUTPUT_MODE_DSI_CMD) {
            hi_info_mipi("do not support now.\n");
            return HI_ERR_MIPI_NOT_SUPPORT;
        }
    }

    if (attr_pair->attr_type == OUTPUT_MODE) {
        if (attr_pair->attr.output_mode == g_mipi_tx_dev_ctx[id].output_mode) {
            hi_info_mipi("attr same do not need configure.\n");
            return HI_SUCCESS;
        }

        /* do not support switch to csi now, only the conversion between dsi and csi is supported */
        if (attr_pair->attr.output_mode == OUTPUT_MODE_CSI) {
            hi_info_mipi("do not support now.\n");
            return HI_ERR_MIPI_NOT_SUPPORT;
        }
    }

    *set_flag = HI_TRUE;
    return HI_SUCCESS;
}

static hi_s32 mipi_tx_set_attr(hi_void *arg)
{
    hi_s32 ret;
    hi_drv_mipi_id id;
    hi_bool set_flag;
    mipi_attr_pair *attr_pair = HI_NULL;
    hi_mipi_output_mode output_mode;

    if (arg == HI_NULL) {
        hi_err_mipi("get attr_pair is null.\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    attr_pair = (mipi_attr_pair *)arg;
    ret = mipi_tx_attr_check(attr_pair);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("attr check failed.\n");
        return ret;
    }

    id = attr_pair->id;
    output_mode = attr_pair->attr.output_mode;
    if (attr_pair->attr_type == DSC_STATUS) {
        ; /* dsc check should write a function alone */
        return HI_SUCCESS; /* not used */
    }

    if (attr_pair->attr_type == BACK_LIGHT) {
        ; /* back light adjustment should write a function alone */
        return HI_SUCCESS;
    }

    set_flag = HI_FALSE;
    ret = set_attr_set_attr_help(attr_pair, &set_flag);
    if (ret != HI_SUCCESS || set_flag != HI_TRUE) {
        return ret;
    }

    /* set output_mode: in the register form, the MODE_CFG itme have only two modes: cmd and video. both csi and
                        dsi(video) must set the MODE_CFG item to video mode, dsi(cmd) must set it to cmd mode;
                        the biggest difference between csi and dsi(video) is they hava different COLOR_CODING(ref to
                        COLOR_CODING item), but the COLOR_CODING come from VDP,can not change, so out_mode can not
                        switch between csi and dsi(video) */
    ret = mipi_tx_drv_set_attr(attr_pair);
    if (ret == HI_SUCCESS) { /* update g_mipi_tx_dev_ctx,need add dsc and backlight */
        if (attr_pair->attr_type == VIDEO_MODE) {
            g_mipi_tx_dev_ctx[id].video_mode = attr_pair->attr.video_mode;
        }

        if (attr_pair->attr_type == OUTPUT_MODE) {
            g_mipi_tx_dev_ctx[id].output_mode = attr_pair->attr.output_mode;
        }
    }

    return ret;
}

static mipi_tx_ioctl_pair g_mipi_tx_ioctl_able[] = {
    { HI_MIPI_TX_GET_ATTR, mipi_tx_get_attr },
    { HI_MIPI_TX_SET_ATTR, mipi_tx_set_attr },
    { HI_MIPI_TX_SET_CMD,  mipi_tx_exec_cmd },
    { 0, HI_NULL },
};

static hi_s32 mipi_open(struct inode *inode, struct file *file)
{
    mipi_func_enter();

    if (inode == HI_NULL) {
        hi_err_mipi("open inode is null\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    if (file == HI_NULL) {
        hi_err_mipi("open file is null\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    mipi_func_exit();

    return HI_SUCCESS;
}

static hi_s32 mipi_release(struct inode *inode, struct file *file)
{
    mipi_func_enter();

    if (inode == HI_NULL) {
        hi_err_mipi("release inode is null\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    if (file == HI_NULL) {
        hi_err_mipi("release file is null\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    mipi_func_exit();
    return HI_SUCCESS;
}

static hi_slong mipi_ioctl_match(struct file *file, hi_u32 cmd, hi_void *arg)
{
    mipi_tx_ioctl_pair *p = g_mipi_tx_ioctl_able;

    if (file == HI_NULL) {
        hi_err_mipi("file is null\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    if (arg == HI_NULL) {
        hi_err_mipi("arg is null\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    for (; p->cmd; p++) {
        if (p->cmd == cmd) {
            return p->func(arg);
        }
    }

    hi_fatal_mipi("unknown ioctl type, cmd = 0x%x", cmd);

    return HI_ERR_MIPI_UNKNOWN_IOCTL_TYPE;
}

static hi_slong mipi_tx_ioctl(struct file *file, hi_u32 cmd, hi_ulong arg)
{
    if (file == HI_NULL) {
        hi_err_mipi("ioctl file is null\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    if (file->f_path.dentry->d_inode == HI_NULL) {
        hi_err_mipi("file->f_path.dentry->d_inode is null\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    return (long)hi_drv_user_copy(file, cmd, arg, mipi_ioctl_match);
}

static struct file_operations g_mipi_fops = {
    .owner          = THIS_MODULE,
    .open           = mipi_open,
    .release        = mipi_release,
    .unlocked_ioctl = mipi_tx_ioctl,
#ifdef CONFIG_COMPAT
    .compat_ioctl   = mipi_tx_ioctl, /* need a different compact_ioctl function */
#endif
};

static hi_s32 mipi_proc_get_help(hi_u32 arg_p, char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    osal_printk("\nUsage as following: \n");
    osal_printk("    cat /proc/msp/mipi                 Display all proc information. \n");
    osal_printk("    echo help > /proc/msp/mipi         Display help infomation for mipi proc module. \n");
    osal_printk("    echo exec_cmd mipi_id cmd_type param_size params > /proc/msp/mipi  \
    exec mipi dcs cmd. \n");
    osal_printk("        For example: echo exec_cmd 0x0 0x23 0x2 0x1 0x2 > /proc/msp/mipi \n");

    osal_printk("\n    Attention:\n");
    osal_printk("        1 Max param_size is 25.\n");
    osal_printk("        2 Input 'cmd_type'&'param_size'&'params' must be take the '0x' prefix in \
    hex format, each param seperate by white space.\n");
    osal_printk("    echo phy_read phy_id phy_len > /proc/msp/mipi \n");
    osal_printk("        For example: echo phy_read 0x0 0x6f > /proc/msp/mipi \n");
    osal_printk("    echo phy_write phy_id phy_addr value > /proc/msp/mipi \n");
    osal_printk("        For example: echo phy_write 0x0 0x6 0x1 > /proc/msp/mipi \n");

    return HI_SUCCESS;
}

static hi_s32 mipi_proc_get_input_param(mipi_cmd_info *cmd_info, hi_char *str)
{
    hi_u32 i;

    if (cmd_info == HI_NULL) {
        osal_printk("cmd_info is null.\n");
        return HI_ERR_MIPI_NULL_PTR;
    }

    cmd_info->in_params = HI_NULL;
    if (cmd_info->in_param_size != 0x0) {
        cmd_info->in_params = kzalloc(cmd_info->in_param_size, GFP_KERNEL);
        if (cmd_info->in_params == HI_NULL) {
            osal_printk("in_params space alloc err.\n");
            return HI_FAILURE;
        }

        for (i = 0; i < cmd_info->in_param_size; i++) {
            cmd_info->in_params[i] = (hi_u32)(osal_strtol(str + i, NULL, 10)); /* 10 is default param */
        }
    }

    return HI_SUCCESS;
}

#define MIPI_PARSE_PROC_LEN 10

static hi_s32 mipi_proc_exec_cmd(hi_u32 arg_p, char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    mipi_cmd_info cmd_info;

    cmd_info.id = (hi_u32)(osal_strtol(arg1_p[1], NULL, MIPI_PARSE_PROC_LEN)); /* 1 is index */
    cmd_info.data_type = (hi_u32)(osal_strtol(arg1_p[2], NULL, MIPI_PARSE_PROC_LEN)); /* 2 is index */
    cmd_info.in_param_size = (hi_u32)(osal_strtol(arg1_p[3], NULL, MIPI_PARSE_PROC_LEN)); /* 3 is index */

    /* get in_params */
    ret = mipi_proc_get_input_param(&cmd_info, arg1_p[4]); /* 4 is index */
    if (ret != HI_SUCCESS) {
        goto out;
    }

    /* out_params */
    cmd_info.out_param_size = 0;
    cmd_info.out_params = HI_NULL;

    ret = mipi_tx_drv_set_cmd_info(&cmd_info);

out:
    if (cmd_info.in_params != HI_NULL) {
        kfree(cmd_info.in_params);
    }

    return ret;
}

static hi_s32 mipi_proc_phy_read(hi_u32 arg_p, char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u32 phy_len, id;

    id = (hi_u32)(osal_strtol(arg1_p[1], NULL, MIPI_PARSE_PROC_LEN)); /* 1 is index */
    phy_len = (hi_u32)(osal_strtol(arg1_p[2], NULL, MIPI_PARSE_PROC_LEN)); /* 2 is index */

    read_phy_reg(id, phy_len);

    return HI_SUCCESS;
}

static hi_s32 mipi_proc_phy_write(hi_u32 arg_p, char (*arg1_p)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u8 value;
    hi_u32 phy_addr, id;

    id = (hi_u32)(osal_strtol(arg1_p[1], NULL, MIPI_PARSE_PROC_LEN)); /* 1 is index */
    phy_addr = (hi_u32)(osal_strtol(arg1_p[2], NULL, MIPI_PARSE_PROC_LEN)); /* 2 is index */
    value = (hi_u32)(osal_strtol(arg1_p[3], NULL, MIPI_PARSE_PROC_LEN)); /* 3 is index */

    set_phy_reg(id, phy_addr, value);

    return HI_SUCCESS;
}

static hi_void mipi_tx_proc_dev_config_show(const mipi_tx_dev_cfg *dev_cfg)
{
    if (dev_cfg == HI_NULL) {
        return;
    }

    osal_printk("%8s%8s%8s%8s%8s%15s%15s%15s%15s%15s%15s\n", "mipi_id", "lane0", "lane1", "lane2",
        "lane3", "output_mode", "video_mode", "output_fmt", "phy_data_rate", "pixel_clk(KHz)", "mipi_mode");

    osal_printk("%8d%8d%8d%8d%8d%15d%15d%15d%15d%15d%15d\n", dev_cfg->id, dev_cfg->lane_id[0x0],
        dev_cfg->lane_id[0x1], dev_cfg->lane_id[0x2], dev_cfg->lane_id[0x3], dev_cfg->output_mode,
        dev_cfg->video_mode, dev_cfg->output_format, dev_cfg->phy_data_rate, dev_cfg->pixel_clk, dev_cfg->mipi_mode);

    osal_printk("\r\n");
}

static hi_void mipi_tx_proc_dev_show(hi_drv_mipi_id id)
{
    mipi_tx_dev_cfg *dev_cfg    = &(g_mipi_tx_dev_ctx[id]);
    mipi_sync_info  *sync_info  = &(g_mipi_tx_dev_ctx[id].sync_info);

    /* mipi tx device config */
    osal_printk("----------MIPI_Tx(%d) DEV CONFIG-------------------------\n", id);
    mipi_tx_proc_dev_config_show(dev_cfg);

    /* mipi tx device sync config */
    osal_printk("----------MIPI_Tx SYNC CONFIG---------------------------\n");

    osal_printk("%14s%14s%14s%14s%14s%14s%14s%14s%14s\n", "pkt_size", "hsa_pixels", "hbp_pixels",
        "hline_pixels", "vsa_lines", "vbp_lines", "vfp_lines", "active_lines", "edpi_cmd_size");

    osal_printk("%14d%14d%14d%14d%14d%14d%14d%14d%14d\n", sync_info->vid_pkt_size, sync_info->vid_hsa_pixels,
        sync_info->vid_hbp_pixels, sync_info->vid_hline_pixels, sync_info->vid_vsa_lines, sync_info->vid_vbp_lines,
        sync_info->vid_vfp_lines, sync_info->vid_active_lines, sync_info->edpi_cmd_size);

    osal_printk("\r\n");

    /* add dsc config */
}

static hi_void mipi_tx_proc_dev_status_show(hi_drv_mipi_id id)
{
    mipi_tx_dev_phy *mipi_phy_ctx = &(g_mipi_tx_phy_ctx_intf[id]);

    osal_printk("----------MIPI_Tx(%d) DEV STATUS-------------------------\n", id);
    osal_printk("%8s%8s%8s%8s\n", "width", "height", "HoriAll", "VertAll");
    osal_printk("%8d%8d%8d%8d\n", mipi_phy_ctx->hact_det, mipi_phy_ctx->vact_det,
        mipi_phy_ctx->hall_det, mipi_phy_ctx->vall_det);
    osal_printk("\r\n");
}

hi_s32 mipi_proc_read(hi_void *s, hi_void *data)
{
    mipi_tx_proc_dev_show(HI_DRV_MIPI_ID_0);
    mipi_tx_proc_dev_show(HI_DRV_MIPI_ID_1);
    mipi_tx_proc_dev_status_show(HI_DRV_MIPI_ID_0);
    mipi_tx_proc_dev_status_show(HI_DRV_MIPI_ID_1);

    return HI_SUCCESS;
}

static osal_proc_cmd g_mipi_proc_info[] = {
    { "help",        mipi_proc_get_help },
    { "exec_cmd",    mipi_proc_exec_cmd },
    { "phy_read",    mipi_proc_phy_read },
    { "phy_write",   mipi_proc_phy_write },
};

static hi_s32 mipi_register_dev(hi_void)
{
    hi_s32 ret;

    ret = alloc_chrdev_region(&g_mipi_devno, 0, 1, "mipi");
    if (ret) {
        hi_fatal_mipi("mipi alloc chrdev region failed, ret is %d\n", ret);
        return HI_FAILURE;
    }

    g_mipi_cdev = cdev_alloc();
    if (IS_ERR(g_mipi_cdev)) {
        hi_fatal_mipi("mipi alloc cdev failed, err(%d)\n", PTR_ERR(g_mipi_cdev));
        ret = HI_FAILURE;
        goto out0;
    }

    cdev_init(g_mipi_cdev, &g_mipi_fops);
    g_mipi_cdev->owner = THIS_MODULE;

    ret = cdev_add(g_mipi_cdev, g_mipi_devno, 1);
    if (ret) {
        hi_fatal_mipi("mipi add cdev failed, ret is %d \n", ret);
        ret = HI_FAILURE;
        goto out1;
    }

    g_mipi_class = class_create(THIS_MODULE, "mipi_class");
    if (IS_ERR(g_mipi_class)) {
        hi_fatal_mipi("mipi create dev class failed, err(%d)\n", PTR_ERR(g_mipi_class));
        ret = HI_FAILURE;
        goto out2;
    }

    g_mipi_dev = device_create(g_mipi_class, HI_NULL, g_mipi_devno, HI_NULL, HI_DEV_MIPI_NAME);
    if (IS_ERR(g_mipi_dev)) {
        hi_fatal_mipi("mipi create dev failed, err(%d)\n", PTR_ERR(g_mipi_dev));
        ret = HI_FAILURE;
        goto out3;
    }

    /* export func for vdp */
    ret = hi_drv_module_register(HI_ID_MIPI, MIPI_NAME, (hi_void *)&g_mipi_export_func, HI_NULL);
    if (ret) {
        hi_fatal_mipi("register demux module failed\n");
        goto out4;
    }

    return HI_SUCCESS;
out4:
    device_destroy(g_mipi_class, g_mipi_devno);
    g_mipi_dev = HI_NULL;
out3:
    class_destroy(g_mipi_class);
    g_mipi_class = HI_NULL;
out2:
    cdev_del(g_mipi_cdev);
out1:
    kfree(g_mipi_cdev);
    g_mipi_cdev = HI_NULL;
out0:
    unregister_chrdev_region(g_mipi_devno, 1);

    return ret;
}

static hi_void mipi_unregister_dev(hi_void)
{
    hi_drv_module_unregister(HI_ID_MIPI);

    device_destroy(g_mipi_class, g_mipi_devno);
    g_mipi_dev = HI_NULL;
    class_destroy(g_mipi_class);
    g_mipi_class = HI_NULL;
    cdev_del(g_mipi_cdev);
    kfree(g_mipi_cdev);
    g_mipi_cdev = HI_NULL;
    unregister_chrdev_region(g_mipi_devno, 1);

    return;
}

static hi_s32 mipi_tx_init(mipi_dev_type dev_type)
{
    return mipi_tx_drv_init(dev_type);
}

static hi_void mipi_tx_exit(hi_void)
{
    mipi_tx_drv_exit();
}

static hi_s32 mipi_get_attr_from_pdm_help(hi_drv_display diplay_id)
{
    hi_s32 ret;
    hi_disp_param disp_param;
    hi_drv_mipi_id id;
    disp_intf_type intf_type;
    hi_pdm_export_func *pdm_funcs = HI_NULL;

    ret = hi_drv_module_get_func(HI_ID_PDM, (hi_void **)&pdm_funcs);
    if (ret != HI_SUCCESS || pdm_funcs->pdm_get_disp_param == HI_NULL) {
        hi_fatal_mipi("hi_drv_module_get_function failed.\n");
        return HI_FAILURE;
    }

    ret = pdm_funcs->pdm_get_disp_param(diplay_id, &disp_param);
    if (ret != HI_SUCCESS) {
        hi_err_mipi("pdm_funcs->pdm_get_disp_param fail, basaparam may not burned.\n");
        return HI_FAILURE;
    }

    intf_type = disp_param.intf[HI_DRV_DISP_INTF_TYPE_MIPI].intf_type;
    if (intf_type != HI_DRV_DISP_INTF_TYPE_MIPI) {
        hi_err_mipi("intf_type( %d) in disp%d is not mipi.\n", intf_type, diplay_id);
        return HI_FAILURE;
    }

    id = disp_param.intf[HI_DRV_DISP_INTF_TYPE_MIPI].un_intf.mipi.mipi_id;
    if (id == HI_MIPI_ID_MAX) {
        hi_err_mipi("mipi id err.\n");
        return HI_FAILURE;
    }

    ret = memcpy_s(&g_mipi_pdm_attr[id].attr_from_pdm, sizeof(hi_drv_mipi_attr_set),
                   &(disp_param.mipi_pdm_attr), sizeof(hi_drv_mipi_attr_set));
    if (ret != HI_SUCCESS) {
        hi_err_mipi("memcpy_s mipi_pdm_attr_set failed.\n");
        return HI_FAILURE;
    }

    g_mipi_pdm_attr[id].id = id;
    return HI_SUCCESS;
}

static hi_s32 mipi_pdm_attr_lane_id_check(hi_u32 index)
{
    hi_s16 i, j;
    hi_s16 not_used_count;
    hi_s16 lane[MAX_LANE_NUM];

    not_used_count = 0;
    for (i = 0; i < MAX_LANE_NUM; i++) {
        lane[i] = g_mipi_pdm_attr[index].attr_from_pdm.lane_id[i];
        if (lane[i] == -1) {
            not_used_count++;
        }
    }

    if (not_used_count == MAX_LANE_NUM) {
        hi_err_mipi("not_used lane is (%d).\n", not_used_count);
        return HI_FAILURE;
    }

    for (i = 0; i < MAX_LANE_NUM; i++) {
        if (lane[i] == -1) {
            continue;
        }

        for (j = 0; j < MAX_LANE_NUM; j++) {
            if (i == j) {
                continue;
            }

            if (lane[i] == lane[j]) {
                hi_err_mipi("lane(%d-%d) same(%d-%d-%d).\n", lane[i], lane[j], i, j, index);
                return HI_FAILURE;
            }
        }
    }

    return HI_SUCCESS;
}

static hi_s32 mipi_pdm_attr_check(hi_void)
{
    hi_u32 i;
    hi_u32 count;
    hi_s32 ret;
    hi_drv_mipi_id id0 = g_mipi_pdm_attr[0].id;
    hi_drv_mipi_id id1 = g_mipi_pdm_attr[1].id;
    hi_drv_mipi_mode mode0 = g_mipi_pdm_attr[0].attr_from_pdm.mipi_mode;
    hi_drv_mipi_mode mode1 = g_mipi_pdm_attr[1].attr_from_pdm.mipi_mode;

    if (id0 == id1) {
        hi_err_mipi("mipi pdm err(%d).\n", id0);
        return HI_FAILURE;
    }

    count = ((id0 == HI_MIPI_ID_MAX) || (id1 == HI_MIPI_ID_MAX)) ? 1 : MAX_CTRL_NUM;
    if (count == 1) {
        i = (id0 != HI_MIPI_ID_MAX) ? id0 : id1;

        ret = mipi_pdm_attr_lane_id_check(i);
        if (ret != HI_SUCCESS) {
            hi_err_mipi("mipi_pdm_attr(%d) check failed.\n", i);
            return HI_FAILURE;
        }
    } else {
        ret = mipi_pdm_attr_lane_id_check(0);
        if (ret != HI_SUCCESS) {
            hi_err_mipi("mipi_pdm_attr0 check failed\n");
            return HI_FAILURE;
        }

        ret = mipi_pdm_attr_lane_id_check(1);
        if (ret != HI_SUCCESS) {
            hi_err_mipi("mipi_pdm_attr1 check failed\n");
            return HI_FAILURE;
        }

        if ((mode0 == HI_DRV_MIPI_MODE_DOUBLE) || (mode1 == HI_DRV_MIPI_MODE_DOUBLE)) {
            hi_err_mipi("pdm mode(%d-%d) check failed\n", mode0, mode1);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

/* need optimize this function later, need check if the value of g_mipi_pdm_attr is invalid */
static hi_s32 mipi_get_attr_from_pdm(hi_void)
{
    hi_s32 ret1, ret2;
    ret1 = mipi_get_attr_from_pdm_help(HI_DRV_DISPLAY_0);
    if (ret1 != HI_SUCCESS) {
        hi_info_mipi("display%d do not have mipi info.\n", HI_DRV_DISPLAY_0);
    }

    ret2 = mipi_get_attr_from_pdm_help(HI_DRV_DISPLAY_1);
    if (ret2 != HI_SUCCESS) {
        hi_info_mipi("display%d do not have mipi info.\n", HI_DRV_DISPLAY_1);
    }

    if (ret1 != HI_SUCCESS && ret2 != HI_SUCCESS) {
        return HI_SUCCESS;
    }

    ret1 = mipi_pdm_attr_check();
    if (ret1 != HI_SUCCESS) {
        hi_err_mipi("mipi_pdm_attr check failed\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 mipi_ctx_init(hi_void)
{
    hi_s32 i;
    hi_s32 ret;

    for (i = 0; i < MAX_CTRL_NUM; i++) {
        g_mipi_pdm_attr[i].id = HI_MIPI_ID_MAX;
    }

    /* need add a function get the output mode from pdm */
    ret = mipi_get_attr_from_pdm(); /* if both mipi controllers used, need get 2 pdm data */
    if (ret != HI_SUCCESS) {
        hi_err_mipi("get pdm data error.\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 mipi_module_init(hi_void)
{
    hi_s32 ret;
    osal_proc_entry *item = HI_NULL;
    mipi_dev_type dev_type;

    mipi_func_enter();

    ret = mipi_register_dev();
    if (ret) {
        hi_fatal_mipi("register mipi device failed\n");
        goto out0;
    }

#ifdef HI_MIPI_TX_PROC_SUPPORT
    item = osal_proc_add("mipi", strlen("mipi"));
    if (item == HI_NULL) {
        hi_err_mipi("add proc mipi failed\n");
    } else { /* need check all mipi satus */
        item->read = mipi_proc_read;
        item->cmd_cnt = sizeof(g_mipi_proc_info) / sizeof(osal_proc_cmd);
        item->cmd_list = g_mipi_proc_info;
        item->private = NULL;
    }

#endif

    ret = mipi_ctx_init();
    if (ret != HI_SUCCESS) {
        hi_err_mipi("mipi_ctx_init error.\n");
        goto out1;
    }

    dev_type = get_dev_type();
    if (dev_type == MIPI_DEV_TYPE_MAX) {
        hi_fatal_mipi("mipi dev type error.\n");
        goto out1;
    }

    /* this func init Interrupt and ref clk, we should confirm wether it is shared by mipi0/1 */
    ret = mipi_tx_init(dev_type);
    if (ret != HI_SUCCESS) {
        hi_fatal_mipi("mipi_tx_init error.\n");
        goto out1;
    }

#ifdef MODULE
    HI_PRINT("Load hi_mipi_tx.ko success. \t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;
out1:
    mipi_unregister_dev();
out0:
    hi_err_mipi("load hi_mipi driver failed!\n");

    mipi_func_exit();
    return HI_FAILURE;
}

static hi_void mipi_module_exit(hi_void)
{
    panel_peripherial_deinit(0);

#ifdef HI_MIPI_TX_PROC_SUPPORT
    osal_proc_remove("mipi", strlen("mipi"));
#endif
    mipi_unregister_dev();
    mipi_tx_exit();

#ifdef MODULE
    HI_PRINT("remove hi_mipi.ko success.\n");
#endif
}

#ifdef MODULE
module_init(mipi_module_init);
module_exit(mipi_module_exit);
#else
hi_s32 drv_mipi_mod_init(hi_void)
{
    return mipi_module_init();
}
EXPORT_SYMBOL(drv_mipi_mod_init);

hi_void drv_mipi_mod_exit(hi_void)
{
    mipi_module_exit();
}
EXPORT_SYMBOL(drv_mipi_mod_exit);
#endif

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("HISILICON");
