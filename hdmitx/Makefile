#===============================================================================
# export variables
#===============================================================================
ifneq ($(srctree),)
    KERNEL_DIR := $(srctree)

    SDK_DIR := $(shell cd $(KERNEL_DIR)/../../../.. && /bin/pwd)
else
    SDK_DIR := $(shell cd $(CURDIR)/../../../../ && /bin/pwd)
endif

include $(SDK_DIR)/base.mak

HDMI_CTRL := hisiv300_ctrl
HDMI_PHY  := hisiv300_phy

HDMI_DRV_DIR := $(MSP_DIR)/drv/hdmitx/

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

EXTRA_CFLAGS += -I$(COMMON_UNF_INCLUDE)                 \
                -I$(COMMON_API_INCLUDE)                 \
                -I$(COMMON_DRV_INCLUDE)                 \
                -I$(MSP_UNF_INCLUDE)                    \
                -I$(MSP_API_INCLUDE)                    \
                -I$(MSP_DRV_INCLUDE)                    \
                -I$(DRV_DIR)/include                    \
                -I$(DRV_DIR)/hdmitx                       \
                -I$(DRV_DIR)/hdmitx/hal                   \
                -I$(DRV_DIR)/hdmitx/hal/$(HDMI_CTRL)      \
                -I$(DRV_DIR)/hdmitx/hal/$(HDMI_CTRL)/include

ifeq ($(CFG_HI_TEE_SUPPORT),y)
    EXTRA_CFLAGS += -DHI_HDMI_TEE_SUPPORT
endif

ifneq ($(CFG_HI_HDMITX_HDCP1X_SUPPORT)_$(CFG_HI_HDMITX_HDCP2X_SUPPORT),_)
EXTRA_CFLAGS += -DHI_HDMITX_HDCP_SUPPORT
endif

MOD_NAME := hi_hdmitx

obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o

$(MOD_NAME)-y   +=  drv_hdmitx.o                    \
                    drv_hdmitx_connector.o          \
                    drv_hdmitx_edid.o               \
                    drv_hdmitx_dsc.o                \
                    drv_hdmitx_ddc.o                \
                    drv_hdmitx_infoframe.o          \
                    drv_hdmitx_sysfs.o              \
                    drv_hdmitx_debug.o              \
                    drv_hdmitx_modes.o              \
                    drv_hdmitx_proc.o	            \
                    drv_hdmitx_trace.o              \
                    drv_hdmitx_cec.o                \
                    hal/hal_hdmitx_io.o             \
                    hal/$(HDMI_CTRL)/hal_hdmitx_dsc.o   \
                    hal/$(HDMI_CTRL)/hal_hdmitx_ctrl.o  \
                    hal/$(HDMI_CTRL)/hal_hdmitx_frl.o   \
                    hal/$(HDMI_CTRL)/hal_hdmitx_ddc.o   \
                    hal/$(HDMI_CTRL)/hal_hdmitx_crg.o   \
                    hal/$(HDMI_CTRL)/hal_hdmitx_phy.o   \
                    hal/$(HDMI_CTRL)/hal_hdmitx_cec.o

ifneq ($(CFG_HI_HDMITX_HDCP1X_SUPPORT)_$(CFG_HI_HDMITX_HDCP2X_SUPPORT),_)
$(MOD_NAME)-y   +=  drv_hdmitx_hdcp.o                          \
                    hal/$(HDMI_CTRL)/hal_hdmitx_hdcp1x.o       \
                    hal/$(HDMI_CTRL)/hal_hdmitx_hdcp2x.o
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
	$(AT)rm -rf $(HI_MODULE_DIR)/$(MOD_NAME).o
	$(AT)rm -rf $(HI_MODULE_DIR)/$(MOD_NAME).ko

$(BUILD_DIR_MAKEFILE): $(BUILD_DIR)
	$(AT)touch $@

$(BUILD_DIR):
	$(AT)mkdir -p $@

