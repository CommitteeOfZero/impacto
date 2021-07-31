#include "game.h"

#include "window.h"
#include "../vendor/nuklear/nuklear_sdl_gl3.h"
#include "workqueue.h"
#include "modelviewer.h"
#include "characterviewer.h"
#include "log.h"
#include "inputsystem.h"

#include "ui/ui.h"

#include "savesystem.h"
#include "audio/audiosystem.h"
#include "audio/audiochannel.h"
#include "audio/audiostream.h"
#include "video/videosystem.h"
#include "renderer2d.h"
#include "background2d.h"
#include "mask2d.h"
#include "character2d.h"
#include "3d/scene.h"
#include "mem.h"
#include "hud/datedisplay.h"
#include "hud/saveicondisplay.h"
#include "hud/loadingdisplay.h"
#include "io/memorystream.h"

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
#include "profile/hud/datedisplay.h"
#include "profile/ui/selectionmenu.h"
#include "profile/ui/sysmesbox.h"
#include "profile/ui/systemmenu.h"
#include "profile/ui/titlemenu.h"
#include "profile/ui/savemenu.h"

namespace Impacto {

using namespace Profile::ScriptVars;

static int const NkMaxVertexMemory = 256 * 1024;
static int const NkMaxElementMemory = 128 * 1024;

nk_context* Nk = 0;

namespace Game {
DrawComponentType DrawComponents[Vm::MaxThreads];

bool ShouldQuit = false;

static void Init() {
  WorkQueue::Init();

  Profile::LoadGameFromJson();

  Io::VfsInit();
  Window::Init();

  memset(DrawComponents, TD_None, sizeof(DrawComponents));

  if (Profile::GameFeatures & GameFeature::Nuklear) {
    Nk = nk_sdl_init(Window::SDLWindow, NkMaxVertexMemory, NkMaxElementMemory);
    struct nk_font_atlas* atlas;
    nk_sdl_font_stash_begin(&atlas);
    // no fonts => default font used, but we still have do the setup
    nk_sdl_font_stash_end();
  }

  if (Profile::GameFeatures & GameFeature::Audio) {
    Audio::AudioInit();
  }

  if (Profile::GameFeatures & GameFeature::Video) {
    Video::VideoInit();
  }

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene3D::Init();
  }

  memset(ScrWork, 0, sizeof(ScrWork));
  memset(FlagWork, 0, sizeof(FlagWork));

  if (Profile::GameFeatures & GameFeature::Renderer2D) {
    Profile::LoadSpritesheets();
    Profile::Charset::Load();
    Profile::LoadFonts();
    Profile::LoadAnimations();
    DialoguePage::Init();

    Renderer2D::Init();
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
    SaveIconDisplay::Init();
    LoadingDisplay::Init();
    Profile::SysMesBox::Configure();
    Profile::TitleMenu::Configure();
    Profile::SystemMenu::Configure();
    Profile::SaveMenu::Configure();
    DateDisplay::Init();
  }

  Profile::ClearJsonProfile();
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

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene3D::Shutdown();
  }

  if (Profile::GameFeatures & GameFeature::Renderer2D) {
    Renderer2D::Shutdown();
  }

  if (Profile::GameFeatures & GameFeature::Nuklear) {
    nk_sdl_shutdown();
  }

  Window::Shutdown();
}

void Update(float dt) {
  SDL_Event e;
  if (Profile::GameFeatures & GameFeature::Nuklear) {
    nk_input_begin(Nk);
  }
  if (Profile::GameFeatures & GameFeature::Input) {
    Input::BeginFrame();
  }
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      ShouldQuit = true;
    }

    if (Profile::GameFeatures & GameFeature::Nuklear) {
      SDL_Event e_nk;
      memcpy(&e_nk, &e, sizeof(SDL_Event));
      Window::AdjustEventCoordinatesForNk(&e_nk);
      if (nk_sdl_handle_event(&e_nk)) continue;
    }

    if (Profile::GameFeatures & GameFeature::Input) {
      if (Input::HandleEvent(&e)) continue;
    }

    WorkQueue::HandleEvent(&e);
  }
  if (Profile::GameFeatures & GameFeature::Input) {
    Input::EndFrame();
  }
  if (Profile::GameFeatures & GameFeature::Nuklear) {
    nk_input_end(Nk);
  }

  if (Profile::GameFeatures & GameFeature::ModelViewer) {
    ModelViewer::Update(dt);
  }

  if (Profile::GameFeatures & GameFeature::CharacterViewer) {
    CharacterViewer::Update(dt);
  }

  if (Profile::GameFeatures & GameFeature::Sc3VirtualMachine) {
    Vm::Update();

    UI::SysMesBoxPtr->Update(dt);
    UI::TitleMenuPtr->Update(dt);
    UI::SystemMenuPtr->Update(dt);
    UI::SaveMenuPtr->Update(dt);
    UI::SelectionMenuPtr->Update(dt);

    SaveIconDisplay::Update(dt);
    LoadingDisplay::Update(dt);
    DateDisplay::Update(dt);
  }

  if (Profile::GameFeatures & GameFeature::Audio) {
    Audio::AudioUpdate(dt);
  }

  if (Profile::GameFeatures & GameFeature::Video) {
    Video::VideoUpdate(dt);
  }

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene3D::Update(dt);
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
  }
}

