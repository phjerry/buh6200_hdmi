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
panel_path := panel

EXTRA_CFLAGS += -I$(DRV_DIR)/include \
                -I$(DRV_DIR)/common/include \
                -I$(USR_DIR)/securec         \
                -I$(DRV_DIR)/vo/include/hi3796cv300/ \
                -I$(DRV_DIR)/$(panel_path) \
                -I$(DRV_DIR)/$(panel_path)/hal \
                -I$(DRV_DIR)/$(panel_path)/hal/include/V900 \
                -I$(DRV_DIR)/$(panel_path)/adapter \
                -I$(DRV_DIR)/$(panel_path)/backlight/0d \
                -I$(DRV_DIR)/$(panel_path)/backlight/local \
                -I$(DRV_DIR)/$(panel_path)/data \
                -I$(DRV_DIR)/$(panel_path)/interface/vbo \
                -I$(DRV_DIR)/$(panel_path)/interface/combophy

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

MOD_NAME := hi_panel

$(MOD_NAME)-y :=    drv_panel.o \
                    drv_panel_intf.o \
                    drv_panel_ctrl.o \
                    drv_panel_power.o \
                    drv_panel_proc.o \
                    hal/hal_panel_comm.o \
                    adapter/drv_panel_adapter.o \
                    backlight/0d/drv_panel_0ddim.o \
                    backlight/0d/drv_panel_pwm.o \
                    hal/hal_panel_vbo.o\
                    hal/hal_panel_combophy.o \
                    hal/hal_panel_dim.o \
                    hal/hal_panel_pwm.o \
                    interface/vbo/drv_panel_vbo.o \
                    interface/combophy/drv_panel_combophy.o \
                    data/drv_panel_data.o \
                    data/drv_panel_param.o

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

clean:
	$(AT)make -C $(LINUX_DIR) ARCH=$(CFG_HI_CPU_ARCH) CROSS_COMPILE=$(HI_KERNEL_TOOLCHAINS_NAME)- M=$(BUILD_DIR) src=$(CURDIR) clean
	$(AT)rm -rf $(HI_MODULE_DIR)/$(MOD_NAME).ko

$(BUILD_DIR_MAKEFILE): $(BUILD_DIR)
	$(AT)touch $@

$(BUILD_DIR):
	$(AT)mkdir -p $@
