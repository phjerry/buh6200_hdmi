#===============================================================================
EXTRA_CFLAGS += -I$(DRV_AIAO)/ao/op/hdmi
#===============================================================================
ifeq ($(HI_SND_HDMI_SUPPORT),y)
EXTRA_CFLAGS += -DHI_SND_HDMI_SUPPORT
$(MOD_NAME)-y += \
    ao/op/hdmi/drv_ao_hdmi.o \
    ao/op/hdmi/hdmi_func.o \
    ao/op/hdmi/hdmi_osal.o \
    ao/op/hdmi/hdmi_ch_status.o
endif
#===============================================================================