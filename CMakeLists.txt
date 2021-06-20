cmake_minimum_required(VERSION 3.1) 
project(magahttpparser VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(ENABLE_TEST "Build Targets test on TRUE" TRUE)

add_library(mgparser INTERFACE)
target_compile_options(mgparser INTERFACE -Wall)
target_include_directories(mgparser INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)

# testing
if(${ENABLE_TEST})
    set(test_files ${CMAKE_CURRENT_SOURCE_DIR}/test/test.cpp)
    add_executable(test ${test_files})
    target_compile_options(test PUBLIC -Wall)

    find_package(GTest)
    if(GTest_FOUND)
        target_link_libraries(test PRIVATE GTest::GTest mgparser)
    else()
        message(STATUS "Did not find the library")
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.10.0
            SOURCE_DIR 
        )

        FetchContent_GetProperties(googletest)
        if(NOT googletest_POPULATED)
            FetchContent_Populate(googletest)
            set(BUILD_GMOCK OFF CACHE BOOL "Don't build GMock" FORCE)
            add_subdirectory("${googletest_SOURCE_DIR}" ${CMAKE_CURRENT_BINARY_DIR}/gtest)
        endif()
        target_link_libraries(test PRIVATE gtest mgparser)
    endif()

    add_test(
        NAME ParserTest
        COMMAND ParserTest
    )
endif()
# formating
file(GLOB_RECURSE ALL_FILES 
    ${test_files}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mgparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/mgparser.hpp
)

add_custom_target(
        format
        COMMAND clang-format
        -style=file
        -i
        --verbose
        ${ALL_FILES}
)