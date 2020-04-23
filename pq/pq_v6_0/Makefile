###############################################################################
# export variables
###############################################################################
ifneq ($(srctree),)
    KERNEL_DIR := $(srctree)

    SDK_DIR := $(shell cd $(KERNEL_DIR)/../../../.. && /bin/pwd)
    else
    SDK_DIR := $(shell cd $(CURDIR)/../../../../.. && /bin/pwd)
    endif

include $(SDK_DIR)/base.mak

#===============================================================================
# local variables
#===============================================================================
VERSION := pq_v6_0
MOD_NAME := hi_pq

HAL_DIR := hal
MNG_DIR := mng
COMM_DIR := common

CFG_HI_HDR_GOLDEN_SUPPORT := n
CFG_HI_PQ_SOFTWARE_ALG_SUPPORT := y
CFG_HI_PQ_PDM_PARA_SUPPORT := n
CFG_HI_PQ_96CV300_ES_SUPPORT := y
CFG_HI_PQ_DPT_V900_SUPPORT := n

obj-$(HI_DRV_BUILDTYPE) += $(MOD_NAME).o

VO_REG_DIR := $(DRV_DIR)/vo/include/
VPSS_REG_DIR := $(DRV_DIR)/vpss/reg_inc/

ifeq ($(CFG_HI_CHIP_TYPE), hi3796cv300)
CHIP := 3796cv300
module :=  comm table gfxcsc gfxzme csc gfxhdr hdr vpsshdr zme tnr snr sr cle db vpsszme acm deshoot dering sharpen dei fmd dm dci
endif

ifeq ($(CFG_HI_CHIP_TYPE), hi3751v900)
CHIP := 3796cv300
module :=  comm table gfxcsc gfxzme csc gfxhdr hdr vpsshdr zme tnr snr sr cle db vpsszme acm deshoot dering sharpen dei fmd dm dci
endif

EXTRA_CFLAGS += $(CFG_HI_KMOD_CFLAGS)

EXTRA_CFLAGS += -I$(USR_DIR)/include                    \
                -I$(USR_DIR)/msp/include                 \
                -I$(USR_DIR)/common/include              \
                -I$(DRV_DIR)/common/include              \
                -I$(DRV_DIR)/include                    \
                -I$(VO_REG_DIR)                       \
                -I$(VPSS_REG_DIR)                       \
                -I$(DRV_DIR)/pq/$(VERSION)/common      \
                -I$(DRV_DIR)/pq/$(VERSION)/mng      \
                -I$(DRV_DIR)/pq/$(VERSION)/mng/$(CHIP) \
                -I$(DRV_DIR)/pq/$(VERSION)/hal/$(CHIP) \
                -I$(DRV_DIR)/pq/$(VERSION)/hal/common  \
                -I$(DRV_DIR)/pq/$(VERSION)

ifdef CFG_HI_SMMU_SUPPORT
EXTRA_CFLAGS += -DHI_PQ_SMMU_SUPPORT
endif

ifeq ($(CFG_HI_TEE_SUPPORT),y)
EXTRA_CFLAGS += -DHI_PQ_TEE_SUPPORT
endif

ifeq ($(CFG_HI_PROC_SUPPORT),y)
EXTRA_CFLAGS += -DHI_PQ_PROC_SUPPORT
endif

ifeq ($(CFG_HI_HDR_GOLDEN_SUPPORT),y)
EXTRA_CFLAGS += -DPQ_HDR_COEF_GOLDEN
endif

ifeq ($(CFG_HI_PQ_SOFTWARE_ALG_SUPPORT),y)
EXTRA_CFLAGS += -DPQ_SOFTWARE_ALG_SUPPORT
endif

ifeq ($(CFG_HI_PQ_PDM_PARA_SUPPORT),y)
EXTRA_CFLAGS += -DPQ_PDM_SUPPORT
endif

ifeq ($(CFG_HI_PQ_96CV300_ES_SUPPORT),y)
EXTRA_CFLAGS += -DPQ_96CV300_ES_SUPPORT
else
EXTRA_CFLAGS += -DPQ_96CV300_CS_SUPPORT
endif

ifeq ($(CFG_HI_PQ_DPT_V900_SUPPORT),y)
EXTRA_CFLAGS += -DPQ_DPT_V900_SUPPORT
endif

$(MOD_NAME)-y +=    drv_pq.o                        \
                    drv_pq_intf.o                   \
                    $(COMM_DIR)/drv_pq_comm.o     \
                    $(COMM_DIR)/drv_pq_mem.o     \
                    drv_pq_table.o                  \
                    drv_pq_proc.o                   \
                    drv_pq_vpss_thread.o            \
                    $(HAL_DIR)/common/pq_hal_comm.o \
                    drv_pq_vdp_thread.o              \
                    $(HAL_DIR)/common/hal_pq_ip_coef.o

ifneq ($(findstring table, $(module)),)
$(MOD_NAME)-y +=  $(HAL_DIR)/$(CHIP)/pq_hal_table_default.o
EXTRA_CFLAGS += -DPQ_ALG_TABLE
endif

ifneq ($(findstring zme, $(module)),)
$(MOD_NAME)-y +=    $(MNG_DIR)/pq_mng_zme.o
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_zme.o
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_zme_set_coef.o
EXTRA_CFLAGS += -DPQ_ALG_ZME
endif

ifneq ($(findstring vpsszme, $(module)),)
$(MOD_NAME)-y +=    $(MNG_DIR)/pq_mng_vpsszme.o
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_vpsszme.o
EXTRA_CFLAGS += -DPQ_ALG_VPSSZME
endif

ifneq ($(findstring gfxcsc, $(module)),)
$(MOD_NAME)-y +=    $(MNG_DIR)/pq_mng_gfxcsc.o
EXTRA_CFLAGS += -DPQ_ALG_GFXCSC
endif

ifneq ($(findstring  gfxzme, $(module)),)
$(MOD_NAME)-y +=    $(MNG_DIR)/$(CHIP)/pq_mng_gfxzme.o
EXTRA_CFLAGS += -DPQ_ALG_GFXZME
endif

ifneq ($(findstring  csc, $(module)),)
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_csc.o
$(MOD_NAME)-y +=    $(MNG_DIR)/pq_mng_csc.o
EXTRA_CFLAGS += -DPQ_ALG_CSC
endif

ifneq ($(findstring gfxhdr, $(module)),)
$(MOD_NAME)-y += $(HAL_DIR)/$(CHIP)/pq_hal_hdrv2_regset_comm.o
$(MOD_NAME)-y += $(HAL_DIR)/$(CHIP)/pq_hal_gfxhdr_regset.o
$(MOD_NAME)-y += $(HAL_DIR)/$(CHIP)/pq_hal_gfxhdr.o
$(MOD_NAME)-y += $(MNG_DIR)/pq_mng_gfxhdr.o
EXTRA_CFLAGS += -DPQ_ALG_GFXHDR
endif

ifneq ($(findstring vpsshdr, $(module)),)
$(MOD_NAME)-y += $(HAL_DIR)/$(CHIP)/pq_hal_hdrv2_regset_comm.o
$(MOD_NAME)-y += $(HAL_DIR)/$(CHIP)/pq_hal_vpsshdr_regset.o
$(MOD_NAME)-y += $(HAL_DIR)/$(CHIP)/pq_hal_vpsshdr.o
$(MOD_NAME)-y += $(MNG_DIR)/pq_mng_vpsshdr.o
EXTRA_CFLAGS += -DPQ_ALG_VPSSHDR
endif

ifneq ($(findstring  hdr, $(module)),)
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_hdrv2_regset_comm.o
$(MOD_NAME)-y +=    $(MNG_DIR)/$(CHIP)/pq_mng_hdr.o
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_hdr.o
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_hdr_regset.o
EXTRA_CFLAGS += -DPQ_ALG_HDR
endif

ifneq ($(findstring cle, $(module)),)
$(MOD_NAME)-y += $(HAL_DIR)/$(CHIP)/pq_hal_cle_regset.o
$(MOD_NAME)-y += $(HAL_DIR)/$(CHIP)/pq_hal_cle.o
$(MOD_NAME)-y += $(MNG_DIR)/$(CHIP)/pq_mng_cle.o
EXTRA_CFLAGS += -DPQ_ALG_CLE
endif

ifneq ($(findstring  tnr, $(module)),)
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_tnr.o
$(MOD_NAME)-y +=    $(MNG_DIR)/$(CHIP)/pq_mng_tnr.o
EXTRA_CFLAGS += -DPQ_ALG_TNR
endif

ifneq ($(findstring  snr, $(module)),)
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_snr.o
$(MOD_NAME)-y +=    $(MNG_DIR)/pq_mng_snr.o
EXTRA_CFLAGS += -DPQ_ALG_SNR
endif

ifneq ($(findstring sr, $(module)),)
$(MOD_NAME)-y += $(HAL_DIR)/$(CHIP)/pq_hal_sr_regset.o
$(MOD_NAME)-y += $(HAL_DIR)/$(CHIP)/pq_hal_sr.o
$(MOD_NAME)-y += $(MNG_DIR)/$(CHIP)/pq_mng_sr.o
EXTRA_CFLAGS += -DPQ_ALG_SR
endif

