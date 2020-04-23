#===============================================================================
# export variables
#===============================================================================
#ifeq ($(CFG_HI_EXPORT_FLAG),)
#    ifneq ($(KERNELRELEASE),)
#    KERNEL_DIR := $(srctree)
#
#    SDK_DIR := $(KERNEL_DIR)/../../..
#    else
#    SDK_DIR := $(CURDIR)/../../../..
#    endif

#include $(SDK_DIR)/base.mak
#endif

KBUILD_EXTRA_SYMBOLS += $(COMMON_DIR)/drv/Module.symvers

export KBUILD_EXTRA_SYMBOLS

TARGET := custom


#===============================================================================
# local variables
#===============================================================================
EXTRA_CFLAGS += -I$(COMMON_UNF_INCLUDE)                 \
                -I$(COMMON_DRV_INCLUDE)                 \
                -I$(MSP_UNF_INCLUDE)                    \
                -I$(MSP_DRV_INCLUDE)                    \
                -I./src/$(CFG_HI_BOARD_NAME)

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

MOD_NAME := hi_$(TARGET)

obj-$(CFG_MSP_BUILDTYPE) += $(MOD_NAME).o

#$(MOD_NAME)-y := drv_custom_intf.o

#===============================================================================
#   rules
#===============================================================================
.PHONY: all clean install uninstall

all:
#	$(AT)-rm -rf ./src
#	$(AT)ln -sf $(SDK_DIR)/source/custom ./src
#	$(AT)make -C $(LINUX_DIR) ARCH=$(CFG_HI_CPU_ARCH) CROSS_COMPILE=$(CFG_HI_TOOLCHAINS_NAME)- M=$(CURDIR) modules

clean:
#	$(AT)make -C $(LINUX_DIR) M=$(CURDIR) clean
	$(AT)rm -rf  *.o
	$(AT)rm -rf  .*.o.cmd
	$(AT)rm -rf .tmp_versions
	$(AT)rm -rf  Module.symvers
	$(AT)rm -rf  modules.order
#	$(AT)rm -rf ./src

install: all
ifneq ($(CFG_MSP_BUILDTYPE),y)
	$(AT)cp -f $(CURDIR)/$(MOD_NAME).ko $(MODULE_DIR)/
endif

uninstall:
	$(AT)rm -rf $(MODULE_DIR)/$(MOD_NAME).ko

