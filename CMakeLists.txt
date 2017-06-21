cmake_minimum_required(VERSION 3.0)

set(SRC_DIR src)
set(TESTS_DIR unittests)

project(hang_detector)

#set(APP_NAME "${PROJECT_NAME}_bin")
set(LIB_NAME "${PROJECT_NAME}")

add_subdirectory(${SRC_DIR})
if (test)
    add_subdirectory(${TESTS_DIR})
endif()
