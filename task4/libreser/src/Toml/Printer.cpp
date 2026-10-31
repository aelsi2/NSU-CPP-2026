#include "LibReser/Toml/Printer.hpp"

#include "LibReser/Utils.hpp"

using namespace reser::toml;
using namespace reser;

void Printer::print(std::ostream &stream, const TomlNode &node) {
  Printer printer(stream);
  node.accept(printer);
}

void Printer::visit(const StringNode &node) { stream << "\"" << tomlEscape(node.value) << "\""; }
void Printer::visit(const IntNode &node) { stream << node.value; }
void Printer::visit(const FloatNode &node) { stream << node.value; }
void Printer::visit(const BoolNode &node) { stream << node.value; }
void Printer::visit(const DateTimeNode &node) { stream << node.value.toString(); }

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
  printKey(kvp.first);
  stream << " = ";
  kvp.second->accept(*this);
  for (auto iter = std::next(node.children.begin()); iter != node.children.end(); iter++) {
    stream << ",\n";
    auto &kvp = *iter;
    printIndent();
    printKey(kvp.first);
    stream << " = ";
    kvp.second->accept(*this);
  }
  stream << "\n";
  indentLevel--;
  printIndent();
  stream << "}";
}

static bool isValidBareKey(std::string_view name) {
  for (const auto ch : name) {
    if (std::isalnum(ch, std::locale::classic())) {
      continue;
    }
    if (ch == '_' || ch == '-') {
      continue;
    }
    return false;
  }
  return true;
}

void Printer::printKey(std::string_view string) {
  if (isValidBareKey(string)) {
    stream << string;
  } else {
    stream << "\"" << tomlEscape(string) << "\"";
  }
}

void Printer::printIndent() {
  for (size_t i = 0; i < indentLevel; i++) {
    stream << indent;
  }
}
