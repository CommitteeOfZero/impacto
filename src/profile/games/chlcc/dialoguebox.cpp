#include "dialoguebox.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace DialogueBox {

Sprite SecondaryADVBoxSprite;

bool HaveSecondaryADVNameTag;
namespace SecondaryADVNameTag {
glm::vec2 Position;
Sprite LeftSprite;
Sprite LineSprite;
Sprite RightSprite;
float LineFactor;
float BaseLineWidth;
}  // namespace SecondaryADVNameTag

Sprite REVBoxSprite;
glm::vec2 REVBoxPos;
glm::vec2 REVWaitIconOffset;
uint8_t REVLineHeight;
uint8_t REVLineSpacing;
uint8_t REVFontSize;

void Configure() {
  SecondaryADVBoxSprite = EnsureGetMemberSprite("SecondaryADVBoxSprite");

  HaveSecondaryADVNameTag = TryPushMember("SecondaryADVNameTag");
  if (HaveSecondaryADVNameTag) {
    AssertIs(LUA_TTABLE);

    SecondaryADVNameTag::Position = EnsureGetMemberVec2("SecondaryPosition");
    SecondaryADVNameTag::LeftSprite = EnsureGetMemberSprite("SecondaryLeftSprite");
    SecondaryADVNameTag::LineSprite = EnsureGetMemberSprite("SecondaryLineSprite");
    SecondaryADVNameTag::RightSprite = EnsureGetMemberSprite("SecondaryRightSprite");
    SecondaryADVNameTag::BaseLineWidth = EnsureGetMemberFloat("SecondaryBaseLineWidth");

    Pop();
  }

  REVBoxSprite = EnsureGetMemberSprite("REVBoxSprite");
  REVBoxPos = EnsureGetMemberVec2("REVBoxPos");
  REVWaitIconOffset = EnsureGetMemberVec2("REVWaitIconOffset");
  REVLineHeight = EnsureGetMemberInt("REVLineHeight");
  REVLineSpacing = EnsureGetMemberInt("REVLineSpacing");
  REVFontSize = EnsureGetMemberInt("REVFontSize");
}

}  // namespace DialogueBox
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto