#pragma once

#include <flat_hash_map.hpp>
#include "renderer2d.h"
#include "texture/texture.h"
#include "spritesheet.h"
#include "loadable.h"
#include "io/inputstream.h"
#include "profile/game.h"
#include <vector>

namespace Impacto {

struct Character2DState {
  int Count;
  glm::vec2* ScreenCoords;
  glm::vec2* TextureCoords;
};

class Character2D : public Loadable<Character2D> {
  friend class Loadable<Character2D>;

 public:
  Sprite CharaSprite;
  int CharaOffsetX;
  int CharaOffsetY;
  bool Show;
  int Layer;
  void Render();

 protected:
  bool LoadSync(uint32_t charaId);
  void UnloadSync();
  void MainThreadOnLoad();

 private:
  Texture CharaTexture;
  SpriteSheet CharaSpriteSheet;
  std::vector<Character2DState> StatesToDraw;
};

int const MaxCharacters2D = 8;

extern Character2D Characters2D[MaxCharacters2D];

}  // namespace Impacto