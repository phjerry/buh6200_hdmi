#===============================================================================
# export variables
#===============================================================================
ifneq ($(srctree),)
    KERNEL_DIR := $(srctree)

    SDK_DIR := $(shell cd $(KERNEL_DIR)/../../../.. && /bin/pwd)
else
    SDK_DIR := $(shell cd $(CURDIR)/../../../.. && /bin/pwd)
endif

include $(SDK_DIR)/base.mak


#===============================================================================
# hal dir
#===============================================================================
HAL_VER := v1_0

#===============================================================================
# local variables
#===============================================================================
EXTRA_CFLAGS += -I$(DRV_DIR)/include                    \
                -I$(DRV_DIR)/memory                     \
                -I$(DRV_DIR)/common/include             \
                -I$(DRV_DIR)/common/log                 \
                -I$(DRV_DIR)/hifb/include           \
                -I$(DRV_DIR)/hifb/inc               \
                -I$(DRV_DIR)/hifb/adp/inc           \
                -I$(DRV_DIR)/hifb/adp/src           \
                -I$(DRV_DIR)/hifb/adp/hal_$(HAL_VER)\
                -I$(DRV_DIR)/hifb/adp/alg           \
                -I$(DRV_DIR)/jpeg/grc_cmm_inc       \
                -I$(DRV_DIR)/tde/include            \
                -I$(DRV_DIR)/vo/include

EXTRA_CFLAGS += -DCONFIG_GFX_PQ

EXTRA_CFLAGS += -DCONFIG_HIFB_VERSION_4_0

EXTRA_CFLAGS += -DCONFIG_HIFB_WBC_UNSUPPORT

ifeq ($(CFG_HIFB_DISP_WINDOW_SYNC_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_HIFB_DISP_WINDOW_SYNC_SUPPORT
endif

ifneq ($(findstring $(CONFIG_SYNC), y),)
ifneq ($(CFG_HI_RECOVERY_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_HIFB_FENCE_SUPPORT
endif
endif

ifeq ($(CFG_ANDROID_PRODUCT),y)
ifneq ($(CFG_HI_RECOVERY_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_HIFB_FENCE_SUPPORT
endif
endif

ifeq ($(CFG_ANDROID_PRODUCT),y)
EXTRA_CFLAGS += -DCFG_HIFB_ANDROID_SUPPORT
endif

ifeq ($(CFG_HI_FB_DECOMPRESS_SUPPORT), y)
EXTRA_CFLAGS += -DCFG_HI_FB_DECOMPRESS_SUPPORT
endif

#===============================================================================
# setting stereo scrolltext and capture
#===============================================================================
ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3716mv430),)
   ifeq ($(CFG_HI_FB_SCROLLTEXT_SUPPORT), y)
      #EXTRA_CFLAGS += -DCONFIG_HIFB_SCROLLTEXT_SUPPORT
   endif
   ifeq ($(CFG_HI_FB_CAPTURE_BMP_SUPPORT), y)
      EXTRA_CFLAGS += -DCONFIG_HIFB_CAPTURE_BMP_SUPPORT
   endif
   ifeq ($(CFG_HI_FB_STEREO_SUPPORT), y)
      EXTRA_CFLAGS += -DCONFIG_HIFB_STEREO_SUPPORT
   endif
else
   #EXTRA_CFLAGS += -DCONFIG_HIFB_SCROLLTEXT_SUPPORT
   EXTRA_CFLAGS += -DCONFIG_HIFB_CAPTURE_BMP_SUPPORT
   EXTRA_CFLAGS += -DCONFIG_HIFB_STEREO_SUPPORT
endif

#===============================================================================
# default mem size
#===============================================================================
ifneq ($(CFG_HI_HD0_FB_VRAM_SIZE),)
EXTRA_CFLAGS += -DCFG_HI_HD0_FB_VRAM_SIZE=$(CFG_HI_HD0_FB_VRAM_SIZE)
endif
ifneq ($(CFG_HI_HD1_FB_VRAM_SIZE),)
EXTRA_CFLAGS += -DCFG_HI_HD1_FB_VRAM_SIZE=$(CFG_HI_HD1_FB_VRAM_SIZE)
endif
ifneq ($(CFG_HI_HD2_FB_VRAM_SIZE),)
EXTRA_CFLAGS += -DCFG_HI_HD2_FB_VRAM_SIZE=$(CFG_HI_HD2_FB_VRAM_SIZE)
endif
ifneq ($(CFG_HI_HD3_FB_VRAM_SIZE),)
EXTRA_CFLAGS += -DCFG_HI_HD3_FB_VRAM_SIZE=$(CFG_HI_HD3_FB_VRAM_SIZE)
endif

EXTRA_CFLAGS += -DCONFIG_GFX_STB_SDK
EXTRA_CFLAGS += -DCONFIG_HIFB_LOGO_SUPPORT
EXTRA_CFLAGS += -DCONFIG_HIFB_LOGO_DEBUG
#EXTRA_CFLAGS += -DCONFIG_GFX_CANVASE_TO_LAYER_SUPPORT
####################################################
#beg debug
ifeq ($(CFG_HI_PROC_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_GFX_PROC_SUPPORT
endif

ifeq ($(CFG_HI_LOG_LEVEL_DEBUG),y)
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_DEBUG
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_INFO
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

ifeq ($(CFG_HI_LOG_LEVEL_INFO),y)
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_INFO
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

ifeq ($(CFG_HI_LOG_LEVEL_WARNING),y)
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

ifeq ($(CFG_HI_LOG_LEVEL_ERROR),y)
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

ifeq ($(CFG_HI_LOG_LEVEL_FATAL),y)
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_ERR
endif

EXTRA_CFLAGS += -DCONFIG_GFX_DFX_MOD_FB
ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3716mv430),)
#EXTRA_CFLAGS += -DCONFIG_USE_SDK_LOG
else
EXTRA_CFLAGS += -DCONFIG_GFX_DFX_MINI
endif

ifneq ($(HI_LOG_SUPPORT),y)
#EXTRA_CFLAGS += -DCONFIG_USE_SDK_LOG
endif

#end debug
####################################################

ifeq ($(CFG_HI_SMMU_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_GFX_MMU_SUPPORT
endif

ifeq ($(CFG_HI_FB0_SMMU_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_GFX_HI_FB0_SMMU_SUPPORT
endif

ifeq ($(CFG_HI_FB1_SMMU_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_GFX_HI_FB1_SMMU_SUPPORT
endif

ifeq ($(CFG_HI_FB2_SMMU_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_GFX_HI_FB2_SMMU_SUPPORT
endif

ifeq ($(CFG_HI_FB3_SMMU_SUPPORT),y)
EXTRA_CFLAGS += -DCONFIG_GFX_HI_FB3_SMMU_SUPPORT
endif

MOD_NAME := hi_fb

obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

DRIVER_PATH := $(DRV_DIR)/hifb

COMMON_FILES := $(wildcard $(DRIVER_PATH)/src/*.c)
ADAPT_FILES  := $(wildcard $(DRIVER_PATH)/adp/src/*.c)
HAL_FILES    := $(wildcard $(DRIVER_PATH)/adp/hal_$(HAL_VER)/*.c)
ALG_FILES    := $(wildcard $(DRIVER_PATH)/adp/alg/*.c)

FULL_PATH_DRV_FILES += $(HAL_FILES)    \
         $(ADAPT_FILES)  \
         $(COMMON_FILES) \
         $(ALG_FILES)

#ifeq ($(CFG_HIFB_SCROLLTEXT_SUPPORT), y)
#FULL_PATH_DRV_FILES := $(filter-out hifb_scrolltext.c, $(FULL_PATH_DRV_FILES))
#endif

DRV_SRCS := $(shell echo $(FULL_PATH_DRV_FILES) \
            | awk '{for(i=1;i<=NF;i++)print $$i}' \
            | sed "s:\(.*\)/\<hifb\>\(.*\):.\2:g")

DRV_SRCS_C := $(filter-out %.S, $(DRV_SRCS))
DRV_SRCS_A := $(filter-out %.c, $(DRV_SRCS))

DRV_OBJS := $(subst .c,.o,$(DRV_SRCS_C))
DRV_OBJS += $(subst .S,.o,$(DRV_SRCS_A))

$(MOD_NAME)-y := $(DRV_OBJS)

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

