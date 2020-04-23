/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author: p00370051
* Create: 2012-12-22
* Notes:
* History: 2019-03-29 p00370051 CSEC
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/bootmem.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/version.h>
#include "himedia_base.h"
#include "linux/hisilicon/securec.h"


/*********************************************************************/
/* himedia bus */
/*********************************************************************/

static void himedia_bus_release(struct device *dev)
{
    return;
}

struct device g_himedia_bus = {
    .init_name = "himedia_bus_dev",
    .release = himedia_bus_release
};
/* top level bus, parent and bus member are both NULL */ /* CNcomment:这是顶层总线，parent 和 bus 成员为 NULL */
static ssize_t modalias_show(struct device *dev, struct device_attribute *a,
                             char *buf)
{
    pm_basedev *pdev = TO_PM_BASEDEV(dev);
    int len = snprintf_s(buf, PAGE_SIZE, (PAGE_SIZE - 1), "himedia:%s\n", (char *)pdev->name);
    if (len < 0) {
        printk("modalias_show call snprintf_s failed!\n");
    }

    return (len >= PAGE_SIZE) ? (PAGE_SIZE - 1) : len;
}


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
static DEVICE_ATTR_RO(modalias);

static struct attribute *g_himedia_dev_attrs[] = {
    &dev_attr_modalias.attr,
    NULL,
};

ATTRIBUTE_GROUPS(g_himedia_dev);
#else
static struct device_attribute g_himedia_dev_attrs[] = {
    __ATTR_RO(modalias),
    __ATTR_NULL,
};
#endif

static int himedia_match(struct device *dev, struct device_driver *drv)
{
    pm_basedev *pdev = TO_PM_BASEDEV(dev);
    return (strncmp(pdev->name, drv->name, HIMEDIA_DEVICE_NAME_MAX_LEN + 8) == 0); /* 8 用于计算drv->name的长度 */
}

static int himedia_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    int ret;
    pm_basedev *pdev = TO_PM_BASEDEV(dev);
    ret = add_uevent_var(env, "MODALIAS=himedia:%s", pdev->name);
    if (ret) {
        return ret;
    }
    return 0;
}

#if 1
static int himedia_legacy_suspend(struct device *dev, pm_message_t mesg)
{
    int ret = 0;

    if (dev->driver && dev->driver->suspend) {
        ret = dev->driver->suspend(dev, mesg);
    }

    return ret;
}

static int himedia_legacy_suspend_late(struct device *dev, pm_message_t mesg)
{
    int ret = 0;
    pm_basedev *pdev = TO_PM_BASEDEV(dev);
    pm_basedrv *pdrv = to_himedia_basedrv(dev->driver);

    if (dev->driver && pdrv->suspend_late) {
        ret = pdrv->suspend_late(pdev, mesg);
    }

    return ret;
}

static int himedia_legacy_resume_early(struct device *dev)
{
    int ret = 0;
    pm_basedev *pdev = TO_PM_BASEDEV(dev);
    pm_basedrv *pdrv = to_himedia_basedrv(dev->driver);

    if (dev->driver && pdrv->resume_early) {
        ret = pdrv->resume_early(pdev);
    }

    return ret;
}

static int himedia_legacy_resume(struct device *dev)
{
    int ret = 0;

    if (dev->driver && dev->driver->resume) {
        ret = dev->driver->resume(dev);
    }

    return ret;
}

static int himedia_pm_prepare(struct device *dev)
{
    struct device_driver *drv = dev->driver;
    int ret = 0;

    if (drv != NULL && drv->pm && drv->pm->prepare) {
        ret = drv->pm->prepare(dev);
    }

    return ret;
}

static void himedia_pm_complete(struct device *dev)
{
    struct device_driver *drv = dev->driver;

    if (drv != NULL && drv->pm && drv->pm->complete) {
        drv->pm->complete(dev);
    }
}

#if 1

static int himedia_pm_suspend(struct device *dev)
{
    int ret = 0;
    struct device_driver *drv = dev->driver;

    if (drv == HI_NULL) {
        return 0;
    }

    if (drv->pm) {
        if (drv->pm->suspend) {
            ret = drv->pm->suspend(dev);
        }
    } else {
        ret = himedia_legacy_suspend(dev, PMSG_SUSPEND);
    }

    return ret;
}

static int himedia_pm_suspend_noirq(struct device *dev)
{
    int ret = 0;
    struct device_driver *drv = dev->driver;

    if (drv == HI_NULL) {
        return 0;
    }

    if (drv->pm) {
        if (drv->pm->suspend_noirq) {
            ret = drv->pm->suspend_noirq(dev);
        }
    } else {
        ret = himedia_legacy_suspend_late(dev, PMSG_SUSPEND);
    }

    return ret;
}

