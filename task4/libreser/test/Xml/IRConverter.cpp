#include "LibReser/Xml/IRConverter.hpp"

#include <cmath>

#include "LibReser/IR.hpp"
#include "gtest/gtest.h"

using namespace reser::xml;
using namespace reser;

TEST(XmlIR, NullRoundTrip) {
  auto irNode = std::make_unique<IR::NullNode>();
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::NullNode>());
}

TEST(XmlIR, IntRoundTrip) {
  auto irNode = std::make_unique<IR::IntNode>(69420);
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::IntNode>());
  ASSERT_EQ(69420, newIrNode->as<IR::IntNode>()->value);
}

TEST(XmlIR, FloatRoundTrip) {
  auto irNode = std::make_unique<IR::FloatNode>(69.420);
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::FloatNode>());
  ASSERT_EQ(69.420, newIrNode->as<IR::FloatNode>()->value);
}

TEST(XmlIR, ZeroFloatRoundTrip) {
  auto irNode = std::make_unique<IR::FloatNode>(0.0);
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::FloatNode>());
  ASSERT_EQ(0.0, newIrNode->as<IR::FloatNode>()->value);
}

TEST(XmlIR, LargeFloatRoundTrip) {
  auto irNode = std::make_unique<IR::FloatNode>(1e+10);
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::FloatNode>());
  ASSERT_EQ(1e+10, newIrNode->as<IR::FloatNode>()->value);
}

TEST(XmlIR, NanRoundTrip) {
  auto irNode = std::make_unique<IR::FloatNode>(std::numeric_limits<double>().quiet_NaN());
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::FloatNode>());
  ASSERT_TRUE(std::isnan(newIrNode->as<IR::FloatNode>()->value));
}

TEST(XmlIR, PlusInfinityRoundTrip) {
  auto irNode = std::make_unique<IR::FloatNode>(std::numeric_limits<double>().infinity());
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::FloatNode>());
  ASSERT_EQ(std::numeric_limits<double>().infinity(), newIrNode->as<IR::FloatNode>()->value);
}

TEST(XmlIR, MinusInfinityRoundTrip) {
  auto irNode = std::make_unique<IR::FloatNode>(-std::numeric_limits<double>().infinity());
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::FloatNode>());
  ASSERT_EQ(-std::numeric_limits<double>().infinity(), newIrNode->as<IR::FloatNode>()->value);
}

TEST(XmlIR, TrueRoundTrip) {
  auto irNode = std::make_unique<IR::BoolNode>(true);
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::BoolNode>());
  ASSERT_EQ(true, newIrNode->as<IR::BoolNode>()->value);
}

TEST(XmlIR, FalseRoundTrip) {
  auto irNode = std::make_unique<IR::BoolNode>(false);
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::BoolNode>());
  ASSERT_EQ(false, newIrNode->as<IR::BoolNode>()->value);
}

TEST(XmlIR, EmptyStringRoundTrip) {
  auto irNode = std::make_unique<IR::StringNode>("");
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::StringNode>());
  ASSERT_EQ("", newIrNode->as<IR::StringNode>()->value);
}

TEST(XmlIR, StringRoundTrip) {
  auto irNode = std::make_unique<IR::StringNode>("FooBar\n&<>");
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::StringNode>());
  ASSERT_EQ("FooBar\n&<>", newIrNode->as<IR::StringNode>()->value);
}

TEST(XmlIR, ZuluDateTimeRoundTrip) {
  DateTime dateTime;
  DateTime::tryFromString("2026-11-03t10:25:37.333z", dateTime);

  auto irNode = std::make_unique<IR::DateTimeNode>(dateTime);
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::DateTimeNode>());
  ASSERT_EQ(dateTime.toString(), newIrNode->as<IR::DateTimeNode>()->value.toString());
}

TEST(XmlIR, OffsetDateTimeRoundTrip) {
  DateTime dateTime;
  DateTime::tryFromString("2026-11-03t10:25:37.333+07:00", dateTime);

  auto irNode = std::make_unique<IR::DateTimeNode>(dateTime);
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::DateTimeNode>());
  ASSERT_EQ(dateTime.toString(), newIrNode->as<IR::DateTimeNode>()->value.toString());
}

TEST(XmlIR, LocalDateTimeRoundTrip) {
  DateTime dateTime;
  DateTime::tryFromString("2026-11-03t10:25:37.333", dateTime);

  auto irNode = std::make_unique<IR::DateTimeNode>(dateTime);
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::DateTimeNode>());
  ASSERT_EQ(dateTime.toString(), newIrNode->as<IR::DateTimeNode>()->value.toString());
}

