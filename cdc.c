#include "cdc.h"
#include "keyboard.h"

#ifdef UNIT_TESTING
  void LEDs_TurnOnLEDs(const uint8_t LEDMask);
  void LEDs_TurnOffLEDs(const uint8_t LEDMask);
#endif

/*sublime-c-static-fn-hoist-start*/
static void remove_bytes_from_buffer(uint_fast8_t count);
static void handle_action_led_state(void);
static void handle_action_keyboard_key_state(void);
static void handle_action_keyboard_full_state(void);
static void handle_action_keyboard_modifier_state(void);
/*sublime-c-static-fn-hoist-end*/

static struct {
  char buf[100];
  uint8_t buf_used;
} self;

_Static_assert(
  sizeof(self.buf) < (1 << (8 * sizeof(self.buf_used))),
  "the type of self.buf_used is too small"
);

void cdc_reset(void) {
  memset(&self, 0, sizeof(self));
}

void cdc_receive_byte(int_fast16_t byte) {
  if (byte >= 0) {
    self.buf[self.buf_used++] = byte;
  }

  cdc_update();
}

void cdc_update(void) {
  if (self.buf_used < 3) {
    return;
  }

  enum action_e action = self.buf[0];

  switch (action) {
    case ACTION_led_state: {
      handle_action_led_state();
      break;
    }

    case ACTION_keyboard_key_state: {
      handle_action_keyboard_key_state();
      break;
    }

    case ACTION_keyboard_full_state: {
      handle_action_keyboard_full_state();
      break;
    }

    case ACTION_keyboard_modifier_state: {
      handle_action_keyboard_modifier_state();
      break;
    }

    default: {
      self.buf_used = 0;
      break;
    }
  }
}

static void remove_bytes_from_buffer(uint_fast8_t count) {
  if (count > self.buf_used) {
    self.buf_used = 0;
    return;
  }

  memmove(
    &self.buf[0],
    &self.buf[count],
    self.buf_used - count
  );

  self.buf_used -= count;
}

static void handle_action_led_state(void) {
  uint8_t led_id = self.buf[1];
  uint8_t led_on = self.buf[2];
  uint8_t led_mask = 0;

  if (led_id == 1) {
    led_mask = LED_BLUE;
  }

  else if (led_id == 2) {
    led_mask = LED_RED;
  }

  else {
    goto ret;
  }

  if (led_on) {
    LEDs_TurnOnLEDs(led_mask);
  }

  else {
    LEDs_TurnOffLEDs(led_mask);
  }

ret:
  remove_bytes_from_buffer(3);
}

static void handle_action_keyboard_key_state(void) {
  if (not keyboard_state_try_lock()) {
    return;
  }

  uint8_t key_code = self.buf[1];
  uint8_t is_down = self.buf[2];

  if (is_down) {
    key_press(key_code);
  }

  else {
    key_release(key_code);
  }

  remove_bytes_from_buffer(3);
}

static void handle_action_keyboard_full_state(void) {
  if (self.buf_used < 8) {
    return;
  }

  if (not keyboard_state_try_lock()) {
    return;
  }

  struct keyboard_state_s state = {
    .modifiers = self.buf[1],
    .key_count = KEYS_DOWN_MAX,
  };

  memcpy(state.keys, &self.buf[2], KEYS_DOWN_MAX);
  keyboard_set_state(&state);

  remove_bytes_from_buffer(8);
}

static void handle_action_keyboard_modifier_state(void) {
  if (self.buf_used < 2) {
    return;
  }

  if (not keyboard_state_try_lock()) {
    return;
  }

  keyboard_set_modifiers(self.buf[1]);
  remove_bytes_from_buffer(2);
}
