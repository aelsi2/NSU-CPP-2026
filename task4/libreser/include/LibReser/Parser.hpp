#ifndef __LIBRESER_PARSER_HPP__
#define __LIBRESER_PARSER_HPP__

#include <cstddef>
#include <stdexcept>

namespace reser {

struct Position {
  size_t line;
  size_t column;
};

class ParserState {
 public:
  ParserState(std::istream &stream) : position{0, 0}, stream(stream) {}

  static constexpr int Eof = -1;

  int peek();
  int consume();
  const Position &pos() const { return position; }

  void parseToken(std::string_view token);
  void skipWhitespace(bool skipNewLine = true);
  std::string readChars(const std::string_view chars);

  [[noreturn]]
  void error(const std::string &message) const;
  [[noreturn]]
  void error(const std::string &message, const Position &position) const;
  [[noreturn]]
  void unexpectedToken(std::initializer_list<std::string_view> expected) const;
  [[noreturn]]
  void unexpectedToken(std::initializer_list<std::string_view> expected, const Position &position) const;

 private:
  Position position;
  std::istream &stream;
};

class ParseError : public std::runtime_error {
  using Base = std::runtime_error;

 public:
  ParseError(const std::string &message, const Position &position) : Base(message), position(position) {}
  const Position &pos() const { return position; }

 private:
  Position position;
};

};  // namespace reser

#endif  // __LIBRESER_PARSER_HPP__
