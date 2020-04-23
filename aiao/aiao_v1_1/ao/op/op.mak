#===============================================================================
EXTRA_CFLAGS += -I$(DRV_AIAO)/ao/op
#===============================================================================
$(MOD_NAME)-y += ao/op/drv_ao_op.o
#===============================================================================
include $(DRV_AIAO)/ao/op/cast/cast.mak
include $(DRV_AIAO)/ao/op/hdmi/hdmi.mak
include $(DRV_AIAO)/ao/op/spdif/spdif.mak
include $(DRV_AIAO)/ao/op/adac/adac.mak
include $(DRV_AIAO)/ao/op/arc/arc.mak
include $(DRV_AIAO)/ao/op/i2s/i2s.mak
#===============================================================================