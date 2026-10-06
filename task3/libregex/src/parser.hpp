#ifndef __LIBREGEX_PARSER_HPP__
#define __LIBREGEX_PARSER_HPP__

#include <optional>
#include <string_view>

#include "blocks.hpp"
#include "libregex/regex.hpp"

namespace regex {

/// A recursive descent parser for regular expressions.
class RegexParser final {
 public:
  /// Creates a new instance of \ref RegexParser to parse the specified string.
  ///
  /// The parser does not own the string, so the parser must not outlive the string.
  RegexParser(std::string_view string) : string(string), position(0) {}

  /// Parses the regex string and returns a sequence of blocks.
  ///
  /// May throw \ref RegexCompileError.
  std::vector<std::unique_ptr<RegexBlock>> parse();

 private:
  /// Reads the next character and consumes it. Returns the consumed character.
  char pop();

  /// Reads and returns the next character without consuming it.
  char peek() const;

  /// Checks if there are characters left to parse, and returns true if there are none.
  bool empty() const;

  /// Throws a \ref RegexCompileError at the position of the last consumed character or the specified position.
  void error(std::string_view message, std::optional<size_t> position = std::nullopt) const;

  std::unique_ptr<RegexBlock> parseGroup();
  std::unique_ptr<RegexBlock> parseChar();
  std::unique_ptr<RegexBlock> parseGroupOrChar();
  std::unique_ptr<RegexBlock> parseModifier(std::unique_ptr<RegexBlock> inner);

  std::string_view string;
  size_t position;
};

}  // namespace regex

#endif  // __LIBREGEX_PARSER_HPP__
