#include "texture.h"

#include "../../log.h"

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

}  // namespace Impacto::Bgfx
