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
TARGET := tde
HAL_DIR := hal_v3_0

ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3798mv310 hi3716mv430),)
EXTRA_CFLAGS += -DCONFIG_GFX_TDE_VERSION_3_0
endif

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

EXTRA_CFLAGS += -I$(DRV_DIR)/include                    \
                -I$(DRV_DIR)/common/include             \
                -I$(DRV_DIR)/jpeg/grc_cmm_inc           \
                -I$(DRV_DIR)/tde/$(TARGET)/include      \
                -I$(DRV_DIR)/tde/$(TARGET)              \
                -I$(DRV_DIR)/tde/include                \
                -I$(DRV_DIR)/tde/adp/$(HAL_DIR)         \
                -I$(DRV_DIR)/tde/adp/inc

ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3716mv430),)
EXTRA_CFLAGS += -DHI_TDE_BUFFER=$(CFG_HI_TDE_NODE_BUFFER_SIZE)
else
EXTRA_CFLAGS += -DHI_TDE_BUFFER=0x200000
endif
EXTRA_CFLAGS += -DCFG_HI_TDE_CSCTMPBUFFER_SIZE=0
EXTRA_CFLAGS += -DHI_TDE_MEMCOUNT_SUPPORT=0
EXTRA_CFLAGS += -DCONFIG_GFX_STB_SDK

ifeq ($(CFG_HI_SMMU_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_GFX_MMU_SUPPORT
EXTRA_CFLAGS += -DCONFIG_GFX_MMU_CLOCK
endif

ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3716mv430),)
#EXTRA_CFLAGS += -DCONFIG_GFX_TDE_LOW_POWER_SUPPORT
endif
####################################################
#beg debug
ifeq ($(CFG_HI_PROC_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_GFX_PROC_SUPPORT
endif

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

EXTRA_CFLAGS += -DCONFIG_GFX_DFX_MOD_TDE
ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3716mv430),)
#EXTRA_CFLAGS += -DCONFIG_USE_SDK_LOG
else
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_MINI
endif

ifneq ($(HI_LOG_SUPPORT),y)
#EXTRA_CFLAGS += -DCONFIG_USE_SDK_LOG
endif
#end debug
####################################################

MOD_NAME := hi_$(TARGET)

obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o

$(MOD_NAME)-y   :=  src/tde_osr.o       \
                    src/tde_osr_k.o     \
                    src/tde_handle_k.o  \
                    src/tde_osilist_k.o \
                    src/tde_proc_k.o    \
                    src/wmalloc_k.o     \
                    src/tde_debug.o     \
                    src/drv_tde_intf.o  \
                    adp/$(HAL_DIR)/tde_hal_k.o     \
                    adp/src/tde_osictl_k.o  \
                    adp/src/tde_adp.o

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

