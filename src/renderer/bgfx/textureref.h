#pragma once

#include "../textureref.h"

#include "texture.h"

namespace Impacto::Bgfx {

class TextureRef : public virtual Impacto::TextureRefInterface {
 public:
  TextureRef() = delete;
  TextureRef(const TextureRef&) = delete;
  TextureRef(TextureRef&& other) { *this = std::move(other); };
  ~TextureRef() = default;

  TextureRef(Texture& texture) : TextureObject(&texture) {}

  TextureRef& operator=(const TextureRef&) = delete;
  TextureRef& operator=(TextureRef&& other) {
    TextureObject = std::exchange(other.TextureObject, nullptr);
    return *this;
  }

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

class MutableTextureRef final : public Impacto::MutableTextureRefInterface,
                                public Bgfx::TextureRef {
 public:
  MutableTextureRef() = delete;
  MutableTextureRef(const MutableTextureRef&) = delete;
  MutableTextureRef(MutableTextureRef&& other) = default;
  ~MutableTextureRef() = default;

  MutableTextureRef(MutableTexture& texture)
      : Bgfx::TextureRef(static_cast<Texture&>(texture)) {}

  MutableTextureRef& operator=(const MutableTextureRef&) = delete;
  MutableTextureRef& operator=(MutableTextureRef&&) = default;

  void Update(std::span<const uint8_t> data, size_t rowStride) override {
    assert(IsValid());
    static_cast<MutableTexture*>(TextureObject)
        ->Update(data, static_cast<uint16_t>(rowStride));
  }

  [[nodiscard]] bool IsValid() const override {
    return Bgfx::TextureRef::IsValid();
  }

  [[nodiscard]] glm::vec<2, size_t> GetDimensions() const override {
    return Bgfx::TextureRef::GetDimensions();
  }

  [[nodiscard]] uint64_t GetTextureId() const override {
    return Bgfx::TextureRef::GetTextureId();
  }
};

}  // namespace Impacto::Bgfx
