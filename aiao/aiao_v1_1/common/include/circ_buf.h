/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: audio circle buffer function
 * Author: audio
 * Create: 2019-05-30
 * Notes: NA
 * History: 2019-05-30 sdk for Hi3796CV300
 */

#ifndef __CIRC_BUF_H__
#define __CIRC_BUF_H__

#include "hi_drv_audio.h"

#if defined(__KERNEL__)
#include "hi_osal.h"
#else
#include "securec.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifndef min
#define min(a, b) ((a) <= (b) ? (a) : (b))
#endif

typedef struct {
    volatile hi_u32 *write;  /* circle buffer write pointer */
    volatile hi_u32 *read;   /* circle buffer read pointer */
    hi_u8 *data;             /* circle buffer data virtual address */
    hi_u32 size;             /* circle buffer size */
} circ_buf;

static inline hi_u32 saturate_add(hi_u32 x, hi_u32 y, hi_u32 saturation)
{
    /* add x and y, saturate to saturation */
    x += y;

    return (x >= saturation) ? (x - saturation) : x;
}

static inline hi_void circ_buf_init(circ_buf *cb,
    hi_u32 *write, hi_u32 *read, hi_void *data, hi_u32 len)
{
    cb->write = write;
    cb->read = read;
    cb->data = data;
    cb->size = len;

    *cb->write = 0;
    *cb->read = 0;
}

static inline hi_void circ_buf_flush(circ_buf *cb)
{
    *cb->read = 0;
    *cb->write = 0;
}

static inline hi_u32 circ_buf_data_size(hi_u32 write, hi_u32 read, hi_u32 size)
{
    return (write >= read) ? (write - read) : (size - read + write);
}

static inline hi_u32 circ_buf_free_size(hi_u32 write, hi_u32 read, hi_u32 size)
{
    return size - circ_buf_data_size(write, read, size);
}

/* read data from circle buffer */
static inline hi_u32 circ_buf_read_data(circ_buf *cb, hi_u8 *to,
    hi_u32 len, hi_u32 off)
{
    hi_u32 l;
    errno_t ret;

    if (to == HI_NULL) {
        /* dump data in circle buffer */
        return len;
    }

    l = min(len, cb->size - off);

    ret = memcpy_s(to, l, cb->data + off, l);
    if (ret != EOK) {
        return 0;
    }

    /* read finished */
    if (len - l == 0) {
        return len;
    }

    ret = memcpy_s(to + l, len - l, cb->data, len - l);
    if (ret != EOK) {
        return 0;
    }

    return len;
}

typedef hi_u32 (*circ_buf_write_data_func)(circ_buf *cb, const hi_u8 *from,
    hi_u32 len, hi_u32 off);

/*
 * circ_buf_write_data - puts some data from kernel space into the circle buffer
 * @cb: address of the circle buffer to be used
 * @from: pointer to the data to be added
 * @len: the length of the data to be added
 * @off: data write offset in circle buffer
 *
 * Note: Check free space in circle buffer before calling this function
 */
static inline hi_u32 circ_buf_write_data(circ_buf *cb, const hi_u8 *from,
    hi_u32 len, hi_u32 off)
{
    errno_t ret;
    hi_u32 l = min(len, cb->size - off);

    if (from == HI_NULL) {
        /* generate mute data if input buffer is HI_NULL */
        ret = memset_s(cb->data + off, l, 0, l);
        if (ret != EOK) {
            return 0;
        }

        if (len - l == 0) {
            return len;
        }

        ret = memset_s(cb->data, len - l, 0, len - l);
        if (ret != EOK) {
            return 0;
        }
    } else {
        ret = memcpy_s(cb->data + off, l, from, l);
        if (ret != EOK) {
            return 0;
        }

        if (len - l == 0) {
            return len;
        }

        ret = memcpy_s(cb->data, len - l, from + l, len - l);
        if (ret != EOK) {
            return 0;
        }
    }

    return len;
}

/*
 * circ_buf_from_user - puts some data from user space into the circle buffer
 * @cb: address of the circle buffer to be used
 * @from: pointer to the data to be added
 * @len: the length of the data to be added
 * @off: data write offset in circle buffer
 *
 * Note: Check free space in circle buffer before calling this function
 */
static inline hi_u32 circ_buf_from_user(circ_buf *cb, const hi_u8 *from,
    hi_u32 len, hi_u32 off)
{
    errno_t ret;
    hi_u32 l = min(len, cb->size - off);

    if (from == HI_NULL) {
        /* generate mute data if input buffer is HI_NULL */
        ret = memset_s(cb->data + off, l, 0, l);
        if (ret != EOK) {
            return 0;
        }

        if (len - l == 0) {
            return len;
        }

        ret = memset_s(cb->data, len - l, 0, len - l);
        if (ret != EOK) {
            return 0;
        }
    } else {
#if defined(__KERNEL__) && !defined(HI_AIAO_CBB)
        if (osal_copy_from_user(cb->data + off, from, l)) {
            return 0;
        }

        if (osal_copy_from_user(cb->data, from + l, len - l)) {
            return 0;
        }
#endif
    }

    return len;
}

static inline hi_u32 circ_buf_query_busy(circ_buf *cb)
{
    return circ_buf_data_size(*cb->write, *cb->read, cb->size);
}

static inline hi_u32 circ_buf_query_free(circ_buf *cb)
{
    return circ_buf_free_size(*cb->write, *cb->read, cb->size);
}

static inline hi_u32 circ_buf_read_with_ext_pos(circ_buf *cb, hi_u8 *to,
    hi_u32 len, hi_u32 *ext_read_pos, hi_u32 *ext_write_pos)
{
    hi_u32 read = *ext_read_pos;
    hi_u32 write = *cb->write;

    if (len == 0) {
        return 0;
    }

    if (circ_buf_data_size(write, read, cb->size) < len) {
        return 0;
    }

    if (circ_buf_read_data(cb, to, len, read) != len) {
        return 0;
    }

    *ext_write_pos = write;
    *ext_read_pos = saturate_add(read, len, cb->size);

    return len;
}

/* read data from circle buffer */
static inline hi_u32 circ_buf_read(circ_buf *cb, hi_u8 *to, hi_u32 len)
{
    hi_u32 read = *cb->read;
    hi_u32 write = *cb->write;

    if (len == 0) {
        return 0;
    }

    if (circ_buf_data_size(write, read, cb->size) < len) {
        return 0;
    }

    if (circ_buf_read_data(cb, to, len, read) != len) {
        return 0;
    }

    *cb->read = saturate_add(read, len, cb->size);

    return len;
}

/* write data into circle buffer */
static inline hi_u32 circ_buf_write(circ_buf *cb, const hi_u8 *from,
    hi_u32 len, hi_bool data_from_kernel)
{
    hi_u32 write = *cb->write;
    circ_buf_write_data_func write_data_func = HI_NULL;

    if (len == 0) {
        return 0;
    }

    if (data_from_kernel == HI_TRUE) {
        write_data_func = circ_buf_write_data;
    } else {
        write_data_func = circ_buf_from_user;
    }

    if (write_data_func(cb, from, len, write) != len) {
        return 0;
    }

    *cb->write = saturate_add(write, len, cb->size);

    return len;
}

static inline hi_u32 circ_buf_cast_read(circ_buf *cb, hi_u32 *data_offset, hi_u32 len)
{
    hi_u32 read = *cb->read;
    hi_u32 write = *cb->write;

    if (circ_buf_data_size(write, read, cb->size) < len) {
        *data_offset = 0;
        return 0;
    }

    *data_offset = read;
    return len;
}

static inline hi_u32 circ_buf_cast_relese(circ_buf *cb, hi_u32 len)
{
    hi_u32 read = *cb->read;
    hi_u32 write = *cb->write;
    hi_u32 data_size = circ_buf_data_size(write, read, cb->size);

    len = min(data_size, len);
    *cb->read = saturate_add(read, len, cb->size);

    return len;
}

static inline hi_u32 circ_buf_update_read_pos(circ_buf *cb, hi_u32 len)
{
    *cb->read = saturate_add(*cb->read, len, cb->size);
    return len;
}

static inline hi_u32 circ_buf_update_write_pos(circ_buf *cb, hi_u32 len)
{
    *cb->write = saturate_add(*cb->write, len, cb->size);
    return len;
}

/* generate mute data before read position, return zero if not enough free space */
static inline hi_void circ_buf_write_mute_at_read_pos(circ_buf *cb, hi_u32 len)
{
    hi_u32 read = *cb->read;
    hi_u32 write = *cb->write;

    /* we can not full circle buffer */
    if (circ_buf_free_size(write, read, cb->size) <= len) {
        return;
    }

    /* calculate new read position */
    read = (read >= len) ? (read - len) : (cb->size + read - len);

    /* write mute data from new read position */
    if (circ_buf_write_data(cb, HI_NULL, len, read) != len) {
        return;
    }

    *cb->read = read;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef __CIRC_BUF_H__ */
