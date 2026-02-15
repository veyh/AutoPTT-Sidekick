#include "cdc.h"
#include "keyboard.h"
#include "core/defs.h"

/*sublime-c-static-fn-hoist-start*/
static int setup(void **ts);
static void test_sets_leds(void **ts);
static void cdc_receive_bytes(uint8_t const *data, size_t len);
static void test_sets_full_keyboard_state(void **ts);
static void test_buffers_keyboard_commands(void **ts);
/*sublime-c-static-fn-hoist-end*/

static struct {
  uint8_t led_mask; // active-low
} self;

void LEDs_TurnOnLEDs(const uint8_t mask) {
  self.led_mask &= ~mask;
}

void LEDs_TurnOffLEDs(const uint8_t mask) {
  self.led_mask |=  mask;
}

static int setup(void **ts) {
  keyboard_reset();
  cdc_reset();

  memset(&self, 0, sizeof(self));
  self.led_mask = 0xFF; // all off

  return 0;
}

static void test_sets_leds(void **ts) {
  assert_int_equal(0xFF, self.led_mask);

  cdc_receive_bytes((uint8_t []) { ACTION_led_state, 1, 1 }, 3);
  assert_int_equal(0xDF, self.led_mask);

  cdc_receive_bytes((uint8_t []) { ACTION_led_state, 2, 1 }, 3);
  assert_int_equal(0x9F, self.led_mask);

  cdc_receive_bytes((uint8_t []) { ACTION_led_state, 1, 0 }, 3);
  assert_int_equal(0xBF, self.led_mask);

  cdc_receive_bytes((uint8_t []) { ACTION_led_state, 2, 0 }, 3);
  assert_int_equal(0xFF, self.led_mask);
}

static void cdc_receive_bytes(uint8_t const *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    cdc_receive_byte(data[i]);
  }
}

static void test_sets_full_keyboard_state(void **ts) {
  cdc_receive_bytes((uint8_t []) {
    ACTION_keyboard_full_state,
    1, // modifiers
    0, 1, 2, 3, 4, 5 // keys, only 4 and 5 are valid
  }, 8);

  struct keyboard_state_s state;
  keyboard_get_state(&state);

  assert_int_equal(1, state.modifiers);
  assert_int_equal(2, state.key_count);
  assert_int_equal(4, state.keys[0]);
  assert_int_equal(5, state.keys[1]);
}

static void test_buffers_keyboard_commands(void **ts) {
  cdc_receive_bytes((uint8_t []) {
    ACTION_keyboard_key_state, 8, 1
  }, 3);

  cdc_receive_bytes((uint8_t []) {
    ACTION_keyboard_full_state,
    1, // modifiers
    4, 0, 0, 0, 0, 0,
  }, 8);

  cdc_receive_bytes((uint8_t []) {
    ACTION_keyboard_key_state, 9, 1
  }, 3);

  cdc_receive_bytes((uint8_t []) {
    ACTION_keyboard_full_state,
    2, // modifiers
    5, 0, 0, 0, 0, 0,
  }, 8);

  struct keyboard_state_s state;
  keyboard_get_state(&state);

  assert_int_equal(1, state.key_count);
  assert_int_equal(0, state.modifiers);
  assert_int_equal(8, state.keys[0]);

  keyboard_state_unlock();
  cdc_update();
  keyboard_get_state(&state);

  assert_int_equal(1, state.key_count);
  assert_int_equal(1, state.modifiers);
  assert_int_equal(4, state.keys[0]);

  keyboard_state_unlock();
  cdc_update();
  keyboard_get_state(&state);

  assert_int_equal(2, state.key_count);
  assert_int_equal(1, state.modifiers);
  assert_int_equal(4, state.keys[0]);
  assert_int_equal(9, state.keys[1]);

  keyboard_state_unlock();
  cdc_update();
  keyboard_get_state(&state);

  assert_int_equal(1, state.key_count);
  assert_int_equal(2, state.modifiers);
  assert_int_equal(5, state.keys[0]);
}

static void test_processes_buffered_commands_even_if_no_new_data_arrives(void **ts) {
  uint8_t two_commands[] = {
    ACTION_keyboard_full_state,
    1, // modifiers
    4, 0, 0, 0, 0, 0,

    ACTION_keyboard_full_state,
    2, // modifiers
    5, 0, 0, 0, 0, 0,
  };

  cdc_receive_bytes(two_commands, sizeof(two_commands));

  {
    struct keyboard_state_s state;
    keyboard_get_state(&state);
    assert_int_equal(1, state.key_count);
    assert_int_equal(1, state.modifiers);
    assert_int_equal(4, state.keys[0]);
  }

  keyboard_state_unlock();
  cdc_receive_byte(-1);

  {
    struct keyboard_state_s state;
    keyboard_get_state(&state);
    assert_int_equal(1, state.key_count);
    assert_int_equal(2, state.modifiers);
    assert_int_equal(5, state.keys[0]);
  }
}

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup(test_sets_leds, setup),
    cmocka_unit_test_setup(test_sets_full_keyboard_state, setup),
    cmocka_unit_test_setup(test_buffers_keyboard_commands, setup),
    cmocka_unit_test_setup(test_processes_buffered_commands_even_if_no_new_data_arrives, setup),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
