#include "LibReser/Xml/AST.hpp"

using namespace reser::xml;
using namespace reser;

bool TagNode::hasText() const {
  for (const auto &child : children) {
    if (child->isa<TextNode>()) {
      return true;
    }
  }
  return false;
}

bool TagNode::empty() const {
  if (children.empty()) {
    return true;
  }
  for (const auto &child : children) {
    if (!child->isa<TextNode>()) {
      return false;
    }
    if (!child->as<TextNode>()->value.empty()) {
      return false;
    }
  }
  return true;
}

void TagNode::normalize() {
  bool allText = true;
  bool allSpace = true;
  for (const auto &child : children) {
    if (!child->isa<TextNode>()) {
      allText = false;
      continue;
    }
    if (!child->as<TextNode>()->isSpace()) {
      allSpace = false;
      break;
    }
  }

  // If there are tags, and all text is whitespace, then remove it.
  // If the text is not all whitespace, we need to preserve it.
  if (!allText && allSpace) {
    for (auto iter = children.begin(); iter != children.end();) {
      if ((*iter)->isa<TextNode>()) {
        iter = children.erase(iter);
      } else {
        iter++;
      }
    }
  }

  // Merge adjacent text nodes.
  auto prev = children.end();
  for (auto iter = children.begin(); iter != children.end();) {
    if (!(*iter)->isa<TextNode>()) {
      prev = children.end();
      iter++;
      continue;
    }
    if (prev == children.end()) {
      prev = iter;
      iter++;
      continue;
    }
    const auto &currString = (*iter)->as<TextNode>()->value;
    auto &prevString = (*prev)->as<TextNode>()->value;
    prevString.append(currString);
    iter = children.erase(iter);
    prev = iter - 1;
  }

  // Remove chidren if all are just empty strings.
  if (empty()) {
    children.clear();
  }
}
