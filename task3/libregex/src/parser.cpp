#include "parser.hpp"

#include <optional>
#include <string_view>

#include "blocks.hpp"

using namespace regex;

std::vector<std::unique_ptr<RegexBlock>> RegexParser::parse() {
  std::vector<std::unique_ptr<RegexBlock>> blocks;
  while (!empty()) {
    std::unique_ptr<RegexBlock> base = parseGroupOrChar();
    std::unique_ptr<RegexBlock> withModifier = parseModifier(std::move(base));
    blocks.push_back(std::move(withModifier));
  }
  return blocks;
}

static bool isValidChar(char ch) {
  if (ch >= '0' && ch <= '9') {
    return true;
  }
  if (ch >= 'a' && ch <= 'z') {
    return true;
  }
  if (ch >= 'A' && ch <= 'Z') {
    return true;
  }
  if (ch == ' ') {
    return true;
  }
  return false;
}

char RegexParser::pop() { return string[position++]; }

char RegexParser::peek() const { return string[position]; }

bool RegexParser::empty() const { return position >= string.size(); }

void RegexParser::error(std::string_view message, std::optional<size_t> position) const {
  throw RegexCompileError(message, string, position.value_or(this->position - 1));
}

std::unique_ptr<RegexBlock> RegexParser::parseGroup() {
  size_t groupStart = position;
  pop();

  std::optional<char> rangeStartChar = std::nullopt;
  bool isRange = false;

  std::vector<CharRange> ranges;
  while (true) {
    if (empty()) {
      error("unterminated group", groupStart);
    }

    char front = pop();

    if (front == '-') {
      if (!rangeStartChar) {
        error("range without start");
      }
      isRange = true;
    } else if (front == ']') {
      if (isRange) {
        error("range without end", position - 2);
      }
      break;
    } else {
      if (!isValidChar(front)) {
        error("unexpected group character");
      }
      if (isRange) {
        ranges.push_back(CharRange(*rangeStartChar, front));
        isRange = false;
        rangeStartChar = std::nullopt;
      } else {
        ranges.push_back(front);
        rangeStartChar = front;
      }
    }
  }
  return std::make_unique<GroupBlock>(std::move(ranges));
}

std::unique_ptr<RegexBlock> RegexParser::parseChar() {
  char front = pop();
  switch (front) {
    case '.':
      return std::make_unique<GroupBlock>();
    default:
      if (!isValidChar(front)) {
        error("unexpected character");
      }
      return std::make_unique<GroupBlock>(front);
  }
}

std::unique_ptr<RegexBlock> RegexParser::parseGroupOrChar() {
  switch (peek()) {
    case '[':
      return parseGroup();
    default:
      return parseChar();
  }
}

std::unique_ptr<RegexBlock> RegexParser::parseModifier(std::unique_ptr<RegexBlock> inner) {
  switch (peek()) {
    case '*':
      pop();
      return std::make_unique<StarBlock>(std::move(inner));
    case '+':
      pop();
      return std::make_unique<PlusBlock>(std::move(inner));
    case '?':
      pop();
      return std::make_unique<QuestionBlock>(std::move(inner));
    default:
      return inner;
  }
}
