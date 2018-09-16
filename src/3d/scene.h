#pragma once

#include "character3d.h"

namespace Impacto {

class Scene {
 public:
  void Init();
  void Update(float dt);
  void Render();

  Character3D CurrentCharacter;
  Character3D GroundPlane;
};

extern Scene g_Scene;

void SceneInit();

}  // namespace Impacto