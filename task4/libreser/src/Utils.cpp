#include "LibReser/Utils.hpp"

#include <map>

using namespace reser;

std::string backslashEscape(const std::string &string, const std::map<char, char> &subst) {
  std::string result = string;
  for (auto iter = result.begin(); iter != result.end(); iter++) {
    char ch = *iter;

    auto newChar = subst.find(ch);
    if (newChar != subst.end()) {
      ch = newChar->second;
      iter = result.erase(iter);
      iter = result.insert(iter, '\\');
      iter = result.insert(++iter, ch);
    }
  }

  return result;
}

std::string backslashUnescape(const std::string &string, const std::map<char, char> &subst) {
  std::string result = string;
  bool prevSlash = false;
  for (auto iter = result.begin(); iter != result.end(); iter++) {
    char ch = *iter;

    if (!prevSlash) {
      if (ch == '\\') {
        prevSlash = true;
      }
      continue;
    }

    iter -= 1;
    iter = result.erase(iter);
    iter = result.erase(iter);

    auto newChar = subst.find(ch);
    if (newChar == subst.end()) {
      throw EscapeError("Invalid escape code");
    }
    iter = result.insert(iter, newChar->second);
    prevSlash = false;
  }

  return result;
}

std::string reser::jsonEscape(const std::string &string) {
  static const std::map<char, char> subst = {
      {'"', '"'}, {'\\', '\\'}, {'\b', 'b'}, {'\f', 'f'}, {'\n', 'n'}, {'\r', 'r'}, {'\t', 't'},
  };

  return backslashEscape(string, subst);
}

std::string reser::tomlEscape(const std::string &string) {
  static const std::map<char, char> subst = {{'"', '"'},  {'\\', '\\'}, {'\b', 'b'}, {'\f', 'f'},
                                             {'\n', 'n'}, {'\r', 'r'},  {'\t', 't'}, {'\033', 'e'}};

  return backslashEscape(string, subst);
}

std::string reser::jsonUnescape(const std::string &string) {
  static const std::map<char, char> subst = {
      {'"', '"'}, {'\\', '\\'}, {'/', '/'}, {'b', '\b'}, {'f', '\f'}, {'n', '\n'}, {'r', '\r'}, {'t', '\t'},
  };
  return backslashUnescape(string, subst);
}

std::string reser::tomlUnescape(const std::string &string) {
  static const std::map<char, char> subst = {
      {'"', '"'}, {'\\', '\\'}, {'b', '\b'}, {'f', '\f'}, {'n', '\n'}, {'r', '\r'}, {'t', '\t'}, {'e', '\033'},
  };
  return backslashUnescape(string, subst);
}
