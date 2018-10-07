#include "game.h"

#include "window.h"
#include "../vendor/nuklear/nuklear_sdl_gl3.h"
#include "workqueue.h"
#include "modelviewer.h"

namespace Impacto {

static int const NkMaxVertexMemory = 1024 * 1024;
static int const NkMaxElementMemory = 1024 * 1024;

static int const GameScrWorkSize = 32000;
static int const GameFlagWorkSize = 1000;

Game* g_Game;

Game::Game(GameFeatureConfig const& config)
    : LayerCount(config.LayerCount), GameFeatures(config.GameFeatures) {
  WindowInit();

  if (GameFeatures & GameFeature_Nuklear) {
    Nk = nk_sdl_init(g_SDLWindow);
    struct nk_font_atlas* atlas;
    nk_sdl_font_stash_begin(&atlas);
    // no fonts => default font used, but we still have do the setup
    nk_sdl_font_stash_end();
  }

  if (GameFeatures & GameFeature_Scene3D) {
    SceneInit();
    Scene3D = new Scene;
  }

  if (GameFeatures & GameFeature_ModelViewer) {
    ModelViewerComponent = new ModelViewer;
  }

  if (GameFeatures & GameFeature_Sc3VirtualMachine) {
    VmComponent = new Vm::Vm(this);
    ScrWork = (uint32_t*)calloc(GameScrWorkSize, sizeof(uint32_t));
    FlagWork = (uint8_t*)calloc(GameFlagWorkSize, sizeof(uint8_t));
  }
}

void Game::Init() {
  if (GameFeatures & GameFeature_Scene3D) {
    Scene3D->Init();
  }
  if (GameFeatures & GameFeature_ModelViewer) {
    ModelViewerComponent->Init();
  }

  if (GameFeatures & GameFeature_Sc3VirtualMachine) {
    VmComponent->Init(4, 0);
  }
}

void Game::InitModelViewer() {
  assert(g_Game == 0);
  GameFeatureConfig config;
  config.LayerCount = 1;
  config.GameFeatures =
      GameFeature_Nuklear | GameFeature_Scene3D | GameFeature_ModelViewer;
  config.Scene3D_BackgroundCount = 1;
  config.Scene3D_CharacterCount = 1;

  g_Game = new Game(config);
  g_Game->Init();
}

void Game::InitVmTest() {
  assert(g_Game == 0);
  GameFeatureConfig config;
  config.LayerCount = 1;
  config.GameFeatures = GameFeature_Sc3VirtualMachine;

  g_Game = new Game(config);
  g_Game->Init();
}

Game::~Game() {
  if (GameFeatures & GameFeature_ModelViewer) {
    delete ModelViewerComponent;
  }

  if (GameFeatures & GameFeature_Sc3VirtualMachine) {
    if (ScrWork) free(ScrWork);
    if (FlagWork) free(FlagWork);
    delete VmComponent;
  }

  if (GameFeatures & GameFeature_Scene3D) {
    if (Scene3D) {
      delete Scene3D;
    }
  }

  if (GameFeatures & GameFeature_Nuklear) {
    nk_sdl_shutdown();
  }
  WindowShutdown();
}

void Game::Update(float dt) {
  SDL_Event e;
  if (GameFeatures & GameFeature_Nuklear) {
    nk_input_begin(Nk);
  }
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      ShouldQuit = true;
    }

    if (GameFeatures & GameFeature_Nuklear) {
      nk_sdl_handle_event(&e);
    }
    WorkQueueHandleEvent(&e);
  }
  if (GameFeatures & GameFeature_Nuklear) {
    nk_input_end(Nk);
  }

  if (GameFeatures & GameFeature_ModelViewer) {
    ModelViewerComponent->Update(dt);
  }

  if (GameFeatures & GameFeature_Sc3VirtualMachine) {
    VmComponent->Update();
  }

  if (GameFeatures & GameFeature_Scene3D) {
    Scene3D->Update(dt);
  }
}

void Game::Render() {
  WindowUpdate();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (GameFeatures & GameFeature_Scene3D) {
    Scene3D->Render();
  }

  for (uint32_t layer = 0; layer < LayerCount; layer++) {
    // TODO
  }

  if (GameFeatures & GameFeature_Nuklear) {
#ifdef IMPACTO_GL_DEBUG
    // Nuklear spams these
    glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                             GL_FALSE);
#endif
    nk_sdl_render(NK_ANTI_ALIASING_OFF, NkMaxVertexMemory, NkMaxElementMemory);
#ifdef IMPACTO_GL_DEBUG
    glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                             GL_TRUE);
#endif
  }

  WindowDraw();
}

void Game::SetFlag(uint32_t flagId, uint32_t value) {
  uint32_t flagIndex = flagId >> 3;
  int flagValue = 1 << (flagId - 8 * (flagId >> 3));
  FlagWork[flagIndex] |= flagValue;
  if (!value) {
    FlagWork[flagIndex] ^= flagValue;
  }
}

bool Game::GetFlag(uint32_t flagId) {
  return ((uint8_t)(1 << (flagId - 8 * (flagId >> 3))) &
          *((uint8_t*)FlagWork + (flagId >> 3))) != 0;
}

}  // namespace Impacto