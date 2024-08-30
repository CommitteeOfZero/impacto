#pragma once

#include "../util.h"
#include "../spritesheet.h"
#include "../text.h"
#include "../hud/waiticondisplay.h"
#include "../hud/autoicondisplay.h"
#include "../hud/skipicondisplay.h"
#include "../hud/dialoguebox.h"
#include "../spriteanimation.h"

namespace Impacto {
namespace Profile {
namespace Dialogue {

inline RectF NVLBounds;
inline RectF ADVBounds;
inline RectF REVBounds;
inline RectF TipsBounds;

inline Sprite ADVBoxSprite;
inline glm::vec2 ADVBoxPos;

inline float FadeOutDuration;
inline float FadeInDuration;

inline DialogueBoxType DialogueBoxCurrentType = DialogueBoxType::None;

inline float NVLBoxMaxOpacity;

inline TextAlignment ADVNameAlignment = TextAlignment::Left;
inline float ADVNameFontSize;
// Unlike most positions, this position is relative to alignment
// e.g. if ADVNameAlignment == TextAlignment::Right, name will *end* at
// ADVNamePos.x
inline glm::vec2 ADVNamePos;
inline bool ADVBoxShowName = true;

BETTER_ENUM(REVNameLocationType, int, None, TopLeft, LeftTop)

inline float REVNameFontSize;
inline int REVNameColor;
inline float REVNameOffset;
inline REVNameLocationType REVNameLocation = REVNameLocationType::None;
inline RendererOutlineMode REVOutlineMode = RendererOutlineMode::RO_Full;

inline float TipsLineSpacing;
inline int TipsColorIndex = 0;

inline Sprite WaitIconSprite;
inline SpriteAnimationDef WaitIconSpriteAnim;
inline glm::vec2 WaitIconOffset;
inline float WaitIconAnimationDuration;
inline int WaitIconFixedSpriteId;
inline WaitIconDisplay::WaitIconType WaitIconCurrentType =
    WaitIconDisplay::WaitIconType::None;

inline Sprite AutoIconSprite;
inline SpriteAnimationDef AutoIconSpriteAnim;
inline glm::vec2 AutoIconOffset;
inline int AutoIconFixedSpriteId;
inline float AutoIconRotationSpeed;
inline AutoIconDisplay::AutoIconType AutoIconCurrentType =
    AutoIconDisplay::AutoIconType::None;

inline Sprite SkipIconSprite;
inline SpriteAnimationDef SkipIconSpriteAnim;
inline glm::vec2 SkipIconOffset;
inline int SkipIconFixedSpriteId;
inline float SkipIconRotationSpeed;
inline SkipIconDisplay::SkipIconType SkipIconCurrentType =
    SkipIconDisplay::SkipIconType::None;

inline Sprite AutoSkipArrowsSprite;

inline Font* DialogueFont;
inline float DefaultFontSize;
inline float RubyFontSize;
inline float RubyYOffset;

inline int ColorCount;
inline DialogueColorPair* ColorTable;

inline int MaxPageSize;
inline int PageCount;

inline bool ColorTagIsUint8;

inline bool HaveADVNameTag;
namespace ADVNameTag {
inline glm::vec2 Position;
inline Sprite LeftSprite;
inline Sprite LineSprite;
inline Sprite RightSprite;
inline float BaseLineWidth;
}  // namespace ADVNameTag

inline bool HasSpeakerPortraits = false;
inline float SpeakerPortraitBaseOffsetX = 250.0f;
inline float SpeakerPortraitBaseOffsetY = 600.0f;

inline bool HasAutoButton = false;
inline Sprite AutoButtonSprite;
inline Sprite AutoButtonActiveSprite;
inline glm::vec2 AutoButtonPosition;

inline bool HasSkipButton = false;
inline Sprite SkipButtonSprite;
inline Sprite SkipButtonActiveSprite;
inline glm::vec2 SkipButtonPosition;

inline bool HasBacklogButton = false;
inline Sprite BacklogButtonSprite;
inline Sprite BacklogButtonActiveSprite;
inline glm::vec2 BacklogButtonPosition;

inline bool HasMenuButton = false;
inline Sprite MenuButtonSprite;
inline Sprite MenuButtonActiveSprite;
inline glm::vec2 MenuButtonPosition;

void Configure();

}  // namespace Dialogue
}  // namespace Profile
}  // namespace Impacto