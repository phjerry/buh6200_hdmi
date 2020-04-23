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
MOD_NAME := hi_vou
VO_VER := vdp_v6_0
HAL_TOP_DIR := $(DRV_DIR)/vo/$(VO_VER)/hal
HAL_DIR := $(DRV_DIR)/vo/$(VO_VER)/hal


EXTRA_CFLAGS += -I$(USR_DIR)/include \
                -I$(USR_DIR)/common/include
EXTRA_CFLAGS += -I$(DRV_DIR)/include \
                -I$(DRV_DIR)/common/include
EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)
EXTRA_CFLAGS += -I$(DRV_DIR)/vo/include/hi3796cv300
EXTRA_CFLAGS += -I$(DRV_DIR)/vo/$(VO_VER)/disp
EXTRA_CFLAGS += -I$(DRV_DIR)/vo/$(VO_VER)/win
EXTRA_CFLAGS += -I$(DRV_DIR)/vo/$(VO_VER)/xdp_ctrl
EXTRA_CFLAGS += -I$(DRV_DIR)/vo/$(VO_VER)/com

#hal header
EXTRA_CFLAGS += -I$(HAL_TOP_DIR)/ 	\
		-I$(HAL_TOP_DIR)/ip \
		-I$(HAL_TOP_DIR)/cfg/v1 \
		-I$(HAL_TOP_DIR)/96cv300	\
		-I$(HAL_TOP_DIR)/ip/csc_v1	\
		-I$(HAL_TOP_DIR)/ip/disp_v1 \
		-I$(HAL_TOP_DIR)/ip/fdr_v1	\
		-I$(HAL_TOP_DIR)/ip/vid_v1	\
		-I$(HAL_TOP_DIR)/ip/mmu_v1	\
		-I$(HAL_TOP_DIR)/ip/hdr_v1	\
		-I$(HAL_TOP_DIR)/ip/zme_v1 \
		-I$(HAL_TOP_DIR)/ip/crg_v1 \
		-I$(HAL_TOP_DIR)/ip/coef_v1 \
		-I$(DISP_TOP_DIR)/

EXTRA_CFLAGS += -I$(DRV_DIR)/vo/common
EXTRA_CFLAGS += -D__DISP_PLATFORM_SDK__
EXTRA_CFLAGS += -DVDP_SDK_PQ_SUPPORT
EXTRA_CFLAGS += -DVDP_SDK_HAL_TEST_SUPPORT
#EXTRA_CFLAGS += -DVDP_SUPPORT_PANEL

VO_OBJS := $(VO_VER)/drv_disp_intf.o
VO_OBJS += $(VO_VER)/disp/drv_disp.o
VO_OBJS += $(VO_VER)/disp/drv_display.o
VO_OBJS += $(VO_VER)/disp/drv_disp_isr.o
VO_OBJS += $(VO_VER)/disp/drv_disp_timing.o
VO_OBJS += $(VO_VER)/disp/drv_disp_interface.o
VO_OBJS +=	$(VO_VER)/drv_win_intf.o \
		$(VO_VER)/win/drv_window.o \
		$(VO_VER)/win/drv_win_mng.o \
		$(VO_VER)/win/drv_win_proc.o	\
		$(VO_VER)/win/drv_win.o	\
		$(VO_VER)/win/drv_win_mapping.o	\
		$(VO_VER)/win/drv_win_ai.o				\
		$(VO_VER)/win/drv_win_attrprocess.o		\
		$(VO_VER)/win/drv_win_buffer.o			\
		$(VO_VER)/win/drv_win_fence.o			\
		$(VO_VER)/win/drv_win_frc.o				\
		$(VO_VER)/win/drv_win_vir.o				\
		$(VO_VER)/win/drv_win_vpss.o \
		$(VO_VER)/xdp_ctrl/xdp_ctrl.o \
		$(VO_VER)/../common/drv_xdp_ion.o\
		$(VO_VER)/../common/drv_xdp_debug.o\
		$(VO_VER)/../common/drv_xdp_list.o \
		$(VO_VER)/../common/drv_xdp_osal.o

VO_OBJS +=	$(VO_VER)/hal/hal_disp_intf.o \
		$(VO_VER)/hal/hal_layer_intf.o \
		$(VO_VER)/hal/hal_wbc_intf.o \
		$(VO_VER)/hal/hal_vdp_comm.o

VO_OBJS +=	$(VO_VER)/com/vdp_ext_func.o

HAL_IP_FILE = $(wildcard $(HAL_DIR)/96cv300/*.c)
HAL_IP_FILE += $(wildcard $(HAL_DIR)/cfg/v1/*.c)
HAL_IP_FILE += $(wildcard $(HAL_DIR)/ip/coef_v1/*.c)
HAL_IP_FILE += $(wildcard $(HAL_DIR)/ip/crg_v1/*.c)
HAL_IP_FILE += $(wildcard $(HAL_DIR)/ip/csc_v1/*.c)
HAL_IP_FILE += $(wildcard $(HAL_DIR)/ip/disp_v1/*.c)
HAL_IP_FILE += $(wildcard $(HAL_DIR)/ip/fdr_v1/*.c)
HAL_IP_FILE += $(wildcard $(HAL_DIR)/ip/hdr_v1/*.c)
HAL_IP_FILE += $(wildcard $(HAL_DIR)/ip/mmu_v1/*.c)
HAL_IP_FILE += $(wildcard $(HAL_DIR)/ip/nxg_v1/*.c)
HAL_IP_FILE += $(wildcard $(HAL_DIR)/ip/vid_v1/*.c)
HAL_IP_FILE += $(wildcard $(HAL_DIR)/ip/zme_v1/*.c)
HAL_IP_FILE := $(filter-out %.mod.c, $(HAL_IP_FILE))
IP_FILES := $(shell echo $(HAL_IP_FILE) | awk '{for(i=1;i<=NF;i++)print $$i}' | sed "s:\(.*\)/\<vo\>\(.*\):.\2:g")
HAL_IP_OBJS := $(patsubst %.c,%.o, $(IP_FILES))

VO_OBJS += $(HAL_IP_OBJS)

$(MOD_NAME)-y :=  $(VO_OBJS)

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
