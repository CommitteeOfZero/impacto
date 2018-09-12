#include <SDL_endian.h>
#include <glm/gtc/type_ptr.hpp>

#include "io.h"

namespace Impacto {
float ReadFloatLE32(SDL_RWops* src) {
  float value;
  SDL_RWread(src, &value, sizeof(value), 1);
  return SDL_SwapFloatLE(value);
}

void ReadArrayFloatLE32(float* dest, SDL_RWops* src, uint32_t count) {
  SDL_RWread(src, dest, sizeof(float), count);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  for (int i = 0; i < count; i++) dest[i] = SDL_SwapFloatLE(dest[i]);
#endif
}

void ReadArrayLE16(uint16_t* dest, SDL_RWops* src, uint32_t count) {
  SDL_RWread(src, dest, sizeof(uint16_t), count);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  for (int i = 0; i < count; i++) dest[i] = SDL_SwapLE16(dest[i]);
#endif
}
void ReadArrayLE16(int16_t* dest, SDL_RWops* src, uint32_t count) {
  SDL_RWread(src, dest, sizeof(int16_t), count);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  for (int i = 0; i < count; i++)
    *reinterpret_cast<uint16_t*>(dest + i) =
        SDL_SwapLE16(reinterpret_cast<uint16_t>(dest[i]));
#endif
}
void ReadArrayLE32(uint32_t* dest, SDL_RWops* src, uint32_t count) {
  SDL_RWread(src, dest, sizeof(uint32_t), count);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  for (int i = 0; i < count; i++) dest[i] = SDL_SwapLE32(dest[i]);
#endif
}
void ReadArrayLE32(int32_t* dest, SDL_RWops* src, uint32_t count) {
  SDL_RWread(src, dest, sizeof(int32_t), count);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  for (int i = 0; i < count; i++)
    *reinterpret_cast<uint32_t*>(dest + i) =
        SDL_SwapLE32(reinterpret_cast<uint32_t>(dest[i]));
#endif
}

void ReadVec2LE32(float* dest, SDL_RWops* src) {
  ReadArrayFloatLE32(dest, src, 2);
}
void ReadVec2LE32(glm::vec2* dest, SDL_RWops* src) {
  ReadArrayFloatLE32((float*)dest, src, 2);
}
void ReadVec3LE32(float* dest, SDL_RWops* src) {
  ReadArrayFloatLE32(dest, src, 3);
}
void ReadVec3LE32(glm::vec3* dest, SDL_RWops* src) {
  ReadArrayFloatLE32((float*)dest, src, 3);
}
void ReadVec4LE32(float* dest, SDL_RWops* src) {
  ReadArrayFloatLE32(dest, src, 4);
}
void ReadVec4LE32(glm::vec4* dest, SDL_RWops* src) {
  ReadArrayFloatLE32((float*)dest, src, 4);
}
void ReadMat4LE32(float* dest, SDL_RWops* src) {
  ReadArrayFloatLE32(dest, src, 4 * 4);
}
void ReadMat4LE32(glm::mat4* dest, SDL_RWops* src) {
  ReadArrayFloatLE32((float*)dest, src, 4 * 4);
}

}  // namespace Impacto