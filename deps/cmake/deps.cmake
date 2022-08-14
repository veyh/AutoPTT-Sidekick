if (NOT DEPS_PATH)
  message(FATAL_ERROR "DEPS_PATH is not set")
endif()

file(GLOB deps "${DEPS_PATH}/*")

foreach (f ${deps})
  file(RELATIVE_PATH dep_name "${DEPS_PATH}" "${f}")

  get_property(
    already_included GLOBAL PROPERTY "${dep_name}_IS_ALREADY_INCLUDED"
  )

  # message("dbg ---------------")
  # message("dbg [${PROJECT_NAME}] dep_name: ${dep_name}")
  # message("dbg [${PROJECT_NAME}] dep_path: ${f}")
  # message("dbg [${PROJECT_NAME}] ${dep_name}_SOURCE_DIR: ${${dep_name}_SOURCE_DIR}")
  # message("dbg [${PROJECT_NAME}] ${dep_name}_BINARY_DIR: ${${dep_name}_BINARY_DIR}")
  # message("dbg [${PROJECT_NAME}] ${dep_name}_IS_ALREADY_INCLUDED: ${already_included}")

  IF (IS_DIRECTORY "${f}"
  AND (NOT DEPS_IGNORED OR NOT (
    "${dep_name}" MATCHES "${DEPS_IGNORED}"
  ))
  AND (
    (EXISTS "${f}/CMakeLists.txt" AND NOT already_included)
  ))
    message("[${PROJECT_NAME}] include dependency: ${dep_name} --> ${f}")
    add_subdirectory("${f}" EXCLUDE_FROM_ALL)
    set_property(GLOBAL PROPERTY "${dep_name}_IS_ALREADY_INCLUDED" ON)

  else()
    # message("dbg [${PROJECT_NAME}] SKIPPED")
  endif()
endforeach()
