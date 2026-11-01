#ifndef __LIBRESER_UTILS_HPP__
#define __LIBRESER_UTILS_HPP__

#include <stdexcept>
#include <string>

namespace reser {
enum class XmlEscapeContext {
  Attribute,
  Text,
};

std::string jsonEscape(std::string_view string);
std::string jsonUnescape(const std::string_view string);

std::string tomlEscape(std::string_view string);
std::string tomlUnescape(const std::string_view string);

std::string xmlEscape(std::string_view string, XmlEscapeContext context);
std::string xmlUnescape(std::string_view string);

bool isValidXmlName(std::string_view string);

class EscapeError : public std::runtime_error {
  using Base = std::runtime_error;

 public:
  EscapeError(const std::string &message) : Base(message) {}
};

inline constexpr std::string_view PositiveInfString = "+inf";
inline constexpr std::string_view NegativeInfString = "-inf";
inline constexpr std::string_view NanString = "nan";
inline constexpr std::string_view NullString = "null";
inline constexpr std::string_view TrueString = "true";
inline constexpr std::string_view FalseString = "false";

std::string formatFloat(double value);
std::string_view formatBool(bool value);

};  // namespace reser

#endif  // __LIBRESER_UTILS_HPP__
