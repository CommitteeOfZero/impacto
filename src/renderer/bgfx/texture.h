#pragma once

#include "../../texture/texture.h"

#include <bgfx/bgfx.h>

#include <span>
#include <cassert>

namespace Impacto::Bgfx {

class Texture {
 public:
  Texture() = delete;
  Texture(Texture&) = delete;
  Texture(Texture&& other) { *this = std::move(other); }
  ~Texture() { Reset(true); }

  Texture& operator=(Texture&) = delete;
  Texture& operator=(Texture&&);

  bool operator==(const Texture& other) const {
    return other.Handle.idx == Handle.idx;
  }

  Texture(TexFmt format, std::span<const uint8_t> data, size_t width,
          size_t height);

  operator bgfx::TextureHandle() { return GetTextureHandle(); }
  bgfx::TextureHandle GetTextureHandle() {
    assert(bgfx::isValid(Handle));
    return Handle;
  }

 private:
  bgfx::TextureHandle Handle = {bgfx::kInvalidHandle};

  void Reset(bool cleanUpResources);
};

}  // namespace Impacto::Bgfx
