#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace ClearListMenu {

extern Sprite BackgroundSprite;
extern Sprite WindowSprite;
extern glm::vec2 WindowPosition;
extern Sprite WindowSpritePartLeft;
extern Sprite WindowSpritePartRight;
extern int FontSize;
extern int SeparatorTable;
extern int SeparatorEntry;

extern glm::vec2 LabelPosition;
extern Sprite ClearListLabel;
extern glm::vec2 EndingsLabelPosition;
extern Sprite EndingsLabel;
extern glm::vec2 EndingCountPosition;
extern glm::vec2 ScenesLabelPosition;
extern Sprite ScenesLabel;
extern glm::vec2 SceneCountPosition;
extern glm::vec2 CompletionLabelPosition;
extern Sprite CompletionLabel;
extern glm::vec2 CompletionPosition;
extern glm::vec2 AlbumLabelPosition;
extern Sprite AlbumLabel;
extern glm::vec2 AlbumCountPosition;
extern glm::vec2 PlayTimeLabelPosition;
extern Sprite PlayTimeLabel;
extern int PlayTimeTextTable;
extern int PlayTimeSecondsTextEntry;
extern int PlayTimeMinutesTextEntry;
extern int PlayTimeHoursTextEntry;
extern glm::vec2 PlayTimeSecondsTextPosition;
extern glm::vec2 PlayTimeMinutesTextPosition;
extern glm::vec2 PlayTimeHoursTextPosition;
extern glm::vec2 PlayTimeSecondsPosition;
extern glm::vec2 PlayTimeMinutesPosition;
extern glm::vec2 PlayTimeHoursPosition;
extern int ClearListColorIndex;
extern Sprite ClearListTextBackground;
extern glm::vec2 ClearListTextBGOffset;

extern Sprite EndingListLabel;
extern int EndingCount;
extern glm::vec2 EndingsListNumberInitialPosition;
extern glm::vec2 EndingsListTextInitialPosition;
extern glm::vec2 EndingsListTextMargin;
extern int EndingsListTextFontSize;
extern int EndingsListTextLockedTable;
extern int EndingsListTextLockedEntry;
extern int EndingsListTextTable;
extern int EndingsListTextColorIndex;

extern Sprite SceneTitleLabel;
extern int SceneCount;
extern glm::vec2 SceneListNumberInitialPosition;
extern glm::vec2 SceneListTextInitialPosition;
extern glm::vec2 SceneListTextMargin;
extern int SceneListFontSize;
extern int SceneListTextTable;
extern RectF SceneTitleItemsRenderingBounds;
extern int SceneListColorIndex;
extern int SceneTitleLockedTable;
extern int SceneTitleLockedEntry;
extern float SceneTitleItemsWidth;
extern float ScrollbarStart;
extern float ScrollAreaHeight;
extern Sprite ScrollbarTrack;
extern Sprite ScrollbarThumb;
extern glm::vec2 ScrollbarPosition;

extern Sprite ArrowLeft;
extern glm::vec2 ArrowLeftPosition;
extern Sprite ArrowRight;
extern glm::vec2 ArrowRightPosition;

extern float ArrowsAnimationDuration;
extern float FadeInDuration;
extern float FadeOutDuration;

void Configure();

}  // namespace ClearListMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto