#include "nv12frame.h"

#include "../renderer.h"

namespace Impacto::Bgfx {

void NV12Frame::Init(const float width, const float height) {
  assert(LumaTexture == nullptr && CbCrTexture == nullptr);

  Width = width;
  Height = height;

  LumaTexture =
      new MutableTexture(bgfx::TextureFormat::R8, static_cast<size_t>(width),
                         static_cast<size_t>(height));
  CbCrTexture = new MutableTexture(bgfx::TextureFormat::RG8,
                                   static_cast<size_t>(width) / 2,
                                   static_cast<size_t>(height) / 2);

  Renderer* const renderer = std::bit_cast<Renderer*>(Impacto::Renderer.get());
  LumaId =
      renderer->DeclareTexture(std::unique_ptr<Texture>(LumaTexture))->first;
  CbCrId =
      renderer->DeclareTexture(std::unique_ptr<Texture>(CbCrTexture))->first;
}

void NV12Frame::Submit(const void* const luma, const int lumaStride,
                       const void* const cbcr, const int cbcrStride) {
  assert(LumaTexture != nullptr && CbCrTexture != nullptr);

  LumaTexture->Update(
      std::span<const uint8_t>(
          std::bit_cast<const uint8_t*>(luma),
          static_cast<size_t>(lumaStride) * static_cast<size_t>(Height)),
      static_cast<uint16_t>(lumaStride));
  CbCrTexture->Update(
      std::span<const uint8_t>(
          std::bit_cast<const uint8_t*>(cbcr),
          static_cast<size_t>(cbcrStride) * (static_cast<size_t>(Height) / 2)),
      static_cast<uint16_t>(cbcrStride));
}

void NV12Frame::Release() {
  assert(LumaTexture != nullptr && CbCrTexture != nullptr);

  Impacto::Renderer->FreeTexture(LumaId);
  Impacto::Renderer->FreeTexture(CbCrId);

  LumaTexture = nullptr;
  CbCrTexture = nullptr;

  LumaId = 0;
  CbCrId = 0;
}

}  // namespace Impacto::Bgfx
