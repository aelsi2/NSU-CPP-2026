#include "LibReser/Toml/Printer.hpp"

#include <limits>
#include <sstream>

#include "LibReser/Toml/AST.hpp"
#include "TestUtils.hpp"
#include "gtest/gtest.h"

using namespace reser::toml;

TEST(TomlPrinter, BoolTrue) {
  std::stringstream stream;

  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<BoolNode>(true);
  Printer::print(stream, *node);

  ASSERT_EQ("value = true", test::trim(stream.str()));
}

TEST(TomlPrinter, BoolFalse) {
  std::stringstream stream;

  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<BoolNode>(false);
  Printer::print(stream, *node);

  ASSERT_EQ("value = false", test::trim(stream.str()));
}

TEST(TomlPrinter, EmptyString) {
  std::stringstream stream;
  
  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<StringNode>("");
  Printer::print(stream, *node);

  ASSERT_EQ("value = \"\"", test::trim(stream.str()));
}

TEST(TomlPrinter, String) {
  std::stringstream stream;
  
  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<StringNode>("Hello, world!");
  Printer::print(stream, *node);

  ASSERT_EQ("value = \"Hello, world!\"", test::trim(stream.str()));
}

TEST(TomlPrinter, StringEscape) {
  std::stringstream stream;

  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<StringNode>("\"\\abcd\"\n");
  Printer::print(stream, *node);

  ASSERT_EQ(R"(value = "\"\\abcd\"\n")", test::trim(stream.str()));
}

TEST(TomlPrinter, PositiveInt) {
  std::stringstream stream;

  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<IntNode>(123);
  Printer::print(stream, *node);

  ASSERT_EQ("value = 123", test::trim(stream.str()));
}

TEST(TomlPrinter, NegativeInt) {
  std::stringstream stream;

  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<IntNode>(-123);
  Printer::print(stream, *node);

  ASSERT_EQ("value = -123", test::trim(stream.str()));
}

TEST(TomlPrinter, ZeroInt) {
  std::stringstream stream;

  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<IntNode>(0);
  Printer::print(stream, *node);

  ASSERT_EQ("value = 0", test::trim(stream.str()));
}

TEST(TomlPrinter, SmallPositiveFloat) {
  std::stringstream stream;

  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<FloatNode>(12.1);
  Printer::print(stream, *node);

  ASSERT_EQ("value = 12.1", test::trim(stream.str()));
}

TEST(TomlPrinter, SmallNegativeFloat) {
  std::stringstream stream;

  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<FloatNode>(-87.526);
  Printer::print(stream, *node);

  ASSERT_EQ("value = -87.526", test::trim(stream.str()));
}

TEST(TomlPrinter, ZeroFloat) {
  std::stringstream stream;

  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<FloatNode>(0);
  Printer::print(stream, *node);

  ASSERT_EQ("value = 0.0", test::trim(stream.str()));
}

TEST(TomlPrinter, LargeFloat) {
  std::stringstream stream;
  
  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<FloatNode>(1e+20);
  Printer::print(stream, *node);

  ASSERT_EQ("value = 1e+20", test::trim(stream.str()));
}

TEST(TomlPrinter, NanFloat) {
  std::stringstream stream;
  
  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<FloatNode>(std::numeric_limits<double>().quiet_NaN());
  Printer::print(stream, *node);

  ASSERT_EQ("value = nan", test::trim(stream.str()));
}

TEST(TomlPrinter, PositiveInfFloat) {
  std::stringstream stream;
  
  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<FloatNode>(std::numeric_limits<double>().infinity());
  Printer::print(stream, *node);

  ASSERT_EQ("value = +inf", test::trim(stream.str()));
}

TEST(TomlPrinter, NegativeInfFloat) {
  std::stringstream stream;
  
  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<FloatNode>(-std::numeric_limits<double>().infinity());
  Printer::print(stream, *node);

  ASSERT_EQ("value = -inf", test::trim(stream.str()));
}

