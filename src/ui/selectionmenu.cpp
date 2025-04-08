#include "selectionmenu.h"

#include "../profile/ui/selectionmenu.h"
#include "../profile/dialogue.h"
#include "../profile/game.h"
#include "../renderer/renderer.h"

namespace Impacto {
namespace UI {

using namespace Impacto::UI::Widgets;
using namespace Impacto::Profile::SelectionMenu;

void SelectionMenu::ChoiceItemOnClick(Button* target) {
  SelectedChoiceId = target->Id;
  ChoiceMade = true;
}

void SelectionMenu::Init(bool isPlain) {
  IsPlain = isPlain;
  ChoiceCount = 0;
  ChoiceMade = false;
  ChoiceHeight = 0.0f;
  ChoiceWidthMax = FLT_MIN;
  ChoiceXMin = FLT_MAX;

  FadeAnimation.DurationIn = FadeAnimationDurationInOut;
  FadeAnimation.DurationOut = FadeAnimationDurationInOut;
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
}

void SelectionMenu::AddChoice(uint8_t* str) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  Choices[ChoiceCount] = TextLayoutPlainLine(
      &dummy, 255, Profile::Dialogue::DialogueFont,
      Profile::Dialogue::DefaultFontSize, Profile::Dialogue::ColorTable[0],
      1.0f, glm::vec2(0.0f, 0.0f), TextAlignment::Left);

  float mesLen = 0.0f;
  for (int i = 0; i < Choices[ChoiceCount].size(); i++) {
    mesLen += Choices[ChoiceCount][i].DestRect.Width;
  }

  ChoiceWidths[ChoiceCount] = mesLen;
  ChoiceCount++;
}

void SelectionMenu::Show() {
  ChoiceItems = new Widgets::Group(this);

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  auto onClick = [this](auto* btn) { return ChoiceItemOnClick(btn); };

  if (IsPlain) {
    float diff = 0.0f;
    float choiceY = (Profile::DesignHeight -
                     (ChoiceCount * (Profile::Dialogue::DefaultFontSize +
                                     PlainSelectionYSpacing))) /
                    2.0f;

    for (int i = 0; i < ChoiceCount; i++) {
      if (ChoiceWidths[i] > ChoiceWidthMax) ChoiceWidthMax = ChoiceWidths[i];

      diff = (Profile::DesignWidth - ChoiceWidths[i]) / 2.0f;
      for (int j = 0; j < Choices[i].size(); j++) {
        Choices[i][j].DestRect.X += diff;
        Choices[i][j].DestRect.Y = choiceY;
      }

      if (Choices[i][0].DestRect.X < ChoiceXMin)
        ChoiceXMin = Choices[i][0].DestRect.X;

      ChoiceHeight +=
          Profile::Dialogue::DefaultFontSize + PlainSelectionYSpacing;
      choiceY += Profile::Dialogue::DefaultFontSize + PlainSelectionYSpacing;

      Button* choice = new Button(
          i, nullSprite, nullSprite, SelectionHighlight,
          glm::vec2(Choices[i][0].DestRect.X, Choices[i][0].DestRect.Y));

      choice->SetText(Choices[i], ChoiceWidths[i],
                      Profile::Dialogue::DefaultFontSize,
                      RendererOutlineMode::Full);
      choice->OnClickHandler = onClick;

      ChoiceItems->Add(choice, FDIR_DOWN);
    }
    ChoiceHeight -= PlainSelectionYSpacing;
  } else {
    float diff = 0.0f;
    float choiceY = 0.0f;

    CurrentSelBackgroundY = SelectionBackgroundY[ChoiceCount - 1];
    choiceY = SelectionBackgroundY[ChoiceCount - 1] +
              (SelectionBackground.Bounds.Height -
               Profile::Dialogue::DefaultFontSize) /
                  2.0f;

    for (int i = 0; i < ChoiceCount; i++) {
      diff = (Profile::DesignWidth - ChoiceWidths[i]) / 2.0f;
      for (int j = 0; j < Choices[i].size(); j++) {
        Choices[i][j].DestRect.X += diff;
        Choices[i][j].DestRect.Y = choiceY;
      }
      choiceY += SelectionYSpacing;

      Button* choice = new Button(
          i, nullSprite, SelectionFocused, SelectionHighlight,
          glm::vec2(Choices[i][0].DestRect.X, Choices[i][0].DestRect.Y));

      choice->SetText(Choices[i], ChoiceWidths[i],
                      Profile::Dialogue::DefaultFontSize,
                      RendererOutlineMode::Full);
      choice->OnClickHandler = onClick;
      if (!HighlightTextOnly) {
        choice->Bounds = RectF(SelectionBackgroundX,
                               CurrentSelBackgroundY + (i * SelectionYSpacing),
                               SelectionBackground.ScaledWidth(),
                               SelectionBackground.ScaledHeight());
      }

      ChoiceItems->Add(choice, FDIR_DOWN);
    }
  }

  ChoiceItems->Show();
  FadeAnimation.StartIn();
  State = Showing;
  Menu::Show();
}

void SelectionMenu::Hide() {
  FadeAnimation.StartOut();
  State = Hiding;
  memset(FocusStart, 0, sizeof(FocusStart));
  if (CurrentlyFocusedElement) CurrentlyFocusedElement->HasFocus = false;
  CurrentlyFocusedElement = 0;
  Menu::Hide();
}

void SelectionMenu::Update(float dt) {
  UpdateInput();
  FadeAnimation.Update(dt);
  if (State != Hidden) {
    if (FadeAnimation.IsIn()) State = Shown;
    if (FadeAnimation.IsOut()) State = Hidden;
    ChoiceItems->Update(dt);
  }
}
void SelectionMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    if (IsPlain) {
      Renderer->DrawSprite(
          PlainSelectionFrameTopLeft,
          glm::vec2(ChoiceXMin - PlainSelectionFrameTopLeft.Bounds.Width,
                    Choices[0][0].DestRect.Y -
                        PlainSelectionFrameTopLeft.Bounds.Height),
          col);
      Renderer->DrawSprite(
          PlainSelectionFrameTopSide,
          RectF(ChoiceXMin,
                Choices[0][0].DestRect.Y -
                    PlainSelectionFrameTopLeft.Bounds.Height,
                ChoiceWidthMax, PlainSelectionFrameTopLeft.Bounds.Height),
          col);
      Renderer->DrawSprite(
          PlainSelectionFrameTopRight,
          glm::vec2(ChoiceXMin + ChoiceWidthMax,
                    Choices[0][0].DestRect.Y -
                        PlainSelectionFrameTopLeft.Bounds.Height),
          col);
      Renderer->DrawSprite(
          PlainSelectionFrameLeftSide,
          RectF(ChoiceXMin - PlainSelectionFrameTopLeft.Bounds.Width,
                Choices[0][0].DestRect.Y,
                PlainSelectionFrameTopLeft.Bounds.Width, ChoiceHeight),
          col);
      Renderer->DrawSprite(
          PlainSelectionFrameRightSide,
          RectF(ChoiceXMin + ChoiceWidthMax, Choices[0][0].DestRect.Y,
                PlainSelectionFrameTopLeft.Bounds.Width, ChoiceHeight),
          col);
      Renderer->DrawSprite(
          PlainSelectionFrameBottomLeft,
          glm::vec2(ChoiceXMin - PlainSelectionFrameTopLeft.Bounds.Width,
                    Choices[0][0].DestRect.Y + ChoiceHeight),
          col);
      Renderer->DrawSprite(PlainSelectionFrameBottomRight,
                           glm::vec2(ChoiceXMin + ChoiceWidthMax,
                                     Choices[0][0].DestRect.Y + ChoiceHeight),
                           col);
      Renderer->DrawSprite(
          PlainSelectionFrameBottomSide,
          RectF(ChoiceXMin, Choices[0][0].DestRect.Y + ChoiceHeight,
                ChoiceWidthMax, PlainSelectionFrameTopLeft.Bounds.Height),
          col);
      Renderer->DrawSprite(PlainSelectionFrameMiddle,
                           RectF(ChoiceXMin, Choices[0][0].DestRect.Y,
                                 ChoiceWidthMax, ChoiceHeight),
                           col);
    } else {
      for (int i = 0; i < ChoiceCount; i++) {
        Renderer->DrawSprite(
            SelectionBackground,
            glm::vec2(SelectionBackgroundX,
                      CurrentSelBackgroundY + (i * SelectionYSpacing)),
            col);
      }
    }

    ChoiceItems->Tint = col;
    ChoiceItems->Render();
  }
}

}  // namespace UI
}  // namespace Impacto