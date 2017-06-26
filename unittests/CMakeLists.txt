cmake_minimum_required(VERSION 2.5)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(unittests)

include( CTest )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++14 -Wall -Wextra -g")
set(GTEST_BINARY_NAME GTestMain)

#FIXME: conditional breakpad inclusion
find_package(PkgConfig)
pkg_check_modules(BREAKPAD QUIET breakpad-client)
include_directories(${BREAKPAD_INCLUDE_DIRS})
link_directories(${BREAKPAD_LIBRARY_DIRS})

#FIXME: conditional glib-2.0 inclusion
pkg_check_modules(GLIB glib-2.0)
include_directories(${GLIB_INCLUDE_DIRS})
link_directories(${GLIB_LIBRARY_DIRS})

enable_testing()
add_test(AllTests ${GTEST_BINARY_NAME})

find_package(GTest REQUIRED)
include_directories(
  ${GTEST_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/include/
  )

add_executable(${GTEST_BINARY_NAME}
  test_main.cc
  test_actions.cc
  )

target_link_libraries(${GTEST_BINARY_NAME}
  ${GTEST_BOTH_LIBRARIES}
  -pthread
  ${LIB_NAME}
  ${BREAKPAD_LIBRARIES}
  ${GLIB_LIBRARIES}
  )

add_custom_command(TARGET ${GTEST_BINARY_NAME}
  POST_BUILD COMMAND ./${GTEST_BINARY_NAME}
  )

add_dependencies(${GTEST_BINARY_NAME}
  ${LIB_NAME}
  )
