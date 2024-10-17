#include "optionsmenu.h"
#include "../../profile_internal.h"
#include "../../../log.h"

#include "../../ui/optionsmenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/cclcc/optionsmenu.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace OptionsMenu {

void Configure() {
  BackgroundPosition = EnsureGetMemberVec2("BackgroundPosition");

  PointerSprite = EnsureGetMemberSprite("PointerSprite");
  PointerOffset = EnsureGetMemberVec2("PointerOffset");

  HeaderSprite = EnsureGetMemberSprite("HeaderSprite");
  HeaderPosition = EnsureGetMemberVec2("HeaderPosition");
  GetMemberSpriteArray(PageHeaderSprites, PageHeaderSpriteCount,
                       "PageHeaderSprites");
  PageHeaderPosition = EnsureGetMemberVec2("PageHeaderPosition");

  PagePanelSprite = EnsureGetMemberSprite("PagePanelSprite");
  PagePanelPosition = EnsureGetMemberVec2("PagePanelPosition");
  GetMemberSpriteArray(PagePanelSprites, PagePanelSpriteCount,
                       "PagePanelSprites");
  PoleAnimation = EnsureGetMemberAnimation("PoleAnimation");

  SliderTrackSprite = EnsureGetMemberSprite("SliderTrackSprite");
  SliderTrackOffset = EnsureGetMemberVec2("SliderTrackOffset");
  VoiceSliderTrackSprite = EnsureGetMemberSprite("VoiceSliderTrackSprite");
  VoiceSliderOffset = EnsureGetMemberVec2("VoiceSliderOffset");
  BinaryBoxSprite = EnsureGetMemberSprite("BinaryBoxSprite");
  BinaryBoxOffset = EnsureGetMemberVec2("BinaryBoxOffset");

  SkipReadSprite = EnsureGetMemberSprite("SkipReadSprite");
  SkipAllSprite = EnsureGetMemberSprite("SkipAllSprite");
  OnSprite = EnsureGetMemberSprite("OnSprite");
  OffSprite = EnsureGetMemberSprite("OffSprite");
  YesSprite = EnsureGetMemberSprite("YesSprite");
  NoSprite = EnsureGetMemberSprite("NoSprite");

  GuideSprite = EnsureGetMemberSprite("GuideSprite");
  VoiceGuideSprite = EnsureGetMemberSprite("VoiceGuideSprite");
  GuidePosition = EnsureGetMemberVec2("GuidePosition");

  EntriesStartPosition = EnsureGetMemberVec2("EntriesStartPosition");
  EntriesVerticalOffset = EnsureGetMemberInt("EntriesVerticalOffset");
  SoundEntriesStartPosition = EnsureGetMemberVec2("SoundEntriesStartPosition");
  SoundEntriesVerticalOffset = EnsureGetMemberInt("SoundEntriesVerticalOffset");
  VoiceEntriesOffset = EnsureGetMemberVec2("VoiceEntriesOffset");

  GetMemberSpriteArray(LabelSprites, LabelSpriteCount, "LabelSprites");
  GetMemberSpriteArray(NametagSprites, NametagSpriteCount, "NametagSprites");
  NametagOffset = EnsureGetMemberVec2("NametagOffset");
  GetMemberSpriteArray(PortraitSprites, PortraitSpriteCount, "PortraitSprites");
  VoicePosition = EnsureGetMemberVec2("VoicePosition");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::OptionsMenuPtr = new UI::CCLCC::OptionsMenu();
  UI::Menus[drawType].push_back(UI::OptionsMenuPtr);
}

}  // namespace OptionsMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto
