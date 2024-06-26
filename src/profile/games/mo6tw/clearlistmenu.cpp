#include "clearlistmenu.h"
#include "../../profile_internal.h"
#include "../../../ui/ui.h"
#include "../../../games/mo6tw/clearlistmenu.h"
#include "../../../text.h"
#include "../../../game.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace ClearListMenu {

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  WindowSprite = EnsureGetMemberSprite("WindowSprite");
  WindowPosition = EnsureGetMemberVec2("WindowPosition");
  WindowSpritePartLeft = EnsureGetMemberSprite("WindowSpritePartLeft");
  WindowSpritePartRight = EnsureGetMemberSprite("WindowSpritePartRight");
  FontSize = EnsureGetMemberFloat("FontSize");
  SeparatorTable = EnsureGetMemberInt("SeparatorTable");
  SeparatorEntry = EnsureGetMemberInt("SeparatorEntry");

  LabelPosition = EnsureGetMemberVec2("LabelPosition");

  ClearListLabel = EnsureGetMemberSprite("ClearListLabel");
  EndingsLabelPosition = EnsureGetMemberVec2("EndingsLabelPosition");
  EndingsLabel = EnsureGetMemberSprite("EndingsLabel");
  EndingCountPosition = EnsureGetMemberVec2("EndingCountPosition");
  ScenesLabelPosition = EnsureGetMemberVec2("ScenesLabelPosition");
  ScenesLabel = EnsureGetMemberSprite("ScenesLabel");
  SceneCountPosition = EnsureGetMemberVec2("SceneCountPosition");
  CompletionLabelPosition = EnsureGetMemberVec2("CompletionLabelPosition");
  CompletionLabel = EnsureGetMemberSprite("CompletionLabel");
  CompletionPosition = EnsureGetMemberVec2("CompletionPosition");
  AlbumLabelPosition = EnsureGetMemberVec2("AlbumLabelPosition");
  AlbumLabel = EnsureGetMemberSprite("AlbumLabel");
  AlbumCountPosition = EnsureGetMemberVec2("AlbumCountPosition");
  PlayTimeLabelPosition = EnsureGetMemberVec2("PlayTimeLabelPosition");
  PlayTimeLabel = EnsureGetMemberSprite("PlayTimeLabel");
  PlayTimeTextTable = EnsureGetMemberInt("PlayTimeTextTable");
  PlayTimeSecondsTextEntry = EnsureGetMemberInt("PlayTimeSecondsTextEntry");
  PlayTimeMinutesTextEntry = EnsureGetMemberInt("PlayTimeMinutesTextEntry");
  PlayTimeHoursTextEntry = EnsureGetMemberInt("PlayTimeHoursTextEntry");
  PlayTimeSecondsTextPosition =
      EnsureGetMemberVec2("PlayTimeSecondsTextPosition");
  PlayTimeMinutesTextPosition =
      EnsureGetMemberVec2("PlayTimeMinutesTextPosition");
  PlayTimeHoursTextPosition = EnsureGetMemberVec2("PlayTimeHoursTextPosition");
  PlayTimeSecondsPosition = EnsureGetMemberVec2("PlayTimeSecondsPosition");
  PlayTimeMinutesPosition = EnsureGetMemberVec2("PlayTimeMinutesPosition");
  PlayTimeHoursPosition = EnsureGetMemberVec2("PlayTimeHoursPosition");
  ClearListColorIndex = EnsureGetMemberInt("ClearListColorIndex");
  ClearListTextBackground = EnsureGetMemberSprite("ClearListTextBackground");
  ClearListTextBGOffset = EnsureGetMemberVec2("ClearListTextBGOffset");

  EndingListLabel = EnsureGetMemberSprite("EndingListLabel");
  EndingCount = EnsureGetMemberInt("EndingCount");
  EndingsListNumberInitialPosition =
      EnsureGetMemberVec2("EndingsListNumberInitialPosition");
  EndingsListTextInitialPosition =
      EnsureGetMemberVec2("EndingsListTextInitialPosition");
  EndingsListTextMargin = EnsureGetMemberVec2("EndingsListTextMargin");
  EndingsListTextFontSize = EnsureGetMemberFloat("EndingsListTextFontSize");
  EndingsListTextLockedTable = EnsureGetMemberInt("EndingsListTextLockedTable");
  EndingsListTextLockedEntry = EnsureGetMemberInt("EndingsListTextLockedEntry");
  EndingsListTextTable = EnsureGetMemberInt("EndingsListTextTable");
  EndingsListTextColorIndex = EnsureGetMemberInt("EndingsListTextColorIndex");

  SceneTitleLabel = EnsureGetMemberSprite("SceneTitleLabel");
  SceneCount = EnsureGetMemberInt("SceneCount");
  SceneListNumberInitialPosition =
      EnsureGetMemberVec2("SceneListNumberInitialPosition");
  SceneListTextInitialPosition =
      EnsureGetMemberVec2("SceneListTextInitialPosition");
  SceneListTextMargin = EnsureGetMemberVec2("SceneListTextMargin");
  SceneListFontSize = EnsureGetMemberFloat("SceneListFontSize");
  SceneListTextTable = EnsureGetMemberInt("SceneListTextTable");
  SceneTitleItemsRenderingBounds =
      EnsureGetMemberRectF("SceneTitleItemsRenderingBounds");
  SceneListColorIndex = EnsureGetMemberInt("SceneListColorIndex");
  SceneTitleLockedTable = EnsureGetMemberInt("SceneTitleLockedTable");
  SceneTitleLockedEntry = EnsureGetMemberInt("SceneTitleLockedEntry");
  SceneTitleItemsWidth = EnsureGetMemberFloat("SceneTitleItemsWidth");
  ScrollbarStart = EnsureGetMemberFloat("ScrollbarStart");
  ScrollAreaHeight = EnsureGetMemberFloat("ScrollAreaHeight");
  ScrollbarTrack = EnsureGetMemberSprite("ScrollbarTrackSprite");
  ScrollbarThumb = EnsureGetMemberSprite("ScrollbarThumbSprite");
  ScrollbarPosition = EnsureGetMemberVec2("ScrollbarPosition");

  ArrowLeft = EnsureGetMemberSprite("ArrowLeft");
  ArrowLeftPosition = EnsureGetMemberVec2("ArrowLeftPosition");
  ArrowRight = EnsureGetMemberSprite("ArrowRight");
  ArrowRightPosition = EnsureGetMemberVec2("ArrowRightPosition");

  ArrowsAnimationDuration = EnsureGetMemberFloat("ArrowsAnimationDuration");
  FadeInDuration = EnsureGetMemberFloat("FadeInDuration");
  FadeOutDuration = EnsureGetMemberFloat("FadeOutDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMemberInt("DrawType"));

  auto clearList = new UI::MO6TW::ClearListMenu();
  UI::Menus[drawType].push_back(clearList);
}

}  // namespace ClearListMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto