#include "tipsmenu.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../inputsystem.h"
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

  TextPage.Clear();
  TextPage.Mode = DPM_TIPS;
  TextPage.FadeAnimation.Progress = 1.0f;

  TipsScrollStartPos = {
      TipsScrollDetailsX,
      TipsScrollYStart + TipsScrollThumbSprite.ScaledHeight() / 2.0f};

  TipsScrollTrackBounds = {
      TipsScrollThumbSprite.Bounds.Width,
      TipsScrollYEnd - TipsScrollYStart - TipsScrollThumbSprite.ScaledHeight()};
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
    delete TipsScrollbar;
    TipsScrollbar = nullptr;
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
      TipsTabType type =
          static_cast<TipsTabType>((CurrentTabType + 1) % TabCount);
      while (!TipsTabs[type]->GetTipEntriesCount()) {
        type = static_cast<TipsTabType>((type + 1) % TabCount);
      }
      SetActiveTab(type);
    } else if (PADinputButtonWentDown & PAD1L1) {
      TipsTabType type =
          static_cast<TipsTabType>((CurrentTabType + TabCount - 1) % TabCount);
      while (!TipsTabs[type]->GetTipEntriesCount()) {
        type = static_cast<TipsTabType>((type + TabCount - 1) % TabCount);
      }
      SetActiveTab(type);
    }

    if (CurrentlyDisplayedTipId != -1) {
      RectF lastCharDest = TextPage.Glyphs.back().DestRect;

      int scrollDistance = 10;
      if (Input::CurMousePos != Input::PrevMousePos) {
        MouseInTextBounds =
            TextPage.BoxBounds.ContainsPoint(Input::CurMousePos);
      }

      // Todo: Implement scroll wheel
      bool upScroll = (Input::CurrentInputDevice == Input::IDEV_Controller &&
                       Input::ControllerAxis[SDL_CONTROLLER_AXIS_RIGHTY] <
                           Input::ControllerAxisLightThreshold) ||
                      Input::KeyboardButtonIsDown[SDL_SCANCODE_LEFTBRACKET];

      bool downScroll = (Input::CurrentInputDevice == Input::IDEV_Controller &&
                         Input::ControllerAxis[SDL_CONTROLLER_AXIS_RIGHTY] >
                             -Input::ControllerAxisLightThreshold) ||
                        Input::KeyboardButtonIsDown[SDL_SCANCODE_RIGHTBRACKET];

      int remainingScroll = TipsScrollbar->MaxValue - TipPageY;
      if (upScroll && (TipPageY > 0)) {
        if (scrollDistance > TipPageY) {
          scrollDistance = TipPageY;
        }
        TipPageY -= scrollDistance;
      } else if (downScroll && (remainingScroll > 0)) {
        if (scrollDistance > remainingScroll) {
          scrollDistance = remainingScroll;
        }
        TipPageY += scrollDistance;
      }
    }
  }
}

void TipsMenu::Update(float dt) {
  if (!HasInitialized) return;
  if (ScrWork[SW_SYSSUBMENUCT] < 32 && State == Shown &&
      (ScrWork[SW_SYSSUBMENUNO] == 2)) {
    Hide();
  } else if (ScrWork[SW_SYSSUBMENUCT] >= 32 && State == Hidden &&
             (ScrWork[SW_SYSSUBMENUNO] == 2)) {
    Show();
  }
  if (State == Shown && ScrWork[SW_SYSSUBMENUNO] == 2) {
    float oldPageY = TipPageY;
    UpdateInput();
    if (TipsScrollbar) {
      TipsScrollbar->UpdateInput();
      TipsScrollbar->Update(dt);
      if (oldPageY != TipPageY) {
        TextPage.Move({0, oldPageY - TipPageY});
      }
    }
    for (int i = 0; i < TabCount; i++) {
      TipsTabs[i]->Update(dt);
    }
  }
  FadeAnimation.Update(dt);
  if (State == Showing && FadeAnimation.Progress == 1.0f) {
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
    maskTint.a = 0.85f * FadeAnimation.Progress;

    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f));
    Renderer->DrawSprite(TipsBookLayerSprite, glm::vec2(0.0f), transition);
    TipsTabs[CurrentTabType]->Render();

    Renderer->DrawSprite(TipsGuideSprite, glm::vec2(TipsGuideX, TipsGuideY),
                         transition);
    if (CurrentlyDisplayedTipId != -1) {
      TipViewItems.Render();
      Renderer->DrawProcessedText(
          TextPage.Glyphs, Profile::Dialogue::DialogueFont,
          FadeAnimation.Progress, FadeAnimation.Progress,
          RendererOutlineMode::RO_None, true, &TipsMaskSheet);
      TipsScrollbar->Render();
    }

    Renderer->DrawSprite(
        TipsMaskSprite,
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        maskTint);
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
  if (id - 1 == CurrentlyDisplayedTipId) return;
  int actualId = SortedTipIds[id - 1];
  auto* record = TipsSystem::GetTipRecord(actualId);

  if (record->IsLocked) {
    Audio::Channels[Audio::AC_SSE]->Play("sysse", 4, false, 0);
    return;
  }
  CurrentlyDisplayedTipId = id - 1;

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

  auto& lastGlyph = TextPage.Glyphs.back();
  int scrollDistance = lastGlyph.DestRect.Y + lastGlyph.DestRect.Height -
                       (TextPage.BoxBounds.Y + TextPage.BoxBounds.Height) +
                       lastGlyph.DestRect.Height;
  ;
  TipPageY = 0;
  delete TipsScrollbar;
  TipsScrollbar = new Scrollbar(
      0, TipsScrollStartPos, 0, std::max(0, scrollDistance), &TipPageY,
      SBDIR_VERTICAL, TipsScrollThumbSprite, TipsScrollTrackBounds);
  TipsScrollbar->HasFocus = false;  // We want to manually control kb/pad input

  Audio::Channels[Audio::AC_SSE]->Play("sysse", 2, false, 0);
}

void TipsMenu::NextTipPage() {}

void TipsMenu::SetActiveTab(TipsTabType type) {
  if (type == CurrentTabType || !TipsTabs[type]->GetTipEntriesCount()) return;

  Audio::Channels[Audio::AC_SSE]->Play("sysse", 1, false, 0);

  TipsTabs[CurrentTabType]->Hide();
  TipsTabs[type]->Show();
  CurrentTabType = type;
}

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto