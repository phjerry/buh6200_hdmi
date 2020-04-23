/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
 * Description:
 */

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <asm/unistd.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/string.h>
#include <asm/signal.h>
#include <linux/time.h>
#include <linux/unistd.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/cdev.h>

#include "hi_type.h"
#include "hi_drv_dev.h"
#include "hi_drv_proc.h"

#include "hi_unf_keyled.h"
#include "hi_drv_keyled.h"
#include "drv_keyled_ioctl.h"
#include "drv_keyled.h"
#include "linux/hisilicon/securec.h"

#ifdef KEYLED_CT1642
#include "drv_keyled_ct1642.h"
#endif

#ifdef KEYLED_FD650
#include "drv_keyled_fd650.h"
#endif

#include "hi_drv_module.h"
#include "hi_errno.h"
#include "hi_drv_sys.h"

#define CHECK_KEY_OPEN() do {\
    if (!((hi_u32)g_keyled_flag & (hi_u32)KEYLED_STATUS_KEYON)) {\
        HI_WARN_KEYLED("Key not open\n");\
        return HI_FAILURE;\
    }\
} while (0)

#define CHECK_LED_OPEN() do {\
    if (!((hi_u32)g_keyled_flag & (hi_u32)KEYLED_STATUS_LEDON)) {\
        HI_WARN_KEYLED("Led not open\n");\
        return HI_FAILURE;\
    }\
} while (0)

/*-------------------------------------------------------------------
 * macro define section
 *-----------------------------------------------------------------*/

/*-------------------------------------------------------------------
 * date structure define section
 *-----------------------------------------------------------------*/

/*-------------------------------------------------------------------
 * variable define section
 *-----------------------------------------------------------------*/
static keyled_status g_keyled_flag = KEYLED_STATUS_KEYOFF_LEDOFF;
static keyled_operation g_keyled_opt;
static atomic_t g_keyled_init_count = ATOMIC_INIT(0);
static atomic_t g_key_open_count = ATOMIC_INIT(0);
static atomic_t g_led_open_count = ATOMIC_INIT(0);
static keyled_proc_info g_keyled_process_info = {0};
static hi_bool g_slect_type_flag = HI_FALSE;


/*-------------------------------------------------------------------
 * array define section
 *-----------------------------------------------------------------*/

/*-------------------------------------------------------------------
 * function define section
 *-----------------------------------------------------------------*/
hi_s32 keyled_open(keyled_status status, atomic_t *count)
{
    hi_s32 ret = 0;

    if (atomic_inc_return(count) == 1) {
        if ((hi_u32)g_keyled_flag & (hi_u32)status) {
            ret = HI_SUCCESS;
        } else {
            ret = ((hi_u32)status & (hi_u32)KEYLED_STATUS_KEYON) ?
                  g_keyled_opt.keyled_key_open() :
                  g_keyled_opt.keyled_led_open();
            if (ret == HI_SUCCESS) {
                g_keyled_flag = (hi_u32)g_keyled_flag | (hi_u32)status;
            }
        }
    }

    return ret;
}

hi_s32 keyled_close(keyled_status status, atomic_t *count)
{
    hi_s32 ret = 0;

    if (atomic_dec_and_test(count)) {
        if ((hi_u32)g_keyled_flag & (hi_u32)status) {
            g_keyled_flag = (hi_u32)g_keyled_flag & (~(hi_u32)status);
            ret = ((hi_u32)status & (hi_u32)KEYLED_STATUS_KEYON) ?
                  g_keyled_opt.keyled_key_close() :
                  g_keyled_opt.keyled_led_close();
        } else {
            ret = HI_SUCCESS;
        }
    } else {
        atomic_inc(count);
    }

    return ret;
}



hi_s32 keyled_set_lock(hi_u32 *lock)
{
    hi_s32 ret = 0;

    CHECK_LED_OPEN();
    if (g_keyled_opt.keyled_led_set_lock_indicator) {
        ret = g_keyled_opt.keyled_led_set_lock_indicator((hi_bool) * lock);
    } else {
        HI_ERR_KEYLED("this set only for fd650 front panel\n");
        ret = HI_FAILURE;
    }

    return ret;
}

