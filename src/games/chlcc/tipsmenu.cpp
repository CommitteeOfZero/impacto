#include "tipsmenu.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../profile/dialogue.h"
#include "../../profile/ui/tipsmenu.h"
#include "../../profile/games/chlcc/tipsmenu.h"
#include "../../data/tipssystem.h"
#include "../../vm/interface/input.h"
#include "../../profile/game.h"
#include "../../profile/charset.h"
#include "../../ui/widgets/chlcc/tipsentrybutton.h"
#include <numeric>

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::TipsMenu;
using namespace Impacto::Profile::CHLCC::TipsMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;
using namespace Impacto::UI::Widgets::CHLCC;

static float GetEndScroll(Group *tipsItemsGroup) {
  if (tipsItemsGroup->Children.empty() || tipsItemsGroup->Children.size() == 1)
    return 0.0f;
  const Widget *lastItem = tipsItemsGroup->Children.back();
  const float lastItemEndPos = lastItem->Bounds.Height + lastItem->Bounds.Y;
  const float pagePos =
      lastItemEndPos - TipsListBounds.Height - TipsListBounds.Y;

  return ((int)(pagePos / TipListYPadding) * TipListYPadding);
}

void TipsMenu::HandlePageChange(Widget *cur, Widget *next) {
  if (cur != next) {
    static_cast<Group *>(cur)->MoveTo(TipsListBounds.GetPos());
    cur->Hide();
    next->Show();
    TipsEntryScrollPos = 0.0f;
    TipsEntriesScrollbar = Scrollbar(
        0, {TipsListBounds.X + TipsListBounds.Width, TipsListBounds.Y}, 0.0f,
        GetEndScroll(static_cast<Group *>(next)), &TipsEntryScrollPos,
        SBDIR_VERTICAL, TipsScrollTrack, TipsScrollThumb, {0.0f, 0.0f},
        TipsScrollThumb.ScaledHeight(), TipsListBounds);
  }
}

TipsMenu::TipsMenu()
    : ItemsList(
          Widgets::CarouselDirection::CDIR_HORIZONTAL,
          [this](Widget *cur, Widget *next) { HandlePageChange(cur, next); },
          [this](Widget *cur, Widget *next) { HandlePageChange(cur, next); }),
      TipViewItems(this) {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.SetDuration(MenuTransitionDuration);

  TitleFade.Direction = AnimationDirection::In;
  TitleFade.LoopMode = AnimationLoopMode::Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  FromSystemMenuTransition.Direction = AnimationDirection::In;
  FromSystemMenuTransition.LoopMode = AnimationLoopMode::Stop;
  FromSystemMenuTransition.DurationIn = TitleFadeInDuration;
  FromSystemMenuTransition.DurationOut = TitleFadeOutDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;

  TipViewItems.FocusLock = false;

  TextPage.Glyphs.reserve(Profile::Dialogue::MaxPageSize);
  TextPage.Clear();
  TextPage.Mode = DPM_TIPS;
  TextPage.FadeAnimation.Progress = 1.0f;
}

void TipsMenu::Show() {
  if (State != Shown) {
    if (State != Showing) {
      FadeAnimation.StartIn();
      FromSystemMenuTransition.StartIn();
    }
    State = Showing;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    UI::FocusedMenu = this;
    AnimationOffset = {0.0f, 0.0f};
    TipsEntryScrollPos = 0.0f;
    (*ItemsList.GetCurrent())->Show();
    TipViewItems.Show();
    if (TipsEntriesScrollbar) {
      TipsEntriesScrollbar->MoveTo(
          {TipsListBounds.X + TipsListBounds.Width, TipsListBounds.Y});
    }
  }
}
void TipsMenu::Hide() {
  if (State != Hidden) {
    if (State != Hiding) {
      FadeAnimation.StartOut();
      FromSystemMenuTransition.StartOut();
    }
    State = Hiding;
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void TipsMenu::Render() {
  if (State == Hidden) return;
  if (FadeAnimation.IsIn()) {
    Renderer->DrawQuad(
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        RgbIntToFloat(BackgroundColor));
  } else if (GetFlag(SF_SYSTEMMENU)) {
    Renderer->DrawQuad(
        RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
        RgbIntToFloat(BackgroundColor, FromSystemMenuTransition.Progress));
  } else {
    DrawCircles();
  }
  DrawErin();
  DrawRedBar();

  if (FadeAnimation.Progress > 0.34f) {
    Renderer->DrawSprite(RedBarLabel, RedTitleLabelPos);

    const CornersQuad titleDest = MenuTitleText.ScaledBounds()
                                      .RotateAroundCenter(MenuTitleTextAngle)
                                      .Translate(RightTitlePos);
    Renderer->DrawSprite(MenuTitleText, titleDest);
  }

  glm::vec3 tint = {1.0f, 1.0f, 1.0f};
  // Alpha goes from 0 to 1 in half the time
  float alpha =
      FadeAnimation.Progress < 0.5f ? FadeAnimation.Progress * 2.0f : 1.0f;
  Renderer->DrawSprite(
      BackgroundFilter,
      RectF(0.0f, 0.0f, Profile::DesignWidth, Profile::DesignHeight),
      glm::vec4(tint, alpha));
  if (FadeAnimation.State != AnimationState::Stopped) {
    Group *currentPage = static_cast<Group *>(*ItemsList.GetCurrent());
    const glm::vec2 pgOffset = [&] {
      glm::vec2 fOffset = TipsListBounds.GetPos() + AnimationOffset;
      return glm::vec2{std::round(fOffset.x),
                       std::round(fOffset.y - TipsEntryScrollPos)};
    }();
    const glm::vec2 renderBoundsOffset =
        TipsListRenderBounds.GetPos() + AnimationOffset;

    currentPage->MoveTo(pgOffset);
    currentPage->RenderingBounds.X = std::round(renderBoundsOffset.x);
    currentPage->RenderingBounds.Y = std::round(renderBoundsOffset.y);
    TipViewItems.MoveTo(AnimationOffset);
    TextPage.MoveTo(Profile::Dialogue::TipsBounds.GetPos() + AnimationOffset);
    if (TipsEntriesScrollbar) {
      const glm::vec2 scrollbarOffset =
          AnimationOffset +
          glm::vec2{TipsListBounds.X + TipsListBounds.Width, TipsListBounds.Y};
      TipsEntriesScrollbar->MoveTo(scrollbarOffset);
    }
  }

  DrawTipsTree();

  if (FadeAnimation.Progress > 0.34) {
    Renderer->DrawSprite(MenuTitleText, LeftTitlePos);
    ItemsList.Tint.a = alpha;
    ItemsList.Render();
    if (CurrentlyDisplayedTipId != -1) {
      TipViewItems.Tint.a = alpha;
      TipViewItems.Render();
      Renderer->DrawProcessedText(TextPage.Glyphs,
                                  Profile::Dialogue::DialogueFont, alpha,
                                  RendererOutlineMode::Full, true);
    }
    if (TipsEntriesScrollbar) {
      TipsEntriesScrollbar->Tint.a = alpha;
      TipsEntriesScrollbar->Render();
    }
  }

  DrawButtonPrompt();
}

void TipsMenu::UpdateInput(float dt) {
  if (State == Shown) {
    ItemsList.UpdateInput(dt);
    float oldScrollPos = TipsEntryScrollPos;
    UpdatePageInput(dt);
    if (TipsEntriesScrollbar) {
      TipsEntriesScrollbar->UpdateInput(dt);
      if (oldScrollPos != TipsEntryScrollPos) {
        float delta = oldScrollPos - TipsEntryScrollPos;
        auto *curPage = static_cast<Group *>(*ItemsList.GetCurrent());
        curPage->Move({0, delta});
      }
    }
    if (CurrentlyDisplayedTipId != -1) {
      if (PADinputButtonWentDown & PAD1X) {
        NextTipPage();
      }
    }
  }
}

void TipsMenu::Update(float dt) {
  if ((!GetFlag(SF_TIPSMENU) || ScrWork[SW_SYSMENUCT] < 10000) &&
      State == Shown) {
    Hide();
  } else if (GetFlag(SF_TIPSMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (FadeAnimation.IsOut() &&
      (ScrWork[SW_SYSMENUCT] == 0 || GetFlag(SF_SYSTEMMENU)) &&
      State == Hiding) {
    State = Hidden;
    (*ItemsList.GetCurrent())->Hide();
    TipViewItems.Hide();
    CurrentlyDisplayedTipId = -1;
    if (LastFocusedMenu) LastFocusedMenu->IsFocused = true;
  } else if (FadeAnimation.IsIn() && ScrWork[SW_SYSMENUCT] == 10000 &&
             State == Showing) {
    State = Shown;
    IsFocused = true;
    ItemsList.Children[0]->HasFocus = true;
    AdvanceFocus(FDIR_DOWN);
  }

  if (State != Hidden) {
    FadeAnimation.Update(dt);
    FromSystemMenuTransition.Update(dt);
    if (FadeAnimation.Direction == AnimationDirection::Out &&
        FadeAnimation.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (FadeAnimation.IsIn() &&
               (TitleFade.Direction == AnimationDirection::In ||
                TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
    UpdateTitles();
  }

  if (State == Shown) {
    UpdateInput(dt);
    ItemsList.Update(dt);
    TipViewItems.Update(dt);
  }

  if (TipsEntriesScrollbar) {
    TipsEntriesScrollbar->Update(dt);
  }

  if (FadeAnimation.Progress > 0.22f) {
    if (FadeAnimation.Progress < 0.72f) {
      // Approximated function from the original, another mess
      AnimationOffset = glm::vec2(
          0.0f,
          glm::mix(-720.0f, 0.0f,
                   1.00397f * std::sin(3.97161f -
                                       3.26438f * FadeAnimation.Progress) -
                       0.00295643f));
    } else {
      AnimationOffset = {0.0f, 0.0f};
    }
  } else {
    AnimationOffset = {0.0f, -720.0f};
  }
}

void TipsMenu::TipOnClick(Button *target) {
  auto tipEntry = static_cast<TipsEntryButton *>(target);
  if (!tipEntry->TipEntryRecord->IsLocked)
    SwitchToTipId(tipEntry->TipEntryRecord->Id);
}

void TipsMenu::Init() {
  auto onClick = [this](auto *btn) { return TipOnClick(btn); };
  int currentCategoryId = -1;

  // String of characters by which tips are sorted, taken from _system script
  auto [scriptBufId, catStrAddr] =
      Vm::ScriptGetTextTableStrAddress(TipsStringTable, CategoryStringIndex);
  uint8_t *categoryString = &Vm::ScriptBuffers[scriptBufId][catStrAddr];
  const auto recordCount = TipsSystem::GetTipCount();
  ItemsList.Clear();
  TipViewItems.Clear();
  Group *allTipsGroup = new Group(this);
  allTipsGroup->Bounds = TipsListBounds;
  allTipsGroup->RenderingBounds = TipsListRenderBounds;
  // Add a bit of margins

  ItemsList.Add(allTipsGroup);
  TipsEntryScrollPos = 0;
  // Sorting tip records
  std::vector<int> indexes(recordCount);
  auto &records = *TipsSystem::GetTipRecords();
  std::iota(indexes.begin(), indexes.end(), 0);
  std::ranges::sort(
      indexes, TipsSystem::TipsSorter(TipsStringTable, SortStringIndex, 2));
  auto sortedView = std::ranges::views::transform(
      indexes, [&records](size_t i) -> TipsSystem::TipsDataRecord & {
        return records[i];
      });
  {
    int i = 0;
    float currentY = TipListEntryBounds.Y;
    for (auto &record : sortedView) {
      //  Each category is a character from the sort string and contains all
      //  tips the names of which begin with that character

      // Start new category
      // We take a character from the sort string and use that as the
      // category name inside a predefined template
      if (record.CategoryLetterIndex != currentCategoryId) {
        currentCategoryId = record.CategoryLetterIndex;
        std::ranges::copy(CategoryString,
                          std::back_inserter(CategoryStringBuffer));
        if (currentCategoryId != std::numeric_limits<uint16_t>::max()) {
          CategoryStringBuffer[1] = UnalignedRead<uint16_t>(
              &categoryString[currentCategoryId * sizeof(uint16_t)]);
        }
        Label *categoryLabel = new Label();
        Vm::Sc3Stream categoryStrStream(
            reinterpret_cast<uint8_t *>(CategoryStringBuffer.data()));
        categoryLabel->Bounds.X = TipListEntryBounds.X;
        categoryLabel->Bounds.Y = currentY;
        categoryLabel->Bounds.X += 5;
        categoryLabel->SetText(categoryStrStream, TipListEntryFontSize,
                               RendererOutlineMode::Full, 0);
        categoryLabel->Bounds.X -= 5;
        allTipsGroup->Add(categoryLabel);
        currentY += TipListYPadding;
      }

      // Actual tip entry button
      RectF bounds = TipListEntryBounds;
      bounds.Y = currentY;
      TipsEntryButton *button =
          new TipsEntryButton(i++, &record, bounds, TipsEntryHighlightBar);
      button->OnClickHandler = onClick;

      allTipsGroup->Add(button, FDIR_DOWN);
      currentY += TipListYPadding;
    }
    allTipsGroup->Show();
    allTipsGroup->HasFocus = false;
  }
  {
    int i = 0;
    float currentY = TipListEntryBounds.Y;

    Group *newTipsGroup = new Group(this);
    Label *categoryLabel = new Label();
    categoryLabel->Bounds.X = TipListEntryBounds.X;
    categoryLabel->Bounds.Y = currentY;
    categoryLabel->Bounds.X += 5;
    categoryLabel->SetText(
        Vm::ScriptGetTextTableStrAddress(TipsStringTable, NewLabelStrIndex),
        TipListEntryFontSize, RendererOutlineMode::Full, 0);
    categoryLabel->Bounds.X -= 5;
    newTipsGroup->Add(categoryLabel);
    currentY += TipListYPadding;
    // I think new tips uses order tips came in
    for (auto &record : records) {
      if (!record.IsNew || record.IsLocked) {
        i++;
        continue;
      }
      RectF bounds = TipListEntryBounds;
      bounds.Y = currentY;
      TipsEntryButton *button =
          new TipsEntryButton(i++, &record, bounds, TipsEntryHighlightBar);
      button->OnClickHandler = onClick;

      newTipsGroup->Add(button, FDIR_DOWN);
      currentY += TipListYPadding;
    }
    ItemsList.Add(newTipsGroup);
    newTipsGroup->Bounds = TipsListBounds;
    newTipsGroup->RenderingBounds = TipsListRenderBounds;
  }

  {
    int i = 0;
    float currentY = TipListEntryBounds.Y;

    Group *unreadTipsGroup = new Group(this);
    Label *categoryLabel = new Label();
    categoryLabel->Bounds.X = TipListEntryBounds.X;
    categoryLabel->Bounds.Y = currentY;

    categoryLabel->Bounds.X += 5;
    categoryLabel->SetText(
        Vm::ScriptGetTextTableStrAddress(TipsStringTable, UnreadLabelStrIndex),
        TipListEntryFontSize, RendererOutlineMode::Full, 0);
    categoryLabel->Bounds.X -= 5;
    unreadTipsGroup->Add(categoryLabel);
    currentY += TipListYPadding;
    for (auto &record : sortedView) {
      if (!record.IsUnread || record.IsLocked) {
        i++;
        continue;
      }
      RectF bounds = TipListEntryBounds;
      bounds.Y = currentY;
      TipsEntryButton *button =
          new TipsEntryButton(i++, &record, bounds, TipsEntryHighlightBar);
      button->OnClickHandler = onClick;

      unreadTipsGroup->Add(button, FDIR_DOWN);
      currentY += TipListYPadding;
    }
    ItemsList.Add(unreadTipsGroup);
    unreadTipsGroup->Bounds = TipsListBounds;
    unreadTipsGroup->RenderingBounds = TipsListRenderBounds;
  }

  TipsEntriesScrollbar =
      Scrollbar(0, {TipsListBounds.X + TipsListBounds.Width, TipsListBounds.Y},
                0.0f, GetEndScroll(allTipsGroup), &TipsEntryScrollPos,
                SBDIR_VERTICAL, TipsScrollTrack, TipsScrollThumb, {0.0f, 0.0f},
                TipsScrollThumb.ScaledHeight(), TipsListBounds);

  Name = new Label();
  Name->Bounds = NameInitialBounds;
  TipViewItems.Add(Name);

  Pronounciation = new Label();
  Pronounciation->Bounds = PronounciationInitialBounds;
  TipViewItems.Add(Pronounciation);

  // Number label
  NumberText = new Label(
      Vm::ScriptGetTextTableStrAddress(TipsStringTable, NumberLabelStrIndex),
      NumberLabelPosition, NumberLabelFontSize, RendererOutlineMode::Full,
      DefaultColorIndex);
  TipViewItems.Add(NumberText);
  // Tip number
  Number = new Label();
  Number->Bounds = NumberBounds;
  TipViewItems.Add(Number);
  // Tip page separator
  PageSeparator = new Label(
      Vm::ScriptGetTextTableStrAddress(TipsStringTable, PageSeparatorIndex),
      PageSeparatorPosition, PageSeparatorFontSize, RendererOutlineMode::Full,
      DefaultColorIndex);
  TipViewItems.Add(PageSeparator);
  // Current tip page
  CurrentPage = new Label();
  CurrentPage->Bounds = CurrentPageBounds;
  TipViewItems.Add(CurrentPage);
  // Total tip pages
  TotalPages = new Label();
  TotalPages->Bounds = TotalPagesBounds;
  TipViewItems.Add(TotalPages);
}

void TipsMenu::DrawCircles() {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = FadeAnimation.Progress * MenuTransitionDuration * 60.0f;
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

void TipsMenu::DrawErin() {
  float y = ErinPosition.y;
  if (FadeAnimation.Progress < 0.78f) {
    y = 801.0f;
    if (FadeAnimation.Progress > 0.22f) {
      // Approximation from the original function, which was a bigger mess
      y = glm::mix(
          -19.0f, 721.0f,
          0.998938f -
              0.998267f * sin(3.97835f - 3.27549f * FadeAnimation.Progress));
    }
  }
  Renderer->DrawSprite(ErinSprite, glm::vec2(ErinPosition.x, y));
}

void TipsMenu::DrawRedBar() {
  if (FadeAnimation.IsIn()) {
    Renderer->DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (FadeAnimation.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    float progress = FadeAnimation.Progress * MenuTransitionDuration * 60.0f;
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

void TipsMenu::UpdatePageInput(float dt) {
  using namespace Vm::Interface;
  if (IsFocused) {
    auto prevEntry = CurrentlyFocusedElement;
    TipsEntriesScrollbar->UpdateInput(dt);

    auto checkScrollBounds = [&]() {
      return !TipsListBounds.Contains(CurrentlyFocusedElement->Bounds);
    };

    const uint32_t btnUp = PADcustom[28];
    const uint32_t btnDown = PADcustom[29];

    const int directionShouldFire = PADinputButtonWentDown & (btnUp | btnDown);
    const bool directionMovement = ((bool)(directionShouldFire & btnUp) ^
                                    (bool)(directionShouldFire & btnDown));

    auto *curPage = static_cast<Group *>(*ItemsList.GetCurrent());

    if (directionMovement) {
      if (directionShouldFire & btnDown) {
        AdvanceFocus(FDIR_DOWN);
      } else {
        AdvanceFocus(FDIR_UP);
      }

      if (CurrentlyFocusedElement != prevEntry && checkScrollBounds())
        if (CurrentlyFocusedElement == curPage->GetFirstFocusableChild()) {
          TipsEntryScrollPos = 0;
        } else if (CurrentlyFocusedElement == curPage->Children.back()) {
          TipsEntryScrollPos = TipsEntriesScrollbar->EndValue;
        } else {
          TipsEntryScrollPos +=
              CurrentlyFocusedElement->Bounds.Y - prevEntry->Bounds.Y;
        }
    }
  }
}

void TipsMenu::DrawTipsTree() {
  glm::vec2 currentTipBackgroundPosition(
      CurrentTipBackgroundPosition.x,
      CurrentTipBackgroundPosition.y + AnimationOffset.y);
  Renderer->DrawSprite(CurrentTipBackgroundSprite,
                       currentTipBackgroundPosition);
  glm::vec2 gradientPosition(GradientPosition.x,
                             GradientPosition.y + AnimationOffset.y);
  Renderer->DrawSprite(TipsGradient, gradientPosition);
  Renderer->DrawQuad(
      RectF(GradientPosition.x,
            TipsListBounds.Y + TipsListBounds.Height + AnimationOffset.y,
            TipsGradient.Bounds.Width, 86.0f),
      RgbIntToFloat(EndOfGradientColor));
  glm::vec2 treePosition(TreePosition.x, TreePosition.y + AnimationOffset.y);
  Renderer->DrawSprite(TipsTree, treePosition);

  const auto *const currentPage =
      static_cast<Widgets::Group *>(*ItemsList.GetCurrent());
  size_t i = 0;
  for (const auto *const widget : currentPage->Children) {
    const glm::vec2 pos = widget->Bounds.GetPos();
    const glm::vec2 linePos = pos + glm::vec2{-22, 0};
    const bool isLastItem = pos.y > AnimationOffset.y + TipsListBounds.Y +
                                        TipsListBounds.Height -
                                        TipListYPadding &&
                            pos.y < AnimationOffset.y + TipsListBounds.Y +
                                        TipsListBounds.Height - TipListYPadding;

    std::reference_wrapper<const Sprite> barSprite = TipsListBgBar;
    std::reference_wrapper<const Sprite> lineSprite = TipsLeftLine;

    if (const auto *const category = dynamic_cast<const Label *>(widget)) {
      if (isLastItem) {
        lineSprite = TipsLeftLineEnd;
      }
    } else if (const auto *const btn =
                   dynamic_cast<const TipsEntryButton *>(widget)) {
      barSprite = TipsListBgBarHole;
      if (isLastItem) {
        lineSprite = (btn == currentPage->Children.back()) ? TipsLeftLineHoleEnd
                                                           : TipsLeftLineEnd;
      } else {
        lineSprite = TipsLeftLineHole;
      }
    } else {
      throw std::invalid_argument("Invalid widget type for tip items");
    }

    Renderer->EnableScissor();
    Renderer->SetScissorRect(currentPage->RenderingBounds);
    Renderer->DrawSprite(barSprite, pos);
    Renderer->DrawSprite(lineSprite, linePos);
    Renderer->DisableScissor();
    ++i;
  }
  // Fill in the rest of the bg if tips is less than full page
  if (i * TipListYPadding < TipsListBounds.Height) {
    const float remainder = TipsListBounds.Height - (i * TipListYPadding);
    const float start =
        TipsListBounds.Y + AnimationOffset.y + (i * TipListYPadding);
    RectF dest{
        GradientPosition.x + AnimationOffset.x,
        start,
        TipsGradient.Bounds.Width,
        remainder,
    };
    Renderer->DrawQuad(dest, RgbIntToFloat(EndOfGradientColor));
    for (float j = start; j < remainder + start; j += TipListYPadding) {
      const glm::vec2 pos = {TipsListBounds.X + AnimationOffset.x - 22.0f, j};
      Renderer->DrawSprite(TipsLeftLine, pos);
    }
  }
}

void TipsMenu::DrawButtonPrompt() {
  if (FadeAnimation.IsIn()) {
    Renderer->DrawSprite(ButtonPromptSprite, ButtonPromptPosition);
  } else if (FadeAnimation.Progress > 0.734f) {
    float x = ButtonPromptPosition.x - 2560.0f * (FadeAnimation.Progress - 1);
    Renderer->DrawSprite(ButtonPromptSprite,
                         glm::vec2(x, ButtonPromptPosition.y));
  }
}

void TipsMenu::UpdateTitles() {
  if (FadeAnimation.Progress <= 0.34f) return;

  RedTitleLabelPos = RedBarLabelPosition;
  RightTitlePos = MenuTitleTextRightPosition;
  LeftTitlePos = glm::vec2(
      MenuTitleTextLeftPosition.x,
      TitleFade.IsIn()
          ? MenuTitleTextLeftPosition.y
          : glm::mix(
                1.0f, 721.0f,
                1.01011f * std::sin(1.62223f * TitleFade.Progress + 3.152f) +
                    1.01012f));

  if (FadeAnimation.Progress >= 0.73f) return;

  RedTitleLabelPos +=
      glm::vec2(-572.0f * (FadeAnimation.Progress * 4.0f - 3.0f),
                460.0f * (FadeAnimation.Progress * 4.0f - 3.0f) / 3.0f);
  RightTitlePos +=
      glm::vec2(-572.0f * (FadeAnimation.Progress * 4.0f - 3.0f),
                460.0f * (FadeAnimation.Progress * 4.0f - 3.0f) / 3.0f);
}

void TipsMenu::SwitchToTipId(int id) {
  CurrentlyDisplayedTipId = id;
  CurrentTipPage = 1;
  TipsSystem::SetTipUnreadState(id, false);
  TipsSystem::SetTipNewState(id, false);
  auto tipsScriptBufferId = TipsSystem::GetTipsScriptBufferId();

  auto tipRecord = TipsSystem::GetTipRecord(id);
  Name->SetText({.ScriptBufferId = tipsScriptBufferId,
                 .IpOffset = tipRecord->StringAdr[0]},
                NameFontSize, RendererOutlineMode::Full, DefaultColorIndex);
  Pronounciation->SetText({.ScriptBufferId = tipsScriptBufferId,
                           .IpOffset = tipRecord->StringAdr[1]},
                          PronounciationFontSize, RendererOutlineMode::Full,
                          DefaultColorIndex);
  // Right alignment
  Name->MoveTo(NameInitialBounds.GetPos() -
               glm::vec2{Name->Bounds.Width, 0.0f});
  Pronounciation->MoveTo(PronounciationInitialBounds.GetPos() -
                         glm::vec2{Pronounciation->Bounds.Width, 0.0f});

  Number->SetText(fmt::format("{:4d}", tipRecord->Id + 1), NumberFontSize,
                  RendererOutlineMode::Full, DefaultColorIndex);

  CurrentPage->SetText(fmt::to_string(CurrentTipPage), PageSeparatorFontSize,
                       RendererOutlineMode::Full, DefaultColorIndex);

  TotalPages->SetText(fmt::to_string(tipRecord->NumberOfContentStrings),
                      PageSeparatorFontSize, RendererOutlineMode::Full,
                      DefaultColorIndex);

  TextPage.Clear();
  Vm::Sc3VmThread dummy;
  dummy.IpOffset = tipRecord->StringAdr[3];
  dummy.ScriptBufferId = tipsScriptBufferId;
  TextPage.AddString(&dummy);
}

void TipsMenu::NextTipPage() {
  CurrentTipPage += 1;
  auto currentRecord = TipsSystem::GetTipRecord(CurrentlyDisplayedTipId);
  if (CurrentTipPage > currentRecord->NumberOfContentStrings)
    CurrentTipPage = 1;

  TextPage.Clear();
  Vm::Sc3VmThread dummy;
  dummy.IpOffset = currentRecord->StringAdr[3 + CurrentTipPage - 1];
  dummy.ScriptBufferId = TipsSystem::GetTipsScriptBufferId();
  TextPage.AddString(&dummy);
  CurrentPage->SetText(fmt::to_string(CurrentTipPage), PageSeparatorFontSize,
                       RendererOutlineMode::Full, DefaultColorIndex);
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto