#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace ClearListMenu {

inline Sprite BackgroundSprite;
inline Sprite WindowSprite;
inline glm::vec2 WindowPosition;
inline Sprite WindowSpritePartLeft;
inline Sprite WindowSpritePartRight;
inline float FontSize;
inline int SeparatorTable;
inline int SeparatorEntry;

inline glm::vec2 LabelPosition;
inline Sprite ClearListLabel;
inline glm::vec2 EndingsLabelPosition;
inline Sprite EndingsLabel;
inline glm::vec2 EndingCountPosition;
inline glm::vec2 ScenesLabelPosition;
inline Sprite ScenesLabel;
inline glm::vec2 SceneCountPosition;
inline glm::vec2 CompletionLabelPosition;
inline Sprite CompletionLabel;
inline glm::vec2 CompletionPosition;
inline glm::vec2 AlbumLabelPosition;
inline Sprite AlbumLabel;
inline glm::vec2 AlbumCountPosition;
inline glm::vec2 PlayTimeLabelPosition;
inline Sprite PlayTimeLabel;
inline int PlayTimeTextTable;
inline int PlayTimeSecondsTextEntry;
inline int PlayTimeMinutesTextEntry;
inline int PlayTimeHoursTextEntry;
inline glm::vec2 PlayTimeSecondsTextPosition;
inline glm::vec2 PlayTimeMinutesTextPosition;
inline glm::vec2 PlayTimeHoursTextPosition;
inline glm::vec2 PlayTimeSecondsPosition;
inline glm::vec2 PlayTimeMinutesPosition;
inline glm::vec2 PlayTimeHoursPosition;
inline int ClearListColorIndex;
inline Sprite ClearListTextBackground;
inline glm::vec2 ClearListTextBGOffset;

inline Sprite EndingListLabel;
inline int EndingCount;
inline glm::vec2 EndingsListNumberInitialPosition;
inline glm::vec2 EndingsListTextInitialPosition;
inline glm::vec2 EndingsListTextMargin;
inline float EndingsListTextFontSize;
inline int EndingsListTextLockedTable;
inline int EndingsListTextLockedEntry;
inline int EndingsListTextTable;
inline int EndingsListTextColorIndex;

inline Sprite SceneTitleLabel;
inline int SceneCount;
inline glm::vec2 SceneListNumberInitialPosition;
inline glm::vec2 SceneListTextInitialPosition;
inline glm::vec2 SceneListTextMargin;
inline float SceneListFontSize;
inline int SceneListTextTable;
inline RectF SceneTitleItemsRenderingBounds;
inline int SceneListColorIndex;
inline int SceneTitleLockedTable;
inline int SceneTitleLockedEntry;
inline float SceneTitleItemsWidth;
inline float ScrollbarStart;
inline float ScrollAreaHeight;
inline Sprite ScrollbarTrack;
inline Sprite ScrollbarThumb;
inline glm::vec2 ScrollbarPosition;

inline Sprite ArrowLeft;
inline glm::vec2 ArrowLeftPosition;
inline Sprite ArrowRight;
inline glm::vec2 ArrowRightPosition;

inline float ArrowsAnimationDuration;
inline float FadeInDuration;
inline float FadeOutDuration;

void Configure();

}  // namespace ClearListMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto