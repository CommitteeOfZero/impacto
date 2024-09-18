#include "saveentrybutton.h"

#include <iomanip>
#include <sstream>
#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/games/cclcc/savemenu.h"
#include "../../../vm/vm.h"
#include <fmt/format.h>
#include <fmt/chrono.h>

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
                                 glm::vec2 pos, Sprite lockedSymbol,
                                 SaveSystem::SaveType saveType,
                                 Sprite NoDataSprite, Sprite BrokenDataSprite)
    : Widgets::Button(
          (saveType == SaveSystem::SaveType::SaveFull
               ? id
               : SaveSystem::GetQuickSaveCount() - id - 1),
          Sprite(SpriteSheet(), focusedBox.Bounds.X, focusedBox.Bounds.Y,
                 focusedBox.Bounds.Width, focusedBox.Bounds.Height),
          Sprite(SpriteSheet(), 0, 0, 0, 0), focusedBox, pos),
      Index(id),
      Page(page),
      FocusedSpriteLabel(focusedText, glm::vec2{pos.x, pos.y - 34}),
      LockedSymbol(lockedSymbol,
                   glm::vec2(Bounds.X, Bounds.Y) + glm::vec2(205.0f, 79.0f)),
      Type(saveType),
      NoDataSymbol(NoDataSprite, glm::vec2(Bounds.X, Bounds.Y) +
                                     glm::vec2(211.0f, 20.0f + 1.0f - 12.0f)),
      BrokenDataSymbol(BrokenDataSprite,
                       glm::vec2(Bounds.X, Bounds.Y) +
                           glm::vec2(211.0f, 20.0f + 1.0f - 12.0f)) {
  DisabledSprite = NormalSprite;
  glm::vec2 relativeTitlePosition = {310, 39};
  CharacterRouteLabel.Bounds.X = Bounds.X + relativeTitlePosition.x;
  CharacterRouteLabel.Bounds.Y = Bounds.Y + relativeTitlePosition.y;

  SceneTitleLabel.Bounds.X = Bounds.X + relativeTitlePosition.x;
  SceneTitleLabel.Bounds.Y = Bounds.Y + relativeTitlePosition.y + 30;

  glm::vec2 relativeTimePosition{315, 147};
  SaveDateLabel.Bounds.X = Bounds.X + relativeTimePosition.x;
  SaveDateLabel.Bounds.Y = Bounds.Y + relativeTimePosition.y;

  Update(0);
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
      SeparationLineSprite[ScrWork[SW_SAVEMENUMODE]],
      glm::vec2(Bounds.X + 308, Bounds.Y + 112), Tint,
      glm::vec2(Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f));
  Renderer->DrawSprite(
      NumberDigitSprite[ScrWork[SW_SAVEMENUMODE]][(Index + 1) / 10],
      glm::vec2(Bounds.X + 720, Bounds.Y + 120), Tint,
      glm::vec2(Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f));
  Renderer->DrawSprite(
      NumberDigitSprite[ScrWork[SW_SAVEMENUMODE]][(Index + 1) % 10],
      glm::vec2(Bounds.X + 752, Bounds.Y + 120), Tint,
      glm::vec2(Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f));
  if (SaveStatus == 1) {
    if (IsLocked) {
      SceneTitleLabel.Render();
      LockedSymbol.Render();
    }
    Renderer->DrawSprite(
        Thumbnail,
        {Bounds.X + 20, Bounds.Y + 22, Thumbnail.Bounds.Width + 30,
         Thumbnail.Bounds.Height + 17},
        Tint);
    CharacterRouteLabel.Render();
    SceneTitleLabel.Render();
    SaveDateLabel.Render();
  } else if (SaveStatus == 0) {
    NoDataSymbol.Render();
  } else {
    BrokenDataSymbol.Render();
  }
}

int SaveEntryButton::GetPage() const { return Page; }

void SaveEntryButton::Move(glm::vec2 relativePosition) {
  Button::Move(relativePosition);
  NormalSpriteLabel.Move(relativePosition);
  FocusedSpriteLabel.Move(relativePosition);
  LockedSymbol.Move(relativePosition);
  Thumbnail.Bounds.X += relativePosition.x;
  Thumbnail.Bounds.Y += relativePosition.y;
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

void SaveEntryButton::RefreshCharacterRouteText(int strIndex) {
  // TODO actually make this look correct
  uint8_t* strAddr = Vm::ScriptGetTextTableStrAddress(1, strIndex);
  float fontSize = 28;
  RendererOutlineMode outlineMode = RendererOutlineMode::RO_BottomRight;
  CharacterRouteLabel.SetText(strAddr, fontSize, outlineMode,
                              IsLocked ? 69 : 70);
}

void SaveEntryButton::RefreshSceneTitleText(int strIndex) {
  // TODO actually make this look correct
  uint8_t* strAddr = Vm::ScriptGetTextTableStrAddress(1, strIndex | 1);
  float fontSize = 28;
  RendererOutlineMode outlineMode = RendererOutlineMode::RO_BottomRight;
  SceneTitleLabel.SetText(strAddr, fontSize, outlineMode, IsLocked ? 69 : 71);
}

void SaveEntryButton::RefreshSaveDateText() {
  tm const& date = SaveSystem::GetSaveDate(Type, Id);
  float fontSize = 32;
  RendererOutlineMode outlineMode = RendererOutlineMode::RO_BottomRight;
  // Maybe fmt will merge my PR for space padded month
  SaveDateLabel.SetText(fmt::format(FMT_STRING("{:%Y/%m/%d %H:%M:%S}"), date),
                        fontSize, outlineMode, IsLocked ? 69 : 71);
}

// TODO: Make this only refresh when saved
void SaveEntryButton::Update(float dt) {
  SaveStatus = SaveSystem::GetSaveStatus(Type, Id);
  IsLocked = SaveSystem::GetSaveFlags(Type, Id) == 1;
  if (SaveStatus == 1) {
    auto strIndex = (SaveSystem::GetSaveTitle(Type, Id) * 2);
    if (strIndex > 40) {
      strIndex = 0;
    }
    RefreshCharacterRouteText(strIndex);
    RefreshSceneTitleText(strIndex);
    RefreshSaveDateText();
    Thumbnail = SaveSystem::GetSaveThumbnail(Type, Id);
  }
}
}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
