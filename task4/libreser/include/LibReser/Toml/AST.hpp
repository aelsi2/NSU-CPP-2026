#ifndef __LIBRESER_TOML_AST_HPP__
#define __LIBRESER_TOML_AST_HPP__

#include <cstdint>
#include <memory>
#include <vector>

#include "LibReser/AST.hpp"
#include "LibReser/DateTime.hpp"

namespace reser {
namespace toml {

struct TomlPathSlice;

struct TomlPath {
  TomlPath() : keys(), isArrayOfTables(false) {}
  TomlPath(std::vector<std::string> keys, bool isArrayOfTables = false)
      : keys(std::move(keys)), isArrayOfTables(isArrayOfTables) {}
  TomlPath(std::string path, bool isArrayOfTables = false) : keys{std::move(path)}, isArrayOfTables(isArrayOfTables) {}

  bool empty() const { return keys.empty(); }
  bool simple() const { return keys.size() == 1; }

  TomlPath concat(const TomlPathSlice &slice) const;

  void push_back(std::string value) { keys.push_back(std::move(value)); }
  void pop_back() { keys.pop_back(); }

  std::vector<std::string> keys;
  bool isArrayOfTables;
};

struct TomlPathSlice {
  TomlPathSlice() : keys(), isArrayOfTables(false) {}
  TomlPathSlice(std::span<const std::string> keys, bool isArray = false) : keys(keys), isArrayOfTables(isArray) {}
  TomlPathSlice(const TomlPath &path) : keys(path.keys), isArrayOfTables(path.isArrayOfTables) {}

  explicit operator TomlPath() const { return TomlPath(std::vector(keys.begin(), keys.end()), isArrayOfTables); }

  bool empty() const { return keys.empty(); }
  bool simple() const { return keys.size() == 1; }

  TomlPath concat(const TomlPathSlice &slice) const;

  const std::string *head() const { return keys.empty() ? nullptr : &keys[0]; }
  TomlPathSlice tail() const { return TomlPathSlice(keys.subspan(keys.empty() ? 0 : 1), isArrayOfTables); }

  std::span<const std::string> keys;
  bool isArrayOfTables;
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

class AstVisitor;

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

class TableNode : public TableNodeImpl<TableNode, TomlNodeType, AstVisitor, TomlNodeType::Table> {
  using TableNodeImpl<TableNode, TomlNodeType, AstVisitor, TomlNodeType::Table>::TableNodeImpl;

 public:
  TableNode *getSection(TomlPathSlice path);
  bool putValue(TomlPathSlice path, std::unique_ptr<TomlNode> value);
};

class ListNode : public ListNodeImpl<ListNode, TomlNodeType, AstVisitor, TomlNodeType::List> {
  using ListNodeImpl<ListNode, TomlNodeType, AstVisitor, TomlNodeType::List>::ListNodeImpl;

 public:
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

  TableNode *getSection(TomlPathSlice path);
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
