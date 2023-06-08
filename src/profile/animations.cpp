#include "animations.h"
#include "profile_internal.h"
#include "../log.h"

namespace Impacto {
namespace Profile {

ska::flat_hash_map<std::string, SpriteAnimationDef> Animations;

void LoadAnimations() {
  EnsurePushMemberOfType("Animations", kObjectType);

  auto const& _animations = TopVal();
  for (Value::ConstMemberIterator it = _animations.MemberBegin();
       it != _animations.MemberEnd(); it++) {
    std::string name(EnsureGetKeyString(it));

    EnsurePushMemberIteratorOfType(it, kObjectType);

    SpriteAnimationDef& animation = Animations[name];
    animation.Duration = EnsureGetMemberFloat("Duration");

    {
      EnsurePushMemberOfType("Frames", kArrayType);

      auto const& _frames = TopVal();
      animation.FrameCount = _frames.Size();
      animation.Frames = (Sprite*)malloc(animation.FrameCount * sizeof(Sprite));

      for (uint32_t i = 0; i < animation.FrameCount; i++) {
        animation.Frames[i] = EnsureGetArrayElementSprite(i);
      }

      Pop();
    }

    Pop();
  }

  Pop();
}

}  // namespace Profile
}  // namespace Impacto