#include "dialoguebox.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace DialogueBox {

Sprite ADVBoxMask;
Sprite NamePlateMainSprites[NamePlateCountMax];
Sprite NamePlateLabelSprites[NamePlateCountMax];

glm::vec2 ADVBoxNamePlateMainPos;
glm::vec2 ADVBoxNamePlateLabelPos;

int NamePlateCount;

float ADVBoxEffectDuration;

void Configure() {
  ADVBoxMask = EnsureGetMemberSprite("ADVBoxMask");
  ADVBoxEffectDuration = EnsureGetMemberFloat("ADVBoxEffectDuration");
  NamePlateCount = EnsureGetMemberInt("ADVBoxNamePlateCount");
  GetMemberSpriteArray(NamePlateMainSprites, NamePlateCount,
                       "ADVBoxNamePlateMainSprites");
  GetMemberSpriteArray(NamePlateLabelSprites, NamePlateCount,
                       "ADVBoxNamePlateLabelSprites");

  ADVBoxNamePlateMainPos = EnsureGetMemberVec2("ADVBoxNamePlateMainPos");
  ADVBoxNamePlateLabelPos = EnsureGetMemberVec2("ADVBoxNamePlateLabelPos");
}

}  // namespace DialogueBox
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto