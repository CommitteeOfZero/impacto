#pragma once

#include "../impacto.h"

namespace Impacto {

class NV12Frame {
 public:
  float Width;
  float Height;
  uint32_t LumaId;
  uint32_t CbCrId;

  virtual void Init(float width, float height) = 0;

  virtual void Submit(const void* luma, int lumaStride, const void* cbcr,
                      int cbcrStride) = 0;
  virtual void Release() = 0;
};

}  // namespace Impacto