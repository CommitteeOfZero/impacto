#pragma once

#include <glm/glm.hpp>

#include "character3d.h"
#include "background3d.h"
#include "camera.h"

namespace Impacto {

enum Object3DLoadStatus { OLS_Unloaded, OLS_Loading, OLS_Loaded };

class Scene {
 public:
  void Update(float dt);
  void Render();

  bool LoadBackgroundAsync(uint32_t id);
  bool LoadCharacterAsync(uint32_t id);

  Camera MainCamera;

  Background3D CurrentBackground;

  Character3D CurrentCharacter;

  Object3DLoadStatus CurrentBackgroundLoadStatus = OLS_Unloaded;
  Object3DLoadStatus CurrentCharacterLoadStatus = OLS_Unloaded;

  glm::vec3 LightPosition;
  glm::vec4 Tint;
  bool DarkMode;

  uint32_t BackgroundToLoadId;
  uint32_t CharacterToLoadId;
};

extern Scene g_Scene;

void SceneInit();

}  // namespace Impacto