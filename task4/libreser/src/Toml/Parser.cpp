#include "LibReser/Toml/Parser.hpp"

#include <charconv>
#include <cmath>
#include <initializer_list>
#include <limits>
#include <locale>

#include "LibReser/Parser.hpp"
#include "LibReser/Utils.hpp"

using namespace reser::toml;
using namespace reser;

std::unique_ptr<TomlNode> Parser::parse(std::istream &stream) {
  Parser parser(stream);
  return parser.parseDocument();
}

std::unique_ptr<TableNode> Parser::parseDocument() {
  auto rootSection = std::make_unique<TableNode>();
  parseSection(*rootSection);

  while (state.peek() == '[') {
    Position sectionPos = state.pos();
    TomlPath path = parseSectionHeader();

    if (auto section = rootSection->getSection(path)) {
      parseSection(*section);
    } else {
      state.error("Conflicting sections.", sectionPos);
    }
  }

  return rootSection;
}

void Parser::parseSection(TableNode &section) {
  parseLineEnd();
  while (auto ch = state.peek()) {
    if (ch == '[') {
      break;
    }
    Position keyStart = state.pos();

    TomlPath childPath = parsePath();
    state.skipWhitespace(false);
    state.parseToken("=");
    state.skipWhitespace(false);
    std::unique_ptr<TomlNode> value = parseInlineNode();

    if (!section.putValue(childPath, std::move(value))) {
      state.error("Conflicting key in section.", keyStart);
    }

    if (!parseLineEnd()) {
      state.error("Section key/value pairs must be on separate lines.");
    }
  }
}

TomlPath Parser::parseSectionHeader() {
  Position start = state.pos();

  bool array;
  if (state.peek2() == '[') {
    state.parseToken("[[");
    array = true;
  } else {
    state.parseToken("[");
    array = false;
  }

  state.skipWhitespace(false);
  TomlPath path = parsePath();
  path.isArrayOfTables = array;
  state.skipWhitespace(false);

  if (!state.peek()) {
    state.error("Unterminated section header", start);
  }

  if (array) {
    state.parseToken("]]");
  } else {
    state.parseToken("]");
  }
  if (!parseLineEnd()) {
    state.error("Section headers must be followed by a newline.");
  }
  return path;
}

std::unique_ptr<TomlNode> Parser::parseInlineNode() {
  if (!state.peek()) {
    state.error("Input file cannot be empty.");
  }
  switch (*state.peek()) {
    case '{':
      return parseTableNode();
    case '[':
      return parseListNode();
    case '\'':
    case '"':
      return parseStringNode();
    case 't':
      return parseTrueNode();
    case 'f':
      return parseFalseNode();
    case 'i':
    case 'n':
      return parseFloatSpecial();
    case '0':
      if (state.match2('0', 'x') || state.match2('0', 'X') || state.match2('0', 'b') || state.match2('0', 'B') ||
          state.match2('0', 'o') || state.match2('0', 'O')) {
        return parseBaseIntegerNode();
      }
      return parseNumberOrDateNode();
    case '+':
      if (state.match2('+', 'i') || state.match2('+', 'n')) {
        return parseFloatSpecial();
      }
      return parseNumberOrDateNode();
    case '-':
      if (state.match2('-', 'i') || state.match2('-', 'n')) {
        return parseFloatSpecial();
      }
      return parseNumberOrDateNode();
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return parseNumberOrDateNode();
    default:
      state.unexpectedToken({"{", "[", "null", "true", "false", "number", "string"});
  }
}

std::unique_ptr<StringNode> Parser::parseStringNode() { return std::make_unique<StringNode>(parseString()); }

std::unique_ptr<BoolNode> Parser::parseTrueNode() {
  state.parseToken("true");
  return std::make_unique<BoolNode>(true);
}

std::unique_ptr<BoolNode> Parser::parseFalseNode() {
  state.parseToken("false");
  return std::make_unique<BoolNode>(false);
}

static std::optional<double> asDouble(std::string_view string) {
  double value = 0.0;
  auto [ptr, ec] = std::from_chars(string.begin(), string.end(), value);
  if (ec == std::errc() && ptr == string.end() && !std::isnan(value) && !std::isinf(value)) {
    return value;
  }
  return std::nullopt;
}

static std::optional<int64_t> asInt(std::string_view string, int base = 10) {
  int64_t value = 0.0;
  auto [ptr, ec] = std::from_chars(string.begin(), string.end(), value, base);
  if (ec == std::errc() && ptr == string.end() && !std::isnan(value) && !std::isinf(value)) {
    return value;
  }
  return std::nullopt;
}

static bool removeNumberUnderscores(std::string &string) {
  bool lastIsDigit = false;
  bool lastIsUnderscore = false;
  for (auto iter = string.begin(); iter != string.end();) {
    if (std::isdigit(*iter, std::locale::classic())) {
      lastIsDigit = true;
      lastIsUnderscore = false;
    }
    if (*iter == '_') {
      if (!lastIsDigit) {
        return false;
      }
      lastIsDigit = false;
      lastIsUnderscore = true;
      iter = string.erase(iter);
      continue;
    }
    iter++;
  }
  return !lastIsUnderscore;
}

std::unique_ptr<IntNode> Parser::parseBaseIntegerNode() {
  Position start = state.pos();

  std::string string = state.readAnyChar("0123456789aAbBcCdDeEfF_xXoObB");

  if (!removeNumberUnderscores(string)) {
    state.error("A number cannot contain two consecutive underscores ('_').", start);
  }

  if (string.starts_with("0x") || string.starts_with("0X")) {
    string.erase(0, 2);
    if (auto integer = asInt(string, 16)) {
      return std::make_unique<IntNode>(*integer);
    }
    state.error("Invalid hexadecimal integer.", start);
  }
  if (string.starts_with("0o") || string.starts_with("0O")) {
    string.erase(0, 2);
    if (auto integer = asInt(string, 8)) {
      return std::make_unique<IntNode>(*integer);
    }
    state.error("Invalid octal integer.", start);
  }
  if (string.starts_with("0b") || string.starts_with("0B")) {
    string.erase(0, 2);
    if (auto integer = asInt(string, 2)) {
      return std::make_unique<IntNode>(*integer);
    }
    state.error("Invalid binary integer.", start);
  }
  state.error("Invalid integer.", start);
}

std::unique_ptr<TomlNode> Parser::parseNumberOrDateNode() {
  Position start = state.pos();

  std::string string = state.readAnyChar("0123456789_+-.eEtTzZ:");
  if (state.peek() == ' ' && state.peek2() && std::isdigit(*state.peek2(), std::locale::classic())) {
    string.push_back(*state.consume());
    string.append(state.readAnyChar("0123456789+-zZ:."));
  }

  bool hasDecimalPoint = string.find(".") != std::string::npos;
  bool hasExponent = string.find_first_of("eE") != std::string::npos;

  bool hasDate = std::count(string.begin(), string.end(), '-') == 2 && !hasExponent;
  bool hasTime = string.find(":") != std::string::npos;

  if (hasDate || hasTime) {
    DateTime dateTime;
    if (DateTime::tryFromString(string, dateTime)) {
      return std::make_unique<DateTimeNode>(dateTime);
    }
    state.error("Invalid date/time.", start);
  }

  if (!removeNumberUnderscores(string)) {
    state.error("A number cannot contain two consecutive underscores ('_').", start);
  }

  if (string.starts_with('+')) {
    string.erase(0, 1);
  }

  if (hasExponent || hasDecimalPoint) {
    if (auto floatNum = asDouble(string)) {
      return std::make_unique<FloatNode>(*floatNum);
    }
    state.error("Invalid floating point number.", start);
  }

  if (auto intNum = asInt(string)) {
    return std::make_unique<IntNode>(*intNum);
  }
  state.error("Invalid decimal integer.", start);
}

std::unique_ptr<FloatNode> Parser::parseFloatSpecial() {
  Position start = state.pos();
  int sign = 1;
  double value;
  if (state.peek() == '+') {
    state.consume();
    sign = 1;
  } else if (state.peek() == '-') {
    state.consume();
    sign = -1;
  }
  if (state.peek() == 'n') {
    value = std::numeric_limits<double>::quiet_NaN();
    state.parseToken("nan");
  } else if (state.peek() == 'i') {
    value = std::numeric_limits<double>::infinity();
    state.parseToken("inf");
  } else {
    state.error("Invalid special float value.", start);
  }
  return std::make_unique<FloatNode>(value * sign);
}

