#pragma once

#include "../../spritesheet.h"
#include "../../text/fonts/font.h"
#include "../../text/processedtextglyph.h"

namespace Impacto::Profile::Overlay {
namespace ToastNotification {

inline Sprite NotificationBackgroundSprite;
inline std::optional<Fonts::ExternalFont> NotificationFont;

inline float Scale;

inline float DisplayDuration;
inline float FadeDuration;

inline float TextPadding;
inline float TextSize;
inline float TextLineGap;

inline DialogueColorPair TextColors;

namespace AchievementNotification {

inline float IconSize;
inline glm::vec2 IconOffset;
inline float TextGap;

inline float TitleFontSize;
inline float DescriptionFontSize;

}  // namespace AchievementNotification
}  // namespace ToastNotification

void Configure();

}  // namespace Impacto::Profile::Overlay