TEST(XmlIR, LocalDateRoundTrip) {
  DateTime dateTime;
  DateTime::tryFromString("2026-11-03", dateTime);

  auto irNode = std::make_unique<IR::DateTimeNode>(dateTime);
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::DateTimeNode>());
  ASSERT_EQ(dateTime.toString(), newIrNode->as<IR::DateTimeNode>()->value.toString());
}

TEST(XmlIR, LocalTimeRoundTrip) {
  DateTime dateTime;
  DateTime::tryFromString("10:25:37.333", dateTime);

  auto irNode = std::make_unique<IR::DateTimeNode>(dateTime);
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::DateTimeNode>());
  ASSERT_EQ(dateTime.toString(), newIrNode->as<IR::DateTimeNode>()->value.toString());
}

TEST(XmlIR, EmptyArrayRoundTrip) {
  auto irNode = std::make_unique<IR::ListNode>();
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::ListNode>());
  ASSERT_TRUE(newIrNode->as<IR::ListNode>()->children.empty());
}

TEST(XmlIR, EmptyTableRoundTrip) {
  auto irNode = std::make_unique<IR::TableNode>();
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::TableNode>());
  ASSERT_TRUE(newIrNode->as<IR::TableNode>()->children.empty());
}

TEST(XmlIR, ListOfIntsRoundTrip) {
  IR::TagNode::ChildVector children;
  children.push_back(std::make_unique<IR::IntNode>(5));
  children.push_back(std::make_unique<IR::IntNode>(6));
  children.push_back(std::make_unique<IR::IntNode>(7));
  auto irNode = std::make_unique<IR::ListNode>(std::move(children));
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::ListNode>());
  auto newIrList = newIrNode->as<IR::ListNode>();

  auto newIrElement1 = newIrList->getChild<IR::IntNode>(0);
  ASSERT_NE(newIrElement1, nullptr);
  auto newIrElement2 = newIrList->getChild<IR::IntNode>(1);
  ASSERT_NE(newIrElement2, nullptr);
  auto newIrElement3 = newIrList->getChild<IR::IntNode>(2);
  ASSERT_NE(newIrElement3, nullptr);
  
  ASSERT_EQ(5, newIrElement1->value);
  ASSERT_EQ(6, newIrElement2->value);
  ASSERT_EQ(7, newIrElement3->value);
}

TEST(XmlIR, NestedTableRoundTrip) {
  IR::TableNode::ChildMap children1;
  children1["1"] = std::make_unique<IR::TableNode>();
  IR::TableNode::ChildMap children2;
  children2["2"] = std::make_unique<IR::TableNode>(std::move(children1));
  IR::TableNode::ChildMap children3;
  children3["3"] = std::make_unique<IR::TableNode>(std::move(children2));
  auto irNode = std::make_unique<IR::TableNode>(std::move(children3));
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::TableNode>());
  auto newIrTable3 = newIrNode->as<IR::TableNode>();

  auto newIrTable2 = newIrTable3->getChild<IR::TableNode>("3");
  ASSERT_NE(newIrTable2, nullptr);
  auto newIrTable1 = newIrTable2->getChild<IR::TableNode>("2");
  ASSERT_NE(newIrTable1, nullptr);
  auto newIrTable0 = newIrTable1->getChild<IR::TableNode>("1");
  ASSERT_NE(newIrTable0, nullptr);
  ASSERT_TRUE(newIrTable0->empty());
}

TEST(XmlIR, TagRoundTrip) {
  IR::TagNode::ChildVector children;
  children.push_back(std::make_unique<IR::TagNode>("child"));
  auto irNode = std::make_unique<IR::TagNode>("tag", std::move(children),
                                              IR::TagNode::AttrMap{{"attr1", "value1"}, {"attr2", "value2"}});
  auto astNode = IRConverter::fromIR(*irNode);
  auto newIrNode = IRConverter::toIR(*astNode);

  ASSERT_TRUE(newIrNode->isa<IR::TagNode>());
  ASSERT_EQ("tag", newIrNode->as<IR::TagNode>()->name);
  ASSERT_EQ("value1", newIrNode->as<IR::TagNode>()->attributes["attr1"]);
  ASSERT_EQ("value2", newIrNode->as<IR::TagNode>()->attributes["attr2"]);

  auto newIrChild = newIrNode->as<IR::TagNode>()->getChild<IR::TagNode>();
  ASSERT_NE(newIrChild, nullptr);
  ASSERT_EQ("child", newIrChild->name);
  ASSERT_TRUE(newIrChild->empty());
}
