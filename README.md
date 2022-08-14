# AutoPTT Sidekick

AutoPTT Sidekick is a virtual USB keyboard that can be controlled from the computer it's plugged into.

I made this for [AutoPTT](https://github.com/veyh/AutoPTT) because some games like CSGO require a physical key press to activate push-to-talk, and, as far as Windows knows, this thing *is* a physical keyboard. :)

I initially implemented this for an old AT90USB162 board (Minimus V1) as a proof of concept. Since that board isn't really available anywhere anymore, I also implemented it for basically any ATmega32u4-based board. I've tried it on a [Sparkfun Pro Micro](https://www.sparkfun.com/products/12640) and an [Adafruit ItsyBitsy](https://www.adafruit.com/product/3677), both of which are easily available and affordable.

Note that if you flash this on an Arduino, it will make the Arduino IDE unable to flash it again because it won't be able to tell the device to enter the bootloader. But you can force the device to enter the bootloader by pressing the reset button twice.

The Sparkfun Pro Micro doesn't have a reset button though, so instead, you'll have to connect the RST and GND pins (which is what the button would have done).

Source: https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide#ts-reset

## Build it yourself

Since I don't have a USB VID (Vendor ID) or a PID (Product ID), I can't distribute this in binary form. You have to build it for yourself with some VID/PID combination. You can use whatever you want, as long as the VID/PID associates with a compatible driver. Not all of them do.

Requirements

- Linux
- [git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git)
- [docker](https://docs.docker.com/get-docker/)
- [earthly](https://earthly.dev/get-earthly)

Copy `cfg.example.mk` to `cfg.mk`, then edit `CFG_VENDOR_ID` and `CFG_PRODUCT_ID` accordingly.

The ItsyBitsy build should work for pretty much any ATmega32u4 board. Once you have set up `cfg.mk`, run:

```bash
earthly +build
```

If everything goes as planned, you'll find an `itsy-bitsy.hex` in the `build/earthly` directory.

## Flash it

You should be able to flash the hex to an Arduino, directly from Arduino IDE.

You can also do it from the command line with `arduino-cli`. Example, for Adafruit ItsyBitsy:

```bash
# setup
arduino-cli config init
arduino-cli config add board_manager.additional_urls https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
arduino-cli core update-index
arduino-cli core install arduino:avr
arduino-cli core install adafruit:avr

# flash - note that you might need to change /dev/ttyACM0 to something else
arduino-cli upload -v \
  -b adafruit:avr:itsybitsy32u4_5V \
  -p /dev/ttyACM0 \
  -i build/earthly/itsy-bitsy.hex
```

**Remember, flashing an Arduino after this software is already on it will require you to manually enter the bootloader!**

In case you happen to have a Minimus board, you can use `dfu-programmer`.

```bash
# setup
sudo apt-get install -y dfu-programmer

# flash
sudo dfu-programmer at90usb162 erase
sudo dfu-programmer at90usb162 flash build/earthly/minimus.hex
```

## Try some commands

Once the device is flashed and plugged in, here are some commands you can use to test it on Linux. Note that you might have to change `/dev/ttyACM0` to something else.

### Change LED state

NOTE: Only works on Minimus at the moment.

```
payload
  u8 action = 0
  u8 led_id
  u8 led_on
```

**Examples**

blue led off

```bash
echo -ne '\x00\x01\x00' | sudo tee /dev/ttyACM0 >/dev/null
```

blue led on

```bash
echo -ne '\x00\x01\x01' | sudo tee /dev/ttyACM0 >/dev/null
```

red led off

```bash
echo -ne '\x00\x02\x00' | sudo tee /dev/ttyACM0 >/dev/null
```

red led on

```bash
echo -ne '\x00\x02\x01' | sudo tee /dev/ttyACM0 >/dev/null
```

### Change keyboard state for single key

NOTE: Don't send modifier keys as key codes, they will be ignored.

```
payload
  u8 action = 1
  u8 key_code
  u8 is_down
```

See `LUFA/Drivers/USB/Class/Common/HIDClassCommon.h` for the key codes (`HID_KEYBOARD_SC_*`)

**Examples**

keyboard f down, wait 100ms, keyboard f up

```bash
echo -ne '\x01\x09\x01' | sudo tee /dev/ttyACM0 >/dev/null
sleep 0.1
echo -ne '\x01\x09\x00' | sudo tee /dev/ttyACM0 >/dev/null
```

### Replace entire keyboard state

See `LUFA/Drivers/USB/Class/Common/HIDClassCommon.h` for the modifiers (`HID_KEYBOARD_MODIFIER_*`)

Modifiers are a bit mask, like `HID_KEYBOARD_MODIFIER_LEFTCTRL | HID_KEYBOARD_MODIFIER_LEFTSHIFT = (1 << 0) | (1 << 1) = 3`

```
payload
  u8 action = 2
  u8 modifiers
  u8 key_codes[6]
```

Key code value 0 means the key at that index is not used.

Key code with a valid key code means that key is currently down.

Duplicate key codes are ignored.

**Examples**

left shift, f  
all up

```bash
echo -ne '\x02\x02\x09\x00\x00\x00\x00\x00' | sudo tee /dev/ttyACM0 >/dev/null
sleep 0.1
echo -ne '\x02\x00\x00\x00\x00\x00\x00\x00' | sudo tee /dev/ttyACM0 >/dev/null
```

### Replace modifier state

```
payload
  u8 action = 3
  u8 modifiers
```

**Examples**

left ctrl, left shift  
all up

```bash
echo -ne '\x03\x03' | sudo tee /dev/ttyACM0 >/dev/null
sleep 0.1
echo -ne '\x03\x00' | sudo tee /dev/ttyACM0 >/dev/null
```

## Test

To build everything and run the tests, just do

```bash
earthly +all
```

