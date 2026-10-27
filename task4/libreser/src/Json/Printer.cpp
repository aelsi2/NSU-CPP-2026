#include "LibReser/Json/Printer.hpp"

#include "LibReser/Utils.hpp"

using namespace reser::json;
using namespace reser;

void Printer::print(std::ostream &stream, const AstNode &node) {
  Printer printer(stream);
  node.accept(printer);
}

void Printer::visit(const NullNode &) { stream << "null"; }
void Printer::visit(const StringNode &node) { stream << "\"" << jsonEscape(node.value) << "\""; }
void Printer::visit(const IntNode &node) { stream << node.value; }
void Printer::visit(const FloatNode &node) { stream << node.value; }
void Printer::visit(const BoolNode &node) { stream << node.value; }

void Printer::visit(const ListNode &node) {
  if (node.children.empty()) {
    stream << "[]";
    return;
  }
  stream << "[\n";
  indentLevel++;
  printIndent();
  (*node.children.begin())->accept(*this);
  for (auto iter = std::next(node.children.begin()); iter != node.children.end(); iter++) {
    stream << ",\n";
    printIndent();
    (*iter)->accept(*this);
  }
  stream << "\n";
  indentLevel--;
  printIndent();
  stream << "]";
}

void Printer::visit(const TableNode &node) {
  if (node.children.empty()) {
    stream << "{}";
    return;
  }
  stream << "{\n";
  indentLevel++;
  auto &kvp = *node.children.begin();
  printIndent();
  stream << "\"" << jsonEscape(kvp.first) << "\": ";
  kvp.second->accept(*this);
  for (auto iter = std::next(node.children.begin()); iter != node.children.end(); iter++) {
    stream << ",\n";
    auto &kvp = *iter;
    printIndent();
    stream << "\"" << jsonEscape(kvp.first) << "\": ";
    kvp.second->accept(*this);
  }
  stream << "\n";
  indentLevel--;
  printIndent();
  stream << "}";
}

void Printer::printIndent() {
  for (size_t i = 0; i < indentLevel; i++) {
    stream << indent;
  }
}
