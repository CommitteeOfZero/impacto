#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <string>

namespace Impacto {
namespace Profile {
namespace AchievementNotification {

inline std::string BackgroundPath;
inline std::string FontPath;
inline float Scale;
inline float DisplayDuration;
inline float FadeDuration;
inline float IconSize;
inline glm::vec2 IconOffset;
inline float TextGap;
inline float TextRightPadding;
inline float TitleFontSize;
inline float DescriptionFontSize;
inline float TextLineGap;
inline uint32_t TextColor;
inline uint32_t OutlineColor;

void Configure();

}  // namespace AchievementNotification
}  // namespace Profile
}  // namespace Impacto
