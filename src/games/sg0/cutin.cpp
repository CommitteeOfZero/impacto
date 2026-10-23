#include "cutin.h"

#include <algorithm>

#include "../../background2d.h"
#include "../../mem.h"
#include "../../util.h"
#include "../../profile/game.h"
#include "../../profile/scriptvars.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace SG0 {

using namespace Impacto::Profile::ScriptVars;

namespace {
constexpr int SlotStride = 20;
}  // namespace

void RenderCutins(const int layer) {
  if (SW_CUTIN1 < 0) return;
  const float scale = Profile::Game::DesignWidth / 1280.0f;

  for (size_t slot = 0; slot < Cutins.size(); slot++) {
    const CutinSlot& cutin = Cutins[slot];
    if (!cutin.Enabled || cutin.BufferMask <= 0) continue;
    const int base = SW_CUTIN1 + SlotStride * static_cast<int>(slot);
    if (ScrWork[base + 10] != layer) continue;
    const int alpha = std::clamp(ScrWork[base + 9], 0, 256);
    if (alpha == 0) continue;

    const auto bg =
        Backgrounds2D.find(ScrWork[SW_BG1SURF + GetBufferId(cutin.BufferMask)]);
    if (bg == Backgrounds2D.end() || bg->second == nullptr ||
        bg->second->Status != LoadStatus::Loaded) {
      continue;
    }

    Sprite region = bg->second->BgSprite;
    region.Bounds = RectF(ScrWork[base + 1] * scale, ScrWork[base + 2] * scale,
                          ScrWork[base + 3] * scale, ScrWork[base + 4] * scale);
    const RectF dest(ScrWork[base + 5] * scale, ScrWork[base + 6] * scale,
                     ScrWork[base + 7] * scale, ScrWork[base + 8] * scale);
    Renderer->DrawSprite(region, dest,
                         glm::vec4(1.0f, 1.0f, 1.0f, alpha / 256.0f));
  }
}

}  // namespace SG0
}  // namespace Impacto
