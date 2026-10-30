#include "texture.h"

#include "../../log.h"
#include "renderer.h"

namespace Impacto::Bgfx {

Texture::Texture(const bgfx::TextureFormat::Enum format,
                 const std::span<const uint8_t> data,
                 const glm::vec<2, size_t> dimensions)
    : Dimensions(dimensions) {
  constexpr uint64_t textureFlags = BGFX_SAMPLER_MIN_ANISOTROPIC |  //
                                    BGFX_SAMPLER_MAG_ANISOTROPIC |  //
                                    BGFX_SAMPLER_UVW_CLAMP;

  Handle = bgfx::createTexture2D(
      static_cast<uint16_t>(dimensions.x), static_cast<uint16_t>(dimensions.y),
      false, 1, format, textureFlags,
      bgfx::copy(data.data(), static_cast<uint32_t>(data.size_bytes())));

  if (!Handle.IsValid()) {
    ImpLog(LogLevel::Error, LogChannel::Render, "Failed to create texture.");
    assert(false);
  }
}

void Texture::AlterRefCount(const int difference) {
  if (difference == 0 || Impacto::Renderer == nullptr) return;
  Bgfx::Renderer& renderer = static_cast<Bgfx::Renderer&>(*Impacto::Renderer);

  const auto textureIt = renderer.Textures.find(GetTextureId());
  assert(textureIt != renderer.Textures.end() &&
         "Tried to alter the refcount of an already deleted texture.");
  size_t& refCount = textureIt->second.second;

  assert(difference >= 0 || static_cast<size_t>(-difference) <= refCount);
  refCount += difference;

  if (refCount == 0) {
    std::unique_ptr<Texture>& textureObject = textureIt->second.first;
    renderer.OrphanedTextures.emplace_back(std::move(textureObject));

    renderer.Textures.erase(textureIt);
  }
}

MutableTexture::MutableTexture(const bgfx::TextureFormat::Enum format,
                               const glm::vec<2, size_t> dimensions) {
  Dimensions = dimensions;

  constexpr uint64_t textureFlags = BGFX_SAMPLER_MIN_ANISOTROPIC |  //
                                    BGFX_SAMPLER_MAG_ANISOTROPIC |  //
                                    BGFX_SAMPLER_UVW_CLAMP;

  Handle = bgfx::createTexture2D(static_cast<uint16_t>(dimensions.x),
                                 static_cast<uint16_t>(dimensions.y), false, 1,
                                 format, textureFlags, nullptr);

  if (!Handle.IsValid()) {
    ImpLog(LogLevel::Error, LogChannel::Render, "Failed to create texture.");
    assert(false);
  }
}

void MutableTexture::Update(const std::span<const uint8_t> data,
                            const size_t rowStride) {
  assert(Handle.IsValid());

  bgfx::updateTexture2D(
      Handle, 0, 0, 0, 0, static_cast<uint16_t>(Dimensions.x),
      static_cast<uint16_t>(Dimensions.y),
      bgfx::copy(data.data(), static_cast<uint32_t>(data.size_bytes())),
      static_cast<uint16_t>(rowStride));
}

NonOwnedTexture::NonOwnedTexture(bgfx::TextureHandle texture,
                                 glm::vec<2, size_t> dimensions,
                                 bool isScreenCap) {
  Handle = texture;
  Dimensions = dimensions;
  ScreenCap = isScreenCap;
}

NonOwnedTexture::~NonOwnedTexture() {
  // Remove the reference to the texture so Handle's destructor won't destroy
  // the texture
  Handle.Handle = {bgfx::kInvalidHandle};
}

}  // namespace Impacto::Bgfx
