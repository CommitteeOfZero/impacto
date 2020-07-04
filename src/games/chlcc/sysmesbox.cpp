//#include "sysmesbox.h"
//
//#include "../../impacto.h"
//#include "../../renderer2d.h"
//#include "../../game.h"
//#include "../../mem.h"
//#include "../../inputsystem.h"
//#include "../../vm/interface/input.h"
//#include "../../profile/scriptvars.h"
//#include "../../profile/ui/sysmesbox.h"
//#include "../../profile/games/chlcc/sysmesbox.h"
//#include "../../profile/dialogue.h"
//
// namespace Impacto {
// namespace CHLCC {
//
// using namespace Impacto::Profile::SysMesBox;
// using namespace Impacto::Profile::CHLCC::SysMesBox;
// using namespace Impacto::Profile::ScriptVars;
//
// static float BoxAnimCount = 0.0f;
// static float BoxTopY = 0.0f;
// static float BoxBottomY = 0.0f;
// static float LineLength = 0.0f;
// static float BoxHeight = 0.0f;
// static float BoxProgressCount = 0.0f;
// static int TextStartCount = 0;
//
// void SysMesBox::Show() {
//  State = Showing;
//
//  // Set up so we don't have to do this each frame
//  float diff = 0.0f;
//  float maxWidth = FLT_MIN;
//  for (int i = 0; i < MessageCount; i++) {
//    if (maxWidth < MessageWidths[i]) maxWidth = MessageWidths[i];
//  }
//
//  float totalChoiceWidth = 0.0f;
//  for (int i = 0; i < ChoiceCount; i++) {
//    totalChoiceWidth += ChoiceWidths[i] + ChoicePadding;
//  }
//  if (maxWidth < totalChoiceWidth) maxWidth = totalChoiceWidth;
//  if (maxWidth < MinMaxMesWidth) maxWidth = MinMaxMesWidth;
//
//  ChoiceX = (maxWidth / 2.0f) - totalChoiceWidth + ChoiceXBase;
//
//  for (int i = 0; i < MessageCount; i++) {
//    diff = Messages[i][0].DestRect.X - (TextX - (maxWidth / 2.0f));
//    for (int j = 0; j < MessageLengths[i]; j++) {
//      Messages[i][j].Colors = Profile::Dialogue::ColorTable[0];
//      Messages[i][j].DestRect.X -= diff;
//      Messages[i][j].DestRect.Y = TextMiddleY + (i * TextLineHeight);
//    }
//  }
//
//  float tempChoiceX = ChoiceX;
//
//  for (int i = 0; i < ChoiceCount; i++) {
//    diff = Choices[i][0].DestRect.X - tempChoiceX;
//    for (int j = 0; j < ChoiceLengths[i]; j++) {
//      Choices[i][j].Colors = Profile::Dialogue::ColorTable[0];
//      Choices[i][j].DestRect.X -= diff;
//      Choices[i][j].DestRect.Y = ChoiceY;
//    }
//    tempChoiceX += ChoiceWidths[i] + ChoicePadding;
//  }
//}
// void SysMesBox::Hide() { State = Hiding; }
//
// void SysMesBox::Update(float dt) {
//  FadeAnimation.Update(dt);
//
//  if (State == Hiding) {
//    BoxAnimCount -= AnimationSpeed * dt;
//    if (BoxAnimCount <= 0.0f) {
//      BoxAnimCount = 0.0f;
//      State = Hidden;
//    }
//  } else if (State == Showing) {
//    BoxAnimCount += AnimationSpeed * dt;
//    if (BoxAnimCount >= ScrWork[SW_SYSMESANIMCTF]) {
//      BoxAnimCount = ScrWork[SW_SYSMESANIMCTF];
//      State = Shown;
//    }
//  }
//
//  ScrWork[SW_SYSMESANIMCTCUR] = std::floor(BoxAnimCount);
//
//  if (State == Shown) {
//    // Nice input
//    if (Input::KeyboardButtonWentDown[SDL_SCANCODE_RIGHT]) {
//      if (CurrentChoice == 255)
//        CurrentChoice = 1;
//      else {
//        CurrentChoice++;
//        if (CurrentChoice > 1) CurrentChoice = 0;
//      }
//    } else if (Input::KeyboardButtonWentDown[SDL_SCANCODE_LEFT]) {
//      if (CurrentChoice == 255)
//        CurrentChoice = 0;
//      else {
//        CurrentChoice--;
//        if (CurrentChoice < 0) CurrentChoice = 1;
//      }
//    } else if (Vm::Interface::PAD1A & Vm::Interface::PADinputButtonWentDown) {
//      ChoiceMade = true;
//    }
//  }
//
//  if (State != Hidden) {
//    if (State == Showing && FadeAnimation.IsOut())
//      FadeAnimation.StartIn();
//    else if (State == Hiding && FadeAnimation.IsIn())
//      FadeAnimation.StartOut();
//
//    if (CurrentChoice != 255 && ChoiceCount > 0) {
//      HighlightX = ChoiceX;
//      for (int i = 0; i < CurrentChoice; i++) {
//        HighlightX += ChoiceWidths[i] + ChoicePadding;
//      }
//      SelectedWidth = ChoiceWidths[CurrentChoice];
//      SelectedWidthAdjusted = SelectedWidth + HighlightRightPartSpriteWidth;
//      if (SelectedWidthAdjusted < MinHighlightWidth) {
//        SelectedWidth = MinHighlightWidth;
//        SelectedWidthAdjusted = SelectedWidth;
//      }
//    }
//  }
//}
//
// void SysMesBox::Render() {
//  if (BoxOpacity) {
//    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
//    Renderer2D::DrawSprite(Box, glm::vec2(BoxX, BoxY), col);
//
//    if (CurrentChoice != 255 && ChoiceCount > 0) {
//      if (SelectedWidth < HighlightBaseWidth) {
//        RectF prevBounds = SelectionLeftPart.Bounds;
//        SelectionLeftPart.Bounds =
//            RectF(SelectionLeftPart.Bounds.X, SelectionLeftPart.Bounds.Y,
//                  SelectedWidth, SelectionLeftPart.Bounds.Height);
//        Renderer2D::DrawSprite(SelectionLeftPart,
//                               glm::vec2(HighlightX - HighlightXOffset,
//                                         ChoiceY - HighlightYOffset),
//                               col);
//        SelectionLeftPart.Bounds = prevBounds;
//        Renderer2D::DrawSprite(
//            SelectionRightPart,
//            glm::vec2(HighlightX + SelectedWidth - HighlightXOffset,
//                      ChoiceY - HighlightYOffset),
//            col);
//      } else {
//        Renderer2D::DrawSprite(SelectionLeftPart,
//                               glm::vec2(HighlightX - HighlightXOffset,
//                                         ChoiceY - HighlightYOffset),
//                               col);
//        SelectedWidthAdjusted -= HighlightBaseWidth;
//        HighlightX += HighlightXStep + 1.0f;
//        while (SelectedWidthAdjusted > HighlightBaseWidth) {
//          Renderer2D::DrawSprite(
//              SelectionMiddlePart,
//              glm::vec2(HighlightX, ChoiceY - HighlightYOffset), col);
//          HighlightX += HighlightXStep;
//          SelectedWidthAdjusted -= HighlightXStep;
//        }
//        RectF prevBounds = SelectionRightPart.Bounds;
//        SelectionRightPart.Bounds =
//            RectF(HighlightXBase - SelectedWidthAdjusted - 1.0f,
//                  SelectionRightPart.Bounds.Y, SelectedWidthAdjusted,
//                  SelectionRightPart.Bounds.Height);
//        Renderer2D::DrawSprite(
//            SelectionRightPart,
//            glm::vec2(HighlightX, ChoiceY - HighlightYOffset), col);
//        SelectionRightPart.Bounds = prevBounds;
//      }
//    }
//
//    for (int i = 0; i < MessageCount; i++) {
//      Renderer2D::DrawProcessedText(Messages[i], MessageLengths[i],
//                                    Profile::Dialogue::DialogueFont, col.a,
//                                    true);
//    }
//
//    for (int i = 0; i < ChoiceCount; i++) {
//      Renderer2D::DrawProcessedText(Choices[i], ChoiceLengths[i],
//                                    Profile::Dialogue::DialogueFont, col.a,
//                                    true);
//    }
//  }
//}
//
//}  // namespace CHLCC
//}  // namespace Impacto