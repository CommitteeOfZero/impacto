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

static float GetEndScroll(Group* tipsItemsGroup) {
  if (tipsItemsGroup->Children.empty() || tipsItemsGroup->Children.size() == 1)
    return 0.0f;
  const Widget* lastItem = tipsItemsGroup->Children.back();
  const float lastItemEndPos = lastItem->Bounds.Height + lastItem->Bounds.Y;
  const float pagePos =
      lastItemEndPos - TipsListBounds.Height - TipsListBounds.Y;
  if (pagePos < 0.0f) return 0.0f;
  return ((int)(pagePos / TipListYPadding) * TipListYPadding);
}

void TipsMenu::HandlePageChange(Widget* cur, Widget* next) {
  if (cur != next) {
    static_cast<Group*>(cur)->MoveTo(TipsListBounds.GetPos());
    cur->Hide();
    next->Show();
    CurrentlyFocusedElement =
        static_cast<Group*>(next)->GetFirstFocusableChild();
    CurrentlyFocusedElement->HasFocus = true;
    TipsEntryScrollPos = 0.0f;
    TipsEntriesScrollbar = Scrollbar(
        0, {TipsListBounds.X + TipsListBounds.Width - 3.0f, TipsListBounds.Y},
        0.0f, GetEndScroll(static_cast<Group*>(next)), &TipsEntryScrollPos,
        SBDIR_VERTICAL, TipsScrollTrack, TipsScrollThumb, {0.0f, -4.0f},
        TipsScrollThumb.ScaledHeight(), TipsListBounds);
    TipsEntriesScrollbar->Step = TipListYPadding;
  }
}

TipsMenu::TipsMenu()
    : ItemsList(
          Widgets::CarouselDirection::CDIR_HORIZONTAL,
          [this](Widget* cur, Widget* next) { HandlePageChange(cur, next); },
          [this](Widget* cur, Widget* next) { HandlePageChange(cur, next); }),
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

  SelectWordAnimation.Direction = AnimationDirection::In;
  SelectWordAnimation.LoopMode = AnimationLoopMode::Loop;
  SelectWordAnimation.DurationIn = SelectWordDuration;
  SelectWordAnimation.DurationOut = 0.0f;

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
      SelectWordAnimation.StartIn(true);
    }
    State = Showing;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    UI::FocusedMenu = this;
    AnimationOffset = {0.0f, 0.0f};
    TipsEntryScrollPos = 0.0f;
    if (ItemsList.Children.size() > 1) {
      ItemsList.Next();
    }
    (*ItemsList.GetCurrent())->Show();
    (*ItemsList.GetCurrent())->HasFocus = false;
    TipViewItems.Bounds = RectF{};
    TipViewItems.Show();
    if (TipsEntriesScrollbar) {
      TipsEntriesScrollbar->MoveTo(
          {TipsListBounds.X + TipsListBounds.Width - 3.0f, TipsListBounds.Y});
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
    Group* currentPage = static_cast<Group*>(*ItemsList.GetCurrent());
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
  DrawSelectWord();
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
    UpdatePageInput(dt);
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

    for (const auto tipNewId : TipsSystem::GetNewTipsIndices()) {
      TipsSystem::SetTipNewState(tipNewId, false);
    }
    TipsSystem::GetNewTipsIndices().resize(0);
    CurrentlyDisplayedTipId = -1;
    if (LastFocusedMenu) LastFocusedMenu->IsFocused = true;
  } else if (FadeAnimation.IsIn() && ScrWork[SW_SYSMENUCT] == 10000 &&
             State == Showing) {
    State = Shown;
    IsFocused = true;
    (*ItemsList.GetCurrent())->HasFocus = true;
    AdvanceFocus(FDIR_DOWN);
  }

  if (State != Hidden) {
    FadeAnimation.Update(dt);
    FromSystemMenuTransition.Update(dt);
    SelectWordAnimation.Update(dt);
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
          glm::mix(-Profile::DesignHeight, 0.0f,
                   1.00397f * std::sin(3.97161f -
                                       3.26438f * FadeAnimation.Progress) -
                       0.00295643f));
    } else {
      AnimationOffset = {0.0f, 0.0f};
    }
  } else {
    AnimationOffset = {0.0f, -Profile::DesignHeight};
  }
}

