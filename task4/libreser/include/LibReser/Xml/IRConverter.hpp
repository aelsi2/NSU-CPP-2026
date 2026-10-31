#ifndef __LIBRESER_XML_IR_CONVERTER_HPP__
#define __LIBRESER_XML_IR_CONVERTER_HPP__

#include <functional>
#include <span>

#include "LibReser/Format.hpp"
#include "LibReser/IR.hpp"
#include "LibReser/Xml/AST.hpp"

namespace reser {
namespace xml {

namespace detail {
class Ast2IR;
class PatternMatcher;

struct TagPattern {
  std::function<bool(const TagNode &, const PatternMatcher &)> match;
  std::function<std::unique_ptr<IR::Node>(const TagNode &, Ast2IR &)> apply;
};

class PatternMatcher {
 public:
  virtual bool isSpecial(const TagNode &node) const = 0;
  virtual size_t getDepth() const = 0;
};

class Ast2IR : AstVisitor, PatternMatcher {
 public:
  Ast2IR(std::span<TagPattern> patterns) : depth(0), tagPatterns(patterns) {}
  std::unique_ptr<IR::Node> toIR(const XmlNode &node, bool quote = false);

 private:
  bool isSpecial(const TagNode &node) const override;
  size_t getDepth() const override;

  void visit(const TagNode &node) override;
  void visit(const TextNode &node) override;

  size_t depth;
  bool quote = false;
  std::unique_ptr<IR::Node> currentNode = nullptr;
  std::span<TagPattern> tagPatterns;
};

class IR2Ast : IR::Visitor, PatternMatcher {
 public:
  IR2Ast(std::span<TagPattern> patterns) : depth(0), tagPatterns(patterns) {}
  std::unique_ptr<XmlNode> toAst(const IR::Node &node);

 private:
  bool isSpecial(const TagNode &node) const override;
  size_t getDepth() const override;

  void visit(const IR::NullNode &node) override;
  void visit(const IR::StringNode &node) override;
  void visit(const IR::IntNode &node) override;
  void visit(const IR::FloatNode &node) override;
  void visit(const IR::BoolNode &node) override;
  void visit(const IR::DateTimeNode &node) override;
  void visit(const IR::TableNode &node) override;
  void visit(const IR::ListNode &node) override;
  void visit(const IR::TagNode &node) override;

  size_t depth;
  std::unique_ptr<XmlNode> currentNode = nullptr;
  std::span<TagPattern> tagPatterns;
};
};  // namespace detail

class IRConverter {
 public:
  static std::unique_ptr<XmlNode> fromIR(const IR::Node &node);
  static std::unique_ptr<IR::Node> toIR(const XmlNode &node);
};
static_assert(IsIRConverter<IRConverter, XmlNode>);

};  // namespace xml
};  // namespace reser

#endif  // __LIBRESER_XML_IR_CONVERTER_HPP__
