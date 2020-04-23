#===============================================================================
# export variables
#===============================================================================
ifeq ($(CFG_HI_EXPORT_FLAG),)
    ifneq ($(srctree),)
    KERNEL_DIR := $(srctree)

    SDK_DIR := $(shell cd $(KERNEL_DIR)/../../../.. && /bin/pwd)
    else
    SDK_DIR := $(shell cd $(CURDIR)/../../../.. && /bin/pwd)
    endif

    include $(SDK_DIR)/base.mak
endif

#===============================================================================
# local variables
#===============================================================================
EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)
EXTRA_CFLAGS += -I$(DRV_DIR)/common/include \
                -I$(DRV_DIR)/include \
                -I$(DRV_DIR)/dvfs

MOD_NAME := hi_dvfs

$(MOD_NAME)-y := drv_dvfs_intf.o \
                 drv_dvfs.o \
                 drv_dvfs_regulator.o \
                 drv_dvfs_proc.o \
                 drv_dvfs_update.o

ifeq ($(CFG_HI_CPU_DVFS_SUPPORT), y)
    EXTRA_CFLAGS += -DHI_CPU_DVFS_SUPPORT
endif

ifeq ($(CFG_HI_TEMP_CTRL_SUPPORT), y)
    EXTRA_CFLAGS += -DHI_TEMP_CTRL_SUPPORT
    EXTRA_CFLAGS += -DHI_ENTER_LOW_TEMP_COMP_THRESHOLD=$(CFG_HI_ENTER_LOW_TEMP_COMP_THRESHOLD)
    EXTRA_CFLAGS += -DHI_QUIT_LOW_TEMP_COMP_THRESHOLD=$(CFG_HI_QUIT_LOW_TEMP_COMP_THRESHOLD)
    EXTRA_CFLAGS += -DHI_ENTER_LIMITING_FREQ_THRESHOLD=$(CFG_HI_ENTER_LIMITING_FREQ_THRESHOLD)
    EXTRA_CFLAGS += -DHI_QUIT_LIMITING_FREQ_THRESHOLD=$(CFG_HI_QUIT_LIMITING_FREQ_THRESHOLD)
    EXTRA_CFLAGS += -DHI_ENTER_STANDBY_THRESHOLD=$(CFG_HI_ENTER_STANDBY_THRESHOLD)
    $(MOD_NAME)-y += drv_dvfs_temperature.o
endif

obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o

BUILD_DIR := $(HI_OUT_DIR)/$(OBJ_NAME)/$(subst $(SDK_DIR)/,,$(CURDIR))
BUILD_DIR_MAKEFILE := $(BUILD_DIR)/Makefile

#===============================================================================
#   rules
#===============================================================================
.PHONY: all clean

all: $(BUILD_DIR_MAKEFILE)
	$(AT)make -C $(LINUX_DIR) ARCH=$(CFG_HI_CPU_ARCH) CROSS_COMPILE=$(HI_KERNEL_TOOLCHAINS_NAME)- M=$(BUILD_DIR) src=$(CURDIR) modules
	$(AT)test -d $(HI_MODULE_DIR) || mkdir -p $(HI_MODULE_DIR)
	$(AT)cp -f $(BUILD_DIR)/$(MOD_NAME).ko $(HI_MODULE_DIR)/

clean:
	$(AT)make -C $(LINUX_DIR) ARCH=$(CFG_HI_CPU_ARCH) CROSS_COMPILE=$(HI_KERNEL_TOOLCHAINS_NAME)- M=$(BUILD_DIR) src=$(CURDIR) clean
	$(AT)rm -rf $(HI_MODULE_DIR)/$(MOD_NAME).ko

$(BUILD_DIR_MAKEFILE): $(BUILD_DIR)
	$(AT)touch $@

$(BUILD_DIR):
	$(AT)mkdir -p $@
