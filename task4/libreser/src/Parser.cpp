#include "LibReser/Parser.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>

using namespace reser;

std::optional<char> ParserState::peek() {
  int ch = stream.peek();
  if (stream.bad()) {
    throw std::runtime_error("I/O error.");
  }
  if (ch == std::char_traits<char>::eof()) {
    return std::nullopt;
  }
  return ch;
}

std::optional<char> ParserState::peek2() {
  int ch = stream.get();
  if (stream.bad()) {
    throw std::runtime_error("I/O error.");
  }
  if (ch == std::char_traits<char>::eof()) {
    stream.clear();
    return std::nullopt;
  }

  std::optional<char> result = peek();
  stream.unget();
  return result;
}

bool ParserState::match(char ch) { return peek() == ch; }

bool ParserState::match2(char ch1, char ch2) { return peek() == ch1 && peek2() == ch2; }

std::optional<char> ParserState::consume() {
  int ch = stream.get();
  if (stream.bad()) {
    throw std::runtime_error("I/O error.");
  }
  if (ch == std::char_traits<char>::eof()) {
    stream.clear();
    return std::nullopt;
  }

  if (ch == '\n') {
    position.line += 1;
    position.column = 0;
  } else {
    position.column += 1;
  }

  return ch;
}

void ParserState::parseToken(std::string_view token) {
  Position start = position;
  for (char ch : token) {
    if (peek() != ch) {
      std::string message("Unexpected token. Expected: ");
      message.append(token);
      error(message, start);
    }
    consume();
  }
}

void ParserState::skipWhitespace(bool skipNewLine) {
  while ((peek() && std::isspace(*peek(), std::locale::classic())) || (peek() == '\n' && !skipNewLine)) {
    consume();
  }
}

std::string ParserState::readAnyChar(const std::string_view chars) {
  std::string result;
  while (peek() && chars.find(*peek()) != chars.npos) {
    result.push_back(*consume());
  }
  return result;
}

std::string ParserState::readUntilAny(const std::string_view chars) {
  std::string result;
  while (peek() && chars.find(*peek()) == chars.npos) {
    result.push_back(*consume());
  }
  return result;
}

void ParserState::unexpectedToken(std::initializer_list<std::string_view> expected) const {
  unexpectedToken(expected, position);
}

void ParserState::unexpectedToken(std::initializer_list<std::string_view> expected, const Position &position) const {
  if (expected.size() == 0) {
    error("Unexpected token.", position);
  }
  std::string message("Unexpected token. Expected: '");
  message.append(*expected.begin());
  for (auto iter = std::next(expected.begin()); iter != expected.end(); iter++) {
    message.append(", ");
    message.append(*iter);
  }
  message.append("' expected");
  error(message, position);
}

void ParserState::error(const std::string &message) const { throw ParseError(message, position); }
void ParserState::error(const std::string &message, const Position &position) const {
  throw ParseError(message, position);
}
