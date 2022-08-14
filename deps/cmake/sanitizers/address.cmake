# https://github.com/google/sanitizers/wiki/AddressSanitizer
#
# https://docs.microsoft.com/en-us/cpp/sanitizers/asan?view=msvc-170
# https://devblogs.microsoft.com/cppblog/asan-for-windows-x64-and-debug-build-support/
#
# NOTE: ASAN appears to ignore "intentional" leaks, as in, you allocate
# something but the pointer stays in scope (eg. it's static),
#
#   static char *leak = NULL;
#
#   int main(void) {
#     leak = malloc(123);
#     return 0;
#   }
#
# However, if the pointer goes out of scope (or is cleared) without free, then
# ASAN will error.
#
#   int main(void) {
#     char *goes_out_of_scope = malloc(123);
#     return 0;
#   }
#
#   int main(void) {
#     leak = malloc(123);
#     leak = NULL;
#     return 0;
#   }
#
# So if you want to catch leaks where you got stuff like...
#
#   static struct {
#     bool something;
#     char *data; // this is malloc'd
#   } self;
#
# ...just memset it to zero before deinit/exit.
#
# Also, in a debugger, you can set a breakpoint on
#
#   __sanitizer::Die
#
# https://github.com/google/sanitizers/wiki/AddressSanitizerAndDebugger

# NOTE: MSVC added support in version 16.8 but this checks for 17.0 since I
# don't know what the exact value would be for 16.8

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC" AND MSVC_VERSION GREATER_EQUAL 1930)
  add_compile_options(
    -fsanitize=address
  )

  link_libraries(
    -fsanitize=address
  )

elseif (NOT CMAKE_C_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(
    -fsanitize=address
    -fno-omit-frame-pointer
  )

  link_libraries(
    -fsanitize=address
    -fno-omit-frame-pointer
  )

else()
  message("AddressSanitizer is not available on this compiler")
endif()
