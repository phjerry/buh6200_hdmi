/* Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved
* Description :Add proc node in virtual fs.
* Author : Linux SDK team
* Created : 2019-06-21
*/
#include "drv_otp_proc.h"

#include "hi_drv_dev.h"
#include "drv_otp_define.h"
#include "drv_ioctl_otp.h"
#include "drv_otp.h"
#include "hal_otp.h"

static hi_void _otp_proc_get_help(hi_void);

#define OTP_FUSE_MAX     0X1000

/* ****** proc virtualotp begin ******* */
typedef enum {
    DRV_OTP_NO_FAKE_FLAG               = 0X00,
    DRV_OTP_GET_FAKE_BUFFER_FLAG       = 0X01,
    DRV_OTP_FAKE_BUFFER_FLAG           = 0X10,
    DRV_OTP_FAKE_BUFFER_RESET_FLAG     = 0X11,
    DRV_OTP_FAKE_ACCESS_ERR_FLAG       = 0X20,
    DRV_OTP_FAKE_CLEAR_BUFFER          = 0X30,
    DRV_OTP_FAKE_MAX
} drv_otp_fake_flag;

static hi_u8  g_fake_otp_buffer[OTP_FUSE_MAX] = {0};
static drv_otp_fake_flag g_fake_otp_flag = DRV_OTP_NO_FAKE_FLAG;

static hi_bool _fake_otp_buffer_flag(hi_void)
{
    if (g_fake_otp_flag == DRV_OTP_FAKE_BUFFER_FLAG) {
        return HI_TRUE;
    }
    return HI_FALSE;
}

static hi_bool _fake_otp_access_err_flag(hi_void)
{
    if (g_fake_otp_flag == DRV_OTP_FAKE_ACCESS_ERR_FLAG) {
        return HI_TRUE;
    }
    return HI_FALSE;
}

static hi_s32 _fake_otp_get_flag(hi_void)
{
    return g_fake_otp_flag;
}

static hi_s32 _fake_otp_clear_buffer(hi_void)
{
    hi_s32 ret;
    hi_drv_proc_echo_helper("clean OTP fake buffer\n");

    ret = (memset_s(g_fake_otp_buffer, sizeof(g_fake_otp_buffer), 0, sizeof(g_fake_otp_buffer)));
    if (ret != EOK) {
        return HI_ERR_OTP_SEC_FAILED;
    }
    return HI_SUCCESS;
}

static hi_s32 _fake_otp_reset_virtual_otp(hi_void)
{
    hi_s32 ret;
    hi_u32 index;

    ret = memset_s(g_fake_otp_buffer, sizeof(g_fake_otp_buffer), 0, sizeof(g_fake_otp_buffer));
    if (ret != EOK) {
        return HI_ERR_OTP_SEC_FAILED;
    }
    for (index = 0; index < OTP_FUSE_MAX; index++) {
        ret = hal_otp_read_byte(index, &g_fake_otp_buffer[index]);
        if (ret != EOK) {
            print_err_func_hex2(hal_otp_read_byte, index, ret);
            return HI_ERR_OTP_SEC_FAILED;
        }
    }
    return HI_SUCCESS;
}

static hi_s32 _fake_otp_read_byte(hi_u32 addr)
{
    if (addr >= OTP_FUSE_MAX) {
        return HI_SUCCESS;
    }
    return g_fake_otp_buffer[addr];
}

static hi_s32 _fake_otp_write_byte(hi_u32 addr, hi_u8 value)
{
    if (addr >= OTP_FUSE_MAX) {
        return HI_ERR_OTP_INVAILED_ADDR;
    }
    g_fake_otp_buffer[addr] = g_fake_otp_buffer[addr] | value;
    return HI_SUCCESS;
}

/*
* below command is just use for OTP test.
* value[0]:type
* value[1]~value[15]:parameter
*/
static hi_u32 _otp_visual_test(hi_u8 *value, hi_u32 len)
{
    hi_s32 ret = 0;

    switch (value[0]) {
        case DRV_OTP_GET_FAKE_BUFFER_FLAG:  /* 0x01 */
            ret = _fake_otp_get_flag();
            value[0x1] = (hi_u8)0xAA;
            value[0x2] = (hi_u8)ret;
            HI_ERR_OTP("Get OTP special test flag(0X%02x)\n", ret);
            break;
        case DRV_OTP_FAKE_BUFFER_FLAG:       /* 0x10 */
        case DRV_OTP_FAKE_BUFFER_RESET_FLAG: /* 0x11 */
            g_fake_otp_flag = value[0];
            ret = _fake_otp_reset_virtual_otp();
            if (ret != HI_SUCCESS) {
                HI_ERR_OTP("Reset virutal OTP failed.\n");
            }
            HI_ERR_OTP("Set OTP special test flag(0X%02x)(Reset or Init).\n", value[0]);
            break;
        case DRV_OTP_FAKE_ACCESS_ERR_FLAG:  /* 0x20 */
        case DRV_OTP_NO_FAKE_FLAG:          /* 0x00 */
            g_fake_otp_flag = value[0];
            HI_ERR_OTP("Set OTP special test flag(0X%02x).\n", value[0]);
            break;
        case DRV_OTP_FAKE_CLEAR_BUFFER:     /* 0x30 */
            ret = _fake_otp_clear_buffer();
            if (ret != HI_SUCCESS) {
                HI_ERR_OTP("Cleaning virutal OTP failed.\n");
            }
            HI_ERR_OTP("Set OTP special test flag(0X%02x)(Clean).\n", value[0]);
            break;
        default:
            return HI_ERR_OTP_NO_SUCH_OPTION;
    }
    return HI_SUCCESS;
}

static hi_s32 _otp_proc_virtual_write(hi_char *buf, hi_s32 len)
{
    hi_char *str = NULL;
    hi_u32 value;

    hi_drv_proc_echo_helper("come to set virtual otp setting\n");
    str = strstr(buf, "0x");
    if (str == HI_NULL) {
        hi_drv_proc_echo_helper("invalid write otp command !\n");
        _otp_proc_get_help();
        return HI_ERR_OTP_PTR_NULL;
    }
    value = (hi_u32)osal_strtoul(str, HI_NULL, 0x10);
    switch (value) {
        case DRV_OTP_FAKE_BUFFER_FLAG:
        case DRV_OTP_FAKE_BUFFER_RESET_FLAG:
            if (_fake_otp_reset_virtual_otp() != HI_SUCCESS) {
                hi_drv_proc_echo_helper("Reset virutal OTP failed.\n");
            }
            g_fake_otp_flag = value;
            hi_drv_proc_echo_helper("OTP set special test flag :0x%02x(Reset or Init)\n", value);
            break;
        case DRV_OTP_FAKE_ACCESS_ERR_FLAG:
        case DRV_OTP_NO_FAKE_FLAG:
            g_fake_otp_flag = value;
            hi_drv_proc_echo_helper("OTP set special test flag :0x%02x\n", value);
            break;
        case DRV_OTP_FAKE_CLEAR_BUFFER:
            if (_fake_otp_clear_buffer() != HI_SUCCESS) {
                HI_ERR_OTP("Cleaning virutal OTP failed.\n");
            }
            hi_drv_proc_echo_helper("OTP set special test flag :0x%02x(Clean)\n", value);
            break;
        default:
            _otp_proc_get_help();
            return HI_ERR_OTP_NO_SUCH_OPTION;
    }

    return HI_SUCCESS;
}

