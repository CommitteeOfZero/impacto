#include "util.h"

#include "profile/game.h"
#include <memory>

namespace Impacto {

CornersQuad& CornersQuad::Transform(const glm::mat4 transformation) {
  TopLeft = transformation * glm::vec4(TopLeft, 0.0f, 1.0f);
  TopRight = transformation * glm::vec4(TopRight, 0.0f, 1.0f);
  BottomRight = transformation * glm::vec4(BottomRight, 0.0f, 1.0f);
  BottomLeft = transformation * glm::vec4(BottomLeft, 0.0f, 1.0f);

  return *this;
}

CornersQuad& CornersQuad::Transform(
    const std::function<glm::vec2(glm::vec2)>& transformation) {
  TopLeft = transformation(TopLeft);
  TopRight = transformation(TopRight);
  BottomLeft = transformation(BottomLeft);
  BottomRight = transformation(BottomRight);

  return *this;
}

CornersQuad& CornersQuad::Translate(const glm::vec2 offset) {
  TopLeft += offset;
  TopRight += offset;
  BottomRight += offset;
  BottomLeft += offset;

  return *this;
}

CornersQuad& CornersQuad::Scale(const glm::vec2 scaling,
                                const glm::vec2 origin) {
  Translate(-origin);

  TopLeft *= scaling;
  TopRight *= scaling;
  BottomRight *= scaling;
  BottomLeft *= scaling;

  Translate(origin);

  return *this;
}

CornersQuad& CornersQuad::Rotate(const float angle, const glm::vec2 origin) {
  Translate(-origin);

  const glm::mat2 rotation = Rotate2D(angle);
  TopLeft = rotation * TopLeft;
  TopRight = rotation * TopRight;
  BottomRight = rotation * BottomRight;
  BottomLeft = rotation * BottomLeft;

  Translate(origin);

  return *this;
}

CornersQuad& CornersQuad::Rotate(const glm::quat rotation, glm::vec3 origin) {
  glm::vec3 topLeft = glm::vec3(TopLeft, 0.0f) - origin;
  glm::vec3 topRight = glm::vec3(TopRight, 0.0f) - origin;
  glm::vec3 bottomRight = glm::vec3(BottomRight, 0.0f) - origin;
  glm::vec3 bottomLeft = glm::vec3(BottomLeft, 0.0f) - origin;

  const glm::mat3 rotationMatrix = glm::mat3_cast(rotation);
  topLeft = rotationMatrix * topLeft;
  topRight = rotationMatrix * topRight;
  bottomRight = rotationMatrix * bottomRight;
  topRight = rotationMatrix * topRight;

  TopLeft = topLeft + origin;
  TopRight = topRight + origin;
  BottomRight = bottomRight + origin;
  BottomLeft = bottomLeft + origin;

  return *this;
}

CornersQuad& CornersQuad::Rotate(const glm::quat rotation,
                                 const glm::vec3 origin, const float depth,
                                 const glm::vec2 vanishingPoint,
                                 const bool stayInScreen) {
  std::array<glm::vec4, 4> vertices = {
      glm::vec4(TopLeft, 0.0f, 1.0f), glm::vec4(BottomLeft, 0.0f, 1.0f),
      glm::vec4(TopRight, 0.0f, 1.0f), glm::vec4(BottomRight, 0.0f, 1.0f)};

  // Rotate
  const glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
  for (glm::vec4& vertex : vertices) {
    vertex -= glm::vec4(origin, 0.0f);
    vertex = rotationMatrix * vertex;
    vertex += glm::vec4(origin, 0.0f);
  }

  // Move into screen
  if (stayInScreen) {
    float maxZ =
        std::max_element(vertices.begin(), vertices.end(),
                         [](auto lhs, auto rhs) { return lhs.z < rhs.z; })
            ->z;

    for (glm::vec4& vertex : vertices) vertex.z -= maxZ;
  }

  // Project
  for (glm::vec4& vertex : vertices) {
    vertex -= glm::vec4(vanishingPoint, 0.0f, 0.0f);
    const float normalizedZ = vertex.z / glm::length(glm::vec3(vertex));
    vertex *= depth / (depth - normalizedZ);
    vertex += glm::vec4(vanishingPoint, 0.0f, 0.0f);
  }

  *this = {glm::vec2(vertices[0]), glm::vec2(vertices[1]),
           glm::vec2(vertices[2]), glm::vec2(vertices[3])};
  return *this;
}

CornersQuad RectF::Transform(glm::mat4 transformation) const {
  return CornersQuad(*this).Transform(transformation);
}

CornersQuad RectF::Transform(
    const std::function<glm::vec2(glm::vec2)>& transformation) const {
  return CornersQuad(*this).Transform(transformation);
}

RectF& RectF::Scale(const glm::vec2 scaling, const glm::vec2 origin) {
  Translate(-origin);

  X *= scaling.x;
  Y *= scaling.y;
  Width *= scaling.x;
  Height *= scaling.y;

  Translate(origin);

  return *this;
}

CornersQuad RectF::Rotate(float angle, glm::vec2 origin) const {
  return CornersQuad(*this).Rotate(angle, origin);
}

CornersQuad RectF::RotateAroundCenter(float angle) const {
  return Rotate(angle, Center());
}

CornersQuad RectF::Rotate(glm::quat rotation, glm::vec3 origin) const {
  return CornersQuad(*this).Rotate(rotation, origin);
}

CornersQuad RectF::Rotate(const glm::quat rotation, const glm::vec3 origin,
                          const float depth, const glm::vec2 vanishingPoint,
                          const bool stayInScreen) const {
  return CornersQuad(*this).Rotate(rotation, origin, depth, vanishingPoint,
                                   stayInScreen);
}

CornersQuad RectF::FlipVertical() const {
  return CornersQuad(*this).FlipVertical();
}

CornersQuad RectF::FlipHorizontal() const {
  return CornersQuad(*this).FlipHorizontal();
}

inline CornersQuad operator*(const glm::mat4 transformation, RectF rect) {
  return rect.Transform(transformation);
}

glm::mat2 Rotate2D(float angle) {
  glm::mat2 result;
  float cosa = cosf(angle);
  float sina = sinf(angle);
  result[0][0] = cosa;
  result[0][1] = sina;
  result[1][0] = -sina;
  result[1][1] = cosa;
  return result;
}

glm::mat4 TransformationMatrix(const glm::vec2 scalingOrigin,
                               const glm::vec2 scaling,
                               const glm::vec2 rotationOrigin,
                               const float rotation,
                               const glm::vec2 translation) {
  return TransformationMatrix(
      scalingOrigin, scaling, {rotationOrigin, 0.0f},
      AxisAngleToQuaternion({0.0f, 0.0f, 1.0f}, rotation), translation);
}

glm::mat4 TransformationMatrix(const glm::vec3 scalingOrigin,
                               const glm::vec3 scaling,
                               const glm::vec3 rotationOrigin,
                               const glm::quat rotation,
                               const glm::vec3 translation) {
  glm::mat4 matrix(1.0f);

  if (rotationOrigin + translation != glm::vec3(0.0f)) {
    matrix = glm::translate(matrix, rotationOrigin + translation);
  }

  if (rotation != glm::quat()) {
    matrix = matrix * glm::mat4_cast(glm::normalize(rotation));
  }

  if (scalingOrigin - rotationOrigin != glm::vec3(0.0f)) {
    matrix = glm::translate(matrix, scalingOrigin - rotationOrigin);
  }

  if (scaling != glm::vec3(1.0f)) {
    matrix = glm::scale(matrix, scaling);
  }

  if (scalingOrigin != glm::vec3(0.0f)) {
    matrix = glm::translate(matrix, -scalingOrigin);
  }

  return matrix;
}

glm::vec4 TransformVector(const glm::vec2 pos, const glm::vec2 scalingOrigin,
                          const glm::vec2 scaling,
                          const glm::vec2 rotationOrigin, const float rotation,
                          const glm::vec2 translation) {
  return TransformationMatrix(scalingOrigin, scaling, rotationOrigin, rotation,
                              translation) *
         glm::vec4(pos, 0.0f, 1.0f);
}

glm::vec4 TransformVector(const glm::vec3 pos, const glm::vec3 scalingOrigin,
                          const glm::vec3 scaling,
                          const glm::vec3 rotationOrigin,
                          const glm::quat rotation,
                          const glm::vec3 translation) {
  return TransformationMatrix(scalingOrigin, scaling, rotationOrigin, rotation,
                              translation) *
         glm::vec4(pos, 1.0f);
}

uint32_t GetHashCode(uint8_t* data, int length) {
  uint32_t hash = 2166136261;
  for (int i = 0; i < length; i++) {
    hash = (hash ^ data[i]) * 16777619;
  }

  return hash;
}

std::string DumpMat4(glm::mat4* matrix, std::string_view columnSeparator,
                     std::string_view rowSeparator) {
  return fmt::sprintf(
      "%s%.03f%s%.03f%s%.03f%s%.03f%s%s%.03f%s%.03f%s%.03f%s%.03f%s%s"
      "%.03f%s%.03f%s%.03f%s%.03f%s%s%.03f%s%.03f%s%.03f%s%.03f",
      columnSeparator, (*matrix)[0][0], columnSeparator, (*matrix)[1][0],
      columnSeparator, (*matrix)[2][0], columnSeparator, (*matrix)[3][0],
      rowSeparator, columnSeparator, (*matrix)[0][1], columnSeparator,
      (*matrix)[1][1], columnSeparator, (*matrix)[2][1], columnSeparator,
      (*matrix)[3][1], rowSeparator, columnSeparator, (*matrix)[0][2],
      columnSeparator, (*matrix)[1][2], columnSeparator, (*matrix)[2][2],
      columnSeparator, (*matrix)[3][2], rowSeparator, columnSeparator,
      (*matrix)[0][3], columnSeparator, (*matrix)[1][3], columnSeparator,
      (*matrix)[2][3], columnSeparator, (*matrix)[3][3]);
}

int ResizeImage(Rect const& srcRect, Rect const& dstRect,
                std::span<uint8_t> src, std::span<uint8_t> dst) {
  using SurfacePtr = std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>;
  if (srcRect.Width == dstRect.Width && srcRect.Height == dstRect.Height) {
    assert(dst.size() >= src.size());
    memcpy(dst.data(), src.data(), src.size());
    return 0;
  }
  const uint32_t sdlFormat = SDL_PIXELFORMAT_RGBA32;
  const int bytesPerPixel = 4;

  assert(src.size() >= srcRect.Width * srcRect.Height * bytesPerPixel);
  SurfacePtr srcSurface(
      SDL_CreateRGBSurfaceWithFormatFrom(
          src.data(), srcRect.Width, srcRect.Height, bytesPerPixel * 8,
          srcRect.Width * bytesPerPixel, sdlFormat),
      SDL_FreeSurface);
  SDL_SetSurfaceBlendMode(srcSurface.get(), SDL_BLENDMODE_NONE);
  if (!srcSurface) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "SDL_CreateRGBSurfaceWithFormatFrom failed: {:s}\n", SDL_GetError());
    return -1;
  }
  assert(dst.size() >= dstRect.Width * dstRect.Height * 4);
  SurfacePtr dstSurface(
      SDL_CreateRGBSurfaceWithFormatFrom(
          dst.data(), dstRect.Width, dstRect.Height, bytesPerPixel * 8,
          dstRect.Width * bytesPerPixel, sdlFormat),
      SDL_FreeSurface);
  if (!dstSurface) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "SDL_CreateRGBSurfaceWithFormat failed: {:s}\n", SDL_GetError());
    return -1;
  }
  SDL_SetSurfaceBlendMode(dstSurface.get(), SDL_BLENDMODE_NONE);

  SDL_Rect srcRectSDL = {srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height};
  SDL_Rect dstRectSDL = {dstRect.X, dstRect.Y, dstRect.Width, dstRect.Height};
  if (SDL_BlitScaled(srcSurface.get(), &srcRectSDL, dstSurface.get(),
                     &dstRectSDL) != 0) {
    ImpLog(LogLevel::Error, LogChannel::Render, "SDL_BlitScaled failed: {:s}\n",
           SDL_GetError());
    return -1;
  }
  return 0;
}

RectF RectF::Coalesce(const RectF& first, const RectF& second) {
  RectF rect;

  rect.X = std::min(first.X, second.X);
  rect.Y = std::min(first.Y, second.Y);
  rect.Width =
      std::max(first.X + first.Width, second.X + second.Width) - rect.X;
  rect.Height =
      std::max(first.Y + first.Height, second.Y + second.Height) - rect.Y;

  return rect;
}

RectF RectF::BoundingBox(const RectF& first, const CornersQuad& second) {
  RectF rect;
  rect.X = std::min({first.X, second.TopLeft.x, second.BottomLeft.x,
                     second.TopRight.x, second.BottomRight.x});

  rect.Y = std::min({first.Y, second.TopLeft.y, second.BottomLeft.y,
                     second.TopRight.y, second.BottomRight.y});
  rect.Width =
      std::max({first.X + first.Width, second.TopLeft.x, second.BottomLeft.x,
                second.TopRight.x, second.BottomRight.x}) -
      rect.X;
  rect.Height =
      std::max({first.Y + first.Height, second.TopLeft.y, second.BottomLeft.y,
                second.TopRight.y, second.BottomRight.y}) -
      rect.Y;
  return rect;
}

RectF RectF::BoundingBox(const CornersQuad& first, const RectF& second) {
  return BoundingBox(second, first);
}

tm CurrentDateTime() {
  using namespace std::chrono;
  const auto zonedTime = zoned_time(current_zone(), system_clock::now());
  const auto now = zonedTime.get_local_time();
  const auto tp = time_point_cast<days>(now);
  const auto date = year_month_day(tp);
  const auto weekday = year_month_weekday(tp).weekday_indexed();
  const auto time = hh_mm_ss(now - floor<days>(now));
  const int yday = duration_cast<days>(now - floor<years>(now)).count();

  return tm{
      .tm_sec = (int)time.seconds().count(),
      .tm_min = (int)time.minutes().count(),
      .tm_hour = (int)time.hours().count(),
      .tm_mday = (int)(unsigned)date.day(),
      .tm_mon = (int)(unsigned)date.month() - 1,
      .tm_year = (int)date.year() - 1900,
      .tm_wday = (int)weekday.index(),
      .tm_yday = yday,
      .tm_isdst = -1,
  };
}

}  // namespace Impacto