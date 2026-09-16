#include "yuvframe.h"

#include "../renderer.h"

namespace Impacto::Bgfx {

void YUVFrame::Init(const float width, const float height) {
  assert(LumaTexture == nullptr && CbTexture == nullptr &&
         CrTexture == nullptr);

  Width = width;
  Height = height;

  LumaTexture =
      new MutableTexture(bgfx::TextureFormat::R8, static_cast<size_t>(width),
                         static_cast<size_t>(height));
  CbTexture = new MutableTexture(bgfx::TextureFormat::R8,
                                 static_cast<size_t>(width) / 2,
                                 static_cast<size_t>(height) / 2);
  CrTexture = new MutableTexture(bgfx::TextureFormat::R8,
                                 static_cast<size_t>(width) / 2,
                                 static_cast<size_t>(height) / 2);

  Renderer* const renderer = std::bit_cast<Renderer*>(Impacto::Renderer.get());
  LumaId =
      renderer->DeclareTexture(std::unique_ptr<Texture>(LumaTexture))->first;
  CbId = renderer->DeclareTexture(std::unique_ptr<Texture>(CbTexture))->first;
  CrId = renderer->DeclareTexture(std::unique_ptr<Texture>(CrTexture))->first;
}

void YUVFrame::Submit(const void* luma, const void* cb, const void* cr) {
  assert(LumaTexture != nullptr && CbTexture != nullptr &&
         CrTexture != nullptr);

  size_t dataSize = static_cast<size_t>(Width) * static_cast<size_t>(Height);

  LumaTexture->Update(
      std::span<const uint8_t>(std::bit_cast<const uint8_t*>(luma), dataSize),
      static_cast<uint16_t>(Width));
  CbTexture->Update(
      std::span<const uint8_t>(std::bit_cast<const uint8_t*>(cb), dataSize / 4),
      static_cast<uint16_t>(Width) / 2);
  CrTexture->Update(
      std::span<const uint8_t>(std::bit_cast<const uint8_t*>(cr), dataSize / 4),
      static_cast<uint16_t>(Width) / 2);
}

void YUVFrame::Release() {
  assert(LumaTexture != nullptr && CbTexture != nullptr &&
         CrTexture != nullptr);

  Impacto::Renderer->FreeTexture(LumaId);
  Impacto::Renderer->FreeTexture(CbId);
  Impacto::Renderer->FreeTexture(CrId);

  LumaTexture = nullptr;
  CbTexture = nullptr;
  CrTexture = nullptr;

  LumaId = 0;
  CbId = 0;
  CrId = 0;
}

}  // namespace Impacto::Bgfx
