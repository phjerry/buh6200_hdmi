#===============================================================================
# For Hi3751V900...
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

TARGET := vi

#===============================================================================
# local variables
#===============================================================================
EXTRA_CFLAGS += -I$(USR_DIR)/include        \
                -I$(USR_DIR)/common/include \
                -I$(DRV_DIR)/include        \
                -I$(DRV_DIR)/common/include

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

EXTRA_CFLAGS += -DVI_USE_PQ
EXTRA_CFLAGS += -DLOG_D_MODULE_ID=HI_ID_VI

MOD_NAME := hi_$(TARGET)

obj-$(CFG_MSP_BUILDTYPE) += $(MOD_NAME).o

$(MOD_NAME)-y   :=  drv_vi_ext.o
$(MOD_NAME)-y   +=  drv_vi_intf.o
$(MOD_NAME)-y   +=  drv_vi_ctrl.o
$(MOD_NAME)-y   +=  drv_vi_proc.o

$(MOD_NAME)-y   +=  vi_comm.o
$(MOD_NAME)-y   +=  vi_utils.o
$(MOD_NAME)-y   +=  vi_dsc.o
$(MOD_NAME)-y   +=  vi_buf.o
$(MOD_NAME)-y   +=  vi_uf.o
$(MOD_NAME)-y   +=  vi_csc.o
$(MOD_NAME)-y   +=  vi_hdr.o
$(MOD_NAME)-y   +=  vi_print.o

$(MOD_NAME)-y   +=  hal/hal_vi_reg_cfg.o
$(MOD_NAME)-y   +=  hal/hal_vi_smmu_reg_cfg.o
$(MOD_NAME)-y   +=  hal/hal_vi_hwbuff.o
$(MOD_NAME)-y   +=  hal/hal_vi.o
$(MOD_NAME)-y   +=  hal/hal_vicap.o

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
