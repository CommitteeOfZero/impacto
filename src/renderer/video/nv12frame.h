#pragma once

#include "../textureref.h"

namespace Impacto {

class NV12Frame final : public TextureInterface {
 public:
  NV12Frame() = default;
  NV12Frame(const NV12Frame&) = default;
  NV12Frame(NV12Frame&&) = default;
  ~NV12Frame() = default;

  NV12Frame(glm::vec<2, size_t> dimensions);

  NV12Frame& operator=(const NV12Frame&) = default;
  NV12Frame& operator=(NV12Frame&&) = default;

  [[nodiscard]] bool IsValid() const override;

  [[nodiscard]] glm::vec<2, size_t> GetDimensions() const override {
    return Dimensions;
  }

  [[nodiscard]] uint64_t GetTextureId() const override {
    assert(IsValid());
    return LumaTexture->GetTextureId();
  }

  void Submit(std::span<const uint8_t> luma, size_t lumaRowStride,
              std::span<const uint8_t> cbCr, size_t cbCrRowStride);

  [[nodiscard]] MutableTextureInterface& GetLuma() const {
    assert(IsValid());
    return *LumaTexture;
  }
  [[nodiscard]] MutableTextureInterface& GetCbCr() const {
    assert(IsValid());
    return *CbCrTexture;
  }

 private:
  glm::vec<2, size_t> Dimensions = {0, 0};

  MutableTextureRef LumaTexture;
  MutableTextureRef CbCrTexture;
};

}  // namespace Impacto
