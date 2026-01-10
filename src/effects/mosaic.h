#pragma once

#include "../spritesheet.h"

namespace Impacto {
namespace Effects {

class MosaicEffect {
 public:
  MosaicEffect();
  ~MosaicEffect();

  void Init();

  void Render(float tileSize);

 private:
  Sprite CaptureSprite;
};

inline MosaicEffect Mosaic;

}  // namespace Effects
}  // namespace Impacto