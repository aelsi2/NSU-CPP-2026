
#include "LibReser/Json/Parser.hpp"

#include <charconv>
#include <cmath>
#include <initializer_list>

#include "LibReser/Parser.hpp"
#include "LibReser/Utils.hpp"

using namespace reser::json;
using namespace reser;
  
std::unique_ptr<AstNode> Parser::parse(std::istream &stream) {
  Parser parser(stream);
  return parser.parseNode();
}

std::unique_ptr<AstNode> Parser::parseNode() {
  state.skipWhitespace();
  switch (state.peek()) {
    case '{':
      return parseTableNode();
    case '[':
      return parseListNode();
    case '"':
      return parseStringNode();
    case 'n':
      return parseNullNode();
    case 't':
      return parseTrueNode();
    case 'f':
      return parseFalseNode();
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return parseIntOrFloatNode();
    default:
      state.unexpectedToken({"{", "[", "null", "true", "false", "number", "string"});
  }
}

std::unique_ptr<NullNode> Parser::parseNullNode() {
  state.parseToken("null");
  return std::make_unique<NullNode>();
}

std::string Parser::parseString() {
  Position start = state.pos();

  std::string string;
  bool lastEscape = false;

  state.parseToken("\"");
  while (true) {
    int ch = state.consume();
    if (ch == state.Eof) {
      state.error("Unterminated string.", start);
    }
    if (ch == '\"' && !lastEscape) {
      break;
    }
    if (ch == '\"') {
      lastEscape = true;
    } else {
      lastEscape = false;
    }
    string.push_back(ch);
  }

  try {
    return jsonUnescape(string);
  } catch (const EscapeError &error) {
    state.error("Invalid escape code in string.", start);
  }
}

std::unique_ptr<StringNode> Parser::parseStringNode() { return std::make_unique<StringNode>(parseString()); }

std::unique_ptr<AstNode> Parser::parseIntOrFloatNode() {
  Position start = state.pos();

  std::string stringNum = state.readChars("0123456789-+.eE");
  const char *stringNumStart = stringNum.data();
  const char *stringNumEnd = stringNum.data() + stringNum.size();

  {
    int64_t value = 0;
    auto [ptr, ec] = std::from_chars(stringNumStart, stringNumEnd, value);
    if (ec == std::errc() && ptr == stringNumEnd) {
      return std::make_unique<IntNode>(value);
    }
  }

  {
    double value = 0.0;
    auto [ptr, ec] = std::from_chars(stringNumStart, stringNumEnd, value);
    if (ec == std::errc() && ptr == stringNumEnd && !std::isnan(value) && !std::isinf(value)) {
      return std::make_unique<FloatNode>(value);
    }
  }

  state.error("Invalid number.", start);
}

std::unique_ptr<BoolNode> Parser::parseTrueNode() {
  state.parseToken("true");
  return std::make_unique<BoolNode>(true);
}

std::unique_ptr<BoolNode> Parser::parseFalseNode() {
  state.parseToken("false");
  return std::make_unique<BoolNode>(false);
}

std::unique_ptr<TableNode> Parser::parseTableNode() {
  std::unique_ptr<TableNode> tableNode = std::make_unique<TableNode>();

  Position start = state.pos();

  state.parseToken("{");
  state.skipWhitespace();
  while (state.peek() != '}') {
    if (state.peek() == state.Eof) {
        state.error("Unterminated table.", start);
    }

    Position keyStart = state.pos();
    std::string key = parseString();
    state.skipWhitespace();
    state.parseToken(":");
    state.skipWhitespace();
    std::unique_ptr<AstNode> value = parseNode();
    state.skipWhitespace();
    
    if (tableNode->children.find(key) != tableNode->children.end()) {
      state.error("Duplicate keys in table", keyStart);
    }
    tableNode->children[key] = std::move(value);

    if (state.peek() == ',') {
      state.consume();
      state.skipWhitespace();
    } else {
      break;
    }
  }
  state.parseToken("}");

  return tableNode;
}
std::unique_ptr<ListNode> Parser::parseListNode() {
  std::unique_ptr<ListNode> listNode = std::make_unique<ListNode>();

  Position start = state.pos();

  state.parseToken("[");
  state.skipWhitespace();
  while (state.peek() != ']') {
    if (state.peek() == state.Eof) {
        state.error("Unterminated list.", start);
    }

    std::unique_ptr<AstNode> value = parseNode();
    state.skipWhitespace();
    
    listNode->children.push_back(std::move(value));

    if (state.peek() == ',') {
      state.consume();
      state.skipWhitespace();
    } else {
      break;
    }
  }
  state.parseToken("]");

  return listNode;
}
