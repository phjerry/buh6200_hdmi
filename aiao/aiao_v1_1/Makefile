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

#===============================================================================
# local variables
#===============================================================================
DRV_ADSP := $(DRV_DIR)/adsp/adsp_v1_1
DRV_AIAO := $(DRV_DIR)/aiao/aiao_v1_1

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

EXTRA_CFLAGS += \
    -I$(USR_DIR)/include \
    -I$(USR_DIR)/msp/include \
    -I$(USR_DIR)/common/include \
    -I$(DRV_DIR)/include \
    -I$(DRV_DIR)/common/include \
    -I$(DRV_ADSP)/include

EXTRA_CFLAGS += \
    -I$(DRV_AIAO)/ao \
    -I$(DRV_AIAO)/osal \
    -I$(DRV_AIAO)/common/include

ifeq ($(CFG_HI_AUDIO_EXTERNAL_AMP),y)
EXTRA_CFLAGS += -DHI_AUDIO_EXTERNAL_AMP
endif

ifeq ($(CFG_HI_AUDIO_INTERNAL_AMP),y)
EXTRA_CFLAGS += -DHI_AUDIO_INTERNAL_AMP
endif

ifeq ($(CFG_HI_AUDIO_AMP_AUTO_SELECT_MODE),y)
EXTRA_CFLAGS += -DHI_AUDIO_AMP_AUTO_SELECT
endif

ifeq ($(CFG_HI_SND_MUTECTL_SUPPORT),y)
EXTRA_CFLAGS += -DHI_SND_MUTECTL_SUPPORT
EXTRA_CFLAGS += -DHI_SND_MUTECTL_GPIO=$(CFG_HI_SND_MUTECTL_GPIO)
EXTRA_CFLAGS += -DHI_SND_MUTECTL_LEVEL=$(CFG_HI_SND_MUTECTL_LEVEL)
endif
#===============================================================================
ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3796cv300 hi3751v900),)
# DSP AOE
# EXTRA_CFLAGS += -DHI_SND_DSP_SUPPORT
endif
#===============================================================================
ifeq ($(CFG_HI_SND_PORT_DELAY_SUPPORT),y)
EXTRA_CFLAGS += -DHI_SOUND_PORT_DELAY_SUPPORT
endif
#===============================================================================
EXTRA_CFLAGS += -DHI_SND_DRV_SUSPEND_SUPPORT
EXTRA_CFLAGS += -DHI_SND_HDMI_PASSTHROUGH_USE_I2S_ONLY

ifeq ($(CFG_HI_SND_HBRA_PASSTHROUGH_SUPPORT),y)
EXTRA_CFLAGS += -DHI_SND_HBRA_PASSTHROUGH_SUPPORT
endif
#===============================================================================
MOD_NAME := hi_aiao

obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o

$(MOD_NAME)-y := \
    drv_aiao_module.o \
    osal/audio_mem.o \
    common/src/audio_util.o \
    common/src/hal_aiao.o \
    common/src/hal_aiao_func.o \
    common/src/hal_aiao_priv.o \
    common/src/hal_tianlai_adac.o

$(MOD_NAME)-y += \
    ao/drv_ao.o \
    ao/drv_ao_engine.o \
    ao/hal_aoe_func.o \
    ao/hal_aoe.o \
    ao/drv_ao_intf.o \
    ao/drv_ao_ext.o
#===============================================================================
include $(DRV_AIAO)/ao/ao.mak
include $(DRV_AIAO)/ai/ai.mak
include $(DRV_AIAO)/alsa/alsa.mak
#===============================================================================
ifeq ($(CFG_HI_AIAO_VERIFICATION_SUPPORT),y)
EXTRA_CFLAGS += -DHI_AIAO_VERIFICATION_SUPPORT
$(MOD_NAME)-y += common/src/drv_aiao_veri.o
endif
#===============================================================================
ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3796cv300 hi3751v900),)
EXTRA_CFLAGS += -DHI_TIANLAI_SV100
$(MOD_NAME)-y += common/src/hal_tianlai_adac_sv100.o
# EXTRA_CFLAGS += -DHI_AIAO_TIMER_SUPPORT
# EXTRA_CFLAGS += -I$(DRV_AIAO)/timer
# $(MOD_NAME)-y += timer/drv_timer.o
endif
#===============================================================================
ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3796cv300 hi3751v900),)
EXTRA_CFLAGS += -DHI_I2S0_SUPPORT
EXTRA_CFLAGS += -DHI_I2S1_SUPPORT
endif
#===============================================================================
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
