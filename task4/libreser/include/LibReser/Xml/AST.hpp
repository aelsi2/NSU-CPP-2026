#ifndef __LIBRESER_XML_AST_HPP__
#define __LIBRESER_XML_AST_HPP__

#include <cctype>
#include <memory>
#include <unordered_map>
#include <vector>

#include "LibReser/AST.hpp"

namespace reser {
namespace xml {

class AstVisitor;

enum class XmlNodeType {
  Text,
  Tag,
};

using XmlNode = AstNode<XmlNodeType, AstVisitor>;

template <typename T>
concept IsXmlNode = IsAstNode<T, XmlNodeType, AstVisitor>;

template <typename Self, XmlNodeType NodeType>
using XmlNodeImpl = AstNodeImpl<Self, XmlNodeType, AstVisitor, NodeType>;

class TextNode : public XmlNodeImpl<TextNode, XmlNodeType::Text> {
 public:
  TextNode(std::string value) : value(std::move(value)) {}

  bool isSpace() const {
    for (const char &ch : value) {
      if (!std::isspace(ch)) {
        return false;
      }
    }
    return true;
  }

  std::string value;
};

class TagNode : public XmlNodeImpl<TagNode, XmlNodeType::Tag> {
 public:
  using ChildVector = std::vector<std::unique_ptr<XmlNode>>;
  using AttrMap = std::unordered_map<std::string, std::string>;

  TagNode(std::string name, ChildVector children, AttrMap attributes)
      : name(std::move(name)), children(std::move(children)), attributes(std::move(attributes)) {
    normalize();
  }
  TagNode(std::string name, ChildVector children) : name(std::move(name)), children(std::move(children)) {
    normalize();
  }
  TagNode(std::string name) : name(std::move(name)) {}
  TagNode(std::string name, std::unique_ptr<XmlNode> child) : name(std::move(name)) {
    children.push_back(std::move(child));
    normalize();
  }

  size_t nAttributes() const { return attributes.size(); }
  size_t nChildren() const { return children.size(); }

  template <IsXmlNode T>
  const T *getChild() const {
    return children.empty() ? nullptr : children[0]->as<T>();
  }

  template <XmlNode>
  const XmlNode *getChild() const {
    return children.empty() ? nullptr : &*children[0];
  }

  bool hasText() const {
    for (const auto &child : children) {
      if (child->isa<TextNode>()) {
        return true;
      }
    }
    return false;
  }

  void normalize() {
    if (children.size() > 1) {
      bool allSpace = true;
      for (const auto &child : children) {
        if (child->isa<TextNode>() && !child->as<TextNode>()->isSpace()) {
          allSpace = false;
          break;
        }
      }
      if (allSpace) {
        for (auto iter = children.begin(); iter != children.end();) {
          if ((*iter)->isa<TextNode>()) {
            iter = children.erase(iter);
          } else {
            iter++;
          }
        }
      }
    }
    if (children.size() == 1 && getChild<TextNode>()) {
      if (getChild<TextNode>()->value.empty()) {
        children.clear();
      }
    }
  }

  std::string name;
  ChildVector children;
  AttrMap attributes;
};

class AstVisitor {
 public:
  virtual void visit(const TextNode &node) = 0;
  virtual void visit(const TagNode &node) = 0;
};

static_assert(IsXmlNode<TextNode>);
static_assert(IsXmlNode<TagNode>);

};  // namespace xml
};  // namespace reser
#endif  // __LIBRESER_XML_AST_HPP__
