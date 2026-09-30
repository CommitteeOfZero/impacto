#pragma once

#include "../textureref.h"

#include "texture.h"

namespace Impacto::Bgfx {

class PlainTextureRef final : public Impacto::PlainTextureRefInterface {
 public:
  PlainTextureRef() = delete;
  PlainTextureRef(const PlainTextureRef&) = delete;
  PlainTextureRef(PlainTextureRef&& other) = default;
  ~PlainTextureRef() = default;

  PlainTextureRef(Texture& texture) : TextureObject(&texture) {}

  PlainTextureRef& operator=(const PlainTextureRef&) = delete;
  PlainTextureRef& operator=(PlainTextureRef&&) = default;

  [[nodiscard]] bool IsValid() const override {
    return TextureObject != nullptr && TextureObject->IsValid();
  }

  [[nodiscard]] glm::vec<2, size_t> GetDimensions() const override {
    assert(IsValid());
    return TextureObject->GetDimensions();
  }

  [[nodiscard]] uint64_t GetTextureId() const override {
    assert(IsValid());
    return TextureObject->GetTextureHandle().idx;
  }

  [[nodiscard]] Texture& GetTexture() {
    assert(TextureObject != nullptr);
    return *TextureObject;
  }

 protected:
  // Note: renderer "owns" the object so it is deleted both when there are no
  // longer any live references *and* when the renderer shuts down
  Texture* TextureObject = nullptr;
};  // namespace Impacto::Bgfx

class MutableTextureRef final : public Impacto::MutableTextureRefInterface {
 public:
  MutableTextureRef() = delete;
  MutableTextureRef(const MutableTextureRef&) = delete;
  MutableTextureRef(MutableTextureRef&& other) = default;
  ~MutableTextureRef() = default;

  MutableTextureRef(MutableTexture& texture)
      : PlainTexture(static_cast<Texture&>(texture)) {}

  MutableTextureRef& operator=(const MutableTextureRef&) = delete;
  MutableTextureRef& operator=(MutableTextureRef&&) = default;

  [[nodiscard]] bool IsValid() const override { return PlainTexture.IsValid(); }

  [[nodiscard]] glm::vec<2, size_t> GetDimensions() const override {
    return PlainTexture.GetDimensions();
  }

  [[nodiscard]] uint64_t GetTextureId() const override {
    return PlainTexture.GetTextureId();
  }

  void Update(std::span<const uint8_t> data, size_t rowStride) override {
    assert(IsValid());
    static_cast<MutableTexture&>(PlainTexture.GetTexture())
        .Update(data, static_cast<uint16_t>(rowStride));
  }

 protected:
  Bgfx::PlainTextureRef PlainTexture;
};

}  // namespace Impacto::Bgfx
