#ifndef __keyboard_h__
#define __keyboard_h__

#include "defs.h"

#define KEYS_DOWN_MAX 6

extern void key_press(uint8_t key_code);
extern bool key_is_down(uint8_t key_code);
extern void key_release(uint8_t key_code);

struct keyboard_state_s {
  uint8_t keys[KEYS_DOWN_MAX];
  uint8_t key_count;
  uint8_t modifiers;
};

extern void keyboard_reset(void);
extern void keyboard_get_state(struct keyboard_state_s *dst);
extern void keyboard_set_state(struct keyboard_state_s const *src);
extern void keyboard_set_modifiers(uint8_t modifiers);

extern bool keyboard_state_try_lock(void);
extern void keyboard_state_unlock(void);

#endif // __keyboard_h__
