#ifndef __LIBRESER_JSON_PARSER_H__
#define __LIBRESER_JSON_PARSER_H__

#include <istream>
#include <memory>

#include "LibReser/Parser.hpp"
#include "LibReser/Format.hpp"
#include "LibReser/Json/AST.hpp"

namespace reser {
namespace json {

class Parser {
 public:
  Parser(std::istream &stream) : state(stream) {}
  static std::unique_ptr<AstNode> parse(std::istream &stream);

 private:
  std::string parseString();
  std::unique_ptr<AstNode> parseNode();
  std::unique_ptr<NullNode> parseNullNode();
  std::unique_ptr<StringNode> parseStringNode();
  std::unique_ptr<AstNode> parseIntOrFloatNode();
  std::unique_ptr<BoolNode> parseTrueNode();
  std::unique_ptr<BoolNode> parseFalseNode();
  std::unique_ptr<TableNode> parseTableNode();
  std::unique_ptr<ListNode> parseListNode();

  ParserState state;
};

static_assert(IsParser<Parser, AstNode>);

};  // namespace json
};  // namespace reser

#endif  // __LIBRESER_JSON_PARSER_H__
