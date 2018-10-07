#include "scene.h"

#include "camera.h"
#include "../log.h"
#include "../window.h"
#include "../workqueue.h"
#include "../shader.h"

namespace Impacto {

static bool IsInit = false;

void SceneInit() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Scene, "Initializing 3D scene system\n");
  IsInit = true;

  Character3DInit();
  Background3DInit();
}

void Scene::Init() {
  MainCamera.Init();

  glGenVertexArrays(1, &VAOScreenFillingTriangle);
  glBindVertexArray(VAOScreenFillingTriangle);
  glGenBuffers(1, &VBOScreenFillingTriangle);
  glBindBuffer(GL_ARRAY_BUFFER, VBOScreenFillingTriangle);

  // clang-format off
  float ScreenFillingTriangle[] = {
      // Position       // UV
      -1.0f, -1.0f,     0.0f, 0.0f,
      3.0f, -1.0f,      2.0f, 0.0f,
      -1.0f, 3.0f,      0.0f, 2.0f
  };
  // clang-format on

  glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenFillingTriangle),
               ScreenFillingTriangle, GL_STATIC_DRAW);
  // Position
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
}

Scene::~Scene() {
  if (VBOScreenFillingTriangle) glDeleteBuffers(1, &VBOScreenFillingTriangle);
  if (VAOScreenFillingTriangle)
    glDeleteVertexArrays(1, &VAOScreenFillingTriangle);
  CleanFramebufferState();
}

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
  MainCamera.AspectRatio = (float)g_WindowWidth / (float)g_WindowHeight;
  MainCamera.Recalculate();

  Background3DUpdateGpu(this, &MainCamera);
  Character3DUpdateGpu(this, &MainCamera);

  if (CurrentBackgroundLoadStatus == OLS_Loaded) {
    CurrentBackground.Render();
  }

  // Draw background without MSAA

  SetupFramebufferState();

  if (CurrentCharacterLoadStatus == OLS_Loaded) {
    CurrentCharacter.Render();
  }

  DrawToScreen();
}

void Scene::SetupFramebufferState() {
  if (g_FramebuffersNeedUpdate) {
    CleanFramebufferState();

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

      glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    } else {
      textureTarget = GL_TEXTURE_2D_MULTISAMPLE;
      glBindTexture(textureTarget, RenderTextureColor);
      glTexImage2DMultisample(textureTarget, g_MsaaCount, GL_RGBA,
                              WindowGetScaledWidth(), WindowGetScaledHeight(),
                              GL_FALSE);
    }

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

    ShaderParamMap shaderParams;
    shaderParams["MultisampleCount"] = g_MsaaCount;
    shaderParams["WindowDimensions"] = glm::vec2(g_WindowWidth, g_WindowHeight);
    shaderParams["RenderScale"] = g_RenderScale;

    ShaderProgram = ShaderCompile("SceneToRT", shaderParams);
    glUseProgram(ShaderProgram);
    glUniform1i(glGetUniformLocation(ShaderProgram, "Framebuffer3D"), 0);
    glUniform1i(glGetUniformLocation(ShaderProgram, "Framebuffer3DMS"), 1);
  } else {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
  }

  glViewport(0, 0, WindowGetScaledWidth(), WindowGetScaledHeight());
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE,
                      GL_ONE_MINUS_SRC_ALPHA);
}

void Scene::CleanFramebufferState() {
  if (ShaderProgram) {
    glDeleteProgram(ShaderProgram);
    ShaderProgram = 0;
  }
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
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  // TODO: Better than linear filtering for supersampling

  if (g_MsaaCount == 0) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, RenderTextureColor);
  } else {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, RenderTextureColor);
  }
  glBindVertexArray(VAOScreenFillingTriangle);
  glUseProgram(ShaderProgram);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

}  // namespace Impacto