#===============================================================================
# export variables
#===============================================================================
$(warning "---asm.mak begin----"$(CFG_HI_EXPORT_FLAG))
ifeq ($(CFG_HI_EXPORT_FLAG),)
    ifneq ($(srctree),)
    KERNEL_DIR := $(srctree)

    SDK_DIR := $(shell cd $(KERNEL_DIR)/../../.. && /bin/pwd)
    else
    SDK_DIR := $(shell cd $(CURDIR)/../../../../../../.. && /bin/pwd)
    endif

    include $(SDK_DIR)/base.mak
endif
#===============================================================================
#   Generate asm
#===============================================================================
include $(SDK_DIR)/asm.mk

PRODUCT_DIR_V6 := hi3796cv300
VFMW_DIR := $(DRV_DIR)/vfmw/vfmw_v6.2
include $(VFMW_DIR)/product/$(PRODUCT_DIR_V6)/linux_cfg/vfmw_make.cfg
DST_DIR =$(VFMW_DIR)/product/$(PRODUCT_DIR_V6)/common/${SYSTEM_CONFIG}
VFMW_BASE := ../..

################################################################################################
#####    PRODUCT

# local include files and macroes, ONLY FOR TEST, PLEASE CHECK CHECK CHECK!!!
ifeq ($(CFG_ARM_HI100_LINUX),y)
ASM_CFLAGS += -I/opt/hisi-linux/x86-arm/aarch64-hi100-linux/target/usr/include
ASM_CFLAGS += -I/opt/hisi-linux/x86-arm/aarch64-hi100-linux/lib/gcc/aarch64-gcc74_glibc230-linux-gnu/7.4.1/include
ASM_CFLAGS += -I$(SDK_DIR)/tools/linux/toolchains/aarch64-hi100-linux/target/usr/include
ASM_CFLAGS += -I$(SDK_DIR)/tools/linux/toolchains/aarch64-hi100-linux/lib/gcc/aarch64-gcc74_glibc230-linux-gnu/7.4.1/include
else
ASM_CFLAGS += -I/opt/hisi-linux/x86-arm/aarch64-histbv400-linux/target/usr/include
ASM_CFLAGS += -I/opt/hisi-linux/x86-arm/aarch64-histbv400-linux/lib/gcc/aarch64-gcc51_glibc222-linux-gnu/5.1.1/include
ASM_CFLAGS += -I$(SDK_DIR)/tools/linux/toolchains/aarch64-histbv400-linux/target/usr/include
ASM_CFLAGS += -I$(SDK_DIR)/tools/linux/toolchains/aarch64-histbv400-linux/lib/gcc/aarch64-gcc51_glibc222-linux-gnu/5.1.1/include
endif

################################################################################################

ASM_CFLAGS += -I$(SDK_DIR)/source/linux/drv/include
ASM_CFLAGS += -I$(SDK_DIR)/source/linux/drv/common/include
ASM_CFLAGS += -I$(SDK_DIR)/source/linux/drv/common/include/hi3796cv300

ASM_CFLAGS += $(VFMW_CFLAGS)
VFMW_SRC_DIR = $(VFMW_BASE)
ALL_SRC := $(VFMW_CORE_SRC:c=S)

#v6
OPEN_SRC :=$(VFMW_BASE)/core/common/%.S $(VFMW_BASE)/core/dps/%.S $(VFMW_BASE)/core/stream/%.S
OPEN_SRC +=$(VFMW_BASE)/core/decode/dec_chn.S $(VFMW_BASE)/core/decode/dec_ctrl.S $(VFMW_BASE)/core/decode/dec_dev.S $(VFMW_BASE)/core/decode/dec_intf.S $(VFMW_BASE)/core/decode/dec_log.S $(VFMW_BASE)/core/decode/dec_post.S $(VFMW_BASE)/core/decode/dec_ref.S $(VFMW_BASE)/core/decode/dec_thread.S
OPEN_SRC +=$(VFMW_BASE)/master/%.S
OPEN_SRC +=$(VFMW_BASE)/osal/%.S
OPEN_SRC +=$(VFMW_BASE)/slave/%.S
OPEN_SRC +=$(VFMW_BASE)/product/hi3796cv300/product.S

ASM_SRC = $(filter-out $(OPEN_SRC),$(ALL_SRC))
ASM_SRC_TEMP = $(addsuffix .tmp,$(ASM_SRC))

ifeq ($(CFG_HI_SMMU_SUPPORT), y)
ASM_CFLAGS += -DVFMW_MMU_SUPPORT
endif

ifeq ($(CFG_KO2_SUPPORT), y)
ASM_CFLAGS += -DVFMW_KO2_SUPPORT
endif

ifeq ($(CFG_SAFE_REG), y)
ASM_CFLAGS += -DVFMW_SAFE_REG
endif

ASM_CFLAGS += -DCHIP_TYPE_HI3796CV300

ifeq ($(CFG_HI_SMMU_SUPPORT), y)
ASM_CFLAGS += -DVFMW_MMU_SUPPORT
endif
$(warning "----------"$(PRODUCT_DIR_V6))
.PHONY: asm asm_clean
asm: $(ASM_SRC)
	mkdir -p ${DST_DIR}/core/decode
	mkdir -p ${DST_DIR}/core/syntax

	cd $(VFMW_BASE)/core/decode && find ./ -name "*.h" | xargs cp -f -t $(VFMW_DIR)/product/$(PRODUCT_DIR_V6)/ && cd -
	cd $(VFMW_BASE)/core/syntax && find ./ -name "*.h" | xargs cp -f -t $(VFMW_DIR)/product/$(PRODUCT_DIR_V6)/ && cd -

	cd $(VFMW_BASE)/core/decode && find ./ -name "*.S" | xargs cp -rf --parent -t ${DST_DIR}/core/decode && cd -
	cd $(VFMW_BASE)/core/decode && find ./ -name "*.S" | xargs rm -rf "*.S" && cd -

	cd $(VFMW_BASE)/core/syntax && find ./ -name "*.S" | xargs cp -rf --parent -t ${DST_DIR}/core/syntax && cd -
	cd $(VFMW_BASE)/core/syntax && find ./ -name "*.S" | xargs rm -rf "*.S" && cd -

asm_clean:
	rm -rf $(ASM_SRC) $(ASM_SRC_TEMP)
	rm -rf $(VFMW_BASE)/product/$(PRODUCT_DIR_V6)/linux_cfg/$(SYSTEM_CONFIG)
