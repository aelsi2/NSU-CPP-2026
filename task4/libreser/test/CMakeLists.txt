find_package(GTest REQUIRED)

add_executable(
  LibReserTest
  main.cpp
  Json/Parser.cpp
  Xml/Parser.cpp
  Toml/Parser.cpp
)

set_target_properties(LibReserTest PROPERTIES OUTPUT_NAME "libreser_test")

target_link_libraries(
  LibReserTest
  PRIVATE 
    LibReser
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(LibReserTest)
