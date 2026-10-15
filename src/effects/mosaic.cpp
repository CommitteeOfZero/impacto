#include "mosaic.h"

#include "../profile/game.h"
#include "../renderer/renderer.h"

namespace Impacto {
namespace Effects {

void MosaicEffect::Render(float tileSize) {
  if (tileSize <= 1.0f) return;

  tileSize = std::min(tileSize, 20.0f);

  assert(CaptureTexture.IsValid());
  Renderer->CaptureScreenCap(CaptureTexture);
  Renderer->DrawMosaic(
      {*CaptureTexture, RectF{0.0f, 0.0f, Profile::Game::DesignWidth,
                              Profile::Game::DesignHeight}},
      {.TileSize = tileSize});
}

}  // namespace Effects
}  // namespace Impacto
