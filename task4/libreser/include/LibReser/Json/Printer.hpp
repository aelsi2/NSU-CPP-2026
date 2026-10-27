#ifndef __LIBRESER_JSON_PRINTER_HPP__
#define __LIBRESER_JSON_PRINTER_HPP__

#include "LibReser/Format.hpp"
#include "LibReser/Json/AST.hpp"

namespace reser {
namespace json {

class Printer : AstVisitor {
 public:
  static void print(std::ostream &stream, const AstNode &node);

 private:
  Printer(std::ostream &stream, std::string_view indent = "    ") : stream(stream), indentLevel(0), indent(indent) {};

  void visit(const NullNode &node) override;
  void visit(const StringNode &node) override;
  void visit(const IntNode &node) override;
  void visit(const FloatNode &node) override;
  void visit(const BoolNode &node) override;
  void visit(const ListNode &node) override;
  void visit(const TableNode &node) override;

  void printIndent();

  std::ostream &stream;
  size_t indentLevel;
  std::string_view indent;
};
static_assert(IsPrinter<Printer, AstNode>);

};  // namespace json
};  // namespace reser

#endif  // __LIBRESER_JSON_PRINTER_HPP__
