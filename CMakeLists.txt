cmake_minimum_required(VERSION 2.8)
project(plexum)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(cmake/catch.cmake)

set(TESTS
    test/test_main.cc
    test/graph_test.cc
    )

add_executable(run_tests ${TESTS})
target_include_directories(run_tests PUBLIC include test/include)

add_executable(sandbox sandbox.cc)
target_include_directories(sandbox PUBLIC include)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
        )
endif(DOXYGEN_FOUND)
