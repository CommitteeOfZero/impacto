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
#include "renderer/window.h"
#include "hud/datedisplay.h"
#include "hud/saveicondisplay.h"
#include "hud/loadingdisplay.h"
#include "hud/tipsnotification.h"
#include "hud/delusiontrigger.h"
#include "games/cclcc/yesnotrigger.h"
#include "games/cclcc/systemmenu.h"

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
#include "profile/ui/helpmenu.h"
#include "profile/data/bgeff.h"

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
  InitCursors();

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

    if (Profile::UseBgChaEffects || Profile::UseBgFrameEffects) {
      Profile::BgEff::Load();
    }
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
    Profile::HelpMenu::Configure();
    Profile::TipsMenu::Configure();
    Profile::ExtraMenus::Configure();
    DateDisplay::Init();
    TipsNotification::Init();
    DelusionTrigger::Init();
    UI::MapSystem::Init();
    CCLCC::YesNoTrigger::Init();

    // Default controls
    Vm::Interface::UpdatePADcustomType(0);

    SaveSystem::InitializeSystemData();
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
    constexpr int maxPlayTime = (999 * 60 + 59) * 60 + 59;
    constexpr int maxTotalPlayTime = (99999 * 60 + 59) * 60 + 59;
    ScrWork[SW_PLAYTIME] = std::min(ScrWork[SW_PLAYTIME] + 1, maxPlayTime);
    ScrWork[SW_TOTALPLAYTIME] =
        std::min(ScrWork[SW_TOTALPLAYTIME] + 1, maxTotalPlayTime);

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
    RequestCursor(CursorType::Default);
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
    Vm::Interface::UpdatePADHoldInput(UpdateSecondCounter);
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
      DelusionTrigger::Update(UpdateSecondCounter);
      UI::MapSystem::Update(UpdateSecondCounter);
      if (CCLCC::YesNoTrigger::YesNoTriggerPtr)
        CCLCC::YesNoTrigger::YesNoTriggerPtr->Update(UpdateSecondCounter);

      if (!GetFlag(SF_MOVIEPLAY)) {
        TipsNotification::Update(UpdateSecondCounter);
      }
    }

    Vm::Update(dt);

    ApplyCursorForFrame();
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
}