static int himedia_pm_resume(struct device *dev)
{
    int ret = 0;
    struct device_driver *drv = dev->driver;

    if (drv == NULL) {
        return 0;
    }

    if (drv->pm) {
        if (drv->pm->resume) {
            ret = drv->pm->resume(dev);
        }
    } else {
        ret = himedia_legacy_resume(dev);
    }

    return ret;
}

static int himedia_pm_resume_early(struct device *dev)
{
    int ret = 0;
    struct device_driver *drv = dev->driver;

    if (drv == NULL) {
        return 0;
    }

    if (drv->pm) {
        if (drv->pm->resume_early) {
            ret = drv->pm->resume_early(dev);
        }
    } else {
        ret = himedia_legacy_resume_early(dev);
    }

    return ret;
}

static int himedia_pm_resume_noirq(struct device *dev)
{
    int ret = 0;
    struct device_driver *drv = dev->driver;

    if (drv == NULL) {
        return 0;
    }

    if (drv->pm) {
        if (drv->pm->resume_noirq) {
            ret = drv->pm->resume_noirq(dev);
        }
    } else {
        ret = himedia_legacy_resume_early(dev);
    }

    return ret;
}

#else /* !CONFIG_SUSPEND */

#define himedia_pm_suspend       NULL
#define himedia_pm_resume        NULL
#define himedia_pm_suspend_noirq NULL
#define himedia_pm_resume_noirq  NULL

#endif /* !CONFIG_SUSPEND */

#ifdef CONFIG_HISI_SNAPSHOT_BOOT
static int himedia_pm_freeze(struct device *dev)
{
    int ret = 0;
    struct device_driver *drv = dev->driver;

    if (drv == NULL) {
        return 0;
    }

    if (drv->pm) {
        if (drv->pm->freeze) {
            ret = drv->pm->freeze(dev);
        }
    } else {
        ret = himedia_legacy_suspend(dev, PMSG_FREEZE);
    }

    return ret;
}

static int himedia_pm_freeze_noirq(struct device *dev)
{
    int ret = 0;
    struct device_driver *drv = dev->driver;

    if (drv == NULL) {
        return 0;
    }

    if (drv->pm) {
        if (drv->pm->freeze_noirq) {
            ret = drv->pm->freeze_noirq(dev);
        }
    } else {
        ret = himedia_legacy_suspend_late(dev, PMSG_FREEZE);
    }

    return ret;
}

static int himedia_pm_thaw(struct device *dev)
{
    int ret = 0;
    struct device_driver *drv = dev->driver;

    if (drv == NULL) {
        return 0;
    }

    if (drv->pm) {
        if (drv->pm->thaw) {
            ret = drv->pm->thaw(dev);
        }
    } else {
        ret = himedia_legacy_resume(dev);
    }

    return ret;
}

static int himedia_pm_thaw_noirq(struct device *dev)
{
    int ret = 0;
    struct device_driver *drv = dev->driver;

    if (drv == NULL) {
        return 0;
    }

    if (drv->pm) {
        if (drv->pm->thaw_noirq) {
            ret = drv->pm->thaw_noirq(dev);
        }
    } else {
        ret = himedia_legacy_resume_early(dev);
    }

    return ret;
}

static int himedia_pm_poweroff(struct device *dev)
{
    int ret = 0;
    struct device_driver *drv = dev->driver;

    if (drv == NULL) {
        return 0;
    }

    if (drv->pm) {
        if (drv->pm->poweroff) {
            ret = drv->pm->poweroff(dev);
        }
    } else {
        ret = himedia_legacy_suspend(dev, PMSG_HIBERNATE);
    }

    return ret;
}

static int himedia_pm_poweroff_noirq(struct device *dev)
{
    int ret = 0;
    struct device_driver *drv = dev->driver;

    if (drv == HI_NULL) {
        return 0;
    }

    if (drv->pm) {
        if (drv->pm->poweroff_noirq) {
            ret = drv->pm->poweroff_noirq(dev);
        }
    } else {
        ret = himedia_legacy_suspend_late(dev, PMSG_HIBERNATE);
    }

    return ret;
}

static int himedia_pm_restore(struct device *dev)
{
    int ret = 0;
    struct device_driver *drv = dev->driver;

    if (drv == NULL) {
        return 0;
    }

    if (drv->pm) {
        if (drv->pm->restore) {
            ret = drv->pm->restore(dev);
        }
    } else {
        ret = himedia_legacy_resume(dev);
    }

    return ret;
}

static int himedia_pm_restore_noirq(struct device *dev)
{
    int ret = 0;
    struct device_driver *drv = dev->driver;

    if (drv == HI_NULL) {
        return 0;
    }

    if (drv->pm) {
        if (drv->pm->restore_noirq) {
            ret = drv->pm->restore_noirq(dev);
        }
    } else {
        ret = himedia_legacy_resume_early(dev);
    }

    return ret;
}

#else /* !CONFIG_HIBERNATION */

#define himedia_pm_freeze         NULL
#define himedia_pm_thaw           NULL
#define himedia_pm_poweroff       NULL
#define himedia_pm_restore        NULL
#define himedia_pm_freeze_noirq   NULL
#define himedia_pm_thaw_noirq     NULL
#define himedia_pm_poweroff_noirq NULL
#define himedia_pm_restore_noirq  NULL

#endif /* !CONFIG_HIBERNATION */

static struct dev_pm_ops g_himedia_dev_pm_ops = {
    .prepare = himedia_pm_prepare,
    .complete = himedia_pm_complete,
    .suspend = himedia_pm_suspend,
    .resume = himedia_pm_resume,
    .freeze = himedia_pm_freeze,
    .thaw = himedia_pm_thaw,
    .poweroff = himedia_pm_poweroff,
    .restore = himedia_pm_restore,
    .suspend_noirq = himedia_pm_suspend_noirq,
    .resume_noirq = himedia_pm_resume_noirq,
    .freeze_noirq = himedia_pm_freeze_noirq,
    .thaw_noirq = himedia_pm_thaw_noirq,
    .poweroff_noirq = himedia_pm_poweroff_noirq,
    .restore_noirq = himedia_pm_restore_noirq,
    .resume_early = himedia_pm_resume_early,
};

#define HIMEDIA_PM_OPS_PTR (&g_himedia_dev_pm_ops)

#else /* !CONFIG_PM_SLEEP */

#define HIMEDIA_PM_OPS_PTR NULL

#endif /* !CONFIG_PM_SLEEP */

struct bus_type g_himedia_bus_type = {
    .name = "himedia_bus",
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
    .dev_groups = g_himedia_dev_groups,
#else
    .dev_attrs = g_himedia_dev_attrs,
#endif
    .match = himedia_match,
    .uevent = himedia_uevent,
    .pm = HIMEDIA_PM_OPS_PTR,
};

int himedia_bus_init(void)
{
    int ret;
    ret = device_register(&g_himedia_bus);
    if (ret) {
        return ret;
    }
    ret = bus_register(&g_himedia_bus_type);
    if (ret) {
        device_unregister(&g_himedia_bus);
    }
    return ret;
}

void himedia_bus_exit(void)
{
    bus_unregister(&g_himedia_bus_type);
    device_unregister(&g_himedia_bus);
    return;
}

/*********************************************************************/
/* himedia  base  device */
/*********************************************************************/
int himedia_device_add(pm_basedev *pdev)
{
    // 0
    if (pdev == NULL) {
        return -EINVAL;
    }
    // 1
    if (!pdev->dev.parent) {
        pdev->dev.parent = &g_himedia_bus;
    }
    pdev->dev.bus = &g_himedia_bus_type;

    if (pdev->id != -1) {
        dev_set_name(&pdev->dev, "%s.%d", pdev->name, pdev->id);
    } else {
        dev_set_name(&pdev->dev, pdev->name);
    }

    return device_add(&pdev->dev);
}

void himedia_device_del(pm_basedev *pdev)
{
    if (pdev != NULL) {
        device_del(&pdev->dev);
    }
    return;
}

void himedia_device_put(pm_basedev *pdev)
{
    if (pdev != NULL) {
        put_device(&pdev->dev);
    }
}

struct himedia_devobj {
    pm_basedev pdev;
    char name[1];
};

static void himedia_device_release(struct device *dev)
{
    struct himedia_devobj *pa = container_of(dev, struct himedia_devobj,
                                             pdev.dev);
    kfree(pa);
    return;
}

pm_basedev *himedia_device_alloc(const char *name, int id)
{
    int size, ret;
    struct himedia_devobj *pa;
    size = strlen(name);
    pa = kmalloc(sizeof(struct himedia_devobj) + size, GFP_KERNEL | __GFP_ZERO);
    if (pa != NULL) {
        ret = snprintf_s(pa->name, sizeof(struct himedia_devobj) + size - sizeof(pa->pdev),
            size, "%s", name);
        if (ret != size) {
            printk("call snprintf_s func error.\n");
        }
        pa->pdev.name = pa->name;
        pa->pdev.id = id;
        device_initialize(&pa->pdev.dev);
        pa->pdev.dev.release = himedia_device_release;
    }
    return pa ? &pa->pdev : NULL;
}

int himedia_device_register(pm_basedev *pdev)
{
    device_initialize(&pdev->dev);
    return himedia_device_add(pdev);
}

void himedia_device_unregister(pm_basedev *pdev)
{
    himedia_device_del(pdev);
    himedia_device_put(pdev);
    return;
}

/*********************************************************************/
/* himedia  base  driver */
/*********************************************************************/
static int himedia_drv_probe(struct device *dev)
{
    pm_basedrv *pdrv = to_himedia_basedrv(dev->driver);
    pm_basedev *pdev = TO_PM_BASEDEV(dev);

    return pdrv->probe(pdev);
}

static int himedia_drv_remove(struct device *dev)
{
    pm_basedrv *pdrv = to_himedia_basedrv(dev->driver);
    pm_basedev *pdev = TO_PM_BASEDEV(dev);

    return pdrv->remove(pdev);
}

static void himedia_drv_shutdown(struct device *dev)
{
    pm_basedrv *pdrv = to_himedia_basedrv(dev->driver);
    pm_basedev *pdev = TO_PM_BASEDEV(dev);

    pdrv->shutdown(pdev);
    return;
}

static int himedia_drv_suspend(struct device *dev, pm_message_t state)
{
    pm_basedrv *pdrv = to_himedia_basedrv(dev->driver);
    pm_basedev *pdev = TO_PM_BASEDEV(dev);

    return pdrv->suspend(pdev, state);
}

static int himedia_drv_resume(struct device *dev)
{
    pm_basedrv *pdrv = to_himedia_basedrv(dev->driver);
    pm_basedev *pdev = TO_PM_BASEDEV(dev);

    return pdrv->resume(pdev);
}

int himedia_driver_register(pm_basedrv *drv)
{
    drv->driver.bus = &g_himedia_bus_type;
    if (drv->probe) {
        drv->driver.probe = himedia_drv_probe;
    }
    if (drv->remove) {
        drv->driver.remove = himedia_drv_remove;
    }
    if (drv->shutdown) {
        drv->driver.shutdown = himedia_drv_shutdown;
    }
    if (drv->suspend) {
        drv->driver.suspend = himedia_drv_suspend;
    }
    if (drv->resume) {
        drv->driver.resume = himedia_drv_resume;
    }
    return driver_register(&drv->driver);
}

void himedia_driver_unregister(pm_basedrv *drv)
{
    driver_unregister(&drv->driver);
}

struct himedia_drvobj {
    pm_basedrv pdrv;
    char name[1];
};

pm_basedrv *himedia_driver_alloc(const char *name, struct module *owner,
    pm_baseops *ops)
{
    int size, ret;
    struct himedia_drvobj *pa;
    size = strlen(name);
    pa = kmalloc(sizeof(struct himedia_drvobj) + size, GFP_KERNEL | __GFP_ZERO);
    if (pa != NULL) {
        // 0
        ret = snprintf_s(pa->name, sizeof(struct himedia_devobj) + size - sizeof(pa->pdrv),
            size, "%s", name);
        if (ret != size) {
            printk("call snprintf_s func error.\n");
        }

        pa->pdrv.driver.name = pa->name;
        pa->pdrv.driver.owner = owner;
        // 1
        if (ops != NULL && ops->probe != NULL) {
            pa->pdrv.probe = ops->probe;
        } else {
            pa->pdrv.probe = NULL;
        }

        if (ops != NULL && ops->remove != NULL) {
            pa->pdrv.remove = ops->remove;
        } else {
            pa->pdrv.remove = NULL;
        }

        if (ops != NULL && ops->shutdown != NULL) {
            pa->pdrv.shutdown = ops->shutdown;
        } else {
            pa->pdrv.shutdown = NULL;
        }

        if (ops != NULL && ops->suspend != NULL) {
            pa->pdrv.suspend = ops->suspend;
        } else {
            pa->pdrv.suspend = NULL;
        }

        if (ops != NULL && ops->resume != NULL) {
            pa->pdrv.resume = ops->resume;
        } else {
            pa->pdrv.resume = NULL;
        }

        if (ops != NULL && ops->suspend_late != NULL) {
            pa->pdrv.suspend_late = ops->suspend_late;
        } else {
            pa->pdrv.suspend_late = NULL;
        }

        if (ops != NULL && ops->resume_early != NULL) {
            pa->pdrv.resume_early = ops->resume_early;
        } else {
            pa->pdrv.resume_early = NULL;
        }
    }
    return pa ? &pa->pdrv : NULL;
}

void himedia_driver_release(pm_basedrv *drv)
{
    struct himedia_drvobj *pa = container_of(drv, struct himedia_drvobj, pdrv);
    kfree(pa);
    return;
}




