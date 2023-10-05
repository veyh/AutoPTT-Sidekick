# AutoPTT Sidekick

AutoPTT Sidekick is a virtual USB keyboard that can be controlled from the computer it's plugged into.

I made this for [AutoPTT](https://github.com/veyh/AutoPTT) because some games like CSGO require a physical key press to activate push-to-talk, and, as far as Windows knows, this thing *is* a physical keyboard. :)

I initially implemented this for an old AT90USB162 board (Minimus V1) as a proof of concept. Since that board isn't really available anywhere anymore, I also implemented it for basically any ATmega32u4-based board. I've tried it on a [Sparkfun Pro Micro](https://www.sparkfun.com/products/12640) and an [Adafruit ItsyBitsy](https://www.adafruit.com/product/3677), both of which are easily available and affordable.

**Note that once this is flashed on an Arduino, further attempts at flashing will require you to manually enter the bootloader by pressing the reset button twice** as the Arduino desktop application will no longer be able to do it automatically.

The Sparkfun Pro Micro doesn't have a reset button though, so instead, you'll have to connect the RST and GND pins (which is what the button would have done).

Source: https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide#ts-reset

## Releases

You can download prebuilt binaries from [Github](https://github.com/veyh/AutoPTT-Sidekick/releases). They are built with USB Vendor ID `0x1209` and Product ID `0xE1E1`, generously provided by [pid.codes](https://pid.codes/).

The ItsyBitsy build should work for pretty much any ATmega32u4 board.

## Build it yourself

You can also build the binary yourself in case you want to make changes.

Requirements

- Linux
- [git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git)
- [docker](https://docs.docker.com/get-docker/)
- [earthly](https://earthly.dev/get-earthly)

Copy `cfg.example.mk` to `cfg.mk`, then edit the values accordingly. Once you're done with that, run

```bash
earthly +build
```

If everything goes as planned, you'll find `itsy-bitsy.hex` and `minimus.hex` in the `build/earthly` directory.

## Flash it
### On an Arduino

**Reminder: To manually enter the bootloader, press the reset button twice!**

For example, this is how you would do it on an Adafruit ItsyBitsy:

Install [arduino-cli](https://arduino.github.io/arduino-cli), then do the initial setup

```bash
arduino-cli config init
arduino-cli config add board_manager.additional_urls https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
arduino-cli core update-index
arduino-cli core install arduino:avr
arduino-cli core install adafruit:avr
```

This is how you would flash it on Linux, provided that the USB device is at `/dev/ttyACM0` and `itsy-bitsy.hex` is in the current directory.

```bash
arduino-cli upload -v -b adafruit:avr:itsybitsy32u4_5V -p /dev/ttyACM0 -i itsy-bitsy.hex
```

On Windows, the command is very similar, but the device will be `COM1` or something like that. You can find that out by opening Device Manger and looking under `Ports (COM & LPT)` while the device is in bootloader. It will most likely be `COM1`.

```bash
arduino-cli upload -v -b adafruit:avr:itsybitsy32u4_5V -p COM1 -i itsy-bitsy.hex
```

[This short video](https://youtu.be/uYgZXsn3ugg) demonstrates flashing on Windows.

### On a Minimus

In case you happen to have a Minimus board, you can use `dfu-programmer`.

Install dfu-programmer with

```bash
sudo apt-get install -y dfu-programmer
```

Then use it to first erase and then flash the device

```bash
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

