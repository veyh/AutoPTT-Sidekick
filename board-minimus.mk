C_INCLUDE_PATH = /opt/avr8/avr/include

MCU          = at90usb162
ARCH         = AVR8
BOARD        = MINIMUS
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = minimus
SRC          = main.c Descriptors.c $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS)
LUFA_PATH    = deps/lufa/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/
LD_FLAGS     =
OBJDIR       = obj/minimus

include shared.mk

.PHONY: flash
flash: $(TARGET).hex
	bin/dfu flash-write $(TARGET).hex
