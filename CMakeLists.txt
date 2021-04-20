cmake_minimum_required(VERSION 3.1) 
project(magahttpparser VERSION 0.1)

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

add_executable(example
    ${CMAKE_SOURCE_DIR}/main.cpp
)

target_link_libraries(example PRIVATE mgparser)