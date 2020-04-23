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

EXTRA_CFLAGS += -I$(DRV_DIR)/include \
                -I$(DRV_DIR)/common/include

EXTRA_CFLAGS += -DDMX_DESCRAMBLER_SUPPORT

ifdef CFG_HI_ADVCA_FUNCTION
    ifeq ($(CFG_HI_ADVCA_FUNCTION),DEBUG)
        EXTRA_CFLAGS += -DHI_DEMUX_PROC_SUPPORT
    endif
else
    EXTRA_CFLAGS += -DHI_DEMUX_PROC_SUPPORT
endif

ifdef CFG_HI_SMMU_SUPPORT
EXTRA_CFLAGS += -DDMX_SMMU_SUPPORT
endif

ifdef CFG_HI_MULTI_THREAD_SUPPORT
EXTRA_CFLAGS += -DHI_DMX_TSBUF_MULTI_THREAD_SUPPORT
endif

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

MOD_NAME := hi_demux

$(MOD_NAME)-y :=    drv_demux.o \
                    drv_demux_dscfct.o \
                    drv_demux_func.o \
                    drv_demux_index.o \
                    drv_demux_intf.o \
                    drv_demux_plyfct.o \
                    drv_demux_recfct.o \
                    drv_demux_rmxfct.o \
                    drv_demux_scd.o \
                    drv_demux_utils.o \
                    hal_demux.o

ifeq ($(CFG_HI_TEE_SUPPORT),y)
EXTRA_CFLAGS += -I$(TEE_DRV_INCLUDE) -DDMX_TEE_SUPPORT
$(MOD_NAME)-y +=    drv_demux_tee.o
endif

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
