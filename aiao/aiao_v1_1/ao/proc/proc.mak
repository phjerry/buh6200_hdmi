#===============================================================================
EXTRA_CFLAGS += -I$(DRV_AIAO)/ao/proc
#===============================================================================
ifeq ($(CFG_HI_PROC_SUPPORT),y)
$(MOD_NAME)-y += ao/proc/drv_ao_proc.o
endif
#===============================================================================