#ifndef __LIBRESER_TOML_PARSER_H__
#define __LIBRESER_TOML_PARSER_H__

#include <istream>
#include <memory>

#include "LibReser/Format.hpp"
#include "LibReser/Parser.hpp"
#include "LibReser/Toml/AST.hpp"

namespace reser {
namespace toml {

class Parser {
 public:
  Parser(std::istream &stream) : state(stream) {}
  static std::unique_ptr<TomlNode> parse(std::istream &stream);

 private:
  std::unique_ptr<TableNode> parseDocument();
  void parseSection(TableNode &section);
  TomlPath parseSectionHeader();

  std::unique_ptr<TomlNode> parseInlineNode();
  std::unique_ptr<StringNode> parseStringNode();
  std::unique_ptr<BoolNode> parseTrueNode();
  std::unique_ptr<BoolNode> parseFalseNode();
  std::unique_ptr<IntNode> parseBaseIntegerNode();
  std::unique_ptr<TomlNode> parseNumberOrDateNode();
  std::unique_ptr<FloatNode> parseFloatSpecial();
  std::unique_ptr<TableNode> parseTableNode();
  std::unique_ptr<ListNode> parseListNode();

  TomlPath parsePath();
  std::string parseKey();
  std::string parseBareKey();
  std::string parseString(bool allowMultiline = true);
  std::string parseQuoteString(bool allowMultiline = true);
  std::string parseAposString(bool allowMultiline = true);
  bool parseLineEnd();

  ParserState state;
};

static_assert(IsParser<Parser, TomlNode>);

};  // namespace toml
};  // namespace reser

#endif  // __LIBRESER_TOML_PARSER_H__
