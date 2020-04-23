#===============================================================================
EXTRA_CFLAGS += -I$(DRV_AIAO)/ao/aef/geq
#===============================================================================
ifeq ($(HI_SND_AEF_SUPPORT), y)
ifeq ($(HI_SND_GEQ_SUPPORT),y)
EXTRA_CFLAGS += -DHI_SND_GEQ_SUPPORT
$(MOD_NAME)-y += ao/aef/geq/drv_ao_geq.o
endif
endif
#===============================================================================