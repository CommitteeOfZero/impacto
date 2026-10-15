#pragma once

#include "../spritesheet.h"

namespace Impacto {
namespace Effects {

class MosaicEffect {
 public:
  void Render(float tileSize);

 private:
  MutableTextureRef CaptureTexture;
};

inline MosaicEffect Mosaic;

}  // namespace Effects
}  // namespace Impacto