/*
 * Copyright (C) Hisilicon Technologies Co., Ltd. 2010-2019. All rights reserved.
 * Description: common driver moudle define
 * Author: guoqingbo
 * Create: 2010-1-25
 */

#include <linux/module.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>

#include "hi_type.h"
#include "hi_osal.h"

#include "hi_drv_log.h"
#include "hi_drv_dev.h"
#include "hi_drv_sys.h"
#include "hi_drv_stat.h"
#include "hi_drv_module.h"
#include "hi_reg_common.h"
#include "drv_log.h"
#include "drv_sys.h"
#include "drv_reg.h"
#include "drv_module.h"
#include "drv_stat.h"
#include "drv_proc.h"
#include "drv_user_proc.h"
#include "drv_common_ioctl.h"

static struct platform_device *g_common_virt_dev = HI_NULL;

int osal_proc_init(hi_void);
hi_void osal_proc_exit(hi_void);
hi_s32 drv_pm_mod_init(hi_void);
hi_void drv_pm_mod_exit(hi_void);

hi_s32 hi_drv_sys_get_irq_by_name(hi_char *name)
{
    struct resource *res = HI_NULL;

    if (!g_common_virt_dev) {
        HI_ERR_SYS("module %s is not registered\n", name);
        return -1;
    }

    res = platform_get_resource_byname(g_common_virt_dev, IORESOURCE_IRQ, name);
    if (!res) {
        HI_ERR_SYS("module %s platform_get_resource_byname fail!\n", name);
        return -1;
    }

    return res->start;
}

static hi_s32 drv_common_virt_probe(struct platform_device *dev)
{
    HI_PRINT("virtual device initialization.\n");
    g_common_virt_dev = dev;
    return 0;
}

static const struct of_device_id g_common_virt_dev_match[] __maybe_unused = {
    { .compatible = "virt-device", },
    {},
};

MODULE_DEVICE_TABLE(of, g_common_virt_dev_match);

struct platform_driver g_common_virt_platform_drv = {
    .probe  = drv_common_virt_probe,
    .driver = {
        .name           = "virt-device",
        .owner          = THIS_MODULE,
        .of_match_table = of_match_ptr(g_common_virt_dev_match),
    },
};

hi_s32 hi_drv_common_init(hi_void)
{
    hi_s32 ret;

    ret = platform_driver_register(&g_common_virt_platform_drv);
    if (ret != 0) {
        HI_ERR_SYS("register driver failed\n");
        return HI_FAILURE;
    }

    ret = drv_pm_mod_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("drv_pm_mod_init failed:%#x!\n", ret);
        goto err_exit_pm;
    }

    ret = drv_sys_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("sys init failed\n");
        goto err_exit_sys;
    }

    ret = drv_reg_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("reg init failed\n");
        goto err_exit_reg;
    }

    ret = drv_log_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("log init failed\n");
        goto err_exit_log;
    }

    ret = drv_module_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("module init failed\n");
        goto err_exit_module;
    }

    ret = drv_proc_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("proc init failed\n");
        goto err_exit_proc;
    }

    ret = osal_proc_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("osal_proc_init failed:%#x!\n", ret);
        goto err_exit_osal_proc;
    }

    ret = drv_user_proc_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("userproc init failed\n");
        goto err_exit_user_proc;
    }

    return HI_SUCCESS;

err_exit_user_proc:
    osal_proc_exit();

err_exit_osal_proc:
    drv_proc_exit();

err_exit_proc:
    drv_module_exit();

err_exit_module:
    drv_log_exit();

err_exit_log:
    drv_reg_exit();

err_exit_reg:
    drv_sys_exit();

err_exit_sys:
    drv_pm_mod_exit();

err_exit_pm:
    platform_driver_unregister(&g_common_virt_platform_drv);

    return ret;
}

hi_void hi_drv_common_exit(hi_void)
{
    drv_proc_exit();

    osal_proc_exit();

    drv_module_exit();

    drv_log_exit();

    drv_reg_exit();

    drv_pm_mod_exit();

    platform_driver_unregister(&g_common_virt_platform_drv);

    return;
}

static hi_s32 common_open(hi_void *private_data)
{
#ifdef HI_PROC_SUPPORT
    return drv_user_proc_open(private_data);
#else
    return HI_SUCCESS;
#endif
}

static hi_s32 common_close(hi_void *private_data)
{
#ifdef HI_PROC_SUPPORT
    return drv_user_proc_close(private_data);
#else
    return HI_SUCCESS;
#endif
}

/*
 * map range 0xF8000000~0xF8000FFF(sys can map one page only)
 * map range 0xF8A20000~0xF8A20FFF(per can map one page only)
 */
/* it depends on how many pages can be maped, 0: one page, 1: two pages, 2: four pages, 3: eight pages */
#define SYS_PAGE_NUM_SHIFT 0
#define SYS_MAP_PAGE_NUM   (1ULL << SYS_PAGE_NUM_SHIFT)
#define SYS_MAP_MASK       (((SYS_MAP_PAGE_NUM) << PAGE_SHIFT) - 1)

static hi_s32 drv_sys_valid_mmap_phy_addr_range(hi_ulong fn, size_t size, hi_ulong reg_phy_mask)
{
    return (fn + (size >> PAGE_SHIFT)) <= (1 + (reg_phy_mask >> PAGE_SHIFT));
}

static hi_s32 common_mmap(osal_vm *vm, unsigned long start, unsigned long end, unsigned long vm_pgoff,
                          hi_void *private_data)
{
    size_t size = end - start;

    /* Invalid register address or size of sys&perl */
    if (!((((vm_pgoff >> SYS_PAGE_NUM_SHIFT) == ((HI_REG_SYS_BASE_ADDR >> PAGE_SHIFT) >> SYS_PAGE_NUM_SHIFT)) &&
         drv_sys_valid_mmap_phy_addr_range(vm_pgoff, size, HI_REG_SYS_BASE_ADDR | SYS_MAP_MASK)) ||
         (((vm_pgoff >> SYS_PAGE_NUM_SHIFT) == ((HI_REG_PERI_BASE_ADDR >> PAGE_SHIFT) >> SYS_PAGE_NUM_SHIFT)) &&
         drv_sys_valid_mmap_phy_addr_range(vm_pgoff, size, HI_REG_PERI_BASE_ADDR | SYS_MAP_MASK)))) {
        HI_ERR_SYS("invalid vm_pgoff[%#x] or size[%#x]\n", vm_pgoff, size);
        return -EINVAL;
    }

    ((struct vm_area_struct *)vm->vm)->vm_page_prot = phys_mem_access_prot(private_data, vm_pgoff, size,
        ((struct vm_area_struct *)vm->vm)->vm_page_prot);

    if (osal_remap_pfn_range(vm, start, vm_pgoff, size, 0)) {
        HI_FATAL_SYS("remap_pfn_range fail.\n");
        return -EAGAIN;
    }

    return HI_SUCCESS;
}

static osal_ioctl_cmd g_common_ioctl_info[] = {
    {SYS_GET_VERSION,                  drv_sys_ioctl},
    {SYS_GET_TIME_STAMP_MS,            drv_sys_ioctl},
    {SYS_GET_DOLBY_SUPPORT,            drv_sys_ioctl},
    {SYS_GET_DTS_SUPPORT,              drv_sys_ioctl},
    {SYS_GET_ADVCA_SUPPORT,            drv_sys_ioctl},
    {SYS_GET_MACRO_VISION_SUPPORT,     drv_sys_ioctl},
    {SYS_GET_HDR10_SUPPORT,            drv_sys_ioctl},
    {SYS_GET_DOLBY_VISION_SUPPORT,     drv_sys_ioctl},
    {SYS_GET_DDR_CFG,                  drv_sys_ioctl},
    {SYS_GET_DIE_ID_64,                drv_sys_ioctl},
    {SYS_GET_DIE_ID_32,                drv_sys_ioctl},
    {SYS_GET_CHIP_PACKAGE_TYPE,        drv_sys_ioctl},
    {SYS_GET_SMP_SUPPORT,              drv_sys_ioctl},
    {SYS_DISABLE_VIDEO_CODEC,          drv_sys_ioctl},
    {SYS_WRITE_REGISTER,               drv_sys_ioctl},
    {SYS_READ_REGISTER,                drv_sys_ioctl},
    {SYS_SET_POLICY_INFO,              drv_sys_ioctl},
    {SYS_GET_POLICY_INFO,              drv_sys_ioctl},
    {STAT_CMPI_EVENT,                  drv_stat_ioctl},
    {STAT_CMPI_LD_EVENT,               drv_stat_ioctl},
    {MODULE_ADD_INFO,                  drv_module_ioctl},
    {MODULE_DEL_INFO,                  drv_module_ioctl},
    {LOG_CMPI_INIT,                    drv_log_ioctl},
    {LOG_CMPI_EXIT,                    drv_log_ioctl},
    {LOG_CMPI_READ,                    drv_log_ioctl},
    {LOG_CMPI_WRITE,                   drv_log_ioctl},
    {LOG_CMPI_SET_PATH,                drv_log_ioctl},
    {LOG_CMPI_SET_STORE_PATH,          drv_log_ioctl},
    {LOG_CMPI_SET_SIZE,                drv_log_ioctl},
#ifdef CONFIG_COMPAT
    {LOG_CMPI_COMPAT_READ,             drv_log_compat_ioctl},
    {LOG_CMPI_COMPAT_WRITE,            drv_log_compat_ioctl},
    {LOG_CMPI_COMPAT_SET_PATH,         drv_log_compat_ioctl},
    {LOG_CMPI_COMPAT_SET_STORE_PATH,   drv_log_compat_ioctl},
#endif
#ifdef HI_PROC_SUPPORT
    {USER_PROC_ADD_ENTRY,              drv_user_proc_ioctl},
    {USER_PROC_REMOVE_ENTRY,           drv_user_proc_ioctl},
    {USER_PROC_ADD_DIR,                drv_user_proc_ioctl},
    {USER_PROC_REMOVE_DIR,             drv_user_proc_ioctl},
    {USER_PROC_GET_CMD,                drv_user_proc_ioctl},
    {USER_PROC_WAKE_READ_TASK,         drv_user_proc_ioctl},
    {USER_PROC_WAKE_WRITE_TASK,        drv_user_proc_ioctl},
#ifdef CONFIG_COMPAT
    {USER_PROC_COMPAT_WAKE_WRITE_TASK, drv_user_proc_compat_ioctl},
    {USER_PROC_ADD_COMPAT_ENTRY,       drv_user_proc_compat_ioctl},
    {USER_PROC_REMOVE_COMPAT_ENTRY,    drv_user_proc_compat_ioctl},
    {USER_PROC_COMPAT_GET_CMD,         drv_user_proc_compat_ioctl},
    {USER_PROC_COMPAT_WAKE_READ_TASK,  drv_user_proc_compat_ioctl},
#endif
#endif
};

