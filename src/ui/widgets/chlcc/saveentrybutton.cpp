#include "saveentrybutton.h"

#include "../../../renderer/renderer.h"
#include "../../../profile/dialogue.h"
#include "../../../profile/ui/savemenu.h"
#include "../../../profile/games/chlcc/savemenu.h"
#include "../../../games/chlcc/savesystem.h"

namespace Impacto {
namespace UI {
namespace Widgets {
namespace CHLCC {

using namespace Impacto::Profile::SaveMenu;
using namespace Impacto::Profile::CHLCC::SaveMenu;
using namespace Impacto::SaveSystem;

glm::vec4 SaveEntryButton::FocusedAlpha = glm::vec4(1.0f);
Animation SaveEntryButton::FocusedAlphaFade;

SaveEntryButton::SaveEntryButton(int id, Sprite const& norm,
                                 Sprite const& focused, Sprite const& highlight,
                                 glm::vec2 pos, int page, uint8_t locked,
                                 Sprite lockedSymbol)
    : Widgets::Button(id, norm, focused, highlight, pos),
      Page(page),
      FocusedSpriteLabel(focused, pos),
      LockedSymbol(lockedSymbol, pos) {
  IsLocked = locked == 1;
}

void SaveEntryButton::Render() {
  NormalSpriteLabel.Render();
  if (HasFocus) {
    SelectionMarkerLabel.Render();
    FocusedSpriteLabel.Tint = FocusedAlpha;
    FocusedSpriteLabel.Render();
  }
  ThumbnailLabel.Render();
  EntryNumberHint.Render();
  EntryNumber.Render();
  if (EntryActive) {
    if (IsLocked) {
      LockedSymbol.Render();
    }
    SceneTitle.Render();
    PlayTimeHint.Render();
    PlayTime.Render();
    SaveDateHint.Render();
    SaveDate.Render();
  } else {
    SceneTitle.Render();
  }
}

int SaveEntryButton::GetPage() const { return Page; }

void SaveEntryButton::AddNormalSpriteLabel(Sprite norm, glm::vec2 pos) {
  NormalSpriteLabel = Label(norm, pos);
}

void SaveEntryButton::AddSelectionMarkerLabel(Sprite norm, glm::vec2 pos) {
  SelectionMarkerLabel = Label(norm, pos);
}

void SaveEntryButton::AddEntryNumberHintText(Vm::BufferOffsetContext strAdr,
                                             float fontSize,
                                             RendererOutlineMode outlineMode,
                                             glm::vec2 relativePosition) {
  EntryNumberHint =
      Label(strAdr, glm::vec2(Bounds.X, Bounds.Y) + relativePosition, fontSize,
            outlineMode, IsLocked ? 69 : 0);
}

void SaveEntryButton::AddEntryNumberText(std::string_view str, float fontSize,
                                         RendererOutlineMode outlineMode,
                                         glm::vec2 relativePosition) {
  EntryNumber = Label(str, glm::vec2(Bounds.X, Bounds.Y) + relativePosition,
                      fontSize, outlineMode, IsLocked ? 69 : 0);
}

void SaveEntryButton::AddSceneTitleText(Vm::BufferOffsetContext strAdr,
                                        float fontSize,
                                        RendererOutlineMode outlineMode,
                                        glm::vec2 relativeTitlePosition,
                                        int colorIndex) {
  SceneTitle =
      Label(strAdr, glm::vec2(Bounds.X, Bounds.Y) + relativeTitlePosition,
            fontSize, outlineMode, colorIndex);
}

void SaveEntryButton::AddPlayTimeHintText(Vm::BufferOffsetContext strAdr,
                                          float fontSize,
                                          RendererOutlineMode outlineMode,
                                          glm::vec2 relativePosition) {
  PlayTimeHint = Label(strAdr, glm::vec2(Bounds.X, Bounds.Y) + relativePosition,
                       fontSize, outlineMode, IsLocked ? 69 : 0);
}

void SaveEntryButton::AddPlayTimeText(std::string_view str, float fontSize,
                                      RendererOutlineMode outlineMode,
                                      glm::vec2 relativePosition) {
  // Spacing is currently set for the C;HLCC font, more or less
  PlayTime = Label(str, glm::vec2(Bounds.X, Bounds.Y) + relativePosition,
                   fontSize, outlineMode, IsLocked ? 69 : 0);
}

void SaveEntryButton::AddSaveDateHintText(Vm::BufferOffsetContext strAdr,
                                          float fontSize,
                                          RendererOutlineMode outlineMode,
                                          glm::vec2 relativePosition) {
  SaveDateHint = Label(strAdr, glm::vec2(Bounds.X, Bounds.Y) + relativePosition,
                       fontSize, outlineMode, IsLocked ? 69 : 0);
}

void SaveEntryButton::AddSaveDateText(std::string_view str, float fontSize,
                                      RendererOutlineMode outlineMode,
                                      glm::vec2 relativePosition) {
  // Spacing is currently set for the C;HLCC font, more or less
  SaveDate = Label(str, glm::vec2(Bounds.X, Bounds.Y) + relativePosition,
                   fontSize, outlineMode, IsLocked ? 69 : 0);
}

void SaveEntryButton::AddThumbnail(Sprite thumbnail, glm::vec2 pos) {
  ThumbnailLabel = Label(thumbnail, pos);
}

void SaveEntryButton::Move(glm::vec2 relativePosition) {
  NormalSpriteLabel.Move(relativePosition);
  SelectionMarkerLabel.Move(relativePosition);
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
  if (FocusedAlphaFade.State == AnimationState::Stopped) {
    FocusedAlphaFade.Direction = AnimationDirection::In;
    FocusedAlphaFade.SetDuration(0.5f);
    FocusedAlphaFade.LoopMode = AnimationLoopMode::ReverseDirection;
    FocusedAlphaFade.StartIn();
  }
}

void SaveEntryButton::FocusedAlphaFadeReset() {
  FocusedAlphaFade.State = AnimationState::Stopped;
  FocusedAlphaFade.Progress = 0.0f;
}

void SaveEntryButton::UpdateFocusedAlphaFade(float dt) {
  FocusedAlphaFade.Update(dt);
  FocusedAlpha =
      glm::vec4(glm::vec3(1.0f), ((FocusedAlphaFade.Progress * 30) + 1) / 85);
}

void SaveEntryButton::RefreshInfo(const SaveSystem::SaveType entryType) {
  const Sprite entrySprite = [] {
    switch (*UI::SaveMenuPtr->ActiveMenuType) {
      case SaveMenuPageType::QuickLoad:
        return QuickLoadEntrySprite;
        break;
      case SaveMenuPageType::Save:
        return SaveEntrySprite;
        break;
      case SaveMenuPageType::Load:
        return LoadEntrySprite;
        break;

      default:
        throw std::runtime_error(
            fmt::format("Unexpected SaveMenuPageType: %d",
                        static_cast<int>(*UI::SaveMenuPtr->ActiveMenuType)));
    }
  }();

  IsLocked = SaveSystem::GetSaveFlags(entryType, Id) & WriteProtect;

  AddNormalSpriteLabel(entrySprite, EntryPositions[Id % 6]);
  AddSelectionMarkerLabel(SelectionMarkerSprite,
                          {EntryPositions[Id % 6].x - SelectionMarkerOffset.x,
                           EntryPositions[Id % 6].y + SelectionMarkerOffset.y});
  AddEntryNumberHintText(Vm::ScriptGetTextTableStrAddress(0, 6), 18,
                         RendererOutlineMode::BottomRight,
                         EntryNumberHintTextRelativePos);
  AddEntryNumberText(fmt::format("{:02}", Id + 1), 18,
                     RendererOutlineMode::BottomRight,
                     EntryNumberTextRelativePos);

  FocusedSpriteLabel.MoveTo(EntryPositions[Id % 6]);
  LockedSymbol.MoveTo(EntryPositions[Id % 6] + LockedSymbolRelativePos);

  switch (SaveSystem::GetSaveStatus(entryType, Id)) {
    case 0: {  // No data
      EntryActive = false;

      AddSceneTitleText(Vm::ScriptGetTextTableStrAddress(0, 1), 24,
                        RendererOutlineMode::BottomRight, NoDataTextRelativePos,
                        0);
      AddThumbnail(EmptyThumbnailSprite,
                   EntryPositions[Id % 6] + ThumbnailRelativePos);
    } break;

    case 1: {  // Filled
      EntryActive = true;

      AddSceneTitleText(Vm::ScriptGetTextTableStrAddress(
                            1, SaveSystem::GetSaveTitle(entryType, Id)),
                        24, RendererOutlineMode::BottomRight,
                        SceneTitleTextRelativePos, IsLocked ? 69 : 0);
      AddPlayTimeHintText(Vm::ScriptGetTextTableStrAddress(0, 2), 18,
                          RendererOutlineMode::BottomRight,
                          PlayTimeHintTextRelativePos);

      const uint32_t time = SaveSystem::GetSavePlayTime(entryType, Id);
      const uint32_t hours = time / 3600;
      const uint32_t minutes = (time % 3600) / 60;
      const uint32_t seconds = (time % 3600) % 60;
      AddPlayTimeText(fmt::format("{:3}:{:02}:{:02}", hours, minutes, seconds),
                      18, RendererOutlineMode::BottomRight,
                      {PlayTimeTextRelativePos.x + (float)((hours < 10) * 10),
                       PlayTimeTextRelativePos.y});

      AddSaveDateHintText(Vm::ScriptGetTextTableStrAddress(0, 3), 18,
                          RendererOutlineMode::BottomRight,
                          SaveDateHintTextRelativePos);
      const tm& date = SaveSystem::GetSaveDate(entryType, Id);
      AddSaveDateText(fmt::format("  {:%y/%m/%d %H:%M:%S}", date), 18,
                      RendererOutlineMode::BottomRight,
                      SaveDateTextRelativePos);

      AddThumbnail(SaveSystem::GetSaveThumbnail(entryType, Id),
                   EntryPositions[Id % 6] + ThumbnailRelativePos);
    } break;

    default:
      ImpLogSlow(LogLevel::Error, LogChannel::General,
                 "Unexpected entry status");
      [[fallthrough]];
    case 2: {  // Corrupted
      EntryActive = false;

      AddSceneTitleText(Vm::ScriptGetTextTableStrAddress(0, 0), 24,
                        RendererOutlineMode::BottomRight, NoDataTextRelativePos,
                        53);
      AddThumbnail(EmptyThumbnailSprite,
                   EntryPositions[Id % 6] + ThumbnailRelativePos);
    } break;
  }
}

void SaveEntryButton::ToggleLock(const SaveSystem::SaveType entryType) {
  const uint8_t newLock =
      SaveSystem::GetSaveFlags(entryType, Id) ^ WriteProtect;
  SaveSystem::SetSaveFlags(entryType, Id, newLock);
  IsLocked = newLock & WriteProtect;
}

}  // namespace CHLCC
}  // namespace Widgets
}  // namespace UI
}  // namespace Impacto
