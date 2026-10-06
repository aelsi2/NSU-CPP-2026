#include "blocks.hpp"

using namespace regex;

bool CharRange::match(char ch) const {
  return ch >= min && ch <= max;
}

bool GroupBlock::match(MatcherState &state, std::stack<MatcherState> &) const {
  if (state.string.length() == 0) {
    return false;
  }
  for (const CharRange &range : ranges) {
    if (range.match(state.string.front())) {
      state.string.remove_prefix(1);
      return true;
    }
  }
  return false;
}

bool PlusBlock::match(MatcherState &state, std::stack<MatcherState> &rollbackStack) const {
  bool matchedAnything = false;
  while (inner->match(state, rollbackStack)) {
    matchedAnything = true;
  }
  return matchedAnything;
}

bool StarBlock::match(MatcherState &state, std::stack<MatcherState> &rollbackStack) const {
  PlusBlock::match(state, rollbackStack);
  return true;
}

bool QuestionBlock::match(MatcherState &state, std::stack<MatcherState> &rollbackStack) const {
  if (state.rollbackIndex == 0) {
    rollbackStack.push(state.createRollback());
    return true;
  }
  return inner->match(state, rollbackStack);
}
