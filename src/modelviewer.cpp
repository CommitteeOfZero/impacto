#include "modelviewer.h"
#include "game.h"

#include "window.h"
#include "audio/audiosystem.h"
#include "audio/audiostream.h"
#include "audio/audiochannel.h"
#include "3d/scene.h"
#include "3d/model.h"

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

  CameraPosition = glm::vec3(0.0f, 12.5f, 23.0f);
  CameraTarget = glm::vec3(0.0f, 12.5f, 0.0f);

  CurrentModel = 0;
  CurrentAnim = 0;
  CurrentBackground = 0;
  CurrentBgm = 0;
  BgmChangeQueued = false;

  BgmFadeOut = 0.0f;
  BgmFadeIn = 0.0f;
  BgmLoop = true;

  UiTintColor = {0.784f, 0.671f, 0.6f, 0.9f};

  Scene3D::Backgrounds[0].LoadAsync(g_BackgroundModelIds[0]);
  Scene3D::Characters[0].LoadAsync(g_ModelIds[0]);

  Scene3D::Tint = glm::vec4(0.784f, 0.671f, 0.6f, 0.9f);
  Scene3D::LightPosition = glm::vec3(-2.85f, 16.68f, 6.30f);
  Scene3D::DarkMode = false;

  LastTime = (float)((double)SDL_GetPerformanceCounter() /
                     (double)SDL_GetPerformanceFrequency());
  Frames = 0;
  FPS = 0.0f;
}

