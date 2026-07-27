#include "animations.h"
#include "profile_internal.h"
#include "../log.h"

namespace Impacto {
namespace Profile {

void LoadAnimations() {
  {
    EnsurePushMemberOfType("Animations", LUA_TTABLE);

    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      std::string name(EnsureGetKey<std::string>());

      SpriteAnimationDef& animation = Animations[name];
      animation.Duration = EnsureGetMember<float>("Duration");
      animation.Frames = EnsureGetMember<std::vector<Sprite>>("Frames");

      Pop();
    }

    Pop();
  }

  {
    EnsurePushMemberOfType("FixedSpriteAnimations", LUA_TTABLE);

    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      std::string name(EnsureGetKey<std::string>());

      FixedSpriteAnimationDef& animation = FixedSpriteAnimations[name];
      animation.Duration = EnsureGetMember<float>("Duration");
      animation.Frames = EnsureGetMember<std::vector<Sprite>>("Frames");
      animation.FixedFrameIdx = EnsureGetMember<size_t>("FixedFrameIdx");

      Pop();
    }

    Pop();
  }
}

}  // namespace Profile
}  // namespace Impacto