#pragma once

#include <glm/glm.hpp>
#include <SDL_rwops.h>

namespace Impacto {

enum IoError : int64_t {
  IoError_OK = 0,
  IoError_Fail = -1,
  IoError_Eof = -2,
  IoError_NotFound = -3
};

// TODO need signed integer versions of SDL_ReadLE16 / SDL_ReadLE32?

float ReadFloatLE32(SDL_RWops* src);

void ReadArrayFloatLE32(float* dest, SDL_RWops* src, uint32_t count);

void ReadArrayLE16(uint16_t* dest, SDL_RWops* src, uint32_t count);
void ReadArrayLE16(int16_t* dest, SDL_RWops* src, uint32_t count);
void ReadArrayLE32(uint32_t* dest, SDL_RWops* src, uint32_t count);
void ReadArrayLE32(int32_t* dest, SDL_RWops* src, uint32_t count);

void ReadVec2LE32(float* dest, SDL_RWops* src);
void ReadVec2LE32(glm::vec2* dest, SDL_RWops* src);
void ReadVec3LE32(float* dest, SDL_RWops* src);
void ReadVec3LE32(glm::vec3* dest, SDL_RWops* src);
void ReadVec4LE32(float* dest, SDL_RWops* src);
void ReadVec4LE32(glm::vec4* dest, SDL_RWops* src);
void ReadMat4LE32(float* dest, SDL_RWops* src);
void ReadMat4LE32(glm::mat4* dest, SDL_RWops* src);

}  // namespace Impacto