#include <linux/device.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/delay.h>
#include <linux/poll.h>
#include <linux/interrupt.h>

#include <linux/types.h>
#include <linux/sched.h>
#include <linux/spinlock.h>

#include "hi_drv_ir.h"
#include "drv_ir_protocol.h"
#include "drv_ir_utils.h"
#include "hi_drv_stat.h"

key_attr *ir_get_write_key_n(ir_buffer *head, hi_u32 n)
{
    if (head->writer + n >= MAX_SYMBOL_NUM) {
        n = head->writer + n - MAX_SYMBOL_NUM;
    } else {
        n = head->writer + n;
    }

    return &head->buf[n];
}

key_attr *ir_first_write_key(ir_buffer *head)
{
    key_attr *key = NULL;

    if (head->reader == head->writer) {
        return NULL;
    }

    key = &head->buf[head->writer];

    return key;
}

key_attr *ir_first_read_key(ir_buffer *head)
{
    key_attr *key = NULL;

    if (head->reader == head->writer) {
        return NULL;
    }

    key = &head->buf[head->reader];

    return key;
}

key_attr *ir_get_read_key_n(ir_buffer *head, hi_u32 n)
{
    if (head->reader + n >= MAX_SYMBOL_NUM) {
        n = head->reader + n - MAX_SYMBOL_NUM;
    } else {
        n = head->reader + n;
    }

    return &head->buf[n];
}

key_attr *ir_get_prev_write_key(ir_buffer *head)
{
    if (head->writer == 0) {
        return &head->buf[MAX_SYMBOL_NUM - 1];
    }

    return &head->buf[head->writer - 1];
}

static DEFINE_SPINLOCK(buffer_lock);
hi_void ir_insert_key_tail(ir_buffer *head, key_attr *key)
{
    hi_ulong       flags;

    spin_lock_irqsave(&buffer_lock, flags);

    head->buf[head->writer].upper = key->upper;
    head->buf[head->writer].lower = key->lower;
    head->buf[head->writer].key_stat = key->key_stat;

    if (memcpy_s(head->buf[head->writer].protocol_name, PROTOCOL_NAME_SZ, key->protocol_name,
                 PROTOCOL_NAME_SZ)) {
        hiir_error("memcpy_s to protocol_name failed\n");
        spin_unlock_irqrestore(&buffer_lock, flags);
        return;
    }

    head->writer++;
    if (head->writer == MAX_SYMBOL_NUM) {
        head->writer = 0;
    }

    if (head == g_ir_local.key_buf) {
        if (key->key_stat == KEY_STAT_DOWN) {
            hi_drv_stat_event(HI_STAT_EVENT_KEY_IN, (hi_u32)key->lower);
        }
        hiir_debug("new key:[%llu, %llu, %d]. next writer:%d\n",
                   key->lower, key->upper, key->key_stat, head->writer);
    } else {
        hiir_debug("new symbol:[%llu, %llu, %d]. next writer:%d\n",
                   key->lower, key->upper, key->key_stat, head->writer);
    }

    spin_unlock_irqrestore(&buffer_lock, flags);
}

key_attr *ir_next_reader_clr_inc(ir_buffer *head)
{
    key_attr *key = NULL;
    hi_ulong flags;

    spin_lock_irqsave(&buffer_lock, flags);
    key = &head->buf[head->reader];
    key->upper = 0;
    key->lower = 0;
    head->reader++;
    if (head->reader >= MAX_SYMBOL_NUM) {
        head->reader = 0;
    }

    key = &head->buf[head->reader];
    spin_unlock_irqrestore(&buffer_lock, flags);
    return key;
}

key_attr *ir_reader_inc(ir_buffer *head)
{
    key_attr *key = NULL;
    hi_ulong flags;

    spin_lock_irqsave(&buffer_lock, flags);
    key = &head->buf[head->reader];
    head->reader++;
    if (head->reader >= MAX_SYMBOL_NUM) {
        head->reader = 0;
    }

    spin_unlock_irqrestore(&buffer_lock, flags);
    return key;
}

EXPORT_SYMBOL(ir_get_write_key_n);
EXPORT_SYMBOL(ir_first_write_key);
EXPORT_SYMBOL(ir_first_read_key);
EXPORT_SYMBOL(ir_get_read_key_n);
EXPORT_SYMBOL(ir_get_prev_write_key);
EXPORT_SYMBOL(ir_insert_key_tail);
EXPORT_SYMBOL(ir_next_reader_clr_inc);
EXPORT_SYMBOL(ir_reader_inc);
