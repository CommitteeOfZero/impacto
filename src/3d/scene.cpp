#include "scene.h"

#include <SDL.h>  // M_PI

#include "camera.h"
#include "../log.h"
#include "../window.h"

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
  g_Scene.GroundPlane.MakePlane();
  g_Scene.GroundPlane.Submit();

  g_Scene.LightPosition = glm::vec3(1.0, 15.0, 1.0);
}

void Scene::Update(float dt) {
  static float radius = 20.0f;
  static float inclination = M_PI / 4;
  static float azimuth = M_PI / 2;
  static float rotationSpeed = 1.0f;
  static int rotateCamera = 0;
  static nk_colorf tintColor = {0.784f, 0.671f, 0.6f, 0.9f};

  if (nk_begin(g_Nk, "Scene", nk_rect(20, 20, 300, 680),
               NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
    if (nk_tree_push(g_Nk, NK_TREE_TAB, "Camera", NK_MAXIMIZED)) {
      nk_layout_row_dynamic(g_Nk, 24, 1);

      if (nk_button_label(g_Nk, "Reset")) {
        radius = 20.0f;
        inclination = M_PI / 4;
        azimuth = M_PI / 2;
        rotationSpeed = 1.0f;
      }

      nk_checkbox_label(g_Nk, "Rotating", &rotateCamera);
      nk_property_float(g_Nk, "Rotation speed", 0.1f, &rotationSpeed, 4.0f,
                        0.1f, 0.02f);
      nk_property_float(g_Nk, "Radius", 1.0f, &radius, 100.0f, 5.0f, 1.0f);
      nk_property_float(g_Nk, "Inclination", 0.0f, &inclination, 2 * M_PI,
                        0.05f, 0.01f);
      nk_property_float(g_Nk, "Azimuth", 0.0f, &azimuth, 2 * M_PI, 0.05f,
                        0.01f);

      nk_tree_pop(g_Nk);
    }

    if (nk_tree_push(g_Nk, NK_TREE_TAB, "Light", NK_MAXIMIZED)) {
      nk_layout_row_dynamic(g_Nk, 24, 1);

      nk_property_float(g_Nk, "LightPosition.x", -40.0f, &LightPosition.x,
                        40.0f, 1.0f, 0.02f);
      nk_property_float(g_Nk, "LightPosition.y", -40.0f, &LightPosition.y,
                        40.0f, 1.0f, 0.02f);
      nk_property_float(g_Nk, "LightPosition.z", -40.0f, &LightPosition.z,
                        40.0f, 1.0f, 0.02f);

      nk_layout_row_dynamic(g_Nk, 120, 1);
      nk_color_pick(g_Nk, &tintColor, NK_RGBA);
      nk_layout_row_dynamic(g_Nk, 24, 1);
      nk_property_float(g_Nk, "Tint.R", 0.0f, &tintColor.r, 1.0f, 0.01f,
                        0.005f);
      nk_property_float(g_Nk, "Tint.G", 0.0f, &tintColor.g, 1.0f, 0.01f,
                        0.005f);
      nk_property_float(g_Nk, "Tint.B", 0.0f, &tintColor.b, 1.0f, 0.01f,
                        0.005f);
      nk_property_float(g_Nk, "Tint.A", 0.0f, &tintColor.a, 1.0f, 0.01f,
                        0.005f);
      Tint.r = tintColor.r;
      Tint.g = tintColor.g;
      Tint.b = tintColor.b;
      Tint.a = tintColor.a;

      nk_tree_pop(g_Nk);
    }

    if (nk_tree_push(g_Nk, NK_TREE_TAB, "Model", NK_MAXIMIZED)) {
      nk_layout_row_dynamic(g_Nk, 24, 1);

      nk_property_float(g_Nk, "Model X", -20.0f,
                        &CurrentCharacter.ModelTransform.Position.x, 20.0f,
                        1.0f, 0.02f);
      nk_property_float(g_Nk, "Model Y", 0.0f,
                        &CurrentCharacter.ModelTransform.Position.y, 20.0f,
                        1.0f, 0.02f);
      nk_property_float(g_Nk, "Model Z", -20.0f,
                        &CurrentCharacter.ModelTransform.Position.z, 20.0f,
                        1.0f, 0.02f);

      nk_tree_pop(g_Nk);
    }
  }
  nk_end(g_Nk);

  glm::vec3 position = glm::vec3(radius * sin(inclination) * cos(azimuth),
                                 radius * cos(inclination),
                                 radius * sin(inclination) * sin(azimuth));
  if (rotateCamera) {
    azimuth += rotationSpeed * dt;
    if (azimuth >= 2 * M_PI) azimuth -= 2 * M_PI;
  }
  g_Camera.Move(position);

  GroundPlane.Update(dt);
  CurrentCharacter.Update(dt);
}
void Scene::Render() {
  // Camera::Recalculate should stay here even for the real game
  g_Camera.Recalculate();

  Character3DUpdateGpu(this, &g_Camera);

  GroundPlane.Render();
  CurrentCharacter.Render();
}

}  // namespace Impacto