/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: adsp audsp common header file
 * Author: Audio
 * Create: 2019-11-11
 * Notes: NA
 * History: 2019-11-11 CSEC Rectification
 */

#ifndef __HI_KFIFO_H__
#define __HI_KFIFO_H__

#ifdef __KERNEL__
#include <linux/uaccess.h>
#include <linux/string.h>

#include "hi_drv_file.h"
#elif defined(__XTENSA__)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif

#include "hi_stdlib.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#ifndef min
#define min(a, b) ((a) <= (b) ? (a) : (b))
#endif

#ifndef is_power_of_two
#define is_power_of_two(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))
#endif

#define KFIFO_SIZE_MIN 2

typedef struct {
    unsigned int *in;  /* fifo write pointer */
    unsigned int *out; /* fifo read pointer */
    unsigned int mask; /* fifo length mask */
    hi_void *data;     /* fifo buffer address */
} kfifo;

static inline unsigned int kfifo_roundup_pow_of_two(unsigned int x)
{
    unsigned int ret = KFIFO_SIZE_MIN;
    while (ret < x) {
        ret <<= 1;
    }
    return ret;
}

static inline int kfifo_init(kfifo *fifo, unsigned int *in, unsigned int *out, hi_void *buffer, unsigned int size)
{
    if (size < KFIFO_SIZE_MIN) {
        return -1;
    }

    if (!is_power_of_two(size)) {
        return -1;
    }

    if (fifo == NULL || in == NULL || out == NULL || buffer == NULL) {
        return -1;
    }

    fifo->in = in;
    fifo->out = out;
    fifo->mask = size - 1;
    fifo->data = buffer;

    return 0;
}

static inline hi_void kfifo_deinit(kfifo *fifo)
{
    if (fifo == NULL) {
        return;
    }

    fifo->in = 0;
    fifo->out = 0;
}

static inline hi_void kfifo_reset(kfifo *fifo)
{
    if (fifo == NULL) {
        return;
    }

    if (fifo->out == NULL || fifo->in == NULL) {
        return;
    }

    *fifo->out = *fifo->in;
}

/* 下面的接口，不判断kfifo结构体指针和内部成员指针的合法性，请调用者保证 */
static inline unsigned int kfifo_unused(kfifo *fifo)
{
    return (fifo->mask + 1) - (*fifo->in - *fifo->out);
}

static inline unsigned int kfifo_used(kfifo *fifo)
{
    return (unsigned int)(*fifo->in - *fifo->out);
}

static inline unsigned int kfifo_copy_in(kfifo *fifo, const hi_void *src, unsigned int len, unsigned int off)
{
    int ret;
    unsigned int l = min(len, fifo->mask + 1 - off);

    if (src == NULL) {
        /* generate mute data if input buffer is HI_NULL */
        ret = memset_s(fifo->data + off, l, 0, l);
        if (ret != EOK) {
            return 0;
        }

        if (len - l == 0) {
            return len;
        }

        ret = memset_s(fifo->data, len - l, 0, len - l);
        if (ret != EOK) {
            return 0;
        }
    } else {
        ret = memcpy_s(fifo->data + off, l, src, l);
        if (ret != EOK) {
            return 0;
        }

        if (len - l == 0) {
            return len;
        }

        ret = memcpy_s(fifo->data, len - l, src + l, len - l);
        if (ret != EOK) {
            return 0;
        }
    }

    return len;
}

static inline unsigned int kfifo_in(kfifo *fifo, const hi_void *buf, unsigned int len)
{
    if (len == 0) {
        return 0;
    }

    if (kfifo_unused(fifo) < len) {
        return 0;
    }

    if (kfifo_copy_in(fifo, buf, len, (*fifo->in) & fifo->mask) != len) {
        return 0;
    }

    *fifo->in += len;
    return len;
}

static inline unsigned int kfifo_copy_out(kfifo *fifo, hi_void *dst, unsigned int len, unsigned int off)
{
    int ret;
    unsigned int l = min(len, fifo->mask + 1 - off);

    if (dst == NULL) {
        /* dump data in fifo */
        return len;
    }

    ret = memcpy_s(dst, l, fifo->data + off, l);
    if (ret != EOK) {
        return 0;
    }

    /* read finished */
    if (len - l == 0) {
        return len;
    }

    ret = memcpy_s(dst + l, len - l, fifo->data, len - l);
    if (ret != EOK) {
        return 0;
    }

    return len;
}

static inline unsigned int kfifo_out(kfifo *fifo, hi_void *buf, unsigned int len)
{
    if (kfifo_used(fifo) < len) {
        return 0;
    }

    if (kfifo_copy_out(fifo, buf, len, (*fifo->out) & fifo->mask) != len) {
        return 0;
    }

    *fifo->out += len;
    return len;
}

static inline unsigned int kfifo_peek(kfifo *fifo, hi_void *buf, unsigned int len)
{
    if (kfifo_used(fifo) < len) {
        return 0;
    }

    if (kfifo_copy_out(fifo, buf, len, (*fifo->out) & fifo->mask) != len) {
        return 0;
    }

    return len;
}

static inline hi_void kfifo_poke(kfifo *fifo, const unsigned int len)
{
    *fifo->out += len;
}

/* 支持seek的kfifo，要求buffer分配的足够大，输入的文件不会填满buffer */
static inline unsigned int kfifo_seek(kfifo *fifo, unsigned int pos)
{
    if (*fifo->in < pos) {
        return 0;
    }

    *fifo->out = pos;

    return pos;
}

static inline unsigned int kfifo_in_with_action(kfifo *fifo,
    unsigned int (*action)(hi_void *to, const hi_void *from, unsigned int size), const hi_void *buf, unsigned int len)
{
    unsigned int l;
    unsigned int off = ((*fifo->in) & fifo->mask);

    if ((len == 0) || (buf == NULL)) {
        return 0;
    }

    if (kfifo_used(fifo) < len) {
        return 0;
    }

    l = min(len, fifo->mask + 1 - off);

    action(fifo->data + off, buf, l);
    action(fifo->data, buf + l, len - l);

    *fifo->in += len;
    return len;
}

static inline unsigned int kfifo_out_with_action(kfifo *fifo,
    unsigned int (*action)(hi_void *to, const hi_void *from, unsigned int size), hi_void *buf, unsigned int len)
{
    unsigned int l;
    unsigned int off = ((*fifo->out) & fifo->mask);

    if ((len == 0) || (buf == NULL)) {
        return 0;
    }

    if (kfifo_used(fifo) < len) {
        return 0;
    }

    l = min(len, fifo->mask + 1 - off);

    action(buf, fifo->data + off, l);
    action(buf + l, fifo->data, len - l);

    *fifo->out += len;
    return len;
}

#ifdef __KERNEL__
static inline unsigned int action_copy_from_user(hi_void *to, const hi_void *from, unsigned int size)
{
    if (size == 0) {
        return 0;
    }

    if (copy_from_user(to, from, size)) {
        return 0;
    }

    return size;
}

static inline unsigned int action_copy_to_user(hi_void *to, const hi_void *from, unsigned int size)
{
    if (size == 0) {
        return 0;
    }

    if (copy_to_user(to, from, size)) {
        return 0;
    }

    return size;
}

static inline unsigned int kfifo_copy_from_user(kfifo *fifo, const hi_void *src, unsigned int len)
{
    return kfifo_in_with_action(fifo, action_copy_from_user, src, len);
}

static inline unsigned int kfifo_copy_to_user(kfifo *fifo, hi_void *dst, unsigned int len)
{
    return kfifo_out_with_action(fifo, action_copy_to_user, dst, len);
}

static inline unsigned int kfifo_copy_from_file(kfifo *fifo, struct file *file, unsigned int len)
{
    int l1 = 0;
    int l2 = 0;
    unsigned int off = ((*fifo->in) & fifo->mask);
    unsigned int l = min(len, fifo->mask + 1 - off);

    if (file == NULL) {
        return kfifo_in(fifo, NULL, len);
    }

    if (kfifo_unused(fifo) < len) {
        return 0;
    }

    if (l) {
        l1 = hi_drv_file_read(file, fifo->data + off, l);
        if (l1 < 0) {
            return 0;
        }
    }

    if (len - l) {
        l2 = hi_drv_file_read(file, fifo->data, len - l);
        if (l2 < 0) {
            return 0;
        }
    }

    *fifo->in += (l1 + l2);
    return (l1 + l2);
}

static inline unsigned int kfifo_copy_to_file(kfifo *fifo, struct file *file, unsigned int len)
{
    unsigned int off = ((*fifo->out) & fifo->mask);
    unsigned int l = min(len, fifo->mask + 1 - off);

    if (kfifo_used(fifo) < len) {
        return 0;
    }

    /* dump data in fifo if file is NULL */
    if (file != NULL) {
        if (l != 0) {
            hi_drv_file_write(file, fifo->data + off, l);
        }

        if ((len - l) != 0) {
            hi_drv_file_write(file, fifo->data, len - l);
        }
    }

    *fifo->out += len;
    return len;
}
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif
