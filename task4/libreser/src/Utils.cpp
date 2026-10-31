#include "LibReser/Utils.hpp"

#include <locale>
#include <map>

using namespace reser;

std::string escape(const std::string_view string, const std::map<char, std::string_view> &subst) {
  std::string result = std::string(string);
  for (auto iter = result.begin(); iter != result.end(); iter++) {
    auto newStr = subst.find(*iter);
    if (newStr != subst.end()) {
      iter = result.erase(iter);
      iter = result.insert(iter, newStr->second.begin(), newStr->second.end());
      iter++;
    }
  }

  return result;
}

std::string backslashUnescape(const std::string_view string, const std::map<char, char> &subst) {
  std::string result = std::string(string);
  bool prevSlash = false;
  for (auto iter = result.begin(); iter != result.end(); iter++) {
    char ch = *iter;

    if (!prevSlash) {
      if (ch == '\\') {
        prevSlash = true;
      }
      continue;
    }

    iter = result.erase(iter - 1, iter + 1);

    auto newChar = subst.find(ch);
    if (newChar == subst.end()) {
      throw EscapeError("Invalid escape code.");
    }
    iter = result.insert(iter, newChar->second);
    prevSlash = false;
  }

  return result;
}

std::string reser::jsonEscape(const std::string_view string) {
  static const std::map<char, std::string_view> subst = {
      {'"', "\\\""}, {'\\', "\\\\"}, {'\b', "\\b"}, {'\f', "\\f"}, {'\n', "\\n"}, {'\r', "\\r"}, {'\t', "\\t"},
  };
  return escape(string, subst);
}

std::string reser::tomlEscape(const std::string_view string) {
  static const std::map<char, std::string_view> subst = {{'"', "\\\""}, {'\\', "\\\\"}, {'\b', "\\b"}, {'\f', "\\f"},
                                                         {'\n', "\\n"}, {'\r', "\\r"},  {'\t', "\\t"}, {'\033', "\\e"}};
  return escape(string, subst);
}

std::string reser::jsonUnescape(const std::string_view string) {
  static const std::map<char, char> subst = {
      {'"', '"'}, {'\\', '\\'}, {'/', '/'}, {'b', '\b'}, {'f', '\f'}, {'n', '\n'}, {'r', '\r'}, {'t', '\t'},
  };
  return backslashUnescape(string, subst);
}

std::string reser::tomlUnescape(const std::string_view string) {
  static const std::map<char, char> subst = {
      {'"', '"'}, {'\\', '\\'}, {'b', '\b'}, {'f', '\f'}, {'n', '\n'}, {'r', '\r'}, {'t', '\t'}, {'e', '\033'},
  };
  return backslashUnescape(string, subst);
}

std::string reser::xmlEscape(const std::string_view string, XmlEscapeContext context) {
  static const std::map<char, std::string_view> substAttribute = {
      {'"', "&quot;"},
      {'&', "&amp;"},
      {'<', "&lt;"},
  };
  static const std::map<char, std::string_view> substText = {
      {'&', "&amp;"},
      {'<', "&lt;"},
  };
  const std::map<char, std::string_view> &subst = context == XmlEscapeContext::Attribute ? substAttribute : substText;

  return escape(string, subst);
}

std::string reser::xmlUnescape(const std::string_view string) {
  static const std::map<std::string_view, char> subst = {
      {"amp;", '&'}, {"quot;", '"'}, {"apos;", '\''}, {"gt;", '>'}, {"lt;", '<'},
  };
  std::string result = std::string(string);
  for (auto iter = result.begin(); iter != result.end(); iter++) {
    if (*iter != '&') {
      continue;
    }
    std::string_view tail = std::string_view(iter + 1, result.end());
    bool prefixFound = false;
    for (const auto prefix : subst) {
      if (!tail.starts_with(prefix.first)) {
        continue;
      }
      prefixFound = true;
      iter = result.erase(iter, iter + prefix.first.size() + 1);
      iter = result.insert(iter, prefix.second);
      break;
    }
    if (!prefixFound) {
      throw EscapeError("Invalid escape code.");
    }
  }
  return result;
}

bool reser::isValidXmlName(const std::string_view string) {
  if (string.empty()) {
    return false;
  }
  if (!std::isalpha(string[0], std::locale::classic()) && string[0] != '_' && string[0] != ':') {
    return false;
  }
  for (auto iter = std::next(string.begin()); iter != string.end(); iter++) {
    char ch = *iter;
    if (std::isalnum(ch, std::locale::classic()) || ch == '.' || ch == '_' || ch == '-' || ch == ':') {
      continue;
    }
    return false;
  }
  return true;
}
