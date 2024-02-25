#pragma once
#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace MapSystem {

static constexpr int MapPartsPhotoSpritesNumMax = 32;
static constexpr int MapPartsArticleSpritesNumMax = 16;
static constexpr int MapPartsPinSpritesNumMax = 12;
static constexpr int MapPartsTagSpritesNumMax = 16;

extern Sprite MapBgSprite;
extern Sprite MapButtonGuideSprite;
extern Sprite MapPartsPhotoSprites[MapPartsPhotoSpritesNumMax];
extern Sprite MapPartsArticleSprites[MapPartsArticleSpritesNumMax];
extern Sprite MapPartsPinSprites[MapPartsPinSpritesNumMax];
extern Sprite MapPartsTagSprites[MapPartsTagSpritesNumMax];
extern Sprite SelectedMapPoolTagSprite;

extern float FadeAnimationDuration;

void Configure();

}  // namespace MapSystem
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto