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

  NVLBounds = EnsureGetMemberRectF("NVLBounds");
  ADVBounds = EnsureGetMemberRectF("ADVBounds");
  REVBounds = EnsureGetMemberRectF("REVBounds");
  TryGetMemberRectF("TipsBounds", TipsBounds);

  ADVBoxSprite = EnsureGetMemberSprite("ADVBoxSprite");
  ADVBoxPos = EnsureGetMemberVec2("ADVBoxPos");

  TryGetMemberBool("ADVBoxShowName", ADVBoxShowName);

  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");
  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");

  TryGetMemberBool("HasAutoButton", HasAutoButton);
  if (HasAutoButton) {
    AutoButtonSprite = EnsureGetMemberSprite("AutoButtonSprite");
    AutoButtonActiveSprite = EnsureGetMemberSprite("AutoButtonActiveSprite");
    AutoButtonPosition = EnsureGetMemberVec2("AutoButtonPosition");
  }

  TryGetMemberBool("HasSkipButton", HasSkipButton);
  if (HasSkipButton) {
    SkipButtonSprite = EnsureGetMemberSprite("SkipButtonSprite");
    SkipButtonActiveSprite = EnsureGetMemberSprite("SkipButtonActiveSprite");
    SkipButtonPosition = EnsureGetMemberVec2("SkipButtonPosition");
  }

  TryGetMemberBool("HasBacklogButton", HasBacklogButton);
  if (HasBacklogButton) {
    BacklogButtonSprite = EnsureGetMemberSprite("BacklogButtonSprite");
    BacklogButtonActiveSprite =
        EnsureGetMemberSprite("BacklogButtonActiveSprite");
    BacklogButtonPosition = EnsureGetMemberVec2("BacklogButtonPosition");
  }

  TryGetMemberBool("HasMenuButton", HasMenuButton);
  if (HasMenuButton) {
    MenuButtonSprite = EnsureGetMemberSprite("MenuButtonSprite");
    MenuButtonActiveSprite = EnsureGetMemberSprite("MenuButtonActiveSprite");
    MenuButtonPosition = EnsureGetMemberVec2("MenuButtonPosition");
  }

  DialogueBoxCurrentType = DialogueBoxType::_from_integral_unchecked(
      EnsureGetMemberInt("DialogueBoxCurrentType"));

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
      ImpLog(LL_Warning, LC_General,
             "Dialogue box is not implemented for the current profile yet!\n");
      break;
  }

  NVLBoxMaxOpacity = EnsureGetMemberFloat("NVLBoxMaxOpacity");

  ADVNameAlignment = TextAlignment::_from_integral_unchecked(
      EnsureGetMemberInt("ADVNameAlignment"));

  ADVNameFontSize = EnsureGetMemberFloat("ADVNameFontSize");
  ADVNamePos = EnsureGetMemberVec2("ADVNamePos");

  REVNameFontSize = EnsureGetMemberFloat("REVNameFontSize");
  REVNameColor = EnsureGetMemberInt("REVNameColor");
  REVNameOffset = EnsureGetMemberFloat("REVNameOffset");
  REVNameLocation = REVNameLocationType::_from_integral_unchecked(
      EnsureGetMemberInt("REVNameLocation"));
  REVOutlineMode = RendererOutlineMode(EnsureGetMemberInt("REVOutlineMode"));

  TryGetMemberFloat("TipsLineSpacing", TipsLineSpacing);
  TryGetMemberInt("TipsColorIndex", TipsColorIndex);

  WaitIconCurrentType = WaitIconDisplay::WaitIconType::_from_integral_unchecked(
      EnsureGetMemberInt("WaitIconCurrentType"));
  if (WaitIconCurrentType == +WaitIconDisplay::WaitIconType::SpriteAnim) {
    WaitIconSpriteAnim = EnsureGetMemberAnimation("WaitIconSpriteAnim");
  } else if (WaitIconCurrentType ==
             +WaitIconDisplay::WaitIconType::SpriteAnimFixed) {
    WaitIconSpriteAnim = EnsureGetMemberAnimation("WaitIconSpriteAnim");
    WaitIconFixedSpriteId = EnsureGetMemberInt("WaitIconFixedSpriteId");
  } else if (WaitIconCurrentType == +WaitIconDisplay::WaitIconType::None) {
    WaitIconSprite = EnsureGetMemberSprite("WaitIconSprite");
  } else {
    WaitIconSprite = EnsureGetMemberSprite("WaitIconSprite");
    WaitIconAnimationDuration =
        EnsureGetMemberFloat("WaitIconAnimationDuration");
  }
  WaitIconOffset = EnsureGetMemberVec2("WaitIconOffset");

  int AutoIconCurrentTypeInt;
  if (!TryGetMemberInt("AutoIconCurrentType", AutoIconCurrentTypeInt))
    AutoIconCurrentTypeInt = 0;  // None
  AutoIconCurrentType = AutoIconDisplay::AutoIconType::_from_integral_unchecked(
      AutoIconCurrentTypeInt);
  switch (AutoIconCurrentType) {
    case AutoIconDisplay::AutoIconType::SpriteAnim:
      AutoIconSpriteAnim = EnsureGetMemberAnimation("AutoIconSpriteAnim");
      AutoIconOffset = EnsureGetMemberVec2("AutoIconOffset");
      break;
    case AutoIconDisplay::AutoIconType::SpriteAnimFixed:
      AutoIconSpriteAnim = EnsureGetMemberAnimation("AutoIconSpriteAnim");
      AutoIconFixedSpriteId = EnsureGetMemberInt("AutoIconFixedSpriteId");
      AutoIconOffset = EnsureGetMemberVec2("AutoIconOffset");
      break;
    case AutoIconDisplay::AutoIconType::CHLCC:
      AutoIconSprite = EnsureGetMemberSprite("AutoIconSprite");
      AutoIconRotationSpeed = EnsureGetMemberFloat("AutoIconRotationSpeed");
      AutoSkipArrowsSprite = EnsureGetMemberSprite("AutoSkipArrowsSprite");
      AutoIconOffset = EnsureGetMemberVec2("AutoIconOffset");
      break;
  }

  int SkipIconCurrentTypeInt;
  if (!TryGetMemberInt("SkipIconCurrentType", SkipIconCurrentTypeInt))
    SkipIconCurrentTypeInt = 0;  // None
  SkipIconCurrentType = SkipIconDisplay::SkipIconType::_from_integral_unchecked(
      SkipIconCurrentTypeInt);
  switch (SkipIconCurrentType) {
    case SkipIconDisplay::SkipIconType::SpriteAnim:
      SkipIconSpriteAnim = EnsureGetMemberAnimation("SkipIconSpriteAnim");
      SkipIconOffset = EnsureGetMemberVec2("SkipIconOffset");
      break;
    case SkipIconDisplay::SkipIconType::SpriteAnimFixed:
      SkipIconSpriteAnim = EnsureGetMemberAnimation("SkipIconSpriteAnim");
      SkipIconFixedSpriteId = EnsureGetMemberInt("SkipIconFixedSpriteId");
      SkipIconOffset = EnsureGetMemberVec2("SkipIconOffset");
      break;
    case SkipIconDisplay::SkipIconType::CHLCC:
      SkipIconSprite = EnsureGetMemberSprite("SkipIconSprite");
      SkipIconRotationSpeed = EnsureGetMemberFloat("SkipIconRotationSpeed");
      SkipIconOffset = EnsureGetMemberVec2("SkipIconOffset");
      break;
  }

  DialogueFont = EnsureGetMemberFont("DialogueFont");
  DefaultFontSize = EnsureGetMemberFloat("DefaultFontSize");
  RubyFontSize = EnsureGetMemberFloat("RubyFontSize");
  RubyYOffset = EnsureGetMemberFloat("RubyYOffset");

  MaxPageSize = EnsureGetMemberInt("MaxPageSize");
  PageCount = EnsureGetMemberInt("PageCount");

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
      int i = EnsureGetKeyInt() - 1;
      AssertIs(LUA_TTABLE);

      auto pairSize = lua_rawlen(LuaState, -1);
      if (pairSize != 2) {
        ImpLog(LL_Fatal, LC_Profile, "Expected two colors\n");
        Window->Shutdown();
      }
      ColorTable[i].TextColor = EnsureGetArrayElementByIndexUint(0);
      ColorTable[i].OutlineColor = EnsureGetArrayElementByIndexUint(1);

      Pop();
    }

    Pop();
  }

  ColorTagIsUint8 = EnsureGetMemberBool("ColorTagIsUint8");

  TryGetMemberBool("HasSpeakerPortraits", HasSpeakerPortraits);

  HaveADVNameTag = TryPushMember("ADVNameTag");
  if (HaveADVNameTag) {
    AssertIs(LUA_TTABLE);

    ADVNameTag::Position = EnsureGetMemberVec2("Position");
    ADVNameTag::LeftSprite = EnsureGetMemberSprite("LeftSprite");
    ADVNameTag::LineSprite = EnsureGetMemberSprite("LineSprite");
    ADVNameTag::RightSprite = EnsureGetMemberSprite("RightSprite");
    ADVNameTag::BaseLineWidth = EnsureGetMemberFloat("BaseLineWidth");

    Pop();
  }

  Pop();
}

}  // namespace Dialogue
}  // namespace Profile
}  // namespace Impacto