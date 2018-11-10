#include "game.h"

#include "window.h"
#include "../vendor/nuklear/nuklear_sdl_gl3.h"
#include "workqueue.h"
#include "modelviewer.h"
#include "log.h"
#include "inputsystem.h"
#include "audio/audiosystem.h"
#include "audio/audiochannel.h"
#include "audio/audiostream.h"
#include "renderer2d.h"
#include "3d/scene.h"
#include "mem.h"
#include "vm/gamespecific_rne.h"

namespace Impacto {

namespace Profile {
int LayerCount;
int GameFeatures;

std::string SystemArchiveName = "";
std::string BgmArchiveName = "";
std::string SeArchiveName = "";
std::string SysseArchiveName = "";
std::string VoiceArchiveName = "";

DialoguePageFeatureConfig Dlg;
}  // namespace Profile

static int const NkMaxVertexMemory = 256 * 1024;
static int const NkMaxElementMemory = 128 * 1024;

nk_context* Nk = 0;

namespace Game {
DrawComponentType DrawComponents[Vm::MaxThreads];

VfsArchive* SystemArchive = 0;
VfsArchive* BgmArchive = 0;
VfsArchive* SeArchive = 0;
VfsArchive* SysseArchive = 0;
VfsArchive* VoiceArchive = 0;

bool ShouldQuit = false;

static void Init() {
  Window::Init();

  memset(DrawComponents, TD_None, sizeof(DrawComponents));

  if (!Profile::SystemArchiveName.empty()) {
    IoError err =
        VfsArchive::Mount(Profile::SystemArchiveName.c_str(), &SystemArchive);
    if (err != IoError_OK) {
      ImpLog(LL_Fatal, LC_General, "Failed to load system archive!\n");
      Window::Shutdown();
      return;
    }
  }
  if (!Profile::BgmArchiveName.empty()) {
    IoError err =
        VfsArchive::Mount(Profile::BgmArchiveName.c_str(), &BgmArchive);
    if (err != IoError_OK) {
      ImpLog(LL_Fatal, LC_General, "Failed to load BGM archive!\n");
      Window::Shutdown();
      return;
    }
  }
  if (!Profile::SeArchiveName.empty()) {
    IoError err = VfsArchive::Mount(Profile::SeArchiveName.c_str(), &SeArchive);
    if (err != IoError_OK) {
      ImpLog(LL_Fatal, LC_General, "Failed to load SE archive!\n");
      Window::Shutdown();
      return;
    }
  }
  if (!Profile::SysseArchiveName.empty()) {
    IoError err =
        VfsArchive::Mount(Profile::SysseArchiveName.c_str(), &SysseArchive);
    if (err != IoError_OK) {
      ImpLog(LL_Fatal, LC_General, "Failed to load SYSSE archive!\n");
      Window::Shutdown();
      return;
    }
  }
  if (!Profile::VoiceArchiveName.empty()) {
    IoError err =
        VfsArchive::Mount(Profile::VoiceArchiveName.c_str(), &VoiceArchive);
    if (err != IoError_OK) {
      ImpLog(LL_Fatal, LC_General, "Failed to load VOICE archive!\n");
      Window::Shutdown();
      return;
    }
  }

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
    Renderer2D::Init();
  }

  for (int i = 0; i < DialoguePageCount; i++) {
    DialoguePages[i].Clear();
    DialoguePages[i].Mode = DPM_NVL;
    DialoguePages[i].Id = i;
  }

  if (Profile::GameFeatures & GameFeature::ModelViewer) {
    ModelViewer::Init();
  }

  if (Profile::GameFeatures & GameFeature::Sc3VirtualMachine) {
    Vm::Init(4, 0);
  }
}

void InitModelViewer() {
  Profile::LayerCount = 1;
  Profile::GameFeatures = GameFeature::Nuklear | GameFeature::Scene3D |
                          GameFeature::ModelViewer | GameFeature::Audio |
                          GameFeature::Input;
  Profile::BgmArchiveName = "bgm.cpk";
  Init();
}

void InitVmTest() {
  Profile::LayerCount = 1;
  Profile::GameFeatures = GameFeature::Sc3VirtualMachine |
                          GameFeature::Renderer2D | GameFeature::Scene3D |
                          GameFeature::Input | GameFeature::Audio;
  Profile::SystemArchiveName = "system.cpk";
  Profile::BgmArchiveName = "bgm.cpk";
  Profile::SeArchiveName = "se.cpk";
  Profile::SysseArchiveName = "sysse.cpk";
  Profile::VoiceArchiveName = "voice.cpk";
  Profile::Dlg = DialoguePageFeatureConfig_RNE;

  Init();

  // Font
  void* texFile;
  int64_t texSz;
  SystemArchive->Slurp(12, &texFile, &texSz);
  SDL_RWops* stream = SDL_RWFromConstMem(texFile, (int)texSz);
  Texture tex;
  tex.Load(stream);
  Profile::Dlg.DialogueFont.Sheet.Texture = tex.Submit();
  SDL_RWclose(stream);
  free(texFile);

  // Data sprite sheet
  void* dataTexFile;
  int64_t dataTexSz;
  SystemArchive->Slurp(9, &dataTexFile, &dataTexSz);
  SDL_RWops* dataStream = SDL_RWFromConstMem(dataTexFile, (int)dataTexSz);
  Texture dataTex;
  dataTex.Load(dataStream);
  Profile::Dlg.DataSpriteSheet.DesignHeight = dataTex.Height;
  Profile::Dlg.DataSpriteSheet.DesignWidth = dataTex.Width;
  Profile::Dlg.DataSpriteSheet.Texture = dataTex.Submit();
  SDL_RWclose(dataStream);
  free(dataTexFile);
}

void InitDialogueTest() {
  Profile::LayerCount = 1;
  Profile::GameFeatures = GameFeature::Renderer2D;
  Profile::SystemArchiveName = "system.cpk";
  Profile::Dlg = DialoguePageFeatureConfig_RNE;

  Init();

  void* texFile;
  int64_t texSz;
  SystemArchive->Slurp(12, &texFile, &texSz);
  SDL_RWops* stream = SDL_RWFromConstMem(texFile, (int)texSz);
  Texture tex;
  tex.Load(stream);
  Profile::Dlg.DialogueFont.Sheet.Texture = tex.Submit();
  SDL_RWclose(stream);
  free(texFile);

  DrawComponents[0] = TD_Text;

  Vm::Sc3VmThread dummy;
  uint8_t string[] = {0x11, 0x01, 0x18, 0x12, 0x00, 0xF0, 0x82, 0x22, 0x82,
                      0x23, 0x81, 0xA6, 0x82, 0x24, 0x81, 0x61, 0x82, 0x25,
                      0x81, 0x60, 0x81, 0x79, 0x81, 0x80, 0x80, 0xBE, 0x82,
                      0x26, 0x81, 0x64, 0x81, 0x65, 0x82, 0x27, 0x80, 0xED,
                      0x81, 0x78, 0x80, 0xBF, 0x08, 0xFF};
  uint8_t string2[] = {0x01, 0x82, 0x20, 0x82, 0x21, 0x02, 0x80,
                       0xD8, 0x80, 0xE3, 0x80, 0xE3, 0x80, 0xE3,
                       0x80, 0xE3, 0x80, 0xD9, 0x03, 0xFF};
  dummy.Ip = string2;
  DialoguePages[0].Mode = DPM_ADV;
  DialoguePages[0].ADVBoxOpacity = 0.0f;
  DialoguePages[0].AnimState = DPAS_Showing;
  DialoguePages[0].AddString(&dummy);
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

  if (Profile::GameFeatures & GameFeature::Sc3VirtualMachine) {
    Vm::Update();
  }

  if (Profile::GameFeatures & GameFeature::Audio) {
    Audio::AudioUpdate(dt);
  }

  if (Profile::GameFeatures & GameFeature::Scene3D) {
    Scene3D::Update(dt);
  }

  if (Profile::GameFeatures & GameFeature::Renderer2D) {
    for (int i = 0; i < DialoguePageCount; i++) DialoguePages[i].Update(dt);
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
    Vm::SetDateDisplay();
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
          }
        }
        default: {
          ImpLog(LL_Error, LC_General,
                 "Encountered unknown draw component type %02X\n",
                 DrawComponents[i]);
          break;
        }
      }
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