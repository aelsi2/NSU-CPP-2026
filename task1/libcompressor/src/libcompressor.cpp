#include "libcompressor.hpp"

#include <bzlib.h>
#include <zconf.h>
#include <zlib.h>

#include <cstdlib>

libcompressor_Buffer libcompressor_compress(libcompressor_CompressionAlgorithm algo, libcompressor_Buffer input) {
  if (!input.data) {
    return {};
  }
  if (input.size < 0) {
    return {};
  }

  libcompressor_Buffer output;
  output.size = input.size + 1024;
  output.data = static_cast<char *>(std::malloc(output.size));

  if (!output.data) {
    return {};
  }
  if (input.size == 0) {
    output.size = 0;
    return output;
  }

  int res;
  switch (algo) {
    case libcompressor_Zlib: {
      uLongf dest_len = output.size;
      res = compress(reinterpret_cast<Bytef *>(output.data), &dest_len, reinterpret_cast<Bytef *>(input.data),
                     input.size);
      output.size = dest_len;
      if (res != Z_OK) {
        std::free(output.data);
        return {};
      }
    } break;
    case libcompressor_Bzip: {
      unsigned int dest_len = output.size;
      res = BZ2_bzBuffToBuffCompress(output.data, &dest_len, input.data, input.size, 1, 0, 0);
      output.size = dest_len;
      if (res != BZ_OK) {
        std::free(output.data);
        return {};
      }
    } break;
    default:
      std::free(output.data);
      return {};
  }
  return output;
}
