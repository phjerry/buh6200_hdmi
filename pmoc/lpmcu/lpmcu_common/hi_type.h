/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: type definition file
 */

#ifndef __HI_TYPE_H__
#define __HI_TYPE_H__

#ifndef NULL
#define NULL                0U
#endif

#define HI_NULL             0U
#define HI_NULL_PTR         0U

#define HI_SUCCESS          0
#define HI_FAILURE          (-1)

typedef unsigned char       hi_u8;
typedef unsigned char       hi_uchar;
typedef unsigned int        hi_u16;
typedef unsigned long       hi_u32;

typedef char                hi_char;
typedef signed char         hi_s8;
typedef int                 hi_s16;
typedef long                hi_s32;

typedef void                hi_void;

/* struct */
typedef union {
    hi_u32 val32;
    hi_u16 val16[2];
    hi_u8 val8[4];
} hi_u32_data;

typedef enum {
    HI_FALSE = 0,
    HI_TRUE  = 1
} hi_bool;

#endif /* __HI_TYPE_H__ */

