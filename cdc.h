#ifndef __cdc_h__
#define __cdc_h__

#include "defs.h"

enum action_e {
  ACTION_led_state,
  ACTION_keyboard_key_state,
  ACTION_keyboard_full_state,
  ACTION_keyboard_modifier_state,
};

extern void cdc_receive_byte(int_fast16_t byte);
extern void cdc_reset(void);
extern void cdc_update(void);

#endif // __cdc_h__
