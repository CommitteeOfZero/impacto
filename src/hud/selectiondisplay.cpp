#include "selectiondisplay.h"

#include "../impacto.h"
#include "../renderer2d.h"
#include "../inputsystem.h"
#include "../profile/hud/selectiondisplay.h"
#include "../profile/dialogue.h"
#include "../profile/game.h"
#include "../vm/interface/input.h"

namespace Impacto {
namespace SelectionDisplay {

using namespace Impacto::Profile::SelectionDisplay;

SelectionDisplayState State;

Animation FadeAnimation;
bool IsPlain;
int ChoiceCount;
bool ChoiceMade = false;
int CurrentChoice = -1;
ProcessedTextGlyph Choices[15][255];  // The official implementation limit is
                                      // actually 60, but... really? 60 choices?
int ChoiceLengths[15];
float ChoiceWidths[15];

float CurrentSelBackgroundY = 0.0f;
float ChoiceHeight = 0.0f;
float ChoiceWidthMax = FLT_MIN;
float ChoiceXMin = FLT_MAX;

void Init() { Configure(); }

void Show() {
  State = Shown;
  ChoiceHeight = 0.0f;
  ChoiceWidthMax = FLT_MIN;
  ChoiceXMin = FLT_MAX;
  FadeAnimation.DurationIn = FadeAnimationDurationInOut;
  FadeAnimation.DurationOut = FadeAnimationDurationInOut;
  FadeAnimation.StartIn();

  if (IsPlain) {
    float diff = 0.0f;
    float choiceY = (Profile::DesignHeight -
                     (ChoiceCount * (Profile::Dialogue::DefaultFontSize +
                                     PlainSelectionYSpacing))) /
                    2.0f;
    for (int i = 0; i < ChoiceCount; i++) {
      if (ChoiceWidths[i] > ChoiceWidthMax) ChoiceWidthMax = ChoiceWidths[i];
      diff = (Profile::DesignWidth - ChoiceWidths[i]) / 2.0f;
      for (int j = 0; j < ChoiceLengths[i]; j++) {
        Choices[i][j].DestRect.X += diff;
        Choices[i][j].DestRect.Y = choiceY;
      }
      if (Choices[i][0].DestRect.X < ChoiceXMin)
        ChoiceXMin = Choices[i][0].DestRect.X;
      ChoiceHeight +=
          Profile::Dialogue::DefaultFontSize + PlainSelectionYSpacing;
      choiceY += Profile::Dialogue::DefaultFontSize + PlainSelectionYSpacing;
    }
    ChoiceHeight -= PlainSelectionYSpacing;
  } else {
    float diff = 0.0f;
    float choiceY = 0.0f;
    switch (ChoiceCount) {
      case 1:
        CurrentSelBackgroundY = SelectionBackgroundY1;
        choiceY = SelectionBackgroundY1 + (SelectionBackground.Bounds.Height -
                                           Profile::Dialogue::DefaultFontSize) /
                                              2.0f;
        break;
      case 2:
        CurrentSelBackgroundY = SelectionBackgroundY2;
        choiceY = SelectionBackgroundY2 + (SelectionBackground.Bounds.Height -
                                           Profile::Dialogue::DefaultFontSize) /
                                              2.0f;
        break;
      case 3:
        CurrentSelBackgroundY = SelectionBackgroundY3;
        choiceY = SelectionBackgroundY3 + (SelectionBackground.Bounds.Height -
                                           Profile::Dialogue::DefaultFontSize) /
                                              2.0f;
        break;
      case 4:
        CurrentSelBackgroundY = SelectionBackgroundY4;
        choiceY = SelectionBackgroundY4 + (SelectionBackground.Bounds.Height -
                                           Profile::Dialogue::DefaultFontSize) /
                                              2.0f;
        break;
      case 5:
        CurrentSelBackgroundY = SelectionBackgroundY5;
        choiceY = SelectionBackgroundY5 + (SelectionBackground.Bounds.Height -
                                           Profile::Dialogue::DefaultFontSize) /
                                              2.0f;
        break;
    }
    for (int i = 0; i < ChoiceCount; i++) {
      diff = (Profile::DesignWidth - ChoiceWidths[i]) / 2.0f;
      for (int j = 0; j < ChoiceLengths[i]; j++) {
        Choices[i][j].DestRect.X += diff;
        Choices[i][j].DestRect.Y = choiceY;
      }
      choiceY += SelectionYSpacing;
    }
  }
}
void Hide() {
  State = Hiding;
  FadeAnimation.StartOut();
  ChoiceMade = false;
}

void Update(float dt) {
  FadeAnimation.Update(dt);
  if (FadeAnimation.IsOut()) State = Hidden;
  if (State == Shown) {
    if (Input::KeyboardButtonWentDown[SDL_SCANCODE_DOWN]) {
      CurrentChoice++;
      if (CurrentChoice > ChoiceCount - 1) CurrentChoice = 0;
    } else if (Input::KeyboardButtonWentDown[SDL_SCANCODE_UP]) {
      CurrentChoice--;
      if (CurrentChoice < 0) CurrentChoice = ChoiceCount - 1;
    } else if (Vm::Interface::PAD1A & Vm::Interface::PADinputWentDown) {
      ChoiceMade = true;
    }
  }
}

void Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    if (IsPlain) {
      Renderer2D::DrawSprite(
          PlainSelectionFrameTopLeft,
          glm::vec2(ChoiceXMin - PlainSelectionFrameTopLeft.Bounds.Width,
                    Choices[0][0].DestRect.Y -
                        PlainSelectionFrameTopLeft.Bounds.Height),
          col);
      Renderer2D::DrawSprite(
          PlainSelectionFrameTopSide,
          RectF(ChoiceXMin,
                Choices[0][0].DestRect.Y -
                    PlainSelectionFrameTopLeft.Bounds.Height,
                ChoiceWidthMax, PlainSelectionFrameTopLeft.Bounds.Height),
          col);
      Renderer2D::DrawSprite(
          PlainSelectionFrameTopRight,
          glm::vec2(ChoiceXMin + ChoiceWidthMax,
                    Choices[0][0].DestRect.Y -
                        PlainSelectionFrameTopLeft.Bounds.Height),
          col);
      Renderer2D::DrawSprite(
          PlainSelectionFrameLeftSide,
          RectF(ChoiceXMin - PlainSelectionFrameTopLeft.Bounds.Width,
                Choices[0][0].DestRect.Y,
                PlainSelectionFrameTopLeft.Bounds.Width, ChoiceHeight),
          col);
      Renderer2D::DrawSprite(
          PlainSelectionFrameRightSide,
          RectF(ChoiceXMin + ChoiceWidthMax, Choices[0][0].DestRect.Y,
                PlainSelectionFrameTopLeft.Bounds.Width, ChoiceHeight),
          col);
      Renderer2D::DrawSprite(
          PlainSelectionFrameBottomLeft,
          glm::vec2(ChoiceXMin - PlainSelectionFrameTopLeft.Bounds.Width,
                    Choices[0][0].DestRect.Y + ChoiceHeight),
          col);
      Renderer2D::DrawSprite(PlainSelectionFrameBottomRight,
                             glm::vec2(ChoiceXMin + ChoiceWidthMax,
                                       Choices[0][0].DestRect.Y + ChoiceHeight),
                             col);
      Renderer2D::DrawSprite(
          PlainSelectionFrameBottomSide,
          RectF(ChoiceXMin, Choices[0][0].DestRect.Y + ChoiceHeight,
                ChoiceWidthMax, PlainSelectionFrameTopLeft.Bounds.Height),
          col);
      Renderer2D::DrawSprite(PlainSelectionFrameMiddle,
                             RectF(ChoiceXMin, Choices[0][0].DestRect.Y,
                                   ChoiceWidthMax, ChoiceHeight),
                             col);

      for (int i = 0; i < ChoiceCount; i++) {
        if (i == CurrentChoice) {
          Renderer2D::DrawSprite(
              SelectionHighlight,
              RectF(Choices[i][0].DestRect.X, Choices[i][0].DestRect.Y,
                    ChoiceWidths[i], Profile::Dialogue::DefaultFontSize));
        }
        Renderer2D::DrawProcessedText(Choices[i], ChoiceLengths[i],
                                      Profile::Dialogue::DialogueFont,
                                      FadeAnimation.Progress, true);
      }
    } else {
      for (int i = 0; i < ChoiceCount; i++) {
        Renderer2D::DrawSprite(
            SelectionBackground,
            glm::vec2(SelectionBackgroundX,
                      CurrentSelBackgroundY + (i * SelectionYSpacing)),
            col);
        if (i == CurrentChoice) {
          Renderer2D::DrawSprite(
              SelectionHighlight,
              RectF(Choices[i][0].DestRect.X, Choices[i][0].DestRect.Y,
                    ChoiceWidths[i], Profile::Dialogue::DefaultFontSize));
        }
        Renderer2D::DrawProcessedText(Choices[i], ChoiceLengths[i],
                                      Profile::Dialogue::DialogueFont,
                                      FadeAnimation.Progress, true);
      }
    }
  }
}

}  // namespace SelectionDisplay
}  // namespace Impacto