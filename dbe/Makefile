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

##########################################################################################
#	prepare param
##########################################################################################
export CUR_DIR=$(shell pwd)
export ARCH=$(CFG_HI_CPU_ARCH)
export KDIR=$(LINUX_DIR)
export CROSS_COMPILE=$(HI_KERNEL_TOOLCHAINS_NAME)-

EXTRA_CFLAGS += -I$(DRV_DIR)/include

##########################################################################################
#	set task
##########################################################################################
all:
	$(MAKE) ARCH=$(ARCH) -C $(KDIR) M=$(CURDIR) EXTRA_CFLAGS=$(EXTRA_CFLAGS) CONFIG_HI_DMA_BUFFER_EXPORTER=$(HI_DRV_BUILDTYPE)
	@cp -f $(CUR_DIR)/hi_dbe.ko $(HI_MODULE_DIR)/
clean:
	$(MAKE) ARCH=$(ARCH) -C $(KDIR) M=$(CURDIR) clean
	@rm -f $(HI_MODULE_DIR)/hi_dbe.ko

