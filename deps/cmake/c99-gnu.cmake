#
# build type flags
#
# NOTE: Supposedly -0g should be better for debugging but it does not seem to
# be the case as often variables that are still in scope are optimized out
# before the scope ends (the compiler has noticed that the variable is no
# longer used, even before the scope ends).
#
# https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
#
# Example
#
#   static void do_something(void *ptr, size_t size) {
#     void *end_ptr = ptr + size;
#
#     for (void *offset = ptr; offset < end_ptr; offset++) {
#       // if we only use `offset` and `end_ptr` here, for example, the
#       // function parameters `ptr` and `size` might have been optimized out
#       // already
#     }
#   }
#

set(CMAKE_C_FLAGS_RELEASE "-D NDEBUG -O3")
set(CMAKE_C_FLAGS_DEBUG   "-D DEBUG  -O0 -g3")

#
# build type
#

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
else()
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "")
endif()

set_property(
  CACHE CMAKE_BUILD_TYPE
  PROPERTY STRINGS
    Debug
    Release
)

#
# link flags
#

set(CORE_LDFLAGS "")

if (UNIX)
  set(CORE_LDFLAGS
    -Wl,-z,norelro
    -Wl,--build-id=none

    # dead code elimination
    # requires compilation with -ffunction-sections -fdata-sections
    -Wl,--gc-sections
  )
endif()

list(JOIN CORE_LDFLAGS " " CORE_LDFLAGS_STRING)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CORE_LDFLAGS_STRING}")

#
# compile options
# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html#Warning-Options
#

add_compile_options(
  # dead code elimination
  -ffunction-sections
  -fdata-sections

  -std=c99
  -D_GNU_SOURCE
  -fms-extensions

  -Wall
  -Wextra

  -Werror
  -Wimplicit-function-declaration

  # -Wdeclaration-after-statement
  -Wformat=2
  # -Wmissing-include-dirs
  -Wnested-externs
  -Wold-style-definition
  # -Wpedantic
  -Wredundant-decls
  -Wshadow
  -Wwrite-strings
  -Wstrict-prototypes
  -Wvla

  # Because this sort of thing is kind of common, we don't want to prevent that
  # struct foo_s foo = { FOO_defaults, .key = value };
  -Wno-override-init

  -Wno-missing-field-initializers
  -Wno-unused-parameter

  -Wformat-truncation
  -fno-common
  # -Wdouble-promotion
  # -Wundef
  # -Wconversion
  # -Wpadded
)

include(CheckCCompilerFlag)
check_c_compiler_flag(-masm=intel COMPILER_SUPPORTS_INTEL_ASM)
check_c_compiler_flag(-fverbose-asm COMPILER_SUPPORTS_VERBOSE_ASM)

if (COMPILER_SUPPORTS_INTEL_ASM AND COMPILER_SUPPORTS_VERBOSE_ASM)
  # To improve readability of generated x86 assembly files, use verbose Intel
  # syntax.
  #
  # With CMake, you can generate said assembly files by listing the targets
  #
  #   remake --targets | grep '\.s$'
  #
  # then you make one of those targets
  #
  #   make src/foo.s
  #
  # It will print where the generated file is, eg.
  #
  #   Compiling C source to assembly CMakeFiles/binary.dir/src/foo.c.s
  #
  # so then you just open that in your editor.

  add_compile_options(-masm=intel -fverbose-asm)
endif()

if (CMAKE_C_COMPILER_ID STREQUAL GNU
AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 7)
  add_compile_options(
    # This is for backwards compatibility for older versions of GCC which did
    # not care about this warning. Causes problems with eg. Nordic nRF5 SDK
    # otherwise.
    -Wno-expansion-to-defined
  )
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  # in a release build some logging calls may be removed so the variables they
  # reference become unused, and similarly some functions might become unused
  # as well
  add_compile_options(
    -Wno-error=unused-function
    -Wno-error=unused-variable
    -Wno-error=unused-but-set-variable
  )
endif()

set(DEBUG_WITH_COVERAGE ON CACHE BOOL "Collect coverage in Debug build")

if (DEBUG_WITH_COVERAGE
AND CMAKE_BUILD_TYPE STREQUAL "Debug"
AND NOT CMAKE_CROSSCOMPILING)
  add_compile_options(
    -fprofile-arcs # generates .gcda files
    -ftest-coverage # generates .gcno files
  )
  link_libraries(-lgcov)
endif()
