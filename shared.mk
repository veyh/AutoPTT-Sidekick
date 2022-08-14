include cfg.mk

CC_FLAGS += -DCFG_VENDOR_ID=$(CFG_VENDOR_ID)
CC_FLAGS += -DCFG_PRODUCT_ID=$(CFG_PRODUCT_ID)
CC_FLAGS += -DCFG_MANUFACTURER_STRING=L\"$(CFG_MANUFACTURER_STRING)\"
CC_FLAGS += -DCFG_PRODUCT_STRING=L\"$(CFG_PRODUCT_STRING)\"

SRC += keyboard.c
SRC += cdc.c

.PHONY: default
default: $(TARGET).hex

DMBS_LUFA_PATH ?= $(LUFA_PATH)/Build/LUFA
include $(DMBS_LUFA_PATH)/lufa-sources.mk
include $(DMBS_LUFA_PATH)/lufa-gcc.mk

DMBS_PATH ?= $(LUFA_PATH)/Build/DMBS/DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/cppcheck.mk
include $(DMBS_PATH)/doxygen.mk
include $(DMBS_PATH)/dfu.mk
include $(DMBS_PATH)/gcc.mk
include $(DMBS_PATH)/hid.mk
include $(DMBS_PATH)/avrdude.mk
include $(DMBS_PATH)/atprogram.mk
