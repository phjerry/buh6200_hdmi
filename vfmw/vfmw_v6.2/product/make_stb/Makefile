#===============================================================================
# export variables
#===============================================================================
ifeq ($(CFG_HI_EXPORT_FLAG),)
    ifneq ($(srctree),)
        KERNEL_DIR := $(srctree)
        SDK_DIR := $(shell cd $(KERNEL_DIR)/../../../.. && /bin/pwd)
    else
        SDK_DIR := $(shell cd $(CURDIR)/../../../../../../.. && /bin/pwd)
    endif

    include $(SDK_DIR)/base.mak
endif
export SDK_DIR
#===============================================================================
# export variables  arm64 or arm
#===============================================================================
########tmp value###############################
ifeq ($(CFG_HI_CPU_ARCH),arm)
EXTRA_CFLAGS += -DHI_VFMW_32BIT
endif

# local variables
#===============================================================================
export VFMW_DIR := $(DRV_DIR)/vfmw/vfmw_v6.2

EXTRA_CFLAGS += $(filter-out -Wformat-security,$(CFG_HI_KMOD_CFLAGS))

EXTRA_CFLAGS += -I$(DRV_DIR)/include \
                -I$(DRV_DIR)/common/include \
                -I$(USR_DIR)/securec

ifeq ($(CFG_HI_TEE_SUPPORT),y)
EXTRA_CFLAGS += -I$(TEE_DRV_INCLUDE)
VFMW_SEC_SUPPORT := y
EXTRA_CFLAGS += -DVFMW_SEC_SUPPORT
endif

################################################################################################
#    PRODUCT
################################################################################################

export VFMW_INC_DIR = $(abspath $(VFMW_DIR))
VFMW_SRC_DIR = ../..

ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3796cv300 hi3751v900),)
include $(VFMW_DIR)/product/hi3796cv300/linux_cfg/vfmw_make.cfg
endif

CFG_KO2_SUPPORT := n
CFG_SAFE_REG    := n

ifeq ($(CFG_KO2_SUPPORT), y)
EXTRA_CFLAGS += -DVFMW_KO2_SUPPORT
endif

ifeq ($(CFG_SAFE_REG), y)
EXTRA_CFLAGS += -DVFMW_SAFE_REG
endif

EXTRA_CFLAGS += $(VFMW_CFLAGS)
EXTRA_CFLAGS += -fno-pic

ifeq ($(CFG_HI_SMMU_SUPPORT), y)
EXTRA_CFLAGS += -DVFMW_MMU_SUPPORT
endif

ifeq ($(CFG_KO2_SUPPORT), n)
MOD_NAME := hi_vfmw
else
MOD_NAME := hi_vfmw2
endif

obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o

VFMW_OBJ := $(VFMW_SRC:%.c=%.o)
$(MOD_NAME)-y := $(VFMW_OBJ)

ifeq ($(VFMW_MDC_SUPPORT), y)
LOS_TGT_DIR := $(SDK_DIR)/source/liteos
MDC_IMG_DIR := $(VFMW_DIR)/master/client/mdc/img
ifeq ($(VFMW_SEC_SUPPORT), y)
VFMW_DAT_DIR := $(LOS_TGT_DIR)/vfmw_tee/img
else
VFMW_DAT_DIR := $(LOS_TGT_DIR)/vfmw_ree/img
endif
endif

BUILD_DIR := $(HI_OUT_DIR)/$(OBJ_NAME)/$(subst $(SDK_DIR)/,,$(CURDIR))
BUILD_DIR_MAKEFILE := $(BUILD_DIR)/Makefile

#===============================================================================
#   rules
#===============================================================================
.PHONY: all clean

all: $(BUILD_DIR_MAKEFILE)
ifeq ($(VFMW_MDC_SUPPORT), y)
	$(AT)mkdir -p $(MDC_IMG_DIR)
	$(AT)cp -rf $(VFMW_DAT_DIR)/vmdc_image.dat $(MDC_IMG_DIR)/
endif
	$(AT)make -C $(LINUX_DIR) ARCH=$(CFG_HI_CPU_ARCH) CROSS_COMPILE=$(HI_KERNEL_TOOLCHAINS_NAME)- M=$(BUILD_DIR) src=$(CURDIR) modules
	$(AT)test -d $(HI_MODULE_DIR) || mkdir -p $(HI_MODULE_DIR)
	$(AT)cp -f $(BUILD_DIR)/$(MOD_NAME).ko $(HI_MODULE_DIR)/
	$(AT)cp -f $(BUILD_DIR)/$(MOD_NAME).ko $(VFMW_DIR)/
ifeq ($(VFMW_MDC_SUPPORT), y)
	$(AT)rm -rf $(MDC_IMG_DIR)
endif

vmdc:
ifeq ($(VFMW_MDC_SUPPORT), y)
	$(warning "enable vmdc")
	$(AT)mkdir -p $(MDC_IMG_DIR)
	$(AT)cp -rf $(VFMW_DAT_DIR)/vmdc_image.dat $(MDC_IMG_DIR)/
else
	$(warning "disable vmdc")
endif

vmdc_clean:
ifeq ($(VFMW_MDC_SUPPORT), y)
	$(warning "enable vmdc")
	$(AT)rm -rf $(MDC_IMG_DIR)
else
	$(warning "disable vmdc")
endif

clean:
ifeq ($(VFMW_MDC_SUPPORT), y)
	$(warning "enable vmdc")
	$(AT)rm -rf $(MDC_IMG_DIR)
else
	$(warning "disable vmdc")
endif
	$(AT)make -C $(LINUX_DIR) ARCH=$(CFG_HI_CPU_ARCH) CROSS_COMPILE=$(HI_KERNEL_TOOLCHAINS_NAME)- M=$(BUILD_DIR)/../../.. src=$(CURDIR) clean
	$(AT)rm -rf $(HI_MODULE_DIR)/$(MOD_NAME).ko

$(BUILD_DIR_MAKEFILE): $(BUILD_DIR)
	$(AT)touch $@

$(BUILD_DIR):
	$(AT)mkdir -p $@

