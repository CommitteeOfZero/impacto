#pragma once

#include "filemeta.h"
#include <stdio.h>
#include "io.h"
#include <SDL_endian.h>

namespace Impacto {
namespace Io {

class InputStream {
 public:
  virtual ~InputStream() {}
  FileMeta Meta;
  int64_t Position = 0;

  bool IsSeekSlow = false;
  bool IsMemory = false;

  virtual int64_t Read(void* buffer, int64_t sz) = 0;
  virtual int64_t Seek(int64_t offset, int origin) = 0;
  virtual IoError Duplicate(InputStream** outStream) = 0;
};

inline uint8_t ReadU8(InputStream* stream) {
  uint8_t result;
  stream->Read(&result, 1);
  return result;
}

template <typename T>
inline T ReadWithoutSwap(InputStream* stream) {
  T result;
  stream->Read(&result, sizeof(T));
  return result;
}

template <int count, typename T>
inline void ReadArrayWithoutSwap(T* dest, InputStream* stream) {
  stream->Read(dest, count * sizeof(T));
}
template <typename T>
inline void ReadArrayWithoutSwap(T* dest, InputStream* stream, int count) {
  stream->Read(dest, count * sizeof(T));
}

template <typename T>
inline T ReadSwap(InputStream* stream);
template <>
inline uint16_t ReadSwap<uint16_t>(InputStream* stream) {
  uint16_t result;
  stream->Read(&result, sizeof(uint16_t));
  return SDL_Swap16(result);
}
template <>
inline uint32_t ReadSwap<uint32_t>(InputStream* stream) {
  uint32_t result;
  stream->Read(&result, sizeof(uint32_t));
  return SDL_Swap32(result);
}
template <>
inline uint64_t ReadSwap<uint64_t>(InputStream* stream) {
  uint64_t result;
  stream->Read(&result, sizeof(uint64_t));
  return SDL_Swap64(result);
}
template <>
inline float ReadSwap<float>(InputStream* stream) {
  float result;
  stream->Read(&result, sizeof(float));
  return SDL_SwapFloat(result);
}
template <>
inline double ReadSwap<double>(InputStream* stream) {
  union {
    double result;
    uint64_t result_int;
  } u;
  stream->Read(&u.result, sizeof(double));
  u.result_int = SDL_Swap64(u.result_int);
  return u.result;
}

template <int count>
inline void ReadArraySwap(uint16_t* dest, InputStream* stream) {
  ReadArrayWithoutSwap<count>(dest, stream);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap16(dest[i]);
}
inline void ReadArraySwap(uint16_t* dest, InputStream* stream, int count) {
  ReadArrayWithoutSwap(dest, stream, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap16(dest[i]);
}
template <int count>
inline void ReadArraySwap(uint32_t* dest, InputStream* stream) {
  ReadArrayWithoutSwap<count>(dest, stream);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap32(dest[i]);
}
inline void ReadArraySwap(uint32_t* dest, InputStream* stream, int count) {
  ReadArrayWithoutSwap(dest, stream, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap32(dest[i]);
}
template <int count>
inline void ReadArraySwap(uint64_t* dest, InputStream* stream) {
  ReadArrayWithoutSwap<count>(dest, stream);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap64(dest[i]);
}
inline void ReadArraySwap(uint64_t* dest, InputStream* stream, int count) {
  ReadArrayWithoutSwap(dest, stream, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap64(dest[i]);
}
template <int count>
inline void ReadArraySwap(float* dest, InputStream* stream) {
  ReadArrayWithoutSwap<count>(dest, stream);
  for (int i = 0; i < count; i++) dest[i] = SDL_SwapFloat(dest[i]);
}
inline void ReadArraySwap(float* dest, InputStream* stream, int count) {
  ReadArrayWithoutSwap(dest, stream, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_SwapFloat(dest[i]);
}
template <int count>
inline void ReadArraySwap(double* dest, InputStream* stream) {
  ReadArrayWithoutSwap<count>(dest, stream);
  uint64_t* _dest = (uint64_t*)dest;
  for (int i = 0; i < count; i++) {
    _dest[i] = SDL_Swap64(_dest[i]);
  }
}
inline void ReadArraySwap(double* dest, InputStream* stream, int count) {
  ReadArrayWithoutSwap(dest, stream, count);
  uint64_t* _dest = (uint64_t*)dest;
  for (int i = 0; i < count; i++) {
    _dest[i] = SDL_Swap64(_dest[i]);
  }
}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
template <typename T>
inline T ReadLE(InputStream* stream) {
  return ReadSwap<T>(stream);
}
template <int count, typename T>
inline void ReadArrayLE(T* dest, InputStream* stream) {
  ReadArraySwap<count>(dest, stream);
}
template <typename T>
inline void ReadArrayLE(T* dest, InputStream* stream, int count) {
  ReadArraySwap(dest, stream, count);
}
template <typename T>
inline T ReadBE(InputStream* stream) {
  return ReadWithoutSwap<T>(stream);
}
template <int count, typename T>
inline void ReadArrayBE(T* dest, InputStream* stream) {
  ReadArrayWithoutSwap<count>(dest, stream);
}
template <typename T>
inline void ReadArrayBE(T* dest, InputStream* stream, int count) {
  ReadArrayWithoutSwap(dest, stream, count);
}
#else
template <typename T>
inline T ReadLE(InputStream* stream) {
  return ReadWithoutSwap<T>(stream);
}
template <int count, typename T>
inline void ReadArrayLE(T* dest, InputStream* stream) {
  ReadArrayWithoutSwap<count>(dest, stream);
}
template <typename T>
inline void ReadArrayLE(T* dest, InputStream* stream, int count) {
  ReadArrayWithoutSwap(dest, stream, count);
}
template <typename T>
inline T ReadBE(InputStream* stream) {
  return ReadSwap<T>(stream);
}
template <int count, typename T>
inline void ReadArrayBE(T* dest, InputStream* stream) {
  ReadArraySwap<count>(dest, stream);
}
template <typename T>
inline void ReadArrayBE(T* dest, InputStream* stream, int count) {
  ReadArraySwap(dest, stream, count);
}
#endif

inline void ReadVec2LE(float* dest, InputStream* stream) {
  ReadArrayLE<2>(dest, stream);
}
inline void ReadVec2LE(glm::vec2* dest, InputStream* stream) {
  ReadArrayLE<2>((float*)dest, stream);
}
inline void ReadVec3LE(float* dest, InputStream* stream) {
  ReadArrayLE<3>(dest, stream);
}
inline void ReadVec3LE(glm::vec3* dest, InputStream* stream) {
  ReadArrayLE<3>((float*)dest, stream);
}
inline void ReadVec4LE(float* dest, InputStream* stream) {
  ReadArrayLE<4>(dest, stream);
}
inline void ReadVec4LE(glm::vec4* dest, InputStream* stream) {
  ReadArrayLE<4>((float*)dest, stream);
}
inline void ReadMat4LE(float* dest, InputStream* stream) {
  ReadArrayLE<16>(dest, stream);
}
inline void ReadMat4LE(glm::mat4* dest, InputStream* stream) {
  ReadArrayLE<16>((float*)dest, stream);
}

inline void ReadVec2BE(float* dest, InputStream* stream) {
  ReadArrayBE<2>(dest, stream);
}
inline void ReadVec2BE(glm::vec2* dest, InputStream* stream) {
  ReadArrayBE<2>((float*)dest, stream);
}
inline void ReadVec3BE(float* dest, InputStream* stream) {
  ReadArrayBE<3>(dest, stream);
}
inline void ReadVec3BE(glm::vec3* dest, InputStream* stream) {
  ReadArrayBE<3>((float*)dest, stream);
}
inline void ReadVec4BE(float* dest, InputStream* stream) {
  ReadArrayBE<4>(dest, stream);
}
inline void ReadVec4BE(glm::vec4* dest, InputStream* stream) {
  ReadArrayBE<4>((float*)dest, stream);
}
inline void ReadMat4BE(float* dest, InputStream* stream) {
  ReadArrayBE<16>(dest, stream);
}
inline void ReadMat4BE(glm::mat4* dest, InputStream* stream) {
  ReadArrayBE<16>((float*)dest, stream);
}

}  // namespace Io
}  // namespace Impacto