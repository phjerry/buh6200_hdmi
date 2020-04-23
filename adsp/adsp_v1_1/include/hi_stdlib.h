/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: memory function for sw and hw adsp
 * Author: Audio
 * Create: 2019-11-11
 * Notes: NA
 * History: 2019-11-11 first version
 */

#ifndef __HI_STDLIB_H__
#define __HI_STDLIB_H__

/* Success */
#ifndef EOK
#define EOK 0
#endif

#if defined(__XTENSA__)

#include <string.h>
#include <xtensa/hal.h>

/* only use these in hifi */
#define __hi_arm_ptr__

#ifndef typeof
#define typeof __typeof__
#endif

/*
 * @type: the type of the structure
 * @member: the member within the structure to get the offset of
 */
#ifndef offsetof
#define offsetof(type, member) ((unsigned int)&((type *)0)->member)
#endif

/*
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 */
#ifndef container_of
#define container_of(ptr, type, member) ({          \
    const typeof(((type *)0)->member) *__mptr = (ptr);    \
    (type *)((char *)__mptr - offsetof(type, member)); })
#endif

/* DSP运行空间起始物理地址 */
#define DSP_MEM_ADDR_START 0x16f00000
#define DSP_MEM_ADDR_END   (0x16f00000 + 0x800000)

#define to_pointer(offset) ((hi_void *)((unsigned char *)NULL + (offset)))

/* 判断所给的buffer指针是否在DSP运行空间内部 */
#define is_dsp_mem(p) (((p) >= to_pointer(DSP_MEM_ADDR_START)) && ((p) <= to_pointer(DSP_MEM_ADDR_END)))

/* only use this function in hifi */
static inline int memset_s(hi_void *dest, size_t destMax, int c, size_t count)
{
    if (count == 0) {
        return EOK;
    }

    adsp_memset(dest, c, count);

    if (!is_dsp_mem(dest)) {
        xthal_dcache_region_writeback(__hi_arm_ptr__ dest, count);
    }

    return EOK;
}

/* only use this function in hifi */
static inline int memcpy_s(hi_void *dest, size_t destMax, const hi_void *src, size_t count)
{
    if (count == 0) {
        return EOK;
    }

    if (!is_dsp_mem(src)) {
        xthal_dcache_region_invalidate(__hi_arm_ptr__ (hi_void *)src, count);
    }

    adsp_memcpy(dest, src, count);

    if (!is_dsp_mem(dest)) {
        xthal_dcache_region_writeback(__hi_arm_ptr__ dest, count);
    }

    return EOK;
}

#else /* kernel */
/*
 * we are unable to include securec.h in kernel during adsp release
 * so we have to delare securec function ourself
 */
#ifdef __aarch64__
int memset_s(hi_void *dest, unsigned long destMax, int c, unsigned long count);
int memcpy_s(hi_void *dest, unsigned long destMax, const hi_void *src, unsigned long count);
#else
int memset_s(hi_void *dest, unsigned int destMax, int c, unsigned int count);
int memcpy_s(hi_void *dest, unsigned int destMax, const hi_void *src, unsigned int count);
#endif
#endif /* __XTENSA__ */
#endif /* __HI_STDLIB_H__ */
