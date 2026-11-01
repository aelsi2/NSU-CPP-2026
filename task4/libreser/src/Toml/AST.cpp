#include "LibReser/Toml/AST.hpp"

using namespace reser::toml;
using namespace reser;

TomlPath TomlPath::concat(const TomlPathSlice &slice) const {
  std::vector<std::string> newKeys = keys;
  newKeys.insert(newKeys.end(), slice.keys.begin(), slice.keys.end());
  return TomlPath(std::move(newKeys), slice.isArrayOfTables);
}

TomlPath TomlPathSlice::concat(const TomlPathSlice &slice) const {
  std::vector<std::string> newKeys = std::vector(keys.begin(), keys.end());
  newKeys.insert(newKeys.end(), slice.keys.begin(), slice.keys.end());
  return TomlPath(std::move(newKeys), slice.isArrayOfTables);
}

TableNode *TableNode::getSection(TomlPathSlice path) {
  if (path.empty()) {
    return this;
  }
  const std::string &head = *path.head();
  auto child = getValue<TomlNode>(head);
  if (!child) {
    if (path.simple() && path.isArrayOfTables) {
      auto newChild = std::make_unique<ListNode>();
      auto section = newChild->getSection(path.tail());
      children[head] = std::move(newChild);
      return section;
    } else {
      auto newChild = std::make_unique<TableNode>();
      auto section = newChild->getSection(path.tail());
      children[head] = std::move(newChild);
      return section;
    }
  }
  if (auto tableChild = child->as<TableNode>()) {
    return tableChild->getSection(path.tail());
  }
  if (auto listChild = child->as<ListNode>()) {
    return listChild->getSection(path.tail());
  }
  return nullptr;
}

TableNode *ListNode::getSection(TomlPathSlice path) {
  if (!empty() && !isOfTables()) {
    return nullptr;
  }
  if (path.empty()) {
    children.push_back(std::make_unique<TableNode>());
    return children.back()->as<TableNode>();
  }
  if (children.empty()) {
    return nullptr;
  }
  if (auto tableChild = children.back()->as<TableNode>()) {
    return tableChild->getSection(path);
  }
  return nullptr;
}

bool TableNode::putValue(TomlPathSlice path, std::unique_ptr<TomlNode> value) {
  if (path.empty()) {
    return false;
  }
  const std::string &key = *path.head();
  if (path.simple()) {
    if (getValue<TomlNode>(key)) {
      return false;
    }
    children[key] = std::move(value);
    return true;
  }
  if (auto existing = getValue<TomlNode>(key)) {
    if (auto existingTable = existing->as<TableNode>()) {
      return existingTable->putValue(path.tail(), std::move(value));
    }
    return false;
  }
  auto newTable = std::make_unique<TableNode>();
  if (newTable->putValue(path.tail(), std::move(value))) {
    children[key] = std::move(newTable);
    return true;
  }
  return false;
}
