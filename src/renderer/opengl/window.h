#pragma once

#include <SDL.h>

#include "../windowtemplate.h"
#include "../../util.h"

namespace Impacto {
class WindowInterface;
namespace OpenGL {
enum GraphicsApi {
  GfxApi_GL,
  // Forces the use of a GLES driver (e.g. ANGLE on Windows)
  GfxApi_ForceNativeGLES,
  // Forces GLES context on desktop GL driver
  GfxApi_ForceDesktopGLES
};
class GLWindow : public WindowTemplate<GLWindow> {
  friend class Impacto::WindowInterface;

 public:
  void Init();
  void SetDimensions(int width, int height, int msaa, float renderScale);
  RectF GetViewport();
  RectF GetScaledViewport();
  void SwapRTs();
  void Update();
  void Draw();
  void Shutdown();

  SDL_GLContext GLContext;

  // FBO of current render target for drawing onto
  GLuint DrawRT;
  // FBO of current render target for reading from
  GLuint ReadRT;
  // Texture associated with ReadRT
  GLuint ReadRenderTexture;

  // This should probably be a compile-time define
  constexpr static GraphicsApi GraphicsApiHint = GraphicsApi::GfxApi_GL;
  GraphicsApi ActualGraphicsApi;

 private:
  void UpdateDimensions();
  void TryCreateGL(GraphicsApi api);
  void CleanFBOs();

  bool GLDebug = false;
  GLuint drawRenderTexture = 0;
};

}  // namespace OpenGL
}  // namespace Impacto