cmake_minimum_required(VERSION 3.14)
project(libmeta CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(libmeta INTERFACE)
target_include_directories(libmeta INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(example)

enable_testing()
add_subdirectory(test)
