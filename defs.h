#ifndef __c_usb_cdc_kb_defs_h__
#define __c_usb_cdc_kb_defs_h__

#ifdef UNIT_TESTING
  #define LEDS_LED1        (1 << 5)
  #define LEDS_LED2        (1 << 6)
#else
  #include <LUFA/Drivers/Board/LEDs.h>
#endif

#define LED_BLUE LEDS_LED1
#define LED_RED  LEDS_LED2

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <iso646.h>

#endif // __c_usb_cdc_kb_defs_h__
