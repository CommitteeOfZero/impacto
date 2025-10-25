#pragma once
#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace MapSystem {

constexpr int MapPartsPhotoSpritesNumMax = 32;
constexpr int MapPartsArticleSpritesNumMax = 16;
constexpr int MapPartsPinSpritesNumMax = 12;
constexpr int MapPartsTagSpritesNumMax = 16;

inline Sprite MapBgSprite;
inline Sprite MapPartsPhotoSprites[MapPartsPhotoSpritesNumMax];
inline Sprite MapPartsArticleSprites[MapPartsArticleSpritesNumMax];
inline Sprite MapPartsPinSprites[MapPartsPinSpritesNumMax];
inline Sprite MapPartsTagSprites[MapPartsTagSpritesNumMax];
inline Sprite SelectedMapPoolTagSprite;
inline Sprite MapLine;
inline Sprite MapLineRed;

inline float FadeAnimationDuration;

void Configure();

}  // namespace MapSystem
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto