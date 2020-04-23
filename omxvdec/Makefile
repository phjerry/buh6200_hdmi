#===============================================================================
# export variables
#===============================================================================

ifneq ($(srctree),)
KERNEL_DIR := $(srctree)
SDK_DIR := $(shell cd $(KERNEL_DIR)/../../../../ && /bin/pwd)
else
SDK_DIR := $(shell cd $(CURDIR)/../../../../ && /bin/pwd)
endif

include $(SDK_DIR)/base.mak


#===============================================================================
# local variables
#===============================================================================
ifneq ($(findstring $(CFG_HI_CHIP_TYPE), hi3796cv300 hi3751v900),)
OMXVDEC_VERSION := omxvdec_v6.0
else
endif

obj-y += $(OMXVDEC_VERSION)/

#===============================================================================
#   rules
#===============================================================================
.PHONY: all clean

all:
	$(AT)make -C $(OMXVDEC_VERSION)/ all

clean:
	$(AT)make -C $(OMXVDEC_VERSION)/ clean

