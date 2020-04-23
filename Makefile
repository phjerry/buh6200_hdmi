#=======================================
#   export variable
#=======================================
ifneq ($(srctree),)
    KERNEL_DIR := $(srctree)

    SDK_DIR := $(shell cd $(KERNEL_DIR)/../../../.. && /bin/pwd)
else
    SDK_DIR := $(shell cd $(CURDIR)/../../.. && /bin/pwd)
endif

include $(SDK_DIR)/base.mak

#=======================================
#   local variable
#======================================
ifeq ($(CFG_HI_LOADER_APPLOADER),y)
    objects := common
else ifeq ($(CFG_HI_RECOVERY_SUPPORT),y)
    objects := common gpio pdm pq cipher hdmitx vo tde hifb otp klad ir i2c jpeg
else
    objects := common demux adec adsp aiao aenc vfmw vdec omxvdec venc hdmitx vo sync avplay i2c klad keyslot otp\
               cipher gpio hifb pmoc pq vpss tde ir pdm dbe jpeg
endif

ifeq ($(CFG_ADVCA_NAGRA),y)
    objects += klad/nagra/cert
endif

ifeq ($(HI_DRV_BUILDTYPE),y)
    objects += base
endif

ifeq ($(CFG_HI_FRONTEND_SUPPORT),y)
    objects += frontend
endif

ifeq ($(CFG_HI_KEYLED_SUPPORT),y)
    objects += keyled
endif

ifeq ($(CFG_HI_TEE_SUPPORT),y)
    objects += ssm
endif

ifeq ($(CFG_HI_PVR_SUPPORT),y)
    objects += pvr
endif

ifeq ($(CFG_HI_GFX2D_SUPPORT),y)
    objects += gfx2d
endif

ifeq ($(CFG_HI_SCI_SUPPORT),y)
    objects += sci
endif

ifneq ($(CFG_HI_CPU_DVFS_SUPPORT)_$(CFG_HI_TEMP_CTRL_SUPPORT),_)
    objects += dvfs
endif

ifeq ($(CFG_HI_TSR2RCIPHER_SUPPORT),y)
    objects += tsr2rcipher
endif

ifeq ($(CFG_HI_MIPI_SUPPORT),y)
    objects += mipi_tx
endif

ifeq ($(CFG_HI_PANEL_SUPPORT),y)
    objects += panel
endif

ifeq ($(CFG_HI_VI_SUPPORT),y)
    objects += vi
endif

ifeq ($(CFG_HI_WIFI_SUPPORT),y)
    objects += wifi/drv
endif

ifeq ($(CFG_HI_GPU_SUPPORT),y)
    objects += gpu
endif

ifeq ($(CFG_HI_NPU_SUPPORT),y)
    objects += npu
endif

ifeq ($(CFG_HI_RM_SUPPORT),y)
    objects += rm
endif

ifeq ($(CFG_HI_TSIO_SUPPORT),y)
    objects += tsio
endif

ifeq ($(CFG_HI_LSADC_SUPPORT),y)
    objects += lsadc
endif

ifeq ($(CFG_HI_SPREAD_SUPPORT),y)
    objects += spread
endif

obj-y += $(addsuffix /,$(objects))

