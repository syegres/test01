cmake_minimum_required(VERSION 3.2)
project(test01)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories("gtest/include")
link_directories("gtest/lib")

set(SOURCE_FILES calculator.hpp calculator.cpp)
add_executable(test01 ${SOURCE_FILES})
target_link_libraries(test01 pthread)

set(SOURCE_FILES_TESTS calculator_test.cpp)
add_executable(test01_test ${SOURCE_FILES_TESTS})
target_link_libraries(test01_test gtest gtest_main pthread)
