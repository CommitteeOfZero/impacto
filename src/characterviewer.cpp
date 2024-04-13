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
static int UiWindowWidth;
static int UiWindowHeight;
static int UiMsaaCount;

static char** BgmNames = 0;
static uint32_t* BgmIds = 0;
static uint32_t BgmCount = 0;
static bool BgmChangeQueued;

static float BgmFadeOut;
static float BgmFadeIn;
static bool BgmLoop;

static char** BackgroundNames = 0;
static uint32_t* BackgroundIds = 0;
static uint32_t BackgroundCount = 0;

static char** CharacterNames = 0;
static uint32_t* CharacterIds = 0;
static uint32_t CharacterCount = 0;

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
      ImGui::DragInt("##Width", &UiWindowWidth, 0.005f, 0, 8192, "%d",
                     ImGuiSliderFlags_AlwaysClamp);
      ImGui::Spacing();
      ImGui::Text("Height");
      ImGui::DragInt("##Height", &UiWindowHeight, 0.005f, 0, 8192, "%d",
                     ImGuiSliderFlags_AlwaysClamp);
      ImGui::Spacing();
      ImGui::Text("MSAA");
      ImGui::DragInt("##MSAA", &UiMsaaCount, 0.005f, 0, 16, "%d",
                     ImGuiSliderFlags_AlwaysClamp);
      ImGui::Spacing();
      if (ImGui::Button("Resize",
                        ImVec2(ImGui::GetContentRegionAvail().x, 0.0f))) {
        Window->SetDimensions(UiWindowWidth, UiWindowHeight, UiMsaaCount,
                              Window->RenderScale);
      }
    }

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Background", ImGuiTreeNodeFlags_DefaultOpen)) {
      uint32_t LastBackground = CurrentBackground;

      const char* comboPreviewValue = BackgroundNames[CurrentBackground];

      ImGui::Spacing();
      if (ImGui::BeginCombo("##backgroundCombo", comboPreviewValue)) {
        for (uint32_t i = 0; i < BackgroundCount; i++) {
          ImGui::PushID(i);
          const bool isSelected = (CurrentBackground == i);
          if (ImGui::Selectable(BackgroundNames[i], isSelected))
            CurrentBackground = i;
          if (isSelected) ImGui::SetItemDefaultFocus();
          ImGui::PopID();
        }
        ImGui::EndCombo();
      }

      if (LastBackground != CurrentBackground ||
          Characters2D[0].Status == LS_Unloaded) {
        Backgrounds2D[0]->LoadAsync(BackgroundIds[CurrentBackground]);
      }
    }

    if (ImGui::CollapsingHeader("Character", ImGuiTreeNodeFlags_DefaultOpen)) {
      uint32_t LastCharacter = CurrentCharacter;

      const char* comboPreviewValue = CharacterNames[CurrentCharacter];

      ImGui::Spacing();
      if (ImGui::BeginCombo("##characterCombo", comboPreviewValue)) {
        for (uint32_t i = 0; i < CharacterCount; i++) {
          ImGui::PushID(i);
          const bool isSelected = (CurrentCharacter == i);
          if (ImGui::Selectable(CharacterNames[i], isSelected))
            CurrentCharacter = i;
          if (isSelected) ImGui::SetItemDefaultFocus();
          ImGui::PopID();
        }
        ImGui::EndCombo();
      }

      if (LastCharacter != CurrentCharacter ||
          Characters2D[0].Status == LS_Unloaded) {
        Characters2D[0].LoadAsync(CharacterIds[CurrentCharacter] | 0x10000);
      }
      if (Characters2D[0].Status == LS_Loaded) Characters2D[0].Show = true;

      ImGui::Spacing();
      ImGui::Text("Character X");
      ImGui::SliderFloat("##characterX", &Characters2D[0].OffsetX, -5000.0f,
                         5000.0f);
      ImGui::Spacing();
      ImGui::Text("Character Y");
      ImGui::SliderFloat("##characterY", &Characters2D[0].OffsetY, -5000.0f,
                         5000.0f);
      ImGui::Spacing();
      Characters2D[0].Face >>= 16;
      ImGui::Text("Character Face");
      ImGui::InputInt("##characterFace", &Characters2D[0].Face);
      Characters2D[0].Face <<= 16;
      ImGui::Spacing();
      ImGui::Text("Character Eye");
      ImGui::InputInt("##characterEye", &Characters2D[0].EyeFrame);
      ImGui::Spacing();
      ImGui::Text("Character Lip");
      ImGui::InputInt("##characterLip", &Characters2D[0].LipFrame);
    }

    ImGui::Spacing();

    if (ImGui::CollapsingHeader("BGM", ImGuiTreeNodeFlags_DefaultOpen)) {
      const char* comboPreviewValue = BgmNames[CurrentBgm];

      ImGui::Spacing();
      if (ImGui::BeginCombo("##bgmCombo", comboPreviewValue)) {
        for (uint32_t i = 0; i < BgmCount; i++) {
          ImGui::PushID(i);
          const bool isSelected = (CurrentBgm == i);
          if (ImGui::Selectable(BgmNames[i], isSelected)) CurrentBgm = i;
          if (isSelected) ImGui::SetItemDefaultFocus();
          ImGui::PopID();
        }
        ImGui::EndCombo();
      }
      ImGui::Spacing();
      ImGui::Checkbox("Loop (takes effect on switch)", &BgmLoop);
      ImGui::Spacing();
      if (ImGui::Button("Switch")) {
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
  if (Characters2D[0].Status == LS_Unloaded) {
    Backgrounds2D[0]->LoadAsync(BackgroundIds[0]);
  }

  if (Characters2D[0].Status == LS_Unloaded) {
    Characters2D[0].LoadAsync(CharacterIds[0] | 0x10000);
  }

  if (Characters2D[0].Status == LS_Loaded) Characters2D[0].Show = true;
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