#include "scene.h"

#include <SDL.h>  // M_PI

#include "camera.h"
#include "../log.h"

namespace Impacto {

bool IsInit = false;

Scene g_Scene;

void SceneInit() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Scene, "Initializing 3D scene system\n");
  IsInit = true;

  g_Camera.Init();
  Character3DInit();

  g_Scene.CurrentCharacter.Load(0);
  g_Scene.CurrentCharacter.Submit();
  g_Scene.CurrentCharacter.ModelTransform.Position =
      glm::vec3(1.0f, 1.0f, 1.0f);
  g_Scene.GroundPlane.MakePlane();
  g_Scene.GroundPlane.Submit();

  g_Scene.LightPosition = glm::vec3(1.0, 1.0, 1.0);
}

void Scene::Update(float dt) {
  float const radius = 13.0f;
  static float angle = 0.0f;
  glm::vec3 position =
      glm::vec3(radius * cos(angle), radius, radius * sin(angle));
  angle += dt;
  if (angle >= 2 * M_PI) angle -= 2 * M_PI;
  g_Camera.Move(position);
  g_Camera.Recalculate();

  GroundPlane.Update(dt);
  CurrentCharacter.Update(dt);
}
void Scene::Render() {
  GroundPlane.Render();
  CurrentCharacter.Render();
}

}  // namespace Impacto