#include "drv_vdec_proc.h"
#include "drv_vdec.h"
#include "drv_vdec_osal.h"
#include "drv_vdec_sdk.h"

typedef struct {
    hi_u32 cur_chan;
} vdec_proc_entry;

static vdec_proc_entry g_vdec_proc_entry = {0};

static vdec_proc_entry* vdec_proc_get_entry(hi_void)
{
    return &g_vdec_proc_entry;
}

static hi_s32 vdec_string_to_value(hi_char *str, hi_u32 str_len, hi_u32 *data)
{
    hi_u32 i, d, dat, weight;

    if (str_len < 10) { /* 10 :a number */
        return HI_FAILURE;
    }

    dat = 0;
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        i = 2;  /* 2 :a number */
        weight = 16;    /* 16 :a number */
    } else {
        i = 0;
        weight = 10;    /* 10 :a number */
    }

    for (; i < 10; i++) {   /* 10 :a number */
        if (str[i] < 0x20) {
            break;
        } else if (weight == 16 && str[i] >= 'a' && str[i] <= 'f') {    /* 16 :a number */
            d = str[i] - 'a' + 10;  /* 10 :a number */
        } else if (weight == 16 && str[i] >= 'A' && str[i] <= 'F') {    /* 16 :a number */
            d = str[i] - 'A' + 10;  /* 10 :a number */
        } else if (str[i] >= '0' && str[i] <= '9') {
            d = str[i] - '0';
        } else {
            return HI_FAILURE;
        }

        dat = dat * weight + d;
    }

    *data = dat;

    return HI_SUCCESS;
}

static hi_void vdec_proc_help_print(hi_void)
{
    HI_PRINT(VDEC_HELP_FMT, "Command", "Value", "Target", "Comment");
    HI_PRINT(VDEC_HELP_FMT, VDEC_PROC_CMD_SET_CHAN, "<chan>", "vdec", "set current proc chan");
}

static hi_s32 vdec_proc_set_chan(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_s32 ret;
    hi_u32 value = 0;
    vdec_proc_entry *entry = vdec_proc_get_entry();

    ret = vdec_string_to_value(argv[1], PROC_CMD_SINGEL_LENGTH_MAX, &value); /* 1: arg idx */
    if (ret != HI_SUCCESS) {
        VDEC_PRINT_ERR("vdec_string_to_value err\n");
        return HI_FAILURE;
    }

    entry->cur_chan = value;
    HI_PRINT("set proc chan %d\n", entry->cur_chan);

    return HI_SUCCESS;
}

static hi_s32 vdec_proc_print_help(hi_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    vdec_proc_help_print();

    return HI_SUCCESS;
}

static osal_proc_cmd g_vdec_proc_cmd_tbl[] = {
    { VDEC_PROC_CMD_SET_CHAN, vdec_proc_set_chan },
    { VDEC_PROC_CMD_HELP, vdec_proc_print_help },
};

hi_s32 vdec_proc_get_cmd(hi_void **cmd_list, hi_u32 *cmd_cnt)
{
    if ((cmd_list == HI_NULL) || (cmd_cnt == HI_NULL)) {
        VDEC_PRINT_ERR("args is null\n");
        return HI_FAILURE;
    }

    *cmd_list = g_vdec_proc_cmd_tbl;
    *cmd_cnt = sizeof(g_vdec_proc_cmd_tbl) / sizeof(osal_proc_cmd);

    return HI_SUCCESS;
}

hi_s32 vdec_proc_read(hi_void *fp)
{
    vdec_proc_entry *entry = vdec_proc_get_entry();

    PROC_PRINT(fp, VP_D, "cur_proc_chan", entry->cur_chan);
    drv_vdec_read_proc(fp, entry->cur_chan);

    return HI_SUCCESS;
}

