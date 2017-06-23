cmake_minimum_required(VERSION 2.5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++14 -Wall -Wextra -g")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

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

target_link_libraries(${LIB_NAME}
  pthread
)

install(TARGETS ${LIB_NAME} DESTINATION lib)
