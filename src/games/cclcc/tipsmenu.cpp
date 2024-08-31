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

SortByTipName::SortByTipName() {
  SortString = Vm::ScriptGetTextTableStrAddress(2, 10);
  int i = 0;
  int distance = 0;
  while (SortString[i] != 0xFF) {
    if (SortString[i] & 0x80) {
      uint8_t sc3Char = SortString[i + 1];
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

bool SortByTipName::operator()(int a, int b) const {
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
    uint8_t aSc3Char = aString[aIndex + 1];
    aIndex += 2;
    uint8_t bSc3Char = bString[bIndex + 1];
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

TipsMenu::TipsMenu()
    : TipViewItems(Carousel(CarouselDirection::CDIR_HORIZONTAL)) {
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
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
      TipViewItems.Next();
    } else if (PADinputButtonWentDown & PAD1L1) {
      TipViewItems.Previous();
    }
  }
}

void TipsMenu::Update(float dt) {
  if (!HasInitialized) return;
  UpdateInput();
  for (int i = 0; i < TabCount; i++) {
    TipsTabs[i]->Update(dt);
  }
  FadeAnimation.Update(dt);
  if (ScrWork[SW_SYSMENUCT] != 32 && State == Shown &&
      ScrWork[SW_SYSSUBMENUNO] == 2) {
    Hide();
  } else if (ScrWork[SW_SYSMENUCT] == 32 && State == Hidden &&
             ScrWork[SW_SYSSUBMENUNO] == 2) {
    Show();
    for (int i = 0; i < TabCount; i++) {
      TipsTabs[i]->UpdateTipsEntries(*SortedTipIds);
      if (TipsTabs[i]->GetTipEntriesCount() > 0) {
        CurrentTabType = static_cast<TipsTabType>(i);
      }
    }
    TipsTabs[CurrentTabType]->Show();
  } else if (State == Showing && FadeAnimation.Progress == 1.0f) {
    State = Shown;
  } else if (State == Hiding && FadeAnimation.Progress == 0.0f) {
    State = Hidden;
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
    TipViewItems.Render();

    Renderer->DrawSprite(
        TipsMaskSprite,
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        maskTint);

    Renderer->DrawSprite(TipsGuideSprite, glm::vec2(TipsGuideX, TipsGuideY),
                         transition);
  }
}

void TipsMenu::Init() {
  auto* TipRecords = TipsSystem::GetTipRecords();
  SortedTipIds = new std::set<int, SortByTipName>();
  std::transform(
      TipRecords->begin(), TipRecords->end(),
      std::inserter(*SortedTipIds, SortedTipIds->begin()),
      [](TipsSystem::TipsDataRecord const& record) { return record.Id; });
  TipsTabs[TipsTabType::AllTips] = new TipsTabGroup(
      this, TipsTabType::AllTips, TipsTabBounds,
      TipsTabNameDisplay + glm::vec2{0 * TipsHighlightedTabAdder, 0},
      TipsHighlightedTabSprite);
  TipsTabs[TipsTabType::UnlockedTips] = new TipsTabGroup(
      this, TipsTabType::UnlockedTips, TipsTabBounds,
      TipsTabNameDisplay + glm::vec2{1 * TipsHighlightedTabAdder, 0},
      TipsHighlightedTabSprite);
  TipsTabs[TipsTabType::UnreadTips] = new TipsTabGroup(
      this, TipsTabType::UnreadTips, TipsTabBounds,
      TipsTabNameDisplay + glm::vec2{2 * TipsHighlightedTabAdder, 0},
      TipsHighlightedTabSprite);
  TipsTabs[TipsTabType::NewTips] = new TipsTabGroup(
      this, TipsTabType::NewTips, TipsTabBounds,
      TipsTabNameDisplay + glm::vec2{3 * TipsHighlightedTabAdder, 0},
      TipsHighlightedTabSprite);
  TipViewItems.Add(TipsTabs[0]);
  TipViewItems.Add(TipsTabs[1]);
  TipViewItems.Add(TipsTabs[2]);
  TipViewItems.Add(TipsTabs[3]);
  HasInitialized = true;
}

void TipsMenu::SwitchToTipId(int id) {}

void TipsMenu::NextTipPage() {}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto