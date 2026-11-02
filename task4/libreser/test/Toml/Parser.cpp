#include "LibReser/Toml/Parser.hpp"

#include <cmath>
#include <limits>
#include <sstream>

#include "LibReser/Toml/AST.hpp"
#include "gtest/gtest.h"

using namespace reser::toml;
using namespace reser;

TEST(TomlParser, Comments) {
  std::istringstream stream(R"(
  # MIT License
  # 
  # Copyright (c) 69420 ACME Corp.
  # 
  # Permission is hereby granted, free of charge, to any person obtaining a copy
  # of this software and associated documentation files (the "Software"), to deal
  # in the Software without restriction, including without limitation the rights
  # to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  # copies of the Software, and to permit persons to whom the Software is
  # furnished to do so, subject to the following conditions:
  # 
  # The above copyright notice and this permission notice shall be included in all
  # copies or substantial portions of the Software.
  # 
  # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  # IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  # FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  # AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  # LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  # OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  # SOFTWARE.
  
  value = {}
  )");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<TableNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_TRUE(value->children.empty());
}

TEST(TomlParser, Empty) {
  std::istringstream stream("");
  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TableNode>() && node->as<TableNode>()->children.empty());
}

TEST(TomlParser, BoolTrue) {
  std::istringstream stream("value = true");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<BoolNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(true, value->value);
}

TEST(TomlParser, BoolFalse) {
  std::istringstream stream("value = false");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<BoolNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(false, value->value);
}

TEST(TomlParser, EmptyInlineArray) {
  std::istringstream stream("value = []");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<ListNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_TRUE(value->children.empty());
}

TEST(TomlParser, EmptyInlineTable) {
  std::istringstream stream("value = {}");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<TableNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_TRUE(value->children.empty());
}

TEST(TomlParser, Int0) {
  std::istringstream stream("value = 0");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<IntNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(0, value->value);
}

TEST(TomlParser, Int69420) {
  std::istringstream stream("value = 69420");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<IntNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(69420, value->value);
}

TEST(TomlParser, IntMinus100500) {
  std::istringstream stream("value = -100500");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<IntNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(-100500, value->value);
}

TEST(TomlParser, IntUnderscores) {
  std::istringstream stream("value = 0_000_100_1");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<IntNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(1001, value->value);
}

TEST(TomlParser, IntHexadecimal) {
  std::istringstream stream("value = 0xf0");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<IntNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(0xf0, value->value);
}

TEST(TomlParser, IntOctal) {
  std::istringstream stream("value = 0o123");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<IntNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(0123, value->value);
}

TEST(TomlParser, IntBinary) {
  std::istringstream stream("value = 0b1110_0011");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<IntNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(0b11100011, value->value);
}

TEST(TomlParser, Float0) {
  std::istringstream stream("value = 0.0");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<FloatNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(0.0, value->value);
}

TEST(TomlParser, Float69420_67) {
  std::istringstream stream("value = 69420.67");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<FloatNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(69420.67, value->value);
}

TEST(TomlParser, FloatMinus65536_32768) {
  std::istringstream stream("value = 65536.32768");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<FloatNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(65536.32768, value->value);
}

TEST(TomlParser, FloatExponent) {
  std::istringstream stream("value = 1e+10");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<FloatNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(1e+10, value->value);
}

TEST(TomlParser, FloatNegative_NegativeExponent) {
  std::istringstream stream("value = -1E-10");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<FloatNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(-1e-10, value->value);
}

TEST(TomlParser, FloatFractionalExponent) {
  std::istringstream stream("value = -12345.12345E-10");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<FloatNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(-12345.12345e-10, value->value);
}

TEST(TomlParser, FloatUnderscores) {
  std::istringstream stream("value = -1_2_345.123_45E-10");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<FloatNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(-12345.12345e-10, value->value);
}

TEST(TomlParser, FloatNan) {
  std::istringstream stream("value = nan");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<FloatNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_TRUE(std::isnan(value->value));
}

TEST(TomlParser, FloatPlusInfinity) {
  std::istringstream stream("value = +inf");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<FloatNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(std::numeric_limits<double>().infinity(), value->value);
}

