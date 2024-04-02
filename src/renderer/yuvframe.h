#pragma once

#include "../impacto.h"

namespace Impacto {

class YUVFrame {
 public:
  float Width;
  float Height;
  uint32_t LumaId;
  uint32_t CbId;
  uint32_t CrId;

  virtual void Init(float width, float height) = 0;

  virtual void Submit(void* luma, void* cb, void* cr) = 0;
  virtual void Release() = 0;
};

}  // namespace Impacto