TEST(TomlPrinter, EmptyList) {
  std::stringstream stream;

  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<ListNode>();
  Printer::print(stream, *node);

  ASSERT_EQ("value = []", test::trim(stream.str()));
}

TEST(TomlPrinter, EmptyTable) {
  std::stringstream stream;
  
  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::make_unique<TableNode>();
  Printer::print(stream, *node);

  ASSERT_EQ("[value]", test::trim(stream.str()));
}

TEST(TomlPrinter, NestedListsIndent) {
  std::stringstream stream;

  auto list = std::make_unique<ListNode>();
  list->children.push_back(std::make_unique<ListNode>());
  list->children[0]->as<ListNode>()->children.push_back(std::make_unique<ListNode>());
  list->children[0]->as<ListNode>()->children[0]->as<ListNode>()->children.push_back(std::make_unique<ListNode>());
  list->children.push_back(std::make_unique<ListNode>());
  list->children[1]->as<ListNode>()->children.push_back(std::make_unique<ListNode>());
  list->children[1]->as<ListNode>()->children[0]->as<ListNode>()->children.push_back(std::make_unique<ListNode>());

  auto node = std::make_unique<TableNode>();
  node->children["value"] = std::move(list);
  Printer::print(stream, *node);

  ASSERT_EQ(
      "value = [\n    [\n        [\n            []\n        ]\n    ],\n    [\n        [\n            []\n        ]\n    ]\n]\n",
      stream.str());
}

TEST(TomlPrinter, NestedTableSections) {
  std::stringstream stream;

  auto table = std::make_unique<TableNode>();
  table->children["a"] = std::make_unique<TableNode>();
  table->children["a"]->as<TableNode>()->children["b"] = std::make_unique<TableNode>();
  table->children["a"]->as<TableNode>()->children["b"]->as<TableNode>()->children["c"] = std::make_unique<TableNode>();
  Printer::print(stream, *table);

  ASSERT_EQ(
      "[a]\n[a.b]\n[a.b.c]\n",
      stream.str());
}

TEST(TomlPrinter, TableArraySections) {
  std::stringstream stream;

  auto table = std::make_unique<TableNode>();
  table->children["array"] = std::make_unique<ListNode>();
  auto &array = *table->getChild<ListNode>("array");

  auto element1 = std::make_unique<TableNode>();
  element1->children["value"] = std::make_unique<IntNode>(456);
  
  auto element2 = std::make_unique<TableNode>();
  element2->children["value"] = std::make_unique<FloatNode>(123);

  array.children.push_back(std::move(element1));
  array.children.push_back(std::move(element2));
  Printer::print(stream, *table);

  ASSERT_EQ(
      "[[array]]\nvalue = 456\n\n[[array]]\nvalue = 123.0\n",
      stream.str());
}

TEST(TomlPrinter, NestedTableArraySections) {
  std::stringstream stream;

  auto table = std::make_unique<TableNode>();
  table->children["array"] = std::make_unique<ListNode>();
  auto &array = *table->getChild<ListNode>("array");

  auto element1 = std::make_unique<TableNode>();
  auto array1 = std::make_unique<ListNode>();
  auto subTable1 = std::make_unique<TableNode>();
  subTable1->children["value"] = std::make_unique<IntNode>(456);
  array1->children.push_back(std::move(subTable1));
  element1->children["sub"] = std::move(array1);
  array.children.push_back(std::move(element1));

  auto element2 = std::make_unique<TableNode>();
  auto array2 = std::make_unique<ListNode>();
  auto subTable2 = std::make_unique<TableNode>();
  subTable2->children["value"] = std::make_unique<FloatNode>(123.456);
  array2->children.push_back(std::move(subTable2));
  element2->children["sub"] = std::move(array2);
  array.children.push_back(std::move(element2));

  Printer::print(stream, *table);

  ASSERT_EQ(
      "[[array]]\n[[array.sub]]\nvalue = 456\n\n[[array]]\n[[array.sub]]\nvalue = 123.456\n",
      stream.str());
}
