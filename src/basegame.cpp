#include "basegame.h"

#include "window.h"
#include "../vendor/nuklear/nuklear_sdl_gl3.h"
#include "workqueue.h"

namespace Impacto {

static int const NkMaxVertexMemory = 1024 * 1024;
static int const NkMaxElementMemory = 1024 * 1024;

BaseGame::BaseGame(GameFeatureConfig const& config)
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
    Scene3D->MainCamera.Init();
  }
}

BaseGame::~BaseGame() {
  if (GameFeatures & GameFeature_Scene3D) {
    if (Scene3D) {
      delete Scene3D;
      Scene3D = 0;
    }
  }

  if (GameFeatures & GameFeature_Nuklear) {
    nk_sdl_shutdown();
  }
  WindowShutdown();
}

void BaseGame::Update(float dt) {
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

  GameUpdate(dt);
  if (GameFeatures & GameFeature_Scene3D) {
    Scene3D->Update(dt);
  }
}

void BaseGame::Render() {
  WindowGetDimensions();
  glViewport(0, 0, g_WindowWidth, g_WindowHeight);

  glClearColor(0.2f, 0.2f, 0.2f, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for (uint32_t layer = 0; layer < LayerCount; layer++) {
    DrawLayer(layer);
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

  SDL_GL_SwapWindow(g_SDLWindow);
}

}  // namespace Impacto