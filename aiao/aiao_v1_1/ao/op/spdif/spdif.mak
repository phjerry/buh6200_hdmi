#===============================================================================
EXTRA_CFLAGS += -I$(DRV_AIAO)/ao/op/spdif
#===============================================================================
ifeq ($(HI_SND_SPDIF_SUPPORT),y)
EXTRA_CFLAGS += -DHI_SND_SPDIF_SUPPORT
$(MOD_NAME)-y += \
    ao/op/spdif/drv_ao_spdif.o \
    ao/op/spdif/spdif_func.o
endif
#===============================================================================