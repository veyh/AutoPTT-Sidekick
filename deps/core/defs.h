#ifndef __core_defs_h__
#define __core_defs_h__

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <iso646.h>
#include <inttypes.h>

#if defined(_MSC_VER)
  #include <BaseTsd.h>
  typedef SSIZE_T ssize_t;
#endif

struct fatptr_s {
  union {
    void *data;
    void *ptr;
    void *value;
  };
  size_t len;
};

#define fatptr_const_s const_fatptr_s

struct const_fatptr_s {
  union {
    void const *data;
    void const *ptr;
    void const *value;
  };
  size_t len;
};

#ifdef UNIT_TESTING
  #include <setjmp.h>
  #include <cmocka.h>

  // CMocka's memory tracking capabilities don't even come close to what
  // AddressSanitizer can do, so just use that.
  #undef malloc
  #undef calloc
  #undef realloc
  #undef free

  #ifndef LINTING
    #undef assert
    #define assert(expression) \
      mock_assert((ssize_t)(expression), #expression, __FILE__, __LINE__)
  #endif

  // for custom errors
  extern void cm_print_error(const char * const format, ...);
#endif

#define w_check(condition, label) { \
  if (!(condition)) {               \
    goto label;                     \
  }                                 \
}

#define w_min_2(a, b) \
  ((a) < (b) ? (a) : (b))

#define w_min_3(a, b, c) \
  ((a) < (b) ? w_min_2((a), (c)) : w_min_2((b), (c))

#define w_max_2(a, b) \
  ((a) > (b) ? (a) : (b))

#define w_max_3(a, b, c) \
  ((a) > (b) ? w_max_2((a), (c)) : w_max_2((b), (c))

#if defined(__GNUC__) || defined(__clang__)
  #define w_unused __attribute__((unused))
  #define w_weak __attribute__((weak))
#else
  #define w_unused
  #define w_weak
#endif

#define NOHOIST

#define w_likely(x) __builtin_expect(!!(x), 1)
#define w_unlikely(x) __builtin_expect(!!(x), 0)

// For documentation. Type qualifier to document transfering ownership of
// a heap-allocated pointer.
//
// Example: caller takes ownership of `char *`, caller is responsible for
// freeing it.
//
//   extern w_move char *aaa(void);
//
// Example: `bbb` takes ownership of `char *x`. `bbb` is responsible for
// freeing it.
//
//   extern void bbb(w_move char *x);
#define w_move

// Type qualifier to indicate that this pointer is heap-allocated, and its
// owner is responsible for freeing it. Goes well with w_move, eg.
//
//   struct {
//     w_heap char *arg;
//   } self;
//
//   void take_it(w_move char *arg) {
//     self.arg = arg;
//   }
#define w_heap

// Indicates this is an STB object (see stb/stb_ds.h)
#define w_stb

#define w_array_size(_array) (sizeof(_array) / sizeof(*(_array)))
#define w_sizeof_field(type, field) (sizeof(((type*)0)->field))

// https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
#if defined(__GNUC__) && __GNUC__ >= 7
  #define w_intentional_fallthrough() __attribute__ ((fallthrough))
#else
  #define w_intentional_fallthrough()
#endif

#endif // __core_defs_h__
