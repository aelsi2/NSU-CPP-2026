#include <sstream>

#include "LibReser/Json/AST.hpp"
#include "LibReser/Json/Parser.hpp"
#include "LibReser/Parser.hpp"
#include "gtest/gtest.h"

using namespace reser::json;

TEST(JsonParser, Null) {
  std::istringstream stream("null");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<NullNode>());
}

TEST(JsonParser, NullWhiteSpace) {
  std::istringstream stream("   \n\nnull\r\n\t");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<NullNode>());
}

TEST(JsonParser, BoolTrue) {
  std::istringstream stream("true");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<BoolNode>() && node->as<BoolNode>()->value == true);
}

TEST(JsonParser, BoolFalse) {
  std::istringstream stream("false");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<BoolNode>() && node->as<BoolNode>()->value == false);
}

TEST(JsonParser, EmptyArray) {
  std::istringstream stream("[]");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<ListNode>() && node->as<ListNode>()->children.empty());
}

TEST(JsonParser, EmptyTable) {
  std::istringstream stream("{}");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TableNode>() && node->as<TableNode>()->children.empty());
}

TEST(JsonParser, Int0) {
  std::istringstream stream("0");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<IntNode>() && node->as<IntNode>()->value == 0);
}

TEST(JsonParser, Int69420) {
  std::istringstream stream("69420");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<IntNode>() && node->as<IntNode>()->value == 69420);
}

TEST(JsonParser, IntMinus100500) {
  std::istringstream stream("-100500");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<IntNode>() && node->as<IntNode>()->value == -100500);
}

TEST(JsonParser, Float0) {
  std::istringstream stream("0.0");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<FloatNode>() && node->as<FloatNode>()->value == 0);
}

TEST(JsonParser, Float69420_67) {
  std::istringstream stream("69420.67");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<FloatNode>() && node->as<FloatNode>()->value == 69420.67);
}

TEST(JsonParser, FloatMinus65536_32768) {
  std::istringstream stream("-65536.32768");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<FloatNode>() && node->as<FloatNode>()->value == -65536.32768);
}

TEST(JsonParser, FloatExponent) {
  std::istringstream stream("1e+10");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<FloatNode>() && node->as<FloatNode>()->value == 1e+10);
}

TEST(JsonParser, FloatNegative_NegativeExponent) {
  std::istringstream stream("-1E-10");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<FloatNode>() && node->as<FloatNode>()->value == -1e-10);
}

TEST(JsonParser, FloatFractionalExponent) {
  std::istringstream stream("-12345.12345E-10");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<FloatNode>() && node->as<FloatNode>()->value == -12345.12345e-10);
}

TEST(JsonParser, EmptyString) {
  std::istringstream stream("\"\"");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<StringNode>() && node->as<StringNode>()->value == "");
}

TEST(JsonParser, String) {
  std::istringstream stream("\"Hello\"");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<StringNode>() && node->as<StringNode>()->value == "Hello");
}

TEST(JsonParser, StringEscape) {
  std::istringstream stream("\"Hello\\n\"");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<StringNode>() && node->as<StringNode>()->value == "Hello\n");
}

TEST(JsonParser, StringEscapeQuotes) {
  std::istringstream stream("\"\\\"He\\\\llo\\\"\"");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<StringNode>() && node->as<StringNode>()->value == "\"He\\llo\"");
}

TEST(JsonParser, EmptyTableKey) {
  std::istringstream stream("{\"\":\"\"}");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TableNode>() && node->as<TableNode>()->getChild<StringNode>(""));
}

TEST(JsonParser, ListOfFloats) {
  std::istringstream stream("[0.1, 1.0, -5.15, 69.1, 4420.3, 0.127]");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<ListNode>());
  ASSERT_EQ(6, node->as<ListNode>()->children.size());
  for (const auto &child : node->as<ListNode>()->children) {
    EXPECT_TRUE(child && child->isa<FloatNode>());
  }
}

TEST(JsonParser, NestedList) {
  std::istringstream stream("[[[[[]]]]]");

  auto root = Parser::parse(stream);
  ASSERT_TRUE(root && root->isa<ListNode>());
  ASSERT_EQ(1, root->as<ListNode>()->children.size());

  const auto &c1 = *root->as<ListNode>()->children[0];
  ASSERT_TRUE(c1.isa<ListNode>());
  ASSERT_EQ(1, c1.as<ListNode>()->children.size());

  const auto &c2 = *c1.as<ListNode>()->children[0];
  ASSERT_TRUE(c2.isa<ListNode>());
  ASSERT_EQ(1, c2.as<ListNode>()->children.size());

  const auto &c3 = *c2.as<ListNode>()->children[0];
  ASSERT_TRUE(c3.isa<ListNode>());
  ASSERT_EQ(1, c3.as<ListNode>()->children.size());

  const auto &c4 = *c3.as<ListNode>()->children[0];
  ASSERT_TRUE(c4.isa<ListNode>());
  ASSERT_EQ(0, c4.as<ListNode>()->children.size());
}

TEST(JsonParser, NestedTable) {
  std::istringstream stream("{\"a\":{\"a\":{\"a\":{\"a\":{}}}}}");

  auto root = Parser::parse(stream);
  ASSERT_TRUE(root && root->isa<TableNode>());
  ASSERT_EQ(1, root->as<TableNode>()->children.size());

  const auto &c1 = *root->as<TableNode>()->getChild<TableNode>("a");
  ASSERT_EQ(1, c1.as<TableNode>()->children.size());

  const auto &c2 = *c1.as<TableNode>()->getChild<TableNode>("a");
  ASSERT_EQ(1, c2.as<TableNode>()->children.size());

  const auto &c3 = *c2.as<TableNode>()->getChild<TableNode>("a");
  ASSERT_EQ(1, c3.as<TableNode>()->children.size());

  const auto &c4 = *c3.as<TableNode>()->getChild<TableNode>("a");
  ASSERT_EQ(0, c4.as<TableNode>()->children.size());
}

TEST(JsonParser, ComplexTable) {
  std::istringstream stream(R"(
    {
      "array": [1,true,1.0],
      "table1" :
      {
        "a": "A",
        "b" :"B",
      },
      "table2": {
        "table3": {}
      }
    }
  )");

  auto result = Parser::parse(stream);
  auto root = result ? result->as<TableNode>() : nullptr;
  ASSERT_NE(root, nullptr);
  ASSERT_EQ(3, root->children.size());

  auto array = root->getChild<ListNode>("array");
  ASSERT_NE(array, nullptr);
  ASSERT_EQ(3, array->children.size());

  auto table1 = root->getChild<TableNode>("table1");
  ASSERT_NE(table1, nullptr);
  ASSERT_EQ(2, table1->children.size());

  auto table2 = root->getChild<TableNode>("table2");
  ASSERT_NE(table2, nullptr);
  ASSERT_EQ(1, table2->children.size());

  auto table1_a = table1->getChild<StringNode>("a");
  auto table1_b = table1->getChild<StringNode>("b");
  ASSERT_TRUE(table1_a && table1_a->value == "A");
  ASSERT_TRUE(table1_b && table1_b->value == "B");

  auto table3 = table2->getChild<TableNode>("table3");
  ASSERT_NE(table3, nullptr);
  ASSERT_EQ(0, table3->children.size());
}

TEST(JsonParser, Empty) {
  std::istringstream stream("");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(JsonParser, UnterminatedArray) {
  std::istringstream stream("[");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(JsonParser, UnterminatedTable) {
  std::istringstream stream("{");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(JsonParser, UnterminatedString) {
  std::istringstream stream("\"");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(JsonParser, UnfinishedNull) {
  std::istringstream stream("nul");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(JsonParser, KeyWithoutQuotes) {
  std::istringstream stream("{key:1}");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(JsonParser, DoubleDoubleDecimalPoint) {
  std::istringstream stream("0..123");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(JsonParser, DoubleDoubleExponent) {
  std::istringstream stream("0ee+10");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(JsonParser, IntDoubleSign) {
  std::istringstream stream("--1");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(JsonParser, InvalidEsacpe) {
  std::istringstream stream("\"\\z\"");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(JsonParser, DuplicateKeys) {
  std::istringstream stream("{\"a\":0, \"a\":1}");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}
