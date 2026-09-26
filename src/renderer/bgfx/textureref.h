#pragma once

#include "../textureref.h"

#include "texture.h"

namespace Impacto::Bgfx {

enum class TextureRefType : uint8_t { Plain, Mutable };

class TextureRefInterface {
 public:
  [[nodiscard]] virtual Texture& GetTexture() const = 0;
  operator Texture&() const { return GetTexture(); }

  [[nodiscard]] virtual size_t GetTextureMapId() const = 0;

  [[nodiscard]] virtual constexpr TextureRefType GetBgfxType() const = 0;

  [[nodiscard]] static const Bgfx::TextureRefInterface&
  ToBgfxTextureRefInterface(const Impacto::TextureRefInterface& texture);
};

class PlainTextureRef final : public Impacto::TextureRefInterface,
                              public Bgfx::TextureRefInterface {
 public:
  PlainTextureRef() = delete;
  PlainTextureRef(const PlainTextureRef&) = delete;
  PlainTextureRef(PlainTextureRef&& other) { *this = std::move(other); }
  ~PlainTextureRef() = default;

  PlainTextureRef(size_t textureMapId, Texture& texture)
      : TextureMapId(textureMapId), TextureObject(&texture) {
    Dimensions = texture.GetDimensions();
  }

  PlainTextureRef& operator=(const PlainTextureRef&) = delete;
  PlainTextureRef& operator=(PlainTextureRef&&);

  [[nodiscard]] constexpr Bgfx::TextureRefType GetBgfxType() const override {
    return Bgfx::TextureRefType::Plain;
  }

  [[nodiscard]] bool IsValid() const override {
    return TextureMapId != 0 && TextureObject != nullptr &&
           TextureObject->IsValid();
  }

  [[nodiscard]] uint64_t GetTextureId() const override {
    assert(IsValid());
    return TextureObject->GetTextureHandle().idx;
  }

  [[nodiscard]] size_t GetTextureMapId() const override { return TextureMapId; }

  [[nodiscard]] Texture& GetTexture() const override {
    assert(TextureObject != nullptr);
    return *TextureObject;
  }

 protected:
  // Note: renderer "owns" the object so it is deleted both when there are no
  // longer any live references *and* when the renderer shuts down
  size_t TextureMapId = 0;
  Texture* TextureObject = nullptr;
};  // namespace Impacto::Bgfx

class MutableTextureRef final : public Impacto::MutableTextureRefInterface,
                                public Bgfx::TextureRefInterface {
 public:
  MutableTextureRef() = delete;
  MutableTextureRef(const MutableTextureRef&) = delete;
  MutableTextureRef(MutableTextureRef&& other) { *this = std::move(other); }
  ~MutableTextureRef() = default;

  MutableTextureRef(size_t textureMapId, MutableTexture& texture)
      : TextureMapId(textureMapId), TextureObject(&texture) {
    Dimensions = texture.GetDimensions();
  }

  MutableTextureRef& operator=(const MutableTextureRef&) = delete;
  MutableTextureRef& operator=(MutableTextureRef&&);

  [[nodiscard]] constexpr Bgfx::TextureRefType GetBgfxType() const override {
    return Bgfx::TextureRefType::Mutable;
  }

  [[nodiscard]] bool IsValid() const override {
    return TextureMapId != 0 && TextureObject != nullptr &&
           TextureObject->IsValid();
  }

  [[nodiscard]] uint64_t GetTextureId() const override {
    assert(IsValid());
    return TextureObject->GetTextureHandle().idx;
  }

  [[nodiscard]] size_t GetTextureMapId() const override { return TextureMapId; }

  [[nodiscard]] MutableTexture& GetMutableTexture() const {
    assert(TextureObject != nullptr);
    return *TextureObject;
  }
  [[nodiscard]] Texture& GetTexture() const override {
    return GetMutableTexture();
  }

  void Update(std::span<const uint8_t> data, size_t rowStride) override {
    assert(IsValid());
    TextureObject->Update(data, static_cast<uint16_t>(rowStride));
  }

 protected:
  // Note: renderer "owns" the object so it is deleted both when there are no
  // longer any live references *and* when the renderer shuts down
  size_t TextureMapId = 0;
  MutableTexture* TextureObject = nullptr;
};

}  // namespace Impacto::Bgfx
