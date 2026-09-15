cmake_minimum_required(VERSION 3.23)

project(Compressor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror")

set(CMAKE_EXPORT_COMPILE_COMMANDS YES)

enable_testing()

include(GNUInstallDirs)

add_subdirectory(compressor)
add_subdirectory(libcompressor)
