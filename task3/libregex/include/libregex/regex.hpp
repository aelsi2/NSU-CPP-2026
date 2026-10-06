#ifndef __LIBREGEX_REGEX_HPP__
#define __LIBREGEX_REGEX_HPP__

#include <memory>
#include <vector>

namespace regex {

class RegexBlock;

/// An error thrown by \ref Regex::compile signalizing invalid regex syntax.
class RegexCompileError : public std::exception {
 public:
  /// Creates an instance of \ref RegexCompileError with the specified error message,
  /// the original input string that caused the error and the position of the error in the string.
  RegexCompileError(std::string_view message, std::string_view input, size_t position)
      : position(position), inputString(input), message(message) {}

  /// Gets the error message.
  const char *what() const noexcept override { return message.c_str(); };

  /// Gets the zero-based position of the error in the string representation of the expression.
  size_t pos() const noexcept { return position; }

  /// Gets the original string representation of the expression that failed to compile.
  const std::string_view input() const noexcept { return inputString; }

 private:
  size_t position;
  std::string inputString;
  std::string message;
};

/// A compiled regular expression that can match whole strings.
class Regex {
 public:
  ~Regex();

  Regex(Regex &&other) noexcept;
  Regex &operator=(Regex &&other) noexcept;

  Regex(const Regex &) = delete;
  Regex &operator=(const Regex &) = delete;

  /// Checks if the string matches the compiled regular expression completely (from start to end).
  bool match(std::string_view string);

  /// Parses the string representation of a regular expression and compiles it into a \ref Regex object.
  static Regex compile(std::string_view string);

 private:
  explicit Regex(std::vector<std::unique_ptr<RegexBlock>> blocks);
  std::vector<std::unique_ptr<RegexBlock>> blocks;
};
}  // namespace regex

#endif  // __LIBREGEX_REGEX_HPP__
