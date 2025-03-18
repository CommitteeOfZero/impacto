#include "animations.h"
#include "profile_internal.h"
#include "../log.h"

namespace Impacto {
namespace Profile {

void LoadAnimations() {
  EnsurePushMemberOfType("Animations", LUA_TTABLE);

  PushInitialIndex();
  while (PushNextTableElement() != 0) {
    std::string name(EnsureGetKey<std::string>());

    SpriteAnimationDef& animation = Animations[name];
    animation.Duration = EnsureGetMember<float>("Duration");

    {
      EnsurePushMemberOfType("Frames", LUA_TTABLE);

      animation.FrameCount = (uint32_t)lua_rawlen(LuaState, -1);
      animation.Frames = (Sprite*)malloc(animation.FrameCount * sizeof(Sprite));
      PushInitialIndex();
      while (PushNextTableElement() != 0) {
        animation.Frames[EnsureGetKey<int32_t>() - 1] =
            EnsureGetArrayElement<Sprite>();
        Pop();
      }

      Pop();
    }

    Pop();
  }

  Pop();
}

}  // namespace Profile
}  // namespace Impacto