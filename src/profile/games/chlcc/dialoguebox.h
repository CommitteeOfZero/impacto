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

}  // namespace DialogueBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto