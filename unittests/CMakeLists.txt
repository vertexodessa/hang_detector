project(unittests)

include( CTest )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++14 -Wall -Wextra -g")
set(GTEST_BINARY_NAME GTestMain)

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
  ${Boost_LIBRARIES}
  )

add_custom_command(TARGET ${GTEST_BINARY_NAME}
  POST_BUILD COMMAND ./${GTEST_BINARY_NAME}
  )

add_dependencies(${GTEST_BINARY_NAME}
   ${LIB_NAME}
   )
