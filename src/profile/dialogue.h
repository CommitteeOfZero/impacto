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
extern float ADVBoxFadeOutDuration;
extern float ADVBoxFadeInDuration;

extern TextAlignment ADVNameAlignment;
extern float ADVNameFontSize;
extern glm::vec2 ADVNamePos;

extern Sprite WaitIconSprite;
extern glm::vec2 WaitIconOffset;
extern float WaitIconAnimationDuration;

extern Font DialogueFont;
extern float DefaultFontSize;
extern float LineSpacing;
extern float RubyFontSize;

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