static osal_fileops g_common_fops = {
    .open     = common_open,
    .release  = common_close,
    .mmap     = common_mmap,
    .cmd_list = g_common_ioctl_info,
    .cmd_cnt  = sizeof(g_common_ioctl_info) / sizeof(osal_ioctl_cmd),
};

static osal_dev g_common_dev = {
    .name  = HI_DEV_SYS_NAME,
    .minor = HI_DEV_SYS_MINOR,
    .fops = &g_common_fops,
};

static hi_s32 drv_common_dev_register(hi_void)
{
    hi_s32 ret;

    ret = osal_exportfunc_register(HI_ID_SYS, "HI_SYS", HI_NULL);
    if (ret != HI_SUCCESS) {
        HI_FATAL_SYS("register module failed\n");
        return HI_FAILURE;
    }

    ret = osal_dev_register(&g_common_dev);
    if (ret != 0) {
        HI_FATAL_SYS("register device failed\n");
        osal_exportfunc_unregister(HI_ID_SYS);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void drv_common_dev_unregister(hi_void)
{
    osal_dev_unregister(&g_common_dev);

    osal_exportfunc_unregister(HI_ID_SYS);
}

hi_s32 drv_common_module_init(hi_void)
{
    hi_s32 ret;

    ret = hi_drv_common_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("common init failed\n");
        return -1;
    }

    ret = drv_common_dev_register();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("dev register failed\n");
        goto err_exit_dev;
    }

    ret = drv_sys_add_proc();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("sys proc failed\n");
        goto err_exit_sys;
    }

    ret = drv_log_add_proc();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("log proc failed\n");
        goto err_exit_log;
    }

    ret = drv_stat_add_proc();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("stat init failed\n");
        goto err_exit_stat;
    }

    ret = drv_module_add_proc();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("module proc failed\n");
        goto err_exit_proc;
    }

    ret = hi_drv_tee_common_init();
    if (ret != HI_SUCCESS) {
        HI_ERR_SYS("tee common failed\n");
        goto err_exit_tee_common;
    }

#ifdef MODULE
    HI_PRINT("Load hi_common.ko success.\t(%s)\n", VERSION_STRING);
#endif

    return HI_SUCCESS;

err_exit_tee_common:
    drv_module_remove_proc();

err_exit_proc:
    drv_stat_remove_proc();

err_exit_stat:
    drv_log_remove_proc();

err_exit_log:
    drv_sys_remove_proc();

err_exit_sys:
    drv_common_dev_unregister();

err_exit_dev:
    hi_drv_common_exit();

    return HI_FAILURE;
}

hi_void drv_common_module_exit(hi_void)
{
    drv_user_proc_exit();

    drv_module_remove_proc();

    drv_stat_remove_proc();

    hi_drv_tee_common_exit();

    drv_log_remove_proc();

    drv_sys_remove_proc();

    drv_common_dev_unregister();

    hi_drv_common_exit();

    HI_PRINT("remove hi_common.ko success.\n");

    return;
}

#ifdef MODULE
module_init(drv_common_module_init);
module_exit(drv_common_module_exit);
#endif

MODULE_AUTHOR("HISILION");
MODULE_LICENSE("GPL");

