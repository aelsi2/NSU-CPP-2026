#include <sstream>

#include "LibReser/Parser.hpp"
#include "LibReser/Xml/AST.hpp"
#include "LibReser/Xml/Parser.hpp"
#include "gtest/gtest.h"

using namespace reser::xml;

TEST(XmlParser, EmptyTag) {
  std::istringstream stream("<tag></tag>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());
  ASSERT_TRUE(node->as<TagNode>()->name == "tag");
  ASSERT_TRUE(node->as<TagNode>()->children.empty());
}

TEST(XmlParser, SelfClosingTag) {
  std::istringstream stream("<tag/>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());
  ASSERT_TRUE(node->as<TagNode>()->name == "tag");
  ASSERT_TRUE(node->as<TagNode>()->children.empty());
}

TEST(XmlParser, NameWithPunctuation) {
  std::istringstream stream("<Text-Block._Text/>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());
  ASSERT_TRUE(node->as<TagNode>()->name == "Text-Block._Text");
  ASSERT_TRUE(node->as<TagNode>()->children.empty());
}

TEST(XmlParser, TagWithString) {
  std::istringstream stream("<tag>Hello, world!</tag>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());

  auto tag = node->as<TagNode>();
  ASSERT_EQ(1, tag->children.size());
  ASSERT_EQ("tag", tag->name);

  auto text = tag->getChild<TextNode>();
  ASSERT_NE(text, nullptr);

  ASSERT_EQ("Hello, world!", text->value);
}

TEST(XmlParser, TagWithStringsAndTag) {
  std::istringstream stream("<tag>Hello, <b>world</b>!</tag>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());

  auto tag = node->as<TagNode>();
  ASSERT_EQ(3, tag->children.size());
  ASSERT_EQ("tag", tag->name);

  auto text1 = tag->children[0]->as<TextNode>();
  ASSERT_NE(text1, nullptr);

  auto b = tag->children[1]->as<TagNode>();
  ASSERT_NE(b, nullptr);
  ASSERT_EQ("b", b->name);

  auto text2 = tag->children[2]->as<TextNode>();
  ASSERT_NE(text2, nullptr);

  auto b_text = b->getChild<TextNode>();
  ASSERT_NE(b_text, nullptr);

  ASSERT_EQ("Hello, ", text1->value);
  ASSERT_EQ("world", b_text->value);
  ASSERT_EQ("!", text2->value);
}

TEST(XmlParser, TagWithStringsAndTagAndComments) {
  std::istringstream stream("<tag>Hello,<!-- greetings --> <b><!-- this is a bold claim -->world</b>!</tag>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());

  auto tag = node->as<TagNode>();
  ASSERT_EQ(3, tag->children.size());
  ASSERT_EQ("tag", tag->name);

  auto text1 = tag->children[0]->as<TextNode>();
  ASSERT_NE(text1, nullptr);

  auto b = tag->children[1]->as<TagNode>();
  ASSERT_NE(b, nullptr);
  ASSERT_EQ("b", b->name);

  auto text2 = tag->children[2]->as<TextNode>();
  ASSERT_NE(text2, nullptr);

  auto b_text = b->getChild<TextNode>();
  ASSERT_NE(b_text, nullptr);

  ASSERT_EQ("Hello, ", text1->value);
  ASSERT_EQ("world", b_text->value);
  ASSERT_EQ("!", text2->value);
}

TEST(XmlParser, TagWithWhitespaceContent) {
  std::istringstream stream("<tag>  </tag>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());

  auto tag = node->as<TagNode>();
  ASSERT_EQ(1, tag->children.size());
  ASSERT_EQ("tag", tag->name);

  auto text = tag->getChild<TextNode>();
  ASSERT_NE(text, nullptr);

  ASSERT_EQ("  ", text->value);
}

TEST(XmlParser, TagWithWhitespaceContentAndComments) {
  std::istringstream stream("<tag> <!--   --> <!-- --></tag>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());

  auto tag = node->as<TagNode>();
  ASSERT_EQ(1, tag->children.size());
  ASSERT_EQ("tag", tag->name);

  auto text = tag->getChild<TextNode>();
  ASSERT_NE(text, nullptr);

  ASSERT_EQ("  ", text->value);
}

TEST(XmlParser, TagWithTagAndWhitespaceContent) {
  std::istringstream stream("<tag>  <inner/>  </tag>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());

  auto tag = node->as<TagNode>();
  ASSERT_EQ(1, tag->children.size());
  ASSERT_EQ("tag", tag->name);

  auto inner = tag->getChild<TagNode>();
  ASSERT_NE(inner, nullptr);

  ASSERT_EQ(0, inner->children.size());
  ASSERT_EQ("inner", inner->name);
}

TEST(XmlParser, TagWithTagAndWhitespaceContentAndComment) {
  std::istringstream stream("<tag> <!-- <>><><\"'--><inner/>  </tag>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());

  auto tag = node->as<TagNode>();
  ASSERT_EQ(1, tag->children.size());
  ASSERT_EQ("tag", tag->name);

  auto inner = tag->getChild<TagNode>();
  ASSERT_NE(inner, nullptr);

  ASSERT_EQ(0, inner->children.size());
  ASSERT_EQ("inner", inner->name);
}

TEST(XmlParser, Attribute) {
  std::istringstream stream("<tag attr=\"hello\"/>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());
  ASSERT_TRUE(node->as<TagNode>()->attributes["attr"] == "hello");
}

TEST(XmlParser, QuotedAttribute) {
  std::istringstream stream("<tag attr=\"'value'\"/>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());
  ASSERT_TRUE(node->as<TagNode>()->attributes["attr"] == "'value'");
}

TEST(XmlParser, AposAttribute) {
  std::istringstream stream("<tag attr='\"value\"'/>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());
  ASSERT_TRUE(node->as<TagNode>()->attributes["attr"] == "\"value\"");
}

TEST(XmlParser, MixedAttributes) {
  std::istringstream stream("<tag attr1=\"'value'\" attr2='\"value\"'/>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());
  ASSERT_TRUE(node->as<TagNode>()->attributes["attr1"] == "'value'");
  ASSERT_TRUE(node->as<TagNode>()->attributes["attr2"] == "\"value\"");
}

TEST(XmlParser, AttributeEscape) {
  std::istringstream stream("<tag attr=\"amp=&amp; lt=&lt; gt=&gt; quot=&quot;\"/>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());
  ASSERT_TRUE(node->as<TagNode>()->attributes["attr"] == "amp=& lt=< gt=> quot=\"");
}

TEST(XmlParser, TextEscapeLtNotGt) {
  std::istringstream stream("<tag>&lt;Hello, world!></tag>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());

  auto tag = node->as<TagNode>();
  ASSERT_EQ(1, tag->children.size());
  ASSERT_EQ("tag", tag->name);

  auto text = tag->getChild<TextNode>();
  ASSERT_NE(text, nullptr);

  ASSERT_EQ("<Hello, world!>", text->value);
}

TEST(XmlParser, QuotesInText) {
  std::istringstream stream("<tag>\"Hello, world!\"</tag>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());

  auto tag = node->as<TagNode>();
  ASSERT_EQ(1, tag->children.size());
  ASSERT_EQ("tag", tag->name);

  auto text = tag->getChild<TextNode>();
  ASSERT_NE(text, nullptr);

  ASSERT_EQ("\"Hello, world!\"", text->value);
}

TEST(XmlParser, TagWithWhitespace) {
  std::istringstream stream("<  tag   attr  =  \"hello\"  />");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());
  ASSERT_TRUE(node->as<TagNode>()->attributes["attr"] == "hello");
}

TEST(XmlParser, Prolog) {
  std::istringstream stream("<?xml version='1.0'?> <tag/>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());
  ASSERT_TRUE(node->as<TagNode>()->name == "tag");
  ASSERT_TRUE(node->as<TagNode>()->children.empty());
}

TEST(XmlParser, PrologWithComments) {
  std::istringstream stream("<?xml version='1.0'?><!-- comment--><!----> <tag/>");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TagNode>());
  ASSERT_TRUE(node->as<TagNode>()->name == "tag");
  ASSERT_TRUE(node->as<TagNode>()->children.empty());
}

TEST(XmlParser, ComplexTag) {
  std::istringstream stream(R"(
    <a f="1">
      <b>FooBar</b>
      <b z="w"/>
      <c><d x="y"/>Bar</c>
    </a>
  )");
  auto node = Parser::parse(stream);
  ASSERT_NE(node, nullptr);

  auto a = node->as<TagNode>();
  ASSERT_NE(a, nullptr);
  ASSERT_EQ("a", a->name);
  ASSERT_EQ("1", a->attributes["f"]);
  ASSERT_EQ(3, a->children.size());

  auto b_1 = a->children[0]->as<TagNode>();
  ASSERT_NE(b_1, nullptr);
  ASSERT_EQ("b", b_1->name);
  ASSERT_EQ(1, b_1->children.size());
  auto b_1_text = b_1->getChild<TextNode>();
  ASSERT_NE(b_1_text, nullptr);
  ASSERT_EQ("FooBar", b_1_text->value);

  auto b_2 = a->children[1]->as<TagNode>();
  ASSERT_NE(b_2, nullptr);
  ASSERT_EQ("b", b_2->name);
  ASSERT_EQ("w", b_2->attributes["z"]);
  ASSERT_TRUE(b_2->empty());

  auto c = a->children[2]->as<TagNode>();
  ASSERT_NE(c, nullptr);
  ASSERT_EQ("c", c->name);
  ASSERT_EQ(2, c->children.size());

  auto d = c->children[0]->as<TagNode>();
  ASSERT_NE(d, nullptr);
  ASSERT_TRUE(d->empty());
  ASSERT_EQ("y", d->attributes["x"]);

  auto c_text = c->children[1]->as<TextNode>();
  ASSERT_NE(c_text, nullptr);
  ASSERT_EQ("Bar", c_text->value);
}

TEST(XmlParser, Empty) {
  std::istringstream stream("");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(XmlParser, RootText) {
  std::istringstream stream("Hello");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(XmlParser, UnterminatedTag) {
  std::istringstream stream("<tag");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(XmlParser, NoClosingTag) {
  std::istringstream stream("<tag>");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(XmlParser, NoOpeningTag) {
  std::istringstream stream("</tag>");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(XmlParser, UnterminatedProlog) {
  std::istringstream stream("<?xml ");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(XmlParser, MismatchedOpeningClosingTags) {
  std::istringstream stream("<tag></tagg>");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(XmlParser, TagNameStartingWithNumber) {
  std::istringstream stream("<0a/>");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(XmlParser, InvalidTagNameChars) {
  std::istringstream stream("<прикол></прикол>");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(XmlParser, EmptyTagName) {
  std::istringstream stream("<></>");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(XmlParser, UnterminatedAttribute) {
  std::istringstream stream("<tag attr='/>");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(XmlParser, DuplicateAttributes) {
  std::istringstream stream("<tag attr='1' attr=\"2\"/>");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(XmlParser, PrologWithPrecedingWhitespace) {
  std::istringstream stream("  <?xml?><tag/>");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(XmlParser, PrologInTag) {
  std::istringstream stream("<tag><?xml?></tag>");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}
