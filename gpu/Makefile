#===============================================================================
#   export variables
#===============================================================================
ifeq ($(CFG_HI_EXPORT_FLAG),)
    ifneq ($(srctree),)
    KERNEL_DIR := $(srctree)

    SDK_DIR := $(shell cd $(KERNEL_DIR)/../../../.. && /bin/pwd)
    else
    SDK_DIR := $(shell cd $(CURDIR)/../../../.. && /bin/pwd)
    endif

    include $(SDK_DIR)/base.mak
endif

ifeq ($(CFG_HI_GPU_MALIG52_SUPPORT),y)
    version := bifrost
else
    $(error error: mali unknown)
endif

ifeq ($(version), bifrost)
    version := bifrost/r16p0-01rel0
endif

obj-y += $(version)/

#=================================================================
#   rules
#=================================================================
.PHONY: all clean

all:
	$(AT)make -C $(version)

clean:
	$(AT)make -C $(version) clean
