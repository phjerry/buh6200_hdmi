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

hal_ctrl=hisiv300_ctrl
hal_phy=hisiv300_phy
chip_name=v900

ifeq ($(CFG_HI_CHIP_TYPE), hi3751v900)
    hal_ctrl=hisiv300_ctrl
    hal_phy=hisiv300_phy
    chip_name=v900
endif

###############################################################################
# local variables
###############################################################################

## header search path
hdmirx_path := hdmirx

$(warning $(CFG_HI_CHIP_TYPE))

EXTRA_CFLAGS += -I$(USR_DIR)/securec                                  \
                -I$(DRV_DIR)/common/include                           \
                -I$(DRV_DIR)/include                                  \
                -I$(DRV_DIR)/$(hdmirx_path)                           \
                -I$(DRV_DIR)/$(hdmirx_path)/hal                       \
                -I$(DRV_DIR)/$(hdmirx_path)/hal/product/$(chip_name)  \
                -I$(DRV_DIR)/$(hdmirx_path)/hal/$(hal_ctrl)           \
                -I$(DRV_DIR)/$(hdmirx_path)/hal/$(hal_ctrl)/regs      \
                -I$(DRV_DIR)/$(hdmirx_path)/hal/$(hal_phy)            \
                -I$(DRV_DIR)/$(hdmirx_path)/hal/$(hal_phy)/regs

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

MOD_NAME := hi_hdmirx

$(MOD_NAME)-y += drv_hdmirx_common.o  \
                 drv_hdmirx_packet.o  \
                 drv_hdmirx_video.o   \
                 drv_hdmirx_audio.o   \
                 drv_hdmirx_hdcp.o    \
                 drv_hdmirx_ctrl.o    \
                 drv_hdmirx_proc.o    \
                 drv_hdmirx_intf.o    \
                 drv_hdmirx_edid.o    \
                 drv_hdmirx_cec.o     \
                 drv_hdmirx_phy.o

$(MOD_NAME)-y += hal/hal_hdmirx_comm.o
$(MOD_NAME)-y += hal/$(hal_ctrl)/hal_hdmirx_video.o
$(MOD_NAME)-y += hal/$(hal_ctrl)/hal_hdmirx_ctrl.o
$(MOD_NAME)-y += hal/$(hal_ctrl)/hal_hdmirx_audio.o
$(MOD_NAME)-y += hal/$(hal_ctrl)/hal_hdmirx_hdcp.o
$(MOD_NAME)-y += hal/$(hal_phy)/hal_hdmirx_phy.o

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
	$(AT)cp -f $(BUILD_DIR)/$(MOD_NAME).ko $(HI_MODULE_DIR)/

clean:
	$(AT)make -C $(LINUX_DIR) ARCH=$(CFG_HI_CPU_ARCH) CROSS_COMPILE=$(HI_KERNEL_TOOLCHAINS_NAME)- M=$(BUILD_DIR) src=$(CURDIR) clean
	$(AT)rm -rf $(HI_MODULE_DIR)/$(MOD_NAME).ko

$(BUILD_DIR_MAKEFILE): $(BUILD_DIR)
	$(AT)touch $@

$(BUILD_DIR):
	$(AT)mkdir -p $@
