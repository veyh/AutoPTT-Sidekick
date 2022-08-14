#include "keyboard.h"

/*sublime-c-static-fn-hoist-start*/
static inline bool key_is_valid(uint8_t key_code);
/*sublime-c-static-fn-hoist-end*/

static struct {
  uint8_t keys_down[KEYS_DOWN_MAX];
  uint8_t key_modifiers;
  bool is_locked;
} self;

void keyboard_reset(void) {
  memset(&self, 0, sizeof(self));
}

void key_press(uint8_t key_code) {
  if (not key_is_valid(key_code)) {
    return;
  }

  for (uint_fast8_t i = 0; i < KEYS_DOWN_MAX; i++) {
    if (key_is_valid(self.keys_down[i])) {
      continue;
    }

    if (self.keys_down[i] == key_code) {
      break;
    }

    self.keys_down[i] = key_code;
    break;
  }
}

bool key_is_down(uint8_t key_code) {
  if (not key_is_valid(key_code)) {
    return false;
  }

  for (uint_fast8_t i = 0; i < KEYS_DOWN_MAX; i++) {
    if (self.keys_down[i] == key_code) {
      return true;
    }
  }

  return false;
}

void key_release(uint8_t key_code) {
  if (not key_is_valid(key_code)) {
    return;
  }

  for (uint_fast8_t i = 0; i < KEYS_DOWN_MAX; i++) {
    if (self.keys_down[i] == key_code) {
      self.keys_down[i] = 0;
    }
  }
}

static inline bool key_is_valid(uint8_t key_code) {
  return key_code > 3;
}

void keyboard_get_state(struct keyboard_state_s *dst) {
  dst->key_count = 0;

  for (uint_fast8_t i = 0; i < KEYS_DOWN_MAX; i++) {
    if (key_is_valid(self.keys_down[i])) {
      dst->keys[dst->key_count] = self.keys_down[i];
      dst->key_count += 1;
    }
  }

  dst->modifiers = self.key_modifiers;
}

void keyboard_set_state(struct keyboard_state_s const *src) {
  memset(self.keys_down, 0, sizeof(self.keys_down));

  for (uint_fast8_t i = 0; i < src->key_count; i++) {
    if (key_is_valid(src->keys[i])) {
      self.keys_down[i] = src->keys[i];
    }
  }

  self.key_modifiers = src->modifiers;
}

void keyboard_set_modifiers(uint8_t modifiers) {
  self.key_modifiers = modifiers;
}

bool keyboard_state_try_lock(void) {
  if (self.is_locked) {
    return false;
  }

  self.is_locked = true;
  return true;
}

void keyboard_state_unlock(void) {
  self.is_locked = false;
}