void TipsMenu::TipOnClick(Button* target) {
  auto tipEntry = static_cast<TipsEntryButton*>(target);
  if (!tipEntry->TipEntryRecord->IsLocked)
    SwitchToTipId(tipEntry->TipEntryRecord->Id);
}

void TipsMenu::Init() {
  auto onClick = [this](auto* btn) { return TipOnClick(btn); };
  int currentCategoryId = -1;

  // String of characters by which tips are sorted, taken from _system script
  auto [scriptBufId, catStrAddr] =
      Vm::ScriptGetTextTableStrAddress(TipsStringTable, CategoryStringIndex);
  uint8_t* categoryString = &Vm::ScriptBuffers[scriptBufId][catStrAddr];
  ItemsList.Clear();
  TipViewItems.Clear();
  Group* allTipsGroup = new Group(this);
  allTipsGroup->Bounds = TipsListBounds;
  allTipsGroup->RenderingBounds = TipsListRenderBounds;
  // Add a bit of margins

  ItemsList.Add(allTipsGroup);
  TipsEntryScrollPos = 0;
  // Sorting tip records
  auto& records = *TipsSystem::GetTipRecords();
  const auto recordCount = TipsSystem::GetTipCount();
  auto indexes = [recordCount] {
    std::vector<int> result(recordCount);
    std::iota(result.begin(), result.end(), 0);
    std::ranges::sort(result, TipsSystem::TipsComparator(TipsStringTable,
                                                         SortStringIndex, 2));
    return result;
  }();
  auto sortedView = std::ranges::views::transform(
      indexes, [&records](size_t i) -> TipsSystem::TipsDataRecord& {
        return records[i];
      });
  std::vector<int> sortedIndicesMap(recordCount);
  for (size_t i = 0; i < recordCount; i++) {
    sortedIndicesMap[i] = records[i].Id;
  }

  auto createCategory = [&](auto labelText, float yPos) {
    Label* categoryLabel = new Label();
    categoryLabel->Bounds.X = TipListEntryBounds.X;
    categoryLabel->Bounds.Y = yPos;
    categoryLabel->Bounds.X += 5;
    categoryLabel->SetText(labelText, TipListEntryFontSize,
                           RendererOutlineMode::Full, 0);
    categoryLabel->Bounds.X -= 5;
    return categoryLabel;
  };
  {
    float currentY = TipListEntryBounds.Y;
    for (auto& record : sortedView) {
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
        Vm::Sc3Stream categoryStrStream(
            reinterpret_cast<uint8_t*>(CategoryStringBuffer.data()));
        allTipsGroup->Add(createCategory(categoryStrStream, currentY));
        currentY += TipListYPadding;
      }

      // Actual tip entry button
      RectF bounds = TipListEntryBounds;
      bounds.Y = currentY;
      TipsEntryButton* button = new TipsEntryButton(
          sortedIndicesMap[record.Id], &record, bounds, TipsEntryHighlightBar);
      button->OnClickHandler = onClick;

      allTipsGroup->Add(button, FDIR_DOWN);
      currentY += TipListYPadding;
    }
  }
  {
    float currentY = TipListEntryBounds.Y;
    auto* newTipsGroup = new Group(this);
    newTipsGroup->Add(createCategory(
        Vm::ScriptGetTextTableStrAddress(TipsStringTable, NewLabelStrIndex),
        currentY));
    currentY += TipListYPadding;

    auto newRecordsView =
        std::ranges::views::transform(
            TipsSystem::GetNewTipsIndices(),
            [&records](size_t i) -> TipsSystem::TipsDataRecord& {
              return records[i];
            }) |
        std::views::reverse;
    for (auto& record : newRecordsView) {
      // assert(!record.IsLocked && record.IsNew);
      RectF bounds = TipListEntryBounds;
      bounds.Y = currentY;
      TipsEntryButton* button = new TipsEntryButton(
          sortedIndicesMap[record.Id], &record, bounds, TipsEntryHighlightBar);
      button->OnClickHandler = onClick;

      newTipsGroup->Add(button, FDIR_DOWN);
      currentY += TipListYPadding;
    }
    if (newTipsGroup->Children.size() > 1) {
      ItemsList.Add(newTipsGroup);
      newTipsGroup->Bounds = TipsListBounds;
      newTipsGroup->RenderingBounds = TipsListRenderBounds;
    } else {
      delete newTipsGroup;
      newTipsGroup = nullptr;
    }
  }

  {
    float currentY = TipListEntryBounds.Y;

    Group* unreadTipsGroup = new Group(this);
    unreadTipsGroup->Add(createCategory(
        Vm::ScriptGetTextTableStrAddress(TipsStringTable, UnreadLabelStrIndex),
        currentY));
    currentY += TipListYPadding;
    for (auto& record : sortedView) {
      if (!record.IsUnread || record.IsLocked) {
        continue;
      }
      RectF bounds = TipListEntryBounds;
      bounds.Y = currentY;
      TipsEntryButton* button = new TipsEntryButton(
          sortedIndicesMap[record.Id], &record, bounds, TipsEntryHighlightBar);
      button->OnClickHandler = onClick;

      unreadTipsGroup->Add(button, FDIR_DOWN);
      currentY += TipListYPadding;
    }
    if (unreadTipsGroup->Children.size() > 1) {
      ItemsList.Add(unreadTipsGroup);
      unreadTipsGroup->Bounds = TipsListBounds;
      unreadTipsGroup->RenderingBounds = TipsListRenderBounds;
    } else {
      delete unreadTipsGroup;
    }
  }

  TipsEntriesScrollbar =
      Scrollbar(0, {TipsListBounds.X + TipsListBounds.Width, TipsListBounds.Y},
                0.0f, GetEndScroll(allTipsGroup), &TipsEntryScrollPos,
                SBDIR_VERTICAL, TipsScrollTrack, TipsScrollThumb, {0.0f, 0.0f},
                TipsScrollThumb.ScaledHeight(), TipsListBounds);
  TipsEntriesScrollbar->Step = TipListYPadding;
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
  auto* const pageSeparator =
      new Label(PageSeparatorSprite, PageSeparatorPosition);
  TipViewItems.Add(pageSeparator);
  // Current tip page
  CurrentPage = new Label();
  CurrentPage->Bounds.SetPos(CurrentPagePosition);
  TipViewItems.Add(CurrentPage);
  // Total tip pages
  TotalPages = new Label();
  TotalPages->Bounds.SetPos(TotalPagesPosition);
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
          -19.0f, Profile::DesignHeight + 1.0f,
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
  if (!IsFocused) return;
  auto prevEntry = CurrentlyFocusedElement;
  const float oldScrollPos = TipsEntryScrollPos;

  auto checkScrollBounds = [&]() {
    return !TipsListBounds.Contains(CurrentlyFocusedElement->Bounds);
  };

  UI::TipsMenu::UpdateInput(dt);

  auto* curPage = static_cast<Group*>(*ItemsList.GetCurrent());

  if (CurrentlyFocusedElement != prevEntry && checkScrollBounds()) {
    if (CurrentlyFocusedElement == curPage->GetFirstFocusableChild()) {
      TipsEntryScrollPos = 0;
    } else if (CurrentlyFocusedElement == curPage->Children.back()) {
      TipsEntryScrollPos = TipsEntriesScrollbar->EndValue;
    } else {
      TipsEntryScrollPos +=
          CurrentlyFocusedElement->Bounds.Y - prevEntry->Bounds.Y;
    }
  }

  if (TipsEntriesScrollbar) {
    TipsEntriesScrollbar->UpdateInput(dt);
  }

  if (oldScrollPos != TipsEntryScrollPos) {
    float delta = oldScrollPos - TipsEntryScrollPos;
    if (std::fmod(std::abs(delta), TipListYPadding) >
        std::numeric_limits<float>::epsilon()) {
      const float newDelta =
          std::round(delta / TipListYPadding) * TipListYPadding;
      TipsEntryScrollPos = oldScrollPos - newDelta;
      delta = newDelta;
    }
    curPage->Move({0, delta});
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
            TipsGradient.Bounds.Width, 88.0f),
      RgbIntToFloat(EndOfGradientColor));
  glm::vec2 treePosition(TreePosition.x, TreePosition.y + AnimationOffset.y);
  Renderer->DrawSprite(TipsTree, treePosition);

  const auto* const currentPage =
      static_cast<Widgets::Group*>(*ItemsList.GetCurrent());
  size_t i = 0;
  for (const auto* const widget : currentPage->Children) {
    const glm::vec2 pos = widget->Bounds.GetPos();
    const glm::vec2 linePos = pos + glm::vec2{-22, 0};
    const float bottomEdge =
        AnimationOffset.y + TipsListBounds.Y + TipsListBounds.Height;
    const bool lastViewable = bottomEdge - pos.y < TipListYPadding + 1.0f &&
                              bottomEdge - pos.y > 0.0f;

    std::reference_wrapper<const Sprite> barSprite = TipsListBgBar;
    std::reference_wrapper<const Sprite> lineSprite = TipsLeftLine;
    if (const auto* const btn = dynamic_cast<const TipsEntryButton*>(widget)) {
      barSprite = TipsListBgBarHole;
      lineSprite = TipsLeftLineHole;
      if (btn == currentPage->Children.back()) {
        lineSprite = TipsLeftLineHoleEnd;
      }
    }

    Renderer->EnableScissor();
    Renderer->SetScissorRect(currentPage->RenderingBounds);
    Renderer->DrawSprite(barSprite, pos);
    Renderer->DrawSprite(lineSprite, linePos);
    Renderer->DisableScissor();
    if (lastViewable && widget != currentPage->Children.back())
      Renderer->DrawSprite(TipsLeftLineEnd,
                           linePos + glm::vec2{0, TipListYPadding});
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
        TipsListBounds.Width,
        remainder,
    };
    Renderer->DrawQuad(dest, RgbIntToFloat(EndOfGradientColor));
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

