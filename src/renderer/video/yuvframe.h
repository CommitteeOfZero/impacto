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

  [[nodiscard]] uint64_t GetTextureId() const override {
    assert(IsValid());
    return LumaTexture.GetTextureId();
  }

  void Submit(std::span<const uint8_t> luma, std::span<const uint8_t> cb,
              std::span<const uint8_t> cr);

  [[nodiscard]] MutableTextureRefInterface* GetLuma() const {
    assert(IsValid());
    return LumaTexture.Get();
  }
  [[nodiscard]] MutableTextureRefInterface* GetCb() const {
    assert(IsValid());
    return CbTexture.Get();
  }
  [[nodiscard]] MutableTextureRefInterface* GetCr() const {
    assert(IsValid());
    return CrTexture.Get();
  }

 private:
  MutableTextureRef LumaTexture;
  MutableTextureRef CbTexture;
  MutableTextureRef CrTexture;
};

}  // namespace Impacto
