#===============================================================================
EXTRA_CFLAGS += -I$(DRV_AIAO)/ao/aef
#===============================================================================
ifeq ($(HI_SND_AEF_SUPPORT),y)
EXTRA_CFLAGS += -DHI_SND_AEF_SUPPORT
$(MOD_NAME)-y += \
    ao/aef/drv_ao_aef.o \
    ao/aef/aef_func.o
endif
#===============================================================================