#include "texture.h"

#include "../../log.h"

namespace Impacto::Bgfx {

Texture& Texture::operator=(Texture&& other) {
  if (this == &other) return *this;
  Reset(true);

  Handle = other.Handle;

  other.Reset(false);

  return *this;
}

Texture::Texture(const TexFmt format, const std::span<const uint8_t> data,
                 const size_t width, const size_t height) {
  const bgfx::TextureFormat::Enum bgfxTextureFormat =
      [format]() -> bgfx::TextureFormat::Enum {
    switch (format) {
      case TexFmt_RGB:
        return bgfx::TextureFormat::RGB8;
      case TexFmt_RGBA:
        return bgfx::TextureFormat::RGBA8;
      case TexFmt_U8:
        return bgfx::TextureFormat::R8;
    }
    assert(false);
    return bgfx::TextureFormat::RGBA8;
  }();

  constexpr uint64_t textureFlags = BGFX_SAMPLER_MIN_ANISOTROPIC |  //
                                    BGFX_SAMPLER_MAG_ANISOTROPIC |  //
                                    BGFX_SAMPLER_UVW_CLAMP |        //
                                    BGFX_TEXTURE_SRGB;

  Handle = bgfx::createTexture2D(
      static_cast<uint16_t>(width), static_cast<uint16_t>(height), false, 1,
      bgfxTextureFormat, textureFlags,
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

}  // namespace Impacto::Bgfx
