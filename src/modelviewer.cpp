#include "modelviewer.h"
#include "game.h"

// #include "window.h"
#include "io/io.h"
#include "log.h"
#include "renderer/renderer.h"
#include "audio/audiosystem.h"
#include "audio/audiochannel.h"
#include "renderer/3d/scene.h"
#include "renderer/3d/model.h"

#include "profile/scene3d.h"

namespace Impacto {
namespace ModelViewer {

static void EnumerateBgm();

static glm::vec3 CameraPosition;
static glm::vec3 CameraTarget;
static bool TrackCamera;
static glm::vec4 UiTintColor;
static uint32_t CurrentModel;
static uint32_t CurrentAnim;
static uint32_t CurrentBackground;
static uint32_t CurrentBgm;
static int UiWindowWidth;
static int UiWindowHeight;
static int UiMsaaCount;

static char** BgmNames = 0;
static uint32_t* BgmIds = 0;
static uint32_t BgmCount = 0;
static bool BgmChangeQueued;

static float BgmFadeOut;
static float BgmFadeIn;
static int BgmLoop;

void Init() {
  Model::EnumerateModels();
  EnumerateBgm();

  CameraPosition = Profile::Scene3D::DefaultCameraPosition;
  CameraTarget = Profile::Scene3D::DefaultCameraTarget;

  CurrentModel = 0;
  CurrentAnim = 0;
  CurrentBackground = 0;
  CurrentBgm = 0;
  BgmChangeQueued = false;

  BgmFadeOut = 0.0f;
  BgmFadeIn = 0.0f;
  BgmLoop = true;

  UiTintColor = glm::vec4(0.784f, 0.671f, 0.6f, 0.9f);

  Renderer->Scene->Renderables[0]->LoadAsync(g_BackgroundModelIds[0]);
  Renderer->Scene->Renderables[1]->LoadAsync(g_ModelIds[0]);

  Renderer->Scene->Tint = glm::vec4(0.784f, 0.671f, 0.6f, 0.9f);
  Renderer->Scene->LightPosition = glm::vec3(-2.85f, 16.68f, 6.30f);
  Renderer->Scene->DarkMode = false;

  UiWindowWidth = Window->WindowWidth;
  UiWindowHeight = Window->WindowHeight;
  UiMsaaCount = Window->MsaaCount;
}

void Update(float dt) {
  if (Window->WindowDimensionsChanged) {
    UiWindowWidth = Window->WindowWidth;
    UiWindowHeight = Window->WindowHeight;
    UiMsaaCount = Window->MsaaCount;
  }

  if (Renderer->Scene->Renderables[0]->Status == LS_Loaded) {
    if (g_BackgroundModelIds[CurrentBackground] !=
        Renderer->Scene->Renderables[0]->StaticModel->Id) {
      Renderer->Scene->Renderables[0]->LoadAsync(
          g_BackgroundModelIds[CurrentBackground]);
    }
  }

#ifndef IMPACTO_DISABLE_IMGUI
  if (ImGui::Begin("Scene", nullptr,
                   ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                       ImGuiWindowFlags_NoResize)) {
    ImGui::SetWindowSize(ImVec2(300.0f, Window->WindowHeight - 40.0f),
                         ImGuiCond_Once);
    ImGui::SetWindowPos(ImVec2(20.0f, 20.0f), ImGuiCond_Once);
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

    ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    if (ImGui::CollapsingHeader("Window")) {
      ImGui::Spacing();
      ImGui::Text("Width");
      ImGui::DragInt("##Width", &UiWindowWidth, 0.1f, 0, 8192, "%d",
                     ImGuiSliderFlags_AlwaysClamp);
      ImGui::Spacing();
      ImGui::Text("Height");
      ImGui::DragInt("##Height", &UiWindowHeight, 0.1f, 0, 8192, "%d",
                     ImGuiSliderFlags_AlwaysClamp);
      ImGui::Spacing();
      ImGui::Text("MSAA");
      ImGui::DragInt("##MSAA", &UiMsaaCount, 0.01f, 0, 16, "%d",
                     ImGuiSliderFlags_AlwaysClamp);
      ImGui::Spacing();
      if (ImGui::Button("Resize",
                        ImVec2(ImGui::GetContentRegionAvail().x, 0.0f))) {
        Window->SetDimensions(UiWindowWidth, UiWindowHeight, UiMsaaCount,
                              Window->RenderScale);
      }
    }

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Camera")) {
      ImGui::Spacing();
      if (ImGui::Button("Reset")) {
        CameraPosition = Profile::Scene3D::DefaultCameraPosition;
        CameraTarget = Profile::Scene3D::DefaultCameraTarget;
      }
      ImGui::Spacing();
      ImGui::Text("Camera X");
      ImGui::SliderFloat("##cameraX", &CameraPosition.x, -1500.0f, 1500.0f);
      ImGui::Spacing();
      ImGui::Text("Camera Y");
      ImGui::SliderFloat("##cameraY", &CameraPosition.y, -1500.0f, 1500.0f);
      ImGui::Spacing();
      ImGui::Text("Camera Z");
      ImGui::SliderFloat("##cameraZ", &CameraPosition.z, -1500.0f, 1500.0f);

      ImGui::Spacing();
      ImGui::Text("Camera target X");
      ImGui::SliderFloat("##cameraTargetX", &CameraTarget.x, -1500.0f, 1500.0f);
      ImGui::Spacing();
      ImGui::Text("Camera target Y");
      ImGui::SliderFloat("##cameraTargetY", &CameraTarget.y, -1500.0f, 1500.0f);
      ImGui::Spacing();
      ImGui::Text("Camera target Z");
      ImGui::SliderFloat("##cameraTargetZ", &CameraTarget.z, -1500.0f, 1500.0f);
    }

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Light")) {
      ImGui::Spacing();
      ImGui::Checkbox("DarkMode", &Renderer->Scene->DarkMode);
      ImGui::Spacing();
      ImGui::Text("LightPosition.x");
      ImGui::SliderFloat("##lightPosX", &Renderer->Scene->LightPosition.x,
                         -40.0f, 40.0f);
      ImGui::Spacing();
      ImGui::Text("LightPosition.y");
      ImGui::SliderFloat("##lightPosY", &Renderer->Scene->LightPosition.y,
                         -40.0f, 40.0f);
      ImGui::Spacing();
      ImGui::Text("LightPosition.z");
      ImGui::SliderFloat("##lightPosZ", &Renderer->Scene->LightPosition.z,
                         -40.0f, 40.0f);

      ImGui::Spacing();
      float lightTint[] = {UiTintColor.r, UiTintColor.g, UiTintColor.b,
                           UiTintColor.a};
      ImGui::ColorEdit4("Tint", lightTint);
      UiTintColor.r = lightTint[0];
      UiTintColor.g = lightTint[1];
      UiTintColor.b = lightTint[2];
      UiTintColor.a = lightTint[3];

      ImGui::Spacing();
      ImGui::Text("Tint.R");
      ImGui::SliderFloat("##tintR", &UiTintColor.r, 0.0f, 1.0f);
      ImGui::Spacing();
      ImGui::Text("Tint.G");
      ImGui::SliderFloat("##tintG", &UiTintColor.g, -0.0f, 1.0f);
      ImGui::Spacing();
      ImGui::Text("Tint.B");
      ImGui::SliderFloat("##tintB", &UiTintColor.b, -0.0f, 1.0f);
      ImGui::Spacing();
      ImGui::Text("Tint.A");
      ImGui::SliderFloat("##tintA", &UiTintColor.a, -0.0f, 1.0f);
      Renderer->Scene->Tint.r = UiTintColor.r;
      Renderer->Scene->Tint.g = UiTintColor.g;
      Renderer->Scene->Tint.b = UiTintColor.b;
      Renderer->Scene->Tint.a = UiTintColor.a;
    }

