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

TARGET := gfx2d

EXTRA_CFLAGS += -I$(DRV_DIR)/include                    \
                -I$(DRV_DIR)/memory                     \
                -I$(DRV_DIR)/common/include             \
                -I$(DRV_DIR)/common/log                 \
                -I$(DRV_DIR)/jpeg/grc_cmm_inc           \
                -I$(DRV_DIR)/$(TARGET)/inc              \
                -I$(DRV_DIR)/$(TARGET)/hal/inc          \
				-I$(DRV_DIR)/tde/adp/hal_v3_0

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)
EXTRA_CFLAGS += -DCONFIG_GFX_STB_SDK

ifneq ($(findstring $(CFG_HI_KERNEL_VERSION), linux-4.9.y linux-4.14.y),)
EXTRA_CFLAGS += -I$(srctree)/drivers/hisilicon/hi_fence
else
EXTRA_CFLAGS += -I$(srctree)/drivers/staging/android
endif

ifneq ($(findstring $(CONFIG_SYNC), y),)
#EXTRA_CFLAGS += -DGFX2D_FENCE_SUPPORT
#EXTRA_CFLAGS += -DGFX2D_SUBMIT_EVERY_TASK
endif

ifeq ($(CFG_ANDROID_PRODUCT),y)
EXTRA_CFLAGS += -DGFX2D_FENCE_SUPPORT
EXTRA_CFLAGS += -DGFX2D_ASYNC_PTHREAD_SUPPORT
EXTRA_CFLAGS += -DGFX2D_SUBMIT_EVERY_TASK
EXTRA_CFLAGS += -DGFX2D_USE_ION_MEM
endif

ifeq ($(CFG_HI_SMMU_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_GFX_MMU_SUPPORT
endif
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_DEBUG
####################################################
#beg debug
ifeq ($(CFG_HI_PROC_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_GFX_PROC_SUPPORT
endif
EXTRA_CFLAGS += -Wno-unused-function
ifeq ($(CFG_HI_LOG_LEVEL_DEBUG),y)
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_DEBUG
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_INFO
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

ifeq ($(CFG_HI_LOG_LEVEL_INFO),y)
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_INFO
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

ifeq ($(CFG_HI_LOG_LEVEL_WARNING),y)
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

ifeq ($(CFG_HI_LOG_LEVEL_ERROR),y)
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

ifeq ($(CFG_HI_LOG_LEVEL_FATAL),y)
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

EXTRA_CFLAGS += -DCONFIG_GFX_DFX_MOD_GFX2D
#EXTRA_CFLAGS += -DCONFIG_USE_SDK_LOG
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_MINI
#end debug
####################################################
GFX2D_CFLAGS = -DHWC_VERSION=0x500 -DGFX2D_SYNC_TDE
GFX2D_VER_DIR = v2

ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3719mv100 hi3718mv100), )
GFX2D_CFLAGS = -DHWC_VERSION=0x200
GFX2D_VER_DIR = v1
endif

ifeq ($(CFG_HI_CHIP_TYPE), hi3798cv200)
GFX2D_CFLAGS = -DHWC_VERSION=0x400
GFX2D_VER_DIR = v1
endif

ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3798mv200 hi3798mv300 hi3798mv310 hi3796mv200 hi3716mv450), )
GFX2D_CFLAGS = -DHWC_VERSION=0x500 -DGFX2D_SYNC_TDE
GFX2D_VER_DIR = v2
endif

ifneq ($(findstring $(CFG_HI_CHIP_TYPE),hi3796mv200 hi3716mv450), )
#GFX2D_CFLAGS += -DGFX2D_ALPHADETECT_SUPPORT
endif
GFX2D_CFLAGS += -DCONFIG_GFX_HWC_VERSION_3_0
ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3798mv310),)
GFX2D_CFLAGS += -DCONFIG_GFX_HWC_VERSION_3_0
endif

EXTRA_CFLAGS += $(GFX2D_CFLAGS)

MOD_NAME := hi_$(TARGET)

obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o


GFX2D_DIR := $(shell cd $(DRV_DIR)/gfx2d && pwd)

GFX2D_SRCS := $(wildcard $(GFX2D_DIR)/*.c)
GFX2D_SRCS += $(wildcard $(GFX2D_DIR)/src/*.c)
GFX2D_SRCS += $(wildcard $(GFX2D_DIR)/hal/$(GFX2D_VER_DIR)/*.c)

GFX2D_OBJS := $(patsubst $(GFX2D_DIR)/%.c, %.o, $(GFX2D_SRCS))

$(MOD_NAME)-y = $(GFX2D_OBJS)

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
