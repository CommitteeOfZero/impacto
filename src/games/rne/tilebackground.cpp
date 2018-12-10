#include "tilebackground.h"
#include "../../renderer2d.h"
#include "../../rng.h"
#include "../../profile/game.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace RNE {

void TileBackground::Init() {
  ColumnWidth = Profile::DesignWidth / (float)Columns;
  RowHeight = Profile::DesignHeight / (float)Rows;
}

void TileBackground::Render() {
  if (Progress == 0) return;
  if (Progress == 1) {
    Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0, 0));
    return;
  }

  Sprite tileSprite = BackgroundSprite;
  tileSprite.Bounds.Width = (BackgroundSprite.Bounds.Width / (float)Columns);
  tileSprite.Bounds.Height = (BackgroundSprite.Bounds.Height / (float)Rows);

  for (int x = 0; x < Columns; x++) {
    for (int y = 0; y < Rows; y++) {
      int i = y * Columns + x;

      float angle = MaxAngle;
      // the first five rows rotate up, the last two rotate down
      // this is how the original behaves too
      if ((float)y / (float)Rows >= CenterY) {
        angle = -angle;
      }

      RNG rng(i, Seed);
      // FloatBetween(x, y) is in seconds here, we divide by DurationIn to get
      // the corresponding progress value for those seconds in the in direction
      float tileDuration = rng.FloatBetween(0.22f, 0.28f) / DurationIn;
      float tileStartProgress;
      uint32_t startTimeLevel = rng.UintBetween(0, 2);
      if (startTimeLevel == 0) {
        tileStartProgress = rng.FloatBetween(0.0f, 0.20f) / DurationIn;
      } else {
        tileStartProgress = rng.FloatBetween(0.25f, 0.45f) / DurationIn;
      }
      float tileEndProgress = tileStartProgress + tileDuration;

      float smoothedTileProgress =
          glm::smoothstep(tileStartProgress, tileEndProgress, Progress);

      angle = angle - smoothedTileProgress * angle;
      glm::vec3 euler(angle, 0, 0);
      glm::quat quat;
      eulerZYXToQuat(&euler, &quat);

      // all rows go from right-angled to screen to parallel, hence the
      // different perspective per row
      glm::vec2 vanishingPoint(VanishingPointX * Profile::DesignWidth,
                               ((float)y + 0.5f) * RowHeight);

      tileSprite.Bounds.X = (float)x * tileSprite.Bounds.Width;
      tileSprite.Bounds.Y = (float)y * tileSprite.Bounds.Height;

      Renderer2D::DrawSprite3DRotated(
          tileSprite,
          RectF((float)x * ColumnWidth, (float)y * RowHeight, ColumnWidth,
                RowHeight),
          2.0f, vanishingPoint, false, quat,
          glm::vec4(1.0f, 1.0f, 1.0f, smoothedTileProgress));
    }
  }
}

}  // namespace RNE
}  // namespace Impacto