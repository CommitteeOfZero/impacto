#pragma once

#include "impacto.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>
#include <SDL_stdinc.h>
#include <random>
#include <string>
#include <cctype>
#include <chrono>
#include <span>
#include <numbers>

#if defined(WIN32) || defined(_WIN32)
#include <malloc.h>
#endif

// TODO own _malloca for gcc

#if defined(_malloca)
#define ImpStackAlloc _malloca
#define ImpStackFree _freea
#else
#define ImpStackAlloc malloc
#define ImpStackFree free
#endif

#define TIME_CODE(code)                                      \
  do {                                                       \
    auto start = std::chrono::high_resolution_clock::now();  \
    code;                                                    \
    auto end = std::chrono::high_resolution_clock::now();    \
    std::chrono::duration<double> elapsed = end - start;     \
    ImpLog(LogLevel::Info, LogChannel::General,              \
           "Time elapsed: {:f} seconds\n", elapsed.count()); \
  } while (0)

namespace Impacto {
struct string_hash {
  using is_transparent = void;  // enable heterogeneous overloads
  using is_avalanching = void;  // mark class as high quality avalanching hash

  [[nodiscard]] auto operator()(std::string_view str) const noexcept
      -> uint64_t {
    return ankerl::unordered_dense::hash<std::string_view>{}(str);
  }
};

template <typename T, typename... Ts>
concept is_any_of = std::disjunction_v<std::is_same<T, Ts>...>;

glm::mat2 Rotate2D(float angle);

glm::mat4 TransformationMatrix(glm::vec2 scalingOrigin, glm::vec2 scaling,
                               glm::vec2 rotationOrigin = glm::vec2(0.0f),
                               float rotation = 0.0f,
                               glm::vec2 translation = glm::vec2(0.0f));
glm::mat4 TransformationMatrix(glm::vec3 scalingOrigin, glm::vec3 scaling,
                               glm::vec3 rotationOrigin = glm::vec3(0.0f),
                               glm::quat rotation = glm::quat(),
                               glm::vec3 translation = glm::vec3(0.0f));
inline glm::mat4 TransformationMatrix(glm::vec2 scalingOrigin,
                                      glm::vec2 scaling,
                                      glm::vec3 rotationOrigin,
                                      glm::quat rotation,
                                      glm::vec2 translation = glm::vec2(0.0f)) {
  return TransformationMatrix(glm::vec3(scalingOrigin, 0.0f),
                              glm::vec3(scaling, 1.0f), rotationOrigin,
                              rotation, glm::vec3(translation, 0.0f));
}

glm::vec4 TransformVector(glm::vec2 pos, glm::vec2 scalingOrigin,
                          glm::vec2 scaling,
                          glm::vec2 rotationOrigin = glm::vec2(0.0f),
                          float rotation = 0.0f,
                          glm::vec2 translation = glm::vec2(0.0f));
glm::vec4 TransformVector(glm::vec3 pos, glm::vec3 scalingOrigin,
                          glm::vec3 scaling,
                          glm::vec3 rotationOrigin = glm::vec3(0.0f),
                          glm::quat rotation = glm::quat(),
                          glm::vec3 translation = glm::vec3(0.0f));
inline glm::vec4 TransformVector(glm::vec2 pos, glm::vec2 scalingOrigin,
                                 glm::vec2 scaling, glm::vec3 rotationOrigin,
                                 glm::quat rotation,
                                 glm::vec2 translation = glm::vec2(0.0f)) {
  return TransformVector({pos, 0.0f}, glm::vec3(scalingOrigin, 0.0f),
                         glm::vec3(scaling, 1.0f), rotationOrigin, rotation,
                         {translation, 0.0f});
}

struct Rect;
struct CornersQuad;

struct RectF {
  float X = 0;
  float Y = 0;
  float Width = 0;
  float Height = 0;

  constexpr RectF() {}
  constexpr RectF(float x, float y, float width, float height)
      : X(x), Y(y), Width(width), Height(height) {}
  constexpr RectF(Rect const& rect);

