#include "dialoguebox.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../renderer/renderer.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace DialogueBox {

void Configure() {
  ADVBoxMask = EnsureGetMember<Sprite>("ADVBoxMask");
  ADVBoxEffectDuration = EnsureGetMember<float>("ADVBoxEffectDuration");
  const int NamePlateCount = EnsureGetMember<int>("ADVBoxNamePlateCount");
  GetMemberArray<Sprite>(NamePlateMainSprites, NamePlateCount,
                         "ADVBoxNamePlateMainSprites");
  GetMemberArray<Sprite>(NamePlateLabelSprites, NamePlateCount,
                         "ADVBoxNamePlateLabelSprites");

  ADVBoxNamePlateMainPos = EnsureGetMember<glm::vec2>("ADVBoxNamePlateMainPos");
  ADVBoxNamePlateLabelPos =
      EnsureGetMember<glm::vec2>("ADVBoxNamePlateLabelPos");
}

}  // namespace DialogueBox
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto