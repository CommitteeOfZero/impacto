#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TipsMenu {

int constexpr inline MaxCategoryString = 5;

inline float MenuTransitionDuration;
inline uint32_t BackgroundColor;
inline Sprite CircleSprite;
inline glm::vec2 CircleStartPosition;
inline float CircleOffset;
inline glm::vec2 ErinPosition;
inline Sprite ErinSprite;
inline Sprite BackgroundFilter;
inline glm::vec2 InitialRedBarPosition;
inline Sprite InitialRedBarSprite;
inline glm::vec2 RightRedBarPosition;
inline glm::vec2 RedBarPosition;
inline float RedBarDivision;
inline float RedBarBaseX;
inline Sprite RedBarSprite;
inline glm::vec2 RedBarLabelPosition;
inline Sprite RedBarLabel;
inline glm::vec2 MenuTitleTextRightPosition;
inline glm::vec2 MenuTitleTextLeftPosition;
inline float MenuTitleTextAngle;
inline float TitleFadeInDuration;
inline float TitleFadeOutDuration;
inline Sprite MenuTitleText;
inline glm::vec2 TreePosition;
inline Sprite TipsTree;
inline glm::vec2 GradientPosition;
inline Sprite TipsGradient;
inline uint32_t EndOfGradientColor;
inline glm::vec2 CurrentTipBackgroundPosition;
inline Sprite CurrentTipBackgroundSprite;
inline glm::vec2 ButtonPromptPosition;
inline Sprite ButtonPromptSprite;
inline int TipsStringTable;
inline int CategoryStringIndex;
inline int SortStringIndex;
inline int PageSeparatorIndex;
inline int LockedTipsIndex;
inline int NumberLabelStrIndex;
inline int NewLabelStrIndex;
inline int UnreadLabelStrIndex;
inline uint16_t CategoryString[MaxCategoryString];
inline glm::vec2 NumberLabelPosition;
inline float NumberLabelFontSize;
inline RectF NumberBounds;
inline float NumberFontSize;
inline int DefaultColorIndex;
inline int UnreadColorIndex;
inline RectF TipListEntryBounds;
inline float TipListEntryFontSize;
inline float TipListYPadding;
inline float PronounciationFontSize;
inline float NameFontSize;
inline float TipListEntryNameXOffset;
inline float TipListEntryTextOffsetX;
inline glm::vec2 TipsListEntryDotOffset;
inline glm::vec2 TipsListNewDotOffset;
inline RectF NameInitialBounds;
inline RectF PronounciationInitialBounds;
inline RectF TipsListBounds;
inline RectF TipsListRenderBounds;
inline Sprite TipsEntryHighlightBar;
inline Sprite TipsEntryHighlightDot;
inline Sprite TipsEntryNewDot;

inline Sprite TipsLeftLine;
inline Sprite TipsLeftLineHole;
inline Sprite TipsLeftLineEnd;
inline Sprite TipsLeftLineHoleEnd;
inline Sprite TipsListBgBar;
inline Sprite TipsListBgBarHole;
inline Sprite TipsScrollThumb;
inline Sprite TipsScrollTrack;

inline glm::vec2 CurrentPagePosition;
inline glm::vec2 TotalPagesPosition;
inline glm::vec2 PageSeparatorPosition;
inline std::vector<Sprite> CurrentPageSprites;
inline std::vector<Sprite> TotalPageSprites;
inline Sprite PageSeparatorSprite;

inline std::vector<Sprite> SelectWordSprites;
inline std::vector<glm::vec2> SelectWordPos;
inline float SelectWordDuration;
inline float SelectWordInterval;

void Configure();

}  // namespace TipsMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto