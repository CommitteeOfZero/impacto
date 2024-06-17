#include "util.h"

#include "profile/game.h"

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

glm::vec2 DesignToNDC(glm::vec2 xy) {
  glm::vec2 result;
  result.x = (xy.x / (Profile::DesignWidth * 0.5f)) - 1.0f;
  result.y = 1.0f - (xy.y / (Profile::DesignHeight * 0.5f));
  return result;
}

glm::vec2 DesignToNDCNonFlipped(glm::vec2 xy) {
  glm::vec2 result;
  result.x = (xy.x / (Profile::DesignWidth * 0.5f)) - 1.0f;
  result.y = (xy.y / (Profile::DesignHeight * 0.5f)) - 1.0f;
  return result;
}

RectF DesignToNDC(RectF const& rect) {
  RectF result;
  glm::vec2 xy = DesignToNDC(glm::vec2(rect.X, rect.Y));
  result.X = xy.x;
  result.Y = xy.y;
  result.Width = rect.Width / (Profile::DesignWidth * 0.5f);
  result.Height = rect.Height / (Profile::DesignHeight * 0.5f);
  return result;
}

uint32_t GetHashCode(uint8_t* data, int length) {
  uint32_t hash = 2166136261;
  for (int i = 0; i < length; i++) {
    hash = (hash ^ data[i]) * 16777619;
  }

  return hash;
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