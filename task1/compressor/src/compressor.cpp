#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_map>

#include "libcompressor.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

static const std::unordered_map<std::string, libcompressor_CompressionAlgorithm> AlgMap = {
    {"zlib", libcompressor_Zlib},
    {"bzip", libcompressor_Bzip},
};

int main(int argc, char **argv) {
  auto err = spdlog::stderr_color_mt("stderr");

  if (argc < 3) {
    err->error("At least two command line arguments are required.");
    return EXIT_FAILURE;
  }

  auto alg_iter = AlgMap.find(argv[1]);
  if (alg_iter == AlgMap.end()) {
    err->error("Invalid algorithm: '{}'", argv[1]);
    return EXIT_FAILURE;
  }

  auto input = libcompressor_Buffer(argv[2], std::strlen(argv[2]));
  auto result = libcompressor_compress(alg_iter->second, input);

  if (result.data == nullptr && result.size == 0) {
    err->error("Compression error.");
    return EXIT_FAILURE;
  }

  for (int i = 0; i < result.size; i++) {
    std::printf("%.2hhx", result.data[i]);
  }
  std::printf("\n");

  std::free(result.data);

  return EXIT_SUCCESS;
}
