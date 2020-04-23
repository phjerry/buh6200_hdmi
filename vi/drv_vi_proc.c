/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: drv vi proc
 * Author: sdk
 * Create: 2019-12-14
 */

#include "hi_type.h"
#include "hi_osal.h"
#include "linux/hisilicon/securec.h"

#include "version.h"
#include "vi_comm.h"
#include "vi_type.h"
#include "vi_hal.h"
#include "vi_utils.h"
#include "vi_uf.h"
#include "vi_string.h"

#include "drv_vi_proc.h"

VI_STATIC hi_s32 vi_proc_save_frame(vi_instance *vi_instance_ctx_p, hi_u32 dump_num,
                                    vi_saveframe_bitwidth save_frame_bit)
{
    hi_s32 ret;
    hi_u32 i;
    vi_buf_node *video_node_p = HI_NULL;
    hi_s8 file_name_str[VI_DUMP_NAME_LEN] = {0};

    if (dump_num == 0) {
        dump_num = 1;
    }

    video_node_p = (vi_buf_node *)osal_vmalloc(HI_ID_VI, sizeof(vi_buf_node));
    if (video_node_p == HI_NULL) {
        vi_drv_log_err("osal_vmalloc fail\n");
        return HI_ERR_VI_NULL_PTR;
    }

    vi_uf_prepare(vi_instance_ctx_p->uf_handle_p, &dump_num);

    for (i = 0; i < dump_num; i++) {
        ret = vi_uf_acquire_frm(vi_instance_ctx_p->uf_handle_p, i, video_node_p);
        if (ret != HI_SUCCESS) {
            vi_drv_log_err("vi_drv_ctrl_user_acquire_frame err:%x, get %d frame\n", ret, i);
            break;
        }

        ret = vi_utils_generate_frame_name(&video_node_p->frame_info, file_name_str, sizeof(file_name_str));
        if (ret != HI_SUCCESS) {
            (hi_void) vi_uf_releasefrm(vi_instance_ctx_p->uf_handle_p, video_node_p);
            vi_drv_log_err("err:0x%x\n", ret);
            break;
        }

        (hi_void) vi_utils_write_frame(file_name_str, video_node_p, save_frame_bit);

        (hi_void) vi_uf_releasefrm(vi_instance_ctx_p->uf_handle_p, video_node_p);
    }

    vi_uf_unprepare(vi_instance_ctx_p->uf_handle_p);

    osal_vfree(HI_ID_VI, video_node_p);

    return HI_SUCCESS;
}

VI_STATIC int vi_proc_cmd_save_frame(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_char *arg1_p = &argv[1][0]; /* param index 1 */
    hi_u32 dump_num = 0;

    if (argc <= 1) { /* param index 1 */
        dump_num = 1;
    } else {
        dump_num = (int)osal_strtol(arg1_p, NULL, OSAL_BASE_DEC);
    }

    return vi_proc_save_frame(private, dump_num, VI_SAVEFRAME_BITWIDTH_8BIT);
}

VI_STATIC int vi_proc_cmd_set_gen_timing(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_char *arg1_p = &argv[1][0]; /* index 1 */
    vi_instance *vi_instance_ctx_p = (vi_instance *)private;
    hi_u32 pattern_type;

    if (argc <= 1) {
        return HI_FAILURE;
    }

    pattern_type = (hi_u32)osal_strtol(arg1_p, NULL, OSAL_BASE_DEC);

    osal_printk("pattern_type=%d\n", pattern_type);

    return vi_hal_set_gen_timing_date(vi_instance_ctx_p->host_handle_p, pattern_type);
}

VI_STATIC int vi_proc_cmd_set_gen_data(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_char *arg1_p = &argv[1][0]; /* index 1 */
    vi_instance *vi_instance_ctx_p = (vi_instance *)private;
    hi_u32 pattern_type;

    if (argc <= 1) {
        return HI_FAILURE;
    }

    pattern_type = (hi_u32)osal_strtol(arg1_p, NULL, OSAL_BASE_DEC);

    osal_printk("pattern_type=%d\n", pattern_type);

    return vi_hal_set_gen_date(vi_instance_ctx_p->host_handle_p, pattern_type);
}

