#===============================================================================
# local variables
#===============================================================================
VFMW_SUB_DIR = vfmw_v6.2

obj-y += $(VFMW_SUB_DIR)/

#===============================================================================
#   rules
#===============================================================================
.PHONY: all clean

all:
	$(AT)make -C $(VFMW_SUB_DIR)/ all

vmdc:
	$(AT)make -C $(VFMW_SUB_DIR)/ vmdc

clean:
	$(AT)make -C $(VFMW_SUB_DIR)/ clean

vmdc_clean:
	$(AT)make -C $(VFMW_SUB_DIR)/ vmdc_clean
