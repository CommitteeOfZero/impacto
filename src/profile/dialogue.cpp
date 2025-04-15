#include "dialogue.h"
#include "profile_internal.h"
#include "../log.h"
#include "../ui/ui.h"

#include "games/mo6tw/dialoguebox.h"
#include "games/chlcc/dialoguebox.h"
#include "games/cc/dialoguebox.h"

namespace Impacto {
namespace Profile {
namespace Dialogue {

void Configure() {
  EnsurePushMemberOfType("Dialogue", LUA_TTABLE);

  NVLBounds = EnsureGetMember<RectF>("NVLBounds");
  ADVBounds = EnsureGetMember<RectF>("ADVBounds");
  REVBounds = EnsureGetMember<RectF>("REVBounds");
  TryGetMember<RectF>("TipsBounds", TipsBounds);

  ADVBoxSprite = EnsureGetMember<Sprite>("ADVBoxSprite");
  ADVBoxPos = EnsureGetMember<glm::vec2>("ADVBoxPos");

  TryGetMember<bool>("ADVBoxShowName", ADVBoxShowName);

  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");
  FadeInDuration = EnsureGetMember<float>("FadeInDuration");

  TryGetMember<bool>("HasAutoButton", HasAutoButton);
  if (HasAutoButton) {
    AutoButtonSprite = EnsureGetMember<Sprite>("AutoButtonSprite");
    AutoButtonActiveSprite = EnsureGetMember<Sprite>("AutoButtonActiveSprite");
    AutoButtonPosition = EnsureGetMember<glm::vec2>("AutoButtonPosition");
  }

  TryGetMember<bool>("HasSkipButton", HasSkipButton);
  if (HasSkipButton) {
    SkipButtonSprite = EnsureGetMember<Sprite>("SkipButtonSprite");
    SkipButtonActiveSprite = EnsureGetMember<Sprite>("SkipButtonActiveSprite");
    SkipButtonPosition = EnsureGetMember<glm::vec2>("SkipButtonPosition");
  }

  TryGetMember<bool>("HasBacklogButton", HasBacklogButton);
  if (HasBacklogButton) {
    BacklogButtonSprite = EnsureGetMember<Sprite>("BacklogButtonSprite");
    BacklogButtonActiveSprite =
        EnsureGetMember<Sprite>("BacklogButtonActiveSprite");
    BacklogButtonPosition = EnsureGetMember<glm::vec2>("BacklogButtonPosition");
  }

  TryGetMember<bool>("HasMenuButton", HasMenuButton);
  if (HasMenuButton) {
    MenuButtonSprite = EnsureGetMember<Sprite>("MenuButtonSprite");
    MenuButtonActiveSprite = EnsureGetMember<Sprite>("MenuButtonActiveSprite");
    MenuButtonPosition = EnsureGetMember<glm::vec2>("MenuButtonPosition");
  }

  DialogueBoxCurrentType = DialogueBoxType::_from_integral_unchecked(
      EnsureGetMember<int>("DialogueBoxCurrentType"));

  switch (DialogueBoxCurrentType) {
    case DialogueBoxType::MO6TW:
      Profile::MO6TW::DialogueBox::Configure();
      break;
    case DialogueBoxType::CHLCC:
      Profile::CHLCC::DialogueBox::Configure();
      break;
    case DialogueBoxType::CC:
      Profile::CC::DialogueBox::Configure();
      break;
    default:
      ImpLog(LogLevel::Warning, LogChannel::General,
             "Dialogue box is not implemented for the current profile yet!\n");
      break;
  }

  NVLBoxMaxOpacity = EnsureGetMember<float>("NVLBoxMaxOpacity");

  ADVNameAlignment = TextAlignment::_from_integral_unchecked(
      EnsureGetMember<int>("ADVNameAlignment"));

  ADVNameFontSize = EnsureGetMember<float>("ADVNameFontSize");
  ADVNamePos = EnsureGetMember<glm::vec2>("ADVNamePos");

  REVNameFontSize = EnsureGetMember<float>("REVNameFontSize");
  REVColor = EnsureGetMember<int>("REVColor");
  REVNameColor = EnsureGetMember<int>("REVNameColor");
  REVNameOffset = EnsureGetMember<float>("REVNameOffset");
  REVNameLocation = REVNameLocationType::_from_integral_unchecked(
      EnsureGetMember<int>("REVNameLocation"));
  REVOutlineMode = RendererOutlineMode(EnsureGetMember<int>("REVOutlineMode"));
  REVNameOutlineMode =
      RendererOutlineMode(EnsureGetMember<int>("REVNameOutlineMode"));

  TryGetMember<float>("TipsLineSpacing", TipsLineSpacing);
  TryGetMember<int>("TipsColorIndex", TipsColorIndex);

  int WaitIconCurrentTypeInt;
  if (!TryGetMember<int>("WaitIconCurrentType", WaitIconCurrentTypeInt))
    WaitIconCurrentTypeInt = +WaitIconDisplay::WaitIconType::None;
  WaitIconCurrentType = WaitIconDisplay::WaitIconType::_from_integral_unchecked(
      WaitIconCurrentTypeInt);
  if (WaitIconCurrentType != +WaitIconDisplay::WaitIconType::None) {
    switch (WaitIconCurrentType) {
      case WaitIconDisplay::WaitIconType::SpriteAnim:
        WaitIconSpriteAnim =
            EnsureGetMember<SpriteAnimationDef>("WaitIconSpriteAnim");
        break;
      case WaitIconDisplay::WaitIconType::SpriteAnimFixed:
        WaitIconSpriteAnim =
            EnsureGetMember<SpriteAnimationDef>("WaitIconSpriteAnim");
        WaitIconFixedSpriteId = EnsureGetMember<int>("WaitIconFixedSpriteId");
        break;
      case WaitIconDisplay::WaitIconType::Fixed:
        WaitIconSprite = EnsureGetMember<Sprite>("WaitIconSprite");
        break;
      default:
        WaitIconSprite = EnsureGetMember<Sprite>("WaitIconSprite");
        WaitIconAnimationDuration =
            EnsureGetMember<float>("WaitIconAnimationDuration");
    }
    WaitIconOffset = EnsureGetMember<glm::vec2>("WaitIconOffset");
  }

  int AutoIconCurrentTypeInt;
  if (!TryGetMember<int>("AutoIconCurrentType", AutoIconCurrentTypeInt))
    AutoIconCurrentTypeInt = 0;  // None
  AutoIconCurrentType = AutoIconDisplay::AutoIconType::_from_integral_unchecked(
      AutoIconCurrentTypeInt);
  switch (AutoIconCurrentType) {
    case AutoIconDisplay::AutoIconType::SpriteAnim:
      AutoIconSpriteAnim =
          EnsureGetMember<SpriteAnimationDef>("AutoIconSpriteAnim");
      AutoIconOffset = EnsureGetMember<glm::vec2>("AutoIconOffset");
      break;
    case AutoIconDisplay::AutoIconType::SpriteAnimFixed:
      AutoIconSpriteAnim =
          EnsureGetMember<SpriteAnimationDef>("AutoIconSpriteAnim");
      AutoIconFixedSpriteId = EnsureGetMember<int>("AutoIconFixedSpriteId");
      AutoIconOffset = EnsureGetMember<glm::vec2>("AutoIconOffset");
      break;
    case AutoIconDisplay::AutoIconType::CHLCC:
      AutoIconSprite = EnsureGetMember<Sprite>("AutoIconSprite");
      AutoIconRotationSpeed = EnsureGetMember<float>("AutoIconRotationSpeed");
      AutoSkipArrowsSprite = EnsureGetMember<Sprite>("AutoSkipArrowsSprite");
      AutoIconOffset = EnsureGetMember<glm::vec2>("AutoIconOffset");
      break;
  }

  int SkipIconCurrentTypeInt;
  if (!TryGetMember<int>("SkipIconCurrentType", SkipIconCurrentTypeInt))
    SkipIconCurrentTypeInt = 0;  // None
  SkipIconCurrentType = SkipIconDisplay::SkipIconType::_from_integral_unchecked(
      SkipIconCurrentTypeInt);
  switch (SkipIconCurrentType) {
    case SkipIconDisplay::SkipIconType::SpriteAnim:
      SkipIconSpriteAnim =
          EnsureGetMember<SpriteAnimationDef>("SkipIconSpriteAnim");
      SkipIconOffset = EnsureGetMember<glm::vec2>("SkipIconOffset");
      break;
    case SkipIconDisplay::SkipIconType::SpriteAnimFixed:
      SkipIconSpriteAnim =
          EnsureGetMember<SpriteAnimationDef>("SkipIconSpriteAnim");
      SkipIconFixedSpriteId = EnsureGetMember<int>("SkipIconFixedSpriteId");
      SkipIconOffset = EnsureGetMember<glm::vec2>("SkipIconOffset");
      break;
    case SkipIconDisplay::SkipIconType::CHLCC:
      SkipIconSprite = EnsureGetMember<Sprite>("SkipIconSprite");
      SkipIconRotationSpeed = EnsureGetMember<float>("SkipIconRotationSpeed");
      SkipIconOffset = EnsureGetMember<glm::vec2>("SkipIconOffset");
      break;
  }

  DialogueFont = EnsureGetMember<Font*>("DialogueFont");
  SetFontSizeRatio = EnsureGetMember<float>("SetFontSizeRatio");
  DefaultFontSize = EnsureGetMember<float>("DefaultFontSize");
  RubyFontSize = EnsureGetMember<float>("RubyFontSize");
  RubyYOffset = EnsureGetMember<float>("RubyYOffset");

  MaxPageSize = EnsureGetMember<int>("MaxPageSize");
  PageCount = EnsureGetMember<int>("PageCount");

  Impacto::DialoguePages = new DialoguePage[PageCount];
  Impacto::DialoguePageCount = PageCount;
  for (int i = 0; i < PageCount; i++) {
    Impacto::DialoguePages[i].Glyphs.reserve(MaxPageSize);
  }

  {
    EnsurePushMemberOfType("ColorTable", LUA_TTABLE);

    ColorCount = (int)lua_rawlen(LuaState, -1);
    ColorTable = new DialogueColorPair[ColorCount];
    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      int i = EnsureGetKey<int32_t>() - 1;
      AssertIs(LUA_TTABLE);

      auto pairSize = lua_rawlen(LuaState, -1);
      if (pairSize != 2) {
        ImpLog(LogLevel::Fatal, LogChannel::Profile, "Expected two colors\n");
        Window->Shutdown();
      }
      ColorTable[i].TextColor = EnsureGetArrayElementByIndex<uint32_t>(0);
      ColorTable[i].OutlineColor = EnsureGetArrayElementByIndex<uint32_t>(1);

      Pop();
    }

    Pop();
  }

  ColorTagIsUint8 = EnsureGetMember<bool>("ColorTagIsUint8");

  TryGetMember<bool>("HasSpeakerPortraits", HasSpeakerPortraits);

  HaveADVNameTag = TryPushMember("ADVNameTag");
  if (HaveADVNameTag) {
    AssertIs(LUA_TTABLE);

    ADVNameTag::Position = EnsureGetMember<glm::vec2>("Position");
    ADVNameTag::LeftSprite = EnsureGetMember<Sprite>("LeftSprite");
    ADVNameTag::LineSprite = EnsureGetMember<Sprite>("LineSprite");
    ADVNameTag::RightSprite = EnsureGetMember<Sprite>("RightSprite");
    ADVNameTag::BaseLineWidth = EnsureGetMember<float>("BaseLineWidth");

    Pop();
  }

  Pop();
}

}  // namespace Dialogue
}  // namespace Profile
}  // namespace Impacto