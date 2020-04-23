#===============================================================================
# export variables
#===============================================================================
ifneq ($(srctree),)
    KERNEL_DIR := $(srctree)

    SDK_DIR := $(shell cd $(KERNEL_DIR)/../../../.. && /bin/pwd)
else
    SDK_DIR := $(shell cd $(CURDIR)/../../../.. && /bin/pwd)
endif

include $(SDK_DIR)/base.mak


#===============================================================================
# module name
#===============================================================================
TARGET := jpeg

#===============================================================================
# local variables
#===============================================================================
EXTRA_CFLAGS += -I$(DRV_DIR)/common/include             \
                -I$(USR_DIR)/include                    \
                -I$(DRV_DIR)/include                    \
                -I$(DRV_DIR)/$(TARGET)/include

EXTRA_CFLAGS += -I$(DRV_DIR)/$(TARGET)/grc_cmm_inc

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

#=====================================================================================
#the platform version
#=====================================================================================
EXTRA_CFLAGS += -DCONFIG_GFX_STB_SDK
#EXTRA_CFLAGS += -DCONFIG_GFX_BVT_SDK
#EXTRA_CFLAGS += -DCONFIG_GFX_TV_SDK

#=====================================================================================
#ddr and stride is 256bytes align
#=====================================================================================
ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3796cv100 hi3798cv100),)
EXTRA_CFLAGS += -DCONFIG_GFX_256BYTE_ALIGN
endif

ifeq ($(CFG_HI_SMMU_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_GFX_MMU_SUPPORT
EXTRA_CFLAGS += -DCONFIG_GFX_MMU_CLOCK
endif

ifeq ($(CFG_HI_PROC_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_GFX_PROC_SUPPORT
endif

ifeq ($(CFG_HI_LOG_LEVEL_DEBUG),y)
#EXTRA_CFLAGS += -DCONFIG_GFX_DFX_DEBUG
#EXTRA_CFLAGS += -DCONFIG_GFX_DFX_INFO
#EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

ifeq ($(CFG_HI_LOG_LEVEL_INFO),y)
#EXTRA_CFLAGS += -DCONFIG_GFX_DFX_INFO
#EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

ifeq ($(CFG_HI_LOG_LEVEL_WARNING),y)
#EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

ifeq ($(CFG_HI_LOG_LEVEL_ERROR),y)
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

ifeq ($(CFG_HI_LOG_LEVEL_FATAL),y)
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

EXTRA_CFLAGS += -DCONFIG_GFX_DFX_MOD_JPEG
ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3716mv430),)
EXTRA_CFLAGS += -DCONFIG_USE_SDK_LOG
else
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_MINI
endif
#=====================================================================================
#the chip env version
#=====================================================================================

#================================================================================
#select the complie file
#================================================================================
MOD_NAME := hi_$(TARGET)

obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o

$(MOD_NAME)-y :=    src/drv_jpeg_hal.o          \
		    src/drv_jpeg_mutex.o        \
                    src/drv_jpeg_intf.o         \
		    src/drv_jpeg_dev.o          \
		    src/drv_jpeg_csc.o          \
		    src/drv_jpeg_mem.o          \
		    src/drv_jpeg_ioctl.o

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

