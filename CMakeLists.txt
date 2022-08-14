cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(usb-cdc-keyboard C)

set(BUILD_SHARED_LIBS OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  include("deps/cmake/sanitizers/address.cmake")
endif()

include("deps/cmake/c99-gnu.cmake")

include_directories("deps")
include_directories("${PROJECT_BINARY_DIR}")

if (PROJECT_IS_TOP_LEVEL AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  enable_testing()
  add_compile_options(-D UNIT_TESTING)
  include("deps/cmake/cmocka-git.cmake")

  add_cmocka_test(cdc_test
    SOURCES cdc_test.c
    LINK_LIBRARIES cmocka cdc keyboard
  )
endif()

add_library(cdc STATIC cdc.c)
target_link_libraries(cdc PRIVATE keyboard)

add_library(keyboard STATIC keyboard.c)

set(DEPS_PATH "${CMAKE_CURRENT_LIST_DIR}/deps")
include("deps/cmake/deps.cmake")
