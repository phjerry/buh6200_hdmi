#===============================================================================
EXTRA_CFLAGS += -I$(DRV_AIAO)/ao/aef/drc
#===============================================================================
ifeq ($(HI_SND_AEF_SUPPORT), y)
ifeq ($(HI_SND_DRC_SUPPORT), y)
EXTRA_CFLAGS += -DHI_SND_DRC_SUPPORT
$(MOD_NAME)-y += \
    ao/aef/drc/drv_ao_drc.o \
    ao/aef/drc/drc_func.o
endif
endif
#===============================================================================