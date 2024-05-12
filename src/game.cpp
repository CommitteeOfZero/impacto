#include "game.h"

#include "workqueue.h"
#include "modelviewer.h"
#include "characterviewer.h"
#include "log.h"
#include "inputsystem.h"
#include "debugmenu.h"

#include "ui/ui.h"

#include "data/savesystem.h"
#include "data/tipssystem.h"
#include "audio/audiosystem.h"
#include "video/videosystem.h"
#include "background2d.h"
#include "mask2d.h"
#include "character2d.h"
#include "renderer/3d/scene.h"
#include "mem.h"
#include "vm/interface/input.h"
#include "hud/datedisplay.h"
#include "hud/saveicondisplay.h"
#include "hud/loadingdisplay.h"
#include "hud/tipsnotification.h"
#include "hud/delusiontrigger.h"
#include "games/cclcc/yesnotrigger.h"

#include "profile/profile.h"
#include "profile/game.h"
#include "profile/sprites.h"
#include "profile/charset.h"
#include "profile/fonts.h"
#include "profile/dialogue.h"
#include "profile/animations.h"
#include "profile/scene3d.h"
#include "profile/vm.h"
#include "profile/scriptvars.h"
#include "profile/ui/selectionmenu.h"
#include "profile/ui/sysmesbox.h"
#include "profile/ui/systemmenu.h"
#include "profile/ui/titlemenu.h"
#include "profile/ui/savemenu.h"
#include "profile/ui/backlogmenu.h"
#include "profile/ui/optionsmenu.h"
#include "profile/ui/tipsmenu.h"
#include "profile/ui/extramenus.h"
#include "profile/ui/trophymenu.h"
#include "profile/ui/mapsystem.h"

namespace Impacto {

using namespace Profile::ScriptVars;

namespace Game {
uint8_t DrawComponents[Vm::MaxThreads];

bool ShouldQuit = false;
float UpdateSecondCounter = 0.0f;

static void Init() {
  WorkQueue::Init();

  Profile::LoadGameFromJson();

  Io::VfsInit();

#ifndef IMPACTO_DISABLE_IMGUI
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  if (Profile::GameFeatures & GameFeature::DebugMenu &&
      Profile::GameFeatures & GameFeature::DebugMenuMultiViewport) {
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  }
#endif

  InitRenderer();

  memset(DrawComponents, DrawComponentType::None, sizeof(DrawComponents));

  if (Profile::GameFeatures & GameFeature::Audio) {
    Audio::AudioInit();
  }

  if (Profile::GameFeatures & GameFeature::Video) {
    Video::VideoInit();
  }

  memset(ScrWork, 0, sizeof(ScrWork));
  memset(FlagWork, 0, sizeof(FlagWork));

  if (Profile::GameFeatures & GameFeature::Renderer2D) {
    Profile::LoadSpritesheets();
    Profile::Charset::Load();
    Profile::LoadFonts();
    Profile::LoadAnimations();
    DialoguePage::Init();

    Background2D::Init();
    Mask2D::Init();
  }

  if (Profile::GameFeatures & GameFeature::ModelViewer) {
    ModelViewer::Init();
  }

  if (Profile::GameFeatures & GameFeature::CharacterViewer) {
    CharacterViewer::Init();
  }

  if (Profile::GameFeatures & GameFeature::Sc3VirtualMachine) {
    Vm::Init();

    Profile::SelectionMenu::Configure();

    SaveSystem::Init();
    TipsSystem::Init();
    SaveIconDisplay::Init();
    LoadingDisplay::Init();
    Profile::SysMesBox::Configure();
    Profile::TitleMenu::Configure();
    Profile::SystemMenu::Configure();
    Profile::SaveMenu::Configure();
    Profile::BacklogMenu::Configure();
    Profile::OptionsMenu::Configure();
    Profile::TrophyMenu::Configure();
    Profile::TipsMenu::Configure();
    Profile::ExtraMenus::Configure();
    DateDisplay::Init();
    TipsNotification::Init();
    DelusionTrigger::Init();
    UI::MapSystem::Init();
    CCLCC::YesNoTrigger::Init();
  }

  Profile::ClearProfile();
}

void InitFromProfile(std::string const& name) {
  Profile::MakeJsonProfile(name);
  Init();
}

void Shutdown() {
  if (Profile::GameFeatures & GameFeature::Audio) {
    Audio::AudioShutdown();
  }

  if (Profile::GameFeatures & GameFeature::Video) {
    Video::VideoShutdown();
  }

  if (Profile::GameFeatures & GameFeature::Renderer2D) {
    Renderer->Shutdown();
  }

  Window->Shutdown();
}

void UpdateGameState(float dt) {
  UpdateSecondCounter += dt;
  if (UpdateSecondCounter >= 1) {
    ScrWork[SW_PLAYTIME]++;
    UpdateSecondCounter -= 1.0f;
  }
}

void Update(float dt) {
  SDL_Event e;
  if (Profile::GameFeatures & GameFeature::Input) {
    Input::BeginFrame();
  }
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      ShouldQuit = true;
    }

#ifndef IMPACTO_DISABLE_IMGUI
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui_ImplSDL2_ProcessEvent(&e) &&
        (io.WantCaptureKeyboard || io.WantCaptureMouse))
      continue;
#endif

    if (Profile::GameFeatures & GameFeature::Input) {
      if (Input::HandleEvent(&e)) continue;
    }

    WorkQueue::HandleEvent(&e);
  }
  if (Profile::GameFeatures & GameFeature::Input) {
    Input::EndFrame();
  }

#ifndef IMPACTO_DISABLE_IMGUI
  Renderer->ImGuiBeginFrame();
#endif

  if (Profile::GameFeatures & GameFeature::ModelViewer) {
    ModelViewer::Update(dt);
  }

  if (Profile::GameFeatures & GameFeature::CharacterViewer) {
    CharacterViewer::Update(dt);
  }

  if (Profile::GameFeatures & GameFeature::Sc3VirtualMachine) {
    Vm::Interface::UpdatePADInput();
    UpdateGameState(dt);

    for (DrawComponentType value : DrawComponentType::_values()) {
      for (auto const& menu : UI::Menus[value]) {
        menu->Update(dt);
      }
    }

    SaveIconDisplay::Update(dt);
    LoadingDisplay::Update(dt);
    DateDisplay::Update(dt);
    TipsNotification::Update(dt);
    DelusionTrigger::Update(dt);
    UI::MapSystem::Update(dt);
    CCLCC::YesNoTrigger::Update(dt);

    Vm::Update();
  }

  if (Profile::GameFeatures & GameFeature::Audio) {
    Audio::AudioUpdate(dt);
  }

  if (Profile::GameFeatures & GameFeature::Video) {
    Video::VideoUpdate(dt);
  }

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Renderer->Scene->Update(dt);
  }

  if (Profile::GameFeatures & GameFeature::Renderer2D) {
    for (int i = 0; i < Profile::Dialogue::PageCount; i++)
      DialoguePages[i].Update(dt);
  }

  if ((Profile::GameFeatures & GameFeature::Renderer2D) &&
      !(Profile::GameFeatures & GameFeature::Scene3D)) {
    for (int i = 0; i < MaxCharacters2D; i++) {
      if (Characters2D[i].Show) Characters2D[i].Update(dt);
    }
    if (Profile::Dialogue::HasSpeakerPortraits) {
      for (int i = 0; i < MaxSpeakerPortraits; i++) {
        if (SpeakerPortraits[i].Show) SpeakerPortraits[i].Update(dt);
      }
    }
  }
}

void Render() {
  Window->Update();

  Renderer->BeginFrame();

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Renderer->Scene->Render();
  }

  Renderer->BeginFrame2D();

#ifndef IMPACTO_DISABLE_IMGUI
  if (Profile::GameFeatures & GameFeature::DebugMenu) {
    DebugMenu::Show();
  }
#endif

  if (Profile::GameFeatures & GameFeature::Renderer2D) {
    for (int i = 0; i < Vm::MaxThreads; i++) {
      if (DrawComponents[i] == +DrawComponentType::None) break;

      switch (DrawComponents[i]) {
        case DrawComponentType::Text: {
          for (int i = 0; i < Profile::Dialogue::PageCount; i++)
            DialoguePages[i].Render();
          break;
        }
        case DrawComponentType::Main: {
          for (uint32_t layer = 0; layer <= Profile::LayerCount; layer++) {
            // TODO

            for (int i = 0; i < MaxBackgrounds2D; i++) {
              int bufId = ScrWork[SW_BG1SURF + i];
              Backgrounds2D[bufId]->Render(i, layer);
            }
            for (int i = 0; i < MaxCharacters2D; i++) {
              int bufId = ScrWork[SW_CHA1SURF + i];
              Characters2D[bufId].Render(layer);
            }
            if (ScrWork[6361] == static_cast<int>(layer) && ScrWork[6360]) {
              UI::MapSystem::Render();
            }
            if (ScrWork[SW_MASK1PRI] == static_cast<int>(layer)) {
              int maskAlpha =
                  ScrWork[SW_MASK1ALPHA_OFS] + ScrWork[SW_MASK1ALPHA];
              if (maskAlpha) {
                float maskPosX = (float)ScrWork[SW_MASK1POSX];
                float maskPosY = (float)ScrWork[SW_MASK1POSY];
                float maskSizeX = (float)ScrWork[SW_MASK1SIZEX];
                float maskSizeY = (float)ScrWork[SW_MASK1SIZEY];
                if (!maskSizeX || !maskSizeY) {
                  maskPosX = 0.0f;
                  maskPosY = 0.0f;
                  maskSizeX = Profile::DesignWidth;
                  maskSizeY = Profile::DesignHeight;
                }
                glm::vec4 col = ScrWorkGetColor(SW_MASK1COLOR);
                col.a = glm::min(maskAlpha / 255.0f, 1.0f);
                Renderer->DrawRect(
                    RectF(maskPosX, maskPosY, maskSizeX, maskSizeY), col);
              }
            }

            if (Profile::UseScreenCapEffects) {
              if (ScrWork[SW_EFF_CAP_BUF] &&
                  ScrWork[SW_EFF_CAP_PRI] == static_cast<int>(layer)) {
                int bufId = (int)std::log2(ScrWork[SW_EFF_CAP_BUF]);
                if (Backgrounds2D[bufId]->Status == LS_Loaded) {
                  Renderer->CaptureScreencap(Backgrounds2D[bufId]->BgSprite);
                }
              }

              if (ScrWork[SW_EFF_CAP_BUF2] &&
                  ScrWork[SW_EFF_CAP_PRI2] == static_cast<int>(layer)) {
                int bufId = (int)std::log2(ScrWork[SW_EFF_CAP_BUF2]);
                if (Backgrounds2D[bufId]->Status == LS_Loaded) {
                  Renderer->CaptureScreencap(Backgrounds2D[bufId]->BgSprite);
                }
              }
            }

            if (Profile::UseMoviePriority &&
                (Profile::GameFeatures & GameFeature::Video)) {
              int videoAlpha;
              if (ScrWork[SW_MOVIEPRI] == static_cast<int>(layer)) {
                videoAlpha = ScrWork[SW_MOVIEALPHA];
              } else if (ScrWork[SW_MOVIEPRI2] == static_cast<int>(layer)) {
                videoAlpha = ScrWork[SW_MOVIEALPHA2];
              } else if (ScrWork[SW_MOVIEPRI3] == static_cast<int>(layer)) {
                videoAlpha = ScrWork[SW_MOVIEALPHA3];
              } else if (ScrWork[SW_MOVIEPRI4] == static_cast<int>(layer)) {
                videoAlpha = ScrWork[SW_MOVIEALPHA4];
              } else {
                videoAlpha = 0;
              }
              if (videoAlpha > 0) {
                Video::VideoRender(videoAlpha / 256.0f);
              }
            }
            if (ScrWork[6417] == layer) DelusionTrigger::Render();
            if (ScrWork[6437] == layer) CCLCC::YesNoTrigger::Render();
          }
          if (GetFlag(SF_Pokecon_Open)) {
            SetFlag(SF_DATEDISPLAY, 0);
            // hack
            ScrWork[SW_POKECON_BOOTANIMECT] = 0;
            ScrWork[SW_POKECON_SHUTDOWNANIMECT] = 0;
            ScrWork[SW_POKECON_MENUSELANIMECT] = 0;
          }
          if (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::Dash) {
            /////////// DaSH hack kind of? ///////
            if (GetFlag(SF_Pokecon_Disable) || GetFlag(SF_Pokecon_Open) ||
                Renderer->Scene->MainCamera.CameraTransform.Position !=
                    Profile::Scene3D::DefaultCameraPosition)
              SetFlag(SF_DATEDISPLAY, 0);
            else
              SetFlag(SF_DATEDISPLAY, 1);
            //////////////////////////////
          }
          DateDisplay::Render();
          TipsNotification::Render();
          // MO8 uses those huge layer indexes for movie menu, it doesn't
          // actually have 4000 layers
          if ((Profile::GameFeatures & GameFeature::Video) &&
              (!Profile::UseMoviePriority ||
               (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::MO8 &&
                (ScrWork[SW_MOVIEPRI] == 3000 ||
                 ScrWork[SW_MOVIEPRI] == 4000)))) {
            Video::VideoRender(ScrWork[SW_MOVIEALPHA] / 256.0f);
          }
          break;
        }
        case DrawComponentType::ExtrasScenes: {
          break;
        }
        case DrawComponentType::Mask: {
          Renderer->DrawRect(
              RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
              glm::vec4(0.0f, 0.0f, 0.0f, (ScrWork[SW_RESTARTMASK] / 256.0f)));
          break;
        }
        case DrawComponentType::SaveMenu: {
          break;
        }
        case DrawComponentType::SystemIcons: {
          UI::MapSystem::RenderButtonGuide();
          LoadingDisplay::Render();
          SaveIconDisplay::Render();
          break;
        }
        case DrawComponentType::TitleMenu: {
          break;
        }
        case DrawComponentType::SystemMenu: {
          break;
        }
        case DrawComponentType::PlayData: {
          break;
        }
        case DrawComponentType::SystemMessage: {
          break;
        }
        case DrawComponentType::SaveIcon: {
          SaveIconDisplay::Render();
          break;
        }
        default: {
          ImpLogSlow(LL_Warning, LC_General,
                     "Encountered unknown draw component type %02X\n",
                     DrawComponents[i]);
          break;
        }
      }

      for (auto const& menu : UI::Menus[DrawComponents[i]]) {
        menu->Render();
      }
    }
  }

  if (Profile::GameFeatures & GameFeature::CharacterViewer) {
    if (Backgrounds2D[0]->Status == LS_Loaded) {
      Renderer->DrawSprite(
          Backgrounds2D[0]->BgSprite,
          RectF(0.0f, 0.0f, Backgrounds2D[0]->BgSprite.ScaledWidth(),
                Backgrounds2D[0]->BgSprite.ScaledHeight()));
    }
    if (Characters2D[0].Status == LS_Loaded) {
      Characters2D[0].Layer = 0;
      ScrWork[SW_CHA1ALPHA] = 256;
      Characters2D[0].Render(0);
    }
  }
  Renderer->EndFrame();

  Window->Draw();
}

}  // namespace Game

}  // namespace Impacto