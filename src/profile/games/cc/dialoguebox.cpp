#include "dialoguebox.h"
#include "../../profile_internal.h"

namespace Impacto {
namespace Profile {
namespace CC {
namespace DialogueBox {

Sprite ADVBoxMask;

float ADVBoxEffectDuration;

void Configure() {
  ADVBoxMask = EnsureGetMemberSprite("ADVBoxMask");
  ADVBoxEffectDuration = EnsureGetMemberFloat("ADVBoxEffectDuration");
}

}  // namespace DialogueBox
}  // namespace CC
}  // namespace Profile
}  // namespace Impacto