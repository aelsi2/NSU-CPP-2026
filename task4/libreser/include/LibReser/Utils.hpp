#ifndef __LIBRESER_UTILS_HPP__
#define __LIBRESER_UTILS_HPP__

#include <stdexcept>
#include <string>

namespace reser {
std::string jsonEscape(const std::string &string);
std::string jsonUnescape(const std::string &string);

std::string tomlEscape(const std::string &string);
std::string tomlUnescape(const std::string &string);

std::string xmlEscape(const std::string &string);
std::string xmlUnescape(const std::string &string);

class EscapeError : public std::runtime_error {
  using Base = std::runtime_error;

 public:
  EscapeError(const std::string &message) : Base(message) {}
};

};  // namespace reser

#endif  // __LIBRESER_UTILS_HPP__
