#include "scene.h"

#include "../../3d/camera.h"
#include "../../../log.h"
#include "../../../workqueue.h"
#include "../glc.h"
#include "renderable3d.h"

#include "../../../profile/scene3d.h"

namespace Impacto {
namespace OpenGL {

Scene3D::Scene3D(GLWindow* window, ShaderCompiler& shaderCompiler)
    : Window(window), Shaders(shaderCompiler) {}

void Scene3D::Init() {
  assert(IsInit == false);
  ImpLog(LogLevel::Info, LogChannel::Scene, "Initializing 3D scene system\n");
  IsInit = true;

  Profile::Scene3D::Configure();

  Renderables = new IRenderable3D*[Profile::Scene3D::MaxRenderables];
  for (int i = 0; i < Profile::Scene3D::MaxRenderables; i++) {
    Renderables[i] = new Renderable3D();
  }

  Renderable3D::Init(Window, Shaders);

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

void Scene3D::Shutdown() {
  if (!IsInit) return;
  if (VBOScreenFillingTriangle) glDeleteBuffers(1, &VBOScreenFillingTriangle);
  if (VAOScreenFillingTriangle)
    glDeleteVertexArrays(1, &VAOScreenFillingTriangle);
  CleanFramebufferState();
  if (Renderables) delete[] Renderables;
}

void Scene3D::Update(float dt) {
  for (int i = 0; i < Profile::Scene3D::MaxRenderables; i++) {
    if (Renderables[i]->Status == LoadStatus::Loaded) {
      Renderables[i]->Update(dt);
    }
  }
}
void Scene3D::Render() {
  RectF viewport = Window->GetViewport();
  MainCamera.AspectRatio = viewport.Width / viewport.Height;
  MainCamera.Recalculate();

  Renderable3D::BeginFrame(this, &MainCamera);

  for (int i = 0; i < Profile::Scene3D::MaxRenderables; i++) {
    if (Renderables[i]->Status == LoadStatus::Loaded &&
        Renderables[i]->StaticModel->Type == ModelType_Background) {
      Renderables[i]->Render();
    }
  }

  // Draw background without MSAA

  SetupFramebufferState();

  for (int i = 0; i < Profile::Scene3D::MaxRenderables; i++) {
    if (Renderables[i]->Status == LoadStatus::Loaded &&
        Renderables[i]->StaticModel->Type == ModelType_Character) {
      Renderables[i]->Render();
    }
  }

  DrawToScreen();
}

void Scene3D::SetupFramebufferState() {
  Rect viewport = Window->GetViewport();
  Rect scaledViewport = Window->GetScaledViewport();

  MSResolveMode msaa = CheckMSResolveMode();

  if (Window->WindowDimensionsChanged) {
    CleanFramebufferState();

    switch (msaa) {
      case MS_None:
        ImpLog(
            LogLevel::Info, LogChannel::Render,
            "Creating 3D scene framebuffer {:d}x{:d} (=> {:d}x{:d}), no MSAA\n",
            scaledViewport.Width, scaledViewport.Height, viewport.Width,
            viewport.Height);
        break;
      case MS_MultisampleTexture:
        ImpLog(LogLevel::Info, LogChannel::Render,
               "Creating 3D scene framebuffer {:d}x{:d} (=> {:d}x{:d}), {:d}x "
               "MSAA using multisample texture\n",
               scaledViewport.Width, scaledViewport.Height, viewport.Width,
               viewport.Height, Window->MsaaCount);
        break;
      case MS_SinglesampleTextureExt:
        ImpLog(LogLevel::Info, LogChannel::Render,
               "Creating 3D scene framebuffer {:d}x{:d} (=> {:d}x{:d}), {:d}x "
               "MSAA using multisampled_render_to_texture\n",
               scaledViewport.Width, scaledViewport.Height, viewport.Width,
               viewport.Height, Window->MsaaCount);
        break;
      case MS_BlitFromRenderbuffer:
        ImpLog(LogLevel::Info, LogChannel::Render,
               "Creating 3D scene framebuffer {:d}x{:d} (=> {:d}x{:d}), {:d}x "
               "MSAA using blit from renderbuffer\n",
               scaledViewport.Width, scaledViewport.Height, viewport.Width,
               viewport.Height, Window->MsaaCount);
        break;
    }

    glGenFramebuffers(1, &FBO);
    GLC::BindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
    glGenTextures(1, &RenderTextureColor);

    GLenum textureTarget;
    if (msaa == MS_MultisampleTexture) {
      textureTarget = GL_TEXTURE_2D_MULTISAMPLE;
      glBindTexture(textureTarget, RenderTextureColor);
      glTexImage2DMultisample(textureTarget, Window->MsaaCount, GL_RGBA,
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
          RenderTextureColor, 0, Window->MsaaCount);
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
              GL_RENDERBUFFER, Window->MsaaCount, GL_DEPTH24_STENCIL8,
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
        glTexImage2DMultisample(textureTarget, Window->MsaaCount,
                                GL_DEPTH_STENCIL, scaledViewport.Width,
                                scaledViewport.Height, GL_FALSE);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                               textureTarget, RenderTextureDS, 0);
      }
    }

    ShaderParamMap shaderParams;
    if (msaa == MS_MultisampleTexture) {
      shaderParams["MultisampleCount"] = Window->MsaaCount;
      shaderParams["MSAA_MODE_MULTISAMPLE_TEXTURE"] = ShaderParameter(1, true);
    }
    shaderParams["WindowDimensions"] =
        glm::vec2(viewport.Width, viewport.Height);
    shaderParams["RenderScale"] = Window->RenderScale;

    ShaderProgram = Shaders.Compile("SceneToRT", shaderParams);
    glUseProgram(ShaderProgram);
    glUniform1i(glGetUniformLocation(ShaderProgram, "Framebuffer3D"), 0);

    ImpLog(LogLevel::Debug, LogChannel::Render, "FBO status: 0x{:08X}\n",
           glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER));

    if (msaa == MS_BlitFromRenderbuffer) {
      glGenFramebuffers(1, &FBOMultisample);
      GLC::BindFramebuffer(GL_DRAW_FRAMEBUFFER, FBOMultisample);
      glGenRenderbuffers(1, &RenderbufferColor);
      glGenRenderbuffers(1, &RenderbufferDS);

      glBindRenderbuffer(GL_RENDERBUFFER, RenderbufferColor);
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, Window->MsaaCount,
                                       GL_RGBA8, scaledViewport.Width,
                                       scaledViewport.Height);
      glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_RENDERBUFFER, RenderbufferColor);

      glBindRenderbuffer(GL_RENDERBUFFER, RenderbufferDS);
      glRenderbufferStorageMultisample(
          GL_RENDERBUFFER, Window->MsaaCount, GL_DEPTH24_STENCIL8,
          scaledViewport.Width, scaledViewport.Height);
      glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,
                                GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                                RenderbufferDS);
    }
  } else {
    if (msaa == MS_BlitFromRenderbuffer) {
      GLC::BindFramebuffer(GL_DRAW_FRAMEBUFFER, FBOMultisample);
    } else {
      GLC::BindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
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

void Scene3D::CleanFramebufferState() {
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
    GLC::DeleteFramebuffers(1, &FBO);
    FBO = 0;
  }
  if (FBOMultisample) {
    GLC::DeleteFramebuffers(1, &FBOMultisample);
    FBOMultisample = 0;
  }
}

void Scene3D::DrawToScreen() {
  Rect viewport = Window->GetViewport();
  Rect scaledViewport = Window->GetScaledViewport();

  MSResolveMode msaa = CheckMSResolveMode();

  if (msaa == MS_BlitFromRenderbuffer) {
    GLC::BindFramebuffer(GL_READ_FRAMEBUFFER, FBOMultisample);
    GLC::BindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);

    glBlitFramebuffer(0, 0, scaledViewport.Width, scaledViewport.Height, 0, 0,
                      scaledViewport.Width, scaledViewport.Height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    GLenum attachments[2] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_STENCIL_ATTACHMENT};
    if (glInvalidateFramebuffer)
      glInvalidateFramebuffer(GL_READ_FRAMEBUFFER, 2, attachments);
  }

  GLC::BindFramebuffer(GL_DRAW_FRAMEBUFFER, Window->DrawRT);
  glViewport(0, 0, viewport.Width, viewport.Height);

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

MSResolveMode Scene3D::CheckMSResolveMode() {
  if (Window->MsaaCount == 1) return MS_None;

  if (ActualGraphicsApi != GfxApi_GL) {
    if (GLAD_GL_EXT_multisampled_render_to_texture) {
      return MS_SinglesampleTextureExt;
    }
    return MS_BlitFromRenderbuffer;
  }

  // return MS_MultisampleTexture;
  return MS_BlitFromRenderbuffer;  // Apparently this is actually slightly
                                   // faster, who'd've thunk..
}

}  // namespace OpenGL
}  // namespace Impacto