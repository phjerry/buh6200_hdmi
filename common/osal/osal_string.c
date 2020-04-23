/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:osal_math
* Author: seg
* Create: 2019-10-11
*/

#include "hi_osal.h"
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/random.h>

int osal_memncmp(const void *buf1, unsigned long size1,
    const void *buf2, unsigned long size2)
{
    if (size1 != size2) {
        return -1;
    }

    return memcmp(buf1, buf2, size1);
}
EXPORT_SYMBOL(osal_memncmp);

int osal_strncmp(const char *str1, unsigned long size1,
    const char *str2, unsigned long size2)
{
    if (size1 != size2) {
        return -1;
    }

    return strncmp(str1, str2, size1);
}
EXPORT_SYMBOL(osal_strncmp);

int osal_strncasecmp(const char *str1, unsigned long size1,
    const char *str2, unsigned long size2)
{
    if (size1 != size2) {
        return -1;
    }

    return strncasecmp(str1, str2, size1);
}
EXPORT_SYMBOL(osal_strncasecmp);

long osal_strtol(const char *str, char **end, unsigned int base)
{
    return simple_strtol(str, end, base);
}
EXPORT_SYMBOL(osal_strtol);

unsigned long  osal_strtoul(const char *str, char **end, unsigned int base)
{
    return simple_strtoul(str, end, base);
}
EXPORT_SYMBOL(osal_strtoul);

void osal_get_random_bytes(void *buf, int nbytes)
{
    get_random_bytes(buf, nbytes);
}
EXPORT_SYMBOL(osal_get_random_bytes);
