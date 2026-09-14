find_package(GTest REQUIRED)

add_executable(
  LibCompressorTest
  libcompressortest.cpp
)

set_target_properties(LibCompressorTest PROPERTIES OUTPUT_NAME "libcompressor_test")

target_link_libraries(
  LibCompressorTest
  PRIVATE 
    LibCompressor
    GTest::gtest_main
)

set(TEST_REPORT ${CMAKE_BINARY_DIR}/test_results.xml)

add_test(
  NAME LibCompressorRunTest
  COMMAND ${CMAKE_BINARY_DIR}/libcompressor/tests/libcompressor_test --gtest_output=xml:${TEST_REPORT}
  BUILD_DEPENDS LibCompressorTest
)