VI_STATIC int vi_proc_cmd_help(unsigned int argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    osal_printk("\n------------------------------------------------------------------------\n");
    osal_printk("echo command   arg1    arg2    > /proc/msp/vixxxx | explaination\n");
    osal_printk("echo help                      > /proc/msp/vi0000 | view help information\n");
    osal_printk("echo saveframe num             > /proc/msp/vi0000 | save output frame [num]\n");

    osal_printk("------------------------------ VI HAL ------------------------------\n");
    osal_printk("echo gentiming     [0-7]           > /proc/msp/vi0000 | generate timing [0:off]\n");
    osal_printk("echo testpattern   [0-7]           > /proc/msp/vi0000 | generate pattern [0:off]\n");
    osal_printk("echo automuteen    enable          > /proc/msp/vi0000 | set auto mute enable\n");
    osal_printk("echo automutethd   cyc hthd vthd   > /proc/msp/vi0000 | %s\n", "set auto mute(cyc:undo mute cycle)");
    osal_printk("echo automuteact   hact vact       > /proc/msp/vi0000 | set auto mute hact and vact\n");

    return HI_SUCCESS;
}

/* clang-format off */
VI_STATIC osal_proc_cmd g_vi_proc_info[] = {
    { "help",           vi_proc_cmd_help },

    { "saveframe",      vi_proc_cmd_save_frame },
    { "gentiming",      vi_proc_cmd_set_gen_timing },
    { "testpattern",    vi_proc_cmd_set_gen_data },
};
/* clang-format on */
static hi_void vi_read_state(hi_void *s, vi_instance *vi_instance_p)
{
    osal_proc_print(s, "--------------------------------------------VI---------------------------------------------\n");

    osal_proc_print(s, "%-40s:%#10X,%#10X,%10s\n", "VI(ViHandle/VoHandle/Status)", vi_instance_p->vi_handle,
                    vi_instance_p->vo_handle, g_vi_instance_str[vi_instance_p->vi_status]);
}

static hi_void vi_read_intf_attr(hi_void *s, vi_instance *vi_instance_p)
{
    hi_vi_attr *attr_orig_p = &vi_instance_p->attr_orig;
    hi_vi_config *config_p = &attr_orig_p->config;
    hi_drv_rect *crop_rect_p = &config_p->crop_rect;
    hi_drv_color_descript *color_descript_p = &attr_orig_p->color_descript;

    osal_proc_print(s, "-------------------------------------------------------------------------------------------\n");

    osal_proc_print(s, "%-40s:%10s,%10s,%10s,%10s,%10s\n", "INTF(type/intf/access/sample/picture)",
                    g_vi_type[attr_orig_p->vi_type], g_vi_intf[attr_orig_p->intf_mode],
                    g_vi_access[attr_orig_p->access_type], g_vi_over_smp[attr_orig_p->over_sample],
                    g_vi_picture_mode[attr_orig_p->picture_mode]);

    osal_proc_print(s, "%-40s:%10s,%10s,%10s,%10s\n", "INTF(source/3d/pixel/bit)",
                    g_vi_source_type[attr_orig_p->source_type], g_vi_3d_fmt[attr_orig_p->video_3d_type],
                    g_vi_pix_fmt[attr_orig_p->pixel_format], g_vi_bit_width[attr_orig_p->bit_width]);

    osal_proc_print(s, "%-40s:%10d,%10d,%10s,%10d,%10d\n", "INTF(width/height/interlace/rate/vblank)", config_p->width,
                    config_p->height, ((config_p->interlace) ? "I" : "P"), config_p->frame_rate, attr_orig_p->vblank);

    osal_proc_print(s, "%-40s:%10s,%10s, (%d,%d,%d,%d)\n", "INTF(sys/field/rect(x,y,w,h)",
                    g_vi_color_sys[config_p->color_sys], g_vi_field_mode[config_p->field_mode], crop_rect_p->rect_x,
                    crop_rect_p->rect_y, crop_rect_p->rect_w, crop_rect_p->rect_h);

    osal_proc_print(
        s, "%-40s:%10s,%10s,%10s,%10s,%10s\n", "INTF(color/range/prim/trans/matrix)",
        g_vi_color_space[color_descript_p->color_space], g_vi_color_quantify_range[color_descript_p->quantify_range],
        g_vi_color_primary[color_descript_p->color_primary], g_vi_color_transfer_curve[color_descript_p->transfer_type],
        g_vi_color_matrix_coeffs[color_descript_p->matrix_coef]);
}

