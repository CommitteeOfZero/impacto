#include "nv12frame.h"

#include "../renderer.h"

namespace Impacto {

NV12Frame::NV12Frame(const glm::vec<2, size_t> dimensions)
    : LumaTexture(Renderer->DeclareMutableTexture(TexFmt_U8, dimensions)),
      CbCrTexture(Renderer->DeclareMutableTexture(
          TexFmt_RG8, dimensions / glm::vec<2, size_t>(2))) {
  Dimensions = dimensions;
}

bool NV12Frame::IsValid() const {
  return LumaTexture != nullptr && CbCrTexture != nullptr &&
         LumaTexture->IsValid() && CbCrTexture->IsValid();
}

void NV12Frame::Submit(const std::span<const uint8_t> luma,
                       const size_t lumaRowStride,
                       const std::span<const uint8_t> cbCr,
                       const size_t cbCrRowStride) {
  LumaTexture->Update(luma, lumaRowStride);
  CbCrTexture->Update(cbCr, cbCrRowStride);
}

}  // namespace Impacto