void TipsMenu::DrawSelectWord() {
  const auto currentLetter = static_cast<size_t>(
      SelectWordAnimation.Progress * SelectWordDuration / SelectWordInterval);
  for (size_t i = 0; i < SelectWordSprites.size(); i++) {
    if (currentLetter < i) break;
    const glm::vec2 pos = SelectWordPos[i];
    const float alpha =
        i == currentLetter
            ? (SelectWordAnimation.Progress * SelectWordDuration -
               i * SelectWordInterval) /
                  SelectWordInterval
            : 1.0f;
    if (i == currentLetter)
      ImpLog(LogLevel::Trace, LogChannel::Render, "alpha {}\n", alpha);
    Renderer->DrawSprite(SelectWordSprites[i], pos + AnimationOffset,
                         glm::vec4(1.0f, 1.0f, 1.0f, alpha));
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
                1.0f, Profile::DesignHeight + 1.0f,
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

  CurrentPage->SetSprite(CurrentPageSprites[CurrentTipPage]);
  TotalPages->SetSprite(TotalPageSprites[tipRecord->NumberOfContentStrings]);

  TextPage.Clear();
  Vm::Sc3VmThread dummy;
  dummy.IpOffset = tipRecord->StringAdr[3];
  dummy.ScriptBufferId = tipsScriptBufferId;
  TextPage.AddString(&dummy);
}

void TipsMenu::NextTipPage() {
  auto currentRecord = TipsSystem::GetTipRecord(CurrentlyDisplayedTipId);
  if (currentRecord->NumberOfContentStrings == 1) return;
  CurrentTipPage += 1;
  if (CurrentTipPage > currentRecord->NumberOfContentStrings)
    CurrentTipPage = 1;

  TextPage.Clear();
  Vm::Sc3VmThread dummy;
  dummy.IpOffset = currentRecord->StringAdr[3 + CurrentTipPage - 1];
  dummy.ScriptBufferId = TipsSystem::GetTipsScriptBufferId();
  TextPage.AddString(&dummy);
  CurrentPage->SetSprite(CurrentPageSprites[CurrentTipPage]);
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto