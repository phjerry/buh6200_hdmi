/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: sdk
 * Create: 2012-04-22
 */

#include "crc_ext.h"
#include "vctrl.h"
#include "dbg.h"
#include "vfmw_osal.h"
#include "linux_proc.h"

#define MAX_CHAN_NUM VFMW_CHAN_NUM

/* --------------------------------- MACRO ------------------------------------ */
#define MAX_FILE_PATH_LEN    (256)
#define MAX_GOLDEN_FRAME_NUM (50 * 1024) /* Max buffer golden frame num */
#define MAX_LOG_LEN          (5 * 1024 * 1024)
#define GOLDEN_TMP_SIZE      (1024)
#define CRC_10BIT_EN         10
#define CRC_2BIT_SIZE        16 /* 2 bit: Min num of bytes of per line */

#ifndef VFMW_KO2_SUPPORT
#define CRC_PROC_NAME "vfmw_crc"
#else
#define CRC_PROC_NAME "vfmw2_crc"
#endif
#define CRC_CMD_NUM (2)
#define CRC_CMD_LEN MAX_FILE_PATH_LEN
typedef hi_u8 (*cmd_str_ptr)[CRC_CMD_NUM][CRC_CMD_LEN];

#define CRC_CMD_OFF        "crc_off"
#define CRC_CMD_MANU_CHECK "crc_manu"
#define CRC_CMD_AUTO_CHECK "crc_auto"
#define CRC_CMD_GENERATE   "crc_gen"
#define CRC_CMD_LOGON      "crc_logon"
#define CRC_CMD_LOGOFF     "crc_logoff"

#define CRC_OVERFLOW "\nLog Overflow!!!\n\n"

/* ---------------------------- LOCAL DEFINITION ------------------------------ */
typedef enum {
    CRC_EXT_2VIEW = 2,
    CRC_EXT_10BIT = 10,
    CRC_EXT_FLAG_BUTT,
} crc_ext_flag;

typedef enum {
    CRC_IDLE_MODE = 0,   /* crc module not enable */
    CRC_MANU_CHECK_MODE, /* check crc without file record */
    CRC_AUTO_CHECK_MODE, /* check crc with file record */
    CRC_GENERATE_MODE,   /* generate crc information */
    CRC_MODE_BUTT,
} crc_mode;

typedef enum {
    CRC_STATE_IDLE = 0,
    CRC_STATE_OPEN,
    CRC_STATE_BUTT,
} crc_state;

typedef struct {
    hi_s32 frame_number;
    hi_u32 frame_offset;
    hi_u32 frame_size;
    hi_s32 frame_type;      /* frame coding type */
    hi_s32 top_field_type;  /* top field coding type */
    hi_s32 btm_field_type;  /* buttom field coding type */
    hi_u32 frame_structure; /* 0-frame ; 1-fieldpair */
    hi_u32 top_field_crc_8bit;
    hi_u32 btm_field_crc_8bit;
    hi_u32 top_field_crc_2bit;
    hi_u32 btm_field_crc_2bit;
    hi_u32 top_field_crc_8bit_1;
    hi_u32 btm_field_crc_8bit_1;
    hi_u32 NVOP_flag;
    hi_s64 frame_pts;
} crc_frame_info;

typedef struct {
    hi_s32 instance_id;
    hi_u32 is_decode_order;
    hi_u32 golden_frame_number;
    hi_u32 Nvop_frame_number;
    hi_u32 error_count_number;
    hi_u32 error_frame_number_8bit;
    hi_u32 error_frame_number_2bit;
    hi_u32 error_frame_number_8bit_1;
    hi_s64 first_pts_offset;
    vfmw_vid_std vid_std;
    OS_FILE *fp_log_file;
    OS_FILE *fp_in_golden_file;
    crc_frame_info golden_frame_info[MAX_GOLDEN_FRAME_NUM];
} crc_check_ctx;

typedef struct {
    hi_u32 out_golden_frame_number;
    OS_FILE *fp_out_golden_file;
} crc_gen_ctx;

typedef struct {
    hi_u32 log_length;
    hi_u32 max_log_length;
    hi_u8 *log_buffer;
} log_recode;

typedef struct {
    hi_s32 instance_id;
    hi_u32 actual_frame_number;
    hi_u32 bit_depth;
    hi_u32 view_number;
    log_recode log_info;

    union {
        crc_check_ctx check_mode;
        crc_gen_ctx gen_mode;
    } unCRC;
} crc_instance_ctx;

typedef struct {
    crc_mode work_mode;
    crc_state instance_state;
    mem_record instance_memory;
    hi_u8 is_decode_order;
    hi_u8 file_path[MAX_FILE_PATH_LEN];
    crc_instance_ctx *instance_ctx;
} crc_instance;

typedef struct {
    crc_mode work_mode;
    hi_u8 log_out;
    hi_u8 is_decode_order;
    hi_u8 file_path[MAX_FILE_PATH_LEN];
} crc_cmd;

/* crc use */
hi_u8 g_CrcMode = 0;
hi_u8 g_CrcOrder = VFMW_DISP_ORDER;
hi_u8 g_CrcStrm[256]; /* 256 :a number */
hi_u8 g_CrcError = 0;
hi_s32 g_CrcNum = 0;
hi_s32 g_MulticrcNum[VFMW_CHAN_NUM] = { 0 };
hi_u8 g_MulticrcStrm[VFMW_CHAN_NUM][256] = { { 0 } }; /* 256 :a number */
hi_s32 g_crc_inst_num = 0;

/* ------------------------------INTERNAL VALUE-------------------------------- */
static hi_s8 g_is_crc_init = CRC_OK;
static crc_instance g_crc_instance[MAX_CHAN_NUM];
static crc_cmd g_crc_cmd;
static OS_SEMA g_crc_sem;

/* ----------------------------INTERNAL FUNCTION------------------------------- */
static hi_u8 *crc_read_line(hi_u8 *buf, hi_u32 bufLen, OS_FILE *fp_src_file)
{
    /* only use fread for linux and liteos */
    hi_s32 ret;
    hi_u8 *str = buf;
    hi_s32 rd_len = 0;

    while (rd_len < bufLen) {
        ret = OS_FREAD(str, 1, fp_src_file);
        if (ret <= 0) {
            break;
        }

        rd_len++;
        if (*str++ == '\n') {
            *str = '\0';
            break;
        }
    }

    if (rd_len <= 0) {
        return HI_NULL;
    }

    return buf;
}

#ifdef VFMW_PROC_SUPPORT
static hi_void crc_help_message(hi_void)
{
    OS_PRINT("\n");
    OS_PRINT("------------------------- HELP INFO ---------------------------\n");
    OS_PRINT("you can run crc module with such commond:echo workmode filepath >/proc/vfmw/crc\n");
    OS_PRINT("\n");
    OS_PRINT("---------------------------------------------------------------\n");

    return;
}

static hi_void CRC_ProcCommandHanlder(hi_u8 cmd[CRC_CMD_NUM][CRC_CMD_LEN])
{
    hi_u32 cmd_number = 0;

    if (!OS_STRNCMP(cmd[cmd_number], CRC_CMD_OFF, CRC_CMD_LEN)) {
        VFMW_CHECK_SEC_FUNC(memset_s(&g_crc_cmd, sizeof(g_crc_cmd), 0, sizeof(g_crc_cmd)));
        return;
    }

    /* Parse work_mode */
    if (!OS_STRNCMP(cmd[cmd_number], CRC_CMD_MANU_CHECK, CRC_CMD_LEN)) {
        g_crc_cmd.work_mode = CRC_MANU_CHECK_MODE;
    } else if (!OS_STRNCMP(cmd[cmd_number], CRC_CMD_AUTO_CHECK, CRC_CMD_LEN)) {
        g_crc_cmd.work_mode = CRC_AUTO_CHECK_MODE;
    } else if (!OS_STRNCMP(cmd[cmd_number], CRC_CMD_GENERATE, CRC_CMD_LEN)) {
        g_crc_cmd.work_mode = CRC_GENERATE_MODE;
    } else if (!OS_STRNCMP(cmd[cmd_number], CRC_CMD_LOGON, CRC_CMD_LEN)) {
        g_crc_cmd.log_out = 1;
        OS_PRINT("CRC log_out Enable\n");
    } else if (!OS_STRNCMP(cmd[cmd_number], CRC_CMD_LOGOFF, CRC_CMD_LEN)) {
        g_crc_cmd.log_out = 0;
        OS_PRINT("CRC log_out Disable\n");
    } else {
        OS_PRINT("Unsupport work mode '%s'!\n", cmd[cmd_number]);
        goto ERROR_EXIT;
    }
    cmd_number++;

    /* Parse File Path */
    if (cmd[cmd_number] != HI_NULL) {
        VFMW_CHECK_SEC_FUNC(memcpy_s(g_crc_cmd.file_path, MAX_FILE_PATH_LEN, cmd[cmd_number], MAX_FILE_PATH_LEN));
        cmd_number++;
    }
    g_CrcMode = g_crc_cmd.work_mode;
    VFMW_CHECK_SEC_FUNC(memcpy_s(g_CrcStrm, sizeof(g_CrcStrm), g_crc_cmd.file_path, MAX_FILE_PATH_LEN));

    if (g_crc_inst_num < VFMW_CHAN_NUM) {
        VFMW_CHECK_SEC_FUNC(memcpy_s(g_MulticrcStrm[g_crc_inst_num],
                                     sizeof(g_MulticrcStrm[g_crc_inst_num]), g_crc_cmd.file_path, MAX_FILE_PATH_LEN));
        g_crc_inst_num++;
    }

    OS_PRINT("Enable CRC (work mode %d) with file %s\n", g_crc_cmd.work_mode, g_crc_cmd.file_path);

    return;

ERROR_EXIT:
    crc_help_message();

    return;
}

hi_s32 CRC_Read_Proc(PROC_FILE *p)
{
    hi_u32 instance_id;
    hi_u32 instance_number = 0;
    crc_instance *instance = HI_NULL;
    hi_s32 page_size = 0;
    hi_s32 used_size = 0;

    if (p == HI_NULL) {
        return CRC_ERR;
    }

    OS_DUMP_PROC(p, page_size, &used_size, 0, "--------------------------- CRC INFO --------------------------\n");

    for (instance_id = 0; instance_id < MAX_CHAN_NUM; instance_id++) {
        instance = &g_crc_instance[instance_id];
        if (instance->work_mode != CRC_IDLE_MODE) {
            instance_number++;
            OS_DUMP_PROC(p, page_size, &used_size, 0,
                         "  Inst: %-2d | State: %-2d | WokeMode: %-2d | file_path: %-30s\n",
                         instance_id, instance->instance_state, g_crc_cmd.work_mode, g_crc_cmd.file_path);
        }
    }

    OS_DUMP_PROC(p, page_size, &used_size, 0, "\n");
    OS_DUMP_PROC(p, page_size, &used_size, 0, "  Total %d instance activated.\n", instance_number);
    OS_DUMP_PROC(p, page_size, &used_size, 0, "  CRC command workmode         : %d\n", g_crc_cmd.work_mode);
    OS_DUMP_PROC(p, page_size, &used_size, 0, "  CRC command logout           : %d\n", g_crc_cmd.log_out);
    if (g_crc_cmd.work_mode != CRC_IDLE_MODE) {
        OS_DUMP_PROC(p, page_size, &used_size, 0, "  CRC command file path        : %s\n", g_crc_cmd.file_path);
    }
    OS_DUMP_PROC(p, page_size, &used_size, 0, "\n");
    OS_DUMP_PROC(p, page_size, &used_size, 0, "---------------------------------------------------------------\n");

    return CRC_OK;
}

static hi_u32 crc_proc_argv_to_cmd_str(hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_s32 argc, cmd_str_ptr cmd_str)
{
    hi_u32 index;
    hi_s32 ret;

    if (argc > PROC_CMD_SINGEL_LENGTH_MAX) {
        return 0;
    }

    if (argc > CRC_CMD_NUM) {
        dprint(PRN_ERROR, "argc:%d > max crc cnt %d\n", argc, CRC_CMD_NUM);
        argc = CRC_CMD_NUM;
    }

    for (index = 0; index < argc; index++) {
        ret = snprintf_s((*cmd_str)[index], CRC_CMD_LEN, PROC_CMD_SINGEL_LENGTH_MAX - 1, *(argv + index));
        if (ret < 0) {
            dprint(PRN_ERROR, "snprintf_s failed ret = %d\n", ret);
            return index;
        }
    }

    return index;
}

static hi_s32 crc_write_proc(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_u8 cmd_string[CRC_CMD_NUM][CRC_CMD_LEN];
    hi_u32 count;

    count = crc_proc_argv_to_cmd_str(argv, argc, &cmd_string);
    if (count == 0) {
        return 0;
    }

    CRC_ProcCommandHanlder(cmd_string);

    return count;
}

static OS_PROC_CMD g_crc_cmd_list[] = {
    {CRC_CMD_OFF, crc_write_proc},
    {CRC_CMD_MANU_CHECK, crc_write_proc},
    {CRC_CMD_AUTO_CHECK, crc_write_proc},
    {CRC_CMD_GENERATE, crc_write_proc},
    {CRC_CMD_LOGON, crc_write_proc},
    {CRC_CMD_LOGOFF, crc_write_proc}
};
#endif

static hi_void crc_reset_golden_info(crc_frame_info *golden_info, hi_u32 len)
{
    hi_u32 i;

    for (i = 0; i < MAX_GOLDEN_FRAME_NUM; i++) {
        VFMW_CHECK_SEC_FUNC(memset_s(&(golden_info[i]), sizeof(crc_frame_info), -1, sizeof(crc_frame_info)));
    }

    return;
}

static hi_s32 crc_frame_type_char_to_SINT32(hi_u8 type)
{
    hi_s32 frame_type;

    if (type == 'I') {
        frame_type = 0;
    } else if (type == 'P') {
        frame_type = 1;
    } else if (type == 'B') {
        frame_type = 2; /* 2 :a number */
    } else {
        frame_type = -1;
    }

    return frame_type;
}

#define crc_read_skip(string) \
    while (*string != ' ')    \
        string++;             \
    while (*string == ' ')    \
        string++;

static hi_s32 crc_read_golden_info(crc_check_ctx *mode_ctx, hi_u32 *total_length)
{
    hi_u32 i;
    hi_s32 ret = CRC_OK;
    hi_u32 total_frame_number = 0;
    hi_u8 *out_memory = HI_NULL;
    hi_u8 *frame_info = HI_NULL;
    hi_u8 type;
    hi_u8 field_type;
    hi_u32 ext_flag;
    hi_u8 *alloc_virtual = HI_NULL;

    *total_length = 0;

    alloc_virtual = OS_ALLOC_VIR("CRC_TMP", GOLDEN_TMP_SIZE);
    if (alloc_virtual == HI_NULL) {
        OS_PRINT("Alloc golden tmp size %d failed!\n", GOLDEN_TMP_SIZE);

        return CRC_ERR;
    }

    out_memory = alloc_virtual;
    VFMW_CHECK_SEC_FUNC(memset_s(out_memory, GOLDEN_TMP_SIZE, 0x00, GOLDEN_TMP_SIZE));
    for (i = 0; i < MAX_GOLDEN_FRAME_NUM; i++) {
        out_memory = crc_read_line(out_memory, GOLDEN_TMP_SIZE, mode_ctx->fp_in_golden_file);
        if (out_memory == HI_NULL) {
            break;
        }

        frame_info = out_memory;
        if (sscanf_s(frame_info, "%d", &(mode_ctx->golden_frame_info[i].frame_number)) <= 0) {
            dprint(PRN_ERROR, "sscanf_s fail!\n");
        }
        crc_read_skip(frame_info);
        if (sscanf_s(frame_info, "%x", &(mode_ctx->golden_frame_info[i].frame_offset)) <= 0) {
            dprint(PRN_ERROR, "sscanf_s fail!\n");
        }
        crc_read_skip(frame_info);
        if (sscanf_s(frame_info, "%d", &(mode_ctx->golden_frame_info[i].frame_size)) <= 0) {
            dprint(PRN_ERROR, "sscanf_s fail!\n");
        }
        crc_read_skip(frame_info);
        if (sscanf_s(frame_info, "%lld", (hi_s64 *)&(mode_ctx->golden_frame_info[i].frame_pts)) <= 0) {
            dprint(PRN_ERROR, "sscanf_s fail!\n");
        }
        crc_read_skip(frame_info);
        if (sscanf_s(frame_info, "%c", &type, 1) <= 0) {
            dprint(PRN_ERROR, "sscanf_s fail!\n");
        }
        mode_ctx->golden_frame_info[i].frame_type = crc_frame_type_char_to_SINT32(type);
        crc_read_skip(frame_info);
        if (sscanf_s(frame_info, "%x", &(mode_ctx->golden_frame_info[i].frame_structure)) <= 0) {
            dprint(PRN_ERROR, "sscanf_s fail!\n");
        }
        crc_read_skip(frame_info);
        if (sscanf_s(frame_info, "%c", &field_type, 1) <= 0) {
            dprint(PRN_ERROR, "sscanf_s fail!\n");
        }
        mode_ctx->golden_frame_info[i].top_field_type = crc_frame_type_char_to_SINT32(field_type);
        frame_info++;
        if (sscanf_s(frame_info, "%c", &field_type, 1) <= 0) {
            dprint(PRN_ERROR, "sscanf_s fail!\n");
        }
        mode_ctx->golden_frame_info[i].btm_field_type = crc_frame_type_char_to_SINT32(field_type);
        crc_read_skip(frame_info);
        if (sscanf_s(frame_info, "%x", &(mode_ctx->golden_frame_info[i].top_field_crc_8bit)) <= 0) {
            dprint(PRN_ERROR, "sscanf_s fail!\n");
        }
        crc_read_skip(frame_info);
        if (sscanf_s(frame_info, "%x", &(mode_ctx->golden_frame_info[i].btm_field_crc_8bit)) <= 0) {
            dprint(PRN_ERROR, "sscanf_s fail!\n");
        }
        crc_read_skip(frame_info);
        if (sscanf_s(frame_info, "%d", &(mode_ctx->golden_frame_info[i].NVOP_flag)) <= 0) {
            dprint(PRN_ERROR, "sscanf_s fail!\n");
        }

        if (frame_info - out_memory > OS_STRLEN(out_memory)) {
            OS_PRINT("ERROR: Frame %d used_length %d (frame_info %p, out_memory %p) > ActLen(out_memory) %d\n",
                     total_frame_number, frame_info - out_memory, frame_info, out_memory, OS_STRLEN(out_memory));
            ret = CRC_ERR;
            break;
        }
        if (OS_STRLEN(frame_info) > CRC_2BIT_SIZE) {
            crc_read_skip(frame_info);
            if (sscanf_s(frame_info, "%d", &ext_flag) <= 0) {
                dprint(PRN_ERROR, "sscanf_s fail!\n");
            }
            if (ext_flag == CRC_EXT_10BIT) {
                crc_read_skip(frame_info);
                if (sscanf_s(frame_info, "%x", &(mode_ctx->golden_frame_info[i].top_field_crc_2bit)) <= 0) {
                    dprint(PRN_ERROR, "sscanf_s fail!\n");
                }
                crc_read_skip(frame_info);
                if (sscanf_s(frame_info, "%x", &(mode_ctx->golden_frame_info[i].btm_field_crc_2bit)) <= 0) {
                    dprint(PRN_ERROR, "sscanf_s fail!\n");
                }
            } else if (ext_flag == CRC_EXT_2VIEW) {
                crc_read_skip(frame_info);
                if (sscanf_s(frame_info, "%x", &(mode_ctx->golden_frame_info[i].top_field_crc_8bit_1)) <= 0) {
                    dprint(PRN_ERROR, "sscanf_s fail!\n");
                }
                crc_read_skip(frame_info);
                if (sscanf_s(frame_info, "%x", &(mode_ctx->golden_frame_info[i].btm_field_crc_8bit_1)) <= 0) {
                    dprint(PRN_ERROR, "sscanf_s fail!\n");
                }
            }
        }

        if (i == 0) {
            mode_ctx->first_pts_offset = mode_ctx->golden_frame_info[i].frame_pts;
        }

        if (mode_ctx->golden_frame_info[i].NVOP_flag == 1) {
            mode_ctx->Nvop_frame_number++;
            mode_ctx->golden_frame_info[i].frame_pts = VFMW_INVALID_PTS;
        } else {
            /* -255 :a number */
            if (mode_ctx->is_decode_order == 0 && mode_ctx->golden_frame_info[i].frame_pts == -255) {
                total_frame_number--;
            }
            mode_ctx->golden_frame_info[i].frame_pts -= mode_ctx->Nvop_frame_number;
            total_frame_number++;
        }
    }

    if (mode_ctx->is_decode_order == 1) {
        mode_ctx->first_pts_offset = 0;
    }
    mode_ctx->golden_frame_number = total_frame_number - mode_ctx->first_pts_offset;
    g_CrcNum = mode_ctx->golden_frame_number;
    g_MulticrcNum[mode_ctx->instance_id] = mode_ctx->golden_frame_number;

    OS_FREE_VIR(alloc_virtual);

    return ret;
}

static hi_void crc_dump_golden_info(crc_check_ctx *mode_ctx)
{
    hi_u32 i;
    crc_frame_info *frame_info = HI_NULL;

    OS_PRINT("Golden frame info:\n");

    OS_PRINT("\n------------------------- GOLDEN INFO -------------------------\n\n");

    for (i = 0; i < MAX_GOLDEN_FRAME_NUM; i++) {
        frame_info = &(mode_ctx->golden_frame_info[i]);
        if (frame_info->frame_number == -1) {
            continue;
        }

        OS_PRINT("frame_number = %d\n", frame_info->frame_number);
        OS_PRINT("frame_offset = %x\n", frame_info->frame_offset);
        OS_PRINT("frame_size = %d\n", frame_info->frame_size);
        OS_PRINT("frame_pts = %ld\n", frame_info->frame_pts);
        OS_PRINT("frame_type = %d\n", frame_info->frame_type);
        OS_PRINT("frame_structure = %x\n", frame_info->frame_structure);
        OS_PRINT("top_field_type = %d\n", frame_info->top_field_type);
        OS_PRINT("btm_field_type = %d\n", frame_info->btm_field_type);
        OS_PRINT("top_field_crc_8bit = %x\n", frame_info->top_field_crc_8bit);
        OS_PRINT("btm_field_crc_8bit = %x\n", frame_info->btm_field_crc_8bit);
        OS_PRINT("top_field_crc_2bit = %x\n", frame_info->top_field_crc_2bit);
        OS_PRINT("btm_field_crc_2bit = %x\n", frame_info->btm_field_crc_2bit);
        OS_PRINT("NVOP_flag = %d\n", frame_info->NVOP_flag);
        OS_PRINT("\n");
    }

    OS_PRINT("\n");
    OS_PRINT("NVOP Frame Num  : %d\n", mode_ctx->Nvop_frame_number);
    OS_PRINT("Golden Frame Num: %d\n", mode_ctx->golden_frame_number);
    OS_PRINT("\n---------------------------------------------------------------\n");

    return;
}

static hi_s32 crc_prepare_golden_info(crc_check_ctx *mode_ctx)
{
    hi_s32 ret;
    hi_u32 TotalLen;

    crc_reset_golden_info(mode_ctx->golden_frame_info, MAX_GOLDEN_FRAME_NUM);

    ret = crc_read_golden_info(mode_ctx, &TotalLen);
    if (ret != CRC_OK) {
        OS_PRINT("Read golden info failed, return: %d\n", ret);
        crc_dump_golden_info(mode_ctx);
        return CRC_ERR;
    }

    return CRC_OK;
}

static hi_void crc_show_to_screen(hi_u8 *error_log, hi_u32 len)
{
    OS_PRINT("%s", error_log);

    return;
}

static hi_void crc_record_to_buffer(crc_instance *instance, hi_u8 *error_log, hi_u32 len)
{
    hi_u8 error_case[256] = { 0 }; /* 256 :a number */
    hi_u8 *log_start = HI_NULL;
    hi_u32 reserved_log_length = sizeof(CRC_OVERFLOW);
    log_recode *log_info = HI_NULL;
    crc_check_ctx *mode_ctx = HI_NULL;

    mode_ctx = &(instance->instance_ctx->unCRC.check_mode);
    log_info = &(instance->instance_ctx->log_info);

    if (mode_ctx->error_count_number == 0) {
        log_start = log_info->log_buffer + log_info->log_length;
        if (snprintf_s(error_case, sizeof(error_case),
                       sizeof(error_case), "\nError Case: %s\n", instance->file_path) < 0) {
            dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
        }
        VFMW_CHECK_SEC_FUNC(memcpy_s(log_start, log_info->max_log_length - log_info->log_length,
                                     error_case, OS_STRLEN(error_case)));
        log_info->log_length += OS_STRLEN(error_case);
    }
    mode_ctx->error_count_number++;

    if (log_info->log_length >= (log_info->max_log_length - reserved_log_length)) {
        log_start = log_info->log_buffer + log_info->log_length;
        if (snprintf_s(error_case, sizeof(error_case), sizeof(error_case), CRC_OVERFLOW) < 0) {
            dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
        }
        VFMW_CHECK_SEC_FUNC(memcpy_s(log_start, log_info->max_log_length - log_info->log_length,
                                     error_case, OS_STRLEN(error_case)));
        log_info->log_length += OS_STRLEN(error_case);
    } else {
        log_start = log_info->log_buffer + log_info->log_length;
        VFMW_CHECK_SEC_FUNC(memcpy_s(log_start, log_info->max_log_length - log_info->log_length,
                                     error_log, OS_STRLEN(error_log)));
        log_info->log_length += OS_STRLEN(error_log);
    }

    return;
}

static hi_void crc_record(crc_instance *instance, const hi_u8 *format, ...)
{
    OS_VA_LIST args;
    hi_s32 total_char;
    hi_u8 error_log[256]; /* 256 :a number */

    OS_VA_START(args, format);
    total_char = vsnprintf_s(error_log, sizeof(error_log), sizeof(error_log) - 1, format, args);
    OS_VA_END(args);

    if ((total_char <= 0) || (total_char >= 256)) { /* 256 :a number */
        return;
    }

    crc_show_to_screen(error_log, 256); /* 256 array size */

    if (instance->work_mode == CRC_AUTO_CHECK_MODE) {
        crc_record_to_buffer(instance, error_log, 256); /* 256 array size */
    }

    return;
}

static hi_void crc_dump_close_log_file(crc_instance *instance)
{
    crc_check_ctx *mode_ctx = HI_NULL;
    hi_s32 pass_frame_number, miss_frame_number;
    hi_u32 pass_rate_h, pass_rate_l;
    hi_u8 result_log_2bit[128]; /* 128 :a number */
    hi_u8 result_log_8bit[128]; /* 128 :a number */
    hi_u8 result_log_8bit_1[128]; /* 128 :a number */

    VFMW_CHECK_SEC_FUNC(memset_s(result_log_2bit, sizeof(result_log_2bit), 0, sizeof(result_log_2bit)));
    VFMW_CHECK_SEC_FUNC(memset_s(result_log_8bit, sizeof(result_log_8bit), 0, sizeof(result_log_8bit)));
    VFMW_CHECK_SEC_FUNC(memset_s(result_log_8bit_1, sizeof(result_log_8bit_1), 0, sizeof(result_log_8bit_1)));
    mode_ctx = &(instance->instance_ctx->unCRC.check_mode);
    miss_frame_number = (mode_ctx->golden_frame_number >=
                         instance->instance_ctx->actual_frame_number)
                        ? mode_ctx->golden_frame_number - instance->instance_ctx->actual_frame_number
                        : 0;

    if (instance->instance_ctx->bit_depth == CRC_EXT_10BIT) {
        pass_frame_number = mode_ctx->golden_frame_number - mode_ctx->error_frame_number_2bit - miss_frame_number;
        pass_rate_h = pass_frame_number * 100 / mode_ctx->golden_frame_number; /* 100 :a number */
        /* 10000 :a number 100 :a number */
        pass_rate_l = pass_frame_number * 10000 / mode_ctx->golden_frame_number % 100;

        if (snprintf_s(result_log_2bit, sizeof(result_log_2bit), sizeof(result_log_2bit),
                       "2bit   PassRate            : %d.%d%%\n2bit   Actual/Golden/Wrong : %d/%d/%d\n",
                       pass_rate_h, pass_rate_l, instance->instance_ctx->actual_frame_number,
                       mode_ctx->golden_frame_number, mode_ctx->error_frame_number_2bit) < 0) {
            dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
        }
        VFMW_CHECK_SEC_FUNC(memcpy_s(result_log_2bit, sizeof(result_log_2bit),
                                     result_log_2bit, OS_STRLEN(result_log_2bit)));
    }

    if (instance->instance_ctx->view_number == CRC_EXT_2VIEW) {
        pass_frame_number = mode_ctx->golden_frame_number - mode_ctx->error_frame_number_8bit_1 - miss_frame_number;
        pass_rate_h = pass_frame_number * 100 / mode_ctx->golden_frame_number; /* 100 :a number */
        /* 10000 :a number 100 :a number */
        pass_rate_l = pass_frame_number * 10000 / mode_ctx->golden_frame_number % 100;

        if (snprintf_s(result_log_8bit_1, sizeof(result_log_8bit_1), sizeof(result_log_8bit_1),
                       "8bit_1 PassRate            : %d.%d%%\n8bit_1 Actual/Golden/Wrong : %d/%d/%d\n",
                       pass_rate_h, pass_rate_l, instance->instance_ctx->actual_frame_number,
                       mode_ctx->golden_frame_number, mode_ctx->error_frame_number_8bit_1) < 0) {
            dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
        }
        VFMW_CHECK_SEC_FUNC(memcpy_s(result_log_8bit_1, sizeof(result_log_8bit_1),
                                     result_log_8bit_1, OS_STRLEN(result_log_8bit_1)));
    }

    pass_frame_number = mode_ctx->golden_frame_number - mode_ctx->error_frame_number_8bit - miss_frame_number;
    pass_rate_h = pass_frame_number * 100 / mode_ctx->golden_frame_number; /* 100 :a number */
    pass_rate_l = pass_frame_number * 10000 / mode_ctx->golden_frame_number % 100; /* 10000 :a number 100 :a number */

    if (snprintf_s(result_log_8bit, sizeof(result_log_8bit), sizeof(result_log_8bit),
                   "8bit   PassRate            : %d.%d%%\n8bit   Actual/Golden/Wrong : %d/%d/%d\n",
                   pass_rate_h, pass_rate_l, instance->instance_ctx->actual_frame_number,
                   mode_ctx->golden_frame_number, mode_ctx->error_frame_number_8bit) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    VFMW_CHECK_SEC_FUNC(memcpy_s(result_log_8bit, sizeof(result_log_8bit),
                                 result_log_8bit, OS_STRLEN(result_log_8bit)));

    OS_PRINT("\n");
    if (mode_ctx->error_frame_number_8bit == 0 &&
        mode_ctx->error_frame_number_2bit == 0 &&
        mode_ctx->error_frame_number_8bit_1 == 0) {
        OS_PRINT(" PassRate            : %d.%d%% \n", pass_rate_h, pass_rate_l);
        OS_PRINT(" Actual/Golden/Wrong : %d/%d/%d\n",
                 instance->instance_ctx->actual_frame_number,
                 mode_ctx->golden_frame_number, mode_ctx->error_frame_number_8bit);
    } else {
        OS_PRINT("%s", result_log_2bit);
        OS_PRINT("%s", result_log_8bit);
        OS_PRINT("%s", result_log_8bit_1);
    }
    OS_PRINT("\n");

    if (mode_ctx->fp_log_file != HI_NULL) {
        if (instance->instance_ctx->log_info.log_buffer != HI_NULL &&
            instance->instance_ctx->log_info.log_length != 0) {
            OS_FWRITE(instance->instance_ctx->log_info.log_buffer, instance->instance_ctx->log_info.log_length,
                      mode_ctx->fp_log_file);

            OS_FWRITE(result_log_2bit, OS_STRLEN(result_log_2bit), mode_ctx->fp_log_file);
            OS_FWRITE(result_log_8bit, OS_STRLEN(result_log_8bit), mode_ctx->fp_log_file);
            OS_FWRITE(result_log_8bit_1, OS_STRLEN(result_log_8bit_1), mode_ctx->fp_log_file);
        }

        OS_FCLOSE(mode_ctx->fp_log_file);
    }

    return;
}

static hi_s32 crc_init_check_ctx(crc_instance *instance)
{
    hi_s32 ret;
    hi_u8 name_string[MAX_FILE_PATH_LEN + 10]; /* 10 :a number */

    crc_check_ctx *mode_ctx = &(instance->instance_ctx->unCRC.check_mode);

    mode_ctx->is_decode_order = instance->is_decode_order;
    mode_ctx->instance_id = instance->instance_ctx->instance_id;
    if (snprintf_s(name_string, sizeof(name_string), sizeof(name_string), "%s.tidx", instance->file_path) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    mode_ctx->fp_in_golden_file = OS_FOPEN(name_string, OS_RDWR, 0);
    if (mode_ctx->fp_in_golden_file == HI_NULL) {
        if (snprintf_s(name_string, sizeof(name_string),
                       sizeof(name_string), "%s.gen.tidx", instance->file_path) < 0) {
            dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
        }
        mode_ctx->fp_in_golden_file = OS_FOPEN(name_string, OS_RDWR, 0);
        if (mode_ctx->fp_in_golden_file == HI_NULL) {
            OS_PRINT("Open file %s failed!\n", name_string);
            return CRC_ERR;
        }
    }

    if (instance->work_mode == CRC_AUTO_CHECK_MODE) {
        if (snprintf_s(name_string, sizeof(name_string), sizeof(name_string), "%s.text", "CrcLog") < 0) {
            dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
        }
        OS_PRINT("Crc-LogFile: %s\n", name_string);
        mode_ctx->fp_log_file = OS_FOPEN(name_string, OS_RDWR | OS_CREATE | OS_APPEND, 0);
        if (mode_ctx->fp_log_file == HI_NULL) {
            OS_PRINT("Open file %s failed!\n", name_string);

            return CRC_ERR;
        }
    }

    ret = crc_prepare_golden_info(mode_ctx);
    if (ret != CRC_OK) {
        crc_record(instance, "Prepare file %s golden info failed!\n", instance->file_path);
        crc_dump_close_log_file(instance);
        OS_FCLOSE(mode_ctx->fp_in_golden_file);

        return CRC_ERR;
    }

    return CRC_OK;
}

static hi_s32 crc_exit_check_ctx(crc_instance *instance)
{
    crc_check_ctx *mode_ctx = HI_NULL;

    mode_ctx = &(instance->instance_ctx->unCRC.check_mode);
#if 0
    if (instance->instance_ctx->actual_frame_number > mode_ctx->golden_frame_number) {
        crc_record(instance, "[Miss]: Actual: %d > Golden %d\n", instance->instance_ctx->actual_frame_number,
                   mode_ctx->golden_frame_number);
    }
#endif
    if (instance->instance_ctx->actual_frame_number < mode_ctx->golden_frame_number) {
        crc_record(instance, "[Miss]: Actual: %d < Golden %d\n", instance->instance_ctx->actual_frame_number,
                   mode_ctx->golden_frame_number);
    }

    crc_dump_close_log_file(instance);

    if (mode_ctx->error_count_number == 0 &&
        instance->instance_ctx->actual_frame_number == mode_ctx->golden_frame_number) {
        g_CrcError = 0;
    }

    OS_FCLOSE(mode_ctx->fp_in_golden_file);

    return CRC_OK;
}

static hi_s32 crc_init_gen_ctx(crc_instance *instance)
{
    hi_u8 name_string[MAX_FILE_PATH_LEN + 10]; /* 10 :a number */

    crc_gen_ctx *golden_ctx = &(instance->instance_ctx->unCRC.gen_mode);

    if (instance->work_mode == CRC_GENERATE_MODE) {
        if (snprintf_s(name_string, sizeof(name_string), \
                       sizeof(name_string), "%s.gen.tidx", instance->file_path) < 0) {
            dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
        }
        golden_ctx->fp_out_golden_file = OS_FOPEN(name_string, OS_WRONLY | OS_CREATE, 0);
        if (golden_ctx->fp_out_golden_file == HI_NULL) {
            OS_PRINT("Open file %s failed!\n", name_string);

            return CRC_ERR;
        }
        golden_ctx->out_golden_frame_number = 0;
    }

    return CRC_OK;
}

static hi_s32 crc_exit_gen_ctx(crc_instance *instance)
{
    crc_gen_ctx *golden_ctx = &(instance->instance_ctx->unCRC.gen_mode);

    if (golden_ctx->fp_out_golden_file != HI_NULL) {
        if (instance->instance_ctx->log_info.log_buffer != HI_NULL) {
            OS_FWRITE(instance->instance_ctx->log_info.log_buffer, instance->instance_ctx->log_info.log_length,
                      golden_ctx->fp_out_golden_file);
        }
        OS_FCLOSE(golden_ctx->fp_out_golden_file);
    }

    return CRC_OK;
}

static hi_s32 crc_inquire_thero_crc(hi_u32 target_frame, crc_check_ctx *mode_ctx,
    hi_u32 *thero_crc_8bit, hi_u32 *thero_crc_2bit, hi_u32 *thero_crc_8bit_1, hi_u32 bit_depth, hi_u32 view_number)
{
    hi_u32 i;
    hi_u32 lead_ignor;
    hi_u32 fix_pts, range;
    hi_s32 thero_pos = 0;

    if (target_frame >= MAX_GOLDEN_FRAME_NUM) {
        OS_PRINT("%s ERROR: target_frame %d >= MAX_GOLDEN_FRAME_NUM %d\n", __func__,
                 target_frame, MAX_GOLDEN_FRAME_NUM);
        return CRC_ERR;
    }

    if (mode_ctx->is_decode_order == 1) {
        thero_crc_8bit[0] = mode_ctx->golden_frame_info[target_frame].top_field_crc_8bit;
        thero_crc_8bit[1] = mode_ctx->golden_frame_info[target_frame].btm_field_crc_8bit;

        if (bit_depth == 10) { /* 10 :a number */
            thero_crc_2bit[0] = mode_ctx->golden_frame_info[target_frame].top_field_crc_2bit;
            thero_crc_2bit[1] = mode_ctx->golden_frame_info[target_frame].btm_field_crc_2bit;
        }

        if (view_number == 2) { /* 2 :a number */
            thero_crc_8bit_1[0] = mode_ctx->golden_frame_info[target_frame].top_field_crc_8bit_1;
            thero_crc_8bit_1[1] = mode_ctx->golden_frame_info[target_frame].btm_field_crc_8bit_1;
        }

        return CRC_OK;
    }

    range = mode_ctx->golden_frame_number + mode_ctx->Nvop_frame_number;
    lead_ignor = mode_ctx->first_pts_offset;
    fix_pts = target_frame + lead_ignor;

    for (i = 0; i <= range; i++) {
        thero_pos = fix_pts + i;
        if (thero_pos < MAX_GOLDEN_FRAME_NUM) {
            if (fix_pts == (hi_u32)mode_ctx->golden_frame_info[thero_pos].frame_pts) {
                if (bit_depth == 10) { /* 10 :a number */
                    thero_crc_2bit[0] = mode_ctx->golden_frame_info[thero_pos].top_field_crc_2bit;
                    thero_crc_2bit[1] = mode_ctx->golden_frame_info[thero_pos].btm_field_crc_2bit;
                }

                if (view_number == 2) { /* 2 :a number */
                    thero_crc_8bit_1[0] = mode_ctx->golden_frame_info[thero_pos].top_field_crc_8bit_1;
                    thero_crc_8bit_1[1] = mode_ctx->golden_frame_info[thero_pos].btm_field_crc_8bit_1;
                }

                thero_crc_8bit[0] = mode_ctx->golden_frame_info[thero_pos].top_field_crc_8bit;
                thero_crc_8bit[1] = mode_ctx->golden_frame_info[thero_pos].btm_field_crc_8bit;

                return CRC_OK;
            }
        }

        thero_pos = fix_pts - i;
        if (thero_pos >= 0) {
            if (fix_pts == (hi_u32)mode_ctx->golden_frame_info[thero_pos].frame_pts) {
                if (bit_depth == 10) { /* 10 :a number */
                    thero_crc_2bit[0] = mode_ctx->golden_frame_info[thero_pos].top_field_crc_2bit;
                    thero_crc_2bit[1] = mode_ctx->golden_frame_info[thero_pos].btm_field_crc_2bit;
                }

                if (view_number == 2) { /* 2 :a number */
                    thero_crc_8bit_1[0] = mode_ctx->golden_frame_info[thero_pos].top_field_crc_8bit_1;
                    thero_crc_8bit_1[1] = mode_ctx->golden_frame_info[thero_pos].btm_field_crc_8bit_1;
                }

                thero_crc_8bit[0] = mode_ctx->golden_frame_info[thero_pos].top_field_crc_8bit;
                thero_crc_8bit[1] = mode_ctx->golden_frame_info[thero_pos].btm_field_crc_8bit;

                return CRC_OK;
            }
        }
    }

    return CRC_ERR;
}

static hi_s32 crc_inquire(crc_instance *instance, hi_u32 *thero_crc_8bit, hi_u32 *thero_crc_2bit,
                          hi_u32 *pTheroCrc_1_8bit)
{
    hi_s32 ret;
    hi_u32 bit_depth = instance->instance_ctx->bit_depth;
    hi_u32 view_number = instance->instance_ctx->view_number;
    hi_u32 target_frame = instance->instance_ctx->actual_frame_number;
    crc_check_ctx *mode_ctx = &(instance->instance_ctx->unCRC.check_mode);

    ret = crc_inquire_thero_crc(target_frame, mode_ctx, thero_crc_8bit, thero_crc_2bit, pTheroCrc_1_8bit, bit_depth,
                                view_number);
    if (ret != CRC_OK) {
        OS_PRINT("Inquire target_frame %d TheroCrc failed!\n", target_frame);
        return CRC_ERR;
    }

    return CRC_OK;
}

static hi_s32 crc_compare(hi_s32 *thero_crc, hi_u32 thero_len, hi_s32 *actul_crc, hi_u32 actul_len, hi_s32 format)
{
    hi_s32 ret = CRC_ERR;

    /* top and bottom */
    if (format == 3) { /* 3 :a number */
        if ((thero_crc[0] == actul_crc[0]) && (thero_crc[1] == actul_crc[1])) {
            ret = CRC_OK;
        }
    } else if (format == 2) { /* bottom field only  2 :a number */
        if ((thero_crc[1] == actul_crc[1])) {
            ret = CRC_OK;
        }
    } else if (format == 1) { /* top field only */
        if ((thero_crc[0] == actul_crc[0])) {
            ret = CRC_OK;
        }
    } else {
        OS_PRINT("CompareCrc: format %d invalid!\n", format);
    }

    return ret;
}

static hi_s32 crc_gen_golden_crc(crc_instance *instance, vfmw_image *image)
{
    hi_u8 tidx_info[256]; /* 256 :a number */
    hi_u32 used_length;
    hi_u8 *golden_frame = HI_NULL;
    hi_u32 *actul_crc_8bit = HI_NULL;
    hi_u32 *actul_crc_2bit = HI_NULL;
    hi_u32 *actul_crc_8bit_1 = HI_NULL;
    crc_gen_ctx *mode_ctx = HI_NULL;
    log_recode *golden_frame_info = HI_NULL;
    hi_u32 reserved_log_length = sizeof(CRC_OVERFLOW);

    mode_ctx = &(instance->instance_ctx->unCRC.gen_mode);
    golden_frame_info = &(instance->instance_ctx->log_info);

    if (mode_ctx->fp_out_golden_file == HI_NULL) {
        return CRC_ERR;
    }

    actul_crc_8bit = image->crc_info.actual_crc_8bit;
    actul_crc_2bit = image->crc_info.actual_crc_2bit;
    actul_crc_8bit_1 = image->crc_info.actual_crc_8bit_1;

    VFMW_CHECK_SEC_FUNC(memset_s(tidx_info, sizeof(tidx_info), 0, sizeof(tidx_info)));

    if (golden_frame_info->log_length >= (golden_frame_info->max_log_length - reserved_log_length)) {
        golden_frame = golden_frame_info->log_buffer + golden_frame_info->log_length;
        if (snprintf_s(tidx_info, sizeof(tidx_info), sizeof(tidx_info), CRC_OVERFLOW) < 0) {
            dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
        }
        VFMW_CHECK_SEC_FUNC(memcpy_s(golden_frame,
                                     golden_frame_info->max_log_length - golden_frame_info->log_length,
                                     tidx_info, OS_STRLEN(tidx_info)));
        golden_frame_info->log_length += OS_STRLEN(tidx_info);
    } else {
        golden_frame = golden_frame_info->log_buffer + golden_frame_info->log_length;

        if (snprintf_s(tidx_info, sizeof(tidx_info), sizeof(tidx_info),
                       "%-4d %-4d %-4d %-4d %-4d %-4d %-4d %-8x %-8x %-4d ",
                       mode_ctx->out_golden_frame_number, 0, 0, mode_ctx->out_golden_frame_number,
                       0, 0, 0, actul_crc_8bit[0], actul_crc_8bit[1], 0) < 0) {
            dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
        }
        used_length = OS_STRLEN(tidx_info);

        if (instance->instance_ctx->bit_depth == CRC_EXT_10BIT) {
            if (snprintf_s(tidx_info + used_length, sizeof(tidx_info) - used_length,
                           /* 10 :a number */
                           sizeof(tidx_info) - used_length, "%-4d %-8x %-8x ", 10, actul_crc_2bit[0],
                           actul_crc_2bit[1]) < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            used_length = OS_STRLEN(tidx_info);
        }

        if (instance->instance_ctx->view_number == CRC_EXT_2VIEW) {
            if (snprintf_s(tidx_info + used_length, sizeof(tidx_info) - used_length,
                           /* 2 :a number */
                           sizeof(tidx_info) - used_length, "%-4d %-8x %-8x ", 2, actul_crc_8bit_1[0],
                           actul_crc_8bit_1[1]) < 0) {
                dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
            }
            used_length = OS_STRLEN(tidx_info);
        }

        if (snprintf_s(tidx_info + used_length, sizeof(tidx_info) - used_length,
                       sizeof(tidx_info) - used_length, "\n") < 0) {
            dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
        }

        VFMW_CHECK_SEC_FUNC(memcpy_s(golden_frame,
                                     golden_frame_info->max_log_length - golden_frame_info->log_length,
                                     tidx_info, OS_STRLEN(tidx_info)));
        golden_frame_info->log_length += OS_STRLEN(tidx_info);
    }

    OS_PRINT("Generating golden crc info: frame %-5d\n", mode_ctx->out_golden_frame_number);

    mode_ctx->out_golden_frame_number++;

    return CRC_OK;
}

static hi_s32 crc_check_image_crc(crc_instance *instance, vfmw_image *image)
{
    hi_s32 ret;
    hi_s32 ret_2bit = CRC_OK;
    hi_s32 ret_8bit = CRC_OK;
    hi_s32 ret_8bit_1 = CRC_OK;
    hi_u32 crc_format;
    hi_u32 thero_crc_8bit[2]; /* 2 :a number */
    hi_u32 thero_crc_8bit_1[2]; /* 2 :a number */
    hi_u32 thero_crc_2bit[2]; /* 2 :a number */
    hi_u32 *actul_crc_8bit = HI_NULL;
    hi_u32 *actul_crc_2bit = HI_NULL;
    hi_u32 *actul_crc_8bit_1 = HI_NULL;
    crc_check_ctx *mode_ctx = HI_NULL;

    crc_format = image->format.field_valid_flag;
    actul_crc_8bit = image->crc_info.actual_crc_8bit;
    actul_crc_2bit = image->crc_info.actual_crc_2bit;
    actul_crc_8bit_1 = image->crc_info.actual_crc_8bit_1;

    mode_ctx = &(instance->instance_ctx->unCRC.check_mode);
    if (instance->instance_ctx->actual_frame_number > mode_ctx->golden_frame_number) {
        OS_PRINT("ChanId %d [Wrong]: actual_frame_number %d more than golden_frame_number %d!\n",
                 instance->instance_ctx->instance_id,
                 instance->instance_ctx->actual_frame_number, mode_ctx->golden_frame_number);

        return CRC_ERR;
    }

    ret = crc_inquire(instance, thero_crc_8bit, thero_crc_2bit, thero_crc_8bit_1);
    if (ret != CRC_OK) {
        return CRC_ERR;
    }

    ret_8bit = crc_compare(thero_crc_8bit, 2, actul_crc_8bit, 2, crc_format); /* 2 is array size */
    if (ret_8bit != CRC_OK) {
        mode_ctx->error_frame_number_8bit++;
        crc_record(instance,
                   "ChanId %d [Wrong]: No.%d/%d, ViewID = %d, format = %d, 8bit Error(A:%9x,%9x | T:%9x,%9x)\n",
                   instance->instance_ctx->instance_id, instance->instance_ctx->actual_frame_number,
                   mode_ctx->golden_frame_number - 1, image->view_id,
                   crc_format, actul_crc_8bit[0], actul_crc_8bit[1], thero_crc_8bit[0], thero_crc_8bit[1]);
    }

    if (instance->instance_ctx->bit_depth == CRC_EXT_10BIT) {
        ret_2bit = crc_compare(thero_crc_2bit, 2, actul_crc_2bit, 2, crc_format); /* 2 is array size */
        if (ret_2bit != CRC_OK) {
            mode_ctx->error_frame_number_2bit++;

            crc_record(instance,
                       "ChanId %d [Wrong]: No.%d/%d, ViewID = %d, format = %d, 2bit Error(A:%9x,%9x | T:%9x,%9x)\n",
                       instance->instance_ctx->instance_id, instance->instance_ctx->actual_frame_number,
                       mode_ctx->golden_frame_number - 1, image->view_id,
                       crc_format, actul_crc_2bit[0], actul_crc_2bit[1], thero_crc_2bit[0], thero_crc_2bit[1]);
        }
    }

    if (instance->instance_ctx->view_number == CRC_EXT_2VIEW) {
        ret_8bit_1 = crc_compare(thero_crc_8bit_1, 2, actul_crc_8bit_1, 2, crc_format); /* 2 is array size */
        if (ret_8bit_1 != CRC_OK) {
            mode_ctx->error_frame_number_8bit_1++;

            crc_record(instance,
                       "ChanId %d [Wrong]: No.%d/%d, ViewID = %d, format = %d, 8bit_1 Error(A:%9x,%9x | T:%9x,%9x)\n",
                       instance->instance_ctx->instance_id, instance->instance_ctx->actual_frame_number,
                       mode_ctx->golden_frame_number - 1, image->view_id,
                       crc_format, actul_crc_8bit_1[0], actul_crc_8bit_1[1], thero_crc_8bit_1[0], thero_crc_8bit_1[1]);
        }
    }

    if (ret_8bit != CRC_OK || ret_2bit != CRC_OK || ret_8bit_1 != CRC_OK) {
        return CRC_ERR;
    } else {
        if (g_crc_cmd.log_out != 0) {
            OS_PRINT("ChanId %d [OK]: No.%d/%d\n", instance->instance_ctx->instance_id,
                     instance->instance_ctx->actual_frame_number,
                     mode_ctx->golden_frame_number - 1);
        }

        return CRC_OK;
    }
}

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
hi_void crc_init(hi_void)
{
    if (g_is_crc_init == CRC_TRUE) {
        return;
    }

    VFMW_CHECK_SEC_FUNC(memset_s(g_crc_instance,
                                 MAX_CHAN_NUM * sizeof(crc_instance), 0, MAX_CHAN_NUM * sizeof(crc_instance)));
    VFMW_CHECK_SEC_FUNC(memset_s(&g_crc_cmd, sizeof(crc_cmd), 0, sizeof(crc_cmd)));
    VFMW_CHECK_SEC_FUNC(memset_s(g_MulticrcStrm, sizeof(g_MulticrcStrm), 0, sizeof(g_MulticrcStrm)));
    VFMW_CHECK_SEC_FUNC(memset_s(g_MulticrcNum, sizeof(g_MulticrcNum), 0, sizeof(g_MulticrcNum)));
    g_crc_inst_num = 0;

#ifdef VFMW_PROC_SUPPORT
    if (OS_PROC_CREATE(CRC_PROC_NAME, CRC_Read_Proc,
        g_crc_cmd_list, sizeof(g_crc_cmd_list) / sizeof(OS_PROC_CMD)) != OSAL_OK) {
        OS_PRINT("%s create proc %s failed!\n", __func__, CRC_PROC_NAME);
        return;
    }
#endif
    OS_SEMA_INIT(&g_crc_sem);

    g_is_crc_init = CRC_TRUE;

    return;
}

hi_void crc_exit(hi_void)
{
    OS_PROC_DESTROY(CRC_PROC_NAME);
    OS_SEMA_EXIT(g_crc_sem);

    g_is_crc_init = CRC_FLASE;

    return;
}

hi_s32 crc_create_instance(hi_s32 instance_id)
{
    hi_s32 ret;
    hi_u8 name_string[20]; /* 20 :a number */
    crc_instance *instance = HI_NULL;
    crc_instance_ctx *instance_ctx = HI_NULL;

    VFMW_ASSERT_RET(g_is_crc_init == CRC_TRUE, CRC_ERR);
    VFMW_ASSERT_RET(instance_id >= 0 && instance_id < MAX_CHAN_NUM, CRC_ERR);

    OS_SEMA_DOWN(g_crc_sem);

    instance = &g_crc_instance[instance_id];
#if 0
    instance->work_mode = g_crc_cmd.work_mode;
    instance->is_decode_order = g_crc_cmd.is_decode_order;
    VFMW_CHECK_SEC_FUNC(memcpy_s(instance->file_path, g_crc_cmd.file_path, MAX_FILE_PATH_LEN);
#else
    instance->work_mode = g_CrcMode;
    instance->is_decode_order = g_CrcOrder;
    VFMW_CHECK_SEC_FUNC(memcpy_s(instance->file_path, sizeof(instance->file_path),
                                 g_CrcStrm, MAX_FILE_PATH_LEN));
    if (g_MulticrcStrm[instance_id][0] != 0x00) {
        VFMW_CHECK_SEC_FUNC(memcpy_s(instance->file_path, sizeof(instance->file_path),
                                     g_MulticrcStrm[instance_id], MAX_FILE_PATH_LEN));
    }
#endif

    if (instance->work_mode == CRC_IDLE_MODE) {
    ret = CRC_OK;
    goto EXIT;
}

if (instance->instance_state != CRC_STATE_IDLE) {
    OS_PRINT("Create inst %d failed for invalid state %d!\n", instance_id, instance->instance_state);
        ret = CRC_ERR;
        goto EXIT;
    }

    if (snprintf_s(name_string, sizeof(name_string), sizeof(name_string), "CRC_INST_%d", instance_id) < 0) {
    dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    instance->instance_memory.vir_addr = OS_ALLOC_VIR("CRC_Inst", sizeof(crc_instance_ctx) + MAX_LOG_LEN);
    if (instance->instance_memory.vir_addr == HI_NULL) {
    OS_PRINT("Inst %d alloc context(%d) failed!\n", instance_id, sizeof(crc_instance_ctx));
        ret = CRC_ERR;
        goto EXIT;
    }

    instance->instance_ctx = (crc_instance_ctx *)instance->instance_memory.vir_addr;
    instance_ctx = instance->instance_ctx;
    VFMW_CHECK_SEC_FUNC(memset_s(instance_ctx, sizeof(crc_instance_ctx), 0, sizeof(crc_instance_ctx)));

    instance_ctx->instance_id = instance_id;
    instance_ctx->log_info.log_buffer = (instance->instance_memory.vir_addr +
                                        sizeof(crc_instance_ctx));
    instance_ctx->log_info.max_log_length = MAX_LOG_LEN;
    instance_ctx->log_info.log_length = 0;

    if (instance->work_mode == CRC_MANU_CHECK_MODE || instance->work_mode == CRC_AUTO_CHECK_MODE) {
    ret = crc_init_check_ctx(instance);
    } else if (instance->work_mode == CRC_GENERATE_MODE) {
    ret = crc_init_gen_ctx(instance);
    } else {
        OS_PRINT("%s inst %d not suppose to be here with work mode %d!\n", __func__, instance_id, instance->work_mode);
        ret = CRC_ERR;
    }

    if (ret == CRC_OK) {
    instance->instance_state = CRC_STATE_OPEN;
} else {
    OS_FREE_VIR(instance->instance_memory.vir_addr);
        VFMW_CHECK_SEC_FUNC(memset_s(instance, sizeof(crc_instance), 0, sizeof(crc_instance)));
    }

    EXIT:
    OS_SEMA_UP(g_crc_sem);

    return ret;
}

hi_s32 crc_destroy_instance(hi_s32 instance_id)
{
    hi_s32 ret;
    crc_instance *instance = HI_NULL;

    VFMW_ASSERT_RET(g_is_crc_init == CRC_TRUE, CRC_ERR);
    VFMW_ASSERT_RET(instance_id >= 0 && instance_id < MAX_CHAN_NUM, CRC_ERR);

    OS_SEMA_DOWN(g_crc_sem);

    instance = &g_crc_instance[instance_id];

    if (instance->work_mode == CRC_IDLE_MODE) {
        ret = CRC_OK;
        goto EXIT;
    }

    if (instance->instance_state == CRC_STATE_IDLE) {
        ret = CRC_OK;
        OS_PRINT("%s inst %d already destroyed.\n", __func__, instance_id);
        goto EXIT;
    }

    if (instance->work_mode == CRC_MANU_CHECK_MODE || instance->work_mode == CRC_AUTO_CHECK_MODE) {
        ret = crc_exit_check_ctx(instance);
    } else if (instance->work_mode == CRC_GENERATE_MODE) {
        ret = crc_exit_gen_ctx(instance);
    } else {
        OS_PRINT("%s inst %d not suppose to be here with work mode %d!\n", __func__, instance_id, instance->work_mode);
        ret = CRC_ERR;
    }

    if (instance->instance_memory.vir_addr != HI_NULL) {
        OS_FREE_VIR(instance->instance_memory.vir_addr);
    } else {
        OS_PRINT("%s inst %d mem phy 0x%x/vir %p invalid!\n", __func__, instance->instance_memory.phy_addr,
                 instance->instance_memory.vir_addr);
        ret = CRC_ERR;
    }

    VFMW_CHECK_SEC_FUNC(memset_s(instance, sizeof(crc_instance), 0, sizeof(crc_instance)));

EXIT:
    g_crc_cmd.work_mode = CRC_IDLE_MODE;

    OS_SEMA_UP(g_crc_sem);

    return ret;
}

hi_s32 crc_set_config(hi_s32 instance_id, vfmw_chan_cfg *config)
{
    crc_instance *instance = HI_NULL;

    VFMW_ASSERT_RET(g_is_crc_init == CRC_TRUE, CRC_ERR);
    VFMW_ASSERT_RET(instance_id >= 0 && instance_id < MAX_CHAN_NUM, CRC_ERR);
    VFMW_ASSERT_RET(config != HI_NULL, CRC_ERR);

    instance = &g_crc_instance[instance_id];

    if (instance->work_mode != CRC_IDLE_MODE) {
        config->crc_mode = instance->work_mode;
        config->v_cmp_mode = VFMW_CMP_OFF;  /* force not compress */
        if (instance->work_mode == CRC_MANU_CHECK_MODE || instance->work_mode == CRC_AUTO_CHECK_MODE) {
            instance->instance_ctx->unCRC.check_mode.vid_std = config->vid_std;
            /* AVS2Э飬CRCģʽ²ܴ֡ڽ֡Ĳʾ⴦ */
            if (config->vid_std == VFMW_AVS2) {
                if (instance->instance_ctx->unCRC.check_mode.first_pts_offset == -255) { /* -255 :a number */
                    instance->instance_ctx->unCRC.check_mode.golden_frame_number -= 255; /* 255 :a number */
                    instance->instance_ctx->unCRC.check_mode.first_pts_offset = 0;
                    g_CrcNum = instance->instance_ctx->unCRC.check_mode.golden_frame_number;
                    g_MulticrcNum[instance_id] = instance->instance_ctx->unCRC.check_mode.golden_frame_number;
                }
            }
        }
    }

    return CRC_OK;
}

hi_s32 crc_put_image(hi_s32 instance_id, vfmw_image *image)
{
    hi_s32 ret;
    crc_instance *instance = HI_NULL;

    VFMW_ASSERT_RET(g_is_crc_init == CRC_TRUE, CRC_ERR);
    VFMW_ASSERT_RET(instance_id >= 0 && instance_id < MAX_CHAN_NUM, CRC_ERR);
    VFMW_ASSERT_RET(image != HI_NULL, CRC_ERR);

    instance = &g_crc_instance[instance_id];
    if (instance->work_mode == CRC_IDLE_MODE) {
        return CRC_OK;
    }

    if (instance->instance_state == CRC_STATE_IDLE) {
        OS_PRINT("%s inst %d should not in idle state.\n", __func__, instance_id, instance->instance_state);
        return CRC_ERR;
    }

    crc_calc_image_crc(image);

    instance->instance_ctx->bit_depth = image->bit_depth;
    instance->instance_ctx->view_number = (image->is_3d == 1) ? 2 : 1; /* 2 :a number */

    if (instance->work_mode == CRC_GENERATE_MODE) {
        ret = crc_gen_golden_crc(instance, image);
    } else {
        ret = crc_check_image_crc(instance, image);
    }

    instance->instance_ctx->actual_frame_number++;

    return ret;
}


