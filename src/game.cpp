#include "game.h"

#include "text.h"
#include "workqueue.h"
#include "modelviewer.h"
#include "characterviewer.h"
#include "log.h"
#include "inputsystem.h"
#include "debugmenu.h"

#include "ui/ui.h"

#include "data/savesystem.h"
#include "data/achievementsystem.h"
#include "data/tipssystem.h"
#include "audio/audiosystem.h"
#include "video/videosystem.h"
#include "background2d.h"
#include "mask2d.h"
#include "character2d.h"
#include "renderer/3d/scene.h"
#include "mem.h"
#include "vm/interface/input.h"
#include "vm/inst_dialogue.h"
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
#include "profile/configsystem.h"
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

static void Init() {
  WorkQueue::Init();

  Profile::LoadGameFromLua();

  Io::VfsInit();

#ifndef IMPACTO_DISABLE_IMGUI
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = NULL;
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

  Profile::ConfigSystem::Configure();

  if (Profile::GameFeatures & GameFeature::Audio) {
    Audio::AudioInit();
  }

  if (Profile::GameFeatures & GameFeature::Video) {
    Video::VideoInit();
  }

  ScrWork = {};
FlagWork = {};

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
    AchievementSystem::Init();
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

    // Default controls
    Vm::Interface::UpdatePADcustomType(0);
  }

  Profile::ClearProfile();
}

void InitFromProfile(std::string const& name) {
  Profile::MakeLuaProfile(name);
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
  WorkQueue::StopWorkQueue();
  Window->Shutdown();
}

void UpdateGameState(float dt) {
  static float UpdateSecondCounter = 0.0f;
  UpdateSecondCounter += dt;
  if (UpdateSecondCounter >= 1) {
    ScrWork[SW_PLAYTIME]++;
    UpdateSecondCounter -= 1.0f;
  }

  if ((ScrWork[SW_GAMESTATE] & 5) == 1 && !GetFlag(SF_SYSTEMMENUDISABLE) &&
      !GetFlag(SF_GAMEPAUSE) && !GetFlag(SF_SYSMENUDISABLE) &&
      Vm::Interface::GetControlState(8)) {
    // Some more stuff here?
    if ((GetFlag(SF_MESWINDOW0OPENFL) &&
         DialoguePages[0].TextIsFullyOpaque()) ||
        (GetFlag(SF_SHOWWAITICON) || GetFlag(SF_SHOWWAITICON + 1) ||
         GetFlag(SF_SHOWWAITICON + 2))) {
      SetFlag(SF_UIHIDDEN, !GetFlag(SF_UIHIDDEN));
    }
  }
  Vm::ChkMesSkip();
}

void UpdateSystem(float dt) {
  static float UpdateSecondCounter = 0.0f;
  UpdateSecondCounter += dt;
  if (UpdateSecondCounter <= 1 / 60.0f) {
    return;
  }

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
  if (Profile::GameFeatures & GameFeature::Sc3VirtualMachine) {
    Vm::Interface::UpdatePADInput();
    UpdateGameState(UpdateSecondCounter);

    for (DrawComponentType value : DrawComponentType::_values()) {
      for (auto const& menu : UI::Menus[value]) {
        menu->Update(UpdateSecondCounter);
      }
    }

    SaveIconDisplay::Update(UpdateSecondCounter);
    LoadingDisplay::Update(UpdateSecondCounter);
    DateDisplay::Update(UpdateSecondCounter);
    if (ScrWork[SW_GAMESTATE] & 5 && !GetFlag(SF_GAMEPAUSE) &&
        !GetFlag(SF_SYSMENUDISABLE)) {
      TipsNotification::Update(UpdateSecondCounter);
      DelusionTrigger::Update(UpdateSecondCounter);
      UI::MapSystem::Update(UpdateSecondCounter);
      if (CCLCC::YesNoTrigger::YesNoTriggerPtr)
        CCLCC::YesNoTrigger::YesNoTriggerPtr->Update(UpdateSecondCounter);
    }

    Vm::Update();
  }
  UpdateSecondCounter = 0.0f;
}

void Update(float dt) {
  UpdateSystem(dt);

#ifndef IMPACTO_DISABLE_IMGUI
  Renderer->ImGuiBeginFrame();
#endif

  if (Profile::GameFeatures & GameFeature::ModelViewer) {
    ModelViewer::Update(dt);
  }

  if (Profile::GameFeatures & GameFeature::CharacterViewer) {
    CharacterViewer::Update(dt);
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

static bool ShouldRender(const int layer) {
  using namespace Impacto::Profile::Vm;

  for (int bgId = 0; bgId < MaxBackgrounds2D; bgId++) {
    if (GetFlag(SF_BG1DISP + bgId) && !GetFlag(SF_BG1LOADEXEC + bgId) &&
        (ScrWork[SW_BG1PRI + ScrWorkBgStructSize * bgId] == layer ||
         ScrWork[SW_BG1PRI2 + ScrWorkBgStructSize * bgId] == layer))
      return true;

    if (GetFlag(SF_BGEFF1DISP + bgId) &&
        (ScrWork[SW_BGEFF1_PRI + ScrWorkBgEffStructSize * bgId] == layer ||
         ScrWork[SW_BGEFF1_PRI2 + ScrWorkBgEffStructSize * bgId] == layer))
      return true;
  }

  for (int capId = 0; capId < MaxScreencaptures; capId++) {
    if (GetFlag(SF_CAP1DISP + capId) &&
        (ScrWork[SW_CAP1PRI + ScrWorkCaptureStructSize * capId] == layer ||
         ScrWork[SW_CAP1PRI2 + ScrWorkCaptureStructSize * capId] == layer))
      return true;
  }

  return false;
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
            const int renderTarget = ScrWork[SW_RENDERTARGET + layer];
            if (0 <= renderTarget && renderTarget <= MaxFramebuffers &&
                ShouldRender(layer)) {
              Renderer->SetFramebuffer(renderTarget);
            }

            for (int i = 0; i < MaxBackgrounds2D; i++) {
              int bufId = ScrWork[SW_BG1SURF + i];
              Backgrounds2D[bufId]->Render(i, layer);
            }

            for (int i = 0; i < MaxCharacters2D; i++) {
              int bufId = ScrWork[SW_CHA1SURF + i];
              Characters2D[bufId].Render(layer);
            }

            for (int bgId = 0; bgId < MaxBackgrounds2D; bgId++) {
              if (GetFlag(SF_BGEFF1DISP + bgId) &&
                  (ScrWork[SW_BGEFF1_PRI + Profile::Vm::ScrWorkBgEffStructSize *
                                               bgId] == layer ||
                   ScrWork[SW_BGEFF1_PRI2 +
                           Profile::Vm::ScrWorkBgEffStructSize * bgId] ==
                       layer)) {
                Framebuffers[0].RenderBgEff(bgId, layer);
              }
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

            for (size_t capId = 0; capId < MaxScreencaptures; capId++) {
              if (!GetFlag(SF_CAP1DISP + capId)) continue;

              for (size_t capLayer = 0; capLayer < MaxScreencaptures;
                   capLayer++) {
                if (ScrWork[SW_CAP1PRI + capId * 20 + capLayer * 8] == layer) {
                  Screencaptures[capId].RenderCapture(capId, layer);
                }
              }
            }

            if (Profile::UseScreenCapEffects) {
              if (ScrWork[SW_EFF_CAP_BUF] && ScrWork[SW_EFF_CAP_PRI] == layer) {
                int bufId = (int)std::log2(ScrWork[SW_EFF_CAP_BUF]);
                if (Backgrounds2D[bufId]->Status == LS_Loaded) {
                  Renderer->CaptureScreencap(Backgrounds2D[bufId]->BgSprite);
                }
              }

              if (ScrWork[SW_EFF_CAP_BUF2] &&
                  ScrWork[SW_EFF_CAP_PRI2] == layer) {
                int bufId = (int)std::log2(ScrWork[SW_EFF_CAP_BUF2]);
                if (Backgrounds2D[bufId]->Status == LS_Loaded) {
                  Renderer->CaptureScreencap(Backgrounds2D[bufId]->BgSprite);
                }
              }
            }

            if (Profile::UseMoviePriority &&
                (Profile::GameFeatures & GameFeature::Video)) {
              int videoAlpha = 0;
              if (Profile::Vm::GameInstructionSet ==
                  +Vm::InstructionSet::CHLCC) {
                if (ScrWork[SW_MOVIEALPHA] > 0 &&
                    ScrWork[SW_MOVIEPRI] == static_cast<int>(layer)) {
                  videoAlpha = ScrWork[SW_MOVIEALPHA];
                } else if (ScrWork[SW_MOVIEALPHA] == 0 &&
                           ScrWork[SW_MOVIEPRI2] == static_cast<int>(layer)) {
                  videoAlpha = ScrWork[SW_MOVIEALPHA2];
                }
              } else {
                if (ScrWork[SW_MOVIEPRI] == static_cast<int>(layer)) {
                  videoAlpha = ScrWork[SW_MOVIEALPHA];
                } else if (ScrWork[SW_MOVIEPRI2] == static_cast<int>(layer)) {
                  videoAlpha = ScrWork[SW_MOVIEALPHA2];
                } else if (ScrWork[SW_MOVIEPRI3] == static_cast<int>(layer)) {
                  videoAlpha = ScrWork[SW_MOVIEALPHA3];
                } else if (ScrWork[SW_MOVIEPRI4] == static_cast<int>(layer)) {
                  videoAlpha = ScrWork[SW_MOVIEALPHA4];
                }
              }
              if (videoAlpha > 0) {
                Video::VideoRender(videoAlpha / 256.0f);
              }
            }
            if (ScrWork[6417] == layer) DelusionTrigger::Render();
            if (CCLCC::YesNoTrigger::YesNoTriggerPtr &&
                ScrWork[6437] == layer) {
              CCLCC::YesNoTrigger::YesNoTriggerPtr->Render();
            }
          }

          Renderer->SetFramebuffer(0);

          if (SaveSystem::Implementation) {
            Renderer->CaptureScreencap(SaveSystem::GetWorkingSaveThumbnail());
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
          ImpLogSlow(LogLevel::Warning, LogChannel::General,
                     "Encountered unknown draw component type 0x{:02x}\n",
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
      Characters2D[0].Layers[0] = 0;
      ScrWork[SW_CHA1ALPHA] = 256;
      Characters2D[0].Render(0);
    }
  }
  Renderer->EndFrame();

  Window->Draw();
}

}  // namespace Game

}  // namespace Impacto