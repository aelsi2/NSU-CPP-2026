#ifndef __LIBRESER_IR_HPP__
#define __LIBRESER_IR_HPP__

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace reser {
namespace IR {

class Visitor;

class Node {
 public:
  virtual ~Node() = default;
  virtual void accept(Visitor &visitor) const = 0;
};

template <typename Self>
class NodeImpl : public Node {
 public:
  void accept(Visitor &visitor) const override;
};

template <typename T>
class PrimitiveNode : public NodeImpl<PrimitiveNode<T>> {
 public:
  PrimitiveNode(T value) : value(std::move(value)) {}

  T value;
};

class NullNode : public NodeImpl<NullNode> {};
using StringNode = PrimitiveNode<std::string>;
using IntNode = PrimitiveNode<std::int64_t>;
using FloatNode = PrimitiveNode<double>;
using BoolNode = PrimitiveNode<bool>;

class TableNode : public NodeImpl<TableNode> {
 public:
  using ChildMap = std::unordered_map<std::string, std::unique_ptr<Node>>;

  TableNode(ChildMap map) : children(std::move(map)) {}

  ChildMap children;
};

class ListNode : public NodeImpl<ListNode> {
 public:
  using ChildVector = std::vector<std::unique_ptr<Node>>;

  ListNode(ChildVector vector) : children(std::move(vector)) {}

  ChildVector children;
};

class TagNode : public NodeImpl<TagNode> {
 public:
  using AttrMap = std::unordered_map<std::string, std::string>;
  using ChildVector = std::vector<std::unique_ptr<Node>>;

  TagNode(std::string name, ChildVector children, AttrMap attributes)
      : name(std::move(name)), children(std::move(children)), attributes(std::move(attributes)) {}

  std::string name;
  ChildVector children;
  AttrMap attributes;
};

class Visitor {
 public:
  virtual void visit(const NullNode &node) = 0;
  virtual void visit(const StringNode &node) = 0;
  virtual void visit(const IntNode &node) = 0;
  virtual void visit(const FloatNode &node) = 0;
  virtual void visit(const BoolNode &node) = 0;
  virtual void visit(const TableNode &node) = 0;
  virtual void visit(const ListNode &node) = 0;
  virtual void visit(const TagNode &node) = 0;
};

template <typename Self>
void NodeImpl<Self>::accept(Visitor &visitor) const {
  visitor.visit(*static_cast<const Self *>(this));
}

};  // namespace IR

};  // namespace reser

#endif  // __LIBRESER_IR_HPP__
