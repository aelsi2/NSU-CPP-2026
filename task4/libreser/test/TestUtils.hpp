#include <string>

namespace test {

inline std::string trim(const std::string &str) {
  auto start = str.find_first_not_of("\n\t ");
  auto end = str.find_last_not_of("\n\t ") + 1;
  return str.substr(start, end);
}

inline bool contains(const std::string_view str, std::string_view substr) {
  return str.find(substr) != std::string_view::npos;
}

};  // namespace test
