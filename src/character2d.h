#pragma once

#include <ankerl/unordered_dense.h>
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

constexpr size_t MaxCharacters2D = 16;

class Character2D : public Loadable<Character2D, bool, uint32_t> {
  friend Loadable<Character2D, bool, uint32_t>;

 public:
  std::string MountPoint = "chara";

  bool Show;
  std::array<int, 2> Layers;

  Sprite CharaSprite;
  int Face;
  int LipFrame;
  int EyeFrame;

  glm::vec2 Offset = {0.0f, 0.0f};
  glm::vec2 Scale = {1.0f, 1.0f};
  glm::quat Rotation = glm::quat();

  glm::vec4 Tint = glm::vec4(1.0f);

  void Update(float dt);
  virtual void UpdateState(size_t chaId);
  static void UpdateEyeMouth();

  void Render(int layer);

 protected:
  bool LoadSync(uint32_t charaId);
  void UnloadSync();
  void MainThreadOnLoad(bool result);

  bool OnLayer(int layer) {
    return std::find(Layers.begin(), Layers.end(), layer) != Layers.end();
  }

  static inline std::array<int, MaxCharacters2D> CurEyeFrame{};
  static inline std::array<int, 3> CurMouthIndex{19, 19, 19};
  constexpr static std::array<std::pair<int, int>, 20> AnimeTable = {
      std::pair{1, 10}, std::pair{2, 5}, std::pair{1, 10}, std::pair{2, 4},
      std::pair{1, 7},  std::pair{2, 6}, std::pair{1, 9},  std::pair{2, 8},
      std::pair{1, 15}, std::pair{0, 2}, std::pair{1, 9},  std::pair{2, 3},
      std::pair{1, 7},  std::pair{0, 2}, std::pair{1, 10}, std::pair{2, 5},
      std::pair{1, 7},  std::pair{2, 5}, std::pair{1, 7},  std::pair{2, 3},
  };

  Texture CharaTexture;
  SpriteSheet CharaSpriteSheet;

  ankerl::unordered_dense::map<int, Character2DState> States;
  std::vector<int> StatesToDraw;

  std::vector<VertexBufferSprites> MvlVertices;
  std::vector<uint16_t> MvlIndices;
};

class CharacterPortrait2D : public Character2D {
 public:
  void UpdateState(size_t chaId) override;
};

inline std::array<Character2D, MaxCharacters2D> Characters2D;
inline std::array<CharacterPortrait2D, 2> SpeakerPortraits;

}  // namespace Impacto