#pragma once

#include "../../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace DialogueBox {

void Configure();

extern Sprite SecondaryADVBoxSprite;

extern bool HaveSecondaryADVNameTag;
namespace SecondaryADVNameTag {
  extern glm::vec2 Position;
  extern Sprite LeftSprite;
  extern Sprite LineSprite;
  extern Sprite RightSprite;
  extern float BaseLineWidth;
}  // namespace SecondaryADVNameTag

extern Sprite REVBoxSprite;
extern glm::vec2 REVBoxPos;
extern glm::vec2 REVWaitIconOffset;
extern uint8_t REVLineSpacing;
extern uint8_t REVLineHeight;
extern uint8_t REVFontSize;

}  // namespace DialogueBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto