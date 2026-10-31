#include "LibReser/Toml/IRConverter.hpp"

#include "LibReser/IR.hpp"
#include "LibReser/Toml/AST.hpp"
#include "LibReser/Utils.hpp"

using namespace reser::toml::detail;
using namespace reser::toml;
using namespace reser;

static std::vector<TablePattern> tagPatterns = {
    {
        .match =
            [](const TableNode &node, const PatternMatcher &matcher) {
              if (node.nValues() != 2) {
                return false;
              }
              auto type = node.getValue<StringNode>("$type");
              if (!type || type->value != "quote") {
                return false;
              }
              if (auto inner = node.getValue<TableNode>("inner")) {
                return matcher.isSpecial(*inner);
              }
              return matcher.getRecursionDepth() == 1;
            },
        .apply = [](const TableNode &node, Ast2IR &converter) -> std::unique_ptr<IR::Node> {
          auto inner = node.getValue<TableNode>("inner");
          return inner ? converter.toIR(*inner, true) : nullptr;
        },
    },
    {
        .match =
            [](const TableNode &node, const PatternMatcher &) {
              auto type = node.getValue<StringNode>("$type");
              return node.nValues() == 1 && type && type->value == "null";
            },
        .apply = [](const TableNode &, Ast2IR &) -> std::unique_ptr<IR::Node> {
          return std::make_unique<IR::NullNode>();
        },
    },
    {
        .match =
            [](const TableNode &node, const PatternMatcher &) {
              auto type = node.getValue<StringNode>("$type");
              auto name = node.getValue<StringNode>("name");
              auto attributes = node.getValue<TableNode>("attributes");
              auto children = node.getValue<ListNode>("children");

              if (node.nValues() != 4) {
                return false;
              }
              if (!type || type->value != "tag") {
                return false;
              }
              if (!name || !isValidXmlName(name->value)) {
                return false;
              }
              if (!children) {
                return false;
              }
              if (!attributes) {
                return false;
              }
              for (auto &attr : attributes->children) {
                if (!attr.second->as<StringNode>()) {
                  return false;
                }
                if (!isValidXmlName(attr.second->as<StringNode>()->value)) {
                  return false;
                }
              }
              return true;
            },
        .apply = [](const TableNode &node, Ast2IR &converter) -> std::unique_ptr<IR::Node> {
          auto nameNode = node.getValue<StringNode>("name");
          auto attributes = node.getValue<TableNode>("attributes");
          auto children = node.getValue<ListNode>("children");

          std::string name;
          if (nameNode) {
            name = nameNode->value;
          }

          IR::TagNode::AttrMap irAttributes;
          if (attributes) {
            for (auto &attr : attributes->children) {
              auto stringAttr = attr.second->as<StringNode>();
              if (stringAttr) {
                irAttributes[attr.first] = stringAttr->value;
              }
            }
          }

          IR::TagNode::ChildVector irChildren;
          if (children) {
            for (auto &child : children->children) {
              irChildren.push_back(converter.toIR(*child));
            }
          }

          return std::make_unique<IR::TagNode>(std::move(name), std::move(irChildren), std::move(irAttributes));
        },
    },
};

std::unique_ptr<IR::Node> Ast2IR::toIR(const TomlNode &node, bool quote) {
  bool oldQuote = this->quote;
  this->quote = quote;
  recursionDepth++;
  node.accept(*this);
  recursionDepth--;
  quote = oldQuote;
  return std::move(currentNode);
}

bool Ast2IR::isSpecial(const TableNode &node) const {
  for (const auto &pattern : tablePatterns) {
    if (pattern.match(node, *this)) {
      return true;
    }
  }
  return false;
}

size_t Ast2IR::getRecursionDepth() const { return recursionDepth; }

void Ast2IR::visit(const StringNode &node) { currentNode = std::make_unique<IR::StringNode>(node.value); }

void Ast2IR::visit(const IntNode &node) { currentNode = std::make_unique<IR::IntNode>(node.value); }

void Ast2IR::visit(const FloatNode &node) { currentNode = std::make_unique<IR::FloatNode>(node.value); }

void Ast2IR::visit(const BoolNode &node) { currentNode = std::make_unique<IR::BoolNode>(node.value); }

void Ast2IR::visit(const DateTimeNode &node) { currentNode = std::make_unique<IR::DateTimeNode>(node.value); }

void Ast2IR::visit(const ListNode &node) {
  IR::ListNode::ChildVector irChildren;
  for (auto &child : node.children) {
    irChildren.push_back(toIR(*child));
  }
  currentNode = std::make_unique<IR::ListNode>(std::move(irChildren));
}

void Ast2IR::visit(const TableNode &node) {
  if (!quote) {
    for (const auto &pattern : tablePatterns) {
      if (!pattern.match(node, *this)) {
        continue;
      }
      currentNode = pattern.apply(node, *this);
      return;
    }
  }
  IR::TableNode::ChildMap irChildren;
  for (auto &kvp : node.children) {
    irChildren[kvp.first] = toIR(*kvp.second);
  }
  currentNode = std::make_unique<IR::TableNode>(std::move(irChildren));
}

std::unique_ptr<TomlNode> IR2Ast::toAst(const IR::Node &node) {
  recursionDepth++;
  node.accept(*this);
  recursionDepth--;
  return std::move(currentNode);
}

bool IR2Ast::isSpecial(const TableNode &node) const {
  for (const auto &pattern : tablePatterns) {
    if (pattern.match(node, *this)) {
      return true;
    }
  }
  return false;
}

size_t IR2Ast::getRecursionDepth() const { return recursionDepth; }

std::unique_ptr<TableNode> IR2Ast::quoteNode(std::unique_ptr<TomlNode> node) {
  TableNode::ChildMap map;
  map["$type"] = std::make_unique<StringNode>("quote");
  map["inner"] = std::move(node);
  return std::make_unique<TableNode>(std::move(map));
}

std::unique_ptr<TomlNode> IR2Ast::quoteIfTopLevel(std::unique_ptr<TomlNode> node) {
  if (getRecursionDepth() == 1) {
    return quoteNode(std::move(node));
  }
  return node;
}

void IR2Ast::visit(const IR::NullNode &) {
  auto node = std::make_unique<TableNode>();
  node->children["$type"] = std::make_unique<StringNode>("null");
  currentNode = std::move(node);
}

// clang-format off
void IR2Ast::visit(const IR::StringNode &node) {
  currentNode = quoteIfTopLevel(std::make_unique<StringNode>(node.value));
}

void IR2Ast::visit(const IR::IntNode &node) { 
  currentNode = quoteIfTopLevel(std::make_unique<IntNode>(node.value)); 
}

void IR2Ast::visit(const IR::FloatNode &node) {
  currentNode = quoteIfTopLevel(std::make_unique<FloatNode>(node.value));
}

void IR2Ast::visit(const IR::BoolNode &node) {
  currentNode = quoteIfTopLevel(std::make_unique<BoolNode>(node.value));
}

void IR2Ast::visit(const IR::DateTimeNode &node) {
  currentNode = quoteIfTopLevel(std::make_unique<DateTimeNode>(node.value));
}
// clang-format on

void IR2Ast::visit(const IR::ListNode &node) {
  auto list = std::make_unique<ListNode>();
  for (auto &child : node.children) {
    list->children.push_back(toAst(*child));
  }
  currentNode = quoteIfTopLevel(std::move(list));
}

void IR2Ast::visit(const IR::TableNode &node) {
  auto table = std::make_unique<TableNode>();
  for (auto &kvp : node.children) {
    table->children[kvp.first] = toAst(*kvp.second);
  }

  if (isSpecial(*table)) {
    table = quoteNode(std::move(table));
  }

  currentNode = std::move(table);
}

void IR2Ast::visit(const IR::TagNode &node) {
  auto type = std::make_unique<StringNode>("tag");
  auto name = std::make_unique<StringNode>(node.name);
  auto attributes = std::make_unique<TableNode>();
  for (auto &kvp : node.attributes) {
    attributes->children[kvp.first] = std::make_unique<StringNode>(kvp.second);
  }
  auto children = std::make_unique<ListNode>();
  for (const auto &child : node.children) {
    children->children.push_back(toAst(*child));
  }

  auto tag = std::make_unique<TableNode>();
  tag->children["$type"] = std::move(type);
  tag->children["name"] = std::move(name);
  tag->children["attributes"] = std::move(attributes);
  tag->children["children"] = std::move(children);

  currentNode = std::move(tag);
}

std::unique_ptr<TomlNode> IRConverter::fromIR(const IR::Node &node) {
  IR2Ast converter(tagPatterns);
  return converter.toAst(node);
}

std::unique_ptr<IR::Node> IRConverter::toIR(const TomlNode &node) {
  Ast2IR converter(tagPatterns);
  return converter.toIR(node);
}
