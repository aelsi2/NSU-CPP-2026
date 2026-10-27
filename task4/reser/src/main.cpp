#include <iostream>

#include <stdexcept>
#include "LibReser/Json/IRConverter.hpp"
#include "LibReser/Json/Parser.hpp"
#include "LibReser/Json/Printer.hpp"
#include "LibReser/Parser.hpp"

int main(/*int argc, char **argv*/) {
  try {
    auto node = reser::json::Parser::parse(std::cin);
    auto irNode = reser::json::IRConverter::toIR(*node);
    auto newNode = reser::json::IRConverter::fromIR(*irNode);
    reser::json::Printer::print(std::cout, *newNode);
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
