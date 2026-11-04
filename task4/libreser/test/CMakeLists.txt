find_package(GTest REQUIRED)

add_executable(
  LibReserTest
  main.cpp
  Integration/Json.cpp
  Integration/Xml.cpp
  Integration/Toml.cpp
  Integration/TomlTypes.cpp
  Json/Parser.cpp
  Json/Printer.cpp
  Json/IRConverter.cpp
  Xml/Parser.cpp
  Xml/Printer.cpp
  Xml/IRConverter.cpp
  Toml/Parser.cpp
  Toml/Printer.cpp
  Toml/IRConverter.cpp
)
target_include_directories(LibReserTest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(LibReserTest PROPERTIES OUTPUT_NAME "libreser_test")


target_link_libraries(
  LibReserTest
  PRIVATE 
    LibReser
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(LibReserTest)
