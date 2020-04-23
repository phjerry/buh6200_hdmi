#===============================================================================
# export variable
#===============================================================================
ifeq ($(CFG_HI_EXPORT_FLAG),)
SDK_DIR ?= $(shell cd $(CURDIR)/../../../../../../.. && /bin/pwd)

include $(SDK_DIR)/base.mak
endif

#===============================================================================
# local variable
#===============================================================================
CFG_HI_CFLAGS += -fstack-protector --param ssp-buffer-size=4
CFG_HI_CFLAGS += -Wformat -Wformat-security -g0 -s
LDFLAGS += -Wl,-z,relro
LDFLAGS += -Wl,-z,noexecstack
CFLAGS += $(CFG_HI_CFLAGS)
CFLAGS += -I$(SDK_DIR)/source/linux/drv/include
CFLAGS += -I$(CURDIR)/../util
CFLAGS += -I$(CURDIR)/../../include
CFLAGS += -I$(SDK_DIR)/source/linux/api/securec
CFLAGS += -DDEBUG
ifeq ($(CFG_HI_WIFI_DEVICE_AP6356S),y)
CFLAGS += -DWIFI_DEVICE_AP6356S
endif
ifeq ($(CFG_HI_WIFI_DEVICE_RTL8822BS),y)
CFLAGS += -DWIFI_DEVICE_RTL8822BS
endif

SRCS := hi_wlan_sta.c \
        hi_wlan_p2p.c \
        wlan_sm.c \
        wlan_hal.c \
        ../util/wlan_util.c

OBJS := $(SRCS:%.c=%.o)

LIB_NAME   := libwlansta

HEADER_FILES := $(SDK_DIR)/source/component/wifi/interface/include/hi_wlan_sta.h
HEADER_FILES += $(SDK_DIR)/source/component/wifi/interface/include/hi_wlan_p2p.h
HEADER_FILES += $(SDK_DIR)/source/component/wifi/interface/include/hi_wlan_common.h

include $(SDK_DIR)/hi_rules.mak
#.PHONY: all libwlanap clean install uninstall
#
#all:$(LIB_NAME)
#	$(AT)cp -f $(HEADER_FILES) $(HI_INCLUDE_DIR)/
#
#$(LIB_NAME):$(OBJS)
#	$(AT)$(CC) -shared -o $@.so  $(OBJS)
#	$(AT)$(AR) -rc -o $@.a $(OBJS)
#
#clean:
#	rm -rf $(OBJS) *.so *.a
