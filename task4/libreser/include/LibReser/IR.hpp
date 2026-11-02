#ifndef __LIBRESER_IR_HPP__
#define __LIBRESER_IR_HPP__

#include <cstdint>
#include <string>
#include <unordered_map>

#include "LibReser/AST.hpp"
#include "LibReser/DateTime.hpp"

namespace reser {
namespace IR {

enum class NodeType {
  Null,
  String,
  Int,
  Float,
  Bool,
  DateTime,
  List,
  Table,
  Tag,
};

class Visitor;

using Node = AstNode<NodeType, Visitor>;

template <typename T>
concept IsNode = IsAstNode<T, NodeType, Visitor>;

template <typename Self, NodeType Type>
using NodeImpl = AstNodeImpl<Self, NodeType, Visitor, Type>;

template <typename Self, NodeType Type>
using PrimitiveNode = PrimitiveNode<Self, NodeType, Visitor, Type>;

class NullNode : public NodeImpl<NullNode, NodeType::Null> {};
using StringNode = PrimitiveNode<std::string, NodeType::String>;
using IntNode = PrimitiveNode<std::int64_t, NodeType::Int>;
using FloatNode = PrimitiveNode<double, NodeType::Float>;
using BoolNode = PrimitiveNode<bool, NodeType::Bool>;
using DateTimeNode = PrimitiveNode<DateTime, NodeType::DateTime>;
class ListNode : public ListNodeImpl<ListNode, NodeType, Visitor, NodeType::List> {
  using ListNodeImpl<ListNode, NodeType, Visitor, NodeType::List>::ListNodeImpl;
};
class TableNode : public TableNodeImpl<TableNode, NodeType, Visitor, NodeType::Table> {
  using TableNodeImpl<TableNode, NodeType, Visitor, NodeType::Table>::TableNodeImpl;
};
class TagNode : public ListNodeImpl<TagNode, NodeType, Visitor, NodeType::Tag> {
 public:
  using AttrMap = std::unordered_map<std::string, std::string>;

  TagNode(std::string name, ChildVector children, AttrMap attributes)
      : ListNodeImpl<TagNode, NodeType, Visitor, NodeType::Tag>(std::move(children)),
        name(std::move(name)),
        attributes(std::move(attributes)) {}
  TagNode(std::string name, ChildVector children)
      : ListNodeImpl<TagNode, NodeType, Visitor, NodeType::Tag>(std::move(children)), name(std::move(name)) {}
  TagNode(std::string name) : name(std::move(name)) {}

  std::string name;
  AttrMap attributes;
};

class Visitor {
 public:
  virtual void visit(const NullNode &node) = 0;
  virtual void visit(const StringNode &node) = 0;
  virtual void visit(const IntNode &node) = 0;
  virtual void visit(const FloatNode &node) = 0;
  virtual void visit(const BoolNode &node) = 0;
  virtual void visit(const DateTimeNode &node) = 0;
  virtual void visit(const TableNode &node) = 0;
  virtual void visit(const ListNode &node) = 0;
  virtual void visit(const TagNode &node) = 0;
};

};  // namespace IR

};  // namespace reser

#endif  // __LIBRESER_IR_HPP__
