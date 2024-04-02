#include "io/io.h"
#include "log.h"
#include "modelviewer.h"
#include "game.h"

// #include "window.h"
#include "renderer/renderer.h"
#include "audio/audiosystem.h"
#include "audio/audiostream.h"
#include "audio/audiochannel.h"
#include "background2d.h"
#include "character2d.h"

#include <cstdint>

namespace Impacto {
namespace CharacterViewer {

static void EnumerateBgm();
static void EnumerateBackgrounds();
static void EnumerateCharacters();

static uint32_t CurrentCharacter;
static uint32_t CurrentBackground;
static uint32_t CurrentBgm;
static nk_colorf UiTintColor;
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

static char** BackgroundNames = 0;
static uint32_t* BackgroundIds = 0;
static uint32_t BackgroundCount = 0;

static char** CharacterNames = 0;
static uint32_t* CharacterIds = 0;
static uint32_t CharacterCount = 0;

// FPS counter
static float LastTime;
static int Frames;
static float FPS;

void Init() {
  EnumerateBgm();
  EnumerateBackgrounds();
  EnumerateCharacters();

  CurrentCharacter = 0;
  CurrentBackground = 0;
  CurrentBgm = 0;
  BgmChangeQueued = false;

  BgmFadeOut = 0.0f;
  BgmFadeIn = 0.0f;
  BgmLoop = true;

  UiTintColor = {0.784f, 0.671f, 0.6f, 0.9f};

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

#ifndef IMPACTO_DISABLE_OPENGL
  if (Renderer->NuklearSupported &&
      nk_begin(Renderer->Nk, "Scene",
               nk_rect(20.0f, 20.0f, 300.0f, Window->WindowHeight - 40.0f),
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

    if (nk_tree_push(Renderer->Nk, NK_TREE_TAB, "Background", NK_MAXIMIZED)) {
      nk_layout_row_dynamic(Renderer->Nk, 24, 1);

      uint32_t LastBackground = CurrentBackground;

      CurrentBackground =
          nk_combo(Renderer->Nk, (const char**)BackgroundNames, BackgroundCount,
                   CurrentBackground, 24, nk_vec2(200, 200));

      if (LastBackground != CurrentBackground ||
          Characters2D[0].Status == LS_Unloaded) {
        Backgrounds2D[0]->LoadAsync(BackgroundIds[CurrentBackground]);
      }

      nk_tree_pop(Renderer->Nk);
    }

    if (nk_tree_push(Renderer->Nk, NK_TREE_TAB, "Character", NK_MAXIMIZED)) {
      nk_layout_row_dynamic(Renderer->Nk, 24, 1);

      uint32_t LastCharacter = CurrentCharacter;

      CurrentCharacter =
          nk_combo(Renderer->Nk, (const char**)CharacterNames, CharacterCount,
                   CurrentCharacter, 24, nk_vec2(200, 200));

      if (LastCharacter != CurrentCharacter ||
          Characters2D[0].Status == LS_Unloaded) {
        Characters2D[0].LoadAsync(CharacterIds[CurrentCharacter] | 0x10000);
      }
      if (Characters2D[0].Status == LS_Loaded) Characters2D[0].Show = true;

      nk_property_float(Renderer->Nk, "Character X", -10000.0f,
                        &Characters2D[0].OffsetX, 10000.0f, 1.0f, 5.0f);
      nk_property_float(Renderer->Nk, "Character Y", -10000.0f,
                        &Characters2D[0].OffsetY, 10000.0f, 1.0f, 5.0f);

      Characters2D[0].Face >>= 16;
      nk_property_int(Renderer->Nk, "Character Face", 1, &Characters2D[0].Face,
                      20, 1, 1.0f);
      Characters2D[0].Face <<= 16;

      nk_property_int(Renderer->Nk, "Character Eye", 0,
                      &Characters2D[0].EyeFrame, 10, 1, 1.0f);

      nk_property_int(Renderer->Nk, "Character Lip", 0,
                      &Characters2D[0].LipFrame, 10, 1, 1.0f);

      nk_tree_pop(Renderer->Nk);
    }

    if (nk_tree_push(Renderer->Nk, NK_TREE_TAB, "BGM", NK_MAXIMIZED)) {
      nk_layout_row_dynamic(Renderer->Nk, 24, 1);

      CurrentBgm = nk_combo(Renderer->Nk, (const char**)BgmNames, BgmCount,
                            CurrentBgm, 24, nk_vec2(200, 200));
      nk_checkbox_label(Renderer->Nk, "Loop (takes effect on switch)",
                        &BgmLoop);
      if (nk_button_label(Renderer->Nk, "Switch")) {
        BgmChangeQueued = true;
        Audio::Channels[Audio::AC_BGM0]->Stop(BgmFadeOut);
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
#endif
    if (Characters2D[0].Status == LS_Unloaded) {
      Backgrounds2D[0]->LoadAsync(BackgroundIds[0]);
    }

    if (Characters2D[0].Status == LS_Unloaded) {
      Characters2D[0].LoadAsync(CharacterIds[0] | 0x10000);
    }

    if (Characters2D[0].Status == LS_Loaded) Characters2D[0].Show = true;
#ifndef IMPACTO_DISABLE_OPENGL
  }
  if (Renderer->NuklearSupported) nk_end(Renderer->Nk);
#endif

  if (BgmChangeQueued &&
      Audio::Channels[Audio::AC_BGM0]->State == Audio::ACS_Stopped) {
    Audio::Channels[Audio::AC_BGM0]->Play("bgm", BgmIds[CurrentBgm], BgmLoop,
                                          BgmFadeIn);
    BgmChangeQueued = false;
  }
}

static void EnumerateBackgrounds() {
  std::map<uint32_t, std::string> listing;
  IoError err = Io::VfsListFiles("bg", listing);
  if (err != IoError_OK) {
    ImpLog(LL_Warning, LC_General,
           "Failed to open backgrounds archive, aborting enumeration!\n");
    return;
  }

  BackgroundCount = (uint32_t)listing.size();

  BackgroundNames = (char**)malloc(BackgroundCount * sizeof(char*));
  BackgroundIds = (uint32_t*)malloc(BackgroundCount * sizeof(uint32_t));

  uint32_t i = 0;
  for (auto const& file : listing) {
    BackgroundIds[i] = file.first;
    BackgroundNames[i] = strdup(file.second.c_str());
    i++;
  }
}

static void EnumerateCharacters() {
  std::map<uint32_t, std::string> listing;
  IoError err = Io::VfsListFiles("chara", listing);
  if (err != IoError_OK) {
    ImpLog(LL_Warning, LC_General,
           "Failed to open character archive, aborting enumeration!\n");
    return;
  }

  CharacterCount = (uint32_t)listing.size() / 2;

  CharacterNames = (char**)malloc(CharacterCount * sizeof(char*));
  CharacterIds = (uint32_t*)malloc(CharacterCount * sizeof(uint32_t));

  uint32_t i = 0;
  for (auto const& file : listing) {
    if (file.first % 2 == 0) {
      CharacterIds[i] = file.first;
      CharacterNames[i] = strdup(file.second.c_str());
      i++;
    }
  }
}

static void EnumerateBgm() {
  std::map<uint32_t, std::string> listing;
  IoError err = Io::VfsListFiles("bgm", listing);
  if (err != IoError_OK) {
    ImpLog(LL_Warning, LC_General,
           "Failed to open BGM archive, aborting enumeration!\n");
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

}  // namespace CharacterViewer
}  // namespace Impacto