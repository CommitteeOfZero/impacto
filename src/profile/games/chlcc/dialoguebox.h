#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace DialogueBox {

void Configure();
void ConfigureNametag();

inline Sprite SecondaryADVBoxSprite;
inline Sprite SecondaryNametagSprite;

inline Sprite ErinBoxSprite;
inline glm::vec2 ErinBoxPos;
inline glm::vec2 REVWaitIconOffset;
inline uint8_t REVLineSpacing;
inline uint8_t REVLineHeight;
inline uint8_t REVFontSize;

}  // namespace DialogueBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto