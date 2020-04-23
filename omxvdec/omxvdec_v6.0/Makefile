#===============================================================================
# export variables
#===============================================================================
ifneq ($(srctree),)
KERNEL_DIR := $(srctree)
SDK_DIR := $(shell cd $(KERNEL_DIR)/../../../.. && /bin/pwd)
else
SDK_DIR := $(shell cd $(CURDIR)/../../../../../ && /bin/pwd)
endif

include $(SDK_DIR)/base.mak


#===============================================================================
# local variables
#===============================================================================
CFG_VFMW_V6_SUPPORT := y
EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)
EXTRA_CFLAGS += -g

EXTRA_CFLAGS += -I$(USR_DIR)/include \
                -I$(USR_DIR)/common/include
EXTRA_CFLAGS += -I$(DRV_DIR)/include \
                -I$(DRV_DIR)/common/include

EXTRA_CFLAGS += -I$(DRV_DIR)/omxvdec/include           \
                -I$(DRV_DIR)/drv/vdec

ifeq ($(CFG_VFMW_V6_SUPPORT),y)
EXTRA_CFLAGS += -I$(DRV_DIR)/vfmw/vfmw_v6.2 \
                -I$(DRV_DIR)/vfmw/vfmw_v6.2/include  \
                -I$(DRV_DIR)/vfmw_v6.2/master  \
                -I$(DRV_DIR)/vfmw/vfmw_v6.2/master/module/binary
EXTRA_CFLAGS += -DVFMW_BINARY_SUPPORT
else
EXTRA_CFLAGS += -I$(DRV_DIR)/vfmw/vfmw_v5.1 \
                -I$(DRV_DIR)/vfmw/vfmw_v5.1/include
endif

ifeq ($(CFG_HI_TEE_SUPPORT),y)
EXTRA_CFLAGS += -DHI_OMX_TEE_SUPPORT
EXTRA_CFLAGS += -I$(TEE_DRV_INCLUDE)
endif

ifeq ($(CFG_HI_SMMU_SUPPORT),y)
EXTRA_CFLAGS += -DHI_SMMU_SUPPORT
endif

ifeq ($(findstring $(CFG_HI_CHIP_TYPE), hi3798mv200 hi3798mv300 hi3796mv200),)
EXTRA_CFLAGS += -DSEDONLY_ENABLE=0
else
EXTRA_CFLAGS += -DSEDONLY_ENABLE=1
endif

EXTRA_CFLAGS += -DVFMW_COMPRESS_SUPPORT

ifeq (y,$(CFG_HI_VDEC_PRE_ALLOC_VDHMMZ_SUPPORT))
EXTRA_CFLAGS += -DPRE_ALLOC_VDEC_VDH_MMZ=1
else
EXTRA_CFLAGS += -DPRE_ALLOC_VDEC_VDH_MMZ=0
endif

ifeq ($(CFG_ANDROID_TVOS),y)
EXTRA_CFLAGS += -DHI_TVOS_SUPPORT
endif

ifeq ($(CFG_GSTREAMER_SUPPORT),y)
EXTRA_CFLAGS += -DHI_TVOS_SUPPORT
endif

ifeq ($(CFG_ANDROID_PRODUCT),y)
ifneq ($(CFG_HI_RECOVERY_SUPPORT),y)
EXTRA_CFLAGS += -DHI_DEC_SYNC_SUPPORT
endif
endif

ifneq ($(findstring $(CFG_HI_KERNEL_VERSION), linux-4.14.y),)
EXTRA_CFLAGS += -I$(srctree)/drivers/hisilicon/hi_fence
else
EXTRA_CFLAGS += -I$(srctree)/drivers/staging/android
endif

MOD_NAME := hi_omxvdec

obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o

$(MOD_NAME)-y := omxvdec.o channel.o message.o decoder_vfmw.o omxvdec_mem.o dec_sync.o

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
