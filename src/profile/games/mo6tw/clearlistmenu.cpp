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
  BackgroundSprite = EnsureGetMember<Sprite>("BackgroundSprite");
  WindowSprite = EnsureGetMember<Sprite>("WindowSprite");
  WindowPosition = EnsureGetMember<glm::vec2>("WindowPosition");
  WindowSpritePartLeft = EnsureGetMember<Sprite>("WindowSpritePartLeft");
  WindowSpritePartRight = EnsureGetMember<Sprite>("WindowSpritePartRight");
  FontSize = EnsureGetMember<float>("FontSize");
  SeparatorTable = EnsureGetMember<int>("SeparatorTable");
  SeparatorEntry = EnsureGetMember<int>("SeparatorEntry");

  LabelPosition = EnsureGetMember<glm::vec2>("LabelPosition");

  ClearListLabel = EnsureGetMember<Sprite>("ClearListLabel");
  EndingsLabelPosition = EnsureGetMember<glm::vec2>("EndingsLabelPosition");
  EndingsLabel = EnsureGetMember<Sprite>("EndingsLabel");
  EndingCountPosition = EnsureGetMember<glm::vec2>("EndingCountPosition");
  ScenesLabelPosition = EnsureGetMember<glm::vec2>("ScenesLabelPosition");
  ScenesLabel = EnsureGetMember<Sprite>("ScenesLabel");
  SceneCountPosition = EnsureGetMember<glm::vec2>("SceneCountPosition");
  CompletionLabelPosition =
      EnsureGetMember<glm::vec2>("CompletionLabelPosition");
  CompletionLabel = EnsureGetMember<Sprite>("CompletionLabel");
  CompletionPosition = EnsureGetMember<glm::vec2>("CompletionPosition");
  AlbumLabelPosition = EnsureGetMember<glm::vec2>("AlbumLabelPosition");
  AlbumLabel = EnsureGetMember<Sprite>("AlbumLabel");
  AlbumCountPosition = EnsureGetMember<glm::vec2>("AlbumCountPosition");
  PlayTimeLabelPosition = EnsureGetMember<glm::vec2>("PlayTimeLabelPosition");
  PlayTimeLabel = EnsureGetMember<Sprite>("PlayTimeLabel");
  PlayTimeTextTable = EnsureGetMember<int>("PlayTimeTextTable");
  PlayTimeSecondsTextEntry = EnsureGetMember<int>("PlayTimeSecondsTextEntry");
  PlayTimeMinutesTextEntry = EnsureGetMember<int>("PlayTimeMinutesTextEntry");
  PlayTimeHoursTextEntry = EnsureGetMember<int>("PlayTimeHoursTextEntry");
  PlayTimeSecondsTextPosition =
      EnsureGetMember<glm::vec2>("PlayTimeSecondsTextPosition");
  PlayTimeMinutesTextPosition =
      EnsureGetMember<glm::vec2>("PlayTimeMinutesTextPosition");
  PlayTimeHoursTextPosition =
      EnsureGetMember<glm::vec2>("PlayTimeHoursTextPosition");
  PlayTimeSecondsPosition =
      EnsureGetMember<glm::vec2>("PlayTimeSecondsPosition");
  PlayTimeMinutesPosition =
      EnsureGetMember<glm::vec2>("PlayTimeMinutesPosition");
  PlayTimeHoursPosition = EnsureGetMember<glm::vec2>("PlayTimeHoursPosition");
  ClearListColorIndex = EnsureGetMember<int>("ClearListColorIndex");
  ClearListTextBackground = EnsureGetMember<Sprite>("ClearListTextBackground");
  ClearListTextBGOffset = EnsureGetMember<glm::vec2>("ClearListTextBGOffset");

  EndingListLabel = EnsureGetMember<Sprite>("EndingListLabel");
  EndingCount = EnsureGetMember<int>("EndingCount");
  EndingsListNumberInitialPosition =
      EnsureGetMember<glm::vec2>("EndingsListNumberInitialPosition");
  EndingsListTextInitialPosition =
      EnsureGetMember<glm::vec2>("EndingsListTextInitialPosition");
  EndingsListTextMargin = EnsureGetMember<glm::vec2>("EndingsListTextMargin");
  EndingsListTextFontSize = EnsureGetMember<float>("EndingsListTextFontSize");
  EndingsListTextLockedTable =
      EnsureGetMember<int>("EndingsListTextLockedTable");
  EndingsListTextLockedEntry =
      EnsureGetMember<int>("EndingsListTextLockedEntry");
  EndingsListTextTable = EnsureGetMember<int>("EndingsListTextTable");
  EndingsListTextColorIndex = EnsureGetMember<int>("EndingsListTextColorIndex");

  SceneTitleLabel = EnsureGetMember<Sprite>("SceneTitleLabel");
  SceneCount = EnsureGetMember<int>("SceneCount");
  SceneListNumberInitialPosition =
      EnsureGetMember<glm::vec2>("SceneListNumberInitialPosition");
  SceneListTextInitialPosition =
      EnsureGetMember<glm::vec2>("SceneListTextInitialPosition");
  SceneListTextMargin = EnsureGetMember<glm::vec2>("SceneListTextMargin");
  SceneListFontSize = EnsureGetMember<float>("SceneListFontSize");
  SceneListTextTable = EnsureGetMember<int>("SceneListTextTable");
  SceneTitleItemsRenderingBounds =
      EnsureGetMember<RectF>("SceneTitleItemsRenderingBounds");
  SceneListColorIndex = EnsureGetMember<int>("SceneListColorIndex");
  SceneTitleLockedTable = EnsureGetMember<int>("SceneTitleLockedTable");
  SceneTitleLockedEntry = EnsureGetMember<int>("SceneTitleLockedEntry");
  SceneTitleItemsWidth = EnsureGetMember<float>("SceneTitleItemsWidth");
  ScrollbarStart = EnsureGetMember<float>("ScrollbarStart");
  ScrollAreaHeight = EnsureGetMember<float>("ScrollAreaHeight");
  ScrollbarTrack = EnsureGetMember<Sprite>("ScrollbarTrackSprite");
  ScrollbarThumb = EnsureGetMember<Sprite>("ScrollbarThumbSprite");
  ScrollbarPosition = EnsureGetMember<glm::vec2>("ScrollbarPosition");

  ArrowLeft = EnsureGetMember<Sprite>("ArrowLeft");
  ArrowLeftPosition = EnsureGetMember<glm::vec2>("ArrowLeftPosition");
  ArrowRight = EnsureGetMember<Sprite>("ArrowRight");
  ArrowRightPosition = EnsureGetMember<glm::vec2>("ArrowRightPosition");

  ArrowsAnimationDuration = EnsureGetMember<float>("ArrowsAnimationDuration");
  FadeInDuration = EnsureGetMember<float>("FadeInDuration");
  FadeOutDuration = EnsureGetMember<float>("FadeOutDuration");

  auto drawType = Game::DrawComponentType::_from_integral_unchecked(
      EnsureGetMember<uint8_t>("DrawType"));

  auto clearList = new UI::MO6TW::ClearListMenu();
  UI::Menus[drawType].push_back(clearList);
}

}  // namespace ClearListMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto