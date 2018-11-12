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
inline void ReadArrayWithoutSwap(InputStream* stream, T* dest) {
  stream->Read(dest, count * sizeof(T));
}
template <typename T>
inline void ReadArrayWithoutSwap(InputStream* stream, T* dest, int count) {
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
inline void ReadArraySwap(InputStream* stream, uint16_t* dest) {
  ReadArrayWithoutSwap<count>(stream, dest);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap16(dest[i]);
}
inline void ReadArraySwap(InputStream* stream, uint16_t* dest, int count) {
  ReadArrayWithoutSwap(stream, dest, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap16(dest[i]);
}
template <int count>
inline void ReadArraySwap(InputStream* stream, uint32_t* dest) {
  ReadArrayWithoutSwap<count>(stream, dest);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap32(dest[i]);
}
inline void ReadArraySwap(InputStream* stream, uint32_t* dest, int count) {
  ReadArrayWithoutSwap(stream, dest, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap32(dest[i]);
}
template <int count>
inline void ReadArraySwap(InputStream* stream, uint64_t* dest) {
  ReadArrayWithoutSwap<count>(stream, dest);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap64(dest[i]);
}
inline void ReadArraySwap(InputStream* stream, uint64_t* dest, int count) {
  ReadArrayWithoutSwap(stream, dest, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap64(dest[i]);
}
template <int count>
inline void ReadArraySwap(InputStream* stream, float* dest) {
  ReadArrayWithoutSwap<count>(stream, dest);
  for (int i = 0; i < count; i++) dest[i] = SDL_SwapFloat(dest[i]);
}
inline void ReadArraySwap(InputStream* stream, float* dest, int count) {
  ReadArrayWithoutSwap(stream, dest, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_SwapFloat(dest[i]);
}
template <int count>
inline void ReadArraySwap(InputStream* stream, double* dest) {
  ReadArrayWithoutSwap<count>(stream, dest);
  uint64_t* _dest = (uint64_t*)dest;
  for (int i = 0; i < count; i++) {
    _dest[i] = SDL_Swap64(_dest[i]);
  }
}
inline void ReadArraySwap(InputStream* stream, double* dest, int count) {
  ReadArrayWithoutSwap(stream, dest, count);
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
inline void ReadArrayLE(InputStream* stream, T* dest) {
  ReadArraySwap<count>(stream, dest);
}
template <typename T>
inline void ReadArrayLE(InputStream* stream, T* dest, int count) {
  ReadArraySwap(stream, dest, count);
}
template <typename T>
inline T ReadBE(InputStream* stream) {
  return ReadWithoutSwap<T>(stream);
}
template <int count, typename T>
inline void ReadArrayBE(InputStream* stream, T* dest) {
  ReadArrayWithoutSwap<count>(stream, dest);
}
template <typename T>
inline void ReadArrayBE(InputStream* stream, T* dest, int count) {
  ReadArrayWithoutSwap(stream, dest, count);
}
#else
template <typename T>
inline T ReadLE(InputStream* stream) {
  return ReadWithoutSwap<T>(stream);
}
template <int count, typename T>
inline void ReadArrayLE(InputStream* stream, T* dest) {
  ReadArrayWithoutSwap<count>(stream, dest);
}
template <typename T>
inline void ReadArrayLE(InputStream* stream, T* dest, int count) {
  ReadArrayWithoutSwap(stream, dest, count);
}
template <typename T>
inline T ReadBE(InputStream* stream) {
  return ReadSwap<T>(stream);
}
template <int count, typename T>
inline void ReadArrayBE(InputStream* stream, T* dest) {
  ReadArraySwap<count>(stream, dest);
}
template <typename T>
inline void ReadArrayBE(InputStream* stream, T* dest, int count) {
  ReadArraySwap(stream, dest, count);
}
#endif

inline void ReadVec2LE(InputStream* stream, float* dest) {
  ReadArrayLE<2>(stream, dest);
}
inline void ReadVec2LE(InputStream* stream, glm::vec2* dest) {
  ReadArrayLE<2>(stream, (float*)dest);
}
inline void ReadVec3LE(InputStream* stream, float* dest) {
  ReadArrayLE<3>(stream, dest);
}
inline void ReadVec3LE(InputStream* stream, glm::vec3* dest) {
  ReadArrayLE<3>(stream, (float*)dest);
}
inline void ReadVec4LE(InputStream* stream, float* dest) {
  ReadArrayLE<4>(stream, dest);
}
inline void ReadVec4LE(InputStream* stream, glm::vec4* dest) {
  ReadArrayLE<4>(stream, (float*)dest);
}
inline void ReadMat4LE(InputStream* stream, float* dest) {
  ReadArrayLE<16>(stream, dest);
}
inline void ReadMat4LE(InputStream* stream, glm::mat4* dest) {
  ReadArrayLE<16>(stream, (float*)dest);
}

inline void ReadVec2BE(InputStream* stream, float* dest) {
  ReadArrayBE<2>(stream, dest);
}
inline void ReadVec2BE(InputStream* stream, glm::vec2* dest) {
  ReadArrayBE<2>(stream, (float*)dest);
}
inline void ReadVec3BE(InputStream* stream, float* dest) {
  ReadArrayBE<3>(stream, dest);
}
inline void ReadVec3BE(InputStream* stream, glm::vec3* dest) {
  ReadArrayBE<3>(stream, (float*)dest);
}
inline void ReadVec4BE(InputStream* stream, float* dest) {
  ReadArrayBE<4>(stream, dest);
}
inline void ReadVec4BE(InputStream* stream, glm::vec4* dest) {
  ReadArrayBE<4>(stream, (float*)dest);
}
inline void ReadMat4BE(InputStream* stream, float* dest) {
  ReadArrayBE<16>(stream, dest);
}
inline void ReadMat4BE(InputStream* stream, glm::mat4* dest) {
  ReadArrayBE<16>(stream, (float*)dest);
}

}  // namespace Io
}  // namespace Impacto