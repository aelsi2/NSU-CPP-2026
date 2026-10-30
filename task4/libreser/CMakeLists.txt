add_library(LibReser OBJECT)

target_sources(
  LibReser
  PRIVATE
    src/Utils.cpp
    src/Parser.cpp
    src/DateTime.cpp
    src/Json/IRConverter.cpp
    src/Json/Printer.cpp
    src/Json/Parser.cpp
    src/Xml/Printer.cpp
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
      include/LibReser/IR.hpp
      include/LibReser/Json/AST.hpp
      include/LibReser/Json/IRConverter.hpp
      include/LibReser/Json/Printer.hpp
      include/LibReser/Json/Parser.hpp
      include/LibReser/Xml/AST.hpp
      include/LibReser/Xml/IRConverter.hpp
      include/LibReser/Xml/Printer.hpp
      include/LibReser/Xml/Parser.hpp
)

add_subdirectory(test)
