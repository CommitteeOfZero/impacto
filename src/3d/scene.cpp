#include "scene.h"

#include "camera.h"
#include "../log.h"
#include "../window.h"
#include "../workqueue.h"

namespace Impacto {

bool IsInit = false;

void SceneInit() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Scene, "Initializing 3D scene system\n");
  IsInit = true;

  Character3DInit();
  Background3DInit();
}

Scene::~Scene() { CleanFBO(); }

static void LoadBackgroundWorker(void* ptr) {
  Scene* scene = (Scene*)ptr;
  scene->CurrentBackground.Load(scene->BackgroundToLoadId);
}

static void OnBackgroundLoaded(void* ptr) {
  Scene* scene = (Scene*)ptr;
  scene->CurrentBackground.Submit();
  scene->CurrentBackgroundLoadStatus = OLS_Loaded;
}

bool Scene::LoadBackgroundAsync(uint32_t id) {
  if (CurrentBackgroundLoadStatus == OLS_Loading) {
    // Cannot currently cancel a load
    return false;
  }
  if (CurrentBackgroundLoadStatus == OLS_Loaded) {
    CurrentBackground.Unload();
    CurrentBackgroundLoadStatus = OLS_Unloaded;
  }

  BackgroundToLoadId = id;
  CurrentBackgroundLoadStatus = OLS_Loading;
  WorkQueuePush(this, &LoadBackgroundWorker, &OnBackgroundLoaded);
  return true;
}

static void LoadCharacterWorker(void* ptr) {
  Scene* scene = (Scene*)ptr;
  scene->CurrentCharacter.Load(scene->CharacterToLoadId);
}

static void OnCharacterLoaded(void* ptr) {
  Scene* scene = (Scene*)ptr;
  scene->CurrentCharacter.Submit();
  scene->CurrentCharacterLoadStatus = OLS_Loaded;
}

bool Scene::LoadCharacterAsync(uint32_t id) {
  if (CurrentCharacterLoadStatus == OLS_Loading) {
    // Cannot currently cancel a load
    return false;
  }
  if (CurrentCharacterLoadStatus == OLS_Loaded) {
    CurrentCharacter.Unload();
    CurrentCharacterLoadStatus = OLS_Unloaded;
  }

  CharacterToLoadId = id;
  CurrentCharacterLoadStatus = OLS_Loading;
  WorkQueuePush(this, &LoadCharacterWorker, &OnCharacterLoaded);
  return true;
}

void Scene::Update(float dt) {
  if (CurrentCharacterLoadStatus == OLS_Loaded) {
    CurrentCharacter.Update(dt);
  }
}
void Scene::Render() {
  SetupFBO();

  MainCamera.AspectRatio = (float)g_WindowWidth / (float)g_WindowHeight;
  MainCamera.Recalculate();

  Background3DUpdateGpu(this, &MainCamera);
  Character3DUpdateGpu(this, &MainCamera);

  if (CurrentBackgroundLoadStatus == OLS_Loaded) {
    CurrentBackground.Render();
  }
  if (CurrentCharacterLoadStatus == OLS_Loaded) {
    CurrentCharacter.Render();
  }

  DrawToScreen();
}

void Scene::SetupFBO() {
  if (g_FramebuffersNeedUpdate) {
    CleanFBO();

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
    glGenTextures(1, &RenderTextureColor);
    glGenTextures(1, &RenderTextureDS);

    // TODO MSAA doesn't work this way on GLES - use
    // https://www.khronos.org/registry/OpenGL/extensions/EXT/EXT_multisampled_render_to_texture.txt
    // or render to renderbuffer

    GLenum textureTarget;
    if (g_MsaaCount == 0) {
      textureTarget = GL_TEXTURE_2D;
      glBindTexture(textureTarget, RenderTextureColor);
      glTexImage2D(textureTarget, 0, GL_RGBA, WindowGetScaledWidth(),
                   WindowGetScaledHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    } else {
      // glBlitFramebuffer() with differently sized source and destination
      // rectangles is not allowed for multisample textures
      // TODO: draw quad instead of copy
      assert(g_RenderScale == 1.0f);
      textureTarget = GL_TEXTURE_2D_MULTISAMPLE;
      glBindTexture(textureTarget, RenderTextureColor);
      glTexImage2DMultisample(textureTarget, g_MsaaCount, GL_RGBA,
                              WindowGetScaledWidth(), WindowGetScaledHeight(),
                              GL_FALSE);
    }

    glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           textureTarget, RenderTextureColor, 0);

    if (g_MsaaCount == 0) {
      textureTarget = GL_TEXTURE_2D;
      glBindTexture(textureTarget, RenderTextureDS);
      glTexImage2D(textureTarget, 0, GL_DEPTH_STENCIL, WindowGetScaledWidth(),
                   WindowGetScaledHeight(), 0, GL_DEPTH_STENCIL,
                   GL_UNSIGNED_INT_24_8, NULL);
    } else {
      textureTarget = GL_TEXTURE_2D_MULTISAMPLE;
      glBindTexture(textureTarget, RenderTextureDS);
      glTexImage2DMultisample(textureTarget, g_MsaaCount, GL_DEPTH_STENCIL,
                              WindowGetScaledWidth(), WindowGetScaledHeight(),
                              GL_FALSE);
    }

    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                           textureTarget, RenderTextureDS, 0);

  } else {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
  }

  glViewport(0, 0, WindowGetScaledWidth(), WindowGetScaledHeight());
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Scene::CleanFBO() {
  if (RenderTextureColor) {
    glDeleteTextures(1, &RenderTextureColor);
    RenderTextureColor = 0;
  }
  if (RenderTextureDS) {
    glDeleteTextures(1, &RenderTextureDS);
    RenderTextureDS = 0;
  }
  if (FBO) {
    glDeleteFramebuffers(1, &FBO);
    FBO = 0;
  }
}

void Scene::DrawToScreen() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_DrawRT);

  glViewport(0, 0, g_WindowWidth, g_WindowHeight);
  glDisable(GL_DEPTH_TEST);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);

  // TODO: Better than linear filtering for supersampling

  glBlitFramebuffer(0, 0, WindowGetScaledWidth(), WindowGetScaledHeight(), 0, 0,
                    g_WindowWidth, g_WindowHeight, GL_COLOR_BUFFER_BIT,
                    GL_LINEAR);
}

}  // namespace Impacto