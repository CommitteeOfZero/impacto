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
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  BackgroundPosition = EnsureGetMember<glm::vec2>("BackgroundPosition");
  BackgroundFadeStartPosition =
      EnsureGetMember<glm::vec2>("BackgroundFadeStartPosition");

  HighlightColor = EnsureGetMember<glm::vec3>("HighlightColor");

  PointerSprite = EnsureGetMember<Sprite>("PointerSprite");
  PointerOffset = EnsureGetMember<glm::vec2>("PointerOffset");

  HeaderSprite = EnsureGetMember<Sprite>("HeaderSprite");
  HeaderPosition = EnsureGetMember<glm::vec2>("HeaderPosition");
  GetMemberArray<Sprite>(PageHeaderSprites, PageCount, "PageHeaderSprites");
  PageHeaderPosition = EnsureGetMember<glm::vec2>("PageHeaderPosition");

  PagePanelSprite = EnsureGetMember<Sprite>("PagePanelSprite");
  PagePanelPosition = EnsureGetMember<glm::vec2>("PagePanelPosition");
  PagePanelFadeStartPosition =
      EnsureGetMember<glm::vec2>("PagePanelFadeStartPosition");
  GetMemberArray<Sprite>(PagePanelSprites, PagePanelSpriteCount,
                       "PagePanelSprites");
  GetMemberArray<glm::vec2>(PagePanelIconOffsets, PageCount,
                            "PagePanelIconOffsets");
  GetMemberArray<RectF>(PagePanelHoverBounds, PageCount,
                        "PagePanelHoverBounds");
  PoleAnimation = EnsureGetMember<SpriteAnimationDef>("PoleAnimation");

  SliderTrackSprite = EnsureGetMember<Sprite>("SliderTrackSprite");
  SliderTrackOffset = EnsureGetMember<glm::vec2>("SliderTrackOffset");
  VoiceSliderTrackSprite = EnsureGetMember<Sprite>("VoiceSliderTrackSprite");
  VoiceSliderOffset = EnsureGetMember<glm::vec2>("VoiceSliderOffset");
  BinaryBoxSprite = EnsureGetMember<Sprite>("BinaryBoxSprite");
  BinaryBoxOffset = EnsureGetMember<glm::vec2>("BinaryBoxOffset");
  SliderSpeed = EnsureGetMember<float>("SliderSpeed");

  SkipReadSprite = EnsureGetMember<Sprite>("SkipReadSprite");
  SkipAllSprite = EnsureGetMember<Sprite>("SkipAllSprite");
  OnSprite = EnsureGetMember<Sprite>("OnSprite");
  OffSprite = EnsureGetMember<Sprite>("OffSprite");
  YesSprite = EnsureGetMember<Sprite>("YesSprite");
  NoSprite = EnsureGetMember<Sprite>("NoSprite");

  GuideSprite = EnsureGetMember<Sprite>("GuideSprite");
  VoiceGuideSprite = EnsureGetMember<Sprite>("VoiceGuideSprite");
  GuidePosition = EnsureGetMember<glm::vec2>("GuidePosition");
  GuideFadeStartPosition = EnsureGetMember<glm::vec2>("GuideFadeStartPosition");

  EntriesStartPosition = EnsureGetMember<glm::vec2>("EntriesStartPosition");
  EntriesVerticalOffset = EnsureGetMember<int>("EntriesVerticalOffset");
  SoundEntriesStartPosition =
      EnsureGetMember<glm::vec2>("SoundEntriesStartPosition");
  SoundEntriesVerticalOffset =
      EnsureGetMember<int>("SoundEntriesVerticalOffset");
  VoiceEntriesOffset = EnsureGetMember<glm::vec2>("VoiceEntriesOffset");
  EntryDimensions = EnsureGetMember<glm::vec2>("EntryDimensions");
  VoiceEntryDimensions = EnsureGetMember<glm::vec2>("VoiceEntryDimensions");

  GetMemberArray<Sprite>(LabelSprites, LabelCount, "LabelSprites");
  LabelOffset = EnsureGetMember<glm::vec2>("LabelOffset");
  GetMemberArray<Sprite>(NametagSprites, NametagCount, "NametagSprites");
  NametagOffset = EnsureGetMember<glm::vec2>("NametagOffset");
  GetMemberArray<Sprite>(PortraitSprites, PortraitCount, "PortraitSprites");
  PortraitOffset = EnsureGetMember<glm::vec2>("PortraitOffset");
  VoicePosition = EnsureGetMember<glm::vec2>("VoicePosition");

  MenuMaskSprite = EnsureGetMember<Sprite>("MenuMask");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<int>("DrawType"));

  UI::OptionsMenuPtr = new UI::CCLCC::OptionsMenu();
  UI::Menus[drawType].push_back(UI::OptionsMenuPtr);
}

}  // namespace OptionsMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto
