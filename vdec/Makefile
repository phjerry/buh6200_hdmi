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

VFMW_DIR := vfmw/vfmw_v6.2

EXTRA_CFLAGS += -I$(DRV_DIR)/include \
                -I$(DRV_DIR)/common/include \
		-I$(DRV_DIR)/vdec/vdec_v6.0 \
		-I$(DRV_DIR)/vdec/vdec_v6.0/include \
		-I$(DRV_DIR)/vdec/vdec_v6.0/modules/proc \
		-I$(DRV_DIR)/vdec/vdec_v6.0/platform/osal/linux \
		-I$(DRV_DIR)/vdec/vdec_v6.0/platform/sdk/stb \
		-I$(DRV_DIR)/vdec/vdec_v6.0/vfmw_adpt/vfmw_v6.2 \
		-I$(DRV_DIR)/$(VFMW_DIR)/

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

MOD_NAME := hi_vdec

$(MOD_NAME)-y   :=  vdec_v6.0/drv_vdec_dev.o        \
                    vdec_v6.0/drv_vdec_intf.o             \
                    vdec_v6.0/drv_vdec.o        \
                    vdec_v6.0/modules/proc/drv_vdec_proc.o       \
                    vdec_v6.0/platform/osal/linux/drv_vdec_osal.o \
		    vdec_v6.0/platform/sdk/stb/drv_vdec_sdk.o \
		    vdec_v6.0/vfmw_adpt/vfmw_v6.2/drv_vdec_vfmw.o \
		    vdec_v6.0/vfmw_adpt/vfmw_v6.2/drv_vdec_convert.o

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
