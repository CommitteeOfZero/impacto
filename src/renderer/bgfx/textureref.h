#pragma once

#include "../textureref.h"

#include "texture.h"

namespace Impacto::Bgfx {

class TextureRef : public Impacto::TextureRef {
 public:
  [[nodiscard]] virtual Texture& GetTexture() = 0;
  operator Texture&() { return GetTexture(); }
};

class PlainTextureRef final : public Bgfx::TextureRef {
 public:
  PlainTextureRef() = delete;
  PlainTextureRef(const PlainTextureRef&) = delete;
  PlainTextureRef(PlainTextureRef&& other) { *this = std::move(other); }
  ~PlainTextureRef();

  PlainTextureRef(Texture& texture) : TextureObject(&texture) {
    Dimensions = texture.GetDimensions();
  }

  PlainTextureRef& operator=(const PlainTextureRef&) = delete;
  PlainTextureRef& operator=(PlainTextureRef&&);

  [[nodiscard]] bool IsValid() const override {
    return TextureObject != nullptr && TextureObject->IsValid();
  }

  [[nodiscard]] uint64_t GetTextureId() const override {
    assert(IsValid());
    return TextureObject->GetTextureHandle().idx;
  }

  [[nodiscard]] Texture& GetTexture() override {
    assert(TextureObject != nullptr);
    return *TextureObject;
  }

 protected:
  // Note: renderer "owns" the object so it is deleted both when there are no
  // longer any live references *and* when the renderer shuts down
  Texture* TextureObject = nullptr;
};

class MutableTextureRef final : public Bgfx::TextureRef,
                                public Impacto::MutableTextureRef {
 public:
  MutableTextureRef() = delete;
  MutableTextureRef(const MutableTextureRef&) = delete;
  MutableTextureRef(MutableTextureRef&& other) { *this = std::move(other); }
  ~MutableTextureRef();

  MutableTextureRef(MutableTexture& texture) : TextureObject(&texture) {
    Impacto::TextureRef::Dimensions = texture.GetDimensions();
  }

  MutableTextureRef& operator=(const MutableTextureRef&) = delete;
  MutableTextureRef& operator=(MutableTextureRef&&);

  [[nodiscard]] bool IsValid() const override {
    return TextureObject != nullptr && TextureObject->IsValid();
  }

  [[nodiscard]] uint64_t GetTextureId() const override {
    assert(IsValid());
    return TextureObject->GetTextureHandle().idx;
  }

  [[nodiscard]] MutableTexture& GetMutableTexture() {
    assert(TextureObject != nullptr);
    return *TextureObject;
  }
  [[nodiscard]] Texture& GetTexture() override { return GetMutableTexture(); }

  void Update(std::span<const uint8_t> data, size_t rowStride) override {
    assert(IsValid());
    TextureObject->Update(data, static_cast<uint16_t>(rowStride));
  }

 protected:
  // Note: renderer "owns" the object so it is deleted both when there are no
  // longer any live references *and* when the renderer shuts down
  MutableTexture* TextureObject = nullptr;
};

}  // namespace Impacto::Bgfx
