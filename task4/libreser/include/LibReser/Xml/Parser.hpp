#ifndef __LIBRESER_XML_PARSER_H__
#define __LIBRESER_XML_PARSER_H__

#include <istream>
#include <memory>

#include "LibReser/Format.hpp"
#include "LibReser/Parser.hpp"
#include "LibReser/Xml/AST.hpp"

namespace reser {
namespace xml {

class Parser {
 public:
  Parser(std::istream &stream) : state(stream) {}
  static std::unique_ptr<XmlNode> parse(std::istream &stream);

 private:
  std::string parseString();
  std::unique_ptr<XmlNode> parseNode();
  std::unique_ptr<TagNode> parseTagNode();
  std::unique_ptr<TextNode> parseTextNode();

  ParserState state;
};

static_assert(IsParser<Parser, XmlNode>);

};  // namespace xml
};  // namespace reser

#endif  // __LIBRESER_XML_PARSER_H__
