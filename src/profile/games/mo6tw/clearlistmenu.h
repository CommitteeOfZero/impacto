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

extern glm::vec2 LabelPosition;
extern Sprite ClearListLabel;
extern glm::vec2 EndingsLabelPosition;
extern Sprite EndingsLabel;
extern glm::vec2 ScenesLabelPosition;
extern Sprite ScenesLabel;
extern glm::vec2 CompletionLabelPosition;
extern Sprite CompletionLabel;
extern glm::vec2 AlbumLabelPosition;
extern Sprite AlbumLabel;
extern glm::vec2 PlayTimeLabelPosition;
extern Sprite PlayTimeLabel;
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
extern Sprite ScrollbarTrack;
extern Sprite ScrollbarThumb;
extern glm::vec2 ScrollbarPosition;

extern float ArrowsAnimationDuration;
extern float FadeInDuration;
extern float FadeOutDuration;

void Configure();

}  // namespace ClearListMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto