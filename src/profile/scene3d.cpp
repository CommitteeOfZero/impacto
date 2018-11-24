#include "scene3d.h"
#include "profile_internal.h"

namespace Impacto {
namespace Profile {
namespace Scene3D {

int MaxRenderables;

glm::vec3 DefaultCameraPosition;
glm::vec3 DefaultCameraTarget;
float DefaultFov;

float AnimationDesignFrameRate;

std::vector<std::pair<uint32_t, uint32_t>> AnimationParseBlacklist;

ska::flat_hash_map<uint32_t, CharacterDef> Characters;
ska::flat_hash_map<uint32_t, CharacterDef&> ModelsToCharacters;

void Configure() {
  auto const& _scene3d =
      AssertIs(EnsureGetMember(Json, "/", "Scene3D"), "/Scene3D", kObjectType);

  MaxRenderables =
      EnsureGetInt(EnsureGetMember(_scene3d, "/Scene3D", "MaxRenderables"),
                   "/Scene3D/MaxRenderables");
  AnimationDesignFrameRate = EnsureGetFloat(
      EnsureGetMember(_scene3d, "/Scene3D", "AnimationDesignFrameRate"),
      "/Scene3D/AnimationDesignFrameRate");

  auto const& _defaultCamera =
      AssertIs(EnsureGetMember(_scene3d, "/Scene3D", "DefaultCamera"),
               "/Scene3D/DefaultCamera", kObjectType);

  DefaultCameraPosition = EnsureGetVec3(
      EnsureGetMember(_defaultCamera, "/Scene3D/DefaultCamera", "Position"),
      "/Scene3D/DefaultCamera/Position");
  DefaultCameraTarget = EnsureGetVec3(
      EnsureGetMember(_defaultCamera, "/Scene3D/DefaultCamera", "Target"),
      "/Scene3D/DefaultCamera/Target");
  DefaultFov = EnsureGetFloat(
      EnsureGetMember(_defaultCamera, "/Scene3D/DefaultCamera", "Fov"),
      "/Scene3D/DefaultCamera/Fov");

  if (_scene3d.HasMember("AnimationParseBlacklist")) {
    auto const& _animationParseBlacklist =
        AssertIs(_scene3d["AnimationParseBlacklist"],
                 "/Scene3D/AnimationParseBlacklist", kObjectType);

    for (Value::ConstMemberIterator it = _animationParseBlacklist.MemberBegin();
         it != _animationParseBlacklist.MemberEnd(); ++it) {
      int model =
          EnsureGetInt(it->name, "/Scene3D/AnimationParseBlacklist/x (name)");
      AssertIs(it->value, "/Scene3D/AnimationParseBlacklist/x", kArrayType);

      for (Value::ConstValueIterator anim = it->value.Begin();
           anim != it->value.End(); ++anim) {
        int animId =
            EnsureGetInt(*anim, "/Scene3D/AnimationParseBlacklist/x/y");
        AnimationParseBlacklist.emplace_back(model, animId);
      }
    }
  }
}

}  // namespace Scene3D
}  // namespace Profile
}  // namespace Impacto