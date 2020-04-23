/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019. All rights reserved.
 * Description: hdmi driver sysfs source file
 * Author: Hisilicon multimedia interface software group
 * Create: 2019-06-22
 */

#include "drv_hdmitx.h"
#include "osal_list.h"

static struct bin_attribute g_edid_attrs = {
    .attr = { .name = "edid_rawdata", .mode = 0400 }, /* 0400:chmod 0400 */
    .size = 0,
    .read = NULL,
    .write = NULL,
    .mmap = NULL,
    .private = (void *)0
};

static struct bin_attribute *g_hdmi_bin_attributes[] = {
    &g_edid_attrs,
    NULL
};

static ssize_t sysfs_oe_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return 0;
}

static ssize_t sysfs_oe_store(struct device *dev,
                              struct device_attribute *attr,
                              const char *buf, size_t count)
{
    int ret = 0;
    struct miscdevice *misc = dev_get_drvdata(dev);
    struct hisilicon_hdmi *hdmi = osal_container_of(misc, struct hisilicon_hdmi, miscdev);
    (void)hdmi;

    return ret;
}

static DEVICE_ATTR_RW(sysfs_oe);

static struct attribute *g_hdmi_attributes[] = {
    &dev_attr_sysfs_oe.attr,
    NULL
};

static const struct attribute_group g_hdmi_attr_group = {
    .name = "settings",
    .bin_attrs = g_hdmi_bin_attributes,
    .attrs = g_hdmi_attributes,
};

int drv_hdmitx_sysfs_init(struct hisilicon_hdmi *hdmi)
{
    return sysfs_create_group(&hdmi->miscdev.this_device->kobj, &g_hdmi_attr_group);
}

void drv_hdmitx_sysfs_exit(struct hisilicon_hdmi *hdmi)
{
    sysfs_remove_group(&hdmi->miscdev.this_device->kobj, &g_hdmi_attr_group);
}