static hi_void vi_read_in_attr(hi_void *s, vi_instance *vi_instance_p)
{
    vi_in_attr *attr_in_p = &vi_instance_p->attr_in;
    hi_drv_color_descript *color_descript_p = &attr_in_p->color_descript;

    osal_proc_print(s, "-------------------------------------------------------------------------------------------\n");

    osal_proc_print(s, "%-40s:%10s,%10s,%10s,%10s,%10s\n", "IN(source/intf/access/sample/hdr)",
                    g_vi_source_type[attr_in_p->source_type], g_vi_intf[attr_in_p->intf_mode],
                    g_vi_access[attr_in_p->access], g_vi_over_smp[attr_in_p->over_sample],
                    g_vi_hdr_type[attr_in_p->hdr_type]);

    osal_proc_print(s, "%-40s:%10d,%10d,%10s,%10d,%10d\n", "IN(width/height/interlace/rate/vblank)", attr_in_p->width,
                    attr_in_p->height, ((attr_in_p->interlace) ? "I" : "P"), attr_in_p->rate, attr_in_p->vblank);

    osal_proc_print(s, "%-40s:%10s,%10s,%10s,%10s\n", "IN(3d/sys/pixel/depth)", g_vi_3d_fmt[attr_in_p->video_3d_type],
                    g_vi_color_sys[attr_in_p->color_sys], g_vi_pix_fmt[attr_in_p->pix_fmt],
                    g_vi_bit_width[attr_in_p->bit_width]);

    osal_proc_print(
        s, "%-40s:%10s,%10s,%10s,%10s,%10s\n", "IN(color/range/prim/trans/matrix)",
        g_vi_color_space[color_descript_p->color_space], g_vi_color_quantify_range[color_descript_p->quantify_range],
        g_vi_color_primary[color_descript_p->color_primary], g_vi_color_transfer_curve[color_descript_p->transfer_type],
        g_vi_color_matrix_coeffs[color_descript_p->matrix_coef]);
}

static hi_void vi_read_out_attr(hi_void *s, vi_instance *vi_instance_p)
{
    vi_out_attr *attr_out_p = &vi_instance_p->attr_out;
    hi_drv_color_descript *color_descript_p = &attr_out_p->color_descript;

    osal_proc_print(s, "-------------------------------------------------------------------------------------------\n");

    osal_proc_print(
        s, "%-40s:%10s,%10s,%10s,%10s,%10s\n", "OUT(color/range/prim/trans/matrix)",
        g_vi_color_space[color_descript_p->color_space], g_vi_color_quantify_range[color_descript_p->quantify_range],
        g_vi_color_primary[color_descript_p->color_primary], g_vi_color_transfer_curve[color_descript_p->transfer_type],
        g_vi_color_matrix_coeffs[color_descript_p->matrix_coef]);

    osal_proc_print(s, "%-40s:%10s,%10s,%10s,%10s,%10s\n", "OUT(hdr_type/3d/sys/pixel/depth)",
                    g_vi_hdr_type[attr_out_p->hdr_type], g_vi_3d_fmt[attr_out_p->video_3d_type],
                    g_vi_color_sys[attr_out_p->color_sys], g_vi_pix_fmt[attr_out_p->pix_fmt],
                    g_vi_bit_width[attr_out_p->bit_width]);

    osal_proc_print(s, "%-40s:%10d,%10d,%10s,%10d\n", "OUT(width/height/interlace/rate)", attr_out_p->width,
                    attr_out_p->height, ((attr_out_p->interlace) ? "I" : "P"), attr_out_p->frame_rate);

    osal_proc_print(s, "%-40s:%10d,%10d\n", "OUT(y_stride/c_stride)", attr_out_p->y_stride, attr_out_p->c_stride);
}

static hi_void vi_read_ctrl_attr(hi_void *s, vi_instance *vi_instance_p)
{
    vi_ctrls_attr *attr_ctrls_p = &vi_instance_p->attr_ctrls;
    hi_drv_rect *crop_rect_p = &attr_ctrls_p->crop_rect;
    hi_vi_nonstd_info *nstd_info_p = &attr_ctrls_p->nstd_info;

    osal_proc_print(s, "-------------------------------------------------------------------------------------------\n");

    osal_proc_print(s, "%-40s:%10s,%10s,%10s,%10s\n", "CTRL(bypassvpss/lowdelay/dolby/hdr)",
                    ((attr_ctrls_p->vpss_bypass) ? "True" : "False"), ((attr_ctrls_p->low_delay) ? "True" : "False"),
                    ((attr_ctrls_p->check_dolby) ? "True" : "False"), ((attr_ctrls_p->check_hdr) ? "True" : "False"));

    osal_proc_print(s, "%-40s:%10s,%10s, (%d,%d,%d,%d)\n", "CTRL(field/range/rect(x,y,w,h))",
                    g_vi_field_mode[attr_ctrls_p->disp_field_mode],
                    g_vi_quantization_range[attr_ctrls_p->quantization_range], crop_rect_p->rect_x, crop_rect_p->rect_y,
                    crop_rect_p->rect_w, crop_rect_p->rect_h);

    if (nstd_info_p->non_std) {
        osal_proc_print(s, "%-40s:%10s,%10d,%10d\n", "CTRL(enable/height/freq)",
                        ((nstd_info_p->non_std) ? "True" : "False"), nstd_info_p->height, nstd_info_p->v_freq);
    }
}

VI_STATIC hi_void vi_read_api_dbg(hi_void *s, vi_instance *vi_instance_p)
{
    hi_vi_dbg_info *dbg_info_p = &vi_instance_p->dbg_data.user_info;
    osal_proc_print(s, "-------------------------------------------------------------------------------------------\n");

    osal_proc_print(s, "%-40s:%10d,%10d,%10d,%10d\n", "thread(pid/tid/exist/run)", dbg_info_p->thread_pid,
                    dbg_info_p->thread_tid, dbg_info_p->thread_exist, dbg_info_p->thread_run);

    osal_proc_print(s, "%-40s:%10d,%10d,%10d,%10d\n", "que(cnt/cost/try/ok)", dbg_info_p->que_thread_cnt,
                    dbg_info_p->que_thread_cost, dbg_info_p->que_win_try, dbg_info_p->que_win_ok);

    osal_proc_print(s, "%-40s:%10d,%10d,%10d,%10d\n", "deq(cnt/cost/try/ok)", dbg_info_p->deq_thread_cnt,
                    dbg_info_p->deq_thread_cost, dbg_info_p->deq_win_try, dbg_info_p->deq_win_ok);

    osal_proc_print(s, "%-40s:%10d,%10d,%10d,%10d\n", "acq(acq_try/acq_ok/rel_try/rel_ok)", dbg_info_p->acq_try,
                    dbg_info_p->acq_ok, dbg_info_p->rel_try, dbg_info_p->rel_ok);
}

VI_STATIC hi_void vi_read_buf_info(hi_void *s, vi_instance *vi_instance_p)
{
    osal_proc_print(s, "-------------------------------------------------------------------------------------------\n");

    vi_buf_proc_print(vi_instance_p->buf_handle_p, s);
}

VI_STATIC hi_s32 vi_drv_proc_read(hi_void *s, hi_void *data)
{
    vi_instance *vi_instance_p = (vi_instance *)data;

    if (s == HI_NULL || data == HI_NULL) {
        vi_drv_log_err("null ptr\n");
        return HI_SUCCESS;
    }

    vi_read_state(s, data);
    vi_read_intf_attr(s, data);
    vi_read_in_attr(s, data);
    vi_read_out_attr(s, data);
    vi_read_ctrl_attr(s, data);
    vi_read_buf_info(s, data);
    vi_read_api_dbg(s, data);

    vi_hal_proc_print(vi_instance_p->host_handle_p, s);

    osal_proc_print(s, "DrvVersion:[%s] %s\n", g_c_drv_verion, VERSION_STRING);

    return HI_SUCCESS;
}

hi_s32 vi_drv_proc_add(vi_instance *instance_p)
{
    hi_s32 len;
    osal_proc_entry *vi_proc_entry = HI_NULL;
    hi_char proc_name[12] = {0}; /* len is 12 */

    len = snprintf_s(proc_name, sizeof(proc_name), sizeof(proc_name) - 1, "vi%04x",
                     vi_hal_get_vicap_id(instance_p->host_handle_p));
    if (len < 0) {
        vi_drv_log_err("snprintf_s failed!\n");
        return HI_FAILURE;
    }

    vi_proc_entry = osal_proc_add(proc_name, strlen(proc_name));
    if (vi_proc_entry == HI_NULL) {
        vi_drv_log_err("VI add proc failed!\n");
        return HI_FAILURE;
    }

    vi_proc_entry->read = vi_drv_proc_read;
    vi_proc_entry->cmd_cnt = sizeof(g_vi_proc_info) / sizeof(osal_proc_cmd);
    vi_proc_entry->cmd_list = g_vi_proc_info;
    vi_proc_entry->private = (hi_void *)instance_p;

    return HI_SUCCESS;
}

hi_void vi_drv_proc_del(vi_instance *instance_p)
{
    hi_s32 len;
    hi_char proc_name[12] = {0}; /* len is 12 */

    len = snprintf_s(proc_name, sizeof(proc_name), sizeof(proc_name) - 1, "vi%04x",
                     vi_hal_get_vicap_id(instance_p->host_handle_p));
    if (len < 0) {
        vi_drv_log_err("snprintf_s failed!\n");
    }

    osal_proc_remove(proc_name, strlen(proc_name));
}
