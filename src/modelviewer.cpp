#include "modelviewer.h"
#include "game.h"

#include "window.h"

namespace Impacto {

ModelViewer::ModelViewer() {
  Model::EnumerateModels();

  CameraPosition = glm::vec3(0.0f, 12.5f, 23.0f);
  CameraTarget = glm::vec3(0.0f, 12.5f, 0.0f);

  CurrentModel = 0;
  CurrentAnim = 0;
  CurrentBackground = 0;

  UiTintColor = {0.784f, 0.671f, 0.6f, 0.9f};
}

void ModelViewer::Init() {
  g_Game->Scene3D->LoadBackgroundAsync(g_BackgroundModelIds[0]);
  g_Game->Scene3D->LoadCharacterAsync(g_ModelIds[0]);

  g_Game->Scene3D->Tint = glm::vec4(0.784f, 0.671f, 0.6f, 0.9f);
  g_Game->Scene3D->LightPosition = glm::vec3(-2.85f, 16.68f, 6.30f);
  g_Game->Scene3D->DarkMode = false;
}

void ModelViewer::Update(float dt) {
  if (g_FramebuffersNeedUpdate) {
    UiWindowWidth = g_WindowWidth;
    UiWindowHeight = g_WindowHeight;
    UiMsaaCount = g_MsaaCount;
  }

  if (nk_begin(g_Game->Nk, "Scene", nk_rect(20, 20, 300, g_WindowHeight - 40),
               NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
    if (nk_tree_push(g_Game->Nk, NK_TREE_TAB, "Window", NK_MINIMIZED)) {
      nk_layout_row_dynamic(g_Game->Nk, 24, 1);

      nk_property_int(g_Game->Nk, "Width", 0, &UiWindowWidth, 8192, 0, 0.0f);
      nk_property_int(g_Game->Nk, "Height", 0, &UiWindowHeight, 8192, 0, 0.0f);
      nk_property_int(g_Game->Nk, "MSAA", 0, &UiMsaaCount, 16, 0, 0);

      if (nk_button_label(g_Game->Nk, "Resize")) {
        WindowSetDimensions(UiWindowWidth, UiWindowHeight, UiMsaaCount,
                            g_RenderScale);
      }

      nk_tree_pop(g_Game->Nk);
    }

    if (nk_tree_push(g_Game->Nk, NK_TREE_TAB, "Camera", NK_MINIMIZED)) {
      nk_layout_row_dynamic(g_Game->Nk, 24, 1);

      if (nk_button_label(g_Game->Nk, "Reset")) {
        CameraPosition = glm::vec3(0.0f, 12.5f, 23.0f);
        CameraTarget = glm::vec3(0.0f, 12.5f, 0.0f);
      }

      nk_property_float(g_Game->Nk, "Camera X", -1500.0f, &CameraPosition.x,
                        1500.0f, 1.0f, 0.2f);
      nk_property_float(g_Game->Nk, "Camera Y", -1500.0f, &CameraPosition.y,
                        1500.0f, 1.0f, 0.2f);
      nk_property_float(g_Game->Nk, "Camera Z", -1500.0f, &CameraPosition.z,
                        1500.0f, 1.0f, 0.2f);

      nk_property_float(g_Game->Nk, "Camera target X", -1500.0f,
                        &CameraTarget.x, 1500.0f, 1.0f, 0.2f);
      nk_property_float(g_Game->Nk, "Camera target Y", -1500.0f,
                        &CameraTarget.y, 1500.0f, 1.0f, 0.2f);
      nk_property_float(g_Game->Nk, "Camera target Z", -1500.0f,
                        &CameraTarget.z, 1500.0f, 1.0f, 0.2f);

      nk_tree_pop(g_Game->Nk);
    }

    if (nk_tree_push(g_Game->Nk, NK_TREE_TAB, "Light", NK_MINIMIZED)) {
      nk_layout_row_dynamic(g_Game->Nk, 24, 1);

      int darkMode = (int)g_Game->Scene3D->DarkMode;
      nk_checkbox_label(g_Game->Nk, "DarkMode", &darkMode);
      g_Game->Scene3D->DarkMode = (bool)darkMode;

      nk_property_float(g_Game->Nk, "LightPosition.x", -40.0f,
                        &g_Game->Scene3D->LightPosition.x, 40.0f, 1.0f, 0.02f);
      nk_property_float(g_Game->Nk, "LightPosition.y", -40.0f,
                        &g_Game->Scene3D->LightPosition.y, 40.0f, 1.0f, 0.02f);
      nk_property_float(g_Game->Nk, "LightPosition.z", -40.0f,
                        &g_Game->Scene3D->LightPosition.z, 40.0f, 1.0f, 0.02f);

      nk_layout_row_dynamic(g_Game->Nk, 120, 1);
      nk_color_pick(g_Game->Nk, &UiTintColor, NK_RGBA);
      nk_layout_row_dynamic(g_Game->Nk, 24, 1);
      nk_property_float(g_Game->Nk, "Tint.R", 0.0f, &UiTintColor.r, 1.0f, 0.01f,
                        0.005f);
      nk_property_float(g_Game->Nk, "Tint.G", 0.0f, &UiTintColor.g, 1.0f, 0.01f,
                        0.005f);
      nk_property_float(g_Game->Nk, "Tint.B", 0.0f, &UiTintColor.b, 1.0f, 0.01f,
                        0.005f);
      nk_property_float(g_Game->Nk, "Tint.A", 0.0f, &UiTintColor.a, 1.0f, 0.01f,
                        0.005f);
      g_Game->Scene3D->Tint.r = UiTintColor.r;
      g_Game->Scene3D->Tint.g = UiTintColor.g;
      g_Game->Scene3D->Tint.b = UiTintColor.b;
      g_Game->Scene3D->Tint.a = UiTintColor.a;

      nk_tree_pop(g_Game->Nk);
    }

    if (g_Game->Scene3D->CurrentBackgroundLoadStatus == OLS_Loaded) {
      if (nk_tree_push(g_Game->Nk, NK_TREE_TAB, "Background", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(g_Game->Nk, 24, 1);

        CurrentBackground = nk_combo(
            g_Game->Nk, (const char**)g_BackgroundModelNames,
            g_BackgroundModelCount, CurrentBackground, 24, nk_vec2(200, 200));
        if (g_BackgroundModelIds[CurrentBackground] !=
            g_Game->Scene3D->CurrentBackground.StaticModel->Id) {
          g_Game->Scene3D->LoadBackgroundAsync(
              g_BackgroundModelIds[CurrentBackground]);
        }

        nk_tree_pop(g_Game->Nk);
      }
    }

    if (g_Game->Scene3D->CurrentCharacterLoadStatus == OLS_Loaded) {
      if (nk_tree_push(g_Game->Nk, NK_TREE_TAB, "Model", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(g_Game->Nk, 24, 1);

        nk_property_float(
            g_Game->Nk, "Model X", -40.0f,
            &g_Game->Scene3D->CurrentCharacter.ModelTransform.Position.x, 40.0f,
            1.0f, 0.02f);
        nk_property_float(
            g_Game->Nk, "Model Y", 0.0f,
            &g_Game->Scene3D->CurrentCharacter.ModelTransform.Position.y, 40.0f,
            1.0f, 0.02f);
        nk_property_float(
            g_Game->Nk, "Model Z", -40.0f,
            &g_Game->Scene3D->CurrentCharacter.ModelTransform.Position.z, 40.0f,
            1.0f, 0.02f);

        CurrentModel =
            nk_combo(g_Game->Nk, (const char**)g_ModelNames, g_ModelCount,
                     CurrentModel, 24, nk_vec2(200, 200));
        if (g_ModelIds[CurrentModel] !=
            g_Game->Scene3D->CurrentCharacter.StaticModel->Id) {
          CurrentAnim = 0;
          g_Game->Scene3D->LoadCharacterAsync(g_ModelIds[CurrentModel]);
        }

        nk_tree_pop(g_Game->Nk);
      }

      if (nk_tree_push(g_Game->Nk, NK_TREE_TAB, "Animation", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(g_Game->Nk, 24, 1);

        int isPlaying =
            (int)g_Game->Scene3D->CurrentCharacter.Animator.IsPlaying;
        nk_checkbox_label(g_Game->Nk, "Playing", &isPlaying);
        g_Game->Scene3D->CurrentCharacter.Animator.IsPlaying = (bool)isPlaying;

        int tweening = (int)g_Game->Scene3D->CurrentCharacter.Animator.Tweening;
        nk_checkbox_label(g_Game->Nk, "Tweening", &tweening);
        g_Game->Scene3D->CurrentCharacter.Animator.Tweening = (bool)tweening;

        if (g_Game->Scene3D->CurrentCharacter.Animator.CurrentAnimation) {
          nk_property_float(
              g_Game->Nk, "Loop start", 0.0f,
              &g_Game->Scene3D->CurrentCharacter.Animator.LoopStart,
              g_Game->Scene3D->CurrentCharacter.Animator.LoopEnd, 1.0f, 0.2f);
          nk_property_float(g_Game->Nk, "Loop end", 0.0f,
                            &g_Game->Scene3D->CurrentCharacter.Animator.LoopEnd,
                            g_Game->Scene3D->CurrentCharacter.Animator
                                .CurrentAnimation->Duration,
                            1.0f, 0.2f);

          // Nice hack
          float backup = g_Game->Scene3D->CurrentCharacter.Animator.CurrentTime;
          nk_property_float(
              g_Game->Nk, "Current time", 0.0f,
              &g_Game->Scene3D->CurrentCharacter.Animator.CurrentTime,
              g_Game->Scene3D->CurrentCharacter.Animator.CurrentAnimation
                  ->Duration,
              1.0f, 0.2f);
          if (backup !=
              g_Game->Scene3D->CurrentCharacter.Animator.CurrentTime) {
            g_Game->Scene3D->CurrentCharacter.Animator.Seek(
                g_Game->Scene3D->CurrentCharacter.Animator.CurrentTime);
          }

          CurrentAnim = nk_combo(
              g_Game->Nk,
              (const char**)
                  g_Game->Scene3D->CurrentCharacter.StaticModel->AnimationNames,
              g_Game->Scene3D->CurrentCharacter.StaticModel->AnimationCount,
              CurrentAnim, 24, nk_vec2(200, 200));
          if (g_Game->Scene3D->CurrentCharacter.StaticModel
                  ->AnimationIds[CurrentAnim] !=
              g_Game->Scene3D->CurrentCharacter.Animator.CurrentAnimation->Id) {
            g_Game->Scene3D->CurrentCharacter.SwitchAnimation(
                g_Game->Scene3D->CurrentCharacter.StaticModel
                    ->AnimationIds[CurrentAnim],
                0.66f);
          }
        }

        nk_tree_pop(g_Game->Nk);
      }
    }
  }
  nk_end(g_Game->Nk);

  g_Game->Scene3D->MainCamera.Move(CameraPosition);
  g_Game->Scene3D->MainCamera.LookAt(CameraTarget);
}

}  // namespace Impacto