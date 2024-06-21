#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace DialogueBox {

void Configure();

inline Sprite SecondaryADVBoxSprite;

inline bool HaveSecondaryADVNameTag;
namespace SecondaryADVNameTag {
inline glm::vec2 Position;
inline Sprite LeftSprite;
inline Sprite LineSprite;
inline Sprite RightSprite;
inline float BaseLineWidth;
}  // namespace SecondaryADVNameTag

inline Sprite REVBoxSprite;
inline glm::vec2 REVBoxPos;
inline glm::vec2 REVWaitIconOffset;
inline uint8_t REVLineSpacing;
inline uint8_t REVLineHeight;
inline uint8_t REVFontSize;

}  // namespace DialogueBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto