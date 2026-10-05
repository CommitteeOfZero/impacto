#pragma once

#include "../../texture/texture.h"
#include "../textureref.h"

#include <bgfx/bgfx.h>
#include <magic_enum/magic_enum_containers.hpp>

#include <span>
#include <cassert>

namespace Impacto::Bgfx {

class Texture : public virtual TextureInterface {
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
          glm::vec<2, size_t> dimensions);

  operator bgfx::TextureHandle() const { return GetTextureHandle(); }
  bgfx::TextureHandle GetTextureHandle() const {
    assert(bgfx::isValid(Handle));
    return Handle;
  }

  [[nodiscard]] bool IsValid() const override { return bgfx::isValid(Handle); }

  [[nodiscard]] glm::vec<2, size_t> GetDimensions() const override {
    return Dimensions;
  }

  [[nodiscard]] uint64_t GetTextureId() const override { return Handle.idx; }

  bool IsScreenCap() const { return ScreenCap; }

 protected:
  bgfx::TextureHandle Handle = {bgfx::kInvalidHandle};

  glm::vec<2, size_t> Dimensions = {0, 0};

  bool ScreenCap = false;

  void Reset(bool cleanUpResources);
};

class MutableTexture final : public MutableTextureInterface,
                             public virtual Texture {
 public:
  MutableTexture() = default;
  MutableTexture(bgfx::TextureFormat::Enum format,
                 glm::vec<2, size_t> dimensions);

  void Update(std::span<const uint8_t> data, size_t rowStride) override;

  [[nodiscard]] bool IsValid() const override { return Texture::IsValid(); }
  [[nodiscard]] glm::vec<2, size_t> GetDimensions() const override {
    return Texture::GetDimensions();
  }
  [[nodiscard]] uint64_t GetTextureId() const override {
    return Texture::GetTextureId();
  }
};

inline constexpr auto TexFmtConversion = []() {
  magic_enum::containers::array<TexFmt, bgfx::TextureFormat::Enum> array;
  using enum bgfx::TextureFormat::Enum;
  static_assert(magic_enum::enum_count<TexFmt>() == 4);

  array[TexFmt_U8] = R8;
  array[TexFmt_RG8] = RG8;
  array[TexFmt_RGB] = RGB8;
  array[TexFmt_RGBA] = RGBA8;

  return array;
}();

}  // namespace Impacto::Bgfx
