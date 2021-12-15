#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace ActorsVoiceMenu {

int const ActorsVoiceCount = 8;

void Configure();

extern Sprite BackgroundSprite;

extern Sprite UnlockedSprites[ActorsVoiceCount];
extern Sprite LockedSprites[ActorsVoiceCount];
extern Sprite UnlockedHighlightedSprites[ActorsVoiceCount];
extern Sprite LockedHighlightedSprites[ActorsVoiceCount];
extern glm::vec2 InitialItemPosition;
extern glm::vec2 ItemOffset;
extern int CharacterBackgroundBufferId;

extern float FadeInDuration;
extern float FadeOutDuration;

}  // namespace ActorsVoiceMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto