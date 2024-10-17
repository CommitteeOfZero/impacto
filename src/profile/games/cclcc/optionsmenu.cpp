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
  HeaderSprite = EnsureGetMemberSprite("HeaderSprite");
  GetMemberSpriteArray(PageHeaderSprites, PageHeaderSpriteCount,
                       "PageHeaderSprites");
  PoleAnimation = EnsureGetMemberAnimation("PoleAnimation");

  SliderTrackSprite = EnsureGetMemberSprite("SliderTrackSprite");
  VoiceSliderTrackSprite = EnsureGetMemberSprite("VoiceSliderTrackSprite");
  BinaryBoxSprite = EnsureGetMemberSprite("BinaryBoxSprite");

  SkipReadSprite = EnsureGetMemberSprite("SkipReadSprite");
  SkipAllSprite = EnsureGetMemberSprite("SkipAllSprite");
  OnSprite = EnsureGetMemberSprite("OnSprite");
  OffSprite = EnsureGetMemberSprite("OffSprite");
  YesSprite = EnsureGetMemberSprite("YesSprite");
  NoSprite = EnsureGetMemberSprite("NoSprite");

  GuideSprite = EnsureGetMemberSprite("GuideSprite");
  VoiceGuideSprite = EnsureGetMemberSprite("VoiceGuideSprite");

  GetMemberSpriteArray(LabelSprites, LabelSpriteCount, "LabelSprites");
  GetMemberSpriteArray(NametagSprites, NametagSpriteCount, "NametagSprites");
  GetMemberSpriteArray(PortraitSprites, PortraitSpriteCount, "PortraitSprites");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  UI::OptionsMenuPtr = new UI::CCLCC::OptionsMenu();
  UI::Menus[drawType].push_back(UI::OptionsMenuPtr);
}

}  // namespace OptionsMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto
