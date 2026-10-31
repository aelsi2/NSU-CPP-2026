#ifndef __LIBRESER_TOML_PRINTER_HPP__
#define __LIBRESER_TOML_PRINTER_HPP__

#include "LibReser/Format.hpp"
#include "LibReser/Toml/AST.hpp"

namespace reser {
namespace toml {

class Printer : AstVisitor {
 public:
  static void print(std::ostream &stream, const TomlNode &node);

 private:
  Printer(std::ostream &stream, std::string_view indent = "    ") : stream(stream), indentLevel(0), indent(indent) {};

  void visit(const StringNode &node) override;
  void visit(const IntNode &node) override;
  void visit(const FloatNode &node) override;
  void visit(const BoolNode &node) override;
  void visit(const DateTimeNode &node) override;
  void visit(const ListNode &node) override;
  void visit(const TableNode &node) override;

  void printKey(std::string_view key);
  void printIndent();

  std::ostream &stream;
  size_t indentLevel;
  std::string_view indent;
};
static_assert(IsPrinter<Printer, TomlNode>);

};  // namespace toml
};  // namespace reser

#endif  // __LIBRESER_TOML_PRINTER_HPP__
