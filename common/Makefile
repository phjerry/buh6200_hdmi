###############################################################################
# export variables
###############################################################################
ifneq ($(srctree),)
    KERNEL_DIR := $(srctree)

    SDK_DIR := $(shell cd $(KERNEL_DIR)/../../../.. && /bin/pwd)
else
    SDK_DIR := $(shell cd $(CURDIR)/../../../.. && /bin/pwd)
endif

include $(SDK_DIR)/base.mak

###############################################################################
# local variables
###############################################################################

## header search path

KERNEL := $(KERNEL_DIR)/$(CFG_HI_KERNEL_VERSION)

EXTRA_CFLAGS += -I$(DRV_DIR)/include \
                -I$(DRV_DIR)/memory \
                -I$(DRV_DIR)/common/include \
                -I$(DRV_DIR)/common/log \
                -I$(KERNEL)

ifdef CFG_HI_LOG_LEVEL
EXTRA_CFLAGS += -DCFG_HI_LOG_LEVEL=$(CFG_HI_LOG_LEVEL)
else
EXTRA_CFLAGS += -DCFG_HI_LOG_LEVEL=1
endif

ifeq ($(CFG_HI_TEE_SUPPORT),y)
EXTRA_CFLAGS += -DCOMMON_TEE_SUPPORT
ifeq ($(CFG_HI_ADVCA_SUPPORT),y)
    ifeq ($(findstring $(CFG_HI_CHIP_TYPE), hi3798cv200 hi3798mv200 hi3798mv100),)
    EXTRA_CFLAGS += -DADVCA_TYPE
    endif
    ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3798cv200 hi3798mv200),)
        ifeq ($(CFG_HI_ADVCA_TYPE),VERIMATRIX_ULTRA)
        EXTRA_CFLAGS += -DADVCA_TYPE
        endif
    endif
endif
endif

EXTRA_CFLAGS +=  -I$(TEE_DRV_INCLUDE)

ifeq ($(CONFIG_ARCH_HI3796MV2X), y)
EXTRA_CFLAGS += -I$(KERNEL_SRC_DIR)/drivers/hisilicon/sec_mmz
endif

ifeq ($(CFG_HI_LOG_NETWORK_SUPPORT),y)
EXTRA_CFLAGS += -DLOG_NETWORK_SUPPORT
endif

ifeq ($(CFG_HI_LOG_UDISK_SUPPORT),y)
EXTRA_CFLAGS += -DLOG_UDISK_SUPPORT
endif

ifeq ($(CFG_PRODUCT_TYPE),android)
ifeq ($(TARGET_BUILD_VARIANT),user)
CFG_HI_PROC_WRITE_SUPPORT := n
endif
endif

ifeq ($(CFG_HI_PROC_WRITE_SUPPORT),y)
EXTRA_CFLAGS += -DHI_PROC_WRITE_SUPPORT
endif

#SMP SUPPORT
ifeq ($(CFG_HI_TEE_SUPPORT),y)
ifeq ($(CFG_HI_TEE_SMP_SUPPORT),y)
EXTRA_CFLAGS += -DHI_SMP_SUPPORT
endif
endif

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

MOD_NAME := hi_common

$(MOD_NAME)-y :=    drv_common_intf.o \
                    drv_common_ext.o \
                    drv_file.o \
                    drv_module.o \
                    drv_osal.o \
                    drv_reg.o \
                    drv_stat.o \
                    drv_sys.o \
                    himedia_base.o \
                    himedia.o

ifeq ($(CFG_HI_LOG_SUPPORT),y)
$(MOD_NAME)-y +=    drv_log.o
endif

ifeq ($(CFG_HI_PROC_SUPPORT),y)
$(MOD_NAME)-y +=    drv_proc.o \
                    drv_user_proc.o
endif

$(MOD_NAME)-y +=    osal/osal_fence.o \
                    osal/osal_debug.o \
                    osal/osal_atomic.o \
                    osal/osal_semaphore.o \
                    osal/osal_mutex.o \
                    osal/osal_spinlock.o \
                    osal/osal_wait.o \
                    osal/osal_workqueue.o \
                    osal/osal_fence.o \
                    osal/osal_interrupt.o \
                    osal/osal_task.o \
                    osal/osal_addr.o \
                    osal/osal_cache.o \
                    osal/osal_barrier.o \
                    osal/osal_string.o \
                    osal/osal_timer.o \
                    osal/osal_proc.o \
                    osal/osal_fileops.o \
                    osal/osal_device.o \
                    osal/osal_math.o \
                    osal/osal_exportfunc.o \
                    osal/osal_gpio.o
$(MOD_NAME)-y +=    osal/linux/osal_mem.o

###############################################################################
obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o

BUILD_DIR := $(HI_OUT_DIR)/$(OBJ_NAME)/$(subst $(SDK_DIR)/,,$(CURDIR))

BUILD_DIR_MAKEFILE := $(BUILD_DIR)/Makefile

#===============================================================================
#   rules
#===============================================================================
.PHONY: all clean

all: $(BUILD_DIR_MAKEFILE)
	$(AT)make -C $(LINUX_DIR) ARCH=$(CFG_HI_CPU_ARCH) CROSS_COMPILE=$(HI_KERNEL_TOOLCHAINS_NAME)- M=$(BUILD_DIR) src=$(CURDIR) modules
	$(AT)$(HI_KERNEL_TOOLCHAINS_NAME)-strip --strip-unneeded $(BUILD_DIR)/$(MOD_NAME).ko
	$(AT)test -d $(HI_MODULE_DIR) || mkdir -p $(HI_MODULE_DIR)
	$(AT)cp -f $(BUILD_DIR)/$(MOD_NAME).ko $(HI_MODULE_DIR)/

clean:
	$(AT)make -C $(LINUX_DIR) ARCH=$(CFG_HI_CPU_ARCH) CROSS_COMPILE=$(HI_KERNEL_TOOLCHAINS_NAME)- M=$(BUILD_DIR) src=$(CURDIR) clean
	$(AT)rm -rf $(HI_MODULE_DIR)/$(MOD_NAME).ko

$(BUILD_DIR_MAKEFILE): $(BUILD_DIR)
	$(AT)touch $@

$(BUILD_DIR):
	$(AT)mkdir -p $@