static bool DebugWindowEnabled = false;
// FPS counter
static float LastTime;
static int Frames = 0;
static float FPS = 0.0f;
static int ScrWorkIndexStart = 0;
static int ScrWorkIndexEnd = 0;
static int FlagWorkIndexStart = 0;
static int FlagWorkIndexEnd = 0;

void Render() {
  Window::Update();

  Rect viewport = Window::GetViewport();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene3D::Render();
  }

  if ((Profile::GameFeatures & GameFeature::Nuklear) &&
      (Profile::GameFeatures & GameFeature::Sc3VirtualMachine)) {
    if (Input::KeyboardButtonWentDown[SDL_SCANCODE_E] ||
        Input::ControllerButtonWentDown[SDL_CONTROLLER_BUTTON_Y])
      DebugWindowEnabled = !DebugWindowEnabled;

    // FPS counter
    Frames++;
    float time = (float)((double)SDL_GetPerformanceCounter() /
                         (double)SDL_GetPerformanceFrequency());
    if (time - LastTime >= 2.0f) {
      FPS = (float)Frames / (time - LastTime);
      LastTime = time;
      Frames = 0;
    }

    if (DebugWindowEnabled) {
      if (nk_begin(Nk, "Debug Editor",
                   nk_rect(20, 20, 300, Window::WindowHeight - 40),
                   NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
        nk_layout_row_dynamic(Nk, 24, 1);
        char buffer[32];  // whatever
        snprintf(buffer, 32, "FPS: %02.2f", FPS);
        nk_label(Nk, buffer, NK_TEXT_ALIGN_CENTERED);

        nk_property_int(Nk, "ScrWork start index", 0, &ScrWorkIndexStart, 8000,
                        1, 1.0f);
        nk_property_int(Nk, "ScrWork end index", 0, &ScrWorkIndexEnd, 8000, 1,
                        1.0f);

        if (ScrWorkIndexEnd < ScrWorkIndexStart)
          ScrWorkIndexEnd = ScrWorkIndexStart;

        nk_property_int(Nk, "FlagWork start index", 0, &FlagWorkIndexStart,
                        7000, 1, 0.0f);
        nk_property_int(Nk, "FlagWork end index", 0, &FlagWorkIndexEnd, 7000, 1,
                        0.0f);

        if (FlagWorkIndexEnd < FlagWorkIndexStart)
          FlagWorkIndexEnd = FlagWorkIndexStart;

        if (nk_tree_push(Nk, NK_TREE_TAB, "ScrWork Editor", NK_MINIMIZED)) {
          nk_layout_row_dynamic(Nk, 24, 1);

          for (int i = ScrWorkIndexStart; i <= ScrWorkIndexEnd; i++) {
            char buf[32];
            snprintf(buf, 32, "ScrWork[%d]", i);
            nk_property_int(Nk, buf, INT_MIN, &ScrWork[i], INT_MAX, 1, 50.0f);
          }

          nk_tree_pop(Nk);
        }

        if (nk_tree_push(Nk, NK_TREE_TAB, "FlagWork Editor", NK_MINIMIZED)) {
          nk_layout_row_dynamic(Nk, 24, 1);

          for (int i = FlagWorkIndexStart; i <= FlagWorkIndexEnd; i++) {
            char buf[32];
            snprintf(buf, 32, "GetFlag(%d)", i);
            int flagVal = (int)GetFlag(i);
            nk_checkbox_label(Nk, buf, &flagVal);
            SetFlag(i, (bool)flagVal);
          }

          nk_tree_pop(Nk);
        }
      }
      nk_end(Nk);
    }
  }

  if (Profile::GameFeatures & GameFeature::Renderer2D) {
    Renderer2D::BeginFrame();
    for (int i = 0; i < Vm::MaxThreads; i++) {
      if (DrawComponents[i] == TD_None) break;

      switch (DrawComponents[i]) {
        case TD_Text: {
          DialoguePages[0].Render();
          UI::SelectionMenuPtr->Render();
          break;
        }
        case TD_Main: {
          for (uint32_t layer = 0; layer <= Profile::LayerCount; layer++) {
            // TODO

            for (int i = 0; i < MaxBackgrounds2D; i++) {
              int bufId = ScrWork[SW_BG1SURF + i];
              Backgrounds2D[bufId]->Render(i, layer);
            }
            for (int i = 0; i < MaxCharacters2D; i++) {
              int bufId = ScrWork[SW_CHA1SURF + i];
              Characters2D[bufId].Render(i, layer);
            }
            if (ScrWork[SW_MASK1PRI] == layer) {
              int maskAlpha =
                  ScrWork[SW_MASK1ALPHA_OFS] + ScrWork[SW_MASK1ALPHA];
              if (maskAlpha) {
                int maskPosX = ScrWork[SW_MASK1POSX];
                int maskPosY = ScrWork[SW_MASK1POSY];
                int maskSizeX = ScrWork[SW_MASK1SIZEX];
                int maskSizeY = ScrWork[SW_MASK1SIZEY];
                if (!maskSizeX || !maskSizeY) {
                  maskPosX = 0;
                  maskPosY = 0;
                  maskSizeX = Profile::DesignWidth;
                  maskSizeY = Profile::DesignHeight;
                }
                glm::vec4 col = ScrWorkGetColor(SW_MASK1COLOR);
                col.a = glm::min(maskAlpha / 255.0f, 1.0f);
                Renderer2D::DrawRect(
                    RectF(maskPosX, maskPosY, maskSizeX, maskSizeY), col);
              }
            }

            if (Profile::UseScreenCapEffects) {
              if (ScrWork[SW_EFF_CAP_BUF] && ScrWork[SW_EFF_CAP_PRI] == layer) {
                int bufId = (int)std::log2(ScrWork[SW_EFF_CAP_BUF]);
                if (Backgrounds2D[bufId]->Status == LS_Loaded) {
                  Renderer2D::CaptureScreencap(Backgrounds2D[bufId]->BgSprite);
                }
              }

              if (ScrWork[SW_EFF_CAP_BUF2] &&
                  ScrWork[SW_EFF_CAP_PRI2] == layer) {
                int bufId = (int)std::log2(ScrWork[SW_EFF_CAP_BUF2]);
                if (Backgrounds2D[bufId]->Status == LS_Loaded) {
                  Renderer2D::CaptureScreencap(Backgrounds2D[bufId]->BgSprite);
                }
              }
            }
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
                Scene3D::MainCamera.CameraTransform.Position !=
                    Profile::Scene3D::DefaultCameraPosition)
              SetFlag(SF_DATEDISPLAY, 0);
            else
              SetFlag(SF_DATEDISPLAY, 1);
            //////////////////////////////
          }
          DateDisplay::Render();
          Video::VideoRender(ScrWork[SW_MOVIEALPHA] / 256.0f);
          break;
        }
        case TD_SystemText: {
          break;
        }
        case TD_SaveMenu: {
          UI::SaveMenuPtr->Render();
          break;
        }
        case TD_SystemIcons: {
          LoadingDisplay::Render();
          SaveIconDisplay::Render();
          break;
        }
        case TD_TitleMenu: {
          UI::TitleMenuPtr->Render();
          break;
        }
        case TD_SystemMenu: {
          // TODO: Ehhh... not the greatest way of doing this...
          if (Profile::Vm::GameInstructionSet != +Vm::InstructionSet::MO6TW) {
            UI::TitleMenuPtr->Render();
          }
          UI::SystemMenuPtr->Render();
          break;
        }
        case TD_SystemMessage: {
          UI::SysMesBoxPtr->Render();
          break;
        }
        case TD_SaveIcon: {
          SaveIconDisplay::Render();
          break;
        }
        default: {
          ImpLogSlow(LL_Error, LC_General,
                     "Encountered unknown draw component type %02X\n",
                     DrawComponents[i]);
          break;
        }
      }
    }
    Renderer2D::EndFrame();
  }

  if (Profile::GameFeatures & GameFeature::CharacterViewer) {
    Renderer2D::BeginFrame();
    if (Backgrounds2D[0]->Status == LS_Loaded) {
      Renderer2D::DrawSprite(
          Backgrounds2D[0]->BgSprite,
          RectF(0.0f, 0.0f, Backgrounds2D[0]->BgSprite.ScaledWidth(),
                Backgrounds2D[0]->BgSprite.ScaledHeight()));
    }
    if (Characters2D[0].Status == LS_Loaded) {
      Characters2D[0].Layer = 0;
      ScrWork[SW_CHA1ALPHA] = 256;
      Characters2D[0].Render(0, 0);
    }
    Renderer2D::EndFrame();
  }

  if (Profile::GameFeatures & GameFeature::Nuklear) {
    if (Window::GLDebug) {
      // Nuklear spams these
      glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                               NULL, GL_FALSE);
    }
    nk_sdl_render(NK_ANTI_ALIASING_OFF, viewport.Width, viewport.Height);
    if (Window::GLDebug) {
      glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                               NULL, GL_TRUE);
    }
  }

  Window::Draw();
}

}  // namespace Game

}  // namespace Impacto