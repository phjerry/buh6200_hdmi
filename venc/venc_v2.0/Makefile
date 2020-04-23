#===============================================================================
# export variables
#===============================================================================
ifeq ($(CFG_HI_EXPORT_FLAG),)
    ifneq ($(srctree),)
    KERNEL_DIR := $(srctree)

    SDK_DIR := $(shell cd $(KERNEL_DIR)/../../../.. && /bin/pwd)
    else
    SDK_DIR := $(shell cd $(CURDIR)/../../../../.. && /bin/pwd)
    endif

    include $(SDK_DIR)/base.mak
endif

#include $(SDK_DIR)/asm.mk
VENC_DIR := $(DRV_DIR)/venc/venc_v2.0
#===============================================================================
# local variables
#===============================================================================
EXTRA_CFLAGS += $(filter-out -Wformat-security,$(CFG_HI_KMOD_CFLAGS))

ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3796cv300, hi3751v900),)
include $(VENC_DIR)/firmware/product/hi3796cv300/32bit/venc_make.cfg
CHIP := 3796cv300
endif

EXTRA_CFLAGS += $(VENC_CFLAGS)

EXTRA_CFLAGS += -I$(DRV_DIR)/include \
                -I$(DRV_DIR)/common/include

MOD_NAME := hi_venc

obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o

$(MOD_NAME)-y := $(venc_obj_list)

#=========================================ASM===================================#
# local include files and macroes, ONLY FOR TEST, PLEASE CHECK CHECK CHECK!!! for 64bit
ASM_CFLAGS += -I/opt/hisi-linux/x86-arm/aarch64-histbv100-linux/target/usr/include
ASM_CFLAGS += -I/opt/hisi-linux/x86-arm/aarch64-histbv100-linux/lib/gcc/aarch64-gcc51_glibc222-linux-gnu/5.1.1/include

ASM_CFLAGS += -I$(SDK_DIR)/tools/linux/toolchains/aarch64-histbv100-linux/target/usr/include
ASM_CFLAGS += -I$(SDK_DIR)/tools/linux/toolchains/aarch64-histbv100-linux/lib/gcc/aarch64-gcc51_glibc222-linux-gnu/5.1.1/include

# local include files and macroes, ONLY FOR TEST, PLEASE CHECK CHECK CHECK!!! for 32bit
ASM_CFLAGS += -I/opt/hisi-linux/x86-arm/arm-histbv310-linux/target/usr/include
ASM_CFLAGS += -I/opt/hisi-linux/x86-arm/arm-histbv310-linux/lib/gcc/arm-gcc49-linux-gnueabi/4.9.2/include

ASM_CFLAGS += -I$(SDK_DIR)/source/common/drv/include
ASM_CFLAGS += -I$(SDK_DIR)/source/common/include
ASM_CFLAGS += $(EXTRA_CFLAGS)

ASM_SRC = $(VENC_DIR)/firmware/core/algorithm/drv_venc_ratecontrol.S
ASM_CHIP_SRC =  $(VENC_DIR)/firmware/core/driver/$(VEDU_VERSION)/drv_venc_hal.S
ifeq ($(CFG_CHIP_COMPATIBLE_VENC),y)
ASM_CHIP_SRC_COMPATIBLE =  $(VENC_DIR)/firmware/core/driver/$(COMPATIBLE_VEDU_VERSION)/drv_venc_hal.S
endif
asflags-y := -I$(COMMON_DRV_INCLUDE)
ifeq ($(findstring -DHI_ADVCA_FUNCTION_RELEASE, $(CFG_HI_CFLAGS)), -DHI_ADVCA_FUNCTION_RELEASE)
asflags-y += -DCONFIG_REMOVE_STRING
endif

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

#===============================================================================
#   generate asm
#===============================================================================
asm:
	make asm_xbit SYSTEM_CONFIG=64bit
	make asm_xbit SYSTEM_CONFIG=32bit

asm_xbit: $(ASM_SRC) $(ASM_CHIP_SRC) $(ASM_CHIP_SRC_COMPATIBLE)
	mkdir -p $(SYSTEM_CONFIG)/$(CHIP)/
ifeq ($(CFG_CHIP_COMPATIBLE_VENC),y)
	mkdir -p $(SYSTEM_CONFIG)/$(CHIP_COMPATIBLE)/
endif
	mv $(ASM_SRC) $(SYSTEM_CONFIG)/
	mv $(ASM_CHIP_SRC) $(SYSTEM_CONFIG)/$(CHIP)/
ifeq ($(CFG_CHIP_COMPATIBLE_VENC),y)
	mv $(ASM_CHIP_SRC_COMPATIBLE) $(SYSTEM_CONFIG)/$(CHIP_COMPATIBLE)/
endif
asm_clean:
	make asm_clean_xbit SYSTEM_CONFIG=64bit
	make asm_clean_xbit SYSTEM_CONFIG=32bit

asm_clean_xbit:
	rm -rf $(SYSTEM_CONFIG)
