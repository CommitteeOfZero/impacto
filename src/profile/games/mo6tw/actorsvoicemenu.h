#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace ActorsVoiceMenu {

int constexpr ActorsVoiceCount = 8;

void Configure();

inline Sprite BackgroundSprite;

inline Sprite UnlockedSprites[ActorsVoiceCount];
inline Sprite LockedSprites[ActorsVoiceCount];
inline Sprite UnlockedHighlightedSprites[ActorsVoiceCount];
inline Sprite LockedHighlightedSprites[ActorsVoiceCount];
inline glm::vec2 InitialItemPosition;
inline glm::vec2 ItemOffset;
inline int CharacterBackgroundBufferId;

inline float FadeInDuration;
inline float FadeOutDuration;

}  // namespace ActorsVoiceMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto