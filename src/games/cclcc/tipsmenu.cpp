#include "tipsmenu.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/scriptvars.h"
#include "../../profile/dialogue.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/ui/tipsmenu.h"
#include "../../profile/games/cclcc/tipsmenu.h"
#include "../../io/memorystream.h"
#include "../../data/tipssystem.h"
#include "../../vm/interface/input.h"
#include "../../profile/game.h"

#include "../../ui/widgets/cclcc/tipstabgroup.h"
namespace Impacto {
namespace UI {
namespace CCLCC {

using namespace Impacto::Profile::TipsMenu;
using namespace Impacto::Profile::CCLCC::TipsMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;
using namespace Impacto::UI::Widgets::CCLCC;

// Todo: correct audio sounds, scrollbars, fix menu not entering hide state
// (not exclusive to this menu)

struct SortByTipName {
  SortByTipName() {
    SortString = Vm::ScriptGetTextTableStrAddress(TipsTextTableIndex,
                                                  TipsTextSortStringIndex);
    int i = 0;
    int distance = 0;
    while (SortString[i] != 0xFF) {
      if (SortString[i] & 0x80) {
        uint16_t sc3Char =
            SDL_SwapBE16(UnalignedRead<uint16_t>(SortString + i));
        Sc3SortMap[sc3Char] = distance++;
        i += 2;
      } else {
        ImpLogSlow(LL_Warning, LC_VM,
                   "SortByTipName: SC3 Tag Found in Sort String\n",
                   SortString[i]);
        i++;
      }
    }
  }
  bool operator()(int a, int b) const {
    auto* aRecord = TipsSystem::GetTipRecord(a);
    auto* bRecord = TipsSystem::GetTipRecord(b);
    uint8_t* aString = aRecord->StringPtrs[3];
    uint8_t* bString = bRecord->StringPtrs[3];

    int aIndex = 0;
    int bIndex = 0;

    while (aString[aIndex] != 0xff && bString[bIndex] != 0xff) {
      if ((aString[aIndex] & 0x80) == 0) {
        aIndex++;
        continue;
      }
      if ((bString[bIndex] & 0x80) == 0) {
        bIndex++;
        continue;
      }
      uint16_t aSc3Char =
          SDL_SwapBE16(UnalignedRead<uint16_t>(aString + aIndex));
      aIndex += 2;

      uint16_t bSc3Char =
          SDL_SwapBE16(UnalignedRead<uint16_t>(bString + bIndex));
      bIndex += 2;
      if (aSc3Char != bSc3Char) {
        auto aSortValue = Sc3SortMap.find(aSc3Char);
        auto bSortValue = Sc3SortMap.find(bSc3Char);
        if (aSortValue != Sc3SortMap.end() && bSortValue != Sc3SortMap.end()) {
          return aSortValue->second < bSortValue->second;
        }
      }
    }
    // If strings are all the same, return the shorter one
    return aString[aIndex] == 0xff && bString[bIndex] != 0xff;
  }
  uint8_t* SortString;
  ska::flat_hash_map<uint16_t, int> Sc3SortMap;
};

TipsMenu::TipsMenu() : TipViewItems(this) {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  Name = new Label();
  Name->Bounds.X = NamePos.x;
  Name->Bounds.Y = NamePos.y;

  Pronounciation = new Label();
  Pronounciation->Bounds.X = PronounciationPos.x;
  Pronounciation->Bounds.Y = PronounciationPos.y;

  Category = new Label();
  Category->Bounds.X = CategoryPos.x;
  Category->Bounds.Y = CategoryPos.y;

  Number = new Label();
  Number->Bounds.X = NumberPos.x;
  Number->Bounds.Y = NumberPos.y;

  TipViewItems.Add(Name);
  TipViewItems.Add(Pronounciation);
  TipViewItems.Add(Category);
  TipViewItems.Add(Number);

  TextPage.Glyphs = new ProcessedTextGlyph[Profile::Dialogue::MaxPageSize];
  TextPage.Clear();
  TextPage.Mode = DPM_TIPS;
  TextPage.FadeAnimation.Progress = 1.0f;
}

void TipsMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;

    for (int i = 0; i < TabCount; i++) {
      TipsTabs[i]->UpdateTipsEntries(SortedTipIds);
      if (TipsTabs[i]->GetTipEntriesCount() > 0) {
        CurrentTabType = static_cast<TipsTabType>(i);
      }
    }
    TipsTabs[CurrentTabType]->Show();
    TipViewItems.Show();
  }
}
void TipsMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
    CurrentlyDisplayedTipId = -1;
    TipsTabs[CurrentTabType]->Hide();
    TipViewItems.Hide();
  }
}

