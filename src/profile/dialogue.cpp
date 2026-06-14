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
  OldNametagPositioning =
      TryGetMember<bool>("OldNametagPositioning").value_or(false);
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

static void ConfigureTextModesInfo() {
  if (!TryPushMember("TextModesInfo")) return;
  ForEachProfileArray([](uint32_t) {
    const size_t index = EnsureGetKey<size_t>();
    if (index >= ProfileTextModesInfoFields.size()) {
      throw std::runtime_error(
          fmt::format("TextModeInfo index {} out of range [0, {})", index,
                      ProfileTextModesInfoFields.size()));
    }
    TextModeInfo& info = TextModesInfo[index];
    auto& fieldInfo = ProfileTextModesInfoFields[index];

    const auto tryOverride = [&fieldInfo]<typename T>(const char* const name,
                                                      T& out) {
      assert(magic_enum::enum_contains<TextModeInfoFieldFlags>(name));

      // Only update value if it's actually defined by the profile
      std::optional<T> member = TryGetMember<T>(name);
      if (member.has_value()) {
        out = *member;
        fieldInfo.set(
            magic_enum::enum_cast<TextModeInfoFieldFlags>(name).value());
      }
    };

    tryOverride("DisplayMode", info.DisplayMode);
    tryOverride("WindowId", info.WindowId);
    tryOverride("WindowPos", info.WindowPos);
    tryOverride("NameDispMode", info.NameDispMode);
    tryOverride("MaxNameWidth", info.MaxNameWidth);
    tryOverride("NamePos", info.NamePos);
    tryOverride("NameGlyphSize", info.NameGlyphSize);
    tryOverride("MaxLineWidth", info.MaxLineWidth);
    tryOverride("CurrentPageId", info.CurrentPageId);
    tryOverride("WaitIconPos", info.WaitIconPos);
    tryOverride("TextGlyphSize", info.TextGlyphSize);
    tryOverride("RubyGlyphSize", info.RubyGlyphSize);
    tryOverride("LineSpacing", info.LineSpacing);
    tryOverride("RubyLineSpacing", info.RubyLineSpacing);
    tryOverride("AlwaysAddRubySpacing", info.AlwaysAddRubySpacing);
    tryOverride("LinefeedSpacing", info.LinefeedSpacing);
    tryOverride("NameAlignment", info.NameAlignment);
    tryOverride("UseNameLengthL", info.UseNameLengthL);
    tryOverride("NameLengthL", info.NameLengthL);
  });
  Pop();
}

void Configure() {
  EnsurePushMemberOfType("Dialogue", LUA_TTABLE);

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

  REVMessageModeIdx = EnsureGetMember<size_t>("REVMessageModeIdx");
  REVColor = EnsureGetMember<int>("REVColor");
  REVNameColor = EnsureGetMember<int>("REVNameColor");
  REVOutlineMode = RendererOutlineMode(EnsureGetMember<int>("REVOutlineMode"));
  REVNameOutlineMode =
      RendererOutlineMode(EnsureGetMember<int>("REVNameOutlineMode"));

  TipsMessageModeIdx = EnsureGetMember<size_t>("TipsMessageModeIdx");
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

  MaxPageSize = EnsureGetMember<int>("MaxPageSize");
  PageCount = EnsureGetMember<int>("PageCount");

  ColorTable = EnsureGetMember<std::vector<DialogueColorPair>>("ColorTable");
  ColorTagIsUint8 = EnsureGetMember<bool>("ColorTagIsUint8");

  ConfigureNametag();
  ConfigureTextModesInfo();

  TryGetMember<bool>("HasSpeakerPortraits", HasSpeakerPortraits);

  Pop();
}

}  // namespace Dialogue
}  // namespace Profile
}  // namespace Impacto