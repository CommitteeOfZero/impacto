#pragma once

#include "renderer2d.h"
#include "texture/texture.h"
#include "spritesheet.h"
#include "loadable.h"

namespace Impacto {

class Background2D : public Loadable<Background2D> {
  friend class Loadable<Background2D>;

 public:
  Sprite BgSprite;
  glm::vec2 DisplayCoords;
  bool Show;
  int Layer;

 protected:
  bool LoadSync(uint32_t bgId);
  void UnloadSync();
  void MainThreadOnLoad();

 private:
  Texture BgTexture;
  SpriteSheet BgSpriteSheet;
};

int const MaxBackgrounds2D = 8;

extern Background2D Backgrounds2D[MaxBackgrounds2D];

}  // namespace Impacto