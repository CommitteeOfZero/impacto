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
      EnsureGetMemberInt("DrawType"));

  NextButtonSprite = EnsureGetMemberSprite("NextButtonSprite");
  NextButtonHighlightedSprite =
      EnsureGetMemberSprite("NextButtonHighlightedSprite");
  NextButtonPosition = EnsureGetMemberVec2("NextButtonPosition");

  BackButtonSprite = EnsureGetMemberSprite("BackButtonSprite");
  BackButtonHighlightedSprite =
      EnsureGetMemberSprite("BackButtonHighlightedSprite");
  BackButtonPosition = EnsureGetMemberVec2("BackButtonPosition");

  ButtonHighlight = EnsureGetMemberSprite("ButtonHighlight");
  PageLabelPosition = EnsureGetMemberVec2("PageLabelPosition");
  ListStartingPosition = EnsureGetMemberVec2("ListStartingPosition");
  ListPadding = EnsureGetMemberVec2("ListPadding");
  OptionGroupItemsOffset = EnsureGetMemberVec2("OptionGroupItemsOffset");
  OptionGroupSliderOffset = EnsureGetMemberVec2("OptionGroupSliderOffset");

  TextPageLabel = EnsureGetMemberSprite("TextPageLabel");

  TextSpeedOptionsNum = EnsureGetMemberInt("TextSpeedOptionsNum");
  if (TextSpeedOptionsNum > 0) {
    TextSpeedOptionsLabel = EnsureGetMemberSprite("TextSpeedOptionsLabel");
    TextSpeedOptionsLabelH = EnsureGetMemberSprite("TextSpeedOptionsLabelH");

    GetMemberSpriteArray(TextSpeedOptionsSprites, TextSpeedOptionsNum,
                         "TextSpeedOptionsSprites");
    GetMemberSpriteArray(TextSpeedOptionsHSprites, TextSpeedOptionsNum,
                         "TextSpeedOptionsHSprites");
  }

  AutoModeOptionsNum = EnsureGetMemberInt("AutoModeOptionsNum");
  if (AutoModeOptionsNum > 0) {
    AutoModeOptionsLabel = EnsureGetMemberSprite("AutoModeOptionsLabel");
    AutoModeOptionsLabelH = EnsureGetMemberSprite("AutoModeOptionsLabelH");

    GetMemberSpriteArray(AutoModeOptionsSprites, AutoModeOptionsNum,
                         "AutoModeOptionsSprites");
    GetMemberSpriteArray(AutoModeOptionsHSprites, AutoModeOptionsNum,
                         "AutoModeOptionsHSprites");
  }

  SkipModeOptionsNum = EnsureGetMemberInt("SkipModeOptionsNum");
  if (SkipModeOptionsNum > 0) {
    SkipModeOptionsLabel = EnsureGetMemberSprite("SkipModeOptionsLabel");
    SkipModeOptionsLabelH = EnsureGetMemberSprite("SkipModeOptionsLabelH");

    GetMemberSpriteArray(SkipModeOptionsSprites, SkipModeOptionsNum,
                         "SkipModeOptionsSprites");
    GetMemberSpriteArray(SkipModeOptionsHSprites, SkipModeOptionsNum,
                         "SkipModeOptionsHSprites");
  }

  SoundPageLabel = EnsureGetMemberSprite("SoundPageLabel");
  SoundModeOptionsNum = EnsureGetMemberInt("SoundModeOptionsNum");
  if (SoundModeOptionsNum > 0) {
    VoiceSyncOptionsLabel = EnsureGetMemberSprite("VoiceSyncOptionsLabel");
    VoiceSyncOptionsLabelH = EnsureGetMemberSprite("VoiceSyncOptionsLabelH");
    VoiceSkipOptionsLabel = EnsureGetMemberSprite("VoiceSkipOptionsLabel");
    VoiceSkipOptionsLabelH = EnsureGetMemberSprite("VoiceSkipOptionsLabelH");
    VoiceHighlightOptionsLabel =
        EnsureGetMemberSprite("VoiceHighlightOptionsLabel");
    VoiceHighlightOptionsLabelH =
        EnsureGetMemberSprite("VoiceHighlightOptionsLabelH");

    GetMemberSpriteArray(SoundModeOptionsSprites, SoundModeOptionsNum,
                         "SoundModeOptionsSprites");
    GetMemberSpriteArray(SoundModeOptionsHSprites, SoundModeOptionsNum,
                         "SoundModeOptionsHSprites");
  }

  BgmVolumeLabel = EnsureGetMemberSprite("BgmVolumeLabel");
  BgmVolumeLabelH = EnsureGetMemberSprite("BgmVolumeLabelH");
  VoiceVolumeLabel = EnsureGetMemberSprite("VoiceVolumeLabel");
  VoiceVolumeLabelH = EnsureGetMemberSprite("VoiceVolumeLabelH");
  SeVolumeLabel = EnsureGetMemberSprite("SeVolumeLabel");
  SeVolumeLabelH = EnsureGetMemberSprite("SeVolumeLabelH");
  SystemSeVolumeLabel = EnsureGetMemberSprite("SystemSeVolumeLabel");
  SystemSeVolumeLabelH = EnsureGetMemberSprite("SystemSeVolumeLabelH");
  CharacterVoiceVolumeLabel =
      EnsureGetMemberSprite("CharacterVoiceVolumeLabel");
  CharacterVoiceVolumeLabelH =
      EnsureGetMemberSprite("CharacterVoiceVolumeLabelH");

  OtherPageLabel = EnsureGetMemberSprite("OtherPageLabel");

  QuickSaveOptionsNum = EnsureGetMemberInt("QuickSaveOptionsNum");
  if (QuickSaveOptionsNum > 0) {
    QuickSaveOptionsLabel = EnsureGetMemberSprite("QuickSaveOptionsLabel");
    QuickSaveOptionsLabelH = EnsureGetMemberSprite("QuickSaveOptionsLabelH");

    GetMemberSpriteArray(QuickSaveOptionsSprites, AutoModeOptionsNum,
                         "QuickSaveOptionsSprites");
    GetMemberSpriteArray(QuickSaveOptionsHSprites, AutoModeOptionsNum,
                         "QuickSaveOptionsHSprites");
  }

  UI::OptionsMenuPtr = new UI::MO8::OptionsMenu();
  UI::Menus[drawType].push_back(UI::OptionsMenuPtr);
}

}  // namespace OptionsMenu
}  // namespace MO8
}  // namespace Profile
}  // namespace Impacto
