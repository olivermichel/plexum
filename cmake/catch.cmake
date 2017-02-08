set(CATCH_VERSION 1.7.1)

file(DOWNLOAD
        https://github.com/philsquared/Catch/releases/download/v${CATCH_VERSION}/catch.hpp
        ${CMAKE_HOME_DIRECTORY}/test/include/catch.h
        )
