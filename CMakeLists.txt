cmake_minimum_required(VERSION 3.1) 
project(magahttpparser VERSION 0.1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(src_files 
    ${CMAKE_SOURCE_DIR}/src/parser.cpp
)

set(include_files
    ${CMAKE_SOURCE_DIR}/include/parser.hpp
)

add_library(mgparser STATIC 
    ${src_files}
    ${include_files}
)

target_include_directories(mgparser PUBLIC ${CMAKE_SOURCE_DIR}/include)

# testing 
set(test_files ${CMAKE_SOURCE_DIR}/test/test.cpp)
add_executable(test ${test_files})

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
        add_subdirectory("${googletest_SOURCE_DIR}")
    endif()

    target_link_libraries(test PRIVATE gtest mgparser)
    target_include_directories(test PUBLIC ${googletest_SOURCE_DIR}/googletest/include)
endif()

add_test(
    NAME ParserTest
    COMMAND ParserTest
)

# formating
file(GLOB_RECURSE ALL_FILES ${src_files} ${include_files} ${test_files})

add_custom_target(
        format
        COMMAND clang-format
        -style=file
        -i
        --verbose
        ${ALL_FILES}
)