/* ****** proc virtualotp end ******* */
static hi_void _otp_proc_get_help(hi_void)
{
    hi_drv_proc_echo_helper("\nUsage as following: \n");
    hi_drv_proc_echo_helper("    cat /proc/msp/otp                     Display all proc information \n");
    hi_drv_proc_echo_helper("    echo help > /proc/msp/otp             Display help infomation for otp proc module \n");
    hi_drv_proc_echo_helper("    echo write addr data > /proc/msp/otp  Set data to addr in otp, byte by byte \n");
    hi_drv_proc_echo_helper("    For example: echo write 0x******** 0x** > /proc/msp/otp \n");

    hi_drv_proc_echo_helper("\n    Attention:\n");
    hi_drv_proc_echo_helper("        1 Operations must be carefully when setting data to otp !!!\n");
    hi_drv_proc_echo_helper("        2 Input 'addr' and 'data' must be take the '0x' prefix in hex format !!!\n");
    hi_drv_proc_echo_helper("        3 Otp must be set byte by byte !!!\n");

    /* ****** proc virtualotp begin ******* */
    hi_drv_proc_echo_helper("\n    Below command only for OTP test:\n");
    hi_drv_proc_echo_helper("    echo virtualotp 0x00 > /proc/msp/otp   use normal otp\n");
    hi_drv_proc_echo_helper("    echo virtualotp 0x10 > /proc/msp/otp   use virtual otp buffer\n");
    hi_drv_proc_echo_helper("    echo virtualotp 0x11 > /proc/msp/otp   reset otp virtual buffer\n");
    hi_drv_proc_echo_helper("    echo virtualotp 0x20 > /proc/msp/otp   use virtual otp error setting\n");
    hi_drv_proc_echo_helper("    echo virtualotp 0x30 > /proc/msp/otp   clean otp virtual buffer\n");
    /* ****** proc virtualotp end ******* */
    return;
}

static hi_void _otp_proc_read_all(struct seq_file *p)
{
    hi_u32 addr;
    hi_u32 val = 0;

    /* ****** proc virtualotp begin ******* */
    if (_fake_otp_access_err_flag() == HI_TRUE) {
        HI_PROC_PRINT(p, "OTP enter access error test mode.\n");
        HI_PROC_PRINT(p, "can not get any value from otp\n");
        return;
    }

    if (_fake_otp_buffer_flag() == HI_TRUE) {
        HI_PROC_PRINT(p, "OTP get from fake buffer\n");
    }
    /* ****** proc virtualotp end ******* */
    HI_PROC_PRINT(p, "OTP read all over:\n");
    for (addr = 0; addr < OTP_FUSE_MAX; addr += WORD_SIZE) {
        if ((addr & 0xf) == 0) {
            HI_PROC_PRINT(p, "\n%04x: ", addr);
        }
        if (hal_otp_read(addr, &val) != HI_SUCCESS) {
            HI_INFO_OTP("fail to hal_otp_read\n");
        }
        HI_PROC_PRINT(p, "%08x ", val);
    }
    HI_PROC_PRINT(p, "\n");

    return ;
}

static hi_s32 _otp_proc_fuse_write(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX])
{
    hi_s32 ret = HI_ERR_OTP_INVAILED_INPUT;
    hi_char *str1 = NULL;
    hi_char *str2 = NULL;
    hi_u32 addr;
    hi_u32 value;

    /* Get addr */
    str1 = strstr(argv[0x1], "0x");
    if (str1 == HI_NULL) {
        goto out;
    }

    addr = (hi_u32)osal_strtoul(str1, &str2, 0x10);
    if (addr >= 0x1000) {
        hi_drv_proc_echo_helper("invalid input addr (0x%x).\n\n", addr);
        goto out;
    }

    /* Get data */
    str2 = strstr(argv[0x2], "0x");
    if (str2 == HI_NULL) {
        goto out;
    }

    value = (hi_u32)osal_strtoul(str2, &str1, 0x10);
    if ((value & 0xffffff00) != 0) {
        hi_drv_proc_echo_helper("invalid input value (0x%08x).\n\n", value);
        goto out;
    }
    HI_WARN_OTP("addr = 0x%08x  value = 0x%02x\n", addr, value & 0xff);

    ret = hal_otp_write_byte(addr, (hi_u8)(value & 0xff));
    if (ret != HI_SUCCESS) {
        hi_drv_proc_echo_helper("write otp failed, ret = 0x%08x.\n", ret);
        goto out;
    }

    return HI_SUCCESS;
out:
    _otp_proc_get_help();
    return ret;
}

