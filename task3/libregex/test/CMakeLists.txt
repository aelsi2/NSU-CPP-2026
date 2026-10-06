find_package(GTest REQUIRED)

add_executable(
  LibRegexTest
  libregextest.cpp
)

set_target_properties(LibRegexTest PROPERTIES OUTPUT_NAME "libregex_test")

target_link_libraries(
  LibRegexTest
  PRIVATE 
    LibRegex
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(LibRegexTest)
