#ifndef __LIBRESER_JSON_AST_HPP__
#define __LIBRESER_JSON_AST_HPP__

#include <concepts>
#include <cstdint>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>

namespace reser {
namespace json {

class AstVisitor;

enum class AstNodeType {
  Null,
  String,
  Int,
  Float,
  Bool,
  List,
  Table,
};

class AstNode;

template <typename T>
concept IsAstNode = requires {
  { T::Type } -> std::same_as<const AstNodeType &>;
  requires std::derived_from<T, AstNode>;
};

class AstNode {
 public:
  virtual ~AstNode() = default;
  virtual void accept(AstVisitor &) const = 0;

  template <IsAstNode T>
  inline bool isa() const {
    return type() == T::Type;
  }

  template <IsAstNode T>
  inline const T *as() const {
    return isa<T>() ? static_cast<const T *>(this) : nullptr;
  }

 protected:
  virtual AstNodeType type() const = 0;
};

template <typename Self, AstNodeType NodeType>
class AstNodeImpl : public AstNode {
 public:
  static constexpr AstNodeType Type = NodeType;
  void accept(AstVisitor &visitor) const override;

 protected:
  AstNodeType type() const override { return Type; }
};

template <typename Primitive, AstNodeType NodeType>
class PrimitiveNode : public AstNodeImpl<PrimitiveNode<Primitive, NodeType>, NodeType> {
 public:
  PrimitiveNode(Primitive value) : value(std::move(value)) {}

  Primitive value;
};

class NullNode : public AstNodeImpl<NullNode, AstNodeType::Null> {};
using StringNode = PrimitiveNode<std::string, AstNodeType::String>;
using IntNode = PrimitiveNode<std::int64_t, AstNodeType::Int>;
using FloatNode = PrimitiveNode<double, AstNodeType::Float>;
using BoolNode = PrimitiveNode<bool, AstNodeType::Bool>;

class ListNode : public AstNodeImpl<ListNode, AstNodeType::List> {
 public:
  using ChildVector = std::vector<std::unique_ptr<AstNode>>;

  ListNode() {}
  ListNode(ChildVector children) : children(std::move(children)) {}

  ChildVector children;
};

class TableNode : public AstNodeImpl<TableNode, AstNodeType::Table> {
 public:
  using ChildMap = std::unordered_map<std::string, std::unique_ptr<AstNode>>;

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

template <typename Self, AstNodeType NodeType>
void AstNodeImpl<Self, NodeType>::accept(AstVisitor &visitor) const {
  visitor.visit(*static_cast<const Self *>(this));
}

static_assert(IsAstNode<NullNode>);
static_assert(IsAstNode<StringNode>);
static_assert(IsAstNode<IntNode>);
static_assert(IsAstNode<FloatNode>);
static_assert(IsAstNode<BoolNode>);
static_assert(IsAstNode<ListNode>);
static_assert(IsAstNode<TableNode>);

};  // namespace json
};  // namespace reser
#endif  // __LIBRESER_JSON_AST_HPP__
