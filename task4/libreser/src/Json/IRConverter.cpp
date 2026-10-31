#include "LibReser/Json/IRConverter.hpp"

#include "LibReser/IR.hpp"
#include "LibReser/Json/AST.hpp"
#include "LibReser/Utils.hpp"
using namespace reser::json::detail;
using namespace reser::json;
using namespace reser;

static std::vector<TablePattern> tagPatterns = {
    {
        .match =
            [](const TableNode &node, const PatternMatcher &matcher) {
              auto type = node.getValue<StringNode>("$type");
              auto inner = node.getValue<TableNode>("inner");
              return node.nValues() == 2 && type && type->value == "quote" && inner && matcher.isSpecial(*inner);
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
    {
        .match =
            [](const TableNode &node, const PatternMatcher &) {
              auto type = node.getValue<StringNode>("$type");
              auto value = node.getValue<StringNode>("value");
              DateTime dateTime;
              return node.nValues() == 2 && type && type->value == "time" && value &&
                     DateTime::tryFromString(value->value, dateTime);
            },
        .apply = [](const TableNode &node, Ast2IR &) -> std::unique_ptr<IR::Node> {
          auto value = node.getValue<StringNode>("value");
          DateTime dateTime;
          DateTime::tryFromString(value->value, dateTime);
          return std::make_unique<IR::DateTimeNode>(dateTime);
        },
    },
};

std::unique_ptr<IR::Node> Ast2IR::toIR(const JsonNode &node, bool quote) {
  this->quote = quote;
  node.accept(*this);
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

void Ast2IR::visit(const NullNode &) {
  currentNode = std::make_unique<IR::NullNode>();
  quote = false;
}

void Ast2IR::visit(const StringNode &node) {
  currentNode = std::make_unique<IR::StringNode>(node.value);
  quote = false;
}

void Ast2IR::visit(const IntNode &node) {
  currentNode = std::make_unique<IR::IntNode>(node.value);
  quote = false;
}

void Ast2IR::visit(const FloatNode &node) {
  currentNode = std::make_unique<IR::FloatNode>(node.value);
  quote = false;
}

void Ast2IR::visit(const BoolNode &node) {
  currentNode = std::make_unique<IR::BoolNode>(node.value);
  quote = false;
}

void Ast2IR::visit(const ListNode &node) {
  IR::ListNode::ChildVector irChildren;
  for (auto &child : node.children) {
    child->accept(*this);
    irChildren.push_back(std::move(currentNode));
  }
  currentNode = std::make_unique<IR::ListNode>(std::move(irChildren));
  quote = false;
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
    kvp.second->accept(*this);
    irChildren[kvp.first] = std::move(currentNode);
  }
  currentNode = std::make_unique<IR::TableNode>(std::move(irChildren));
  quote = false;
}

std::unique_ptr<JsonNode> IR2Ast::toAst(const IR::Node &node) {
  node.accept(*this);
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

void IR2Ast::visit(const IR::NullNode &) { currentNode = std::make_unique<NullNode>(); }

void IR2Ast::visit(const IR::StringNode &node) { currentNode = std::make_unique<StringNode>(node.value); }

void IR2Ast::visit(const IR::IntNode &node) { currentNode = std::make_unique<IntNode>(node.value); }

void IR2Ast::visit(const IR::FloatNode &node) { currentNode = std::make_unique<FloatNode>(node.value); }

void IR2Ast::visit(const IR::BoolNode &node) { currentNode = std::make_unique<BoolNode>(node.value); }

void IR2Ast::visit(const IR::DateTimeNode &node) {
  auto type = std::make_unique<StringNode>("time");
  auto value = std::make_unique<StringNode>(node.value.toString());

  auto time = std::make_unique<TableNode>();
  time->children["$type"] = std::move(type);
  time->children["value"] = std::move(value);

  currentNode = std::move(time);
}

void IR2Ast::visit(const IR::ListNode &node) {
  auto list = std::make_unique<ListNode>();
  for (auto &child : node.children) {
    child->accept(*this);
    list->children.push_back(std::move(currentNode));
  }

  currentNode = std::move(list);
}

void IR2Ast::visit(const IR::TableNode &node) {
  auto table = std::make_unique<TableNode>();
  for (auto &kvp : node.children) {
    kvp.second->accept(*this);
    table->children[kvp.first] = std::move(currentNode);
  }

  if (isSpecial(*table)) {
    TableNode::ChildMap map;
    map["$type"] = std::make_unique<StringNode>("quote");
    map["inner"] = std::move(table);
    table = std::make_unique<TableNode>(std::move(map));
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
    child->accept(*this);
    children->children.push_back(std::move(currentNode));
  }

  auto tag = std::make_unique<TableNode>();
  tag->children["$type"] = std::move(type);
  tag->children["name"] = std::move(name);
  tag->children["attributes"] = std::move(attributes);
  tag->children["children"] = std::move(children);

  currentNode = std::move(tag);
}

std::unique_ptr<JsonNode> IRConverter::fromIR(const IR::Node &node) {
  IR2Ast converter(tagPatterns);
  return converter.toAst(node);
}

std::unique_ptr<IR::Node> IRConverter::toIR(const JsonNode &node) {
  Ast2IR converter(tagPatterns);
  return converter.toIR(node);
}
