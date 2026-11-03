#ifndef __LIBRESER_XML_AST_HPP__
#define __LIBRESER_XML_AST_HPP__

#include <cctype>
#include <memory>
#include <map>
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
      if (!std::isspace(ch, std::locale::classic())) {
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
  using AttrMap = std::map<std::string, std::string>;

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
  bool empty() const;

  bool hasText() const;

  template <IsXmlNode T>
  const T *getChild(size_t index = 0) const {
    if (children.size() <= index) {
      return nullptr;
    }
    return children[index]->template as<T>();
  }

  template <IsXmlNode T>
  T *getChild(size_t index = 0) {
    if (children.size() <= index) {
      return nullptr;
    }
    return children[index]->template as<T>();
  }

  void normalize();

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
