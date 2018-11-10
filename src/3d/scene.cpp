#include "scene.h"

#include "camera.h"
#include "../log.h"
#include "../window.h"
#include "../workqueue.h"
#include "../shader.h"

namespace Impacto {
namespace Scene3D {

enum MSResolveMode {
  MS_None,
  // Use a framebuffer with multisample texture
  MS_MultisampleTexture,
  // Use a framebuffer with singlesample texture provided by
  // EXT_multisampled_render_to_texture
  MS_SinglesampleTextureExt,
  // Use a framebuffer with multisample renderbuffer and blit to framebuffer
  // with singlesample texture
  MS_BlitFromRenderbuffer
};

static bool IsInit = false;

Camera MainCamera;

Background3D Backgrounds[MaxBackgrounds];
Character3D Characters[MaxCharacters];

glm::vec3 LightPosition;
glm::vec4 Tint;
bool DarkMode;

uint32_t CharacterToLoadId;

static void SetupFramebufferState();
static void CleanFramebufferState();
static void DrawToScreen();

static MSResolveMode CheckMSResolveMode();

static GLuint FBO = 0;
static GLuint RenderTextureColor = 0;
static GLuint RenderTextureDS = 0;

// Only for MS_BlitFromRenderbuffer
static GLuint FBOMultisample = 0;
static GLuint RenderbufferColor = 0;
static GLuint RenderbufferDS = 0;

static GLuint VAOScreenFillingTriangle = 0;
static GLuint VBOScreenFillingTriangle = 0;

static GLuint ShaderProgram = 0;

void Init() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Scene, "Initializing 3D scene system\n");
  IsInit = true;

  Character3D::Init();
  Background3D::Init();

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

void Shutdown() {
  if (!IsInit) return;
  if (VBOScreenFillingTriangle) glDeleteBuffers(1, &VBOScreenFillingTriangle);
  if (VAOScreenFillingTriangle)
    glDeleteVertexArrays(1, &VAOScreenFillingTriangle);
  CleanFramebufferState();
}

void Update(float dt) {
  for (int i = 0; i < MaxCharacters; i++) {
    if (Characters[i].Status == LS_Loaded) {
      Characters[i].Update(dt);
    }
  }
}
void Render() {
  RectF viewport = Window::GetViewport();
  MainCamera.AspectRatio = viewport.Width / viewport.Height;
  MainCamera.Recalculate();

  Background3D::UpdateGpu(&MainCamera);
  Character3D::UpdateGpu(&MainCamera);

  for (int i = 0; i < MaxBackgrounds; i++) {
    if (Backgrounds[i].Status == LS_Loaded) {
      Backgrounds[i].Render();
    }
  }

  // Draw background without MSAA

  SetupFramebufferState();

  for (int i = 0; i < MaxCharacters; i++) {
    if (Characters[i].Status == LS_Loaded) {
      Characters[i].Render();
    }
  }

  DrawToScreen();
}

static void SetupFramebufferState() {
  Rect viewport = Window::GetViewport();
  Rect scaledViewport = Window::GetScaledViewport();

  MSResolveMode msaa = CheckMSResolveMode();

  if (Window::WindowDimensionsChanged) {
    CleanFramebufferState();

    switch (msaa) {
      case MS_None:
        ImpLog(LL_Info, LC_Render,
               "Creating 3D scene framebuffer %dx%d (=> %dx%d), no MSAA\n",
               scaledViewport.Width, scaledViewport.Height, viewport.Width,
               viewport.Height);
        break;
      case MS_MultisampleTexture:
        ImpLog(LL_Info, LC_Render,
               "Creating 3D scene framebuffer %dx%d (=> %dx%d), %dx MSAA using "
               "multisample texture\n",
               scaledViewport.Width, scaledViewport.Height, viewport.Width,
               viewport.Height, Window::MsaaCount);
        break;
      case MS_SinglesampleTextureExt:
        ImpLog(LL_Info, LC_Render,
               "Creating 3D scene framebuffer %dx%d (=> %dx%d), %dx MSAA using "
               "multisampled_render_to_texture\n",
               scaledViewport.Width, scaledViewport.Height, viewport.Width,
               viewport.Height, Window::MsaaCount);
        break;
      case MS_BlitFromRenderbuffer:
        ImpLog(LL_Info, LC_Render,
               "Creating 3D scene framebuffer %dx%d (=> %dx%d), %dx MSAA using "
               "blit from renderbuffer\n",
               scaledViewport.Width, scaledViewport.Height, viewport.Width,
               viewport.Height, Window::MsaaCount);
        break;
    }

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
    glGenTextures(1, &RenderTextureColor);

    GLenum textureTarget;
    if (msaa == MS_MultisampleTexture) {
      textureTarget = GL_TEXTURE_2D_MULTISAMPLE;
      glBindTexture(textureTarget, RenderTextureColor);
      glTexImage2DMultisample(textureTarget, Window::MsaaCount, GL_RGBA,
                              scaledViewport.Width, scaledViewport.Height,
                              GL_FALSE);
    } else {
      textureTarget = GL_TEXTURE_2D;
      glBindTexture(textureTarget, RenderTextureColor);
      glTexImage2D(textureTarget, 0, GL_RGBA, scaledViewport.Width,
                   scaledViewport.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

      glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    if (msaa == MS_SinglesampleTextureExt) {
      glFramebufferTexture2DMultisampleEXT(
          GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget,
          RenderTextureColor, 0, Window::MsaaCount);
    } else {
      glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             textureTarget, RenderTextureColor, 0);
    }

    // If blitting from renderbuffer, our destination framebuffer does not need
    // depth/stencil since we aren't rendering 3D content on it directly
    if (msaa != MS_BlitFromRenderbuffer) {
      // Combining multisampled renderbuffer + multisampled texture does not
      // work on Nvidia for me, so only use DS renderbuffer for the other cases
      if (msaa == MS_None || msaa == MS_SinglesampleTextureExt) {
        glGenRenderbuffers(1, &RenderbufferDS);
        glBindRenderbuffer(GL_RENDERBUFFER, RenderbufferDS);
        if (msaa == MS_SinglesampleTextureExt) {
          glRenderbufferStorageMultisampleEXT(
              GL_RENDERBUFFER, Window::MsaaCount, GL_DEPTH24_STENCIL8,
              scaledViewport.Width, scaledViewport.Height);
        } else {
          glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                                scaledViewport.Width, scaledViewport.Height);
        }
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,
                                  GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                  RenderbufferDS);
      } else {
        // MS_MultisampleTexture only
        textureTarget = GL_TEXTURE_2D_MULTISAMPLE;
        glGenTextures(1, &RenderTextureDS);
        glBindTexture(textureTarget, RenderTextureDS);
        glTexImage2DMultisample(textureTarget, Window::MsaaCount,
                                GL_DEPTH_STENCIL, scaledViewport.Width,
                                scaledViewport.Height, GL_FALSE);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                               textureTarget, RenderTextureDS, 0);
      }
    }

