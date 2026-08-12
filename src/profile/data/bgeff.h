#pragma once

namespace Impacto {
namespace Profile {
namespace BgEff {

enum class BgEffTypeEnum : uint8_t {
  Disabled,
  FullscreenMask,
  TopRightQuadrantMask,
};

inline BgEffTypeEnum BgFrameEffectType = BgEffTypeEnum::Disabled;
inline BgEffTypeEnum BgChaEffectType = BgEffTypeEnum::Disabled;

void Load();

}  // namespace BgEff
}  // namespace Profile
}  // namespace Impacto