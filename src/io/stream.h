#pragma once

#include "filemeta.h"
#include <stdio.h>
#include "io.h"
#include <SDL_rwops.h>
#include <SDL_endian.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>

namespace Impacto {
namespace Io {

class Stream {
 public:
  virtual ~Stream() {}
  FileMeta Meta;
  int64_t Position = 0;
  int64_t WritePosition = 0;

  bool IsSeekSlow = false;
  bool IsMemory = false;

  virtual int64_t Read(void* buffer, int64_t sz) = 0;
  virtual int64_t Seek(int64_t offset, int origin) = 0;
  virtual int64_t Write(void* buffer, int64_t sz, int cnt = 1) {
    assert(false && "Write not implemented for this stream");
    return IoError_Fail;
  }
  virtual IoError Duplicate(Stream** outStream) = 0;
};

inline uint8_t ReadU8(Stream* stream) {
  uint8_t result;
  stream->Read(&result, 1);
  return result;
}

template <typename T>
inline T ReadWithoutSwap(Stream* stream) {
  T result;
  stream->Read(&result, sizeof(T));
  return result;
}

template <int count, typename T>
inline void ReadArrayWithoutSwap(T* dest, Stream* stream) {
  stream->Read(dest, count * sizeof(T));
}
template <typename T>
inline void ReadArrayWithoutSwap(T* dest, Stream* stream, int count) {
  stream->Read(dest, count * sizeof(T));
}

template <typename T>
inline T SwapHelper(T value) {
  if constexpr (std::is_same_v<uint16_t, T> || std::is_same_v<int16_t, T>) {
    return SDL_Swap16(value);
  } else if constexpr (std::is_same_v<uint32_t, T> ||
                       std::is_same_v<int32_t, T>) {
    return SDL_Swap32(value);
  } else if constexpr (std::is_same_v<uint64_t, T> ||
                       std::is_same_v<int64_t, T>) {
    return SDL_Swap64(value);
  } else if constexpr (std::is_same_v<float, T>) {
    return SDL_SwapFloat(value);
  } else if constexpr (std::is_same_v<double, T>) {
    union {
      double result;
      uint64_t result_int;
    } u;
    u.result = value;
    u.result_int = SDL_Swap64(u.result_int);
    return u.result;
  }
}

template <typename T>
inline T ReadSwap(Stream* stream) {
  T result;
  stream->Read(&result, sizeof(T));
  return SwapHelper(result);
}

template <int count, typename T>
inline void ReadArraySwap(T* dest, Stream* stream) {
  ReadArrayWithoutSwap<count, T>(dest, stream);
  for (int i = 0; i < count; i++) dest[i] = SwapHelper<T>(dest[i]);
}

template <typename T>
inline void ReadArraySwap(T* dest, Stream* stream, int count) {
  ReadArrayWithoutSwap<T>(dest, stream, count);
  for (int i = 0; i < count; i++) dest[i] = SwapHelper<T>(dest[i]);
}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
template <typename T>
inline T ReadLE(InputStream* stream) {
  return ReadSwap<T>(stream);
}
template <int count, typename T>
inline void ReadArrayLE(T* dest, InputStream* stream) {
  ReadArraySwap<count, T>(dest, stream);
}
template <typename T>
inline void ReadArrayLE(T* dest, InputStream* stream, int count) {
  ReadArraySwap<T>(dest, stream, count);
}
template <typename T>
inline T ReadBE(InputStream* stream) {
  return ReadWithoutSwap<T>(stream);
}
template <int count, typename T>
inline void ReadArrayBE(T* dest, InputStream* stream) {
  ReadArrayWithoutSwap<count, T>(dest, stream);
}
template <typename T>
inline void ReadArrayBE(T* dest, InputStream* stream, int count) {
  ReadArrayWithoutSwap<T>(dest, stream, count);
}
#else
template <typename T>
inline T ReadLE(Stream* stream) {
  return ReadWithoutSwap<T>(stream);
}
template <int count, typename T>
inline void ReadArrayLE(T* dest, Stream* stream) {
  ReadArrayWithoutSwap<count, T>(dest, stream);
}
template <typename T>
inline void ReadArrayLE(T* dest, Stream* stream, int count) {
  ReadArrayWithoutSwap<T>(dest, stream, count);
}
template <typename T>
inline T ReadBE(Stream* stream) {
  return ReadSwap<T>(stream);
}
template <int count, typename T>
inline void ReadArrayBE(T* dest, Stream* stream) {
  ReadArraySwap<count, T>(dest, stream);
}
template <typename T>
inline void ReadArrayBE(T* dest, Stream* stream, int count) {
  ReadArraySwap<T>(dest, stream, count);
}
#endif

inline void ReadVec2LE(float* dest, Stream* stream) {
  ReadArrayLE<2>(dest, stream);
}
inline void ReadVec2LE(glm::vec2* dest, Stream* stream) {
  ReadArrayLE<2>((float*)dest, stream);
}
inline void ReadVec3LE(float* dest, Stream* stream) {
  ReadArrayLE<3>(dest, stream);
}
inline void ReadVec3LE(glm::vec3* dest, Stream* stream) {
  ReadArrayLE<3>((float*)dest, stream);
}
inline void ReadVec4LE(float* dest, Stream* stream) {
  ReadArrayLE<4>(dest, stream);
}
inline void ReadVec4LE(glm::vec4* dest, Stream* stream) {
  ReadArrayLE<4>((float*)dest, stream);
}
inline void ReadMat4LE(float* dest, Stream* stream) {
  ReadArrayLE<16>(dest, stream);
}
inline void ReadMat4LE(glm::mat4* dest, Stream* stream) {
  ReadArrayLE<16>((float*)dest, stream);
}

inline void ReadVec2BE(float* dest, Stream* stream) {
  ReadArrayBE<2>(dest, stream);
}
inline void ReadVec2BE(glm::vec2* dest, Stream* stream) {
  ReadArrayBE<2>((float*)dest, stream);
}
inline void ReadVec3BE(float* dest, Stream* stream) {
  ReadArrayBE<3>(dest, stream);
}
inline void ReadVec3BE(glm::vec3* dest, Stream* stream) {
  ReadArrayBE<3>((float*)dest, stream);
}
inline void ReadVec4BE(float* dest, Stream* stream) {
  ReadArrayBE<4>(dest, stream);
}
inline void ReadVec4BE(glm::vec4* dest, Stream* stream) {
  ReadArrayBE<4>((float*)dest, stream);
}
inline void ReadMat4BE(float* dest, Stream* stream) {
  ReadArrayBE<16>(dest, stream);
}
inline void ReadMat4BE(glm::mat4* dest, Stream* stream) {
  ReadArrayBE<16>((float*)dest, stream);
}

inline void WriteU8(Stream* stream, uint8_t value) { stream->Write(&value, 1); }

template <typename T>
inline void WriteWithoutSwap(Stream* stream, T value) {
  stream->Write(&value, sizeof(T));
}

template <int count, typename T>
inline void WriteArrayWithoutSwap(T* src, Stream* stream) {
  stream->Write(src, sizeof(T), count);
}
template <typename T>
inline void WriteArrayWithoutSwap(T* src, Stream* stream, int count) {
  stream->Write(src, sizeof(T), count);
}

template <typename T>
inline void WriteSwap(Stream* stream, T value) {
  T result = SwapHelper<T>(value);
  stream->Write(&result, sizeof(T));
}

template <int count, typename T>
inline void WriteArraySwap(T* src, Stream* stream) {
  std::array<T, count> swapped;
  for (int i = 0; i < count; i++) swapped[i] = SwapHelper<T>(src[i]);
  WriteArrayWithoutSwap<count, T>(swapped.data(), stream);
}

template <typename T>
inline void WriteArraySwap(T* src, Stream* stream, int count) {
  std::vector<T> swapped(src, src + count);
  for (int i = 0; i < count; i++) swapped[i] = SwapHelper(src[i]);
  WriteArrayWithoutSwap<T>(swapped.data(), stream, count);
}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
template <typename T>
inline T WriteLE(InputStream* stream, T value) {
  return WriteSwap<T>(stream, value);
}
template <int count, typename T>
inline void WriteArrayLE(T* src, InputStream* stream) {
  WriteArraySwap<count, t>(src, stream);
}
template <typename T>
inline void WriteArrayLE(T* src, InputStream* stream, int count) {
  WriteArraySwap<T>(src, stream, count);
}
template <typename T>
inline T WriteBE(InputStream* stream, T value) {
  return WriteWithoutSwap<T>(stream, value);
}
template <int count, typename T>
inline void WriteArrayBE(T* src, InputStream* stream) {
  WriteArrayWithoutSwap<count, T>(src, stream);
}
template <typename T>
inline void WriteArrayBE(T* src, InputStream* stream, int count) {
  WriteArrayWithoutSwap<T>(src, stream, count);
}
#else
template <typename T>
inline void WriteLE(Stream* stream, T value) {
  return WriteWithoutSwap<T>(stream, value);
}
template <int count, typename T>
inline void WriteArrayLE(T* src, Stream* stream) {
  WriteArrayWithoutSwap<count, T>(src, stream);
}
template <typename T>
inline void WriteArrayLE(T* src, Stream* stream, int count) {
  WriteArrayWithoutSwap<T>(src, stream, count);
}
template <typename T>
inline void WriteBE(Stream* stream, T value) {
  return WriteSwap<T>(stream, value);
}
template <int count, typename T>
inline void WriteArrayBE(T* src, Stream* stream) {
  WriteArraySwap<count, T>(src, stream);
}
template <typename T>
inline void WriteArrayBE(T* src, Stream* stream, int count) {
  WriteArraySwap<T>(src, stream, count);
}
#endif

inline void WriteVec2LE(float* src, Stream* stream) {
  WriteArrayLE<2>(src, stream);
}
inline void WriteVec2LE(glm::vec2* src, Stream* stream) {
  WriteArrayLE<2>((float*)src, stream);
}
inline void WriteVec3LE(float* src, Stream* stream) {
  WriteArrayLE<3>(src, stream);
}
inline void WriteVec3LE(glm::vec3* src, Stream* stream) {
  WriteArrayLE<3>((float*)src, stream);
}
inline void WriteVec4LE(float* src, Stream* stream) {
  WriteArrayLE<4>(src, stream);
}
inline void WriteVec4LE(glm::vec4* src, Stream* stream) {
  WriteArrayLE<4>((float*)src, stream);
}
inline void WriteMat4LE(float* src, Stream* stream) {
  WriteArrayLE<16>(src, stream);
}
inline void WriteMat4LE(glm::mat4* src, Stream* stream) {
  WriteArrayLE<16>((float*)src, stream);
}

inline void WriteVec2BE(float* src, Stream* stream) {
  WriteArrayBE<2>(src, stream);
}
inline void WriteVec2BE(glm::vec2* src, Stream* stream) {
  WriteArrayBE<2>((float*)src, stream);
}
inline void WriteVec3BE(float* src, Stream* stream) {
  WriteArrayBE<3>(src, stream);
}
inline void WriteVec3BE(glm::vec3* src, Stream* stream) {
  WriteArrayBE<3>((float*)src, stream);
}
inline void WriteVec4BE(float* src, Stream* stream) {
  WriteArrayBE<4>(src, stream);
}
inline void WriteVec4BE(glm::vec4* src, Stream* stream) {
  WriteArrayBE<4>((float*)src, stream);
}
inline void WriteMat4BE(float* src, Stream* stream) {
  WriteArrayBE<16>(src, stream);
}
inline void WriteMat4BE(glm::mat4* src, Stream* stream) {
  WriteArrayBE<16>((float*)src, stream);
}

}  // namespace Io
}  // namespace Impacto