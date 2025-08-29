#include "dialoguebox.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace DialogueBox {

void Configure() {
  SecondaryADVBoxSprite = EnsureGetMember<Sprite>("SecondaryADVBoxSprite");

  HaveSecondaryADVNameTag = TryPushMember("SecondaryADVNameTag");
  if (HaveSecondaryADVNameTag) {
    AssertIs(LUA_TTABLE);

    SecondaryADVNameTag::Position =
        EnsureGetMember<glm::vec2>("SecondaryPosition");
    SecondaryADVNameTag::LeftSprite =
        EnsureGetMember<Sprite>("SecondaryLeftSprite");
    SecondaryADVNameTag::LineSprite =
        EnsureGetMember<Sprite>("SecondaryLineSprite");
    SecondaryADVNameTag::RightSprite =
        EnsureGetMember<Sprite>("SecondaryRightSprite");
    SecondaryADVNameTag::BaseLineWidth =
        EnsureGetMember<float>("SecondaryBaseLineWidth");

    Pop();
  }

  ErinBoxSprite = EnsureGetMember<Sprite>("ErinBoxSprite");
  ErinBoxPos = EnsureGetMember<glm::vec2>("ErinBoxPos");
  REVWaitIconOffset = EnsureGetMember<glm::vec2>("REVWaitIconOffset");
  REVLineHeight = EnsureGetMember<uint8_t>("REVLineHeight");
  REVLineSpacing = EnsureGetMember<uint8_t>("REVLineSpacing");
  REVFontSize = EnsureGetMember<uint8_t>("REVFontSize");
}

}  // namespace DialogueBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto