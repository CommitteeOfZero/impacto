#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace MO6TW {
namespace AlbumMenu {

void Configure();

int const CharacterButtonCount = 8;
int const CharacterPortraitCount = 5;

extern Sprite BackgroundSprite;

extern Sprite CharacterButtonSprites[CharacterButtonCount];
extern Sprite HighlightedCharacterButtonSprites[CharacterButtonCount];
extern glm::vec2 InitialButtonPosition;
extern float ButtonOddX;
extern float ButtonEvenX;
extern glm::vec2 ButtonMargin;
extern float HighlightAnimationDuration;
extern int YunoButtonIdx;
extern int SuzuButtonIdx;
extern Sprite CharacterPortraits[CharacterPortraitCount];
extern glm::vec2 PortraitPosition;

extern float FadeInDuration;
extern float FadeOutDuration;

}  // namespace AlbumMenu
}  // namespace MO6TW
}  // namespace Profile
}  // namespace Impacto