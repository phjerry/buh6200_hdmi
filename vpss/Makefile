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

$(warning  $(SDK_DIR))

TARGET := vpss
ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3796cv300 hi3751v900),)
DRIVER_PATH := $(DRV_DIR)/vpss
endif

ifeq (y, $(CFG_HI_FPGA))
EXTRA_CFLAGS += -DHI_FPGA
$(warning "We will build FPGA vpss driver version")
else
$(warning "We will build ASIC vpss driver version")
endif
#EXTRA_CFLAGS += -DHI_FPGA

#EXTRA_CFLAGS += -DHI_VPSS_DRV_USE_GOLDEN
#存图，每个通道，用于保证各个通道的数据正确；打开后理论上会存所有的图像出来

#EXTRA_CFLAGS += -DHI_VPSS_DRV_USE_GOLDEN_COEF
#强制刷系数，不依赖任何PQ参数；在PQ未ready的时候打开刷默认参数；后续PQ上来之后也可以用作自测使用

#宽高比dbg
#EXTRA_CFLAGS += -DVPSS_LBX_DBG
#===============================================================================
# local variables
#===============================================================================
EXTRA_CFLAGS += -I$(DRV_DIR)/include                       \
                -I$(DRV_DIR)/common/include                \
                -I$(DRIVER_PATH)                           \
                -I$(DRIVER_PATH)/comm                      \
                -I$(DRIVER_PATH)/drv                       \
                -I$(DRIVER_PATH)/chip

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)
EXTRA_CFLAGS += -DHI_VPSS_MAX_BUFFER_NUMB=6
#EXTRA_CFLAGS += -DLOG_D_MODULE_ID=HI_ID_VPSS
EXTRA_CFLAGS += -DLOG_D_MODULE_ID=HI_ID_VPSS

ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3796cv300 hi3751v900),)
    EXTRA_CFLAGS += -I$(DRV_DIR)/vpss/reg_inc/hi3796cv300
    EXTRA_CFLAGS += -DHI_VPSS_DRV_VER_96CV300
    EXTRA_CFLAGS += -DHI_VPSS_DRV_USE_PQ
#    EXTRA_CFLAGS += -DHI_VPSS_DRV_USE_GOLDEN
    CHIP_DIR     := chip/hi3796cv300
$(warning "We will build vpss driver version with hi3796cv300")
endif

$(warning  $(CHIP_DIR))
$(warning  $(DRIVER_PATH))

ifeq ($(CFG_HI_SMMU_SUPPORT),y)
    EXTRA_CFLAGS += -DHI_VPSS_SMMU_SUPPORT
    $(warning "We will build vpss driver version with smmu")
else
    $(warning "We will build vpss driver version with nosmmu")
endif

EXTRA_CFLAGS += -I$(DRIVER_PATH)/$(CHIP_DIR)                     \
                -I$(DRIVER_PATH)/$(CHIP_DIR)/hal                 \
                -I$(DRIVER_PATH)/$(CHIP_DIR)/hal/cbb             \
                -I$(DRIVER_PATH)/$(CHIP_DIR)/wbc                 \
                -I$(DRIVER_PATH)/$(CHIP_DIR)/stt                 

MOD_NAME := hi_$(TARGET)

HAL_DEV_FILES      := $(wildcard $(DRIVER_PATH)/$(CHIP_DIR)/hal/*.c)
HAL_CBB_FILES      := $(wildcard $(DRIVER_PATH)/$(CHIP_DIR)/hal/cbb/*.c)
HAL_WBC_FILES      := $(wildcard $(DRIVER_PATH)/$(CHIP_DIR)/wbc/*.c)
HAL_STT_FILES      := $(wildcard $(DRIVER_PATH)/$(CHIP_DIR)/stt/*.c)
HAL_POLICY_FILES   := $(wildcard $(DRIVER_PATH)/$(CHIP_DIR)/policy/*.c)

HAL_FILES := $(HAL_CBB_FILES)
HAL_FILES += $(HAL_WBC_FILES)
HAL_FILES += $(HAL_STT_FILES)
HAL_FILES += $(HAL_POLICY_FILES)

HAL_FILES += $(HAL_DEV_FILES)

COM_FILES    := $(wildcard $(DRIVER_PATH)/comm/*.c)
DRV_FILES    := $(wildcard $(DRIVER_PATH)/drv/*.c)
INTF_FILES   := $(wildcard $(DRIVER_PATH)/*.c)

VPSS_FILES   := $(HAL_FILES)           \
                $(COM_FILES)           \
                $(DRV_FILES)           \
                $(INTF_FILES)

VPSS_FILES   := $(filter-out %.mod.c, $(VPSS_FILES))

DRV_SRCS     := $(shell echo $(VPSS_FILES) | awk '{for(i=1;i<=NF;i++)print $$i}' | sed "s:\(.*\)/\<vpss\>\(.*\):.\2:g")

DRV_OBJS     := $(patsubst %.c,%.o, $(DRV_SRCS))

obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o

$(warning  $(DRV_SRCS))
$(warning  $(DRV_OBJS))

$(MOD_NAME)-y = $(DRV_OBJS)
BUILD_DIR := $(HI_OUT_DIR)/$(OBJ_NAME)/$(subst $(SDK_DIR)/,,$(CURDIR))

BUILD_DIR_MAKEFILE := $(BUILD_DIR)/Makefile

$(warning  $(HI_OUT_DIR))
$(warning  $(BUILD_DIR))
$(warning  $(HI_KERNEL_TOOLCHAINS_NAME))
$(warning  $(CFG_HI_CPU_ARCH))
$(warning  $(DRV_SRCS))
$(warning  $(MOD_NAME))
$(warning  $(HI_MODULE_DIR))
$(warning  $(CURDIR))

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
