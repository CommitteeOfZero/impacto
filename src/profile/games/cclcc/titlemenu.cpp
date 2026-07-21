#include "titlemenu.h"
#include "../../../log.h"
#include "../../profile_internal.h"

#include "../../ui/titlemenu.h"
#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/cclcc/titlemenu.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace TitleMenu {

void Configure() {
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  MainBackgroundSprite = EnsureGetMember<Sprite>("MainBackgroundSprite");
  CopyrightTextSprite = EnsureGetMember<Sprite>("CopyrightTextSprite");
  MenuSprite = EnsureGetMember<Sprite>("MenuSprite");
  ItemHighlightSprite = EnsureGetMember<Sprite>("ItemHighlightSprite");
  LoadSprite = EnsureGetMember<Sprite>("LoadSprite");
  LoadHighlightSprite = EnsureGetMember<Sprite>("LoadHighlightSprite");
  QuickLoadSprite = EnsureGetMember<Sprite>("QuickLoadSprite");
  QuickLoadHighlightSprite =
      EnsureGetMember<Sprite>("QuickLoadHighlightSprite");
  TipsSprite = EnsureGetMember<Sprite>("TipsSprite");
  TipsHighlightSprite = EnsureGetMember<Sprite>("TipsHighlightSprite");
  LibrarySprite = EnsureGetMember<Sprite>("LibrarySprite");
  LibraryHighlightSprite = EnsureGetMember<Sprite>("LibraryHighlightSprite");
  EndingListSprite = EnsureGetMember<Sprite>("EndingListSprite");
  EndingListHighlightSprite =
      EnsureGetMember<Sprite>("EndingListHighlightSprite");
  if (HasScriptedExitLogic) {
    ExitSprite = EnsureGetMember<Sprite>("ExitSprite");
  }

  PrimaryFadeInDuration = EnsureGetMember<float>("PrimaryFadeInDuration");
  PrimaryFadeOutDuration = EnsureGetMember<float>("PrimaryFadeOutDuration");
  SecondaryFadeInDuration = EnsureGetMember<float>("SecondaryFadeInDuration");
  SecondaryFadeOutDuration = EnsureGetMember<float>("SecondaryFadeOutDuration");
  CopyrightTextPos = EnsureGetMember<glm::vec2>("CopyrightTextPos");
  MenuPos = EnsureGetMember<glm::vec2>("MenuPos");
  ItemHighlightOffset = EnsureGetMember<glm::vec2>("ItemHighlightOffset");
  ItemPadding = EnsureGetMember<float>("ItemPadding");
  ItemYBase = EnsureGetMember<float>("ItemYBase");
  SecondaryFirstItemHighlightOffsetX =
      EnsureGetMember<float>("SecondaryFirstItemHighlightOffsetX");
  SecondarySecondItemHighlightOffsetX =
      EnsureGetMember<float>("SecondarySecondItemHighlightOffsetX");
  SecondaryThirdItemHighlightOffsetX =
      EnsureGetMember<float>("SecondaryThirdItemHighlightOffsetX");
  ItemHighlightPointerSprite =
      EnsureGetMember<Sprite>("ItemHighlightPointerSprite");
  ItemHighlightPointerY = EnsureGetMember<float>("ItemHighlightPointerY");

  TitleAnimationDurationIn = EnsureGetMember<float>("TitleAnimationDurationIn");
  TitleAnimationDurationOut =
      EnsureGetMember<float>("TitleAnimationDurationOut");
  TitleAnimationStartFrame = EnsureGetMember<int>("TitleAnimationStartFrame");
  TitleAnimationFrameCount = EnsureGetMember<int>("TitleAnimationFrameCount");
  TitleAnimationFileId = EnsureGetMember<int>("TitleAnimationFileId");

  ChoiceBlinkAnimationDurationIn =
      EnsureGetMember<float>("ChoiceBlinkAnimationDurationIn");
  SlideItemsAnimationDurationIn =
      EnsureGetMember<float>("SlideItemsAnimationDurationIn");
  SlideItemsAnimationDurationOut =
      EnsureGetMember<float>("SlideItemsAnimationDurationOut");
  HighlightAnimationDurationIn =
      EnsureGetMember<float>("HighlightAnimationDurationIn");
  HighlightAnimationDurationOut =
      EnsureGetMember<float>("HighlightAnimationDurationOut");
  ExtraDisabledTint = EnsureGetMember<uint32_t>("ExtraDisabledTint");

  UI::TitleMenuPtr = new UI::CCLCC::TitleMenu();
  auto drawType = EnsureGetMember<Game::DrawComponentType>("DrawType");
  UI::Menus[drawType].push_back(UI::TitleMenuPtr);
}

}  // namespace TitleMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto