#pragma once

#include "renderer2d.h"
#include "texture/texture.h"
#include "spritesheet.h"
#include "loadable.h"

#define BackgroundRenderer(name) \
  void name(Background2D* bg, int bgId, glm::vec4 col)

namespace Impacto {

class Background2D : public Loadable<Background2D> {
  friend class Loadable<Background2D>;

 public:
  Sprite BgSprite;
  glm::vec2 DisplayCoords;
  bool Show;
  int Layer;
  void Render(int bgId, int layer);

 protected:
  bool LoadSync(uint32_t bgId);
  void UnloadSync();
  void MainThreadOnLoad();

 private:
  Texture BgTexture;
  SpriteSheet BgSpriteSheet;
};

typedef void (*BackgroundRenderProc)(Background2D* bg, int bgId, glm::vec4 col);

int const MaxBackgrounds2D = 8;

extern Background2D Backgrounds2D[MaxBackgrounds2D];

BackgroundRenderer(RenderRegular);
BackgroundRenderer(RenderMasked);
BackgroundRenderer(RenderFade);

BackgroundRenderProc static BackgroundRenderTable[30] = {
    RenderRegular,  // 0
    RenderFade,     // 1
    RenderRegular,  // 2
    RenderRegular,  // 3
    RenderRegular,  // 4
    RenderRegular,  // 5
    RenderRegular,  // 6
    RenderRegular,  // 7
    RenderRegular,  // 8
    RenderRegular,  // 9
    RenderRegular,  // 10
    RenderRegular,  // 11
    RenderRegular,  // 12
    RenderRegular,  // 13
    RenderRegular,  // 14
    RenderMasked,   // 15
    RenderMasked,   // 16
    RenderRegular,  // 17
    RenderRegular,  // 18
    RenderRegular,  // 19
    RenderRegular,  // 20
    RenderRegular,  // 21
    RenderRegular,  // 22
    RenderRegular,  // 23
    RenderRegular,  // 24
    RenderRegular,  // 25
    RenderRegular,  // 26
    RenderRegular,  // 27
    RenderRegular,  // 28
    RenderRegular,  // 29
};

}  // namespace Impacto