/* register tuner standard interface function */
#ifdef KEYLED_CT1642
static hi_void keyled_select_ct1642(hi_void)
{
    HI_INFO_KEYLED("select ct1642 keyled\n");
    g_keyled_opt.keyled_key_open  = keyled_key_open_ct1642;
    g_keyled_opt.keyled_key_close = keyled_key_close_ct1642;
    g_keyled_opt.keyled_key_reset = keyled_key_reset_ct1642;
    g_keyled_opt.keyled_key_get_value = keyled_key_get_value_ct1642;
    g_keyled_opt.keyled_key_set_block_time = keyled_key_set_block_time_ct1642;
    g_keyled_opt.keyled_key_set_repeat_time = keyled_key_set_repeat_time_ct1642;
    g_keyled_opt.keyled_key_enable_repeat_key = keyled_key_enable_repeat_key_ct1642;
    g_keyled_opt.keyled_key_enable_key_up  = keyled_key_enable_key_up_ct1642;
    g_keyled_opt.keyled_led_open  = keyled_led_open_ct1642;
    g_keyled_opt.keyled_led_close = keyled_led_close_ct1642;
    g_keyled_opt.keyled_led_display = keyled_led_display_ct1642;
    g_keyled_opt.keyled_led_display_time  = keyled_led_display_time_ct1642;
    g_keyled_opt.keyled_led_set_flash_pin  = keyled_led_set_flash_pin_ct1642;
    g_keyled_opt.keyled_led_set_flash_freq = keyled_led_set_flash_freq_ct1642;
    g_keyled_opt.keyled_led_set_lock_indicator = HI_NULL;
    g_keyled_opt.keyled_led_display_led = HI_NULL;
    g_keyled_opt.keyled_set_mode = HI_NULL;
    g_keyled_opt.keyled_get_proc_info = keyled_get_proc_info_ct1642;

    g_keyled_opt.keyled_suspend = keyled_suspend_ct1642;
    g_keyled_opt.keyled_resume  = keyled_resume_ct1642;

    return;
}
#endif

static hi_s32 keyled_select_type(hi_keyled_type keyled_type)
{
    hi_bool initialized_flag = HI_FALSE;

    if (keyled_type == KEYLED_TYPE_CT1642) {
#ifdef KEYLED_CT1642
        keyled_select_ct1642();
        initialized_flag = HI_TRUE;
#endif
    } else if (keyled_type == KEYLED_TYPE_FD650) {
#ifdef KEYLED_FD650
        HI_INFO_KEYLED("select fd650 keyled\n");

        g_keyled_opt.keyled_key_open  = drv_keyled_key_open_fd650;
        g_keyled_opt.keyled_key_close = drv_keyled_key_close_fd650;
        g_keyled_opt.keyled_key_reset = drv_keyled_key_reset_fd650;
        g_keyled_opt.keyled_key_get_value = drv_keyled_key_get_value_fd650;
        g_keyled_opt.keyled_key_set_block_time = drv_keyled_key_set_block_time_fd650;
        g_keyled_opt.keyled_key_set_repeat_time = drv_keyled_key_set_repeat_time_fd650;
        g_keyled_opt.keyled_key_enable_repeat_key = drv_keyled_key_enable_repeat_key_fd650;
        g_keyled_opt.keyled_key_enable_key_up  = drv_keyled_key_enable_key_up_fd650;

        g_keyled_opt.keyled_led_open  = drv_keyled_led_open_fd650;
        g_keyled_opt.keyled_led_close = drv_keyled_led_close_fd650;
        g_keyled_opt.keyled_led_display = drv_keyled_led_display_fd650;
        g_keyled_opt.keyled_led_display_time  = drv_keyled_led_display_time_fd650;
        g_keyled_opt.keyled_led_set_flash_pin  = drv_keyled_led_set_flash_pin_fd650;
        g_keyled_opt.keyled_led_set_flash_freq = drv_keyled_led_set_flash_freq_fd650;
        g_keyled_opt.keyled_led_set_lock_indicator = drv_keyled_led_set_lock_indicator_fd650;
        g_keyled_opt.keyled_led_display_led = HI_NULL;
        g_keyled_opt.keyled_set_mode = HI_NULL;
        g_keyled_opt.keyled_get_proc_info = drv_keyled_get_proc_info_fd650;

        g_keyled_opt.keyled_suspend = drv_keyled_suspend_fd650;
        g_keyled_opt.keyled_resume  = drv_keyled_resume_fd650;

        initialized_flag = HI_TRUE;
#endif
    } else {
        HI_ERR_KEYLED("This version doesn't support this type(%d) keyled \n", keyled_type);
    }

    /* If set ct1642 in kernel, force the functions to ct1642 */
#ifdef HI_KEYLED_CT1642_KERNEL_SUPPORT
    keyled_select_ct1642();
    initialized_flag = HI_TRUE;
#endif

    g_slect_type_flag = initialized_flag;

    if (HI_FALSE == initialized_flag) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_slong keyled_ioctl(struct file *file, hi_u32 cmd, hi_void *arg)
{
    hi_s32 ret = 0;
    get_key_value *value_para = HI_NULL;
    keyled_status  status = 0;
    hi_keyled_time display_time = {0};
    hi_u32 repeat_timems = 0;

    if (HI_ID_KEYLED != _IOC_TYPE(cmd)) {
        return -ENOTTY;
    }

    if ((cmd != HI_KEYLED_SELECT_CMD) && (g_slect_type_flag ==  HI_FALSE)) {
        HI_ERR_KEYLED("Please select keyled type first!\n");
        return HI_FAILURE;
    }

    switch (cmd) {
        case HI_KEYLED_SELECT_CMD:
            if (arg == HI_NULL) {
                return HI_ERR_KEYLED_NULL_PTR;
            }
            g_keyled_process_info.keyled_type = *(hi_keyled_type *)arg;

            if (g_keyled_process_info.keyled_type > KEYLED_TYPE_MAX) {
                return HI_ERR_KEYLED_INVALID_PARA;
            }
            ret = keyled_select_type(*(hi_keyled_type *)arg);
            break;

        case HI_KEYLED_KEY_OPEN_CMD:
            status = KEYLED_STATUS_KEYON;
            ret =  keyled_open(status, &g_key_open_count);
            break;

        case HI_KEYLED_KEY_CLOSE_CMD:
            status = KEYLED_STATUS_KEYON;
            ret =  keyled_close(status, &g_key_open_count);
            break;

        case HI_KEYLED_KEY_RESET_CMD:
            CHECK_KEY_OPEN();
            ret = g_keyled_opt.keyled_key_reset();
            break;

        case HI_KEYLED_KEY_GET_VALUE_CMD:
            CHECK_KEY_OPEN();
            if (arg == HI_NULL) {
                return HI_ERR_KEYLED_NULL_PTR;
            }
            value_para = (get_key_value *)arg;
            ret = g_keyled_opt.keyled_key_get_value(&value_para->key_status, &value_para->key_code);
            break;

        case HI_KEYLED_SET_BLOCK_TIME_CMD:
            CHECK_KEY_OPEN();
            if (arg == HI_NULL) {
                return HI_ERR_KEYLED_NULL_PTR;
            }
            ret = g_keyled_opt.keyled_key_set_block_time(*(hi_u32 *)arg);
            break;

        case HI_KEYLED_SET_IS_KEYUP_CMD:
            CHECK_KEY_OPEN();
            if (arg == HI_NULL) {
                return HI_ERR_KEYLED_NULL_PTR;
            }
            if (*(hi_u32 *)arg > 1) {
                return HI_ERR_KEYLED_INVALID_PARA;
            }
            ret = g_keyled_opt.keyled_key_enable_key_up(*(hi_u32 *)arg);
            break;

        case HI_KEYLED_SET_IS_REPKEY_CMD:
            CHECK_KEY_OPEN();
            if (arg == HI_NULL) {
                return HI_ERR_KEYLED_NULL_PTR;
            }
            if (*(hi_u32 *)arg > 1) {
                return HI_ERR_KEYLED_INVALID_PARA;
            }
            ret = g_keyled_opt.keyled_key_enable_repeat_key(*(hi_u32 *)arg);
            break;

        case HI_KEYLED_SET_REPKEY_TIME_CMD:
            CHECK_KEY_OPEN();
            if (arg == HI_NULL) {
                return HI_ERR_KEYLED_NULL_PTR;
            }
            repeat_timems = *(hi_u32 *)arg;
            if (repeat_timems < 108) {
                repeat_timems = 108;
            } else if (repeat_timems > 65536) {
                repeat_timems = 65536;
            }

            ret = g_keyled_opt.keyled_key_set_repeat_time(repeat_timems);
            break;

        case HI_KEYLED_LED_OPEN_CMD:
            status = KEYLED_STATUS_LEDON;
            ret =  keyled_open(status, &g_led_open_count);
            break;

        case HI_KEYLED_LED_CLOSE_CMD:
            status = KEYLED_STATUS_LEDON;
            ret =  keyled_close(status, &g_led_open_count);
            break;

        case HI_KEYLED_DISPLAY_CODE_CMD:
            CHECK_LED_OPEN();
            if (arg == HI_NULL) {
                return HI_ERR_KEYLED_NULL_PTR;
            }
            g_keyled_process_info.display_code = *(hi_u32 *)arg;
            ret = g_keyled_opt.keyled_led_display(*(hi_u32 *)arg);
            break;

        case HI_KEYLED_DISPLAY_TIME_CMD:
            CHECK_LED_OPEN();
            if (arg == HI_NULL) {
                return HI_ERR_KEYLED_NULL_PTR;
            }
            display_time = *(hi_keyled_time *)arg;
            if ((display_time.hour > 23) || (display_time.minute > 59)) {
                return HI_ERR_KEYLED_INVALID_PARA;
            }
            g_keyled_process_info.led_time = *(hi_keyled_time *)arg;
            ret = g_keyled_opt.keyled_led_display_time(*(hi_keyled_time *)arg);
            break;

        case HI_KEYLED_SET_FLASH_PIN_CMD:
            CHECK_LED_OPEN();
            if (arg == HI_NULL) {
                return HI_ERR_KEYLED_NULL_PTR;
            }
            if ((*(hi_keyled_light *)arg > 6) || (*(hi_keyled_light *)arg < 1)) {
                return HI_ERR_KEYLED_INVALID_PARA;
            }
            ret = g_keyled_opt.keyled_led_set_flash_pin(*(hi_keyled_light *)arg);
            break;

        case HI_KEYLED_CONFIG_FLASH_FREQ_CMD:
            CHECK_LED_OPEN();
            if (arg == HI_NULL) {
                return HI_ERR_KEYLED_NULL_PTR;
            }
            if ((*(hi_keyled_level *)arg < 1) || (*(hi_keyled_level *)arg > 5)) {
                return  HI_ERR_KEYLED_INVALID_PARA;
            }
            ret = g_keyled_opt.keyled_led_set_flash_freq(*(hi_keyled_level *)arg);
            break;

        case HI_KEYLED_SETLOCK_CMD:
            if (arg == HI_NULL) {
                return HI_ERR_KEYLED_NULL_PTR;
            }
            ret = keyled_set_lock((hi_u32 *)arg);
            break;

        default:
            return -EFAULT;
    }

    return ret;
}
EXPORT_SYMBOL(keyled_ioctl);

#ifdef HI_PROC_SUPPORT
static hi_s8 keyled_type_param[5][16] = {"74HC164", "PT6961", "CT1642", "PT6964", "FD650"};
static hi_s8 led_flash_type[7][16] = {"Not Set", "First", "Second", "Third", "Fourth", "ALL", "NONE"};
static hi_s32 keyled_proc_read(struct seq_file *p, hi_void *v)
{
    if (p == HI_NULL) {
        return HI_ERR_KEYLED_NULL_PTR;
    }

    if (g_keyled_opt.keyled_get_proc_info != HI_NULL) {
        (hi_void)g_keyled_opt.keyled_get_proc_info(&g_keyled_process_info);
    }

    if (g_keyled_process_info.flash_pin >= KEYLED_LIGHT_MAX) {
        g_keyled_process_info.flash_pin = 0; /* means not set flash pin. */
    }

    HI_PROC_PRINT(p, "---------Hisilicon KeyLed Info---------\n");

    HI_PROC_PRINT(p,
                  "KeyStatus               \t :%s\n"
                  "LedStatus               \t :%s\n"
                  "Select KeyLed Type      \t :%s\n"
                  "Timeout of Reading Key  \t :%u(ms)\n"
                  "Key Up Report           \t :%s\n"
                  "Repeat Key Report       \t :%s\n"
                  "Repeat Key Report time  \t :%u(ms)\n"
                  "Led Display Code        \t :0x%08x\n"
                  "Led Display Time        \t :%u:%u(hour:minute)\n"
                  "Led Flash Pin           \t :%s\n"
                  "Led Flash Pin Level     \t :%u\n"
                  "Key Buffer Length       \t :%u\n"
                  "Key Buffer Head         \t :%u\n"
                  "Key Buffer Tail         \t :%u\n"
                  "Key Come Number         \t :%u\n"
                  "Key Read Number         \t :%u\n"
                  ,

                  ((hi_u32)g_keyled_flag & (hi_u32)KEYLED_STATUS_KEYON) ? "Open" : "Close",
                  ((hi_u32)g_keyled_flag & (hi_u32)KEYLED_STATUS_LEDON) ? "Open" : "Close",
                  keyled_type_param[g_keyled_process_info.keyled_type],
                  g_keyled_process_info.block_time,
                  (g_keyled_process_info.is_up_key_enable) ? "Enable" : "Disable",
                  (g_keyled_process_info.is_repeat_key_enable) ? "Enable" : "Disable",
                  g_keyled_process_info.repeat_key_time_ms,
                  g_keyled_process_info.display_code,
                  g_keyled_process_info.led_time.hour, g_keyled_process_info.led_time.minute,
                  led_flash_type[g_keyled_process_info.flash_pin],
                  g_keyled_process_info.flash_level,
                  g_keyled_process_info.key_buf_size,
                  g_keyled_process_info.key_buf_head,
                  g_keyled_process_info.key_buf_tail,
                  g_keyled_process_info.key_come_num,
                  g_keyled_process_info.key_read_num);

    return HI_SUCCESS;
}
#endif

static hi_slong drv_keyled_ioctl(struct file *file, hi_u32 cmd, hi_size_t arg)
{
    hi_slong ret;

    ret = hi_drv_user_copy(file, cmd, arg, keyled_ioctl);

    return ret;
}

static hi_s32 drv_keyled_open(struct inode *inode, struct file *filp)
{
    atomic_inc(&g_keyled_init_count);

    return HI_SUCCESS;
}

static hi_s32 drv_keyled_release(struct inode *inode, struct file *filp)
{
    if (atomic_dec_and_test(&g_keyled_init_count)) {
        if (atomic_read(&g_key_open_count)) {
            if (atomic_dec_and_test(&g_key_open_count)) {
                g_keyled_flag = (hi_u32)g_keyled_flag & (~(hi_u32)KEYLED_STATUS_KEYON);
                g_keyled_opt.keyled_key_close();
            }
        }

        if (atomic_read(&g_led_open_count)) {
            if (atomic_dec_and_test(&g_led_open_count)) {
                g_keyled_flag = (hi_u32)g_keyled_flag & (~(hi_u32)KEYLED_STATUS_LEDON);
                g_keyled_opt.keyled_led_close();
            }
        }
    }

    return HI_SUCCESS;

}

static hi_s32 keyled_pm_suspend(struct device *dev)
{
    hi_s32 ret;

    if (g_keyled_opt.keyled_suspend) {
        ret = g_keyled_opt.keyled_suspend();
        if (ret) {
            HI_FATAL_KEYLED("keyled_pm_suspend err \n");
            return HI_FAILURE;
        } else {
            HI_PRINT("KEYLED suspend OK\n");
        }
    }

    return HI_SUCCESS;
}

static hi_s32 keyled_pm_resume(struct device *dev)
{
    hi_s32 ret;

    if (g_keyled_opt.keyled_resume) {
        ret = g_keyled_opt.keyled_resume();
        if (ret) {
            HI_FATAL_KEYLED("keyled_pm_resume err \n");
            return HI_FAILURE;
        } else {
            HI_PRINT("KEYLED resume OK\n");
        }
    }

    return HI_SUCCESS;
}

static struct dev_pm_ops g_keyled_pm_ops = {
    .suspend        = keyled_pm_suspend,
    .suspend_late   = NULL,
    .resume_early   = NULL,
    .resume         = keyled_pm_resume,
};

static struct file_operations keyled_opts = {
open :
    drv_keyled_open,
release:
    drv_keyled_release,
unlocked_ioctl:
    drv_keyled_ioctl,
#ifdef CONFIG_COMPAT
compat_ioctl:
    drv_keyled_ioctl,
#endif
};

/********************************************************************/

static struct class *g_keyled_class = HI_NULL;
static dev_t g_keyled_devno;
static struct cdev *g_keyled_cdev = HI_NULL;
static struct device *g_keyled_dev = HI_NULL;

static hi_s32 keyled_register_dev(hi_void)
{
    hi_s32 ret;

    ret = alloc_chrdev_region(&g_keyled_devno, 0, 1, "keyled");
    if (ret != HI_SUCCESS) {
        HI_FATAL_KEYLED("keyled alloc chrdev region failed\n");

        return HI_FAILURE;
    }

    g_keyled_cdev = cdev_alloc();
    if (IS_ERR(g_keyled_cdev)) {
        HI_FATAL_KEYLED("keyled alloc cdev failed! \n");

        ret = HI_FAILURE;
        goto out0;
    }

    cdev_init(g_keyled_cdev, &keyled_opts);
    g_keyled_cdev->owner = THIS_MODULE;

    ret = cdev_add(g_keyled_cdev, g_keyled_devno, 1);
    if (ret != HI_SUCCESS) {
        HI_FATAL_KEYLED("keyled add cdev failed, ret(%d).\n", ret);
        ret = HI_FAILURE;
        goto out1;
    }

    g_keyled_class = class_create(THIS_MODULE, "keyled_class");
    if (IS_ERR(g_keyled_class)) {
        HI_FATAL_KEYLED("keyled create dev class failed! \n");

        ret = HI_FAILURE;
        goto out2;
    }

    g_keyled_class->pm = &g_keyled_pm_ops;
    g_keyled_dev = device_create(g_keyled_class, HI_NULL, g_keyled_devno, HI_NULL, HI_DEV_KEYLED_NAME);
    if (IS_ERR(g_keyled_dev)) {
        HI_FATAL_KEYLED("keyled create dev failed! \n");
        ret = HI_FAILURE;

        goto out3;
    }

    return HI_SUCCESS;

out3:
    class_destroy(g_keyled_class);
    g_keyled_class = HI_NULL;
out2:
    cdev_del(g_keyled_cdev);
out1:
    kfree(g_keyled_cdev);
    g_keyled_cdev = HI_NULL;
out0:
    unregister_chrdev_region(g_keyled_devno, 1);

    return ret;
}

static hi_void keyled_unregister_dev(hi_void)
{
    device_destroy(g_keyled_class, g_keyled_devno);
    g_keyled_dev = HI_NULL;
    class_destroy(g_keyled_class);
    g_keyled_class = HI_NULL;
    cdev_del(g_keyled_cdev);
    kfree(g_keyled_cdev);
    g_keyled_cdev = HI_NULL;
    unregister_chrdev_region(g_keyled_devno, 1);

    return;
}

static hi_void keyled_init_ct1642(hi_void)
{
    HI_INFO_KEYLED("select ct1642 keyled\n");

    g_keyled_opt.keyled_key_open  = keyled_key_open_ct1642;
    g_keyled_opt.keyled_key_close = keyled_key_close_ct1642;
    g_keyled_opt.keyled_key_reset = keyled_key_reset_ct1642;
    g_keyled_opt.keyled_key_get_value = keyled_key_get_value_ct1642;
    g_keyled_opt.keyled_key_set_block_time = keyled_key_set_block_time_ct1642;
    g_keyled_opt.keyled_key_set_repeat_time = keyled_key_set_repeat_time_ct1642;
    g_keyled_opt.keyled_key_enable_repeat_key = keyled_key_enable_repeat_key_ct1642;
    g_keyled_opt.keyled_key_enable_key_up  = keyled_key_enable_key_up_ct1642;
    g_keyled_opt.keyled_led_open  = keyled_led_open_ct1642;
    g_keyled_opt.keyled_led_close = keyled_led_close_ct1642;
    g_keyled_opt.keyled_led_display = keyled_led_display_ct1642;
    g_keyled_opt.keyled_led_display_time  = keyled_led_display_time_ct1642;
    g_keyled_opt.keyled_led_set_flash_pin  = keyled_led_set_flash_pin_ct1642;
    g_keyled_opt.keyled_led_set_flash_freq = keyled_led_set_flash_freq_ct1642;
    g_keyled_opt.keyled_led_set_lock_indicator = HI_NULL;
    g_keyled_opt.keyled_led_display_led = HI_NULL;
    g_keyled_opt.keyled_set_mode = HI_NULL;

    g_keyled_opt.keyled_suspend = keyled_suspend_ct1642;
    g_keyled_opt.keyled_resume  = keyled_resume_ct1642;

}

hi_s32 keyled_drv_module_init(hi_void)
{
    hi_s32 ret;
#ifdef HI_PROC_SUPPORT
    hi_proc_item *item = HI_NULL;
#endif

#if defined (KEYLED_CT1642) || \
    defined (KEYLED_FD650)
#else
    HI_ERR_KEYLED("keyled init failed \n");
    return HI_FAILURE;
#endif

    ret = hi_drv_module_register(HI_ID_KEYLED, "HI_KEYLED", HI_NULL, HI_NULL);
    if (HI_SUCCESS != ret) {
        HI_ERR_KEYLED("keyled init failed ret = 0x%x\n", ret);
        return ret;
    }

    g_keyled_flag = 0;

    if (keyled_register_dev() < 0) {
        HI_FATAL_KEYLED("register keyled failed.\n");
        return HI_FAILURE;
    }
#ifdef HI_PROC_SUPPORT
    item = hi_drv_proc_add_module(HI_MOD_KEYLED_NAME, NULL, NULL);
    if (!item) {
        HI_FATAL_KEYLED("add proc module failed\n");
        return HI_FAILURE;
    }

    item->read  = keyled_proc_read;
#endif

#ifdef KEYLED_CT1642
    keyled_init_ct1642();
#endif

#ifdef KEYLED_FD650
    HI_INFO_KEYLED("select fd650 keyled\n");

    g_keyled_opt.keyled_key_open  = drv_keyled_key_open_fd650;
    g_keyled_opt.keyled_key_close = drv_keyled_key_close_fd650;
    g_keyled_opt.keyled_key_reset = drv_keyled_key_reset_fd650;
    g_keyled_opt.keyled_key_get_value = drv_keyled_key_get_value_fd650;
    g_keyled_opt.keyled_key_set_block_time = drv_keyled_key_set_block_time_fd650;
    g_keyled_opt.keyled_key_set_repeat_time = drv_keyled_key_set_repeat_time_fd650;
    g_keyled_opt.keyled_key_enable_repeat_key = drv_keyled_key_enable_repeat_key_fd650;
    g_keyled_opt.keyled_key_enable_key_up  = drv_keyled_key_enable_key_up_fd650;

    g_keyled_opt.keyled_led_open  = drv_keyled_led_open_fd650;
    g_keyled_opt.keyled_led_close = drv_keyled_led_close_fd650;
    g_keyled_opt.keyled_led_display = drv_keyled_led_display_fd650;
    g_keyled_opt.keyled_led_display_time  = drv_keyled_led_display_time_fd650;
    g_keyled_opt.keyled_led_set_flash_pin  = drv_keyled_led_set_flash_pin_fd650;
    g_keyled_opt.keyled_led_set_flash_freq = drv_keyled_led_set_flash_freq_fd650;
    g_keyled_opt.keyled_led_set_lock_indicator = drv_keyled_led_set_lock_indicator_fd650;
    g_keyled_opt.keyled_led_display_led = HI_NULL;
    g_keyled_opt.keyled_set_mode = HI_NULL;

    g_keyled_opt.keyled_suspend = drv_keyled_suspend_fd650;
    g_keyled_opt.keyled_resume  = drv_keyled_resume_fd650;
#endif
#ifdef MODULE
    HI_PRINT("Load hi_keyled.ko success.\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;
}

hi_void keyled_drv_module_exit(hi_void)
{
#ifdef HI_PROC_SUPPORT
    hi_drv_proc_remove_module(HI_MOD_KEYLED_NAME);
#endif

    (hi_void)hi_drv_module_unregister(HI_ID_KEYLED);

    /* unregister demux device */
    keyled_unregister_dev();
}

#ifdef MODULE
module_init(keyled_drv_module_init);
module_exit(keyled_drv_module_exit);
#endif

MODULE_LICENSE("GPL");