ifneq ($(findstring dei, $(module)),)
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_dei.o
$(MOD_NAME)-y +=    $(MNG_DIR)/$(CHIP)/pq_mng_dei.o
EXTRA_CFLAGS += -DPQ_ALG_DEI
endif

ifneq ($(findstring  fmd, $(module)),)
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_fmd.o
$(MOD_NAME)-y +=    $(MNG_DIR)/pq_mng_ifmd.o
$(MOD_NAME)-y +=    $(MNG_DIR)/pq_mng_fod_alg.o
$(MOD_NAME)-y +=    $(MNG_DIR)/pq_mng_ifmd_alg.o
EXTRA_CFLAGS += -DPQ_ALG_FMD
endif

ifneq ($(findstring  acm, $(module)),)
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_acm.o
$(MOD_NAME)-y +=    $(MNG_DIR)/$(CHIP)/pq_mng_acm.o
EXTRA_CFLAGS += -DPQ_ALG_ACM
endif

ifneq ($(findstring  db, $(module)),)
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_db.o
$(MOD_NAME)-y +=    $(MNG_DIR)/$(CHIP)/pq_mng_db.o
EXTRA_CFLAGS += -DPQ_ALG_DB
endif

ifneq ($(findstring dering, $(module)),)
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_dering.o
$(MOD_NAME)-y +=    $(MNG_DIR)/pq_mng_dering.o
EXTRA_CFLAGS += -DPQ_ALG_DERING
endif

ifneq ($(findstring deshoot, $(module)),)
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_deshoot.o
$(MOD_NAME)-y +=    $(MNG_DIR)/pq_mng_deshoot.o
EXTRA_CFLAGS += -DPQ_ALG_DESHOOT
endif

ifneq ($(findstring  dm, $(module)),)
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_dm.o
$(MOD_NAME)-y +=    $(MNG_DIR)/$(CHIP)/pq_mng_dm.o
EXTRA_CFLAGS += -DPQ_ALG_DM
endif

ifeq ($(CFG_HI_HDR_GOLDEN_SUPPORT),y)
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_hdr_cfg_golden.o
endif

ifneq ($(findstring sharpen, $(module)),)
$(MOD_NAME)-y += $(HAL_DIR)/$(CHIP)/pq_hal_sharpen_regset.o
$(MOD_NAME)-y += $(HAL_DIR)/$(CHIP)/pq_hal_sharpen.o
$(MOD_NAME)-y += $(MNG_DIR)/pq_mng_sharpen.o
EXTRA_CFLAGS += -DPQ_ALG_SHARPEN
endif

ifneq ($(findstring  dci, $(module)),)
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_dci_regset.o
$(MOD_NAME)-y +=    $(HAL_DIR)/$(CHIP)/pq_hal_dci.o
$(MOD_NAME)-y +=    $(MNG_DIR)/pq_mng_dci.o
EXTRA_CFLAGS += -DPQ_ALG_DCI
endif

#EXTRA_CFLAGS += -DHI_PQ_PROC_SUPPORT

BUILD_DIR := $(HI_OUT_DIR)/$(OBJ_NAME)/$(subst $(SDK_DIR)/,,$(CURDIR))

BUILD_DIR_MAKEFILE := $(BUILD_DIR)/Makefile

#===============================================================================
#   rules
#===============================================================================
.PHONY: all clean

all: $(BUILD_DIR_MAKEFILE)
	$(AT)make -C $(LINUX_DIR) ARCH=$(CFG_HI_CPU_ARCH) CROSS_COMPILE=$(HI_KERNEL_TOOLCHAINS_NAME)- M=$(BUILD_DIR) src=$(CURDIR) modules
	$(AT)test -d $(HI_MODULE_DIR) || mkdir -p $(HI_MODULE_DIR)
	$(AT)cp -f $(BUILD_DIR)/$(MOD_NAME).ko $(HI_MODULE_DIR)/

clean:
	$(AT)make -C $(LINUX_DIR) ARCH=$(CFG_HI_CPU_ARCH) CROSS_COMPILE=$(HI_KERNEL_TOOLCHAINS_NAME)- M=$(BUILD_DIR) src=$(CURDIR) clean
	$(AT)rm -rf $(HI_MODULE_DIR)/$(MOD_NAME).ko

$(BUILD_DIR_MAKEFILE): $(BUILD_DIR)
	$(AT)touch $@

$(BUILD_DIR):
	$(AT)mkdir -p $@
