#pragma once

#include <glm/glm.hpp>

#include "character3d.h"
#include "background3d.h"
#include "camera.h"

namespace Impacto {

enum Object3DLoadStatus { OLS_Unloaded, OLS_Loading, OLS_Loaded };

class Scene {
 public:
  ~Scene();

  void Init();
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

 private:
  void SetupFramebufferState();
  void CleanFramebufferState();
  void DrawToScreen();

  GLuint FBO = 0;
  GLuint RenderTextureColor = 0;
  GLuint RenderTextureDS = 0;

  GLuint VAOScreenFillingTriangle = 0;
  GLuint VBOScreenFillingTriangle = 0;

  GLuint ShaderProgram = 0;
};

void SceneInit();

}  // namespace Impacto