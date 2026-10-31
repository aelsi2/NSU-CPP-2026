#include <iostream>
#include <stdexcept>

#include "LibReser/Format.hpp"
#include "LibReser/Json/Backend.hpp"
#include "LibReser/Parser.hpp"
#include "LibReser/Toml/Backend.hpp"
#include "LibReser/Xml/Backend.hpp"

using namespace reser;

static std::unordered_map<std::string, FormatBackend &> Backends = {
    {"json", json::Backend},
    {"xml", xml::Backend},
    {"toml", toml::Backend},
};

static void printFormats(std::ostream &stream) {
  stream << "Available formats:\n";
  for (const auto &kvp : Backends) {
    stream << kvp.first << "\n";
  }
}

int main(int argc, char **argv) {
  try {
    if (argc != 3) {
      std::cerr << "Usage: " << argv[0] << " <in-format> <out-format>\n";
      return 1;
    }
    auto inFormat = Backends.find(argv[1]);
    if (inFormat == Backends.end()) {
      std::cerr << "Unknown in-format: " << argv[1] << "\n";
      printFormats(std::cerr);
      return 1;
    }
    auto outFormat = Backends.find(argv[2]);
    if (outFormat == Backends.end()) {
      std::cerr << "Unknown out-format: " << argv[2] << "\n";
      printFormats(std::cerr);
      return 1;
    }

    auto node = inFormat->second.fromStream(std::cin);
    outFormat->second.toStream(std::cout, *node);
    std::cout << '\n';
  } catch (const reser::ParseError &err) {
    std::cerr << "Parse error at line " << (err.pos().line + 1) << ", column " << (err.pos().column + 1) << ":\n"
              << err.what() << "\n";
    return 1;
  } catch (const std::runtime_error &err) {
    std::cerr << "Unexpected error: " << err.what() << "\n";
    return 1;
  } catch (...) {
    std::cerr << "Unknown error\n";
    return 1;
  }
  return 0;
}
