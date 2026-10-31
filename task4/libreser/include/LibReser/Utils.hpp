#ifndef __LIBRESER_UTILS_HPP__
#define __LIBRESER_UTILS_HPP__

#include <stdexcept>
#include <string>

namespace reser {
enum class XmlEscapeContext {
  Attribute,
  Text,
};

std::string jsonEscape(const std::string_view string);
std::string jsonUnescape(const std::string_view string);

std::string tomlEscape(const std::string_view string);
std::string tomlUnescape(const std::string_view string);

std::string xmlEscape(const std::string_view string, XmlEscapeContext context);
std::string xmlUnescape(const std::string_view string);

class EscapeError : public std::runtime_error {
  using Base = std::runtime_error;

 public:
  EscapeError(const std::string &message) : Base(message) {}
};

};  // namespace reser

#endif  // __LIBRESER_UTILS_HPP__
