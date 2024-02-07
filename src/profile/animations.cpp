#include "animations.h"
#include "profile_internal.h"
#include "../log.h"

namespace Impacto {
namespace Profile {

ska::flat_hash_map<std::string, SpriteAnimationDef> Animations;

void LoadAnimations() {
  EnsurePushMemberOfType("Animations", LUA_TTABLE);

  PushInitialIndex();
  while (PushNextTableElement() != 0) {
    std::string name(EnsureGetKeyString());

    SpriteAnimationDef& animation = Animations[name];
    animation.Duration = EnsureGetMemberFloat("Duration");

    {
      EnsurePushMemberOfType("Frames", LUA_TTABLE);

      animation.FrameCount = lua_rawlen(LuaState, -1);
      animation.Frames = (Sprite*)malloc(animation.FrameCount * sizeof(Sprite));
      PushInitialIndex();
      while (PushNextTableElement() != 0) {
        animation.Frames[EnsureGetKeyInt() - 1] = EnsureGetArrayElementSprite();
        Pop();
      }

      LuaDumpStack();

      Pop();
    }

    Pop();
  }

  Pop();
}

}  // namespace Profile
}  // namespace Impacto