#pragma once

#include <flat_hash_map.hpp>
#include "renderer/renderer.h"
#include "texture/texture.h"
#include "spritesheet.h"
#include "loadable.h"
#include "io/stream.h"
#include "profile/game.h"
#include <vector>
#include <memory>
#include <variant>

namespace Impacto {
struct Character2DState {
  struct LAYData {
    LAYData() = default;
    LAYData(int count)
        : ScreenCoords(std::make_unique<glm::vec2[]>(count)),
          TextureCoords(std::make_unique<glm::vec2[]>(count)) {}
    std::unique_ptr<glm::vec2[]> ScreenCoords;
    std::unique_ptr<glm::vec2[]> TextureCoords;
  };
  struct MVLData {
    MVLData() = default;
    MVLData(size_t count) : Indices(std::make_unique<uint16_t[]>(count)) {}
    std::unique_ptr<uint16_t[]> Indices;
  };

  int Count;
  std::variant<LAYData, MVLData> Data;

  Character2DState() = default;
  Character2DState(int count, bool isMVL)
      : Count(count), Data(ConstructData(count, isMVL)) {}

 private:
  static auto ConstructData(int count, bool isMVL)
      -> std::variant<LAYData, MVLData> {
    if (isMVL) {
      return std::variant<LAYData, MVLData>{std::in_place_type<MVLData>, count};
    } else {
      return std::variant<LAYData, MVLData>{std::in_place_type<LAYData>, count};
    }
  }
};

int constexpr MaxMvlIndices = 128 * 1024;

class Character2D : public Loadable<Character2D> {
  friend class Loadable<Character2D>;

 public:
  Sprite CharaSprite;
  float OffsetX;
  float OffsetY;
  float ScaleX = 1.0f;
  float ScaleY = 1.0f;
  float RotationX = 0.0f;
  float RotationY = 0.0f;
  float RotationZ = 0.0f;
  int Face;
  int LipFrame;
  int EyeFrame;
  bool Show;
  std::array<int, 2> Layers;
  glm::vec4 Tint = glm::vec4(1.0f);
  void Update(float dt);
  void Render(int layer);

  std::string MountPoint = "chara";

 protected:
  bool LoadSync(uint32_t charaId);
  void UnloadSync();
  void MainThreadOnLoad();

  bool OnLayer(int layer) {
    return std::find(Layers.begin(), Layers.end(), layer) != Layers.end();
  }

 private:
  Texture CharaTexture;
  SpriteSheet CharaSpriteSheet;

  ska::flat_hash_map<int, Character2DState> States;
  std::vector<int> StatesToDraw;

  int MvlVerticesCount;
  std::vector<float> MvlVertices;
  std::array<uint16_t, MaxMvlIndices> MvlIndices;
  int MvlIndicesCount;
};

int constexpr MaxCharacters2D = 16;

inline Character2D Characters2D[MaxCharacters2D];

int constexpr MaxSpeakerPortraits = 2;

inline Character2D SpeakerPortraits[MaxSpeakerPortraits];

}  // namespace Impacto