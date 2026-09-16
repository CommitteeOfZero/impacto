#pragma once

#include "../../texture/texture.h"

#include <bgfx/bgfx.h>

#include <span>
#include <cassert>

namespace Impacto::Bgfx {

class Texture {
 public:
  Texture() = default;
  Texture(Texture&) = delete;
  Texture(Texture&& other) { *this = std::move(other); }
  virtual ~Texture() { Reset(true); }

  Texture& operator=(Texture&) = delete;
  Texture& operator=(Texture&&);

  bool operator==(const Texture& other) const {
    return other.Handle.idx == Handle.idx;
  }

  Texture(bgfx::TextureFormat::Enum format, std::span<const uint8_t> data,
          size_t width, size_t height);

  operator bgfx::TextureHandle() { return GetTextureHandle(); }
  bgfx::TextureHandle GetTextureHandle() {
    assert(bgfx::isValid(Handle));
    return Handle;
  }

 protected:
  bgfx::TextureHandle Handle = {bgfx::kInvalidHandle};

  size_t Width = 0;
  size_t Height = 0;

  void Reset(bool cleanUpResources);
};

class MutableTexture final : public Texture {
 public:
  MutableTexture() = default;
  MutableTexture(bgfx::TextureFormat::Enum format, size_t width, size_t height);

  void Update(std::span<const uint8_t> data, uint16_t stride);
};

}  // namespace Impacto::Bgfx
