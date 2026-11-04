#ifndef __LIBRESER_TEST_INGTEGRATION_TEST_UTIL_HPP__
#define __LIBRESER_TEST_INGTEGRATION_TEST_UTIL_HPP__

#include <sstream>

#include "LibReser/Format.hpp"
#include "gtest/gtest.h"

template <typename Parser, typename Printer>
inline static void testRoundTrip(const std::string_view input, const reser::FormatBackend &backend1,
                                 const reser::FormatBackend &backend2, const reser::FormatBackend &backend3) {
  std::istringstream stream1{std::string(input)};

  std::ostringstream referenceStream;
  auto astNode = Parser::parse(stream1);
  Printer::print(referenceStream, *astNode);

  stream1.clear();
  stream1.seekg(0);
  auto ir1 = backend1.fromStream(stream1);
  std::stringstream stream2;
  backend2.toStream(stream2, *ir1);

  auto ir2 = backend2.fromStream(stream2);
  std::stringstream stream3;
  backend3.toStream(stream3, *ir2);

  auto ir3 = backend3.fromStream(stream3);
  std::stringstream stream0;
  backend1.toStream(stream0, *ir3);

  ASSERT_EQ(stream0.str(), referenceStream.str());
}

#endif // __LIBRESER_TEST_INGTEGRATION_TEST_UTIL_HPP__
