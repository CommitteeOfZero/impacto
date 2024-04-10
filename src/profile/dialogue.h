#pragma once

#include "../util.h"
#include "../spritesheet.h"
#include "../text.h"
#include "../hud/waiticondisplay.h"
#include "../hud/dialoguebox.h"
#include "../spriteanimation.h"

namespace Impacto {
namespace Profile {
namespace Dialogue {

extern RectF NVLBounds;
extern RectF ADVBounds;
extern RectF REVBounds;
extern RectF TipsBounds;

extern Sprite ADVBoxSprite;
extern glm::vec2 ADVBoxPos;

extern float FadeOutDuration;
extern float FadeInDuration;

extern DialogueBoxType DialogueBoxCurrentType;

extern float NVLBoxMaxOpacity;

extern TextAlignment ADVNameAlignment;
extern float ADVNameFontSize;
// Unlike most positions, this position is relative to alignment
// e.g. if ADVNameAlignment == TextAlignment::Right, name will *end* at
// ADVNamePos.x
extern glm::vec2 ADVNamePos;
extern bool ADVBoxShowName;

extern float REVNameFontSize;
extern int REVNameColor;
extern float REVNameOffset;

extern float TipsLineSpacing;

extern Sprite WaitIconSprite;
extern SpriteAnimationDef WaitIconSpriteAnim;
extern glm::vec2 WaitIconOffset;
extern float WaitIconAnimationDuration;
extern WaitIconDisplay::WaitIconType WaitIconCurrentType;

extern Font* DialogueFont;
extern float DefaultFontSize;
extern float RubyFontSize;
extern float RubyYOffset;

extern int ColorCount;
extern DialogueColorPair* ColorTable;

extern int MaxPageSize;
extern int PageCount;

extern bool ColorTagIsUint8;

extern bool HaveADVNameTag;
namespace ADVNameTag {
extern glm::vec2 Position;
extern Sprite LeftSprite;
extern Sprite LineSprite;
extern Sprite RightSprite;
extern float BaseLineWidth;
}  // namespace ADVNameTag

extern bool HasSpeakerPortraits;
extern float SpeakerPortraitBaseOffsetX;
extern float SpeakerPortraitBaseOffsetY;

extern bool HasAutoButton;
extern Sprite AutoButtonSprite;
extern Sprite AutoButtonActiveSprite;
extern glm::vec2 AutoButtonPosition;

extern bool HasSkipButton;
extern Sprite SkipButtonSprite;
extern Sprite SkipButtonActiveSprite;
extern glm::vec2 SkipButtonPosition;

extern bool HasBacklogButton;
extern Sprite BacklogButtonSprite;
extern Sprite BacklogButtonActiveSprite;
extern glm::vec2 BacklogButtonPosition;

extern bool HasMenuButton;
extern Sprite MenuButtonSprite;
extern Sprite MenuButtonActiveSprite;
extern glm::vec2 MenuButtonPosition;

void Configure();

}  // namespace Dialogue
}  // namespace Profile
}  // namespace Impacto