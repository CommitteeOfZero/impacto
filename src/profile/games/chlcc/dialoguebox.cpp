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

  REVBoxSprite = EnsureGetMember<Sprite>("REVBoxSprite");
  REVBoxPos = EnsureGetMember<glm::vec2>("REVBoxPos");
  REVWaitIconOffset = EnsureGetMember<glm::vec2>("REVWaitIconOffset");
  REVLineHeight = EnsureGetMember<int>("REVLineHeight");
  REVLineSpacing = EnsureGetMember<int>("REVLineSpacing");
  REVFontSize = EnsureGetMember<int>("REVFontSize");
}

}  // namespace DialogueBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto