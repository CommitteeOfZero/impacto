#pragma once

#include "stream.h"
#include <algorithm>
#include <string.h>

namespace Impacto {
namespace Io {

template <class T>
class OutputStream {
 public:
  virtual ~OutputStream() {}
  int64_t WritePosition = 0;

  bool IsSeekSlow = false;
  bool IsMemory = false;

  virtual int64_t Write(void* buffer, int64_t sz) = 0;
  virtual int64_t SeekWrite(int64_t offset, int origin) = 0;
};

inline uint8_t WriteU8(Stream* stream) {
  uint8_t result;
  stream->Write(&result, 1);
  return result;
}

template <typename T>
inline T WriteWithoutSwap(Stream* stream) {
  T result;
  stream->Write(&result, sizeof(T));
  return result;
}

template <int count, typename T>
inline void WriteArrayWithoutSwap(T* dest, Stream* stream) {
  stream->Write(dest, count * sizeof(T));
}
template <typename T>
inline void WriteArrayWithoutSwap(T* dest, Stream* stream, int count) {
  stream->Write(dest, count * sizeof(T));
}

template <typename T>
inline T WriteSwap(Stream* stream);
template <>
inline uint16_t WriteSwap<uint16_t>(Stream* stream) {
  uint16_t result;
  stream->Write(&result, sizeof(uint16_t));
  return SDL_Swap16(result);
}
template <>
inline int16_t WriteSwap<int16_t>(Stream* stream) {
  int16_t result;
  stream->Write(&result, sizeof(int16_t));
  return SDL_Swap16(result);
}
template <>
inline uint32_t WriteSwap<uint32_t>(Stream* stream) {
  uint32_t result;
  stream->Write(&result, sizeof(uint32_t));
  return SDL_Swap32(result);
}
template <>
inline int32_t WriteSwap<int32_t>(Stream* stream) {
  int32_t result;
  stream->Write(&result, sizeof(int32_t));
  return SDL_Swap32(result);
}
template <>
inline uint64_t WriteSwap<uint64_t>(Stream* stream) {
  uint64_t result;
  stream->Write(&result, sizeof(uint64_t));
  return SDL_Swap64(result);
}
template <>
inline int64_t WriteSwap<int64_t>(Stream* stream) {
  int64_t result;
  stream->Write(&result, sizeof(int64_t));
  return SDL_Swap64(result);
}
template <>
inline float WriteSwap<float>(Stream* stream) {
  float result;
  stream->Write(&result, sizeof(float));
  return SDL_SwapFloat(result);
}
template <>
inline double WriteSwap<double>(Stream* stream) {
  union {
    double result;
    uint64_t result_int;
  } u;
  stream->Write(&u.result, sizeof(double));
  u.result_int = SDL_Swap64(u.result_int);
  return u.result;
}

template <int count>
inline void WriteArraySwap(uint16_t* dest, Stream* stream) {
  WriteArrayWithoutSwap<count>(dest, stream);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap16(dest[i]);
}
inline void WriteArraySwap(uint16_t* dest, Stream* stream, int count) {
  WriteArrayWithoutSwap(dest, stream, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap16(dest[i]);
}
template <int count>
inline void WriteArraySwap(int16_t* dest, Stream* stream) {
  WriteArrayWithoutSwap<count>(dest, stream);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap16(dest[i]);
}
inline void WriteArraySwap(int16_t* dest, Stream* stream, int count) {
  WriteArrayWithoutSwap(dest, stream, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap16(dest[i]);
}
template <int count>
inline void WriteArraySwap(uint32_t* dest, Stream* stream) {
  WriteArrayWithoutSwap<count>(dest, stream);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap32(dest[i]);
}
inline void WriteArraySwap(uint32_t* dest, Stream* stream, int count) {
  WriteArrayWithoutSwap(dest, stream, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap32(dest[i]);
}
template <int count>
inline void WriteArraySwap(int32_t* dest, Stream* stream) {
  WriteArrayWithoutSwap<count>(dest, stream);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap32(dest[i]);
}
inline void WriteArraySwap(int32_t* dest, Stream* stream, int count) {
  WriteArrayWithoutSwap(dest, stream, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap32(dest[i]);
}
template <int count>
inline void WriteArraySwap(uint64_t* dest, Stream* stream) {
  WriteArrayWithoutSwap<count>(dest, stream);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap64(dest[i]);
}
inline void WriteArraySwap(uint64_t* dest, Stream* stream, int count) {
  WriteArrayWithoutSwap(dest, stream, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap64(dest[i]);
}
template <int count>
inline void WriteArraySwap(int64_t* dest, Stream* stream) {
  WriteArrayWithoutSwap<count>(dest, stream);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap64(dest[i]);
}
inline void WriteArraySwap(int64_t* dest, Stream* stream, int count) {
  WriteArrayWithoutSwap(dest, stream, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_Swap64(dest[i]);
}
template <int count>
inline void WriteArraySwap(float* dest, Stream* stream) {
  WriteArrayWithoutSwap<count>(dest, stream);
  for (int i = 0; i < count; i++) dest[i] = SDL_SwapFloat(dest[i]);
}
inline void WriteArraySwap(float* dest, Stream* stream, int count) {
  WriteArrayWithoutSwap(dest, stream, count);
  for (int i = 0; i < count; i++) dest[i] = SDL_SwapFloat(dest[i]);
}
template <int count>
inline void WriteArraySwap(double* dest, Stream* stream) {
  WriteArrayWithoutSwap<count>(dest, stream);
  uint64_t* _dest = (uint64_t*)dest;
  for (int i = 0; i < count; i++) {
    _dest[i] = SDL_Swap64(_dest[i]);
  }
}
inline void WriteArraySwap(double* dest, Stream* stream, int count) {
  WriteArrayWithoutSwap(dest, stream, count);
  uint64_t* _dest = (uint64_t*)dest;
  for (int i = 0; i < count; i++) {
    _dest[i] = SDL_Swap64(_dest[i]);
  }
}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
template <typename T>
inline T WriteLE(Stream* stream) {
  return WriteSwap<T>(stream);
}
template <int count, typename T>
inline void WriteArrayLE(T* dest, Stream* stream) {
  WriteArraySwap<count>(dest, stream);
}
template <typename T>
inline void WriteArrayLE(T* dest, Stream* stream, int count) {
  WriteArraySwap(dest, stream, count);
}
template <typename T>
inline T WriteBE(Stream* stream) {
  return WriteWithoutSwap<T>(stream);
}
template <int count, typename T>
inline void WriteArrayBE(T* dest, Stream* stream) {
  WriteArrayWithoutSwap<count>(dest, stream);
}
template <typename T>
inline void WriteArrayBE(T* dest, Stream* stream, int count) {
  WriteArrayWithoutSwap(dest, stream, count);
}
#else
template <typename T>
inline T WriteLE(Stream* stream) {
  return WriteWithoutSwap<T>(stream);
}
template <int count, typename T>
inline void WriteArrayLE(T* dest, Stream* stream) {
  WriteArrayWithoutSwap<count>(dest, stream);
}
template <typename T>
inline void WriteArrayLE(T* dest, Stream* stream, int count) {
  WriteArrayWithoutSwap(dest, stream, count);
}
template <typename T>
inline T WriteBE(Stream* stream) {
  return WriteSwap<T>(stream);
}
template <int count, typename T>
inline void WriteArrayBE(T* dest, Stream* stream) {
  WriteArraySwap<count>(dest, stream);
}
template <typename T>
inline void WriteArrayBE(T* dest, Stream* stream, int count) {
  WriteArraySwap(dest, stream, count);
}
#endif

inline void WriteVec2LE(float* dest, Stream* stream) {
  WriteArrayLE<2>(dest, stream);
}
inline void WriteVec2LE(glm::vec2* dest, Stream* stream) {
  WriteArrayLE<2>((float*)dest, stream);
}
inline void WriteVec3LE(float* dest, Stream* stream) {
  WriteArrayLE<3>(dest, stream);
}
inline void WriteVec3LE(glm::vec3* dest, Stream* stream) {
  WriteArrayLE<3>((float*)dest, stream);
}
inline void WriteVec4LE(float* dest, Stream* stream) {
  WriteArrayLE<4>(dest, stream);
}
inline void WriteVec4LE(glm::vec4* dest, Stream* stream) {
  WriteArrayLE<4>((float*)dest, stream);
}
inline void WriteMat4LE(float* dest, Stream* stream) {
  WriteArrayLE<16>(dest, stream);
}
inline void WriteMat4LE(glm::mat4* dest, Stream* stream) {
  WriteArrayLE<16>((float*)dest, stream);
}

inline void WriteVec2BE(float* dest, Stream* stream) {
  WriteArrayBE<2>(dest, stream);
}
inline void WriteVec2BE(glm::vec2* dest, Stream* stream) {
  WriteArrayBE<2>((float*)dest, stream);
}
inline void WriteVec3BE(float* dest, Stream* stream) {
  WriteArrayBE<3>(dest, stream);
}
inline void WriteVec3BE(glm::vec3* dest, Stream* stream) {
  WriteArrayBE<3>((float*)dest, stream);
}
inline void WriteVec4BE(float* dest, Stream* stream) {
  WriteArrayBE<4>(dest, stream);
}
inline void WriteVec4BE(glm::vec4* dest, Stream* stream) {
  WriteArrayBE<4>((float*)dest, stream);
}
inline void WriteMat4BE(float* dest, Stream* stream) {
  WriteArrayBE<16>(dest, stream);
}
inline void WriteMat4BE(glm::mat4* dest, Stream* stream) {
  WriteArrayBE<16>((float*)dest, stream);
}

};

}  // namespace Io
}  // namespace Impacto