find_package(GTest REQUIRED)

add_executable(
  LibReserTest
  main.cpp
  Json/Parser.cpp
  Json/IRConverter.cpp
  Xml/Parser.cpp
  Xml/IRConverter.cpp
  Toml/Parser.cpp
  Toml/IRConverter.cpp
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
