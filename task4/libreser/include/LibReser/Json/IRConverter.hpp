#ifndef __LIBRESER_JSON_IR_CONVERTER_HPP__
#define __LIBRESER_JSON_IR_CONVERTER_HPP__

#include <functional>
#include <span>

#include "LibReser/Format.hpp"
#include "LibReser/IR.hpp"
#include "LibReser/Json/AST.hpp"

namespace reser {
namespace json {

namespace detail {
class Ast2IR;
class PatternMatcher;

struct TablePattern {
  std::function<bool(const TableNode &, const PatternMatcher &)> match;
  std::function<std::unique_ptr<IR::Node>(const TableNode &, Ast2IR &)> apply;
};

class PatternMatcher {
 public:
  virtual bool isSpecial(const TableNode &node) const = 0;
};

class Ast2IR : AstVisitor, PatternMatcher {
 public:
  Ast2IR(std::span<TablePattern> patterns) : tablePatterns(patterns) {}
  std::unique_ptr<IR::Node> toIR(const JsonNode &node, bool quote = false);

 private:
  bool isSpecial(const TableNode &node) const override;

  void visit(const NullNode &node) override;
  void visit(const StringNode &node) override;
  void visit(const IntNode &node) override;
  void visit(const FloatNode &node) override;
  void visit(const BoolNode &node) override;
  void visit(const ListNode &node) override;
  void visit(const TableNode &node) override;

  bool quote = false;
  std::unique_ptr<IR::Node> currentNode = nullptr;
  std::span<TablePattern> tablePatterns;
};

class IR2Ast : IR::Visitor, PatternMatcher {
 public:
  IR2Ast(std::span<TablePattern> patterns) : tablePatterns(patterns) {}
  std::unique_ptr<JsonNode> toAst(const IR::Node &node);

 private:
  bool isSpecial(const TableNode &node) const override;

  void visit(const IR::NullNode &node) override;
  void visit(const IR::StringNode &node) override;
  void visit(const IR::IntNode &node) override;
  void visit(const IR::FloatNode &node) override;
  void visit(const IR::BoolNode &node) override;
  void visit(const IR::DateTimeNode &node) override;
  void visit(const IR::TableNode &node) override;
  void visit(const IR::ListNode &node) override;
  void visit(const IR::TagNode &node) override;

  std::unique_ptr<JsonNode> currentNode = nullptr;
  std::span<TablePattern> tablePatterns;
};
};  // namespace detail

class IRConverter {
 public:
  static std::unique_ptr<JsonNode> fromIR(const IR::Node &node);
  static std::unique_ptr<IR::Node> toIR(const JsonNode &node);
};
static_assert(IsIRConverter<IRConverter, JsonNode>);

};  // namespace json
};  // namespace reser

#endif  // __LIBRESER_JSON_IR_CONVERTER_HPP__
