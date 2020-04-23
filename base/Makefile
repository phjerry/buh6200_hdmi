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

#===============================================================================
# local variables
#===============================================================================
EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

EXTRA_CFLAGS += -I$(DRV_DIR)/include
EXTRA_CFLAGS += -I$(DRV_DIR)/adsp/adsp_v1_1/include
EXTRA_CFLAGS += -I$(DRV_DIR)/common/include

ifeq ($(CFG_HI_TEE_SUPPORT), y)
EXTRA_CFLAGS += -DHI_TEE_SUPPORT
endif

ifeq ($(CFG_HI_TSR2RCIPHER_SUPPORT), y)
EXTRA_CFLAGS += -I$(DRV_DIR)/tsr2rcipher
endif

ifeq ($(CFG_HI_RECOVERY_SUPPORT),y)
EXTRA_CFLAGS += -DHI_RECOVERY_SUPPORT
endif

ifeq ($(CFG_HI_JPGE_SUPPORT),y)
EXTRA_CFLAGS += -DHI_JPGE_SUPPORT
endif

ifeq ($(CFG_HI_CPU_DVFS_SUPPORT),y)
EXTRA_CFLAGS += -DHI_CPU_DVFS_SUPPORT
endif
ifeq ($(CFG_HI_TEMP_CTRL_SUPPORT),y)
EXTRA_CFLAGS += -DHI_TEMP_CTRL_SUPPORT
endif

ifeq ($(CFG_HI_FRONTEND_SUPPORT),y)
EXTRA_CFLAGS += -DHI_FRONTEND_SUPPORT
endif

MOD_NAME := hi_base

obj-y += $(MOD_NAME).o

$(MOD_NAME)-y   :=  hi_init.o
