#pragma once

#include <magic_enum/magic_enum_containers.hpp>

#include "../util.h"
#include "../spritesheet.h"
#include "../hud/waiticondisplay.h"
#include "../hud/autoicondisplay.h"
#include "../hud/skipicondisplay.h"
#include "../hud/dialoguebox.h"
#include "../spriteanimation.h"

#include "../text/textpage.h"

namespace Impacto {
namespace Profile {
namespace Dialogue {
enum class TextModeInfoFieldFlags : uint32_t;
}
}  // namespace Profile
}  // namespace Impacto

namespace magic_enum::customize {
template <>
struct enum_range<Impacto::Profile::Dialogue::TextModeInfoFieldFlags> {
  constexpr static bool is_flags = true;
};
}  // namespace magic_enum::customize

namespace Impacto {
namespace Profile {
namespace Dialogue {

constexpr uint32_t NO_NAME = 0xffffffff;

inline Sprite ADVBoxSprite;
inline glm::vec2 ADVBoxPos;

inline float FadeOutDuration;
inline float FadeInDuration;

inline float TextFadeInDuration = 0.0f;
inline float TextFadeOutDuration = 0.0f;

inline DialogueBoxType DialogueBoxCurrentType = DialogueBoxType::None;

inline float NVLBoxMaxOpacity;

enum class NametagType : int {
  None,
  Sprite,
  TwoPiece,
  ThreePiece,
  CHLCC,
  CC,
};
inline size_t REVMessageModeIdx;
inline int REVColor;
inline int REVNameColor;
inline RendererOutlineMode REVOutlineMode = RendererOutlineMode::Full;
inline RendererOutlineMode REVNameOutlineMode = RendererOutlineMode::Full;

inline size_t TipsMessageModeIdx;
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

inline std::vector<DialogueColorPair> ColorTable;

inline int MaxPageSize;
inline int PageCount;

inline bool ColorTagIsUint8;

inline bool OldNametagPositioning = false;
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

enum class TextModeInfoFieldFlags : uint32_t {
  None = 0,
  DisplayMode = 1 << 0,
  WindowId = 1 << 1,
  WindowPos = 1 << 2,
  NameDispMode = 1 << 3,
  MaxNameWidth = 1 << 4,
  NamePos = 1 << 5,
  NameGlyphSize = 1 << 6,
  MaxLineWidth = 1 << 7,
  CurrentPageId = 1 << 8,
  WaitIconPos = 1 << 9,
  TextGlyphSize = 1 << 10,
  RubyGlyphSize = 1 << 11,
  LineSpacing = 1 << 12,
  RubyLineSpacing = 1 << 13,
  AlwaysAddRubySpacing = 1 << 14,
  LinefeedSpacing = 1 << 15,
  NameAlignment = 1 << 16,
  UseNameLengthL = 1 << 17,
  NameLengthL = 1 << 18,
  All = 0xFFFFFFFF,
};
inline std::array<magic_enum::containers::bitset<TextModeInfoFieldFlags>,
                  TextModesInfo.size()>
    ProfileTextModesInfoFields;

void Configure();

}  // namespace Dialogue
}  // namespace Profile
}  // namespace Impacto
