###############################################################################
# export variables
###############################################################################
ifneq ($(srctree),)
    KERNEL_DIR := $(srctree)

    SDK_DIR := $(shell cd $(KERNEL_DIR)/../../../.. && /bin/pwd)
else
    SDK_DIR := $(shell cd $(CURDIR)/../../../.. && /bin/pwd)
endif

include $(SDK_DIR)/base.mak

###############################################################################
# local variables
###############################################################################

FRONTEND_DIR := $(DRV_DIR)/frontend

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

## header search path

EXTRA_CFLAGS += -I$(DRV_DIR)/include \
                -I$(DRV_DIR)/common/include \
                -I$(FRONTEND_DIR) \
                -I$(FRONTEND_DIR)/common/math \
                -I$(FRONTEND_DIR)/common/i2c \
                -I$(FRONTEND_DIR)/common/time

EXTRA_CFLAGS += -DLOG_MODULE_ID=HI_ID_FRONTEND

#################### base #####################

MOD_NAME := hi_frontend

$(MOD_NAME)-y := drv_frontend_intf.o
$(MOD_NAME)-y += drv_frontend_ctrl.o

ifeq ($(CFG_HI_PROC_SUPPORT),y)
    $(MOD_NAME)-y += drv_frontend_proc.o
endif

$(MOD_NAME)-y += common/math/drv_fe_math.o \
                 common/i2c/drv_fe_i2c.o \
                 common/time/drv_fe_time.o

############### demod type config #############
ifeq ($(CFG_HI_DEMOD_TYPE_CXD2878_CAB),y)
    CFG_HI_DEMOD_TYPE_CXD2878 = y
endif
ifeq ($(CFG_HI_DEMOD_TYPE_CXD2878_TER),y)
    CFG_HI_DEMOD_TYPE_CXD2878 = y
endif
ifeq ($(CFG_HI_DEMOD_TYPE_CXD2878_SAT),y)
    CFG_HI_DEMOD_TYPE_CXD2878 = y
endif
ifeq ($(CFG_HI_DEMOD_TYPE_CXD2878),y)
    EXTRA_CFLAGS += -I$(FRONTEND_DIR)/demod/cxd2878 \
                    -I$(FRONTEND_DIR)/demod/cxd2878/i2c \
                    -I$(FRONTEND_DIR)/demod/cxd2878/i2c/hi_sony_i2c \
                    -I$(FRONTEND_DIR)/demod/cxd2878/dvb_terr \
                    -I$(FRONTEND_DIR)/demod/cxd2878/dvb_cable \
                    -I$(FRONTEND_DIR)/demod/cxd2878/dvb_sat \
                    -I$(FRONTEND_DIR)/demod/cxd2878/sat_device_ctrl \
                    -I$(FRONTEND_DIR)/demod/cxd2878/sat_device_ctrl/lnb_controller \
                    -I$(FRONTEND_DIR)/demod/cxd2878/atsc \
                    -I$(FRONTEND_DIR)/demod/cxd2878/isdb_terr \
                    -I$(FRONTEND_DIR)/demod/cxd2878/isdb_cable \
                    -I$(FRONTEND_DIR)/demod/cxd2878/isdb_sat \
                    -I$(FRONTEND_DIR)/demod/cxd2878/j83b \
                    -I$(FRONTEND_DIR)/demod/cxd2878/tuner \
                    -I$(FRONTEND_DIR)/demod/cxd2878/tuner/terr_cable_ascot3 \
                    -I$(FRONTEND_DIR)/demod/cxd2878/tuner/terr_cable_ascot3/refcode \
                    -I$(FRONTEND_DIR)/demod/cxd2878/lib
    EXTRA_CFLAGS += -DSONY_DRIVER_BUILD_OPTION_CXD2878
    EXTRA_CFLAGS += -DDEMOD_DEV_TYPE_CXD2878
    CXD2878_DIR = demod/cxd2878
    $(MOD_NAME)-y += $(CXD2878_DIR)/cxd2878.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/sony_stopwatch_port.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/lib/sony_math.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/sony_demod.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/sony_integ.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_sat/sony_integ_dvbs_s2.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_sat/sony_demod_dvbs_s2_monitor.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_sat/sony_demod_dvbs_s2.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/isdb_terr/sony_demod_isdbt.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/isdb_terr/sony_integ_isdbt.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/isdb_terr/sony_demod_isdbt_monitor.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_terr/sony_integ_dvbt2.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_terr/sony_demod_dvbt2.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_terr/sony_demod_dvbt.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_terr/sony_integ_dvbt.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_terr/sony_demod_dvbt2_monitor.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_terr/sony_integ_dvbt_t2.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_terr/sony_demod_dvbt_monitor.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/atsc/sony_demod_atsc.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/atsc/sony_demod_atsc_monitor.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/atsc/sony_demod_atsc3.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/atsc/sony_demod_atsc3_monitor.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/atsc/sony_demod_atsc3_chbond.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/isdb_sat/sony_integ_isdbs.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/isdb_sat/sony_demod_isdbs.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/isdb_sat/sony_demod_isdbs_monitor.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/isdb_sat/sony_demod_isdbs3.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/isdb_sat/sony_integ_isdbs3.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/isdb_sat/sony_demod_isdbs3_monitor.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/i2c/sony_i2c.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/i2c/hi_sony_i2c/hi_sony_i2c.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/j83b/sony_demod_j83b.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/j83b/sony_integ_j83b.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/j83b/sony_demod_j83b_monitor.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/sony_common.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_cable/sony_demod_dvbc2_monitor.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_cable/sony_integ_dvbc2.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_cable/sony_demod_dvbc_monitor.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_cable/sony_demod_dvbc2.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_cable/sony_integ_dvbc.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/dvb_cable/sony_demod_dvbc.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/isdb_cable/sony_demod_isdbc_monitor.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/isdb_cable/sony_integ_isdbc.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/isdb_cable/sony_demod_isdbc.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/isdb_cable/sony_demod_isdbc_chbond.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/tuner/terr_cable_ascot3/sony_tuner_ascot3.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/tuner/terr_cable_ascot3/refcode/sony_ascot3.o
    $(MOD_NAME)-y += $(CXD2878_DIR)/sat_device_ctrl/sony_demod_sat_device_ctrl.o
