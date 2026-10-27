#ifndef __LIBRESER_FORMAT_HPP__
#define __LIBRESER_FORMAT_HPP__

#include <concepts>
#include <istream>
#include <memory>

#include "IR.hpp"

namespace reser {

template <class T, class AST>
concept IsPrinter = requires(std::ostream &ostream, const AST &ast) {
  { T::print(ostream, ast) } -> std::same_as<void>;
};

template <class T, class AST>
concept IsParser = requires(std::istream &istream) {
  { T::parse(istream) } -> std::same_as<std::unique_ptr<AST>>;
};

template <class T, class AST>
concept IsIRConverter = requires(const IR::Node &irNode, const AST &ast) {
  { T::fromIR(irNode) } -> std::same_as<std::unique_ptr<AST>>;
  { T::toIR(ast) } -> std::same_as<std::unique_ptr<IR::Node>>;
};

};  // namespace reser

#endif  // __LIBRESER_FORMAT_HPP__
