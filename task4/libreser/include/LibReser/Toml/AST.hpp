#ifndef __LIBRESER_TOML_AST_HPP__
#define __LIBRESER_TOML_AST_HPP__

#include <cstdint>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>

#include "LibReser/AST.hpp"
#include "LibReser/DateTime.hpp"

namespace reser {
namespace toml {

class AstVisitor;

struct TomlPath {
  TomlPath() {}
  TomlPath(std::string string) { elements.push_back(std::move(string)); }

  std::vector<std::string> elements;
};

enum class TomlNodeType {
  String,
  Int,
  Float,
  Bool,
  DateTime,
  List,
  Table,
};

using TomlNode = AstNode<TomlNodeType, AstVisitor>;

template <typename T>
concept IsTomlNode = IsAstNode<T, TomlNodeType, AstVisitor>;

template <typename Self, TomlNodeType NodeType>
using TomlNodeImpl = AstNodeImpl<Self, TomlNodeType, AstVisitor, NodeType>;

template <typename Self, TomlNodeType NodeType>
using TomlPrimitiveNode = PrimitiveNode<Self, TomlNodeType, AstVisitor, NodeType>;

using StringNode = TomlPrimitiveNode<std::string, TomlNodeType::String>;
using IntNode = TomlPrimitiveNode<std::int64_t, TomlNodeType::Int>;
using FloatNode = TomlPrimitiveNode<double, TomlNodeType::Float>;
using BoolNode = TomlPrimitiveNode<bool, TomlNodeType::Bool>;
using DateTimeNode = TomlPrimitiveNode<DateTime, TomlNodeType::DateTime>;

class TableNode : public TomlNodeImpl<TableNode, TomlNodeType::Table> {
 public:
  using ChildMap = std::unordered_map<std::string, std::unique_ptr<TomlNode>>;

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

class ListNode : public TomlNodeImpl<ListNode, TomlNodeType::List> {
 public:
  using ChildVector = std::vector<std::unique_ptr<TomlNode>>;

  ListNode() {}
  ListNode(ChildVector children) : children(std::move(children)) {}

  bool isOfTables() const {
    if (children.empty()) {
      return false;
    }
    for (const auto &child : children) {
      if (!child->isa<TableNode>()) {
        return false;
      }
    }
    return true;
  }

  ChildVector children;
};


class AstVisitor {
 public:
  virtual void visit(const StringNode &node) = 0;
  virtual void visit(const IntNode &node) = 0;
  virtual void visit(const FloatNode &node) = 0;
  virtual void visit(const BoolNode &node) = 0;
  virtual void visit(const DateTimeNode &node) = 0;
  virtual void visit(const ListNode &node) = 0;
  virtual void visit(const TableNode &node) = 0;
};

static_assert(IsTomlNode<StringNode>);
static_assert(IsTomlNode<IntNode>);
static_assert(IsTomlNode<FloatNode>);
static_assert(IsTomlNode<BoolNode>);
static_assert(IsTomlNode<DateTimeNode>);
static_assert(IsTomlNode<ListNode>);
static_assert(IsTomlNode<TableNode>);

};  // namespace toml
};  // namespace reser
#endif  // __LIBRESER_TOML_AST_HPP__
