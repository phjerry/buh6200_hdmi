#===============================================================================
EXTRA_CFLAGS += -DHI_SND_ADVANCED_SUPPORT
#===============================================================================
HI_SND_AEF_SUPPORT = y
HI_SND_AVC_SUPPORT = y
HI_SND_DRC_SUPPORT = y
# HI_SND_GEQ_SUPPORT = y
HI_SND_PEQ_SUPPORT = y
#===============================================================================
HI_SND_CAST_SUPPORT = y
HI_SND_HDMI_SUPPORT = y
HI_SND_SPDIF_SUPPORT = y
HI_SND_ADAC_SUPPORT = y
HI_SND_I2S_SUPPORT = y
HI_SND_AR_SUPPORT = y
# HI_SND_ARC_SUPPORT = y
#===============================================================================
include $(DRV_AIAO)/ao/aef/aef.mak
include $(DRV_AIAO)/ao/aef/avc/avc.mak
include $(DRV_AIAO)/ao/aef/drc/drc.mak
include $(DRV_AIAO)/ao/aef/geq/geq.mak
include $(DRV_AIAO)/ao/aef/peq/peq.mak
include $(DRV_AIAO)/ao/op/op.mak
include $(DRV_AIAO)/ao/track/track.mak
include $(DRV_AIAO)/ao/ar/ar.mak
include $(DRV_AIAO)/ao/proc/proc.mak
#===============================================================================