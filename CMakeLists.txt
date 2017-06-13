cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++1y -Wall -Wextra -g")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(LIB_NAME
  hangdetector
)

include_directories(
  .
  ${CMAKE_SOURCE_DIR}/include
  )

set(SRC_FILES
  src/hang_detector.cc
  )

add_library(${LIB_NAME} SHARED ${SRC_FILES})

target_link_libraries(${LIB_NAME}
  pthread
  )
