#pragma once

#include "../../animation.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace RNE {
class TileBackground : public Animation {
 public:
  void Init();
  void Render() override;

  uint32_t Seed;

  int Columns;
  int Rows;

  float VanishingPointX;
  float CenterY;
  float Depth;

  float MaxAngle;

  Sprite BackgroundSprite;

 private:
  float ColumnWidth;
  float RowHeight;
};
}  // namespace RNE
}  // namespace Impacto