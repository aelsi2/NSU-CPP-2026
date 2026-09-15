#ifndef __LIBCOMPRESSOR_HPP__
#define __LIBCOMPRESSOR_HPP__

/// Comporession algorithm for \ref libcompressor_compress.
enum libcompressor_CompressionAlgorithm {
  /// ZLib compression.
  libcompressor_Zlib,
  /// BZip compression.
  libcompressor_Bzip,
};

/// Non-owning input/output buffer for \ref libcompressor_compress.
struct libcompressor_Buffer {
  /// Points to the underlying data block in memory.
  char *data;
  /// Contains size of the data block pointed to by \ref data field in bytes.
  int size;

  /// \brief Constructs an instance of \ref libcompressor_Buffer with the specified data pointer and size.
  libcompressor_Buffer(char *data, int size) : data(data), size(size) {}

  /// \brief Constructs an instance of \ref libcompressor_Buffer with \ref libcompressor_Buffer::data set to nullptr and
  /// \ref libcompressor_Buffer::size set to 0.
  libcompressor_Buffer() : libcompressor_Buffer(nullptr, 0) {}
};

/// \brief Compresses the data with the specified algorithm.
///
/// Returns an instance of \ref libcompressor_Buffer with the compressed data.
/// The pointed-to data must be freed manually with std::free. The input buffer is left unmodified.
/// On error returns a buffer with \ref libcompressor_Buffer::data set to nullptr and \ref libcompressor_Buffer::size
/// set to 0.
libcompressor_Buffer libcompressor_compress(libcompressor_CompressionAlgorithm algo, libcompressor_Buffer input);

#endif  // __LIBCOMPRESSOR_HPP__
