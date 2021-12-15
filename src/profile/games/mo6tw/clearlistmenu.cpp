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

Sprite BackgroundSprite;
Sprite WindowSprite;
glm::vec2 WindowPosition;
Sprite WindowSpritePartLeft;
Sprite WindowSpritePartRight;
int FontSize;
int SeparatorTable;
int SeparatorEntry;

glm::vec2 LabelPosition;
Sprite ClearListLabel;
glm::vec2 EndingsLabelPosition;
Sprite EndingsLabel;
glm::vec2 EndingCountPosition;
glm::vec2 ScenesLabelPosition;
Sprite ScenesLabel;
glm::vec2 SceneCountPosition;
glm::vec2 CompletionLabelPosition;
Sprite CompletionLabel;
glm::vec2 CompletionPosition;
glm::vec2 AlbumLabelPosition;
Sprite AlbumLabel;
glm::vec2 AlbumCountPosition;
glm::vec2 PlayTimeLabelPosition;
Sprite PlayTimeLabel;
int PlayTimeTextTable;
int PlayTimeSecondsTextEntry;
int PlayTimeMinutesTextEntry;
int PlayTimeHoursTextEntry;
glm::vec2 PlayTimeSecondsTextPosition;
glm::vec2 PlayTimeMinutesTextPosition;
glm::vec2 PlayTimeHoursTextPosition;
glm::vec2 PlayTimeSecondsPosition;
glm::vec2 PlayTimeMinutesPosition;
glm::vec2 PlayTimeHoursPosition;
int ClearListColorIndex;
Sprite ClearListTextBackground;
glm::vec2 ClearListTextBGOffset;

Sprite EndingListLabel;
int EndingCount;
glm::vec2 EndingsListNumberInitialPosition;
glm::vec2 EndingsListTextInitialPosition;
glm::vec2 EndingsListTextMargin;
int EndingsListTextFontSize;
int EndingsListTextLockedTable;
int EndingsListTextLockedEntry;
int EndingsListTextTable;
int EndingsListTextColorIndex;

Sprite SceneTitleLabel;
int SceneCount;
glm::vec2 SceneListNumberInitialPosition;
glm::vec2 SceneListTextInitialPosition;
glm::vec2 SceneListTextMargin;
int SceneListFontSize;
int SceneListTextTable;
RectF SceneTitleItemsRenderingBounds;
int SceneListColorIndex;
int SceneTitleLockedTable;
int SceneTitleLockedEntry;
float SceneTitleItemsWidth;
float ScrollbarStart;
float ScrollAreaHeight;
Sprite ScrollbarTrack;
Sprite ScrollbarThumb;
glm::vec2 ScrollbarPosition;

Sprite ArrowLeft;
glm::vec2 ArrowLeftPosition;
Sprite ArrowRight;
glm::vec2 ArrowRightPosition;

float ArrowsAnimationDuration;
float FadeInDuration;
float FadeOutDuration;

void Configure() {
  BackgroundSprite = EnsureGetMemberSprite("BackgroundSprite");
  WindowSprite = EnsureGetMemberSprite("WindowSprite");
  WindowPosition = EnsureGetMemberVec2("WindowPosition");
  WindowSpritePartLeft = EnsureGetMemberSprite("WindowSpritePartLeft");
  WindowSpritePartRight = EnsureGetMemberSprite("WindowSpritePartRight");
  FontSize = EnsureGetMemberInt("FontSize");
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
  EndingsListTextFontSize = EnsureGetMemberInt("EndingsListTextFontSize");
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
  SceneListFontSize = EnsureGetMemberInt("SceneListFontSize");
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