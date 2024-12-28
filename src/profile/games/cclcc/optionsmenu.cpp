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
  BackgroundFadeStartPosition =
      EnsureGetMemberVec2("BackgroundFadeStartPosition");

  HighlightColor = EnsureGetMemberVec3("HighlightColor");

  PointerSprite = EnsureGetMemberSprite("PointerSprite");
  PointerOffset = EnsureGetMemberVec2("PointerOffset");

  HeaderSprite = EnsureGetMemberSprite("HeaderSprite");
  HeaderPosition = EnsureGetMemberVec2("HeaderPosition");
  GetMemberSpriteArray(PageHeaderSprites, PageCount, "PageHeaderSprites");
  PageHeaderPosition = EnsureGetMemberVec2("PageHeaderPosition");

  PagePanelSprite = EnsureGetMemberSprite("PagePanelSprite");
  PagePanelPosition = EnsureGetMemberVec2("PagePanelPosition");
  PagePanelFadeStartPosition =
      EnsureGetMemberVec2("PagePanelFadeStartPosition");
  GetMemberSpriteArray(PagePanelSprites, PagePanelSpriteCount,
                       "PagePanelSprites");
  GetMemberVec2Array(PagePanelIconOffsets, PageCount, "PagePanelIconOffsets");
  GetMemberRectFArray(PagePanelHoverBounds, PageCount, "PagePanelHoverBounds");
  PoleAnimation = EnsureGetMemberAnimation("PoleAnimation");

  SliderTrackSprite = EnsureGetMemberSprite("SliderTrackSprite");
  SliderTrackOffset = EnsureGetMemberVec2("SliderTrackOffset");
  VoiceSliderTrackSprite = EnsureGetMemberSprite("VoiceSliderTrackSprite");
  VoiceSliderOffset = EnsureGetMemberVec2("VoiceSliderOffset");
  BinaryBoxSprite = EnsureGetMemberSprite("BinaryBoxSprite");
  BinaryBoxOffset = EnsureGetMemberVec2("BinaryBoxOffset");
  SliderSpeed = EnsureGetMemberFloat("SliderSpeed");

  SkipReadSprite = EnsureGetMemberSprite("SkipReadSprite");
  SkipAllSprite = EnsureGetMemberSprite("SkipAllSprite");
  OnSprite = EnsureGetMemberSprite("OnSprite");
  OffSprite = EnsureGetMemberSprite("OffSprite");
  YesSprite = EnsureGetMemberSprite("YesSprite");
  NoSprite = EnsureGetMemberSprite("NoSprite");

  GuideSprite = EnsureGetMemberSprite("GuideSprite");
  VoiceGuideSprite = EnsureGetMemberSprite("VoiceGuideSprite");
  GuidePosition = EnsureGetMemberVec2("GuidePosition");
  GuideFadeStartPosition = EnsureGetMemberVec2("GuideFadeStartPosition");

  EntriesStartPosition = EnsureGetMemberVec2("EntriesStartPosition");
  EntriesVerticalOffset = EnsureGetMemberInt("EntriesVerticalOffset");
  SoundEntriesStartPosition = EnsureGetMemberVec2("SoundEntriesStartPosition");
  SoundEntriesVerticalOffset = EnsureGetMemberInt("SoundEntriesVerticalOffset");
  VoiceEntriesOffset = EnsureGetMemberVec2("VoiceEntriesOffset");
  EntryDimensions = EnsureGetMemberVec2("EntryDimensions");
  VoiceEntryDimensions = EnsureGetMemberVec2("VoiceEntryDimensions");

  GetMemberSpriteArray(LabelSprites, LabelCount, "LabelSprites");
  LabelOffset = EnsureGetMemberVec2("LabelOffset");
  GetMemberSpriteArray(NametagSprites, NametagCount, "NametagSprites");
  NametagOffset = EnsureGetMemberVec2("NametagOffset");
  GetMemberSpriteArray(PortraitSprites, PortraitCount, "PortraitSprites");
  PortraitOffset = EnsureGetMemberVec2("PortraitOffset");
  VoicePosition = EnsureGetMemberVec2("VoicePosition");

  MinButtonHoldTime = EnsureGetMemberFloat("MinButtonHoldTime");
  ButtonHoldTimeInterval = EnsureGetMemberFloat("ButtonHoldTimeInterval");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::OptionsMenuPtr = new UI::CCLCC::OptionsMenu();
  UI::Menus[drawType].push_back(UI::OptionsMenuPtr);
}

}  // namespace OptionsMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto
