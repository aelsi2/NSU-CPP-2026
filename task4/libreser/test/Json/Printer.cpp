#include "LibReser/Json/Printer.hpp"

#include <sstream>

#include "LibReser/Json/AST.hpp"
#include "TestUtils.hpp"
#include "gtest/gtest.h"

using namespace reser::json;

TEST(JsonPrinter, Null) {
  std::stringstream stream;

  auto node = std::make_unique<NullNode>();
  Printer::print(stream, *node);

  ASSERT_EQ("null", test::trim(stream.str()));
}

TEST(JsonPrinter, BoolTrue) {
  std::stringstream stream;

  auto node = std::make_unique<BoolNode>(true);
  Printer::print(stream, *node);

  ASSERT_EQ("true", test::trim(stream.str()));
}

TEST(JsonPrinter, BoolFalse) {
  std::stringstream stream;

  auto node = std::make_unique<BoolNode>(false);
  Printer::print(stream, *node);

  ASSERT_EQ("false", test::trim(stream.str()));
}

TEST(JsonPrinter, EmptyString) {
  std::stringstream stream;

  auto node = std::make_unique<StringNode>("");
  Printer::print(stream, *node);

  ASSERT_EQ("\"\"", test::trim(stream.str()));
}

TEST(JsonPrinter, String) {
  std::stringstream stream;

  auto node = std::make_unique<StringNode>("Hello, world!");
  Printer::print(stream, *node);

  ASSERT_EQ("\"Hello, world!\"", test::trim(stream.str()));
}

TEST(JsonPrinter, StringEscape) {
  std::stringstream stream;

  auto node = std::make_unique<StringNode>("\"\\abcd\"\n");
  Printer::print(stream, *node);

  ASSERT_EQ(R"("\"\\abcd\"\n")", test::trim(stream.str()));
}

TEST(JsonPrinter, PositiveInt) {
  std::stringstream stream;

  auto node = std::make_unique<IntNode>(123);
  Printer::print(stream, *node);

  ASSERT_EQ("123", test::trim(stream.str()));
}

TEST(JsonPrinter, NegativeInt) {
  std::stringstream stream;

  auto node = std::make_unique<IntNode>(-123);
  Printer::print(stream, *node);

  ASSERT_EQ("-123", test::trim(stream.str()));
}

TEST(JsonPrinter, ZeroInt) {
  std::stringstream stream;

  auto node = std::make_unique<IntNode>(0);
  Printer::print(stream, *node);

  ASSERT_EQ("0", test::trim(stream.str()));
}

TEST(JsonPrinter, SmallPositiveFloat) {
  std::stringstream stream;

  auto node = std::make_unique<FloatNode>(12.1);
  Printer::print(stream, *node);

  ASSERT_EQ("12.1", test::trim(stream.str()));
}

TEST(JsonPrinter, SmallNegativeFloat) {
  std::stringstream stream;

  auto node = std::make_unique<FloatNode>(-87.526);
  Printer::print(stream, *node);

  ASSERT_EQ("-87.526", test::trim(stream.str()));
}

TEST(JsonPrinter, ZeroFloat) {
  std::stringstream stream;

  auto node = std::make_unique<FloatNode>(0);
  Printer::print(stream, *node);

  ASSERT_EQ("0.0", test::trim(stream.str()));
}

TEST(JsonPrinter, LargeFloat) {
  std::stringstream stream;

  auto node = std::make_unique<FloatNode>(1e+20);
  Printer::print(stream, *node);

  ASSERT_EQ("1e+20", test::trim(stream.str()));
}

TEST(JsonPrinter, EmptyList) {
  std::stringstream stream;

  auto node = std::make_unique<ListNode>();
  Printer::print(stream, *node);

  ASSERT_EQ("[]", test::trim(stream.str()));
}

TEST(JsonPrinter, EmptyTable) {
  std::stringstream stream;

  auto node = std::make_unique<TableNode>();
  Printer::print(stream, *node);

  ASSERT_EQ("{}", test::trim(stream.str()));
}

TEST(JsonPrinter, NestedListsIndent) {
  std::stringstream stream;

  auto list = std::make_unique<ListNode>();
  list->children.push_back(std::make_unique<ListNode>());
  list->children[0]->as<ListNode>()->children.push_back(std::make_unique<ListNode>());
  list->children[0]->as<ListNode>()->children[0]->as<ListNode>()->children.push_back(std::make_unique<ListNode>());
  list->children.push_back(std::make_unique<ListNode>());
  list->children[1]->as<ListNode>()->children.push_back(std::make_unique<ListNode>());
  list->children[1]->as<ListNode>()->children[0]->as<ListNode>()->children.push_back(std::make_unique<ListNode>());
  Printer::print(stream, *list);

  ASSERT_EQ(
      "[\n    [\n        [\n            []\n        ]\n    ],\n    [\n        [\n            []\n        ]\n    ]\n]\n",
      stream.str());
}

TEST(JsonPrinter, NestedTablesIndent) {
  std::stringstream stream;

  auto table = std::make_unique<TableNode>();
  table->children["a"] = std::make_unique<TableNode>();
  table->children["a"]->as<TableNode>()->children["b"] = std::make_unique<TableNode>();
  table->children["a"]->as<TableNode>()->children["b"]->as<TableNode>()->children["c"] = std::make_unique<TableNode>();
  Printer::print(stream, *table);

  ASSERT_EQ(
      "{\n    \"a\": {\n        \"b\": {\n            \"c\": {}\n        }\n    }\n}\n",
      stream.str());
}
