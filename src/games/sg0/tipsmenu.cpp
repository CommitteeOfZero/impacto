#include "tipsmenu.h"

#include <algorithm>

#include "fullwidth.h"
#include "../../profile/games/sg0/tipsmenu.h"
#include "../../profile/ui/tipsmenu.h"
#include "../../profile/dialogue.h"
#include "../../profile/scriptvars.h"
#include "../../profile/vm.h"
#include "../../data/tipssystem.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../audio/audiosystem.h"

namespace Impacto {
namespace UI {
namespace SG0 {

using namespace Impacto::Profile::SG0::TipsMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Vm::Interface;

namespace {
constexpr int SubMenuNo = 2;
// Tip record strings: 0 category, 1 name, 2 sort key, 3 list name, 4.. note.
constexpr int CategoryString = 0;
constexpr int NameString = 1;
constexpr int ListNameString = 3;
constexpr int NoteString = 4;

std::vector<ProcessedTextGlyph> LayoutStream(Vm::Sc3Stream stream,
                                             glm::vec2 pos, float size,
                                             uint32_t color) {
  return TextLayoutPlainLine(stream, 255, *Profile::Dialogue::DialogueFont,
                             size, {color, 0}, 1.0f, pos, TextAlignment::Left);
}

std::vector<ProcessedTextGlyph> LayoutAscii(std::string_view text,
                                            glm::vec2 pos, float size,
                                            uint32_t color) {
  return TextLayoutPlainString(FullWidth(text),
                               *Profile::Dialogue::DialogueFont, size,
                               {color, 0}, 1.0f, pos, TextAlignment::Left);
}

bool InTab(const TipsSystem::TipsDataRecord& record, int tab) {
  switch (tab) {
    case 1:
      return !record.IsLocked;
    case 2:
      return !record.IsLocked && record.IsUnread;
    case 3:
      return !record.IsLocked && record.IsNew;
    default:
      return true;
  }
}
}  // namespace

void TipsMenu::Init() {
  TipsSystem::UpdateTipRecords();
  BuildList();
}

void TipsMenu::BuildList() {
  const auto* records = TipsSystem::GetTipRecords();
  TabCounts.fill(0);
  if (records) {
    for (const auto& record : *records) {
      for (int tab = 0; tab < TabCount; tab++) {
        if (InTab(record, tab)) TabCounts[tab]++;
      }
    }
  }

  Items.clear();
  if (!records) return;
  for (const auto& record : *records) {
    if (!InTab(record, CurrentTab)) continue;
    const float y = ListTop + Items.size() * RowHeight;
    ListItem item{record.Id, {}};
    item.Text = LayoutAscii(fmt::format("{:03d}", record.Id + 1),
                            glm::vec2(ListNumberX, y), ListFontSize, TextColor);
    std::vector<ProcessedTextGlyph> name =
        record.IsLocked
            ? LayoutAscii("???", glm::vec2(ListNameX, y), ListFontSize,
                          LockedColor)
            : LayoutStream(
                  TipsSystem::GetTextStringStream(record.Id, ListNameString),
                  glm::vec2(ListNameX, y), ListFontSize, TextColor);
    SquishText(name, ListNameMaxWidth, ListNameX);
    item.Text.insert(item.Text.end(), name.begin(), name.end());
    Items.push_back(std::move(item));
  }
  Cursor =
      std::clamp(Cursor, 0, std::max(0, static_cast<int>(Items.size()) - 1));
  ClampListScroll();
}

void TipsMenu::SetTab(int tab) {
  if (tab == CurrentTab || TabCounts[tab] == 0) return;
  CurrentTab = tab;
  Cursor = 0;
  ListScroll = 0;
  BuildList();
  Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
}

void TipsMenu::ClampListScroll() {
  if (Cursor < ListScroll) ListScroll = Cursor;
  if (Cursor >= ListScroll + VisibleRows) ListScroll = Cursor - VisibleRows + 1;
  ListScroll = std::clamp(
      ListScroll, 0, std::max(0, static_cast<int>(Items.size()) - VisibleRows));
}

void TipsMenu::ShowTip(int tipId) {
  TipsSystem::TipsDataRecord* record = TipsSystem::GetTipRecord(tipId);
  if (!record || record->IsLocked) {
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 4, false, 0);
    return;
  }
  DisplayedTip = tipId;
  TipsSystem::SetTipUnreadState(tipId, false);
  TipsSystem::SetTipNewState(tipId, false);

  NumberGlyphs = LayoutAscii(fmt::format("{:03d}", tipId + 1), NumberPos,
                             HeaderFontSize, TextColor);
  CategoryGlyphs =
      LayoutStream(TipsSystem::GetTextStringStream(tipId, CategoryString),
                   CategoryPos, HeaderFontSize, TextColor);
  NameGlyphs = LayoutStream(TipsSystem::GetTextStringStream(tipId, NameString),
                            NamePos, NameFontSize, TextColor);
  SquishText(NameGlyphs, NameMaxWidth, NamePos.x);

  auto [buffers, tipsBufferId] = TipsSystem::GetTipsScriptBufferCtx();
  Vm::Sc3VmThread dummy;
  dummy.ScriptBufferId = tipsBufferId;
  dummy.UseMSBBuffers = Profile::Vm::UseMsbStrings;
  dummy.SetStringIp(TipsSystem::GetTextStringStream(tipId, NoteString).Data());
  TextPage.Clear();
  TextPage.AddString(&dummy);
  NoteScroll = 0.0f;
  NoteScrollMax =
      std::max(0.0f, TextPage.Bounds.Bottom() - NoteBounds.Bottom());

  Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0);
}

void TipsMenu::Show() {
  if (State == Shown || State == Showing) return;
  State = Showing;
  TipsSystem::UpdateTipRecords();
  BuildList();
  DisplayedTip = -1;
  if (UI::FocusedMenu != nullptr && UI::FocusedMenu != this) {
    LastFocusedMenu = UI::FocusedMenu;
    LastFocusedMenu->IsFocused = false;
  }
  UI::FocusedMenu = this;
}

void TipsMenu::Hide() {
  if (State == Hidden || State == Hiding) return;
  State = Hiding;
  IsFocused = false;
  if (LastFocusedMenu != nullptr) {
    UI::FocusedMenu = LastFocusedMenu;
    LastFocusedMenu->IsFocused = true;
    LastFocusedMenu = nullptr;
  } else {
    UI::FocusedMenu = nullptr;
  }
}

void TipsMenu::UpdateInput(float dt) {
  if (!IsFocused) return;
  const int count = static_cast<int>(Items.size());

  int next = Cursor;
  if (PADinputButtonRepeatAccelDown & PAD1UP) next = std::max(Cursor - 1, 0);
  if (PADinputButtonRepeatAccelDown & PAD1DOWN) {
    next = std::min(Cursor + 1, std::max(count - 1, 0));
  }
  if (PADinputButtonRepeatAccelDown & PAD1LEFT) {
    next = std::max(Cursor - VisibleRows, 0);
  }
  if (PADinputButtonRepeatAccelDown & PAD1RIGHT) {
    next = std::min(Cursor + VisibleRows, std::max(count - 1, 0));
  }

  // Mouse: hover a row to select it, click to open; wheel scrolls the list
  // (or the note while the pointer is over it).
  const glm::vec2 mouse = Input::CurMousePos;
  const bool overNote = NoteBounds.ContainsPoint(mouse);
  for (int row = 0; row < VisibleRows && ListScroll + row < count; row++) {
    const RectF bounds(ListBounds.X, ListTop + row * RowHeight - RowPadding,
                       ListBounds.Width, RowHeight);
    if (!bounds.ContainsPoint(mouse)) continue;
    if (Input::PrevMousePos != mouse) next = ListScroll + row;
    if (Input::MouseButtonWentDown[SDL_BUTTON_LEFT]) {
      Cursor = ListScroll + row;
      ShowTip(Items[Cursor].TipId);
    }
  }
  if (!overNote && Input::MouseWheelDeltaY != 0) {
    next = std::clamp(Cursor - static_cast<int>(Input::MouseWheelDeltaY), 0,
                      std::max(count - 1, 0));
  }
  if (next != Cursor) {
    Cursor = next;
    ClampListScroll();
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
  }

  for (int tab = 0; tab < TabCount; tab++) {
    if (Input::MouseButtonWentDown[SDL_BUTTON_LEFT] &&
        TabBounds[tab].ContainsPoint(mouse)) {
      SetTab(tab);
    }
  }
  if (PADinputButtonWentDown & (PAD1R1 | PAD1L1)) {
    const int step = PADinputButtonWentDown & PAD1R1 ? 1 : TabCount - 1;
    int tab = (CurrentTab + step) % TabCount;
    while (TabCounts[tab] == 0 && tab != CurrentTab) {
      tab = (tab + step) % TabCount;
    }
    SetTab(tab);
  }

  if (PADinputButtonWentDown & PAD1A && count > 0) ShowTip(Items[Cursor].TipId);

  if (DisplayedTip >= 0) {
    float scroll = 0.0f;
    if (PADinputButtonIsDown & PAD1UP_RS) scroll -= NoteScrollSpeed * dt;
    if (PADinputButtonIsDown & PAD1DOWN_RS) scroll += NoteScrollSpeed * dt;
    if (overNote) scroll -= Input::MouseWheelDeltaY * RowHeight;
    NoteScroll = std::clamp(NoteScroll + scroll, 0.0f, NoteScrollMax);
  }
}

void TipsMenu::Update(float dt) {
  const int count =
      ScrWork[SW_SYSSUBMENUNO] == SubMenuNo ? ScrWork[SW_SYSSUBMENUCT] : 0;
  if (count > LastSubMenuCount && count > 0) {
    Show();
  } else if (count < LastSubMenuCount) {
    Hide();
  }
  LastSubMenuCount = count;

  if (State == Showing && count >= CountMax) {
    State = Shown;
    IsFocused = UI::FocusedMenu == this;
  } else if (State == Hiding && count <= 0) {
    State = Hidden;
  }
  if (State == Shown) UpdateInput(dt);
}

void TipsMenu::Render() {
  if (State == Hidden) return;
  const float alpha =
      ScrWork[SW_SYSSUBMENUNO] == SubMenuNo
          ? std::clamp(ScrWork[SW_SYSSUBMENUCT] / float(CountMax), 0.0f, 1.0f)
          : 0.0f;
  const glm::vec4 tint(1.0f, 1.0f, 1.0f, alpha);
  Fonts::Font& font = *Profile::Dialogue::DialogueFont;

  Renderer->DrawSprite(Profile::TipsMenu::BackgroundSprite, glm::vec2(0.0f),
                       tint);
  // The TIPS emblem sits behind the (translucent) panels.
  Renderer->DrawSprite(HeaderSprite, HeaderPos, tint);
  Renderer->DrawSprite(HeaderIconSprite, HeaderIconPos, tint);
  Renderer->DrawSprite(PanelsSprite, glm::vec2(0.0f), tint);

  for (int tab = 0; tab < TabCount; tab++) {
    const RectF& cell = TabBounds[tab];
    const Sprite& label = tab == CurrentTab    ? TabSelectedSprites[tab]
                          : TabCounts[tab] > 0 ? TabSprites[tab]
                                               : TabDisabledSprites[tab];
    Renderer->DrawSprite(
        label,
        glm::vec2(cell.X + (cell.Width - label.ScaledWidth()) / 2.0f,
                  cell.Y + (cell.Height - label.ScaledHeight()) / 2.0f),
        tint);
  }

  // List: rows ListScroll .. ListScroll + VisibleRows, shifted to the top.
  const float listShift = -ListScroll * RowHeight;
  Renderer->EnableScissor();
  Renderer->SetScissorRect(ListBounds);
  for (int i = ListScroll;
       i < std::min(static_cast<int>(Items.size()), ListScroll + VisibleRows);
       i++) {
    const float y = ListTop + i * RowHeight + listShift;
    if (i == Cursor) {
      Renderer->DrawSprite(SelectionSprite,
                           glm::vec2(ListBounds.X, y - RowPadding), tint);
    }
    DrawTextAt(Items[i].Text, glm::vec2(0.0f, listShift), alpha,
               RendererOutlineMode::None);
    const auto* record = TipsSystem::GetTipRecord(Items[i].TipId);
    if (record && !record->IsLocked && record->IsNew) {
      Renderer->DrawSprite(NewBadgeSprite,
                           glm::vec2(NewBadgeX, y - RowPadding + 6.0f), tint);
    }
  }
  Renderer->DisableScissor();

  if (DisplayedTip >= 0) {
    font.DrawProcessedText(NumberGlyphs, alpha, alpha,
                           RendererOutlineMode::None);
    font.DrawProcessedText(CategoryGlyphs, alpha, alpha,
                           RendererOutlineMode::None);
    font.DrawProcessedText(NameGlyphs, alpha, alpha, RendererOutlineMode::None);

    Renderer->EnableScissor();
    Renderer->SetScissorRect(NoteBounds);
    DrawTextAt(TextPage.Glyphs, glm::vec2(0.0f, -NoteScroll), alpha,
               RendererOutlineMode::None);
    Renderer->DisableScissor();

    if (NoteScrollMax > 0.0f) {
      const float travel = NoteBounds.Height - NoteThumbSprite.ScaledHeight();
      Renderer->DrawSprite(
          NoteThumbSprite,
          glm::vec2(NoteThumbX,
                    NoteBounds.Y + travel * (NoteScroll / NoteScrollMax)),
          tint);
    }
  }

  Renderer->DrawSprite(GuideSprite, GuidePos, tint);
}

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
