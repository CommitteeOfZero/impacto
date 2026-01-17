#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace AlbumMenu {

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
inline float TitleFadeInDuration;
inline float TitleFadeOutDuration;
inline Sprite CGList;
inline glm::vec2 CGListPosition;
inline Sprite PageCountLabel;
inline glm::vec2 PageLabelPosition;
inline Sprite CGBox;
inline glm::vec2 CGBoxTemplatePosition;
inline Sprite AlbumThumbnails[63];
inline glm::vec2 ThumbnailTemplatePosition;
inline Sprite VariationUnlocked;
inline Sprite VariationLocked;
inline glm::vec2 VariationTemplateOffset;
inline Sprite LockedCG;
inline glm::vec2 ThumbnailOffset;
inline Sprite ThumbnailHighlight;
inline int32_t AlbumPages;
inline int32_t EntriesPerPage;
inline Sprite PageNums[10];
inline glm::vec2 CurrentPageNumPos;
inline glm::vec2 MaxPageNumPos;
inline Sprite PageNumSeparatorSlash;
inline glm::vec2 PageNumSeparatorSlashPos;
inline Sprite ReachablePageNums[10];
inline Sprite ButtonGuide;
inline glm::vec2 ButtonGuidePos;
inline std::vector<Sprite> SelectDataSprites;
inline std::vector<glm::vec2> SelectDataPos;
inline Sprite AlbumMenuTitle;
inline glm::vec2 AlbumMenuTitleRightPos;
inline glm::vec2 AlbumMenuTitleLeftPos;
inline float AlbumMenuTitleAngle;
inline Sprite CgViewerButtonGuideVariation;
inline Sprite CgViewerButtonGuideNoVariation;
inline glm::vec2 CgViewerButtonGuidePos;
inline Sprite SelectionMarkerSprite;
inline glm::vec2 SelectionMarkerRelativePos;

void Configure();

}  // namespace AlbumMenu
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto