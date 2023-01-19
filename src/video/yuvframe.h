#pragma once

#include "../impacto.h"

#include <glm/glm.hpp>

namespace Impacto {

struct YUVFrame {
  int Width;
  int Height;
  GLuint LumaId;
  GLuint CbId;
  GLuint CrId;

  void Init(int width, int height);

  void Submit(void* luma, void* cb, void* cr);
  void Release();
};

}  // namespace Impacto