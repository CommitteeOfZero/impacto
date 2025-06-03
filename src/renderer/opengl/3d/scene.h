#pragma once

#include <glm/glm.hpp>

#include "../../3d/scene.h"
#include "../shader.h"
#include "../window.h"
#include "../renderer.h"

namespace Impacto {
namespace OpenGL {

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

class Scene3D : public IScene3D {
 public:
  Scene3D(GLWindow* window, std::shared_ptr<ShaderCompiler> shaderCompiler);

  void Init();
  void Shutdown();
  void Update(float dt);
  void Render();

 private:
  bool IsInit = false;

  GLWindow* Window;
  std::shared_ptr<ShaderCompiler> Shaders;

  void SetupFramebufferState();
  void CleanFramebufferState();
  void DrawToScreen();

  MSResolveMode CheckMSResolveMode();

  GLuint FBO = 0;
  GLuint RenderTextureColor = 0;
  GLuint RenderTextureDS = 0;

  // Only for MS_BlitFromRenderbuffer
  GLuint FBOMultisample = 0;
  GLuint RenderbufferColor = 0;
  GLuint RenderbufferDS = 0;

  GLuint VAOScreenFillingTriangle = 0;
  GLuint VBOScreenFillingTriangle = 0;

  GLuint ShaderProgram = 0;
};

}  // namespace OpenGL
}  // namespace Impacto