#include "dialoguebox.h"
#include "../../profile_internal.h"
#include "../../../log.h"
#include "../../../window.h"

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

static void GetMemberSpriteArray(Sprite* arr, uint32_t count,
                                 char const* name) {
  EnsurePushMemberOfType(name, kArrayType);

  if (TopVal().Size() != count) {
    ImpLog(LL_Fatal, LC_Profile, "Expected to have %d sprites for %s\n", count,
           name);
    Window::Shutdown();
  }

  for (uint32_t i = 0; i < count; i++) {
    arr[i] = EnsureGetArrayElementSprite(i);
  }

  Pop();
}

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