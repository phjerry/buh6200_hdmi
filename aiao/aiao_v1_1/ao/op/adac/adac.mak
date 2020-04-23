#===============================================================================
EXTRA_CFLAGS += -I$(DRV_AIAO)/ao/op/adac
#===============================================================================
ifeq ($(HI_SND_ADAC_SUPPORT),y)
EXTRA_CFLAGS += -DHI_SND_ADAC_SUPPORT
$(MOD_NAME)-y += \
    ao/op/adac/drv_ao_adac.o \
    ao/op/adac/adac_func.o
endif
#===============================================================================