#===============================================================================
# ALSA SUPPORT is optional
ifeq ($(CFG_HI_SND_ALSA_AO_SUPPORT),y)
ALSA_DIR := alsa/dsp_mix_v2
EXTRA_CFLAGS += -DHI_ALSA_AO_SUPPORT
EXTRA_CFLAGS += -DHI_ALSA_HRTIMER_SUPPORT
EXTRA_CFLAGS += -I$(DRV_AIAO)/$(ALSA_DIR)
$(MOD_NAME)-y += \
    $(ALSA_DIR)/hiaiao_card.o \
    $(ALSA_DIR)/hiaiao_dma.o \
    $(ALSA_DIR)/hiaiao_codec.o \
    $(ALSA_DIR)/hiaiao_i2s.o \
    $(ALSA_DIR)/alsa_aiao_proc_func.o \
    $(ALSA_DIR)/drv_ao_func.o \
    $(ALSA_DIR)/alsa_aiao_volume_func.o
endif
#===============================================================================
ifeq ($(CFG_HI_SND_ALSA_AI_SUPPORT),y)
ALSA_DIR := alsa/dsp_mix_v2
EXTRA_CFLAGS += -DHI_ALSA_AI_SUPPORT
$(MOD_NAME)-y += $(ALSA_DIR)/drv_ai_func.o
endif
#===============================================================================