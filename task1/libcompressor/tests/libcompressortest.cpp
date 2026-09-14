#include <gtest/gtest.h>

#include <cstring>
#include <libcompressor.hpp>

TEST(LibCompressor, ZlibNotEmptyToNotEmpty) {
  char input_buf[] = "Hello";
  auto input = libcompressor_Buffer(input_buf, std::strlen(input_buf));
  auto output = libcompressor_compress(libcompressor_Zlib, input);
  std::free(output.data);

  EXPECT_NE(0, output.size);
}

TEST(LibCompressor, BzipNotEmptyToNotEmpty) {
  char input_buf[] = "Hello";
  auto input = libcompressor_Buffer(input_buf, std::strlen(input_buf));
  auto output = libcompressor_compress(libcompressor_Bzip, input);
  std::free(output.data);

  EXPECT_NE(0, output.size);
}

TEST(LibCompressor, ZlibEmptyToEmpty) {
  char input_buf[16];
  auto input = libcompressor_Buffer(input_buf, 0);
  auto output = libcompressor_compress(libcompressor_Zlib, input);
  std::free(output.data);

  EXPECT_EQ(0, output.size);
}

TEST(LibCompressor, BzipEmptyToEmpty) {
  char input_buf[16];
  auto input = libcompressor_Buffer(input_buf, 0);
  auto output = libcompressor_compress(libcompressor_Bzip, input);
  std::free(output.data);

  EXPECT_EQ(0, output.size);
}

TEST(LibCompressor, ZlibTestString) {
  char input_buf[] = "test_string";
  auto input = libcompressor_Buffer(input_buf, std::strlen(input_buf));

  unsigned char expected_output[] = {
      0x78, 0x9c, 0x2b, 0x49, 0x2d, 0x2e, 0x89, 0x2f, 0x2e, 0x29, 0xca, 0xcc, 0x4b, 0x07, 0x00, 0x1c, 0x79, 0x04, 0xb7,
  };

  auto output = libcompressor_compress(libcompressor_Zlib, input);
  std::free(output.data);

  EXPECT_EQ(sizeof(expected_output), output.size);
  EXPECT_EQ(0, std::memcmp(output.data, expected_output, sizeof(expected_output)));
}

TEST(LibCompressor, BzipTestString) {
  char input_buf[] = "test_string";
  auto input = libcompressor_Buffer(input_buf, std::strlen(input_buf));

  unsigned char expected_output[] = {
      0x42, 0x5a, 0x68, 0x31, 0x31, 0x41, 0x59, 0x26, 0x53, 0x59, 0x4a, 0x7c,  0x69, 0x05, 0x00, 0x00,
      0x04, 0x83, 0x80, 0x00, 0x00, 0x82, 0xa1, 0x1c, 0x00, 0x20, 0x00, 0x22,  0x03, 0x68, 0x84,  0x30,
      0x22, 0x50, 0xdf, 0x04, 0x99, 0xe2, 0xee, 0x48, 0xa7, 0x0a, 0x12, 0x09, 0x4f, 0x8d, 0x20,  0xa0,
  };

  auto output = libcompressor_compress(libcompressor_Bzip, input);
  std::free(output.data);

  EXPECT_EQ(sizeof(expected_output), output.size);
  EXPECT_EQ(0, std::memcmp(output.data, expected_output, sizeof(expected_output)));
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
