#include "albummenu.h"

#include "../../profile/games/chlcc/albummenu.h"
#include "../../profile/scriptvars.h"
#include "../../profile/profile_internal.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../data/savesystem.h"
#include "../../ui/widgets/chlcc/albumthumbnailbutton.h"
#include "../../ui/widgets/group.h"
#include "../../background2d.h"

namespace Impacto {
namespace UI {
namespace CHLCC {
using namespace Impacto::Profile::CHLCC::AlbumMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

using namespace Impacto::Vm::Interface;
using namespace Impacto::UI::Widgets::CHLCC;

void AlbumMenu::OnCgVariationEnd(Widgets::CgViewer* target) {
  CgViewerGroup->Hide();
  ShowCgViewer = false;
}

void AlbumMenu::CgOnClick(Widgets::Button* target) {
  int total, viewed = 0;
  SaveSystem::GetEVStatus(target->Id, &total, &viewed);

  ShowCgViewer = true;
  CgViewerWidget->LoadCgSprites((size_t)target->Id, "bg",
                                Profile::SaveSystem::AlbumData[target->Id]);
}

AlbumMenu::AlbumMenu() {
  CurrentPage = 0;

  MenuTransition.Direction = AnimationDirection::In;
  MenuTransition.LoopMode = AnimationLoopMode::Stop;
  MenuTransition.SetDuration(MenuTransitionDuration);

  TitleFade.Direction = AnimationDirection::In;
  TitleFade.LoopMode = AnimationLoopMode::Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;

  CgViewerWidget = new Widgets::CgViewer();
  CgViewerWidget->OnVariationEndHandler = [this](auto* btn) {
    return OnCgVariationEnd(btn);
  };
  CgViewerGroup = new Group(this);
  CgViewerGroup->Add(CgViewerWidget, FDIR_DOWN);

  auto cgOnClick = [this](auto* btn) { return CgOnClick(btn); };

  for (int k = 0; k < AlbumPages; k++) {
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
    if (State != Showing) MenuTransition.StartIn();
    UpdatePages();
    Pages[CurrentPage]->Show();
    State = Showing;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}

void AlbumMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    AlbumThumbnailButton::FocusedAlphaFadeReset();
    MenuTransition.StartOut();
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
  if (State != Hidden) {
    if (MenuTransition.IsIn()) {
      Renderer->DrawQuad(RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                         RgbIntToFloat(BackgroundColor));
    } else {
      DrawCircles();
    }

    DrawErin();
    DrawRedBar();

    if (MenuTransition.Progress > 0.34f) {
      Renderer->DrawSprite(RedBarLabel, RedTitleLabelPos);

      const CornersQuad titleDest = AlbumMenuTitle.ScaledBounds()
                                        .RotateAroundCenter(AlbumMenuTitleAngle)
                                        .Translate(RightTitlePos);
      Renderer->DrawSprite(AlbumMenuTitle, titleDest);
    }

    Renderer->CaptureScreencap(ShaderScreencapture.BgSprite);
    Renderer->DrawCHLCCMenuBackground(
        ShaderScreencapture.BgSprite, BackgroundFilter,
        RectF(0.0f, 0.0f, 1280.0f, 720.0f), MenuTransition.Progress);

    if (MenuTransition.Progress > 0.34f) {
      Renderer->DrawSprite(AlbumMenuTitle, LeftTitlePos);
    }

    glm::vec2 offset(0.0f, 0.0f);
    if (MenuTransition.Progress > 0.22f) {
      if (MenuTransition.Progress < 0.72f) {
        // Approximated function from the original, another mess
        offset = glm::vec2(
            0.0f,
            glm::mix(-720.0f, 0.0f,
                     1.00397f * std::sin(3.97161f -
                                         3.26438f * MenuTransition.Progress) -
                         0.00295643f));
      }
      DrawPage(offset);

      for (int idx = 0; idx < 10; idx++) {
        Renderer->DrawSprite(SelectData[idx], SelectDataPos[idx] + offset);
      }

      CgViewerGroup->Render();
      DrawButtonGuide();
    }
  }
}

void AlbumMenu::UpdateInput(float dt) {
  Menu::UpdateInput(dt);
  if (State == Shown) {
    if (PADinputButtonWentDown & PAD1B || PADinputMouseWentDown & PAD1B) {
      if (CgViewerGroup->IsShown) {
        CgViewerGroup->Hide();
        ShowCgViewer = false;
      } else {
        SetFlag(SF_ALBUMEND, true);
      }
    }
  }
}

void AlbumMenu::Update(float dt) {
  UpdateInput(dt);
  if (ScrWork[SW_SYSMENUCT] < 10000 && State == Shown) {
    Hide();
  } else if (GetFlag(SF_ALBUMMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (MenuTransition.IsOut() && ScrWork[SW_SYSMENUCT] == 0 && State == Hiding) {
    Pages[CurrentPage]->Hide();
    State = Hidden;
  } else if (MenuTransition.IsIn() && ScrWork[SW_SYSMENUCT] == 10000 &&
             State == Showing) {
    State = Shown;
    AlbumThumbnailButton::FocusedAlphaFadeStart();
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    if (MenuTransition.Direction == +AnimationDirection::Out &&
        MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == +AnimationDirection::In ||
                TitleFade.IsOut())) {
      TitleFade.StartIn();
    }

    TitleFade.Update(dt);
    UpdateTitles();
    AlbumThumbnailButton::UpdateFocusedAlphaFade(dt);

    if (ShowCgViewer) {
      if (!CgViewerGroup->IsShown) CgViewerGroup->Show();
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

inline void AlbumMenu::DrawCircles() {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = MenuTransition.Progress * MenuTransitionDuration * 60.0f;
  for (int line = 0; line < 4; line++) {
    int counter = resetCounter;
    float x = CircleStartPosition.x;
    for (int col = 0; col < 7; col++) {
      if (counter + 1 <= (progress)) {
        float scale = ((progress) - (counter + 1.0f)) * 16.0f;
        scale = scale <= 320.0f ? scale : 320.0f;
        scale *= CircleSprite.Bounds.Height / 106.0f;
        Renderer->DrawSprite(
            CircleSprite, RectF(x + (CircleSprite.Bounds.Width - scale) / 2.0f,
                                y + (CircleSprite.Bounds.Height - scale) / 2.0f,
                                scale, scale));
        x += CircleOffset;
      }
      counter += 2;
    }
    y += CircleOffset;
    resetCounter += 2;
  }
}

inline void AlbumMenu::DrawErin() {
  float y = ErinPosition.y;
  if (MenuTransition.Progress < 0.78f) {
    y = 801.0f;
    if (MenuTransition.Progress > 0.22f) {
      // Approximation from the original function, which was a bigger mess
      y = glm::mix(
          -19.0f, 721.0f,
          0.998938f -
              0.998267f * sin(3.97835f - 3.27549f * MenuTransition.Progress));
    }
  }
  Renderer->DrawSprite(ErinSprite, glm::vec2(ErinPosition.x, y));
}

inline void AlbumMenu::DrawRedBar() {
  if (MenuTransition.IsIn()) {
    Renderer->DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (MenuTransition.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    float progress = MenuTransition.Progress * MenuTransitionDuration * 60.0f;
    float pixelPerAdvanceLeft = RedBarBaseX * (progress - 47.0f) / 17.0f;
    RedBarSprite.Bounds.X = RedBarDivision - pixelPerAdvanceLeft;
    RedBarSprite.Bounds.Width = pixelPerAdvanceLeft;
    RedBarPosition.x = RedBarBaseX - pixelPerAdvanceLeft;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
    float pixelPerAdvanceRight = 13.0f * (progress - 47.0f);
    RedBarSprite.Bounds.X = RedBarDivision;
    RedBarSprite.Bounds.Width = pixelPerAdvanceRight;
    RedBarPosition = RightRedBarPosition;
    Renderer->DrawSprite(RedBarSprite, RedBarPosition);
  }
}
void AlbumMenu::UpdatePages() {
  int totalVariations = 0;
  int viewedVariations = 0;
  int lastNonEmptyPage = 0;
  bool pageLocked = true;
  for (int idx = 0; idx < AlbumPages * EntriesPerPage; idx++) {
    SaveSystem::GetEVStatus(idx, &totalVariations, &viewedVariations);
    static_cast<AlbumThumbnailButton*>(
        Pages[idx / EntriesPerPage]->Children[idx % EntriesPerPage])
        ->UpdateVariations(totalVariations, viewedVariations);

    if (viewedVariations > 0) pageLocked = false;
    if (idx % 9 == 0 && pageLocked == false) lastNonEmptyPage = idx / 9;
  }
  MaxReachablePage = lastNonEmptyPage;

  for (int idx = 0; idx < EntriesPerPage * MaxReachablePage; idx++) {
    int nextChild = ((idx + 1) + ((idx + 1) % 3 == 0) * 6) %
                    (EntriesPerPage * MaxReachablePage);
    Pages[idx / EntriesPerPage]->Children[idx % EntriesPerPage]->SetFocus(
        Pages[nextChild / EntriesPerPage]->Children[nextChild % EntriesPerPage],
        FDIR_RIGHT);

    nextChild = (idx - 1) - (idx % 3 == 0) * 6;
    nextChild = (nextChild < 0) * EntriesPerPage * MaxReachablePage + nextChild;
    Pages[idx / EntriesPerPage]->Children[idx % EntriesPerPage]->SetFocus(
        Pages[nextChild / EntriesPerPage]->Children[nextChild % EntriesPerPage],
        FDIR_LEFT);

    nextChild =
        (idx + 3) % EntriesPerPage + (idx / EntriesPerPage) * EntriesPerPage;
    Pages[idx / EntriesPerPage]->Children[idx % EntriesPerPage]->SetFocus(
        Pages[idx / EntriesPerPage]->Children[nextChild % EntriesPerPage],
        FDIR_DOWN);

    nextChild =
        (idx - 3 < EntriesPerPage * (idx / EntriesPerPage)) * EntriesPerPage +
        idx - 3;
    Pages[idx / EntriesPerPage]->Children[idx % EntriesPerPage]->SetFocus(
        Pages[idx / EntriesPerPage]->Children[nextChild % EntriesPerPage],
        FDIR_UP);
  }
}

inline void AlbumMenu::DrawPage(const glm::vec2& offset) {
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
}
void AlbumMenu::DrawButtonGuide() {
  if (MenuTransition.Progress > 0.734f) {
    Renderer->DrawSprite(
        ButtonGuide,
        glm::vec2(ButtonGuidePos.x + (1 - MenuTransition.Progress) * 2560,
                  ButtonGuidePos.y));
  }
  if (ShowCgViewer) {
    Renderer->DrawSprite(
        (CgViewerWidget->isOnLastVariation() ? CgViewerButtonGuideNoVariation
                                             : CgViewerButtonGuideVariation),
        CgViewerButtonGuidePos);
  }
}

void AlbumMenu::UpdateTitles() {
  if (MenuTransition.Progress <= 0.34f) return;

  RedTitleLabelPos = RedBarLabelPosition;
  RightTitlePos = AlbumMenuTitleRightPos;
  LeftTitlePos = glm::vec2(
      AlbumMenuTitleLeftPos.x,
      TitleFade.IsIn()
          ? AlbumMenuTitleLeftPos.y
          : glm::mix(
                1.0f, 721.0f,
                1.01011f * std::sin(1.62223f * TitleFade.Progress + 3.152f) +
                    1.01012f));

  if (MenuTransition.Progress >= 0.73f) return;

  RedTitleLabelPos +=
      glm::vec2(-572.0f * (MenuTransition.Progress * 4.0f - 3.0f),
                460.0f * (MenuTransition.Progress * 4.0f - 3.0f) / 3.0f);
  RightTitlePos +=
      glm::vec2(-572.0f * (MenuTransition.Progress * 4.0f - 3.0f),
                460.0f * (MenuTransition.Progress * 4.0f - 3.0f) / 3.0f);
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto