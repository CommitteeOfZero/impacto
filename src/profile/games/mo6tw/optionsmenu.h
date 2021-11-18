#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace OptionsMenu {

int const VoiceToggleCount = 14;

extern Sprite VoiceToggleEnabledSprites[VoiceToggleCount];
extern Sprite VoiceToggleDisabledSprites[VoiceToggleCount];
extern Sprite VoiceToggleHighlightSprite;

extern glm::vec2 VoiceToggleStart;
extern glm::vec2 VoiceTogglePadding;
extern int VoiceTogglePerLine;

void Configure();

}  // namespace OptionsMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto