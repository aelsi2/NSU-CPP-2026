#ifndef __LIBRESER_XML_PRINTER_HPP__
#define __LIBRESER_XML_PRINTER_HPP__

#include "LibReser/Format.hpp"
#include "LibReser/Xml/AST.hpp"

namespace reser {
namespace xml {

class Printer : AstVisitor {
 public:
  static void print(std::ostream &stream, const XmlNode &node);

 private:
  Printer(std::ostream &stream, std::string_view indent = "    ") : stream(stream), indentLevel(0), indent(indent) {};

  void visit(const TagNode &node) override;
  void visit(const TextNode &node) override;

  void printIndent();

  std::ostream &stream;
  size_t indentLevel;
  std::string_view indent;
};
static_assert(IsPrinter<Printer, XmlNode>);

};  // namespace xml
};  // namespace reser

#endif  // __LIBRESER_XML_PRINTER_HPP__
