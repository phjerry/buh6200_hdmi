#===============================================================================
EXTRA_CFLAGS += -I$(DRV_AIAO)/ao/aef/peq
#===============================================================================
ifeq ($(HI_SND_AEF_SUPPORT), y)
ifeq ($(HI_SND_PEQ_SUPPORT),y)
EXTRA_CFLAGS += -DHI_SND_PEQ_SUPPORT
$(MOD_NAME)-y += \
    ao/aef/peq/drv_ao_peq.o \
    ao/aef/peq/peq_func.o
endif
endif
#===============================================================================