#pragma once

#include "../textureref.h"

namespace Impacto {

class YUVFrame final : public TextureRef {
 public:
  YUVFrame() = default;
  YUVFrame(const YUVFrame&) = delete;
  YUVFrame(YUVFrame&&) = default;
  ~YUVFrame() = default;

  YUVFrame(glm::vec<2, size_t> dimensions);

  YUVFrame& operator=(const YUVFrame&) = delete;
  YUVFrame& operator=(YUVFrame&&) = default;

  [[nodiscard]] bool IsValid() const override;

  [[nodiscard]] uint64_t GetTextureId() const override {
    assert(IsValid());
    return LumaTexture->GetTextureId();
  }

  void Submit(std::span<const uint8_t> luma, std::span<const uint8_t> cb,
              std::span<const uint8_t> cr);

  [[nodiscard]] MutableTextureRef& GetLuma() const {
    assert(IsValid());
    return *LumaTexture;
  }
  [[nodiscard]] MutableTextureRef& GetCb() const {
    assert(IsValid());
    return *CbTexture;
  }
  [[nodiscard]] MutableTextureRef& GetCr() const {
    assert(IsValid());
    return *CrTexture;
  }

 private:
  std::unique_ptr<MutableTextureRef> LumaTexture;
  std::unique_ptr<MutableTextureRef> CbTexture;
  std::unique_ptr<MutableTextureRef> CrTexture;
};

}  // namespace Impacto