void Update(float dt) {
  if (Window::WindowDimensionsChanged) {
    UiWindowWidth = Window::WindowWidth;
    UiWindowHeight = Window::WindowHeight;
    UiMsaaCount = Window::MsaaCount;
  }

  if (nk_begin(Nk, "Scene", nk_rect(20, 20, 300, Window::WindowHeight - 40),
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

    nk_layout_row_dynamic(Nk, 24, 1);
    char buffer[32];  // whatever
    snprintf(buffer, 32, "FPS: %02.2f", FPS);
    nk_label(Nk, buffer, NK_TEXT_ALIGN_CENTERED);

    if (nk_tree_push(Nk, NK_TREE_TAB, "Window", NK_MINIMIZED)) {
      nk_layout_row_dynamic(Nk, 24, 1);

      nk_property_int(Nk, "Width", 0, &UiWindowWidth, 8192, 0, 0.0f);
      nk_property_int(Nk, "Height", 0, &UiWindowHeight, 8192, 0, 0.0f);
      nk_property_int(Nk, "MSAA", 0, &UiMsaaCount, 16, 0, 0);

      if (nk_button_label(Nk, "Resize")) {
        Window::SetDimensions(UiWindowWidth, UiWindowHeight, UiMsaaCount,
                              Window::RenderScale);
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

      int darkMode = (int)Scene3D::DarkMode;
      nk_checkbox_label(Nk, "DarkMode", &darkMode);
      Scene3D::DarkMode = (bool)darkMode;

      nk_property_float(Nk, "LightPosition.x", -40.0f,
                        &Scene3D::LightPosition.x, 40.0f, 1.0f, 0.02f);
      nk_property_float(Nk, "LightPosition.y", -40.0f,
                        &Scene3D::LightPosition.y, 40.0f, 1.0f, 0.02f);
      nk_property_float(Nk, "LightPosition.z", -40.0f,
                        &Scene3D::LightPosition.z, 40.0f, 1.0f, 0.02f);

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
      Scene3D::Tint.r = UiTintColor.r;
      Scene3D::Tint.g = UiTintColor.g;
      Scene3D::Tint.b = UiTintColor.b;
      Scene3D::Tint.a = UiTintColor.a;

      nk_tree_pop(Nk);
    }

    if (Scene3D::Backgrounds[0].Status == LS_Loaded) {
      Scene3D::Backgrounds[0].IsVisible = true;

      if (nk_tree_push(Nk, NK_TREE_TAB, "Background", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(Nk, 24, 1);

        CurrentBackground = nk_combo(Nk, (const char**)g_BackgroundModelNames,
                                     g_BackgroundModelCount, CurrentBackground,
                                     24, nk_vec2(200, 200));
        if (g_BackgroundModelIds[CurrentBackground] !=
            Scene3D::Backgrounds[0].StaticModel->Id) {
          Scene3D::Backgrounds[0].LoadAsync(
              g_BackgroundModelIds[CurrentBackground]);
        }

        nk_tree_pop(Nk);
      }
    }

    if (Scene3D::Characters[0].Status == LS_Loaded) {
      Scene3D::Characters[0].IsVisible = true;

      if (nk_tree_push(Nk, NK_TREE_TAB, "Model", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(Nk, 24, 1);

        nk_property_float(Nk, "Model X", -40.0f,
                          &Scene3D::Characters[0].ModelTransform.Position.x,
                          40.0f, 1.0f, 0.02f);
        nk_property_float(Nk, "Model Y", 0.0f,
                          &Scene3D::Characters[0].ModelTransform.Position.y,
                          40.0f, 1.0f, 0.02f);
        nk_property_float(Nk, "Model Z", -40.0f,
                          &Scene3D::Characters[0].ModelTransform.Position.z,
                          40.0f, 1.0f, 0.02f);

        nk_checkbox_label(Nk, "Track camera", &TrackCamera);
        if (TrackCamera) {
          Scene3D::Characters[0].ModelTransform.SetRotationFromEuler(
              LookAtEulerZYX(Scene3D::Characters[0].ModelTransform.Position +
                                 glm::vec3(0.0f, 12.5f, 0.0f),
                             CameraPosition));
        } else {
          Scene3D::Characters[0].ModelTransform.Rotation = glm::quat();
        }

        CurrentModel = nk_combo(Nk, (const char**)g_ModelNames, g_ModelCount,
                                CurrentModel, 24, nk_vec2(200, 200));
        if (g_ModelIds[CurrentModel] !=
            Scene3D::Characters[0].StaticModel->Id) {
          CurrentAnim = 0;
          Scene3D::Characters[0].LoadAsync(g_ModelIds[CurrentModel]);
        }

        nk_tree_pop(Nk);
      }

      if (nk_tree_push(Nk, NK_TREE_TAB, "Animation", NK_MAXIMIZED)) {
        nk_layout_row_dynamic(Nk, 24, 1);

        int isPlaying = (int)Scene3D::Characters[0].Animator.IsPlaying;
        nk_checkbox_label(Nk, "Playing", &isPlaying);
        Scene3D::Characters[0].Animator.IsPlaying = (bool)isPlaying;

        int tweening = (int)Scene3D::Characters[0].Animator.Tweening;
        nk_checkbox_label(Nk, "Tweening", &tweening);
        Scene3D::Characters[0].Animator.Tweening = (bool)tweening;

        if (Scene3D::Characters[0].Animator.CurrentAnimation) {
          nk_property_float(Nk, "Loop start", 0.0f,
                            &Scene3D::Characters[0].Animator.LoopStart,
                            Scene3D::Characters[0].Animator.LoopEnd, 1.0f,
                            0.2f);
          nk_property_float(
              Nk, "Loop end", 0.0f, &Scene3D::Characters[0].Animator.LoopEnd,
              Scene3D::Characters[0].Animator.CurrentAnimation->Duration, 1.0f,
              0.2f);

          // Nice hack
          float backup = Scene3D::Characters[0].Animator.CurrentTime;
          nk_property_float(
              Nk, "Current time", 0.0f,
              &Scene3D::Characters[0].Animator.CurrentTime,
              Scene3D::Characters[0].Animator.CurrentAnimation->Duration, 1.0f,
              0.2f);
          if (backup != Scene3D::Characters[0].Animator.CurrentTime) {
            Scene3D::Characters[0].Animator.Seek(
                Scene3D::Characters[0].Animator.CurrentTime);
          }

          CurrentAnim = nk_combo(
              Nk,
              (const char**)Scene3D::Characters[0].StaticModel->AnimationNames,
              Scene3D::Characters[0].StaticModel->AnimationCount, CurrentAnim,
              24, nk_vec2(200, 200));
          if (Scene3D::Characters[0].StaticModel->AnimationIds[CurrentAnim] !=
              Scene3D::Characters[0].Animator.CurrentAnimation->Id) {
            Scene3D::Characters[0].SwitchAnimation(
                Scene3D::Characters[0].StaticModel->AnimationIds[CurrentAnim],
                0.66f);
          }
        }

        nk_tree_pop(Nk);
      }
    }

    if (nk_tree_push(Nk, NK_TREE_TAB, "BGM", NK_MAXIMIZED)) {
      nk_layout_row_dynamic(Nk, 24, 1);

      CurrentBgm = nk_combo(Nk, (const char**)BgmNames, BgmCount, CurrentBgm,
                            24, nk_vec2(200, 200));
      nk_checkbox_label(Nk, "Loop (takes effect on switch)", &BgmLoop);
      if (nk_button_label(Nk, "Switch")) {
        BgmChangeQueued = true;
        Audio::Channels[Audio::AC_BGM0].Stop(BgmFadeOut);
      }

      nk_property_float(Nk, "Master volume", 0.0f, &Audio::MasterVolume, 1.0f,
                        0.01f, 0.01f);
      nk_property_float(Nk, "BGM volume", 0.0f,
                        &Audio::GroupVolumes[Audio::ACG_BGM], 1.0f, 0.01f,
                        0.01f);

      nk_property_float(Nk, "Fade out duration", 0.0f, &BgmFadeOut, 5.0f, 0.1f,
                        0.02f);
      nk_property_float(Nk, "Fade in duration", 0.0f, &BgmFadeIn, 5.0f, 0.1f,
                        0.02f);

      nk_tree_pop(Nk);
    }
  }
  nk_end(Nk);

  if (BgmChangeQueued &&
      Audio::Channels[Audio::AC_BGM0].State == Audio::ACS_Stopped) {
    SDL_RWops* stream;
    Game::BgmArchive->Open(BgmIds[CurrentBgm], &stream);
    Audio::Channels[Audio::AC_BGM0].Play(Audio::AudioStream::Create(stream),
                                         BgmLoop, BgmFadeIn);
    BgmChangeQueued = false;
  }

  Scene3D::MainCamera.CameraTransform.Position = CameraPosition;
  Scene3D::MainCamera.LookAt(CameraTarget);
}

static void EnumerateBgm() {
  uint32_t iterator;
  VfsFileInfo info;

  IoError err = Game::BgmArchive->EnumerateStart(&iterator, &info);
  while (err == IoError_OK) {
    BgmCount++;
    err = Game::BgmArchive->EnumerateNext(&iterator, &info);
  }

  BgmNames = (char**)malloc(BgmCount * sizeof(char*));
  BgmIds = (uint32_t*)malloc(BgmCount * sizeof(uint32_t));

  uint32_t i = 0;
  err = Game::BgmArchive->EnumerateStart(&iterator, &info);
  while (err == IoError_OK) {
    BgmIds[i] = info.Id;
    BgmNames[i] = strdup(info.Name);
    i++;
    err = Game::BgmArchive->EnumerateNext(&iterator, &info);
  }
}

}  // namespace ModelViewer
}  // namespace Impacto