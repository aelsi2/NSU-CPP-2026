#include <cstdlib>
#include <iostream>
#include <libregex/regex.hpp>

int main() {
  try {
    std::string line;
    if (!std::getline(std::cin, line)) {
      if (!std::cin.eof()) {
        std::cerr << "input error\n";
        return EXIT_FAILURE;
      }
    }

    bool matchedAny = false;
    regex::Regex regex = regex::Regex::compile(line);
    while (!std::cin.eof()) {
      if (!std::getline(std::cin, line)) {
        if (!std::cin.eof()) {
          std::cerr << "input error\n";
          return EXIT_FAILURE;
        } else {
          break;
        }
      }
      if (regex.match(line)) {
        matchedAny = true;
        std::cout << "true\n";
      } else {
        std::cout << "false\n";
      }
    }

    return matchedAny ? EXIT_SUCCESS : EXIT_FAILURE;
  } catch (const regex::RegexCompileError &error) {
    std::cerr << "regex error at position " << (error.pos() + 1) << ": " << error.what() << "\n";
    std::cerr << "\n  " << error.input() << "\n  ";
    for (size_t i = 0; i < error.pos(); i++) {
      std::cerr << ' ';
    }
    std::cerr << "^\n\n";
    return EXIT_FAILURE;
  } catch (const std::exception &exception) {
    std::cerr << "unhandled exception: " << exception.what() << "\n";
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "unknown exception\n";
    return EXIT_FAILURE;
  }
}
