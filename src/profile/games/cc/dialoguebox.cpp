#include "dialoguebox.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace DialogueBox {

void Configure() {
  ADVBoxMask = EnsureGetMemberSprite("ADVBoxMask");
  ADVBoxEffectDuration = EnsureGetMemberFloat("ADVBoxEffectDuration");
  const int NamePlateCount = EnsureGetMemberInt("ADVBoxNamePlateCount");
  GetMemberSpriteArray(NamePlateMainSprites, NamePlateCount,
                       "ADVBoxNamePlateMainSprites");
  GetMemberSpriteArray(NamePlateLabelSprites, NamePlateCount,
                       "ADVBoxNamePlateLabelSprites");

  ADVBoxNamePlateMainPos = EnsureGetMemberVec2("ADVBoxNamePlateMainPos");
  ADVBoxNamePlateLabelPos = EnsureGetMemberVec2("ADVBoxNamePlateLabelPos");

  SkipAnimation = EnsureGetMemberAnimation("SkipAnimation");
  AutoAnimation = EnsureGetMemberAnimation("AutoAnimation");
  SkipIconPos = EnsureGetMemberVec2("SkipIconPos");
  AutoIconPos = EnsureGetMemberVec2("AutoIconPos");
  SkipAutoAnimationDuration = EnsureGetMemberFloat("SkipAutoAnimationDuration");
}

}  // namespace DialogueBox
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto