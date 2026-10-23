#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace SG0 {
namespace Phone {

void Configure();

// Phone body (phone_b.dds), drawn at Scale; positions are design pixels.
inline Sprite BodySprite;
inline float Scale;
inline glm::vec2 ShownPos;
inline float HiddenX;
inline glm::vec2 ScreenOffset;  // screen rectangle relative to the body
inline glm::vec2 ScreenSize;

inline Sprite WallpaperSprite;
inline float ScreenShadeAlpha;
inline Sprite SelectionSprite;

// Text inside the screen, relative to the screen's top left.
inline glm::vec2 Padding;
inline float ItemsTop;
inline float RowHeight;
inline float RowPadding;
inline float TitleFontSize;
inline float ItemFontSize;
inline uint32_t TextColor;
inline uint32_t OutlineColor;

// RINE screen (phone_rine.dds avatars and stickers), relative to the screen.
inline Sprite RineAvatarSprite;  // avatar 0; avatar n is RineAvatarStepY lower
inline float RineAvatarStepY;
inline Sprite RineStickerSprite;  // sticker 0 of a RineStickerColumns-wide grid
inline glm::vec2 RineStickerStep;
inline int RineStickerColumns;
inline float RineTop;  // message area top (below the title)
inline float RineMargin;
inline float RineGap;
inline float RineAvatarSize;
inline glm::vec2 RineStickerSize;
inline float RineFontSize;
inline float RineLineHeight;
inline float RineBubbleMaxWidth;
inline glm::vec2 RineBubblePadding;
inline uint32_t RineOwnColor;
inline uint32_t RineOtherColor;
inline uint32_t RineTextColor;
inline float RineReplyRowHeight;
inline glm::vec2 RineReplyStickerSize;
inline float RineInputHeight;

}  // namespace Phone
}  // namespace SG0
}  // namespace Profile
}  // namespace Impacto
