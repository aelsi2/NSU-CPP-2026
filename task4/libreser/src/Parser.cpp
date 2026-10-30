#include "LibReser/Parser.hpp"

#include <cctype>
#include <iostream>

using namespace reser;

int ParserState::peek() {
  if (stream.eof()) {
    return Eof;
  }
  return stream.peek();
}

int ParserState::peek2() {
  if (stream.eof()) {
    return Eof;
  }
  stream.get();
  int result = stream.eof() ? Eof : stream.peek();
  stream.unget();
  return result;
}

bool ParserState::match(char ch) {
  return peek() == ch;
}

bool ParserState::match2(char ch1, char ch2) {
  return peek() == ch1 && peek2() == ch2;
}

int ParserState::consume() {
  if (stream.eof()) {
    return Eof;
  }

  int ch = stream.get();

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
  while ((peek() != Eof && std::isspace(peek())) || (peek() == '\n' && !skipNewLine)) {
    consume();
  }
}

std::string ParserState::readAnyChar(const std::string_view chars) {
  std::string result;
  while (peek() != Eof && chars.find(peek()) != chars.npos) {
    int ch = consume();
    result.push_back(ch);
  }
  return result;
}

std::string ParserState::readUntilAny(const std::string_view chars) {
  std::string result;
  while (peek() != Eof && chars.find(peek()) == chars.npos) {
    int ch = consume();
    result.push_back(ch);
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
