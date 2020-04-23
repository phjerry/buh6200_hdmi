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
#include <mach/hardware.h>
#include <asm/signal.h>
#include <linux/time.h>
#include <linux/unistd.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/poll.h>
#include "hi_type.h"
#include "hi_drv_mem.h"
#include "drv_gpio_ext.h"
#include "hi_drv_module.h"

#include "hi_unf_keyled.h"
#include "hi_error_mpi.h"
#include "hi_drv_keyled.h"
#include "drv_keyled_ioctl.h"
#include "drv_keyled.h"
#include "drv_keyled_ct1642.h"
#ifdef CONFIG_HIGH_RES_TIMERS
#include <linux/hrtimer.h>
#endif

static  gpio_ext_func *g_gpio_func = HI_NULL;

static atomic_t g_ct1642_open = ATOMIC_INIT(0);

#define DEFAULT_REP_KEY_TIME   300
static  hi_u8  g_led_code[LED_BIT_COUNT];

static keyled_dev_attr g_keyled_dev;
static hi_keyled_time g_keyled_dev = {0};
static wait_queue_head_t g_wait_keyleds;

static hi_u32 g_key_detected_flag = 0;
static hi_bool g_timer_run_flag =  HI_FALSE;

static hi_u32 g_key_press_flag = 0;
static hi_u32 g_key_code_last = KEY_MACRO_NO;
static hi_u32 g_key_code_curr = KEY_MACRO_NO;


static hi_u32 g_cycle_cnt = 0;

static hi_u32 g_led_dig_dis_dot_ct1642[]  = { 0xfd, 0x61, 0xdb, 0xf3, 0x67, 0xb7, 0xbf, 0xe1, 0xff, 0xf7 };

static hi_void keyled_key_scan(hi_void);                                  /* KEY scan manage func */
static hi_void keyled_led_scan(hi_u8 position);                           /* LED scan manage func */
static hi_void keyled_send_bit_data(hi_u8 character, hi_u8 position);     /* light 1 LED data func */


#define  GPIO_CLOCK_SET(val)\
    do {\
            if (g_gpio_func && g_gpio_func->pfn_gpio_write_bit)\
                g_gpio_func->pfn_gpio_write_bit(CLK_CT1642, val);\
    } while(0)

#define  GPIO_DATA_SET(val)\
    do {\
            if (g_gpio_func && g_gpio_func->pfn_gpio_write_bit)\
                g_gpio_func->pfn_gpio_write_bit(DAT_CT1642, val);\
    } while(0)

#define  GPIO_KEY_GET(val)\
    do {\
           if (g_gpio_func && g_gpio_func->pfn_gpio_read_bit)\
               g_gpio_func->pfn_gpio_read_bit(KEY_CT1642, &val);\
    } while(0)\

#define  KEYLED_TIMER_INTERVAL_MS 2

hi_void *g_led_timer_ptr = NULL;


