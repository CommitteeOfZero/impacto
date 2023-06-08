#include "modelviewer.h"
#include "game.h"

//#include "window.h"
#include "renderer/renderer.h"
#include "audio/audiosystem.h"
#include "audio/audiostream.h"
#include "audio/audiochannel.h"
#include "renderer/3d/scene.h"
#include "renderer/3d/model.h"

#include "profile/scene3d.h"

namespace Impacto {
namespace ModelViewer {

static void EnumerateBgm();

static glm::vec3 CameraPosition;
static glm::vec3 CameraTarget;
static int TrackCamera;
static nk_colorf UiTintColor;
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

// FPS counter
static float LastTime;
static int Frames;
static float FPS;

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

  UiTintColor = {0.784f, 0.671f, 0.6f, 0.9f};

  Renderer->Scene->Renderables[0]->LoadAsync(g_BackgroundModelIds[0]);
  Renderer->Scene->Renderables[1]->LoadAsync(g_ModelIds[0]);

  Renderer->Scene->Tint = glm::vec4(0.784f, 0.671f, 0.6f, 0.9f);
  Renderer->Scene->LightPosition = glm::vec3(-2.85f, 16.68f, 6.30f);
  Renderer->Scene->DarkMode = false;

  LastTime = (float)((double)SDL_GetPerformanceCounter() /
                     (double)SDL_GetPerformanceFrequency());
  Frames = 0;
  FPS = 0.0f;
}

void Update(float dt) {
  if (Window->WindowDimensionsChanged) {
    UiWindowWidth = Window->WindowWidth;
    UiWindowHeight = Window->WindowHeight;
    UiMsaaCount = Window->MsaaCount;
  }

  if (Renderer->NuklearSupported &&
      nk_begin(Renderer->Nk, "Scene",
               nk_rect(20, 20, 300, Window->WindowHeight - 40),
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

    nk_layout_row_dynamic(Renderer->Nk, 24, 1);
    char buffer[32];  // whatever
    snprintf(buffer, 32, "FPS: %02.2f", FPS);
    nk_label(Renderer->Nk, buffer, NK_TEXT_ALIGN_CENTERED);

    if (nk_tree_push(Renderer->Nk, NK_TREE_TAB, "Window", NK_MINIMIZED)) {
      nk_layout_row_dynamic(Renderer->Nk, 24, 1);

      nk_property_int(Renderer->Nk, "Width", 0, &UiWindowWidth, 8192, 0, 0.0f);
      nk_property_int(Renderer->Nk, "Height", 0, &UiWindowHeight, 8192, 0,
                      0.0f);
      nk_property_int(Renderer->Nk, "MSAA", 0, &UiMsaaCount, 16, 0, 0);

      if (nk_button_label(Renderer->Nk, "Resize")) {
        Window->SetDimensions(UiWindowWidth, UiWindowHeight, UiMsaaCount,
                              Window->RenderScale);
      }

      nk_tree_pop(Renderer->Nk);
    }

    if (nk_tree_push(Renderer->Nk, NK_TREE_TAB, "Camera", NK_MINIMIZED)) {
      nk_layout_row_dynamic(Renderer->Nk, 24, 1);

      if (nk_button_label(Renderer->Nk, "Reset")) {
        CameraPosition = Profile::Scene3D::DefaultCameraPosition;
        CameraTarget = Profile::Scene3D::DefaultCameraTarget;
      }

      nk_property_float(Renderer->Nk, "Camera X", -1500.0f, &CameraPosition.x,
                        1500.0f, 1.0f, 0.2f);
      nk_property_float(Renderer->Nk, "Camera Y", -1500.0f, &CameraPosition.y,
                        1500.0f, 1.0f, 0.2f);
      nk_property_float(Renderer->Nk, "Camera Z", -1500.0f, &CameraPosition.z,
                        1500.0f, 1.0f, 0.2f);

      nk_property_float(Renderer->Nk, "Camera target X", -1500.0f,
                        &CameraTarget.x, 1500.0f, 1.0f, 0.2f);
      nk_property_float(Renderer->Nk, "Camera target Y", -1500.0f,
                        &CameraTarget.y, 1500.0f, 1.0f, 0.2f);
      nk_property_float(Renderer->Nk, "Camera target Z", -1500.0f,
                        &CameraTarget.z, 1500.0f, 1.0f, 0.2f);

      nk_tree_pop(Renderer->Nk);
    }

    if (nk_tree_push(Renderer->Nk, NK_TREE_TAB, "Light", NK_MINIMIZED)) {
      nk_layout_row_dynamic(Renderer->Nk, 24, 1);

      int darkMode = (int)Renderer->Scene->DarkMode;
      nk_checkbox_label(Renderer->Nk, "DarkMode", &darkMode);
      Renderer->Scene->DarkMode = (bool)darkMode;

      nk_property_float(Renderer->Nk, "LightPosition.x", -40.0f,
                        &Renderer->Scene->LightPosition.x, 40.0f, 1.0f, 0.02f);
      nk_property_float(Renderer->Nk, "LightPosition.y", -40.0f,
                        &Renderer->Scene->LightPosition.y, 40.0f, 1.0f, 0.02f);
      nk_property_float(Renderer->Nk, "LightPosition.z", -40.0f,
                        &Renderer->Scene->LightPosition.z, 40.0f, 1.0f, 0.02f);

      nk_layout_row_dynamic(Renderer->Nk, 120, 1);
      nk_color_pick(Renderer->Nk, &UiTintColor, NK_RGBA);
      nk_layout_row_dynamic(Renderer->Nk, 24, 1);
      nk_property_float(Renderer->Nk, "Tint.R", 0.0f, &UiTintColor.r, 1.0f,
                        0.01f, 0.005f);
      nk_property_float(Renderer->Nk, "Tint.G", 0.0f, &UiTintColor.g, 1.0f,
                        0.01f, 0.005f);
      nk_property_float(Renderer->Nk, "Tint.B", 0.0f, &UiTintColor.b, 1.0f,
                        0.01f, 0.005f);
      nk_property_float(Renderer->Nk, "Tint.A", 0.0f, &UiTintColor.a, 1.0f,
                        0.01f, 0.005f);
      Renderer->Scene->Tint.r = UiTintColor.r;
      Renderer->Scene->Tint.g = UiTintColor.g;
      Renderer->Scene->Tint.b = UiTintColor.b;
      Renderer->Scene->Tint.a = UiTintColor.a;

      nk_tree_pop(Renderer->Nk);
    }

    if (Renderer->Scene->Renderables[0]->Status == LS_Loaded) {
      Renderer->Scene->Renderables[0]->IsVisible = true;

      if (nk_tree_push(Renderer->Nk, NK_TREE_TAB, "Background", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(Renderer->Nk, 24, 1);

        CurrentBackground = nk_combo(
            Renderer->Nk, (const char**)g_BackgroundModelNames,
            g_BackgroundModelCount, CurrentBackground, 24, nk_vec2(200, 200));
        if (g_BackgroundModelIds[CurrentBackground] !=
            Renderer->Scene->Renderables[0]->StaticModel->Id) {
          Renderer->Scene->Renderables[0]->LoadAsync(
              g_BackgroundModelIds[CurrentBackground]);
        }

        nk_tree_pop(Renderer->Nk);
      }
    }

    if (Renderer->Scene->Renderables[1]->Status == LS_Loaded) {
      Renderer->Scene->Renderables[1]->IsVisible = true;

      if (nk_tree_push(Renderer->Nk, NK_TREE_TAB, "Model", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(Renderer->Nk, 24, 1);

        nk_property_float(
            Renderer->Nk, "Model X", -40.0f,
            &Renderer->Scene->Renderables[1]->ModelTransform.Position.x, 40.0f,
            1.0f, 0.02f);
        nk_property_float(
            Renderer->Nk, "Model Y", 0.0f,
            &Renderer->Scene->Renderables[1]->ModelTransform.Position.y, 40.0f,
            1.0f, 0.02f);
        nk_property_float(
            Renderer->Nk, "Model Z", -40.0f,
            &Renderer->Scene->Renderables[1]->ModelTransform.Position.z, 40.0f,
            1.0f, 0.02f);

        nk_checkbox_label(Renderer->Nk, "Track camera", &TrackCamera);
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

        CurrentModel =
            nk_combo(Renderer->Nk, (const char**)g_ModelNames, g_ModelCount,
                     CurrentModel, 24, nk_vec2(200, 200));
        if (g_ModelIds[CurrentModel] !=
            Renderer->Scene->Renderables[1]->StaticModel->Id) {
          CurrentAnim = 0;
          Renderer->Scene->Renderables[1]->LoadAsync(g_ModelIds[CurrentModel]);
        }

        nk_tree_pop(Renderer->Nk);
      }

      if (nk_tree_push(Renderer->Nk, NK_TREE_TAB, "Animation", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(Renderer->Nk, 24, 1);

        int isPlaying =
            (int)Renderer->Scene->Renderables[1]->Animator.IsPlaying;
        nk_checkbox_label(Renderer->Nk, "Playing", &isPlaying);
        Renderer->Scene->Renderables[1]->Animator.IsPlaying = (bool)isPlaying;

        int tweening = (int)Renderer->Scene->Renderables[1]->Animator.Tweening;
        nk_checkbox_label(Renderer->Nk, "Tweening", &tweening);
        Renderer->Scene->Renderables[1]->Animator.Tweening = (bool)tweening;

        if (Renderer->Scene->Renderables[1]->Animator.CurrentAnimation) {
          nk_property_float(
              Renderer->Nk, "Loop start", 0.0f,
              &Renderer->Scene->Renderables[1]->Animator.LoopStart,
              Renderer->Scene->Renderables[1]->Animator.LoopEnd, 1.0f, 0.2f);
          nk_property_float(Renderer->Nk, "Loop end", 0.0f,
                            &Renderer->Scene->Renderables[1]->Animator.LoopEnd,
                            Renderer->Scene->Renderables[1]
                                ->Animator.CurrentAnimation->Duration,
                            1.0f, 0.2f);

          // Nice hack
          float backup = Renderer->Scene->Renderables[1]->Animator.CurrentTime;
          nk_property_float(
              Renderer->Nk, "Current time", 0.0f,
              &Renderer->Scene->Renderables[1]->Animator.CurrentTime,
              Renderer->Scene->Renderables[1]
                  ->Animator.CurrentAnimation->Duration,
              1.0f, 0.2f);
          if (backup != Renderer->Scene->Renderables[1]->Animator.CurrentTime) {
            Renderer->Scene->Renderables[1]->Animator.Seek(
                Renderer->Scene->Renderables[1]->Animator.CurrentTime);
          }

          int oneShot = (int)Renderer->Scene->Renderables[1]->Animator.OneShot;
          nk_checkbox_label(Renderer->Nk, "OneShot", &oneShot);
          Renderer->Scene->Renderables[1]->Animator.OneShot = oneShot;

          CurrentAnim = nk_combo(
              Renderer->Nk,
              (const char**)Renderer->Scene->Renderables[1]
                  ->StaticModel->AnimationNames,
              Renderer->Scene->Renderables[1]->StaticModel->AnimationCount,
              CurrentAnim, 24, nk_vec2(200, 200));

          if (nk_button_label(Renderer->Nk, "Switch")) {
            Renderer->Scene->Renderables[1]->SwitchAnimation(
                Renderer->Scene->Renderables[1]
                    ->StaticModel->AnimationIds[CurrentAnim],
                0.66f);
          }
        }

        nk_tree_pop(Renderer->Nk);
      }
    }

    if (nk_tree_push(Renderer->Nk, NK_TREE_TAB, "BGM", NK_MAXIMIZED)) {
      nk_layout_row_dynamic(Renderer->Nk, 24, 1);

      CurrentBgm = nk_combo(Renderer->Nk, (const char**)BgmNames, BgmCount,
                            CurrentBgm, 24, nk_vec2(200, 200));
      nk_checkbox_label(Renderer->Nk, "Loop (takes effect on switch)",
                        &BgmLoop);
      if (nk_button_label(Renderer->Nk, "Switch")) {
        BgmChangeQueued = true;
        Audio::Channels[Audio::AC_BGM0].Stop(BgmFadeOut);
      }

      nk_property_float(Renderer->Nk, "Master volume", 0.0f,
                        &Audio::MasterVolume, 1.0f, 0.01f, 0.01f);
      nk_property_float(Renderer->Nk, "BGM volume", 0.0f,
                        &Audio::GroupVolumes[Audio::ACG_BGM], 1.0f, 0.01f,
                        0.01f);

      nk_property_float(Renderer->Nk, "Fade out duration", 0.0f, &BgmFadeOut,
                        5.0f, 0.1f, 0.02f);
      nk_property_float(Renderer->Nk, "Fade in duration", 0.0f, &BgmFadeIn,
                        5.0f, 0.1f, 0.02f);

      nk_tree_pop(Renderer->Nk);
    }
  } else {
    if (Renderer->Scene->Renderables[0]->Status == LS_Loaded) {
      Renderer->Scene->Renderables[0]->IsVisible = true;
    }

    if (Renderer->Scene->Renderables[1]->Status == LS_Loaded) {
      Renderer->Scene->Renderables[1]->IsVisible = true;
    }
  }
  if (Renderer->NuklearSupported) nk_end(Renderer->Nk);

  if (BgmChangeQueued &&
      Audio::Channels[Audio::AC_BGM0].State == Audio::ACS_Stopped) {
    Io::InputStream* stream;
    Io::VfsOpen("bgm", BgmIds[CurrentBgm], &stream);
    Audio::Channels[Audio::AC_BGM0].Play(Audio::AudioStream::Create(stream),
                                         BgmLoop, BgmFadeIn);
    BgmChangeQueued = false;
  }

  Renderer->Scene->MainCamera.CameraTransform.Position = CameraPosition;
  Renderer->Scene->MainCamera.LookAt(CameraTarget);
}

static void EnumerateBgm() {
  std::map<uint32_t, std::string> listing;
  IoError err = Io::VfsListFiles("bgm", listing);

  BgmCount = listing.size();

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