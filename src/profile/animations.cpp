#include "animations.h"
#include "profile_internal.h"
#include "../log.h"
#include "../window.h"

namespace Impacto {
namespace Profile {

ska::flat_hash_map<std::string, SpriteAnimationDef> Animations;

void LoadAnimations() {
  auto const& _animations =
      EnsureGetMemberOfType(Json, "/", "Animations", kObjectType);

  for (Value::ConstMemberIterator it = _animations.MemberBegin();
       it != _animations.MemberEnd(); ++it) {
    std::string name(EnsureGetString(it->name, "/Animations/x (name)"));
    AssertIs(it->value, "/Animations/x", kObjectType);

    ImpLog(LL_Debug, LC_Profile, "Loading animation %s\n", name.c_str());

    SpriteAnimationDef& animation = Animations[name];
    animation.Duration =
        EnsureGetMemberFloat(it->value, "/Animations/x", "Duration");

    auto const& frames =
        EnsureGetMemberOfType(it->value, "/Animations/x", "Frames", kArrayType);

    animation.FrameCount = frames.Size();
    animation.Frames = (Sprite*)malloc(frames.Size() * sizeof(Sprite));

    int i = 0;
    for (auto it = frames.Begin(); it != frames.End(); it++) {
      animation.Frames[i] = EnsureGetSprite(*it, "/Animations/x/Frames/y");
      i++;
    }
  }
}

}  // namespace Profile
}  // namespace Impacto