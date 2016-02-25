cmake_minimum_required(VERSION 3.3)
project(omg)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(cmake/catch.cmake)

set(TESTS
        test/test_main.cc
        test/graph_test.cc
        )

add_executable(run_tests ${TESTS})
target_include_directories(run_tests PUBLIC include test/include)
