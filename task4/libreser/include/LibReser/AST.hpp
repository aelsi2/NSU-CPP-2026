#ifndef __LIBRESER_AST_HPP__
#define __LIBRESER_AST_HPP__

#include <algorithm>
#include <concepts>
#include <type_traits>
namespace reser {

template <typename AstNodeType, typename Visitor>
class AstNode;

template <typename T, typename AstNodeType, typename AstVisitor>
concept IsAstConcreteNode = requires {
  { T::Type } -> std::same_as<const AstNodeType &>;
  requires std::derived_from<T, AstNode<AstNodeType, AstVisitor>>;
};

template <typename T, typename AstNodeType, typename AstVisitor>
concept IsAstBaseNode = std::derived_from<AstNode<AstNodeType, AstVisitor>, T>;

template <typename T, typename AstNodeType, typename AstVisitor>
concept IsAstNode = IsAstBaseNode<T, AstNodeType, AstVisitor> || IsAstConcreteNode<T, AstNodeType, AstVisitor>;

template <typename AstNodeType, typename AstVisitor>
class AstNode {
 public:
  virtual ~AstNode() = default;
  virtual void accept(AstVisitor &) const = 0;

  template <IsAstNode<AstNodeType, AstVisitor> T>
  inline bool isa() const {
    if constexpr (std::is_same_v<T, AstNode<AstNodeType, AstVisitor>>) {
      return true;
    } else {
      return type() == T::Type;
    }
  }

  template <IsAstNode<AstNodeType, AstVisitor> T>
  inline const T *as() const {
    return isa<T>() ? static_cast<const T *>(this) : nullptr;
  }

 protected:
  virtual AstNodeType type() const = 0;
};

template <typename Self, typename AstNodeType, typename AstVisitor, AstNodeType NodeType>
class AstNodeImpl : public AstNode<AstNodeType, AstVisitor> {
 public:
  static constexpr AstNodeType Type = NodeType;
  void accept(AstVisitor &visitor) const override;

 protected:
  AstNodeType type() const override { return Type; }
};

template <typename Primitive, typename AstNodeType, typename AstVisitor, AstNodeType NodeType>
class PrimitiveNode : public AstNodeImpl<PrimitiveNode<Primitive, AstNodeType, AstVisitor, NodeType>, AstNodeType,
                                         AstVisitor, NodeType> {
 public:
  PrimitiveNode(Primitive value) : value(std::move(value)) {}

  Primitive value;
};

template <typename Self, typename AstNodeType, typename AstVisitor, AstNodeType NodeType>
void AstNodeImpl<Self, AstNodeType, AstVisitor, NodeType>::accept(AstVisitor &visitor) const {
  visitor.visit(*static_cast<const Self *>(this));
}

};  // namespace reser
#endif  // __LIBRESER_AST_HPP__