  // RectF is rotated around center
  constexpr glm::vec2 Center() const {
    return glm::vec2(X + Width / 2.0f, Y + Height / 2.0f);
  }

  constexpr bool ContainsPoint(glm::vec2 point, float angle = 0.0f) const {
    point -= Center();
    if (angle != 0.0f) {
      point = Rotate2D(-angle) * point;
    }

    return point.x >= -Width / 2.0f && point.x <= Width / 2.0f &&
           point.y >= -Height / 2.0f && point.y <= Height / 2.0f;
  }

  constexpr bool Intersects(RectF const& rect) const {
    return (X <= rect.X + rect.Width && rect.X <= X + Width &&
            Y <= rect.Y + rect.Height && rect.Y <= Y + Height);
  }

  constexpr bool Contains(RectF const& rect) const {
    return (X <= rect.X && rect.X + rect.Width <= X + Width && Y <= rect.Y &&
            rect.Y + rect.Height <= Y + Height);
  }

  constexpr RectF operator+(const glm::vec2 movementVector) const {
    return RectF(X + movementVector.x, Y + movementVector.y, Width, Height);
  }

  constexpr RectF operator-(const glm::vec2 movementVector) const {
    return RectF(X - movementVector.x, Y - movementVector.y, Width, Height);
  }

  constexpr RectF& operator+=(const glm::vec2 movementVector) {
    X += movementVector.x;
    Y += movementVector.y;

    return *this;
  }

  constexpr RectF& operator-=(const glm::vec2 movementVector) {
    X -= movementVector.x;
    Y -= movementVector.y;

    return *this;
  }

  constexpr bool operator==(RectF const& other) const {
    constexpr auto fltCmp = [](float a, float b) {
      const auto absDiff = (a - b) < 0 ? b - a : a - b;
      return absDiff < std::numeric_limits<float>::epsilon();
    };
    return fltCmp(X, other.X) && fltCmp(Y, other.Y) &&
           fltCmp(Width, other.Width) && fltCmp(Height, other.Height);
  }
  constexpr bool operator!=(RectF const& other) const {
    return !(*this == other);
  }

  constexpr glm::vec2 GetPos() const { return glm::vec2(X, Y); }
  constexpr glm::vec2 GetSize() const { return glm::vec2(Width, Height); }

  static RectF Coalesce(const RectF& first, const RectF& second);
  static RectF BoundingBox(const RectF& first, const CornersQuad& second);
  static RectF BoundingBox(const CornersQuad& first, const RectF& second);

  CornersQuad Transform(glm::mat4 transformation) const;
  CornersQuad Transform(
      const std::function<glm::vec2(glm::vec2)>& transformation) const;

  RectF& Translate(glm::vec2 offset) { return *this += offset; }
  RectF& Translate(float dx, float dy) { return Translate({dx, dy}); }

  RectF& Scale(glm::vec2 scaling, glm::vec2 origin);
  RectF& ScaleAroundCenter(glm::vec2 scaling) {
    return Scale(scaling, Center());
  }

  CornersQuad Rotate(float angle, glm::vec2 origin) const;
  CornersQuad RotateAroundCenter(float angle) const;
  CornersQuad Rotate(glm::quat rotation, glm::vec3 origin) const;
  CornersQuad Rotate(glm::quat rotation, glm::vec3 origin, float depth,
                     glm::vec2 vanishingPoint, bool stayInScreen = false) const;

  CornersQuad FlipVertical() const;
  CornersQuad FlipHorizontal() const;
};

inline CornersQuad operator*(const glm::mat4 transformation, RectF rect);

struct Rect {
  int X = 0;
  int Y = 0;
  int Width = 0;
  int Height = 0;

  constexpr Rect() {}
  constexpr Rect(int x, int y, int width, int height)
      : X(x), Y(y), Width(width), Height(height) {}
  constexpr Rect(RectF const& rect)
      : Rect((int)rect.X, (int)rect.Y, (int)rect.Width, (int)rect.Height) {}

