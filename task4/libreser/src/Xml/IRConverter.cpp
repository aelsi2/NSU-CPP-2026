#include "LibReser/Xml/IRConverter.hpp"

#include <unordered_set>

#include "LibReser/IR.hpp"
#include "LibReser/Utils.hpp"
#include "LibReser/Xml/AST.hpp"

using namespace reser::xml::detail;
using namespace reser::xml;
using namespace reser;

namespace reser::xml::token {
namespace {
constexpr std::string_view Quote = "quote";
constexpr std::string_view Int = "int";
constexpr std::string_view Float = "float";
constexpr std::string_view Bool = "bool";
constexpr std::string_view True = "true";
constexpr std::string_view False = "false";
constexpr std::string_view List = "list";
constexpr std::string_view Element = "element";
constexpr std::string_view Table = "table";
constexpr std::string_view Key = "key";
constexpr std::string_view Null = "null";
constexpr std::string_view Time = "time";
};  // namespace
};  // namespace reser::xml::token

static std::vector<TagPattern> tagPatterns = {
    {
        .match =
            [](const TagNode &node, const PatternMatcher &matcher) {
              if (node.name != token::Quote) {
                return false;
              }
              if (node.nAttributes() != 0) {
                return false;
              }
              if (node.nChildren() > 1) {
                return false;
              }
              if (auto tagChild = node.getChild<TagNode>()) {
                return matcher.isSpecial(*tagChild);
              }
              return matcher.getRecursionDepth() == 1;
            },
        .apply = [](const TagNode &node, Ast2IR &converter) -> std::unique_ptr<IR::Node> {
          auto child = node.getChild<XmlNode>();
          return child ? converter.toIR(*child, true) : std::make_unique<IR::StringNode>("");
        },
    },
    {
        .match =
            [](const TagNode &node, const PatternMatcher &) {
              return node.name == token::Null && node.nAttributes() == 0 && node.nChildren() == 0;
            },
        .apply = [](const TagNode &, Ast2IR &) -> std::unique_ptr<IR::Node> {
          return std::make_unique<IR::NullNode>();
        },
    },
    {
        .match =
            [](const TagNode &node, const PatternMatcher &) {
              if (node.name != token::Int || node.nAttributes() != 0 || node.nChildren() != 1) {
                return false;
              }
              if (auto child = node.getChild<TextNode>()) {
                const char *stringNumStart = child->value.data();
                const char *stringNumEnd = child->value.data() + child->value.size();
                int64_t value = 0;
                auto [ptr, ec] = std::from_chars(stringNumStart, stringNumEnd, value);
                if (ec == std::errc() && ptr == stringNumEnd) {
                  return true;
                }
              }
              return true;
            },
        .apply = [](const TagNode &node, Ast2IR &) -> std::unique_ptr<IR::Node> {
          if (auto child = node.getChild<TextNode>()) {
            const char *stringNumStart = child->value.data();
            const char *stringNumEnd = child->value.data() + child->value.size();
            int64_t value = 0;
            auto [ptr, ec] = std::from_chars(stringNumStart, stringNumEnd, value);
            if (ec == std::errc() && ptr == stringNumEnd) {
              return std::make_unique<IR::IntNode>(value);
            }
          }
          return nullptr;
        },
    },
    {
        .match =
            [](const TagNode &node, const PatternMatcher &) {
              if (node.name != token::Float || node.nAttributes() != 0 || node.nChildren() != 1) {
                return false;
              }
              if (auto child = node.getChild<TextNode>()) {
                if (child->value == ctoken::PositiveInf || child->value == ctoken::NegativeInf ||
                    child->value == ctoken::NaN) {
                  return true;
                }
                const char *stringNumStart = child->value.data();
                const char *stringNumEnd = child->value.data() + child->value.size();
                double value = 0;
                auto [ptr, ec] = std::from_chars(stringNumStart, stringNumEnd, value);
                if (ec == std::errc() && ptr == stringNumEnd) {
                  return true;
                }
              }
              return true;
            },
        .apply = [](const TagNode &node, Ast2IR &) -> std::unique_ptr<IR::Node> {
          if (auto child = node.getChild<TextNode>()) {
            if (child->value == ctoken::PositiveInf) {
              return std::make_unique<IR::FloatNode>(std::numeric_limits<double>::infinity());
            }
            if (child->value == ctoken::NegativeInf) {
              return std::make_unique<IR::FloatNode>(-std::numeric_limits<double>::infinity());
            }
            if (child->value == ctoken::NaN) {
              return std::make_unique<IR::FloatNode>(std::numeric_limits<double>::quiet_NaN());
            }
            const char *stringNumStart = child->value.data();
            const char *stringNumEnd = child->value.data() + child->value.size();
            double value = 0;
            auto [ptr, ec] = std::from_chars(stringNumStart, stringNumEnd, value);
            if (ec == std::errc() && ptr == stringNumEnd) {
              return std::make_unique<IR::FloatNode>(value);
            }
          }
          return nullptr;
        },
    },
    {
        .match =
            [](const TagNode &node, const PatternMatcher &) {
              auto child = node.getChild<TextNode>();
              return node.name == token::Bool && node.nAttributes() == 0 && node.nChildren() == 1 && child &&
                     (child->value == token::True || child->value == token::False);
            },
        .apply = [](const TagNode &node, Ast2IR &) -> std::unique_ptr<IR::Node> {
          auto child = node.getChild<TextNode>();
          return child ? std::make_unique<IR::BoolNode>(child->value == token::True) : nullptr;
        },
    },
    {
        .match =
            [](const TagNode &node, const PatternMatcher &) {
              if (node.name != token::Time || node.nAttributes() != 0 || node.nChildren() != 1) {
                return false;
              }
              if (auto child = node.getChild<TextNode>()) {
                DateTime dateTime;
                if (DateTime::tryFromString(child->value, dateTime)) {
                  return true;
                }
              }
              return true;
            },
        .apply = [](const TagNode &node, Ast2IR &) -> std::unique_ptr<IR::Node> {
          if (auto child = node.getChild<TextNode>()) {
            DateTime dateTime;
            if (DateTime::tryFromString(child->value, dateTime)) {
              return std::make_unique<IR::DateTimeNode>(dateTime);
            }
          }
          return nullptr;
        },
    },
    {
        .match =
            [](const TagNode &node, const PatternMatcher &) {
              if (node.name != token::List || node.nAttributes() != 0) {
                return false;
              }
              for (const auto &child : node.children) {
                if (auto element = child->as<TagNode>()) {
                  if (element->name == token::Element && element->nAttributes() == 0 && element->nChildren() <= 1) {
                    continue;
                  }
                }
                return false;
              }
              return true;
            },
        .apply = [](const TagNode &node, Ast2IR &converter) -> std::unique_ptr<IR::Node> {
          IR::ListNode::ChildVector children;
          for (const auto &child : node.children) {
            if (auto element = child->as<TagNode>()) {
              if (auto inner = element->getChild<XmlNode>()) {
                children.push_back(converter.toIR(*inner));
              } else {
                children.push_back(std::make_unique<IR::StringNode>(""));
              }
            }
          }
          return std::make_unique<IR::ListNode>(std::move(children));
        },
    },
    {
        .match =
            [](const TagNode &node, const PatternMatcher &) {
              if (node.name != token::Table || node.nAttributes() != 0) {
                return false;
              }
              std::unordered_set<std::string> keys;
              for (const auto &child : node.children) {
                if (!child->isa<TagNode>()) {
                  return false;
                }
                auto element = child->as<TagNode>();
                if (element->name != token::Element || element->nAttributes() != 1 || element->nChildren() > 1) {
                  return false;
                }
                auto attribute = element->attributes.find(std::string(token::Key));
                if (attribute == element->attributes.end()) {
                  return false;
                }
                auto [_, success] = keys.insert(attribute->second);
                if (!success) {
                  return false;
                }
              }
              return true;
            },
        .apply = [](const TagNode &node, Ast2IR &converter) -> std::unique_ptr<IR::Node> {
          auto tableNode = std::make_unique<IR::TableNode>();
          for (const auto &child : node.children) {
            if (auto element = child->as<TagNode>()) {
              const std::string &key = element->attributes.find(std::string(token::Key))->second;
              if (auto inner = element->getChild<XmlNode>()) {
                tableNode->children[key] = converter.toIR(*inner);
              } else {
                tableNode->children[key] = std::make_unique<IR::StringNode>("");
              }
            }
          }
          return tableNode;
        },
    },
};

std::unique_ptr<IR::Node> Ast2IR::toIR(const XmlNode &node, bool quote) {
  bool oldQuote = this->quote;
  this->quote = quote;
  recursionDepth++;
  node.accept(*this);
  recursionDepth--;
  quote = oldQuote;
  return std::move(currentNode);
}

bool Ast2IR::isSpecial(const TagNode &node) const {
  for (const auto &pattern : tagPatterns) {
    if (pattern.match(node, *this)) {
      return true;
    }
  }
  return false;
}

void Ast2IR::visit(const TextNode &node) { currentNode = std::make_unique<IR::StringNode>(node.value); }

void Ast2IR::visit(const TagNode &node) {
  if (!quote) {
    for (const auto &pattern : tagPatterns) {
      if (!pattern.match(node, *this)) {
        continue;
      }
      currentNode = pattern.apply(node, *this);
      return;
    }
  }
  auto tagNode = std::make_unique<IR::TagNode>(node.name);
  for (auto &child : node.children) {
    tagNode->children.push_back(toIR(*child));
  }
  tagNode->attributes = node.attributes;
  currentNode = std::move(tagNode);
}

std::unique_ptr<XmlNode> IR2Ast::toAst(const IR::Node &node) {
  recursionDepth++;
  node.accept(*this);
  recursionDepth--;
  return std::move(currentNode);
}

bool IR2Ast::isSpecial(const TagNode &node) const {
  for (const auto &pattern : tagPatterns) {
    if (pattern.match(node, *this)) {
      return true;
    }
  }
  return false;
}

void IR2Ast::visit(const IR::NullNode &) { currentNode = std::make_unique<TagNode>(std::string(token::Null)); }

void IR2Ast::visit(const IR::StringNode &node) {
  currentNode = std::make_unique<TextNode>(node.value);
  if (getRecursionDepth() == 1) {
    currentNode = std::make_unique<TagNode>(std::string(token::Quote), std::move(currentNode));
  }
}

void IR2Ast::visit(const IR::IntNode &node) {
  auto text = std::make_unique<TextNode>(std::format("{}", node.value));
  currentNode = std::make_unique<TagNode>(std::string(token::Int), std::move(text));
}

void IR2Ast::visit(const IR::FloatNode &node) {
  auto text = std::make_unique<TextNode>(formatFloat(node.value));
  currentNode = std::make_unique<TagNode>(std::string(token::Float), std::move(text));
}

void IR2Ast::visit(const IR::BoolNode &node) {
  auto text = std::make_unique<TextNode>(std::string(node.value ? token::True : token::False));
  currentNode = std::make_unique<TagNode>(std::string(token::Bool), std::move(text));
}

void IR2Ast::visit(const IR::DateTimeNode &node) {
  auto text = std::make_unique<TextNode>(node.value.toString());
  currentNode = std::make_unique<TagNode>(std::string(token::Time), std::move(text));
}

void IR2Ast::visit(const IR::ListNode &node) {
  TagNode::ChildVector children;
  for (auto &child : node.children) {
    auto element = std::make_unique<TagNode>(std::string(token::Element), toAst(*child));
    children.push_back(std::move(element));
  }
  currentNode = std::make_unique<TagNode>(std::string(token::List), std::move(children));
}

void IR2Ast::visit(const IR::TableNode &node) {
  TagNode::ChildVector children;
  for (auto &kvp : node.children) {
    auto element = std::make_unique<TagNode>(std::string(token::Element), toAst(*(kvp.second)));
    element->attributes[std::string(token::Key)] = kvp.first;
    children.push_back(std::move(element));
  }
  currentNode = std::make_unique<TagNode>(std::string(token::Table), std::move(children));
}

void IR2Ast::visit(const IR::TagNode &node) {
  TagNode::ChildVector children;
  for (const auto &child : node.children) {
    children.push_back(toAst(*child));
  }
  auto tag = std::make_unique<TagNode>(node.name, std::move(children), node.attributes);

  if (isSpecial(*tag)) {
    tag = std::make_unique<TagNode>(std::string(token::Quote), std::move(tag));
  }

  currentNode = std::move(tag);
}

std::unique_ptr<XmlNode> IRConverter::fromIR(const IR::Node &node) {
  IR2Ast converter(tagPatterns);
  return converter.toAst(node);
}

std::unique_ptr<IR::Node> IRConverter::toIR(const XmlNode &node) {
  Ast2IR converter(tagPatterns);
  return converter.toIR(node);
}
