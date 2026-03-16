#include "optionsmenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"

#include "../../ui/optionsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/mo8/optionsmenu.h"

namespace Impacto {
namespace Profile {
namespace MO8 {
namespace OptionsMenu {

void Configure() {
  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");

  NextButtonSprite = EnsureGetMember<Sprite>("NextButtonSprite");
  NextButtonHighlightedSprite =
      EnsureGetMember<Sprite>("NextButtonHighlightedSprite");
  NextButtonPosition = EnsureGetMember<glm::vec2>("NextButtonPosition");

  BackButtonSprite = EnsureGetMember<Sprite>("BackButtonSprite");
  BackButtonHighlightedSprite =
      EnsureGetMember<Sprite>("BackButtonHighlightedSprite");
  BackButtonPosition = EnsureGetMember<glm::vec2>("BackButtonPosition");

  SliderTrackSprite = EnsureGetMember<Sprite>("SliderTrackSprite");
  SliderFillSprite = EnsureGetMember<Sprite>("SliderFillSprite");
  SliderThumbSprite = EnsureGetMember<Sprite>("SliderThumbSprite");

  ButtonHighlight = EnsureGetMember<Sprite>("ButtonHighlight");
  PageLabelPosition = EnsureGetMember<glm::vec2>("PageLabelPosition");
  ListStartingPosition = EnsureGetMember<glm::vec2>("ListStartingPosition");
  ListPadding = EnsureGetMember<glm::vec2>("ListPadding");
  OptionGroupItemsOffset = EnsureGetMember<glm::vec2>("OptionGroupItemsOffset");
  OptionGroupSliderOffset =
      EnsureGetMember<glm::vec2>("OptionGroupSliderOffset");

  TextPageLabel = EnsureGetMember<Sprite>("TextPageLabel");

  TextSpeedOptionsNum = EnsureGetMember<int>("TextSpeedOptionsNum");
  if (TextSpeedOptionsNum > 0) {
    TextSpeedOptionsLabel = EnsureGetMember<Sprite>("TextSpeedOptionsLabel");
    TextSpeedOptionsLabelH = EnsureGetMember<Sprite>("TextSpeedOptionsLabelH");

    GetMemberArray<Sprite>(
        std::span(TextSpeedOptionsSprites, TextSpeedOptionsNum),
        "TextSpeedOptionsSprites");
    GetMemberArray<Sprite>(
        std::span(TextSpeedOptionsHSprites, TextSpeedOptionsNum),
        "TextSpeedOptionsHSprites");
  }

  AutoModeOptionsNum = EnsureGetMember<int>("AutoModeOptionsNum");
  if (AutoModeOptionsNum > 0) {
    AutoModeOptionsLabel = EnsureGetMember<Sprite>("AutoModeOptionsLabel");
    AutoModeOptionsLabelH = EnsureGetMember<Sprite>("AutoModeOptionsLabelH");

    GetMemberArray<Sprite>(
        std::span(AutoModeOptionsSprites, AutoModeOptionsNum),
        "AutoModeOptionsSprites");
    GetMemberArray<Sprite>(
        std::span(AutoModeOptionsHSprites, AutoModeOptionsNum),
        "AutoModeOptionsHSprites");
  }

  SkipModeOptionsNum = EnsureGetMember<int>("SkipModeOptionsNum");
  if (SkipModeOptionsNum > 0) {
    SkipModeOptionsLabel = EnsureGetMember<Sprite>("SkipModeOptionsLabel");
    SkipModeOptionsLabelH = EnsureGetMember<Sprite>("SkipModeOptionsLabelH");

    GetMemberArray<Sprite>(
        std::span(SkipModeOptionsSprites, SkipModeOptionsNum),
        "SkipModeOptionsSprites");
    GetMemberArray<Sprite>(
        std::span(SkipModeOptionsHSprites, SkipModeOptionsNum),
        "SkipModeOptionsHSprites");
  }

  SoundPageLabel = EnsureGetMember<Sprite>("SoundPageLabel");
  SoundModeOptionsNum = EnsureGetMember<int>("SoundModeOptionsNum");
  if (SoundModeOptionsNum > 0) {
    VoiceSyncOptionsLabel = EnsureGetMember<Sprite>("VoiceSyncOptionsLabel");
    VoiceSyncOptionsLabelH = EnsureGetMember<Sprite>("VoiceSyncOptionsLabelH");
    VoiceSkipOptionsLabel = EnsureGetMember<Sprite>("VoiceSkipOptionsLabel");
    VoiceSkipOptionsLabelH = EnsureGetMember<Sprite>("VoiceSkipOptionsLabelH");
    VoiceHighlightOptionsLabel =
        EnsureGetMember<Sprite>("VoiceHighlightOptionsLabel");
    VoiceHighlightOptionsLabelH =
        EnsureGetMember<Sprite>("VoiceHighlightOptionsLabelH");

    GetMemberArray<Sprite>(
        std::span(SoundModeOptionsSprites, SoundModeOptionsNum),
        "SoundModeOptionsSprites");
    GetMemberArray<Sprite>(
        std::span(SoundModeOptionsHSprites, SoundModeOptionsNum),
        "SoundModeOptionsHSprites");
  }

  BgmVolumeLabel = EnsureGetMember<Sprite>("BgmVolumeLabel");
  BgmVolumeLabelH = EnsureGetMember<Sprite>("BgmVolumeLabelH");
  VoiceVolumeLabel = EnsureGetMember<Sprite>("VoiceVolumeLabel");
  VoiceVolumeLabelH = EnsureGetMember<Sprite>("VoiceVolumeLabelH");
  SeVolumeLabel = EnsureGetMember<Sprite>("SeVolumeLabel");
  SeVolumeLabelH = EnsureGetMember<Sprite>("SeVolumeLabelH");
  SystemSeVolumeLabel = EnsureGetMember<Sprite>("SystemSeVolumeLabel");
  SystemSeVolumeLabelH = EnsureGetMember<Sprite>("SystemSeVolumeLabelH");
  CharacterVoiceVolumeLabel =
      EnsureGetMember<Sprite>("CharacterVoiceVolumeLabel");
  CharacterVoiceVolumeLabelH =
      EnsureGetMember<Sprite>("CharacterVoiceVolumeLabelH");

  OtherPageLabel = EnsureGetMember<Sprite>("OtherPageLabel");

  QuickSaveOptionsNum = EnsureGetMember<int>("QuickSaveOptionsNum");
  if (QuickSaveOptionsNum > 0) {
    QuickSaveOptionsLabel = EnsureGetMember<Sprite>("QuickSaveOptionsLabel");
    QuickSaveOptionsLabelH = EnsureGetMember<Sprite>("QuickSaveOptionsLabelH");

    GetMemberArray<Sprite>(
        std::span(QuickSaveOptionsSprites, AutoModeOptionsNum),
        "QuickSaveOptionsSprites");
    GetMemberArray<Sprite>(
        std::span(QuickSaveOptionsHSprites, AutoModeOptionsNum),
        "QuickSaveOptionsHSprites");
  }

  UI::OptionsMenuPtr = new UI::MO8::OptionsMenu();
  UI::Menus[drawType].push_back(UI::OptionsMenuPtr);
}

}  // namespace OptionsMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto
