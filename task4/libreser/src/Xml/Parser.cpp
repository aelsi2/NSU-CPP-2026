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
  while (state.peek() != state.Eof) {
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
  while (state.peek() != state.Eof) {
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
    switch (state.peek()) {
      case ParserState::Eof:
        state.error("Unterminated tag.", start);
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

  if (node->children.size() > 1) {
    bool allSpace = true;
    for (const auto &child : node->children) {
      if (child->isa<TextNode>() && !child->as<TextNode>()->isSpace()) {
        allSpace = false;
        break;
      }
    }
    if (allSpace) {
      for (auto iter = node->children.begin(); iter != node->children.end();) {
        if ((*iter)->isa<TextNode>()) {
          iter = node->children.erase(iter);
        } else {
          iter++;
        }
      }
    }
  }

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

std::unique_ptr<TextNode> Parser::parseTextNode() { return std::make_unique<TextNode>(state.readUntilAny("<")); }

void Parser::parseComment() {
  Position start = state.pos();

  state.parseToken("<!--");

  size_t dashCount = 0;
  while (true) {
    int ch = state.consume();
    if (ch == state.Eof) {
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
    switch (state.peek()) {
      case ParserState::Eof:
        state.error("Unterminated prolog.", start);
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

  if (state.peek() == state.Eof) {
    state.error("Expected name.", start);
  }
  if (std::isalpha(state.peek()) || state.peek() == '_' || state.peek() == ':') {
    name.push_back(state.consume());
  } else {
    state.error("Names must start with a letter or an underscore ('_').", start);
  }

  while (true) {
    int ch = state.peek();
    if (ch == state.Eof) {
      break;
    }
    if (std::isalpha(ch) || std::isdigit(ch) || ch == '.' || ch == '_' || ch == '-' || ch == ':') {
      name.push_back(state.consume());
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

  state.parseToken("\"");
  std::string string = state.readUntilAny("\"");
  state.parseToken("\"");

  try {
    return xmlUnescape(string);
  } catch (const EscapeError &error) {
    state.error("Invalid escape code in string.", start);
  }
}
