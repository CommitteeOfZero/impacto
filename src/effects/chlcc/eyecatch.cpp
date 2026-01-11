#include "eyecatch.h"

#include "../../profile/ui/gamespecific.h"
#include "../../renderer/renderer.h"
#include "../../spritesheet.h"
#include "../../profile/game.h"
#include "../../background2d.h"
#include "../../util.h"

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::GameSpecific;

namespace Impacto {
namespace UI {
namespace CHLCC {

EyecatchEffect::EyecatchEffect() {
  Texture textureStarsMask{};
  textureStarsMask.LoadSolidColor(static_cast<int>(Profile::DesignWidth),
                                  static_cast<int>(Profile::DesignHeight), 0);
  SpriteSheet sheetStarsMask(Profile::DesignWidth, Profile::DesignHeight);
  sheetStarsMask.Texture = textureStarsMask.Submit();
  sheetStarsMask.IsScreenCap = true;
  StarsMask =
      Sprite(sheetStarsMask, 0, 0, Profile::DesignWidth, Profile::DesignHeight);
}

void EyecatchEffect::RenderMain() {
  if (!ScrWork[SW_EYECATCH_COUNT]) return;
  Renderer->Clear(glm::vec4(0.0f));
  const auto scaleRatio =
      glm::vec2{Profile::DesignWidth / 1280.0f, Profile::DesignHeight / 720.0f};
  for (int i = 0; i < 4; ++i) {
    int scaleOffset = 2 * i;
    for (int j = 0; j < 7; ++j) {
      if (scaleOffset + 1 <= ScrWork[SW_EYECATCH_COUNT]) {
        const int scale =
            std::min((ScrWork[SW_EYECATCH_COUNT] - (scaleOffset + 1)) * 18,
                     400) *
            280 / 106;
        const float y = scaleRatio.y * (20.0f + 200 * i);
        const float x = scaleRatio.x * (20.0f + 200 * j);
        RectF dest{x, y, EyecatchStar.ScaledWidth(),
                   EyecatchStar.ScaledHeight()};
        dest.ScaleAroundCenter(glm::vec2{scale / 256.0f});
        Renderer->DrawSprite(EyecatchStar, dest, glm::vec4(1.0f),
                             glm::vec3{1.0f});
      }
      scaleOffset += 2;
    }
  }
  Renderer->CaptureScreencap(StarsMask);
}

void EyecatchEffect::RenderLayer(int layer) {
  if (ScrWork[SW_EYECATCH_BUF] != 0 && ScrWork[SW_EYECATCH_COUNT] != 0 &&
      layer == ScrWork[SW_EYECATCH_PRI]) {
    const int childBufId = GetBufferId(ScrWork[SW_EYECATCH_BUF]);
    if (ScrWork[SW_EYECATCH_COUNT] < 64) {
      Renderer->Clear(glm::vec4(0.0f));
      Renderer->DrawMaskedSprite(
          Backgrounds2D[ScrWork[SW_BG1SURF + childBufId]]->BgSprite, StarsMask,
          255, 256, glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 0.0f},
          glm::vec4(1.0f), false, false);

    } else {
      Renderer->DrawSprite(
          Backgrounds2D[ScrWork[SW_BG1SURF + childBufId]]->BgSprite,
          RectF{0, 0, Profile::DesignWidth, Profile::DesignHeight},
          glm::vec4{1.0f});
    }
  }
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto