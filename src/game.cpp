#include "game.h"

#include "window.h"
#include "../vendor/nuklear/nuklear_sdl_gl3.h"
#include "workqueue.h"
#include "modelviewer.h"
#include "characterviewer.h"
#include "log.h"
#include "inputsystem.h"
#include "audio/audiosystem.h"
#include "audio/audiochannel.h"
#include "audio/audiostream.h"
#include "renderer2d.h"
#include "background2d.h"
#include "character2d.h"
#include "3d/scene.h"
#include "mem.h"
#include "hud/datedisplay.h"
#include "hud/saveicondisplay.h"
#include "hud/sysmesbox.h"
#include "hud/loadingdisplay.h"
#include "hud/titlemenu.h"
#include "hud/mainmenu.h"
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
#include "profile/hud/sysmesbox.h"

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
  }

  if (Profile::GameFeatures & GameFeature::ModelViewer) {
    ModelViewer::Init();
  }

  if (Profile::GameFeatures & GameFeature::CharacterViewer) {
    CharacterViewer::Init();
  }

  if (Profile::GameFeatures & GameFeature::Sc3VirtualMachine) {
    SaveIconDisplay::Init();
    LoadingDisplay::Init();
    SysMesBox::Init();
    TitleMenu::Init();
    MainMenu::Init();
    DateDisplay::Init();

    Vm::Init();
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
    SaveIconDisplay::Update(dt);
    SysMesBox::Update(dt);
    LoadingDisplay::Update(dt);
    DateDisplay::Update(dt);
    TitleMenu::Update(dt);
    MainMenu::Update(dt);
  }

  if (Profile::GameFeatures & GameFeature::Audio) {
    Audio::AudioUpdate(dt);
  }

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene3D::Update(dt);
  }

  if (Profile::GameFeatures & GameFeature::Renderer2D) {
    for (int i = 0; i < Profile::Dialogue::PageCount; i++)
      DialoguePages[i].Update(dt);
  }
}

void Render() {
  Window::Update();

  Rect viewport = Window::GetViewport();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene3D::Render();
  }

  if (Profile::GameFeatures & GameFeature::Renderer2D) {
    Renderer2D::BeginFrame();
    for (int i = 0; i < Vm::MaxThreads; i++) {
      if (DrawComponents[i] == TD_None) break;

      switch (DrawComponents[i]) {
        case TD_Text: {
          DialoguePages[0].Render();
          break;
        }
        case TD_Main: {
          for (uint32_t layer = 0; layer < Profile::LayerCount; layer++) {
            // TODO

            for (int i = 0; i < MaxBackgrounds2D; i++) {
              if (Backgrounds2D[i].Status == LS_Loaded &&
                  Backgrounds2D[i].Layer == layer && Backgrounds2D[i].Show) {
                glm::vec4 col = glm::vec4(1.0f);
                if (Profile::Vm::GameInstructionSet ==
                    +Vm::InstructionSet::Dash) {
                  col.a =
                      ScrWork[SW_BG1ALPHA +
                              Impacto::Profile::Vm::ScrWorkBgStructSize * i] /
                      256.0f;
                }
                Renderer2D::DrawSprite(
                    Backgrounds2D[i].BgSprite,
                    RectF(Backgrounds2D[i].DisplayCoords.x,
                          Backgrounds2D[i].DisplayCoords.y,
                          Backgrounds2D[i].BgSprite.ScaledWidth(),
                          Backgrounds2D[i].BgSprite.ScaledHeight()),
                    col);
              }
            }
            for (int i = 0; i < MaxCharacters2D; i++) {
              if (Characters2D[i].Status == LS_Loaded &&
                  Characters2D[i].Layer == layer && Characters2D[i].Show) {
                Characters2D[i].Render();
              }
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
          }
          if (GetFlag(SF_POKECOMENABLE)) {
            SetFlag(SF_DATEDISPLAY, 0);
            // hack
            ScrWork[6381] = 0;
            ScrWork[6382] = 0;
            ScrWork[6384] = 0;
          }
          if (Profile::Vm::GameInstructionSet == +Vm::InstructionSet::Dash) {
            /////////// DaSH hack? ///////
            if (GetFlag(2904) || GetFlag(2900) ||
                Scene3D::MainCamera.CameraTransform.Position !=
                    Profile::Scene3D::DefaultCameraPosition)
              SetFlag(1615, 0);
            else
              SetFlag(1615, 1);
            //////////////////////////////
          }
          DateDisplay::Render();
          break;
        }
        case TD_SystemText: {
          break;
        }
        case TD_SystemIcons: {
          LoadingDisplay::Render();
          break;
        }
        case TD_SystemMenu: {
          TitleMenu::Render();
          MainMenu::Render();
          break;
        }
        case TD_SystemMessage: {
          SysMesBox::Render();
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
    if (Backgrounds2D[0].Status == LS_Loaded) {
      Renderer2D::DrawSprite(
          Backgrounds2D[0].BgSprite,
          RectF(0.0f, 0.0f, Backgrounds2D[0].BgSprite.ScaledWidth(),
                Backgrounds2D[0].BgSprite.ScaledHeight()));
    }
    if (Characters2D[0].Status == LS_Loaded) {
      Characters2D[0].Render();
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