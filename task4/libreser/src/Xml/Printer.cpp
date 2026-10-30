#include "LibReser/Xml/Printer.hpp"

#include "LibReser/Utils.hpp"

using namespace reser::xml;
using namespace reser;

void Printer::print(std::ostream &stream, const XmlNode &node) {
  Printer printer(stream);
  node.accept(printer);
}

void Printer::visit(const TextNode &node) { stream << xmlEscape(node.value); }
void Printer::visit(const TagNode &node) {
  printIndent();
  stream << "<" << node.name;

  for (const auto &attr : node.attributes) {
    stream << " " << attr.first << "=\"" << xmlEscape(attr.second) << "\"";
  }
  if (node.children.empty()) {
    stream << "/>";
    return;
  } else {
    stream << ">";
  }

  bool hasText = node.hasText();

  size_t oldIndent = indentLevel;
  if (hasText) {
    indentLevel = 0;
  } else {
    indentLevel++;
    stream << "\n";
  }
  
  for (const auto &child : node.children) {
    child->accept(*this);
    if (!hasText) {
      stream << "\n";
    }
  }

  indentLevel = oldIndent;
  if (!hasText) {
    printIndent();
  }

  stream << "</" << node.name << ">";
}

void Printer::printIndent() {
  for (size_t i = 0; i < indentLevel; i++) {
    stream << indent;
  }
}
