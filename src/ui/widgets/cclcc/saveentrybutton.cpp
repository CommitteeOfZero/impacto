#include "saveentrybutton.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/games/cclcc/savemenu.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {
using namespace Impacto::Profile::CCLCC::SaveMenu;
using namespace Impacto::Profile::ScriptVars;

glm::vec4 SaveEntryButton::FocusedAlpha = glm::vec4(1.0f);
Animation SaveEntryButton::FocusedAlphaFade;

SaveEntryButton::SaveEntryButton(int id, Sprite const& focusedBox,
                                 Sprite const& focusedText, int page,
                                 glm::vec2 pos, uint8_t locked,
                                 Sprite lockedSymbol, int saveStatus,
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
      SaveStatus(saveStatus),
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

    Renderer->DrawSprite(
        NumberDigitSprite[ScrWork[SW_SAVEMENUMODE]][(Id + 1) / 10],
        glm::vec2(Bounds.X + 20 + 668 + 32 + 1, Bounds.Y + 20 + 99 + 1), Tint,
        glm::vec2(Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f));
    Renderer->DrawSprite(
        NumberDigitSprite[ScrWork[SW_SAVEMENUMODE]][(Id + 1) % 10],
        glm::vec2(Bounds.X + 20 + 668 + 64 + 1, Bounds.Y + 20 + 99 + 1), Tint,
        glm::vec2(Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f));
  }
  ThumbnailLabel.Render();
  EntryNumberHint.Render();
  EntryNumber.Render();
  if (SaveStatus == 1) {
    if (IsLocked) {
      LockedSymbol.Render();
    }
    SceneTitle.Render();
    PlayTimeHint.Render();
    PlayTime.Render();
    SaveDateHint.Render();
    SaveDate.Render();
  } else if (SaveStatus == 0) {
    NoDataSymbol.Render();
  } else {
    BrokenDataSymbol.Render();
  }
}

int SaveEntryButton::GetPage() const { return Page; }

void SaveEntryButton::AddNormalSpriteLabel(Sprite norm, glm::vec2 pos) {
  NormalSpriteLabel = Label(norm, pos);
}

void SaveEntryButton::AddEntryNumberHintText(uint8_t* str, float fontSize,
                                             RendererOutlineMode outlineMode,
                                             glm::vec2 relativePosition) {
  EntryNumberHint = Label(str, glm::vec2(Bounds.X, Bounds.Y) + relativePosition,
                          fontSize, outlineMode, IsLocked ? 69 : 0);
}

void SaveEntryButton::AddEntryNumberText(std::string str, float fontSize,
                                         RendererOutlineMode outlineMode,
                                         glm::vec2 relativePosition) {
  EntryNumber = Label(str, glm::vec2(Bounds.X, Bounds.Y) + relativePosition,
                      fontSize, outlineMode, IsLocked ? 69 : 0);
}

void SaveEntryButton::AddSceneTitleText(uint8_t* str, float fontSize,
                                        RendererOutlineMode outlineMode,
                                        glm::vec2 relativeTitlePosition,
                                        glm::vec2 relativeNoDataPosition) {
  if (SaveStatus == 1) {
    SceneTitle =
        Label(str, glm::vec2(Bounds.X, Bounds.Y) + relativeTitlePosition,
              fontSize, outlineMode, IsLocked ? 69 : 0);
  }
}

void SaveEntryButton::AddPlayTimeHintText(uint8_t* str, float fontSize,
                                          RendererOutlineMode outlineMode,
                                          glm::vec2 relativePosition) {
  PlayTimeHint = Label(str, glm::vec2(Bounds.X, Bounds.Y) + relativePosition,
                       fontSize, outlineMode, IsLocked ? 69 : 0);
}

void SaveEntryButton::AddPlayTimeText(std::string str, float fontSize,
                                      RendererOutlineMode outlineMode,
                                      glm::vec2 relativePosition) {
  // Spacing is currently set for the C;HLCC font, more or less
  PlayTime = Label(str, glm::vec2(Bounds.X, Bounds.Y) + relativePosition,
                   fontSize, outlineMode, IsLocked ? 69 : 0);
}

void SaveEntryButton::AddSaveDateHintText(uint8_t* str, float fontSize,
                                          RendererOutlineMode outlineMode,
                                          glm::vec2 relativePosition) {
  SaveDateHint = Label(str, glm::vec2(Bounds.X, Bounds.Y) + relativePosition,
                       fontSize, outlineMode, IsLocked ? 69 : 0);
}

void SaveEntryButton::AddSaveDateText(std::string str, float fontSize,
                                      RendererOutlineMode outlineMode,
                                      glm::vec2 relativePosition) {
  SaveDate = Label(str, glm::vec2(Bounds.X, Bounds.Y) + relativePosition,
                   fontSize, outlineMode, IsLocked ? 69 : 0);
}

void SaveEntryButton::AddThumbnail(Sprite thumbnail, glm::vec2 pos) {
  ThumbnailLabel = Label(thumbnail, pos);
}

void SaveEntryButton::Move(glm::vec2 relativePosition) {
  NormalSpriteLabel.Move(relativePosition);
  FocusedSpriteLabel.Move(relativePosition);
  LockedSymbol.Move(relativePosition);
  ThumbnailLabel.Move(relativePosition);
  EntryNumberHint.Move(relativePosition);
  EntryNumber.Move(relativePosition);
  SceneTitle.Move(relativePosition);
  PlayTimeHint.Move(relativePosition);
  PlayTime.Move(relativePosition);
  SaveDateHint.Move(relativePosition);
  SaveDate.Move(relativePosition);
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
