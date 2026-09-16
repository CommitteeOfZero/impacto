#pragma once

#include "../impacto.h"

namespace Impacto {

class YUVFrame {
 public:
  float Width;
  float Height;

  uint32_t LumaId = 0;
  uint32_t CbId = 0;
  uint32_t CrId = 0;

  virtual void Init(float width, float height) = 0;

  virtual void Submit(const void* luma, const void* cb, const void* cr) = 0;
  virtual void Release() = 0;
};

}  // namespace Impacto