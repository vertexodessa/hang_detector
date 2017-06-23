cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++14 -Wall -Wextra -g -fstack-protector-all")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#set(LIB_NAME
#  hang_detector
#)

#set(APP_NAME
#  hangdetector_test
#)

find_package(PkgConfig)
pkg_check_modules(BREAKPAD QUIET breakpad-client)
include_directories(${BREAKPAD_INCLUDE_DIRS})
link_directories(${BREAKPAD_LIBRARY_DIRS})

include_directories(
  .
  ${CMAKE_SOURCE_DIR}/include
)

set(SRC_FILES
  hang_detector.cc
  hang_action.cc
)

add_library(${LIB_NAME} SHARED ${SRC_FILES})

#add_executable(${APP_NAME}
#  test/main.cc
#)

#add_dependencies(${APP_NAME} ${LIB_NAME})

#target_link_libraries(${APP_NAME}
#  pthread
#  -l${LIB_NAME}
#  -L${CMAKE_BINARY_DIR}
#)

target_link_libraries(${LIB_NAME}
  pthread
)
