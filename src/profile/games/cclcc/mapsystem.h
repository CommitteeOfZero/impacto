#pragma once
#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace MapSystem {

static constexpr int MapPartsPhotoSpritesNumMax = 32;
static constexpr int MapPartsArticleSpritesNumMax = 16;
static constexpr int MapPartsPinSpritesNumMax = 12;

extern Sprite MapBgSprite;
extern Sprite MapPartsPhotoSprites[MapPartsPhotoSpritesNumMax];
extern Sprite MapPartsArticleSprites[MapPartsArticleSpritesNumMax];
extern Sprite MapPartsPinSprites[MapPartsPinSpritesNumMax];
extern Sprite SelectedMapPoolTagSprite;

void Configure();

}  // namespace MapSystem
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto