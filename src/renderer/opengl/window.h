#pragma once

#include "../window.h"

namespace Impacto {
namespace OpenGL {

class GLWindow : public BaseWindow {
 public:
  void Init() override;
  void SetDimensions(int width, int height, int msaa,
                     float renderScale) override;
  RectF GetViewport() override;
  RectF GetScaledViewport() override;
  void SwapRTs() override;
  void Update() override;
  void Draw() override;
  void Shutdown() override;

  SDL_GLContext GLContext;

  // FBO of current render target for drawing onto
  GLuint DrawRT;
  // FBO of current render target for reading from
  GLuint ReadRT;
  // Texture associated with ReadRT
  GLuint ReadRenderTexture;
  // Stencil buffer associated with both ReadRT and DrawRT
  GLuint StencilBuffer;

 private:
  void UpdateDimensions() override;
  void TryCreateGL(GraphicsApi api);
  void CleanFBOs();

  bool GLDebug = false;
  GLuint drawRenderTexture = 0;
};

}  // namespace OpenGL
}  // namespace Impacto