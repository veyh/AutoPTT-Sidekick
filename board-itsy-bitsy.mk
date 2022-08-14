C_INCLUDE_PATH = /opt/avr8/avr/include

MCU          = atmega32u4
ARCH         = AVR8
BOARD        = NONE
F_CPU        = 16000000
F_USB        = $(F_CPU)
OPTIMIZATION = s
TARGET       = itsy-bitsy
SRC          = main.c Descriptors.c $(LUFA_SRC_USB) $(LUFA_SRC_USBCLASS)
LUFA_PATH    = deps/lufa/LUFA
CC_FLAGS     = -DUSE_LUFA_CONFIG_HEADER -IConfig/
LD_FLAGS     =
OBJDIR       = obj/itsy-bitsy

include shared.mk

.PHONY: flash
flash: $(TARGET).hex
	arduino-cli upload -v \
		-b adafruit:avr:itsybitsy32u4_5V \
		-p /dev/ttyACM0 \
		-i $(TARGET).hex
