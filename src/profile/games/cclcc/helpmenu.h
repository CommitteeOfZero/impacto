#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace HelpMenu {

inline std::vector<Sprite> ManualPages;
inline Sprite HelpMaskSprite;

inline float FadeInDuration;
inline float FadeOutDuration;
inline float NextPageInDuration;
inline float NextPageOutDuration;

void Configure();

}  // namespace HelpMenu
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto