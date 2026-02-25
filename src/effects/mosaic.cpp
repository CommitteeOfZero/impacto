#include "mosaic.h"

#include "../profile/game.h"
#include "../renderer/renderer.h"

namespace Impacto {
namespace Effects {

MosaicEffect::MosaicEffect()
    : CaptureSprite(SpriteSheet(Profile::DesignWidth, Profile::DesignHeight),
                    0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight) {
  CaptureSprite.Sheet.IsScreenCap = true;
}

MosaicEffect::~MosaicEffect() {
  if (Renderer) Renderer->FreeTexture(CaptureSprite.Sheet.Texture);
}

void MosaicEffect::Init() {
  if (CaptureSprite.Sheet.Texture != 0) return;

  Texture texture{};
  texture.LoadSolidColor(static_cast<int>(Profile::DesignWidth),
                         static_cast<int>(Profile::DesignHeight), 0);
  CaptureSprite.Sheet.Texture = texture.Submit();
}

void MosaicEffect::Render(float tileSize) {
  if (tileSize <= 1.0f) return;

  tileSize = std::min(tileSize, 20.0f);

  assert(CaptureSprite.Sheet.Texture != 0);
  Renderer->CaptureScreencap(CaptureSprite);
  Renderer->DrawMosaic(
      CaptureSprite,
      RectF{0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight}, tileSize,
      glm::mat4(1.0f), glm::vec4(1.0f));
}

}  // namespace Effects
}  // namespace Impacto
