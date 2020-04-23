#===============================================================================
EXTRA_CFLAGS += -I$(DRV_AIAO)/ao/op/cast
#===============================================================================
ifeq ($(HI_SND_CAST_SUPPORT),y)
EXTRA_CFLAGS += -DHI_SND_CAST_SUPPORT
$(MOD_NAME)-y += \
    ao/op/cast/drv_ao_cast.o \
    ao/op/cast/cast_func.o \
    ao/op/cast/hal_cast.o
else
ifeq ($(CFG_HI_PROC_SUPPORT),y)
$(MOD_NAME)-y += \
    ao/op/cast/drv_ao_cast.o \
    ao/op/cast/cast_func.o \
    ao/op/cast/hal_cast.o
endif
endif
#===============================================================================