#include "dialogue.h"
#include "profile_internal.h"
#include "../log.h"
#include "../ui/ui.h"

#include "games/mo6tw/dialoguebox.h"
#include "games/chlcc/dialoguebox.h"
#include "games/cc/dialoguebox.h"

#include "../hud/nametagdisplay.h"

namespace Impacto {
namespace Profile {
namespace Dialogue {

static void ConfigureNametag() {
  NametagCurrentType = EnsureGetMember<NametagType>("NametagCurrentType");

  switch (NametagCurrentType) {
    case NametagType::None:
      break;

    case NametagType::Sprite:
      NametagPosition = EnsureGetMember<glm::vec2>("NametagPosition");
      NametagSprite = EnsureGetMember<Sprite>("NametagSprite");
      break;

    case NametagType::TwoPiece:
    case NametagType::ThreePiece:
      NametagPosition = EnsureGetMember<glm::vec2>("NametagPosition");

      NametagLeftSprite = EnsureGetMember<Sprite>("NametagLeftSprite");
      NametagRightSprite = EnsureGetMember<Sprite>("NametagRightSprite");

      if (NametagCurrentType == NametagType::ThreePiece) {
        NametagMiddleSprite = EnsureGetMember<Sprite>("NametagMiddleSprite");
        NametagMiddleBaseWidth =
            EnsureGetMember<float>("NametagMiddleBaseWidth");
      }
      break;

    case NametagType::CHLCC:
      CHLCC::DialogueBox::ConfigureNametag();
      break;

    case NametagType::CC:
      CC::DialogueBox::ConfigureNametag();
      break;

    default:
      throw std::runtime_error(
          fmt::format("Tried to configure unknown nametag type {:d}",
                      static_cast<int>(NametagCurrentType)));
  }
}

void Configure() {
  EnsurePushMemberOfType("Dialogue", LUA_TTABLE);

  NVLBounds = EnsureGetMember<RectF>("NVLBounds");
  ADVBounds = EnsureGetMember<RectF>("ADVBounds");
  REVBounds = EnsureGetMember<RectF>("REVBounds");
  TryGetMember<RectF>("SecondaryREVBounds", SecondaryREVBounds);
  TryGetMember<RectF>("TipsBounds", TipsBounds);

  ADVBoxSprite = EnsureGetMember<Sprite>("ADVBoxSprite");
  ADVBoxPos = EnsureGetMember<glm::vec2>("ADVBoxPos");

  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");
  FadeInDuration = EnsureGetMember<float>("FadeInDuration");

  TextFadeInDuration = EnsureGetMember<float>("TextFadeInDuration");
  TextFadeOutDuration = EnsureGetMember<float>("TextFadeOutDuration");

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

  DialogueBoxCurrentType =
      EnsureGetMember<DialogueBoxType>("DialogueBoxCurrentType");

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

  ADVNameAlignment = EnsureGetMember<TextAlignment>("ADVNameAlignment");

  ADVNameFontSize = EnsureGetMember<float>("ADVNameFontSize");
  ADVNamePos = EnsureGetMember<glm::vec2>("ADVNamePos");

  REVNameFontSize = EnsureGetMember<float>("REVNameFontSize");
  REVColor = EnsureGetMember<int>("REVColor");
  REVNameColor = EnsureGetMember<int>("REVNameColor");
  REVNameOffset = EnsureGetMember<float>("REVNameOffset");
  REVNameLocation = EnsureGetMember<REVNameLocationType>("REVNameLocation");
  REVOutlineMode = RendererOutlineMode(EnsureGetMember<int>("REVOutlineMode"));
  REVNameOutlineMode =
      RendererOutlineMode(EnsureGetMember<int>("REVNameOutlineMode"));

  TryGetMember<float>("TipsLineSpacing", TipsLineSpacing);
  TryGetMember<int>("TipsColorIndex", TipsColorIndex);

  WaitIconCurrentType =
      TryGetMember<WaitIconDisplay::WaitIconType>("WaitIconCurrentType")
          .value_or(WaitIconDisplay::WaitIconType::None);
  if (WaitIconCurrentType != WaitIconDisplay::WaitIconType::None) {
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

  AutoIconCurrentType =
      TryGetMember<AutoIconDisplay::AutoIconType>("AutoIconCurrentType")
          .value_or(AutoIconDisplay::AutoIconType::None);
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

    case AutoIconDisplay::AutoIconType::Fixed:
    case AutoIconDisplay::AutoIconType::None:
      break;
  }

  SkipIconCurrentType =
      TryGetMember<SkipIconDisplay::SkipIconType>("SkipIconCurrentType")
          .value_or(SkipIconDisplay::SkipIconType::None);
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

    case SkipIconDisplay::SkipIconType::Fixed:
    case SkipIconDisplay::SkipIconType::None:
      break;
  }

  DialogueFont = EnsureGetMember<Font*>("DialogueFont");
  SetFontSizeRatio = EnsureGetMember<float>("SetFontSizeRatio");
  DefaultFontSize = EnsureGetMember<float>("DefaultFontSize");
  RubyFontSize = EnsureGetMember<float>("RubyFontSize");
  RubyYOffset = EnsureGetMember<float>("RubyYOffset");

  MaxPageSize = EnsureGetMember<int>("MaxPageSize");
  PageCount = EnsureGetMember<int>("PageCount");

  ColorTable = EnsureGetMember<std::vector<DialogueColorPair>>("ColorTable");
  ColorTagIsUint8 = EnsureGetMember<bool>("ColorTagIsUint8");

  ConfigureNametag();

  TryGetMember<bool>("HasSpeakerPortraits", HasSpeakerPortraits);

  Pop();
}

}  // namespace Dialogue
}  // namespace Profile
}  // namespace Impacto