    ShaderParamMap shaderParams;
    if (msaa == MS_MultisampleTexture) {
      shaderParams["MultisampleCount"] = Window::MsaaCount;
      shaderParams["MSAA_MODE_MULTISAMPLE_TEXTURE"] = ShaderParameter(1, true);
    }
    shaderParams["WindowDimensions"] =
        glm::vec2(viewport.Width, viewport.Height);
    shaderParams["RenderScale"] = Window::RenderScale;

    ShaderProgram = ShaderCompile("SceneToRT", shaderParams);
    glUseProgram(ShaderProgram);
    glUniform1i(glGetUniformLocation(ShaderProgram, "Framebuffer3D"), 0);

    ImpLog(LL_Debug, LC_Render, "FBO status: 0x%04X\n",
           glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));

    if (msaa == MS_BlitFromRenderbuffer) {
      glGenFramebuffers(1, &FBOMultisample);
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBOMultisample);
      glGenRenderbuffers(1, &RenderbufferColor);
      glGenRenderbuffers(1, &RenderbufferDS);

      glBindRenderbuffer(GL_RENDERBUFFER, RenderbufferColor);
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, Window::MsaaCount,
                                       GL_RGBA8, scaledViewport.Width,
                                       scaledViewport.Height);
      glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_RENDERBUFFER, RenderbufferColor);

      glBindRenderbuffer(GL_RENDERBUFFER, RenderbufferDS);
      glRenderbufferStorageMultisample(
          GL_RENDERBUFFER, Window::MsaaCount, GL_DEPTH24_STENCIL8,
          scaledViewport.Width, scaledViewport.Height);
      glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,
                                GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                RenderbufferDS);
    }
  } else {
    if (msaa == MS_BlitFromRenderbuffer) {
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBOMultisample);
    } else {
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
    }
  }

  glViewport(0, 0, scaledViewport.Width, scaledViewport.Height);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE,
                      GL_ONE_MINUS_SRC_ALPHA);
}

static void CleanFramebufferState() {
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
  if (RenderbufferDS) {
    glDeleteRenderbuffers(1, &RenderbufferDS);
    RenderbufferDS = 0;
  }
  if (RenderbufferColor) {
    glDeleteRenderbuffers(1, &RenderbufferColor);
    RenderbufferColor = 0;
  }
  if (FBO) {
    glDeleteFramebuffers(1, &FBO);
    FBO = 0;
  }
  if (FBOMultisample) {
    glDeleteFramebuffers(1, &FBOMultisample);
    FBOMultisample = 0;
  }
}

static void DrawToScreen() {
  Rect viewport = Window::GetViewport();
  Rect scaledViewport = Window::GetScaledViewport();

  MSResolveMode msaa = CheckMSResolveMode();

  if (msaa == MS_BlitFromRenderbuffer) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBOMultisample);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

    glBlitFramebuffer(0, 0, scaledViewport.Width, scaledViewport.Height, 0, 0,
                      scaledViewport.Width, scaledViewport.Height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    GLenum attachments[2] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_STENCIL_ATTACHMENT};
    glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, 2, attachments);
  }

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Window::DrawRT);
  glViewport(0, 0, viewport.Width, viewport.Height);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  // TODO: Better than linear filtering for supersampling

  glActiveTexture(GL_TEXTURE0);
  if (msaa == MS_MultisampleTexture) {
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, RenderTextureColor);
  } else {
    glBindTexture(GL_TEXTURE_2D, RenderTextureColor);
  }

  glBindVertexArray(VAOScreenFillingTriangle);
  glUseProgram(ShaderProgram);
  glDrawArrays(GL_TRIANGLES, 0, 3);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static MSResolveMode CheckMSResolveMode() {
  if (Window::MsaaCount == 0) return MS_None;

  if (Window::ActualGraphicsApi != Window::GfxApi_GL) {
    if (GLAD_GL_EXT_multisampled_render_to_texture) {
      return MS_SinglesampleTextureExt;
    }
    return MS_BlitFromRenderbuffer;
  }

  // return MS_MultisampleTexture;
  return MS_BlitFromRenderbuffer;  // Apparently this is actually slightly
                                   // faster, who'd've thunk..
}

}  // namespace Scene3D
}  // namespace Impacto