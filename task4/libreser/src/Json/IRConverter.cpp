#include "LibReser/Json/IRConverter.hpp"

#include <cmath>

#include "LibReser/IR.hpp"
#include "LibReser/Json/AST.hpp"
#include "LibReser/Utils.hpp"

using namespace reser::json::detail;
using namespace reser::json;
using namespace reser;

namespace reser::json::token {
namespace {
constexpr std::string_view Type = "$type";
constexpr std::string_view Quote = "quote";
constexpr std::string_view Inner = "inner";
constexpr std::string_view Time = "time";
constexpr std::string_view Tag = "tag";
constexpr std::string_view TagName = "name";
constexpr std::string_view TagChildren = "children";
constexpr std::string_view TagAttributes = "attributes";
};  // namespace
};  // namespace reser::json::token

static std::vector<TablePattern> tagPatterns = {
    {
        .match =
            [](const TableNode &node, const PatternMatcher &matcher) {
              auto type = node.getValue<StringNode>(token::Type);
              auto inner = node.getValue<TableNode>(token::Inner);
              return node.nValues() == 2 && type && type->value == token::Quote && inner && matcher.isSpecial(*inner);
            },
        .apply = [](const TableNode &node, Ast2IR &converter) -> std::unique_ptr<IR::Node> {
          auto inner = node.getValue<TableNode>(token::Inner);
          return inner ? converter.toIR(*inner, true) : nullptr;
        },
    },
    {
        .match =
            [](const TableNode &node, const PatternMatcher &) {
              auto type = node.getValue<StringNode>(token::Type);
              return node.nValues() == 1 && type && type->value == ctoken::PositiveInf;
            },
        .apply = [](const TableNode &, Ast2IR &) -> std::unique_ptr<IR::Node> {
          return std::make_unique<IR::FloatNode>(std::numeric_limits<double>::infinity());
        },
    },
    {
        .match =
            [](const TableNode &node, const PatternMatcher &) {
              auto type = node.getValue<StringNode>(token::Type);
              return node.nValues() == 1 && type && type->value == ctoken::NegativeInf;
            },
        .apply = [](const TableNode &, Ast2IR &) -> std::unique_ptr<IR::Node> {
          return std::make_unique<IR::FloatNode>(-std::numeric_limits<double>::infinity());
        },
    },
    {
        .match =
            [](const TableNode &node, const PatternMatcher &) {
              auto type = node.getValue<StringNode>(token::Type);
              return node.nValues() == 1 && type && type->value == ctoken::NaN;
            },
        .apply = [](const TableNode &, Ast2IR &) -> std::unique_ptr<IR::Node> {
          return std::make_unique<IR::FloatNode>(std::numeric_limits<double>::quiet_NaN());
        },
    },
    {
        .match =
            [](const TableNode &node, const PatternMatcher &) {
              auto type = node.getValue<StringNode>(token::Type);
              auto name = node.getValue<StringNode>(token::TagName);
              auto attributes = node.getValue<TableNode>(token::TagAttributes);
              auto children = node.getValue<ListNode>(token::TagChildren);

              if (node.nValues() != 4) {
                return false;
              }
              if (!type || type->value != token::Tag) {
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
          auto nameNode = node.getValue<StringNode>(token::TagName);
          auto attributes = node.getValue<TableNode>(token::TagAttributes);
          auto children = node.getValue<ListNode>(token::TagChildren);

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
              auto type = node.getValue<StringNode>(token::Type);
              auto value = node.getValue<StringNode>(token::Inner);
              DateTime dateTime;
              return node.nValues() == 2 && type && type->value == token::Time && value &&
                     DateTime::tryFromString(value->value, dateTime);
            },
        .apply = [](const TableNode &node, Ast2IR &) -> std::unique_ptr<IR::Node> {
          auto value = node.getValue<StringNode>(token::Inner);
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

void IR2Ast::visit(const IR::BoolNode &node) { currentNode = std::make_unique<BoolNode>(node.value); }

void IR2Ast::visit(const IR::FloatNode &node) {
  if (std::isinf(node.value) && node.value > 0) {
    auto table = std::make_unique<TableNode>();
    auto value = std::make_unique<StringNode>(std::string(ctoken::PositiveInf));

    table->children[std::string(token::Type)] = std::move(value);
    currentNode = std::move(table);

  } else if (std::isinf(node.value) && node.value < 0) {
    auto table = std::make_unique<TableNode>();
    auto value = std::make_unique<StringNode>(std::string(ctoken::NegativeInf));

    table->children[std::string(token::Type)] = std::move(value);
    currentNode = std::move(table);

  } else if (std::isnan(node.value)) {
    auto table = std::make_unique<TableNode>();
    auto value = std::make_unique<StringNode>(std::string(ctoken::NaN));

    table->children[std::string(token::Type)] = std::move(value);
    currentNode = std::move(table);

  } else {
    currentNode = std::make_unique<FloatNode>(node.value);
  }
}

void IR2Ast::visit(const IR::DateTimeNode &node) {
  auto table = std::make_unique<TableNode>();
  auto type = std::make_unique<StringNode>(std::string(token::Time));
  auto value = std::make_unique<StringNode>(node.value.toString());

  table->children[std::string(token::Type)] = std::move(type);
  table->children[std::string(token::Inner)] = std::move(value);
  currentNode = std::move(table);
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
    map[std::string(token::Type)] = std::make_unique<StringNode>(std::string(token::Quote));
    map[std::string(token::Inner)] = std::move(table);
    table = std::make_unique<TableNode>(std::move(map));
  }

  currentNode = std::move(table);
}

void IR2Ast::visit(const IR::TagNode &node) {
  auto type = std::make_unique<StringNode>(std::string(token::Tag));
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

  auto table = std::make_unique<TableNode>();
  table->children[std::string(token::Type)] = std::move(type);
  table->children[std::string(token::TagName)] = std::move(name);
  table->children[std::string(token::TagAttributes)] = std::move(attributes);
  table->children[std::string(token::TagChildren)] = std::move(children);

  currentNode = std::move(table);
}

std::unique_ptr<JsonNode> IRConverter::fromIR(const IR::Node &node) {
  IR2Ast converter(tagPatterns);
  return converter.toAst(node);
}

std::unique_ptr<IR::Node> IRConverter::toIR(const JsonNode &node) {
  Ast2IR converter(tagPatterns);
  return converter.toIR(node);
}