static void RenderMain() {
  Background2D::LastRenderedBackground = nullptr;

  for (uint32_t layer = 0; layer <= Profile::LayerCount; layer++) {
    const int renderTarget = ScrWork[SW_RENDERTARGET + layer];
    if (0 <= renderTarget && renderTarget <= MaxFramebuffers) {
      Renderer->SetFramebuffer(renderTarget);
    }

    for (int bgId = 0; bgId < std::ssize(Backgrounds); bgId++) {
      int bufId = ScrWork[SW_BG1SURF + bgId];
      Backgrounds2D[bufId]->UpdateState(bgId);
      Backgrounds2D[bufId]->Render(layer);
    }

    if (Background2D::LastRenderedBackground != nullptr) {
      Background2D::LastRenderedBackground->RenderBgEff(layer);
    }

    if ((Profile::GameFeatures & GameFeature::Renderer2D) &&
        !(Profile::GameFeatures & GameFeature::Scene3D)) {
      for (int chaId = 0; chaId < std::ssize(Characters2D); chaId++) {
        int bufId = ScrWork[SW_CHA1SURF + chaId];
        Characters2D[bufId].UpdateState(chaId);
        Characters2D[bufId].Render(layer);
      }
    }

    for (int bgId = 0; bgId < std::ssize(Backgrounds); bgId++) {
      Framebuffers[0].UpdateState(bgId);
      Framebuffers[0].Render(layer);
    }

    if (ScrWork[SW_MAP_PRI] == static_cast<int>(layer) &&
        ScrWork[SW_MAP_ALPHA]) {
      UI::MapSystem::Render();
    }
    if (ScrWork[SW_MASK1PRI] == static_cast<int>(layer)) {
      const int maskAlpha = ScrWork[SW_MASK1ALPHA_OFS] + ScrWork[SW_MASK1ALPHA];

      if (maskAlpha > 0) {
        RectF maskRect = {
            (float)ScrWork[SW_MASK1POSX], (float)ScrWork[SW_MASK1POSY],
            (float)ScrWork[SW_MASK1SIZEX], (float)ScrWork[SW_MASK1SIZEY]};
        if (maskRect.GetSize() == glm::vec2(0.0f)) {
          maskRect = {0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight};
        }

        glm::vec4 col = ScrWorkGetColor(SW_MASK1COLOR);
        col.a = glm::min(maskAlpha / 256.0f, 1.0f);

        Renderer->DrawQuad(maskRect, col);
      }
    }

    if (Profile::UseScreenCapEffects) {
      if (ScrWork[SW_EFF_CAP_BUF] &&
          static_cast<uint32_t>(ScrWork[SW_EFF_CAP_PRI]) == layer) {
        int bufId = (int)std::log2(ScrWork[SW_EFF_CAP_BUF]);
        if (Backgrounds2D[bufId]->Status == LoadStatus::Loaded) {
          Renderer->CaptureScreencap(Backgrounds2D[bufId]->BgSprite);
        }
      }

      if (ScrWork[SW_EFF_CAP_BUF2] &&
          static_cast<uint32_t>(ScrWork[SW_EFF_CAP_PRI2]) == layer) {
        int bufId = (int)std::log2(ScrWork[SW_EFF_CAP_BUF2]);
        if (Backgrounds2D[bufId]->Status == LoadStatus::Loaded) {
          Renderer->CaptureScreencap(Backgrounds2D[bufId]->BgSprite);
        }
      }
    }

    if (Profile::UseMoviePriority &&
        (Profile::GameFeatures & GameFeature::Video)) {
      int videoAlpha = 0;
      if (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::CHLCC) {
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
    if (static_cast<uint32_t>(ScrWork[SW_DELUSION_PRI]) == layer)
      DelusionTrigger::Render();
    if (CCLCC::YesNoTrigger::YesNoTriggerPtr &&
        static_cast<uint32_t>(ScrWork[SW_YESNO_PRI]) == layer) {
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

  if (!GetFlag(SF_UIHIDDEN) && !GetFlag(SF_MOVIEPLAY)) {
    TipsNotification::Render();
  }

  // MO8 uses those huge layer indexes for movie menu, it doesn't
  // actually have 4000 layers
  if ((Profile::GameFeatures & GameFeature::Video) &&
      (!Profile::UseMoviePriority ||
       (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::MO8 &&
        (ScrWork[SW_MOVIEPRI] == 3000 || ScrWork[SW_MOVIEPRI] == 4000)))) {
    Video::VideoRender(ScrWork[SW_MOVIEALPHA] / 256.0f);
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
    if (Window->WindowDimensionsChanged) {
      Background2D::InitFrameBuffers();
    }
    for (int i = 0; i < Vm::MaxThreads; i++) {
      if (DrawComponents[i] == +DrawComponentType::None) break;

      switch (DrawComponents[i]) {
        case DrawComponentType::Text: {
          if (!GetFlag(SF_UIHIDDEN) &&
              (!GetFlag(SF_SELECTMODE) || GetFlag(SF_SYSTEMMENUCAPTURE))) {
            // Dialogue pages drawn in reverse order, at least for cclcc
            for (int pageId = Profile::Dialogue::PageCount - 1; pageId >= 0;
                 pageId--)
              DialoguePages[pageId].Render();
          }
          // System menu capture
          if (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::CC &&
              GetFlag(SF_SYSTEMMENUCAPTURE)) {
            Renderer->CaptureScreencap(
                static_cast<UI::CCLCC::SystemMenu*>(UI::SystemMenuPtr)
                    ->ScreenCap);
            SetFlag(SF_SYSTEMMENUCAPTURE, false);
          }
          break;
        }
        case DrawComponentType::Main: {
          if (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::CC &&
              !(!GetFlag(SF_SELECTMODE) ||
                (GetFlag(SF_SYSTEMMENUCAPTURE) &&
                 ScrWork[SW_RESTARTMASK] != 0x100))) {
            break;
          }
          RenderMain();
          break;
        }
        case DrawComponentType::ExtrasScenes: {
          break;
        }
        case DrawComponentType::Mask: {
          Renderer->DrawQuad(
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
    if (Backgrounds2D[0]->Status == LoadStatus::Loaded) {
      Renderer->DrawSprite(
          Backgrounds2D[0]->BgSprite,
          RectF(0.0f, 0.0f, Backgrounds2D[0]->BgSprite.ScaledWidth(),
                Backgrounds2D[0]->BgSprite.ScaledHeight()));
    }
    if (Characters2D[0].Status == LoadStatus::Loaded) {
      Characters2D[0].Layers[0] = 0;
      ScrWork[SW_CHA1ALPHA] = 256;

      Characters2D[0].UpdateState(0);
      Characters2D[0].Render(0);
    }
  }
  Renderer->EndFrame();

  Window->Draw();
}

}  // namespace Game

}  // namespace Impacto