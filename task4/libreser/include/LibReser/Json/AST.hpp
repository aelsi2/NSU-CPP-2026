#ifndef __LIBRESER_JSON_AST_HPP__
#define __LIBRESER_JSON_AST_HPP__

#include <cstdint>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>

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

template <typename Self, JsonNodeType NodeType>
using JsonPrimitiveNode = PrimitiveNode<Self, JsonNodeType, AstVisitor, NodeType>;

class NullNode : public JsonNodeImpl<NullNode, JsonNodeType::Null> {};
using StringNode = JsonPrimitiveNode<std::string, JsonNodeType::String>;
using IntNode = JsonPrimitiveNode<std::int64_t, JsonNodeType::Int>;
using FloatNode = JsonPrimitiveNode<double, JsonNodeType::Float>;
using BoolNode = JsonPrimitiveNode<bool, JsonNodeType::Bool>;

class ListNode : public JsonNodeImpl<ListNode, JsonNodeType::List> {
 public:
  using ChildVector = std::vector<std::unique_ptr<JsonNode>>;

  ListNode() {}
  ListNode(ChildVector children) : children(std::move(children)) {}

  ChildVector children;
};

class TableNode : public JsonNodeImpl<TableNode, JsonNodeType::Table> {
 public:
  using ChildMap = std::unordered_map<std::string, std::unique_ptr<JsonNode>>;

  TableNode() {}
  TableNode(ChildMap children) : children(std::move(children)) {}

  template <typename T>
  const T *getValue(const std::string_view name) const {
    auto child = children.find(std::string(name));
    return child == children.end() ? nullptr : child->second->as<T>();
  }
  inline size_t nValues() const { return children.size(); }

  ChildMap children;
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
