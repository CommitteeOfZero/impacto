#pragma once

#include "../../yuvframe.h"
#include "../texture.h"

namespace Impacto::Bgfx {

class YUVFrame final : public Impacto::YUVFrame {
 public:
  void Init(float width, float height) override;

  void Submit(const void* luma, const void* cb, const void* cr) override;
  void Release() override;

 private:
  MutableTexture* LumaTexture = nullptr;
  MutableTexture* CbTexture = nullptr;
  MutableTexture* CrTexture = nullptr;
};

}  // namespace Impacto::Bgfx
