#include "modelviewergame.h"

#include "window.h"

namespace Impacto {

GameFeatureConfig ModelViewerGameConfig() {
  GameFeatureConfig result;
  result.LayerCount = 1;
  result.GameFeatures = GameFeature_Nuklear | GameFeature_Scene3D;
  result.Scene3D_BackgroundCount = 1;
  result.Scene3D_CharacterCount = 1;
  return result;
}

ModelViewerGame::ModelViewerGame() : BaseGame(ModelViewerGameConfig()) {
  Model::EnumerateModels();
  Scene3D->LoadBackgroundAsync(g_BackgroundModelIds[0]);
  Scene3D->LoadCharacterAsync(g_ModelIds[0]);

  Scene3D->Tint = glm::vec4(0.784f, 0.671f, 0.6f, 0.9f);
  Scene3D->LightPosition = glm::vec3(-2.85f, 16.68f, 6.30f);
  Scene3D->DarkMode = false;

  CameraPosition = glm::vec3(0.0f, 12.5f, 23.0f);
  CameraTarget = glm::vec3(0.0f, 12.5f, 0.0f);

  CurrentModel = 0;
  CurrentAnim = 0;
  CurrentBackground = 0;

  UiTintColor = {0.784f, 0.671f, 0.6f, 0.9f};

  UiWindowDimsNeedUpdate = true;
}

void ModelViewerGame::GameUpdate(float dt) {
  if (nk_begin(Nk, "Scene", nk_rect(20, 20, 300, g_WindowHeight - 40),
               NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
    if (nk_tree_push(Nk, NK_TREE_TAB, "Window", NK_MINIMIZED)) {
      nk_layout_row_dynamic(Nk, 24, 1);

      if (UiWindowDimsNeedUpdate) {
        UiWindowWidth = g_WindowWidth;
        UiWindowHeight = g_WindowHeight;
        UiWindowDimsNeedUpdate = false;
      }

      nk_property_int(Nk, "Width", 0, &UiWindowWidth, 8192, 0, 0.0f);
      nk_property_int(Nk, "Height", 0, &UiWindowHeight, 8192, 0, 0.0f);

      if (nk_button_label(Nk, "Resize")) {
        WindowSetDimensions(UiWindowWidth, UiWindowHeight);
        UiWindowDimsNeedUpdate = true;
      }

      nk_tree_pop(Nk);
    }

    if (nk_tree_push(Nk, NK_TREE_TAB, "Camera", NK_MINIMIZED)) {
      nk_layout_row_dynamic(Nk, 24, 1);

      if (nk_button_label(Nk, "Reset")) {
        CameraPosition = glm::vec3(0.0f, 12.5f, 23.0f);
        CameraTarget = glm::vec3(0.0f, 12.5f, 0.0f);
      }

      nk_property_float(Nk, "Camera X", -1500.0f, &CameraPosition.x, 1500.0f,
                        1.0f, 0.2f);
      nk_property_float(Nk, "Camera Y", -1500.0f, &CameraPosition.y, 1500.0f,
                        1.0f, 0.2f);
      nk_property_float(Nk, "Camera Z", -1500.0f, &CameraPosition.z, 1500.0f,
                        1.0f, 0.2f);

      nk_property_float(Nk, "Camera target X", -1500.0f, &CameraTarget.x,
                        1500.0f, 1.0f, 0.2f);
      nk_property_float(Nk, "Camera target Y", -1500.0f, &CameraTarget.y,
                        1500.0f, 1.0f, 0.2f);
      nk_property_float(Nk, "Camera target Z", -1500.0f, &CameraTarget.z,
                        1500.0f, 1.0f, 0.2f);

      nk_tree_pop(Nk);
    }

    if (nk_tree_push(Nk, NK_TREE_TAB, "Light", NK_MINIMIZED)) {
      nk_layout_row_dynamic(Nk, 24, 1);

      int darkMode = (int)Scene3D->DarkMode;
      nk_checkbox_label(Nk, "DarkMode", &darkMode);
      Scene3D->DarkMode = (bool)darkMode;

      nk_property_float(Nk, "LightPosition.x", -40.0f,
                        &Scene3D->LightPosition.x, 40.0f, 1.0f, 0.02f);
      nk_property_float(Nk, "LightPosition.y", -40.0f,
                        &Scene3D->LightPosition.y, 40.0f, 1.0f, 0.02f);
      nk_property_float(Nk, "LightPosition.z", -40.0f,
                        &Scene3D->LightPosition.z, 40.0f, 1.0f, 0.02f);

      nk_layout_row_dynamic(Nk, 120, 1);
      nk_color_pick(Nk, &UiTintColor, NK_RGBA);
      nk_layout_row_dynamic(Nk, 24, 1);
      nk_property_float(Nk, "Tint.R", 0.0f, &UiTintColor.r, 1.0f, 0.01f,
                        0.005f);
      nk_property_float(Nk, "Tint.G", 0.0f, &UiTintColor.g, 1.0f, 0.01f,
                        0.005f);
      nk_property_float(Nk, "Tint.B", 0.0f, &UiTintColor.b, 1.0f, 0.01f,
                        0.005f);
      nk_property_float(Nk, "Tint.A", 0.0f, &UiTintColor.a, 1.0f, 0.01f,
                        0.005f);
      Scene3D->Tint.r = UiTintColor.r;
      Scene3D->Tint.g = UiTintColor.g;
      Scene3D->Tint.b = UiTintColor.b;
      Scene3D->Tint.a = UiTintColor.a;

      nk_tree_pop(Nk);
    }

    if (Scene3D->CurrentBackgroundLoadStatus == OLS_Loaded) {
      if (nk_tree_push(Nk, NK_TREE_TAB, "Background", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(Nk, 24, 1);

        CurrentBackground = nk_combo(Nk, (const char**)g_BackgroundModelNames,
                                     g_BackgroundModelCount, CurrentBackground,
                                     24, nk_vec2(200, 200));
        if (g_BackgroundModelIds[CurrentBackground] !=
            Scene3D->CurrentBackground.StaticModel->Id) {
          Scene3D->LoadBackgroundAsync(g_BackgroundModelIds[CurrentBackground]);
        }

        nk_tree_pop(Nk);
      }
    }

    if (Scene3D->CurrentCharacterLoadStatus == OLS_Loaded) {
      if (nk_tree_push(Nk, NK_TREE_TAB, "Model", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(Nk, 24, 1);

        nk_property_float(Nk, "Model X", -40.0f,
                          &Scene3D->CurrentCharacter.ModelTransform.Position.x,
                          40.0f, 1.0f, 0.02f);
        nk_property_float(Nk, "Model Y", 0.0f,
                          &Scene3D->CurrentCharacter.ModelTransform.Position.y,
                          40.0f, 1.0f, 0.02f);
        nk_property_float(Nk, "Model Z", -40.0f,
                          &Scene3D->CurrentCharacter.ModelTransform.Position.z,
                          40.0f, 1.0f, 0.02f);

        CurrentModel = nk_combo(Nk, (const char**)g_ModelNames, g_ModelCount,
                                CurrentModel, 24, nk_vec2(200, 200));
        if (g_ModelIds[CurrentModel] !=
            Scene3D->CurrentCharacter.StaticModel->Id) {
          CurrentAnim = 0;
          Scene3D->LoadCharacterAsync(g_ModelIds[CurrentModel]);
        }

        nk_tree_pop(Nk);
      }

      if (nk_tree_push(Nk, NK_TREE_TAB, "Animation", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(Nk, 24, 1);

        int isPlaying = (int)Scene3D->CurrentCharacter.Animator.IsPlaying;
        nk_checkbox_label(Nk, "Playing", &isPlaying);
        Scene3D->CurrentCharacter.Animator.IsPlaying = (bool)isPlaying;

        int tweening = (int)Scene3D->CurrentCharacter.Animator.Tweening;
        nk_checkbox_label(Nk, "Tweening", &tweening);
        Scene3D->CurrentCharacter.Animator.Tweening = (bool)tweening;

        if (Scene3D->CurrentCharacter.Animator.CurrentAnimation) {
          nk_property_float(Nk, "Loop start", 0.0f,
                            &Scene3D->CurrentCharacter.Animator.LoopStart,
                            Scene3D->CurrentCharacter.Animator.LoopEnd, 1.0f,
                            0.2f);
          nk_property_float(
              Nk, "Loop end", 0.0f, &Scene3D->CurrentCharacter.Animator.LoopEnd,
              Scene3D->CurrentCharacter.Animator.CurrentAnimation->Duration,
              1.0f, 0.2f);

          // Nice hack
          float backup = Scene3D->CurrentCharacter.Animator.CurrentTime;
          nk_property_float(
              Nk, "Current time", 0.0f,
              &Scene3D->CurrentCharacter.Animator.CurrentTime,
              Scene3D->CurrentCharacter.Animator.CurrentAnimation->Duration,
              1.0f, 0.2f);
          if (backup != Scene3D->CurrentCharacter.Animator.CurrentTime) {
            Scene3D->CurrentCharacter.Animator.Seek(
                Scene3D->CurrentCharacter.Animator.CurrentTime);
          }

          CurrentAnim = nk_combo(
              Nk,
              (const char**)
                  Scene3D->CurrentCharacter.StaticModel->AnimationNames,
              Scene3D->CurrentCharacter.StaticModel->AnimationCount,
              CurrentAnim, 24, nk_vec2(200, 200));
          if (Scene3D->CurrentCharacter.StaticModel
                  ->AnimationIds[CurrentAnim] !=
              Scene3D->CurrentCharacter.Animator.CurrentAnimation->Id) {
            Scene3D->CurrentCharacter.SwitchAnimation(
                Scene3D->CurrentCharacter.StaticModel
                    ->AnimationIds[CurrentAnim],
                0.66f);
          }
        }

        nk_tree_pop(Nk);
      }
    }
  }
  nk_end(Nk);

  Scene3D->MainCamera.Move(CameraPosition);
  Scene3D->MainCamera.LookAt(CameraTarget);
}

void ModelViewerGame::DrawLayer(uint32_t layerId) {
  if (layerId == 0) {
    Scene3D->Render();
  }
}

}  // namespace Impacto