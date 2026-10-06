add_library(LibRegex OBJECT)

target_sources(
  LibRegex
  PRIVATE
    src/regex.cpp
    src/parser.cpp
    src/blocks.cpp
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
      include/libregex/regex.hpp
)

add_subdirectory(test)
