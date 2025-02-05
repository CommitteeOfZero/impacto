#pragma once

#include <flat_hash_map.hpp>
#include "texture/texture.h"
#include "spritesheet.h"
#include "loadable.h"

#define BackgroundRenderer(name) void name(Background2D* bg, glm::vec4 col)

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
  int Layer;
  LinkState Links[MaxLinks];

  void Render(int bgId, int layer);
  void RenderCapture(int capId, int layer);
  void LoadSolidColor(uint32_t color, int width, int height);

 protected:
  bool LoadSync(uint32_t bgId);
  void UnloadSync();
  void MainThreadOnLoad();

 private:
  Texture BgTexture;
};

typedef void (*BackgroundRenderProc)(Background2D* bg, glm::vec4 col);

int constexpr MaxBackgrounds2D = 8;
int constexpr MaxScreencaptures = 2;

inline Background2D Backgrounds[MaxBackgrounds2D];
inline Background2D Screencaptures[MaxScreencaptures];
inline Background2D ShaderScreencapture;

inline ska::flat_hash_map<int, Background2D*> Backgrounds2D;

BackgroundRenderer(RenderRegular);
BackgroundRenderer(RenderMasked);
BackgroundRenderer(RenderMaskedInverted);
BackgroundRenderer(RenderFade);

BackgroundRenderProc static BackgroundRenderTable[30] = {
    RenderRegular,         // 0
    RenderFade,            // 1
    RenderRegular,         // 2
    RenderRegular,         // 3
    RenderRegular,         // 4
    RenderRegular,         // 5
    RenderRegular,         // 6
    RenderRegular,         // 7
    RenderRegular,         // 8
    RenderRegular,         // 9
    RenderRegular,         // 10
    RenderRegular,         // 11
    RenderRegular,         // 12
    RenderRegular,         // 13
    RenderRegular,         // 14
    RenderMasked,          // 15
    RenderMaskedInverted,  // 16
    RenderRegular,         // 17
    RenderRegular,         // 18
    RenderRegular,         // 19
    RenderRegular,         // 20
    RenderRegular,         // 21
    RenderRegular,         // 22
    RenderRegular,         // 23
    RenderRegular,         // 24
    RenderRegular,         // 25
    RenderRegular,         // 26
    RenderRegular,         // 27
    RenderRegular,         // 28
    RenderRegular,         // 29
};

}  // namespace Impacto