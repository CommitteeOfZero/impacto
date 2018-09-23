#pragma once

#include <glm/glm.hpp>

#include "character3d.h"

namespace Impacto {

enum CharacterLoadStatus { CLS_Unloaded, CLS_Loading, CLS_Loaded };

class Scene {
 public:
  void Update(float dt);
  void Render();

  bool LoadCharacterAsync(uint32_t id);

  Character3D CurrentCharacter;
  Character3D GroundPlane;

  CharacterLoadStatus CurrentCharacterLoadStatus = CLS_Unloaded;

  glm::vec3 LightPosition;
  glm::vec4 Tint;
  bool DarkMode;

  uint32_t CharacterToLoadId;
};

extern Scene g_Scene;

void SceneInit();

}  // namespace Impacto