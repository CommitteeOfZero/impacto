#include "saveentrybutton.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/games/cclcc/savemenu.h"
#include "../../../vm/vm.h"
#include "../../../ui/ui.h"
#include "../../../profile/game.h"
#include <fmt/format.h>

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CCLCC {
using namespace Impacto::Profile::CCLCC::SaveMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::SaveSystem;

SaveEntryButton::SaveEntryButton(int id, int index, Sprite const& focusedBox,
                                 Sprite const& focusedText, int page,
                                 glm::vec2 pos, Sprite lockedSymbol,
                                 SaveSystem::SaveType saveType,
                                 Sprite noDataSprite, Sprite brokenDataSprite)
    : Widgets::Button(
          id,
          Sprite(SpriteSheet(), focusedBox.Bounds.X, focusedBox.Bounds.Y,
                 focusedBox.Bounds.Width, focusedBox.Bounds.Height),
          Sprite(SpriteSheet(), 0, 0, 0, 0), focusedBox, pos),
      Index(index),
      Page(page),
      Type(saveType),
      FocusedSpriteLabel(focusedText, glm::vec2{pos.x, pos.y - 34}),
      LockedSymbol(lockedSymbol,
                   glm::vec2(Bounds.X, Bounds.Y) + SlotLockedSpritePosition),
      NoDataSymbol(noDataSprite,
                   glm::vec2(Bounds.X, Bounds.Y) + NoDataSpritePosition),
      BrokenDataSymbol(brokenDataSprite,
                       glm::vec2(Bounds.X, Bounds.Y) +
                           glm::vec2(211.0f, 20.0f + 1.0f - 12.0f)) {
  glm::vec2 relativeThumbnailPosition = {20, 20};
  Thumbnail.Bounds.X = Bounds.X + relativeThumbnailPosition.x;
  Thumbnail.Bounds.Y = Bounds.Y + relativeThumbnailPosition.y;

  glm::vec2 relativeTitlePosition = {310, 39};
  CharacterRouteLabel.Bounds.X = Bounds.X + relativeTitlePosition.x;
  CharacterRouteLabel.Bounds.Y = Bounds.Y + relativeTitlePosition.y;

  SceneTitleLabel.Bounds.X = Bounds.X + relativeTitlePosition.x;
  SceneTitleLabel.Bounds.Y = Bounds.Y + relativeTitlePosition.y + 30;

  glm::vec2 relativeTimePosition{315, 147};
  SaveDateLabel.Bounds.X = Bounds.X + relativeTimePosition.x;
  SaveDateLabel.Bounds.Y = Bounds.Y + relativeTimePosition.y;

  SaveEntryButton::Update(0);
}

void SaveEntryButton::Render() {
  const glm::vec2 scale = {Bounds.Width / HighlightSprite.ScaledWidth(), 1.0f};

  NormalSpriteLabel.Tint = Tint;
  NormalSpriteLabel.Render();
  if (HasFocus) {
    FocusedSpriteLabel.Tint = Tint;
    FocusedSpriteLabel.Render();

    const RectF highlightDest =
        HighlightSprite.ScaledBounds()
            .Scale(scale, {0.0f, 0.0f})
            .Translate(Bounds.GetPos() + HighlightOffset);
    Renderer->DrawSprite(HighlightSprite, highlightDest, Tint);
  }

  const RectF numberDigitDest = NumberDigitSprite[0][0]
                                    .ScaledBounds()
                                    .Scale(scale, {0.0f, 0.0f})
                                    .Translate(Bounds.GetPos());
  Renderer->DrawSprite(
      NumberDigitSprite[UI::SaveMenuPtr->ActiveMenuType->_to_integral()]
                       [(Index + 1) / 10],
      RectF(numberDigitDest).Translate({720, 120}), Tint);
  Renderer->DrawSprite(
      NumberDigitSprite[UI::SaveMenuPtr->ActiveMenuType->_to_integral()]
                       [(Index + 1) % 10],
      RectF(numberDigitDest).Translate({752, 120}), Tint);

  if (SaveStatus == 1) {
    const RectF separationLineDest =
        SeparationLineSprite[0]
            .ScaledBounds()
            .Scale(scale, {0.0f, 0.0f})
            .Translate(Bounds.GetPos() + glm::vec2(308, 112));
    Renderer->DrawSprite(
        SeparationLineSprite[UI::SaveMenuPtr->ActiveMenuType->_to_integral()],
        separationLineDest, Tint);

    if (IsLocked) {
      LockedSymbol.Tint = Tint;
      LockedSymbol.Render();
    }

    Thumbnail.Bounds.Width = 270;
    Thumbnail.Bounds.Height = 152;
    Thumbnail.Tint = Tint;
    Thumbnail.Render();

    CharacterRouteLabel.Tint = Tint;
    CharacterRouteLabel.Render();
    SceneTitleLabel.Tint = Tint;
    SceneTitleLabel.Render();
    SaveDateLabel.Tint = Tint;
    SaveDateLabel.Render();

  } else if (SaveStatus == 0) {
    NoDataSymbol.Tint = Tint;
    NoDataSymbol.Render();

  } else {
    BrokenDataSymbol.Tint = Tint;
    BrokenDataSymbol.Render();
  }
}

int SaveEntryButton::GetPage() const { return Page; }

void SaveEntryButton::Move(glm::vec2 relativePosition) {
  Button::Move(relativePosition);
  NormalSpriteLabel.Move(relativePosition);
  FocusedSpriteLabel.Move(relativePosition);
  LockedSymbol.Move(relativePosition);
  CharacterRouteLabel.Move(relativePosition);
  SceneTitleLabel.Move(relativePosition);
  SaveDateLabel.Move(relativePosition);
  Thumbnail.Move(relativePosition);
  NoDataSymbol.Move(relativePosition);
  BrokenDataSymbol.Move(relativePosition);
}

void SaveEntryButton::RefreshCharacterRouteText(int strIndex) {
  auto strAddr = Vm::ScriptGetTextTableStrAddress(1, strIndex);
  float fontSize = 28;
  RendererOutlineMode outlineMode = RendererOutlineMode::Full;
  DialogueColorPair colorPair =
      UI::SaveMenuPtr->ActiveMenuType->_to_integral() == SaveMenuPageType::Save
          ? DialogueColorPair{SaveEntryPrimaryColor, SaveEntryPrimaryColor}
          : DialogueColorPair{LoadEntryPrimaryColor, LoadEntryPrimaryColor};
  CharacterRouteLabel.SetText(strAddr, fontSize, outlineMode, colorPair);
}

void SaveEntryButton::RefreshSceneTitleText(int strIndex) {
  auto strAddr = Vm::ScriptGetTextTableStrAddress(1, strIndex + 1);
  float fontSize = 28;
  RendererOutlineMode outlineMode = RendererOutlineMode::Full;
  SceneTitleLabel.SetText(strAddr, fontSize, outlineMode,
                          {SaveEntrySecondaryColor, SaveEntrySecondaryColor});
}

void SaveEntryButton::RefreshSaveDateText() {
  tm const& date = SaveSystem::GetSaveDate(Type, Id);
  float fontSize = 32;
  RendererOutlineMode outlineMode = RendererOutlineMode::Full;
  // Maybe fmt will merge my PR for space padded month
  SaveDateLabel.SetText(
      fmt::format(fmt::runtime(Profile::DateFormat.FormattedString()), date) +
          fmt::format(" {:%H:%M:%S}", date),
      fontSize, outlineMode,
      {SaveEntrySecondaryColor, SaveEntrySecondaryColor});
}

void SaveEntryButton::ToggleLock() {
  const uint8_t newLock = SaveSystem::GetSaveFlags(Type, Id) ^ WriteProtect;
  SaveSystem::SetSaveFlags(Type, Id, newLock);
  IsLocked = newLock & WriteProtect;
}

void SaveEntryButton::RefreshInfo() {
  SaveStatus = SaveSystem::GetSaveStatus(Type, Id);
  IsLocked = SaveSystem::GetSaveFlags(Type, Id) & WriteProtect;

  if (SaveStatus == 1) {
    auto strIndex = SaveSystem::GetSaveTitle(Type, Id);
    if (strIndex > 21) {
      strIndex = 0;
    }
    RefreshCharacterRouteText(strIndex * 2);
    RefreshSceneTitleText(strIndex * 2);
    RefreshSaveDateText();
    Thumbnail.SetSprite(SaveSystem::GetSaveThumbnail(Type, Id));
  }
}
}  // namespace CCLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
