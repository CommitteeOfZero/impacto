#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <flat_hash_map.hpp>
#include "../renderer/3d/model.h"

namespace Impacto {
namespace Profile {
namespace Scene3D {

inline LKMVersion Version = LKMVersion::RNE;

inline int MaxRenderables;

inline glm::vec3 DefaultCameraPosition;
inline glm::vec3 DefaultCameraTarget;
inline float DefaultFov;

inline float AnimationDesignFrameRate;

inline std::vector<std::pair<uint32_t, int16_t>> AnimationParseBlacklist;

struct AnimationDef {
  uint32_t CharacterId;
  int16_t AnimId;
  bool OneShot = false;
  float LoopStart = 0.0f;
  float LoopEnd = 0.0f;
};

class CharacterDef {
 public:
  uint32_t CharacterId;
  int16_t IdleAnimation;
  std::vector<uint32_t> Models;
  ska::flat_hash_map<int16_t, AnimationDef> Animations;
};

inline ska::flat_hash_map<uint32_t, CharacterDef> Characters;
inline ska::flat_hash_map<uint32_t, uint32_t> ModelsToCharacters;

void Configure();

}  // namespace Scene3D
}  // namespace Profile
}  // namespace Impacto