std::unique_ptr<TableNode> Parser::parseTableNode() {
  std::unique_ptr<TableNode> tableNode = std::make_unique<TableNode>();

  Position start = state.pos();

  state.parseToken("{");
  parseLineEnd();
  while (state.peek() != '}') {
    if (!state.peek()) {
      state.error("Unterminated table.", start);
    }

    Position keyStart = state.pos();

    TomlPath childPath = parsePath();
    state.skipWhitespace(false);
    state.parseToken("=");
    state.skipWhitespace(false);
    std::unique_ptr<TomlNode> value = parseInlineNode();

    if (!tableNode->putValue(childPath, std::move(value))) {
      state.error("Conflicting key in inline table.", keyStart);
    }

    state.skipWhitespace(false);
    if (state.peek() == ',') {
      state.consume();
      parseLineEnd();
    } else {
      parseLineEnd();
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
  parseLineEnd();
  while (state.peek() != ']') {
    if (!state.peek()) {
      state.error("Unterminated list.", start);
    }

    std::unique_ptr<TomlNode> value = parseInlineNode();
    state.skipWhitespace(false);

    listNode->children.push_back(std::move(value));

    if (state.peek() == ',') {
      state.consume();
      parseLineEnd();
    } else {
      parseLineEnd();
      break;
    }
  }
  state.parseToken("]");

  return listNode;
}

TomlPath Parser::parsePath() {
  std::vector<std::string> keys;
  keys.push_back(parseKey());
  state.skipWhitespace(false);
  while (state.peek() == '.') {
    state.consume();
    state.skipWhitespace(false);
    keys.push_back(parseKey());
    state.skipWhitespace(false);
  }
  return TomlPath(std::move(keys), false);
}

std::string Parser::parseKey() {
  if (!state.peek()) {
    state.error("Key expected.");
  }
  if (state.peek() == '\'') {
    return parseAposString(false);
  }
  if (state.peek() == '\"') {
    return parseQuoteString(false);
  }
  return parseBareKey();
}

std::string Parser::parseBareKey() {
  std::string key;
  while (auto ch = state.peek()) {
    if (std::isalnum(*ch, std::locale::classic())) {
      key.push_back(*state.consume());
      continue;
    }
    if (ch == '_' || ch == '-') {
      key.push_back(*state.consume());
      continue;
    }
    break;
  }
  if (key.empty()) {
    state.error("Bare key expected.");
  }
  return key;
}

std::string Parser::parseString(bool allowMultiline) {
  if (state.peek() == '\'') {
    return parseAposString(allowMultiline);
  }
  if (state.peek() == '\"') {
    return parseQuoteString(allowMultiline);
  }
  state.error("String expected.");
}

std::string Parser::parseQuoteString(bool allowMultiline) {
  Position start = state.pos();

  size_t quoteCount = 0;
  while (state.peek() == '"' && quoteCount < 3) {
    quoteCount++;
    state.consume();
  }

  bool multiline = false;
  switch (quoteCount) {
    case 0:
      state.error("Expected quoted string", start);
    case 1:
      multiline = false;
      break;
    case 2:
      return std::string();
    case 3:
      if (!allowMultiline) {
        state.error("Multiline strings are not allowed in this context.", start);
      }
      multiline = true;
      break;
  }

  quoteCount = 0;
  std::string string;
  while (true) {
    auto ch = state.consume();
    if (!ch) {
      state.error("Unterminated string.", start);
    }
    if (!multiline && ch == '\n') {
      state.error("Newlines are not allowed in single-line strings.");
    }
    if (multiline && ch == '\\' && state.peek() == '\n') {
      state.skipWhitespace();
      quoteCount = 0;
      continue;
    }
    if (ch == '\\' && state.peek()) {
      string.push_back('\\');
      string.push_back(*state.consume());
      quoteCount = 0;
      continue;
    }
    if (ch == '"') {
      quoteCount++;
    } else {
      quoteCount = 0;
    }
    string.push_back(*ch);
    if (!multiline && quoteCount == 1) {
      string.resize(string.size() - 1);
      break;
    }
    if (multiline && quoteCount == 3) {
      string.resize(string.size() - 3);
      break;
    }
  }

  try {
    return tomlUnescape(string);
  } catch (const EscapeError &error) {
    state.error("Invalid escape code in string.", start);
  }
}

std::string Parser::parseAposString(bool allowMultiline) {
  Position start = state.pos();

  size_t quoteCount = 0;
  while (state.peek() == '\'' && quoteCount < 3) {
    quoteCount++;
    state.consume();
  }

  bool multiline = false;
  switch (quoteCount) {
    case 0:
      state.error("Expected single-quoted string", start);
    case 1:
      multiline = false;
      break;
    case 2:
      return std::string();
    case 3:
      if (!allowMultiline) {
        state.error("Multiline strings are not allowed in this context.", start);
      }
      multiline = true;
      break;
  }

  quoteCount = 0;
  std::string string;
  while (true) {
    auto ch = state.consume();
    if (!ch) {
      state.error("Unterminated string.", start);
    }
    if (!multiline && ch == '\n') {
      state.error("Newlines are not allowed in single-line strings.");
    }
    if (ch == '\'') {
      quoteCount++;
    } else {
      quoteCount = 0;
    }
    string.push_back(*ch);
    if (!multiline && quoteCount == 1) {
      string.resize(string.size() - 1);
      break;
    }
    if (multiline && quoteCount == 3) {
      string.resize(string.size() - 3);
      break;
    }
  }

  return string;
}

bool Parser::parseLineEnd() {
  bool foundNewLine = false;
  while (state.peek()) {
    state.skipWhitespace(foundNewLine);
    if (state.peek() == '\n') {
      foundNewLine = true;
      continue;
    }
    if (state.peek() == '#') {
      state.readUntilAny("\n");
      foundNewLine = true;
      continue;
    }
    break;
  }
  if (!state.peek()) {
    foundNewLine = true;
  }
  return foundNewLine;
}
