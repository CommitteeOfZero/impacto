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

extern Sprite MapBgSprite;
extern Sprite MapButtonGuideSprite;
extern Sprite MapPartsPhotoSprites[MapPartsPhotoSpritesNumMax];
extern Sprite MapPartsArticleSprites[MapPartsArticleSpritesNumMax];
extern Sprite MapPartsPinSprites[MapPartsPinSpritesNumMax];
extern Sprite MapPartsTagSprites[MapPartsTagSpritesNumMax];
extern Sprite SelectedMapPoolTagSprite;
extern Sprite MapLine;
extern Sprite MapLineRed;

extern float FadeAnimationDuration;

void Configure();

}  // namespace MapSystem
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto