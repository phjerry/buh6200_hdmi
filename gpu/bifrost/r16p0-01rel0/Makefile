ifeq ($(CFG_HI_EXPORT_FLAG),)
    ifneq ($(srctree),)
    KERNEL_DIR := $(srctree)

    SDK_DIR := $(shell cd $(KERNEL_DIR)/../../../.. && /bin/pwd)
    else
    SDK_DIR := $(shell cd $(CURDIR)/../../../../../../ && /bin/pwd)
    endif

    include $(SDK_DIR)/base.mak
endif

$(warning "captain: top srctree = $(srctree), CURDIR = $(CURDIR)")

export CUR_DIR=$(shell pwd)
export ARCH=$(CFG_HI_CPU_ARCH)
export KDIR=$(LINUX_DIR)

ifeq ($(ARCH), arm64)
export CROSS_COMPILE=$(CFG_HI_AARCH64_TOOLCHAINS_NAME)-
else
export CROSS_COMPILE=$(CFG_HI_ARM_TOOLCHAINS_NAME)-
endif

all:
	#make -C $(CUR_DIR)/drivers/base/dma_buf_test_exporter
	make -C $(CUR_DIR)/drivers/gpu/arm/midgard
	@cp -f $(CUR_DIR)/drivers/gpu/arm/midgard/mali_kbase.ko $(HI_MODULE_DIR)/

clean:
	#make -C $(CUR_DIR)/drivers/base/dma_buf_test_exporter/ clean
	make -C $(CUR_DIR)/drivers/gpu/arm/midgard/ clean
	@rm -f $(HI_MODULE_DIR)/mali_kbase.ko

