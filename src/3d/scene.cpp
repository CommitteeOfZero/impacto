#include "scene.h"

#include <SDL.h>  // M_PI

#include "camera.h"
#include "../log.h"
#include "../window.h"
#include "../workqueue.h"

namespace Impacto {

bool IsInit = false;

Scene g_Scene;

void SceneInit() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Scene, "Initializing 3D scene system\n");
  IsInit = true;

  g_Camera.Init();
  Character3DInit();
  Background3DInit();

  g_Scene.LoadBackgroundAsync(g_BackgroundModelIds[0]);
  g_Scene.LoadCharacterAsync(g_ModelIds[0]);
  g_Scene.GroundPlane.MakePlane();
  g_Scene.GroundPlane.Submit();

  g_Scene.Tint = glm::vec4(0.784f, 0.671f, 0.6f, 0.9f);
  g_Scene.LightPosition = glm::vec3(1.0, 15.0, 1.0);
  g_Scene.DarkMode = false;
}

static void LoadBackgroundWorker(void* ptr) {
  Scene* scene = (Scene*)ptr;
  scene->CurrentBackground.Load(scene->BackgroundToLoadId);
}

static void OnBackgroundLoaded(void* ptr) {
  Scene* scene = (Scene*)ptr;
  scene->CurrentBackground.Submit();
  scene->CurrentBackgroundLoadStatus = OLS_Loaded;
}

bool Scene::LoadBackgroundAsync(uint32_t id) {
  if (CurrentBackgroundLoadStatus == OLS_Loading) {
    // Cannot currently cancel a load
    return false;
  }
  if (CurrentBackgroundLoadStatus == OLS_Loaded) {
    CurrentBackground.Unload();
    CurrentBackgroundLoadStatus = OLS_Unloaded;
  }

  BackgroundToLoadId = id;
  CurrentBackgroundLoadStatus = OLS_Loading;
  WorkQueuePush(this, &LoadBackgroundWorker, &OnBackgroundLoaded);
  return true;
}

static void LoadCharacterWorker(void* ptr) {
  Scene* scene = (Scene*)ptr;
  scene->CurrentCharacter.Load(scene->CharacterToLoadId);
}

static void OnCharacterLoaded(void* ptr) {
  Scene* scene = (Scene*)ptr;
  scene->CurrentCharacter.Submit();
  scene->CurrentCharacterLoadStatus = OLS_Loaded;
}

bool Scene::LoadCharacterAsync(uint32_t id) {
  if (CurrentCharacterLoadStatus == OLS_Loading) {
    // Cannot currently cancel a load
    return false;
  }
  if (CurrentCharacterLoadStatus == OLS_Loaded) {
    CurrentCharacter.Unload();
    Model::UnloadAnimations();
    CurrentCharacterLoadStatus = OLS_Unloaded;
  }

  CharacterToLoadId = id;
  CurrentCharacterLoadStatus = OLS_Loading;
  WorkQueuePush(this, &LoadCharacterWorker, &OnCharacterLoaded);
  return true;
}