endif
ifeq ($(CFG_HI_DEMOD_TYPE_MXL541),y)
endif
ifeq ($(CFG_HI_DEMOD_TYPE_MXL254),y)
    CFG_HI_DEMOD_MXL254_MXL214 := y
endif
ifeq ($(CFG_HI_DEMOD_TYPE_MXL214),y)
    CFG_HI_DEMOD_MXL254_MXL214 := y
endif
ifeq ($(CFG_HI_DEMOD_MXL254_MXL214),y)
    EXTRA_CFLAGS += -DDEMOD_DEV_TYPE_MXL254
    EXTRA_CFLAGS += -I$(FRONTEND_DIR)/demod/mxl254
    $(MOD_NAME)-y  += demod/mxl254/mxl254.o
    $(MOD_NAME)-y  += demod/mxl254/MxL_HRCLS_CalCtrl.o
    $(MOD_NAME)-y  += demod/mxl254/MxL_HRCLS_CommonApi.o
    $(MOD_NAME)-y  += demod/mxl254/MxL_HRCLS_DebugApi.o
    $(MOD_NAME)-y  += demod/mxl254/MxL_HRCLS_DemodApi.o
    $(MOD_NAME)-y  += demod/mxl254/MxL_HRCLS_OEM_Drv.o
    $(MOD_NAME)-y  += demod/mxl254/MxL_HRCLS_OobApi.o
    $(MOD_NAME)-y  += demod/mxl254/MxL_HRCLS_PhyCtrl.o
    $(MOD_NAME)-y  += demod/mxl254/MxL_HRCLS_SerdesApi.o
    $(MOD_NAME)-y  += demod/mxl254/MxL_HRCLS_XptApi.o
endif
############### tuner type config #############
ifeq ($(CFG_HI_TUNER_TYPE_CXD2871),y)
    EXTRA_CFLAGS += -DTUNER_DEV_TYPE_CXD2871
endif
ifeq ($(CFG_HI_TUNER_TYPE_SUT_PJ987),y)
    EXTRA_CFLAGS += -DTUNER_DEV_TYPE_SUT_PJ987
    EXTRA_CFLAGS += -I$(FRONTEND_DIR)/tuner/sut_pj987
    $(MOD_NAME)-y += tuner/sut_pj987/sut_pj987_isdbs_v1000.o
    $(MOD_NAME)-y += tuner/sut_pj987/sut_pj987_isdbt_v1000.o
    $(MOD_NAME)-y += tuner/sut_pj987/sut_pj987.o
    $(MOD_NAME)-y += tuner/sut_pj987/sut_pj987_i2c.o
endif

################ lnb type config ##############
$(MOD_NAME)-y += lnb/drv_lnbctrl.o
EXTRA_CFLAGS += -I$(FRONTEND_DIR)/lnb

ifeq ($(CFG_HI_LNB_CTRL_LNBH30),y)
    $(MOD_NAME)-y += lnb/lnbh30.o
    EXTRA_CFLAGS += -DLNB_CTRL_LNBH30
endif

ifeq ($(CFG_HI_LNB_CTRL_A8300),y)
    $(MOD_NAME)-y += lnb/a8300.o
    EXTRA_CFLAGS += -DLNB_CTRL_A8300
endif

###############################################################################
obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o

BUILD_DIR := $(HI_OUT_DIR)/$(OBJ_NAME)/$(subst $(SDK_DIR)/,,$(CURDIR))

BUILD_DIR_MAKEFILE := $(BUILD_DIR)/Makefile

#===============================================================================
#   rules
#===============================================================================
.PHONY: all clean

all: $(BUILD_DIR_MAKEFILE)
	$(AT)make -C $(LINUX_DIR) ARCH=$(CFG_HI_CPU_ARCH) CROSS_COMPILE=$(HI_KERNEL_TOOLCHAINS_NAME)- M=$(BUILD_DIR) src=$(CURDIR) modules
	$(AT)$(HI_KERNEL_TOOLCHAINS_NAME)-strip --strip-unneeded $(BUILD_DIR)/$(MOD_NAME).ko
	$(AT)test -d $(HI_MODULE_DIR) || mkdir -p $(HI_MODULE_DIR)
	$(AT)cp -f $(BUILD_DIR)/$(MOD_NAME).ko $(HI_MODULE_DIR)/

clean:
	$(AT)make -C $(LINUX_DIR) ARCH=$(CFG_HI_CPU_ARCH) CROSS_COMPILE=$(HI_KERNEL_TOOLCHAINS_NAME)- M=$(BUILD_DIR) src=$(CURDIR) clean
	$(AT)rm -rf $(HI_MODULE_DIR)/$(MOD_NAME).ko

$(BUILD_DIR_MAKEFILE): $(BUILD_DIR)
	$(AT)touch $@

$(BUILD_DIR):
	$(AT)mkdir -p $@
