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

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

EXTRA_CFLAGS += -I$(DRV_DIR)/include \
                -I$(DRV_DIR)/common/include \
                -I$(USR_DIR)/include \
                -I$(USR_DIR)/common/include \
                -I$(DRV_DIR)/sync/hal/include

ifeq ($(CFG_HI_PROC_SUPPORT),y)
EXTRA_CFLAGS += -DHI_SYNC_PROC_SUPPORT
endif

MOD_NAME := hi_sync

$(MOD_NAME)-y := drv_sync.o \
                 drv_sync_intf.o \
                 drv_sync_intf_k.o

ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3796cv300),)
ifneq ($(CFG_HI_TEE_SUPPORT),y)
$(MOD_NAME)-y += hal/hi3796cv300/drv_stc_hal.o \
                 drv_sync_stc.o

#EXTRA_CFLAGS += -DSYNC_PCR_ADJUST_STC_SUPPORT
endif
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
