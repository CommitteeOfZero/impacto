#pragma once

#include "../../nv12frame.h"
#include "../texture.h"

namespace Impacto::Bgfx {

class NV12Frame final : public Impacto::NV12Frame {
 public:
  void Init(float width, float height) override;

  void Submit(const void* luma, int lumaStride, const void* cbcr,
              int cbcrStride) override;
  void Release() override;

 private:
  MutableTexture* LumaTexture = nullptr;
  MutableTexture* CbCrTexture = nullptr;
};

}  // namespace Impacto::Bgfx
