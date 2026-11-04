#include "IntegrationTestUtil.hpp"
#include "LibReser/Json/Backend.hpp"
#include "LibReser/Toml/Parser.hpp"
#include "LibReser/Toml/Printer.hpp"
#include "LibReser/Toml/Backend.hpp"
#include "LibReser/Xml/Backend.hpp"
#include "gtest/gtest.h"

using namespace reser;

TEST(Integration, TomlOffsetDateTimes) {
  static const std::string_view input = R"(
    offsetDateTimes = [
      2026-11-03 12:54:12.324+07:00,
      1970-01-01T00:00-17:04,
      2028-02-29t12:54:12-17:04,
    ]
  )";
  testRoundTrip<toml::Parser, toml::Printer>(input, toml::Backend, xml::Backend, json::Backend);
}

TEST(Integration, TomlZuluDateTimes) {
  static const std::string_view input = R"(
    zuluDateTimes = [
      2026-11-03 12:54:12.324z,
      1970-01-01T00:00+00:00,
      2028-02-29t12:54:12Z,
    ]
  )";
  testRoundTrip<toml::Parser, toml::Printer>(input, toml::Backend, xml::Backend, json::Backend);
}

TEST(Integration, TomlLocalDateTimes) {
  static const std::string_view input = R"(
    localDateTimes = [
      2026-11-03 12:54:12.324,
      1970-01-01T00:00,
      2028-02-29t12:54:12,
    ]
  )";
  testRoundTrip<toml::Parser, toml::Printer>(input, toml::Backend, xml::Backend, json::Backend);
}

TEST(Integration, TomlLocalDates) {
  static const std::string_view input = R"(
    localDates = [
      2026-11-03,
      1970-01-01,
      2028-02-29,
    ]
  )";
  testRoundTrip<toml::Parser, toml::Printer>(input, toml::Backend, xml::Backend, json::Backend);
}

TEST(Integration, TomlLocalTimes) {
  static const std::string_view input = R"(
    localTimes = [
      12:54:12.324,
      00:00,
      12:54:12,
    ]
  )";
  testRoundTrip<toml::Parser, toml::Printer>(input, toml::Backend, xml::Backend, json::Backend);
}

TEST(Integration, TomlNans) {
  static const std::string_view input = R"(
    nan = nan
    plus-nan = +nan
    minus-nan = -nan
  )";
  testRoundTrip<toml::Parser, toml::Printer>(input, toml::Backend, xml::Backend, json::Backend);
}

TEST(Integration, TomlInfs) {
  static const std::string_view input = R"(
    inf = inf
    plus-inf = +inf
    minus-inf = -inf
  )";
  testRoundTrip<toml::Parser, toml::Printer>(input, toml::Backend, xml::Backend, json::Backend);
}

TEST(Integration, TomlSmallFloats) {
  static const std::string_view input = R"(
    0-f = 0.0
    12-f = 12.0
    -256-f = -256.0
    127-f = 127e-0
  )";
  testRoundTrip<toml::Parser, toml::Printer>(input, toml::Backend, xml::Backend, json::Backend);
}

TEST(Integration, TomlLargeFloats) {
  static const std::string_view input = R"(
    1e20 = 1e+20
    10e10 = 10e+10
    1.001e15 = 1.001e+15
    -2.123e-23 = -2.123e-23
  )";
  testRoundTrip<toml::Parser, toml::Printer>(input, toml::Backend, xml::Backend, json::Backend);
}

TEST(Integration, TomlEncodedJsonNan) {
  static const std::string_view input = R"(
    "$type" = "nan"
  )";
  testRoundTrip<toml::Parser, toml::Printer>(input, toml::Backend, xml::Backend, json::Backend);
}
