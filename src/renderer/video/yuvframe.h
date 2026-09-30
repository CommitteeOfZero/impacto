#pragma once

#include "../textureref.h"

namespace Impacto {

class YUVFrame final : public TextureRefInterface {
 public:
  YUVFrame() = default;
  YUVFrame(const YUVFrame&) = default;
  YUVFrame(YUVFrame&&) = default;
  ~YUVFrame() = default;

  YUVFrame(glm::vec<2, size_t> dimensions);

  YUVFrame& operator=(const YUVFrame&) = default;
  YUVFrame& operator=(YUVFrame&&) = default;

  [[nodiscard]] bool IsValid() const override;

  [[nodiscard]] glm::vec<2, size_t> GetDimensions() const override {
    return Dimensions;
  }

  [[nodiscard]] uint64_t GetTextureId() const override {
    assert(IsValid());
    return LumaTexture->GetTextureId();
  }

  void Submit(std::span<const uint8_t> luma, std::span<const uint8_t> cb,
              std::span<const uint8_t> cr);

  [[nodiscard]] MutableTextureRefInterface& GetLuma() const {
    assert(IsValid());
    return *LumaTexture;
  }
  [[nodiscard]] MutableTextureRefInterface& GetCb() const {
    assert(IsValid());
    return *CbTexture;
  }
  [[nodiscard]] MutableTextureRefInterface& GetCr() const {
    assert(IsValid());
    return *CrTexture;
  }

 private:
  glm::vec<2, size_t> Dimensions = {0, 0};

  MutableTextureRef LumaTexture;
  MutableTextureRef CbTexture;
  MutableTextureRef CrTexture;
};

}  // namespace Impacto
