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
# local variable
#===============================================================================
DRV_SRC_BASE=$(DRV_DIR)/keyled

EXTRA_CFLAGS += -I$(UNF_DIR)         \
                -I$(DRV_DIR)/include \
                -I$(DRV_DIR)/common/include \
                -I$(DRV_DIR)/keyled

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)


MOD_NAME := hi_keyled

obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o

ifeq ($(CFG_HI_KEYLED_CT1642_KERNEL_SUPPORT),y)
$(MOD_NAME)-y   :=  drv_keyled_init_ct1642.o
endif

$(MOD_NAME)-y   +=  drv_keyled_intf.o


ifeq ($(CFG_HI_KEYLED_CT1642_SUPPORT), y)
    EXTRA_CFLAGS += -DKEYLED_CT1642
    EXTRA_CFLAGS += -I$(DRV_SRC_BASE)/keyled_ct1642
    ifneq ($(CFG_HI_KEYLED_CT1642_GPIO_MODE),y)
        $(MOD_NAME)-y  += keyled_ct1642/drv_keyled_ct1642_inner.o
    else
    ifdef CFG_HI_KEYLED_CT1642_CLOCK_GPIO
        EXTRA_CFLAGS += -DHI_KEYLED_CT1642_CLOCK_GPIO=$(CFG_HI_KEYLED_CT1642_CLOCK_GPIO)
        EXTRA_CFLAGS += -DHI_KEYLED_CT1642_DAT_GPIO=$(CFG_HI_KEYLED_CT1642_DAT_GPIO)
        EXTRA_CFLAGS += -DHI_KEYLED_CT1642_KEY_GPIO=$(CFG_HI_KEYLED_CT1642_KEY_GPIO)
    endif
        $(MOD_NAME)-y  += keyled_ct1642/drv_keyled_ct1642.o
    endif
endif

ifeq ($(CFG_HI_KEYLED_FD650_SUPPORT), y)
    EXTRA_CFLAGS += -I$(DRV_SRC_BASE)/keyled_fd650
    EXTRA_CFLAGS += -DKEYLED_FD650
    $(MOD_NAME)-y  += keyled_fd650/drv_keyled_fd650.o
ifdef CFG_HI_KEYLED_FD650_CLOCK_GPIO
    EXTRA_CFLAGS += -DHI_KEYLED_FD650_CLOCK_GPIO=$(CFG_HI_KEYLED_FD650_CLOCK_GPIO)
    EXTRA_CFLAGS += -DHI_KEYLED_FD650_DINOUT_GPIO=$(CFG_HI_KEYLED_FD650_DINOUT_GPIO)
endif
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

