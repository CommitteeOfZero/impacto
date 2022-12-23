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
  void AdjustEventCoordinatesForNk(SDL_Event* ev) override;
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

 private:
  void UpdateDimensions() override;
  void TryCreateGL(GraphicsApi api);
  void CleanFBOs();

  bool GLDebug = false;
  GLuint drawRenderTexture = 0;

  int lastWidth = -1;
  int lastHeight = -1;
  int lastMsaa = 0;
  float lastRenderScale = 1.0f;
};

} // OpenGL
} // Impacto