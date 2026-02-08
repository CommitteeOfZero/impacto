#include "nametagdisplay.h"

#include "../profile/dialogue.h"
#include "chlcc/nametagdisplay.h"
#include "cc/nametagdisplay.h"
#include "../log.h"

#include <numeric>

namespace Impacto {

using namespace Impacto::Profile::Dialogue;

std::unique_ptr<NametagDisplay> NametagDisplay::Create() {
  switch (NametagCurrentType) {
    default:
      ImpLog(LogLevel::Warning, LogChannel::General,
             "Attempted to create unexpected nametag type {:d}; defaulting to "
             "Void",
             static_cast<int>(NametagCurrentType));
      [[fallthrough]];
    case NametagType::None:
      return std::make_unique<VoidNametagDisplay>();

    case NametagType::Sprite:
      return std::make_unique<SpriteNametagDisplay>();

    case NametagType::TwoPiece:
    case NametagType::ThreePiece:
      return std::make_unique<ThreePieceNametagDisplay>();

    case NametagType::CHLCC:
      return std::make_unique<CHLCC::NametagDisplay>();

    case NametagType::CC:
      return std::make_unique<CC::NametagDisplay>();
  }
}

void VoidNametagDisplay::Render(const NameInfo& nameInfo,
                                const glm::vec4 tint) {
  if (Hidden || !nameInfo.RenderWindow) return;

  Renderer->DrawProcessedText(nameInfo.Name, DialogueFont, tint.a,
                              RendererOutlineMode::Full);
}

void SpriteNametagDisplay::Render(const NameInfo& nameInfo,
                                  const glm::vec4 tint) {
  if (Hidden || !nameInfo.RenderWindow) return;

  Renderer->DrawSprite(NametagSprite, NametagPosition, tint);

  Renderer->DrawProcessedText(nameInfo.Name, DialogueFont, tint.a,
                              RendererOutlineMode::Full);
}

void ThreePieceNametagDisplay::Render(const NameInfo& nameInfo,
                                      const glm::vec4 tint) {
  if (Hidden || !nameInfo.RenderWindow) return;

  Renderer->DrawSprite(NametagLeftSprite, NametagPosition, tint);

  const RectF nameBounds = std::accumulate(
      nameInfo.Name.begin() + 1, nameInfo.Name.end(), nameInfo.Name[0].DestRect,
      [](const RectF rect, const ProcessedTextGlyph& glyph) {
        return RectF::Coalesce(rect, glyph.DestRect);
      });
  float rightX =
      NametagPosition.x + NametagLeftSprite.ScaledWidth() + nameBounds.Width;

  if (NametagCurrentType == +NametagType::ThreePiece) {
    rightX -= NametagMiddleBaseWidth;

    // Draw middle sprites
    const float middleBegin =
        NametagPosition.x + NametagLeftSprite.ScaledWidth();

    Sprite middleSprite = NametagMiddleSprite;
    for (float x = middleBegin; x < rightX;
         x += NametagMiddleSprite.ScaledWidth()) {
      middleSprite.SetScaledWidth(
          std::min(rightX - x, NametagMiddleSprite.ScaledWidth()));
      Renderer->DrawSprite(middleSprite, {x, NametagPosition.y}, tint);
    }
  }

  Renderer->DrawSprite(NametagRightSprite, {rightX, NametagPosition.y}, tint);

  Renderer->DrawProcessedText(nameInfo.Name, DialogueFont, tint.a,
                              RendererOutlineMode::Full);
}

}  // namespace Impacto