void Scene::Update(float dt) {
  static float radius = 20.0f;
  static float inclination = M_PI / 4;
  static float azimuth = M_PI / 2;
  static float rotationSpeed = 1.0f;
  static int rotateCamera = 0;
  static nk_colorf tintColor = {0.784f, 0.671f, 0.6f, 0.9f};
  static uint32_t currentModel = 0;
  static uint32_t currentAnim = 0;
  static uint32_t currentBackground = 0;

  if (nk_begin(g_Nk, "Scene", nk_rect(20, 20, 300, g_WindowHeight - 40),
               NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
    if (nk_tree_push(g_Nk, NK_TREE_TAB, "Window", NK_MINIMIZED)) {
      nk_layout_row_dynamic(g_Nk, 24, 1);

      static int width;
      static int height;
      static bool needRefresh = true;
      if (needRefresh) {
        width = g_WindowWidth;
        height = g_WindowHeight;
        needRefresh = false;
      }

      nk_property_int(g_Nk, "Width", 0, &width, 8192, 0, 0.0f);
      nk_property_int(g_Nk, "Height", 0, &height, 8192, 0, 0.0f);

      if (nk_button_label(g_Nk, "Resize")) {
        WindowSetDimensions(width, height);
        needRefresh = true;
      }

      nk_tree_pop(g_Nk);
    }

    if (nk_tree_push(g_Nk, NK_TREE_TAB, "Camera", NK_MINIMIZED)) {
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

    if (nk_tree_push(g_Nk, NK_TREE_TAB, "Light", NK_MINIMIZED)) {
      nk_layout_row_dynamic(g_Nk, 24, 1);

      int darkMode = (int)DarkMode;
      nk_checkbox_label(g_Nk, "DarkMode", &darkMode);
      DarkMode = (bool)darkMode;

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

    if (CurrentBackgroundLoadStatus == OLS_Loaded) {
      if (nk_tree_push(g_Nk, NK_TREE_TAB, "Background", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(g_Nk, 24, 1);

        currentBackground = nk_combo(g_Nk, (const char**)g_BackgroundModelNames,
                                     g_BackgroundModelCount, currentBackground,
                                     24, nk_vec2(200, 200));
        if (g_BackgroundModelIds[currentBackground] !=
            CurrentBackground.StaticModel->Id) {
          LoadBackgroundAsync(g_BackgroundModelIds[currentBackground]);
        }

        nk_tree_pop(g_Nk);
      }
    }

    if (CurrentCharacterLoadStatus == OLS_Loaded) {
      if (nk_tree_push(g_Nk, NK_TREE_TAB, "Model", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(g_Nk, 24, 1);

        nk_property_float(g_Nk, "Model X", -40.0f,
                          &CurrentCharacter.ModelTransform.Position.x, 40.0f,
                          1.0f, 0.02f);
        nk_property_float(g_Nk, "Model Y", 0.0f,
                          &CurrentCharacter.ModelTransform.Position.y, 40.0f,
                          1.0f, 0.02f);
        nk_property_float(g_Nk, "Model Z", -40.0f,
                          &CurrentCharacter.ModelTransform.Position.z, 40.0f,
                          1.0f, 0.02f);

        currentModel = nk_combo(g_Nk, (const char**)g_ModelNames, g_ModelCount,
                                currentModel, 24, nk_vec2(200, 200));
        if (g_ModelIds[currentModel] != CurrentCharacter.StaticModel->Id) {
          currentAnim = 0;
          LoadCharacterAsync(g_ModelIds[currentModel]);
        }

        nk_tree_pop(g_Nk);
      }

      if (nk_tree_push(g_Nk, NK_TREE_TAB, "Animation", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(g_Nk, 24, 1);

        int isPlaying = (int)CurrentCharacter.Animator.IsPlaying;
        nk_checkbox_label(g_Nk, "Playing", &isPlaying);
        CurrentCharacter.Animator.IsPlaying = (bool)isPlaying;

        int tweening = (int)CurrentCharacter.Animator.Tweening;
        nk_checkbox_label(g_Nk, "Tweening", &tweening);
        CurrentCharacter.Animator.Tweening = (bool)tweening;

        if (CurrentCharacter.Animator.CurrentAnimation) {
          nk_property_float(g_Nk, "Loop start", 0.0f,
                            &CurrentCharacter.Animator.LoopStart,
                            CurrentCharacter.Animator.LoopEnd, 1.0f, 0.2f);
          nk_property_float(
              g_Nk, "Loop end", 0.0f, &CurrentCharacter.Animator.LoopEnd,
              CurrentCharacter.Animator.CurrentAnimation->Duration, 1.0f, 0.2f);

          // Nice hack
          float backup = CurrentCharacter.Animator.CurrentTime;
          nk_property_float(
              g_Nk, "Current time", 0.0f,
              &CurrentCharacter.Animator.CurrentTime,
              CurrentCharacter.Animator.CurrentAnimation->Duration, 1.0f, 0.2f);
          if (backup != CurrentCharacter.Animator.CurrentTime) {
            CurrentCharacter.Animator.Seek(
                CurrentCharacter.Animator.CurrentTime);
          }

          currentAnim =
              nk_combo(g_Nk, (const char**)g_AnimationNames, g_AnimationCount,
                       currentAnim, 24, nk_vec2(200, 200));
          if (g_AnimationIds[currentAnim] !=
              CurrentCharacter.Animator.CurrentAnimation->Id) {
            CurrentCharacter.Animator.Start(g_AnimationIds[currentAnim]);
          }
        }

        nk_tree_pop(g_Nk);
      }
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

  // g_Camera.Move(glm::vec3(0.0f, 12.5f, 23.0f));
  // g_Camera.LookAt(glm::vec3(0.449f, 12.579f, -23.285f));

  GroundPlane.Update(dt);
  if (CurrentCharacterLoadStatus == OLS_Loaded) {
    CurrentCharacter.Update(dt);
  }
}
void Scene::Render() {
  // Camera::Recalculate should stay here even for the real game
  g_Camera.Recalculate();

  Background3DUpdateGpu(this, &g_Camera);
  Character3DUpdateGpu(this, &g_Camera);

  if (CurrentBackgroundLoadStatus == OLS_Loaded) {
    CurrentBackground.Render();
  }
  GroundPlane.Render();
  if (CurrentCharacterLoadStatus == OLS_Loaded) {
    CurrentCharacter.Render();
  }
}

}  // namespace Impacto