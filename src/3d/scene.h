#pragma once

#include <glm/glm.hpp>

#include "character3d.h"

namespace Impacto {

class Scene {
 public:
  void Update(float dt);
  void Render();

  Character3D CurrentCharacter;
  Character3D GroundPlane;

  glm::vec3 LightPosition;
  glm::vec4 Tint;
  bool DarkMode;
};

extern Scene g_Scene;

void SceneInit();

}  // namespace Impacto