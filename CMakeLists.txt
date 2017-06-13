cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++1y -Wall -Wextra -g")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(LIB_NAME
  hang_detector
)

set(APP_NAME
  hangdetector_test
)

include_directories(
  .
  ${CMAKE_SOURCE_DIR}/include
)

set(SRC_FILES
  src/hang_detector.cc
  src/hang_action.cc
)

add_library(${LIB_NAME} SHARED ${SRC_FILES})

add_executable(${APP_NAME}
  test/main.cc
)

add_dependencies(${APP_NAME} ${LIB_NAME})

target_link_libraries(${APP_NAME}
  pthread
  -l${LIB_NAME}
  -L${CMAKE_BINARY_DIR}
)

target_link_libraries(${LIB_NAME}
  pthread
)
