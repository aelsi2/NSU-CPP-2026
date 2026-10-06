#ifndef __LIBREGEX_BLOCKS_HPP__
#define __LIBREGEX_BLOCKS_HPP__

#include <climits>
#include <memory>
#include <stack>
#include <string_view>
#include <vector>

namespace regex {

class RegexBlock;

/// Value used to identify a \ref RegexBlock in the matching chain and specify the rollback destination.
typedef size_t BlockToken;

/// Regex matcher state.
struct MatcherState {
  /// The remainder of the input string being matched. Characters are removed from the front.
  std::string_view string;

  /// The identifier of the \ref RegexBlock being executed or to be executed on rollback.
  BlockToken blockToken;

  /// The index of the current rollback iteration. Index 0 means that the block is being executed for the first time.
  int rollbackIndex;

  /// Creates an entry for the rollback stack from the current state.
  ///
  /// All fields are left unchanged, except for \ref rollbackIndex, which is incremented.
  inline MatcherState createRollback() const { return {string, blockToken, rollbackIndex + 1}; }
};

/// Describes a continuous character range in a \ref GroupBlock.
class CharRange {
 public:
  /// Creates a character range that contains all characters.
  CharRange() : min(CHAR_MIN), max(CHAR_MAX) {}

  /// Creates a character range that contains a single character.
  CharRange(char ch) : min(ch), max(ch) {}

  /// Creates a character range with the specified min and max values (both inclusive).
  CharRange(char min, char max) : min(min), max(max) {}

  /// Checks if the specified character is in the range.
  bool match(char ch) const;

 private:
  char min;
  char max;
};

/// An abstract regex block that matches sequences of characters according to some rules.
class RegexBlock {
 public:
  virtual ~RegexBlock() = default;

  /// Checks if the input string starts from a matching character sequence and consumes it if it does.
  ///
  /// May push entries to the rollback stack.
  virtual bool match(MatcherState &state, std::stack<MatcherState> &rollbackStack) const = 0;
};

/// A block that matches a single character, which must be present in at least one of the specified ranges.
class GroupBlock : public RegexBlock {
 public:
  /// Creates a \ref GroupBlock that accepts characters from the specified ranges.
  explicit GroupBlock(std::vector<CharRange> ranges) : ranges(std::move(ranges)) {}
  /// Creates a \ref GroupBlock that accepts one specific character.
  explicit GroupBlock(char ch) { ranges.push_back(ch); }
  /// Creates a \ref GroupBlock that accepts any characters.
  GroupBlock() { ranges.push_back(CharRange()); }

  virtual bool match(MatcherState &state, std::stack<MatcherState> &rollbackStack) const override;

 private:
  std::vector<CharRange> ranges;
};

/// A block that matches repeating sequences (at least one) matched by the inner block.
///
/// Executed greedily (i.e. matches as many characters as it can, and does not roll back).
class PlusBlock : public RegexBlock {
 public:
  /// Creates a \ref PlusBlock with the specified inner block.
  explicit PlusBlock(std::unique_ptr<RegexBlock> inner) : inner(std::move(inner)) {}

  virtual bool match(MatcherState &state, std::stack<MatcherState> &rollbackStack) const override;

 private:
  std::unique_ptr<RegexBlock> inner;
};

/// A block that matches repeating sequences (zero or more) matched by the inner block.
///
/// Executed greedily (i.e. matches as many characters as it can, and does not roll back).
class StarBlock : public PlusBlock {
 public:
  /// Creates a \ref StarBlock with the specified inner block.
  explicit StarBlock(std::unique_ptr<RegexBlock> inner) : PlusBlock(std::move(inner)) {}

  virtual bool match(MatcherState &state, std::stack<MatcherState> &rollbackStack) const override;
};

/// A block that matches either a sequence matched by the inner block or an empty string.
///
/// Executed lazily (i.e. first tries to match an empty string).
class QuestionBlock : public RegexBlock {
 public:
  /// Creates a \ref QuestionBlock with the specified inner block.
  explicit QuestionBlock(std::unique_ptr<RegexBlock> inner) : inner(std::move(inner)) {}

  virtual bool match(MatcherState &state, std::stack<MatcherState> &rollbackStack) const override;

 private:
  std::unique_ptr<RegexBlock> inner;
};

};  // namespace regex

#endif  // __LIBREGEX_BLOCKS_HPP__
