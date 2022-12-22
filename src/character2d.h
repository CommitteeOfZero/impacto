#pragma once

#include <flat_hash_map.hpp>
#include "renderer/renderer.h"
#include "texture/texture.h"
#include "spritesheet.h"
#include "loadable.h"
#include "io/inputstream.h"
#include "profile/game.h"
#include <vector>

namespace Impacto {

struct Character2DState {
  int Count;
  // LAY
  glm::vec2* ScreenCoords;
  glm::vec2* TextureCoords;
  // MVL
  uint16_t* Indices;
};

int const MaxMvlIndices = 128 * 1024;

class Character2D : public Loadable<Character2D> {
  friend class Loadable<Character2D>;

 public:
  Sprite CharaSprite;
  int OffsetX;
  int OffsetY;
  int Face;
  int LipFrame;
  int EyeFrame;
  bool Show;
  int Layer;
  void Update(float dt);
  void Render(int chaId, int layer);

  std::string MountPoint = "chara";

 protected:
  bool LoadSync(uint32_t charaId);
  void UnloadSync();
  void MainThreadOnLoad();

 private:
  Texture CharaTexture;
  SpriteSheet CharaSpriteSheet;

  ska::flat_hash_map<int, Character2DState> States;
  std::vector<int> StatesToDraw;

  float* MvlVertices;
  uint16_t MvlIndices[MaxMvlIndices];
  int MvlVerticesCount;
  int MvlIndicesCount;
};

int const MaxCharacters2D = 16;

extern Character2D Characters2D[MaxCharacters2D];

}  // namespace Impacto