  // Rect is rotated around center
  constexpr glm::ivec2 Center() const {
    return glm::ivec2(X + Width / 2, Y + Height / 2);
  }
  constexpr bool ContainsPoint(glm::vec2 point, float angle = 0.0f) const {
    point -= Center();
    if (angle != 0.0f) {
      point = (glm::ivec2)(Rotate2D(-angle) * (glm::vec2)point);
    }
    return point.x >= -Width / 2 && point.x <= Width / 2 &&
           point.y >= -Height / 2 && point.y <= Height / 2;
  }

  constexpr bool Intersects(Rect const& rect) const {
    return (X <= rect.X + rect.Width && rect.X <= X + Width &&
            Y <= rect.Y + rect.Height && rect.Y <= Y + Height);
  }

  constexpr bool operator==(Rect const& other) const {
    return X == other.X && Y == other.Y && Width == other.Width &&
           Height == other.Height;
  }

  constexpr bool operator!=(Rect const& other) const {
    return !(*this == other);
  }
};

struct CornersQuad {
  glm::vec2 TopLeft;
  glm::vec2 BottomLeft;
  glm::vec2 TopRight;
  glm::vec2 BottomRight;

  constexpr CornersQuad(glm::vec2 tl, glm::vec2 bl, glm::vec2 tr, glm::vec2 br)
      : TopLeft(tl), BottomLeft(bl), TopRight(tr), BottomRight(br) {}

  constexpr CornersQuad(RectF const& rect)
      : CornersQuad({rect.X, rect.Y}, {rect.X, rect.Y + rect.Height},
                    {rect.X + rect.Width, rect.Y},
                    {rect.X + rect.Width, rect.Y + rect.Height}) {}

  constexpr CornersQuad(Rect const& rect)
      : CornersQuad(
            {(float)rect.X, (float)rect.Y},
            {(float)rect.X, (float)(rect.Y + rect.Height)},
            {(float)(rect.X + rect.Width), (float)rect.Y},
            {(float)(rect.X + rect.Width), (float)(rect.Y + rect.Height)}) {}

  constexpr glm::vec2 Center() const {
    return (TopLeft + BottomLeft + TopRight + BottomRight) / 4.0f;
  }

  CornersQuad& Transform(glm::mat4 transformation);
  CornersQuad& Transform(
      const std::function<glm::vec2(glm::vec2)>& transformation);

  CornersQuad& Translate(glm::vec2 offset);
  CornersQuad& Translate(float dx, float dy) { return Translate({dx, dy}); }

  CornersQuad& Scale(glm::vec2 scaling, glm::vec2 origin);
  CornersQuad& ScaleAroundCenter(glm::vec2 scaling, glm::vec2 origin) {
    return Scale(scaling, Center());
  }

  CornersQuad& Rotate(float angle, glm::vec2 origin);
  CornersQuad& RotateAroundCenter(float angle) {
    return Rotate(angle, Center());
  }
  CornersQuad& Rotate(glm::quat rotation, glm::vec3 origin);
  CornersQuad& Rotate(glm::quat rotation, glm::vec3 origin, float depth,
                      glm::vec2 vanishingPoint, bool stayInScreen = false);

  CornersQuad& FlipVertical() {
    std::swap(TopLeft, BottomLeft);
    std::swap(TopRight, BottomRight);
    return *this;
  }

