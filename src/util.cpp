#include "util.h"
#include "window.h"

namespace Impacto {

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

RectF::RectF() {}
RectF::RectF(float x, float y, float width, float height)
    : X(x), Y(y), Width(width), Height(height) {}
RectF::RectF(Rect const& rect)
    : RectF(rect.X, rect.Y, rect.Width, rect.Height) {}

glm::vec2 RectF::Center() const {
  return glm::vec2(X + Width / 2.0f, Y + Height / 2.0f);
}
bool RectF::ContainsPoint(glm::vec2 point, float angle) const {
  point -= Center();
  if (angle != 0.0f) {
    point = Rotate2D(-angle) * point;
  }

  return point.x >= -Width / 2.0f && point.x <= Width / 2.0f &&
         point.y >= -Height / 2.0f && point.y <= Height / 2.0f;
}

Rect::Rect() {}
Rect::Rect(int x, int y, int width, int height)
    : X(x), Y(y), Width(width), Height(height) {}
Rect::Rect(RectF const& rect)
    : Rect((int)rect.X, (int)rect.Y, (int)rect.Width, (int)rect.Height) {}

glm::ivec2 Rect::Center() const {
  return glm::ivec2(X + Width / 2, Y + Height / 2);
}
bool Rect::ContainsPoint(glm::ivec2 point, float angle) const {
  point -= Center();
  if (angle != 0.0f) {
    point = (glm::ivec2)(Rotate2D(-angle) * (glm::vec2)point);
  }
  return point.x >= -Width / 2 && point.x <= Width / 2 &&
         point.y >= -Height / 2 && point.y <= Height / 2;
}

glm::vec2 DesignToNDC(glm::vec2 xy) {
  glm::vec2 result;
  result.x = (xy.x / (g_DesignWidth * 0.5f)) - 1.0f;
  result.y = 1.0f - (xy.y / (g_DesignHeight * 0.5f));
  return result;
}

RectF DesignToNDC(RectF const& rect) {
  RectF result;
  glm::vec2 xy = DesignToNDC(glm::vec2(rect.X, rect.Y));
  result.X = xy.x;
  result.Y = xy.y;
  result.Width = rect.Width / (g_DesignWidth * 0.5f);
  result.Height = rect.Height / (g_DesignHeight * 0.5f);
  return result;
}

glm::vec4 RgbaIntToFloat(uint32_t rgba) {
  glm::vec4 result;
  result.a = (float)((rgba >> 24) & 0xFF) / 255.0f;
  result.r = (float)((rgba >> 16) & 0xFF) / 255.0f;
  result.g = (float)((rgba >> 8) & 0xFF) / 255.0f;
  result.b = (float)((rgba >> 0) & 0xFF) / 255.0f;
  return result;
}

char* DumpMat4(glm::mat4* matrix, const char* columnSeparator,
               const char* rowSeparator) {
  size_t reqSz =
      snprintf(
          NULL, 0,
          "%s%.03f%s%.03f%s%.03f%s%.03f%s%s%.03f%s%.03f%s%.03f%s%.03f%s%s%"
          ".03f%s%.03f%s%.03f%s%.03f%s%s%.03f%s%.03f%s%.03f%s%.03f",
          columnSeparator, (*matrix)[0][0], columnSeparator, (*matrix)[1][0],
          columnSeparator, (*matrix)[2][0], columnSeparator, (*matrix)[3][0],
          rowSeparator, columnSeparator, (*matrix)[0][1], columnSeparator,
          (*matrix)[1][1], columnSeparator, (*matrix)[2][1], columnSeparator,
          (*matrix)[3][1], rowSeparator, columnSeparator, (*matrix)[0][2],
          columnSeparator, (*matrix)[1][2], columnSeparator, (*matrix)[2][2],
          columnSeparator, (*matrix)[3][2], rowSeparator, columnSeparator,
          (*matrix)[0][3], columnSeparator, (*matrix)[1][3], columnSeparator,
          (*matrix)[2][3], columnSeparator, (*matrix)[3][3]) +
      1;
  char* result = (char*)malloc(reqSz);
  sprintf(result,
          "%s%.03f%s%.03f%s%.03f%s%.03f%s%s%.03f%s%.03f%s%.03f%s%.03f%s%s%"
          ".03f%s%.03f%s%.03f%s%.03f%s%s%.03f%s%.03f%s%.03f%s%.03f",
          columnSeparator, (*matrix)[0][0], columnSeparator, (*matrix)[1][0],
          columnSeparator, (*matrix)[2][0], columnSeparator, (*matrix)[3][0],
          rowSeparator, columnSeparator, (*matrix)[0][1], columnSeparator,
          (*matrix)[1][1], columnSeparator, (*matrix)[2][1], columnSeparator,
          (*matrix)[3][1], rowSeparator, columnSeparator, (*matrix)[0][2],
          columnSeparator, (*matrix)[1][2], columnSeparator, (*matrix)[2][2],
          columnSeparator, (*matrix)[3][2], rowSeparator, columnSeparator,
          (*matrix)[0][3], columnSeparator, (*matrix)[1][3], columnSeparator,
          (*matrix)[2][3], columnSeparator, (*matrix)[3][3]);
  return result;
}
}  // namespace Impacto