static hi_s32 _otp_proc_help(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (argv == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    _otp_proc_get_help();

    return HI_SUCCESS;
}

/* ****** proc virtualotp begin ******* */
static hi_s32 _otp_proc_virtual(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    hi_char *str = HI_NULL;

    if (argv == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    if (argc < 0x2) { /* 0x2: virtualotp 0x */
        return HI_ERR_OTP_INVALID_PARA;
    }

    str = argv[0x1];

    return _otp_proc_virtual_write(str, strlen(str));
}
/* ****** proc virtualotp end ******* */

static hi_s32 _otp_proc_write(hi_u32 argc, hi_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], hi_void *private)
{
    if (argv == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    if (argc < 0x3) { /* 0x3: write addr data */
        return HI_ERR_OTP_INVALID_PARA;
    }

    return _otp_proc_fuse_write(argc, argv);
}

static hi_s32 _otp_proc_read(hi_void *seqfile, hi_void *private)
{
    struct seq_file *p = HI_NULL;

    if (seqfile == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    p = (struct seq_file *)seqfile;
    HI_PROC_PRINT(p, "---------hisilicon OTP info---------\n");
    _otp_proc_read_all(p);
    HI_PROC_PRINT(p, "---------hisilicon otp info end---------\n");

    return HI_SUCCESS;
}

/* ****** proc virtualotp begin ******* */
hi_s32 fake_otp_virtual_read(hi_u32 addr, hi_u32 *value)
{
    if (value == HI_NULL) {
        return HI_ERR_OTP_PTR_NULL;
    }

    if (_fake_otp_access_err_flag() == HI_TRUE) {
        return HI_ERR_OTP_FAKE_ACCESS_FAILED;
    }

    if (_fake_otp_buffer_flag() != HI_TRUE) {
        return HI_ERR_OTP_NON_FAKE_MODE;
    }

    *value = _fake_otp_read_byte(addr + 0x00) +
             _fake_otp_read_byte(addr + 0x01) * 0x100 +
             _fake_otp_read_byte(addr + 0x02) * 0x10000 +
             _fake_otp_read_byte(addr + 0x03) * 0x1000000; /* get one word */
    return HI_SUCCESS;
}

hi_s32 fake_otp_virtual_write_byte(hi_u32 addr, hi_u8 value)
{
    if (_fake_otp_access_err_flag() == HI_TRUE) {
        return HI_ERR_OTP_FAKE_ACCESS_FAILED;
    }
    if (_fake_otp_buffer_flag() != HI_TRUE) {
        return HI_ERR_OTP_NON_FAKE_MODE;
    } else {
        return _fake_otp_write_byte(addr, (hi_u8)(value & 0xff));
    }
}

hi_s32 otp_virtual_test(hi_u32 cmd, hi_void *arg, hi_void *private_data)
{
    hi_s32 ret;
    hi_u8 value[0x10] = {0};

    ret = memcpy_s(value, sizeof(value), arg, sizeof(value));
    if (ret != EOK) {
        return HI_ERR_OTP_SEC_FAILED;
    }
    return _otp_visual_test(value, sizeof(value));
}
/* ****** proc virtualotp end   ******* */

static osal_proc_cmd g_proc_cmd[] = {
        { "help",       _otp_proc_help },
/* ****** proc virtualotp begin ******* */
        { "virtualotp", _otp_proc_virtual },
/* ****** proc virtualotp end   ******* */
        { "write",      _otp_proc_write },
};

hi_s32 otp_register_proc(hi_void)
{
    osal_proc_entry *item;

    item = osal_proc_add(HI_MOD_OTP_NAME, sizeof(HI_MOD_OTP_NAME));
    if (item == NULL) {
        print_err_func(osal_proc_add, HI_FAILURE);
        return HI_FAILURE;
    }

    item->read = _otp_proc_read;
    item->cmd_list = g_proc_cmd;
    item->cmd_cnt = sizeof(g_proc_cmd) / sizeof(osal_proc_cmd);

    return HI_SUCCESS;
}

hi_void otp_remove_proc(hi_void)
{
    osal_proc_remove(HI_MOD_OTP_NAME, strlen(HI_MOD_OTP_NAME));
    return;
}

