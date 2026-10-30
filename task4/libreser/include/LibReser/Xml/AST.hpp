#ifndef __LIBRESER_XML_AST_HPP__
#define __LIBRESER_XML_AST_HPP__

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

template <typename Self, XmlNodeType NodeType>
using XmlPrimitiveNode = PrimitiveNode<Self, XmlNodeType, AstVisitor, NodeType>;

using TextNode = XmlPrimitiveNode<std::string, XmlNodeType::Text>;

class TagNode : public XmlNodeImpl<TagNode, XmlNodeType::Tag> {
 public:
  using ChildVector = std::vector<std::unique_ptr<XmlNode>>;
  using AttrMap = std::unordered_map<std::string, std::string>;

  TagNode(std::string name, ChildVector children, AttrMap attributes)
      : name(std::move(name)), children(std::move(children)), attributes(std::move(attributes)) {}

  bool hasText() const {
    for (const auto &child : children) {
      if (child->isa<TextNode>()) {
        return true;
      }
    }
    return false;
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
