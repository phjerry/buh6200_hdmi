#===============================================================================
# local variables
#===============================================================================
VFMW_MAKE_DIR = product/make_stb

obj-y += $(VFMW_MAKE_DIR)/

#===============================================================================
#   rules
#===============================================================================
.PHONY: all clean

all:
	$(AT)make -C $(VFMW_MAKE_DIR)/ all

vmdc:
	$(AT)make -C $(VFMW_MAKE_DIR)/ vmdc

clean:
	$(AT)make -C $(VFMW_MAKE_DIR)/ clean

vmdc_clean:
	$(AT)make -C $(VFMW_MAKE_DIR)/ vmdc_clean
