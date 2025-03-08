#pragma once

#include <flat_hash_map.hpp>
#include "texture/texture.h"
#include "spritesheet.h"
#include "loadable.h"
#include "renderer/renderer.h"

namespace Impacto {

enum LinkDirection {
  LD_Off,
  LD_Up,
  LD_Down,
  LD_Left,
  LD_Right,
  LD_Up3,
  LD_Down3
};

int constexpr MaxLinks = 2;

class Background2D;

struct LinkState {
  Background2D* LinkedBuffer;
  LinkDirection Direction;
  glm::vec2 DisplayCoords;
};

class Background2D : public Loadable<Background2D> {
  friend class Loadable<Background2D>;

 public:
  static void Init();

  Sprite BgSprite;

  glm::vec2 DisplayCoords;
  int MaskNumber;

  int FadeCount;
  int FadeRange;

  bool Show;
  std::array<int, 2> Layers;
  LinkState Links[MaxLinks];

  void Render(int bgId, int layer);
  void RenderCapture(int capId, int layer);
  void RenderBgEff(int bgId, int layer);

  void LoadSolidColor(uint32_t color, int width, int height);

 protected:
  bool LoadSync(uint32_t bgId);
  void UnloadSync();
  void MainThreadOnLoad();

  bool OnLayer(int layer) {
    return std::find(Layers.begin(), Layers.end(), layer) != Layers.end();
  }

 private:
  Texture BgTexture;

  using BackgroundRenderProc = auto (Background2D::*)(glm::vec4 col) -> void;

  void RenderRegular(glm::vec4 col);
  void RenderMasked(glm::vec4 col);
  void RenderMaskedInverted(glm::vec4 col);
  void RenderFade(glm::vec4 col);

  BackgroundRenderProc constexpr static BackgroundRenderTable[30] = {
      &Background2D::RenderRegular,         // 0
      &Background2D::RenderFade,            // 1
      &Background2D::RenderRegular,         // 2
      &Background2D::RenderRegular,         // 3
      &Background2D::RenderRegular,         // 4
      &Background2D::RenderRegular,         // 5
      &Background2D::RenderRegular,         // 6
      &Background2D::RenderRegular,         // 7
      &Background2D::RenderRegular,         // 8
      &Background2D::RenderRegular,         // 9
      &Background2D::RenderRegular,         // 10
      &Background2D::RenderRegular,         // 11
      &Background2D::RenderRegular,         // 12
      &Background2D::RenderRegular,         // 13
      &Background2D::RenderRegular,         // 14
      &Background2D::RenderMasked,          // 15
      &Background2D::RenderMaskedInverted,  // 16
      &Background2D::RenderRegular,         // 17
      &Background2D::RenderRegular,         // 18
      &Background2D::RenderRegular,         // 19
      &Background2D::RenderRegular,         // 20
      &Background2D::RenderRegular,         // 21
      &Background2D::RenderRegular,         // 22
      &Background2D::RenderRegular,         // 23
      &Background2D::RenderRegular,         // 24
      &Background2D::RenderRegular,         // 25
      &Background2D::RenderRegular,         // 26
      &Background2D::RenderRegular,         // 27
      &Background2D::RenderRegular,         // 28
      &Background2D::RenderRegular,         // 29
  };
};

int constexpr MaxBackgrounds2D = 8;
int constexpr MaxScreencaptures = 2;

inline std::array<Background2D, MaxBackgrounds2D> Backgrounds;
inline std::array<Background2D, MaxScreencaptures> Screencaptures;
inline std::array<Background2D, MaxFramebuffers> Framebuffers;
inline Background2D ShaderScreencapture;

inline ska::flat_hash_map<int, Background2D*> Backgrounds2D;

}  // namespace Impacto