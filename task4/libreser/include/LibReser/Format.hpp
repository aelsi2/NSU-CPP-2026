#ifndef __LIBRESER_FORMAT_HPP__
#define __LIBRESER_FORMAT_HPP__

#include <concepts>
#include <istream>
#include <memory>

#include "IR.hpp"

namespace reser {

template <typename T, typename AST>
concept IsPrinter = requires(std::ostream &ostream, const AST &ast) {
  { T::print(ostream, ast) } -> std::same_as<void>;
};

template <typename T, typename AST>
concept IsParser = requires(std::istream &istream) {
  { T::parse(istream) } -> std::same_as<std::unique_ptr<AST>>;
};

template <typename T, typename AST>
concept IsIRConverter = requires(const IR::Node &irNode, const AST &ast) {
  { T::fromIR(irNode) } -> std::same_as<std::unique_ptr<AST>>;
  { T::toIR(ast) } -> std::same_as<std::unique_ptr<IR::Node>>;
};

struct FormatBackend {
  virtual std::unique_ptr<IR::Node> fromStream(std::istream &stream) const = 0;
  virtual void toStream(std::ostream &stream, IR::Node &node) const = 0;
};

template <typename AST, IsPrinter<AST> Printer, IsParser<AST> Parser, IsIRConverter<AST> IRConverter>
struct FormatBackendImpl : FormatBackend {
  static FormatBackendImpl<AST, Printer, Parser, IRConverter> Instance;

  std::unique_ptr<IR::Node> fromStream(std::istream &stream) const override {
    auto node = Parser::parse(stream);
    return IRConverter::toIR(*node);
  }

  void toStream(std::ostream &stream, IR::Node &node) const override {
    auto ast = IRConverter::fromIR(node);
    Printer::print(stream, *ast);
  }
};

};  // namespace reser

#endif  // __LIBRESER_FORMAT_HPP__
