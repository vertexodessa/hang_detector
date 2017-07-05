cmake_minimum_required(VERSION 2.5)

set(SRC_DIR src)
set(TESTS_DIR unittests)

project(hangdetector)

#set(APP_NAME "${PROJECT_NAME}_bin")
set(LIB_NAME "${PROJECT_NAME}")

add_subdirectory(${SRC_DIR})
if (test)
    add_subdirectory(${TESTS_DIR})
endif()

set(PUBLIC_HEADERS
  include/hang_action.h
  include/hang_detector.h
  include/hang_detector_glib.h
  )

# pkg-config
configure_file(hangdetector.pc.cmake hangdetector.pc @ONLY)
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/hangdetector.pc"
    DESTINATION
    lib/pkgconfig
)

install(FILES ${PUBLIC_HEADERS}  DESTINATION include)
