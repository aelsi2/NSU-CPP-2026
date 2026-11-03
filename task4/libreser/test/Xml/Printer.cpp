#include "LibReser/Xml/Printer.hpp"

#include <sstream>

#include "LibReser/Xml/AST.hpp"
#include "TestUtils.hpp"
#include "gtest/gtest.h"

using namespace reser::xml;

TEST(XmlPrinter, EmptyTag) {
  std::stringstream stream;

  auto tag = std::make_unique<TagNode>("tag");
  Printer::print(stream, *tag);

  ASSERT_EQ("<tag/>", test::trim(stream.str()));
}

TEST(XmlPrinter, TagWithString) {
  std::stringstream stream;

  auto tag = std::make_unique<TagNode>("tag");
  tag->children.push_back(std::make_unique<TextNode>("content"));
  Printer::print(stream, *tag);

  ASSERT_EQ("<tag>content</tag>", test::trim(stream.str()));
}

TEST(XmlPrinter, TagWithStringTag) {
  std::stringstream stream;

  auto tag = std::make_unique<TagNode>("tag");
  tag->children.push_back(std::make_unique<TextNode>("content  "));
  tag->children.push_back(std::make_unique<TagNode>("thing"));
  tag->children.push_back(std::make_unique<TextNode>("  more content"));
  Printer::print(stream, *tag);

  ASSERT_EQ("<tag>content  <thing/>  more content</tag>", test::trim(stream.str()));
}

TEST(XmlPrinter, TagAttribute1) {
  std::stringstream stream;

  auto tag = std::make_unique<TagNode>("tag");
  tag->attributes["attr"] = "value";
  Printer::print(stream, *tag);

  ASSERT_EQ("<tag attr=\"value\"/>", test::trim(stream.str()));
}

TEST(XmlPrinter, TagAttribute3) {
  std::stringstream stream;

  auto tag = std::make_unique<TagNode>("tag");
  tag->attributes["attr1"] = "foo";
  tag->attributes["attr2"] = "bar";
  tag->attributes["attr3"] = "baz";
  Printer::print(stream, *tag);

  auto str = test::trim(stream.str());
  ASSERT_TRUE(str.starts_with("<tag"));
  ASSERT_TRUE(str.ends_with("/>"));
  ASSERT_TRUE(test::contains(str, "attr1=\"foo\""));
  ASSERT_TRUE(test::contains(str, "attr2=\"bar\""));
  ASSERT_TRUE(test::contains(str, "attr3=\"baz\""));
}

TEST(XmlPrinter, TextEscape) {
  std::stringstream stream;

  auto tag = std::make_unique<TagNode>("tag");
  tag->children.push_back(std::make_unique<TextNode>("<&content&>"));
  Printer::print(stream, *tag);

  ASSERT_EQ("<tag>&lt;&amp;content&amp;></tag>", test::trim(stream.str()));
}

TEST(XmlPrinter, AttributeEscape) {
  std::stringstream stream;

  auto tag = std::make_unique<TagNode>("tag");
  tag->attributes["attr"] = "\"<>&value";
  Printer::print(stream, *tag);

  ASSERT_EQ("<tag attr=\"&quot;&lt;>&amp;value\"/>", test::trim(stream.str()));
}

TEST(XmlPrinter, NestedTagsIndent) {
  std::stringstream stream;

  auto tag = std::make_unique<TagNode>("tag");
  tag->children.push_back(std::make_unique<TagNode>("tag"));
  tag->children[0]->as<TagNode>()->children.push_back(std::make_unique<TagNode>("tag"));
  tag->children[0]->as<TagNode>()->children[0]->as<TagNode>()->children.push_back(std::make_unique<TagNode>("tag"));
  Printer::print(stream, *tag);

  ASSERT_EQ("<tag>\n    <tag>\n        <tag>\n            <tag/>\n        </tag>\n    </tag>\n</tag>\n", stream.str());
}

