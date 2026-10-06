#include <libregex/regex.hpp>

#include "blocks.hpp"
#include "parser.hpp"

using namespace regex;

Regex::Regex(std::vector<std::unique_ptr<RegexBlock>> blocks) : blocks(std::move(blocks)) {}

Regex::~Regex() = default;
Regex::Regex(Regex &&other) noexcept = default;
Regex &Regex::operator=(Regex &&other) noexcept = default;

bool Regex::match(std::string_view string) {
  if (blocks.empty()) {
    return string.empty();
  }

  std::stack<MatcherState> rollbackStack;
  rollbackStack.push({
      .string = string,
      .blockToken = 0,
      .rollbackIndex = 0,
  });
  while (!rollbackStack.empty()) {
    MatcherState currentState = rollbackStack.top();
    rollbackStack.pop();
    for (; currentState.blockToken < blocks.size(); currentState.blockToken++, currentState.rollbackIndex = 0) {
      if (!blocks[currentState.blockToken]->match(currentState, rollbackStack)) {
        break;
      }
    }
    if (currentState.blockToken == blocks.size() && currentState.string.empty()) {
      return true;
    }
  }
  return false;
}

Regex Regex::compile(std::string_view string) {
  RegexParser parser(string);
  return Regex(parser.parse());
}
