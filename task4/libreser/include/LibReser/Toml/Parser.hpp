#ifndef __LIBRESER_TOML_PARSER_H__
#define __LIBRESER_TOML_PARSER_H__

#include <istream>
#include <memory>

#include "LibReser/Parser.hpp"
#include "LibReser/Format.hpp"
#include "LibReser/Toml/AST.hpp"

namespace reser {
namespace toml {

class Parser {
 public:
  Parser(std::istream &stream) : state(stream) {}
  static std::unique_ptr<TomlNode> parse(std::istream &stream);

 private:
  std::string parseString();
  std::unique_ptr<TomlNode> parseNode();
  std::unique_ptr<StringNode> parseStringNode();
  std::unique_ptr<TomlNode> parseIntOrFloatNode();
  std::unique_ptr<BoolNode> parseTrueNode();
  std::unique_ptr<BoolNode> parseFalseNode();
  std::unique_ptr<TableNode> parseTableNode();
  std::unique_ptr<ListNode> parseListNode();

  ParserState state;
};

static_assert(IsParser<Parser, TomlNode>);

};  // namespace toml
};  // namespace reser

#endif  // __LIBRESER_TOML_PARSER_H__