void TipsMenu::UpdateInput() {
  Menu::UpdateInput();
  if (State == Shown) {
    if (CurrentlyDisplayedTipId != -1) {
      if (PADinputButtonWentDown & PAD1X) {
        NextTipPage();
      }
    }
    if (PADinputButtonWentDown & PAD1R1) {
      SetActiveTab(static_cast<TipsTabType>((CurrentTabType + 1) % TabCount));
      Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0);
    } else if (PADinputButtonWentDown & PAD1L1) {
      SetActiveTab(
          static_cast<TipsTabType>((CurrentTabType + TabCount - 1) % TabCount));
      Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0);
    }
  }
}

void TipsMenu::Update(float dt) {
  if (!HasInitialized || State == Hidden) return;
  UpdateInput();
  for (int i = 0; i < TabCount; i++) {
    TipsTabs[i]->Update(dt);
  }
  FadeAnimation.Update(dt);
  if (State == Showing && FadeAnimation.Progress == 1.0f) {
    State = Shown;
  } else if (State == Hiding && FadeAnimation.Progress == 0.0f) {
    State = Hidden;
  }
  if (State == Shown && CurrentlyDisplayedTipId != -1) {
    RectF lastCharDest = TextPage.Glyphs[TextPage.Length - 1].DestRect;

    // TODO: Add buttons to scroll text
    if (lastCharDest.Y + TipPageY < TextPage.BoxBounds.Y) {
      TextPage.Move({0, lastCharDest.Height});
      TipPageY += lastCharDest.Height;
    } else if (lastCharDest.Y + TipPageY > TextPage.BoxBounds.Y +
                                               TextPage.BoxBounds.Height +
                                               lastCharDest.Height) {
      TextPage.Move({0, -lastCharDest.Height});
      TipPageY -= lastCharDest.Height;
    }
  }
}

void TipsMenu::Render() {
  if (!HasInitialized) return;
  if (State != Hidden && ScrWork[SW_SYSMENUCT] == 32 &&
      ScrWork[SW_SYSSUBMENUNO] == 2) {
    glm::vec4 transition(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    glm::vec4 maskTint = glm::vec4(1.0f);
    maskTint.a = 0.85f;

    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f));
    Renderer->DrawSprite(TipsBookLayerSprite, glm::vec2(0.0f), transition);
    TipsTabs[CurrentTabType]->Render();

    Renderer->DrawSprite(
        TipsMaskSprite,
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        maskTint);

    Renderer->DrawSprite(TipsGuideSprite, glm::vec2(TipsGuideX, TipsGuideY),
                         transition);
    if (CurrentlyDisplayedTipId != -1) {
      TipViewItems.Render();
      Renderer->DrawProcessedText(
          TextPage.Glyphs, TextPage.Length, Profile::Dialogue::DialogueFont, 1,
          1, TextPage.BoxBounds, RendererOutlineMode::RO_None, true);
    }
  }
}

void TipsMenu::Init() {
  auto* TipRecords = TipsSystem::GetTipRecords();
  std::transform(
      TipRecords->begin(), TipRecords->end(), std::back_inserter(SortedTipIds),
      [](TipsSystem::TipsDataRecord const& record) { return record.Id; });
  std::sort(SortedTipIds.begin(), SortedTipIds.end(), SortByTipName());
  for (int i = 0; i < TabCount; i++) {
    TipsTabType type = static_cast<TipsTabType>(i);
    TipsTabs[i] = new TipsTabGroup(
        type, [this, type](Button*) { SetActiveTab(type); },
        [this](Button* target) { SwitchToTipId(target->Id); });
  }
  HasInitialized = true;
}

void TipsMenu::SwitchToTipId(int id) {
  CurrentlyDisplayedTipId = id - 1;
  int actualId = SortedTipIds[id - 1];
  auto* record = TipsSystem::GetTipRecord(actualId);
  if (record->IsLocked) return;

  TipsSystem::SetTipUnreadState(actualId, false);
  Category->SetText(record->StringPtrs[0], CategoryFontSize,
                    RendererOutlineMode::RO_None, {TipsMenuDarkTextColor, 0});
  Name->SetText(record->StringPtrs[1], NameFontSize,
                RendererOutlineMode::RO_None, {TipsMenuDarkTextColor, 0});
  Pronounciation->SetText(record->StringPtrs[2], PronounciationFontSize,
                          RendererOutlineMode::RO_None, 0);

  char temp[5];
  sprintf(temp, "%4d", id);
  Number->SetText(std::string(temp), NumberFontSize,
                  RendererOutlineMode::RO_None, 0);

  Vm::Sc3VmThread dummy;
  dummy.Ip = record->StringPtrs[4];
  TextPage.Clear();
  TextPage.AddString(&dummy);
  TipViewItems.HasFocus = true;
}

void TipsMenu::NextTipPage() {}

void TipsMenu::SetActiveTab(TipsTabType type) {
  if (!TipsTabs[type]->GetTipEntriesCount()) return;
  TipsTabs[CurrentTabType]->Hide();
  TipsTabs[type]->Show();
  CurrentTabType = type;
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto