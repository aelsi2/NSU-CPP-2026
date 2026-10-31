#include "LibReser/Xml/Parser.hpp"

#include "LibReser/Parser.hpp"
#include "LibReser/Utils.hpp"
#include "LibReser/Xml/AST.hpp"

using namespace reser::xml;
using namespace reser;

std::unique_ptr<XmlNode> Parser::parse(std::istream &stream) {
  Parser parser(stream);
  return parser.parseDocument();
}

std::unique_ptr<TagNode> Parser::parseDocument() {
  if (state.match2('<', '?')) {
    parseProlog();
  }
  while (state.peek()) {
    state.skipWhitespace();
    if (state.match2('<', '!')) {
      parseComment();
    } else if (state.match('<')) {
      break;
    }
  }
  return parseTagNode();
}

std::unique_ptr<XmlNode> Parser::parseNode() {
  while (state.peek()) {
    if (state.match2('<', '!')) {
      parseComment();
    } else if (state.match('<')) {
      return parseTagNode();
    } else {
      return parseTextNode();
    }
  }
  state.error("Expected node.");
}

std::unique_ptr<TagNode> Parser::parseTagNode() {
  Position start = state.pos();

  state.parseToken("<");
  state.skipWhitespace();
  std::unique_ptr<TagNode> node = std::make_unique<TagNode>(parseName());

  bool headParsed = false;
  while (!headParsed) {
    state.skipWhitespace();
    if (!state.peek()) {
      state.error("Unterminated tag.", start);
    }
    switch (*state.peek()) {
      case '/':
        state.parseToken("/>");
        return node;
      case '>':
        headParsed = true;
        state.consume();
        break;
      default:
        Position attributeStart = state.pos();
        auto [key, value] = parseAttribute();
        if (node->attributes.find(key) != node->attributes.end()) {
          state.error("Duplicate attributes.", attributeStart);
        }
        node->attributes[key] = value;
        break;
    }
  }

  while (true) {
    if (state.match2('<', '/')) {
      break;
    }
    node->children.push_back(parseNode());
  }

  node->normalize();

  state.parseToken("</");
  state.skipWhitespace();
  Position closingNameStart = state.pos();
  std::string closingName = parseName();
  if (node->name != closingName) {
    state.error("Closing tag name does not match the name of the opening tag.", closingNameStart);
  }
  state.skipWhitespace();
  state.parseToken(">");
  return node;
}

std::unique_ptr<TextNode> Parser::parseTextNode() {
  Position start = state.pos();
  std::string text = state.readUntilAny("<");
  try {
    return std::make_unique<TextNode>(xmlUnescape(text));
  } catch (const EscapeError &error) {
    state.error("Invalid escape code in text.", start);
  }
}

void Parser::parseComment() {
  Position start = state.pos();

  state.parseToken("<!--");

  size_t dashCount = 0;
  while (true) {
    auto ch = state.consume();
    if (!ch) {
      state.error("Unterminated comment.", start);
    }
    if (ch == '-') {
      dashCount++;
      dashCount &= 3;
    } else if (ch == '>' && dashCount >= 2) {
      break;
    } else {
      dashCount = 0;
    }
  }
}

void Parser::parseProlog() {
  Position start = state.pos();

  state.parseToken("<?xml");
  while (true) {
    state.skipWhitespace();
    if (!state.peek()) {
      state.error("Unterminated prolog.", start);
    }
    switch (*state.peek()) {
      case '?':
        state.parseToken("?>");
        return;
      default:
        parseAttribute();
        break;
    }
  }
}

std::string Parser::parseName() {
  Position start = state.pos();

  std::string name;

  if (!state.peek()) {
    state.error("Expected name.", start);
  }
  if (std::isalpha(*state.peek(), std::locale::classic()) || state.peek() == '_' || state.peek() == ':') {
    name.push_back(*state.consume());
  } else {
    state.error("Names must start with a letter or an underscore ('_').", start);
  }

  while (true) {
    auto ch = state.peek();
    if (!ch) {
      break;
    }
    if (std::isalnum(*ch, std::locale::classic()) || ch == '.' || ch == '_' || ch == '-' || ch == ':') {
      name.push_back(*state.consume());
    } else {
      break;
    }
  }

  return name;
}

std::pair<std::string, std::string> Parser::parseAttribute() {
  std::string name = parseName();
  state.skipWhitespace();
  state.parseToken("=");
  state.skipWhitespace();
  std::string value = parseString();
  return {name, value};
}

std::string Parser::parseString() {
  Position start = state.pos();
  const char *terminator;
  if (state.match('\'')) {
    terminator = "'";
  } else {
    terminator = "\"";
  }
  state.parseToken(terminator);
  std::string string = state.readUntilAny(terminator);
  state.parseToken(terminator);

  try {
    return xmlUnescape(string);
  } catch (const EscapeError &error) {
    state.error("Invalid escape code in string.", start);
  }
}
