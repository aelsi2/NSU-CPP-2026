add_library(LibCompressor STATIC)
set_target_properties(LibCompressor PROPERTIES OUTPUT_NAME "compressor")

target_sources(
  LibCompressor
  PRIVATE
    src/libcompressor.cpp
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
      include/libcompressor.hpp
)

install(
  TARGETS LibCompressor
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)

target_link_libraries(LibCompressor PRIVATE ZLIB::ZLIB)
target_link_libraries(LibCompressor PRIVATE BZip2::BZip2)

find_package(Doxygen REQUIRED)

set(DOXYGEN_PROJECT_NAME "LibCompressor")
set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
set(DOXYGEN_EXTRACT_ALL YES)

doxygen_add_docs(LibCompressorDocs include/libcompressor.hpp)

add_subdirectory(tests)
