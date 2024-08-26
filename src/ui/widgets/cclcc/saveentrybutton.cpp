#include "saveentrybutton.h"

#include <iomanip>
#include <sstream>
#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/games/cclcc/savemenu.h"
#include "../../../vm/vm.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {
using namespace Impacto::Profile::CCLCC::SaveMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::SaveSystem;

glm::vec4 SaveEntryButton::FocusedAlpha = glm::vec4(1.0f);
Animation SaveEntryButton::FocusedAlphaFade;

SaveEntryButton::SaveEntryButton(int id, Sprite const& focusedBox,
                                 Sprite const& focusedText, int page,
                                 glm::vec2 pos, uint8_t locked,
                                 Sprite lockedSymbol,
                                 SaveSystem::SaveType saveType,
                                 Sprite NoDataSprite, Sprite BrokenDataSprite)
    : Widgets::Button(
          id,
          Sprite(SpriteSheet(), focusedBox.Bounds.X, focusedBox.Bounds.Y,
                 focusedBox.Bounds.Width, focusedBox.Bounds.Height),
          Sprite(SpriteSheet(), 0, 0, 0, 0), focusedBox, pos),
      FocusedSpriteLabel(focusedText, glm::vec2{pos.x, pos.y - 34}),
      Page(page),
      LockedSymbol(lockedSymbol,
                   glm::vec2(Bounds.X, Bounds.Y) + glm::vec2(205.0f, 79.0f)),
      Type(saveType),
      NoDataSymbol(NoDataSprite, glm::vec2(Bounds.X, Bounds.Y) +
                                     glm::vec2(211.0f, 20.0f + 1.0f - 12.0f)),
      BrokenDataSymbol(BrokenDataSprite,
                       glm::vec2(Bounds.X, Bounds.Y) +
                           glm::vec2(211.0f, 20.0f + 1.0f - 12.0f)) {
  DisabledSprite = NormalSprite;
  IsLocked = locked == 1;
}

void SaveEntryButton::Render() {
  NormalSpriteLabel.Render();
  if (HasFocus) {
    FocusedSpriteLabel.Tint = FocusedAlpha;
    FocusedSpriteLabel.Render();
    Renderer->DrawSprite(
        HighlightSprite,
        glm::vec2(Bounds.X + HighlightOffset.x, Bounds.Y + HighlightOffset.y),
        Tint, glm::vec2(Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f));
  }

  Renderer->DrawSprite(
      NumberDigitSprite[ScrWork[SW_SAVEMENUMODE]][(Id + 1) / 10],
      glm::vec2(Bounds.X + 20 + 668 + 32 + 1, Bounds.Y + 20 + 99 + 1), Tint,
      glm::vec2(Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f));
  Renderer->DrawSprite(
      NumberDigitSprite[ScrWork[SW_SAVEMENUMODE]][(Id + 1) % 10],
      glm::vec2(Bounds.X + 20 + 668 + 64 + 1, Bounds.Y + 20 + 99 + 1), Tint,
      glm::vec2(Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f));
  ThumbnailLabel.Render();
  uint8_t saveStatus = SaveSystem::GetSaveStatus(Type, Id);
  if (saveStatus == 1) {
    if (IsLocked) {
      LockedSymbol.Render();
    }
    RenderSceneTitleText();
    RenderSaveDateText();
  } else if (saveStatus == 0) {
    NoDataSymbol.Render();
  } else {
    BrokenDataSymbol.Render();
  }
}

int SaveEntryButton::GetPage() const { return Page; }

void SaveEntryButton::RenderSceneTitleText() {
  // TODO actually make this look correct
  uint8_t* strAddr =
      Vm::ScriptGetTextTableStrAddress(1, SaveSystem::GetSaveTitle(Type, Id));
  float fontSize = 24;
  RendererOutlineMode outlineMode = RendererOutlineMode::RO_BottomRight;
  glm::vec2 relativeTitlePosition = {20, 20};
  Label(strAddr, glm::vec2(Bounds.X, Bounds.Y) + relativeTitlePosition,
        fontSize, outlineMode, IsLocked ? 69 : 0)
      .Render();
}

void SaveEntryButton::RenderSaveDateText() {
  tm date = SaveSystem::GetSaveDate(Type, Id);
  std::stringstream dateStr;
  float fontSize = 32;
  RendererOutlineMode outlineMode = RendererOutlineMode::RO_Full;
  glm::vec2 relativePosition{20 + 290 * 1280.0 / 1920,
                             (71 + 120) * 1280.0 / 1920};
  dateStr << std::put_time(&date, "%Y/%m/%d %H:%M:%S");

  Label(dateStr.str(), glm::vec2(Bounds.X, Bounds.Y) + relativePosition,
        fontSize, outlineMode, IsLocked ? 69 : 0)
      .Render();
}

void SaveEntryButton::AddThumbnail(Sprite thumbnail, glm::vec2 pos) {
  ThumbnailLabel = Label(thumbnail, pos);
}

void SaveEntryButton::Move(glm::vec2 relativePosition) {
  Button::Move(relativePosition);
  NormalSpriteLabel.Move(relativePosition);
  FocusedSpriteLabel.Move(relativePosition);
  LockedSymbol.Move(relativePosition);
  ThumbnailLabel.Move(relativePosition);
}

void SaveEntryButton::FocusedAlphaFadeStart() {
  if (FocusedAlphaFade.State == AS_Stopped) {
    FocusedAlphaFade.Direction = 1;
    FocusedAlphaFade.DurationIn = 0.5f;
    FocusedAlphaFade.DurationOut = 0.5f;
    FocusedAlphaFade.LoopMode = ALM_ReverseDirection;
    FocusedAlphaFade.StartIn();
  }
}

void SaveEntryButton::FocusedAlphaFadeReset() {
  FocusedAlphaFade.State = AS_Stopped;
  FocusedAlphaFade.Progress = 0.0f;
}

void SaveEntryButton::UpdateFocusedAlphaFade(float dt) {
  FocusedAlphaFade.Update(dt);
  FocusedAlpha =
      glm::vec4(glm::vec3(1.0f), ((FocusedAlphaFade.Progress * 30) + 1) / 85);
}

}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
