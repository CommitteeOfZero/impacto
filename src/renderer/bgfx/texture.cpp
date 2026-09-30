#include "texture.h"

#include "../../log.h"

namespace Impacto::Bgfx {

Texture& Texture::operator=(Texture&& other) {
  if (this == &other) return *this;
  Reset(true);

  Handle = other.Handle;
  Dimensions = other.Dimensions;

  other.Reset(false);

  return *this;
}

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

  if (!bgfx::isValid(Handle)) {
    ImpLog(LogLevel::Error, LogChannel::Render, "Failed to create texture.");
    assert(false);
  }
}

void Texture::Reset(const bool cleanUpResources) {
  if (cleanUpResources) {
    if (bgfx::isValid(Handle)) bgfx::destroy(Handle);
  }

  Handle.idx = bgfx::kInvalidHandle;
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

  if (!bgfx::isValid(Handle)) {
    ImpLog(LogLevel::Error, LogChannel::Render, "Failed to create texture.");
    assert(false);
  }
}

void MutableTexture::Update(const std::span<const uint8_t> data,
                            const uint16_t rowStride) {
  assert(bgfx::isValid(Handle));

  bgfx::updateTexture2D(
      Handle, 0, 0, 0, 0, static_cast<uint16_t>(Dimensions.x),
      static_cast<uint16_t>(Dimensions.y),
      bgfx::copy(data.data(), static_cast<uint32_t>(data.size_bytes())),
      rowStride);
}

}  // namespace Impacto::Bgfx
