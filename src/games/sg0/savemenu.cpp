#include "savemenu.h"
#include "fullwidth.h"

#include <algorithm>
#include <ctime>

#include "../../profile/games/sg0/savemenu.h"
#include "../../profile/dialogue.h"
#include "../../profile/scriptvars.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../audio/audiosystem.h"

namespace Impacto {
namespace UI {
namespace SG0 {

using namespace Impacto::Profile::SG0::SaveMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

SaveMenu::SaveMenu() : UI::SaveMenu() {}

SaveSystem::SaveType SaveMenu::CurrentSaveType() const {
  return ActiveMenuType == SaveMenuPageType::QuickLoad
             ? SaveSystem::SaveType::Quick
             : SaveSystem::SaveType::Full;
}

glm::vec2 SaveMenu::EntryPos(int id) const {
  const int slot = id % EntriesPerPage;
  return PanelPos + EntryStart +
         glm::vec2(slot % EntriesPerRow, slot / EntriesPerRow) * EntrySpacing;
}

void SaveMenu::RefreshEntry(int id) {
  EntryInfo& entry = Entries[id];
  const SaveSystem::SaveType type = CurrentSaveType();
  entry.Status = SaveSystem::GetSaveStatus(type, id);
  entry.Locked = SaveSystem::GetSaveFlags(type, id) &
                 SaveSystem::SaveFlagsMode::WriteProtect;
  entry.PlayTime.clear();
  entry.SaveDate.clear();
  if (entry.Status != 1) return;

  const uint32_t time = SaveSystem::GetSavePlayTime(type, id);
  const std::string playTime = fmt::format("{:d}:{:02d}:{:02d}", time / 3600,
                                           (time % 3600) / 60, time % 60);
  char date[32];
  std::strftime(date, sizeof(date), "%Y/%m/%d %H:%M",
                &SaveSystem::GetSaveDate(type, id));

  const glm::vec2 pos = EntryPos(id);
  const DialogueColorPair colors{0xFFFFFF, 0x000000};
  entry.PlayTime = TextLayoutPlainString(
      FullWidth(playTime), *Profile::Dialogue::DialogueFont, TextFontSize,
      colors, 1.0f, pos + PlayTimeTextOffset, TextAlignment::Left);
  entry.SaveDate = TextLayoutPlainString(
      FullWidth(date), *Profile::Dialogue::DialogueFont, TextFontSize, colors,
      1.0f, pos + SaveDateTextOffset, TextAlignment::Left);
}

void SaveMenu::RefreshAllEntries() {
  for (int id = 0; id < SaveSystem::MaxSaveEntries; id++) RefreshEntry(id);
}

void SaveMenu::RefreshCurrentEntryInfo() { RefreshEntry(Cursor); }

void SaveMenu::Choose(int id) {
  const SaveSystem::SaveType type = CurrentSaveType();
  const uint8_t status = SaveSystem::GetSaveStatus(type, id);
  if (status != 1 && ActiveMenuType != SaveMenuPageType::Save) {
    // Nothing to load in an empty slot.
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 4, false, 0);
    return;
  }
  ScrWork[SW_SAVEFILENO] = id;
  ScrWork[SW_SAVEFILETYPE] = static_cast<int>(type);
  ScrWork[SW_SAVEFILESTATUS] = status;
  SetFlag(SF_SAVEPROTECTED, SaveSystem::GetSaveFlags(type, id) &
                                SaveSystem::SaveFlagsMode::WriteProtect);
  ChoiceMade = true;
}

void SaveMenu::Show() {
  if (State == Shown || State == Showing) return;
  if (!ActiveMenuType.has_value()) {
    // SW_SYSSUBMENUNO 0/3/4 -> Load/Save/QuickLoad (_Startup_win F7/F6/F5).
    switch (ScrWork[SW_SYSSUBMENUNO]) {
      case 3:
        ActiveMenuType = SaveMenuPageType::Save;
        break;
      case 4:
        ActiveMenuType = SaveMenuPageType::QuickLoad;
        break;
      default:
        ActiveMenuType = SaveMenuPageType::Load;
        break;
    }
  }
  State = Showing;
  if (UI::FocusedMenu != nullptr) {
    LastFocusedMenu = UI::FocusedMenu;
    LastFocusedMenu->IsFocused = false;
  }
  UI::FocusedMenu = this;
  RefreshAllEntries();
}

void SaveMenu::Hide() {
  if (State == Hidden || State == Hiding) return;
  State = Hiding;
  IsFocused = false;
  if (LastFocusedMenu != nullptr) {
    UI::FocusedMenu = LastFocusedMenu;
    LastFocusedMenu->IsFocused = true;
  } else {
    UI::FocusedMenu = nullptr;
  }
}

void SaveMenu::UpdateInput(float dt) {
  Menu::UpdateInput(dt);
  if (!IsFocused) return;

  const int page = Cursor / EntriesPerPage;
  const int slot = Cursor % EntriesPerPage;
  const int col = slot % EntriesPerRow;
  const int row = slot / EntriesPerRow;
  const int total = Pages * EntriesPerPage;
  int next = Cursor;

  if (PADinputButtonWentDown & PAD1DOWN) {
    next = row + 1 < RowsPerPage ? Cursor + EntriesPerRow
                                 : ((page + 1) % Pages) * EntriesPerPage + col;
  } else if (PADinputButtonWentDown & PAD1UP) {
    next = row > 0 ? Cursor - EntriesPerRow
                   : ((page + Pages - 1) % Pages) * EntriesPerPage +
                         (RowsPerPage - 1) * EntriesPerRow + col;
  } else if (PADinputButtonWentDown & PAD1RIGHT) {
    next = col + 1 < EntriesPerRow
               ? Cursor + 1
               : ((page + 1) % Pages) * EntriesPerPage + row * EntriesPerRow;
  } else if (PADinputButtonWentDown & PAD1LEFT) {
    next = col > 0 ? Cursor - 1
                   : ((page + Pages - 1) % Pages) * EntriesPerPage +
                         row * EntriesPerRow + EntriesPerRow - 1;
  } else if (Input::MouseWheelDeltaY < 0 ||
             PADinputButtonWentDown & PADcustom[8]) {
    next = ((page + 1) % Pages) * EntriesPerPage + slot;
  } else if (Input::MouseWheelDeltaY > 0 ||
             PADinputButtonWentDown & PADcustom[7]) {
    next = ((page + Pages - 1) % Pages) * EntriesPerPage + slot;
  }

  // Mouse: hover selects, click chooses.
  const glm::vec2 frameSize = EntryFrameSprite.ScaledBounds().GetSize();
  for (int i = 0; i < EntriesPerPage; i++) {
    const int id = page * EntriesPerPage + i;
    const RectF bounds(EntryPos(id).x + FrameOffset.x,
                       EntryPos(id).y + FrameOffset.y, frameSize.x,
                       frameSize.y);
    if (bounds.ContainsPoint(Input::CurMousePos)) {
      if (Input::PrevMousePos != Input::CurMousePos) next = id;
      if (Input::MouseButtonWentDown[SDL_BUTTON_LEFT]) Choose(id);
    }
  }

  next = std::clamp(next, 0, total - 1);
  if (next != Cursor) {
    Cursor = next;
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
  }

  if (PADinputButtonWentDown & PAD1A) Choose(Cursor);

  if (PADinputButtonWentDown & PAD1Y) {
    const SaveSystem::SaveType type = CurrentSaveType();
    if (SaveSystem::GetSaveStatus(type, Cursor) == 1) {
      SaveSystem::SetSaveFlags(type, Cursor,
                               SaveSystem::GetSaveFlags(type, Cursor) ^
                                   SaveSystem::SaveFlagsMode::WriteProtect);
      RefreshEntry(Cursor);
      Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0);
    } else {
      Audio::PlayInGroup(Audio::ACG_SE, "sysse", 4, false, 0);
    }
  }
}

void SaveMenu::Update(float dt) {
  const int count = ScrWork[SW_SYSSUBMENUCT];
  const int subMenu = ScrWork[SW_SYSSUBMENUNO];
  const bool isSaveSubMenu = subMenu == 0 || subMenu == 3 || subMenu == 4;

  if (count > LastSubMenuCount && count > 0 && isSaveSubMenu &&
      (State == Hidden || State == Hiding)) {
    Show();
  } else if (count < LastSubMenuCount && count < SubMenuCountMax &&
             (State == Shown || State == Showing)) {
    Hide();
  }
  LastSubMenuCount = count;

  if (State == Showing && count >= SubMenuCountMax) {
    State = Shown;
    IsFocused = true;
  } else if (State == Hiding && count <= 0) {
    State = Hidden;
    ActiveMenuType = std::nullopt;
    if (UI::FocusedMenu) UI::FocusedMenu->IsFocused = true;
  }

  if (State == Shown && isSaveSubMenu) UpdateInput(dt);
}

void SaveMenu::Render() {
  if (State == Hidden || !ActiveMenuType.has_value()) return;

  const float alpha =
      std::clamp(ScrWork[SW_SYSSUBMENUCT] / float(SubMenuCountMax), 0.0f, 1.0f);
  const glm::vec4 tint(1.0f, 1.0f, 1.0f, alpha);

  Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f), tint);
  Renderer->DrawSprite(PanelSprite, PanelPos, tint);
  Renderer->DrawSprite(HeaderSprites[static_cast<int>(*ActiveMenuType)],
                       HeaderPos, tint);

  const SaveSystem::SaveType type = CurrentSaveType();
  const int page = Cursor / EntriesPerPage;
  for (int i = 0; i < EntriesPerPage; i++) {
    const int id = page * EntriesPerPage + i;
    const EntryInfo& entry = Entries[id];
    const glm::vec2 pos = EntryPos(id);

    if (id == Cursor) {
      Renderer->DrawSprite(EntryFrameSprite, pos + FrameOffset, tint);
    }
    if (entry.Status == 1) {
      Renderer->DrawSprite(
          SaveSystem::GetSaveThumbnail(type, id),
          RectF(pos.x + ThumbnailOffset.x, pos.y + ThumbnailOffset.y,
                ThumbnailSize.x, ThumbnailSize.y),
          tint);
      Profile::Dialogue::DialogueFont->DrawProcessedText(
          entry.PlayTime, alpha, alpha, RendererOutlineMode::Full);
      Profile::Dialogue::DialogueFont->DrawProcessedText(
          entry.SaveDate, alpha, alpha, RendererOutlineMode::Full);
    } else {
      Renderer->DrawSprite(NoDataSprite, pos + ThumbnailOffset, tint);
    }
    Renderer->DrawSprite(PlayTimeLabelSprite, pos + PlayTimeLabelOffset, tint);
    Renderer->DrawSprite(SaveDateLabelSprite, pos + SaveDateLabelOffset, tint);
    if (entry.Locked) {
      Renderer->DrawSprite(LockedSprite, pos + LockedOffset, tint);
    }
  }

  // "<page>/6"
  const Sprite& current = PageDigitSprites[1 + page];
  Renderer->DrawSprite(current, PageNumberPos, tint);
  const float slashX = PageNumberPos.x + current.ScaledWidth();
  Renderer->DrawSprite(PageDigitSprites[0], glm::vec2(slashX, PageNumberPos.y),
                       tint);

  Renderer->DrawSprite(ButtonGuideSprite, ButtonGuidePos, tint);
}

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
