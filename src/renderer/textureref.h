#pragma once

#include <glm/glm.hpp>

#include <span>

namespace Impacto {

class TextureRef {
 public:
  TextureRef() = default;
  TextureRef(const TextureRef&) = delete;
  TextureRef(TextureRef&&) = default;
  virtual ~TextureRef() = default;

  TextureRef& operator=(const TextureRef&) = delete;
  TextureRef& operator=(TextureRef&&) = default;

  [[nodiscard]] virtual bool IsValid() const = 0;

  [[nodiscard]] glm::vec<2, size_t> GetDimensions() const {
    assert(IsValid());
    return Dimensions;
  }

  [[nodiscard]] virtual uint64_t GetTextureId() const = 0;

 protected:
  glm::vec<2, size_t> Dimensions = {0, 0};
};

class MutableTextureRef : public TextureRef {
 public:
  virtual void Update(std::span<const uint8_t> data, size_t rowStride) = 0;
};

}  // namespace Impacto