hi_void *hi_high_res_timer_init(hi_void *timer_proc)
{

#ifdef CONFIG_HIGH_RES_TIMERS       /* defined in linux kernel */
    struct hrtimer *api_timer_ptr = HI_NULL;
    api_timer_ptr = (struct hrtimer *)HI_KMALLOC(HI_ID_KEYLED, sizeof(struct hrtimer), GFP_ATOMIC);
    if (api_timer_ptr != HI_NULL) {
        hrtimer_init(api_timer_ptr, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
        api_timer_ptr->function = timer_proc;
        HI_INFO_KEYLED("hrtimer_init OK! \n");
        return (void *)api_timer_ptr;
    }
#else
    struct timer_list *api_timer_ptr = HI_NULL;
    api_timer_ptr = (struct timer_list *)HI_KMALLOC(HI_ID_KEYLED, sizeof(struct timer_list), GFP_ATOMIC);
    if (api_timer_ptr != HI_NULL) {
        init_timer(api_timer_ptr);
        api_timer_ptr->function = timer_proc;
        api_timer_ptr->data = 0;
        HI_INFO_KEYLED("init_timer OK! \n");
        return (void *)api_timer_ptr;
    }
#endif

    return HI_NULL;
}

hi_s32 hi_high_res_timer_start(hi_void *handle, hi_bool init)
{
    if (handle == HI_NULL) {
        HI_ERR_KEYLED("HI_HIGH_RES_TimerStart handle invalid \n");
        return HI_FAILURE;
    }

#ifdef CONFIG_HIGH_RES_TIMERS    /* defined in linux kernel */
    hrtimer_start((struct hrtimer *)handle, ktime_set(0, KEYLED_TIMER_INTERVAL_MS * 1000000), HRTIMER_MODE_REL);
#else
    {
        struct timer_list *api_timer_ptr = NULL;
        api_timer_ptr = (struct timer_list *)handle;
        api_timer_ptr->expires = (jiffies + msecs_to_jiffies(KEYLED_TIMER_INTERVAL_MS));
        if (init == HI_TRUE) {
            add_timer(api_timer_ptr);
        } else {
            mod_timer(api_timer_ptr, api_timer_ptr->expires);
        }
    }
#endif

    return HI_SUCCESS;
}

hi_s32 hi_high_res_timer_update(hi_void *handle)
{
    if (handle == HI_NULL) {
        HI_ERR_KEYLED("HI_HIGH_RES_TimerUpdate handle invalid \n");
        return HI_FAILURE;
    }

#ifdef CONFIG_HIGH_RES_TIMERS    /* defined in linux kernel */
    hrtimer_forward((struct hrtimer *)handle, ktime_get(), ktime_set(0, KEYLED_TIMER_INTERVAL_MS * 1000000));
#else
    {
        struct timer_list *api_timer_ptr = NULL;
        api_timer_ptr = (struct timer_list *)handle;
        api_timer_ptr->expires = (jiffies + msecs_to_jiffies(KEYLED_TIMER_INTERVAL_MS));
        mod_timer(api_timer_ptr, api_timer_ptr->expires);
    }
#endif

    return HI_SUCCESS;
}

hi_s32 hi_high_res_timer_destroy(hi_void *handle)
{
    if (handle == HI_NULL) {
        HI_ERR_KEYLED("HI_HIGH_RES_TimerDestroy handle invalid \n");
        return HI_FAILURE;
    }

#ifdef CONFIG_HIGH_RES_TIMERS    /* defined in linux kernel */
    hrtimer_cancel((struct hrtimer *)handle);
#else
    del_timer_sync((struct timer_list *)handle);
#endif

    HI_KFREE(HI_ID_KEYLED, handle);

    return HI_SUCCESS;
}

hi_void led_timer_proc(hi_void)
{
    static hi_u8 led_position = 0;

    if (hi_high_res_timer_update(g_led_timer_ptr) != HI_SUCCESS) {
        HI_ERR_KEYLED("Err in HI_HIGH_RES_TimerUpdate!\n");
        return;
    }

    if (led_position == 4) {
        keyled_key_scan();
        led_position = 0;
    } else {
        keyled_led_scan(led_position);
        led_position++;
    }
    if (hi_high_res_timer_start(g_led_timer_ptr, HI_FALSE) != HI_SUCCESS) {
        HI_ERR_KEYLED("Err in HI_HIGH_RES_TimerStart!\n");
        return;
    }

    return;
}

hi_s32 led_init_timer(hi_void)
{
    if (g_led_timer_ptr != HI_NULL) {
        HI_ERR_KEYLED("LED_InitTimer g_led_timer_ptr is not NULL\n");
        return HI_FAILURE;
    }

    g_led_timer_ptr = hi_high_res_timer_init(led_timer_proc);

    if (hi_high_res_timer_start(g_led_timer_ptr, HI_TRUE) != HI_SUCCESS) {
        HI_ERR_KEYLED("HI_HIGH_RES_TimerStart start failure \n");
        return HI_FAILURE;
    }
    g_timer_run_flag = HI_TRUE;

    return HI_SUCCESS;
}

hi_void led_delete_timer(hi_void)
{
    hi_high_res_timer_destroy(g_led_timer_ptr);

    g_led_timer_ptr = NULL;
    g_timer_run_flag = HI_FALSE;

    HI_INFO_KEYLED("DeleteTimer g_timer_run_flag =  %d\n", g_timer_run_flag);

    return;
}

static hi_void keyled_send_bit_data(hi_u8 character, hi_u8 position)
{
    hi_u8 bit_position = 0;                   /* LED Selected */
    hi_u8 bit_character = character;     /* LED display Code */
    hi_u8 i = 0;

    switch (position) {
        /* display thousand LED,bit_character|=0x01 show light on */
        /* Power indicator light D1                              */
        case 0: {
            bit_position = 0xef;
            bit_character |= 0x01;
            break;
        }
        /* display hundred LED          */
        /* 1--D2 light on,0 D2 lightoff */
        case 1: {
            bit_position = 0xdf;
            break;
            } /* display decade LED */
        case 2: {
            bit_position = 0xbf;
            break;
            } /* display last LED  */
        case 3: {
            bit_position = 0x7f;
            break;
            } /* display off for key scan */
        case 4: {
            bit_position = 0xff;
            break;
            } /* default no display */
        default: {
            bit_position = 0xff;
            bit_character = 0x00;
        }
    }

    for (i = 0; i < 8; i++) {/* send 8bit addr */
        GPIO_CLOCK_SET(HI_FALSE);
        if ((bit_position << i) & 0x80) {
            GPIO_DATA_SET(HI_TRUE);
        } else {
            GPIO_DATA_SET(HI_FALSE);
        }
        GPIO_CLOCK_SET(HI_TRUE);
    }

    GPIO_DATA_SET(HI_FALSE);        /* send two bit nop */
    GPIO_CLOCK_SET(HI_FALSE);
    GPIO_CLOCK_SET(HI_TRUE);

    GPIO_DATA_SET(HI_FALSE);
    GPIO_CLOCK_SET(HI_FALSE);
    GPIO_CLOCK_SET(HI_TRUE);

    for (i = 0; i < 8; i++) {       /* send 8bit addr */
        GPIO_CLOCK_SET(HI_FALSE);
        if ((bit_character << i) & 0x80) {
            GPIO_DATA_SET(HI_TRUE);
        } else {
            GPIO_DATA_SET(HI_FALSE);
        }
        GPIO_CLOCK_SET(HI_TRUE);
    }

    GPIO_DATA_SET(HI_FALSE);        /* Output flip-latch Data */
    GPIO_DATA_SET(HI_TRUE);

    GPIO_CLOCK_SET(HI_FALSE);
    GPIO_DATA_SET(HI_FALSE);
    GPIO_DATA_SET(HI_TRUE);
    GPIO_CLOCK_SET(HI_TRUE);

    return;
}

hi_s32 keyled_ct1642_get_keycode(hi_void)
{
    hi_u8 i = 0;
    hi_s32 flag = 0;
    hi_s32 key_press = 0;
    hi_s32 key_code_sum = 0;
    hi_s32 const key_code[9] = {
        11, /* Key connect with pin Q9 */
        12, /* Key connect with pin Q8 */
        13, /* Key connect with pin Q7 */
        14, /* Key connect with pin Q6 */
        15, /* Key connect with pin Q5 */
        16, /* Key connect with pin Q4 */
        17, /* Key connect with pin Q3 */
        18, /* Key connect with pin Q2 */
        STB_KEY_NULL /* Key that not exist */
    };


    for (i = 0; i < 8; i++) { /* query which key */
        keyled_send_bit_data(1 << i, 0x04);
        udelay(5);
        GPIO_KEY_GET(key_press);

        if (key_press == 1) {
            flag++;
            key_code_sum += key_code[i];
        }
    }

    if (flag > 0) {
        return key_code_sum;
    } else { /* when none */
        return HI_FAILURE;
    }
}

hi_s32 keyled_ct1642_handle_keycode(hi_void)
{
    if (g_key_detected_flag == 0) {/* no key pressed in this period */
        if (g_key_code_last == KEY_MACRO_NO) { /* no key in last period */
            ;
        } else { /* has key in last period */
            if (g_key_press_flag) {
                /* report last key 'RELEASE', and recode no key pressed in this time */
                if (g_keyled_dev.enable_key_up) {
                    KEYLED_BUF_HEAD.keyled_code = g_key_code_last;
                    KEYLED_BUF_HEAD.keyled_state = KEY_RELEASE;
                    g_keyled_dev.head = KEYLED_INC_BUF(g_keyled_dev.head, g_keyled_dev.buf_len);

                    wake_up_interruptible(&g_wait_keyleds);
                }
                g_key_press_flag = HI_FALSE;
            } else { /*warning: key twitter*/
                ;
            }
            g_key_code_last = KEY_MACRO_NO;
        }
    } else { /* has key pressed in this period */
        if (g_key_code_curr != g_key_code_last) { /* new key */
            if (g_key_code_last == KEY_MACRO_NO) { /* no key in last scan */
                ;
            } else { /* has key in last scan */
                if (g_key_press_flag) { /* last key is valid */
                    if (g_keyled_dev.enable_key_up) {
                        KEYLED_BUF_HEAD.keyled_state = KEY_RELEASE;
                        KEYLED_BUF_HEAD.keyled_code = g_key_code_last;
                        g_keyled_dev.head = KEYLED_INC_BUF(g_keyled_dev.head, g_keyled_dev.buf_len);

                        wake_up_interruptible(&g_wait_keyleds);
                    }
                    g_key_press_flag = HI_FALSE;
                } else { /* warning: key twitter */
                    ;
                }
            }
            g_cycle_cnt = 0;
            g_key_code_last = g_key_code_curr;
        } else { /* old key */
            g_cycle_cnt ++;
            if ((g_cycle_cnt == 1) && (g_key_press_flag != HI_TRUE)) {
                KEYLED_BUF_HEAD.keyled_state = KEY_PRESS;
                KEYLED_BUF_HEAD.keyled_code = g_key_code_last;
                g_keyled_dev.head = KEYLED_INC_BUF(g_keyled_dev.head, g_keyled_dev.buf_len);

                wake_up_interruptible(&g_wait_keyleds);
                g_cycle_cnt = 0;
                g_key_press_flag = HI_TRUE;
            }

            if ((g_cycle_cnt >= g_keyled_dev.repeat_key_delay_time * 8 / (SCAN_INTV_CT1642)) &&
                    (g_key_press_flag)) {
                if (g_keyled_dev.enable_repeat_key) {
                    KEYLED_BUF_HEAD.keyled_state = KEY_HOLD;
                    KEYLED_BUF_HEAD.keyled_code = g_key_code_last;
                    g_keyled_dev.head = KEYLED_INC_BUF(g_keyled_dev.head, g_keyled_dev.buf_len);

                    wake_up_interruptible(&g_wait_keyleds);
                }

                g_cycle_cnt = 0;
                g_key_press_flag = HI_TRUE;
            }
        }
    }

    g_key_code_curr = KEY_MACRO_NO;
    g_key_detected_flag = 0;

    return HI_FAILURE;
}

static hi_void keyled_key_scan(hi_void)
{
    hi_s32 key_code = 0;

    key_code = keyled_ct1642_get_keycode();

    if (key_code == HI_FAILURE) {
        g_key_detected_flag = 0;
        g_key_code_curr = KEY_MACRO_NO;
    } else {
        g_key_detected_flag = 1;
        g_key_code_curr = key_code;
        g_keyled_dev.key_come++;
        HI_INFO_KEYLED("enter hi_keyled_read : head=%d, tail=%d, buf_len=%d, getkey:%d\n",
                       g_keyled_dev.head, g_keyled_dev.tail, g_keyled_dev.buf_len, g_key_code_curr);
    }

    keyled_ct1642_handle_keycode();

}

static hi_void keyled_led_scan(hi_u8 position)
{
    keyled_send_bit_data(g_led_code[position], position);

    return;
}

static hi_void keyled_led_display_off(hi_void)
{
    keyled_send_bit_data(0x00, 0x04);

    return;
}

hi_s32 keyled_ct1642_gpio_dirset(hi_void)
{
    if ((g_gpio_func != HI_NULL) && (g_gpio_func->pfn_gpio_direction_set_bit != HI_NULL)) {
        g_gpio_func->pfn_gpio_direction_set_bit(CLK_CT1642, 0); /* output */
        g_gpio_func->pfn_gpio_direction_set_bit(DAT_CT1642, 0); /* output */
        g_gpio_func->pfn_gpio_direction_set_bit(KEY_CT1642, 1); /* input */

        return 0;
    }

    HI_ERR_KEYLED("g_gpio_func or g_gpio_func->pfnGpioDirSetBit is NULL! \n");
    return  HI_FAILURE;
}

hi_void keyled_ct1642_clear_keybuf_func(hi_void)
{
    g_keyled_dev.tail = g_keyled_dev.head;
    g_keyled_dev.key_come = 0;
    g_keyled_dev.key_read = 0;

    return;
}

hi_s32 keyled_init_ct1642(hi_void)
{
    hi_s32 ret = HI_SUCCESS;

    ret = hi_drv_module_get_function(HI_ID_GPIO, (hi_void **)&g_gpio_func);
    if ((ret != HI_SUCCESS) || (g_gpio_func == HI_NULL)) {
        HI_ERR_KEYLED("Get GPIO function failed! \n");
        return HI_FAILURE;
    }

    init_waitqueue_head(&g_wait_keyleds);

    g_keyled_dev.head = 0;
    g_keyled_dev.tail = 0;
    g_keyled_dev.buf_len = KEYBUF_LEN;
    g_keyled_dev.enable_repeat_key = 1;
    g_keyled_dev.enable_key_up = 1;
    g_keyled_dev.key_come = 0;
    g_keyled_dev.key_read = 0;
    g_keyled_dev.block_time = DEFAULT_BLOCKTIME;
    g_keyled_dev.repeat_key_delay_time = DEFAULT_REP_KEY_TIME;

    keyled_ct1642_gpio_dirset();
    keyled_ct1642_clear_keybuf_func();

    if (g_timer_run_flag == HI_FALSE) {
        led_init_timer();
        HI_INFO_KEYLED("KEYLED_Init_CT1642  do  LED_InitTimer\n");
    }

    return HI_SUCCESS;
}

hi_s32 keyled_deinit_ct1642(hi_void)
{
    led_delete_timer();
    keyled_led_display_off();

    return HI_SUCCESS;
}

/* led display data */
hi_s32 keyled_led_display_ct1642(hi_u32 value)
{
    hi_u8 i = 0;

    for (i = 0; i < LED_BIT_COUNT; i++) {
        g_led_code[LED_BIT_COUNT - 1 - i] = ((hi_u32)value >> (8 * i)) & 0xff;
    }

    return HI_SUCCESS;
}

hi_s32 keyled_key_open_ct1642(hi_void)
{
    HI_INFO_KEYLED("KEYLED_KEY_Open_CT1642 --- g_ct1642_open = %d \n", g_ct1642_open);

    if (atomic_inc_return(&g_ct1642_open) == 1) {
        keyled_init_ct1642();
        HI_INFO_KEYLED("KEYLED ct1642 init \n");
    }

    return HI_SUCCESS;
}

hi_s32 keyled_key_close_ct1642(hi_void)
{
    HI_INFO_KEYLED("KEYLED_KEY_Close_CT1642 --- g_ct1642_open = %d \n", g_ct1642_open);

    if (atomic_dec_and_test(&g_ct1642_open)) {
        HI_INFO_KEYLED("KEYLED ct1642 deinit \n");
        keyled_deinit_ct1642();
    }

    if (atomic_read(&g_ct1642_open) < 0) {
        atomic_set(&g_ct1642_open, 0);
    }

    return HI_SUCCESS;
}

hi_s32 keyled_key_reset_ct1642(hi_void)
{
    keyled_ct1642_clear_keybuf_func();

    return HI_SUCCESS;
}

hi_s32 keyled_key_get_value_ct1642(hi_u32 *press_status_ptr, hi_u32 *key_id_ptr)
{
    keyled_key_info keyled_to_user = {0};
    hi_s32 err = 0;

    HI_INFO_KEYLED("enter hi_keyled_read : head=%d, tail=%d, buf_len=%d\n",
                   g_keyled_dev.head, g_keyled_dev.tail, g_keyled_dev.buf_len);

    if ((g_keyled_dev.head) == (g_keyled_dev.tail)) {
        if (g_keyled_dev.block_time == 0) {
            return HI_ERR_KEYLED_NO_NEW_KEY;
        }

        err = wait_event_interruptible_timeout(g_wait_keyleds, (g_keyled_dev.head != g_keyled_dev.tail),
                                               (hi_slong)(g_keyled_dev.block_time * HZ / 1000));

        if (err <= 0) {/*not wake up by received key*/
            HI_INFO_KEYLED("wake up by timeout\n");
            return HI_ERR_KEYLED_TIMEOUT;
        }
    }

    HI_INFO_KEYLED("enter hi_keyled_read : head=%d, tail=%d, buf_len=%d\n",
                   g_keyled_dev.head, g_keyled_dev.tail, g_keyled_dev.buf_len);

    if (g_keyled_dev.head != g_keyled_dev.tail) {
        keyled_to_user  = KEYLED_BUF_TAIL;
        g_keyled_dev.tail = KEYLED_INC_BUF(g_keyled_dev.tail, g_keyled_dev.buf_len);
        g_keyled_dev.key_read++;

        *press_status_ptr = keyled_to_user.keyled_state;
        *key_id_ptr = keyled_to_user.keyled_code;

        return HI_SUCCESS;
    } else {
        return HI_ERR_KEYLED_NO_NEW_KEY;
    }

    return HI_SUCCESS;
}

hi_s32 keyled_key_enable_repeat_key_ct1642(hi_u32 enable_repkey)
{
    g_keyled_dev.enable_repeat_key = enable_repkey;

    return HI_SUCCESS;
}

hi_s32 keyled_key_enable_key_up_ct1642(hi_u32 enable_keyup)
{
    g_keyled_dev.enable_key_up = enable_keyup;

    return HI_SUCCESS;
}

hi_s32 keyled_key_set_block_time_ct1642(hi_u32 block_time_ms)
{
    g_keyled_dev.block_time = block_time_ms;

    return HI_SUCCESS;
}

hi_s32 keyled_key_set_repeat_time_ct1642(hi_u32 repeat_time_ms)
{
    g_keyled_dev.repeat_key_delay_time = repeat_time_ms;

    return HI_SUCCESS;
}

hi_s32 keyled_led_open_ct1642(hi_void)
{
    HI_INFO_KEYLED("KEYLED_LED_Open_CT1642 --- g_ct1642_open = %d \n", g_ct1642_open);

    if (atomic_inc_return(&g_ct1642_open) == 1) {
        keyled_init_ct1642();
        HI_INFO_KEYLED("KEYLED ct1642 init \n");
    }

    return HI_SUCCESS;
}

hi_s32 keyled_led_close_ct1642(hi_void)
{
    HI_INFO_KEYLED("KEYLED_LED_Close_CT1642 --- g_ct1642_open = %d \n", g_ct1642_open);

    if (atomic_dec_and_test(&g_ct1642_open)) {
        HI_INFO_KEYLED("KEYLED ct1642 deinit \n");
        keyled_deinit_ct1642();
    }

    if (atomic_read(&g_ct1642_open) < 0) {
        atomic_set(&g_ct1642_open, 0);
    }

    return HI_SUCCESS;
}

hi_s32 keyled_led_set_flash_pin_ct1642(hi_keyled_light pin)
{
    return HI_SUCCESS;
}

hi_s32 keyled_led_set_flash_freq_ct1642(hi_keyled_level level)
{
    return HI_SUCCESS;
}

hi_s32 keyled_led_display_time_ct1642(hi_keyled_time display_time)
{
    hi_s32 index = 0;
    g_keyled_dev = display_time;

    index = g_keyled_dev.hour / 10;
    g_led_code[3] = g_led_dig_dis_dot_ct1642[index];
    index = g_keyled_dev.hour % 10;
    g_led_code[2] = g_led_dig_dis_dot_ct1642[index];

    index = g_keyled_dev.minute / 10;
    g_led_code[1] = g_led_dig_dis_dot_ct1642[index];
    index = g_keyled_dev.minute % 10;
    g_led_code[0] = g_led_dig_dis_dot_ct1642[index];

    return HI_SUCCESS;
}

hi_s32 keyled_get_proc_info_ct1642(keyled_proc_info *proc_info)
{
    proc_info->key_buf_size = g_keyled_dev.buf_len;
    proc_info->key_buf_head = g_keyled_dev.head;
    proc_info->key_buf_tail = g_keyled_dev.tail;
    proc_info->key_come_num = g_keyled_dev.key_come;
    proc_info->key_read_num = g_keyled_dev.key_read;
    proc_info->repeat_key_time_ms = g_keyled_dev.repeat_key_delay_time;
    proc_info->is_repeat_key_enable = g_keyled_dev.enable_repeat_key;
    proc_info->is_up_key_enable = g_keyled_dev.enable_key_up;
    proc_info->block_time = g_keyled_dev.block_time;
    proc_info->flash_pin = g_keyled_dev.flash_pin;
    proc_info->flash_level = g_keyled_dev.flash_level;
    proc_info->key_buf_tail = g_keyled_dev.tail;

    return HI_SUCCESS;
}

hi_s32 keyled_suspend_ct1642(hi_void)
{
    keyled_ct1642_clear_keybuf_func();

    return HI_SUCCESS;
}

hi_s32 keyled_resume_ct1642(hi_void)
{
    /* del by sdk for the settings of GPIO5 do not lose when power up */
    return HI_SUCCESS;
}

#ifndef MODULE

EXPORT_SYMBOL(keyled_key_open_ct1642);
EXPORT_SYMBOL(keyled_key_close_ct1642);
EXPORT_SYMBOL(keyled_key_reset_ct1642);
EXPORT_SYMBOL(keyled_key_get_value_ct1642);
EXPORT_SYMBOL(keyled_key_set_block_time_ct1642);
EXPORT_SYMBOL(keyled_key_set_repeat_time_ct1642);
EXPORT_SYMBOL(keyled_key_enable_repeat_key_ct1642);
EXPORT_SYMBOL(keyled_key_enable_key_up_ct1642);

EXPORT_SYMBOL(keyled_led_open_ct1642);
EXPORT_SYMBOL(keyled_led_close_ct1642);
EXPORT_SYMBOL(keyled_led_display_ct1642);
EXPORT_SYMBOL(keyled_led_display_time_ct1642);
EXPORT_SYMBOL(keyled_led_set_flash_pin_ct1642);
EXPORT_SYMBOL(keyled_led_set_flash_freq_ct1642);
EXPORT_SYMBOL(keyled_suspend_ct1642);
EXPORT_SYMBOL(keyled_resume_ct1642);
#endif

