#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace SaveMenu {

int const Pages = 8;
int const EntriesPerPage = 6;

extern uint32_t BackgroundColor;
extern uint32_t SaveBackgroundColor;
extern uint32_t LoadBackgroundColor;
extern uint32_t QuickLoadBackgroundColor;
extern Sprite CircleSprite;
extern Sprite SaveCircle;
extern Sprite LoadCircle;
extern Sprite QuickLoadCircle;
extern glm::vec2 CircleStartPosition;
extern float CircleOffset;
extern glm::vec2 ErinPosition;
extern Sprite ErinSprite;
extern Sprite BackgroundFilter;
extern glm::vec2 InitialRedBarPosition;
extern Sprite InitialRedBarSprite;
extern glm::vec2 RightRedBarPosition;
extern glm::vec2 RedBarPosition;
extern float RedBarDivision;
extern float RedBarBaseX;
extern Sprite RedBarSprite;
extern glm::vec2 RedBarLabelPosition;
extern Sprite RedBarLabel;
extern Sprite MenuTitleTextSprite;
extern Sprite QuickLoadTextSprite;
extern Sprite LoadTextSprite;
extern Sprite SaveTextSprite;
extern glm::vec2 MenuTitleTextLeftPos;
extern glm::vec2 MenuTitleTextRightPos;
extern float MenuTitleTextAngle;
extern float TitleFadeInDuration;
extern float TitleFadeOutDuration;

extern glm::vec2 SaveListPosition;
extern Sprite SaveListSprite;
extern glm::vec2 EntryPositions[EntriesPerPage];
extern Sprite QuickLoadEntrySprite;
extern Sprite SaveEntrySprite;
extern Sprite LoadEntrySprite;
extern Sprite EntryHighlightedSprite;
extern Sprite LockedSymbolSprite;
extern glm::vec2 ThumbnailRelativePos;
extern glm::vec2 PageNumBackgroundPos;
extern Sprite PageNumBackgroundSprite;
extern glm::vec2 CurrentPageNumPos;
extern Sprite BigDigits[10];
extern glm::vec2 PageNumSeparatorSlashPos;
extern Sprite PageNumSeparatorSlashSprite;
extern glm::vec2 MaxPageNumPos;
extern Sprite MaxPageNumSprite;
extern glm::vec2 ButtonPromptPosition;
extern Sprite ButtonPromptSprite;
extern float SelectDataFadeDuration;
// 10 letters in "SELECT DATA"
extern glm::vec2 SelectDataTextPositions[10];
extern Sprite SelectDataTextSprites[10];

extern glm::vec2 EntryNumberHintTextRelativePos;
extern glm::vec2 EntryNumberTextRelativePos;
extern glm::vec2 SceneTitleTextRelativePos;
extern glm::vec2 NoDataTextRelativePos;
extern glm::vec2 PlayTimeHintTextRelativePos;
extern glm::vec2 PlayTimeTextRelativePos;
extern glm::vec2 SaveDateHintTextRelativePos;
extern glm::vec2 SaveDateTextRelativePos;

void Configure();

}  // namespace SaveMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto