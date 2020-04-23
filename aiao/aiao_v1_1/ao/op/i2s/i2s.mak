#===============================================================================
EXTRA_CFLAGS += -I$(DRV_AIAO)/ao/op/i2s
#===============================================================================
ifeq ($(HI_SND_I2S_SUPPORT),y)
EXTRA_CFLAGS += -DHI_SND_I2S_SUPPORT
$(MOD_NAME)-y += \
    ao/op/i2s/drv_ao_i2s.o \
    ao/op/i2s/i2s_func.o
endif
#===============================================================================