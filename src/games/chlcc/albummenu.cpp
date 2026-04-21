#include "albummenu.h"

#include "../../profile/games/chlcc/albummenu.h"
#include "../../profile/scriptvars.h"
#include "../../profile/profile_internal.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../inputsystem.h"
#include "../../data/savesystem.h"
#include "../../ui/widgets/chlcc/albumthumbnailbutton.h"
#include "../../profile/games/chlcc/commonmenu.h"
#include "../../vm/interface/input.h"

#include "../../ui/widgets/group.h"

namespace Impacto {
namespace UI {
namespace CHLCC {
using namespace Impacto::Profile::CHLCC::CommonMenu;

using namespace Impacto::Profile::CHLCC::AlbumMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

using namespace Impacto::Vm::Interface;
using namespace Impacto::UI::Widgets::CHLCC;

void AlbumMenu::OnCgVariationEnd(Widgets::CgViewer* target) {
  CgViewerWidget->Hide();
  ButtonGuideFade.StartIn();
}

void AlbumMenu::CgOnClick(Widgets::Button* target) {
  if (!target->IsLocked) {
    int total, viewed = 0;
    SaveSystem::GetEVStatus(target->Id, &total, &viewed);

    ShowCgViewer = true;
    CgViewerWidget->LoadCgSprites((size_t)target->Id, "bg",
                                  Profile::SaveSystem::AlbumData[target->Id]);
    ButtonGuideFade.StartOut();
  }
}

AlbumMenu::AlbumMenu() : CommonMenu(false) {
  CurrentPage = 0;
  MaxReachablePage = 0;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;

  ButtonGuideFade.SetDuration(CgFadeDuration);
  ButtonGuideFade.Finish();

  CgViewerWidget = new Widgets::CgViewer(CgFadeDuration);
  CgViewerWidget->OnVariationEndHandler = [this](auto* btn) {
    return OnCgVariationEnd(btn);
  };
  CgViewerGroup = new Group(this);
  CgViewerGroup->Add(CgViewerWidget, FDIR_DOWN);

  auto cgOnClick = [this](auto* btn) { return CgOnClick(btn); };

  for (int k = 0; k <= AlbumPages; k++) {
    auto page = new Group(this);
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        auto button = new AlbumThumbnailButton(
            (EntriesPerPage * k + 3 * i + j),
            (AlbumThumbnails[EntriesPerPage * k + 3 * i + j]),
            ThumbnailHighlight, ThumbnailHighlight,
            glm::vec2(ThumbnailTemplatePosition.x + ThumbnailOffset.x * j,
                      ThumbnailTemplatePosition.y + ThumbnailOffset.y * i),
            0, 0, VariationUnlocked, VariationLocked, VariationTemplateOffset,
            LockedCG, SelectionMarkerSprite, SelectionMarkerRelativePos);
        button->OnClickHandler = cgOnClick;
        page->Add(button);
      }
    }
    Pages.push_back(page);
  }
}

void AlbumMenu::Show() {
  if (State != Shown) {
    if (State != Showing) {
      MenuTransition.StartIn();
      SelectAnimation.StartIn(true);
    };
    UpdatePages();
    Pages[CurrentPage]->Show();
    Pages[CurrentPage]->HasFocus = false;
    State = Showing;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
    Pages[CurrentPage]->Children.front()->HasFocus = true;
    CurrentlyFocusedElement = Pages[CurrentPage]->Children.front();
  }
}

void AlbumMenu::Hide() {
  if (State != Hidden) {
    if (State != Hiding) {
      AlbumThumbnailButton::FocusedAlphaFadeReset();
      MenuTransition.StartOut();
    }
    Pages[CurrentPage]->HasFocus = false;
    State = Hiding;
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void AlbumMenu::Render() {
  if (State == Hidden) return;
  CommonMenu::DrawSubmenu(BackgroundColor, CircleSprite, AlbumMenuTitle,
                          AlbumMenuTitleAngle, true);

  if (MenuTransition.Progress < 0.22f) return;

  DrawPage();
  CgViewerGroup->Render();
  // show only when CGViewer is hidden, hiding, showing
  if (!CgViewerWidget->IsShown()) {
    CommonMenu::DrawButtonPrompt(ButtonGuide, ButtonGuidePos);
  }
  if (CgViewerWidget->IsFadingBetweenVariations() &&
      CgViewerWidget->IsOnLastVariation()) {
    float progress = CgViewerWidget->GetVariationFadeProgress();
    float alpha = glm::smoothstep(1.0f, 0.0f, progress);

    Renderer->DrawSprite(CgViewerButtonGuideVariation, CgViewerButtonGuidePos,
                         glm::vec4(1.0f, 1.0f, 1.0f, alpha));
    Renderer->DrawSprite(CgViewerButtonGuideNoVariation, CgViewerButtonGuidePos,
                         glm::vec4(1.0f, 1.0f, 1.0f, 1.0f - alpha));
  } else {
    float alpha = glm::smoothstep(0.0f, 1.0f, 1.0f - ButtonGuideFade.Progress);
    auto sprite = CgViewerWidget->IsOnLastVariation()
                      ? CgViewerButtonGuideNoVariation
                      : CgViewerButtonGuideVariation;
    Renderer->DrawSprite(sprite, CgViewerButtonGuidePos,
                         glm::vec4(1.0f, 1.0f, 1.0f, alpha));
  }
}

void AlbumMenu::UpdateInput(float dt) {
  using namespace Vm::Interface;
  if (!ShowCgViewer) Menu::UpdateInput(dt);
  if (State == Shown) {
    CgViewerGroup->UpdateInput(dt);
    if (!ShowCgViewer) Pages[CurrentPage]->UpdateInput(dt);
    if (PADinputButtonWentDown & PAD1B || PADinputMouseWentDown & PAD1B) {
      if (CgViewerGroup->VisibilityState != Hidden) {
        CgViewerWidget->Hide();
        ButtonGuideFade.StartIn();
      } else {
        SetFlag(SF_ALBUMEND, true);
      }
    }
    const auto updatePage = [&](int nextPage) {
      PrevPage = CurrentPage;
      if (CurrentlyFocusedElement) {
        CurrentlyFocusedElement->HasFocus = false;
        CurrentlyFocusedElement->Hovered = false;
      }
      CurrentPage = nextPage;
      Pages[CurrentPage]->Show();
      Pages[CurrentPage]->Children.front()->HasFocus = true;
      CurrentlyFocusedElement = Pages[CurrentPage]->Children.front();
    };
    if (IsFocused && !ShowCgViewer) {
      if (Input::MouseWheelDeltaY < 0 ||
          PADinputButtonWentDown & PADcustom[8]) {
        updatePage((CurrentPage + 1) % AlbumPages);
      } else if (Input::MouseWheelDeltaY > 0 ||
                 PADinputButtonWentDown & PADcustom[7]) {
        updatePage((CurrentPage - 1 + AlbumPages) % AlbumPages);
      }
    }
  }
}

void AlbumMenu::Update(float dt) {
  if ((!GetFlag(SF_ALBUMMENU) || ScrWork[SW_SYSMENUCT] < 10000) &&
      State == Shown) {
    Hide();
  } else if (GetFlag(SF_ALBUMMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (CgViewerWidget->IsHidden() && ShowCgViewer) {
    CgViewerGroup->Hide();
    ShowCgViewer = false;
  }

  if (MenuTransition.IsOut() &&
      (ScrWork[SW_SYSMENUCT] == 0 || GetFlag(SF_SYSTEMMENU)) &&
      State == Hiding) {
    State = Hidden;
    Pages[CurrentPage]->Hide();
  } else if (MenuTransition.IsIn() && ScrWork[SW_SYSMENUCT] == 10000 &&
             State == Showing) {
    State = Shown;
    Pages[CurrentPage]->HasFocus = true;
    AlbumThumbnailButton::FocusedAlphaFadeStart();
  }

  if (State != Hidden) {
    ButtonGuideFade.Update(dt);
    MenuTransition.Update(dt);
    SelectAnimation.Update(dt);
    if (MenuTransition.Direction == AnimationDirection::Out &&
        MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == AnimationDirection::In ||
                TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    if (State == Shown) {
      UpdateInput(dt);
    }
    TitleFade.Update(dt);
    CommonMenu::UpdateTitles(AlbumMenuTitleRightPos, AlbumMenuTitleLeftPos);

    AlbumThumbnailButton::UpdateFocusedAlphaFade(dt);

    if (ShowCgViewer) {
      if (CgViewerGroup->VisibilityState == Hidden) CgViewerGroup->Show();
      CgViewerGroup->Update(dt);
    } else {
      auto button = static_cast<AlbumThumbnailButton*>(CurrentlyFocusedElement);
      if (button != nullptr && (button->Id / EntriesPerPage != CurrentPage)) {
        Pages[CurrentPage]->Hide();
        CurrentPage = button->Id / EntriesPerPage;
        Pages[CurrentPage]->Show();
        button->HasFocus = true;
        CurrentlyFocusedElement = button;
      }
      Pages[CurrentPage]->Update(dt);
    }
  }
}

void AlbumMenu::UpdatePages() {
  int totalVariations = 0;
  int viewedVariations = 0;
  int lastNonEmptyPage = 0;
  bool pageLocked = true;
  for (int i = 0; i <= AlbumPages * EntriesPerPage; i++) {
    SaveSystem::GetEVStatus(i, &totalVariations, &viewedVariations);
    static_cast<AlbumThumbnailButton*>(
        Pages[i / EntriesPerPage]->Children[i % EntriesPerPage])
        ->UpdateVariations(totalVariations, viewedVariations);

    if (viewedVariations > 0) pageLocked = false;
    if (i % 9 == 0 && pageLocked == false) lastNonEmptyPage = i / 9;
  }
  MaxReachablePage = lastNonEmptyPage;

  for (int i = 0; i < EntriesPerPage * MaxReachablePage; i++) {
    int nextChild = ((i + 1) + ((i + 1) % 3 == 0) * 6) %
                    (EntriesPerPage * MaxReachablePage);
    Pages[i / EntriesPerPage]->Children[i % EntriesPerPage]->SetFocus(
        Pages[nextChild / EntriesPerPage]->Children[nextChild % EntriesPerPage],
        FDIR_RIGHT);

    nextChild = (i - 1) - (i % 3 == 0) * 6;
    nextChild = (nextChild < 0) * EntriesPerPage * MaxReachablePage + nextChild;
    Pages[i / EntriesPerPage]->Children[i % EntriesPerPage]->SetFocus(
        Pages[nextChild / EntriesPerPage]->Children[nextChild % EntriesPerPage],
        FDIR_LEFT);

    nextChild =
        (i + 3) % EntriesPerPage + (i / EntriesPerPage) * EntriesPerPage;
    Pages[i / EntriesPerPage]->Children[i % EntriesPerPage]->SetFocus(
        Pages[i / EntriesPerPage]->Children[nextChild % EntriesPerPage],
        FDIR_DOWN);

    nextChild =
        (i - 3 < EntriesPerPage * (i / EntriesPerPage)) * EntriesPerPage + i -
        3;
    Pages[i / EntriesPerPage]->Children[i % EntriesPerPage]->SetFocus(
        Pages[i / EntriesPerPage]->Children[nextChild % EntriesPerPage],
        FDIR_UP);
  }
}

inline void AlbumMenu::DrawPage() {
  glm::vec2 offset = MenuTransition.GetPageOffset();
  Renderer->DrawSprite(PageCountLabel, PageLabelPosition + offset);
  Renderer->DrawSprite(PageNums[CurrentPage + 1], CurrentPageNumPos + offset);
  Renderer->DrawSprite(PageNumSeparatorSlash,
                       PageNumSeparatorSlashPos + offset);
  Renderer->DrawSprite(ReachablePageNums[MaxReachablePage],
                       MaxPageNumPos + offset);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      Renderer->DrawSprite(
          CGBox, glm::vec2(CGBoxTemplatePosition.x + ThumbnailOffset.x * i,
                           CGBoxTemplatePosition.y + ThumbnailOffset.y * j +
                               offset.y));
    }
  }

  Renderer->DrawSprite(CGList, CGListPosition + offset);
  Pages[CurrentPage]->MoveTo(offset);
  Pages[CurrentPage]->Render();
  SelectAnimation.Draw(SelectDataSprites, SelectDataPos, offset);
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto