#ifndef __LIBRESER_JSON_AST_HPP__
#define __LIBRESER_JSON_AST_HPP__

#include <cstdint>

#include "LibReser/AST.hpp"

namespace reser {
namespace json {

class AstVisitor;

enum class JsonNodeType {
  Null,
  String,
  Int,
  Float,
  Bool,
  List,
  Table,
};

using JsonNode = AstNode<JsonNodeType, AstVisitor>;

template <typename T>
concept IsJsonNode = IsAstNode<T, JsonNodeType, AstVisitor>;

template <typename Self, JsonNodeType NodeType>
using JsonNodeImpl = AstNodeImpl<Self, JsonNodeType, AstVisitor, NodeType>;

template <typename Primitive, JsonNodeType NodeType>
using JsonPrimitiveNode = PrimitiveNode<Primitive, JsonNodeType, AstVisitor, NodeType>;

class NullNode : public JsonNodeImpl<NullNode, JsonNodeType::Null> {};
using StringNode = JsonPrimitiveNode<std::string, JsonNodeType::String>;
using IntNode = JsonPrimitiveNode<std::int64_t, JsonNodeType::Int>;
using FloatNode = JsonPrimitiveNode<double, JsonNodeType::Float>;
using BoolNode = JsonPrimitiveNode<bool, JsonNodeType::Bool>;
class ListNode : public ListNodeImpl<ListNode, JsonNodeType, AstVisitor, JsonNodeType::List> {
  using ListNodeImpl<ListNode, JsonNodeType, AstVisitor, JsonNodeType::List>::ListNodeImpl;
};
class TableNode : public TableNodeImpl<TableNode, JsonNodeType, AstVisitor, JsonNodeType::Table> {
  using TableNodeImpl<TableNode, JsonNodeType, AstVisitor, JsonNodeType::Table>::TableNodeImpl;
};

class AstVisitor {
 public:
  virtual void visit(const NullNode &node) = 0;
  virtual void visit(const StringNode &node) = 0;
  virtual void visit(const IntNode &node) = 0;
  virtual void visit(const FloatNode &node) = 0;
  virtual void visit(const BoolNode &node) = 0;
  virtual void visit(const ListNode &node) = 0;
  virtual void visit(const TableNode &node) = 0;
};

static_assert(IsJsonNode<NullNode>);
static_assert(IsJsonNode<StringNode>);
static_assert(IsJsonNode<IntNode>);
static_assert(IsJsonNode<FloatNode>);
static_assert(IsJsonNode<BoolNode>);
static_assert(IsJsonNode<ListNode>);
static_assert(IsJsonNode<TableNode>);

};  // namespace json
};  // namespace reser
#endif  // __LIBRESER_JSON_AST_HPP__
