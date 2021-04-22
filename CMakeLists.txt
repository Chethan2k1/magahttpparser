cmake_minimum_required(VERSION 3.1) 
project(magahttpparser VERSION 0.1)
set(CMAKE_CXX_STANDARD 11)

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
find_package(GTest REQUIRED)
set(test_files ${CMAKE_SOURCE_DIR}/test/test.cpp)
add_executable(test ${test_files})

target_link_libraries(test PRIVATE GTest::GTest mgparser)

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