  CornersQuad& FlipHorizontal() {
    std::swap(TopLeft, TopRight);
    std::swap(BottomLeft, BottomRight);
    return *this;
  }
};

inline CornersQuad operator*(const glm::mat4 transformation, CornersQuad quad) {
  return quad.Transform(transformation);
}

inline constexpr RectF::RectF(Rect const& rect)
    : RectF((float)rect.X, (float)rect.Y, (float)rect.Width,
            (float)rect.Height) {}

constexpr glm::vec4 RgbIntToFloat(uint32_t rgb) {
  return glm::vec4{(float)((rgb >> 16) & 0xFF) / 255.0f,
                   (float)((rgb >> 8) & 0xFF) / 255.0f,
                   (float)((rgb >> 0) & 0xFF) / 255.0f, 1.0f};
}

uint32_t GetHashCode(uint8_t* data, int length);

std::string DumpMat4(glm::mat4* matrix, std::string_view columnSeparator = "\t",
                     std::string_view rowSeparator = "\n");

// Thanks https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
constexpr int Uint32Log2(uint32_t v) {
  unsigned int const b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
  unsigned int const S[] = {1, 2, 4, 8, 16};

  unsigned int r = 0;           // result of log2(v) will go here
  for (int i = 4; i >= 0; i--)  // unroll for speed...
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
  result.y = atan2f(forward.x, forward.z) - (float)M_PI;

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

constexpr float DegToRad(float deg) { return deg * (float)M_PI / 180.0f; }
constexpr float RadToDeg(float rad) { return rad * 180.0f / (float)M_PI; }
inline float NormalizeDeg(float deg) {
  deg = fmodf(deg + 180, 360);
  if (deg < 0) deg += 360;
  return deg - 180;
}
inline float NormalizeRad(float rad) {
  rad = fmodf(rad + (float)M_PI, 2.0f * (float)M_PI);
  if (rad < 0) rad += 2.0f * (float)M_PI;
  return rad - (float)M_PI;
}
constexpr float ScrWorkAngleToRad(int angle) {
  return ((float)angle * 2.0f * (float)std::numbers::pi) / (float)(1 << 16);
}

inline glm::quat ScrWorkAnglesToQuaternion(int x, int y, int z) {
  return glm::quat(
      {ScrWorkAngleToRad(x), ScrWorkAngleToRad(y), ScrWorkAngleToRad(z)});
}

inline glm::quat ScrWorkAngleZToQuaternion(int angle) {
  return ScrWorkAnglesToQuaternion(0, 0, angle);
}

inline glm::quat AxisAngleToQuaternion(glm::vec3 axis, float angle) {
  return glm::quat(cos(angle / 2.0f), sin(angle / 2.0f) * axis);
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

// Trim whitespace in-place
inline void TrimString(std::string& str) {
  // from the left
  str.erase(str.begin(),
            std::find_if(str.begin(), str.end(),
                         [](unsigned char c) { return !std::isspace(c); }));

  // from the right
  str.erase(std::find_if(str.rbegin(), str.rend(),
                         [](unsigned char c) { return !std::isspace(c); })
                .base(),
            str.end());
}

inline void MakeLowerCase(std::string& str) {
  std::transform(str.begin(), str.end(), str.begin(),
                 [](const char c) { return std::tolower(c); });
}

template <typename T>
T UnalignedRead(void* ptr) {
  static_assert(std::is_trivially_copyable<T>::value,
                "!std::is_trivially_copyable<T>");
  T value;
  memcpy(&value, ptr, sizeof value);
  return value;
}

template <typename T>
void UnalignedWrite(void* ptr, T value) {
  static_assert(std::is_trivially_copyable<T>::value,
                "!std::is_trivially_copyable<T>");
  memcpy(ptr, &value, sizeof value);
}

template <class Enum>
constexpr std::underlying_type_t<Enum> to_underlying(Enum e) noexcept {
  return static_cast<std::underlying_type_t<Enum>>(e);
}

int ResizeImage(Rect const& srcRect, Rect const& dstRect,
                std::span<uint8_t> src, std::span<uint8_t> dst);

inline int CALCrnd(int max) {
  static std::random_device rd;
  static std::mt19937 gen(std::random_device{}());
  static std::uniform_int_distribution<> distr(0, 0x7FFF);
  return distr(gen) * max >> 0xf;
}

tm CurrentDateTime();

}  // namespace Impacto