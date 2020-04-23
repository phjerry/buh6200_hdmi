#===============================================================================
EXTRA_CFLAGS += -I$(DRV_AIAO)/ao/aef/avc
#===============================================================================
ifeq ($(HI_SND_AEF_SUPPORT), y)
ifeq ($(HI_SND_AVC_SUPPORT), y)
EXTRA_CFLAGS += -DHI_SND_AVC_SUPPORT
$(MOD_NAME)-y += ao/aef/avc/drv_ao_avc.o
endif
endif
#===============================================================================