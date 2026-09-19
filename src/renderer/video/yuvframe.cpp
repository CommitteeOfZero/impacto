#include "yuvframe.h"

#include "../renderer.h"

namespace Impacto {

YUVFrame::YUVFrame(const glm::vec<2, size_t> dimensions)
    : LumaTexture(Renderer->DeclareMutableTexture(TexFmt_U8, dimensions)),
      CbTexture(Renderer->DeclareMutableTexture(
          TexFmt_U8, dimensions / glm::vec<2, size_t>(2))),
      CrTexture(Renderer->DeclareMutableTexture(
          TexFmt_U8, dimensions / glm::vec<2, size_t>(2))) {
  Dimensions = dimensions;
}

bool YUVFrame::IsValid() const {
  return LumaTexture != nullptr && CbTexture != nullptr &&
         CrTexture != nullptr && LumaTexture->IsValid() &&
         CbTexture->IsValid() && CrTexture->IsValid();
}

void YUVFrame::Submit(const std::span<const uint8_t> luma,
                      const std::span<const uint8_t> cb,
                      const std::span<const uint8_t> cr) {
  LumaTexture->Update(luma, Dimensions.x);
  CbTexture->Update(cb, Dimensions.x / 2);
  CrTexture->Update(cr, Dimensions.x / 2);
}

}  // namespace Impacto
