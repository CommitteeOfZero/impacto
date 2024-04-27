#include "sysmesbox.h"

#include "../../profile/ui/sysmesbox.h"
#include "../../profile/games/chlcc/sysmesbox.h"
#include "../../profile/dialogue.h"
#include "../../profile/scriptvars.h"
#include "../../mem.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::UI::Widgets;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::SysMesBox;
using namespace Impacto::Profile::CHLCC::SysMesBox;

void SysMesBox::ChoiceItemOnClick(Button* target) {
  ScrWork[SW_SYSSEL] = target->Id;
  ChoiceMade = true;
}

void SysMesBox::Show() {
  MessageItems = new Widgets::Group(this);
  ChoiceItems = new Widgets::Group(this);

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  auto onClick =
      std::bind(&SysMesBox::ChoiceItemOnClick, this, std::placeholders::_1);

  float diff = 0.0f;
  float maxWidth = FLT_MIN;
  for (int i = 0; i < MessageCount; i++) {
    if (maxWidth < MessageWidths[i]) maxWidth = MessageWidths[i];
  }
  if (maxWidth < MinHighlightWidth) maxWidth = MinHighlightWidth;

  for (int i = 0; i < MessageCount; i++) {
    diff = Messages[i][0].DestRect.X - (TextX - (maxWidth / 2.0f));
    for (int j = 0; j < MessageLengths[i]; j++) {
      Messages[i][j].Colors = Profile::Dialogue::ColorTable[0];
      Messages[i][j].DestRect.X -= diff;
      Messages[i][j].DestRect.Y = TextMiddleY + (i * TextLineHeight);
    }

    Label* message = new Label(Messages[i], MessageLengths[i], MessageWidths[i],
                               TextFontSize, RO_Full);

    MessageItems->Add(message, FDIR_DOWN);
  }

  float totalChoiceWidth = 0.0f;
  for (int i = 0; i < ChoiceCount; i++) {
    totalChoiceWidth += ChoiceWidths[i] + ChoicePadding;
  }
  if (maxWidth < totalChoiceWidth) maxWidth = totalChoiceWidth;
  if (maxWidth < MinMaxMesWidth) maxWidth = MinMaxMesWidth;

  ChoiceX = (maxWidth / 2.0f) - totalChoiceWidth + ChoiceXBase;

  float tempChoiceX = ChoiceX;

  for (int i = 0; i < ChoiceCount; i++) {
    diff = Choices[i][0].DestRect.X - tempChoiceX;
    for (int j = 0; j < ChoiceLengths[i]; j++) {
      Choices[i][j].Colors = Profile::Dialogue::ColorTable[0];
      Choices[i][j].DestRect.X -= diff;
      Choices[i][j].DestRect.Y = ChoiceY;
    }

    Button* choice = new Button(
        i, nullSprite, nullSprite, SelectionLeftPart,
        glm::vec2(Choices[i][0].DestRect.X, Choices[i][0].DestRect.Y));
    choice->HighlightOffset = glm::vec2(HighlightXOffset, HighlightYOffset);

    choice->SetText(Choices[i], ChoiceLengths[i], ChoiceWidths[i],
                    Profile::Dialogue::DefaultFontSize, RO_Full);
    choice->OnClickHandler = onClick;

    ChoiceItems->Add(choice, FDIR_LEFT);

    tempChoiceX += ChoiceWidths[i] + ChoicePadding;
  }

  FadeAnimation.StartIn();
  MessageItems->Show();
  MessageItems->HasFocus = false;
  if (ChoiceCount != 0) ChoiceItems->Show();
  State = Showing;

  if (UI::FocusedMenu != 0) {
    LastFocusedMenu = UI::FocusedMenu;
    LastFocusedMenu->IsFocused = false;
  }
  IsFocused = true;
  UI::FocusedMenu = this;
}

void SysMesBox::Hide() {
  FadeAnimation.StartOut();
  State = Hiding;
  if (LastFocusedMenu != 0) {
    UI::FocusedMenu = LastFocusedMenu;
    LastFocusedMenu->IsFocused = true;
  } else {
    UI::FocusedMenu = 0;
  }
  IsFocused = false;
}

void SysMesBox::Update(float dt) {
  UpdateInput();

  FadeAnimation.Update(dt);
  if (State != Hidden) {
    if (FadeAnimation.IsIn()) State = Shown;
    if (FadeAnimation.IsOut()) State = Hidden;

    if (IsFocused) {
      MessageItems->Update(dt);
      ChoiceItems->Update(dt);
    }
  }
}

void SysMesBox::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);

    float maxWidth = FLT_MIN;
    for (int i = 0; i < MessageCount; i++) {
      if (maxWidth < MessageWidths[i]) maxWidth = MessageWidths[i];
    }
    if (maxWidth < MinMaxMesWidth) maxWidth = MinMaxMesWidth;

    Renderer->DrawSprite(Box, glm::vec2(BoxX, BoxY), col);

    MessageItems->Tint.a = FadeAnimation.Progress;
    MessageItems->Render();
    ChoiceItems->Tint.a = FadeAnimation.Progress;
    ChoiceItems->Render();
  }
}

void SysMesBox::Init() {
  ChoiceMade = false;
  MessageCount = 0;
  ChoiceCount = 0;

  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;

  memset(Messages, 0, (8 * 255) * sizeof(ProcessedTextGlyph));
  memset(Choices, 0, (8 * 255) * sizeof(ProcessedTextGlyph));
}

void SysMesBox::AddMessage(uint8_t* str) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  int len = TextLayoutPlainLine(&dummy, 255, Messages[MessageCount],
                                Profile::Dialogue::DialogueFont, TextFontSize,
                                Profile::Dialogue::ColorTable[10], 1.0f,
                                glm::vec2(TextX, 0.0f), TextAlignment::Left);
  float mesLen = 0.0f;
  for (int i = 0; i < len; i++) {
    mesLen += Messages[MessageCount][i].DestRect.Width;
  }
  MessageWidths[MessageCount] = mesLen;
  MessageLengths[MessageCount] = len;
  MessageCount++;
}

void SysMesBox::AddChoice(uint8_t* str) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  int len = TextLayoutPlainLine(&dummy, 255, Choices[ChoiceCount],
                                Profile::Dialogue::DialogueFont, TextFontSize,
                                Profile::Dialogue::ColorTable[10], 1.0f,
                                glm::vec2(TextX, 0.0f), TextAlignment::Left);
  float mesLen = 0.0f;
  for (int i = 0; i < len; i++) {
    mesLen += Choices[ChoiceCount][i].DestRect.Width;
  }
  ChoiceWidths[ChoiceCount] = mesLen;
  ChoiceLengths[ChoiceCount] = len;
  ChoiceCount++;
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto