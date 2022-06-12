#pragma once

#include "impacto.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>
#include <SDL_stdinc.h>
#include <string>

// TODO own _malloca for gcc

#if defined(_malloca)
#define ImpStackAlloc _malloca
#define ImpStackFree _freea
#else
#define ImpStackAlloc malloc
#define ImpStackFree free
#endif

namespace Impacto {

glm::mat2 Rotate2D(float angle);

struct Rect;

struct RectF {
  float X;
  float Y;
  float Width;
  float Height;

  RectF();
  RectF(float x, float y, float width, float height);
  RectF(Rect const& rect);

  glm::vec2 Center() const;
  // Rect is rotated around center
  bool ContainsPoint(glm::vec2 point, float angle = 0.0f) const;
  bool Intersects(RectF const& rect) const;
};

struct Rect {
  int X;
  int Y;
  int Width;
  int Height;

  Rect();
  Rect(int x, int y, int width, int height);
  Rect(RectF const& rect);

  glm::ivec2 Center() const;
  // Rect is rotated around center
  bool ContainsPoint(glm::ivec2 point, float angle = 0.0f) const;
};

glm::vec2 DesignToNDC(glm::vec2 xy);
RectF DesignToNDC(RectF const& rect);

glm::vec4 RgbIntToFloat(uint32_t rgb);

uint32_t GetHashCode(uint8_t* data, int length);

char* DumpMat4(glm::mat4* matrix, const char* columnSeparator = "\t",
               const char* rowSeparator = "\n");

// Thanks https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
inline int Uint32Log2(uint32_t v) {
  unsigned int const b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
  unsigned int const S[] = {1, 2, 4, 8, 16};
  int i;

  unsigned int r = 0;       // result of log2(v) will go here
  for (i = 4; i >= 0; i--)  // unroll for speed...
  {
    if (v & b[i]) {
      v >>= S[i];
      r |= S[i];
    }
  }
  return r;
}

// lots of guessing, again...
inline glm::vec3 LookAtEulerZYX(glm::vec3 from, glm::vec3 to,
                                glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)) {
  glm::vec3 result(0.0f);

  glm::vec3 forward = glm::normalize(from - to);

  result.x =
      atan2f(forward.y, sqrtf(forward.x * forward.x + forward.z * forward.z));
  result.y = atan2f(forward.x, forward.z) - M_PI;

  return result;
}

// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Euler_Angles_to_Quaternion_Conversion
// + guessing at the order :)
inline void eulerZYXToQuat(glm::vec3 const* zyx, glm::quat* quat) {
  float cosy = cos(zyx->z * 0.5f);
  float siny = sin(zyx->z * 0.5f);
  float cosr = cos(zyx->x * 0.5f);
  float sinr = sin(zyx->x * 0.5f);
  float cosp = cos(zyx->y * 0.5f);
  float sinp = sin(zyx->y * 0.5f);

  // This is currently slower than the scalar variant
  // TODO use sse swizzle instead of set
#if defined(__SSE2__) && 0
  __m128 a = _mm_set_ps(cosy, siny, cosy, cosy);
  __m128 b = _mm_set_ps(cosr, cosr, cosr, sinr);
  a = _mm_mul_ps(a, b);
  b = _mm_set_ps(cosp, cosp, sinp, cosp);
  a = _mm_mul_ps(a, b);

  b = _mm_set_ps(siny, cosy, siny, siny);
  __m128 c = _mm_set_ps(sinr, sinr, sinr, cosr);
  b = _mm_mul_ps(b, c);
  c = _mm_set_ps(sinp, sinp, cosp, sinp);
  b = _mm_mul_ps(b, c);

  const __m128 d = _mm_set_ps(1.0f, -1.0f, 1.0f, -1.0f);
  b = _mm_mul_ps(b, d);

  *(__m128*)quat = _mm_add_ps(a, b);
#else
  quat->x = cosy * sinr * cosp - siny * cosr * sinp;
  quat->y = cosy * cosr * sinp + siny * sinr * cosp;
  quat->z = siny * cosr * cosp - cosy * sinr * sinp;
  quat->w = cosy * cosr * cosp + siny * sinr * sinp;
#endif
}

inline float DegToRad(float deg) { return deg * (float)M_PI / 180.0f; }
inline float RadToDeg(float rad) { return rad * 180.0f / (float)M_PI; }
inline float NormalizeDeg(float deg) {
  deg = fmodf(deg + 180, 360);
  if (deg < 0) deg += 360;
  return deg - 180;
}
inline float NormalizeRad(float rad) {
  rad = fmodf(rad + (float)M_PI, 2.0f * (float)M_PI);
  if (rad < 0) rad += 2.0 * M_PI;
  return rad - M_PI;
}

inline bool StringEndsWith(std::string const& str, std::string const& ending) {
  if (str.length() < ending.length()) return false;
  return std::equal(ending.rbegin(), ending.rend(), str.rbegin());
}

inline bool StringEndsWithCi(std::string const& str,
                             std::string const& ending) {
  if (str.length() < ending.length()) return false;
  return 0 == SDL_strcasecmp(str.c_str() + str.length() - ending.length(),
                             ending.c_str());
}

}  // namespace Impacto