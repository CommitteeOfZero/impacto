#include "texture.h"

#include "../../log.h"

namespace Impacto::Bgfx {

Texture& Texture::operator=(Texture&& other) {
  if (this == &other) return *this;
  Reset(true);

  Handle = other.Handle;
  Width = other.Width;
  Height = other.Height;

  other.Reset(false);

  return *this;
}

Texture::Texture(const bgfx::TextureFormat::Enum format,
                 const std::span<const uint8_t> data, const size_t width,
                 const size_t height)
    : Width(width), Height(height) {
  constexpr uint64_t textureFlags = BGFX_SAMPLER_MIN_ANISOTROPIC |  //
                                    BGFX_SAMPLER_MAG_ANISOTROPIC |  //
                                    BGFX_SAMPLER_UVW_CLAMP;

  Handle = bgfx::createTexture2D(
      static_cast<uint16_t>(width), static_cast<uint16_t>(height), false, 1,
      format, textureFlags,
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
                               const size_t width, const size_t height) {
  Width = width;
  Height = height;

  constexpr uint64_t textureFlags = BGFX_SAMPLER_MIN_ANISOTROPIC |  //
                                    BGFX_SAMPLER_MAG_ANISOTROPIC |  //
                                    BGFX_SAMPLER_UVW_CLAMP;

  Handle = bgfx::createTexture2D(static_cast<uint16_t>(width),
                                 static_cast<uint16_t>(height), false, 1,
                                 format, textureFlags, nullptr);

  if (!bgfx::isValid(Handle)) {
    ImpLog(LogLevel::Error, LogChannel::Render, "Failed to create texture.");
    assert(false);
  }
}

void MutableTexture::Update(const std::span<const uint8_t> data,
                            const uint16_t stride) {
  assert(bgfx::isValid(Handle));

  bgfx::updateTexture2D(
      Handle, 0, 0, 0, 0, static_cast<uint16_t>(Width),
      static_cast<uint16_t>(Height),
      bgfx::copy(data.data(), static_cast<uint32_t>(data.size_bytes())),
      stride);
}

}  // namespace Impacto::Bgfx