    if (Renderer->Scene->Renderables[0]->Status == LS_Loaded) {
      Renderer->Scene->Renderables[0]->IsVisible = true;

      if (ImGui::CollapsingHeader("Background",
                                  ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Spacing();

        const char* comboPreviewValue =
            g_BackgroundModelNames[CurrentBackground];
        if (ImGui::BeginCombo("##backgroundCombo", comboPreviewValue)) {
          for (uint32_t i = 0; i < g_BackgroundModelCount; i++) {
            const bool isSelected = (CurrentBackground == i);
            if (ImGui::Selectable(g_BackgroundModelNames[i], isSelected))
              CurrentBackground = i;
            if (isSelected) ImGui::SetItemDefaultFocus();
          }
          ImGui::EndCombo();
        }

        if (g_BackgroundModelIds[CurrentBackground] !=
            Renderer->Scene->Renderables[0]->StaticModel->Id) {
          Renderer->Scene->Renderables[0]->LoadAsync(
              g_BackgroundModelIds[CurrentBackground]);
        }
      }
    }

    ImGui::Spacing();

    if (Renderer->Scene->Renderables[1]->Status == LS_Loaded) {
      Renderer->Scene->Renderables[1]->IsVisible = true;

      if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Spacing();
        ImGui::Text("Model X");
        ImGui::SliderFloat(
            "##modelX",
            &Renderer->Scene->Renderables[1]->ModelTransform.Position.x, -40.0f,
            40.0f);
        ImGui::Spacing();
        ImGui::Text("Model Y");
        ImGui::SliderFloat(
            "##modelY",
            &Renderer->Scene->Renderables[1]->ModelTransform.Position.y, -40.0f,
            40.0f);
        ImGui::Spacing();
        ImGui::Text("Model Z");
        ImGui::SliderFloat(
            "##modelZ",
            &Renderer->Scene->Renderables[1]->ModelTransform.Position.z, -40.0f,
            40.0f);

        ImGui::Spacing();
        ImGui::Checkbox("Track camera", &TrackCamera);
        if (TrackCamera) {
          Renderer->Scene->Renderables[1]->ModelTransform.SetRotationFromEuler(
              LookAtEulerZYX(
                  Renderer->Scene->Renderables[1]->ModelTransform.Position +
                      Profile::Scene3D::DefaultCameraTarget,
                  CameraPosition));
        } else {
          Renderer->Scene->Renderables[1]->ModelTransform.Rotation =
              glm::quat();
        }

        ImGui::Spacing();

        const char* comboPreviewValue = g_ModelNames[CurrentModel];
        if (ImGui::BeginCombo("##modelCombo", comboPreviewValue)) {
          for (uint32_t i = 0; i < g_ModelCount; i++) {
            const bool isSelected = (CurrentModel == i);
            if (ImGui::Selectable(g_ModelNames[i], isSelected))
              CurrentModel = i;
            if (isSelected) ImGui::SetItemDefaultFocus();
          }
          ImGui::EndCombo();
        }

        if (g_ModelIds[CurrentModel] !=
            Renderer->Scene->Renderables[1]->StaticModel->Id) {
          CurrentAnim = 0;
          Renderer->Scene->Renderables[1]->LoadAsync(g_ModelIds[CurrentModel]);
        }
      }

      if (ImGui::CollapsingHeader("Animation",
                                  ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Spacing();
        ImGui::Checkbox("Playing",
                        &Renderer->Scene->Renderables[1]->Animator.IsPlaying);

        ImGui::Spacing();
        ImGui::Checkbox("Tweening",
                        &Renderer->Scene->Renderables[1]->Animator.Tweening);

        if (Renderer->Scene->Renderables[1]->Animator.CurrentAnimation) {
          ImGui::Spacing();
          ImGui::Text("Loop start");
          ImGui::SliderFloat(
              "##loopStart",
              &Renderer->Scene->Renderables[1]->Animator.LoopStart, 0.0f,
              Renderer->Scene->Renderables[1]->Animator.LoopEnd);
          ImGui::Spacing();
          ImGui::Text("Loop end");
          ImGui::SliderFloat("##loopEnd",
                             &Renderer->Scene->Renderables[1]->Animator.LoopEnd,
                             0.0f,
                             Renderer->Scene->Renderables[1]
                                 ->Animator.CurrentAnimation->Duration);

          // Nice hack
          float backup = Renderer->Scene->Renderables[1]->Animator.CurrentTime;
          ImGui::Spacing();
          ImGui::Text("Current time");
          ImGui::SliderFloat(
              "##currentTime",
              &Renderer->Scene->Renderables[1]->Animator.CurrentTime, 0.0f,
              Renderer->Scene->Renderables[1]
                  ->Animator.CurrentAnimation->Duration);
          if (backup != Renderer->Scene->Renderables[1]->Animator.CurrentTime) {
            Renderer->Scene->Renderables[1]->Animator.Seek(
                Renderer->Scene->Renderables[1]->Animator.CurrentTime);
          }

          ImGui::Spacing();
          ImGui::Checkbox("OneShot",
                          &Renderer->Scene->Renderables[1]->Animator.OneShot);

          ImGui::Spacing();
          const char* comboPreviewValue =
              Renderer->Scene->Renderables[1]
                  ->StaticModel->AnimationNames[CurrentAnim];
          if (ImGui::BeginCombo("##animationCombo", comboPreviewValue)) {
            for (uint32_t i = 0;
                 i <
                 Renderer->Scene->Renderables[1]->StaticModel->AnimationCount;
                 i++) {
              const bool isSelected = (CurrentAnim == i);
              if (ImGui::Selectable(Renderer->Scene->Renderables[1]
                                        ->StaticModel->AnimationNames[i],
                                    isSelected))
                CurrentAnim = i;
              if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
          }
          ImGui::Spacing();
          if (ImGui::Button("Switch")) {
            Renderer->Scene->Renderables[1]->SwitchAnimation(
                Renderer->Scene->Renderables[1]
                    ->StaticModel->AnimationIds[CurrentAnim],
                0.66f);
          }
        }
      }
    }

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("BGM", ImGuiTreeNodeFlags_DefaultOpen)) {
      const char* comboPreviewValue = BgmNames[CurrentBgm];

      ImGui::Spacing();
      if (ImGui::BeginCombo("##bgmCombo", comboPreviewValue)) {
        for (uint32_t i = 0; i < BgmCount; i++) {
          const bool isSelected = (CurrentBgm == i);
          if (ImGui::Selectable(BgmNames[i], isSelected)) CurrentBgm = i;
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::Spacing();
      bool bgmLoop = (bool)BgmLoop;
      ImGui::Checkbox("Loop (takes effect on switch)", &bgmLoop);
      BgmLoop = (int)bgmLoop;
      ImGui::Spacing();
      if (ImGui::Button("Switch##switchBgm")) {
        BgmChangeQueued = true;
        Audio::Channels[Audio::AC_BGM0]->Stop(BgmFadeOut);
      }
      ImGui::Spacing();
      ImGui::Text("Master volume");
      ImGui::SliderFloat("##masterVolume", &Audio::MasterVolume, 0.0f, 1.0f);
      ImGui::Spacing();
      ImGui::Text("BGM volume");
      ImGui::SliderFloat("##bgmVolume", &Audio::GroupVolumes[Audio::ACG_BGM],
                         0.0f, 1.0f);
      ImGui::Spacing();
      ImGui::Text("Fade out duration");
      ImGui::SliderFloat("##fadeOutDur", &BgmFadeOut, 0.0f, 5.0f);
      ImGui::Spacing();
      ImGui::Text("Fade in duration");
      ImGui::SliderFloat("##fadeInDur", &BgmFadeIn, 0.0f, 5.0f);
    }

    ImGui::Spacing();
  }

  ImGui::End();
#else
  if (Renderer->Scene->Renderables[0]->Status == LS_Loaded) {
    Renderer->Scene->Renderables[0]->IsVisible = true;
  }

  if (Renderer->Scene->Renderables[1]->Status == LS_Loaded) {
    Renderer->Scene->Renderables[1]->IsVisible = true;
  }
#endif

  if (BgmChangeQueued &&
      Audio::Channels[Audio::AC_BGM0]->State == Audio::ACS_Stopped) {
    Audio::Channels[Audio::AC_BGM0]->Play("bgm", BgmIds[CurrentBgm], BgmLoop,
                                          BgmFadeIn);
    BgmChangeQueued = false;
  }

  Renderer->Scene->MainCamera.CameraTransform.Position = CameraPosition;
  Renderer->Scene->MainCamera.LookAt(CameraTarget);
}

static void EnumerateBgm() {
  std::map<uint32_t, std::string> listing;
  IoError err = Io::VfsListFiles("bgm", listing);
  if (err != IoError_OK) {
    ImpLog(LL_Warning, LC_General,
           "Failed to list BGM archive files, stopping enumeration!\n");
    return;
  }

  BgmCount = (uint32_t)listing.size();

  BgmNames = (char**)malloc(BgmCount * sizeof(char*));
  BgmIds = (uint32_t*)malloc(BgmCount * sizeof(uint32_t));

  uint32_t i = 0;
  for (auto const& file : listing) {
    BgmIds[i] = file.first;
    BgmNames[i] = strdup(file.second.c_str());
    i++;
  }
}

}  // namespace ModelViewer
}  // namespace Impacto