#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <flat_hash_map.hpp>

namespace Impacto {
namespace Profile {
namespace Scene3D {

extern int MaxRenderables;

extern glm::vec3 DefaultCameraPosition;
extern glm::vec3 DefaultCameraTarget;
extern float DefaultFov;

extern float AnimationDesignFrameRate;

extern std::vector<std::pair<uint32_t, uint16_t>> AnimationParseBlacklist;

struct AnimationDef {
  uint32_t CharacterId;
  uint16_t AnimId;
  bool OneShot = false;
  float LoopStart = 0.0f;
  float LoopEnd = 0.0f;
};

class CharacterDef {
 public:
  uint32_t CharacterId;
  std::vector<uint32_t> Models;
  ska::flat_hash_map<uint16_t, AnimationDef> Animations;
};

extern ska::flat_hash_map<uint32_t, CharacterDef> Characters;
extern ska::flat_hash_map<uint32_t, uint32_t> ModelsToCharacters;

void Configure();

}  // namespace Scene3D
}  // namespace Profile
}  // namespace Impacto