TEST(TomlParser, FloatMinusInfinity) {
  std::istringstream stream("value = -inf");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<FloatNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(-std::numeric_limits<double>().infinity(), value->value);
}

TEST(TomlParser, EmptyString) {
  std::istringstream stream("value = \"\"");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<StringNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ("", value->value);
}

TEST(TomlParser, String) {
  std::istringstream stream("value = \"Hello\"");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<StringNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ("Hello", value->value);
}

TEST(TomlParser, StringEscape) {
  std::istringstream stream("value = \"Hello\\n\"");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<StringNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ("Hello\n", value->value);
}

TEST(TomlParser, StringEscapeQuotes) {
  std::istringstream stream("value = \"\\\"He\\\\llo\\\"\"");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<StringNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ("\"He\\llo\"", value->value);
}

TEST(TomlParser, StringApos) {
  std::istringstream stream(R"(value = 'C:\Users\aelsi2\Documents\projects\cpp\NSU-CPP-2026')");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<StringNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(R"(C:\Users\aelsi2\Documents\projects\cpp\NSU-CPP-2026)", value->value);
}

TEST(TomlParser, StringAposTriple) {
  std::istringstream stream(R"(value = '''"I [dw]on't need \d{2} apples"''')");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<StringNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(R"("I [dw]on't need \d{2} apples")", value->value);
}

TEST(TomlParser, StringQuoteTriple) {
  std::istringstream stream("value = \"\"\"\"I [dw]on't need \\\n   \tapples\\\"\"\"\"");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<StringNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(R"("I [dw]on't need apples")", value->value);
}

TEST(TomlParser, EmptyKey) {
  std::istringstream stream("\"\" = \"\"");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<StringNode>("");
  ASSERT_NE(value, nullptr);
}

TEST(TomlParser, ArrayOfFloats) {
  std::istringstream stream("value = [0.1, 1.0, -5.15, 69.1, 4420.3, 0.127]");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<ListNode>("value");
  ASSERT_NE(value, nullptr);

  ASSERT_EQ(6, value->children.size());
  for (const auto &child : value->children) {
    EXPECT_TRUE(child && child->isa<FloatNode>());
  }
}

TEST(TomlParser, NestedInlineArray) {
  std::istringstream stream("value = [[[[[]]]]]");

  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<ListNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(1, value->children.size());

  const auto &c1 = *value->as<ListNode>()->children[0];
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

TEST(TomlParser, ArraySection) {
  std::istringstream stream(R"(
  [[array]]
  value = 1

  [[array]]
  value = 2
  )");

  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto array = root->getValue<ListNode>("array");
  ASSERT_NE(array, nullptr);
  ASSERT_EQ(2, array->children.size());

  const auto &c1 = *array->children[0];
  ASSERT_TRUE(c1.isa<TableNode>());
  auto value1 = c1.as<TableNode>()->getValue<IntNode>("value");
  ASSERT_NE(value1, nullptr);
  ASSERT_EQ(1, value1->value);
  
  const auto &c2 = *array->children[1];
  ASSERT_TRUE(c1.isa<TableNode>());
  auto value2 = c2.as<TableNode>()->getValue<IntNode>("value");
  ASSERT_NE(value2, nullptr);
  ASSERT_EQ(2, value2->value);
}

TEST(TomlParser, NestedArraySection) {
  std::istringstream stream(R"(
  [[array]]
  [[array.sub.array]]
  value = 2
  )");

  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto array = root->getValue<ListNode>("array");
  ASSERT_NE(array, nullptr);
  ASSERT_EQ(1, array->children.size());

  const auto &array_el = *array->children[0];
  ASSERT_TRUE(array_el.isa<TableNode>());
  auto sub = array_el.as<TableNode>()->getValue<TableNode>("sub");
  ASSERT_NE(sub, nullptr);
  
  auto sub_array = sub->getValue<ListNode>("array");
  ASSERT_NE(sub_array, nullptr);
  ASSERT_EQ(1, sub_array->children.size());
  
  const auto &sub_array_el = *sub_array->children[0];
  ASSERT_TRUE(sub_array_el.isa<TableNode>());
  auto value = sub_array_el.as<TableNode>()->getValue<IntNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(2, value->value);
}

TEST(TomlParser, NestedTable) {
  std::istringstream stream("value = {a={a={a={a={}}}}}");

  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<TableNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(1, value->children.size());

  const auto &c1 = *value->getValue<TableNode>("a");
  ASSERT_EQ(1, c1.as<TableNode>()->children.size());

  const auto &c2 = *c1.as<TableNode>()->getValue<TableNode>("a");
  ASSERT_EQ(1, c2.as<TableNode>()->children.size());

  const auto &c3 = *c2.as<TableNode>()->getValue<TableNode>("a");
  ASSERT_EQ(1, c3.as<TableNode>()->children.size());

  const auto &c4 = *c3.as<TableNode>()->getValue<TableNode>("a");
  ASSERT_EQ(0, c4.as<TableNode>()->children.size());
}

TEST(TomlParser, NestedInlineTablePath) {
  std::istringstream stream("value.a.a.a.a = {}");

  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<TableNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(1, value->children.size());

  const auto &c1 = *value->getValue<TableNode>("a");
  ASSERT_EQ(1, c1.as<TableNode>()->children.size());

  const auto &c2 = *c1.as<TableNode>()->getValue<TableNode>("a");
  ASSERT_EQ(1, c2.as<TableNode>()->children.size());

  const auto &c3 = *c2.as<TableNode>()->getValue<TableNode>("a");
  ASSERT_EQ(1, c3.as<TableNode>()->children.size());

  const auto &c4 = *c3.as<TableNode>()->getValue<TableNode>("a");
  ASSERT_EQ(0, c4.as<TableNode>()->children.size());
}

TEST(TomlParser, NestedTableSections) {
  std::istringstream stream("[value.a.a.a.a]");

  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<TableNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(1, value->children.size());

  const auto &c1 = *value->getValue<TableNode>("a");
  ASSERT_EQ(1, c1.as<TableNode>()->children.size());

  const auto &c2 = *c1.as<TableNode>()->getValue<TableNode>("a");
  ASSERT_EQ(1, c2.as<TableNode>()->children.size());

  const auto &c3 = *c2.as<TableNode>()->getValue<TableNode>("a");
  ASSERT_EQ(1, c3.as<TableNode>()->children.size());

  const auto &c4 = *c3.as<TableNode>()->getValue<TableNode>("a");
  ASSERT_EQ(0, c4.as<TableNode>()->children.size());
}

TEST(TomlParser, ComplexTable) {
  std::istringstream stream(R"(
    value = {
      array = [1,true,1.0],
      table1 = {
        a= "A",
        b ="B",
      },
      table2= {
        table3 = {}
      }
    }
  )");

  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<TableNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(3, value->children.size());

  auto array = value->getValue<ListNode>("array");
  ASSERT_NE(array, nullptr);
  ASSERT_EQ(3, array->children.size());

  auto table1 = value->getValue<TableNode>("table1");
  ASSERT_NE(table1, nullptr);
  ASSERT_EQ(2, table1->children.size());

  auto table2 = value->getValue<TableNode>("table2");
  ASSERT_NE(table2, nullptr);
  ASSERT_EQ(1, table2->children.size());

  auto table1_a = table1->getValue<StringNode>("a");
  auto table1_b = table1->getValue<StringNode>("b");
  ASSERT_TRUE(table1_a && table1_a->value == "A");
  ASSERT_TRUE(table1_b && table1_b->value == "B");

  auto table3 = table2->getValue<TableNode>("table3");
  ASSERT_NE(table3, nullptr);
  ASSERT_EQ(0, table3->children.size());
}

TEST(TomlParser, ComplexTablePaths) {
  std::istringstream stream(R"(
  value.array = [1, true, 1.0]
  value.table1.a = "A"
  value.table1.b = "B"
  value.table2.table3 = {}
  )");

  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<TableNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(3, value->children.size());

  auto array = value->getValue<ListNode>("array");
  ASSERT_NE(array, nullptr);
  ASSERT_EQ(3, array->children.size());

  auto table1 = value->getValue<TableNode>("table1");
  ASSERT_NE(table1, nullptr);
  ASSERT_EQ(2, table1->children.size());

  auto table2 = value->getValue<TableNode>("table2");
  ASSERT_NE(table2, nullptr);
  ASSERT_EQ(1, table2->children.size());

  auto table1_a = table1->getValue<StringNode>("a");
  auto table1_b = table1->getValue<StringNode>("b");
  ASSERT_TRUE(table1_a && table1_a->value == "A");
  ASSERT_TRUE(table1_b && table1_b->value == "B");

  auto table3 = table2->getValue<TableNode>("table3");
  ASSERT_NE(table3, nullptr);
  ASSERT_EQ(0, table3->children.size());
}

TEST(TomlParser, ComplexTableSections) {
  std::istringstream stream(R"(
  [value]
  array = [1, true, 1.0]
  
  [value.table1]
  a = "A"
  b = "B"

  [value.table2]
  [value.table2.table3]
  )");

  auto node = Parser::parse(stream);
  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<TableNode>("value");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(3, value->children.size());

  auto array = value->getValue<ListNode>("array");
  ASSERT_NE(array, nullptr);
  ASSERT_EQ(3, array->children.size());

  auto table1 = value->getValue<TableNode>("table1");
  ASSERT_NE(table1, nullptr);
  ASSERT_EQ(2, table1->children.size());

  auto table2 = value->getValue<TableNode>("table2");
  ASSERT_NE(table2, nullptr);
  ASSERT_EQ(1, table2->children.size());

  auto table1_a = table1->getValue<StringNode>("a");
  auto table1_b = table1->getValue<StringNode>("b");
  ASSERT_TRUE(table1_a && table1_a->value == "A");
  ASSERT_TRUE(table1_b && table1_b->value == "B");

  auto table3 = table2->getValue<TableNode>("table3");
  ASSERT_NE(table3, nullptr);
  ASSERT_EQ(0, table3->children.size());
}

TEST(TomlParser, QuotedKey) {
  std::istringstream stream(R"("this is a quoted key" = true)");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<BoolNode>("this is a quoted key");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(true, value->value);
}

TEST(TomlParser, AposKey) {
  std::istringstream stream(R"('\this is an apostrophe quoted key\' = true)");
  auto node = Parser::parse(stream);

  ASSERT_TRUE(node && node->isa<TableNode>());
  auto root = node->as<TableNode>();

  auto value = root->getValue<BoolNode>("\\this is an apostrophe quoted key\\");
  ASSERT_NE(value, nullptr);
  ASSERT_EQ(true, value->value);
}

TEST(TomlParser, RootValue) {
  std::istringstream stream("12345");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(TomlParser, UnterminatedArray) {
  std::istringstream stream("value = [");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(TomlParser, UnterminatedTable) {
  std::istringstream stream("value = {");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(TomlParser, UnterminatedString) {
  std::istringstream stream("value = \"");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(TomlParser, UnterminatedSectionHeader) {
  std::istringstream stream("[section");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(TomlParser, DoubleDoubleDecimalPoint) {
  std::istringstream stream("value = 0..123");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(TomlParser, IntDoubleUnderscore) {
  std::istringstream stream("value = 0__01");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(TomlParser, DoubleDoubleExponent) {
  std::istringstream stream("value = 0ee+10");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(TomlParser, IntDoubleSign) {
  std::istringstream stream("value = --1");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(TomlParser, InvalidEsacpe) {
  std::istringstream stream("value = \"\\z\"");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(TomlParser, KeyValueOnSeparateLines) {
  std::istringstream stream(R"(
  value = 
  10
  )");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(TomlParser, DuplicateKeys) {
  std::istringstream stream("value = {a=0, a=1}");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(TomlParser, ValuePathConflict) {
  std::istringstream stream(R"(
  value.sub = 5

  [value.sub.a]
  value = 10
  )");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(TomlParser, MultilineQuoteKey) {
  std::istringstream stream(R"("""value""" = "foo")");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}

TEST(TomlParser, MultilineAposKey) {
  std::istringstream stream(R"('''value''' = "foo")");
  ASSERT_THROW(Parser::parse(stream), reser::ParseError);
}
