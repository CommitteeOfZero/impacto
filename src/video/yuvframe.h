#pragma once

#include "../impacto.h"
#include <vector>
#include "../io/inputstream.h"
#include "../texture/texture.h"

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