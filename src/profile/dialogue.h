#pragma once

#include "../util.h"
#include "../spritesheet.h"
#include "../hud/waiticondisplay.h"
#include "../hud/autoicondisplay.h"
#include "../hud/skipicondisplay.h"
#include "../hud/dialoguebox.h"
#include "../spriteanimation.h"

#include "../text/text.h"

namespace Impacto {
namespace Profile {
namespace Dialogue {

inline RectF NVLBounds;
inline RectF ADVBounds;
inline RectF REVBounds;
inline RectF SecondaryREVBounds;
inline RectF TipsBounds;

inline Sprite ADVBoxSprite;
inline glm::vec2 ADVBoxPos;

inline float FadeOutDuration;
inline float FadeInDuration;

inline float TextFadeInDuration = 0.0f;
inline float TextFadeOutDuration = 0.0f;

inline DialogueBoxType DialogueBoxCurrentType = DialogueBoxType::None;

inline float NVLBoxMaxOpacity;

inline TextAlignment ADVNameAlignment = TextAlignment::Left;
inline float ADVNameFontSize;
// Unlike most positions, this position is relative to alignment
// e.g. if ADVNameAlignment == TextAlignment::Right, name will *end* at
// ADVNamePos.x
inline glm::vec2 ADVNamePos;

BETTER_ENUM(NametagType, int, None, Sprite, TwoPiece, ThreePiece, CHLCC, CC)

BETTER_ENUM(REVNameLocationType, int, None, TopLeft, LeftTop)

inline float REVNameFontSize;
inline int REVColor;
inline int REVNameColor;
inline float REVNameOffset;
inline REVNameLocationType REVNameLocation = REVNameLocationType::None;
inline RendererOutlineMode REVOutlineMode = RendererOutlineMode::Full;
inline RendererOutlineMode REVNameOutlineMode = RendererOutlineMode::Full;

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
inline float SetFontSizeRatio;
inline float DefaultFontSize;
inline float RubyFontSize;
inline float RubyYOffset;

inline int ColorCount;
inline DialogueColorPair* ColorTable;

inline int MaxPageSize;
inline int PageCount;

inline bool ColorTagIsUint8;

inline NametagType NametagCurrentType = NametagType::None;

inline glm::vec2 NametagPosition{};
inline Sprite NametagSprite;
inline Sprite NametagLeftSprite;
inline Sprite NametagMiddleSprite;
inline Sprite NametagRightSprite;
inline float NametagMiddleBaseWidth = 0.0f;

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