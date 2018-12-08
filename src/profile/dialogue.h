#pragma once

#include "../util.h"
#include "../spritesheet.h"
#include "../text.h"

namespace Impacto {
namespace Profile {
namespace Dialogue {

extern RectF NVLBounds;
extern RectF ADVBounds;

extern Sprite ADVBoxSprite;
extern glm::vec2 ADVBoxPos;

extern float FadeOutDuration;
extern float FadeInDuration;

extern float NVLBoxMaxOpacity;

extern TextAlignment ADVNameAlignment;
extern float ADVNameFontSize;
// Unlike most positions, this position is relative to alignment
// e.g. if ADVNameAlignment == TextAlignment::Right, name will *end* at
// ADVNamePos.x
extern glm::vec2 ADVNamePos;

extern Sprite WaitIconSprite;
extern glm::vec2 WaitIconOffset;
extern float WaitIconAnimationDuration;

extern Font* DialogueFont;
extern float DefaultFontSize;
extern float RubyFontSize;
extern float RubyYOffset;

extern int ColorCount;
extern DialogueColorPair* ColorTable;

extern int MaxPageSize;
extern int PageCount;

extern bool HaveADVNameTag;
namespace ADVNameTag {
extern glm::vec2 Position;
extern Sprite LeftSprite;
extern Sprite LineSprite;
extern Sprite RightSprite;
extern float BaseLineWidth;
}  // namespace ADVNameTag

void Configure();

}  // namespace Dialogue
}  // namespace Profile
}  // namespace Impacto