#===============================================================================
# AI SUPPORT is optional
ifeq ($(CFG_HI_AI_SUPPORT), y)
EXTRA_CFLAGS += -DHI_AUDIO_AI_SUPPORT
EXTRA_CFLAGS += -I$(DRV_AIAO)/ai
$(MOD_NAME)-y += \
    ai/drv_ai_intf.o \
    ai/drv_ai.o
endif
#===============================================================================