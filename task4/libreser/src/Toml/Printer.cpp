#include "LibReser/Toml/Printer.hpp"

#include "LibReser/Toml/AST.hpp"
#include "LibReser/Utils.hpp"

using namespace reser::toml;
using namespace reser;

void Printer::print(std::ostream &stream, const TomlNode &node) {
  Printer printer(stream);
  printer.printDocument(node);
}

void Printer::printDocument(const TomlNode &node) {
  Printer printer(stream);
  const TableNode *table = node.as<TableNode>();
  if (table) {
    printSection(TomlPathSlice(), *table);
  } else {
    throw std::logic_error("Invalid TOML root node.");
  }
}

void Printer::printSection(TomlPathSlice path, const TableNode &node) {
  printSectionHeader(path);

  bool printedAnyLines = false;

  // plain values
  for (const auto &kvp : node.children) {
    if (kvp.second->isa<TableNode>()) {
      continue;
    }
    if (auto list = kvp.second->as<ListNode>()) {
      if (list->isOfTables()) {
        continue;
      }
    }

    printKvp(kvp.first, *kvp.second);
    stream << "\n";
    printedAnyLines = true;
  }

  // nested sections
  TomlPath nextPath = TomlPath(path);
  nextPath.push_back("");
  for (const auto &kvp : node.children) {
    if (!kvp.second->isa<TableNode>()) {
      continue;
    }

    nextPath.isArrayOfTables = false;
    nextPath.pop_back();
    nextPath.push_back(kvp.first);

    if (printedAnyLines) {
      stream << "\n";
    } else {
      printedAnyLines = true;
    }
    printSection(nextPath, *kvp.second->as<TableNode>());
  }

  // nested array sections
  for (const auto &kvp : node.children) {
    if (!kvp.second->isa<ListNode>()) {
      continue;
    }
    if (!kvp.second->as<ListNode>()->isOfTables()) {
      continue;
    }

    nextPath.isArrayOfTables = true;
    nextPath.pop_back();
    nextPath.push_back(kvp.first);

    for (const auto &item : kvp.second->as<ListNode>()->children) {
      if (printedAnyLines) {
        stream << "\n";
      } else {
        printedAnyLines = true;
      }
      printSection(nextPath, *item->as<TableNode>());
    }
  }
}

void Printer::printSectionHeader(TomlPathSlice path) {
  if (path.empty()) {
    return;
  }
  if (path.isArrayOfTables) {
    stream << "[[";
  } else {
    stream << "[";
  }
  printKey(path.keys[0]);
  for (const auto &key : path.tail().keys) {
    stream << ".";
    printKey(key);
  }
  if (path.isArrayOfTables) {
    stream << "]]\n";
  } else {
    stream << "]\n";
  }
}

void Printer::visit(const StringNode &node) { stream << "\"" << tomlEscape(node.value) << "\""; }
void Printer::visit(const IntNode &node) { stream << std::format("{}", node.value); }
void Printer::visit(const FloatNode &node) { stream << formatFloat(node.value); }
void Printer::visit(const BoolNode &node) { stream << (node.value ? "true" : "false"); }
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
  printKvp(kvp.first, *kvp.second);
  for (auto iter = std::next(node.children.begin()); iter != node.children.end(); iter++) {
    stream << ",\n";
    auto &kvp = *iter;
    printIndent();
    printKvp(kvp.first, *kvp.second);
  }
  stream << "\n";
  indentLevel--;
  printIndent();
  stream << "}";
}

static bool isValidBareKey(std::string_view name) {
  if (name.empty()) {
    return false;
  }
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

void Printer::printKvp(std::string_view key, const TomlNode &value) {
  printKey(key);
  stream << " = ";
  value.accept(*this);
}

void Printer::printIndent() {
  for (size_t i = 0; i < indentLevel; i++) {
    stream << indent;
  }
}
