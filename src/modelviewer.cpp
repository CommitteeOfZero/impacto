#include "modelviewer.h"
#include "game.h"

#include "window.h"

namespace Impacto {

ModelViewer::ModelViewer(Game* game) : GameContext(game) {
  Model::EnumerateModels();

  CameraPosition = glm::vec3(0.0f, 12.5f, 23.0f);
  CameraTarget = glm::vec3(0.0f, 12.5f, 0.0f);

  CurrentModel = 0;
  CurrentAnim = 0;
  CurrentBackground = 0;

  UiTintColor = {0.784f, 0.671f, 0.6f, 0.9f};
}

void ModelViewer::Init() {
  GameContext->Scene3D->Backgrounds[0].LoadAsync(g_BackgroundModelIds[0]);
  GameContext->Scene3D->Characters[0].LoadAsync(g_ModelIds[0]);

  GameContext->Scene3D->Tint = glm::vec4(0.784f, 0.671f, 0.6f, 0.9f);
  GameContext->Scene3D->LightPosition = glm::vec3(-2.85f, 16.68f, 6.30f);
  GameContext->Scene3D->DarkMode = false;

  LastTime = (float)((double)SDL_GetPerformanceCounter() /
                     (double)SDL_GetPerformanceFrequency());
  Frames = 0;
  FPS = 0.0f;
}

void ModelViewer::Update(float dt) {
  if (g_WindowDimensionsChanged) {
    UiWindowWidth = g_WindowWidth;
    UiWindowHeight = g_WindowHeight;
    UiMsaaCount = g_MsaaCount;
  }

  if (nk_begin(GameContext->Nk, "Scene",
               nk_rect(20, 20, 300, g_WindowHeight - 40),
               NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
    // FPS counter
    Frames++;
    float time = (float)((double)SDL_GetPerformanceCounter() /
                         (double)SDL_GetPerformanceFrequency());
    if (time - LastTime >= 2.0f) {
      FPS = (float)Frames / (time - LastTime);
      LastTime = time;
      Frames = 0;
    }

    nk_layout_row_dynamic(GameContext->Nk, 24, 1);
    char buffer[32];  // whatever
    snprintf(buffer, 32, "FPS: %02.2f", FPS);
    nk_label(GameContext->Nk, buffer, NK_TEXT_ALIGN_CENTERED);

    if (nk_tree_push(GameContext->Nk, NK_TREE_TAB, "Window", NK_MINIMIZED)) {
      nk_layout_row_dynamic(GameContext->Nk, 24, 1);

      nk_property_int(GameContext->Nk, "Width", 0, &UiWindowWidth, 8192, 0,
                      0.0f);
      nk_property_int(GameContext->Nk, "Height", 0, &UiWindowHeight, 8192, 0,
                      0.0f);
      nk_property_int(GameContext->Nk, "MSAA", 0, &UiMsaaCount, 16, 0, 0);

      if (nk_button_label(GameContext->Nk, "Resize")) {
        WindowSetDimensions(UiWindowWidth, UiWindowHeight, UiMsaaCount,
                            g_RenderScale);
      }

      nk_tree_pop(GameContext->Nk);
    }

    if (nk_tree_push(GameContext->Nk, NK_TREE_TAB, "Camera", NK_MINIMIZED)) {
      nk_layout_row_dynamic(GameContext->Nk, 24, 1);

      if (nk_button_label(GameContext->Nk, "Reset")) {
        CameraPosition = glm::vec3(0.0f, 12.5f, 23.0f);
        CameraTarget = glm::vec3(0.0f, 12.5f, 0.0f);
      }

      nk_property_float(GameContext->Nk, "Camera X", -1500.0f,
                        &CameraPosition.x, 1500.0f, 1.0f, 0.2f);
      nk_property_float(GameContext->Nk, "Camera Y", -1500.0f,
                        &CameraPosition.y, 1500.0f, 1.0f, 0.2f);
      nk_property_float(GameContext->Nk, "Camera Z", -1500.0f,
                        &CameraPosition.z, 1500.0f, 1.0f, 0.2f);

      nk_property_float(GameContext->Nk, "Camera target X", -1500.0f,
                        &CameraTarget.x, 1500.0f, 1.0f, 0.2f);
      nk_property_float(GameContext->Nk, "Camera target Y", -1500.0f,
                        &CameraTarget.y, 1500.0f, 1.0f, 0.2f);
      nk_property_float(GameContext->Nk, "Camera target Z", -1500.0f,
                        &CameraTarget.z, 1500.0f, 1.0f, 0.2f);

      nk_tree_pop(GameContext->Nk);
    }

    if (nk_tree_push(GameContext->Nk, NK_TREE_TAB, "Light", NK_MINIMIZED)) {
      nk_layout_row_dynamic(GameContext->Nk, 24, 1);

      int darkMode = (int)GameContext->Scene3D->DarkMode;
      nk_checkbox_label(GameContext->Nk, "DarkMode", &darkMode);
      GameContext->Scene3D->DarkMode = (bool)darkMode;

      nk_property_float(GameContext->Nk, "LightPosition.x", -40.0f,
                        &GameContext->Scene3D->LightPosition.x, 40.0f, 1.0f,
                        0.02f);
      nk_property_float(GameContext->Nk, "LightPosition.y", -40.0f,
                        &GameContext->Scene3D->LightPosition.y, 40.0f, 1.0f,
                        0.02f);
      nk_property_float(GameContext->Nk, "LightPosition.z", -40.0f,
                        &GameContext->Scene3D->LightPosition.z, 40.0f, 1.0f,
                        0.02f);

      nk_layout_row_dynamic(GameContext->Nk, 120, 1);
      nk_color_pick(GameContext->Nk, &UiTintColor, NK_RGBA);
      nk_layout_row_dynamic(GameContext->Nk, 24, 1);
      nk_property_float(GameContext->Nk, "Tint.R", 0.0f, &UiTintColor.r, 1.0f,
                        0.01f, 0.005f);
      nk_property_float(GameContext->Nk, "Tint.G", 0.0f, &UiTintColor.g, 1.0f,
                        0.01f, 0.005f);
      nk_property_float(GameContext->Nk, "Tint.B", 0.0f, &UiTintColor.b, 1.0f,
                        0.01f, 0.005f);
      nk_property_float(GameContext->Nk, "Tint.A", 0.0f, &UiTintColor.a, 1.0f,
                        0.01f, 0.005f);
      GameContext->Scene3D->Tint.r = UiTintColor.r;
      GameContext->Scene3D->Tint.g = UiTintColor.g;
      GameContext->Scene3D->Tint.b = UiTintColor.b;
      GameContext->Scene3D->Tint.a = UiTintColor.a;

      nk_tree_pop(GameContext->Nk);
    }

    if (GameContext->Scene3D->Backgrounds[0].Status == LS_Loaded) {
      if (nk_tree_push(GameContext->Nk, NK_TREE_TAB, "Background",
                       NK_MAXIMIZED)) {
        nk_layout_row_dynamic(GameContext->Nk, 24, 1);

        CurrentBackground = nk_combo(
            GameContext->Nk, (const char**)g_BackgroundModelNames,
            g_BackgroundModelCount, CurrentBackground, 24, nk_vec2(200, 200));
        if (g_BackgroundModelIds[CurrentBackground] !=
            GameContext->Scene3D->Backgrounds[0].StaticModel->Id) {
          GameContext->Scene3D->Backgrounds[0].LoadAsync(
              g_BackgroundModelIds[CurrentBackground]);
        }

        nk_tree_pop(GameContext->Nk);
      }
    }

    if (GameContext->Scene3D->Characters[0].Status == LS_Loaded) {
      if (nk_tree_push(GameContext->Nk, NK_TREE_TAB, "Model", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(GameContext->Nk, 24, 1);

        nk_property_float(
            GameContext->Nk, "Model X", -40.0f,
            &GameContext->Scene3D->Characters[0].ModelTransform.Position.x,
            40.0f, 1.0f, 0.02f);
        nk_property_float(
            GameContext->Nk, "Model Y", 0.0f,
            &GameContext->Scene3D->Characters[0].ModelTransform.Position.y,
            40.0f, 1.0f, 0.02f);
        nk_property_float(
            GameContext->Nk, "Model Z", -40.0f,
            &GameContext->Scene3D->Characters[0].ModelTransform.Position.z,
            40.0f, 1.0f, 0.02f);

        CurrentModel =
            nk_combo(GameContext->Nk, (const char**)g_ModelNames, g_ModelCount,
                     CurrentModel, 24, nk_vec2(200, 200));
        if (g_ModelIds[CurrentModel] !=
            GameContext->Scene3D->Characters[0].StaticModel->Id) {
          CurrentAnim = 0;
          GameContext->Scene3D->Characters[0].LoadAsync(
              g_ModelIds[CurrentModel]);
        }

        nk_tree_pop(GameContext->Nk);
      }

      if (nk_tree_push(GameContext->Nk, NK_TREE_TAB, "Animation",
                       NK_MAXIMIZED)) {
        nk_layout_row_dynamic(GameContext->Nk, 24, 1);

        int isPlaying =
            (int)GameContext->Scene3D->Characters[0].Animator.IsPlaying;
        nk_checkbox_label(GameContext->Nk, "Playing", &isPlaying);
        GameContext->Scene3D->Characters[0].Animator.IsPlaying =
            (bool)isPlaying;

        int tweening =
            (int)GameContext->Scene3D->Characters[0].Animator.Tweening;
        nk_checkbox_label(GameContext->Nk, "Tweening", &tweening);
        GameContext->Scene3D->Characters[0].Animator.Tweening = (bool)tweening;

        if (GameContext->Scene3D->Characters[0].Animator.CurrentAnimation) {
          nk_property_float(
              GameContext->Nk, "Loop start", 0.0f,
              &GameContext->Scene3D->Characters[0].Animator.LoopStart,
              GameContext->Scene3D->Characters[0].Animator.LoopEnd, 1.0f, 0.2f);
          nk_property_float(
              GameContext->Nk, "Loop end", 0.0f,
              &GameContext->Scene3D->Characters[0].Animator.LoopEnd,
              GameContext->Scene3D->Characters[0]
                  .Animator.CurrentAnimation->Duration,
              1.0f, 0.2f);

          // Nice hack
          float backup =
              GameContext->Scene3D->Characters[0].Animator.CurrentTime;
          nk_property_float(
              GameContext->Nk, "Current time", 0.0f,
              &GameContext->Scene3D->Characters[0].Animator.CurrentTime,
              GameContext->Scene3D->Characters[0]
                  .Animator.CurrentAnimation->Duration,
              1.0f, 0.2f);
          if (backup !=
              GameContext->Scene3D->Characters[0].Animator.CurrentTime) {
            GameContext->Scene3D->Characters[0].Animator.Seek(
                GameContext->Scene3D->Characters[0].Animator.CurrentTime);
          }

          CurrentAnim = nk_combo(
              GameContext->Nk,
              (const char**)GameContext->Scene3D->Characters[0]
                  .StaticModel->AnimationNames,
              GameContext->Scene3D->Characters[0].StaticModel->AnimationCount,
              CurrentAnim, 24, nk_vec2(200, 200));
          if (GameContext->Scene3D->Characters[0]
                  .StaticModel->AnimationIds[CurrentAnim] !=
              GameContext->Scene3D->Characters[0]
                  .Animator.CurrentAnimation->Id) {
            GameContext->Scene3D->Characters[0].SwitchAnimation(
                GameContext->Scene3D->Characters[0]
                    .StaticModel->AnimationIds[CurrentAnim],
                0.66f);
          }
        }

        nk_tree_pop(GameContext->Nk);
      }
    }
  }
  nk_end(GameContext->Nk);

  GameContext->Scene3D->MainCamera.Move(CameraPosition);
  GameContext->Scene3D->MainCamera.LookAt(CameraTarget);
}

}  // namespace Impacto